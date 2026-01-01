// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
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
#  include <map>
#  include <stack>
#  include <sstream>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#include "command_parser.h"

#include "regex.h"
#include "utilities.h"

//#define DEBUG

using namespace std;

namespace
{

const char c_escape = '\\';

const char c_token_begin = '<';
const char c_token_finish = '>';
const char c_opt_branch_begin = '[';
const char c_opt_branch_finish = ']';
const char c_match_branch_begin = '{';
const char c_match_branch_finish = '}';
const char c_alternative_separator = '|';

const char c_sub_expression_divider = '/';

const char* const c_expr_type_opt = "opt";
const char* const c_expr_type_pat = "pat";
const char* const c_expr_type_val = "val";
const char* const c_expr_type_oval = "oval";
const char* const c_expr_type_list = "list";
const char* const c_expr_type_olist = "olist";

const char c_exp_type_opt_value_separator = '#';
const char c_exp_type_list_default_separator = ',';
const char c_exp_type_list_default_terminator = ' ';
const char c_exp_type_prefix_default_separator = '=';

struct node
{
   node( )
    :
    id( 0 ),
    is_alt( false ),
    is_okay( false ),
    p_next_node( 0 ),
    p_prev_node( 0 ),
    p_match_node( 0 )
   {
      separator = c_exp_type_list_default_separator;
      terminator = c_exp_type_list_default_terminator;
   }

   size_t id;
   bool is_alt;
   bool is_okay;

   string type;
   string prefix;
   string parameter;
   string default_val;
   string description;

   char separator;
   char terminator;

   string expression;

   node* p_next_node;
   node* p_prev_node;

   node* p_match_node;

   vector< node* > opt_branch_nodes;

   private:
   node( const node& );
   node& operator =( const node& );
};

string get_next_sub_expression( string& expr, char divider )
{
   string sub_expr;
   string::size_type i;

   bool escaped = false;

   for( i = 0; i < expr.size( ); i++ )
   {
      if( escaped )
         escaped = false;
      else if( expr[ i ] == '\\' )
         escaped = true;
      else if( expr[ i ] == divider )
         break;

      if( !escaped )
         sub_expr += expr[ i ];
   }

   if( i == expr.size( ) )
      expr.erase( );
   else
      expr.erase( 0, i + 1 );

   return sub_expr;
}

bool all_list_values_are_not_empty( const string& input, char separator )
{
   bool retval = true;

   if( input.empty( ) )
      retval = false;
   else
   {
      bool was_escape = false;
      bool last_was_escape = false;
      for( string::size_type i = 0; i < input.size( ); i++ )
      {
         if( was_escape )
         {
            was_escape = false;
            last_was_escape = true;
         }
         else
         {
            if( input[ i ] == c_escape )
               was_escape = true;
            else if( input[ i ] == separator
             && ( i == 0 || i == input.size( ) - 1 || ( !last_was_escape && input[ i - 1 ] == separator ) ) )
            {
               retval = false;
               break;
            }

            last_was_escape = false;
         }
      }
   }

   return retval;
}

}

class command_parser::impl
{
   public:
   impl( )
    :
    p_node( 0 ),
    is_invalid( false ),
    end_of_input( false )
   {
      num_nodes = 0;
      error_pos = 0;
   }

   ~impl( )
   {
      clear( );
   }

   void clear( );

   void parse_syntax( const char* p_input );

   bool parse_command( const vector< string >& arguments, map< string, string >& parameters );

   bool okay( ) { return !is_invalid; }

   void dump_nodes( ostream& ostr ) const;

   string get_usage( ) const;
   void output_usage( ostream& ostr ) const;
   void output_syntax( ostream& ostr ) const;

   void get_parameter_names( vector< string >& parameters, vector< string >* p_default_vals ) const;

   size_t get_error_pos( ) const { return error_pos; }

   size_t get_num_nodes( ) const { return num_nodes; }

   private:
   void do_clear( node* p_node );
   void do_parse_syntax( node* p_node, const char*& p_input );
   bool do_parse_command( node* p_node, size_t& argnum, bool is_opt, bool is_branch = false );

   void do_dump_nodes( node* p_node, ostream& ostr, int indent_level = 0 ) const;
   void do_output_usage( node* p_node, ostream& ostr, bool after_match = false ) const;
   void do_output_syntax( node* p_node, ostream& ostr ) const;

   void do_get_parameter_names( node* p_node, vector< string >& parameters, vector< string >* p_default_vals ) const;

   void parse_syntax_expression( node* p_node );

   string::size_type search_for_pattern(
    const string& pat, const string& input, string::size_type& length, vector< string >& refs );

   bool parse_command_expression( node* p_node, size_t& argnum );
   bool parse_separated_list_items( node* p_node, size_t& argnum, string& value );

   bool is_invalid;
   bool end_of_input;

   node* p_node;
   size_t num_nodes;
   size_t error_pos;

   const char* p_input_error;
   const char* p_original_input;

   stack< node* > opt_node_stack;
   stack< node* > match_node_stack;

   const vector< string >* p_arguments;
   map< string, string >* p_parameters;

   typedef map< string, string >::iterator parameter_iterator;
   typedef map< string, string >::value_type parameter_value_type;
};

void command_parser::impl::clear( )
{
   if( p_node )
   {
      do_clear( p_node );
      p_node = 0;
   }
}

