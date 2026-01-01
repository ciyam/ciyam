// Copyright (c) 2012 CIYAM Pty. Ltd. ACN 093 704 539
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
#  include <memory.h>
#  include <string>
#  include <sstream>
#  include <iomanip>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#include "sha256.h"

#include "utilities.h"

using namespace std;

//#define COMPILE_TESTBED_MAIN

namespace
{

const int c_file_buflen = 8192;

/*
Test Vectors
""
  E3B0C442 98FC1C14 9AFBF4C8 996FB924 27AE41E4 649B934C A495991B 7852B855
"abc"
  BA7816BF 8F01CFEA 414140DE 5DAE2223 B00361A3 96177A9C B410FF61 F20015AD
"secure hash algorithm"
  F30CEB2B B2829E79 E4CA9753 D35A8ECC 00262D16 4CC07708 0295381C BD643F0D
"This is exactly 64 bytes long, not counting the terminating byte"
  AB64EFF7 E88E2E46 165E29F2 BCE41826 BD4C7B35 52F6B382 A9E7D3AF 47C245F8
*/

const unsigned int c_buffer_size = 4096;

typedef unsigned int uint;
typedef unsigned char uchar;

#define DBL_INT_ADD( a, b, c ) if( a > 0xffffffff - ( c ) ) ++b; a += c;
#define ROTLEFT( a, b ) ( ( ( a ) << ( b ) ) | ( ( a ) >> ( 32 - ( b ) ) ) )
#define ROTRIGHT( a, b ) ( ( ( a ) >> ( b ) ) | ( ( a ) << ( 32 - ( b ) ) ) )

#define CH( x, y, z ) ( ( ( x ) & ( y ) ) ^ ( ~( x ) & ( z ) ) )
#define MAJ( x, y, z ) ( ( ( x ) & ( y ) ) ^ ( ( x ) & ( z ) ) ^ ( ( y ) & ( z ) ) )
#define EP0( x ) ( ROTRIGHT( x, 2 ) ^ ROTRIGHT( x, 13 ) ^ ROTRIGHT( x, 22 ) )
#define EP1( x ) ( ROTRIGHT( x, 6 ) ^ ROTRIGHT( x, 11 ) ^ ROTRIGHT( x, 25 ) )
#define SIG0( x ) ( ROTRIGHT( x, 7 ) ^ ROTRIGHT( x, 18 ) ^ ( ( x ) >> 3 ) )
#define SIG1( x ) ( ROTRIGHT( x, 17 ) ^ ROTRIGHT( x, 19 ) ^ ( ( x ) >> 10 ) )

typedef struct
{
   uchar data[ 64 ];
   uint datalen;
   uint bitlen[ 2 ];
   uint state[ 8 ];
} SHA256_CTX;

uint k[ 64 ] =
{
   0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
   0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
   0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
   0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
   0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
   0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
   0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
   0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

void sha256_transform( SHA256_CTX* ctx, uchar data[ ] )
{  
   uint a, b, c, d, e, f, g, h, i, j, t1, t2, m[ 64 ];
      
   for( i = 0, j = 0; i < 16; ++i, j += 4 )
      m[ i ] = ( data[ j ] << 24 ) | ( data[ j + 1 ] << 16 ) | ( data[ j + 2 ] << 8 ) | ( data[ j + 3 ] );

   for( ; i < 64; ++i )
      m[ i ] = SIG1( m[ i - 2 ] ) + m[ i - 7 ] + SIG0( m[ i - 15 ] ) + m[ i - 16 ];

   a = ctx->state[ 0 ];
   b = ctx->state[ 1 ];
   c = ctx->state[ 2 ];
   d = ctx->state[ 3 ];
   e = ctx->state[ 4 ];
   f = ctx->state[ 5 ];
   g = ctx->state[ 6 ];
   h = ctx->state[ 7 ];
   
   for( i = 0; i < 64; ++i )
   {
      t1 = h + EP1( e ) + CH( e, f, g ) + k[ i ] + m[ i ];
      t2 = EP0( a ) + MAJ( a, b, c );
      h = g;
      g = f;
      f = e;
      e = d + t1;
      d = c;
      c = b;
      b = a;
      a = t1 + t2;
   }
   
   ctx->state[ 0 ] += a;
   ctx->state[ 1 ] += b;
   ctx->state[ 2 ] += c;
   ctx->state[ 3 ] += d;
   ctx->state[ 4 ] += e;
   ctx->state[ 5 ] += f;
   ctx->state[ 6 ] += g;
   ctx->state[ 7 ] += h;
}

void sha256_init( SHA256_CTX* ctx )
{  
   ctx->datalen = 0; 
   ctx->bitlen[ 0 ] = 0;
   ctx->bitlen[ 1 ] = 0;
   ctx->state[ 0 ] = 0x6a09e667;
   ctx->state[ 1 ] = 0xbb67ae85;
   ctx->state[ 2 ] = 0x3c6ef372;
   ctx->state[ 3 ] = 0xa54ff53a;
   ctx->state[ 4 ] = 0x510e527f;
   ctx->state[ 5 ] = 0x9b05688c;
   ctx->state[ 6 ] = 0x1f83d9ab;
   ctx->state[ 7 ] = 0x5be0cd19;
}

void sha256_update( SHA256_CTX* ctx, uchar data[ ], uint len )
{  
   uint i;
   
   for( i = 0; i < len; ++i )
   {
      ctx->data[ ctx->datalen ] = data[ i ];
      ctx->datalen++; 

      if( ctx->datalen == 64 )
      {
         sha256_transform( ctx, ctx->data );
         DBL_INT_ADD( ctx->bitlen[ 0 ], ctx->bitlen[ 1 ], 512 );
         ctx->datalen = 0; 
      }
   }
}

void sha256_final( SHA256_CTX* ctx, uchar hash[ ] )
{  
   uint i; 
   
   i = ctx->datalen; 
   
   // Pad whatever data is left in the buffer. 
   if( ctx->datalen < 56 )
   {
      ctx->data[ i++ ] = 0x80;
      while( i < 56 )
         ctx->data[ i++ ] = 0x00;
   }
   else
   {
      ctx->data[ i++ ] = 0x80;
      while( i < 64 )
         ctx->data[ i++ ] = 0x00;

      sha256_transform( ctx, ctx->data );
      memset( ctx->data, 0, 56 );
   }

   DBL_INT_ADD( ctx->bitlen[ 0 ], ctx->bitlen[ 1 ], ctx->datalen * 8 );
   ctx->data[ 63 ] = ( uchar )( ctx->bitlen[ 0 ] );
   ctx->data[ 62 ] = ( uchar )( ctx->bitlen[ 0 ] >> 8 );
   ctx->data[ 61 ] = ( uchar )( ctx->bitlen[ 0 ] >> 16 );
   ctx->data[ 60 ] = ( uchar )( ctx->bitlen[ 0 ] >> 24 );
   ctx->data[ 59 ] = ( uchar )( ctx->bitlen[ 1 ] );
   ctx->data[ 58 ] = ( uchar )( ctx->bitlen[ 1 ] >> 8 );
   ctx->data[ 57 ] = ( uchar )( ctx->bitlen[ 1 ] >> 16 );
   ctx->data[ 56 ] = ( uchar )( ctx->bitlen[ 1 ] >> 24 );
   sha256_transform( ctx, ctx->data );
   
   for( i = 0; i < 4; ++i )
   {
      hash[ i ] = ( uchar )( ( ctx->state[ 0 ] >> ( 24 - i * 8 ) ) & 0x000000ff );
      hash[ i + 4 ] = ( uchar )( ( ctx->state[ 1 ] >> ( 24 - i * 8 ) ) & 0x000000ff );
      hash[ i + 8 ] = ( uchar )( ( ctx->state[ 2 ] >> ( 24 - i * 8 ) ) & 0x000000ff );
      hash[ i + 12 ] = ( uchar )( ( ctx->state[ 3 ] >> ( 24 - i * 8 ) ) & 0x000000ff );
      hash[ i + 16 ] = ( uchar )( ( ctx->state[ 4 ] >> ( 24 - i * 8 ) ) & 0x000000ff );
      hash[ i + 20 ] = ( uchar )( ( ctx->state[ 5 ] >> ( 24 - i * 8 ) ) & 0x000000ff );
      hash[ i + 24 ] = ( uchar )( ( ctx->state[ 6 ] >> ( 24 - i * 8 ) ) & 0x000000ff );
      hash[ i + 28 ] = ( uchar )( ( ctx->state[ 7 ] >> ( 24 - i * 8 ) ) & 0x000000ff );
   }
}

} // namespace

struct sha256::impl
{
   bool final;
   unsigned char digest[ c_sha256_digest_size ];

