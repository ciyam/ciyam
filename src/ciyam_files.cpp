// Copyright (c) 2013-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstring>
#  include <map>
#  include <set>
#  include <memory>
#  include <fstream>
#  include <stdexcept>
#  ifdef __GNUG__
#     include <unistd.h>
#  endif
#endif

#define CIYAM_BASE_IMPL

#include "ciyam_files.h"

#include "ods.h"
#include "sio.h"
#include "regex.h"
#include "base64.h"
#include "config.h"
#include "format.h"
#include "sha256.h"
#include "sockets.h"
#include "threads.h"
#include "progress.h"
#include "date_time.h"
#include "utilities.h"
#include "ciyam_base.h"
#include "file_utils.h"
#include "auto_script.h"
#include "fs_iterator.h"
#include "crypt_stream.h"
#include "ciyam_variables.h"
#include "ods_file_system.h"
#include "ciyam_core_files.h"

#ifdef ZLIB_SUPPORT
#  include <zlib.h>
#endif

using namespace std;

namespace
{

const int c_status_info_pad_len = 10;
const int c_yyyymmmddhhmmss_len = 14;

const int c_min_size_to_compress = 33;

const int c_num_digest_characters = c_sha256_digest_size * 2;

const int c_depth_to_omit_blob_content = 999;

const size_t c_max_repo_lock_attempts = 10;

unsigned int c_repo_lock_attempt_msleep = 1000;

const char c_crc32_separator = '#';

const char c_prefix_wildcard_separator = ':';

const char* const c_time_stamp_tag_prefix = "ts.";

const char* const c_time_stamp_unix_epoch = "19700101";

const char* const c_file_archive_path = "path";
const char* const c_file_archive_size_avail = "size_avail";
const char* const c_file_archive_size_limit = "size_limit";
const char* const c_file_archive_status_info = "status_info";

const char* const c_file_archive_status_is_full = "is full";
const char* const c_file_archive_status_bad_write = "bad write";
const char* const c_file_archive_status_bad_access = "bad access";
const char* const c_file_archive_status_status_bad_create = "bad create";

const char* const c_attribute_local_hash = "local_hash";
const char* const c_attribute_local_public_key = "local_public_key";
const char* const c_attribute_master_public_key = "master_public_key";

#include "ciyam_constants.h"

#include "udp_stream_helper.cpp"

trace_mutex g_mutex;

string g_empty_string;

string hex_crc32( const string& s )
{
   uint32_t crc = crc32( s );

   return hex_encode( ( const unsigned char* )&crc, sizeof( uint32_t ) );
}

class file_hash_info
{
   public:
   file_hash_info( )
    :
    type( 0 )
   {
      memset( data, '\0', sizeof( data ) );
   }

   file_hash_info( const file_hash_info& src )
    :
    type( src.type )
   {
      memcpy( data, src.data, c_sha256_digest_size );
   }

   file_hash_info( const string& hash, unsigned char type = 0 )
    :
    type( type )
   {
      string encoded( hash );

      size_t num_hex_chars = ( c_sha256_digest_size * 2 );

      if( encoded.length( ) < num_hex_chars )
         encoded += string( num_hex_chars - hash.length( ), '0' );

      hex_decode( encoded, data, sizeof( data ) );
   }

   bool operator <( const file_hash_info& o ) const
   {
      return memcmp( data, o.data, sizeof( data ) ) < 0;
   }

   bool operator ==( const file_hash_info& o ) const
   {
      return memcmp( data, o.data, sizeof( data ) ) == 0;
   }

   bool operator !=( const file_hash_info& o ) const
   {
      return memcmp( data, o.data, sizeof( data ) ) != 0;
   }

   unsigned char get_type( ) const { return type; }

   string get_hash_string( ) const
   {
      return hex_encode( data, sizeof( data ) );
   }

   private:
   unsigned char type;
   unsigned char data[ c_sha256_digest_size ];
};

map< string, file_hash_info > g_tag_hashes;
multimap< file_hash_info, string > g_hash_tags;

struct archive_file_info
{
   bool has_file( const string& hash ) const;

   void add_file( const string& hash, int64_t time_stamp );
   void remove_file( const string& hash, bool skip_not_found = false );

   string get_oldest_file( );

   size_t total_files( ) const { return hash_times.size( ); }

   map< file_hash_info, int64_t > hash_times;
   multimap< int64_t, file_hash_info > time_hashes;
};

bool archive_file_info::has_file( const string& hash ) const
{
   return hash_times.count( hash );
}

void archive_file_info::add_file( const string& hash, int64_t time_stamp )
{
   hash_times.insert( make_pair( hash, time_stamp ) );
   time_hashes.insert( make_pair( time_stamp, hash ) );
}

void archive_file_info::remove_file( const string& hash, bool skip_not_found )
{
   map< file_hash_info, int64_t >::iterator hi = hash_times.find( hash );

   if( hi != hash_times.end( ) )
   {
      multimap< int64_t, file_hash_info >::iterator ti = time_hashes.lower_bound( hi->second );

      while( true )
      {
         if( ti->second == hash )
            break;

         ++ti;

         if( ( ti == time_hashes.end( ) ) || ( ti->first != hi->second ) )
            break;
      }

      if( ti == time_hashes.end( ) || ti->second != hash )
         throw runtime_error( "unexpected time " + to_string( hi->second )
          + " for file '" + hash + "' not found in archive_file_info::remove_file" );

      hash_times.erase( hi );
      time_hashes.erase( ti );
   }
   else
   {
      if( !skip_not_found )
         throw runtime_error( "unexpected file '" + hash + "' not found in archive_file_info::remove_file" );
   }
}

string archive_file_info::get_oldest_file( )
{
   string retval;

   multimap< int64_t, file_hash_info >::iterator ti = time_hashes.begin( );

   if( ti != time_hashes.end( ) )
      retval = ti->second.get_hash_string( );

   return retval;
}

map< string, archive_file_info > g_archive_file_info;

unsigned char get_file_type_and_extra( const string& hash, const char* p_file_name = 0, bool* p_read_from_file = 0 )
{
   unsigned char file_type_and_extra = '\0';

   multimap< file_hash_info, string >::iterator i = g_hash_tags.lower_bound( hash );

   if( i == g_hash_tags.end( ) || ( hash != i->first.get_hash_string( ) ) )
   {
      if( !p_file_name )
         throw runtime_error( "unexpected hash tag for " + hash + " not found" );

      file_size( p_file_name, &file_type_and_extra, sizeof( file_type_and_extra ) );

      if( p_read_from_file )
         *p_read_from_file = true;
   }
   else
      file_type_and_extra = i->first.get_type( );

   return file_type_and_extra;
}

void set_file_type_and_extra( const string& hash, unsigned char file_type_and_extra )
{
   multimap< file_hash_info, string >::iterator i = g_hash_tags.lower_bound( hash );

   if( i == g_hash_tags.end( ) || ( hash != i->first.get_hash_string( ) ) )
      throw runtime_error( "unable to find " + hash + " in 'set_file_type_and_extra'" );
   else 
   {
      vector< string > tags;

      file_hash_info info( i->first.get_hash_string( ), file_type_and_extra );

      while( true )
      {
         tags.push_back( i->second );

         g_hash_tags.erase( i );

         i = g_hash_tags.lower_bound( hash );

         if( i == g_hash_tags.end( ) || hash != i->first.get_hash_string( ) )
            break;
      }

      // NOTE: Only replace "g_hash_tags" items as "type" in the "file_hash_info"
      // used in "g_tag_hashes" is not used (only the hash itself is being used).
      for( size_t n = 0; n < tags.size( ); n++ )
         g_hash_tags.insert( make_pair( info, tags[ n ] ) );
   }
}

size_t g_total_files = 0;
int64_t g_total_bytes = 0;

set< string > g_files_constructing;

struct file_constructor
{
   file_constructor( const string& file_name );
   ~file_constructor( );

   string file_name;
};

file_constructor::file_constructor( const string& file_name )
 :
 file_name( file_name )
{
   guard g( g_mutex );

   if( g_files_constructing.count( file_name ) )
      throw runtime_error( "file '" + file_name + "' is already being constructed" );

   g_files_constructing.insert( file_name );
}

file_constructor::~file_constructor( )
{
   guard g( g_mutex );

   g_files_constructing.erase( file_name );
}

int64_t local_secs_diff( )
{
   date_time local( date_time::local( ) );
   date_time standard( date_time::standard( ) );

   int64_t secs_diff = seconds_between( standard, local );

   if( ( secs_diff % 60 ) == 1 )
      --secs_diff;
   else if( ( secs_diff % 60 ) == 59 )
      ++secs_diff;

   return secs_diff;
}

string unique_time_stamp_tag( const string& prefix, const date_time& dt )
{
   string retval;

   bool okay = false;

   string formatted_time( dt.as_string( e_time_format_hhmmss, false ) );

   // NOTE: To ensure that the tag is unique try multiple variations (changing the final digits).
   for( int i = 0; i < 100000; i++ )
   {
      string next( formatted_time + to_comparable_string( i, false, 5 ) );

      if( !has_tag( prefix + next ) )
      {
         okay = true;
         retval = next;

         break;
      }
   }

   if( !okay )
      throw runtime_error( "unable to create a unique time-stamp tag" );

   return retval;
}

void create_directory_if_not_exists( const string& dir_name )
{
   if( !dir_exists( dir_name ) )
   {
      bool rc;

      create_dir( dir_name, &rc );

      if( !rc )
         throw runtime_error( "unable to create directory '" + dir_name + "'" );
   }
}

string construct_file_name_from_hash(
 const string& hash, bool create_directory = false,
 bool check_hash_pattern = true, string* p_alt_path = 0, bool alt_path_only = false )
{
   if( hash.length( ) < 3 )
      return string( );

   string file_name;

   if( check_hash_pattern )
   {
      regex expr( c_regex_hash_256, true, true );

      if( expr.search( hash ) == string::npos )
         throw runtime_error( "unexpected hash '" + hash + "'" );
   }

   if( p_alt_path )
   {
      file_name = *p_alt_path;

      file_name += '/';

      file_name += hash;

      if( alt_path_only || file_exists( file_name ) )
         return file_name;
   }

   file_name = get_files_area_dir( );

   file_name += '/';
   file_name += lower( hash.substr( 0, 2 ) );

   if( create_directory )
      create_directory_if_not_exists( file_name );

   file_name += '/';
   file_name += lower( hash.substr( 2 ) );

   return file_name;
}

string create_empty_file( )
{
   guard g( g_mutex );

   string data( c_file_type_str_blob );

   string hash( sha256( data ).get_digest_as_string( ) );

   string file_name( construct_file_name_from_hash( hash, true ) );

   if( !file_exists( file_name ) )
   {
      if( g_total_files >= get_files_area_item_max_num( ) )
         throw runtime_error( "unexpected maximum files would be exceeded for 'create_empty_file'" );

      ++g_total_bytes;
      ++g_total_files;

      write_file( file_name, data );
   }

   return hash;
}

void validate_list( const string& data, bool* p_rc = 0,
 bool allow_missing_items = false, bool* p_has_encrypted_blobs = 0 )
{
   vector< string > list_items;

   split_list_items( data, list_items );

   for( size_t i = 0; i < list_items.size( ); i++ )
   {
      string next_list_item( list_items[ i ] );
      string::size_type pos = next_list_item.find( ' ' );

      string next_hash( next_list_item.substr( 0, pos ) );

      if( allow_missing_items )
      {
         if( !are_hex_nibbles( next_hash )
          || ( next_hash.size( ) != ( c_sha256_digest_size * 2 ) ) )
         {
            if( p_rc )
            {
               *p_rc = false;
               return;
            }
            else
               throw runtime_error( "list item file '" + next_list_item.substr( 0, pos ) + "' is invalid" );
         }
      }
      else if( !has_file( next_hash ) && !has_file_been_archived( next_hash ) )
      {
         if( p_rc )
         {
            *p_rc = false;
            return;
         }
         else
            throw runtime_error( "list item file '" + next_list_item.substr( 0, pos ) + "' does not exist" );
      }
      else
      {
         bool is_list_item = false;

         if( p_has_encrypted_blobs && is_encrypted_file( next_hash, &is_list_item ) )
         {
            if( !is_list_item )
               *p_has_encrypted_blobs = true;
         }
      }
   }

   if( p_rc )
      *p_rc = true;
}

void validate_hash_with_uncompressed_content( const string& hash, const string& file_name )
{
   sha256 test_hash;
   test_hash.update( file_name, true );

   if( hash != test_hash.get_digest_as_string( ) )
      throw runtime_error( "invalid content for '" + hash + "' (hash does not match hashed data)" );
}

void validate_hash_with_uncompressed_content( const string& hash,
 unsigned char* p_data, unsigned long length, const char* p_alt_error_message = 0 )
{
   bool okay = false;

   if( length )
   {
      // NOTE: Although the compressed bit flag could be still set it is
      // required that the data checked here will have been uncompressed
      // first.
      bool is_compressed = ( p_data[ 0 ] & c_file_type_val_compressed );

      if( is_compressed )
         p_data[ 0 ] &= ~c_file_type_val_compressed;

      sha256 test_hash;
      test_hash.update( p_data, length );

      if( lower( hash ) == test_hash.get_digest_as_string( ) )
         okay = true;

      if( is_compressed )
         p_data[ 0 ] |= c_file_type_val_compressed;
   }

   if( !okay )
   {
      if( p_alt_error_message )
         throw runtime_error( p_alt_error_message );
      else
         throw runtime_error( "invalid content for '" + hash + "' (hash does not match hashed data)" );
   }
}

void remove_file_padding( const string& hash, const string& extra_header, string& file_data, size_t& total_bytes )
{
   string extra_header_info( base64::decode( extra_header ) );

   stringstream ss( extra_header_info );

   crypt_stream( ss, hash, e_stream_cipher_chacha20 );

   extra_header_info = ss.str( );

   size_t padding = 0;

   string::size_type pos = extra_header_info.find( c_crc32_separator );

   if( pos == string::npos )
      throw runtime_error( "invalid extra file transfer header (missing checksum)" );

   string chksum( hex_crc32( file_data ) );

   if( chksum != extra_header_info.substr( pos + 1 ) )
      throw runtime_error( "invalid extra file transfer header (incorrect checksum)" );

   extra_header_info.erase( pos );

   pos = extra_header_info.find( '-' );

   if( pos == string::npos )
      throw runtime_error( "invalid extra file transfer header (missing separator)" );

   size_t total = from_string< size_t >( extra_header_info.substr( 0, pos ) );

   padding = from_string< size_t >( extra_header_info.substr( pos + 1 ) );

   if( padding )
   {
      total_bytes -= padding;
      file_data.erase( file_data.length( ) - padding );
   }

   if( total != total_bytes )
      throw runtime_error( "extra header total: " + to_string( total )
       + " does not equal total_bytes value: " + to_string( total_bytes ) );
}

void encrypt_file_buffer( const string& hash, const string& repository,
 session_file_buffer_access& file_buffer, string& crypt_password, size_t offset, size_t length, bool is_shared )
{
   string file_data;
   file_buffer.copy_to_string( file_data, offset, length );

   string stream_cipher( get_session_variable(
    get_special_var_name( e_special_var_stream_cipher ) ) );

   stringstream ss( file_data.substr( 1 ) );

   // NOTE: Use the file content hash as salt.
   crypt_stream( ss, combined_clear_key( crypt_password, hash ), stream_cipher_value( stream_cipher ) );

   string new_file_data( file_data.substr( 0, 1 ) );

   new_file_data += ss.str( );

   new_file_data[ 0 ] |= c_file_type_val_encrypted;

   file_buffer.copy_from_string( new_file_data, offset );

   if( is_shared )
      add_peer_mapped_hash_info( repository, hash, sha256( new_file_data ).get_digest_as_string( ) );
   else if( !repository.empty( ) )
   {
      string dummy, public_key;

      if( has_repository_entry_record( repository, hash ) )
         fetch_repository_entry_record( repository, hash, dummy, dummy, public_key );
      else
      {
         create_peer_repository_entry_push_info( hash, crypt_password, &public_key, false );

         store_repository_entry_record( repository, hash, "", public_key, public_key );
      }

      add_peer_mapped_hash_info( repository, hash, sha256( new_file_data ).get_digest_as_string( ) + ':' + public_key );
   }

   clear_key( crypt_password );
}

string transform_shared_list_info( const string& repository,
 session_file_buffer_access& file_buffer, size_t offset, size_t length, progress* p_progress )
{
   string list_data;
   file_buffer.copy_to_string( list_data, offset, length );

   string encrypted_list_data( list_data.substr( 0, 1 ) );

   vector< string > list_items;

   split_list_items( list_data.substr( 1 ), list_items );

   string archive_path( get_session_variable(
    get_special_var_name( e_special_var_blockchain_archive_path ) ) );

   date_time dtm( date_time::local( ) );

   map< string, string > mapped_hashes;

   size_t num_seconds;

   session_progress_settings( num_seconds, p_progress );

   for( size_t i = 0; i < list_items.size( ); i++ )
   {
      string next_item( list_items[ i ] );

      string::size_type pos = next_item.find( ' ' );

      if( pos == string::npos )
         throw runtime_error( "unexpected next_item '" + next_item + "' in transform_shared_list_info" );

      string next_hash( next_item.substr( 0, pos ) );
      string next_name( next_item.substr( pos + 1 ) );

      bool is_list = false;
      bool is_repeated = false;
      bool was_peer_mapped = false;

      string encrypted_hash;

      if( mapped_hashes.count( next_hash ) )
      {
         is_repeated = true;
         encrypted_hash = mapped_hashes[ next_hash ];
      }
      else
         encrypted_hash = get_peer_mapped_hash_info( repository, next_hash );

      if( !encrypted_hash.empty( ) )
         was_peer_mapped = !is_repeated;
      else if( is_encrypted_file( next_hash, &is_list ) )
      {
         if( !is_list )
         {
            string file_name( construct_file_name_from_hash(
             next_hash, false, archive_path.empty( ), ( archive_path.empty( ) ? 0 : &archive_path ) ) );

            string encrypted_blob_data( buffer_file( file_name ) );

            encrypted_hash = sha256( encrypted_blob_data ).get_digest_as_string( );
         }
      }

      if( encrypted_hash.empty( ) )
         throw runtime_error( "unexpected encrypted hash for blob not found in transform_shared_list_info" );

      if( was_peer_mapped )
         clear_peer_mapped_hash( repository, next_hash );

      if( !is_repeated )
         mapped_hashes[ next_hash ] = encrypted_hash;

      // NOTE: Initial length of 1 is the type and extra.
      if( encrypted_list_data.length( ) > 2 )
         encrypted_list_data += '\n';

      if( encrypted_hash.empty( ) )
         encrypted_list_data += next_item;
      else
      {
         encrypted_list_data += hex_decode( next_hash );
         encrypted_list_data += hex_decode( encrypted_hash );

         encrypted_list_data += ':' + next_name;
      }

      if( p_progress )
      {
         date_time now( date_time::local( ) );

         uint64_t elapsed = seconds_between( dtm, now );

         if( elapsed >= num_seconds )
         {
            dtm = now;
            p_progress->output_progress( "." );
         }
      }
   }

   file_buffer.copy_from_string( encrypted_list_data, offset );

   return sha256( encrypted_list_data ).get_digest_as_string( );
}

string create_repository_lists(
 const string& repository, session_file_buffer_access& file_buffer,
 size_t offset, size_t length, string* p_encrypted_file_data, progress* p_progress )
{
   string file_data;
   file_buffer.copy_to_string( file_data, offset, length );

   string encrypted_file_data( file_data.substr( 0, 1 ) );
   string public_key_file_data( file_data.substr( 0, 1 ) );

   vector< string > list_items;

   split_list_items( file_data.substr( 1 ), list_items );

   string archive_path( get_session_variable(
    get_special_var_name( e_special_var_blockchain_archive_path ) ) );

   date_time dtm( date_time::local( ) );

   map< string, string > mapped_hashes;

   size_t num_seconds;

   session_progress_settings( num_seconds, p_progress );

   for( size_t i = 0; i < list_items.size( ); i++ )
   {
      string next_item( list_items[ i ] );

      string::size_type pos = next_item.find( ' ' );

      if( pos == string::npos )
         throw runtime_error( "unexpected next_item '" + next_item + "' in create_repository_lists" );

      string next_hash( next_item.substr( 0, pos ) );
      string next_name( next_item.substr( pos + 1 ) );

      bool is_list = false;
      bool is_repeated = false;
      bool was_peer_mapped = false;

      string public_key, encrypted_hash;

      if( mapped_hashes.count( next_hash ) )
      {
         is_repeated = true;
         encrypted_hash = mapped_hashes[ next_hash ];
      }
      else
         encrypted_hash = get_peer_mapped_hash_info( repository, next_hash );
   
      if( !encrypted_hash.empty( ) )
      {
         was_peer_mapped = !is_repeated;

         string::size_type pos = encrypted_hash.find( ':' );
         if( pos == string::npos )
            throw runtime_error( "unexpected encrypted_hash missing public key" );

         public_key = encrypted_hash.substr( pos + 1 );
         encrypted_hash.erase( pos );
      }
      else if( is_encrypted_file( next_hash, &is_list ) )
      {
         if( !is_list )
         {
            string file_name( construct_file_name_from_hash(
             next_hash, false, archive_path.empty( ), ( archive_path.empty( ) ? 0 : &archive_path ) ) );

            string encrypted_blob_data( buffer_file( file_name ) );

            encrypted_hash = sha256( encrypted_blob_data ).get_digest_as_string( );

            if( !has_repository_entry_record( repository, next_hash ) )
               throw runtime_error( "unexpected repository entry for '" + next_hash + "' not found in create_repository_lists" );

            string dummy;
            fetch_repository_entry_record( repository, next_hash, dummy, dummy, public_key );
         }
      }

      if( encrypted_hash.empty( ) )
         throw runtime_error( "unexpected encrypted hash for blob not found in create_repository_lists" );

      if( was_peer_mapped )
         clear_peer_mapped_hash( repository, next_hash );

      if( !is_repeated )
         mapped_hashes[ next_hash ] = encrypted_hash + ':' + public_key;

      // NOTE: Initial length of 1 is the type and extra.
      if( encrypted_file_data.length( ) > 2 )
         encrypted_file_data += '\n';

      if( public_key_file_data.length( ) > 2 )
         public_key_file_data += '\n';

      if( encrypted_hash.empty( ) )
         encrypted_file_data += next_item;
      else
      {
         encrypted_file_data += hex_decode( next_hash );
         encrypted_file_data += hex_decode( encrypted_hash );

         encrypted_file_data += ':' + next_name;
      }

      if( public_key.length( ) < 3 )
         public_key_file_data += next_item;
      else
      {
         public_key_file_data += hex_decode( next_hash );

         string prefix( public_key.substr( 0, 2 ) );
         public_key.erase( 0, 2 );

         public_key_file_data += hex_decode( public_key );

         if( prefix == "02" )
            public_key_file_data += '@';
         else if( prefix == "03" )
            public_key_file_data += '#';
         else
            throw runtime_error( "unexpected public key prefix '" + prefix + "' in create_repository_lists" );

         public_key_file_data += next_name;
      }

      if( p_progress )
      {
         date_time now( date_time::local( ) );

         uint64_t elapsed = seconds_between( dtm, now );

         if( elapsed >= num_seconds )
         {
            dtm = now;
            p_progress->output_progress( "." );
         }
      }
   }

   if( p_encrypted_file_data )
      *p_encrypted_file_data = encrypted_file_data;

   file_buffer.copy_from_string( public_key_file_data, offset );

   return sha256( public_key_file_data ).get_digest_as_string( );
}

string get_archive_status( const string& path )
{
   string retval( c_okay );

   string tmp_file_name( path + "/~" + uuid( ).as_string( ) );

   try
   {
      if( !dir_exists( path ) )
         retval = string( c_file_archive_status_bad_access );
      else
      {
         ofstream outf( tmp_file_name.c_str( ), ios::out );

         if( !outf )
            retval = string( c_file_archive_status_status_bad_create );
         else
         {
            outf << ".";

            outf.flush( );

            if( !outf.good( ) )
               retval = string( c_file_archive_status_bad_write );
         }
      }

      file_remove( tmp_file_name );

      return retval;
   }
   catch( ... )
   {
      file_remove( tmp_file_name );
      throw;
   }
}

bool path_already_used_in_archive( const string& path )
{
   bool retval = false;

   vector< string > paths;

   list_file_archives( e_archive_list_type_minimal, &paths );

   for( size_t i = 0; i < paths.size( ); i++ )
   {
      if( paths[ i ] == path )
      {
         retval = true;
         break;
      }
   }

   return retval;
}

void output_repository_progress( progress* p_progress,
 size_t num, size_t total, date_time& dtm, const date_time& now,
 bool set_session_progress = false, bool* p_has_output_progress = 0, bool is_remove = false )
{
   string progress;

   string extra, value( to_string( num ) );

   if( total )
   {
      if( num > total )
         total = num;

      set_session_variable( get_special_var_name( e_special_var_progress_count ), to_string( num ) );
      set_session_variable( get_special_var_name( e_special_var_progress_total ), to_string( total ) );

      value = get_raw_session_variable( get_special_var_name( e_special_var_progress_value ) );
   }

   extra = get_raw_session_variable( get_special_var_name( e_special_var_blockchain_height_processing ) );

   if( extra.empty( ) )
      extra = get_raw_session_variable( get_special_var_name( e_special_var_blockchain_zenith_height ) );

   if( !extra.empty( ) )
   {
      string other( get_raw_session_variable(
       get_special_var_name( e_special_var_blockchain_height_other ) ) );

      if( !other.empty( ) && ( other != extra ) )
         extra += '/' + other;

      extra = " at height " + extra;
   }

   // FUTURE: These messages should be handled as a server string message.
   if( !is_remove )
      progress = "Tallying" + extra + " - " + value + "...";
   else
      progress = "Trimming" + extra + " - " + value + "...";

   if( set_session_progress )
   {
      set_session_progress_message( progress );

      progress = ".";
   }

   p_progress->output_progress( progress );

   dtm = now;

   if( p_has_output_progress )
      *p_has_output_progress = true;
}

struct repository_lock
{
   repository_lock( const string& repository );
   ~repository_lock( );