void command_parser::impl::parse_syntax( const char* p_input )
{
#ifdef DEBUG
   cout << "\n**** begin parse_syntax: p_input = '" << p_input << "' ****" << endl;
#endif
   error_pos = 0;
   p_input_error = 0;
   p_original_input = p_input;

   is_invalid = false;
   end_of_input = false;

   while( !opt_node_stack.empty( ) )
      opt_node_stack.pop( );

   while( !match_node_stack.empty( ) )
      match_node_stack.pop( );

   if( p_node )
      do_clear( p_node );

   p_node = new node;
   p_node->id = ++num_nodes;

   const char* p_input_copy( p_input );
   do_parse_syntax( p_node, p_input_copy );

   if( *p_input_copy != 0 || !opt_node_stack.empty( ) || !match_node_stack.empty( ) )
   {
#ifdef DEBUG
      if( *p_input_copy != 0 )
         cout << "bad due to: *p_input_copy != 0" << endl;
      if( !opt_node_stack.empty( ) )
         cout << "bad due to: !opt_node_stack.empty( )" << endl;
      if( !match_node_stack.empty( ) )
         cout << "bad due to: !match_node_stack.empty( )" << endl;
#endif
      is_invalid = true;
      p_input_error = p_input_copy;
   }

   if( p_input_error )
      error_pos = p_input_error - p_original_input;
#ifdef DEBUG
   else if( is_invalid )
      cout << "*** oops - p_input_error was not set ***" << endl;
#endif
#ifdef DEBUG
   cout << "**** finish parse_syntax: p_input = '" << p_input << "' ****" << endl;
#endif
}

bool command_parser::impl::parse_command( const vector< string >& arguments, map< string, string >& parameters )
{
   bool retval = false;

#ifdef DEBUG
   cout << "\n**** begin parse_command ****" << endl;
#endif
   if( p_node )
   {
      parameters.clear( );
      p_parameters = &parameters;
#ifdef DEBUG
      cout << "\n[arguments]\n";
      for( vector< string >::size_type i = 0; i < arguments.size( ); i++ )
         cout << i << " [" << arguments[ i ] << "]\n";
      cout << endl;
#endif

      size_t argnum = 0;
      p_arguments = &arguments;
      retval = do_parse_command( p_node, argnum, false );
      if( retval && argnum != arguments.size( ) )
         retval = false;
   }

#ifdef DEBUG
   cout << "final retval = " << retval << endl;
   cout << "**** finish parse_command ****" << endl;
#endif
   return retval;
}

void command_parser::impl::dump_nodes( ostream& ostr ) const
{
   if( p_node )
      do_dump_nodes( p_node, ostr );
}

string command_parser::impl::get_usage( ) const
{
   ostringstream osstr;
   if( p_node )
      do_output_usage( p_node, osstr );

   return osstr.str( );
}

void command_parser::impl::output_usage( ostream& ostr ) const
{
   if( p_node )
      do_output_usage( p_node, ostr );
}

void command_parser::impl::output_syntax( ostream& ostr ) const
{
   if( p_node )
      do_output_syntax( p_node, ostr );
}

void command_parser::impl::get_parameter_names( vector< string >& parameters, vector< string >* p_default_vals ) const
{
   parameters.clear( );
   if( p_node )
      do_get_parameter_names( p_node, parameters, p_default_vals );
}

void command_parser::impl::do_clear( node* p_node )
{
   if( p_node->p_match_node )
      do_clear( p_node->p_match_node );

   for( vector< node* >::size_type i = 0; i < p_node->opt_branch_nodes.size( ); i++ )
      do_clear( p_node->opt_branch_nodes[ i ] );

   node* p_next_node = p_node->p_next_node;
   delete p_node;
   --num_nodes;

   if( p_next_node )
      do_clear( p_next_node );
}

