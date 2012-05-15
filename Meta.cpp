// Copyright (c) 2008
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

#ifdef __BORLANDC__
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

#define MODULE_META_IMPL

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

      handler.issue_command_reponse( osstr.str( ) );
   }
   catch( exception& x )
   {
      handler.issue_command_reponse( string( "(error) " ) + x.what( ), true );
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

