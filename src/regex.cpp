// Copyright (c) 2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2013 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef __BORLANDC__
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <sstream>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#endif

#include "regex.h"

#include "utilities.h"

//#define DEBUG
//#define COMPILE_TESTBED_MAIN

using namespace std;

namespace
{

enum part_type
{
   e_part_type_lit,
   e_part_type_set
};

struct part
{
   part( )
    :
    type( e_part_type_lit ),
    min_matches( 1 ),
    max_matches( 1 ),
    inverted( false ),
    start_ref( false ),
    finish_ref( false )
   {
   }

   void clear( )
   {
      type = e_part_type_lit;
      literal.erase( );

      min_matches = 1;
      max_matches = 1;

      inverted = false;
      start_ref = false;
      finish_ref = false;

      matches.clear( );
   }

   part_type type;
   string literal;

   int min_matches;
   int max_matches;

   bool inverted;
   bool start_ref;
   bool finish_ref;

   vector< pair< char, char > > matches;
};

bool is_special_set( char ch )
{
   return ( ch == 'd' || ch == 's' || ch == 'w' );
}

void add_special_set( part& p, char ch )
{
   p.type = e_part_type_set;

   if( ch == 's' )
   {
      p.matches.push_back( make_pair( ' ', ' ' ) );
      p.matches.push_back( make_pair( '\t', '\t' ) );
      p.matches.push_back( make_pair( '\r', '\r' ) );
      p.matches.push_back( make_pair( '\n', '\n' ) );
   }
   else
   {
      p.matches.push_back( make_pair( '0', '9' ) );
      if( ch != 'd' )
      {
         p.matches.push_back( make_pair( 'a', 'z' ) );
         p.matches.push_back( make_pair( 'A', 'Z' ) );
         p.matches.push_back( make_pair( '_', '_' ) );
      }
   }
}

size_t literal_size( const string& lit )
{
   size_t len = 0;

   for( size_t i = 0; i < lit.length( ); i++ )
   {
      if( lit[ i ] != '\\' && lit[ i ] != '\b' )
         ++len;
   }

   return len;
}

string literal_prefix( const string& lit )
{
   string s;

   for( size_t i = 0; i < lit.size( ); i++ )
   {
      char ch = lit[ i ];

      if( ch == '\b' )
      {
         if( i == 0 )
            continue;
         else
            break;
      }

      s += lit[ i ];
   }

   return s;
}

inline bool is_word_char( char ch )
{
   bool rc = false;

   if( ch >= 'A' && ch <= 'Z' )
      rc = true;
   else if( ch >= 'a' && ch <= 'z' )
      rc = true;
   else if( ch >= '0' && ch <= '9' )
      rc = true;
   else if( ch == '_' )
      rc = true;

   return rc;
}

bool match_literal( const string& lit, const string& text, size_t pos )
{
   size_t len( text.length( ) );

   for( size_t i = 0; i < lit.length( ); i++ )
   {
      char ch = lit[ i ];
      char tc = text[ pos ];

      if( ch == '\b' )
      {
         if( pos == 0 )
            continue;

         if( is_word_char( tc ) )
            return false;
      }
      else if( ch != tc )
         return false;

      if( ++pos >= len )
      {
         if( i == lit.length( ) - 2 && lit[ lit.length( ) - 1 ] == '\b' )
            return true;

         return ( i == lit.length( ) - 1 );
      }
   }

   return true;
}

#ifdef DEBUG
void dump_state( const string& msg, char ch, char last_ch, bool ch_used, bool is_range,
 bool is_matches, bool had_empty, bool had_range, bool is_in_set, bool was_in_set, bool had_escape,
 bool has_minimum, bool has_maximum, bool set_started, bool is_set_matches )
{
   if( !msg.empty( ) )
      cout << msg << endl;

   cout << "ch = " << ch << endl;
   cout << "last_ch = " << last_ch << endl;

   cout << "ch_used = " << ch_used << endl;
   cout << "is_range = " << is_range << endl;
   cout << "had_empty = " << had_empty << endl;
   cout << "had_range = " << had_range << endl;
   cout << "is_in_set = " << is_in_set << endl;
   cout << "was_in_set = " << was_in_set << endl;
   cout << "is_matches = " << is_matches << endl;
   cout << "had_escape = " << had_escape << endl;
   cout << "has_minimum = " << has_minimum << endl;
   cout << "has_maximum = " << has_maximum << endl;
   cout << "set_started = " << set_started << endl;
   cout << "is_set_matches = " << is_set_matches << endl;
}
#endif

}

struct regex::impl
{
   impl( const string& expr );

   string get_expr( ) const { return expr; }

   int get_min_size( ) const { return min_size; }
   int get_max_size( ) const { return max_size; }

   string::size_type search( const string& text, string::size_type* p_length, vector< string >* p_refs );
   string::size_type do_search( const string& text, size_t start, string::size_type* p_length, vector< string >* p_refs );

   bool match_set( const string& text, size_t off, const part& p );

   string& expand_refs( string& literal );

   void dump( ostream& os );

   string expr;
   string prefix;

   int min_size;
   int max_size;

   bool match_at_start;
   bool match_at_finish;

   bool prefix_at_boundary;

   size_t last_unlimited_part;
   size_t max_size_from_finish;

   vector< part > parts;
   vector< string > refs;

   map< int, string > node_refs;
};

