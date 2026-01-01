// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
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
#  include <cassert>
#  include <cstring>
#  include <map>
#  include <stack>
#  include <memory>
#  include <vector>
#  include <string>
#  include <sstream>
#  include <fstream>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#  include <sys/time.h>
#endif

#include "macros.h"
#include "console.h"
#include "pointers.h"
#include "utilities.h"

//#define DEBUG

/*

Expression Replacement Utility
------------------------------

Basic tokens:

`` becomes `

`' start/finish literal
`$ variable prefix

`= variable assign
`^ variable append

`, separator

`* append
`+ union/item separator
`% intersection/secondary set separator
`~ exclusive intersection

`: single find (use with `=)
`/ multiple find or replace (repeat)

`; comment/item padding

`{ left brace
`} right brace

`[ left bracket
`] right bracket

`( left parenthesis
`) right parenthesis

`& and
`| or
`! not
`? test

`@ function name prefix

`# include expression file

Note: If `! is used after the template value expression then the value expression will not be split into a set.

Special tokens for item expressions in template expressions (versions above zero require ` prior to initial \):

\\ \
\t tab
\r cr
\n lf
\# set ordinal
\+ set ordinal + 1
\$ initial value
\? modified value
\^ upper case initial value
\~ upper case modified value
\! switch between initial and modified secondary set values
\0..9 secondary set value (secondary set separator is literal text after `% in value expression)

Version zero only:
\=n (where n is a number between 1 and 9) expands to (n*2) \ characters (so \=5 becomes \\\\\\\\\\) *

Literal text (as used in template expressions) can be broken across multiple lines. Version zero expressions can be
split across lines with a trailing \ and for newer versions lines ending with `\ are considered line continuations.


Grammar:
--------

expression                    ::= "`{" sub_expression "`}"

sub_expression                ::= [ "`;" literal_text ] [ or_expression ] xrep_expression [ "`," sub_expression ]

xrep_expression               ::= literal_text
                              ::= basic_expression
                              ::= include_expression
                              ::= template_expression [ literal_text ]
                              ::= assign_append_expression or_expression

assign_append_expression      ::= variable_identifier "`="
                              ::= variable_identifier "`^"

or_expression                 ::= and_expression [ "`|" or_expression ]

and_expression                ::= unary_expression [ "`&" and_expression ]

unary_expression              ::= [ "`!" ] unary_expression
                              ::= "`?" variable_identifier
                              ::= function
                              ::= basic_expression

function                      ::= "`@eq" "`(" unary_expression "`," unary_expression "`)"
                              ::= "`@in" "`(" unary_set_expression "`," unary_set_expression "`)"
                              ::= "`@count" "`(" unary_set_expression "`)"

basic_expression              ::= set_union_expression
                              ::= "`(" or_expression "`)"

set_union_expression          ::= set_intersection_expression [ "`+" | "`*" set_union_expression ]

set_intersection_expression   ::= basic_set_expression [ "`%" | "`~" set_intersection_expression ]

basic_set_expression          ::= "`(" set_union_expression "`)"
                              ::= unary_set_expression

unary_set_expression          ::= set_function
                              ::= set_simple_expression

set_function                  ::= set_function_name "`(" set_union_expression "`)"

set_function_name             ::= "`@sort" | "`@upper" | "`@lower" | "`@encode"

set_simple_expression         ::= value_expression
                              ::= literal_expression

include_expression            ::= "`#" literal_text

template_expression           ::= "`[" template_value_expression template_item_expression "`]"

template_value_expression     :: value_expression [ "`!" | padding_expression ] [ primary_split_expression ] [ secondary_split_expression ] [ prefix_expression ]

value_expression              ::= variable_identifier [ replacement_expression ]
                              ::= include_expression [ replacement_expression ]

prefix_expression             ::= "`," | literal_expression

replacement_expression        ::= "`/" variable_or_literal_text "`/" [ variable_or_literal_text ] [ replacement_expression ]
                              ::= "`:" variable_or_literal_text "`=" [ variable_or_literal_text ] [ replacement_expression ]

variable_or_literal_text      ::= variable_identifier | literal_text

padding_expression            ::= "`;" number_and_opt_fill_character

primary_split_expression      ::= "`*" literal_text

secondary_split_expression    ::= "`%" literal_text

template_item_expression      ::= item_expression [ item_separator_expression ]

item_expression               ::= literal_text

item_separator_expression     ::= "`+" [ literal_text ]

variable_identifier           ::= "`$" name_expression

name_expression               ::= first_char subsequent_char ...

first_char                    ::= "A".."Z" | "a".."z" | "_"

subsequent_char               ::= first_char | "0".."9"

literal_expression            ::= "`'" escaped_character ... "`'"

literal_text                  ::= escaped_character ...

escaped_character             ::= "`" escaped_character
                              ::= simple_character

simple_character              ::= any_char except "`"

any_char                      ::= any printable character

*/

using namespace std;

const char c_escape = '`';
const char c_hidden_escape = '\1'; // NOTE: For MBCS '\0' would be better, however, error messages will end up truncated if present.

const char c_variable = '$';
const char c_optional = '?';

const char c_ordinal = '#';
const char c_ordinal1 = '+';
const char c_original = '$';
const char c_modified = '?';
const char c_upper_original = '^';
const char c_upper_modified = '~';
const char c_switch_secondary = '!';

const char c_template_escape = '\\';

const char* const c_quote = "`'";
const char* const c_op_or = "`|";
const char* const c_op_and = "`&";
const char* const c_op_not = "`!";
const char* const c_op_test = "`?";

const char* const c_op_add = "`+";
const char* const c_op_mod = "`%";
const char* const c_op_mul = "`*";
const char* const c_op_comma = "`,";
const char* const c_op_colon = "`:";
const char* const c_op_assign = "`=";
const char* const c_op_append = "`^";
const char* const c_op_exclude = "`~";
const char* const c_op_semicolon = "`;";

const char* const c_find_repl = "`/";

const char* const c_include = "`#";

const char* const c_left_brace = "`{";
const char* const c_right_brace = "`}";
const char* const c_left_bracket = "`[";
const char* const c_right_bracket = "`]";
const char* const c_left_parenthesis = "`(";
const char* const c_right_parenthesis = "`)";

const char* const c_escaped_special = "``";
const char* const c_variable_prefix = "`$";
const char* const c_function_prefix = "`@";

const char* const c_func_name_eq = "eq";
const char* const c_func_name_in = "in";
const char* const c_func_name_count = "count";

const char* const c_set_func_name_sort = "sort";
const char* const c_set_func_name_lower = "lower";
const char* const c_set_func_name_upper = "upper";
const char* const c_set_func_name_encode = "encode";

const char* const c_primary_split_value = "*";
const char* const c_template_prefix_name = "`";
const char* const c_secondary_split_value = "%";
const char* const c_original_variable_name = "$";
const char* const c_modified_variable_name = "?";
const char* const c_do_not_split_into_set_name = "!";
const char* const c_padding_info_variable_name = "#";

string c_true = string( 1, ( char )( 27 ) ); // i.e. ASCII ESC

string c_false;

const char* const c_minimum_version = "0";
const char* const c_using_version_1 = "1";

const char* const c_var_name_uuid = "uuid";
const char* const c_var_name_linux = "linux";
const char* const c_var_name_version = "version";

const char* const c_xrep_using_version_1 = "`{`$1`}";

const char* const c_ver_1_cont = "`\\";

size_t g_ver_1_cont_len = 0;

bool g_exec_system = false;
bool g_is_include_exception = false;

#ifdef DEBUG
int function_call_depth = -1;

struct function_call_debug
{
   function_call_debug( const char* p_name, bool is_opt )
   {
      cout << string( ++function_call_depth, ' ' ) << "(" << p_name << ") is_opt = " << is_opt << endl;
   }

   ~function_call_debug( ) { --function_call_depth; }
};
#endif

string encode( const string& input )
{
   string output;

   for( size_t i = 0; i < input.size( ); i++ )
   {
      if( input[ i ] == ' ' )
         output += "%20";
      else if( input[ i ] == '!' )
         output += "%21";
      else if( input[ i ] == '"' )
         output += "%22";
      else if( input[ i ] == '#' )
         output += "%23";
      else if( input[ i ] == '$' )
         output += "%24";
      else if( input[ i ] == '%' )
         output += "%25";
      else if( input[ i ] == '&' )
         output += "%26";
      else if( input[ i ] == '\'' )
         output += "%27";
      else if( input[ i ] == '(' )
         output += "%28";
      else if( input[ i ] == ')' )
         output += "%29";
      else if( input[ i ] == '*' )
         output += "%2A";
      else if( input[ i ] == '+' )
         output += "%2B";
      else if( input[ i ] == ',' )
         output += "%2C";
      else if( input[ i ] == '-' )
         output += "%2D";
      else if( input[ i ] == '.' )
         output += "%2E";
      else if( input[ i ] == '/' )
         output += "%2F";
      else if( input[ i ] == '<' )
         output += "%3C";
      else if( input[ i ] == '=' )
         output += "%3D";
      else if( input[ i ] == '>' )
         output += "%3E";
      else if( input[ i ] == '?' )
         output += "%3F";
      else if( input[ i ] == '@' )
         output += "%40";
      else if( input[ i ] == '[' )
         output += "%5B";
      else if( input[ i ] == '\\' )
         output += "%5C";
      else if( input[ i ] == ']' )
         output += "%5D";
      else if( input[ i ] == '^' )
         output += "%5E";
      else if( input[ i ] == '`' )
         output += "%60";
      else if( input[ i ] == '{' )
         output += "%7B";
      else if( input[ i ] == '|' )
         output += "%7C";
      else if( input[ i ] == '}' )
         output += "%7D";
      else if( input[ i ] == '~' )
         output += "%7E";
      else
         output += input[ i ];
   }

   return output;
}

size_t find_escape_level_sequence( const string& input, size_t pos = 0 )
{
   size_t retval( string::npos );

   int match_count = 0;

   for( size_t i = pos; i < input.length( ); i++ )
   {
      if( input[ i ] == '\\' )
         match_count = 1;
      else if( input[ i ] == '=' && match_count == 1 )
         ++match_count;
      else if( input[ i ] >= '0' && input[ i ] <= '9' && match_count == 2 )
         ++match_count;
      else
         match_count = 0;

      if( match_count == 3 )
      {
         retval = i - 2;
         break;
      }
   }

   return retval;
}

void replace_escape_level_sequence( string& str, size_t pos )
{
   int level = str[ pos + 2 ] - '0';

   string extra( "\\" );

   while( --level > 0 )
      extra += extra;

   str.replace( pos, 3, extra );
}

bool split_padding_info( const string& padding_info, int& num, char& fill, bool throw_on_error = true )
{
   num = 0;
   fill = '\0';

   for( string::size_type i = 0; i < padding_info.size( ); i++ )
   {
      if( padding_info[ i ] >= '0' && padding_info[ i ] <= '9' )
      {
         if( fill != '\0' )
         {
            if( !throw_on_error )
               return false;
            else
               throw runtime_error( "invalid padding info '" + padding_info + "'" );
         }

         num *= 10;
         num += padding_info[ i ] - '0';
      }
      else
      {
         if( fill != '\0' )
         {
            if( !throw_on_error )
               return false;
            else
               throw runtime_error( "invalid padding info '" + padding_info + "'" );
         }

         fill = padding_info[ i ];
      }
   }

   if( fill == '\0' )
      fill = ' ';

   return true;
}

string join_set( const vector< string >& set, const string& separator = " " )
{
   string retval;

   for( vector< string >::size_type i = 0; i < set.size( ); i++ )
   {
      if( i > 0 )
         retval += separator;

      retval += set[ i ];
   }

   return retval;
}

