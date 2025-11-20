// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  ifdef __GNUG__
#     include <unistd.h>
#  endif
#  include <memory.h>
#  include <cstdio>
#  include <set>
#  include <memory>
#  include <vector>
#  include <string>
#  include <sstream>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#endif

#ifdef __GNUG__
#  define _access access
#endif

#include "sio.h"
#include "utilities.h"

using namespace std;

//#define DEBUG

enum output_type
{
   e_output_type_none,
   e_output_type_generate,
   e_output_type_automatic
};

struct test_step
{
   string name;
   string exec;

   bool has_input;
   output_type output;
};

struct test
{
   string name;
   string description;

   vector< string > kills;

   vector< test_step > test_steps;
};

struct group
{
   string name;
   vector< test > tests;
};

enum error_stop_point
{
   e_error_stop_point_none,
   e_error_stop_point_step,
   e_error_stop_point_test,
   e_error_stop_point_group
};

bool g_is_quiet = false;

string g_files_directory = ".";
string g_tests_directory = ".";

size_t g_num_test_steps_captured = 0;
size_t g_num_test_steps_attempted = 0;
size_t g_num_test_steps_succeeded = 0;
size_t g_num_test_steps_mismatched = 0;

error_stop_point g_error_stop = e_error_stop_point_step;

vector< group > g_groups;

typedef vector< group >::iterator group_iterator;
typedef vector< group >::size_type group_size_type;

vector< string > g_all_kills;

const char c_env_var_prefix = '$';

const char* const c_true = "true";
const char* const c_false = "false";

const char* const c_section_group = "group";
const char* const c_section_groups = "groups";

const char* const c_section_test = "test";
const char* const c_section_tests = "tests";

const char* const c_section_test_step = "test_step";

const char* const c_group_name_value = "name";

const char* const c_test_name_value = "name";
const char* const c_test_description_value = "description";

const char* const c_test_step_name_value = "name";
const char* const c_test_step_kill_value = "kill";
const char* const c_test_step_exec_value = "exec";
const char* const c_test_step_input_value = "input";
const char* const c_test_step_output_value = "output";

const char* const c_output_type_none = "none";
const char* const c_output_type_generate = "generate";
const char* const c_output_type_automatic = "automatic";

const char* const c_special_test_step_name_init = "*init*";
const char* const c_special_test_step_name_term = "*term*";

const char* const c_null_device_name = "/dev/null";

const char* const c_files_directory_var_name = "FILES_DIR";
const char* const c_tests_directory_var_name = "TESTS_DIR";

const int c_buffer_size = 32768;

bool exists( const char* p_file_name )
{
   return _access( p_file_name, 0 ) == 0;
}

void remove_file( const char* p_file_name )
{
   if( !exists( p_file_name ) )
      return;

   if( remove( p_file_name ) != 0 )
      throw runtime_error( "unable to remove file '" + string( p_file_name ) + "'" );
}

void copy_file( const char* p_input_file, const char* p_output_file )
{
   ifstream inpf( p_input_file, ios::in | ios::binary );

   if( !inpf )
      throw runtime_error( "unable to open file '" + string( p_input_file ) + "' for input" );

   ofstream outf( p_output_file, ios::out | ios::trunc | ios::binary );

   if( !outf )
      throw runtime_error( "unable to open file '" + string( p_output_file ) + "' for output" );

   inpf.seekg( 0, ios::end );

   if( !inpf.good( ) )
      throw runtime_error( "unexpected seek error for '" + string( p_input_file ) + "'" );

   size_t length = ( size_t )inpf.tellg( );

   inpf.seekg( 0, ios::beg );

   if( !inpf.good( ) )
      throw runtime_error( "unexpected seek error for '" + string( p_input_file ) + "'" );

   unique_ptr< char > up_buffer( new char[ c_buffer_size ] );

   for( size_t i = 0; i < length - 1; i += c_buffer_size )
   {
      size_t chunk = c_buffer_size;

      if( ( i + chunk ) > ( length - 1 ) )
         chunk = length - i;

      inpf.read( up_buffer.get( ), chunk );

      if( !inpf.good( ) )
         throw runtime_error( "unexpected read error for '" + string( p_input_file ) + "'" );

      outf.write( up_buffer.get( ), chunk );

      if( !outf.good( ) )
         throw runtime_error( "unexpected write error for '" + string( p_output_file ) + "'" );
   }
}

