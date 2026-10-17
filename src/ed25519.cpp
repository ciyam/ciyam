// Copyright (c) 2015 Orson Peters <orsonpeters@gmail.com>
// Copyright (c) 2026 CIYAM Developers
//
// (sourced from https://github.com/orlp/ed25519 - ALTERED from the original: reformatted to the CIYAM coding standard, made
// C++, using CIYAM's "sha512" in place of its own and OpenSSL's random bytes (or CIYAM's "uuid") in place of "seed.c", a
// signature's S checked to be less than the group order on verifying, a shared secret of all zeros refused, and a key pair
// class in hex added; "add_scalar.c" left out)
//
// This software is provided 'as-is', without any express or implied warranty. In no event will the
// authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose, including commercial
// applications, and to alter it and redistribute it freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the
//    original software. If you use this software in a product, an acknowledgment in the product
//    documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as
//    being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstring>
#  include <string>
#  include <stdexcept>
#endif

#ifdef SSL_SUPPORT
#  include <openssl/rand.h>
#endif

#include "ed25519.h"

#include "sha512.h"
#include "utilities.h"
#include "ed25519_fe.h"
#include "ed25519_ge.h"
#include "ed25519_sc.h"

using namespace std;
using namespace ed25519_internal;

namespace
{

bool consttime_equal( const unsigned char* p_x, const unsigned char* p_y )
{
   unsigned char r = 0;

   for( size_t i = 0; i < 32; i++ )
      r |= p_x[ i ] ^ p_y[ i ];

   return !r;
}

// NOTE: The group order L, little endian - 2^252 + 27742317777372353535851937790883648493.
const unsigned char c_group_order[ ] =
{
   0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58, 0xd6, 0x9c, 0xf7, 0xa2, 0xde, 0xf9, 0xde, 0x14,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10
};

// NOTE: Whether a signature's S is less than L, as RFC 8032 requires - "orlp" checked only its top three bits, so S + L was
// taken too, a second valid signature for the same message (malleable). A signature is public, so no constant time needed.
bool is_canonical_scalar( const unsigned char* p_s )
{
   for( int i = 31; i >= 0; i-- )
   {
      if( p_s[ i ] < c_group_order[ i ] )
         return true;

      if( p_s[ i ] > c_group_order[ i ] )
         return false;
   }

   return false;
}

void clear_bytes( void* p_bytes, size_t num_bytes )
{
   volatile unsigned char* p = ( volatile unsigned char* )p_bytes;

   while( num_bytes-- )
      *p++ = 0;
}

// NOTE: "sha512::update( )" takes an "unsigned int" length, so a message is hashed in pieces in case it is longer.
void hash_message( sha512& hash, const unsigned char* p_message, size_t message_len )
{
   const size_t c_max_piece = 0x40000000;

   while( message_len > c_max_piece )
   {
      hash.update( p_message, ( unsigned int )c_max_piece );

      p_message += c_max_piece;
      message_len -= c_max_piece;
   }

   hash.update( p_message, ( unsigned int )message_len );
}

void check_hex( const string& hex, size_t num_bytes, const char* p_what )
{
   if( ( hex.length( ) != num_bytes * 2 ) || !are_hex_nibbles( hex ) )
      throw runtime_error( string( "an ed25519 " ) + p_what + " is " + to_string( num_bytes * 2 ) + " hex digits" );
}

}

void ed25519_create_seed( unsigned char* p_seed )
{
#ifdef SSL_SUPPORT
   if( RAND_bytes( p_seed, c_ed25519_seed_size ) != 1 )
      throw runtime_error( "unexpected failure to create an ed25519 seed" );
#else
   uuid first;
   uuid second;

   first.copy_bytes( p_seed );
   second.copy_bytes( p_seed + c_uuid_size );
#endif
}

void ed25519_create_keypair( unsigned char* p_public_key, unsigned char* p_private_key, const unsigned char* p_seed )
{
   ge_p3 A;

   sha512 hash( p_seed, c_ed25519_seed_size );

   hash.copy_digest_to_buffer( p_private_key );

   p_private_key[ 0 ] &= 248;
   p_private_key[ 31 ] &= 63;
   p_private_key[ 31 ] |= 64;

   ge_scalarmult_base( &A, p_private_key );
   ge_p3_tobytes( p_public_key, &A );
}

