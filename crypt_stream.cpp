// Copyright (c) 2010
//
// CIYAM Pty. Ltd.
// ACN 093 704 539
//
// ALL RIGHTS RESERVED
//
// Permission to use this software for non-commercial purposes is hereby granted. Permission to
// distribute this software privately is granted provided that the source code is unaltered and
// complete or that any alterations and omissions have been first approved by CIYAM. Commercial
// usage of this software is not permitted without first obtaining a license for such a purpose
// from CIYAM. This software may not be publicly distributed unless written permission to do so
// has been obtained from CIYAM.

#ifdef __BORLANDC__
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <ctime>
#  include <memory>
#  include <sstream>
#  include <iostream>
#endif

#include "crypt_stream.h"

#include "md5.h"
#include "base64.h"

#ifdef _WIN32
// KLUDGE: Although applink.c has been included and CRYPTO_malloc_init is called as per the
// OpenSSL FAQ calls to crypt_stream (sometimes the first but sometimes the second) results
// in an access violation so for now only the home grown encryption algorithm is used under
// Windows.
#  undef SSL_SUPPORT
#endif

#ifdef SSL_SUPPORT
#  include <openssl/aes.h>
#  ifdef _WIN32
#     include <openssl/ssl.h>
#     include <openssl/applink.c>
#  endif
// KLUDGE: Suppress the "function now deprecated" warning as it is being incorrectly issued for
// the "sgetn" I/O function (an issue at least with the VS Express 2005 version of VC8).
#  ifdef _MSC_VER
#     pragma warning (disable: 4996)
#  endif
#endif

using namespace std;

const size_t c_max_key_size = 128;
const size_t c_file_buf_size = 32768;

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
void crypt_stream( iostream& io, const char* p_key, size_t key_length, crypt_op op )
{
#  ifdef _WIN32
   static bool initialised = false;

   if( !initialised )
   {
      initialised = true;
      CRYPTO_malloc_init( );
   }
#  endif

   int num, bytes_read, bytes_written;

   unsigned char indata[ AES_BLOCK_SIZE ];
   unsigned char outdata[ AES_BLOCK_SIZE ];

   auto_ptr< char > ap_digest( MD5( ( unsigned char* )p_key ).hex_digest( ) );

   unsigned char* p_ckey = ( unsigned char* )ap_digest.get( );
   unsigned char* p_ivec = ( unsigned char* )ap_digest.get( ) + 16;

   AES_KEY key;
   AES_set_encrypt_key( p_ckey, 128, &key );

   io.seekg( 0, ios::beg );

   while( true )
   {
      fstream::pos_type pos = io.tellg( );
      bytes_read = io.rdbuf( )->sgetn( ( char* )indata, AES_BLOCK_SIZE );

      AES_cfb128_encrypt( indata, outdata,
       bytes_read, &key, p_ivec, &num, op == e_crypt_op_decrypt ? AES_DECRYPT : AES_ENCRYPT );

      io.seekp( pos );
      bytes_written = io.rdbuf( )->sputn( ( char* )outdata, bytes_read );

      if( bytes_read < AES_BLOCK_SIZE )
         break;
   }
}
#endif

string password_encrypt( const string& password, const string& keyval )
{
   string s( password );

   srand( time( 0 ) );

   // NOTE: If the password is less than 20 characters then append a '\0' followed
   // by as many random characters as needed so that the length of the password is
   // disguised from non-technical user observation.
   char c( '\0' );
   while( s.length( ) < 20 )
   {
      s += c;
      c = rand( ) % 256;
   }

   stringstream ss( s );
   ss.seekp( 0 );

   auto_ptr< char > ap_digest( MD5( ( unsigned char* )keyval.c_str( ) ).hex_digest( ) );

#ifndef SSL_SUPPORT
   crypt_stream( ss, ap_digest.get( ), 32 );
#else
   crypt_stream( ss, ap_digest.get( ), 32, e_crypt_op_encrypt );
#endif
   s = base64::encode( ss.str( ) );

   return s;
}

string password_decrypt( const string& password, const string& keyval )
{
   string s;
   stringstream ss( base64::decode( password ) );

   auto_ptr< char > ap_digest( MD5( ( unsigned char* )keyval.c_str( ) ).hex_digest( ) );

#ifndef SSL_SUPPORT
   crypt_stream( ss, ap_digest.get( ), 32 );
#else
   crypt_stream( ss, ap_digest.get( ), 32, e_crypt_op_decrypt );
#endif
   s = ss.str( );

   return s.c_str( ); // NOTE: Remove any trailing padding from encryption.
}

