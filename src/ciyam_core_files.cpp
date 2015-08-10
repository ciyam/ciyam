// Copyright (c) 2014-2015 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstring>
#  include <map>
#  include <set>
#  include <deque>
#  include <limits>
#  include <fstream>
#  include <stdexcept>
#endif

#ifdef __GNUG__
#  include <unistd.h>
#endif

#define CIYAM_BASE_IMPL

#include "ciyam_core_files.h"

#include "regex.h"
#include "base64.h"
#include "config.h"
#include "sha256.h"
#include "threads.h"
#include "utilities.h"
#include "ciyam_base.h"
#ifdef SSL_SUPPORT
#  include "crypto_keys.h"
#endif
#include "hash_chain.h"
#include "ciyam_files.h"

using namespace std;

namespace
{

const size_t c_id_length = 10;
const size_t c_hash_buf_size = 32;

const size_t c_tx_soft_limit = 100;

const unsigned int c_default_exp = 5;
const unsigned int c_max_core_line_size = 4096;

const uint16_t c_tx_min_non_confirmed = 5;

mutex g_mutex;

#include "ciyam_constants.h"

struct block_info
{
   block_info( ) : block_height( 0 ), block_weight( 0 ), total_weight( 0 ), had_secondary_account( false ) { }

   string minter_id;
   string minter_hash;
   string minter_lock;
   string minter_pubkey;

   string previous_block;

   uint64_t block_height;

   uint64_t block_weight;
   uint64_t total_weight;

   vector< string > transaction_hashes;

   bool had_secondary_account;
};

struct chain_info
{
   chain_info( )
    :
    is_test( false ),
    mint_charge( 0 ),
    mint_reward( 0 ),
    account_charge( 0 ),
    transaction_reward( 0 ),
    checkpoint_tolerance( 0 ),
    ocheckpoint_tolerance( 0 ),
    num_accounts( 0 ),
    checkpoint_length( 0 ),
    checkpoint_start_height( 0 )
   {
   }

   bool is_test;

   uint64_t mint_charge;
   uint64_t mint_reward;
   uint64_t account_charge;
   uint64_t transaction_reward;

   uint64_t checkpoint_tolerance;
   uint64_t ocheckpoint_tolerance;

   uint64_t num_accounts;

   uint16_t checkpoint_length;
   uint64_t checkpoint_start_height;
};

struct account_info
{
   account_info( ) : exp( 0 ), is_banned( false ), balance( 0 ), last_height( 0 ), num_transactions( 0 ) { }

   string tag;

   unsigned int exp;

   bool is_banned;

   string msg_key;

   string block_hash;
   string block_lock;

   string transaction_hash;
   string transaction_lock;

   uint64_t balance;

   uint64_t last_height;
   uint64_t num_transactions;

   string last_transaction_id;
};

struct transaction_info
{
   transaction_info( ) : sequence( 0 ), is_included_in_best_chain( false ) { }

   string account_id;

   uint64_t sequence;

   string application;
   string previous_transaction;

   vector< string > log_lines;

