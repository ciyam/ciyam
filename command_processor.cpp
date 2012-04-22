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

#ifdef __BORLANDC__
#  include "precompile.h"
#endif
#pragma hdrstop

#include "command_processor.h"

#include "utilities.h"
#include "command_handler.h"

using namespace std;

command_processor::command_processor( command_handler& handler )
 :
 is_continuation( false )
{
   handlers.push( &handler );
   handler.p_command_processor = this;
}

void command_processor::process_commands( )
{
   string cmd_and_args;
   while( !handlers.empty( ) && is_still_processing( ) )
   {
      is_continuation = !cmd_and_args.empty( );

      cmd_and_args += get_cmd_and_args( );

      if( cmd_and_args.size( ) )
      {
         remove_trailing_cr_from_text_file_line( cmd_and_args );

         // NOTE: A trailing backslash (that is not escaped) is interpreted as a line continuation.
         string::size_type pos = cmd_and_args.find_last_not_of( '\\' );
         if( pos != cmd_and_args.size( ) - 1 && ( cmd_and_args.size( ) - pos - 1 ) % 2 )
         {
            cmd_and_args.erase( cmd_and_args.size( ) - 1 );
            continue;
         }
      }

      execute_command( cmd_and_args );
      cmd_and_args.erase( );

      if( handlers.top( )->has_finished( ) )
         handlers.pop( );
   }

   finalise_command_processing( );
}

void command_processor::execute_command( const std::string& cmd_and_args )
{
   handlers.top( )->execute_command( cmd_and_args );
}

string command_processor::get_usage_for_commands( const string& wildcard_match_expr, char cmd_arg_separator ) const
{
   return handlers.top( )->get_usage_for_all_commands( wildcard_match_expr, cmd_arg_separator );
}

void command_processor::push_handler( command_handler& handler )
{
   handlers.push( &handler );
   handler.p_command_processor = this;
}

void command_processor::pop_handler( )
{
   if( !handlers.empty( ) )
      handlers.pop( );
}

