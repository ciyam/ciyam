// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstdlib>
#  include <string>
#  include <fstream>
#  include <sstream>
#  include <iostream>
#  include <stdexcept>
#  include <time.h>
#  include <utime.h>
#  include <sys/stat.h>
#endif

#include "md5.h"
#include "console.h"
#include "utilities.h"
#include "crypt_stream.h"

using namespace std;

const int c_buffer_size = 1024;

const size_t c_num_hrounds = 1000000;

const char* const c_title = "crypt v0.1m";

void create_checksum_test_file( fstream& fs, const string& test_file_name )
{
   fs.seekg( 0, ios::end );
   fstream::pos_type size = fs.tellg( );
   fs.seekg( 0, ios::beg );

   MD5 md5;
   unsigned char buffer[ c_buffer_size ];

   int size_left = ( int )size;

   while( size_left )
   {
      int next_len( min( size_left, c_buffer_size ) );

      fs.read( ( char* )buffer, next_len );

      int len = fs.gcount( );
      md5.update( buffer, len );

      size_left -= next_len;
   }

   md5.finalize( );

   fs.seekg( 0, ios::beg );

   stringstream ss;
   ss << md5.hex_digest( );

   ofstream outf( test_file_name.c_str( ), ios::out | ios::binary );
   if( !outf )
      throw runtime_error( "Unable to open file '" + test_file_name + "' for output." );

   outf << ss.str( );
   outf.flush( );
   if( !outf.good( ) )
      throw runtime_error( "Unexpected error occurred whilst writing to file '" + test_file_name + "'." );
}