void command_parser::impl::do_parse_syntax( node* p_node, const char*& p_input )
{
#ifdef DEBUG
   cout << "do_parse_syntax: id = " << p_node->id << ", p_input = '" << p_input << "'" << endl;
#endif
   bool had_begin = false;
   bool had_escape = false;
   bool had_finish = false;
   bool had_invalid = false;
   bool add_right_link = false;

   while( true )
   {
#ifdef DEBUG
      cout << "*p_input = '" << *p_input << "'" << endl;
      cout << "had_begin = " << had_begin << endl;
      cout << "had_finish = " << had_finish << endl;
      cout << "p_node->id = " << p_node->id << endl;
#endif
      if( *p_input == '\0' )
      {
         end_of_input = true;

         if( !opt_node_stack.empty( ) )
            p_input_error = p_input;
         else if( !match_node_stack.empty( ) )
            p_input_error = p_input;

         break;
      }

      if( !had_escape && *p_input == '\\' )
      {
         had_escape = true;

         ++p_input;
         continue;
      }
      else if( had_escape )
      {
         had_escape = false;
         p_node->expression += *p_input;

         ++p_input;
         continue;
      }

      if( !had_begin && *p_input == c_opt_branch_begin && *( p_input + 1 ) != c_opt_branch_finish )
      {
#ifdef DEBUG
         cout << "adding opt branch to id: " << p_node->id << endl;
#endif
         node* p_new_node = new node;
         p_new_node->id = ++num_nodes;

         p_node->opt_branch_nodes.push_back( p_new_node );

#ifdef DEBUG
         cout << "::: opt push :::" << endl;
#endif
         opt_node_stack.push( p_node );
         do_parse_syntax( p_new_node, ++p_input );
#ifdef DEBUG
         cout << "[after opt push] p_input ==> '" << p_input << "'" << endl;
#endif
         continue;
      }
      else if( !had_begin
       && *p_input == c_match_branch_begin && *( p_input + 1 ) != c_match_branch_finish )
      {
#ifdef DEBUG
         cout << "adding match branch to id: " << p_node->id << endl;
#endif
         node* p_new_node = new node;
         p_new_node->id = ++num_nodes;
         p_node->p_match_node = p_new_node;

         // NOTE: Any node is permitted to just contain a match branch.
         p_node->is_okay = true;
#ifdef DEBUG
         cout << "::: match push :::" << endl;
#endif
         match_node_stack.push( p_node );
         do_parse_syntax( p_new_node, ++p_input );
#ifdef DEBUG
         cout << "[after match push] p_input ==> '" << p_input << "'" << endl;
#endif
         if( !match_node_stack.empty( ) && *p_input == c_match_branch_finish && *( p_input + 1 ) == '\0' )
         {
#ifdef DEBUG
            cout << "::: match pop :::" << endl;
#endif
            match_node_stack.pop( );
#ifdef DEBUG
            cout << "*** is at end ***" << endl;
#endif
            if( *p_input != '\0' )
               ++p_input;

            return;
         }

         if( !match_node_stack.empty( ) && match_node_stack.top( )->expression.empty( ) )
         {
            match_node_stack.top( )->p_next_node = new node;
            match_node_stack.top( )->p_next_node->p_prev_node = p_node;

            p_node = match_node_stack.top( )->p_next_node;
            match_node_stack.pop( );
#ifdef DEBUG
            cout << "::: match pop :::" << endl;
#endif

            p_node->id = ++num_nodes;
         }
         else
            break;
      }
      else if( had_begin == had_finish && *p_input == c_opt_branch_finish && !opt_node_stack.empty( ) )
      {
#ifdef DEBUG
         cout << "::: opt pop :::" << endl;
#endif
         p_node = opt_node_stack.top( );
         opt_node_stack.pop( );
#ifdef DEBUG
         cout << "back to id: " << p_node->id << endl;
#endif
      }
      else if( had_begin == had_finish && *p_input == c_match_branch_finish && !match_node_stack.empty( ) )
      {
#ifdef DEBUG
         cout << "::: match pop :::" << endl;
#endif
         p_node = match_node_stack.top( );
         match_node_stack.pop( );
#ifdef DEBUG
         cout << "back to id: " << p_node->id << endl;
#endif
         if( *p_input != '\0' )
            ++p_input;

         return;
      }
      else if( !had_begin
       && !p_node->is_alt && *p_input == c_alternative_separator
       && p_node->p_prev_node && p_node->p_prev_node->is_okay )
      {
#ifdef DEBUG
         cout << "<<< is_alt >>>" << endl;
#endif
         if( !p_node->opt_branch_nodes.empty( ) )
         {
            p_node->is_okay = true;
            add_right_link = true;
            break;
         }

         p_node->is_alt = true;
      }
      else if( !had_begin && *p_input == c_token_begin )
      {
#ifdef DEBUG
         cout << "<<< had_begin >>>" << endl;
#endif
         had_begin = true;
      }
      else if( had_begin && !had_finish && *p_input == c_token_finish )
      {
#ifdef DEBUG
         cout << "<<< had_finish >>>" << endl;
#endif
         had_finish = true;
         ++p_input;
         break;
      }
      else if( had_begin
       && *p_input != c_token_begin && *p_input != c_token_finish
       && *p_input != c_alternative_separator && isprint( *p_input )
       && ( ( *p_input != c_opt_branch_begin && *p_input != c_opt_branch_finish ) || had_finish == false ) )
      {
#ifdef DEBUG
         cout << "adding expr char '" << *p_input << "'" << endl;
#endif
         p_node->expression += *p_input;
      }
      else
      {
         is_invalid = true;
         had_invalid = true;
         if( !p_input_error )
            p_input_error = p_input;

#ifdef DEBUG
         cout << "invalid/unexpected token '" << *p_input << "'" << endl;
#endif
      }

      if( end_of_input )
         break;

      if( *p_input != '\0' )
         ++p_input;
   }

   if( !end_of_input && *( p_input ) == '\0' )
      end_of_input = true;

#ifdef DEBUG
   cout << "::: after loop :::" << endl;
   if( end_of_input )
      cout << "==> end_of_input" << endl;
#endif

   if( !had_invalid && had_begin && had_finish && !p_node->expression.empty( ) )
   {
      if( !p_node->is_alt || ( p_node->is_alt && ( p_node->p_prev_node || p_node->p_next_node ) ) )
         parse_syntax_expression( p_node );
#ifdef DEBUG
      cout << "id: " << p_node->id << " p_input ==> '" << p_input << "'" << endl;
      if( p_node->is_okay )
         cout << "(okay)" << endl;
#endif
   }

   if( had_finish && !end_of_input && *p_input == c_match_branch_begin )
   {
#ifdef DEBUG
      cout << "*** process match branch ***" << endl;
#endif
      do_parse_syntax( p_node, p_input );

      if( !match_node_stack.empty( ) && *p_input == c_match_branch_finish )
      {
         ++p_input;
         p_node = match_node_stack.top( );
         match_node_stack.pop( );
#ifdef DEBUG
         cout << "::: match pop (after loop) :::" << endl;
#endif
      }
   }

   if( *p_input == '\0' )
      end_of_input = true;

   if( had_finish && !end_of_input )
   {
#ifdef DEBUG
      cout << "<<< node completed (id = " << p_node->id << ") >>>" << endl;
#endif
      if( *p_input == c_token_begin
       || *p_input == c_opt_branch_begin || *p_input == c_alternative_separator )
         add_right_link = true;
   }

   if( !had_invalid && !had_begin && !end_of_input && p_node->p_match_node && !p_node->p_next_node
    && ( *p_input == c_token_begin || *p_input == c_opt_branch_begin || *p_input == c_alternative_separator ) )
      add_right_link = true;

   if( !end_of_input || ( opt_node_stack.empty( ) && match_node_stack.empty( ) ) )
   {
      if( !p_node->is_okay )
      {
#ifdef DEBUG
         cout << "(check if okay)" << endl;
         cout << "id = " << p_node->id << endl;
         cout << "is_invalid = " << is_invalid << endl;
         cout << "p_input = '" << p_input << "'" << endl;
         cout << "p_node->is_alt = " << p_node->is_alt << endl;
         cout << "p_node->opt_branch_nodes.size( ) = " << p_node->opt_branch_nodes.size( ) << endl;
#endif
         if( !is_invalid && !p_node->is_alt && !p_node->opt_branch_nodes.empty( ) )
         {
            bool found_empty = false;
            for( vector< node* >::size_type i = 0; i < p_node->opt_branch_nodes.size( ); i++ )
            {
               if( !p_node->opt_branch_nodes[ i ]->p_next_node
                && p_node->opt_branch_nodes[ i ]->expression.empty( ) )
               {
                  found_empty = true;
                  break;
               }
            }

            if( !found_empty )
               p_node->is_okay = true;
         }
         else if( !is_invalid && p_node->is_alt && p_node->p_match_node && p_node->p_match_node->is_okay )
            p_node->is_okay = true;

#ifdef DEBUG
         if( p_node->is_okay )
            cout << "(okay)" << endl;
#endif
      }

      if( !end_of_input && p_node->is_okay )
      {
         bool have_popped = false;

         if( have_popped && *p_input != '\0' && *p_input != c_alternative_separator )
         {
            add_right_link = true;
            p_node->is_okay = true;
         }
      }
   }

   if( add_right_link )
   {
      while( p_node->p_next_node )
         p_node = p_node->p_next_node;
#ifdef DEBUG
      cout << "=== adding right link to id: " << p_node->id << " ===" << endl;
#endif
      p_node->p_next_node = new node;
      p_node->p_next_node->id = ++num_nodes;

      p_node->p_next_node->p_prev_node = p_node;

      do_parse_syntax( p_node->p_next_node, p_input );
      return;
   }

   if( !p_node->is_okay )
   {
      is_invalid = true;
      if( !p_input_error )
         p_input_error = p_input;
#ifdef DEBUG
      cout << "*** error *** id: " << p_node->id << endl;
#endif
   }
}

