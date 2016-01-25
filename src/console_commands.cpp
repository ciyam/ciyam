// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2016 CIYAM Developers
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
#  include <map>
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
#include "macros.h"
#include "sha256.h"
#include "console.h"
#include "pointers.h"
#include "date_time.h"
#include "utilities.h"

#ifdef __GNUG__
#  ifdef RDLINE_SUPPORT
extern "C"
{
#     include <readline/history.h>
}
#  endif
#endif

using namespace std;

namespace
{

const int c_max_args = 10;
const int c_max_usage_width = 84;

const int c_max_history = 1000;

const char c_startup_prefix = '-';

const char* const c_help_command = "help";
const char* const c_default_value_prompt = "VALUE=";

#ifdef _WIN32
const char* const c_startup_alt_help = "/?";
#else
const char* const c_startup_alt_help = "--help";
#endif

const char c_read_input_prefix = '<';
const char c_write_history_prefix = '>';
const char c_quiet_command_prefix = '.';
const char c_output_command_usage = '?';
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

const char* const c_non_command_prefix = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";

const char* const c_unix_timestamp = "unix";

const char* const c_function_aschex = "aschex";
const char* const c_function_hexasc = "hexasc";
const char* const c_function_hexbig = "hexbig";
const char* const c_function_hexlit = "hexlit";
const char* const c_function_sha256 = "sha256";
const char* const c_function_substr = "substr";

const char* const c_envcond_command_else = "else";
const char* const c_envcond_command_endif = "endif";
const char* const c_envcond_command_ifdef = "ifdef";
const char* const c_envcond_command_ifndef = "ifndef";

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
const char* const c_message_press_any_key = "(press any key to continue)...";

string replace_input_arg_values( const vector< string >& args, const string& input, char marker )
{
   string str( input );

   string::size_type pos = str.find( marker );
   while( pos != string::npos )
   {
      if( str.size( ) > pos + 1 )
      {
         if( str[ pos + 1 ] == marker )
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

      pos = str.find( marker, pos + 1 );
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
// a string of X characters (so the number 10 would become the string ".........."). Thus
// care needs to be taken not to accidentally convert a large number into a string.
//
// Some simple examples are as follows:
//
// *#=255
// *#16
// *#2
// *#*2
// *#
//
// *.+
// *$=abc
// *$+def
// *$*...
// *$
//
// *@hello=$`Hello`world!`
// *!hello
//
// *@test=$@^test2=$?__@?__<__!^test2_#=10_!^test2
// *!test
//
// *@ror=$@^^x=@?_@^^y=_>^^x,^^y_>^^y,^^x_@?=@^^x_@?
// *@rol=$@^^x=@?_@^^y=_<^^y,^^x_<^^x,^^y_@?=@^^x_@?
// *$=X... @? !ror !ror !ror !rol !rol !rol

const char c_fissile_op_none = '\0';
const char c_fissile_base_suffix = '#';

const size_t c_fissile_max_depth = 100;

const char* const c_fissile_tmp_special_char = "^@s";

const char* const c_fissile_cmds_limit_variable = "*";

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
         if( i >= len || ( l && ( l + i >= len ) ) )
            return string( );

         return string( l ? l : len - i, '.' );
      }
      else
         return l ? data.substr( i, l ) : data.substr( i );
   }

   operator string( ) const
   {
      if( compressed )
         return string( len, '.' );
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
         data = string( len, '.' );
      }
   }

