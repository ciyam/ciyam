// Copyright (c) 2017 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cmath>
#  include <map>
#  include <deque>
#  include <vector>
#  include <sstream>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#include "cube.h"

#include "utilities.h"

//#define DEBUG

using namespace std;

namespace
{

const int c_max_suggests = 8;

const int c_default_rounds = 3;

const size_t c_max_rounds = 100000000;

const char* const c_type_2_2_2 = "2x2x2";
const char* const c_type_3_3_3 = "3x3x3";
const char* const c_type_4_4_4 = "4x4x4";
const char* const c_type_5_5_5 = "5x5x5";

const char* const c_goal_found = "found";
const char* const c_train_prefix = "train";

const char* const c_type_separator = ":";

string get_init_state_for_type( const string& type )
{
   size_t num_per_side = 0;

   if( type == c_type_2_2_2 )
      num_per_side = 4;
   else if( type == c_type_3_3_3 )
      num_per_side = 9;
   else if( type == c_type_4_4_4 )
      num_per_side = 16;
   else if( type == c_type_5_5_5 )
      num_per_side = 25;
   else
      throw runtime_error( "unsupported cube type: " + type );

   string y( num_per_side, 'Y' );
   string g( num_per_side, 'G' );
   string o( num_per_side, 'O' );
   string b( num_per_side, 'B' );
   string r( num_per_side, 'R' );
   string w( num_per_side, 'W' );

   return y + g + o + b + r + w;
}

bool char_match( char lhs, char rhs, char* p_sch = 0 )
{
   bool match = false;

   if( lhs == '.' )
      match = ( p_sch != 0 );
   else if( lhs == '?' || lhs == '!' || lhs == '`' )
   {
      if( p_sch )
      {
         // NOTE: Using the ` character forces re-capture for comparison
         // so it's possible to support the matching of sets that aren't
         // all the same (e.g. ..`?....`?.. will match two pairs such as
         // ..XX....YY.. whereas ..??....??.. matches ..XX....XX..).
         if( *p_sch == '\0' || lhs == '`' )
         {
            *p_sch = lhs;
            match = true;
         }
         else
         {
            if( lhs == '?' )
               match = ( lhs == *p_sch );
            else
               match = ( lhs != *p_sch );
         }
      }
   }
   else
   {
      if( islower( lhs ) )
         match = toupper( lhs ) != rhs;
      else
         match = ( lhs == rhs );
   }

   return match;
}

bool has_found_goal( const string& str, const string& goal )
{
   bool found = true;

   char sch = '\0';

   for( size_t i = 0; i < min( str.length( ), goal.length( ) ); i++ )
   {
      if( !char_match( goal[ i ], str[ i ], &sch ) )
      {
         found = false;
         break;
      }
   }

   return found;
}

bool has_partial_goal( const string& str, const string& goal )
{
   bool has_partial = false;

   for( size_t i = 0; i < min( str.length( ), goal.length( ) ); i++ )
   {
      if( char_match( str[ i ], goal[ i ] ) )
      {
         has_partial = true;
         break;
      }
   }

   return has_partial;
}

bool close_to_goal(
 const string& new_state, const string& old_state,
 const string& goal, bool must_be_closer_to_goal = true )
{
   int new_matches = 0;
   int old_matches = 0;

   size_t min_state_length( min( new_state.length( ), old_state.length( ) ) );

   for( size_t i = 0; i < min( min_state_length, goal.length( ) ); i++ )
   {
      if( char_match( goal[ i ], new_state[ i ] ) )
         ++new_matches;

      if( char_match( goal[ i ], old_state[ i ] ) )
         ++old_matches;
   }

   return must_be_closer_to_goal ? new_matches > old_matches : new_matches >= old_matches;
}

bool is_closer_to_goal( const string& new_state, const string& old_state, const string& goal )
{
   return close_to_goal( new_state, old_state, goal );
}

bool is_as_close_to_goal( const string& new_state, const string& old_state, const string& goal )
{
   return close_to_goal( new_state, old_state, goal, false );
}

bool matches_pattern( const string& pat, const string& str )
{
   bool matches = true;

   char sch = '\0';

   for( size_t i = 0; i < min( str.length( ), pat.length( ) ); i++ )
   {
      if( !char_match( pat[ i ], str[ i ], &sch ) )
      {
         matches = false;
         break;
      }
   }

   return matches;
}

string expand_pattern( cube& c, const string& algo )
{
   string algx( algo );
   string::size_type pos = algx.find( '*' );

   while( pos != string::npos )
   {
      ostringstream ss;
      c.scramble( &ss, 1, false );

      string random_move( ss.str( ) );

      algx.erase( pos, 1 );
      algx.insert( pos, random_move );

      pos = algx.find( '*', pos + random_move.length( ) );
   }

   return algx;
}

void parse_stage_info( const string& stage_info, string& stage_name, string& stage_pattern )
{
   string::size_type pos = stage_info.find( ' ' );

   if( pos == string::npos )
      throw runtime_error( "unexpected stage info '" + stage_info + "'" );

   stage_name = stage_info.substr( 0, pos );
   stage_pattern = stage_info.substr( pos + 1 );
}

multimap< string, string > g_goal_algos;
multimap< string, string > g_algo_checks;

}

cube::cube( const string& type_and_or_state )
{
   bool has_called_init = false;

   string type_or_state( type_and_or_state );

   string::size_type pos = type_or_state.find( ':' );
   if( pos != string::npos )
   {
      initial = type_or_state.substr( pos + 1 );

      if( pos != 0 )
         type_or_state.erase( pos );
      else
         type_or_state.erase( pos, 1 );
   }

   if( type_or_state.empty( ) )
      type = c_type_3_3_3;
   else if( type_or_state.size( ) == 5 )
   {
      if( type_or_state != c_type_2_2_2 && type_or_state != c_type_3_3_3
       && type_or_state != c_type_4_4_4 && type_or_state != c_type_5_5_5 )
         throw runtime_error( "unknown cube type: " + type_or_state );

      type = type_or_state;
   }
   else
   {
      init( type_or_state );
      has_called_init = true;
   }

   if( !has_called_init )
      init( get_init_state_for_type( type ) );
}

void cube::init( const string& state )
{
   if( state.length( ) % 6 != 0 )
      throw runtime_error( "invalid state length % 6 != 0" );
   else
   {
      size_t offset = 0;
      size_t num_per_side = state.length( ) / 6;

      if( num_per_side == 4 )
         type = c_type_2_2_2;
      else if( num_per_side == 9 )
         type = c_type_3_3_3;
      else if( num_per_side == 16 )
         type = c_type_4_4_4;
      else if( num_per_side == 25 )
         type = c_type_5_5_5;
      else
         throw runtime_error( "unsupported num_per_side = " + to_string( num_per_side ) );

      if( initial.empty( ) )
         initial = state;

      top.clear( );
      top = state.substr( offset, num_per_side );

      lft.clear( );
      offset += num_per_side;
      lft = state.substr( offset, num_per_side );

      fnt.clear( );
      offset += num_per_side;
      fnt = state.substr( offset, num_per_side );

      rgt.clear( );
      offset += num_per_side;
      rgt = state.substr( offset, num_per_side );

      bck.clear( );
      offset += num_per_side;
      bck = state.substr( offset, num_per_side );

      bot.clear( );
      offset += num_per_side;
      bot = state.substr( offset, num_per_side );
   }
}

void cube::reset( )
{
   init( initial );
}

void cube::scramble( ostream* p_os, size_t num_moves, bool actually_scramble )
{
   vector< string > available_moves;

   available_moves.push_back( "B" );
   available_moves.push_back( "D" );
   available_moves.push_back( "F" );
   available_moves.push_back( "L" );
   available_moves.push_back( "R" );
   available_moves.push_back( "U" );
   available_moves.push_back( "X" );
   available_moves.push_back( "Y" );
   available_moves.push_back( "Z" );

   if( type != c_type_2_2_2 )
   {
      available_moves.push_back( "M" );
      available_moves.push_back( "E" );
      available_moves.push_back( "S" );
      available_moves.push_back( "Bw" );
      available_moves.push_back( "Dw" );
      available_moves.push_back( "Fw" );
      available_moves.push_back( "Lw" );
      available_moves.push_back( "Uw" );
      available_moves.push_back( "Rw" );
   }

   if( type == c_type_4_4_4 || type == c_type_5_5_5 )
   {
      available_moves.push_back( "b" );
      available_moves.push_back( "d" );
      available_moves.push_back( "f" );
      available_moves.push_back( "l" );
      available_moves.push_back( "r" );
      available_moves.push_back( "u" );

      available_moves.push_back( "Bb" );
      available_moves.push_back( "Dd" );
      available_moves.push_back( "Ff" );
      available_moves.push_back( "Ll" );
      available_moves.push_back( "Rr" );
      available_moves.push_back( "Uu" );
   }

   vector< string > moves;

   size_t range = available_moves.size( );

   size_t scramble_moves = num_moves;

   if( scramble_moves <= 0 )
   {
      scramble_moves = 10;

      if( type == c_type_3_3_3 )
         scramble_moves = 20;
      else if( type == c_type_4_4_4 )
         scramble_moves = 50;
      else if( type == c_type_5_5_5 )
         scramble_moves = 100;
   }

   for( size_t i = 0; i < scramble_moves; i++ )
   {
      string move( available_moves[ rand( ) % range ] );

      if( rand( ) % 3 == 0 )
         move += '\'';
      else if( rand( ) % 5 == 0 )
         move += '2';

      moves.push_back( move );
   }

   for( size_t i = 0; i < moves.size( ); i++ )
   {
      if( p_os )
      {
         if( i > 0 )
            *p_os << ' ';
         *p_os << moves[ i ];
      }

      if( actually_scramble )
         perform_moves( moves[ i ] );
   }
}