regex::impl::impl( const string& expr )
 :
 expr( expr ),
 min_size( 0 ),
 max_size( 1 ),
 match_at_start( false ),
 match_at_finish( false ),
 prefix_at_boundary( false ),
 last_unlimited_part( 0 ),
 max_size_from_finish( 0 )
{
   part next_part;

   char last_ch = '\0';

   bool ch_used = false;
   bool is_range = false;
   bool had_empty = false;
   bool had_range = false;
   bool is_in_ref = false;
   bool is_in_set = false;
   bool was_in_set = false;
   bool is_matches = false;
   bool had_escape = false;
   bool has_minimum = false;
   bool has_maximum = false;
   bool set_started = false;
   bool is_set_matches = false;

   size_t start = 0;

   for( size_t i = 0; i < expr.size( ); i++ )
   {
      char ch = expr[ i ];

      if( i == 0 && ch == '^' )
      {
         start++;
         match_at_start = true;

         continue;
      }

      char next_ch = '\0';
      if( i < expr.size( ) - 1 )
         next_ch = expr[ i + 1 ];

      ch_used = false;

      if( had_escape )
      {
         had_escape = false;

         switch( ch )
         {
            case 'a':
            ch = '\a';
            break;

            case 'b':
            ch = '\b';
            break;

            case 'e':
            ch = 0x27;
            break;

            case 'f':
            ch = '\f';
            break;

            case 'n':
            ch = '\n';
            break;

            case 'r':
            ch = '\r';
            break;

            case 't':
            ch = '\t';
            break;

            case 'v':
            ch = '\v';
            break;
         }

         if( ch == 'x' && i < expr.size( ) - 1 )
         {
            ch = hex_nibble( expr[ i + 1 ] );
            ch <<= 4;
            ch += hex_nibble( expr[ i + 2 ] );

            i += 2;

            if( i < expr.size( ) - 1 )
               next_ch = expr[ i + 1 ];
            else
               next_ch = '\0';
         }

         if( is_in_set )
         {
            ch_used = true;

            if( is_range )
            {
               had_range = true;
               next_part.matches.push_back( make_pair( last_ch, last_ch ) );
            }
            else if( is_special_set( ch ) )
            {
               add_special_set( next_part, ch );
               continue;
            }
            else
               next_part.matches.push_back( make_pair( last_ch, last_ch ) );

            is_range = false;
         }
         else
         {
            if( has_minimum || was_in_set || ( is_special_set( ch ) && !next_part.literal.empty( ) ) )
            {
#ifdef DEBUG
               ostringstream osstr;
               osstr << "[at 1: part #" << ( parts.size( ) + 1 ) << "]";
               dump_state( osstr.str( ), ch, last_ch, ch_used, is_range, had_empty, had_range, is_matches,
                is_in_set, was_in_set, had_escape, has_minimum, has_maximum, set_started, is_set_matches );
#endif
               parts.push_back( next_part );
               next_part.clear( );

               was_in_set = false;
               has_minimum = false;
               has_maximum = false;
            }

            ch_used = true;

            // NOTE: Ensure references are not being unescaped.
            if( ch >= '1' && ch <= '9' )
               next_part.literal += '\\';

            if( is_special_set( ch ) )
            {
               add_special_set( next_part, ch );

               if( i != expr.size( ) - 1 )
               {
                  parts.push_back( next_part );
                  next_part.clear( );
               }
            }
            else
               next_part.literal += ch;
         }
      }
      else if( ch == '\\' )
      {
         if( is_matches || is_set_matches )
            throw runtime_error( "invalid escaping for matches in: " + expr );

         had_escape = true;
         continue;
      }
      else
      {
         if( i == expr.size( ) - 1 && ch == '$' )
         {
            ch_used = true;
            match_at_finish = true;

            break;
         }

         if( ch == '(' )
         {
            if( is_in_ref )
               throw runtime_error( "invalid nested ref in: " + expr );

            if( i != start && !next_part.literal.empty( ) )
            {
#ifdef DEBUG
               ostringstream osstr;
               osstr << "[at 2: part #" << ( parts.size( ) + 1 ) << "]";
               dump_state( osstr.str( ), ch, last_ch, ch_used, is_range, had_empty, had_range, is_matches,
                is_in_set, was_in_set, had_escape, has_minimum, has_maximum, set_started, is_set_matches );
#endif
               parts.push_back( next_part );
               next_part.clear( );
            }

            is_in_ref = next_part.start_ref = true;
         }
         else if( ch == ')' )
         {
            if( !is_in_ref )
               throw runtime_error( "invalid finish ref in: " + expr );

            ch_used = true;
            is_in_ref = false;

            if( next_part.start_ref )
               next_part.finish_ref = true;
            else
               parts.back( ).finish_ref = true;
         }
         else if( ch == '[' )
         {
            if( is_in_set )
               throw runtime_error( "invalid set start in: " + expr );

            if( i != start && last_ch != '*' && last_ch != '+'
             && last_ch != '?' && last_ch != '(' && last_ch != ')' )
            {
#ifdef DEBUG
               ostringstream osstr;
               osstr << "[at 3: part #" << ( parts.size( ) + 1 ) << "]";
               dump_state( osstr.str( ), ch, last_ch, ch_used, is_range, had_empty, had_range, is_matches,
                is_in_set, was_in_set, had_escape, has_minimum, has_maximum, set_started, is_set_matches );
#endif
               parts.push_back( next_part );
               next_part.clear( );
            }

            if( next_ch == '^' )
            {
               next_part.inverted = true;
               ++i;
            }

            is_range = false;
            is_in_set = true;
            had_empty = false;
            was_in_set = false;
            set_started = true;

            next_part.type = e_part_type_set;
         }
         else if( ch == ']' )
         {
            if( !is_in_set )
               throw runtime_error( "invalid set finish in: " + expr );

            if( !had_range && last_ch != '[' )
               next_part.matches.push_back( make_pair( last_ch, last_ch ) );

            ch_used = true;
            is_range = false;
            had_range = false;
            is_in_set = false;
            was_in_set = true;
            set_started = false;

            if( next_part.matches.empty( ) )
               throw runtime_error( "invalid empty set in: " + expr );
         }
         else if( is_in_set )
         {
            if( set_started || had_range )
            {
               had_range = false;
               set_started = false;
            }
            else
            {
               if( ch == '-' && next_ch != ']' )
                  is_range = true;
               else
               {
                  if( is_range )
                  {
                     had_range = true;
                     next_part.matches.push_back( make_pair( last_ch, ch ) );
                  }
                  else
                     next_part.matches.push_back( make_pair( last_ch, last_ch ) );

                  is_range = false;
               }
            }
         }
         else if( was_in_set )
         {
            if( is_set_matches )
            {
               if( ch < '0' || ch > '9' )
               {
                  if( ch == ',' && !has_minimum )
                     has_minimum = true;
                  else if( ch != '}' )
                     throw runtime_error( "invalid set matches in: " + expr );
                  else
                  {
                     ch_used = true;
                     is_set_matches = false;
                  }
               }
               else
               {
                  ch_used = true;

                  if( !has_minimum )
                     next_part.min_matches = ( 10 * next_part.min_matches ) + ( ch - '0' );
                  else
                  {
                     has_maximum = true;
                     next_part.max_matches = ( 10 * next_part.max_matches ) + ( ch - '0' );
                  }
               }
            }
            else
            {
               if( ch == '*' )
                  next_part.min_matches = next_part.max_matches = 0;
               else if( ch == '+' )
               {
                  next_part.min_matches = 1;
                  next_part.max_matches = 0;
               }

               ch_used = true;

               if( ch == '{' )
               {
                  if( next_ch < '0' || next_ch > '9' )
                     throw runtime_error( "invalid set matches in: " + expr );

                  is_set_matches = true;
                  next_part.min_matches = 0;
                  next_part.max_matches = 0;
               }
               else
               {
                  if( ch == '?' )
                  {
                     next_part.min_matches = 0;
                     next_part.max_matches = 1;
                  }
                  else if( has_minimum && !has_maximum )
                     next_part.max_matches = next_part.min_matches;

                  was_in_set = false;
                  has_minimum = false;
                  has_maximum = false;
#ifdef DEBUG
                  ostringstream osstr;
                  osstr << "[at 4: part #" << ( parts.size( ) + 1 ) << "]";
                  dump_state( osstr.str( ), ch, last_ch, ch_used, is_range, had_empty, had_range, is_matches,
                   is_in_set, was_in_set, had_escape, has_minimum, has_maximum, set_started, is_set_matches );
#endif
                  parts.push_back( next_part );
                  next_part.clear( );

                  if( ch == '.' )
                     had_empty = true;
                  else if( ch != '[' && ch != '*' && ch != '+' && ch != '?' )
                     next_part.literal += ch;
               }
            }
         }
         else if( ch == '+' )
         {
            if( !next_part.finish_ref )
            {
               if( next_part.literal.length( ) > 1 && next_part.literal[ 0 ] != '\\' )
               {
                  next_part.literal.erase( next_part.literal.length( ) - 1 );
#ifdef DEBUG
                  ostringstream osstr;
                  osstr << "[at 5: part #" << ( parts.size( ) + 1 ) << "]";
                  dump_state( osstr.str( ), ch, last_ch, ch_used, is_range, had_empty, had_range, is_matches,
                   is_in_set, was_in_set, had_escape, has_minimum, has_maximum, set_started, is_set_matches );
#endif
                  parts.push_back( next_part );
                  next_part.clear( );

                  next_part.literal = last_ch;
               }
               else if( has_minimum )
               {
#ifdef DEBUG
                  ostringstream osstr;
                  osstr << "[at 6: part #" << ( parts.size( ) + 1 ) << "]";
                  dump_state( osstr.str( ), ch, last_ch, ch_used, is_range, had_empty, had_range, is_matches,
                   is_in_set, was_in_set, had_escape, has_minimum, has_maximum, set_started, is_set_matches );
#endif
                  parts.push_back( next_part );
                  next_part.clear( );

                  if( last_ch != '.' )
                     next_part.literal = last_ch;
               }
            }

            next_part.max_matches = 0;
#ifdef DEBUG
            ostringstream osstr;
            osstr << "[at 7: part #" << ( parts.size( ) + 1 ) << "]";
            dump_state( osstr.str( ), ch, last_ch, ch_used, is_range, had_empty, had_range, is_matches,
             is_in_set, was_in_set, had_escape, has_minimum, has_maximum, set_started, is_set_matches );
#endif
            parts.push_back( next_part );
            next_part.clear( );

            ch_used = true;
            had_empty = false;
         }
         else if( ch == '*' )
         {
            if( !next_part.finish_ref )
            {
               if( next_part.literal.length( ) > 1 )
               {
                  next_part.literal.erase( next_part.literal.length( ) - 1 );
#ifdef DEBUG
                  ostringstream osstr;
                  osstr << "[at 8: part #" << ( parts.size( ) + 1 ) << "]";
                  dump_state( osstr.str( ), ch, last_ch, ch_used, is_range, had_empty, had_range, is_matches,
                   is_in_set, was_in_set, had_escape, has_minimum, has_maximum, set_started, is_set_matches );
#endif
                  parts.push_back( next_part );
                  next_part.clear( );

                  next_part.literal = last_ch;
               }
            }

            next_part.min_matches = next_part.max_matches = 0;
#ifdef DEBUG
            ostringstream osstr;
            osstr << "[at 9: part #" << ( parts.size( ) + 1 ) << "]";
            dump_state( osstr.str( ), ch, last_ch, ch_used, is_range, had_empty, had_range, is_matches,
             is_in_set, was_in_set, had_escape, has_minimum, has_maximum, set_started, is_set_matches );
#endif
            parts.push_back( next_part );
            next_part.clear( );

            ch_used = true;
            had_empty = false;
         }
         else if( ch == '{' )
         {
            if( next_ch < '0' || next_ch > '9' )
               throw runtime_error( "invalid matches in: " + expr );

            if( !next_part.finish_ref && next_part.literal.length( ) > 1 )
            {
               next_part.literal.erase( next_part.literal.length( ) - 1 );
#ifdef DEBUG
               ostringstream osstr;
               osstr << "[at 10: part #" << ( parts.size( ) + 1 ) << "]";
               dump_state( osstr.str( ), ch, last_ch, ch_used, is_range, had_empty, had_range, is_matches,
                is_in_set, was_in_set, had_escape, has_minimum, has_maximum, set_started, is_set_matches );
#endif
               parts.push_back( next_part );
               next_part.clear( );

               next_part.literal = last_ch;
            }
  
            had_empty = false;
            is_matches = true;
            next_part.min_matches = 0;
            next_part.max_matches = 0;
         }
         else if( ch == '?' )
         {
            if( next_part.finish_ref )
            {
               next_part.min_matches = 0;
               next_part.max_matches = 1;
            }

            if( next_part.literal.length( ) > 1 )
            {
               if( !next_part.finish_ref )
                  next_part.literal.erase( next_part.literal.length( ) - 1 );
#ifdef DEBUG
               ostringstream osstr;
               osstr << "[at 11: part #" << ( parts.size( ) + 1 ) << "]";
               dump_state( osstr.str( ), ch, last_ch, ch_used, is_range, had_empty, had_range, is_matches,
                is_in_set, was_in_set, had_escape, has_minimum, has_maximum, set_started, is_set_matches );
#endif
               parts.push_back( next_part );
               next_part.clear( );

               if( !next_part.finish_ref && last_ch != ')' )
                  next_part.literal = last_ch;
            }

            if( parts.empty( ) || !parts.back( ).finish_ref )
            {
               next_part.min_matches = 0;
               next_part.max_matches = 1;

               parts.push_back( next_part );
               next_part.clear( );
            }

            ch_used = true;
         }
         else if( is_matches )
         {
            if( ch >= '0' && ch <= '9' )
            {
               ch_used = true;

               if( !has_minimum )
                  next_part.min_matches = ( 10 * next_part.min_matches ) + ( ch - '0' );
               else
               {
                  has_maximum = true;
                  next_part.max_matches = ( 10 * next_part.max_matches ) + ( ch - '0' );
               }
            }
            else if( ch == ',' && !has_minimum )
               has_minimum = true;
            else if( ch == '}' )
            {
               ch_used = true;
               is_matches = false;
            }
            else
               throw runtime_error( "invalid matches found in: " + expr );
         }
         else
         {
            if( has_minimum || had_empty || ( ch == '.' && !next_part.literal.empty( ) ) )
            {
#ifdef DEBUG
               ostringstream osstr;
               osstr << "[at 12: part #" << ( parts.size( ) + 1 ) << "]";
               dump_state( osstr.str( ), ch, last_ch, ch_used, is_range, had_empty, had_range, is_matches,
                is_in_set, was_in_set, had_escape, has_minimum, has_maximum, set_started, is_set_matches );
#endif
               parts.push_back( next_part );
               next_part.clear( );

               if( ch == '.' )
                  had_empty = true;
               else
                  had_empty = false;

               has_minimum = false;
               has_maximum = false;
            }

            if( ch != '.' )
            {
               ch_used = true;
               next_part.literal += ch;
            }
         }
      }

      if( !is_range )
         last_ch = ch;
   }

   if( is_in_set && !was_in_set )
      throw runtime_error( "missing set finish in: " + expr );

   if( is_set_matches )
      throw runtime_error( "missing set matches finish in: " + expr );

   if( was_in_set || !next_part.literal.empty( ) )
   {
      if( is_range && !has_maximum )
         next_part.max_matches = next_part.min_matches;

      parts.push_back( next_part );
   }

   if( !ch_used && !expr.empty( ) )
   {
      next_part.clear( );

      char ch = expr[ expr.length( ) - 1 ];
      if( ch != '.' )
         next_part.literal = expr[ expr.length( ) - 1 ];

      parts.push_back( next_part );
   }

#ifdef DEBUG
   cout << "expr: " << expr << "\n\n";
#endif
   for( size_t i = 0; i < parts.size( ); i++ )
   {
      if( parts[ i ].type == e_part_type_lit )
      {
         if( parts[ i ].min_matches && !parts[ i ].literal.empty( ) )
         {
            string lit_prefix( literal_prefix( parts[ i ].literal ) );

            if( parts[ i ].literal[ 0 ] == '\b' )
               prefix_at_boundary = true;
            else
               prefix_at_boundary = false;

            size_t min_matches = parts[ i ].min_matches;
            if( lit_prefix.size( ) != parts[ i ].literal.size( ) )
               min_matches = 1;

            for( size_t j = 0; j < min_matches; j++ )
               prefix += lit_prefix;

            if( parts[ i ].finish_ref
             || lit_prefix.size( ) != parts[ i ].literal.size( )
             || parts[ i ].max_matches != parts[ i ].min_matches )
               break;
         }
         else
            break;
      }
      else
         break;
   }

   for( size_t i = 0; i < parts.size( ); i++ )
   {
      if( parts[ i ].type == e_part_type_lit )
      {
         size_t size = literal_size( parts[ i ].literal );
         if( size == 0 )
            ++size;

         min_size += ( size * parts[ i ].min_matches );
      }
      else
         min_size += ( 1 * parts[ i ].min_matches );

      if( !parts[ i ].max_matches )
         max_size = 0;

      int size = 1;
      if( !parts[ i ].literal.empty( ) )
         size = literal_size( parts[ i ].literal );

      if( max_size )
         max_size += ( size * parts[ i ].max_matches );
   }

   if( max_size )
      --max_size;
#ifdef DEBUG
   cout << "min_size = " << min_size << endl;
   cout << "max_size = " << max_size << endl;
   dump( cout );
#endif
}

