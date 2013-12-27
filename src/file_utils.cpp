// Copyright (c) 2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2013 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <deque>
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

bool delete_directory_files( const string& path, bool include_directory )
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

   if( okay && include_directory )
      okay = ( _rmdir( path.c_str( ) ) == 0 );

   return okay;
}