void cube::output_sides( ostream& os ) const
{
   output_side_info( os, "top", top );
   output_side_info( os, "lft", lft );
   output_side_info( os, "fnt", fnt );
   output_side_info( os, "rgt", rgt );
   output_side_info( os, "bck", bck );
   output_side_info( os, "bot", bot );
}

string cube::get_state( bool include_initial ) const
{
   string retval( top + lft + fnt + rgt + bck + bot );

   if( include_initial )
      retval += ':' + initial;

   return retval;
}

bool cube::solved( ) const
{
   return ( get_state( false ) == initial );
}

void cube::output_top_side( ostream& os ) const
{
   output_side_info( os, "", top );
}

void cube::output_back_side( ostream& os ) const
{
   output_side_info( os, "", bck );
}

void cube::output_left_side( ostream& os ) const
{
   output_side_info( os, "", lft );
}

void cube::output_front_side( ostream& os ) const
{
   output_side_info( os, "", fnt );
}

void cube::output_right_side( ostream& os ) const
{
   output_side_info( os, "", rgt );
}

void cube::output_bottom_side( ostream& os ) const
{
   output_side_info( os, "", bot );
}

void cube::output_side( ostream& os, const string& name ) const
{
   if( name == "top" )
      output_side_info( os, "", top );
   else if( name == "bck" || name == "back" )
      output_side_info( os, "", bck );
   else if( name == "lft" || name == "left" )
      output_side_info( os, "", lft );
   else if( name == "fnt" || name == "front" )
      output_side_info( os, "", fnt );
   else if( name == "rgt" || name == "right" )
      output_side_info( os, "", rgt );
   else if( name == "bot" || name == "bottom" )
      output_side_info( os, "", bot );
}

void cube::output_matching_cubie( ostream& os, const string& cubie ) const
{
   if( !cubie.empty( ) )
   {
      int offset = 0;

      if( cubie.length( ) > 1 && cubie[ 1 ] >= '1' && cubie[ 1 ] <= '9' )
         offset = cubie[ 1 ] - '1';

      if( cubie.length( ) > 2 && cubie[ 2 ] >= '0' && cubie[ 2 ] <= '9' )
      {
         offset *= 10;
         offset += cubie[ 2 ] - '0';
      }

      if( offset > top.length( ) )
         throw runtime_error( "invalid cubie '" + cubie + "' for cube type '" + type + "'" );

      if( cubie[ 0 ] == 'U' )
         os << top[ offset ];
      else if( cubie[ 0 ] == 'D' )
         os << bot[ offset ];
      else if( cubie[ 0 ] == 'L' )
         os << lft[ offset ];
      else if( cubie[ 0 ] == 'R' )
         os << rgt[ offset ];
      else if( cubie[ 0 ] == 'F' )
         os << fnt[ offset ];
      else if( cubie[ 0 ] == 'B' )
         os << bck[ offset ];
   }
}

void cube::output_matching_cubies( ostream& os, const string& color ) const
{
   if( !color.empty( ) )
   {
      string cubie( "U" );

      for( size_t i = 0; i < top.length( ); i++ )
      {
         string extra;

         if( top[ i ] == color[ 0 ] )
         {
            if( i < 9 && type != c_type_2_2_2 && type != c_type_3_3_3 )
               extra = "0";

            os << cubie << extra << ( i + 1 ) << '\n';
         }
      }

      cubie = "D";

      for( size_t i = 0; i < bot.length( ); i++ )
      {
         string extra;

         if( bot[ i ] == color[ 0 ] )
         {
            if( i < 9 && type != c_type_2_2_2 && type != c_type_3_3_3 )
               extra = "0";

            os << cubie << extra << ( i + 1 ) << '\n';
         }
      }

      cubie = "L";

      for( size_t i = 0; i < lft.length( ); i++ )
      {
         string extra;

         if( lft[ i ] == color[ 0 ] )
         {
            if( i < 9 && type != c_type_2_2_2 && type != c_type_3_3_3 )
               extra = "0";

            os << cubie << extra << ( i + 1 ) << '\n';
         }
      }

      cubie = "R";

      for( size_t i = 0; i < rgt.length( ); i++ )
      {
         string extra;

         if( rgt[ i ] == color[ 0 ] )
         {
            if( i < 9 && type != c_type_2_2_2 && type != c_type_3_3_3 )
               extra = "0";

            os << cubie << extra << ( i + 1 ) << '\n';
         }
      }

      cubie = "F";

      for( size_t i = 0; i < fnt.length( ); i++ )
      {
         string extra;

         if( fnt[ i ] == color[ 0 ] )
         {
            if( i < 9 && type != c_type_2_2_2 && type != c_type_3_3_3 )
               extra = "0";

            os << cubie << extra << ( i + 1 ) << '\n';
         }
      }

      cubie = "B";

      for( size_t i = 0; i < bck.length( ); i++ )
      {
         string extra;

         if( bck[ i ] == color[ 0 ] )
         {
            if( i < 9 && type != c_type_2_2_2 && type != c_type_3_3_3 )
               extra = "0";

            os << cubie << extra << ( i + 1 ) << '\n';
         }
      }
   }
}

void cube::flip( )
{
   string tmp( top );

   top = bot;
   bot = tmp;

   tmp = fnt;
   fnt = bck;
   bck = tmp;

   rotate_face( bck, true );
   rotate_face( bck, true );

   rotate_face( fnt, true );
   rotate_face( fnt, true );

   rotate_face( lft, true );
   rotate_face( lft, true );

   rotate_face( rgt, true );
   rotate_face( rgt, true );
}

void cube::move( const string& op )
{
   if( !op.empty( ) )
   {
      if( op[ 0 ] == 'U' || op[ 0 ] == 'u' )
         move_top( op );
      else if( op[ 0 ] == 'B' || op[ 0 ] == 'b' )
         move_back( op );
      else if( op[ 0 ] == 'L' || op[ 0 ] == 'l' )
         move_left( op );
      else if( op[ 0 ] == 'F' || op[ 0 ] == 'f' )
         move_front( op );
      else if( op[ 0 ] == 'R' || op[ 0 ] == 'r' )
         move_right( op );
      else if( op[ 0 ] == 'D' || op[ 0 ] == 'd' )
         move_bottom( op );
      else
         throw runtime_error( "unknown move op: " + op );
   }
}

void cube::move_top( const string& op )
{
   string op_val( op );

   int num_cubies_per_edge = ( int )sqrt( ( float )lft.size( ) );

   if( num_cubies_per_edge < 2 )
      throw runtime_error( "unexpected num_cubies_per_edge < 2" );
   else
   {
      if( op.empty( ) )
         throw runtime_error( "unexpected empty op for move_top" );
      else
      {
         bool clockwise = true;

         if( op_val.length( ) > 1 && op_val[ op_val.length( ) - 1 ] == '\'' )
         {
            clockwise = false;
            op_val.erase( op_val.length( ) - 1 );
         }

         int repeats = 1;

         if( op_val.length( ) > 1
          && op_val[ op_val.length( ) - 1 ] >= '1' && op_val[ op_val.length( ) - 1 ] <= '9' )
         {
            repeats = op_val[ op_val.length( ) - 1 ] - '0';
            op_val.erase( op_val.length( ) - 1 );
         }

         for( int i = 0; i < repeats; i++ )
         {
            string old_fnt( fnt );

            if( op_val == "u" || op_val == "U" || op_val == "Uu" || op_val == "Uw" )
            {
               if( clockwise )
               {
                  if( op_val != "u" )
                  {
                     swap_row( fnt, rgt, 0 );
                     swap_row( rgt, bck, 0 );
                     swap_row( bck, lft, 0 );
                     swap_row( lft, old_fnt, 0 );
                  }

                  if( op_val == "u" || op_val == "Uu" )
                  {
                     swap_row( fnt, rgt, 1 );
                     swap_row( rgt, bck, 1 );
                     swap_row( bck, lft, 1 );
                     swap_row( lft, old_fnt, 1 );
                  }
                  else if( op_val == "Uw" )
                  {
                     for( int k = 1; k < num_cubies_per_edge - 1; k++ )
                     {
                        swap_row( fnt, rgt, k );
                        swap_row( rgt, bck, k );
                        swap_row( bck, lft, k );
                        swap_row( lft, old_fnt, k );
                     }
                  }
               }
               else
               {
                  if( op_val != "u" )
                  {
                     swap_row( fnt, lft, 0 );
                     swap_row( lft, bck, 0 );
                     swap_row( bck, rgt, 0 );
                     swap_row( rgt, old_fnt, 0 );
                  }

                  if( op_val == "u" || op_val == "Uu" )
                  {
                     swap_row( fnt, lft, 1 );
                     swap_row( lft, bck, 1 );
                     swap_row( bck, rgt, 1 );
                     swap_row( rgt, old_fnt, 1 );
                  }
                  else if( op_val == "Uw" )
                  {
                     for( int k = 1; k < num_cubies_per_edge - 1; k++ )
                     {
                        swap_row( fnt, lft, k );
                        swap_row( lft, bck, k );
                        swap_row( bck, rgt, k );
                        swap_row( rgt, old_fnt, k );
                     }
                  }
               }

               if( op_val != "u" )
                  rotate_face( top, clockwise );
            }
            else
               throw runtime_error( "unsupported cube op " + op_val );
         }
      }
   }
}