void split_set( const string& set_string, vector< string >& set, const string& separator = " " )
{
   string str( set_string );

   if( !str.empty( ) )
   {
      while( true )
      {
         string::size_type pos = str.find( separator.c_str( ) );
         string next( str.substr( 0, pos ) );

         set.push_back( next );

         if( pos == string::npos )
            break;

         str.erase( 0, pos + separator.size( ) );
      }
   }
}

string set_union( const string& lhs, const string& rhs, const string& separator = " " )
{
   string result;
   vector< string > lhs_set;
   vector< string > rhs_set;

   split_set( lhs, lhs_set, separator );
   split_set( rhs, rhs_set, separator );

   for( vector< string >::size_type i = 0; i < lhs_set.size( ); i++ )
   {
      if( find( rhs_set.begin( ), rhs_set.end( ), lhs_set[ i ] ) == rhs_set.end( ) )
      {
         if( !result.empty( ) )
            result += separator;

         result += lhs_set[ i ];
      }
   }

   for( vector< string >::size_type i = 0; i < rhs_set.size( ); i++ )
   {
      if( !result.empty( ) )
         result += separator;

      result += rhs_set[ i ];
   }

   return result;
}

string set_intersection( const string& lhs, const string& rhs, bool exclusive_intersection = false, const string& separator = " " )
{
   string result;
   vector< string > lhs_set;
   vector< string > rhs_set;

   split_set( lhs, lhs_set, separator );
   split_set( rhs, rhs_set, separator );

   for( vector< string >::size_type i = 0; i < lhs_set.size( ); i++ )
   {
      if( find( rhs_set.begin( ), rhs_set.end( ), lhs_set[ i ] ) != rhs_set.end( ) )
      {
         if( !exclusive_intersection )
         {
            if( !result.empty( ) )
               result += separator;
            result += lhs_set[ i ];
         }
      }
      else if( exclusive_intersection )
      {
         if( !result.empty( ) )
            result += separator;
         result += lhs_set[ i ];
      }
   }

   if( exclusive_intersection )
   {
      for( vector< string >::size_type i = 0; i < rhs_set.size( ); i++ )
      {
         if( find( lhs_set.begin( ), lhs_set.end( ), rhs_set[ i ] ) == lhs_set.end( ) )
         {
            if( !result.empty( ) )
               result += separator;
            result += rhs_set[ i ];
         }
      }
   }

   return result;
}

class xrep_info
{
   public:
   xrep_info( bool optional_variables = false )
    :
    version( 0 ),
    handled_include( false ),
    optional_variables( optional_variables )
   {
   }

   size_t get_version( ) const { return version; }
   void set_version( size_t version_num ) { version = version_num; }

   bool had_handled_include( ) const { return handled_include; }
   void set_handled_include( bool handled ) { handled_include = handled; }

   bool has_variable( const string& identifier );

   string get_variable( const string& identifier, bool opt = false );
   string set_variable( const string& identifier, const string& value );

   void remove_variable( const string& identifier );

   private:
   size_t version;

   bool handled_include;
   bool optional_variables;

   map< string, string > variables;
};

bool xrep_info::has_variable( const string& identifier )
{
   return variables.find( identifier ) != variables.end( );
}

string xrep_info::get_variable( const string& identifier, bool opt )
{
   string retval( c_false );

   string variable( identifier );
   string::size_type pos = identifier.find( c_variable );

   if( opt && pos != string::npos )
   {
      string prefix( identifier.substr( 0, pos ) );
      string suffix( identifier.substr( pos + 1 ) );

      if( !has_variable( suffix ) )
         variable.erase( );
      else
      {
         suffix = variables[ suffix ];
         variable = prefix + suffix;
      }
   }

   if( has_variable( variable ) )
      retval = variables[ variable ];
   else if( variable == "i" )
   {
      string prompt( "Input: " );

      if( has_variable( "ip" ) && !variables[ "ip" ].empty( ) )
         prompt = variables[ "ip" ];

      retval = get_line( prompt.c_str( ) );
   }
   else if( variable == "p" )
   {
      string prompt( "Password: " );

      if( has_variable( "pp" ) && !variables[ "pp" ].empty( ) )
         prompt = variables[ "pp" ];

      retval = get_password( prompt );
   }
   else if( !opt && !optional_variables )
      throw runtime_error( "variable '" + variable + "' has not been assigned" );

   return retval;
}

string xrep_info::set_variable( const string& identifier, const string& value )
{
   variables[ identifier ] = value;
   return value;
}

void xrep_info::remove_variable( const string& identifier )
{
   variables.erase( identifier );
}

void add_date_variables( xrep_info& xi )
{
   time_t t;
   struct tm* p_t;

   t = time( 0 );
   p_t = localtime( &t );

   xi.set_variable( "d", to_string( p_t->tm_mday ) );
   xi.set_variable( "m", to_string( p_t->tm_mon + 1 ) );
   xi.set_variable( "y", to_string( p_t->tm_year + 1900 ) );

   xi.set_variable( "ut", to_string( t ) );
}

struct lexer
{
   virtual bool has_input( ) = 0;
   virtual string get_input( ) = 0;

   virtual void push_input( ) = 0;
   virtual void pop_input( bool restore ) = 0;

   virtual string peek_next_token( ) = 0;

   virtual string read_next_token( ) = 0;
   virtual bool read_next_token( const string& token ) = 0;

   virtual string get_last_read_token( ) const = 0;

   virtual void write_back_token( const string& token ) = 0;

   virtual void set_error_width( int width ) = 0;
   virtual string get_input_error( const string& prefix ) = 0;
};

class xrep_lexer : public lexer
{
   public:
   xrep_lexer( const string& input, size_t version )
    :
    input( input ),
    version( version ),
    error_width( 1 )
   {
      initial = input;
   }

   bool has_input( )
   {
      return !input.empty( );
   }

   string get_input( )
   {
      return input;
   }

   void push_input( )
   {
      input_stack.push( input );
   }

   void pop_input( bool restore )
   {
      if( restore )
         input = input_stack.top( );
      input_stack.pop( );
   }

   string get_next_token( );

   string peek_next_token( )
   {
      return get_next_token( );
   }

   string read_next_token( );

   bool read_next_token( const string& token )
   {
      string::size_type pos = input.find( token );

      if( pos != 0 )
         return false;

      last_read_token = token;
      input.erase( 0, token.size( ) );

      return true;
   }

   string get_last_read_token( ) const { return last_read_token; }

   void write_back_token( const string& token );

   size_t get_version( ) const { return version; }

   void set_error_width( int width ) { error_width = width; }

   string get_input_error( const string& prefix );

   private:
   string input;
   string initial;

   size_t version;

   int error_width;

   string last_read_token;

   stack< string > input_stack;
};

class scoped_lexer_input_holder
{
   public:
   scoped_lexer_input_holder( lexer& l )
    :
    l( l ),
    restore_original( false )
   {
      l.push_input( );
   }

   void restore( )
   {
      restore_original = true;
   }

   ~scoped_lexer_input_holder( )
   {
      l.pop_input( restore_original );
   }

   private:
   lexer& l;
   bool restore_original;
};

string xrep_lexer::get_next_token( )
{
   string retval;

   if( !has_input( ) )
      throw runtime_error( "unexpected end-of-input" );

   string::size_type i;

   if( input[ 0 ] == c_escape )
   {
      if( input.size( ) == 1 )
         throw runtime_error( "unexpected trailing '" + string( 1, c_escape ) + "' found" );

      i = 2;
   }
   else
   {
      for( i = 0; i < input.size( ); i++ )
      {
         if( ( i < input.size( ) - 1 ) && ( input[ i + 1 ] == c_escape ) )
         {
            ++i;
            break;
         }
      }
   }

   if( i )
      retval = input.substr( 0, i );

   return retval;
}

string xrep_lexer::read_next_token( )
{
   string retval( get_next_token( ) );

   if( !retval.empty( ) )
   {
      last_read_token = retval;
      input.erase( 0, retval.size( ) );
   }

   return retval;
}

void xrep_lexer::write_back_token( const string& token )
{
   if( token != last_read_token )
      throw runtime_error( "attempt to write back token '" + token + "' when expecting token '" + last_read_token + "'" );

   input = token + input;
   last_read_token.erase( );
}

string xrep_lexer::get_input_error( const string& prefix )
{
   string val( prefix );

   val += initial;
   val += '\n';

   string::size_type pos = prefix.size( );

   for( string::size_type i = 0; i < initial.size( ); i++ )
   {
      if( i == initial.size( ) - input.size( ) )
         break;

      if( initial[ i ] == '\n' )
         pos = 0;
      else
         ++pos;
   }

   val += string( pos, ' ' );
   val += string( error_width, '^' );

   error_width = 1;

   return val;
}

class expression_base
{
   public:
   expression_base( expression_base* p_lhs = 0, expression_base* p_rhs = 0 )
    :
    okay( false ),
    up_lhs( p_lhs ),
    up_rhs( p_rhs )
   {
#ifdef DEBUG
      ++instance_count;
#endif
   }

   virtual ~expression_base( )
   {
#ifdef DEBUG
      --instance_count;
#endif
   }

   virtual string evaluate( xrep_info& xi ) = 0;

   virtual string get_label( ) const = 0;

   virtual string get_node_type( ) const = 0;

   void set_okay( bool new_okay = true ) { okay = new_okay; }

   expression_base* get_lhs( ) { return up_lhs.get( ); }
   expression_base* get_rhs( ) { return up_rhs.get( ); }

   const expression_base* get_lhs_expr( ) const { return up_lhs.get( ); }
   const expression_base* get_rhs_expr( ) const { return up_rhs.get( ); }

#ifdef DEBUG
   static int instance_count;
#endif

   protected:
   bool okay;

   unique_ptr< expression_base > up_lhs;
   unique_ptr< expression_base > up_rhs;
};

#ifdef DEBUG
int expression_base::instance_count = 0;
#endif

void dump_expression_nodes( const expression_base* p_node, ostream& outs, int indent = 0 )
{
   if( p_node )
   {
      string prefix;

      if( indent > 0 )
         prefix += string( indent, ' ' );

      outs << prefix << p_node->get_node_type( ) << " (" << p_node->get_label( ) << ")\n";

      outs << prefix << "[lhs]\n";
      dump_expression_nodes( p_node->get_lhs_expr( ), outs, indent + 1 );

      outs << prefix << "[rhs]\n";
      dump_expression_nodes( p_node->get_rhs_expr( ), outs, indent + 1 );
   }
}

class xrep_expression : public expression_base
{
   public:
   xrep_expression( expression_base* p_lhs, expression_base* p_rhs )
    : expression_base( p_lhs, p_rhs )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );
};

string xrep_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate xrep_expression" << endl;
#endif
   string lhs( up_lhs->evaluate( xi ) );

   if( !lhs.empty( ) )
      up_rhs->set_okay( );
   else
      up_rhs->set_okay( false );

   return up_rhs->evaluate( xi );
}

string xrep_expression::get_label( ) const { return string( " " ); }

string xrep_expression::static_node_type( ) { return "xrep_expression"; }

class okay_expression : public expression_base
{
   public:
   okay_expression( expression_base* p_lhs, expression_base* p_rhs = 0 )
    : expression_base( p_lhs, p_rhs )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );
};

string okay_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate okay_expression" << endl;
#endif
   if( okay )
      return up_lhs->evaluate( xi );
   else
   {
      if( !up_rhs.get( ) )
         return c_false;
      else
         return up_rhs->evaluate( xi );
   }
}

