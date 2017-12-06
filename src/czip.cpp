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

struct simple_tests
{
   const char* p_input;
   const char* p_output;
}
g_simple_tests[ ] =
{
   { "a", "a" },
   { "aa", "aa" },
   { "aaa", "a\xf0" },
   { "aaaa", "a\xf1" },
   { "aaaaa", "a\xf2" },
   { "abc", "abc" },
   { "abcd", "a\xf9\x80" },
   { "abcde", "a\xf9\x81" },
   { "abcdef", "a\xf9\x82" },
   { "abcdefg", "a\xf9\x83" },
   { "abcdefgh", "a\xf9\x84" },
   { "abcdefghi", "a\xf9\x85" },
   { "abcdefghij", "a\xf9\x86" },
   { "abcdefghijk", "a\xf9\x87" },
   { "abcdefghijkl", "a\xf9\x88" },
   { "abcdefghijklm", "a\xf9\x89" },
   { "abcdefghijklmn", "a\xf9\x8a" },
   { "abcdefghijklmno", "a\xf9\x8b" },
   { "abcdefghijklmnop", "a\xf9\x8c" },
   { "abcdefghijklmnopq", "a\xf9\x8d" },
   { "abcdefghijklmnopqr", "a\xf9\x8e" },
   { "abcdefghijklmnopqrs", "a\xf9\x8f" },
   { "abcdefghijklmnopqrst", "a\xf9\x8ft" },
   { "zyx", "zyx" },
   { "zyxw", "z\xf9\x90" },
   { "zyxwv", "z\xf9\x91" },
   { "zyxwvu", "z\xf9\x92" },
   { "zyxwvut", "z\xf9\x93" },
   { "zyxwvuts", "z\xf9\x94" },
   { "zyxwvutsr", "z\xf9\x95" },
   { "zyxwvutsrq", "z\xf9\x96" },
   { "zyxwvutsrqp", "z\xf9\x97" },
   { "zyxwvutsrqpo", "z\xf9\x98" },
   { "zyxwvutsrqpon", "z\xf9\x99" },
   { "zyxwvutsrqponm", "z\xf9\x9a" },
   { "zyxwvutsrqponml", "z\xf9\x9b" },
   { "zyxwvutsrqponmlk", "z\xf9\x9c" },
   { "zyxwvutsrqponmlkj", "z\xf9\x9d" },
   { "zyxwvutsrqponmlkji", "z\xf9\x9e" },
   { "zyxwvutsrqponmlkjih", "z\xf9\x9f" },
   { "zyxwvutsrqponmlkjihg", "z\xf9\x9fg" },
   { "and", "\xf3\x09" },
   { "not", "\xf3\x97" },
   { "zoo", "\xf3\xff" },
   { "AND", "\xf4\x09" },
   { "NOT", "\xf4\x97" },
   { "ZOO", "\xf4\xff" },
   { "andAND", "\xf3\x09\xf4\x09" },
   { "notNOT", "\xf3\x97\xf4\x97" },
   { "zooZOO", "\xf3\xff\xf4\xff" },
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

      if( inc > 1 && i > inc && ( ( i - 1 ) % inc ) )
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

   for( size_t i = 0; i <  ARRAY_SIZE( g_simple_tests ); i++ )
   {
      stringstream is( string( g_simple_tests[ i ].p_input ) );
      stringstream os;

      encode_clz_data( is, os );

      os.seekg( 0, ios::beg );

      if( os.str( ) != string( g_simple_tests[ i ].p_output ) )
         throw runtime_error( "failed test with input: " + string( g_simple_tests[ i ].p_input ) );
   }

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

   string file_to_remove;

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

            file_to_remove = output_file;
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
      if( !file_to_remove.empty( ) )
         file_remove( file_to_remove );

      cerr << "error: " << e.what( ) << endl;
      return 1;
   }

   return 0;
}
