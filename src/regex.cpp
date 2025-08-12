// Copyright (c) 2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
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
    finish_ref( false ),
    has_back_refs( false )
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

      has_back_refs = false;

      matches.clear( );
   }

   part_type type;

   string literal;

   size_t min_matches;
   size_t max_matches;

   bool inverted;

   bool start_ref;
   bool finish_ref;

   bool has_back_refs;

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
      if( ( lit[ i ] != '\\' ) && ( lit[ i ] != '\b' ) )
         ++len;
   }

   return len;
}

string literal_prefix( const string& lit )
{
   string s;

   for( size_t i = 0; i < lit.length( ); i++ )
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
         if( ( i == ( lit.length( ) - 2 ) ) && ( lit[ lit.length( ) - 1 ] == '\b' ) )
            return true;

         return ( i == ( lit.length( ) - 1 ) );
      }
   }

   return true;
}

struct search_state
{
   search_state( )
    :
    ch( '\0' ),
    last_ch( '\0' ),
    ch_used( false ),
    is_range( false ),
    had_empty( false ),
    had_range( false ),
    is_in_ref( false ),
    is_in_set( false ),
    was_in_set( false ),
    is_matches( false ),
    had_escape( false ),
    has_minimum( false ),
    has_maximum( false ),
    set_started( false ),
    had_finish_ref( false ),
    is_set_matches( false )
   {
   }

   char ch;
   char last_ch;

   bool ch_used;
   bool is_range;
   bool had_empty;
   bool had_range;
   bool is_in_ref;
   bool is_in_set;
   bool was_in_set;
   bool is_matches;
   bool had_escape;
   bool has_minimum;
   bool has_maximum;
   bool set_started;
   bool had_finish_ref;
   bool is_set_matches;
};

#ifdef DEBUG
void dump_state( const string& msg, const search_state& s, size_t offset )
{
   if( !msg.empty( ) )
      cout << '[' << msg << ": part #" << ( offset + 1 ) << ']' << endl;

   cout << "ch = " << s.ch << endl;
   cout << "last_ch = " << s.last_ch << endl;

   cout << "ch_used = " << s.ch_used << endl;
   cout << "is_range = " << s.is_range << endl;
   cout << "had_empty = " << s.had_empty << endl;
   cout << "had_range = " << s.had_range << endl;
   cout << "is_in_ref = " << s.is_in_ref << endl;
   cout << "is_in_set = " << s.is_in_set << endl;
   cout << "was_in_set = " << s.was_in_set << endl;
   cout << "is_matches = " << s.is_matches << endl;
   cout << "had_escape = " << s.had_escape << endl;
   cout << "has_minimum = " << s.has_minimum << endl;
   cout << "has_maximum = " << s.has_maximum << endl;
   cout << "set_started = " << s.set_started << endl;
   cout << "had_finish_ref = " << s.had_finish_ref << endl;
   cout << "is_set_matches = " << s.is_set_matches << endl;
}
#endif

}

struct regex::impl
{
   impl( const string& expr, bool match_at_start, bool match_at_finish );

   string get_expr( ) const { return expr; }

   size_t get_min_size( ) const { return min_size; }
   size_t get_max_size( ) const { return max_size; }

   string::size_type search( const string& text,
    string::size_type* p_length, vector< string >* p_refs );

   string::size_type do_search( const string& text, size_t start,
    string::size_type* p_length, vector< string >* p_refs, size_t* p_last_part_matched = 0 );

   bool match_set( const string& text, size_t off, const part& p );

   string& expand_refs( string& literal );

   void dump( ostream& os );

   string expr;
   string prefix;

   size_t min_size;
   size_t max_size;

   bool match_at_start;
   bool match_at_finish;

   bool prefix_at_boundary;

   size_t last_unlimited_part;

   size_t min_size_from_finish;
   size_t max_size_from_finish;

   vector< part > parts;

   vector< string > ref_vals;

   map< int, string > part_refs;
};

