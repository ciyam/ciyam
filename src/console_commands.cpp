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
#  include <ctype.h>
#  include <cstring>
#  include <memory>
#  include <fstream>
#  include <sstream>
#  include <iostream>
#  include <stdexcept>
#  ifdef __GNUG__
#     include <unistd.h>
#  endif
#endif

#include "console_commands.h"

#include "config.h"
#include "format.h"
#include "macros.h"
#include "sha256.h"
#include "console.h"
#include "pointers.h"
#include "date_time.h"
#include "utilities.h"
#ifdef SSL_SUPPORT
#  include "crypto_keys.h"
#endif
#include "fs_iterator.h"

#ifdef __GNUG__
#  ifdef RDLINE_SUPPORT
extern "C"
{
#     include <readline/history.h>
#     include <readline/readline.h>
}
#  endif
#endif

using namespace std;

namespace
{

const int c_max_args = 10;
const int c_max_history = 1000;
const int c_max_usage_width = 84;

const int c_bad_system_exit = 999;

const size_t c_max_pwd_size = 128;

const char c_startup_prefix = '-';

const char* const c_help_command = "help";
const char* const c_startup_alt_help = "-help";

const char* const c_retain_all = "*";

const char* const c_env_var_error = "ERROR";
const char* const c_env_var_output = "OUTPUT";

const char* const c_env_var_ciyam_fissile = "CIYAM_FISSILE";

const char* const c_default_value_prompt = "VALUE=";

const char c_read_input_prefix = '<';
const char c_write_output_prefix = '>';
const char c_quiet_command_prefix = '.';
const char c_output_command_usage = '?';
const char c_forget_command_prefix = '-';
const char c_ignore_command_prefix = '+';
const char c_prompted_input_prefix = '&';
const char c_system_command_prefix = '~';
const char c_history_command_prefix = '!';
const char c_comment_command_prefix = ';';
const char c_envcond_command_prefix = '@';
const char c_message_command_prefix = '#';
const char c_environment_variable_assign = '=';
const char c_pause_message_command_prefix = '^';
const char c_environment_variable_marker_1 = '$';
const char c_environment_variable_marker_2 = '%';

const char* const c_env_var_system_retval = "SYSTEM_RETVAL";
const char* const c_env_var_progress_prefix = "PROGRESS_PREFIX";

const char* const c_non_command_prefix = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";

const char* const c_unix_time = "unix";

const char* const c_function_file = "file";
const char* const c_function_files = "files";
const char* const c_function_lower = "lower";
const char* const c_function_paths = "paths";
const char* const c_function_upper = "upper";
const char* const c_function_aschex = "aschex";
const char* const c_function_dechex = "dechex";
const char* const c_function_hexasc = "hexasc";
const char* const c_function_hexbig = "hexbig";
const char* const c_function_hexdec = "hexdec";
const char* const c_function_hexlit = "hexlit";
const char* const c_function_padlen = "padlen";
const char* const c_function_repstr = "repstr";
const char* const c_function_sha256 = "sha256";
const char* const c_function_substr = "substr";
const char* const c_function_fullpath = "fullpath";
const char* const c_function_password = "password";

const char* const c_envcond_command_else = "else";
const char* const c_envcond_command_ifeq = "ifeq";
const char* const c_envcond_command_skip = "skip";
const char* const c_envcond_command_depth = "depth";
const char* const c_envcond_command_endif = "endif";
const char* const c_envcond_command_ifdef = "ifdef";
const char* const c_envcond_command_ifneq = "ifneq";
const char* const c_envcond_command_label = "label";
const char* const c_envcond_command_ifndef = "ifndef";
const char* const c_envcond_command_retain = "retain";

const char* const c_cmd_echo = "echo";
const char* const c_cmd_quiet = "quiet";
const char* const c_cmd_monitor = "monitor";
const char* const c_cmd_no_pause = "no_pause";
const char* const c_cmd_no_prompt = "no_prompt";
const char* const c_cmd_no_stderr = "no_stderr";
const char* const c_cmd_no_progress = "no_progress";

command_definition startup_command_definitions[ ] =
{
   { c_cmd_echo, "", "switch on local echo of input" },
   { c_cmd_quiet, "", "switch on quiet operating mode" },
   { c_cmd_monitor, "", "switch on progress monitoring" },
   { c_cmd_no_pause, "", "switch off support for pausing" },
   { c_cmd_no_prompt, "", "switch off console command prompt" },
   { c_cmd_no_stderr, "", "switch off outputting errors to stderr" },
   { c_cmd_no_progress, "", "switch off outputting progress messages" }
};

const char* const c_command_prompt = "\n> ";
const char* const c_message_press_any_key = "(press any key to continue)...";

void system_command( const char* p_cmd )
{
   int rc = system( p_cmd );

   if( WIFEXITED( rc ) )
      rc = WEXITSTATUS( rc );
   else
      rc = c_bad_system_exit;

   set_environment_variable( c_env_var_system_retval, to_string( rc ) );
}

void split_all_extras( const string& all_extras, vector< string >& includes, vector< string >& excludes )
{
   if( !all_extras.empty( ) )
   {
      vector< string > extras;
      split( all_extras, extras );

      for( size_t i = 0; i < extras.size( ); i++ )
      {
         string next( extras[ i ] );

         if( !next.empty( ) )
         {
            // NOTE: An include can begin with "\^" to
            // not be confused as being an exclude.
            if( next[ 0 ] != '^' )
               includes.push_back( unescaped( next ) );
            else
               excludes.push_back( next.substr( 1 ) );
         }
      }
   }
}

bool should_be_included( const string& name, const vector< string >& includes, const vector< string >& excludes )
{
   bool found = false;

   if( !excludes.empty( ) )
   {
      for( size_t i = 0; i < excludes.size( ); i++ )
      {
         string next( excludes[ i ] );

         if( next == name )
         {
            found = true;
            break;
         }
         else if( wildcard_match( next, name ) )
         {
            found = true;
            break;
         }
      }
   }

   if( found )
      return false;

   if( includes.empty( ) )
      found = true;
   else
   {
      for( size_t i = 0; i < includes.size( ); i++ )
      {
         string next( includes[ i ] );

         if( next == name )
         {
            found = true;
            break;
         }
         else if( wildcard_match( next, name ) )
         {
            found = true;
            break;
         }
      }
   }

   return found;
}

void insert_name_or_chunks( const string& name, const string& full_name, size_t chunk_size, set< string >& results )
{
   if( chunk_size )
   {
      size_t offset = 0;
      size_t total_bytes = file_size( full_name );

      if( total_bytes <= chunk_size )
         results.insert( name );
      else
      {
         while( total_bytes )
         {
            string next_name( name );

            next_name += ':' + to_string( offset );

            size_t next_chunk = min( chunk_size, total_bytes );

            next_name += '+' + to_string( next_chunk );

            results.insert( next_name );

            offset += chunk_size;
            total_bytes -= next_chunk;
         }
      }
   }
   else
      results.insert( name );
}

string::size_type find_non_escaped_char( const string& s, char ch, string::size_type from = 0, char esc = '\\' )
{
   string::size_type pos = string::npos;

   bool had_escape = false;

   for( string::size_type i = from; i < s.length( ); i++ )
   {
      if( had_escape )
      {
         had_escape = false;
         continue;
      }
      else if( s[ i ] == esc )
         had_escape = true;
      else if( s[ i ] == ch )
      {
         pos = i;
         break;
      }
   }

   return pos;
}

void replace_input_arg_values( string& str, const vector< string >& args, char marker )
{
   string::size_type pos = str.find( marker );

   while( pos != string::npos )
   {
      if( str.size( ) > ( pos + 1 ) )
      {
         if( str[ pos + 1 ] == marker )
            ++pos;
         else if( ( str[ pos + 1 ] >= '0' ) && ( str[ pos + 1 ] <= '9' ) )
         {
            int argnum = str[ pos + 1 ] - '0';

            string argval( args[ argnum ] );

            // NOTE: If the argument has been quoted but the position at which it is being
            // inserted after is also a quote then remove the quotes from the argument.
            if( argval.length( ) > 1 && argval[ 0 ] == '"'
             && argval[ argval.length( ) - 1 ] == '"' && pos > 0 && str[ pos - 1 ] == '"' )
               argval = argval.substr( 1, argval.length( ) - 2 );

            str.erase( pos, 2 );
            str.insert( pos, argval );

            // NOTE: If the argument value is an empty string and the next character
            // is a space then erase this space to ensure the correct command syntax.
            if( argval.empty( ) && str[ pos ] == ' ' )
               str.erase( pos, 1 );

            pos += argval.length( );
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

            pos += all.length( );
         }
      }

      pos = str.find( marker, pos + 1 );
   }
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
   else if( command == c_cmd_monitor )
      handler.add_option( c_cmd_monitor );
   else if( command == c_cmd_no_pause )
      handler.add_option( c_cmd_no_pause );
   else if( command == c_cmd_no_prompt )
      handler.add_option( c_cmd_no_prompt );
   else if( command == c_cmd_no_stderr )
      handler.add_option( c_cmd_no_stderr );
   else if( command == c_cmd_no_progress )
      handler.add_option( c_cmd_no_progress );
}

command_functor* startup_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new startup_command_functor( handler );
}

struct choice
{
   char ch;
   string show;
   string value;
   string output;
};

bool is_choice_input( const string& input )
{
   string::size_type pos = input.find( '[' );

   if( pos != string::npos && input.find( ']', pos + 1 ) != string::npos )
      return true;
   else
      return false;
}

// Supports single key selection of one of a simple set choices for special input.
//
// If you used the following:
//
// &Continue? [yes=1|no=] (choose one)
//
// then it will output:
//
// Continue? [y]es, [n]o (choose one)
//
// and assuming you keyed in 'y' it would then output:
//
// Continue? yes
//
// but the actual string returned will be:
//
// Continue=1
//
// (in the above example if 'n' was selected it would return "Continue=" instead)
//
// If instead you used the following:
//
// &Continue? [CONT:yes=1!Yes|no=!No] (choose one)
//
// then the output after typing 'y' or 'n' would be 'Yes' or 'No' instead of 'yes'
// or 'no' and the actual string returned will now be:
//
// CONT=1
//
// (or CONT= if 'n' was pressed)

string get_input_from_choices( const string& input )
{
   string str( input );
   string var;

   string::size_type pos = input.find( '[' );

   if( pos != string::npos )
   {
      string::size_type rpos = input.find( ']', pos + 1 );

      if( rpos != string::npos )
      {
         vector< choice > choices;

         string choice_info( input.substr( pos + 1, rpos - pos - 1 ) );

         str.erase( pos, rpos - pos + 1 );

         string::size_type vpos = choice_info.find( ':' );

         if( vpos != string::npos )
         {
            var = choice_info.substr( 0, vpos );

            choice_info.erase( 0, vpos + 1 );
         }

         while( true )
         {
            string::size_type npos = choice_info.find( '|' );

            string next( choice_info.substr( 0, npos ) );

            choice next_choice;

            bool has_output = false;

            string::size_type opos = next.find( '!' );

            if( opos != string::npos )
            {
               next_choice.output = next.substr( opos + 1 );

               has_output = true;

               next.erase( opos );
            }

            string::size_type epos = next.find( '=' );

            if( ( epos == 0 ) || ( epos == string::npos ) )
               break;

            next_choice.value = next.substr( epos + 1 );

            next.erase( epos );

            char ch = '\0';
            bool found = false;

            for( size_t i = 0; i < next.length( ); i++ )
            {
               ch = next[ i ];

               if( !has_output )
                  next_choice.output += ch;

               bool okay = true;

               for( size_t j = 0; j < choices.size( ); j++ )
               {
                  if( choices[ j ].ch == ch )
                  {
                     okay = false;
                     break;
                  }
               }

               if( found || !okay )
                  next_choice.show += ch;
               else
               {
                  found = true;

                  next_choice.ch = ch;

                  next_choice.show += '[' + string( 1, ch ) + ']';
               }
            }

            choices.push_back( next_choice );

            if( npos == string::npos )
               break;

            choice_info.erase( 0, npos + 1 );
         }

         choice_info.erase( );

         for( size_t i = 0; i < choices.size( ); i++ )
         {
            if( i > 0 )
               choice_info += ", ";

            choice_info += choices[ i ].show;
         }

         str.insert( pos, choice_info );

         cout << str;
         cout.flush( );

         string value, output;
         bool found = false;

         while( !found )
         {
            char ch = get_char( );

            for( size_t i = 0; i < choices.size( ); i++ )
            {
               if( choices[ i ].ch == ch )
               {
                  value = choices[ i ].value;
                  output = choices[ i ].output;

                  found = true;
                  break;
               }
            }
         }

         size_t num_to_erase = str.length( ) - pos;

         cout << string( num_to_erase, '\b' )
          << string( num_to_erase, ' ' ) << string( num_to_erase, '\b' ) << output << endl;

         pos = str.find_first_of( "#$%=:?" );

         if( pos != string::npos )
         {
            str.erase( pos );

            if( var.empty( ) )
               str += '=';
            else
               var += '=';
         }

         if( !var.empty( ) )
            str = var;

         str += value;
      }
   }

   return str;
}