   bool is_included_in_best_chain;
};

string base64_to_tag_name( const string& base64 )
{
   return replaced( base64, "/", "$", "=", "-" );
}

string tag_name_to_base64( const string& tag_name )
{
   return replaced( tag_name, "$", "/", "-", "=" );
}

string get_hash( const string& seed, size_t rounds )
{
   unsigned char buf[ c_hash_buf_size ];

   sha256 hash( seed );
   hash.copy_digest_to_buffer( buf );

   for( size_t i = 0; i <= rounds; i++ )
   {
      hash.update( buf, sizeof( buf ) );
      hash.copy_digest_to_buffer( buf );
   }

   return string( ( const char* )buf, sizeof( buf ) );
}

string get_hash_secret( sha256& hash )
{
   // NOTE: The first nibble is zeroed out to ensure that the return
   // value is always valid to use as a Bitcoin address "secret".
   return "0" + hash.get_digest_as_string( ).substr( 1 );
}

void validate_ascii_hash( const string& hash )
{
   regex expr( c_regex_hash_256 );

   if( expr.search( hash ) == string::npos )
      throw runtime_error( "unexpected hash '" + hash + "'" );
}

uint64_t get_expected_weight( const string& hash, uint64_t id, uint64_t height )
{
   uint64_t val = *( uint64_t* )( hash.c_str( ) );

   val >>= 28;

   uint64_t hit = *( uint64_t* )( get_hash( to_string( id ) + to_string( height ), 0 ).c_str( ) );

   hit >>= 28;

   if( val > hit )
      return val - hit;
   else
      return hit - val;
}

string get_chain_info( chain_info& info, const string& chain )
{
   string chain_info_tag( "c" + chain );

   if( !has_tag( chain_info_tag ) )
      throw runtime_error( "unable to find blockchain meta info '" + chain_info_tag + "'" );

   string chain_info_hash = tag_file_hash( chain_info_tag );
   string chain_info_info( extract_file( chain_info_hash, "", c_file_type_char_core_blob ) );

   string::size_type pos = chain_info_info.find( ':' );

   if( pos == string::npos || chain_info_info.substr( 0, pos ) != string( c_file_type_core_blockchain_meta_object ) )
      throw runtime_error( "invalid or missing blockchain meta core file type for " + chain );

   chain_info_info.erase( 0, pos + 1 );

   vector< string > chain_info_items;
   split( chain_info_info, chain_info_items, '\n' );

   if( chain_info_items.size( ) < 2 )
      throw runtime_error( "unexpected invalid chain_info '" + chain_info_info + "'" );

   string requisites( chain_info_items[ 0 ] );

   if( !requisites.empty( ) && requisites[ 0 ] == 'T' )
   {
      info.is_test = true;
      requisites.erase( 0, 1 );
   }

   pos = requisites.find( '~' );
   if( pos != string::npos )
   {
      string::size_type npos = requisites.find( '<', pos );
      if( npos == string::npos )
         throw runtime_error( "unexpected missing length and tolerance separator in '" + requisites + "'" );

      info.checkpoint_length = from_string< uint16_t >( requisites.substr( pos + 1, npos - pos - 1 ) );
      info.checkpoint_tolerance = info.ocheckpoint_tolerance = from_string< uint64_t >( requisites.substr( npos + 1 ) );

      requisites.erase( pos );
   }

   pos = requisites.find( '!' );
   if( pos != string::npos )
   {
      info.account_charge = from_string< uint64_t >( requisites.substr( pos + 1 ) );
      requisites.erase( pos );
   }

   pos = requisites.find( '+' );
   if( pos != string::npos )
   {
      info.transaction_reward = from_string< uint64_t >( requisites.substr( pos + 1 ) );
      requisites.erase( pos );
   }

   pos = requisites.find( '-' );
   if( pos != string::npos )
   {
      info.mint_charge = from_string< uint64_t >( requisites.substr( pos + 1 ) );
      requisites.erase( pos );

      if( !info.transaction_reward )
         info.transaction_reward = info.mint_charge;
   }

   info.mint_reward = from_string< uint64_t >( requisites );

   string extras( chain_info_items[ 1 ] );

   pos = extras.find( ':' );
   if( pos != string::npos )
   {
      info.checkpoint_start_height = from_string< uint64_t >( extras.substr( pos + 1 ) );
      extras.erase( pos );
   }

   info.num_accounts = from_string< int >( extras );

   if( info.num_accounts )
      info.checkpoint_tolerance /= info.num_accounts;

   return chain_info_hash;
}

string chain_info_blob( const chain_info& cinfo, bool had_zero_explicit_account_charge )
{
   string extra( c_file_type_str_core_blob );

   extra += string( c_file_type_core_blockchain_meta_object ) + ':';

   if( cinfo.is_test )
      extra += 'T';

   extra += to_string( cinfo.mint_reward );

   if( cinfo.mint_charge )
      extra += '-' + to_string( cinfo.mint_charge );

   if( cinfo.transaction_reward )
      extra += '+' + to_string( cinfo.transaction_reward );

   if( cinfo.account_charge || had_zero_explicit_account_charge )
      extra += '!' + to_string( cinfo.account_charge );

   if( cinfo.ocheckpoint_tolerance )
      extra += '~' + to_string( cinfo.checkpoint_length ) + '<' + to_string( cinfo.ocheckpoint_tolerance );

   extra += '\n' + to_string( cinfo.num_accounts ) + ':' + to_string( cinfo.checkpoint_start_height );

   return extra;
}

string get_account_info( account_info& info, const string& account_id )
{
   string account_tag( list_file_tags( account_id + ".h*" ) );

   string::size_type pos = account_tag.find( ".h" );
   string::size_type rpos = account_tag.find( ".b" );

   if( pos == string::npos || rpos == string::npos || rpos < pos )
      throw runtime_error( "unable to find account " + account_id );

   info.tag = account_tag;

   info.last_height = from_string< uint64_t >( account_tag.substr( pos + 2, rpos - pos - 2 ) );

   info.balance = from_string< uint64_t >( account_tag.substr( rpos + 2 ) );

   if( account_tag.find( ".banned" ) != string::npos )
      info.is_banned = true;

   string account_hash( tag_file_hash( account_tag ) );
   string account_data( extract_file( account_hash, "", c_file_type_char_core_blob ) );

   pos = account_data.find( ':' );

   if( pos == string::npos || account_data.substr( 0, pos ) != string( c_file_type_core_account_object ) )
      throw runtime_error( "invalid or missing account core file type for " + account_id );

   account_data.erase( 0, pos + 1 );

   vector< string > account_items;
   split( account_data, account_items, '\n' );

   if( account_items.size( ) < 3 )
      throw runtime_error( "unexpected invalid account_data '" + account_data + "'" );

   pos = account_items[ 0 ].find( ':' );
   if( pos == string::npos )
      throw runtime_error( "missing account message key in " + account_id );

   info.exp = from_string< unsigned int >( account_items[ 0 ].substr( 0, pos ) );

   info.msg_key = account_items[ 0 ].substr( pos + 1 );

   info.block_hash = account_items[ 1 ];
   info.block_lock = account_items[ 2 ];

   if( account_items.size( ) > 3 )
   {
      if( account_items.size( ) < 5 )
         throw runtime_error( "unexpected invalid account_data '" + account_data + "'" );

      info.transaction_hash = account_items[ 3 ];
      info.transaction_lock = account_items[ 4 ];

      if( account_items.size( ) > 5 )
      {
         string extra_transaction_info( account_items[ 5 ] );
         string::size_type pos = extra_transaction_info.find( ':' );

         info.num_transactions = from_string< size_t >( extra_transaction_info.substr( 0, pos ) );
         info.last_transaction_id = extra_transaction_info.substr( pos + 1 );
      }
   }

   return account_hash;
}

string get_account_data( const account_info& info,
 const string& account_hash, const string& account_lock,
 const string& transaction_hash, const string& transaction_lock, const string& last_transaction_id )
{
   string extras( c_file_type_core_account_object );

   extras += ':' + to_string( info.exp ) + ':' + info.msg_key;

   extras += '\n' + ( account_hash.empty( ) ? info.block_hash : account_hash );
   extras += '\n' + ( account_lock.empty( ) ? info.block_lock : account_lock );

   if( !info.transaction_hash.empty( ) )
   {
      extras += '\n' + ( transaction_hash.empty( ) ? info.transaction_hash : transaction_hash );
      extras += '\n' + ( transaction_lock.empty( ) ? info.transaction_lock : transaction_lock );

      if( info.num_transactions )
      {
         extras += '\n' + to_string( info.num_transactions );

         if( !last_transaction_id.empty( ) )
            extras += ':' + last_transaction_id;
         else if( !info.last_transaction_id.empty( ) )
            extras += ':' + info.last_transaction_id;
      }
   }

   return extras;
}

string get_account_id_from_password( const string& password )
{
   unsigned char buf[ c_hash_buf_size ];

   sha256 hash( password );
   hash.copy_digest_to_buffer( buf );

   uint64_t id = *( uint64_t* )( buf );
   id >>= 28;

   return to_string( id );
}

uint64_t get_balance_from_minter_id( const string& minter_id, string* p_minter_hash = 0, string* p_minter_tag = 0 )
{
   string minter_tag( list_file_tags( minter_id + ".h*" ) );

   if( p_minter_tag )
      *p_minter_tag = minter_tag;

   if( p_minter_hash )
      *p_minter_hash = tag_file_hash( minter_tag );

   string::size_type pos = minter_tag.find( ".b" );
   if( pos == string::npos )
      throw runtime_error( "unable to find account balance in '" + minter_tag + "'" );

   return from_string< uint64_t >( minter_tag.substr( pos + 2 ) );
}

void get_ordered_checkpoint_hashes( const string& chain_id, map< uint64_t, string >& ordered_checkpoint_hashes )
{
   string all_checkpoint_tags( list_file_tags( "c" + chain_id + ".checkpoint.h*" ) );

   if( !all_checkpoint_tags.empty( ) )
   {
      vector< string > checkpoint_tags;
      split( all_checkpoint_tags, checkpoint_tags, '\n' );

      // NOTE: Need to ensure that the ordering of the checkpoint tags is
      // according to the checkpoint height.
      for( size_t i = 0; i < checkpoint_tags.size( ); i++ )
      {
         string next_tag( checkpoint_tags[ i ] );

         string::size_type pos = next_tag.find( ".h" );
         if( pos == string::npos )
            throw runtime_error( "unexpect checkpoint tag '" + next_tag + "'" );

         if( next_tag.find( ".info" ) != string::npos )
            continue;

         uint64_t height = from_string< uint64_t >( next_tag.substr( pos + 2 ) );

         ordered_checkpoint_hashes.insert( make_pair( height, tag_file_hash( next_tag ) ) );
      }
   }
}

void get_sequenced_transactions( const string& chain_id,
 multimap< uint64_t, string >& sequenced_transactions,
 bool is_test, bool no_sigs, transactions_info* p_txs_info = 0 )
{
   string all_transaction_tags( list_file_tags( "c" + chain_id + ".a*.t*" ) );

   if( !all_transaction_tags.empty( ) )
   {
      vector< string > transaction_tags;
      split( all_transaction_tags, transaction_tags, '\n' );

      map< string, string > tx_sigs;
      if( p_txs_info )
      {
         for( size_t i = 0; i < p_txs_info->transaction_hashes_with_sigs.size( ); i++ )
         {
            string next_tx( p_txs_info->transaction_hashes_with_sigs[ i ] );

            string::size_type pos = next_tx.find( ':' );
            if( pos != string::npos )
               tx_sigs.insert( make_pair( next_tx.substr( 0, pos ), next_tx.substr( pos + 1 ) ) );
         }
      }

      // NOTE: Use the transaction sequence numbers to order all transactions to be added (this will
      // ensure the ordering is correct and will also naturally favour the accounts that create less
      // transacions).
      for( size_t i = 0; i < transaction_tags.size( ); i++ )
      {
         string next_transaction_tag( transaction_tags[ i ] );
         string next_transaction_hash( tag_file_hash( next_transaction_tag ) );

         string encoded_hash( hex_to_base64( next_transaction_hash ) );

         string::size_type pos = next_transaction_tag.find( ".t" );

         if( pos != string::npos )
         {
            next_transaction_tag.erase( 0, pos + 2 );
            pos = next_transaction_tag.find( ".s" );

            string sig_suffix;

            if( pos != string::npos )
            {
               sig_suffix = ':' + tag_name_to_base64( next_transaction_tag.substr( pos + 2 ) );
               next_transaction_tag.erase( pos );
            }
            else if( tx_sigs.count( next_transaction_hash ) )
               sig_suffix = ':' + tx_sigs[ next_transaction_hash ];

            if( !no_sigs )
               encoded_hash += sig_suffix;

            sequenced_transactions.insert( make_pair(
             from_string< uint64_t >( next_transaction_tag ),
             !is_test ? encoded_hash : next_transaction_hash ) );
         }
      }
   }
}

string::size_type insert_account_into_transaction_log_line( const string& account, string& next_log_line )
{
   string::size_type pos = next_log_line.find( ' ' );
   if( pos == string::npos )
      throw runtime_error( "invalid next log line '"
       + next_log_line + "' when attempting to construct script" );

   next_log_line.insert( pos + 1, account + ' ' );

   pos = next_log_line.find( '"' );

   if( pos != string::npos )
   {
      string::size_type npos = next_log_line.find( '"', pos + 1 );

      if( npos == string::npos )
         throw runtime_error( "invalid next log line '"
          + next_log_line + "' when attempting to construct script" );
      else
      {
         string key_or_val_assign( next_log_line.substr( pos, npos - pos + 1 ) );

         if( key_or_val_assign.find( '=' ) == string::npos )
            pos = next_log_line.find( '"', npos + 1 );
         else if( key_or_val_assign.length( ) > 4 && key_or_val_assign.substr( 0, 4 ) == "\"-v=" )
            pos += 3;
      }
   }

   if( pos == string::npos )
      throw runtime_error( "invalid next log line '"
       + next_log_line + "' when attempting to construct script" );

   return pos;
}

pair< uint64_t, uint64_t > verify_block( const string& content,
 bool check_sigs, vector< pair< string, string > >* p_extras, block_info* p_block_info = 0 );

void verify_transaction( const string& content, bool check_sigs,
 vector< pair< string, string > >* p_extras, transaction_info* p_transaction_info = 0 );

void verify_blockchain_info( const string& content,
 vector< pair< string, string > >* p_extras, blockchain_info* p_blockchain_info = 0 );

void verify_checkpoint_info( const string& content,
 vector< pair< string, string > >* p_extras, checkpoint_info* p_checkpoint_info = 0 );

void verify_checkpoint_prune( const string& content, vector< pair< string, string > >* p_extras = 0 );

void verify_transactions_info( const string& content,
 vector< pair< string, string > >* p_extras, transactions_info* p_transactions_info = 0 );

pair< uint64_t, uint64_t > get_block_info( block_info& binfo, const string& block_hash )
{
   if( !has_file( block_hash ) )
      throw runtime_error( "block file '" + block_hash + "' does not exist" );

   string block_data( extract_file( block_hash, "", c_file_type_char_core_blob ) );

   string::size_type pos = block_data.find( ':' );

   if( pos == string::npos || block_data.substr( 0, pos ) != string( c_file_type_core_block_object ) )
      throw runtime_error( "invalid block file " + block_hash );

   return verify_block( block_data.substr( pos + 1 ), false, 0, &binfo );
}

bool get_transaction_info( transaction_info& tinfo, const string& transaction_hash )
{
   if( !has_file( transaction_hash ) )
      throw runtime_error( "transaction file '" + transaction_hash + "' does not exist" );

   string transaction_data( extract_file( transaction_hash, "", c_file_type_char_core_blob ) );

   string::size_type pos = transaction_data.find( ':' );

   if( pos == string::npos || transaction_data.substr( 0, pos ) != string( c_file_type_core_transaction_object ) )
      throw runtime_error( "invalid transaction file " + transaction_hash );

   verify_transaction( transaction_data.substr( pos + 1 ), false, 0, &tinfo );

   return tinfo.is_included_in_best_chain;
}

void get_transactions_info( transactions_info& txs_info, const string& transactions_info_hash )
{
   if( !has_file( transactions_info_hash ) )
      throw runtime_error( "transactions file '" + transactions_info_hash + "' does not exist" );

   string transactions_info_data( extract_file( transactions_info_hash, "", c_file_type_char_core_blob ) );

   string::size_type pos = transactions_info_data.find( ':' );

   if( pos == string::npos || transactions_info_data.substr( 0, pos ) != string( c_file_type_core_transactions_info_object ) )
      throw runtime_error( "invalid transactions info file " + transactions_info_hash );

   verify_transactions_info( transactions_info_data.substr( pos + 1 ), 0, &txs_info );
}

void get_transactions_and_hash_offsets( const string& chain_id,
 transactions_info& txs_info, map< string, size_t >& tx_hash_offsets )
{
   string txs_info_tag( "c" + chain_id + ".txinfo" );

   if( has_tag( txs_info_tag ) )
   {
      string txs_info_hash( tag_file_hash( txs_info_tag ) );
      get_transactions_info( txs_info, txs_info_hash );

      for( size_t i = 0; i < txs_info.transaction_hashes_with_sigs.size( ); i++ )
      {
         string next_hash_with_sig( txs_info.transaction_hashes_with_sigs[ i ] );

         string::size_type pos = next_hash_with_sig.find( ':' );
         tx_hash_offsets.insert( make_pair( next_hash_with_sig.substr( 0, pos ), i ) );
      }
   }
}

string get_block_hash_for_account_and_height( const string& chain_id,
 const string& account_id, uint64_t height, block_info* p_block_info = 0 )
{
   string block_hash;

   string all_block_tags( list_file_tags( "c" + chain_id + ".b" + to_string( height ) + "-*" ) );

   if( !all_block_tags.empty( ) )
   {
      vector< string > block_tags;
      split( all_block_tags, block_tags, '\n' );

      for( size_t i = 0; i < block_tags.size( ); i++ )
      {
         string next_tag( block_tags[ i ] );
         string next_hash( tag_file_hash( next_tag ) );

         block_info binfo;
         get_block_info( binfo, next_hash );

         string::size_type pos = binfo.minter_id.find( '.' );
         if( account_id == binfo.minter_id.substr( pos == string::npos ? 0 : pos + 2 ) )
         {
            block_hash = next_hash;

            if( p_block_info )
               *p_block_info = binfo;

            break;
         }
      }
   }

   return block_hash;
}

pair< uint64_t, uint64_t > verify_block( const string& content,
 bool check_sigs, vector< pair< string, string > >* p_extras, block_info* p_block_info )
{
   guard g( g_mutex );

   vector< string > lines;
   split( content, lines, '\n' );

   if( lines.empty( ) )
      throw runtime_error( "unexpected empty block content" );

   uint64_t block_height = 0;
   uint64_t previous_block_height = 0;

   uint64_t block_weight = 0;
   uint64_t total_weight = 0;
   uint64_t previous_block_weight = 0;

   string chain, account, account_hash, account_lock, previous_head, previous_block, public_key_base64;

   chain_info cinfo;

   size_t non_blob_extras = 0;
   size_t account_extra_offset = 0;

   string block_signature, past_previous_block, parallel_block_minted_minter_id, parallel_block_minted_previous_block;

   set< string > parallel_block_minted_transaction_hashes;

   unsigned int parallel_block_minted_num_transactions = 0;

   bool parallel_block_minted_had_secondary_account = false;

   string chain_info_hash;

   bool had_zero_explicit_account_charge = false;

   string header( lines[ 0 ] );
   if( header.empty( ) )
      throw runtime_error( "unexpected empty block header" );
   else
   {
      vector< string > attributes;
      split( header, attributes );

      if( attributes.empty( ) )
         throw runtime_error( "unexpected empty block header attributes" );

      bool has_height = false;
      bool has_weight = false;
      bool has_account = false;
      bool has_public_key = false;
      bool has_account_hash = false;
      bool has_account_lock = false;
      bool has_total_weight = false;
      bool has_previous_block = false;

      for( size_t i = 0; i < attributes.size( ); i++ )
      {
         string next_attribute( attributes[ i ] );
         if( next_attribute.empty( ) )
            throw runtime_error( "unexpected empty attribute in block header '" + header + "'" );

         if( !has_account )
         {
            if( next_attribute.find( c_file_type_core_block_header_account_prefix ) != 0 )
               throw runtime_error( "unexpected missing account attribute in block header '" + header + "'" );

            has_account = true;
            account = next_attribute.substr(
             string( c_file_type_core_block_header_account_prefix ).length( ) );

            string::size_type pos = account.find( '.' );

            if( pos == string::npos )
               chain = account;
            else
            {
               chain = account.substr( 0, pos );
               account.erase( 0, pos + 1 );

               chain_info_hash = get_chain_info( cinfo, chain );
            }
         }
         else if( !has_height )
         {
            if( next_attribute.find( c_file_type_core_block_header_height_prefix ) != 0 )
               throw runtime_error( "unexpected missing height attribute in block header '" + header + "'" );

            string value( next_attribute.substr(
             string( c_file_type_core_block_header_height_prefix ).length( ) ) );

            regex expr( c_regex_integer );

            if( ( value.length( ) > 1 && value[ 0 ] == '0' ) || expr.search( value ) != 0 )
               throw runtime_error( "invalid height value '" + value + "'" );

            has_height = true;

            block_height = from_string< uint64_t >( value );
         }
         else if( block_height && !has_weight )
         {
            if( next_attribute.find( c_file_type_core_block_header_weight_prefix ) != 0 )
               throw runtime_error( "unexpected missing weight attribute in block header '" + header + "'" );

            string value( next_attribute.substr(
             string( c_file_type_core_block_header_weight_prefix ).length( ) ) );

            regex expr( c_regex_integer );

            if( ( !value.empty( ) && value[ 0 ] == '0' ) || expr.search( value ) != 0 )
               throw runtime_error( "invalid weight value '" + value + "'" );

            has_weight = true;

            block_weight = from_string< uint64_t >( value );
         }
         else if( block_height && !has_account_hash )
         {
            if( next_attribute.find( c_file_type_core_block_header_account_hash_prefix ) != 0 )
               throw runtime_error( "unexpected missing account hash in block header '" + header + "'" );

            string value( next_attribute.substr(
             string( c_file_type_core_block_header_account_hash_prefix ).length( ) ) );

            base64::validate( value );

            has_account_hash = true;
            account_hash = value;
         }
         else if( block_height && !has_account_lock )
         {
            if( next_attribute.find( c_file_type_core_block_header_account_lock_prefix ) != 0 )
               throw runtime_error( "unexpected missing account lock in block header '" + header + "'" );

            has_account_lock = true;

            account_lock = next_attribute.substr(
             string( c_file_type_core_block_header_account_lock_prefix ).length( ) );
         }
         else if( !has_previous_block )
         {
            if( !block_height )
            {
               if( next_attribute.find( c_file_type_core_block_header_chain_meta_prefix ) != 0 )
                  throw runtime_error( "unexpected missing chain meta attribute in block header '" + header + "'" );

               next_attribute.erase( 0, string( c_file_type_core_block_header_chain_meta_prefix ).length( ) );

               vector< string > meta_data;
               split( next_attribute, meta_data, ';' );

               bool has_reward = false;

               for( size_t j = 0; j < meta_data.size( ); j++ )
               {
                  string next_meta( meta_data[ j ] );

                  if( next_meta.length( ) < 2 )
                     throw runtime_error( "invalid chain_meta item '" + next_meta + "' in block header '" + header + "'" );
                  else if( !has_reward )
                  {
                     if( next_meta.find( c_file_type_core_block_header_chain_meta_requisite_prefix ) != 0 )
                        throw runtime_error( "unexpected missing requisite meta item in block header '" + header + "'" );

                     next_meta.erase( 0, string( c_file_type_core_block_header_chain_meta_requisite_prefix ).length( ) );

                     if( !next_meta.empty( ) && next_meta[ 0 ] == 'T' )
                     {
                        cinfo.is_test = true;
                        next_meta.erase( 0, 1 );
                     }

                     string::size_type pos = next_meta.find( '~' );
                     if( pos != string::npos )
                     {
                        string::size_type npos = next_meta.find( '<', pos );
                        if( npos == string::npos )
                           throw runtime_error( "unexpected missing length and tolerance separator in '" + next_meta + "'" );

                        cinfo.checkpoint_length = from_string< uint16_t >( next_meta.substr( pos + 1, npos - pos - 1 ) );
                        cinfo.checkpoint_tolerance = cinfo.ocheckpoint_tolerance = from_string< uint64_t >( next_meta.substr( npos + 1 ) );

                        if( cinfo.checkpoint_length < 2 || !cinfo.checkpoint_tolerance )
                           throw runtime_error( "invalid checkpoint length and/or tolerance amounts in '" + next_meta + "'" );

                        next_meta.erase( pos );
                     }

                     pos = next_meta.find( '!' );
                     if( pos != string::npos )
                     {
                        cinfo.account_charge = from_string< uint64_t >( next_meta.substr( pos + 1 ) );
                        next_meta.erase( pos );

                        if( !cinfo.account_charge )
                           had_zero_explicit_account_charge = true;
                     }

                     pos = next_meta.find( '+' );
                     if( pos != string::npos )
                     {
                        cinfo.transaction_reward = from_string< uint64_t >( next_meta.substr( pos + 1 ) );
                        next_meta.erase( pos );
                     }

                     pos = next_meta.find( '-' );
                     if( pos != string::npos )
                     {
                        cinfo.mint_charge = from_string< uint64_t >( next_meta.substr( pos + 1 ) );
                        next_meta.erase( pos );

                        if( !cinfo.transaction_reward )
                           cinfo.transaction_reward = cinfo.mint_charge;
                     }

                     has_reward = true;
                     cinfo.mint_reward = from_string< uint64_t >( next_meta );
                  }
               }

               if( !has_reward )
                  throw runtime_error( "unexpected missing reward meta item in block header '" + header + "'" );

               has_previous_block = true;
            }
            else
            {
               string::size_type pos = next_attribute.find( c_file_type_core_block_header_previous_block_prefix );

               if( pos == string::npos )
                  throw runtime_error( "unxpected missing previous block item in block header '" + header + "'" );

               has_previous_block = true;

               if( cinfo.is_test )
                  previous_block = next_attribute.substr( pos
                   + string( c_file_type_core_block_header_previous_block_prefix ).length( ) );
               else
                  previous_block = base64_to_hex( next_attribute.substr( pos
                   + string( c_file_type_core_block_header_previous_block_prefix ).length( ) ) );
            }
         }
         else if( !has_public_key )
         {
            string::size_type pos = next_attribute.find( c_file_type_core_block_header_public_key_prefix );
            if( pos != 0 )
               throw runtime_error( "unexpected missing public_key attribute in block header '" + header + "'" );

            has_public_key = true;
            public_key_base64 = next_attribute.substr( pos
             + string( c_file_type_core_block_header_public_key_prefix ).length( ) );
         }
         else if( !has_total_weight )
         {
            string::size_type pos = next_attribute.find( c_file_type_core_block_header_total_weight_prefix );
            if( pos != 0 )
               throw runtime_error( "unexpected missing total weight attribute in block header '" + header + "'" );

            has_total_weight = true;
            total_weight = from_string< uint64_t >( next_attribute.substr( pos
             + string( c_file_type_core_block_header_total_weight_prefix ).length( ) ) );
         }
         else
            throw runtime_error( "unexpected extra attribute '" + next_attribute + "' in block header" );
      }

      if( block_height && !has_tag( "c" + chain + ".a" + account + "*" ) )
         throw runtime_error( "unknown account '" + account + "' for block header" );

      if( block_height == 0 && !get_is_known_blockchain( account ) )
         throw runtime_error( "invalid unrecognised blockchain '" + account + "'" );
   }

   if( p_extras && block_height )
   {
      block_info binfo;
      if( p_extras && get_block_info( binfo, previous_block ).first != block_height - 1 )
         throw runtime_error( "chain height is not one above previous block height" );

      string account_tag( list_file_tags( "c" + chain + ".a" + account + ".h*" ) );

      if( account_tag.empty( ) )
         throw runtime_error( "invalid account '" + account + "' for chain '" + chain + "'" );

      if( account_tag.find( ".banned" ) != string::npos )
         throw runtime_error( "account '" + account + "' has been banned for chain '" + chain + "'" );

      past_previous_block = binfo.previous_block;
      previous_block_height = binfo.block_height;
      previous_block_weight = binfo.block_weight;

      uint64_t expected = get_expected_weight(
       binfo.minter_hash, from_string< uint64_t >( account ), block_height );

      if( expected != block_weight )
         throw runtime_error( "incorrect weight specified in block (expecting "
          + to_string( expected ) + " but found " + to_string( block_weight ) + ")" );

      if( binfo.total_weight + block_weight != total_weight )
         throw runtime_error( "incorrect total weight specified in block (expecting "
          + to_string( binfo.total_weight + block_weight ) + " but found " + to_string( total_weight ) + ")" );
   }

   string verify( string( c_file_type_core_block_object ) + ':' + header );

   bool is_new_chain_head = false;
   bool is_better_chain_head = false;

   string mint_address;

   if( p_extras && block_height )
   {
      string block_tag( "c" + chain + ".b" + to_string( block_height ) );

      if( !has_tag( block_tag ) )
      {
         is_new_chain_head = true;
         previous_head = tag_file_hash( "c" + chain + ".b" + to_string( block_height - 1 ) );
      }
      else
      {
         string next_block_tag( "c" + chain + ".b" + to_string( block_height + 1 ) );

         if( !has_tag( next_block_tag ) )
         {
            previous_head = tag_file_hash( block_tag );

            block_info binfo;
            if( get_block_info( binfo, previous_head ).second > total_weight )
            {
               is_new_chain_head = true;
               is_better_chain_head = true;

               parallel_block_minted_minter_id = binfo.minter_id;
               parallel_block_minted_previous_block = binfo.previous_block;
               parallel_block_minted_num_transactions = binfo.transaction_hashes.size( );
               parallel_block_minted_had_secondary_account = binfo.had_secondary_account;

               parallel_block_minted_transaction_hashes.insert(
                binfo.transaction_hashes.begin( ), binfo.transaction_hashes.end( ) );
            }
         }   
      }
   }

   uint64_t num_accounts = 0;

   vector< string > transaction_hashes;

   bool had_signature = false;
   bool has_secondary_account = false;

   for( size_t i = 1; i < lines.size( ); i++ )
   {
      string next_line( lines[ i ] );

      if( next_line.size( ) < 3 )
         throw runtime_error( "unexpected line < 3 '" + next_line + "' in verify_block" );

      if( next_line.length( ) > c_max_core_line_size )
         throw runtime_error( "unexpected line length exceeds "
          + to_string( c_max_core_line_size ) + " in '" + next_line + "' in verify_block" );

      string prefix( next_line.substr( 0, 2 ) );
      next_line.erase( 0, 2 );

      if( prefix == string( c_file_type_core_block_detail_account_prefix ) )
      {
         verify += "\n" + lines[ i ];

         vector< string > attributes;
         split( next_line, attributes );

         if( attributes.size( ) < 3 )
            throw runtime_error( "invalid account format '" + next_line + "'" );

         bool has_id = false;
         bool has_exp = false;
         bool has_hash = false;
         bool has_lock = false;
         bool has_msg_key = false;
         bool has_tx_hash = false;
         bool has_tx_lock = false;

         string id, exp, hash, lock, msg_key, tx_hash, tx_lock;

         for( size_t j = 0; j < attributes.size( ); j++ )
         {
            string next_attribute( attributes[ j ] );

            if( next_attribute.size( ) < 2 )
               throw runtime_error( "invalid account attribute '" + next_attribute + "'" );

            if( !has_id )
            {
               regex expr( "^[0-9]{1,12}$" );

               if( expr.search( next_attribute ) != 0 )
                  throw runtime_error( "invalid account id '" + next_attribute + "'" );

               has_id = true;

               id = next_attribute;
            }
            else if( !has_exp )
            {
               size_t len = string( c_file_type_core_block_detail_account_exp_prefix ).length( );

               if( next_attribute.substr( 0, len ) != string( c_file_type_core_block_detail_account_exp_prefix ) )
                  throw runtime_error( "invalid account exp attribute '" + next_attribute + "'" );

               next_attribute.erase( 0, len );

               if( next_attribute.size( ) != 1 || next_attribute[ 0 ] < '1' || next_attribute[ 0 ] > '9' )
                  throw runtime_error( "invalid account exp value: " + next_attribute );

               has_exp = true;
               exp = next_attribute;
            }
            else if( !has_hash )
            {
               size_t len = string( c_file_type_core_block_detail_account_hash_prefix ).length( );

               if( next_attribute.substr( 0, len ) != string( c_file_type_core_block_detail_account_hash_prefix ) )
                  throw runtime_error( "invalid account hash attribute '" + next_attribute + "'" );

               next_attribute.erase( 0, len );

               base64::validate( next_attribute );

               has_hash = true;
               hash = next_attribute;
            }
            else if( !has_lock )
            {
               size_t len = string( c_file_type_core_block_detail_account_lock_prefix ).length( );

               if( next_attribute.length( ) < len + 1
                || next_attribute.substr( 0, len ) != string( c_file_type_core_block_detail_account_lock_prefix ) )
                  throw runtime_error( "invalid account lock attribute '" + next_attribute + "'" );

               regex expr( c_regex_bitcoin_address );

               next_attribute.erase( 0, len );

               if( expr.search( next_attribute ) != 0 )
                  throw runtime_error( "invalid account lock address '" + next_attribute + "'" );

               has_lock = true;
               lock = next_attribute;
            }
            else if( !has_msg_key )
            {
               size_t len = string( c_file_type_core_block_detail_account_msg_key_prefix ).length( );

               if( next_attribute.length( ) < len + 1
                || next_attribute.substr( 0, len ) != string( c_file_type_core_block_detail_account_msg_key_prefix ) )
                  throw runtime_error( "invalid account msg key attribute '" + next_attribute + "'" );

               next_attribute.erase( 0, len );

               base64::validate( next_attribute );

               has_msg_key = true;
               msg_key = next_attribute;
            }
            else if( !has_tx_hash )
            {
               size_t len = string( c_file_type_core_block_detail_account_tx_hash_prefix ).length( );

               if( next_attribute.substr( 0, len ) != string( c_file_type_core_block_detail_account_tx_hash_prefix ) )
                  throw runtime_error( "invalid account tx hash attribute '" + next_attribute + "'" );

               next_attribute.erase( 0, len );

               base64::validate( next_attribute );

               has_tx_hash = true;
               tx_hash = next_attribute;
            }
            else if( !has_tx_lock )
            {
               size_t len = string( c_file_type_core_block_detail_account_tx_lock_prefix ).length( );

               if( next_attribute.length( ) < len + 1
                || next_attribute.substr( 0, len ) != string( c_file_type_core_block_detail_account_tx_lock_prefix ) )
                  throw runtime_error( "invalid account tx lock attribute '" + next_attribute + "'" );

               regex expr( c_regex_bitcoin_address );

               next_attribute.erase( 0, len );

               if( expr.search( next_attribute ) != 0 )
                  throw runtime_error( "invalid account tx lock address '" + next_attribute + "'" );

               has_tx_lock = true;
               tx_lock = next_attribute;
            }
            else
               throw runtime_error( "unexpected account attribute '" + next_attribute + "'" );
         }

         if( !has_lock || ( has_tx_hash && !has_tx_lock ) )
            throw runtime_error( "unexpected incomplete account information '" + next_line + "'" );

         if( p_extras )
         {
            if( !block_height )
            {
               string extra( c_file_type_str_core_blob );
               extra += string( c_file_type_core_account_object ) + ':';

               extra += exp + ':' + msg_key + '\n' + hash + '\n' + lock;

               if( !tx_hash.empty( ) )
                  extra += '\n' + tx_hash + '\n' + tx_lock;

               ++num_accounts;

               string tags( "c" + chain + ".a" + id + ".h0.b" + to_string( cinfo.mint_reward - cinfo.mint_charge ) );
               p_extras->push_back( make_pair( extra, tags ) );
            }
            else if( !has_secondary_account )
            {
               has_secondary_account = true;

               string extra( c_file_type_str_core_blob );
               extra += string( c_file_type_core_account_object ) + ':';

               extra += exp + ':' + msg_key + '\n' + hash + '\n' + lock;

               if( !tx_hash.empty( ) )
                  extra += '\n' + tx_hash + '\n' + tx_lock;

               string tags( "c" + chain + ".a" + id + ".h" + to_string( block_height ) + ".b0" );
               p_extras->push_back( make_pair( extra, tags ) );
            }
            else
               throw runtime_error( "invalid attempt to create more than one secondary account" );
         }
      }
      else if( prefix == string( c_file_type_core_block_detail_transaction_prefix ) )
      {
         verify += "\n" + lines[ i ];

         string tx_hash( cinfo.is_test ? next_line : base64_to_hex( next_line ) );

         if( p_extras && !has_file( tx_hash ) )
            throw runtime_error( "transaction file '" + tx_hash + "' was not found" );

         transaction_hashes.push_back( tx_hash );
      }
      else if( !had_signature
       && prefix == string( c_file_type_core_block_detail_signature_prefix ) )
      {
         had_signature = true;
         block_signature = next_line;
#ifdef SSL_SUPPORT
         public_key pkey( public_key_base64, true );

         if( block_height )
            mint_address = pkey.get_address( true, true );

         if( check_sigs && !pkey.verify_signature( verify, block_signature ) )
            throw runtime_error( "invalid block signature" );
#endif
      }
      else
         throw runtime_error( "unexpected line '" + lines[ i ] + "' in verify_block" );
   }

   if( p_extras && !had_signature )
      throw runtime_error( "block signature missing" );

   if( !block_height )
      account_hash = "0" + public_key_base64;

   if( p_block_info )
   {
      p_block_info->minter_id = "c" + chain + ".a" + account;

      p_block_info->minter_hash = account_hash;
      p_block_info->minter_lock = account_lock;
      p_block_info->minter_pubkey = public_key_base64;

      p_block_info->previous_block = previous_block;

      p_block_info->block_height = block_height;
      p_block_info->block_weight = block_weight;
      p_block_info->total_weight = total_weight;

      p_block_info->transaction_hashes = transaction_hashes;

      p_block_info->had_secondary_account = has_secondary_account;
   }

   if( p_extras )
   {
      string raw_block_data( c_file_type_str_core_blob );
      raw_block_data += verify;

      string block_id( sha256( raw_block_data ).get_digest_as_string( ).substr( 0, c_id_length ) );

      string block_file_tag( "c" + chain + ".b" + to_string( block_height ) );

      string block_file_with_weight( block_file_tag + "-" + to_string( block_weight ) );
      string block_file_with_weight_and_sig_tag( block_file_with_weight + ".s" + base64_to_tag_name( block_signature ) );

      string tags;
      if( !cinfo.is_test )
         tags = block_file_with_weight_and_sig_tag;
      else
         tags = block_file_with_weight + ".a" + account;

      if( !block_height || is_new_chain_head )
         tags += "\n" + block_file_tag + "\nc" + chain + ".head";

      size_t block_extra_offset = p_extras->size( );
      p_extras->push_back( make_pair( raw_block_data, tags ) );

      map< uint64_t, string > new_chain_height_blocks;
      vector< vector< string > > all_transaction_hashes;

      uint64_t parallel_block_height = 0;
      all_transaction_hashes.push_back( transaction_hashes );

      set_session_variable( get_special_var_name( e_special_var_rewind_height ), to_string( c_unconfirmed_revision ) );

      if( !block_height )
      {
         cinfo.num_accounts = num_accounts;

         // NOTE: Add blockchain meta information which is used to store all relevant
         // meta-data for the blockchain (as the root block will end up being removed
         // after checkpoint pruning).
         string extra( chain_info_blob( cinfo, had_zero_explicit_account_charge ) );

         string chain_info_tag( "c" + chain );
         p_extras->push_back( make_pair( extra, chain_info_tag ) );

         TRACE_LOG( TRACE_CORE_FLS, "created new blockchain " + chain + " with block " + block_id );
      }
      else
      {
         if( block_height < cinfo.checkpoint_start_height )
            throw runtime_error( "invalid block height is below current checkpoint" );

         num_accounts = cinfo.num_accounts;

         // NOTE: If the account charge is explicitly provided with a zero value
         // then this will determine that the blockchain will not allow creation
         // of any secondary accounts.
         if( cinfo.account_charge == 0 && has_secondary_account )
            throw runtime_error( "secondary account creation not permitted" );

         map< string, uint64_t > account_balances;

         set< string > reused_transactions;
         set< string > retagged_transactions;

         map< string, string > old_transaction_hashes;
         set< string > new_transaction_hashes( transaction_hashes.begin( ), transaction_hashes.end( ) );

         if( !parallel_block_minted_minter_id.empty( )
          || ( is_new_chain_head && previous_head != previous_block ) )
         {
            bool need_to_rewind_storage = false;

            string new_previous_block, old_previous_block;

            if( !parallel_block_minted_minter_id.empty( ) )
            {
               parallel_block_height = block_height;

               string prior_block_minter_hash;
               uint64_t previous_balance = get_balance_from_minter_id( parallel_block_minted_minter_id, &prior_block_minter_hash );

               uint64_t total_reward = cinfo.mint_reward
                + ( cinfo.transaction_reward * parallel_block_minted_num_transactions );

               if( previous_balance < total_reward )
                  previous_balance = 0;
               else if( !parallel_block_minted_had_secondary_account )
                  previous_balance -= total_reward;

               account_balances.insert( make_pair( parallel_block_minted_minter_id, previous_balance ) );

               ++non_blob_extras;
               p_extras->push_back(
                make_pair( prior_block_minter_hash, parallel_block_minted_minter_id
                + ".h" + to_string( block_height ) + ".b*" + to_string( previous_balance ) ) );

               need_to_rewind_storage = ( parallel_block_minted_transaction_hashes.size( ) > 0 );

               // NOTE: Retag any txs from the previous best block that aren't included in the new block.
               for( set< string >::iterator i =
                parallel_block_minted_transaction_hashes.begin( );
                i != parallel_block_minted_transaction_hashes.end( ); ++i )
               {
                  if( new_transaction_hashes.count( *i ) )
                     reused_transactions.insert( *i );
                  else
                  {
                     retagged_transactions.insert( *i );

                     transaction_info tinfo;
                     get_transaction_info( tinfo, *i );

                     string tx_tag( parallel_block_minted_minter_id + ".t" + to_string( tinfo.sequence ) );

                     ++non_blob_extras;
                     p_extras->push_back( make_pair( *i, tx_tag ) );
                  }
               }

               new_previous_block = previous_block;
               old_previous_block = parallel_block_minted_previous_block;
            }
            else
            {
               old_previous_block = previous_head;
               new_previous_block = previous_block;
            }

            // NOTE: If previous blocks in the chain were not matching then need to adjust the
            // balance of the parallel minters back to the last block they both had in common.
            if( new_previous_block != old_previous_block )
            {
               parallel_block_height = block_height - 1;

               while( parallel_block_height
                && new_previous_block != old_previous_block )
               {
                  TRACE_LOG( TRACE_CORE_FLS, "chain " + chain
                   + " replacing " + old_previous_block.substr( 0, c_id_length ) + " with "
                   + new_previous_block.substr( 0, c_id_length ) + " at height " + to_string( parallel_block_height ) );

                  new_chain_height_blocks.insert( make_pair( parallel_block_height, new_previous_block ) );

                  ++non_blob_extras;
                  p_extras->push_back( make_pair( new_previous_block,
                   "c" + chain + ".b" + to_string( parallel_block_height ) ) );

                  block_info old_binfo;
                  get_block_info( old_binfo, old_previous_block );

                  uint64_t total_reward = cinfo.mint_reward
                   + ( cinfo.transaction_reward * old_binfo.transaction_hashes.size( ) );

                  if( !need_to_rewind_storage )
                     need_to_rewind_storage = ( old_binfo.transaction_hashes.size( ) > 0 );

                  // NOTE: Remember the transaction hashes (and minter) that were part of the previous best chain.
                  for( size_t i = 0; i < old_binfo.transaction_hashes.size( ); i++ )
                  {
                     if( !new_transaction_hashes.count( old_binfo.transaction_hashes[ i ] ) )
                        old_transaction_hashes.insert( make_pair( old_binfo.transaction_hashes[ i ], old_binfo.minter_id ) );
                  }

                  string old_minter_hash, old_minter_tag;

                  uint64_t previous_balance = get_balance_from_minter_id(
                   old_binfo.minter_id, &old_minter_hash, &old_minter_tag );

                  if( account_balances.count( old_binfo.minter_id ) )
                     previous_balance = account_balances[ old_binfo.minter_id ];

                  if( previous_balance < total_reward )
                     previous_balance = 0;
                  else if( !old_binfo.had_secondary_account )
                     previous_balance -= total_reward;

                  old_previous_block = old_binfo.previous_block;

                  account_balances[ old_binfo.minter_id ] = previous_balance;

                  string::size_type pos = old_minter_tag.find( ".b" );
                  if( pos == string::npos )
                     throw runtime_error( "unexpected old_minter_tag '" + old_minter_tag + "'" );

                  old_minter_tag.erase( pos );

                  ++non_blob_extras;
                  p_extras->push_back( make_pair( old_minter_hash,
                   old_minter_tag + ".b*" + to_string( previous_balance ) ) );

                  block_info new_binfo;
                  get_block_info( new_binfo, new_previous_block );

                  string new_minter_hash, new_minter_tag;
                  uint64_t new_account_balance = get_balance_from_minter_id(
                   new_binfo.minter_id, &new_minter_hash, &new_minter_tag );

                  all_transaction_hashes.push_back( new_binfo.transaction_hashes );

                  for( size_t i = 0; i < new_binfo.transaction_hashes.size( ); i++ )
                     new_transaction_hashes.insert( new_binfo.transaction_hashes[ i ] );

                  if( account_balances.count( new_binfo.minter_id ) )
                     new_account_balance = account_balances[ new_binfo.minter_id ];

                  if( !new_binfo.had_secondary_account )
                     new_account_balance += total_reward;

                  new_previous_block = new_binfo.previous_block;

                  account_balances[ new_binfo.minter_id ] = new_account_balance;

                  pos = new_minter_tag.find( ".b" );
                  if( pos == string::npos )
                     throw runtime_error( "unexpected new_minter_tag '" + new_minter_tag + "'" );

                  new_minter_tag.erase( pos );

                  ++non_blob_extras;
                  p_extras->push_back( make_pair( new_minter_hash,
                   new_minter_tag + ".b*" + to_string( new_account_balance ) ) );

                  if( new_previous_block != old_previous_block )
                     --parallel_block_height;
               }

               // NOTE: Any old best chain transactions that did not end up in the new best chain are now re-tagged.
               for( map< string, string >::iterator i = old_transaction_hashes.begin( ); i != old_transaction_hashes.end( ); ++i )
               {
                  if( !new_transaction_hashes.count( i->first ) )
                  {
                     retagged_transactions.insert( i->first );

                     transaction_info tinfo;
                     get_transaction_info( tinfo, i->first );

                     string tx_tag( "c" + chain + ".a" + i->second + ".t" + to_string( tinfo.sequence ) );

                     ++non_blob_extras;
                     p_extras->push_back( make_pair( i->first, tx_tag ) );
                  }
               }
            }

            if( need_to_rewind_storage )
            {
               set_session_variable(
                get_special_var_name( e_special_var_rewind_height ), to_string( parallel_block_height ) );
            }
         }

         string minter_account( "c" + chain + ".a" + account );

         account_info ainfo;
         get_account_info( ainfo, minter_account );

         string minter_account_tag( ainfo.tag );
         string minter_account_hash( tag_file_hash( minter_account_tag ) );

         bool had_existing = false;

         // NOTE: If this block conflicts with an existing one at the same height then assuming
         // the account hash and public key match then do not check those next as the following
         // code will reject the block after banning the account.
         if( block_height == ainfo.last_height )
         {
            block_info binfo;
            string existing_block_tag( list_file_tags(
             "c" + chain + ".b" + to_string( ainfo.last_height ) + "-*.a" + account ) );

            if( !existing_block_tag.empty( ) )
            {
               string existing_block_hash( tag_file_hash( existing_block_tag ) );

               get_block_info( binfo, existing_block_hash );
               if( account_hash == binfo.minter_hash && public_key_base64 == binfo.minter_pubkey )
                  had_existing = true;
            }
         }

         if( !had_existing && !check_if_valid_hash_pair( account_hash, ainfo.block_hash, true ) )
            throw runtime_error( "invalid hash from minter" );

         if( !had_existing && ainfo.block_lock != mint_address )
            throw runtime_error( "invalid public key from minter" );

         // NOTE: If an account has already minted then make sure that this block is higher than
         // the previous one minted. As any account that is producing such invalid height blocks
         // is likely trying to cause a fork, the account is set to be banned which will prevent
         // any further blocks being accepted from it. Assuming accounts are considered to be of
         // some value (with a "banned" account having none) it is not very likely for such fork
         // attempts to occur often, however, a mechanism to evaluate which side of the fork has
         // the lightest weight should be developed in order for a peer to be able to get itself
         // back onto the best chain rather than to be permanently stuck on a fork.
         if( block_height <= ainfo.last_height )
         {
            string conflict( list_file_tags(
             "c" + chain + ".b" + to_string( ainfo.last_height ) + "-*.a" + minter_account ) );

            tag_file( "c" + chain + ".a" + account
             + ".h" + to_string( ainfo.last_height ) + ".b*anned", minter_account_hash );

            throw runtime_error( "invalid block height for minting account" );
         }

         string::size_type pos = minter_account_tag.find( ".b" );

         if( pos == string::npos )
            throw runtime_error( "unable to find account balance in '" + minter_account_tag + "'" );

         if( account_balances.count( minter_account ) )
            ainfo.balance = account_balances[ minter_account ];

         if( ainfo.balance < cinfo.mint_charge )
            throw runtime_error( "insufficient balance to mint" );

         if( has_secondary_account && ainfo.balance < cinfo.account_charge )
            throw runtime_error( "insufficient balance to create an account" );

         if( has_secondary_account )
         {
            if( !cinfo.account_charge )
               ainfo.balance = 0;
            else
               ainfo.balance -= cinfo.account_charge;
         }
         else
            ainfo.balance -= cinfo.mint_charge;

         pos = minter_account_tag.find( ".h" );
         if( pos != string::npos )
            minter_account_tag.erase( pos );

         if( is_new_chain_head )
         {
            TRACE_LOG( TRACE_CORE_FLS, string( is_better_chain_head ? "better chain " : "new chain " )
             + chain + " head block " + block_id + " at height " + to_string( block_height ) );

            if( !has_secondary_account )
               ainfo.balance += cinfo.mint_reward + ( transaction_hashes.size( ) * cinfo.transaction_reward );
         }

         // NOTE: If is the new chain head then remove the transaction file tag for all transactions included
         // (after verifying that the transaction hasn't already been included and that it has no unconfirmed
         // previous transaction).
         if( is_new_chain_head && !transaction_hashes.empty( ) )
         {
            set< string > included_transactions;

            for( size_t i = 0; i < transaction_hashes.size( ); i++ )
            {
               transaction_info tinfo;

               if( included_transactions.count( transaction_hashes[ i ] )
                || ( get_transaction_info( tinfo, transaction_hashes[ i ] )
                && !reused_transactions.count( transaction_hashes[ i ] ) ) )
                  throw runtime_error( "transaction " + transaction_hashes[ i ] + " has already been included" );

               uint64_t sequence = tinfo.sequence;

               string previous_transaction = tinfo.previous_transaction;

               if( previous_transaction.empty( ) && sequence != 1 )
                  throw runtime_error( "initial transaction " + transaction_hashes[ i ] + " has incorrect sequence" );

               if( !cinfo.is_test
                && !previous_transaction.empty( ) && has_file( previous_transaction ) )
               {
                  bool included = get_transaction_info( tinfo, previous_transaction );

                  if( !included
                   && ( included_transactions.count( previous_transaction )
                   || retagged_transactions.count( previous_transaction ) ) )
                     included = true;

                  if( !included || sequence != tinfo.sequence + 1 )
                     throw runtime_error( "transaction " + transaction_hashes[ i ] + " is not in sequence" );
               }

               included_transactions.insert( transaction_hashes[ i ] );

               if( retagged_transactions.count( transaction_hashes[ i ] ) )
                  retagged_transactions.erase( transaction_hashes[ i ] );
               else if( !reused_transactions.count( transaction_hashes[ i ] ) )
               {
                  ++non_blob_extras;
                  p_extras->push_back( make_pair(
                   transaction_hashes[ i ], get_hash_tags( transaction_hashes[ i ] ) + "*" ) );
               }    
            }
         }

         string tags, extra;

         extra = string( c_file_type_str_core_blob );
         extra += get_account_data( ainfo, account_hash, account_lock, "", "", "" );

         // NOTE: The line feed is required to ensure that the tag is applied as a secondary one.
         tags += "\n" + minter_account_tag + ".h*" + to_string( block_height ) + ".b" + to_string( ainfo.balance );

         account_extra_offset = p_extras->size( );
         p_extras->push_back( make_pair( extra, tags ) );

         // NOTE: The previous account blob instance will be removed.
         ++non_blob_extras;
         p_extras->push_back( make_pair( "", minter_account_hash ) );
      }

      if( cinfo.is_test && is_new_chain_head )
      {
         uint64_t start = cinfo.checkpoint_start_height + 1;

         string full_chain_tag( "c" + chain + ".chain*" );
         for( uint64_t i = start; i < block_height; i++ )
         {
            string next_tag( list_file_tags( "c" + chain + ".b" + to_string( i ) ) );

            if( next_tag.empty( ) )
               continue;

            string next_hash( tag_file_hash( next_tag ) );

            if( new_chain_height_blocks.count( i ) )
               next_hash = new_chain_height_blocks[ i ];

            string all_tags( get_hash_tags( next_hash ) );

            vector< string > tags;
            split( all_tags, tags, '\n' );

            for( size_t j = 0; j < tags.size( ); j++ )
            {
               if( tags[ j ].find( ".c" ) != string::npos )
                  continue;

               if( tags[ j ].length( ) > next_tag.length( ) )
               {
                  string::size_type pos = tags[ j ].find( ".b" );
                  if( pos != string::npos )
                  {
                     string::size_type npos = tags[ j ].find( ".a" );

                     if( npos == string::npos )
                        full_chain_tag += tags[ j ].substr( pos );
                     else
                        full_chain_tag += tags[ j ].substr( pos, npos - pos );

                     break;
                  }
               }
            }
         }

         full_chain_tag += ".b" + to_string( block_height ) + "-" + to_string( block_weight );

         ( *p_extras )[ block_extra_offset ].second += "\n" + full_chain_tag;
      }

      // NOTE: The parallel block height (if the chain has not just been extended)
      // as well as all relevant transaction hashes (which include all those found
      // in the current block as well as those from all previous blocks going back
      // to the parallel block height) are appended to a <blockchain>.txs file.
      if( is_new_chain_head && !all_transaction_hashes.empty( ) )
      {
         string filename( chain + ".txs" );

         ofstream outf( filename.c_str( ), ios::out | ios::app );
         if( !outf )
            throw runtime_error( "unable to open '" + filename + "' for append" );

         for( size_t i = all_transaction_hashes.size( ) - 1; ; )
         {
            if( all_transaction_hashes[ i ].size( ) )
               outf << ( block_height - i ) << '\n';

            for( size_t j = 0; j < all_transaction_hashes[ i ].size( ); j++ )
               outf << all_transaction_hashes[ i ][ j ] << '\n';

            if( i == 0 )
               break;
            else
               --i;
         }

         outf.flush( );
         if( !outf.good( ) )
            throw runtime_error( "*** unexpected error occurred appending tx hash for blockchain ***" );
      }

      uint64_t checkpoint_height = 0;

      // NOTE: Determine whether or not a new checkpoint has occurred.
      if( cinfo.checkpoint_length
       && block_height > cinfo.checkpoint_start_height + ( cinfo.checkpoint_length * 2 ) )
      {
         size_t num_found = 0;

         string checkpoint_hash;
         uint64_t checkpoint_weight = 0;

         set< string > block_links;
         set< string > all_block_links;

         deque< string > checkpoint_blocks;

         if( cinfo.is_test )
         {
            string temp_previous_block( previous_block );
            string temp_past_previous_block( past_previous_block );
            for( size_t i = 0; ; i++ )
            {
               if( temp_previous_block.empty( ) || !has_file( temp_previous_block ) )
                  break;

               all_block_links.insert( temp_previous_block );

               if( temp_past_previous_block.empty( ) || !has_file( temp_past_previous_block ) )
                  break;

               block_info info;
               get_block_info( info, temp_past_previous_block );

               temp_previous_block = temp_past_previous_block;
               temp_past_previous_block = info.previous_block;
            }
         }

         // NOTE: If the checkpoint length is say three then blocks to be
         // checked start from three blocks prior (back to six blocks) to
         // make it less likely that nodes would checkpoint when a re-org
         // was about to appear. Therefore larger checkpoint lengths will
         // create less forks than shorter ones. Another important factor
         // is the checkpoint tolerance which is used to decide if or not
         // the weight is acceptable for checkpointing (the greater value
         // used then the more likely forks are to occur).
         for( size_t i = 0; i < cinfo.checkpoint_length * 2; i++ )
         {
            if( previous_block.empty( ) || !has_file( previous_block ) )
               break;

            if( i >= cinfo.checkpoint_length
             && previous_block_weight >= cinfo.checkpoint_tolerance )
               break;

            if( i >= cinfo.checkpoint_length )
            {
               if( !num_found++ )
               {
                  checkpoint_hash = previous_block;
                  checkpoint_height = previous_block_height;
               }

               checkpoint_weight += previous_block_weight;
               checkpoint_blocks.push_front( previous_block );
            }
            else
               block_links.insert( previous_block );

            if( past_previous_block.empty( ) || !has_file( past_previous_block ) )
               break;

            block_info info;
            get_block_info( info, past_previous_block );

            previous_block = past_previous_block;
            past_previous_block = info.previous_block;
            previous_block_height = info.block_height;
            previous_block_weight = info.block_weight;
         }

         if( num_found >= cinfo.checkpoint_length )
         {
            TRACE_LOG( TRACE_CORE_FLS, "new checkpoint for " + chain + " at height " + to_string( checkpoint_height ) );

            map< string, unsigned int > account_new_others;
            map< string, unsigned int > account_new_transactions;
            map< string, pair< unsigned int, unsigned int > > account_new_mint_info;

            uint64_t last_height = block_height - 1;
            while( true )
            {
               if( last_height == checkpoint_height )
                  break;

               string all_block_tags( list_file_tags( "c" + chain + ".b" + to_string( last_height ) + "-*" ) );

               if( all_block_tags.empty( ) )
                  break;

               vector< string > block_tags;
               split( all_block_tags, block_tags, '\n' );

               for( size_t i = 0; i < block_tags.size( ); i++ )
               {
                  string last_block_tag( block_tags[ i ] );
                  string last_block_hash( tag_file_hash( last_block_tag ) );

                  block_info info;
                  get_block_info( info, last_block_hash );

                  if( info.had_secondary_account )
                     ++account_new_others[ info.minter_id ];
                  else
                  {
                     if( block_links.count( last_block_hash ) )
                        ++account_new_mint_info[ info.minter_id ].first;
                     else
                        ++account_new_mint_info[ info.minter_id ].second;

                     if( block_links.count( last_block_hash ) )
                        account_new_transactions[ info.minter_id ] += info.transaction_hashes.size( );
                  }
               }

               --last_height;
            }

            string all_account_tags( list_file_tags( "c" + chain + ".a*.h*" ) );

            vector< string > account_tags;
            split( all_account_tags, account_tags, '\n' );

            num_accounts = has_secondary_account ? 1 : 0;

            // NOTE: All accounts have their balance set to the default (apart from
            // the current minter and those minters of blocks that occurred between
            // the current blockchain head and the new checkpoint).
            for( size_t i = 0; i < account_tags.size( ); i++ )
            {
               string next_account_tag( account_tags[ i ] );

               string::size_type pos = next_account_tag.find( ".b" );
               string::size_type hpos = next_account_tag.find( ".h" );

               if( pos == string::npos || hpos == string::npos )
                  continue;

               uint64_t account_height
                = from_string< uint64_t >( next_account_tag.substr( hpos + 2, pos - hpos - 2 ) );

               bool is_banned = ( next_account_tag.find( "banned" ) != string::npos );

               if( !is_banned )
                  ++num_accounts;

               uint64_t balance = 0;

               string next_account( next_account_tag.substr( 0, hpos ) );

               // NOTE: If the account is that of the minter then the tag that already
               // had been added as an extra to the minter's new account blob needs to
               // be instead modified.
               if( next_account_tag.substr( 1, hpos - 1 ) == chain + ".a" + account )
               {
                  string account_tags( ( *p_extras )[ account_extra_offset ].second );
                  string::size_type pos = account_tags.rfind( '\n' );

                  if( pos == string::npos )
                     throw runtime_error( "unexpected extra account_tags '" + account_tags + "' for minting account" );

                  account_tags.erase( pos + 1 );

                  int num_new_accts = account_new_others[ next_account ];
                  int num_consensus = account_new_mint_info[ next_account ].first;
                  int num_non_consensus = account_new_mint_info[ next_account ].second;

                  balance = ( cinfo.mint_reward - cinfo.mint_charge ) * 2;
                  balance += transaction_hashes.size( ) * cinfo.transaction_reward;

                  balance += ( cinfo.mint_reward - cinfo.mint_charge ) * num_consensus;
                  balance += account_new_transactions[ next_account ] * cinfo.transaction_reward;

                  if( cinfo.mint_charge * num_non_consensus > balance )
                     balance = 0;
                  else
                     balance -= ( cinfo.mint_charge * num_non_consensus );

                  if( !cinfo.account_charge && num_new_accts )
                     balance = 0;
                  else if( num_new_accts * cinfo.account_charge > balance )
                     balance = 0;
                  else
                     balance -= ( num_new_accts * cinfo.account_charge );

                  account_tags += "c" + chain + ".a" + account
                   + ".h*" + to_string( block_height ) + ".b" + to_string( balance );

                  ( *p_extras )[ account_extra_offset ].second = account_tags;
               }
               else if( !is_banned )
               {
                  string account_file_hash( tag_file_hash( next_account_tag ) );

                  int num_new_accts = account_new_others[ next_account ];
                  int num_consensus = account_new_mint_info[ next_account ].first;
                  int num_non_consensus = account_new_mint_info[ next_account ].second;

                  balance = ( cinfo.mint_reward - cinfo.mint_charge );

                  balance += ( cinfo.mint_reward - cinfo.mint_charge ) * num_consensus;
                  balance += account_new_transactions[ next_account ] * cinfo.transaction_reward;

                  if( cinfo.mint_charge * num_non_consensus > balance )
                     balance = 0;
                  else
                     balance -= ( cinfo.mint_charge * num_non_consensus );

                  if( !cinfo.account_charge && num_new_accts )
                     balance = 0;
                  else if( num_new_accts * cinfo.account_charge > balance )
                     balance = 0;
                  else
                     balance -= ( num_new_accts * cinfo.account_charge );

                  ++non_blob_extras;
                  p_extras->push_back( make_pair( account_file_hash,
                   next_account + ".h*" + to_string( account_height ) + ".b" + to_string( balance ) ) );
               }
            }

            // NOTE: Add checkpoint files.
            string checkpoint_block_data( c_file_type_str_core_blob );
            checkpoint_block_data += string( c_file_type_core_checkpoint_blocks_object ) + ':';

            checkpoint_block_data += to_string( checkpoint_weight );

            for( size_t i = 0; i < checkpoint_blocks.size( ); i++ )
            {
               checkpoint_block_data += '\n';

               if( cinfo.is_test )
                  checkpoint_block_data += checkpoint_blocks[ i ];
               else
                  checkpoint_block_data += hex_to_base64( checkpoint_blocks[ i ] );
            }

            set< string > checkpoint_tx_hashes;

            string checkpoint_transaction_data( c_file_type_str_core_blob );
            checkpoint_transaction_data += string( c_file_type_core_checkpoint_transactions_object ) + ':';

            checkpoint_transaction_data += to_string( checkpoint_weight );

            for( size_t i = 0; i < checkpoint_blocks.size( ); i++ )
            {
               block_info info;
               get_block_info( info, checkpoint_blocks[ i ] );

               for( size_t j = 0; j < info.transaction_hashes.size( ); j++ )
               {
                  checkpoint_transaction_data += '\n';

                  if( cinfo.is_test )
                     checkpoint_transaction_data += info.transaction_hashes[ j ];
                  else
                     checkpoint_transaction_data += hex_to_base64( info.transaction_hashes[ j ] );

                  checkpoint_tx_hashes.insert( info.transaction_hashes[ j ] );
               }
            }

            string checkpoint_data( c_file_type_str_core_blob );
            checkpoint_data += string( c_file_type_core_checkpoint_object ) + ':';

            checkpoint_data += to_string( checkpoint_weight ) + '\n';

            if( cinfo.is_test )
            {
               checkpoint_data += sha256( checkpoint_block_data ).get_digest_as_string( ) + '\n';
               checkpoint_data += sha256( checkpoint_transaction_data ).get_digest_as_string( );
            }
            else
            {
               checkpoint_data += hex_to_base64( sha256( checkpoint_block_data ).get_digest_as_string( ) ) + '\n';
               checkpoint_data += hex_to_base64( sha256( checkpoint_transaction_data ).get_digest_as_string( ) );
            }

            p_extras->push_back( make_pair( checkpoint_data,
             "c" + chain + ".checkpoint.h" + to_string( checkpoint_height ) ) );

            p_extras->push_back( make_pair( checkpoint_block_data, "" ) );
            p_extras->push_back( make_pair( checkpoint_transaction_data, "" ) );

            string checkpoint_info_data( c_file_type_str_core_blob );
            checkpoint_info_data += string( c_file_type_core_checkpoint_info_object ) + ':';

            checkpoint_info_data += string( c_file_type_core_checkpoint_info_header_chain_prefix ) + chain;

            checkpoint_info_data += ','
             + string( c_file_type_core_checkpoint_info_header_height_prefix ) + to_string( checkpoint_height );

            checkpoint_info_data += ','
             + string( c_file_type_core_checkpoint_info_header_checkpoint_prefix );

            if( cinfo.is_test )
               checkpoint_info_data += sha256( checkpoint_data ).get_digest_as_string( );
            else
               checkpoint_info_data += hex_to_base64( sha256( checkpoint_data ).get_digest_as_string( ) );

            transactions_info txs_info;
            map< string, size_t > tx_hash_offsets;

            get_transactions_and_hash_offsets( chain, txs_info, tx_hash_offsets );

            for( uint64_t i = cinfo.checkpoint_start_height + 1; i <= checkpoint_height; i++ )
            {
               checkpoint_info_data += '\n' + to_string( i );

               string all_block_tags( list_file_tags( "c" + chain + ".b" + to_string( i ) + "-*" ) );

               vector< string > block_tags;
               split( all_block_tags, block_tags, '\n' );

               for( size_t j = 0; j < block_tags.size( ); j++ )
               {
                  string next_tag( block_tags[ j ] );
                  string next_hash( tag_file_hash( next_tag ) );

                  string::size_type pos = next_tag.find( ".s" );

                  block_info binfo;
                  get_block_info( binfo, next_hash );

                  // NOTE: Put all transaction hashes with signatures for each block
                  // before appending the block hash and signature.
                  for( size_t k = 0; k < binfo.transaction_hashes.size( ); k++ )
                  {
                     string next_tx_hash( binfo.transaction_hashes[ k ] );

                     if( !tx_hash_offsets.count( next_tx_hash ) )
                        throw runtime_error( "unable to find tx hash " + next_tx_hash + " in verify_block" );

                     checkpoint_info_data += '\n';

                     string transaction_hash_with_sig(
                      txs_info.transaction_hashes_with_sigs[ tx_hash_offsets[ next_tx_hash ] ] );

                     if( cinfo.is_test )
                        checkpoint_info_data += transaction_hash_with_sig;
                     else
                     {
                        string::size_type pos = transaction_hash_with_sig.find( ':' );
                        if( pos == string::npos )
                           throw runtime_error( "invalid transaction hash with sig format '" + transaction_hash_with_sig + "'" );

                        checkpoint_info_data += hex_to_base64(
                         transaction_hash_with_sig.substr( 0, pos ) ) + transaction_hash_with_sig.substr( pos );
                     }
                  }

                  string next_line( cinfo.is_test ? next_hash : hex_to_base64( next_hash ) );

                  if( pos != string::npos )
                     next_line += ':' + tag_name_to_base64( next_tag.substr( pos + 2 ) );

                  checkpoint_info_data += '\n' + next_line;
               }
            }

            p_extras->push_back( make_pair( checkpoint_info_data,
             "c" + chain + ".checkpoint.h" + to_string( checkpoint_height ) + ".info" ) );

            // NOTE: Rework the transaction info blob to remove all txs that were included in
            // the checkpoint's "best chain".
            string txinfo_tag( "c" + chain + ".txinfo" );

            if( has_tag( txinfo_tag ) )
            {
               string txinfo_hash( tag_file_hash( txinfo_tag ) );

               // NOTE: The previous txinfo blob instance will be removed.
               p_extras->push_back( make_pair( "", txinfo_hash ) );

               string txinfo_data( c_file_type_str_core_blob );

               txinfo_data += string( c_file_type_core_transactions_info_object ) + ":";
               txinfo_data += string( c_file_type_core_transactions_info_header_chain_prefix ) + chain;

               bool still_has_any_txs = false;
               for( map< string, size_t >::iterator i = tx_hash_offsets.begin( ); i != tx_hash_offsets.end( ); ++i )
               {
                  if( !checkpoint_tx_hashes.count( i->first ) )
                  {
                     still_has_any_txs = true;

                     string transaction_hash_with_sig( txs_info.transaction_hashes_with_sigs[ i->second ] );

                     string::size_type pos = transaction_hash_with_sig.find( ':' );

                     if( !cinfo.is_test && pos == string::npos )
                        throw runtime_error( "invalid transaction hash with sig format '"
                         + transaction_hash_with_sig + " in verify_block" );

                     if( cinfo.is_test )
                        txinfo_data += '\n' + transaction_hash_with_sig;
                     else
                        txinfo_data += '\n' + hex_to_base64( transaction_hash_with_sig.substr( 0, pos ) )
                         + transaction_hash_with_sig.substr( pos );
                  }
               }

               if( still_has_any_txs )
                  p_extras->push_back( make_pair( txinfo_data, txinfo_tag ) );
            }
         }
         else
            checkpoint_height = 0;
      }

      // NOTE: Update blockchain meta information with the number of active accounts and checkpoint height.
      if( !chain_info_hash.empty( )
       && ( num_accounts != cinfo.num_accounts || checkpoint_height > cinfo.checkpoint_start_height ) )
      {
         if( num_accounts != cinfo.num_accounts )
            cinfo.num_accounts = num_accounts;

         if( checkpoint_height > cinfo.checkpoint_start_height )
            cinfo.checkpoint_start_height = checkpoint_height;

         string extra( chain_info_blob( cinfo, had_zero_explicit_account_charge ) );

         string chain_info_tag( "c" + chain );
         p_extras->push_back( make_pair( extra, chain_info_tag ) );

         // NOTE: Remove the previous blockchain meta information blob.
         ++non_blob_extras;
         p_extras->push_back( make_pair( "", chain_info_hash ) );
      }
   }

   return make_pair( block_height, total_weight );
}

void verify_rewind( const string& content, vector< pair< string, string > >* p_extras )
{
   guard g( g_mutex );

   if( !p_extras )
      throw runtime_error( "unexpected null extras in verify_rewind" );

   string::size_type pos = content.find( '.' );
   if( pos == 0 || pos == string::npos )
      throw runtime_error( "invalid rewind content '" + content + "'" );

   if( p_extras )
   {
      uint64_t new_block_height = from_string< uint64_t >( content.substr( pos + 2 ) );

      string chain( content.substr( 0, pos ) );
      string chain_id( chain.substr( 1 ) );

      string chain_tag( chain + ".head" );
      string chain_hash( tag_file_hash( chain_tag ) );

      string current_block_hash( chain_hash );

      chain_info cinfo;
      get_chain_info( cinfo, chain_id );

      if( new_block_height <= cinfo.checkpoint_start_height )
         throw runtime_error( "invalid attempt to rewind through checkpoint at height " + to_string( cinfo.checkpoint_start_height ) );

      block_info binfo;
      get_block_info( binfo, chain_hash );

      string destination_block_hash( tag_file_hash( content ) );

      uint64_t block_reward = cinfo.mint_reward - cinfo.mint_charge;

      map< string, string > account_block_locks;
      map< string, string > account_block_hashes;

      map< string, uint64_t > account_balances;
      map< string, uint64_t > account_block_heights;

      TRACE_LOG( TRACE_CORE_FLS, "rewinding chain " + chain_id + " to " + to_string( new_block_height ) );

      set< string > chain_blocks;

      while( true )
      {
         chain_blocks.insert( current_block_hash );

         if( binfo.previous_block.empty( ) || binfo.previous_block == destination_block_hash )
            break;

         current_block_hash = binfo.previous_block;

         block_info new_binfo;
         if( get_block_info( new_binfo, binfo.previous_block ).first != binfo.block_height - 1 )
            throw runtime_error( "chain height is not one below current chain height in verify_rewind" );

         binfo = new_binfo;
      }

      string all_account_tags( list_file_tags( "c" + chain_id + ".a*.h*" ) );

      vector< string > account_tags;
      split( all_account_tags, account_tags, '\n' );

      for( size_t i = 0; i < account_tags.size( ); i++ )
      {
         string next_account;
         string next_account_tag( account_tags[ i ] );

         string::size_type pos = next_account_tag.find( ".a" );
         if( pos == string::npos )
            throw runtime_error( "unexpected account tag '" + next_account_tag + "' missing .a" );

         next_account = next_account_tag.substr( pos + 2 );

         pos = next_account.find( ".h" );

         if( pos == string::npos )
            throw runtime_error( "unexpected account tag '" + next_account_tag + "' missing .h" );

         string next_account_height( next_account.substr( pos + 2 ) );

         next_account.erase( pos );
         pos = next_account_height.find( ".b" );

         if( pos == string::npos )
            throw runtime_error( "unexpected account tag '" + next_account_tag + "' missing .b" );

         uint64_t balance = from_string< uint64_t >( next_account_height.substr( pos + 2 ) );

         next_account_height.erase( pos );

         uint64_t height = from_string< uint64_t >( next_account_height );

         if( height > new_block_height )
         {
            account_balances[ next_account ] = balance;
            account_block_heights[ next_account ] = height;

            block_info next_account_binfo;
            string next_account_block_hash(
             get_block_hash_for_account_and_height( chain_id, next_account, height, &next_account_binfo ) );

            if( next_account_block_hash.empty( ) )
               throw runtime_error( "unexpected failure to find block at height "
                + to_string( height ) + " for account " + next_account + " in blockchain " + chain_id );

            account_block_locks[ next_account ] = next_account_binfo.minter_lock;
            account_block_hashes[ next_account ] = next_account_binfo.minter_hash;

            while( next_account_binfo.block_height >= new_block_height )
            {
               if( chain_blocks.count( next_account_block_hash ) )
               {
                  uint64_t total_reward = block_reward + ( cinfo.transaction_reward * next_account_binfo.transaction_hashes.size( ) );

                  if( next_account_binfo.had_secondary_account )
                  {
                     if( !cinfo.account_charge )
                        balance = block_reward;
                     else if( balance > cinfo.account_charge )
                        balance -= cinfo.account_charge;
                     else
                        balance = 0;
                  }
                  else if( balance > total_reward )
                     balance -= total_reward;
                  else
                     balance = 0;
               }
               else if( next_account_binfo.block_height > new_block_height )
                  balance += cinfo.mint_charge;

               height = next_account_binfo.block_height;

               account_balances[ next_account ] = balance;
               account_block_locks[ next_account ] = next_account_binfo.minter_lock;
               account_block_hashes[ next_account ] = next_account_binfo.minter_hash;
               account_block_heights[ next_account ] = new_block_height;

               // NOTE: Re-tag any txs that were part of the main chain.
               if( chain_blocks.count( next_account_block_hash )
                && next_account_binfo.block_height > new_block_height )
               {
                  for( size_t j = 0; j < next_account_binfo.transaction_hashes.size( ); j++ )
                  {
                     string next_tx_hash( next_account_binfo.transaction_hashes[ j ] );

                     transaction_info tinfo;
                     get_transaction_info( tinfo, next_tx_hash );

                     string tx_tag( tinfo.account_id + ".t" + to_string( tinfo.sequence ) );

                     p_extras->push_back( make_pair( next_tx_hash, tx_tag ) );
                  }
               }

               // NOTE: If the block is above the new height then remove it.
               if( next_account_binfo.block_height > new_block_height )
                  p_extras->push_back( make_pair( "", next_account_block_hash ) );

               bool okay = true;
               while( true )
               {
                  if( --height <= cinfo.checkpoint_start_height )
                  {
                     okay = false;
                     break;
                  }

                  next_account_block_hash = get_block_hash_for_account_and_height( chain_id, next_account, height, &next_account_binfo );

                  if( next_account_block_hash.empty( ) )
                     continue;

                  break;
               }

               if( !okay )
                  break;
            }
         }
      }

      for( map< string, uint64_t >::iterator i = account_balances.begin( ); i != account_balances.end( ); ++i )
      {
         string account_tag( "c" + chain_id + ".a" + i->first );

         account_info ainfo;
         string account_hash = get_account_info( ainfo, account_tag );

         if( ainfo.is_banned )
            continue;

         if( ainfo.block_lock != account_block_locks[ i->first ] )
         {
            ainfo.block_lock = account_block_locks[ i->first ];
            ainfo.block_hash = account_block_hashes[ i->first ];

            string new_account_blob( c_file_type_str_core_blob );
            new_account_blob += get_account_data( ainfo, "", "", "", "", "" );

            account_tag += ".h*" + to_string( account_block_heights[ i->first ] ) + ".b" + to_string( i->second );
            p_extras->push_back( make_pair( new_account_blob, account_tag ) );

            // NOTE: Remove the previous account blob.
            p_extras->push_back( make_pair( "", account_hash ) );
         }
         else
         {
            account_tag += ".h*" + to_string( account_block_heights[ i->first ] ) + ".b" + to_string( i->second );
            p_extras->push_back( make_pair( account_hash, account_tag ) );
         }
      }

      // NOTE: If a "debug" chain tag exists then truncate it.
      string debug_chain_tag( list_file_tags( "c" + chain_id + ".chain.*" ) );
      if( !debug_chain_tag.empty( ) )
      {
         string debug_chain_hash( tag_file_hash( debug_chain_tag ) );

         string::size_type pos = debug_chain_tag.find( ".b" + to_string( new_block_height + 1 ) + "-" );
         if( pos != string::npos )
         {
            debug_chain_tag.erase( pos );
            p_extras->push_back( make_pair( debug_chain_hash, debug_chain_tag + "**" ) );
         }
      }

      p_extras->push_back( make_pair( destination_block_hash, chain + ".head" ) );
   }
}

void verify_transaction( const string& content, bool check_sigs,
 vector< pair< string, string > >* p_extras, transaction_info* p_transaction_info )
{
   guard g( g_mutex );

   vector< string > lines;
   split( content, lines, '\n' );

   if( lines.empty( ) )
      throw runtime_error( "unexpected empty transaction content" );

   string chain, account, application, transaction_hash, transaction_lock, previous_transaction, public_key_base64;

   vector< string > log_lines;

   chain_info cinfo;
   string transaction_signature;

   uint64_t sequence = 0;

   string header( lines[ 0 ] );
   if( header.empty( ) )
      throw runtime_error( "unexpected empty transaction header" );
   else
   {
      vector< string > attributes;
      split( header, attributes );

      if( attributes.empty( ) )
         throw runtime_error( "unexpected empty transaction header attributes" );

      bool has_account = false;
      bool has_sequence = false;
      bool has_public_key = false;
      bool has_application = false;
      bool has_transaction_hash = false;
      bool has_transaction_lock = false;
      bool has_previous_transaction = false;

      for( size_t i = 0; i < attributes.size( ); i++ )
      {
         string next_attribute( attributes[ i ] );
         if( next_attribute.empty( ) )
            throw runtime_error( "unexpected empty attribute in transaction header '" + header + "'" );

         if( !has_account )
         {
            if( next_attribute.find( c_file_type_core_transaction_header_account_prefix ) != 0 )
               throw runtime_error( "unexpected missing account attribute in transaction header '" + header + "'" );

            has_account = true;

            account = next_attribute.substr(
             string( c_file_type_core_transaction_header_account_prefix ).length( ) );

            string::size_type pos = account.find( '.' );

            if( pos == string::npos )
               throw runtime_error( "account '" + account + "' is missing chain prefix" );

            chain = account.substr( 0, pos );
            account.erase( 0, pos + 1 );

            get_chain_info( cinfo, chain );
         }
         else if( !has_sequence )
         {
            if( next_attribute.find( c_file_type_core_transaction_header_sequence_prefix ) != 0 )
               throw runtime_error( "unexpected missing sequence attribute in transaction header '" + header + "'" );

            has_sequence = true;

            string value( next_attribute.substr(
             string( c_file_type_core_transaction_header_sequence_prefix ).length( ) ) );

            regex expr( c_regex_integer );

            if( ( !value.empty( ) && value[ 0 ] == '0' ) || expr.search( value ) != 0 )
               throw runtime_error( "invalid sequence value '" + value + "'" );

            sequence = from_string< uint64_t >( value );
         }
         else if( !has_application )
         {
            if( next_attribute.find( c_file_type_core_transaction_header_application_prefix ) != 0 )
               throw runtime_error( "unexpected missing application attribute in transaction header '" + header + "'" );

            has_application = true;

            application = next_attribute.substr(
             string( c_file_type_core_transaction_header_application_prefix ).length( ) );
         }
         else if( !has_public_key )
         {
            if( next_attribute.find( c_file_type_core_transaction_header_public_key_prefix ) != 0 )
               throw runtime_error( "unexpected missing public key attribute in transaction header '" + header + "'" );

            has_public_key = true;

            public_key_base64 = next_attribute.substr(
             string( c_file_type_core_transaction_header_public_key_prefix ).length( ) );
         }
         else if( !has_previous_transaction )
         {
            if( next_attribute.find( c_file_type_core_transaction_header_previous_tchain_prefix ) != 0 )
               throw runtime_error( "unexpected missing previous transaction attribute in transaction header '" + header + "'" );

            has_previous_transaction = true;

            string value( next_attribute.substr(
             string( c_file_type_core_transaction_header_previous_tchain_prefix ).length( ) ) );

            // NOTE: The very first transaction will have "0" instead of a valid previous transaction hash.
            if( value != "0" )
               previous_transaction = cinfo.is_test ? value : base64_to_hex( value );
         }
         else if( !has_transaction_hash )
         {
            if( next_attribute.find( c_file_type_core_transaction_header_transaction_hash_prefix ) != 0 )
               throw runtime_error( "unexpected missing transaction hash attribute in transaction header '" + header + "'" );

            has_transaction_hash = true;

            transaction_hash = next_attribute.substr(
             string( c_file_type_core_transaction_header_transaction_hash_prefix ).length( ) );
         }
         else if( !has_transaction_lock )
         {
            if( next_attribute.find( c_file_type_core_transaction_header_transaction_lock_prefix ) != 0 )
               throw runtime_error( "unexpected missing transaction lock attribute in transaction header '" + header + "'" );

            has_transaction_lock = true;

            transaction_lock = next_attribute.substr(
             string( c_file_type_core_transaction_header_transaction_lock_prefix ).length( ) );
         }
         else
            throw runtime_error( "unexpected extra attribute '" + next_attribute + "' in transaction header" );
      }

      if( !has_public_key )
         throw runtime_error( "invalid incompleted transaction header '" + header + "'" );
   }

   uint64_t expected_sequence = 0;

   account_info ainfo;
   string account_hash;

   if( p_extras )
   {
      string tx_account( "c" + chain + ".a" + account );

      account_hash = get_account_info( ainfo, tx_account );

      if( !ainfo.balance )
         throw runtime_error( "zero balance tx not permitted for account '" + account + "' in chain '" + chain + "'" );

      expected_sequence = ainfo.num_transactions + 1;
   }

   string verify( string( c_file_type_core_transaction_object ) + ':' + header );

   string transaction_address;

   int num_log_lines = 0;
   bool had_signature = false;

   for( size_t i = 1; i < lines.size( ); i++ )
   {
      string next_line( lines[ i ] );

      if( next_line.size( ) < 3 )
         throw runtime_error( "unexpected line length < 3 '" + next_line + "' in verify_transaction" );

      if( next_line.length( ) > c_max_core_line_size )
         throw runtime_error( "unexpected line length exceeds "
          + to_string( c_max_core_line_size ) + " in '" + next_line + "' in verify_transaction" );

      string prefix( next_line.substr( 0, 2 ) );
      next_line.erase( 0, 2 );

      if( prefix == string( c_file_type_core_transaction_detail_log_prefix ) )
      {
         ++num_log_lines;
         verify += "\n" + lines[ i ];

         log_lines.push_back( lines[ i ].substr( 2 ) );

         string::size_type pos = next_line.find( ' ' );
         if( pos == string::npos )
            throw runtime_error( "invalid transaction log line '" + next_line + "'" );

         string cmd( next_line.substr( 0, pos ) );

         if( cmd != "pc" && cmd != "pu" && cmd != "pd" && cmd != "pe" )
            throw runtime_error( "invalid cmd '" + cmd + "' in log line '" + next_line + "'" );

         if( cmd == "pc" || cmd == "pu" )
         {
            pos = next_line.find( '"' );
            if( pos == string::npos )
               throw runtime_error( "invalid transaction log line '" + next_line + "'" );
         }
      }
      else if( !had_signature
       && prefix == string( c_file_type_core_transaction_detail_signature_prefix ) )
      {
         had_signature = true;
         transaction_signature = next_line;

#ifdef SSL_SUPPORT
         public_key pkey( public_key_base64, true );

         transaction_address = pkey.get_address( true, true );

         if( check_sigs && !pkey.verify_signature( verify, transaction_signature ) )
            throw runtime_error( "invalid transaction signature" );
#endif
      }
      else
         throw runtime_error( "unexpected line '" + lines[ i ] + "' in verify_block" );
   }

   if( p_extras && !had_signature )
      throw runtime_error( "transaction signature missing" );

   string error_message;

   if( !cinfo.is_test && !num_log_lines )
      error_message = "invalid missing tx log lines";
   else if( p_extras && sequence != expected_sequence )
      error_message = "invalid transaction sequence " + to_string( sequence );
   else if( sequence != 1 && previous_transaction.empty( ) )
      error_message = "incorrect initial sequence number for transaction";

   if( !p_extras && !error_message.empty( ) )
      throw runtime_error( error_message );

   string raw_transaction_data( c_file_type_str_core_blob );
   raw_transaction_data += verify;

   string transaction_id = sha256( raw_transaction_data ).get_digest_as_string( );

   if( p_transaction_info )
   {
      p_transaction_info->account_id = "c" + chain + ".a" + account;
      p_transaction_info->sequence = sequence;
      p_transaction_info->log_lines = log_lines;
      p_transaction_info->application = application;
      p_transaction_info->previous_transaction = previous_transaction;
      p_transaction_info->is_included_in_best_chain = get_hash_tags( transaction_id ).empty( );
   }

   if( p_extras )
   {
      if( !cinfo.is_test && !previous_transaction.empty( ) && error_message.empty( ) )
      {
         transaction_info tinfo;
         string next_transaction_id( previous_transaction );

         bool is_in_best_chain = get_transaction_info( tinfo, next_transaction_id );
         if( sequence != tinfo.sequence + 1 )
            error_message = "sequence number does not follow that of the previous transaction";

         uint16_t transactions_not_in_best_chain = 0;

         while( !is_in_best_chain && error_message.empty( ) )
         {
            if( ++transactions_not_in_best_chain
             > max( c_tx_min_non_confirmed, ( uint16_t )( cinfo.checkpoint_length * 2 ) ) )
               throw runtime_error( "already has maximum non-confirmed transactions for account: " + account );

            next_transaction_id = tinfo.previous_transaction;
            if( next_transaction_id.empty( ) || !has_file( next_transaction_id ) )
               break;

            is_in_best_chain = get_transaction_info( tinfo, next_transaction_id );
         }
      }

      string transaction_file_tag( "c" + chain + ".a" + account + ".t" + to_string( sequence ) );

      if( !list_file_tags( transaction_file_tag ).empty( ) )
         throw runtime_error( "transaction file tag '" + transaction_file_tag + "' already exists" );

      if( !cinfo.is_test )
         transaction_file_tag += ".s" + base64_to_tag_name( transaction_signature );

      p_extras->push_back( make_pair( raw_transaction_data, transaction_file_tag ) );

      if( !check_if_valid_hash_pair( transaction_hash, ainfo.transaction_hash, true ) )
         error_message = "invalid hash in transaction";

      if( ainfo.transaction_lock != transaction_address )
         error_message = "invalid public key in transaction";

      if( ainfo.num_transactions && !has_file( previous_transaction ) )
         error_message = "previous transaction '" + previous_transaction + "' does not exist";

      // NOTE: If an invalid (but correctly signed) tx has been provided by an account
      // then ban that account (as most likely this would only occur due to an account
      // trying to cause a fork).
      if( !error_message.empty( ) )
      {
         tag_file( "c" + chain + ".a" + account
          + ".h" + to_string( ainfo.last_height ) + ".b*anned", account_hash );

         throw runtime_error( error_message );
      }

      ++ainfo.num_transactions;

      string extra( c_file_type_str_core_blob );

      extra += get_account_data( ainfo, "", "",
       transaction_hash, transaction_lock, hex_to_base64( transaction_id ) );

      p_extras->push_back( make_pair( extra, ainfo.tag ) );

      // NOTE: The previous account blob instance will be removed.
      p_extras->push_back( make_pair( "", tag_file_hash( ainfo.tag ) ) );

      string txinfo_tag( "c" + chain + ".txinfo" );
      string txinfo_data( c_file_type_str_core_blob );

      if( has_tag( txinfo_tag ) )
      {
         string txinfo_hash( tag_file_hash( txinfo_tag ) );
         txinfo_data += extract_file( txinfo_hash, "", c_file_type_char_core_blob );

         // NOTE: The previous txinfo blob instance will be removed.
         p_extras->push_back( make_pair( "", txinfo_hash ) );
      }
      else
      {
         txinfo_data += string( c_file_type_core_transactions_info_object ) + ":";
         txinfo_data += string( c_file_type_core_transactions_info_header_chain_prefix ) + chain;
      }

      if( cinfo.is_test )
         txinfo_data += "\n" + transaction_id;
      else
         txinfo_data += "\n" + hex_to_base64( transaction_id );

      if( !cinfo.is_test )
         txinfo_data += ':' + transaction_signature;

      p_extras->push_back( make_pair( txinfo_data, txinfo_tag ) );
   }
}

void verify_blockchain_info( const string& content,
 vector< pair< string, string > >* p_extras, blockchain_info* p_blockchain_info )
{
   guard g( g_mutex );

   bool construct_info = false;
   if( content.find( ".info" ) != string::npos )
      construct_info = true;

   if( !construct_info )
   {
      chain_info cinfo;

      vector< string > lines;
      split( content, lines, '\n' );

      string chain_id;

      bool had_block_height = false;
      bool is_prior_to_checkpoint = false;

      uint64_t height = 0;
      uint64_t block_height = 0;

      vector< string > checkpoint_info;
      vector< string > blob_hashes_with_sigs;

      for( size_t i = 0; i < lines.size( ); i++ )
      {
         string next_line( lines[ i ] );

         if( i == 0 )
         {
            string all_headers( next_line );
            vector< string > headers;

            split( all_headers, headers );

            bool has_chain = false;
            bool has_height = false;

            for( size_t j = 0; j < headers.size( ); j++ )
            {
               string next_header( headers[ j ] );
               string::size_type pos = next_header.find( '=' );

               if( pos == string::npos )
                  throw runtime_error( "invalid header '" + next_header + "' in verify_blockchain_info" );

               string prefix( next_header.substr( 0, pos + 1 ) );
               string remainder( next_header.substr( pos + 1 ) );

               if( !has_chain )
               {
                  if( prefix != string( c_file_type_core_blockchain_info_header_chain_prefix ) )
                     throw runtime_error( "unexpected missing chain prefix header in verify_blockchain_info" );

                  has_chain = true;
                  chain_id = remainder;

                  get_chain_info( cinfo, chain_id );
               }
               else if( !has_height )
               {
                  if( prefix != string( c_file_type_core_blockchain_info_header_height_prefix ) )
                     throw runtime_error( "unexpected missing height prefix header in verify_blockchain_info" );

                  has_height = true;
                  height = from_string< uint64_t >( remainder );

                  if( cinfo.checkpoint_start_height && height <= cinfo.checkpoint_start_height )
                     is_prior_to_checkpoint = true;
               }
               else
                  throw runtime_error( "unexpected extra header '" + next_header + "' in verify_blockchain_info" );
            }

            if( !has_height )
               throw runtime_error( "one or more missing headers in verify_blockchain_info" );
         }
         else if( next_line.length( ) < 10 )
         {
            uint64_t next_height = from_string< uint64_t >( next_line );

            if( block_height && next_height == 0 )
               throw runtime_error( "invalid block height 0 in verify_blockchain_info" );

            if( block_height && next_height != block_height + 1 )
               throw runtime_error( "unexpected block height " + to_string( next_height )
                + " after previous block height " + to_string( block_height ) + " in verify_blockchain_info" );

            had_block_height = true;
            block_height = next_height;
         }
         else if( !block_height && !had_block_height )
         {
            string::size_type pos = next_line.find( '.' );
            if( pos == string::npos )
               throw runtime_error( "unexpected checkpoint info '" + next_line + "' in verify_blockchain_info" );

            if( cinfo.is_test )
               checkpoint_info.push_back( next_line );
            else
               checkpoint_info.push_back(
                base64_to_hex( next_line.substr( 0, pos ) )
                + '.' + base64_to_hex( next_line.substr( pos + 1 ) ) );
         }
         else
         {
            string::size_type pos = next_line.find( ':' );

            if( !cinfo.is_test && pos == string::npos )
               throw runtime_error( "unexpected missing signature from '" + next_line + "' in verify_blockchain_info" );

            string blob_hash_with_sig;

            if( pos == string::npos )
               blob_hash_with_sig = next_line;
            else
               blob_hash_with_sig = base64_to_hex( next_line.substr( 0, pos ) ) + next_line.substr( pos );

            blob_hashes_with_sigs.push_back( blob_hash_with_sig );
         }
      }

      if( p_blockchain_info )
      {
         p_blockchain_info->chain_id = chain_id;
         p_blockchain_info->checkpoint_info = checkpoint_info;

         if( !is_prior_to_checkpoint )
            p_blockchain_info->blob_hashes_with_sigs = blob_hashes_with_sigs;
      }
   }
   else
   {
      string::size_type pos = content.find( "." );

      string chain( content.substr( 0, pos ) );
      string chain_id( chain.substr( 1 ) );

      chain_info cinfo;
      get_chain_info( cinfo, chain_id );

      string chain_tag( chain + ".head" );
      string chain_hash( tag_file_hash( chain_tag ) );

      block_info binfo;
      get_block_info( binfo, chain_hash );

      uint64_t height = binfo.block_height;

      if( cinfo.checkpoint_start_height && height <= cinfo.checkpoint_start_height )
         throw runtime_error( "cannot get blockchain info at height "
          + to_string( height ) + " as checkpoint height is " + to_string( cinfo.checkpoint_start_height ) );

      string blockchain_info_data( c_file_type_str_core_blob );
      blockchain_info_data += string( c_file_type_core_blockchain_info_object ) + ':';

      blockchain_info_data += string( c_file_type_core_blockchain_info_header_chain_prefix ) + chain_id
       + "," + string( c_file_type_core_blockchain_info_header_height_prefix ) + to_string( height );

      map< uint64_t, string > ordered_checkpoint_hashes;
      get_ordered_checkpoint_hashes( chain_id, ordered_checkpoint_hashes );

      for( map< uint64_t, string >::iterator i
       = ordered_checkpoint_hashes.begin( ); i != ordered_checkpoint_hashes.end( ); ++i )
      {
         string info_hash( tag_file_hash( "c"
          + chain_id + ".checkpoint.h" + to_string( i->first ) + ".info" ) );

         if( cinfo.is_test )
            blockchain_info_data += '\n' + i->second + '.' + info_hash;
         else
            blockchain_info_data += '\n' + hex_to_base64( i->second ) + '.' + hex_to_base64( info_hash );
      }

      uint64_t next_height = cinfo.checkpoint_start_height + 1;

      if( !cinfo.checkpoint_start_height )
         --next_height;

      string txs_info_tag( "c" + chain_id + ".txinfo" );

      transactions_info txs_info;
      map< string, size_t > tx_hash_offsets;

      get_transactions_and_hash_offsets( chain_id, txs_info, tx_hash_offsets );

      while( true )
      {
         string all_block_tags = list_file_tags( "c" + chain_id + ".b" + to_string( next_height ) + "-*" );

         if( all_block_tags.empty( ) )
            throw runtime_error( "no blocks found at height " + to_string( next_height ) + " in verify_blockchain_info" );

         blockchain_info_data += '\n' + to_string( next_height );

         vector< string > block_tags;
         split( all_block_tags, block_tags, '\n' );

         for( size_t i = 0; i < block_tags.size( ); i++ )
         {
            string next_tag( block_tags[ i ] );
            string::size_type pos = next_tag.find( ".s" );

            if( !cinfo.is_test && pos == string::npos )
               throw runtime_error( "unexpected block tag format '" + next_tag + "' in verify_blockchain_info" );

            block_info binfo;
            get_block_info( binfo, tag_file_hash( next_tag ) );

            // NOTE: Put all transaction hashes with signatures for each block
            // before appending the block hash and signature.
            for( size_t j = 0; j < binfo.transaction_hashes.size( ); j++ )
            {
               string next_tx_hash( binfo.transaction_hashes[ j ] );

               if( !tx_hash_offsets.count( next_tx_hash ) )
                  throw runtime_error( "unable to find tx hash " + next_tx_hash + " in verify_blockchain_info" );

               if( cinfo.is_test )
                  blockchain_info_data += '\n'
                   + txs_info.transaction_hashes_with_sigs[ tx_hash_offsets[ next_tx_hash ] ];
               else
               {
                  string transaction_hash_with_sig(
                   txs_info.transaction_hashes_with_sigs[ tx_hash_offsets[ next_tx_hash ] ] );

                  string::size_type pos = transaction_hash_with_sig.find( ':' );

                  if( pos == string::npos )
                     throw runtime_error( "unexpected transaction hash with sig '"
                      + transaction_hash_with_sig + "' in verify_blockchain_info" );

                  blockchain_info_data += '\n'
                   + hex_to_base64( transaction_hash_with_sig.substr( 0, pos ) )
                   + transaction_hash_with_sig.substr( pos );
               }
            }

            if( cinfo.is_test )
               blockchain_info_data += '\n' + tag_file_hash( next_tag );
            else
               blockchain_info_data += '\n' + hex_to_base64( tag_file_hash( next_tag ) )
                + ':' + tag_name_to_base64( next_tag.substr( pos + 2 ) );
         }

         if( ++next_height > height )
            break;
      }

      multimap< uint64_t, string > sequenced_transactions;
      get_sequenced_transactions( chain_id, sequenced_transactions, cinfo.is_test, false, &txs_info );

      for( multimap< uint64_t, string >::iterator
       i = sequenced_transactions.begin( ); i!= sequenced_transactions.end( ); ++i )
         blockchain_info_data += "\n" + i->second;

      if( p_extras )
      {
         string existing_hash;
         string chain_info_tag( "c" + chain_id + ".info" );

         if( has_tag( chain_info_tag ) )
            existing_hash = tag_file_hash( chain_info_tag );

         if( existing_hash != sha256( blockchain_info_data ).get_digest_as_string( ) )
         {
            p_extras->push_back( make_pair( blockchain_info_data, "c" + chain_id + ".info" ) );

            // NOTE: If there was a previous blockchain info blob then it will be removed.
            if( !existing_hash.empty( ) )
               p_extras->push_back( make_pair( "", existing_hash ) );
         }
      }
   }
}

void verify_checkpoint_info( const string& content,
 vector< pair< string, string > >* p_extras, checkpoint_info* p_checkpoint_info )
{
   guard g( g_mutex );

   chain_info cinfo;

   vector< string > lines;
   split( content, lines, '\n' );

   string chain_id;
   string checkpoint_hash;

   uint64_t height = 0;

   vector< string > blob_hashes_with_sigs;

   for( size_t i = 0; i < lines.size( ); i++ )
   {
      string next_line( lines[ i ] );

      if( i == 0 )
      {
         string all_headers( next_line );
         vector< string > headers;

         split( all_headers, headers );

         bool has_chain = false;
         bool has_height = false;
         bool has_checkpoint = false;

         for( size_t j = 0; j < headers.size( ); j++ )
         {
            string next_header( headers[ j ] );
            string::size_type pos = next_header.find( '=' );

            if( pos == string::npos )
               throw runtime_error( "invalid header '" + next_header + "' in verify_checkpoint_info" );

            string prefix( next_header.substr( 0, pos + 1 ) );
            string remainder( next_header.substr( pos + 1 ) );

            if( !has_chain )
            {
               if( prefix != string( c_file_type_core_checkpoint_info_header_chain_prefix ) )
                  throw runtime_error( "unexpected missing chain prefix header in verify_checkpoint_info" );

               has_chain = true;
               chain_id = remainder;

               get_chain_info( cinfo, chain_id );
            }
            else if( !has_height )
            {
               if( prefix != string( c_file_type_core_checkpoint_info_header_height_prefix ) )
                  throw runtime_error( "unexpected missing height prefix header in verify_checkpoint_info" );

               has_height = true;
               height = from_string< uint64_t >( remainder );
            }
            else if( !has_checkpoint )
            {
               if( prefix != string( c_file_type_core_checkpoint_info_header_checkpoint_prefix ) )
                  throw runtime_error( "unexpected missing checkpoint prefix header in verify_checkpoint_info" );

               has_checkpoint = true;

               if( cinfo.is_test )
                  checkpoint_hash = remainder;
               else
                  checkpoint_hash = base64_to_hex( remainder );
            }
            else
               throw runtime_error( "unexpected extra header '" + next_header + "' in verify_checkpoint_info" );
         }

         if( !has_checkpoint )
            throw runtime_error( "one or more missing headers in verify_checkpoint_info" );
      }
      else
      {
         if( next_line.length( ) < 10 )
         {
            uint64_t next_height = from_string< uint64_t >( next_line );

            if( i > 1 && next_height != height + 1 )
               throw runtime_error( "unexpected height " + to_string( next_height ) + " in verify_checkpoint_info" );

            height = next_height;
         }
         else
         {
            if( cinfo.is_test )
               blob_hashes_with_sigs.push_back( next_line );
            else
            {
               string::size_type pos = next_line.find( ':' );

               if( pos == string::npos )
                  throw runtime_error( "unexpected blob hash with sig '" + next_line + " in verify_checkpoint_info" );

               blob_hashes_with_sigs.push_back(
                base64_to_hex( next_line.substr( 0, pos ) ) + next_line.substr( pos ) );
            }
         }
      }
   }

   if( p_checkpoint_info )
   {
      p_checkpoint_info->chain_id = chain_id;
      p_checkpoint_info->checkpoint_hash = checkpoint_hash;
      p_checkpoint_info->blob_hashes_with_sigs = blob_hashes_with_sigs;
   }
}

void verify_checkpoint_prune( const string& content, vector< pair< string, string > >* p_extras )
{
   guard g( g_mutex );

   if( content.find( ".checkpoint.h" ) == string::npos )
      throw runtime_error( "invalid checkpoint prune content '" + content + "'" );

   if( p_extras )
   {
      string::size_type pos = content.find( "." );

      string chain( content.substr( 0, pos ) );
      string chain_id( chain.substr( 1 ) );

      pos = content.find( ".h" );
      uint64_t checkpoint_height = from_string< uint64_t >( content.substr( pos + 2 ) );

      string checkpoint_hash( tag_file_hash( content ) );
      string checkpoint_info_hash( tag_file_hash( content + ".info" ) );

      map< uint64_t, string > ordered_checkpoint_hashes;
      get_ordered_checkpoint_hashes( chain_id, ordered_checkpoint_hashes );

      if( !ordered_checkpoint_hashes.empty( ) && checkpoint_hash != ordered_checkpoint_hashes.begin( )->second )
         throw runtime_error( "can only prune the checkpoint at height " + to_string( ordered_checkpoint_hashes.begin( )->first ) );

      string checkpoint_data( extract_file( checkpoint_hash, "", c_file_type_char_core_blob ) );

      vector< string > lines;
      split( checkpoint_data, lines, '\n' );

      if( lines.size( ) != 3 )
         throw runtime_error( "unexpected checkpoint content was not 3 lines" );

      string all_checkpoint_blocks( extract_file( lines[ 1 ], "", c_file_type_char_core_blob ) );
      string all_checkpoint_transactions( extract_file( lines[ 2 ], "", c_file_type_char_core_blob ) );

      vector< string > checkpoint_transactions;
      split( all_checkpoint_transactions, checkpoint_transactions, '\n' );

      for( size_t i = 1; i < checkpoint_transactions.size( ); i++ )
      {
         if( has_file( checkpoint_transactions[ i ] ) )
            p_extras->push_back( make_pair( "", checkpoint_transactions[ i ] ) );
      }

      uint64_t height = checkpoint_height;

      // NOTE: Remove all block blobs that are at or below the checkpoint height.
      while( true )
      {
         string all_block_tags( list_file_tags( "c" + chain_id + ".b" + to_string( height ) + "-*" ) );

         if( all_block_tags.empty( ) )
            break;

         vector< string > block_tags;
         split( all_block_tags, block_tags, '\n' );

         for( size_t i = 0; i < block_tags.size( ); i++ )
            p_extras->push_back( make_pair( "", tag_file_hash( block_tags[ i ] ) ) );

         if( height-- == 0 )
            break;
      }

      // NOTE: The checkpoint blobs will also be removed.
      p_extras->push_back( make_pair( "", lines[ 1 ] ) );
      p_extras->push_back( make_pair( "", lines[ 2 ] ) );
      p_extras->push_back( make_pair( "", checkpoint_hash ) );
      p_extras->push_back( make_pair( "", checkpoint_info_hash ) );
   }
}

void verify_transactions_info( const string& content,
 vector< pair< string, string > >* p_extras, transactions_info* p_transactions_info )
{
   guard g( g_mutex );

   chain_info cinfo;

   vector< string > lines;
   split( content, lines, '\n' );

   string chain_id;

   vector< string > transaction_hashes_with_sigs;

   for( size_t i = 0; i < lines.size( ); i++ )
   {
      string next_line( lines[ i ] );

      if( i == 0 )
      {
         string all_headers( next_line );
         vector< string > headers;

         split( all_headers, headers );

         bool has_chain = false;

         for( size_t j = 0; j < headers.size( ); j++ )
         {
            string next_header( headers[ j ] );
            string::size_type pos = next_header.find( '=' );

            if( pos == string::npos )
               throw runtime_error( "invalid header '" + next_header + "' in verify_transactions_info" );

            string prefix( next_header.substr( 0, pos + 1 ) );
            string remainder( next_header.substr( pos + 1 ) );

            if( !has_chain )
            {
               if( prefix != string( c_file_type_core_transactions_info_header_chain_prefix ) )
                  throw runtime_error( "unexpected missing chain prefix header in verify_transactions_info" );

               has_chain = true;
               chain_id = remainder;

               get_chain_info( cinfo, chain_id );
            }
            else
               throw runtime_error( "unexpected extra header '" + next_header + "' in verify_transactions_info" );
         }

         if( !has_chain )
            throw runtime_error( "one or more missing headers in verify_transactions_info" );
      }
      else
      {
         string::size_type pos = next_line.find( ':' );

         if( !cinfo.is_test && pos == string::npos )
            throw runtime_error( "unexpected missing signature from '" + next_line + "' in verify_transactions_info" );

         if( cinfo.is_test )
            transaction_hashes_with_sigs.push_back( next_line );
         else
            transaction_hashes_with_sigs.push_back(
             base64_to_hex( next_line.substr( 0, pos ) ) + next_line.substr( pos ) );
      }
   }

   if( p_transactions_info )
   {
      p_transactions_info->chain_id = chain_id;
      p_transactions_info->transaction_hashes_with_sigs = transaction_hashes_with_sigs;
   }
}

}

