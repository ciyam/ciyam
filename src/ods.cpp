// Copyright (c) 2000-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <map>
#  include <set>
#  include <stack>
#  include <vector>
#  include <memory>
#  include <fstream>
#  include <iomanip>
#  include <sstream>
#  include <iostream>
#  include <algorithm>
#  include <ctime>
#  include <cerrno>
#  include <cstdio>
#  ifdef __GNUG__
#     include <fcntl.h>
#     include <unistd.h>
#     include <sys/time.h>
#  endif
#  include <sys/stat.h>
#endif

#ifdef __GNUG__
#  define _open open
#  define _read ::read
#  define _write ::write
#  define _close close
#  define _lseek lseek
#endif

#include "ods.h"
#include "cache.h"
#include "format.h"
#include "macros.h"
#include "sha256.h"
#include "pointers.h"
#include "progress.h"
#include "date_time.h"
#include "utilities.h"
#include "fs_iterator.h"
#include "crypt_stream.h"
#include "read_write_stream.h"

//#define ODS_DEBUG

using namespace std;
using namespace cache;

#ifndef O_BINARY
#  define O_BINARY 0
#endif

#ifdef __GNUG__
#  define ODS_DEFAULT_PERMS S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH
#endif

namespace
{

const char* const c_ops_file_name_ext = ".ops";
const char* const c_data_file_name_ext = ".dat";
const char* const c_index_file_name_ext = ".idx";
const char* const c_header_file_name_ext = ".hdr";
const char* const c_tranlog_file_name_ext = ".tlg";

const char* const c_sav_file_name_ext = ".sav";
const char* const c_lock_file_name_ext = ".lck";

const int c_review_max_attempts = 50;
const int c_review_attempt_sleep_time = 100;

const int c_update_max_attempts = 20;
const int c_update_attempt_sleep_time = 150;

const int c_delete_max_attempts = 10;
const int c_delete_attempt_sleep_time = 250;

const int c_bulk_pause_sleep_time = 250;

const int c_bulk_dump_max_attempts = 10;
const int c_bulk_dump_attempt_sleep_time = 100;

const int c_bulk_read_max_attempts = 25;
const int c_bulk_read_attempt_sleep_time = 100;

const int c_bulk_write_max_attempts = 50;
const int c_bulk_write_attempt_sleep_time = 100;

const int c_data_lock_max_attempts = 10;
const int c_data_lock_attempt_sleep_time = 5;

const int c_header_lock_max_attempts = 100;
const int c_header_lock_attempt_sleep_time = 25;

const int c_data_bytes_per_item = c_ods_page_size;
const int c_data_max_cache_items = 5000;
const int c_data_items_per_region = 50000;
const int c_data_num_cache_regions = 10;

const int c_index_items_per_item = 128;
const int c_index_max_cache_items = 1000;
const int c_index_items_per_region = 10000;
const int c_index_num_cache_regions = 10;

const int c_trans_op_max_cache_items = 1000;
const int c_trans_op_items_per_region = 10000;
const int c_trans_data_max_cache_items = 5000;
const int c_trans_data_items_per_region = 50000;

mutex g_ods_lock;

#ifdef ODS_DEBUG
mutex g_debug_lock;

void debug_log( const string& msg )
{
   static bool is_first = true;
   guard lock_debug( g_debug_lock );

   ios::openmode oflags = ios::out;
   if( is_first )
      is_first = false;
   else
      oflags |= ios::app;

   ofstream outf( "debug.log", oflags );
   outf << msg << endl;
   outf.close( );
}
#  define DEBUG_LOG( msg ) debug_log( msg )
#else
#  define DEBUG_LOG( msg )
#endif

void throw_ods_error( const string& s )
{
#ifdef ODS_DEBUG
   DEBUG_LOG( "================ ods exception was thrown ================" );
   DEBUG_LOG( s );
   DEBUG_LOG( "==========================================================" );
#endif
   throw runtime_error( s );
}

#define THROW_ODS_ERROR( s ) throw_ods_error( s )

// NOTE: It would be expected that if a "major" version change has occurred then any previous file
// format would no longer be compatible, however, if a "minor" version change has occurred then it
// should still be possible to operate on previous file formats (with the same "major" version).
const int16_t c_major_ver = 1;
const int16_t c_minor_ver = 1;

const int64_t c_prior_minor_ver = 0;

const int16_t c_version = ( c_major_ver << 8 ) | c_minor_ver;
const int16_t c_prior_version = ( c_major_ver << 8 ) | c_prior_minor_ver;

const int16_t c_version_mask = 0x7fff;
const int16_t c_encrypted_flag = 0x8000;

const int16_t c_version_major_mask = 0x7f00;
const int16_t c_version_minor_mask = 0x00ff;

const int16_t c_stream_cipher_mask = 0x0003; // i.e. lower two bits of the minor version

const int64_t c_bit_1 = UINT64_C( 1 );
const int64_t c_bit_2 = UINT64_C( 2 );

const uint64_t c_int_type_hi_bit = UINT64_C( 1 ) << ( numeric_limits< uint64_t >::digits - 1 );

const uint64_t c_int_type_low_bits = UINT64_C( -1 ) >> ( numeric_limits< uint64_t >::digits / 2 );
const uint64_t c_int_type_high_bits = c_int_type_low_bits << ( numeric_limits< uint64_t >::digits / 2 );

const int c_buffer_chunk_size = c_ods_page_size;

inline bool is_print( char ch ) { return ch >= ' ' && ch <= '~'; }

string format_version( int16_t version )
{
   ostringstream osstr;
   osstr << ( ( version & c_version_major_mask ) >> 8 ) << '.' << ( version & c_version_minor_mask );

   return osstr.str( );
}

void check_version( int16_t version )
{
   bool valid = false;

   if( ( version & c_version_mask ) == c_version )
      valid = true;
   else if( ( version & c_version_mask ) == c_prior_version )
      valid = true;

   if( !valid )
      THROW_ODS_ERROR( "incompatible ODS version found" );
}

stream_cipher determine_stream_cipher( int16_t version )
{
   stream_cipher cipher = e_stream_cipher_unknown;

   switch( version & c_stream_cipher_mask )
   {
      case 1:
      cipher = e_stream_cipher_chacha20;
      break;

      case 2:
      cipher = e_stream_cipher_dbl_hash;
      break;

      case 3:
      // FUTURE: Available when necessary.
      break;
   }

   return cipher;
}

class log_stream : public read_write_stream
{
   public:
   log_stream( const char* p_file_name = 0, bool use_sync = false )
    :
    fd( 0 ),
    pos( 0 ),
    use_sync( use_sync )
   {
      if( p_file_name )
         init( p_file_name );
   }

   ~log_stream( )
   {
      term( );
   }

   void init( const char* p_file_name )
   {
#ifdef __GNUG__
      if( !use_sync )
         fd = _open( p_file_name, O_RDWR | O_CREAT, ODS_DEFAULT_PERMS );
      else
         fd = _open( p_file_name, O_RDWR | O_CREAT | O_SYNC, ODS_DEFAULT_PERMS );
#else
      fd = _sopen( p_file_name, O_BINARY | _O_RDWR | _O_CREAT, SH_DENYWR, S_IREAD | S_IWRITE );
#endif
      if( fd <= 0 )
         THROW_ODS_ERROR( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
   }

   void term( )
   {
#ifndef ODS_DEBUG
      if( fd )
      {
         _close( fd );
         fd = 0;
      }
#else
      if( fd && _close( fd ) != 0 )
      {
         ostringstream osstr;

         osstr << "_close failed for fd in term (errno = " << errno << ')';

         DEBUG_LOG( osstr.str( ) );
      }
#endif
   }

   int64_t get_pos( ) const
   {
      return pos;
   }

   void set_pos( int64_t new_pos )
   {
      if( _lseek( fd, new_pos, SEEK_SET ) < 0 )
         THROW_ODS_ERROR( "unexpected seek at " STRINGIZE( __LINE__ ) " failed" );

      pos = new_pos;
   }

   void read( unsigned char* p_buf, size_t len, read_write_type type = e_read_write_type_none )
   {
      ( void )type;

      if( _read( fd, ( void* )p_buf, len ) != len )
         THROW_ODS_ERROR( "unexpected read at " STRINGIZE( __LINE__ ) " failed" );

      pos += len;
   }

   void write( const unsigned char* p_buf, size_t len, read_write_type type = e_read_write_type_none )
   {
      ( void )type;

      if( _write( fd, ( void* )p_buf, len ) != len )
         THROW_ODS_ERROR( "unexpected write at " STRINGIZE( __LINE__ ) " failed" );

      pos += len;
   }

   private:
   int fd;
   int64_t pos;

   bool use_sync;
};

struct log_info
{
   log_info( )
    :
    version( c_version ),
    sequence( 1 ),
    val_hash( 0 ),
    init_time( 0 ),
    entry_offs( 0 ),
    entry_time( 0 ),
    append_offs( size_of( ) ),
    sequence_new_tm( 0 ),
    sequence_old_tm( 0 )
   {
   }

   void dump( ostream& os, bool omit_dtms = false ) const
   {
      string extra;

      if( version & c_encrypted_flag )
         extra += " (encrypted)";

      os << "version = " << format_version( version & c_version_mask ) << extra << '\n';
      os << "sequence = " << sequence << '\n';

      if( version & c_encrypted_flag )
         os << "val_hash = " << val_hash << '\n';

      if( !omit_dtms )
         os << "init_time = " << init_time << '\n';

      os << "entry_offs = " << entry_offs << '\n';

      if( !omit_dtms )
         os << "entry_time = " << entry_time << '\n';

      os << "append_offs = " << append_offs << '\n';

      if( !omit_dtms )
      {
         os << "sequence_new_tm = " << sequence_new_tm << '\n';
         os << "sequence_old_tm = " << sequence_old_tm << endl;
      }
   }

   int64_t size_of( ) const
   {
      return sizeof( version ) + sizeof( sequence ) + sizeof( val_hash )
       + sizeof( init_time ) + sizeof( entry_offs ) + sizeof( entry_time )
       + sizeof( append_offs ) + sizeof( sequence_new_tm ) + sizeof( sequence_old_tm );
   }

   void read( istream& is )
   {
      is.read( ( char* )&version, sizeof( version ) );

      check_version( version );

      is.read( ( char* )&sequence, sizeof( sequence ) );
      is.read( ( char* )&val_hash, sizeof( val_hash ) );
      is.read( ( char* )&init_time, sizeof( init_time ) );
      is.read( ( char* )&entry_offs, sizeof( entry_offs ) );
      is.read( ( char* )&entry_time, sizeof( entry_time ) );
      is.read( ( char* )&append_offs, sizeof( append_offs ) );
      is.read( ( char* )&sequence_new_tm, sizeof( sequence_new_tm ) );
      is.read( ( char* )&sequence_old_tm, sizeof( sequence_old_tm ) );

      if( !is.good( ) )
         THROW_ODS_ERROR( "unexpected bad log_info read" );
   }

   void read( read_stream& rs )
   {
      rs.read( ( unsigned char* )&version, sizeof( version ) );

      check_version( version );

      rs.read( ( unsigned char* )&sequence, sizeof( sequence ) );
      rs.read( ( unsigned char* )&val_hash, sizeof( val_hash ) );
      rs.read( ( unsigned char* )&init_time, sizeof( init_time ) );
      rs.read( ( unsigned char* )&entry_offs, sizeof( entry_offs ) );
      rs.read( ( unsigned char* )&entry_time, sizeof( entry_time ) );
      rs.read( ( unsigned char* )&append_offs, sizeof( append_offs ) );
      rs.read( ( unsigned char* )&sequence_new_tm, sizeof( sequence_new_tm ) );
      rs.read( ( unsigned char* )&sequence_old_tm, sizeof( sequence_old_tm ) );
   }

   void write( write_stream& ws ) const
   {
      ws.write( ( const unsigned char* )&version, sizeof( version ) );
      ws.write( ( const unsigned char* )&sequence, sizeof( sequence ) );
      ws.write( ( const unsigned char* )&val_hash, sizeof( val_hash ) );
      ws.write( ( const unsigned char* )&init_time, sizeof( init_time ) );
      ws.write( ( const unsigned char* )&entry_offs, sizeof( entry_offs ) );
      ws.write( ( const unsigned char* )&entry_time, sizeof( entry_time ) );
      ws.write( ( const unsigned char* )&append_offs, sizeof( append_offs ) );
      ws.write( ( const unsigned char* )&sequence_new_tm, sizeof( sequence_new_tm ) );
      ws.write( ( const unsigned char* )&sequence_old_tm, sizeof( sequence_old_tm ) );
   }

   int16_t version;
   int16_t sequence;

   uint32_t val_hash;

   int64_t init_time;

   int64_t entry_offs;
   int64_t entry_time;

   int64_t append_offs;

   int64_t sequence_new_tm;
   int64_t sequence_old_tm;
};

const size_t c_max_tx_label_size = 15;

struct log_entry
{
   log_entry( )
    :
    tx_id( 0 ),
    tx_time( 0 ),
    commit_offs( 0 ),
    commit_items( 0 ),
    next_entry_offs( 0 ),
    prior_entry_offs( 0 ),
    total_data_bytes( 0 ),
    data_transform_id( 0 ),
    index_transform_id( 0 )
   {
      memset( label, '\0', sizeof( label ) );
   }

   void dump( ostream& os, int64_t offs = 0, bool omit_dtms = false ) const
   {
      if( label[ 0 ] )
         os << '[' << label << ']' << '\n';

      os << "tx_id = " << tx_id;
      if( offs )
          os << " (offs = " << offs << ")";
      os << '\n';

      if( !omit_dtms )
         os << "tx_time = " << tx_time << '\n';

      os << "commit_offs = " << commit_offs << '\n';
      os << "commit_items = " << commit_items << '\n';
      os << "next_entry_offs = " << next_entry_offs << '\n';
      os << "prior_entry_offs = " << prior_entry_offs << '\n';
      os << "total_data_bytes = " << total_data_bytes << '\n';
      os << "data_transform_id = " << data_transform_id << '\n';
      os << "index_transform_id = " << index_transform_id << endl;
   }

   int64_t size_of( ) const
   {
      return sizeof( label )
       + sizeof( tx_id ) + sizeof( tx_time )
       + sizeof( commit_offs ) + sizeof( commit_items )
       + sizeof( next_entry_offs ) + sizeof( prior_entry_offs )
       + sizeof( total_data_bytes ) + sizeof( data_transform_id ) + sizeof( index_transform_id );
   }

   void read( istream& is )
   {
      is.read( ( char* )label, sizeof( label ) );
      is.read( ( char* )&tx_id, sizeof( tx_id ) );
      is.read( ( char* )&tx_time, sizeof( tx_time ) );
      is.read( ( char* )&commit_offs, sizeof( commit_offs ) );
      is.read( ( char* )&commit_items, sizeof( commit_items ) );
      is.read( ( char* )&next_entry_offs, sizeof( next_entry_offs ) );
      is.read( ( char* )&prior_entry_offs, sizeof( prior_entry_offs ) );
      is.read( ( char* )&total_data_bytes, sizeof( total_data_bytes ) );
      is.read( ( char* )&data_transform_id, sizeof( data_transform_id ) );
      is.read( ( char* )&index_transform_id, sizeof( index_transform_id ) );

      if( !is.good( ) )
         THROW_ODS_ERROR( "unexpected bad log_entry read" );
   }

   void read( read_stream& rs )
   {
      rs.read( ( unsigned char* )label, sizeof( label ) );
      rs.read( ( unsigned char* )&tx_id, sizeof( tx_id ) );
      rs.read( ( unsigned char* )&tx_time, sizeof( tx_time ) );
      rs.read( ( unsigned char* )&commit_offs, sizeof( commit_offs ) );
      rs.read( ( unsigned char* )&commit_items, sizeof( commit_items ) );
      rs.read( ( unsigned char* )&next_entry_offs, sizeof( next_entry_offs ) );
      rs.read( ( unsigned char* )&prior_entry_offs, sizeof( prior_entry_offs ) );
      rs.read( ( unsigned char* )&total_data_bytes, sizeof( total_data_bytes ) );
      rs.read( ( unsigned char* )&data_transform_id, sizeof( data_transform_id ) );
      rs.read( ( unsigned char* )&index_transform_id, sizeof( index_transform_id ) );
   }

   void write( write_stream& ws ) const
   {
      ws.write( ( const unsigned char* )label, sizeof( label ) );
      ws.write( ( const unsigned char* )&tx_id, sizeof( tx_id ) );
      ws.write( ( const unsigned char* )&tx_time, sizeof( tx_time ) );
      ws.write( ( const unsigned char* )&commit_offs, sizeof( commit_offs ) );
      ws.write( ( const unsigned char* )&commit_items, sizeof( commit_items ) );
      ws.write( ( const unsigned char* )&next_entry_offs, sizeof( next_entry_offs ) );
      ws.write( ( const unsigned char* )&prior_entry_offs, sizeof( prior_entry_offs ) );
      ws.write( ( const unsigned char* )&total_data_bytes, sizeof( total_data_bytes ) );
      ws.write( ( const unsigned char* )&data_transform_id, sizeof( data_transform_id ) );
      ws.write( ( const unsigned char* )&index_transform_id, sizeof( index_transform_id ) );
   }

   char label[ c_max_tx_label_size + 1 ];

   int64_t tx_id;
   int64_t tx_time;

   int64_t commit_offs;
   int64_t commit_items;

   int64_t next_entry_offs;
   int64_t prior_entry_offs;

   int64_t total_data_bytes;
   int64_t data_transform_id;
   int64_t index_transform_id;
};

const unsigned char c_log_entry_item_op_store = 0;
const unsigned char c_log_entry_item_op_create = 1;
const unsigned char c_log_entry_item_op_update = 2;
const unsigned char c_log_entry_item_op_destroy = 3;

const unsigned char c_log_entry_item_mask_op = 0x03;

const unsigned char c_log_entry_item_type_normal = 0;
const unsigned char c_log_entry_item_type_nested_tx = 4;
const unsigned char c_log_entry_item_type_reserved_8 = 8;

const unsigned char c_log_entry_item_mask_type = 0x0c;

const unsigned char c_log_entry_item_flag_is_post_op = 0x10;
const unsigned char c_log_entry_item_flag_has_old_pos = 0x20;
const unsigned char c_log_entry_item_flag_reserved_40 = 0x40;
const unsigned char c_log_entry_item_flag_reserved_80 = 0x80;

struct log_entry_item
{
   log_entry_item( )
    :
    flags( 0 ),
    tx_oid( 0 ),
    data_pos( 0 ),
    data_opos( 0 ),
    data_size( 0 ),
    index_entry_id( 0 )
   {
   }

   bool is_store( ) const { return ( flags & c_log_entry_item_mask_op ) == c_log_entry_item_op_store; }
   bool is_create( ) const { return ( flags & c_log_entry_item_mask_op ) == c_log_entry_item_op_create; }
   bool is_destroy( ) const { return ( flags & c_log_entry_item_mask_op ) == c_log_entry_item_op_destroy; }

   bool is_post_op( ) const { return ( flags & c_log_entry_item_flag_is_post_op ); }

   bool is_nested_tx( ) const { return ( flags & c_log_entry_item_mask_type ) == c_log_entry_item_type_nested_tx; }

   bool has_old_tran_id( ) const
   {
      return !is_post_op( ) && !is_nested_tx( );
   }

   bool has_old_pos( ) const { return ( flags & c_log_entry_item_flag_has_old_pos ); }

   bool has_pos_and_size( ) const
   {
      unsigned char op = ( flags & c_log_entry_item_mask_op );

      bool is_post_op = ( flags & c_log_entry_item_flag_is_post_op );
      bool has_old_pos = ( flags & c_log_entry_item_flag_has_old_pos );

      bool has_data_pos_and_size = ( op == c_log_entry_item_op_update || op == c_log_entry_item_op_destroy );

      if( op == c_log_entry_item_op_store )
         has_data_pos_and_size = ( is_post_op || has_old_pos );
      else if( is_post_op && op == c_log_entry_item_op_destroy )
         has_data_pos_and_size = false;

      return has_data_pos_and_size;
   }

   void dump( ostream& os, int64_t offs = 0 ) const
   {
      os << "flags = " << ( int )flags;

      unsigned char op = ( flags & c_log_entry_item_mask_op );
      unsigned char type = ( flags & c_log_entry_item_mask_type );

      bool pos_and_size = has_pos_and_size( );

      if( type == c_log_entry_item_type_normal )
      {
         if( op == c_log_entry_item_op_store )
            os << " (store)";
         else if( op == c_log_entry_item_op_create )
            os << " (create)";
         else if( op == c_log_entry_item_op_update )
            os << " (update)";
         else
         {
            if( !is_post_op( ) )
               os << " (destroy)";
            else
               os << " (freelist)";
         }
      }
      else if( type == c_log_entry_item_type_nested_tx )
      {
         if( op == c_log_entry_item_op_store )
            os << " (nested start)";
         else if( op == c_log_entry_item_op_create )
            os << " (nested commit)";
         else
            os << " (nested rollback)";
      }

      if( offs )
         os << " offs = " << offs;

      os << '\n';

      if( type == c_log_entry_item_type_normal )
      {
         if( has_old_tran_id( ) )
            os << "tx_oid = " << tx_oid << '\n';

         if( pos_and_size )
            os << "data_pos = " << data_pos << '\n';

         if( has_old_pos( ) )
            os << "data_opos = " << data_opos << '\n';

         if( pos_and_size )
            os << "data_size = " << data_size << '\n';

         os << "index_entry_id = " << index_entry_id << endl;
      }
      else if( type == c_log_entry_item_type_nested_tx )
         os << "nested tx level = " << index_entry_id << endl;
   }

   void read( istream& is )
   {
      is.read( ( char* )&flags, sizeof( flags ) );

      bool pos_and_size = has_pos_and_size( );

      if( has_old_tran_id( ) )
         is.read( ( char* )&tx_oid, sizeof( tx_oid ) );

      if( pos_and_size )
         is.read( ( char* )&data_pos, sizeof( data_pos ) );

      if( has_old_pos( ) )
         is.read( ( char* )&data_opos, sizeof( data_opos ) );

      if( pos_and_size )
         is.read( ( char* )&data_size, sizeof( data_size ) );

      is.read( ( char* )&index_entry_id, sizeof( index_entry_id ) );

      if( !is.good( ) )
         THROW_ODS_ERROR( "unexpected bad log_entry_item read" );
   }

   void read( read_stream& rs )
   {
      rs.read( ( unsigned char* )&flags, sizeof( flags ) );

      bool pos_and_size = has_pos_and_size( );

      if( has_old_tran_id( ) )
         rs.read( ( unsigned char* )&tx_oid, sizeof( tx_oid ) );

      if( pos_and_size )
         rs.read( ( unsigned char* )&data_pos, sizeof( data_pos ) );

      if( has_old_pos( ) )
         rs.read( ( unsigned char* )&data_opos, sizeof( data_opos ) );

      if( pos_and_size )
         rs.read( ( unsigned char* )&data_size, sizeof( data_size ) );

      rs.read( ( unsigned char* )&index_entry_id, sizeof( index_entry_id ) );
   }

   void write( write_stream& ws ) const
   {
      ws.write( ( const unsigned char* )&flags, sizeof( flags ) );

      bool pos_and_size = has_pos_and_size( );

      if( has_old_tran_id( ) )
         ws.write( ( const unsigned char* )&tx_oid, sizeof( tx_oid ) );

      if( pos_and_size )
         ws.write( ( const unsigned char* )&data_pos, sizeof( data_pos ) );

      if( has_old_pos( ) )
         ws.write( ( const unsigned char* )&data_opos, sizeof( data_opos ) );

      if( pos_and_size )
         ws.write( ( const unsigned char* )&data_size, sizeof( data_size ) );

      ws.write( ( const unsigned char* )&index_entry_id, sizeof( index_entry_id ) );
   }

   unsigned char flags;

   int64_t tx_oid;
   int64_t data_pos;
   int64_t data_opos;
   int64_t data_size;
   int64_t index_entry_id;
};

void init_key_buffer( char* p_key_buf, size_t buf_len,
 int64_t num, const string& pwd_hash, stream_cipher cipher_type )
{
   string buf_str( buf_len, '\0' );
   string num_str( to_string( num ) );

   stringstream ss;

   // NOTE: Try using own buffer to avoid internal key pad copy.
   ss.rdbuf( )->pubsetbuf( ( char* )buf_str.data( ), buf_len );

   // NOTE: Creates a unique key pad for each buffer number.
   sha256 hash( combined_clear_key( pwd_hash, num_str ) );

   crypt_stream( ss, hash.get_digest_as_string( ), cipher_type );

   string key_pad( ss.str( ) );

   for( size_t i = 0; i < buf_len; i++ )
      *( p_key_buf + i ) = key_pad[ i ];

   clear_key( key_pad );
   clear_key( buf_str );
}

void crypt_data_buffer( char* p_buf, const char* p_key_buf, size_t buf_len )
{
   for( size_t i = 0; i < buf_len; i++ )
      *( p_buf + i ) ^= *( p_key_buf + i );
}

}

string ods_file_names( const string& name, char sep, bool include_tranlog )
{
   string retval( name + c_data_file_name_ext );

   retval += sep + name + c_index_file_name_ext;
   retval += sep + name + c_header_file_name_ext;

   if( include_tranlog )
      retval += sep + name + c_tranlog_file_name_ext;

   return retval;
}

string ods_backup_file_names( const string& name, const char* p_ext, char sep, bool include_tranlog )
{
   string retval;

   string ext( p_ext ? p_ext : c_sav_file_name_ext );

   retval = name + c_data_file_name_ext + ext;

   retval += sep + name + c_index_file_name_ext + ext;
   retval += sep + name + c_header_file_name_ext + ext;

   if( include_tranlog )
      retval += sep + name + c_tranlog_file_name_ext + ext;

   return retval;
}

void char_buffer::operator =( const char* s )
{
   guard lock_buffer( buffer_lock );

   int64_t n;
   for( n = 0; n < max - pos; n++ )
      if( ( p_data[ n ] = s[ n ] ) == '\0' )
         break;

   len = n;
}

void char_buffer::operator +=( const char* s )
{
   guard lock_buffer( buffer_lock );

   int64_t n, o;
   for( n = 0, o = len; o < max - pos; n++, o++ )
      if( ( p_data[ o ] = s[ n ] ) == '\0' )
         break;

   len = o;
}

bool char_buffer::is_equal_to( const char_buffer& src ) const
{
   guard lock_buffer( buffer_lock );

   if( len != src.len )
      return false;

   for( int64_t i = 0; i < len; i++ )
      if( src.p_data[ i ] != p_data[ i ] )
         return false;

   return true;
}

bool char_buffer::is_less_than( const char_buffer& src ) const
{
   guard lock_buffer( buffer_lock );

   int diff;
   int64_t length = src.len;

   for( int64_t i = 0; i < min( len, length ); i++ )
   {
      diff = src.p_data[ i ] - p_data[ i ];
      if( !diff )
         continue;
      return diff < 0 ? true : false;
   }

   return length < len;
}

int64_t char_buffer::first_of( const char* matches ) const
{
   guard lock_buffer( buffer_lock );

   for( int64_t i = 0; i < len; i++ )
   {
      for( const char* p = matches; *p != '\0'; ++p )
      {
         if( *p == p_data[ i ] )
            return i;
      }
   }

   return -1;
}

bool char_buffer::has_one_of( const char* matches ) const
{
   guard lock_buffer( buffer_lock );

   for( int64_t i = 0; i < len; i++ )
   {
      for( const char* p = matches; *p != '\0'; ++p )
      {
         if( *p == p_data[ i ] )
            return true;
      }
   }

   return false;
}

bool char_buffer::begins_with( const char_buffer& src, int64_t max ) const
{
   guard lock_buffer( buffer_lock );

   if( max == 0 )
      max = src.len;
   if( max > len )
      max = len;

   for( int64_t i = 0; i < max; ++i )
   {
      if( p_data[ i ] != src.p_data[ i ] )
         return false;
   }

   return true;
}

read_stream& operator >>( read_stream& rs, char_buffer& cb )
{
   guard lock_buffer( cb.buffer_lock );

   rs >> cb.len;
   rs.read( ( unsigned char* )cb.p_data, cb.len );
   return rs;
}

write_stream& operator <<( write_stream& ws, const char_buffer& cb )
{
   guard lock_buffer( cb.buffer_lock );

   ws << cb.len;
   ws.write( ( unsigned char* )cb.p_data, cb.len );
   return ws;
}

ostream& operator <<( ostream& outf, const char_buffer& c )
{
   guard lock_buffer( c.buffer_lock );

   for( int64_t s = 0; s < c.len; s++ )
      outf << c.p_data[ s ];

   return outf;
}

class ods::ods_stream : public read_write_stream
{
   public:
   explicit ods_stream( ods& o ) : o( o ) { }

   void read( unsigned char* p_buf, size_t len, read_write_type ) { o.read( p_buf, len ); }
   void write( const unsigned char* p_buf, size_t len, read_write_type ) { o.write( p_buf, len ); }

   void read_meta( string& s ) { s = o.get_meta( ); }
   void write_meta( string& s ) { s = o.get_meta( ); }

   private:
   ods& o;
};

read_stream& operator >>( read_stream& rs, oid& id )
{
   rs.read( ( unsigned char* )&id, sizeof( oid ) );
   return rs;
}

write_stream& operator <<( write_stream& ws, const oid& id )
{
   ws.write( ( unsigned char* )&id, sizeof( oid ) );
   return ws;
}

read_stream& operator >>( read_stream& rs, byte_skip& bs )
{
   rs.read( 0, bs.num );
   return rs;
}

write_stream& operator <<( write_stream& ws, const byte_skip& bs )
{
   ws.write( 0, bs.num );
   return ws;
}

template< typename T > class temp_set_value
{
   public:
   temp_set_value( T& instance, const T& temp_value )
    : instance( instance ), initial_value( instance )
   {
      instance = temp_value;
   }

   ~temp_set_value( )
   {
      instance = initial_value;
   }

   private:
   temp_set_value( const temp_set_value& );
   void operator =( const temp_set_value& );

   T& instance;
   T initial_value;
};

template< typename T > class finalise_value
{
   public:
   finalise_value( T& instance, const T& value )
    : value( value ), instance( instance )
   {
   }

   ~finalise_value( )
   {
      instance = value;
   }

   T value;
   T& instance;
};

struct header_info
{
   int16_t version;
   int16_t num_logs;