void cube::move_left( const string& op )
{
   string op_val( op );

   int num_cubies_per_edge = ( int )sqrt( ( float )top.size( ) );

   if( num_cubies_per_edge < 2 )
      throw runtime_error( "unexpected num_cubies_per_edge < 2" );
   else
   {
      if( op.empty( ) )
         throw runtime_error( "unexpected empty op for move_left" );
      else
      {
         bool clockwise = true;

         if( op_val.length( ) > 1 && op_val[ op_val.length( ) - 1 ] == '\'' )
         {
            clockwise = false;
            op_val.erase( op_val.length( ) - 1 );
         }

         int repeats = 1;

         if( op_val.length( ) > 1
          && op_val[ op_val.length( ) - 1 ] >= '1' && op_val[ op_val.length( ) - 1 ] <= '9' )
         {
            repeats = op_val[ op_val.length( ) - 1 ] - '0';
            op_val.erase( op_val.length( ) - 1 );
         }

         for( int i = 0; i < repeats; i++ )
         {
            string old_fnt( fnt );

            if( op_val == "l" || op_val == "L" || op_val == "Ll" || op_val == "Lw" )
            {
               if( clockwise )
               {
                  if( op_val != "l" )
                  {
                     swap_column( fnt, top, 0 );
                     swap_column( top, bck, 0, num_cubies_per_edge - 1 );
                     swap_column( bck, bot, num_cubies_per_edge - 1, 0 );
                     swap_column( bot, old_fnt, 0 );

                     reverse_column( top, 0 );
                     reverse_column( bck, num_cubies_per_edge - 1 );
                  }

                  if( op_val == "l" || op_val == "Ll" )
                  {
                     swap_column( fnt, top, 1 );
                     swap_column( top, bck, 1, num_cubies_per_edge - 2 );
                     swap_column( bck, bot, num_cubies_per_edge - 2, 1 );
                     swap_column( bot, old_fnt, 1 );

                     reverse_column( top, 1 );
                     reverse_column( bck, num_cubies_per_edge - 2 );
                  }
                  else if( op_val == "Lw" )
                  {
                     for( int x = 1, k = num_cubies_per_edge - 2; k > 0; k--, x++ )
                     {
                        swap_column( fnt, top, x );
                        swap_column( top, bck, x, k );
                        swap_column( bck, bot, k, x );
                        swap_column( bot, old_fnt, x );

                        reverse_column( top, x );
                        reverse_column( bck, k );
                     }
                  }
               }
               else
               {
                  if( op_val != "l" )
                  {
                     swap_column( fnt, bot, 0 );
                     swap_column( bot, bck, 0, num_cubies_per_edge - 1 );
                     swap_column( bck, top, num_cubies_per_edge - 1, 0 );
                     swap_column( top, old_fnt, 0 );

                     reverse_column( bot, 0 );
                     reverse_column( bck, num_cubies_per_edge - 1 );
                  }

                  if( op_val == "l" || op_val == "Ll" )
                  {
                     swap_column( fnt, bot, 1 );
                     swap_column( bot, bck, 1, num_cubies_per_edge - 2 );
                     swap_column( bck, top, num_cubies_per_edge - 2, 1 );
                     swap_column( top, old_fnt, 1 );

                     reverse_column( bot, 1 );
                     reverse_column( bck, num_cubies_per_edge - 2 );
                  }
                  else if( op_val == "Lw" )
                  {
                     for( int x = 1, k = num_cubies_per_edge - 2; k > 0; k--, x++ )
                     {
                        swap_column( fnt, bot, x );
                        swap_column( bot, bck, x, k );
                        swap_column( bck, top, k, x );
                        swap_column( top, old_fnt, x );

                        reverse_column( bot, x );
                        reverse_column( bck, k );
                     }
                  }
               }

               if( op_val != "l" )
                  rotate_face( lft, clockwise );
            }
            else
               throw runtime_error( "unsupported cube op " + op_val );
         }
      }
   }
}

void cube::move_back( const string& op )
{
   string op_val( op );

   int num_cubies_per_edge = ( int )sqrt( ( float )lft.size( ) );

   if( num_cubies_per_edge < 2 )
      throw runtime_error( "unexpected num_cubies_per_edge < 2" );
   else
   {
      if( op.empty( ) )
         throw runtime_error( "unexpected empty op for move_back" );
      else
      {
         bool clockwise = true;

         if( op_val.length( ) > 1 && op_val[ op_val.length( ) - 1 ] == '\'' )
         {
            clockwise = false;
            op_val.erase( op_val.length( ) - 1 );
         }

         int repeats = 1;

         if( op_val.length( ) > 1
          && op_val[ op_val.length( ) - 1 ] >= '1' && op_val[ op_val.length( ) - 1 ] <= '9' )
         {
            repeats = op_val[ op_val.length( ) - 1 ] - '0';
            op_val.erase( op_val.length( ) - 1 );
         }

         for( int i = 0; i < repeats; i++ )
         {
            string old_lft( lft );
            string old_rgt( rgt );

            if( op_val == "b" || op_val == "B" || op_val == "Bb" || op_val == "Bw" )
            {
               if( !clockwise )
               {
                  if( op_val != "b" )
                  {
                     swap_row_with_column( bot, lft, num_cubies_per_edge - 1, 0 );
                     swap_row_with_column( bot, rgt, num_cubies_per_edge - 1 );
                     swap_row_with_column( top, rgt, 0, num_cubies_per_edge - 1 );
                     swap_row_with_column( top, old_lft, 0 );

                     reverse_row( top, 0 );
                     reverse_row( bot, num_cubies_per_edge - 1 );
                  }

                  if( op_val == "b" || op_val == "Bb" )
                  {
                     swap_row_with_column( bot, lft, num_cubies_per_edge - 2, 1 );
                     swap_row_with_column( bot, rgt, num_cubies_per_edge - 2 );
                     swap_row_with_column( top, rgt, 1, num_cubies_per_edge - 2 );
                     swap_row_with_column( top, old_lft, 1 );

                     reverse_row( top, 1 );
                     reverse_row( bot, num_cubies_per_edge - 2 );
                  }
                  else if( op_val == "Bw" )
                  {
                     for( int x = 1, k = num_cubies_per_edge - 2; k > 0; k--, x++ )
                     {
                        swap_row_with_column( bot, lft, k, x );
                        swap_row_with_column( bot, rgt, k );
                        swap_row_with_column( top, rgt, x, k );
                        swap_row_with_column( top, old_lft, x );

                        reverse_row( top, x );
                        reverse_row( bot, k );
                     }
                  }
               }
               else
               {
                  if( op_val != "b" )
                  {
                     swap_row_with_column( bot, rgt, num_cubies_per_edge - 1 );
                     swap_row_with_column( bot, lft, num_cubies_per_edge - 1, 0 );
                     swap_row_with_column( top, lft, 0 );
                     swap_row_with_column( top, old_rgt, 0, num_cubies_per_edge - 1 );

                     reverse_column( lft, 0 );
                     reverse_column( rgt, num_cubies_per_edge - 1 );
                  }

                  if( op_val == "b" || op_val == "Bb" )
                  {
                     swap_row_with_column( bot, rgt, num_cubies_per_edge - 2 );
                     swap_row_with_column( bot, lft, num_cubies_per_edge - 2, 1 );
                     swap_row_with_column( top, lft, 1 );
                     swap_row_with_column( top, old_rgt, 1, num_cubies_per_edge - 2 );

                     reverse_column( lft, 1 );
                     reverse_column( rgt, num_cubies_per_edge - 2 );
                  }
                  else if( op_val == "Bw" )
                  {
                     for( int x = 1, k = num_cubies_per_edge - 2; k > 0; k--, x++ )
                     {
                        swap_row_with_column( bot, rgt, k );
                        swap_row_with_column( bot, lft, k, x );
                        swap_row_with_column( top, lft, x );
                        swap_row_with_column( top, old_rgt, x, k );

                        reverse_column( lft, x );
                        reverse_column( rgt, k );
                     }
                  }
               }

               if( op_val != "b" )
                  rotate_face( bck, clockwise );
            }
            else
               throw runtime_error( "unsupported cube op " + op_val );
         }
      }
   }
}

