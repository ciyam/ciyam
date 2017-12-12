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
   { "aaaaaa", "a\xf2""a" },
   { "aaaaaaa", "a\xf2""aa" },
   { "aaaaaaaa", "a\xf2""a\xf0" },
   { "aaaaaaaaa", "a\xf2""a\xf1" },
   { "and", "\xf3\x0a" },
   { "not", "\xf3\x97" },
   { "zes", "\xf3\xff" },
   { "And", "\xf4\x0a" },
   { "Not", "\xf4\x97" },
   { "Zes", "\xf4\xff" },
   { "AND", "\xf5\x0a" },
   { "NOT", "\xf5\x97" },
   { "ZES", "\xf5\xff" },
   { "andAnd", "\xf3\x0a\xf4\x0a" },
   { "notNot", "\xf3\x97\xf4\x97" },
   { "zesZes", "\xf3\xff\xf4\xff" },
   { "andAND", "\xf3\x0a\xf5\x0a" },
   { "notNOT", "\xf3\x97\xf5\x97" },
   { "zesZES", "\xf3\xff\xf5\xff" },
   { "andNotZES", "\xf3\x0a\xf4\x97\xf5\xff" },
   { "abc", "abc" },
   { "abcd", "a\xf6\x80" },
   { "abcde", "a\xf6\x81" },
   { "abcdef", "a\xf6\x82" },
   { "abcdefg", "a\xf6\x83" },
   { "abcdefgh", "a\xf6\x84" },
   { "abcdefghi", "a\xf6\x85" },
   { "abcdefghij", "a\xf6\x86" },
   { "abcdefghijk", "a\xf6\x87" },
   { "abcdefghijkl", "a\xf6\x88" },
   { "abcdefghijklm", "a\xf6\x89" },
   { "abcdefghijklmn", "a\xf6\x8a" },
   { "abcdefghijklmno", "a\xf6\x8b" },
   { "abcdefghijklmnop", "a\xf6\x8c" },
   { "abcdefghijklmnopq", "a\xf6\x8d" },
   { "abcdefghijklmnopqr", "a\xf6\x8e" },
   { "abcdefghijklmnopqrs", "a\xf6\x8f" },
   { "abcdefghijklmnopqrst", "a\xf6\x8f""t" },
   { "zyx", "zyx" },
   { "zyxw", "z\xf6\x90" },
   { "zyxwv", "z\xf6\x91" },
   { "zyxwvu", "z\xf6\x92" },
   { "zyxwvut", "z\xf6\x93" },
   { "zyxwvuts", "z\xf6\x94" },
   { "zyxwvutsr", "z\xf6\x95" },
   { "zyxwvutsrq", "z\xf6\x96" },
   { "zyxwvutsrqp", "z\xf6\x97" },
   { "zyxwvutsrqpo", "z\xf6\x98" },
   { "zyxwvutsrqpon", "z\xf6\x99" },
   { "zyxwvutsrqponm", "z\xf6\x9a" },
   { "zyxwvutsrqponml", "z\xf6\x9b" },
   { "zyxwvutsrqponmlk", "z\xf6\x9c" },
   { "zyxwvutsrqponmlkj", "z\xf6\x9d" },
   { "zyxwvutsrqponmlkji", "z\xf6\x9e" },
   { "zyxwvutsrqponmlkjih", "z\xf6\x9f" },
   { "zyxwvutsrqponmlkjihg", "z\xf6\x9f""g" },
   { "aabb", "aabb" },
   { "aabbcc", "aa\xf6\xa0" },
   { "aabbccdd", "aa\xf6\xa1" },
   { "aabbccddee", "aa\xf6\xa2" },
   { "aabbccddeeff", "aa\xf6\xa3" },
   { "aabbccddeeffgg", "aa\xf6\xa4" },
   { "aabbccddeeffgghh", "aa\xf6\xa5" },
   { "aabbccddeeffgghhii", "aa\xf6\xa6" },
   { "aabbccddeeffgghhiijj", "aa\xf6\xa7" },
   { "aabbccddeeffgghhiijjkk", "aa\xf6\xa8" },
   { "aabbccddeeffgghhiijjkkll", "aa\xf6\xa9" },
   { "aabbccddeeffgghhiijjkkllmm", "aa\xf6\xaa" },
   { "aabbccddeeffgghhiijjkkllmmnn", "aa\xf6\xab" },
   { "aabbccddeeffgghhiijjkkllmmnnoo", "aa\xf6\xac" },
   { "aabbccddeeffgghhiijjkkllmmnnoopp", "aa\xf6\xad" },
   { "aabbccddeeffgghhiijjkkllmmnnooppqq", "aa\xf6\xae" },
   { "aabbccddeeffgghhiijjkkllmmnnooppqqrr", "aa\xf6\xaf" },
   { "aabbccddeeffgghhiijjkkllmmnnooppqqrrss", "aa\xf6\xaf""ss" },
   { "zzyy", "zzyy" },
   { "zzyyxx", "zz\xf6\xb0" },
   { "zzyyxxww", "zz\xf6\xb1" },
   { "zzyyxxwwvv", "zz\xf6\xb2" },
   { "zzyyxxwwvvuu", "zz\xf6\xb3" },
   { "zzyyxxwwvvuutt", "zz\xf6\xb4" },
   { "zzyyxxwwvvuuttss", "zz\xf6\xb5" },
   { "zzyyxxwwvvuuttssrr", "zz\xf6\xb6" },
   { "zzyyxxwwvvuuttssrrqq", "zz\xf6\xb7" },
   { "zzyyxxwwvvuuttssrrqqpp", "zz\xf6\xb8" },
   { "zzyyxxwwvvuuttssrrqqppoo", "zz\xf6\xb9" },
   { "zzyyxxwwvvuuttssrrqqppoonn", "zz\xf6\xba" },
   { "zzyyxxwwvvuuttssrrqqppoonnmm", "zz\xf6\xbb" },
   { "zzyyxxwwvvuuttssrrqqppoonnmmll", "zz\xf6\xbc" },
   { "zzyyxxwwvvuuttssrrqqppoonnmmllkk", "zz\xf6\xbd" },
   { "zzyyxxwwvvuuttssrrqqppoonnmmllkkjj", "zz\xf6\xbe" },
   { "zzyyxxwwvvuuttssrrqqppoonnmmllkkjjii", "zz\xf6\xbf" },
   { "zzyyxxwwvvuuttssrrqqppoonnmmllkkjjiihh", "zz\xf6\xbf""hh" },
   { "abac", "abac" },
   { "abacad", "ab\xf6\xc0" },
   { "abacadae", "ab\xf6\xc1" },
   { "abacadaeaf", "ab\xf6\xc2" },
   { "abacadaeafag", "ab\xf6\xc3" },
   { "abacadaeafagah", "ab\xf6\xc4" },
   { "abacadaeafagahai", "ab\xf6\xc5" },
   { "abacadaeafagahaiaj", "ab\xf6\xc6" },
   { "abacadaeafagahaiajak", "ab\xf6\xc7" },
   { "abacadaeafagahaiajakal", "ab\xf6\xc8" },
   { "abacadaeafagahaiajakalam", "ab\xf6\xc9" },
   { "abacadaeafagahaiajakalaman", "ab\xf6\xca" },
   { "abacadaeafagahaiajakalamanao", "ab\xf6\xcb" },
   { "abacadaeafagahaiajakalamanaoap", "ab\xf6\xcc" },
   { "abacadaeafagahaiajakalamanaoapaq", "ab\xf6\xcd" },
   { "abacadaeafagahaiajakalamanaoapaqar", "ab\xf6\xce" },
   { "abacadaeafagahaiajakalamanaoapaqaras", "ab\xf6\xcf" },
   { "abacadaeafagahaiajakalamanaoapaqarasat", "ab\xf6\xcf""at" },
   { "zyzx", "zyzx" },
   { "zyzxzw", "zy\xf6\xd0" },
   { "zyzxzwzv", "zy\xf6\xd1" },
   { "zyzxzwzvzu", "zy\xf6\xd2" },
   { "zyzxzwzvzuzt", "zy\xf6\xd3" },
   { "zyzxzwzvzuztzs", "zy\xf6\xd4" },
   { "zyzxzwzvzuztzszr", "zy\xf6\xd5" },
   { "zyzxzwzvzuztzszrzq", "zy\xf6\xd6" },
   { "zyzxzwzvzuztzszrzqzp", "zy\xf6\xd7" },
   { "zyzxzwzvzuztzszrzqzpzo", "zy\xf6\xd8" },
   { "zyzxzwzvzuztzszrzqzpzozn", "zy\xf6\xd9" },
   { "zyzxzwzvzuztzszrzqzpzoznzm", "zy\xf6\xda" },
   { "zyzxzwzvzuztzszrzqzpzoznzmzl", "zy\xf6\xdb" },
   { "zyzxzwzvzuztzszrzqzpzoznzmzlzk", "zy\xf6\xdc" },
   { "zyzxzwzvzuztzszrzqzpzoznzmzlzkzj", "zy\xf6\xdd" },
   { "zyzxzwzvzuztzszrzqzpzoznzmzlzkzjzi", "zy\xf6\xde" },
   { "zyzxzwzvzuztzszrzqzpzoznzmzlzkzjzizh", "zy\xf6\xdf" },
   { "zyzxzwzvzuztzszrzqzpzoznzmzlzkzjzizhzg", "zy\xf6\xdf""zg" },
   { "baca", "baca" },
   { "bacada", "ba\xf6\xe0" },
   { "bacadaea", "ba\xf6\xe1" },
   { "bacadaeafa", "ba\xf6\xe2" },
   { "bacadaeafaga", "ba\xf6\xe3" },
   { "bacadaeafagaha", "ba\xf6\xe4" },
   { "bacadaeafagahaia", "ba\xf6\xe5" },
   { "bacadaeafagahaiaja", "ba\xf6\xe6" },
   { "bacadaeafagahaiajaka", "ba\xf6\xe7" },
   { "bacadaeafagahaiajakala", "ba\xf6\xe8" },
   { "bacadaeafagahaiajakalama", "ba\xf6\xe9" },
   { "bacadaeafagahaiajakalamana", "ba\xf6\xea" },
   { "bacadaeafagahaiajakalamanaoa", "ba\xf6\xeb" },
   { "bacadaeafagahaiajakalamanaoapa", "ba\xf6\xec" },
   { "bacadaeafagahaiajakalamanaoapaqa", "ba\xf6\xed" },
   { "bacadaeafagahaiajakalamanaoapaqara", "ba\xf6\xee" },
   { "bacadaeafagahaiajakalamanaoapaqarasa", "ba\xf6\xef" },
   { "bacadaeafagahaiajakalamanaoapaqarasata", "ba\xf6\xef""ta" },
   { "zaya", "zaya" },
   { "zayaxa", "za\xf6\xf0" },
   { "zayaxawa", "za\xf6\xf1" },
   { "zayaxawava", "za\xf6\xf2" },
   { "zayaxawavaua", "za\xf6\xf3" },
   { "zayaxawavauata", "za\xf6\xf4" },
   { "zayaxawavauatasa", "za\xf6\xf5" },
   { "zayaxawavauatasara", "za\xf6\xf6" },
   { "zayaxawavauatasaraqa", "za\xf6\xf7" },
   { "zayaxawavauatasaraqapa", "za\xf6\xf8" },
   { "zayaxawavauatasaraqapaoa", "za\xf6\xf9" },
   { "zayaxawavauatasaraqapaoana", "za\xf6\xfa" },
   { "zayaxawavauatasaraqapaoanama", "za\xf6\xfb" },
   { "zayaxawavauatasaraqapaoanamala", "za\xf6\xfc" },
   { "zayaxawavauatasaraqapaoanamalaka", "za\xf6\xfd" },
   { "zayaxawavauatasaraqapaoanamalakaja", "za\xf6\xfe" },
   { "zayaxawavauatasaraqapaoanamalakajaia", "za\xf6\xff" },
   { "zayaxawavauatasaraqapaoanamalakajaiaha", "za\xf6\xff""ha" },
   { "ACBECG", "ACBECG" },
   { "ACBECGDI", "ACBE\xf7\x12" },
   { "ACBECGDIEK", "ACBE\xf7\x13" },
   { "ACBECGDIEKFM", "ACBE\xf7\x14" },
   { "ACBECGDIEKFMGO", "ACBE\xf7\x15" },
   { "ACBECGDIEKFMGOHQ", "ACBE\xf7\x16" },
   { "ACBECGDIEKFMGOHQIS", "ACBE\xf7\x17" },
   { "ACBECGDIEKFMGOHQISJU", "ACBE\xf7\x18" },
   { "ACBECGDIEKFMGOHQISJUKW", "ACBE\xf7\x19" },
   { "ACBECGDIEKFMGOHQISJUKWLY", "ACBE\xf7\x1a" },
   { "ACBECGDIEKFMGOHQISJUKWLYM[", "ACBE\xf7\x1b" },
   { "ACBECGDIEKFMGOHQISJUKWLYM[N]", "ACBE\xf7\x1c" },
   { "ACBECGDIEKFMGOHQISJUKWLYM[N]O_", "ACBE\xf7\x1d" },
   { "ACBECGDIEKFMGOHQISJUKWLYM[N]O_Pa", "ACBE\xf7\x1e" },
   { "ACBECGDIEKFMGOHQISJUKWLYM[N]O_PaQc", "ACBE\xf7\x1f" },
   { "ACBECGDIEKFMGOHQISJUKWLYM[N]O_PaQcRe", "ACBE\xf7\x1f""Re" },
   { "ADBFCH", "ADBFCH" },
   { "ADBFCHDJ", "ADBF\xf7\x12" },
   { "ADBFCHDJEL", "ADBF\xf7\x13" },
   { "ADBFCHDJELFN", "ADBF\xf7\x14" },
   { "ADBFCHDJELFNGP", "ADBF\xf7\x15" },
   { "ADBFCHDJELFNGPHR", "ADBF\xf7\x16" },
   { "ADBFCHDJELFNGPHRIT", "ADBF\xf7\x17" },
   { "ADBFCHDJELFNGPHRITJV", "ADBF\xf7\x18" },
   { "ADBFCHDJELFNGPHRITJVKX", "ADBF\xf7\x19" },
   { "ADBFCHDJELFNGPHRITJVKXLZ", "ADBF\xf7\x1a" },
   { "ADBFCHDJELFNGPHRITJVKXLZM\\", "ADBF\xf7\x1b" },
   { "ADBFCHDJELFNGPHRITJVKXLZM\\N^", "ADBF\xf7\x1c" },
   { "ADBFCHDJELFNGPHRITJVKXLZM\\N^O`", "ADBF\xf7\x1d" },
   { "ADBFCHDJELFNGPHRITJVKXLZM\\N^O`Pb", "ADBF\xf7\x1e" },
   { "ADBFCHDJELFNGPHRITJVKXLZM\\N^O`PbQd", "ADBF\xf7\x1f" },
   { "ADBFCHDJELFNGPHRITJVKXLZM\\N^O`PbQdRf", "ADBF\xf7\x1f""Rf" },
   { "zxyvxt", "zxyvxt" },
   { "zxyvxtwr", "zxyv\xf7\x12" },
   { "zxyvxtwrvp", "zxyv\xf7\x13" },
   { "zxyvxtwrvpun", "zxyv\xf7\x14" },
   { "zxyvxtwrvpuntl", "zxyv\xf7\x15" },
   { "zxyvxtwrvpuntlsj", "zxyv\xf7\x16" },
   { "zxyvxtwrvpuntlsjrh", "zxyv\xf7\x17" },
   { "zxyvxtwrvpuntlsjrhqf", "zxyv\xf7\x18" },
   { "zxyvxtwrvpuntlsjrhqfpd", "zxyv\xf7\x19" },
   { "zxyvxtwrvpuntlsjrhqfpdob", "zxyv\xf7\x1a" },
   { "zxyvxtwrvpuntlsjrhqfpdobn`", "zxyv\xf7\x1b" },
   { "zxyvxtwrvpuntlsjrhqfpdobn`m^", "zxyv\xf7\x1c" },
   { "zxyvxtwrvpuntlsjrhqfpdobn`m^l\\", "zxyv\xf7\x1d" },
   { "zxyvxtwrvpuntlsjrhqfpdobn`m^l\\kZ", "zxyv\xf7\x1e" },
   { "zxyvxtwrvpuntlsjrhqfpdobn`m^l\\kZjX", "zxyv\xf7\x1f" },
   { "zxyvxtwrvpuntlsjrhqfpdobn`m^l\\kZjXiV", "zxyv\xf7\x1f""iV" },
   { "zwyuxs", "zwyuxs" },
   { "zwyuxswq", "zwyu\xf7\x12" },
   { "zwyuxswqvo", "zwyu\xf7\x13" },
   { "zwyuxswqvoum", "zwyu\xf7\x14" },
   { "zwyuxswqvoumtk", "zwyu\xf7\x15" },
   { "zwyuxswqvoumtksi", "zwyu\xf7\x16" },
   { "zwyuxswqvoumtksirg", "zwyu\xf7\x17" },
   { "zwyuxswqvoumtksirgqe", "zwyu\xf7\x18" },
   { "zwyuxswqvoumtksirgqepc", "zwyu\xf7\x19" },
   { "zwyuxswqvoumtksirgqepcoa", "zwyu\xf7\x1a" },
   { "zwyuxswqvoumtksirgqepcoan_", "zwyu\xf7\x1b" },
   { "zwyuxswqvoumtksirgqepcoan_m]", "zwyu\xf7\x1c" },
   { "zwyuxswqvoumtksirgqepcoan_m]l[", "zwyu\xf7\x1d" },
   { "zwyuxswqvoumtksirgqepcoan_m]l[kY", "zwyu\xf7\x1e" },
   { "zwyuxswqvoumtksirgqepcoan_m]l[kYjW", "zwyu\xf7\x1f" },
   { "zwyuxswqvoumtksirgqepcoan_m]l[kYjWiU", "zwyu\xf7\x1f""iU" },
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