string okay_expression::get_label( ) const { return string( " " ); }

string okay_expression::static_node_type( ) { return "okay_expression"; }

class include_expression : public expression_base
{
   public:
   include_expression( expression_base* p_lhs )
    : expression_base( p_lhs )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );
};

void process_input( istream& is, xrep_info& xi, ostream& os, bool append_final_lf );

string include_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate include_expression" << endl;
#endif
   string str( up_lhs->evaluate( xi ) );

   bool use_current_variables = false;
   vector< pair< string, string > > variable_values;

   string::size_type pos = str.find( ' ' );
   string filename( str.substr( 0, pos ) );

   while( pos != string::npos )
   {
      str.erase( 0, pos + 1 );

      if( variable_values.empty( ) && ( str == "@" ) )
      {
         use_current_variables = true;
         break;
      }

      pos = str.find( '=' );

      if( pos == string::npos )
         throw runtime_error( "unexpected include argument format '" + str + "'" );

      string identifier = str.substr( 0, pos );
      str.erase( 0, pos + 1 );

      pos = str.find( ' ' );

      if( !str.empty( ) && str[ 0 ] == '"' )
      {
         str.erase( 0, 1 );

         for( string::size_type i = 0; i < str.size( ); i++ )
         {
            if( str[ i ] == '"' )
            {
               str.erase( i, 1 );
               pos = i;
               break;
            }
         }
      }

      string value = str.substr( 0, pos );

      if( !value.empty( ) && ( value[ 0 ] == c_variable || value[ 0 ] == c_optional ) )
         value = xi.get_variable( value.substr( 1 ), ( value[ 0 ] == c_optional ) );
      else
         value = unescaped( value, c_special_characters );

      variable_values.push_back( pair< string, string >( identifier, value ) );
   }

   if( !filename.empty( ) && filename[ 0 ] == c_variable )
      filename = xi.get_variable( filename.substr( 1 ) );

   string retval;

   // NOTE: If the filename is specified as "." then this is specially handled as though
   // an include had been successfully processed with an empty return string (i.e. dummy).
   if( filename == "." )
      xi.set_handled_include( true );
   else
   {
      ifstream inpf( filename.c_str( ) );

      if( !inpf )
         throw runtime_error( "unable to open file '" + filename + "' for input" );

      xrep_info new_xi;

      for( vector< pair< string, string > >::size_type i = 0; i < variable_values.size( ); i++ )
         new_xi.set_variable( variable_values[ i ].first, variable_values[ i ].second );

      if( !new_xi.has_variable( c_var_name_version ) )
         new_xi.set_variable( c_var_name_version, c_minimum_version );

      try
      {
         ostringstream osstr;
         process_input( inpf, use_current_variables ? xi : new_xi, osstr, false );

         retval = osstr.str( );
         xi.set_handled_include( true );
      }
      catch( exception& x )
      {
         g_is_include_exception = true;

         string xx( "(" );

         xx += filename;
         xx += ") ";
         xx += x.what( );

         throw runtime_error( xx );
      }
   }

   return retval;
}

string include_expression::get_label( ) const { return string( " " ); }

string include_expression::static_node_type( ) { return "include_expression"; }

class combined_expression : public expression_base
{
   public:
   combined_expression( expression_base* p_lhs, expression_base* p_rhs = 0 )
    : expression_base( p_lhs, p_rhs )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );
};

string combined_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate combined_expression" << endl;
#endif
   string retval( up_lhs->evaluate( xi ) );

   if( up_rhs.get( ) )
      retval += up_rhs->evaluate( xi );

   return retval;
}

string combined_expression::get_label( ) const { return string( " " ); }

string combined_expression::static_node_type( ) { return "combined_expression"; }

class replacement_expression : public expression_base
{
   public:
   replacement_expression( bool is_multi, const string& replacement_text,
    const string& replacement_variable, expression_base* p_lhs, expression_base* p_rhs = 0 )
    : expression_base( p_lhs, p_rhs ),
    is_multi( is_multi ),
    replacement_text( replacement_text ),
    replacement_variable( replacement_variable )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );

   bool get_is_multi( ) const { return is_multi; }
   const string& get_replacement_text( ) const { return replacement_text; }
   const string& get_replacement_variable( ) const { return replacement_variable; }

   private:
   bool is_multi;

   string replacement_text;
   string replacement_variable;
};

string replacement_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate replacement_expression" << endl;
#endif
   return up_lhs->evaluate( xi );
}

string replacement_expression::get_label( ) const
{
   string retval;

   retval += '"';
   retval += replacement_text;
   retval += replacement_variable;
   retval += '"';

   return retval;
}

string replacement_expression::static_node_type( ) { return "replacement_expression"; }

class padding_expression : public expression_base
{
   public:
   padding_expression( expression_base* p_lhs, const string& padding_info )
    : expression_base( p_lhs ),
    padding_info( padding_info )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );

   private:
   string padding_info;
};

string padding_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate padding_expression" << endl;
#endif
   string retval( up_lhs->evaluate( xi ) );

   xi.set_variable( c_padding_info_variable_name, padding_info );

   return retval;
}

string padding_expression::get_label( ) const { return string( " " ); }

string padding_expression::static_node_type( ) { return "padding_expression"; }

class prefix_expression : public expression_base
{
   public:
   prefix_expression( expression_base* p_lhs )
    : expression_base( p_lhs )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );
};

string prefix_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate prefix_expression" << endl;
#endif
   string retval( up_lhs->evaluate( xi ) );

   xi.set_variable( c_template_prefix_name, retval );

   return retval;
}

string prefix_expression::get_label( ) const { return string( " " ); }

string prefix_expression::static_node_type( ) { return "prefix_expression"; }

class value_expression : public expression_base
{
   public:
   value_expression( expression_base* p_lhs, expression_base* p_rhs = 0 )
    : expression_base( p_lhs, p_rhs ),
    primary_split_value( " " ),
    do_not_split_into_set_name( false )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );

   void set_primary_split_value( const string& value ) { primary_split_value = value; }

   void set_secondary_split_value( const string& value ) { secondary_split_value = value; }

   void set_do_not_split_into_set_name( ) { do_not_split_into_set_name = true; }

   private:
   string primary_split_value;
   string secondary_split_value;

   bool do_not_split_into_set_name;
};

string value_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate value_expression" << endl;
#endif
   string retval( up_lhs->evaluate( xi ) );

   xi.set_variable( c_original_variable_name, retval );

   expression_base* p_rhs( up_rhs.get( ) );

   while( p_rhs )
   {
      replacement_expression* p_rep_expr( dynamic_cast< replacement_expression* >( p_rhs ) );

      if( !p_rep_expr )
         throw runtime_error( "unexpected non-replacement expression in rhs of value expression" );

      string::size_type lpos = 0;

      string find_text( p_rep_expr->evaluate( xi ) );

      while( true )
      {
         string::size_type pos = retval.find( find_text, lpos );
         if( pos == string::npos )
            break;

         retval.erase( pos, find_text.length( ) );

         string value( p_rep_expr->get_replacement_text( ) );
         string variable( p_rep_expr->get_replacement_variable( ) );

         if( !variable.empty( ) )
            value = xi.get_variable( variable );

         retval.insert( pos, value );
         lpos = pos + value.length( );

         if( !p_rep_expr->get_is_multi( ) )
            break;
      }

      p_rhs = p_rhs->get_rhs( );
   }

   xi.set_variable( c_modified_variable_name, retval );
   xi.set_variable( c_primary_split_value, primary_split_value );

   if( !secondary_split_value.empty( ) )
      xi.set_variable( c_secondary_split_value, secondary_split_value );

   if( do_not_split_into_set_name )
      xi.set_variable( c_do_not_split_into_set_name, "" );

   return retval;
}

string value_expression::get_label( ) const { return string( " " ); }

string value_expression::static_node_type( ) { return "value_expression"; }

class item_expression : public expression_base
{
   public:
   item_expression( const string& separator_text, expression_base* p_lhs )
    : expression_base( p_lhs ),
    separator_text( separator_text )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );

   const string& get_separator_text( ) const { return separator_text; }

   private:
   string separator_text;
};

string item_expression::get_label( ) const
{
   string retval;

   retval += '"';
   retval += separator_text;
   retval += '"';

   return retval;
}

string item_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate item_expression" << endl;
#endif
   return up_lhs->evaluate( xi );
}

string item_expression::static_node_type( ) { return "item_expression"; }

class template_expression : public expression_base
{
   public:
   template_expression( expression_base* p_lhs, expression_base* p_rhs )
    : expression_base( p_lhs, p_rhs )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );
};

string template_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate template_expression" << endl;
#endif
   string lhs( up_lhs->evaluate( xi ) );
   string prefix;

   prefix_expression* p_prefix = dynamic_cast< prefix_expression* >( up_lhs->get_lhs( ) );

   if( p_prefix )
      prefix = unescaped( p_prefix->evaluate( xi ), c_special_characters );

   vector< string > original_set;
   vector< string > modified_set;

   if( xi.has_variable( c_do_not_split_into_set_name ) )
   {
      xi.remove_variable( c_do_not_split_into_set_name );

      original_set.push_back( xi.get_variable( c_original_variable_name ) );
      modified_set.push_back( xi.get_variable( c_modified_variable_name ) );
   }
   else
   {
      string primary_separator( unescaped( xi.get_variable( c_primary_split_value ), c_special_characters ) );

      split_set( xi.get_variable( c_original_variable_name ), original_set, primary_separator );
      split_set( xi.get_variable( c_modified_variable_name ), modified_set, primary_separator );
   }

   string padding_info;

   if( xi.has_variable( c_padding_info_variable_name ) )
   {
      padding_info = xi.get_variable( c_padding_info_variable_name );

      xi.remove_variable( c_padding_info_variable_name );

      int num;
      char fill;

      split_padding_info( padding_info, num, fill );

      for( vector< string >::size_type i = 0; i < modified_set.size( ); i++ )
      {
         int pad = num - modified_set[ i ].size( );

         if( pad > 0 )
            modified_set[ i ] = modified_set[ i ] + string( pad, fill );
      }
   }

   string rhs( up_rhs->evaluate( xi ) );

   string separator;

   item_expression* p_item = dynamic_cast< item_expression* >( up_rhs.get( ) );

   if( p_item )
      separator = unescaped( p_item->get_separator_text( ), c_special_characters );

   string secondary_separator;

   if( xi.has_variable( c_secondary_split_value ) )
      secondary_separator = unescaped( xi.get_variable( c_secondary_split_value ), c_special_characters );

   string retval;

   for( vector< string >::size_type i = 0; i < min( original_set.size( ), modified_set.size( ) ); i++ )
   {
      if( i == 0 )
         retval = prefix;
      else if( !separator.empty( ) )
         retval += separator;

      vector< string > secondary_original_set;
      vector< string > secondary_modified_set;

      if( !secondary_separator.empty( ) )
      {
         split_set( original_set[ i ], secondary_original_set, secondary_separator );
         split_set( modified_set[ i ], secondary_modified_set, secondary_separator );
      }

      vector< string >* p_secondary_set( &secondary_original_set );

      bool can_escape = true;
      bool was_escaped = false;

      if( xi.get_version( ) )
         can_escape = false;

      for( string::size_type j = 0; j < rhs.size( ); j++ )
      {
         if( xi.get_version( ) && ( rhs[ j ] == c_escape ) )
         {
            can_escape = true;
            continue;
         }

         if( was_escaped )
         {
            was_escaped = false;

            if( rhs[ j ] == 't' )
               retval += '\t';
            else if( rhs[ j ] == 'r' )
               retval += '\r';
            else if( rhs[ j ] == 'n' )
               retval += '\n';
            else if( rhs[ j ] == c_ordinal )
               retval += to_string( i );
            else if( rhs[ j ] == c_ordinal1 )
               retval += to_string( i + 1 );
            else if( rhs[ j ] == c_original )
               retval += original_set[ i ];
            else if( rhs[ j ] == c_modified )
               retval += modified_set[ i ];
            else if( rhs[ j ] == c_upper_original )
               retval += upper( original_set[ i ] );
            else if( rhs[ j ] == c_upper_modified )
               retval += upper( modified_set[ i ] );
            else if( rhs[ j ] == c_switch_secondary )
            {
               if( p_secondary_set == &secondary_original_set )
                  p_secondary_set = &secondary_modified_set;
               else
                  p_secondary_set = &secondary_original_set;
            }
            else if( rhs[ j ] >= '0' && rhs[ j ] <= '9' )
            {
               size_t offset = rhs[ j ] - '0';
               if( offset < p_secondary_set->size( ) )
                  retval += ( *p_secondary_set )[ offset ];
            }
            else
               retval += rhs[ j ];
         }
         else if( rhs[ j ] == c_template_escape )
         {
            was_escaped = can_escape;

            if( !was_escaped )
               retval += rhs[ j ];

            if( xi.get_version( ) )
               can_escape = false;
         }
         else
            retval += rhs[ j ];
      }
   }

   return retval;
}