void cube::move_front( const string& op )
{
   string op_val( op );

   int num_cubies_per_edge = ( int )sqrt( ( float )lft.size( ) );

   if( num_cubies_per_edge < 2 )
      throw runtime_error( "unexpected num_cubies_per_edge < 2" );
   else
   {
      if( op.empty( ) )
         throw runtime_error( "unexpected empty op for move_front" );
      else
      {
         bool clockwise = true;

         if( op_val.length( ) > 1 && op_val[ op_val.length( ) - 1 ] == '\'' )
         {
            clockwise = false;
            op_val.erase( op_val.length( ) - 1 );
         }

         int repeats = 1;

         if( op_val.length( ) > 1
          && op_val[ op_val.length( ) - 1 ] >= '1' && op_val[ op_val.length( ) - 1 ] <= '9' )
         {
            repeats = op_val[ op_val.length( ) - 1 ] - '0';
            op_val.erase( op_val.length( ) - 1 );
         }

         for( int i = 0; i < repeats; i++ )
         {
            string old_lft( lft );
            string old_rgt( rgt );

            if( op_val == "f" || op_val == "F" || op_val == "Ff" || op_val == "Fw" )
            {
               if( clockwise )
               {
                  if( op_val != "f" )
                  {
                     swap_row_with_column( bot, lft, 0, num_cubies_per_edge - 1 );
                     swap_row_with_column( bot, rgt, 0 );
                     swap_row_with_column( top, rgt, num_cubies_per_edge - 1, 0 );
                     swap_row_with_column( top, old_lft, num_cubies_per_edge - 1 );

                     reverse_row( bot, 0 );
                     reverse_row( top, num_cubies_per_edge - 1 );
                  }

                  if( op_val == "f" || op_val == "Ff" )
                  {
                     swap_row_with_column( bot, lft, 1, num_cubies_per_edge - 2 );
                     swap_row_with_column( bot, rgt, 1 );
                     swap_row_with_column( top, rgt, num_cubies_per_edge - 2, 1 );
                     swap_row_with_column( top, old_lft, num_cubies_per_edge - 2 );

                     reverse_row( bot, 1 );
                     reverse_row( top, num_cubies_per_edge - 2 );
                  }
                  else if( op_val == "Fw" )
                  {
                     for( int x = 1, k = num_cubies_per_edge - 2; k > 0; k--, x++ )
                     {
                        swap_row_with_column( bot, lft, x, k );
                        swap_row_with_column( bot, rgt, x );
                        swap_row_with_column( top, rgt, k, x );
                        swap_row_with_column( top, old_lft, k );

                        reverse_row( bot, x );
                        reverse_row( top, k );
                     }
                  }
               }
               else
               {
                  if( op_val != "f" )
                  {
                     swap_row_with_column( bot, rgt, 0 );
                     swap_row_with_column( bot, lft, 0, num_cubies_per_edge - 1 );
                     swap_row_with_column( top, lft, num_cubies_per_edge - 1 );
                     swap_row_with_column( top, old_rgt, num_cubies_per_edge - 1, 0 );

                     reverse_column( rgt, 0 );
                     reverse_column( lft, num_cubies_per_edge - 1 );
                  }

                  if( op_val == "f" || op_val == "Ff" )
                  {
                     swap_row_with_column( bot, rgt, 1 );
                     swap_row_with_column( bot, lft, 1, num_cubies_per_edge - 2 );
                     swap_row_with_column( top, lft, num_cubies_per_edge - 2 );
                     swap_row_with_column( top, old_rgt, num_cubies_per_edge - 2, 1 );

                     reverse_column( rgt, 1 );
                     reverse_column( lft, num_cubies_per_edge - 2 );
                  }
                  else if( op_val == "Fw" )
                  {
                     for( int x = 1, k = num_cubies_per_edge - 2; k > 0; k--, x++ )
                     {
                        swap_row_with_column( bot, rgt, x, k );
                        swap_row_with_column( bot, lft, x );
                        swap_row_with_column( top, lft, k, x );
                        swap_row_with_column( top, old_rgt, k );

                        reverse_column( rgt, x );
                        reverse_column( lft, k );
                     }
                  }
               }

               if( op_val != "f" )
                  rotate_face( fnt, clockwise );
            }
            else
               throw runtime_error( "unsupported cube op " + op_val );
         }
      }
   }
}

void cube::move_right( const string& op )
{
   string op_val( op );

   int num_cubies_per_edge = ( int )sqrt( ( float )top.size( ) );

   if( num_cubies_per_edge < 2 )
      throw runtime_error( "unexpected num_cubies_per_edge < 2" );
   else
   {
      if( op.empty( ) )
         throw runtime_error( "unexpected empty op for move_right" );
      else
      {
         bool clockwise = true;

         if( op_val.length( ) > 1 && op_val[ op_val.length( ) - 1 ] == '\'' )
         {
            clockwise = false;
            op_val.erase( op_val.length( ) - 1 );
         }

         int repeats = 1;

         if( op_val.length( ) > 1
          && op_val[ op_val.length( ) - 1 ] >= '1' && op_val[ op_val.length( ) - 1 ] <= '9' )
         {
            repeats = op_val[ op_val.length( ) - 1 ] - '0';
            op_val.erase( op_val.length( ) - 1 );
         }

         for( int i = 0; i < repeats; i++ )
         {
            string old_fnt( fnt );

            if( op_val == "r" || op_val == "R" || op_val == "Rr" || op_val == "Rw" )
            {
               if( !clockwise )
               {
                  if( op_val != "r" )
                  {
                     swap_column( fnt, top, num_cubies_per_edge - 1 );
                     swap_column( top, bck, num_cubies_per_edge - 1, 0 );
                     swap_column( bck, bot, 0, num_cubies_per_edge - 1 );
                     swap_column( bot, old_fnt, num_cubies_per_edge - 1 );

                     reverse_column( bck, 0 );
                     reverse_column( top, num_cubies_per_edge - 1 );
                  }

                  if( op_val == "r" || op_val == "Rr" )
                  {
                     swap_column( fnt, top, num_cubies_per_edge - 2 );
                     swap_column( top, bck, num_cubies_per_edge - 2, 1 );
                     swap_column( bck, bot, 1, num_cubies_per_edge - 2 );
                     swap_column( bot, old_fnt, num_cubies_per_edge - 2 );

                     reverse_column( bck, 1 );
                     reverse_column( top, num_cubies_per_edge - 2 );
                  }
                  else if( op_val == "Rw" )
                  {
                     for( int x = 1, k = num_cubies_per_edge - 2; k > 0; k--, x++ )
                     {
                        swap_column( fnt, top, k );
                        swap_column( top, bck, k, x );
                        swap_column( bck, bot, x, k );
                        swap_column( bot, old_fnt, k );

                        reverse_column( bck, x );
                        reverse_column( top, k );
                     }
                  }
               }
               else
               {
                  if( op_val != "r" )
                  {
                     swap_column( fnt, bot, num_cubies_per_edge - 1 );
                     swap_column( bot, bck, num_cubies_per_edge - 1, 0 );
                     swap_column( bck, top, 0, num_cubies_per_edge - 1 );
                     swap_column( top, old_fnt, num_cubies_per_edge - 1 );

                     reverse_column( bck, 0 );
                     reverse_column( bot, num_cubies_per_edge - 1 );
                  }

                  if( op_val == "r" || op_val == "Rr" )
                  {
                     swap_column( fnt, bot, num_cubies_per_edge - 2 );
                     swap_column( bot, bck, num_cubies_per_edge - 2, 1 );
                     swap_column( bck, top, 1, num_cubies_per_edge - 2 );
                     swap_column( top, old_fnt, num_cubies_per_edge - 2 );

                     reverse_column( bck, 1 );
                     reverse_column( bot, num_cubies_per_edge - 2 );
                  }
                  else if( op_val == "Rw" )
                  {
                     for( int x = 1, k = num_cubies_per_edge - 2; k > 0; k--, x++ )
                     {
                        swap_column( fnt, bot, k );
                        swap_column( bot, bck, k, x );
                        swap_column( bck, top, x, k );
                        swap_column( top, old_fnt, k );

                        reverse_column( bck, x );
                        reverse_column( bot, k );
                     }
                  }
               }

               if( op_val != "r" )
                  rotate_face( rgt, clockwise );
            }
            else
               throw runtime_error( "unsupported cube op " + op_val );
         }
      }
   }
}

void cube::move_bottom( const string& op )
{
   string op_val( op );
   int num_cubies_per_side = ( int )sqrt( ( float )lft.size( ) );

   if( num_cubies_per_side < 2 )
      throw runtime_error( "unexpected num_cubies_per_side < 2" );
   else
   {
      if( op.empty( ) )
         throw runtime_error( "unexpected empty op for move_bottom" );
      else
      {
         bool clockwise = true;

         if( op_val.length( ) > 1 && op_val[ op_val.length( ) - 1 ] == '\'' )
         {
            clockwise = false;
            op_val.erase( op_val.length( ) - 1 );
         }

         int repeats = 1;

         if( op_val.length( ) > 1
          && op_val[ op_val.length( ) - 1 ] >= '1' && op_val[ op_val.length( ) - 1 ] <= '9' )
         {
            repeats = op_val[ op_val.length( ) - 1 ] - '0';
            op_val.erase( op_val.length( ) - 1 );
         }

         for( int i = 0; i < repeats; i++ )
         {
            string old_fnt( fnt );

            if( op_val == "d" || op_val == "D" || op_val == "Dd" || op_val == "Dw" )
            {
               if( !clockwise )
               {
                  if( op_val != "d" )
                  {
                     swap_row( fnt, rgt, num_cubies_per_side - 1 );
                     swap_row( rgt, bck, num_cubies_per_side - 1 );
                     swap_row( bck, lft, num_cubies_per_side - 1 );
                     swap_row( lft, old_fnt, num_cubies_per_side - 1 );
                  }

                  if( op_val == "d" || op_val == "Dd" )
                  {
                     swap_row( fnt, rgt, num_cubies_per_side - 2 );
                     swap_row( rgt, bck, num_cubies_per_side - 2 );
                     swap_row( bck, lft, num_cubies_per_side - 2 );
                     swap_row( lft, old_fnt, num_cubies_per_side - 2 );
                  }
                  else if( op_val == "Dw" )
                  {
                     for( int k = 2; k < num_cubies_per_side; k++ )
                     {
                        swap_row( fnt, rgt, num_cubies_per_side - k );
                        swap_row( rgt, bck, num_cubies_per_side - k );
                        swap_row( bck, lft, num_cubies_per_side - k );
                        swap_row( lft, old_fnt, num_cubies_per_side - k );
                     }
                  }
               }
               else
               {
                  if( op_val != "d" )
                  {
                     swap_row( fnt, lft, num_cubies_per_side - 1 );
                     swap_row( lft, bck, num_cubies_per_side - 1 );
                     swap_row( bck, rgt, num_cubies_per_side - 1 );
                     swap_row( rgt, old_fnt, num_cubies_per_side - 1 );
                  }

                  if( op_val == "d" || op_val == "Dd" )
                  {
                     swap_row( fnt, lft, num_cubies_per_side - 2 );
                     swap_row( lft, bck, num_cubies_per_side - 2 );
                     swap_row( bck, rgt, num_cubies_per_side - 2 );
                     swap_row( rgt, old_fnt, num_cubies_per_side - 2 );
                  }
                  else if( op_val == "Dw" )
                  {
                     for( int k = 2; k < num_cubies_per_side; k++ )
                     {
                        swap_row( fnt, lft, num_cubies_per_side - k );
                        swap_row( lft, bck, num_cubies_per_side - k );
                        swap_row( bck, rgt, num_cubies_per_side - k );
                        swap_row( rgt, old_fnt, num_cubies_per_side - k );
                     }
                  }
               }

               if( op_val != "d" )
                  rotate_face( bot, clockwise );
            }
            else
               throw runtime_error( "unsupported cube op " + op_val );
         }
      }
   }
}

