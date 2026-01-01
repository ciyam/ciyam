// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <vector>
#  include <string>
#  include <fstream>
#  include <iostream>
#endif

#include "utilities.h"
#include "command_parser.h"

//#define DEBUG

using namespace std;

const char* const c_app_title = "generate_commands";
const char* const c_app_version = "0.1";
const char* const c_app_usage = "usage: generate_commands [-p[<prefix>]] <input_file.cms> [<output_file.cmh>]";

const char* const c_arg_prefix = "-p";

const char* const c_error_prefix = "error:";

const char* const c_default_input_file_extension = ".cms";
const char* const c_default_output_file_extension = ".cmh";

const char* const c_const_int_prefix = "const int c_cmd_";
const char* const c_const_char_prefix = "const char* const c_cmd_";

const char* const c_output_command_prefix = "const char* const c_cmd_";

const char* const c_output_command_syntax_suffix = "_syntax";
const char* const c_output_command_default_suffix = "_default";

const char* const c_output_command_definition = "command_definition";
const char* const c_output_command_definition_suffix = "command_definitions[ ] =";

struct command_item
{
   command_item( const string& name, const string& syntax, const string& description )
    :
    name( name ),
    syntax( syntax ),
    description( description )
   {
   }

   string name;
   string syntax;
   string description;
};

