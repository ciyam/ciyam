// Copyright (c) 2018-2026 CIYAM Developers
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

/*
Test Vectors
""
  CF83E1357EEFB8BD F1542850D66D8007 D620E4050B5715DC 83F4A921D36CE9CE 47D0D13C5D85F2B0 FF8318D2877EEC2F 63B931BD47417A81 A538327AF927DA3E
"abc"
  DDAF35A193617ABA CC417349AE204131 12E6FA4E89A97EA2 0A9EEEE64B55D39A 2192992A274FC1A8 36BA3C23A3FEEBBD 454D4423643CE80E 2A9AC94FA54CA49F
"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
  204A8FC6DDA82F0A 0CED7BEB8E08A416 57C16EF468B228A8 279BE331A703C335 96FD15C13B1B07F9 AA1D3BEA57789CA0 31AD85C7A71DD703 54EC631238CA3445
"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"
  8E959B75DAE313DA 8CF4F72814FC143F 8F7779C6EB9F7FA1 7299AEADB6889018 501D289E4900F7E4 331B99DEC4B5433A C7D329EEB6DD2654 5E96E55B874BE909
*/

/*
 * Updated to C++, zedwood.com 2012
 * Based on Olivier Gay's version
 * See Modified BSD License below: 
 *
 * FIPS 180-2 SHA-224/256/384/512 implementation
 * Issue date:  04/30/2005
 * http://www.ouah.org/ogay/sha2/
 *
 * Copyright (C) 2005, 2007 Olivier Gay <olivier.gay@a3.epfl.ch>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "sha512.h"
 
#include "utilities.h"

using namespace std;

#define SHA2_SHFR( x, n ) ( x >> n )

#define SHA2_ROTR( x, n ) ( ( x >> n ) | ( x << ( ( sizeof( x ) << 3 ) - n ) ) )
#define SHA2_ROTL( x, n ) ( ( x << n ) | ( x >> ( ( sizeof( x ) << 3 ) - n ) ) )

#define SHA2_CH( x, y, z ) ( ( x & y ) ^ ( ~x & z ) )
#define SHA2_MAJ( x, y, z ) ( ( x & y ) ^ ( x & z ) ^ ( y & z ) )

#define SHA512_F1( x ) ( SHA2_ROTR( x, 28 ) ^ SHA2_ROTR( x, 34 ) ^ SHA2_ROTR( x, 39 ) )
#define SHA512_F2( x ) ( SHA2_ROTR( x, 14 ) ^ SHA2_ROTR( x, 18 ) ^ SHA2_ROTR( x, 41 ) )
#define SHA512_F3( x ) ( SHA2_ROTR( x, 1 ) ^ SHA2_ROTR( x, 8 ) ^ SHA2_SHFR( x, 7 ) )
#define SHA512_F4( x ) ( SHA2_ROTR( x, 19 ) ^ SHA2_ROTR( x, 61 ) ^ SHA2_SHFR( x,  6 ) )

#define SHA2_UNPACK32( x, str ) \
{ \
   *( ( str ) + 3 ) = ( uint8_t ) ( ( x ) ); \
   *( ( str ) + 2 ) = ( uint8_t ) ( ( x ) >> 8 ); \
   *( ( str ) + 1 ) = ( uint8_t ) ( ( x ) >> 16 ); \
   *( ( str ) + 0 ) = ( uint8_t ) ( ( x ) >> 24 ); \
}

#define SHA2_UNPACK64( x, str ) \
{ \
   *( ( str ) + 7 ) = ( uint8_t ) ( ( x ) ); \
   *( ( str ) + 6 ) = ( uint8_t ) ( ( x ) >> 8 ); \
   *( ( str ) + 5 ) = ( uint8_t ) ( ( x ) >> 16 ); \
   *( ( str ) + 4 ) = ( uint8_t ) ( ( x ) >> 24 ); \
   *( ( str ) + 3 ) = ( uint8_t ) ( ( x ) >> 32 ); \
   *( ( str ) + 2 ) = ( uint8_t ) ( ( x ) >> 40 ); \
   *( ( str ) + 1 ) = ( uint8_t ) ( ( x ) >> 48 ); \
   *( ( str ) + 0 ) = ( uint8_t ) ( ( x ) >> 56 ); \
}

#define SHA2_PACK64( str, x ) \
{ \
   *( x ) = ( ( uint64_t ) *( ( str ) + 7 ) ) \
    | ( ( uint64_t ) *( ( str ) + 6 ) << 8 ) \
    | ( ( uint64_t ) *( ( str ) + 5 ) << 16 ) \
    | ( ( uint64_t ) *( ( str ) + 4 ) << 24 ) \
    | ( ( uint64_t ) *( ( str ) + 3 ) << 32 ) \
    | ( ( uint64_t ) *( ( str ) + 2 ) << 40 ) \
    | ( ( uint64_t ) *( ( str ) + 1 ) << 48 ) \
    | ( ( uint64_t ) *( ( str ) + 0 ) << 56 ); \
}

namespace
{

const int c_file_buflen = 8192;

const int c_sha512_block_size = 128;

const unsigned int c_buffer_size = 4096;

};

const uint64_t sha512::sha512_k[ 80 ] =
{
   UINT64_C( 0x428a2f98d728ae22 ), UINT64_C( 0x7137449123ef65cd ),
   UINT64_C( 0xb5c0fbcfec4d3b2f ), UINT64_C( 0xe9b5dba58189dbbc ),
   UINT64_C( 0x3956c25bf348b538 ), UINT64_C( 0x59f111f1b605d019 ),
   UINT64_C( 0x923f82a4af194f9b ), UINT64_C( 0xab1c5ed5da6d8118 ),
   UINT64_C( 0xd807aa98a3030242 ), UINT64_C( 0x12835b0145706fbe ),
   UINT64_C( 0x243185be4ee4b28c ), UINT64_C( 0x550c7dc3d5ffb4e2 ),
   UINT64_C( 0x72be5d74f27b896f ), UINT64_C( 0x80deb1fe3b1696b1 ),
   UINT64_C( 0x9bdc06a725c71235 ), UINT64_C( 0xc19bf174cf692694 ),
   UINT64_C( 0xe49b69c19ef14ad2 ), UINT64_C( 0xefbe4786384f25e3 ),
   UINT64_C( 0x0fc19dc68b8cd5b5 ), UINT64_C( 0x240ca1cc77ac9c65 ),
   UINT64_C( 0x2de92c6f592b0275 ), UINT64_C( 0x4a7484aa6ea6e483 ),
   UINT64_C( 0x5cb0a9dcbd41fbd4 ), UINT64_C( 0x76f988da831153b5 ),
   UINT64_C( 0x983e5152ee66dfab ), UINT64_C( 0xa831c66d2db43210 ),
   UINT64_C( 0xb00327c898fb213f ), UINT64_C( 0xbf597fc7beef0ee4 ),
   UINT64_C( 0xc6e00bf33da88fc2 ), UINT64_C( 0xd5a79147930aa725 ),
   UINT64_C( 0x06ca6351e003826f ), UINT64_C( 0x142929670a0e6e70 ),
   UINT64_C( 0x27b70a8546d22ffc ), UINT64_C( 0x2e1b21385c26c926 ),
   UINT64_C( 0x4d2c6dfc5ac42aed ), UINT64_C( 0x53380d139d95b3df ),
   UINT64_C( 0x650a73548baf63de ), UINT64_C( 0x766a0abb3c77b2a8 ),
   UINT64_C( 0x81c2c92e47edaee6 ), UINT64_C( 0x92722c851482353b ),
   UINT64_C( 0xa2bfe8a14cf10364 ), UINT64_C( 0xa81a664bbc423001 ),
   UINT64_C( 0xc24b8b70d0f89791 ), UINT64_C( 0xc76c51a30654be30 ),
   UINT64_C( 0xd192e819d6ef5218 ), UINT64_C( 0xd69906245565a910 ),
   UINT64_C( 0xf40e35855771202a ), UINT64_C( 0x106aa07032bbd1b8 ),
   UINT64_C( 0x19a4c116b8d2d0c8 ), UINT64_C( 0x1e376c085141ab53 ),
   UINT64_C( 0x2748774cdf8eeb99 ), UINT64_C( 0x34b0bcb5e19b48a8 ),
   UINT64_C( 0x391c0cb3c5c95a63 ), UINT64_C( 0x4ed8aa4ae3418acb ),
   UINT64_C( 0x5b9cca4f7763e373 ), UINT64_C( 0x682e6ff3d6b2b8a3 ),
   UINT64_C( 0x748f82ee5defb2fc ), UINT64_C( 0x78a5636f43172f60 ),
   UINT64_C( 0x84c87814a1f0ab72 ), UINT64_C( 0x8cc702081a6439ec ),
   UINT64_C( 0x90befffa23631e28 ), UINT64_C( 0xa4506cebde82bde9 ),
   UINT64_C( 0xbef9a3f7b2c67915 ), UINT64_C( 0xc67178f2e372532b ),
   UINT64_C( 0xca273eceea26619c ), UINT64_C( 0xd186b8c721c0c207 ),
   UINT64_C( 0xeada7dd6cde0eb1e ), UINT64_C( 0xf57d4f7fee6ed178 ),
   UINT64_C( 0x06f067aa72176fba ), UINT64_C( 0x0a637dc5a2c898a6 ),
   UINT64_C( 0x113f9804bef90dae ), UINT64_C( 0x1b710b35131c471b ),
   UINT64_C( 0x28db77f523047d84 ), UINT64_C( 0x32caab7b40c72493 ),
   UINT64_C( 0x3c9ebe0a15c9bebc ), UINT64_C( 0x431d67c49c100d4c ),
   UINT64_C( 0x4cc5d4becb3e42b6 ), UINT64_C( 0x597f299cfc657e2a ),
   UINT64_C( 0x5fcb6fab3ad6faec ), UINT64_C( 0x6c44198c4a475817 )
};

sha512::sha512( )
{
   init( );
}

sha512::sha512( const string& str )
{
   init( );
   update( ( const unsigned char* )&str[ 0 ], str.length( ) );
}

sha512::sha512( const unsigned char* p_data, unsigned int length )
{
   init( );
   update( p_data, length );
}

void sha512::update( const string& str, bool is_filename )
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

void sha512::update( const unsigned char* p_data, unsigned int length )
{
   unsigned char buf[ c_buffer_size ];

   unsigned int pos = 0;
   unsigned int chunk = min( length, c_buffer_size );

   if( is_final )
      init( );

   while( chunk > 0 )
   {
      memcpy( buf, p_data + pos, chunk );
      do_update( buf, chunk );

      pos += chunk;
      length -= chunk;

      chunk = min( length, c_buffer_size );
   }
}

void sha512::copy_digest_to_buffer( unsigned char* p_buffer, unsigned int length )
{
   if( !is_final )
      finalise( );

   // NOTE: Allows for copying just a number of the leading digest characters if desired.
   memcpy( p_buffer, m_digest, min( length, ( unsigned int )c_sha512_digest_size ) );
}

void sha512::get_digest_as_string( string& s )
{
   if( !is_final )
      finalise( );

   if( s.length( ) > c_sha512_digest_size * 2 )
      s.resize( c_sha512_digest_size * 2 );
   else if( s.length( ) != c_sha512_digest_size * 2 )
      s = string( c_sha512_digest_size * 2, '\0' );

   for( size_t i = 0, j = 0; i < c_sha512_digest_size; i++ )
   {
      s[ j++ ] = ascii_digit( ( m_digest[ i ] & 0xf0 ) >> 4 );
      s[ j++ ] = ascii_digit( m_digest[ i ] & 0x0f );
   }
}

string sha512::get_digest_as_string( char separator )
{
   if( !is_final )
      finalise( );

   ostringstream outs;
   for( size_t i = 0; i < c_sha512_digest_size; i++ )
   {
      if( i && i % 4 == 0 && separator != '\0' )
         outs << separator;
      outs << hex << setw( 2 ) << setfill( '0' ) << ( unsigned )m_digest[ i ];
   }

   return outs.str( );
}

void sha512::init( )
{
   m_h[ 0 ] = UINT64_C( 0x6a09e667f3bcc908 );
   m_h[ 1 ] = UINT64_C( 0xbb67ae8584caa73b );
   m_h[ 2 ] = UINT64_C( 0x3c6ef372fe94f82b );
   m_h[ 3 ] = UINT64_C( 0xa54ff53a5f1d36f1 );
   m_h[ 4 ] = UINT64_C( 0x510e527fade682d1 );
   m_h[ 5 ] = UINT64_C( 0x9b05688c2b3e6c1f );
   m_h[ 6 ] = UINT64_C( 0x1f83d9abfb41bd6b ); 
   m_h[ 7 ] = UINT64_C( 0x5be0cd19137e2179 );

   is_final = false;

   m_len = 0;
   m_tot_len = 0;
}

void sha512::finalise( )
{
   unsigned int block_nb;
   unsigned int pm_len;
   unsigned int len_b;

   block_nb = 1 + ( ( c_sha512_block_size - 17 ) < ( m_len % c_sha512_block_size ) );
   len_b = ( m_tot_len + m_len ) << 3;
   pm_len = block_nb << 7;

   memset( m_block + m_len, 0, pm_len - m_len );

   m_block[ m_len ] = 0x80;
   SHA2_UNPACK32( len_b, m_block + pm_len - 4 );

   transform( m_block, block_nb );

   for( int i = 0; i < 8; i++ )
   {
      SHA2_UNPACK64( m_h[ i ], &m_digest[ i << 3 ] );
   }

   is_final = true;
}

void sha512::do_update( const unsigned char* p_message, unsigned int len )
{
   unsigned int block_nb;
   unsigned int new_len, rem_len, tmp_len;
   const unsigned char* p_shifted_message;

   tmp_len = c_sha512_block_size - m_len;
   rem_len = len < tmp_len ? len : tmp_len;

   memcpy( &m_block[ m_len ], p_message, rem_len );

   if( m_len + len < c_sha512_block_size )
      m_len += len;
   else
   {
      new_len = len - rem_len;
      block_nb = new_len / c_sha512_block_size;

      p_shifted_message = p_message + rem_len;

      transform( m_block, 1 );
      transform( p_shifted_message, block_nb );

      rem_len = new_len % c_sha512_block_size;

      memcpy( m_block, &p_shifted_message[ block_nb << 7 ], rem_len );

      m_len = rem_len;
      m_tot_len += ( block_nb + 1 ) << 7;
   }
}

void sha512::transform( const unsigned char* p_message, unsigned int block_nb )
{
   uint64_t w[ 80 ];
   uint64_t wv[ 8 ];
   uint64_t t1, t2;

   int i, j;
   const unsigned char* p_sub_block;

   for( i = 0; i < ( int )block_nb; i++ )
   {
      p_sub_block = p_message + ( i << 7 );

      for( j = 0; j < 16; j++ )
      {
         SHA2_PACK64( &p_sub_block[ j << 3 ], &w[ j ] );
      }

      for( j = 16; j < 80; j++ )
      {
         w[ j ] = SHA512_F4( w[ j - 2 ] ) + w[ j - 7 ] + SHA512_F3( w[ j - 15 ] ) + w[ j - 16 ];
      }

      for( j = 0; j < 8; j++ )
         wv[ j ] = m_h[ j ];

      for( j = 0; j < 80; j++ )
      {
         t1 = wv[ 7 ] + SHA512_F2( wv[ 4 ] ) + SHA2_CH( wv[ 4 ], wv[ 5 ], wv[ 6 ] ) + sha512_k[ j ] + w[ j ];
         t2 = SHA512_F1( wv[ 0 ] ) + SHA2_MAJ( wv[ 0 ], wv[ 1 ], wv[ 2 ] );

         wv[ 7 ] = wv[ 6 ];
         wv[ 6 ] = wv[ 5 ];
         wv[ 5 ] = wv[ 4 ];
         wv[ 4 ] = wv[ 3 ] + t1;
         wv[ 3 ] = wv[ 2 ];
         wv[ 2 ] = wv[ 1 ];
         wv[ 1 ] = wv[ 0 ];
         wv[ 0 ] = t1 + t2;
      }

      for( j = 0; j < 8; j++ )
         m_h[ j ] += wv[ j ];
   }
}

string hmac_sha512( const string& key, const string& message )
{
   string s( c_sha512_digest_size * 2, '\0' );
   unsigned char buffer[ c_sha512_digest_size ];

   hmac_sha512( key, message, buffer );

   for( size_t i = 0, j = 0; i < c_sha512_digest_size; i++ )
   {
      s[ j++ ] = ascii_digit( ( buffer[ i ] & 0xf0 ) >> 4 );
      s[ j++ ] = ascii_digit( buffer[ i ] & 0x0f );
   }

   return s;
}

void hmac_sha512( const string& key, const string& message, unsigned char* p_buffer )
{
   string tmp_key( key );

   if( tmp_key.length( ) > c_sha512_block_size )
   {
      sha512 hash( tmp_key );

      tmp_key.resize( c_sha512_digest_size );
      hash.copy_digest_to_buffer( ( unsigned char* )tmp_key.data( ) );
   }

   while( tmp_key.length( ) < c_sha512_block_size )
      tmp_key += '\0';

   string o_key_pad( tmp_key );

   for( unsigned int i = 0; i < c_sha512_block_size; i++ )
      o_key_pad[ i ] = tmp_key[ i ] ^ ( char )0x5c;

   string i_key_pad( tmp_key );

   for( unsigned int i = 0; i < c_sha512_block_size; i++ )
      i_key_pad[ i ] = tmp_key[ i ] ^ ( char )0x36;

   sha512 hash1( i_key_pad + message );

   tmp_key.resize( c_sha512_digest_size );
   hash1.copy_digest_to_buffer( ( unsigned char* )tmp_key.data( ) );

   sha512 hash2( o_key_pad + tmp_key );

   hash2.copy_digest_to_buffer( p_buffer );
}
