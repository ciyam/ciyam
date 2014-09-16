// Copyright (c) 2014 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstring>
#  include <vector>
#  include <stdexcept>
#endif

#include <openssl/bn.h>
#include <openssl/ecdh.h>
#include <openssl/rand.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>

#include "crypto_keys.h"

#include "base64.h"
#include "sha256.h"
#include "utilities.h"
#include "crypt_stream.h"

using namespace std;

namespace
{

const int c_num_hash_bytes = 32;
const int c_num_secret_bytes = 32;
const int c_max_public_key_bytes = 65;

// NOTE: This function was sourced from the Bitcoin project.
bool check_secret( const unsigned char buf[ c_num_secret_bytes ] )
{
   static const unsigned char maxval[ c_num_secret_bytes ] =
   {
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
      0xBA, 0xAE, 0xDC, 0xE6, 0xAF, 0x48, 0xA0, 0x3B,
      0xBF, 0xD2, 0x5E, 0x8C, 0xD0, 0x36, 0x41, 0x40
   };

   bool is_zero = true;
   for( int i = 0; i < c_num_secret_bytes && is_zero; i++ )
   {
      if( buf[ i ] != 0 )
         is_zero = false;
   }

   if( is_zero )
      return false;

   for( int i = 0; i < c_num_secret_bytes; i++ )
   {
      if( buf[ i ] < maxval[ i ] )
         return true;

      if( buf[ i ] > maxval[ i ] )
         return false;
   }

   return true;
}

// NOTE: This function was sourced from the Bitcoin project.
bool EC_KEY_regenerate_key( EC_KEY* p_key, BIGNUM* p_priv_key )
{
   bool okay = false;

   BN_CTX* p_ctx = 0;
   EC_POINT* p_pub_key = 0;

   if( p_key )
   {
      const EC_GROUP* p_group = EC_KEY_get0_group( p_key );

      if( ( p_ctx = BN_CTX_new( ) ) != 0 )
      {
         p_pub_key = EC_POINT_new( p_group );

         if( p_pub_key )
         {
            if( EC_POINT_mul( p_group, p_pub_key, p_priv_key, 0, 0, p_ctx ) )
            {
               EC_KEY_set_public_key( p_key, p_pub_key );
               EC_KEY_set_private_key( p_key, p_priv_key );

               okay = true;
            }

            EC_POINT_free( p_pub_key );
         }

         BN_CTX_free( p_ctx );
      }
   }

   return okay;
}

}

struct public_key::impl
{
   EC_KEY* p_key;

   impl( )
   {
      p_key = EC_KEY_new_by_curve_name( NID_secp256k1 );

      if( !p_key )
         throw runtime_error( "failed to create EC_KEY for NID_secp256k1" );
   }

   ~impl( )
   {
      EC_KEY_free( p_key );
   }

   int get_public_key( unsigned char bytes[ c_max_public_key_bytes ] )
   {
      int size = i2o_ECPublicKey( p_key, 0 );
      if( !size || size > c_max_public_key_bytes )
         throw runtime_error( "unexpected invalid size returned by i2o_ECPublicKey" );

      int nsize = i2o_ECPublicKey( p_key, &bytes );
      if( size != nsize )
         throw runtime_error( "unexpected invalid size returned by i2o_ECPublicKey" );

      return nsize;
   }

   bool verify_signature( const unsigned char hash[ c_num_hash_bytes ], vector< unsigned char >& signature )
   {
      bool okay = false;

      if( ECDSA_verify( 0, &hash[ 0 ], sizeof( hash ), &signature[ 0 ], signature.size( ), p_key ) == 1 )
         okay = true;

      return okay;
   }
};

public_key::public_key( )
{
   p_impl = new impl;
}

public_key::public_key( const string& base64_key )
{
   p_impl = new impl;

   unsigned char buf[ c_max_public_key_bytes ];
   size_t size = base64::decode( base64_key, buf, c_max_public_key_bytes );

   const unsigned char* pbegin = &buf[ 0 ];

   o2i_ECPublicKey( &p_impl->p_key, &pbegin, size );
}

public_key::~public_key( )
{
   delete p_impl;
}

string public_key::get_public( ) const
{
   unsigned char buf[ c_max_public_key_bytes ];

   int len = p_impl->get_public_key( buf );

   return base64::encode( buf, len );
}

bool public_key::verify_signature( const string& msg, const string& sig ) const
{
   unsigned char buf[ c_num_hash_bytes ];

   sha256 hash( msg );
   hash.copy_digest_to_buffer( buf );

   vector< unsigned char > signature;
   signature.resize( base64::decode_size( sig ) );

   base64::decode( sig, &signature[ 0 ], signature.size( ) );

   return p_impl->verify_signature( buf, signature );
}

struct private_key::impl
{
   public_key::impl* p_pub_impl;

   impl( public_key::impl* p_pub_impl ) : p_pub_impl( p_pub_impl ) { }

