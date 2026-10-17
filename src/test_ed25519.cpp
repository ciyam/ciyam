// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <iostream>
#  include <stdexcept>
#  include <string>
#endif

#include "ed25519.h"

#include "utilities.h"

using namespace std;

// NOTE: Ed25519 against RFC 8032's own tests (checked independently with OpenSSL, through Python's "cryptography"), and the
// X25519 shared secret from two of their key pairs against the same: X25519 of one's clamped scalar with the other's public
// key in its Montgomery form - "5166f24a...".

namespace
{

int failures = 0;

void check( const string& label, bool okay )
{
   cout << ( okay ? "(okay) " : "*** error *** " ) << label << endl;

   if( !okay )
      ++failures;
}

struct rfc8032_test
{
   const char* p_seed;
   const char* p_message;
   const char* p_public_key;
   const char* p_signature;
};

const rfc8032_test c_rfc8032_tests[ ] =
{
   {
      "9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60",
      "",
      "d75a980182b10ab7d54bfed3c964073a0ee172f3daa62325af021a68f707511a",
      "e5564300c360ac729086e2cc806e828a84877f1eb8e5d974d873e065224901555fb8821590a33bacc61e39701cf9b46bd25bf5f0595bbe24655141438e7a100b"
   },
   {
      "4ccd089b28ff96da9db6c346ec114e0f5b8a319f35aba624da8cf6ed4fb8a6fb",
      "72",
      "3d4017c3e843895a92b70aa74d1b7ebc9c982ccf2ec4968cc0cd55f12af4660c",
      "92a009a9f0d4cab8720e820b5f642540a2b27b5416503f8fb3762223ebdb69da085ac1e43e15996e458f3613d0f11d8c387b2eaeb4302aeeb00d291612bb0c00"
   },
   {
      "c5aa8df43f9f837bedb7442f31dcb7b166d38535076f094b85ce3a2e0b4458f7",
      "af82",
      "fc51cd8e6218a1a38da47ed00230f0580816ed13ba3303ac5deb911548908025",
      "6291d657deec24024827e69c3abe01a30ce548a284743a445e3680d7db5ac3ac18ff9b538d16f290ae67f760984dc6594a7c15e9716ed28dc027beceea1ec40a"
   }
};

const char* const c_shared_from_tests_1_and_2 = "5166f24a6918368e2af831a4affadd97af0ac326bdf143596c045967cc00230e";

// NOTE: S + L, little endian - the same signature in a second form, which RFC 8032 says must be refused.
string with_group_order_added( const string& signature_hex )
{
   const unsigned char c_group_order[ ] =
   {
      0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58, 0xd6, 0x9c, 0xf7, 0xa2, 0xde, 0xf9, 0xde, 0x14,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10
   };

   unsigned char signature[ c_ed25519_signature_size ];

   hex_decode( signature_hex, signature, sizeof( signature ) );

   unsigned int carry = 0;

   for( size_t i = 0; i < 32; i++ )
   {
      unsigned int sum = signature[ 32 + i ] + c_group_order[ i ] + carry;

      signature[ 32 + i ] = ( unsigned char )( sum & 0xff );
      carry = sum >> 8;
   }

   return hex_encode( signature, sizeof( signature ) );
}

}

int main( )
{
   try
   {
      for( size_t i = 0; i < sizeof( c_rfc8032_tests ) / sizeof( c_rfc8032_tests[ 0 ] ); i++ )
      {
         const rfc8032_test& test( c_rfc8032_tests[ i ] );

         string label( "RFC 8032 test " + to_string( i + 1 ) );
         string message( hex_decode( test.p_message ) );

         ed25519_key_pair keys( test.p_seed );

         check( label + " - the public key", keys.get_public( ) == test.p_public_key );
         check( label + " - the signature", keys.sign( message ) == test.p_signature );
         check( label + " - verified", ed25519_verify( message, test.p_signature, test.p_public_key ) );
         check( label + " - another message refused", !ed25519_verify( message + "x", test.p_signature, test.p_public_key ) );
         check( label + " - S + L refused", !ed25519_verify( message, with_group_order_added( test.p_signature ), test.p_public_key ) );
      }

      ed25519_key_pair first( c_rfc8032_tests[ 0 ].p_seed );
      ed25519_key_pair second( c_rfc8032_tests[ 1 ].p_seed );

      check( "X25519 - the first's secret with the second's public key", first.construct_shared( second.get_public( ) ) == c_shared_from_tests_1_and_2 );
      check( "X25519 - the second's with the first's, the same", second.construct_shared( first.get_public( ) ) == c_shared_from_tests_1_and_2 );

      ed25519_key_pair random_one;
      ed25519_key_pair random_two;

      check( "random key pairs - different", random_one.get_public( ) != random_two.get_public( ) );
      check( "random key pairs - one shared secret between them",
       random_one.construct_shared( random_two.get_public( ) ) == random_two.construct_shared( random_one.get_public( ) ) );
      check( "random key pairs - a signature verified", ed25519_verify( "hello", random_one.sign( "hello" ), random_one.get_public( ) ) );

      bool refused = false;

      try
      {
         // NOTE: The identity point - small order, so its shared secret is all zeros whoever's key it meets.
         first.construct_shared( "0100000000000000000000000000000000000000000000000000000000000000" );
      }
      catch( exception& )
      {
         refused = true;
      }

      check( "a public key of small order refused for a shared secret", refused );

      refused = false;

      try
      {
         // NOTE: A mistyped seed - "hex_decode( )" would quietly read the "x" as a zero.
         ed25519_key_pair mistyped( "x" + string( c_rfc8032_tests[ 0 ].p_seed ).substr( 1 ) );
      }
      catch( exception& )
      {
         refused = true;
      }

      check( "a seed that is not hex refused", refused );
   }
   catch( exception& x )
   {
      cout << "error: " << x.what( ) << endl;
      ++failures;
   }

   cout << ( failures ? to_string( failures ) + " failed" : "all passed" ) << endl;

   return failures ? 1 : 0;
}
