// Copyright (c) 2013-2019 CIYAM Developers
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

const char* const c_timestamp_tag_prefix = "ts.";
const char* const c_important_file_suffix = "!";

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
   string cwd( get_cwd( ) );

   bool rc;
   set_cwd( dir_name, &rc );

   if( !rc )
   {
      create_dir( dir_name, &rc );
      if( !rc )
         throw runtime_error( "unable to create directory '" + dir_name + "'" );
   }
   else
      set_cwd( cwd );
}

string construct_file_name_from_hash( const string& hash,
 bool create_directory = false, bool check_hash_pattern = true )
{
   if( check_hash_pattern )
   {
      regex expr( c_regex_hash_256 );

      if( expr.search( hash ) == string::npos )
         throw runtime_error( "unexpected hash '" + hash + "'" );
   }

   string filename( c_files_directory );

   filename += '/';
   filename += lower( hash.substr( 0, 2 ) );

   if( create_directory )
      create_directory_if_not_exists( filename );

   filename += '/';
   filename += lower( hash.substr( 2 ) );

   return filename;
}

void validate_list( const string& data, bool* p_rc = 0 )
{
   vector< string > list_items;
   split( data, list_items, '\n' );

   for( size_t i = 0; i < list_items.size( ); i++ )
   {
      string next_list_item( list_items[ i ] );
      string::size_type pos = next_list_item.find( ' ' );

      if( !has_file( next_list_item.substr( 0, pos ) ) )
      {
         if( p_rc )
         {
            *p_rc = false;
            return;
         }
         else
            throw runtime_error( "file '" + next_list_item.substr( 0, pos ) + "' does not exist" );
      }
   }

   if( p_rc )
      *p_rc = true;
}