void verify_core_file( const string& content, bool check_sigs, vector< pair< string, string > >* p_extras )
{
   if( content.empty( ) )
      throw runtime_error( "invalid empty core file content" );
   else
   {
      unsigned char file_type = content[ 0 ];

      if( file_type == c_file_type_char_core_blob )
      {
         string::size_type pos = content.find( ':' );

         if( pos == string::npos )
            throw runtime_error( "invalid content '" + content + "' for core file" );

         string type( content.substr( 1, pos - 1 ) );

         if( type == string( c_file_type_core_block_object ) )
            verify_block( content.substr( pos + 1 ), check_sigs, p_extras );
         else if( type == string( c_file_type_core_rewind_object ) )
            verify_rewind( content.substr( pos + 1 ), p_extras );
         else if( type == string( c_file_type_core_transaction_object ) )
            verify_transaction( content.substr( pos + 1 ), check_sigs, p_extras );
         else if( type == string( c_file_type_core_blockchain_info_object ) )
            verify_blockchain_info( content.substr( pos + 1 ), p_extras );
         else if( type == string( c_file_type_core_checkpoint_prune_object ) )
            verify_checkpoint_prune( content.substr( pos + 1 ), p_extras );
         else if( type == string( c_file_type_core_transactions_info_object ) )
            verify_transactions_info( content.substr( pos + 1 ), p_extras );
         else
            throw runtime_error( "unknown type '" + type + "' for core file" );
      }
      else
         throw runtime_error( "unable to verify non-blob core files" );
   }
}

