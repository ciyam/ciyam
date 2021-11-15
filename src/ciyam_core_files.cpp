// Copyright (c) 2014-2021 CIYAM Developers
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
#include "utilities.h"
#include "class_base.h"
#include "ciyam_base.h"
#ifdef SSL_SUPPORT
#  include "crypto_keys.h"
#endif
#include "hash_chain.h"
#include "ciyam_files.h"
#include "crypt_stream.h"
#include "ciyam_variables.h"

#ifdef SSL_SUPPORT
#  include <openssl/rand.h>
#endif

using namespace std;

trace_mutex g_mutex;

namespace
{

#include "ciyam_constants.h"

const size_t c_id_length = 10;
const size_t c_hash_buf_size = 32;

const int c_tx_soft_limit = 100;

const unsigned int c_default_exp = 5;
const unsigned int c_max_core_line_size = 200000;

const uint16_t c_tx_max_unconfirmed = 5;

const uint64_t c_mask_adjust_factor_8 = UINT64_C( 0xffffffffffffff00 );
const uint64_t c_mask_adjust_factor_7 = UINT64_C( 0xffffffffffff0000 );
const uint64_t c_mask_adjust_factor_6 = UINT64_C( 0xffffffffff000000 );
const uint64_t c_mask_adjust_factor_5 = UINT64_C( 0xffffffff00000000 );
const uint64_t c_mask_adjust_factor_4 = UINT64_C( 0xffffff0000000000 );
const uint64_t c_mask_adjust_factor_3 = UINT64_C( 0xffff000000000000 );
const uint64_t c_mask_adjust_factor_2 = UINT64_C( 0xff00000000000000 );

struct block_info
{
   block_info( )
    :
    nonce_value( 0 ),
    block_height( 0 ),
    block_weight( 0 ),
    total_weight( 0 ),
    had_secondary_account( false )
   {
   }

   string minter_id;
   string minter_hash;
   string minter_lock;
   string minter_pubkey;

   string previous_block;

   uint32_t nonce_value;

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

struct data_info
{
   data_info( ) : unix_time_stamp( 0 ) { }

   string public_key_hash;
   uint64_t unix_time_stamp;
};

struct lamport_info
{
   lamport_info( ) : unix_time_stamp( 0 ) { }

   string data_file_hash;
   string public_key_hash;

