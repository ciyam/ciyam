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
#endif

#include "crypto_keys.h"

#include "utilities.h"

//#define DEBUG

using namespace std;

int main( )
{
   try
   {
      private_key skey( "18E14A7B6A307F426A94F8114701E7C8E774E7F9A47E2C2035DB29A206321725" );

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

      private_key priv_key( secret );
      cout << "\npriv_key.verify_signature( \"This is a test\", signature ) = " << priv_key.verify_signature( "This is a test", signature ) << endl;
      cout << "priv_key.verify_signature( \"This is a test.\", signature ) = " << priv_key.verify_signature( "This is a test.", signature ) << endl;

      private_key priv_key1( "710e7b873ca568c965b4d5de2e5f82359d625e8e4b258bae422903bd5aa8f939" );
      private_key priv_key2( "720e7b873ca568c965b4d5de2e5f82359d625e8e4b258bae422903bd5aa8f939" );

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
       "76a9143a89c372f5308cb58828979bb624f7e8822672ed88ac", new private_key( priv_key2.get_wif_secret( ), true ) ) );
#endif

      vector< output_information > outputs;
      outputs.push_back( output_information( 20000, "1ciyam3htJit1feGa26p2wQ4aw6KFTejU" ) );

      cout << "\n" << construct_raw_transaction( inputs, outputs ) << endl;
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

   return 0;
}

