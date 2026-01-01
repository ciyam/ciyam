// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <string>
#endif

#include "class_base.h"

#include "numeric.h"
#include "utilities.h"
#include "command_handler.h"

using namespace std;

namespace
{

#include "numeric.cmh"

const char* const c_okay = "okay";

}

class numeric_command_handler;

class numeric_command_functor : public command_functor
{
   public:
   numeric_command_functor( numeric_command_handler& handler );

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   numeric_command_handler& cmd_handler;
};

command_functor* numeric_command_functor_factory( const string& /*name*/, command_handler& handler );

class numeric_command_handler : public command_handler
{
   friend class numeric_command_functor;

   public:
   numeric_command_handler( )
    :
    p_numeric( 0 )
   {
      add_commands( 0, numeric_command_functor_factory, ARRAY_PTR_AND_SIZE( numeric_command_definitions ) );
   }

   const string& get_retval( ) const { return retval; }

   void set_numeric( numeric* p_new_numeric ) { p_numeric = p_new_numeric; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      ( void )cmd_and_args;

      if( command == "?" )
      {
         retval = "\n";
         retval += "numeric commands:\n";
         retval += "==============\n";
         retval += get_usage_for_all_commands( "" );
      }
      else
         retval = "unknown numeric command '" + command + "'";
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      retval = "invalid numeric command usage '" + cmd_and_args + "'";
   }

   private:
   numeric* p_numeric;

   protected:
   string retval;
};

numeric_command_functor::numeric_command_functor( numeric_command_handler& handler )
 : command_functor( handler ),
 cmd_handler( handler )
{
}

void numeric_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   cmd_handler.retval.erase( );

   if( command == c_cmd_numeric_inc )
      cmd_handler.retval = to_string( ++*cmd_handler.p_numeric );
   else if( command == c_cmd_numeric_dec )
      cmd_handler.retval = to_string( --*cmd_handler.p_numeric );
   else if( command == c_cmd_numeric_add )
   {
      numeric n( get_parm_val( parameters, c_cmd_numeric_add_num ).c_str( ) );

      cmd_handler.retval = to_string( *cmd_handler.p_numeric += n );
   }
   else if( command == c_cmd_numeric_sub )
   {
      numeric n( get_parm_val( parameters, c_cmd_numeric_sub_num ).c_str( ) );

      cmd_handler.retval = to_string( *cmd_handler.p_numeric -= n );
   }
   else if( command == c_cmd_numeric_mul )
   {
      numeric n( get_parm_val( parameters, c_cmd_numeric_mul_num ).c_str( ) );

      cmd_handler.retval = to_string( *cmd_handler.p_numeric *= n );
   }
   else if( command == c_cmd_numeric_div )
   {
      numeric n( get_parm_val( parameters, c_cmd_numeric_div_num ).c_str( ) );

      cmd_handler.retval = to_string( *cmd_handler.p_numeric /= n );
   }
   else if( command == c_cmd_numeric_round )
   {
      numeric::round_method m( numeric::e_round_method_normal );
      if( has_parm_val( parameters, c_cmd_numeric_round_up ) )
         m = numeric::e_round_method_up;
      else if( has_parm_val( parameters, c_cmd_numeric_round_down ) )
         m = numeric::e_round_method_down;
      else if( has_parm_val( parameters, c_cmd_numeric_round_normal ) )
         m = numeric::e_round_method_normal;
      else if( has_parm_val( parameters, c_cmd_numeric_round_bankers ) )
         m = numeric::e_round_method_bankers;

      int n = atoi( get_parm_val( parameters, c_cmd_numeric_round_decimals ).c_str( ) );
      cmd_handler.retval = to_string( cmd_handler.p_numeric->round( n, m ) );
   }
}

command_functor* numeric_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new numeric_command_functor( dynamic_cast< numeric_command_handler& >( handler ) );
}

void execute_command( numeric& n, const string& cmd_and_args, string& retval )
{
   static numeric_command_handler cmd_handler;

   cmd_handler.set_numeric( &n );
   cmd_handler.execute_command( cmd_and_args );

   retval = cmd_handler.get_retval( );
}

