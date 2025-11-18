// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <map>
#  include <memory>
#  include <fstream>
#  include <sstream>
#  include <iostream>
#  include <stdexcept>
#  ifdef __GNUG__
#     include <limits.h>
#     include <unistd.h>
#  endif
#endif

#include "md5.h"
#include "base64.h"
#include "config.h"
#include "format.h"
#include "date_time.h"
#include "utilities.h"
#include "fs_iterator.h"

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

const int c_max_bytes_per_line = 96; // (for 128 characters)

const char* const c_app_title = "bundle";
const char* const c_app_version = "0.1i";

const char* const c_zlib_extension = ".gz";
const char* const c_default_extension = ".bun";

const char* const c_delete_dummy_path = "<delete>";

MD5 g_md5;

string g_cwd;
string g_bundle_file_name;
string g_output_file_name;

set< string > g_output_directories;

bool is_root_path( const string& absolute_path )
{
   if( absolute_path == "/" )
      return true;

   return false;
}

string escaped_line( const string& line )
{
   string str;
   str.reserve( line.size( ) * 2 );

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
         throw runtime_error( "reading zlib char" );

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
      if( !gzwrite( gzf, str.c_str( ), str.size( ) ) )
         throw runtime_error( "writing zlib string" );

      if( !gzwrite( gzf, "\n", 1 ) )
         throw runtime_error( "writing zlib linefeed" );
   }
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
   else if( header == "RAW" )
      encoding = e_encoding_type_raw;
   else
      throw runtime_error( "*** unknown bundle data encoding type " + header + " in '" + filename + "' ***" );
}

#ifndef ZLIB_SUPPORT
void output_directory( set< string >& file_names,
 const string& path_name, const string& path_prefix, int level, ostream& os )
#else
void output_directory( set< string >& file_names,
 const string& path_name, const string& path_prefix, int level, ostream& os, bool use_zlib, gzFile& gzf )
#endif
{
   string::size_type pos = path_name.find_last_of( '/' );
   if( level > 1 && pos != string::npos && !g_output_directories.count( path_name.substr( 0, pos ) ) )
   {
#ifndef ZLIB_SUPPORT
      output_directory( file_names, path_name.substr( 0, pos ), path_prefix, level - 1, os );
#else
      output_directory( file_names, path_name.substr( 0, pos ), path_prefix, level - 1, os, use_zlib, gzf );
#endif
   }

   if( !file_names.count( path_name ) )
   {
      string perms;

      if( path_prefix.empty( ) )
         perms = file_perms( path_name );
      else
         perms = file_perms( path_prefix + '/' + path_name );

      MD5 md5;
      md5.update( ( unsigned char* )perms.c_str( ), perms.length( ) );
      md5.update( ( unsigned char* )path_name.c_str( ), path_name.length( ) );
      md5.finalize( );

      auto_ptr< char > ap_digest( md5.hex_digest( ) );

      ostringstream osstr;
      osstr << "D " << level << ' ' << perms << ' ' << path_name << ' ' << ap_digest.get( );

      g_md5.update( ( unsigned char* )ap_digest.get( ), 32 );

#ifndef ZLIB_SUPPORT
      os << osstr.str( ) << '\n';
#else
      if( !use_zlib )
         os << osstr.str( ) << '\n';
      else
         write_zlib_line( gzf, osstr.str( ) );
#endif

      file_names.insert( path_name );
   }

   g_output_directories.insert( path_name );
}

#ifndef ZLIB_SUPPORT
void process_directory( const string& directory, const string& filespec_path,
 const vector< string >& filename_filters, const vector< string >* p_filename_exclusions,
 set< string >& matched_filters, set< string >& file_names, bool recurse,
 bool prune, bool is_quieter, bool is_append, encoding_type encoding, ostream& os )
#else
void process_directory( const string& directory, const string& filespec_path,
 const vector< string >& filename_filters, const vector< string >* p_filename_exclusions,
 set< string >& matched_filters, set< string >& file_names, bool recurse, bool prune,
 bool is_quieter, bool is_append, encoding_type encoding, ostream& os, bool use_zlib, gzFile& gzf )
