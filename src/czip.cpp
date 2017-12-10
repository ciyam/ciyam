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
   { "abcdefghijklmnopqrst", "a\xf9\x8f""t" },
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
   { "zyxwvutsrqponmlkjihg", "z\xf9\x9f""g" },
   { "aabb", "aabb" },
   { "aabbcc", "aa\xf9\xa0" },
   { "aabbccdd", "aa\xf9\xa1" },
   { "aabbccddee", "aa\xf9\xa2" },
   { "aabbccddeeff", "aa\xf9\xa3" },
   { "aabbccddeeffgg", "aa\xf9\xa4" },
   { "aabbccddeeffgghh", "aa\xf9\xa5" },
   { "aabbccddeeffgghhii", "aa\xf9\xa6" },
   { "aabbccddeeffgghhiijj", "aa\xf9\xa7" },
   { "aabbccddeeffgghhiijjkk", "aa\xf9\xa8" },
   { "aabbccddeeffgghhiijjkkll", "aa\xf9\xa9" },
   { "aabbccddeeffgghhiijjkkllmm", "aa\xf9\xaa" },
   { "aabbccddeeffgghhiijjkkllmmnn", "aa\xf9\xab" },
   { "aabbccddeeffgghhiijjkkllmmnnoo", "aa\xf9\xac" },
   { "aabbccddeeffgghhiijjkkllmmnnoopp", "aa\xf9\xad" },
   { "aabbccddeeffgghhiijjkkllmmnnooppqq", "aa\xf9\xae" },
   { "aabbccddeeffgghhiijjkkllmmnnooppqqrr", "aa\xf9\xaf" },
   { "aabbccddeeffgghhiijjkkllmmnnooppqqrrss", "aa\xf9\xaf""ss" },
   { "zzyy", "zzyy" },
   { "zzyyxx", "zz\xf9\xb0" },
   { "zzyyxxww", "zz\xf9\xb1" },
   { "zzyyxxwwvv", "zz\xf9\xb2" },
   { "zzyyxxwwvvuu", "zz\xf9\xb3" },
   { "zzyyxxwwvvuutt", "zz\xf9\xb4" },
   { "zzyyxxwwvvuuttss", "zz\xf9\xb5" },
   { "zzyyxxwwvvuuttssrr", "zz\xf9\xb6" },
   { "zzyyxxwwvvuuttssrrqq", "zz\xf9\xb7" },
   { "zzyyxxwwvvuuttssrrqqpp", "zz\xf9\xb8" },
   { "zzyyxxwwvvuuttssrrqqppoo", "zz\xf9\xb9" },
   { "zzyyxxwwvvuuttssrrqqppoonn", "zz\xf9\xba" },
   { "zzyyxxwwvvuuttssrrqqppoonnmm", "zz\xf9\xbb" },
   { "zzyyxxwwvvuuttssrrqqppoonnmmll", "zz\xf9\xbc" },
   { "zzyyxxwwvvuuttssrrqqppoonnmmllkk", "zz\xf9\xbd" },
   { "zzyyxxwwvvuuttssrrqqppoonnmmllkkjj", "zz\xf9\xbe" },
   { "zzyyxxwwvvuuttssrrqqppoonnmmllkkjjii", "zz\xf9\xbf" },
   { "zzyyxxwwvvuuttssrrqqppoonnmmllkkjjiihh", "zz\xf9\xbf""hh" },
   { "abac", "abac" },
   { "abacad", "ab\xf9\xc0" },
   { "abacadae", "ab\xf9\xc1" },
   { "abacadaeaf", "ab\xf9\xc2" },
   { "abacadaeafag", "ab\xf9\xc3" },
   { "abacadaeafagah", "ab\xf9\xc4" },
   { "abacadaeafagahai", "ab\xf9\xc5" },
   { "abacadaeafagahaiaj", "ab\xf9\xc6" },
   { "abacadaeafagahaiajak", "ab\xf9\xc7" },
   { "abacadaeafagahaiajakal", "ab\xf9\xc8" },
   { "abacadaeafagahaiajakalam", "ab\xf9\xc9" },
   { "abacadaeafagahaiajakalaman", "ab\xf9\xca" },
   { "abacadaeafagahaiajakalamanao", "ab\xf9\xcb" },
   { "abacadaeafagahaiajakalamanaoap", "ab\xf9\xcc" },
   { "abacadaeafagahaiajakalamanaoapaq", "ab\xf9\xcd" },
   { "abacadaeafagahaiajakalamanaoapaqar", "ab\xf9\xce" },
   { "abacadaeafagahaiajakalamanaoapaqaras", "ab\xf9\xcf" },
   { "abacadaeafagahaiajakalamanaoapaqarasat", "ab\xf9\xcf""at" },
   { "zyzx", "zyzx" },
   { "zyzxzw", "zy\xf9\xd0" },
   { "zyzxzwzv", "zy\xf9\xd1" },
   { "zyzxzwzvzu", "zy\xf9\xd2" },
   { "zyzxzwzvzuzt", "zy\xf9\xd3" },
   { "zyzxzwzvzuztzs", "zy\xf9\xd4" },
   { "zyzxzwzvzuztzszr", "zy\xf9\xd5" },
   { "zyzxzwzvzuztzszrzq", "zy\xf9\xd6" },
   { "zyzxzwzvzuztzszrzqzp", "zy\xf9\xd7" },
   { "zyzxzwzvzuztzszrzqzpzo", "zy\xf9\xd8" },
   { "zyzxzwzvzuztzszrzqzpzozn", "zy\xf9\xd9" },
   { "zyzxzwzvzuztzszrzqzpzoznzm", "zy\xf9\xda" },
   { "zyzxzwzvzuztzszrzqzpzoznzmzl", "zy\xf9\xdb" },
   { "zyzxzwzvzuztzszrzqzpzoznzmzlzk", "zy\xf9\xdc" },
   { "zyzxzwzvzuztzszrzqzpzoznzmzlzkzj", "zy\xf9\xdd" },
   { "zyzxzwzvzuztzszrzqzpzoznzmzlzkzjzi", "zy\xf9\xde" },
   { "zyzxzwzvzuztzszrzqzpzoznzmzlzkzjzizh", "zy\xf9\xdf" },
   { "zyzxzwzvzuztzszrzqzpzoznzmzlzkzjzizhzg", "zy\xf9\xdf""zg" },
   { "baca", "baca" },
   { "bacada", "ba\xf9\xe0" },
   { "bacadaea", "ba\xf9\xe1" },
   { "bacadaeafa", "ba\xf9\xe2" },
   { "bacadaeafaga", "ba\xf9\xe3" },
   { "bacadaeafagaha", "ba\xf9\xe4" },
   { "bacadaeafagahaia", "ba\xf9\xe5" },
   { "bacadaeafagahaiaja", "ba\xf9\xe6" },
   { "bacadaeafagahaiajaka", "ba\xf9\xe7" },
   { "bacadaeafagahaiajakala", "ba\xf9\xe8" },
   { "bacadaeafagahaiajakalama", "ba\xf9\xe9" },
   { "bacadaeafagahaiajakalamana", "ba\xf9\xea" },
   { "bacadaeafagahaiajakalamanaoa", "ba\xf9\xeb" },
   { "bacadaeafagahaiajakalamanaoapa", "ba\xf9\xec" },
   { "bacadaeafagahaiajakalamanaoapaqa", "ba\xf9\xed" },
   { "bacadaeafagahaiajakalamanaoapaqara", "ba\xf9\xee" },
   { "bacadaeafagahaiajakalamanaoapaqarasa", "ba\xf9\xef" },
   { "bacadaeafagahaiajakalamanaoapaqarasata", "ba\xf9\xef""ta" },
   { "zaya", "zaya" },
   { "zayaxa", "za\xf9\xf0" },
   { "zayaxawa", "za\xf9\xf1" },
   { "zayaxawava", "za\xf9\xf2" },
   { "zayaxawavaua", "za\xf9\xf3" },
   { "zayaxawavauata", "za\xf9\xf4" },
   { "zayaxawavauatasa", "za\xf9\xf5" },
   { "zayaxawavauatasara", "za\xf9\xf6" },
   { "zayaxawavauatasaraqa", "za\xf9\xf7" },
   { "zayaxawavauatasaraqapa", "za\xf9\xf8" },
   { "zayaxawavauatasaraqapaoa", "za\xf9\xf9" },
   { "zayaxawavauatasaraqapaoana", "za\xf9\xfa" },
   { "zayaxawavauatasaraqapaoanama", "za\xf9\xfb" },
   { "zayaxawavauatasaraqapaoanamala", "za\xf9\xfc" },
   { "zayaxawavauatasaraqapaoanamalaka", "za\xf9\xfd" },
   { "zayaxawavauatasaraqapaoanamalakaja", "za\xf9\xfe" },
   { "zayaxawavauatasaraqapaoanamalakajaia", "za\xf9\xff" },
   { "zayaxawavauatasaraqapaoanamalakajaiaha", "za\xf9\xff""ha" },
   { "and", "\xf3\x0a" },
   { "not", "\xf3\x97" },
   { "zes", "\xf3\xff" },
   { "AND", "\xf5\x0a" },
   { "NOT", "\xf5\x97" },
   { "ZES", "\xf5\xff" },
   { "And", "\xf4\x0a" },
   { "Not", "\xf4\x97" },
   { "Zes", "\xf4\xff" },
   { "andAnd", "\xf3\x0a\xf4\x0a" },
   { "notNot", "\xf3\x97\xf4\x97" },
   { "zesZes", "\xf3\xff\xf4\xff" },
   { "andAND", "\xf3\x0a\xf5\x0a" },
   { "notNOT", "\xf3\x97\xf5\x97" },
   { "zesZES", "\xf3\xff\xf5\xff" },
   { "andNotZES", "\xf3\x0a\xf4\x97\xf5\xff" },
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
