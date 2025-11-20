// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
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

#include "sha1.h"
#include "base32.h"
#include "base64.h"
#include "sha256.h"
#include "sha512.h"
#include "chacha20.h"
#include "utilities.h"

#ifdef SSL_SUPPORT
#  include <openssl/aes.h>
#  include <openssl/ssl.h>
#endif

using namespace std;

namespace
{

const size_t c_max_key_size = 256;
const size_t c_file_buf_size = 32768;

const size_t c_work_buffer_size = 0x8000000;
const size_t c_work_buffer_pos_mask = 0x7ffff00;

const size_t c_password_rounds_multiplier = 3;

const char* const c_stream_cipher_chacha20 = "cc";
const char* const c_stream_cipher_dbl_hash = "dh";

}

stream_cipher stream_cipher_value( const string& str )
{
   stream_cipher cipher = e_stream_cipher_unknown;

   if( !str.empty( ) )
   {
      if( str == c_stream_cipher_chacha20 )
         cipher = e_stream_cipher_chacha20;
      else if( str == c_stream_cipher_dbl_hash )
         cipher = e_stream_cipher_dbl_hash;
      else
         throw runtime_error( "unknown stream cipher '" + str + "'" );
   }

   return cipher;
}

string stream_cipher_string( stream_cipher cipher )
{
   string retval;

   switch( cipher )
   {
      case e_stream_cipher_chacha20:
      retval = c_stream_cipher_chacha20;
      break;

      case e_stream_cipher_dbl_hash:
      retval = c_stream_cipher_dbl_hash;
      break;

      default:
      throw runtime_error( "unknown stream cipher #" + to_string( cipher ) );
   }

   return retval;
}

// NOTE: Uses the "chacha20" stream cipher (with zero values for both the
// nonce and starting counter).
//
// CHACHA20: RFC 7539 https://www.rfc-editor.org/rfc/rfc7539
void cc_crypt_stream( iostream& io, const char* p_key, size_t key_length )
{
   unsigned char buf[ c_file_buf_size ];
   unsigned char key_buf[ c_sha256_digest_size ];

   sha256 hash( ( const unsigned char* )p_key, key_length );

   hash.copy_digest_to_buffer( key_buf );

   chacha20_ctx ctx;
   memset( &ctx, '\0', sizeof( ctx ) );

   // NOTE: Nonce is always been set to zero.
   chacha20_init_ctx( &ctx, key_buf, ctx.nonce, 0 );

   io.seekg( 0, ios::end );

   size_t length = ( size_t )io.tellg( );

   io.seekg( 0, ios::beg );

   size_t buflen = c_file_buf_size;

   for( size_t pos = 0; pos < length; pos += buflen )
   {
      if( length - pos < c_file_buf_size )
         buflen = length - pos;

      io.seekg( pos, ios::beg );
      io.read( ( char* )buf, buflen );

      chacha20_xor_next( &ctx, buf, buflen );

      io.seekg( pos, ios::beg );
      io.write( ( char* )buf, buflen );
   }

   memset( &ctx, '\0', sizeof( ctx ) );

   memset( buf, '\0', c_file_buf_size );
   memset( key_buf, '\0', sizeof( key_buf ) );
}

// NOTE: This cipher uses two SHA512 hash values of the provided key where
// each is being prefixed differently to produce two pseudo random digests
// that are then each updated using the current chunk number. Although not
// actually a stream cipher this could be easily modified to specify which
// chunk to start from in to support a starting offset.
void dh_crypt_stream( iostream& io, const char* p_key, size_t key_length )
{
   unsigned char buf[ c_file_buf_size ];

   unsigned char buf1[ c_sha512_digest_size ];
   unsigned char buf2[ c_sha512_digest_size ];

   io.seekg( 0, ios::end );

   size_t length = ( size_t )io.tellg( );

   io.seekg( 0, ios::beg );

   size_t chunk = 0;

   string key1( key_length + 1, '\0' );
   string key2( key_length + 1, '\0' );

   // NOTE: Prefix the key with two different characters then
   // will hash and update each with the current chunk number
   // so that an XOR pad is the combination of these two hash
   // digest streams.
   key1[ 0 ] = 'A';
   key2[ 0 ] = 'B';

   memcpy( &key1[ 1 ], p_key, key_length );
   memcpy( &key2[ 1 ], p_key, key_length );

   sha512 hash1( key1 );
   sha512 hash2( key2 );

   size_t buflen = c_file_buf_size;

   for( size_t pos = 0; pos < length; pos += buflen )
   {
      if( length - pos < c_file_buf_size )
         buflen = length - pos;

      io.seekg( pos, ios::beg );
      io.read( ( char* )buf, buflen );

      unsigned char ch;

      for( size_t offset = 0; offset < buflen; offset++ )
      {
         ch = buf[ offset ];

         if( ( offset % c_sha512_digest_size ) == 0 )
         {
            sha512 next_hash1;
            sha512 next_hash2;

            memcpy( &next_hash1, &hash1, sizeof( sha512 ) );
            memcpy( &next_hash2, &hash2, sizeof( sha512 ) );

            next_hash1.update( to_string( chunk ) );
            next_hash2.update( to_string( chunk++ ) );

            next_hash1.copy_digest_to_buffer( buf1 );
            next_hash2.copy_digest_to_buffer( buf2 );
         }

         ch ^= buf1[ offset % c_sha512_digest_size ];
         ch ^= buf2[ offset % c_sha512_digest_size ];

         buf[ offset ] = ch;
      }

      io.seekg( pos, ios::beg );
      io.write( ( char* )buf, buflen );
   }

   clear_key( key1 );
   clear_key( key2 );

   memset( buf, '\0', c_file_buf_size );

   memset( buf1, '\0', c_sha512_digest_size );
   memset( buf2, '\0', c_sha512_digest_size );
}

