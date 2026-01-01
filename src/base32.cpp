// Copyright (c) 2013-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#include <iostream>

#include "base32.h"

using namespace std;

namespace
{

const string g_b32_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

void base32_encode( char* p_output, const unsigned char* p_data, unsigned length )
{
   if( length > 0 )
   {
      int next = 1;
      int count = 0;
      int bits_left = 8;

      int buffer = p_data[ 0 ];

      while( bits_left > 0 || next < length )
      {
         if( bits_left < 5 )
         {
            if( next < length )
            {
               buffer <<= 8;
               buffer |= p_data[ next++ ] & 0xff;

               bits_left += 8;
            }
            else
            {
               int pad = 5 - bits_left;

               buffer <<= pad;
               bits_left += pad;
            }
         }

         int index = 0x1f & ( buffer >> ( bits_left - 5 ) );

         bits_left -= 5;
         p_output[ count++ ] = g_b32_table[ index ];
      }
   }
}

int base32_decode( unsigned char* p_output, const char* p_in, unsigned length )
{
   int count = 0;
   int buffer = 0;
   int bits_left = 0;

   const char* p = p_in;

   for( unsigned i = 0; i < length; ++i, ++p )
   {
      char ch = *p;

      buffer <<= 5;

      if( ( ch >= 'A' && ch <= 'Z' ) || ( ch >= 'a' && ch <= 'z' ) )
         ch = ( ch & 0x1f ) - 1;
      else if( ch >= '2' && ch <= '7' )
         ch -= '2' - 26;
      else
         return -1;

      buffer |= ch;
      bits_left += 5;

      if( bits_left >= 8 )
      {
         p_output[ count++ ] = buffer >> ( bits_left - 8 );
         bits_left -= 8;
      }
   }

   return count;
}

}

string base32::encode( const string& input )
{
   string str( ( ( input.length( ) * 8 ) + 4 ) / 5, '\0' );

   base32_encode( ( char* )&str[ 0 ], ( const unsigned char* )input.data( ), input.length( ) );

   return str;
}

string base32::decode( const string& input )
{
   string str( ( input.length( ) * 5 ) / 8, '\0' );

   base32_decode( ( unsigned char* )&str[ 0 ], ( const char* )input.data( ), input.length( ) );

   return str;
}

