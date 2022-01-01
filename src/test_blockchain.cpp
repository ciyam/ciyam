// Copyright (c) 2014-2022 CIYAM Developers
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
#  include <algorithm>
#endif

#include "base64.h"
#include "sha256.h"
#include "date_time.h"
#include "utilities.h"
#include "crypto_keys.h"

using namespace std;

namespace
{

#include "ciyam_constants.h"

struct account
{
   account( ) : id( 0 ), skips( 0 ), num_transactions( 0 ) { }

   uint64_t id;

   string seed;
   string rseed;

   unsigned int skips;

   unsigned long num_transactions;

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
   uint64_t id = *( uint64_t* )( offset == 0 ? hash.c_str( ) : get_hash( hash, 0, offset ).c_str( ) );

   id >>= 28;

   return id;
}

uint64_t get_next_weight( const string& hash, uint64_t id, size_t height )
{
   uint64_t val = *( uint64_t* )( hash.c_str( ) );

   val >>= 28;

   uint64_t hit = *( uint64_t* )( get_hash( to_string( id ) + to_string( height ), 0, 0 ).c_str( ) );

   hit >>= 28;

   if( val > hit )
      return val - hit;
   else
      return hit - val;
}

string generate_blockchain_script( const string& chain_meta,
 size_t initial_accounts, size_t rounds, const char* p_seed = 0, int tolerance_multiplier = 0 )
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

   string script( ".session_variable @debug_blockchain 1\n" );
   script += ".file_raw -core blob ";

   string header( string( c_file_type_core_block_object ) + ":"
    + string( c_file_type_core_block_header_account_prefix ) + to_string( root_id )
    + "," + string( c_file_type_core_block_header_height_prefix ) + string( "0" )
    + "," + string( c_file_type_core_block_header_chain_meta_prefix )
    + string( c_file_type_core_block_header_chain_meta_requisite_prefix ) + chain_meta
    + "," + string( c_file_type_core_block_header_public_key_prefix ) + root_pub_key );

   uint64_t initial_tolerance = 0;
   string::size_type pos = chain_meta.find( '<' );
   if( pos != string::npos )
      initial_tolerance = from_string< uint64_t >( chain_meta.substr( pos + 1 ) );

   script += header + "\\n";
   string validate( header );

   script += "\\";

   vector< account > accounts;

   for( size_t i = 0; i < initial_accounts; i++ )
   {
      account new_account;

      new_account.id = get_account_id( root_pub_key, i + 1 );
      new_account.seed = new_account.rseed = to_string( new_account.id );

      reverse( new_account.rseed.begin( ), new_account.rseed.end( ) );

      for( size_t j = 0; j <= rounds * 2; j++ )
      {
         private_key priv_key( new_account.seed + to_string( j ) );
         new_account.secrets.push_back( priv_key.get_secret( true ) );
      }

      accounts.push_back( new_account );

      private_key priv_key( new_account.secrets[ 0 ] );
      private_key tx_priv_key( new_account.secrets[ rounds ] );

      string encoded_bhash( base64::encode( get_hash( new_account.seed, 0, rounds ) ) );

      string next_account( string( c_file_type_core_block_detail_account_prefix ) + to_string( new_account.id )
       + "," + string( c_file_type_core_block_detail_account_exp_prefix ) + to_string( 2 )
       + "," + string( c_file_type_core_block_detail_account_hash_prefix ) + encoded_bhash
       + "," + string( c_file_type_core_block_detail_account_lock_prefix ) + priv_key.get_address( true, true ) );

      next_account += ","
       + string( c_file_type_core_block_detail_account_msg_key_prefix ) + priv_key.get_public( false, true );

      string encoded_thash( base64::encode( get_hash( new_account.rseed, 0, rounds ) ) );

      next_account += "," + string( c_file_type_core_block_detail_account_tx_hash_prefix ) + encoded_thash
       + "," + string( c_file_type_core_block_detail_account_tx_lock_prefix ) + tx_priv_key.get_address( true, true );

      script += "\n" + next_account + "\\n";
      validate += "\n" + next_account;

      script += "\\";
   }

