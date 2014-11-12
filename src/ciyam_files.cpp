// Copyright (c) 2013-2014 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <map>
#  include <vector>
#  include <fstream>
#  include <stdexcept>
#  ifdef __GNUG__
#     include <unistd.h>
#  endif
#endif

#define CIYAM_BASE_IMPL

#include "ciyam_files.h"

#include "regex.h"
#include "config.h"
#include "format.h"
#include "sha256.h"
#include "sockets.h"
#include "threads.h"
#include "utilities.h"
#include "ciyam_base.h"
#include "file_utils.h"
#include "fs_iterator.h"

#ifdef ZLIB_SUPPORT
#  include <zlib.h>
#endif

using namespace std;

namespace
{

mutex g_mutex;

map< string, string > g_tag_hashes;
multimap< string, string > g_hash_tags;

#include "ciyam_constants.h"

size_t g_total_files = 0;
int64_t g_total_bytes = 0;

const char* const c_files_directory = "files";

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
   filename += hash.substr( 0, 2 );

   if( create_directory )
      create_directory_if_not_exists( filename );

   filename += '/';
   filename += hash.substr( 2 );

   return filename;
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

   s += " ";
   s += to_string( g_hash_tags.size( ) );
   s += " tag(s)";

   return s;
}

void init_files_area( )
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

                  sha256 test_hash;
                  test_hash.update( file_path, true );

                  string hash( dfsi.get_name( ) + fs.get_name( ) );

                  if( hash != lower( test_hash.get_digest_as_string( ) ) )
                     files_to_delete.push_back( file_path );
                  else if( file_size( file_path ) > max_size )
                     files_to_delete.push_back( file_path );
                  else if( g_total_files >= max_num )
                     files_to_delete.push_back( file_path );
                  else
                  {
                     ++g_total_files;
                     g_total_bytes += file_size( file_path );
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

bool has_file( const string& hash )
{
   string filename( construct_file_name_from_hash( hash ) );

   return file_exists( filename );
}

int64_t file_bytes( const string& hash )
{
   string filename( construct_file_name_from_hash( hash ) );

   return file_size( filename );
}

void create_raw_file( const string& data )
{
   guard g( g_mutex );

   if( data.empty( ) )
      throw runtime_error( "cannot create a raw file empty data" );

   unsigned char file_type = ( data[ 0 ] & c_file_type_val_mask );

   if( file_type != c_file_type_val_blob
    && file_type != c_file_type_val_item && file_type != c_file_type_val_tree )
      throw runtime_error( "invalid file type '0x" + hex_encode( &file_type, 1 ) + "' for raw file creation" );

   string hash( lower( sha256( data ).get_digest_as_string( ) ) );

   string filename( construct_file_name_from_hash( hash, true ) );

   if( !file_exists( filename ) )
   {
      if( g_total_files >= get_files_area_item_max_num( ) )
         throw runtime_error( "maximum file area item limit has been reached" );

      size_t max_num = get_files_area_item_max_num( );
      size_t max_size = get_files_area_item_max_size( );

      if( data.size( ) > max_size )
         throw runtime_error( "maximum file area item size limit cannot be exceeded" );

      int64_t max_bytes = ( int64_t )max_num * ( int64_t )max_size;

      if( g_total_bytes + data.size( ) > max_bytes )
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

      outf << data;

      ++g_total_files;
      g_total_bytes += data.size( );
   }
}

void tag_del( const string& name )
{
   guard g( g_mutex );

   string tag_filename( c_files_directory );

   tag_filename += "/" + name;

   if( file_exists( tag_filename ) )
      file_remove( tag_filename );

   if( g_tag_hashes.count( name ) )
   {
      string hash = g_tag_hashes[ name ];
      g_tag_hashes.erase( name );

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
   }
}

void tag_file( const string& name, const string& hash )
{
   guard g( g_mutex );

   string filename( construct_file_name_from_hash( hash ) );

   if( !file_exists( filename ) )
      throw runtime_error( hash + " was not found" );

   tag_del( name );

   string tag_filename( c_files_directory );

   tag_filename += "/" + name;

   ofstream outf( tag_filename.c_str( ) );
   if( !outf )
      throw runtime_error( "unable to open file '" + tag_filename + "' for output" );

   outf << hash;

   outf.flush( );
   if( !outf.good( ) )
      throw runtime_error( "unexpected bad output stream" );

   g_hash_tags.insert( make_pair( hash, name ) );
   g_tag_hashes.insert( make_pair( name, hash ) );
}

string get_hash_tags( const string& hash )
{
   guard g( g_mutex );

   string retval;

   multimap< string, string >::iterator i;
   for( i = g_hash_tags.lower_bound( hash ); i != g_hash_tags.end( ); ++i )
   {
      if( i->first != hash )
         break;

      if( !retval.empty( ) )
         retval += '\n';

      retval += i->second;
   }

   return retval;
}

string tag_file_hash( const string& name )
{
   string tag_filename( c_files_directory );
   tag_filename += "/" + name;

   if( !file_exists( tag_filename ) )
      throw runtime_error( "link '" + name + "' not found" );

   return buffer_file( tag_filename );
}

string list_file_tags( const string& pat )
{
   string retval;

   file_filter ff;
   fs_iterator fs( c_files_directory, &ff );

   while( fs.has_next( ) )
   {
      if( !retval.empty( ) )
         retval += "\n";

      if( wildcard_match( pat, fs.get_name( ) ) )
         retval += fs.get_name( );
   }

   return retval;
}

string hash_two_with_token_separator( const string& hash1, const string& hash2, const string& separator )
{
   string filename1( construct_file_name_from_hash( hash1 ) );
   string filename2( construct_file_name_from_hash( hash2 ) );

   sha256 hash;
   hash.update( filename1, true );

   if( !separator.empty( ) )
      hash.update( separator );

   hash.update( filename2, true );

   return lower( hash.get_digest_as_string( ) );
}

void fetch_file( const string& hash, tcp_socket& socket )
{
   string filename( construct_file_name_from_hash( hash ) );

   file_transfer( filename,
    socket, e_ft_direction_send, get_files_area_item_max_size( ),
    c_response_okay_more, c_file_transfer_initial_timeout,
    c_file_transfer_line_timeout, c_file_transfer_max_line_size );
}

void store_file( const string& hash, tcp_socket& socket, const char* p_tag )
{
   string filename( construct_file_name_from_hash( hash, true ) );

   bool existing = file_exists( filename );
   int64_t existing_bytes = 0;

   if( existing )
      existing_bytes = file_size( filename );

   if( !existing && g_total_files >= get_files_area_item_max_num( ) )
      throw runtime_error( "maximum file area item limit has been reached" );

#ifndef _WIN32
   int um = umask( 077 );
#endif
   try
   {
      session_file_buffer_access file_buffer;

      file_transfer( filename,
       socket, e_ft_direction_receive, get_files_area_item_max_size( ),
       c_response_okay_more, c_file_transfer_initial_timeout, c_file_transfer_line_timeout,
       c_file_transfer_max_line_size, '\0', file_buffer.get_buffer( ), file_buffer.get_size( ) );

#ifdef ZLIB_SUPPORT
      bool is_compressed = ( file_buffer.get_buffer( )[ 0 ] & c_file_type_val_compressed );

      if( is_compressed )
      {
         unsigned long size = file_size( filename );
         unsigned long usize = file_buffer.get_size( ) - size;

         if( uncompress( ( Bytef * )&file_buffer.get_buffer( )[ size ],
          &usize, ( Bytef * )file_buffer.get_buffer( ), size ) != Z_OK )
            throw runtime_error( "invalid content for '" + hash + "' (bad compressed or uncompressed too large)" );
      }
#endif

#ifndef _WIN32
      umask( um );
#endif
      sha256 test_hash;
      test_hash.update( filename, true );

      if( hash != lower( test_hash.get_digest_as_string( ) ) )
      {
         file_remove( filename );
         throw runtime_error( "invalid content for '" + hash + "' (hash does not match hashed data)" );
      }

      guard g( g_mutex );

      g_total_bytes -= existing_bytes;
      g_total_bytes += file_size( filename );
   }
   catch( ... )
   {
#ifndef _WIN32
      umask( um );
#endif
      guard g( g_mutex );

      if( existing && !file_exists( filename ) )
      {
         --g_total_files;
         g_total_bytes -= existing_bytes;
      }
      throw;
   }

   if( !existing )
      ++g_total_files;

   string tag_name;
   if( p_tag )
      tag_name = string( p_tag );

   if( !tag_name.empty( ) )
      tag_file( tag_name, hash );
}

void fetch_temp_file( const string& name, tcp_socket& socket )
{
   file_transfer( name, socket,
    e_ft_direction_send, get_files_area_item_max_size( ),
    c_response_okay_more, c_file_transfer_initial_timeout,
    c_file_transfer_line_timeout, c_file_transfer_max_line_size );
}

void store_temp_file( const string& name, tcp_socket& socket )
{
#ifndef _WIN32
   int um = umask( 077 );
#endif
   try
   {
      file_transfer( name, socket,
       e_ft_direction_receive, get_files_area_item_max_size( ),
       c_response_okay_more, c_file_transfer_initial_timeout,
       c_file_transfer_line_timeout, c_file_transfer_max_line_size );

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

