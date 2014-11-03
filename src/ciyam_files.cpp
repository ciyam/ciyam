// Copyright (c) 2013-2014 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <vector>
#  include <fstream>
#  include <stdexcept>
#endif

#ifdef __GNUG__
#  include <unistd.h>
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

using namespace std;

namespace
{

mutex g_mutex;

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

string construct_file_name_from_hash( const string& hash, bool create_directory = false )
{
   regex expr( c_regex_hash_256 );

   if( expr.search( hash ) == string::npos )
      throw runtime_error( "unexpected hash '" + hash + "'" );

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

   return s;
}

void init_files_area( )
{
   string cwd( get_cwd( ) );

   bool rc;
   set_cwd( c_files_directory, &rc );

   if( !rc )
      create_dir( c_files_directory, &rc, ( dir_perms )c_directory_perm_val );
   else
   {
      directory_filter df;
      fs_iterator dfsi( ".", &df );

      do
      {
         size_t max_num = get_files_area_item_max_num( );
         size_t max_size = get_files_area_item_max_size( );

         file_filter ff;
         fs_iterator fs( dfsi.get_path_name( ), &ff );

         vector< string > files_to_delete;

         while( fs.has_next( ) )
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

         for( size_t i = 0; i < files_to_delete.size( ); i++ )
            file_remove( files_to_delete[ i ] );

      } while( dfsi.has_next( ) );
   }

   set_cwd( cwd );
}

void init_file( const string& hash, const string& data )
{
   guard g( g_mutex );

   if( hash != lower( sha256( data ).get_digest_as_string( ) ) )
      throw runtime_error( "invalid content for '" + hash + "' (hash does not match hashed data)" );

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

void fetch_file( const string& hash, tcp_socket& socket )
{
   string filename( construct_file_name_from_hash( hash ) );

   file_transfer( filename, socket, e_ft_direction_send, get_files_area_item_max_size( ),
    c_response_okay_more, c_file_transfer_line_timeout, c_file_transfer_max_line_size );
}

void store_file( const string& hash, tcp_socket& socket )
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
      file_transfer( filename, socket, e_ft_direction_fetch, get_files_area_item_max_size( ),
       c_response_okay_more, c_file_transfer_line_timeout, c_file_transfer_max_line_size );

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
}

void fetch_temp_file( const string& name, tcp_socket& socket )
{
   file_transfer( name, socket, e_ft_direction_send, get_files_area_item_max_size( ),
    c_response_okay_more, c_file_transfer_line_timeout, c_file_transfer_max_line_size );
}

void store_temp_file( const string& name, tcp_socket& socket )
{
#ifndef _WIN32
   int um = umask( 077 );
#endif
   try
   {
      file_transfer( name, socket, e_ft_direction_fetch, get_files_area_item_max_size( ),
       c_response_okay_more, c_file_transfer_line_timeout, c_file_transfer_max_line_size );

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

