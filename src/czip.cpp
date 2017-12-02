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

const char* const c_speed_fast = "fast";
const char* const c_speed_slow = "slow";

const char* const c_speed_faster = "faster";
const char* const c_speed_slower = "slower";

const char* const c_speed_fastest = "fastest";
const char* const c_speed_slowest = "slowest";

enum speed
{
   e_speed_fast = 25,
   e_speed_slow = 5,
   e_speed_faster = 50,
   e_speed_slower = 2,
   e_speed_fastest = 100,
   e_speed_slowest = 1
};

bool do_test( size_t length, speed inc )
{
   bool okay = true;

   size_t min_length = 0;
   size_t max_length = 0;

   size_t max_repeats = 36000;

   size_t max_occurrence = 0;

   string s, alphabet( "abcdefghijklmnopqrstuvwxyz" );

   max_repeats -= ( length * 1000 );

   // NOTE: Do this to reduce the amount of memory re-allocation when looping.
   s.reserve( max_repeats * length );

   cout << "Perfoming " << length << " byte tests (1.." << max_repeats << " occurrences of '" << alphabet.substr( 0, length ) << "' in steps of " << inc << ")..." << endl;

   for( size_t i = 1; i <= max_repeats; i++ )
   {
      s += alphabet.substr( 0, length );

      if( inc > 1 && ( ( i - 1 ) % inc ) )
         continue;

      stringstream sstr1( s );
      stringstream sstr2;

      encode_clz_data( sstr1, sstr2 );

      size_t next_length = sstr2.str( ).length( );

      if( !min_length )
         min_length = next_length;
      else if( next_length < min_length )
         min_length = next_length;

      if( next_length > max_length )
      {
         max_occurrence = i;
         max_length = next_length;
      }

      sstr2.seekg( 0, ios::beg );

      ostringstream osstr;
      decode_clz_data( sstr2, osstr );

      if( osstr.str( ) != s )
      {
         cout << "*** failed for occurrence " << i << " of " << length << " byte value (" << s.length( ) << " bytes) ***" << endl;
         okay = false;
         break;
      }
   }

   if( okay )
      cout << "[passed] min_length = " << min_length << ", max_length = " << max_length << " (for occurrence " << max_occurrence << " of " << ( max_occurrence * length ) << " bytes)" << endl;

   return okay;
}

bool do_tests( const string& test_info )
{
   bool okay = true;

   string info( test_info );

   speed inc = e_speed_fast;
   speed new_inc = e_speed_fast;

   size_t new_start = 0;

   if( !info.empty( ) )
   {
      if( info[ 0 ] == ':' )
         info.erase( 0, 1 );

      string::size_type pos = info.find( '=' );
      if( pos != string::npos )
      {
         new_start = from_string< size_t >( info.substr( pos + 1 ) );
         info.erase( pos );
      }

      if( info == string( c_speed_slow ) )
         new_inc = e_speed_slow;
      else if( info == string( c_speed_faster ) )
         new_inc = e_speed_faster;
      else if( info == string( c_speed_slower ) )
         new_inc = e_speed_slower;
      else if( info == string( c_speed_fastest ) )
         new_inc = e_speed_fastest;
      else if( info == string( c_speed_slowest ) )
         new_inc = e_speed_slowest;

      if( !new_start )
         inc = new_inc;
   }

   for( size_t i = 1; i <= 26; i++ )
   {
      if( i == new_start )
         inc = new_inc;

      okay = do_test( i, inc );

      if( !okay )
         break;
   }

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
      if( file.find( "@test" ) == 0 )
         return do_tests( file.substr( 5 ) );

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