   uint64_t unix_time_stamp;
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

uint64_t adjusted_hit( uint64_t hit, uint64_t target, int factor )
{
   uint64_t adjusted_hit = hit;

   switch( factor )
   {
      case 0:
      case 1:
      break;

      case 2:
      adjusted_hit &= ( hit & ~c_mask_adjust_factor_2 );
      adjusted_hit |= ( target & c_mask_adjust_factor_2 );
      break;

      case 3:
      adjusted_hit &= ( hit & ~c_mask_adjust_factor_3 );
      adjusted_hit |= ( target & c_mask_adjust_factor_3 );
      break;

      case 4:
      adjusted_hit &= ( hit & ~c_mask_adjust_factor_4 );
      adjusted_hit |= ( target & c_mask_adjust_factor_4 );
      break;

      case 5:
      adjusted_hit &= ( hit & ~c_mask_adjust_factor_5 );
      adjusted_hit |= ( target & c_mask_adjust_factor_5 );
      break;

      case 6:
      adjusted_hit &= ( hit & ~c_mask_adjust_factor_6 );
      adjusted_hit |= ( target & c_mask_adjust_factor_6 );
      break;

      case 7:
      adjusted_hit &= ( hit & ~c_mask_adjust_factor_7 );
      adjusted_hit |= ( target & c_mask_adjust_factor_7 );
      break;

      default:
      adjusted_hit &= ( hit & ~c_mask_adjust_factor_8 );
      adjusted_hit |= ( target & c_mask_adjust_factor_8 );
   }

   return adjusted_hit;
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

uint64_t get_prior_balance_for_minter( const chain_info& cinfo, const block_info& binfo, uint64_t current_balance )
{
   uint64_t prior_balance = current_balance;

   uint64_t total_reward = ( cinfo.transaction_reward * binfo.transaction_hashes.size( ) );

   if( !binfo.had_secondary_account )
      total_reward += cinfo.mint_reward;
   else
      prior_balance += cinfo.account_charge;

   if( prior_balance > total_reward )
      prior_balance -= total_reward;
   else
      prior_balance = 0;

   return prior_balance;
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

string::size_type insert_account_into_transaction_log_line( const string& account, string& next_log_line )
{
   string::size_type pos = next_log_line.find( ' ' );
   if( pos == string::npos )
      throw runtime_error( "invalid next log line '"
       + next_log_line + "' when attempting to construct script" );

   string cmd( next_log_line.substr( 0, pos ) );

   bool is_perform_command = false;

   if( cmd == "pc" || cmd == "pu" || cmd == "pd" || cmd == "pe" )
   {
      is_perform_command = true;
      next_log_line.insert( pos + 1, account + ' ' );
   }

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

   if( pos == string::npos && is_perform_command )
      throw runtime_error( "invalid next log line '"
       + next_log_line + "' when attempting to construct script" );

   return pos;
}

pair< uint64_t, uint64_t > verify_block( const string& content,
 bool check_sigs, vector< pair< string, string > >* p_extras, block_info* p_block_info = 0 );

void verify_data( const string& content,
 bool check_sigs, vector< pair< string, string > >* p_extras, data_info* p_data_info = 0 );

void verify_lamport( const string& content,
 bool check_sigs, vector< pair< string, string > >* p_extras, lamport_info* p_lamport_info = 0 );

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

void get_unconfirmed_transactions(
 const transactions_info& tx_info, vector< string >& txs,
 bool is_test, bool exclude_sigs = false, bool add_file_hashes = false )
{
   map< string, unsigned int > account_txs;

   for( size_t i = 0; i < tx_info.transaction_hashes_with_sigs.size( ); i++ )
   {
      string next_tx( tx_info.transaction_hashes_with_sigs[ i ] );

      string::size_type pos = next_tx.find( ':' );

      string signature_with_sep;
      if( pos != string::npos )
      {
         signature_with_sep = next_tx.substr( pos );
         next_tx.erase( pos );
      }

      string tags( get_hash_tags( next_tx ) );
      string encoded_hash( hex_to_base64( next_tx ) );

      if( !tags.empty( ) )
      {
         pos = tags.find( ".t" );

         if( ++account_txs[ tags.substr( 0, pos ) ] > c_tx_max_unconfirmed )
            break;

         if( add_file_hashes )
         {
            transaction_info tinfo;
            get_transaction_info( tinfo, next_tx );

            for( size_t j = 0; j < tinfo.log_lines.size( ); j++ )
            {
               string next_log_line( tinfo.log_lines[ j ] );

               if( next_log_line.find( c_file_type_core_transaction_special_file_extract_prefix ) == 0 )
               {
                  vector< string > parts;
                  split( next_log_line, parts, ' ' );

                  if( parts.size( ) > 1 )
                     txs.push_back( is_test ? parts[ 1 ] : hex_to_base64( parts[ 1 ] ) );
               }
            }
         }

         string s( is_test ? next_tx : encoded_hash );

         txs.push_back( ( is_test || exclude_sigs ) ? s : s + signature_with_sep );
      }
   }
}

void get_transactions_from_transactions_info( const string& chain_id,
 transactions_info& txs_info, map< string, size_t >* p_tx_hash_offsets = 0 )
{
   string txs_info_tag( "c" + chain_id + ".txinfo" );

   if( has_tag( txs_info_tag ) )
   {
      string txs_info_hash( tag_file_hash( txs_info_tag ) );
      get_transactions_info( txs_info, txs_info_hash );

      if( p_tx_hash_offsets )
      {
         for( size_t i = 0; i < txs_info.transaction_hashes_with_sigs.size( ); i++ )
         {
            string next_hash_with_sig( txs_info.transaction_hashes_with_sigs[ i ] );

            string::size_type pos = next_hash_with_sig.find( ':' );
            p_tx_hash_offsets->insert( make_pair( next_hash_with_sig.substr( 0, pos ), i ) );
         }
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
   guard g( g_mutex, "verify_block" );

   vector< string > lines;

   if( !content.empty( ) )
      split( content, lines, '\n', c_esc, false );

   if( lines.empty( ) )
      throw runtime_error( "unexpected empty block content" );

   uint64_t block_height = 0;
   uint64_t previous_block_height = 0;

   uint64_t block_weight = 0;
   uint64_t total_weight = 0;
   uint64_t previous_block_weight = 0;

   string chain, account, account_hash, account_lock, previous_head, previous_block, public_key_base64;

   chain_info cinfo;

   string block_signature, chain_info_hash, past_previous_block;

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

               pos = account.find( '!' );

               if( pos != string::npos )
               {
                  string encoded_list_hash( account.substr( pos + 1 ) );
                  set_session_variable( get_special_var_name( e_special_var_ciyam_list_hash ), base64_to_hex( encoded_list_hash ) );

                  account.erase( pos );
               }

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

      if( block_height < cinfo.checkpoint_start_height )
         throw runtime_error( "invalid block height is below current checkpoint" );
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

   string mint_address;

   bool is_new_chain_head = false;
   bool is_better_chain_head = false;
   bool is_better_parallel_block = false;

   uint32_t previous_nonce_value = 0;

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
            uint64_t previous_weight = get_block_info( binfo, previous_head ).second;

            if( previous_weight > total_weight )
            {
               is_new_chain_head = true;
               is_better_chain_head = true;
               is_better_parallel_block = true;
            }
            else if( previous_weight == total_weight )
               previous_nonce_value = binfo.nonce_value;
         }
      }
   }

   uint64_t num_accounts = 0;

   vector< string > transaction_hashes;

   bool had_nonce = false;
   bool had_signature = false;
   bool has_secondary_account = false;

   string nonce_data;
   uint32_t nonce_value;

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

               string tags( "c" + chain + ".a" + id + ".h0.b" + to_string( cinfo.mint_reward ) );
               p_extras->push_back( make_pair( extra, tags ) );
            }
            else if( !has_secondary_account )
            {
               has_secondary_account = true;

               // NOTE: If the account charge is explicitly provided with a zero value
               // then this determines that the blockchain will not allow the creation
               // of any secondary accounts.
               if( had_zero_explicit_account_charge )
                  throw runtime_error( "secondary account creation not permitted" );

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
      else if( !had_nonce
       && prefix == string( c_file_type_core_block_detail_proof_of_work_prefix ) )
      {
         had_nonce = true;

         nonce_data = verify;
         nonce_value = from_string< uint32_t >( next_line );

         verify += "\n" + lines[ i ];
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
   else if( p_extras && !cinfo.is_test )
   {
      if( !had_nonce )
         throw runtime_error( "block proof of work missing" );

      // NOTE: As the proof checking does involve considerable effort this
      // is only done after the signature and other header information has
      // already been verified.
      if( check_for_proof_of_work( nonce_data, nonce_value, 1 ).empty( ) )
         throw runtime_error( "invalid block proof of work" );
   }

   bool check_hashes_for_better_chain_head = false;

   // NOTE: If a new block with the same height and weight as the previous
   // chain head has been provided then the "better" chain head is decided
   // by the block with the lowest nonce value. If both the weight and the
   // nonce are identical then the lowest valued hash of the two blocks is
   // used to decide the better chain head.
   if( previous_nonce_value )
   {
      if( nonce_value < previous_nonce_value )
         is_better_chain_head = true;
      else if( nonce_value == previous_nonce_value )
         check_hashes_for_better_chain_head = true;
   }

   if( p_block_info )
   {
      p_block_info->minter_id = "c" + chain + ".a" + account;

      p_block_info->minter_hash = account_hash;
      p_block_info->minter_lock = account_lock;
      p_block_info->minter_pubkey = public_key_base64;

      p_block_info->previous_block = previous_block;

      p_block_info->nonce_value = nonce_value;

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

      string new_block_hash( sha256( raw_block_data ).get_digest_as_string( ) );

      if( check_hashes_for_better_chain_head && new_block_hash < previous_head )
         is_better_chain_head = true;

      string block_id( new_block_hash.substr( 0, c_id_length ) );

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
      vector< vector< string > > retagged_transaction_hashes;

      bool has_any_tx_hashes = !transaction_hashes.empty( );

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
         num_accounts = cinfo.num_accounts;

         map< string, uint64_t > account_balances;

         set< string > reused_transactions;

         map< string, string > old_transaction_hashes;
         set< string > new_transaction_hashes( transaction_hashes.begin( ), transaction_hashes.end( ) );

         if( is_better_parallel_block
          || ( is_new_chain_head && previous_head != previous_block ) )
         {
            bool need_to_rewind_storage = false;

            string new_previous_block, old_previous_block;

            if( is_better_parallel_block )
            {
               parallel_block_height = block_height;

               block_info old_binfo;
               get_block_info( old_binfo, previous_head );

               string prior_block_minter_hash;
               uint64_t previous_balance = get_balance_from_minter_id(
                old_binfo.minter_id, &prior_block_minter_hash );

               previous_balance = get_prior_balance_for_minter( cinfo, old_binfo, previous_balance );

               account_balances.insert( make_pair( old_binfo.minter_id, previous_balance ) );

               p_extras->push_back(
                make_pair( prior_block_minter_hash, old_binfo.minter_id
                + ".h" + to_string( block_height ) + ".b*" + to_string( previous_balance ) ) );

               need_to_rewind_storage = ( old_binfo.transaction_hashes.size( ) > 0 );

               vector< string > retagged_transactions;

               // NOTE: Retag any txs from the previous best block that aren't included in the new block.
               for( vector< string >::iterator i =
                old_binfo.transaction_hashes.begin( );
                i != old_binfo.transaction_hashes.end( ); ++i )
               {
                  if( new_transaction_hashes.count( *i ) )
                     reused_transactions.insert( *i );
                  else
                  {
                     retagged_transactions.push_back( *i );

                     transaction_info tinfo;
                     get_transaction_info( tinfo, *i );

                     string tx_tag( old_binfo.minter_id + ".t" + to_string( tinfo.sequence ) );

                     p_extras->push_back( make_pair( *i, tx_tag ) );
                  }
               }

               if( !retagged_transactions.empty( ) )
                  retagged_transaction_hashes.push_back( retagged_transactions );

               new_previous_block = previous_block;
               old_previous_block = old_binfo.previous_block;
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
               parallel_block_height = block_height;

               while( --parallel_block_height
                && new_previous_block != old_previous_block )
               {
                  TRACE_LOG( TRACE_CORE_FLS, "chain " + chain
                   + " replacing " + old_previous_block.substr( 0, c_id_length ) + " with "
                   + new_previous_block.substr( 0, c_id_length ) + " at height " + to_string( parallel_block_height ) );

                  new_chain_height_blocks.insert( make_pair( parallel_block_height, new_previous_block ) );

                  p_extras->push_back( make_pair( new_previous_block,
                   "c" + chain + ".b" + to_string( parallel_block_height ) ) );

                  block_info old_binfo;
                  get_block_info( old_binfo, old_previous_block );

                  old_previous_block = old_binfo.previous_block;

                  uint64_t total_reward = cinfo.mint_reward
                   + ( cinfo.transaction_reward * old_binfo.transaction_hashes.size( ) );

                  if( !need_to_rewind_storage )
                     need_to_rewind_storage = ( old_binfo.transaction_hashes.size( ) > 0 );

                  // NOTE: Remember the transaction hashes (and minter) that were part of the previous best chain.
                  for( size_t i = 0; i < old_binfo.transaction_hashes.size( ); i++ )
                  {
                     if( new_transaction_hashes.count( old_binfo.transaction_hashes[ i ] ) )
                        reused_transactions.insert( old_binfo.transaction_hashes[ i ] );
                     else
                        old_transaction_hashes.insert( make_pair( old_binfo.transaction_hashes[ i ], old_binfo.minter_id ) );
                  }

                  string old_minter_hash, old_minter_tag;

                  uint64_t previous_balance = get_balance_from_minter_id(
                   old_binfo.minter_id, &old_minter_hash, &old_minter_tag );

                  if( account_balances.count( old_binfo.minter_id ) )
                     previous_balance = account_balances[ old_binfo.minter_id ];

                  previous_balance = get_prior_balance_for_minter( cinfo, old_binfo, previous_balance );

                  account_balances[ old_binfo.minter_id ] = previous_balance;

                  string::size_type pos = old_minter_tag.find( ".b" );
                  if( pos == string::npos )
                     throw runtime_error( "unexpected old_minter_tag '" + old_minter_tag + "'" );

                  old_minter_tag.erase( pos );

                  p_extras->push_back( make_pair( old_minter_hash,
                   old_minter_tag + ".b*" + to_string( previous_balance ) ) );

                  block_info new_binfo;
                  get_block_info( new_binfo, new_previous_block );

                  string new_minter_hash, new_minter_tag;
                  uint64_t new_account_balance = get_balance_from_minter_id(
                   new_binfo.minter_id, &new_minter_hash, &new_minter_tag );

                  if( !new_binfo.transaction_hashes.empty( ) )
                     has_any_tx_hashes = true;

                  all_transaction_hashes.push_back( new_binfo.transaction_hashes );

                  for( size_t i = 0; i < new_binfo.transaction_hashes.size( ); i++ )
                  {
                     if( !new_transaction_hashes.count( new_binfo.transaction_hashes[ i ] ) )
                        new_transaction_hashes.insert( new_binfo.transaction_hashes[ i ] );
                     else
                        throw runtime_error( "transaction " + new_binfo.transaction_hashes[ i ]
                         + " has been repeated in the new block at height " + to_string( block_height ) );
                  }

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

                  p_extras->push_back( make_pair( new_minter_hash,
                   new_minter_tag + ".b*" + to_string( new_account_balance ) ) );
               }

               vector< string > retagged_transactions;

               // NOTE: Any old best chain transactions that did not end up in the new best chain are now re-tagged.
               for( map< string, string >::iterator i = old_transaction_hashes.begin( ); i != old_transaction_hashes.end( ); ++i )
               {
                  if( new_transaction_hashes.count( i->first ) )
                     reused_transactions.insert( i->first );
                  else
                  {
                     retagged_transactions.push_back( i->first );

                     transaction_info tinfo;
                     get_transaction_info( tinfo, i->first );

                     string tx_tag( "c" + chain + ".a" + i->second + ".t" + to_string( tinfo.sequence ) );

                     p_extras->push_back( make_pair( i->first, tx_tag ) );
                  }
               }

               if( !retagged_transactions.empty( ) )
                  retagged_transaction_hashes.push_back( retagged_transactions );
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

         if( ainfo.block_lock != mint_address )
            throw runtime_error( "invalid public key from minter" );

         if( !check_if_valid_hash_pair( account_hash, ainfo.block_hash, true ) )
            throw runtime_error( "invalid hash from minter" );

         string::size_type pos = minter_account_tag.find( ".b" );

         if( pos == string::npos )
            throw runtime_error( "unable to find account balance in '" + minter_account_tag + "'" );

         if( account_balances.count( minter_account ) )
            ainfo.balance = account_balances[ minter_account ];

         if( has_secondary_account && ainfo.balance < cinfo.account_charge )
            throw runtime_error( "insufficient balance to create an account" );

         if( has_secondary_account )
            ainfo.balance -= cinfo.account_charge;

         pos = minter_account_tag.find( ".h" );
         if( pos != string::npos )
            minter_account_tag.erase( pos );

         if( is_new_chain_head )
         {
            TRACE_LOG( TRACE_CORE_FLS, string( is_better_chain_head ? "better chain " : "new chain " )
             + chain + " head block " + block_id + " at height " + to_string( block_height ) );

            if( !has_secondary_account )
               ainfo.balance += cinfo.mint_reward;

            ainfo.balance += ( transaction_hashes.size( ) * cinfo.transaction_reward );
         }

         // NOTE: If is the new chain head then remove the transaction file tag for all transactions included
         // (after verifying that the transaction hasn't already been included and that it has no unconfirmed
         // previous transaction). After processing all the transactions in the block itself any transactions
         // that were included from parallel blocks will have their tag (if present) removed also.
         if( is_new_chain_head && !new_transaction_hashes.empty( ) )
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

               included_transactions.insert( transaction_hashes[ i ] );

               if( !reused_transactions.count( transaction_hashes[ i ] ) )
                  p_extras->push_back( make_pair(
                   transaction_hashes[ i ], get_hash_tags( transaction_hashes[ i ] ) + "*" ) );
            }

            for( set< string >::iterator i = new_transaction_hashes.begin( ); i != new_transaction_hashes.end( ); ++i )
            {
               if( !included_transactions.count( *i ) )
               {
                  string hash_tags( get_hash_tags( *i ) );

                  if( !hash_tags.empty( ) )
                     p_extras->push_back( make_pair( *i, hash_tags + "*" ) );
               }
            }
         }

         string tags, extra;

         extra = string( c_file_type_str_core_blob );
         extra += get_account_data( ainfo, account_hash, account_lock, "", "", "" );

         // NOTE: The line feed is required to ensure that the tag is applied as a secondary one.
         tags += "\n" + minter_account_tag + ".h*" + to_string( block_height ) + ".b" + to_string( ainfo.balance );

         p_extras->push_back( make_pair( extra, tags ) );

         // NOTE: The previous account blob instance will be removed.
         p_extras->push_back( make_pair( "", minter_account_hash ) );
      }

      // NOTE: The parallel block height (if the chain has not just been extended)
      // as well as all relevant transaction hashes (which include all those found
      // in the current block as well as those from all previous blocks going back
      // to the parallel block height) are appended to a <blockchain>.txs file.
      if( has_any_tx_hashes && is_new_chain_head )
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
            throw runtime_error( "*** unexpected error occurred appending .txs for blockchain ***" );
      }

      // NOTE: Any transactions that have ended up being retagged will be appended
      // in their original order to a <blockchain>.txs.new file so that they later
      // will be able to be appended in the same manner that local txs are.
      if( is_new_chain_head && !retagged_transaction_hashes.empty( ) )
      {
         string filename( chain + ".txs.new" );

         ofstream outf( filename.c_str( ), ios::out | ios::app );
         if( !outf )
            throw runtime_error( "unable to open '" + filename + "' for append" );

         for( size_t i = retagged_transaction_hashes.size( ) - 1; ; )
         {
            for( size_t j = 0; j < retagged_transaction_hashes[ i ].size( ); j++ )
               outf << retagged_transaction_hashes[ i ][ j ] << '\n';

            if( i == 0 )
               break;
            else
               --i;
         }

         outf.flush( );
         if( !outf.good( ) )
            throw runtime_error( "*** unexpected error occurred appending .txs.new for blockchain ***" );
      }

      uint64_t checkpoint_height = 0;

      // NOTE: Determine whether or not a new automatic checkpoint has occurred.
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
            TRACE_LOG( TRACE_CORE_FLS, "new checkpoint for "
             + chain + " at height " + to_string( checkpoint_height ) );

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

            for( size_t i = 0; i < account_tags.size( ); i++ )
            {
               string next_account_tag( account_tags[ i ] );

               string::size_type pos = next_account_tag.find( ".b" );
               string::size_type hpos = next_account_tag.find( ".h" );

               if( pos == string::npos || hpos == string::npos )
                  continue;

               bool is_banned = ( next_account_tag.find( "banned" ) != string::npos );

               if( !is_banned )
                  ++num_accounts;
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

            get_transactions_from_transactions_info( chain, txs_info, &tx_hash_offsets );

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

                  // NOTE: Put all transaction hashes (with signatures) for each block
                  // before appending the block hash and signature. Any attached files
                  // that a transaction will have their hashes included as well.
                  for( size_t k = 0; k < binfo.transaction_hashes.size( ); k++ )
                  {
                     string next_tx_hash( binfo.transaction_hashes[ k ] );

                     if( !tx_hash_offsets.count( next_tx_hash ) )
                        throw runtime_error( "unable to find tx hash " + next_tx_hash + " in verify_block" );

                     transaction_info tinfo;
                     get_transaction_info( tinfo, next_tx_hash );

                     for( size_t l = 0; l < tinfo.log_lines.size( ); l++ )
                     {
                        string next_log_line( tinfo.log_lines[ l ] );

                        if( next_log_line.find( c_file_type_core_transaction_special_file_extract_prefix ) == 0 )
                        {
                           vector< string > parts;
                           split( next_log_line, parts, ' ' );

                           if( parts.size( ) > 1 )
                              checkpoint_info_data += '\n' + ( cinfo.is_test ? parts[ 1 ] : hex_to_base64( parts[ 1 ] ) );
                        }
                     }

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

            // NOTE: Remove the "chain" tags that had been created prior to the checkpoint.
            if( cinfo.is_test )
            {
               string all_chain_tags( list_file_tags( "c" + chain
                + ".chain.b" + to_string( cinfo.checkpoint_start_height + 1 ) + "-*" ) );

               if( !all_chain_tags.empty( ) )
               {
                  vector< string > chain_tags;
                  split( all_chain_tags, chain_tags, '\n' );

                  for( size_t i = 0; i < chain_tags.size( ); i++ )
                  {
                     string next_tag( chain_tags[ i ] );
                     string next_hash( tag_file_hash( next_tag ) );

                     p_extras->push_back( make_pair( next_hash, next_tag + "*" ) );
                  }
               }
            }

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

      // NOTE: If is a new chain head on a test chain that hasn't just
      // checkpointed then add a "chain" tag to the block (in order to
      // make it easy to visualise the links to each previous block).
      if( cinfo.is_test && is_new_chain_head && !checkpoint_height )
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
         p_extras->push_back( make_pair( "", chain_info_hash ) );
      }
   }

   return make_pair( block_height, total_weight );
}

void verify_data( const string& content,
 bool check_sigs, vector< pair< string, string > >* p_extras, data_info* p_data_info )
{
   guard g( g_mutex, "verify_data" );

   vector< string > lines;

   if( !content.empty( ) )
      split( content, lines, '\n', c_esc, false );

   if( lines.empty( ) )
      throw runtime_error( "unexpected empty data content" );

   string identity, last_data_hash, public_key_hash, tree_root_hash;

   uint64_t data_height = 0;
   uint64_t unix_time_stamp = 0;

   string header( lines[ 0 ] );
   if( header.empty( ) )
      throw runtime_error( "unexpected empty data header" );
   else
   {
      vector< string > attributes;
      split( header, attributes );

      if( attributes.empty( ) )
         throw runtime_error( "unexpected empty data header attributes" );

      bool has_height = false;
      bool has_identity = false;

      for( size_t i = 0; i < attributes.size( ); i++ )
      {
         string next_attribute( attributes[ i ] );
         if( next_attribute.empty( ) )
            throw runtime_error( "unexpected empty attribute in data header '" + header + "'" );

         if( !has_height )
         {
            if( next_attribute.find( c_file_type_core_data_header_height_prefix ) != 0 )
               throw runtime_error( "unexpected missing height attribute in data header '" + header + "'" );

            string value( next_attribute.substr(
             string( c_file_type_core_data_header_height_prefix ).length( ) ) );

            regex expr( c_regex_integer );

            if( ( value.length( ) > 1 && value[ 0 ] == '0' ) || expr.search( value ) != 0 )
               throw runtime_error( "invalid height value '" + value + "'" );

            has_height = true;

            data_height = from_string< uint64_t >( value );
         }
         else if( !has_identity )
         {
            if( next_attribute.find( c_file_type_core_data_header_identity_prefix ) != 0 )
               throw runtime_error( "unexpected missing identity attribute in data header '" + header + "'" );

            has_identity = true;

            identity = next_attribute.substr(
             string( c_file_type_core_data_header_identity_prefix ).length( ) );
         }
         else
            throw runtime_error( "unexpected extraneous attribute in data header '" + header + "'" );
      }

      if( !has_identity )
         throw runtime_error( "unexpected missing identity attribute in data header '" + header + "'" );
   }

   data_info info;

   for( size_t i = 1; i < lines.size( ); i++ )
   {
      string next_line( lines[ i ] );

      if( next_line.size( ) < 3 )
         throw runtime_error( "unexpected line < 3 '" + next_line + "' in verify_data" );

      if( next_line.length( ) > c_max_core_line_size )
         throw runtime_error( "unexpected line length exceeds "
          + to_string( c_max_core_line_size ) + " in '" + next_line + "' in verify_data" );

      string next_attribute( next_line );

      bool found = false;

      if( data_height > 1 && last_data_hash.empty( ) )
      {
         size_t len = strlen( c_file_type_core_data_detail_last_hash_prefix );

         if( next_attribute.substr( 0, len ) != string( c_file_type_core_data_detail_last_hash_prefix ) )
            throw runtime_error( "invalid data last data hash attribute '" + next_attribute + "'" );

         next_attribute.erase( 0, len );

         last_data_hash = hex_encode( base64::decode( next_attribute ) );

         if( !has_file( last_data_hash ) )
            throw runtime_error( "last data file '" + last_data_hash + "' not found" );

         if( check_sigs )
         {
            string last_data_info( extract_file( last_data_hash, "", c_file_type_char_core_blob ) );

            string::size_type pos = last_data_info.find( ':' );
            if( pos == string::npos )
               throw runtime_error( "unexpected invalid data info in validate_data" );

            verify_data( last_data_info.substr( pos + 1 ), false, 0, &info );
         }

         found = true;
      }

      if( !found && public_key_hash.empty( ) )
      {
         size_t len = strlen( c_file_type_core_data_detail_pubkey_hash_prefix );

         if( next_attribute.substr( 0, len ) != string( c_file_type_core_data_detail_pubkey_hash_prefix ) )
            throw runtime_error( "invalid data public key hash attribute '" + next_attribute + "'" );

         next_attribute.erase( 0, len );

         public_key_hash = hex_encode( base64::decode( next_attribute ) );

         if( !has_file( public_key_hash ) )
            throw runtime_error( "public key file '" + public_key_hash + "' not found" );

         if( p_data_info )
            p_data_info->public_key_hash = public_key_hash;

         found = true;
      }

      if( !found && tree_root_hash.empty( ) )
      {
         size_t len = strlen( c_file_type_core_data_detail_tree_root_hash_prefix );

         if( next_attribute.substr( 0, len ) == string( c_file_type_core_data_detail_tree_root_hash_prefix ) )
         {
            next_attribute.erase( 0, len );

            tree_root_hash = hex_encode( base64::decode( next_attribute ) );

            if( !has_file( tree_root_hash ) )
               throw runtime_error( "tree root file '" + tree_root_hash + "' not found" );

            found = true;
         }
      }

      if( !found && !unix_time_stamp )
      {
         size_t len = strlen( c_file_type_core_data_detail_unix_time_stamp_prefix );

         if( next_attribute.substr( 0, len ) != string( c_file_type_core_data_detail_unix_time_stamp_prefix ) )
            throw runtime_error( "invalid data unix time stamp attribute '" + next_attribute + "'" );

         next_attribute.erase( 0, len );

         unix_time_stamp = from_string< uint64_t >( next_attribute );

         if( p_data_info )
            p_data_info->unix_time_stamp = unix_time_stamp;

         if( check_sigs && ( data_height > 1 && unix_time_stamp <= info.unix_time_stamp ) )
            throw runtime_error( "invalid unix data time stamp not more recent than last" );

         found = true;
      }

      if( !found )
         throw runtime_error( "unexpected extraneous data attribute '" + next_attribute + "'" );
   }

   if( public_key_hash.empty( ) )
      throw runtime_error( "unexpected missing public key hash attribute" );

   if( data_height > 1 && last_data_hash.empty( ) )
      throw runtime_error( "unexpected missing data last data hash attribute" );

   if( !unix_time_stamp )
      throw runtime_error( "unexpected missing unix data time stamp attribute" );
}

void verify_lamport( const string& content,
 bool check_sigs, vector< pair< string, string > >* p_extras, lamport_info* p_lamport_info )
{
   guard g( g_mutex, "verify_lamport" );

   vector< string > lines;

   if( !content.empty( ) )
      split( content, lines, '\n', c_esc, false );

   if( lines.empty( ) )
      throw runtime_error( "unexpected empty lamport content" );

   string identity;

   uint64_t lamport_height = 0;
   uint64_t unix_time_stamp = 0;

   string header( lines[ 0 ] );
   if( header.empty( ) )
      throw runtime_error( "unexpected empty lamport header" );
   else
   {
      vector< string > attributes;
      split( header, attributes );

      if( attributes.empty( ) )
         throw runtime_error( "unexpected empty lamport header attributes" );

      bool has_height = false;
      bool has_identity = false;

      for( size_t i = 0; i < attributes.size( ); i++ )
      {
         string next_attribute( attributes[ i ] );
         if( next_attribute.empty( ) )
            throw runtime_error( "unexpected empty attribute in lamport header '" + header + "'" );

         if( !has_height )
         {
            if( next_attribute.find( c_file_type_core_lamport_header_height_prefix ) != 0 )
               throw runtime_error( "unexpected missing height attribute in lamport header '" + header + "'" );

            string value( next_attribute.substr(
             string( c_file_type_core_lamport_header_height_prefix ).length( ) ) );

            regex expr( c_regex_integer );

            if( ( value.length( ) > 1 && value[ 0 ] == '0' ) || expr.search( value ) != 0 )
               throw runtime_error( "invalid height value '" + value + "'" );

            has_height = true;

            lamport_height = from_string< uint64_t >( value );
         }
         else if( !has_identity )
         {
            if( next_attribute.find( c_file_type_core_lamport_header_identity_prefix ) != 0 )
               throw runtime_error( "unexpected missing identity attribute in lamport header '" + header + "'" );

            has_identity = true;

            identity = next_attribute.substr(
             string( c_file_type_core_lamport_header_identity_prefix ).length( ) );
         }
         else
            throw runtime_error( "unexpected extraneous attribute in lamport header '" + header + "'" );
      }

      if( identity.length( ) != c_bc_identity_length )
         throw runtime_error( "unexpected missing or incorrect identity attribute in lamport header '" + header + "'" );
   }

   data_info data;
   lamport_info info;

   string last_block_hash, public_key_hash, signature_file_hash;

   bool has_primary_pubkey = false;
   bool has_secondary_pubkey = false;

   for( size_t i = 1; i < lines.size( ); i++ )
   {
      string next_line( lines[ i ] );

      if( next_line.size( ) < 3 )
         throw runtime_error( "unexpected line < 3 '" + next_line + "' in verify_lamport" );

      if( next_line.length( ) > c_max_core_line_size )
         throw runtime_error( "unexpected line length exceeds "
          + to_string( c_max_core_line_size ) + " in '" + next_line + "' in verify_lamport" );

      string next_attribute( next_line );

      if( !lamport_height )
      {
         if( !has_primary_pubkey )
         {
            size_t len = strlen( c_file_type_core_lamport_detail_primary_pubkey_prefix );

            if( next_attribute.substr( 0, len ) != string( c_file_type_core_lamport_detail_primary_pubkey_prefix ) )
               throw runtime_error( "invalid genesis lamport primary pubkey attribute '" + next_attribute + "'" );

            next_attribute.erase( 0, len );

            has_primary_pubkey = true;

            if( p_lamport_info )
               p_lamport_info->public_key_hash = hex_encode( base64::decode( next_attribute ) );
         }
         else if( !has_secondary_pubkey )
         {
            size_t len = strlen( c_file_type_core_lamport_detail_secondary_pubkey_prefix );

            if( next_attribute.substr( 0, len ) != string( c_file_type_core_lamport_detail_secondary_pubkey_prefix ) )
               throw runtime_error( "invalid genesis lamport secondary pubkey attribute '" + next_attribute + "'" );

            next_attribute.erase( 0, len );

            has_secondary_pubkey = true;
         }
         else if( !unix_time_stamp )
         {
            size_t len = strlen( c_file_type_core_lamport_detail_unix_block_time_stamp_prefix );

            if( next_attribute.substr( 0, len ) != string( c_file_type_core_lamport_detail_unix_block_time_stamp_prefix ) )
               throw runtime_error( "invalid unix lamport time stamp attribute '" + next_attribute + "'" );

            next_attribute.erase( 0, len );

            unix_time_stamp = from_string< uint64_t >( next_attribute );

            if( p_lamport_info )
               p_lamport_info->unix_time_stamp = unix_time_stamp;
         }
         else
            throw runtime_error( "unexpected extraneous genesis lamport attribute '" + next_attribute + "'" );
      }
      else
      {
         if( last_block_hash.empty( ) )
         {
            size_t len = strlen( c_file_type_core_lamport_detail_last_hash_prefix );

            if( next_attribute.substr( 0, len ) != string( c_file_type_core_lamport_detail_last_hash_prefix ) )
               throw runtime_error( "invalid lamport last block hash attribute '" + next_attribute + "'" );

            next_attribute.erase( 0, len );

            last_block_hash = hex_encode( base64::decode( next_attribute ) );

            if( !has_file( last_block_hash ) )
               throw runtime_error( "last block file '" + last_block_hash + "' not found" );

            string last_block_tag( "bc." + identity + "." + to_string( lamport_height - 1 ) + ".blk" );

            if( tag_file_hash( last_block_tag ) != last_block_hash )
               throw runtime_error( "incorrect last block hash '" + last_block_hash + "'" );
         }
         else if( public_key_hash.empty( ) )
         {
            size_t len = strlen( c_file_type_core_lamport_detail_pubkey_hash_prefix );

            if( next_attribute.substr( 0, len ) != string( c_file_type_core_lamport_detail_pubkey_hash_prefix ) )
               throw runtime_error( "invalid data public key hash attribute '" + next_attribute + "'" );

            next_attribute.erase( 0, len );

            public_key_hash = hex_encode( base64::decode( next_attribute ) );

            if( !has_file( public_key_hash ) )
               throw runtime_error( "public key file '" + public_key_hash + "' not found" );

            if( p_lamport_info )
               p_lamport_info->public_key_hash = public_key_hash;
         }
         else if( signature_file_hash.empty( ) )
         {
            size_t len = strlen( c_file_type_core_lamport_detail_signature_file_hash_prefix );

            if( next_attribute.substr( 0, len ) != string( c_file_type_core_lamport_detail_signature_file_hash_prefix ) )
               throw runtime_error( "invalid lamport signature file hash attribute '" + next_attribute + "'" );

            next_attribute.erase( 0, len );

            signature_file_hash = hex_encode( base64::decode( next_attribute ) );

            if( !has_file( signature_file_hash ) )
               throw runtime_error( "signature file '" + signature_file_hash + "' not found" );

            if( check_sigs )
            {
               string last_block_info( extract_file( last_block_hash, "", c_file_type_char_core_blob ) );

               string::size_type pos = last_block_info.find( ':' );
               if( pos == string::npos )
                  throw runtime_error( "unexpected invalid block info in validate_lamport" );

               verify_lamport( last_block_info.substr( pos + 1 ), false, 0, &info );

               string file_hash_info( info.public_key_hash + ':' + signature_file_hash );
               string data_file_hash( crypto_lamport( file_hash_info, "", false, true ) );

               if( p_lamport_info )
                  p_lamport_info->data_file_hash = data_file_hash;

               if( !has_file( data_file_hash ) )
                  throw runtime_error( "data file '" + data_file_hash + "' not found" );

               string data_file_info( extract_file( data_file_hash, "", c_file_type_char_core_blob ) );

               pos = data_file_info.find( ':' );
               if( pos == string::npos )
                  throw runtime_error( "unexpected invalid data info in validate_lamport" );

               verify_data( data_file_info.substr( pos + 1 ), false, 0, &data );

               if( public_key_hash != data.public_key_hash )
                  throw runtime_error( "unexpected public_key_hash does not match data info" );
            }
         }
         else if( !unix_time_stamp )
         {
            size_t len = strlen( c_file_type_core_lamport_detail_unix_block_time_stamp_prefix );

            if( next_attribute.substr( 0, len ) != string( c_file_type_core_lamport_detail_unix_block_time_stamp_prefix ) )
               throw runtime_error( "invalid unix lamport time stamp attribute '" + next_attribute + "'" );

            next_attribute.erase( 0, len );

            unix_time_stamp = from_string< uint64_t >( next_attribute );

            if( p_lamport_info )
               p_lamport_info->unix_time_stamp = unix_time_stamp;

            if( check_sigs && ( unix_time_stamp <= info.unix_time_stamp ) )
               throw runtime_error( "invalid unix lamport time stamp not more recent than last" );

            if( check_sigs && ( unix_time_stamp < data.unix_time_stamp ) )
               throw runtime_error( "invalid unix lamport time stamp older than unix data time stamp" );
         }
         else
            throw runtime_error( "unexpected extraneous lamport attribute '" + next_attribute + "'" );
      }
   }

   if( !unix_time_stamp )
      throw runtime_error( "unexpected missing unix lamport time stamp attribute" );

   if( !lamport_height )
   {
      if( !has_primary_pubkey )
         throw runtime_error( "unexpected missing genesis lamport primary pubkey attribute" );

      if( !has_secondary_pubkey )
         throw runtime_error( "unexpected missing genesis lamport secondary pubkey attribute" );
   }
   else
   {
      if( last_block_hash.empty( ) )
         throw runtime_error( "unexpected missing lamport last block hash attribute" );

      if( signature_file_hash.empty( ) )
         throw runtime_error( "unexpected missing lamport signature file hash attribute" );
   }
}

void verify_rewind( const string& content, vector< pair< string, string > >* p_extras )
{
   guard g( g_mutex, "verify_rewind" );

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

      map< string, string > account_block_locks;
      map< string, string > account_block_hashes;

      map< string, uint64_t > account_balances;
      map< string, uint64_t > account_block_heights;

      TRACE_LOG( TRACE_CORE_FLS, "rewinding chain " + chain_id + " to block height " + to_string( new_block_height ) );

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
                  balance = get_prior_balance_for_minter( cinfo, next_account_binfo, balance );

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

      p_extras->push_back( make_pair( destination_block_hash, chain + ".head" ) );
   }
}

void verify_transaction( const string& content, bool check_sigs,
 vector< pair< string, string > >* p_extras, transaction_info* p_transaction_info )
{
   guard g( g_mutex, "verify_transaction" );

   vector< string > lines;
   split( content, lines, '\n', c_esc, false );

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

      // FUTURE: The minimum balance for being able to issue a transaction should
      // be an attribute of the chain meta data rather than being hard-coded here.
      if( !ainfo.balance )
         throw runtime_error( "zero balance tx not permitted for account '" + account + "' in chain '" + chain + "'" );

      expected_sequence = ainfo.num_transactions + 1;
   }