void ed25519_sign( unsigned char* p_signature, const unsigned char* p_message,
 size_t message_len, const unsigned char* p_public_key, const unsigned char* p_private_key )
{
   unsigned char hram[ c_sha512_digest_size ];
   unsigned char r[ c_sha512_digest_size ];

   ge_p3 R;

   sha512 r_hash;

   r_hash.update( p_private_key + 32, 32 );
   hash_message( r_hash, p_message, message_len );
   r_hash.copy_digest_to_buffer( r );

   sc_reduce( r );
   ge_scalarmult_base( &R, r );
   ge_p3_tobytes( p_signature, &R );

   sha512 hram_hash;

   hram_hash.update( p_signature, 32 );
   hram_hash.update( p_public_key, 32 );
   hash_message( hram_hash, p_message, message_len );
   hram_hash.copy_digest_to_buffer( hram );

   sc_reduce( hram );
   sc_muladd( p_signature + 32, hram, p_private_key, r );

   clear_bytes( r, sizeof( r ) );
}

bool ed25519_verify( const unsigned char* p_signature,
 const unsigned char* p_message, size_t message_len, const unsigned char* p_public_key )
{
   unsigned char h[ c_sha512_digest_size ];
   unsigned char checker[ 32 ];

   ge_p3 A;
   ge_p2 R;

   if( !is_canonical_scalar( p_signature + 32 ) )
      return false;

   if( ge_frombytes_negate_vartime( &A, p_public_key ) != 0 )
      return false;

   sha512 hash;

   hash.update( p_signature, 32 );
   hash.update( p_public_key, 32 );
   hash_message( hash, p_message, message_len );
   hash.copy_digest_to_buffer( h );

   sc_reduce( h );
   ge_double_scalarmult_vartime( &R, h, &A, p_signature + 32 );
   ge_tobytes( checker, &R );

   return consttime_equal( checker, p_signature );
}

void ed25519_key_exchange( unsigned char* p_shared_secret, const unsigned char* p_public_key, const unsigned char* p_private_key )
{
   unsigned char e[ 32 ];

   fe x1;
   fe x2;
   fe z2;
   fe x3;
   fe z3;
   fe tmp0;
   fe tmp1;

   unsigned int swap;
   unsigned int b;

   // copy the private key and make sure it's valid
   for( size_t i = 0; i < 32; ++i )
      e[ i ] = p_private_key[ i ];

   e[ 0 ] &= 248;
   e[ 31 ] &= 63;
   e[ 31 ] |= 64;

   // unpack the public key and convert edwards to montgomery
   // due to CodesInChaos: montgomeryX = (edwardsY + 1)*inverse(1 - edwardsY) mod p
   fe_frombytes( x1, p_public_key );
   fe_1( tmp1 );
   fe_add( tmp0, x1, tmp1 );
   fe_sub( tmp1, tmp1, x1 );
   fe_invert( tmp1, tmp1 );
   fe_mul( x1, tmp0, tmp1 );

   fe_1( x2 );
   fe_0( z2 );
   fe_copy( x3, x1 );
   fe_1( z3 );

   swap = 0;

   for( int pos = 254; pos >= 0; --pos )
   {
      b = e[ pos / 8 ] >> ( pos & 7 );
      b &= 1;
      swap ^= b;
      fe_cswap( x2, x3, swap );
      fe_cswap( z2, z3, swap );
      swap = b;

      // from montgomery.h
      fe_sub( tmp0, x3, z3 );
      fe_sub( tmp1, x2, z2 );
      fe_add( x2, x2, z2 );
      fe_add( z2, x3, z3 );
      fe_mul( z3, tmp0, x2 );
      fe_mul( z2, z2, tmp1 );
      fe_sq( tmp0, tmp1 );
      fe_sq( tmp1, x2 );
      fe_add( x3, z3, z2 );
      fe_sub( z2, z3, z2 );
      fe_mul( x2, tmp1, tmp0 );
      fe_sub( tmp1, tmp1, tmp0 );
      fe_sq( z2, z2 );
      fe_mul121666( z3, tmp1 );
      fe_sq( x3, x3 );
      fe_add( tmp0, tmp0, z3 );
      fe_mul( z3, x1, z2 );
      fe_mul( z2, tmp1, tmp0 );
   }

   fe_cswap( x2, x3, swap );
   fe_cswap( z2, z3, swap );

   fe_invert( z2, z2 );
   fe_mul( x2, x2, z2 );
   fe_tobytes( p_shared_secret, x2 );

   clear_bytes( e, sizeof( e ) );

   clear_bytes( x2, sizeof( fe ) );
   clear_bytes( z2, sizeof( fe ) );
   clear_bytes( x3, sizeof( fe ) );
   clear_bytes( z3, sizeof( fe ) );
   clear_bytes( tmp0, sizeof( fe ) );
   clear_bytes( tmp1, sizeof( fe ) );
}

