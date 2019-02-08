// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2019 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <stdexcept>
#endif

#include "base64.h"

using namespace std;

namespace
{

const char c_fillchar = '=';

const string g_b64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char get_b64_char_value( char c )
{
   if( c >= 'a' )
      return c - 'a' + 26;

   if( c >= 'A' )
      return c - 'A';

   if( c >= '0' )
      return c - '0' + 52;

   return c == '+' ? 62 : 63;
}

}

void base64::encode( const unsigned char* p_dat, size_t length, char* p_enc, size_t* p_enc_len )
{
   char c;
   string::size_type i, o = 0;
   string::size_type len = length;

   for( i = 0; i < len; ++i )
   {
      c = ( p_dat[ i ] >> 2 ) & 0x3f;
      p_enc[ o++ ] = g_b64_table[ c ];

      c = ( p_dat[ i ] << 4 ) & 0x3f;

      if( ++i < len )
         c |= ( p_dat[ i ] >> 4 ) & 0x0f;

      p_enc[ o++ ] = g_b64_table[ c ];

      if( i < len )
      {
         c = ( p_dat[ i ] << 2 ) & 0x3f;
         if( ++i < len )
            c |= ( p_dat[ i ] >> 6 ) & 0x03;

         p_enc[ o++ ] = g_b64_table[ c ];
      }
      else
      {
         ++i;
         p_enc[ o++ ] = c_fillchar;
      }

      if( i < len )
      {
         c = p_dat[ i ] & 0x3f;
         p_enc[ o++ ] = g_b64_table[ c ];
      }
      else
         p_enc[ o++ ] = c_fillchar;
   }

   if( p_enc_len )
      *p_enc_len = o;
}

void base64::validate( const string& input, bool* p_rc )
{
   bool invalid = false;

   if( !input.length( ) || input.length( ) % 4 != 0 )
      invalid = true;
   else
   {
      size_t i;
      string table( g_b64_table );

      for( i = 0; i < input.length( ); i++ )
      {
         if( table.find( input[ i ] ) == string::npos )
            break;
      }

      if( i < input.length( ) - 2 )
         invalid = true;
      else if( i < input.length( ) )
      {
         if( input[ i ] != '=' )
            invalid = true;
         else if( i == input.length( ) - 2 )
         {
            if( input[ i + 1 ] != '=' )
               invalid = true;
         }
      }
   }

   if( p_rc )
      *p_rc = !invalid;
   else if( invalid )
      throw runtime_error( "invalid base64 value: " + input );
}

size_t base64::decode_size( const string& input )
{
   string::size_type l = ( input.length( ) / 4 ) * 3;

   for( string::size_type i = input.length( ); i > 1; i-- )
   {
      if( input[ i - 1 ] == '=' )
         --l;
      else
         break;
   }

   return l;
}

size_t base64::decode_size( size_t length, bool minimum_possible )
{
   return ( ( length / 4 ) * 3 ) - ( minimum_possible ? 2 : 0 );
}

size_t base64::decode( const string& input, unsigned char* p_data, size_t length )
{
   string::size_type l = decode_size( input );

   if( l > length )
      throw runtime_error( "buffer not big enough to decode base64 (given "
       + to_string( length ) + " bytes but need " + to_string( l ) + " bytes)" );

   char c;
   char c1;
   string::size_type o = 0;
   string::size_type len = input.length( );

   for( string::size_type i = 0; i < len; ++i )
   {
      c = get_b64_char_value( input[ i ] );
      ++i;

      c1 = get_b64_char_value( input[ i ] );
      c = ( c << 2 ) | ( ( c1 >> 4 ) & 0x03 );

      p_data[ o++ ] = c;

      if( ++i < len )
      {
         c = input[ i ];
         if( c == c_fillchar )
            break;

         c = get_b64_char_value( c );
         c1 = ( ( c1 << 4 ) & 0xf0 ) | ( ( c >> 2 ) & 0x0f );

         p_data[ o++ ] = c1;
      }

      if( ++i < len )
      {
         c1 = input[ i ];
         if( c1 == c_fillchar )
            break;

         c1 = get_b64_char_value( c1 );
         c = ( ( c << 6 ) & 0xc0 ) | c1;

         p_data[ o++ ] = c;
      }
   }

   return l;
}