   void compress_if_possible( )
   {
      if( !compressed )
      {
         bool can_compress = true;

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
      for( uint64_t i = 0; i < fs.len; i++ )
         os << fs[ i ];
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
         if( double_as_single && i < str.size( ) - 1 && str[ i + 1 ] == ch )
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
            if( pos == item_set.length( ) - item.length( ) - 1 )
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
               if( lhs[ i ] < rhs[ i ] && rhs[ i ] != '.' )
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
               if( lhs[ i ] > rhs[ i ] && rhs[ i ] != '.' )
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

      if( ch == special && old_length )
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
                  str = str.substr( 0,  str.length( ) / next.length( ) );
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

   if( tpos == string::npos && op == c_fissile_op_none )
   {
      if( !next.empty( ) )
         next += '#';
      else
         next = default_base;
   }
   else
   {
      if( tpos == next.length( ) - 1 )
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
               src = old_src.substr( 0,  old_src.length( ) / src.length( ) );
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
      if( var_name.empty( ) || var_name == "?" || var_name == "!" )
         break;

      if( !fissile_values.count( var_name ) )
         throw runtime_error( "unknown fissile variable name '" + var_name + "'" );

      string new_name( fissile_values.find( var_name )->second );

      if( !new_name.empty( ) && new_name[ 0 ] == '@' )
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
      if( !value.empty( ) && value[ 0 ] == '@' )
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
 ostream& outs, bool interractive,
 const string& input, fissile_string& fissile_data,
 string& last_fissile_line, fissile_string& last_fissile_output,
 map< string, fissile_string >& fissile_values, bool& use_special_fissile_character )
{
   string next_fissile_line( input );

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

            if( next[ 1 ] == '=' || next[ 1 ] == '<' || next[ 1 ] == '>' || next[ 1 ] == '?' )
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
                && final_execute[ 0 ] == '!' && final_execute[ 1 ] != '!'
                && final_execute[ 1 ] != '?' && !isalpha( final_execute[ 1 ] ) )
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
            if( !check_var.empty( ) && check_var[ 0 ] == '@' )
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
            if( final_execute.length( ) >= 2 && final_execute.substr( 0, 2 ) == "!!" )
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

               if( interractive )
                  outs << endl;
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

               if( next.size( ) > 2 && next[ 1 ] == '?' )
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
            if( next.size( ) == 2 && next[ 1 ] == next[ 0 ] )
               outs << '\n';
            else
               outs << replace_variable_names_with_fissile_values(
                fissile_values, replace_char_with_spaces( next.substr( 1 ), '`' ) );
         }
      }
      else if( next[ 0 ] == '*' )
      {
         bool matching_prefix_only = false;

         if( next.length( ) > 2 && next[ next.length( ) - 1 ] == '*' )
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
         }
      }
      else if( next.length( ) > 2 && next.substr( 0, 2 ) == "*=" )
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
      else if( next.length( ) > 2 && next.substr( 0, 2 ) == "--" )
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
      else if( next.length( ) > 2 && next.substr( 0, 2 ) == "++" )
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
      else if( next[ 0 ] == '<' || next[ 0 ] == '>' )
      {
         char separator = ',';
         bool is_right = false;

         if( next[ 0 ] == '>' )
            is_right = true;

         bool is_item = false;
         if( next.size( ) > 1 && next[ 0 ] == next[ 1 ] )
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

               if( interractive )
                  outs << endl;
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

            if( interractive )
               outs << endl;
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

            if( interractive )
               outs << endl;
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

                  if( interractive )
                     outs << endl;
               }
            }
         }
         else if( next[ pos ] == '=' && pos == next.size( ) - 1 )
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

               if( op == '#' || op == '$' )
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

                     if( interractive )
                        outs << endl;
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

                     if( interractive )
                        outs << endl;
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

      if( cmds_allowed && --cmds_allowed == 0 && !cmds.empty( ) )
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
   fissile_string fissile_data;

   string last_fissile_line;

   fissile_string last_fissile_output;

   bool use_special_fissile_character;

   impl( ) : use_special_fissile_character( false ) { }

   map< string, fissile_string > fissile_values;
};

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
#  ifdef RDLINE_SUPPORT
   if( isatty( STDIN_FILENO ) )
      using_history( );
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
      if( !str.empty( ) && str[ 0 ] == c_prompted_input_prefix )
      {
         string msg( c_default_value_prompt );
         if( str.length( ) > 1 )
            msg = str.substr( 1 );

         str = msg;
         bool is_first = true;

         while( true )
         {
            char ch = get_char( is_first ? msg.c_str( ) : 0 );

            if( ch == '\r' )
               break;

            cout << ch;

            str += ch;
            is_first = false;
         }

         cout << '\n';
      }

#ifdef __GNUG__
#  ifdef RDLINE_SUPPORT
      if( isatty( STDIN_FILENO ) && !is_executing_commands && !str.empty( ) && str != last_command )
         add_history( str.c_str( ) );
