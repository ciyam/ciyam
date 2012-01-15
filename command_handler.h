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

#ifndef COMMAND_HANDLER_H
#  define COMMAND_HANDLER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <map>
#     include <vector>
#     include <string>
#     include <utility>
#  endif

#  include "progress.h"

#  ifdef CIYAM_BASE_LIB
#     ifdef CIYAM_BASE_IMPL
#        define COMMAND_HANDLER_DECL_SPEC DYNAMIC_EXPORT
#     else
#        define COMMAND_HANDLER_DECL_SPEC DYNAMIC_IMPORT
#     endif
#  else
#     define COMMAND_HANDLER_DECL_SPEC
#  endif

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
   command_item( const std::string& dispatch_name, size_t group_num, const std::string& description )
    :
    dispatch_name( dispatch_name ),
    group_num( group_num ),
    description( description )
   {
   }

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

class COMMAND_HANDLER_DECL_SPEC command_handler : public progress
{
   friend class command_processor;

   public:
   command_handler( )
    :
    finished( false ),
    change_notify( true ),
    quiet_command( false ),
    p_command_processor( 0 )
   {
   }

   virtual ~command_handler( );

   bool has_finished( ) const { return finished; }
   void set_finished( ) { finished = true; }

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

   void issue_command_reponse( const std::string& response, bool is_special = false )
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

   std::string command_prefix;
   std::string command_and_args;

   command_processor* p_command_processor;

   typedef std::map< std::string, std::string >::iterator option_iterator;
   typedef std::map< std::string, std::string >::const_iterator option_const_iterator;

   std::vector< command_item > command_items;
   std::map< std::string, command_dispatcher > command_dispatchers;
   typedef std::map< std::string, command_dispatcher >::iterator command_dispatcher_iterator;
   typedef std::map< std::string, command_dispatcher >::const_iterator command_dispatcher_const_iterator;

   std::map< std::string, std::string > options;

   virtual void perform_after_command_changes( ) { }

   virtual std::string format_usage_output( const std::string& command,
    char cmd_arg_separator, const std::string& usage, const std::string& description ) const;

   virtual std::string preprocess_command_and_args( const std::string& cmd_and_args ) { return cmd_and_args; }

   virtual void handle_unknown_command( const std::string& command ) = 0;
   virtual void handle_invalid_command( const command_parser& parser, const std::string& cmd_and_args ) = 0;

   virtual void handle_command_response( const std::string& response, bool is_special ) { }

   protected:
   bool get_is_quiet_command( ) const { return quiet_command; }
   void set_quiet_command( bool new_val ) { quiet_command = new_val; }

   std::string get_cmd_and_args( ) const { return command_and_args; }
   size_t get_num_commands( ) const { return command_dispatchers.size( ); }

   const std::string& get_command_name( size_t num ) const { return command_items[ num ].dispatch_name; }
};

#endif

