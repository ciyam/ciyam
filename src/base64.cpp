// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2013 CIYAM Open Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef __BORLANDC__
#  include "precompile.h"
#endif
#pragma hdrstop

#include "base64.h"

using namespace std;

//#define DEBUG

namespace
{

const char c_fillchar = '=';
const string b64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#if ( 'a' != 97 ) || ( 'A' != 65 )
#  error This function has been only implemented for ASCII character platforms...
#endif
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

string base64::encode( const string& input )
{
   string str( ( ( input.length( ) + 2 ) / 3 ) * 4, '\0' );
#ifdef DEBUG
   cout << "input.length( ) = " << input.length( ) << endl;
   cout << "str.length( ) = " << str.length( ) << endl;
#endif

   char c;
   string::size_type i, o = 0;
   string::size_type len = input.length( );

   for( i = 0; i < len; ++i )
   {
      c = ( input[ i ] >> 2 ) & 0x3f;
      str[ o++ ] = b64_table[ c ];

      c = ( input[ i ] << 4 ) & 0x3f;

      if( ++i < len )
         c |= ( input[ i ] >> 4 ) & 0x0f;

      str[ o++ ] = b64_table[ c ];

      if( i < len )
      {
         c = ( input[ i ] << 2 ) & 0x3f;
         if( ++i < len )
            c |= ( input[ i ] >> 6 ) & 0x03;

         str[ o++ ] = b64_table[ c ];
      }
      else
      {
         ++i;
         str[ o++ ] = c_fillchar;
      }

      if( i < len )
      {
         c = input[ i ] & 0x3f;
         str[ o++ ] = b64_table[ c ];
      }
      else
         str[ o++ ] = c_fillchar;
   }

#ifdef DEBUG
   cout << "o = " << o << endl;
#endif
   return str;
}

string base64::decode( const string& input )
{
   string::size_type l = ( input.length( ) / 4 ) * 3;
   for( string::size_type i = input.length( ); i > 1; i-- )
   {
      if( input[ i - 1 ] == '=' )
         --l;
      else
         break;
   }

   string str( l, '\0' );
#ifdef DEBUG
   cout << "input.length( ) = " << input.length( ) << endl;
   cout << "str.length( ) = " << str.length( ) << endl;
#endif

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

      str[ o++ ] = c;

      if( ++i < len )
      {
         c = input[ i ];
         if( c == c_fillchar )
            break;

         c = get_b64_char_value( c );
         c1 = ( ( c1 << 4 ) & 0xf0 ) | ( ( c >> 2 ) & 0x0f );

         str[ o++ ] = c1;
      }

      if( ++i < len )
      {
         c1 = input[ i ];
         if( c1 == c_fillchar )
            break;

         c1 = get_b64_char_value( c1 );
         c = ( ( c << 6 ) & 0xc0 ) | c1;

         str[ o++ ] = c;
      }
   }

#ifdef DEBUG
   cout << "o = " << o << endl;
#endif
   return str;
}

