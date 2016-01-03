// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2016 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
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

// KLUDGE: Suppress the "function now deprecated" warning as it is being incorrectly issued for
// the "sgetn" I/O function (an issue at least with the VS Express 2005 version of VC8).
#ifdef _MSC_VER
#  pragma warning (disable: 4996)
#endif

#ifdef __GNUG__
#  define _mkdir mkdir
#  define _access access
#endif

#include "md5.h"
#include "base64.h"
#include "config.h"
#include "format.h"
#include "console.h"
#include "utilities.h"

#ifdef ZLIB_SUPPORT
#  include <zlib.h>
#endif

using namespace std;

int c_format_version = 1;

enum encoding_type
{
   e_encoding_type_b64,
   e_encoding_type_esc,
   e_encoding_type_raw
};

const char c_type_file = 'F';
const char c_type_checksum = 'C';
const char c_type_directory = 'D';

const int c_buffer_size = 65536;
const int c_progress_lines = 250;

const char* const c_zlib_extension = ".gz";
const char* const c_default_extension = ".bun";

MD5 g_md5;

string unescaped_line( const string& line )
{
   string str;
   str.reserve( line.size( ) );

   bool is_escape = false;
   for( size_t i = 0; i < line.size( ); i++ )
   {
      char c = line[ i ];

      if( is_escape )
      {
         is_escape = false;

         if( c == 'n' )
            c = '\n';
      }
      else if( c == '\\' )
         is_escape = true;
      else if( c == '\n' )
         break;

      if( !is_escape )
         str += c;
   }

   return str;
}

#ifdef ZLIB_SUPPORT
bool read_zlib_line( gzFile& gzf, string& s )
{
   char c;
   s.erase( );

   bool is_escape = false;
   while( true )
   {
      if( gzread( gzf, &c, sizeof( char ) ) <= 0 )
      {
         if( gzeof( gzf ) )
            break;
         else
            throw runtime_error( "reading char" );
      }

      if( is_escape )
      {
         is_escape = false;

         if( c == 'n' )
            c = '\n';
      }
      else if( c == '\\' )
         is_escape = true;
      else if( c == '\n' )
         break;

      if( !is_escape )
         s += c;
   }

   return !s.empty( );
}
#endif

#ifndef ZLIB_SUPPORT
void check_file_header( ifstream& inpf, const string& filename, encoding_type& encoding )
#else
void check_file_header( ifstream& inpf, const string& filename, encoding_type& encoding, bool use_zlib, gzFile& gzf )
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
      encoding = e_encoding_type_b64;
   else if( header == "ESC" )
      encoding = e_encoding_type_esc;
#ifdef ZLIB_SUPPORT
   else if( header == "RAW" )
      encoding = e_encoding_type_raw;
#else
   else if( header == "RAW" )
      throw runtime_error( "*** invalid RAW bundle data encoding type found in -ngz bundle '" + filename + "' ***" );
#endif
   else
      throw runtime_error( "*** unknown bundle data encoding type " + header + " in '" + filename + "' ***" );
}

void create_all_directories( deque< string >& create_directories,
 const map< string, string >& directory_perms, const string& destination_directory, bool list_only, bool is_quieter )
{
   while( !create_directories.empty( ) )
   {
      string path_name( create_directories.front( ) );
      if( list_only )
         cout << path_name << "/" << endl;
      else
      {
         if( !directory_perms.count( path_name ) )
            throw runtime_error( "unexpected missing directory perms for '" + path_name + "'" );

         string full_path = destination_directory + path_name;
#ifdef _WIN32
         int rc = _mkdir( full_path.c_str( ) );
#else
         int um = umask( 0 );
         int rc = _mkdir( full_path.c_str( ), S_IRWXU );
         umask( um );
#endif

         if( rc < 0 && errno != EEXIST )
            throw runtime_error( "unable to create directory '" + full_path + "'" );
         else if( rc >= 0 || errno != EEXIST )
         {
            file_perms( full_path, directory_perms.find( path_name )->second );

            if( !is_quieter )
               cout << "created directory '" << full_path << "'..." << endl;
         }
      }

      create_directories.pop_front( );
   }
}