   string verify( string( c_file_type_core_transaction_object ) + ':' + header );

   string transaction_address;

   int num_log_lines = 0;
   bool had_signature = false;

   string file_extract_lines;
   string create_and_update_lines;

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

         if( cmd != c_file_type_core_transaction_special_file_extract_command
          && cmd != c_file_type_core_transaction_special_perform_create_command
          && cmd != c_file_type_core_transaction_special_perform_update_command
          && cmd != c_file_type_core_transaction_special_perform_destroy_command
          && cmd != c_file_type_core_transaction_special_perform_execute_command )
            throw runtime_error( "invalid cmd '" + cmd + "' in log line '" + next_line + "'" );

         if( cmd == c_file_type_core_transaction_special_file_extract_command )
         {
            if( !file_extract_lines.empty( ) )
               file_extract_lines += '\n';
            file_extract_lines += next_line;
         }

         if( cmd == c_file_type_core_transaction_special_perform_create_command
          || cmd == c_file_type_core_transaction_special_perform_update_command )
         {
            pos = next_line.find( '"' );
            if( pos == string::npos )
               throw runtime_error( "invalid transaction log line '" + next_line + "'" );

            if( !create_and_update_lines.empty( ) )
               create_and_update_lines += '\n';
            create_and_update_lines += next_line;
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
         throw runtime_error( "unexpected line '" + lines[ i ] + "' in verify_transaction" );
   }

