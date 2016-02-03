// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2016 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <memory.h>
#  include <ctime>
#  include <cstdlib>
#  include <memory>
#  include <sstream>
#  include <iostream>
#  include <stdexcept>
#endif

#include "crypt_stream.h"

#include "md5.h"
#include "sha1.h"
#include "base32.h"
#include "base64.h"
#include "sha256.h"
#include "utilities.h"

#ifdef SSL_SUPPORT
#  include <openssl/aes.h>
#  include <openssl/ssl.h>
// KLUDGE: Suppress the "function now deprecated" warning as it is being incorrectly issued for
// the "sgetn" I/O function (an issue at least with the VS Express 2005 version of VC8).
#  ifdef _MSC_VER
#     pragma warning (disable: 4996)
#  endif
#endif

using namespace std;

namespace
{

const size_t c_max_key_size = 128;
const size_t c_file_buf_size = 32768;

const size_t c_work_buffer_size = c_sha256_digest_size * 5000000;

const size_t c_password_rounds_multiplier = 3;

}

// NOTE: This algorithm is an XOR approach for encrypting a stream in place
// and is very quick, however, it is not considered "strong encryption" and
// therefore should not be used for encrypting very sensitive information.
void crypt_stream( iostream& io, const char* p_key, size_t key_length )
{
   unsigned char key[ c_max_key_size ];
   unsigned char buf[ c_file_buf_size ];

   io.seekg( 0, ios::end );
   size_t length = ( size_t )io.tellg( );

   io.seekg( 0, ios::beg );

   memcpy( key, p_key, key_length );

   unsigned char datkey = 0;
   for( size_t i = 0; i < key_length; i++ )
      datkey += key[ i ];

   size_t buflen = c_file_buf_size;
   for( size_t pos = 0; pos < length; pos += buflen )
   {
      if( length - pos < c_file_buf_size )
         buflen = length - pos;

      int dir = 1;
      int key_offset = 0;

      io.seekg( pos, ios::beg );
      io.read( ( char* )buf, buflen );

      unsigned char ch;
      for( size_t offset = 0; offset < buflen; offset++ )
      {
         ch = buf[ offset ];
         ch ^= datkey;
         buf[ offset ] = ch;

         datkey += ( key[ key_offset ] % 131 );
         key[ key_offset ] ^= datkey;

         key_offset += dir;

         if( datkey % 31 == 0 )
            dir *= -1;

         if( key_offset >= ( int )key_length )
            key_offset = 0;

         if( key_offset < 0 )
            key_offset = ( int )key_length - 1;
      }

      io.seekg( pos, ios::beg );
      io.write( ( char* )buf, buflen );
   }

   memset( buf, '\0', c_file_buf_size );
   memset( key, '\0', c_max_key_size );
}

#ifdef SSL_SUPPORT
string aes_crypt( const string& s, const char* p_key, size_t key_length, crypt_op op, bool use_256 )
{
   string output( s.length( ) + AES_BLOCK_SIZE, '\0' );
   unsigned char* p_output = ( unsigned char* )output.data( );

   unsigned char* p_ckey;
   unsigned char* p_ivec;

   unsigned char buffer[ 64 ];
   auto_ptr< char > ap_digest;

   if( !use_256 )
   {
      // NOTE: Use an MD5 hash of the key as ckey and ivec must be 16 bytes each.
      ap_digest.reset( MD5( ( unsigned char* )p_key ).hex_digest( ) );

      p_ckey = ( unsigned char* )ap_digest.get( );
      p_ivec = ( unsigned char* )ap_digest.get( ) + 16;
   }
   else
   {
      sha256 hash( p_key );
      hash.copy_digest_to_buffer( buffer );

      hash.update( buffer, 32 );
      hash.copy_digest_to_buffer( buffer + 32 );

      p_ckey = buffer;
      p_ivec = buffer + 32;
   }

   EVP_CIPHER_CTX ctx;
   EVP_CIPHER_CTX_init( &ctx );

   if( op == e_crypt_op_encrypt )
      EVP_EncryptInit_ex( &ctx, use_256 ? EVP_aes_256_cbc( ) : EVP_aes_128_cbc( ), 0, p_ckey, p_ivec );
   else
      EVP_DecryptInit_ex( &ctx, use_256 ? EVP_aes_256_cbc( ) : EVP_aes_128_cbc( ), 0, p_ckey, p_ivec );

   int num = 0;
   if( op == e_crypt_op_encrypt )
      EVP_EncryptUpdate( &ctx, p_output, &num, ( const unsigned char* )s.data( ), s.size( ) );
   else
      EVP_DecryptUpdate( &ctx, p_output, &num, ( const unsigned char* )s.data( ), s.size( ) );

   int tlen = 0;
   if( op == e_crypt_op_encrypt )
      EVP_EncryptFinal_ex( &ctx, p_output + num, &tlen );
   else
      EVP_DecryptFinal_ex( &ctx, p_output + num, &tlen );

   EVP_CIPHER_CTX_cleanup( &ctx );

   output.resize( num + tlen );

   return output;
}
#endif