int main( int argc, char* argv[ ] )
{
   int first_arg = 0;
   bool junk = false;
   bool prune = false;
   bool include = false;
   bool use_zlib = false;
   bool is_quiet = false;
   bool list_only = false;
   bool overwrite = false;
   bool is_quieter = false;

   if( argc > first_arg + 1 )
   {
      if( string( argv[ first_arg + 1 ] ) == "-i" )
      {
         ++first_arg;
         include = true;
      }
      else if( string( argv[ first_arg + 1 ] ) == "-j" )
      {
         ++first_arg;
         junk = true;
      }
   }

   if( argc > first_arg + 1 )
   {
      if( string( argv[ first_arg + 1 ] ) == "-l" )
      {
         ++first_arg;
         list_only = true;
      }
   }

   if( argc > first_arg + 1 )
   {
      if( string( argv[ first_arg + 1 ] ) == "-o" )
      {
         ++first_arg;
         overwrite = true;
      }
   }

   if( argc > first_arg + 1 )
   {
      if( string( argv[ first_arg + 1 ] ) == "-p" )
      {
         ++first_arg;
         prune = true;
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
      if( string( argv[ first_arg + 1 ] ) == "-qq" )
      {
         ++first_arg;
         is_quiet = true;
         is_quieter = true;
      }
   }

   if( !is_quiet )
      cout << "unbundle v0.1f\n";

   if( ( argc - first_arg < 2 )
    || string( argv[ 1 ] ) == "?" || string( argv[ 1 ] ) == "/?" || string( argv[ 1 ] ) == "-?" )
   {
      cout << "usage: unbundle [-i|-j] [-l] [-o] [-p] [-q[q]] <fname> [<fspec1> [<fspec2> [...]]] [-x <fspec1> [...]] [-d <directory>]" << endl;

      cout << "\nwhere: -i to include top level directory and -j to junk all directories" << endl;
      cout << "  and: -l to list rather than create all matching files and directories" << endl;
      cout << "  and: -o to overwrite existing files and -p to prune empty directories" << endl;
      cout << "  and: -q for quiet mode (-qq to suppress all output apart from errors)" << endl;
      cout << "  and: -x identifies one or more filespecs that are to be excluded" << endl;
      cout << " also: -d <directory> to set a directory origin for output" << endl;
      return 0;
   }

#ifdef __GNUG__
   umask( DEFAULT_UMASK );
#endif

   try
   {
      string filename( argv[ first_arg + 1 ] );

      if( !filename.empty( ) && filename[ 0 ] == '-' )
         throw runtime_error( "unknown or bad option '" + filename + "' use -? to see options" );

      if( filename.length( ) > 3
       && filename.substr( filename.length( ) - 3 ) == string( c_zlib_extension ) )
      {
#ifdef ZLIB_SUPPORT
         use_zlib = true;
#else
         throw runtime_error( "this program has not been compiled with ZLIB support" );
#endif
      }

      string::size_type pos = filename.find( '.' );

      if( pos == string::npos || _access( filename.c_str( ), 0 ) != 0 )
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

      if( !is_quiet && !list_only )
         cout << "==> started unbundling '" << filename << "'\n";

      bool get_exclude_filespecs = false;

      string destination_directory;
      bool get_destination_directory = false;

      vector< string > filename_filters;
      vector< string > exclude_filename_filters;
      for( int i = first_arg + 2; i < argc; i++ )
      {
         string next( argv[ i ] );

         if( !next.empty( ) && next[ 0 ] == '-' )
         {
            if( next == "-d" && !get_destination_directory )
            {
               next.erase( );
               get_destination_directory = true;
            }
            else if( next == "-x" && !get_exclude_filespecs )
            {
               next.erase( );
               get_exclude_filespecs = true;
            }
            else
               throw runtime_error( "unknown or bad option '" + next + "' use -? to see options" );
         }

         if( !next.empty( ) )
         {
            if( get_destination_directory && destination_directory.empty( ) )
            {
               destination_directory = next;
#ifndef _WIN32
               if( destination_directory[ destination_directory.size( ) - 1 ] != '/' )
                  destination_directory += '/';
#else
               if( destination_directory[ destination_directory.size( ) - 1 ] != '/'
                && destination_directory[ destination_directory.size( ) - 1 ] != '\\'
                && destination_directory[ destination_directory.size( ) - 1 ] != ':' )
                  destination_directory += '/';

               replace( destination_directory, "\\", "/" );
#endif
            }
            else if( get_exclude_filespecs )
               exclude_filename_filters.push_back( next );
            else
               filename_filters.push_back( next );
         }
      }

      int level = -1;
      bool is_first = false;

      string next_file;
      string rwx_perms;
      int line_size = 0;
      int file_data_lines = 0;
      int64_t raw_file_size = 0;

      stack< string > paths;
      deque< string > create_directories;
      map< string, string > directory_perms;

      set< string > created;
      set< string > matched_filters;

      auto_ptr< ofstream > ap_ofstream;

      MD5 md5;
      string next;
      string next_md5;
      size_t line = 1;
      size_t count = 0;

      bool finished = false;
      bool replace_all = false;
      bool replace_none = false;

      encoding_type encoding;
      string top_level_directory;

      int progress = c_progress_lines;

      // NOTE: If any filespecs were provided then automatically prune empty directories.
      if( !filename_filters.empty( ) )
         prune = true;

#ifndef ZLIB_SUPPORT
      check_file_header( inpf, filename, encoding );
#else
      check_file_header( inpf, filename, encoding, use_zlib, gzf );
#endif

      while( true )
      {
         if( raw_file_size )
         {
            if( !ap_ofstream.get( ) )
            {
#ifdef ZLIB_SUPPORT
               if( use_zlib )
                  gzseek( gzf, raw_file_size, SEEK_CUR );
               else
                  inpf.seekg( raw_file_size, ios::cur );
#else
               inpf.seekg( raw_file_size, ios::cur );
#endif
               raw_file_size = 0;
            }
            else
            {
               int64_t chunk = 0;
               while( raw_file_size > 0 )
               {
                  char buffer[ c_buffer_size ];

                  int count = c_buffer_size;
                  if( raw_file_size < c_buffer_size )
                     count = raw_file_size;

#ifdef ZLIB_SUPPORT
                  if( use_zlib )
                  {
                     if( !gzread( gzf, buffer, count ) )
                        throw runtime_error( "reading zlib input" );
                  }
                  else
                  {
                     if( inpf.rdbuf( )->sgetn( buffer, count ) != count )
                        throw runtime_error( "reading file input" );
                  }
#else
                  if( inpf.rdbuf( )->sgetn( buffer, count ) != count )
                     throw runtime_error( "reading file input" );
#endif

                  if( !is_quieter && ++chunk % c_progress_lines == 0 )
                  {
                     if( line == c_progress_lines )
                        cout << ' ';
                     cout << '.';
                     cout.flush( );
                  }

                  md5.update( ( unsigned char* )buffer, count );

                  ap_ofstream->rdbuf( )->sputn( buffer, count );
                  if( !ap_ofstream->good( ) )
                     throw runtime_error( "unexpected bad output file stream" );

                  raw_file_size -= count;
               }

               md5.finalize( );
               auto_ptr< char > ap_digest( md5.hex_digest( ) );

               if( next_md5 != string( ap_digest.get( ) ) )
                  cerr << "*** error: file '" << next_file << "' failed MD5 digest check ***" << endl;

               ap_ofstream.reset( );
               file_perms( next_file, rwx_perms );

               if( !is_quieter )
                  cout << endl;

               continue;
            }
         }

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

         if( next.empty( ) )
            throw runtime_error( "unexpected empty line #" + to_string( line ) );

         if( file_data_lines )
         {
            --file_data_lines;

            if( count == 0 )
            {
               line_size = unescaped( next ).size( );

               if( line_size >= 1048576 ) // i.e. 1 MB
                  progress = 2;
               else
                  progress = c_progress_lines;
            }

            // NOTE: If skipping a file then there is no need to actually
            // read the data so by determining the line size of the first
            // line (after unescaping) it is safe to "seek" one byte less
            // forwards and then read the remainder of the line (the size
            // of this will depend upon how much escaping is being used).
            if( !ap_ofstream.get( ) && line_size && file_data_lines > 1 )
            {
#ifdef ZLIB_SUPPORT
               if( use_zlib )
                  gzseek( gzf, line_size - 1, SEEK_CUR );
               else
                  inpf.seekg( line_size - 1, ios::cur );
#else
               inpf.seekg( line_size - 1, ios::cur );
#endif
            }

            if( ++count % progress == 0 && !is_quieter && ap_ofstream.get( ) )
            {
               if( count == progress )
                  cout << ' ';
               cout << '.';
               cout.flush( );
            }

            if( !is_quieter && ap_ofstream.get( ) && file_data_lines == 0 )
               cout << endl;

            if( ap_ofstream.get( ) )
            {
               string fdata;

               if( use_zlib )
                  fdata = next;
               else if( encoding != e_encoding_type_b64 )
                  fdata = unescaped_line( next );
               else
                  fdata = base64::decode( next );

               if( ap_ofstream->rdbuf( )->sputn( fdata.c_str( ), fdata.length( ) ) != ( int )fdata.length( ) )
                  throw runtime_error( "write failed for file '" + next_file + "'" );

               md5.update( ( unsigned char* )fdata.c_str( ), fdata.length( ) );

               if( file_data_lines == 0 )
               {
                  ap_ofstream->flush( );
                  if( !ap_ofstream->good( ) )
                     throw runtime_error( "flush failed for file '" + next_file + "'" );
                  ap_ofstream->close( );

                  ap_ofstream.reset( );
                  file_perms( next_file, rwx_perms );

                  md5.finalize( );
                  auto_ptr< char > ap_digest( md5.hex_digest( ) );

                  if( next_md5 != string( ap_digest.get( ) ) )
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

            if( encoding == e_encoding_type_raw )
               raw_file_size = unformat_bytes( next.substr( 0, pos ) );
            else
            {
               int num_lines( atoi( next.substr( 0, pos ).c_str( ) ) );
               if( num_lines < 0 )
                  throw runtime_error( "invalid number of lines "
                   + to_string( num_lines ) + " specified in line #" + to_string( line ) );

               file_data_lines = num_lines;
            }

            next.erase( 0, pos + 1 );

            string::size_type pos = next.find_last_of( " " );
            if( pos == string::npos )
               throw runtime_error( "unexpected file entry format in line #" + to_string( line ) );

            next_md5 = next.substr( pos + 1 );
            next.erase( pos );

            g_md5.update( ( unsigned char* )next_md5.c_str( ), next_md5.length( ) );

            pos = next.find( ' ' );
            if( pos == string::npos )
               throw runtime_error( "unexpected file entry format in line #" + to_string( line ) );

            rwx_perms = next.substr( 0, pos );
            next.erase( 0, pos + 1 );

            string test_file( next );
            if( !paths.empty( ) )
               test_file = paths.top( ) + "/" + next;

            if( junk )
               next_file = destination_directory + next;
            else
               next_file = destination_directory + test_file;

            md5.update( ( unsigned char* )rwx_perms.c_str( ), rwx_perms.length( ) );
            md5.update( ( unsigned char* )next.c_str( ), next.length( ) );

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
                     matched_filters.insert( filename_filters[ i ] );

                     break;
                  }
               }
            }

            if( !exclude_filename_filters.empty( ) )
            {
               for( size_t i = 0; i < exclude_filename_filters.size( ); i++ )
               {
                  string wildcard( exclude_filename_filters[ i ] );

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
                     matched = false;
                     break;
                  }
               }
            }

            if( matched && !list_only && !overwrite && !replace_all && file_exists( next_file ) )
            {
               bool replace = false;
               if( !is_quiet && !replace_none )
               {
                  char ch;
                  string prompt( "File '" + next_file + "' already exists. Replace [y/n/A/N]? " );
                  while( true )
                  {
                     ch = get_char( prompt.c_str( ) );

                     if( ch == 'A' )
                        replace_all = true;
                     else if( ch == 'N' )
                        replace_none = true;

                     if( ch == 'y' || ch == 'A' )
                        replace = true;

                     if( replace || replace_all || replace_none || ch == 'n' )
                     {
                        cout << ch << '\n';
                        break;
                     }
                  }
               }

               if( !replace )
               {
                  matched = false;

                  if( is_quieter )
                     cerr << "*** error: file '" << next_file << "' already exists ***" << endl;
               }
            }

            count = 0;

            if( matched )
               create_all_directories( create_directories,
                directory_perms, destination_directory, list_only, is_quieter );

            if( !matched )
               ap_ofstream.reset( );
            else if( list_only )
            {
               ap_ofstream.reset( );
               cout << next_file << endl;
            }
            else
            {
               if( !is_quieter )
                  cout << "extracting \"" << next_file << "\"";

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

               if( !include || create_directories.size( ) > 1 )
               {
                  size_t test_level( next_level );
                  if( next_level > 0 && !include )
                     --test_level;

                  while( create_directories.size( ) > test_level )
                     create_directories.pop_back( );
               }

               level = next_level;

               while( ( int )paths.size( ) > level )
                  paths.pop( );
            }

            next.erase( 0, pos + 1 );

            pos = next.find( ' ' );
            if( pos == string::npos )
               throw runtime_error( "unexpected format in line #" + to_string( line ) );

            string rwx_perms( next.substr( 0, pos ) );
            next.erase( 0, pos + 1 );

            string path_name( next );

            if( top_level_directory.empty( ) )
               top_level_directory = path_name;

            MD5 md5;
            md5.update( ( unsigned char* )rwx_perms.c_str( ), rwx_perms.length( ) );
            md5.update( ( unsigned char* )path_name.c_str( ), path_name.length( ) );
            md5.finalize( );

            auto_ptr< char > ap_digest( md5.hex_digest( ) );

            if( check != string( ap_digest.get( ) ) )
               cerr << "*** error: directory '" << path_name << "' failed MD5 digest check ***" << endl;

            g_md5.update( ( unsigned char* )ap_digest.get( ), 32 );

            if( include || level > 0 )
            {
               if( !include )
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
                  directory_perms.insert( make_pair( path_name, rwx_perms ) );

                  if( !prune )
                     create_all_directories( create_directories,
                      directory_perms, destination_directory, list_only, is_quieter );
               }

               paths.push( path_name );
            }

            is_first = false;
         }
         else if( type == c_type_checksum )
         {
            g_md5.finalize( );
            auto_ptr< char > ap_digest( g_md5.hex_digest( ) );

            next.erase( 0, 2 );

            if( next != string( ap_digest.get( ) ) )
               cerr << "*** error: bundle file failed MD5 digest check ***" << endl;

            finished = true;
         }
         else
            throw runtime_error( "unexpected entry type '" + to_string( type ) + "' found in line #" + to_string( line ) );
      }

      if( !is_quieter )
      {
         for( size_t i = 0; i < filename_filters.size( ); i++ )
         {
            if( matched_filters.count( filename_filters[ i ] ) == 0 )
               cout << "warning: found no files matching '" << filename_filters[ i ] << "'" << endl;
         }
      }

      if( !finished )
         cerr << "*** error: final MD5 digest not found (file truncated?) ***" << endl;

#ifdef ZLIB_SUPPORT
      if( use_zlib && !gzeof( gzf ) )
         throw runtime_error( "unexpected error occurred whilst reading '" + filename + "' for input" );
#endif
      if( !use_zlib && !inpf.eof( ) )
         throw runtime_error( "unexpected error occurred whilst reading '" + filename + "' for input" );

      if( !is_quiet && !list_only )
         cout << "==> finished unbundling '" << filename << "'" << endl;
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
      return 1;
   }

   return 0;
}

