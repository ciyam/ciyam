// Copyright (c) 2007-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstdio>
#  include <cstring>
#  include <string>
#  include <sstream>
#  include <iomanip>
#  include <iostream>
#  include <algorithm>
#endif

#include "sha1.h"

#include "utilities.h"

using namespace std;

//#define COMPILE_TESTBED_MAIN

namespace
{

const unsigned int c_buffer_size = 4096;

/*
SHA-1 in C
By Steve Reid <steve@edmweb.com>
100% Public Domain

Test Vectors (from FIPS PUB 180-1)
"abc"
  A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
  84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
A million repetitions of "a"
  34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/

//#define LITTLE_ENDIAN // This should be #define'd if true
//#define SHA1HANDSOFF // Copies data before messing with it.

typedef struct
{
   unsigned int state[ 5 ];
   unsigned int count[ 2 ];
   unsigned char buffer[ 64 ];
} sha1_context;

void sha1_transform( unsigned int state[ 5 ], unsigned char buffer[ 64 ] );
void sha1_init( sha1_context* context );
void sha1_update( sha1_context* context, unsigned char* data, unsigned int len );
void sha1_final( unsigned char digest[ 20 ], sha1_context* context );

#define rol( value, bits ) ( ( ( value ) << ( bits ) ) | ( ( value ) >> ( 32 - ( bits ) ) ) )

// blk0( ) and blk( ) perform the initial expand.
// I got the idea of expanding during the round function from SSLeay
#ifdef LITTLE_ENDIAN
#  define blk0( i ) ( block->l[ i ] = \
 ( rol( block->l[ i ], 24 ) & 0xFF00FF00 ) | ( rol ( block->l[ i ], 8 ) & 0x00FF00FF ) )
#else
#  define blk0( i ) block->l[ i ]
#endif

#define blk( i ) ( block->l[ i & 15 ] = \
 rol( block->l[ ( i + 13 ) & 15 ] ^ block->l[ ( i + 8 ) & 15 ] \
 ^ block->l[ ( i + 2 ) & 15 ] ^ block->l[ i & 15 ], 1 ) )

// ( R0 + R1 ), R2, R3, R4 are the different operations used in SHA1
#define R0( v, w, x, y, z, i ) z += ( ( w & ( x ^ y ) ) ^ y ) \
 + blk0( i ) + 0x5A827999 + rol( v, 5 ); w = rol( w, 30 );
#define R1( v, w, x, y, z, i ) z += ( ( w & ( x ^ y ) ) ^ y ) \
 + blk( i ) + 0x5A827999 + rol( v, 5 ); w = rol( w, 30 );
#define R2( v, w, x, y, z, i ) z += ( w ^ x ^ y ) \
 + blk( i ) + 0x6ED9EBA1 + rol( v, 5 ); w = rol( w, 30 );
#define R3( v, w, x, y, z, i ) z += ( ( ( w | x ) & y ) | ( w & x ) ) \
 + blk( i ) + 0x8F1BBCDC + rol( v, 5 ); w = rol( w, 30 );
#define R4( v, w, x, y, z, i ) z += ( w ^ x ^ y ) \
 + blk( i ) + 0xCA62C1D6 + rol( v, 5 ); w = rol( w, 30 );

// Hash a single 512-bit block. This is the core of the algorithm.

void sha1_transform( unsigned int state[ 5 ], unsigned char buffer[ 64 ] )
{
   unsigned int a, b, c, d, e;
   typedef union
   {
      unsigned char c[ 64 ];
      unsigned int l[ 16 ];
   } CHAR64LONG16;

   CHAR64LONG16* block;
#ifdef SHA1HANDSOFF
   static unsigned char workspace[ 64 ];
   block = ( CHAR64LONG16* )workspace;
   memcpy( block, buffer, 64 );
#else
   block = ( CHAR64LONG16* )buffer;
#endif
   // Copy context->state[ ] to working vars
   a = state[ 0 ];
   b = state[ 1 ];
   c = state[ 2 ];
   d = state[ 3 ];
   e = state[ 4 ];

   // 4 rounds of 20 operations each. Loop unrolled.
   R0( a, b, c, d, e, 0 ); R0( e, a, b, c, d, 1 ); R0( d, e, a, b, c, 2 ); R0( c, d, e, a, b, 3 );
   R0( b, c, d, e, a, 4 ); R0( a, b, c, d, e, 5 ); R0( e, a, b, c, d, 6 ); R0( d, e, a, b, c, 7 );
   R0( c, d, e, a, b, 8 ); R0( b, c, d, e, a, 9 ); R0( a, b, c, d, e, 10 ); R0( e, a, b, c, d, 11 );
   R0( d, e, a, b, c, 12 ); R0( c, d, e, a, b, 13 ); R0( b, c, d, e, a, 14 ); R0( a, b, c, d, e, 15 );
   R1( e, a, b, c, d, 16 ); R1( d, e, a, b, c, 17 ); R1( c, d, e, a, b, 18 ); R1( b, c, d, e, a, 19 );
   R2( a, b, c, d, e, 20 ); R2( e, a, b, c, d, 21 ); R2( d, e, a, b, c, 22 ); R2( c, d, e, a, b, 23 );
   R2( b, c, d, e, a, 24 ); R2( a, b, c, d, e, 25 ); R2( e, a, b, c, d, 26 ); R2( d, e, a, b, c, 27 );
   R2( c, d, e, a, b, 28 ); R2( b, c, d, e, a, 29 ); R2( a, b, c, d, e, 30 ); R2( e, a, b, c, d, 31 );
   R2( d, e, a, b, c, 32 ); R2( c, d, e, a, b, 33 ); R2( b, c, d, e, a, 34 ); R2( a, b, c, d, e, 35 );
   R2( e, a, b, c, d, 36 ); R2( d, e, a, b, c, 37 ); R2( c, d, e, a, b, 38 ); R2( b, c, d, e, a, 39 );
   R3( a, b, c, d, e, 40 ); R3( e, a, b, c, d, 41 ); R3( d, e, a, b, c, 42 ); R3( c, d, e, a, b, 43 );
   R3( b, c, d, e, a, 44 ); R3( a, b, c, d, e, 45 ); R3( e, a, b, c, d, 46 ); R3( d, e, a, b, c, 47 );
   R3( c, d, e, a, b, 48 ); R3( b, c, d, e, a, 49 ); R3( a, b, c, d, e, 50 ); R3( e, a, b, c, d, 51 );
   R3( d, e, a, b, c, 52 ); R3( c, d, e, a, b, 53 ); R3( b, c, d, e, a, 54 ); R3( a, b, c, d, e, 55 );
   R3( e, a, b, c, d, 56 ); R3( d, e, a, b, c, 57 ); R3( c, d, e, a, b, 58 ); R3( b, c, d, e, a, 59 );
   R4( a, b, c, d, e, 60 ); R4( e, a, b, c, d, 61 ); R4( d, e, a, b, c, 62 ); R4( c, d, e, a, b, 63 );
   R4( b, c, d, e, a, 64 ); R4( a, b, c, d, e, 65 ); R4( e, a, b, c, d, 66 ); R4( d, e, a, b, c, 67 );
   R4( c, d, e, a, b, 68 ); R4( b, c, d, e, a, 69 ); R4( a, b, c, d, e, 70 ); R4( e, a, b, c, d, 71 );
   R4( d, e, a, b, c, 72 ); R4( c, d, e, a, b, 73 ); R4( b, c, d, e, a, 74 ); R4( a, b, c, d, e, 75 );
   R4( e, a, b, c, d, 76 ); R4( d, e, a, b, c, 77 ); R4( c, d, e, a, b, 78 ); R4( b, c, d, e, a, 79 );

   // Add the working vars back into context.state[ ]
   state[ 0 ] += a;
   state[ 1 ] += b;
   state[ 2 ] += c;
   state[ 3 ] += d;
   state[ 4 ] += e;

   // Wipe variables
   a = b = c = d = e = 0;

   ( void )a; // avoid BCB "is assigned a value that is never used" warning
}

void sha1_init( sha1_context* context )
{
   // SHA1 initialization constants
   context->state[ 0 ] = 0x67452301;
   context->state[ 1 ] = 0xEFCDAB89;
   context->state[ 2 ] = 0x98BADCFE;
   context->state[ 3 ] = 0x10325476;
   context->state[ 4 ] = 0xC3D2E1F0;
   context->count[ 0 ] = context->count[ 1 ] = 0;
}

// Run your data through this.

void sha1_update( sha1_context* context, unsigned char* data, unsigned int len )
{
   unsigned int i, j;

   j = ( context->count[ 0 ] >> 3 ) & 63;
   if( ( context->count[ 0 ] += len << 3 ) < ( len << 3 ) )
      context->count[ 1 ]++;

   context->count[ 1 ] += ( len >> 29 );
   if( ( j + len ) > 63 )
   {
      memcpy( &context->buffer[ j ], data, ( i = 64 - j ) );
      sha1_transform( context->state, context->buffer );
      for( ; i + 63 < len; i += 64 )
         sha1_transform( context->state, &data[ i ] );
      j = 0;
   }
   else
      i = 0;

   if( len > i )
      memcpy( &context->buffer[ j ], &data[ i ], len - i );
}

// Add padding and return the message digest.

void sha1_final( unsigned char digest[ 20 ], sha1_context* context )
{
   unsigned int i, j;
   unsigned char finalcount[ 8 ];

   for( i = 0; i < 8; i++ )
      finalcount[ i ] = // Endian independent
       ( unsigned char )( ( context->count[ ( i >= 4 ? 0 : 1 ) ] >> ( ( 3 - ( i & 3 ) ) * 8 ) ) & 255 );

   sha1_update( context, ( unsigned char* )"\200", 1 );
   while( ( context->count[ 0 ] & 504 ) != 448 )
      sha1_update( context, ( unsigned char* )"\0", 1 );

   sha1_update( context, finalcount, 8 ); // should cause a sha1_transform( )
   for( i = 0; i < 20; i++ )
      digest[ i ] = ( unsigned char )( ( context->state[ i >> 2 ] >> ( ( 3 - ( i & 3 ) ) * 8 ) ) & 255 );

    // Wipe variables
    i = j = 0;
    memset( context->buffer, 0, 64 );
    memset( context->state, 0, 20 );
    memset( context->count, 0, 8 );
    memset( &finalcount, 0, 8 );
#ifdef SHA1HANDSOFF  // make sha1_transform overwrite it's own static vars
    sha1_transform( context->state, context->buffer );
#endif

   ( void )i; // avoid BCB "is assigned a value that is never used" warning
   ( void )j; // avoid BCB "is assigned a value that is never used" warning
}

} // namespace

struct sha1::impl
{
   bool final;
   unsigned char digest[ c_sha1_digest_size ];

   sha1_context context;

   ~impl( )
   {
      for( size_t i = 0; i < c_sha1_digest_size; i++ )
         digest[ i ] = 0;
   }
};

sha1::sha1( )
{
   p_impl = new impl;

   init( );
}

sha1::sha1( const std::string& str )
{
   p_impl = new impl;

   init( );
   update( ( const unsigned char* )&str[ 0 ], str.length( ) );
}

sha1::sha1( const unsigned char* p_data, unsigned int length )
{
   p_impl = new impl;

   init( );
   update( p_data, length );
}

sha1::~sha1( )
{
   delete p_impl;
}

void sha1::init( )
{
   p_impl->final = false;
   sha1_init( &p_impl->context );
}

void sha1::update( const std::string& str )
{
   update( ( const unsigned char* )&str[ 0 ], str.length( ) );
}

void sha1::update( const unsigned char* p_data, unsigned int length )
{
   unsigned char buf[ c_buffer_size ];

   unsigned int pos = 0;
   unsigned int chunk = min( length, c_buffer_size );

   if( p_impl->final )
      init( );

   while( chunk > 0 )
   {
      memcpy( buf, p_data + pos, chunk );
      sha1_update( &p_impl->context, buf, chunk );

      pos += chunk;
      length -= chunk;
      chunk = min( length, c_buffer_size );
   }
}

void sha1::copy_digest_to_buffer( unsigned char* p_buffer, unsigned int length )
{
   if( !p_impl->final )
   {
      p_impl->final = true;
      sha1_final( p_impl->digest, &p_impl->context );
   }

   // NOTE: Allows for copying just a number of the leading digest characters if desired.
   memcpy( p_buffer, p_impl->digest, min( length, ( unsigned int )c_sha1_digest_size ) );
}

void sha1::get_digest_as_string( string& s )
{
   if( !p_impl->final )
   {
      p_impl->final = true;
      sha1_final( p_impl->digest, &p_impl->context );
   }

   if( s.length( ) > 40 )
      s.resize( 40 );
   else if( s.length( ) != 40 )
      s = string( 40, '\0' );

   for( size_t i = 0, j = 0; i < c_sha1_digest_size; i++ )
   {
      s[ j++ ] = ascii_digit( ( p_impl->digest[ i ] & 0xf0 ) >> 4 );
      s[ j++ ] = ascii_digit( p_impl->digest[ i ] & 0x0f );
   }
}

string sha1::get_digest_as_string( char separator )
{
   if( !p_impl->final )
   {
      p_impl->final = true;
      sha1_final( p_impl->digest, &p_impl->context );
   }

   ostringstream outs;
   for( size_t i = 0; i < c_sha1_digest_size; i++ )
   {
      if( i && i % 4 == 0 && separator != '\0' )
         outs << separator;
      outs << hex << setw( 2 ) << setfill( '0' ) << ( unsigned )p_impl->digest[ i ];
   }

   return outs.str( );
}

string hmac_sha1( const string& key, const string& message )
{
   string s( 40, '\0' );
   unsigned char buffer[ c_sha1_digest_size ];

   hmac_sha1( key, message, buffer );

   for( size_t i = 0, j = 0; i < c_sha1_digest_size; i++ )
   {
      s[ j++ ] = ascii_digit( ( buffer[ i ] & 0xf0 ) >> 4 );
      s[ j++ ] = ascii_digit( buffer[ i ] & 0x0f );
   }

   return s;
}

void hmac_sha1( const string& key, const string& message, unsigned char* p_buffer )
{
   int key_len = key.length( );
   int msg_len = message.length( );

   const unsigned char* p_key = ( const unsigned char* )key.data( );
   const unsigned char* p_msg = ( const unsigned char* )message.data( );

   unsigned char k_ipad[ 65 ];
   unsigned char k_opad[ 65 ];

   unsigned char tk[ c_sha1_digest_size ];
   unsigned char tk2[ c_sha1_digest_size ];
   unsigned char bufferIn[ 1024 ];
   unsigned char bufferOut[ 1024 ];

   if( key_len > 64 )
   {
      sha1 hash( key );
      hash.copy_digest_to_buffer( tk );

      p_key = tk;
      key_len = 64;
   }
 
   memset( k_ipad, 0, 65 );
   memset( k_opad, 0, 65 );

   memcpy( k_ipad, p_key, key_len );
   memcpy( k_opad, p_key, key_len );
 
   for( int i = 0; i < 64; i++ )
   {
      k_ipad[ i ] ^= 0x36;
      k_opad[ i ] ^= 0x5c;
   }
 
   memset( bufferIn, 0x00, 1024 );
   memcpy( bufferIn, k_ipad, 64 );
   memcpy( bufferIn + 64, p_msg, msg_len );
 
   sha1 hash1( bufferIn, 64 + msg_len );
   hash1.copy_digest_to_buffer( tk2 );

   memset( bufferOut, 0x00, 1024 );
   memcpy( bufferOut, k_opad, 64 );
   memcpy( bufferOut + 64, tk2, c_sha1_digest_size );
 
   sha1 hash2( bufferOut, 64 + c_sha1_digest_size );
   hash2.copy_digest_to_buffer( p_buffer );
}

#ifdef COMPILE_TESTBED_MAIN
int main( int argc, char* argv[ ] )
{
   if( argc > 1 && argv[ 1 ] == string( "/?" ) )
   {
      cout << "Usage: sha1 [<file>]" << endl;
      return 0;
   }

   sha1 hash;

   if( argc > 1 )
   {
      FILE* fp = fopen( argv[ 1 ], "rb" );

      if( !fp )
      {
         cerr << "Error: Unable to open file '" << argv[ 1 ] << "' for input." << endl;
         return 1;
      }

      unsigned int len;
      unsigned char buf[ 8192 ];
      while( !feof( fp ) )
      {
         len = fread( buf, 1, 8192, fp );
         hash.update( buf, len );
      }
      fclose( fp );

      string s = hash.get_digest_as_string( ' ' );
      transform( s.begin( ), s.end( ), s.begin( ), ( int( * )( int ) )toupper );
      cout << "SHA-1 digest: " << s << endl;
   }
   else
   {
      string next;
      while( getline( cin, next ) )
      {
         if( cin.eof( ) )
            break;

         hash.init( );
         hash.update( ( const unsigned char* )next.c_str( ), next.length( ) );

         string s = hash.get_digest_as_string( ' ' );
         transform( s.begin( ), s.end( ), s.begin( ), ( int( * )( int ) )toupper );
         cout << "SHA-1 digest: " << s << '\n' << '\n';
      }
   }
}
#endif

