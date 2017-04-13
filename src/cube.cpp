// Copyright (c) 2012-2017 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cmath>
#  include <deque>
#  include <vector>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#include "cube.h"

#include "utilities.h"

using namespace std;

namespace
{

const char* const c_type_2_2_2 = "2x2x2";
const char* const c_type_3_3_3 = "3x3x3";
const char* const c_type_4_4_4 = "4x4x4";
const char* const c_type_5_5_5 = "5x5x5";
const char* const c_type_6_6_6 = "6x6x6";
const char* const c_type_7_7_7 = "7x7x7";

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
   else if( type == c_type_6_6_6 )
      num_per_side = 36;
   else if( type == c_type_7_7_7 )
      num_per_side = 49;

   string y( num_per_side, 'Y' );
   string w( num_per_side, 'W' );
   string b( num_per_side, 'B' );
   string g( num_per_side, 'G' );
   string r( num_per_side, 'R' );
   string o( num_per_side, 'O' );

   return y + w + b + g + r + o;
}

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
       && type_or_state != c_type_4_4_4 && type_or_state != c_type_5_5_5
       && type_or_state != c_type_6_6_6 && type_or_state != c_type_7_7_7 )
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
      else if( num_per_side == 36 )
         type = c_type_6_6_6;
      else if( num_per_side == 49 )
         type = c_type_7_7_7;
      else
         throw runtime_error( "unsupported num_per_side = " + to_string( num_per_side ) );

      if( initial.empty( ) )
         initial = state;

      top.clear( );
      top = state.substr( offset, num_per_side );

      bot.clear( );
      offset += num_per_side;
      bot = state.substr( offset, num_per_side );

      lft.clear( );
      offset += num_per_side;
      lft = state.substr( offset, num_per_side );

      rgt.clear( );
      offset += num_per_side;
      rgt = state.substr( offset, num_per_side );

      fnt.clear( );
      offset += num_per_side;
      fnt = state.substr( offset, num_per_side );

      bck.clear( );
      offset += num_per_side;
      bck = state.substr( offset, num_per_side );
   }
}

void cube::reset( )
{
   init( initial );
}

void cube::scramble( ostream* p_os )
{
   vector< string > available_moves;

   available_moves.push_back( "B" );
   available_moves.push_back( "D" );
   available_moves.push_back( "F" );
   available_moves.push_back( "L" );
   available_moves.push_back( "R" );
   available_moves.push_back( "U" );

   if( type != c_type_2_2_2 )
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
      available_moves.push_back( "Dw" );
      available_moves.push_back( "Lw" );
      available_moves.push_back( "Uw" );
      available_moves.push_back( "Rw" );
   }

   vector< string > moves;

   size_t range = available_moves.size( );

   int scramble_moves = 10;

   if( type == c_type_3_3_3 )
      scramble_moves = 20;
   else if( type == c_type_4_4_4 )
      scramble_moves = 40;
   else if( type == c_type_5_5_5 )
      scramble_moves = 80;
   else if( type == c_type_6_6_6 )
      scramble_moves = 160;
   else if( type == c_type_7_7_7 )
      scramble_moves = 320;

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

      rotate( moves[ i ] );
   }

   if( p_os )
      *p_os << '\n';
}

void cube::output_sides( ostream& os ) const
{
   output_side_info( os, "top", top );
   output_side_info( os, "bot", bot );
   output_side_info( os, "lft", lft );
   output_side_info( os, "rgt", rgt );
   output_side_info( os, "fnt", fnt );
   output_side_info( os, "bck", bck );
}

