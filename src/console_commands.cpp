// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2013 CIYAM Open Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef __BORLANDC__
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstring>
#  include <memory>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#  ifdef __GNUG__
#     include <unistd.h>
#     ifdef RDLINE_SUPPORT
extern "C"
{
#        include <readline/history.h>
}
#     endif
#  endif
#endif

#include "console_commands.h"

#include "macros.h"
#include "console.h"
#include "pointers.h"
#include "utilities.h"

using namespace std;

namespace
{

const int c_max_args = 10;
const int c_max_usage_width = 84;

const int c_max_history = 1000;

const char c_startup_prefix = '-';

const char* const c_startup_alt_help = "/?";

const char c_read_input_prefix = '<';
const char c_write_history_prefix = '>';
const char c_quiet_command_prefix = '.';
const char c_output_command_usage = '?';
const char c_system_command_prefix = '~';
const char c_history_command_prefix = '!';
const char c_comment_command_prefix = ';';
const char c_envcond_command_prefix = '@';
const char c_message_command_prefix = '#';
const char c_environment_variable_marker = '%';
const char c_pause_message_command_prefix = '^';

const char* const c_non_command_prefix = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";

const char* const c_cmd_echo = "echo";
const char* const c_cmd_quiet = "quiet";
const char* const c_cmd_no_pause = "no_pause";
const char* const c_cmd_no_prompt = "no_prompt";

command_definition startup_command_definitions[ ] =
{
   { c_cmd_echo, "", "switch on local echoing of input" },
   { c_cmd_quiet, "", "switch on quiet operating mode" },
   { c_cmd_no_pause, "", "switch off support for pausing" },
   { c_cmd_no_prompt, "", "switch off console command prompt" }
};

const char* const c_command_prompt = "\n> ";
const char* const c_message_press_any_key = "press any key...";

string replace_input_arg_values( const vector< string >& args, const string& input )
{
   string str( input );

   string::size_type pos = str.find( c_environment_variable_marker );
   while( pos != string::npos )
   {
      if( str.size( ) > pos + 1 )
      {
         if( str[ pos + 1 ] == c_environment_variable_marker )
            ++pos;
         else if( str[ pos + 1 ] >= '0' && str[ pos + 1 ] <= '9' )
         {
            int argnum = str[ pos + 1 ] - '0';
            string argval( args[ argnum ] );

            str.erase( pos, 2 );
            str.insert( pos, argval );

            // NOTE: If the argument value is an empty string and the next character
            // is a space then erase this space to ensure the correct command syntax.
            if( argval.empty( ) && str[ pos ] == ' ' )
               str.erase( pos, 1 );
         }
         else if( str[ pos + 1 ] == '*' )
         {
            string all;
            for( int i = 0; i < 10; i++ )
            {
               string argval( args[ i ] );

               if( !argval.empty( ) )
               {
                  if( !all.empty( ) )
                     all += " ";
                  all += argval;
               }
            }

            str.erase( pos, 2 );
            str.insert( pos, all );

            if( all.empty( ) && str[ pos ] == ' ' ) // see above NOTE
               str.erase( pos, 1 );
         }
      }

      pos = str.find( c_environment_variable_marker, pos + 1 );
   }

   return str;
}

class startup_command_functor : public command_functor
{
   public:
   startup_command_functor( command_handler& handler )
    : command_functor( handler )
   {
   }

   void operator ( )( const std::string& command, const parameter_info& parameters );
};

void startup_command_functor::operator ( )( const string& command, const parameter_info& )
{
   if( command == c_cmd_echo )
      handler.add_option( c_cmd_echo );
   else if( command == c_cmd_quiet )
      handler.add_option( c_cmd_quiet );
   else if( command == c_cmd_no_pause )
      handler.add_option( c_cmd_no_pause );
   else if( command == c_cmd_no_prompt )
      handler.add_option( c_cmd_no_prompt );
}

command_functor* startup_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new startup_command_functor( handler );
}

}

console_command_handler::console_command_handler( )
 :
 line_number( 0 ),
 description_offset( 0 ),
 num_custom_startup_options( 0 ),
 is_executing_commands( false ),
 allow_history_addition( true ),
 handling_startup_options( false )
{
#ifdef __GNUG__
   if( isatty( STDIN_FILENO ) )
      using_history( );
#endif
   add_option( c_prompt_prefix_option );

   for( size_t i = 0; i < c_max_args; i++ )
      args.push_back( string( ) );
}

