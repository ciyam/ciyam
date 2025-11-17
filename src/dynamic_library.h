// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef DYNAMIC_LIBRARY_H
#  define DYNAMIC_LIBRARY_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
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