#endif
{
   directory_filter df;
   fs_iterator dfsi( filespec_path, &df );

   string::size_type pos = filespec_path.find_last_of( '/' );

   string path_prefix;

   bool all_filters_have_dirs = !filename_filters.empty( );

   for( int i = 0; i < filename_filters.size( ); i++ )
   {
      string next_filter( filename_filters[ i ] );

      string::size_type dpos = next_filter.find( '/' );
      string::size_type wpos = next_filter.find_first_of( "?*" );

      if( ( dpos == string::npos )
       || ( ( wpos != string::npos ) && ( wpos < dpos ) ) )
      {
         all_filters_have_dirs = false;
         break;
      }
   }

   if( pos != string::npos )
      path_prefix = filespec_path.substr( 0, pos );

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

      bool has_output_directory = false;

      // NOTE: If every filter has a directory prefix (with no wildcards) then
      // will treat processing as though the "prune" option had been provided.
      if( ( !prune && !all_filters_have_dirs ) || ( dfsi.get_level( ) == 0 ) )
      {
         has_output_directory = true;
#ifndef ZLIB_SUPPORT
         output_directory( file_names, path_name, path_prefix, dfsi.get_level( ), os );
#else
         output_directory( file_names, path_name, path_prefix, dfsi.get_level( ), os, use_zlib, gzf );
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

         if( p_filename_exclusions )
         {
            for( int j = 0; j < p_filename_exclusions->size( ); j++ )
            {
               string next_filter( ( *p_filename_exclusions )[ j ] );

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
                  matched = false;
                  break;
               }
            }
         }

         if( !matched )
            continue;

         string absolute_file_name;
         absolute_path( ffsi.get_full_name( ), absolute_file_name );

         if( absolute_file_name == g_bundle_file_name || absolute_file_name == g_output_file_name )
            continue;

         if( !has_output_directory )
         {
            has_output_directory = true;
#ifndef ZLIB_SUPPORT
            output_directory( file_names, path_name, path_prefix, dfsi.get_level( ), os );
#else
            output_directory( file_names, path_name, path_prefix, dfsi.get_level( ), os, use_zlib, gzf );
#endif
         }

         if( !is_quieter )
         {
            string next_path( path_name );

            if( next_path.find( directory ) == 0 )
            {
               next_path.erase( 0, directory.size( ) );
               if( !next_path.empty( ) && next_path[ 0 ] == '/' )
                  next_path.erase( 0, 1 );
            }

            if( !next_path.empty( ) )
               next_path += "/";

            cout << "adding \"" << next_path << ffsi.get_name( ) << "\"";
         }

         int64_t size = file_size( ffsi.get_full_name( ) );
         string perms = file_perms( ffsi.get_full_name( ) );

         string fname( ffsi.get_name( ) );

         ifstream inpf( ffsi.get_full_name( ).c_str( ), ios::in | ios::binary );
         if( !inpf )
            throw runtime_error( "unable to open file '" + ffsi.get_full_name( ) + "' for input" );

         MD5 md5;
         unsigned char buffer[ c_buffer_size ];

         md5.update( ( unsigned char* )perms.c_str( ), perms.length( ) );
         md5.update( ( unsigned char* )fname.c_str( ), fname.length( ) );

         bool initial_progress = true;

         date_time dtm( date_time::local( ) );

         int64_t size_left = size;

         while( size_left )
         {
            int next_len = c_buffer_size;
            if( size_left < c_buffer_size )
               next_len = ( int )size_left;

            inpf.read( ( char* )buffer, next_len );

            int len = inpf.gcount( );
            md5.update( buffer, len );

            size_left -= next_len;

            if( !is_quieter )
            {
               date_time now( date_time::local( ) );

               uint64_t elapsed = seconds_between( dtm, now );

               if( elapsed >= 1 )
               {
                  if( initial_progress )
                     cout << ' ';

                  cout << '.';
                  cout.flush( );

                  dtm = now;
                  initial_progress = false;
               }
            }
         }

         md5.finalize( );
         inpf.seekg( 0, ios::beg );

         int max_size = c_max_bytes_per_line;
         if( encoding != e_encoding_type_b64 )
            max_size = c_buffer_size;

         int64_t num;
         if( encoding == e_encoding_type_raw )
            num = size;
         else
         {
            num = size / max_size;
            if( size % max_size )
               ++num;
         }

         auto_ptr< char > ap_digest( md5.hex_digest( ) );

         ostringstream osstr;
         osstr << "F " << num << ' ' << perms << ' ' << fname << ' ' << ap_digest.get( );

         file_names.insert( ffsi.get_name( ) );

         g_md5.update( ( unsigned char* )ap_digest.get( ), 32 );

#ifndef ZLIB_SUPPORT
         os << osstr.str( ) << '\n';
#else
         if( !use_zlib )
            os << osstr.str( ) << '\n';
         else
            write_zlib_line( gzf, osstr.str( ) );
#endif

         int line = 0;

         while( size > 0 )
         {
            char buffer[ c_buffer_size ];

            streamsize count = inpf.rdbuf( )->sgetn( buffer, max_size );

            if( count == 0 )
               throw runtime_error( "read failed for file '" + ffsi.get_full_name( ) + "'" );

            if( !is_quieter )
            {
               date_time now( date_time::local( ) );

               uint64_t elapsed = seconds_between( dtm, now );

               if( elapsed >= 1 )
               {
                  if( initial_progress )
                     cout << ' ';

                  cout << '.';
                  cout.flush( );

                  dtm = now;
                  initial_progress = false;
               }
            }

            string encoded;
            if( encoding == e_encoding_type_esc )
               encoded = escaped_line( string( buffer, count ) );
            else if( encoding == e_encoding_type_b64 )
               encoded = base64::encode( string( buffer, count ) );

#ifndef ZLIB_SUPPORT
            if( encoding != e_encoding_type_raw )
               os << encoded << '\n';
            else
               os.rdbuf( )->sputn( buffer, count );

            if( !os.good( ) )
               throw runtime_error( "unexpected bad output file stream" );
#else
            if( !use_zlib )
            {
               if( encoding != e_encoding_type_raw )
                  os << encoded << '\n';
               else
                  os.rdbuf( )->sputn( buffer, count );

               if( !os.good( ) )
                  throw runtime_error( "unexpected bad output file stream" );
            }
            else
            {
               if( encoding != e_encoding_type_raw )
                  write_zlib_line( gzf, encoded );
               else if( !gzwrite( gzf, buffer, count ) )
                  throw runtime_error( "writing zlib block" );
            }
#endif

            size -= count;
         }

         if( !is_quieter )
            cout << endl;
      }
   } while( recurse && dfsi.has_next( ) );
}