bool is_block( const string& core_type )
{
   return ( core_type == string( c_file_type_core_block_object ) );
}

bool is_checkpoint( const string& core_type )
{
   return ( core_type == string( c_file_type_core_checkpoint_object ) );
}

bool is_transaction( const string& core_type )
{
   return ( core_type == string( c_file_type_core_transaction_object ) );
}

bool is_blockchain_info( const string& core_type )
{
   return ( core_type == string( c_file_type_core_blockchain_info_object ) );
}

bool is_checkpoint_info( const string& core_type )
{
   return ( core_type == string( c_file_type_core_checkpoint_info_object ) );
}

bool is_checkpoint_blocks( const string& core_type )
{
   return ( core_type == string( c_file_type_core_checkpoint_blocks_object ) );
}

bool is_checkpoint_transactions( const string& core_type )
{
   return ( core_type == string( c_file_type_core_checkpoint_transactions_object ) );
}

void get_blockchain_info( const string& content, blockchain_info& bc_info )
{
   string::size_type pos = content.find( ':' );

   if( pos == string::npos
    || content.substr( 0, pos ) != string( c_file_type_core_blockchain_info_object ) )
      throw runtime_error( "invalid content provided to get_blockchain_info" );

   verify_blockchain_info( content.substr( pos + 1 ), 0, &bc_info );
}