void validate_hash_with_uncompressed_content( const string& hash, const string& filename )
{
   sha256 test_hash;
   test_hash.update( filename, true );

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
   string cwd( get_cwd( ) );

   string tmp_filename( "~" + uuid( ).as_string( ) );

   try
   {
      bool rc;
      set_cwd( path, &rc );

      if( !rc )
         retval = string( c_file_archive_status_bad_access );
      else
      {
         ofstream outf( tmp_filename.c_str( ), ios::out );
         if( !outf )
            retval = string( c_file_archive_status_status_bad_create );
         else
         {
            outf << "." << endl;

            outf.flush( );

            if( !outf.good( ) )
               retval = string( c_file_archive_status_bad_write );
         }
      }

      file_remove( tmp_filename );
      set_cwd( cwd );

      return retval;
   }
   catch( ... )
   {
      file_remove( tmp_filename );
      set_cwd( cwd );
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

void init_files_area( vector< string >* p_untagged )
{
   string cwd( get_cwd( ) );

   try
   {
      bool rc;
      set_cwd( c_files_directory, &rc );

      if( !rc )
         create_dir( c_files_directory, &rc );
      else
      {
         directory_filter df;
         fs_iterator dfsi( ".", &df );

         bool is_first = true;
         do
         {
            size_t max_num = get_files_area_item_max_num( );
            size_t max_size = get_files_area_item_max_size( );

            file_filter ff;
            fs_iterator fs( dfsi.get_path_name( ), &ff );

            vector< string > files_to_delete;

            while( fs.has_next( ) )
            {
               if( is_first )
               {
                  string data( buffer_file( fs.get_full_name( ) ) );
                  string filename( construct_file_name_from_hash( data, false, false ) );

                  if( !file_exists( "../" + filename ) )
                     file_remove( fs.get_full_name( ) );

                  g_hash_tags.insert( make_pair( data, fs.get_name( ) ) );
                  g_tag_hashes.insert( make_pair( fs.get_name( ), data ) );
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
            }

            is_first = false;

            for( size_t i = 0; i < files_to_delete.size( ); i++ )
               file_remove( files_to_delete[ i ] );

         } while( dfsi.has_next( ) );
      }

      set_cwd( cwd );
   }
   catch( ... )
   {
      set_cwd( cwd );
      throw;
   }
}

void resync_files_area( vector< string >* p_untagged )
{
   guard g( g_mutex );

   g_hash_tags.clear( );
   g_tag_hashes.clear( );

   g_total_bytes = g_total_files = 0;

   init_files_area( p_untagged );
}

void term_files_area( )
{
   // FUTURE: Implementation to be added.
}

string current_timestamp_tag( bool truncated, size_t days_ahead )
{
   string retval( c_timestamp_tag_prefix );

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
         retval += dt.as_string( e_time_format_hhmmsstht, false ) + "00000000";
   }

   return retval;
}

bool has_tag( const string& name )
{
   guard g( g_mutex );

   string::size_type pos = name.rfind( '*' );
   map< string, string >::iterator i = ( pos == 0 ? g_tag_hashes.end( ) : g_tag_hashes.lower_bound( name.substr( 0, pos ) ) );

   if( i == g_tag_hashes.end( ) || ( pos == string::npos && i->first != name ) )
      return false;
   else
      return true;
}

bool has_file( const string& hash, bool check_is_hash )
{
   guard g( g_mutex );

   string filename( construct_file_name_from_hash( hash, false, check_is_hash ) );

   return file_exists( filename );
}

int64_t file_bytes( const string& hash, bool blobs_for_lists )
{
   guard g( g_mutex );

   string filename( construct_file_name_from_hash( hash ) );

   long file_size = 0;

   string data( buffer_file( filename, 1, &file_size ) );

   unsigned char file_type = ( data[ 0 ] & c_file_type_val_mask );

   bool is_encrypted = ( data[ 0 ] & c_file_type_val_encrypted );
   bool is_compressed = ( data[ 0 ] & c_file_type_val_compressed );

   if( blobs_for_lists && file_type == c_file_type_val_list )
      file_size = 0;

   if( !is_encrypted && blobs_for_lists && file_type == c_file_type_val_list )
   {
      data = buffer_file( filename );

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
 bool add_size, const char* p_prefix, bool allow_all_after )
{
   guard g( g_mutex );

   string hash, filename;

   int depth = max_depth;
   bool output_last_only = false;

   if( depth < 0 )
   {
      depth *= -1;
      output_last_only = true;
   }

   if( file_exists( string( c_files_directory ) + '/' + tag_or_hash ) )
   {
      hash = tag_file_hash( tag_or_hash );
      filename = construct_file_name_from_hash( hash );
   }
   else
   {
      bool is_base64 = false;

      if( tag_or_hash.length( ) != c_num_digest_characters )
          base64::validate( tag_or_hash, &is_base64 );

      hash = !is_base64 ? tag_or_hash : base64_to_hex( tag_or_hash );

      if( hash.length( ) == c_num_digest_characters )
         filename = construct_file_name_from_hash( hash, false, false );
   }

   if( !file_exists( filename ) )
      throw runtime_error( "file '" + tag_or_hash + "' was not found" );

   long file_size = 0;
   long max_to_buffer = 0;

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

   string data( buffer_file( filename, max_to_buffer, &file_size ) );
   
   if( data.empty( ) )
      throw runtime_error( "unexpected empty file '" + tag_or_hash + "'" );

   unsigned char file_type = ( data[ 0 ] & c_file_type_val_mask );

   bool is_core = ( data[ 0 ] & c_file_type_val_extra_core );
   bool is_encrypted = ( data[ 0 ] & c_file_type_val_encrypted );
   bool is_compressed = ( data[ 0 ] & c_file_type_val_compressed );

   if( file_type != c_file_type_val_blob && file_type != c_file_type_val_list )
      throw runtime_error( "invalid file type '0x" + hex_encode( &file_type, 1 ) + "' found in file_info" );

   if( !is_encrypted && max_to_buffer == 1 && file_type == c_file_type_val_list )
   {
      data = buffer_file( filename );

      if( data.size( ) <= 1 )
         throw runtime_error( "unexpected truncated file content for '" + tag_or_hash + "'" );
      else if( !get_session_variable( buffered_var_name ).empty( ) )
         set_session_variable( buffered_var_name, increment_special );
   }

   if( !is_encrypted && !is_compressed && ( file_type == c_file_type_val_list
    || ( ( depth != c_depth_to_omit_blob_content ) && ( expansion != e_file_expansion_recursive_hashes ) ) ) )
   {
      sha256 test_hash( data );

      if( hash != test_hash.get_digest_as_string( ) )
         throw runtime_error( "invalid content for '" + tag_or_hash + "' (hash does not match hashed data)" );
   }

   string final_data( data );

#ifdef ZLIB_SUPPORT
   if( !is_encrypted && is_compressed && final_data.size( ) > 1 )
   {
      session_file_buffer_access file_buffer;

      unsigned long size = final_data.size( ) - 1;
      unsigned long usize = file_buffer.get_size( ) - size;

      if( uncompress( ( Bytef * )file_buffer.get_buffer( ), &usize, ( Bytef * )&final_data[ 1 ], size ) != Z_OK )
         throw runtime_error( "invalid content for '" + tag_or_hash + "' (bad compressed or uncompressed too large)" );

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

               if( add_size )
                  retval += " " + size_info;
            }
         }

         for( size_t i = 0; i < list_items.size( ); i++ )
         {
            string next( list_items[ i ] );
            string::size_type pos = next.find( ' ' );

            string next_hash( next.substr( 0, pos ) );

            string next_name;
            if( pos != string::npos )
               next_name = next.substr( pos + 1 );

            string item_num( to_comparable_string( i, false, 6 ) ); 

            string size;
            if( output_last_only && depth == indent + 1 )
               size = " (" + format_bytes( file_bytes( next_hash, true ) ) + ")";

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
               if( !p_prefix || string( p_prefix ).find( next_name ) == 0 )
               {
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
                     allow_all = true;

                  string additional( file_type_info( next_hash,
                   expansion, max_depth, indent + 1, add_size, allow_all ? 0 : p_prefix ) );

                  if( !additional.empty( ) )
                  {
                     if( !retval.empty( ) )
                        retval += "\n";
                     retval += additional;
                  }
               }
            }
         }
      }
   }

   return retval;
}