// FISSILE
//
// Functional
// Interpreted
// Simple
// Symbolic
// Integer and
// Lexical
// Evaluator
//
// NOTE: This experimental interpreter is for all "console_command_handler" applications.
// If a command starts with an asterix then it is assumed to in fact be a list of FISSILE
// commands (separated by spaces) which will be executed from left to right. It should be
// noted that numerics are unsigned 64 bit integers that when converted into a string are
// a string of X characters (so the number 6 would become the string "......") unless the
// value is greater than 99 (with the number 100 becoming the string "...[ 100 ]...").
//
// Some simple examples are as follows:
//
// *#=255
// *#16
// *#2
// *#*2
// *#
//
// *$=abc
// *$+def
// *.+
// *$*...
// *.-
// *$
//
// *@hello=$`Hello`world!`
// *!hello
//
// *@hex2dec=$@^x=$@?=#16#_@^x+@?_!^x_@?#10
// *!hex2dec?ffff
//
// *@ror=$@^^x=@?_@^^y=_>^^x,^^y_>^^y,^^x_@?=@^^x_@?
// *@rol=$@^^x=@?_@^^y=_<^^y,^^x_<^^x,^^y_@?=@^^x_@?
// *$=X... @? !ror !ror !ror !rol !rol !rol
//
// *@test=$xxx
// *@test
//
// *@test=$@^test2=$`.__?@?!!__<__!^test2_@?=#100_!^test2_``
// *!test
//
// Use the following to view all current variable values (^ prefixed are temporary):
// **

const char c_fissile_op_none = '\0';
const char c_fissile_base_suffix = '#';

const size_t c_fissile_max_depth = 100;

const size_t c_fissile_trunc_num_length = 100;

const char* const c_fissile_tmp_special_char = "^@s";

const char* const c_fissile_cmds_limit_variable = "*";

const char* const c_fissile_compressed_prefix = "...[ ";
const char* const c_fissile_compressed_suffix = " ]...";

struct fissile_pair
{
   const char* p_key;
   const char* p_data;
}
g_default_fissile_pairs[ ] =
{
   { "#", "10#" },
   { "*", "10000" },
   { "2#0", "" },
   { "2#1", "." },
   { "8#0", "" },
   { "8#1", "." },
   { "8#2", ".." },
   { "8#3", "..." },
   { "8#4", "...." },
   { "8#5", "....." },
   { "8#6", "......" },
   { "8#7", "......." },
   { "10#0", "" },
   { "10#1", "." },
   { "10#2", ".." },
   { "10#3", "..." },
   { "10#4", "...." },
   { "10#5", "....." },
   { "10#6", "......" },
   { "10#7", "......." },
   { "10#8", "........" },
   { "10#9", "........." },
   { "16#0", "" },
   { "16#1", "." },
   { "16#2", ".." },
   { "16#3", "..." },
   { "16#4", "...." },
   { "16#5", "....." },
   { "16#6", "......" },
   { "16#7", "......." },
   { "16#8", "........" },
   { "16#9", "........." },
   { "16#a", ".........." },
   { "16#b", "..........." },
   { "16#c", "............" },
   { "16#d", "............." },
   { "16#e", ".............." },
   { "16#f", "..............." },
};

class fissile_string
{
   friend ostream& operator <<( ostream& os, const fissile_string& fs );

   public:
   fissile_string( ) : len( 0 ), compressed( false ) { }

   fissile_string( const string& str ) : len( 0 ), data( str ), compressed( false ) { }

   fissile_string( uint64_t len ) : len( len ), compressed( true ) { }

   bool is_compressed( ) const { return compressed; }

   char& operator [ ]( uint64_t i )
   {
      if( compressed )
      {
         dummy = i < len ? '.' : '\0';
         return dummy;
      }
      else
         return data[ i ];
   }

   char operator [ ]( uint64_t i ) const
   {
      if( compressed )
         return  i < len ? '.' : '\0';
      else
         return data[ i ];
   }

   fissile_string& operator +=( const string& s )
   {
      fissile_string rhs( s );

      compress_if_possible( );
      rhs.compress_if_possible( );

      if( is_compressed( ) && rhs.is_compressed( ) )
         append( rhs.length( ) );
      else
      {
         uncompress( );
         data += s;
      }

      return *this;
   }

   void erase( uint64_t f = 0, uint64_t l = 0 )
   {
      if( f == 0 && l == 0 )
      {
         if( compressed )
            len = 0;
         else
            data.erase( );
      }
      else
      {
         if( compressed )
         {
            if( f < len )
            {
               if( l && l >= len )
                  len = 0;
               else
                  len = l ? len - l : len - f;
            }
         }
         else
         {
            if( l == 0 )
               data.erase( f );
            else
               data.erase( f, l );
         }
      }
   }

   uint64_t length( ) const
   {
      return compressed ? len : data.length( );
   }

   bool empty( ) const
   {
      return compressed ? len == 0 : data.empty( );
   }

   string substr( uint64_t i, uint64_t l = 0 ) const
   {
      if( compressed )
      {
         if( ( i >= len ) || ( l && ( l + i >= len ) ) )
            return string( );

         uint64_t slen = ( l ? l : len - i );

         if( slen < c_fissile_trunc_num_length )
            return string( slen, '.' );
         else
            return string( c_fissile_compressed_prefix + to_string( slen ) + c_fissile_compressed_suffix );
      }
      else
         return l ? data.substr( i, l ) : data.substr( i );
   }

   operator string( ) const
   {
      if( compressed )
      {
         if( len < c_fissile_trunc_num_length )
            return string( len, '.' );
         else
            return string( c_fissile_compressed_prefix + to_string( len ) + c_fissile_compressed_suffix );
      }
      else
         return data;
   }

   fissile_string& append( uint64_t amt )
   {
      if( compressed )
         len += amt;
      else
         data += string( amt, '.' );

      return *this;
   }

   fissile_string& remove( uint64_t amt )
   {
      if( compressed )
         len -= amt;
      else
         data = data.substr( 0, data.length( ) - amt );

      return *this;
   }

   fissile_string& expand( uint64_t amt )
   {
      if( compressed )
         len *= amt;
      else if( data.length( ) )
         data += string( data.length( ) * amt, '.' );

      return *this;
   }

   fissile_string& shrink( uint64_t amt )
   {
      if( compressed )
         len /= amt;
      else if( data.length( ) )
         data = data.substr( 0, data.length( ) / amt );

      return *this;
   }

   void uncompress( )
   {
      if( compressed )
      {
         compressed = false;

         if( len < c_fissile_trunc_num_length )
            data = string( len, '.' );
         else
            data = string( c_fissile_compressed_prefix + to_string( len ) + c_fissile_compressed_suffix );
      }
   }

   void compress_if_possible( )
   {
      if( !compressed )
      {
         bool can_compress = true;

         string::size_type ppos = data.find( c_fissile_compressed_prefix );

         if( ppos != string::npos )
         {
            string::size_type spos = data.rfind( c_fissile_compressed_suffix );

            if( spos == ( data.length( ) - strlen( c_fissile_compressed_suffix ) ) )
            {
               len = atoi( data.substr( strlen( c_fissile_compressed_prefix ),
                data.length( ) - strlen( c_fissile_compressed_prefix ) - strlen( c_fissile_compressed_suffix ) ).c_str( ) );

               compressed = true;
               data.erase( );

               can_compress = false;
            }
         }

         if( can_compress )
         {
            for( uint64_t i = 0; i < data.length( ); i++ )
            {
               if( data[ i ] != '.' )
               {
                  can_compress = false;
                  break;
               }
            }

            if( can_compress )
            {
               len = data.length( );

               compressed = true;
               data.erase( );
            }
         }
      }
   }

   private:
   char dummy;
   uint64_t len;

   string data;

   bool compressed;
};

ostream& operator <<( ostream& os, const fissile_string& fs )
{
   if( fs.compressed )
   {
      if( fs.len >= c_fissile_trunc_num_length )
         os << c_fissile_compressed_prefix << fs.len << c_fissile_compressed_suffix;
      else
      {
         for( uint64_t i = 0; i < fs.len; i++ )
            os << fs[ i ];
      }
   }
   else
      os << fs.data;

   return os;
}

string replace_char_with_spaces( const string& str, char ch = '_', bool double_as_single = true )
{
   string rs;

   for( size_t i = 0; i < str.size( ); i++ )
   {
      if( str[ i ] == ch )
      {
         if( double_as_single && ( i < str.size( ) - 1 ) && ( str[ i + 1 ] == ch ) )
            rs += str[ ++i ];
         else
            rs += ' ';
      }
      else
         rs += str[ i ];
   }

   // NOTE: If the last character is a space then instead treat it as a carriage return.
   if( !rs.empty( ) && rs[ rs.length( ) - 1 ] == ' ' )
      rs[ rs.length( ) - 1 ] = '\n';

   return rs;
}

void split_into_fissile_cmds( const string& next, deque< string >& cmds, bool add_marker = false )
{
   string::size_type start = 0;

   deque< string > temp_cmds;

   if( !next.empty( ) )
   {
      while( true )
      {
         string::size_type pos = next.find( ' ', start );

         temp_cmds.push_back( next.substr( start, pos - start ) );

         if( pos == string::npos )
            break;

         start = pos + 1;
      }

      if( add_marker && !temp_cmds.empty( ) )
         cmds.push_front( "@" );

      while( !temp_cmds.empty( ) )
      {
         cmds.push_front( temp_cmds.back( ) );
         temp_cmds.pop_back( );
      }
   }
}

string::size_type find_item_in_set( const string& item,
 const string& item_set, size_t* p_len = 0, char sep = ',' )
{
   string::size_type pos = string::npos;

   if( p_len )
      *p_len = item.length( );

   if( item.length( ) == item_set.length( ) )
      pos = item_set.find( item );
   else if( item.length( ) < item_set.length( ) )
   {
      pos = item_set.find( item + ',' );

      if( pos == 0 )
      {
         if( p_len )
            ++( *p_len );
      }
      else
      {
         pos = item_set.find( ',' + item + ',' );

         if( pos != string::npos )
         {
            if( p_len )
               ++( *p_len );
         }
         else
         {
            pos = item_set.rfind( ',' + item );

            if( pos == ( item_set.length( ) - item.length( ) - 1 ) )
            {
               if( p_len )
                  ++( *p_len );
            }
         }
      }
   }

   return pos;
}

bool compare_fissile_values( char op, const fissile_string& lhs, const fissile_string& rhs )
{
   bool rc = true;

   if( op == '=' )
   {
      if( lhs.length( ) != rhs.length( ) )
         rc = false;
      else
      {
         if( !lhs.is_compressed( ) || !rhs.is_compressed( ) )
         {
            for( size_t i = 0; i < lhs.length( ); i++ )
            {
               if( lhs[ i ] != rhs[ i ] && rhs[ i ] != '.' )
               {
                  rc = false;
                  break;
               }
            }
         }
      }
   }
   else if( op == '<' )
   {
      if( lhs.length( ) > rhs.length( ) )
         rc = false;
      else if( lhs.length( ) == rhs.length( ) )
      {
         rc = false;

         if( !lhs.is_compressed( ) || !rhs.is_compressed( ) )
         {
            for( size_t i = 0; i < lhs.length( ); i++ )
            {
               if( ( lhs[ i ] < rhs[ i ] ) && ( rhs[ i ] != '.' ) )
               {
                  rc = true;
                  break;
               }
            }
         }
      }
   }
   else if( op == '>' )
   {
      if( lhs.length( ) < rhs.length( ) )
         rc = false;
      else if( lhs.length( ) == rhs.length( ) )
      {
         rc = false;

         if( !lhs.is_compressed( ) || !rhs.is_compressed( ) )
         {
            for( size_t i = 0; i < lhs.length( ); i++ )
            {
               if( ( lhs[ i ] > rhs[ i ] ) && ( rhs[ i ] != '.' ) )
               {
                  rc = true;
                  break;
               }
            }
         }
      }
   }
   else if( op == '?' )
   {
      if( !rhs.length( ) || !lhs.length( ) )
         rc = false;
      else if( find_item_in_set( lhs, rhs ) == string::npos )
         rc = false;
   }

   return rc;
}

string get_fissile_value_as_cmds( const map< string, fissile_string >& fissile_values, const string& name )
{
   string cmds;

   if( fissile_values.count( name ) )
      cmds = replace_char_with_spaces( fissile_values.find( name )->second );

   return cmds;
}

string replace_variable_names_with_fissile_values( const map< string, fissile_string >& fissile_values, const string& str )
{
   string rs( str );

   string::size_type start = 0;

   while( true )
   {
      string::size_type pos = rs.find( '@', start );

      if( pos == string::npos )
         break;

      string::size_type rpos = rs.find( ' ', pos + 1 );

      string var_name;

      if( rpos == string::npos )
         var_name = rs.substr( pos + 1 );
      else
         var_name = rs.substr( pos + 1, rpos - pos - 1 );

      if( fissile_values.count( var_name ) )
      {
         rs.erase( pos, rpos - pos );
         rs.insert( pos, fissile_values.find( var_name )->second );

         pos = start + fissile_values.find( var_name )->second.length( );
      }

      start = pos + 1;
   }

   return rs;
}