bool command_parser::impl::do_parse_command( node* p_node, size_t& argnum, bool is_opt, bool is_branch )
{
#ifdef DEBUG
   cout << "argnum = " << argnum << endl;
   cout << "is_opt = " << is_opt << endl;
   cout << "is_branch = " << is_branch << endl;
   cout << "p_node->id = " << p_node->id << endl;
   cout << "p_node->is_alt = " << p_node->is_alt << endl;
   cout << "p_node->expression = '" << p_node->expression << "'" << endl;
#endif

   bool okay = true;
   bool retval = false;
   bool matched = false;
   size_t old_argnum = argnum;

   if( argnum == p_arguments->size( ) )
   {
      if( p_node->parameter.empty( ) && !p_node->p_next_node )
         retval = true;

      if( retval && p_node->p_match_node )
         retval = do_parse_command( p_node->p_match_node, argnum, false, true );
   }
   else
   {
      bool prefix_matched = false;
      string input( ( *p_arguments )[ argnum ] );

      // NOTE: If the node has a prefix that is found in the input then don't check
      // any optional branch nodes (as it is clear it should match with this node).
      if( !p_node->prefix.empty( ) && input.find( p_node->prefix ) == 0 )
         prefix_matched = true;

      if( !prefix_matched && !p_node->opt_branch_nodes.empty( ) )
      {
#ifdef DEBUG
         cout << "(checking optional branch nodes)" << endl;
#endif
         okay = false;
         vector< bool > opt_done( p_node->opt_branch_nodes.size( ) );

         // IMPORTANT: Optional commands that are in the same branch can be provided in any order
         // and options that are non-prefixed lists are permitted to be repeated multiple times.
         for( vector< node* >::size_type i = 0; i < p_node->opt_branch_nodes.size( ); )
         {
            if( ( opt_done[ i ] == false
             || ( !p_node->opt_branch_nodes[ i ]->prefix.empty( )
             && ( p_node->opt_branch_nodes[ i ]->type == c_expr_type_list
             || p_node->opt_branch_nodes[ i ]->type == c_expr_type_olist ) ) )
             && ( do_parse_command( p_node->opt_branch_nodes[ i ], argnum, true ) ) == true )
            {
               opt_done[ i ] = true;
               okay = true;
               i = 0;
            }
            else
               i++;
         }

         if( okay && argnum == p_arguments->size( ) && argnum != old_argnum && p_node->expression.empty( ) )
            retval = true;

         if( !okay && argnum == old_argnum )
            okay = true;
#ifdef DEBUG
         cout << "okay = " << okay << endl;
         cout << "retval = " << retval << endl;
         cout << "argnum now = " << argnum << endl;
         cout << "p_node->prefix now = " << p_node->prefix << endl;
#endif
      }

#ifdef DEBUG
      cout << "(after opt checks) p_node->id = " << p_node->id << endl;
#endif
      if( !is_opt && !p_node->parameter.empty( ) )
         retval = false;

      if( okay && p_node->expression.empty( ) )
         retval = true;

      if( okay && !retval && argnum < p_arguments->size( ) && parse_command_expression( p_node, argnum ) )
      {
         retval = true;
         matched = true;
      }
      else if( is_branch )
         argnum = old_argnum;

#ifdef DEBUG
      cout << "*** has matched: " << matched << endl;
#endif
      deque< node* > alt_branch_nodes;

      bool use_current_node = false;
      if( retval && p_node->p_match_node )
      {
         retval = do_parse_command( p_node->p_match_node, argnum, false, true );

         // NOTE: Special case to handle {branch}|{branch} situations where an empty
         // right "alt" node needs to be skipped in order to get to the other branch.
         if( !retval && p_node->p_next_node && p_node->p_next_node->is_alt
          && p_node->p_next_node->p_match_node && p_node->p_next_node->expression.empty( ) )
         {
            node* p_tmp_node = p_node->p_next_node->p_next_node;

            // NOTE: There may be several "alt" branches so will
            // need to remember each for next link checks later.
            while( p_tmp_node )
            {
               alt_branch_nodes.push_back( p_tmp_node );
               p_tmp_node = p_tmp_node->p_next_node;
            }

            retval = true;
            use_current_node = true;
            p_node = p_node->p_next_node->p_match_node;
         }
      }

      if( retval )
      {
         old_argnum = argnum;
         node* p_next_node( use_current_node ? p_node : p_node->p_next_node );

         while( p_next_node )
         {
            // NOTE: If did not just complete a match branch and the next node expression is empty then unless
            // the current node was matched it's assumed that this is an optional branch that must be skipped.
            if( !matched && !p_node->p_match_node && p_next_node->expression.empty( ) )
               p_next_node = p_next_node->p_next_node;

            // NOTE: Skip alternatives if the current node was matched or if just processed an optional branch.
            if( p_next_node && p_next_node->is_alt && ( matched || p_node->expression.empty( ) ) )
               p_next_node = p_next_node->p_next_node;
            else
               break;
         }

         while( p_next_node )
         {
#ifdef DEBUG
            cout << "now checking next link..." << endl;
#endif
            retval = do_parse_command( p_next_node, argnum, false );
            if( !retval && p_next_node->expression.empty( ) )
            {
               retval = true;
               argnum = old_argnum;
            }

            if( retval || alt_branch_nodes.empty( ) )
               break;

            p_next_node = alt_branch_nodes.front( );
            alt_branch_nodes.pop_front( );
         }
      }
      else if( !matched )
      {
         if( p_node->is_alt )
            argnum = old_argnum;

         if( p_node->p_next_node )
         {
            if( p_node->p_next_node->is_alt )
               retval = do_parse_command( p_node->p_next_node, argnum, is_opt );
            else if( p_node->p_next_node->expression.empty( )
             && p_node->p_next_node->p_next_node && p_node->p_next_node->p_next_node->is_alt )
               retval = do_parse_command( p_node->p_next_node->p_next_node, argnum, is_opt );
         }
      }
   }

#ifdef DEBUG
   cout << "retval is: " << retval << endl;
#endif
   return retval;
}

