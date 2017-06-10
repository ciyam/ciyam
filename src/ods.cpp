// Copyright (c) 2000-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2017 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <set>
#  include <stack>
#  include <vector>
#  include <memory>
#  include <iomanip>
#  include <sstream>
#  include <iostream>
#  include <algorithm>
#  include <cerrno>
#  include <cstdio>
#  ifdef __BORLANDC__
#     include <cio>
#     include <cfcntl>
#  endif
#  ifdef _MSC_VER
#     include <io.h>
#     include <fcntl.h>
#  endif
#  ifdef __GNUG__
#     include <fcntl.h>
#     include <unistd.h>
#     include <sys/time.h>
#  endif
#  ifdef _WIN32
#     include <share.h>
#     include <sys/locking.h>
#  endif
#  include <sys/stat.h>
#  ifdef _WIN32
#     ifndef STRICT
#        define STRICT // Needed for "windows.h" by various Borland headers.
#     endif
#     define NOMINMAX
#     include <windows.h>
#  endif
#endif

#ifdef __GNUG__
#  define _open open
#  define _read ::read
#  define _write ::write
#  define _close close
#  define _lseek lseek
#  define _access access
#endif

#ifdef _WIN32
#  define _lseek _lseeki64
#endif

#ifdef __BORLANDC__
#  define _locking locking
#endif

#include "ods.h"
#include "cache.h"
#include "macros.h"
#include "pointers.h"
#include "utilities.h"
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

const char* const c_ops_file_name_ext = ".ops";
const char* const c_data_file_name_ext = ".dat";
const char* const c_index_file_name_ext = ".idx";
const char* const c_header_file_name_ext = ".hdr";
const char* const c_tranlog_file_name_ext = ".tlg";

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

const int c_data_bytes_per_item = 4096;
const int c_data_max_cache_items = 500;
const int c_data_items_per_region = 10000;
const int c_data_num_cache_regions = 10;

const int c_index_items_per_item = 128;
const int c_index_max_cache_items = 500;
const int c_index_items_per_region = 10000;
const int c_index_num_cache_regions = 10;

const int c_trans_op_max_cache_items = 500;
const int c_trans_op_items_per_region = 10000;
const int c_trans_data_max_cache_items = 500;
const int c_trans_data_items_per_region = 10000;

mutex g_ods_lock;

#ifdef ODS_DEBUG
#  include <fstream>
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

const int_t c_bit_1 = UINT64_C( 1 );
const int_t c_bit_2 = UINT64_C( 2 );

// NOTE: It would be expected that if a "major" version change has occurred then any previous file
// format would no longer be compatible, however, if a "minor" version change has occurred then it
// should still be possible to operate on previous file formats (with the same "major" version).
const int32_t c_major_ver = 1;
const int32_t c_minor_ver = 0;

const int32_t c_version_id = ( c_major_ver << 16 ) | c_minor_ver;

const uint_t c_int_type_hi_bit = UINT64_C( 1 ) << ( std::numeric_limits< uint_t >::digits - 1 );

const uint_t c_int_type_low_bits = UINT64_C( -1 ) >> ( std::numeric_limits< uint_t >::digits / 2 );
const uint_t c_int_type_high_bits = c_int_type_low_bits << ( std::numeric_limits< uint_t >::digits / 2 );

const int c_buffer_chunk_size = 1024;

inline bool is_print( char ch ) { return ch >= ' ' && ch <= '~'; }

void char_buffer::operator =( const char* s )
{
   guard lock_buffer( buffer_lock );

   int_t n;
   for( n = 0; n < max - pos; n++ )
      if( ( p_data[ n ] = s[ n ] ) == '\0' )
         break;

   len = n;
}

void char_buffer::operator +=( const char* s )
{
   guard lock_buffer( buffer_lock );

   int_t n, o;
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

   for( int_t i = 0; i < len; i++ )
      if( src.p_data[ i ] != p_data[ i ] )
         return false;

   return true;
}

bool char_buffer::is_less_than( const char_buffer& src ) const
{
   guard lock_buffer( buffer_lock );

   int diff;
   int_t length = src.len;

   for( int_t i = 0; i < min( len, length ); i++ )
   {
      diff = src.p_data[ i ] - p_data[ i ];
      if( !diff )
         continue;
      return diff < 0 ? true : false;
   }

   return length < len;
}

int_t char_buffer::first_of( const char* matches ) const
{
   guard lock_buffer( buffer_lock );

   for( int_t i = 0; i < len; i++ )
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

   for( int_t i = 0; i < len; i++ )
   {
      for( const char* p = matches; *p != '\0'; ++p )
      {
         if( *p == p_data[ i ] )
            return true;
      }
   }

   return false;
}

bool char_buffer::begins_with( const char_buffer& src, int_t max ) const
{
   guard lock_buffer( buffer_lock );

   if( max == 0 )
      max = src.len;
   if( max > len )
      max = len;

   for( int_t i = 0; i < max; ++i )
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

   for( int_t s = 0; s < c.len; s++ )
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

ods_error::ods_error( const char* s )
{
   init( s );
}

ods_error::ods_error( const string& msg )
{
   init( msg.c_str( ) );
}

void ods_error::init( const char* s )
{
#ifdef ODS_DEBUG
   DEBUG_LOG( "================ ods exception was thrown ================" );
   DEBUG_LOG( s );
   DEBUG_LOG( "==========================================================" );
#endif
   buf[ ODS_MAX_ERROR_MSG_LENGTH - 1 ] = '\0';
   strncpy( buf, s, ODS_MAX_ERROR_MSG_LENGTH - 1 );
}

struct header_info
{
   int32_t version_id;
   int32_t num_writers;

   int_t total_entries;
   int_t index_free_list;
   int_t transaction_id;
   int_t total_size_of_data;
   int_t data_transform_id;
   int_t index_transform_id;

   header_info( )
    :
    num_writers( 0 ),
    total_entries( 0 ),
    index_free_list( 0 ),
    transaction_id( 0 ),
    total_size_of_data( 0 ),
    data_transform_id( 0 ),
    index_transform_id( 0 )
   {
   }

   bool operator ==( const header_info& other ) const
   {
      return version_id == other.version_id && num_writers == other.num_writers
       && total_entries == other.total_entries && index_free_list == other.index_free_list
       && transaction_id == other.transaction_id && total_size_of_data == other.total_size_of_data
       && data_transform_id == other.data_transform_id && index_transform_id == other.index_transform_id;
   }

   bool operator !=( const header_info& other ) const
   {
      return !( *this == other );
   }
};

class ods_index_entry
{
   public:
   struct data_t
   {
      data_t( ) : pos( 0 ), size( 0 ), tran_id( 0 ), tran_op( 0 ) { }

      int_t pos;
      int_t size;
      int_t tran_id;
      int_t tran_op;
   } data;

   private:
   ods_index_entry( );

   void dump_entry( ostream& os, int_t num );

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

   int_t get_pos( ) const { return data.pos; }
   int_t get_size( ) const { return data.size; }
   int_t get_tran_id( ) const { return data.tran_id; }
   int_t get_tran_op( ) const { return data.tran_op; }

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
   header_file( const char* p_file_name, bool is_exclusive );

   ~header_file( );

   operator int( ) const { return handle; }

   bool lock( );
   void unlock( );

   void acquire_lock_offset( );
   void release_lock_offset( );

   int get_lock_offset( ) const { return lock_offset; }

   bool is_locked_for_exclusive( ) const { return is_exclusive; }

   private:
   int handle;
   short lock_offset;
   bool is_exclusive;

#ifdef __GNUG__
   flock file_lock;
   flock header_lock;
#endif
};

header_file::header_file( const char* p_file_name, bool is_exclusive )
 :
 handle( 0 ),
 lock_offset( -1 ),
 is_exclusive( is_exclusive )
{
#ifdef __GNUG__
   handle = _open( p_file_name, O_RDWR | O_CREAT, ODS_DEFAULT_PERMS );

   if( handle > 0 )
   {
      if( !is_exclusive )
         file_lock.l_type = F_RDLCK;
      else
         file_lock.l_type = F_WRLCK;

      file_lock.l_len = 1;
      file_lock.l_start = 0;
      file_lock.l_whence = SEEK_SET;

      if( fcntl( handle, F_SETLK, &file_lock ) != 0 )
      {
         _close( handle );
         handle = 0;
      }

      if( handle )
         lock_offset = 0;
   }
#endif

#ifdef _WIN32
   int sflags = SH_DENYNO;
   if( is_exclusive )
      sflags = SH_DENYRW;

   handle = _sopen( p_file_name, O_BINARY | _O_RDWR | _O_CREAT, sflags, S_IREAD | S_IWRITE );
#endif

   if( handle > 0 )
      acquire_lock_offset( );
#ifdef ODS_DEBUG
   ostringstream osstr;
   osstr << "handle = " << handle << ", lock_offset = " << lock_offset;
   DEBUG_LOG( osstr.str( ) );
#endif
}

header_file::~header_file( )
{
   if( handle > 0 )
   {
#ifdef __GNUG__
      file_lock.l_type = F_UNLCK;

      if( fcntl( handle, F_SETLK, &file_lock ) != 0 )
         DEBUG_LOG( "unexpected fcntl failure occurred at " STRINGIZE( __LINE__ ) );
#endif
      release_lock_offset( );

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
         DEBUG_LOG( "closed file" );
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
      header_lock.l_start = 1;
      header_lock.l_whence = SEEK_SET;

      if( fcntl( handle, F_SETLK, &header_lock ) == 0 )
         retval = true;
#endif

#ifdef _WIN32
      if( _lseek( handle, 0, SEEK_SET ) != 0 )
         throw ods_error( "unexpected _lseek result at " STRINGIZE( __LINE__ ) );

      if( _locking( handle, LK_NBLCK, 1 ) == 0 )
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
         throw ods_error( "unexpected fcntl at " STRINGIZE( __LINE__ ) " failed..." );
#endif

#ifdef _WIN32
      if( _lseek( handle, 0, SEEK_SET ) != 0 )
         throw ods_error( "unexpected _lseek result at " STRINGIZE( __LINE__ ) );

      if( _locking( handle, LK_UNLCK, 1 ) != 0 )
         throw ods_error( "unexpected _locking at " STRINGIZE( __LINE__ ) " failed..." );
#endif
   }
}

void header_file::acquire_lock_offset( )
{
#ifdef _WIN32
   if( handle > 0 )
   {
      for( size_t i = sizeof( header_info );
       i < sizeof( header_info ) + sizeof( ods_index_entry::data_t ); i++ )
      {
         if( _lseek( handle, ( int64_t )i, SEEK_SET ) != ( int64_t )i )
            throw ods_error( "unexpected _lseek result at " STRINGIZE( __LINE__ ) );

         if( _locking( handle, LK_NBLCK, 1 ) == 0 )
         {
            lock_offset = ( short )( i - sizeof( header_info ) );
            break;
         }
      }
   }
#endif
}

void header_file::release_lock_offset( )
{
#ifdef _WIN32
   if( handle > 0 && lock_offset >= 0 )
   {
      DEBUG_LOG( "releasing lock offset" );

      int64_t pos = lock_offset + sizeof( header_info );
      if( _lseek( handle, pos, SEEK_SET ) != pos )
         throw ods_error( "unexpected _lseek result at " STRINGIZE( __LINE__ ) );
      if( _locking( handle, LK_UNLCK, 1 ) != 0 )
         throw ods_error( "unexpected _locking at " STRINGIZE( __LINE__ ) " failed..." );

      lock_offset = -1;
   }
#endif
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
    bool use_placement_new = true, bool allow_lazy_writes = true )
    :
    cache_base< ods_data_entry_buffer >( max_cache_items,
     items_per_region, regions_in_cache, use_placement_new, allow_lazy_writes ),
    o( o ),
    fname( fname ),
    read_data_handle( 0 ),
    write_data_handle( 0 ),
    write_lock_handle( 0 )
   {
#ifdef __GNUG__
      int rc = posix_memalign( ( void** )&p_data, getpagesize( ), sizeof( ods_data_entry_buffer ) );

      if( rc != 0 || !p_data )
         throw runtime_error( "unexpected failure for posix_memalign" );
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

   bool lock_region( int_t start, int_t len )
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
            throw ods_error( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
      }

#ifdef __GNUG__
      lock.l_len = len;
      lock.l_type = F_WRLCK;
      lock.l_start = start;
      lock.l_whence = SEEK_SET;

      if( fcntl( write_lock_handle, F_SETLK, &lock ) == 0 )
         retval = true;
#endif

#ifdef _WIN32
      if( _lseek( write_lock_handle, start, SEEK_SET ) != start )
         throw ods_error( "unexpected _lseek result at " STRINGIZE( __LINE__ ) );

      if( _locking( write_lock_handle, LK_NBLCK, len ) == 0 )
         retval = true;
#endif

      return retval;
   }

   void unlock_region( int_t start, int_t len )
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
         throw ods_error( "unexpected fcntl at " STRINGIZE( __LINE__ ) " failed..." );
#endif

#ifdef _WIN32
      if( _lseek( write_lock_handle, start, SEEK_SET ) != start )
         throw ods_error( "unexpected _lseek result at " STRINGIZE( __LINE__ ) );

      if( _locking( write_lock_handle, LK_UNLCK, len ) != 0 )
         throw ods_error( "unexpected _locking at " STRINGIZE( __LINE__ ) " failed..." );
#endif
   }

   private:
   mutex data_lock;

   ods& o;
   string fname;
   int read_data_handle;
   int write_data_handle;
   int write_lock_handle;

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
         read_data_handle = _open( fname.c_str( ), O_RDONLY | O_CREAT | O_SYNC | O_DIRECT, ODS_DEFAULT_PERMS );
