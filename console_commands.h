// Copyright (c) 2004
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

#ifndef CONSOLE_COMMANDS_H
#  define CONSOLE_COMMANDS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <deque>
#     include <vector>
#     include <string>
#  endif

#  include "command_handler.h"
#  include "command_processor.h"

const char* const c_prompt_prefix_option = "prompt_prefix";

class startup_command_processor;

class console_command_handler : public command_handler
{
   friend class startup_command_processor;

   public:
   console_command_handler( );

   bool has_option_quiet( ) const;

   bool is_handling_startup_options( ) const { return handling_startup_options; }

   void set_prompt_prefix( const std::string& prefix ) { set_option_value( c_prompt_prefix_option, prefix ); }

   void set_handling_startup_options( bool val ) { handling_startup_options = val; }

   size_t get_num_custom_startup_options( ) const { return num_custom_startup_options; }
   const std::string& get_custom_startup_options_usage( ) const { return custom_startup_options_usage; }

   private:
   void perform_after_command_changes( );

   std::string format_usage_output(
    const std::string& command, const std::string& usage, const std::string& description ) const;

   size_t line_number;
   std::string script_file;

   size_t description_offset;

   bool is_executing_commands;
   bool allow_history_addition;
   bool handling_startup_options;

   size_t num_custom_startup_options;
   std::string custom_startup_options_usage;

   std::vector< std::string > args;

   std::vector< bool > completed;
   std::vector< bool > conditions;
   std::vector< bool > dummy_conditions;

   std::string prompt_prefix;
   std::deque< std::string > command_history;

   protected:
   std::string preprocess_command_and_args( const std::string& cmd_and_args );

   void handle_unknown_command( const std::string& command );

   void handle_invalid_command( const command_parser& parser, const std::string& cmd_and_args );

   void handle_command_response( const std::string& response, bool is_special );

   void set_custom_startup_options( size_t num, const std::string& usage )
   {
      num_custom_startup_options = num;
      custom_startup_options_usage = usage;
   }

   virtual void process_custom_startup_option( size_t num, const std::string& option ) { }

   virtual void handle_extraneous_custom_option( const std::string& option );
};

class console_command_processor : public command_processor
{
   public:
   console_command_processor( command_handler& handler )
    : command_processor( handler ),
    handler( handler )
   {
   }

   private:
   command_handler& handler;

   bool is_still_processing( );

   std::string get_cmd_and_args( );

   void output_command_usage( const std::string& wildcard_match_expr ) const;
};

enum app_info_request
{
   e_app_info_request_title,
   e_app_info_request_version,
   e_app_info_request_title_and_version
};

typedef std::string ( *fp_get_app_info_string )( app_info_request );

class startup_command_processor : public command_processor
{
   public:
   startup_command_processor( console_command_handler& handler,
    fp_get_app_info_string get_app_info_string_func, int argc, char* argv[ ] );

   startup_command_processor( console_command_handler& handler,
    fp_get_app_info_string get_app_info_string_func, size_t group_num, int argc, char* argv[ ] );

   ~startup_command_processor( );

   private:
   std::vector< std::string > args;
   std::vector< std::string > extra_args;

   bool using_standard_console_options;

   console_command_handler& handler;
   fp_get_app_info_string get_app_info_string_func;

   bool is_still_processing( ) { return !args.empty( ); }
   std::string get_cmd_and_args( );

   void output_command_usage( const std::string& wildcard_match_expr ) const;

   void finalise_command_processing( );
};

#endif

