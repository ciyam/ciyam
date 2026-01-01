// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
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
const string g_b64_url_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

char get_b64_char_value( char c, bool url_encoding )
{
   if( url_encoding )
   {
      if( c == '-' )
         return 62;
      else if( c == '_' )
         return 63;
   }
   else
   {
      if( c == '+' )
         return 62;
      else if( c == '/' )
         return 63;
   }

   if( c >= 'a' )
      return c - 'a' + 26;

   if( c >= 'A' )
      return c - 'A';

   if( c >= '0' )
      return c - '0' + 52;

   throw runtime_error( "found invalid b64 char '" + to_string( c ) + "'" );
}

}

size_t base64::decode_size( const string& input, bool url_encoding )
{
   string::size_type l = 0;

   if( url_encoding )
   {
      l = ( input.length( ) / 4 ) * 3;

      size_t mod = ( input.length( ) % 4 );

      if( mod == 1 )
         throw runtime_error( "invalid URL encoded base64 '" + input + "'" );

      if( mod == 2 )
         ++l;
      else if( mod == 3 )
         l += 2;
   }
   else
   {
      l = ( input.length( ) / 4 ) * 3;

      for( string::size_type i = input.length( ); i > 1; i-- )
      {
         if( input[ i - 1 ] == c_fillchar )
            --l;
         else
            break;
      }
   }

   return l;
}

size_t base64::decode( const string& input, unsigned char* p_data, size_t length, bool url_encoding )
{
   string::size_type l = decode_size( input, url_encoding );

   if( l > length )
      throw runtime_error( "buffer not big enough to decode base64 (given "
       + to_string( length ) + " bytes but need " + to_string( l ) + " bytes)" );

   char c;
   char c1;
   string::size_type o = 0;
   string::size_type len = input.length( );

   for( string::size_type i = 0; i < len; ++i )
   {
      c = get_b64_char_value( input[ i ], url_encoding );
      ++i;

      c1 = get_b64_char_value( input[ i ], url_encoding );
      c = ( c << 2 ) | ( ( c1 >> 4 ) & 0x03 );

      p_data[ o++ ] = c;

      if( ++i < len )
      {
         c = input[ i ];

         if( c == c_fillchar )
            break;

         c = get_b64_char_value( c, url_encoding );
         c1 = ( ( c1 << 4 ) & 0xf0 ) | ( ( c >> 2 ) & 0x0f );

         p_data[ o++ ] = c1;
      }

      if( ++i < len )
      {
         c1 = input[ i ];

         if( c1 == c_fillchar )
            break;

         c1 = get_b64_char_value( c1, url_encoding );
         c = ( ( c << 6 ) & 0xc0 ) | c1;

         p_data[ o++ ] = c;
      }
   }

   return l;
}

void base64::encode( const unsigned char* p_dat, size_t length, char* p_enc, size_t* p_enc_len, bool url_encoding )
{
   char c;
   string::size_type i, o = 0;
   string::size_type len = length;

   string table( g_b64_table );

   if( url_encoding )
      table = g_b64_url_table;

   for( i = 0; i < len; ++i )
   {
      c = ( p_dat[ i ] >> 2 ) & 0x3f;
      p_enc[ o++ ] = table[ c ];

      c = ( p_dat[ i ] << 4 ) & 0x3f;

      if( ++i < len )
         c |= ( p_dat[ i ] >> 4 ) & 0x0f;

      p_enc[ o++ ] = table[ c ];

      if( i < len )
      {
         c = ( p_dat[ i ] << 2 ) & 0x3f;
         if( ++i < len )
            c |= ( p_dat[ i ] >> 6 ) & 0x03;

         p_enc[ o++ ] = table[ c ];
      }
      else
      {
         ++i;
         if( !url_encoding )
            p_enc[ o++ ] = c_fillchar;
      }

      if( i < len )
      {
         c = p_dat[ i ] & 0x3f;
         p_enc[ o++ ] = table[ c ];
      }
      else if( !url_encoding )
         p_enc[ o++ ] = c_fillchar;
   }

   if( p_enc_len )
      *p_enc_len = o;
}

bool base64::valid_characters( const string& input, bool url_encoding )
{
   bool rc = true;

   if( !input.length( ) )
      rc = false;
   else if( url_encoding )
   {
      string::size_type pos = input.find_first_not_of( g_b64_url_table );

      if( pos != string::npos )
         rc = false;
   }
   else
   {
      if( input.length( ) % 4 != 0 )
         rc = false;
      else
      {
         string::size_type pos = input.find_first_not_of( g_b64_table + c_fillchar );

         if( pos != string::npos
          && ( ( pos < input.size( ) - 2 ) || ( input[ pos ] != c_fillchar ) ) )
            rc = false;
      }
   }

   return rc;
}

void base64::validate( const string& input, bool* p_rc, bool url_encoding )
{
   bool invalid = false;

   if( !input.length( ) )
      invalid = true;
   else if( url_encoding )
   {
      for( size_t i = 0; i < input.length( ); i++ )
      {
         if( g_b64_url_table.find( input[ i ] ) == string::npos )
         {
            invalid = true;
            break;
         }
      }
   }
   else
   {
      if( input.length( ) % 4 != 0 )
         invalid = true;
      else
      {
         size_t i;

         for( i = 0; i < input.length( ); i++ )
         {
            if( g_b64_table.find( input[ i ] ) == string::npos )
               break;
         }

         if( i < input.length( ) - 2 )
            invalid = true;
         else if( i < input.length( ) )
         {
            if( input[ i ] != c_fillchar )
               invalid = true;
            else if( i == input.length( ) - 2 )
            {
               if( input[ i + 1 ] != c_fillchar )
                  invalid = true;
            }
         }
      }
   }

   if( p_rc )
      *p_rc = !invalid;
   else if( invalid )
      throw runtime_error( "invalid base64 value: " + input );
}