string create_raw_file( const string& data, bool compress, const char* p_tag, bool* p_is_existing, const char* p_hash )
{
   guard g( g_mutex );

   if( data.empty( ) )
      throw runtime_error( "cannot create a raw file empty data" );

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

   string filename( construct_file_name_from_hash( hash, true ) );

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

   bool was_existing( file_exists( filename ) );

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

      ofstream outf( filename.c_str( ), ios::out | ios::binary );

      if( !outf )
         throw runtime_error( "unable to create output file '" + filename + "'" );

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
      tag_file( current_timestamp_tag( ) + tag_name, hash );

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

void tag_del( const string& name, bool unlink, bool auto_tag_with_time )
{
   guard g( g_mutex );

   string::size_type pos = name.find_first_of( "*?" );

   if( pos == string::npos )
   {
      string tag_filename( c_files_directory );

      tag_filename += "/" + name;

      file_remove( tag_filename );

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
            tag_file( current_timestamp_tag( ), hash );
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
      string filename( construct_file_name_from_hash( hash ) );

      if( !file_exists( filename ) )
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
            tag_name = current_timestamp_tag( );
      }

      if( !tag_name.empty( ) )
      {
         string tag_filename( c_files_directory );

         tag_filename += "/" + tag_name;

         ofstream outf( tag_filename.c_str( ) );
         if( !outf )
            throw runtime_error( "unable to open file '" + tag_filename + "' for output" );

         outf << hash;

         outf.flush( );
         if( !outf.good( ) )
            throw runtime_error( "unexpected bad output stream" );

         g_hash_tags.insert( make_pair( hash, tag_name ) );
         g_tag_hashes.insert( make_pair( tag_name, hash ) );
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
         throw runtime_error( "tag '" + name + "' not found" );

      retval = i->second;
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
   string filename( construct_file_name_from_hash( hash ) );

   sha256 temp_hash;

   if( !nonce.empty( ) )
      temp_hash.update( nonce );
   temp_hash.update( filename, true );

   return temp_hash.get_digest_as_string( );
}

void crypt_file( const string& tag_or_hash, const string& password, bool recurse )
{
   string hash( tag_or_hash );

   if( has_tag( tag_or_hash ) )
      hash = tag_file_hash( tag_or_hash );

   string filename( construct_file_name_from_hash( hash ) );

   if( !file_exists( filename ) )
      throw runtime_error( hash + " was not found" );

   string file_data( buffer_file( filename ) );

   if( file_data.empty( ) )
      throw runtime_error( "unexpected empty file content for '" + hash + "'" );

   unsigned char flags = file_data[ 0 ];

   unsigned char file_type = ( flags & c_file_type_val_mask );

   bool is_encrypted = ( flags & c_file_type_val_encrypted );
   bool is_compressed = ( flags & c_file_type_val_compressed );
   bool is_no_encrypt = ( flags & c_file_type_val_no_encrypt );

   if( is_no_encrypt )
      throw runtime_error( "attempt to encrypt file flagged with 'no encrypt'" );

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

      write_file( filename, new_file_data );

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

               crypt_file( next.substr( 0, pos ), password, recurse );
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

      string bad_hash_error( "invalid password to decrypt file '" + tag_or_hash + "'" );

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

      write_file( filename, file_data );

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

               crypt_file( next.substr( 0, pos ), password, recurse );
            }
         }
      }
   }
}

