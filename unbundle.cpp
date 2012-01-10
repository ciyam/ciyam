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
#  include <cerrno>
#  include <cstdlib>
#  include <set>
#  include <deque>
#  include <stack>
#  include <string>
#  include <memory>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#  include <sys/stat.h>
#  ifdef __GNUG__
#     include <unistd.h>
#  endif
#  ifdef __BORLANDC__
#     include <cio>
#     include <dos.h>
#     include <dir.h>
#  endif
#  ifdef _MSC_VER
#     include <io.h>
#     include <direct.h>
#  endif
#endif

#ifdef __GNUG__
#  define _mkdir mkdir
#  define _access access
#endif

#include "md5.h"
#include "base64.h"
#include "config.h"
#include "utilities.h"

#ifdef ZLIB_SUPPORT
#  include <zlib.h>
#endif

using namespace std;

const char c_type_file = 'F';
const char c_type_checksum = 'C';
const char c_type_directory = 'D';

const char* const c_zlib_extension = ".gz";
const char* const c_default_extension = ".bun";

MD5 g_md5;

#ifdef ZLIB_SUPPORT
bool read_zlib_line( gzFile& gzf, string& s )
{
   char c;
   s.erase( );

   while( !gzeof( gzf ) )
   {
      if( !gzread( gzf, &c, sizeof( char ) ) )
         throw runtime_error( "reading char" );

      if( c == '\n' )
         break;

      s += c;
   }

   return !s.empty( );
}
#endif

void create_all_directories( deque< string >& create_directories )
{
   while( !create_directories.empty( ) )
   {
      string path_name( create_directories.front( ) );
      cout << "creating directory '" << path_name << "'..." << endl;
#ifdef _WIN32
      if( _mkdir( path_name.c_str( ) ) < 0 )
         throw runtime_error( "unable to create directory '" + path_name + "' (already exists?)" );
#else
      if( _mkdir( path_name.c_str( ), S_IRWXU ) < 0 )
         throw runtime_error( "unable to create directory '" + path_name + "' (already exists?)" );
#endif
      create_directories.pop_front( );
   }
}