#ifdef SSL_SUPPORT
void aes_crypt( string& o, const string& s, const char* p_key, size_t key_length, crypt_op op )
{
   o.resize( s.length( ) + AES_BLOCK_SIZE );

   unsigned char* p_output = ( unsigned char* )o.data( );

   unsigned char* p_ckey;
   unsigned char* p_ivec;

   unsigned char buffer[ 64 ];

   // NOTE: Use SHA256 hashes of the key as ckey and ivec must be 32 bytes each.
   sha256 hash( p_key );
   hash.copy_digest_to_buffer( buffer );

   hash.update( buffer, 32 );
   hash.copy_digest_to_buffer( buffer + 32 );

   p_ckey = buffer;
   p_ivec = buffer + 32;

   EVP_CIPHER_CTX* p_ctx = EVP_CIPHER_CTX_new( );

   EVP_CIPHER_CTX_init( p_ctx );

   if( op == e_crypt_op_encrypt )
      EVP_EncryptInit_ex( p_ctx, EVP_aes_256_cbc( ), 0, p_ckey, p_ivec );
   else
      EVP_DecryptInit_ex( p_ctx, EVP_aes_256_cbc( ), 0, p_ckey, p_ivec );

   int num = 0;

   size_t input_offset = 0;
   size_t output_offset = 0;
   size_t remaining = s.size( );

   while( remaining )
   {
      int next = min( ( size_t )128, remaining );

      if( op == e_crypt_op_encrypt )
         EVP_EncryptUpdate( p_ctx, p_output + output_offset,
          &num, ( const unsigned char* )s.data( ) + input_offset, next );
      else
         EVP_DecryptUpdate( p_ctx, p_output + output_offset,
          &num, ( const unsigned char* )s.data( ) + input_offset, next );

      input_offset += next;
      output_offset += num;

      if( remaining <= 128 )
         break;

      remaining -= 128;
   }

   int tlen = 0;

   if( op == e_crypt_op_encrypt )
      EVP_EncryptFinal_ex( p_ctx, p_output + output_offset, &tlen );
   else
      EVP_DecryptFinal_ex( p_ctx, p_output + output_offset, &tlen );

   EVP_CIPHER_CTX_cleanup( p_ctx );

   EVP_CIPHER_CTX_free( p_ctx );

   memset( buffer, '\0', sizeof( buffer ) );

   o.resize( output_offset + tlen );
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

   unsigned int truncated_hash = 0;

   for( int i = 0; i < 4; ++i )
   {
      truncated_hash <<= 8;
      truncated_hash |= hash[ offset + i ];
   }

   truncated_hash &= 0x7fffffff;
   truncated_hash %= 1000000;

   totp = to_string( truncated_hash );

   while( totp.length( ) < 6 )
      totp = '0' + totp;

   return totp;
}

string get_totp_secret( const string& user_unique, const string& system_unique )
{
   return base32::encode( sha256( user_unique + system_unique ).get_digest_as_string( ).substr( 0, 20 ) );
}

void data_decrypt( string& s, const string& dat, const string& key )
{
   if( dat.empty( ) )
      return;

   string salt;

#ifdef SSL_SUPPORT
   bool use_ssl = true;
#else
   bool use_ssl = false;
#endif

   string::size_type pos = dat.find( ':' );

   if( pos == string::npos )
   {
      pos = 0;

      if( dat[ 0 ] == '#' )
         ++pos;
      else
         use_ssl = false;
   }
   else
   {
      salt = dat.substr( 0, ++pos );

      if( salt[ 0 ] == '#' )
      {
         salt.erase( 0, 1 );

         if( !use_ssl )
            throw runtime_error( "unable to decrypt SSL encrypted password without SSL support" );
      }
      else
         use_ssl = false;
   }

   stringstream ss( base64::decode( dat.substr( pos ) ) );
   ss.seekp( 0 );

   string salted_key;

   salted_key.reserve( ( c_sha256_digest_size * 2 ) + key.length( ) );

   salted_key += key;
   salted_key += salt;

   if( !salt.empty( ) )
   {
      sha256 hash;

      for( size_t i = 0; i < c_password_hash_rounds * c_password_rounds_multiplier; i++ )
      {
         string tmp;

         tmp.reserve( salted_key.length( ) + key.length( ) );

         tmp += salted_key;
         tmp += key;

         hash.update( tmp );

         hash.get_digest_as_string( salted_key );

         clear_key( tmp );
      }
   }

   if( !use_ssl )
   {
      crypt_stream( ss, salted_key.c_str( ), salted_key.length( ) );

      s = ss.str( );
   }

#ifdef SSL_SUPPORT
   if( use_ssl )
      aes_crypt( s, ss.str( ), salted_key.c_str( ), salted_key.length( ), e_crypt_op_decrypt );
#endif

   clear_key( salted_key );

   // NOTE: Remove any trailing padding from encryption.
   pos = s.find( '\0' );

   if( pos != string::npos )
      s.resize( pos );
}