   string repo_lock_name;
};

repository_lock::repository_lock( const string& repository )
{
   bool okay = false;

   string repo_lock_prefix( get_special_var_name( e_special_var_repo_lock ) );

   repo_lock_name = ( repo_lock_prefix + '_' + repository );

   string true_value( c_true_value );

   for( size_t i = 0; i < c_max_repo_lock_attempts; i++ )
   {
      okay = set_system_variable( repo_lock_name, true_value, g_empty_string );

      if( okay )
         break;

      msleep( c_repo_lock_attempt_msleep );
   }

   if( !okay )
      throw runtime_error( "unable to obtain repository lock for '" + repository + "'" );
}

repository_lock::~repository_lock( )
{
   set_system_variable( repo_lock_name, "" );
}

}

size_t get_total_files( )
{
   guard g( g_mutex );

   return g_total_files;
}

int64_t get_total_bytes( )
{
   guard g( g_mutex );

   return g_total_bytes;
}

string get_file_stats( )
{
   guard g( g_mutex );

   string s;

   size_t max_num = get_files_area_item_max_num( );
   size_t max_size = get_files_area_item_max_size( );

   int64_t max_bytes = ( int64_t )max_num * ( int64_t )max_size;

   s = '[' + to_string( g_total_files ) + '/' + to_string( max_num )
    + ']' + format_bytes( g_total_bytes ) + '/' + format_bytes( max_bytes );

   s += ":";
   s += to_string( g_hash_tags.size( ) );
   s += " tag(s)";

   return s;
}

mutex& get_mutex_for_ciyam_files( )
{
   return g_mutex;
}

void init_files_area( progress* p_progress, bool remove_invalid_tags )
{
   string cwd( get_cwd( ) );

   string files_area_dir( get_system_variable( get_special_var_name( e_special_var_files_area_dir ) ) );

   set_files_area_dir( files_area_dir );

   // NOTE: If no system variable then setting the directory to an empty string
   // results in it being reset to the default directory (which is now fetched).
   files_area_dir = get_files_area_dir( );

   if( !files_area_dir.empty( ) && files_area_dir[ 0 ] != '/' )
      files_area_dir = cwd + '/' + files_area_dir;

   size_t max_num = get_files_area_item_max_num( );
   size_t max_size = get_files_area_item_max_size( );

   if( !dir_exists( files_area_dir ) )
      create_directories( files_area_dir + "/" );
   else
   {
      date_time dtm( date_time::local( ) );
      date_time dt_epoch( c_time_stamp_unix_epoch );

      directory_filter df;
      fs_iterator dfsi( files_area_dir, &df );

      bool is_first = true;

      int64_t secs_diff = local_secs_diff( );

      string ciyam_prefix( c_ciyam_tag );
      ciyam_prefix += '_';

      size_t num_seconds;

      session_progress_settings( num_seconds, p_progress );

      do
      {
         // NOTE: Skip directories that might be archives.
         if( !is_first && dfsi.get_name( ).length( ) > 2 )
            continue;

         file_filter ff;
         fs_iterator fs( dfsi.get_path_name( ), &ff );

         vector< string > files_to_delete;

         while( fs.has_next( ) )
         {
            if( is_first )
            {
               // NOTE: Need to ignore the application server's own files.
               if( fs.get_name( ).find( ciyam_prefix ) == 0 )
                  continue;

               string data( buffer_file( fs.get_full_name( ) ) );
               string file_name( construct_file_name_from_hash( data, false, false ) );

               if( !file_exists( file_name ) )
               {
                  if( remove_invalid_tags )
                     file_remove( fs.get_full_name( ) );
                  else
                  {
                     // FUTURE: This message should be handled as a server string message.
                     TRACE_LOG( TRACE_ANYTHING, "Skipping unexpected invalid tag file '" + fs.get_name( ) + "'." );
                  }
               }
               else
               {
                  unsigned char file_type_and_extra = '\0';
                  file_size( file_name, &file_type_and_extra, sizeof( file_type_and_extra ) );

                  file_hash_info hash_info( data, file_type_and_extra );

                  g_hash_tags.insert( make_pair( hash_info, fs.get_name( ) ) );
                  g_tag_hashes.insert( make_pair( fs.get_name( ), hash_info ) );
               }
            }
            else
            {
               string file_path( fs.get_full_name( ) );

               if( g_total_files >= max_num )
                  files_to_delete.push_back( file_path );
               else
               {
                  unsigned char file_type_and_extra = '\0';

                  int64_t next_size = file_size( file_path, &file_type_and_extra, sizeof( file_type_and_extra ) );

                  if( next_size > max_size )
                     files_to_delete.push_back( file_path );
                  else
                  {
                     ++g_total_files;

                     g_total_bytes += next_size;

                     string hash( dfsi.get_name( ) + fs.get_name( ) );

                     // NOTE: If the file has no persistent tags then add
                     // a time-stamp tag for it.
                     if( !g_hash_tags.count( hash ) )
                     {
                        date_time dt( last_modification_time( file_path ) );

                        // NOTE: If the "unix epoch" was returned then
                        // will use the current date and time instead.
                        if( dt == dt_epoch )
                           dt = date_time::standard( );

                        dt += ( seconds )secs_diff;

                        string tag_name( c_time_stamp_tag_prefix );

                        tag_name += unique_time_stamp_tag( tag_name, dt );

                        file_hash_info hash_info( hash, file_type_and_extra );

                        g_hash_tags.insert( make_pair( hash_info, tag_name ) );
                        g_tag_hashes.insert( make_pair( tag_name, hash_info ) );
                     }
                  }
               }
            }

            if( p_progress )
            {
               date_time now( date_time::local( ) );

               uint64_t elapsed = seconds_between( dtm, now );

               if( elapsed >= num_seconds )
               {
                  dtm = now;

                  // FUTURE: This message should be handled as a server string message.
                  p_progress->output_progress( "Processed " + to_string( g_total_files ) + " files..." );
               }
            }
         }

         is_first = false;

         for( size_t i = 0; i < files_to_delete.size( ); i++ )
            file_remove( files_to_delete[ i ] );

      } while( dfsi.has_next( ) );
   }

   // NOTE: Create the special "empty" file if it does not already
   // exist (and the files area has enough room for another file).
   if( !has_tag( c_false_value ) && ( g_total_files < max_num ) )
      tag_file( c_false_value, create_empty_file( ), true );
}

void term_files_area( )
{
   // FUTURE: Implementation to be added.
}

void resync_files_area( progress* p_progress, bool remove_invalid_tags )
{
   guard g( g_mutex, "resync_files_area" );

   g_hash_tags.clear( );
   g_tag_hashes.clear( );

   g_total_bytes = g_total_files = 0;

   init_files_area( p_progress, remove_invalid_tags );
}

void init_archive_info( progress* p_progress, bool has_restored_system )
{
   guard g( g_mutex, "init_archive_info" );

   vector< string > paths;

   system_ods_bulk_read ods_bulk_read;

   string all_archives( list_file_archives( e_archive_list_type_minimal, &paths ) );

   if( !all_archives.empty( ) )
   {
      vector< string > archives;

      split( all_archives, archives, '\n' );

      if( paths.size( ) != archives.size( ) )
         throw runtime_error( "unexpected paths.size( ) != archives.size( )" );

      ods_file_system& ods_fs( system_ods_file_system( ) );

      size_t total_files = 0;

      date_time dtm( date_time::local( ) );

      size_t num_seconds;

      session_progress_settings( num_seconds, p_progress );

      for( size_t i = 0; i < archives.size( ); i++ )
      {
         string path( paths[ i ] );
         string archive( archives[ i ] );

         ods_fs.set_root_folder( c_system_archives_folder );

         ods_fs.set_folder( archive );

         int64_t used = 0;

         int64_t avail = 0;
         ods_fs.fetch_from_text_file( c_file_archive_size_avail, avail );

         int64_t limit = 0;
         ods_fs.fetch_from_text_file( c_file_archive_size_limit, limit );

         if( !dir_exists( path ) )
            // FUTURE: This message should be handled as a server string message.
            throw runtime_error( "Archive '" + archive + "' path '" + path + "' is not accessible." );

         // NOTE: Iterate through the files in the path adding all
         // that have names that appear to be valid SHA256 hashes.
         file_filter ff;
         fs_iterator fs( path, &ff );

         regex expr( c_regex_hash_256, true, true );

         g_archive_file_info.insert( make_pair( archive, archive_file_info( ) ) );

         while( fs.has_next( ) )
         {
            string name( fs.get_name( ) );

            ++total_files;

            if( expr.search( name ) == 0 )
            {
               used += file_size( fs.get_full_name( ) );

               int64_t tm_val = last_modification_time( fs.get_full_name( ) );

               g_archive_file_info[ archive ].add_file( fs.get_name( ), tm_val );
            }

            if( p_progress )
            {
               date_time now( date_time::local( ) );

               uint64_t elapsed = seconds_between( dtm, now );

               if( elapsed >= num_seconds )
               {
                  dtm = now;

                  // FUTURE: This message should be handled as a server string message.
                  p_progress->output_progress( "Processed " + to_string( total_files ) + " archive files..." );
               }
            }
         }

         if( used != ( limit - avail ) )
         {
            if( has_restored_system )
               set_system_variable( get_special_var_name(
                e_special_var_queue_archive_info_for_resize ), archive + ',' + to_string( used ) );
            else
               // FUTURE: This message should be handled as a server string message.
               TRACE_LOG( TRACE_ANYTHING, "Archive '" + archive + "' needs to be repaired (incorrect size info)." );
         }
      }
   }
}

void resync_archive_info( progress* p_progress )
{
   g_archive_file_info.clear( );

   init_archive_info( p_progress );
}

string current_time_stamp_tag( bool truncated, size_t days_ahead )
{
   guard g( g_mutex );

   string retval( c_time_stamp_tag_prefix );

   string dummy_time_stamp( get_session_variable( get_special_var_name( e_special_var_dummy_time_stamp ) ) );

   if( !dummy_time_stamp.empty( ) )
   {
      retval += dummy_time_stamp;
      set_session_variable( get_special_var_name( e_special_var_dummy_time_stamp ), "" );
   }
   else
   {
      date_time dt( date_time::local( ) );

      if( days_ahead )
         dt += ( days )days_ahead;

      if( !truncated )
         retval += unique_time_stamp_tag( retval, dt );
      else
         retval += dt.as_string( e_time_format_hhmmssth, false );
   }

   return retval;
}

bool has_tag( const string& name, file_type type )
{
   guard g( g_mutex );

   if( name.empty( ) )
      return false;

   string::size_type pos = name.rfind( '*' );
   map< string, file_hash_info >::iterator i = ( pos == 0 ? g_tag_hashes.end( ) : g_tag_hashes.lower_bound( name.substr( 0, pos ) ) );

   if( i == g_tag_hashes.end( ) || ( pos == string::npos && i->first != name ) )
      return false;
   else
   {
      if( type == e_file_type_any )
         return true;
      else
      {
         string hash( tag_file_hash( name ) );
         string file_name( construct_file_name_from_hash( hash ) );

         unsigned char file_type_and_extra = get_file_type_and_extra( hash, file_name.c_str( ) );

         unsigned char file_type = ( file_type_and_extra & c_file_type_val_mask );

         if( file_type != c_file_type_val_blob && file_type != c_file_type_val_list )
            throw runtime_error( "invalid file type '0x" + hex_encode( &file_type, 1 ) + "' found in has_tag" );

         if( ( type == e_file_type_blob && file_type != c_file_type_val_blob )
          || ( type == e_file_type_list && file_type != c_file_type_val_list ) )
            return false;

         return true;
      }
   }
}

bool has_file( const string& hash, bool check_is_hash, bool* p_is_in_archive )
{
   guard g( g_mutex );

   bool retval = false;

   if( !hash.empty( ) )
   {
      string archive_path( get_session_variable(
       get_special_var_name( e_special_var_blockchain_archive_path ) ) );

      string file_name( construct_file_name_from_hash(
       hash, false, check_is_hash, ( archive_path.empty( ) ? 0 : &archive_path ) ) );

      retval = file_exists( file_name );

      if( p_is_in_archive )
      {
         if( !retval || archive_path.empty( ) )
            *p_is_in_archive = false;
         else
            *p_is_in_archive = ( file_name.find( archive_path ) == 0 );
      }
   }

   return retval;
}

bool is_core_file( const string& hash )
{
   guard g( g_mutex );

   multimap< file_hash_info, string >::iterator i = g_hash_tags.lower_bound( hash );

   string archive_path( get_session_variable(
    get_special_var_name( e_special_var_blockchain_archive_path ) ) );

   string file_name( construct_file_name_from_hash(
    hash, false, false, ( archive_path.empty( ) ? 0 : &archive_path ) ) );

   unsigned char file_type_and_extra = get_file_type_and_extra( hash, file_name.c_str( ) );

   return ( file_type_and_extra & c_file_type_val_extra_core );
}

bool is_list_file( unsigned char ch )
{
   unsigned char file_type = ( ch & c_file_type_val_mask );

   bool is_encrypted = ( ch & c_file_type_val_encrypted );

   return ( !is_encrypted && ( file_type == c_file_type_val_list ) );
}

bool is_list_file( const string& hash, bool* p_is_encrypted )
{
   guard g( g_mutex );

   multimap< file_hash_info, string >::iterator i = g_hash_tags.lower_bound( hash );

   string archive_path( get_session_variable(
    get_special_var_name( e_special_var_blockchain_archive_path ) ) );

   string file_name( construct_file_name_from_hash(
    hash, false, false, ( archive_path.empty( ) ? 0 : &archive_path ) ) );

   unsigned char file_type_and_extra = get_file_type_and_extra( hash, file_name.c_str( ) );

   unsigned char file_type = ( file_type_and_extra & c_file_type_val_mask );

   bool is_encrypted = ( file_type_and_extra & c_file_type_val_encrypted );

   if( p_is_encrypted )
      *p_is_encrypted = is_encrypted;

   return ( !is_encrypted && ( file_type == c_file_type_val_list ) );
}

bool is_encrypted_file( const string& hash, bool* p_is_list )
{
   guard g( g_mutex );

   multimap< file_hash_info, string >::iterator i = g_hash_tags.lower_bound( hash );

   string archive_path( get_session_variable(
    get_special_var_name( e_special_var_blockchain_archive_path ) ) );

   string file_name( construct_file_name_from_hash(
    hash, false, false, ( archive_path.empty( ) ? 0 : &archive_path ) ) );

   unsigned char file_type_and_extra = get_file_type_and_extra( hash, file_name.c_str( ) );

   if( p_is_list )
   {
      unsigned char file_type = ( file_type_and_extra & c_file_type_val_mask );
      *p_is_list = ( file_type == c_file_type_val_list );
   }

   return ( file_type_and_extra & c_file_type_val_encrypted );
}

int64_t file_bytes( const string& hash, bool blobs_for_lists )
{
   guard g( g_mutex );

   string archive_path( get_session_variable(
    get_special_var_name( e_special_var_blockchain_archive_path ) ) );

   string file_name( construct_file_name_from_hash(
    hash, false, false, ( archive_path.empty( ) ? 0 : &archive_path ) ) );

   unsigned char file_type_and_extra = '\0';

   int64_t size = file_size( file_name.c_str( ), &file_type_and_extra, sizeof( file_type_and_extra ) );

   unsigned char file_type = ( file_type_and_extra & c_file_type_val_mask );

   bool is_encrypted = ( file_type_and_extra & c_file_type_val_encrypted );
   bool is_compressed = ( file_type_and_extra & c_file_type_val_compressed );

   if( blobs_for_lists && file_type == c_file_type_val_list )
      size = 0;

   if( !is_encrypted && blobs_for_lists && file_type == c_file_type_val_list )
   {
      string data( buffer_file( file_name ) );

      string increment_special( get_special_var_name( e_special_var_increment ) );
      string buffered_var_name( get_special_var_name( e_special_var_file_info_buffered ) );

      if( !get_session_variable( buffered_var_name ).empty( ) )
         set_session_variable( buffered_var_name, increment_special );

      string final_data( data );

#ifdef ZLIB_SUPPORT
      if( is_compressed && final_data.size( ) > 1 )
      {
         session_file_buffer_access file_buffer;

         unsigned long size = final_data.size( ) - 1;
         unsigned long usize = file_buffer.get_size( ) - size;

         if( uncompress( ( Bytef * )file_buffer.get_buffer( ), &usize, ( Bytef * )&final_data[ 1 ], size ) != Z_OK )
            throw runtime_error( "invalid content for '" + hash + "' (bad compressed or uncompressed too large)" );

         final_data.erase( 1 );
         final_data += string( ( const char* )file_buffer.get_buffer( ), usize );

         validate_hash_with_uncompressed_content( hash, ( unsigned char* )final_data.data( ), final_data.length( ) );
      }
#endif

      if( final_data.size( ) > 1 )
      {
         vector< string > list_items;

         split_list_items( final_data.substr( 1 ), list_items );

         for( size_t i = 0; i < list_items.size( ); i++ )
         {
            string next( list_items[ i ] );
            string::size_type pos = next.find( ' ' );

            size += file_bytes( next.substr( 0, pos ), true );
         }
      }
   }

   return size;
}

