// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <dlfcn.h>
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
   ::dlclose( dyn_lib );
}

void dynamic_library::init( )
{
   string error;

   dyn_lib = ::dlopen( file_name.c_str( ), RTLD_LAZY );

   const char* p_error = ::dlerror( );

   if( p_error )
      error = string( p_error );

   if( !error.empty( ) )
      throw runtime_error( error );
}

void* dynamic_library::bind_to_function( const string& func_name )
{
   void* p_func;

   string error;

   p_func = ( void* )::dlsym( dyn_lib, func_name.c_str( ) );

   const char* p_error = ::dlerror( );

   if( p_error )
      error = string( p_error );

   if( !error.empty( ) )
      throw runtime_error( error );

   return p_func;
}
