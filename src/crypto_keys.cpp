// Copyright (c) 2014-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstring>
#  include <map>
#  include <memory>
#  include <iomanip>
#  include <sstream>
#  include <stdexcept>
#endif

#include <openssl/bn.h>
#include <openssl/ecdh.h>
#include <openssl/rand.h>
#include <openssl/ecdsa.h>
#include <openssl/ripemd.h>
#include <openssl/obj_mac.h>
#include <openssl/opensslv.h>

#ifdef OPENSSL_VERSION_MAJOR
#  if OPENSSL_VERSION_MAJOR >= 3
#     define CRYPTO_NO_BIGNUM_INIT
#  endif
#endif

#ifdef LIBRESSL_VERSION_NUMBER
#  if LIBRESSL_VERSION_NUMBER >= 0x30000000L
#     define CRYPTO_NO_BIGNUM_INIT
#  endif
#endif

// KLUDGE: Work-around for OpenSSL 3.0 changes.
#ifdef CRYPTO_NO_BIGNUM_INIT
void BN_init( void* ) { }
#endif

// KLUDGE: Work-around for OpenSSL 1.1 changes.
#ifdef OPENSSL_VERSION_NUMBER
#  if OPENSSL_VERSION_NUMBER >= 0x10100000L
typedef struct ECDSA_SIG_st
{
   BIGNUM* r;
   BIGNUM* s;
} ECDSA_SIG;
#  endif
#endif

#include "crypto_keys.h"

#include "base64.h"
#include "sha256.h"
#include "utilities.h"
#include "crypt_stream.h"

#include <stdio.h>

using namespace std;