   if( p_extras && !had_signature )
      throw runtime_error( "transaction signature missing" );

   if( !file_extract_lines.empty( ) )
   {
      vector< string > file_extracts;
      split( file_extract_lines, file_extracts, '\n' );

      for( size_t i = 0; i < file_extracts.size( ); i++ )
      {
         string next_file_extract( file_extracts[ i ] );

         vector< string > parts;
         split( next_file_extract, parts, ' ' );

         if( parts.size( ) != 3 )
            throw runtime_error( "invalid line '" + next_file_extract + "' in verify_transaction" );

         string filename( parts[ 2 ] );

         string::size_type spos = filename.find( '-' );
         string::size_type xpos = filename.find( '.' );
         string::size_type fpos = filename.find( 'F', spos );

         if( spos == string::npos || xpos == string::npos || fpos == string::npos || fpos < spos )
            throw runtime_error( "invalid format for line '" + next_file_extract + "' in verify_transaction" );

         string field( filename.substr( fpos, xpos - fpos ) );

         string field_and_value( field + '=' + filename );

         string::size_type pos = create_and_update_lines.find( field_and_value );
         if( pos == string::npos )
            throw runtime_error( "did not find '" + field_and_value + "' in transaction create/update lines" );

         create_and_update_lines.erase( pos, field_and_value.length( ) );
      }
   }

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