int main( int argc, char* argv[ ] )
{
   if( argc < 2
    || ( string( argv[ 1 ] ) == "?" || string( argv[ 1 ] ) == "/?" || string( argv[ 1 ] ) == "-?" ) )
   {
      cout << c_app_title << " v" << c_app_version << '\n';
      cout << c_app_usage << endl;
      return 0;
   }

   int first_arg = 1;
   bool had_prefix_option = false;

   string command_constant_prefix;

   while( first_arg < argc && argv[ first_arg ][ 0 ] == '-' )
   {
      if( string( argv[ first_arg ] ).substr( 0, 2 ) == string( c_arg_prefix ) )
      {
         command_constant_prefix = string( argv[ first_arg ] ).substr( 2 );
         had_prefix_option = true;
         ++first_arg;
      }
      else
      {
         cerr << "error: invalid command line option '" << argv[ first_arg ] << "'" << endl;
         return 1;
      }
   }

   if( first_arg == argc )
   {
      cerr << "error: must specify an input filename" << endl;
      return 1;
   }

   string input_file_name( argv[ first_arg ] );
   string::size_type pos = input_file_name.find( '.' );
   if( pos == string::npos )
      input_file_name += string( c_default_input_file_extension );

   ifstream inpf( input_file_name.c_str( ) );
   if( !inpf )
   {
      cerr << "error: unable to open file '" << argv[ first_arg ] << "' for input" << endl;
      return 1;
   }

   string next;
   if( !had_prefix_option )
   {
      pos = input_file_name.find_last_of( '.' );
      command_constant_prefix = input_file_name.substr( 0, pos );
      command_constant_prefix += '_';
   }

   cout << input_file_name << ':' << endl;

   string output_file_name;
   if( first_arg < argc - 1 )
      output_file_name = string( argv[ first_arg + 1 ] );
   else
      output_file_name = input_file_name.substr( 0, input_file_name.find_last_of( '.' ) );

   pos = output_file_name.find( '.' );
   if( pos == string::npos )
      output_file_name += c_default_output_file_extension;

   ofstream outf( output_file_name.c_str( ) );
   if( !outf )
   {
      cerr << "error: unable to open file '" << output_file_name << "' for output" << endl;
      return 1;
   }

   command_parser p;
   vector< command_item > command_syntax_items;
   typedef vector< command_item >::size_type command_syntax_size_type;
   typedef vector< command_item >::value_type command_syntax_value_type;

   outf << "// NOTE: DO NOT EDIT FILE (this file was automatically generated)\n";

   size_t line_number = 0;
   while( getline( inpf, next ) )
   {
      ++line_number;

      remove_trailing_cr_from_text_file_line( next );
#ifdef DEBUG
      cout << "next line is: " << next << endl;
#endif

      // NOTE: If a line begins with "//" it will be ignored (i.e. as a comment).
      if( next.size( ) >= 2 && next[ 0 ] == '/' && next[ 1 ] == '/' )
         continue;

      pos = next.find( ' ' );
      string command( next.substr( 0, pos ) );

      string short_cmd;
      string::size_type spos = command.find( '|' );
      if( spos != string::npos )
      {
         short_cmd = command.substr( spos );
         command.erase( spos );
      }

#ifdef DEBUG
      cout << " command is: " << command << endl;
#endif
      if( command.empty( ) )
         continue;

      outf << '\n' << c_output_command_prefix << command_constant_prefix
       << command << " = \"" << command << "\";" << '\n';

      string syntax;
      string description;
      if( pos != string::npos )
      {
         next.erase( 0, pos + 1 );

         if( next[ 0 ] == '"' )
         {
            pos = next.find( '"', 1 );
            if( pos == string::npos )
            {
               cerr << "error: unexpected description missing end-quote at line #" << line_number << endl;
               return 1;
            }

            description = next.substr( 1, pos - 1 );
            next.erase( 0, pos + 1 );
#ifdef DEBUG
            cout << " description is: " << description << endl;
#endif
         }

         if( !next.empty( ) )
         {
            if( !description.empty( ) )
            {
               if( next[ 0 ] != ' ' )
               {
                  cerr << "error: unexpected missing space after description at line #" << line_number << endl;
                  return 1;
               }

               next.erase( 0, 1 );
            }

            syntax = next;
#ifdef DEBUG
            cout << " command syntax is: " << description << endl;
#endif
            p.parse_syntax( syntax.c_str( ) );
            if( !p.okay( ) )
            {
               cerr << "error: invalid syntax found in command '" << command << "' at line #" << line_number;
               string error_marker( p.get_error_pos( ), ' ' );
               error_marker += '^';
               cerr << '\n' << syntax << '\n' << error_marker << endl;
               return 1;
            }

            vector< string > parameters;
            vector< string > default_vals;

            p.get_parameter_names( parameters, &default_vals );

            for( vector< string >::size_type i = 0; i < parameters.size( ); i++ )
            {
               outf << c_output_command_prefix << command_constant_prefix
                << command << '_' << parameters[ i ] << " = \"" << parameters[ i ] << "\";\n";

               if( !default_vals[ i ].empty( ) )
               {
                  string default_constant( default_vals[ i ] );

                  bool is_number = true;

                  for( size_t j = 0; j < default_constant.size( ); j++ )
                  {
                     if( default_constant[ j ] < '0' || default_constant[ j ] > '9' )
                     {
                        is_number = false;
                        break;
                     }
                  }

                  string const_prefix( c_const_int_prefix );

                  if( !is_number )
                  {
                     const_prefix = string( c_const_char_prefix );
                     default_constant = "\"" + default_constant + "\"";
                  }

                  outf << const_prefix << command_constant_prefix
                   << command << '_' << parameters[ i ]
                   << c_output_command_default_suffix << " = " << default_constant << ";\n";
               }
            }
         }
      }

      command_syntax_items.push_back( command_syntax_value_type( command + short_cmd, syntax, description ) );
   }

   if( !inpf.eof( ) )
   {
      cerr << "error: unexpected error occurred whilst reading '" << input_file_name << "' for input" << endl;
      return 1;
   }

   outf << '\n' << c_output_command_definition << ' '
    << command_constant_prefix << c_output_command_definition_suffix << '\n';

   outf << "{\n";
   for( command_syntax_size_type i = 0; i < command_syntax_items.size( ); i++ )
   {
      if( i != 0 )
         outf << ",\n";

      outf << "   { "
       << "\"" << command_syntax_items[ i ].name
       << "\", \"" << command_syntax_items[ i ].syntax
       << "\", \"" << command_syntax_items[ i ].description << "\" }";
   }
   outf << "\n};\n";

   if( !outf.good( ) )
   {
      cerr << "error: unexpected write error for file '" << output_file_name << "'" << endl;
      return 1;
   }
}