bool console_command_handler::has_option_quiet( ) const
{
   return has_option( c_cmd_quiet );
}

void console_command_handler::perform_after_command_changes( )
{
   string::size_type max_size = 0;
   size_t num = get_num_commands( );
   for( size_t i = 0; i < num; i++ )
   {
      string name( get_command_name( i ) );
      string::size_type size( name.size( ) );

      string usage( get_usage_for_command( name ) );
      if( !usage.empty( ) )
         size += usage.size( ) + 1;

      if( size <= c_max_usage_width && size > max_size )
         max_size = size;
   }

   description_offset = max_size + 1;
}

string console_command_handler::format_usage_output(
 const string& command, const string& usage, const string& description ) const
{
   string retval( command );

   if( !usage.empty( ) )
   {
      retval += ' ';
      retval += usage;
   }

   size_t start( retval.length( ) );

   if( retval.size( ) > c_max_usage_width )
   {
      start = 0;
      retval += '\n';
   }

   if( !description.empty( ) )
   {
      if( start < description_offset && description_offset - start )
         retval += string( description_offset - start, ' ' );

      retval += '(';
      retval += description;
      retval += ')';
   }

   return retval;
}

string console_command_handler::preprocess_command_and_args( const string& cmd_and_args )
{
   string str( cmd_and_args );

   set_quiet_command( false );

   remove_trailing_cr_from_text_file_line( str );

   if( handling_startup_options )
   {
      if( !str.empty( ) )
      {
         if( str[ 0 ] == c_startup_prefix )
            str.erase( 0, 1 );

         if( !str.empty( ) )
         {
            if( str[ 0 ] == c_output_command_usage || str == string( c_startup_alt_help ) )
            {
               if( get_command_processor( ) )
                  get_command_processor( )->output_command_usage( "" );

               str.erase( );
            }
         }
      }
   }
   else
   {
      if( has_option( c_cmd_echo ) )
      {
         if( str.empty( ) )
            cout << endl;
         else if( strchr( c_non_command_prefix, str[ 0 ] ) != 0 )
            cout << str << endl;
      }

#ifdef __GNUG__
#  ifdef RDLINE_SUPPORT
      if( isatty( STDIN_FILENO ) && !is_executing_commands )
         add_history( str.c_str( ) );
#  endif
#endif

      string str_for_history( str );

      string error_context;
      if( !script_file.empty( ) )
         error_context = " processing script '" + script_file + "' at line #" + to_string( line_number );

      if( !conditions.empty( ) )
      {
         if( !str.empty( ) && str[ 0 ] != c_envcond_command_prefix )
         {
            if( !conditions.back( ) || !dummy_conditions.empty( ) )
               str.erase( );
         }
      }

      if( !str.empty( ) )
      {
         str = replace_input_arg_values( args, str );
         str = replace_environment_variables( str.c_str( ), c_environment_variable_marker );

         bool add_to_history = allow_history_addition;

         if( str[ 0 ] == c_quiet_command_prefix )
         {
            str.erase( 0, 1 );
            set_quiet_command( true );
         }
         else if( str[ 0 ] == c_read_input_prefix )
         {
            size_t file_name_offset = 1;
            auto_ptr< restorable< bool > > ap_tmp_permit_history_addition;

            // NOTE: If the read input prefix is repeated then each command read from the input file will
            // be added to the history rather than adding the read input line itself as the history item.
            if( str.size( ) > 1 && str[ 1 ] == c_read_input_prefix )
            {
               ++file_name_offset;
               add_to_history = false;
            }
            else
               ap_tmp_permit_history_addition.reset( new restorable< bool >( allow_history_addition, false ) );

            vector< string > new_args;
            size_t num_new_args = setup_arguments( str.c_str( ) + 1, new_args );

            if( num_new_args == 0 )
               throw runtime_error( "unexpected missing file name for input" );

            if( num_new_args > c_max_args )
               throw runtime_error( "unexpected max. number of input args exceeded" );

            for( vector< string >::size_type i = new_args.size( ); i < args.size( ); i++ )
               new_args.push_back( string( ) );

            ifstream inpf( new_args[ 0 ].c_str( ) );
            if( !inpf )
               throw runtime_error( "unable to open file '" + new_args[ 0 ] + "' for input" );

            vector< bool > dummy_vector;

            // NOTE: As one script may call another all relevant state information must be
            // captured before and then restored after processing each script.
            restorable< size_t > tmp_line_number( line_number, 0 );
            restorable< vector< string > > tmp_args( args, new_args );
            restorable< string > tmp_script_file( script_file, new_args[ 0 ] );
            restorable< vector< bool > > tmp_completed( completed, dummy_vector );
            restorable< vector< bool > > tmp_conditions( conditions, dummy_vector );
            restorable< bool > tmp_executing_commands( is_executing_commands, true );
            restorable< vector< bool > > tmp_dummy_conditions( dummy_conditions, dummy_vector );

            string next;
            string next_command;
            bool is_first = true;
            while( getline( inpf, next ) )
            {
               ++line_number;
               bool is_continuation = false;

               if( next.empty( ) )
                  continue;

               remove_trailing_cr_from_text_file_line( next, is_first );

               if( is_first )
                  is_first = false;

               // NOTE: A trailing backslash (that is not escaped) is interpreted as a line continuation.
               string::size_type pos = next.find_last_not_of( '\\' );
               if( pos != next.size( ) - 1 && ( next.size( ) - pos - 1 ) % 2 )
               {
                  next.erase( next.size( ) - 1 );
                  is_continuation = true;
               }

               next_command += next;

               if( !is_continuation )
               {
                  execute_command( next_command );
                  next_command.erase( );
               }
            }

            if( !conditions.empty( ) )
               throw runtime_error( "missing 'endif' for conditional in '" + new_args[ 0 ] + "'" + error_context );

            if( !inpf.eof( ) )
               throw runtime_error( "unexpected error occurred whilst reading '" + new_args[ 0 ] + "'" + error_context );

            str.erase( );
         }
         else if( str[ 0 ] == c_write_history_prefix )
         {
            add_to_history = false;
            size_t file_name_offset = 1;
            ios::openmode output_flags = ios::out;

            // NOTE: If the write history prefix is repeated then open the output file for append.
            if( str.size( ) > 1 && str[ 1 ] == c_write_history_prefix )
            {
               ++file_name_offset;
               output_flags = ios::app;
            }

            ofstream outf( str.c_str( ) + file_name_offset, output_flags );
            if( !outf )
               throw runtime_error( "unable to open file '" + str.substr( file_name_offset ) + "' for output" );

            if( !command_history.empty( ) )
               for( deque< string >::size_type i = 0; i < command_history.size( ); i++ )
                  outf << command_history[ i ] << '\n';

            str.erase( );
         }
         else if( str[ 0 ] == c_output_command_usage )
         {
            string wildcard_match_expr;

            string::size_type pos = str.find( ' ' );
            if( pos != string::npos )
               wildcard_match_expr = str.substr( pos + 1 );

            if( get_command_processor( ) )
               get_command_processor( )->output_command_usage( wildcard_match_expr );

            str.erase( );
         }
         else if( str[ 0 ] == c_system_command_prefix )
         {
            system( str.c_str( ) + 1 );

            str.erase( );
         }
         else if( str[ 0 ] == c_history_command_prefix )
         {
            add_to_history = false;

            if( command_history.size( ) )
            {
               if( str.size( ) > 1 )
               {
                  size_t offset = 1;
                  bool remove = false;

                  if( str.size( ) > 2 && str[ 1 ] == '-' )
                  {
                     offset = 2;
                     remove = true;
                  }

                  int n;
                  if( str.size( ) == 2 && str[ 1 ] == c_history_command_prefix )
                     n = ( int )command_history.size( );
                  else
                     n = atoi( str.substr( offset ).c_str( ) );

                  if( n < 0 || n > ( int )( command_history.size( ) ) )
                     throw runtime_error( "command #" + str.substr( 1 ) + " is invalid" );

                  if( !remove )
                  {
                     restorable< bool > tmp_executing_commands( is_executing_commands, true );

                     if( n > 0 )
                     {
                        execute_command( command_history[ n - 1 ] );
                        command_history.pop_back( );
                     }
                     else
                     {
                        for( vector< string >::size_type i = 0; i < command_history.size( ); i++ )
                        {
                           execute_command( command_history[ i ] );
                           command_history.pop_back( );
                        }
                     }
                  }
                  else
                  {
                     if( n == 0 )
                        command_history.clear( );
                     else
                        command_history.erase( command_history.begin( ) + ( n - 1 ) );
                  }
               }
               else
               {
                  for( vector< string >::size_type i = 0; i < command_history.size( ); i++ )
                     cout << ( i + 1 ) << '\t' << command_history[ i ] << '\n';
               }
            }
            else
               cout << "(no history is available)\n";

            str.erase( );
         }
         else if( str[ 0 ] == c_comment_command_prefix )
            str.erase( );
         else if( str[ 0 ] == c_envcond_command_prefix )
         {
            size_t pos = 0;
            for( size_t i = 1; i < str.length( ); i++ )
            {
               if( str[ i ] != ' ' )
               {
                  pos = i;
                  break;
               }
            }

            if( pos == 0 )
               throw runtime_error( "invalid conditional expression '" + str + "'" + error_context );

            string expression( str.substr( pos ) );
            if( !expression.empty( ) )
            {
               string::size_type pos = expression.find( ' ' );

               string token = expression.substr( 0, pos );

               string symbol;
               if( pos != string::npos )
                  symbol = expression.substr( pos + 1 );

               if( token == "ifdef" )
               {
                  if( !conditions.empty( ) && !conditions.back( ) )
                     dummy_conditions.push_back( 0 );
                  else
                  {
                     completed.push_back( false );
                     conditions.push_back( !symbol.empty( ) );
                  }
               }
               else if( token == "ifndef" )
               {
                  if( !conditions.empty( ) && !conditions.back( ) )
                     dummy_conditions.push_back( 0 );
                  else
                  {
                     completed.push_back( false );
                     conditions.push_back( symbol.empty( ) );
                  }
               }
               else if( token == "else" )
               {
                  if( !symbol.empty( ) )
                     throw runtime_error( "invalid 'else' expression" + error_context );

                  if( dummy_conditions.empty( ) )
                  {
                     if( conditions.empty( ) || completed.back( ) )
                        throw runtime_error( "no matching 'ifdef' found for 'else' expression" + error_context );

                     bool val = conditions.back( );

                     completed.back( ) = true;

                     conditions.pop_back( );
                     conditions.push_back( !val );
                  }
               }
               else if( token == "endif" )
               {
                  bool pop_cond = true;
                  if( !dummy_conditions.empty( ) )
                  {
                     if( !dummy_conditions.back( ) )
                        pop_cond = false;
                     dummy_conditions.pop_back( );
                  }

                  if( pop_cond )
                  {
                     if( conditions.empty( ) )
                        throw runtime_error( "no matching 'if' found for 'endif' expression" + error_context );

                     completed.pop_back( );
                     conditions.pop_back( );
                  }
               }
               else
                  throw runtime_error( "invalid conditional expression '" + str + "'" + error_context );
            }
            str.erase( );
         }
         else if( str[ 0 ] == c_message_command_prefix )
         {
            cout << str.substr( 1 ) << '\n';
            str.erase( );
         }
         else if( str[ 0 ] == c_pause_message_command_prefix )
         {
            if( !has_option( c_cmd_no_pause ) )
            {
               string msg( c_message_press_any_key );
               if( str.length( ) > 1 )
                  msg = str.substr( 1 );

               str.erase( );
               get_char( msg.c_str( ) );
               cout << '\r' << string( msg.length( )
                + prompt_prefix.length( ) + strlen( c_command_prompt ), ' ' ) << '\r';
            }
            else
               str.erase( );
         }

         if( add_to_history )
         {
            command_history.push_back( str_for_history );

            if( command_history.size( ) > c_max_history )
               command_history.pop_front( );
         }
      }
   }

   return str;
}