         get_transaction_info( tinfo, next_transaction_id );
         if( sequence != tinfo.sequence + 1 )
            error_message = "sequence number does not follow that of the previous transaction";

         // NOTE: If isn't a local tx then need to ensure the max. number of unconfirmed (ignoring
         // those that have become unconfirmed again due to a rewind) transactions for the account
         // has not been exceeded.
         if( get_raw_session_variable( get_special_var_name( e_special_var_blockchain ) ).empty( ) )
         {
            string all_unconfirmed( list_file_tags( tinfo.account_id + ".t*.s*" ) );
            if( !all_unconfirmed.empty( ) )
            {
               if( split_size( all_unconfirmed, '\n' ) > c_tx_max_unconfirmed )
                  throw runtime_error( "already has maximum unconfirmed transactions for account: " + account );
            }
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

      if( !error_message.empty( ) )
         throw runtime_error( error_message );

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
   guard g( g_mutex, "verify_blockchain_info" );

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
            string blob_hash_with_sig;

            string::size_type pos = next_line.find( ':' );

            if( pos == string::npos )
               blob_hash_with_sig = cinfo.is_test ? next_line : base64_to_hex( next_line );
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

      get_transactions_from_transactions_info( chain_id, txs_info, &tx_hash_offsets );

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

               transaction_info tinfo;
               get_transaction_info( tinfo, next_tx_hash );

               for( size_t j = 0; j < tinfo.log_lines.size( ); j++ )
               {
                  string next_log_line( tinfo.log_lines[ j ] );

                  if( next_log_line.find( c_file_type_core_transaction_special_file_extract_prefix ) == 0 )
                  {
                     vector< string > parts;
                     split( next_log_line, parts, ' ' );

                     if( parts.size( ) > 1 )
                        blockchain_info_data += '\n' + ( cinfo.is_test ? parts[ 1 ] : hex_to_base64( parts[ 1 ] ) );
                  }
               }

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

      vector< string > unconfirmed_txs;
      get_unconfirmed_transactions( txs_info, unconfirmed_txs, cinfo.is_test, false, true );

      for( size_t i = 0; i < unconfirmed_txs.size( ); i++ )
         blockchain_info_data += "\n" + unconfirmed_txs[ i ];

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
   guard g( g_mutex, "verify_checkpoint_info" );

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
   guard g( g_mutex, "verify_checkpoint_prune" );

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

      // NOTE: All of the relevant checkpoint files will be re-tagged using the
      // special "current time stamp" tag so that they can be later archived or
      // removed (to ensure each tag is unique a counter is being appended).
      string replace_with_timestamp_tag( "*" + current_time_stamp_tag( true ) );

      int timestamp_counter = 0;

      // NOTE: Re-tag all of the checkpoint transaction blobs.
      for( size_t i = 1; i < checkpoint_transactions.size( ); i++ )
      {
         if( has_file( checkpoint_transactions[ i ] ) )
         {
            p_extras->push_back( make_pair( checkpoint_transactions[ i ],
             construct_key_from_int( replace_with_timestamp_tag, ++timestamp_counter, 9 ) ) );
         }
      }

      uint64_t height = checkpoint_height;

      // NOTE: Re-tag all block blobs that are at or below the checkpoint height.
      while( true )
      {
         string all_block_tags( list_file_tags( "c" + chain_id + ".b" + to_string( height ) + "-*" ) );

         if( all_block_tags.empty( ) )
            break;

         vector< string > block_tags;
         split( all_block_tags, block_tags, '\n' );

         for( size_t i = 0; i < block_tags.size( ); i++ )
         {
            p_extras->push_back( make_pair( tag_file_hash( block_tags[ i ] ),
             construct_key_from_int( replace_with_timestamp_tag, ++timestamp_counter, 9 ) ) );
         }

         if( height-- == 0 )
            break;
      }

      // NOTE: The checkpoint blobs themselves are finally re-tagged.
      p_extras->push_back( make_pair( lines[ 1 ],
       construct_key_from_int( replace_with_timestamp_tag, ++timestamp_counter, 9 ) ) );

      p_extras->push_back( make_pair( lines[ 2 ],
       construct_key_from_int( replace_with_timestamp_tag, ++timestamp_counter, 9 ) ) );

      p_extras->push_back( make_pair( checkpoint_hash,
       construct_key_from_int( replace_with_timestamp_tag, ++timestamp_counter, 9 ) ) );

      p_extras->push_back( make_pair( checkpoint_info_hash,
       construct_key_from_int( replace_with_timestamp_tag, ++timestamp_counter, 9 ) ) );
   }
}

void verify_transactions_info( const string& content,
 vector< pair< string, string > >* p_extras, transactions_info* p_transactions_info )
{
   guard g( g_mutex, "verify_transactions_info" );

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

void list_mutex_lock_ids_for_ciyam_core_files( ostream& outs )
{
   outs << "ciyam_core_files::g_mutex = " << g_mutex.get_lock_id( ) << '\n';
}

trace_mutex& get_core_files_trace_mutex( )
{
   return g_mutex;
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

         bool is_lamport = false;

         if( content.find( ":h=" ) == pos )
            is_lamport = true;

         string type( content.substr( 1, pos - 1 ) );

         if( type == string( c_file_type_core_data_object ) )
            verify_data( content.substr( pos + 1 ), check_sigs, p_extras );
         else if( type == string( c_file_type_core_block_object ) )
         {
            if( !is_lamport )
               verify_block( content.substr( pos + 1 ), check_sigs, p_extras );
            else
               verify_lamport( content.substr( pos + 1 ), check_sigs, p_extras );
         }
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

bool has_better_block( const string& blockchain, uint64_t height, uint64_t weight, string* p_acct )
{
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

               if( p_acct )
               {
                  account_info ainfo;
                  get_account_info( ainfo, *p_acct );

                  if( ainfo.last_height >= height )
                     p_acct->erase( );
               }

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

string construct_new_block(
 const string& blockchain, const string& password, const string& account,
 bool use_core_file_format, new_block_info* p_new_block_info, bool search_for_proof_of_work_nonce )
{
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
   string previous_block_hash;

   uint64_t height = 0;
   uint64_t weight = 0;
   uint64_t total_weight = 0;
   uint64_t previous_block_weight = 0;

   chain_info cinfo;

   if( !chain.empty( ) )
      get_chain_info( cinfo, chain );

   if( !chain.empty( ) )
   {
      head_hash = tag_file_hash( "c" + chain + ".head" );

      block_info binfo;
      get_block_info( binfo, head_hash );

      // NOTE: First attempt to create a new block at the same height as the chain head.
      if( !binfo.previous_block.empty( ) )
      {
         height = binfo.block_height;

         block_info prev_binfo;
         get_block_info( prev_binfo, binfo.previous_block );

         weight = get_expected_weight( prev_binfo.minter_hash, from_string< uint64_t >( account_id ), height );

         previous_block_hash = binfo.previous_block;
         previous_block_weight = prev_binfo.block_weight;

         total_weight = prev_binfo.total_weight + weight;
      }

      // NOTE: If the chain head is the genesis block or the new weight isn't better then
      // create a new block that is one block after the current height of the chain head.
      if( weight == 0 || weight >= binfo.block_weight )
      {
         height = binfo.block_height + 1;
         weight = get_expected_weight( binfo.minter_hash, from_string< uint64_t >( account_id ), height );

         previous_block_hash = head_hash;
         previous_block_weight = binfo.block_weight;

         total_weight = binfo.total_weight + weight;
      }

      if( p_new_block_info )
      {
         chain_info cinfo;
         get_chain_info( cinfo, chain );

         p_new_block_info->acct = acct;

         p_new_block_info->height = height;
         p_new_block_info->weight = weight;

         p_new_block_info->total_weight = total_weight;

         p_new_block_info->previous_block_weight = previous_block_weight;

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
   {
      data += "." + acct;

      if( has_files_area_tag( c_ciyam_tag, e_file_type_list ) )
         data += "!" + hex_to_base64( tag_file_hash( c_ciyam_tag ) );
   }

   data += "," + string( c_file_type_core_block_header_height_prefix ) + to_string( height );

   if( acct.empty( ) )
      data += "," + string( c_file_type_core_block_header_chain_meta_prefix )
       + string( c_file_type_core_block_header_chain_meta_requisite_prefix ) + blockchain;
   else
      data += "," + string( c_file_type_core_block_header_weight_prefix ) + to_string( weight )
      + "," + string( c_file_type_core_block_header_account_hash_prefix ) + key_info.block_hash
      + "," + string( c_file_type_core_block_header_account_lock_prefix ) + key_info.block_lock;

   if( !previous_block_hash.empty( ) )
      data += "," + string( c_file_type_core_block_header_previous_block_prefix )
       + ( cinfo.is_test ? previous_block_hash : hex_to_base64( previous_block_hash ) );

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

   transactions_info txs_info;
   get_transactions_from_transactions_info( chain, txs_info );

   int num_txs = 0;

   vector< string > unconfirmed_txs;
   get_unconfirmed_transactions( txs_info, unconfirmed_txs, cinfo.is_test, true );

   for( size_t i = 0; i < unconfirmed_txs.size( ); i++ )
   {
      // NOTE: The "hard limit" for the number of txs per block is determined by the actual
      // file item size limit but a smaller "soft limit" is being used here.
      if( ++num_txs > c_tx_soft_limit )
         break;

      data += "\n" + string( c_file_type_core_block_detail_transaction_prefix ) + unconfirmed_txs[ i ];
   }

   uint32_t start = 0;
#ifdef SSL_SUPPORT
   RAND_bytes( ( unsigned char* )&start, sizeof( start ) );
#endif
   string nonce;

   // NOTE: Don't search for a valid nonce unless it is required.
   if( !cinfo.is_test && search_for_proof_of_work_nonce )
      nonce = check_for_proof_of_work( data, start, p_new_block_info ? 32 : 64 );

   if( p_new_block_info )
      p_new_block_info->num_txs = ( nonce.empty( ) && search_for_proof_of_work_nonce ) ? -1 : num_txs;

   if( !nonce.empty( ) )
      data += "\n" + string( c_file_type_core_block_detail_proof_of_work_prefix ) + nonce;

#ifdef SSL_SUPPORT
   data += "\n" + string( c_file_type_core_block_detail_signature_prefix ) + priv_key.construct_signature( data, true );
#endif

   if( use_core_file_format )
      data = string( c_file_type_str_core_blob ) + data;

   return data;
}

string construct_new_transaction( const string& blockchain, const string& password,
 const string& account, const string& application, const string& transaction_log_lines,
 bool use_core_file_format, string* p_tx_hash, const vector< string >* p_file_info )
{
   string acct;

   string id( get_account_id_from_password( password ) );
   if( !list_file_tags( "c" + blockchain + ".a" + id + ".h*" ).empty( ) )
      acct = id;

   if( acct.empty( ) )
      throw runtime_error( "unknown account: " + id );

   string test_account( account == string( c_admin ) ? blockchain : account );

   if( !test_account.empty( ) && acct != test_account )
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

   if( p_file_info && !p_file_info->empty( ) )
   {
      for( size_t i = 0; i < p_file_info->size( ); i++ )
         data += "\n" + string( c_file_type_core_transaction_detail_log_prefix ) + ( *p_file_info )[ i ];
   }

   if( !transaction_log_lines.empty( ) )
   {
      vector< string > tx_log_lines;
      split( transaction_log_lines, tx_log_lines, '\n', c_esc, false );

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

   if( account_id == string( c_admin ) )
      account_id = blockchain;

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

   string test_account( account == string( c_admin ) ? blockchain : account );

   if( !test_account.empty( ) && acct != test_account )
      throw runtime_error( "invalid password for account: " + account );

   account_key_info key_info;
   string account_id( construct_account_info( blockchain, password, c_default_exp, acct, &key_info ) );

   return key_info.msg_secret;
}

string create_peer_repository_entry_pull_info( const string& hash )
{
   string retval;

#ifndef SSL_SUPPORT
   throw runtime_error( "create_peer_repository_entry_pull_info requires SSL support" );
#else
   string local_hash, local_public_key, master_public_key;

   if( fetch_repository_entry_record( hash,
    local_hash, local_public_key, master_public_key, false ) && has_file( local_hash ) )
   {
      string file_data( c_file_type_str_blob );

      file_data += c_file_repository_meta_data_line_prefix;
      file_data += c_file_repository_meta_data_info_type_raw;
      file_data += '\n';

      file_data += c_file_repository_public_key_line_prefix;
      file_data += base64::encode( hex_decode( local_public_key ) );
      file_data += '\n';

      file_data += c_file_repository_source_hash_line_prefix;
      file_data += base64::encode( hex_decode( local_hash ) );
      file_data += '\n';

      file_data += c_file_repository_target_hash_line_prefix;
      file_data += base64::encode( hex_decode( hash ) );

      // NOTE: Don't allow compression to assist with interactive testing and/or debugging.
      retval = create_raw_file( file_data, false );
   }
#endif

   return retval;
}

string create_peer_repository_entry_push_info( const string& file_hash, const string& password )
{
   string retval;

#ifndef SSL_SUPPORT
   throw runtime_error( "create_peer_repository_entry_push_info requires SSL support" );
#else
   string file_data( c_file_type_str_blob );

   file_data += c_file_repository_meta_data_line_prefix;
   file_data += c_file_repository_meta_data_info_type_raw;
   file_data += '\n';

   // NOTE: The first nibble is zeroed out to ensure that the hash value is always valid to use
   // as a Bitcoin address "secret" (as the range of its EC is smaller than the full 256 bits).
   private_key priv_key( "0" + sha256( file_hash + password ).get_digest_as_string( ).substr( 1 ) );

   file_data += c_file_repository_public_key_line_prefix;
   file_data += priv_key.get_public( true, true );
   file_data += '\n';

   file_data += c_file_repository_source_hash_line_prefix;
   file_data += base64::encode( hex_decode( file_hash ) );

   // NOTE: Don't allow compression to assist with interactive testing and/or debugging.
   retval = create_raw_file( file_data, false );
#endif

   return retval;
}

void decrypt_pulled_peer_file( const string& dest_hash, const string& src_hash, const string& password )
{
#ifndef SSL_SUPPORT
   throw runtime_error( "decrypt_pulled_peer_file requires SSL support" );
#else
   string all_tags( get_hash_tags( src_hash ) );

   vector< string > tags;
   if( !all_tags.empty( ) )
   {
      split( all_tags, tags, '\n' );

      string public_key_in_hex;

      // NOTE: The source file is expected to have a tag that starts with a tilda
      // and is followed by the public key (in hex) for the ephemeral private key
      // that had been used to encrypt the content.
      for( size_t i = 0; i < tags.size( ); i++ )
      {
         string next( tags[ i ] );

         if( next.size( ) > 48 && next[ 0 ] == '~' )
         {
            public_key_in_hex = next.substr( 1 );
            break;
         }
      }

      if( public_key_in_hex.empty( ) )
         throw runtime_error( "unable to locate peer public key tag for file '" + src_hash + "'" );
      else
      {
         public_key pub_key( public_key_in_hex );

         // NOTE: The first nibble is zeroed out to ensure that the hash value is always valid to use
         // as a Bitcoin address "secret" (as the range of its EC is smaller than the full 256 bits).
         private_key priv_key( "0" + sha256( dest_hash + password ).get_digest_as_string( ).substr( 1 ) );

         string file_data( extract_file( src_hash, "" ) );

         stringstream ss( file_data );
         crypt_stream( ss, priv_key.construct_shared( pub_key ) );

         string hash( create_raw_file( string( c_file_type_str_blob ) + ss.str( ) ) );

         if( hash != dest_hash )
         {
            delete_file( hash );
            throw runtime_error( "cannot decrypt peer file (bad password or incorrect content)" );
         }
      }
   }
#endif
}

void perform_storage_rewind( const string& blockchain, uint64_t block_height )
{
   guard g( g_mutex, "perform_storage_rewind" );

   if( block_height != c_unconfirmed_revision )
   {
      chain_info cinfo;
      get_chain_info( cinfo, blockchain );

      if( block_height <= cinfo.checkpoint_start_height )
         throw runtime_error( "invalid attempt to rewind to " + to_string( block_height )
          + " through checkpoint at height " + to_string( cinfo.checkpoint_start_height ) );
   }

   map< string, string > file_info;
   storage_process_undo( block_height, file_info );

   // NOTE: Remove or extract and tag any relevant file attachments that were in the undo.
   for( map< string, string >::iterator i = file_info.begin( ); i != file_info.end( ); ++i )
   {
      string path( i->first );

      if( i->second.empty( ) )
         file_remove( path );
      else
      {
         extract_file( i->second, path );

         string::size_type pos = path.find( c_files_directory );
         if( pos == string::npos )
            throw runtime_error( "invalid path: " + path );

         string remainder( path.substr( pos + strlen( c_files_directory ) ) );

         string tag( "c" + blockchain + ".f" + storage_name( ) + replace( remainder, "/", "." ) );

         tag_file( tag, i->second );
      }
   }
}

string construct_blockchain_info_file( const string& blockchain )
{
   guard g( g_mutex, "construct_blockchain_info_file" );

   string data( c_file_type_str_core_blob );

   data += string( c_file_type_core_blockchain_info_object ) + ":c" + blockchain + ".info";

   vector< pair< string, string > > extras;

   verify_core_file( data, true, &extras );

   // NOTE: There is no need to create a raw file for the info "request"
   // so only the extras are passed for raw file creation.
   create_raw_file_with_extras( "", extras );

   return tag_file_hash( "c" + blockchain + ".info" );
}

uint64_t construct_transaction_scripts_for_blockchain(
 const string& blockchain, const string& tx_hash, vector< string >& applications )
{
   guard g( g_mutex, "construct_transaction_scripts_for_blockchain" );

   string filename( blockchain + ".txs" );

   uint64_t block_height = tx_hash.empty( ) ? 0 : c_unconfirmed_revision;

   if( !tx_hash.empty( ) || file_exists( filename ) )
   {
      set< string > txs;
      vector< string > lines;

      if( !tx_hash.empty( ) )
         lines.push_back( tx_hash );
      else
         buffer_file_lines( filename, lines );

      string block_height_marker( get_special_var_name( e_special_var_bh ) );

      bool has_new_height = false;
      map< string, vector< string > > app_log_lines;

      for( size_t i = 0; i < lines.size( ); i++ )
      {
         string next( lines[ i ] );

         if( next.length( ) < numeric_limits< uint64_t >::digits10 )
         {
            has_new_height = true;
            block_height = from_string< uint64_t >( next );
         }
         else
         {
            transaction_info tinfo;
            get_transaction_info( tinfo, next );

            if( !tx_hash.empty( ) )
            {
               app_log_lines[ tinfo.application ].push_back( "session_variable "
                + get_special_var_name( e_special_var_blockchain ) + " " + blockchain );

               app_log_lines[ tinfo.application ].push_back( "session_variable "
                + get_special_var_name( e_special_var_transaction ) + " " + tx_hash );
            }
            else if( has_new_height )
            {
               has_new_height = false;

               app_log_lines[ tinfo.application ].push_back(
                "storage_comment \"" + string( c_block_prefix ) + " " + to_string( block_height ) + "\"" );
            }

            txs.insert( next );

            string account( tinfo.account_id );
            string::size_type pos = account.find( ".a" );

            if( pos != string::npos )
               account.erase( 0, pos + 2 );

            vector< string > file_tags_and_exports;

            for( size_t j = 0; j < tinfo.log_lines.size( ); j++ )
            {
               string next_log_line( tinfo.log_lines[ j ] );

               if( next_log_line.find( c_file_type_core_transaction_special_file_extract_prefix ) == 0 )
               {
                  vector< string > parts;
                  split( next_log_line, parts, ' ' );

                  if( parts.size( ) != 3 )
                     throw runtime_error( "unexpected log line: " + next_log_line );

                  string filename( parts[ 2 ] );
                  string::size_type pos = filename.find( '-' );

                  if( pos == string::npos )
                     throw runtime_error( "unexpected filename '" + filename + "' in log line: " + next_log_line );

                  string module_and_class( filename.substr( pos + 1 ) );

                  pos = module_and_class.find( '.' );
                  if( pos != string::npos )
                     module_and_class.erase( pos );

                  pos = module_and_class.find( 'F' );
                  if( pos == string::npos )
                     throw runtime_error( "unexpected class name '" + module_and_class + "' in log line: " + next_log_line );

                  module_and_class.erase( pos );

                  pos = module_and_class.find( 'C' );
                  if( pos == string::npos )
                     throw runtime_error( "unexpected class name '" + module_and_class + "' in log line: " + next_log_line );

                  string module( module_and_class.substr( 0, pos ) );

                  next_log_line = "file_tag " + parts[ 1 ] + " c" + blockchain
                   + ".f" + tinfo.application + "." + module + "." + module_and_class + "." + filename;

                  file_tags_and_exports.push_back( next_log_line );

                  next_log_line = "storage_file_export " + parts[ 1 ];
                  next_log_line += ' ' + module + ' ' + module_and_class + ' ' + filename;

                  file_tags_and_exports.push_back( next_log_line );
               }
               else
               {
                  string::size_type pos = insert_account_into_transaction_log_line( account, next_log_line );

                  if( tx_hash.empty( ) )
                  {
                     if( next_log_line[ pos + 1 ] == '"' )
                        next_log_line.insert( pos + 1, block_height_marker + "=" + to_string( block_height ) );
                     else
                        next_log_line.insert( pos + 1, block_height_marker + "=" + to_string( block_height ) + "," );
                  }

                  app_log_lines[ tinfo.application ].push_back( next_log_line );
               }
            }

            // NOTE: The file tag and export commands are placed after the normal commands within
            // an error check conditional (so they are not executed if any normal commands fail).
            if( !file_tags_and_exports.empty( ) )
            {
               app_log_lines[ tinfo.application ].push_back( "@ifndef %ERROR%" );

               for( size_t j = 0; j < file_tags_and_exports.size( ); j++ )
                  app_log_lines[ tinfo.application ].push_back( file_tags_and_exports[ j ] );

               app_log_lines[ tinfo.application ].push_back( "@endif" );
            }
         }
      }

      map< string, vector< string > > retagged_txs;

      string retagged_txs_filename( filename + ".new" );

      // NOTE: If any retagged txs exist then buffer them
      // according to the application that they belong to.
      if( file_exists( retagged_txs_filename ) )
      {
         vector< string > new_txs;
         buffer_file_lines( retagged_txs_filename, new_txs );

         for( size_t i = 0; i < new_txs.size( ); i++ )
         {
            string next( new_txs[ i ] );

            transaction_info tinfo;
            get_transaction_info( tinfo, next );

            retagged_txs[ tinfo.application ].push_back( next );
         }

         file_remove( retagged_txs_filename );
      }

      for( map< string, vector< string > >::iterator i = app_log_lines.begin( ); i!= app_log_lines.end( ); ++i )
      {
         string filename( i->first + ".txs.cin" );
         string local_txs( i->first + ".txs.log" );

         vector< string > old_local_txs;
         vector< string > new_local_txs;

         if( retagged_txs.count( i->first ) )
         {
            for( size_t j = 0; j < retagged_txs[ i->first ].size( ); j++ )
               new_local_txs.push_back( retagged_txs[ i->first ][ j ] );
         }

         if( tx_hash.empty( ) && file_exists( local_txs ) )
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
         {
            string next( ( i->second )[ j ] );

            if( !next.empty( ) )
            {
               if( next[ 0 ] != '@' )
                  outf << '.';

               outf << next << '\n';
            }
         }

         // NOTE: If any transactions had ended up being retagged then they along
         // with any previous local txs that have not been included are reissued.
         if( !new_local_txs.empty( ) )
         {
            outf << ".storage_comment \"" + string( c_block_prefix )
             + " " + to_string( c_unconfirmed_revision ) + "\"\n";

            for( size_t j = 0; j < new_local_txs.size( ); j++ )
            {
               string next( new_local_txs[ j ] );

               transaction_info tinfo;
               get_transaction_info( tinfo, next );

               string account( tinfo.account_id );
               string::size_type pos = account.find( ".a" );

               if( pos != string::npos )
                  account.erase( 0, pos + 2 );

               for( size_t k = 0; k < tinfo.log_lines.size( ); k++ )
               {
                  string next_log_line( tinfo.log_lines[ k ] );

                  if( !next_log_line.empty( ) )
                  {
                     insert_account_into_transaction_log_line( account, next_log_line );

                     if( next_log_line[ 0 ] != '@' )
                        outf << '.';

                     outf << next_log_line << '\n';
                  }
               }
            }

            write_file_lines( i->first + ".txs.log.new", new_local_txs );
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