string template_expression::get_label( ) const { return string( " " ); }

string template_expression::static_node_type( ) { return "template_expression"; }

class or_expression : public expression_base
{
   public:
   or_expression( expression_base* p_lhs, expression_base* p_rhs )
    : expression_base( p_lhs, p_rhs )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );
};

string or_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate or_expression" << endl;
#endif
   string lhs( up_lhs->evaluate( xi ) );

   if( !lhs.empty( ) )
      return c_true;

   if( !up_rhs->evaluate( xi ).empty( ) )
      return c_true;

   return c_false;
}

string or_expression::get_label( ) const { return string( c_op_or ); }

string or_expression::static_node_type( ) { return "or_expression"; }

class and_expression : public expression_base
{
   public:
   and_expression( expression_base* p_lhs, expression_base* p_rhs )
    : expression_base( p_lhs, p_rhs )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );
};

string and_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate and_expression" << endl;
#endif
   string lhs( up_lhs->evaluate( xi ) );

   if( lhs.empty( ) )
      return lhs;

   if( !up_rhs->evaluate( xi ).empty( ) )
      return c_true;

   return c_false;
}

string and_expression::get_label( ) const { return string( c_op_and ); }

string and_expression::static_node_type( ) { return "and_expression"; }

class not_expression : public expression_base
{
   public:
   not_expression( expression_base* p_lhs )
    : expression_base( p_lhs )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );
};

string not_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate not_expression" << endl;
#endif
   string lhs( up_lhs->evaluate( xi ) );

   if( lhs.empty( ) )
      return c_true;
   else
      return c_false;
}

string not_expression::get_label( ) const { return string( c_op_not ); }

string not_expression::static_node_type( ) { return "not_expression"; }

class test_expression : public expression_base
{
   public:
   test_expression( expression_base* p_lhs )
    : expression_base( p_lhs )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );
};

string test_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate test_expression" << endl;
#endif
   string lhs( up_lhs->evaluate( xi ) );

   if( xi.has_variable( lhs ) )
      return c_true;
   else
      return c_false;
}

string test_expression::get_label( ) const { return string( c_op_test ); }

string test_expression::static_node_type( ) { return "test_expression"; }

class union_expression : public expression_base
{
   public:
   union_expression( expression_base* p_lhs, expression_base* p_rhs )
    : expression_base( p_lhs, p_rhs )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );
};

string union_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate union_expression" << endl;
#endif
   string lhs( up_lhs->evaluate( xi ) );
   string rhs( up_rhs->evaluate( xi ) );

   return set_union( lhs, rhs );
}

string union_expression::get_label( ) const { return string( c_op_add ); }

string union_expression::static_node_type( ) { return "union_expression"; }

class append_expression : public expression_base
{
   public:
   append_expression( expression_base* p_lhs, expression_base* p_rhs )
    : expression_base( p_lhs, p_rhs )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );
};

string append_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate append_expression" << endl;
#endif
   string lhs( up_lhs->evaluate( xi ) );
   string rhs( up_rhs->evaluate( xi ) );

   return lhs + rhs;
}

string append_expression::get_label( ) const { return string( c_op_mul ); }

string append_expression::static_node_type( ) { return "append_expression"; }

class intersection_expression : public expression_base
{
   public:
   intersection_expression( expression_base* p_lhs, expression_base* p_rhs, bool is_exclusive )
    : expression_base( p_lhs, p_rhs ),
    is_exclusive( is_exclusive )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   bool is_exclusive;

   static string static_node_type( );
};

string intersection_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate intersection_expression" << endl;
#endif
   string lhs( up_lhs->evaluate( xi ) );
   string rhs( up_rhs->evaluate( xi ) );

   return set_intersection( lhs, rhs, is_exclusive );
}

string intersection_expression::get_label( ) const { return is_exclusive ? string( c_op_exclude ) : string( c_op_mod ); }

string intersection_expression::static_node_type( ) { return "intersection_expression"; }

class assign_append_expression : public expression_base
{
   public:
   assign_append_expression( expression_base* p_lhs, expression_base* p_rhs, bool is_append )
    : expression_base( p_lhs, p_rhs ),
    is_append( is_append )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );

   private:
   bool is_append;
};

string assign_append_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate assign_append_expression" << endl;
#endif
   string val;

   if( is_append )
      val = xi.get_variable( up_lhs->evaluate( xi ) );

   val += up_rhs->evaluate( xi );
   xi.set_variable( up_lhs->evaluate( xi ), val );

   return string( );
}

string assign_append_expression::get_label( ) const { return is_append ? string( c_op_append ) : string( c_op_assign ); }

string assign_append_expression::static_node_type( ) { return "assign_append_expression"; }

class eq_function_expression : public expression_base
{
   public:
   eq_function_expression( expression_base* p_lhs, expression_base* p_rhs )
    : expression_base( p_lhs, p_rhs )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );
};

string eq_function_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate eq_function_expression" << endl;
#endif
   string retval( c_false );

   string lhs( up_lhs->evaluate( xi ) );
   string rhs( up_rhs->evaluate( xi ) );

   if( lhs == rhs )
      retval = c_true;

   return retval;
}

string eq_function_expression::get_label( ) const
{
   string retval( c_function_prefix );

   retval += string( c_func_name_eq );

   return retval;
}

string eq_function_expression::static_node_type( ) { return "eq_function_expression"; }

class in_function_expression : public expression_base
{
   public:
   in_function_expression( expression_base* p_lhs, expression_base* p_rhs )
    : expression_base( p_lhs, p_rhs )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );
};

string in_function_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate in_function_expression" << endl;
#endif
   string retval;
   string lhs( up_lhs->evaluate( xi ) );
   string rhs( up_rhs->evaluate( xi ) );

   vector< string > lhss;
   vector< string > rhss;

   split_set( lhs, lhss );
   split_set( rhs, rhss );

   for( vector< string >::size_type i = 0; i < lhss.size( ); i++ )
   {
      if( find( rhss.begin( ), rhss.end( ), lhss[ i ] ) != rhss.end( ) )
         return c_true;
   }

   return c_false;
}

string in_function_expression::get_label( ) const
{
   string retval( c_function_prefix );

   retval += string( c_func_name_in );

   return retval;
}

string in_function_expression::static_node_type( ) { return "in_function_expression"; }

class count_function_expression : public expression_base
{
   public:
   count_function_expression( expression_base* p_lhs )
    : expression_base( p_lhs )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );
};

string count_function_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate count_function_expression" << endl;
#endif
   string lhs( up_lhs->evaluate( xi ) );

   vector< string > lhs_set;

   split_set( lhs, lhs_set );

   return to_string( lhs_set.size( ) );
}

string count_function_expression::get_label( ) const
{
   string retval( c_function_prefix );

   retval += string( c_func_name_count );

   return retval;
}

string count_function_expression::static_node_type( ) { return "count_function_expression"; }

class set_function_expression : public expression_base
{
   public:
   set_function_expression( const string& func_name, expression_base* p_lhs )
    : expression_base( p_lhs ),
    func_name( func_name )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );

   private:
   string func_name;
};

string set_function_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate set_function_expression" << endl;
#endif
   string retval( up_lhs->evaluate( xi ) );

   if( func_name == c_set_func_name_sort )
   {
      vector< string > items;

      split_set( retval, items );

      sort( items.begin( ), items.end( ) );

      retval = join_set( items );
   }
   else if( func_name == c_set_func_name_lower )
      to_lower( retval );
   else if( func_name == c_set_func_name_upper )
      to_upper( retval );
   else if( func_name == c_set_func_name_encode )
      retval = encode( retval );
   else
      throw runtime_error( "unexpected set function name '" + func_name + "'" );

   return retval;
}

string set_function_expression::get_label( ) const
{
   string retval( c_function_prefix );

   retval += func_name;

   return retval;
}

string set_function_expression::static_node_type( ) { return "set_function_expression"; }

class literal_expression : public expression_base
{
   public:
   literal_expression( const string& value )
    :
    value( value )
   {
   }

   string evaluate( xrep_info& ) { return value; }

   string get_label( ) const;

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );

   const string& get_literal_value( ) const { return value; }

   private:
   string value;
};

string literal_expression::get_label( ) const
{
   string retval;

   retval += '"';
   retval += value;
   retval += '"';

   return retval;
}

string literal_expression::static_node_type( ) { return "literal_expression"; }

class variable_expression : public expression_base
{
   public:
   variable_expression( bool is_ref, const string& identifier )
    :
    is_ref( is_ref ),
    identifier( identifier )
   {
   }

   string evaluate( xrep_info& xi );

   string get_label( ) const { return identifier; }

   string get_node_type( ) const { return static_node_type( ); }

   static string static_node_type( );

   private:
   bool is_ref;
   string identifier;
};

string variable_expression::evaluate( xrep_info& xi )
{
#ifdef DEBUG
cout << "evaluate variable_expression" << endl;
#endif
   if( is_ref )
      return identifier;

   return xi.get_variable( identifier );
}

string variable_expression::static_node_type( ) { return "variable_expression"; }

unique_ptr< expression_base > parse_or_expression( xrep_lexer& xl, bool is_opt );
unique_ptr< expression_base > parse_unary_expression( xrep_lexer& xl, bool is_opt );
unique_ptr< expression_base > parse_value_expression( xrep_lexer& xl, bool is_opt );
unique_ptr< expression_base > parse_variable_identifier( xrep_lexer& xl, bool is_ref, bool is_opt );
unique_ptr< expression_base > parse_set_union_expression( xrep_lexer& xl, bool is_opt );

unique_ptr< expression_base > parse_literal_text(
 xrep_lexer& xl, bool is_opt, bool create_even_if_empty = false, bool unescape_value = false )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_literal_text", is_opt );