void cube::perform_moves( const string& ops )
{
   string all_ops( ops );
   vector< string > move_ops;

   if( type == c_type_2_2_2 )
   {
      replace( all_ops, "X2", "X X" );
      replace( all_ops, "Y2", "Y Y" );
      replace( all_ops, "Z2", "Z Z" );

      replace( all_ops, "X'", "R' L", "X", "R L'" );
      replace( all_ops, "Y'", "U' D", "Y", "U D'" );
      replace( all_ops, "Z'", "B F'", "Z", "B' F" );
   }
   else
   {
      replace( all_ops, "M2", "M M" );
      replace( all_ops, "E2", "E E" );
      replace( all_ops, "S2", "S S" );

      replace( all_ops, "X2", "X X" );
      replace( all_ops, "Y2", "Y Y" );
      replace( all_ops, "Z2", "Z Z" );

      replace( all_ops, "X'", "Rw' L", "X", "Rw L'" );
      replace( all_ops, "Y'", "Uw' D", "Y", "Uw D'" );
      replace( all_ops, "Z'", "Bw F'", "Z", "Bw' F" );
   }

   if( type == c_type_3_3_3 )
   {
      replace( all_ops, "M'", "Rr R'", "M", "Rr' R" );
      replace( all_ops, "E'", "Dd' D", "E", "Dd D'" );
      replace( all_ops, "S'", "Ff' F", "S", "Ff F'" );
   }
   else if( type == c_type_4_4_4 )
   {
      replace( all_ops, "M'", "Lw' L", "M", "Lw L'" );
      replace( all_ops, "E'", "Dw' D", "E", "Dw D'" );
      replace( all_ops, "S'", "Bw B'", "S", "Bw' B" );
   }
   else if( type == c_type_5_5_5 )
   {
      replace( all_ops, "M'", "Rw' Rr l'", "M", "Rw Rr' l" );
      replace( all_ops, "E'", "Dw' u' Dd", "E", "Dw u Dd'" );
      replace( all_ops, "S'", "Fw' Ff b'", "S", "Fw Ff' b" );
   }

   replace( all_ops, ",", " " );

   if( !all_ops.empty( ) )
      split( all_ops, move_ops, ' ' );

   for( size_t i = 0; i < move_ops.size( ); i++ )
      move( move_ops[ i ] );
}

void cube::suggest( ostream& os, const string& info )
{
   int num_suggests = 0;
   bool has_had_output = false;

   string stop_at_stage;

   cube work_cube( *this );

#ifdef DEBUG
   cout << "[suggest] " << info << endl;
#endif
   while( !info.empty( ) )
   {
      string output;
      cube tmp_cube( work_cube );

      vector< string > lines;

      if( info[ 0 ] != '@' )
         lines.push_back( info );
      else
      {
         string filename( info.substr( 1 ) );

         string::size_type pos = filename.find( ':' );
         if( pos != string::npos )
         {
            stop_at_stage = filename.substr( pos + 1 );

            if( stop_at_stage.length( ) > 2 && stop_at_stage.substr( 0, 2 ) == "?:" )
               stop_at_stage.erase( 0, 2 );

            filename.erase( pos );
         }

         buffer_file_lines( filename, lines, false );
      }

      size_t start_from = 0;

      string last_pattern;

      bool last_was_found = false;
      bool ignore_next_for_start = false;

      map< int, bool > continuations;
      map< int, string > continuation_patterns;

      string stage_reached;

      // NOTE: Find the last goal that has already been reached to be
      // the starting point for algo suggestions.
      for( size_t i = 0; i < lines.size( ); i++ )
      {
         string next_line( lines[ i ] );

         continuations[ i ] = false;
         continuation_patterns[ i ] = string( );

         if( next_line.empty( ) || next_line[ 0 ] == '#' )
            continue;

         if( next_line[ 0 ] == ':' )
         {
            string stage_name, stage_pattern;
            parse_stage_info( next_line.substr( 1 ), stage_name, stage_pattern );

            if( has_found_goal( work_cube.get_state( false ), stage_pattern ) )
            {
               stage_reached = stage_name;

               if( stage_reached == stop_at_stage )
                  break;
            }

            continue;
         }

         if( next_line[ 0 ] == '!' )
         {
            ignore_next_for_start = true;
            continue;
         }

         string::size_type pos = next_line.find( ' ' );
         if( pos != string::npos )
         {
            string state( work_cube.get_state( false ) );
            string pattern( next_line.substr( 0, pos ) );

            if( pattern.find_first_not_of( "." ) == string::npos )
            {
               continuations[ i ] = true;
               continuation_patterns[ i ] = last_pattern;

               continue;
            }
            else
            {
               last_pattern = pattern;

                if( !ignore_next_for_start
                 && pattern.length( ) == state.length( ) && has_found_goal( state, pattern ) )
                  start_from = i;
            }

            if( !ignore_next_for_start
             && has_found_goal( state, next_line.substr( pos + 1 ) ) )
               start_from = i;

            ignore_next_for_start = false;
         }
      }

      if( !stop_at_stage.empty( ) && stage_reached == stop_at_stage )
         break;

      int num_found = 0;
      bool had_any_moves = false;

#ifdef DEBUG
      cout << "starting from line # " << ( start_from + 1 ) << endl;
      cout << "starting from stage: " << stage_reached << endl;
      cout << "stopping upon stage: " << stop_at_stage << endl;
#endif
      for( size_t i = start_from; i < lines.size( ); i++ )
      {
         string next_line( lines[ i ] );

         if( next_line.empty( ) || next_line[ 0 ] == '#' || next_line[ 0 ] == '!' )
            continue;

         if( next_line[ 0 ] == ':' )
         {
            string stage_name, stage_pattern;
            parse_stage_info( next_line.substr( 1 ), stage_name, stage_pattern );

            if( has_found_goal( work_cube.get_state( false ), stage_pattern ) )
            {
               stage_reached = stage_name;
#ifdef DEBUG
               cout << "reached stage: " << stage_reached << endl;
#endif
               if( stop_at_stage == "?:" )
                  stop_at_stage = stage_reached;

               if( stage_reached == stop_at_stage )
                  break;
            }

            continue;
         }

         // NOTE: A continuation re-uses the previous pattern to attempt
         // to reach a different goal but will not keep a partial match.
         string::size_type pos = next_line.find( ' ' );
         if( pos != string::npos )
         {
            string patterns( next_line.substr( 0, pos ) );

            if( patterns.find_first_not_of( "." ) == string::npos )
            {
               if( continuation_patterns[ i ].empty( ) )
                  throw runtime_error( "unexpected continuation without a continuation pattern" );

               next_line = continuation_patterns[ i ] + " " + next_line.substr( pos + 1 );
            }
         }

#ifdef DEBUG
         cout << "line # " << ( i + 1 ) << endl;
#endif
         last_was_found = false;
         bool found_next = false;

         while( true )
         {
            map< int, string > lengths_and_algos;

            for( size_t j = 0; j < c_default_rounds; j++ )
            {
               stringstream ss;

               if( tmp_cube.suggest_algo( ss, next_line, j + 1, true, &found_next ) )
               {
                  string result( ss.str( ) );

                  if( result == c_goal_found )
                  {
                     found_next = true;
                     ++num_found;
                     break;
                  }

                  if( !result.empty( ) )
                  {
                     if( found_next )
                        lengths_and_algos.clear( );

                     if( found_next || i == lines.size( ) - 1 || !continuations[ i + 1 ] )
                        lengths_and_algos.insert( make_pair( result.length( ), result ) );

                     if( found_next )
                        break;
                  }
               }
            }

            if( !lengths_and_algos.empty( ) )
            {
               string moves( lengths_and_algos.begin( )->second );

               if( had_any_moves )
                  output += ' ';
               output += moves;

               had_any_moves = true;

#ifdef DEBUG
               cout << "moves " << moves << endl;
#endif
               tmp_cube.perform_moves( moves );

               work_cube = tmp_cube;
#ifdef DEBUG
               cout << "state " << tmp_cube.get_state( false ) << endl;
#endif
            }
            else if( !found_next )
               break;

            if( found_next )
               break;
         }

         last_was_found = found_next;
      }

      if( num_found >= lines.size( ) )
      {
         os << c_goal_found;
         break;
      }

      // NOTE: It's possible that unwanted repeats can occur
      // due to one algorithm starting with a move which had
      // been the last move in the previous algorithm (or in
      // fact the exact opposite move).
      if( output.empty( ) || ++num_suggests >= c_max_suggests )
         break;
      else
      {
#ifdef DEBUG
         cout << "original: " << output << endl;
#endif
         output = " " + output + " ";

         for( size_t i = 0; i < 3; i++ )
         {
            replace( output, " L L' ", " " );
            replace( output, " R R' ", " " );
            replace( output, " U U' ", " " );
            replace( output, " D D' ", " " );
            replace( output, " F F' ", " " );
            replace( output, " B B' ", " " );
            replace( output, " X X' ", " " );
            replace( output, " Y Y' ", " " );
            replace( output, " Z Z' ", " " );

            replace( output, " L' L ", " " );
            replace( output, " R' R ", " " );
            replace( output, " U' U ", " " );
            replace( output, " D' D ", " " );
            replace( output, " F' F ", " " );
            replace( output, " B' B ", " " );
            replace( output, " X' X ", " " );
            replace( output, " Y' Y ", " " );
            replace( output, " Z' Z ", " " );

            replace( output, " L L ", " L2 " );
            replace( output, " R R ", " R2 " );
            replace( output, " U U ", " U2 " );
            replace( output, " D D ", " D2 " );
            replace( output, " F F ", " F2 " );
            replace( output, " B B ", " B2 " );
            replace( output, " X X ", " X2 " );
            replace( output, " Y Y ", " Y2 " );
            replace( output, " Z Z ", " Z2 " );

            replace( output, " L L2 ", " L' " );
            replace( output, " R R2 ", " R' " );
            replace( output, " U U2 ", " U' " );
            replace( output, " D D2 ", " D' " );
            replace( output, " F F2 ", " F' " );
            replace( output, " B B2 ", " B' " );
            replace( output, " X X2 ", " X' " );
            replace( output, " Y Y2 ", " Y' " );
            replace( output, " Z Z2 ", " Z' " );

            replace( output, " L2 L ", " L' " );
            replace( output, " R2 R ", " R' " );
            replace( output, " U2 U ", " U' " );
            replace( output, " D2 D ", " D' " );
            replace( output, " F2 F ", " F' " );
            replace( output, " B2 B ", " B' " );
            replace( output, " X2 X ", " X' " );
            replace( output, " Y2 Y ", " Y' " );
            replace( output, " Z2 Z ", " Z' " );

            replace( output, " L' L2 ", " L " );
            replace( output, " R' R2 ", " R " );
            replace( output, " U' U2 ", " U " );
            replace( output, " D' D2 ", " D " );
            replace( output, " F' F2 ", " F " );
            replace( output, " B' B2 ", " B " );
            replace( output, " X' X2 ", " X " );
            replace( output, " Y' Y2 ", " Y " );
            replace( output, " Z' Z2 ", " Z " );

            replace( output, " L2 L' ", " L " );
            replace( output, " R2 R' ", " R " );
            replace( output, " U2 U' ", " U " );
            replace( output, " D2 D' ", " D " );
            replace( output, " F2 F' ", " F " );
            replace( output, " B2 B' ", " B " );
            replace( output, " X2 X' ", " X " );
            replace( output, " Y2 Y' ", " Y " );
            replace( output, " Z2 Z' ", " Z " );

            replace( output, " L' L' ", " L2 " );
            replace( output, " R' R' ", " R2 " );
            replace( output, " U' U' ", " U2 " );
            replace( output, " D' D' ", " D2 " );
            replace( output, " F' F' ", " F2 " );
            replace( output, " B' B' ", " B2 " );
            replace( output, " X' X' ", " X2 " );
            replace( output, " Y' Y' ", " Y2 " );
            replace( output, " Z' Z' ", " Z2 " );

            replace( output, " L2 L2 ", " " );
            replace( output, " R2 R2 ", " " );
            replace( output, " U2 U2 ", " " );
            replace( output, " D2 D2 ", " " );
            replace( output, " F2 F2 ", " " );
            replace( output, " B2 B2 ", " " );
            replace( output, " X2 X2 ", " " );
            replace( output, " Y2 Y2 ", " " );
            replace( output, " Z2 Z2 ", " " );
         }

         output = output.substr( 1, output.length( ) - 2 );

#ifdef DEBUG
         cout << "replaced: " << output << endl;
#endif
         if( output.empty( ) )
            break;

         if( has_had_output )
            os << ' ';
         os << output;

         has_had_output = true;
      }

      if( work_cube.solved( )
       || ( !stop_at_stage.empty( ) && stage_reached == stop_at_stage ) )
         break;

   }
}