   SHA256_CTX context;

   ~impl( )
   {
      for( size_t i = 0; i < c_sha256_digest_size; i++ )
         digest[ i ] = 0;
   }
};

sha256::sha256( )
{
   p_impl = new impl;

   init( );
}

sha256::sha256( const string& str )
{
   p_impl = new impl;

   init( );
   update( ( const unsigned char* )&str[ 0 ], str.length( ) );
}

sha256::sha256( const unsigned char* p_data, unsigned int length )
{
   p_impl = new impl;

   init( );
   update( p_data, length );
}

sha256::~sha256( )
{
   delete p_impl;
}

void sha256::init( )
{
   p_impl->final = false;
   sha256_init( &p_impl->context );
}

void sha256::update( const string& str, bool is_filename )
{
   if( !is_filename )
      update( ( const unsigned char* )&str[ 0 ], str.length( ) );
   else
   {
      FILE* fp = fopen( str.c_str( ), "rb" );

      if( !fp )
         throw runtime_error( "unable to open file '" + str + "' for input" );

      unsigned int len;
      unsigned char buf[ c_file_buflen ];

      while( !feof( fp ) )
      {
         len = fread( buf, 1, c_file_buflen, fp );
         update( buf, len );
      }

      fclose( fp );
   }
}

void sha256::update( const unsigned char* p_data, unsigned int length )
{
   unsigned char buf[ c_buffer_size ];

   unsigned int pos = 0;
   unsigned int chunk = min( length, c_buffer_size );

   if( p_impl->final )
      init( );

   while( chunk > 0 )
   {
      memcpy( buf, p_data + pos, chunk );
      sha256_update( &p_impl->context, buf, chunk );

      pos += chunk;
      length -= chunk;

      chunk = min( length, c_buffer_size );
   }
}

void sha256::copy_digest_to_buffer( unsigned char* p_buffer, unsigned int length )
{
   if( !p_impl->final )
   {
      p_impl->final = true;
      sha256_final( &p_impl->context, p_impl->digest );
   }

   // NOTE: Allows for copying just a number of the leading digest characters if desired.
   memcpy( p_buffer, p_impl->digest, min( length, ( unsigned int )c_sha256_digest_size ) );
}

void sha256::get_digest_as_string( string& s )
{
   if( !p_impl->final )
   {
      p_impl->final = true;
      sha256_final( &p_impl->context, p_impl->digest );
   }

   if( s.length( ) > c_sha256_digest_size * 2 )
      s.resize( c_sha256_digest_size * 2 );
   else if( s.length( ) != c_sha256_digest_size * 2 )
      s = string( c_sha256_digest_size * 2, '\0' );

   for( size_t i = 0, j = 0; i < c_sha256_digest_size; i++ )
   {
      s[ j++ ] = ascii_digit( ( p_impl->digest[ i ] & 0xf0 ) >> 4 );
      s[ j++ ] = ascii_digit( p_impl->digest[ i ] & 0x0f );
   }
}

string sha256::get_digest_as_string( char separator )
{
   if( !p_impl->final )
   {
      p_impl->final = true;
      sha256_final( &p_impl->context, p_impl->digest );
   }

   ostringstream outs;
   for( size_t i = 0; i < c_sha256_digest_size; i++ )
   {
      if( i && i % 4 == 0 && separator != '\0' )
         outs << separator;
      outs << hex << setw( 2 ) << setfill( '0' ) << ( unsigned )p_impl->digest[ i ];
   }

   return outs.str( );
}

string hmac_sha256( const string& key, const string& message )
{
   string s( c_sha256_digest_size * 2, '\0' );
   unsigned char buffer[ c_sha256_digest_size ];

   hmac_sha256( key, message, buffer );

   for( size_t i = 0, j = 0; i < c_sha256_digest_size; i++ )
   {
      s[ j++ ] = ascii_digit( ( buffer[ i ] & 0xf0 ) >> 4 );
      s[ j++ ] = ascii_digit( buffer[ i ] & 0x0f );
   }

   return s;
}

void hmac_sha256( const string& key, const string& message, unsigned char* p_buffer )
{
   int key_len = key.length( );
   int msg_len = message.length( );

   const unsigned char* p_key = ( const unsigned char* )key.data( );
   const unsigned char* p_msg = ( const unsigned char* )message.data( );

   unsigned char k_ipad[ 65 ];
   unsigned char k_opad[ 65 ];

   unsigned char tk[ c_sha256_digest_size ];
   unsigned char tk2[ c_sha256_digest_size ];
   unsigned char bufferIn[ 1024 ];
   unsigned char bufferOut[ 1024 ];

   if( key_len > 64 )
   {
      sha256 hash( key );
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
 
   sha256 hash1( bufferIn, 64 + msg_len );
   hash1.copy_digest_to_buffer( tk2 );

   memset( bufferOut, 0x00, 1024 );
   memcpy( bufferOut, k_opad, 64 );
   memcpy( bufferOut + 64, tk2, c_sha256_digest_size );
 
   sha256 hash2( bufferOut, 64 + c_sha256_digest_size );
   hash2.copy_digest_to_buffer( p_buffer );
}

#ifdef COMPILE_TESTBED_MAIN
int main( int argc, char* argv[ ] )
{
   if( argc > 1 && argv[ 1 ] == string( "/?" ) )
   {
      cout << "Usage: sha256 [<file>]" << endl;
      return 0;
   }

   sha256 hash;

   if( argc > 1 )
   {
      FILE* fp = fopen( argv[ 1 ], "rb" );

      if( !fp )
      {
         cerr << "Error: Unable to open file '" << argv[ 1 ] << "' for input." << endl;
         return 1;
      }

      unsigned int len;
      unsigned char buf[ c_file_buflen ];
      while( !feof( fp ) )
      {
         len = fread( buf, 1, c_file_buflen, fp );
         hash.update( buf, len );
      }
      fclose( fp );

      string s = hash.get_digest_as_string( ' ' );
      transform( s.begin( ), s.end( ), s.begin( ), ( int( * )( int ) )toupper );
      cout << "SHA-256 digest: " << s << endl;
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
         cout << "SHA-256 digest: " << s << '\n' << '\n';
      }
   }
}
#endif
