// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2019 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  ifndef _WIN32
#     include <dlfcn.h>
#  endif
#  include <stdexcept>
#endif

#include "dynamic_library.h"

using namespace std;

dynamic_library::dynamic_library( const string& file_name )
 :
 file_name( file_name )
{
   init( );
}

dynamic_library::dynamic_library( const string& file_name, const string& module_name )
 :
 file_name( file_name ),
 module_name( module_name )
{
   init( );
}

dynamic_library::~dynamic_library( )
{
#ifdef _WIN32
   ::FreeLibrary( dyn_lib );
#else
   ::dlclose( dyn_lib );
#endif
}

void dynamic_library::init( )
{
   string error;
#ifdef _WIN32
   char buf[ 256 ];
   dyn_lib = ::LoadLibrary( file_name.c_str( ) );
   if( !dyn_lib )
   {
      ::FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 0, ::GetLastError( ), 0, buf, 128, 0 );
      error = string( buf );
   }
#else
   dyn_lib = ::dlopen( file_name.c_str( ), RTLD_LAZY );
   const char* p_error = ::dlerror( );
   if( p_error )
      error = string( p_error );
#endif

   if( !error.empty( ) )
      throw runtime_error( error );
}

void* dynamic_library::bind_to_function( const string& func_name )
{
   void* p_func;
   string error;
#ifdef _WIN32
   char buf[ 256 ];
   FARPROC p_proc_addr;
   p_proc_addr = ::GetProcAddress( dyn_lib, func_name.c_str( ) );
   if( !p_proc_addr )
   {
      ::FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 0, ::GetLastError( ), 0, buf, 128, 0 );
      error = string( buf ) + " (" + func_name + ")";
   }
   else
      p_func = ( void* )p_proc_addr;
#else
   p_func = ( void* )::dlsym( dyn_lib, func_name.c_str( ) );
   const char* p_error = ::dlerror( );
   if( p_error )
      error = string( p_error );
#endif
   if( !error.empty( ) )
      throw runtime_error( error );

   return p_func;
}

