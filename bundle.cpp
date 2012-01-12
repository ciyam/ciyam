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

int c_format_version = 1;

const char c_type_file = 'F';
const char c_type_checksum = 'C';
const char c_type_directory = 'D';

const int c_buffer_size = 8192;

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

string escaped_line( const string& line )
{
   string str;

   bool is_escape = false;
   for( size_t i = 0; i < line.length( ); i++ )
   {
      if( line[ i ] == '\\' )
         str += '\\';

      if( line[ i ] == '\n' )
         str += "\\n";
      else
         str += line[ i ];
   }

   return str;
}

#ifdef ZLIB_SUPPORT
bool read_zlib_line( gzFile& gzf, string& s, bool unescape = true )
{
   char c;
   s.erase( );

   bool is_escape = false;
   while( !gzeof( gzf ) )
   {
      if( !gzread( gzf, &c, sizeof( char ) ) )
         throw runtime_error( "reading char" );

      if( is_escape )
      {
         is_escape = false;

         if( c == 'n' && unescape )
            c = '\n';
      }
      else if( c == '\\' )
         is_escape = true;
      else if( c == '\n' )
         break;

      if( !is_escape || !unescape )
         s += c;
   }

   return !s.empty( );
}

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
void check_file_header( ifstream& inpf, const string& filename, bool& is_base64 )
#else
void check_file_header( ifstream& inpf, const string& filename, bool& is_base64, bool use_zlib, gzFile& gzf )
#endif
{
   string header;
#ifdef ZLIB_SUPPORT
   if( use_zlib )
      read_zlib_line( gzf, header );
   else
      getline( inpf, header );
#else
   getline( inpf, header );
#endif

   string::size_type pos = header.find( ' ' );
   if( pos != 1 || header[ 0 ] != 'B' )
      throw runtime_error( "*** no bundle header found in '" + filename + "' ***" );

   header.erase( 0, 2 );
   pos = header.find( ' ' );
   if( pos == string::npos )
      throw runtime_error( "*** invalid bundle header was found in '" + filename + "' ***" );

   int ver = atoi( header.substr( 0, pos ).c_str( ) );
   header.erase( 0, pos + 1 );

   if( ver > c_format_version )
      throw runtime_error( "*** unknown bundle format version " + to_string( ver ) + " in '" + filename + "' ***" );

   if( header == "B64" )
      is_base64 = true;
   else if( header == "ESC" )
      is_base64 = false;
   else
      throw runtime_error( "*** unknown bundle data encoding type " + header + " in '" + filename + "' ***" );
}

#ifndef ZLIB_SUPPORT
void output_directory( set< string >& file_names, const string& path_name, int level, ofstream& outf )
#else
void output_directory( set< string >& file_names,
 const string& path_name, int level, ofstream& outf, bool use_zlib, gzFile& gzf )
#endif
{
   string::size_type pos = path_name.find_last_of( '/' );
   if( level > 1 && pos != string::npos && !g_output_directories.count( path_name.substr( 0, pos ) ) )
   {
#ifndef ZLIB_SUPPORT
      output_directory( file_names, path_name.substr( 0, pos ), level - 1, outf );
#else
      output_directory( file_names, path_name.substr( 0, pos ), level - 1, outf, use_zlib, gzf );
#endif
   }

   if( !file_names.count( path_name ) )
   {
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

      file_names.insert( path_name );
   }

   g_output_directories.insert( path_name );
}

#ifndef ZLIB_SUPPORT
void process_directory( const string& directory,
 const string& filespec_path, const vector< string >& filename_filters, set< string >& matched_filters,
 set< string >& file_names, bool recurse, bool prune, bool is_quiet, bool is_append, bool use_base64, ofstream& outf )
#else
void process_directory( const string& directory,
 const string& filespec_path, const vector< string >& filename_filters,
 set< string >& matched_filters, set< string >& file_names, bool recurse, bool prune,
 bool is_quiet, bool is_append, bool use_base64, ofstream& outf, bool use_zlib, gzFile& gzf )
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
         output_directory( file_names, path_name, dfsi.get_level( ), outf );
#else
         output_directory( file_names, path_name, dfsi.get_level( ), outf, use_zlib, gzf );
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
               matched_filters.insert( filename_filters[ i ] );

               break;
            }
         }

         if( file_names.count( ffsi.get_name( ) ) )
            matched = false;

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
            output_directory( file_names, path_name, dfsi.get_level( ), outf );
#else
            output_directory( file_names, path_name, dfsi.get_level( ), outf, use_zlib, gzf );