   int16_t num_trans;
   int16_t num_writers;

   int64_t init_tranlog;
   int64_t total_entries;
   int64_t tranlog_offset;

   int64_t index_free_list;
   int64_t total_size_of_data;

   int64_t transaction_id;
   int64_t data_transform_id;
   int64_t index_transform_id;

   header_info( )
    :
    version( 0 ),
    num_logs( 0 ),
    num_trans( 0 ),
    num_writers( 0 ),
    init_tranlog( 0 ),
    total_entries( 0 ),
    tranlog_offset( 0 ),
    index_free_list( 0 ),
    total_size_of_data( 0 ),
    transaction_id( 1 ),
    data_transform_id( 0 ),
    index_transform_id( 0 )
   {
   }
};

class ods_index_entry
{
   public:
   struct data_t
   {
      data_t( ) : pos( 0 ), size( 0 ), tran_id( 0 ), tran_op( 0 ) { }

      int64_t pos;
      int64_t size;
      int64_t tran_id;
      int64_t tran_op;
   } data;

   private:
   ods_index_entry( );

   void dump_entry( ostream& os, int64_t num );

   enum lock
   {
      e_lock_none,
      e_lock_entry
   };

   enum trans
   {
      e_trans_none,
      e_trans_change,
      e_trans_delete,
      e_trans_free_list
   };

   int64_t get_pos( ) const { return data.pos; }
   int64_t get_size( ) const { return data.size; }
   int64_t get_tran_id( ) const { return data.tran_id; }
   int64_t get_tran_op( ) const { return data.tran_op; }

   lock get_lock_flag( ) const { return lock_flag; }
   trans get_trans_flag( ) const { return trans_flag; }

   static size_t get_size_of( ) { return sizeof( data_t ); }

   lock lock_flag;
   trans trans_flag;

   friend class ods;
   friend ods& operator >>( ods& o, storable_base& s );
   friend ods& operator <<( ods& o, storable_base& s );
};

class header_file
{
   public:
   header_file( const char* p_file_name, ods::write_mode w_mode, bool use_sync_write = false );

   ~header_file( );

   operator int( ) const { return handle; }

   bool lock( );
   void unlock( );

   int get_offset( ) const { return offset; }

   bool is_locked_for_exclusive( ) const { return is_exclusive; }

   private:
   int handle;
   int offset;

   int lock_handle;
   string lock_file_name;

   bool is_exclusive;
   bool use_sync_write;

#ifdef __GNUG__
   flock file_lock;
   flock header_lock;
#endif
};

header_file::header_file( const char* p_file_name, ods::write_mode w_mode, bool use_sync_write )
 :
 handle( 0 ),
 offset( -1 ),
 lock_handle( 0 ),
 lock_file_name( p_file_name ),
 is_exclusive( w_mode == ods::e_write_mode_exclusive ),
 use_sync_write( use_sync_write )
{
   lock_file_name += c_lock_file_name_ext;

   bool okay = false;

#ifdef __GNUG__
   if( w_mode == ods::e_write_mode_none )
      okay = true;
   else
   {
      if( !use_sync_write )
         lock_handle = _open( lock_file_name.c_str( ), O_RDWR | O_CREAT, ODS_DEFAULT_PERMS );
      else
         lock_handle = _open( lock_file_name.c_str( ), O_RDWR | O_CREAT | O_SYNC, ODS_DEFAULT_PERMS );

      if( lock_handle > 0 )
      {
         if( !is_exclusive )
            file_lock.l_type = F_RDLCK;
         else
            file_lock.l_type = F_WRLCK;

         file_lock.l_len = 1;
         file_lock.l_start = 0;
         file_lock.l_whence = SEEK_SET;

         if( fcntl( lock_handle, F_SETLK, &file_lock ) == 0 )
            okay = true;
         else
         {
            _close( lock_handle );
            lock_handle = 0;
         }
      }
   }

   if( okay )
   {
      offset = 0;

      if( !use_sync_write )
         handle = _open( p_file_name, O_RDWR | O_CREAT, ODS_DEFAULT_PERMS );
      else
         handle = _open( p_file_name, O_RDWR | O_CREAT | O_SYNC, ODS_DEFAULT_PERMS );
   }
#endif

#ifdef ODS_DEBUG
   ostringstream osstr;
   osstr << "handle = " << handle << ", offset = " << offset << ", lock_handle = " << lock_handle;
   DEBUG_LOG( osstr.str( ) );
#endif
}

header_file::~header_file( )
{
   if( lock_handle > 0 )
   {
#ifdef __GNUG__
      file_lock.l_type = F_UNLCK;

      if( fcntl( lock_handle, F_SETLK, &file_lock ) != 0 )
         DEBUG_LOG( "unexpected fcntl failure occurred at " STRINGIZE( __LINE__ ) );
#endif

#ifndef ODS_DEBUG
      _close( lock_handle );
#else
      if( _close( lock_handle ) != 0 )
      {
         ostringstream osstr;

         osstr << "_close lock failed in ~header_file (errno = " << errno << ')';

         DEBUG_LOG( osstr.str( ) );
      }
      else
         DEBUG_LOG( "closed header lock file" );
#endif

      // NOTE: If was using exclusive access then can now remove all lock files.
      if( is_exclusive )
      {
         file_remove( lock_file_name );

         size_t ext_len = strlen( c_header_file_name_ext ) + strlen( c_lock_file_name_ext );

         string file_name( lock_file_name.substr( 0, lock_file_name.length( ) - ext_len ) );

         file_remove( file_name + c_data_file_name_ext + c_lock_file_name_ext );
         file_remove( file_name + c_index_file_name_ext + c_lock_file_name_ext );
      }
   }

   if( handle > 0 )
   {
#ifndef ODS_DEBUG
      _close( handle );
#else
      if( _close( handle ) != 0 )
      {
         ostringstream osstr;

         osstr << "_close failed in ~header_file (errno = " << errno << ')';

         DEBUG_LOG( osstr.str( ) );
      }
      else
         DEBUG_LOG( "closed header file" );
#endif
   }
}

bool header_file::lock( )
{
   bool retval = false;

   if( handle > 0 )
   {
#ifdef __GNUG__
      header_lock.l_len = 1;
      header_lock.l_type = F_WRLCK;
      header_lock.l_start = 0;
      header_lock.l_whence = SEEK_SET;

      if( fcntl( handle, F_SETLK, &header_lock ) == 0 )
         retval = true;
#endif
   }

   return retval;
}

void header_file::unlock( )
{
   if( handle > 0 )
   {
#ifdef __GNUG__
      header_lock.l_type = F_UNLCK;

      if( fcntl( handle, F_SETLK, &header_lock ) != 0 )
         THROW_ODS_ERROR( "unexpected fcntl at " STRINGIZE( __LINE__ ) " failed" );
#endif
   }
}

struct ods_data_entry_buffer
{
   char data[ c_data_bytes_per_item ];
};

class ods_data_cache_buffer : public cache_base< ods_data_entry_buffer >
{
   public:
   ods_data_cache_buffer( ods& o,
    const string& fname, unsigned max_cache_items,
    unsigned items_per_region, unsigned regions_in_cache = 1,
    bool use_placement_new = true, bool allow_lazy_writes = true, bool use_sync_file_writes = false )
    :
    cache_base< ods_data_entry_buffer >( max_cache_items,
     items_per_region, regions_in_cache, use_placement_new, allow_lazy_writes ),
    o( o ),
    fname( fname ),
    read_data_handle( 0 ),
    write_data_handle( 0 ),
    write_lock_handle( 0 ),
    use_sync_file_writes( use_sync_file_writes )
   {
#ifdef __GNUG__
      int rc = posix_memalign( ( void** )&p_data, getpagesize( ), sizeof( ods_data_entry_buffer ) );

      if( rc != 0 || !p_data )
         THROW_ODS_ERROR( "unexpected failure for posix_memalign" );
#endif
   }

   ~ods_data_cache_buffer( )
   {
#ifdef __GNUG__
      free( p_data );
#endif
#ifndef ODS_DEBUG
      if( read_data_handle )
         _close( read_data_handle );

      if( write_data_handle )
         _close( write_data_handle );

      if( write_lock_handle )
         _close( write_lock_handle );
#else
      if( read_data_handle && _close( read_data_handle ) != 0 )
      {
         ostringstream osstr;

         osstr << "_close failed for read handle in ~ods_data_cache_buffer (errno = " << errno << ')';

         DEBUG_LOG( osstr.str( ) );
      }

      if( write_data_handle && _close( write_data_handle ) != 0 )
      {
         ostringstream osstr;

         osstr << "_close failed for write handle in ~ods_data_cache_buffer (errno = " << errno << ')';

         DEBUG_LOG( osstr.str( ) );
      }

      if( write_lock_handle && _close( write_lock_handle ) != 0 )
      {
         ostringstream osstr;

         osstr << "_close failed for locking handle in ~ods_data_cache_buffer (errno = " << errno << ')';

         DEBUG_LOG( osstr.str( ) );
      }
#endif
   }