void get_checkpoint_info( const string& blockchain, const string& content, checkpoint_info& cp_info )
{
   string::size_type pos = content.find( ':' );

   if( pos == string::npos
    || content.substr( 0, pos ) != string( c_file_type_core_checkpoint_info_object ) )
      throw runtime_error( "invalid content provided to get_checkpoint_info" );

   verify_checkpoint_info( content.substr( pos + 1 ), 0, &cp_info );
}

bool has_better_block( const string& blockchain, uint64_t height, uint64_t weight )
{
   guard g( g_mutex );

   bool retval = false;

   string all_tags( list_file_tags( "c" + blockchain + ".b" + to_string( height ) + "-*" ) );

   if( !all_tags.empty( ) )
   {
      vector< string > tags;
      split( all_tags, tags, '\n' );

      for( size_t i = 0; i < tags.size( ); i++ )
      {
         string next_tag( tags[ i ] );

         string::size_type pos = next_tag.find( ".s" );

         if( pos != string::npos )
            next_tag.erase( pos );

         pos = next_tag.find( "-" );
         if( pos != string::npos )
         {
            uint64_t next_weight = from_string< uint64_t >( next_tag.substr( pos + 1 ) );

            if( next_weight < weight )
            {
               retval = true;
               break;
            }
         }
      }
   }

   return retval;
}

