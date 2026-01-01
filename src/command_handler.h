// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef COMMAND_HANDLER_H
#  define COMMAND_HANDLER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <map>
#     include <vector>
#     include <string>
#     include <utility>
#  endif

#  include "progress.h"

class command_parser;
class command_handler;
class command_processor;

struct option_definition
{
   const char* p_name;
};

struct command_definition
{
   const char* p_name;
   const char* p_syntax;
   const char* p_description;
};

typedef std::map< std::string, std::string > parameter_info;

class command_functor
{
   public:
   command_functor( command_handler& handler )
    :
    handler( handler )
   {
   }

   virtual void operator ( )(
    const std::string& command, const parameter_info& parameters ) = 0;

   protected:
   command_handler& handler;
};

inline bool has_parm_val( const parameter_info& parameters, const std::string& parameter_name )
{
   return parameters.find( parameter_name ) != parameters.end( );
}

inline const std::string& get_parm_val( const parameter_info& parameters, const std::string& parameter_name )
{
   std::map< std::string, std::string >::const_iterator ci = parameters.find( parameter_name );

   // NOTE: The parameters container is assumed to always have an empty pair of strings at its beginning.
   if( ci == parameters.end( ) )
      ci = parameters.begin( );

   return ci->second;
}

typedef command_functor* command_functor_creator( const std::string& name, command_handler& handler );

struct command_item
{
   command_item( const std::string& short_name,
    const std::string& dispatch_name, size_t group_num, const std::string& description )
    :
    short_name( short_name ),
    dispatch_name( dispatch_name ),
    group_num( group_num ),
    description( description )
   {
   }

   std::string short_name;
   std::string dispatch_name;

   size_t group_num;
   std::string description;
};

struct command_dispatcher
{
   command_dispatcher( const std::string& name, command_parser* p_parser, command_functor* p_functor )
    :
    name( name ),
    p_parser( p_parser ),
    p_functor( p_functor )
   {
   }

   std::string name;
   command_parser* p_parser;
   command_functor* p_functor;
};

class command_handler : public progress
{
   friend class command_processor;

   public:
   command_handler( );

   virtual ~command_handler( );

   bool has_finished( ) const { return finished; }
   void set_finished( ) { finished = true; }

   bool get_is_quiet_command( ) const { return quiet_command; }
   bool is_additional_command( ) const { return additional_command; }

   void add_option( const std::string& name );
   bool has_option( const std::string& name ) const;
   void remove_option( const std::string& name );

   const std::string& get_option_value( const std::string& name ) const;
   void set_option_value( const std::string& name, const std::string& value );

   void add_command( const std::string& name, size_t group_num,
    const std::string& syntax, const std::string& description, command_functor* p_functor );

   void add_command( const std::string& name, size_t group_num,
    const std::string& syntax, const std::string& description, command_functor_creator creator );

   void remove_command( const std::string& name );

   void execute_command( const std::string& cmd_and_args );

   void do_execute_command( const std::string& cmd_and_args );

   void issue_command_response( const std::string& response, bool is_special = false )
   {
      handle_command_response( quiet_command ? std::string( ) : response, is_special );
   }

   std::string get_usage_for_command( const std::string& name ) const;
   std::string get_usage_for_all_commands( const std::string& wildcard_match_expr, char cmd_arg_separator = ' ' ) const;

   command_processor* get_command_processor( ) const { return p_command_processor; }

   void add_options( option_definition definitions[ ], size_t num_options );
   void remove_options( option_definition definitions[ ], size_t num_options );

   void add_commands( size_t group_num,
    command_functor_creator functor_creator, command_definition definitions[ ], size_t num_commands );

   void add_commands( size_t group_num, const std::string& prefix,
    command_functor_creator functor_creator, command_definition definitions[ ], size_t num_commands );

   void remove_commands( command_definition definitions[ ], size_t num_commands );
   void remove_commands( const std::string& prefix, command_definition definitions[ ], size_t num_commands );

   private:
   bool finished;
   bool change_notify;
   bool quiet_command;
   bool additional_command;

   std::string command_prefix;

   command_processor* p_command_processor;

   typedef std::map< std::string, std::string >::iterator option_iterator;
   typedef std::map< std::string, std::string >::const_iterator option_const_iterator;

   std::vector< command_item > command_items;
   std::map< std::string, std::string > short_commands;
   std::map< std::string, command_dispatcher > command_dispatchers;
   typedef std::map< std::string, command_dispatcher >::iterator command_dispatcher_iterator;
   typedef std::map< std::string, command_dispatcher >::const_iterator command_dispatcher_const_iterator;

   std::map< std::string, std::string > options;

   virtual void perform_after_command_changes( ) { }

   virtual std::string format_usage_output( const std::string& command,
    char cmd_arg_separator, const std::string& usage, const std::string& description ) const;

   virtual void preprocess_command_and_args( std::string& str,
    const std::string& cmd_and_args, bool skip_command_usage = false )
   {
      str = cmd_and_args;
   }

   virtual void postprocess_command_and_args( const std::string& /*cmd_and_args*/ ) { }

   virtual bool is_special_command( const std::string& /*cmd_and_args*/ ) { return false; }

   virtual void handle_special_command( const std::string& /*cmd_and_args*/ ) { }

   virtual void handle_unknown_command( const std::string& command, const std::string& cmd_and_args ) = 0;
   virtual void handle_invalid_command( const command_parser& parser, const std::string& cmd_and_args ) = 0;

   virtual void handle_command_response( const std::string& response, bool is_special = false ) { }

   protected:
   virtual std::string get_additional_command( ) { return std::string( ); }

   void set_quiet_command( bool new_val ) { quiet_command = new_val; }

   size_t get_num_commands( ) const { return command_dispatchers.size( ); }

   const std::string& get_command_name( size_t num ) const { return command_items[ num ].dispatch_name; }
};

#endif