#endif
   unique_ptr< expression_base > up_node;

   scoped_lexer_input_holder sli( xl );

   string literal_text;

   while( true )
   {
      string s( xl.read_next_token( ) );

      if( s.size( ) && s[ 0 ] == c_escape )
      {
         if( xl.get_version( ) )
         {
            if( ( s.length( ) > 1 ) && ( s[ 1 ] == c_template_escape ) )
            {
               literal_text += s;
               continue;
            }
         }

         if( s == c_escaped_special )
            literal_text += c_escape;
         else
         {
            xl.write_back_token( s );
            break;
         }
      }
      else
         literal_text += s;
   }

   if( literal_text.empty( ) )
   {
      if( !create_even_if_empty )
      {
         if( is_opt )
         {
            sli.restore( );

            return up_node;
         }
         else
            throw runtime_error( "missing expected literal text" );
      }
   }

   if( unescape_value )
      literal_text = unescaped( literal_text, c_special_characters );

#ifdef DEBUG
   cout << string( function_call_depth, ' ' ) << "(parse_literal_text) parsed: " << literal_text << endl;
#endif
   up_node.reset( new literal_expression( literal_text ) );

   return up_node;
}

unique_ptr< expression_base > parse_literal_expression( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_literal_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;

   scoped_lexer_input_holder sli( xl );

   if( !xl.read_next_token( c_quote ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return up_node;
      }
      else
         throw runtime_error( "missing expected token '" + string( c_quote ) + "' before literal expression" );
   }

   up_node = parse_literal_text( xl, true, true );

   if( !xl.read_next_token( c_quote ) )
      throw runtime_error( "missing expected token '" + string( c_quote ) + "' after literal expression" );

   return up_node;
}

unique_ptr< expression_base > parse_set_simple_expression( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_set_simple_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;

   scoped_lexer_input_holder sli( xl );

   up_node = parse_literal_expression( xl, true );

   if( up_node.get( ) )
      return up_node;
   else
   {
      up_node = parse_value_expression( xl, is_opt );

      if( !up_node.get( ) )
      {
         if( is_opt )
         {
            sli.restore( );

            return up_node;
         }
         else
            throw runtime_error( "missing expected literal expression or variable identifier in simple set expression" );
      }
   }

   return up_node;
}

unique_ptr< expression_base > parse_set_function( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_set_function", is_opt );
#endif
   unique_ptr< expression_base > up_node;
   unique_ptr< expression_base > up_lhs_expr;

   scoped_lexer_input_holder sli( xl );

   if( !xl.read_next_token( c_function_prefix ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return up_node;
      }
      else
         throw runtime_error( "missing expected token '" + string( c_function_prefix ) + "' before set function name" );
   }

   string func_name = xl.read_next_token( );

   if( func_name != c_set_func_name_sort && func_name != c_set_func_name_upper
    && func_name != c_set_func_name_lower && func_name != c_set_func_name_encode )
   {
      sli.restore( );

      return up_node;
   }

#ifdef DEBUG
   cout << string( function_call_depth, ' ' ) << "(parse_set_function) parsed: " << func_name << endl;
#endif

   if( !xl.read_next_token( c_left_parenthesis ) )
      throw runtime_error( "missing expected token '" + string( c_left_parenthesis ) + "' after set function name" );

   up_lhs_expr = parse_set_union_expression( xl, is_opt );

   if( !up_lhs_expr.get( ) )
      throw runtime_error( "missing expected set union expression" );

   if( !xl.read_next_token( c_right_parenthesis ) )
      throw runtime_error( "missing expected token '" + string( c_right_parenthesis ) + "' after set union expression" );

   up_node.reset( new set_function_expression( func_name, up_lhs_expr.release( ) ) );

   return up_node;
}

unique_ptr< expression_base > parse_unary_set_expression( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_unary_set_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;

   scoped_lexer_input_holder sli( xl );

   up_node = parse_set_function( xl, true );

   if( up_node.get( ) )
      return up_node;
   else
   {
      up_node = parse_set_simple_expression( xl, is_opt );
      if( !up_node.get( ) )
      {
         if( is_opt )
         {
            sli.restore( );

            return up_node;
         }
         else
            throw runtime_error( "missing expected set function or simple set expression" );
      }
   }

   return up_node;
}

unique_ptr< expression_base > parse_basic_set_expression( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_basic_set_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;

   scoped_lexer_input_holder sli( xl );

   if( xl.read_next_token( c_left_parenthesis ) )
   {
#ifdef DEBUG
      cout << string( function_call_depth, ' ' ) << "(parse_basic_set_expression) parsed: " << c_left_parenthesis << endl;
#endif
      up_node = parse_set_union_expression( xl, is_opt );
      if( !up_node.get( ) )
      {
         if( is_opt )
         {
            sli.restore( );

            return up_node;
         }
         else
            throw runtime_error( "missing expected or expression in basic expression" );
      }

      if( !xl.read_next_token( c_right_parenthesis ) )
      {
         if( is_opt )
         {
            sli.restore( );
            up_node.reset( );

            return up_node;
         }
         else
            throw runtime_error( "missing expected token '" + string( c_right_parenthesis ) + "' after or_expression" );
      }
#ifdef DEBUG
      cout << string( function_call_depth, ' ' ) << "(parse_basic_set_expression) parsed: " << c_right_parenthesis << endl;
#endif
   }
   else
   {
      up_node = parse_unary_set_expression( xl, is_opt );

      if( !up_node.get( ) )
      {
         if( is_opt )
         {
            sli.restore( );

            return up_node;
         }
         else
            throw runtime_error( "missing expected unary set expression in basic set expression" );
      }
   }

   return up_node;
}

unique_ptr< expression_base > parse_set_intersection_expression( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_set_intersection_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;
   unique_ptr< expression_base > up_lhs_expr;
   unique_ptr< expression_base > up_rhs_expr;

   scoped_lexer_input_holder sli( xl );

   up_lhs_expr = parse_basic_set_expression( xl, is_opt );

   if( !up_lhs_expr.get( ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return up_node;
      }
      else
         throw runtime_error( "missing expected basic set expression in set intersection expression" );
   }

   bool is_exclusive = false;

   if( xl.read_next_token( c_op_exclude ) )
      is_exclusive = true;

   if( is_exclusive || xl.read_next_token( c_op_mod ) )
   {
#ifdef DEBUG
      cout << string( function_call_depth, ' ' )
       << "(parse_set_intersection_expression) parsed: " << ( is_exclusive ? c_op_exclude : c_op_mod ) << endl;
#endif
      up_rhs_expr = parse_set_intersection_expression( xl, false );

      if( !up_rhs_expr.get( ) )
         throw runtime_error( "missing expected basic set expression in rhs of set intersection expression" );
   }

   if( up_lhs_expr.get( ) && up_rhs_expr.get( ) )
      up_node.reset( new intersection_expression( up_lhs_expr.release( ), up_rhs_expr.release( ), is_exclusive ) );
   else if( up_lhs_expr.get( ) )
      up_node = move( up_lhs_expr );

   return up_node;
}

unique_ptr< expression_base > parse_set_union_expression( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_set_union_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;
   unique_ptr< expression_base > up_lhs_expr;
   unique_ptr< expression_base > up_rhs_expr;

   scoped_lexer_input_holder sli( xl );

   up_lhs_expr = parse_set_intersection_expression( xl, is_opt );

   if( !up_lhs_expr.get( ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return up_node;
      }
      else
         throw runtime_error( "missing expected set intersection expression in set union expression" );
   }

   bool is_append = false;

   if( xl.read_next_token( c_op_add ) )
   {
#ifdef DEBUG
      cout << string( function_call_depth, ' ' ) << "(parse_set_union_expression) parsed: " << c_op_add << endl;
#endif
      up_rhs_expr = parse_set_union_expression( xl, false );

      if( !up_rhs_expr.get( ) )
         throw runtime_error( "missing expected set intersection expression in rhs of set union expression" );
   }
   else if( xl.read_next_token( c_op_mul ) )
   {
#ifdef DEBUG
      cout << string( function_call_depth, ' ' ) << "(parse_set_union_expression) parsed: " << c_op_mul << endl;
#endif
      is_append = true;

      up_rhs_expr = parse_set_union_expression( xl, false );

      if( !up_rhs_expr.get( ) )
         throw runtime_error( "missing expected set intersection expression in rhs of set union expression" );
   }

   if( up_lhs_expr.get( ) && up_rhs_expr.get( ) )
   {
      if( !is_append )
         up_node.reset( new union_expression( up_lhs_expr.release( ), up_rhs_expr.release( ) ) );
      else
         up_node.reset( new append_expression( up_lhs_expr.release( ), up_rhs_expr.release( ) ) );
   }
   else if( up_lhs_expr.get( ) )
      up_node = move( up_lhs_expr );

   return up_node;
}

unique_ptr< expression_base > parse_basic_expression( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_basic_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;

   scoped_lexer_input_holder sli( xl );

   up_node = parse_set_union_expression( xl, true );

   if( up_node.get( ) )
      return up_node;

   if( xl.read_next_token( c_left_parenthesis ) )
   {
#ifdef DEBUG
      cout << string( function_call_depth, ' ' ) << "(parse_basic_expression) parsed: " << c_left_parenthesis << endl;
#endif
      up_node = parse_or_expression( xl, false );

      if( !up_node.get( ) )
         throw runtime_error( "missing expected or expression in basic expression" );

      if( !xl.read_next_token( c_right_parenthesis ) )
         throw runtime_error( "missing expected token '" + string( c_right_parenthesis ) + "' after or_expression" );
#ifdef DEBUG
      cout << string( function_call_depth, ' ' ) << "(parse_basic_expression) parsed: " << c_right_parenthesis << endl;
#endif
   }

   if( !is_opt && !up_node.get( ) )
      throw runtime_error( "missing expected basic expression" );

   return up_node;
}

unique_ptr< expression_base > parse_function( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_function", is_opt );
#endif
   unique_ptr< expression_base > up_node;
   unique_ptr< expression_base > up_lhs_expr;
   unique_ptr< expression_base > up_rhs_expr;

   scoped_lexer_input_holder sli( xl );

   if( !xl.read_next_token( c_function_prefix ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return up_node;
      }
      else
         throw runtime_error( "missing expected token '" + string( c_function_prefix ) + "' before function name" );
   }

   string func_name = xl.read_next_token( );

#ifdef DEBUG
   cout << string( function_call_depth, ' ' ) << "(parse_function) parsed: " << func_name << endl;
#endif

   if( !xl.read_next_token( c_left_parenthesis ) )
      throw runtime_error( "missing expected token '" + string( c_left_parenthesis ) + "' after function name" );

   if( func_name == c_func_name_in )
   {
      up_lhs_expr = parse_unary_set_expression( xl, true );

      if( !up_lhs_expr.get( ) )
         throw runtime_error( "missing expected lhs unary set expression" );

      if( !xl.read_next_token( c_op_comma ) )
         throw runtime_error( "missing expected token '" + string( c_op_comma ) + "' after lhs unary set expression" );

#ifdef DEBUG
      cout << string( function_call_depth, ' ' ) << "(parse_function) parsed: " << c_op_comma << endl;
#endif

      up_rhs_expr = parse_unary_set_expression( xl, true );

      if( !up_rhs_expr.get( ) )
         throw runtime_error( "missing expected rhs unary set expression" );

      if( !xl.read_next_token( c_right_parenthesis ) )
         throw runtime_error( "missing expected token '" + string( c_right_parenthesis ) + "' after rhs unary set expression" );

      up_node.reset( new in_function_expression( up_lhs_expr.release( ), up_rhs_expr.release( ) ) );
   }
   else if( func_name == c_func_name_count )
   {
      up_lhs_expr = parse_unary_set_expression( xl, true );

      if( !up_lhs_expr.get( ) )
         throw runtime_error( "missing expected unary set expression" );

      if( !xl.read_next_token( c_right_parenthesis ) )
         throw runtime_error( "missing expected token '" + string( c_right_parenthesis ) + "' after unary set expression" );

      up_node.reset( new count_function_expression( up_lhs_expr.release( ) ) );
   }
   else if( func_name == c_func_name_eq )
   {
      up_lhs_expr = parse_unary_expression( xl, true );

      if( !up_lhs_expr.get( ) )
         throw runtime_error( "missing expected lhs unary expression" );

      if( !xl.read_next_token( c_op_comma ) )
         throw runtime_error( "missing expected token '" + string( c_op_comma ) + "' after lhs unary expression" );

#ifdef DEBUG
      cout << string( function_call_depth, ' ' ) << "(parse_function) parsed: " << c_op_comma << endl;
#endif

      up_rhs_expr = parse_unary_expression( xl, true );

      if( !up_rhs_expr.get( ) )
         throw runtime_error( "missing expected rhs unary expression" );

      if( !xl.read_next_token( c_right_parenthesis ) )
         throw runtime_error( "missing expected token '" + string( c_right_parenthesis ) + "' after rhs unary expression" );

      up_node.reset( new eq_function_expression( up_lhs_expr.release( ), up_rhs_expr.release( ) ) );
   }
   else
   {
      sli.restore( );

      return up_node;
   }

   return up_node;
}