bool cube::suggest_algo( ostream& os,
 const string& info, size_t rounds, bool check_only_after_last_round, bool* p_found )
{
   bool rc = false;
   bool was_found = false;

   string::size_type pos = info.find( ' ' );
   if( pos == string::npos )
      throw runtime_error( "unexpected info '" + info + "'" );

   string pat( info.substr( 0, pos ) );
   string goal( info.substr( pos + 1 ) );

   string state( get_state( false ) );

   if( p_found )
      *p_found = false;

#ifdef DEBUG
   cout << "goal: " << goal << endl;
   cout << "state " << state << endl;
#endif
   if( has_found_goal( state, goal ) )
   {
      rc = was_found = true;

      if( p_found )
         *p_found = true;

      os << c_goal_found;
   }
   else if( !goal.empty( ) && !g_algo_checks.empty( ) )
   {
      vector< string > patterns;
      split( pat, patterns, '|' );

      multimap< string, string >::iterator mi;

      cube best_cube( *this );

      string best_match, best_partial;
      string goal_algo_key( type + c_type_separator + goal );

      mi = g_goal_algos.lower_bound( goal_algo_key );

      while( mi != g_goal_algos.end( ) )
      {
         if( mi->first != goal_algo_key )
            break;

         string algo( mi->second );

         string algo_check_key( type + c_type_separator + algo );

         multimap< string, string >::iterator mmi;
         mmi = g_algo_checks.lower_bound( algo_check_key );

         while( mmi != g_algo_checks.end( ) &&  mmi->first == algo_check_key )
         {
            string mask( mmi->second );

            cube tmp_cube( *this );
            cube new_cube( tmp_cube );

            replace( algo, ",", " " );

            size_t max_rounds = c_default_rounds;

            pos = mask.find( '=' );
            if( pos != string::npos )
            {
               max_rounds = from_string< size_t >( mask.substr( pos + 1 ) );
               mask.erase( pos );
            }

            bool okay = false;
            bool found = false;

            bool has_parts = algo.find( ' ' ) != string::npos;

            size_t min_size = min( goal.size( ), mask.size( ) );

            bool is_full_length_pattern = false;

            if( patterns.size( ) == 1 && patterns[ 0 ].length( ) == mask.length( ) )
            {
               rounds = max_rounds = 1;
               is_full_length_pattern = true;
            }

            if( rounds <= max_rounds )
            {
               // NOTE: If only one pattern that is the same length as the mask then it
               // is expected that the current state must match as if it were the goal.
               if( is_full_length_pattern )
                  okay = has_found_goal( tmp_cube.get_state( false ), patterns[ 0 ] );
               else
               {
                  for( size_t i = 0; i < patterns.size( ); i++ )
                  {
                     string next_pat( patterns[ i ] );

                     for( size_t j = 0; j <= min_size - next_pat.length( ); j++ )
                     {
                        if( mask[ j ] == '*' )
                        {
                           if( matches_pattern( next_pat, state.substr( j, next_pat.length( ) ) ) )
                           {
                              okay = true;
                              break;
                           }
                        }
                     }

                     if( okay )
                        break;
                  }
               }
            }

            if( okay )
            {
               string output;

               for( size_t j = 0; j < rounds; j++ )
               {
                  if( has_parts )
                  {
                     if( j > 0 )
                        output += ' ';

                     output += algo;
                  }

                  new_cube.perform_moves( algo );
#ifdef DEBUG
                  cout << algo << endl;
                  cout << "news: " << new_cube.get_state( false ) << endl;
#endif

                  if( check_only_after_last_round && j != rounds - 1 )
                     continue;

                  if( has_found_goal( new_cube.get_state( false ), goal ) )
                  {
                     if( !has_parts )
                     {
                        output = algo;

                        if( j == 1 )
                           output += "2";
                        else if( j == 2 )
                           output += "'";
                     }

                     rc = found = was_found = true;

                     if( best_match.empty( ) || output.length( ) < best_match.length( ) )
                        best_match = output;

#ifdef DEBUG
                     cout << "[found] best_match: " << best_match << endl;
#endif
                     if( p_found )
                        *p_found = true;

                     break;
                  }
                  else if( !is_full_length_pattern )
                  {
                     if( is_closer_to_goal( new_cube.get_state( false ), best_cube.get_state( false ), goal ) )
                     {
                        best_cube = tmp_cube = new_cube;

                        if( !has_parts )
                        {
                           output = algo;

                           if( j == 1 )
                              output += "2";
                           else if( j == 2 )
                              output += "'";
                        }

                        best_partial = output;
#ifdef DEBUG
                        cout << "[partial] best_partial: " << best_partial << endl;
#endif
                     }
                     else if( !best_partial.empty( )
                      && is_as_close_to_goal( new_cube.get_state( false ), best_cube.get_state( false ), goal ) )
                     {
                        if( !has_parts )
                        {
                           output = algo;

                           if( j == 1 )
                              output += "2";
                           else if( j == 2 )
                              output += "'";
                        }

                        if( output.length( ) < best_partial.length( ) )
                        {
                           best_partial = output;
                           best_cube = tmp_cube = new_cube;
#ifdef DEBUG
                           cout << "[partial] best_partial: " << best_partial << endl;
#endif
                        }
                     }
                  }
               }

               if( found )
                  break;
            }

            if( ++mmi == g_algo_checks.end( ) )
               break;
         }

         if( was_found )
            break;

         ++mi;
      }

      if( was_found )
      {
         rc = true;
         os << best_match;
      }
      else if( best_partial.empty( ) )
         rc = false;
      else
      {
         rc = true;
         os << best_partial;
      }
   }

   return rc;
}