int main( int argc, char* argv[ ] )
{
   if( argc < 2
    || string( argv[ 1 ] ) == "?" || string( argv[ 1 ] ) == "-?" || string( argv[ 1 ] ) == "/?" )
   {
      cout << c_title << "\nUsage: crypt [-q] [-c] [-h] [-cc|-dh] [-p[s]] [-t] <file> [<program>]" << endl;
      return 0;
   }

   int rc = 0;

   int first_arg = 1;
   int hash_count = 0;

   bool is_quiet = false;
   bool use_confirm = false;
   bool use_hrounds = false;
   bool use_chacha20 = false;
   bool use_dbl_hash = false;
   bool use_pin_hash = false;
   bool use_test_file = false;
   bool use_pin_as_salt = false;
   bool create_test_file = false;

   while( first_arg < argc )
   {
      if( argv[ first_arg ][ 0 ] != '-' )
         break;

      if( string( argv[ first_arg ] ) == string( "-q" ) )
      {
         ++first_arg;
         is_quiet = true;
      }
      else if( string( argv[ first_arg ] ) == string( "-c" ) )
      {
         ++first_arg;
         use_confirm = true;
      }
      else if( string( argv[ first_arg ] ) == string( "-h" ) )
      {
         ++first_arg;
         use_hrounds = true;
      }
      else if( string( argv[ first_arg ] ) == string( "-cc" ) )
      {
         ++first_arg;
         use_chacha20 = true;
      }
      else if( string( argv[ first_arg ] ) == string( "-dh" ) )
      {
         ++first_arg;
         use_dbl_hash = true;
      }
      else if( string( argv[ first_arg ] ) == string( "-p" ) )
      {
         ++first_arg;
         use_pin_hash = true;
      }
      else if( string( argv[ first_arg ] ) == string( "-ps" ) )
      {
         ++first_arg;
         use_pin_hash = true;
         use_pin_as_salt = true;
      }
      else if( string( argv[ first_arg ] ) == string( "-t" ) )
      {
         ++first_arg;
         use_test_file = true;
      }
      else
      {
         cout << c_title << endl;
         cerr << "Error: Unexpected option '" << argv[ first_arg ] << "'." << endl;
         return 1;
      }
   }

   if( !is_quiet )
      cout << c_title << endl;

   if( first_arg == argc )
   {
      cerr << "Error: Incorrect usage (use crypt -? to see usage)." << endl;
      return 1;
   }

   try
   {
      string file_name( argv[ first_arg ] );

      if( use_chacha20 && use_dbl_hash )
         throw runtime_error( "Can choose either the \"-cc\" or \"-dh\" option but not both." );

      stream_cipher cipher = e_stream_cipher_unknown;

      // NOTE: If unspecified defaults to "chacha20".
      if( !use_chacha20 && !use_dbl_hash )
         use_chacha20 = true;

      if( use_chacha20 )
         cipher = e_stream_cipher_chacha20;
      else if( use_dbl_hash )
         cipher = e_stream_cipher_dbl_hash;

      struct stat statbuf;
      struct utimbuf otimes;

      if( stat( argv[ first_arg ], &statbuf ) != 0 )
      {
         cerr << "Error: Unable to open file '" << argv[ first_arg ] << "' for input." << endl;
         return 1;
      }

      otimes.actime = statbuf.st_atime;
      otimes.modtime = statbuf.st_mtime;

      // NOTE: Empty code block for scope purposes.
      {
         fstream fs;
         fs.open( argv[ first_arg ], ios::in | ios::out | ios::binary );
         if( !fs )
         {
            cerr << "Error: Unable to open file '" << argv[ first_arg ] << "' for input and output." << endl;
            return 1;
         }

         string password;
         while( password.empty( ) )
            password = get_password( "Password: " );

         if( use_confirm )
         {
            string confirm( get_password( " Confirm: " ) );
            if( confirm != password )
            {
               cerr << "Error: Password confirmation failed." << endl;
               return 1;
            }
         }

         if( use_hrounds )
         {
            for( size_t i = 0; i < c_num_hrounds; i++ )
            {
               MD5 md5( ( unsigned char* )password.c_str( ) );
               password = string( md5.hex_digest( ) );
            }
         }

         if( use_pin_hash )
         {
            string pin( get_password( "Type PIN: " ) );

            if( use_confirm )
            {
               string confirm( get_password( " Confirm: " ) );

               if( confirm != pin )
               {
                  cerr << "Error: PIN confirmation failed." << endl;
                  return 1;
               }
            }

            if( use_pin_as_salt )
               password += pin;

            string original_password( password );

            hash_count = atoi( pin.c_str( ) );
            for( int i = 0; i < hash_count; i++ )
            {
               MD5 md5( ( unsigned char* )password.c_str( ) );
               password = string( md5.hex_digest( ) );
            }

            size_t pos = password.length( ) / 2;
            password = password.substr( 0, pos - 1 ) + original_password + password.substr( pos );
         }

         string test_file_name( argv[ first_arg ] );
         if( use_test_file )
         {
            test_file_name += ".chk";
            if( !file_exists( test_file_name ) )
            {
               create_test_file = true;

               if( first_arg + 1 < argc )
                  throw runtime_error( "Cannot run program as .chk file not found (not encrypted?)." );
            }

            if( create_test_file )
               create_checksum_test_file( fs, test_file_name );
         }

         crypt_stream( fs, password.c_str( ), password.size( ), cipher );

         if( !fs.good( ) )
            throw runtime_error( "Unexpected error occurred whilst writing to file '" + file_name + "'." );

         if( use_test_file && !create_test_file )
         {
            fs.seekg( 0, ios::end );
            fstream::pos_type size = fs.tellg( );
            fs.seekg( 0, ios::beg );

            MD5 md5;
            unsigned char buffer[ c_buffer_size ];

            int size_left = ( int )size;
            while( size_left )
            {
               int next_len( min( size_left, c_buffer_size ) );

               fs.read( ( char* )buffer, next_len );

               int len = fs.gcount( );
               md5.update( buffer, len );

               size_left -= next_len;
            }
            md5.finalize( );
            fs.seekg( 0, ios::beg );

            string test_chk = buffer_file( test_file_name );

            if( test_chk != md5.hex_digest( ) )
            {
               cerr << "Error: Failed checksum test (reverting file contents)." << endl;

               crypt_stream( fs, password.c_str( ), password.size( ), cipher );

               return 2;
            }
         }

         if( !fs.good( ) )
            throw runtime_error( "Unexpected error occurred whilst writing to file '" + file_name + "'." );

         if( ++first_arg < argc )
         {
            fs.close( );

            int rc = system( argv[ first_arg ] );
            ( void )rc;

            fs.open( file_name.c_str( ), ios::in | ios::out | ios::binary );

            crypt_stream( fs, password.c_str( ), password.size( ), cipher );

            if( !fs.good( ) )
            {
               cerr << "Error: Unexpected error occurred whilst writing to file '" << argv[ first_arg ] << "'." << endl;
               return 2;
            }
         }
         else if( use_test_file && !create_test_file )
            file_remove( test_file_name );
      }

      utime( argv[ first_arg - 1 ], &otimes );

      if( !is_quiet )
         cout << "Finished." << endl;
   }
   catch( exception& x )
   {
      rc = 1;

      cerr << "Error: " << x.what( ) << endl;
   }
   catch( ... )
   {
      rc = 2;

      cerr << "Error: unexpected unknown exception caught" << endl;
   }

   return rc;
}