bool files_differ( const char* p_file_name_1, const char* p_file_name_2 )
{
   ifstream inpf1( p_file_name_1, ios::in | ios::binary );

   if( !inpf1 )
      throw runtime_error( "unable to open file '" + string( p_file_name_1 ) + "' for input" );

   ifstream inpf2( p_file_name_2, ios::in | ios::binary );

   if( !inpf2 )
      throw runtime_error( "unable to open file '" + string( p_file_name_2 ) + "' for input" );

   inpf1.seekg( 0, ios::end );

   if( !inpf1.good( ) )
      throw runtime_error( "unexpected seek error for '" + string( p_file_name_1 ) + "'" );

   inpf2.seekg( 0, ios::end );

   if( !inpf2.good( ) )
      throw runtime_error( "unexpected seek error for '" + string( p_file_name_2 ) + "'" );

   bool different = false;

   if( inpf1.tellg( ) != inpf2.tellg( ) )
      different = true;
   else
   {
      size_t length = ( size_t )inpf1.tellg( );

      inpf1.seekg( 0, ios::beg );

      if( !inpf1.good( ) )
         throw runtime_error( "unexpected seek error for '" + string( p_file_name_1 ) + "'" );

      inpf2.seekg( 0, ios::beg );

      if( !inpf2.good( ) )
         throw runtime_error( "unexpected seek error for '" + string( p_file_name_2 ) + "'" );

      unique_ptr< char > up_buffer1( new char[ c_buffer_size ] );
      unique_ptr< char > up_buffer2( new char[ c_buffer_size ] );

      for( size_t i = 0; i < length - 1; i += c_buffer_size )
      {
         size_t chunk = c_buffer_size;

         if( ( i + chunk ) > ( length - 1 ) )
            chunk = length - i;

         inpf1.read( up_buffer1.get( ), chunk );

         if( !inpf1.good( ) )
            throw runtime_error( "unexpected read error for '" + string( p_file_name_1 ) + "'" );

         inpf2.read( up_buffer2.get( ), chunk );

         if( !inpf2.good( ) )
            throw runtime_error( "unexpected read error for '" + string( p_file_name_2 ) + "'" );

         if( memcmp( up_buffer1.get( ), up_buffer2.get( ), chunk ) != 0 )
         {
            different = true;
            break;
         }
      }
   }

   return different;
}

void test_sio_reader_writer( const string& file_name )
{
   ifstream inpf( file_name.c_str( ) );

   if( !inpf )
      throw runtime_error( "unable to open file '" + file_name + "' for input" );

   // NOTE: Initial comments are found at the very top of the sio file.
   vector< string > initial_comments;

   sio_reader reader( inpf, true, &initial_comments );

   ostringstream osstr;

   sio_writer writer( osstr, &initial_comments );

   string comment;

   while( reader )
   {
      while( reader.has_read_comment( comment ) )
         writer.write_comment( comment );

      string name, value;

      while( reader.has_started_section( name ) )
      {
         writer.start_section( name );

         name.erase( );
      }

      while( reader.has_read_comment( comment ) )
         writer.write_comment( comment );

      while( reader.has_read_attribute( name, value ) )
      {
         writer.write_attribute( name, value );

         name.erase( );
      }

      while( reader.has_read_comment( comment ) )
         writer.write_comment( comment );

      while( reader.has_finished_section( name ) )
      {
         writer.finish_section( name );

         name.erase( );
      }
   }

   inpf.clear( );
   inpf.seekg( 0, ios::beg );

   istringstream isstr( osstr.str( ) );

   if( reader.get_next_line_num( ) > 0 )
   {
      string orig_line, copy_line;

      size_t copy_lines = 0;
      size_t orig_lines = reader.get_next_line_num( ) - 1;

      while( getline( inpf, orig_line ) )
      {
         remove_trailing_cr_from_text_file_line( orig_line );

         if( !getline( isstr, copy_line ) )
            break;

         if( orig_line != copy_line )
            break;

         ++copy_lines;
      }

      if( orig_lines != copy_lines )
      {
         ostringstream osstr;

         osstr << "unexpected sio reader/writer mismatch (only matched "
          << copy_lines << " of " << orig_lines << " total lines)";

         throw runtime_error( osstr.str( ) );
      }
   }
}

