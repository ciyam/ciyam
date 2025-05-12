// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CONSOLE_COMMANDS_H
#  define CONSOLE_COMMANDS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <map>
#     include <set>
#     include <deque>
#     include <stack>
#     include <iosfwd>
#     include <string>
#     include <vector>
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
   ~console_command_handler( );

   bool has_option_echo( ) const;
   bool has_option_quiet( ) const;

   bool has_option_no_prompt( ) const;
   bool has_option_no_progress( ) const;

   bool is_redirected_output( ) const;

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

   size_t input_depth;

   size_t history_offset;
   size_t max_history_lines;

   std::ostream* p_std_err;
   std::ostream* p_std_out;

   std::string script_file;

   size_t description_offset;

   bool ignore_prior;
   bool is_reading_input;
   bool is_skipping_to_label;
   bool is_executing_commands;
   bool allow_history_addition;
   bool handling_startup_options;

   size_t num_custom_startup_options;
   std::string custom_startup_options_usage;

   std::vector< std::string > args;

   std::vector< bool > completed;
   std::vector< bool > conditions;
   std::vector< bool > dummy_conditions;

   std::vector< size_t > lines_for_conditions;

   std::set< std::string > next_retaining;

   std::stack< std::set< std::string > > variables_retaining;
   std::stack< std::map< std::string, std::string > > variables_prior_values;

   std::string label;

   std::string prior_command;
   std::string prompt_prefix;

   std::deque< size_t > history_line_number;
   std::deque< std::string > command_history;

   struct impl;
   impl* p_impl;

   protected:
   void preprocess_command_and_args( std::string& str, const std::string& cmd_and_args, bool skip_command_usage = false );

   std::ostream* get_std_err( ) const { return p_std_err; }
   std::ostream* get_std_out( ) const { return p_std_out; }

   bool is_special_command( const std::string& cmd_and_args );

   void handle_special_command( const std::string& cmd_and_args );

   void handle_unknown_command( const std::string& command, const std::string& cmd_and_args );

   void handle_invalid_command( const command_parser& parser, const std::string& cmd_and_args );

   void handle_command_response( const std::string& response, bool is_special = false );

   void handle_progress_message( const std::string& message );

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

   void get_cmd_and_args( std::string& cmd_and_args );

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

   void get_cmd_and_args( std::string& cmd_and_args );

   void output_command_usage( const std::string& wildcard_match_expr ) const;

   void finalise_command_processing( );
};

#endif