   bool lock_region( int64_t start, int64_t len )
   {
#ifdef ODS_DEBUG
      ostringstream osstr;

      osstr << "ods_data_cache_buffer::lock_region( ) start = " << start << ", len = " << len;

      DEBUG_LOG( osstr.str( ) );
#endif
      bool retval = false;
      guard lock_data( data_lock );

      if( !write_lock_handle )
      {
         string lock_file_name( fname );
         lock_file_name += c_lock_file_name_ext;
#ifdef __GNUG__
         write_lock_handle = _open( lock_file_name.c_str( ), O_RDWR | O_CREAT, ODS_DEFAULT_PERMS );
#else
         write_lock_handle = _sopen( lock_file_name.c_str( ), O_BINARY | O_WRONLY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
#endif
         if( write_lock_handle <= 0 )
            THROW_ODS_ERROR( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
      }

#ifdef __GNUG__
      lock.l_len = len;
      lock.l_type = F_WRLCK;
      lock.l_start = start;
      lock.l_whence = SEEK_SET;

      if( fcntl( write_lock_handle, F_SETLK, &lock ) == 0 )
         retval = true;
#endif

      return retval;
   }

   void unlock_region( int64_t start, int64_t len )
   {
#ifdef ODS_DEBUG
      ostringstream osstr;

      osstr << "ods_data_cache_buffer::unlock_region( ) start = " << start << ", len = " << len;

      DEBUG_LOG( osstr.str( ) );
#endif
      guard lock_data( data_lock );

#ifdef __GNUG__
      ( void )len;
      ( void )start;

      lock.l_type = F_UNLCK;

      if( fcntl( write_lock_handle, F_SETLK, &lock ) != 0 )
         THROW_ODS_ERROR( "unexpected fcntl at " STRINGIZE( __LINE__ ) " failed" );
#endif
   }

   private:
   mutex data_lock;

   ods& o;

   string fname;

   int read_data_handle;
   int write_data_handle;
   int write_lock_handle;

   bool use_sync_file_writes;

#ifdef __GNUG__
   flock lock;
   char* p_data;
#endif

   protected:
   void perform_fetch( ods_data_entry_buffer& data, unsigned num )
   {
#ifdef ODS_DEBUG
      ostringstream osstr;

      osstr << "ods_data_cache_buffer::perform_fetch( ) num = " << num;

      DEBUG_LOG( osstr.str( ) );
#endif
      guard lock_data( data_lock );

      if( !read_data_handle )
      {
#ifdef __GNUG__
         if( !use_sync_file_writes )
            read_data_handle = _open( fname.c_str( ), O_RDONLY | O_CREAT | O_DIRECT, ODS_DEFAULT_PERMS );
         else
            read_data_handle = _open( fname.c_str( ), O_RDONLY | O_CREAT | O_SYNC | O_DIRECT, ODS_DEFAULT_PERMS );
#else
         read_data_handle = _sopen( fname.c_str( ), O_BINARY | O_RDONLY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
#endif
         if( read_data_handle <= 0 )
            THROW_ODS_ERROR( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
      }

      int64_t pos;
      if( ( pos = _lseek( read_data_handle, ( num * sizeof( ods_data_entry_buffer ) ), SEEK_SET ) ) < 0 )
         THROW_ODS_ERROR( "unexpected _lseek at " STRINGIZE( __LINE__ ) " failed" );

#ifndef __GNUG__
      char* p_data( ( char* )&data.data );
#endif

      int len = _read( read_data_handle, ( void* )p_data, sizeof( ods_data_entry_buffer ) );

      if( len != sizeof( ods_data_entry_buffer ) )
      {
         if( _lseek( read_data_handle, 0, SEEK_CUR ) >= _lseek( read_data_handle, 0, SEEK_END ) )
            memset( p_data + len, 0, sizeof( ods_data_entry_buffer ) - len );
         else
            THROW_ODS_ERROR( "unexpected read at " STRINGIZE( __LINE__ ) " failed" );
      }

#ifdef __GNUG__
      memcpy( &data, p_data, sizeof( ods_data_entry_buffer ) );
#endif
   }

   void perform_store( const ods_data_entry_buffer& data, unsigned num )
   {
#ifdef ODS_DEBUG
      ostringstream osstr;

      osstr << "ods_data_cache_buffer::perform_store( ) num = " << num;

      DEBUG_LOG( osstr.str( ) );
#endif
      guard lock_data( data_lock );

      if( !write_data_handle )
      {
#ifdef __GNUG__
         if( !use_sync_file_writes )
            write_data_handle = _open( fname.c_str( ), O_WRONLY | O_CREAT | O_DIRECT, ODS_DEFAULT_PERMS );
         else
            write_data_handle = _open( fname.c_str( ), O_WRONLY | O_CREAT | O_SYNC | O_DIRECT, ODS_DEFAULT_PERMS );
#else
         write_data_handle = _sopen( fname.c_str( ), O_BINARY | O_WRONLY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
#endif
         if( write_data_handle <= 0 )
            THROW_ODS_ERROR( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
      }

      if( _lseek( write_data_handle, ( num * sizeof( ods_data_entry_buffer ) ), SEEK_SET ) < 0 )
         THROW_ODS_ERROR( "unexpected seek at " STRINGIZE( __LINE__ ) " failed" );

#ifndef __GNUG__
      char* p_data( ( char* )&data );
#else
      memcpy( p_data, &data, sizeof( ods_data_entry_buffer ) );
#endif

      if( _write( write_data_handle, ( void* )p_data,
       sizeof( ods_data_entry_buffer ) ) != sizeof( ods_data_entry_buffer ) )
         THROW_ODS_ERROR( "unexpected write at " STRINGIZE( __LINE__ ) " failed" );
   }
};

struct ods_index_entry_buffer
{
   ods_index_entry::data_t item[ c_index_items_per_item ];
};

class ods_index_cache_buffer : public cache_base< ods_index_entry_buffer >
{
   public:
   ods_index_cache_buffer( const string& file_name, int lock_offset,
    unsigned max_cache_items, unsigned items_per_region, unsigned regions_in_cache = 1,
    bool use_placement_new = true, bool allow_lazy_writes = true, bool use_sync_file_writes = false )
    :
    cache_base< ods_index_entry_buffer >( max_cache_items,
     items_per_region, regions_in_cache, use_placement_new, allow_lazy_writes ),
    file_name( file_name ),
    lock_offset( lock_offset ),
    lock_index_handle( 0 ),
    read_index_handle( 0 ),
    write_index_handle( 0 ),
    use_sync_file_writes( use_sync_file_writes )
   {
#ifdef __GNUG__
      int rc = posix_memalign( ( void** )&p_data, getpagesize( ), sizeof( ods_index_entry_buffer ) );

      if( rc != 0 || !p_data )
         THROW_ODS_ERROR( "unexpected failure for posix_memalign" );
#endif
   }

   ~ods_index_cache_buffer( )
   {
#ifdef __GNUG__
      free( p_data );
#endif
#ifndef ODS_DEBUG
      if( lock_index_handle )
         _close( lock_index_handle );

      if( read_index_handle )
         _close( read_index_handle );

      if( write_index_handle )
         _close( write_index_handle );
#else
      if( lock_index_handle && _close( lock_index_handle ) != 0 )
      {
         ostringstream osstr;

         osstr << "_close failed for lock handle in ~ods_index_cache_buffer (errno = " << errno << ')';

         DEBUG_LOG( osstr.str( ) );
      }

      if( read_index_handle && _close( read_index_handle ) != 0 )
      {
         ostringstream osstr;

         osstr << "_close failed for read handle in ~ods_index_cache_buffer (errno = " << errno << ')';

         DEBUG_LOG( osstr.str( ) );
      }

      if( write_index_handle && _close( write_index_handle ) != 0 )
      {
         ostringstream osstr;

         osstr << "_close failed for write handle in ~ods_index_cache_buffer (errno = " << errno << ')';

         DEBUG_LOG( osstr.str( ) );
      }
#endif
   }

   int64_t get_file_size( )
   {
      guard lock_index( index_lock );

      if( !read_index_handle )
      {
#ifdef __GNUG__
         if( !use_sync_file_writes )
            read_index_handle = _open( file_name.c_str( ), O_RDONLY | O_DIRECT );
         else
            read_index_handle = _open( file_name.c_str( ), O_RDONLY | O_SYNC | O_DIRECT );
#else
         read_index_handle = _sopen( file_name.c_str( ), O_BINARY | O_RDONLY, SH_DENYNO );
#endif
         if( read_index_handle <= 0 )
            THROW_ODS_ERROR( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
      }

      int64_t pos = _lseek( read_index_handle, 0, SEEK_END );

      if( pos < 0 )
         THROW_ODS_ERROR( "unexpected seek at " STRINGIZE( __LINE__ ) " failed" );

      return pos;
   }

   bool lock_entry( int64_t entry_num, bool is_write )
   {
#ifdef ODS_DEBUG
      ostringstream osstr;
      osstr << "ods_index_cache_buffer::lock_entry( ) entry_num = " << entry_num << ", is_write = " << is_write;
      DEBUG_LOG( osstr.str( ) );
#endif
      guard lock_index( index_lock );

      if( !lock_index_handle )
      {
         string lock_file_name( file_name + c_lock_file_name_ext );

#ifdef __GNUG__
         lock_index_handle = _open( lock_file_name.c_str( ), O_RDWR | O_CREAT, ODS_DEFAULT_PERMS );
#else
         lock_index_handle = _sopen( lock_file_name.c_str( ), O_BINARY | O_RDWR | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
#endif
         if( lock_index_handle <= 0 )
            THROW_ODS_ERROR( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
      }

#ifdef __GNUG__
      lock.l_len = sizeof( ods_index_entry::data_t );
      lock.l_type = is_write ? F_WRLCK : F_RDLCK;
      lock.l_start = entry_num;
      lock.l_whence = SEEK_SET;

      return fcntl( lock_index_handle, F_SETLK, &lock ) == 0;
#endif
   }

   void unlock_entry( int64_t entry_num, bool is_write )
   {
#ifdef ODS_DEBUG
      ostringstream osstr;

      osstr << "ods_index_cache_buffer::unlock_entry( ) entry_num = " << entry_num << ", is_write = " << is_write;

      DEBUG_LOG( osstr.str( ) );
#endif
      guard lock_index( index_lock );

#ifdef __GNUG__
      ( void )is_write;
      ( void )entry_num;

      lock.l_type = F_UNLCK;

      if( fcntl( lock_index_handle, F_SETLK, &lock ) != 0 )
         DEBUG_LOG( "unexpected fcntl failure occurred at " STRINGIZE( __LINE__ ) );
#endif
   }

   private:
   mutex index_lock;

   string file_name;

   int lock_offset;
   int lock_index_handle;

   int read_index_handle;
   int write_index_handle;

   bool use_sync_file_writes;

#ifdef __GNUG__
   flock lock;
   char* p_data;
#endif

   protected:
   void perform_fetch( ods_index_entry_buffer& data, unsigned num )
   {
#ifdef ODS_DEBUG
      ostringstream osstr;

      osstr << "ods_index_cache_buffer::perform_fetch( ) num = " << num;

      DEBUG_LOG( osstr.str( ) );
#endif
      guard lock_index( index_lock );

      if( !read_index_handle )
      {
#ifdef __GNUG__
         if( !use_sync_file_writes )
            read_index_handle = _open( file_name.c_str( ), O_RDONLY | O_CREAT | O_DIRECT, ODS_DEFAULT_PERMS );
         else
            read_index_handle = _open( file_name.c_str( ), O_RDONLY | O_CREAT | O_SYNC | O_DIRECT, ODS_DEFAULT_PERMS );
#else
         read_index_handle = _sopen( file_name.c_str( ), O_BINARY | O_RDONLY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
#endif
         if( read_index_handle <= 0 )
            THROW_ODS_ERROR( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
      }

      if( _lseek( read_index_handle, ( num * sizeof( ods_index_entry_buffer ) ), SEEK_SET ) < 0 )
         THROW_ODS_ERROR( "unexpected seek at " STRINGIZE( __LINE__ ) " failed" );

#ifndef __GNUG__
      char* p_data( ( char* )&data );
#else
#endif

      int len = _read( read_index_handle, ( void* )p_data, sizeof( ods_index_entry_buffer ) );

      if( len != sizeof( ods_index_entry_buffer ) )
      {
         if( _lseek( read_index_handle, 0, SEEK_CUR ) >= _lseek( read_index_handle, 0, SEEK_END ) )
            memset( p_data + len, 0, sizeof( ods_index_entry_buffer ) - len );
         else
            THROW_ODS_ERROR( "unexpected read at " STRINGIZE( __LINE__ ) " failed" );
      }

#ifdef __GNUG__
      memcpy( &data, p_data, sizeof( ods_index_entry_buffer ) );
#endif
   }

   void perform_store( const ods_index_entry_buffer& data, unsigned num )
   {
#ifdef ODS_DEBUG
      ostringstream osstr;

      osstr << "ods_index_cache_buffer::perform_store( ) num = " << num;

      DEBUG_LOG( osstr.str( ) );
#endif
      guard lock_index( index_lock );

      if( !write_index_handle )
      {
#ifdef __GNUG__
         if( !use_sync_file_writes )
            write_index_handle = _open( file_name.c_str( ), O_WRONLY | O_CREAT | O_DIRECT, ODS_DEFAULT_PERMS );
         else
            write_index_handle = _open( file_name.c_str( ), O_WRONLY | O_CREAT | O_SYNC | O_DIRECT, ODS_DEFAULT_PERMS );
#else
         write_index_handle = _sopen( file_name.c_str( ), O_BINARY | O_WRONLY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
#endif
         if( write_index_handle <= 0 )
            THROW_ODS_ERROR( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
      }

      if( _lseek( write_index_handle, ( num * sizeof( ods_index_entry_buffer ) ), SEEK_SET ) < 0 )
         THROW_ODS_ERROR( "unexpected seek at " STRINGIZE( __LINE__ ) " failed" );

#ifndef __GNUG__
      char* p_data( ( char* )&data );
#else
      memcpy( p_data, &data, sizeof( ods_index_entry_buffer ) );
#endif

      if( _write( write_index_handle, ( void* )p_data,
       sizeof( ods_index_entry_buffer ) ) != sizeof( ods_index_entry_buffer ) )
         THROW_ODS_ERROR( "unexpected write at " STRINGIZE( __LINE__ ) " failed" );
   }
};

class ods_trans_op_cache_buffer : public cache_base< trans_op_buffer >
{
   public:
   ods_trans_op_cache_buffer( unsigned max_cache_items,
    unsigned items_per_region, unsigned regions_in_cache = 1,
    bool use_placement_new = true, bool allow_lazy_writes = true, bool use_sync_file_writes = false )
    :
    cache_base< trans_op_buffer >( max_cache_items,
     items_per_region, regions_in_cache, use_placement_new, allow_lazy_writes ),
    tran_ops_handle( 0 ),
    has_begun_trans( false ),
    use_sync_file_writes( use_sync_file_writes )
   {
#ifdef __GNUG__
      int rc = posix_memalign( ( void** )&p_data, getpagesize( ), sizeof( trans_op_buffer ) );

      if( rc != 0 || !p_data )
         THROW_ODS_ERROR( "unexpected failure for posix_memalign" );
#endif
   }

   ~ods_trans_op_cache_buffer( )
   {
#ifdef __GNUG__
      free( p_data );
#endif
   }

   void new_transaction( int64_t tran_id )
   {
#ifdef ODS_DEBUG
      DEBUG_LOG( "in ods_trans_op_cache_buffer::new_transaction( )" );
#endif
      guard lock_trans( trans_op_lock );

      ostringstream osstr;
      osstr << hex << setw( sizeof( int64_t ) * 2 ) << setfill( '0' ) << tran_id;

      file_name = osstr.str( ) + c_ops_file_name_ext;

      has_begun_trans = true;
   }

   void end_transaction( )
   {
#ifdef ODS_DEBUG
      DEBUG_LOG( "in ods_trans_op_cache_buffer::end_transaction( )" );
#endif
      guard lock_trans( trans_op_lock );

      if( tran_ops_handle )
      {
         if( _close( tran_ops_handle ) != 0 )
            THROW_ODS_ERROR( "unexpected _close at " STRINGIZE( __LINE__ ) " failed" );
         if( remove( file_name.c_str( ) ) != 0 )
            THROW_ODS_ERROR( "unexpected remove at " STRINGIZE( __LINE__ ) " failed" );

         tran_ops_handle = 0;
         has_begun_trans = false;
      }

      clear( );
   }

   private:
   int tran_ops_handle;

   bool has_begun_trans;
   bool use_sync_file_writes;

   string file_name;
   mutex trans_op_lock;

#ifdef __GNUG__
   char* p_data;
#endif

   protected:
   void perform_fetch( trans_op_buffer& data, unsigned num )
   {
#ifdef ODS_DEBUG
      ostringstream osstr;
      osstr << "ods_trans_op_cache_buffer::perform_fetch( ) num = " << num;
      DEBUG_LOG( osstr.str( ) );
#endif
      guard lock_trans( trans_op_lock );

      if( !tran_ops_handle )
         THROW_ODS_ERROR( "unexpected null file handle at "  STRINGIZE( __LINE__ ) );

      if( _lseek( tran_ops_handle, ( num * sizeof( trans_op_buffer ) ), SEEK_SET ) < 0 )
         THROW_ODS_ERROR( "unexpected seek at " STRINGIZE( __LINE__ ) " failed" );

#ifndef __GNUG__
      char* p_data( ( char* )&data );
#endif

      if( _read( tran_ops_handle,
       ( void* )p_data, sizeof( trans_op_buffer ) ) != sizeof( trans_op_buffer ) )
         THROW_ODS_ERROR( "unexpected read at " STRINGIZE( __LINE__ ) " failed" );

#ifdef __GNUG__
      memcpy( &data, p_data, sizeof( trans_op_buffer ) );
#endif
   }

   void perform_store( const trans_op_buffer& data, unsigned num )
   {
#ifdef ODS_DEBUG
      ostringstream osstr;
      osstr << "ods_trans_op_cache_buffer::perform_store( ) num = " << num;
      DEBUG_LOG( osstr.str( ) );
#endif
      guard lock_trans( trans_op_lock );

      if( !tran_ops_handle )
      {
         if( !has_begun_trans )
            THROW_ODS_ERROR( "unexpected not in transaction at " STRINGIZE( __LINE__ ) );

#ifdef __GNUG__
         if( !use_sync_file_writes )
            tran_ops_handle = _open( file_name.c_str( ), O_RDWR | O_CREAT | O_DIRECT, ODS_DEFAULT_PERMS );
         else
            tran_ops_handle = _open( file_name.c_str( ), O_RDWR | O_CREAT | O_SYNC | O_DIRECT, ODS_DEFAULT_PERMS );
#else
         tran_ops_handle = _sopen( file_name.c_str( ), O_BINARY | O_RDWR | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
#endif

         if( tran_ops_handle <= 0 )
         {
#ifdef ODS_DEBUG
            if( errno == EMFILE )
               DEBUG_LOG( "too many files open for trans op cache" );
            else
            {
               ostringstream osstr;
               osstr << "errno = " << errno;
               DEBUG_LOG( osstr.str( ) );
            }
#endif
            THROW_ODS_ERROR( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
         }
      }

      if( _lseek( tran_ops_handle, ( num * sizeof( trans_op_buffer ) ), SEEK_SET ) < 0 )
         THROW_ODS_ERROR( "unexpected seek at " STRINGIZE( __LINE__ ) " failed" );

#ifndef __GNUG__
      char* p_data( ( char* )&data );
#else
      memcpy( p_data, &data, sizeof( trans_op_buffer ) );
#endif

      if( _write( tran_ops_handle, ( void* )p_data,
       sizeof( trans_op_buffer ) ) != sizeof( trans_op_buffer ) )
         THROW_ODS_ERROR( "unexpected write at " STRINGIZE( __LINE__ ) " failed" );
   }
};

class ods_trans_data_cache_buffer : public cache_base< trans_data_buffer >
{
   public:
   ods_trans_data_cache_buffer( unsigned max_cache_items,
    unsigned items_per_region, unsigned regions_in_cache = 1,
    bool use_placement_new = true, bool allow_lazy_writes = true, bool use_sync_file_writes = false )
    :
    cache_base< trans_data_buffer >( max_cache_items,
     items_per_region, regions_in_cache, use_placement_new, allow_lazy_writes ),
    tran_data_handle( 0 ),
    has_begun_trans( false ),
    use_sync_file_writes( use_sync_file_writes )
   {
#ifdef __GNUG__
      int rc = posix_memalign( ( void** )&p_data, getpagesize( ), sizeof( trans_data_buffer ) );

      if( rc != 0 || !p_data )
         THROW_ODS_ERROR( "unexpected failure for posix_memalign" );
#endif
   }

   ~ods_trans_data_cache_buffer( )
   {
#ifdef __GNUG__
      free( p_data );
#endif
   }

   void new_transaction( int64_t tran_id )
   {
#ifdef ODS_DEBUG
      DEBUG_LOG( "in ods_trans_data_cache_buffer::new_transaction( )" );
#endif
      guard lock_trans( trans_data_lock );

      ostringstream osstr;
      osstr << hex << setw( sizeof( int64_t ) * 2 ) << setfill( '0' ) << tran_id;

      file_name = osstr.str( ) + c_data_file_name_ext;

      has_begun_trans = true;
   }

   void end_transaction( )
   {
#ifdef ODS_DEBUG
      DEBUG_LOG( "in ods_trans_data_cache_buffer::end_transaction( )" );
#endif
      guard lock_trans( trans_data_lock );

      if( tran_data_handle )
      {
         if( _close( tran_data_handle ) != 0 )
            THROW_ODS_ERROR( "unexpected _close at " STRINGIZE( __LINE__ ) " failed" );
         if( remove( file_name.c_str( ) ) != 0 )
            THROW_ODS_ERROR( "unexpected remove at " STRINGIZE( __LINE__ ) " failed" );

         tran_data_handle = 0;
         has_begun_trans = false;
      }

      clear( );
   }

   private:
   int tran_data_handle;

   bool has_begun_trans;
   bool use_sync_file_writes;

   string file_name;
   mutex trans_data_lock;

#ifdef __GNUG__
   char* p_data;
#endif

   protected:
   void perform_fetch( trans_data_buffer& data, unsigned num )
   {
#ifdef ODS_DEBUG
      ostringstream osstr;
      osstr << "ods_trans_data_cache_buffer::perform_fetch( ) num = " << num;
      DEBUG_LOG( osstr.str( ) );
#endif
      guard lock_trans( trans_data_lock );

      if( !tran_data_handle )
         THROW_ODS_ERROR( "unexpected null file handle at " STRINGIZE( __LINE__ ) );

      if( _lseek( tran_data_handle, ( num * sizeof( trans_data_buffer ) ), SEEK_SET ) < 0 )
         THROW_ODS_ERROR( "unexpected seek at " STRINGIZE( __LINE__ ) " failed" );

#ifndef __GNUG__
      char* p_data( ( char* )&data );
#endif

      if( _read( tran_data_handle,
       ( void* )p_data, sizeof( trans_data_buffer ) ) != sizeof( trans_data_buffer ) )
         THROW_ODS_ERROR( "unexpected read at " STRINGIZE( __LINE__ ) " failed" );

#ifdef __GNUG__
      memcpy( &data, p_data, sizeof( trans_data_buffer ) );
#endif
   }

   void perform_store( const trans_data_buffer& data, unsigned num )
   {
#ifdef ODS_DEBUG
      ostringstream osstr;
      osstr << "ods_trans_data_cache_buffer::perform_store( ) num = " << num;
      DEBUG_LOG( osstr.str( ) );
#endif
      guard lock_trans( trans_data_lock );

      if( !tran_data_handle )
      {
         if( !has_begun_trans )
            THROW_ODS_ERROR( "unexpected not in transaction at " STRINGIZE( __LINE__ ) );

#ifdef __GNUG__
         if( !use_sync_file_writes )
            tran_data_handle = _open( file_name.c_str( ), O_RDWR | O_CREAT | O_DIRECT, ODS_DEFAULT_PERMS );
         else
            tran_data_handle = _open( file_name.c_str( ), O_RDWR | O_CREAT | O_SYNC | O_DIRECT, ODS_DEFAULT_PERMS );
#else
         tran_data_handle = _sopen( file_name.c_str( ), O_BINARY | O_RDWR | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
#endif

         if( tran_data_handle <= 0 )
         {
#ifdef ODS_DEBUG
            if( errno == EMFILE )
               DEBUG_LOG( "too many files open for trans data cache" );
            else
            {
               ostringstream osstr;
               osstr << "errno = " << errno;
               DEBUG_LOG( osstr.str( ) );
            }
#endif
            THROW_ODS_ERROR( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
         }
      }

      if( _lseek( tran_data_handle, ( num * sizeof( trans_data_buffer ) ), SEEK_SET ) < 0 )
         THROW_ODS_ERROR( "unexpected seek at " STRINGIZE( __LINE__ ) " failed" );

#ifndef __GNUG__
      char* p_data( ( char* )&data );
#else
      memcpy( p_data, &data, sizeof( trans_data_buffer ) );
#endif

      if( _write( tran_data_handle, ( void* )p_data,
       sizeof( trans_data_buffer ) ) != sizeof( trans_data_buffer ) )
         THROW_ODS_ERROR( "unexpected write at " STRINGIZE( __LINE__ ) " failed" );
   }
};

struct transaction_buffer
{
   transaction_buffer( ) : tran_id( 0 ) { }

   int64_t tran_id;

   stack< transaction_level_info > levels;
};

struct ods::impl
{
   enum bulk_mode
   {
      e_bulk_mode_none,
      e_bulk_mode_dump,
      e_bulk_mode_read,
      e_bulk_mode_write
   };

   impl( )
    :
    is_new( false ),
    is_corrupt( false ),
    is_encrypted( false ),
    is_read_only( false ),
    is_restoring( false ),
    force_padding( false ),
    using_tranlog( false ),
    cipher_type( e_stream_cipher_unknown ),
    trans_level( 0 ),
    tranlog_offset( 0 ),
    read_from_trans( false ),
    total_trans_size( 0 ),
    total_trans_op_count( 0 )
   {
   }

   string name;

   string pwd_hash;

   string data_file_name;
   string index_file_name;
   string header_file_name;
   string tranlog_file_name;

#ifdef __GNUG__
   ref_count_ptr< flock > rp_lock;
#endif

   bool is_new;
   bool is_corrupt;
   bool is_encrypted;
   bool is_read_only;
   bool is_restoring;

   bool force_padding;
   bool using_tranlog;

   stream_cipher cipher_type;

   thread_id dummy_thread_id;

   ref_count_ptr< int > rp_bulk_mode;
   ref_count_ptr< int > rp_bulk_level;
   ref_count_ptr< bool > rp_has_changed;
   ref_count_ptr< bool > rp_is_in_bulk_pause;

   ref_count_ptr< thread_id > rp_bulk_read_thread_id;
   ref_count_ptr< thread_id > rp_bulk_write_thread_id;

   ref_count_ptr< header_file > rp_header_file;
   ref_count_ptr< header_info > rp_header_info;

   ods_data_entry_buffer data_read_buffer;
   ods_data_entry_buffer data_write_buffer;
   ods_data_entry_buffer data_read_key_buffer;
   ods_data_entry_buffer data_write_key_buffer;
   ref_count_ptr< ods_data_cache_buffer > rp_ods_data_cache_buffer;

   ods_index_entry_buffer index_item_buffer;
   ref_count_ptr< ods_index_cache_buffer > rp_ods_index_cache_buffer;

   int64_t trans_level;
   int64_t tranlog_offset;

   bool read_from_trans;

   int64_t total_trans_size;
   int64_t total_trans_op_count;

   transaction_buffer* p_trans_buffer;
   ods_trans_op_cache_buffer* p_ods_trans_op_cache_buffer;
   ods_trans_data_cache_buffer* p_ods_trans_data_cache_buffer;

   ref_count_ptr< int > rp_open_store_ref_count;

   ref_count_ptr< int64_t > rp_session_create_total;
   ref_count_ptr< int64_t > rp_session_revive_total;
   ref_count_ptr< int64_t > rp_session_review_total;
   ref_count_ptr< int64_t > rp_session_update_total;
   ref_count_ptr< int64_t > rp_session_delete_total;

   ref_count_ptr< mutex > rp_impl_lock;
   ref_count_ptr< mutex > rp_file_section;

   void read_header_file_info( );
   void write_header_file_info( bool for_close = false );

   void force_write_header_file_info( bool for_close = false );

   bool found_instance_currently_reading( int64_t num );
   bool found_instance_currently_writing( int64_t num );

   ref_count_ptr< vector< ods* > > rp_instances;
};

void ods::impl::read_header_file_info( )
{
   if( _lseek( *rp_header_file, 0, SEEK_SET ) != 0 )
      THROW_ODS_ERROR( "unexpected seek at " STRINGIZE( __LINE__ ) " failed" );

   if( _read( *rp_header_file,
    ( void* )rp_header_info.get( ), sizeof( header_info ) ) != sizeof( header_info ) )
      THROW_ODS_ERROR( "unexpected read at " STRINGIZE( __LINE__ ) " failed" );
}

void ods::impl::write_header_file_info( bool for_close )
{
   // NOTE: If this is the commencement of one or more write operations then increment
   // a counter for active writers which will only be decremented if a change had been
   // flagged and the "for_close" is true to indicate write operations were completed.
   if( !*rp_has_changed )
   {
      *rp_has_changed = true;
      ++rp_header_info->num_writers;
   }
   else if( for_close )
      --rp_header_info->num_writers;

   if( _lseek( *rp_header_file, 0, SEEK_SET ) != 0 )
      THROW_ODS_ERROR( "unexpected seek at " STRINGIZE( __LINE__ ) " failed" );

   if( _write( *rp_header_file,
    ( void* )rp_header_info.get( ), sizeof( header_info ) ) != sizeof( header_info ) )
      THROW_ODS_ERROR( "unexpected write at " STRINGIZE( __LINE__ ) " failed" );

#ifdef ODS_DEBUG
   DEBUG_LOG( "*** header file info written ***" );
#endif
}

void ods::impl::force_write_header_file_info( bool for_close )
{
   // NOTE: The file will be marked as corrupt unless "has_changed" is set true.
   temp_set_value< bool > temp_has_changed( *rp_has_changed, true );

   write_header_file_info( for_close );
}

bool ods::impl::found_instance_currently_reading( int64_t num )
{
   for( vector< ods* >::iterator iter = rp_instances->begin( ); iter != rp_instances->end( ); ++iter )
      if( *iter && ( ( *iter )->current_read_object_num == num ) )
         return true;

   return false;
}

bool ods::impl::found_instance_currently_writing( int64_t num )
{
   for( vector< ods* >::iterator iter = rp_instances->begin( ); iter != rp_instances->end( ); ++iter )
      if( *iter && ( ( *iter )->current_write_object_num == num ) )
         return true;

   return false;
}

struct ods_index_entry_pos
{
   int64_t id;
   int64_t pos;
   int64_t size;
   int64_t tran_id;

   bool operator <( const ods_index_entry_pos& lval ) const
   {
      return pos < lval.pos;
   }
};

ods_index_entry::ods_index_entry( )
 :
 lock_flag( e_lock_none ),
 trans_flag( e_trans_none )
{
}

void ods_index_entry::dump_entry( ostream& os, int64_t num )
{
   os << "num: " << hex
    << setw( sizeof( int64_t ) * 2 ) << setfill( '0' ) << num
    << "          pos: " << setw( sizeof( int64_t ) * 2 ) << setfill( '0' ) << data.pos
    << "          len: " << setw( sizeof( int64_t ) * 2 ) << setfill( '0' ) << data.size
    << "\ntxn: " << setw( sizeof( int64_t ) * 2 ) << setfill( '0' ) << data.tran_id
    << "          txo: " << setw( sizeof( int64_t ) * 2 ) << setfill( '0' ) << data.tran_op
    << "               flags: lk=" << setw( 1 ) << setfill( '0' ) << lock_flag
    << " tx=" << setw( 1 ) << setfill( '0' ) << trans_flag << dec << '\n';
}

struct ods::file_scope
{
   file_scope( ods& o )
    :
    o( o )
   {
      o.open_store( );
   }

   ~file_scope( )
   {
      o.close_store( );
   }

   ods& o;
};

struct ods::header_file_lock
{
   header_file_lock( ods& o )
    :
    o( o )
   {
      o.lock_header_file( );
   }

   ~header_file_lock( )
   {
      o.unlock_header_file( );
   }

   ods& o;
};

ods* ods::instance( ods* p_ods, bool force_assign )
{
   static TLS( ods )* gtp_ods;

   if( p_ods || force_assign )
      gtp_ods = p_ods;

   return gtp_ods;
}

ods::ods( const ods& o )
 :
 okay( false ),
 p_progress( 0 ),
 is_in_read( false ),
 is_in_write( false ),
 permit_copy( false ),
 use_sync_write( false ),
 prevent_lazy_write( false ),
 data_read_buffer_num( -1 ),
 data_read_buffer_offs( 0 ),
 data_write_buffer_num( -1 ),
 data_write_buffer_offs( 0 ),
 index_item_buffer_num( -1 ),
 trans_read_ops_buffer_num( -1 ),
 trans_read_data_buffer_num( -1 ),
 trans_read_data_buffer_offs( 0 ),
 had_interim_trans_op_write( false ),
 had_interim_trans_data_write( false ),
 trans_write_ops_buffer_num( -1 ),
 trans_write_data_buffer_num( -1 ),
 trans_write_data_buffer_offs( 0 ),
 current_read_object_num( -1 ),
 current_write_object_num( -1 )
{
   guard lock_copy_ctor( g_ods_lock );

   DEBUG_LOG( "ods::ods( const ods& )" );

   if( !o.permit_copy )
      THROW_ODS_ERROR( "copy construction was disallowed" );

   auto_ptr< impl > ap_impl( new impl( *o.p_impl ) );
   p_impl = ap_impl.get( );

   guard tmp_lock( *p_impl->rp_impl_lock );

   if( !file_exists( p_impl->data_file_name ) )
      THROW_ODS_ERROR( "could not find database data file..." );
   if( !file_exists( p_impl->index_file_name ) )
      THROW_ODS_ERROR( "could not find database index file..." );

   if( *p_impl->rp_header_file <= 0 )
      THROW_ODS_ERROR( "unexpected header file handle is invalid" );

   auto_ptr< transaction_buffer > ap_trans_buffer( new transaction_buffer );

   auto_ptr< ods_trans_op_cache_buffer > ap_ods_trans_op_cache_buffer(
    new ods_trans_op_cache_buffer( c_trans_op_max_cache_items, c_trans_op_items_per_region ) );

   auto_ptr< ods_trans_data_cache_buffer > ap_ods_trans_data_cache_buffer(
    new ods_trans_data_cache_buffer( c_trans_data_max_cache_items, c_trans_data_items_per_region ) );

   vector< ods* >::iterator iter;
   for( iter = p_impl->rp_instances->begin( ); iter != p_impl->rp_instances->end( ); ++iter )
   {
      if( !*iter )
      {
         *iter = this;
         break;
      }
   }

   if( iter == p_impl->rp_instances->end( ) )
      p_impl->rp_instances->push_back( this );

   p_impl->trans_level = 0;
   p_impl->read_from_trans = false;

   p_impl->total_trans_size = 0;
   p_impl->total_trans_op_count = 0;

   p_impl->p_trans_buffer = ap_trans_buffer.release( );
   p_impl->p_ods_trans_op_cache_buffer = ap_ods_trans_op_cache_buffer.release( );
   p_impl->p_ods_trans_data_cache_buffer = ap_ods_trans_data_cache_buffer.release( );

   ap_impl.release( );

   okay = true;
   permit_copy = true;
}

ods::ods(
 const char* p_name, open_mode o_mode,
 write_mode w_mode, bool using_tranlog,
 bool* p_not_found, const char* p_password, bool use_sync_write )
 :
 okay( false ),
 p_progress( 0 ),
 is_in_read( false ),
 is_in_write( false ),
 permit_copy( false ),
 use_sync_write( use_sync_write ),
 prevent_lazy_write( false ),
 data_read_buffer_num( -1 ),
 data_read_buffer_offs( 0 ),
 data_write_buffer_num( -1 ),
 data_write_buffer_offs( 0 ),
 index_item_buffer_num( -1 ),
 trans_read_ops_buffer_num( -1 ),
 trans_read_data_buffer_num( -1 ),
 trans_read_data_buffer_offs( 0 ),
 had_interim_trans_op_write( false ),
 had_interim_trans_data_write( false ),
 trans_write_ops_buffer_num( -1 ),
 trans_write_data_buffer_num( -1 ),
 trans_write_data_buffer_offs( 0 ),
 current_read_object_num( -1 ),
 current_write_object_num( -1 )
{
   guard lock_ctor( g_ods_lock );

   DEBUG_LOG( "ods::ods( )" );

   auto_ptr< impl > ap_impl( new impl );
   p_impl = ap_impl.get( );

   p_impl->name = string( p_name );

   p_impl->data_file_name = p_impl->name + c_data_file_name_ext;
   p_impl->index_file_name = p_impl->name + c_index_file_name_ext;
   p_impl->header_file_name = p_impl->name + c_header_file_name_ext;
   p_impl->tranlog_file_name = p_impl->name + c_tranlog_file_name_ext;

   if( p_password )
   {
      p_impl->is_encrypted = true;
      sha256 hash( ( const unsigned char* )p_password, strlen( p_password ) );

      p_impl->pwd_hash.resize( c_sha256_digest_size );
      hash.copy_digest_to_buffer( ( unsigned char* )p_impl->pwd_hash.data( ) );
   }

#ifdef __GNUG__
   p_impl->rp_lock = new flock;
#endif

   p_impl->rp_bulk_mode = new int( 0 );
   p_impl->rp_bulk_level = new int( 0 );
   p_impl->rp_has_changed = new bool( false );
   p_impl->rp_is_in_bulk_pause = new bool( false );
   p_impl->rp_bulk_read_thread_id = new thread_id;
   p_impl->rp_bulk_write_thread_id = new thread_id;

#ifdef ODS_DEBUG
   if( w_mode == e_write_mode_exclusive )
      DEBUG_LOG( "********** capturing exclusive write use **********" );
#endif

   p_impl->rp_header_file
    = new header_file( p_impl->header_file_name.c_str( ), w_mode, ( !using_tranlog && use_sync_write ) );

   if( *p_impl->rp_header_file <= 0 )
      THROW_ODS_ERROR( "unable to open database header file (locked?)" );

   if( p_impl->rp_header_file->get_offset( ) < 0 )
      THROW_ODS_ERROR( "unable to acquire valid offset (max. concurrent processes active?)" );

   if( w_mode == e_write_mode_none )
      p_impl->is_read_only = true;

   if( p_impl->is_read_only && o_mode == e_open_mode_create_if_not_exist )
      THROW_ODS_ERROR( "cannot create if not exists when opening database for read only access" );

   auto_ptr< ods::header_file_lock > ap_header_file_lock( new ods::header_file_lock( *this ) );

   if( !file_exists( p_impl->index_file_name ) )
      p_impl->is_new = true;

   switch( o_mode )
   {
      case e_open_mode_exist:
      if( p_impl->is_new )
      {
         ap_header_file_lock.reset( );

         p_impl->rp_header_file = 0;
         remove( p_impl->header_file_name.c_str( ) );

         if( p_not_found )
            *p_not_found = true;

         THROW_ODS_ERROR( "database index file does not exist" );
      }
      break;

      case e_open_mode_not_exist:
      if( !p_impl->is_new )
         THROW_ODS_ERROR( "database index file already exists" );
      break;

      case e_open_mode_create_if_not_exist:
      break;

      default:
      THROW_ODS_ERROR( "unexpected default case at " STRINGIZE( __LINE__ ) );
   }

   p_impl->rp_impl_lock = new mutex;
   p_impl->rp_file_section = new mutex;

   p_impl->rp_header_info = new header_info;

   int64_t now = time( 0 );

   log_info tranlog_info;

   if( p_impl->is_encrypted )
   {
      tranlog_info.version |= c_encrypted_flag;
      tranlog_info.val_hash = *( ( uint32_t* )( p_impl->pwd_hash.data( ) ) );
   }

   if( p_impl->is_new )
   {
      if( file_exists( p_impl->tranlog_file_name ) )
      {
         ifstream inpf( p_impl->tranlog_file_name.c_str( ), ios::in | ios::binary );

         if( !inpf )
            THROW_ODS_ERROR( "unable to open database tranlog file for input" );

         tranlog_info.read( inpf );

         if( tranlog_info.val_hash != *( ( uint32_t* )( p_impl->pwd_hash.data( ) ) ) )
            THROW_ODS_ERROR( "incorrect password" );

         using_tranlog = true;

         p_impl->is_corrupt = true;
      }
      else if( using_tranlog )
      {
         // NOTE: If using a transaction log then create a default tranlog header.
         log_stream logf( p_impl->tranlog_file_name.c_str( ), use_sync_write );

         tranlog_info.init_time = tranlog_info.sequence_new_tm = now;

         tranlog_info.write( logf );
      }
   }

   if( !p_impl->is_new )
   {
      p_impl->read_header_file_info( );

      // NOTE: Ensure encryption password is only provided when required.
      if( p_impl->rp_header_info->version & c_encrypted_flag )
      {
         if( !p_impl->is_read_only && !p_impl->is_encrypted )
            THROW_ODS_ERROR( "encrypted database requires password to unlock" );
      }
      else
      {
         if( p_impl->is_encrypted )
            THROW_ODS_ERROR( "password not applicable for unencrypted database" );
      }

      // NOTE: File corruption can be determined if we have an exclusive write lock but
      // then find that one or more writers/transactions appear to be currently active.
      if( ( w_mode == e_write_mode_exclusive )
       && ( p_impl->rp_header_info->num_trans || p_impl->rp_header_info->num_writers ) )
         p_impl->is_corrupt = true;

      if( p_impl->rp_header_info->init_tranlog )
      {
         if( !file_exists( p_impl->tranlog_file_name ) )
            THROW_ODS_ERROR( "database tranlog file does not exist" );

         ifstream inpf( p_impl->tranlog_file_name.c_str( ), ios::in | ios::binary );

         if( !inpf )
            THROW_ODS_ERROR( "unable to open database tranlog file for input" );

         tranlog_info.read( inpf );

         if( tranlog_info.version != p_impl->rp_header_info->version )
            THROW_ODS_ERROR( "database transaction log version mismatch" );

         if( tranlog_info.sequence != p_impl->rp_header_info->num_logs )
            THROW_ODS_ERROR( "database transaction log sequence mismatch" );

         if( tranlog_info.init_time != p_impl->rp_header_info->init_tranlog )
            THROW_ODS_ERROR( "database transaction log init time mismatch" );

         // NOTE: If the "sequence" and "init_time" match but the offset of the
         // transaction log is greater than expected then assumes that this log
         // contains one or more transactions that have yet to be processed and
         // therefore will require repairing to do so (this supports using just
         // the newer transaction log to update an older ODS DB).
         if( tranlog_info.entry_offs > p_impl->rp_header_info->tranlog_offset )
            p_impl->is_corrupt = true;

         p_impl->using_tranlog = true;
      }
      else if( using_tranlog )
         THROW_ODS_ERROR( "database was not created with a transaction log" );
   }
   else
   {
      // NOTE: Use "tranlog_info.version" to ensure that if a tx log was found
      // then a newly created header file is given its matching version number.
      p_impl->rp_header_info->version = tranlog_info.version;

      if( using_tranlog )
      {
         p_impl->using_tranlog = true;
         p_impl->rp_header_info->num_logs = tranlog_info.sequence;
         p_impl->rp_header_info->init_tranlog = tranlog_info.init_time;
      }

      p_impl->force_write_header_file_info( );
   }

   p_impl->rp_instances = new vector< ods* >( );

   p_impl->rp_instances->push_back( this );

   if( p_impl->is_new )
      DEBUG_LOG( "[opened new file]" );
   else
   {
      DEBUG_LOG( "[opened existing file]" );

      if( !file_exists( p_impl->data_file_name ) )
         THROW_ODS_ERROR( "could not find database data file" );
   }

   p_impl->cipher_type = determine_stream_cipher( p_impl->rp_header_info->version );

   p_impl->rp_open_store_ref_count = new int( 0 );

   p_impl->rp_session_create_total = new int64_t( 0 );
   p_impl->rp_session_revive_total = new int64_t( 0 );
   p_impl->rp_session_review_total = new int64_t( 0 );
   p_impl->rp_session_update_total = new int64_t( 0 );
   p_impl->rp_session_delete_total = new int64_t( 0 );

   p_impl->rp_ods_data_cache_buffer =
    new ods_data_cache_buffer( *this, p_impl->data_file_name, c_data_max_cache_items,
    c_data_items_per_region, c_data_num_cache_regions, true, true, ( !using_tranlog && use_sync_write ) );

   p_impl->rp_ods_index_cache_buffer = new ods_index_cache_buffer(
    p_impl->index_file_name, p_impl->rp_header_file->get_offset( ), c_index_max_cache_items,
    c_index_items_per_region, c_index_num_cache_regions, true, true, ( !using_tranlog && use_sync_write ) );

   auto_ptr< transaction_buffer > ap_trans_buffer( new transaction_buffer );

   auto_ptr< ods_trans_op_cache_buffer > ap_ods_trans_op_cache_buffer(
    new ods_trans_op_cache_buffer( c_trans_op_max_cache_items, c_trans_op_items_per_region ) );

   auto_ptr< ods_trans_data_cache_buffer > ap_ods_trans_data_cache_buffer(
    new ods_trans_data_cache_buffer( c_trans_data_max_cache_items, c_trans_data_items_per_region ) );

   p_impl->p_trans_buffer = ap_trans_buffer.release( );
   p_impl->p_ods_trans_op_cache_buffer = ap_ods_trans_op_cache_buffer.release( );
   p_impl->p_ods_trans_data_cache_buffer = ap_ods_trans_data_cache_buffer.release( );

   ap_impl.release( );

   okay = true;

   if( w_mode == e_write_mode_shared )
   {
      // NOTE: This is done in order to determine a
      // corrupted DB when using shared write mode.
      ods::bulk_read bulk_read( *this );
   }

   permit_copy = true;
}

ods::~ods( )
{
   guard lock_dtor( g_ods_lock );

   permit_copy = false;

   DEBUG_LOG( "ods::~ods( )" );
   {
      guard tmp_lock( *p_impl->rp_impl_lock );

      delete p_impl->p_trans_buffer;
      delete p_impl->p_ods_trans_op_cache_buffer;
      delete p_impl->p_ods_trans_data_cache_buffer;

      vector< ods* >::iterator iter;

      if( p_impl->rp_instances )
      {
         for( iter = p_impl->rp_instances->begin( ); iter != p_impl->rp_instances->end( ); ++iter )
         {
            if( *iter == this )
            {
               *iter = 0;
               break;
            }
         }
      }
#ifdef ODS_DEBUG
      if( *p_impl->rp_instances.get_ref( ) == 1 && p_impl->rp_header_file->is_locked_for_exclusive( ) )
         DEBUG_LOG( "********** releasing exclusive write use **********" );
#endif
   }

   delete p_impl;
}

bool ods::is_new( ) const
{
   return p_impl->is_new && !p_impl->is_corrupt;
}

bool ods::is_corrupt( ) const
{
   return p_impl->is_corrupt;
}

bool ods::has_progress( ) const
{
   return p_progress;
}

bool ods::is_encrypted( ) const
{
   return p_impl->is_encrypted;
}

bool ods::is_bulk_locked( ) const
{
   guard lock_impl( *p_impl->rp_impl_lock );

   return ( *p_impl->rp_bulk_mode >= impl::e_bulk_mode_read );
}

bool ods::is_bulk_read_locked( ) const
{
   guard lock_impl( *p_impl->rp_impl_lock );

   return ( *p_impl->rp_bulk_mode == impl::e_bulk_mode_read );
}

bool ods::is_bulk_write_locked( ) const
{
   guard lock_impl( *p_impl->rp_impl_lock );

   return ( *p_impl->rp_bulk_mode == impl::e_bulk_mode_write );
}

bool ods::is_thread_bulk_locked( ) const
{
   bool retval = false;

   guard lock_impl( *p_impl->rp_impl_lock );

   if( *p_impl->rp_bulk_mode >= impl::e_bulk_mode_read )
   {
      if( *p_impl->rp_bulk_mode == impl::e_bulk_mode_read )
         retval = ( *p_impl->rp_bulk_read_thread_id == current_thread_id( ) );
      else if( *p_impl->rp_bulk_mode == impl::e_bulk_mode_write )
         retval = ( *p_impl->rp_bulk_write_thread_id == current_thread_id( ) );
   }

   return retval;
}

bool ods::is_thread_bulk_read_locked( ) const
{
   bool retval = false;

   guard lock_impl( *p_impl->rp_impl_lock );

   if( *p_impl->rp_bulk_mode == impl::e_bulk_mode_read )
      retval = ( *p_impl->rp_bulk_read_thread_id == current_thread_id( ) );

   return retval;
}

bool ods::is_thread_bulk_write_locked( ) const
{
   bool retval = false;

   guard lock_impl( *p_impl->rp_impl_lock );

   if( *p_impl->rp_bulk_mode == impl::e_bulk_mode_write )
      retval = ( *p_impl->rp_bulk_write_thread_id == current_thread_id( ) );

   return retval;
}

bool ods::is_using_transaction_log( ) const
{
   return p_impl->using_tranlog;
}

int64_t ods::get_total_entries( ) const
{
   guard lock_impl( *p_impl->rp_impl_lock );

   return p_impl->rp_header_info->total_entries;
}

int64_t ods::get_session_review_total( ) const
{
   guard lock_impl( *p_impl->rp_impl_lock );

   return *p_impl->rp_session_review_total;
}

int64_t ods::get_session_create_total( ) const
{
   guard lock_impl( *p_impl->rp_impl_lock );

   return *p_impl->rp_session_create_total;
}

int64_t ods::get_session_revive_total( ) const
{
   guard lock_impl( *p_impl->rp_impl_lock );

   return *p_impl->rp_session_revive_total;
}

int64_t ods::get_session_update_total( ) const
{
   guard lock_impl( *p_impl->rp_impl_lock );

   return *p_impl->rp_session_update_total;
}

int64_t ods::get_session_delete_total( ) const
{
   guard lock_impl( *p_impl->rp_impl_lock );

   return *p_impl->rp_session_delete_total;
}

int64_t ods::get_transaction_id( ) const
{
   return p_impl->p_trans_buffer->tran_id;
}

int64_t ods::get_transaction_level( ) const
{
   return p_impl->trans_level;
}

int64_t ods::get_next_transaction_id( ) const
{
   guard lock_impl( *p_impl->rp_impl_lock );

   return p_impl->rp_header_info->transaction_id;
}

string ods::get_file_names( const char* p_ext, char sep, bool add_tranlog_always ) const
{
   return ods_file_names( p_impl->name, sep, add_tranlog_always || p_impl->using_tranlog );
}

void ods::reconstruct_database( progress* p_progress )
{
   if( !p_impl->using_tranlog )
      THROW_ODS_ERROR( "cannot reconstruct database unless using a transaction log" );
   else
   {
      restore_from_transaction_log( true, p_progress );

      p_impl->is_new = false;
      p_impl->is_corrupt = false;
   }
}

void ods::repair_corrupt_database( progress* p_progress )
{
   if( p_impl->is_corrupt )
   {
      if( !p_impl->using_tranlog )
         rollback_dead_transactions( p_progress );
      else
         restore_from_transaction_log( false, p_progress );

      p_impl->is_new = false;
      p_impl->is_corrupt = false;
   }
}

void ods::rewind_transactions(
 const string& label_or_txid, int64_t rewind_value, progress* p_progress )
{
   guard lock_write( write_lock );
   guard lock_read( read_lock );
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   if( p_impl->trans_level )
      THROW_ODS_ERROR( "cannot rewind transactions whilst in a transaction" );

   if( !p_impl->using_tranlog )
      THROW_ODS_ERROR( "cannot rewind transactions in a non-transaction log database" );

   if( !p_impl->rp_header_file->is_locked_for_exclusive( ) )
      THROW_ODS_ERROR( "cannot rewind transactions unless locked for exclusive write" );

   if( *p_impl->rp_bulk_level && ( *p_impl->rp_bulk_mode != impl::e_bulk_mode_write ) )
      THROW_ODS_ERROR( "cannot rewind transactions when bulk locked for dumping or reading" );

   if( !rewind_value && label_or_txid.empty( ) )
      THROW_ODS_ERROR( "neither id/label or rewind value specifified in rewind_transactions" );

   int64_t rewind_tx_id = 0;
   int64_t rewind_tx_time = 0;

   if( rewind_value < 0 )
      rewind_tx_id = p_impl->rp_header_info->transaction_id + rewind_value;
   else if( rewind_value > 0 )
      rewind_tx_time = rewind_value;

   if( !label_or_txid.empty( ) && ( rewind_tx_id || rewind_tx_time ) )
      THROW_ODS_ERROR( "invalid non-empty id/label when rewind value specified in rewind_transactions" );

   log_stream logf( p_impl->tranlog_file_name.c_str( ), use_sync_write );

   log_info tranlog_info;

   tranlog_info.read( logf );

   bool is_encrypted = p_impl->is_encrypted;

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !*p_impl->rp_bulk_level )
      ap_bulk_write.reset( new ods::bulk_write( *this, p_progress ) );

   if( tranlog_info.append_offs > tranlog_info.size_of( ) )
   {
      set< int64_t > entry_offsets;
      set< int64_t > committed_transactions;

      int64_t last_tx_time = 0;

      int64_t first_entry_offset = 0;
      int64_t largest_commit_offs = 0;

      int64_t append_offset = tranlog_info.append_offs;

      while( true )
      {
         int64_t entry_offset = logf.get_pos( );

         log_entry tranlog_entry;
         tranlog_entry.read( logf );

         int64_t tx_id = tranlog_entry.tx_id;
         int64_t next_offs = tranlog_entry.next_entry_offs;

         if( !first_entry_offset )
         {
            string next_label_or_txid;

            if( !tranlog_entry.label[ 0 ] )
               next_label_or_txid = to_string( tx_id );
            else
               next_label_or_txid = string( tranlog_entry.label );

            bool found = false;

            if( rewind_tx_id )
               found = ( tx_id == rewind_tx_id );
            else if( !rewind_tx_time )
               found = ( label_or_txid == next_label_or_txid );
            else
               found = ( tranlog_entry.tx_time > rewind_tx_time );

            if( found )
            {
               if( entry_offset < largest_commit_offs )
               {
                  logf.term( );
                  THROW_ODS_ERROR( "rewind point cannot be an overlapped transaction" );
               }

               tranlog_info.entry_offs = tranlog_entry.prior_entry_offs;
               tranlog_info.entry_time = last_tx_time;
               tranlog_info.append_offs = first_entry_offset = entry_offset;

               p_impl->rp_header_info->transaction_id = tx_id;

               p_impl->rp_header_info->tranlog_offset = tranlog_entry.prior_entry_offs;

               p_impl->rp_header_info->total_size_of_data = tranlog_entry.total_data_bytes;

               p_impl->rp_header_info->data_transform_id = tranlog_entry.data_transform_id;
               p_impl->rp_header_info->index_transform_id = tranlog_entry.index_transform_id;
            }
         }

         last_tx_time = tranlog_entry.tx_time;

         if( tranlog_entry.commit_offs > largest_commit_offs )
            largest_commit_offs = tranlog_entry.commit_offs;

         if( first_entry_offset )
         {
            entry_offsets.insert( entry_offset );

            if( tranlog_entry.commit_offs )
               committed_transactions.insert( tx_id );
         }

         if( !next_offs )
            break;

         logf.set_pos( next_offs );
      }

      if( !first_entry_offset )
      {
         logf.term( );

         string error( "transaction " );

         if( rewind_tx_id )
            error += "id " + to_string( rewind_tx_id );
         else if( rewind_tx_time )
            error += "time " + to_string( rewind_tx_time );
         else
            error += "id/label '" + label_or_txid + "'";

         error += " not found for database rewind";
         
         THROW_ODS_ERROR( error );
      }
      else if( !tranlog_info.entry_offs )
      {
         logf.term( );
         THROW_ODS_ERROR( "cannot rewind the very first database transaction" );
      }
      else if( !entry_offsets.empty( ) )
      {
         date_time dtm( date_time::local( ) );

         set< int64_t >::iterator si( entry_offsets.end( ) );

         while( true )
         {
            logf.set_pos( *--si );

            log_entry tranlog_entry;

            tranlog_entry.read( logf );

            int64_t current_pos = logf.get_pos( );

            if( current_pos == append_offset )
               break;

            // NOTE: If no log entry items exist for the current log entry then
            // the current file position will be the entry offset that followed
            // it so either continue with the previous entry or break if at the
            // first entry already.
            if( entry_offsets.count( current_pos ) )
            {
               if( si == entry_offsets.begin( ) )
                  break;
               else
                  continue;
            }

            int64_t tx_id = tranlog_entry.tx_id;

            map< int64_t, int64_t > freelist_removals;
            deque< pair< int64_t, int64_t > > freelist_additions;

            while( true )
            {
               log_entry_item tranlog_item;

               tranlog_item.read( logf );

               bool skip_data = false;

               if( committed_transactions.count( tx_id ) )
               {
                  ods_index_entry index_entry;

                  read_index_entry( index_entry, tranlog_item.index_entry_id );

                  bool write_entry = false;

                  int64_t old_tx_id = tx_id;

                  if( tranlog_item.has_old_tran_id( ) )
                     old_tx_id = tranlog_item.tx_oid;

                  if( tranlog_item.has_pos_and_size( ) )
                  {
                     if( !tranlog_item.is_post_op( ) )
                     {
                        int64_t dpos = 0;

                        if( !tranlog_item.has_old_pos( ) )
                           dpos = tranlog_item.data_pos;
                        else
                           dpos = tranlog_item.data_opos;

                        int64_t chunk = c_buffer_chunk_size;

                        char buffer[ c_buffer_chunk_size ];

                        set_write_data_pos( dpos, is_encrypted, is_encrypted );

                        for( int64_t j = 0; j < tranlog_item.data_size; j += chunk )
                        {
                           if( ( j + chunk ) > tranlog_item.data_size )
                              chunk = tranlog_item.data_size - j;

                           logf.read( ( unsigned char* )buffer, chunk );
                           write_data_bytes( buffer, chunk, is_encrypted, is_encrypted );

                           if( p_progress )
                           {
                              date_time now( date_time::local( ) );

                              uint64_t elapsed = seconds_between( dtm, now );

                              if( elapsed >= p_progress->num_seconds )
                              {
                                 dtm = now;
                                 p_progress->output_progress( "." );
                              }
                           }
                        }

                        write_entry = true;

                        if( index_entry.trans_flag == ods_index_entry::e_trans_free_list )
                           freelist_removals.insert(
                            make_pair( tranlog_item.index_entry_id + 1, index_entry.data.pos ) );

                        index_entry.data.pos = dpos;
                        index_entry.data.size = tranlog_item.data_size;
                        index_entry.trans_flag = ods_index_entry::e_trans_none;
                     }
                     else
                        skip_data = true;
                  }
                  else if( tranlog_item.is_create( ) )
                  {
                     if( index_entry.trans_flag == ods_index_entry::e_trans_free_list )
                        freelist_removals.insert(
                         make_pair( tranlog_item.index_entry_id + 1, index_entry.data.pos ) );

                     if( !old_tx_id )
                     {
                        if( tranlog_item.index_entry_id < p_impl->rp_header_info->total_entries )
                           p_impl->rp_header_info->total_entries = tranlog_item.index_entry_id;
                     }
                     else
                        freelist_additions.push_back( make_pair( tranlog_item.index_entry_id, old_tx_id ) );
                  }

                  if( write_entry )
                  {
                     index_entry.data.tran_op = 0;
                     index_entry.data.tran_id = old_tx_id;

                     write_index_entry( index_entry, tranlog_item.index_entry_id );
                  }
               }
               else if( tranlog_item.has_pos_and_size( ) )
                  skip_data = true;

               if( skip_data )
                  logf.set_pos( logf.get_pos( ) + tranlog_item.data_size );

               int64_t current_pos = logf.get_pos( );

               if( ( current_pos >= append_offset ) || entry_offsets.count( current_pos ) )
                  break;
            }

            int64_t next_pos = p_impl->rp_header_info->index_free_list;

            // NOTE: Any transaction entries that had ended up on the freelist
            // are first skipped (so the freelist ordering will be preserved).
            while( freelist_removals.count( next_pos ) )
               next_pos = freelist_removals[ next_pos ];

            // NOTE: Now process any freelist additions in their reverse order.
            while( !freelist_additions.empty( ) )
            {
               ods_index_entry index_entry;

               read_index_entry( index_entry, freelist_additions.back( ).first );

               index_entry.data.pos = next_pos;
               index_entry.data.size = 0;

               index_entry.data.tran_op = 0;
               index_entry.data.tran_id = freelist_additions.back( ).second;

               index_entry.trans_flag = ods_index_entry::e_trans_free_list;

               write_index_entry( index_entry, freelist_additions.back( ).first );

               if( p_progress )
               {
                  date_time now( date_time::local( ) );
                  uint64_t elapsed = seconds_between( dtm, now );

                  if( elapsed >= p_progress->num_seconds )
                  {
                     dtm = now;
                     p_progress->output_progress( "." );
                  }
               }

               next_pos = freelist_additions.back( ).first + 1;

               freelist_additions.pop_back( );
            }

            p_impl->rp_header_info->index_free_list = next_pos;

            if( si == entry_offsets.begin( ) )
               break;
         }

         // NOTE: Need to clear the "next_entry_offs" value for the new last entry.
         if( tranlog_info.entry_offs )
         {
            logf.set_pos( tranlog_info.entry_offs );

            log_entry tranlog_entry;
            tranlog_entry.read( logf );

            tranlog_entry.next_entry_offs = 0;
            logf.set_pos( tranlog_info.entry_offs );

            tranlog_entry.write( logf );
         }

         logf.set_pos( 0 );

         tranlog_info.write( logf );

         set_read_data_pos( -1 );

         data_and_index_write( true, is_encrypted );

         p_impl->rp_ods_data_cache_buffer->clear( );
         p_impl->rp_ods_index_cache_buffer->clear( );

         // NOTE: If no write has occurred then act as though
         // one has in order to ensure that the "num_writers"
         // will be correctly zeroed.
         if( !*p_impl->rp_has_changed )
         {
            *p_impl->rp_has_changed = true;
            ++p_impl->rp_header_info->num_writers;
         }

         p_impl->write_header_file_info( true );

         *p_impl->rp_has_changed = false;
      }
   }
   else
   {
      logf.term( );
      THROW_ODS_ERROR( "cannot rewind with an empty transaction log" );
   }
}

int64_t ods::get_size( const oid& id )
{
   guard lock_read( read_lock );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   DEBUG_LOG( "ods::get_size( )" );

   ods_index_entry index_entry;

   if( id.get_num( ) < 0 )
      return 0;

   bool found = false;

   int attempts = c_review_max_attempts;

   while( attempts-- )
   {
      // NOTE: Start file lock section.
      {
         guard tmp_lock( *p_impl->rp_impl_lock );

         ods::header_file_lock header_file_lock( *this );

         auto_ptr< ods::file_scope > ap_file_scope;

         if( !*p_impl->rp_bulk_level )
            ap_file_scope.reset( new ods::file_scope( *this ) );

         if( id.get_num( ) < p_impl->rp_header_info->total_entries )
         {
            read_index_entry( index_entry, id.get_num( ) );
            found = true;
            break;
         }
      }
      // NOTE: Finish file lock section.

      msleep( c_review_attempt_sleep_time );
   }

   if( !found )
      THROW_ODS_ERROR( "cannot get_size (max. attempts exceeded)" );

   if( ( index_entry.trans_flag != ods_index_entry::e_trans_none )
    && ( index_entry.data.tran_id == p_impl->p_trans_buffer->tran_id ) )
   {
      transaction_op op;
      read_transaction_op( op, index_entry.data.tran_op - 1 );

      return op.data.size;
   }
   else
      return index_entry.data.size;
}

void ods::destroy( const oid& id )
{
   if( id.get_num( ) < 0 )
      return;

   guard lock_write( write_lock );
   guard lock_read( read_lock );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state..." );

   if( p_impl->is_read_only )
      THROW_ODS_ERROR( "attempt to perform destroy when database was opened for read only access" );

   DEBUG_LOG( "ods::destroy( )" );

   if( !bulk_operation_none_or_write( ) )
      THROW_ODS_ERROR( "cannot destroy when database is bulk locked for dumping or reading" );

   ods_index_entry index_entry;

   bool deleted = false;

   int attempts = c_delete_max_attempts;

   while( attempts-- )
   {
      // NOTE: Start file lock section.
      {
         guard tmp_lock( *p_impl->rp_impl_lock );

         ods::header_file_lock header_file_lock( *this );

         auto_ptr< ods::file_scope > ap_file_scope;

         if( !*p_impl->rp_bulk_level )
            ap_file_scope.reset( new ods::file_scope( *this ) );

         int64_t tx_id = p_impl->p_trans_buffer->tran_id;

         if( id.get_num( ) < p_impl->rp_header_info->total_entries )
         {
            unsigned char flags = c_log_entry_item_op_destroy;

            read_index_entry( index_entry, id.get_num( ) );

            int64_t old_tx_id = index_entry.data.tran_id;

            if( index_entry.lock_flag == ods_index_entry::e_lock_none )
            {
               if( ( index_entry.trans_flag == ods_index_entry::e_trans_none )
                || ( p_impl->trans_level && ( index_entry.data.tran_id == tx_id )
                && ( index_entry.trans_flag != ods_index_entry::e_trans_delete ) ) )
               {
                  bool skip_log_entry = false;

                  if( p_impl->trans_level )
                  {
                     transaction_op op;

                     op.type = transaction_op::e_op_type_destroy;

                     op.data.id = id;
                     op.data.old_tran_op = index_entry.data.tran_op;

                     write_transaction_op( op );

                     if( index_entry.data.tran_id == tx_id )
                        skip_log_entry = true;

                     index_entry.data.tran_op = 0;
                     index_entry.data.tran_id = tx_id;

                     if( !skip_log_entry && p_impl->using_tranlog )
                        append_log_entry_item( id.get_num( ), index_entry, flags, true, old_tx_id, 0, p_progress );

                     index_entry.trans_flag = ods_index_entry::e_trans_delete;
                  }
                  else
                  {
                     index_entry.data.tran_op = 0;

                     index_entry.data.tran_id = p_impl->rp_header_info->transaction_id;

                     if( p_impl->using_tranlog )
                     {
                        p_impl->tranlog_offset = append_log_entry( p_impl->rp_header_info->transaction_id );

                        append_log_entry_item( id.get_num( ), index_entry, flags, false, old_tx_id, 0, p_progress );

                        if( !p_impl->rp_header_info->tranlog_offset )
                           p_impl->rp_header_info->tranlog_offset = p_impl->tranlog_offset;
                     }

                     if( ( index_entry.data.pos + index_entry.data.size )
                      == p_impl->rp_header_info->total_size_of_data )
                        p_impl->rp_header_info->total_size_of_data -= index_entry.data.size;

                     index_entry.data.pos = p_impl->rp_header_info->index_free_list;
                     index_entry.data.size = 0;

                     index_entry.trans_flag = ods_index_entry::e_trans_free_list;

                     p_impl->rp_header_info->index_free_list = id.get_num( ) + 1;
                  }

                  write_index_entry( index_entry, id.get_num( ) );

                  if( !skip_log_entry )
                     ++p_impl->rp_header_info->index_transform_id;

                  if( !p_impl->trans_level )
                     ++p_impl->rp_header_info->transaction_id;
                  else
                  {
                     ++p_impl->p_trans_buffer->levels.top( ).op_count;
                     ++p_impl->total_trans_op_count;
                  }

                  deleted = true;

                  ++( *p_impl->rp_session_delete_total );
               }
            }
         }

         if( deleted )
            break;
      }
      // NOTE: Finish file lock section.

      msleep( c_delete_attempt_sleep_time );
   }

   if( !deleted )
      THROW_ODS_ERROR( "cannot delete (max. attempts exceeded)" );

   if( p_impl->trans_level && trans_write_ops_buffer_num != -1 )
   {
      p_impl->p_ods_trans_op_cache_buffer->put( trans_write_ops_buffer, trans_write_ops_buffer_num );

      trans_write_ops_buffer_num = -1;
      had_interim_trans_op_write = true;
   }
}

string ods::backup_database( const char* p_ext, char sep )
{
   guard lock_write( write_lock );
   guard lock_read( read_lock );
   guard lock_impl( *p_impl->rp_impl_lock );

   string retval;

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   if( p_impl->trans_level )
      THROW_ODS_ERROR( "cannot backup a database whilst in a transaction" );

   if( !p_impl->rp_header_file->is_locked_for_exclusive( ) )
      THROW_ODS_ERROR( "cannot backup a database unless locked for exclusive write" );

   if( *p_impl->rp_bulk_level && ( *p_impl->rp_bulk_mode != impl::e_bulk_mode_write ) )
      THROW_ODS_ERROR( "cannot backup a database when bulk locked for dumping or reading" );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !*p_impl->rp_bulk_level )
      ap_bulk_write.reset( new ods::bulk_write( *this ) );

   string ext( p_ext ? p_ext : c_sav_file_name_ext );

   string backup_data_file_name( p_impl->data_file_name + ext );
   string backup_index_file_name( p_impl->index_file_name + ext );
   string backup_header_file_name( p_impl->header_file_name + ext );
   string backup_tranlog_file_name( p_impl->tranlog_file_name + ext );

   ifstream data_ifs( p_impl->data_file_name.c_str( ), ios::in | ios::binary );
   ifstream index_ifs( p_impl->index_file_name.c_str( ), ios::in | ios::binary );

   if( !data_ifs || !index_ifs )
      THROW_ODS_ERROR( "unable to open data and/or index input files for database backup" );

   ofstream data_ofs( backup_data_file_name.c_str( ), ios::out | ios::binary );
   ofstream index_ofs( backup_index_file_name.c_str( ), ios::out | ios::binary );
   ofstream header_ofs( backup_header_file_name.c_str( ), ios::out | ios::binary );

   if( !data_ofs || !index_ofs || !header_ofs )
      THROW_ODS_ERROR( "unable to open data, index and/or header output files for database backup" );

   copy_stream( data_ifs, data_ofs );
   copy_stream( index_ifs, index_ofs );

   data_ofs.flush( );

   if( !data_ofs.good( ) )
      THROW_ODS_ERROR( "unexpected bad data output stream in database backup" );

   retval = backup_data_file_name;

   index_ofs.flush( );

   if( !index_ofs.good( ) )
      THROW_ODS_ERROR( "unexpected bad index output stream in database backup" );

   retval += sep + backup_index_file_name;

   header_ofs.write( ( const char* )p_impl->rp_header_info.get( ), sizeof( header_info ) );

   header_ofs.flush( );

   if( !header_ofs.good( ) )
      THROW_ODS_ERROR( "unexpected bad header output stream in database backup" );

   retval += sep + backup_header_file_name;

   if( p_impl->using_tranlog )
   {
      ifstream tranlog_ifs( p_impl->tranlog_file_name.c_str( ), ios::in | ios::binary );

      if( !tranlog_ifs )
         THROW_ODS_ERROR( "unable to open tranlog input file for database backup" );

      ofstream tranlog_ofs( backup_tranlog_file_name.c_str( ), ios::out | ios::binary );

      if( !tranlog_ofs )
         THROW_ODS_ERROR( "unable to open tranlog output file for database backup" );

      copy_stream( tranlog_ifs, tranlog_ofs );

      tranlog_ofs.flush( );

      if( !tranlog_ofs.good( ) )
         THROW_ODS_ERROR( "unexpected bad tranlog output stream in database backup" );

      retval += sep + backup_tranlog_file_name;
   }

   return retval;
}

void ods::move_free_data_to_end( progress* p_progress )
{
   guard lock_write( write_lock );
   guard lock_read( read_lock );
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   if( p_impl->trans_level )
      THROW_ODS_ERROR( "cannot move free data to end whilst in a transaction" );

   if( !p_impl->rp_header_file->is_locked_for_exclusive( ) )
      THROW_ODS_ERROR( "cannot move free data to end unless locked for exclusive write" );

   if( *p_impl->rp_bulk_level && ( *p_impl->rp_bulk_mode != impl::e_bulk_mode_write ) )
      THROW_ODS_ERROR( "cannot move free data to end when bulk locked for dumping or reading" );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !*p_impl->rp_bulk_level )
      ap_bulk_write.reset( new ods::bulk_write( *this, p_progress ) );

   ods_index_entry_pos entry;
   set< ods_index_entry_pos > entries;

   ods_index_entry index_entry;

   date_time dtm( date_time::local( ) );

   int64_t total_entries = p_impl->rp_header_info->total_entries;
   int64_t total_size_of_data = p_impl->rp_header_info->total_size_of_data;

   // FUTURE: This approach requires keeping in memory an "ods_index_entry_pos" item for every active
   // "index_entry" in the entire database. If a "deque" (rather than a "set") was used then it could
   // be limited to a maximum size with potentially multiple passes being performed across all of the
   // index entries that would then only insert items for those index entries whose "pos" is actually
   // greater than the last item from the previous pass.
   for( int64_t i = 0; i < total_entries; i++ )
   {
      if( p_progress )
      {
         date_time now( date_time::local( ) );

         uint64_t elapsed = seconds_between( dtm, now );

         if( elapsed >= p_progress->num_seconds )
         {
            dtm = now;
            p_progress->output_progress( "." );
         }
      }

      read_index_entry( index_entry, i );

      if( index_entry.lock_flag != ods_index_entry::e_lock_none )
         THROW_ODS_ERROR( "cannot move free data to end when one or more entries have been locked" );

      if( index_entry.trans_flag != ods_index_entry::e_trans_none
       && index_entry.trans_flag != ods_index_entry::e_trans_free_list )
         THROW_ODS_ERROR( "cannot move free data to end whilst one or more dead transactions exist" );

      if( index_entry.trans_flag != ods_index_entry::e_trans_free_list )
      {
         entry.id = i;
         entry.pos = index_entry.data.pos;
         entry.size = index_entry.data.size;
         entry.tran_id = index_entry.data.tran_id;

         entries.insert( entry );
      }
   }

   int64_t num_moved = 0;

   int64_t log_entry_offs = 0;
   int64_t old_append_offs = 0;

   int64_t tran_id = p_impl->rp_header_info->transaction_id;

   set< ods_index_entry_pos >::const_iterator ci, end;

   if( p_impl->using_tranlog )
   {
      log_entry_offs = append_log_entry( p_impl->rp_header_info->transaction_id++, &old_append_offs );

      log_stream logf( p_impl->tranlog_file_name.c_str( ), use_sync_write );

      logf.set_pos( old_append_offs );

      bool is_encrypted = p_impl->is_encrypted;

      set_read_data_pos( 0, is_encrypted, is_encrypted );

      int64_t new_pos = 0;
      int64_t read_pos = 0;

      for( ci = entries.begin( ), end = entries.end( ); ci != end; ++ci )
      {
         int64_t next_id = ci->id;
         int64_t next_pos = ci->pos;
         int64_t next_size = ci->size;
         int64_t next_tran_id = ci->tran_id;

         if( next_pos < read_pos )
            THROW_ODS_ERROR( "unexpected next_pos < read_pos at " STRINGIZE( __LINE__ ) );

         adjust_read_data_pos( next_pos - read_pos, is_encrypted );

         if( next_pos != new_pos )
         {
            ++num_moved;

            int64_t chunk = c_buffer_chunk_size;

            char buffer[ c_buffer_chunk_size ];

            log_entry_item tranlog_item;

            tranlog_item.flags = ( c_log_entry_item_op_store | c_log_entry_item_flag_has_old_pos );

            tranlog_item.tx_oid = next_tran_id;
            tranlog_item.index_entry_id = next_id;

            tranlog_item.data_pos = new_pos;
            tranlog_item.data_opos = next_pos;
            tranlog_item.data_size = next_size;

            tranlog_item.write( logf );

            for( int64_t j = 0; j < next_size; j += chunk )
            {
               if( ( j + chunk ) > next_size )
                  chunk = next_size - j;

               read_data_bytes( buffer, chunk, is_encrypted );

               logf.write( ( unsigned char* )buffer, chunk );

               if( p_progress )
               {
                  date_time now( date_time::local( ) );

                  uint64_t elapsed = seconds_between( dtm, now );

                  if( elapsed >= p_progress->num_seconds )
                  {
                     dtm = now;
                     p_progress->output_progress( "." );
                  }
               }
            }

            read_pos = next_pos + next_size;
         }
         else
            read_pos = next_pos;

         new_pos += next_size;
      }

      int64_t offs = logf.get_pos( );

      logf.set_pos( 0 );

      log_info tranlog_info;
      tranlog_info.read( logf );

      tranlog_info.append_offs = offs;

      logf.set_pos( 0 );
      tranlog_info.write( logf );
   }

   set_read_data_pos( 0, true );

   int64_t new_pos = 0;
   int64_t read_pos = 0;
   int64_t actual_size = 0;

   for( ci = entries.begin( ), end = entries.end( ); ci != end; ++ci )
   {
      int64_t next_id = ci->id;
      int64_t next_pos = ci->pos;
      int64_t next_size = ci->size;

      if( next_pos < read_pos )
         THROW_ODS_ERROR( "unexpected next_pos < read_pos at " STRINGIZE( __LINE__ ) );

      adjust_read_data_pos( next_pos - read_pos );

      if( next_pos != new_pos )
      {
         int64_t chunk = c_buffer_chunk_size;

         char buffer[ c_buffer_chunk_size ];

         set_write_data_pos( new_pos );

         for( int64_t j = 0; j < next_size; j += chunk )
         {
            if( ( j + chunk ) > next_size )
               chunk = next_size - j;

            read_data_bytes( buffer, chunk );
            write_data_bytes( buffer, chunk );

            if( p_progress )
            {
               date_time now( date_time::local( ) );
               uint64_t elapsed = seconds_between( dtm, now );

               if( elapsed >= p_progress->num_seconds )
               {
                  dtm = now;
                  p_progress->output_progress( "." );
               }
            }
         }

         read_index_entry( index_entry, next_id );

         index_entry.data.pos = new_pos;
         index_entry.data.tran_id = tran_id;

         write_index_entry( index_entry, next_id );

         ++p_impl->rp_header_info->index_transform_id;

         read_pos = next_pos + next_size;
      }
      else
         read_pos = next_pos;

      new_pos += next_size;
      actual_size += next_size;
   }

   if( log_entry_offs )
   {
      log_stream logf( p_impl->tranlog_file_name.c_str( ), use_sync_write );

      log_entry tranlog_entry;

      logf.set_pos( log_entry_offs );

      tranlog_entry.read( logf );

      tranlog_entry.commit_offs = old_append_offs;
      tranlog_entry.commit_items = num_moved;

      logf.set_pos( log_entry_offs );

      tranlog_entry.write( logf );

      p_impl->rp_header_info->tranlog_offset = log_entry_offs;
   }

   if( num_moved )
      ++p_impl->rp_header_info->data_transform_id;

   p_impl->rp_header_info->total_size_of_data = actual_size;

   data_and_index_write( );

   p_impl->force_write_header_file_info( );
}

void ods::truncate_log( const char* p_ext, bool reset, progress* p_progress )
{
   guard lock_write( write_lock );
   guard lock_read( read_lock );
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   if( p_impl->trans_level )
      THROW_ODS_ERROR( "cannot truncate log whilst in a transaction" );

   if( !p_impl->using_tranlog )
      THROW_ODS_ERROR( "cannot truncate log unless using a transaction log" );

   if( !p_impl->rp_header_file->is_locked_for_exclusive( ) )
      THROW_ODS_ERROR( "cannot truncate log unless locked for exclusive write" );

   if( *p_impl->rp_bulk_level && ( *p_impl->rp_bulk_mode != impl::e_bulk_mode_write ) )
      THROW_ODS_ERROR( "cannot truncate log when bulk locked for dumping or reading" );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !*p_impl->rp_bulk_level )
      ap_bulk_write.reset( new ods::bulk_write( *this, p_progress ) );

   log_info tranlog_info;

   string log_file_name( p_impl->tranlog_file_name );

   // NOTE: Empty code block for scope purposes.
   {
      ifstream tranlog_ifs( log_file_name.c_str( ), ios::in | ios::binary );

      if( !tranlog_ifs )
         THROW_ODS_ERROR( "unable to open tranlog file for input" );

      tranlog_info.read( tranlog_ifs );

      if( !tranlog_info.entry_offs )
         THROW_ODS_ERROR( "cannot truncate an emtpy transaction log" );
   }

   string ext( p_ext ? p_ext : "" );

   if( ext.empty( ) )
      ext = "." + to_string( tranlog_info.sequence );

   string sequence_file_name( log_file_name + ext );

   if( !file_rename( log_file_name, sequence_file_name ) )
      throw runtime_error( "unable to rename '" + log_file_name + "' to '" + sequence_file_name + "'" );

   int64_t dtm = time( 0 );

   // NOTE: Empty code block for scope purposes.
   {
      log_stream logf( log_file_name.c_str( ), use_sync_write );

      if( !reset )
         ++tranlog_info.sequence;
      else
      {
         tranlog_info.sequence = 1;
         tranlog_info.init_time = dtm;
      }

      tranlog_info.entry_offs = 0;
      tranlog_info.entry_time = 0;
      tranlog_info.append_offs = tranlog_info.size_of( );

      if( reset )
      {
         tranlog_info.sequence_old_tm = 0;
         tranlog_info.sequence_new_tm = dtm;
      }
      else
      {
         tranlog_info.sequence_old_tm = tranlog_info.sequence_new_tm;
         tranlog_info.sequence_new_tm = dtm;
      }

      tranlog_info.write( logf );

      if( reset )
      {
         p_impl->rp_header_info->init_tranlog = dtm;
         p_impl->rp_header_info->transaction_id = 1;
         p_impl->rp_header_info->tranlog_offset = tranlog_info.append_offs;
      }

      p_impl->rp_header_info->num_logs = tranlog_info.sequence;

      p_impl->force_write_header_file_info( );
   }

   if( reset )
   {
      bool is_encrypted = p_impl->is_encrypted;

      log_stream logf( log_file_name.c_str( ), use_sync_write );

      tranlog_info.read( logf );

      log_entry tranlog_entry;

      tranlog_entry.tx_id = 1;
      tranlog_entry.tx_time = dtm;

      tranlog_entry.write( logf );

      tranlog_info.append_offs = logf.get_pos( );

      logf.set_pos( 0 );

      tranlog_info.write( logf );

      logf.set_pos( tranlog_info.append_offs );

      p_impl->rp_header_info->index_free_list = 0;

      ods_index_entry index_entry;

      int64_t total_bytes = 0;
      int64_t total_entries = p_impl->rp_header_info->total_entries;

      int64_t commit_items = 0;
      int64_t commit_offset = tranlog_info.append_offs;

      for( int64_t i = 0; i < total_entries; i++ )
      {
         if( p_progress )
         {
            int64_t now = time( 0 );

            uint64_t elapsed = ( now - dtm );

            if( elapsed >= 1 )
            {
               dtm = now;
               p_progress->output_progress( "." );
            }
         }

         unsigned char flags = c_log_entry_item_flag_is_post_op;

         ++commit_items;

         read_index_entry( index_entry, i );

         index_entry.data.tran_id = 1;

         if( index_entry.trans_flag != ods_index_entry::e_trans_free_list )
            flags |= c_log_entry_item_op_store;
         else
            flags |= c_log_entry_item_op_destroy;

         log_entry_item tranlog_item;

         tranlog_item.flags = flags;

         tranlog_item.index_entry_id = i;

         if( tranlog_item.has_pos_and_size( ) )
         {
            tranlog_item.data_pos = index_entry.data.pos;
            tranlog_item.data_size = index_entry.data.size;

            int64_t pos_and_size = index_entry.data.pos + index_entry.data.size;

            if( pos_and_size > total_bytes )
               total_bytes = pos_and_size;
         }

         tranlog_item.write( logf );

         if( tranlog_item.has_pos_and_size( ) )
         {
            set_read_data_pos( index_entry.data.pos, true, is_encrypted );

            int64_t chunk = c_buffer_chunk_size;

            char buffer[ c_buffer_chunk_size ];

            for( int64_t j = 0; j < index_entry.data.size; j += chunk )
            {
               if( j + chunk > index_entry.data.size )
                  chunk = index_entry.data.size - j;

               read_data_bytes( buffer, chunk, is_encrypted );

               logf.write( ( unsigned char* )buffer, chunk );

               if( p_progress )
               {
                  int64_t now = time( 0 );

                  uint64_t elapsed = ( now - dtm );

                  if( elapsed >= 1 )
                  {
                     dtm = now;
                     p_progress->output_progress( "." );
                  }
               }
            }

            if( is_encrypted )
               set_read_data_pos( -1 );
         }
         else
         {
            index_entry.data.pos = p_impl->rp_header_info->index_free_list;
            index_entry.data.size = 0;

            index_entry.trans_flag = ods_index_entry::e_trans_free_list;

            p_impl->rp_header_info->index_free_list = i + 1;
         }

         write_index_entry( index_entry, i );

         tranlog_info.append_offs = logf.get_pos( );
      }

      logf.set_pos( p_impl->rp_header_info->tranlog_offset );

      tranlog_entry.read( logf );

      tranlog_entry.commit_offs = commit_offset;
      tranlog_entry.commit_items = commit_items;

      logf.set_pos( p_impl->rp_header_info->tranlog_offset );

      tranlog_entry.write( logf );

      logf.set_pos( 0 );

      tranlog_info.entry_offs = p_impl->rp_header_info->tranlog_offset;

      tranlog_info.write( logf );

      p_impl->rp_header_info->transaction_id = 2;

      p_impl->rp_header_info->data_transform_id = 1;
      p_impl->rp_header_info->index_transform_id = 1;

      p_impl->rp_header_info->total_size_of_data = total_bytes;

      p_impl->force_write_header_file_info( );

      // FUTURE: Perhaps an option should determine whether to remove
      // the original log before or after the creation of the new log
      // (depending upon the amount of free disk space available).
      file_remove( sequence_file_name );
   }
}

void ods::compress_and_reset_tx_log( progress* p_progress )
{
   guard lock_write( write_lock );
   guard lock_read( read_lock );
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   if( p_impl->trans_level )
      THROW_ODS_ERROR( "cannot truncate log whilst in a transaction" );

   if( !p_impl->rp_header_file->is_locked_for_exclusive( ) )
      THROW_ODS_ERROR( "cannot truncate log unless locked for exclusive write" );

   if( *p_impl->rp_bulk_level && ( *p_impl->rp_bulk_mode != impl::e_bulk_mode_write ) )
      THROW_ODS_ERROR( "cannot truncate log when bulk locked for dumping or reading" );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !*p_impl->rp_bulk_level )
      ap_bulk_write.reset( new ods::bulk_write( *this, p_progress ) );

   move_free_data_to_end( p_progress );

   if( p_impl->using_tranlog )
      truncate_log( "", true, p_progress );

   clear_cache_statistics( );
}

void ods::clear_cache_statistics( )
{
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   p_impl->rp_ods_data_cache_buffer->clear_statistics( );
   p_impl->rp_ods_index_cache_buffer->clear_statistics( );
}

string ods::get_cache_hit_ratios( ) const
{
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   float data_ratio = p_impl->rp_ods_data_cache_buffer->get_item_hit_ratio( ) * 100.0;
   float index_ratio = p_impl->rp_ods_index_cache_buffer->get_item_hit_ratio( ) * 100.0;

   ostringstream osstr;

   osstr << "data: " << setfill( '0' ) << ffmt( 1, 2 ) << data_ratio
    << "%, index: " << setfill( '0' ) << ffmt( 1, 2 ) << index_ratio << '%';

   return osstr.str( );
}

void ods::dump_file_info( ostream& os, bool omit_dtms ) const
{
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   string extra;

   if( p_impl->rp_header_info->version & c_encrypted_flag )
      extra += " (encrypted)";

   if( *p_impl->rp_bulk_level
    && ( p_impl->rp_header_info->num_trans || p_impl->rp_header_info->num_writers ) )
      extra += " *** possibly corrupted file detected ***";

   os << "Version: " << format_version( p_impl->rp_header_info->version ) << extra
    << "\nNum Logs = " << p_impl->rp_header_info->num_logs
    << "\nNum Trans = " << p_impl->rp_header_info->num_trans
    << "\nNum Writers = " << p_impl->rp_header_info->num_writers;

   if( !omit_dtms )
      os << "\nInit Tranlog = " << p_impl->rp_header_info->init_tranlog;

   os << "\nTotal Entries = " << p_impl->rp_header_info->total_entries
    << "\nTranlog Offset = " << p_impl->rp_header_info->tranlog_offset
    << "\nTransaction Id = " << p_impl->rp_header_info->transaction_id
    << "\nIndex Free List = " << ( p_impl->rp_header_info->index_free_list ?
    to_string( p_impl->rp_header_info->index_free_list - 1 ) : to_string( "n/a" ) )
    << "\nTotal Size of Data = " << p_impl->rp_header_info->total_size_of_data
    << "\nData Transformation Id = " << p_impl->rp_header_info->data_transform_id
    << "\nIndex Transformation Id = " << p_impl->rp_header_info->index_transform_id << endl;

   int64_t found = p_impl->rp_ods_index_cache_buffer->get_file_size( );
   int64_t expected = ods_index_entry::get_size_of( ) * p_impl->rp_header_info->total_entries;

   if( p_impl->rp_header_info->total_entries % c_index_items_per_item )
      expected += ods_index_entry::get_size_of( ) *
       ( c_index_items_per_item - ( p_impl->rp_header_info->total_entries % c_index_items_per_item ) );

   if( found != expected )
      os << "Unexpected index file size: (expected = " << expected << ", found = " << found << ")" << endl;
}

void ods::dump_free_list( ostream& os )
{
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   ods_index_entry index_entry;

   if( !p_impl->rp_header_info->index_free_list )
      os << "No freelist entries." << endl;
   else
   {
      int64_t count = 0;

      int64_t last = 0;
      int64_t next = p_impl->rp_header_info->index_free_list;

      os << "First freelist entry = " << ( p_impl->rp_header_info->index_free_list - 1 ) << '\n';

      os << "Iterating over freelist...";

      while( next )
      {
         ++count;

         read_index_entry( index_entry, next - 1 );

         last = next;
         next = index_entry.data.pos;

         if( next == last )
            THROW_ODS_ERROR( "invalid freelist (next == last)" );
      }

      os << "(OK)";
      os << "\nFinal freelist entry = " << ( last - 1 );
      os << "\nTotal freelist entries = " << count << endl;
   }
}

void ods::dump_index_entry( ostream& os, int64_t num )
{
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   ods_index_entry index_entry;

   if( num >= p_impl->rp_header_info->total_entries )
      os << "error: max. entry num = " << ( p_impl->rp_header_info->total_entries - 1 ) << endl;
   else
   {
      read_index_entry( index_entry, num );
      index_entry.dump_entry( os, num );
   }
}

void ods::dump_instance_data( ostream& os, int64_t num, bool only_pos_and_size )
{
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   ods_index_entry index_entry;

   if( num >= p_impl->rp_header_info->total_entries )
      os << "error: max. entry num = " << ( p_impl->rp_header_info->total_entries - 1 ) << endl;
   else
   {
      read_index_entry( index_entry, num );

      if( index_entry.trans_flag == ods_index_entry::e_trans_free_list )
      {
         os << "(freelist entry)";
         if( index_entry.data.pos )
            os << " link: " << hex
             << setw( sizeof( int64_t ) * 2 ) << setfill( '0' ) << ( index_entry.data.pos - 1 ) << dec;
         else
            os << " link: <at end>";
         os << "\n";
      }
      else
      {
         if( !only_pos_and_size )
         {
            int64_t chunk = 16;
            unsigned char buffer[ 16 ];

            os << hex;

            set_read_data_pos( index_entry.data.pos );

            for( int64_t i = 0; i < index_entry.data.size; i += chunk )
            {
               if( i + chunk > index_entry.data.size )
                  chunk = index_entry.data.size - i;
               read_data_bytes( ( char* )buffer, chunk );

               os << hex << setw( sizeof( int64_t ) * 2 )
                << setfill( '0' ) << ( index_entry.data.pos + i ) << "  ";

               for( int j = 0; j < chunk; j++ )
                  os << setw( 2 ) << setfill( '0' ) << ( unsigned )buffer[ j ] << " ";

               if( chunk < 16 )
                  os << string( ( 16 - chunk ) * 3, ' ' );

               os << ' ';
               for( int j = 0; j < chunk; j++ )
               {
                  if( !is_print( buffer[ j ] ) )
                     os << ".";
                  else
                     os << buffer[ j ];
               }
               os << "\n";
            }

            os << dec;
         }
      }
   }
}

void ods::dump_transaction_log( ostream& os, bool omit_dtms,
 bool header_only, string* p_entry_ranges, bool skip_header, bool entries_are_condensed ) const
{
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   fstream fs;
   fs.open( p_impl->tranlog_file_name.c_str( ), ios::in | ios::binary );

   if( !fs )
      THROW_ODS_ERROR( "unable to open transaction log '" + p_impl->tranlog_file_name + "' in dump_transaction_log" );

   log_info tranlog_info;
   tranlog_info.read( fs );

   if( header_only || !skip_header )
      tranlog_info.dump( os, omit_dtms );

   if( !header_only && tranlog_info.entry_offs )
   {
      bool entries_are_tx_ids = false;

      int64_t tx_id_start = 0;
      int64_t tx_id_finish = 0;

      map< int64_t, int64_t > range_pairs;

      // NOTE: Optionally restrict the output to a map of log entry item ranges
      // or to a single tx or to a single range of txs (where 0 is the last tx).
      if( p_entry_ranges && !p_entry_ranges->empty( ) )
      {
         string entries( *p_entry_ranges );

         if( entries[ 0 ] == ':' )
         {
            entries.erase( 0, 1 );

            if( entries.empty( ) || entries == "*" || entries == "all" )
               entries = "1+0";

            bool is_from_end = false;

            if( !entries.empty( ) && entries[ 0 ] == '-' )
            {
               is_from_end = true;
               entries.erase( 0, 1 );
            }

            string::size_type pos = entries.find_first_of( "-+" );

            string first_tx_id( entries.substr( 0, pos ) );

            if( is_from_end || ( first_tx_id == "0" ) )
            {
               int64_t num_from_end = from_string< int64_t >( first_tx_id ) + 1;

               if( num_from_end >= get_next_transaction_id( ) )
                  throw runtime_error( "tx id entry info " + p_entry_ranges->substr( 1 ) + " is out of range" );

               tx_id_start = tx_id_finish = ( get_next_transaction_id( ) - num_from_end );

               if( tx_id_start && ( pos != string::npos ) )
               {
                  tx_id_start = tx_id_finish - from_string< int64_t >( entries.substr( pos + 1 ) );

                  if( tx_id_start < 1 )
                     tx_id_start = 1;
               }
            }
            else
            {
               tx_id_start = tx_id_finish = from_string< int64_t >( first_tx_id );

               if( pos != string::npos )
               {
                  entries.erase( 0, pos + 1 );

                  if( entries == "0" )
                     tx_id_finish = 0;
                  else
                     tx_id_finish = tx_id_start + from_string< int64_t >( entries );
               }
            }
         }
         else
         {
            if( entries_are_condensed )
               split_range_pairs( entries, range_pairs );
            else
               split_and_condense_range_pairs( entries, range_pairs, get_total_entries( ) );
         }
      }

      if( !skip_header )
         os << '\n';

      bool first_entry = true;

      while( true )
      {
         int64_t offs = fs.tellg( );

         log_entry tranlog_entry;

         tranlog_entry.read( fs );

         int64_t next_offs = tranlog_entry.next_entry_offs;

         if( !next_offs )
            next_offs = tranlog_info.append_offs;

         if( tx_id_start || tx_id_finish )
         {
            if( ( tx_id_start && ( tranlog_entry.tx_id < tx_id_start ) )
             || ( tx_id_finish && ( tranlog_entry.tx_id > tx_id_finish ) ) )
            {
               fs.seekg( next_offs, ios::beg );

               if( fs.tellg( ) > tranlog_info.entry_offs )
                  break;

               continue;
            }
         }

         bool first_item = true;
         ostringstream osstr;

         if( first_entry )
            first_entry = false;
         else
            osstr << '\n';

         tranlog_entry.dump( osstr, offs, omit_dtms );

         // NOTE: If dumping all then also include empty txs.
         if( !p_entry_ranges && ( fs.tellg( ) == next_offs ) )
            os << osstr.str( );

         while( fs.tellg( ) < next_offs )
         {
            int64_t offs = fs.tellg( );

            log_entry_item tranlog_item;
            tranlog_item.read( fs );

            bool dump_item = true;

            if( !range_pairs.empty( ) )
            {
               map< int64_t, int64_t >::iterator i = range_pairs.lower_bound( tranlog_item.index_entry_id );

               if( i == range_pairs.end( )
                || ( i != range_pairs.begin( ) && i->first > tranlog_item.index_entry_id ) )
                  --i;

               if( !( tranlog_item.index_entry_id >= i->first && tranlog_item.index_entry_id <= i->second ) )
                  dump_item = false;
            }

            if( dump_item )
            {
               if( first_item )
               {
                  first_item = false;
                  os << osstr.str( );
               }

               os << '\n';
               tranlog_item.dump( os, offs );
            }

            if( tranlog_item.has_pos_and_size( ) )
            {
               int64_t chunk = 16;
               unsigned char buffer[ 16 ];

               if( dump_item )
                  os << hex;

               for( int64_t i = 0; i < tranlog_item.data_size; i += chunk )
               {
                  if( i + chunk > tranlog_item.data_size )
                     chunk = tranlog_item.data_size - i;

                  fs.read( ( char* )buffer, chunk );

                  if( dump_item )
                  {
                     os << setw( sizeof( int64_t ) * 2 ) << setfill( '0' ) << i << "  ";

                     for( int j = 0; j < chunk; j++ )
                        os << setw( 2 ) << setfill( '0' ) << ( unsigned )buffer[ j ] << " ";

                     if( chunk < 16 )
                        os << string( ( 16 - chunk ) * 3, ' ' );

                     os << ' ';
                     for( int j = 0; j < chunk; j++ )
                     {
                        if( !is_print( buffer[ j ] ) )
                           os << ".";
                        else
                           os << buffer[ j ];
                     }

                     os << "\n";
                  }
               }

               if( dump_item )
                  os << dec;
            }
         }

         if( fs.tellg( ) > tranlog_info.entry_offs )
            break;
      }
   }
}

void ods::bulk_base::pause( )
{
   guard lock_write( o.write_lock );
   guard lock_read( o.read_lock );
   guard lock_impl( *o.p_impl->rp_impl_lock );

   o.bulk_operation_pause( );
}

ods::bulk_base::bulk_base( ods& o, progress* p_progress )
 :
 o( o )
{
   p_old_progress = o.p_progress;
   was_preventing_lazy_write = o.prevent_lazy_write;

   if( p_progress )
   {
      o.p_progress = p_progress;
      o.prevent_lazy_write = true;
   }
}

ods::bulk_base::~bulk_base( )
{
   o.p_progress = p_old_progress;
   o.prevent_lazy_write = was_preventing_lazy_write;
}

ods::bulk_dump::bulk_dump( ods& o, progress* p_progress )
 : bulk_base( o, p_progress )
{
   int i;

   for( i = 0; i < c_bulk_dump_max_attempts; i++ )
   {
      if( i > 0 )
         msleep( c_bulk_dump_attempt_sleep_time );

      guard lock_write( o.write_lock );
      guard lock_read( o.read_lock );
      guard lock_impl( *o.p_impl->rp_impl_lock );

      if( *o.p_impl->rp_bulk_mode != impl::e_bulk_mode_none )
         continue;

      impl::bulk_mode old_bulk_mode( ( impl::bulk_mode )*o.p_impl->rp_bulk_mode );

      *o.p_impl->rp_bulk_mode = impl::e_bulk_mode_dump;

      try
      {
         o.bulk_operation_start( );
         break;
      }
      catch( ... )
      {
         *o.p_impl->rp_bulk_mode = old_bulk_mode;
         throw;
      }
   }

   if( i == c_bulk_dump_max_attempts )
      THROW_ODS_ERROR( "thread cannot obtain bulk dump lock (max. attempts exceeded)" );
}

ods::bulk_dump::~bulk_dump( )
{
   guard lock_write( o.write_lock );
   guard lock_read( o.read_lock );
   guard lock_impl( *o.p_impl->rp_impl_lock );

   if( o.is_okay( ) )
   {
      o.bulk_operation_finish( );

      if( !*o.p_impl->rp_bulk_level )
         *o.p_impl->rp_bulk_mode = impl::e_bulk_mode_none;
   }
}

ods::bulk_read::bulk_read( ods& o, progress* p_progress, bool allow_thread_demotion )
 : bulk_base( o, p_progress )
{
   int i;

   for( i = 0; i < c_bulk_read_max_attempts; i++ )
   {
      if( i > 0 )
         msleep( c_bulk_read_attempt_sleep_time );

      guard lock_write( o.write_lock );
      guard lock_read( o.read_lock );
      guard lock_impl( *o.p_impl->rp_impl_lock );

      if( *o.p_impl->rp_bulk_mode == impl::e_bulk_mode_dump )
         continue;

      if( *o.p_impl->rp_bulk_mode >= impl::e_bulk_mode_read )
      {
         if( ( *o.p_impl->rp_bulk_mode == impl::e_bulk_mode_write )
          && ( *o.p_impl->rp_bulk_write_thread_id != current_thread_id( ) ) )
            continue;
         else if( !allow_thread_demotion && ( *o.p_impl->rp_bulk_mode != impl::e_bulk_mode_read ) )
            THROW_ODS_ERROR( "invalid attempt to obtain bulk read lock (needs allow_thread_demotion)" );
      }

      if( ( *o.p_impl->rp_bulk_mode == impl::e_bulk_mode_read )
       && ( *o.p_impl->rp_bulk_read_thread_id != current_thread_id( ) ) )
         continue;

      impl::bulk_mode old_bulk_mode( ( impl::bulk_mode )*o.p_impl->rp_bulk_mode );

      *o.p_impl->rp_bulk_mode = impl::e_bulk_mode_read;

      try
      {
         o.bulk_operation_start( );
         *o.p_impl->rp_bulk_read_thread_id = current_thread_id( );
         break;
      }
      catch( ... )
      {
         *o.p_impl->rp_bulk_mode = old_bulk_mode;
         throw;
      }
   }

   if( i == c_bulk_read_max_attempts )
      THROW_ODS_ERROR( "thread cannot obtain bulk read lock (max. attempts exceeded)" );
}

ods::bulk_read::~bulk_read( )
{
   guard lock_write( o.write_lock );
   guard lock_read( o.read_lock );
   guard lock_impl( *o.p_impl->rp_impl_lock );

   if( o.is_okay( ) )
   {
      o.bulk_operation_finish( );

      if( !*o.p_impl->rp_bulk_level )
      {
         *o.p_impl->rp_bulk_mode = impl::e_bulk_mode_none;
         *o.p_impl->rp_bulk_read_thread_id = o.p_impl->dummy_thread_id;
      }
   }
}

ods::bulk_write::bulk_write( ods& o, progress* p_progress, bool allow_thread_promotion )
 : bulk_base( o, p_progress )
{
   if( o.p_impl->is_read_only )
      THROW_ODS_ERROR( "attempt to obtain bulk write lock when database was opened for read only access" );

   int i;

   for( i = 0; i < c_bulk_write_max_attempts; i++ )
   {
      if( i > 0 )
         msleep( c_bulk_write_attempt_sleep_time );

      guard lock_write( o.write_lock );
      guard lock_read( o.read_lock );
      guard lock_impl( *o.p_impl->rp_impl_lock );

      if( ( *o.p_impl->rp_bulk_mode != impl::e_bulk_mode_none )
       && ( *o.p_impl->rp_bulk_mode != impl::e_bulk_mode_write ) )
      {
         if( *o.p_impl->rp_bulk_read_thread_id != current_thread_id( ) )
            continue;
         else if( !allow_thread_promotion )
            THROW_ODS_ERROR( "invalid attempt to obtain bulk write lock (needs allow_thread_promotion)" );
      }

      if( ( *o.p_impl->rp_bulk_mode == impl::e_bulk_mode_write )
       && ( *o.p_impl->rp_bulk_write_thread_id != current_thread_id( ) ) )
         continue;

      impl::bulk_mode old_bulk_mode( ( impl::bulk_mode )*o.p_impl->rp_bulk_mode );

      *o.p_impl->rp_bulk_mode = impl::e_bulk_mode_write;

      try
      {
         o.bulk_operation_start( );
         *o.p_impl->rp_bulk_write_thread_id = current_thread_id( );
         break;
      }
      catch( ... )
      {
         *o.p_impl->rp_bulk_mode = old_bulk_mode;
         throw;
      }
   }

   if( i == c_bulk_write_max_attempts )
      THROW_ODS_ERROR( "thread cannot obtain bulk write lock (max. attempts exceeded)" );
}

ods::bulk_write::~bulk_write( )
{
   guard lock_write( o.write_lock );
   guard lock_read( o.read_lock );
   guard lock_impl( *o.p_impl->rp_impl_lock );

   if( o.is_okay( ) )
   {
      o.bulk_operation_finish( );

      if( !*o.p_impl->rp_bulk_level )
      {
         *o.p_impl->rp_bulk_mode = impl::e_bulk_mode_none;
         *o.p_impl->rp_bulk_write_thread_id = o.p_impl->dummy_thread_id;
      }
   }
}

ods::transaction::transaction( ods& o, bool is_not_dummy )
 :
 o( o ),
 is_dummy( !is_not_dummy ),
 can_commit( true ),
 has_committed( false )
{
   if( !is_dummy )
      o.transaction_start( );
}

ods::transaction::transaction( ods& o, const string& label )
 :
 o( o ),
 is_dummy( false ),
 can_commit( true ),
 has_committed( false )
{
   o.transaction_start( label.empty( ) ? 0 : label.c_str( ) );
}

ods::transaction::~transaction( )
{
   if( !is_dummy && o.is_okay( ) )
   {
      if( can_commit && !has_committed )
         o.transaction_rollback( );
   }
}

void ods::transaction::commit( )
{
   if( !is_dummy )
   {
      if( !can_commit )
         THROW_ODS_ERROR( "attempt to commit an already processed transaction" );

      o.transaction_commit( );

      can_commit = false;
      has_committed = true;
   }
}

void ods::transaction::rollback( )
{
   if( !is_dummy )
   {
      if( !can_commit || has_committed )
         THROW_ODS_ERROR( "attempt to rollback an already processed transaction" );

      o.transaction_rollback( );

      can_commit = false;
   }
}

void ods::open_store( )
{
   DEBUG_LOG( "(opening)" );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   if( ++( *p_impl->rp_open_store_ref_count ) > 1 )
      return;

   DEBUG_LOG( "ods::open_store( )" );

   int64_t last_data_transformation = p_impl->rp_header_info->data_transform_id;
   int64_t last_index_transformation = p_impl->rp_header_info->index_transform_id;

   p_impl->read_header_file_info( );

#ifdef ODS_DEBUG
   ostringstream osstr;

   osstr << "(header info read from store)"
    << "\nversion = 0x0" << hex << ( p_impl->rp_header_info->version & c_version_mask ) << dec
    << ( ( p_impl->rp_header_info->version & c_encrypted_flag ) ? " (encrypted)" : "" )
    << ", num_logs = " << p_impl->rp_header_info->num_logs
    << ", num_trans = " << p_impl->rp_header_info->num_trans
    << ", num_writers = " << p_impl->rp_header_info->num_writers
    << "\ninit tranlog = " << p_impl->rp_header_info->init_tranlog
    << ", total_entries = " << p_impl->rp_header_info->total_entries
    << ", tranlog_offset = " << p_impl->rp_header_info->tranlog_offset
    << ", index_free_list = " << p_impl->rp_header_info->index_free_list
    << "\ntransaction_id = " << p_impl->rp_header_info->transaction_id
    << ", total_size_of_data = " << p_impl->rp_header_info->total_size_of_data
    << ", data_transform_id = " << p_impl->rp_header_info->data_transform_id
    << ", index_transform_id = " << p_impl->rp_header_info->index_transform_id;

   DEBUG_LOG( osstr.str( ) );
#endif

   check_version( p_impl->rp_header_info->version );

   if( p_impl->rp_header_info->data_transform_id != last_data_transformation )
      p_impl->rp_ods_data_cache_buffer->clear( );

   if( p_impl->rp_header_info->index_transform_id != last_index_transformation )
   {
      index_item_buffer_num = -1;
      p_impl->rp_ods_index_cache_buffer->clear( );
   }
}

void ods::close_store( )
{
   DEBUG_LOG( "(closing)" );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   if( --( *p_impl->rp_open_store_ref_count ) != 0 )
      return;

   data_and_index_write( );

   if( *p_impl->rp_has_changed )
   {
      // NOTE: If there are no transactions in progress and there are no other active writers
      // then if the "tranlog_offset" is greater than the value that is already stored in the
      // header it will now be updated.
      if( ( p_impl->tranlog_offset > p_impl->rp_header_info->tranlog_offset )
       && ( p_impl->rp_header_info->num_trans == 0 ) && ( p_impl->rp_header_info->num_writers == 1 ) )
         p_impl->rp_header_info->tranlog_offset = p_impl->tranlog_offset;

      p_impl->write_header_file_info( true );

      *p_impl->rp_has_changed = false;
   }

   DEBUG_LOG( "ods::close_store( )" );
}

void ods::bulk_operation_open( )
{
   guard header_file_lock( *p_impl->rp_file_section );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   DEBUG_LOG( "ods::bulk_operation_open( )" );

   lock_header_file( );
   try
   {
      open_store( );

      if( !p_impl->is_restoring
       && p_impl->rp_header_info->num_writers
       && ( *p_impl->rp_bulk_mode != impl::e_bulk_mode_dump ) )
      {
         if( !permit_copy )
            p_impl->is_corrupt = true;
         else
            THROW_ODS_ERROR( "database file corruption detected" );
      }
   }
   catch( ... )
   {
      unlock_header_file( );
      throw;
   }
}

void ods::bulk_operation_close( )
{
   guard header_file_lock( *p_impl->rp_file_section );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   close_store( );
   unlock_header_file( );

   DEBUG_LOG( "ods::bulk_operation_close( )" );
}

void ods::bulk_operation_start( )
{
   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   DEBUG_LOG( "ods::bulk_operation_start( )" );

   if( !*p_impl->rp_bulk_level )
      bulk_operation_open( );

   // NOTE: Increment must be done afterwards otherwise header file locking will get confused.
   ++*p_impl->rp_bulk_level;
}

void ods::bulk_operation_pause( )
{
   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   DEBUG_LOG( "ods::bulk_operation_pause( )" );

   *p_impl->rp_is_in_bulk_pause = true;
   bulk_operation_close( );

   msleep( c_bulk_pause_sleep_time );

   bulk_operation_open( );
   *p_impl->rp_is_in_bulk_pause = false;
}

void ods::bulk_operation_finish( )
{
   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   if( !--( *p_impl->rp_bulk_level ) )
      bulk_operation_close( );

   DEBUG_LOG( "ods::bulk_operation_finish( )" );
}

bool ods::bulk_operation_none_or_write( )
{
   int i;

   for( i = 0; i < c_bulk_write_max_attempts; i++ )
   {
      if( i > 0 )
         msleep( c_bulk_write_attempt_sleep_time );

      guard lock_impl( *p_impl->rp_impl_lock );

      if( !*p_impl->rp_bulk_level || ( *p_impl->rp_bulk_mode == impl::e_bulk_mode_write ) )
         break;

      // NOTE: If read locked by current thread then skip any further attempts.
      if( ( *p_impl->rp_bulk_mode == impl::e_bulk_mode_read )
       && ( *p_impl->rp_bulk_read_thread_id == current_thread_id( ) ) )
         return false;
   }

   return ( i != c_bulk_write_max_attempts );
}

void ods::transaction_start( const char* p_label )
{
   guard lock_write( write_lock );
   guard lock_read( read_lock );
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   if( p_impl->is_read_only )
      THROW_ODS_ERROR( "attempt to start transaction when database was opened for read only access" );

   if( !p_impl->trans_level )
   {
      // NOTE: Start file lock section.
      {
         ods::header_file_lock header_file_lock( *this );

         auto_ptr< ods::file_scope > ap_file_scope;

         if( !*p_impl->rp_bulk_level )
            ap_file_scope.reset( new ods::file_scope( *this ) );

         p_impl->p_trans_buffer->tran_id = p_impl->rp_header_info->transaction_id++;

         if( p_impl->using_tranlog )
         {
            p_impl->tranlog_offset = append_log_entry( p_impl->p_trans_buffer->tran_id, 0, p_label );

            if( !p_impl->rp_header_info->tranlog_offset )
               p_impl->rp_header_info->tranlog_offset = p_impl->tranlog_offset;
         }

         p_impl->p_ods_trans_op_cache_buffer->new_transaction( p_impl->p_trans_buffer->tran_id );
         p_impl->p_ods_trans_data_cache_buffer->new_transaction( p_impl->p_trans_buffer->tran_id );

         p_impl->total_trans_size = 0;
         p_impl->total_trans_op_count = 0;

         // NOTE: Need to act as though a write has occurred (in case none
         // does before the next file close) to ensure that the values for
         // "num_trans" and "num_writers" will be both correctly set.
         if( !*p_impl->rp_has_changed )
         {
            *p_impl->rp_has_changed = true;
            ++p_impl->rp_header_info->num_writers;
         }

         ++p_impl->rp_header_info->num_trans;

         p_impl->write_header_file_info( );
      }
      // NOTE: Finish file lock section.
   }
   else if( p_impl->using_tranlog )
   {
      // NOTE: Start file lock section.
      {
         ods::header_file_lock header_file_lock( *this );

         unsigned char flags = ( c_log_entry_item_op_store | c_log_entry_item_type_nested_tx );

         ods_index_entry index_entry;

         append_log_entry_item( p_impl->trans_level + 1, index_entry, flags, true );
      }
      // NOTE: Finish file lock section.
   }

   ++p_impl->trans_level;

   transaction_level_info trans_level_info;

   if( p_impl->trans_level > 1 )
   {
      trans_level_info.offset += p_impl->total_trans_size;
      trans_level_info.op_offset += p_impl->total_trans_op_count;
   }

   p_impl->p_trans_buffer->levels.push( trans_level_info );

#ifdef ODS_DEBUG
   ostringstream osstr;

   osstr << "ods::transaction_start( ) level = " << p_impl->trans_level;

   if( p_label )
      osstr << ", label = " << p_label;

   osstr << "\ntrans_level_info.offset = " << trans_level_info.offset
    << "\ntrans_level_info.op_offset = " << trans_level_info.op_offset;

   DEBUG_LOG( osstr.str( ) );
#endif
}

void ods::transaction_commit( )
{
   guard lock_write( write_lock );
   guard lock_read( read_lock );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   if( !bulk_operation_none_or_write( ) )
      THROW_ODS_ERROR( "cannot commit transaction when bulk locked for dumping or reading" );

   guard lock_impl( *p_impl->rp_impl_lock );

   int64_t size = p_impl->p_trans_buffer->levels.top( ).size;
   int64_t op_count = p_impl->p_trans_buffer->levels.top( ).op_count;

   p_impl->p_trans_buffer->levels.pop( );

   if( !p_impl->p_trans_buffer->levels.empty( ) )
   {
      p_impl->p_trans_buffer->levels.top( ).size += size;
      p_impl->p_trans_buffer->levels.top( ).op_count += op_count;
   }

#ifdef ODS_DEBUG
   ostringstream osstr;

   osstr << "ods::transaction_commit( )"
    << "\nop_count = " << op_count << "\nsize = " << size;

   DEBUG_LOG( osstr.str( ) );
#endif

   if( p_impl->trans_level == 1 )
   {
      transaction_op op;
      ods_index_entry index_entry;

      if( had_interim_trans_op_write )
      {
         trans_read_ops_buffer_num = -1;
         had_interim_trans_op_write = false;
      }

      if( had_interim_trans_data_write )
      {
         trans_read_data_buffer_num = -1;
         trans_read_data_buffer_offs = 0;
         had_interim_trans_data_write = false;

         if( p_impl->is_encrypted )
            memset( trans_read_buffer.data, '\0', c_trans_bytes_per_item );
      }

      bool had_any_ops = false;

      int64_t tx_id = p_impl->p_trans_buffer->tran_id;

      auto_ptr< ods::bulk_write > ap_bulk_write;

      if( !*p_impl->rp_bulk_level )
         ap_bulk_write.reset( new ods::bulk_write( *this ) );

      int64_t commit_items = 0;
      int64_t append_offset = 0;

      int64_t last_delete_pos = 0;
      int64_t last_delete_size = 0;
      int64_t prior_deleted_bytes = 0;

      date_time dtm( date_time::local( ) );

      if( p_impl->using_tranlog )
         append_offset = log_append_offset( );

      // NOTE: Ops are processed in reverse order so that earlier ops on the same entry
      // can simply be ignored through setting and later checking the trans_flag value.
      // If they were instead processed in ascending order then any repeated writes (or
      // writes that are later followed by a delete) would all have to take place which
      // could end up being much less efficient.
      for( int64_t i = op_count - 1; i >= 0; i-- )
      {
#ifdef ODS_DEBUG
         ostringstream osstr;

         osstr << "==> processing op #" << i;

         DEBUG_LOG( osstr.str( ) );
#endif
         read_transaction_op( op, i );

         if( p_progress )
         {
            date_time now( date_time::local( ) );
            uint64_t elapsed = seconds_between( dtm, now );

            if( elapsed >= p_progress->num_seconds )
            {
               dtm = now;
               p_progress->output_progress( "." );
            }
         }

         if( op.data.id.get_num( ) < p_impl->rp_header_info->total_entries )
         {
            unsigned char flags = c_log_entry_item_flag_is_post_op;

            read_index_entry( index_entry, op.data.id.get_num( ) );

            if( ( index_entry.trans_flag != ods_index_entry::e_trans_none )
             && ( index_entry.trans_flag != ods_index_entry::e_trans_free_list ) )
            {
               if( index_entry.lock_flag == ods_index_entry::e_lock_none )
               {
                  if( index_entry.trans_flag == ods_index_entry::e_trans_delete )
                  {
                     bool clear_last = false;

                     int64_t pos_and_size = index_entry.data.pos + index_entry.data.size;

                     if( last_delete_size )
                     {
                        if( ( last_delete_pos + last_delete_size ) != index_entry.data.pos )
                        {
                           clear_last = true;
                           prior_deleted_bytes = 0;
                        }
                        else
                           prior_deleted_bytes += last_delete_size;
                     }

                     if( pos_and_size == p_impl->rp_header_info->total_size_of_data )
                     {
                        clear_last = true;
                        p_impl->rp_header_info->total_size_of_data -= ( index_entry.data.size + prior_deleted_bytes );
                     }

                     if( clear_last )
                     {
                        last_delete_pos = 0;
                        last_delete_size = 0;

                        prior_deleted_bytes = 0;
                     }
                     else
                     {
                        last_delete_pos = index_entry.data.pos;
                        last_delete_size = index_entry.data.size;
                     }

                     flags |= c_log_entry_item_op_destroy;

                     index_entry.data.pos = p_impl->rp_header_info->index_free_list;

                     index_entry.data.size = 0;
                     index_entry.trans_flag = ods_index_entry::e_trans_free_list;

                     p_impl->rp_header_info->index_free_list = op.data.id.get_num( ) + 1;
                  }
                  else
                  {
                     flags |= c_log_entry_item_op_store;

                     index_entry.data.size = op.data.size;

                     if( op.type == transaction_op::e_op_type_append )
                     {
                        index_entry.data.pos = p_impl->rp_header_info->total_size_of_data;
                        p_impl->rp_header_info->total_size_of_data += op.data.size;
                     }

                     set_write_data_pos( index_entry.data.pos );

                     set_read_trans_data_pos( op.data.pos );

                     int64_t chunk = c_buffer_chunk_size;
                     char buffer[ c_buffer_chunk_size ];

                     for( int64_t j = 0; j < op.data.size; j += chunk )
                     {
                        if( j + chunk > op.data.size )
                           chunk = op.data.size - j;

                        read_trans_data_bytes( buffer, chunk );

                        // NOTE: Progress output may need to be performed
                        // whilst writing data for objects that are large.
                        if( p_progress )
                        {
                           date_time now( date_time::local( ) );
                           uint64_t elapsed = seconds_between( dtm, now );

                           if( elapsed >= p_progress->num_seconds )
                           {
                              dtm = now;
                              p_progress->output_progress( "." );
                           }
                        }

                        write_data_bytes( buffer, chunk );
                     }

                     if( p_impl->using_tranlog )
                     {
                        if( p_impl->is_encrypted )
                           crypt_data_buffer( p_impl->data_write_buffer.data,
                            p_impl->data_write_key_buffer.data, c_data_bytes_per_item );

                        p_impl->rp_ods_data_cache_buffer->put( p_impl->data_write_buffer, data_write_buffer_num );

                        data_write_buffer_num = -1;
                        data_write_buffer_offs = 0;
                     }
                  }

                  if( p_impl->using_tranlog )
                  {
                     ++commit_items;

                     append_log_entry_item( op.data.id.get_num( ), index_entry, flags, true, 0, 0, p_progress );
                  }

                  index_entry.data.tran_op = 0;

                  if( index_entry.trans_flag != ods_index_entry::e_trans_free_list )
                     index_entry.trans_flag = ods_index_entry::e_trans_none;

                  write_index_entry( index_entry, op.data.id.get_num( ) );

                  had_any_ops = true;
               }
               else
                  THROW_ODS_ERROR( "unexpected lock flag at " STRINGIZE( __LINE__ ) );
            }
         }
         else
            THROW_ODS_ERROR( "unexpected id > total_entries at " STRINGIZE( __LINE__ ) );
      }

      if( op_count )
      {
         if( p_impl->using_tranlog )
            log_entry_commit( p_impl->tranlog_offset, append_offset, commit_items );

         ++p_impl->rp_header_info->data_transform_id;
         ++p_impl->rp_header_info->index_transform_id;
      }

      data_and_index_write( false );

      // NOTE: This call needs to occur within the scope of the bulk write lock.
      transaction_completed( );
   }
   else
   {
      // NOTE: Start file lock section.
      if( p_impl->using_tranlog )
      {
         ods::header_file_lock header_file_lock( *this );

         unsigned char flags = ( c_log_entry_item_op_create | c_log_entry_item_type_nested_tx );

         ods_index_entry index_entry;

         append_log_entry_item( p_impl->trans_level, index_entry, flags, true );
      }
      // NOTE: Finish file lock section.

      transaction_completed( true );
   }
}

void ods::transaction_rollback( )
{
   guard lock_write( write_lock );
   guard lock_read( read_lock );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   if( !bulk_operation_none_or_write( ) )
      THROW_ODS_ERROR( "cannot rollback transaction when bulk locked for dumping or reading" );

   guard lock_impl( *p_impl->rp_impl_lock );

   int64_t size = p_impl->p_trans_buffer->levels.top( ).size;

   int64_t op_count = p_impl->p_trans_buffer->levels.top( ).op_count;
   int64_t op_offset = p_impl->p_trans_buffer->levels.top( ).op_offset;

   p_impl->p_trans_buffer->levels.pop( );

   transaction_op op;
   ods_index_entry index_entry;

   if( had_interim_trans_op_write )
   {
      trans_read_ops_buffer_num = -1;
      had_interim_trans_op_write = false;
   }

   if( had_interim_trans_data_write )
   {
      trans_read_data_buffer_num = -1;
      trans_read_data_buffer_offs = 0;
      had_interim_trans_data_write = false;

      if( p_impl->is_encrypted )
         memset( trans_read_buffer.data, '\0', c_trans_bytes_per_item );
   }

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !*p_impl->rp_bulk_level )
      ap_bulk_write.reset( new ods::bulk_write( *this ) );

   if( p_impl->using_tranlog && ( p_impl->trans_level > 1 ) )
   {
      unsigned char flags = ( c_log_entry_item_op_destroy | c_log_entry_item_type_nested_tx );

      ods_index_entry index_entry;

      append_log_entry_item( p_impl->trans_level, index_entry, flags, true );
   }

#ifdef ODS_DEBUG
   ostringstream osstr;

   osstr << "ods::transaction_rollback( )"
     << "\nsize = " << size << "\nop_count = " << op_count << "\nop_offset = " << op_offset;

   DEBUG_LOG( osstr.str( ) );
#endif

   if( op_count )
   {
      // NOTE: Ops are processed in reverse order so that the total size of any data
      // that had been appended can be correctly subtracted from the total data size
      // as well as ensuring that index entries which had been either created or had
      // been revived will now be placed into the freelist.
      for( int64_t i = op_count - 1; i >= 0; i-- )
      {
         read_transaction_op( op, op_offset + i );

         if( op.data.id.get_num( ) < p_impl->rp_header_info->total_entries )
         {
            read_index_entry( index_entry, op.data.id.get_num( ) );

            if( ( index_entry.trans_flag != ods_index_entry::e_trans_none )
             && ( index_entry.trans_flag != ods_index_entry::e_trans_free_list ) )
            {
               if( ( op.data.old_tran_op == 0 ) && ( op.data.old_tran_id == -1 ) )
               {
                  if( ( index_entry.data.pos + index_entry.data.size )
                   == p_impl->rp_header_info->total_size_of_data )
                     p_impl->rp_header_info->total_size_of_data -= index_entry.data.size;

                  index_entry.data.pos = p_impl->rp_header_info->index_free_list;

                  index_entry.data.size = 0;
                  index_entry.trans_flag = ods_index_entry::e_trans_free_list;

                  p_impl->rp_header_info->index_free_list = op.data.id.get_num( ) + 1;
               }

               index_entry.data.tran_op = op.data.old_tran_op;

               if( op.data.old_tran_id > 0 )
                  index_entry.data.tran_id = op.data.old_tran_id;

               if( index_entry.trans_flag != ods_index_entry::e_trans_free_list )
               {
                  if( op.data.old_tran_op == 0 )
                     index_entry.trans_flag = ods_index_entry::e_trans_none;
                  else
                     index_entry.trans_flag = ods_index_entry::e_trans_change;
               }

               write_index_entry( index_entry, op.data.id.get_num( ) );
            }
         }
         else
            THROW_ODS_ERROR( "unexpected id > total_entries at " STRINGIZE( __LINE__ ) );
      }

      data_and_index_write( false );

      if( p_impl->trans_level == 1 )
         ++p_impl->rp_header_info->index_transform_id;

      set_write_trans_data_pos( p_impl->total_trans_size - size );
   }

   p_impl->total_trans_op_count -= op_count;
   p_impl->total_trans_size -= size;

#ifdef ODS_DEBUG
   osstr.clear( );

   osstr << "\ntotal_trans_size = " << ( int64_t )p_impl->total_trans_size
    << "\ntotal_trans_op_count = " << ( int64_t )p_impl->total_trans_op_count;

   DEBUG_LOG( osstr.str( ) );
#endif

   unsigned num_from = ( p_impl->total_trans_op_count / c_trans_ops_per_item );

   if( p_impl->total_trans_op_count % c_trans_ops_per_item )
      ++num_from;

#ifdef ODS_DEBUG
   DEBUG_LOG( "<< destroy transaction cache entries after current >>" );
#endif
   p_impl->p_ods_trans_op_cache_buffer->clear_from( num_from );
   p_impl->p_ods_trans_data_cache_buffer->clear_from( trans_write_data_buffer_num + 1 );

   transaction_completed( );
}

void ods::transaction_completed( bool keep_buffered )
{
   guard lock_write( write_lock );
   guard lock_read( read_lock );
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

#ifdef ODS_DEBUG
   ostringstream osstr;

   osstr << "ods::transaction_completed( ) level = " << p_impl->trans_level;

   DEBUG_LOG( osstr.str( ) );
#endif

   if( !--p_impl->trans_level )
   {
      keep_buffered = false;

      p_impl->p_ods_trans_op_cache_buffer->end_transaction( );
      p_impl->p_ods_trans_data_cache_buffer->end_transaction( );

      // NOTE: Start file lock section.
      {
         ods::header_file_lock header_file_lock( *this );

         // NOTE: If no write has occurred since the last close then need to act as
         // though one has in order to correctly set "num_trans" and "num_writers".
         if( !*p_impl->rp_has_changed )
         {
            *p_impl->rp_has_changed = true;
            ++p_impl->rp_header_info->num_writers;
         }

         --p_impl->rp_header_info->num_trans;

         p_impl->write_header_file_info( );
      }
      // NOTE: Finish file lock section.
   }

   if( !keep_buffered )
   {
      trans_read_ops_buffer_num = -1;
      trans_read_data_buffer_num = -1;
      trans_read_data_buffer_offs = 0;

      if( p_impl->is_encrypted )
         memset( trans_read_buffer.data, '\0', c_trans_bytes_per_item );
   }
}

void ods::lock_header_file( )
{
   if( *p_impl->rp_bulk_level && !*p_impl->rp_is_in_bulk_pause )
      return;

   int attempts = c_header_lock_max_attempts;

   while( attempts )
   {
      if( p_impl->rp_header_file->lock( ) )
         break;

      --attempts;
      msleep( c_header_lock_attempt_sleep_time );
   }

   if( !attempts )
      THROW_ODS_ERROR( "unable to lock header file" );

   DEBUG_LOG( ">>>>>>>>>> captured header file lock <<<<<<<<<<" );
}

void ods::unlock_header_file( )
{
   if( *p_impl->rp_bulk_level && !*p_impl->rp_is_in_bulk_pause )
      return;

   p_impl->rp_header_file->unlock( );

   DEBUG_LOG( ">>>>>>>>>> released header file lock <<<<<<<<<<" );
}

void ods::data_and_index_write( bool flush, bool skip_encrypt )
{
   if( data_write_buffer_num != -1 )
   {
      if( !skip_encrypt && p_impl->is_encrypted )
         crypt_data_buffer( p_impl->data_write_buffer.data,
          p_impl->data_write_key_buffer.data, c_data_bytes_per_item );

      p_impl->rp_ods_data_cache_buffer->put( p_impl->data_write_buffer, data_write_buffer_num );

      p_impl->rp_ods_data_cache_buffer->unlock_region(
       data_write_buffer_num * c_data_bytes_per_item, c_data_bytes_per_item );

      data_write_buffer_num = -1;
      data_write_buffer_offs = 0;

      if( p_impl->is_encrypted )
         memset( p_impl->data_write_key_buffer.data, '\0', c_data_bytes_per_item );
   }

   if( flush )
      p_impl->rp_ods_data_cache_buffer->flush( );

   if( index_item_buffer_num != -1 )
   {
      p_impl->rp_ods_index_cache_buffer->put( p_impl->index_item_buffer, index_item_buffer_num );

      index_item_buffer_num = -1;
   }

   if( flush )
      p_impl->rp_ods_index_cache_buffer->flush( );
}

int64_t ods::log_append_offset( )
{
   fstream fs;

   fs.open( p_impl->tranlog_file_name.c_str( ), ios::in | ios::binary );

   if( !fs )
      THROW_ODS_ERROR( "unable to open transaction log '" + p_impl->tranlog_file_name + "' in log_append_offset" );

   log_info tranlog_info;

   tranlog_info.read( fs );

   fs.close( );

   return tranlog_info.append_offs;
}

int64_t ods::append_log_entry( int64_t tx_id, int64_t* p_append_offset, const char* p_label )
{
   log_stream logf( p_impl->tranlog_file_name.c_str( ), use_sync_write );

   log_info tranlog_info;

   tranlog_info.read( logf );

   int64_t tx_time = time( 0 );

   log_entry tranlog_entry;

   if( tranlog_info.entry_offs )
   {
      logf.set_pos( tranlog_info.entry_offs );
      tranlog_entry.read( logf );

      if( tx_id < tranlog_entry.tx_id )
         THROW_ODS_ERROR( "unexpected tx_id is less than that of the last transaction log entry" );

      if( tx_time < tranlog_entry.tx_time )
         THROW_ODS_ERROR( "unexpected tx_time is less than that of the last transaction log entry" );

      tranlog_entry.commit_offs = tranlog_entry.commit_items = 0;
   }

   memset( tranlog_entry.label, '\0', sizeof( tranlog_entry.label ) );

   if( p_label )
      memcpy( tranlog_entry.label, p_label, min( strlen( p_label ), sizeof( tranlog_entry.label ) ) );

   tranlog_entry.tx_id = tx_id;
   tranlog_entry.tx_time = tx_time;

   tranlog_entry.total_data_bytes = p_impl->rp_header_info->total_size_of_data;

   tranlog_entry.data_transform_id = p_impl->rp_header_info->data_transform_id;
   tranlog_entry.index_transform_id = p_impl->rp_header_info->index_transform_id;

   tranlog_entry.prior_entry_offs = tranlog_info.entry_offs;

   tranlog_info.entry_offs = tranlog_info.append_offs;
   tranlog_info.entry_time = tx_time;

   logf.set_pos( tranlog_info.append_offs );

   tranlog_entry.write( logf );

   if( tranlog_entry.prior_entry_offs )
   {
      log_entry prior_log_entry;

      logf.set_pos( tranlog_entry.prior_entry_offs );

      prior_log_entry.read( logf );
      prior_log_entry.next_entry_offs = tranlog_info.append_offs;

      logf.set_pos( tranlog_entry.prior_entry_offs );
      prior_log_entry.write( logf );
   }

   tranlog_info.append_offs += tranlog_entry.size_of( );

   if( p_append_offset )
      *p_append_offset = tranlog_info.append_offs;

   logf.set_pos( 0 );

   tranlog_info.write( logf );

   return tranlog_info.entry_offs;
}

void ods::log_entry_commit( int64_t entry_offset, int64_t commit_offs, int64_t commit_items )
{
   // NOTE: Even if "use_sync_write" is false force a sync now.
   log_stream logf( p_impl->tranlog_file_name.c_str( ), true );

   log_entry tranlog_entry;

   logf.set_pos( entry_offset );

   tranlog_entry.read( logf );

   tranlog_entry.commit_offs = commit_offs;
   tranlog_entry.commit_items = commit_items;

   logf.set_pos( entry_offset );

   tranlog_entry.write( logf );
}

int64_t ods::append_log_entry_item( int64_t num,
 const ods_index_entry& index_entry, unsigned char flags,
 bool is_in_tx, int64_t old_tx_id, int64_t old_data_pos, progress* p_progress )
{
   date_time dtm( date_time::local( ) );

   log_stream logf( p_impl->tranlog_file_name.c_str( ), use_sync_write );

   log_info tranlog_info;
   tranlog_info.read( logf );

   log_entry_item tranlog_item;

   tranlog_item.flags = flags;

   if( tranlog_item.has_old_tran_id( ) )
      tranlog_item.tx_oid = old_tx_id;

   tranlog_item.index_entry_id = num;

   if( old_data_pos )
   {
      tranlog_item.data_opos = old_data_pos;
      tranlog_item.flags |= c_log_entry_item_flag_has_old_pos;
   }

   if( tranlog_item.has_pos_and_size( ) )
   {
      tranlog_item.data_pos = index_entry.data.pos;
      tranlog_item.data_size = index_entry.data.size;
   }

   logf.set_pos( tranlog_info.append_offs );

   tranlog_item.write( logf );

   bool is_encrypted = p_impl->is_encrypted;

   if( tranlog_item.has_pos_and_size( ) )
   {
      set_read_data_pos( index_entry.data.pos, true, is_encrypted );

      int64_t chunk = c_buffer_chunk_size;
      char buffer[ c_buffer_chunk_size ];

      for( int64_t j = 0; j < index_entry.data.size; j += chunk )
      {
         if( j + chunk > index_entry.data.size )
            chunk = index_entry.data.size - j;

         read_data_bytes( buffer, chunk, is_encrypted );
         logf.write( ( unsigned char* )buffer, chunk );

         if( p_progress )
         {
            date_time now( date_time::local( ) );
            uint64_t elapsed = seconds_between( dtm, now );

            if( elapsed >= p_progress->num_seconds )
            {
               dtm = now;
               p_progress->output_progress( "." );
            }
         }
      }

      if( is_encrypted )
         set_read_data_pos( -1 );
   }

   int64_t old_append_offs = tranlog_info.append_offs;

   tranlog_info.append_offs = logf.get_pos( );

   int64_t log_entry_offs = p_impl->tranlog_offset;

   if( log_entry_offs )
   {
      log_entry tranlog_entry;

      logf.set_pos( log_entry_offs );
      tranlog_entry.read( logf );

      if( !is_in_tx )
      {
         tranlog_entry.commit_offs = old_append_offs;
         tranlog_entry.commit_items = 1;
      }
      else
         tranlog_entry.commit_offs = tranlog_info.append_offs;

      logf.set_pos( log_entry_offs );
      tranlog_entry.write( logf );
   }

   logf.set_pos( 0 );
   tranlog_info.write( logf );

   return tranlog_info.append_offs;
}

void ods::rollback_dead_transactions( progress* p_progress )
{
   guard lock_write( write_lock );
   guard lock_read( read_lock );
   guard lock_impl( *p_impl->rp_impl_lock );

   DEBUG_LOG( "(rollback dead transactions)" );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   if( p_impl->trans_level )
      THROW_ODS_ERROR( "cannot rollback dead transactions whilst in a transaction" );

   if( !p_impl->rp_header_file->is_locked_for_exclusive( ) )
      THROW_ODS_ERROR( "cannot rollback dead transactions unless locked for exclusive write" );

   if( *p_impl->rp_bulk_level && ( *p_impl->rp_bulk_mode != impl::e_bulk_mode_write ) )
      THROW_ODS_ERROR( "cannot rollback dead transactions when bulk locked for dumping or reading" );

   date_time dtm( date_time::local( ) );

   temp_set_value< bool > temp_is_restoring( p_impl->is_restoring, true );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !*p_impl->rp_bulk_level )
      ap_bulk_write.reset( new ods::bulk_write( *this, p_progress ) );

   ods_index_entry index_entry;

   int64_t total = p_impl->rp_header_info->total_entries;

   for( int64_t i = 0; i < total; i++ )
   {
      if( p_progress )
      {
         date_time now( date_time::local( ) );
         uint64_t elapsed = seconds_between( dtm, now );

         if( elapsed >= p_progress->num_seconds )
         {
            dtm = now;
            p_progress->output_progress( "." );
         }
      }

      read_index_entry( index_entry, i );

      if( ( index_entry.lock_flag == ods_index_entry::e_lock_none )
       && ( index_entry.trans_flag != ods_index_entry::e_trans_none )
       && ( index_entry.trans_flag != ods_index_entry::e_trans_free_list ) )
      {
         if( index_entry.data.size == 0 )
         {
            index_entry.data.pos = p_impl->rp_header_info->index_free_list;
            index_entry.data.size = 0;

            index_entry.data.tran_op = 0;
            index_entry.trans_flag = ods_index_entry::e_trans_free_list;

            p_impl->rp_header_info->index_free_list = i + 1;
         }
         else
         {
            index_entry.data.tran_op = 0;
            index_entry.trans_flag = ods_index_entry::e_trans_none;
         }

         write_index_entry( index_entry, i );
      }
   }

   data_and_index_write( );

   p_impl->rp_header_info->num_trans = 0;
   p_impl->rp_header_info->num_writers = 0;

   *p_impl->rp_has_changed = true;
   p_impl->write_header_file_info( );

   *p_impl->rp_has_changed = false;
}

void ods::restore_from_transaction_log( bool force_reconstruct, progress* p_progress )
{
   guard lock_write( write_lock );
   guard lock_read( read_lock );
   guard lock_impl( *p_impl->rp_impl_lock );

   DEBUG_LOG( "(restore from transaction log)" );

   if( !okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   if( p_impl->trans_level )
      THROW_ODS_ERROR( "cannot restore from transaction log whilst in a transaction" );

   if( !p_impl->using_tranlog )
      THROW_ODS_ERROR( "cannot restore non-transaction log database from transaction log" );

   if( !p_impl->rp_header_file->is_locked_for_exclusive( ) )
      THROW_ODS_ERROR( "cannot restore from transaction log unless locked for exclusive write" );

   if( *p_impl->rp_bulk_level && ( *p_impl->rp_bulk_mode != impl::e_bulk_mode_write ) )
      THROW_ODS_ERROR( "cannot restore from transaction log when bulk locked for dumping or reading" );

   date_time dtm( date_time::local( ) );

   temp_set_value< bool > temp_is_restoring( p_impl->is_restoring, true );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !*p_impl->rp_bulk_level )
      ap_bulk_write.reset( new ods::bulk_write( *this, p_progress ) );

   fstream fs;
   log_info tranlog_info;

   map< int16_t, pair< string, log_info > > sequenced_logs;
   map< int16_t, pair< string, log_info > >::iterator sli;

   if( force_reconstruct )
   {
      DEBUG_LOG( "*** force_reconstruct ***" );

      p_impl->rp_header_info->total_entries = 0;
      p_impl->rp_header_info->tranlog_offset = 0;
      p_impl->rp_header_info->transaction_id = 0;
      p_impl->rp_header_info->index_free_list = 0;
      p_impl->rp_header_info->data_transform_id = 0;
      p_impl->rp_header_info->index_transform_id = 0;
      p_impl->rp_header_info->total_size_of_data = 0;
   }

   int64_t tranlog_offset = p_impl->rp_header_info->tranlog_offset;

   bool is_reconstruct = ( tranlog_offset == 0 );

   if( !is_reconstruct )
   {
      fs.open( p_impl->tranlog_file_name.c_str( ), ios::in | ios::binary );

      if( !fs )
         THROW_ODS_ERROR( "unable to open transaction log '" + p_impl->tranlog_file_name + "' in restore_from_transaction_log" );

      tranlog_info.read( fs );

      fs.close( );

      sequenced_logs.insert( make_pair( tranlog_info.sequence, make_pair( p_impl->tranlog_file_name, tranlog_info ) ) );
   }
   else
   {
      string tranlog_path( "." );
      string tranlog_file( p_impl->tranlog_file_name );

      string::size_type pos = tranlog_file.rfind( '/' );

      if( pos != string::npos )
      {
         tranlog_path = tranlog_file.substr( 0, pos );
         tranlog_file.erase( 0, pos + 1 );
      }

      file_filter ff;
      fs_iterator fsi( tranlog_path, &ff );

      while( fsi.has_next( ) )
      {
         string::size_type pos = fsi.get_name( ).find( tranlog_file );

         if( pos != 0 )
            continue;

         string tranlog_path_and_file( tranlog_path + '/' + fsi.get_name( ) );

         fs.open( tranlog_path_and_file.c_str( ), ios::in | ios::binary );

         if( !fs )
            THROW_ODS_ERROR( "unable to open transaction log '" + tranlog_path_and_file + "' in restore_from_transaction_log" );

         tranlog_info.read( fs );

         fs.close( );

         sequenced_logs.insert( make_pair( tranlog_info.sequence, make_pair( tranlog_path_and_file, tranlog_info ) ) );
      }

      int64_t init_time = 0;
      int64_t last_time = 0;
      int16_t last_sequence = 0;

      for( sli = sequenced_logs.begin( ); sli != sequenced_logs.end( ); ++sli )
      {
         if( sli->first != last_sequence + 1 )
            THROW_ODS_ERROR( "cannot reconstruct database due to missing transaction log #" + to_string( last_sequence + 1 ) );

         last_sequence = sli->first;

         if( !init_time )
            init_time = sli->second.second.init_time;
         else if( sli->second.second.init_time != init_time )
            THROW_ODS_ERROR( "cannot reconstruct database due to invalid init_time in transaction log #" + to_string( sli->first ) );

         if( last_time && sli->second.second.sequence_old_tm != last_time )
            THROW_ODS_ERROR( "cannot reconstruct database due to invalid sequence_old_tm in transaction log #" + to_string( sli->first ) );

         last_time = sli->second.second.sequence_new_tm;
      }
   }

   int64_t entry_num = 0;

   int64_t last_offs = 0;
   int64_t last_tx_id = 0;

   int64_t last_entry_offs = 0;
   int64_t last_entry_time = 0;
   int64_t last_append_offs = 0;

   int64_t last_data_transform_id = 0;
   int64_t last_index_transform_id = 0;

   bool had_any_entries = false;

   bool is_encrypted = p_impl->is_encrypted;

   for( sli = sequenced_logs.begin( ); sli != sequenced_logs.end( ); ++sli )
   {
      tranlog_info = sli->second.second;

      if( tranlog_info.append_offs > tranlog_info.size_of( ) )
      {
         set< int64_t > committed_transactions;

         fs.open( sli->second.first.c_str( ), ios::in | ios::binary );

         if( !fs )
            THROW_ODS_ERROR( "unable to open transaction log '" + sli->second.first + "' in restore_from_transaction_log" );

         fs.seekg( tranlog_info.size_of( ), ios::beg );

         if( is_reconstruct )
            tranlog_offset = 0;

         while( true )
         {
            int64_t entry_offset = fs.tellg( );

            if( entry_num && p_progress )
            {
               date_time now( date_time::local( ) );
               uint64_t elapsed = seconds_between( dtm, now );

               if( elapsed >= p_progress->num_seconds )
               {
                  dtm = now;
                  p_progress->output_progress( "." );
               }
            }

            ++entry_num;

            log_entry tranlog_entry;
            tranlog_entry.read( fs );

            int64_t tx_id = tranlog_entry.tx_id;

            if( tx_id >= p_impl->rp_header_info->transaction_id )
               p_impl->rp_header_info->transaction_id = tx_id + 1;

            int64_t commit_offs = tranlog_entry.commit_offs;

            if( tranlog_entry.commit_items )
               committed_transactions.insert( tx_id );

            int64_t next_offs = tranlog_entry.next_entry_offs;

            last_data_transform_id = tranlog_entry.data_transform_id;
            last_index_transform_id = tranlog_entry.index_transform_id;

            if( committed_transactions.find( tx_id ) != committed_transactions.end( ) )
            {
               last_tx_id = tx_id;
               last_entry_offs = entry_offset;
               last_entry_time = tranlog_entry.tx_time;
               last_append_offs = next_offs;
            }

            if( entry_offset <= tranlog_offset )
            {
               p_impl->rp_header_info->data_transform_id = tranlog_entry.data_transform_id;
               p_impl->rp_header_info->index_transform_id = tranlog_entry.index_transform_id;
            }

            // NOTE: After we have passed the last "good point" that was recorded in the database
            // header process every item according to whether the transaction should be committed
            // or rolled back.
            if( entry_offset > tranlog_offset )
            {
               had_any_entries = true;
               tranlog_offset = entry_offset;

               if( !next_offs || ( next_offs == last_offs ) )
                  next_offs = last_append_offs = tranlog_info.append_offs;

               bool had_any_data = false;

               size_t nested_level = 0;
               size_t ignore_start_nested = 0;

               int64_t last_delete_pos = 0;
               int64_t last_delete_size = 0;
               int64_t prior_deleted_bytes = 0;

               map< size_t, size_t > num_at_height;
 
               map< int64_t, deque< int64_t > > rollback_freelist_entries;

               while( fs.tellg( ) < next_offs )
               {
                  log_entry_item tranlog_item;
                  tranlog_item.read( fs );

                  bool write_entry = false;
                  bool add_to_free_list = false;

                  bool commit = ( committed_transactions.find( tx_id ) != committed_transactions.end( ) );

                  ods_index_entry index_entry;

                  if( tranlog_item.is_nested_tx( ) )
                  {
                     if( tranlog_item.is_create( ) )
                     {
                        ++num_at_height[ --nested_level ];

                        continue;
                     }
                     else if( tranlog_item.is_destroy( ) )
                     {
                        --nested_level;

                        if( rollback_freelist_entries.count( commit_offs ) )
                        {
                           int64_t tran_id = rollback_freelist_entries[ commit_offs ].front( );

                           while( !rollback_freelist_entries[ commit_offs ].empty( ) )
                           {
                              int64_t index_entry_id = rollback_freelist_entries[ commit_offs ].back( );

                              if( index_entry_id == -1 )
                              {
                                 rollback_freelist_entries[ commit_offs ].pop_back( );

                                 if( !num_at_height[ nested_level + 1 ] )
                                    break;

                                 --num_at_height[ nested_level + 1 ];

                                 continue;
                              }

                              add_entry_to_freelist( index_entry_id, tran_id );

                              rollback_freelist_entries[ commit_offs ].pop_back( );
                           }
                        }

                        continue;
                     }
                     else
                     {
                        ++nested_level;

                        if( !rollback_freelist_entries.count( commit_offs ) )
                           rollback_freelist_entries[ commit_offs ].push_back( tx_id );

                        rollback_freelist_entries[ commit_offs ].push_back( -1 );

                        continue;
                     }
                  }

                  if( !tranlog_item.is_post_op( ) )
                     ++p_impl->rp_header_info->index_transform_id;

                  bool can_read = true;

                  if( tranlog_item.index_entry_id >= p_impl->rp_header_info->total_entries )
                  {
                     can_read = false;
                     ods_index_entry blank_entry;

                     index_entry = blank_entry;
                     p_impl->rp_header_info->total_entries = tranlog_item.index_entry_id + 1;
                  }

                  int64_t tran_id = tx_id;

                  if( !commit && tranlog_item.has_old_tran_id( ) )
                     tran_id = tranlog_item.tx_oid;

                  if( !can_read )
                     write_entry = true;
                  else
                     read_index_entry( index_entry, tranlog_item.index_entry_id );

                  if( tranlog_item.has_pos_and_size( ) )
                  {
                     if( tranlog_item.has_old_pos( )
                      || ( commit && tranlog_item.is_post_op( ) )
                      || ( !commit && !tranlog_item.is_post_op( ) ) )
                     {
                        int64_t dpos = 0;
                        int64_t offs = 0;

                        if( commit || !tranlog_item.has_old_pos( ) )
                           dpos = tranlog_item.data_pos;
                        else
                           dpos = tranlog_item.data_opos;

                        // NOTE: If reconstructing then there is no need to write
                        // old data for transactions that had been rolled back.
                        if( !commit && is_reconstruct )
                           fs.seekg( tranlog_item.data_size, ios::cur );
                        else
                        {
                           had_any_data = true;

                           int64_t chunk = c_buffer_chunk_size;

                           char buffer[ c_buffer_chunk_size ];
                           char key_buffer[ c_buffer_chunk_size * 2 ];

                           int64_t crypt_dpos = tranlog_item.data_pos;
                           int64_t crypt_opos = tranlog_item.data_opos;

                           set_write_data_pos( dpos, is_encrypted, is_encrypted );

                           for( int64_t j = 0; j < tranlog_item.data_size; j += chunk )
                           {
                              if( j + chunk > tranlog_item.data_size )
                                 chunk = tranlog_item.data_size - j;

                              fs.read( buffer, chunk );

                              // NOTE: If an encrypted item was moved then it first needs to be decrypted
                              // using the old number and offset and then encrypted with both new values.
                              // As the chunk size plus offset can overrun this key buffer (whether using
                              // old or new pos assuming that both are not exactly aligned) the buffer is
                              // twice the size required and "init" is being called for both the page for
                              // the current pos and the following page).
                              if( commit && is_encrypted && tranlog_item.has_old_pos( ) )
                              {
                                 init_key_buffer( key_buffer,
                                  c_data_bytes_per_item, crypt_opos / c_data_bytes_per_item, p_impl->pwd_hash, p_impl->cipher_type );

                                 init_key_buffer( key_buffer + c_data_bytes_per_item,
                                  c_data_bytes_per_item, ( crypt_opos / c_data_bytes_per_item ) + 1, p_impl->pwd_hash, p_impl->cipher_type );

                                 offs = crypt_opos % c_data_bytes_per_item;

                                 crypt_data_buffer( buffer, key_buffer + offs, chunk );

                                 init_key_buffer( key_buffer,
                                  c_data_bytes_per_item, crypt_dpos / c_data_bytes_per_item, p_impl->pwd_hash, p_impl->cipher_type );

                                 init_key_buffer( key_buffer + c_data_bytes_per_item,
                                  c_data_bytes_per_item, ( crypt_dpos / c_data_bytes_per_item ) + 1, p_impl->pwd_hash, p_impl->cipher_type );

                                 offs = crypt_dpos % c_data_bytes_per_item;

                                 crypt_data_buffer( buffer, key_buffer + offs, chunk );
                              }

                              crypt_dpos += chunk;
                              crypt_opos += chunk;

                              write_data_bytes( buffer, chunk, is_encrypted, is_encrypted );

                              if( p_progress )
                              {
                                 date_time now( date_time::local( ) );
                                 uint64_t elapsed = seconds_between( dtm, now );

                                 if( elapsed >= p_progress->num_seconds )
                                 {
                                    dtm = now;
                                    p_progress->output_progress( "." );
                                 }
                              }
                           }

                           // NOTE: If "move_free_data_to_end" has been called then this applies.
                           if( commit && tranlog_item.has_old_pos( )
                            && ( p_impl->rp_header_info->total_size_of_data
                            == ( tranlog_item.data_opos + tranlog_item.data_size ) ) )
                              p_impl->rp_header_info->total_size_of_data = dpos + tranlog_item.data_size;

                           if( commit
                            && ( p_impl->rp_header_info->total_size_of_data < ( dpos + tranlog_item.data_size ) ) )
                              p_impl->rp_header_info->total_size_of_data = dpos + tranlog_item.data_size;
                        }

                        write_entry = true;

                        index_entry.data.pos = dpos;
                        index_entry.data.size = tranlog_item.data_size;

                        index_entry.trans_flag = ods_index_entry::e_trans_none;
                     }
                     else
                        fs.seekg( tranlog_item.data_size, ios::cur );
                  }
                  else
                  {
                     if( commit && tranlog_item.is_destroy( ) && tranlog_item.is_post_op( ) )
                        add_to_free_list = true;
                     else if( tranlog_item.is_create( ) )
                     {
                        write_entry = true;

                        index_entry.trans_flag = ods_index_entry::e_trans_none;

                        if( !rollback_freelist_entries.count( commit_offs ) )
                           rollback_freelist_entries[ commit_offs ].push_back( tx_id );

                        rollback_freelist_entries[ commit_offs ].push_back( tranlog_item.index_entry_id );
                     }
                  }

                  if( tranlog_item.is_create( )
                   && ( p_impl->rp_header_info->index_free_list == tranlog_item.index_entry_id + 1 ) )
                     p_impl->rp_header_info->index_free_list = tranlog_item.data_opos;

                  if( commit && tranlog_item.is_destroy( ) && tranlog_item.is_post_op( ) )
                  {
                     bool clear_last = false;

                     int64_t pos_and_size = index_entry.data.pos + index_entry.data.size;

                     if( last_delete_size )
                     {
                        if( ( last_delete_pos + last_delete_size ) != index_entry.data.pos )
                        {
                           clear_last = true;
                           prior_deleted_bytes = 0;
                        }
                        else
                           prior_deleted_bytes += last_delete_size;
                     }

                     if( pos_and_size == p_impl->rp_header_info->total_size_of_data )
                     {
                        clear_last = true;
                        p_impl->rp_header_info->total_size_of_data -= ( index_entry.data.size + prior_deleted_bytes );
                     }

                     if( clear_last )
                     {
                        last_delete_pos = 0;
                        last_delete_size = 0;

                        prior_deleted_bytes = 0;
                     }
                     else
                     {
                        last_delete_pos = index_entry.data.pos;
                        last_delete_size = index_entry.data.size;
                     }
                  }

                  if( add_to_free_list )
                  {
                     if( !p_impl->rp_header_info->index_free_list )
                     {
                        index_entry.data.pos = 0;
                        p_impl->rp_header_info->index_free_list = tranlog_item.index_entry_id + 1;
                     }
                     else
                     {
                        index_entry.data.pos = p_impl->rp_header_info->index_free_list;
                        p_impl->rp_header_info->index_free_list = tranlog_item.index_entry_id + 1;
                     }

                     write_entry = true;

                     index_entry.data.size = 0;
                     index_entry.trans_flag = ods_index_entry::e_trans_free_list;
                  }

                  if( write_entry )
                  {
                     index_entry.data.tran_id = tran_id;
                     index_entry.data.tran_op = 0;

                     write_index_entry( index_entry, tranlog_item.index_entry_id );

                     ++last_index_transform_id;
                  }
               }

               if( tranlog_entry.commit_items )
                  rollback_freelist_entries.erase( commit_offs );

               if( rollback_freelist_entries.count( commit_offs ) )
               {
                  int64_t tran_id = rollback_freelist_entries[ commit_offs ].front( );

                  rollback_freelist_entries[ commit_offs ].pop_front( );

                  while( !rollback_freelist_entries[ commit_offs ].empty( ) )
                  {
                     int64_t index_entry_id = rollback_freelist_entries[ commit_offs ].back( );

                     if( index_entry_id >= 0 )
                        add_entry_to_freelist( index_entry_id, tran_id );

                     rollback_freelist_entries[ commit_offs ].pop_back( );
                  }

                  ++last_index_transform_id;
                  ++p_impl->rp_header_info->index_transform_id;
               }

               rollback_freelist_entries.erase( commit_offs );

               if( had_any_data )
               {
                  ++last_data_transform_id;
                  ++p_impl->rp_header_info->data_transform_id;

                  ++p_impl->rp_header_info->index_transform_id;
               }
            }
            else
            {
               if( !next_offs )
                  break;

               fs.seekg( next_offs, ios::beg );
            }

            last_offs = next_offs;

            if( fs.tellg( ) > tranlog_info.entry_offs )
               break;
         }

         fs.close( );
      }
   }

   if( is_encrypted )
      set_write_data_pos( -1, true, true );

   if( had_any_entries )
   {
      data_and_index_write( false );

      if( is_reconstruct )
         p_impl->rp_header_info->tranlog_offset = tranlog_offset;
      else
      {
         tranlog_info.entry_offs = last_entry_offs;
         tranlog_info.entry_time = last_entry_time;

         tranlog_info.append_offs = last_append_offs;

         log_stream logf( p_impl->tranlog_file_name.c_str( ), use_sync_write );

         tranlog_info.write( logf );

         p_impl->rp_header_info->tranlog_offset = last_entry_offs;
         p_impl->rp_header_info->transaction_id = last_tx_id + 1;

         p_impl->rp_header_info->data_transform_id = last_data_transform_id;
         p_impl->rp_header_info->index_transform_id = last_index_transform_id;
      }
   }

   p_impl->rp_header_info->num_trans = 0;
   p_impl->rp_header_info->num_writers = 0;

   bool for_close = false;

   // NOTE: If no write has occurred then act as though
   // one has in order to ensure that the "num_writers"
   // will be correctly zeroed.
   if( !*p_impl->rp_has_changed )
   {
      for_close = true;

      *p_impl->rp_has_changed = true;
      ++p_impl->rp_header_info->num_writers;
   }

   p_impl->write_header_file_info( for_close );

   *p_impl->rp_has_changed = false;
}

ods& operator >>( ods& o, storable_base& s )
{
   guard lock_storable( s.storable_lock );
   guard lock_read( o.read_lock );

   DEBUG_LOG( "[read object]" );

   if( !o.okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   if( o.is_in_read )
      THROW_ODS_ERROR( "invalid attempt to re-enter database instance read" );

   if( s.id.get_num( ) < 0 )
      THROW_ODS_ERROR( "unexpected null object id at " STRINGIZE( __LINE__ ) );

   if( *o.p_impl->rp_bulk_level && ( *o.p_impl->rp_bulk_mode < ods::impl::e_bulk_mode_read ) )
      THROW_ODS_ERROR( "cannot read when bulk locked for dumping" );

   temp_set_value< bool > tmp_in_read( o.is_in_read, true );

   finalise_value< int64_t > finalise_reading_object( o.current_read_object_num, -1 );

   int64_t trans_read_pos = -1;

   ods_index_entry index_entry;

   storable_base::read_scope read_scope( s, o );

   o.bytes_retrieved = 0;

   o.data_read_buffer_num = -1;

   memset( o.p_impl->data_read_key_buffer.data, '\0', c_data_bytes_per_item );

   bool can_read = false;

   bool has_locked = false;
   bool was_locked = false;

   int attempts = c_review_max_attempts;

   while( attempts-- )
   {
      // NOTE: Start file lock section.
      {
         guard tmp_lock( *o.p_impl->rp_impl_lock );
         ods::header_file_lock header_file_lock( o );

         auto_ptr< ods::file_scope > ap_file_scope;

         if( !*o.p_impl->rp_bulk_level )
            ap_file_scope.reset( new ods::file_scope( o ) );

         if( s.id.get_num( ) < o.p_impl->rp_header_info->total_entries )
            can_read = true;
         else
            can_read = false;

         was_locked = false;

         if( can_read )
         {
            if( o.p_impl->found_instance_currently_writing( s.id.get_num( ) ) )
               THROW_ODS_ERROR( "cannot read object currently being written" );

            o.read_index_entry( index_entry, s.id.get_num( ) );

            o.bytes_stored = index_entry.data.size;

            if( index_entry.lock_flag != ods_index_entry::e_lock_none )
               can_read = false;
            else
            {
               if( !o.p_impl->rp_ods_index_cache_buffer->lock_entry( s.id.get_num( ), false ) )
               {
                  can_read = false;
                  was_locked = true;
               }
               else
                  has_locked = true;
            }
         }

         if( can_read )
         {
            if( o.had_interim_trans_op_write )
            {
               o.trans_read_ops_buffer_num = -1;
               o.had_interim_trans_op_write = false;
            }

            if( o.had_interim_trans_data_write )
            {
               o.trans_read_data_buffer_num = -1;
               o.trans_read_data_buffer_offs = 0;
               o.had_interim_trans_data_write = false;

               if( o.p_impl->is_encrypted )
                  memset( o.trans_read_buffer.data, '\0', c_trans_bytes_per_item );
            }

            if( ( index_entry.trans_flag == ods_index_entry::e_trans_none )
             || ( index_entry.data.tran_id != o.p_impl->p_trans_buffer->tran_id ) )
            {
               o.p_impl->read_from_trans = false;
               s.last_size = index_entry.data.size;
            }
            else
            {
               o.p_impl->read_from_trans = true;

               transaction_op op;
               o.read_transaction_op( op, index_entry.data.tran_op - 1 );

               s.last_size = op.data.size;
               trans_read_pos = op.data.pos;
               o.bytes_stored = op.data.size;
            }

            ++( *o.p_impl->rp_session_review_total );

            o.current_read_object_num = s.id.get_num( );

            break;
         }
      }
      // NOTE: Finish file lock section.

      msleep( c_review_attempt_sleep_time );
   }

   if( !can_read )
   {
      if( !was_locked )
         THROW_ODS_ERROR( "cannot read object (max. attempts exceeded)" );
      else
         THROW_ODS_ERROR( "cannot read from buffer (max. attempts exceeded)" );
   }

   if( o.p_impl->read_from_trans )
      o.set_read_trans_data_pos( trans_read_pos );
   else
      o.set_read_data_pos( index_entry.data.pos );

   s.last_tran_id = index_entry.data.tran_id;

   ods::ods_stream ods_stream( o );

   s.get_instance( ods_stream );

   if( has_locked )
      o.p_impl->rp_ods_index_cache_buffer->unlock_entry( s.id.get_num( ), false );

   if( o.bytes_retrieved > o.bytes_stored )
   {
      ostringstream osstr;

      osstr << "max. data length exceeded (retrieved = "
       << o.bytes_retrieved << ", stored = " << o.bytes_stored << ")";

      THROW_ODS_ERROR( osstr.str( ) );
   }

   return o;
}

ods& operator <<( ods& o, storable_base& s )
{
   guard lock_storable( s.storable_lock );
   guard lock_write( o.write_lock );

   DEBUG_LOG( "[write object]" );

   if( !o.okay )
      THROW_ODS_ERROR( "database instance in bad state" );

   if( o.is_in_write )
      THROW_ODS_ERROR( "invalid attempt to re-enter database instance write" );

   if( *o.p_impl->rp_bulk_level && ( *o.p_impl->rp_bulk_mode != ods::impl::e_bulk_mode_write ) )
      THROW_ODS_ERROR( "cannot write when database bulk locked for dumping or reading" );

   if( o.p_impl->is_read_only )
      THROW_ODS_ERROR( "attempt to perform write when database was opened for read only access" );

   temp_set_value< bool > tmp_in_write( o.is_in_write, true );

   finalise_value< int64_t > finalise_writing_object( o.current_write_object_num, -1 );

   int64_t old_tran_id = -1;
   int64_t trans_write_pos = -1;

   ods_index_entry index_entry, old_index_entry;

   bool is_new_object = true;
   bool was_updated_in_place = false;

   storable_base::write_scope write_scope( s, o );

   o.bytes_used = 0;
   o.bytes_reserved = s.get_size_of( );

   bool can_write = false;
   bool has_locked = false;

   unsigned char flags = 0;

   int attempts = c_update_max_attempts;

   while( attempts-- )
   {
      // NOTE: Start file lock section.
      {
         guard tmp_lock( *o.p_impl->rp_impl_lock );
         ods::header_file_lock header_file_lock( o );

         auto_ptr< ods::file_scope > ap_file_scope;

         if( !*o.p_impl->rp_bulk_level )
            ap_file_scope.reset( new ods::file_scope( o ) );

#ifdef ODS_DEBUG
         ostringstream osstr;

         osstr << "oid: num = " << s.id.get_num( );

         DEBUG_LOG( osstr.str( ) );
#endif
         if( s.id.get_num( ) >= 0 )
         {
            if( s.id.get_num( ) >= o.p_impl->rp_header_info->total_entries )
               THROW_ODS_ERROR( "cannot store out of range oid" );

            if( o.p_impl->found_instance_currently_reading( s.id.get_num( ) ) )
               THROW_ODS_ERROR( "cannot write object currently being read" );

            if( o.p_impl->found_instance_currently_writing( s.id.get_num( ) ) )
               THROW_ODS_ERROR( "cannot write object currently being written" );

            o.read_index_entry( index_entry, s.id.get_num( ) );

            old_tran_id = index_entry.data.tran_id;

            old_index_entry = index_entry;

            is_new_object = false;
         }

         int64_t old_data_pos = 0;
         int64_t old_total_size = o.p_impl->rp_header_info->total_size_of_data;

         int64_t* p_total_to_increment = 0;

         if( is_new_object )
         {
            if( !o.p_impl->rp_header_info->index_free_list )
            {
               s.id.num = o.p_impl->rp_header_info->total_entries;
               p_total_to_increment = &*o.p_impl->rp_session_create_total;
            }
            else
            {
               o.read_index_entry( index_entry, o.p_impl->rp_header_info->index_free_list - 1 );

               if( index_entry.lock_flag != ods_index_entry::e_lock_none )
                  THROW_ODS_ERROR( "unexpected lock flag value found in freelist entry" );

               if( index_entry.trans_flag != ods_index_entry::e_trans_free_list )
                  THROW_ODS_ERROR( "unexpected trans flag value found in freelist entry" );

               index_entry.trans_flag = ods_index_entry::e_trans_none;
               s.id.num = o.p_impl->rp_header_info->index_free_list - 1;

               old_data_pos = index_entry.data.pos;
               index_entry.data.pos = 0;

               p_total_to_increment = &*o.p_impl->rp_session_revive_total;
            }

#ifdef ODS_DEBUG
            ostringstream osstr;

            osstr << "oid (new): num = " << s.id.get_num( );

            DEBUG_LOG( osstr.str( ) );
#endif
            can_write = true;
         }
         else
         {
            if( index_entry.lock_flag == ods_index_entry::e_lock_none )
            {
               if( ( index_entry.trans_flag == ods_index_entry::e_trans_none )
                || ( o.p_impl->trans_level
                && ( index_entry.data.tran_id == o.p_impl->p_trans_buffer->tran_id )
                && ( index_entry.trans_flag != ods_index_entry::e_trans_delete ) ) )
               {
                  can_write = true;
                  p_total_to_increment = &*o.p_impl->rp_session_update_total;
               }
            }
         }

         if( can_write )
         {
            if( !o.p_impl->rp_ods_index_cache_buffer->lock_entry( s.id.get_num( ), true ) )
               can_write = false;
            else
               has_locked = true;
         }

         if( can_write )
         {
            if( p_total_to_increment == &*o.p_impl->rp_session_create_total )
            {
               DEBUG_LOG( "(create index entry)" );

               flags |= c_log_entry_item_op_create;
               ++o.p_impl->rp_header_info->total_entries;
            }
            else if( p_total_to_increment == &*o.p_impl->rp_session_revive_total )
            {
               DEBUG_LOG( "(revive index entry)" );

               flags |= c_log_entry_item_op_create;
               o.p_impl->rp_header_info->index_free_list = old_data_pos;
            }
            else
            {
               DEBUG_LOG( "(update index entry)" );

               flags |= c_log_entry_item_op_update;
            }

            ++*p_total_to_increment;

            bool skip_log_entry = false;

            int64_t old_tx_id = index_entry.data.tran_id;

            transaction_op op;

            if( o.p_impl->trans_level )
            {
               op.data.id = s.id;
               op.data.size = o.bytes_reserved;

               if( !is_new_object && o.bytes_reserved <= index_entry.data.size )
                  op.type = transaction_op::e_op_type_update;
               else
                  op.type = transaction_op::e_op_type_append;

               op.data.old_tran_id = old_tran_id;

               if( index_entry.trans_flag != ods_index_entry::e_trans_none )
                  op.data.old_tran_op = index_entry.data.tran_op;

               op.data.pos =
                o.p_impl->p_trans_buffer->levels.top( ).offset
                + o.p_impl->p_trans_buffer->levels.top( ).size;

               index_entry.data.tran_op =
                o.p_impl->p_trans_buffer->levels.top( ).op_offset
                + o.p_impl->p_trans_buffer->levels.top( ).op_count + 1;

               if( !is_new_object && ( old_index_entry.data.tran_id == o.p_impl->p_trans_buffer->tran_id ) )
                  skip_log_entry = true;

               index_entry.trans_flag = ods_index_entry::e_trans_change;

               old_index_entry.data.tran_id = index_entry.data.tran_id = o.p_impl->p_trans_buffer->tran_id;
            }
            else
            {
               if( !is_new_object && ( o.bytes_reserved <= index_entry.data.size ) )
                  was_updated_in_place = true;
               else
                  index_entry.data.pos = old_total_size;

               index_entry.data.size = o.bytes_reserved;

               old_index_entry.data.tran_id = index_entry.data.tran_id = o.p_impl->rp_header_info->transaction_id;
            }

            if( !*o.p_impl->rp_bulk_level && !o.p_impl->trans_level )
               index_entry.lock_flag = ods_index_entry::e_lock_entry;

            if( !o.p_impl->trans_level )
            {
               if( o.p_impl->using_tranlog )
               {
                  o.p_impl->tranlog_offset = o.append_log_entry( o.p_impl->rp_header_info->transaction_id );

                  if( !o.p_impl->rp_header_info->tranlog_offset )
                     o.p_impl->rp_header_info->tranlog_offset = o.p_impl->tranlog_offset;
               }

               ++o.p_impl->rp_header_info->transaction_id;

               if( !was_updated_in_place )
                  o.p_impl->rp_header_info->total_size_of_data += o.bytes_reserved;
            }

            if( !skip_log_entry && o.p_impl->using_tranlog )
               o.append_log_entry_item( s.id.num,
                ( !is_new_object ? old_index_entry : index_entry ),
                flags, ( o.p_impl->trans_level > 0 ), old_tx_id, old_data_pos, o.p_progress );

            o.write_index_entry( index_entry, s.id.num );

            if( !skip_log_entry )
               ++o.p_impl->rp_header_info->index_transform_id;

            if( o.p_impl->trans_level )
            {
               ++o.p_impl->p_trans_buffer->levels.top( ).op_count;
               o.p_impl->p_trans_buffer->levels.top( ).size += o.bytes_reserved;

               o.write_transaction_op( op );

               ++o.p_impl->total_trans_op_count;
               trans_write_pos = o.p_impl->total_trans_size;

               o.p_impl->total_trans_size += o.bytes_reserved;
            }

            o.current_write_object_num = s.id.get_num( );

            break;
         }
      }
      // NOTE: Finish file lock section.

      msleep( c_update_attempt_sleep_time );
   }

   if( !can_write )
      THROW_ODS_ERROR( "cannot write (max. attempts exceeded)" );

   if( !o.p_impl->trans_level )
      o.set_write_data_pos( index_entry.data.pos );
   else
      o.set_write_trans_data_pos( trans_write_pos );

   if( old_tran_id != s.last_tran_id )
      s.flags |= storable_base::e_flag_interim_update;

   ods::ods_stream ods_stream( o );

   if( o.bytes_reserved )
      s.put_instance( ods_stream );

   if( o.bytes_used < o.bytes_reserved )
   {
#ifdef ODS_DEBUG
      ostringstream osstr;

      osstr << "<<< adding zero padding for "
       << ( o.bytes_reserved - o.bytes_used )
       << " unused bytes that were reserved >>>";

      DEBUG_LOG( osstr.str( ) );
#endif
      restorable< bool > force_padding( o.p_impl->force_padding, true );

      byte_skip bs( o.bytes_reserved - o.bytes_used );

      ods_stream << bs;
   }

   // NOTE: Start file lock section.
   {
      guard tmp_lock( *o.p_impl->rp_impl_lock );
      ods::header_file_lock header_file_lock( o );

      auto_ptr< ods::file_scope > ap_file_scope;

      if( !*o.p_impl->rp_bulk_level )
         ap_file_scope.reset( new ods::file_scope( o ) );

      flags &= ~c_log_entry_item_mask_op;
      flags |= c_log_entry_item_op_store | c_log_entry_item_flag_is_post_op;

      if( !o.p_impl->trans_level )
      {
         if( o.data_write_buffer_num != -1 )
         {
            if( o.p_impl->is_encrypted )
               crypt_data_buffer( o.p_impl->data_write_buffer.data,
                o.p_impl->data_write_key_buffer.data, c_data_bytes_per_item );

            o.p_impl->rp_ods_data_cache_buffer->put( o.p_impl->data_write_buffer, o.data_write_buffer_num );

            o.p_impl->rp_ods_data_cache_buffer->unlock_region(
             o.data_write_buffer_num * c_data_bytes_per_item, c_data_bytes_per_item );

            o.data_write_buffer_num = -1;
            o.data_write_buffer_offs = 0;

            if( o.p_impl->is_encrypted )
               memset( o.p_impl->data_write_key_buffer.data, '\0', c_data_bytes_per_item );

            if( o.p_impl->using_tranlog )
               o.append_log_entry_item( s.id.num, index_entry, flags, false, 0, 0, o.p_progress );

            ++o.p_impl->rp_header_info->data_transform_id;
         }

         ++o.p_impl->rp_header_info->index_transform_id;

         if( !*o.p_impl->rp_bulk_level )
         {
            index_entry.lock_flag = ods_index_entry::e_lock_none;
            o.write_index_entry( index_entry, s.id.num );
         }
      }

      if( has_locked )
         o.p_impl->rp_ods_index_cache_buffer->unlock_entry( s.id.get_num( ), true );

      if( o.p_impl->trans_level && o.trans_write_ops_buffer_num != -1 )
      {
         o.p_impl->p_ods_trans_op_cache_buffer->put( o.trans_write_ops_buffer, o.trans_write_ops_buffer_num );

         o.trans_write_ops_buffer_num = -1;
         o.had_interim_trans_op_write = true;
      }

      if( o.p_impl->trans_level && ( o.trans_write_data_buffer_num != -1 ) )
      {
         if( o.p_impl->is_encrypted )
            crypt_data_buffer( o.trans_write_buffer.data,
             o.trans_write_key_buffer.data, c_trans_bytes_per_item );

         o.p_impl->p_ods_trans_data_cache_buffer->put( o.trans_write_buffer, o.trans_write_data_buffer_num );

         o.trans_write_data_buffer_num = -1;
         o.trans_write_data_buffer_offs = 0;
         o.had_interim_trans_data_write = true;

         if( o.p_impl->is_encrypted )
            memset( o.trans_write_buffer.data, '0', c_trans_bytes_per_item );
      }
   }
   // NOTE: Finish file lock section.

   if( o.bytes_used > o.bytes_reserved )
   {
      ostringstream osstr;

      osstr << "max. data length exceeded (reserved = "
       << o.bytes_reserved << ", used = " << o.bytes_used << ")";

      THROW_ODS_ERROR( osstr.str( ) );
   }

   return o;
}

void ods::read( unsigned char* p_buf, int64_t len )
{
#ifdef ODS_DEBUG
   ostringstream osstr;

   osstr << "ods::read (len = " << len << ")";

   DEBUG_LOG( osstr.str( ) );
#endif

   bytes_retrieved += len;
   if( bytes_retrieved <= bytes_stored )
   {
      if( !p_impl->read_from_trans )
         read_data_bytes( ( char* )p_buf, len );
      else
         read_trans_data_bytes( ( char* )p_buf, len );
   }
}

void ods::write( const unsigned char* p_buf, int64_t len )
{
#ifdef ODS_DEBUG
   ostringstream osstr;

   osstr << "ods::write (len = " << len << ")";

   DEBUG_LOG( osstr.str( ) );
#endif

   bytes_used += len;

   if( bytes_used <= bytes_reserved )
   {
      if( !p_impl->trans_level )
         write_data_bytes( ( char* )p_buf, len );
      else
         write_trans_data_bytes( ( char* )p_buf, len );
   }
}

void ods::set_read_data_pos( int64_t pos, bool force_get, bool skip_decrypt )
{
#ifdef ODS_DEBUG
   ostringstream osstr;

   osstr << "ods::set_read_data_pos( ) pos = " << pos << ", data_read_buffer_num = " << data_read_buffer_num;

   DEBUG_LOG( osstr.str( ) );
#endif

   if( pos == -1 )
   {
      data_read_buffer_num = -1;
      data_read_buffer_offs = 0;
   }
   else
   {
      int64_t current_data_buffer_num = data_read_buffer_num;

      data_read_buffer_num = pos / c_data_bytes_per_item;
      data_read_buffer_offs = pos % c_data_bytes_per_item;

      if( force_get || ( data_read_buffer_num != current_data_buffer_num ) )
      {
         p_impl->data_read_buffer = p_impl->rp_ods_data_cache_buffer->get( data_read_buffer_num );

         if( !skip_decrypt && p_impl->is_encrypted )
         {
            init_key_buffer( p_impl->data_read_key_buffer.data,
             c_data_bytes_per_item, data_read_buffer_num, p_impl->pwd_hash, p_impl->cipher_type );

            crypt_data_buffer( p_impl->data_read_buffer.data,
             p_impl->data_read_key_buffer.data, c_data_bytes_per_item );
         }
      }
   }
}

void ods::set_write_data_pos( int64_t pos, bool skip_decrypt, bool skip_encrypt )
{
#ifdef ODS_DEBUG
   ostringstream osstr;

   osstr << "ods::set_write_data_pos( ) pos = " << pos
    << ", data_write_buffer_num = " << data_write_buffer_num;

   DEBUG_LOG( osstr.str( ) );
#endif

   int64_t current_data_buffer_num( data_write_buffer_num );

   if( pos == -1 )
   {
      data_write_buffer_num = -1;
      data_write_buffer_offs = 0;
   }
   else
   {
      data_write_buffer_num = pos / c_data_bytes_per_item;
      data_write_buffer_offs = pos % c_data_bytes_per_item;
   }

   if( data_write_buffer_num != current_data_buffer_num )
   {
      if( current_data_buffer_num != -1 )
      {
         if( !skip_encrypt && p_impl->is_encrypted )
         {
            init_key_buffer( p_impl->data_write_key_buffer.data,
             c_data_bytes_per_item, current_data_buffer_num, p_impl->pwd_hash, p_impl->cipher_type );

            crypt_data_buffer( p_impl->data_write_buffer.data,
             p_impl->data_write_key_buffer.data, c_data_bytes_per_item );
         }

         p_impl->rp_ods_data_cache_buffer->put( p_impl->data_write_buffer, current_data_buffer_num );

         p_impl->rp_ods_data_cache_buffer->unlock_region(
          current_data_buffer_num * c_data_bytes_per_item, c_data_bytes_per_item );
      }

      if( data_write_buffer_num != -1 )
      {
         int attempts = c_data_lock_max_attempts;

         while( attempts )
         {
            if( p_impl->rp_ods_data_cache_buffer->lock_region(
             data_write_buffer_num * c_data_bytes_per_item, c_data_bytes_per_item ) )
               break;

            --attempts;
            msleep( c_data_lock_attempt_sleep_time );
         }

         if( !attempts )
            THROW_ODS_ERROR( "unable to lock data region for set_write_data_pos" );

         p_impl->data_write_buffer = p_impl->rp_ods_data_cache_buffer->get( data_write_buffer_num );

         if( !skip_decrypt && p_impl->is_encrypted )
         {
            init_key_buffer( p_impl->data_write_key_buffer.data,
             c_data_bytes_per_item, data_write_buffer_num, p_impl->pwd_hash, p_impl->cipher_type );

            crypt_data_buffer( p_impl->data_write_buffer.data,
             p_impl->data_write_key_buffer.data, c_data_bytes_per_item );
         }
      }
   }
}

void ods::adjust_read_data_pos( int64_t adjust, bool skip_decrypt )
{
#ifdef ODS_DEBUG
   ostringstream osstr;

   osstr << "ods::adjust_read_data_pos( ) adjust = " << adjust;

   DEBUG_LOG( osstr.str( ) );
#endif

   int64_t current_data_buffer_num( data_read_buffer_num );
   int64_t pos = ( data_read_buffer_num * c_data_bytes_per_item ) + data_read_buffer_offs + adjust;

   data_read_buffer_num = pos / c_data_bytes_per_item;
   data_read_buffer_offs = pos % c_data_bytes_per_item;

   if( data_read_buffer_num != current_data_buffer_num )
   {
      p_impl->data_read_buffer = p_impl->rp_ods_data_cache_buffer->get( data_read_buffer_num );

      if( !skip_decrypt && p_impl->is_encrypted )
      {
         init_key_buffer( p_impl->data_read_key_buffer.data,
          c_data_bytes_per_item, data_read_buffer_num, p_impl->pwd_hash, p_impl->cipher_type );

         crypt_data_buffer( p_impl->data_read_buffer.data,
          p_impl->data_read_key_buffer.data, c_data_bytes_per_item );
      }
   }
}

void ods::read_data_bytes( char* p_dest, int64_t len, bool skip_decrypt )
{
   int64_t chunk = min( len, c_data_bytes_per_item - data_read_buffer_offs );

   while( len > 0 )
   {
      if( p_dest )
         memcpy( p_dest, &p_impl->data_read_buffer.data[ data_read_buffer_offs ], chunk );

      len -= chunk;

      if( len )
      {
         data_read_buffer_offs = 0;
         p_impl->data_read_buffer = p_impl->rp_ods_data_cache_buffer->get( ++data_read_buffer_num );

         if( !skip_decrypt && p_impl->is_encrypted )
         {
            init_key_buffer( p_impl->data_read_key_buffer.data,
             c_data_bytes_per_item, data_read_buffer_num, p_impl->pwd_hash, p_impl->cipher_type );

            crypt_data_buffer( p_impl->data_read_buffer.data,
             p_impl->data_read_key_buffer.data, c_data_bytes_per_item );
         }

         if( p_dest )
            p_dest += chunk;

         chunk = min( len, ( int64_t )c_data_bytes_per_item );
      }
      else
         data_read_buffer_offs += chunk;
   }
}

void ods::write_data_bytes( const char* p_src, int64_t len, bool skip_decrypt, bool skip_encrypt )
{
   int64_t chunk = min( len, c_data_bytes_per_item - data_write_buffer_offs );

   while( len > 0 )
   {
      if( p_src )
         memcpy( &p_impl->data_write_buffer.data[ data_write_buffer_offs ], p_src, chunk );
      else if( p_impl->force_padding )
         memset( &p_impl->data_write_buffer.data[ data_write_buffer_offs ], '\0', chunk );

      len -= chunk;

      if( len )
      {
         if( !skip_encrypt && p_impl->is_encrypted )
            crypt_data_buffer( p_impl->data_write_buffer.data,
             p_impl->data_write_key_buffer.data, c_data_bytes_per_item );

         p_impl->rp_ods_data_cache_buffer->put(
          p_impl->data_write_buffer, data_write_buffer_num, true, prevent_lazy_write );

         p_impl->rp_ods_data_cache_buffer->unlock_region(
          data_write_buffer_num * c_data_bytes_per_item, c_data_bytes_per_item );

         ++data_write_buffer_num;
         data_write_buffer_offs = 0;

         int attempts = c_data_lock_max_attempts;

         while( attempts )
         {
            if( p_impl->rp_ods_data_cache_buffer->lock_region(
             data_write_buffer_num * c_data_bytes_per_item, c_data_bytes_per_item ) )
               break;

            --attempts;
            msleep( c_data_lock_attempt_sleep_time );
         }

         if( !attempts )
            THROW_ODS_ERROR( "unable to lock data region for write_data_bytes" );

         p_impl->data_write_buffer = p_impl->rp_ods_data_cache_buffer->get( data_write_buffer_num );

         if( !skip_decrypt && p_impl->is_encrypted )
         {
            init_key_buffer( p_impl->data_write_key_buffer.data,
             c_data_bytes_per_item, data_write_buffer_num, p_impl->pwd_hash, p_impl->cipher_type );

            crypt_data_buffer( p_impl->data_write_buffer.data,
             p_impl->data_write_key_buffer.data, c_data_bytes_per_item );
         }

         if( p_src )
            p_src += chunk;

         chunk = min( len, ( int64_t )c_data_bytes_per_item );
      }
      else
         data_write_buffer_offs += chunk;
   }
}

void ods::read_index_entry( ods_index_entry& index_entry, int64_t num )
{
   int64_t current_index_buffer_num( index_item_buffer_num );

   index_item_buffer_num = num / c_index_items_per_item;

   if( index_item_buffer_num != current_index_buffer_num )
   {
      if( current_index_buffer_num != -1 )
         p_impl->rp_ods_index_cache_buffer->put( p_impl->index_item_buffer, current_index_buffer_num );

      p_impl->index_item_buffer = p_impl->rp_ods_index_cache_buffer->get( index_item_buffer_num );
   }

   ods_index_entry::data_t& data = p_impl->index_item_buffer.item[ num % c_index_items_per_item ];

   index_entry.lock_flag = ods_index_entry::e_lock_none;
   index_entry.trans_flag = ods_index_entry::e_trans_none;

   index_entry.data = data;

   if( index_entry.data.pos & c_int_type_hi_bit )
   {
      index_entry.data.pos &= ~c_int_type_hi_bit;
      ( int& )index_entry.lock_flag |= c_bit_1;
   }

   if( index_entry.data.size & c_int_type_hi_bit )
   {
      index_entry.data.size &= ~c_int_type_hi_bit;
      ( int& )index_entry.lock_flag |= c_bit_2;
   }

   if( index_entry.data.tran_id & c_int_type_hi_bit )
   {
      index_entry.data.tran_id &= ~c_int_type_hi_bit;
      ( int& )index_entry.trans_flag |= c_bit_1;
   }

   if( index_entry.data.tran_op & c_int_type_hi_bit )
   {
      index_entry.data.tran_op &= ~c_int_type_hi_bit;
      ( int& )index_entry.trans_flag |= c_bit_2;
   }
}

void ods::write_index_entry( const ods_index_entry& index_entry, int64_t num )
{
   if( !*p_impl->rp_has_changed )
   {
      *p_impl->rp_has_changed = true;
      ++p_impl->rp_header_info->num_writers;
   }

   int64_t current_index_buffer_num( index_item_buffer_num );

   index_item_buffer_num = num / c_index_items_per_item;

   if( index_item_buffer_num != current_index_buffer_num )
   {
      if( current_index_buffer_num != -1 )
         p_impl->rp_ods_index_cache_buffer->put(
          p_impl->index_item_buffer, current_index_buffer_num, true, prevent_lazy_write );

      p_impl->index_item_buffer = p_impl->rp_ods_index_cache_buffer->get( index_item_buffer_num );
   }

   ods_index_entry::data_t& data = p_impl->index_item_buffer.item[ num % c_index_items_per_item ];

   data = index_entry.data;

   if( index_entry.lock_flag & c_bit_1 )
      data.pos |= c_int_type_hi_bit;

   if( index_entry.lock_flag & c_bit_2 )
      data.size |= c_int_type_hi_bit;

   if( index_entry.trans_flag & c_bit_1 )
      data.tran_id |= c_int_type_hi_bit;

   if( index_entry.trans_flag & c_bit_2 )
      data.tran_op |= c_int_type_hi_bit;
}

void ods::add_entry_to_freelist( int64_t num, int64_t tran_id )
{
   ods_index_entry index_entry;

   read_index_entry( index_entry, num );

   if( index_entry.trans_flag != ods_index_entry::e_trans_free_list )
   {
      if( !p_impl->rp_header_info->index_free_list )
      {
         index_entry.data.pos = 0;
         p_impl->rp_header_info->index_free_list = num + 1;
      }
      else
      {
         index_entry.data.pos = p_impl->rp_header_info->index_free_list;
         p_impl->rp_header_info->index_free_list = num + 1;
      }

      index_entry.data.size = 0;

      index_entry.data.tran_id = tran_id;
      index_entry.data.tran_op = 0;

      index_entry.trans_flag = ods_index_entry::e_trans_free_list;

      write_index_entry( index_entry, num );
   }
}

void ods::read_transaction_op( transaction_op& op, int64_t num )
{
   int64_t current_trans_ops_buffer_num( trans_read_ops_buffer_num );

   trans_read_ops_buffer_num = num / c_trans_ops_per_item;

   if( trans_read_ops_buffer_num != current_trans_ops_buffer_num )
      trans_read_ops_buffer = p_impl->p_ods_trans_op_cache_buffer->get( trans_read_ops_buffer_num );

   transaction_op::data_t& data( trans_read_ops_buffer.item[ num % c_trans_ops_per_item ] );

   op.data = data;
   op.type = transaction_op::e_op_type_none;

   if( op.data.pos & c_int_type_hi_bit )
   {
      ( int& )op.type |= c_bit_1;
      op.data.pos &= ~c_int_type_hi_bit;
   }

   if( op.data.size & c_int_type_hi_bit )
   {
      ( int& )op.type |= c_bit_2;
      op.data.size &= ~c_int_type_hi_bit;
   }
}

void ods::write_transaction_op( const transaction_op& op )
{
   int64_t current_trans_ops_buffer_num( trans_write_ops_buffer_num );

   trans_write_ops_buffer_num = p_impl->total_trans_op_count / c_trans_ops_per_item;

   if( trans_write_ops_buffer_num != current_trans_ops_buffer_num )
   {
      if( current_trans_ops_buffer_num != -1 )
         p_impl->p_ods_trans_op_cache_buffer->put( trans_write_ops_buffer, current_trans_ops_buffer_num );

      if( p_impl->total_trans_op_count % c_trans_ops_per_item == 0 )
         memset( ( char* )&trans_write_ops_buffer, '\0', sizeof( trans_op_buffer ) );
      else
         trans_write_ops_buffer = p_impl->p_ods_trans_op_cache_buffer->get( trans_write_ops_buffer_num );
   }

   transaction_op::data_t& data(
    trans_write_ops_buffer.item[ p_impl->total_trans_op_count % c_trans_ops_per_item ] );

   data = op.data;

   if( op.type & c_bit_1 )
      data.pos |= c_int_type_hi_bit;
   if( op.type & c_bit_2 )
      data.size |= c_int_type_hi_bit;
}

void ods::set_read_trans_data_pos( int64_t pos )
{
   int64_t current_trans_buffer_num( trans_read_data_buffer_num );

   trans_read_data_buffer_num = pos / c_trans_bytes_per_item;
   trans_read_data_buffer_offs = pos % c_trans_bytes_per_item;

#ifdef ODS_DEBUG
   ostringstream osstr;

   osstr << "ods::set_read_trans_data_pos( ) pos = " << pos
    << "\ntrans_read_data_buffer_num = " << trans_read_data_buffer_num
    << ", trans_read_data_buffer_offs = " << trans_read_data_buffer_offs;

   DEBUG_LOG( osstr.str( ) );
#endif

   if( trans_read_data_buffer_num != current_trans_buffer_num )
   {
      trans_read_buffer = p_impl->p_ods_trans_data_cache_buffer->get( trans_read_data_buffer_num );

      if( p_impl->is_encrypted )
      {
         init_key_buffer( trans_read_key_buffer.data,
          c_trans_bytes_per_item, trans_read_data_buffer_num, p_impl->pwd_hash, p_impl->cipher_type );

         crypt_data_buffer( trans_read_buffer.data,
          trans_read_key_buffer.data, c_trans_bytes_per_item );
      }
   }
}

void ods::set_write_trans_data_pos( int64_t pos )
{
   int64_t current_trans_buffer_num( trans_write_data_buffer_num );

   trans_write_data_buffer_num = pos / c_trans_bytes_per_item;
   trans_write_data_buffer_offs = pos % c_trans_bytes_per_item;

#ifdef ODS_DEBUG
   ostringstream osstr;

   osstr << "ods::set_write_trans_data_pos( ) pos = " << pos
    << "\ntrans_write_data_buffer_num = " << trans_write_data_buffer_num
    << ", trans_write_data_buffer_offs = " << trans_write_data_buffer_offs;

   DEBUG_LOG( osstr.str( ) );
#endif

   if( trans_write_data_buffer_num != current_trans_buffer_num )
   {
      if( current_trans_buffer_num != -1 )
      {
         if( p_impl->is_encrypted )
            crypt_data_buffer( trans_write_buffer.data,
             trans_write_key_buffer.data, c_trans_bytes_per_item );

         p_impl->p_ods_trans_data_cache_buffer->put( trans_write_buffer, current_trans_buffer_num );
      }

      if( p_impl->is_encrypted )
         init_key_buffer( trans_write_key_buffer.data,
          c_trans_bytes_per_item, trans_write_data_buffer_num, p_impl->pwd_hash, p_impl->cipher_type );

      if( pos % c_trans_bytes_per_item == 0 )
         memset( ( char* )&trans_write_buffer, '\0', sizeof( trans_data_buffer ) );
      else
      {
         trans_write_buffer = p_impl->p_ods_trans_data_cache_buffer->get( trans_write_data_buffer_num );

         if( p_impl->is_encrypted )
            crypt_data_buffer( trans_write_buffer.data,
             trans_write_key_buffer.data, c_trans_bytes_per_item );
      }
   }
}

void ods::read_trans_data_bytes( char* p_dest, int64_t len )
{
#ifdef ODS_DEBUG
   ostringstream osstr;

   osstr << "ods::read_trans_data_bytes( ) len = " << len
    << "\ntrans_read_data_buffer_num = " << trans_read_data_buffer_num
    << "\ntrans_read_data_buffer_offs = " << trans_read_data_buffer_offs;

   DEBUG_LOG( osstr.str( ) );
#endif

   int64_t chunk = min( len, c_trans_bytes_per_item - trans_read_data_buffer_offs );

   while( len > 0 )
   {
      if( p_dest )
         memcpy( p_dest, &trans_read_buffer.data[ trans_read_data_buffer_offs ], chunk );

      len -= chunk;

      if( len )
      {
         trans_read_data_buffer_offs = 0;
         trans_read_buffer = p_impl->p_ods_trans_data_cache_buffer->get( ++trans_read_data_buffer_num );

         if( p_impl->is_encrypted )
         {
            init_key_buffer( trans_read_key_buffer.data,
             c_trans_bytes_per_item, trans_read_data_buffer_num, p_impl->pwd_hash, p_impl->cipher_type );

            crypt_data_buffer( trans_read_buffer.data,
             trans_read_key_buffer.data, c_trans_bytes_per_item );
         }

         if( p_dest )
            p_dest += chunk;

         chunk = min( len, c_trans_bytes_per_item );
      }
      else
         trans_read_data_buffer_offs += chunk;
   }
}

void ods::write_trans_data_bytes( const char* p_src, int64_t len )
{
#ifdef ODS_DEBUG
   ostringstream osstr;

   osstr << "ods::write_trans_data_bytes( ) len = " << len
    << "\ntrans_write_data_buffer_num = " << trans_write_data_buffer_num
    << "\ntrans_write_data_buffer_offs = " << trans_write_data_buffer_offs;

   DEBUG_LOG( osstr.str( ) );
#endif

   int64_t chunk = min( len, c_trans_bytes_per_item - trans_write_data_buffer_offs );

   while( len > 0 )
   {
      if( p_src )
         memcpy( &trans_write_buffer.data[ trans_write_data_buffer_offs ], p_src, chunk );
      else if( p_impl->force_padding )
         memset( &trans_write_buffer.data[ trans_write_data_buffer_offs ], '\0', chunk );

      len -= chunk;

      if( len )
      {
         trans_write_data_buffer_offs = 0;

         if( p_impl->is_encrypted )
            crypt_data_buffer( trans_write_buffer.data,
             trans_write_key_buffer.data, c_trans_bytes_per_item );

         p_impl->p_ods_trans_data_cache_buffer->put( trans_write_buffer, trans_write_data_buffer_num++ );

         memset( &trans_write_buffer, '\0', sizeof( trans_data_buffer ) );

         if( p_impl->is_encrypted )
            init_key_buffer( trans_write_key_buffer.data,
             c_trans_bytes_per_item, trans_write_data_buffer_num, p_impl->pwd_hash, p_impl->cipher_type );

         if( p_src )
            p_src += chunk;

         chunk = min( len, c_trans_bytes_per_item );
      }
      else
         trans_write_data_buffer_offs += chunk;
   }
}