void command_parser::impl::do_dump_nodes( node* p_node, ostream& ostr, int indent_level ) const
{
   string indent( indent_level, ' ' );

   ostr << '\n' << indent;
   ostr << "[node #" << p_node->id << "]\n" << indent;
   ostr << "type: " << p_node->type << '\n' << indent;
   if( p_node->is_alt )
      ostr << "* is_alt *\n" << indent;
   ostr << "prefix: \"" << p_node->prefix << "\"\n" << indent;
   ostr << "parameter: " << p_node->parameter << '\n' << indent;
   ostr << "description: " << p_node->description << '\n';

   if( !p_node->type.empty( ) )
      ostr << indent << "separator: '" << p_node->separator << "'\n";

   if( p_node->type == c_expr_type_list || p_node->type == c_expr_type_olist )
      ostr << indent << "terminator: '" << p_node->terminator << "'\n";

   if( p_node->p_next_node )
      ostr << indent << "--> (next): " << p_node->p_next_node->id << '\n';
   if( p_node->p_match_node )
      ostr << indent << "--> (match): " << p_node->p_match_node->id << '\n';

   for( vector< node* >::size_type i = 0; i < p_node->opt_branch_nodes.size( ); i++ )
      do_dump_nodes( p_node->opt_branch_nodes[ i ], ostr, indent_level + 1 );

   if( p_node->p_match_node )
      do_dump_nodes( p_node->p_match_node, ostr, indent_level );

   node* p_next_node = p_node->p_next_node;

   if( p_next_node )
      do_dump_nodes( p_next_node, ostr, indent_level );
}

void command_parser::impl::do_output_usage( node* p_node, ostream& ostr, bool after_match ) const
{
   if( p_node->is_alt )
      ostr << c_alternative_separator;

   bool had_output_branch = false;
   for( vector< node* >::size_type i = 0; i < p_node->opt_branch_nodes.size( ); i++ )
   {
      if( i > 0 )
         ostr << ' ';
      ostr << c_opt_branch_begin;
      do_output_usage( p_node->opt_branch_nodes[ i ], ostr );
      ostr << c_opt_branch_finish;
      had_output_branch = true;
   }

   if( !p_node->expression.empty( ) )
   {
      if( had_output_branch )
         ostr << ' ';

      if( p_node->type == c_expr_type_opt )
         ostr << p_node->prefix;
      else
      {
         if( p_node->type != c_expr_type_pat )
            ostr << p_node->prefix;

         ostr << c_token_begin;

         if( p_node->type == c_expr_type_oval || p_node->type == c_expr_type_olist )
            ostr << c_opt_branch_begin;

         ostr << p_node->description;

         if( p_node->type == c_expr_type_pat )
            ostr << ": " << p_node->prefix;

         if( p_node->type == c_expr_type_oval || p_node->type == c_expr_type_olist )
            ostr << c_opt_branch_finish;

         if( p_node->separator && ( p_node->type == c_expr_type_list || p_node->type == c_expr_type_olist ) )
            ostr << p_node->separator;

         if( p_node->type == c_expr_type_val && !p_node->default_val.empty( ) )
            ostr << ':' << p_node->default_val;

         ostr << c_token_finish;

         if( p_node->separator && ( p_node->type == c_expr_type_val || p_node->type == c_expr_type_oval ) )
            ostr << p_node->separator;

         if( p_node->terminator && p_node->terminator != ' ' )
            ostr << p_node->terminator;
      }
   }

   if( p_node->p_match_node )
   {
      if( had_output_branch || !p_node->expression.empty( ) )
         ostr << ' ';

      ostr << c_match_branch_begin;
      do_output_usage( p_node->p_match_node, ostr );
      ostr << c_match_branch_finish;
   }

   if( p_node->p_next_node )
   {
      if( !p_node->expression.empty( ) && !p_node->p_next_node->is_alt )
         ostr << ' ';
      do_output_usage( p_node->p_next_node, ostr, p_node->p_match_node != 0 );
   }
}