void fetch_file( const string& hash, tcp_socket& socket, progress* p_progress )
{
   string tmp_filename( "~" + uuid( ).as_string( ) );
   string filename( construct_file_name_from_hash( hash, false, false ) );

   try
   {
      // NOTE: As the file may end up being deleted whilst it is being
      // transferred it is copied to a temporary file which is instead
      // used for the transfer (and deleted afterwards).
      if( !filename.empty( ) )
      {
         guard g( g_mutex );

         if( !file_exists( filename ) )
            throw runtime_error( "file '" + hash + "' was not found" );

         file_copy( filename, tmp_filename );
      }

      file_transfer( tmp_filename, socket,
       e_ft_direction_send, get_files_area_item_max_size( ),
       c_response_okay_more, c_file_transfer_initial_timeout,
       c_file_transfer_line_timeout, c_file_transfer_max_line_size, 0, 0, 0, p_progress );

      file_remove( tmp_filename );
   }
   catch( ... )
   {
      file_remove( tmp_filename );

      throw;
   }
}

void store_file( const string& hash, tcp_socket& socket,
 const char* p_tag, progress* p_progress, bool allow_core_file, size_t max_bytes )
{
   string tmp_filename( "~" + uuid( ).as_string( ) );
   string filename( construct_file_name_from_hash( hash, true ) );

   bool existing = false;
   int64_t existing_bytes = 0;

   bool is_in_blacklist = false;

   bool file_extra_is_core = false;

   if( !max_bytes || max_bytes > get_files_area_item_max_size( ) )
      max_bytes = get_files_area_item_max_size( );

   if( !filename.empty( ) )
   {
      guard g( g_mutex );

      existing = file_exists( filename );

      if( existing )
         existing_bytes = file_size( filename );
   }

   try
   {
      session_file_buffer_access file_buffer;

      file_transfer( tmp_filename, socket, e_ft_direction_recv, max_bytes,
       c_response_okay_more, c_file_transfer_initial_timeout, c_file_transfer_line_timeout,
       c_file_transfer_max_line_size, 0, file_buffer.get_buffer( ), file_buffer.get_size( ), p_progress );

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
         unsigned long size = file_size( tmp_filename ) - 1;
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
         validate_hash_with_uncompressed_content( hash, tmp_filename );

      if( rc )
      {
         guard g( g_mutex );

         if( !existing )
            is_in_blacklist = file_has_been_blacklisted( hash );

         if( !existing && !is_in_blacklist && g_total_files >= get_files_area_item_max_num( ) )
         {
            // NOTE: First attempt to relegate an existing file in order to make room.
            relegate_timestamped_files( "", "", 1, 0, true );

            if( g_total_files >= get_files_area_item_max_num( ) )
               throw runtime_error( "maximum file area item limit has been reached" );
         }

         if( !is_in_blacklist )
         {
#ifndef ZLIB_SUPPORT
            file_copy( tmp_filename, filename );
#else
            bool has_written = false;
            unsigned long size = file_size( tmp_filename ) - 1;

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

                     write_file( filename, ( unsigned char* )&file_buffer.get_buffer( )[ size ], csize + 1 );
                  }
               }
               else if( rc != Z_BUF_ERROR )
                  throw runtime_error( "unexpected compression error in store_file" );
            }

            if( !has_written )
               file_copy( tmp_filename, filename );
#endif
         }

         file_remove( tmp_filename );

         if( !existing && !is_in_blacklist )
            ++g_total_files;

         if( !is_in_blacklist )
         {
            g_total_bytes -= existing_bytes;
            g_total_bytes += file_size( filename );
         }
      }
   }
   catch( ... )
   {
      file_remove( tmp_filename );
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
      else if( !existing && !file_extra_is_core )
         tag_file( current_timestamp_tag( ) + tag_name, hash );
   }
}

