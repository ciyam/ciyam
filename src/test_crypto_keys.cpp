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

