// Copyright (c) 2014 CIYAM Developers
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
      private_key skey( "700e7b873ca568c965b4d5de2e5f82359d625e8e4b258bae422903bd5aa8f939" );

      string key( skey.get_public( ) );
      string secret( skey.get_secret( ) );
      cout << "skey.get_public( )\n" << key << endl;
      cout << "\nskey.get_secret( )\n" << secret << endl;

      string signature( skey.construct_signature( "This is a test." ) );
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

