// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#include "command_processor.h"

#include "utilities.h"
#include "command_handler.h"

using namespace std;

const int c_string_reserve = 1024;

command_processor::command_processor( command_handler& handler )
 :
 is_continuation( false )
{
   handlers.push( &handler );
   handler.p_command_processor = this;
}

void command_processor::process_commands( )
{
   string next, cmd_and_args;

   next.reserve( c_string_reserve );
   cmd_and_args.reserve( c_string_reserve );

   while( !handlers.empty( ) && is_still_processing( ) )
   {
      is_continuation = !cmd_and_args.empty( );

      get_cmd_and_args( next );

      cmd_and_args += next;
      clear_key( next, true );

      if( !cmd_and_args.empty( ) )
      {
         remove_trailing_cr_from_text_file_line( cmd_and_args );

         // NOTE: A trailing backslash (that is not escaped) is interpreted as a line continuation.
         string::size_type pos = cmd_and_args.find_last_not_of( '\\' );
         if( pos != cmd_and_args.size( ) - 1 && ( cmd_and_args.size( ) - pos - 1 ) % 2 )
         {
            cmd_and_args.erase( cmd_and_args.size( ) - 1 );
            continue;
         }

         execute_command( cmd_and_args );

         clear_key( cmd_and_args, true );
      }

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