string file_type_info( const string& tag_or_hash,
 file_expansion expansion, int max_depth, int indent, bool add_size,
 const char* p_prefix, bool allow_all_after, bool output_total_blob_size,
 progress* p_progress, date_time* p_dtm, size_t* p_total, bool ignore_not_found )
{
   guard g( g_mutex, "file_type_info" );

   string hash, file_name;

   int depth = max_depth;
   bool output_last_only = false;
   bool include_header_suffix = false;

   string header_suffix;
   string use_tag_or_hash( tag_or_hash );
   string files_area_dir( get_files_area_dir( ) );

   size_t num_seconds;

   session_progress_settings( num_seconds, p_progress );

   if( p_dtm )
   {
      date_time now( date_time::local( ) );

      uint64_t elapsed = seconds_between( *p_dtm, now );

      if( !p_progress )
      {
         if( elapsed > 30 )
            // FUTURE: This message should be handled as a server string message.
            throw runtime_error( "Exceeded time limit for processing list items." );
      }
      else if( elapsed >= num_seconds )
      {
         *p_dtm = now;

         if( !p_total )
            p_progress->output_progress( "." );
         else
            // FUTURE: This message should be handled as a server string message.
            p_progress->output_progress( "Processed " + to_string( *p_total ) + " items..." );
      }
   }

   if( p_total )
      ++*p_total;

   if( !use_tag_or_hash.empty( ) )
   {
      string::size_type pos = use_tag_or_hash.find( '!' );

      if( pos != string::npos )
      {
         include_header_suffix = true;

         if( pos == 0 )
            use_tag_or_hash.erase( 0, 1 );
         else
         {
            header_suffix = use_tag_or_hash.substr( 0, pos );
            use_tag_or_hash.erase( 0, pos + 1 );
         }
      }
   }

   if( depth < 0 )
   {
      depth *= -1;
      output_last_only = true;
   }

   bool rc = false;

   string archive_path( get_session_variable(
    get_special_var_name( e_special_var_blockchain_archive_path ) ) );

   hash = tag_file_hash( use_tag_or_hash, &rc );

   if( rc )
      file_name = construct_file_name_from_hash( hash,
       false, false, ( archive_path.empty( ) ? 0 : &archive_path ) );
   else
   {
      bool is_base64 = false;

      if( use_tag_or_hash.length( ) != c_num_digest_characters )
          base64::validate( use_tag_or_hash, &is_base64 );

      hash = !is_base64 ? use_tag_or_hash : base64_to_hex( use_tag_or_hash );

      if( hash.length( ) == c_num_digest_characters )
         file_name = construct_file_name_from_hash( hash,
          false, false, ( archive_path.empty( ) ? 0 : &archive_path ) );
   }

   string archive;
   bool has_been_archived = false;

   if( !file_exists( file_name ) )
      has_been_archived = has_file_been_archived( hash, &archive );

   if( indent && has_been_archived )
      retrieve_file_from_archive( hash, current_time_stamp_tag( ) );

   if( !file_exists( file_name ) && !has_been_archived )
   {
      if( ignore_not_found )
         return string( );

      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "File '" + tag_or_hash + "' was not found." );
   }

   if( !indent && has_been_archived )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "File '" + tag_or_hash + "' is in archive '" + archive + "'." );

   size_t file_size = 0;
   size_t max_to_buffer = 0;

   string prefix;
   if( p_prefix )
      prefix = string( p_prefix );

   bool is_inverted_prefix = false;
   bool is_wildcard_prefix = false;

   string::size_type pos = prefix.find( c_prefix_wildcard_separator );

   // NOTE: If the prefix contains a ':' then if it's the first character it
   // will remove it and treat the remaining as a wildcard prefix but if not
   // the first character it instead handles the supplied as being an indent
   // level separator for splitting up a wildcard expression into parts that
   // are applied as seperate wildcard expressions for each indent level. So
   // if the prefix supplied is 'x:*y*:z*' then only the item 'x' is matched
   // at indent level zero while at indent level one each item that contains
   // a 'y' will be matched and then at indent level two only the items that
   // start with a 'z' will be matched.
   if( pos != string::npos )
   {
      is_wildcard_prefix = true;

      if( pos == 0 )
         prefix.erase( 0, 1 );
      else
      {
         vector< string > parts;
         split( prefix, parts, c_prefix_wildcard_separator );

         prefix.erase( );

         for( size_t i = 0; i < parts.size( ); i++ )
         {
            if( i == indent )
            {
               prefix = parts[ i ];
               break;
            }
            else if( i > indent )
               break;
         }
      }
   }

   // NOTE: If the prefix begins with a caret character
   // then it will be treated inversely (i.e. will only
   // match items that do not match).
   if( !prefix.empty( ) && prefix[ 0 ] == '^' )
   {
      prefix.erase( 0, 1 );
      is_inverted_prefix = true;

      // NOTE: Treat ^^ as a ^ without inverting.
      if( !prefix.empty( ) && prefix[ 0 ] == '^' )
         is_inverted_prefix = false;
   }

   string increment_special( get_special_var_name( e_special_var_increment ) );
   string buffered_var_name( get_special_var_name( e_special_var_file_info_buffered ) );

   // NOTE: If not going to output blob content then to make things faster
   // only read the first byte to get the file type information then later
   // re-read the whole file only if it is a "list".
   if( ( depth == c_depth_to_omit_blob_content )
    || ( expansion == e_file_expansion_recursive_hashes ) )
      max_to_buffer = 1;
   else if( !get_session_variable( buffered_var_name ).empty( ) )
      set_session_variable( buffered_var_name, increment_special );

   string data( buffer_file( file_name, max_to_buffer, &file_size ) );
   
   if( data.empty( ) )
   {
      if( hash == c_ciyam_empty_file_hash )
         data = string( c_file_type_str_blob );
      else
         throw runtime_error( "unexpected empty file '" + use_tag_or_hash + "'" );
   }

   unsigned char file_type = ( data[ 0 ] & c_file_type_val_mask );

   bool is_core = ( data[ 0 ] & c_file_type_val_extra_core );
   bool is_encrypted = ( data[ 0 ] & c_file_type_val_encrypted );
   bool is_compressed = ( data[ 0 ] & c_file_type_val_compressed );

   if( ( file_type != c_file_type_val_blob ) && ( file_type != c_file_type_val_list ) )
      throw runtime_error( "invalid file type '0x" + hex_encode( &file_type, 1 ) + "' found in file_info" );

   if( !is_encrypted && ( max_to_buffer == 1 ) && ( file_type == c_file_type_val_list ) )
   {
      data = buffer_file( file_name );

      if( data.size( ) <= 1 )
         throw runtime_error( "unexpected truncated file content for '" + use_tag_or_hash + "'" );
      else if( !get_session_variable( buffered_var_name ).empty( ) )
         set_session_variable( buffered_var_name, increment_special );
   }

   if( !is_encrypted && !is_compressed && ( ( file_type == c_file_type_val_list )
    || ( ( depth != c_depth_to_omit_blob_content ) && ( expansion != e_file_expansion_recursive_hashes ) ) ) )
   {
      sha256 test_hash( data );

      if( hash != test_hash.get_digest_as_string( ) )
         throw runtime_error( "invalid content for '" + use_tag_or_hash + "' (hash does not match hashed data)" );
   }

   string final_data( data );

#ifdef ZLIB_SUPPORT
   if( !is_encrypted && is_compressed && final_data.size( ) > 1 )
   {
      session_file_buffer_access file_buffer;

      unsigned long size = final_data.size( ) - 1;
      unsigned long usize = file_buffer.get_size( ) - size;

      if( uncompress( ( Bytef * )file_buffer.get_buffer( ), &usize, ( Bytef * )&final_data[ 1 ], size ) != Z_OK )
         throw runtime_error( "invalid content for '" + use_tag_or_hash + "' (bad compressed or uncompressed too large)" );

      final_data.erase( 1 );
      final_data += string( ( const char* )file_buffer.get_buffer( ), usize );

      validate_hash_with_uncompressed_content( hash, ( unsigned char* )final_data.data( ), final_data.length( ) );
   }
#endif

   string retval;

   // NOTE: As other functions rely upon the output format care should be taken to not alter this
   // without the use of additional function arguments (that default to keeping the format as is).
   if( expansion != e_file_expansion_recursive_hashes )
   {
      if( !output_last_only || depth == indent + 1 )
      {
         if( !output_last_only )
            retval += string( indent, ' ' );

         retval += '[';

         if( is_core )
            retval += "core-";

         if( file_type == c_file_type_val_blob )
            retval += "blob]";
         else if( file_type == c_file_type_val_list )
            retval += "list]";
      }
   }

   string size_info( "(" + format_bytes( file_size ) + ")" );

   if( is_encrypted || ( expansion == e_file_expansion_none ) )
   {
      if( !output_last_only || depth == indent + 1 )
      {
         if( expansion == e_file_expansion_recursive_hashes )
            retval += lower( hash );
         else
         {
            retval += " " + lower( hash );

            if( !header_suffix.empty( ) )
               retval += " " + header_suffix;

            if( add_size )
               retval += " " + size_info;

            if( is_encrypted && ( expansion != e_file_expansion_none ) )
               retval += " [***]";

            if( is_core && !is_encrypted )
            {
               string::size_type pos = final_data.find( ':' );

               if( pos != string::npos )
                  retval += " " + final_data.substr( 1, pos - 1 );
            }
         }
      }
   }
   else
   {
      if( file_type == c_file_type_val_blob )
      {
         if( !output_last_only || depth == indent + 1 )
         {
            if( expansion == e_file_expansion_recursive_hashes )
               retval += lower( hash );
            else
            {
               retval += " " + lower( hash );

               if( !header_suffix.empty( ) )
                  retval += " " + header_suffix;

               if( add_size )
                  retval += " " + size_info;

               if( depth && indent >= depth )
               {
                  if( output_last_only )
                     retval += "\n...";
                  else
                     retval += "\n" + string( indent, ' ' ) + "...";
               }
               else if( depth != c_depth_to_omit_blob_content )
               {
                  string blob_info( final_data.substr( 1 ) );

                  if( blob_info.empty( ) )
                     retval += " [n/a]\n";
                  else if( is_valid_utf8( blob_info ) )
                  {
                     retval += " [utf8]";
                     retval += "\n" + utf8_replace( blob_info, "\r", "" );
                  }
                  else
                  {
                     retval += " [base64]";
                     retval += "\n" + base64::encode( blob_info );
                  }
               }
            }
         }
      }
      else if( file_type == c_file_type_val_list )
      {
         vector< string > list_items;

         split_list_items( final_data.substr( 1 ), list_items );

         if( !output_last_only || depth == indent + 1 )
         {
            if( expansion == e_file_expansion_recursive_hashes )
               retval += lower( hash );
            else
            {
               retval += " " + lower( hash );

               if( !header_suffix.empty( ) )
                  retval += " " + header_suffix;

               if( add_size )
                  retval += " " + size_info;
            }
         }

         bool matched_any = false;

         for( size_t i = 0; i < list_items.size( ); i++ )
         {
            string next( list_items[ i ] );
            string::size_type pos = next.find( ' ' );

            string next_hash( next.substr( 0, pos ) );

            string next_name;
            if( pos != string::npos )
               next_name = next.substr( pos + 1 );

            string size, item_num( to_comparable_string( i, false, 6 ) );

            if( output_last_only && depth == indent + 1 )
               size = " (" + format_bytes( file_bytes( next_hash, output_total_blob_size ) ) + ")";

            if( expansion == e_file_expansion_content )
            {
               if( !output_last_only || depth == indent + 1 )
               {
                  if( output_last_only )
                     retval += "\n" + item_num + ' ' + next + size;
                  else
                     retval += "\n" + string( indent, ' ' ) + next;
               }
            }
            else if( depth && indent >= depth
             && expansion != e_file_expansion_recursive_hashes )
            {
               if( i == 0 && ( !output_last_only || depth == indent + 1 ) )
               {
                  if( output_last_only )
                     retval += "\n...";
                  else
                     retval += "\n" + string( indent, ' ' ) + "...";
               }
            }
            else
            {
               if( prefix.empty( )
                || ( is_inverted_prefix && !is_wildcard_prefix && prefix.find( next_name ) != 0 )
                || ( !is_inverted_prefix && !is_wildcard_prefix && prefix.find( next_name ) == 0 )
                || ( is_inverted_prefix && is_wildcard_prefix && !wildcard_match( prefix, next_name ) )
                || ( !is_inverted_prefix && is_wildcard_prefix && wildcard_match( prefix, next_name ) ) )
               {
                  if( p_prefix )
                     matched_any = true;

                  if( ( !output_last_only || depth == indent + 1 )
                   && ( !next_name.empty( ) && expansion != e_file_expansion_recursive_hashes ) )
                  {
                     if( output_last_only )
                        retval += "\n" + item_num + ' ' + next + size;
                     else
                        retval += "\n" + string( indent, ' ' ) + next_name;
                  }

                  bool allow_all = false;

                  // NOTE: If instructed to "allow_all_after" then once an item's name is found to
                  // be equal to or longer in length than the provided prefix then every following
                  // item in the entire branch below this will be output.
                  if( p_prefix && allow_all_after && next_name.length( ) >= strlen( p_prefix ) )
                  {
                     allow_all = true;
                     allow_all_after = false;
                  }

                  string next_tag_or_hash( next_hash );

                  if( include_header_suffix )
                  {
                     if( header_suffix.empty( ) )
                        next_tag_or_hash = next_name + '!' + next_hash;
                     else
                        next_tag_or_hash = header_suffix + ':' + next_name + '!' + next_hash;
                  }

                  string additional( file_type_info(
                   next_tag_or_hash, expansion, max_depth,
                   indent + 1, add_size, ( allow_all ? 0 : p_prefix ), allow_all_after,
                   output_total_blob_size, p_progress, p_dtm, p_total, ignore_not_found ) );

                  if( !additional.empty( ) )
                  {
                     if( !retval.empty( ) )
                        retval += "\n";
                     retval += additional;
                  }
               }
            }
         }

         // NOTE: Special case omits the header for the initial
         // list if there were zero matching items found within.
         if( p_prefix
          && ( expansion != e_file_expansion_recursive_hashes )
          && !matched_any && output_last_only && depth == indent + 1 )
            retval.erase( );
      }
   }

   return retval;
}

void file_list_item_pos(
 const string& repository, const string& tag_or_hash,
 size_t& total, file_total_type total_type, const string& item_hash, size_t& item_pos,
 bool recurse, progress* p_progress, date_time* p_dtm, bool is_hidden, bool is_top_level )
{
   guard g( g_mutex, "file_list_item_pos" );

   string archive;

   string hash( tag_or_hash );

   if( has_tag( tag_or_hash ) )
      hash = tag_file_hash( tag_or_hash );

   system_ods_bulk_read ods_bulk_read;

   bool has_set_missing = false;

   string::size_type wpos = item_hash.find_first_of( "?*" );

   if( is_top_level && !repository.empty( ) )
      set_session_variable(
       get_special_var_name( e_special_var_repo_entry_missing ), "" );

   if( is_list_file( hash ) )
   {
      string all_list_items( extract_file( hash, "" ) );

      vector< string > list_items;

      split_list_items( all_list_items, list_items );

      size_t num_seconds;

      session_progress_settings( num_seconds, p_progress );

      for( size_t i = 0; i < list_items.size( ); i++ )
      {
         if( p_dtm && p_progress )
         {
            date_time now( date_time::local( ) );
            uint64_t elapsed = seconds_between( *p_dtm, now );

            if( elapsed >= num_seconds )
            {
               *p_dtm = now;

               // FUTURE: This message should be handled as a server string message.
               p_progress->output_progress( "Processed " + to_string( total ) + " items..." );
            }
         }

         string next_item( list_items[ i ] );

         if( !next_item.empty( ) )
         {
            if( total_type == e_file_total_type_all_items )
               ++total;

            string::size_type pos = next_item.find( ' ' );

            string next_hash( next_item.substr( 0, pos ) );
            string next_name;

            if( pos != string::npos )
               next_name = next_item.substr( pos + 1 );

            if( !item_pos && !is_hidden )
            {
               if( next_hash == item_hash )
                  item_pos = total;
               else if( wpos != string::npos )
               {
                  if( wildcard_match( item_hash, next_hash ) )
                     item_pos = total;
               }
            }

            // NOTE: If a top-level list item name begins with a '.' then any blobs below it
            // will be excluded as "hidden" if the session variable has been set accordingly.
            if( is_top_level && !get_session_variable(
             get_special_var_name( e_special_var_totals_exclude_hidden_blobs ) ).empty( ) )
            {
               if( !next_name.empty( ) && next_name[ 0 ] == '.' )
                  is_hidden = true;
               else
                  is_hidden = false;
            }

            if( !recurse )
               continue;

            bool is_encrypted = false;
            bool was_hidden_blob = false;

            if( ( total_type == e_file_total_type_repository_entries )
             && has_repository_entry_record( repository, next_hash ) )
            {
               if( !is_hidden )
                  ++total;
            }
            else if( has_file( next_hash ) )
            {
               if( is_list_file( next_hash, &is_encrypted ) )
               {
                  // NOTE: Do not ignore a list hash even if it's in a hidden branch.
                  if( !item_pos && !item_hash.empty( ) )
                  {
                     if( next_hash == item_hash )
                        item_pos = total;
                     else if( wpos != string::npos )
                     {
                        if( wildcard_match( item_hash, next_hash ) )
                           item_pos = total;
                     }
                  }

                  file_list_item_pos( repository, next_hash,
                   total, total_type, item_hash, item_pos, recurse, p_progress, p_dtm, is_hidden, false );
               }
               else if( is_hidden )
                  was_hidden_blob = true;
               else if( total_type != e_file_total_type_all_items )
               {
                  if( total_type == e_file_total_type_blobs_only )
                     ++total;
                  else if( is_encrypted && ( total_type != e_file_total_type_repository_entries ) )
                     ++total;
               }
            }
            else
            {
               if( is_hidden )
                  was_hidden_blob = true;

               if( !has_set_missing
                && ( total_type == e_file_total_type_repository_entries ) )
               {
                  has_set_missing = true;

                  set_session_variable( get_special_var_name(
                   e_special_var_repo_entry_missing ), next_hash, "" );
               }
            }

            // NOTE: The total was already incremented above before
            // it was known whether it was a blob or a list. So now
            // if is a hidden blob will need to decrement the total.
            if( was_hidden_blob && ( total_type == e_file_total_type_all_items ) )
               --total;
         }
      }
   }
}

