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
#  include <map>
#  include <fstream>
#  include <sstream>
#  include <iostream>
#  include <stdexcept>
#  ifdef __BORLANDC__
#     include <dir.h>
#  endif
#  ifdef _MSC_VER
#     include <direct.h>
#  endif
#  ifdef __GNUG__
#     include <limits.h>
#     include <unistd.h>
#  endif
#endif

// KLUDGE: Suppress the "function now deprecated" warning as it is being incorrectly issued for
// the "sgetn" I/O function (an issue at least with the VS Express 2005 version of VC8).
#ifdef _MSC_VER
#  pragma warning (disable: 4996)
#endif

#ifdef _WIN32
#  define PATH_MAX MAX_PATH
#  include <windows.h>
#endif

#include "md5.h"
#include "base64.h"
#include "config.h"
#include "fs_iterator.h"
#include "utilities.h"

#ifdef ZLIB_SUPPORT
#  include <zlib.h>
#endif

using namespace std;

const int c_buffer_size = 1024;

// const int c_max_bytes_per_line = 57; // (for 76 characters)
const int c_max_bytes_per_line = 96; // (for 128 characters)

const char* const c_zlib_extension = ".gz";
const char* const c_default_extension = ".bun";

MD5 g_md5;

string g_cwd;
string g_bundle_file_name;

set< string > g_output_directories;

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

#ifndef ZLIB_SUPPORT
void output_directory( const string& path_name, int level, ofstream& outf )
#else
void output_directory( const string& path_name, int level, ofstream& outf, bool use_zlib, gzFile& gzf )
#endif
{
   string::size_type pos = path_name.find_last_of( '/' );
   if( level > 1 && pos != string::npos && !g_output_directories.count( path_name.substr( 0, pos ) ) )
   {
#ifndef ZLIB_SUPPORT
      output_directory( path_name.substr( 0, pos ), level - 1, outf );
#else
      output_directory( path_name.substr( 0, pos ), level - 1, outf, use_zlib, gzf );
#endif
   }

   MD5 md5;
   md5.update( ( unsigned char* )path_name.c_str( ), path_name.length( ) );
   md5.finalize( );

   string digest( md5.hex_digest( ) );

   ostringstream osstr;
   osstr << "D " << level << ' ' << path_name << ' ' << digest;

   g_md5.update( ( unsigned char* )digest.c_str( ), digest.length( ) );

#ifndef ZLIB_SUPPORT
   outf << osstr.str( ) << '\n';
#else
   if( !use_zlib )
      outf << osstr.str( ) << '\n';
   else
      write_zlib_line( gzf, osstr.str( ) );
#endif

   g_output_directories.insert( path_name );
}

#ifndef ZLIB_SUPPORT
void process_directory( const string& directory, const string& filespec_path,
 const vector< string >& filename_filters, bool recurse, bool prune, ofstream& outf )
#else
void process_directory( const string& directory, const string& filespec_path,
 const vector< string >& filename_filters, bool recurse, bool prune, ofstream& outf, bool use_zlib, gzFile& gzf )