string check_account( const string& blockchain, const string& password )
{
   string retval;

   string id( get_account_id_from_password( password ) );

   if( !list_file_tags( "c" + blockchain + ".a" + id + ".h*" ).empty( ) )
      retval = id;
   else
      throw runtime_error( "invalid account: " + id + " for blockchain: " + blockchain );

   return retval;
}

string construct_new_block( const string& blockchain,
 const string& password, const string& account, bool use_core_file_format, new_block_info* p_new_block_info )
{
   guard g( g_mutex );

   string acct( account );
   string accts_file;

   if( !acct.empty( ) && acct[ 0 ] == '@' )
   {
      accts_file = acct.substr( 1 );
      acct.erase( );
   }
   else if( acct.empty( ) )
   {
      string id( get_account_id_from_password( password ) );
      if( !list_file_tags( "c" + blockchain + ".a" + id + ".h*" ).empty( ) )
         acct = id;
   }

   string chain( blockchain );
   if( acct.empty( ) )
      chain.erase( );

   uint64_t balance = 0;
   account_key_info key_info;
   string account_id( construct_account_info( chain, password, c_default_exp, acct, &key_info, &balance ) );

   string head_hash;
   uint64_t height = 0;

   uint64_t weight = 0;
   uint64_t total_weight = 0;

   chain_info cinfo;

   if( !chain.empty( ) )
      get_chain_info( cinfo, chain );

   if( !chain.empty( ) )
   {
      head_hash = tag_file_hash(  "c" + chain + ".head" );

      block_info binfo;
      get_block_info( binfo, head_hash );

      height = binfo.block_height + 1;
      weight = get_expected_weight( binfo.minter_hash, from_string< uint64_t >( account_id ), height );

      total_weight = binfo.total_weight + weight;

      if( p_new_block_info )
      {
         chain_info cinfo;
         get_chain_info( cinfo, chain );

         p_new_block_info->height = height;
         p_new_block_info->weight = weight;

         p_new_block_info->total_weight = total_weight;
         p_new_block_info->previous_block_weight = binfo.block_weight;

         p_new_block_info->can_mint = ( balance > cinfo.mint_charge );

         if( !cinfo.checkpoint_tolerance || weight < cinfo.checkpoint_tolerance )
            p_new_block_info->range = new_block_info::e_target_range_optimal;
         else if( weight < cinfo.checkpoint_tolerance * 2 )
            p_new_block_info->range = new_block_info::e_target_range_sub_optimal;
         else
            p_new_block_info->range = new_block_info::e_target_range_less_optimal;
      }
   }

   string data( string( c_file_type_core_block_object )
    + ":" + string( c_file_type_core_block_header_account_prefix ) + ( chain.empty( ) ? account_id : chain ) );

   if( !acct.empty( ) )
      data += "." + acct;

   data += "," + string( c_file_type_core_block_header_height_prefix ) + to_string( height );

   if( acct.empty( ) )
      data += "," + string( c_file_type_core_block_header_chain_meta_prefix )
       + string( c_file_type_core_block_header_chain_meta_requisite_prefix ) + blockchain;
   else
      data += "," + string( c_file_type_core_block_header_weight_prefix ) + to_string( weight )
      + "," + string( c_file_type_core_block_header_account_hash_prefix ) + key_info.block_hash
      + "," + string( c_file_type_core_block_header_account_lock_prefix ) + key_info.block_lock;

   if( !head_hash.empty( ) )
      data += "," + string( c_file_type_core_block_header_previous_block_prefix )
       + ( cinfo.is_test ? head_hash : hex_to_base64( head_hash ) );

#ifdef SSL_SUPPORT
   private_key priv_key( key_info.block_secret.empty( ) ? uuid( ).as_string( ) : key_info.block_secret );
   data += "," + string( c_file_type_core_block_header_public_key_prefix ) + priv_key.get_public( true, true );
#endif

   if( !chain.empty( ) )
      data += "," + string( c_file_type_core_block_header_total_weight_prefix ) + to_string( total_weight );
   else if( !accts_file.empty( ) )
   {
      vector< string > accounts;
      buffer_file_lines( accts_file, accounts );

      for( size_t i = 0; i < accounts.size( ); i++ )
         data += '\n' + accounts[ i ];
   }
   else
   {
      data += "\n" + string( c_file_type_core_block_detail_account_prefix ) + account_id
       + "," + string( c_file_type_core_block_detail_account_exp_prefix ) + to_string( c_default_exp )
       + "," + string( c_file_type_core_block_detail_account_hash_prefix ) + key_info.block_hash
       + "," + string( c_file_type_core_block_detail_account_lock_prefix ) + key_info.block_lock
       + "," + string( c_file_type_core_block_detail_account_msg_key_prefix ) + key_info.msg_pubkey
       + "," + string( c_file_type_core_block_detail_account_tx_hash_prefix ) + key_info.trans_hash
       + "," + string( c_file_type_core_block_detail_account_tx_lock_prefix ) + key_info.trans_lock;
   }

   multimap< uint64_t, string > sequenced_transactions;
   get_sequenced_transactions( blockchain, sequenced_transactions, cinfo.is_test, true );

   size_t num_txs = 0;

   for( multimap< uint64_t, string >::iterator
    i = sequenced_transactions.begin( ); i!= sequenced_transactions.end( ); ++i )
   {
      // NOTE: The "hard limit" for the number of txs per block is determined by the actual
      // file item size limit but a smaller "soft limit" is being used here.
      if( ++num_txs > c_tx_soft_limit )
         break;

      data += "\n" + string( c_file_type_core_block_detail_transaction_prefix ) + i->second;
   }

#ifdef SSL_SUPPORT
   data += "\n" + string( c_file_type_core_block_detail_signature_prefix ) + priv_key.construct_signature( data, true );
#endif

   if( use_core_file_format )
      data = string( c_file_type_str_core_blob ) + data;

   return data;
}