   script += "\n"
    + string( c_file_type_core_block_detail_signature_prefix )
    + root_priv_key.construct_signature( validate, true ) + '\n';

   sha256 hash( string( c_file_type_str_core_blob ) + validate );

   uint64_t total_weight = 0;
   uint64_t odd_total_weight = 0;
   uint64_t even_total_weight = 0;

   size_t num_accounts( initial_accounts );

   string last_hash( "0" + root_pub_key );
   string last_odd_hash( last_hash );
   string last_even_hash( last_hash );

   string block_hash( hash.get_digest_as_string( ) );

   string odd_block_hash( block_hash );
   string even_block_hash( block_hash );

   vector< string > last_hashes;
   vector< string > block_hashes;
   vector< string > last_tx_hashes;
   vector< uint64_t > total_weights;

   for( size_t i = 1; i <= rounds; i++ )
   {
      uint64_t least_weight = UINT64_C( 0xffffffffffffffff );
      uint64_t least_weight_odd = UINT64_C( 0xffffffffffffffff );
      uint64_t least_weight_even = UINT64_C( 0xffffffffffffffff );

      string next_last_hash, next_block_hash;
      string next_odd_last_hash, next_odd_block_hash;
      string next_even_last_hash, next_even_block_hash;

      uint64_t current_tolerance = 0;
      if( initial_tolerance )
         current_tolerance = initial_tolerance / num_accounts;

      vector< string > new_last_hashes;
      vector< string > new_block_hashes;
      vector< uint64_t > new_total_weights;

      string skipped;

      for( size_t j = 0; j < num_accounts; j++ )
      {
         string previous_block( block_hash );

         // NOTE: If not wanting determinstic data then randomly decide whether to skip.
         if( !had_fixed_seed && rand( ) % num_accounts == j )
         {
            if( !skipped.empty( ) )
               skipped += ' ';
            skipped += to_string( accounts[ j ].id );
            ++accounts[ j ].skips;
            continue;
         }

         // NOTE: If not wanting determinstic data then randomly choose a previous block.
         if( !had_fixed_seed && !block_hashes.empty( ) )
         {
            size_t offset = rand( ) % block_hashes.size( );

            last_hash = last_hashes[ offset ];
            total_weight = total_weights[ offset ];
            previous_block = block_hashes[ offset ];
         }

         // NOTE: If the tolerance multiplier is -1 then each account will always link to
         // to its own last minted block.
         if( tolerance_multiplier == -1 && !block_hashes.empty( ) )
         {
            last_hash = last_hashes[ j ];
            total_weight = total_weights[ j ];
            previous_block = block_hashes[ j ];
         }

         // NOTE: If the tolerance multiplier is -2 then each account will always link to
         // one of two separate chains (depending whether account offset is odd or even).
         if( tolerance_multiplier == -2 && !block_hashes.empty( ) )
         {
            if( j % 2 == 1 )
            {
               last_hash = last_odd_hash;
               total_weight = odd_total_weight;
               previous_block = odd_block_hash;
            }
            else
            {
               last_hash = last_even_hash;
               total_weight = even_total_weight;
               previous_block = even_block_hash;
            }
         }

         uint64_t next_weight = get_next_weight( last_hash, accounts[ j ].id, i );

         // NOTE: If a positive tolerance multiplier was provided then skip minting
         // if the weight is above the current tolerance.
         if( tolerance_multiplier > 0 && current_tolerance )
         {
            if( next_weight > tolerance_multiplier * current_tolerance )
            {
               if( !skipped.empty( ) )
                  skipped += ' ';
               skipped += to_string( accounts[ j ].id );
               ++accounts[ j ].skips;
               continue;
            }
         }

         ++accounts[ j ].num_transactions;

         string next_hash( base64::encode( get_hash( accounts[ j ].seed, i - accounts[ j ].skips, rounds ) ) );

         string data( string( c_file_type_core_block_object ) + ":"
          + string( c_file_type_core_block_header_account_prefix )
          + to_string( root_id ) + "." + to_string( accounts[ j ].id )
          + "," + string( c_file_type_core_block_header_height_prefix ) + to_string( i ) );

         validate = data;
         data += "\\\n";

         string next( "," + string( c_file_type_core_block_header_weight_prefix ) + to_string( next_weight )
          + "," + string( c_file_type_core_block_header_account_hash_prefix ) + next_hash );

         validate += next;
         data += next + "\\\n";

         private_key priv_key( accounts[ j ].secrets[ i - accounts[ j ].skips ] );
         next = "," + string( c_file_type_core_block_header_account_lock_prefix ) + priv_key.get_address( true, true )
          + "," + string( c_file_type_core_block_header_previous_block_prefix ) + previous_block;

         data += next;
         validate += next;

         private_key sign_key( accounts[ j ].secrets[ i - accounts[ j ].skips - 1 ] );

         next = "," + string( c_file_type_core_block_header_public_key_prefix ) + sign_key.get_public( true, true )
          + "," + string( c_file_type_core_block_header_total_weight_prefix ) + to_string( total_weight + next_weight );

         data += "\\\n" + next;
         validate += next;

         sha256 new_hash( string( c_file_type_str_core_blob ) + validate );

         new_last_hashes.push_back( next_hash );
         new_block_hashes.push_back( new_hash.get_digest_as_string( ) );
         new_total_weights.push_back( total_weight + next_weight );

         if( next_weight < least_weight )
         {
            least_weight = next_weight;

            next_last_hash = next_hash;
            next_block_hash = new_block_hashes.back( );
         }

         if( j % 2 == 1 && next_weight < least_weight_odd )
         {
            least_weight_odd = next_weight;

            next_odd_last_hash = next_hash;
            next_odd_block_hash = new_block_hashes.back( );
         }

         if( j % 2 == 0 && next_weight < least_weight_even )
         {
            least_weight_even = next_weight;

            next_even_last_hash = next_hash;
            next_even_block_hash = new_block_hashes.back( );
         }

         data += "\\n\\";

         script += ".file_raw -core blob " + data + "\n"
          + string( c_file_type_core_block_detail_signature_prefix )
          + sign_key.construct_signature( validate, true ) + '\n';
      }

      script += "#height " + to_string( i ) + "\n";
      if( !skipped.empty( ) )
         script += "#skipped " + skipped + "\n";

      script += "file_tags c" + to_string( root_id ) + ".b*-*\n";
      script += "file_tags c" + to_string( root_id ) + ".a*.b*\n";
      script += "file_tags c" + to_string( root_id ) + ".c*\n";

      last_hash = next_last_hash;
      block_hash = next_block_hash;
      total_weight += least_weight;

      last_odd_hash = next_odd_last_hash;
      odd_block_hash = next_odd_block_hash;
      odd_total_weight += least_weight_odd;

      last_even_hash = next_even_last_hash;
      even_block_hash = next_even_block_hash;
      even_total_weight += least_weight_even;

      last_hashes = new_last_hashes;
      block_hashes = new_block_hashes;
      total_weights = new_total_weights;
   }

   script += "file_stats\n";

   return script;
}

}

int main( int argc, char* argv[ ] )
{
   try
   {
      if( argc < 4 )
      {
         cout << "usage: test_blockchain <chain_meta> <num_accounts> <num_rounds> [<seed> [<tolerance multiplier>]]\n";
         return 0;
      }
      else
      {
         string chain_meta( "T" + string( argv[ 1 ] ) );
         size_t num_accounts = from_string< size_t >( argv[ 2 ] );
         size_t num_rounds = from_string< size_t >( argv[ 3 ] );

         string seed;
         if( argc > 4 )
            seed = string( argv[ 4 ] );

         int tolerance_multiplier = 0;
         if( argc > 5 )
            tolerance_multiplier = atoi( argv[ 5 ] );

         if( seed.length( ) && seed.length( ) < 8 )
            throw runtime_error( "seed must be at least 8 bytes" );

         if( num_accounts < 1 || num_accounts > 100 )
            throw runtime_error( "num accounts must be in the range 1..100" );

         cout << generate_blockchain_script( chain_meta, num_accounts,
          num_rounds, seed.empty( ) ? "" : seed.c_str( ), tolerance_multiplier ) << endl;
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