int main( int argc, char* argv[ ] )
{
   cout << "unbundle v0.1b\n";

   int first_arg = 0;
   bool junk = false;
   bool skip = false;
   bool prune = false;
   bool use_zlib = false;

   if( argc > first_arg + 1  )
   {
      if( string( argv[ first_arg + 1 ] ) == "-j" )
      {
         ++first_arg;
         junk = true;
      }
      else if( string( argv[ first_arg + 1 ] ) == "-s" )
      {
         ++first_arg;
         skip = true;
      }
   }

   if( argc > first_arg + 1  )
   {
      if( string( argv[ first_arg + 1 ] ) == "-p" )
      {
         ++first_arg;
         prune = true;
      }
   }

   if( ( argc - first_arg < 2 )
    || string( argv[ 1 ] ) == "?" || string( argv[ 1 ] ) == "/?" || string( argv[ 1 ] ) == "-?" )
   {
      cout << "usage: unbundle [-j|-s] [-p] <filename> [<filespec1> [<filespec2> [...]]" << endl;

      cout << "\nwhere: -j is to junk directories or -s skips the top level directory" << endl;
      cout << "  and: -p is to prune empty directories" << endl;
      return 0;
   }

   try
   {
      string filename( argv[ first_arg + 1 ] );

      if( filename.length( ) > 3
       && filename.substr( filename.length( ) - 3 ) == string( c_zlib_extension ) )
      {
#ifdef ZLIB_SUPPORT
         use_zlib = true;
#else
         throw runtime_error( "this program has not been compiled with ZLIB support" );
#endif
      }

      if( _access( filename.c_str( ), 0 ) != 0 )
         filename += c_default_extension;

#ifdef ZLIB_SUPPORT
      if( _access( filename.c_str( ), 0 ) != 0 )
      {
         use_zlib = true;
         filename += c_zlib_extension;
      }

      gzFile gzf;
      ifstream inpf;

      if( !use_zlib )
         inpf.open( filename.c_str( ) );
      else
         gzf = gzopen( filename.c_str( ), "rb" );

      if( ( use_zlib && !gzf ) || ( !use_zlib && !inpf ) )
         throw runtime_error( "unable to open file '" + filename + "' for input" );
#else
      ifstream inpf( filename.c_str( ) );
      if( !inpf )
         throw runtime_error( "unable to open file '" + filename + "' for input" );
#endif

      cout << "started unbundling '" << filename << "'\n";

      vector< string > filename_filters;
      for( int i = first_arg + 2; i < argc; i++ )
         filename_filters.push_back( argv[ i ] );

      int level = -1;
      bool is_first = false;

      string next_file;
      int file_data_lines = 0;

      stack< string > paths;
      deque< string > create_directories;

      set< string > created;
      auto_ptr< ofstream > ap_ofstream;

      MD5 md5;
      string next;
      string next_md5;
      size_t line = 0;
      bool finished = false;
      string top_level_directory;

      while( true )
      {
#ifdef ZLIB_SUPPORT
         if( use_zlib )
         {
            if( !read_zlib_line( gzf, next ) )
               break;
         }
         else if( !getline( inpf, next ) )
            break;
#else
         if( !getline( inpf, next ) )
            break;
#endif
         ++line;
         // NOTE: In case the input file had been treated as binary during an FTP remove trailing CR.
         if( next.size( ) && next[ next.size( ) - 1 ] == '\r' )
            next.erase( next.size( ) - 1 );

         if( next.empty( ) )
            throw runtime_error( "unexpected empty line #" + to_string( line ) );

         if( file_data_lines )
         {
            --file_data_lines;

            if( ap_ofstream.get( ) )
            {
               string fdata( base64::decode( next ) );
               if( ap_ofstream->rdbuf( )->sputn( fdata.c_str( ), fdata.length( ) ) != ( int )fdata.length( ) )
                  throw runtime_error( "write failed for file '" + next_file + "'" );

               md5.update( ( unsigned char* )fdata.c_str( ), fdata.length( ) );

               if( file_data_lines == 0 )
               {
                  ap_ofstream->flush( );
                  if( !ap_ofstream->good( ) )
                     throw runtime_error( "flush failed for file '" + next_file + "'" );
                  ap_ofstream->close( );

                  md5.finalize( );
                  string digest( md5.hex_digest( ) );

                  if( next_md5 != digest )
                     cerr << "*** error: file '" << next_file << "' failed MD5 digest check ***" << endl;
               }
            }

            continue;
         }

         if( finished )
            throw runtime_error( "unexpected end of file not found at line #" + to_string( line ) );

         string::size_type pos = next.find( ' ' );
         if( pos != 1 )
            throw runtime_error( "unexpected format in line #" + to_string( line ) );

         char type( next[ 0 ] );
         if( type == c_type_file )
         {
            if( is_first )
               throw runtime_error( "unexpected file entry in line #" + to_string( line ) );

            md5.init( );

            next.erase( 0, 2 );
            pos = next.find( ' ' );
            if( pos == string::npos )
               throw runtime_error( "unexpected format in line #" + to_string( line ) );

            int num_lines( atoi( next.substr( 0, pos ).c_str( ) ) );
            if( num_lines < 0 )
               throw runtime_error( "invalid number of lines "
                + to_string( num_lines ) + " specified in line #" + to_string( line ) );

            file_data_lines = num_lines;

            next.erase( 0, pos + 1 );

            string::size_type pos = next.find_last_of( " " );
            if( pos == string::npos )
               throw runtime_error( "unexpected file entry format in line #" + to_string( line ) );

            next_md5 = next.substr( pos + 1 );
            next.erase( pos );

            g_md5.update( ( unsigned char* )next_md5.c_str( ), next_md5.length( ) );

            string test_file( next );
            if( !paths.empty( ) )
               test_file = paths.top( ) + "/" + next;

            if( junk )
               next_file = next;
            else
               next_file = test_file;

            bool matched = false;
            if( filename_filters.empty( ) )
               matched = true;
            else
            {
               for( size_t i = 0; i < filename_filters.size( ); i++ )
               {
                  string wildcard( filename_filters[ i ] );

                  string::size_type pos = wildcard.find( "/" );
                  if( pos == string::npos )
                  {
                     pos = test_file.find_last_of( "/" );
                     if( pos != string::npos )
                        test_file.erase( 0, pos + 1 );
                  }
                  else if( junk )
                  {
                     if( wildcard.find( top_level_directory + "/" ) != 0 )
                        wildcard = top_level_directory + "/" + wildcard;
                  }

                  if( wildcard_match( wildcard, test_file ) )
                  {
                     matched = true;
                     break;
                  }
               }
            }

            if( matched )
               create_all_directories( create_directories );

            if( !matched )
               ap_ofstream.reset( );
            else
            {
               cout << "extracting '" << next_file << "'..." << endl;

               ap_ofstream = auto_ptr< ofstream >( new ofstream( next_file.c_str( ), ios::out | ios::binary ) );
               if( !*ap_ofstream.get( ) )
                  throw runtime_error( "unable to open file '" + next_file + "' for output" );
            }
         }
         else if( type == c_type_directory )
         {
            next.erase( 0, 2 );
            pos = next.find( ' ' );
            if( pos == string::npos )
               throw runtime_error( "unexpected format in line #" + to_string( line ) );

            string::size_type cpos = next.find_last_of( ' ' );
            if( cpos == pos || cpos == string::npos )
               throw runtime_error( "unexpected format in line #" + to_string( line ) );

            string check( next.substr( cpos + 1 ) );
            next.erase( cpos );

            int next_level( atoi( next.substr( 0, pos ).c_str( ) ) );
            if( next_level > level )
            {
               if( next_level != level + 1 )
                  throw runtime_error( "expecting level " + to_string( level + 1 )
                   + " but found " + to_string( next_level ) + " in line #" + to_string( line ) );

               level = next_level;
            }
            else
            {
               if( next_level < 0 )
                  throw runtime_error( "invalid level " + to_string( next_level ) + " found in line #" + to_string( line ) );

               if( skip || create_directories.size( ) > 1 )
               {
                  size_t test_level( next_level );
                  if( next_level > 0 && skip )
                     --test_level;

                  while( create_directories.size( ) > test_level )
                     create_directories.pop_back( );
               }

               level = next_level;

               while( ( int )paths.size( ) > level )
                  paths.pop( );
            }

            next.erase( 0, pos + 1 );
            string path_name( next );

            if( top_level_directory.empty( ) )
               top_level_directory = path_name;

            MD5 md5;
            md5.update( ( unsigned char* )path_name.c_str( ), path_name.length( ) );
            md5.finalize( );

            string digest( md5.hex_digest( ) );

            if( check != digest )
               cerr << "*** error: directory '" << path_name << "' failed MD5 digest check ***" << endl;

            g_md5.update( ( unsigned char* )digest.c_str( ), digest.length( ) );

            if( !skip || level > 0 )
            {
               if( skip )
               {
                  string::size_type pos = path_name.find( '/' );
                  if( pos == string::npos )
                     throw runtime_error( "unexpected path_name '" + path_name + " found in line #" + to_string( line ) );

                  path_name.erase( 0, pos + 1 );
               }

               if( !junk && !created.count( path_name ) )
               {
                  created.insert( path_name );
                  create_directories.push_back( path_name );

                  if( !prune )
                     create_all_directories( create_directories );
               }

               paths.push( path_name );
            }

            is_first = false;
         }
         else if( type == c_type_checksum )
         {
            g_md5.finalize( );
            string digest( g_md5.hex_digest( ) );

            next.erase( 0, 2 );

            if( next != digest )
               cerr << "*** error: bundle file failed MD5 digest check ***" << endl;

            finished = true;
         }
         else
            throw runtime_error( "unexpected entry type '" + to_string( type ) + "' found in line #" + to_string( line ) );
      }

      if( !finished )
         cerr << "*** error: final MD5 digest not found (file truncated?) ***" << endl;

#ifdef ZLIB_SUPPORT
      if( use_zlib && !gzeof( gzf ) )
         throw runtime_error( "unexpected error occurred whilst reading '" + filename + "' for input" );
#endif
      if( !use_zlib && !inpf.eof( ) )
         throw runtime_error( "unexpected error occurred whilst reading '" + filename + "' for input" );

      cout << "finished unbundling '" << filename << "'" << endl;
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
      return 1;
   }

   return 0;
}

