// Copyright (c) 2005
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
#  include <fstream>
#  include <sstream>
#  include <iostream>
#  include <stdexcept>
#endif

// KLUDGE: Suppress the "function now deprecated" warning as it is being incorrectly issued for
// the "sgetn" I/O function (an issue at least with the VS Express 2005 version of VC8).
#ifdef _MSC_VER
#  pragma warning (disable: 4996)
#endif

#define ZLIB_SUPPORT

#ifdef ZLIB_SUPPORT
#  include <zlib.h>
#endif

#include "md5.h"
#include "base64.h"
#include "fs_iterator.h"
#include "utilities.h"

using namespace std;

const int c_buffer_size = 1024;

// const int c_Max_Bytes_Per_Line = 57; // (for 76 characters)
const int c_Max_Bytes_Per_Line = 96; // (for 128 characters)

const char* const c_zlib_extension = ".gz";
const char* const c_default_extension = ".bun";

bool is_root_path( const string& absolute_path )
{
   if( absolute_path == "/" )
      return true;
#ifdef _WIN32
   if( absolute_path.size( ) == 3 )
      return true;
#endif
   return false;
}

#ifdef ZLIB_SUPPORT
void write_zlib_line( gzFile& gzf, const string& str )
{
   if( str.size( ) )
   {
      string s( str );
      s += '\n';

      if( !gzwrite( gzf, s.c_str( ), s.size( ) ) )
         throw runtime_error( "writing string content for '" + str + "'" );
   }
}
#endif