regex::impl::impl( const string& expr, bool match_at_start, bool match_at_finish )
 :
 expr( expr ),
 min_size( 0 ),
 max_size( 1 ),
 match_at_start( match_at_start ),
 match_at_finish( match_at_finish ),
 prefix_at_boundary( false ),
 last_unlimited_part( 0 ),
 min_size_from_finish( 0 ),
 max_size_from_finish( 0 )
{
   part next_part;

   search_state s;

   size_t start = 0;

   for( size_t i = 0; i < expr.length( ); i++ )
   {
      s.ch = expr[ i ];

      // NOTE: Although "match_at_start" can be
      // defaulted via the constructor argument
      // it can also be explicitly provided (as
      // can "match_at_finish") as a character.
      if( ( i == 0 ) && ( s.ch == '^' ) )
      {
         start++;

         this->match_at_start = true;

         continue;
      }

      char next_ch = '\0';

      if( i < ( expr.length( ) - 1 ) )
         next_ch = expr[ i + 1 ];

      s.ch_used = false;

      if( s.had_escape )
      {
         s.had_escape = false;

         switch( s.ch )
         {
            case 'a':
            s.ch = '\a';
            break;

            case 'b':
            s.ch = '\b';
            break;

            case 'e':
            s.ch = 0x27;
            break;

            case 'f':
            s.ch = '\f';
            break;

            case 'n':
            s.ch = '\n';
            break;

            case 'r':
            s.ch = '\r';
            break;

            case 't':
            s.ch = '\t';
            break;

            case 'v':
            s.ch = '\v';
            break;
         }

         if( ( s.ch == 'x' )
          && ( i < ( expr.length( ) - 1 ) ) )
         {
            s.ch = hex_nibble( expr[ i + 1 ] );

            s.ch <<= 4;

            s.ch += hex_nibble( expr[ i + 2 ] );

            i += 2;

            if( i < ( expr.length( ) - 1 ) )
               next_ch = expr[ i + 1 ];
            else
               next_ch = '\0';
         }

         if( s.is_in_set )
         {
            s.ch_used = true;

            if( s.is_range )
            {
               s.had_range = true;
               next_part.matches.push_back( make_pair( s.last_ch, s.last_ch ) );
            }
            else if( is_special_set( s.ch ) )
            {
               add_special_set( next_part, s.ch );

               continue;
            }
            else
               next_part.matches.push_back( make_pair( s.last_ch, s.last_ch ) );

            s.is_range = false;
         }
         else
         {
            if( s.was_in_set || s.has_minimum
             || ( is_special_set( s.ch ) && !next_part.literal.empty( ) ) )
            {
#ifdef DEBUG
               dump_state( "at 1", s, parts.size( ) );
#endif
               parts.push_back( next_part );

               next_part.clear( );

               s.was_in_set = false;

               s.has_minimum = false;
               s.has_maximum = false;

               s.had_finish_ref = false;
            }

            s.ch_used = true;

            // NOTE: Ensure that back-references are not being unescaped.
            if( ( s.ch >= '1' ) && ( s.ch <= '9' ) )
            {
               if( parts.empty( ) )
                  throw runtime_error( "invalid back-reference found in the initial part" );

               next_part.literal += '\\';
               next_part.literal += s.ch;

               next_part.has_back_refs = true;
            }
            else if( is_special_set( s.ch ) )
            {
               add_special_set( next_part, s.ch );

               if( i != ( expr.length( ) - 1 ) )
               {
#ifdef DEBUG
                  dump_state( "at 2", s, parts.size( ) );
#endif
                  parts.push_back( next_part );

                  next_part.clear( );

                  s.was_in_set = false;

                  s.has_minimum = false;
                  s.has_maximum = false;

                  s.had_finish_ref = false;
               }
            }
            else
               next_part.literal += s.ch;
         }
      }
      else if( s.ch == '\\' )
      {
         if( s.is_matches || s.is_set_matches )
            throw runtime_error( "invalid escaping for matches in: " + expr );

         s.had_escape = true;

         continue;
      }
      else
      {
         if( ( s.ch == '$' ) && ( i == ( expr.length( ) - 1 ) ) )
         {
            s.ch_used = true;

            this->match_at_finish = true;

            break;
         }

         if( s.ch == '(' )
         {
            if( s.is_in_ref )
               throw runtime_error( "invalid nested ref in: " + expr );

            if( ( i != start )
             && ( ( s.last_ch == '.' ) || !next_part.literal.empty( ) ) )
            {
#ifdef DEBUG
               dump_state( "at 3", s, parts.size( ) );
#endif
               parts.push_back( next_part );

               next_part.clear( );

               s.was_in_set = false;

               s.has_minimum = false;
               s.has_maximum = false;

               s.had_finish_ref = false;
            }
            else if( s.had_finish_ref && !parts.empty( )
             && !parts[ parts.size( ) - 1 ].finish_ref )
               parts[ parts.size( ) - 1 ].finish_ref = true;

            s.had_finish_ref = false;

            s.is_in_ref = next_part.start_ref = true;
         }
         else if( s.ch == ')' )
         {
            if( !s.is_in_ref )
               throw runtime_error( "invalid finish ref in: " + expr );

            s.ch_used = true;

            s.is_in_ref = false;

            s.had_finish_ref = true;

            if( parts.empty( ) || next_part.start_ref )
               next_part.finish_ref = true;
            else if( i != ( expr.length( ) - 1 ) )
               parts[ parts.size( ) - 1 ].finish_ref = true;
         }
         else if( s.ch == '[' )
         {
            if( s.is_in_set )
               throw runtime_error( "invalid set start in: " + expr );

            if( ( i != start ) && ( s.last_ch != '*' ) && ( s.last_ch != '+' )
             && ( s.last_ch != '?' ) && ( s.last_ch != '(' ) && ( s.last_ch != ')' ) )
            {
#ifdef DEBUG
               dump_state( "at 4", s, parts.size( ) );
#endif
               parts.push_back( next_part );

               next_part.clear( );

               s.was_in_set = false;

               s.has_minimum = false;
               s.has_maximum = false;

               s.had_finish_ref = false;
            }

            if( next_ch == '^' )
            {
               next_part.inverted = true;

               ++i;
            }

            s.is_range = false;
            s.is_in_set = true;

            s.had_empty = false;
            s.was_in_set = false;
            s.set_started = true;

            next_part.type = e_part_type_set;
         }
         else if( s.ch == ']' )
         {
            if( !s.is_in_set )
               throw runtime_error( "invalid set finish in: " + expr );

            if( !s.had_range && ( s.last_ch != '[' ) )
               next_part.matches.push_back( make_pair( s.last_ch, s.last_ch ) );

            s.ch_used = true;
            s.is_range = false;
            s.had_range = false;

            s.is_in_set = false;
            s.was_in_set = true;
            s.set_started = false;

            if( next_part.matches.empty( ) )
               throw runtime_error( "invalid empty set in: " + expr );
         }
         else if( s.is_in_set )
         {
            if( s.set_started || s.had_range )
            {
               s.had_range = false;
               s.set_started = false;
            }
            else
            {
               if( ( s.ch == '-' ) && ( next_ch != ']' ) )
                  s.is_range = true;
               else
               {
                  if( s.is_range )
                  {
                     s.had_range = true;
                     next_part.matches.push_back( make_pair( s.last_ch, s.ch ) );
                  }
                  else
                     next_part.matches.push_back( make_pair( s.last_ch, s.last_ch ) );

                  s.is_range = false;
               }
            }
         }
         else if( s.was_in_set )
         {
            if( s.is_set_matches )
            {
               if( ( s.ch < '0' ) || ( s.ch > '9' ) )
               {
                  if( ( s.ch == ',' ) && !s.has_minimum )
                     s.has_minimum = true;
                  else if( s.ch != '}' )
                     throw runtime_error( "invalid set matches in: " + expr );
                  else
                  {
                     s.ch_used = true;

                     if( ( s.last_ch != ',' ) && !s.has_maximum )
                        next_part.max_matches = next_part.min_matches;

                     s.is_set_matches = false;
                  }
               }
               else
               {
                  s.ch_used = true;

                  if( !s.has_minimum )
                     next_part.min_matches = ( 10 * next_part.min_matches ) + ( s.ch - '0' );
                  else
                  {
                     s.has_maximum = true;
                     next_part.max_matches = ( 10 * next_part.max_matches ) + ( s.ch - '0' );
                  }
               }
            }
            else
            {
               if( s.ch == '*' )
                  next_part.min_matches = next_part.max_matches = 0;
               else if( s.ch == '+' )
               {
                  next_part.min_matches = 1;
                  next_part.max_matches = 0;
               }

               s.ch_used = true;

               if( s.ch == '{' )
               {
                  if( ( next_ch < '0' ) || ( next_ch > '9' ) )
                     throw runtime_error( "invalid set matches in: " + expr );

                  s.is_set_matches = true;

                  next_part.min_matches = 0;
                  next_part.max_matches = 0;
               }
               else
               {
                  if( s.ch == '?' )
                  {
                     next_part.min_matches = 0;
                     next_part.max_matches = 1;
                  }
                  else if( s.has_minimum && !s.has_maximum )
                     next_part.max_matches = next_part.min_matches;

#ifdef DEBUG
                  dump_state( "at 5", s, parts.size( ) );
#endif
                  parts.push_back( next_part );

                  next_part.clear( );

                  s.was_in_set = false;

                  s.has_minimum = false;
                  s.has_maximum = false;

                  s.had_finish_ref = false;

                  if( s.ch == '.' )
                     s.had_empty = true;
                  else if( ( s.ch != '[' ) && ( s.ch != '*' ) && ( s.ch != '+' ) && ( s.ch != '?' ) )
                     next_part.literal += s.ch;
               }
            }
         }
         else if( s.ch == '+' )
         {
            if( !next_part.finish_ref )
            {
               if( ( next_part.literal.length( ) > 1 ) && ( next_part.literal[ 0 ] != '\\' ) )
               {
                  next_part.literal.erase( next_part.literal.length( ) - 1 );
#ifdef DEBUG
                  dump_state( "at 6", s, parts.size( ) );
#endif
                  parts.push_back( next_part );

                  next_part.clear( );

                  s.was_in_set = false;

                  s.has_minimum = false;
                  s.has_maximum = false;

                  s.had_finish_ref = false;

                  next_part.literal = s.last_ch;
               }
               else if( s.has_minimum )
               {
#ifdef DEBUG
                  dump_state( "at 7", s, parts.size( ) );
#endif
                  parts.push_back( next_part );

                  next_part.clear( );

                  s.was_in_set = false;

                  s.has_minimum = false;
                  s.has_maximum = false;

                  s.had_finish_ref = false;

                  if( s.last_ch != '.' )
                     next_part.literal = s.last_ch;
               }
            }

            next_part.max_matches = 0;
#ifdef DEBUG
            dump_state( "at 8", s, parts.size( ) );
#endif
            parts.push_back( next_part );

            next_part.clear( );

            s.ch_used = true;
            s.had_empty = false;

            s.was_in_set = false;

            s.has_minimum = false;
            s.has_maximum = false;

            s.had_finish_ref = false;
         }
         else if( s.ch == '*' )
         {
            if( !next_part.finish_ref )
            {
               if( next_part.literal.length( ) > 1 )
               {
                  next_part.literal.erase( next_part.literal.length( ) - 1 );
#ifdef DEBUG
                  dump_state( "at 9", s, parts.size( ) );
#endif
                  parts.push_back( next_part );

                  next_part.clear( );

                  s.was_in_set = false;

                  s.has_minimum = false;
                  s.has_maximum = false;

                  s.had_finish_ref = false;

                  next_part.literal = s.last_ch;
               }
            }

            next_part.min_matches = next_part.max_matches = 0;
#ifdef DEBUG
            dump_state( "at 10", s, parts.size( ) );
#endif
            parts.push_back( next_part );

            next_part.clear( );

            s.ch_used = true;
            s.had_empty = false;

            s.was_in_set = false;

            s.has_minimum = false;
            s.has_maximum = false;

            s.had_finish_ref = false;
         }
         else if( s.ch == '{' )
         {
            if( ( next_ch < '0' ) || ( next_ch > '9' ) )
               throw runtime_error( "invalid matches in: " + expr );

            if( !next_part.finish_ref && ( next_part.literal.length( ) > 1 ) )
            {
               next_part.literal.erase( next_part.literal.length( ) - 1 );
#ifdef DEBUG
               dump_state( "at 11", s, parts.size( ) );
#endif
               parts.push_back( next_part );

               next_part.clear( );

               s.was_in_set = false;

               s.has_minimum = false;
               s.has_maximum = false;

               s.had_finish_ref = false;

               next_part.literal = s.last_ch;
            }
  
            s.had_empty = false;
            s.is_matches = true;

            next_part.min_matches = 0;
            next_part.max_matches = 0;
         }
         else if( s.ch == '?' )
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
               dump_state( "at 12", s, parts.size( ) );
#endif
               parts.push_back( next_part );

               next_part.clear( );

               s.was_in_set = false;

               s.has_minimum = false;
               s.has_maximum = false;

               s.had_finish_ref = false;
            }

            if( parts.empty( ) || !parts.back( ).finish_ref )
            {
               next_part.min_matches = 0;
               next_part.max_matches = 1;

#ifdef DEBUG
               dump_state( "at 13", s, parts.size( ) );
#endif
               parts.push_back( next_part );

               next_part.clear( );

               s.was_in_set = false;

               s.has_minimum = false;
               s.has_maximum = false;

               s.had_finish_ref = false;
            }

            s.ch_used = true;
         }
         else if( s.is_matches )
         {
            if( ( s.ch >= '0' ) && ( s.ch <= '9' ) )
            {
               s.ch_used = true;

               if( !s.has_minimum )
                  next_part.min_matches = ( 10 * next_part.min_matches ) + ( s.ch - '0' );
               else
               {
                  s.has_maximum = true;
                  next_part.max_matches = ( 10 * next_part.max_matches ) + ( s.ch - '0' );
               }
            }
            else if( ( s.ch == ',' ) && !s.has_minimum )
               s.has_minimum = true;
            else if( s.ch == '}' )
            {
               s.ch_used = true;
               s.is_matches = false;
            }
            else
               throw runtime_error( "invalid matches found in: " + expr );
         }
         else
         {
            if( s.has_minimum || s.had_empty
             || ( ( s.ch == '.' ) && !next_part.literal.empty( ) ) )
            {
#ifdef DEBUG
               dump_state( "at 14", s, parts.size( ) );
#endif
               parts.push_back( next_part );

               next_part.clear( );

               if( s.ch == '.' )
                  s.had_empty = true;
               else
                  s.had_empty = false;

               s.was_in_set = false;

               s.has_minimum = false;
               s.has_maximum = false;

               s.had_finish_ref = false;
            }

            if( s.ch != '.' )
            {
               s.ch_used = true;
               next_part.literal += s.ch;
            }
         }
      }

      if( !s.is_range )
         s.last_ch = s.ch;
   }

   if( s.is_in_set && !s.was_in_set )
      throw runtime_error( "missing set finish in: " + expr );

   if( s.is_set_matches )
      throw runtime_error( "missing set matches finish in: " + expr );

   if( s.was_in_set || !next_part.literal.empty( ) )
   {
      if( s.is_range && !s.has_maximum )
         next_part.max_matches = next_part.min_matches;

      if( s.had_finish_ref )
         next_part.finish_ref = true;

#ifdef DEBUG
      dump_state( "at 15", s, parts.size( ) );
#endif
      parts.push_back( next_part );

      s.had_finish_ref = false;
   }

   if( !s.ch_used && !expr.empty( ) )
   {
      next_part.clear( );

      s.ch = expr[ expr.length( ) - 1 ];

      if( s.ch != '.' )
         next_part.literal = expr[ expr.length( ) - 1 ];

#ifdef DEBUG
      dump_state( "at 16", s, parts.size( ) );
#endif
      parts.push_back( next_part );

      s.had_finish_ref = false;
   }

   if( s.had_finish_ref && parts.size( )
    && !parts[ parts.size( ) - 1 ].finish_ref )
      parts[ parts.size( ) - 1 ].finish_ref = true;

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

            if( lit_prefix.length( ) != parts[ i ].literal.length( ) )
               min_matches = 1;

            for( size_t j = 0; j < min_matches; j++ )
               prefix += lit_prefix;

            if( parts[ i ].finish_ref
             || ( lit_prefix.length( ) != parts[ i ].literal.length( ) )
             || ( parts[ i ].max_matches != parts[ i ].min_matches ) )
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
   cout << "\nmin_size = " << min_size << endl;
   cout << "max_size = " << max_size << endl;