#endif
         }

         if( !is_quiet )
         {
            if( !is_append )
               cout << "adding '" << path_name << '/' << ffsi.get_name( ) << "'..." << endl;
            else
               cout << "adding/replacing '" << path_name << '/' << ffsi.get_name( ) << "'..." << endl;
         }

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

         int max_size = c_max_bytes_per_line;
         if( !use_base64 )
            max_size = c_buffer_size;

         size_t num_lines( size / max_size );
         if( size % max_size )
            ++num_lines;

         string digest( md5.hex_digest( ) );

         ostringstream osstr;
         osstr << "F " << num_lines << ' ' << ffsi.get_name( ) << ' ' << digest;

         file_names.insert( ffsi.get_name( ) );

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
            char buffer[ c_buffer_size ];
            streamsize count = inpf.rdbuf( )->sgetn( buffer, max_size );

            if( count == 0 )
               throw runtime_error( "read failed for file '" + ffsi.get_full_name( ) + "'" );

            string encoded;
            if( !use_base64 )
               encoded = escaped_line( string( buffer, count ) );
            else
               encoded = base64::encode( string( buffer, count ) );

#ifndef ZLIB_SUPPORT
            outf << encoded << '\n';
#else
            if( !use_zlib )
               outf << encoded << '\n';
            else
               write_zlib_line( gzf, encoded );
#endif

            size -= count;
         }
      }
   } while( recurse && dfsi.has_next( ) );
}