unique_ptr< expression_base > parse_unary_expression( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_unary_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;

   scoped_lexer_input_holder sli( xl );

   if( xl.read_next_token( c_op_not ) )
   {
#ifdef DEBUG
      cout << string( function_call_depth, ' ' ) << "(parse_unary_expression) parsed: " << c_op_not << endl;
#endif
      up_node = parse_unary_expression( xl, false );

      if( !up_node.get( ) )
         throw runtime_error( "missing expected unary expression after not operator" );

      up_node.reset( new not_expression( up_node.release( ) ) );

      return up_node;
   }

   if( xl.read_next_token( c_op_test ) )
   {
#ifdef DEBUG
      cout << string( function_call_depth, ' ' ) << "(parse_unary_expression) parsed: " << c_op_test << endl;
#endif
      up_node = parse_variable_identifier( xl, true, is_opt );

      if( !up_node.get( ) )
         throw runtime_error( "missing expected variable identifier after test operator" );

      up_node.reset( new test_expression( up_node.release( ) ) );

      return up_node;
   }

   up_node = parse_function( xl, true );

   if( up_node.get( ) )
      return up_node;
   else
   {
      up_node = parse_basic_expression( xl, is_opt );

      if( !up_node.get( ) )
      {
         if( is_opt )
         {
            sli.restore( );

            return up_node;
         }
         else
            throw runtime_error( "missing expected basic expression in unary expression" );
      }
   }

   return up_node;
}

unique_ptr< expression_base > parse_and_expression( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_and_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;
   unique_ptr< expression_base > up_lhs_expr;
   unique_ptr< expression_base > up_rhs_expr;

   scoped_lexer_input_holder sli( xl );

   up_lhs_expr = parse_unary_expression( xl, is_opt );

   if( !up_lhs_expr.get( ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return up_node;
      }
      else
         throw runtime_error( "missing expected unary expression in and expression" );
   }

   if( xl.read_next_token( c_op_and ) )
   {
#ifdef DEBUG
      cout << string( function_call_depth, ' ' ) << "(parse_and_expression) parsed: " << c_op_and << endl;
#endif
      up_rhs_expr = parse_and_expression( xl, false );

      if( !up_rhs_expr.get( ) )
         throw runtime_error( "missing expected unary expression in rhs of and expression" );
   }

   if( up_lhs_expr.get( ) && up_rhs_expr.get( ) )
      up_node.reset( new and_expression( up_lhs_expr.release( ), up_rhs_expr.release( ) ) );
   else if( up_lhs_expr.get( ) )
      up_node = move( up_lhs_expr );

   return up_node;
}

unique_ptr< expression_base > parse_or_expression( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_or_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;
   unique_ptr< expression_base > up_lhs_expr;
   unique_ptr< expression_base > up_rhs_expr;

   scoped_lexer_input_holder sli( xl );

   up_lhs_expr = parse_and_expression( xl, is_opt );

   if( !up_lhs_expr.get( ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return up_node;
      }
      else
         throw runtime_error( "missing expected and expression in or expression" );
   }

   if( xl.read_next_token( c_op_or ) )
   {
#ifdef DEBUG
      cout << string( function_call_depth, ' ' ) << "(parse_or_expression) parsed: " << c_op_or << endl;
#endif
      up_rhs_expr = parse_or_expression( xl, false );

      if( !up_rhs_expr.get( ) )
         throw runtime_error( "missing expected and expression in rhs of or expression" );
   }

   if( up_lhs_expr.get( ) && up_rhs_expr.get( ) )
      up_node.reset( new or_expression( up_lhs_expr.release( ), up_rhs_expr.release( ) ) );
   else if( up_lhs_expr.get( ) )
      up_node = move( up_lhs_expr );

   return up_node;
}

unique_ptr< expression_base > parse_variable_identifier( xrep_lexer& xl, bool is_ref, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_variable_identifier", is_opt );
#endif
   unique_ptr< expression_base > up_node;

   scoped_lexer_input_holder sli( xl );

   if( !xl.read_next_token( c_variable_prefix ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return up_node;
      }
      else
         throw runtime_error( "missing expected '" + string( c_variable_prefix ) + "' for variable identifier" );
   }

   string identifier( xl.read_next_token( ) );

   for( string::size_type i = 0; i < identifier.size( ); i++ )
   {
      if( i == 0 )
      {
         if( !isalpha( static_cast< unsigned char >( identifier[ i ] ) ) )
         {
            xl.write_back_token( identifier );
            xl.set_error_width( identifier.size( ) );

            throw runtime_error( "invalid first character for variable identifier" );
         }
      }
      else
      {
         if( !isalnum( static_cast< unsigned char >( identifier[ i ] ) ) && identifier[ i ] != '_' )
         {
            xl.write_back_token( identifier );
            xl.set_error_width( identifier.size( ) );

            throw runtime_error( "invalid character '" + string( 1, identifier[ i ] ) + "' found in variable identifier" );
         }
      }
   }
#ifdef DEBUG
   cout << string( function_call_depth, ' ' ) << "(parse_variable_identifier) parsed: " << identifier << endl;
#endif

   up_node.reset( new variable_expression( is_ref, identifier ) );

   return up_node;
}

unique_ptr< expression_base > parse_assign_append_expression( xrep_lexer& xl, bool is_opt, bool& is_append )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_assign_append_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;

   scoped_lexer_input_holder sli( xl );

   up_node = parse_variable_identifier( xl, true, is_opt );

   if( !up_node.get( ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return up_node;
      }
      else
         throw runtime_error( "missing expected variable identifier in assignment expression" );
   }

   if( !xl.read_next_token( c_op_assign ) && !xl.read_next_token( c_op_append ) )
   {
      if( is_opt )
      {
         sli.restore( );
         up_node.reset( );

         return up_node;
      }
      else
         throw runtime_error( "missing expected token '" + string( c_op_assign )
          + "' or '" + string( c_op_exclude ) + "' after variable identifier" );
   }

   if( xl.get_last_read_token( ) == string( c_op_append ) )
      is_append = true;
   else
      is_append = false;

#ifdef DEBUG
   cout << string( function_call_depth, ' ' ) << "(parse_assign_append_expression) parsed: " << xl.get_last_read_token( ) << endl;
#endif

   return up_node;
}

unique_ptr< expression_base > parse_variable_or_literal_text( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_variable_or_literal_text", is_opt );
#endif
   unique_ptr< expression_base > up_node;

   scoped_lexer_input_holder sli( xl );

   up_node = parse_variable_identifier( xl, false, true );

   if( !up_node.get( ) )
      up_node = parse_literal_text( xl, true, false, true );

   if( !up_node.get( ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return up_node;
      }
      else
         throw runtime_error( "missing expected variable or literal text" );
   }

   return up_node;
}

unique_ptr< expression_base > parse_replacement_expression( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_replacement_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;
   unique_ptr< expression_base > up_find_node;
   unique_ptr< expression_base > up_repl_node;
   unique_ptr< expression_base > up_temp_node;

   scoped_lexer_input_holder sli( xl );

   bool is_multi = false;

   if( !xl.read_next_token( c_find_repl ) && !xl.read_next_token( c_op_colon ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return up_node;
      }
      else
         throw runtime_error( "missing expected token '"
          + string( c_find_repl ) + "' or '" + string( c_op_colon ) + "' for find part of basic replacement expression" );
   }

   if( xl.get_last_read_token( ) == string( c_find_repl ) )
      is_multi = true;

   up_find_node = parse_variable_or_literal_text( xl, false );

   if( !up_find_node.get( ) )
      throw runtime_error( "missing expected literal or variable for find part of basic replacement expression" );

   if( ( is_multi && !xl.read_next_token( c_find_repl ) ) || ( !is_multi && !xl.read_next_token( c_op_assign ) ) )
      throw runtime_error( "missing expected token '"
       + string( c_find_repl ) + "' or '" + string( c_op_assign ) + "' for replacement part of basic replacement expression" );

   string replacement_text;
   string replacement_variable;

   up_repl_node = parse_variable_or_literal_text( xl, true );

   if( up_repl_node.get( ) )
   {
      literal_expression* p_lit_node = dynamic_cast< literal_expression* >( up_repl_node.get( ) );
      variable_expression* p_var_node = dynamic_cast< variable_expression* >( up_repl_node.get( ) );

      if( !p_lit_node && !p_var_node )
         throw runtime_error( "unexpected non-literal or variable node for replacement part of basic replacement expression" );

      if( p_lit_node )
         replacement_text = p_lit_node->get_literal_value( );
      else
         replacement_variable = p_var_node->get_label( );
   }

   up_temp_node = parse_replacement_expression( xl, true );

   up_node.reset( new replacement_expression( is_multi,
    replacement_text, replacement_variable, up_find_node.release( ), up_temp_node.release( ) ) );

   return up_node;
}

unique_ptr< expression_base > parse_padding_expression( unique_ptr< expression_base >& up_node, xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_padding_expression", is_opt );
#endif
   scoped_lexer_input_holder sli( xl );

   if( !xl.read_next_token( c_op_semicolon ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return move( up_node );
      }
      else
         throw runtime_error( "missing expected token '" + string( c_op_semicolon ) + "' for padding expression" );
   }

   int num;
   char fill;

   string padding_info = xl.read_next_token( );

   if( padding_info.empty( ) || !split_padding_info( padding_info, num, fill, false ) )
   {
      xl.write_back_token( padding_info );

      if( !padding_info.empty( ) && padding_info[ 0 ] != c_escape )
      {
         xl.set_error_width( padding_info.size( ) );

         throw runtime_error( "invalid padding info provided for padding expression" );
      }
      else
         throw runtime_error( "missing expected padding info for padding expression" );
   }

   up_node.reset( new padding_expression( up_node.release( ), padding_info ) );

   return move( up_node );
}

unique_ptr< expression_base > parse_include_expression( xrep_lexer& xl, bool is_opt );

unique_ptr< expression_base > parse_value_expression( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_value_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;
   unique_ptr< expression_base > up_lhs_expr;
   unique_ptr< expression_base > up_rhs_expr;

   scoped_lexer_input_holder sli( xl );

   up_lhs_expr = parse_variable_identifier( xl, false, is_opt );

   if( !up_lhs_expr.get( ) )
      up_lhs_expr = parse_include_expression( xl, is_opt );

   if( !up_lhs_expr.get( ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return move( up_node );
      }
      else
         throw runtime_error( "missing expected variable identifier in value expression" );
   }

   up_rhs_expr = parse_replacement_expression( xl, true );

   if( up_lhs_expr.get( ) && up_rhs_expr.get( ) )
      up_node.reset( new value_expression( up_lhs_expr.release( ), up_rhs_expr.release( ) ) );
   else if( up_lhs_expr.get( ) )
      up_node.reset( new value_expression( up_lhs_expr.release( ) ) );

   return move( up_node );
}