void console_command_handler::handle_unknown_command( const string& command )
{
   if( handling_startup_options )
      throw runtime_error( "unknown option '" + command + "'" );

   cout << "unknown command '" << command << "'" << endl;
}

void console_command_handler::handle_invalid_command( const command_parser&, const string& cmd_and_args )
{
   cout << "invalid command usage '" << cmd_and_args << "'" << endl;
}

void console_command_handler::handle_command_response( const string& response, bool is_special )
{
   if( !is_special )
      cout << response << '\n';
   else
      cerr << response << endl;
}

void console_command_handler::handle_extraneous_custom_option( const string& option )
{
   throw runtime_error( "unexpected argument '" + option + "'" );
}

bool console_command_processor::is_still_processing( )
{
   return cin;
}

string console_command_processor::get_cmd_and_args( )
{
   string prompt;

   if( !get_is_continuation( ) && !handler.has_option( c_cmd_no_prompt ) )
   {
      prompt = string( c_command_prompt );
      string prefix( handler.get_option_value( c_prompt_prefix_option ) );
      if( !prefix.empty( ) )
         prompt.insert( 1, prefix );
   }

   return get_line( prompt.c_str( ) );
}

void console_command_processor::output_command_usage( const string& wildcard_match_expr ) const
{
   cout << '\n';
   cout << "commands:\n";
   cout << "=========\n";

   cout << get_usage_for_commands( wildcard_match_expr );
}

