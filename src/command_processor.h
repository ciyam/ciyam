// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef COMMAND_PROCESSOR_H
#  define COMMAND_PROCESSOR_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <stack>
#     include <string>
#  endif

class command_handler;

class command_processor
{
   public:
   command_processor( command_handler& handler );

   void process_commands( );

   void execute_command( const std::string& cmd_and_args );

   std::string get_usage_for_commands( const std::string& wildcard_match_expr, char cmd_arg_separator = ' ' ) const;

   void push_handler( command_handler& handler );
   void pop_handler( );

   virtual void output_command_usage( const std::string& wildcard_match_expr ) const = 0;

   private:
   virtual bool is_still_processing( ) = 0;

   virtual void get_cmd_and_args( std::string& cmd_and_args ) = 0;

   virtual void finalise_command_processing( ) { }

   bool is_continuation;
   std::stack< command_handler* > handlers;

   protected:
   bool get_is_continuation( ) const { return is_continuation; }
};

#endif