string construct_new_transaction( const string& blockchain,
 const string& password, const string& account, const string& application,
 const string& transaction_log_lines, bool use_core_file_format, string* p_tx_hash )
{
   string acct;

   string id( get_account_id_from_password( password ) );
   if( !list_file_tags( "c" + blockchain + ".a" + id + ".h*" ).empty( ) )
      acct = id;

   if( acct.empty( ) )
      throw runtime_error( "unknown account: " + id );

   if( !account.empty( ) && acct != account )
      throw runtime_error( "invalid password for account: " + account );

   string data( string( c_file_type_core_transaction_object )
    + ":" + string( c_file_type_core_transaction_header_account_prefix ) + blockchain + "." + acct );

   uint64_t balance = 0;
   uint64_t num_transactions = 0;

   string last_transaction_id( "0" );

   account_key_info key_info;
   string account_id( construct_account_info( blockchain,
    password, c_default_exp, acct, &key_info, &balance, &num_transactions, &last_transaction_id ) );

   if( balance == 0 )
      throw runtime_error( "cannot create a transaction for a zero balance account" );

   data += "," + string( c_file_type_core_transaction_header_sequence_prefix ) + to_string( num_transactions + 1 );
   data += "," + string( c_file_type_core_transaction_header_application_prefix ) + application;

#ifdef SSL_SUPPORT
   private_key priv_key( key_info.trans_secret );
   data += "," + string( c_file_type_core_transaction_header_public_key_prefix ) + priv_key.get_public( true, true );
#endif

   data += "," + string( c_file_type_core_transaction_header_previous_tchain_prefix ) + last_transaction_id;

   data += "," + string( c_file_type_core_transaction_header_transaction_hash_prefix ) + key_info.trans_hash;
   data += "," + string( c_file_type_core_transaction_header_transaction_lock_prefix ) + key_info.trans_lock;

   if( !transaction_log_lines.empty( ) )
   {
      vector< string > tx_log_lines;
      split( transaction_log_lines, tx_log_lines, '\n' );

      for( size_t i = 0; i < tx_log_lines.size( ); i++ )
         data += "\n" + string( c_file_type_core_transaction_detail_log_prefix ) + tx_log_lines[ i ];
   }

   if( p_tx_hash )
      *p_tx_hash = sha256( string( c_file_type_str_core_blob ) + data ).get_digest_as_string( );

#ifdef SSL_SUPPORT
   data += "\n" + string( c_file_type_core_transaction_detail_signature_prefix ) + priv_key.construct_signature( data, true );
#endif

   if( use_core_file_format )
      data = string( c_file_type_str_core_blob ) + data;

   return data;
}

