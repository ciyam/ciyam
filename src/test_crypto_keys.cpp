// Copyright (c) 2014-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <iomanip>
#  include <iostream>
#  include <stdexcept>
#  include <string>
#endif

#include "crypto_keys.h"

#include "ed25519.h"
#include "utilities.h"

//#define DEBUG

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

void check_ed25519( )
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

   // NOTE: The same through "private_key" and "public_key" on the "ed" curve.
   check( "key curves - names", ( key_curve_value( "ed" ) == e_key_curve_ed25519 )
    && ( key_curve_value( "k1" ) == e_key_curve_secp256k1 ) && ( key_curve_string( e_key_curve_ed25519 ) == "ed" ) );

   const rfc8032_test& test_2( c_rfc8032_tests[ 1 ] );

   private_key ed_key( test_2.p_seed, e_key_curve_ed25519 );

   check( "private_key - the curve", ed_key.get_curve( ) == e_key_curve_ed25519 );
   check( "private_key - the public key", ed_key.get_public( ) == test_2.p_public_key );
   check( "private_key - the secret is the seed", ed_key.get_secret( ) == test_2.p_seed );
   check( "private_key - the signature", ed_key.construct_signature( hex_decode( test_2.p_message ) ) == test_2.p_signature );

   public_key ed_public( test_2.p_public_key );

   check( "public_key - its curve from its size", ed_public.get_curve( ) == e_key_curve_ed25519 );

   string signature( ed_key.construct_signature( "hello", true ) );

   check( "public_key - a signature verified", ed_public.verify_signature( "hello", signature ) );
   check( "public_key - another message refused", !ed_public.verify_signature( "hullo", signature ) );

   unsigned char digest[ 32 ];

   for( size_t i = 0; i < sizeof( digest ); i++ )
      digest[ i ] = ( unsigned char )i;

   string digest_signature( ed_key.construct_signature( digest ) );

   check( "public_key - a signature of a digest verified", ed_public.verify_signature( digest, digest_signature ) );

   digest[ 0 ] ^= 1;

   check( "public_key - another digest refused", !ed_public.verify_signature( digest, digest_signature ) );

   public_key ed_public_base64( ed_key.get_public( true, true ), true );

   check( "public_key - from base64", ( ed_public_base64.get_curve( ) == e_key_curve_ed25519 )
    && ( ed_public_base64.get_public( ) == test_2.p_public_key ) );

   private_key ed_key_base64( ed_key.get_secret( true ), e_key_curve_ed25519 );

   check( "private_key - a secret in base64", ed_key_base64.get_public( ) == test_2.p_public_key );

   private_key ed_first( c_rfc8032_tests[ 0 ].p_seed, e_key_curve_ed25519 );
   public_key ed_first_public( ed_first.get_public( ) );

   check( "private_key - the X25519 shared secret", ed_first.construct_shared( ed_public ) == c_shared_from_tests_1_and_2 );
   check( "private_key - the same from the other side", ed_key.construct_shared( ed_first_public ) == c_shared_from_tests_1_and_2 );

   string encrypted( ed_first.encrypt_message( ed_public, "a message for the second" ) );

   check( "private_key - a message encrypted and decrypted",
    ed_key.decrypt_message( ed_first_public, encrypted ) == "a message for the second" );

   private_key ed_random( e_key_curve_ed25519 );

   check( "private_key - a random key", ( ed_random.get_curve( ) == e_key_curve_ed25519 )
    && ( ed_random.get_public( ).length( ) == c_ed25519_public_key_size * 2 ) );

   refused = false;

   try
   {
      ed_first.construct_shared( public_key( "0100000000000000000000000000000000000000000000000000000000000000" ) );
   }
   catch( exception& )
   {
      refused = true;
   }

   check( "private_key - a public key of small order refused", refused );

#ifdef SSL_SUPPORT
   private_key k1_key( e_key_curve_secp256k1 );
   public_key k1_public( k1_key.get_public( ) );

   check( "public_key - a secp256k1 key from its size", k1_public.get_curve( ) == e_key_curve_secp256k1 );

   refused = false;

   try
   {
      ed_first.construct_shared( k1_public );
   }
   catch( exception& )
   {
      refused = true;
   }

   check( "private_key - a shared secret with a key on the other curve refused", refused );

   refused = false;

   try
   {
      ed_first.get_wif_secret( );
   }
   catch( exception& )
   {
      refused = true;
   }

   check( "private_key - WIF refused for ed25519", refused );
#else
   refused = false;

   try
   {
      private_key k1_key( e_key_curve_secp256k1 );
   }
   catch( exception& )
   {
      refused = true;
   }

   check( "private_key - secp256k1 refused without SSL support", refused );
#endif
}

}