   // NOTE: This function was sourced from the Bitcoin project.
   void get_secret_bytes( unsigned char bytes[ c_num_secret_bytes ] )
   {
      const BIGNUM* p_bn = EC_KEY_get0_private_key( p_pub_impl->p_key );
      if( !p_bn )
         throw runtime_error( "unexpected EC_KEY_get0_private_key failure in get_secret_bytes" );

      int num_bytes = BN_num_bytes( p_bn );
      int num = BN_bn2bin( p_bn, &bytes[ c_num_secret_bytes - num_bytes ] );
      if( num != num_bytes )
         throw runtime_error( "unexpected num != num_bytes in get_secret_bytes" );

      memset( bytes, 0, c_num_secret_bytes - num_bytes );
   }

   // NOTE: This function was sourced from the Bitcoin project.
   void set_secret_bytes( const unsigned char bytes[ c_num_secret_bytes ] )
   {
      bool rc;
      BIGNUM bn;
      BN_init( &bn );

      rc = BN_bin2bn( bytes, c_num_secret_bytes, &bn );
      if( !rc )
         throw runtime_error( "unexpected failure for BN_bin2bn in set_secret_bytes" );

      if( !EC_KEY_regenerate_key( p_pub_impl->p_key, &bn ) )
         throw runtime_error( "unexpected failure for EC_KEY_regenerate_key in set_secret_byts" );

      BN_clear_free( &bn );
   }

   void sign_message( const unsigned char hash[ c_num_hash_bytes ], vector< unsigned char >& signature )
   {
      ECDSA_SIG* p_sig = ECDSA_do_sign( &hash[ 0 ], sizeof( hash ), p_pub_impl->p_key );
      if( !p_sig )
         throw runtime_error( "unexpected failure for ECDSA_do_sign in sign_message" );

      signature.clear( );

      unsigned int size = ECDSA_size( p_pub_impl->p_key );
      signature.resize( size );

      unsigned char* p = &signature[ 0 ];
      size = i2d_ECDSA_SIG( p_sig, &p );
      signature.resize( size );

      ECDSA_SIG_free( p_sig );
   }

   void get_shared_secret( unsigned char bytes[ c_num_secret_bytes ], const EC_KEY* p_key )
   {
      const EC_POINT* p_pub = EC_KEY_get0_public_key( p_key );
      const BIGNUM* p_priv = EC_KEY_get0_private_key( p_pub_impl->p_key );

      ECDH_compute_key( &bytes[ 0 ], c_num_secret_bytes, p_pub, p_pub_impl->p_key, 0 );
   }
};

private_key::private_key( )
{
   unsigned char buf[ c_num_secret_bytes ];
   do
   {
      RAND_bytes( buf, c_num_secret_bytes );
   } while( !check_secret( buf ) );

   p_impl = new impl( public_key::p_impl );
   p_impl->set_secret_bytes( buf );
}

private_key::private_key( const string& secret )
{
   unsigned char buf[ c_num_secret_bytes ];
   hex_decode( secret, buf, c_num_secret_bytes );

   if( !check_secret( buf ) )
      throw runtime_error( "unexpected invalid secret key" );

   p_impl = new impl( public_key::p_impl );
   p_impl->set_secret_bytes( buf );
}

private_key::~private_key( )
{
   delete p_impl;
}

string private_key::get_secret( ) const
{
   unsigned char buf[ c_num_secret_bytes ];
   p_impl->get_secret_bytes( buf );

   return hex_encode( buf, c_num_secret_bytes );
}

string private_key::decrypt_message( const public_key& pub, const string& base64, const char* p_id )
{
   unsigned char buf[ c_num_secret_bytes ];
   p_impl->get_shared_secret( buf, pub.p_impl->p_key );

   string decoded( base64::decode( base64 ) );

   if( p_id )
   {
      sha256 hash( p_id );
      unsigned char buf2[ c_num_secret_bytes ];

      hash.copy_digest_to_buffer( buf2 );

      for( size_t i = 0; i < c_num_secret_bytes; i++ )
         buf[ i ] ^= buf2[ i ];
   }

   return password_decrypt( base64, hex_encode( buf, c_num_secret_bytes ), true );
}

string private_key::encrypt_message(
 const public_key& pub, const string& message, const char* p_id, bool add_salt )
{
   unsigned char buf[ c_num_secret_bytes ];
   p_impl->get_shared_secret( buf, pub.p_impl->p_key );

   string encrypted_message( message );

   if( p_id )
   {
      sha256 hash( p_id );
      unsigned char buf2[ c_num_secret_bytes ];

      hash.copy_digest_to_buffer( buf2 );

      for( size_t i = 0; i < c_num_secret_bytes; i++ )
         buf[ i ] ^= buf2[ i ];
   }

   return password_encrypt( message, hex_encode( buf, c_num_secret_bytes ), true, add_salt );
}

string private_key::construct_shared( const public_key& pub )
{
   unsigned char buf[ c_num_secret_bytes ];
   p_impl->get_shared_secret( buf, pub.p_impl->p_key );

   return hex_encode( buf, sizeof( buf ) );
}

string private_key::construct_signature( const string& msg ) const
{
   unsigned char buf[ c_num_hash_bytes ];

   sha256 hash( msg );
   hash.copy_digest_to_buffer( buf );

   vector< unsigned char > signature;
   p_impl->sign_message( buf, signature );

   return base64::encode( &signature[ 0 ], signature.size( ) );
}