int main( int argc, char* argv[ ] )
{
   cout << "bundle v0.1\n";

   if( ( argc < 2 )
    || string( argv[ 1 ] ) == "?" || string( argv[ 1 ] ) == "/?" || string( argv[ 1 ] ) == "-?" )
   {
#ifndef ZLIB_SUPPORT
      cout << "usage: bundle <directory> [<filename>]" << endl;
#else
      cout << "usage: bundle [-z] <directory> [<filename>]" << endl;
#endif
      return 0;
   }

   int first_arg = 0;
   bool use_zlib = false;

#ifdef ZLIB_SUPPORT
   if( string( argv[ 1 ] ) == "-z" )
   {
      ++first_arg;
      use_zlib = true;
   }
#endif

   try
   {
      string absolute_bundle_path;
      if( !absolute_path( argv[ first_arg + 1 ], absolute_bundle_path ) )
         throw runtime_error( "unable to determine absolute path for '" + string( argv[ first_arg + 1 ] ) + "'" );

      if( is_root_path( absolute_bundle_path ) )
         throw runtime_error( "cannot bundle directory '" + string( argv[ first_arg + 1 ] ) + "' (need to specify a non-root directory)" );

      directory_filter df;
      fs_iterator dfsi( absolute_bundle_path, &df );

#ifndef _WIN32
      string::size_type rpos = absolute_bundle_path.find_last_of( "/" );
#else
      string::size_type rpos = absolute_bundle_path.find_last_of( "/\\" );
#endif

      string filename( argc > first_arg + 2 ? string( argv[ first_arg + 2 ] ) : absolute_bundle_path );
      if( argc == first_arg + 2 )
      {
         if( rpos != string::npos )
            filename.erase( 0, rpos + 1 );
         filename += c_default_extension;

         if( use_zlib )
            filename += c_zlib_extension;
      }

#ifndef ZLIB_SUPPORT
      ofstream outf( filename.c_str( ) );
      if( !outf )
         throw runtime_error( "unable to open file '" + filename + "' for output" );
#else
      gzFile gzf;
      ofstream outf;

      if( !use_zlib )
         outf.open( filename.c_str( ) );
      else
         gzf = gzopen( filename.c_str( ), "wb" );

      if( ( use_zlib && !gzf ) || ( !use_zlib && !outf ) )
         throw runtime_error( "unable to open file '" + filename + "' for output" );
#endif
      cout << "creating bundle '" << filename << "'\n";

      string absolute_bundle_file_name;

      if( !absolute_path( filename, absolute_bundle_file_name ) )
         throw runtime_error( "unable to determine absolute path for '" + filename + "'" );

      do
      {
         file_filter ff;
         fs_iterator ffsi( dfsi.get_path_name( ), &ff );

         string path_name( dfsi.get_path_name( ) );
         if( rpos != string::npos )
            path_name.erase( 0, rpos + 1 );

#ifdef _WIN32
         string::size_type pos;
         while( ( pos = path_name.find( "\\" ) ) != string::npos )
            path_name[ pos ] = '/';
#endif
         cout << "entered '" << path_name << "'..." << endl;

         ostringstream osstr;
         osstr << "D " << dfsi.get_level( ) << ' ' << path_name;

#ifndef ZLIB_SUPPORT
         outf << osstr.str( ) << '\n';
#else
         if( !use_zlib )
            outf << osstr.str( ) << '\n';
         else
            write_zlib_line( gzf, osstr.str( ) );
#endif
         while( ffsi.has_next( ) )
         {
            string absolute_file_name;
            if( !absolute_path( ffsi.get_full_name( ), absolute_file_name ) )
               throw runtime_error( "unable to determine absolute path for '" + ffsi.get_full_name( ) + "'" );

            if( absolute_file_name == absolute_bundle_file_name )
               continue;

            cout << "adding '" << path_name << '/' << ffsi.get_name( ) << "'..." << endl;

            ifstream inpf( ffsi.get_full_name( ).c_str( ), ios::in | ios::binary );
            if( !inpf )
               throw runtime_error( "unable to open file '" + ffsi.get_full_name( ) + "' for input" );

            inpf.seekg( 0, ios::end );
            ifstream::pos_type size = inpf.tellg( );
            inpf.seekg( 0, ios::beg );

            MD5 md5;
            unsigned char buffer[ c_buffer_size ];

            int size_left = ( int )size;
            while( size_left )
            {
               int next_len( min( size_left, c_buffer_size ) );

               inpf.read( ( char* )buffer, next_len );

               int len = inpf.gcount( );
               md5.update( buffer, len );

               size_left -= next_len;
            }
            md5.finalize( );
            inpf.seekg( 0, ios::beg );

            size_t num_lines( size / c_Max_Bytes_Per_Line );
            if( size % c_Max_Bytes_Per_Line )
               ++num_lines;

            ostringstream osstr;
            osstr << "F " << num_lines << ' ' << ffsi.get_name( ) << ' ' << md5.hex_digest( );

#ifndef ZLIB_SUPPORT
            outf << osstr.str( ) << '\n';
#else
            if( !use_zlib )
               outf << osstr.str( ) << '\n';
            else
               write_zlib_line( gzf, osstr.str( ) );
#endif

            while( size > 0 )
            {
               char buffer[ c_Max_Bytes_Per_Line + 1 ];
               streamsize count = inpf.rdbuf( )->sgetn( buffer, c_Max_Bytes_Per_Line );

               if( count == 0 )
                  throw runtime_error( "read failed for file '" + ffsi.get_full_name( ) + "'" );

               string b64_encoded( base64::encode( string( buffer, count ) ) );

#ifndef ZLIB_SUPPORT
               outf << b64_encoded << '\n';
#else
               if( !use_zlib )
                  outf << b64_encoded << '\n';
               else
                  write_zlib_line( gzf, b64_encoded );
#endif

               size -= count;
            }
         }
      } while( dfsi.has_next( ) );

#ifdef ZLIB_SUPPORT
      if( use_zlib )
         gzclose( gzf );
#endif

      if( !use_zlib )
      {
         outf.flush( );
         if( !outf.good( ) )
            throw runtime_error( "unexpected write failure for output file '" + filename + "'" );
      }

      cout << "finished bundle '" << filename << "'" << endl;
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
      return 1;
   }

   return 0;
}

