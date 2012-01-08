// Copyright (c) 2005
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
#  include <string>
#endif

#define CAT_BASE_IMPL

#include "class_base.h"

#include "date_time.h"
#include "utilities.h"
#include "command_handler.h"
#include "date_time_helper.h"

using namespace std;

namespace
{

#include "date_time.cmh"

const char* const c_okay = "okay";

}

class date_time_command_handler;

class date_time_command_functor : public command_functor
{
   public:
   date_time_command_functor( date_time_command_handler& handler );

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   date_time_command_handler& cmd_handler;
};

command_functor* date_time_command_functor_factory( const string& /*name*/, command_handler& handler );

class date_time_command_handler : public command_handler
{
   friend class date_time_command_functor;

   public:
   date_time_command_handler( )
    :
    p_date_time( 0 )
   {
      add_commands( 0, date_time_command_functor_factory, ARRAY_PTR_AND_SIZE( date_time_command_definitions ) );
   }

   const string& get_retval( ) const { return retval; }

   void set_date_time( date_time* p_new_date_time ) { p_date_time = p_new_date_time; }

   void handle_unknown_command( const string& command )
   {
      if( command == "?" )
      {
         retval = "\n";
         retval += "date_time commands:\n";
         retval += "==================\n";
         retval += get_usage_for_all_commands( "" );
      }
      else
         retval = "unknown date_time command '" + command + "'";
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      retval = "invalid date_time command usage '" + cmd_and_args + "'";
   }

   private:
   date_time* p_date_time;

   protected:
   string retval;
};

date_time_command_functor::date_time_command_functor( date_time_command_handler& handler )
 : command_functor( handler ),
 cmd_handler( handler )
{
}

void date_time_command_functor::operator ( )( const string& command, const parameter_info& /*parameters*/ )
{
   cmd_handler.retval.erase( );

   if( command == c_cmd_date_time_inc )
      cmd_handler.retval = to_string( ++*cmd_handler.p_date_time );
   else if( command == c_cmd_date_time_dec )
      cmd_handler.retval = to_string( --*cmd_handler.p_date_time );
   else if( command == c_cmd_date_time_julian )
      cmd_handler.retval = to_string( ( julian )*cmd_handler.p_date_time );
   else if( command == c_cmd_date_time_weekday )
      cmd_handler.retval = to_string( cmd_handler.p_date_time->weekday_name( ) );
}

command_functor* date_time_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new date_time_command_functor( dynamic_cast< date_time_command_handler& >( handler ) );
}

void execute_command( date_time& n, const string& cmd_and_args, string& retval )
{
   static date_time_command_handler cmd_handler;

   cmd_handler.set_date_time( &n );
   cmd_handler.execute_command( cmd_and_args );

   retval = cmd_handler.get_retval( );
}

