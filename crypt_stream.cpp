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

   string key( MD5( ( unsigned char* )keyval.c_str( ) ).hex_digest( ) );

   crypt_stream( ss, key.c_str( ), key.length( ) );
   s = base64::encode( ss.str( ) );

   return s;
}

string password_decrypt( const string& password, const string& keyval )
{
   string s;
   stringstream ss( base64::decode( password ) );

   string key( MD5( ( unsigned char* )keyval.c_str( ) ).hex_digest( ) );

   crypt_stream( ss, key.c_str( ), key.length( ) );
   s = ss.str( );

   return s.c_str( ); // NOTE: Remove any trailing padding from encryption.
}

