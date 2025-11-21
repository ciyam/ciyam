// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <map>
#  include <string>
#  include <sstream>
#  include <fstream>
#  include <iostream>
#  include <exception>
#endif

#include "utilities.h"
#include "ciyam_base.h"
#include "class_base.h"
#include "ciyam_macros.h"
#include "module_strings.h"
#include "command_handler.h"

#include "module_interface.cpp"

// [<start includes>]
// [<finish includes>]

module_details g_module_details =
{
   "100",
   "0.1",
   "Meta"
};

namespace
{

#include "Meta.cmh"

#include "ciyam_constants.h"

}

void obtain_module_details( const module_details*& p_info )
{
   p_info = &g_module_details;
}

class module_Meta_command_functor : public command_functor
{
   public:
   module_Meta_command_functor( command_handler& handler )
    : command_functor( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );
};

void module_Meta_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   ( void )parameters;

   try
   {
      ostringstream osstr;

      if( command == c_cmd_Meta_ver )
         osstr << get_module_string( "title" );
      // [<start commands_impl>]
      // [<finish commands_impl>]

      handler.issue_command_response( osstr.str( ) );
   }
   catch( exception& x )
   {
      handler.issue_command_response( string( c_response_error_prefix ) + x.what( ), true );
   }
}

command_functor* module_Meta_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new module_Meta_command_functor( handler );
}

void obtain_externals( externals_container*& p_externals )
{
   static vector< string > externals;

   p_externals = &externals;
}

void obtain_module_commands(
 command_functor_creator*& p_functor_creator, command_definition*& p_definitions, size_t* p_num_commands )
{
   p_functor_creator = &module_Meta_command_functor_factory;

   p_definitions = Meta_command_definitions;
   *p_num_commands = ARRAY_SIZE( Meta_command_definitions );
}