int main( int argc, char* argv[ ] )
{
   int first_arg = 0;

   bool prune = false;
   bool is_cout = false;
   bool recurse = false;
   bool invalid = false;
   bool is_quiet = false;
   bool use_zlib = true;
   bool is_delete = false;
   bool is_quieter = false;
   bool skip_warns = false;

   string open_mode( "wb" );

#ifndef ZLIB_SUPPORT
   encoding_type encoding = e_encoding_type_esc;
#else
   encoding_type encoding = e_encoding_type_raw;
#endif

   if( argc > first_arg + 1 )
   {
      if( string( argv[ first_arg + 1 ] ) == "-0" )
      {
         ++first_arg;
         open_mode += "0";
      }
      else if( string( argv[ first_arg + 1 ] ) == "-1" )
      {
         ++first_arg;
         open_mode += "1";
      }
      else if( string( argv[ first_arg + 1 ] ) == "-9" )
      {
         ++first_arg;
         open_mode += "9";
      }
   }

   if( argc > first_arg + 1 )
   {
      if( string( argv[ first_arg + 1 ] ) == "-d" )
      {
         ++first_arg;
         is_delete = true;
      }
   }

   if( !is_delete )
   {
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
      if( string( argv[ first_arg + 1 ] ) == "-qw" )
      {
         ++first_arg;
         is_quiet = true;
         skip_warns = true;
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

   if( argc > first_arg + 1 )
   {
      if( string( argv[ first_arg + 1 ] ) == "-b64" )
      {
         ++first_arg;
         encoding = e_encoding_type_b64;
      }
      else if( string( argv[ first_arg + 1 ] ) == "-esc" )
      {
         ++first_arg;
         encoding = e_encoding_type_esc;
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

   if( invalid || ( argc - first_arg < 2 )
    || string( argv[ 1 ] ) == "?" || string( argv[ 1 ] ) == "/?" || string( argv[ 1 ] ) == "-?" )
   {
      if( !is_quiet )
         cout << c_app_title << " v" << c_app_version << "\n";
#ifndef ZLIB_SUPPORT
      cout << "usage: bundle [-0|-1|-9] [-d]|[-r [-p]] [-q[w|q]] [-b64|-esc] <fname> [<fspec1> [<fspec2> [...]]] [-x <fspec1> [...]]" << endl;
#else
      cout << "usage: bundle [-0|-1|-9] [-d]|[-r [-p]] [-q[w|q]] [-b64|-esc] [-ngz] <fname> [<fspec1> [<fspec2> [...]]] [-x <fspec1> [...]]" << endl;
#endif

      cout << "\nwhere: -0/-1/-9 is used for setting zero/fastest/best compression level" << endl;
      cout << "  and: -d is to delete matching files which exist in an existing bundle" << endl;
      cout << "  and: -r is to recurse sub-directories (-p to prune empty directories)" << endl;
      cout << "  and: -q for quiet mode (-qw omit warnings and -qq only output errors)" << endl;
      cout << "  and: -b64/-esc stores file data using b64/esc encoding for text lines" << endl;
#ifdef ZLIB_SUPPORT
      cout << "  and: -ngz in order to not perform zlib compression (similar to 'tar')" << endl;
#endif
      cout << " also: -x identifies one or multiple filespecs which are to be excluded" << endl;
      return 0;
   }

   try
   {
      g_cwd = get_cwd( );

      string::size_type pos;

      pos = g_cwd.find_last_of( '/' );

      if( is_root_path( g_cwd ) )
         throw runtime_error( "cannot created a bundle in the root directory" );

      map< string, vector< string > > all_filespecs;
      map< string, vector< string > > all_exclude_filespecs;

      string filename( argv[ first_arg + 1 ] );

      if( filename.length( ) > 1 && filename[ 0 ] == '-' )
         throw runtime_error( "unknown or bad option '" + filename + "' use -? to see options" );

      if( filename == "-" )
         is_cout = is_quiet = is_quieter = true;
      else
      {
         if( !is_quiet )
            cout << c_app_title << " v" << c_app_version << "\n";

         string ext( c_default_extension );
         if( use_zlib )
            ext += c_zlib_extension;

         if( filename.find( ext ) == string::npos )
            filename += ext;
      }

      bool get_exclude_filespecs = false;
      string directory = g_cwd.substr( pos + 1 );

      if( is_delete )
      {
         for( int i = first_arg + 2; i < argc; i++ )
            all_filespecs[ c_delete_dummy_path ].push_back( string( argv[ i ] ) );
      }
      else
      {
         for( int i = first_arg + 2; i < argc; i++ )
         {
            string next( argv[ i ] );

            if( !next.empty( ) && next[ 0 ] == '-' )
            {
               if( next == "-x" && !get_exclude_filespecs )
               {
                  next.erase( );
                  get_exclude_filespecs = true;
               }
               else
                  throw runtime_error( "unknown or bad option '" + next + "' use -? to see options" );
            }

            if( !next.empty( ) )
            {
               string::size_type wpos = next.find_first_of( "?*" );

               string filespec_path;

               if( wpos == 0 )
               {
                  filespec_path = g_cwd + "/" + next;
                  wpos = string::npos;
               }
               else
               {
                  bool rc = false;

                  string name( next.substr( 0, wpos ) );

                  absolute_path( name, filespec_path, &rc );

                  if( !rc )
                  {
                     if( get_exclude_filespecs )
                        continue;
                     else
                        throw runtime_error( "unable to determine absolute path for fspec '" + name + "'" );
                  }
               }

               if( is_root_path( filespec_path ) )
                  throw runtime_error( "cannot bundle directory '" + next + "' (need to specify a non-root directory)" );

               if( wpos != string::npos )
                  filespec_path += "/" + next.substr( wpos );

               string::size_type rpos = filespec_path.find_last_of( '/' );

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

               if( !get_exclude_filespecs )
                  all_filespecs[ filespec_path ].push_back( filename_filter );
               else
                  all_exclude_filespecs[ filespec_path ].push_back( filename_filter );
            }
         }

         if( all_filespecs.empty( ) )
            all_filespecs[ g_cwd ].push_back( "*" );
      }

      int num_deleted = 0;

      set< string > file_names;
      set< string > matched_filters;

      bool is_append = false;
      if( !is_cout && file_exists( filename ) )
         is_append = true;

      if( is_delete && first_arg + 2 >= argc )
         throw runtime_error( "cannot delete files without specifying at least one filespec" );

      if( is_delete && !is_append )
         throw runtime_error( "no bundle file '" + filename + "' was found" );

      string output_filename( filename );
      if( is_append )
         output_filename = output_filename + ".tmp";

      // NOTE: Empty code block for scope purposes.
      {
         ostream* p_os = 0;
#ifndef ZLIB_SUPPORT
         ofstream outf;

         if( is_cout )
            p_os = &cout;
         else
         {
            outf.open( output_filename.c_str( ) );

            if( !outf )
               throw runtime_error( "unable to open file '" + output_filename + "' for output" );

            p_os = &outf;
         }
#else
         gzFile gzf;
         ofstream outf;

         if( !use_zlib )
         {
            if( is_cout )
               p_os = &cout;
            else
            {
               outf.open( output_filename.c_str( ) );

               if( !outf )
                  throw runtime_error( "unable to open file '" + output_filename + "' for output" );

               p_os = &outf;
            }
         }
         else
         {
            if( is_cout )
               gzf = gzdopen( fileno( stdout ), open_mode.c_str( ) );
            else
               gzf = gzopen( output_filename.c_str( ), open_mode.c_str( ) );
         }
#endif

         if( !is_quiet )
         {
            if( !is_append )
               cout << "==> started bundling '" << filename << "'\n";
            else
               cout << "==> continue bundling '" << filename << "'\n";
         }

         absolute_path( filename, g_bundle_file_name );
         absolute_path( output_filename, g_output_file_name );

         string header( "B " );
         header += to_string( c_format_version );

         switch( encoding )
         {
            case e_encoding_type_b64:
            header += " B64";
            break;

            case e_encoding_type_esc:
            header += " ESC";
            break;

            case e_encoding_type_raw:
            header += " RAW";
            break;
         }

#ifndef ZLIB_SUPPORT
         *p_os << header << '\n';
#else
         if( !use_zlib )
            *p_os << header << '\n';
         else
            write_zlib_line( gzf, header );
#endif

         if( !is_delete )
         {
            for( map< string, vector< string > >::iterator i = all_filespecs.begin( ); i != all_filespecs.end( ); ++i )
            {
               string filespec_path( i->first );
               vector< string >& filename_filters( i->second );

               vector< string >* p_filename_exclusions = 0;
               if( all_exclude_filespecs.count( i->first ) )
                  p_filename_exclusions = &all_exclude_filespecs[ i->first ];

               string next_directory( directory );

               string::size_type pos = filespec_path.find_last_of( '/' );

               if( pos != string::npos && pos != filespec_path.length( ) - 1 )
                  next_directory = filespec_path.substr( pos + 1 );

#ifndef ZLIB_SUPPORT
               process_directory( next_directory, filespec_path, filename_filters, p_filename_exclusions,
                matched_filters, file_names, recurse, prune, is_quieter, is_append, encoding, *p_os );
#else
               process_directory( next_directory, filespec_path, filename_filters, p_filename_exclusions,
                matched_filters, file_names, recurse, prune, is_quieter, is_append, encoding, *p_os, use_zlib, gzf );
#endif
               if( !skip_warns && !is_quieter )
               {
                  for( size_t i = 0; i < filename_filters.size( ); i++ )
                  {
                     if( matched_filters.count( filename_filters[ i ] ) == 0 )
                        cout << "warning: found no files matching '" << filename_filters[ i ] << "'" << endl;
                  }
               }
            }
         }

         if( is_append )
         {
#ifdef ZLIB_SUPPORT
            gzFile igzf;
            ifstream inpf;

            if( !use_zlib )
               inpf.open( filename.c_str( ) );
            else
               igzf = gzopen( filename.c_str( ), "rb" );

            if( ( use_zlib && !igzf ) || ( !use_zlib && !inpf ) )
               throw runtime_error( "unable to open file '" + filename + "' for input" );
#else
            ifstream inpf( filename.c_str( ) );
            if( !inpf )
               throw runtime_error( "unable to open file '" + filename + "' for input" );
#endif

            encoding_type old_encoding;
#ifndef ZLIB_SUPPORT
            check_file_header( inpf, filename, old_encoding );
#else
            check_file_header( inpf, filename, old_encoding, use_zlib, igzf );
#endif

            if( old_encoding != encoding )
               throw runtime_error( "*** cannot combine different bundle data encoding types ***" );

            int line = 0;
            int count = 0;
            int line_size = 0;
            int file_data_lines = 0;

            int64_t raw_file_size = 0;

            bool initial_progress = true;
            bool skip_existing_file = false;

            string next;
            string current_sub_path;

            date_time dtm( date_time::local( ) );

            while( true )
            {
               if( raw_file_size )
               {
                  if( skip_existing_file )
                  {
#ifdef ZLIB_SUPPORT
                     if( use_zlib )
                        gzseek( igzf, raw_file_size, SEEK_CUR );
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
                           if( !gzread( igzf, buffer, count ) )
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
                        if( !is_quieter )
                        {
                           date_time now( date_time::local( ) );

                           uint64_t elapsed = seconds_between( dtm, now );

                           if( elapsed >= 1 )
                           {
                              if( initial_progress )
                                 cout << ' ';

                              cout << '.';
                              cout.flush( );

                              dtm = now;
                              initial_progress = false;
                           }
                        }

#ifndef ZLIB_SUPPORT
                        p_os->rdbuf( )->sputn( buffer, count );
                        if( !p_os->good( ) )
                           throw runtime_error( "unexpected bad output file stream" );
#else
                        if( !use_zlib )
                        {
                           p_os->rdbuf( )->sputn( buffer, count );

                           if( !p_os->good( ) )
                              throw runtime_error( "unexpected bad output file stream" );
                        }
                        else if( !gzwrite( gzf, buffer, count ) )
                           throw runtime_error( "writing zlib block" );
#endif

                        raw_file_size -= count;
                     }

                     if( !is_quieter )
                        cout << endl;

                     continue;
                  }
               }

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

                  if( count == 0 )
                     line_size = unescaped( next ).size( );

                  // NOTE: If skipping a file then there is no need to actually
                  // read the data so by determining the line size of the first
                  // line (after unescaping) it is safe to "seek" one byte less
                  // forwards and then read the remainder of the line (the size
                  // of this will depend upon how much escaping is being used).
                  if( skip_existing_file && line_size && file_data_lines > 1 )
                  {
#ifdef ZLIB_SUPPORT
                     if( use_zlib )
                        gzseek( igzf, line_size - 1, SEEK_CUR );
                     else
                        inpf.seekg( line_size - 1, ios::cur );
#else
                     inpf.seekg( line_size - 1, ios::cur );
#endif
                  }

                  if( !is_quieter )
                  {
                     date_time now( date_time::local( ) );

                     uint64_t elapsed = seconds_between( dtm, now );

                     if( !skip_existing_file && elapsed >= 1 )
                     {
                        if( initial_progress )
                           cout << ' ';

                        cout << '.';
                        cout.flush( );

                        dtm = now;
                        initial_progress = false;
                     }
                  }

                  if( !is_quieter && !skip_existing_file && file_data_lines == 0 )
                     cout << endl;

                  if( !skip_existing_file )
                  {
#ifndef ZLIB_SUPPORT
                     *p_os << line << '\n';
#else
                     if( !use_zlib )
                        *p_os << next << '\n';
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

                     string check = next.substr( pos + 1 );
                     next.erase( pos );

                     pos = next.find( ' ' );
                     if( pos == string::npos )
                        throw runtime_error( "unexpected file entry format in line #" + to_string( line ) );

                     string rwx_perms( next.substr( 0, pos ) );
                     next.erase( 0, pos + 1 );

                     count = 0;

                     if( is_delete )
                     {
                        bool matched = false;
                        vector< string >& exprs( all_filespecs[ c_delete_dummy_path ] );

                        string next_path( current_sub_path + '/' + next );
                        for( size_t i = 0; i < exprs.size( ); i++ )
                        {
                           string next_expr( current_sub_path + '/' + exprs[ i ] );

                           if( wildcard_match( next_expr, next_path ) )
                           {
                              if( !is_quieter )
                                 cout << "*kill* \"" << next << "\"" << endl;
                              ++num_deleted;
                              matched = true;
                              break;
                           }
                        }

                        if( matched )
                        {
                           skip_existing_file = true;
                           continue;
                        }
                     }

                     pos = current_sub_path.find( '/' );
                     if( pos != string::npos )
                        next = current_sub_path.substr( pos + 1 ) + '/' + next;

                     if( file_names.count( next ) )
                     {
                        skip_existing_file = true;
                        continue;
                     }
                     else
                     {
                        skip_existing_file = false;

                        if( !is_quieter )
                        {
                           cout << "append \"" << next << "\"";

                           if( ( !raw_file_size && encoding == e_encoding_type_raw )
                            || ( !file_data_lines && encoding != e_encoding_type_raw ) )
                              cout << endl;
                        }

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

                     int next_level( atoi( next.substr( 0, pos ).c_str( ) ) );
                     next.erase( 0, pos + 1 );

                     pos = next.find( ' ' );
                     if( pos == string::npos )
                        throw runtime_error( "unexpected format in line #" + to_string( line ) );

                     string rwx_perms( next.substr( 0, pos ) );
                     next.erase( 0, pos + 1 );

                     current_sub_path = next;

                     if( file_names.count( next ) )
                        continue;
                     else
                     {
                        pos = next.find( '/' );
                        if( !is_quieter && pos != string::npos )
                           cout << "append \"" << next.substr( pos + 1 ) << "/\"\n";

                        file_names.insert( next );
                        g_md5.update( ( unsigned char* )check.c_str( ), check.length( ) );
                     }
                  }
                  else if( type == c_type_checksum )
                     break;
                  else
                     throw runtime_error( "unexpected entry type '" + to_string( type ) + "' found in line #" + to_string( line ) );

#ifndef ZLIB_SUPPORT
                  *p_os << next_line << '\n';
#else
                  if( !use_zlib )
                     *p_os << next_line << '\n';
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
         auto_ptr< char > ap_digest( g_md5.hex_digest( ) );

         osstr << "C " << ap_digest.get( );

#ifndef ZLIB_SUPPORT
         *p_os << osstr.str( ) << '\n';
#else
         if( !use_zlib )
            *p_os << osstr.str( ) << '\n';
         else
            write_zlib_line( gzf, osstr.str( ) );
#endif

#ifdef ZLIB_SUPPORT
         if( use_zlib )
            gzclose( gzf );
#endif

         if( !use_zlib )
         {
            p_os->flush( );
            if( !p_os->good( ) )
               throw runtime_error( "unexpected write failure for output file '" + filename + "'" );
         }
      }

      if( is_append )
      {
         if( !file_remove( filename ) || rename( output_filename.c_str( ), filename.c_str( ) ) != 0 )
            throw runtime_error( "unable to replace original '" + filename + "' with '" + output_filename + "'" );

         if( is_delete && !skip_warns && !is_quieter && num_deleted == 0 )
            cout << "warning: found no matching files to delete" << endl;
      }
      else if( matched_filters.empty( ) )
      {
         file_remove( filename );

         if( !skip_warns )
            cout << "warning: *** nothing to do ***" << endl;
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
