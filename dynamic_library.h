// Copyright (c) 2003
//
// CIYAM Pty. Ltd.
// ACN 093 704 539
//
// ALL RIGHTS RESERVED
//
// Permission to use this software for non-commercial purposes is hereby granted. Permission to
// distribute this software privately is granted provided that the source code is unaltered and
// complete or that any alterations and omissions have been first approved by CIYAM. Commercial
// usage of this software is not permitted without first obtaining a license for such a purpose
// from CIYAM. This software may not be publicly distributed unless written permission to do so
// has been obtained from CIYAM.

#ifndef DYNAMIC_LIBRARY_H
#  define DYNAMIC_LIBRARY_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     ifdef _WIN32
#        define NOMINMAX
#        define _WINSOCKAPI_
#        include <windows.h>
#     endif
#  endif

#  include "macros.h"

class dynamic_library
{
   public:
   dynamic_library( const std::string& file_name );
   dynamic_library( const std::string& file_name, const std::string& module_name );
   virtual ~dynamic_library( );

   void init( );

   const std::string& get_file_name( ) const { return file_name; }

   const std::string& get_module_name( ) const { return module_name; }

   void* bind_to_function( const std::string& func_name );

   private:
   std::string file_name;
   std::string module_name;

   DYNAMIC_LIBRARY dyn_lib;
};

#endif

