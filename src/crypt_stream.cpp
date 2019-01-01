// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2019 CIYAM Developers
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

const size_t c_max_key_size = 256;
const size_t c_file_buf_size = 32768;

const size_t c_work_buffer_size = 0x8000000;
const size_t c_work_buffer_pos_mask = 0x7ffff00;

const size_t c_password_rounds_multiplier = 3;

}

// NOTE: This algorithm is an XOR approach for encrypting a stream in place
// and is very quick. It prevents the key used from being easily discovered
// by constantly modifying a copy of it and by doing this in a manner which
// also prevents its length being discovered through frequency analysis. It
// should be noted that the strength of this cypher depends upon the length
// of the key that is used.
void crypt_stream( iostream& io, const char* p_key, size_t key_length )
{
   unsigned char key[ c_max_key_size ];
   unsigned char buf[ c_file_buf_size ];

   memset( key, '\0', c_max_key_size );

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
      // NOTE: Use SHA256 hashes of the key as ckey and ivec must be 32 bytes each.
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

   size_t input_offset = 0;
   size_t output_offset = 0;
   size_t remaining = s.size( );

   while( remaining )
   {
      int next = min( ( size_t )128, remaining );

      if( op == e_crypt_op_encrypt )
         EVP_EncryptUpdate( &ctx, p_output + output_offset,
          &num, ( const unsigned char* )s.data( ) + input_offset, next );
      else
         EVP_DecryptUpdate( &ctx, p_output + output_offset,
          &num, ( const unsigned char* )s.data( ) + input_offset, next );

      input_offset += next;
      output_offset += num;

      if( remaining <= 128 )
         break;

      remaining -= 128;
   }

   int tlen = 0;
   if( op == e_crypt_op_encrypt )
      EVP_EncryptFinal_ex( &ctx, p_output + output_offset, &tlen );
   else
      EVP_DecryptFinal_ex( &ctx, p_output + output_offset, &tlen );

   EVP_CIPHER_CTX_cleanup( &ctx );

   output.resize( output_offset + tlen );

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

   bool use_MD5 = false;

   // NOTE: For compatability with older 128 bit AES encrypted
   // passwords the 256 bit ones are prefixed with an asterisk
   // or a hash (the asterisk prefixed ones use an MD5 hash of
   // the key rather than the key itself and so are considered
   // to be less secure).
   bool use_256 = false;
   if( dat[ pos ] == '*' || dat[ pos ] == '#' )
   {
      ++pos;
      use_256 = true;

      if( dat[ pos ] == '*' )
         use_MD5 = true;
   }
   else
   {
      if( !salt.empty( ) && ( salt[ 0 ] == '*' || salt[ 0 ] == '#' ) )
      {
         use_256 = true;

         if( salt[ 0 ] == '*' )
            use_MD5 = true;

         salt.erase( 0, 1 );
      }
   }

   // NOTE: Legacy encryption is less secure (but assuming that
   // existing encrypted information has been re-encrypted then
   // this should not be an issue).
   if( !use_256 )
      use_MD5 = true;

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

   auto_ptr< char > ap_digest;

   if( use_MD5 )
      ap_digest.reset( MD5( ( unsigned char* )salted_key.c_str( ) ).hex_digest( ) );

   if( !use_ssl )
   {
      if( use_MD5 )
         crypt_stream( ss, ap_digest.get( ), 32 );
      else
         crypt_stream( ss, salted_key.c_str( ), salted_key.length( ) );

      s = ss.str( );
   }

#ifdef SSL_SUPPORT
   if( use_ssl )
   {
      if( use_MD5 )
         s = aes_crypt( ss.str( ), ap_digest.get( ), 32, e_crypt_op_decrypt, use_256 );
      else
         s = aes_crypt( ss.str( ), salted_key.c_str( ), salted_key.length( ), e_crypt_op_decrypt, use_256 );
   }
#endif

   // NOTE: Overwrite each byte of the copied key to protect from potential swap file discovery.
   for( size_t i = 0; i < salted_key.length( ); i++ )
      salted_key[ i ] = '\0';

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

   if( !use_ssl )
   {
      crypt_stream( ss, salted_key.c_str( ), salted_key.length( ) );
      s = salt + base64::encode( ss.str( ) );
   }

#ifdef SSL_SUPPORT
   if( use_ssl )
   {
      s = '#' + salt + base64::encode(
       aes_crypt( ss.str( ), salted_key.c_str( ), salted_key.length( ), e_crypt_op_encrypt ) );
   }
#endif

   // NOTE: Overwrite each byte of the copied key to protect from potential swap file discovery.
   for( size_t i = 0; i < salted_key.length( ); i++ )
      salted_key[ i ] = '\0';

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
 const string& data, uint32_t start, uint32_t range, nonce_difficulty difficulty, bool pause_between_passes )
{
   unsigned char hash_buffer[ c_sha256_digest_size ];
   unsigned char orig_buffer[ c_sha256_digest_size ];

   if( range == 0 )
      throw runtime_error( "invalid range 0 for 'check_for_proof_of_work'" );

   auto_ptr< unsigned char > ap_buffer( new unsigned char[ c_work_buffer_size ] );

   bool okay = true;
   uint32_t nonce = 0;
   string hash_string;

   sha256 hash( data );
   hash.copy_digest_to_buffer( hash_buffer );

   memcpy( orig_buffer, hash_buffer, c_sha256_digest_size );

   for( uint32_t i = 0; i < range; i++ )
   {
      if( i != 0 )
         memcpy( hash_buffer, orig_buffer, c_sha256_digest_size );

      uint8_t offset = 0;
      uint32_t num_bytes = 0;

      uint32_t temp = nonce = start + i;

      for( uint8_t j = 0; j < c_sha256_digest_size; j++ )
      {
         hash_buffer[ j ] ^= ( unsigned char )( temp );
         temp >>= 1;
      }

      unsigned char ch = '\0';
      unsigned char* p_start = ap_buffer.get( );

      unsigned char* p_next = p_start;
      uint8_t wrap = c_sha256_digest_size - 1;

      // NOTE: The purpose of this algorithm is to transform during copying such that
      // it shouldn't be possible to do the hashing without using the memory for this
      // transforming (if this algorithm can be implemented without requiring all the
      // memory to be allocated then it will need to be reworked).
      while( num_bytes < c_work_buffer_size )
      {
         memcpy( p_next, hash_buffer, c_sha256_digest_size );

         if( ++offset >= wrap )
            offset = 0;

         ch += hash_buffer[ offset ];

         for( size_t j = 0; j < c_sha256_digest_size; j++ )
            hash_buffer[ j ] ^= ( ch + j );

         // NOTE: Effectively choose a random byte within the total buffer range
         // to do a bit flip on (so random access to the entire memory range has
         // to be provided during this entire loop).
         *( p_start + ( *p_next & c_work_buffer_pos_mask ) ) ^= 0xaa;

         p_next += c_sha256_digest_size;
         num_bytes += c_sha256_digest_size;
      }

      // NOTE: The content is reversed prior to hashing to ensure that the entire
      // pass has to have been completed before any hashing can commence. Another
      // approach would be to change the SHA256 code to be able to operate itself
      // in reverse but tests showed that the reversing time is not significant.
      reverse( ap_buffer.get( ), ap_buffer.get( ) + c_work_buffer_size );

      sha256 buf_hash( ap_buffer.get( ), c_work_buffer_size );

      okay = true;
      hash_string = buf_hash.get_digest_as_string( );

      if( difficulty > e_nonce_difficulty_none && hash_string[ 0 ] != '0' )
         okay = false;

      if( difficulty > e_nonce_difficulty_easy && hash_string[ 1 ] != '0' )
         okay = false;

      if( difficulty > e_nonce_difficulty_hard && hash_string[ 2 ] != '0' )
         okay = false;

      if( okay )
         break;

      // NOTE: Take a short break after each pass to let any other threads process.
      if( pause_between_passes )
         msleep( 250 );
   }

   if( range == 1 )
      return okay ? hash_string : string( );
   else
      return okay ? to_string( nonce ) : string( );
}