string construct_blob_for_block_content( const string& block_content, const string& block_signature )
{
   return string( c_file_type_str_core_blob ) + block_content
    + "\n" + string( c_file_type_core_block_detail_signature_prefix ) + block_signature;
}

string construct_blob_for_transaction_content(
 const string& transaction_content, const string& transaction_signature )
{
   return string( c_file_type_str_core_blob ) + transaction_content
    + "\n" + string( c_file_type_core_transaction_detail_signature_prefix ) + transaction_signature;
}

string construct_account_info(
 const string& blockchain, const string& password,
 unsigned int exp, const string& account, account_key_info* p_key_info,
 uint64_t* p_balance, uint64_t* p_num_transactions, string* p_last_transaction_id )
{
   guard g( g_mutex );

   string account_id( account );

   string last_block_hash;
   string last_block_lock;

   string last_trans_hash;
   string last_trans_lock;

   account_key_info key_info;

   unsigned int exponent( exp == 0 ? c_default_exp : exp );

   if( !account_id.empty( ) )
   {
      account_info ainfo;
      get_account_info( ainfo, "c" + blockchain + ".a" + account_id );

      exponent = ainfo.exp;

      if( p_balance )
         *p_balance = ainfo.balance;

      if( p_num_transactions )
         *p_num_transactions = ainfo.num_transactions;

      if( p_last_transaction_id && !ainfo.last_transaction_id.empty( ) )
         *p_last_transaction_id = ainfo.last_transaction_id;

      last_block_hash = base64::decode( ainfo.block_hash );
      last_trans_hash = base64::decode( ainfo.transaction_hash );

      last_block_lock = ainfo.block_lock;
      last_trans_lock = ainfo.transaction_lock;
   }

   if( account_id.empty( ) )
      account_id = get_account_id_from_password( password );
   else if( account_id != get_account_id_from_password( password ) )
      throw runtime_error( "invalid password for account '" + account + "'" );

   sha256 message_hash( password + "." + password );
   key_info.msg_secret = get_hash_secret( message_hash );

#ifdef SSL_SUPPORT
   private_key message_privkey( key_info.msg_secret );
   key_info.msg_pubkey = message_privkey.get_public( true, true );
#endif

   sha256 block_hash( account_id + password );
   sha256 trans_hash( password + account_id );

   unsigned char buf[ c_hash_buf_size ];

   unsigned char bbuf[ c_hash_buf_size ];
   unsigned char tbuf[ c_hash_buf_size ];

   block_hash.copy_digest_to_buffer( bbuf );
   trans_hash.copy_digest_to_buffer( tbuf );

   size_t block_round = 0;
   size_t trans_round = 0;

   bool found_block = false;
   bool found_trans = false;

   size_t rounds = 10;
   for( size_t i = 1; i < exponent; i++ )
      rounds *= 10;

   for( size_t i = 0; i < rounds; i++ )
   {
      if( !found_block )
      {
         block_hash.update( bbuf, c_hash_buf_size );
         block_hash.copy_digest_to_buffer( buf );

         if( memcmp( buf, last_block_hash.data( ), c_hash_buf_size ) == 0 )
         {
            block_round = i;
            found_block = true;
            key_info.block_hash = base64::encode( bbuf, c_hash_buf_size );
         }

         memcpy( bbuf, buf, c_hash_buf_size );
      }

      if( !found_trans )
      {
         trans_hash.update( tbuf, c_hash_buf_size );
         trans_hash.copy_digest_to_buffer( buf );

         if( memcmp( buf, last_trans_hash.data( ), c_hash_buf_size ) == 0 )
         {
            trans_round = i;
            found_trans = true;
            key_info.trans_hash = base64::encode( tbuf, c_hash_buf_size );
         }

         memcpy( tbuf, buf, c_hash_buf_size );
      }

      if( found_block && found_trans )
         break;
   }

   if( found_block )
   {
      if( block_round >= rounds )
         throw runtime_error( "account block hashes have been exhausted" );

      sha256 hash( to_string( block_round + 1 ) + password );
      key_info.block_secret = get_hash_secret( hash );

#ifdef SSL_SUPPORT
      private_key pkey( key_info.block_secret );
      if( pkey.get_address( true, true ) != last_block_lock )
         throw runtime_error( "unexpected invalid private key for block lock" );
#endif
   }
   else if( !account.empty( ) )
      throw runtime_error( "invalid account " + account + " for blockchain " + blockchain );
   else
   {
      block_round = rounds;
      key_info.block_hash = base64::encode( bbuf, c_hash_buf_size );
   }

#ifdef SSL_SUPPORT
   sha256 block_lock_hash( to_string( block_round ) + password );
   private_key block_privkey( get_hash_secret( block_lock_hash ) );
   key_info.block_lock = block_privkey.get_address( true, true );
#endif

   if( found_trans )
   {
      if( trans_round >= rounds )
         throw runtime_error( "account trans hashes have been exhausted" );

      sha256 hash( password + to_string( trans_round + 1 ) );
      key_info.trans_secret = get_hash_secret( hash );

#ifdef SSL_SUPPORT
      private_key pkey( key_info.trans_secret );
      if( pkey.get_address( true, true ) != last_trans_lock )
         throw runtime_error( "unexpected invalid private key for trans lock" );
#endif
   }
   else
   {
      trans_round = rounds;
      key_info.trans_hash = base64::encode( tbuf, c_hash_buf_size );
   }

#ifdef SSL_SUPPORT
   sha256 trans_lock_hash( password + to_string( trans_round ) );
   private_key trans_privkey( get_hash_secret( trans_lock_hash ) );
   key_info.trans_lock = trans_privkey.get_address( true, true );
#endif

   if( p_key_info )
   {
      *p_key_info = key_info;
      return account_id;
   }
   else
   {
      return string( c_file_type_core_block_detail_account_prefix ) + account_id
       + "," + string( c_file_type_core_block_detail_account_exp_prefix ) + to_string( exponent )
       + "," + string( c_file_type_core_block_detail_account_hash_prefix ) + key_info.block_hash
       + "," + string( c_file_type_core_block_detail_account_lock_prefix ) + key_info.block_lock
       + "," + string( c_file_type_core_block_detail_account_msg_key_prefix ) + key_info.msg_pubkey
       + "," + string( c_file_type_core_block_detail_account_tx_hash_prefix ) + key_info.trans_hash
       + "," + string( c_file_type_core_block_detail_account_tx_lock_prefix ) + key_info.trans_lock;
   }    
}

string get_account_msg_secret( const string& blockchain, const string& password, const string& account )
{
   string acct;

   string id( get_account_id_from_password( password ) );
   if( !list_file_tags( "c" + blockchain + ".a" + id + ".h*" ).empty( ) )
      acct = id;

   if( acct.empty( ) )
      throw runtime_error( "unknown account: " + id );

   if( !account.empty( ) && acct != account )
      throw runtime_error( "invalid password for account: " + account );

   account_key_info key_info;
   string account_id( construct_account_info( blockchain, password, c_default_exp, acct, &key_info ) );

   return key_info.msg_secret;
}

void perform_storage_rewind( const string& blockchain, uint64_t block_height )
{
   guard g( g_mutex );

   if( block_height != c_unconfirmed_revision )
   {
      chain_info cinfo;
      get_chain_info( cinfo, blockchain );

      if( block_height <= cinfo.checkpoint_start_height )
         throw runtime_error( "invalid attempt to rewind through checkpoint at height " + to_string( cinfo.checkpoint_start_height ) );
   }
   
   storage_process_undo( block_height );
}

string construct_blockchain_info_file( const string& blockchain )
{
   guard g( g_mutex );

   string data( c_file_type_str_core_blob );

   data += string( c_file_type_core_blockchain_info_object ) + ":c" + blockchain + ".info";

   vector< pair< string, string > > extras;

   verify_core_file( data, true, &extras );

   // NOTE: There is no need to create a raw file for the info "request"
   // so only the extras are passed for raw file creation.
   create_raw_file_with_extras( "", extras );

   return tag_file_hash( "c" + blockchain + ".info" );
}

uint64_t construct_transaction_scripts_for_blockchain( const string& blockchain, vector< string >& applications )
{
   guard g( g_mutex );

   string filename( blockchain + ".txs" );

   uint64_t block_height = 0;

   if( file_exists( filename ) )
   {
      set< string > txs;
      vector< string > lines;
      buffer_file_lines( filename, lines );

      string block_height_marker( get_special_var_name( e_special_var_bh ) );

      uint64_t height = 0;
      bool has_new_height = false;
      map< string, vector< string > > app_log_lines;

      for( size_t i = 0; i < lines.size( ); i++ )
      {
         string next( lines[ i ] );

         if( next.length( ) < numeric_limits< uint64_t >::digits10 )
         {
            has_new_height = true;
            height = from_string< uint64_t >( next );
         }
         else
         {
            transaction_info tinfo;
            if( get_transaction_info( tinfo, next ) )
            {
               if( has_new_height )
               {
                  has_new_height = false;

                  app_log_lines[ tinfo.application ].push_back(
                   "storage_comment \"" + string( c_block_prefix ) + " " + to_string( height ) + "\"" );
               }

               txs.insert( next );

               string account( tinfo.account_id );
               string::size_type pos = account.find( ".a" );

               if( pos != string::npos )
                  account.erase( 0, pos + 2 );

               for( size_t j = 0; j < tinfo.log_lines.size( ); j++ )
               {
                  string next_log_line( tinfo.log_lines[ j ] );
                  string::size_type pos = insert_account_into_transaction_log_line( account, next_log_line );

                  if( next_log_line[ pos + 1 ] == '"' )
                     next_log_line.insert( pos + 1, block_height_marker + "=" + to_string( height ) );
                  else
                     next_log_line.insert( pos + 1, block_height_marker + "=" + to_string( height ) + "," );

                  app_log_lines[ tinfo.application ].push_back( next_log_line );
               }
            }
         }
      }

      for( map< string, vector< string > >::iterator i = app_log_lines.begin( ); i!= app_log_lines.end( ); ++i )
      {
         string filename( i->first + ".txs.cin" );
         string local_txs( i->first + ".txs.log" );

         vector< string > old_local_txs;
         vector< string > new_local_txs;

         if( file_exists( local_txs ) )
         {
            buffer_file_lines( local_txs, old_local_txs );

            for( size_t j = 0; j < old_local_txs.size( ); j++ )
            {
               if( !txs.count( old_local_txs[ j ] ) )
                  new_local_txs.push_back( old_local_txs[ j ] );
            }
         }

         ofstream outf( filename.c_str( ), ios::out | ios::app );
         if( !outf )
            throw runtime_error( "unable to open file '" + filename + "' for output" );

         for( size_t j = 0; j < ( i->second ).size( ); j++ )
            outf << '.' << ( i->second )[ j ] << '\n';

         // NOTE: If any previous local txs have not been processed then issue them again.
         if( !new_local_txs.empty( ) )
         {
            outf << ".session_variable " <<
             get_special_var_name( e_special_var_blockchain ) << " " << blockchain << '\n';

            for( size_t j = 0; j < new_local_txs.size( ); j++ )
            {
               string next( new_local_txs[ j ] );

               transaction_info tinfo;
               if( get_transaction_info( tinfo, next ) )
               {
                  string account( tinfo.account_id );

                  for( size_t k = 0; k < tinfo.log_lines.size( ); k++ )
                  {
                     string next_log_line( tinfo.log_lines[ k ] );
                     insert_account_into_transaction_log_line( account, next_log_line );

                     outf << '.' << next_log_line << '\n';
                  }
               }
            }
         }

         outf.flush( );
         if( !outf.good( ) )
            throw runtime_error( "*** unexpected error occurred appending tx script for blockchain ***" );

         applications.push_back( i->first );
      }

      file_remove( filename );
   }

   return block_height;
}