string append_to_fissile_str(
 const map< string, fissile_string >& fissile_values,
 const string& str, size_t length_to_append, const string& rhs, bool use_special_character )
{
   string new_str( str );

   size_t old_first = 0;
   size_t old_length = str.length( );

   size_t lpos = 0;
   size_t rpos = 0;

   char special = '.';

   if( fissile_values.count( c_fissile_tmp_special_char ) )
   {
      string tmp_val( fissile_values.find( c_fissile_tmp_special_char )->second );

      if( tmp_val.empty( ) )
         special = '\0';
      else
         special = tmp_val[ 0 ];
   }
   else if( !use_special_character )
      special = '\0';

   while( length_to_append-- )
   {
      char ch = '.';

      if( !rhs.empty( ) )
      {
         if( rpos >= rhs.length( ) )
            rpos = 0;

         ch = rhs[ rpos++ ];
      }

      if( ( ch == special ) && old_length )
      {
         if( lpos >= old_length )
            lpos = 0;

         ch = str[ lpos++ ];
      }

      new_str += ch;
   }

   return new_str;
}

string expand_fissile_string(
 const map< string, fissile_string >& fissile_values,
 const string& str, const string& rhs, bool use_special_character )
{
   string new_str;

   char special = '.';

   if( fissile_values.count( c_fissile_tmp_special_char ) )
   {
      string tmp_val( fissile_values.find( c_fissile_tmp_special_char )->second );

      if( tmp_val.empty( ) )
         special = '\0';
      else
         special = tmp_val[ 0 ];
   }
   else if( !use_special_character )
      special = '\0';

   for( size_t i = 0; i < rhs.size( ); i++ )
   {
      if( rhs[ i ] == special )
         new_str += str;
      else
         new_str += string( str.length( ), rhs[ i ] );
   }

   return new_str;
}

size_t total_in_fissile_range(
 const map< string, fissile_string >& fissile_values,
 const string& type, vector< string >* p_symbols = 0 )
{
   map< string, fissile_string >::const_iterator ci = fissile_values.lower_bound( type );

   size_t total = 0;
   while( true )
   {
      ++total;

      if( p_symbols )
         p_symbols->push_back( ci->first.substr( type.length( ) ) );

      if( ++ci == fissile_values.end( )
       || ci->first.length( ) <= type.length( )
       || ci->first.substr( 0, type.length( ) ) != type )
         break;
   }

   return total;
}

string get_fissile_numeric(
 const map< string, fissile_string >& fissile_values, const string& type, const fissile_string& num )
{
   string str;

   deque< string > digits;
   vector< string > symbols;

   const map< string, fissile_string >::const_iterator ci = fissile_values.lower_bound( type );

   if( ci == fissile_values.end( ) )
      throw runtime_error( "fissile pattern type '" + type + "' not found" );

   uint64_t remaining_length( num.length( ) );

   size_t total = total_in_fissile_range( fissile_values, type, &symbols );

   if( remaining_length == 0 || symbols.size( ) == 1 )
      str = symbols[ 0 ];
   else
   {
      while( remaining_length )
      {
         digits.push_front( symbols[ remaining_length % total ] );

         if( remaining_length < total )
            remaining_length = 0;
         else if( remaining_length == total )
         {
            remaining_length = 0;
            digits.push_front( symbols[ 1 ] );
         }
         else
            remaining_length /= total;
      }
   }

   for( size_t j = 0; j < digits.size( ); j++ )
      str += digits[ j ];

   return str;
}

fissile_string handle_string_fissile_op(
 const map< string, fissile_string >& fissile_values,
 char op, const fissile_string& val, const fissile_string& src, bool use_special_character )
{
   fissile_string str( src ), next( val );

   if( op == '=' )
      str = next;
   else if( op == '+' )
   {
      if( str.is_compressed( ) && next.is_compressed( ) )
         str.append( next.length( ) );
      else
         str = append_to_fissile_str( fissile_values, str, next.length( ), next, use_special_character );
   }
   else if( op == '-' )
   {
      if( str.is_compressed( ) && next.is_compressed( ) )
         str.remove( next.length( ) );
      else
      {
         if( next.length( ) >= str.length( ) )
            str.erase( );
         else
            str = str.substr( 0, str.length( ) - next.length( ) );
      }
   }
   else if( op == '*' || op == '/' )
   {
      if( op == '*' )
      {
         if( next.empty( ) )
            str.erase( );
         else
         {
            if( str.is_compressed( ) && next.is_compressed( ) )
               str.expand( next.length( ) );
            else
               str = expand_fissile_string( fissile_values, str, next, use_special_character );
         }
      }
      else
      {
         if( !next.length( ) )
         {
            if( str.is_compressed( ) && next.is_compressed( ) )
               throw runtime_error( "attempt to divide by zero" );
            else
               throw runtime_error( "cannot shrink without fissile value" );
         }
         else
         {
            if( str.length( ) )
            {
               if( str.is_compressed( ) && next.is_compressed( ) )
                  str.shrink( next.length( ) );
               else
                  str = str.substr( 0, str.length( ) / next.length( ) );
            }
         }
      }
   }

   return str;
}

fissile_string handle_numeric_fissile_op(
 const map< string, fissile_string >& fissile_values, char op,
 const string& val, const string& default_base, fissile_string& src, bool use_special_character )
{
   string str, next( val );

   bool append = false;
   bool expand = false;
   bool remove = false;
   bool shrink = false;

   if( op == '=' )
   {
      src.erase( );
      append = true;
   }
   else if( op == '+' )
      append = true;
   else if( op == '*' )
      expand = true;
   else if( op == '-' )
      remove = true;
   else if( op == '/' )
      shrink = true;
   else if( op != c_fissile_op_none )
      throw runtime_error( "invalid numeric fissile op" );

   string::size_type tpos = next.find( '#' );

   if( ( tpos == string::npos ) && ( op == c_fissile_op_none ) )
   {
      if( !next.empty( ) )
         next += '#';
      else
         next = default_base;
   }
   else
   {
      if( tpos == ( next.length( ) - 1 ) )
      {
         next.erase( tpos );
         tpos = string::npos;
      }

      if( tpos == string::npos )
         next = default_base + next;
   }

   tpos = next.find( '#' );
   if( tpos == string::npos )
      throw runtime_error( "did not find fissile numeric type prefix" );

   string type( next.substr( 0, tpos + 1 ) );
   next.erase( 0, tpos + 1 );

   if( next.empty( ) )
      str = get_fissile_numeric( fissile_values, type, src );
   else
   {
      size_t total = total_in_fissile_range( fissile_values, type );

      fissile_string old_src( src );
      old_src.compress_if_possible( );

      src.erase( );

      for( size_t j = 0; j < next.size( ); j++ )
      {
         if( !src.empty( ) )
         {
            size_t length = src.length( );

            if( src.is_compressed( ) )
               src.expand( total );
            else
            {
               src.erase( );

               for( size_t k = 0; k < length; k++ )
                  src += string( total, '.' );
            }
         }

         const map< string, fissile_string >::const_iterator ci = fissile_values.find( type + next[ j ] );

         if( ci != fissile_values.end( ) && !ci->second.empty( ) )
            src += ci->second;
      }

      if( append )
      {
         if( old_src.is_compressed( ) )
            src = old_src.append( src.length( ) );
         else
            src = append_to_fissile_str( fissile_values, old_src, src.length( ), "", use_special_character );
      }
      else if( remove )
      {
         if( old_src.length( ) <= src.length( ) )
            src.erase( );
         else
         {
            if( old_src.is_compressed( ) )
               src = old_src.remove( src.length( ) );
            else
               src = old_src.substr( 0, old_src.length( ) - src.length( ) );
         }
      }
      else if( expand )
      {
         if( old_src.is_compressed( ) )
            src = old_src.expand( src.length( ) );
         else
            src = expand_fissile_string( fissile_values, old_src, src, use_special_character );
      }
      else if( shrink )
      {
         if( !src.length( ) )
            throw runtime_error( "attempt to divide by zero" );
         else if( old_src.length( ) )
         {
            if( old_src.is_compressed( ) )
               src = old_src.shrink( src.length( ) );
            else
               src = old_src.substr( 0, old_src.length( ) / src.length( ) );
         }
      }

      str = get_fissile_numeric( fissile_values, type, src );
   }

   return str;
}

string get_fissile_name( const map< string, fissile_string >& fissile_values, const string& name )
{
   string var_name( name );

   size_t depth = 0, max_depth = c_fissile_max_depth;

   while( true )
   {
      if( var_name.empty( ) || ( var_name == "?" ) || ( var_name == "!" ) )
         break;

      if( !fissile_values.count( var_name ) )
         throw runtime_error( "unknown fissile variable name '" + var_name + "'" );

      string new_name( fissile_values.find( var_name )->second );

      if( !new_name.empty( ) && ( new_name[ 0 ] == '@' ) )
         var_name = new_name.substr( 1 );
      else
         break;

      if( ++depth > max_depth )
         throw runtime_error( "maximum fissile indirection depth reached for: " + name );
   }

   return var_name;
}

fissile_string get_fissile_value(
 const map< string, fissile_string >& fissile_values, const string& var_name,
 fissile_string& data, const fissile_string& last_fissile_output, bool must_exist = true )
{
   fissile_string value;

   if( var_name == "?" )
      value = data;
   else if( var_name == "!" )
      value = last_fissile_output;
   else if( fissile_values.count( var_name ) && !fissile_values.find( var_name )->second.empty( ) )
      value = fissile_values.find( var_name )->second;
   else if( must_exist && !fissile_values.count( var_name ) )
      throw runtime_error( "unknown fissile variable name '" + var_name + "'" );

   size_t depth = 0, max_depth = c_fissile_max_depth;

   string items, remainder;

   while( true )
   {
      if( !value.empty( ) && ( value[ 0 ] == '@' ) )
      {
         string next_var( value.substr( 1 ) );

         string::size_type pos = next_var.find( "," );

         if( pos != string::npos )
         {
            remainder = next_var.substr( pos );
            next_var.erase( pos );
         }

         if( next_var == "?" )
            value = data;
         else if( next_var == "!" )
            value = last_fissile_output;
         else if( fissile_values.count( next_var ) )
            value = fissile_values.find( next_var )->second;
         else
         {
            if( !must_exist )
            {
               value.erase( );
               break;
            }
            else
               throw runtime_error( "unknown fissile variable name '" + next_var + "'" );
         }

         value += remainder;

         remainder.erase( );
      }
      else
      {
         if( !remainder.empty( ) )
         {
            value += remainder;

            remainder.erase( );
         }

         if( value.is_compressed( ) )
            break;

         string::size_type pos = string( value ).find( ",@" );

         if( pos == string::npos )
            break;
         else
         {
            if( pos != 0 )
            {
               if( !items.empty( ) )
                  items += ',';

               items += value.substr( 0, pos );
            }

            value.erase( 0, pos + 1 );
         }
      }

      if( ++depth > max_depth )
         throw runtime_error( "maximum fissile indirection depth reached for: " + ( string )value );
   }

   if( !items.empty( ) )
   {
      if( value.empty( ) )
         value = items;
      else
         value = items + ',' + ( string )value;
   }

   return value;
}