string::size_type regex::impl::search(
 const string& text, string::size_type* p_length, vector< string >* p_refs )
{
   if( parts.empty( ) || ( min_size && text.length( ) < min_size ) )
      return string::npos;

#ifdef DEBUG
   cout << "text: " << text << endl;
   dump( cout );
#endif

   // NOTE: If the expression needs to match at the start and starts with a literal
   // or it needs to match at the end and ends with a literal then first check that
   // the literal is present before continuing.
   if( match_at_start && parts[ 0 ].type == e_part_type_lit )
   {
      part& test_part( parts[ 0 ] );

      if( test_part.min_matches )
      {
         string literal;
         for( size_t i = 0; i < test_part.min_matches; i++ )
            literal += test_part.literal;

         if( !match_literal( literal, text, 0 ) )
            return string::npos;
      }
   }

   if( match_at_finish && parts[ parts.size( ) - 1 ].type == e_part_type_lit )
   {
      part& test_part( parts[ parts.size( ) - 1 ] );

      if( test_part.min_matches )
      {
         bool okay = true;

         string literal;
         for( size_t i = 0; i < test_part.min_matches; i++ )
            literal += test_part.literal;

         if( !match_literal( literal, text, text.length( ) - literal_size( literal ) ) )
            return string::npos;
      }
   }

   last_unlimited_part = 0;
   max_size_from_finish = 0;

#ifdef DEBUG
   bool has_unlimited_parts = false;
#endif
   if( match_at_finish )
   {
      for( size_t i = 0; i < parts.size( ); i++ )
      {
         if( !parts[ i ].max_matches )
         {
            last_unlimited_part = i;
#ifdef DEBUG
            has_unlimited_parts = true;
#endif

            max_size_from_finish = 0;
         }
         else
            max_size_from_finish += parts[ i ].max_matches;
      }
   }

#ifdef DEBUG
   if( has_unlimited_parts )
      cout << "last_unlimited_part = #" << ( last_unlimited_part + 1 ) << endl;
   cout << "max_size_from_finish = " << max_size_from_finish << '\n' << endl;
#endif
   string::size_type pos = string::npos;

   // NOTE: If matching at the end but not at the start then start as close
   // to the end as possible and work backwards until has reached the start
   // or has matched.
   if( match_at_finish && !match_at_start )
   {
      string::size_type start = text.size( ) - min_size;

      string::size_type from = start;

      while( true )
      {
         pos = do_search( text, from, p_length, p_refs );

         if( from == 0 || pos != string::npos )
            break;

         --from;
      }
   }
   else
      pos = do_search( text, 0, p_length, p_refs );

   return pos;
}