string create_raw_file( const string& data, bool compress,
 const char* p_tag, bool* p_is_existing, const char* p_hash, bool allow_uncompress, bool allow_missing_items )
{
   guard g( g_mutex, "create_raw_file" );

   if( data.empty( ) )
      throw runtime_error( "cannot create a raw file with empty data" );

   bool file_extra_is_core = false;

   unsigned char type_and_extra = data[ 0 ];

   unsigned char file_type = ( type_and_extra & c_file_type_val_mask );
   unsigned char file_extra = ( type_and_extra & c_file_type_val_extra_mask );

   if( file_extra & c_file_type_val_extra_core )
      file_extra_is_core = true;

   if( file_type != c_file_type_val_blob && file_type != c_file_type_val_list )
      throw runtime_error( "invalid file type '0x" + hex_encode( &file_type, 1 ) + "' for raw file creation" );

   string final_data( data );

   bool is_encrypted = ( type_and_extra & c_file_type_val_encrypted );
   bool is_compressed = ( type_and_extra & c_file_type_val_compressed );

#ifdef ZLIB_SUPPORT
   session_file_buffer_access file_buffer;

   // NOTE: Will attempt to uncompress even if encrypted as a way to detect if a file is not actually encrypted.
   if( is_compressed && allow_uncompress )
   {
      unsigned long size = final_data.size( ) - 1;
      unsigned long usize = file_buffer.get_size( );

      bool okay = true;

      if( uncompress( ( Bytef * )file_buffer.get_buffer( ), &usize, ( Bytef * )&final_data[ 1 ], size ) != Z_OK )
      {
         if( is_encrypted )
            okay = false;
         else
            throw runtime_error( "invalid content for create_raw_file (bad compressed or uncompressed too large)" );
      }

      if( okay )
      {
         is_encrypted = false;
         is_compressed = false;

         type_and_extra &= ~( c_file_type_val_encrypted | c_file_type_val_compressed );

         final_data = ( char )type_and_extra + string( ( const char* )file_buffer.get_buffer( ), usize );
      }
   }
#else
   if( is_compressed )
      throw runtime_error( "create_raw_file doesn't support compressed files (without ZLIB support)" );
#endif

   string hash( p_hash ? string( p_hash ) : sha256( final_data ).get_digest_as_string( ) );

   if( p_hash && !is_encrypted && !is_compressed )
   {
      if( hash != sha256( final_data ).get_digest_as_string( ) )
         throw runtime_error( "incorrect hash for file content was provdided to create_raw_file" );
   }

   string file_name( construct_file_name_from_hash( hash, true ) );

   if( file_type != c_file_type_val_blob )
      validate_list( final_data.substr( 1 ), 0, allow_missing_items );

#ifdef ZLIB_SUPPORT
   if( compress && !is_encrypted && !is_compressed && final_data.size( ) >= c_min_size_to_compress )
   {
      unsigned long size = final_data.size( ) - 1;
      unsigned long csize = file_buffer.get_size( );

      size_t offset = 1;

      int rc = compress2(
       ( Bytef * )file_buffer.get_buffer( ),
       &csize, ( Bytef * )&final_data[ offset ], size, 9 ); // i.e. 9 is for maximum compression

      if( rc == Z_OK )
      {
         if( csize + offset < final_data.size( ) )
         {
            final_data[ 0 ] = ( type_and_extra | c_file_type_val_compressed );

            final_data.erase( offset );
            final_data += string( ( const char* )file_buffer.get_buffer( ), csize );

            is_compressed = true;
         }
      }
      else if( rc != Z_BUF_ERROR )
         throw runtime_error( "unexpected compression error in create_raw_file" );
   }
#endif

   bool was_existing( file_exists( file_name ) );

   if( !was_existing )
   {
      if( p_is_existing )
         *p_is_existing = false;

      if( g_files_constructing.count( file_name ) )
         throw runtime_error( "unexpected file name '" + file_name + "' is currently under construction" );

      if( g_total_files + g_files_constructing.size( ) >= get_files_area_item_max_num( ) )
         // NOTE: First attempt to relegate an existing file in order to make room.
         relegate_one_or_num_oldest_files( "", "", 1, 0, true );

      if( g_total_files + g_files_constructing.size( ) >= get_files_area_item_max_num( ) )
         throw runtime_error( "maximum file area item limit has been reached" );

      size_t max_num = get_files_area_item_max_num( );
      size_t max_size = get_files_area_item_max_size( );

      if( final_data.size( ) > max_size )
         throw runtime_error( "maximum file area item size limit cannot be exceeded" );

      int64_t max_bytes = ( int64_t )max_num * ( int64_t )max_size;

      if( g_total_bytes + final_data.size( ) > max_bytes )
         throw runtime_error( "maximum file area size limit cannot be exceeded" );

      ofstream outf( file_name.c_str( ), ios::out | ios::binary );

      if( !outf )
         throw runtime_error( "unable to create output file '" + file_name + "'" );

      outf << final_data;

      outf.flush( );
      if( !outf.good( ) )
         throw runtime_error( "unexpected bad output stream creating output file '" + file_name + "'" );

      ++g_total_files;

      g_total_bytes += final_data.size( );
   }
   else if( p_is_existing )
      *p_is_existing = true;

   bool skip_tagging = false;

   if( was_existing && !get_session_variable(
    get_special_var_name( e_special_var_skip_tagging_if_exists ) ).empty( ) )
      skip_tagging = true;

   if( !skip_tagging )
   {
      string tag_name;

      if( p_tag )
         tag_name = string( p_tag );

      if( !tag_name.empty( ) )
         tag_file( tag_name, hash );
      else if( !file_extra_is_core )
         tag_file( current_time_stamp_tag( ), hash, true );
   }

   return hash;
}

string create_list_file( const string& add_tags, const string& del_items,
 bool sort_items, const string& tag_or_hash, const string& new_tag, const string& old_tag )
{
   guard g( g_mutex, "create_list_file" );

   string hash( tag_or_hash );
   string data;

   string use_new_tag( new_tag );

   bool allow_new = false;
   bool is_new_list = false;

   if( !use_new_tag.empty( ) && use_new_tag[ 0 ] == '!' )
   {
      allow_new = true;
      use_new_tag.erase( 0, 1 );
   }

   if( has_tag( tag_or_hash ) )
      hash = tag_file_hash( tag_or_hash );

   if( !has_file( hash, false ) )
   {
      if( allow_new )
         is_new_list = true;
      else
         throw runtime_error( "file '" + tag_or_hash + "' not found" );
   }

   if( !is_new_list )
   {
      bool is_list = false;
      data = extract_file( hash, "", '\0', &is_list );

      if( !is_list )
         throw runtime_error( "file '" + tag_or_hash + "' is not a list" );
   }

   vector< string > tags_to_add;
   vector< string > items_to_add;
   vector< string > items_to_remove;

   set< string > hashes_to_add;

   if( !del_items.empty( ) )
      split( del_items, items_to_remove );

   if( !add_tags.empty( ) )
   {
      split( add_tags, tags_to_add );

      for( size_t i = 0; i < tags_to_add.size( ); i++ )
      {
         string next_tag( tags_to_add[ i ] );
         string next_hash;

         // NOTE: If the new item is not expected to actually be a tag then its hash
         // needs to be provided before the item name.
         string::size_type pos = next_tag.find( ' ' );
         if( pos != string::npos )
         {
            next_hash = next_tag.substr( 0, pos );
            next_tag.erase( 0, pos + 1 );
         }

         // NOTE: An added item can take its name from a corresponding delete item by
         // prefixing the tag name with a '?' character.
         string old_tag;
         if( !next_tag.empty( ) && next_tag[ 0 ] == '?' )
         {
            if( i >= items_to_remove.size( ) )
               throw runtime_error( "invalid ? prefixed tag (not enough delete items)" );

            old_tag = items_to_remove[ i ];

            next_tag.erase( 0, 1 );
         }

         if( next_hash.empty( ) && !has_tag( next_tag ) )
            throw runtime_error( "file tag '" + next_tag + "' not found" );

         if( next_hash.empty( ) )
            next_hash = tag_file_hash( next_tag );

         items_to_add.push_back( next_hash + ' ' + ( old_tag.empty( ) ? next_tag : old_tag ) );

         if( sort_items && hashes_to_add.count( next_hash ) )
            throw runtime_error( "attempt to add '" + next_tag + "' more than once" );

         hashes_to_add.insert( next_hash );
      }
   }

   vector< string > items;

   if( !data.empty( ) )
      split_list_items( data, items );

   vector< string > new_items;

   for( size_t i = 0; i < items.size( ); i++ )
   {
      string next( items[ i ] );

      string::size_type pos = next.find( ' ' );

      string next_hash( next.substr( 0, pos ) );
      string next_name;

      if( next_hash == hash )
         throw runtime_error( "invalid attempt to add self to list" );

      if( pos != string::npos )
         next_name = next.substr( pos + 1 );

      if( items_to_remove.empty( ) )
      {
         if( sort_items && hashes_to_add.count( next_hash ) )
            throw runtime_error( "invalid attempt to add existing list item '" + next_hash + "'" );

         new_items.push_back( next );
      }
      else
      {
         bool was_removed = false;

         for( size_t j = 0; j < items_to_remove.size( ); j++ )
         {
            string next_to_remove( items_to_remove[ j ] );

            size_t num = 0;
            bool erase_all = false;

            string::size_type pos = next_to_remove.find( '*' );

            if( pos != string::npos )
            {
               num = from_string< size_t >( next_to_remove.substr( pos + 1 ) );

               if( num == 0 )
                  erase_all = true;

               next_to_remove.erase( pos );
            }

            if( ( next_hash == next_to_remove ) || ( next_name == next_to_remove ) )
            {
               was_removed = true;

               if( !erase_all )
               {
                  if( num <= 1 )
                     items_to_remove.erase( items_to_remove.begin( ) + j );
                  else
                     items_to_remove[ j ] = next_to_remove + "*" + to_string( --num );
               }

               break;
            }
         }

         if( !was_removed )
            new_items.push_back( next );
      }
   }

   for( size_t i = 0; i < items_to_add.size( ); i++ )
      new_items.push_back( items_to_add[ i ] );

   if( new_items.empty( ) )
      throw runtime_error( "invalid empty list" );

   data = string( c_file_type_str_list );

   map< string, string > sorted_items;

   for( size_t i = 0; i < new_items.size( ); i++ )
   {
      if( !sort_items )
      {
         if( i > 0 )
            data += '\n';
         data += new_items[ i ];
      }
      else
      {
         string new_item( new_items[ i ] );

         string::size_type pos = new_item.find( ' ' );

         if( pos == string::npos )
            throw runtime_error( "unexpected missing item name in '" + new_item + "'" );

         sorted_items[ new_item.substr( pos + 1 ) ] = new_item.substr( 0, pos );
      }
   }

   if( sort_items )
   {
      for( map< string, string >::iterator i = sorted_items.begin( ); i != sorted_items.end( ); ++i )
      {
         if( i != sorted_items.begin( ) )
            data += '\n';

         data += ( i->second + ' ' + i->first );
      }
   }

   if( !old_tag.empty( ) )
      tag_file( old_tag, hash );

   return create_raw_file( data, true, use_new_tag.c_str( ) );
}

string create_list_tree( const string& add_tags, const string& del_items,
 bool sort_items, const string& tag_and_branches, const string& new_tag, const string& old_tag )
{
   guard g( g_mutex, "create_list_tree" );

   string retval;

   if( !tag_and_branches.empty( ) )
   {
      bool use_parts = false;

      size_t part = 0;
      size_t length = tag_and_branches.length( );

      if( tag_and_branches[ length - 1 ] == '*' )
      {
         --length;
         use_parts = true;
      }

      if( length )
      {
         vector< string > branches;
         split( tag_and_branches.substr( 0, length ), branches, ':' );

         string hash, name, root;

         vector< string > items;
         deque< string > found_lists;

         for( size_t i = 0; i < branches.size( ); i++ )
         {
            string next_branch( branches[ i ] );

            name = next_branch;

            if( i == 0 )
            {
               if( has_tag( next_branch ) )
                  hash = root = tag_file_hash( next_branch );

               if( hash.empty( ) )
                  continue;
               else
               {
                  bool is_list = false;
                  string data( extract_file( hash, "", '\0', &is_list ) );

                  if( !is_list )
                     throw runtime_error( "file '" + next_branch + "' is not a list" );

                  split_list_items( data, items );

                  found_lists.push_back( hash );
               }
            }
            else if( hash.empty( ) )
               continue;
            else
            {
               hash.erase( );

               for( size_t j = 0; j < items.size( ); j++ )
               {
                  string next_item( items[ j ] );
                  string::size_type pos = next_item.find( ' ' );

                  if( pos != string::npos
                   && ( next_branch == next_item.substr( pos + 1 ) ) )
                  {
                     hash = next_item.substr( 0, pos );
                     found_lists.push_back( hash );
                     break;
                  }
               }

               items.clear( );

               if( hash.empty( ) )
                  continue;
               else
               {
                  bool is_list = false;
                  string data( extract_file( hash, "", '\0', &is_list ) );

                  if( !is_list )
                     throw runtime_error( "file '" + next_branch + "' is not a list" );

                  split_list_items( data, items );
               }
            }
         }

         if( !use_parts )
            branches.pop_back( );

         if( hash.empty( ) )
         {
            if( use_parts )
               name += "." + to_comparable_string( part, false, 5 );

            hash = create_list_file( add_tags, del_items, sort_items, hash, "!", "" );
         }
         else
         {
            if( !use_parts )
               hash = create_list_file( add_tags, del_items, sort_items, hash, "!", "" );
            else
            {
               string last_item( items.back( ) );

               string::size_type pos = last_item.find( ' ' );
               if( pos == string::npos )
                  throw runtime_error( "unexpected list item '" + last_item + "' found in create_list_tree" );

               string item_hash( last_item.substr( 0, pos ) );
               string item_name( last_item.substr( pos + 1 ) );

               if( use_parts )
                  part = from_string< size_t >( item_name.substr( branches.back( ).length( ) + 1 ) );

               bool is_list = false;
               string data( extract_file( item_hash, "", '\0', &is_list ) );

               if( !is_list )
                  throw runtime_error( "file '" + item_hash + "' is not a list" );

               items.clear( );

               split_list_items( data, items );

               vector< string > new_tags;
               split( add_tags, new_tags );

               vector< string > del_item_names;
               split( del_items, del_item_names );

               size_t num_to_be_added = new_tags.size( );

               // NOTE: Check all the existing list items to see how much the list needs to increase in size.
               for( size_t i = 0; i < items.size( ); i++ )
               {
                  string next( items[ i ] );

                  string::size_type pos = next.find( ' ' );

                  if( pos == string::npos )
                     throw runtime_error( "unexpected next list item '" + next + "' found in create_list_tree" );

                  string old_item( next.substr( pos + 1 ) );

                  for( size_t j = 0; j < new_tags.size( ); j++ )
                  {
                     string new_item( new_tags[ j ] );

                     if( !new_item.empty( ) && new_item[ 0 ] == '?' )
                     {
                        if( j >= del_item_names.size( ) )
                           throw runtime_error( "invalid ? prefixed tag (not enough delete items)" );

                        new_item = del_item_names[ j ];
                     }

                     if( new_item == old_item )
                        --num_to_be_added;
                  }
               }

               size_t new_data_size = data.size( ) + 1 + ( last_item.length( ) * num_to_be_added );

               if( new_data_size > get_files_area_item_max_size( ) )
               {
                  item_name = branches.back( )
                   + "." + to_comparable_string( ++part, false, 5 );

                  hash = create_list_file( add_tags, del_items, sort_items, "", "!", "" );
               }
               else
                  hash = create_list_file( add_tags, del_items, sort_items, item_hash, "!", "" );

               name = item_name;
            }
         }

         while( found_lists.size( ) > branches.size( ) )
            found_lists.pop_back( );

         for( size_t i = branches.size( ); i > 0; i-- )
         {
            size_t offset = i - 1;

            string new_item( hash + ' ' + name );
            string list_hash;

            if( i == found_lists.size( ) )
            {
               list_hash = found_lists[ offset ];
               found_lists.pop_back( );
            }

            if( offset > 0 )
               hash = create_list_file( new_item, name, sort_items, list_hash, "!", "" );
            else
               retval = create_list_file( new_item, name, sort_items, root, new_tag, old_tag );

            name = branches[ offset ];
         }
      }
   }

   return retval;
}

void tag_del( const string& name, bool unlink, bool auto_tag_with_time )
{
   guard g( g_mutex );

   string::size_type pos = name.find_first_of( "*?" );

   string empty_file_tag( c_false_value );

   if( pos == string::npos )
   {
      if( name == empty_file_tag )
         throw runtime_error( "invalid attempt to remove the empty file tag" );

      if( name.find( c_time_stamp_tag_prefix ) != 0 )
         file_remove( get_files_area_dir( ) + '/' + name );

      if( g_tag_hashes.count( name ) )
      {
         file_hash_info hash_info = g_tag_hashes[ name ];
         g_tag_hashes.erase( name );

         // NOTE: Need to also remove the matching entry in the hash tags multimap.
         multimap< file_hash_info, string >::iterator i;

         for( i = g_hash_tags.lower_bound( hash_info ); i != g_hash_tags.end( ); ++i )
         {
            if( i->first != hash_info )
               break;

            if( i->second == name )
            {
               g_hash_tags.erase( i );
               break;
            }
         }

         if( unlink && !g_hash_tags.count( hash_info ) )
            delete_file( hash_info.get_hash_string( ) );
         else if( auto_tag_with_time && !g_hash_tags.count( hash_info ) )
            tag_file( current_time_stamp_tag( ), hash_info.get_hash_string( ), true );
      }
   }
   else
   {
      if( name == "*" )
         throw runtime_error( "invalid attempt to delete all file system tags (use ** if really wanting to do this)" );

      string prefix( name.substr( 0, pos ) );
      map< string, file_hash_info >::iterator i = g_tag_hashes.lower_bound( prefix );

      vector< string > matching_tag_names;

      for( ; i != g_tag_hashes.end( ); ++i )
      {
         if( wildcard_match( name, i->first ) )
            matching_tag_names.push_back( i->first );

         if( i->first.length( ) < prefix.length( ) || i->first.substr( 0, prefix.length( ) ) != prefix )
            break;
      }

      for( size_t i = 0; i < matching_tag_names.size( ); i++ )
      {
         string next_tag_name( matching_tag_names[ i ] );

         if( next_tag_name != empty_file_tag )
            tag_del( next_tag_name, unlink, auto_tag_with_time );
      }
   }
}

void tag_file( const string& name, const string& hash, bool skip_tag_del, bool is_external )
{
   guard g( g_mutex );

   if( is_external && name.find( c_time_stamp_tag_prefix ) == 0 )
      throw runtime_error( "invalid file time-stamp prefixed tag name '" + name + "'" );

   string ciyam_prefix( c_ciyam_tag );
   ciyam_prefix += '_';

   if( is_external && name.find( ciyam_prefix ) == 0 )
      throw runtime_error( "invalid '" + ciyam_prefix + "' prefixed tag name '" + name + "'" );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty tag name found in tag_file" );
   else
   {
      string file_name( construct_file_name_from_hash( hash ) );

      if( !file_exists( file_name ) )
         // FUTURE: This message should be handled as a server string message.
         throw runtime_error( "File '" + hash + "' was not found." );

      string tag_name;

      // NOTE: If an asterisk is included in the name then existing tags matching
      // the wildcard expression will be removed first then if the asterisk is at
      // the very end no new tag will be added unless two asterisks were used for
      // the name suffix in which case the new tag name will become the truncated
      // version.
      string::size_type pos = name.rfind( '*' );

      if( pos == string::npos )
      {
         if( name != valid_file_name( name ) )
            // FUTURE: This message should be handled as a server string message.
            throw runtime_error( "Invalid file tag name '" + name + "'." );

         if( !skip_tag_del )
            tag_del( name );

         tag_name = name;
      }
      else
      {
         // NOTE: If a question mark preceeds the asterisk then only the exact tag
         // will be removed.
         if( pos > 1 && name[ pos - 1 ] == '?' )
            remove_file_tags( hash, name.substr( 0, pos - 1 ) );
         else
            remove_file_tags( hash, name.substr( 0, pos + 1 ) );

         if( pos != name.length( ) - 1 )
            tag_name = name.substr( 0, pos ) + name.substr( pos + 1 );
         else if( pos > 1 && name[ pos - 1 ] == '*' )
            tag_name = name.substr( 0, pos - 1 );

         // NOTE: If a question mark as found at the end then the tag will become
         // instead a "current time-stamp" tag.
         if( tag_name.length( ) && tag_name[ tag_name.length( ) - 1 ] == '?' )
            tag_name = current_time_stamp_tag( );
      }

      if( !tag_name.empty( ) )
      {
         string all_tags( get_hash_tags( hash ) );

         string ts_tag_to_remove;
         vector< string > tags;

         // NOTE: If the file has just a time-stamp tag then this will be removed.
         if( !all_tags.empty( ) )
         {
            split( all_tags, tags, '\n' );

            if( tags.size( ) == 1 && tags[ 0 ].find( c_time_stamp_tag_prefix ) == 0 )
               ts_tag_to_remove = tags[ 0 ];
         }

         bool insert_tag = true;

         string prefix( get_files_area_dir( ) );

         // NOTE: If the tag name is time-stamp prefixed then will "touch" the
         // file so it will have the same tag prefix (to the same second) when
         // the files area is initialised or resynchronised.
         if( tag_name.find( c_time_stamp_tag_prefix ) == 0 )
         {
            if( tags.size( ) && ts_tag_to_remove.empty( ) )
               insert_tag = false;
            else
            {
               date_time dtm( tag_name.substr(
                strlen( c_time_stamp_tag_prefix ), c_yyyymmmddhhmmss_len ) );

               time_t tm;
               unix_time( dtm, &tm );

               file_touch( file_name, &tm );
            }
         }
         else
         {
            string tag_file_name( prefix + "/" + tag_name );

            ofstream outf( tag_file_name.c_str( ) );
            if( !outf )
               throw runtime_error( "unable to open file '" + tag_file_name + "' for output" );

            outf << hash;

            outf.flush( );
            if( !outf.good( ) )
               throw runtime_error( "unexpected bad output stream" );
         }

         // NOTE: Do not add a time-stamp prefixed tag if the
         // file has any existing persistent tags.
         if( insert_tag )
         {
            unsigned char file_type_and_extra = get_file_type_and_extra( hash, file_name.c_str( ) );

            file_hash_info hash_info( hash, file_type_and_extra );

            g_hash_tags.insert( make_pair( hash_info, tag_name ) );
            g_tag_hashes.insert( make_pair( tag_name, hash_info ) );
         }

         if( !ts_tag_to_remove.empty( ) )
            tag_del( ts_tag_to_remove, false, false );
      }
   }
}

void touch_file( const string& hash,
 const string& archive, bool set_archive_path, bool* p_has_updated_archive )
{
   guard g( g_mutex );

   string old_archive_path;

   if( set_archive_path )
   {
      string archive_path;

      old_archive_path = get_session_variable(
       get_special_var_name( e_special_var_blockchain_archive_path ) );

      if( has_file_archive( archive, &archive_path ) )
      {
         set_session_variable( get_special_var_name(
          e_special_var_blockchain_archive_path ), archive_path );
      }
   }

   bool is_in_archive = false;
   bool updated_archive = false;

   if( has_file( hash, false, &is_in_archive ) )
   {
      if( is_in_archive )
         updated_archive = touch_file_in_archive( hash, archive );
      else
      {
         string all_tags( get_hash_tags( hash ) );

         if( !all_tags.empty( ) )
         {
            vector< string > tags;

            split( all_tags, tags, '\n' );

            if( tags.size( ) == 1 && tags[ 0 ].find( c_time_stamp_tag_prefix ) == 0 )
               tag_file( current_time_stamp_tag( ), hash, true );
         }
      }
   }

   if( updated_archive && p_has_updated_archive )
      *p_has_updated_archive = true;

   if( set_archive_path )
      set_session_variable( get_special_var_name(
       e_special_var_blockchain_archive_path ), old_archive_path );
}

