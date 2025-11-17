// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
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
#  include <dirent.h>
#  include <stdexcept>
#endif

#include "fs_iterator.h"

#include "utilities.h"

using namespace std;

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

   absolute_path( new_root, path );

   bool found_true_root = false;

   DIR* p_dir = opendir( path.c_str( ) );

   if( !p_dir )
      throw runtime_error( "unable to open directory '" + path + "'" );

   if( path == "/" )
      found_true_root = true;

   if( !path.empty( ) && path[ path.length( ) - 1 ] == '/' )
      truncate = true;

   struct dirent* p_dirent = readdir( p_dir );

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

   directories.push( new directory_info( p_dir, path, p_dirent ) );
}

bool fs_iterator::has_next( )
{
   bool found = false;

   while( !found )
   {
      while( is_first ? directories.top( )->p_dirent
       : directories.top( )->p_dirent = readdir( directories.top( )->p_dir ) )
      {
         struct stat statbuf;

         is_first = false;

         string next( directories.top( )->p_dirent->d_name );

         string path( path_name.c_str( ) );

         path += c_dir_char;
         path += next;

         int rc = 0;

         if( !use_lstat )
            rc = stat( path.c_str( ), &statbuf );
         else
            rc = lstat( path.c_str( ), &statbuf );

         if( rc == 0 )
         {
            if( p_filter && !( *p_filter )( statbuf ) )
               continue;

            if( statbuf.st_mode & S_IFDIR )
            {
               if( next != "." && next != ".." )
               {
                  DIR* p_dir = opendir( path.c_str( ) );

                  if( p_dir )
                  {
                     name = next;

                     full_name = path;
                     path_name = full_name;

                     directories.push( new directory_info( p_dir, path, 0 ) );

                     found = true;
                     is_directory = true;

                     break;
                  }
               }
            }
            else
            {
               name = next;

               full_name = path;

               found = true;
               is_directory = false;

               break;
            }
         }
      }

      if( found )
         break;

      closedir( directories.top( )->p_dir );

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
      closedir( directories.top( )->p_dir );

      delete directories.top( );

      directories.pop( );
   }
}
