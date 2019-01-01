// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2019 CIYAM Developers
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
#  ifdef _WIN32
#     ifndef STRICT
#        define STRICT // Needed for "windows.h" by various Borland headers.
#     endif
#     include <windows.h>
#  else
#     include <time.h>
#     include <utime.h>
#     include <sys/stat.h>
#  endif
#endif

#include "md5.h"
#include "console.h"
#include "utilities.h"
#include "crypt_stream.h"

using namespace std;

const int c_buffer_size = 1024;
const char* const c_title = "crypt v0.1h";

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
      cout << c_title << "\nUsage: crypt [-q] [-c] [-p[s]] [-t] <file> [<program>]" << endl;
      return 0;
   }

   int first_arg = 1;
   int hash_count = 0;
   bool is_quiet = false;
   bool use_confirm = false;
   bool use_hashing = false;
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
      else if( string( argv[ first_arg ] ) == string( "-p" ) )
      {
         ++first_arg;
         use_hashing = true;
      }
      else if( string( argv[ first_arg ] ) == string( "-ps" ) )
      {
         ++first_arg;
         use_hashing = true;
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

#ifdef _WIN32
      FILETIME atime, ctime, mtime;

      HANDLE hFile = ::CreateFile( argv[ first_arg ], GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );

      if( hFile == INVALID_HANDLE_VALUE )
      {
         cerr << "Error: Unable to open file '" << argv[ first_arg ] << "' for input." << endl;
         return 1;
      }

      ::GetFileTime( hFile, &ctime, &atime, &mtime );

      ::CloseHandle( hFile );
#else
      struct stat statbuf;
      struct utimbuf otimes;

      if( stat( argv[ first_arg ], &statbuf ) != 0 )
      {
         cerr << "Error: Unable to open file '" << argv[ first_arg ] << "' for input." << endl;
         return 1;
      }

      otimes.actime = statbuf.st_atime;
      otimes.modtime = statbuf.st_mtime;
#endif

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

         if( use_hashing )
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

         crypt_stream( fs, password.c_str( ), password.size( ) );

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
               crypt_stream( fs, password.c_str( ), password.size( ) );
               return 2;
            }
         }

         if( !fs.good( ) )
            throw runtime_error( "Unexpected error occurred whilst writing to file '" + file_name + "'." );

         if( use_test_file && !create_test_file )
            file_remove( test_file_name );

         if( ++first_arg < argc )
         {
            fs.close( );
            system( argv[ first_arg ] );

            fs.open( file_name.c_str( ), ios::in | ios::out | ios::binary );

            if( use_test_file && !create_test_file )
               create_checksum_test_file( fs, test_file_name );

            crypt_stream( fs, password.c_str( ), password.size( ) );

            if( !fs.good( ) )
            {
               cerr << "Error: Unexpected error occurred whilst writing to file '" << argv[ first_arg ] << "'." << endl;
               return 2;
            }
         }
      }

#ifndef _WIN32
      utime( argv[ first_arg - 1 ], &otimes );
#else
      hFile = ::CreateFile( argv[ first_arg - 1 ],
       GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0 );

      if( hFile != INVALID_HANDLE_VALUE )
      {
         ::SetFileTime( hFile, &ctime, &atime, &mtime );
         ::CloseHandle( hFile );
      }
#endif

      if( !is_quiet )
         cout << "Finished." << endl;
   }
   catch( exception& x )
   {
      cerr << "Error: " << x.what( ) << endl;
      return 2;
   }
   catch( ... )
   {
      cerr << "Error: unexpected exception caught" << endl;
      return 2;
   }

   return 0;
}

