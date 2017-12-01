// Copyright (c) 2017 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <string>
#  include <fstream>
#  include <sstream>
#  include <iostream>
#  include <stdexcept>
#endif

#include "clz.h"

#include "utilities.h"

using namespace std;

const char* const c_cz_ext = ".cz";

const char* const c_quiet_opt = "-q";

bool do_tests( )
{
   bool okay = true;

   stringstream sstr1, sstr2;

   cout << "Perfoming single byte tests (1..25000 bytes of 'x')..." << endl;

   size_t min_length = 0;
   size_t max_length = 0;

   for( size_t i = 1; i <= 25000; i++ )
   {
      string s( i, 'x' );

      stringstream sstr1( s );
      stringstream sstr2;

      encode_clz_data( sstr1, sstr2 );

      size_t length = sstr2.str( ).length( );

      if( !min_length )
         min_length = length;
      else if( length < min_length )
         min_length = length;

      if( length > max_length )
         max_length = length;

      sstr2.seekg( 0, ios::beg );

      ostringstream osstr;
      decode_clz_data( sstr2, osstr );

      if( osstr.str( ) != s )
      {
         cout << "*** failed for " << i << " length single byte value ***" << endl;
         okay = false;
         break;
      }
   }

   cout << "[passed] min_length = " << min_length << ", max_length = " << max_length << endl;

   return okay;
}

int main( int argc, char* argv[ ] )
{
   int first_arg = 1;

   if( argc < 2 || argc > 3 || ( argc == 3 && argv[ 1 ] != string( c_quiet_opt ) ) )
   {
      cout << "czip v0.1a\n";
      cout << "Usage: czip [" << c_quiet_opt << "] <file>" << endl;

      return 1;
   }

   bool quiet = false;
   if( argc > 1 + first_arg && argv[ first_arg ] == string( c_quiet_opt ) )
   {
      ++first_arg;
      quiet = true;
   }

   try
   {
      string file( argv[ first_arg ] );

      // NOTE: If the file name is "@test" then run tests instead.
      if( file == "@test" )
         return do_tests( );

      string::size_type pos = file.find( c_cz_ext );

      // NOTE: If <file>.cz exists then assume this was intended.
      if( pos == string::npos && file_exists( file + c_cz_ext ) )
      {
         pos = file.length( );
         file += string( c_cz_ext );
      }

      if( pos == string::npos )
      {
         if( !file_exists( file ) )
            throw runtime_error( "file '" + file + "' not found" );

         string output_file( file + c_cz_ext );

         if( file_exists( output_file ) )
            throw runtime_error( "file '" + output_file + "' already exists" );
         else
         {
            ifstream is( file.c_str( ), ios::in | ios::binary );
            if( !is )
               throw runtime_error( "unable to open '" + file + "' for input" );

            ofstream os( output_file.c_str( ), ios::out | ios::binary );
            if( !os )
               throw runtime_error( "unable to open '" + output_file + "' for input" );

            encode_clz_data( is, os );
         }

         file_remove( file );
      }
      else
      {
         if( pos == 0 )
            throw runtime_error( "file '" + file + "' must be <name> or <name>.cz" );

         if( !file_exists( file ) )
            throw runtime_error( "file '" + file + "' not found" );

         string output_file( file.substr( 0, pos ) );

         if( file_exists( output_file ) )
            throw runtime_error( "file '" + output_file + "' already exists" );
         else
         {
            ifstream is( file.c_str( ), ios::in | ios::binary );
            if( !is )
               throw runtime_error( "unable to open '" + file + "' for input" );

            ofstream os( output_file.c_str( ), ios::out | ios::binary );
            if( !os )
               throw runtime_error( "unable to open '" + output_file + "' for input" );

            decode_clz_data( is, os );
         }

         file_remove( file );
      }
   }
   catch( exception& e )
   {
      cerr << "error: " << e.what( ) << endl;
      return 1;
   }

   return 0;
}
