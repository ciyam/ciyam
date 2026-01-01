// Copyright (c) 2017-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
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

op_algo_handler* cube::create_clone( ) const
{
   return new cube( *this );
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
   string all_moves( scramble_moves( num_moves, actually_scramble ) );

   if( p_os )
      *p_os << all_moves;

   if( actually_scramble )
      perform_moves( all_moves );
}

string cube::scramble_moves( size_t num_moves, bool for_actual_scramble ) const
{
   vector< string > available_moves;

   available_moves.push_back( "B" );
   available_moves.push_back( "D" );
   available_moves.push_back( "F" );
   available_moves.push_back( "L" );
   available_moves.push_back( "R" );
   available_moves.push_back( "U" );

   if( !for_actual_scramble )
   {
      available_moves.push_back( "X" );
      available_moves.push_back( "Y" );
      available_moves.push_back( "Z" );
   }

   if( type != c_type_2_2_2 )
   {
      available_moves.push_back( "M" );
      available_moves.push_back( "E" );
      available_moves.push_back( "S" );
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

      available_moves.push_back( "Bw" );
      available_moves.push_back( "Dw" );
      available_moves.push_back( "Fw" );
      available_moves.push_back( "Lw" );
      available_moves.push_back( "Rw" );
      available_moves.push_back( "Uw" );
   }

   vector< string > moves;

   size_t range = available_moves.size( );

   size_t scramble_moves = num_moves;

   if( scramble_moves <= 0 )
   {
      scramble_moves = 12;

      if( type == c_type_3_3_3 )
         scramble_moves = 20;
      else if( type == c_type_4_4_4 )
         scramble_moves = 30;
      else if( type == c_type_5_5_5 )
         scramble_moves = 40;
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

   string all_moves;

   for( size_t i = 0; i < moves.size( ); i++ )
   {
      if( i > 0 )
         all_moves += ' ';
      all_moves += moves[ i ];
   }

   all_moves = cleanup_output( all_moves );

   return all_moves;
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
               throw runtime_error( "unsupported cube op " + op );
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
               throw runtime_error( "unsupported cube op " + op );
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
               throw runtime_error( "unsupported cube op " + op );
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
               throw runtime_error( "unsupported cube op " + op );
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
               throw runtime_error( "unsupported cube op " + op );
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
               throw runtime_error( "unsupported cube op " + op );
         }
      }
   }
}

void cube::perform_moves( const string& ops )
{
   string all_ops( ops );

   bool reverse_ops = false;

   if( !all_ops.empty( ) && all_ops[ 0 ] == '~' )
   {
      reverse_ops = true;
      all_ops.erase( 0, 1 );
   }

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
   else if( type == c_type_4_4_4 || type == c_type_5_5_5 )
   {
      replace( all_ops, "M'", "Rw R'", "M", "Rw' R" );
      replace( all_ops, "E'", "Dw' D", "E", "Dw D'" );
      replace( all_ops, "S'", "Fw' F", "S", "Fw F'" );
   }

   replace( all_ops, ",", " " );

   vector< string > move_ops;

   if( !all_ops.empty( ) )
      split( all_ops, move_ops, ' ' );

   if( !reverse_ops )
   {
      for( size_t i = 0; i < move_ops.size( ); i++ )
         move( move_ops[ i ] );
   }
   else
   {
      reverse( move_ops.begin( ), move_ops.end( ) );

      for( size_t i = 0; i < move_ops.size( ); i++ )
      {
         string next_op( move_ops[ i ] );

         if( !next_op.empty( ) )
         {
            if( next_op[ next_op.length( ) - 1 ] == '\'' )
               next_op = next_op.substr( 0, next_op.length( ) - 1 );
            else if( next_op[ next_op.length( ) - 1 ] != '2' )
               next_op += '\'';

            move( next_op );
         }
      }
   }
}

string cube::random_ops( size_t num_ops ) const
{
   return scramble_moves( num_ops );
}