unique_ptr< expression_base > parse_template_value_expression( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_template_value_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;
   unique_ptr< expression_base > up_lhs_node;
   unique_ptr< expression_base > up_rhs_node;
   unique_ptr< expression_base > up_pad_node;

   scoped_lexer_input_holder sli( xl );

   up_lhs_node = parse_value_expression( xl, is_opt );

   if( !up_lhs_node.get( ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return move( up_node );
      }
      else
         throw runtime_error( "missing expected value expression in template value expression" );
   }

   if( xl.read_next_token( c_op_not ) )
   {
      value_expression* p_val_expr( dynamic_cast< value_expression* >( up_lhs_node.get( ) ) );

      if( !p_val_expr )
         throw runtime_error( "unexpected dynamic cast< value_expression* > failure" );

      p_val_expr->set_do_not_split_into_set_name( );
   }
   else
      up_lhs_node = parse_padding_expression( up_lhs_node, xl, true );

   if( xl.read_next_token( c_op_mul ) )
   {
      value_expression* p_val_expr( dynamic_cast< value_expression* >( up_lhs_node.get( ) ) );

      if( !p_val_expr )
         throw runtime_error( "unexpected dynamic cast< value_expression* > failure" );

      xrep_info dummy_info;
      p_val_expr->set_primary_split_value( parse_literal_text( xl, false )->evaluate( dummy_info ) );
   }

   if( xl.read_next_token( c_op_mod ) )
   {
      value_expression* p_val_expr( dynamic_cast< value_expression* >( up_lhs_node.get( ) ) );

      if( !p_val_expr )
         throw runtime_error( "unexpected dynamic cast< value_expression* > failure" );

      xrep_info dummy_info;
      p_val_expr->set_secondary_split_value( parse_literal_text( xl, false )->evaluate( dummy_info ) );
   }

   up_rhs_node = parse_literal_expression( xl, true );

   if( up_rhs_node.get( ) )
      up_rhs_node.reset( new prefix_expression( up_rhs_node.release( ) ) );
   else
   {
      if( xl.peek_next_token( ) == string( c_op_comma ) )
         xl.read_next_token( c_op_comma );
   }

   if( up_lhs_node.get( ) && up_rhs_node.get( ) )
      up_node.reset( new combined_expression( up_rhs_node.release( ), up_lhs_node.release( ) ) );
   else
      up_node = move( up_lhs_node );

   return move( up_node );
}

unique_ptr< expression_base > parse_item_expression( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_item_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;

   scoped_lexer_input_holder sli( xl );

   up_node = parse_literal_text( xl, is_opt );

   if( !up_node.get( ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return move( up_node );
      }
      else
         throw runtime_error( "missing expected literal text in item expression" );
   }

   return move( up_node );
}

unique_ptr< expression_base > parse_item_separator_expression( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_item_separator_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;

   scoped_lexer_input_holder sli( xl );

   if( !xl.read_next_token( c_op_add ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return up_node;
      }
      else
         throw runtime_error( "missing expected token '" + string( c_op_add ) + "' for item separator expression" );
   }

   up_node = parse_literal_text( xl, true );

   return move( up_node );
}

unique_ptr< expression_base > parse_template_item_expression( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_template_item_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;
   unique_ptr< expression_base > up_lhs_node;
   unique_ptr< expression_base > up_rhs_node;

   scoped_lexer_input_holder sli( xl );

   up_lhs_node = parse_item_expression( xl, is_opt );

   if( !up_lhs_node.get( ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return move( up_node );
      }
      else
         throw runtime_error( "unexpected missing item expression in template item expression" );
   }

   string separator_text;

   up_rhs_node = parse_item_separator_expression( xl, true );

   if( up_rhs_node.get( ) )
   {
      literal_expression* p_lit_node = dynamic_cast< literal_expression* >( up_rhs_node.get( ) );

      if( !p_lit_node )
         throw runtime_error( "unexpected non-literal text node for item expression separator" );

      separator_text = p_lit_node->get_literal_value( );
   }

   up_node.reset( new item_expression( separator_text, up_lhs_node.release( ) ) );

   return move( up_node );
}

unique_ptr< expression_base > parse_template_expression( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_template_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;
   unique_ptr< expression_base > up_lhs_node;
   unique_ptr< expression_base > up_rhs_node;

   scoped_lexer_input_holder sli( xl );

   if( !xl.read_next_token( c_left_bracket ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return move( up_node );
      }
      else
         throw runtime_error( "missing expected token '" + string( c_left_bracket ) + "' in template expression" );
   }

   up_lhs_node = parse_template_value_expression( xl, false );

   if( !up_lhs_node.get( ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return move( up_node );
      }
      else
         throw runtime_error( "missing expected template value expression in template expression" );
   }

   up_rhs_node = parse_template_item_expression( xl, false );

   if( !up_rhs_node.get( ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return move( up_node );
      }
      else
         throw runtime_error( "missing expected template item expression in template expression" );
   }

   up_node.reset( new template_expression( up_lhs_node.release( ), up_rhs_node.release( ) ) );

   if( !xl.read_next_token( c_right_bracket ) )
   {
      if( is_opt )
      {
         sli.restore( );
         up_node.reset( );

         return move( up_node );
      }
      else
         throw runtime_error( "missing expected token '" + string( c_right_bracket ) + "' in template expression" );
   }

   return move( up_node );
}

unique_ptr< expression_base > parse_include_expression( xrep_lexer& xl, bool is_opt )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_include_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;

   scoped_lexer_input_holder sli( xl );

   if( !xl.read_next_token( c_include ) )
   {
      if( is_opt )
      {
         sli.restore( );

         return move( up_node );
      }
      else
         throw runtime_error( "missing expected token '" + string( c_include ) + "' for include expression" );
   }

   up_node = parse_literal_text( xl, false );
   up_node.reset( new include_expression( up_node.release( ) ) );

   return move( up_node );
}

unique_ptr< expression_base > parse_xrep_expression( xrep_lexer& xl, bool is_opt, bool need_braces = true )
{
#ifdef DEBUG
   function_call_debug fcd( "parse_xrep_expression", is_opt );
#endif
   unique_ptr< expression_base > up_node;
   unique_ptr< expression_base > up_lhs_expr;
   unique_ptr< expression_base > up_rhs_expr;
   unique_ptr< expression_base > up_temp_expr;

   scoped_lexer_input_holder sli( xl );

   if( need_braces )
   {
      if( !xl.read_next_token( c_left_brace ) )
      {
         if( is_opt )
         {
            sli.restore( );

            return move( up_node );
         }
         else
            throw runtime_error( "missing expected token '" + string( c_left_brace ) + "' for xrep expression" );
      }

#ifdef DEBUG
      cout << string( function_call_depth, ' ' ) << "(parse_xrep_expression) parsed: " << c_left_brace << endl;
#endif
   }

   if( xl.read_next_token( c_op_semicolon ) )
      parse_literal_text( xl, false );

   bool had_assign = false;
   bool had_include = false;

   bool is_append;

   up_lhs_expr = parse_assign_append_expression( xl, true, is_append );

   if( up_lhs_expr.get( ) )
      had_assign = true;

   bool is_conditional = false;

   up_temp_expr = parse_or_expression( xl, !had_assign );

   if( up_temp_expr.get( ) )
      is_conditional = true;

   // NOTE: The assignment RHS is an "or_expression" from which a "basic_expression" can be reached.
   // It is expected that a "basic_expression" would be the typical usage, however, it could be that
   // assigning a boolean value is required so the "or_expression" is used instead.
   if( up_lhs_expr.get( ) && up_temp_expr.get( ) )
      up_lhs_expr.reset( new assign_append_expression( up_lhs_expr.release( ), up_temp_expr.release( ), is_append ) );
   else if( up_temp_expr.get( ) )
      up_lhs_expr = move( up_temp_expr );

   if( !had_assign )
   {
      up_rhs_expr = parse_include_expression( xl, true );

      if( up_rhs_expr.get( ) )
         had_include = true;
   }

   if( !had_assign && !had_include && up_lhs_expr.get( ) )
   {
      up_temp_expr = parse_assign_append_expression( xl, true, is_append );

      if( up_temp_expr.get( ) )
      {
         had_assign = true;

         up_rhs_expr.reset( new assign_append_expression(
          up_temp_expr.release( ), parse_or_expression( xl, false ).release( ), is_append ) );
      }
   }

   if( !had_assign && !had_include )
   {
      up_rhs_expr = parse_template_expression( xl, true );

      up_temp_expr = parse_literal_text( xl, true, false, true );

      if( up_rhs_expr.get( ) && up_temp_expr.get( ) )
         up_rhs_expr.reset( new combined_expression( up_rhs_expr.release( ), up_temp_expr.release( ) ) );
      else if( !up_rhs_expr.get( ) && up_temp_expr.get( ) )
         up_rhs_expr = move( up_temp_expr );
   }

   if( !had_assign && !had_include && !up_rhs_expr.get( ) )
      up_rhs_expr = parse_basic_expression( xl, true );

   unique_ptr< expression_base > up_else_expr;

   if( is_conditional && up_rhs_expr.get( ) )
   {
      if( xl.read_next_token( c_op_comma ) )
         up_else_expr = parse_xrep_expression( xl, false, false );
   }

   if( up_rhs_expr.get( ) )
   {
      bool new_condition = true;

      if( !up_else_expr.get( ) )
         new_condition = false;

      up_rhs_expr.reset( new okay_expression( up_rhs_expr.release( ), up_else_expr.release( ) ) );

      if( !up_lhs_expr.get( ) || ( !need_braces && !new_condition ) )
         up_rhs_expr->set_okay( );
   }

   if( need_braces )
   {
      if( !xl.read_next_token( c_right_brace ) )
      {
         if( is_opt )
         {
            sli.restore( );

            return move( up_node );
         }
         else
            throw runtime_error( "missing expected token '" + string( c_right_brace ) + "' for xrep expression" );
      }

#ifdef DEBUG
      cout << string( function_call_depth, ' ' ) << "(parse_xrep_expression) parsed: " << c_right_brace << endl;
#endif
   }

   if( up_lhs_expr.get( ) && up_rhs_expr.get( ) )
      up_node.reset( new xrep_expression( up_lhs_expr.release( ), up_rhs_expr.release( ) ) );
   else if( up_lhs_expr.get( ) )
      up_node = move( up_lhs_expr );
   else if( up_rhs_expr.get( ) )
      up_node = move( up_rhs_expr );

   return move( up_node );
}

unique_ptr< expression_base > parse_expression( const string& input, int line_number, size_t version )
{
   xrep_lexer xl( input, version );

   try
   {
      unique_ptr< expression_base > up_node( parse_xrep_expression( xl, false ) );

      return move( up_node );
   }
   catch( exception& x )
   {
      ostringstream osstr;

      osstr << "line #" << line_number << ' ' << x.what( );

      string xx( osstr.str( ) );

      xx += '\n';
      xx += xl.get_input_error( "input: " );

      throw runtime_error( xx );
   }
}

string evaluate_expression( xrep_info& xi, expression_base* p_node )
{
   string retval;

   if( p_node )
      retval = p_node->evaluate( xi );

   return retval;
}