void cube::train( const std::string& info )
{
   if( !info.empty( ) )
   {
      vector< string > lines;

      if( info[ 0 ] != '@' )
         lines.push_back( info );
      else
         buffer_file_lines( info.substr( 1 ), lines, false );

      // NOTE: If multiple lines then first erase any previous training.
      if( lines.size( ) > 1 )
      {
         multimap< string, string >::iterator mi;
         string type_key_prefix( type + c_type_separator );

         while( true )
         {
            mi = g_goal_algos.lower_bound( type_key_prefix );

            if( mi == g_goal_algos.end( ) || mi->first.find( type_key_prefix ) != 0 )
               break;

            g_goal_algos.erase( mi );
         }

         while( true )
         {
            mi = g_algo_checks.lower_bound( type_key_prefix );

            if( mi == g_algo_checks.end( ) || mi->first.find( type_key_prefix ) != 0 )
               break;

            g_algo_checks.erase( mi );
         }
      }

      for( size_t i = 0; i < lines.size( ); i++ )
      {
         string next_line( lines[ i ] );

         if( next_line.empty( ) || next_line[ 0 ] == '#' )
            continue;

         vector< string > train_args;
         split( next_line, train_args, ' ' );

         if( train_args.size( ) < 3 )
            throw runtime_error( "unexpected train args < 3" );

         if( train_args.size( ) == 3 )
            train_algo( train_args[ 0 ], train_args[ 1 ], train_args[ 2 ] );
         else if( train_args.size( ) == 4 )
            train_algo( train_args[ 0 ],
             train_args[ 1 ], train_args[ 2 ], from_string< size_t >( train_args[ 3 ] ) );
         else
            train_algo( train_args[ 0 ], train_args[ 1 ],
             train_args[ 2 ], from_string< size_t >( train_args[ 3 ] ), from_string< size_t >( train_args[ 4 ] ) );
      }
   }
}

void cube::train_algo( const string& pat,
 const string& goal, const string& algo, size_t rounds,
 size_t max_tries_allowed, bool* p_can_keep, bool* p_found_match )
{
   cube tmp_cube( *this );

   string mask;
   vector< string > all_masks;

   if( pat.empty( ) )
      throw runtime_error( "invalid empty pat for test" );

   size_t max_tries = rounds;

   if( p_can_keep )
      *p_can_keep = false;

   bool found_match = false;
   bool found_repeat = false;
   bool found_partial = false;

   vector< string > patterns;

   split( pat, patterns, '|' );

   bool is_full_state = false;

   bool using_random_patterns = false;

   if( algo.find( '*' ) != string::npos )
      using_random_patterns = true;
   else
      tmp_cube.scramble( );

   string scramble( tmp_cube.get_state( false ) );

   if( goal.length( ) < scramble.length( ) )
      throw runtime_error( "invalid goal length < scramble length" );

   // NOTE: If there is only one pattern whose length matches the goal's
   // then it's expected that the current state will match this pattern.
   if( patterns.size( ) == 1 && patterns[ 0 ].size( ) == goal.size( ) )
   {
      is_full_state = true;

      tmp_cube = cube( patterns[ 0 ] );

      if( !using_random_patterns )
      {
         rounds = 1;
         scramble = goal;
      }

      for( size_t i = 0; i < goal.size( ); i++ )
      {
         if( goal[ i ] == '.' )
            mask += '-';
         else
            mask += '*';
      }
   }

   found_match = has_found_goal( scramble, goal );
   found_partial = has_partial_goal( scramble, goal );

   if( rounds > c_max_rounds )
      throw runtime_error( "max rounds for train_algo exceeded" );

   string algx( algo );

#ifdef DEBUG
   cout << "algo = " << algo << endl;
   cout << "pat >>\n" << pat << endl;
#endif
   for( size_t i = 0; i < rounds; i++ )
   {
      if( using_random_patterns )
      {
         tmp_cube = cube( scramble );
         algx = expand_pattern( tmp_cube, algo );
      }

      tmp_cube.perform_moves( algx );
      string next( tmp_cube.get_state( false ) );

      if( next == scramble || using_random_patterns )
      {
         if( !found_repeat && !using_random_patterns )
         {
            max_tries = i;

            if( i == 0 )
               ++max_tries;
            else
               found_repeat = true;
         }
         else if( using_random_patterns )
         {
            max_tries = 1;
            found_match = has_found_goal( next, goal );
         }

         if( found_match )
            replace( mask, "?", "*" );
         else if( !found_partial )
            replace( mask, "?", "-" );

         all_masks.push_back( mask );

         if( !using_random_patterns )
         {
            if( is_full_state )
               break;

            mask.erase( );
            found_partial = false;

            tmp_cube.scramble( );
            next = scramble = tmp_cube.get_state( false );

            found_match = has_found_goal( scramble, goal );
            found_partial = has_partial_goal( next, goal );
         }
      }

      if( using_random_patterns )
      {
         if( found_match )
            break;
      }
      else
      {
         if( is_full_state )
            break;

         bool had_match = false;
         bool had_partial = false;

         for( size_t j = 0; j < scramble.length( ); j++ )
         {
            bool potential = false;

            for( size_t k = 0; k < patterns.size( ); k++ )
            {
               string next_pat( patterns[ k ] );

               if( j <= next.length( ) - next_pat.length( ) )
               {
                  if( matches_pattern( next_pat, next.substr( j, next_pat.length( ) ) ) )
                  {
                     potential = true;
                     break;
                  }
               }
            }

            if( goal[ j ] != '.' )
            {
               if( next[ j ] != goal[ j ] )
                  had_match = false;
               else
                  had_partial = true;
            }

            if( mask.length( ) < scramble.length( ) )
            {
               if( next[ j ] == goal[ j ] )
                  mask += '*';
               else if( potential )
                  mask += '?';
               else
                  mask += '-';
            }
            else
            {
               if( next[ j ] == goal[ j ] )
                  mask[ j ] = '*';
               else if( potential )
                  mask[ j ] = '?';
               else
                  mask[ j ] = '-';
            }
         }

         if( had_partial )
            found_partial = true;
      }
   }

   if( !is_full_state )
   {
      replace( mask, "?", "-" );
      replace( mask, "*", "-" );

      vector< size_t > weights( mask.size( ) );

      size_t total_weight = 0;

      for( size_t i = 0; i < all_masks.size( ); i++ )
      {
         for( size_t j = 0; j < all_masks[ i ].size( ); j++ )
         {
            size_t weight = 0;

            if( all_masks[ i ][ j ] == '?' )
               weight = 1;
            else if( all_masks[ i ][ j ] == '*' )
               weight = 2;

            total_weight += weight;
            weights[ j ] = weights[ j ] + weight;
         }
      }

      size_t median = total_weight / weights.size( );

      mask.erase( );
      for( size_t i = 0; i < weights.size( ); i++ )
      {
         if( weights[ i ] > median )
            mask += "*";
         else
            mask += "-";
      }
   }

#ifdef DEBUG
   if( using_random_patterns )
   {
      if( found_match )
         cout << algx << endl;
   }
   cout << goal << endl;
   cout << mask << endl;
#endif

   if( p_found_match )
      *p_found_match = found_match;

   if( mask.find( '*' ) != string::npos )
   {
      if( p_can_keep )
         *p_can_keep = true;
      else
      {
         g_goal_algos.insert( make_pair( type + c_type_separator + goal, algx ) );

         g_algo_checks.insert(
          make_pair( type + c_type_separator + algx,
          mask + "=" + to_string( min( max_tries, max_tries_allowed ) ) ) );
      }
   }
}

void cube::attempt( ostream& os, const string& info )
{
   vector< string > attempt_args;
   split( info, attempt_args, ' ' );

   if( attempt_args.size( ) < 2 )
      throw runtime_error( "attempt args < 2" );

   if( attempt_args.size( ) == 2 )
      attempt_own_algo( os, attempt_args[ 0 ], attempt_args[ 1 ] );
   else
      attempt_own_algo( os, attempt_args[ 0 ], attempt_args[ 1 ], from_string< size_t >( attempt_args[ 2 ] ) );
}