string::size_type regex::impl::do_search(
 const string& text, size_t start, string::size_type* p_length, vector< string >* p_refs )
{
   bool done = false;
   bool okay = false;

   string::size_type begins = string::npos;
   string::size_type finishes = string::npos;

   refs.clear( );
   node_refs.clear( );

   size_t pf = 0;
   bool had_min = false;
   // NOTE: Need to force all refs to be empty strings in case they are in optional parts.
   for( size_t i = 0; i < parts.size( ); i++ )
   {
      if( parts[ i ].start_ref )
         node_refs[ i ] = "";

      // NOTE: If matching at the end but not at the start then only start matching after
      // any non-optional parts (i.e. preceeding optional parts are ignored in this case).
      if( !had_min && match_at_finish && !match_at_start )
      {
         if( !parts[ i ].min_matches )
            ++pf;
         else
            had_min = true;
      }
   }

   while( !parts.empty( ) )
   {
      size_t part_from = pf;
      size_t last_found = 0;

      size_t ref_starts = 0;
      size_t ref_started = string::npos;
      size_t ref_finished = string::npos;
      size_t ref_started_by = string::npos;

      bool has_matched = false;

      if( !prefix.empty( ) )
      {
         size_t from( start );
         if( prefix_at_boundary && start )
            ++from;

         string::size_type pos = text.find( prefix, from );
         if( pos == string::npos )
            return pos;

         start = pos;
         if( prefix_at_boundary && start > 0 )
            --start;
      }

      bool has_last_set_used = false;
      bool has_last_literal_used = false;

      int remaining_used = 0;
      string last_literal_used;
      size_t last_lit_part_used = 0;
      size_t last_set_part_used = 0;
      bool last_ref_finish_used = false;
      bool last_set_inverted_used = false;
      vector< pair< char, char > > last_set_used;

      for( size_t i = start; i < text.size( ); i++ )
      {
         bool has_last_set = false;
         bool has_last_literal = false;

         int remaining = 0;
         string last_literal;
         size_t last_lit_part = 0;
         size_t last_set_part = 0;
         bool last_ref_finish = false;
         bool last_set_inverted = false;
         vector< pair< char, char > > last_set;

         int matched_last_part = -1;

         size_t old_part_from = part_from;

         for( size_t j = part_from; j < parts.size( ); j++ )
         {
            bool found = false;
            const part& p = parts[ j ];

            if( i >= text.size( ) )
               break;

            size_t li = i;
            bool already_matched = false;

            bool old_okay = okay;
            size_t old_finishes( finishes );

            if( j == matched_last_part )
               already_matched = true;
            else
            {
               if( p.type == e_part_type_lit )
               {
                  string literal( p.literal );
                  for( int x = 1; x < p.min_matches; x++ )
                     literal += p.literal;

                  expand_refs( literal );

                  if( ( literal.empty( )
                   && i + p.min_matches <= text.size( )
                   && text.substr( i, p.min_matches ).find( '\n' ) == string::npos )
                   || match_literal( literal, text, i ) )
                  {
#ifdef DEBUG
                     cout << "matched literal in part #" << ( j + 1 ) << " for " << literal
                      << " at " << i << ( text.length( ) > 50 ? string( ) : " ==> " + text.substr( start, i - start + 1 ) ) << endl;
#endif
                     if( !literal.empty( ) && literal[ 0 ] == '\b' && !is_word_char( text[ i ] ) )
                        ++i;

                     if( !okay )
                        begins = i;

                     okay = true;
                     found = true;

                     last_found = j;
                     has_matched = true;

                     if( p.start_ref && ( ref_started == string::npos || j != ref_started_by ) )
                     {
                        if( ref_started != string::npos && ref_finished != string::npos )
                           node_refs[ ref_starts ] = text.substr( ref_started, ref_finished - ref_started );

                        ref_starts = j;
                        ref_started = i;
                        ref_started_by = j;

                        ref_finished = string::npos;
                        node_refs[ ref_starts ] = "";
                     }

                     part_from = j + 1;

                     if( literal.empty( ) )
                        i += p.min_matches;
                     else
                        i += literal_size( literal );

                     if( p.finish_ref )
                     {
                        ref_finished = i;

                        if( ref_started != string::npos )
                           node_refs[ ref_starts ] = text.substr( ref_started, ref_finished - ref_started );
                     }

                     finishes = i - 1;

                     has_last_set = false;

                     if( p.max_matches != 1 )
                        has_last_literal = true;
                     else
                        has_last_literal = false;

                     last_lit_part = j;

                     last_literal = p.literal;
                     last_ref_finish = p.finish_ref;

                     if( !p.min_matches )
                        remaining = p.max_matches - 1;
                     else
                        remaining = p.max_matches - p.min_matches;
                  }
               }
               else
               {
                  for( size_t x = 0; x < p.matches.size( ); x++ )
                  {
                     if( match_set( text, i, p ) )
                     {
#ifdef DEBUG
                        cout << "matched set in part #" << ( j + 1 )
                         << ( p.inverted ? " (inverted)" : "" ) << " for " << text[ i ]
                         << " at " << i << ( text.length( ) > 50 ? string( ) : " ==> " + text.substr( start, i - start + 1 ) ) << endl;
#endif
                        if( !okay )
                           begins = i;

                        okay = true;
                        found = true;

                        last_found = j;
                        has_matched = true;

                        int min_matches = max( 1, p.min_matches );

                        if( p.start_ref && ( ref_started == string::npos || j != ref_started_by ) )
                        {
                           if( ref_started != string::npos && ref_finished != string::npos )
                              node_refs[ ref_starts ] = text.substr( ref_started, ref_finished - ref_started );

                           ref_starts = j;
                           ref_started = i;
                           ref_started_by = j;

                           ref_finished = string::npos;
                           node_refs[ ref_starts ] = "";
                        }

                        part_from = j + 1;

                        i += min_matches;

                        if( p.finish_ref )
                        {
                           ref_finished = i;

                           if( ref_started != string::npos )
                              node_refs[ ref_starts ] = text.substr( ref_started, ref_finished - ref_started );
                        }

                        finishes = i - 1;

                        if( p.max_matches != 1 )
                           has_last_set = true;
                        else
                           has_last_set = false;

                        has_last_literal = false;

                        last_set_part = j;

                        last_set = p.matches;
                        last_ref_finish = p.finish_ref;
                        last_set_inverted = p.inverted;

                        if( !p.min_matches )
                           remaining = p.max_matches - 1;
                        else
                           remaining = p.max_matches - p.min_matches;

                        break;
                     }
                  }
               }
            }

            if( !found && ref_started != string::npos && p.finish_ref && p.min_matches == 0 )
            {
               ref_finished = i;
               node_refs[ ref_starts ] = text.substr( ref_started, ref_finished - ref_started );
            }

            bool was_last_part = ( matched_last_part == parts.size( ) - 1 );
            matched_last_part = -1;

            bool force_repeat = false;
            if( found && j < parts.size( ) - 1 && parts[ j + 1 ].inverted )
               force_repeat = true;

            // NOTE: If we need to match at the end and have matched the last unlimited part in
            // the expression then if the length is less than maximum that can follow prefer to
            // keep matching rather than skipping ahead to the next part (otherwise a match may
            // not be found at all).
            if( ( found || already_matched ) && max_size_from_finish
             && j >= last_unlimited_part && li + 1 <= text.size( ) - max_size_from_finish )
               force_repeat = true;

            bool forcing_previous_repeat = false;
            if( !found
             && ( ( has_last_set_used && last_set_part_used == last_found - 1 )
             || ( has_last_literal_used && last_lit_part_used == last_found - 1 ) ) )
               forcing_previous_repeat = true;

            if( force_repeat && forcing_previous_repeat )
            {
               okay = old_okay;
               finishes = old_finishes;
               part_from = old_part_from;
            }

            // NOTE: If we have matched with the last unlimited part but have reached the point
            // where the remaining parts could require all the remaining characters then simply
            // try matching the next part (from the last position) rather than repeating.
            if( found && !force_repeat && already_matched
             && max_size_from_finish && j == last_unlimited_part && li + 1 >= text.size( ) - max_size_from_finish )
            {
               i = li - 1;
               continue;
            }

            if( !found && !force_repeat && !was_last_part && p.min_matches > 0 )
            {
               if( ( has_matched || already_matched ) && j > last_found )
               {
                  --j;
                  force_repeat = true;
               }
               else if( has_matched )
                  continue;
               else
                  break;
            }

            if( force_repeat && forcing_previous_repeat )
            {
#ifdef DEBUG
               cout << "(forcing previous repeat)" << endl;
#endif
               remaining = remaining_used;

               has_last_set = has_last_set_used;
               has_last_literal = has_last_literal_used;

               last_literal = last_literal_used;
               last_lit_part = last_lit_part_used;

               last_set = last_set_used;
               last_set_part = last_set_part_used;
               last_ref_finish = last_ref_finish_used;
               last_set_inverted = last_set_inverted_used;
            }

            // NOTE: The approach to repeat matching is to initially try to match
            // the next part of the expression and only if this fails (or this is
            // the last part of the expression or it has matched the current part
            // and the next part is inverted) then attempt the repeat. Some other
            // more esoteric reasons to "force" repeating are documented above.
            if( okay && ( ( !found && !already_matched ) || force_repeat || j == parts.size( ) - 1 ) )
            {
               if( has_last_literal && remaining != 0
                && match_literal( expand_refs( last_literal ), text, i ) )
               {
#ifdef DEBUG
                  cout << "matched last literal: " << last_literal
                   << " in part #" << ( last_lit_part + 1 ) << " at " << i
                   << ( text.length( ) > 50 ? string( ) : " ==> " + text.substr( start, i - start + 1 ) ) << endl;
#endif
                  j = part_from = matched_last_part = last_lit_part;

                  if( j == parts.size( ) - 1 || p.min_matches > 0 )
                     --j;

                  found = true;

                  if( last_literal.empty( ) )
                     ++i;
                  else
                  {
                     i += literal_size( last_literal );

                     if( !last_literal.empty( ) && last_literal[ 0 ] == '\b' )
                        ++i;
                  }

                  if( was_last_part && i == text.size( ) )
                     done = true;

                  if( last_ref_finish )
                     ref_finished = i;

                  if( ref_started != string::npos && ref_finished != string::npos )
                     node_refs[ ref_starts ] = text.substr( ref_started, ref_finished - ref_started );

                  finishes = i - 1;

                  if( remaining )
                     --remaining;

                  remaining_used = remaining;

                  has_last_set_used = false;
                  has_last_literal_used = true;

                  last_literal_used = last_literal;
                  last_lit_part_used = last_lit_part;
               }

               if( has_last_set && remaining != 0 && i < text.size( ) )
               {
                  for( size_t x = 0; x < last_set.size( ); x++ )
                  {
                     bool matched = false;

                     if( last_set[ x ].first == '\0'
                      || ( text[ i ] >= last_set[ x ].first && text[ i ] <= last_set[ x ].second ) )
                     {
                        if( !last_set_inverted )
                           matched = true;
                     }
                     else if( last_set_inverted )
                        matched = true;

                     if( matched )
                     {
#ifdef DEBUG
                        cout << "matched last set: " << last_set[ x ].first
                         << "-" << last_set[ x ].second << ( last_set_inverted ? "  (inverted)" : "" )
                         << " in part #" << ( last_set_part + 1 ) << " for " << text[ i ] << " at " << i
                         << ( text.length( ) > 50 ? string( ) : " ==> " + text.substr( start, i - start + 1 ) ) << endl;
#endif
                        j = part_from = matched_last_part = last_set_part;

                        if( j == parts.size( ) - 1 || p.min_matches > 0 )
                        {
                           --j;

                           if( was_last_part && i == text.size( ) - 1 )
                              done = true;
                        }

                        found = true;
                        finishes = i++;

                        if( last_ref_finish )
                           ref_finished = i;

                        if( ref_started != string::npos && ref_finished != string::npos )
                           node_refs[ ref_starts ] = text.substr( ref_started, ref_finished - ref_started );

                        if( remaining )
                           --remaining;

                        remaining_used = remaining;

                        has_last_set_used = true;
                        has_last_literal_used = false;

                        last_set_used = last_set;
                        last_set_part_used = last_set_part;
                        last_ref_finish_used = last_ref_finish;
                        last_set_inverted_used = last_set_inverted;

                        break;
                     }
                  }
               }
            }

            if( !found && ( p.min_matches || j == parts.size( ) - 1 ) )
            {
               if( !has_matched || ( j > last_found && p.min_matches ) )
                  okay = false;

               part_from = 0;
               has_last_set = false;
               has_last_literal = false;

               break;
            }
         }

         if( match_at_start && ( !okay || begins != 0 ) )
            break;

         if( done || part_from >= parts.size( ) )
            break;

         if( i > finishes && i < text.size( ) - 1 )
         {
            okay = false;
            break;
         }
      }

      if( ref_started != string::npos && ref_finished != string::npos )
         node_refs[ ref_starts ] = text.substr( ref_started, ref_finished - ref_started );

      if( okay && !done )
      {
         for( size_t j = part_from; j < parts.size( ); j++ )
         {
            if( parts[ j ].min_matches )
            {
               // NOTE: If the finish position is the last character and the last
               // part is just a boundary match character then is still matching.
               // Also if the last part is both required and inverted then it can
               // only succeed if it doesn't match.
               if( j != parts.size( ) - 1 || finishes != text.length( ) - 1
                || ( parts[ j ].type == e_part_type_lit && parts[ j ].literal != "\b" )
                || ( parts[ j ].type == e_part_type_set && parts[ j ].inverted
                && ( text.size( ) < parts[ j ].min_matches
                || !match_set( text, text.size( ) - parts[ j ].min_matches, parts[ j ] ) ) ) )
               {
                  okay = false;
                  break;
               }
            }
         }
      }

      if( okay || ( match_at_start && begins == string::npos ) )
         break;

      if( !text.size( ) || start++ >= text.size( ) - 1 )
         break;
#ifdef DEBUG
       cout << "retry match from pos: " << start << endl;
#endif
   }

   if( okay && match_at_finish && finishes != text.size( ) - 1 )
      okay = false;

   // NOTE: If the last literal ends with a boundary match character
   // and the last matched character is a non-word character then it
   // will be necessary to remove the last character from the match.
   if( okay && parts.size( ) )
   {
      size_t last_part = parts.size( ) - 1;
      if( okay && parts[ last_part ].type == e_part_type_lit )
      {
         string last_literal = parts[ last_part ].literal;

         if( !last_literal.empty( )
          && last_literal[ last_literal.size( ) - 1 ] == '\b' && !is_word_char( text[ finishes ] ) )
            --finishes;
      }
   }

   if( okay && p_length )
      *p_length = finishes - begins + 1;

   if( okay && p_refs )
   {
      for( map< int, string >::iterator i = node_refs.begin( ); i != node_refs.end( ); ++i )
         refs.push_back( i->second );

      *p_refs = refs;
   }

   return okay ? begins : string::npos;
}

