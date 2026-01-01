// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
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
#  include <sstream>
#  include <iostream>
#  include <stdexcept>
#endif

#include "pdf_gen.h"

#include "utilities.h"
#include "console_commands.h"

using namespace std;

#include "test_pdf_gen.cmh"

string g_format_file;
string g_output_file;
string g_variables_file;

string_container g_string_variables;

const char* const c_app_title = "test_pdf_gen";
const char* const c_app_version = "0.1";

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
      title += " v";
      title += string( c_app_version );

      return title;
   }
   else
   {
      ostringstream osstr;
      osstr << "unknown app_info_request: " << request;
      throw runtime_error( osstr.str( ) );
   }
}

const char* const c_grid_variable = "@grid";
const char* const c_true_variable = "@true";
const char* const c_false_variable = "@false";
const char* const c_footer_size_variable = "@footer_size";
const char* const c_header_size_variable = "@header_size";

const char* const c_grid_normal = "normal";
const char* const c_grid_reverse = "reverse";

class test_pdf_gen_command_functor;

class test_pdf_gen_command_handler : public console_command_handler
{
   friend class test_pdf_gen_command_functor;

   public:
   test_pdf_gen_command_handler( )
   {
      set_custom_startup_options( 3, "[<format_file>] [<variables_file>] [<output_file>]" );

      g_string_variables.insert( make_pair( c_true_variable, "True" ) );
      g_string_variables.insert( make_pair( c_false_variable, "False" ) );
   }

   void process_custom_startup_option( size_t num, const string& option );
};

void test_pdf_gen_command_handler::process_custom_startup_option( size_t num, const string& option )
{
   if( num == 0 )
      g_format_file = option;
   else if( num == 1 )
      g_variables_file = option;
   else
      g_output_file = option;
}

class test_pdf_gen_command_functor : public command_functor
{
   public:
   test_pdf_gen_command_functor( test_pdf_gen_command_handler& pdf_view_handler )
    : command_functor( pdf_view_handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );
};

void test_pdf_gen_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   try
   {
      if( command == c_cmd_test_pdf_gen_set )
      {
         string name( get_parm_val( parameters, c_cmd_test_pdf_gen_set_name ) );
         string value( get_parm_val( parameters, c_cmd_test_pdf_gen_set_value ) );

         g_string_variables.insert( make_pair( name, value ) );
      }
      else if( command == c_cmd_test_pdf_gen_clear )
      {
         g_string_variables.clear( );
      }
      else if( command == c_cmd_test_pdf_gen_list )
      {
         cout << "\nvariables:\n";
         cout << "==========\n";

         for( map< string, string >::iterator
          i = g_string_variables.begin( ); i != g_string_variables.end( ); ++i )
         {
            cout << '[' << i->first << "] " << i->second << '\n';
         }
      }
      else if( command == c_cmd_test_pdf_gen_read_vars )
      {
         string filename( get_parm_val( parameters, c_cmd_test_pdf_gen_read_vars_filename ) );

         g_string_variables.clear( );
         read_strings( filename, g_string_variables );
      }
      else if( command == c_cmd_test_pdf_gen_grid )
      {
         bool is_on( has_parm_val( parameters, c_cmd_test_pdf_gen_grid_on ) );
         bool is_off( has_parm_val( parameters, c_cmd_test_pdf_gen_grid_off ) );
         bool is_reverse( has_parm_val( parameters, c_cmd_test_pdf_gen_grid_reverse ) );

         if( is_on )
            g_string_variables.insert( make_pair( c_grid_variable, is_reverse ? c_grid_reverse : c_grid_normal ) );
         else
            g_string_variables.erase( c_grid_variable );
      }
      else if( command == c_cmd_test_pdf_gen_header )
      {
         string size( get_parm_val( parameters, c_cmd_test_pdf_gen_header_size ) );

         if( !size.empty( ) )
            g_string_variables[ c_header_size_variable ] = size;
         else
         {
            float size = 0.0;
            if( g_string_variables.count( c_header_size_variable ) )
               size = atof( g_string_variables[ c_header_size_variable ].c_str( ) );

            cout << size << endl;
         }
      }
      else if( command == c_cmd_test_pdf_gen_footer )
      {
         string size( get_parm_val( parameters, c_cmd_test_pdf_gen_footer_size ) );

         if( !size.empty( ) )
            g_string_variables[ c_footer_size_variable ] = size;
         else
         {
            float size = 0.0;
            if( g_string_variables.count( c_footer_size_variable ) )
               size = atof( g_string_variables[ c_footer_size_variable ].c_str( ) );

            cout << size << endl;
         }
      }
      else if( command == c_cmd_test_pdf_gen_format )
      {
         string format_filename( get_parm_val( parameters, c_cmd_test_pdf_gen_format_filename ) );

         g_format_file = format_filename;
      }
      else if( command == c_cmd_test_pdf_gen_generate )
      {
         string filename( get_parm_val( parameters, c_cmd_test_pdf_gen_generate_filename ) );

         generate_pdf_doc( g_format_file, filename, g_string_variables );

         cout << "created " << filename << endl;
      }
      else if( command == c_cmd_test_pdf_gen_exit )
         handler.set_finished( );
   }
   catch( exception& x )
   {
      cout << "error: " << x.what( ) << endl;
   }
}

command_functor* test_pdf_gen_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new test_pdf_gen_command_functor( dynamic_cast< test_pdf_gen_command_handler& >( handler ) );
}

int main( int argc, char* argv[ ] )
{
   test_pdf_gen_command_handler cmd_handler;

   try
   {
      // NOTE: Use block scope for startup command processor object...
      {
         startup_command_processor processor( cmd_handler, application_title, 0, argc, argv );

         processor.process_commands( );
      }

      if( !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;

      if( !g_variables_file.empty( ) )
      {
         g_string_variables.clear( );
         read_strings( g_variables_file, g_string_variables );

         g_string_variables.insert( make_pair( c_true_variable, "True" ) );
         g_string_variables.insert( make_pair( c_false_variable, "False" ) );
      }

      if( !g_output_file.empty( ) )
      {
         generate_pdf_doc( g_format_file, g_output_file, g_string_variables );

         if( !cmd_handler.has_option_quiet( ) )
            cout << "created " << g_output_file << endl;
      }
      else
      {
         cmd_handler.add_commands( 0,
          test_pdf_gen_command_functor_factory, ARRAY_PTR_AND_SIZE( test_pdf_gen_command_definitions ) );

         console_command_processor processor( cmd_handler );
         processor.process_commands( );
      }
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
      return 1;
   }
   catch( ... )
   {
      cerr << "error: unexpected exception occurred" << endl;
      return 2;
   }
}