string cube::get_state( ) const
{
   return top + bot + lft + rgt + fnt + bck + ':' + initial;
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

void cube::rotate( const string& op )
{
   if( !op.empty( ) )
   {
      if( op[ 0 ] == 'U' || op[ 0 ] == 'u' )
         rotate_top( op );
      else if( op[ 0 ] == 'B' || op[ 0 ] == 'b' )
         rotate_back( op );
      else if( op[ 0 ] == 'L' || op[ 0 ] == 'l' )
         rotate_left( op );
      else if( op[ 0 ] == 'F' || op[ 0 ] == 'f' )
         rotate_front( op );
      else if( op[ 0 ] == 'R' || op[ 0 ] == 'r' )
         rotate_right( op );
      else if( op[ 0 ] == 'D' || op[ 0 ] == 'd' )
         rotate_bottom( op );
      else
         throw runtime_error( "unknown rotate op: " + op );
   }
}

void cube::rotate_top( const string& op )
{
   string op_val( op );

   int num_cubies_per_edge = ( int )sqrt( ( float )lft.size( ) );

   if( num_cubies_per_edge < 2 )
      throw runtime_error( "unexpected num_cubies_per_edge < 2" );
   else
   {
      if( op.empty( ) )
         throw runtime_error( "unexpected empty op for rotate_top" );
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

            if( op_val == "U" || op_val == "Uu" || op_val == "Uw" )
            {
               if( clockwise )
               {
                  swap_row( fnt, rgt, 0 );
                  swap_row( rgt, bck, 0 );
                  swap_row( bck, lft, 0 );
                  swap_row( lft, old_fnt, 0 );

                  if( op_val == "Uu" )
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
                  swap_row( fnt, lft, 0 );
                  swap_row( lft, bck, 0 );
                  swap_row( bck, rgt, 0 );
                  swap_row( rgt, old_fnt, 0 );

                  if( op_val == "Uu" )
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

               rotate_face( top, clockwise );
            }
            else if( op_val == "u" )
            {
               if( clockwise )
               {
                  swap_row( fnt, rgt, 1 );
                  swap_row( rgt, bck, 1 );
                  swap_row( bck, lft, 1 );
                  swap_row( lft, old_fnt, 1 );
               }
               else
               {
                  swap_row( fnt, lft, 1 );
                  swap_row( lft, bck, 1 );
                  swap_row( bck, rgt, 1 );
                  swap_row( rgt, old_fnt, 1 );
               }
            }
         }
      }
   }
}

void cube::rotate_left( const string& op )
{
   string op_val( op );

   int num_cubies_per_edge = ( int )sqrt( ( float )top.size( ) );

   if( num_cubies_per_edge < 2 )
      throw runtime_error( "unexpected num_cubies_per_edge < 2" );
   else
   {
      if( op.empty( ) )
         throw runtime_error( "unexpected empty op for rotate_left" );
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

            if( op_val == "L" || op_val == "Ll" || op_val == "Lw" )
            {
               if( clockwise )
               {
                  swap_column( fnt, top, 0 );
                  swap_column( top, bck, 0, num_cubies_per_edge - 1 );
                  swap_column( bck, bot, num_cubies_per_edge - 1, 0 );
                  swap_column( bot, old_fnt, 0 );

                  reverse_column( top, 0 );
                  reverse_column( bck, num_cubies_per_edge - 1 );

                  if( op_val == "Ll" )
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
                  swap_column( fnt, bot, 0 );
                  swap_column( bot, bck, 0, num_cubies_per_edge - 1 );
                  swap_column( bck, top, num_cubies_per_edge - 1, 0 );
                  swap_column( top, old_fnt, 0 );

                  reverse_column( bot, 0 );
                  reverse_column( bck, num_cubies_per_edge - 1 );

                  if( op_val == "Ll" )
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

               rotate_face( lft, clockwise );
            }
            else if( op_val == "l" )
            {
               if( clockwise )
               {
                  swap_column( fnt, top, 1 );
                  swap_column( top, bck, 1, num_cubies_per_edge - 1 );
                  swap_column( bck, bot, num_cubies_per_edge - 1, 1 );
                  swap_column( bot, old_fnt, 1 );
               }
               else
               {
                  swap_column( fnt, bot, 1 );
                  swap_column( bot, bck, 1, num_cubies_per_edge - 1 );
                  swap_column( bck, top, num_cubies_per_edge - 1, 1 );
                  swap_column( top, old_fnt, 1 );
               }
            }
         }
      }
   }
}