void delete_file( const string& hash, bool even_if_tagged )
{
   guard g( g_mutex );

   string tags( get_hash_tags( hash ) );
   string filename( construct_file_name_from_hash( hash ) );

   if( tags.empty( ) || even_if_tagged )
   {
      if( !file_exists( filename ) )
         throw runtime_error( "file '" + filename + "' not found" );

      if( !tags.empty( ) )
      {
         vector< string > all_tags;
         split( tags, all_tags, '\n' );

         for( size_t i = 0; i < all_tags.size( ); i++ )
            tag_del( all_tags[ i ], false, false );
      }

      int64_t existing_bytes = file_size( filename );

      file_remove( filename );

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
      delete_file( hashes[ i ] );
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

void copy_raw_file( const string& hash, const string& dest_filename )
{
   guard g( g_mutex );

   string filename( construct_file_name_from_hash( hash ) );

   if( !file_exists( filename ) )
      throw runtime_error( "file '" + filename + "' not found" );

   file_copy( filename, dest_filename );
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
   string filename( construct_file_name_from_hash( hash ) );

   return files_are_identical( temp_name, filename );
}

string extract_file( const string& hash, const string& dest_filename, unsigned char check_file_type_and_extra )
{
   guard g( g_mutex );

   string filename( construct_file_name_from_hash( hash ) );

   string data( buffer_file( filename ) );

   if( !data.empty( ) )
   {
      if( check_file_type_and_extra )
      {
         if( data[ 0 ] & check_file_type_and_extra != check_file_type_and_extra )
            throw runtime_error( "unexpected file type/extra" );
      }

      unsigned char file_type = ( data[ 0 ] & c_file_type_val_mask );
      unsigned char file_extra = ( data[ 0 ] & c_file_type_val_extra_mask );

      bool is_compressed = ( data[ 0 ] & c_file_type_val_compressed );

      session_file_buffer_access file_buffer;

      unsigned long size = file_size( filename ) - 1;

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

      if( !dest_filename.empty( ) )
         write_file( dest_filename, file_buffer.get_buffer( ), size );

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
   string tmp_filename( "~" + uuid( ).as_string( ) );

   if( path.empty( )
    || path[ path.length( ) - 1 ] == '\\' || path[ path.length( ) - 1 ] == '/' )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "invalid path '" + path + "' for add_file_archive" );

   if( path_already_used_in_archive( path ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "an archive with the path '" + path + "' already exists" );

   int64_t min_limit = get_files_area_item_max_size( ) * 10;

   if( size_limit < min_limit )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "archive minimum size must be at least " + to_string( min_limit ) + " bytes" );

   string status_info( get_archive_status( path ) );

   ods::bulk_write bulk_write( ciyam_ods_instance( ) );
   ods_file_system& ods_fs( ciyam_ods_file_system( ) );

   ods::transaction ods_tx( ciyam_ods_instance( ) );

   ods_fs.set_root_folder( c_file_archives_folder );

   if( ods_fs.has_folder( name ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "archive '" + name + "' already exists" );

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
      throw runtime_error( "archive '" + name + "' not found" );
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
      throw runtime_error( "archive '" + name + "' not found" );
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

string relegate_timestamped_files( const string& hash,
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
      string timestamp_expr( c_timestamp_tag_prefix );
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
                     tag_for_file = current_timestamp_tag( false, days_ahead );

                  create_raw_file( file_data, false, tag_for_file.c_str( ), 0, hash.c_str( ) );

                  break;
               }
            }
         }
      }

      if( retval.empty( ) )
         // FUTURE: This message should be handled as a server string message.
         throw runtime_error( "unable to retrieve file " + hash + " from archival" );
   }

   return retval;
}

void delete_file_from_archive( const string& hash, const string& archive, bool add_to_blacklist )
{
   guard g( g_mutex );

   regex expr( c_regex_hash_256 );

   if( expr.search( hash ) == string::npos )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "invalid file hash '" + hash + "'" );

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