#else
         read_data_handle = _sopen( fname.c_str( ), O_BINARY | O_RDONLY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
#endif
         if( read_data_handle <= 0 )
            throw ods_error( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
      }

      int_t pos;
      if( ( pos = _lseek( read_data_handle, ( num * sizeof( ods_data_entry_buffer ) ), SEEK_SET ) ) < 0 )
         throw ods_error( "unexpected _lseek at " STRINGIZE( __LINE__ ) " failed..." );

#ifndef __GNUG__
      char* p_data( ( char* )&data.data );
#endif

      int len = _read( read_data_handle, ( void* )p_data, sizeof( ods_data_entry_buffer ) );
      if( len != sizeof( ods_data_entry_buffer ) )
      {
#ifdef __BORLANDC__
         if( eof( read_data_handle ) )
#else
         if( _lseek( read_data_handle, 0, SEEK_CUR ) >= _lseek( read_data_handle, 0, SEEK_END ) )
#endif
            memset( p_data + len, 0, sizeof( ods_data_entry_buffer ) - len );
         else
            throw ods_error( "unexpected read at " STRINGIZE( __LINE__ ) " failed..." );
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
         write_data_handle = _open( fname.c_str( ), O_WRONLY | O_CREAT | O_SYNC | O_DIRECT, ODS_DEFAULT_PERMS );
#else
         write_data_handle = _sopen( fname.c_str( ), O_BINARY | O_WRONLY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
#endif
         if( write_data_handle <= 0 )
            throw ods_error( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
      }

      if( _lseek( write_data_handle, ( num * sizeof( ods_data_entry_buffer ) ), SEEK_SET ) < 0 )
         throw ods_error( "unexpected seek at " STRINGIZE( __LINE__ ) " failed..." );

#ifndef __GNUG__
      char* p_data( ( char* )&data );
#else
      memcpy( p_data, &data, sizeof( ods_data_entry_buffer ) );
#endif

      if( _write( write_data_handle, ( void* )p_data,
       sizeof( ods_data_entry_buffer ) ) != sizeof( ods_data_entry_buffer ) )
         throw ods_error( "unexpected write at " STRINGIZE( __LINE__ ) " failed..." );
   }
};

struct ods_index_entry_buffer
{
   ods_index_entry::data_t item[ c_index_items_per_item ];
};

class ods_index_cache_buffer : public cache_base< ods_index_entry_buffer >
{
   public:
   ods_index_cache_buffer( const string& file_name,
    short lock_offset, unsigned max_cache_items, unsigned items_per_region,
    unsigned regions_in_cache = 1, bool use_placement_new = true, bool allow_lazy_writes = true )
    :
    cache_base< ods_index_entry_buffer >( max_cache_items,
     items_per_region, regions_in_cache, use_placement_new, allow_lazy_writes ),
    file_name( file_name ),
    lock_offset( lock_offset ),
    lock_index_handle( 0 ),
    read_index_handle( 0 ),
    write_index_handle( 0 )
   {
#ifdef __GNUG__
      int rc = posix_memalign( ( void** )&p_data, getpagesize( ), sizeof( ods_index_entry_buffer ) );

      if( rc != 0 || !p_data )
         throw runtime_error( "unexpected failure for posix_memalign" );
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

   int_t get_file_size( )
   {
      guard lock_index( index_lock );

      if( !read_index_handle )
      {
#ifdef __GNUG__
         read_index_handle = _open( file_name.c_str( ), O_RDONLY | O_SYNC | O_DIRECT );
#else
         read_index_handle = _sopen( file_name.c_str( ), O_BINARY | O_RDONLY, SH_DENYNO );
#endif
         if( read_index_handle <= 0 )
            throw ods_error( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
      }

      int_t pos = _lseek( read_index_handle, 0, SEEK_END );

      if( pos < 0 )
         throw ods_error( "unexpected seek at " STRINGIZE( __LINE__ ) " failed..." );

      return pos;
   }

   bool lock_entry( int_t entry_num, bool is_write )
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
            throw ods_error( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
      }

#ifdef __GNUG__
      lock.l_len = sizeof( ods_index_entry::data_t );
      lock.l_type = is_write ? F_WRLCK : F_RDLCK;
      lock.l_start = entry_num;
      lock.l_whence = SEEK_SET;

      return fcntl( lock_index_handle, F_SETLK, &lock ) == 0;
#endif

#ifdef _WIN32
      size_t len = 1;
      int64_t pos = entry_num * sizeof( ods_index_entry::data_t );
      if( !is_write )
         pos += lock_offset;
      else
         len = sizeof( ods_index_entry::data_t );

      if( _lseek( lock_index_handle, pos, SEEK_SET ) != pos )
         throw ods_error( "unexpected _lseek result at " STRINGIZE( __LINE__ ) );

      return _locking( lock_index_handle, LK_NBLCK, len ) == 0;
#endif
   }

   void unlock_entry( int_t entry_num, bool is_write )
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

#ifdef _WIN32
      size_t len = 1;
      int64_t pos = entry_num * sizeof( ods_index_entry::data_t );
      if( !is_write )
         pos += lock_offset;
      else
         len = sizeof( ods_index_entry::data_t );

      if( _lseek( lock_index_handle, pos, SEEK_SET ) != pos )
         throw ods_error( "unexpected _lseek result at " STRINGIZE( __LINE__ ) );

      if( _locking( lock_index_handle, LK_UNLCK, len ) != 0 )
         throw ods_error( "unexpected _locking at " STRINGIZE( __LINE__ ) " failed..." );
#endif
   }

   private:
   mutex index_lock;

   string file_name;
   short lock_offset;
   int lock_index_handle;
   int read_index_handle;
   int write_index_handle;

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
         read_index_handle = _open( file_name.c_str( ), O_RDONLY | O_CREAT | O_SYNC | O_DIRECT, ODS_DEFAULT_PERMS );
#else
         read_index_handle = _sopen( file_name.c_str( ), O_BINARY | O_RDONLY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
#endif
         if( read_index_handle <= 0 )
            throw ods_error( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
      }

      if( _lseek( read_index_handle, ( num * sizeof( ods_index_entry_buffer ) ), SEEK_SET ) < 0 )
         throw ods_error( "unexpected seek at " STRINGIZE( __LINE__ ) " failed..." );

#ifndef __GNUG__
      char* p_data( ( char* )&data );
#endif

      int len = _read( read_index_handle, ( void* )p_data, sizeof( ods_index_entry_buffer ) );
      if( len != sizeof( ods_index_entry_buffer ) )
      {
#ifdef __BORLANDC__
         if( eof( read_index_handle ) )
#else
         if( _lseek( read_index_handle, 0, SEEK_CUR ) >= _lseek( read_index_handle, 0, SEEK_END ) )
#endif
            memset( p_data + len, 0, sizeof( ods_index_entry_buffer ) - len );
         else
            throw ods_error( "unexpected read at " STRINGIZE( __LINE__ ) " failed..." );
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
         write_index_handle = _open( file_name.c_str( ), O_WRONLY | O_CREAT | O_SYNC | O_DIRECT, ODS_DEFAULT_PERMS );
#else
         write_index_handle = _sopen( file_name.c_str( ), O_BINARY | O_WRONLY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
#endif
         if( write_index_handle <= 0 )
            throw ods_error( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
      }

      if( _lseek( write_index_handle, ( num * sizeof( ods_index_entry_buffer ) ), SEEK_SET ) < 0 )
         throw ods_error( "unexpected seek at " STRINGIZE( __LINE__ ) " failed..." );

#ifndef __GNUG__
      char* p_data( ( char* )&data );
#else
      memcpy( p_data, &data, sizeof( ods_index_entry_buffer ) );
#endif

      if( _write( write_index_handle, ( void* )p_data,
       sizeof( ods_index_entry_buffer ) ) != sizeof( ods_index_entry_buffer ) )
         throw ods_error( "unexpected write at " STRINGIZE( __LINE__ ) " failed..." );
   }
};

class ods_trans_op_cache_buffer : public cache_base< trans_op_buffer >
{
   public:
   ods_trans_op_cache_buffer( unsigned max_cache_items, unsigned items_per_region,
    unsigned regions_in_cache = 1, bool use_placement_new = true, bool allow_lazy_writes = true )
    :
    cache_base< trans_op_buffer >( max_cache_items,
     items_per_region, regions_in_cache, use_placement_new, allow_lazy_writes ),
    tran_ops_handle( 0 ),
    has_begun_trans( false )
   {
#ifdef __GNUG__
      int rc = posix_memalign( ( void** )&p_data, getpagesize( ), sizeof( trans_op_buffer ) );
      if( rc != 0 || !p_data )
         throw runtime_error( "unexpected failure for posix_memalign" );
#endif
   }

   ~ods_trans_op_cache_buffer( )
   {
#ifdef __GNUG__
      free( p_data );
#endif
   }

   void new_transaction( int_t tran_id )
   {
#ifdef ODS_DEBUG
      DEBUG_LOG( "in ods_trans_op_cache_buffer::new_transaction( )..." );
#endif
      guard lock_trans( trans_op_lock );

      ostringstream osstr;
      osstr << hex << setw( sizeof( int_t ) * 2 ) << setfill( '0' ) << tran_id;

      file_name = osstr.str( ) + c_ops_file_name_ext;

      has_begun_trans = true;
   }

   void end_transaction( )
   {
#ifdef ODS_DEBUG
      DEBUG_LOG( "in ods_trans_op_cache_buffer::end_transaction( )..." );
#endif
      guard lock_trans( trans_op_lock );

      if( tran_ops_handle )
      {
         if( _close( tran_ops_handle ) != 0 )
            throw ods_error( "unexpected _close at " STRINGIZE( __LINE__ ) " failed..." );
         if( remove( file_name.c_str( ) ) != 0 )
            throw ods_error( "unexpected remove at " STRINGIZE( __LINE__ ) " failed..." );

         tran_ops_handle = 0;
         has_begun_trans = false;
      }

      clear( );
   }

   private:
   int tran_ops_handle;
   bool has_begun_trans;

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
         throw ods_error( "unexpected null file handle at "  STRINGIZE( __LINE__ ) );

      if( _lseek( tran_ops_handle, ( num * sizeof( trans_op_buffer ) ), SEEK_SET ) < 0 )
         throw ods_error( "unexpected seek at " STRINGIZE( __LINE__ ) " failed..." );

#ifndef __GNUG__
      char* p_data( ( char* )&data );
#endif

      if( _read( tran_ops_handle,
       ( void* )p_data, sizeof( trans_op_buffer ) ) != sizeof( trans_op_buffer ) )
         throw ods_error( "unexpected read at " STRINGIZE( __LINE__ ) " failed..." );

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
            throw ods_error( "unexpected not in transaction at " STRINGIZE( __LINE__ ) );

#ifdef __GNUG__
         tran_ops_handle = _open( file_name.c_str( ), O_RDWR | O_CREAT | O_SYNC | O_DIRECT, ODS_DEFAULT_PERMS );
#else
         tran_ops_handle = _sopen( file_name.c_str( ), O_BINARY | O_RDWR | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
#endif

         if( tran_ops_handle <= 0 )
         {
#ifdef ODS_DEBUG
            if( errno == EMFILE )
               DEBUG_LOG( "too many files open..." );
            else
            {
               ostringstream osstr;
               osstr << "errno = " << errno;
               DEBUG_LOG( osstr.str( ) );
            }
#endif
            throw ods_error( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
         }
      }

      if( _lseek( tran_ops_handle, ( num * sizeof( trans_op_buffer ) ), SEEK_SET ) < 0 )
         throw ods_error( "unexpected seek at " STRINGIZE( __LINE__ ) " failed..." );

#ifndef __GNUG__
      char* p_data( ( char* )&data );
#else
      memcpy( p_data, &data, sizeof( trans_op_buffer ) );
#endif

      if( _write( tran_ops_handle, ( void* )p_data,
       sizeof( trans_op_buffer ) ) != sizeof( trans_op_buffer ) )
         throw ods_error( "unexpected write at " STRINGIZE( __LINE__ ) " failed..." );
   }
};

class ods_trans_data_cache_buffer : public cache_base< trans_data_buffer >
{
   public:
   ods_trans_data_cache_buffer( unsigned max_cache_items, unsigned items_per_region,
    unsigned regions_in_cache = 1, bool use_placement_new = true, bool allow_lazy_writes = true )
    :
    cache_base< trans_data_buffer >( max_cache_items,
     items_per_region, regions_in_cache, use_placement_new, allow_lazy_writes ),
    tran_data_handle( 0 ),
    has_begun_trans( false )
   {
#ifdef __GNUG__
      int rc = posix_memalign( ( void** )&p_data, getpagesize( ), sizeof( trans_data_buffer ) );
      if( rc != 0 || !p_data )
         throw runtime_error( "unexpected failure for posix_memalign" );
#endif
   }

   ~ods_trans_data_cache_buffer( )
   {
#ifdef __GNUG__
      free( p_data );
#endif
   }

   void new_transaction( int_t tran_id )
   {
#ifdef ODS_DEBUG
      DEBUG_LOG( "in ods_trans_data_cache_buffer::new_transaction( )..." );
#endif
      guard lock_trans( trans_data_lock );

      ostringstream osstr;
      osstr << hex << setw( sizeof( int_t ) * 2 ) << setfill( '0' ) << tran_id;

      file_name = osstr.str( ) + c_data_file_name_ext;

      has_begun_trans = true;
   }

   void end_transaction( )
   {
#ifdef ODS_DEBUG
      DEBUG_LOG( "in ods_trans_data_cache_buffer::end_transaction( )..." );
#endif
      guard lock_trans( trans_data_lock );

      if( tran_data_handle )
      {
         if( _close( tran_data_handle ) != 0 )
            throw ods_error( "unexpected _close at " STRINGIZE( __LINE__ ) " failed..." );
         if( remove( file_name.c_str( ) ) != 0 )
            throw ods_error( "unexpected remove at " STRINGIZE( __LINE__ ) " failed..." );

         tran_data_handle = 0;
         has_begun_trans = false;
      }

      clear( );
   }

   private:
   int tran_data_handle;
   bool has_begun_trans;

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
         throw ods_error( "unexpected null file handle at " STRINGIZE( __LINE__ ) );

      if( _lseek( tran_data_handle, ( num * sizeof( trans_data_buffer ) ), SEEK_SET ) < 0 )
         throw ods_error( "unexpected seek at " STRINGIZE( __LINE__ ) " failed..." );

#ifndef __GNUG__
      char* p_data( ( char* )&data );
#endif

      if( _read( tran_data_handle,
       ( void* )p_data, sizeof( trans_data_buffer ) ) != sizeof( trans_data_buffer ) )
         throw ods_error( "unexpected read at " STRINGIZE( __LINE__ ) " failed..." );

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
            throw ods_error( "unexpected not in transaction at " STRINGIZE( __LINE__ ) );

#ifdef __GNUG__
         tran_data_handle = _open( file_name.c_str( ), O_RDWR | O_CREAT | O_SYNC | O_DIRECT, ODS_DEFAULT_PERMS );
#else
         tran_data_handle = _sopen( file_name.c_str( ), O_BINARY | O_RDWR | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
#endif

         if( tran_data_handle <= 0 )
         {
#ifdef ODS_DEBUG
            if( errno == EMFILE )
               DEBUG_LOG( "too many files open..." );
            else
            {
               ostringstream osstr;
               osstr << "errno = " << errno;
               DEBUG_LOG( osstr.str( ) );
            }
#endif
            throw ods_error( "unexpected bad handle at " STRINGIZE( __LINE__ ) );
         }
      }

      if( _lseek( tran_data_handle, ( num * sizeof( trans_data_buffer ) ), SEEK_SET ) < 0 )
         throw ods_error( "unexpected seek at " STRINGIZE( __LINE__ ) " failed..." );

#ifndef __GNUG__
      char* p_data( ( char* )&data );
#else
      memcpy( p_data, &data, sizeof( trans_data_buffer ) );
#endif

      if( _write( tran_data_handle, ( void* )p_data,
       sizeof( trans_data_buffer ) ) != sizeof( trans_data_buffer ) )
         throw ods_error( "unexpected write at " STRINGIZE( __LINE__ ) " failed..." );
   }
};

struct transaction_buffer
{
   transaction_buffer( ) : tran_id( 0 ) { }

   int_t tran_id;
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
    is_exclusive( false ),
    force_padding( false ),
    trans_level( 0 ),
    read_from_trans( false ),
    total_trans_size( 0 ),
    total_trans_op_count( 0 )
   {
   }

   string data_file_name;
   string index_file_name;
   string header_file_name;
   string tranlog_file_name;

#ifdef __GNUG__
   ref_count_ptr< flock > rp_lock;
#endif

   bool is_new;
   bool is_corrupt;
   bool is_exclusive;

   bool force_padding;

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
   ref_count_ptr< ods_data_cache_buffer > rp_ods_data_cache_buffer;

   ods_index_entry_buffer index_item_buffer;
   ref_count_ptr< ods_index_cache_buffer > rp_ods_index_cache_buffer;

   int_t trans_level;
   bool read_from_trans;

   int_t total_trans_size;
   int_t total_trans_op_count;

   transaction_buffer* p_trans_buffer;
   ods_trans_op_cache_buffer* p_ods_trans_op_cache_buffer;
   ods_trans_data_cache_buffer* p_ods_trans_data_cache_buffer;

   ref_count_ptr< int > rp_open_store_ref_count;

   ref_count_ptr< int_t > rp_session_create_total;
   ref_count_ptr< int_t > rp_session_revive_total;
   ref_count_ptr< int_t > rp_session_review_total;
   ref_count_ptr< int_t > rp_session_update_total;
   ref_count_ptr< int_t > rp_session_delete_total;

   ref_count_ptr< mutex > rp_impl_lock;
   ref_count_ptr< mutex > rp_file_section;

   void read_header_file_info( );
   void write_header_file_info( bool for_close = false );

   bool found_instance_currently_reading( int_t num );
   bool found_instance_currently_writing( int_t num );

   ref_count_ptr< vector< ods* > > rp_instances;
};

void ods::impl::read_header_file_info( )
{
   if( _lseek( *rp_header_file, 0, SEEK_SET ) != 0 )
      throw ods_error( "unexpected seek at " STRINGIZE( __LINE__ ) " failed..." );

   if( _read( *rp_header_file,
    ( void* )rp_header_info.get( ), sizeof( header_info ) ) != sizeof( header_info ) )
      throw ods_error( "unexpected read at " STRINGIZE( __LINE__ ) " failed..." );
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
      throw ods_error( "unexpected seek at " STRINGIZE( __LINE__ ) " failed..." );

   if( _write( *rp_header_file,
    ( void* )rp_header_info.get( ), sizeof( header_info ) ) != sizeof( header_info ) )
      throw ods_error( "unexpected write at " STRINGIZE( __LINE__ ) " failed..." );
}

bool ods::impl::found_instance_currently_reading( int_t num )
{
   for( vector< ods* >::iterator iter = rp_instances->begin( ); iter != rp_instances->end( ); ++iter )
      if( *iter && ( *iter )->current_read_object_num == num )
         return true;

   return false;
}

bool ods::impl::found_instance_currently_writing( int_t num )
{
   for( vector< ods* >::iterator iter = rp_instances->begin( ); iter != rp_instances->end( ); ++iter )
      if( *iter && ( *iter )->current_write_object_num == num )
         return true;

   return false;
}

struct ods_index_entry_pos
{
   int_t id;
   int_t pos;
   int_t size;

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

void ods_index_entry::dump_entry( ostream& os, int_t num )
{
   os << "num: " << hex
    << setw( sizeof( int_t ) * 2 ) << setfill( '0' ) << num
    << "          pos: " << hex << setw( sizeof( int_t ) * 2 ) << setfill( '0' ) << data.pos
    << "          len: " << hex << setw( sizeof( int_t ) * 2 ) << setfill( '0' ) << data.size
    << "\ntxi: " << hex << setw( sizeof( int_t ) * 2 ) << setfill( '0' ) << data.tran_id
    << "          txo: " << hex << setw( sizeof( int_t ) * 2 ) << setfill( '0' ) << data.tran_op
    << "               flags: lk=" << hex << setw( 1 ) << setfill( '0' ) << lock_flag
    << " tx=" << hex << setw( 1 ) << setfill( '0' ) << trans_flag << dec << '\n';
}

size_t ods::header_info_size( )
{
   return sizeof( header_info );
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
 is_in_read( false ),
 is_in_write( false ),
 permit_copy( false ),
 data_read_buffer_num( -1 ),
 data_read_buffer_offs( 0 ),
 data_write_buffer_num( -1 ),
 data_write_buffer_offs( 0 ),
 index_item_buffer_num( -1 ),
 has_written_index_item( false ),
 trans_read_ops_buffer_num( -1 ),
 trans_read_data_buffer_num( -1 ),
 trans_read_data_buffer_offs( 0 ),
 had_interim_trans_op_write( false ),
 had_interim_trans_data_write( false ),
 has_written_trans_op( false ),
 trans_write_ops_buffer_num( -1 ),
 trans_write_data_buffer_num( -1 ),
 trans_write_data_buffer_offs( 0 ),
 current_read_object_num( -1 ),
 current_write_object_num( -1 )
{
   guard lock_copy_ctor( g_ods_lock );

   DEBUG_LOG( "ods::ods( const ods& )..." );

   if( !o.permit_copy )
      throw ods_error( "copy construction was disallowed..." );

   auto_ptr< impl > ap_impl( new impl( *o.p_impl ) );
   p_impl = ap_impl.get( );

   guard tmp_lock( *p_impl->rp_impl_lock );

   if( _access( p_impl->data_file_name.c_str( ), 0 ) != 0 )
      throw ods_error( "could not find data file..." );
   if( _access( p_impl->index_file_name.c_str( ), 0 ) != 0 )
      throw ods_error( "could not find index file..." );

   if( *p_impl->rp_header_file <= 0 )
      throw ods_error( "unexpected header file handle is invalid..." );

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

ods::ods( const char* name, open_mode o_mode, share_mode s_mode )
 :
 okay( false ),
 is_in_read( false ),
 is_in_write( false ),
 permit_copy( false ),
 data_read_buffer_num( -1 ),
 data_read_buffer_offs( 0 ),
 data_write_buffer_num( -1 ),
 data_write_buffer_offs( 0 ),
 index_item_buffer_num( -1 ),
 has_written_index_item( false ),
 trans_read_ops_buffer_num( -1 ),
 trans_read_data_buffer_num( -1 ),
 trans_read_data_buffer_offs( 0 ),
 had_interim_trans_op_write( false ),
 had_interim_trans_data_write( false ),
 has_written_trans_op( false ),
 trans_write_ops_buffer_num( -1 ),
 trans_write_data_buffer_num( -1 ),
 trans_write_data_buffer_offs( 0 ),
 current_read_object_num( -1 ),
 current_write_object_num( -1 )
{
   guard lock_ctor( g_ods_lock );

   DEBUG_LOG( "ods::ods( )..." );

   auto_ptr< impl > ap_impl( new impl );
   p_impl = ap_impl.get( );

   p_impl->data_file_name = string( name ) + c_data_file_name_ext;
   p_impl->index_file_name = string( name ) + c_index_file_name_ext;
   p_impl->header_file_name = string( name ) + c_header_file_name_ext;
   p_impl->tranlog_file_name = string( name ) + c_tranlog_file_name_ext;

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
   if( s_mode == e_share_mode_exclusive )
      DEBUG_LOG( "********** capturing exclusive file use **********" );
#endif

   p_impl->rp_header_file
    = new header_file( p_impl->header_file_name.c_str( ), s_mode == e_share_mode_exclusive );

   if( *p_impl->rp_header_file <= 0 )
      throw ods_error( "unable to open header file (locked?)..." );

   if( p_impl->rp_header_file->get_lock_offset( ) < 0 )
      throw ods_error( "unable to acquire valid lock offset (max. concurrent processes active?)..." );

   p_impl->is_exclusive = ( s_mode == e_share_mode_exclusive );

   auto_ptr< ods::header_file_lock > ap_header_file_lock( new ods::header_file_lock( *this ) );

   if( _access( p_impl->index_file_name.c_str( ), 0 ) != 0 )
      p_impl->is_new = true;

   switch( o_mode )
   {
      case e_open_mode_exist:
      if( p_impl->is_new )
      {
         ap_header_file_lock.reset( );
         p_impl->rp_header_file = 0;
         remove( p_impl->header_file_name.c_str( ) );
         throw ods_error( "index file does not exist" );
      }
      break;

      case e_open_mode_not_exist:
      if( !p_impl->is_new )
         throw ods_error( "index file already exists" );
      break;

      case e_open_mode_create_if_not_exist:
      break;

      default:
      throw ods_error( "unexpected default case at " STRINGIZE( __LINE__ ) );
   }

   p_impl->rp_impl_lock = new mutex;
   p_impl->rp_file_section = new mutex;

   p_impl->rp_header_info = new header_info;
   if( !p_impl->is_new )
   {
      p_impl->read_header_file_info( );

      // NOTE: File corruption can be determined if we have an exclusive file
      // lock but find that one or more writers appear to be currently active.
      if( s_mode == e_share_mode_exclusive && p_impl->rp_header_info->num_writers )
         p_impl->is_corrupt = true;
   }
   else
   {
      p_impl->rp_header_info->version_id = c_version_id;

      // NOTE: The newly created file will be marked as corrupt unless "has_changed" is set true.
      temp_set_value< bool > temp_has_changed( *p_impl->rp_has_changed, true );
      p_impl->write_header_file_info( );
   }

   p_impl->rp_instances = new vector< ods* >( );
   p_impl->rp_instances->push_back( this );

   if( p_impl->is_new )
      DEBUG_LOG( "[opened new file]" );
   else
   {
      DEBUG_LOG( "[opened existing file]" );
      if( _access( p_impl->data_file_name.c_str( ), 0 ) != 0 )
         throw ods_error( "could not find data file..." );
   }

   p_impl->rp_open_store_ref_count = new int( 0 );

   p_impl->rp_session_create_total = new int_t( 0 );
   p_impl->rp_session_revive_total = new int_t( 0 );
   p_impl->rp_session_review_total = new int_t( 0 );
   p_impl->rp_session_update_total = new int_t( 0 );
   p_impl->rp_session_delete_total = new int_t( 0 );

   p_impl->rp_ods_data_cache_buffer =
    new ods_data_cache_buffer( *this, p_impl->data_file_name,
     c_data_max_cache_items, c_data_items_per_region, c_data_num_cache_regions );

   p_impl->rp_ods_index_cache_buffer = new ods_index_cache_buffer( p_impl->index_file_name,
    p_impl->rp_header_file->get_lock_offset( ), c_index_max_cache_items, c_index_items_per_region, c_index_num_cache_regions );

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
   permit_copy = true;
}

ods::~ods( )
{
   guard lock_dtor( g_ods_lock );

   permit_copy = false;

   DEBUG_LOG( "ods::~ods( )..." );
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
         DEBUG_LOG( "********** releasing exclusive file use **********" );
#endif
   }

   delete p_impl;
}

bool ods::is_new( )
{
   return p_impl->is_new;
}

bool ods::is_corrupt( )
{
   return p_impl->is_corrupt;
}

bool ods::is_bulk_locked( ) const
{
   return ( *p_impl->rp_bulk_mode != impl::e_bulk_mode_none );
}

bool ods::is_bulk_dump_locked( ) const
{
   return ( *p_impl->rp_bulk_mode == impl::e_bulk_mode_dump );
}

bool ods::is_bulk_read_locked( ) const
{
   return ( *p_impl->rp_bulk_mode == impl::e_bulk_mode_read );
}

bool ods::is_bulk_write_locked( ) const
{
   return ( *p_impl->rp_bulk_mode == impl::e_bulk_mode_write );
}

int_t ods::get_total_entries( )
{
   return p_impl->rp_header_info->total_entries;
}

int_t ods::get_session_review_total( )
{
   return *p_impl->rp_session_review_total;
}

int_t ods::get_session_create_total( )
{
   return *p_impl->rp_session_create_total;
}

int_t ods::get_session_revive_total( )
{
   return *p_impl->rp_session_revive_total;
}

int_t ods::get_session_update_total( )
{
   return *p_impl->rp_session_update_total;
}

int_t ods::get_session_delete_total( )
{
   return *p_impl->rp_session_delete_total;
}

int_t ods::get_transaction_id( ) const
{
   return p_impl->p_trans_buffer->tran_id;
}

int_t ods::get_transaction_level( ) const
{
   return p_impl->trans_level;
}

int_t ods::get_next_transaction_id( ) const
{
   return p_impl->rp_header_info->transaction_id;
}

void ods::bulk_base::pause( )
{
   guard lock_write( o.write_lock );
   guard lock_read( o.read_lock );
   guard lock_impl( *o.p_impl->rp_impl_lock );

   o.bulk_operation_pause( );
}

ods::bulk_dump::bulk_dump( ods& o )
 : bulk_base( o )
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
      throw ods_error( "thread cannot obtain bulk dump lock (max. attempts exceeded)..." );
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

ods::bulk_read::bulk_read( ods& o )
 : bulk_base( o )
{
   int i;
   for( i = 0; i < c_bulk_read_max_attempts; i++ )
   {
      if( i > 0 )
         msleep( c_bulk_read_attempt_sleep_time );

      guard lock_write( o.write_lock );
      guard lock_read( o.read_lock );
      guard lock_impl( *o.p_impl->rp_impl_lock );

      if( *o.p_impl->rp_bulk_mode != impl::e_bulk_mode_none
       && *o.p_impl->rp_bulk_mode != impl::e_bulk_mode_read )
      {
         if( *o.p_impl->rp_bulk_write_thread_id != current_thread_id( ) )
            continue;
         else
            throw runtime_error( "invalid attempt to obtain bulk read lock whilst already bulk write locked" );
      }

      if( *o.p_impl->rp_bulk_mode == impl::e_bulk_mode_read
       && *o.p_impl->rp_bulk_read_thread_id != current_thread_id( ) )
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
      throw ods_error( "thread cannot obtain bulk read lock (max. attempts exceeded)..." );
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

ods::bulk_write::bulk_write( ods& o )
 : bulk_base( o )
{
   int i;
   for( i = 0; i < c_bulk_write_max_attempts; i++ )
   {
      if( i > 0 )
         msleep( c_bulk_write_attempt_sleep_time );

      guard lock_write( o.write_lock );
      guard lock_read( o.read_lock );
      guard lock_impl( *o.p_impl->rp_impl_lock );

      if( *o.p_impl->rp_bulk_mode != impl::e_bulk_mode_none
       && *o.p_impl->rp_bulk_mode != impl::e_bulk_mode_write )
      {
         if( *o.p_impl->rp_bulk_read_thread_id != current_thread_id( ) )
            continue;
         else
            throw runtime_error( "invalid attempt to obtain bulk write lock whilst already bulk read locked" );
      }

      if( *o.p_impl->rp_bulk_mode == impl::e_bulk_mode_write
       && *o.p_impl->rp_bulk_write_thread_id != current_thread_id( ) )
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
      throw ods_error( "thread cannot obtain bulk write lock (max. attempts exceeded)..." );
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
         throw ods_error( "attempt to commit an already processed transaction" );

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
         throw ods_error( "attempt to rollback an already processed transaction" );

      o.transaction_rollback( );
      can_commit = false;
   }
}

void ods::destroy( const oid& id )
{
   guard lock_write( write_lock );
   guard lock_read( read_lock );

   if( !okay )
      throw ods_error( "ods instance in bad state..." );

   DEBUG_LOG( "ods::destroy( )" );
   ods_index_entry index_entry;
   if( id.get_num( ) < 0 )
      return;

   if( *p_impl->rp_bulk_level && *p_impl->rp_bulk_mode != impl::e_bulk_mode_write )
      throw ods_error( "cannot destroy when bulk locked for dumping or reading" );

   bool deleted = false;
   int attempts = c_delete_max_attempts;

   while( attempts-- )
   {
      { // start of file lock section...
         guard tmp_lock( *p_impl->rp_impl_lock );
         ods::header_file_lock header_file_lock( *this );

         auto_ptr< ods::file_scope > ap_file_scope;
         if( !*p_impl->rp_bulk_level )
            ap_file_scope.reset( new ods::file_scope( *this ) );

         if( id.get_num( ) < p_impl->rp_header_info->total_entries )
         {
            read_index_entry( index_entry, id.get_num( ) );

            if( index_entry.lock_flag == ods_index_entry::e_lock_none )
            {
               if( index_entry.trans_flag == ods_index_entry::e_trans_none
                || ( p_impl->trans_level
                && index_entry.data.tran_id == p_impl->p_trans_buffer->tran_id
                && index_entry.trans_flag != ods_index_entry::e_trans_delete ) )
               {
                  if( index_entry.trans_flag == ods_index_entry::e_trans_none )
                     index_entry.data.tran_op = 0;

                  if( p_impl->trans_level )
                  {
                     transaction_op op;

                     op.type = transaction_op::e_op_type_destroy;

                     op.data.id = id;
                     op.data.old_tran_op = index_entry.data.tran_op;

                     write_transaction_op( op );

                     index_entry.data.tran_op = 0;
                     index_entry.data.tran_id = p_impl->p_trans_buffer->tran_id;

                     index_entry.trans_flag = ods_index_entry::e_trans_delete;
                  }
                  else
                  {
                     if( index_entry.data.pos + index_entry.data.size
                      == p_impl->rp_header_info->total_size_of_data )
                        p_impl->rp_header_info->total_size_of_data -= index_entry.data.size;

                     if( !p_impl->rp_header_info->index_free_list )
                        index_entry.data.pos = 0;
                     else
                        index_entry.data.pos = p_impl->rp_header_info->index_free_list;

                     index_entry.data.size = 0;

                     index_entry.data.tran_op = 0;
                     index_entry.data.tran_id = p_impl->rp_header_info->transaction_id;

                     index_entry.trans_flag = ods_index_entry::e_trans_free_list;

                     p_impl->rp_header_info->index_free_list = id.get_num( ) + 1;
                  }

                  write_index_entry( index_entry, id.get_num( ) );

                  if( !p_impl->trans_level )
                     ++p_impl->rp_header_info->transaction_id;
                  else
                  {
                     ++p_impl->p_trans_buffer->levels.top( ).op_count;
                     ++p_impl->total_trans_op_count;
                  }

                  deleted = true;
                  ++p_impl->rp_header_info->index_transform_id;

                  ++( *p_impl->rp_session_delete_total );
               }
            }
         }

         if( deleted )
            break;
      } // end of file lock section...

      msleep( c_delete_attempt_sleep_time );
   }

   if( !deleted )
      throw ods_error( "cannot delete (max. attempts exceeded)..." );

   if( p_impl->trans_level && trans_write_ops_buffer_num != -1 && has_written_trans_op )
   {
      p_impl->p_ods_trans_op_cache_buffer->put( trans_write_ops_buffer, trans_write_ops_buffer_num );

      has_written_trans_op = false;
      had_interim_trans_op_write = true;
   }
}

int_t ods::get_size( const oid& id )
{
   guard lock_read( read_lock );

   if( !okay )
      throw ods_error( "ods instance in bad state..." );

   DEBUG_LOG( "ods::get_size( )" );
   ods_index_entry index_entry;
   if( id.get_num( ) < 0 )
      return 0;

   bool found = false;
   int attempts = c_review_max_attempts;

   while( attempts-- )
   {
      { // start of file lock section...
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
      } // end of file lock section...

      msleep( c_review_attempt_sleep_time );
   }

   if( !found )
      throw ods_error( "cannot get_size (max. attempts exceeded)..." );

   return index_entry.data.size;
}

void ods::move_free_data_to_end( )
{
   guard lock_write( write_lock );
   guard lock_read( read_lock );
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      throw ods_error( "ods instance in bad state..." );

   if( p_impl->trans_level )
      throw ods_error( "cannot move free data to end whilst in a transaction" );

   if( !p_impl->rp_header_file->is_locked_for_exclusive( ) )
      throw ods_error( "cannot move free data to end unless locked for exclusive use" );

   if( *p_impl->rp_bulk_level && *p_impl->rp_bulk_mode != impl::e_bulk_mode_write )
      throw ods_error( "cannot move free data to end when bulk locked for dumping or reading" );

   auto_ptr< ods::bulk_write > ap_bulk_write;
   if( !*p_impl->rp_bulk_level )
      ap_bulk_write.reset( new ods::bulk_write( *this ) );

   ods_index_entry_pos entry;
   set< ods_index_entry_pos > entries;

   ods_index_entry index_entry;

   for( int_t i = 0; i < p_impl->rp_header_info->total_entries; i++ )
   {
      read_index_entry( index_entry, i );
      if( index_entry.lock_flag != ods_index_entry::e_lock_none )
         throw ods_error( "cannot move free data to end when one or more entries have been locked" );

      if( index_entry.trans_flag != ods_index_entry::e_trans_none
       && index_entry.trans_flag != ods_index_entry::e_trans_free_list )
         throw ods_error( "cannot move free data to end whilst one or more dead transactions exist" );

      if( index_entry.trans_flag != ods_index_entry::e_trans_free_list )
      {
         entry.id = i;
         entry.pos = index_entry.data.pos;
         entry.size = index_entry.data.size;
         entries.insert( entry );
      }
   }

   set_read_data_pos( 0 );

   int_t new_pos = 0;
   int_t read_pos = 0;
   int_t actual_size = 0;
   set< ods_index_entry_pos >::const_iterator ci, end;

   for( ci = entries.begin( ), end = entries.end( ); ci != end; ++ci )
   {
      int_t next_id = ci->id;
      int_t next_pos = ci->pos;
      int_t next_size = ci->size;

      if( next_pos < read_pos )
         throw ods_error( "unexpected next_pos < read_pos at " STRINGIZE( __LINE__ ) );

      adjust_read_data_pos( next_pos - read_pos );

      if( next_pos != new_pos )
      {
         int_t chunk = c_buffer_chunk_size;
         char buffer[ c_buffer_chunk_size ];

         set_write_data_pos( new_pos );
         for( int_t j = 0; j < next_size; j += chunk )
         {
            if( j + chunk > next_size )
               chunk = next_size - j;

            read_data_bytes( buffer, chunk );
            write_data_bytes( buffer, chunk );
         }

         read_index_entry( index_entry, next_id );

         index_entry.data.pos = new_pos;
         write_index_entry( index_entry, next_id );

         read_pos = next_pos + next_size;
      }
      else
         read_pos = next_pos;

      new_pos += next_size;
      actual_size += next_size;
   }

   ++p_impl->rp_header_info->data_transform_id;
   ++p_impl->rp_header_info->index_transform_id;

   p_impl->rp_header_info->total_size_of_data = actual_size;

   if( has_written_index_item )
   {
      has_written_index_item = false;
      p_impl->rp_ods_index_cache_buffer->put( p_impl->index_item_buffer, index_item_buffer_num );
   }

   if( !p_impl->trans_level && data_write_buffer_num != -1 && data_write_buffer_offs )
   {
      p_impl->rp_ods_data_cache_buffer->put( p_impl->data_write_buffer, data_write_buffer_num );

      p_impl->rp_ods_data_cache_buffer->unlock_region(
       data_write_buffer_num * c_data_bytes_per_item, c_data_bytes_per_item );

      data_write_buffer_num = -1;
      data_write_buffer_offs = 0;
   }
}

void ods::rollback_dead_transactions( )
{
   guard lock_write( write_lock );
   guard lock_read( read_lock );
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      throw ods_error( "ods instance in bad state..." );

   if( p_impl->trans_level )
      throw ods_error( "cannot rollback dead transactions whilst in a transaction" );

   if( !p_impl->rp_header_file->is_locked_for_exclusive( ) )
      throw ods_error( "cannot rollback dead transactions unless locked for exclusive use" );

   if( *p_impl->rp_bulk_level && *p_impl->rp_bulk_mode != impl::e_bulk_mode_write )
      throw ods_error( "cannot rollback dead transactions when bulk locked for dumping or reading" );

   auto_ptr< ods::bulk_write > ap_bulk_write;
   if( !*p_impl->rp_bulk_level )
      ap_bulk_write.reset( new ods::bulk_write( *this ) );

   ods_index_entry index_entry;

   for( int_t i = 0; i < p_impl->rp_header_info->total_entries; i++ )
   {
      read_index_entry( index_entry, i );

      if( index_entry.lock_flag == ods_index_entry::e_lock_none
       && index_entry.trans_flag != ods_index_entry::e_trans_none
       && index_entry.trans_flag != ods_index_entry::e_trans_free_list )
      {
         if( index_entry.data.size == 0 )
         {
            if( !p_impl->rp_header_info->index_free_list )
               index_entry.data.pos = 0;
            else
               index_entry.data.pos = p_impl->rp_header_info->index_free_list;

            index_entry.data.size = 0;

            index_entry.data.tran_op = 0;
            index_entry.trans_flag = ods_index_entry::e_trans_free_list;

            p_impl->rp_header_info->index_free_list = i + 1;
            ++( *p_impl->rp_session_delete_total );
         }
         else
         {
            ++( *p_impl->rp_session_update_total );

            index_entry.data.tran_op = 0;
            index_entry.trans_flag = ods_index_entry::e_trans_none;
         }

         write_index_entry( index_entry, i );
      }
   }

   if( has_written_index_item )
   {
      has_written_index_item = false;
      p_impl->rp_ods_index_cache_buffer->put( p_impl->index_item_buffer, index_item_buffer_num );
   }

   // NOTE: The file will be marked as corrupt unless "has_changed" is set true.
   temp_set_value< bool > temp_has_changed( *p_impl->rp_has_changed, true );
   p_impl->write_header_file_info( );
}

void ods::dump_file_info( ostream& os )
{
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      throw ods_error( "ods instance in bad state..." );

   string corrupt;
   if( *p_impl->rp_bulk_level && p_impl->rp_header_info->num_writers )
      corrupt = " *** corrupted file detected ***";

   os << "Version Id = " << ( int_t )p_impl->rp_header_info->version_id
    << "\nNum Writers = " << ( int_t )p_impl->rp_header_info->num_writers << corrupt
    << "\nTotal Entries = " << ( int_t )p_impl->rp_header_info->total_entries
    << "\nTransaction Id = " << ( int_t )p_impl->rp_header_info->transaction_id
    << "\nIndex Free List = " << ( int_t )p_impl->rp_header_info->index_free_list
    << "\nTotal Size of Data = " << ( int_t )p_impl->rp_header_info->total_size_of_data
    << "\nData Transformation Id = " << ( int_t )p_impl->rp_header_info->data_transform_id
    << "\nIndex Transformation Id = " << ( int_t )p_impl->rp_header_info->index_transform_id << endl;

   int_t found = p_impl->rp_ods_index_cache_buffer->get_file_size( );
   int_t expected = ods_index_entry::get_size_of( ) * p_impl->rp_header_info->total_entries;

   if( p_impl->rp_header_info->total_entries % c_index_items_per_item )
      expected += ods_index_entry::get_size_of( ) *
       ( c_index_items_per_item - ( p_impl->rp_header_info->total_entries % c_index_items_per_item ) );

   if( found != expected )
   {
      os << "Unexpected index file size: (expected = "
       << expected << ", found = " << found << ")" << endl;
   }
}

void ods::dump_free_list( ostream& os )
{
   guard lock_impl( *p_impl->rp_impl_lock );

   ods_index_entry index_entry;

   if( !p_impl->rp_header_info->index_free_list )
      os << "No freelist entries." << endl;
   else
   {
      int_t count = 0;

      int_t last = 0;
      int_t next = p_impl->rp_header_info->index_free_list;
      os << "First freelist entry = " << dec << ( p_impl->rp_header_info->index_free_list - 1 ) << '\n';

      os << "Iterating over freelist...";
      while( next )
      {
         ++count;
         read_index_entry( index_entry, next - 1 );

         last = next;
         next = index_entry.data.pos;
      }
      os << "(OK)";
      os << "\nLast freelist entry = " << dec << ( last - 1 );
      os << "\nTotal freelist entries = " << dec << count << endl;
   }
}

void ods::dump_index_entry( ostream& os, int_t num )
{
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      throw ods_error( "ods instance in bad state..." );

   ods_index_entry index_entry;

   if( num >= p_impl->rp_header_info->total_entries )
      os << "error: max. entry num = " << dec << ( p_impl->rp_header_info->total_entries - 1 ) << endl;
   else
   {
      read_index_entry( index_entry, num );
      index_entry.dump_entry( os, num );
   }
}

void ods::dump_instance_data( ostream& os, int_t num, bool only_pos_and_size )
{
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      throw ods_error( "ods instance in bad state..." );

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
             << setw( sizeof( int_t ) * 2 ) << setfill( '0' ) << ( index_entry.data.pos - 1 ) << dec;
         else
            os << " link: <at end>";
         os << "\n";
      }
      else
      {
         if( !only_pos_and_size )
         {
            int_t chunk = 16;
            unsigned char buffer[ 16 ];

            set_read_data_pos( index_entry.data.pos );
            for( int_t i = 0; i < index_entry.data.size; i += chunk )
            {
               if( i + chunk > index_entry.data.size )
                  chunk = index_entry.data.size - i;
               read_data_bytes( ( char* )buffer, chunk );

               os << hex << setw( sizeof( int_t ) * 2 )
                << setfill( '0' ) << ( index_entry.data.pos + i ) << "  ";

               for( int j = 0; j < chunk; j++ )
                  os << hex << setw( 2 ) << setfill( '0' ) << ( unsigned )buffer[ j ] << " ";

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
               os << dec << "\n";
            }
         }
      }
   }
}

void ods::open_store( )
{
   DEBUG_LOG( "ods::open_store( )" );

   if( !okay )
      throw ods_error( "ods instance in bad state..." );

   if( ++( *p_impl->rp_open_store_ref_count ) > 1 )
      return;

   int_t last_data_transformation = p_impl->rp_header_info->data_transform_id;
   int_t last_index_transformation = p_impl->rp_header_info->index_transform_id;

   p_impl->read_header_file_info( );

#ifdef ODS_DEBUG
   ostringstream osstr;
   osstr << "(header info read from store)"
    << "\nversion_id = " << ( int_t )p_impl->rp_header_info->version_id
    << ", num_writers = " << ( int_t )p_impl->rp_header_info->num_writers
    << "\ntotal_entries = " << ( int_t )p_impl->rp_header_info->total_entries
    << ", index_free_list = " << ( int_t )p_impl->rp_header_info->index_free_list
    << "\ntransaction_id = " << ( int_t )p_impl->rp_header_info->transaction_id
    << ", total_size_of_data = " << ( int_t )p_impl->rp_header_info->total_size_of_data
    << ", data_transform_id = " << ( int_t )p_impl->rp_header_info->data_transform_id
    << ", index_transform_id = " << ( int_t )p_impl->rp_header_info->index_transform_id;
   DEBUG_LOG( osstr.str( ) );
#endif

   if( ( p_impl->rp_header_info->version_id & ~c_int_type_hi_bit ) != c_version_id )
      throw ods_error( "ods incompatible version info found..." );

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
   DEBUG_LOG( "ods::close_store( )" );

   if( !okay )
      throw ods_error( "ods instance in bad state..." );

   if( --( *p_impl->rp_open_store_ref_count ) != 0 )
      return;

   if( data_write_buffer_num != -1 && data_write_buffer_offs )
   {
      p_impl->rp_ods_data_cache_buffer->put( p_impl->data_write_buffer, data_write_buffer_num );

      p_impl->rp_ods_data_cache_buffer->unlock_region(
       data_write_buffer_num * c_data_bytes_per_item, c_data_bytes_per_item );
   }

   data_write_buffer_num = -1;
   data_write_buffer_offs = 0;

   p_impl->rp_ods_data_cache_buffer->flush( );

   if( has_written_index_item )
   {
      has_written_index_item = false;
      p_impl->rp_ods_index_cache_buffer->put( p_impl->index_item_buffer, index_item_buffer_num );
   }

   p_impl->rp_ods_index_cache_buffer->flush( );

   if( *p_impl->rp_has_changed )
   {
      p_impl->write_header_file_info( true );
      *p_impl->rp_has_changed = false;
   }
}

void ods::bulk_operation_open( )
{
   guard header_file_lock( *p_impl->rp_file_section );

   if( !okay )
      throw ods_error( "ods instance in bad state..." );

   DEBUG_LOG( "ods::bulk_operation_open( )" );

   lock_header_file( );
   try
   {
      open_store( );

      if( p_impl->rp_header_info->num_writers && *p_impl->rp_bulk_mode != impl::e_bulk_mode_dump )
         throw ods_error( "ods file corruption detected" );
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
      throw ods_error( "ods instance in bad state..." );

   DEBUG_LOG( "ods::bulk_operation_close( )" );

   close_store( );
   unlock_header_file( );
}

void ods::bulk_operation_start( )
{
   if( !okay )
      throw ods_error( "ods instance in bad state..." );

   DEBUG_LOG( "ods::bulk_operation_start( )" );
   if( !*p_impl->rp_bulk_level )
      bulk_operation_open( );

   // NOTE: Increment must be done afterwards otherwise header file locking will get confused.
   ++*p_impl->rp_bulk_level;
}

void ods::bulk_operation_pause( )
{
   if( !okay )
      throw ods_error( "ods instance in bad state..." );

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
      throw ods_error( "ods instance in bad state..." );

   DEBUG_LOG( "ods::bulk_operation_finish( )" );
   if( !--( *p_impl->rp_bulk_level ) )
      bulk_operation_close( );
}

void ods::transaction_start( )
{
   guard lock_write( write_lock );
   guard lock_read( read_lock );
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      throw ods_error( "ods instance in bad state..." );

   if( !p_impl->trans_level )
   {
      { // start of file lock section...
         ods::header_file_lock header_file_lock( *this );

         auto_ptr< ods::file_scope > ap_file_scope;
         if( !*p_impl->rp_bulk_level )
            ap_file_scope.reset( new ods::file_scope( *this ) );

         p_impl->p_trans_buffer->tran_id = p_impl->rp_header_info->transaction_id++;

         p_impl->p_ods_trans_op_cache_buffer->new_transaction( p_impl->p_trans_buffer->tran_id );
         p_impl->p_ods_trans_data_cache_buffer->new_transaction( p_impl->p_trans_buffer->tran_id );

         p_impl->total_trans_size = 0;
         p_impl->total_trans_op_count = 0;

         p_impl->write_header_file_info( );
      } // end of file lock section...
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
   osstr << "ods::transaction_start( ) level = " << p_impl->trans_level
    << "\ntrans_level_info.offset = " << trans_level_info.offset
    << "\ntrans_level_info.op_offset = " << trans_level_info.op_offset;
   DEBUG_LOG( osstr.str( ) );
#endif
}

void ods::transaction_commit( )
{
   guard lock_write( write_lock );
   guard lock_read( read_lock );
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      throw ods_error( "ods instance in bad state..." );

   if( *p_impl->rp_bulk_level && *p_impl->rp_bulk_mode != impl::e_bulk_mode_write )
      throw ods_error( "cannot commit transaction when bulk locked for dumping or reading" );

   int_t size = p_impl->p_trans_buffer->levels.top( ).size;
   int_t op_count = p_impl->p_trans_buffer->levels.top( ).op_count;

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
      }

      auto_ptr< ods::bulk_write > ap_bulk_write;
      if( !*p_impl->rp_bulk_level )
         ap_bulk_write.reset( new ods::bulk_write( *this ) );

      // NOTE: Ops are processed in reverse order so that earlier ops on the same entry
      // can simply be ignored through setting and later checking the trans_flag value.
      // If they were instead processed in ascending order then any repeated writes (or
      // writes that are later followed by a delete) would all have to take place which
      // would be less efficient.
      for( int_t i = op_count - 1; i >= 0; i-- )
      {
#ifdef ODS_DEBUG
         ostringstream osstr;
         osstr << "==> processing op #" << i;
         DEBUG_LOG( osstr.str( ) );
#endif
         read_transaction_op( op, i );

         if( op.data.id.get_num( ) < p_impl->rp_header_info->total_entries )
         {
            read_index_entry( index_entry, op.data.id.get_num( ) );

            if( index_entry.trans_flag != ods_index_entry::e_trans_none
             && index_entry.trans_flag != ods_index_entry::e_trans_free_list )
            {
               if( index_entry.lock_flag == ods_index_entry::e_lock_none )
               {
                  if( index_entry.trans_flag == ods_index_entry::e_trans_delete )
                  {
                     if( index_entry.data.pos + index_entry.data.size
                      == p_impl->rp_header_info->total_size_of_data )
                        p_impl->rp_header_info->total_size_of_data -= index_entry.data.size;

                     if( !p_impl->rp_header_info->index_free_list )
                        index_entry.data.pos = 0;
                     else
                        index_entry.data.pos = p_impl->rp_header_info->index_free_list;

                     index_entry.data.size = 0;
                     index_entry.trans_flag = ods_index_entry::e_trans_free_list;

                     p_impl->rp_header_info->index_free_list = op.data.id.get_num( ) + 1;
                  }
                  else
                  {
                     index_entry.data.size = op.data.size;

                     if( op.type == transaction_op::e_op_type_append )
                     {
                        index_entry.data.pos = p_impl->rp_header_info->total_size_of_data;
                        p_impl->rp_header_info->total_size_of_data += op.data.size;
                     }

                     set_write_data_pos( index_entry.data.pos );

                     set_read_trans_data_pos( op.data.pos );

                     int_t chunk = c_buffer_chunk_size;
                     char buffer[ c_buffer_chunk_size ];

                     for( int_t j = 0; j < op.data.size; j += chunk )
                     {
                        if( j + chunk > op.data.size )
                           chunk = op.data.size - j;

                        read_trans_data_bytes( buffer, chunk );
                        write_data_bytes( buffer, chunk );
                     }
                  }

                  index_entry.data.tran_op = 0;

                  if( index_entry.trans_flag != ods_index_entry::e_trans_free_list )
                     index_entry.trans_flag = ods_index_entry::e_trans_none;

                  write_index_entry( index_entry, op.data.id.get_num( ) );
               }
               else
                  throw ods_error( "unexpected lock flag at " STRINGIZE( __LINE__ ) );
            }
         }
         else
            throw ods_error( "unexpected id > total_entries at " STRINGIZE( __LINE__ ) );
      }

      if( op_count )
      {
         ++p_impl->rp_header_info->data_transform_id;
         ++p_impl->rp_header_info->index_transform_id;
      }

      if( has_written_index_item )
      {
         has_written_index_item = false;
         p_impl->rp_ods_index_cache_buffer->put( p_impl->index_item_buffer, index_item_buffer_num );
      }

      if( data_write_buffer_num != -1 && data_write_buffer_offs )
      {
         p_impl->rp_ods_data_cache_buffer->put( p_impl->data_write_buffer, data_write_buffer_num );

         p_impl->rp_ods_data_cache_buffer->unlock_region(
          data_write_buffer_num * c_data_bytes_per_item, c_data_bytes_per_item );

         data_write_buffer_num = -1;
         data_write_buffer_offs = 0;
      }

      trans_read_ops_buffer_num = -1;
      trans_read_data_buffer_num = -1;
   }

   transaction_completed( );
}

void ods::transaction_rollback( )
{
   guard lock_write( write_lock );
   guard lock_read( read_lock );
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      throw ods_error( "ods instance in bad state..." );

   if( *p_impl->rp_bulk_level && *p_impl->rp_bulk_mode != impl::e_bulk_mode_write )
      throw ods_error( "cannot rollback transaction when bulk locked for dumping or reading" );

   int_t size = p_impl->p_trans_buffer->levels.top( ).size;
   int_t op_count = p_impl->p_trans_buffer->levels.top( ).op_count;
   int_t op_offset = p_impl->p_trans_buffer->levels.top( ).op_offset;

   p_impl->p_trans_buffer->levels.pop( );

   transaction_op op;
   ods_index_entry index_entry;

   if( had_interim_trans_op_write )
   {
      trans_read_ops_buffer_num = -1;
      had_interim_trans_op_write = false;
   }

   auto_ptr< ods::bulk_write > ap_bulk_write;
   if( !*p_impl->rp_bulk_level )
      ap_bulk_write.reset( new ods::bulk_write( *this ) );

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
      for( int_t i = op_count - 1; i >= 0; i-- )
      {
         read_transaction_op( op, op_offset + i );

         if( op.data.id.get_num( ) < p_impl->rp_header_info->total_entries )
         {
            read_index_entry( index_entry, op.data.id.get_num( ) );

            if( index_entry.trans_flag != ods_index_entry::e_trans_none
             && index_entry.trans_flag != ods_index_entry::e_trans_free_list )
            {
               if( op.data.old_tran_op == 0 && op.data.old_tran_id == -1 )
               {
                  if( index_entry.data.pos + index_entry.data.size
                   == p_impl->rp_header_info->total_size_of_data )
                     p_impl->rp_header_info->total_size_of_data -= index_entry.data.size;

                  if( !p_impl->rp_header_info->index_free_list )
                     index_entry.data.pos = 0;
                  else
                     index_entry.data.pos = p_impl->rp_header_info->index_free_list;

                  index_entry.data.size = 0;

                  index_entry.trans_flag = ods_index_entry::e_trans_free_list;

                  p_impl->rp_header_info->index_free_list = op.data.id.get_num( ) + 1;
               }

               index_entry.data.tran_op = op.data.old_tran_op;

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
            throw ods_error( "unexpected id > total_entries at " STRINGIZE( __LINE__ ) );
      }

      ++p_impl->rp_header_info->index_transform_id;

      if( has_written_index_item )
      {
         has_written_index_item = false;
         p_impl->rp_ods_index_cache_buffer->put( p_impl->index_item_buffer, index_item_buffer_num );
      }

      set_write_trans_data_pos( p_impl->total_trans_size - size, p_impl->total_trans_size );
   }

   p_impl->total_trans_op_count -= op_count;
   p_impl->total_trans_size -= size;

#ifdef ODS_DEBUG
   osstr.clear( );
   osstr << "\ntotal_trans_size = " << ( int_t )p_impl->total_trans_size
    << "\ntotal_trans_op_count = " << ( int_t )p_impl->total_trans_op_count;
   DEBUG_LOG( osstr.str( ) );
#endif

   unsigned num_from( p_impl->total_trans_op_count / c_trans_ops_per_item );

   if( p_impl->total_trans_op_count % c_trans_ops_per_item )
      ++num_from;

#ifdef ODS_DEBUG
   DEBUG_LOG( "<< destroy transaction cache entries after current >>" );
#endif
   p_impl->p_ods_trans_op_cache_buffer->clear_from( num_from );

   trans_read_ops_buffer_num = -1;
   trans_read_data_buffer_num = -1;

   p_impl->p_ods_trans_data_cache_buffer->clear_from( trans_write_data_buffer_num + 1 );

   transaction_completed( );
}

void ods::transaction_completed( )
{
   guard lock_write( write_lock );
   guard lock_read( read_lock );
   guard lock_impl( *p_impl->rp_impl_lock );

   if( !okay )
      throw ods_error( "ods instance in bad state..." );

#ifdef ODS_DEBUG
   ostringstream osstr;
   osstr << "ods::transaction_completed( ) level = " << p_impl->trans_level;
   DEBUG_LOG( osstr.str( ) );
#endif

   if( !--p_impl->trans_level )
   {
      p_impl->p_ods_trans_op_cache_buffer->end_transaction( );

      trans_read_data_buffer_num = -1;
      trans_read_data_buffer_offs = 0;
      p_impl->p_ods_trans_data_cache_buffer->end_transaction( );

      if( *p_impl->rp_has_changed )
      {
         { // start of file lock section...
            ods::header_file_lock header_file_lock( *this );

            p_impl->write_header_file_info( true );
            *p_impl->rp_has_changed = false;
         } // end of file lock section...
      }
   }
}

void ods::lock_header_file( )
{
   if( p_impl->rp_header_file->is_locked_for_exclusive( )
    || ( *p_impl->rp_bulk_level && !*p_impl->rp_is_in_bulk_pause ) )
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
      throw ods_error( "unable to lock header file..." );

   DEBUG_LOG( ">>>>>>>>>> captured header file lock <<<<<<<<<<" );
}

void ods::unlock_header_file( )
{
   if( p_impl->rp_header_file->is_locked_for_exclusive( )
    || ( *p_impl->rp_bulk_level && !*p_impl->rp_is_in_bulk_pause ) )
      return;

   p_impl->rp_header_file->unlock( );

   DEBUG_LOG( ">>>>>>>>>> released header file lock <<<<<<<<<<" );
}

ods& operator >>( ods& o, storable_base& s )
{
   guard lock_storable( s.storable_lock );
   guard lock_read( o.read_lock );

   DEBUG_LOG( "[read object]" );

   if( !o.okay )
      throw ods_error( "ods instance in bad state..." );

   if( o.is_in_read )
      throw ods_error( "attempt to re-enter read for same ods instance..." );

   if( s.id.get_num( ) < 0 )
      throw ods_error( "unexpected null object id at " STRINGIZE( __LINE__ ) );

   if( *o.p_impl->rp_bulk_level && *o.p_impl->rp_bulk_mode < ods::impl::e_bulk_mode_read )
      throw ods_error( "cannot read when bulk locked for dumping" );

   temp_set_value< bool > tmp_in_read( o.is_in_read, true );
   finalise_value< int_t > finalise_reading_object( o.current_read_object_num, -1 );

   int_t trans_read_pos = -1;

   ods_index_entry index_entry;

   storable_base::read_scope read_scope( s, o );

   o.bytes_retrieved = 0;

   o.data_read_buffer_num = -1;

   bool can_read = false;
   bool has_locked = false;

   int attempts = c_review_max_attempts;
   while( attempts-- )
   {
      { // start of file lock section...
         guard tmp_lock( *o.p_impl->rp_impl_lock );
         ods::header_file_lock header_file_lock( o );

         auto_ptr< ods::file_scope > ap_file_scope;
         if( !*o.p_impl->rp_bulk_level )
            ap_file_scope.reset( new ods::file_scope( o ) );

         if( s.id.get_num( ) >= o.p_impl->rp_header_info->total_entries )
            can_read = false;
         else
            can_read = true;

         if( can_read )
         {
            if( o.p_impl->found_instance_currently_writing( s.id.get_num( ) ) )
               throw ods_error( "cannot read object currently being written..." );

            o.read_index_entry( index_entry, s.id.get_num( ) );
            o.bytes_stored = index_entry.data.size;

            if( index_entry.lock_flag != ods_index_entry::e_lock_none )
               can_read = false;
            else
            {
               if( !o.p_impl->trans_level
                || ( index_entry.trans_flag == ods_index_entry::e_trans_none )
                || ( index_entry.data.tran_id == o.p_impl->p_trans_buffer->tran_id ) )
               {
                  if( !o.p_impl->rp_ods_index_cache_buffer->lock_entry( s.id.get_num( ), false ) )
                     can_read = false;
                  else
                     has_locked = true;
               }
               else
                  can_read = false;
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
            }

            if( index_entry.trans_flag == ods_index_entry::e_trans_none
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
      } // end of file lock section...

      msleep( c_review_attempt_sleep_time );
   }

   if( !can_read )
      throw ods_error( "cannot read (max. attempts exceeded)..." );

   // NOTE: If within a transaction then reading data that has been committed by another transaction
   // that commenced after this one is prevented (this is needed to make transactions serializable).
   if( o.p_impl->trans_level && index_entry.data.tran_id > o.p_impl->p_trans_buffer->tran_id )
   {
      if( has_locked )
         o.p_impl->rp_ods_index_cache_buffer->unlock_entry( s.id.get_num( ), false );
      throw ods_error( "unable to read due to interim transaction write..." );
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
      throw ods_error( osstr.str( ) );
   }

   return o;
}

ods& operator <<( ods& o, storable_base& s )
{
   guard lock_storable( s.storable_lock );
   guard lock_write( o.write_lock );

   DEBUG_LOG( "[write object]" );

   if( !o.okay )
      throw ods_error( "ods instance in bad state..." );

   if( o.is_in_write )
      throw ods_error( "attempt to re-enter write for same ods instance..." );

   if( *o.p_impl->rp_bulk_level && *o.p_impl->rp_bulk_mode != ods::impl::e_bulk_mode_write )
      throw ods_error( "cannot write when bulk locked for dumping or reading" );

   temp_set_value< bool > tmp_in_write( o.is_in_write, true );
   finalise_value< int_t > finalise_writing_object( o.current_write_object_num, -1 );

   int_t old_tran_id = -1;
   int_t trans_write_pos = -1;

   ods_index_entry index_entry;
   bool is_new_object = true;
   bool was_updated_in_place = false;

   storable_base::write_scope write_scope( s, o );

   o.bytes_used = 0;
   o.bytes_reserved = s.get_size_of( );

   bool can_write = false;
   bool has_locked = false;

   int attempts = c_update_max_attempts;
   while( attempts-- )
   {
      { // start of file lock section...
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
               throw ods_error( "cannot store out of range oid..." );

            if( o.p_impl->found_instance_currently_reading( s.id.get_num( ) ) )
               throw ods_error( "cannot write object currently being read..." );

            if( o.p_impl->found_instance_currently_writing( s.id.get_num( ) ) )
               throw ods_error( "cannot write object currently being written..." );

            o.read_index_entry( index_entry, s.id.get_num( ) );
            old_tran_id = index_entry.data.tran_id;

            is_new_object = false;
         }

         int_t old_data_pos = -1;
         int_t old_total_size = o.p_impl->rp_header_info->total_size_of_data;

         int_t* p_total_to_increment = 0;

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
                  throw ods_error( "unexpected lock flag value found in freelist entry" );

               if( index_entry.trans_flag != ods_index_entry::e_trans_free_list )
                  throw ods_error( "unexpected trans flag value found in freelist entry" );

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
               if( index_entry.trans_flag == ods_index_entry::e_trans_none
                || ( o.p_impl->trans_level
                && index_entry.data.tran_id == o.p_impl->p_trans_buffer->tran_id
                && index_entry.trans_flag != ods_index_entry::e_trans_delete ) )
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
               ++o.p_impl->rp_header_info->total_entries;
            }
            else if( p_total_to_increment == &*o.p_impl->rp_session_revive_total )
            {
               DEBUG_LOG( "(revive index entry)" );
               o.p_impl->rp_header_info->index_free_list = old_data_pos;
            }
            else
               DEBUG_LOG( "(update index entry)" );

            ++*p_total_to_increment;

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

               index_entry.data.tran_id = o.p_impl->p_trans_buffer->tran_id;
               index_entry.trans_flag = ods_index_entry::e_trans_change;
            }
            else
            {
               if( !is_new_object && o.bytes_reserved <= index_entry.data.size )
                  was_updated_in_place = true;
               else
                  index_entry.data.pos = old_total_size;

               index_entry.data.size = o.bytes_reserved;
               index_entry.data.tran_id = o.p_impl->rp_header_info->transaction_id;
            }

            if( !*o.p_impl->rp_bulk_level && !o.p_impl->trans_level )
               index_entry.lock_flag = ods_index_entry::e_lock_entry;

            o.write_index_entry( index_entry, s.id.num );

            if( !o.p_impl->trans_level )
            {
               ++o.p_impl->rp_header_info->transaction_id;

               if( !was_updated_in_place )
                  o.p_impl->rp_header_info->total_size_of_data += o.bytes_reserved;
            }

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
      } // end of file lock section...

      msleep( c_update_attempt_sleep_time );
   }

   if( !can_write )
      throw ods_error( "cannot write (max. attempts exceeded)..." );

   if( !o.p_impl->trans_level )
      o.set_write_data_pos( index_entry.data.pos );
   else
      o.set_write_trans_data_pos( trans_write_pos, trans_write_pos );

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

   { // start of file lock section...
      guard tmp_lock( *o.p_impl->rp_impl_lock );
      ods::header_file_lock header_file_lock( o );

      auto_ptr< ods::file_scope > ap_file_scope;
      if( !*o.p_impl->rp_bulk_level )
         ap_file_scope.reset( new ods::file_scope( o ) );

      if( !o.p_impl->trans_level )
      {
         ++o.p_impl->rp_header_info->data_transform_id;

         if( !*o.p_impl->rp_bulk_level )
         {
            index_entry.lock_flag = ods_index_entry::e_lock_none;
            o.write_index_entry( index_entry, s.id.num );
         }
      }

      if( has_locked )
         o.p_impl->rp_ods_index_cache_buffer->unlock_entry( s.id.get_num( ), true );

      if( !o.p_impl->trans_level && o.data_write_buffer_num != -1 && o.data_write_buffer_offs )
      {
         o.p_impl->rp_ods_data_cache_buffer->put( o.p_impl->data_write_buffer, o.data_write_buffer_num );

         o.p_impl->rp_ods_data_cache_buffer->unlock_region(
          o.data_write_buffer_num * c_data_bytes_per_item, c_data_bytes_per_item );

         o.data_write_buffer_num = -1;
         o.data_write_buffer_offs = 0;
      }

      if( o.p_impl->trans_level && o.trans_write_ops_buffer_num != -1 && o.has_written_trans_op )
      {
         o.p_impl->p_ods_trans_op_cache_buffer->put( o.trans_write_ops_buffer, o.trans_write_ops_buffer_num );

         o.has_written_trans_op = false;
         o.had_interim_trans_op_write = true;
      }

      if( o.p_impl->trans_level && o.trans_write_data_buffer_num != -1 && o.trans_write_data_buffer_offs )
      {
         o.p_impl->p_ods_trans_data_cache_buffer->put( o.trans_write_buffer, o.trans_write_data_buffer_num );

         o.trans_write_data_buffer_offs = 0;
         o.had_interim_trans_data_write = true;
      }
   } // end of file lock section...

   if( o.bytes_used > o.bytes_reserved )
   {
      ostringstream osstr;
      osstr << "max. data length exceeded (reserved = "
       << o.bytes_reserved << ", used = " << o.bytes_used << ")";
      throw ods_error( osstr.str( ) );
   }

   return o;
}

void ods::read( unsigned char* p_buf, int_t len )
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

void ods::write( const unsigned char* p_buf, int_t len )
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

void ods::set_read_data_pos( int_t pos )
{
#ifdef ODS_DEBUG
   ostringstream osstr;
   osstr << "ods::set_read_data_pos( ) pos = " << pos << ", data_read_buffer_num = " << data_read_buffer_num;
   DEBUG_LOG( osstr.str( ) );
#endif

   int_t current_data_buffer_num( data_read_buffer_num );

   data_read_buffer_num = pos / c_data_bytes_per_item;
   data_read_buffer_offs = pos % c_data_bytes_per_item;

   if( data_read_buffer_num != current_data_buffer_num )
      p_impl->data_read_buffer = p_impl->rp_ods_data_cache_buffer->get( data_read_buffer_num );
}

void ods::set_write_data_pos( int_t pos )
{
#ifdef ODS_DEBUG
   ostringstream osstr;
   osstr << "ods::set_write_data_pos( ) pos = " << pos
    << ", data_write_buffer_num = " << data_write_buffer_num;
   DEBUG_LOG( osstr.str( ) );
#endif
   int_t current_data_buffer_num( data_write_buffer_num );

   data_write_buffer_num = pos / c_data_bytes_per_item;
   data_write_buffer_offs = pos % c_data_bytes_per_item;

   if( data_write_buffer_num != current_data_buffer_num )
   {
      if( current_data_buffer_num != -1 )
      {
         p_impl->rp_ods_data_cache_buffer->put( p_impl->data_write_buffer, current_data_buffer_num );

         p_impl->rp_ods_data_cache_buffer->unlock_region(
          current_data_buffer_num * c_data_bytes_per_item, c_data_bytes_per_item );
      }

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
         throw ods_error( "unable to lock data region for set_write_data_pos..." );

      p_impl->data_write_buffer = p_impl->rp_ods_data_cache_buffer->get( data_write_buffer_num );
   }
}

void ods::adjust_read_data_pos( int_t adjust )
{
#ifdef ODS_DEBUG
   ostringstream osstr;
   osstr << "ods::adjust_read_data_pos( ) adjust = " << adjust;
   DEBUG_LOG( osstr.str( ) );
#endif

   int_t current_data_buffer_num( data_read_buffer_num );
   int_t pos = ( data_read_buffer_num * c_data_bytes_per_item ) + data_read_buffer_offs + adjust;

   data_read_buffer_num = pos / c_data_bytes_per_item;
   data_read_buffer_offs = pos % c_data_bytes_per_item;

   if( data_read_buffer_num != current_data_buffer_num )
      p_impl->data_read_buffer = p_impl->rp_ods_data_cache_buffer->get( data_read_buffer_num );
}

void ods::adjust_write_data_pos( int_t adjust )
{
#ifdef ODS_DEBUG
   ostringstream osstr;
   osstr << "ods::adjust_write_data_pos( ) adjust = " << adjust;
   DEBUG_LOG( osstr.str( ) );
#endif

   int_t current_data_buffer_num( data_write_buffer_num );
   int_t pos = ( data_write_buffer_num * c_data_bytes_per_item ) + data_write_buffer_offs + adjust;

   data_write_buffer_num = pos / c_data_bytes_per_item;
   data_write_buffer_offs = pos % c_data_bytes_per_item;

   if( data_write_buffer_num != current_data_buffer_num )
   {
      if( current_data_buffer_num != -1 )
      {
         p_impl->rp_ods_data_cache_buffer->put( p_impl->data_write_buffer, current_data_buffer_num );

         p_impl->rp_ods_data_cache_buffer->unlock_region(
          current_data_buffer_num * c_data_bytes_per_item, c_data_bytes_per_item );
      }

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
         throw ods_error( "unable to lock data region for adjust_write_data_pos..." );

      p_impl->data_write_buffer = p_impl->rp_ods_data_cache_buffer->get( data_write_buffer_num );
   }
}

void ods::read_data_bytes( char* p_dest, int_t len )
{
   int_t chunk = min( len, c_data_bytes_per_item - data_read_buffer_offs );

   while( len > 0 )
   {
      if( p_dest )
         memcpy( p_dest, &p_impl->data_read_buffer.data[ data_read_buffer_offs ], chunk );

      len -= chunk;

      if( len )
      {
         data_read_buffer_offs = 0;
         p_impl->data_read_buffer = p_impl->rp_ods_data_cache_buffer->get( ++data_read_buffer_num );

         if( p_dest )
            p_dest += chunk;

         chunk = min( len, ( int_t )c_data_bytes_per_item );
      }
      else
         data_read_buffer_offs += chunk;
   }
}

void ods::write_data_bytes( const char* p_src, int_t len )
{
   int_t chunk = min( len, c_data_bytes_per_item - data_write_buffer_offs );

   while( len > 0 )
   {
      if( p_src )
         memcpy( &p_impl->data_write_buffer.data[ data_write_buffer_offs ], p_src, chunk );
      else if( p_impl->force_padding )
         memset( &p_impl->data_write_buffer.data[ data_write_buffer_offs ], '\0', chunk );

      len -= chunk;

      if( len )
      {
         p_impl->rp_ods_data_cache_buffer->put( p_impl->data_write_buffer, data_write_buffer_num );

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
            throw ods_error( "unable to lock data region for write_data_bytes..." );

         p_impl->data_write_buffer = p_impl->rp_ods_data_cache_buffer->get( data_write_buffer_num );

         if( p_src )
            p_src += chunk;

         chunk = min( len, ( int_t )c_data_bytes_per_item );
      }
      else
         data_write_buffer_offs += chunk;
   }
}

void ods::read_index_entry( ods_index_entry& index_entry, int_t num )
{
   int_t current_index_buffer_num( index_item_buffer_num );

   index_item_buffer_num = num / c_index_items_per_item;

   if( index_item_buffer_num != current_index_buffer_num )
   {
      if( has_written_index_item )
      {
         has_written_index_item = false;
         p_impl->rp_ods_index_cache_buffer->put( p_impl->index_item_buffer, current_index_buffer_num );
      }

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

void ods::write_index_entry( const ods_index_entry& index_entry, int_t num )
{
   if( !*p_impl->rp_has_changed )
      p_impl->write_header_file_info( );

   int_t current_index_buffer_num( index_item_buffer_num );

   index_item_buffer_num = num / c_index_items_per_item;

   if( index_item_buffer_num != current_index_buffer_num )
   {
      if( has_written_index_item )
      {
         has_written_index_item = false;
         p_impl->rp_ods_index_cache_buffer->put( p_impl->index_item_buffer, current_index_buffer_num );
      }

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

   has_written_index_item = true;
}

void ods::read_transaction_op( transaction_op& op, int_t num )
{
   int_t current_trans_ops_buffer_num( trans_read_ops_buffer_num );

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

void ods::write_transaction_op( transaction_op& op )
{
   int_t current_trans_ops_buffer_num( trans_write_ops_buffer_num );

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

   has_written_trans_op = true;
}

void ods::set_read_trans_data_pos( int_t pos )
{
   int_t current_trans_buffer_num( trans_read_data_buffer_num );
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
      trans_read_buffer = p_impl->p_ods_trans_data_cache_buffer->get( trans_read_data_buffer_num );
}

void ods::set_write_trans_data_pos( int_t pos, int_t old_trans_total_size )
{
   int_t current_trans_buffer_num( trans_write_data_buffer_num );
   trans_write_data_buffer_num = pos / c_trans_bytes_per_item;
   trans_write_data_buffer_offs = pos % c_trans_bytes_per_item;

#ifdef ODS_DEBUG
   ostringstream osstr;
   osstr << "ods::set_write_trans_data_pos( ) pos = " << pos
    << ", old_trans_total_size = " << old_trans_total_size
    << "\ntrans_write_data_buffer_num = " << trans_write_data_buffer_num
    << ", trans_write_data_buffer_offs = " << trans_write_data_buffer_offs;
   DEBUG_LOG( osstr.str( ) );
#endif

   if( trans_write_data_buffer_num != current_trans_buffer_num )
   {
      if( current_trans_buffer_num != -1 )
         p_impl->p_ods_trans_data_cache_buffer->put( trans_write_buffer, current_trans_buffer_num );

      if( pos == old_trans_total_size )
         memset( ( char* )&trans_write_buffer, '\0', sizeof( trans_data_buffer ) );
      else
         trans_write_buffer = p_impl->p_ods_trans_data_cache_buffer->get( trans_write_data_buffer_num );
   }
}

void ods::adjust_read_trans_data_pos( int_t adjust )
{
#ifdef ODS_DEBUG
   ostringstream osstr;
   osstr << "ods::adjust_read_trans_data_pos( ) adjust = " << adjust
   << "\ntrans_read_data_buffer_num = " << trans_read_data_buffer_num
   << "\ntrans_read_data_buffer_offs = " << trans_read_data_buffer_offs;
   DEBUG_LOG( osstr.str( ) );
#endif

   int_t current_trans_buffer_num( trans_read_data_buffer_num );
   int_t pos = ( trans_read_data_buffer_num * c_trans_bytes_per_item ) + trans_read_data_buffer_offs + adjust;

   trans_read_data_buffer_num = pos / c_trans_bytes_per_item;
   trans_read_data_buffer_offs = pos % c_trans_bytes_per_item;

   if( trans_read_data_buffer_num != current_trans_buffer_num )
      trans_read_buffer = p_impl->p_ods_trans_data_cache_buffer->get( trans_read_data_buffer_num );
}

void ods::adjust_write_trans_data_pos( int_t adjust )
{
#ifdef ODS_DEBUG
   ostringstream osstr;
   osstr << "ods::adjust_write_trans_data_pos( ) adjust = " << adjust
    << "\ntrans_write_data_buffer_num = " << trans_write_data_buffer_num
    << "\ntrans_write_data_buffer_offs = " << trans_write_data_buffer_offs;
   DEBUG_LOG( osstr.str( ) );
#endif

   int_t current_trans_buffer_num( trans_write_data_buffer_num );
   int_t pos = ( trans_write_data_buffer_num * c_trans_bytes_per_item ) + trans_write_data_buffer_offs + adjust;

   trans_write_data_buffer_num = pos / c_trans_bytes_per_item;
   trans_write_data_buffer_offs = pos % c_trans_bytes_per_item;

   if( trans_write_data_buffer_num != current_trans_buffer_num )
   {
      if( current_trans_buffer_num != -1 )
         p_impl->p_ods_trans_data_cache_buffer->put( trans_write_buffer, current_trans_buffer_num );
      trans_write_buffer = p_impl->p_ods_trans_data_cache_buffer->get( trans_write_data_buffer_num );
   }
}

void ods::read_trans_data_bytes( char* p_dest, int_t len )
{
#ifdef ODS_DEBUG
   ostringstream osstr;
   osstr << "ods::read_trans_data_bytes( ) len = " << len
    << "\ntrans_read_data_buffer_num = " << trans_read_data_buffer_num
    << "\ntrans_read_data_buffer_offs = " << trans_read_data_buffer_offs;
   DEBUG_LOG( osstr.str( ) );
#endif

   int_t chunk = min( len, c_trans_bytes_per_item - trans_read_data_buffer_offs );

   while( len > 0 )
   {
      if( p_dest )
         memcpy( p_dest, &trans_read_buffer.data[ trans_read_data_buffer_offs ], chunk );

      len -= chunk;

      if( len )
      {
         trans_read_data_buffer_offs = 0;
         trans_read_buffer = p_impl->p_ods_trans_data_cache_buffer->get( ++( trans_read_data_buffer_num ) );

         if( p_dest )
            p_dest += chunk;
         chunk = min( len, c_trans_bytes_per_item );
      }
      else
         trans_read_data_buffer_offs += chunk;
   }
}

void ods::write_trans_data_bytes( const char* p_src, int_t len )
{
#ifdef ODS_DEBUG
   ostringstream osstr;
   osstr << "ods::write_trans_data_bytes( ) len = " << len
    << "\ntrans_write_data_buffer_num = " << trans_write_data_buffer_num
    << "\ntrans_write_data_buffer_offs = " << trans_write_data_buffer_offs;
   DEBUG_LOG( osstr.str( ) );
#endif

   int_t chunk = min( len, c_trans_bytes_per_item - trans_write_data_buffer_offs );

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
         p_impl->p_ods_trans_data_cache_buffer->put( trans_write_buffer, trans_write_data_buffer_num++ );

         memset( &trans_write_buffer, '\0', sizeof( trans_data_buffer ) );

         if( p_src )
            p_src += chunk;
         chunk = min( len, c_trans_bytes_per_item );
      }
      else
         trans_write_data_buffer_offs += chunk;
   }
}