int main( int argc, char* argv[ ] )
{
   int first_arg = 0;
   bool prune = false;
   bool recurse = false;
   bool invalid = false;
   bool is_quiet = false;
   bool use_zlib = true;
   bool use_base64 = false;

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

   if( argc > first_arg + 1 )
   {
      if( string( argv[ first_arg + 1 ] ) == "-q" )
      {
         ++first_arg;
         is_quiet = true;
      }
   }

   if( argc > first_arg + 1 )
   {
      // NOTE: Ignore '-y' for compatibility with zip.
      if( string( argv[ first_arg + 1 ] ) == "-y" )
         ++first_arg;
   }

   if( argc > first_arg + 1 )
   {
      if( string( argv[ first_arg + 1 ] ) == "-b64" )
      {
         ++first_arg;
         use_base64 = true;
      }
   }

#ifdef ZLIB_SUPPORT
   if( argc > first_arg + 1 )
   {
      if( string( argv[ first_arg + 1 ] ) == "-ngz" )
      {
         ++first_arg;
         use_zlib = false;
      }
   }
#endif

   if( !is_quiet )
      cout << "bundle v0.1b\n";

   if( invalid || ( argc - first_arg < 2 )
    || string( argv[ 1 ] ) == "?" || string( argv[ 1 ] ) == "/?" || string( argv[ 1 ] ) == "-?" )
   {
#ifndef ZLIB_SUPPORT
      cout << "usage: bundle [-r [-p]] [-q] [-b64] <name> [<filespec1> [<filespec2> [...]]]" << endl;
#else
      cout << "usage: bundle [-r [-p]] [-q] [-b64] [-ngz] <name> [<filespec1> [<filespec2> [...]]]" << endl;
#endif

      cout << "\nwhere: -r is to recurse sub-directories (-p to prune empty directories)" << endl;
      cout << "  and: -q to suppress all output apart from errors" << endl;
      cout << "  and: -b64 stores file data using base64 encoding" << endl;
#ifdef ZLIB_SUPPORT
      cout << "  and: -ngz in order to not preform zlib compression" << endl;
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

      if( !filename.empty( ) && filename[ 0 ] == '-' )
         throw runtime_error( "unknown or bad option '" + filename + "' use -? to see options" );

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

      if( all_filespecs.empty( ) )
         all_filespecs[ g_cwd ].push_back( "*" );

      set< string > file_names;
      set< string > matched_filters;

      bool is_append = false;
      if( file_exists( filename ) )
         is_append = true;

      string output_filename( filename );
      if( is_append )
         output_filename = "~" + output_filename;

      // NOTE: Empty code block for scope purposes.
      {
#ifndef ZLIB_SUPPORT
         ofstream outf( output_filename.c_str( ), ios::out | ios::binary );
         if( !outf )
            throw runtime_error( "unable to open file '" + output_filename + "' for output" );
#else
         gzFile gzf;
         ofstream outf;

         if( !use_zlib )
            outf.open( output_filename.c_str( ), ios::out | ios::binary );
         else
            gzf = gzopen( output_filename.c_str( ), "wb9" );

         if( ( use_zlib && !gzf ) || ( !use_zlib && !outf ) )
            throw runtime_error( "unable to open file '" + output_filename + "' for output" );
#endif

         if( !is_quiet )
         {
            if( !is_append )
               cout << "==> started bundling '" << filename << "'\n";
            else
               cout << "==> continue bundling '" << filename << "'\n";
         }

         absolute_path( filename, g_bundle_file_name );

         string header( "B " );
         header += to_string( c_format_version );

         if( use_base64 )
            header += " B64";
         else
            header += " ESC";

#ifndef ZLIB_SUPPORT
         outf << header << '\n';
#else
         if( !use_zlib )
            outf << header << '\n';
         else
            write_zlib_line( gzf, header );
#endif

         for( map< string, vector< string > >::iterator i = all_filespecs.begin( ); i != all_filespecs.end( ); ++i )
         {
            string filespec_path( i->first );
            vector< string >& filename_filters( i->second );

#ifndef ZLIB_SUPPORT
            process_directory( directory, filespec_path, filename_filters,
             matched_filters, file_names, recurse, prune, is_quiet, is_append, use_base64, outf );
#else
            process_directory( directory, filespec_path, filename_filters,
             matched_filters, file_names, recurse, prune, is_quiet, is_append, use_base64, outf, use_zlib, gzf );
#endif
            if( !is_quiet )
            {
               for( size_t i = 0; i < filename_filters.size( ); i++ )
               {
                  if( matched_filters.count( filename_filters[ i ] ) == 0 )
                     cout << "warning: found no files matching '" << filename_filters[ i ] << "'" << endl;
               }
            }
         }

         if( is_append )
         {
#ifdef ZLIB_SUPPORT
            gzFile igzf;
            ifstream inpf;

            if( !use_zlib )
               inpf.open( filename.c_str( ), ios::in | ios::binary );
            else
               igzf = gzopen( filename.c_str( ), "rb" );

            if( ( use_zlib && !igzf ) || ( !use_zlib && !inpf ) )
               throw runtime_error( "unable to open file '" + filename + "' for input" );
#else
            ifstream inpf( filename.c_str( ), ios::in | ios::binary );
            if( !inpf )
               throw runtime_error( "unable to open file '" + filename + "' for input" );
#endif

            bool was_base64;
#ifndef ZLIB_SUPPORT
            check_file_header( inpf, filename, was_base64 );
#else
            check_file_header( inpf, filename, was_base64, use_zlib, igzf );
#endif

            if( was_base64 != use_base64 )
               throw runtime_error( "*** cannot combine different bundle data encoding types ***" );

            string next;
            int line = 0;
            int file_data_lines = 0;
            bool skip_existing_file = false;

            while( true )
            {
#ifdef ZLIB_SUPPORT
               if( use_zlib )
               {
                  if( !read_zlib_line( igzf, next, false ) )
                     break;
               }
               else if( !getline( inpf, next ) )
                  break;
#else
               if( !getline( inpf, next ) )
                  break;
#endif
               ++line;
               if( file_data_lines )
               {
                  --file_data_lines;
                  if( !skip_existing_file )
                  {
#ifndef ZLIB_SUPPORT
                     outf << line << '\n';
#else
                     if( !use_zlib )
                        outf << next << '\n';
                     else
                        write_zlib_line( gzf, next );
#endif
                  }
               }
               else
               {
                  string::size_type pos = next.find( ' ' );
                  if( pos != 1 )
                     throw runtime_error( "unexpected format in line #" + to_string( line ) );

                  char type( next[ 0 ] );

                  string next_line( next );

                  if( type == c_type_file )
                  {
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

                     string check = next.substr( pos + 1 );
                     next.erase( pos );

                     if( file_names.count( next ) )
                     {
                        skip_existing_file = true;
                        continue;
                     }
                     else
                     {
                        skip_existing_file = false;

                        file_names.insert( next );
                        g_md5.update( ( unsigned char* )check.c_str( ), check.length( ) );
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

                     pos = next.find( ' ' );
                     if( pos == string::npos )
                        throw runtime_error( "unexpected format in line #" + to_string( line ) );

                     next.erase( 0, pos + 1 );

                     if( file_names.count( next ) )
                        continue;
                     else
                     {
                        file_names.insert( next );
                        g_md5.update( ( unsigned char* )check.c_str( ), check.length( ) );
                     }
                  }
                  else if( type == c_type_checksum )
                     break;
                  else
                     throw runtime_error( "unexpected entry type '" + to_string( type ) + "' found in line #" + to_string( line ) );

#ifndef ZLIB_SUPPORT
                  outf << next_line << '\n';
#else
                  if( !use_zlib )
                     outf << next_line << '\n';
                  else
                     write_zlib_line( gzf, next_line );
#endif
               }
            }

#ifdef ZLIB_SUPPORT
            if( use_zlib )
               gzclose( igzf );
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
      }

      if( is_append )
      {
         if( !file_remove( filename ) || rename( output_filename.c_str( ), filename.c_str( ) ) != 0 )
            throw runtime_error( "unable to replace original '" + filename + "' with '" + output_filename + "'" );
      }
      else if( matched_filters.empty( ) )
      {
         file_remove( filename );
         throw runtime_error( "*** nothing to do ***" );
      }

      if( !is_quiet )
         cout << "==> finished bundling '" << filename << "'" << endl;
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
      return 1;
   }

   return 0;
}