void cube::rotate_back( const string& op )
{
   string op_val( op );

   int num_cubies_per_edge = ( int )sqrt( ( float )lft.size( ) );

   if( num_cubies_per_edge < 2 )
      throw runtime_error( "unexpected num_cubies_per_edge < 2" );
   else
   {
      if( op.empty( ) )
         throw runtime_error( "unexpected empty op for rotate_back" );
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

            if( op_val == "B" || op_val == "Bb" )
            {
               if( !clockwise )
               {
                  swap_row_with_column( bot, lft, num_cubies_per_edge - 1, 0 );
                  swap_row_with_column( bot, rgt, num_cubies_per_edge - 1 );
                  swap_row_with_column( top, rgt, 0, num_cubies_per_edge - 1 );
                  swap_row_with_column( top, old_lft, 0 );

                  reverse_row( top, 0 );
                  reverse_row( bot, num_cubies_per_edge - 1 );

                  if( op_val == "Bb" )
                  {
                     swap_row_with_column( bot, lft, num_cubies_per_edge - 2, 1 );
                     swap_row_with_column( bot, rgt, num_cubies_per_edge - 2 );
                     swap_row_with_column( top, rgt, 1, num_cubies_per_edge - 2 );
                     swap_row_with_column( top, old_lft, 1 );

                     reverse_row( top, 1 );
                     reverse_row( bot, num_cubies_per_edge - 2 );
                  }
               }
               else
               {
                  swap_row_with_column( bot, rgt, num_cubies_per_edge - 1 );
                  swap_row_with_column( bot, lft, num_cubies_per_edge - 1, 0 );
                  swap_row_with_column( top, lft, 0 );
                  swap_row_with_column( top, old_rgt, 0, num_cubies_per_edge - 1 );

                  reverse_column( lft, 0 );
                  reverse_column( rgt, num_cubies_per_edge - 1 );

                  if( op_val == "Bb" )
                  {
                     swap_row_with_column( bot, rgt, num_cubies_per_edge - 2 );
                     swap_row_with_column( bot, lft, num_cubies_per_edge - 2, 1 );
                     swap_row_with_column( top, lft, 1 );
                     swap_row_with_column( top, old_rgt, 1, num_cubies_per_edge - 2 );

                     reverse_column( lft, 1 );
                     reverse_column( rgt, num_cubies_per_edge - 2 );
                  }
               }

               rotate_face( bck, clockwise );
            }
            else if( op_val == "b" )
            {
               if( !clockwise )
               {
                  swap_row_with_column( bot, lft, num_cubies_per_edge - 2, 1 );
                  swap_row_with_column( bot, rgt, num_cubies_per_edge - 2 );
                  swap_row_with_column( top, rgt, 1, num_cubies_per_edge - 2 );
                  swap_row_with_column( top, old_lft, 1 );

                  reverse_row( top, 1 );
                  reverse_row( bot, num_cubies_per_edge - 2 );
               }
               else
               {
                  swap_row_with_column( bot, rgt, num_cubies_per_edge - 2 );
                  swap_row_with_column( bot, lft, num_cubies_per_edge - 2, 1 );
                  swap_row_with_column( top, lft, 1 );
                  swap_row_with_column( top, old_rgt, 1, num_cubies_per_edge - 2 );

                  reverse_column( lft, 1 );
                  reverse_column( rgt, num_cubies_per_edge - 2 );
               }
            }
         }
      }
   }
}