string cube::cleanup_output( const string& original ) const
{
   string output( " " + original + " " );

   size_t last_length = output.length( );

   while( true )
   {
      replace( output, " l l' ", " " );
      replace( output, " r r' ", " " );
      replace( output, " u u' ", " " );
      replace( output, " d d' ", " " );
      replace( output, " f f' ", " " );
      replace( output, " b b' ", " " );
      replace( output, " L L' ", " " );
      replace( output, " R R' ", " " );
      replace( output, " U U' ", " " );
      replace( output, " D D' ", " " );
      replace( output, " F F' ", " " );
      replace( output, " B B' ", " " );
      replace( output, " M M' ", " " );
      replace( output, " E E' ", " " );
      replace( output, " S S' ", " " );
      replace( output, " X X' ", " " );
      replace( output, " Y Y' ", " " );
      replace( output, " Z Z' ", " " );
      replace( output, " Ll Ll' ", " " );
      replace( output, " Rr Rr' ", " " );
      replace( output, " Uu Uu' ", " " );
      replace( output, " Dd Dd' ", " " );
      replace( output, " Ff Ff' ", " " );
      replace( output, " Bb Bb' ", " " );

      replace( output, " l' l ", " " );
      replace( output, " r' r ", " " );
      replace( output, " u' u ", " " );
      replace( output, " d' d ", " " );
      replace( output, " f' f ", " " );
      replace( output, " b' b ", " " );
      replace( output, " L' L ", " " );
      replace( output, " R' R ", " " );
      replace( output, " U' U ", " " );
      replace( output, " D' D ", " " );
      replace( output, " F' F ", " " );
      replace( output, " B' B ", " " );
      replace( output, " M' M ", " " );
      replace( output, " E' E ", " " );
      replace( output, " S' S ", " " );
      replace( output, " X' X ", " " );
      replace( output, " Y' Y ", " " );
      replace( output, " Z' Z ", " " );
      replace( output, " Ll' Ll ", " " );
      replace( output, " Rr' Rr ", " " );
      replace( output, " Uu' Uu ", " " );
      replace( output, " Dd' Dd ", " " );
      replace( output, " Ff' Ff ", " " );
      replace( output, " Bb' Bb ", " " );

      replace( output, " l l ", " l2 " );
      replace( output, " r r ", " r2 " );
      replace( output, " u u ", " u2 " );
      replace( output, " d d ", " d2 " );
      replace( output, " f f ", " f2 " );
      replace( output, " b b ", " b2 " );
      replace( output, " L L ", " L2 " );
      replace( output, " R R ", " R2 " );
      replace( output, " U U ", " U2 " );
      replace( output, " D D ", " D2 " );
      replace( output, " F F ", " F2 " );
      replace( output, " B B ", " B2 " );
      replace( output, " M M ", " M2 " );
      replace( output, " E E ", " E2 " );
      replace( output, " S S ", " S2 " );
      replace( output, " X X ", " X2 " );
      replace( output, " Y Y ", " Y2 " );
      replace( output, " Z Z ", " Z2 " );
      replace( output, " Ll Ll ", " Ll2 " );
      replace( output, " Rr Rr ", " Rr2 " );
      replace( output, " Uu Uu ", " Uu2 " );
      replace( output, " Dd Dd ", " Dd2 " );
      replace( output, " Ff Ff ", " Ff2 " );
      replace( output, " Bb Bb ", " Bb2 " );

      replace( output, " l l2 ", " l' " );
      replace( output, " r r2 ", " r' " );
      replace( output, " u u2 ", " u' " );
      replace( output, " d d2 ", " d' " );
      replace( output, " f f2 ", " f' " );
      replace( output, " b b2 ", " b' " );
      replace( output, " L L2 ", " L' " );
      replace( output, " R R2 ", " R' " );
      replace( output, " U U2 ", " U' " );
      replace( output, " D D2 ", " D' " );
      replace( output, " F F2 ", " F' " );
      replace( output, " B B2 ", " B' " );
      replace( output, " M M2 ", " M' " );
      replace( output, " E E2 ", " E' " );
      replace( output, " S S2 ", " S' " );
      replace( output, " X X2 ", " X' " );
      replace( output, " Y Y2 ", " Y' " );
      replace( output, " Z Z2 ", " Z' " );
      replace( output, " Ll Ll2 ", " Ll' " );
      replace( output, " Rr Rr2 ", " Rr' " );
      replace( output, " Uu Uu2 ", " Uu' " );
      replace( output, " Dd Dd2 ", " Dd' " );
      replace( output, " Ff Ff2 ", " Ff' " );
      replace( output, " Bb Bb2 ", " Bb' " );

      replace( output, " l2 l ", " l' " );
      replace( output, " r2 r ", " r' " );
      replace( output, " u2 u ", " u' " );
      replace( output, " d2 d ", " d' " );
      replace( output, " f2 f ", " f' " );
      replace( output, " b2 b ", " b' " );
      replace( output, " L2 L ", " L' " );
      replace( output, " R2 R ", " R' " );
      replace( output, " U2 U ", " U' " );
      replace( output, " D2 D ", " D' " );
      replace( output, " F2 F ", " F' " );
      replace( output, " B2 B ", " B' " );
      replace( output, " M2 M ", " M' " );
      replace( output, " E2 E ", " E' " );
      replace( output, " S2 S ", " S' " );
      replace( output, " X2 X ", " X' " );
      replace( output, " Y2 Y ", " Y' " );
      replace( output, " Z2 Z ", " Z' " );
      replace( output, " Ll2 Ll ", " Ll' " );
      replace( output, " Rr2 Rr ", " Rr' " );
      replace( output, " Uu2 Uu ", " Uu' " );
      replace( output, " Dd2 Dd ", " Dd' " );
      replace( output, " Ff2 Ff ", " Ff' " );
      replace( output, " Bb2 Bb ", " Bb' " );

      replace( output, " l' l2 ", " l " );
      replace( output, " r' r2 ", " r " );
      replace( output, " u' u2 ", " u " );
      replace( output, " d' d2 ", " d " );
      replace( output, " f' f2 ", " f " );
      replace( output, " b' b2 ", " b " );
      replace( output, " L' L2 ", " L " );
      replace( output, " R' R2 ", " R " );
      replace( output, " U' U2 ", " U " );
      replace( output, " D' D2 ", " D " );
      replace( output, " F' F2 ", " F " );
      replace( output, " B' B2 ", " B " );
      replace( output, " M' M2 ", " M " );
      replace( output, " E' E2 ", " E " );
      replace( output, " S' S2 ", " S " );
      replace( output, " X' X2 ", " X " );
      replace( output, " Y' Y2 ", " Y " );
      replace( output, " Z' Z2 ", " Z " );
      replace( output, " Ll' Ll2 ", " Ll " );
      replace( output, " Rr' Rr2 ", " Rr " );
      replace( output, " Uu' Uu2 ", " Uu " );
      replace( output, " Dd' Dd2 ", " Dd " );
      replace( output, " Ff' Ff2 ", " Ff " );
      replace( output, " Bb' Bb2 ", " Bb " );

      replace( output, " l2 l' ", " l " );
      replace( output, " r2 r' ", " r " );
      replace( output, " u2 u' ", " u " );
      replace( output, " d2 d' ", " d " );
      replace( output, " f2 f' ", " f " );
      replace( output, " b2 b' ", " b " );
      replace( output, " L2 L' ", " L " );
      replace( output, " R2 R' ", " R " );
      replace( output, " U2 U' ", " U " );
      replace( output, " D2 D' ", " D " );
      replace( output, " F2 F' ", " F " );
      replace( output, " B2 B' ", " B " );
      replace( output, " M2 M' ", " M " );
      replace( output, " E2 E' ", " E " );
      replace( output, " S2 S' ", " S " );
      replace( output, " X2 X' ", " X " );
      replace( output, " Y2 Y' ", " Y " );
      replace( output, " Z2 Z' ", " Z " );
      replace( output, " Ll2 Ll' ", " Ll " );
      replace( output, " Rr2 Rr' ", " Rr " );
      replace( output, " Uu2 Uu' ", " Uu " );
      replace( output, " Dd2 Dd' ", " Dd " );
      replace( output, " Ff2 Ff' ", " Ff " );
      replace( output, " Bb2 Bb' ", " Bb " );

      replace( output, " l' l' ", " l2 " );
      replace( output, " r' r' ", " r2 " );
      replace( output, " u' u' ", " u2 " );
      replace( output, " d' d' ", " d2 " );
      replace( output, " f' f' ", " f2 " );
      replace( output, " b' b' ", " b2 " );
      replace( output, " L' L' ", " L2 " );
      replace( output, " R' R' ", " R2 " );
      replace( output, " U' U' ", " U2 " );
      replace( output, " D' D' ", " D2 " );
      replace( output, " F' F' ", " F2 " );
      replace( output, " B' B' ", " B2 " );
      replace( output, " M' M' ", " M2 " );
      replace( output, " E' E' ", " E2 " );
      replace( output, " S' S' ", " S2 " );
      replace( output, " X' X' ", " X2 " );
      replace( output, " Y' Y' ", " Y2 " );
      replace( output, " Z' Z' ", " Z2 " );
      replace( output, " Ll' Ll' ", " Ll2 " );
      replace( output, " Rr' Rr' ", " Rr2 " );
      replace( output, " Uu' Uu' ", " Uu2 " );
      replace( output, " Dd' Dd' ", " Dd2 " );
      replace( output, " Ff' Ff' ", " Ff2 " );
      replace( output, " Bb' Bb' ", " Bb2 " );

      replace( output, " l2 l2 ", " " );
      replace( output, " r2 r2 ", " " );
      replace( output, " u2 u2 ", " " );
      replace( output, " d2 d2 ", " " );
      replace( output, " f2 f2 ", " " );
      replace( output, " b2 b2 ", " " );
      replace( output, " L2 L2 ", " " );
      replace( output, " R2 R2 ", " " );
      replace( output, " U2 U2 ", " " );
      replace( output, " D2 D2 ", " " );
      replace( output, " F2 F2 ", " " );
      replace( output, " B2 B2 ", " " );
      replace( output, " M2 M2 ", " " );
      replace( output, " E2 E2 ", " " );
      replace( output, " S2 S2 ", " " );
      replace( output, " X2 X2 ", " " );
      replace( output, " Y2 Y2 ", " " );
      replace( output, " Z2 Z2 ", " " );
      replace( output, " Ll2 Ll2 ", " " );
      replace( output, " Rr2 Rr2 ", " " );
      replace( output, " Uu2 Uu2 ", " " );
      replace( output, " Dd2 Dd2 ", " " );
      replace( output, " Ff2 Ff2 ", " " );
      replace( output, " Bb2 Bb2 ", " " );

      size_t new_length = output.length( );

      if( new_length > 2 && new_length < last_length )
         last_length = new_length;
      else
         break;
   }

   if( output.length( ) <= 2 )
      output.erase( );
   else
      output = output.substr( 1, output.length( ) - 2 );

   return output;
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

