// Copyright (c) 2014 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstdlib>
#  include <vector>
#  include <iostream>
#  include <stdexcept>
#endif

#include "base64.h"
#include "sha256.h"
#include "utilities.h"
#include "crypto_keys.h"

using namespace std;

namespace
{

#include "ciyam_constants.h"

struct account
{
   uint64_t id;
   string seed;
   vector< string > secrets;
};

string get_hash( const string& seed, size_t round, size_t rounds )
{
   unsigned char buf[ 32 ];

   sha256 hash( seed );
   hash.copy_digest_to_buffer( buf );

   for( size_t i = round; i <= rounds; i++ )
   {
      hash.update( buf, sizeof( buf ) );
      hash.copy_digest_to_buffer( buf );
   }

   return string( ( const char* )buf, sizeof( buf ) );
}

uint64_t get_account_id( const string& hash, int offset = 0 )
{
   uint64_t id = *( uint64_t* )( hash.c_str( ) );

   if( offset )
      id *= offset;

   id >>= 28;

   return id;
}

uint64_t get_account_hit( const string& hash, uint64_t id, size_t height )
{
   uint64_t val = *( uint64_t* )( hash.c_str( ) );

   val *= height;

   val >>= 28;

   if( val > id )
      return val - id;
   else
      return id - val;
}

string generate_blockchain_script(
 const string& chain_meta, size_t initial_accounts, size_t rounds, const char* p_seed = 0 )
{
   string seed;
   bool had_fixed_seed = false;

   if( p_seed && string( p_seed ).size( ) )
   {
      had_fixed_seed = true;
      seed = string( p_seed );
   }
   else
      seed = uuid( ).as_string( );

   string root_seed( seed );
   private_key root_priv_key( root_seed );

   uint64_t root_id( get_account_id( root_seed ) );
   string root_pub_key( root_priv_key.get_public( true, true ) );

   string script( "file_raw -core -full blob " );

   string header( "blk:a=" + to_string( root_id ) + ",h=0,cm=r:" + chain_meta + ",pk=" + root_pub_key );

   script += header + "\\n";
   string validate( header );

   script += "\\";

   vector< account > accounts;

   for( size_t i = 0; i < initial_accounts; i++ )
   {
      account new_account;

      new_account.id = get_account_id( root_pub_key, i + 1 );
      new_account.seed = to_string( new_account.id );

      for( size_t j = 0; j <= rounds; j++ )
      {
         private_key priv_key( new_account.seed + to_string( j ) );
         new_account.secrets.push_back( priv_key.get_secret( true ) );
      }

      accounts.push_back( new_account );

      private_key priv_key( new_account.secrets[ 0 ] );

      string next_account( "a:" + to_string( new_account.id ) + ",h="
       + base64::encode( get_hash( new_account.seed, 0, rounds ) )
       + ",l=" + priv_key.get_address( true, true ) );

      script += "\n" + next_account + "\\n";
      validate += "\n" + next_account;

      script += "\\";
   }

   script += "\ns:" + root_priv_key.construct_signature( validate, true ) + '\n';
   script += "file_kill %OUTPUT%\n";

   sha256 hash( string( c_file_type_str_core_blob ) + validate );

   uint64_t total_weight = 0;
   size_t num_accounts( initial_accounts );

   string last_hash( "0" + root_pub_key );
   string block_hash( hash.get_digest_as_string( ) );

   vector< string > last_hashes;
   vector< string > block_hashes;
   vector< uint64_t > total_weights;

   for( size_t i = 1; i <= rounds; i++ )
   {
      uint64_t least_weight = 0;
      string next_last_hash, next_block_hash;

      vector< string > new_last_hashes;
      vector< string > new_block_hashes;
      vector< uint64_t > new_total_weights;

      for( size_t j = 0; j < num_accounts; j++ )
      {
         string previous_block( block_hash );

         // NOTE: If not wanting determinstic data then randomly choose a previous block.
         if( !had_fixed_seed && !block_hashes.empty( ) )
         {
            size_t offset = rand( ) % block_hashes.size( );

            last_hash = last_hashes[ offset ];
            total_weight = total_weights[ offset ];
            previous_block = block_hashes[ offset ];
         }

         uint64_t next_weight = get_account_hit( last_hash, accounts[ j ].id, i );
         string next_hash( base64::encode( get_hash( accounts[ j ].seed, i, rounds ) ) );

         string data( "blk:a=" + to_string( root_id )
          + "." + to_string( accounts[ j ].id ) + ",h=" + to_string( i ) );

         validate = data;
         data += "\\\n";

         string next( ",w=" + to_string( next_weight ) + ",ah=" + next_hash );

         validate += next;
         data += next + "\\\n";

         private_key priv_key( accounts[ j ].secrets[ i ] );
         next = ",al=" + priv_key.get_address( true, true ) + ",pb=" + previous_block;

         data += next;
         validate += next;

         private_key sign_key( accounts[ j ].secrets[ i - 1 ] );

         next = ",pk=" + sign_key.get_public( true, true ) + ",tw=" + to_string( total_weight + next_weight );

         data += "\\\n" + next;
         validate += next;

         sha256 new_hash( string( c_file_type_str_core_blob ) + validate );

         new_last_hashes.push_back( next_hash );
         new_block_hashes.push_back( new_hash.get_digest_as_string( ) );
         new_total_weights.push_back( total_weight + next_weight );

         if( !least_weight || next_weight < least_weight )
         {
            least_weight = next_weight;

            next_last_hash = next_hash;
            next_block_hash = new_block_hashes.back( );
         }

         data += "\\n\\";

         script += "file_raw -core -full blob " + data + "\ns:" + sign_key.construct_signature( validate, true ) + '\n';
         script += "file_kill %OUTPUT%\n";
      }

      last_hash = next_last_hash;
      block_hash = next_block_hash;
      total_weight += least_weight;

      last_hashes = new_last_hashes;
      block_hashes = new_block_hashes;
      total_weights = new_total_weights;
   }

   return script;
}

}

int main( int argc, char* argv[ ] )
{
   try
   {
      if( argc < 4 )
      {
         cout << "usage: test_blockchain <chain_meta> <num_accounts> <num_rounds> [<seed>]\n";
         return 0;
      }
      else
      {
         string chain_meta( argv[ 1 ] );
         size_t num_accounts = from_string< size_t >( argv[ 2 ] );
         size_t num_rounds = from_string< size_t >( argv[ 3 ] );

         string seed;
         if( argc > 4 )
            seed = string( argv[ 4 ] );

         if( seed.length( ) && seed.length( ) < 8 )
            throw runtime_error( "seed must be at least 8 bytes" );

         if( num_accounts < 1 || num_accounts > 100 )
            throw runtime_error( "num accounts must be in the range 1..100" );

         cout << generate_blockchain_script( chain_meta, num_accounts, num_rounds, seed.empty( ) ? "" : seed.c_str( ) ) << endl;
      }
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
}