struct specific_test_spec
{
   string group;
   string tests;
   string steps;
};

size_t group_index( const string& group_name )
{
   for( size_t i = 0; i < g_groups.size( ); i++ )
      if( g_groups[ i ].name == group_name )
         return i + 1;

   return 0;
}

size_t group_test_index( const group& g, const string& test_name )
{
   for( size_t i = 0; i < g.tests.size( ); i++ )
      if( g.tests[ i ].name == test_name )
         return i + 1;

   return 0;
}

size_t group_test_step_index( const test& t, const string& step_name )
{
   for( size_t i = 0; i < t.test_steps.size( ); i++ )
      if( t.test_steps[ i ].name == step_name )
         return i + 1;

   return 0;
}

void init_group( const group& g, string& group_name )
{
   group_name = g.name;

   if( !g_is_quiet )
      cout << '[' << group_name << "]\n";
}

void init_group_test( const test& t, string& test_name )
{
   test_name += '_';
   test_name += t.name;

   if( !g_is_quiet )
      cout << ' ' << test_name << " - " << t.description << '\n';

   for( vector< string >::size_type i = 0; i < t.kills.size( ); i++ )
   {
      string next( t.kills[ i ] );

      if( !next.empty( ) )
      {
         // NOTE: If a name is prefixed with an exclamation then it will only
         // be removed here (and not also later when the tests are completed).
         if( next[ 0 ] == '!' )
            next.erase( 0, 1 );
         else
            g_all_kills.push_back( next );

         remove_file( next.c_str( ) );
      }
   }
}

void perform_test_step( const test_step& s, const string& test_name )
{
   ++g_num_test_steps_attempted;

   string command( "./" );

   string exec( s.exec );

   bool expecting_failure = false;

   if( !exec.empty( ) && ( exec[ 0 ] == '!' ) )
   {
      exec.erase( 0, 1 );
      expecting_failure = true;
   }

   bool is_async = false;
   if( !exec.empty( ) && ( exec[ 0 ] == '*' ) )
   {
      is_async = true;
      exec.erase( 0, 1 );
   }

   if( !exec.empty( ) && ( exec[ 0 ] == '$' ) )
   {
      command.erase( );
      exec.erase( 0, 1 );
   }

   command += exec;

   if( s.has_input )
   {
      command += " <";

      command += g_tests_directory + '/';
      command += test_name + '_' + s.name;

      command += ".cin";
   }

   string test_step_name;
   string temp_output_file_name;
   string test_output_file_name;

   command += " >";

   if( s.output != e_output_type_none )
   {
      test_step_name = test_output_file_name = temp_output_file_name = test_name + '_' + s.name;

      temp_output_file_name = g_tests_directory + '/' + temp_output_file_name;
      test_output_file_name = g_tests_directory + '/' + test_output_file_name;

      temp_output_file_name += ".new";
      test_output_file_name += ".tst";

      if( s.output == e_output_type_generate )
         command += temp_output_file_name;
      else
         command += c_null_device_name;
   }
   else
      command += c_null_device_name;

   if( is_async )
      command += " &";

   if( !s.exec.empty( ) )
   {
#ifdef DEBUG
      cout << "exec: " << command << endl;
#endif
      if( ( system( command.c_str( ) ) != 0 ) && !expecting_failure )
         throw runtime_error( "unexpected system failure" );
   }

   // NOTE: Wait for half a second to give
   // async programs some time to start up.
   if( is_async )
      msleep( 500 );

   if( !test_output_file_name.empty( ) )
   {
      if( _access( test_output_file_name.c_str( ), 0 ) == 0 )
      {
         if( files_differ( test_output_file_name.c_str( ), temp_output_file_name.c_str( ) ) )
         {
            ++g_num_test_steps_mismatched;

            cout << "  " << test_step_name << ": *** failed *** "
             << "diff " << test_output_file_name << ' ' << temp_output_file_name << endl;

            if( !g_is_quiet )
            {
               string str( "./diff -q " );

               str += test_output_file_name;
               str += ' ';
               str += temp_output_file_name;

               int rc = system( str.c_str( ) );
               ( void )rc;
            }
         }
         else
         {
            ++g_num_test_steps_succeeded;
            if( !g_is_quiet )
               cout << "  " << test_step_name << ": passed" << endl;

            remove_file( temp_output_file_name.c_str( ) );
         }
      }
      else
      {
         copy_file( temp_output_file_name.c_str( ), test_output_file_name.c_str( ) );

         ++g_num_test_steps_captured;

         if( !g_is_quiet )
            cout << "  " << test_step_name << ": (captured)" << endl;

         remove_file( temp_output_file_name.c_str( ) );
      }
   }
   else
      ++g_num_test_steps_succeeded; // NOTE: If no output to capture then simply assume that the test step has succeeded.
}

