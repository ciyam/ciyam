// Copyright (c) 2012
//
// CIYAM Pty. Ltd.
// ACN 093 704 539
//
// ALL RIGHTS RESERVED
//
// Permission to use this software for non-commercial purposes is hereby granted. Permission to
// distribute this software privately is granted provided that the source code is unaltered and
// complete or that any alterations and omissions have been first approved by CIYAM. Commercial
// usage of this software is not permitted without first obtaining a license for such a purpose
// from CIYAM. This software may not be publicly distributed unless written permission to do so
// has been obtained from CIYAM.

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
      if( lit[ i ] != '\b' )
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

   void expand_refs( string& literal );

   void dump( ostream& os );

   string expr;
   string prefix;

   int min_size;
   int max_size;

   bool match_at_start;
   bool match_at_finish;

   bool prefix_at_boundary;

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
 match_at_finish( false )
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

   string::size_type pos = string::npos;

   // NOTE: If the expression needs to match at the end then matching is first
   // attempted at the last possible position and from the first position. The
   // search completes immediately if successful closest to the first position
   // and a mid-point between the high and low is tested in order to shift one
   // or the other positions to reduce the number of comparisons.
   if( match_at_finish && !match_at_start )
   {
      string::size_type start = text.size( ) - min_size;

      string::size_type low = 0;
      string::size_type high = start;

      while( true )
      {
         start = do_search( text, high, p_length, p_refs );
         if( start != string::npos && start < high )
            pos = high = start;

         start = do_search( text, low, p_length, p_refs );
         if( start != string::npos )
         {
            pos = low = start;
            break;
         }

         string::size_type mid = ( low + high ) / 2;

         if( mid != pos )
         {
            vector< string > old_refs( refs );
            start = do_search( text, mid, p_length, p_refs );

            if( start == string::npos )
               low = mid;
            else if( start < high )
               pos = high = start;
            else if( p_refs )
               *p_refs = old_refs;
         }

         if( low == high )
            break;

         ++low;
         --high;
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

   // NOTE: Need to force all refs to be empty strings in case they are in optional parts.
   for( size_t i = 0; i < parts.size( ); i++ )
   {
      if( parts[ i ].start_ref )
         node_refs[ i ] = "";
   }

   while( !parts.empty( ) )
   {
      size_t last_found = 0;
      size_t start_from = 0;

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

      for( size_t i = start; i < text.size( ); i++ )
      {
         bool has_last_set = false;
         bool has_last_literal = false;

         int remaining = 0;
         string last_literal;
         bool last_ref_finish = false;
         bool last_set_inverted = false;
         vector< pair< char, char > > last_set;

         for( size_t j = start_from; j < parts.size( ); j++ )
         {
            bool found = false;
            const part& p = parts[ j ];

            if( i >= text.size( ) )
               break;

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
                  cout << "matched literal in part #" << ( j + 1 ) << " for " << literal << " at #" << i << endl;
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

                  start_from = j + 1;

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
                     cout << "matched set in part #" << ( j + 1 ) << ( p.inverted ? " (inverted)" : "" ) << " for " << text[ i ] << " at #" << i << endl;
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

                     start_from = j + 1;

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

            bool force_repeat = false;
            if( found && j < parts.size( ) - 1 && parts[ j + 1 ].inverted )
               force_repeat = true;

            // NOTE: The approach to repeat matching is to initially try to match
            // the next part of the expression and only if this fails (or this is
            // the last part of the expression or it has matched the current part
            // and the next part is inverted) then attempt the repeat.
            if( okay && ( !found || force_repeat || j == parts.size( ) - 1 ) )
            {
               expand_refs( last_literal );

               if( has_last_literal && remaining != 0
                && match_literal( last_literal, text, i ) )
               {
#ifdef DEBUG
                  cout << "matched last literal: " << last_literal << endl;
#endif
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

                  if( last_ref_finish )
                     ref_finished = i;

                  if( ref_started != string::npos && ref_finished != string::npos )
                     node_refs[ ref_starts ] = text.substr( ref_started, ref_finished - ref_started );

                  finishes = i - 1;

                  if( remaining )
                     --remaining;
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
                         << "-" << last_set[ x ].second << ( last_set_inverted ? "  (inverted)" : "" ) << endl;
#endif
                        --j;
                        found = true;

                        finishes = i++;

                        if( last_ref_finish )
                           ref_finished = i;

                        if( ref_started != string::npos && ref_finished != string::npos )
                           node_refs[ ref_starts ] = text.substr( ref_started, ref_finished - ref_started );

                        if( remaining )
                           --remaining;

                        break;
                     }
                  }
               }
            }

            if( !found && ( p.min_matches || j == parts.size( ) - 1 ) )
            {
               if( !has_matched || ( j > last_found && p.min_matches ) )
                  okay = false;
               else if( okay )
                  done = true;

               start_from = 0;
               has_last_set = false;
               has_last_literal = false;

               break;
            }
         }

         if( match_at_start && ( !okay || begins != 0 ) )
            break;

         if( done || start_from == 0 || start_from >= parts.size( ) )
            break;
      }

      if( ref_started != string::npos && ref_finished != string::npos )
         node_refs[ ref_starts ] = text.substr( ref_started, ref_finished - ref_started );

      if( okay && !done && start_from )
      {
         for( size_t j = start_from; j < parts.size( ); j++ )
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

void regex::impl::expand_refs( string& literal )
{
   int j = 0;
   for( map< int, string >::iterator i = node_refs.begin( ); i != node_refs.end( ); ++i )
   {
      string s( "\\" + to_string( ++j ) );
      replace( literal, s, i->second );
   }
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

