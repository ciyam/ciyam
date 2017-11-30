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
#  include <iostream>
#  include <stdexcept>
#endif

#include "clz.h"

#include "utilities.h"

using namespace std;

const char* const c_cz_ext = ".cz";

const char* const c_quiet_opt = "-q";

int main( int argc, char* argv[ ] )
{
   int first_arg = 1;

   if( argc < 2 || argc > 3 || ( argc == 3 &&  argv[ 1 ] != string( c_quiet_opt ) ) )
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

      string::size_type pos = file.find( c_cz_ext );

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