string process_expression( const string& input, xrep_info& xi, int line_number )
{
   string retval;
   unique_ptr< expression_base > up_node( parse_expression( input, line_number, xi.get_version( ) ) );
#ifdef DEBUG
   dump_expression_nodes( up_node.get( ), cout );
#endif
   try
   {
      retval = evaluate_expression( xi, up_node.get( ) );
   }
   catch( exception& x )
   {
      if( g_is_include_exception )
      {
         g_is_include_exception = false;

         throw;
      }
      else
      {
         ostringstream osstr;

         osstr << "line #" << line_number << ' ' << x.what( );

         string xx( osstr.str( ) );

         xx += "\ninput: ";
         xx += input;

         throw runtime_error( xx );
      }
   }

#ifdef DEBUG
   up_node.reset( );
   cout << "expression_base::instance_count = " << expression_base::instance_count << endl;
#endif

   return retval;
}

void pre_process_expr( string& expr )
{
   int expr_level = 0;
   bool was_escape = false;

   for( string::size_type i = 0; i < expr.size( ); i++ )
   {
      if( was_escape )
      {
         was_escape = false;
         if( expr[ i ] == c_left_brace[ 1 ] )
         {
            if( ++expr_level > 1 )
               expr[ i - 1 ] = c_hidden_escape;
         }
         else if( expr_level && expr[ i ] == c_right_brace[ 1 ] )
         {
            if( !--expr_level )
               expr[ i - 1 ] = c_escape;
         }
      }
      else if( expr[ i ] == c_escape )
      {
         was_escape = true;

         if( expr_level > 1 )
            expr[ i ] = c_hidden_escape;
      }
   }
}

void post_process_result( string& result )
{
   for( string::size_type i = 0; i < result.size( ); i++ )
   {
      if( result[ i ] == c_hidden_escape )
         result[ i ] = c_escape;
   }
}

bool process_next( const string& line, string& result,
 xrep_info& xi, int line_number, string& last, int& nested_expressions, bool& include_appended )
{
   bool was_escape = false;
   bool has_processed_expression = false;

   string::size_type end = 0;
   string::size_type start = string::npos;

   result.erase( );

   if( !last.empty( ) )
      start = 0;

   include_appended = false;

   for( string::size_type i = 0; i < line.size( ); i++ )
   {
      if( was_escape )
      {
         was_escape = false;

         if( line[ i ] == c_left_brace[ 1 ] )
         {
            if( start == string::npos )
               start = end = i - 1;
            else
               ++nested_expressions;
         }
         else if( start != string::npos && line[ i ] == c_right_brace[ 1 ] )
         {
            if( nested_expressions )
               --nested_expressions;
            else
            {
               string expr( last );

               expr += line.substr( start, i - start + 1 );
#ifdef DEBUG
               cout << "[[[ expr:\n" << expr << "\n]]]" << endl;
#endif

               pre_process_expr( expr );
               result += process_expression( expr, xi, line_number );
               post_process_result( result );

               // NOTE: If an include has been processed at the end of the input
               // line then signal this to the caller (refer to "process_input").
               // This also applies if include content is followed by a single LF.
               if( xi.had_handled_include( ) && ( i == line.size( ) - 1
                || ( i == line.size( ) - 2 && line[ line.size( ) - 1 ] == '\n' ) ) )
                  include_appended = true;

               end = i + 1;
               last.erase( );
               start = string::npos;
               has_processed_expression = true;

               if( end < line.size( ) )
                  result += line.substr( end );
               break;
            }
         }
         else if( start == string::npos )
            result += line[ i ];
      }
      else
      {
         if( line[ i ] == c_escape )
            was_escape = true;
         else if( start == string::npos )
            result += line[ i ];
      }
   }

   if( start == string::npos )
      last.erase( );
   else if( end < line.size( ) )
      last += line.substr( end );

   return has_processed_expression || !result.empty( );
}

bool had_continuation( xrep_info& xi, string& line )
{
   bool retval = false;

   if( !xi.get_version( ) )
   {
      string::size_type pos = line.find_last_not_of( '\\' );

      if( ( pos != line.size( ) - 1 ) && ( ( line.size( ) - pos - 1 ) % 2 ) )
      {
         line.erase( line.size( ) - 1 );

         retval = true;
      }
   }
   else
   {
      string::size_type pos = line.find(
       c_ver_1_cont, line.length( ) - g_ver_1_cont_len );

      if( pos != string::npos )
      {
         size_t num_escapes = 1;

         while( true )
         {
            if( !pos )
               break;

            if( line[ --pos ] != c_escape )
               break;

            ++num_escapes;
         }

         if( num_escapes % 2 )
         {
            line.erase( line.size( ) - g_ver_1_cont_len );

            retval = true;
         }
      }
   }

   return retval;
}

void process_input( istream& is, xrep_info& xi, ostream& os, bool append_final_lf )
{
   string cont, last, next, result;

   int line_number = 0;
   int num_continuations = 0;
   int nested_expressions = 0;

   bool had_output = false;
   bool had_partial = false;
   bool has_expression = false;
   bool include_appended = false;

   while( has_expression || getline( is, next ) )
   {
      if( !line_number && !num_continuations )
      {
         size_t pos = next.find( c_xrep_using_version_1 );

         if( pos == 0 )
         {
            next.erase( 0, strlen( c_xrep_using_version_1 ) );

            xi.set_version( 1 );

            xi.set_variable( c_var_name_version, c_using_version_1 );
         }
      }

      if( !cont.empty( ) )
      {
         next = cont + next;

         cont.erase( );
      }

      if( !has_expression )
      {
         xi.set_handled_include( false );

         remove_trailing_cr_from_text_file_line( next );

         if( !xi.get_version( ) )
         {
            // NOTE: A character sequence in the form \=n (where n is a number between 1 and 9) will be replaced
            // with the number of \ characters for the specified level (n) of nesting (e.g. \=5 ==> \\\\\\\\\\).
            size_t pos = 0;

            while( ( pos = find_escape_level_sequence( next, pos ) ) != string::npos )
               replace_escape_level_sequence( next, pos );
         }
      }

      if( xi.get_version( ) && had_continuation( xi, next ) )
      {
         cont = next;

         next.erase( );

         ++num_continuations;

         continue;
      }

      if( next.empty( ) )
      {
         assert( has_expression == false );

         if( last.empty( ) )
         {
            if( !g_exec_system )
               os << '\n';

            ++line_number;
         }
         else
         {
            last += '\n';
            ++num_continuations;
         }

         continue;
      }

      bool had_expression = false;

      if( !has_expression )
      {
         if( last.empty( ) )
            ++line_number;
         else
            ++num_continuations;
      }
      else
      {
         had_expression = true;
         has_expression = false;
      }

      bool had_result( process_next( next, result, xi, line_number, last, nested_expressions, include_appended ) );

      if( !last.empty( ) )
      {
         if( !xi.get_version( ) && had_continuation( xi, last ) )
            ;
         else if( !had_expression )
            last += '\n';
      }

      if( had_result )
      {
         line_number += num_continuations;

         num_continuations = 0;

         if( result.find( c_left_brace ) != string::npos )
         {
            next = result;
            next += last;

            last.erase( );
            has_expression = true;
         }
         else
         {
            if( !result.empty( ) )
            {
               if( had_output && !g_exec_system )
                  os << '\n';

               had_output = true;

               if( result != c_true )
               {
                  if( !g_exec_system )
                     os << result;
                  else
                  {
                     int rc = system( result.c_str( ) );
                     ( void )rc;
                  }
               }

               // NOTE: If there is a partial expression outstanding or if the result consists
               // only of content that was injected by an "include" expression then prevent the
               // next result from being preceeded by a LF.
               if( !last.empty( ) || include_appended )
               {
                  had_output = false;

                  if( !last.empty( ) )
                     had_partial = true;
               }
            }
            else if( had_partial )
            {
               had_partial = false;

               // NOTE: If we had issued output prior to the partial expression then the next output
               // will be preceeded by a LF unless the empty output was provided using an "include".
               if( last.empty( ) && !include_appended )
                  had_output = true;
            }
         }
      }
   }

   if( !last.empty( ) )
      throw runtime_error( "unexpected end of input found whilst processing expression started at line #" + to_string( line_number ) );

   if( !is.eof( ) )
      throw runtime_error( "unexpected error occurred whilst reading from input stream" );

   if( append_final_lf && !g_exec_system )
      os << '\n';
}

int main( int argc, char* argv[ ] )
{
   int rc = 0;
   string input_filename;

   g_ver_1_cont_len = strlen( c_ver_1_cont );

   try
   {
      string next;
      xrep_info xi;

      add_date_variables( xi );

      xi.set_variable( c_var_name_uuid, uuid( ).as_string( ) );

      xi.set_variable( c_var_name_linux, c_true );
      xi.set_variable( c_var_name_version, c_minimum_version );

      for( int i = 1; i < argc; i++ )
      {
         string arg( argv[ i ] );

         if( i == 1 && !arg.empty( ) )
         {
            if( arg == string( "?" ) || arg == string( "-?" ) || arg == string( "/?" ) )
            {
               cout << "xrep v0.1v\n";
               cout << "Usage: xrep [-x] [@<filename>] [var1=<value> [var2=<value> [...]]]\n\n";

               cout << "Notes: If the @<filename> is not provided then input is read from std::cin.\n";
               cout << "       If the -x option is used then each line is executed as a system command.\n";
               cout << "       Each <value> can also be expressed as @<filename> (useful for large values).\n";
               return 0;
            }
         }

         if( input_filename.empty( ) && !arg.empty( ) && arg[ 0 ] == '@' )
         {
            input_filename = arg.substr( 1 );
            continue;
         }

         if( !g_exec_system && arg == "-x" )
         {
            g_exec_system = true;
            continue;
         }

         string::size_type pos = arg.find( '=' );

         if( pos == string::npos )
            throw runtime_error( "invalid format for argument '" + arg + "'" );

         string value( arg.substr( pos + 1 ) );

         arg.erase( pos );

         if( !value.empty( ) )
         {
            if( value[ 0 ] == '@' )
            {
               ifstream inpf( value.substr( 1 ).c_str( ) );
               if( !inpf )
                  throw runtime_error( "unable to open file '" + value.substr( 1 ) + "' for input" );

               value.erase( );

               while( getline( inpf, next ) )
               {
                  remove_trailing_cr_from_text_file_line( next );

                  if( next.empty( ) )
                     continue;

                  if( !value.empty( ) )
                     value += ' ';
                  value += next;
               }
            }
            else
               unescape( value, c_special_characters );
         }

         xi.set_variable( arg, value );
      }

      istream* p_input( &cin );

      unique_ptr< ifstream > up_fstream;

      if( !input_filename.empty( ) )
      {
         up_fstream.reset( new ifstream( input_filename.c_str( ) ) );

         if( !*up_fstream )
            throw runtime_error( "unable to open file '" + input_filename + "' for input" );

         p_input = up_fstream.get( );
      }

      process_input( *p_input, xi, cout, true );
   }
   catch( exception& x )
   {
      rc = 1;
      stringstream ss;
      ss << x.what( );

      // NOTE: Switching between writing to and reading from a stream requires a seek.
      ss.seekg( 0 );

      string next, first;

      while( getline( ss, next ) )
      {
         post_process_result( next );

         if( first.empty( ) )
            first = next;
         else
            cerr << next << '\n';
      }

      cerr << "error: (";

      if( input_filename.empty( ) )
         cerr << "std::cin";
      else
         cerr << input_filename;

      cerr << ") " << first << endl;
   }
   catch( ... )
   {
      rc = 2;
      cerr << "error: unexpected exception was caught" << endl;
   }

   return rc;
}
