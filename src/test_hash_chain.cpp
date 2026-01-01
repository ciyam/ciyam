// Copyright (c) 2014-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <memory.h>
#  include <vector>
#  include <iostream>
#  include <stdexcept>
#endif

#include "sha256.h"
#include "utilities.h"
#include "hash_chain.h"

using namespace std;

const int c_test_rounds = 100;

const char* const c_test_hash_chain_internal = "~test_hash_chain_internal";
const char* const c_test_hash_chain_external = "~test_hash_chain_external";

const char* const c_test_hash_chain_password = "password";

int main( )
{
   unsigned char buffer[ c_sha256_digest_size ];

   memset( buffer, 0, c_sha256_digest_size );

   try
   {
      cout << "01. create seedless internal hash chain and get the last hash\n";
      hash_chain chain_internal( c_test_hash_chain_internal, true, c_test_rounds, false );

      cout << chain_internal.get_next_hashes_to_publish( c_test_hash_chain_password ) << '\n';

      cout << "\n02. manually verify the last hash in the chain matches expected\n";
      sha256 seed_hash( "" );
      seed_hash.copy_digest_to_buffer( buffer );

      sha256 hash( c_test_hash_chain_password );
      hash.update( buffer, c_sha256_digest_size );

      string last, next;
      for( int i = 0; i < c_test_rounds; i++ )
      {
         hash.copy_digest_to_buffer( buffer );
         hash.update( buffer, sizeof( buffer ) );

         if( last.empty( ) )
            last = hash.get_digest_as_string( );
         else if( i == c_test_rounds - 2 )
            next = hash.get_digest_as_string( );
      }

      cout << hash.get_digest_as_string( ) << '\n';

      hash_chain chain_external( c_test_hash_chain_external, true );
      chain_external.check_and_update_if_good( hash.get_digest_as_string( ) );

      string new_hash_value( chain_internal.get_next_hashes_to_publish( c_test_hash_chain_password ) );

      cout << "\n03. create external hash chain and verify the next internal hash is valid\n";
      if( chain_external.check_and_update_if_good( new_hash_value ) )
         cout << "pass\n";
      else
         cout << "fail\n";

      cout << new_hash_value << '\n';

      cout << "\n04. check manually calculated next hash matches this new internal hash value\n";
      cout << next << '\n';

      cout << "\n05. verify an internal hash generated with the wrong password is not accepted\n";
      new_hash_value = chain_internal.get_next_hashes_to_publish( "" );
      if( chain_external.check_and_update_if_good( new_hash_value ) )
         cout << "fail\n";
      else
         cout << "pass\n";

      cout << "\n06. check with limit one that external chain will not accept a now correct hash\n";
      new_hash_value = chain_internal.get_next_hashes_to_publish( c_test_hash_chain_password );
      if( chain_external.check_and_update_if_good( new_hash_value, 1 ) )
         cout << "fail\n";
      else
         cout << "pass\n";

      cout << "\n07. check with limit three the external chain now accepts the next correct hash\n";
      new_hash_value = chain_internal.get_next_hashes_to_publish( c_test_hash_chain_password );
      if( chain_external.check_and_update_if_good( new_hash_value, 3 ) )
         cout << "pass\n";
      else
         cout << "fail\n";

      cout << "\n08. check fetching two hashes at once are both verified by the external chain\n";
      new_hash_value = chain_internal.get_next_hashes_to_publish( c_test_hash_chain_password, 2 );

      vector< string > new_hashes;
      split( new_hash_value, new_hashes );

      if( new_hashes.size( ) != 2 )
         throw runtime_error( "did not return 2 hashes when expected to" );

      if( chain_external.check_and_update_if_good( new_hashes[ 1 ] )
       && chain_external.check_and_update_if_good( new_hashes[ 0 ] ) )
         cout << "pass\n";
      else
         cout << "fail\n";

      while( !chain_internal.has_been_depleted( ) )
      {
         new_hash_value = chain_internal.get_next_hashes_to_publish( c_test_hash_chain_password );
         chain_external.check_and_update_if_good( new_hash_value );
      }

      cout << "\n09. check all remaining internal chain hashes are verified by the external chain\n";
      cout << new_hash_value << '\n';

      cout << "\n10. check that the last hash matches what was manually calculated\n";
      cout << new_hash_value << '\n';

      memset( buffer, 0, c_sha256_digest_size );

      cout << "\n11. check that copying leading digest bytes works as expected\n";
      for( unsigned int i = 0; i <= c_sha256_digest_size + 1; i++ )
      {
         hash.copy_digest_to_buffer( buffer, i );
         cout << hex_encode( buffer, c_sha256_digest_size ) << '\n';
      }

      file_remove( c_test_hash_chain_internal );
      file_remove( c_test_hash_chain_external );
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