startup_command_processor::startup_command_processor( console_command_handler& handler,
 fp_get_app_info_string get_app_info_string_func, int argc, char* argv[ ] )
 : command_processor( handler ),
 handler( handler ),
 get_app_info_string_func( get_app_info_string_func )
{
   int i;
   for( i = 1; i < argc; i++ )
   {
      if( argv[ i ][ 0 ] == c_startup_prefix
       || string( argv[ i ] ) == string( c_startup_alt_help )
       || string( argv[ i ] ) == string( 1, c_output_command_usage ) )
         args.push_back( argv[ i ] );
      else
         break;
   }

   while( i < argc )
      extra_args.push_back( argv[ i++ ] );

   handler.set_handling_startup_options( true );

   using_standard_console_options = false;
}

startup_command_processor::startup_command_processor( console_command_handler& handler,
 fp_get_app_info_string get_app_info_string_func, size_t group_num, int argc, char* argv[ ] )
 : command_processor( handler ),
 handler( handler ),
 get_app_info_string_func( get_app_info_string_func )
{
   int i;
   for( i = 1; i < argc; i++ )
   {
      if( argv[ i ][ 0 ] == c_startup_prefix
       || string( argv[ i ] ) == string( c_startup_alt_help )
       || string( argv[ i ] ) == string( 1, c_output_command_usage ) )
         args.push_back( argv[ i ] );
      else
         break;
   }

   while( i < argc )
      extra_args.push_back( argv[ i++ ] );

   handler.set_handling_startup_options( true );

   handler.add_commands( group_num,
    startup_command_functor_factory, ARRAY_PTR_AND_SIZE( startup_command_definitions ) );

   using_standard_console_options = true;
}