bool regex::impl::match_set( const string& text, size_t off, const part& p )
{
   bool okay = false;
   int rounds = p.min_matches;
   while( true )
   {
      okay = false;
      for( size_t x = 0; x < p.matches.size( ); x++ )
      {
         if( p.matches[ x ].first == '\0'
          || ( text[ off ] >= p.matches[ x ].first && text[ off ] <= p.matches[ x ].second ) )
         {
            okay = true;
            break;
         }
      }

      if( p.inverted )
         okay = !okay;

      if( !okay )
         break;

      if( --rounds < 1 )
         break;

      if( ++off > text.length( ) - 1 )
         break;
   }

   return okay && rounds <= 0;
}

string& regex::impl::expand_refs( string& literal )
{
   int j = 0;
   for( map< int, string >::iterator i = node_refs.begin( ); i != node_refs.end( ); ++i )
   {
      string s( "\\" + to_string( ++j ) );
      replace( literal, s, i->second );
   }

   return literal;
}

void regex::impl::dump( ostream& os )
{
   os << "\nexpr: " << expr << '\n';

   for( size_t i = 0; i < parts.size( ); i++ )
   {
      os << "\n[part #" << ( i + 1 ) << "]\n";
      os << "type: " << ( parts[ i ].type == e_part_type_lit ? "lit" : "set" ) << "\n";

      if( parts[ i ].type == e_part_type_lit )
         os << "literal: " << parts[ i ].literal << '\n';
      else if( parts[ i ].inverted )
         os << "*inverted*\n";

      if( parts[ i ].start_ref )
         os << "(start ref)\n";
      if( parts[ i ].finish_ref )
         os << "(finish ref)\n";

      os << "min_matches: " << parts[ i ].min_matches << '\n';
      os << "max_matches: " << parts[ i ].max_matches << '\n';

      if( parts[ i ].type != e_part_type_lit )
      {
         for( size_t j = 0; j < parts[ i ].matches.size( ); j++ )
         {
            os << "set match from: "
             << parts[ i ].matches[ j ].first << " to: " << parts[ i ].matches[ j ].second << '\n';
         }
      }
   }

   os << "\nmatch_at_start = " << match_at_start << '\n';
   os << "match_at_finish = " << match_at_finish << '\n';

   os << endl;
}