#endif
}

string::size_type regex::impl::search(
 const string& text, string::size_type* p_length, vector< string >* p_refs )
{
   if( parts.empty( ) || ( min_size && ( text.length( ) < min_size ) ) )
      return string::npos;

#ifdef DEBUG
   cout << "\ntext: " << text << endl;
   dump( cout );
#endif

   // NOTE: If the expression needs to match at the start and starts with a literal
   // or it needs to match at the end and ends with a literal then first check that
   // the literal is present before continuing.
   if( match_at_start && ( parts[ 0 ].type == e_part_type_lit ) )
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

   if( match_at_finish
    && ( parts[ parts.size( ) - 1 ].type == e_part_type_lit ) )
   {
      part& test_part( parts[ parts.size( ) - 1 ] );

      // NOTE: If a literal contains any back-references then
      // its size cannot be known in advance so will skip the
      // check (this is not relevant for the "match at start"
      // check above as a back-reference would not make sense
      // to be in the first part).
      if( test_part.min_matches && !test_part.has_back_refs )
      {
         string literal;

         for( size_t i = 0; i < test_part.min_matches; i++ )
            literal += test_part.literal;

         if( !match_literal( literal, text, ( text.length( ) - literal_size( literal ) ) ) )
            return string::npos;
      }
   }

   size_t last_part_matched = 0;
   size_t last_part_to_match = 0;

   last_unlimited_part = 0;

   min_size_from_finish = 0;
   max_size_from_finish = 0;

   bool has_optional_parts = false;

#ifdef DEBUG
   bool has_unlimited_parts = false;
#endif

   for( size_t i = 0; i < parts.size( ); i++ )
   {
      if( parts[ i ].min_matches )
         last_part_to_match = i;
      else
         has_optional_parts = true;

      if( match_at_finish )
      {
         if( !parts[ i ].max_matches )
         {
            last_unlimited_part = i;
#ifdef DEBUG
            has_unlimited_parts = true;
#endif

            min_size_from_finish = 0;
            max_size_from_finish = 0;
         }
         else
         {
            min_size_from_finish += parts[ i ].min_matches;
            max_size_from_finish += parts[ i ].max_matches;
         }
      }
   }

#ifdef DEBUG
   cout << "last_part_to_match = #" << ( last_part_to_match + 1 ) << endl;

   if( has_unlimited_parts )
      cout << "last_unlimited_part = #" << ( last_unlimited_part + 1 ) << endl;

   cout << "\nmin_size_from_finish = " << min_size_from_finish
    << '\n' << "max_size_from_finish = " << max_size_from_finish << endl;
#endif

   bool had_refs_or_length = false;

   if( p_refs || p_length )
      had_refs_or_length = true;

   string::size_type dummy_length = 0;

   if( !p_length )
      p_length = &dummy_length;

   string::size_type pos = string::npos;

   // NOTE: If matching at the end but not at the start then start as close
   // to the end as possible and work backwards until has reached the start
   // or has matched.
   if( match_at_finish && !match_at_start )
   {
      string::size_type start = ( text.length( ) - min_size );

      string::size_type from = start;

      while( true )
      {
         last_part_matched = 0;

         pos = do_search( text, from, p_length, p_refs, &last_part_matched );

         if( !from || ( pos != string::npos ) )
            break;

         --from;
      }
   }
   else
      pos = do_search( text, 0, p_length, p_refs, &last_part_matched );

   if( last_part_matched < last_part_to_match )
   {
#ifdef DEBUG
      if( pos != string::npos )
         cout << "\nlast_part_matched = #" << ( last_part_matched + 1 ) << endl;
#endif
      pos = string::npos;
   }

   // NOTE: Due to the way that parts are processed when searching
   // the initial match might include optional parts that could be
   // omitted so will copy the original text from its start to the
   // the last character included in the matching expression range
   // and will loop trying to match with the final character being
   // removed in each iteration (stopping when it fails to match).
   // Finally if a shorter match was found then need to repeat for
   // a final time to set "pos", "p_length" and "p_refs".
   //
   // FUTURE: With a few changes the final repeat could be removed
   // and the part processing approach should be reviewed to check
   // if these repeat searches can become redundant.
   if( p_length
    && ( *p_length > min_size )
    && !match_at_finish && has_optional_parts
    && had_refs_or_length && ( pos != string::npos ) )
   {
      string text_copy( text, 0, pos + *p_length );

      string text_final( text_copy );

      string last_okay;

      size_t copy_last_part_matched = 0;

      string::size_type cpos = string::npos;

      while( text_copy.length( ) > 1 )
      {
         text_copy.erase( text_copy.length( ) - 1 );

         cpos = do_search( text_copy, 0, 0, 0, &copy_last_part_matched );

         if( copy_last_part_matched < last_part_to_match )
            cpos = string::npos;

         if( cpos == string::npos )
            break;
         else
            text_final = text_final.erase( text_final.length( ) - 1 );
      }

      if( text.length( ) != text_final.length( ) )
      {
         if( p_refs )
            p_refs->clear( );

         *p_length = 0;

         pos = do_search( text_final, 0, p_length, p_refs );
      }
   }

   return pos;
}

