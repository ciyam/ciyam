// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2017 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <memory>
#  include <sstream>
#  include <stdexcept>
#endif

#include "command_handler.h"

#include "utilities.h"
#include "command_parser.h"

using namespace std;

command_handler::~command_handler( )
{
   command_dispatcher_iterator i, end;
   for( i = command_dispatchers.begin( ), end = command_dispatchers.end( ); i != end; ++i )
   {
      delete i->second.p_parser;
      delete i->second.p_functor;
   }
}

void command_handler::add_option( const string& name )
{
   options.insert( make_pair( name, string( ) ) );
}

bool command_handler::has_option( const string& name ) const
{
   option_const_iterator i = options.find( name );
   return i != options.end( );
}

void command_handler::remove_option( const string& name )
{
   option_iterator i = options.find( name );
   if( i == options.end( ) )
      throw runtime_error( "unknown command handler option '" + name + "'" );

   options.erase( i );
}

const string& command_handler::get_option_value( const string& name ) const
{
   option_const_iterator i = options.find( name );
   if( i == options.end( ) )
      throw runtime_error( "unknown command handler option '" + name + "'" );

   return i->second;
}

void command_handler::set_option_value( const string& name, const string& value )
{
   option_iterator i = options.find( name );
   if( i == options.end( ) )
      throw runtime_error( "unknown command handler option '" + name + "'" );

   i->second = value;
}

void command_handler::add_command( const string& name, size_t group_num,
 const string& syntax, const string& description, command_functor* p_functor )
{
   auto_ptr< command_functor > ap_functor( p_functor );

   string dispatch_name( command_prefix );

   string::size_type pos = name.find( '|' );
   dispatch_name += name.substr( 0, pos );

   if( pos != string::npos )
      short_commands.insert( make_pair( name.substr( pos + 1 ), name.substr( 0, pos ) ) );

   auto_ptr< command_parser > ap_parser( new command_parser );
   ap_parser->parse_syntax( syntax.c_str( ) );

   command_items.push_back( command_item( dispatch_name, group_num, description ) );
   try
   {
      command_dispatchers.insert( make_pair( dispatch_name,
       command_dispatcher( name, ap_parser.release( ), ap_functor.release( ) ) ) );

      if( change_notify )
         perform_after_command_changes( );
   }
   catch( ... )
   {
      command_items.pop_back( );
      throw;
   }
}

void command_handler::add_command( const string& name, size_t group_num,
 const string& syntax, const string& description, command_functor_creator creator )
{
   add_command( name, group_num, syntax, description, creator( name, *this ) );
}

void command_handler::remove_command( const string& name )
{
   string dispatch_name( command_prefix );
   dispatch_name += name;

   string short_name;
   for( map< string, string >::iterator sci = short_commands.begin( ); sci != short_commands.end( ); ++sci )
   {
      if( sci->second == name )
      {
         short_name = sci->first;
         break;
      }
   }

   command_dispatcher_iterator i = command_dispatchers.find( dispatch_name );
   if( i == command_dispatchers.end( ) )
      handle_unknown_command( name );
   else
   {
      for( vector< command_item >::size_type j = 0; j < command_items.size( ); j++ )
      {
         if( dispatch_name == command_items[ j ].dispatch_name )
         {
            command_items.erase( command_items.begin( ) + j );
            break;
         }
      }

      delete i->second.p_parser;
      delete i->second.p_functor;
      command_dispatchers.erase( i );

      if( !short_name.empty( ) )
         short_commands.erase( short_name );

      if( change_notify )
         perform_after_command_changes( );
   }
}

void command_handler::execute_command( const string& cmd_and_args )
{
   command_and_args = cmd_and_args;

   if( is_special_command( cmd_and_args ) )
      handle_special_command( cmd_and_args );
   else
   {
      string s = preprocess_command_and_args( cmd_and_args );

      if( !s.empty( ) )
      {
         string::size_type pos = s.find( ' ' );
         string cmd( s.substr( 0, pos ) );

         if( short_commands.count( cmd ) )
            cmd = short_commands[ cmd ];

         command_dispatcher_const_iterator ci = command_dispatchers.find( cmd );

         if( ci == command_dispatchers.end( ) )
            handle_unknown_command( cmd );
         else
         {
            vector< string > arguments;
            map< string, string > parameters;

            bool valid = true;

            if( pos != string::npos )
            {
               try
               {
                  setup_arguments( s.substr( pos + 1 ).c_str( ), arguments );
               }
               catch( exception& )
               {
                  valid = false;
               }
            }

            if( valid && ci->second.p_parser->parse_command( arguments, parameters ) )
            {
               // NOTE: Place an empty pair of strings at the start of the map to help the "get_parm_val" function.
               parameters.insert( make_pair( string( ), string( ) ) );

               string name( ci->second.name );
               string::size_type pos = name.find( '|' );
               ci->second.p_functor->operator( )( name.substr( 0, pos ), parameters );
            }
            else
               handle_invalid_command( *ci->second.p_parser, s );
         }

         postprocess_command_and_args( s );
      }
   }
}