// NOTE: This TOTP algorithm is in accordance with RFC 6238.
string get_totp( const string& base32_encoded_secret, int freq )
{
   string totp, secret = base32::decode( base32_encoded_secret );

   uint64_t tm = time( 0 ) / freq;

   uint8_t challenge[ 8 ];
   for( int i = 8; i--; tm >>= 8 )
      challenge[ i ] = tm;

   string message( ( const char* )&challenge[ 0 ], 8 );

   uint8_t hash[ 20 ];
   hmac_sha1( secret, message, hash );

   int offset = hash[ 19 ] & 0xf;
   unsigned int truncatedHash = 0;
   for( int i = 0; i < 4; ++i )
   {
      truncatedHash <<= 8;
      truncatedHash |= hash[ offset + i ];
   }

   truncatedHash &= 0x7fffffff;
   truncatedHash %= 1000000;

   totp = to_string( truncatedHash );
   while( totp.length( ) < 6 )
      totp = '0' + totp;

   return totp;
}

string get_totp_secret( const string& user_unique, const string& system_unique )
{
   return base32::encode( sha256( user_unique + system_unique ).get_digest_as_string( ).substr( 0, 20 ) );
}

string data_decrypt( const string& dat, const string& key, bool use_ssl )
{
   string s;
   if( dat.empty( ) )
      return s;

   string salt;
   string::size_type pos = dat.find( ':' );

   if( pos == string::npos )
      pos = 0;
   else
      salt = dat.substr( 0, ++pos );

   // NOTE: For compatability with older 128 bit AES encrypted
   // passwords the 256 bit ones are prefixed with an asterisk.
   bool use_256 = false;
   if( dat[ pos ] == '*' )
   {
      ++pos;
      use_256 = true;
   }
   else
   {
      if( !salt.empty( ) && salt[ 0 ] == '*' )
      {
         use_256 = true;
         salt.erase( 0, 1 );
      }
   }

   stringstream ss( base64::decode( dat.substr( pos ) ) );

#ifndef SSL_SUPPORT
   use_ssl = false;
#endif

   string salted_key( key + salt );

   if( !salt.empty( ) )
   {
      sha256 hash;
      for( size_t i = 0; i < c_password_hash_rounds * c_password_rounds_multiplier; i++ )
      {
         hash.update( salted_key + key );
         hash.get_digest_as_string( salted_key );
      }
   }
   
   auto_ptr< char > ap_digest( MD5( ( unsigned char* )salted_key.c_str( ) ).hex_digest( ) );

   if( !use_ssl )
   {
      crypt_stream( ss, ap_digest.get( ), 32 );
      s = ss.str( );
   }

#ifdef SSL_SUPPORT
   if( use_ssl )
      s = aes_crypt( ss.str( ), ap_digest.get( ), 32, e_crypt_op_decrypt, use_256 );
#endif

   return s.c_str( ); // NOTE: Remove any trailing padding from encryption.
}

