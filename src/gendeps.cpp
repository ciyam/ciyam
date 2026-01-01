// Copyright (c) 2009-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <set>
#  include <vector>
#  include <string>
#  include <fstream>
#  include <iostream>
#endif

using namespace std;

const char* const c_title = "GENDEPS v0.1e";
const char* const c_usage = "Usage: gendeps [-i<include_paths>] [-o<object_path>] [-x<dependency ext>] <src_file>";

const char* const c_lit_include = "include";

set< string > include_names;
set< string > include_paths;

void transform_path_separators( string& path, char from = '/', char to = '\\' )
{
   for( size_t i = 0; i < path.size( ); i++ )
   {
      if( path[ i ] == from )
         path[ i ] = to;
   }
}

bool get_include_name( const string& line, string& include )
{
   size_t pos;
   bool okay = true;

   for( pos = 0; pos < line.size( ); pos++ )
   {
      if( line[ pos ] != ' ' )
         break;
   }

   if( pos == line.size( ) || line[ pos ] != '#' )
      okay = false;

   if( okay )
   {
      for( pos = pos + 1; pos < line.size( ); pos++ )
      {
         if( line[ pos ] != ' ' )
            break;
      }

      if( pos == line.size( ) )
         okay = false;

      if( okay )
      {
         string next( line.substr( pos ) );

         // IMPORTANT: Remove C/C++ comments and trailing whitespace...
         size_t pos_1 = next.find( "//" );
         size_t pos_2 = next.find( "/*" );
         pos = min( pos_1, pos_2 );
         if( pos != string::npos )
         {
            while( pos > 0 && next[ pos - 1 ] == ' ' )
               --pos;
            next.erase( pos );
         }

         if( next.size( ) > string( c_lit_include ).size( ) )
         {
            if( next.substr( 0, string( c_lit_include ).size( ) ) != string( c_lit_include ) )
               okay = false;
         }

         if( okay )
         {
            next.erase( 0, string( c_lit_include ).size( ) );
            pos = 0;
            while( next[ pos ] == ' ' )
               ++pos;

            next = next.substr( pos );
            if( next.size( ) < 3 || next[ 0 ] != '"' || next[ next.size( ) - 1 ] != '"' )
            {
               okay = false;
               if( next.size( ) > 3 && ( next[ 0 ] != '<' || next[ next.size( ) - 1 ] != '>' ) )
               {
                  cerr << "Error: Found unexpected string '" << next << "' parsing include line." << endl;
                  exit( 1 );
               }
            }
            else
               include = next.substr( 1, next.size( ) - 2 );
         }
      }
   }

   return okay;
}

void add_include_names( const string& file_name, bool is_root_file = true )
{
   string path;

   ifstream inpf( file_name.c_str( ) );

   if( is_root_file && !inpf )
   {
      cerr << "Error: Unable to open source file '" << file_name << "' for input." << endl;
      exit( 1 );
   }

   if( !is_root_file )
   {
      set< string >::iterator i;
      for( i = include_paths.begin( ); i != include_paths.end( ); ++i )
      {
         string str( *i );
         str += '\\';
         str += file_name;

         inpf.clear( );
         inpf.open( str.c_str( ), ios::in );
         if( inpf )
         {
            path = *i;
            break;
         }
      }

      if( !inpf )
      {
         cerr << "Error: Unable to open source file '" << file_name << "' for input." << endl;
         exit( 1 );
      }

      string str( path );
      if( !str.empty( ) )
         str += '\\';

      str += file_name;
      transform_path_separators( str );

      if( include_names.find( str ) != include_names.end( ) )
         return;

      include_names.insert( str );
      size_t pos = str.find_last_of( '\\' );
      if( pos != string::npos )
      {
         string current = str.substr( 0, pos );
         if( include_paths.find( current ) == include_paths.end( ) )
            include_paths.insert( current );
      }
   }

   string line, include;
   while( getline( inpf, line ) )
   {
      if( get_include_name( line, include ) )
         add_include_names( include, false );
   }
}

int main( int argc, char* argv[ ] )
{
   bool has_object_path = false;
   string src_file_name, dependency_file_extension( "obj" );
   string object_file_path;

   if( argc < 2 || argc > 5 )
   {
      cout << c_title << '\n' << c_usage << endl;
      return 0;
   }

   int argnum = 1;
   while( argnum < argc - 1 )
   {
      string opt( argv[ argnum++ ] );
      if( opt.size( ) < 2
       || ( opt.substr( 0, 2 ) != "-i" && opt.substr( 0, 2 ) != "/i"
       && opt.substr( 0, 2 ) != "-o" && opt.substr( 0, 2 ) != "/o"
       && opt.substr( 0, 2 ) != "-x" && opt.substr( 0, 2 ) != "/x" ) )
      {
         cerr << "Error: Unknown option '" << opt << "'." << endl;
         return 1;
      }

      bool is_object_path = false;
      bool is_include_path = false;
      if( opt.substr( 0, 2 ) == "-o" || opt.substr( 0, 2 ) == "/o" )
         is_object_path = true;
      else if( opt.substr( 0, 2 ) == "-i" || opt.substr( 0, 2 ) == "/i" )
         is_include_path = true;

      opt.erase( 0, 2 );

      if( is_object_path )
      {
         object_file_path = opt;
         has_object_path = true;
         transform_path_separators( object_file_path );

         if( !object_file_path.empty( ) && object_file_path[ object_file_path.size( ) - 1 ] != '\\' )
            object_file_path += '\\';
      }
      else if( is_include_path )
      {
         size_t pos = opt.find( ';' );
         while( true )
         {
            include_paths.insert( opt.substr( 0, pos ) );
            if( pos == string::npos )
               break;

            opt.erase( 0, pos + 1 );
            pos = opt.find( ';' );
         }
      }
      else
         dependency_file_extension = opt;
   }

   src_file_name = argv[ argnum ];

   size_t pos = src_file_name.find_last_of( '.' );
   if( pos == string::npos )
   {
      cerr << "Error: Didn't find file extension in '" << src_file_name << "'." << endl;
      return 1;
   }

   string object_file( src_file_name.substr( 0, pos ) );

   if( has_object_path )
   {
      transform_path_separators( object_file );

      pos = object_file.find_last_of( '\\' );
      if( pos != string::npos )
         object_file.erase( 0, pos + 1 );

      object_file = object_file_path + object_file;
   }

   add_include_names( src_file_name );

   object_file = object_file + '.' + dependency_file_extension;
   cout << object_file << ":\\\n" << ' ' << src_file_name;

   set< string >::iterator i;
   for( i = include_names.begin( ); i != include_names.end( ); i++ )
      cout << "\\\n " << *i;

   cout << endl;

   return 0;
}

