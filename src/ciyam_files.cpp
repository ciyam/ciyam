// Copyright (c) 2013-2017 CIYAM Developers
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
#  include <fstream>
#  include <stdexcept>
#  ifdef __GNUG__
#     include <unistd.h>
#  endif
#endif

#define CIYAM_BASE_IMPL

#include "ciyam_files.h"

#include "ods.h"
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
#include "ods_file_system.h"

#ifdef ZLIB_SUPPORT
#  include <zlib.h>
#endif

using namespace std;

namespace
{

const int c_status_info_pad_len = 10;

const char* const c_archives = "archives";

const char* const c_timestamp_tag_prefix = "ts.";

const char* const c_file_archive_path = "path";
const char* const c_file_archive_size_avail = "size_avail";
const char* const c_file_archive_size_limit = "size_limit";
const char* const c_file_archive_status_info = "status_info";

const char* const c_folder_archive_stored_files = "stored_files";

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
      create_dir( dir_name, &rc, ( dir_perms )c_directory_perm_val );
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

void validate_hash_with_uncompressed_content( const string& hash, unsigned char* p_data, unsigned long length )
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
      throw runtime_error( "invalid content for file hash '" + hash + "'" );
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

bool has_archived_file( const string& hash, string* p_archive = 0 )
{
   bool retval = false;

   vector< string > paths;

   string all_archives( list_file_archives( true, &paths ) );

   string archive;
   vector< string > archives;

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
      ods_fs.set_folder( c_folder_archive_stored_files );

      if( ods_fs.has_folder( hash ) )
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

   s += " ";
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
         create_dir( c_files_directory, &rc, ( dir_perms )c_directory_perm_val );
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

string current_timestamp_tag( bool truncated )
{
   string retval( c_timestamp_tag_prefix );

   date_time dt( date_time::local( ) );

   if( truncated )
      retval += dt.as_string( e_time_format_hhmmssth, false );
   else
      retval += dt.as_string( e_time_format_hhmmsstht, false ) + "00000000";

   return retval;
}

bool has_tag( const string& name )
{
   guard g( g_mutex );

   string::size_type pos = name.rfind( '*' );
   map< string, string >::iterator i = g_tag_hashes.lower_bound( name.substr( 0, pos ) );

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

int64_t file_bytes( const string& hash )
{
   guard g( g_mutex );

   string filename( construct_file_name_from_hash( hash ) );

   return file_size( filename );
}

string file_type_info( const string& tag_or_hash,
 file_expansion expansion, int max_depth, int indent, bool add_size )
{
   guard g( g_mutex );

   string hash, filename;
   if( file_exists( string( c_files_directory ) + '/' + tag_or_hash ) )
   {
      hash = tag_file_hash( tag_or_hash );
      filename = construct_file_name_from_hash( hash );
   }
   else
   {
      bool is_base64 = false;

      if( tag_or_hash.length( ) != 64 )
          base64::validate( tag_or_hash, &is_base64 );

      hash = !is_base64 ? tag_or_hash : base64_to_hex( tag_or_hash );

      filename = construct_file_name_from_hash( hash, false, false );
   }

   if( !file_exists( filename ) )
      throw runtime_error( "file '" + tag_or_hash + "' was not found" );

   string data( buffer_file( filename ) );

   if( data.empty( ) )
      throw runtime_error( "unexpected empty file" );

   unsigned char file_type = ( data[ 0 ] & c_file_type_val_mask );

   bool is_core = ( data[ 0 ] & c_file_type_val_extra_core );
   bool is_compressed = ( data[ 0 ] & c_file_type_val_compressed );

   if( file_type != c_file_type_val_blob && file_type != c_file_type_val_list )
      throw runtime_error( "invalid file type '0x" + hex_encode( &file_type, 1 ) + "' for raw file creation" );

   if( !is_compressed )
   {
      sha256 test_hash( data );

      if( hash != test_hash.get_digest_as_string( ) )
         throw runtime_error( "invalid content for '" + tag_or_hash + "' (hash does not match hashed data)" );
   }

   string final_data( data );

#ifdef ZLIB_SUPPORT
   if( is_compressed )
   {
      session_file_buffer_access file_buffer;

      unsigned long size = data.size( ) - 1;
      unsigned long usize = file_buffer.get_size( ) - size;

      if( uncompress( ( Bytef * )file_buffer.get_buffer( ),
       &usize, ( Bytef * )&data[ 1 ], size ) != Z_OK )
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
      retval += string( indent, ' ' );

      retval += '[';

      if( is_core )
         retval += "core-";

      if( file_type == c_file_type_val_blob )
         retval += "blob]";
      else if( file_type == c_file_type_val_list )
         retval += "list]";
   }

   string size_info( "(" + format_bytes( final_data.size( ) ) + ")" );

   if( expansion == e_file_expansion_none )
   {
      retval += " " + lower( hash );

      if( add_size )
         retval += " " + size_info;

      if( is_core )
      {
         string::size_type pos = final_data.find( ':' );
         if( pos != string::npos )
            retval += " " + final_data.substr( 1, pos - 1 );
      }
   }
   else
   {
      if( file_type == c_file_type_val_blob )
      {
         if( expansion == e_file_expansion_recursive_hashes )
            retval += lower( hash );
         else
         {
            string blob_info( final_data.substr( 1 ) );

            if( is_valid_utf8( blob_info ) )
            {
               retval += " " + lower( hash );

               if( add_size )
                  retval += " " + size_info;

               if( max_depth && indent >= max_depth )
                  retval += "\n" + string( indent, ' ' ) + "...";
               else
               {
                  retval += " [utf8]";
                  retval += "\n" + utf8_replace( blob_info, "\r", "" );
               }
            }
            else
            {
               retval += " " + lower( hash );

               if( add_size )
                  retval += " " + size_info;

               if( max_depth && indent >= max_depth )
                  retval += "\n" + string( indent, ' ' ) + "...";
               else
               {
                  retval += " [base64]";

                  retval += "\n" + base64::encode( blob_info );
               }
            }
         }
      }
      else if( file_type == c_file_type_val_list )
      {
         string list_info( final_data.substr( 1 ) );

         vector< string > list_items;
         split( list_info, list_items, '\n' );

         if( expansion == e_file_expansion_recursive_hashes )
            retval += lower( hash );
         else
         {
            retval += " " + lower( hash );

            if( add_size )
               retval += " " + size_info;
         }

         for( size_t i = 0; i < list_items.size( ); i++ )
         {
            string next_list_item( list_items[ i ] );

            if( expansion == e_file_expansion_content )
               retval += "\n" + string( indent, ' ' ) + next_list_item;
            else if( max_depth && indent >= max_depth
             && expansion != e_file_expansion_recursive_hashes )
            {
               if( i == 0 )
                  retval += "\n" + string( indent, ' ' ) + "...";
            }
            else
            {
               string::size_type pos = next_list_item.find( ' ' );

               if( pos != string::npos && expansion != e_file_expansion_recursive_hashes )
                  retval += "\n" + string( indent, ' ' ) + next_list_item.substr( pos + 1 );

               retval += "\n" + file_type_info( next_list_item.substr( 0, pos ), expansion, max_depth, indent + 1, add_size );
            }
         }
      }
   }

   return retval;
}

string create_raw_file( const string& data, bool compress, const char* p_tag, bool* p_is_existing )
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

   bool is_compressed = ( data[ 0 ] & c_file_type_val_compressed );

#ifdef ZLIB_SUPPORT
   session_file_buffer_access file_buffer;

   if( is_compressed )
   {
      unsigned long size = final_data.size( ) - 1;
      unsigned long usize = file_buffer.get_size( ) - size;

      size_t offset = 1;

      if( uncompress( ( Bytef * )file_buffer.get_buffer( ),
       &usize, ( Bytef * )&final_data[ offset ], size ) != Z_OK )
         throw runtime_error( "invalid content for create_raw_file (bad compressed or uncompressed too large)" );

      compress = true;
      is_compressed = false;

      final_data = string( ( const char* )file_buffer.get_buffer( ) );
   }
#else
   if( is_compressed )
      throw runtime_error( "create_raw_file doesn't support compressed files (without ZLIB support)" );
#endif

   string hash( sha256( final_data ).get_digest_as_string( ) );

   string filename( construct_file_name_from_hash( hash, true ) );

   if( file_type != c_file_type_val_blob )
      validate_list( final_data.substr( 1 ) );

#ifdef ZLIB_SUPPORT
   if( compress && !is_compressed && data.size( ) > 32 ) // i.e. don't even bother trying to compress tiny files
   {
      unsigned long size = data.size( ) - 1;
      unsigned long csize = file_buffer.get_size( );

      size_t offset = 1;

      if( compress2( ( Bytef * )file_buffer.get_buffer( ),
       &csize, ( Bytef * )&data[ offset ], size, 9 ) != Z_OK ) // i.e. 9 is for maximum compression
         throw runtime_error( "invalid content in create_raw_file (bad compress or buffer too small)" );

      if( csize + offset < data.size( ) )
      {
         final_data[ 0 ] |= c_file_type_val_compressed;

         final_data.erase( offset );
         final_data += string( ( const char* )file_buffer.get_buffer( ), csize );

         is_compressed = true;
      }
   }
#endif

   if( !file_exists( filename ) )
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

#ifndef _WIN32
      int um = umask( 077 );
#endif
      ofstream outf( filename.c_str( ), ios::out | ios::binary );
#ifndef _WIN32
      umask( um );
#endif
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

   if( !tag_name.empty( ) )
      tag_file( tag_name, hash );
   else if( !file_extra_is_core )
      tag_file( current_timestamp_tag( ), hash );

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

void tag_del( const string& name, bool unlink )
{
   guard g( g_mutex );

   string::size_type pos = name.find_first_of( "*?" );

   if( pos == string::npos )
   {
      string tag_filename( c_files_directory );

      tag_filename += "/" + name;

      if( file_exists( tag_filename ) )
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
         tag_del( matching_tag_names[ i ], unlink );
   }
}

void tag_file( const string& name, const string& hash )
{
   guard g( g_mutex );

   string filename( construct_file_name_from_hash( hash ) );

   if( !file_exists( filename ) )
      throw runtime_error( hash + " was not found" );

   string tag_name;

   // NOTE: If an asterisk is included in the name then existing tags matching
   // the wildcard expression will be removed first then if the asterisk is at
   // the very end no new tag will be added unless two asterisks were used for
   // the name suffix in which case the new tag name will become the truncated
   // version (any other wildcard characters included will cause an error).
   string::size_type pos = name.rfind( '*' );

   if( pos == string::npos )
   {
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
      if( tag_name[ tag_name.length( ) - 1 ] == '?' )
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

   // NOTE: If the name is just "*" then return all the hashes that have been tagged or if is just "?"
   // then will return all the hashes of files that have not been tagged.
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

string list_file_tags( const string& pat, size_t max_tags )
{
   guard g( g_mutex );

   string retval;
   size_t num_tags = 0;

   if( !pat.empty( ) )
   {
      string::size_type pos = pat.find_first_of( "*?" );
      string prefix = pat.substr( 0, pos );

      map< string, string >::iterator i = g_tag_hashes.lower_bound( prefix );

      for( ; i != g_tag_hashes.end( ); ++i )
      {
         if( wildcard_match( pat, i->first ) )
         {
            ++num_tags;

            if( !retval.empty( ) )
               retval += "\n";
            retval += i->first;
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
         ++num_tags;

         if( !retval.empty( ) )
            retval += "\n";
         retval += i->first;

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

void fetch_file( const string& hash, tcp_socket& socket, progress* p_progress )
{
   string tmp_filename( "~" + uuid( ).as_string( ) );
   string filename( construct_file_name_from_hash( hash ) );

#ifndef _WIN32
   int um = umask( 077 );
#endif
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

#ifndef _WIN32
      umask( um );
#endif
      file_remove( tmp_filename );
   }
   catch( ... )
   {
#ifndef _WIN32
      umask( um );
#endif
      file_remove( tmp_filename );

      throw;
   }
}

void store_file( const string& hash, tcp_socket& socket,
 const char* p_tag, progress* p_progress, bool allow_core_file )
{
   string tmp_filename( "~" + uuid( ).as_string( ) );
   string filename( construct_file_name_from_hash( hash, true ) );

   bool existing = false;
   int64_t existing_bytes = 0;

   bool file_extra_is_core = false;

   if( !filename.empty( ) )
   {
      guard g( g_mutex );

      existing = file_exists( filename );

      if( existing )
         existing_bytes = file_size( filename );
   }

#ifndef _WIN32
   int um = umask( 077 );
#endif
   try
   {
      session_file_buffer_access file_buffer;

      file_transfer( tmp_filename,
       socket, e_ft_direction_recv, get_files_area_item_max_size( ),
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

      bool is_compressed = ( file_buffer.get_buffer( )[ 0 ] & c_file_type_val_compressed );

#ifdef ZLIB_SUPPORT
      if( is_compressed )
      {
         unsigned long size = file_size( tmp_filename ) - 1;
         unsigned long usize = file_buffer.get_size( ) - size;

         if( uncompress( ( Bytef * )&file_buffer.get_buffer( )[ size + 1 ],
          &usize, ( Bytef * )&file_buffer.get_buffer( )[ 1 ], size ) != Z_OK )
            throw runtime_error( "invalid content for '" + hash + "' (bad compressed or uncompressed too large)" );

         file_buffer.get_buffer( )[ size ] = file_buffer.get_buffer( )[ 0 ];
         validate_hash_with_uncompressed_content( hash, &file_buffer.get_buffer( )[ size ], usize + 1 );

         bool rc = true;

         if( file_type != c_file_type_val_blob )
            validate_list( ( const char* )&file_buffer.get_buffer( )[ size ], &rc );

         if( !rc )
            throw runtime_error( "invalid 'list' file" );
      }
#endif

      bool rc = true;

      if( !is_compressed && file_type != c_file_type_val_blob )
         validate_list( ( const char* )file_buffer.get_buffer( ), &rc );

      if( !rc )
         throw runtime_error( "invalid 'list' file" );

      if( !is_compressed )
      {
         sha256 test_hash;
         test_hash.update( tmp_filename, true );

         if( hash != test_hash.get_digest_as_string( ) )
            throw runtime_error( "invalid content for '" + hash + "' (hash does not match hashed data)" );
      }

#ifndef _WIN32
      umask( um );
#endif
      if( rc )
      {
         guard g( g_mutex );

         if( !existing && g_total_files >= get_files_area_item_max_num( ) )
            throw runtime_error( "maximum file area item limit has been reached" );

         file_copy( tmp_filename, filename );

         file_remove( tmp_filename );

         if( !existing )
            ++g_total_files;

         g_total_bytes -= existing_bytes;
         g_total_bytes += file_size( filename );
      }
   }
   catch( ... )
   {
#ifndef _WIN32
      umask( um );
#endif

      file_remove( tmp_filename );
      throw;
   }

   string tag_name;
   if( p_tag )
      tag_name = string( p_tag );

   if( !tag_name.empty( ) )
      tag_file( tag_name, hash );
   else if( !file_extra_is_core )
      tag_file( current_timestamp_tag( ), hash );
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
            tag_del( all_tags[ i ] );
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
#ifndef _WIN32
   int um = umask( 077 );
#endif
   try
   {
      file_transfer( name, socket,
       e_ft_direction_recv, get_files_area_item_max_size( ),
       c_response_okay_more, c_file_transfer_initial_timeout,
       c_file_transfer_line_timeout, c_file_transfer_max_line_size, 0, 0, 0, p_progress );

#ifndef _WIN32
      umask( um );
#endif
   }
   catch( ... )
   {
#ifndef _WIN32
      umask( um );
#endif
      throw;
   }
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

         if( uncompress( ( Bytef * )&file_buffer.get_buffer( )[ 0 ],
          &usize, ( Bytef * )&data[ offset ], size ) != Z_OK )
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

   string status_info( get_archive_status( path ) );

   ods::bulk_write bulk_write( ciyam_ods_instance( ) );
   ods_file_system& ods_fs( ciyam_ods_file_system( ) );

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

   ods_fs.add_folder( c_folder_archive_stored_files );
}

void remove_file_archive( const string& name )
{
   guard g( g_mutex );

   ods::bulk_write bulk_write( ciyam_ods_instance( ) );
   ods_file_system& ods_fs( ciyam_ods_file_system( ) );

   ods_fs.set_root_folder( c_file_archives_folder );

   if( !ods_fs.has_folder( name ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "archive '" + name + "' not found" );
   else
      ods_fs.remove_folder( name, 0, true );
}

void archives_status_update( )
{
   guard g( g_mutex );

   ods::bulk_write bulk_write( ciyam_ods_instance( ) );
   ods_file_system& ods_fs( ciyam_ods_file_system( ) );

   ods_fs.set_root_folder( c_file_archives_folder );

   vector< string > names;
   ods_fs.list_folders( names );

   for( size_t i = 0; i < names.size( ); i++ )
   {
      string next( names[ i ] );

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

      if( status_info != new_status_info )
         ods_fs.store_as_text_file( c_file_archive_status_info, new_status_info, c_status_info_pad_len );

      ods_fs.set_folder( ".." );
   }
}

string list_file_archives( bool minimal, vector< string >* p_paths, int64_t min_avail, bool stop_after_first )
{
   guard g( g_mutex );

   string retval;
   vector< string > names;

   ods::bulk_read bulk_read( ciyam_ods_instance( ) );
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

string relegate_file_to_archive( const string& hash )
{
   guard g( g_mutex );

   vector< string > paths;
   int64_t num_bytes = file_bytes( hash );

   string all_archives( list_file_archives( true, &paths, num_bytes ) );

   string archive;
   vector< string > archives;

   if( !all_archives.empty( ) )
   {
      split( all_archives, archives, '\n' );

      if( paths.size( ) != archives.size( ) )
         throw runtime_error( "unexpected paths.size( ) != archives.size( )" );

      if( has_archived_file( hash, &archive ) )
         delete_file( hash, true );
      else
      {
         ods::bulk_write bulk_write( ciyam_ods_instance( ) );
         ods_file_system& ods_fs( ciyam_ods_file_system( ) );

         for( size_t i = 0; i < archives.size( ); i++ )
         {
            string dest( paths[ i ] + "/" + hash );

            copy_raw_file( hash, dest );

            if( !temp_file_is_identical( dest, hash ) )
               file_remove( dest );
            else
            {
               archive = archives[ i ];

               ods_fs.set_root_folder( c_file_archives_folder );

               ods_fs.set_folder( archive );

               int64_t avail = 0;
               ods_fs.fetch_from_text_file( c_file_archive_size_avail, avail );

               avail -= num_bytes;
               ods_fs.store_as_text_file( c_file_archive_size_avail, avail );

               ods_fs.set_folder( c_folder_archive_stored_files );

               ods_fs.add_folder( hash );
               delete_file( hash, true );

               break;
            }
         }
      }
   }

   return archive;
}

string retrieve_file_from_archive( const string& hash, const string& tag )
{
   guard g( g_mutex );

   string retval;

   if( !has_file( hash ) )
   {
      vector< string > paths;

      string all_archives( list_file_archives( true, &paths ) );

      string archive;
      vector< string > archives;

      string tag_for_file( tag );
      if( tag_for_file.empty( ) )
         tag_for_file = current_timestamp_tag( );

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
            ods_fs.set_folder( c_folder_archive_stored_files );

            if( ods_fs.has_folder( hash ) )
            {
               retval = archive;

               string src_file( paths[ i ] + "/" + hash );

               if( file_exists( src_file ) )
               {
                  string file_data( buffer_file( src_file ) );
                  create_raw_file( file_data, false, tag_for_file.c_str( ) );

                  break;
               }
            }
         }
      }
   }

   return retval;
}