string data_encrypt( const string& dat, const string& key, bool use_ssl, bool add_salt )
{
   string s( dat );
   if( dat.empty( ) )
      return s;

#ifndef SSL_SUPPORT
   use_ssl = false;
#endif

   // NOTE: If the password is less than 20 characters then append a '\0' followed
   // by as many random characters as needed so that the length of the password is
   // disguised from non-technical user observation.
   if( !use_ssl )
   {
      char c( '\0' );
      while( s.length( ) < 20 )
      {
         s += c;
         c = rand( ) % 256;
      }
   }

   stringstream ss( s );
   ss.seekp( 0 );

   string salt;
   if( add_salt )
      salt += uuid( ).as_string( ) + ':';

   string salted_key( key + salt );

   if( add_salt )
   {
      sha256 hash;
      for( size_t i = 0; i < c_password_hash_rounds * c_password_rounds_multiplier; i++ )
      {
         hash.update( salted_key + key );
         hash.get_digest_as_string( salted_key );
      }
   }

   auto_ptr< char > ap_digest( MD5( ( unsigned char* )salted_key.c_str( ) ).hex_digest( ) );

   if( !use_ssl )
   {
      crypt_stream( ss, ap_digest.get( ), 32 );
      s = salt + base64::encode( ss.str( ) );
   }

#ifdef SSL_SUPPORT
   if( use_ssl )
      s = '*' + salt + base64::encode( aes_crypt( ss.str( ), ap_digest.get( ), 32, e_crypt_op_encrypt ) );
#endif

   return s;
}

string harden_key_with_salt( const string& key, const string& salt )
{
   sha256 hash;
   string salted_key( key + salt );

   for( size_t i = 0; i < c_password_hash_rounds * c_password_rounds_multiplier; i++ )
   {
      hash.update( salted_key + key );
      hash.get_digest_as_string( salted_key );
   }

   return salted_key;
}

string check_for_proof_of_work(
 const string& data, size_t start, size_t range, size_t num_leading_zeroes )
{
   unsigned char hash_buffer[ c_sha256_digest_size ];

   if( range == 0 )
      throw runtime_error( "invalid range 0 for 'check_for_proof_of_work'" );

   auto_ptr< unsigned char > ap_buffer( new unsigned char[ c_work_buffer_size ] );

   bool okay = true;
   size_t nonce = 0;
   string hash_string;

   for( size_t i = 0; i < range; i++ )
   {
      sha256 hash( data );
      hash.copy_digest_to_buffer( hash_buffer );

      size_t offset = 0;
      size_t num_bytes = 0;

      nonce = start + i;

      if( nonce < 2 )
         throw runtime_error( "invalid nonce < 2 for 'check_for_proof_of_work'" );

      // NOTE: The purpose of this algorithm is to transform during copying such that
      // it shouldn't be possible to do the hashing without using the memory for this
      // transforming (if this algorithm can be implemented without requiring all the
      // memory to be allocated then it will need to be reworked).
      while( num_bytes < c_work_buffer_size )
      {
         memcpy( ap_buffer.get( ) + num_bytes, hash_buffer, c_sha256_digest_size );

         if( num_bytes && num_bytes % nonce == 0 )
         {
            if( ++offset >= c_sha256_digest_size )
               offset = 0;

            hash_buffer[ offset ] ^= 0xaa;
         }

         num_bytes += c_sha256_digest_size;
      }

      reverse( ap_buffer.get( ), ap_buffer.get( ) + c_work_buffer_size );

      sha256 buf_hash( ap_buffer.get( ), c_work_buffer_size );

      // NOTE: Just in case the nonce was bigger than the buffer itself (in which case
      // the hash buffer transformation above would not have occurred) perform another
      // round with the nonce value (as a string).
      buf_hash.update( to_string( nonce ) );

      okay = true;

      hash_string = buf_hash.get_digest_as_string( );

      for( size_t j = 0; j < num_leading_zeroes; j++ )
      {
         if( hash_string[ j ] != '0' )
         {
            okay = false;
            break;
         }
      }

      if( okay )
         break;
   }

   if( range == 1 )
      return okay ? hash_string : string( );
   else
      return okay ? to_string( nonce ) : string( );
}