#endif
{
   directory_filter df;
   fs_iterator dfsi( filespec_path, &df );

   do
   {
      file_filter ff;
      fs_iterator ffsi( dfsi.get_path_name( ), &ff );

      string path_name( dfsi.get_path_name( ) );
      if( path_name.find( filespec_path ) == 0 )
         path_name.erase( 0, filespec_path.length( ) );

      if( path_name.empty( ) )
         path_name = directory;
      else
         path_name = directory + path_name;

#ifdef _WIN32
      string::size_type pos;
      while( ( pos = path_name.find( "\\" ) ) != string::npos )
         path_name[ pos ] = '/';
#endif

      bool has_output_directory = false;

      if( !prune || dfsi.get_level( ) == 0 )
      {
         has_output_directory = true;
#ifndef ZLIB_SUPPORT
         output_directory( path_name, dfsi.get_level( ), outf );
#else
         output_directory( path_name, dfsi.get_level( ), outf, use_zlib, gzf );
#endif
      }

      while( ffsi.has_next( ) )
      {
         bool matched = false;
         for( int i = 0; i < filename_filters.size( ); i++ )
         {
            string next_filter( filename_filters[ i ] );

            string::size_type pos = next_filter.find_last_of( '/' );
            if( pos != string::npos )
            {
               if( g_cwd != filespec_path )
                  continue;

               string sub_path( directory + "/" + next_filter.substr( 0, pos ) );

               if( path_name.find( sub_path ) != 0 )
                  continue;

               next_filter.erase( 0, pos + 1 );
            }

            if( wildcard_match( next_filter, ffsi.get_name( ) ) )
            {
               matched = true;
               break;
            }
         }

         if( !matched )
            continue;

         string absolute_file_name;
         if( !absolute_path( ffsi.get_full_name( ), absolute_file_name ) )
            throw runtime_error( "unable to determine absolute path for '" + ffsi.get_full_name( ) + "'" );

         if( absolute_file_name == g_bundle_file_name )
            continue;

         if( !has_output_directory )
         {
            has_output_directory = true;
#ifndef ZLIB_SUPPORT
            output_directory( path_name, dfsi.get_level( ), outf );
#else
            output_directory( path_name, dfsi.get_level( ), outf, use_zlib, gzf );
#endif
         }

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

         size_t num_lines( size / c_max_bytes_per_line );
         if( size % c_max_bytes_per_line )
            ++num_lines;

         string digest( md5.hex_digest( ) );

         ostringstream osstr;
         osstr << "F " << num_lines << ' ' << ffsi.get_name( ) << ' ' << digest;

         g_md5.update( ( unsigned char* )digest.c_str( ), digest.length( ) );

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
            char buffer[ c_max_bytes_per_line + 1 ];
            streamsize count = inpf.rdbuf( )->sgetn( buffer, c_max_bytes_per_line );

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
   } while( recurse && dfsi.has_next( ) );
}

int main( int argc, char* argv[ ] )
{
   cout << "bundle v0.1b\n";

   int first_arg = 0;
   bool prune = false;
   bool recurse = false;
   bool invalid = false;
   bool use_zlib = false;

   if( argc > first_arg + 1 )
   {
      if( string( argv[ first_arg + 1 ] ) == "-r" )
      {
         ++first_arg;
         recurse = true;
      }
   }

   if( argc > first_arg + 1 )
   {
      if( string( argv[ first_arg + 1 ] ) == "-p" )
      {
         ++first_arg;
         prune = true;

         if( !recurse )
            invalid = true;
      }
   }

#ifdef ZLIB_SUPPORT
   if( argc > first_arg + 1 )
   {
      if( string( argv[ first_arg + 1 ] ) == "-z" )
      {
         ++first_arg;
         use_zlib = true;
      }
   }
#endif

   if( invalid || ( argc - first_arg < 3 )
    || string( argv[ 1 ] ) == "?" || string( argv[ 1 ] ) == "/?" || string( argv[ 1 ] ) == "-?" )
   {
#ifndef ZLIB_SUPPORT
      cout << "usage: bundle [-r [-p]] <name> <filespec1> [<filespec2> [...]]" << endl;
#else
      cout << "usage: bundle [-r [-p]] [-z] <name> <filespec1> [<filespec2 [...]>]" << endl;
#endif

      cout << "\nwhere: -r is to recurse sub-directories (-p to prune empty directories)" << endl;
#ifdef ZLIB_SUPPORT
      cout << "  and: -z to use zlib compression" << endl;
#endif
      return 0;
   }

   try
   {
      char buf[ PATH_MAX ];
      getcwd( buf, PATH_MAX );

      g_cwd = string( buf );

#ifndef _WIN32
      string::size_type pos = g_cwd.find_last_of( "/" );
#else
      string::size_type pos = g_cwd.find_last_of( "/\\" );
#endif
      if( is_root_path( g_cwd ) )
         throw runtime_error( "cannot created a bundle in the root directory" );

      map< string, vector< string > > all_filespecs;

      string filename( argv[ first_arg + 1 ] );
      filename += c_default_extension;

      if( use_zlib )
         filename += c_zlib_extension;

      string directory = g_cwd.substr( pos + 1 );
      for( int i = first_arg + 2; i < argc; i++ )
      {
         string filespec_path;
         if( !absolute_path( argv[ i ], filespec_path ) )
            throw runtime_error( "unable to determine absolute path for '" + string( argv[ i ] ) + "'" );

         if( is_root_path( filespec_path ) )
            throw runtime_error( "cannot bundle directory '" + string( argv[ i ] ) + "' (need to specify a non-root directory)" );

#ifndef _WIN32
         string::size_type rpos = filespec_path.find_last_of( "/" );
#else
         string::size_type rpos = filespec_path.find_last_of( "/\\" );
#endif

         string filename_filter;
         if( rpos != string::npos )
         {
            filename_filter = filespec_path.substr( rpos + 1 );
            filespec_path.erase( rpos );
         }

         if( recurse == true
          && filespec_path.length( ) > g_cwd.length( ) && filespec_path.find( g_cwd ) == 0 )
         {
            filename_filter = filespec_path.substr( g_cwd.length( ) + 1 ) + "/" + filename_filter;
            filespec_path = g_cwd;
         }

         all_filespecs[ filespec_path ].push_back( filename_filter );
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

      absolute_path( filename, g_bundle_file_name );

      for( map< string, vector< string > >::iterator i = all_filespecs.begin( ); i != all_filespecs.end( ); ++i )
      {
         string filespec_path( i->first );
         vector< string >& filename_filters( i->second );

#ifndef ZLIB_SUPPORT
         process_directory( directory, filespec_path, filename_filters, recurse, prune, outf );
#else
         process_directory( directory, filespec_path, filename_filters, recurse, prune, outf, use_zlib, gzf );
#endif
      }

      g_md5.finalize( );
      ostringstream osstr;
      osstr << "C " << g_md5.hex_digest( );

#ifndef ZLIB_SUPPORT
      outf << osstr.str( ) << '\n';
#else
      if( !use_zlib )
         outf << osstr.str( ) << '\n';
      else
         write_zlib_line( gzf, osstr.str( ) );
#endif


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