ed25519_key_pair::ed25519_key_pair( )
{
   unsigned char seed[ c_ed25519_seed_size ];

   ed25519_create_seed( seed );

   init( seed );

   clear_bytes( seed, sizeof( seed ) );
}

ed25519_key_pair::ed25519_key_pair( const string& seed_hex )
{
   check_hex( seed_hex, c_ed25519_seed_size, "seed" );

   unsigned char seed[ c_ed25519_seed_size ];

   hex_decode( seed_hex, seed, sizeof( seed ) );

   init( seed );

   clear_bytes( seed, sizeof( seed ) );
}

ed25519_key_pair::~ed25519_key_pair( )
{
   clear_bytes( private_key, sizeof( private_key ) );
}

void ed25519_key_pair::init( const unsigned char* p_seed )
{
   ed25519_create_keypair( public_key, private_key, p_seed );
}

string ed25519_key_pair::get_public( ) const
{
   return hex_encode( public_key, sizeof( public_key ) );
}

string ed25519_key_pair::sign( const string& message ) const
{
   unsigned char signature[ c_ed25519_signature_size ];

   ed25519_sign( signature, ( const unsigned char* )message.data( ), message.length( ), public_key, private_key );

   return hex_encode( signature, sizeof( signature ) );
}

// NOTE: The X25519 shared secret with another's public key - all zeros, from a public key of small order, is refused: it
// would be the same whoever's key it was used with.
string ed25519_key_pair::construct_shared( const string& public_key_hex ) const
{
   check_hex( public_key_hex, c_ed25519_public_key_size, "public key" );

   unsigned char other[ c_ed25519_public_key_size ];
   unsigned char shared[ c_ed25519_shared_secret_size ];

   hex_decode( public_key_hex, other, sizeof( other ) );

   ed25519_key_exchange( shared, other, private_key );

   unsigned char any = 0;

   for( size_t i = 0; i < sizeof( shared ); i++ )
      any |= shared[ i ];

   if( !any )
      throw runtime_error( "unusable ed25519 public key for a shared secret" );

   string retval( hex_encode( shared, sizeof( shared ) ) );

   clear_bytes( shared, sizeof( shared ) );

   return retval;
}

bool ed25519_verify( const string& message, const string& signature_hex, const string& public_key_hex )
{
   if( ( signature_hex.length( ) != c_ed25519_signature_size * 2 ) || ( public_key_hex.length( ) != c_ed25519_public_key_size * 2 )
    || !are_hex_nibbles( signature_hex ) || !are_hex_nibbles( public_key_hex ) )
      return false;

   unsigned char signature[ c_ed25519_signature_size ];
   unsigned char public_key[ c_ed25519_public_key_size ];

   hex_decode( signature_hex, signature, sizeof( signature ) );
   hex_decode( public_key_hex, public_key, sizeof( public_key ) );

   return ed25519_verify( signature, ( const unsigned char* )message.data( ), message.length( ), public_key );
}