void command_parser::impl::do_output_syntax( node* p_node, ostream& ostr ) const
{
   if( p_node->is_alt )
      ostr << c_alternative_separator;

   for( vector< node* >::size_type i = 0; i < p_node->opt_branch_nodes.size( ); i++ )
   {
      ostr << c_opt_branch_begin;
      do_output_syntax( p_node->opt_branch_nodes[ i ], ostr );
      ostr << c_opt_branch_finish;
   }

   if( !p_node->expression.empty( ) )
   {
      ostr << c_token_begin;
      ostr << p_node->expression;
      ostr << c_token_finish;
   }

   if( p_node->p_match_node )
   {
      ostr << c_match_branch_begin;
      do_output_syntax( p_node->p_match_node, ostr );
      ostr << c_match_branch_finish;
   }

   if( p_node->p_next_node )
      do_output_syntax( p_node->p_next_node, ostr );
}

void command_parser::impl::do_get_parameter_names( node* p_node, vector< string >& parameters, vector< string >* p_default_vals ) const
{
   for( vector< node* >::size_type i = 0; i < p_node->opt_branch_nodes.size( ); i++ )
      do_get_parameter_names( p_node->opt_branch_nodes[ i ], parameters, p_default_vals );

   if( !p_node->parameter.empty( ) )
   {
      parameters.push_back( p_node->parameter );

      if( p_default_vals )
         p_default_vals->push_back( p_node->default_val );
   }

   if( p_node->p_match_node )
      do_get_parameter_names( p_node->p_match_node, parameters, p_default_vals );

   if( p_node->p_next_node )
      do_get_parameter_names( p_node->p_next_node, parameters, p_default_vals );
}

void command_parser::impl::parse_syntax_expression( node* p_node )
{
   bool found_error = false;
   bool had_separator = false;
   bool had_terminator = false;

   string expression( p_node->expression );

   size_t num_subs = count( expression.begin( ), expression.end( ), c_sub_expression_divider );

   p_node->type = get_next_sub_expression( expression, c_sub_expression_divider );

   if( expression.empty( ) )
      found_error = true;
   else
   {
      if( p_node->type != c_expr_type_opt && p_node->type != c_expr_type_pat
       && p_node->type != c_expr_type_val && p_node->type != c_expr_type_oval
       && p_node->type != c_expr_type_list && p_node->type != c_expr_type_olist )
         found_error = true;

      if( !found_error )
      {
         p_node->prefix = get_next_sub_expression( expression, c_sub_expression_divider );

         if( p_node->type == c_expr_type_pat )
         {
            // NOTE: Construct a regex object here in order to test that its syntax is valid.
            try
            {
               regex expr( p_node->prefix );
            }
            catch( exception& )
            {
               found_error = true;
            }
         }
         else if( !p_node->prefix.empty( )
          && ( p_node->type == c_expr_type_val || p_node->type == c_expr_type_oval ) )
         {
            string::size_type pos = p_node->prefix.find( c_exp_type_prefix_default_separator );
            if( pos != string::npos )
            {
               p_node->default_val = p_node->prefix.substr( pos + 1 );
               p_node->prefix.erase( pos + 1 );
            }
         }

         if( !expression.empty( ) )
         {
            p_node->parameter = get_next_sub_expression( expression, c_sub_expression_divider );

            if( !expression.empty( ) )
            {
               p_node->description = get_next_sub_expression( expression, c_sub_expression_divider );

               if( !expression.empty( ) )
               {
                  string separator = get_next_sub_expression( expression, c_sub_expression_divider );
                  if( separator.size( ) != 1 )
                     found_error = true;
                  else
                  {
                     had_separator = true;
                     p_node->separator = separator[ 0 ];
                     if( p_node->separator == c_exp_type_list_default_terminator )
                        p_node->terminator = '\0';

                     if( !expression.empty( ) )
                     {
                        string terminator = get_next_sub_expression( expression, c_sub_expression_divider );
                        if( terminator.size( ) != 1 )
                           found_error = true;
                        else
                        {
                           had_terminator = true;
                           p_node->terminator = terminator[ 0 ];
                        }
                     }
                  }
               }
            }
            else
               p_node->description = p_node->parameter;
         }
         else
            p_node->description = p_node->parameter = p_node->prefix;
      }
   }

   if( !found_error && !expression.empty( ) )
      found_error = true;

   if( !found_error && !had_separator && p_node->type != c_expr_type_list && p_node->type != c_expr_type_olist )
      p_node->separator = '\0';

   if( !found_error && had_terminator && p_node->type != c_expr_type_list && p_node->type != c_expr_type_olist )
      found_error = true;

   if( !found_error && p_node->description.empty( ) )
      p_node->description = p_node->parameter;

#ifdef DEBUG
   cout << "\n[node expression details]\n";
   cout << "id: " << p_node->id << '\n';
   cout << "type: " << p_node->type << '\n';
   cout << "prefix: \"" << p_node->prefix << "\"\n";
   cout << "parameter: " << p_node->parameter << '\n';
   cout << "description: " << p_node->description << '\n';
   cout << "separator: '" << p_node->separator << "'\n";
   cout << "terminator: '" << p_node->terminator << "'\n";
   cout << endl;
#endif
   if( !found_error && !p_node->parameter.empty( ) )
      p_node->is_okay = true;
}