int main( )
{
   try
   {
#ifdef SSL_SUPPORT
      private_key skey( "18E14A7B6A307F426A94F8114701E7C8E774E7F9A47E2C2035DB29A206321725", e_key_curve_secp256k1 );

      string key( skey.get_public( ) );
      string secret( skey.get_secret( ) );
      cout << "skey.get_public( ) = " << key << endl;
      cout << "skey.get_public( true, true ) = " << skey.get_public( true, true ) << endl;

      cout << "\nskey.get_hash160( true ) = " << skey.get_hash160( true ) << endl;
      cout << "skey.get_hash160( false ) = " << skey.get_hash160( false ) << endl;

      cout << "\nskey.get_secret( ) = " << secret << endl;

      cout << "\nskey.get_address( true ) = " << skey.get_address( true ) << endl;
      cout << "skey.get_address( false ) = " << skey.get_address( false ) << endl;

      cout << "\nskey.get_wif_secret( true ) = " << skey.get_wif_secret( true ) << endl;
      cout << "skey.get_wif_secret( false ) = " << skey.get_wif_secret( false ) << endl;

      string signature( skey.construct_signature( "This is a test.", true ) );
#ifndef DEBUG
      cout << "\nskey.construct_signature( \"This is a test.\" )" << endl;
#else
      cout << "\nskey.construct_signature( \"This is a test.\" )\n" << signature << endl;
#endif
      cout << "\nskey.verify_signature( \"This is a test\", signature ) = " << skey.verify_signature( "This is a test", signature ) << endl;
      cout << "skey.verify_signature( \"This is a test.\", signature ) = " << skey.verify_signature( "This is a test.", signature ) << endl;

      public_key pub_key( key );
      cout << "\npub_key.verify_signature( \"This is a test\", signature ) = " << pub_key.verify_signature( "This is a test", signature ) << endl;
      cout << "pub_key.verify_signature( \"This is a test.\", signature ) = " << pub_key.verify_signature( "This is a test.", signature ) << endl;

      private_key priv_key( secret, e_key_curve_secp256k1 );
      cout << "\npriv_key.verify_signature( \"This is a test\", signature ) = " << priv_key.verify_signature( "This is a test", signature ) << endl;
      cout << "priv_key.verify_signature( \"This is a test.\", signature ) = " << priv_key.verify_signature( "This is a test.", signature ) << endl;

      private_key priv_key1( "710e7b873ca568c965b4d5de2e5f82359d625e8e4b258bae422903bd5aa8f939", e_key_curve_secp256k1 );
      private_key priv_key2( "720e7b873ca568c965b4d5de2e5f82359d625e8e4b258bae422903bd5aa8f939", e_key_curve_secp256k1 );

      public_key pub_key1( priv_key1.get_public( ) );
      public_key pub_key2( priv_key2.get_public( ) );

      cout << "\npriv_key1.construct_shared( pub_key2 ) = " << priv_key1.construct_shared( pub_key2 ) << endl;
      cout << "priv_key2.construct_shared( pub_key1 ) = " << priv_key2.construct_shared( pub_key1 ) << endl;

      string message( "This is a test for encryption using a password derived from a shared secret." );
      string encrypted( priv_key1.encrypt_message( pub_key2, message ) );

      cout << "\npriv_key1.encrypt_message( pub_key2, \"" << message << "\" )" << endl;
      cout << encrypted << endl;

      cout << "\npriv_key2.decrypt_message( pub_key1, \""
       << encrypted.substr( 0, 52 ) << "...\" )\n" << priv_key2.decrypt_message( pub_key1, encrypted ) << endl;

      vector< utxo_information > inputs;
#ifndef DEBUG
      inputs.push_back( utxo_information( 0, "3c9eab4935d1b8fe2bad3af1c443c2c61ab8212c3f45c2ba59ff4a2e93c481e2" ) );
#else
      inputs.push_back( utxo_information( 0, "3c9eab4935d1b8fe2bad3af1c443c2c61ab8212c3f45c2ba59ff4a2e93c481e2",
       "76a9143a89c372f5308cb58828979bb624f7e8822672ed88ac", new private_key( priv_key2.get_wif_secret( ), true, 0, e_key_curve_secp256k1 ) ) );
#endif

      vector< output_information > outputs;
      outputs.push_back( output_information( 20000, "1ciyam3htJit1feGa26p2wQ4aw6KFTejU" ) );

      cout << "\n" << construct_raw_transaction( inputs, outputs ) << endl;

      cout << endl;
#endif

      // NOTE: Ed25519 is checked whether or not SSL is supported.
      check_ed25519( );

      cout << ( failures ? to_string( failures ) + " failed" : "all passed" ) << endl;
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
      return 1;
   }
   catch( ... )
   {
      cerr << "error: unexpected exception caught" << endl;
      return 2;
   }

   return failures ? 3 : 0;
}