void process_fissile_commands(
 ostream& outs, bool interactive,
 const string& input, fissile_string& fissile_data,
 string& last_fissile_line, fissile_string& last_fissile_output,
 map< string, fissile_string >& fissile_values, bool& use_special_fissile_character )
{
   string next_fissile_line( input );

   bool has_output = false;

   size_t cmds_allowed = 0;

   deque< string > cmds;
   split_into_fissile_cmds( next_fissile_line, cmds );

   if( fissile_values.count( c_fissile_cmds_limit_variable ) )
      cmds_allowed = atoi( ( ( string )fissile_values[ c_fissile_cmds_limit_variable ] ).c_str( ) );

   while( !cmds.empty( ) )
   {
      string next( cmds.front( ) );

      bool is_last_command = ( cmds.size( ) == 1 );

      cmds.pop_front( );

      string default_base( 1, c_fissile_base_suffix );

      if( fissile_values.find( default_base ) != fissile_values.end( ) )
         default_base = fissile_values[ default_base ];

      if( isalpha( next[ 0 ] ) )
      {
         if( fissile_values.count( next ) )
            next = fissile_values[ next ];
      }

      if( next[ 0 ] == '?' )
      {
         string check_value, final_execute;

         fissile_string final_output;

         if( next.size( ) > 1 )
         {
            char op = '\0';

            if( ( next[ 1 ] == '=' ) || ( next[ 1 ] == '<' )
             || ( next[ 1 ] == '>' ) || ( next[ 1 ] == '?' ) )
            {
               op = next[ 1 ];
               next.erase( 1, 1 );
            }

            string::size_type pos = next.find_first_of( "!`", 2 );

            if( pos != string::npos )
            {
               final_execute = next.substr( pos );
               next.erase( pos );

               if( final_execute.size( ) > 1
                && ( final_execute[ 0 ] == '!' ) && ( final_execute[ 1 ] != '!' )
                && ( final_execute[ 1 ] != '?' ) && !isalpha( final_execute[ 1 ] ) )
                  final_execute.erase( 0, 1 );
            }

            string check_var( next.substr( 1 ) );

            // NOTE: The check variable can optionally be prefixed with @ so
            // that checking a special variable is not ambiguous with either
            // a ?? or !! or ?! command (therefore use ?@?!! to test data is
            // not empty or if it is then continue from the previous line).
            //
            // FUTURE: Perhaps such a prefix should be required in order for
            // literals to be used in place of variables.
            if( !check_var.empty( ) && ( check_var[ 0 ] == '@' ) )
               check_var.erase( 0, 1 );

            if( op != '\0' )
            {
               pos = check_var.find( ',' );

               if( pos == string::npos )
               {
                  check_value = compare_fissile_values( op,
                   fissile_data, get_fissile_value( fissile_values,
                   check_var, fissile_data, last_fissile_output, false ) ) ? "1" : "";
               }
               else
               {
                  string lhs_var( check_var.substr( 0, pos ) );
                  string rhs_var( check_var.substr( pos + 1 ) );

                  check_value = compare_fissile_values( op,
                   get_fissile_value( fissile_values, lhs_var, fissile_data, last_fissile_output ),
                   get_fissile_value( fissile_values, rhs_var, fissile_data, last_fissile_output, false ) ) ? "1" : "";
               }
            }
            else
               check_value = get_fissile_value( fissile_values,
                check_var, fissile_data, last_fissile_output, false );
         }
         else
            final_output = fissile_data;

         if( check_value.empty( ) )
         {
            if( ( final_execute.length( ) >= 2 ) && ( final_execute.substr( 0, 2 ) == "!!" ) )
            {
               while( !cmds.empty( ) )
               {
                  if( cmds.front( ) == "@" )
                     break;

                  cmds.pop_front( );
               }

               if( final_execute.size( ) > 2 )
                  final_execute.erase( 0, 1 );
               else
                  final_execute.clear( );
            }
            else
               cmds.clear( );

            if( !final_output.empty( ) )
            {
               last_fissile_output = final_output;

               if( !final_output.is_compressed( ) )
                  outs << final_output;
               else
                  outs << get_fissile_numeric( fissile_values, default_base, final_output );

               if( interactive )
                  outs << endl;

               has_output = true;
            }
            else if( !final_execute.empty( ) )
               cmds.push_front( final_execute );
         }
      }
      else if( next[ 0 ] == '~' )
      {
         if( next.size( ) > 1 )
         {
            if( next.substr( 1 ) == "*" )
               fissile_values.clear( );
            else if( next[ next.length( ) - 1 ] == '*' )
            {
               next.erase( next.length( ) - 1 );

               string pat( next.substr( 1 ) );

               while( true )
               {
                  if( fissile_values.lower_bound( pat ) == fissile_values.end( ) )
                     break;

                  if( fissile_values.lower_bound( pat )->first.length( ) < pat.length( )
                   || fissile_values.lower_bound( pat )->first.substr( 0, pat.length( ) ) != pat )
                     break;

                  fissile_values.erase( fissile_values.lower_bound( pat ) );
               }
            }
            else if( fissile_values.count( next.substr( 1 ) ) )
               fissile_values.erase( next.substr( 1 ) );
         }
      }
      else if( next[ 0 ] == '!' )
      {
         // NOTE: Command execution can be done in a few ways:
         // !              reprocess the last line of commands
         // !?             process "fissile_data" as a set of commands
         // !<name>        process fissile value <name> as a set of commands
         // !?<name>       optionally process fissile <name> as a set of commands
         // !<name>?<lit>  process <name> as a set of commands after setting "fissile_data" to <lit>
         // !?<name>?<lit> optionally process <name> as a set of commands after setting "fissile_data" to <lit>
         if( next.size( ) > 1 )
         {
            if( next == "!?" )
               split_into_fissile_cmds( fissile_data, cmds, true );
            else
            {
               bool optional = false;

               if( next.size( ) > 2 && ( next[ 1 ] == '?' ) )
               {
                  optional = true;
                  next.erase( 1, 1 );
               }

               fissile_string new_data( fissile_data );

               string::size_type pos = next.find( '?' );

               if( pos != string::npos )
               {
                  new_data = next.substr( pos + 1 );
                  next.erase( pos );
               }

               if( fissile_values.count( next.substr( 1 ) ) )
               {
                  fissile_data = new_data;

                  split_into_fissile_cmds(
                   get_fissile_value_as_cmds( fissile_values, next.substr( 1 ) ), cmds, true );
               }
               else if( !optional )
                  throw runtime_error( "cannot execute unknown fissile variable: " + next.substr( 1 ) );
            }
         }
         else if( !last_fissile_line.empty( ) )
            split_into_fissile_cmds( last_fissile_line, cmds );
      }
      else if( next[ 0 ] == '`' )
      {
         if( next.size( ) > 1 )
         {
            if( ( next.size( ) == 2 ) && ( next[ 1 ] == next[ 0 ] ) )
               outs << '\n';
            else
               outs << replace_variable_names_with_fissile_values(
                fissile_values, replace_char_with_spaces( next.substr( 1 ), '`' ) );

            has_output = true;
         }
      }
      else if( next[ 0 ] == '*' )
      {
         bool matching_prefix_only = false;

         if( ( next.length( ) > 2 ) && ( next[ next.length( ) - 1 ] == '*' ) )
         {
            matching_prefix_only = true;
            next.erase( next.length( ) - 1 );
         }

         for( map< string, fissile_string >::iterator i = !matching_prefix_only
          ? fissile_values.begin( ) : fissile_values.lower_bound( next.substr( 1 ) ); i != fissile_values.end( ); ++i )
         {
            if( matching_prefix_only && i->first.find( next.substr( 1 ) ) != 0 )
               break;

            outs << i->first;

            if( i->second.is_compressed( ) )
               outs << " (compressed) "
                << get_fissile_numeric( fissile_values, default_base, i->second ) << '\n';
            else
               outs << " (raw string) " << i->second << '\n';

            has_output = true;
         }
      }
      else if( ( next.length( ) > 2 ) && ( next.substr( 0, 2 ) == "*=" ) )
         fissile_values[ c_fissile_cmds_limit_variable ] = next.substr( 2 );
      else if( next == "--" )
      {
         if( fissile_data.is_compressed( ) )
            fissile_data.remove( 1 );
         else
         {
            for( size_t i = 0; i < fissile_data.length( ); i++ )
               --fissile_data[ i ];
         }
      }
      else if( ( next.length( ) > 2 ) && ( next.substr( 0, 2 ) == "--" ) )
      {
         next.erase( 0, 2 );
         string::size_type pos = next.find( ',' );

         string lhs_var( get_fissile_name( fissile_values, next.substr( 0, pos ) ) );
         string item_set( get_fissile_value( fissile_values, lhs_var, fissile_data, last_fissile_output ) );

         string item;
         if( pos == string::npos )
            item = fissile_data;
         else
            item = get_fissile_value( fissile_values, next.substr( pos + 1 ), fissile_data, last_fissile_output );

         if( !item_set.empty( ) )
         {
            size_t len;
            pos = find_item_in_set( item, item_set, &len );

            if( pos != string::npos )
            {
               item_set.erase( pos, len );

               if( lhs_var == "?" )
                  fissile_data = item_set;
               else if( lhs_var == "!" )
                  last_fissile_output = item_set;
               else
                  fissile_values[ lhs_var ] = item_set;
            }
         }
      }
      else if( next == "++" )
      {
         if( fissile_data.is_compressed( ) )
            fissile_data.append( 1 );
         else
         {
            for( size_t i = 0; i < fissile_data.length( ); i++ )
               ++fissile_data[ i ];
         }
      }
      else if( ( next.length( ) > 2 ) && ( next.substr( 0, 2 ) == "++" ) )
      {
         next.erase( 0, 2 );

         string::size_type pos = next.find( ',' );

         string lhs_var( get_fissile_name( fissile_values, next.substr( 0, pos ) ) );
         string item_set( get_fissile_value( fissile_values, lhs_var, fissile_data, last_fissile_output ) );

         string item;
         if( pos == string::npos )
            item = fissile_data;
         else
            item = get_fissile_value( fissile_values, next.substr( pos + 1 ), fissile_data, last_fissile_output );

         if( !item.empty( ) )
         {
            if( !item_set.empty( ) )
               item_set += ',';
            item_set += item;

            if( lhs_var == "?" )
               fissile_data = item_set;
            else if( lhs_var == "!" )
               last_fissile_output = item_set;
            else
               fissile_values[ lhs_var ] = item_set;
         }
      }
      else if( ( next[ 0 ] == '<' ) || ( next[ 0 ] == '>' ) )
      {
         char separator = ',';
         bool is_right = false;

         if( next[ 0 ] == '>' )
            is_right = true;

         bool is_item = false;

         if( ( next.size( ) > 1 ) && ( next[ 0 ] == next[ 1 ] ) )
         {
            is_item = true;

            next.erase( 0, 2 );

            if( !next.empty( ) && !isalnum( next[ next.length( ) - 1 ] ) )
            {
               separator = next[ next.length( ) - 1 ];

               next.erase( next.length( ) - 1 );

               if( separator == '`' )
                  separator = ' ';
            }
         }
         else
            next.erase( 0, 1 );

         string str_separator( 1, separator );

         if( next.empty( ) )
         {
            if( is_right && !fissile_data.empty( ) )
            {
               if( !is_item )
                  fissile_data.erase( fissile_data.length( ) - 1 );
               else
               {
                  string::size_type pos = string( fissile_data ).rfind( separator );

                  if( pos == string::npos )
                     fissile_data.erase( );
                  else
                     fissile_data.erase( pos );
               }
            }
            else if( !is_right && !fissile_data.empty( ) )
            {
               if( !is_item )
                  fissile_data.erase( 0, 1 );
               else
               {
                  string::size_type pos = string( fissile_data ).find( separator );

                  if( pos == string::npos )
                     fissile_data.erase( );
                  else
                     fissile_data.erase( 0, pos + 1 );
               }
            }

            if( !fissile_data.empty( ) && is_last_command )
            {
               last_fissile_output = fissile_data;

               outs << fissile_data;

               if( interactive )
                  outs << endl;

               has_output = true;
            }
         }
         else
         {
            string::size_type pos = next.find( separator );

            string lhs( next.substr( 0, pos ) );
            string rhs( pos == string::npos ? string( ) : next.substr( pos + 1 ) );

            if( fissile_values.count( lhs ) && ( rhs.empty( ) || fissile_values.count( rhs ) ) )
            {
               string lhs_value( get_fissile_value( fissile_values, lhs, fissile_data, last_fissile_output ) );

               string rhs_value( rhs.empty( ) ? fissile_data
                : get_fissile_value( fissile_values, rhs, fissile_data, last_fissile_output ) );

               if( is_right )
               {
                  if( !lhs_value.empty( ) )
                  {
                     if( !is_item )
                     {
                        rhs_value = lhs_value[ lhs_value.size( ) - 1 ] + rhs_value;
                        lhs_value.erase( lhs_value.size( ) - 1 );
                     }
                     else
                     {
                        string::size_type pos = lhs_value.rfind( separator );
                        if( pos == string::npos )
                        {
                           rhs_value = lhs_value
                            + ( rhs_value.empty( ) ? "" : str_separator ) + rhs_value;

                           lhs_value.erase( );
                        }
                        else
                        {
                           rhs_value = lhs_value.substr( pos + 1 )
                            + ( rhs_value.empty( ) ? "" : str_separator ) + rhs_value;

                           lhs_value.erase( pos );
                        }
                     }

                     fissile_values[ lhs ] = lhs_value;

                     if( rhs.empty( ) )
                        fissile_data = rhs_value;
                     else
                        fissile_values[ rhs ] = rhs_value;
                  }
               }
               else
               {
                  if( !rhs_value.empty( ) )
                  {
                     if( !is_item )
                     {
                        lhs_value += rhs_value[ 0 ];
                        rhs_value.erase( 0, 1 );
                     }
                     else
                     {
                        string::size_type pos = rhs_value.find( separator );

                        if( !lhs_value.empty( ) )
                           lhs_value += separator;

                        lhs_value += rhs_value.substr( 0, pos );

                        if( pos == string::npos )
                           rhs_value.erase( );
                        else
                           rhs_value.erase( 0, pos + 1 );
                     }

                     fissile_values[ lhs ] = lhs_value;

                     if( rhs.empty( ) )
                        fissile_data = rhs_value;
                     else
                        fissile_values[ rhs ] = rhs_value;
                  }
               }
            }
         }
      }
      else if( next[ 0 ] == '#' )
      {
         next.erase( 0, 1 );

         char op;
         if( next.empty( ) || isdigit( next[ 0 ] ) )
            op = c_fissile_op_none;
         else
         {
            op = next[ 0 ];
            next.erase( 0, 1 );
         }

         fissile_string new_data( fissile_data );

         fissile_string output( handle_numeric_fissile_op(
          fissile_values, op, next, default_base, new_data, use_special_fissile_character ) );

         if( op != c_fissile_op_none )
            fissile_data = new_data;
         else if( !output.empty( ) && is_last_command )
         {
            last_fissile_output = output;

            outs << output;

            if( interactive )
               outs << endl;

            has_output = true;
         }
      }
      else if( next[ 0 ] == '$' )
      {
         next.erase( 0, 1 );

         if( !next.empty( ) )
            fissile_data = handle_string_fissile_op( fissile_values,
             next[ 0 ], next.substr( 1 ), fissile_data, use_special_fissile_character );
         else if( !fissile_data.empty( ) && is_last_command )
         {
            last_fissile_output = fissile_data;

            outs << fissile_data;

            if( interactive )
               outs << endl;

            has_output = true;
         }
      }
      else if( next[ 0 ] == '@' )
      {
         next.erase( 0, 1 );

         string::size_type pos = next.find_first_of( "=+-*/#$%" );

         string dest_var( next.substr( 0, pos ) );

         if( pos == string::npos )
         {
            if( !next.empty( ) )
            {
               fissile_string value(
                get_fissile_value( fissile_values, dest_var, fissile_data, last_fissile_output ) );

               if( !value.empty( ) )
               {
                  last_fissile_output = value;

                  if( !value.is_compressed( ) )
                     outs << value;
                  else
                     outs << get_fissile_numeric( fissile_values, default_base, value );

                  if( interactive )
                     outs << endl;

                  has_output = true;
               }
            }
            else if( !fissile_data.empty( ) && is_last_command && !has_output )
            {
               last_fissile_output = fissile_data;

               outs << fissile_data;

               if( interactive )
                  outs << endl;

               has_output = true;
            }
         }
         else if( ( next[ pos ] == '=' ) && ( pos == next.size( ) - 1 ) )
         {
            if( dest_var == "?" )
               fissile_data.erase( );
            else if( dest_var == "!" )
               last_fissile_output.erase( );
            else
               fissile_values[ dest_var ] = string( );
         }
         else
         {
            if( next.size( ) )
            {
               char op = next[ pos ];

               char var_type;
               size_t offset = 1;

               if( ( op == '#' ) || ( op == '$' ) )
               {
                  op = c_fissile_op_none;
                  var_type = next[ pos ];
               }
               else
                  var_type = next[ pos + offset++ ];

               if( var_type == '@' )
               {
                  string src_var( next.substr( pos + 2 ) );

                  fissile_string value(
                   get_fissile_value( fissile_values, src_var, fissile_data, last_fissile_output ) );

                  if( dest_var == "?" )
                     fissile_data = handle_string_fissile_op(
                      fissile_values, op, value, fissile_data, use_special_fissile_character );
                  else if( dest_var == "!" )
                     last_fissile_output = handle_string_fissile_op(
                      fissile_values, op, value, last_fissile_output, use_special_fissile_character );
                  else
                     fissile_values[ dest_var ] = handle_string_fissile_op(
                      fissile_values, op, value, fissile_values[ dest_var ], use_special_fissile_character );
               }
               else if( var_type == '#' )
               {
                  fissile_string value(
                   get_fissile_value( fissile_values, dest_var, fissile_data, last_fissile_output, false ) );

                  fissile_string output( handle_numeric_fissile_op( fissile_values,
                   op, next.substr( pos + offset ), default_base, value, use_special_fissile_character ) );

                  if( op != c_fissile_op_none )
                  {
                     if( dest_var == "?" )
                        fissile_data = value;
                     else if( dest_var == "!" )
                        last_fissile_output = value;
                     else
                        fissile_values[ dest_var ] = value;
                  }
                  else if( !output.empty( ) )
                  {
                     last_fissile_output = output;

                     outs << output;

                     if( interactive )
                        outs << endl;

                     has_output = true;
                  }
               }
               else if( var_type == '$' )
               {
                  fissile_string value(
                   get_fissile_value( fissile_values, dest_var, fissile_data, last_fissile_output, false ) );

                  fissile_string output( handle_string_fissile_op(
                   fissile_values, op, next.substr( pos + offset ), value, use_special_fissile_character ) );

                  if( op != c_fissile_op_none )
                  {
                     if( dest_var == "?" )
                        fissile_data = output;
                     else if( dest_var == "!" )
                        last_fissile_output = output;
                     else
                        fissile_values[ dest_var ] = output;
                  }
                  else if( !output.empty( ) )
                  {
                     last_fissile_output = output;

                     outs << output;

                     if( interactive )
                        outs << endl;

                     has_output = true;
                  }
               }
               else if( var_type == '%' )
               {
                  set< string > items;

                  string value(
                   get_fissile_value( fissile_values, dest_var, fissile_data, last_fissile_output ) );

                  while( true )
                  {
                     string::size_type pos = value.find( ',' );

                     if( pos != 0 )
                        items.insert( value.substr( 0, pos ) );

                     if( pos == string::npos )
                        break;
                     else
                        value.erase( 0, pos + 1 );
                  }

                  string new_value;

                  for( set< string >::iterator i = items.begin( ); i != items.end( ); ++i )
                  {
                     if( !new_value.empty( ) )
                        new_value += ',';
                     new_value += *i;
                  }

                  fissile_values[ dest_var ] = new_value;
               }
               else
                  throw runtime_error( "invalid fissile variable expression: " + next );
            }
         }
      }
      else if( next[ 0 ] == '.' )
      {
         if( next.size( ) > 1 )
         {
            if( next[ 1 ] == '+' )
               use_special_fissile_character = true;
            else if( next[ 1 ] == '-' )
               use_special_fissile_character = false;
         }
      }
      else
         throw runtime_error( "unknown or invalid fissile command: " + next );

      if( cmds_allowed && ( --cmds_allowed == 0 ) && !cmds.empty( ) )
      {
         throw runtime_error( "fissile maximum command limit exceeded" );
         break;
      }
   }

   while( true )
   {
      map< string, fissile_string >::iterator i = fissile_values.lower_bound( "^" );

      if( i == fissile_values.end( ) || i->first.empty( ) || i->first[ 0 ] != '^' )
         break;

      fissile_values.erase( i->first );
   }

   last_fissile_line = next_fissile_line;
}

}