regex::regex( const string& expr )
{
#ifdef DEBUG
   try
   {
#endif
      p_impl = new regex::impl( expr );
#ifdef DEBUG
   }
   catch( exception& x )
   {
      cout << "error: " << x.what( ) << endl;
      throw;
   }
#endif
}

regex::~regex( )
{
   delete p_impl;
}

string regex::get_expr( ) const
{
   return p_impl->expr;
}

int regex::get_min_size( ) const
{
   return p_impl->min_size;
}

int regex::get_max_size( ) const
{
   return p_impl->max_size;
}

string::size_type regex::search( const string& text, string::size_type* p_length, vector< string >* p_refs )
{
   return p_impl->search( text, p_length, p_refs );
}

void regex::dump( ostream& os )
{
   p_impl->dump( os );
}

#ifdef COMPILE_TESTBED_MAIN
int main( )
{
   try
   {
      string input;
      cout << "input regex: ";
      cin >> input;

      regex expr( input );

      expr.dump( cout );

      cout << "min_size = " << expr.get_min_size( ) << endl;
      cout << "max_size = " << expr.get_max_size( ) << endl;

      cout << "\ninput text to match: ";
      cin >> input;

      cout << "\nregex is: " << expr.get_input( )
       << "\nsearching: " << input << "\n\n";

      string::size_type length;
      string::size_type pos = expr.search( input, &length );
      if( pos != string::npos )
         cout << "matched: " << input.substr( pos, length );
      else
         cout << "*** no match ***" << endl;
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
   }
}
#endif

