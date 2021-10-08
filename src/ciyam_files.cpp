// Copyright (c) 2013-2021 CIYAM Developers
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

#ifdef ZLIB_SUPPORT
#  include <zlib.h>
#endif

using namespace std;

namespace
{

const int c_status_info_pad_len = 10;

const int c_min_size_to_compress = 33;

const int c_num_digest_characters = c_sha256_digest_size * 2;

const int c_depth_to_omit_blob_content = 999;

const char c_prefix_wildcard_separator = ':';

const char* const c_important_file_suffix = "!";
const char* const c_time_stamp_tag_prefix = "ts.";

const char* const c_file_archive_path = "path";
const char* const c_file_archive_size_avail = "size_avail";
const char* const c_file_archive_size_limit = "size_limit";
const char* const c_file_archive_status_info = "status_info";

const char* const c_folder_archive_files_folder = "files";

const char* const c_file_archive_status_is_full = "is full";
const char* const c_file_archive_status_bad_write = "bad write";
const char* const c_file_archive_status_bad_access = "bad access";
const char* const c_file_archive_status_status_bad_create = "bad create";

#include "ciyam_constants.h"

mutex g_mutex;

map< string, string > g_tag_hashes;
multimap< string, string > g_hash_tags;

size_t g_total_files = 0;
int64_t g_total_bytes = 0;

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

string construct_file_name_from_hash( const string& hash,
 bool create_directory = false, bool check_hash_pattern = true )
{
   if( hash.length( ) < 3 )
      return string( );

   if( check_hash_pattern )
   {
      regex expr( c_regex_hash_256 );

      if( expr.search( hash ) == string::npos )
         throw runtime_error( "unexpected hash '" + hash + "'" );
   }

   string file_name( get_files_area_dir( ) );

   file_name += '/';
   file_name += lower( hash.substr( 0, 2 ) );

   if( create_directory )
      create_directory_if_not_exists( file_name );

   file_name += '/';
   file_name += lower( hash.substr( 2 ) );

   return file_name;
}

void validate_list( const string& data, bool* p_rc = 0 )
{
   vector< string > list_items;
   split( data, list_items, '\n' );

   for( size_t i = 0; i < list_items.size( ); i++ )
   {
      string next_list_item( list_items[ i ] );
      string::size_type pos = next_list_item.find( ' ' );

      string next_hash( next_list_item.substr( 0, pos ) );

      if( !has_file( next_hash ) && !has_file_been_archived( next_hash ) )
      {
         if( p_rc )
         {
            *p_rc = false;
            return;
         }
         else
            throw runtime_error( "list item file '" + next_list_item.substr( 0, pos ) + "' does not exist" );
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

void validate_hash_with_uncompressed_content( const string& hash, unsigned char* p_data, unsigned long length, const char* p_alt_error_message = 0 )
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

string get_archive_status( const string& path )
{
   string retval( c_okay );

   string tmp_file_name( "~" + uuid( ).as_string( ) );

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

   list_file_archives( true, &paths );

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

bool has_archived_file( ods_file_system& ods_fs, const string& hash, string* p_archive = 0 )
{
   bool retval = false;

   temporary_set_folder tmp_folder( ods_fs, "/" );

   vector< string > paths;

   string all_archives( list_file_archives( true, &paths ) );

   string archive;
   vector< string > archives;

   split( all_archives, archives, '\n' );

   if( paths.size( ) != archives.size( ) )
      throw runtime_error( "unexpected paths.size( ) != archives.size( )" );

   for( size_t i = 0; i < archives.size( ); i++ )
   {
      archive = archives[ i ];

      if( p_archive && !p_archive->empty( ) && archive != *p_archive )
         continue;

      ods_fs.set_root_folder( c_file_archives_folder );

      ods_fs.set_folder( archive );
      ods_fs.set_folder( c_folder_archive_files_folder );

      if( ods_fs.has_file( hash ) )
      {
         retval = true;

         if( p_archive )
            *p_archive = archive;

         break;
      }
   }

   return retval;
}

}

void list_mutex_lock_ids_for_ciyam_files( ostream& outs )
{
   outs << "ciyam_files::g_mutex = " << g_mutex.get_lock_id( ) << '\n';
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

void init_files_area( vector< string >* p_untagged, progress* p_progress, bool remove_invalid_tags )
{
   string cwd( get_cwd( ) );

   string files_area_dir( get_system_variable( get_special_var_name( e_special_var_files_area_dir ) ) );

   set_files_area_dir( files_area_dir );

   // NOTE: If no system variable then setting the directory to an empty string
   // results in it being reset to the default directory (which is now fetched).
   files_area_dir = get_files_area_dir( );

   if( !files_area_dir.empty( ) && files_area_dir[ 0 ] != '/' )
      files_area_dir = cwd + '/' + files_area_dir;

   bool rc = true;

   if( !dir_exists( files_area_dir ) )
      create_directories( files_area_dir + "/" );
   else
   {
      date_time dtm( date_time::local( ) );

      directory_filter df;
      fs_iterator dfsi( files_area_dir, &df );

      bool is_first = true;

      size_t max_num = get_files_area_item_max_num( );
      size_t max_size = get_files_area_item_max_size( );

      do
      {
         file_filter ff;
         fs_iterator fs( dfsi.get_path_name( ), &ff );

         vector< string > files_to_delete;

         while( fs.has_next( ) )
         {
            if( is_first )
            {
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
                  g_hash_tags.insert( make_pair( data, fs.get_name( ) ) );
                  g_tag_hashes.insert( make_pair( fs.get_name( ), data ) );
               }
            }
            else
            {
               string file_path( fs.get_full_name( ) );

               if( file_size( file_path ) > max_size )
                  files_to_delete.push_back( file_path );
               else if( g_total_files >= max_num )
                  files_to_delete.push_back( file_path );
               else
               {
                  ++g_total_files;
                  g_total_bytes += file_size( file_path );

                  if( p_untagged )
                  {
                     string::size_type pos = file_path.find_last_of( "/\\" );
                     if( pos != string::npos && pos >= 2 )
                     {
                        string hash( file_path.substr( pos - 2, 2 ) );
                        hash += file_path.substr( pos + 1 );

                        if( !g_hash_tags.count( hash ) )
                           p_untagged->push_back( hash );
                     }
                  }
               }
            }

            if( p_progress )
            {
               date_time now( date_time::local( ) );

               uint64_t elapsed = seconds_between( dtm, now );

               if( elapsed >= 1 )
               {
                  // FUTURE: This message should be handled as a server string message.
                  p_progress->output_progress( "Processed " + to_string( g_total_files ) + " files..." );

                  dtm = now;
               }
            }
         }

         is_first = false;

         for( size_t i = 0; i < files_to_delete.size( ); i++ )
            file_remove( files_to_delete[ i ] );

      } while( dfsi.has_next( ) );
   }
}

void resync_files_area( vector< string >* p_untagged, progress* p_progress, bool remove_invalid_tags )
{
   guard g( g_mutex );

   g_hash_tags.clear( );
   g_tag_hashes.clear( );

   g_total_bytes = g_total_files = 0;

   init_files_area( p_untagged, p_progress, remove_invalid_tags );
}

void term_files_area( )
{
   // FUTURE: Implementation to be added.
}

string current_time_stamp_tag( bool truncated, size_t days_ahead )
{
   guard g( g_mutex );

   string retval( c_time_stamp_tag_prefix );

   string dummy_timestamp( get_session_variable( get_special_var_name( e_special_var_dummy_timestamp ) ) );

   if( !dummy_timestamp.empty( ) )
   {
      retval += dummy_timestamp;
      set_session_variable( get_special_var_name( e_special_var_dummy_timestamp ), "" );
   }
   else
   {
      date_time dt( date_time::local( ) );

      dt += ( days )days_ahead;

      if( truncated )
         retval += dt.as_string( e_time_format_hhmmssth, false );
      else
      {
         bool okay = false;

         // NOTE: To ensure that the tag is unique try multiple variations (changing the final digits).
         for( int i = 0; i < 1000; i++ )
         {
            string next( dt.as_string( e_time_format_hhmmsstht, false ) + to_comparable_string( i, false, 8 ) );

            if( !has_tag( retval + next ) )
            {
               okay = true;
               retval += next;

               break;
            }
         }

         if( !okay )
            throw runtime_error( "unable to create a unique current timestamp tag" );
      }
   }

   return retval;
}

bool has_tag( const string& name, file_type type )
{
   guard g( g_mutex );

   if( name.empty( ) )
      return false;

   string::size_type pos = name.rfind( '*' );
   map< string, string >::iterator i = ( pos == 0 ? g_tag_hashes.end( ) : g_tag_hashes.lower_bound( name.substr( 0, pos ) ) );

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

         string data( buffer_file( file_name, 1 ) );
         
         if( data.empty( ) )
            throw runtime_error( "unexpected empty file '" + name + "'" );

         unsigned char file_type = ( data[ 0 ] & c_file_type_val_mask );

         if( file_type != c_file_type_val_blob && file_type != c_file_type_val_list )
            throw runtime_error( "invalid file type '0x" + hex_encode( &file_type, 1 ) + "' found in has_tag" );

         if( ( type == e_file_type_blob && file_type != c_file_type_val_blob )
          || ( type == e_file_type_list && file_type != c_file_type_val_list ) )
            return false;

         return true;
      }
   }
}

bool has_file( const string& hash, bool check_is_hash )
{
   guard g( g_mutex );

   if( hash.empty( ) )
      return false;
   else
   {
      string file_name( construct_file_name_from_hash( hash, false, check_is_hash ) );

      return file_exists( file_name );
   }
}

int64_t file_bytes( const string& hash, bool blobs_for_lists )
{
   guard g( g_mutex );

   string file_name( construct_file_name_from_hash( hash ) );

   long file_size = 0;

   string data( buffer_file( file_name, 1, &file_size ) );

   unsigned char file_type = ( data[ 0 ] & c_file_type_val_mask );

   bool is_encrypted = ( data[ 0 ] & c_file_type_val_encrypted );
   bool is_compressed = ( data[ 0 ] & c_file_type_val_compressed );

   if( blobs_for_lists && file_type == c_file_type_val_list )
      file_size = 0;

   if( !is_encrypted && blobs_for_lists && file_type == c_file_type_val_list )
   {
      data = buffer_file( file_name );

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
         split( final_data.substr( 1 ), list_items, '\n' );

         for( size_t i = 0; i < list_items.size( ); i++ )
         {
            string next( list_items[ i ] );
            string::size_type pos = next.find( ' ' );

            file_size += file_bytes( next.substr( 0, pos ), true );
         }
      }
   }

   return file_size;
}

string file_type_info( const string& tag_or_hash,
 file_expansion expansion, int max_depth, int indent,
 bool add_size, const char* p_prefix, bool allow_all_after, bool output_total_blob_size )
{
   guard g( g_mutex );

   string hash, file_name;

   int depth = max_depth;
   bool output_last_only = false;
   bool include_header_suffix = false;

   string header_suffix;
   string use_tag_or_hash( tag_or_hash );
   string files_area_dir( get_files_area_dir( ) );

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

   if( file_exists( files_area_dir + '/' + use_tag_or_hash ) )
   {
      hash = tag_file_hash( use_tag_or_hash );
      file_name = construct_file_name_from_hash( hash );
   }
   else
   {
      bool is_base64 = false;

      if( use_tag_or_hash.length( ) != c_num_digest_characters )
          base64::validate( use_tag_or_hash, &is_base64 );

      hash = !is_base64 ? use_tag_or_hash : base64_to_hex( use_tag_or_hash );

      if( hash.length( ) == c_num_digest_characters )
         file_name = construct_file_name_from_hash( hash, false, false );
   }

   if( !file_exists( file_name ) && indent && has_file_been_archived( hash ) )
      retrieve_file_from_archive( hash, current_time_stamp_tag( ) );

   if( !file_exists( file_name ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "File '" + tag_or_hash + "' was not found." );

   long file_size = 0;
   long max_to_buffer = 0;

   string prefix;
   if( p_prefix )
      prefix = string( p_prefix );

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
      throw runtime_error( "unexpected empty file '" + use_tag_or_hash + "'" );

   unsigned char file_type = ( data[ 0 ] & c_file_type_val_mask );

   bool is_core = ( data[ 0 ] & c_file_type_val_extra_core );
   bool is_encrypted = ( data[ 0 ] & c_file_type_val_encrypted );
   bool is_compressed = ( data[ 0 ] & c_file_type_val_compressed );

   if( file_type != c_file_type_val_blob && file_type != c_file_type_val_list )
      throw runtime_error( "invalid file type '0x" + hex_encode( &file_type, 1 ) + "' found in file_info" );

   if( !is_encrypted && max_to_buffer == 1 && file_type == c_file_type_val_list )
   {
      data = buffer_file( file_name );

      if( data.size( ) <= 1 )
         throw runtime_error( "unexpected truncated file content for '" + use_tag_or_hash + "'" );
      else if( !get_session_variable( buffered_var_name ).empty( ) )
         set_session_variable( buffered_var_name, increment_special );
   }

   if( !is_encrypted && !is_compressed && ( file_type == c_file_type_val_list
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

                  if( is_valid_utf8( blob_info ) )
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
         string list_info( final_data.substr( 1 ) );

         vector< string > list_items;
         split( list_info, list_items, '\n' );

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
                || ( !is_wildcard_prefix && prefix.find( next_name ) == 0 )
                || ( is_wildcard_prefix && wildcard_match( prefix, next_name ) ) )
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

                  string additional( file_type_info( next_tag_or_hash, expansion, max_depth, indent + 1,
                   add_size, ( allow_all ? 0 : p_prefix ), allow_all_after, output_total_blob_size ) );

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

string create_raw_file( const string& data, bool compress, const char* p_tag, bool* p_is_existing, const char* p_hash )
{
   guard g( g_mutex );

   if( data.empty( ) )
      throw runtime_error( "cannot create a raw file with empty data" );

   bool file_extra_is_core = false;

   unsigned char file_type = ( data[ 0 ] & c_file_type_val_mask );
   unsigned char file_extra = ( data[ 0 ] & c_file_type_val_extra_mask );

   if( file_extra & c_file_type_val_extra_core )
      file_extra_is_core = true;

   if( file_type != c_file_type_val_blob && file_type != c_file_type_val_list )
      throw runtime_error( "invalid file type '0x" + hex_encode( &file_type, 1 ) + "' for raw file creation" );

   string final_data( data );

   bool is_encrypted = ( data[ 0 ] & c_file_type_val_encrypted );
   bool is_compressed = ( data[ 0 ] & c_file_type_val_compressed );

#ifdef ZLIB_SUPPORT
   session_file_buffer_access file_buffer;

   if( !is_encrypted && is_compressed )
   {
      unsigned long size = final_data.size( ) - 1;
      unsigned long usize = file_buffer.get_size( );

      if( uncompress( ( Bytef * )file_buffer.get_buffer( ), &usize, ( Bytef * )&final_data[ 1 ], size ) != Z_OK )
         throw runtime_error( "invalid content for create_raw_file (bad compressed or uncompressed too large)" );

      compress = true;
      is_compressed = false;

      final_data = ( data[ 0 ] & ~c_file_type_val_compressed );
      final_data += string( ( const char* )file_buffer.get_buffer( ), usize );
   }
#else
   if( is_compressed )
      throw runtime_error( "create_raw_file doesn't support compressed files (without ZLIB support)" );
#endif

   string hash( p_hash ? string( p_hash ) : sha256( final_data ).get_digest_as_string( ) );

   string file_name( construct_file_name_from_hash( hash, true ) );

   if( file_type != c_file_type_val_blob )
      validate_list( final_data.substr( 1 ) );

#ifdef ZLIB_SUPPORT
   if( compress && !is_compressed && final_data.size( ) >= c_min_size_to_compress )
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
            final_data[ 0 ] |= c_file_type_val_compressed;

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
         
      if( g_total_files >= get_files_area_item_max_num( ) )
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

      ++g_total_files;
      g_total_bytes += final_data.size( );
   }
   else if( p_is_existing )
      *p_is_existing = true;

   string tag_name;
   if( p_tag )
      tag_name = string( p_tag );

   if( !tag_name.empty( )
    && tag_name != string( c_important_file_suffix ) )
      tag_file( tag_name, hash );
   else if( !was_existing && !file_extra_is_core )
      tag_file( current_time_stamp_tag( ) + tag_name, hash );

   return hash;
}

string create_raw_file_with_extras( const string& data,
 vector< pair< string, string > >& extras, bool compress, const char* p_tag )
{
   guard g( g_mutex );

   string retval;

   if( g_total_files + !data.empty( ) + extras.size( ) >= get_files_area_item_max_num( ) )
      throw runtime_error( "maximum file area item limit has been reached" );

   bool is_existing = false;

   if( !data.empty( ) )
      retval = create_raw_file( data, compress, p_tag, &is_existing );

   // NOTE: It is being assumed that "extras" should not be larger than the main file
   // so that assuming the main file is created there should be no risk that the max.
   // file storage capacity will be exceeded creating the extra files.
   if( !is_existing )
   {
      vector< string > all_hashes;
      all_hashes.push_back( retval );

      for( size_t i = 0; i < extras.size( ); i++ )
      {
         if( extras[ i ].first.empty( ) )
            delete_file( extras[ i ].second );
         else
         {
            // NOTE: If a core blob or list is being added then the hash of the
            // main file being added can be expanded using @0 as well as hashes
            // of other extra files using @1..@n.
            if( extras[ i ].first[ 0 ] == c_file_type_char_core_blob
             || extras[ i ].first[ 0 ] == c_file_type_char_core_list )
            {
               string details( extras[ i ].first.substr( 1 ) );

               if( details.find( '@' ) != string::npos )
               {
                  for( size_t j = all_hashes.size( ) - 1; ; j-- )
                  {
                     string str( "@" );
                     str += to_string( j );

                     replace( details, str, all_hashes[ j ] );

                     if( j == 0 || details.find( '@' ) == string::npos )
                        break;
                  }
               }

               extras[ i ].first.erase( 1 );
               extras[ i ].first += details;
            }

            regex expr( c_regex_hash_256 );

            // NOTE: If the first part of the extra is a file hash then
            // the second part is split up and applied as tags for this
            // file (which is expected to exist).
            if( expr.search( extras[ i ].first ) == 0 )
            {
               vector< string > tags;
               split( extras[ i ].second, tags, '\n' );

               for( size_t j = 0; j < tags.size( ); j++ )
                  tag_file( tags[ j ], extras[ i ].first );
            }
            else
            {
               string tag( extras[ i ].second );
               string secondary_tags;

               string::size_type pos = tag.find( '\n' );
               if( pos != string::npos )
               {
                  secondary_tags = tag.substr( pos + 1 );
                  tag.erase( pos );
               }

               string next_hash = create_raw_file( extras[ i ].first, compress, tag.c_str( ) );
               all_hashes.push_back( next_hash );

               if( !secondary_tags.empty( ) )
               {
                  vector< string > all_secondary_tags;
                  split( secondary_tags, all_secondary_tags, '\n' );

                  for( size_t i = 0; i < all_secondary_tags.size( ); i++ )
                     tag_file( all_secondary_tags[ i ], next_hash );
               }
            }
         }
      }
   }

   return retval;
}

string create_list_file( const string& add_tags, const string& del_items,
 bool sort_items, const string& tag_or_hash, const string& new_tag, const string& old_tag )
{
   guard g( g_mutex );

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

         if( hashes_to_add.count( next_hash ) )
            throw runtime_error( "attempt to add '" + next_tag + "' more than once" );

         hashes_to_add.insert( next_hash );
      }
   }

   vector< string > items;

   if( !data.empty( ) )
      split( data, items, '\n' );

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
         if( hashes_to_add.count( next_hash ) )
            throw runtime_error( "invalid attempt to add existing list item '" + next_hash + "'" );

         new_items.push_back( next );
      }
      else
      {
         for( size_t j = 0; j < items_to_remove.size( ); j++ )
         {
            string next_to_remove( items_to_remove[ j ] );

            if( next_hash != next_to_remove && next_name != next_to_remove )
               new_items.push_back( next );
         }
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
   guard g( g_mutex );

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

                  split( data, items, '\n' );

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

                  split( data, items, '\n' );
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
               split( data, items, '\n' );

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

void tag_del( const string& name, bool unlink, bool auto_tag_with_time, bool remove_tag_file )
{
   guard g( g_mutex );

   string::size_type pos = name.find_first_of( "*?" );

   if( pos == string::npos )
   {
      string tag_file_name( get_files_area_dir( ) );

      tag_file_name += "/" + name;

      if( remove_tag_file )
         file_remove( tag_file_name );

      if( g_tag_hashes.count( name ) )
      {
         string hash = g_tag_hashes[ name ];
         g_tag_hashes.erase( name );

         // NOTE: Need to also remove the matching entry in the hash tags multimap.
         multimap< string, string >::iterator i;
         for( i = g_hash_tags.lower_bound( hash ); i != g_hash_tags.end( ); ++i )
         {
            if( i->first != hash )
               break;

            if( i->second == name )
            {
               g_hash_tags.erase( i );
               break;
            }
         }

         if( unlink && !g_hash_tags.count( hash ) )
            delete_file( hash );
         else if( auto_tag_with_time && !g_hash_tags.count( hash ) )
            tag_file( current_time_stamp_tag( ), hash );
      }
   }
   else
   {
      if( name == "*" )
         throw runtime_error( "invalid attempt to delete all file system tags (use ** if really wanting to do this)" );

      string prefix( name.substr( 0, pos ) );
      map< string, string >::iterator i = g_tag_hashes.lower_bound( prefix );

      vector< string > matching_tag_names;
      for( ; i != g_tag_hashes.end( ); ++i )
      {
         if( wildcard_match( name, i->first ) )
            matching_tag_names.push_back( i->first );

         if( i->first.length( ) < prefix.length( ) || i->first.substr( 0, prefix.length( ) ) != prefix )
            break;
      }

      for( size_t i = 0; i < matching_tag_names.size( ); i++ )
         tag_del( matching_tag_names[ i ], unlink, auto_tag_with_time );
   }
}

void tag_file( const string& name, const string& hash )
{
   guard g( g_mutex );

   if( name != get_special_var_name( e_special_var_none ) )
   {
      string file_name( construct_file_name_from_hash( hash ) );

      if( !file_exists( file_name ) )
         throw runtime_error( hash + " was not found" );

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
            throw runtime_error( "invalid file tag name '" + name + "'" );

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
         // instead a "current time stamp" tag.
         if( tag_name.length( ) && tag_name[ tag_name.length( ) - 1 ] == '?' )
            tag_name = current_time_stamp_tag( );
      }

      if( !tag_name.empty( ) )
      {
         string all_tags( get_hash_tags( hash ) );

         string ts_tag_to_remove;
         vector< string > tags;

         bool remove_tag_file = true;

         // NOTE: If the file has just a time stamp tag then this will be removed.
         split( all_tags, tags, '\n' );
         if( tags.size( ) == 1 && tags[ 0 ].find( c_time_stamp_tag_prefix ) == 0 )
            ts_tag_to_remove = tags[ 0 ];

         string prefix( get_files_area_dir( ) );

         // NOTE: If an old time stamp tag is being replaced by a new one then instead
         // of creating a new file and removing the old one just rename the old to new.
         if( !ts_tag_to_remove.empty( ) && tag_name.find( c_time_stamp_tag_prefix ) == 0 )
         {
            remove_tag_file = false;
            file_rename( prefix + '/' + ts_tag_to_remove, prefix + '/' + tag_name );
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

         g_hash_tags.insert( make_pair( hash, tag_name ) );
         g_tag_hashes.insert( make_pair( tag_name, hash ) );

         if( !ts_tag_to_remove.empty( ) )
            tag_del( ts_tag_to_remove, false, false, remove_tag_file );
      }
   }
}

string get_hash_tags( const string& hash )
{
   guard g( g_mutex );

   string retval;
   set< string > tags_found;

   multimap< string, string >::iterator i = g_hash_tags.lower_bound( hash );

   for( ; i != g_hash_tags.end( ); ++i )
   {
      if( i->first != hash )
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

string tag_file_hash( const string& name )
{
   guard g( g_mutex );

   string retval;

   // NOTE: If the name is just "*" then return the hashes of all files that have been tagged or
   // if is just "?" then will instead return all the hashes of files that have not been tagged.
   if( name == "*" )
   {
      set< string > hashes;

      for( map< string, string >::iterator i = g_tag_hashes.begin( ); i != g_tag_hashes.end( ); ++i )
         hashes.insert( i->second );

      for( set< string >::iterator i = hashes.begin( ); i != hashes.end( ); ++i )
      {
         if( i != hashes.begin( ) )
            retval += '\n';
         retval += *i;
      }
   }
   else if( name == "?" )
   {
      vector< string > untagged_hashes;

      resync_files_area( &untagged_hashes );

      for( size_t i = 0; i < untagged_hashes.size( ); i++ )
      {
         if( i > 0 )
            retval += '\n';
         retval += untagged_hashes[ i ];
      }
   }
   else
   {
      string::size_type pos = name.rfind( '*' );
      map< string, string >::iterator i = g_tag_hashes.lower_bound( name.substr( 0, pos ) );

      if( i == g_tag_hashes.end( ) || ( pos == string::npos && i->first != name ) )
         // FUTURE: This message should be handled as a server string message.
         throw runtime_error( "Tag '" + name + "' not found." );

      retval = i->second;
   }

   return retval;
}

string extract_tags_from_lists( const string& tag_or_hash, const string& prefix, int depth, int level )
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

   if( is_list && !data.empty( ) )
   {
      vector< string > list_items;
      split( data, list_items, '\n' );

      for( size_t i = 0; i < list_items.size( ); i++ )
      {
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
            string extra( extract_tags_from_lists( next_hash, prefix, depth, level + 1 ) );

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

string list_file_tags( const string& pat,
 const char* p_excludes, size_t max_tags, int64_t max_bytes,
 int64_t* p_min_bytes, deque< string >* p_hashes, bool include_multiples )
{
   guard g( g_mutex );

   string retval;

   size_t num_tags = 0;
   int64_t min_bytes = 0;
   int64_t num_bytes = 0;

   string all_excludes( p_excludes ? p_excludes : "" );
   vector< string > excludes;

   if( !all_excludes.empty( ) )
      split( all_excludes, excludes );

   if( !pat.empty( ) )
   {
      string::size_type pos = pat.find_first_of( "*?" );
      string prefix = pat.substr( 0, pos );

      map< string, string >::iterator i = g_tag_hashes.lower_bound( prefix );

      for( ; i != g_tag_hashes.end( ); ++i )
      {
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
               multimap< string, string >::iterator j = g_hash_tags.lower_bound( i->second );

               if( j != g_hash_tags.end( ) && ++j != g_hash_tags.end( ) )
               {
                  if( j->first == i->second )
                     continue;
               }
            }

            int64_t next_bytes = file_bytes( i->second );

            if( max_bytes && num_bytes + next_bytes > max_bytes )
               continue;

            ++num_tags;

            if( !min_bytes || next_bytes < min_bytes )
               min_bytes = next_bytes;

            num_bytes += next_bytes;

            if( !retval.empty( ) )
               retval += "\n";
            retval += i->first;

            if( p_hashes )
               p_hashes->push_back( i->second );
         }

         if( max_tags && num_tags >= max_tags )
            break;

         if( i->first.length( ) < prefix.length( ) || i->first.substr( 0, prefix.length( ) ) != prefix )
            break;
      }
   }
   else
   {
      for( map< string, string >::iterator i = g_tag_hashes.begin( ); i != g_tag_hashes.end( ); ++i )
      {
         // NOTE: Skip matching tags for files that have more than one tag.
         if( !include_multiples )
         {
            multimap< string, string >::iterator j = g_hash_tags.lower_bound( i->second );

            if( j != g_hash_tags.end( ) && ++j != g_hash_tags.end( ) )
            {
               if( j->first == i->second )
                  continue;
            }
         }

         int64_t next_bytes = file_bytes( i->second );

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
            p_hashes->push_back( i->second );

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

void crypt_file( const string& tag_or_hash,
 const string& password, bool recurse, progress* p_progress, date_time* p_dtm, long* p_total )
{
   string hash( tag_or_hash );

   if( has_tag( tag_or_hash ) )
      hash = tag_file_hash( tag_or_hash );

   string file_name( construct_file_name_from_hash( hash ) );

   if( !file_exists( file_name ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( hash + " was not found." );

   string file_data( buffer_file( file_name ) );

   if( file_data.empty( ) )
      throw runtime_error( "unexpected empty file content for '" + hash + "'" );

   unsigned char flags = file_data[ 0 ];

   unsigned char file_type = ( flags & c_file_type_val_mask );

   bool is_encrypted = ( flags & c_file_type_val_encrypted );
   bool is_compressed = ( flags & c_file_type_val_compressed );
   bool is_no_encrypt = ( flags & c_file_type_val_no_encrypt );

   if( is_no_encrypt )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Attempt to encrypt file flagged with 'no encrypt'." );

   if( p_total )
      ++*p_total;

   if( p_dtm && p_progress )
   {
      date_time now( date_time::local( ) );
      uint64_t elapsed = seconds_between( *p_dtm, now );

      if( elapsed >= 1 )
      {
         *p_dtm = now;

         if( !p_total )
            p_progress->output_progress( "." );
         else
            // FUTURE: This message should be handled as a server string message.
            p_progress->output_progress( "Processed " + to_string( *p_total ) + " files..." );
      }
   }

   if( !is_encrypted )
   {
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

      stringstream ss( file_data.substr( 1 ) );

      // NOTE: Use the file content hash as salt.
      crypt_stream( ss, password + hash );

      string new_file_data( file_data.substr( 0, 1 ) );

      new_file_data += ss.str( );

      new_file_data[ 0 ] |= c_file_type_val_encrypted;

      write_file( file_name, new_file_data );

      if( recurse && file_type == c_file_type_val_list )
      {
         string list_info( uncompressed_data.substr( 1 ) );

         if( !list_info.empty( ) )
         {
            vector< string > list_items;
            split( list_info, list_items, '\n' );

            for( size_t i = 0; i < list_items.size( ); i++ )
            {
               string next( list_items[ i ] );
               string::size_type pos = next.find( ' ' );

               crypt_file( next.substr( 0, pos ), password, recurse, p_progress, p_dtm, p_total );
            }
         }
      }
   }
   else
   {
      stringstream ss( file_data.substr( 1 ) );

      // NOTE: Use the file content hash as salt.
      crypt_stream( ss, password + hash );

      file_data.erase( 1 );
      file_data += ss.str( );

      file_data[ 0 ] &= ~c_file_type_val_encrypted;

      string uncompressed_data( file_data );

      // FUTURE: This message should be handled as a server string message.
      string bad_hash_error( "Invalid password to decrypt file '" + tag_or_hash + "'." );

#ifdef ZLIB_SUPPORT
      if( is_compressed && file_data.size( ) > 1 )
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

      write_file( file_name, file_data );

      if( recurse && file_type == c_file_type_val_list )
      {
         string list_info( uncompressed_data.substr( 1 ) );

         if( !list_info.empty( ) )
         {
            vector< string > list_items;
            split( list_info, list_items, '\n' );

            for( size_t i = 0; i < list_items.size( ); i++ )
            {
               string next( list_items[ i ] );
               string::size_type pos = next.find( ' ' );

               crypt_file( next.substr( 0, pos ), password, recurse, p_progress, p_dtm, p_total );
            }
         }
      }
   }
}

void fetch_file( const string& hash, tcp_socket& socket, progress* p_progress )
{
   string tmp_file_name( "~" + uuid( ).as_string( ) );
   string file_name( construct_file_name_from_hash( hash, false, false ) );

   try
   {
      // NOTE: As the file may end up being deleted whilst it is being
      // transferred it is copied to a temporary file which is instead
      // used for the transfer (and deleted afterwards).
      if( !file_name.empty( ) )
      {
         guard g( g_mutex );

         if( !file_exists( file_name ) )
            // FUTURE: This message should be handled as a server string message.
            throw runtime_error( "File '" + hash + "' was not found." );

         file_copy( file_name, tmp_file_name );
      }

      file_transfer( tmp_file_name, socket,
       e_ft_direction_send, get_files_area_item_max_size( ),
       c_response_okay_more, c_file_transfer_initial_timeout,
       c_file_transfer_line_timeout, c_file_transfer_max_line_size, 0, 0, 0, p_progress );

      file_remove( tmp_file_name );
   }
   catch( ... )
   {
      file_remove( tmp_file_name );

      throw;
   }
}

void store_file( const string& hash, tcp_socket& socket,
 const char* p_tag, progress* p_progress, bool allow_core_file, size_t max_bytes )
{
   string tmp_file_name( "~" + uuid( ).as_string( ) );
   string file_name( construct_file_name_from_hash( hash, true ) );

   bool existing = false;
   int64_t existing_bytes = 0;

   bool is_in_blacklist = false;

   bool file_extra_is_core = false;

   if( !max_bytes || max_bytes > get_files_area_item_max_size( ) )
      max_bytes = get_files_area_item_max_size( );

   if( !file_name.empty( ) )
   {
      guard g( g_mutex );

      existing = file_exists( file_name );

      if( existing )
         existing_bytes = file_size( file_name );
   }

   try
   {
      session_file_buffer_access file_buffer;

      file_transfer( tmp_file_name,
       socket, e_ft_direction_recv, max_bytes,
       ( existing ? c_response_okay_skip : c_response_okay_more ),
       c_file_transfer_initial_timeout, c_file_transfer_line_timeout, c_file_transfer_max_line_size,
       0, file_buffer.get_buffer( ), file_buffer.get_size( ), p_progress, ( !existing ? 0 : c_response_okay_skip ) );

      if( !existing )
      {
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
            unsigned long size = file_size( tmp_file_name ) - 1;
            unsigned long usize = file_buffer.get_size( ) - size;

            if( uncompress( ( Bytef * )&file_buffer.get_buffer( )[ size + 1 ],
             &usize, ( Bytef * )&file_buffer.get_buffer( )[ 1 ], size ) != Z_OK )
               throw runtime_error( "invalid content for '" + hash + "' (bad compressed or uncompressed too large)" );

            if( usize + 1 > max_bytes )
               throw runtime_error( "uncompressed file size exceeds maximum permitted file size" );

            file_buffer.get_buffer( )[ size ] = file_buffer.get_buffer( )[ 0 ];
            validate_hash_with_uncompressed_content( hash, &file_buffer.get_buffer( )[ size ], usize + 1 );

            bool rc = true;

            if( file_type != c_file_type_val_blob )
               validate_list( ( const char* )&file_buffer.get_buffer( )[ size + 1 ], &rc );

            if( !rc )
               throw runtime_error( "invalid 'list' file" );
         }
#endif

         bool rc = true;

         if( !is_encrypted && !is_compressed && file_type != c_file_type_val_blob )
            validate_list( ( const char* )&file_buffer.get_buffer( )[ 1 ], &rc );

         if( !rc )
            throw runtime_error( "invalid 'list' file" );

         if( !is_encrypted && !is_compressed )
            validate_hash_with_uncompressed_content( hash, tmp_file_name );

         if( rc )
         {
            guard g( g_mutex );

            if( !existing )
               is_in_blacklist = file_has_been_blacklisted( hash );

            if( !existing && !is_in_blacklist && g_total_files >= get_files_area_item_max_num( ) )
            {
               // NOTE: First attempt to relegate an existing file in order to make room.
               relegate_time_stamped_files( "", "", 1, 0, true );

               if( g_total_files >= get_files_area_item_max_num( ) )
                  // FUTURE: This message should be handled as a server string message.
                  throw runtime_error( "Maximum file area item limit has been reached." );
            }

            if( !existing && !is_in_blacklist )
            {
#ifndef ZLIB_SUPPORT
               file_copy( tmp_file_name, file_name );
#else
               bool has_written = false;
               unsigned long size = file_size( tmp_file_name ) - 1;

               if( !is_no_compress
                && !is_encrypted && !is_compressed && size >= c_min_size_to_compress )
               {
                  unsigned long csize = file_buffer.get_size( );

                  int rc = compress2(
                   ( Bytef * )&file_buffer.get_buffer( )[ size + 1 ],
                   &csize, ( Bytef * )&file_buffer.get_buffer( )[ 1 ], size, 9 ); // i.e. 9 is for maximum compression

                  if( rc == Z_OK )
                  {
                     if( csize < size )
                     {
                        has_written = true;
                        is_compressed = true;

                        file_buffer.get_buffer( )[ size ] = file_buffer.get_buffer( )[ 0 ] | c_file_type_val_compressed;

                        write_file( file_name, ( unsigned char* )&file_buffer.get_buffer( )[ size ], csize + 1 );
                     }
                  }
                  else if( rc != Z_BUF_ERROR )
                     throw runtime_error( "unexpected compression error in store_file" );
               }

               if( !has_written )
                  file_copy( tmp_file_name, file_name );
#endif
            }
         }

         if( !existing && !is_in_blacklist )
            ++g_total_files;

         if( !is_in_blacklist )
         {
            g_total_bytes -= existing_bytes;
            g_total_bytes += file_size( file_name );
         }
      }

      file_remove( tmp_file_name );
   }
   catch( ... )
   {
      file_remove( tmp_file_name );
      throw;
   }

   if( !is_in_blacklist )
   {
      string tag_name;
      if( p_tag )
         tag_name = string( p_tag );

      if( !tag_name.empty( )
       && tag_name != string( c_important_file_suffix ) )
         tag_file( tag_name, hash );
      else if( !file_extra_is_core )
         tag_file( current_time_stamp_tag( ) + tag_name, hash );
   }
}

void delete_file( const string& hash, bool even_if_tagged, bool ignore_not_found )
{
   guard g( g_mutex );

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

void delete_file_tree( const string& hash )
{
   string all_hashes( file_type_info( hash, e_file_expansion_recursive_hashes ) );

   vector< string > hashes;
   split( all_hashes, hashes, '\n' );

   for( size_t i = 0; i < hashes.size( ); i++ )
      delete_file( hashes[ i ], true, true );
}

void delete_files_for_tags( const string& pat )
{
   guard g( g_mutex );

   string tags( list_file_tags( pat ) );

   if( !tags.empty( ) )
   {
      vector< string > all_tags;
      split( tags, all_tags, '\n' );

      set< string > hashes;

      for( size_t i = 0; i < all_tags.size( ); i++ )
         hashes.insert( tag_file_hash( all_tags[ i ] ) );

      for( set< string >::iterator i = hashes.begin( ); i != hashes.end( ); ++i )
         delete_file( *i );
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

void fetch_temp_file( const string& name, tcp_socket& socket, progress* p_progress )
{
   file_transfer( name, socket,
    e_ft_direction_send, get_files_area_item_max_size( ),
    c_response_okay_more, c_file_transfer_initial_timeout,
    c_file_transfer_line_timeout, c_file_transfer_max_line_size, 0, 0, 0, p_progress );
}

void store_temp_file( const string& name, tcp_socket& socket, progress* p_progress )
{
   file_transfer( name, socket,
    e_ft_direction_recv, get_files_area_item_max_size( ),
    c_response_okay_more, c_file_transfer_initial_timeout,
    c_file_transfer_line_timeout, c_file_transfer_max_line_size, 0, 0, 0, p_progress );
}

bool temp_file_is_identical( const string& temp_name, const string& hash )
{
   string file_name( construct_file_name_from_hash( hash ) );

   return files_are_identical( temp_name, file_name );
}

string extract_file( const string& hash, const string& dest_file_name, unsigned char check_file_type_and_extra, bool* p_is_list )
{
   guard g( g_mutex );

   string file_name( construct_file_name_from_hash( hash ) );

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

      bool is_compressed = ( data[ 0 ] & c_file_type_val_compressed );

      session_file_buffer_access file_buffer;

      unsigned long size = file_size( file_name ) - 1;

      size_t offset = 1;

#ifdef ZLIB_SUPPORT
      if( is_compressed )
      {
         unsigned long usize = file_buffer.get_size( ) - size;

         if( uncompress(
          ( Bytef * )&file_buffer.get_buffer( )[ 0 ], &usize, ( Bytef * )&data[ offset ], size ) != Z_OK )
            throw runtime_error( "invalid content for '" + hash + "' (bad compressed or uncompressed too large)" );

         size = usize;
      }
#endif

      if( !is_compressed )
         memcpy( file_buffer.get_buffer( ), &data[ offset ], size );

      if( !dest_file_name.empty( ) )
         write_file( dest_file_name, file_buffer.get_buffer( ), size );

      data = string( ( const char* )file_buffer.get_buffer( ), size );
   }

   return data;
}

void add_file_archive( const string& name, const string& path, int64_t size_limit )
{
   guard g( g_mutex );

   if( size_limit < 0 )
      throw runtime_error( "unexpected negative size_limit provided to add_file_archive" );

   string cwd( get_cwd( ) );
   string tmp_file_name( "~" + uuid( ).as_string( ) );

   if( path.empty( )
    || path[ path.length( ) - 1 ] == '\\' || path[ path.length( ) - 1 ] == '/' )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Invalid path '" + path + "' for 'add_file_archive'." );

   if( path_already_used_in_archive( path ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "An archive with the path '" + path + "' already exists." );

   int64_t min_limit = get_files_area_item_max_size( ) * 10;

   if( size_limit < min_limit )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Archive minimum size must be at least " + to_string( min_limit ) + " bytes." );

   string status_info( get_archive_status( path ) );

   ods::bulk_write bulk_write( ciyam_ods_instance( ) );
   ods_file_system& ods_fs( ciyam_ods_file_system( ) );

   ods::transaction ods_tx( ciyam_ods_instance( ) );

   ods_fs.set_root_folder( c_file_archives_folder );

   if( ods_fs.has_folder( name ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Archive '" + name + "' already exists." );

   ods_fs.add_folder( name );
   ods_fs.set_folder( name );

   ods_fs.store_as_text_file( c_file_archive_path, path );
   ods_fs.store_as_text_file( c_file_archive_size_avail, size_limit );
   ods_fs.store_as_text_file( c_file_archive_size_limit, size_limit );

   ods_fs.store_as_text_file( c_file_archive_status_info, status_info, c_status_info_pad_len );

   ods_fs.add_folder( c_folder_archive_files_folder );

   ods_tx.commit( );
}

void remove_file_archive( const string& name, bool destroy_files )
{
   guard g( g_mutex );

   ods::bulk_write bulk_write( ciyam_ods_instance( ) );
   ods_file_system& ods_fs( ciyam_ods_file_system( ) );

   ods_fs.set_root_folder( c_file_archives_folder );

   if( !ods_fs.has_folder( name ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Archive '" + name + "' not found." );
   else
   {
      ods::transaction ods_tx( ciyam_ods_instance( ) );

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
            ods_fs.set_folder( c_folder_archive_files_folder );

            vector< string > file_names;
            ods_fs.list_files( file_names );

            for( size_t i = 0; i < file_names.size( ); i++ )
               file_remove( path + '/' + file_names[ i ] );

            ods_fs.set_root_folder( c_file_archives_folder );
         }
      }

      ods_fs.remove_folder( name, 0, true );

      ods_tx.commit( );
   }
}

void repair_file_archive( const string& name )
{
   guard g( g_mutex );

   ods::bulk_write bulk_write( ciyam_ods_instance( ) );
   ods_file_system& ods_fs( ciyam_ods_file_system( ) );

   ods_fs.set_root_folder( c_file_archives_folder );

   if( !ods_fs.has_folder( name ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Archive '" + name + "' not found." );
   else
   {
      ods::transaction ods_tx( ciyam_ods_instance( ) );

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
         ods_fs.remove_folder( c_folder_archive_files_folder, 0, true );

         ods_fs.add_folder( c_folder_archive_files_folder );
         ods_fs.set_folder( c_folder_archive_files_folder );

         // NOTE: Iterate through the files in the path adding all
         // that have names that appear to be valid SHA256 hashes.
         file_filter ff;
         fs_iterator fs( path, &ff );

         while( fs.has_next( ) )
         {
            string name( fs.get_name( ) );
            regex expr( c_regex_hash_256 );

            if( expr.search( name ) == 0 )
            {
               ods_fs.add_file( name, c_file_zero_length );
               size_used += file_size( fs.get_full_name( ) );
            }
         }

         ods_fs.set_folder( ".." );

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

void archives_status_update( const string& name )
{
   guard g( g_mutex );

   ods::bulk_write bulk_write( ciyam_ods_instance( ) );
   ods_file_system& ods_fs( ciyam_ods_file_system( ) );

   ods_fs.set_root_folder( c_file_archives_folder );

   vector< string > names;
   ods_fs.list_folders( names );

   ods::transaction ods_tx( ciyam_ods_instance( ) );

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

   ods::bulk_read bulk_read( ciyam_ods_instance( ) );
   ods_file_system& ods_fs( ciyam_ods_file_system( ) );

   ods_fs.set_root_folder( c_file_blacklist_folder );

   if( ods_fs.has_file( hash ) )
      retval = true;

   return retval;
}

string list_file_archives( bool minimal, vector< string >* p_paths, int64_t min_avail, bool stop_after_first )
{
   guard g( g_mutex );

   string retval;
   vector< string > names;

   auto_ptr< ods::bulk_read > ap_bulk;

   if( !ciyam_ods_instance( ).is_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_read( ciyam_ods_instance( ) ) );

   ods_file_system& ods_fs( ciyam_ods_file_system( ) );

   ods_fs.set_root_folder( c_file_archives_folder );

   ods_fs.list_folders( names );

   for( size_t i = 0; i < names.size( ); i++ )
   {
      string next( names[ i ] );

      if( !retval.empty( ) )
         retval += '\n';

      ods_fs.set_folder( next );

      string path;
      ods_fs.fetch_from_text_file( c_file_archive_path, path );

      int64_t avail = 0;
      int64_t limit = 0;
      string status_info;

      ods_fs.fetch_from_text_file( c_file_archive_size_avail, avail );
      ods_fs.fetch_from_text_file( c_file_archive_size_limit, limit );
      ods_fs.fetch_from_text_file( c_file_archive_status_info, status_info );

      if( min_avail <= 0 || avail >= min_avail )
      {
         retval += next;

         if( !minimal )
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

string relegate_time_stamped_files( const string& hash,
 const string& archive, uint32_t max_files, int64_t max_bytes, bool delete_files_always )
{
   guard g( g_mutex );

   string retval;

   vector< string > paths;
   set< string > importants;
   deque< string > file_hashes;

   int64_t min_bytes = 0;

   if( !hash.empty( ) )
      file_hashes.push_back( hash );
   else
   {
      string timestamp_expr( c_time_stamp_tag_prefix );
      timestamp_expr += "*";

      string all_tags( list_file_tags( timestamp_expr, 0, max_files, max_bytes, &min_bytes, &file_hashes, false ) );

      if( !all_tags.empty( ) )
      {
         vector< string > tags;
         split( all_tags, tags, '\n' );

         if( tags.size( ) != file_hashes.size( ) )
            throw runtime_error( "unexpected tags.size( ) != file_hashes.size( )" );

         for( size_t i = 0; i < tags.size( ); i++ )
         {
            if( tags[ i ].find( c_important_file_suffix ) != string::npos )
               importants.insert( file_hashes[ i ] );
         }
      }
   }

   int64_t num_bytes = hash.empty( ) ? min_bytes : file_bytes( hash );

   string all_archives( list_file_archives( true, &paths, num_bytes ) );

   if( !file_hashes.empty( ) && !all_archives.empty( ) )
   {
      vector< string > archives;
      split( all_archives, archives, '\n' );

      if( paths.size( ) != archives.size( ) )
         throw runtime_error( "unexpected paths.size( ) != archives.size( )" );

      ods::bulk_write bulk_write( ciyam_ods_instance( ) );
      ods_file_system& ods_fs( ciyam_ods_file_system( ) );

      ods::transaction ods_tx( ciyam_ods_instance( ) );

      for( size_t i = 0; i < archives.size( ); i++ )
      {
         if( !archive.empty( ) && archives[ i ] != archive )
            continue;

         string next_archive( archives[ i ] );

         ods_fs.set_root_folder( c_file_archives_folder );

         ods_fs.set_folder( next_archive );

         int64_t avail = 0;
         ods_fs.fetch_from_text_file( c_file_archive_size_avail, avail );

         while( !file_hashes.empty( ) )
         {
            string next_hash( file_hashes.front( ) );

            if( has_archived_file( ods_fs, next_hash, &next_archive ) )
            {
               // NOTE: If the file had been tagged as "important" then even if
               // it was already archived it will be archived again (which will
               // ensure that if such files are retrieved and later re-archived
               // several times then multiple archived copies will exist).
               if( i < archives.size( ) - 1 && importants.count( next_hash ) )
                  break;

               delete_file( next_hash, true );

               if( !retval.empty( ) )
                  retval += '\n';
               retval = next_hash + ' ' + next_archive;

               if( importants.count( next_hash ) )
                  importants.erase( next_hash );

               file_hashes.pop_front( );

               continue;
            }

            num_bytes = file_bytes( next_hash );

            if( num_bytes > avail )
               break;

            string dest( paths[ i ] + "/" + next_hash );

            copy_raw_file( next_hash, dest );

            if( !temp_file_is_identical( dest, next_hash ) )
            {
               file_remove( dest );
               break;
            }
            else
            {
               avail -= num_bytes;
               ods_fs.store_as_text_file( c_file_archive_size_avail, avail );

               ods_fs.set_folder( c_folder_archive_files_folder );

               ods_fs.add_file( next_hash, c_file_zero_length );
               delete_file( next_hash, true );

               ods_fs.set_folder( ".." );

               if( !retval.empty( ) )
                  retval += '\n';
               retval += next_hash + ' ' + next_archive;

               if( importants.count( next_hash ) )
                  importants.erase( next_hash );

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

bool has_file_been_archived( const string& hash )
{
   guard g( g_mutex );

   bool retval = false;
   vector< string > paths;

   string all_archives( list_file_archives( true, &paths ) );

   string archive;
   vector< string > archives;

   if( !all_archives.empty( ) )
   {
      split( all_archives, archives, '\n' );

      if( paths.size( ) != archives.size( ) )
         throw runtime_error( "unexpected paths.size( ) != archives.size( )" );

      ods::bulk_read bulk_read( ciyam_ods_instance( ) );
      ods_file_system& ods_fs( ciyam_ods_file_system( ) );

      for( size_t i = 0; i < archives.size( ); i++ )
      {
         archive = archives[ i ];

         ods_fs.set_root_folder( c_file_archives_folder );

         ods_fs.set_folder( archive );
         ods_fs.set_folder( c_folder_archive_files_folder );

         if( ods_fs.has_file( hash ) )
         {
            retval = true;
            break;
         }
      }
   }

   return retval;
}

string retrieve_file_from_archive( const string& hash, const string& tag, size_t days_ahead )
{
   guard g( g_mutex );

   string retval;

   if( !has_file( hash ) )
   {
      vector< string > paths;

      string all_archives( list_file_archives( true, &paths ) );

      string archive;
      vector< string > archives;

      if( !all_archives.empty( ) )
      {
         split( all_archives, archives, '\n' );

         if( paths.size( ) != archives.size( ) )
            throw runtime_error( "unexpected paths.size( ) != archives.size( )" );

         ods::bulk_read bulk_read( ciyam_ods_instance( ) );
         ods_file_system& ods_fs( ciyam_ods_file_system( ) );

         for( size_t i = 0; i < archives.size( ); i++ )
         {
            archive = archives[ i ];

            ods_fs.set_root_folder( c_file_archives_folder );

            ods_fs.set_folder( archive );
            ods_fs.set_folder( c_folder_archive_files_folder );

            if( ods_fs.has_file( hash ) )
            {
               retval = archive;

               string src_file( paths[ i ] + "/" + hash );

               if( file_exists( src_file ) )
               {
                  string file_data( buffer_file( src_file ) );

                  string tag_for_file( tag );

                  if( tag_for_file.empty( ) )
                     tag_for_file = current_time_stamp_tag( false, days_ahead );

                  create_raw_file( file_data, false, tag_for_file.c_str( ), 0, hash.c_str( ) );

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

void delete_file_from_archive( const string& hash, const string& archive, bool add_to_blacklist )
{
   guard g( g_mutex );

   regex expr( c_regex_hash_256 );

   if( expr.search( hash ) == string::npos )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Invalid file hash '" + hash + "'." );

   vector< string > paths;
   vector< string > archives;

   string all_archives( list_file_archives( true, &paths ) );

   auto_ptr< ods::bulk_write > ap_bulk_write;
   if( !ciyam_ods_instance( ).is_bulk_locked( ) )
      ap_bulk_write.reset( new ods::bulk_write( ciyam_ods_instance( ) ) );

   ods_file_system& ods_fs( ciyam_ods_file_system( ) );

   auto_ptr< ods::transaction > ap_ods_tx;
   if( !ciyam_ods_instance( ).is_in_transaction( ) )
      ap_ods_tx.reset( new ods::transaction( ciyam_ods_instance( ) ) );

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

         ods_fs.set_root_folder( c_file_archives_folder );

         ods_fs.set_folder( next_archive );

         int64_t avail = 0;
         ods_fs.fetch_from_text_file( c_file_archive_size_avail, avail );

         int64_t limit = 0;
         ods_fs.fetch_from_text_file( c_file_archive_size_limit, limit );

         ods_fs.set_folder( c_folder_archive_files_folder );

         if( ods_fs.has_file( hash ) )
         {
            ods_fs.remove_file( hash );
            string src_file( paths[ i ] + "/" + hash );

            if( file_exists( src_file ) )
            {
               avail += file_size( src_file );
               file_remove( src_file );

               if( avail > limit )
                  avail = 0;

               ods_fs.set_folder( ".." );
               ods_fs.store_as_text_file( c_file_archive_size_avail, avail );
            }
         }
      }
   }

   // NOTE: If no archive was specified then will also remove
   // the file from the files area.
   if( archive.empty( ) && has_file( hash ) )
      delete_file( hash, true );

   if( add_to_blacklist )
   {
      ods_fs.set_root_folder( c_file_blacklist_folder );
      ods_fs.add_file( hash, c_file_zero_length );

      // NOTE: If a matching repository entry is found then will
      // delete it along with removing the equivalent local file
      // from all archives and the files area.
      ods_fs.set_root_folder( c_file_repository_folder );

      if( ods_fs.has_file( hash ) )
      {
         stringstream sio_data;
         ods_fs.get_file( hash, &sio_data, true );
         
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