int main( int argc, char* argv[ ] )
{
   int first_arg = 1;

   if( argc < 2
    || string( argv[ 1 ] ) == "?" || string( argv[ 1 ] ) == "-?" || string( argv[ 1 ] ) == "/?" )
   {
      cout << "usage: test [-q] [-e={none|step|test|group}] [-f=<files_dir>] [-t=<tests_dir>] <tests_file.sio> [<group_1[:test[;step]]> [<group_2[:test[;step]]> [...]]]" << endl;

      return 0;
   }

   string test_set_file_name;

   vector< specific_test_spec > specific_tests;

   if( argc >= 2 )
   {
      string next_arg( argv[ first_arg ] );

      while( next_arg.size( ) > 1 && next_arg[ 0 ] == '-' )
      {
         if( next_arg == string( "-q" ) )
            g_is_quiet = true;
         else if( next_arg.substr( 0, 2 ) == string( "-e" ) )
         {
            next_arg.erase( 0, 2 );

            if( next_arg.size( ) > 1 && next_arg[ 0 ] == '=' );
               next_arg.erase( 0, 1 );

            if( next_arg == string( "none" ) )
               g_error_stop = e_error_stop_point_none;
            else if( next_arg == string( "step" ) )
               g_error_stop = e_error_stop_point_step;
            else if( next_arg == string( "test" ) )
               g_error_stop = e_error_stop_point_test;
            else if( next_arg == string( "group" ) )
               g_error_stop = e_error_stop_point_group;
            else
            {
               cerr << "error: unknown error stop point '" << next_arg << "'" << endl;
               return 1;
            }
         }
         else if( next_arg.substr( 0, 2 ) == string( "-f" ) )
         {
            next_arg.erase( 0, 2 );

            if( next_arg.size( ) > 1 && next_arg[ 0 ] == '=' );
               next_arg.erase( 0, 1 );

            g_files_directory = next_arg;
         }
         else if( next_arg.substr( 0, 2 ) == string( "-t" ) )
         {
            next_arg.erase( 0, 2 );

            if( next_arg.size( ) > 1 && next_arg[ 0 ] == '=' );
               next_arg.erase( 0, 1 );

            g_tests_directory = next_arg;
         }

         if( first_arg + 1 == argc )
            break;

         next_arg = string( argv[ ++first_arg ] );
      }

      test_set_file_name = next_arg;
   }

   set_environment_variable( c_files_directory_var_name, g_files_directory );
   set_environment_variable( c_tests_directory_var_name, g_tests_directory );

   while( ++first_arg < argc )
   {
      string next( argv[ first_arg ] );

      specific_test_spec specific_test;

      string::size_type pos = next.find( ':' );

      specific_test.group = next.substr( 0, pos );

      if( pos != string::npos )
      {
         next.erase( 0, pos + 1 );

         pos = next.find( ';' );

         specific_test.tests = next.substr( 0, pos );

         if( pos != string::npos )
         {
            next.erase( 0, pos + 1 );
            specific_test.steps = next;
         }
      }

      specific_tests.push_back( specific_test );
   }

   try
   {
      // NOTE: The test set file can be either
      // in the current directory or (when not
      // in current) in the "tests" directory.
      if( !file_exists( test_set_file_name ) )
      {
         if( !g_tests_directory.empty( )
          && file_exists( g_tests_directory + '/' + test_set_file_name ) )
            test_set_file_name = g_tests_directory + '/' + test_set_file_name;
      }

      test_sio_reader_writer( test_set_file_name );

      string::size_type pos = test_set_file_name.find( '.' );

      string init_script( "init_" + test_set_file_name.substr( 0, pos ) );

      if( file_exists( init_script ) )
      {
         init_script = "./" + init_script;

         int rc = system( init_script.c_str( ) );
         ( void )rc;
      }

      ifstream inpf( test_set_file_name.c_str( ) );

      if( !inpf )
      {
         cerr << "error: unable to open file '" << test_set_file_name << "' for input" << endl;
         return 1;
      }

      set< string > step_names;
      set< string > test_names;
      set< string > group_names;

      sio_reader reader( inpf );

      reader.start_section( c_section_groups );

      while( reader.has_started_section( c_section_group ) )
      {
         group g;

         g.name = reader.read_attribute( c_group_name_value );

         if( group_names.count( g.name ) )
            throw runtime_error( "repeated group name '"
             + g.name + "' found in line #" + to_string( reader.get_last_line_num( ) ) );

         group_names.insert( g.name );

         reader.start_section( c_section_tests );

         while( reader.has_started_section( c_section_test ) )
         {
            test t;
            string s;

            t.name = reader.read_attribute( c_test_name_value );

            if( test_names.count( g.name + '.' + t.name ) )
               throw runtime_error( "repeated test name '"
                + t.name + "' found in line #" + to_string( reader.get_last_line_num( ) ) );

            test_names.insert( g.name + '.' + t.name );

            t.description = reader.read_attribute( c_test_description_value );

            while( reader.has_read_attribute( c_test_step_kill_value, s ) )
            {
               string findstr( 1, c_env_var_prefix );

               findstr += c_files_directory_var_name;

               replace( s, findstr, g_files_directory );

               t.kills.push_back( s );
            }

            while( reader.has_started_section( c_section_test_step ) )
            {
               test_step ts;

               ts.name = reader.read_attribute( c_test_step_name_value );

               if( step_names.count( g.name + '.' + t.name + '.' + ts.name ) )
                  throw runtime_error( "repeated test step name '"
                   + ts.name + "' found in line #" + to_string( reader.get_last_line_num( ) ) );

               step_names.insert( g.name + '.' + t.name + '.' + ts.name );

               ts.exec = reader.read_attribute( c_test_step_exec_value );

               s = reader.read_attribute( c_test_step_input_value );

               if( s == c_true )
                  ts.has_input = true;
               else if( s == c_false )
                  ts.has_input = false;
               else
                  throw runtime_error( "invalid boolean value '" + s + "' (expected 'true' or 'false')" );

               s = reader.read_attribute( c_test_step_output_value );

               if( s == c_output_type_none )
                  ts.output = e_output_type_none;
               else if( s == c_output_type_generate )
                  ts.output = e_output_type_generate;
               else if( s == c_output_type_automatic )
                  ts.output = e_output_type_automatic;
               else
                  throw runtime_error( "invalid value '" + s
                   + "' for output (expected 'none', 'generated' or 'automatic')" );

               t.test_steps.push_back( ts );

               reader.finish_section( c_section_test_step );
            }

            g.tests.push_back( t );

            reader.finish_section( c_section_test );
         }

         reader.finish_section( c_section_tests );

         g_groups.push_back( g );

         reader.finish_section( c_section_group );
      }

      reader.finish_section( c_section_groups );

      reader.verify_finished_sections( );

      if( specific_tests.empty( ) )
      {
         for( group_size_type g = 0; g < g_groups.size( ); g++ )
         {
            string group_name;

            init_group( g_groups[ g ], group_name );

            bool had_init = false;

            size_t num_tests = g_groups[ g ].tests.size( );

            for( vector< test >::size_type t = 0; t < num_tests; t++ )
            {
               string test_name( group_name );

               init_group_test( g_groups[ g ].tests[ t ], test_name );

               size_t num_steps = g_groups[ g ].tests[ t ].test_steps.size( );

               for( vector< test_step >::size_type s = 0; s < num_steps; s++ )
               {
                  perform_test_step( g_groups[ g ].tests[ t ].test_steps[ s ], test_name );

                  if( g_error_stop == e_error_stop_point_step && g_num_test_steps_mismatched )
                     break;

                  // NOTE: If the very first step in the first group test is named as an init
                  // step then remember that this has been executed so a matching term in the
                  // very last step of the last group test will be executed on an error exit.
                  if( ( t == 0 ) && ( s == 0 )
                   && ( g_groups[ g ].tests[ t ].test_steps[ s ].name == string( c_special_test_step_name_init ) ) )
                     had_init = true;
               }

               if( ( g_error_stop == e_error_stop_point_step ) && g_num_test_steps_mismatched )
                  break;

               if( ( g_error_stop == e_error_stop_point_test ) && g_num_test_steps_mismatched )
                  break;
            }

            size_t final_steps = g_groups[ g ].tests[ num_tests - 1 ].test_steps.size( );

            string final_step_name = g_groups[ g ].tests[ num_tests - 1 ].test_steps[ final_steps - 1 ].name;

            if( ( g_error_stop == e_error_stop_point_step && g_num_test_steps_mismatched )
             || ( g_error_stop == e_error_stop_point_test && g_num_test_steps_mismatched ) )
            {
               // NOTE: If an init step had been executed in the first group test then if
               // the final step of the final group test is a term step then execute it.
               if( had_init && ( final_step_name == string( c_special_test_step_name_term ) ) )
                  perform_test_step( g_groups[ g ].tests[ num_tests - 1 ].test_steps[ final_steps - 1 ], group_name );

               return 1;
            }

            if( ( g_error_stop == e_error_stop_point_group ) && g_num_test_steps_mismatched )
               return 1;
         }
      }
      else
      {
         for( vector< specific_test_spec >::size_type i = 0; i < specific_tests.size( ); i++ )
         {
            size_t gi = group_index( specific_tests[ i ].group );

            if( gi == 0 )
            {
               cerr << "error: unknown group name '" << specific_tests[ i ].group << "'" << endl;

               return 1;
            }

            size_t g = gi - 1;

            string group_name;

            init_group( g_groups[ g ], group_name );

            string tests( specific_tests[ i ].tests );

            if( tests.empty( ) )
            {
               for( vector< test >::size_type t = 0; t < g_groups[ g ].tests.size( ); t++ )
               {
                  string test_name( group_name );

                  init_group_test( g_groups[ g ].tests[ t ], test_name );

                  for( vector< test_step >::size_type s = 0; s < g_groups[ g ].tests[ t ].test_steps.size( ); s++ )
                  {
                     perform_test_step( g_groups[ g ].tests[ t ].test_steps[ s ], test_name );

                     if( ( g_error_stop == e_error_stop_point_step ) && g_num_test_steps_mismatched )
                        return 1;
                  }

                  if( ( g_error_stop == e_error_stop_point_test ) && g_num_test_steps_mismatched )
                     return 1;
               }
            }
            else
            {
               string::size_type tpos = tests.find( ',' );

               while( true )
               {
                  string next_test( tests.substr( 0, tpos ) );

                  string test_name( group_name );

                  size_t ti = group_test_index( g_groups[ g ], next_test );

                  if( ti == 0 )
                  {
                     cerr << "error: unknown test name '"
                      << next_test << "' for group '" << group_name << "'" << endl;

                     return 1;
                  }

                  size_t t = ti - 1;

                  init_group_test( g_groups[ g ].tests[ t ], test_name );

                  string steps( specific_tests[ i ].steps );

                  if( steps.empty( ) )
                  {
                     for( vector< test_step >::size_type s = 0; s < g_groups[ g ].tests[ t ].test_steps.size( ); s++ )
                     {
                        perform_test_step( g_groups[ g ].tests[ t ].test_steps[ s ], test_name );

                        if( ( g_error_stop == e_error_stop_point_step ) && g_num_test_steps_mismatched )
                           return 1;
                     }
                  }
                  else
                  {
                     string::size_type spos = steps.find( ',' );

                     while( true )
                     {
                        string next_step( steps.substr( 0, spos ) );

                        size_t si = group_test_step_index( g_groups[ g ].tests[ t ], next_step );

                        if( si == 0 )
                        {
                           cerr << "error: unknown test step '" << next_step
                            << "' for test '" << test_name << "' in group '" << group_name << "'" << endl;

                           return 1;
                        }

                        size_t s = si - 1;

                        perform_test_step( g_groups[ g ].tests[ t ].test_steps[ s ], test_name );

                        if( ( g_error_stop == e_error_stop_point_step ) && g_num_test_steps_mismatched )
                           return 1;

                        if( spos == string::npos )
                           break;

                        steps.erase( 0, spos + 1 );

                        spos = steps.find( ',' );
                     }
                  }

                  if( ( g_error_stop == e_error_stop_point_test ) && g_num_test_steps_mismatched )
                     return 1;

                  if( tpos == string::npos )
                     break;

                  tests.erase( 0, tpos + 1 );

                  tpos = tests.find( ',' );
               }
            }

            if( ( g_error_stop == e_error_stop_point_group ) && g_num_test_steps_mismatched )
               return 1;
         }
      }

      if( !g_is_quiet )
         cout << endl;
         
      if( ( g_num_test_steps_succeeded + g_num_test_steps_captured ) == g_num_test_steps_attempted )
      {
         cout << "summary: passed (" << g_num_test_steps_succeeded;

         if( g_num_test_steps_succeeded == 1 )
            cout << " test step succeeded";
         else
            cout << " test steps succeeded";

         if( g_num_test_steps_captured )
         {
            cout << " and " << g_num_test_steps_captured;

            if( g_num_test_steps_captured == 1 )
               cout << " test was captured";
            else
               cout << " tests were captured";
         }

         cout << ')' << endl;

         // NOTE: If tests completed successfully then repeat kills that were performed during testing
         // that had not been specifically prefixed not to (i.e. clean up).
         for( vector< string >::size_type i = 0; i < g_all_kills.size( ); i++ )
            remove_file( g_all_kills[ i ].c_str( ) );
      }
      else
      {
         cerr << "summary: *** failed *** (" << g_num_test_steps_attempted;

         if( g_num_test_steps_attempted == 1 )
            cerr << " was attempted";
         else
            cerr << " were attempted";

         if( g_num_test_steps_captured )
         {
            cerr << " and " << g_num_test_steps_captured;

            if( g_num_test_steps_captured == 1 )
               cerr << " test was captured";
            else
               cerr << " tests were captured";
         }

         if( g_num_test_steps_mismatched )
         {
            cerr << " with " << g_num_test_steps_mismatched;

            if( g_num_test_steps_mismatched == 1 )
               cerr << " failure";
            else
               cerr << " failures";
         }

         size_t num_accounted = g_num_test_steps_succeeded + g_num_test_steps_captured + g_num_test_steps_mismatched;

         if( num_accounted != g_num_test_steps_attempted )
            cerr << " and " << ( g_num_test_steps_attempted - num_accounted ) << " unaccounted for";

         cerr << ')' << endl;
      }
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;

      return 1;
   }
}