namespace
{

const int c_tx_max_msg_len = 80;
const int c_num_secret_bytes = 32;
const int c_max_public_key_bytes = 65;

const char* const c_zero_sequence_bytes = "00000000";
const char* const c_default_sequence_bytes = "ffffffff";

const char* const c_zero_amount = "0000000000000000";

const char* const c_empty_sig_script = "00";
const char* const c_sig_script_marker = "XX";
const char* const c_hash_code_type_all = "01000000";

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

static const char* gp_base58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

// NOTE: This function was sourced from the Bitcoin project.
string base58_encode( const unsigned char* p_buf, size_t len )
{
   string str;

   BN_CTX* p_ctx = BN_CTX_new( );
   if( !p_ctx )
      throw runtime_error( "unexpected BN_CTX_new failure in base58_encode" );

   BIGNUM* p_bn0;
   BIGNUM* p_bn58;

   p_bn0 = BN_new( );
   p_bn58 = BN_new( );

   BN_init( p_bn0 );
   BN_init( p_bn58 );

   BN_set_word( p_bn0, 0 );
   BN_set_word( p_bn58, 58 );

   vector< unsigned char > temp_buf( len + 1, 0 );
   reverse_copy( p_buf, p_buf + len, temp_buf.begin( ) );

   BIGNUM* p_bn;
   p_bn = BN_new( );

   BN_init( p_bn );

   vector< unsigned char > temp_buf2( temp_buf.size( ) + 4, 0 );
   unsigned int size = temp_buf.size( );

   temp_buf2[ 0 ] = ( size >> 24 ) & 0xff;
   temp_buf2[ 1 ] = ( size >> 16 ) & 0xff;
   temp_buf2[ 2 ] = ( size >> 8 ) & 0xff;
   temp_buf2[ 3 ] = ( size >> 0 ) & 0xff;

   reverse_copy( temp_buf.begin( ), temp_buf.end( ), temp_buf2.begin( ) + 4 );
   BN_mpi2bn( &temp_buf2[ 0 ], temp_buf2.size( ), p_bn );

   str.reserve( len * 138 / 100 + 1 );

   BIGNUM* p_dv;
   BIGNUM* p_rem;

   p_dv = BN_new( );
   p_rem = BN_new( );

   BN_init( p_dv );
   BN_init( p_rem );

   while( BN_cmp( p_bn, p_bn0 ) > 0 )
   {
      if( !BN_div( p_dv, p_rem, p_bn, p_bn58, p_ctx ) )
         throw runtime_error( "unexpected BN_div failure in base58_encode" );

      BN_copy( p_bn, p_dv );
      unsigned int c = BN_get_word( p_rem );

      str += gp_base58[ c ];
   }

   BN_clear_free( p_rem );
   BN_clear_free( p_dv );

   for( const unsigned char* p = p_buf; p < p_buf + len && *p == 0; p++ )
      str += gp_base58[ 0 ];

   reverse( str.begin( ), str.end( ) );

   BN_clear_free( p_bn );

   BN_clear_free( p_bn58 );
   BN_clear_free( p_bn0 );

   BN_CTX_free( p_ctx );

   return str;
}

// NOTE: This function was sourced from the Bitcoin project.
void base58_decode( const string& encoded, vector< unsigned char >& buf )
{
   BN_CTX* p_ctx = BN_CTX_new( );
   if( !p_ctx )
      throw runtime_error( "unexpected BN_CTX_new failure in base58_decode" );

   BIGNUM* p_bn;
   BIGNUM* p_bn58;
   BIGNUM* p_bnchar;

   p_bn = BN_new( );
   p_bn58 = BN_new( );
   p_bnchar = BN_new( );

   BN_init( p_bn );
   BN_init( p_bn58 );
   BN_init( p_bnchar );

   BN_set_word( p_bn, 0 );
   BN_set_word( p_bn58, 58 );

   string error;

   try
   {
      for( size_t i = 0; i < encoded.size( ); i++ )
      {
         const char* pc = strchr( gp_base58, encoded[ i ] );
         if( !pc )
            throw runtime_error( "invalid base58 encoded input '" + encoded + "'" );

         BN_set_word( p_bnchar, pc - gp_base58 );

         if( !BN_mul( p_bn, p_bn, p_bn58, p_ctx ) )
            throw runtime_error( "unexpected BN_mul failure in base58_decode" );

         BN_add( p_bn, p_bn, p_bnchar );
      }

      vector< unsigned char > temp_buf;

      unsigned int size = BN_bn2mpi( p_bn, 0 );
      if( size > 4 )
      {
         temp_buf.resize( size );
         BN_bn2mpi( p_bn, &temp_buf[ 0 ] );
         temp_buf.erase( temp_buf.begin( ), temp_buf.begin( ) + 4 );
         reverse( temp_buf.begin( ), temp_buf.end( ) );
      }

      if( temp_buf.size( ) >= 2 && temp_buf.end( )[ -1 ] == 0 && temp_buf.end( )[ -2 ] >= 0x80 )
         temp_buf.erase( temp_buf.end( ) - 1 );

      int leading = 0;
      for( size_t i = 0; i < encoded.size( ); i++ )
      {
         if( encoded[ i ] == gp_base58[ 0 ] )
            ++leading;
         else
            break;
      }

      buf.assign( leading + temp_buf.size( ), 0 );
      reverse_copy( temp_buf.begin( ), temp_buf.end( ), buf.end( ) - temp_buf.size( ) );
   }
   catch( exception& x )
   {
      error = x.what( );
   }
   catch( ... )
   {
      error = "unexpected unknown exception occurred in base58_decode";
   }

   BN_clear_free( p_bnchar );
   BN_clear_free( p_bn58 );
   BN_clear_free( p_bn );

   BN_CTX_free( p_ctx );

   if( !error.empty( ) )
      throw runtime_error( error );
}

bool is_big_endian( )
{
   union
   {
      uint32_t i;
      char c[ 4 ];
   } bint = { 0x01020304 };

   return bint.c[ 0 ] == 1;
}

inline uint32_t fix_endian( uint32_t x )
{
   if( is_big_endian( ) )
      return x;
   else
      return ( ( ( x & 0x000000ff ) << 24 ) | ( ( x & 0x0000ff00 ) << 8 )
       | ( ( x & 0x00ff0000 ) >> 8 ) | ( ( x & 0xff000000 ) >> 24 ) );
}

inline uint64_t fix_endian( uint64_t x )
{
   if( !is_big_endian( ) )
   {
      x = ( x & 0x00000000ffffffff ) << 32 | ( x & 0xffffffff00000000 ) >> 32;
      x = ( x & 0x0000ffff0000ffff ) << 16 | ( x & 0xffff0000ffff0000 ) >> 16;
      x = ( x & 0x00ff00ff00ff00ff ) << 8 | ( x & 0xff00ff00ff00ff00 ) >> 8;
   }

   return x;
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

   int get_public_key( unsigned char bytes[ c_max_public_key_bytes ], bool compressed )
   {
      EC_KEY_set_conv_form( p_key, compressed ? POINT_CONVERSION_COMPRESSED : POINT_CONVERSION_UNCOMPRESSED );

      int size = i2o_ECPublicKey( p_key, 0 );
      if( !size || size > c_max_public_key_bytes )
         throw runtime_error( "unexpected invalid size returned by i2o_ECPublicKey" );

      int nsize = i2o_ECPublicKey( p_key, &bytes );
      if( size != nsize )
         throw runtime_error( "unexpected invalid size returned by i2o_ECPublicKey" );

      return nsize;
   }

   bool verify_signature( const unsigned char hash[ c_sha256_digest_size ], vector< unsigned char >& signature )
   {
      bool okay = false;

      if( ECDSA_verify( 0, &hash[ 0 ], c_sha256_digest_size, &signature[ 0 ], signature.size( ), p_key ) == 1 )
         okay = true;

      return okay;
   }
};

public_key::public_key( )
{
   p_impl = new impl;
}

public_key::public_key( const string& encoded, bool use_base64 )
{
   p_impl = new impl;

   unsigned char buf[ c_max_public_key_bytes ];

   size_t size;
   if( use_base64 )
      size = base64::decode( encoded, buf, c_max_public_key_bytes );
   else
   {
      size = encoded.size( ) / 2;
      hex_decode( encoded, buf, c_max_public_key_bytes );
   }

   const unsigned char* pbegin = &buf[ 0 ];

   o2i_ECPublicKey( &p_impl->p_key, &pbegin, size );
}

public_key::~public_key( )
{
   delete p_impl;
}

string public_key::get_public( bool compressed, bool use_base64 ) const
{
   unsigned char buf[ c_max_public_key_bytes ];

   int len = p_impl->get_public_key( buf, compressed );

   if( !use_base64 )
      return hex_encode( buf, len );
   else
      return base64::encode( buf, len );
}

string public_key::get_hash160( bool compressed ) const
{
   unsigned char buf1[ c_max_public_key_bytes ];

   int len = p_impl->get_public_key( buf1, compressed );

   sha256 hash1( buf1, len );
   hash1.copy_digest_to_buffer( buf1 );

   unsigned char buf2[ RIPEMD160_DIGEST_LENGTH ];
   RIPEMD160( buf1, c_sha256_digest_size, buf2 );

   return hex_encode( buf2, RIPEMD160_DIGEST_LENGTH );
}

string public_key::get_address( bool compressed, bool use_override, address_prefix override ) const
{
   unsigned char buf1[ c_max_public_key_bytes ];

   int len = p_impl->get_public_key( buf1, compressed );

   sha256 hash1( buf1, len );
   hash1.copy_digest_to_buffer( buf1 );

   unsigned char buf2[ RIPEMD160_DIGEST_LENGTH + 5 ];
   RIPEMD160( buf1, c_sha256_digest_size, buf2 + 1 );

   if( !use_override )
      buf2[ 0 ] = e_address_prefix_btc_p2pkh_mainnet;
   else
      buf2[ 0 ] = override ? override : e_address_prefix_btc_p2pkh_testnet;

   unsigned char buf3[ c_sha256_digest_size ];
   sha256 hash2( buf2, RIPEMD160_DIGEST_LENGTH + 1 );
   hash2.copy_digest_to_buffer( buf3 );

   hash2.update( buf3, c_sha256_digest_size );
   hash2.copy_digest_to_buffer( buf3 );

   memcpy( &buf2[ RIPEMD160_DIGEST_LENGTH + 1 ], buf3, 4 );

   return base58_encode( buf2, RIPEMD160_DIGEST_LENGTH + 5 );
}

bool public_key::verify_signature( const string& msg, const string& sig ) const
{
   unsigned char buf[ c_sha256_digest_size ];

   sha256 hash( msg );
   hash.copy_digest_to_buffer( buf );

   vector< unsigned char > signature;
   signature.resize( base64::decode_size( sig ) );

   base64::decode( sig, &signature[ 0 ], signature.size( ) );

   return p_impl->verify_signature( buf, signature );
}

bool public_key::verify_signature( const unsigned char* p_data, const string& sig ) const
{
   vector< unsigned char > signature;
   signature.resize( sig.size( ) / 2 );

   hex_decode( sig, &signature[ 0 ], signature.size( ) );

   return p_impl->verify_signature( p_data, signature );
}

string public_key::address_to_hash160( const string& address )
{
   if( address.length( ) < 33 )
      throw runtime_error( "invalid address '" + address + "'" );

   vector< unsigned char > dbuf;
   base58_decode( address, dbuf );

   return hex_encode( &dbuf[ 1 ], dbuf.size( ) - 5 );
}

string public_key::hash160_to_address(
 const string& hash160, bool use_override, address_prefix override )
{
   if( hash160.length( ) != RIPEMD160_DIGEST_LENGTH * 2 )
      throw runtime_error( "invalid hash160 '" + hash160 + "'" );

   unsigned char buf[ RIPEMD160_DIGEST_LENGTH + 5 ];
   hex_decode( hash160, buf + 1, RIPEMD160_DIGEST_LENGTH );

   if( !use_override )
      buf[ 0 ] = e_address_prefix_btc_p2pkh_mainnet;
   else
      buf[ 0 ] = override ? override : e_address_prefix_btc_p2pkh_testnet;

   unsigned char buf2[ c_sha256_digest_size ];
   sha256 hash2( buf, RIPEMD160_DIGEST_LENGTH + 1 );
   hash2.copy_digest_to_buffer( buf2 );

   hash2.update( buf2, c_sha256_digest_size );
   hash2.copy_digest_to_buffer( buf2 );

   memcpy( &buf[ RIPEMD160_DIGEST_LENGTH + 1 ], buf2, 4 );

   return base58_encode( buf, RIPEMD160_DIGEST_LENGTH + 5 );
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
      BIGNUM* p_bn;

      p_bn = BN_new( );

      BN_init( p_bn );

      rc = BN_bin2bn( bytes, c_num_secret_bytes, p_bn );
      if( !rc )
         throw runtime_error( "unexpected failure for BN_bin2bn in set_secret_bytes" );

      if( !EC_KEY_regenerate_key( p_pub_impl->p_key, p_bn ) )
         throw runtime_error( "unexpected failure for EC_KEY_regenerate_key in set_secret_byts" );

      BN_clear_free( p_bn );
   }