#  endif
#endif
      string str_for_history( str );

      last_command = str_for_history;

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

      string::size_type pos = str.find( ' ' );
      string::size_type apos = string::npos;

      if( isalpha( str[ 0 ] ) )
         apos = str.substr( 0, pos ).find( c_environment_variable_assign );

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
         else if( str[ 0 ] != c_quiet_command_prefix
          && str[ 0 ] != c_comment_command_prefix && str[ 0 ] != c_history_command_prefix
          && ( ( apos == string::npos && str[ 0 ] != c_read_input_prefix
          && str[ 0 ] != c_write_history_prefix && str[ 0 ] != c_system_command_prefix
          && str[ 0 ] != c_envcond_command_prefix && str[ 0 ] != c_message_command_prefix )
          || !has_option( c_cmd_quiet ) ) )
            cout << str << endl;
      }

      if( !str.empty( ) )
      {
         string assign_env_var_name;
         if( apos != string::npos )
         {
            assign_env_var_name = str.substr( 0, apos );
            str.erase( 0, apos + 1 );
         }

         // NOTE: First try the format %<name>% and %* or %1..9 then try $* or $1..9 and $<name> so
         // console scripts can utilise either (or a combination) of these two environment variable
         // styles. The %<name>% has to be replaced before trying %* and %1..9 otherwise the second
         // % could confuse the replacement (if it is followed by a * or 1..9).
         str = replace_environment_variables( str.c_str( ), c_environment_variable_marker_2 );
         str = replace_input_arg_values( args, str, c_environment_variable_marker_2 );

         str = replace_input_arg_values( args, str, c_environment_variable_marker_1 );
         str = replace_unquoted_environment_variables( str.c_str( ), c_environment_variable_marker_1 );

         bool add_to_history = allow_history_addition;

         // NOTE: For environment variable assignment support VAR=@<fname> to set the variable to the
         // contents of a file (if wanting to literally assign VAR to "@fname" use "@@fname" instead)
         // or even to the output of a system call using VAR=@~<cmd> (with both the stdout and stderr
         // output being redirected to a temporary file). Another usage is VAR=@<expression> where an
         // expression can be a special symbol (such as "unix" for unix timestamp) or a single simple
         // math operation between two numbers (where the LHS can be a special symbol). The usage can
         // be handy for working out the unix timestamp value one hour from the current time with the
         // following:
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
         // Next we'll define a "join" function that joins the items from a set into a single string.
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
         // TIME=@unix
         // HEX_BIG_ENDIAN=@hexbig:$TIME
         // LAST_FOUR_BYTES=@substr:8:$HEX_BIG_ENDIAN
         // REV=*!reverse_hex?$LAST_FOUR_BYTES
         //
         if( !assign_env_var_name.empty( ) )
         {
            if( !str.empty( ) && str[ 0 ] == '@' )
            {
               str.erase( 0, 1 );

               if( file_exists( str ) )
                  str = buffer_file_lines( str );
               else if( !str.empty( ) && str[ 0 ] == '~' )
               {
                  string tmp_name( "~" + uuid( ).as_string( ) );
                  str += " 2>&1 >" + tmp_name;

                  int rc = system( str.substr( 1 ).c_str( ) );

                  ( void )rc;

                  str = buffer_file_lines( tmp_name );

                  file_remove( tmp_name );
               }
               else if( !str.empty( ) )
               {
                  pos = str.find_first_of( "+-*/:" );

                  int64_t val = 0;

                  string lhs( str.substr( 0, pos ) );

                  if( lhs == string( c_unix_timestamp ) )
                     val = unix_timestamp( );

                  if( pos != string::npos )
                  {
                     char op = str[ pos ];

                     bool was_transform = false;

                     int64_t rval = from_string< int64_t >( str.substr( pos + 1 ) );

                     if( !val )
                     {
                        if( op == ':' )
                        {
                           was_transform = true;

                           if( lhs == c_function_aschex )
                              str = hex_encode( str.substr( pos + 1 ) );
                           else if( lhs == c_function_hexasc )
                              str = hex_decode( str.substr( pos + 1 ) );
                           else if( lhs == c_function_hexbig || lhs == c_function_hexlit )
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
                                    size_t len = atoi( str.substr( pos + 1 ).c_str( ) );

                                    if( rhs.size( ) > rval + len )
                                       str = rhs.substr( rval, len );
                                    else if( rhs.size( ) > rval )
                                       str = rhs.substr( rval );
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
                           val /= rval == 0 ? 1 : rval;
                           break;
                        }

                        str = to_string( val );
                     }
                  }
                  else if( val )
                     str = to_string( val );
               }
            }
            else if( !str.empty( ) && str[ 0 ] == '*' )
            {
               ostringstream osstr;

               process_fissile_commands( osstr, false,
                str.substr( 1 ), p_impl->fissile_data, p_impl->last_fissile_line,
                p_impl->last_fissile_output, p_impl->fissile_values, p_impl->use_special_fissile_character );

               str = osstr.str( );
            }

            set_environment_variable( assign_env_var_name, str );
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
            else if( str[ 0 ] == c_output_command_usage || str.substr( 0, pos ) == c_help_command )
            {
               string wildcard_match_expr;
               if( pos != string::npos )
                  wildcard_match_expr = str.substr( pos + 1 );

               if( get_command_processor( ) )
                  get_command_processor( )->output_command_usage( wildcard_match_expr );

               str.erase( );
            }
            else if( str[ 0 ] == c_system_command_prefix )
            {
               int rc = system( str.c_str( ) + 1 );

               ( void )rc;

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
                  {
                     symbol = expression.substr( pos + 1 );

                     // NOTE: A symbol is considered as being undefined
                     // if it is just empty quotes.
                     if( symbol == "\"\"" )
                        symbol.erase( );
                  }

                  if( token == c_envcond_command_ifdef )
                  {
                     if( !conditions.empty( ) && !conditions.back( ) )
                        dummy_conditions.push_back( 0 );
                     else
                     {
                        completed.push_back( false );
                        conditions.push_back( !symbol.empty( ) );
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
                     }
                  }
                  else if( token == c_envcond_command_else )
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
                  else if( token == c_envcond_command_endif )
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
      process_fissile_commands( cout, true,
       cmd_and_args.substr( 1 ), p_impl->fissile_data, p_impl->last_fissile_line,
       p_impl->last_fissile_output, p_impl->fissile_values, p_impl->use_special_fissile_character );
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

