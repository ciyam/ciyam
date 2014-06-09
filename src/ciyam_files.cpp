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

#include "format.h"
#include "sockets.h"
#include "utilities.h"
#include "ciyam_base.h"
#include "fs_iterator.h"

using namespace std;

namespace
{

#include "ciyam_constants.h"

size_t g_total_files = 0;
int64_t g_total_bytes = 0;

const char* const c_files_directory = "files";

}

size_t get_total_files( )
{
   return g_total_files;
}

int64_t get_total_bytes( )
{
   return g_total_bytes;
}

string get_file_stats( )
{
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
   bool rc;
   set_cwd( c_files_directory, &rc );

   if( !rc )
   {
      create_dir( c_files_directory );
      set_cwd( c_files_directory );
   }
   else
   {
      size_t max_num = get_files_area_item_max_num( );
      size_t max_size = get_files_area_item_max_size( );

      file_filter ff;
      fs_iterator fs( ".", &ff );

      vector< string > files_to_delete;

      while( fs.has_next( ) )
      {
         if( file_size( fs.get_name( ) ) > max_size )
            files_to_delete.push_back( fs.get_name( ) );
         else if( g_total_files >= max_num )
            files_to_delete.push_back( fs.get_name( ) );
         else
         {
            ++g_total_files;
            g_total_bytes += file_size( fs.get_name( ) );
         }
      }

      for( size_t i = 0; i < files_to_delete.size( ); i++ )
         file_remove( files_to_delete[ i ] );
   }

   set_cwd( ".." );
}

void init_file( const string& name, const string& data )
{
   string filename( string( c_files_directory ) + "/" + name );

   if( file_exists( filename ) )
   {
      --g_total_files;
      g_total_bytes -= file_size( filename );

      file_remove( filename );
   }

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

void fetch_file( const string& name, tcp_socket& socket )
{
   string filename( string( c_files_directory ) + "/" + name );

   file_transfer( filename, socket, e_ft_direction_send, get_files_area_item_max_size( ),
    c_response_okay_more, c_file_transfer_line_timeout, c_file_transfer_max_line_size );
}

void store_file( const string& name, tcp_socket& socket )
{
   string filename( string( c_files_directory ) + "/" + name );

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
      g_total_bytes -= existing_bytes;
      g_total_bytes += file_size( filename );
   }
   catch( ... )
   {
#ifndef _WIN32
      umask( um );
#endif
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