struct console_command_handler::impl
{
   ~impl( ) { output_file.close( ); }

   fissile_string fissile_data;

   string last_fissile_line;

   fissile_string last_fissile_output;

   bool use_special_fissile_character;

   impl( ) : use_special_fissile_character( false ) { }

   map< string, fissile_string > fissile_values;

   fstream output_file;
};

console_command_handler::console_command_handler( )
 :
 line_number( 0 ),
 input_depth( 0 ),
 p_std_err( &cerr ),
 p_std_out( &cout ),
 history_offset( 0 ),
 max_history_lines( c_max_history ),
 description_offset( 0 ),
 num_custom_startup_options( 0 ),
 ignore_prior( false ),
 is_reading_input( false ),
 is_skipping_to_label( false ),
 is_executing_commands( false ),
 allow_history_addition( true ),
 handling_startup_options( false )
{
#ifdef __GNUG__
#  ifdef RDLINE_SUPPORT
   if( isatty( STDIN_FILENO ) )
   {
      using_history( );

      // NOTE: In order for multi-line pastes to be handled as though
      // "enter" is pressed after each line need to switch this off.
      char* p_input_rc = strdup( "set enable-bracketed-paste off" );

      rl_parse_and_bind( p_input_rc );

      free( p_input_rc );
   }
#  endif
#endif
   add_option( c_prompt_prefix_option );

   for( size_t i = 0; i < c_max_args; i++ )
      args.push_back( string( ) );

   p_impl = new impl;

   for( size_t i = 0; i < sizeof( g_default_fissile_pairs ) / sizeof( g_default_fissile_pairs[ 0 ] ); i++ )
      p_impl->fissile_values.insert( make_pair( g_default_fissile_pairs[ i ].p_key, g_default_fissile_pairs[ i ].p_data ) );
}

console_command_handler::~console_command_handler( )
{
   delete p_impl;
}

bool console_command_handler::has_option_echo( ) const
{
   return has_option( c_cmd_echo );
}

bool console_command_handler::has_option_quiet( ) const
{
   return has_option( c_cmd_quiet );
}

bool console_command_handler::has_option_no_prompt( ) const
{
   return has_option( c_cmd_no_prompt );
}

bool console_command_handler::has_option_no_progress( ) const
{
   return has_option( c_cmd_no_progress );
}