string command_handler::get_usage_for_command( const string& name ) const
{
   string cmd( name );
   if( short_commands.count( name ) )
      cmd = short_commands.find( name )->second;

   command_dispatcher_const_iterator ci = command_dispatchers.find( cmd );
   if( ci == command_dispatchers.end( ) )
      throw runtime_error( "command '" + cmd + "' was not found" );

   return ci->second.p_parser->get_usage( );
}

string command_handler::get_usage_for_all_commands( const string& wildcard_match_expr, char cmd_arg_separator ) const
{
   ostringstream osstr;
   bool is_first = true;
   size_t last_group_num;
   for( vector< command_item >::size_type i = 0; i < command_items.size( ); i++ )
   {
      if( !wildcard_match_expr.empty( ) )
      {
         if( !wildcard_match( wildcard_match_expr.c_str( ), command_items[ i ].dispatch_name.c_str( ) ) )
            continue;
      }

      command_dispatcher_const_iterator ci = command_dispatchers.find( command_items[ i ].dispatch_name );

      string usage( ci->second.p_parser->get_usage( ) );

      if( is_first )
      {
         is_first = false;
         last_group_num = command_items[ i ].group_num;
      }   
      else
      {
         if( last_group_num != command_items[ i ].group_num )
            osstr << '\n';
         last_group_num = command_items[ i ].group_num;
      }

      osstr << format_usage_output(
       command_items[ i ].dispatch_name, cmd_arg_separator, usage, command_items[ i ].description ) << '\n';
   }

   return osstr.str( );
}

void command_handler::add_options( option_definition definitions[ ], size_t num_options )
{
   for( size_t i = 0; i < num_options; i++ )
      add_option( definitions[ i ].p_name );
}

void command_handler::remove_options( option_definition definitions[ ], size_t num_options )
{
   for( size_t i = 0; i < num_options; i++ )
      remove_option( definitions[ i ].p_name );
}

void command_handler::add_commands( size_t group_num,
 command_functor_creator functor_creator, command_definition definitions[ ], size_t num_commands )
{
   restorable< bool > change_notify_restorer( change_notify );
   change_notify = false;

   for( size_t i = 0; i < num_commands; i++ )
      add_command( definitions[ i ].p_name, group_num,
       definitions[ i ].p_syntax, definitions[ i ].p_description, functor_creator );

   perform_after_command_changes( );
}

void command_handler::add_commands( size_t group_num, const string& prefix,
 command_functor_creator functor_creator, command_definition definitions[ ], size_t num_commands )
{
   restorable< string > command_prefix_restorer( command_prefix );
   command_prefix = prefix;

   add_commands( group_num, functor_creator, definitions, num_commands );
}

void command_handler::remove_commands( command_definition definitions[ ], size_t num_commands )
{
   restorable< bool > change_notify_restorer( change_notify );
   change_notify = false;

   for( size_t i = 0; i < num_commands; i++ )
      remove_command( definitions[ i ].p_name );

   perform_after_command_changes( );
}

void command_handler::remove_commands(
 const string& prefix, command_definition definitions[ ], size_t num_commands )
{
   restorable< string > command_prefix_restorer( command_prefix );
   command_prefix = prefix;

   remove_commands( definitions, num_commands );
}

string command_handler::format_usage_output( const string& command,
 char cmd_arg_separator, const string& usage, const string& description ) const
{
   string retval( command );

   for( map< string, string >::const_iterator ci = short_commands.begin( ); ci != short_commands.end( ); ++ci )
   {
      if( ci->second == command )
      {
         retval += '|' + ci->first;
         break;
      }
   }

   if( !usage.empty( ) )
   {
      retval += cmd_arg_separator;
      retval += usage;
   }

   if( !description.empty( ) )
   {
      retval += string( " (" );
      retval += description;
      retval += ')';
   }

   return retval;
}

