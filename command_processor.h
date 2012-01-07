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
   virtual std::string get_cmd_and_args( ) = 0;

   virtual void finalise_command_processing( ) { }

   bool is_continuation;
   std::stack< command_handler* > handlers;

   protected:
   bool get_is_continuation( ) const { return is_continuation; }
};

#endif