void cube::rotate_front( const string& op )
{
   string op_val( op );

   int num_cubies_per_edge = ( int )sqrt( ( float )lft.size( ) );

   if( num_cubies_per_edge < 2 )
      throw runtime_error( "unexpected num_cubies_per_edge < 2" );
   else
   {
      if( op.empty( ) )
         throw runtime_error( "unexpected empty op for rotate_front" );
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

            if( op_val == "F" || op_val == "Ff" )
            {
               if( clockwise )
               {
                  swap_row_with_column( bot, lft, 0, num_cubies_per_edge - 1 );
                  swap_row_with_column( bot, rgt, 0 );
                  swap_row_with_column( top, rgt, num_cubies_per_edge - 1, 0 );
                  swap_row_with_column( top, old_lft, num_cubies_per_edge - 1 );

                  reverse_row( bot, 0 );
                  reverse_row( top, num_cubies_per_edge - 1 );

                  if( op_val == "Ff" )
                  {
                     swap_row_with_column( bot, lft, 1, num_cubies_per_edge - 2 );
                     swap_row_with_column( bot, rgt, 1 );
                     swap_row_with_column( top, rgt, num_cubies_per_edge - 2, 1 );
                     swap_row_with_column( top, old_lft, num_cubies_per_edge - 2 );

                     reverse_row( bot, 1 );
                     reverse_row( top, num_cubies_per_edge - 2 );
                  }
               }
               else
               {
                  swap_row_with_column( bot, rgt, 0 );
                  swap_row_with_column( bot, lft, 0, num_cubies_per_edge - 1 );
                  swap_row_with_column( top, lft, num_cubies_per_edge - 1 );
                  swap_row_with_column( top, old_rgt, num_cubies_per_edge - 1, 0 );

                  reverse_column( rgt, 0 );
                  reverse_column( lft, num_cubies_per_edge - 1 );

                  if( op_val == "Ff" )
                  {
                     swap_row_with_column( bot, rgt, 1 );
                     swap_row_with_column( bot, lft, 1, num_cubies_per_edge - 2 );
                     swap_row_with_column( top, lft, num_cubies_per_edge - 2 );
                     swap_row_with_column( top, old_rgt, num_cubies_per_edge - 2, 1 );

                     reverse_column( rgt, 1 );
                     reverse_column( lft, num_cubies_per_edge - 2 );
                  }
               }

               rotate_face( fnt, clockwise );
            }
            else if( op_val == "f" )
            {
               if( clockwise )
               {
                  swap_row_with_column( bot, lft, 1, num_cubies_per_edge - 2 );
                  swap_row_with_column( bot, rgt, 1 );
                  swap_row_with_column( top, rgt, num_cubies_per_edge - 2, 1 );
                  swap_row_with_column( top, old_lft, num_cubies_per_edge - 2 );

                  reverse_row( bot, 1 );
                  reverse_row( top, num_cubies_per_edge - 2 );
               }
               else
               {
                  swap_row_with_column( bot, rgt, 1 );
                  swap_row_with_column( bot, lft, 1, num_cubies_per_edge - 2 );
                  swap_row_with_column( top, lft, num_cubies_per_edge - 2 );
                  swap_row_with_column( top, old_rgt, num_cubies_per_edge - 2, 1 );

                  reverse_column( rgt, 1 );
                  reverse_column( lft, num_cubies_per_edge - 2 );
               }
            }
         }
      }
   }
}

