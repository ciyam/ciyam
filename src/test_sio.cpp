// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <memory>
#  include <string>
#  include <fstream>
#  include <iostream>
#  ifdef __GNUG__
#     include <unistd.h>
#  endif
#endif

#include "sio.h"
#include "config.h"
#include "macros.h"
#include "console.h"
#include "utilities.h"
#include "console_commands.h"

using namespace std;

#include "test_sio.cmh"

const char* const c_app_title = "test_sio";
const char* const c_app_version = "0.1";

const char* const c_error_prefix = "error: ";

const char* const c_env_var_output = "OUTPUT";

bool g_application_title_called = false;

string application_title( app_info_request request )
{
   g_application_title_called = true;

   if( request == e_app_info_request_title )
      return string( c_app_title );
   else if( request == e_app_info_request_version )
      return string( c_app_version );
   else if( request == e_app_info_request_title_and_version )
   {
      string title( c_app_title );

      title += " v" + string( c_app_version );

      return title;
   }
   else
   {
      ostringstream osstr;

      osstr << "unknown app_info_request: " << request;

      throw runtime_error( osstr.str( ) );
   }
}

class test_sio_command_functor;

class test_sio_command_handler : public console_command_handler
{
   friend class test_sio_command_functor;

   public:
   test_sio_command_handler( )
   {
   }

   private:
   unique_ptr< sio_graph > up_sio_graph;
};

class test_sio_command_functor : public command_functor
{
   public:
   test_sio_command_functor( test_sio_command_handler& sio_test_handler )
    : command_functor( sio_test_handler ),
    up_sio_graph( sio_test_handler.up_sio_graph )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   unique_ptr< sio_graph >& up_sio_graph;
};

void test_sio_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   try
   {
      if( command == c_cmd_test_sio_load )
      {
         string name( get_parm_val( parameters, c_cmd_test_sio_load_name ) );

         ifstream inpf( name.c_str( ) );

         sio_reader reader( inpf );

         up_sio_graph.reset( new sio_graph( reader ) );
      }
      else if( command == c_cmd_test_sio_save )
      {
         string name( get_parm_val( parameters, c_cmd_test_sio_save_name ) );

         if( !up_sio_graph.get( ) )
            cerr << "no sio content exists to save" << endl;
         else
         {
            if( name.empty( ) )
               write_graph( *up_sio_graph );
            else
            {
               ofstream outf( name.c_str( ) );

               write_graph( *up_sio_graph, &outf );
            }
         }
      }
      else if( command == c_cmd_test_sio_output )
      {
         bool json = has_parm_val( parameters, c_cmd_test_sio_output_json );
         string path( get_parm_val( parameters, c_cmd_test_sio_output_path ) );

         if( up_sio_graph.get( ) )
         {
            if( !json )
               write_graph( *up_sio_graph, 0, &path );
            else
            {
               const section_node* p_section_node = get_section_node_from_path( up_sio_graph->get_root_node( ), path );

               if( p_section_node )
                  convert_sio_to_json( *p_section_node, cout );
               else
                  cerr << "unable to find section '" << path << "'" << endl;
            }
         }
      }
      else if( command == c_cmd_test_sio_sections )
      {
         bool json = has_parm_val( parameters, c_cmd_test_sio_sections_json );
         string path( get_parm_val( parameters, c_cmd_test_sio_sections_path ) );

         if( up_sio_graph.get( ) )
         {
            const section_node* p_section_node = get_section_node_from_path( up_sio_graph->get_root_node( ), path );

            string output;

            if( !p_section_node )
               cerr << "unable to find starting section '" << path << "'" << endl;
            else
            {
               size_t num_children = p_section_node->get_num_child_nodes( );

               if( json )
                  output += "[\n";

               for( size_t i = 0; i < num_children; i++ )
               {
                  if( i > 0 )
                  {
                     if( json )
                        output += ',';

                     output += '\n';
                  }

                  if( !json )
                     output += to_comparable_string( i, false, 5 ) + ' ' + p_section_node->get_child_node( i ).get_name( );
                  else
                  {
                     output += " {\n  \"" + to_comparable_string( i, false, 5 ) + "\": \"" + p_section_node->get_child_node( i ).get_name( ) + "\"\n }";
                  }
               }

               if( json )
                  output += "\n]";
            }

            if( !output.empty( ) )
            {
               cout << output << endl;

               set_environment_variable( c_env_var_output, output );
            }
         }
      }
      else if( command == c_cmd_test_sio_available )
      {
         bool json = has_parm_val( parameters, c_cmd_test_sio_available_json );
         string path( get_parm_val( parameters, c_cmd_test_sio_available_path ) );

         if( up_sio_graph.get( ) )
         {
            const section_node* p_section_node = get_section_node_from_path( up_sio_graph->get_root_node( ), path );

            if( !p_section_node )
               cerr << "unable to find section '" << path << "'" << endl;
            else
            {
               string output;

               size_t num_attributes = p_section_node->get_num_attributes( );

               if( json )
                  output = "[\n";

               for( size_t i = 0; i < num_attributes; i++ )
               {
                  if( i > 0 )
                  {
                     if( json )
                        output += ',';

                     output += '\n';
                  }

                  if( json )
                     output += " \"";

                  output += p_section_node->get_attribute( i ).get_name( );

                  if( json )
                     output += '"';
               }

               if( json )
                  output += "\n]";

               cout << output << endl;

               set_environment_variable( c_env_var_output, output );
            }
         }
      }
      else if( command == c_cmd_test_sio_attributes )
      {
         bool json = has_parm_val( parameters, c_cmd_test_sio_attributes_json );
         string names( get_parm_val( parameters, c_cmd_test_sio_attributes_names ) );

         if( !up_sio_graph.get( ) )
            cerr << "no sio content is available" << endl;
         else
         {
            string output( get_attributes_for_name_query( up_sio_graph->get_root_node( ), names, json ) );

            string::size_type pos = output.find( '\n' );

            set_environment_variable( c_env_var_output, output.substr( 0, pos ) );

            if( !output.empty( ) )
               cout << output << endl;
         }
      }
      else if( command == c_cmd_test_sio_exit )
         handler.set_finished( );
   }
   catch( exception& x )
   {
      handler.issue_command_response( string( c_error_prefix ) + x.what( ), true );
   }
}

command_functor* test_sio_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new test_sio_command_functor( dynamic_cast< test_sio_command_handler& >( handler ) );
}

int main( int argc, char* argv[ ] )
{
   int rc = 0;

   test_sio_command_handler cmd_handler;

   try
   {
      // NOTE: Use block scope for startup command processor object...
      {
         startup_command_processor processor( cmd_handler, application_title, 0, argc, argv );

         processor.process_commands( );
      }

      if( !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;

      cmd_handler.add_commands( 0,
       test_sio_command_functor_factory, ARRAY_PTR_AND_SIZE( test_sio_command_definitions ) );

      console_command_processor processor( cmd_handler );

      processor.process_commands( );
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;

      rc = 1;
   }
   catch( ... )
   {
      cerr << "error: unexpected exception occurred" << endl;

      rc = 2;
   }

   return rc;
}