string get_hash( const string& prefix )
{
   guard g( g_mutex );

   string retval;

   string::size_type pos = prefix.find_first_of( "?*" );

   multimap< file_hash_info, string >::iterator i = g_hash_tags.lower_bound( prefix.substr( 0, pos ) );

   while( i != g_hash_tags.end( ) )
   {
      string next_hash( i->first.get_hash_string( ) );

      if( pos == string::npos )
      {
         if( next_hash.find( prefix ) == 0 )
            retval = next_hash;
         break;
      }
      else
      {
         if( wildcard_match( prefix, next_hash ) )
         {
            if( !retval.empty( ) )
               retval += '\n';
            retval += next_hash;

            ++i;
         }
         else
            break;
      }
   }

   return retval;
}

bool has_any_tags( const string& hash )
{
   guard g( g_mutex );

   bool retval = false;

   multimap< file_hash_info, string >::iterator i = g_hash_tags.lower_bound( hash );

   if( ( i != g_hash_tags.end( ) ) && ( i->first.get_hash_string( ) == hash ) )
      retval = true;

   return retval;
}

string get_hash_tags( const string& hash )
{
   guard g( g_mutex );

   string retval;
   set< string > tags_found;

   multimap< file_hash_info, string >::iterator i = g_hash_tags.lower_bound( hash );

   for( ; i != g_hash_tags.end( ); ++i )
   {
      if( i->first.get_hash_string( ) != hash )
         break;

      tags_found.insert( i->second );
   }

   for( set< string >::iterator si = tags_found.begin( ); si != tags_found.end( ); ++si )
   {
      if( !retval.empty( ) )
         retval += '\n';

      retval += *si;
   }

   return retval;
}

string tag_file_hash( const string& name, bool* p_rc )
{
   guard g( g_mutex );

   string retval;

   // NOTE: If the name is just "*" then return the hashes of all files that have been tagged or
   // if is just "?" then will instead return all the hashes of files that have not been tagged.
   if( name == "*" )
   {
      set< string > hashes;

      for( map< string, file_hash_info >::iterator i = g_tag_hashes.begin( ); i != g_tag_hashes.end( ); ++i )
         hashes.insert( i->second.get_hash_string( ) );

      for( set< string >::iterator i = hashes.begin( ); i != hashes.end( ); ++i )
      {
         if( i != hashes.begin( ) )
            retval += '\n';
         retval += *i;
      }
   }
   else
   {
      string::size_type pos = name.rfind( '*' );
      map< string, file_hash_info >::iterator i = g_tag_hashes.lower_bound( name.substr( 0, pos ) );

      if( i == g_tag_hashes.end( ) || ( pos == string::npos && i->first != name ) )
      {
         if( p_rc )
         {
            *p_rc = false;
            return retval;
         }

         // FUTURE: This message should be handled as a server string message.
         throw runtime_error( "Tag '" + name + "' not found." );
      }

      if( p_rc )
         *p_rc = true;

      retval = i->second.get_hash_string( );
   }

   return retval;
}

string extract_tags_from_lists( const string& tag_or_hash,
 const string& prefix, int depth, int level, progress* p_progress, date_time* p_dtm, size_t* p_total )
{
   guard g( g_mutex );

   string retval;

   string hash( tag_or_hash );

   if( has_tag( tag_or_hash ) )
      hash = tag_file_hash( tag_or_hash );

   if( !has_file( hash ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "File '" + tag_or_hash + "' not found." );

   if( depth == 0 )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Depth must not be equal to zero." );

   bool max_depth_only = false;

   int depth_val = depth;

   if( depth_val < 0 )
   {
      depth_val *= -1;
      max_depth_only = true;
   }

   bool is_list = false;
   string data( extract_file( hash, "", '\0', &is_list ) );

   if( !is_list && !level )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "File '" + tag_or_hash + "' is not a list." );

   size_t num_seconds;

   session_progress_settings( num_seconds, p_progress );

   if( is_list && !data.empty( ) )
   {
      vector< string > list_items;

      split_list_items( data, list_items );

      for( size_t i = 0; i < list_items.size( ); i++ )
      {
         if( p_total )
            ++*p_total;

         if( p_dtm && p_progress )
         {
            date_time now( date_time::local( ) );
            uint64_t elapsed = seconds_between( *p_dtm, now );

            if( elapsed >= num_seconds )
            {
               *p_dtm = now;

               if( !p_total )
                  p_progress->output_progress( "." );
               else
                  // FUTURE: This message should be handled as a server string message.
                  p_progress->output_progress( "Processed " + to_string( *p_total ) + " items..." );
            }
         }

         string next_item( list_items[ i ] );

         string::size_type pos = next_item.find( ' ' );

         if( pos == string::npos )
            throw runtime_error( "unexpected invalid list item format '" + next_item + "'" );

         string next_hash( next_item.substr( 0, pos ) );
         string next_name( next_item.substr( pos + 1 ) );

         string check_prefix( prefix );
         if( check_prefix.length( ) > next_name.size( ) )
            check_prefix.erase( next_name.size( ) );

         if( !prefix.empty( ) && next_name.find( check_prefix ) != 0 )
            continue;

         if( !max_depth_only || level + 1 == depth_val )
         {
            tag_file( next_name, next_hash );

            if( !retval.empty( ) )
               retval += '\n';
            retval += next_name;
         }

         if( level + 1 < depth_val )
         {
            string extra( extract_tags_from_lists(
             next_hash, prefix, depth, level + 1, p_progress, p_dtm, p_total ) );

            if( !extra.empty( ) )
            {
               if( !retval.empty( ) )
                  retval += '\n';
               retval += extra;
            }
         }
      }
   }

   return retval;
}

string list_file_tags(
 const string& pat, const char* p_excludes, size_t max_tags,
 int64_t max_bytes, int64_t* p_min_bytes, deque< string >* p_hashes,
 bool include_multiples, progress* p_progress, date_time* p_dtm )
{
   guard g( g_mutex );

   string retval;

   size_t pcount = 0;
   size_t num_tags = 0;
   int64_t min_bytes = 0;
   int64_t num_bytes = 0;

   date_time dtm( date_time::local( ) );

   string all_excludes( p_excludes ? p_excludes : "" );
   vector< string > excludes;

   if( pat.empty( ) )
      excludes.push_back( string( c_time_stamp_tag_prefix ) + "*" );

   if( !all_excludes.empty( ) )
      split( all_excludes, excludes );

   size_t num_seconds;

   session_progress_settings( num_seconds, p_progress );

   if( !pat.empty( ) )
   {
      string::size_type pos = pat.find_first_of( "*?" );
      string prefix = pat.substr( 0, pos );

      map< string, file_hash_info >::iterator i = g_tag_hashes.lower_bound( prefix );

      for( ; i != g_tag_hashes.end( ); ++i )
      {
         ++pcount;

         if( p_progress )
         {
            date_time now( date_time::local( ) );

            uint64_t elapsed = seconds_between( p_dtm ? *p_dtm : dtm, now );

            if( elapsed >= num_seconds )
            {
               if( !p_dtm )
                  dtm = now;
               else
                  *p_dtm = now;

               // FUTURE: This message should be handled as a server string message.
               p_progress->output_progress( "Processed " + to_string( pcount ) + " tags..." );
            }
         }

         if( wildcard_match( pat, i->first ) )
         {
            bool is_excluded = false;

            for( size_t j = 0; j < excludes.size( ); j++ )
            {
               if( wildcard_match( excludes[ j ], i->first ) )
               {
                  is_excluded = true;
                  break;
               }
            }

            if( is_excluded )
               continue;

            // NOTE: Skip matching tags for files that have more than one tag.
            if( !include_multiples )
            {
               multimap< file_hash_info, string >::iterator j = g_hash_tags.lower_bound( i->second );

               if( j != g_hash_tags.end( ) && ++j != g_hash_tags.end( ) )
               {
                  if( j->first == i->second )
                     continue;
               }
            }

            int64_t next_bytes = file_bytes( i->second.get_hash_string( ) );

            if( max_bytes && ( num_bytes + next_bytes > max_bytes ) )
               continue;

            ++num_tags;

            if( !min_bytes || ( next_bytes < min_bytes ) )
               min_bytes = next_bytes;

            num_bytes += next_bytes;

            if( !retval.empty( ) )
               retval += "\n";
            retval += i->first;

            if( p_hashes )
               p_hashes->push_back( i->second.get_hash_string( ) );
         }

         if( max_tags && num_tags >= max_tags )
            break;

         if( i->first.length( ) < prefix.length( ) || i->first.substr( 0, prefix.length( ) ) != prefix )
            break;
      }
   }
   else
   {
      for( map< string, file_hash_info >::iterator i = g_tag_hashes.begin( ); i != g_tag_hashes.end( ); ++i )
      {
         ++pcount;

         date_time now( date_time::local( ) );

         uint64_t elapsed = seconds_between( dtm, now );

         if( elapsed >= num_seconds )
         {
            dtm = now;

            // FUTURE: This message should be handled as a server string message.
            p_progress->output_progress( "Processed " + to_string( pcount ) + " tags..." );
         }

         bool is_excluded = false;

         for( size_t j = 0; j < excludes.size( ); j++ )
         {
            if( wildcard_match( excludes[ j ], i->first ) )
            {
               is_excluded = true;
               break;
            }
         }

         // NOTE: Skip matching tags for files that have more than one tag.
         if( !include_multiples )
         {
            multimap< file_hash_info, string >::iterator j = g_hash_tags.lower_bound( i->second );

            if( j != g_hash_tags.end( ) && ++j != g_hash_tags.end( ) )
            {
               if( j->first == i->second )
                  is_excluded = true;
            }
         }

         if( is_excluded )
            continue;

         int64_t next_bytes = file_bytes( i->second.get_hash_string( ) );

         if( max_bytes && num_bytes + next_bytes > max_bytes )
            break;

         ++num_tags;

         if( !min_bytes || next_bytes < min_bytes )
            min_bytes = next_bytes;

         num_bytes += next_bytes;

         if( !retval.empty( ) )
            retval += "\n";
         retval += i->first;

         if( p_hashes )
            p_hashes->push_back( i->second.get_hash_string( ) );

         if( max_tags && num_tags >= max_tags )
            break;
      }
   }

   return retval;
}

void remove_file_tags( const string& hash, const string& pat )
{
   string tags( get_hash_tags( hash ) );

   if( !tags.empty( ) )
   {
      vector< string > all_tags;
      split( tags, all_tags, '\n' );

      for( size_t i = 0; i < all_tags.size( ); i++ )
      {
         if( wildcard_match( pat, all_tags[ i ] ) )
            tag_del( all_tags[ i ] );
      }
   }
}

string hash_with_nonce( const string& hash, const string& nonce )
{
   string file_name( construct_file_name_from_hash( hash ) );

   sha256 temp_hash;

   if( !nonce.empty( ) )
      temp_hash.update( nonce );
   temp_hash.update( file_name, true );

   return temp_hash.get_digest_as_string( );
}