void cube::rotate_right( const string& op )
{
   string op_val( op );

   int num_cubies_per_edge = ( int )sqrt( ( float )top.size( ) );

   if( num_cubies_per_edge < 2 )
      throw runtime_error( "unexpected num_cubies_per_edge < 2" );
   else
   {
      if( op.empty( ) )
         throw runtime_error( "unexpected empty op for rotate_right" );
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

            if( op_val == "R" || op_val == "Rr" || op_val == "Rw" )
            {
               if( !clockwise )
               {
                  swap_column( fnt, top, num_cubies_per_edge - 1 );
                  swap_column( top, bck, num_cubies_per_edge - 1, 0 );
                  swap_column( bck, bot, 0, num_cubies_per_edge - 1 );
                  swap_column( bot, old_fnt, num_cubies_per_edge - 1 );

                  reverse_column( bck, 0 );
                  reverse_column( top, num_cubies_per_edge - 1 );

                  if( op_val == "Rr" )
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
                  swap_column( fnt, bot, num_cubies_per_edge - 1 );
                  swap_column( bot, bck, num_cubies_per_edge - 1, 0 );
                  swap_column( bck, top, 0, num_cubies_per_edge - 1 );
                  swap_column( top, old_fnt, num_cubies_per_edge - 1 );

                  reverse_column( bck, 0 );
                  reverse_column( bot, num_cubies_per_edge - 1 );

                  if( op_val == "Rr" )
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

               rotate_face( rgt, clockwise );
            }
            else if( op_val == "r" )
            {
               if( !clockwise )
               {
                  swap_column( fnt, top, num_cubies_per_edge - 2 );
                  swap_column( top, bck, num_cubies_per_edge - 2, 1 );
                  swap_column( bck, bot, 1, num_cubies_per_edge - 2 );
                  swap_column( bot, old_fnt, num_cubies_per_edge - 2 );
               }
               else
               {
                  swap_column( fnt, bot, num_cubies_per_edge - 2 );
                  swap_column( bot, bck, num_cubies_per_edge - 2, 1 );
                  swap_column( bck, top, 1, num_cubies_per_edge - 2 );
                  swap_column( top, old_fnt, num_cubies_per_edge - 2 );
               }
            }
         }
      }
   }
}

void cube::rotate_bottom( const string& op )
{
   string op_val( op );
   int num_cubies_per_side = ( int )sqrt( ( float )lft.size( ) );

   if( num_cubies_per_side < 2 )
      throw runtime_error( "unexpected num_cubies_per_side < 2" );
   else
   {
      if( op.empty( ) )
         throw runtime_error( "unexpected empty op for rotate_bottom" );
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

            if( op_val == "D" || op_val == "Dd" || op_val == "Dw" )
            {
               if( !clockwise )
               {
                  swap_row( fnt, rgt, num_cubies_per_side - 1 );
                  swap_row( rgt, bck, num_cubies_per_side - 1 );
                  swap_row( bck, lft, num_cubies_per_side - 1 );
                  swap_row( lft, old_fnt, num_cubies_per_side - 1 );

                  if( op_val == "Dd" )
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
                  swap_row( fnt, lft, num_cubies_per_side - 1 );
                  swap_row( lft, bck, num_cubies_per_side - 1 );
                  swap_row( bck, rgt, num_cubies_per_side - 1 );
                  swap_row( rgt, old_fnt, num_cubies_per_side - 1 );

                  if( op_val == "Dd" )
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

               rotate_face( bot, clockwise );
            }
            else if( op_val == "d" )
            {
               if( !clockwise )
               {
                  swap_row( fnt, rgt, num_cubies_per_side - 2 );
                  swap_row( rgt, bck, num_cubies_per_side - 2 );
                  swap_row( bck, lft, num_cubies_per_side - 2 );
                  swap_row( lft, old_fnt, num_cubies_per_side - 2 );
               }
               else
               {
                  swap_row( fnt, lft, num_cubies_per_side - 2 );
                  swap_row( lft, bck, num_cubies_per_side - 2 );
                  swap_row( bck, rgt, num_cubies_per_side - 2 );
                  swap_row( rgt, old_fnt, num_cubies_per_side - 2 );
               }
            }
         }
      }
   }
}

void cube::perform_rotations( const string& ops )
{
   vector< string > rotate_ops;

   if( !ops.empty( ) )
   {
      if( ops.find( ' ' ) == string::npos )
         split( ops, rotate_ops );
      else
         split( ops, rotate_ops, ' ' );
   }

   for( size_t i = 0; i < rotate_ops.size( ); i++ )
      rotate( rotate_ops[ i ] );
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