void data_encrypt( string& s, const string& dat, const string& key, bool use_ssl, bool add_salt )
{
   s = dat;

   if( dat.empty( ) )
      return;

#ifndef SSL_SUPPORT
   use_ssl = false;
#endif

   // NOTE: If the password is less than 20 characters then append a '\0' followed
   // by as many random characters as needed so that the length of the password is
   // disguised from casual observation.
   if( !use_ssl )
   {
      char c( '\0' );

      while( s.length( ) < 20 )
      {
         s += c;
         c = rand( ) % 256;
      }
   }

   size_t len = s.length( );

   stringstream ss( s );

   ss.seekp( 0 );

   string salt;

   if( add_salt )
      salt += uuid( ).as_string( ) + ':';

   string salted_key;

   salted_key.reserve( ( c_sha256_digest_size * 2 ) + key.length( ) );

   salted_key += key;
   salted_key += salt;

   if( add_salt )
   {
      sha256 hash;

      for( size_t i = 0; i < c_password_hash_rounds * c_password_rounds_multiplier; i++ )
      {
         string tmp;

         tmp.reserve( salted_key.length( ) + key.length( ) );

         tmp += salted_key;
         tmp += key;

         hash.update( tmp );

         hash.get_digest_as_string( salted_key );

         clear_key( tmp );
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
      string tmp( len, '\0' );

      aes_crypt( tmp, ss.str( ), salted_key.c_str( ), salted_key.length( ), e_crypt_op_encrypt );

      s = '#' + salt + base64::encode( tmp );

      clear_key( tmp );
   }
#endif

   ss.seekp( 0 );
   ss << string( len, '\0' );

   clear_key( salted_key );
}

void harden_key_with_hash_rounds( string& s, const string& key, const string& extra, size_t extra_multiplier )
{
   sha256 hash( key );

   s.reserve( c_sha256_digest_size * 2 );

   hash.update( extra );

   hash.get_digest_as_string( s );

   size_t num_rounds = ( c_password_hash_rounds * c_password_rounds_multiplier ) * extra_multiplier;

   unsigned char offset = 0;

   for( size_t i = 0; i < num_rounds; i++ )
   {
      // NOTE: Cycles through inverting one byte
      // at a time (changing the input but doing
      // so with minimal CPU effort).
      s[ offset ] ^= 0xaa;

      hash.update( s );

      if( ++offset >= 64 )
         offset = 0;
   }

   hash.get_digest_as_string( s );
}

string check_for_proof_of_work(
 const string& data, uint32_t start, uint32_t range, nonce_difficulty difficulty, bool pause_between_passes )
{
   unsigned char hash_buffer[ c_sha256_digest_size ];
   unsigned char orig_buffer[ c_sha256_digest_size ];

   if( range == 0 )
      throw runtime_error( "invalid range 0 for 'check_for_proof_of_work'" );

   unique_ptr< unsigned char > up_buffer( new unsigned char[ c_work_buffer_size ] );

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
      unsigned char* p_start = up_buffer.get( );

      unsigned char* p_next = p_start;
      uint8_t wrap = c_sha256_digest_size - 1;

      temp = 0;

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
         {
            temp += ch;
            temp *= ch;

            hash_buffer[ j ] ^= ( ch + j );
         }

         // NOTE: Effectively choose a random byte within the total buffer range
         // to do a bit flip on (so random access to the entire memory range has
         // to be provided during this entire loop).
         *( p_start + ( temp & c_work_buffer_pos_mask ) ) ^= 0xaa;

         p_next += c_sha256_digest_size;
         num_bytes += c_sha256_digest_size;
      }

      // NOTE: The content is reversed prior to hashing to ensure that the entire
      // pass has to have been completed before any hashing can commence. Another
      // approach would be to change the SHA256 code to be able to operate itself
      // in reverse but tests showed that the reversing time is not significant.
      reverse( up_buffer.get( ), up_buffer.get( ) + c_work_buffer_size );

      sha256 buf_hash( up_buffer.get( ), c_work_buffer_size );

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
