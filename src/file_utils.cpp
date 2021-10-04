// Copyright (c) 2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2021 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstring>
#  include <deque>
#  include <fstream>
#  include <stdexcept>
#endif

#ifdef __GNUG__
#  include <unistd.h>
#  define _rmdir rmdir
#endif

#ifdef _WIN32
#  include <direct.h>
#  include <windows.h>
#endif

#include "file_utils.h"

#include "utilities.h"
#include "fs_iterator.h"

using namespace std;

namespace
{

const int c_buf_size = 65536;

#ifdef _WIN32
bool delete_files( const char* p_dir, bool recycle )
{
   int len = strlen( p_dir );
   char* p_from = new char[ len + 2 ];
   memcpy( ( void* )p_from, ( void* )p_dir, len );

   // NOTE: Requires two trailing zeros to mark the end of the file list.
   p_from[ len ] = 0;
   p_from[ len + 1 ] = 0;
  
   SHFILEOPSTRUCT fileop;
   fileop.hwnd = 0;
   fileop.wFunc = FO_DELETE;
   fileop.pFrom = p_from;
   fileop.pTo = 0;
   fileop.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
  
   if( recycle )
      fileop.fFlags |= FOF_ALLOWUNDO;

   fileop.fAnyOperationsAborted = 0;
   fileop.lpszProgressTitle = 0;
   fileop.hNameMappings = 0;

   int ret = ::SHFileOperation( &fileop );
   delete[ ] p_from;

   return ret == 0;
}
#endif

}

void create_directories( const string& file_path, int perm, int mask )
{
#ifndef _WIN32
   string::size_type pos = file_path.rfind( '/' );
#else
   string::size_type pos = file_path.find_last_of( "/\\" );
#endif

   if( !file_path.empty( ) && !file_exists( file_path ) )
   {
      vector< string > directories;
      string dir_path( file_path.substr( 0, pos ) );

      string next;

#ifdef _WIN32
      replace( dir_path, '\\', '/' );

      string::size_type dpos = dir_path.find( ':' );
      if( dpos != string::npos )
      {
         next += dir_path.substr( 0, pos + 1 );
         dir_path.erase( 0, pos + 1 );
      }
#endif

      bool absolute = false;

      if( !dir_path.empty( ) && dir_path[ 0 ] == '/' )
      {
         absolute = true;
         dir_path.erase( 0, 1 );
      }

      split( dir_path, directories, '/' );

      if( !absolute )
         next = get_cwd( );

      bool rc = true;

      for( size_t i = 0; i < directories.size( ); i++ )
      {
         next += '/' + directories[ i ];

         if( !dir_exists( next ) )
            create_dir( next, &rc, ( dir_perms )perm, ( mask < 0 ? STANDARD_UMASK : mask ) );
      }

      if( !rc )
         throw runtime_error( "unable to create directory '" + next + "' for '" + file_path + "'" );
   }
}

bool files_are_identical( const string& file_path1, const string& file_path2 )
{
   ifstream inpf1( file_path1.c_str( ), ios::in | ios::binary );
   ifstream inpf2( file_path2.c_str( ), ios::in | ios::binary );

   if( !inpf1 || !inpf2 )
      return false;

   char buf1[ c_buf_size ];
   char buf2[ c_buf_size ];

   do
   {
      inpf1.read( buf1, c_buf_size );
      inpf2.read( buf2, c_buf_size );

      if( inpf1.gcount( ) != inpf2.gcount( ) )
         return false;

      int bytes_read = inpf1.gcount( );

      if( memcmp( buf1, buf2, bytes_read ) != 0 )
         return false;

   } while( inpf1.good( ) || inpf2.good( ) );

   return true;
}

bool delete_directory_files( const string& path, bool include_directory_itself )
{
   bool okay = true;
#ifdef _WIN32
   okay = delete_files( ( path + "\\*.*" ).c_str( ), false );
#else
   directory_filter df;
   fs_iterator dfsi( path, &df, true );

   deque< string > directories;

   bool is_first = true;
   do
   {
      file_filter ff;
      fs_iterator ffsi( dfsi.get_path_name( ), &ff, true );

      while( ffsi.has_next( ) )
      {
         okay = file_remove( ffsi.get_full_name( ) );
         if( !okay )
            break;
      }

      if( !okay )
         break;

      if( is_first )
         is_first = false;
      else
         directories.push_front( dfsi.get_full_name( ) );

   } while( dfsi.has_next( ) );

   // NOTE: Now remove all sub-directories (children before parents).
   for( size_t i = 0; i < directories.size( ); i++ )
   {
      okay = ( _rmdir( directories[ i ].c_str( ) ) == 0 );
      if( !okay )
         break;
   }
#endif

   if( okay && include_directory_itself )
      okay = ( _rmdir( path.c_str( ) ) == 0 );

   return okay;
}

