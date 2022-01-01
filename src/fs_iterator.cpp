// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2022 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstdio>
#  include <cerrno>
#  include <cassert>
#  include <stdexcept>
#  ifdef __BORLANDC__
#     include <cio>
#     include <dos.h>
#  endif
#  ifdef _MSC_VER
#     include <io.h>
#     include <direct.h>
#  endif
#  ifdef _WIN32
#     ifndef STRICT
#        define STRICT
#     endif
#     define NOMINMAX
#     include <windows.h>
#  else
#     include <dirent.h>
#  endif
#endif

#include "fs_iterator.h"

#include "utilities.h"

using namespace std;

#ifdef _WIN32
struct directory_info
{
   directory_info( HANDLE fhdl, const string& path, WIN32_FIND_DATA fdata )
    :
    fhdl( fhdl ), path( path ), fdata( fdata )
   {
   }

   HANDLE fhdl;
   string path;
   WIN32_FIND_DATA fdata;
};
#else
struct directory_info
{
   directory_info( DIR* p_dir, const string& path, struct dirent* p_dirent )
    :
    p_dir( p_dir ), path( path ), p_dirent( p_dirent )
   {
   }

   DIR* p_dir;
   string path;
   struct dirent* p_dirent;
};
#endif

fs_iterator::fs_iterator( const string& root, fs_filter* p_filter, bool treat_symbolic_links_as_files )
 :
 p_filter( p_filter ),
 use_lstat( treat_symbolic_links_as_files )
{
   set_root( root );
}

fs_iterator::~fs_iterator( )
{
   reset( );
}

void fs_iterator::set_root( const string& new_root )
{
   bool truncate = false;
   string path;

   if( !absolute_path( new_root, path ) )
      throw runtime_error( "unable to determine absolute path for '" + new_root + "'" );

   bool found_true_root = false;

#ifdef _WIN32
   WIN32_FIND_DATA fdata;
   string fspec( path );
   if( !fspec.empty( ) )
   {
      if( path == string( "/" ) || path == string( "\\" ) )
      {
         found_true_root = true;
         path = string( 1, _getdrive( ) - 1 + 'A' );
         path += ':';
         path += string( 1, c_dir_char );
      }
      else if( path.length( ) == 2 && path[ 1 ] == ':' )
      {
         found_true_root = true;
         path[ 0 ] = toupper( path[ 0 ] );
         path += string( 1, c_dir_char );
      }
      else if( path.length( ) == 3 && path[ 1 ] == ':' && ( path[ 2 ] == '/' || path[ 2 ] == '\\' ) )
      {
         found_true_root = true;
         path[ 0 ] = toupper( path[ 0 ] );
         path[ 2 ] = c_dir_char;
      }

      char last_char = fspec[ fspec.length( ) - 1 ];
      if( last_char != '/' && last_char != '\\' )
         fspec += "\\*";
      else
      {
         fspec += "*";
         truncate = true;
      }
   }

   if( _access( path.c_str( ), 0 ) != 0 )
      throw runtime_error( "unable to find directory '" + path + "'" );

   HANDLE fhdl = FindFirstFile( fspec.c_str( ), &fdata );
#else
   DIR* p_dir = opendir( path.c_str( ) );
   if( !p_dir )
      throw runtime_error( "unable to open directory '" + path + "'" );

   if( path == "/" )
      found_true_root = true;

   if( !path.empty( ) && path[ path.length( ) - 1 ] == '/' )
      truncate = true;

   struct dirent* p_dirent = readdir( p_dir );
#endif

   if( found_true_root )
   {
      true_root = path;
      is_true_root = true;
      path.erase( );
   }
   else
   {
      true_root.erase( );
      is_true_root = false;
      if( truncate )
         path = path.substr( 0, path.length( ) - 1 );
   }

   reset( );
   is_first = true;
   is_directory = true;
   path_name = full_name = name = root = path;

#ifdef _WIN32
   directories.push( new directory_info( fhdl, path, fdata ) );
#else
   directories.push( new directory_info( p_dir, path, p_dirent ) );
#endif
}

bool fs_iterator::has_next( )
{
   bool found = false;

   while( !found )
   {
#ifdef _WIN32
      while( is_first ? directories.top( )->fhdl != INVALID_HANDLE_VALUE
       : FindNextFile( directories.top( )->fhdl, &directories.top( )->fdata ) )
#else
      while( is_first ? directories.top( )->p_dirent
       : directories.top( )->p_dirent = readdir( directories.top( )->p_dir ) )
#endif
      {
         struct stat statbuf;

         is_first = false;

#ifdef _WIN32
         string next( directories.top( )->fdata.cFileName );
#else
         string next( directories.top( )->p_dirent->d_name );
#endif

         string path( path_name.c_str( ) );
         path += c_dir_char;
         path += next;

#ifdef _WIN32
         if( stat( path.c_str( ), &statbuf ) == 0 )
#else
         int rc;
         if( !use_lstat )
            rc = stat( path.c_str( ), &statbuf );
         else
            rc = lstat( path.c_str( ), &statbuf );

         if( rc == 0 )
#endif
         {
            if( p_filter && !( *p_filter )( statbuf ) )
               continue;

            if( statbuf.st_mode & S_IFDIR )
            {
               if( next != "." && next != ".." )
               {
#ifdef _WIN32
                  WIN32_FIND_DATA fdata;
                  HANDLE fhdl = FindFirstFile( string( path + "/*" ).c_str( ), &fdata );

                  name = next;
                  full_name = path;
                  path_name = full_name;

                  directories.push( new directory_info( fhdl, path, fdata ) );

                  is_directory = true;
                  found = true;
                  break;
#else
                  DIR* p_dir = opendir( path.c_str( ) );
                  if( p_dir )
                  {
                     name = next;
                     full_name = path;
                     path_name = full_name;

                     directories.push( new directory_info( p_dir, path, 0 ) );

                     is_directory = true;
                     found = true;
                     break;
                  }
#endif
               }
            }
            else
            {
               name = next;
               full_name = path;

               is_directory = false;
               found = true;
               break;
            }
         }
      }

      if( found )
         break;

#ifdef _WIN32
      FindClose( directories.top( )->fhdl );
#else
      closedir( directories.top( )->p_dir );
#endif
      delete directories.top( );
      directories.pop( );

      if( directories.empty( ) )
         break;

      path_name = full_name = name = directories.top( )->path;
   }

   return found;
}

void fs_iterator::reset( )
{
   while( !directories.empty( ) )
   {
#ifdef _WIN32
      FindClose( directories.top( )->fhdl );
#else
      closedir( directories.top( )->p_dir );
#endif
      delete directories.top( );
      directories.pop( );
   }
}