bool console_command_handler::is_redirected_output( ) const
{
   return ( p_std_out == &p_impl->output_file );
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

void console_command_handler::preprocess_command_and_args( string& str, const string& cmd_and_args, bool skip_command_usage )
{
   str = cmd_and_args;

   remove_trailing_cr_from_text_file_line( str );

   if( handling_startup_options )
   {
      if( !str.empty( ) )
      {
         if( str[ 0 ] == c_startup_prefix )
            str.erase( 0, 1 );

         if( !str.empty( ) )
         {
            if( str[ 0 ] == c_output_command_usage
             || str == string( c_help_command ) || str == string( c_startup_alt_help ) )
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
      if( !is_executing_commands )
         ++line_number;

      string str_for_history( str );

      // NOTE: The history line is stored before prompted input to
      // prevent accidental infinite looping so for something like
      // "Yes, No, All or Quit" the "All" assignment would require
      // being in a conditional to prevent all further prompts.
      if( !is_skipping_to_label
       && dummy_conditions.empty( )
       && ( conditions.empty( ) || conditions.back( ) ) )
      {
         if( !str.empty( ) && ( str[ 0 ] == c_prompted_input_prefix ) )
         {
            string msg( c_default_value_prompt );

            if( str.length( ) > 1 )
               msg = str.substr( 1 );

            if( !is_choice_input( msg ) )
               str = msg + get_line( msg.c_str( ), false );
            else
               str = get_input_from_choices( msg + ' ' );
         }
      }

      string error_context;

      if( !script_file.empty( ) )
         error_context = " processing script '" + script_file + "' at line #" + to_string( line_number );

      if( is_skipping_to_label || !conditions.empty( ) )
      {
         if( !str.empty( ) && ( str[ 0 ] != c_envcond_command_prefix ) )
         {
            if( is_skipping_to_label || !conditions.back( ) || !dummy_conditions.empty( ) )
               str.erase( );
         }
      }

      bool was_password = false;

      bool add_to_history = allow_history_addition;

      // NOTE: Any single space prefixed command will not be added to history.
      if( !str.empty( ) && ( str[ 0 ] == ' ' ) )
      {
         str.erase( 0, 1 );
         add_to_history = false;

         clear_key( str_for_history );
      }

      bool skip_replacements = false;

      // NOTE: Any underbar prefixed command will skip the variable replacements.
      if( !str.empty( ) && ( str[ 0 ] == '_' ) )
      {
         str.erase( 0, 1 );
         skip_replacements = true;
      }

      string::size_type pos = str.find( ' ' );
      string::size_type apos = string::npos;

      if( isalpha( str[ 0 ] ) )
      {
         apos = str.find( c_environment_variable_assign );

         if( apos > pos )
            apos = string::npos;
      }

      if( has_option( c_cmd_echo ) )
      {
         if( str.empty( ) )
         {
            if( !has_option( c_cmd_quiet ) )
            {
               // NOTE: If using echo and not quiet then any commands that are
               // not being executed due to a conditional are output as though
               // they are comments (which can be helpful for debugging).
               if( str_for_history.empty( ) )
                  cout << endl;
               else
                  cout << c_comment_command_prefix << str_for_history << endl;
            }
         }
         else if( ( str[ 0 ] != c_quiet_command_prefix )
          && ( str[ 0 ] != c_comment_command_prefix ) && ( str[ 0 ] != c_history_command_prefix )
          && ( ( ( apos == string::npos ) && ( str[ 0 ] != c_read_input_prefix )
          && ( str[ 0 ] != c_write_output_prefix ) && ( str[ 0 ] != c_system_command_prefix )
          && ( str[ 0 ] != c_envcond_command_prefix ) && ( str[ 0 ] != c_message_command_prefix ) )
          || !has_option( c_cmd_quiet ) ) )
            cout << str << endl;
      }

      if( !str.empty( ) )
      {
         string assign_env_var_name;

         if( apos != string::npos )
         {
            // NOTE: Avoid creating a temporary string.
            assign_env_var_name.resize( apos );
            memcpy( &assign_env_var_name[ 0 ], &str[ 0 ], apos );

            str.erase( 0, apos + 1 );
         }

         if( !skip_replacements )
         {
            // NOTE: First try the format %<name>% and %* or %0..9 then try $* or $0..9 and $<name> so
            // console scripts can utilise either (or a combination) of these two environment variable
            // styles. The %<name>% has to be replaced before trying %* and %0..9 otherwise the second
            // % could confuse the replacement (if it is followed by a * or 0..9).
            replace_quoted_environment_variables( str );
            replace_input_arg_values( str, args, c_environment_variable_marker_2 );

            replace_input_arg_values( str, args, c_environment_variable_marker_1 );
            replace_unquoted_environment_variables( str );

            // NOTE: With either format doubled tokens (but not %LIKE%%THIS%) are replaced by a single.
            string doubled_marker_1( 2, c_environment_variable_marker_1 );
            replace( str, doubled_marker_1, string( 1, c_environment_variable_marker_1 ) );

            string doubled_marker_2( 2, c_environment_variable_marker_2 );
            replace( str, doubled_marker_2, string( 1, c_environment_variable_marker_2 ) );
         }

         // NOTE: For environment variable assignment VAR=@file:<fname> allows the variable to be set
         // to the file's content or even to the output of a system call using VAR=@file:~<cmd> (with
         // both the stdout and stderr output being redirected to a temporary file). Another usage is
         // VAR=@<expression>[<op>:] where the expression can be a special symbol (such as "unix" for
         // unix time stamp) or a single simple math operation between two numbers. This is useful to
         // work out the unix time stamp value one hour from the current time with the following:
         //
         // TIME=@unix+3600
         //
         // or to do something a little more exotic such as this:
         //
         // HRS_01=3600
         // HRS_24=@$HRS_01*24
         // TIME=@unix+$HRS_24
         //
         // Transformation functions can also be provided in the form VAR=@<func>:<lit> such as:
         //
         // HEX_LITTLE_ENDIAN=@hexlit:$TIME
         //
         // and a simple sub-string transformation function is also available:
         //
         // FIRST_FOUR_BYTES=@substr:0,8:$HEX_LITTLE_ENDIAN
         //
         // It is also possible to assign an environment variable to a FISSILE expression. Instead of
         // the usual @ use a * prefix for this. The following example is a binary to decimal literal
         // conversion followed then by the hexadecimal equivalent (noting that the value used in the
         // second expression was the one stored in the first).
         //
         // DEC=*#=2#11011 #
         // HEX=*#16
         //
         // Imagine if there was no @hexlit transformation but there was a @hexbig (of course if this
         // function wasn't one of a pair it would probably have a different name such as just @hex).
         // For the sake of demonstration let's just assume that to be the case. Now of course if one
         // needs to reverse the endian one could always add the missing @hexlit function but that is
         // something that would require recompiling the software (which is likely not what you might
         // want to be doing with a production system). This is where the FISSILE language (as tricky
         // as it is) can help out as it is entirely interpreted (so you can always add new functions
         // at runtime with it). To solve this fictitious example we'll first break down this problem
         // into several parts (partly why the language name was chosen is the approach needed to use
         // it effectively is to always break down the problem into a set of functional operations).
         //
         // First let's define a "split.n" function that will let us split an arbitrary string into a
         // set of items each having (at most) "n" characters (to test: TEST=*!split.n?2,01020304).
         //
         // *@split.n.add.char=$<^z,^y
         // *@split.n.add.sep=$?^z!!_@^z+$,
         // *@split.n.loop=$?^x!!_<^y_>^x,^w_!split.n.add.char_!split.n.loop
         // *@split.n.item=$?@?!!_!split.n.add.sep_@^x=@^n_@^w=_@^y=_!split.n.loop_!split.n.item
         // *@split.n=$@^n=_<<^n_@^nn=$@^n=#_@^nn+@^n_!^nn_@^z=_!split.n.item_@?=@^z
         //
         // Now let's define a "reverse.set" function that will reverse the order of items in a set.
         //
         // *@reverse.set.final=$@?=@^z
         // *@reverse.set.next=$?^x!!reverse.set.final_>>^x,^y_<<^z,^y_!reverse.set.next
         // *@reverse.set=$@^x=@?_@^y=_@^z=_!reverse.set.next
         //
         // Next we'll define a "join" function that joins the items from a set into one string (that
         // can be tested with: TEST=*!join)
         //
         // *@join.next=$?@?!!_<<^y_@^x+@^y_@^y=_!join.next
         // *@join=$@^x=_@^y=_!join.next_@?=@^x
         //
         // And we'll define a helper function to use the previous functions for our specific purpose.
         //
         // *@reverse_hex=$@^x=$!split.n?2,_@^x+@?_!^x_!reverse.set_!join_@?
         //
         // Finally let's use the newly created FISSILE functions to reverse the endianness.
         //
         // TIME=@unix:
         // HEX_BIG_ENDIAN=@hexbig:$TIME
         // LAST_FOUR_BYTES=@substr:8:$HEX_BIG_ENDIAN
         // LAST_FOUR_REVERSED=*!reverse_hex?$LAST_FOUR_BYTES
         //
         if( !assign_env_var_name.empty( ) )
         {
            bool not_replaced = false;

            if( !str.empty( ) && ( str[ 0 ] == '@' ) )
            {
               str.erase( 0, 1 );

               if( !str.empty( ) )
               {
                  // NOTE: There needs to have been at least
                  // one character before a valid operator.
                  pos = str.find_first_of( "+-*/#=<>:", 1 );

                  int64_t val = 0;
                  int64_t rval = 0;

                  string lhs;

                  if( pos != string::npos )
                  {
                     // NOTE: Avoid creating a temporary string.
                     lhs.resize( pos );
                     memcpy( &lhs[ 0 ], &str[ 0 ], pos );
                  }

                  if( lhs == string( c_unix_time ) )
                     val = unix_time( );

                  if( pos != string::npos )
                  {
                     char op = str[ pos ];

                     bool or_equal = false;
                     bool is_negative = false;
                     bool was_transform = false;

                     for( size_t i = pos + 1; i < str.length( ); i++ )
                     {
                        char ch = str[ i ];

                        if( i == pos + 1 )
                        {
                           if( ch == '=' )
                           {
                              or_equal = true;
                              continue;
                           }
                           else if( ch == '-' )
                           {
                              is_negative = true;
                              continue;
                           }
                        }

                        if( ch >= '0' && ch <= '9' )
                        {
                           rval *= 10;
                           rval += ( ch - '0' );
                        }
                        else
                           break;
                     }

                     if( is_negative )
                        rval *= -1;

                     if( !val )
                     {
                        if( op == ':' )
                        {
                           was_transform = true;

                           if( lhs == c_function_file )
                           {
                              string rhs( str.substr( pos + 1 ) );

                              if( !rhs.empty( )
                               && ( rhs[ 0 ] == c_system_command_prefix ) )
                              {
                                 string tmp_name( "~" + uuid( ).as_string( ) );

                                 rhs += " >" + tmp_name + " 2>&1";

                                 system_command( rhs.c_str( ) + 1 );

                                 str = buffer_file_lines( tmp_name );

                                 file_remove( tmp_name );
                              }
                              else
                              {
                                 if( !file_exists( rhs ) )
                                    throw runtime_error( "file '" + rhs + "' not found" );

                                 str = buffer_file_lines( rhs );
                              }
                           }
                           else if( lhs == c_function_files )
                           {
                              string rhs( str.substr( pos + 1 ) );

                              vector< string > includes;
                              vector< string > excludes;

                              pos = rhs.find( op );

                              size_t chunk_size = 0;

                              // NOTE: Allows for a comma separated list of strings to
                              // act as includes/excludes (prefix with ^ for excludes)
                              // for filtering file names. XX=@files:/path:^.tmp,^.sav
                              if( pos != string::npos )
                              {
                                 string all_extras( rhs.substr( pos + 1 ) );

                                 rhs.erase( pos );

                                 pos = all_extras.find( ':' );

                                 if( pos != string::npos )
                                 {
                                    chunk_size = unformat_bytes( all_extras.substr( pos + 1 ) );
                                    all_extras.erase( pos );
                                 }

                                 if( !all_extras.empty( ) )
                                    split_all_extras( all_extras, includes, excludes );
                              }

                              // NOTE: Ensure the path ends with a '/'.
                              if( rhs.empty( ) )
                                 rhs = "./";
                              else if( rhs[ rhs.length( ) - 1 ] != '/' )
                                 rhs += '/';

                              bool is_single_file_name = false;

                              if( excludes.empty( ) && ( includes.size( ) == 1 ) )
                              {
                                 string::size_type pos = includes[ 0 ].find_first_of( "?*" );

                                 if( pos == string::npos )
                                    is_single_file_name = true;
                              }

                              str.erase( );

                              set< string > results;

                              if( is_single_file_name )
                              {
                                 string single_file_name( includes[ 0 ] );

                                 if( file_exists( rhs + single_file_name ) )
                                    insert_name_or_chunks( single_file_name, rhs + single_file_name, chunk_size, results );
                              }
                              else
                              {
                                 file_filter ff;
                                 fs_iterator ffsi( rhs, &ff );

                                 while( ffsi.has_next( ) )
                                 {
                                    string next( ffsi.get_name( ) );

                                    if( !should_be_included( next, includes, excludes ) )
                                       continue;

                                    insert_name_or_chunks( next, ffsi.get_full_name( ), chunk_size, results );
                                 }
                              }

                              str = join( results, '\n' );
                           }
                           else if( lhs == c_function_lower )
                              str = lower( str.substr( pos + 1 ) );
                           else if( lhs == c_function_paths )
                           {
                              string rhs( str.substr( pos + 1 ) );

                              vector< string > includes;
                              vector< string > excludes;

                              pos = rhs.find( op );

                              // NOTE: Allows for a comma separated list of strings to
                              // act as includes/excludes (prefix with ^ for excludes)
                              // for filtering path names. XX=@paths:/path:^.tmp,^.sav
                              if( pos != string::npos )
                              {
                                 string all_extras( rhs.substr( pos + 1 ) );
                                 rhs.erase( pos );

                                 if( !all_extras.empty( ) )
                                    split_all_extras( all_extras, includes, excludes );
                              }

                              if( rhs.empty( ) )
                                 rhs = "./";

                              absolute_path( rhs, rhs );

                              directory_filter df;
                              fs_iterator dfsi( rhs, &df );

                              str.erase( );

                              set< string > results;
                              size_t len = rhs.length( ) + 1;

                              while( dfsi.has_next( ) )
                              {
                                 string next_path( dfsi.get_full_name( ) );

                                 if( next_path.length( ) <= len )
                                    continue;

                                 if( !should_be_included( next_path.substr( len ), includes, excludes ) )
                                    continue;

                                 results.insert( next_path.substr( len ) );
                              }

                              str = join( results, '\n' );
                           }
                           else if( lhs == c_function_upper )
                              str = upper( str.substr( pos + 1 ) );
                           else if( lhs == c_function_aschex )
                              str = hex_encode( str.substr( pos + 1 ) );
                           else if( lhs == c_function_dechex )
                           {
                              int64_t val = from_string< int64_t >( str.substr( pos + 1 ) );

                              stringstream ss;
                              ss << hex << val;

                              str = ss.str( );
                           }
                           else if( lhs == c_function_hexasc )
                              str = hex_decode( str.substr( pos + 1 ) );
                           else if( lhs == c_function_hexdec )
                           {
                              int64_t val;

                              stringstream ss( str.substr( pos + 1 ) );
                              ss >> hex >>  val;

                              str = to_string( val );
                           }
                           else if( ( lhs == c_function_hexbig ) || ( lhs == c_function_hexlit ) )
                           {
                              str = hex_encode( ( const unsigned char* )&rval, sizeof( rval ) );
#ifdef LITTLE_ENDIAN
                              if( lhs == c_function_hexbig )
                                 str = hex_reverse( str );
#else
                              if( lhs == c_function_hexlit )
                                 str = hex_reverse( str );
#endif
                           }
                           else if( lhs == c_function_padlen )
                           {
                              pos = str.find( op, pos + 1 );

                              if( pos != string::npos )
                              {
                                 string rhs( str.substr( pos + 1 ) );

                                 char pad = '0';

                                 str.erase( pos );
                                 pos = str.find( ',' );

                                 if( pos != string::npos )
                                    pad = str[ pos + 1 ];

                                 while( rhs.length( ) < rval )
                                    rhs = pad + rhs;

                                 str = rhs;
                              }
                           }
                           else if( lhs == c_function_repstr )
                           {
                              pos = find_non_escaped_char( str, op, pos + 1 );

                              if( pos != string::npos )
                              {
                                 string rhs( str.substr( pos + 1 ) );

                                 str.erase( pos );
                                 pos = find_non_escaped_char( str, '/' );

                                 string find, repl;
                                 find = str.substr( 0, pos );

                                 find.erase( 0, strlen( c_function_repstr ) + 1 );

                                 if( pos != string::npos )
                                    repl = str.substr( pos + 1 );

                                 unescape( find, c_special_characters );
                                 unescape( repl, c_special_characters );

                                 str = replace( rhs, find, repl );
                              }
                           }
                           else if( lhs == c_function_sha256 )
                              str = sha256( str.substr( pos + 1 ) ).get_digest_as_string( );
                           else if( lhs == c_function_substr )
                           {
                              pos = str.find( op, pos + 1 );

                              if( pos != string::npos )
                              {
                                 string rhs( str.substr( pos + 1 ) );

                                 str.erase( pos );

                                 pos = str.find( ',' );

                                 if( pos != string::npos )
                                 {
                                    int len = atoi( str.substr( pos + 1 ).c_str( ) );

                                    // NOTE: Use of a negative value for "len" will treat the
                                    // value as being that much less than the size of "rhs".
                                    if( len < 0 )
                                    {
                                       len *= -1;

                                       if( len >= rhs.size( ) )
                                          len = 0;
                                       else
                                          len = rhs.size( ) - len;
                                    }

                                    if( rhs.size( ) > rval + len )
                                       str = rhs.substr( rval, len );
                                    else if( rhs.size( ) > rval )
                                       str = rhs.substr( rval );
                                    else
                                       str.erase( );
                                 }
                                 else
                                 {
                                    // NOTE: Use of a negative value for "rval" will result
                                    // in the value becoming the last X characters of "rhs".
                                    if( rval < 0 )
                                    {
                                       rval *= -1;

                                       if( rval >= rhs.size( ) )
                                          str = rhs;
                                       else
                                          str = rhs.substr( rhs.size( ) - rval );
                                    }
                                    else
                                    {
                                       if( rhs.size( ) > rval )
                                          str = rhs.substr( rval );
                                       else
                                          str.erase( );
                                    }
                                 }
                              }
                           }
                           else if( lhs == c_function_fullpath )
                           {
                              string abs_path;

                              absolute_path( str.substr( pos + 1 ), abs_path );

                              str = abs_path;
                           }
                           else if( lhs == c_function_password )
                           {
                              string rhs( &str[ pos + 1 ] );

                              pos = rhs.find( ':' );

                              string pubkey;

                              if( pos != string::npos )
                              {
                                 pubkey = rhs.substr( pos + 1 );
                                 rhs.erase( pos + 1 );
                              }

                              if( !rhs.empty( ) )
                                 rhs += ' ';
                              else
                                 rhs = string( "Enter Password: " );

                              was_password = true;

                              char buffer[ c_max_pwd_size ];

                              get_password( rhs.c_str( ), buffer, c_max_pwd_size );

                              str.resize( strlen( buffer ) + 1 );
                              strncpy( &str[ 0 ], buffer, str.length( ) );

                              memset( buffer, '\0', c_max_pwd_size );

                              if( !pubkey.empty( ) )
                              {
#ifdef SSL_SUPPORT
                                 public_key pub_key( pubkey );
                                 private_key priv_key;

                                 string data( priv_key.encrypt_message( pub_key, str ) );

                                 data += ":" + priv_key.get_public( );

                                 clear_key( str );
                                 str = data;
#else
                                 throw runtime_error( "@password with public key crypto requires SSL support" );
#endif
                              }
                           }
                        }
                        else
                           val = from_string< int64_t >( str.substr( 0, pos ) );
                     }

                     if( !was_transform )
                     {
                        switch( op )
                        {
                           case '+':
                           val += rval;
                           break;

                           case '-':
                           val -= rval;
                           break;

                           case '*':
                           val *= rval;
                           break;

                           case '/':
                           val /= ( rval == 0 ) ? 1 : rval;
                           break;

                           case '#':
                           val = ( val % rval );
                           break;

                           case '=':
                           val = ( val == rval );
                           break;

                           case '<':
                           val = ( !or_equal ? val < rval : val <= rval );
                           break;

                           case '>':
                           val = ( !or_equal ? val > rval : val >= rval );
                           break;
                        }

                        str = to_string( val );
                     }
                  }
                  else if( val )
                     str = to_string( val );
                  else
                     not_replaced = true;
               }
               else
                  not_replaced = true;

               // NOTE: If was not a known function or numeric op then put back the '@' prefix.
               if( not_replaced )
                  str.insert( str.begin( ), '@' );
            }
            else if( !str.empty( ) && ( str[ 0 ] == '*' ) )
            {
               ostringstream osstr;

               process_fissile_commands( osstr, false,
                str.substr( 1 ), p_impl->fissile_data, p_impl->last_fissile_line,
                p_impl->last_fissile_output, p_impl->fissile_values, p_impl->use_special_fissile_character );

               str = osstr.str( );
            }
            else
               unescape( str, c_special_characters );

            if( input_depth
             && ( assign_env_var_name != c_env_var_error )
             && ( assign_env_var_name != c_env_var_output )
             && ( assign_env_var_name != c_env_var_ciyam_fissile )
             && !variables_retaining.top( ).count( c_retain_all )
             && !variables_retaining.top( ).count( assign_env_var_name )
             && !variables_prior_values.empty( ) && !variables_prior_values.top( ).count( assign_env_var_name ) )
               variables_prior_values.top( ).insert( make_pair( assign_env_var_name, get_environment_variable( assign_env_var_name ) ) );

            set_environment_variable( assign_env_var_name.c_str( ), str.c_str( ) );

            if( was_password )
               clear_key( str );

            str.erase( );
         }
         else
         {
            if( str[ 0 ] == c_quiet_command_prefix )
            {
               str.erase( 0, 1 );
               set_quiet_command( true );
            }
            else if( str[ 0 ] == c_read_input_prefix )
            {
               try
               {
                  size_t file_name_offset = 1;
                  bool keep_added_history = false;

                  size_t virtual_history_size = command_history.size( ) + history_offset;

                  // NOTE: If the read input prefix is repeated then each command read from the input file will
                  // be added to the history rather than adding the read input line itself as the history item.
                  if( ( str.size( ) > 1 ) && ( str[ 1 ] == c_read_input_prefix ) )
                  {
                     ++file_name_offset;

                     add_to_history = false;
                     keep_added_history = true;

#ifdef __GNUG__
#  ifdef RDLINE_SUPPORT
                     if( isatty( STDIN_FILENO ) && ( ignore_prior || ( str != prior_command ) ) )
                     {
                        prior_command = str;
                        add_history( str.c_str( ) );
                     }
#  endif
#endif
                  }

                  vector< string > new_args;

                  size_t num_new_args = setup_arguments( str.c_str( ) + file_name_offset, new_args );

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
                  vector< size_t > dummy_lines_vector;

                  // NOTE: As one script may call another all relevant state information must be
                  // captured before and then restored after processing each script.
                  restorable< size_t > tmp_line_number( line_number, 0 );
                  restorable< vector< string > > tmp_args( args, new_args );
                  restorable< string > tmp_script_file( script_file, new_args[ 0 ] );
                  restorable< vector< bool > > tmp_completed( completed, dummy_vector );
                  restorable< vector< bool > > tmp_conditions( conditions, dummy_vector );
                  restorable< bool > tmp_is_reading_input( is_reading_input, true );
                  restorable< bool > tmp_executing_commands( is_executing_commands, false );
                  restorable< bool > tmp_allow_history_addition( allow_history_addition, true );
                  restorable< size_t > tmp_max_history_lines( max_history_lines, c_max_history );
                  restorable< vector< bool > > tmp_dummy_conditions( dummy_conditions, dummy_vector );
                  restorable< vector< size_t > > tmp_lines_for_conditions( lines_for_conditions, dummy_lines_vector );

                  string next;
                  string next_command;

                  ++input_depth;

                  bool is_first = true;

                  variables_retaining.push( next_retaining );

                  variables_prior_values.push( map< string, string >( ) );

                  // NOTE: When a script begins "next_retaining" is cleared
                  // (so it will not accidentally pass on values to further
                  // input scripts).
                  next_retaining.clear( );

                  while( getline( inpf, next ) )
                  {
                     bool is_continuation = false;

                     if( next.empty( ) )
                        continue;

                     remove_trailing_cr_from_text_file_line( next, is_first );

                     if( is_first )
                        is_first = false;

                     // NOTE: A trailing backslash (which is not escaped)
                     // will be interpreted as being a line continuation.
                     string::size_type pos = next.find_last_not_of( '\\' );

                     if( ( pos != next.size( ) - 1 )
                      && ( ( next.size( ) - pos - 1 ) % 2 ) )
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

                  str.erase( );

                  --input_depth;

                  if( !keep_added_history )
                  {
                     size_t offset = virtual_history_size;

                     if( virtual_history_size <= history_offset )
                        offset = 0;
                     else
                        offset -= history_offset;

                     command_history.erase( command_history.begin( ) + offset, command_history.end( ) );
                     history_line_number.erase( history_line_number.begin( ) + offset, history_line_number.end( ) );
                  }

                  if( !conditions.empty( ) )
                     throw runtime_error( "missing 'endif' for conditional started at line #"
                      + to_string( lines_for_conditions.back( ) ) + " in '" + new_args[ 0 ] + "'" + error_context );

                  if( !inpf.eof( ) )
                     throw runtime_error( "unexpected error occurred whilst reading '" + new_args[ 0 ] + "'" + error_context );

                  if( !variables_retaining.empty( ) )
                     variables_retaining.pop( );

                  if( !variables_prior_values.empty( ) )
                  {
                     map< string, string >::const_iterator ci;

                     for( ci = variables_prior_values.top( ).begin( ); ci != variables_prior_values.top( ).end( ); ++ci )
                        set_environment_variable( ci->first, ci->second );

                     variables_prior_values.pop( );
                  }
               }
               catch( exception& x )
               {
                  str.erase( );

                  cout << "Error: " << x.what( ) << endl;
               }
               catch( ... )
               {
                  str.erase( );

                  cerr << "error: unexpected exception caught" << endl;
               }
            }
            else if( str[ 0 ] == c_write_output_prefix )
            {
               size_t file_name_offset = 1;
               ios::openmode output_flags = ios::out;

               // NOTE: If the write output prefix is repeated then open the output file for append.
               if( ( str.size( ) > 1 ) && ( str[ 1 ] == c_write_output_prefix ) )
               {
                  ++file_name_offset;
                  output_flags |= ios::app;
               }

               if( file_name_offset > ( str.size( ) - 1 ) )
               {
                  p_impl->output_file.close( );

                  p_std_out = &cout;
                  p_std_err = &cerr;
               }
               else
               {
                  p_impl->output_file.open( str.c_str( ) + file_name_offset, output_flags );

                  if( !p_impl->output_file )
                     throw runtime_error( "unable to open file '" + str.substr( file_name_offset ) + "' for output" );

                  p_std_out = p_std_err = &p_impl->output_file;
               }

               str.erase( );
            }
            else if( ( str[ 0 ] == c_output_command_usage ) || ( str.substr( 0, pos ) == c_help_command ) )
            {
#ifdef __GNUG__
#  ifdef RDLINE_SUPPORT
               if( add_to_history && isatty( STDIN_FILENO ) )
               {
                  if( ignore_prior || ( str != prior_command ) )
                     add_history( str.c_str( ) );
               }
#  endif
#endif
               if( !skip_command_usage )
               {
                  string wildcard_match_expr;

                  if( pos != string::npos )
                     wildcard_match_expr = str.substr( pos + 1 );

                  if( get_command_processor( ) )
                     get_command_processor( )->output_command_usage( wildcard_match_expr );

                  str.erase( );
               }

               add_to_history = false;
            }
            else if( str[ 0 ] == c_system_command_prefix )
            {
               system_command( str.c_str( ) + 1 );

               str.erase( );
            }
            else if( str[ 0 ] == c_history_command_prefix )
            {
               add_to_history = false;

               // NOTE: Use "!=" to display or "!=N" to set the max history size.
               if( ( str.size( ) > 1 ) && ( str[ 1 ] == '=' ) )
               {
                  if( str.size( ) == 2 )
                     cout << max_history_lines << '\n';
                  else
                  {
                     max_history_lines = from_string< size_t >( str.substr( 2 ) );

                     if( !max_history_lines )
                     {
                        history_offset = 0;

                        command_history.clear( );
                        history_line_number.clear( );
                     }

                     while( command_history.size( ) > max_history_lines )
                     {
                        ++history_offset;

                        command_history.pop_front( );
                        history_line_number.pop_front( );
                     }
                  }
               }
               else if( command_history.size( ) )
               {
                  if( str.size( ) > 1 )
                  {
                     if( ( str.size( ) > 2 ) && ( str[ 1 ] == c_write_output_prefix ) )
                     {
                        str.erase( 0, 1 );

                        size_t file_name_offset = 1;
                        ios::openmode output_flags = ios::out;

                        // NOTE: If the write output prefix is repeated then open the output file for append.
                        if( ( str.size( ) > 1 ) && ( str[ 1 ] == c_write_output_prefix ) )
                        {
                           ++file_name_offset;
                           output_flags = ios::app;
                        }

                        ofstream outf( str.c_str( ) + file_name_offset, output_flags );

                        if( !outf )
                           throw runtime_error( "unable to open file '" + str.substr( file_name_offset ) + "' for output" );

                        if( !command_history.empty( ) )
                        {
                           for( deque< string >::size_type i = 0; i < command_history.size( ); i++ )
                              outf << command_history[ i ] << '\n';
                        }
                     }
                     else
                     {
                        size_t offset = 1;

                        bool remove = false;

                        if( ( str.size( ) >= 2 ) && ( str[ 1 ] == '-' ) )
                        {
                           offset = 2;
                           remove = true;
                        }

                        int b = 0;
                        int e = 0;
                        int n = 0;
                        int r = 0;

                        bool is_loop = false;
                        bool is_range = false;
                        bool is_old_loop = false;

                        string::size_type pos = str.find( '#' );

                        if( pos != string::npos )
                        {
                           r = ( int )atoi( str.substr( pos + 1 ).c_str( ) );

                           if( r > command_history.size( ) )
                              r = command_history.size( );

                           is_range = true;
                           str.erase( pos );
                        }

                        if( ( str.size( ) >= 2 ) && ( str[ 1 ] == c_history_command_prefix ) )
                        {
                           if( ( str.size( ) > 2 ) && ( str[ 2 ] == '@' ) )
                           {
                              for( vector< string >::size_type i = command_history.size( ) - 1; i > 0; i-- )
                              {
                                 // NOTE: If an older loop is being re-run via history then make sure that
                                 // no history commands after that loop will be executed as a part of that.
                                 if( command_history[ i ] == str )
                                 {
                                    e = i + 1;
                                    is_old_loop = true;
                                 }
                                 else if( command_history[ i ] == str.substr( 2 ) )
                                 {
                                    b = i + 1;
                                    is_loop = true;

                                    // NOTE: Unlike normal history commands loops
                                    // are included in the history to allow these
                                    // to be nested.
                                    if( !is_executing_commands )
                                    {
                                       command_history.push_back( str_for_history );
                                       history_line_number.push_back( line_number );
                                    }

                                    break;
                                 }
                              }

                              if( !is_loop )
                                 throw runtime_error( "command '" + str.substr( 2 ) + "' not found in history" );
                           }
                           else if( ( str.size( ) == 3 ) && ( str[ 2 ] == '-' ) )
                           {
                              remove = true;

                              if( !is_range )
                                 n = command_history.size( );
                           }
                           else
                              n = ( int )command_history.size( );
                        }
                        else
                        {
                           if( offset >= str.length( ) )
                              n = ( ( !remove || is_range ) ? 0 : command_history.size( ) ); // i.e. treat !- the same as !!-
                           else
                              n = atoi( str.substr( offset ).c_str( ) );
                        }

                        if( ( n < 0 ) || ( n > ( int )( command_history.size( ) ) ) )
                           throw runtime_error( "command #" + str.substr( 1 ) + " is invalid" );

                        if( is_range )
                        {
                           if( n > 0 )
                              b = n - 1;
                           else if( r > 0 )
                              b = command_history.size( ) - r;

                           n = 0;

                           if( r > 0 )
                              e = b + r;
                           else
                              e = command_history.size( );
                        }

                        if( ( e == 0 ) || ( e > command_history.size( ) ) )
                           e = command_history.size( );

                        if( !remove )
                        {
                           restorable< size_t > tmp_line_number( line_number, line_number );

                           restorable< bool > tmp_executing_commands( is_executing_commands, true );
                           restorable< bool > tmp_allow_history_addition( allow_history_addition, false );

                           unique_ptr< restorable< bool > > up_tmp_is_skipping_to_label;

                           // NOTE: If an existing loop is being replayed then make sure that once it has been completed no further
                           // skipping will take place (otherwise the console could effectively end up "stuck" waiting for a label).
                           if( is_old_loop )
                              up_tmp_is_skipping_to_label.reset( new restorable< bool >( is_skipping_to_label, is_skipping_to_label ) );

                           if( n > 0 )
                           {
                              line_number = history_line_number[ n - 1 ];
                              execute_command( command_history[ n - 1 ] );
                           }
                           else
                           {
                              // NOTE: Rather than call "execute_command" for the actual loop will simply keep on
                              // executing each of the commands within the loop until it is skipping past the end
                              // of it (otherwise a large loop could cause a stack overflow to occur).
                              do
                              {
                                 for( vector< string >::size_type i = b; i < e; i++ )
                                 {
                                    if( is_loop && ( i == e - 1 ) )
                                    {
                                       if( is_skipping_to_label )
                                          is_loop = false;
                                    }
                                    else
                                    {
                                       line_number = history_line_number[ i ];
                                       execute_command( command_history[ i ] );
                                    }
                                 }
                              } while( is_loop );
                           }
                        }
                        else
                        {
                           if( n == 0 )
                           {
                              if( !is_range )
                              {
                                 history_offset = 0;

                                 command_history.clear( );
                                 history_line_number.clear( );
                              }
                              else
                              {
                                 command_history.erase( command_history.begin( ) + b, command_history.begin( ) + e );
                                 history_line_number.erase( history_line_number.begin( ) + b, history_line_number.begin( ) + e );
                              }
                           }
                           else
                           {
                              command_history.erase( command_history.begin( ) + ( n - 1 ) );
                              history_line_number.erase( history_line_number.begin( ) + ( n - 1 ) );
                           }
                        }
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
            {
               clear_key( str );
               str.erase( );
            }
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
                  {
                     symbol = expression.substr( pos + 1 );

                     // NOTE: A symbol is considered as being undefined
                     // if it is just empty quotes.
                     if( symbol == "\"\"" )
                        symbol.erase( );
                  }

                  // NOTE: If currently within an inactive conditional section
                  // then ignore 'skip', 'vars', 'depth' and 'label' commands.
                  if( !conditions.empty( )
                   && ( !conditions.back( ) || !dummy_conditions.empty( ) )
                   && ( ( token == c_envcond_command_skip ) || ( token == c_envcond_command_retain )
                   || ( token == c_envcond_command_depth ) || ( token == c_envcond_command_label ) ) )
                     token.erase( );

                  // NOTE: This first condition must take place before the next
                  // one in order for skipping to label to operate as expected.
                  if( !token.empty( ) && ( token[ 0 ] == ':' ) )
                  {
                     if( is_skipping_to_label && token.substr( 1 ) == label )
                     {
                        label.erase( );
                        is_skipping_to_label = false;
                     }
                  }
                  else if( token.empty( ) || is_skipping_to_label )
                     ; // i.e. do nothing
                  else if( token == c_envcond_command_ifeq )
                  {
                     if( !conditions.empty( ) && !conditions.back( ) )
                        dummy_conditions.push_back( 0 );
                     else
                     {
                        vector< string > cond_args;
                        size_t num_args = setup_arguments( symbol, cond_args );

                        if( num_args != 2 )
                           throw runtime_error( "unexpected num_args != 2 for 'ifeq' expression" + error_context );

                        completed.push_back( false );
                        conditions.push_back( cond_args[ 0 ] == cond_args[ 1 ] );

                        lines_for_conditions.push_back( line_number );
                     }
                  }
                  else if( token == c_envcond_command_ifdef )
                  {
                     if( !conditions.empty( ) && !conditions.back( ) )
                        dummy_conditions.push_back( 0 );
                     else
                     {
                        completed.push_back( false );
                        conditions.push_back( !symbol.empty( ) );

                        lines_for_conditions.push_back( line_number );
                     }
                  }
                  else if( token == c_envcond_command_ifneq )
                  {
                     if( !conditions.empty( ) && !conditions.back( ) )
                        dummy_conditions.push_back( 0 );
                     else
                     {
                        vector< string > cond_args;
                        size_t num_args = setup_arguments( symbol, cond_args );

                        if( num_args != 2 )
                           throw runtime_error( "unexpected num_args != 2 for 'ifneq' expression" + error_context );

                        completed.push_back( false );
                        conditions.push_back( cond_args[ 0 ] != cond_args[ 1 ] );

                        lines_for_conditions.push_back( line_number );
                     }
                  }
                  else if( token == c_envcond_command_ifndef )
                  {
                     if( !conditions.empty( ) && !conditions.back( ) )
                        dummy_conditions.push_back( 0 );
                     else
                     {
                        completed.push_back( false );
                        conditions.push_back( symbol.empty( ) );

                        lines_for_conditions.push_back( line_number );
                     }
                  }
                  else if( token == c_envcond_command_else )
                  {
                     if( !symbol.empty( ) )
                        throw runtime_error( "invalid 'else' expression" + error_context );

                     if( dummy_conditions.empty( ) )
                     {
                        if( conditions.empty( ) || completed.back( ) )
                           throw runtime_error( "no matching 'if' found for 'else' expression" + error_context );

                        bool val = conditions.back( );

                        completed.back( ) = true;

                        conditions.pop_back( );
                        conditions.push_back( !val );
                     }
                  }
                  else if( token == c_envcond_command_depth )
                  {
                     cout << "depth is " << conditions.size( ) << " at line #" << line_number;

                     if( !symbol.empty( ) )
                        cout << ' ' << symbol;

                     cout << endl;
                  }
                  else if( token == c_envcond_command_endif )
                  {
                     bool pop_cond = true;

                     if( !dummy_conditions.empty( ) )
                     {
                        pop_cond = false;
                        dummy_conditions.pop_back( );
                     }

                     if( pop_cond )
                     {
                        if( conditions.empty( ) )
                           throw runtime_error( "no matching 'if' found for 'endif' expression" + error_context );

                        completed.pop_back( );
                        conditions.pop_back( );

                        lines_for_conditions.pop_back( );
                     }
                  }
                  else if( token == c_envcond_command_skip )
                  {
                     if( !label.empty( ) )
                        is_skipping_to_label = true;
                  }
                  else if( token == c_envcond_command_label )
                  {
                     if( !symbol.empty( ) )
                        label = symbol;
                     else
                        cout << label << '\n';
                  }
                  else if( token == c_envcond_command_retain )
                  {
                     if( symbol.empty( ) )
                     {
                        if( !variables_retaining.empty( ) )
                        {
                           set< string >::const_iterator i;

                           for( i = variables_retaining.top( ).begin( ); i != variables_retaining.top( ).end( ); ++i )
                              cout << *i << endl;
                        }
                     }
                     else
                     {
                        split( symbol, next_retaining );

                        // NOTE: If retaining all then remove any specifics.
                        if( next_retaining.count( c_retain_all ) )
                        {
                           next_retaining.clear( );
                           next_retaining.insert( c_retain_all );
                        }
                     }
                  }
                  else
                     throw runtime_error( "invalid conditional expression '" + str + "'" + error_context );
               }

               str.erase( );
            }
            else if( str[ 0 ] == c_forget_command_prefix )
            {
               str.erase( 0, 1 );
               str_for_history.erase( );

               if( str.empty( ) )
                  ignore_prior = false;
            }
            else if( str[ 0 ] == c_ignore_command_prefix )
            {
               str.erase( 0, 1 );
               str_for_history = str;

               if( str.empty( ) )
                  ignore_prior = true;
               else
                  prior_command.erase( );
            }
            else if( str[ 0 ] == c_message_command_prefix )
            {
               if( str.size( ) > 1 )
               {
                  if( str[ 1 ] != c_message_command_prefix )
                     handle_command_response( str.substr( 1 ) );
                  else
                     handle_progress_message( str.substr( 2 ) );
               }

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
         }
      }

      if( !str_for_history.empty( ) )
      {
         if( add_to_history )
         {
#ifdef __GNUG__
#  ifdef RDLINE_SUPPORT
            if( !is_reading_input && isatty( STDIN_FILENO ) )
            {
               if( !ignore_prior && ( str_for_history == prior_command ) )
                  str_for_history.erase( );
               else
                  add_history( str_for_history.c_str( ) );
            }
#  endif
#endif
            if( max_history_lines && !str_for_history.empty( ) )
            {
               command_history.push_back( str_for_history );
               history_line_number.push_back( line_number );
            }

            if( !ignore_prior && !is_reading_input && !str_for_history.empty( ) )
               prior_command = str_for_history;
         }
      }

      if( !max_history_lines )
      {
         history_offset = 0;

         command_history.clear( );
         history_line_number.clear( );
      }
      else
      {
         while( command_history.size( ) > max_history_lines )
         {
            ++history_offset;

            command_history.pop_front( );
            history_line_number.pop_front( );
         }
      }
   }
}

bool console_command_handler::is_special_command( const string& cmd_and_args )
{
   if( !cmd_and_args.empty( ) && cmd_and_args[ 0 ] == '*' )
      return true;
   else
      return false;
}

void console_command_handler::handle_special_command( const string& cmd_and_args )
{
   if( !cmd_and_args.empty( ) )
   {
      process_fissile_commands( cout, true,
       cmd_and_args.substr( 1 ), p_impl->fissile_data, p_impl->last_fissile_line,
       p_impl->last_fissile_output, p_impl->fissile_values, p_impl->use_special_fissile_character );

      bool skip_repeated = false;

#ifdef __GNUG__
#  ifdef RDLINE_SUPPORT
      if( !is_reading_input && isatty( STDIN_FILENO ) )
      {
         if( !ignore_prior
          && ( !command_history.empty( ) && ( cmd_and_args == command_history.back( ) ) ) )
            skip_repeated = true;
         else
            add_history( cmd_and_args.c_str( ) );
      }
#  endif
#endif

      if( !skip_repeated )
      {
         command_history.push_back( cmd_and_args );
         history_line_number.push_back( line_number );

         if( command_history.size( ) > max_history_lines )
         {
            command_history.pop_front( );
            history_line_number.pop_front( );
         }
      }
   }
}

void console_command_handler::handle_unknown_command( const string& command, const string& cmd_and_args )
{
   ( void )cmd_and_args;

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
   if( !is_special || has_option( c_cmd_no_stderr ) )
      *p_std_out << response << endl;
   else
      *p_std_err << response << endl;
}

void console_command_handler::handle_progress_message( const string& message )
{
   string prefix( get_environment_variable( c_env_var_progress_prefix ) );

   if( has_option( c_cmd_monitor ) )
      put_line( prefix + message, message.length( ) != 1 );
   else if( message.length( ) == 1 )
   {
      *p_std_out << prefix << message;

      p_std_out->flush( );
   }
   else
      *p_std_out << prefix << message << endl;

   set_environment_variable( c_env_var_progress_prefix, "" );
}

void console_command_handler::handle_extraneous_custom_option( const string& option )
{
   throw runtime_error( "unexpected argument '" + option + "'" );
}

bool console_command_processor::is_still_processing( )
{
   return bool( cin );
}

void console_command_processor::get_cmd_and_args( string& cmd_and_args )
{
   string prompt;

   if( !get_is_continuation( ) && !handler.has_option( c_cmd_no_prompt ) )
   {
      prompt = string( c_command_prompt );
      string prefix( handler.get_option_value( c_prompt_prefix_option ) );

      if( !prefix.empty( ) )
         prompt.insert( 1, prefix );
   }

   get_line( cmd_and_args, prompt.c_str( ) );
}

void console_command_processor::output_command_usage( const string& wildcard_match_expr ) const
{
   cout << '\n';
   cout << "commands:";

   if( !wildcard_match_expr.empty( ) )
      cout << " " << wildcard_match_expr;

   cout << "\n=========\n";

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
      if( ( argv[ i ][ 0 ] == c_startup_prefix )
       || ( string( argv[ i ] ) == string( c_startup_alt_help ) )
       || ( string( argv[ i ] ) == string( 1, c_output_command_usage ) ) )
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
      if( ( argv[ i ][ 0 ] == c_startup_prefix )
       || ( string( argv[ i ] ) == string( c_startup_alt_help ) )
       || ( string( argv[ i ] ) == string( 1, c_output_command_usage ) ) )
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

void startup_command_processor::get_cmd_and_args( string& cmd_and_args )
{
   string next( args.back( ) );

   size_t pos = next.find( '=' );

   if( pos != string::npos )
      next[ pos ] = ' ';

   args.pop_back( );

   cmd_and_args = next;
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