string::size_type regex::impl::do_search( const string& text, size_t start,
 string::size_type* p_length, vector< string >* p_refs, size_t* p_last_part_matched )
{
   bool done = false;
   bool okay = false;

   string::size_type begins = string::npos;
   string::size_type finishes = string::npos;

   ref_vals.clear( );
   part_refs.clear( );

   size_t pf = 0;

   bool had_min = false;

   deque< size_t > max_chars_remaining;
   deque< size_t > min_chars_remaining;

#ifdef DEBUG
cout << "\ntext = '" << text << "'" << endl;
#endif
   if( parts.size( ) > 1 )
   {
      bool skip_maximum = false;
      bool skip_minimum = false;

      bool clear_maximums = false;
      bool clear_minimums = false;

      size_t total_max_chars = 0;
      size_t total_min_chars = 0;

      size_t i = ( parts.size( ) - 1 );

      // NOTE: Iterate in reverse from the last
      // part in order to determine the max/min
      // number of characters that will be able
      // to follow after the current part. Each
      // match max/min will be added to a total
      // but once an unlimited max/min has been
      // found will continue with zero values.
      while( true )
      {
         size_t max_matches = parts[ i ].max_matches;
         size_t min_matches = parts[ i ].min_matches;

         bool has_back_refs = parts[ i ].has_back_refs;

         size_t length_multiplier = 1;

         if( parts[ i ].type == e_part_type_lit )
            length_multiplier = literal_size( parts[ i ].literal );

         if( skip_maximum )
            max_chars_remaining.push_front( 0 );
         else
         {
            if( !max_matches || has_back_refs )
            {
               skip_maximum = true;

               total_max_chars = 0;
            }
            else
               total_max_chars += ( max_matches * length_multiplier );

            // NOTE: Do not push any if all are to be skipped.
            if( skip_maximum && max_chars_remaining.empty( ) )
               clear_maximums = true;

            max_chars_remaining.push_front( total_max_chars );
         }

         if( skip_minimum )
            min_chars_remaining.push_front( 0 );
         else
         {
            if( !min_matches || has_back_refs )
            {
               skip_minimum = true;

               total_min_chars = 0;
            }
            else
               total_min_chars += ( min_matches * length_multiplier );

            // NOTE: Do not push any if all are to be skipped.
            if( skip_minimum && min_chars_remaining.empty( ) )
               clear_minimums = true;

            min_chars_remaining.push_front( total_min_chars );
         }

         if( i == 0 )
            break;

         --i;
      }

      if( clear_maximums )
         max_chars_remaining.clear( );

      if( clear_minimums )
         min_chars_remaining.clear( );

#ifdef DEBUG
      if( max_chars_remaining.size( ) )
         cout << '\n';
      for( size_t i = 0; i < max_chars_remaining.size( ); i++ )
         cout << "max_chars_remaining from part #" << ( i + 1 ) << " is " << max_chars_remaining[ i ] << endl;

      if( min_chars_remaining.size( ) )
         cout << '\n';
      for( size_t i = 0; i < min_chars_remaining.size( ); i++ )
         cout << "min_chars_remaining from part #" << ( i + 1 ) << " is " << min_chars_remaining[ i ] << endl;
#endif
   }

   // NOTE: Need to force all refs to be empty strings in case they are in optional parts.
   for( size_t i = 0; i < parts.size( ); i++ )
   {
      if( parts[ i ].start_ref )
         part_refs[ i ] = "";

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

      size_t repeats_used = 0;

      string last_literal_used;

      size_t last_lit_part_used = 0;
      size_t last_set_part_used = 0;

      bool last_ref_finish_used = false;
      bool last_set_inverted_used = false;

      vector< pair< char, char > > last_set_used;

      for( size_t i = start; i < text.length( ); i++ )
      {
         bool has_last_set = false;
         bool has_last_literal = false;

         size_t repeats = 0;

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

            if( i >= text.length( ) )
               break;

            size_t li = i;

            bool already_matched = false;

            bool old_okay = okay;

            size_t old_finishes = finishes;

            bool skip = false;

            // NOTE: If needing to match at the finish then if the total number of unmatched
            // characters remaining is more than the maximum number of possible matches from
            // this part onwards then will skip attempting to match the new part but instead
            // will later force matching with the last part.
            if( match_at_finish && ( j < max_chars_remaining.size( ) )
             && max_chars_remaining[ j ] && ( ( i + max_chars_remaining[ j ] ) < text.length( ) ) )
               skip = true;

            // NOTE: If there are further parts to be considered and the
            // minimum remaining number of characters from the following
            // part would be needed then will skip matching this part if
            // it can be skipped.
            if( !skip
             && !p.min_matches && min_chars_remaining.size( )
             && ( j < min_chars_remaining.size( ) - 1 )
             && ( ( i + min_chars_remaining[ j + 1 ] ) >= text.length( ) ) )
               skip = true;

            if( j == matched_last_part )
               already_matched = true;
            else if( !skip )
            {
               if( p.type == e_part_type_lit )
               {
                  string literal( p.literal );

                  for( int x = 1; x < p.min_matches; x++ )
                     literal += p.literal;

                  expand_refs( literal );

                  if( ( literal.empty( )
                   && ( ( i + p.min_matches ) <= text.length( ) )
                   && ( text.substr( i, p.min_matches ).find( '\n' ) == string::npos ) )
                   || match_literal( literal, text, i ) )
                  {
                     bool ignore_match = false;

                     // NOTE: If needing to match at the end then if a later literal match is found
                     // that would still leave enough room then will simply ignore this match (just
                     // in case it can continue matching with the previous set). Although one would
                     // not normally expect the non-greedy matching to continue it also makes sense
                     // that the matching at the end be non-greedy (in which case the last possible
                     // literal match should be chosen over any earlier one).
                     if( old_okay && match_at_finish
                      && ( text.length( ) > min_size_from_finish )
                      && ( ( j == ( last_unlimited_part + 1 ) )
                      && ( i < ( text.length( ) - min_size_from_finish ) ) ) )
                     {
                        if( literal.empty( ) )
                           ignore_match = true;
                        else
                        {
                           string::size_type next_pos = text.find( literal, i + 1 );

                           if( ( next_pos != string::npos )
                            && ( next_pos <= ( text.length( ) - min_size_from_finish ) ) )
                              ignore_match = true;
                        }
                     }

                     if( !ignore_match )
                     {
                        if( p_last_part_matched )
                           *p_last_part_matched = j;
#ifdef DEBUG
                        if( j == 0 )
                           cout << '\n';
                        cout << "matched literal in part #" << ( j + 1 ) << " for " << literal
                         << " at " << i << ( text.length( ) > 50 ? string( ) : " ==> "
                         + text.substr( start, i - start + literal.length( ) ) ) << endl;
#endif
                        if( !literal.empty( ) && ( literal[ 0 ] == '\b' ) && !is_word_char( text[ i ] ) )
                           ++i;

                        if( !okay )
                           begins = i;

                        okay = true;
                        found = true;

                        last_found = j;

                        has_matched = true;

                        if( p.start_ref
                         && ( ( ref_started == string::npos ) || ( j != ref_started_by ) ) )
                        {
                           if( ( ref_started != string::npos ) && ( ref_finished != string::npos ) )
                              part_refs[ ref_starts ] = text.substr( ref_started, ref_finished - ref_started );

                           ref_starts = j;
                           ref_started = i;
                           ref_started_by = j;

                           ref_finished = string::npos;

                           part_refs[ ref_starts ] = "";
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
                              part_refs[ ref_starts ] = text.substr( ref_started, ref_finished - ref_started );
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
                           repeats = ( p.max_matches - 1 );
                        else
                           repeats = ( p.max_matches - p.min_matches );
                     }
                  }
               }
               else
               {
                  for( size_t x = 0; x < p.matches.size( ); x++ )
                  {
                     if( match_set( text, i, p ) )
                     {
                        if( p_last_part_matched )
                           *p_last_part_matched = j;

                        size_t min_matches = max( ( size_t )1, p.min_matches );
#ifdef DEBUG
                        if( j == 0 )
                           cout << '\n';
                        cout << "matched set in part #" << ( j + 1 )
                         << ( p.inverted ? " (inverted)" : "" )
                         << " for " << text.substr( i, min_matches )
                         << " at " << i << ( text.length( ) > 50 ? string( )
                         : " ==> " + text.substr( start, i - start + min_matches ) ) << endl;
#endif
                        if( !okay )
                           begins = i;

                        okay = true;
                        found = true;

                        last_found = j;
                        has_matched = true;

                        if( p.start_ref
                         && ( ( ref_started == string::npos ) || ( j != ref_started_by ) ) )
                        {
                           if( ( ref_started != string::npos ) && ( ref_finished != string::npos ) )
                              part_refs[ ref_starts ] = text.substr( ref_started, ref_finished - ref_started );

                           ref_starts = j;
                           ref_started = i;
                           ref_started_by = j;

                           ref_finished = string::npos;

                           part_refs[ ref_starts ] = "";
                        }

                        part_from = j + 1;

                        i += min_matches;

                        if( p.finish_ref )
                        {
                           ref_finished = i;

                           if( ref_started != string::npos )
                              part_refs[ ref_starts ] = text.substr( ref_started, ref_finished - ref_started );
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
                           repeats = ( p.max_matches - 1 );
                        else
                           repeats = ( p.max_matches - p.min_matches );

                        break;
                     }
                  }
               }
            }

            if( !found
             && ( ref_started != string::npos )
             && p.finish_ref && ( p.min_matches == 0 ) )
            {
               ref_finished = i;

               part_refs[ ref_starts ] = text.substr( ref_started, ref_finished - ref_started );
            }

            bool was_last_part = ( matched_last_part == parts.size( ) - 1 );

            matched_last_part = -1;

            bool force_repeat = false;

            // NOTE: If the next part is an *inverted* match or will match anything then force
            // repeat matching to occur rather than proceeding.
            if( found && ( j < parts.size( ) - 1 ) && ( parts[ j + 1 ].inverted
             || ( ( parts[ j + 1 ].type == e_part_type_lit ) && parts[ j + 1 ].literal.empty( ) ) ) )
               force_repeat = true;

            // NOTE: If needing to match at the end and have matched the last unlimited part in
            // the expression then if the length is less than maximum that can follow prefer to
            // keep matching rather than skipping ahead to the next part (otherwise a match may
            // not be found at all).
            if( ( found || already_matched )
             && max_size_from_finish
             && ( text.length( ) > max_size_from_finish )
             && ( j >= last_unlimited_part )
             && ( ( li + 1 ) <= ( text.length( ) - max_size_from_finish ) ) )
               force_repeat = true;

            bool forcing_previous_repeat = false;

            if( !found
             && ( ( has_last_set_used && ( last_set_part_used == ( last_found - 1 ) ) )
             || ( has_last_literal_used && ( last_lit_part_used == ( last_found - 1 ) ) ) ) )
               forcing_previous_repeat = true;

            if( force_repeat && forcing_previous_repeat )
            {
               okay = old_okay;
               finishes = old_finishes;
               part_from = old_part_from;
            }

            // NOTE: If matching occurs with the last unlimited part but have reached the point
            // where the remaining parts could require all the remaining characters then simply
            // try matching the next part (from the last position) rather than repeating.
            if( found && !force_repeat && already_matched
             && max_size_from_finish
             && ( text.length( ) > max_size_from_finish )
             && ( j == last_unlimited_part )
             && ( ( li + 1 ) >= ( text.length( ) - max_size_from_finish ) ) )
            {
               i = li - 1;

               continue;
            }

            // NOTE: If must match at finish then will check whether proceeding
            // to the next part would make matching impossible due to the total
            // maximum match limits that follow.
            if( found && !force_repeat && match_at_finish
             && max_chars_remaining.size( ) && ( j < ( max_chars_remaining.size( ) - 1 ) ) )
            {
               size_t max_remaining = max_chars_remaining[ j + 1 ];

               if( max_remaining && ( ( i + max_remaining ) < text.length( ) ) )
               {
                  --j;
                  force_repeat = true;
               }
            }

            if( !found && !force_repeat
             && !was_last_part && ( p.min_matches > 0 ) )
            {
               if( ( has_matched || already_matched ) && ( j > last_found ) )
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
               repeats = repeats_used;

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
            if( okay && ( ( !found && !already_matched )
             || force_repeat || ( j == ( parts.size( ) - 1 ) ) ) )
            {
               if( has_last_literal && ( repeats != 0 )
                && match_literal( expand_refs( last_literal ), text, i ) )
               {
#ifdef DEBUG
                  cout << "matched last literal: " << last_literal
                   << " in part #" << ( last_lit_part + 1 ) << " at " << i
                   << ( text.length( ) > 50 ? string( ) : " ==> " + text.substr( start, i - start + 1 ) ) << endl;
#endif
                  j = part_from = matched_last_part = last_lit_part;

                  if( ( j == ( parts.size( ) - 1 ) ) || ( p.min_matches > 0 ) )
                     --j;

                  found = true;

                  if( last_literal.empty( ) )
                     ++i;
                  else
                  {
                     i += literal_size( last_literal );

                     if( !last_literal.empty( ) && ( last_literal[ 0 ] == '\b' ) )
                        ++i;
                  }

                  if( was_last_part && ( !match_at_finish || ( i == text.length( ) ) ) )
                     done = true;

                  if( last_ref_finish )
                     ref_finished = i;

                  if( ( ref_started != string::npos ) && ( ref_finished != string::npos ) )
                     part_refs[ ref_starts ] = text.substr( ref_started, ( ref_finished - ref_started ) );

                  finishes = i - 1;

                  if( repeats )
                     --repeats;

                  repeats_used = repeats;

                  has_last_set_used = false;
                  has_last_literal_used = true;

                  last_literal_used = last_literal;
                  last_lit_part_used = last_lit_part;
               }

               if( has_last_set && ( repeats != 0 ) && ( i < text.length( ) ) )
               {
                  for( size_t x = 0; x < last_set.size( ); x++ )
                  {
                     bool matched = false;

                     if( ( last_set[ x ].first == '\0' )
                      || ( ( text[ i ] >= last_set[ x ].first ) && ( text[ i ] <= last_set[ x ].second ) ) )
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

                        if( ( j == ( parts.size( ) - 1 ) ) || ( p.min_matches > 0 ) )
                        {
                           --j;

                           if( was_last_part && ( !match_at_finish || ( i == ( text.length( ) - 1 ) ) ) )
                              done = true;
                        }

                        found = true;

                        finishes = i++;

                        if( last_ref_finish )
                           ref_finished = i;

                        if( ( ref_started != string::npos ) && ( ref_finished != string::npos ) )
                           part_refs[ ref_starts ] = text.substr( ref_started, ( ref_finished - ref_started ) );

                        if( repeats )
                           --repeats;

                        repeats_used = repeats;

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

            if( !found && ( p.min_matches || ( j == ( parts.size( ) - 1 ) ) ) )
            {
               if( !has_matched || ( ( j > last_found ) && p.min_matches ) )
                  okay = false;

               part_from = 0;

               has_last_set = false;
               has_last_literal = false;

               break;
            }
         }

         if( match_at_start && ( !okay || ( begins != 0 ) ) )
            break;

         if( done || ( part_from >= parts.size( ) ) )
            break;

         if( ( i > finishes ) && ( i < ( text.length( ) - 1 ) ) )
         {
            okay = false;
            break;
         }
      }

      if( ( ref_started != string::npos ) && ( ref_finished != string::npos ) )
         part_refs[ ref_starts ] = text.substr( ref_started, ( ref_finished - ref_started ) );

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
               if( ( j != ( parts.size( ) - 1 ) ) || ( finishes != ( text.length( ) - 1 ) )
                || ( ( parts[ j ].type == e_part_type_lit ) && ( parts[ j ].literal != "\b" ) )
                || ( ( parts[ j ].type == e_part_type_set ) && parts[ j ].inverted
                && ( ( text.length( ) < parts[ j ].min_matches )
                || !match_set( text, ( text.length( ) - parts[ j ].min_matches ), parts[ j ] ) ) ) )
               {
                  okay = false;
                  break;
               }
            }
         }
      }

      if( okay || ( match_at_start && ( begins == string::npos ) ) )
         break;

      if( !text.length( ) || ( start++ >= ( text.length( ) - 1 ) ) )
         break;

      if( match_at_start )
         break;
#ifdef DEBUG
       cout << "\n(retry match from pos: " << start << ")" << endl;
#endif
   }

   if( okay && match_at_finish && ( finishes != ( text.length( ) - 1 ) ) )
      okay = false;

   // NOTE: If the last literal ends with a boundary match character
   // and the last matched character is a non-word character then it
   // will be necessary to remove the last character from the match.
   if( okay && parts.size( ) )
   {
      size_t last_part = ( parts.size( ) - 1 );

      if( okay && ( parts[ last_part ].type == e_part_type_lit ) )
      {
         string last_literal = parts[ last_part ].literal;

         if( !last_literal.empty( )
          && ( last_literal[ last_literal.length( ) - 1 ] == '\b' ) && !is_word_char( text[ finishes ] ) )
            --finishes;
      }
   }

   if( okay && p_length )
      *p_length = ( finishes - begins + 1 );

   if( okay && p_refs )
   {
      for( map< int, string >::iterator i = part_refs.begin( ); i != part_refs.end( ); ++i )
         ref_vals.push_back( i->second );

      *p_refs = ref_vals;
   }

   return ( okay ? begins : string::npos );
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
         if( ( p.matches[ x ].first == '\0' )
          || ( ( text[ off ] >= p.matches[ x ].first ) && ( text[ off ] <= p.matches[ x ].second ) ) )
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

      if( ++off > ( text.length( ) - 1 ) )
         break;
   }

   return ( okay && ( rounds <= 0 ) );
}

string& regex::impl::expand_refs( string& literal )
{
   int j = 0;

   for( map< int, string >::iterator i = part_refs.begin( ); i != part_refs.end( ); ++i )
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

regex::regex( const string& expr, bool match_at_start, bool match_at_finish )
{
#ifdef DEBUG
   try
   {
#endif
      p_impl = new regex::impl( expr, match_at_start, match_at_finish );
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

regex_chain::regex_chain( const string& expr )
{
   bool is_in_ref = false;
   bool is_in_set = false;
   bool had_escape = false;
   bool is_in_range = false;
   bool was_inverted = false;
   bool was_separator = false;

   string next_expr;

   char last_ch = '\0';

   regex* p_regex = 0;

   for( size_t i = 0; i < expr.length( ); i++ )
   {
      char ch = expr[ i ];

      if( was_separator )
      {
         bool inverted = false;

         if( ch == '!' )
            inverted = true;

         if( !inverted && ( ch != last_ch ) )
            throw runtime_error( "invalid chain separator suffix found in '" + expr + "'" );

         try
         {
            p_regex = new regex( next_expr );
         }
         catch( ... )
         {
            cleanup( );

            throw;
         }

         regexes.push_back( make_pair( p_regex, was_inverted ) );

         next_expr.erase( );

         was_separator = false;

         was_inverted = inverted;

         continue;
      }

      if( had_escape )
         had_escape = false;
      else if( ch == '\\' )
         had_escape = true;
      else if( ch == '&' && !is_in_ref && !is_in_set && !is_in_range )
      {
         if( i == 0 )
            throw runtime_error( "invalid leading chain separator found in '" + expr + "'" );

         was_separator = true;
      }
      else if( ch == '(' && !is_in_set )
      {
         if( is_in_range )
            throw runtime_error( "invalid ref start character found in range for '" + expr + "'" );

         is_in_ref = true;
      }
      else if( ch == ')' && !is_in_set )
      {
         if( !is_in_ref )
            throw runtime_error( "invalid ref finish character found in '" + expr + "'" );

         is_in_ref = false;
      }
      else if( ch == '[' )
      {
         if( is_in_range )
            throw runtime_error( "invalid set start character found in range for '" + expr + "'" );

         is_in_set = true;
      }
      else if( ch == ']' )
      {
         if( !is_in_set )
            throw runtime_error( "invalid set finish character found in '" + expr + "'" );

         is_in_set = false;
      }
      else if( ch == '{' && !is_in_set )
         is_in_range = true;
      else if( ch == '}' && !is_in_set )
      {
         if( !is_in_range )
            throw runtime_error( "invalid range finish character found in '" + expr + "'" );

         is_in_range = false;
      }

      if( !was_separator )
         next_expr += ch;

      last_ch = ch;
   }

   if( !next_expr.empty( ) )
   {
      try
      {
         p_regex = new regex( next_expr );
      }
      catch( ... )
      {
         cleanup( );

         throw;
      }

      regexes.push_back( make_pair( p_regex, was_inverted ) );
   }
}

regex_chain::~regex_chain( )
{
   cleanup( );
}

string::size_type regex_chain::search( const string& text, string::size_type* p_length, vector< string >* p_refs )
{
   string::size_type pos = string::npos;
   string::size_type last_pos = string::npos;

   for( size_t i = 0; i < regexes.size( ); i++ )
   {
      regex* p_regex = regexes[ i ].first;

      bool inverted = regexes[ i ].second;

      pos = ( regexes[ i ].first )->search( text, ( inverted ? 0 : p_length ), ( inverted ? 0 : p_refs ) );

      if( inverted )
      {
         if( pos == string::npos )
            pos = last_pos;
         else
         {
            pos = string::npos;

            break;
         }
      }
      else if( pos == string::npos )
         break;
      else
         last_pos = pos;
   }

   return pos;
}

void regex_chain::cleanup( )
{
   for( size_t i = 0; i < regexes.size( ); i++ )
      delete regexes[ i ].first;
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

      cout << "\nregex is: " << expr.get_expr( )
       << "\nsearching: " << input << "\n\n";

      string::size_type length;
      string::size_type pos = expr.search( input, &length );

      if( pos != string::npos )
         cout << "matched: " << input.substr( pos, length ) << endl;
      else
         cout << "*** no match ***" << endl;
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
   }
}
#endif