string::size_type command_parser::impl::search_for_pattern(
 const string& pat, const string& input, string::size_type& length, vector< string >& refs )
{
   regex expr( pat );
   return expr.search( input, &length, &refs );
}

bool command_parser::impl::parse_command_expression( node* p_node, size_t& argnum )
{
   bool retval = false;

   char separator_buffer[ 2 ];
   separator_buffer[ 0 ] = p_node->separator;
   separator_buffer[ 1 ] = '\0';

   string input( ( *p_arguments )[ argnum ] );

   string match_prefix( p_node->prefix );

#ifdef DEBUG
   cout << "input = '" << input << "'" << endl;
   cout << "p_node->expression = '" << p_node->expression << "'" << endl;
#endif
   if( p_node->expression.empty( ) )
      return retval;

   char prefix_separator = '\0';
   if( !match_prefix.empty( ) )
      prefix_separator = match_prefix[ match_prefix.size( ) - 1 ];

   bool prefix_separator_is_whitespace = false;
   if( isspace( static_cast< unsigned char >( prefix_separator ) ) )
   {
      prefix_separator_is_whitespace = true;
      if( !match_prefix.empty( ) )
         match_prefix = p_node->prefix.substr( 0, match_prefix.size( ) - 1 );
   }

   if( !match_prefix.empty( )
    && ( match_prefix.size( ) <= input.size( ) || p_node->type == c_expr_type_pat ) )
   {
      if( p_node->type == c_expr_type_opt )
      {
#ifdef DEBUG
         cout << "check opt prefix..." << endl;
#endif
         if( match_prefix == input )
         {
#ifdef DEBUG
            cout << "opt matched prefix '" << match_prefix << "'" << endl;
#endif
            ++argnum;
            retval = true;

            string value;
            string parameter( p_node->parameter );
            string::size_type pos = parameter.find( c_exp_type_opt_value_separator );

            if( pos != string::npos )
            {
               value = parameter.substr( pos + 1 );
               parameter.erase( pos );
            }

            p_parameters->insert( parameter_value_type( parameter, value ) );
         }
      }
      else if( p_node->type == c_expr_type_pat )
      {
         vector< string > refs;
         string::size_type start, length;

         if( ( start = search_for_pattern( p_node->prefix, input, length, refs ) ) != string::npos )
         {
#ifdef DEBUG
            cout << "pat matched prefix '" << match_prefix << "'" << endl;
#endif
            ++argnum;
            retval = true;

            string value;
            string parameter( p_node->parameter );
            string::size_type pos = parameter.find( c_exp_type_opt_value_separator );

            if( pos != string::npos )
            {
               value = parameter.substr( pos + 1 );
               parameter.erase( pos );
            }

            value += input.substr( start, length );

            p_parameters->insert( parameter_value_type( parameter, value ) );

            if( !refs.empty( ) )
            {
               for( size_t i = 0; i < refs.size( ); i++ )
               {
                  string prefix( "_" );
                  if( refs.size( ) > 9 && i < 9 )
                     prefix += "0";

                  p_parameters->insert( parameter_value_type( parameter + prefix + to_string( i + 1 ), refs[ i ] ) );
               }
            }
         }
      }
      else if( p_node->type == c_expr_type_val || p_node->type == c_expr_type_oval )
      {
#ifdef DEBUG
         cout << "check val prefix..." << endl;
#endif
         if( prefix_separator_is_whitespace && match_prefix == input )
         {
            if( argnum < p_arguments->size( ) - 1 )
            {
#ifdef DEBUG
               cout << "val matched prefix '" << match_prefix << "'" << endl;
#endif
               if( p_node->type == c_expr_type_oval || !( *p_arguments )[ argnum + 1 ].empty( ) )
               {
                  ++argnum;
                  retval = true;
                  p_parameters->insert(
                   parameter_value_type( p_node->parameter, ( *p_arguments )[ argnum++ ] ) );
               }
            }
         }
         else if( !prefix_separator_is_whitespace && match_prefix == input.substr( 0, match_prefix.size( ) ) )
         {
            input.erase( 0, match_prefix.size( ) );
            if( !input.empty( ) || argnum <= p_arguments->size( ) - 1 )
            {
#ifdef DEBUG
               cout << "val matched substr prefix '" << match_prefix << "'" << endl;
#endif
               if( p_node->type == c_expr_type_oval || !input.empty( ) )
               {
                  ++argnum;
                  retval = true;
                  p_parameters->insert( parameter_value_type( p_node->parameter, input ) );
               }
            }
         }
         else if( !prefix_separator_is_whitespace && !p_node->default_val.empty( ) )
         {
            retval = true;
            p_parameters->insert( parameter_value_type( p_node->parameter, p_node->default_val ) );
         }
      }
      else if( p_node->type == c_expr_type_list || p_node->type == c_expr_type_olist )
      {
#ifdef DEBUG
         cout << "check list prefix..." << endl;
#endif
         string value;
         if( prefix_separator_is_whitespace && match_prefix == input )
         {
            if( argnum < p_arguments->size( ) - 1 )
            {
#ifdef DEBUG
               cout << "list matched prefix '" << match_prefix << "'" << endl;
#endif
               if( p_node->type == c_expr_type_olist || !( *p_arguments )[ argnum + 1 ].empty( ) )
               {
                  ++argnum;
                  retval = true;
                  value = ( *p_arguments )[ argnum++ ];
               }
            }
         }
         else if( !prefix_separator_is_whitespace && match_prefix == input.substr( 0, match_prefix.size( ) ) )
         {
            input.erase( 0, match_prefix.size( ) );
            if( !input.empty( ) || argnum <= p_arguments->size( ) - 1 )
            {
#ifdef DEBUG
               cout << "list matched substr prefix '" << match_prefix << "'" << endl;
#endif
               if( p_node->type == c_expr_type_olist || all_list_values_are_not_empty( input, p_node->separator ) )
               {
                  ++argnum;
                  retval = true;
                  value = input;
               }
            }
         }

         if( retval )
         {
            if( isspace( static_cast< unsigned char >( p_node->separator ) ) )
               escape( value, separator_buffer );

            if( !parse_separated_list_items( p_node, argnum, value ) )
               retval = false;
            else
            {
               parameter_iterator pi = p_parameters->find( p_node->parameter );
               if( pi != p_parameters->end( ) )
               {
                  string tmp( pi->second );
                  tmp += p_node->separator;
                  tmp += value;
                  value = tmp;
                  p_parameters->erase( pi );
               }

               p_parameters->insert( parameter_value_type( p_node->parameter, value ) );
            }
         }
      }
   }
   else
   {
#ifdef DEBUG
      cout << "check no prefix..." << endl;
#endif
      if( match_prefix.empty( ) && !p_node->parameter.empty( ) )
      {
#ifdef DEBUG
         cout << "check match for non-prefixed parameter..." << endl;
#endif
         if( p_node->type == c_expr_type_oval
          || p_node->type == c_expr_type_olist
          || ( p_node->type != c_expr_type_list && !input.empty( ) )
          || ( p_node->type == c_expr_type_list && all_list_values_are_not_empty( input, p_node->separator ) ) )
         {
            ++argnum;
            retval = true;

            string value( input );

            if( p_node->type == c_expr_type_list && isspace( static_cast< unsigned char >( p_node->separator ) ) )
               escape( value, separator_buffer );

            if( p_node->type == c_expr_type_list || p_node->type == c_expr_type_olist )
               retval = parse_separated_list_items( p_node, argnum, value );
            else if( p_node->separator != '\0' )
            {
               if( value.empty( ) || value[ value.size( ) - 1 ] != p_node->separator )
               {
                  --argnum;
                  retval = false;
               }
               else
                  value = value.substr( 0, value.size( ) - 1 );
            }

            if( retval && value.empty( ) && p_node->type != c_expr_type_oval && p_node->type != c_expr_type_olist )
            {
               --argnum;
               retval = false;
            }

            if( retval )
            {
#ifdef DEBUG
               cout << "non-prefixed parameter match..." << endl;
               cout << "value: [" << value << "]" << endl;
#endif
               p_parameters->insert( parameter_value_type( p_node->parameter, value ) );
            }
         }
      }
   }

   return retval;
}