   // NOTE: This function was sourced from the Bitcoin project.
   void sign_message( const unsigned char hash[ c_sha256_digest_size ], vector< unsigned char >& signature )
   {
      ECDSA_SIG* p_sig = ECDSA_do_sign( &hash[ 0 ], c_sha256_digest_size, p_pub_impl->p_key );
      if( !p_sig )
         throw runtime_error( "unexpected failure for ECDSA_do_sign in sign_message" );

      BN_CTX* p_ctx = BN_CTX_new( );
      BN_CTX_start( p_ctx );

      const EC_GROUP* p_group = EC_KEY_get0_group( p_pub_impl->p_key );

      BIGNUM* p_order = BN_CTX_get( p_ctx );
      BIGNUM* p_halforder = BN_CTX_get( p_ctx );

      EC_GROUP_get_order( p_group, p_order, p_ctx );

      BN_rshift1( p_halforder, p_order );

      // NOTE: Enforce low S values, by negating the value (modulo the order) if above order / 2.
      if( BN_cmp( p_sig->s, p_halforder ) > 0 )
         BN_sub( p_sig->s, p_order, p_sig->s );

      BN_CTX_end( p_ctx );
      BN_CTX_free( p_ctx );

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
   p_impl = new impl( public_key::p_impl );

   unsigned char buf[ c_num_secret_bytes ];

   do
   {
      RAND_bytes( buf, c_num_secret_bytes );
   } while( !check_secret( buf ) );

   p_impl->set_secret_bytes( buf );
}

private_key::private_key( const string& secret, bool is_wif_format, bool* p_is_compressed )
{
   p_impl = new impl( public_key::p_impl );

   unsigned char buf[ c_num_secret_bytes ];

   memset( buf, 0, sizeof( buf ) );

   if( secret.length( ) < 30 )
   {
      if( is_wif_format )
         throw runtime_error( "invalid WIF '" + secret + "' in private_key ctor" );

      // NOTE: Allow an insecure secret to be used for testing purposes (i.e. do
      // *not* use this method for anything other than performing simple tests).
      for( size_t i = 0; i < secret.length( ); i++ )
         buf[ i ] = secret[ i ];
   }
   else
   {
      if( !is_wif_format )
      {
         if( secret.length( ) > 50 )
            hex_decode( secret, buf, c_num_secret_bytes );
         else
            base64::decode( secret, buf, c_num_secret_bytes );
      }
      else
      {
         // NOTE: If the first encoded byte is K, L or c then is compressed.
         if( p_is_compressed && !secret.empty( ) )
         {
            if( secret[ 0 ] == 'K' || secret[ 0 ] == 'L' || secret[ 0 ] == 'c' )
               *p_is_compressed = true;
            else
               *p_is_compressed = false;
         }

         vector< unsigned char > dbuf;

         base58_decode( secret, dbuf );

         if( dbuf.size( ) < c_num_secret_bytes + 5 )
            throw runtime_error( "invalid WIF '" + secret + "' in private_key ctor" );

         sha256 hash( &dbuf[ 0 ], dbuf.size( ) - 4 );
         hash.copy_digest_to_buffer( buf );

         hash.update( buf, c_sha256_digest_size );
         hash.copy_digest_to_buffer( buf );

         if( memcmp( buf, &dbuf[ dbuf.size( ) - 4 ], 4 ) != 0 )
            throw runtime_error( "invalid WIF checksum for '" + secret + "' in private_key ctor" );

         memcpy( buf, &dbuf[ 1 ], c_num_secret_bytes );
      }
   }

   if( !check_secret( buf ) )
      throw runtime_error( "unexpected invalid secret in private_key ctor" );

   p_impl->set_secret_bytes( buf );

   memset( buf, '\0', sizeof( buf ) );
}

private_key::~private_key( )
{
   delete p_impl;
}

void private_key::get_secret( string& s, bool use_base64 ) const
{
   unsigned char buf[ c_num_secret_bytes ];
   p_impl->get_secret_bytes( buf );

   if( !use_base64 )
      hex_encode( s, buf, c_num_secret_bytes );
   else
      base64::encode( s, buf, c_num_secret_bytes );

   memset( buf, 0, c_num_secret_bytes );
}

string private_key::get_wif_secret( bool compressed, bool use_override, address_prefix override ) const
{
   unsigned char buf[ c_num_secret_bytes + 6 ];

   if( !use_override )
      buf[ 0 ] = e_address_prefix_btc_wif_mainnet;
   else
      buf[ 0 ] = override ? override : e_address_prefix_btc_wif_testnet;

   p_impl->get_secret_bytes( buf + 1 );

   if( compressed )
      buf[ c_num_secret_bytes + 1 ] = 0x01;

   unsigned char buf2[ c_sha256_digest_size ];

   sha256 hash( buf, compressed ? c_num_secret_bytes + 2 : c_num_secret_bytes + 1 );
   hash.copy_digest_to_buffer( buf2 );

   hash.update( buf2, c_sha256_digest_size );
   hash.copy_digest_to_buffer( buf2 );

   memcpy( buf + ( compressed ? c_num_secret_bytes + 2 : c_num_secret_bytes + 1 ), buf2, 4 );

   return base58_encode( buf, compressed ? c_num_secret_bytes + 6 : c_num_secret_bytes + 5 );
}

void private_key::decrypt_message( string& s,
 const public_key& pub, const string& base64, const char* p_id ) const
{
   unsigned char buf[ c_num_secret_bytes ];
   p_impl->get_shared_secret( buf, pub.p_impl->p_key );

   if( p_id )
   {
      sha256 hash( p_id );
      unsigned char buf2[ c_sha256_digest_size ];

      hash.copy_digest_to_buffer( buf2 );

      for( size_t i = 0; i < c_sha256_digest_size; i++ )
         buf[ i ] ^= buf2[ i ];
   }

   data_decrypt( s, base64, hex_encode( buf, c_num_secret_bytes ) );
}

void private_key::encrypt_message( string& s,
 const public_key& pub, const string& message, const char* p_id, bool add_salt ) const
{
   unsigned char buf[ c_num_secret_bytes ];
   p_impl->get_shared_secret( buf, pub.p_impl->p_key );

   if( p_id )
   {
      sha256 hash( p_id );
      unsigned char buf2[ c_sha256_digest_size ];

      hash.copy_digest_to_buffer( buf2 );

      for( size_t i = 0; i < c_sha256_digest_size; i++ )
         buf[ i ] ^= buf2[ i ];
   }

   data_encrypt( s, message, hex_encode( buf, c_num_secret_bytes ), true, add_salt );
}

void private_key::construct_shared( string& s, const public_key& pub ) const
{
   unsigned char buf[ c_num_secret_bytes ];
   p_impl->get_shared_secret( buf, pub.p_impl->p_key );

   hex_encode( s, buf, sizeof( buf ) );
}

string private_key::construct_signature( const unsigned char* p_digest, bool use_base64 ) const
{
   vector< unsigned char > signature;

   p_impl->sign_message( p_digest, signature );

   return use_base64 ? base64::encode( &signature[ 0 ], signature.size( ) ) : hex_encode( &signature[ 0 ], signature.size( ) );
}

string private_key::construct_signature( const string& msg, bool use_base64 ) const
{
   unsigned char buf[ c_sha256_digest_size ];

   sha256 hash( msg );
   hash.copy_digest_to_buffer( buf );

   vector< unsigned char > signature;
   p_impl->sign_message( buf, signature );

   return use_base64 ? base64::encode( &signature[ 0 ], signature.size( ) ) : hex_encode( &signature[ 0 ], signature.size( ) );
}

string create_p2sh_address( const string& hex_script, bool use_override, address_prefix override )
{
   size_t size = hex_script.size( ) / 2;

   unique_ptr< unsigned char > up_buf;

   up_buf.reset( new unsigned char[ size ] );

   hex_decode( hex_script, up_buf.get( ), size );

   unsigned char buf1[ c_sha256_digest_size ];

   sha256 hash( up_buf.get( ), size );

   hash.copy_digest_to_buffer( buf1 );

   unsigned char buf2[ RIPEMD160_DIGEST_LENGTH ];
   RIPEMD160( buf1, c_sha256_digest_size, buf2 );

   return public_key::hash160_to_address(
    hex_encode( buf2, sizeof( buf2 ) ), true, use_override
    ? ( override ? override : e_address_prefix_btc_p2sh_testnet ) : e_address_prefix_btc_p2sh_mainnet );
}

string create_secret_for_address_prefix_with_leading_hash160_bytes( const string& prefix, const string& bytes )
{
   string secret;

   while( true )
   {
      private_key priv_key;

      if( priv_key.get_address( ).find( prefix ) == 0 )
      {
         if( priv_key.get_hash160( ).substr( 0, bytes.length( ) ) == bytes )
         {
            secret = priv_key.get_secret( );
            break;
         }
      }
   }

   return secret;
}

void generate_secrets_for_leading_byte_encoded_message( const string& message, vector< string >& secrets )
{
   for( size_t i = 0; i < message.size( ); i++ )
   {
      string bytes( hex_encode( ( const unsigned char* )&message[ i ], 1 ) );
      string hash160( bytes + "00000000000000000000000000000000000000" );

      string address( public_key::hash160_to_address( hash160 ) );

      string prefix( address.substr( 0, 2 ) );
      secrets.push_back( create_secret_for_address_prefix_with_leading_hash160_bytes( prefix, bytes ) );
   }
}

string decode_message_from_leading_byte_encoded_addresses( const vector< string >& addresses )
{
   string encoded;
   for( size_t i = 0; i < addresses.size( ); i++ )
   {
      string hash160( public_key::address_to_hash160( addresses[ i ] ) );
      encoded += hash160.substr( 0, 2 );
   }

   return hex_decode( encoded );
}

string construct_raw_transaction(
 const vector< utxo_information >& inputs,
 const vector< output_information >& outputs, bool* p_is_complete,
 bool randomly_order_outputs, const char* p_message, uint32_t lock_time,
 vector< string >* p_extra_sig_script_items )
{
   string raw_transaction( "01000000" ); // i.e. version

   unsigned char size = ( unsigned char )inputs.size( );
   raw_transaction += hex_encode( &size, sizeof( unsigned char ) );

   string signing_info_prefix( raw_transaction );

   vector< string > empty_input_sig_info;
   vector< string > signing_input_sig_info;

   bool has_p2sh_redeem = false;

   for( size_t i = 0; i < inputs.size( ); i++ )
   {
      string next_empty_sig_info, next_input_sig_info;

      ostringstream outs;
      outs << hex << setw( 8 ) << setfill( '0' ) << fix_endian( inputs[ i ].index );

      raw_transaction += inputs[ i ].reversed_txid + outs.str( );
      next_empty_sig_info += inputs[ i ].reversed_txid + outs.str( );
      next_input_sig_info += inputs[ i ].reversed_txid + outs.str( );

      next_empty_sig_info += c_empty_sig_script;

      if( !inputs[ i ].original_script.empty( ) )
      {
         // NOTE: This test is being repeated further down (in the tx signing section).
         if( inputs[ i ].original_script.length( ) / 2 > 0xff )
            throw runtime_error( "P2SH redeem script is too big to handle with PUSH_DATA1" );

         size = ( unsigned char )( inputs[ i ].original_script.length( ) / 2 );

         next_input_sig_info += hex_encode( &size, sizeof( unsigned char ) );

         next_input_sig_info += inputs[ i ].original_script;
      }
      else
         next_input_sig_info += c_empty_sig_script;

      raw_transaction += c_sig_script_marker;

      string sequence( inputs[ i ].is_p2sh_redeem ? c_zero_sequence_bytes : c_default_sequence_bytes );

      if( inputs[ i ].is_p2sh_redeem )
         has_p2sh_redeem = true;

      raw_transaction += sequence;
      next_empty_sig_info += sequence;
      next_input_sig_info += sequence;

      empty_input_sig_info.push_back( next_empty_sig_info );
      signing_input_sig_info.push_back( next_input_sig_info );
   }

   size = ( unsigned char )outputs.size( );

   size_t msg_len = p_message ? strlen( p_message ) : 0;

   if( p_message && msg_len > c_tx_max_msg_len )
      throw runtime_error( "max. message size is " + to_string( c_tx_max_msg_len ) + " bytes per transaction" );

   if( p_message && msg_len )
      ++size;

   raw_transaction += hex_encode( &size, sizeof( unsigned char ) );

   string signing_info_suffix( hex_encode( &size, sizeof( unsigned char ) ) );

   map< string, int > ordering;

   // NOTE: In order to make sure that the "change address" isn't always found at the end
   // the outputs will be processed in a random order unless it is specifically requested
   // that this should not occur.
   for( size_t i = 0; i < outputs.size( ); i++ )
   {
      if( randomly_order_outputs )
         ordering.insert( make_pair( uuid( ).as_string( ), i ) );
      else
      {
         string s( to_string( i ) );
         if( s.length( ) < 6 )
            s = string( 6 - s.length( ), '0' ) + s;

         ordering.insert( make_pair( s, i ) );
      }
   }

   for( map< string, int >::iterator i = ordering.begin( ); i != ordering.end( ); ++i )
   {
      ostringstream outs;
      outs << hex << setw( 16 ) << setfill( '0' ) << fix_endian( outputs[ i->second ].amount );

      raw_transaction += outs.str( );
      signing_info_suffix += outs.str( );

      bool is_p2sh = false;

      // NOTE: If the address prefix is found to be 2 or 3 then assume P2SH rather than P2PKH.
      if( !outputs[ i->second ].address.empty( )
       && ( outputs[ i->second ].address[ 0 ] == '2' || outputs[ i->second ].address[ 0 ] == '3' ) )
         is_p2sh = true;

      string script_pub_key;

      if( is_p2sh )
         script_pub_key = "a9"; // i.e. OP_HASH160
      else
         script_pub_key = "76a9"; // i.e. OP_DUP OP_HASH160

      string hash160( public_key::address_to_hash160( outputs[ i->second ].address ) );

      size = ( unsigned char )( hash160.size( ) / 2 );
      script_pub_key += hex_encode( &size, sizeof( unsigned char ) );

      script_pub_key += hash160;

      if( is_p2sh )
         script_pub_key += "87"; // i.e. OP_EQUAL
      else
         script_pub_key += "88ac"; // i.e. OP_EQUALVERIFY OP_CHECKSIG

      size = ( unsigned char )( script_pub_key.size( ) / 2 );

      raw_transaction += hex_encode( &size, sizeof( unsigned char ) );
      signing_info_suffix += hex_encode( &size, sizeof( unsigned char ) );

      raw_transaction += script_pub_key;
      signing_info_suffix += script_pub_key;
   }

   if( p_message && msg_len )
   {
      raw_transaction += c_zero_amount;
      signing_info_suffix += c_zero_amount;

      string script_return( "6a" ); // i.e. OP_RETURN

      size = msg_len;
      script_return += hex_encode( &size, sizeof( unsigned char ) );

      script_return += hex_encode( ( const unsigned char* )p_message, msg_len );

      size = ( unsigned char )( script_return.size( ) / 2 );

      raw_transaction += hex_encode( &size, sizeof( unsigned char ) );
      signing_info_suffix += hex_encode( &size, sizeof( unsigned char ) );

      raw_transaction += script_return;
      signing_info_suffix += script_return;
   }

   string nlocktime( hex_encode( ( const unsigned char* )&lock_time, sizeof( lock_time ) ) );

#ifndef LITTLE_ENDIAN
   nlocktime = hex_reverse( osstr.str( ) );
#endif

   raw_transaction += nlocktime;
   signing_info_suffix += nlocktime;

   signing_info_suffix += c_hash_code_type_all;

   for( size_t i = 0; i < inputs.size( ); i++ )
   {
      if( inputs[ i ].rp_private_key )
      {
         string signing_information( signing_info_prefix );

         // NOTE: For inputs other than the one being signed an empty
         // sig needs to be used.
         for( size_t j = 0; j < signing_input_sig_info.size( ); j++ )
         {
            if( j != i )
               signing_information += empty_input_sig_info[ j ];
            else
               signing_information += signing_input_sig_info[ j ];
         }

         signing_information += signing_info_suffix;

         vector< unsigned char > buffer( signing_information.size( ) / 2, 0 );
         hex_decode( signing_information, &buffer[ 0 ], buffer.size( ) );

         sha256 hash( &buffer[ 0 ], buffer.size( ) );

         unsigned char buf[ c_sha256_digest_size ];
         hash.copy_digest_to_buffer( buf );

         hash.update( buf, c_sha256_digest_size );
         hash.copy_digest_to_buffer( buf );

         string sig( inputs[ i ].rp_private_key->construct_signature( &buf[ 0 ] ) );

         size = ( unsigned char )( sig.size( ) / 2 ) + 1;
         string scriptSig( hex_encode( &size, sizeof( unsigned char ) ) );

         scriptSig += sig;

         scriptSig += "01"; // i.e. SIGHASH_ALL

         string pub( inputs[ i ].rp_private_key->get_public( ) );

         size = ( unsigned char )( pub.size( ) / 2 );
         scriptSig += hex_encode( &size, sizeof( unsigned char ) );

         scriptSig += pub;

         if( p_extra_sig_script_items && !p_extra_sig_script_items->empty( ) )
         {
            for( size_t i = 0; i < p_extra_sig_script_items->size( ); i++ )
            {
               string extra( ( *p_extra_sig_script_items )[ i ] );

               size = ( unsigned char )( extra.size( ) / 2 );
               scriptSig += hex_encode( &size, sizeof( unsigned char ) );

               scriptSig += extra;
            }
         }

         if( inputs[ i ].is_p2sh_redeem )
         {
            string p2sh_redeem_script( inputs[ i ].original_script );

            if( p2sh_redeem_script.length( ) / 2 > 0xff )
               throw runtime_error( "P2SH redeem script is too big to handle with PUSH_DATA1" );

            size = ( p2sh_redeem_script.length( ) / 2 );

            if( size > 0x4b )
               scriptSig += "4c"; // i.e. PUSH_DATA1

            scriptSig += hex_encode( &size, sizeof( unsigned char ) );

            scriptSig += p2sh_redeem_script;
         }

         string::size_type pos = raw_transaction.find( c_sig_script_marker );

         if( pos != string::npos )
         {
            raw_transaction.erase( pos, 2 );
            size = ( unsigned char )( scriptSig.size( ) / 2 );

            raw_transaction.insert( pos, hex_encode( &size, sizeof( unsigned char ) ) );
            raw_transaction.insert( pos + 2, scriptSig );
         }
      }
   }

   if( p_is_complete )
      *p_is_complete = raw_transaction.find( c_sig_script_marker ) == string::npos;

   return replaced( raw_transaction, c_sig_script_marker, c_empty_sig_script );
}
