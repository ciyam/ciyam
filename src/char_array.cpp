// Copyright (c) 2017-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cmath>
#  include <vector>
#  include <iostream>
#  include <stdexcept>
#endif

#include "char_array.h"

#include "utilities.h"

using namespace std;

namespace
{

const size_t c_max_char_array_width = 1000;
const size_t c_max_char_array_height = 1000;

}

char_array::char_array( const string& type_and_or_state )
{
   ch = '*';
   type = type_and_or_state;

   string::size_type pos = type.find( ':' );

   if( pos != string::npos )
   {
      data = type.substr( pos + 1 );
      type.erase( pos );

      ch = type[ 0 ];
      type.erase( 0, 1 );
   }

   pos = type.find( 'x' );
   if( pos == string::npos )
      throw runtime_error( "unexpected char_array type info '" + type_and_or_state + "'" );

   width = from_string< size_t >( type.substr( 0, pos ) );
   height = from_string< size_t >( type.substr( pos + 1 ) );

   if( width < 2 || width > c_max_char_array_width )
      throw runtime_error( "char_array width " + to_string( width ) + " not permitted" );

   if( height < 2 || height > c_max_char_array_height )
      throw runtime_error( "char_array height " + to_string( height ) + " not permitted" );

   if( data.empty( ) )
      data = string( width * height, '.' );
   else
   {
      if( data.size( ) > width * height )
         data.erase( width * height );
      else if( data.size( ) < width * height )
         data += string( width * height - data.size( ), '.' );
   }
}

void char_array::output_data( ostream& os )
{
   size_t offset = 0;

   for( size_t i = 0; i < height; i++ )
   {
      if( i > 0 )
         os << '\n';

      for( size_t j = 0; j < width; j++ )
         os << data[ offset++ ];
   }
}

void char_array::place_chars( const string& chars )
{
   for( size_t i = 0; i < chars.length( ); i++ )
      data[ rand( ) % data.size( ) ] = chars[ i ];

   if( !chars.empty( ) )
      ch = chars[ 0 ];
}

size_t char_array::get_ch_pos( )
{
   size_t pos = string::npos;

   for( size_t i = 0; i < data.size( ); i++ )
   {
      if( data[ i ] == ch )
      {
         pos = i;
         break;
      }
   }

   return pos;
}

void char_array::move_up( )
{
   size_t pos = get_ch_pos( );

   if( pos != string::npos )
   {
      if( pos >= width )
      {
         data[ pos ] = '.';
         data[ pos - width ] = ch;
      }
   }
}

void char_array::move_down( )
{
   size_t pos = get_ch_pos( );

   if( pos != string::npos )
   {
      if( pos < ( height - 1 ) * width )
      {
         data[ pos ] = '.';
         data[ pos + width ] = ch;
      }
   }
}

void char_array::move_left( )
{
   size_t pos = get_ch_pos( );

   if( pos != string::npos )
   {
      if( pos % width != 0 )
      {
         data[ pos ] = '.';
         data[ pos - 1 ] = ch;
      }
   }
}

void char_array::move_right( )
{
   size_t pos = get_ch_pos( );

   if( pos != string::npos )
   {
      if( pos % width != width - 1 )
      {
         data[ pos ] = '.';
         data[ pos + 1 ] = ch;
      }
   }
}

void char_array::rotate_forwards( )
{
   char last_ch = data[ data.size( ) - 1 ];

   for( size_t i = data.size( ) - 1; i > 0; i-- )
      data[ i ] = data[ i - 1 ];

   data[ 0 ] = last_ch;
}

void char_array::rotate_backwards( )
{
   char first_ch = data[ 0 ];

   for( size_t i = 0; i < data.size( ) - 1; i++ )
      data[ i ] = data[ i + 1 ];

   data[ data.size( ) - 1 ] = first_ch;
}

void char_array::perform_moves( const string& moves )
{
   string all_moves( moves );

   if( !all_moves.empty( ) )
   {
      vector< string > moves;
      replace( all_moves, " ", "," );

      split( all_moves, moves );

      for( size_t i = 0; i < moves.size( ); i++ )
      {
         string next( moves[ i ] );

         if( next == "U" )
            move_up( );
         else if( next == "D" )
            move_down( );
         else if( next == "L" )
            move_left( );
         else if( next == "R" )
            move_right( );
         else if( next == "F" )
            rotate_forwards( );
         else if( next == "B" )
            rotate_backwards( );
      }
   }
}