startup_command_processor::~startup_command_processor( )
{
   if( using_standard_console_options )
      handler.remove_commands( ARRAY_PTR_AND_SIZE( startup_command_definitions ) );

   handler.set_handling_startup_options( false );
}

string startup_command_processor::get_cmd_and_args( )
{
   string next( args.back( ) );

   size_t pos = next.find( '=' );
   if( pos != string::npos )
      next[ pos ] = ' ';

   args.pop_back( );
   return next;
}

void startup_command_processor::output_command_usage( const string& wildcard_match_expr ) const
{
   cout << ( *get_app_info_string_func )( e_app_info_request_title_and_version ) << endl;

   cout << "usage: "
    << ( *get_app_info_string_func )( e_app_info_request_title ) << " [" << c_startup_prefix << "<options>]";

   if( !handler.get_custom_startup_options_usage( ).empty( ) )
      cout << ' ' << handler.get_custom_startup_options_usage( );

   cout << "\n\nwhere <options> can be one or more of the following:\n\n";

   cout << get_usage_for_commands( wildcard_match_expr, '=' );

   exit( 0 );
}

void startup_command_processor::finalise_command_processing( )
{
   size_t num( min( extra_args.size( ), handler.get_num_custom_startup_options( ) ) );

   for( size_t i = 0; i < num; i++ )
      handler.process_custom_startup_option( i, extra_args[ i ] );

   for( size_t i = num; i < extra_args.size( ); i++ )
      handler.handle_extraneous_custom_option( extra_args[ i ] );
}