bool command_parser::impl::parse_separated_list_items( node* p_node, size_t& argnum, string& value )
{
   bool retval = true;
   char separator_buffer[ 2 ];
   separator_buffer[ 0 ] = p_node->separator;
   separator_buffer[ 1 ] = '\0';

   if( p_node->terminator != c_exp_type_list_default_terminator )
   {
#ifdef DEBUG
      cout << "checking for separated list items..." << endl;
#endif
      bool done = false;
      if( !value.empty( ) && value[ value.size( ) - 1 ] == p_node->terminator )
      {
         value = value.substr( 0, value.size( ) - 1 );
         if( isspace( static_cast< unsigned char >( p_node->separator ) ) )
            escape( value, separator_buffer );

         done = true;
      }

      string input;
      while( !done && argnum < p_arguments->size( ) )
      {
         input = ( *p_arguments )[ argnum ];

         if( !input.empty( ) )
         {
            if( input == string( 1, p_node->terminator ) )
            {
               done = true;
               input.erase( );
               if( p_node->type != c_expr_type_olist )
               {
                  retval = false;
                  break;
               }
            }

            if( !done && input[ input.size( ) - 1 ] == p_node->terminator )
            {
               done = true;
               input = input.substr( 0, input.size( ) - 1 );
            }
         }
         else if( p_node->type != c_expr_type_olist )
         {
            retval = false;
            break;
         }

         if( isspace( static_cast< unsigned char >( p_node->separator ) ) )
            escape( input, separator_buffer );

         ++argnum;
         if( !value.empty( ) )
            value += p_node->separator;
         value += input;
      }
   }

   return retval;
}

command_parser::command_parser( )
{
   p_impl = new impl;
}

command_parser::~command_parser( )
{
   delete p_impl;
}

void command_parser::clear( )
{
   p_impl->clear( );
}

void command_parser::parse_syntax( const char* p_input )
{
   p_impl->parse_syntax( p_input );
}

bool command_parser::parse_command( const vector< string >& arguments, map< string, string >& parameters )
{
   return p_impl->parse_command( arguments, parameters );
}

bool command_parser::okay( )
{
   return p_impl->okay( );
}

void command_parser::dump_nodes( ostream& ostr ) const
{
   return p_impl->dump_nodes( ostr );
}

string command_parser::get_usage( ) const
{
   return p_impl->get_usage( );
}

void command_parser::output_usage( ostream& ostr ) const
{
   p_impl->output_usage( ostr );
}

void command_parser::output_syntax( ostream& ostr ) const
{
   p_impl->output_syntax( ostr );
}

void command_parser::get_parameter_names( vector< string >& parameters, vector< string >* p_default_vals ) const
{
   p_impl->get_parameter_names( parameters, p_default_vals );
}

size_t command_parser::get_error_pos( ) const
{
   return p_impl->get_error_pos( );
}

size_t command_parser::get_num_nodes( ) const
{
   return p_impl->get_num_nodes( );
}