void crypt_file( const string& repository,
 const string& tag_or_hash, const string& password, bool recurse,
 crypt_target target, progress* p_progress, date_time* p_dtm, size_t* p_total,
 crypt_operation operation, set< string >* p_files_processed, stream_cipher cipher )
{
   guard g( g_mutex, "crypt_file" );

   string hash( tag_or_hash );

   if( has_tag( tag_or_hash ) )
      hash = tag_file_hash( tag_or_hash );

   string archive_path( get_session_variable(
    get_special_var_name( e_special_var_blockchain_archive_path ) ) );

   string file_name( construct_file_name_from_hash(
    hash, false, false, ( archive_path.empty( ) ? 0 : &archive_path ) ) );

   if( !file_exists( file_name ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( hash + " was not found." );

   bool read_from_file = false;

   unsigned char type_and_extra = get_file_type_and_extra( hash, file_name.c_str( ), &read_from_file );

   unsigned char file_type = ( type_and_extra & c_file_type_val_mask );

   bool is_encrypted = ( type_and_extra & c_file_type_val_encrypted );
   bool is_compressed = ( type_and_extra & c_file_type_val_compressed );
   bool is_no_encrypt = ( type_and_extra & c_file_type_val_no_encrypt );

   if( is_no_encrypt )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Attempt to encrypt file flagged with 'no encrypt'." );

   if( recurse && ( target == e_crypt_target_all ) && ( operation == e_crypt_operation_recrypt ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Attempt to recrypt recursively when not 'blobs only'." );

   size_t num_seconds;

   session_progress_settings( num_seconds, p_progress );

   auto_ptr< set< string > > ap_files_processed;

   // NOTE: Use the file content hash as salt.
   combined_clear_key password_and_hash( password, hash );

   if( !p_files_processed )
   {
      ap_files_processed.reset( new set< string >( ) );
      p_files_processed = ap_files_processed.get( );
   }

   p_files_processed->insert( hash );

   if( p_total )
      ++*p_total;

   if( p_dtm && p_progress )
   {
      date_time now( date_time::local( ) );
      uint64_t elapsed = seconds_between( *p_dtm, now );

      if( elapsed >= num_seconds )
      {
         *p_dtm = now;

         if( !p_total
          || ( operation == e_crypt_operation_decrypt )
          || ( operation == e_crypt_operation_encrypt ) )
            p_progress->output_progress( "." );
         else
            // FUTURE: This message should be handled as a server string message.
            p_progress->output_progress( "Processed " + to_string( *p_total ) + " files..." );
      }
   }

   string file_data;

   if( !is_encrypted )
   {
      file_data = buffer_file( file_name );

      string uncompressed_data( file_data );

#ifdef ZLIB_SUPPORT
      if( is_compressed && file_data.size( ) > 1 )
      {
         session_file_buffer_access file_buffer;

         unsigned long size = file_data.size( ) - 1;
         unsigned long usize = file_buffer.get_size( ) - size;

         if( uncompress( ( Bytef * )file_buffer.get_buffer( ), &usize, ( Bytef * )&file_data[ 1 ], size ) != Z_OK )
            throw runtime_error( "invalid content for '" + tag_or_hash + "' (bad compressed or uncompressed too large)" );

         uncompressed_data = file_data.substr( 0, 1 );
         uncompressed_data += string( ( const char* )file_buffer.get_buffer( ), usize );
      }
#endif

      if( ( operation != e_crypt_operation_decrypt )
       && ( ( target == e_crypt_target_all ) || ( file_type == c_file_type_val_blob ) ) )
      {
         if( operation == e_crypt_operation_recrypt )
            // FUTURE: This message should be handled as a server string message.
            throw runtime_error( "Cannot recrypt the unencrypted file '" + tag_or_hash + "'." );

         stringstream ss( file_data.substr( 1 ) );

         crypt_stream( ss, password_and_hash, cipher );

         string new_file_data( file_data.substr( 0, 1 ) );

         new_file_data += ss.str( );

         new_file_data[ 0 ] |= c_file_type_val_encrypted;

         write_file( file_name, new_file_data );

         if( !read_from_file )
            set_file_type_and_extra( hash, ( unsigned char )new_file_data[ 0 ] );
      }

      if( ( file_type == c_file_type_val_blob )
       && ( target == e_crypt_target_blobs_only_repo ) )
      {
         if( !has_repository_entry_record( repository, hash ) )
         {
            string pub_key;
            create_peer_repository_entry_push_info( hash, password, &pub_key, false );

            store_repository_entry_record( repository, hash, "", pub_key, pub_key );
         }
      }

      if( recurse && file_type == c_file_type_val_list )
      {
         string list_info( uncompressed_data.substr( 1 ) );

         if( !list_info.empty( ) )
         {
            vector< string > list_items;

            split_list_items( list_info, list_items );

            for( size_t i = 0; i < list_items.size( ); i++ )
            {
               string next( list_items[ i ] );
               string::size_type pos = next.find( ' ' );

               if( pos != string::npos )
                  next.erase( pos );

               if( !p_files_processed->count( next ) )
                  crypt_file( repository, next, password, recurse, target,
                   p_progress, p_dtm, p_total, operation, p_files_processed, cipher );
            }
         }
      }
   }
   else
   {
      if( recurse && ( file_type == c_file_type_val_list ) && ( operation == e_crypt_operation_recrypt ) )
         // FUTURE: This message should be handled as a server string message.
         throw runtime_error( "Invalid recrypt attempted on encrypted list file '" + tag_or_hash + "'" );

      string uncompressed_data;

      if( ( file_type == c_file_type_val_list ) || ( operation != e_crypt_operation_encrypt ) )
      {
         file_data = buffer_file( file_name );

         stringstream ss( file_data.substr( 1 ) );

         crypt_stream( ss, password_and_hash, cipher );

         file_data.erase( 1 );
         file_data += ss.str( );

         file_data[ 0 ] &= ~c_file_type_val_encrypted;

         uncompressed_data = file_data;

         // FUTURE: This message should be handled as a server string message.
         string bad_hash_error( "Invalid cipher/password to decrypt file '" + tag_or_hash + "'." );

         bool recrypt = ( operation == e_crypt_operation_recrypt );

         try
         {
#ifdef ZLIB_SUPPORT
            if( ( is_compressed && file_data.size( ) > 1 ) )
            {
               session_file_buffer_access file_buffer;

               unsigned long size = file_data.size( ) - 1;
               unsigned long usize = file_buffer.get_size( ) - size;

               if( uncompress( ( Bytef * )file_buffer.get_buffer( ), &usize, ( Bytef * )&file_data[ 1 ], size ) != Z_OK )
                  throw runtime_error( bad_hash_error ); // NOTE: Assume it is a bad password rather than a bad uncompress.

               uncompressed_data = file_data.substr( 0, 1 );
               uncompressed_data += string( ( const char* )file_buffer.get_buffer( ), usize );

               validate_hash_with_uncompressed_content( hash,
                ( unsigned char* )uncompressed_data.data( ), uncompressed_data.length( ), bad_hash_error.c_str( ) );
            }
#endif

            if( !is_compressed )
               validate_hash_with_uncompressed_content( hash,
                ( unsigned char* )file_data.data( ), file_data.length( ), bad_hash_error.c_str( ) );

            if( operation == e_crypt_operation_recrypt )
            {
               recrypt = false;
               throw runtime_error( "unexpected recrypt has decrypted (same password as when encrypted?)" );
            }
         }
         catch( ... )
         {
            if( !recrypt )
               throw;

            // NOTE: Rather than throw the encryption bit is set back on for a recrypt.
            file_data[ 0 ] |= c_file_type_val_encrypted;
         }

         write_file( file_name, file_data );

         if( !read_from_file )
            set_file_type_and_extra( hash, ( unsigned char )file_data[ 0 ] );
      }

      if( recurse && file_type == c_file_type_val_list )
      {
         string list_info( uncompressed_data.substr( 1 ) );

         if( !list_info.empty( ) )
         {
            vector< string > list_items;

            split_list_items( list_info, list_items );

            for( size_t i = 0; i < list_items.size( ); i++ )
            {
               string next( list_items[ i ] );
               string::size_type pos = next.find( ' ' );

               if( pos != string::npos )
                  next.erase( pos );

               if( !p_files_processed->count( next ) )
                  crypt_file( repository, next, password, recurse, target,
                   p_progress, p_dtm, p_total, operation, p_files_processed, cipher );
            }
         }
      }
   }
}

void fetch_file( const string& hash, tcp_socket& socket, progress* p_sock_progress, unsigned char* p_file_type )
{
   string archive_path( get_session_variable(
    get_special_var_name( e_special_var_blockchain_archive_path ) ) );

   string file_name( construct_file_name_from_hash(
    hash, false, false, ( archive_path.empty( ) ? 0 : &archive_path ) ) );

   string extra_header;

   session_file_buffer_access file_buffer;

   // NOTE: As the file could end up being deleted whilst it is
   // being transferred it is copied to the session file buffer.
   if( !file_name.empty( ) )
   {
      guard g( g_mutex, "fetch_file" );

      if( !file_exists( file_name ) )
         // FUTURE: This message should be handled as a server string message.
         throw runtime_error( "File '" + hash + "' was not found." );

      string content( buffer_file( file_name ) );

      if( hash == c_ciyam_empty_file_hash )
         content = string( c_file_type_str_blob );

      unsigned char file_type = ( content[ 0 ] & c_file_type_val_mask );
      unsigned char file_extra = ( content[ 0 ] & c_file_type_val_extra_mask );

      if( p_file_type )
         *p_file_type = file_type;

      bool is_encrypted = ( content[ 0 ] & c_file_type_val_encrypted );

      bool is_encrypted_password = false;

      string crypt_password( get_session_variable(
       get_special_var_name( e_special_var_repo_crypt_password ) ) );

      if( crypt_password.empty( ) )
      {
         crypt_password = get_session_variable(
          get_special_var_name( e_special_var_encrypted_password ) );

         if( !crypt_password.empty( ) )
            is_encrypted_password = true;
      }

      if( is_encrypted && !crypt_password.empty( ) )
      {
         if( is_encrypted_password )
            decrypt_data( crypt_password, crypt_password );
         else if( crypt_password == get_special_var_name( e_special_var_sid ) )
            get_identity( crypt_password, false, true );

         string stream_cipher( get_session_variable(
          get_special_var_name( e_special_var_stream_cipher ) ) );

         stringstream ss( content.substr( 1 ) );

         // NOTE: Use the file content hash as salt.
         crypt_stream( ss, combined_clear_key( crypt_password, hash ), stream_cipher_value( stream_cipher ) );

         string new_file_data( content.substr( 0, 1 ) );

         new_file_data += ss.str( );

         new_file_data[ 0 ] &= ~c_file_type_val_encrypted;

         content = new_file_data;

         clear_key( crypt_password );
      }

      string session_secret( get_session_secret( ) );

      // NOTE: Supports the encryption of file content immediately prior to transmission after padding this
      // content (to a maximum line length multiple) with random characters along with including the actual
      // size and number of padding characters in an encrtyped file transfer extra header.
      if( !session_secret.empty( ) )
      {
         size_t total_size = content.length( );

         if( total_size )
         {
            size_t padding = c_file_transfer_max_line_size - ( total_size % c_file_transfer_max_line_size );

            if( padding == c_file_transfer_max_line_size )
               padding = 0;

            if( padding )
               content += random_characters( padding );

            extra_header = to_comparable_string( total_size, false, 8 ) + '-' + to_comparable_string( padding, false, 8 );

            extra_header += c_crc32_separator + hex_crc32( content );

            stringstream ss( extra_header );

            crypt_stream( ss, hash, e_stream_cipher_chacha20 );

            extra_header = base64::encode( ss.str( ) );
         }

         stringstream ss( content );

         crypt_stream( ss, combined_clear_key( session_secret, hash ), e_stream_cipher_chacha20 );

         content = ss.str( );

         clear_key( session_secret );
      }

      file_buffer.copy_from_string( content );
   }

   ft_extra_info ft_extra( c_file_transfer_initial_timeout,
    c_file_transfer_line_timeout, c_file_transfer_max_line_size,
    0, file_buffer.get_buffer( ), file_buffer.get_size( ), c_response_okay_skip );

   ft_extra.extra_header = extra_header;

   file_transfer( "", socket, e_ft_direction_send,
    get_files_area_item_max_size( ), c_response_okay_more, &ft_extra, p_sock_progress );
}

bool store_file( const string& hash,
 tcp_socket& socket, const char* p_tag, progress* p_sock_progress,
 bool allow_core_file, size_t max_bytes, bool allow_missing_items,
 string* p_file_data, size_t* p_total_bytes, progress* p_progress, string* p_raw_file_data )
{
   string tag_name;
   if( p_tag )
      tag_name = string( p_tag );

   string archive;

   string archive_path( !tag_name.empty( ) ? string( )
    : get_session_variable( get_special_var_name( e_special_var_blockchain_archive_path ) ) );

   if( !archive_path.empty( ) )
   {
      string::size_type pos = archive_path.rfind( '/' );

      if( pos == string::npos )
         archive = archive_path;
      else
         archive = archive_path.substr( pos + 1 );
   }

   string file_hash( hash );

   // NOTE: As it is possible for the "file_hash" to change (due to list data transformation) the
   // potential creation of a directory may be unnecessary but as it is not possible to determine
   // this until after the file transfer has completed it is being overlooked.
   string file_name( construct_file_name_from_hash( file_hash, true,
    archive_path.empty( ), ( archive_path.empty( ) ? 0 : &archive_path ), !archive_path.empty( ) ) );

   size_t total_bytes = 0;

   int64_t existing_bytes = 0;

   bool is_existing = false;
   bool is_in_blacklist = false;
   bool file_extra_is_core = false;
   bool list_has_encrypted_blobs = false;

   if( !max_bytes || ( max_bytes > get_files_area_item_max_size( ) ) )
      max_bytes = get_files_area_item_max_size( );

   if( !p_file_data )
   {
      guard g( g_mutex, "store_file" );

      is_existing = file_exists( file_name );

      if( is_existing )
         existing_bytes = file_size( file_name );
   }

   string encrypted_list_data;

   // NOTE: Empty code block for scope purposes.
   {
      session_file_buffer_access file_buffer;

      auto_ptr< udp_stream_helper > ap_udp_stream_helper;

      if( get_stream_sock( ) )
         ap_udp_stream_helper.reset( new udp_stream_helper( hash ) );

      ft_extra_info ft_extra( c_file_transfer_initial_timeout,
       c_file_transfer_line_timeout, c_file_transfer_max_line_size, 0,
       file_buffer.get_buffer( ), file_buffer.get_size( ), ( !is_existing ? 0 : c_response_okay_skip ) );

      total_bytes = file_transfer( "", socket, e_ft_direction_recv, max_bytes,
       ( is_existing ? c_response_okay_skip : c_response_okay_more ), &ft_extra, p_sock_progress, ap_udp_stream_helper.get( ) );

      string session_secret( get_session_secret( ) );

      // NOTE: Supports the decryption of file data immediately after receiving. Also if found
      // will decrypt the extra file transfer header which should contain the actual size with
      // the number of padding bytes that were added (which are then removed).
      if( !session_secret.empty( ) )
      {
         string temporary( total_bytes, '\0' );

         file_buffer.copy_to_string( temporary, 0, total_bytes );

         stringstream ss( temporary );

         crypt_stream( ss, combined_clear_key( session_secret, hash ), e_stream_cipher_chacha20 );

         temporary = ss.str( );

         string extra_header( ft_extra.extra_header );

         if( extra_header.empty( ) )
            throw runtime_error( "unexpected missing extra header" );
         else
            remove_file_padding( hash, extra_header, temporary, total_bytes );

         file_buffer.copy_from_string( temporary, 0, false );

         clear_key( session_secret );
      }

      if( p_total_bytes )
         *p_total_bytes = total_bytes;

      if( p_raw_file_data )
      {
         p_raw_file_data->reserve( total_bytes );
         file_buffer.copy_to_string( *p_raw_file_data, 0, total_bytes );
      }

      if( !is_existing )
      {
         file_constructor new_file( file_name );

         unsigned char file_type = ( file_buffer.get_buffer( )[ 0 ] & c_file_type_val_mask );
         unsigned char file_extra = ( file_buffer.get_buffer( )[ 0 ] & c_file_type_val_extra_mask );

         if( file_type != c_file_type_val_blob && file_type != c_file_type_val_list )
            throw runtime_error( "invalid file type '0x" + hex_encode( &file_type, 1 ) + "' for store_file" );

         if( file_extra & c_file_type_val_extra_core )
         {
            if( allow_core_file )
               file_extra_is_core = true;
            else
               throw runtime_error( "core file not allowed for this store_file" );
         }

         bool is_encrypted = ( file_buffer.get_buffer( )[ 0 ] & c_file_type_val_encrypted );
         bool is_compressed = ( file_buffer.get_buffer( )[ 0 ] & c_file_type_val_compressed );
         bool is_no_encrypt = ( file_buffer.get_buffer( )[ 0 ] & c_file_type_val_no_encrypt );
         bool is_no_compress = ( file_buffer.get_buffer( )[ 0 ] & c_file_type_val_no_compress );

         if( is_encrypted && is_no_encrypt )
            throw runtime_error( "file must not have both the 'encrypted' and 'no_encrypt' bit flags set" );

         if( is_compressed && is_no_compress )
            throw runtime_error( "file must not have both the 'compressed' and 'no_compress' bit flags set" );

#ifdef ZLIB_SUPPORT
         if( !is_encrypted && is_compressed )
         {
            unsigned long size = total_bytes - 1;
            unsigned long usize = file_buffer.get_size( ) - size;

            if( uncompress( ( Bytef * )&file_buffer.get_buffer( )[ size + 1 ],
             &usize, ( Bytef * )&file_buffer.get_buffer( )[ 1 ], size ) != Z_OK )
               throw runtime_error( "invalid content for '" + hash + "' (bad compressed or uncompressed too large)" );

            if( usize + 1 > max_bytes )
               throw runtime_error( "uncompressed file size exceeds maximum permitted file size" );

            file_buffer.get_buffer( )[ size ] = file_buffer.get_buffer( )[ 0 ];
            validate_hash_with_uncompressed_content( hash, &file_buffer.get_buffer( )[ size ], usize + 1 );

            bool rc = true;

            const char* p_data_buffer = ( const char* )&file_buffer.get_buffer( )[ size + 1 ];

            if( !p_file_data && ( file_type != c_file_type_val_blob ) )
               validate_list( p_data_buffer, &rc, allow_missing_items, &list_has_encrypted_blobs );

            if( !rc )
               throw runtime_error( "invalid 'list' file" );

            if( p_file_data )
            {
               is_compressed = false;
               is_no_compress = true;

               total_bytes = usize + 1;

               file_buffer.get_buffer( )[ 0 ] &= ~c_file_type_val_compressed;

               memcpy( &file_buffer.get_buffer( )[ 1 ], &file_buffer.get_buffer( )[ size + 1 ], usize );
            }
         }
#endif

         bool rc = true;

         const char* p_data_buffer = ( const char* )&file_buffer.get_buffer( )[ 1 ];

         if( !p_file_data && !is_encrypted && !is_compressed && ( file_type != c_file_type_val_blob ) )
            validate_list( p_data_buffer, &rc, allow_missing_items, &list_has_encrypted_blobs );

         if( !rc )
            throw runtime_error( "invalid 'list' file" );

         if( !is_encrypted && !is_compressed )
            validate_hash_with_uncompressed_content( hash, &file_buffer.get_buffer( )[ 0 ], total_bytes );

         if( rc )
         {
            if( !is_existing )
               is_in_blacklist = file_has_been_blacklisted( hash );

            if( !is_existing && !is_in_blacklist )
            {
               guard g( g_mutex, "store_file" );

               if( g_total_files + g_files_constructing.size( ) > get_files_area_item_max_num( ) )
               {
                  // NOTE: First attempt to relegate an existing file in order to make room.
                  relegate_one_or_num_oldest_files( "", "", 1, 0, true );

                  if( g_total_files + g_files_constructing.size( ) > get_files_area_item_max_num( ) )
                     // FUTURE: This message should be handled as a server string message.
                     throw runtime_error( "Maximum files area item limit has been reached." );
               }
            }

            if( !is_existing && !is_in_blacklist )
            {
               string repository( get_system_variable(
                get_special_var_name( e_special_var_blockchain_backup_identity ) ) );

               string crypt_password;

               bool is_shared = false;

               if( !repository.empty( ) )
               {
                  crypt_password = get_session_variable(
                   get_special_var_name( e_special_var_repo_crypt_password ) );

                  if( crypt_password == get_special_var_name( e_special_var_sid ) )
                     get_identity( crypt_password, false, true );
               }

               string shared_identity( get_system_variable(
                get_special_var_name( e_special_var_blockchain_shared_identity ) ) );

               if( crypt_password.empty( ) && !shared_identity.empty( ) )
               {
                  crypt_password = get_session_variable(
                   get_special_var_name( e_special_var_encrypted_password ) );

                  if( !crypt_password.empty( ) )
                  {
                     is_shared = true;
                     decrypt_data( crypt_password, crypt_password );
                  }
               }

#ifndef ZLIB_SUPPORT
               if( !p_file_data )
               {
                  if( !is_encrypted && !crypt_password.empty( ) && ( file_type == c_file_type_val_blob ) )
                     encrypt_file_buffer( hash, repository, file_buffer, crypt_password, 0, total_bytes, is_shared );

                  if( !tag_name.empty( ) && list_has_encrypted_blobs && !crypt_password.empty( ) )
                  {
                     if( is_shared )
                        file_hash = transform_shared_list_info( repository, file_buffer, 0, total_bytes, p_progress );
                     else
                        file_hash = create_repository_lists( repository, file_buffer, 0, total_bytes, &encrypted_list_data, p_progress );

                     file_name = construct_file_name_from_hash( file_hash, true, true );

                     if( hash != file_hash )
                     {
                        is_existing = file_exists( file_name );

                        if( is_existing )
                           existing_bytes = file_size( file_name );
                     }
                  }

                  guard g( g_mutex, "store_file" );

                  if( archive.empty( ) )
                     write_file( file_name, ( unsigned char* )&file_buffer.get_buffer( )[ 0 ], total_bytes );
                  else
                  {
                     string file_data;
                     file_buffer.copy_to_string( file_data, 0, total_bytes );

                     create_raw_file_in_archive( archive, file_hash, file_data );
                  }
               }
               else
               {
                  *p_file_data = string( total_bytes, '\0' );
                  memcpy( &( *p_file_data )[ 0 ], &file_buffer.get_buffer( )[ 0 ], total_bytes );
               }
#else
               bool has_written = false;

               unsigned long size = total_bytes;

               if( !is_encrypted && !crypt_password.empty( ) && ( file_type == c_file_type_val_blob ) )
                  encrypt_file_buffer( hash, repository, file_buffer, crypt_password, 0, total_bytes, is_shared );

               if( !tag_name.empty( ) && list_has_encrypted_blobs && !crypt_password.empty( ) )
               {
                  if( is_shared )
                     file_hash = transform_shared_list_info( repository, file_buffer, 0, total_bytes, p_progress );
                  else
                     file_hash = create_repository_lists( repository, file_buffer, 0, total_bytes, &encrypted_list_data, p_progress );

                  file_name = construct_file_name_from_hash( file_hash, true, true );

                  if( hash != file_hash )
                  {
                     is_existing = file_exists( file_name );

                     if( is_existing )
                        existing_bytes = file_size( file_name );
                  }
               }

               if( !p_file_data && !is_no_compress
                && !is_encrypted && !is_compressed && size >= c_min_size_to_compress )
               {
                  unsigned long csize = file_buffer.get_size( );

                  int rc = compress2(
                   ( Bytef * )&file_buffer.get_buffer( )[ size + 1 ],
                   &csize, ( Bytef * )&file_buffer.get_buffer( )[ 1 ], size - 1, 9 ); // i.e. 9 is for maximum compression

                  if( rc == Z_OK )
                  {
                     if( csize < size )
                     {
                        has_written = true;
                        is_compressed = true;

                        file_buffer.get_buffer( )[ size ] = file_buffer.get_buffer( )[ 0 ] | c_file_type_val_compressed;

                        guard g( g_mutex, "store_file" );

                        if( !p_file_data )
                        {
                           if( archive.empty( ) )
                              write_file( file_name, ( unsigned char* )&file_buffer.get_buffer( )[ size ], csize + 1 );
                           else
                           {
                              string file_data;
                              file_buffer.copy_to_string( file_data, size, csize + 1 );

                              create_raw_file_in_archive( archive, hash, file_data );
                           }
                        }
                        else
                        {
                           *p_file_data = string( csize + 1, '\0' );
                           memcpy( &( *p_file_data )[ 0 ], &file_buffer.get_buffer( )[ size ], csize + 1 );
                        }
                     }
                  }
                  else if( rc != Z_BUF_ERROR )
                     throw runtime_error( "unexpected compression error in store_file" );
               }

               if( !has_written )
               {
                  if( !p_file_data )
                  {
                     guard g( g_mutex, "store_file" );

                     if( archive.empty( ) )
                        write_file( file_name, ( unsigned char* )&file_buffer.get_buffer( )[ 0 ], total_bytes );
                     else
                     {
                        string file_data;
                        file_buffer.copy_to_string( file_data, 0, total_bytes );

                        create_raw_file_in_archive( archive, hash, file_data );
                     }
                  }
                  else
                  {
                     *p_file_data = string( total_bytes, '\0' );
                     memcpy( &( *p_file_data )[ 0 ], &file_buffer.get_buffer( )[ 0 ], total_bytes );
                  }
               }
#endif
            }
         }

         if( !p_file_data && !is_in_blacklist && archive_path.empty( ) )
         {
            guard g( g_mutex, "store_file" );

            if( !is_existing )
               ++g_total_files;

            g_total_bytes -= existing_bytes;

            g_total_bytes += file_size( file_name );
         }
      }
   }

   if( !p_file_data && !is_in_blacklist )
   {
      guard g( g_mutex, "store_file" );

      if( !tag_name.empty( ) )
         tag_file( tag_name, file_hash );
      else if( !file_extra_is_core )
      {
         if( !archive_path.empty( ) )
            touch_file_in_archive( file_hash, archive );
         else
            tag_file( current_time_stamp_tag( ), file_hash, true );
      }

      if( !tag_name.empty( ) && !encrypted_list_data.empty( ) )
      {
         tag_name += c_repository_info_suffix;

         create_raw_file( encrypted_list_data, true, tag_name.c_str( ) );
      }
   }

   return !is_existing;
}

void delete_file( const string& hash, bool even_if_tagged, bool ignore_not_found )
{
   guard g( g_mutex );

   if( hash == c_ciyam_empty_file_hash )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Deleting the empty file is not allowed." );

   string tags( get_hash_tags( hash ) );
   string file_name( construct_file_name_from_hash( hash ) );

   if( tags.empty( ) || even_if_tagged )
   {
      if( !file_exists( file_name ) )
      {
         if( ignore_not_found )
            return;
         else
            // FUTURE: This message should be handled as a server string message.
            throw runtime_error( "File '" + file_name + "' not found." );
      }

      if( !tags.empty( ) )
      {
         vector< string > all_tags;
         split( tags, all_tags, '\n' );

         for( size_t i = 0; i < all_tags.size( ); i++ )
            tag_del( all_tags[ i ], false, false );
      }

      int64_t existing_bytes = file_size( file_name );

      file_remove( file_name );

      --g_total_files;

      g_total_bytes -= existing_bytes;
   }
}

void delete_file_tree( const string& hash, progress* p_progress )
{
   size_t total = 0;
   date_time dtm( date_time::local( ) );

   string all_hashes( file_type_info( hash,
    e_file_expansion_recursive_hashes, 0, 0, false, 0, true, false, p_progress, &dtm, &total, true ) );

   if( !all_hashes.empty( ) )
   {
      vector< string > hashes;
      split( all_hashes, hashes, '\n' );

      for( size_t i = 0; i < hashes.size( ); i++ )
         delete_file( hashes[ i ], true, true );
   }
}

void delete_files_for_tags( const string& pat, progress* p_progress, const char* p_excludes )
{
   guard g( g_mutex );

   date_time dtm( date_time::local( ) );

   string tags( list_file_tags( pat, p_excludes, 0, 0, 0, 0, true, p_progress, &dtm ) );

   if( !tags.empty( ) )
   {
      vector< string > all_tags;
      split( tags, all_tags, '\n' );

      set< string > hashes;

      for( size_t i = 0; i < all_tags.size( ); i++ )
         hashes.insert( tag_file_hash( all_tags[ i ] ) );

      size_t num_seconds;

      session_progress_settings( num_seconds, p_progress );

      size_t num = 0;

      for( set< string >::iterator i = hashes.begin( ); i != hashes.end( ); ++i )
      {
         ++num;

         if( p_progress )
         {
            date_time now( date_time::local( ) );

            uint64_t elapsed = seconds_between( dtm, now );

            if( elapsed >= num_seconds )
            {
               dtm = now;

               // FUTURE: This message should be handled as a server string message.
               p_progress->output_progress( "Processed " + to_string( num ) + " files..." );
            }
         }

         delete_file( *i );
      }
   }
}

void copy_raw_file( const string& hash, const string& dest_file_name )
{
   guard g( g_mutex );

   string file_name( construct_file_name_from_hash( hash ) );

   if( !file_exists( file_name ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "File '" + file_name + "' not found." );

   file_copy( file_name, dest_file_name );
}

void store_temp_file( const string& hash, const string& name, tcp_socket& socket, progress* p_progress, bool is_existing )
{
   session_file_buffer_access file_buffer;

   ft_extra_info ft_extra( c_file_transfer_initial_timeout,
    c_file_transfer_line_timeout, c_file_transfer_max_line_size, 0,
    file_buffer.get_buffer( ), file_buffer.get_size( ), ( !is_existing ? 0 : c_response_okay_skip ) );

   size_t total_bytes = file_transfer(
    name, socket, e_ft_direction_recv, get_files_area_item_max_size( ),
    ( is_existing ? c_response_okay_skip : c_response_okay_more ), &ft_extra, p_progress );

   if( !name.empty( ) )
   {
      string session_secret( get_session_secret( ) );

      // NOTE: Supports the decryption of file data immediately after receiving.
      if( !session_secret.empty( ) )
      {
         string temporary( total_bytes, '\0' );

         file_buffer.copy_to_string( temporary, 0, total_bytes );

         stringstream ss( temporary );

         crypt_stream( ss, combined_clear_key( session_secret, hash ), e_stream_cipher_chacha20 );

         temporary = ss.str( );

         string extra_header( ft_extra.extra_header );

         if( !extra_header.empty( ) )
            remove_file_padding( hash, extra_header, temporary, total_bytes );

         file_buffer.copy_from_string( temporary, 0, false );

         clear_key( session_secret );
      }

      write_file( name, ( unsigned char* )&file_buffer.get_buffer( )[ 0 ], total_bytes );
   }
}

bool temp_file_is_identical( const string& temp_name, const string& hash )
{
   string file_name( construct_file_name_from_hash( hash ) );

   return files_are_identical( temp_name, file_name );
}

string extract_file( const string& hash,
 const string& dest_file_name, unsigned char check_file_type_and_extra,
 bool* p_is_list, unsigned char* p_type_and_extra, bool* p_is_encrypted, bool set_is_encrypted )
{
   guard g( g_mutex );

   string archive_path( get_session_variable(
    get_special_var_name( e_special_var_blockchain_archive_path ) ) );

   string file_name( construct_file_name_from_hash(
    hash, false, archive_path.empty( ), ( archive_path.empty( ) ? 0 : &archive_path ) ) );

   string data( buffer_file( file_name ) );

   if( !data.empty( ) )
   {
      if( check_file_type_and_extra )
      {
         if( data[ 0 ] & check_file_type_and_extra != check_file_type_and_extra )
            throw runtime_error( "unexpected file type/extra" );
      }

      unsigned char file_type = ( data[ 0 ] & c_file_type_val_mask );
      unsigned char file_extra = ( data[ 0 ] & c_file_type_val_extra_mask );

      if( p_is_list )
         *p_is_list = ( file_type == c_file_type_val_list );

      if( set_is_encrypted )
         data[ 0 ] |= c_file_type_val_encrypted;

      bool is_encrypted = ( data[ 0 ] & c_file_type_val_encrypted );
      bool is_compressed = ( data[ 0 ] & c_file_type_val_compressed );

      if( p_is_encrypted )
         *p_is_encrypted = is_encrypted;

      if( p_type_and_extra )
         *p_type_and_extra = data[ 0 ];

      session_file_buffer_access file_buffer;

      size_t offset = 1;

      unsigned long size = file_size( file_name ) - offset;

#ifdef ZLIB_SUPPORT
      if( !is_encrypted && is_compressed )
      {
         unsigned long usize = file_buffer.get_size( ) - size;

         if( uncompress(
          ( Bytef * )&file_buffer.get_buffer( )[ 0 ], &usize, ( Bytef * )&data[ offset ], size ) != Z_OK )
            throw runtime_error( "invalid content for '" + hash + "' (bad compressed or uncompressed too large)" );

         size = usize;
      }
#endif

      if( is_encrypted || !is_compressed )
         memcpy( file_buffer.get_buffer( ), &data[ offset ], size );

      if( !dest_file_name.empty( ) )
         write_file( dest_file_name, file_buffer.get_buffer( ), size );

      data = string( ( const char* )file_buffer.get_buffer( ), size );
   }

   return data;
}

struct system_ods_fs_guard::impl
{
   impl( )
    :
    g( g_mutex )
   {
   }

   guard g;
};

system_ods_fs_guard::system_ods_fs_guard( )
{
   p_impl = new impl;
}

system_ods_fs_guard::~system_ods_fs_guard( )
{
   delete p_impl;
}

void add_file_archive( const string& name, const string& path, int64_t size_limit )
{
   guard g( g_mutex );

   if( size_limit < 0 )
      throw runtime_error( "unexpected negative size_limit provided to add_file_archive" );

   string tmp_file_name( "~" + uuid( ).as_string( ) );

   if( path.empty( )
    || path[ path.length( ) - 1 ] == '\\' || path[ path.length( ) - 1 ] == '/' )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Invalid path '" + path + "' for 'add_file_archive'." );

   if( path_already_used_in_archive( path ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "An archive with the path '" + path + "' already exists." );

   int64_t min_limit = get_files_area_item_max_size( );

   if( size_limit < min_limit )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Archive minimum size must be at least " + format_bytes( min_limit ) + " bytes." );

   string status_info( get_archive_status( path ) );

   system_ods_bulk_write ods_bulk_write;

   ods_file_system& ods_fs( system_ods_file_system( ) );

   ods::transaction ods_tx( system_ods_instance( ) );

   ods_fs.set_root_folder( c_system_archives_folder );

   if( ods_fs.has_folder( name ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Archive '" + name + "' already exists." );

   ods_fs.add_folder( name );
   ods_fs.set_folder( name );

   ods_fs.store_as_text_file( c_file_archive_path, path );
   ods_fs.store_as_text_file( c_file_archive_size_avail, size_limit );
   ods_fs.store_as_text_file( c_file_archive_size_limit, size_limit );

   ods_fs.store_as_text_file( c_file_archive_status_info, status_info, c_status_info_pad_len );

   ods_tx.commit( );

   g_archive_file_info.insert( make_pair( name, archive_file_info( ) ) );
}

void clear_file_archive( const string& name )
{
   guard g( g_mutex );

   system_ods_bulk_write ods_bulk_write;

   ods_file_system& ods_fs( system_ods_file_system( ) );

   ods_fs.set_root_folder( c_system_archives_folder );

   if( !ods_fs.has_folder( name ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Archive '" + name + "' not found." );
   else
   {
      ods::transaction ods_tx( system_ods_instance( ) );

      ods_fs.set_folder( name );

      string path;
      ods_fs.fetch_from_text_file( c_file_archive_path, path );

      int64_t size_limit = 0;
      ods_fs.fetch_from_text_file( c_file_archive_size_limit, size_limit );

      string status_info;
      ods_fs.fetch_from_text_file( c_file_archive_status_info, status_info );

      string new_status_info( get_archive_status( path ) );

      if( trim( status_info ) != new_status_info )
         ods_fs.store_as_text_file( c_file_archive_status_info, new_status_info, c_status_info_pad_len );

      if( new_status_info == string( c_okay ) )
      {
         while( true )
         {
            string next( g_archive_file_info[ name ].get_oldest_file( ) );

            if( next.empty( ) )
               break;

            file_remove( path + '/' + next );

            g_archive_file_info[ name ].remove_file( next );
         }

         ods_fs.store_as_text_file( c_file_archive_size_avail, size_limit );
         ods_fs.store_as_text_file( c_file_archive_size_limit, size_limit );

         ods_tx.commit( );
      }
   }
}

void remove_file_archive( const string& name, bool destroy_files, bool remove_directory, progress* p_progress )
{
   guard g( g_mutex );

   date_time dtm( date_time::local( ) );

   system_ods_bulk_write ods_bulk_write( p_progress );

   ods_file_system& ods_fs( system_ods_file_system( ) );

   ods_fs.set_root_folder( c_system_archives_folder );

   if( !ods_fs.has_folder( name ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Archive '" + name + "' not found." );
   else
   {
      ods::transaction ods_tx( system_ods_instance( ) );

      if( destroy_files )
      {
         ods_fs.set_folder( name );

         string path;
         ods_fs.fetch_from_text_file( c_file_archive_path, path );

         string status_info;
         ods_fs.fetch_from_text_file( c_file_archive_status_info, status_info );

         string new_status_info( get_archive_status( path ) );

         if( trim( status_info ) != new_status_info )
            ods_fs.store_as_text_file( c_file_archive_status_info, new_status_info, c_status_info_pad_len );

         if( new_status_info == string( c_okay ) )
         {
            size_t num_seconds;

            session_progress_settings( num_seconds, p_progress );

            while( true )
            {
               string next( g_archive_file_info[ name ].get_oldest_file( ) );

               if( next.empty( ) )
                  break;

               file_remove( path + '/' + next );

               g_archive_file_info[ name ].remove_file( next );

               if( p_progress )
               {
                  date_time now( date_time::local( ) );

                  uint64_t elapsed = seconds_between( dtm, now );

                  if( elapsed >= num_seconds )
                  {
                     dtm = now;
                     p_progress->output_progress( "." );
                  }
               }
            }

            if( remove_directory )
               file_remove( path );

            ods_fs.set_root_folder( c_system_archives_folder );
         }
      }

      ods_fs.remove_folder( name, 0, true );

      ods_tx.commit( );
   }

   g_archive_file_info.erase( name );
}

void repair_file_archive( const string& name, progress* p_progress )
{
   guard g( g_mutex, "repair_file_archive" );

   g_archive_file_info.erase( name );
   g_archive_file_info.insert( make_pair( name, archive_file_info( ) ) );

   date_time dtm( date_time::local( ) );

   system_ods_bulk_write ods_bulk_write;

   ods_file_system& ods_fs( system_ods_file_system( ) );

   ods_fs.set_root_folder( c_system_archives_folder );

   if( !ods_fs.has_folder( name ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Archive '" + name + "' not found." );
   else
   {
      ods::transaction ods_tx( system_ods_instance( ) );

      ods_fs.set_folder( name );

      string path;
      ods_fs.fetch_from_text_file( c_file_archive_path, path );

      int64_t size_used = 0;
      int64_t size_avail = 0;

      int64_t size_limit = 0;
      ods_fs.fetch_from_text_file( c_file_archive_size_limit, size_limit );

      string status_info;
      ods_fs.fetch_from_text_file( c_file_archive_status_info, status_info );

      string new_status_info( get_archive_status( path ) );

      if( trim( status_info ) != new_status_info )
         ods_fs.store_as_text_file( c_file_archive_status_info, new_status_info, c_status_info_pad_len );

      if( new_status_info == string( c_okay ) )
      {
         // NOTE: Iterate through the files in the path adding all
         // that have names that appear to be valid SHA256 hashes.
         file_filter ff;
         fs_iterator fs( path, &ff );

         regex expr( c_regex_hash_256, true, true );

         size_t num_seconds;

         session_progress_settings( num_seconds, p_progress );

         while( fs.has_next( ) )
         {
            string file_name( fs.get_name( ) );

            if( expr.search( file_name ) == 0 )
            {
               size_used += file_size( fs.get_full_name( ) );

               int64_t tm_val = last_modification_time( fs.get_full_name( ) );

               g_archive_file_info[ name ].add_file( file_name, tm_val );
            }

            if( p_progress )
            {
               date_time now( date_time::local( ) );

               uint64_t elapsed = seconds_between( dtm, now );

               if( elapsed >= num_seconds )
               {
                  dtm = now;
                  p_progress->output_progress( "." );
               }
            }
         }

         if( size_used > size_limit )
            size_limit = size_used;
         else
            size_avail = size_limit - size_used;

         ods_fs.store_as_text_file( c_file_archive_size_avail, size_avail );
         ods_fs.store_as_text_file( c_file_archive_size_limit, size_limit );
      }

      ods_tx.commit( );
   }
}

void resize_file_archive( const string& name, int64_t new_size_limit, progress* p_progress, int64_t* p_bytes_used )
{
   guard g( g_mutex, "resize_file_archive" );

   int64_t min_limit = get_files_area_item_max_size( );

   if( new_size_limit && ( new_size_limit < min_limit ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Archive minimum size must be at least " + format_bytes( min_limit ) + " bytes." );

   system_ods_bulk_write ods_bulk_write( p_progress );

   ods_file_system& ods_fs( system_ods_file_system( ) );

   ods_fs.set_root_folder( c_system_archives_folder );

   if( !ods_fs.has_folder( name ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Archive '" + name + "' not found." );
   else
   {
      ods::transaction ods_tx( system_ods_instance( ) );

      ods_fs.set_folder( name );

      string path;
      ods_fs.fetch_from_text_file( c_file_archive_path, path );

      int64_t size_avail = 0;
      ods_fs.fetch_from_text_file( c_file_archive_size_avail, size_avail );

      int64_t size_limit = 0;
      ods_fs.fetch_from_text_file( c_file_archive_size_limit, size_limit );

      int64_t bytes_used = ( size_limit - size_avail );

      if( p_bytes_used )
      {
         bytes_used = *p_bytes_used;

         if( size_limit < bytes_used )
            size_limit = bytes_used;
      }

      if( !new_size_limit )
         new_size_limit = size_limit;

      string status_info;
      ods_fs.fetch_from_text_file( c_file_archive_status_info, status_info );

      string new_status_info( get_archive_status( path ) );

      if( trim( status_info ) != new_status_info )
         ods_fs.store_as_text_file( c_file_archive_status_info, new_status_info, c_status_info_pad_len );

      if( new_status_info == string( c_okay ) )
      {
         if( bytes_used > new_size_limit )
         {
            while( true )
            {
               string next( g_archive_file_info[ name ].get_oldest_file( ) );

               if( next.empty( ) )
                  break;

               bytes_used -= file_size( path + '/' + next );

               file_remove( path + '/' + next );

               g_archive_file_info[ name ].remove_file( next );

               if( bytes_used <= new_size_limit )
                  break;
            }
         }

         size_avail = ( new_size_limit - bytes_used );

         ods_fs.store_as_text_file( c_file_archive_size_avail, size_avail );
         ods_fs.store_as_text_file( c_file_archive_size_limit, new_size_limit );
      }

      ods_tx.commit( );
   }
}

void archives_status_update( const string& name )
{
   guard g( g_mutex, "archives_status_update" );

   system_ods_bulk_write ods_bulk_write;

   ods_file_system& ods_fs( system_ods_file_system( ) );

   ods_fs.set_root_folder( c_system_archives_folder );

   vector< string > names;
   ods_fs.list_folders( names );

   ods::transaction ods_tx( system_ods_instance( ) );

   for( size_t i = 0; i < names.size( ); i++ )
   {
      string next( names[ i ] );

      if( !name.empty( ) && next != name )
         continue;

      ods_fs.set_folder( next );

      string path;
      ods_fs.fetch_from_text_file( c_file_archive_path, path );

      int64_t avail = 0;
      ods_fs.fetch_from_text_file( c_file_archive_size_avail, avail );

      string status_info;
      ods_fs.fetch_from_text_file( c_file_archive_status_info, status_info );

      string new_status_info;

      if( avail == 0 )
         new_status_info = string( c_file_archive_status_is_full );
      else
         new_status_info = get_archive_status( path );

      if( trim( status_info ) != new_status_info )
         ods_fs.store_as_text_file( c_file_archive_status_info, new_status_info, c_status_info_pad_len );

      ods_fs.set_folder( ".." );
   }

   ods_tx.commit( );
}

bool file_has_been_blacklisted( const string& hash )
{
   guard g( g_mutex );

   bool retval = false;

   system_ods_bulk_read ods_bulk_read;

   ods_file_system& ods_fs( system_ods_file_system( ) );

   ods_fs.set_root_folder( c_system_blacklist_folder );

   if( ods_fs.has_file( hash ) )
      retval = true;

   return retval;
}

bool has_file_archive( const string& archive, string* p_path )
{
   guard g( g_mutex );

   bool retval = false;

   vector< string > paths;

   string all_archives( list_file_archives( e_archive_list_type_minimal, &paths ) );

   if( !all_archives.empty( ) )
   {
      vector< string > archives;

      split( all_archives, archives, '\n' );

      if( paths.size( ) != archives.size( ) )
         throw runtime_error( "unexpected paths.size( ) != archives.size( )" );

      for( size_t i = 0; i < archives.size( ); i++ )
      {
         string next( archives[ i ] );

         if( next == archive )
         {
            if( p_path )
               *p_path = paths[ i ];

            retval = true;
            break;
         }
      }
   }

   return retval;
}

string list_file_archives( archive_list_type list_type,
 vector< string >* p_paths, int64_t min_avail, bool stop_after_first, const string* p_name )
{
   guard g( g_mutex );

   string retval;
   vector< string > names;

   system_ods_bulk_read ods_bulk_read;

   ods_file_system& ods_fs( system_ods_file_system( ) );

   ods_fs.set_root_folder( c_system_archives_folder );

   ods_fs.list_folders( names );

   for( size_t i = 0; i < names.size( ); i++ )
   {
      string next( names[ i ] );

      if( p_name && next != *p_name )
         continue;

      if( !retval.empty( ) )
         retval += '\n';

      ods_fs.set_folder( next );

      string path;
      ods_fs.fetch_from_text_file( c_file_archive_path, path );

      int64_t avail = 0;
      int64_t limit = 0;

      if( list_type == e_archive_list_type_minimal )
      {
         retval += next;

         if( p_paths )
            p_paths->push_back( path );

         ods_fs.set_folder( ".." );

         continue;
      }
      else if( list_type == e_archive_list_type_path_only )
      {
         retval += path;

         if( p_paths )
            p_paths->push_back( path );

         ods_fs.set_folder( ".." );

         continue;
      }

      string status_info;

      ods_fs.fetch_from_text_file( c_file_archive_size_avail, avail );
      ods_fs.fetch_from_text_file( c_file_archive_size_limit, limit );
      ods_fs.fetch_from_text_file( c_file_archive_status_info, status_info );

      if( min_avail <= 0 || avail >= min_avail )
      {
         retval += next;

         retval += " [" + status_info + "] ("
          + format_bytes( limit - avail ) + '/' + format_bytes( limit ) + ") " + path;

         if( p_paths )
            p_paths->push_back( path );

         if( stop_after_first )
            break;
      }

      ods_fs.set_folder( ".." );
   }

   return retval;
}

void create_raw_file_in_archive( const string& archive,
 const string& hash, const string& file_data, string* p_hash )
{
   guard g( g_mutex, "create_raw_file_in_archive" );

   vector< string > paths;
   vector< string > archives;

   string file_hash( hash );

   if( file_hash.empty( ) )
   {
      file_hash = sha256( file_data ).get_digest_as_string( );

      if( p_hash )
         *p_hash = file_hash;
   }

   string archive_found( archive );

   if( has_file_been_archived( file_hash, &archive_found, true ) )
      touch_file_in_archive( file_hash, archive );
   else
   {
      system_ods_bulk_write ods_bulk_write;

      string all_archives;

      // NOTE: If "@blockchain_archive_path" has been set then can avoid
      // calling "list_file_archives".
      string archive_path( get_session_variable(
       get_special_var_name( e_special_var_blockchain_archive_path ) ) );

      if( !archive_path.empty( ) )
      {
         all_archives = archive;
         paths.push_back( archive_path );
      }

      if( all_archives.empty( ) )
         all_archives = list_file_archives( e_archive_list_type_minimal, &paths );

      ods_file_system& ods_fs( system_ods_file_system( ) );

      bool found = false;
      bool has_changed = false;

      int64_t new_avail = 0;

      if( !all_archives.empty( ) )
      {
         split( all_archives, archives, '\n' );

         if( paths.size( ) != archives.size( ) )
            throw runtime_error( "unexpected paths.size( ) != archives.size( )" );

         for( size_t i = 0; i < archives.size( ); i++ )
         {
            string next_archive( archives[ i ] );

            if( archive != next_archive )
               continue;

            found = true;

            ods_fs.set_root_folder( c_system_archives_folder );

            ods_fs.set_folder( next_archive );

            int64_t avail = 0;
            ods_fs.fetch_from_text_file( c_file_archive_size_avail, avail );

            int64_t prior_avail = avail;

            int64_t limit = 0;
            ods_fs.fetch_from_text_file( c_file_archive_size_limit, limit );

            while( avail < file_data.size( ) )
            {
               string next_hash( g_archive_file_info[ next_archive ].get_oldest_file( ) );

               string file_path( paths[ i ] + "/" + next_hash );

               avail += file_size( file_path );

               file_remove( file_path );

               g_archive_file_info[ next_archive ].remove_file( next_hash );
            }

            avail -= file_data.size( );

            write_file( paths[ i ] + "/" + file_hash, file_data );

            int64_t tm_val = last_modification_time( paths[ i ] + "/" + file_hash );

            g_archive_file_info[ archive ].add_file( file_hash, tm_val );

            if( avail != prior_avail )
            {
               new_avail = avail;
               has_changed = true;
            }

            break;
         }
      }

      if( !found )
         // FUTURE: This message should be handled as a server string message.
         throw runtime_error( "File archive '" + archive + "' was not found." );

      if( has_changed )
      {
         auto_ptr< ods::transaction > ap_ods_tx;

         if( !system_ods_instance( ).is_in_transaction( ) )
            ap_ods_tx.reset( new ods::transaction( system_ods_instance( ) ) );

         ods_fs.store_as_text_file( c_file_archive_size_avail, new_avail );

         if( ap_ods_tx.get( ) )
            ap_ods_tx->commit( );
      }
   }
}

string relegate_one_or_num_oldest_files( const string& hash,
 const string& archive, uint32_t max_files, int64_t max_bytes, bool delete_files_always )
{
   guard g( g_mutex, "relegate_one_or_num_oldest_files" );

   string retval;

   vector< string > paths;
   deque< string > file_hashes;

   int64_t min_bytes = 0;

   if( !hash.empty( ) )
      file_hashes.push_back( hash );
   else
   {
      string time_stamp_expr( c_time_stamp_tag_prefix );
      time_stamp_expr += "*";

      string all_tags( list_file_tags( time_stamp_expr, 0, max_files, max_bytes, &min_bytes, &file_hashes, false ) );

      if( !all_tags.empty( ) )
      {
         vector< string > tags;
         split( all_tags, tags, '\n' );

         if( tags.size( ) != file_hashes.size( ) )
            throw runtime_error( "unexpected tags.size( ) != file_hashes.size( )" );
      }
   }

   int64_t num_bytes = hash.empty( ) ? min_bytes : file_bytes( hash );

   string all_archives( list_file_archives( e_archive_list_type_minimal, &paths, num_bytes ) );

   if( !file_hashes.empty( ) && !all_archives.empty( ) )
   {
      vector< string > archives;
      split( all_archives, archives, '\n' );

      if( paths.size( ) != archives.size( ) )
         throw runtime_error( "unexpected paths.size( ) != archives.size( )" );

      system_ods_bulk_write ods_bulk_write;

      ods_file_system& ods_fs( system_ods_file_system( ) );

      ods::transaction ods_tx( system_ods_instance( ) );

      for( size_t i = 0; i < archives.size( ); i++ )
      {
         if( !archive.empty( ) && archives[ i ] != archive )
            continue;

         string next_archive( archives[ i ] );

         ods_fs.set_root_folder( c_system_archives_folder );

         ods_fs.set_folder( next_archive );

         int64_t avail = 0;
         ods_fs.fetch_from_text_file( c_file_archive_size_avail, avail );

         while( !file_hashes.empty( ) )
         {
            string next_hash( file_hashes.front( ) );

            if( has_file_been_archived( next_hash, &next_archive, true ) )
            {
               delete_file( next_hash, true );

               if( !retval.empty( ) )
                  retval += '\n';
               retval = next_hash + ' ' + next_archive;

               file_hashes.pop_front( );

               continue;
            }

            num_bytes = file_bytes( next_hash );

            if( num_bytes > avail )
               break;

            string dest( paths[ i ] + "/" + next_hash );

            copy_raw_file( next_hash, dest );

            int64_t tm_val = last_modification_time( dest );

            if( !temp_file_is_identical( dest, next_hash ) )
            {
               file_remove( dest );
               break;
            }
            else
            {
               avail -= num_bytes;
               ods_fs.store_as_text_file( c_file_archive_size_avail, avail );

               g_archive_file_info[ next_archive ].add_file( next_hash, tm_val );

               delete_file( next_hash, true );

               if( !retval.empty( ) )
                  retval += '\n';

               retval += next_hash + ' ' + next_archive;

               file_hashes.pop_front( );
            }
         }
      }

      ods_tx.commit( );
   }

   // NOTE: If "delete_files_always" is set true then delete the entire
   // file list regardless of whether any were relegated to an archive.
   if( !file_hashes.empty( ) && delete_files_always )
   {
      while( !file_hashes.empty( ) )
      {
         string next_hash( file_hashes.front( ) );
         delete_file( next_hash, true );
         file_hashes.pop_front( );
      }
   }

   return retval;
}

size_t files_in_archive( const string& archive )
{
   guard g( g_mutex );

   size_t total_files = 0;

   if( g_archive_file_info.find( archive ) != g_archive_file_info.end( ) )
      total_files = g_archive_file_info[ archive ].total_files( );

   return total_files;
}

bool has_file_been_archived( const string& hash, string* p_archive_name, bool in_specific_archive_only )
{
   guard g( g_mutex );

   bool retval = false;

   string archive;
   map< string, archive_file_info >::const_iterator ci = g_archive_file_info.begin( );

   while( ci != g_archive_file_info.end( ) )
   {
      bool check = true;

      archive = ci->first;

      if( p_archive_name && in_specific_archive_only )
      {
         if( archive != *p_archive_name )
            check = false;
      }

      if( check && ci->second.has_file( hash ) )
      {
         if( p_archive_name && !in_specific_archive_only )
            *p_archive_name = archive;

         retval = true;
         break;
      }

      ++ci;
   }

   return retval;
}

string retrieve_file_from_archive( const string& hash, const string& tag, size_t days_ahead )
{
   guard g( g_mutex );

   string retval;

   // NOTE: Need to use this as "has_file" will return true if the
   // "@blockchain_archive_path" session variable has been set and
   // the file is found in that archive.
   bool is_in_archive = false;

   if( !has_file( hash, false, &is_in_archive ) || is_in_archive )
   {
      vector< string > paths;

      string all_archives( list_file_archives( e_archive_list_type_minimal, &paths ) );

      string archive;
      vector< string > archives;

      if( !all_archives.empty( ) )
      {
         split( all_archives, archives, '\n' );

         if( paths.size( ) != archives.size( ) )
            throw runtime_error( "unexpected paths.size( ) != archives.size( )" );

         for( size_t i = 0; i < archives.size( ); i++ )
         {
            archive = archives[ i ];

            if( g_archive_file_info[ archive ].has_file( hash ) )
            {
               retval = archive;

               string src_file( paths[ i ] + "/" + hash );

               if( file_exists( src_file ) )
               {
                  string file_data( buffer_file( src_file ) );

                  string tag_for_file( tag );

                  if( tag_for_file.empty( ) )
                     tag_for_file = current_time_stamp_tag( false, days_ahead );

                  create_raw_file( file_data, false, tag_for_file.c_str( ), 0, hash.c_str( ), false );

                  break;
               }
            }
         }
      }

      if( retval.empty( ) )
         // FUTURE: This message should be handled as a server string message.
         throw runtime_error( "Unable to retrieve file " + hash + " from archival." );
   }

   return retval;
}

bool touch_file_in_archive( const string& hash, const string& archive )
{
   guard g( g_mutex );

   bool retval = false;

   vector< string > paths;
   vector< string > archives;

   string all_archives;

   if( !archive.empty( ) )
   {
      // NOTE: If "@blockchain_archive_path" has been set then can avoid
      // calling "list_file_archives".
      string archive_path( get_session_variable(
       get_special_var_name( e_special_var_blockchain_archive_path ) ) );

      if( !archive_path.empty( ) )
      {
         all_archives = archive;
         paths.push_back( archive_path );
      }
   }

   if( all_archives.empty( ) )
      all_archives = list_file_archives( e_archive_list_type_minimal, &paths );

   if( !all_archives.empty( ) )
   {
      split( all_archives, archives, '\n' );

      if( paths.size( ) != archives.size( ) )
         throw runtime_error( "unexpected paths.size( ) != archives.size( )" );

      for( size_t i = 0; i < archives.size( ); i++ )
      {
         string next_archive( archives[ i ] );

         if( !archive.empty( ) && archive != next_archive )
            continue;

         if( g_archive_file_info[ next_archive ].has_file( hash ) )
         {
            file_touch( paths[ i ] + '/' + hash );

            int64_t tm_val = last_modification_time( paths[ i ] + '/' + hash );

            g_archive_file_info[ next_archive ].remove_file( hash );
            g_archive_file_info[ next_archive ].add_file( hash, tm_val );

            retval = true;
         }
      }
   }

   return retval;
}

void delete_file_from_archive( const string& hash, const string& archive, bool add_to_blacklist )
{
   guard g( g_mutex, "delete_file_from_archive" );

   regex expr( c_regex_hash_256, true, true );

   if( expr.search( hash ) == string::npos )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Invalid file hash '" + hash + "'." );

   vector< string > paths;
   vector< string > archives;

   system_ods_bulk_write ods_bulk_write;

   string all_archives( list_file_archives( e_archive_list_type_minimal, &paths ) );

   auto_ptr< ods::transaction > ap_ods_tx;

   if( !system_ods_instance( ).is_in_transaction( ) )
      ap_ods_tx.reset( new ods::transaction( system_ods_instance( ) ) );

   ods_file_system& ods_fs( system_ods_file_system( ) );

   if( !all_archives.empty( ) )
   {
      split( all_archives, archives, '\n' );

      if( paths.size( ) != archives.size( ) )
         throw runtime_error( "unexpected paths.size( ) != archives.size( )" );

      for( size_t i = 0; i < archives.size( ); i++ )
      {
         string next_archive( archives[ i ] );

         if( !archive.empty( ) && archive != next_archive )
            continue;

         ods_fs.set_root_folder( c_system_archives_folder );

         ods_fs.set_folder( next_archive );

         int64_t avail = 0;
         ods_fs.fetch_from_text_file( c_file_archive_size_avail, avail );

         int64_t limit = 0;
         ods_fs.fetch_from_text_file( c_file_archive_size_limit, limit );

         if( g_archive_file_info[ next_archive ].has_file( hash ) )
         {
            string src_file( paths[ i ] + "/" + hash );

            if( file_exists( src_file ) )
            {
               avail += file_size( src_file );
               file_remove( src_file );

               if( avail > limit )
                  avail = limit;

               ods_fs.store_as_text_file( c_file_archive_size_avail, avail );
            }

            g_archive_file_info[ next_archive ].remove_file( hash );
         }
      }
   }

   // NOTE: If no archive was specified then will also remove
   // the file from the files area.
   if( archive.empty( ) && has_file( hash ) )
      delete_file( hash, true );

   if( add_to_blacklist )
   {
      ods_fs.set_root_folder( c_system_blacklist_folder );
      ods_fs.add_file( hash, c_file_zero_length );

      // NOTE: If a matching repository entry is found then will
      // delete it along with removing the equivalent local file
      // from all archives and the files area.
      ods_fs.set_root_folder( c_system_repository_folder );

      if( ods_fs.has_file( hash ) )
      {
         stringstream sio_data;
         ods_fs.get_file( hash, &sio_data );
         
         sio_reader reader( sio_data );

         string local_hash( reader.read_opt_attribute( c_file_repository_local_hash_attribute ) );

         ods_fs.remove_file( hash );

         if( !local_hash.empty( ) )
            delete_file_from_archive( local_hash, "" );
      }
   }

   if( ap_ods_tx.get( ) )
      ap_ods_tx->commit( );
}

bool has_repository_entry_record( const string& repository, const string& hash )
{
   guard g( g_mutex );

   system_ods_bulk_read ods_bulk_read;

   ods_file_system& ods_fs( system_ods_file_system( ) );

   ods_fs.set_root_folder( c_system_repository_folder );

   string file_name( repository + '.' + base64::encode( hex_decode( hash ), true ) );

   return ods_fs.has_file( file_name, true );
}

bool fetch_repository_entry_record(
 const string& repository, const string& hash, string& local_hash, bool must_exist )
{
   guard g( g_mutex );

   system_ods_bulk_read ods_bulk_read;

   ods_file_system& ods_fs( system_ods_file_system( ) );

   ods_fs.set_root_folder( c_system_repository_folder );

   string file_name( repository + '.' + base64::encode( hex_decode( hash ), true ) );

   string suffix;
   bool has_file = ods_fs.has_file( file_name, true, &suffix );

   if( !has_file && !must_exist )
      return false;

   if( suffix.length( ) > 1 )
   {
      local_hash = suffix.substr( 1 );
      local_hash = hex_encode( base64::decode( local_hash, true ) );
   }
   else
   {
      try
      {
         stringstream sio_data;
         ods_fs.get_file( file_name + suffix, &sio_data );

         sio_reader reader( sio_data );

         local_hash = reader.read_attribute( c_attribute_local_hash );
      }
      catch( exception& x )
      {
         throw runtime_error( x.what( ) + string( " when fetching " ) + file_name );
      }
      catch( ... )
      {
         throw runtime_error( "unexpected error occurred when fetching " + file_name );
      }
   }

   return true;
}

bool fetch_repository_entry_record( const string& repository, const string& hash,
 string& local_hash, string& local_public_key, string& master_public_key, bool must_exist )
{
   guard g( g_mutex );

   system_ods_bulk_read ods_bulk_read;

   ods_file_system& ods_fs( system_ods_file_system( ) );

   ods_fs.set_root_folder( c_system_repository_folder );

   string file_name( repository + '.' + base64::encode( hex_decode( hash ), true ) );

   string suffix;
   bool has_file = ods_fs.has_file( file_name, true, &suffix );

   if( !has_file && !must_exist )
      return false;

   try
   {
      stringstream sio_data;
      ods_fs.get_file( file_name + suffix, &sio_data );

      sio_reader reader( sio_data );

      local_hash = reader.read_attribute( c_attribute_local_hash );
      local_public_key = reader.read_attribute( c_attribute_local_public_key );
      master_public_key = reader.read_attribute( c_attribute_master_public_key );
   }
   catch( exception& x )
   {
      throw runtime_error( x.what( ) + string( " when fetching " ) + file_name );
   }
   catch( ... )
   {
      throw runtime_error( "unexpected error occurred when fetching " + file_name );
   }

   return true;
}

void store_repository_entry_record( const string& repository, const string& hash,
 const string& local_hash, const string& local_public_key, const string& master_public_key )
{
   guard g( g_mutex );

   repository_lock repo_lock( repository );

   system_ods_bulk_write ods_bulk_write;

   ods_file_system& ods_fs( system_ods_file_system( ) );

   ods_fs.set_root_folder( c_system_repository_folder );

   string file_name( repository + '.' + base64::encode( hex_decode( hash ), true ) );

   if( !local_hash.empty( ) )
      file_name += '.' + base64::encode( hex_decode( local_hash ), true );

   try
   {
      stringstream sio_data;
      sio_writer writer( sio_data );

      writer.write_attribute( c_attribute_local_hash, local_hash );
      writer.write_attribute( c_attribute_local_public_key, local_public_key );
      writer.write_attribute( c_attribute_master_public_key, master_public_key );

      writer.finish_sections( );

      ods_fs.store_file( file_name, 0, &sio_data );
   }
   catch( exception& x )
   {
      throw runtime_error( x.what( ) + string( " when storing " ) + file_name );
   }
   catch( ... )
   {
      throw runtime_error( "unexpected error occurred when storing " + file_name );
   }
}

bool destroy_repository_entry_record( const string& repository, const string& hash, bool must_exist )
{
   guard g( g_mutex );

   repository_lock repo_lock( repository );

   system_ods_bulk_write ods_bulk_write;

   ods_file_system& ods_fs( system_ods_file_system( ) );

   ods_fs.set_root_folder( c_system_repository_folder );

   string file_name( repository + '.' + base64::encode( hex_decode( hash ), true ) );

   string suffix;
   bool has_file = ods_fs.has_file( file_name, true, &suffix );

   if( !has_file && !must_exist )
      return false;

   ods_fs.remove_file( file_name + suffix );

   return true;
}

size_t count_total_repository_entries(
 const string& repository, date_time* p_dtm, progress* p_progress )
{
   repository_lock repo_lock( repository );

   ods_file_system& ods_fs( system_ods_file_system( ) );

   ods_fs.set_root_folder( c_system_repository_folder );

   string last_key;

   size_t total_entries = 0;
   size_t total_archive_files = files_in_archive( repository );

   size_t num_seconds;

   session_progress_settings( num_seconds, p_progress );

   if( p_progress )
      get_raw_session_variable( get_special_var_name( e_special_var_progress_clear ) );

   while( true )
   {
      system_ods_bulk_read ods_bulk_read;

      date_time now( date_time::local( ) );

      vector< string > repo_entries;
      ods_fs.list_files( repository + '*', repo_entries, false, last_key, last_key.empty( ), 1000 );

      if( repo_entries.empty( ) )
         break;

      if( p_progress )
      {
         uint64_t elapsed = seconds_between( *p_dtm, now );

         if( elapsed >= num_seconds )
            output_repository_progress( p_progress, total_entries, total_archive_files, *p_dtm, now );
      }

      total_entries += repo_entries.size( );

      if( repo_entries.size( ) < 1000 )
         break;

      last_key = repo_entries[ repo_entries.size( ) - 1 ];
   }

   return total_entries;
}

size_t remove_all_repository_entries( const string& repository,
 date_time* p_dtm, progress* p_progress, bool set_session_progress )
{
   bool has_output_progress = false;

   repository_lock repo_lock( repository );

   string archive_path;
   vector< string > paths;

   string all_archives( list_file_archives( e_archive_list_type_minimal, &paths ) );

   if( !all_archives.empty( ) )
   {
      vector< string > archives;

      split( all_archives, archives, '\n' );

      if( paths.size( ) != archives.size( ) )
         throw runtime_error( "unexpected paths.size( ) != archives.size( )" );

      for( size_t i = 0; i < archives.size( ); i++ )
      {
         if( archives[ i ] == repository )
         {
            archive_path = paths[ i ];
            break;
         }
      }
   }

   auto_ptr< temporary_session_variable > ap_temp_archive_path;

   if( !archive_path.empty( ) )
      ap_temp_archive_path.reset( new temporary_session_variable(
       get_special_var_name( e_special_var_blockchain_archive_path ), archive_path ) );

   ods_file_system& ods_fs( system_ods_file_system( ) );

   ods_fs.set_root_folder( c_system_repository_folder );

   size_t total_entries = 0;
   size_t total_archive_files = files_in_archive( repository );

   string last_key;
   vector< string > files_to_remove;

   size_t num_seconds;

   session_progress_settings( num_seconds, p_progress );

   if( p_progress )
      get_raw_session_variable( get_special_var_name( e_special_var_progress_clear ) );

   while( true )
   {
      system_ods_bulk_read ods_bulk_read;

      date_time now( date_time::local( ) );

      vector< string > repo_entries;
      ods_fs.list_files( repository + '*', repo_entries, false, last_key, last_key.empty( ), 100 );

      if( repo_entries.empty( ) )
         break;

      if( p_progress )
      {
         uint64_t elapsed = seconds_between( *p_dtm, now );

         if( elapsed >= num_seconds )
            output_repository_progress( p_progress, total_entries,
             total_archive_files, *p_dtm, now, set_session_progress, &has_output_progress );
      }

      for( size_t i = 0; i < repo_entries.size( ); i++ )
      {
         ++total_entries;

         last_key = repo_entries[ i ];

         files_to_remove.push_back( last_key );
      }

      if( repo_entries.size( ) < 100 )
         break;
   }

   total_entries = files_to_remove.size( );

   if( total_entries )
   {
      system_ods_bulk_write ods_bulk_write( p_progress );

      ods::transaction ods_tx( system_ods_instance( ) );

      for( size_t i = 0; i < files_to_remove.size( ); i++ )
      {
         date_time now( date_time::local( ) );

         ods_fs.remove_file( files_to_remove[ i ] );

         if( p_progress )
         {
            uint64_t elapsed = seconds_between( *p_dtm, now );

            if( elapsed >= num_seconds )
               output_repository_progress( p_progress, ( i + 1 ), total_entries,
                *p_dtm, now, set_session_progress, &has_output_progress, true );
         }
      }

      ods_tx.commit( );
   }

   return total_entries;
}

size_t remove_obsolete_repository_entries( const string& repository,
 date_time* p_dtm, progress* p_progress, bool set_session_progress )
{
   bool has_output_progress = false;

   repository_lock repo_lock( repository );

   string archive_path;
   vector< string > paths;

   string all_archives( list_file_archives( e_archive_list_type_minimal, &paths ) );

   if( !all_archives.empty( ) )
   {
      vector< string > archives;

      split( all_archives, archives, '\n' );

      if( paths.size( ) != archives.size( ) )
         throw runtime_error( "unexpected paths.size( ) != archives.size( )" );

      for( size_t i = 0; i < archives.size( ); i++ )
      {
         if( archives[ i ] == repository )
         {
            archive_path = paths[ i ];
            break;
         }
      }
   }

   auto_ptr< temporary_session_variable > ap_temp_archive_path;

   if( !archive_path.empty( ) )
      ap_temp_archive_path.reset( new temporary_session_variable(
       get_special_var_name( e_special_var_blockchain_archive_path ), archive_path ) );

   ods_file_system& ods_fs( system_ods_file_system( ) );

   ods_fs.set_root_folder( c_system_repository_folder );

   size_t total_entries = 0;
   size_t total_archive_files = files_in_archive( repository );

   string last_key;
   vector< string > files_to_remove;

   size_t num_seconds;

   session_progress_settings( num_seconds, p_progress );

   if( p_progress )
      get_raw_session_variable( get_special_var_name( e_special_var_progress_clear ) );

   while( true )
   {
      system_ods_bulk_read ods_bulk_read;

      date_time now( date_time::local( ) );

      vector< string > repo_entries;
      ods_fs.list_files( repository + '*', repo_entries, false, last_key, last_key.empty( ), 100 );

      if( repo_entries.empty( ) )
         break;

      if( p_progress )
      {
         uint64_t elapsed = seconds_between( *p_dtm, now );

         if( elapsed >= num_seconds )
            output_repository_progress( p_progress, total_entries,
             total_archive_files, *p_dtm, now, set_session_progress, &has_output_progress );
      }

      for( size_t i = 0; i < repo_entries.size( ); i++ )
      {
         ++total_entries;

         last_key = repo_entries[ i ];

         if( last_key.size( ) > 54 )
         {
            string local_hash( last_key.substr( 54 ) );

            local_hash = hex_encode( base64::decode( local_hash, true ) );

            if( !has_file( local_hash ) )
               files_to_remove.push_back( last_key );
         }
         else
         {
            stringstream sio_data;
            ods_fs.get_file( last_key, &sio_data );

            sio_reader reader( sio_data );

            string file_hash( last_key );
            string::size_type pos = file_hash.find( '.' );

            if( pos != string::npos )
            {
               file_hash.erase( 0, pos + 1 );
               file_hash = hex_encode( base64::decode( file_hash, true ) );
            }

            string local_hash( reader.read_attribute( c_attribute_local_hash ) );

            bool found = true;

            if( local_hash.empty( ) && !has_file( file_hash ) )
               found = false;

            if( !local_hash.empty( ) && !has_file( local_hash ) )
               found = false;

            if( !found )
               files_to_remove.push_back( last_key );
         }
      }

      if( repo_entries.size( ) < 100 )
         break;
   }

   total_entries = files_to_remove.size( );

   if( total_entries )
   {
      system_ods_bulk_write ods_bulk_write( p_progress );

      ods::transaction ods_tx( system_ods_instance( ) );

      for( size_t i = 0; i < total_entries; i++ )
      {
         date_time now( date_time::local( ) );

         ods_fs.remove_file( files_to_remove[ i ] );

         if( p_progress )
         {
            uint64_t elapsed = seconds_between( *p_dtm, now );

            if( elapsed >= num_seconds )
               output_repository_progress( p_progress, ( i + 1 ), total_entries,
                *p_dtm, now, set_session_progress, &has_output_progress, true );
         }
      }

      ods_tx.commit( );
   }

   return total_entries;
}