void cube::attempt_own_algo( ostream& os, const string& pat, const string& goal, size_t max_moves )
{
   bool can_keep = false;
   bool found_match = false;

   string last_retained;

   for( size_t i = 1; i <= max_moves; i++ )
   {
      for( size_t j = 0; j < 100; j++ )
      {
         ostringstream osstr;
         scramble( &osstr, i, false );
         string next_algo( osstr.str( ) );

         int rounds = c_cube_default_train_rounds;

         if( i == 4 )
            rounds = 400;
         else if( i == 5 )
            rounds = 900;

         train_algo( pat, goal, next_algo, rounds, c_cube_default_max_tries, &can_keep, &found_match );

         if( can_keep )
         {
            cube tmp_cube( *this );
            tmp_cube.perform_moves( next_algo );

            if( is_closer_to_goal( tmp_cube.get_state( false ), get_state( false ), goal ) )
            {
               last_retained = next_algo;
               break;
            }
         }

         if( found_match )
            break;

         if( i == 1 && j > 4 )
            break;

         if( i == 2 && j > 9 )
            break;

         if( i == 3 && j > 24 )
            break;

         if( i == 4 && j > 49 )
            break;
      }

      if( found_match || !last_retained.empty( ) )
         break;
   }

   if( !last_retained.empty( ) )
      os << last_retained;
}

void cube::output_algos( ostream& os, size_t algo_padding_size )
{
   string type_key_prefix( type + c_type_separator );

   multimap< string, string >::iterator mi;

   mi = g_goal_algos.lower_bound( type_key_prefix );

   while( mi != g_goal_algos.end( ) )
   {
      if( mi->first.find( type_key_prefix ) != 0 )
         break;

      os << mi->first.substr( type_key_prefix.length( ) ) << " " << mi->second << '\n';
      ++mi;
   }

   mi = g_algo_checks.lower_bound( type_key_prefix );

   while( mi != g_algo_checks.end( ) )
   {
      if( mi->first.find( type_key_prefix ) != 0 )
         break;

      string separator( " " );

      if( algo_padding_size && mi->first.length( ) < algo_padding_size )
         separator += string( algo_padding_size - mi->first.length( ), ' ' );

      os << mi->first.substr( type_key_prefix.length( ) ) << separator << mi->second << '\n';
      ++mi;
   }
}

void cube::swap_row( string& lhs, string& rhs, int num )
{
   vector< string > lhs_rows;
   vector< string > rhs_rows;

   int cubies = ( int )sqrt( ( float )lhs.length( ) );
   int num_cubies_per_edge = ( lhs.length( ) / cubies );

   for( size_t i = 0; i < num_cubies_per_edge; i++ )
   {
      string next_row;
      for( size_t j = 0; j < num_cubies_per_edge; j++ )
         next_row += lhs[ ( i * num_cubies_per_edge ) + j ];

      lhs_rows.push_back( next_row );
   }

   for( size_t i = 0; i < num_cubies_per_edge; i++ )
   {
      string next_row;
      for( size_t j = 0; j < num_cubies_per_edge; j++ )
         next_row += rhs[ ( i * num_cubies_per_edge ) + j ];

      rhs_rows.push_back( next_row );
   }

   string temp_row( lhs_rows[ num ] );

   lhs_rows[ num ] = rhs_rows[ num ];
   rhs_rows[ num ] = temp_row;

   lhs.erase( );
   rhs.erase( );

   for( size_t i = 0; i < lhs_rows.size( ); i++ )
      lhs += lhs_rows[ i ];

   for( size_t i = 0; i < rhs_rows.size( ); i++ )
      rhs += rhs_rows[ i ];
}

void cube::swap_column( string& lhs, string& rhs, int lnum, int rnum )
{
   vector< string > lhs_columns;
   vector< string > rhs_columns;

   if( rnum < 0 )
      rnum = lnum;

   int cubies = ( int )sqrt( ( float )lhs.length( ) );
   int num_cubies_per_edge = ( lhs.length( ) / cubies );

   for( size_t i = 0; i < num_cubies_per_edge; i++ )
   {
      string next_column;
      for( size_t j = 0; j < num_cubies_per_edge; j++ )
         next_column += lhs[ ( j * num_cubies_per_edge ) + i ];

      lhs_columns.push_back( next_column );
   }

   for( size_t i = 0; i < num_cubies_per_edge; i++ )
   {
      string next_column;
      for( size_t j = 0; j < num_cubies_per_edge; j++ )
         next_column += rhs[ ( j * num_cubies_per_edge ) + i ];

      rhs_columns.push_back( next_column );
   }

   string tmp_column( lhs_columns[ lnum ] );

   lhs_columns[ lnum ] = rhs_columns[ rnum ];
   rhs_columns[ rnum ] = tmp_column;

   lhs.erase( );
   rhs.erase( );

   for( size_t i = 0; i < num_cubies_per_edge; i++ )
   {
      for( size_t j = 0; j < lhs_columns.size( ); j++ )
         lhs += lhs_columns[ j ][ i ];
   }

   for( size_t i = 0; i < num_cubies_per_edge; i++ )
   {
      for( size_t j = 0; j < rhs_columns.size( ); j++ )
         rhs += rhs_columns[ j ][ i ];
   }
}

void cube::swap_row_with_column( string& lhs, string& rhs, int rnum, int cnum )
{
   if( cnum < 0 )
      cnum = rnum;

   vector< string > lhs_rows;
   vector< string > rhs_columns;

   int cubies = ( int )sqrt( ( float )lhs.length( ) );
   int num_cubies_per_edge = ( lhs.length( ) / cubies );

   for( size_t i = 0; i < num_cubies_per_edge; i++ )
   {
      string next_row;
      for( size_t j = 0; j < num_cubies_per_edge; j++ )
         next_row += lhs[ ( i * num_cubies_per_edge ) + j ];

      lhs_rows.push_back( next_row );
   }

   for( size_t i = 0; i < num_cubies_per_edge; i++ )
   {
      string next_column;
      for( size_t j = 0; j < num_cubies_per_edge; j++ )
         next_column += rhs[ ( j * num_cubies_per_edge ) + i ];

      rhs_columns.push_back( next_column );
   }

   string tmp_row( lhs_rows[ rnum ] );

   lhs_rows[ rnum ] = rhs_columns[ cnum ];
   rhs_columns[ cnum ] = tmp_row;

   lhs.erase( );
   rhs.erase( );

   for( size_t i = 0; i < lhs_rows.size( ); i++ )
      lhs += lhs_rows[ i ];

   for( size_t i = 0; i < num_cubies_per_edge; i++ )
   {
      for( size_t j = 0; j < rhs_columns.size( ); j++ )
         rhs += rhs_columns[ j ][ i ];
   }
}

void cube::reverse_row( string& data, int num )
{
   vector< string > rows;

   int cubies = ( int )sqrt( ( float )data.length( ) );
   int num_cubies_per_edge = ( data.length( ) / cubies );

   for( size_t i = 0; i < num_cubies_per_edge; i++ )
   {
      string next_row;
      for( size_t j = 0; j < num_cubies_per_edge; j++ )
         next_row += data[ ( i * num_cubies_per_edge ) + j ];

      if( i == num )
         reverse( next_row.begin( ), next_row.end( ) );

      rows.push_back( next_row );
   }

   data.erase( );

   for( size_t i = 0; i < rows.size( ); i++ )
      data += rows[ i ];
}

void cube::reverse_column( string& data, int num )
{
   vector< string > columns;

   int cubies = ( int )sqrt( ( float )data.length( ) );
   int num_cubies_per_edge = ( data.length( ) / cubies );

   for( size_t i = 0; i < num_cubies_per_edge; i++ )
   {
      string next_column;
      for( size_t j = 0; j < num_cubies_per_edge; j++ )
         next_column += data[ ( j * num_cubies_per_edge ) + i ];

      if( i == num )
         reverse( next_column.begin( ), next_column.end( ) );

      columns.push_back( next_column );
   }

   data.erase( );

   for( size_t i = 0; i < num_cubies_per_edge; i++ )
   {
      for( size_t j = 0; j < columns.size( ); j++ )
         data += columns[ j ][ i ];
   }
}

void cube::rotate_face( string& data, bool clockwise )
{
   int cubies = sqrt( ( float )data.length( ) );
   int num_cubies_per_edge = ( data.length( ) / cubies );

   vector< string > rows;
   deque< string > columns;

   if( clockwise )
   {
      for( size_t i = 0; i < num_cubies_per_edge; i++ )
      {
         string next_row;
         for( size_t j = num_cubies_per_edge; j > 0; j-- )
            next_row += data[ ( ( j - 1 ) * num_cubies_per_edge ) + i ];

         rows.push_back( next_row );
      }
   }
   else
   {
      for( size_t i = 0; i < num_cubies_per_edge; i++ )
      {
         string next_column;
         for( size_t j = 0; j < num_cubies_per_edge; j++ )
            next_column += data[ ( j * num_cubies_per_edge ) + i ];

         columns.push_front( next_column );
      }
   }

   data.erase( );

   if( clockwise )
   {
      for( size_t i = 0; i < rows.size( ); i++ )
         data += rows[ i ];
   }
   else
   {
      for( size_t i = 0; i < columns.size( ); i++ )
         data += columns[ i ];
   }
}

void cube::output_side_info( ostream& os, const string& name, const string& data ) const
{
   int cubies = ( int )sqrt( ( float )data.length( ) );

   if( !name.empty( ) )
      os << "[" << name << "]" << '\n';

   int offset = 0;

   for( int i = 0; i < cubies; i++ )
   {
      if( !name.empty( ) )
         os << ' ';

      os << data.substr( offset, ( int )cubies ) << '\n';

      offset += cubies;
   }
}

