// Copyright (c) 2014 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <map>
#  include <deque>
#  include <fstream>
#  include <stdexcept>
#endif

#define CIYAM_BASE_IMPL

#include "ciyam_core_files.h"

#include "regex.h"
#include "config.h"
#include "ptypes.h"
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

mutex g_mutex;

#include "ciyam_constants.h"

struct block_info
{
   block_info( ) : block_height( 0 ), block_weight( 0 ), total_weight( 0 ), had_secondary_account( false ) { }

   string mint_id;
   string mint_hash;
   string mint_lock;

   string previous_block;

   unsigned long block_height;

   uint64_t block_weight;
   uint64_t total_weight;

   bool had_secondary_account;
};

string base64_to_tag_name( const string& base64 )
{
   return replaced( base64, "/", "$", "=", "-" );
}

string tag_name_to_base64( const string& tag_name )
{
   return replaced( tag_name, "$", "/", "-", "=" );
}

pair< unsigned long, uint64_t > verify_block( const string& content,
 bool check_sigs, vector< pair< string, string > >* p_extras, block_info* p_block_info = 0 )
{
   guard g( g_mutex );

   vector< string > lines;
   split( content, lines, '\n' );

   if( lines.empty( ) )
      throw runtime_error( "unexpected empty block content" );

   unsigned long block_height = 0;
   unsigned long previous_block_height = 0;

   uint64_t block_weight = 0;
   uint64_t total_weight = 0;
   uint64_t previous_block_weight = 0;

   string chain, account, account_hash, account_lock, previous_block, public_key_base64;

   size_t account_extra_offset = 0;

   string block_signature, previous_block_link, previous_block_minter_id;

   bool previous_block_had_secondary_account = false;

   uint64_t mint_charge = 0;
   uint64_t mint_reward = 0;
   uint64_t account_charge = 0;

   uint64_t checkpoint_tolerance = 0;
   unsigned long checkpoint_length = 0;

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
            }
         }
         else if( !has_height )
         {
            if( next_attribute.find( c_file_type_core_block_header_height_prefix ) != 0 )
               throw runtime_error( "unexpected missing height attribute in block header '" + header + "'" );

            has_height = true;
            block_height = atoi( next_attribute.substr(
             string( c_file_type_core_block_header_height_prefix ).length( ) ).c_str( ) );
         }
         else if( block_height && !has_weight )
         {
            if( next_attribute.find( c_file_type_core_block_header_weight_prefix ) != 0 )
               throw runtime_error( "unexpected missing weight attribute in block header '" + header + "'" );

            has_weight = true;
            block_weight = from_string< uint64_t >( next_attribute.substr(
             string( c_file_type_core_block_header_weight_prefix ).length( ) ) );
         }
         else if( block_height && !has_account_hash )
         {
            if( next_attribute.find( c_file_type_core_block_header_account_hash_prefix ) != 0 )
               throw runtime_error( "unexpected missing account hash in block header '" + header + "'" );

            has_account_hash = true;
            account_hash = next_attribute.substr(
             string( c_file_type_core_block_header_account_hash_prefix ).length( ) );
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

                     string::size_type pos = next_meta.find( '~' );
                     if( pos != string::npos )
                     {
                        string::size_type npos = next_meta.find( '<', pos );
                        if( npos == string::npos )
                           throw runtime_error( "unexpected missing length and tolerance separator in '" + next_meta + "'" );

                        checkpoint_length = atol( next_meta.substr( pos + 1, npos - pos - 1 ).c_str( ) );
                        checkpoint_tolerance = from_string< uint64_t >( next_meta.substr( npos + 1 ) );

                        if( checkpoint_length < 2 || !checkpoint_tolerance )
                           throw runtime_error( "invalid checkpoint length and/or tolerance amounts in '" + next_meta + "'" );

                        next_meta.erase( pos );
                     }

                     pos = next_meta.find( '!' );
                     if( pos != string::npos )
                     {
                        account_charge = from_string< uint64_t >( next_meta.substr( pos + 1 ) );
                        next_meta.erase( pos );

                        if( !account_charge )
                           had_zero_explicit_account_charge = true;
                     }

                     pos = next_meta.find( '-' );
                     if( pos != string::npos )
                     {
                        mint_charge = from_string< uint64_t >( next_meta.substr( pos + 1 ) );
                        next_meta.erase( pos );
                     }

                     has_reward = true;
                     mint_reward = from_string< uint64_t >( next_meta );
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
               previous_block = next_attribute.substr( pos
                + string( c_file_type_core_block_header_previous_block_prefix ).length( ) );
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
      if( !has_file( previous_block ) )
         throw runtime_error( "previous block '" + previous_block + "' does not exist" );

      string previous_block_data( extract_file( previous_block, "", c_file_type_char_core_blob ) );

      string::size_type pos = previous_block_data.find( ':' );
      if( pos == string::npos || previous_block_data.substr( 0, pos ) != string( c_file_type_core_block_object ) )
         throw runtime_error( "invalid previous block file" );

      block_info info;
      if( p_extras && verify_block( previous_block_data.substr( pos + 1 ), false, 0, &info ).first != block_height - 1 )
         throw runtime_error( "chain height is not one above previous block height" );

      previous_block_link = info.previous_block;
      previous_block_height = info.block_height;
      previous_block_weight = info.block_weight;

      if( info.total_weight + block_weight != total_weight )
         throw runtime_error( "incorrect total weight specified in block (expecting "
          + to_string( info.total_weight + block_weight ) + " but found " + to_string( total_weight ) + ")" );
   }

   string verify( string( c_file_type_core_block_object ) + ':' + header );

   bool is_new_chain_head = false;
   string mint_address, mint_test_address, mint_address_uncompressed, mint_test_address_uncompressed;

   if( p_extras && block_height )
   {
      string block_tag( "c" + chain + ".b" + to_string( block_height ) );

      if( !has_tag( block_tag ) )
         is_new_chain_head = true;
      else
      {
         string current_block_data( extract_file( tag_file_hash( block_tag ), "", c_file_type_char_core_blob ) );

         string::size_type pos = current_block_data.find( ':' );

         if( pos == string::npos || current_block_data.substr( 0, pos ) != string( c_file_type_core_block_object ) )
            throw runtime_error( "invalid current block file" );

         block_info info;
         if( verify_block( current_block_data.substr( pos + 1 ), false, 0, &info ).second > block_weight )
         {
            is_new_chain_head = true;
            previous_block_minter_id = info.mint_id;
            previous_block_had_secondary_account = info.had_secondary_account;
         }
      }
   }

   bool had_signature = false;
   bool has_secondary_account = false;

   for( size_t i = 1; i < lines.size( ); i++ )
   {
      string next_line( lines[ i ] );

      if( next_line.size( ) < 3 )
         throw runtime_error( "unexpected line '" + next_line + "' in verify_block" );

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
         bool has_hash = false;
         bool has_lock = false;
         bool has_tx_hash = false;
         bool has_tx_lock = false;

         string id, hash, lock, tx_hash, tx_lock;

         for( size_t j = 0; j < attributes.size( ); j++ )
         {
            string next_attribute( attributes[ j ] );

            if( next_attribute.size( ) < 2 )
               throw runtime_error( "invalid account attribute '" + next_attribute + "'" );

            if( !has_id )
            {
               regex expr( "^[A-F0-9]{16}$" );

               if( expr.search( next_attribute ) != 0 )
                  throw runtime_error( "invalid account id '" + next_attribute + "'" );

               has_id = true;

               id = next_attribute;
            }
            else if( !has_hash )
            {
               size_t len = string( c_file_type_core_block_detail_account_hash_prefix ).length( );

               if( next_attribute.substr( 0, len ) != string( c_file_type_core_block_detail_account_hash_prefix ) )
                  throw runtime_error( "invalid account hash attribute '" + next_attribute + "'" );

               has_hash = true;

               hash = next_attribute.substr( len );
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
            else if( !has_tx_hash )
            {
               size_t len = string( c_file_type_core_block_detail_account_tx_hash_prefix ).length( );

               if( next_attribute.substr( 0, len ) != string( c_file_type_core_block_detail_account_tx_hash_prefix ) )
                  throw runtime_error( "invalid account tx hash attribute '" + next_attribute + "'" );

               has_tx_hash = true;

               tx_hash = next_attribute.substr( len );
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
               extra += hash + '\n' + lock;

               if( !tx_hash.empty( ) )
                  extra += '\n' + tx_hash + '\n' + tx_lock;

               string tags( "c" + chain + ".a" + id + ".h0.b" + to_string( mint_reward - mint_charge ) );
               p_extras->push_back( make_pair( extra, tags ) );
            }
            else if( !has_secondary_account )
            {
               has_secondary_account = true;

               string extra( c_file_type_str_core_blob );
               extra += hash + '\n' + lock + "\n<acct>";;

               if( !tx_hash.empty( ) )
                  extra += '\n' + tx_hash + '\n' + tx_lock;

               string tags( "c" + chain + ".a" + id + ".h0.b0" );
               p_extras->push_back( make_pair( extra, tags ) );
            }
            else
               throw runtime_error( "invalid attempt to create more than one secondary account" );
         }
      }
      else if( !had_signature
       && prefix == string( c_file_type_core_block_detail_signature_prefix ) )
      {
         had_signature = true;
         block_signature = next_line;
#ifdef SSL_SUPPORT
         public_key pkey( public_key_base64, true );

         if( block_height )
         {
            mint_address = pkey.get_address( );
            mint_test_address = pkey.get_address( true, true );

            mint_address_uncompressed = pkey.get_address( false );
            mint_test_address_uncompressed = pkey.get_address( false, true );
         }

         if( check_sigs && !pkey.verify_signature( verify, block_signature ) )
            throw runtime_error( "invalid block signature" );
#endif
      }
      else
         throw runtime_error( "unexpected line '" + lines[ i ] + "' in verify_block" );
   }

   if( p_extras && !had_signature )
      throw runtime_error( "block signature missing" );

   if( block_height && p_block_info )
   {
      p_block_info->mint_id = "c" + chain + ".a" + account;

      p_block_info->mint_hash = account_hash;
      p_block_info->mint_lock = account_lock;

      p_block_info->previous_block = previous_block;

      p_block_info->block_height = block_height;
      p_block_info->block_weight = block_weight;
      p_block_info->total_weight = total_weight;

      p_block_info->had_secondary_account = has_secondary_account;
   }

   if( p_extras )
   {
      string raw_block_data( c_file_type_str_core_blob );
      raw_block_data += verify;

      string block_file_tag( "c" + chain + ".b" + to_string( block_height ) );

      string block_file_with_weight( block_file_tag + "-" + to_string( block_weight ) );
      string block_file_with_weight_and_sig_tag( block_file_with_weight + ".s" + base64_to_tag_name( block_signature ) );

      string tags( block_file_with_weight_and_sig_tag );

      if( !block_height || is_new_chain_head )
         tags += "\n" + block_file_tag + "\nc" + chain + ".head";

      p_extras->push_back( make_pair( raw_block_data, tags ) );

      if( !block_height )
      {
         string extra( c_file_type_str_core_blob );
         extra += to_string( mint_reward );

         if( mint_charge )
            extra += '-' + to_string( mint_charge );

         if( account_charge || had_zero_explicit_account_charge )
            extra += '!' + to_string( account_charge );

         if( checkpoint_tolerance )
            extra += '~' + to_string( checkpoint_length ) + '<' + to_string( checkpoint_tolerance );

         string chain_account_tag( "c" + chain + ".a" + account );
         p_extras->push_back( make_pair( extra, chain_account_tag ) );
      }
      else
      {
         string chain_account_tag( "c" + chain + ".a" + chain );

         if( !has_tag( chain_account_tag ) )
            throw runtime_error( "unable to find root chain account for '" + chain_account_tag + "'" );

         string chain_account_hash( tag_file_hash( chain_account_tag ) );
         string chain_account_info( extract_file( chain_account_hash, "", c_file_type_char_core_blob ) );

         vector< string > chain_account_items;
         split( chain_account_info, chain_account_items, '\n' );

         if( chain_account_items.size( ) < 1 )
            throw runtime_error( "unexpected invalid chain_account_info '" + chain_account_info + "'" );

         string requisites( chain_account_items[ 0 ] );

         string::size_type pos = requisites.find( '~' );
         if( pos != string::npos )
         {
            string::size_type npos = requisites.find( '<', pos );
            if( npos == string::npos )
               throw runtime_error( "unexpected missing length and tolerance separator in '" + requisites + "'" );

            checkpoint_length = atol( requisites.substr( pos + 1, npos - pos - 1 ).c_str( ) );
            checkpoint_tolerance = from_string< uint64_t >( requisites.substr( npos + 1 ) );

            requisites.erase( pos );
         }

         pos = requisites.find( '!' );
         if( pos != string::npos )
         {
            account_charge = from_string< uint64_t >( requisites.substr( pos + 1 ) );
            requisites.erase( pos );

            // NOTE: If the account charge is explicitly provided with a zero value
            // then this will determine that the blockchain will not allow creation
            // of secondary accounts.
            if( account_charge == 0 && has_secondary_account )
               throw runtime_error( "secondary account creation not permitted" );
         }

         pos = requisites.find( '-' );
         if( pos != string::npos )
         {
            mint_charge = from_string< uint64_t >( requisites.substr( pos + 1 ) );
            requisites.erase( pos );
         }

         mint_reward = from_string< uint64_t >( requisites );

         string tags, extra;

         if( !previous_block_minter_id.empty( ) && !previous_block_had_secondary_account )
         {
            string prior_block_minter_tag( list_file_tags( previous_block_minter_id + "*" ) );

            string prior_block_minter_hash( tag_file_hash( prior_block_minter_tag ) );
            string prior_block_minter_data( extract_file( prior_block_minter_hash, "", c_file_type_char_core_blob ) );

            vector< string > prior_block_minter_items;
            split( prior_block_minter_data, prior_block_minter_items, '\n' );

            if( prior_block_minter_items.size( ) < 2 )
               throw runtime_error( "unexpected invalid prior_block_minter_data '" + prior_block_minter_data + "'" );

            string::size_type pos = prior_block_minter_tag.find( ".b" );
            if( pos == string::npos )
               throw runtime_error( "unable to find account balance in '" + prior_block_minter_tag + "'" );

            uint64_t previous_balance = from_string< uint64_t >( prior_block_minter_tag.substr( pos + 2 ) );

            if( previous_balance < mint_reward )
               previous_balance = 0;
            else
               previous_balance -= mint_reward;

            extra = string( c_file_type_str_core_blob );
            extra += prior_block_minter_items[ 0 ];

            for( size_t i = 1; i < prior_block_minter_items.size( ); i++ )
               extra += '\n' + prior_block_minter_items[ i ];

            p_extras->push_back( make_pair( extra, previous_block_minter_id
             + ".h" + to_string( block_height ) + ".b*" + to_string( previous_balance ) ) );
         }

         string minter_account( "c" + chain + ".a" + account );
         string minter_account_tag( list_file_tags( minter_account + ".h*" ) );

         pos = minter_account_tag.find( ".h" );
         string::size_type rpos = minter_account_tag.find( ".b" );

         if( pos == string::npos || rpos == string::npos || rpos < pos )
            throw runtime_error( "unexpected minter_account_tag '" + minter_account_tag + "'" );

         unsigned long last_height = atol( minter_account_tag.substr( pos + 2, rpos - pos - 2 ).c_str( ) );

         // NOTE: If an account has already minted then make sure that this block is higher than
         // the previous one minted.
         if( block_height <= last_height )
            throw runtime_error( "invalid block height for minting account" );

         string minter_account_hash( tag_file_hash( minter_account_tag ) );
         string minter_account_data( extract_file( minter_account_hash, "", c_file_type_char_core_blob ) );

         vector< string > minter_account_items;
         split( minter_account_data, minter_account_items, '\n' );

         if( minter_account_items.size( ) < 2 )
            throw runtime_error( "unexpected invalid minter_account_data '" + minter_account_data + "'" );

         string previous_hash( minter_account_items[ 0 ] );
         string previous_lock( minter_account_items[ 1 ] );

         if( !check_if_valid_hash_pair( account_hash, previous_hash, true ) )
            throw runtime_error( "invalid hash from minter" );

         if( previous_lock != mint_address && previous_lock != mint_address_uncompressed
          && previous_lock != mint_test_address && previous_lock != mint_test_address_uncompressed )
            throw runtime_error( "invalid public key from minter" );

         pos = minter_account_tag.find( ".b" );
         if( pos == string::npos )
            throw runtime_error( "unable to find account balance in '" + minter_account_tag + "'" );

         uint64_t balance = from_string< uint64_t >( minter_account_tag.substr( pos + 2 ) );

         if( balance < mint_charge )
            throw runtime_error( "unsufficient balance to mint" );

         if( has_secondary_account && balance < account_charge )
            throw runtime_error( "unsufficient balance to create an account" );

         if( has_secondary_account )
         {
            if( !account_charge )
               balance = 0;
            else
               balance -= account_charge;
         }
         else
            balance -= mint_charge;

         pos = minter_account_tag.find( ".h" );
         if( pos != string::npos )
            minter_account_tag.erase( pos );

         if( is_new_chain_head && !has_secondary_account )
            balance += mint_reward;

         extra = string( c_file_type_str_core_blob );
         extra += account_hash + '\n' + account_lock;

         for( size_t i = 2; i < minter_account_items.size( ); i++ )
            extra += '\n' + minter_account_items[ i ];

         tags += "\n" + minter_account_tag + ".h*" + to_string( block_height ) + ".b" + to_string( balance );

         account_extra_offset = p_extras->size( );
         p_extras->push_back( make_pair( extra, tags ) );

         // NOTE: The previous account blob instance will be removed.
         p_extras->push_back( make_pair( "", minter_account_hash ) );
      }

      // NOTE: Determine whether or not a new checkpoint has occurred.
      if( checkpoint_length && block_height > checkpoint_length * 2 )
      {
         size_t num_found = 0;

         string checkpoint_hash;
         uint64_t checkpoint_weight = 0;
         unsigned long checkpoint_height = 0;

         set< string > block_links;
         deque< string > checkpoint_blocks;

         // NOTE: If the checkpoint length is say three then blocks to be
         // checked start from three blocks prior (back to six blocks) to
         // make it less likely that nodes would checkpoint when a re-org
         // was about to appear. Therefore larger checkpoint lengths will
         // create less forks than shorter ones. Another important factor
         // is the checkpoint tolerance which is used to decide if or not
         // the weight is acceptable for checkpointing (the greater value
         // used then the more likely forks are to occur).
         for( size_t i = 0; i < checkpoint_length * 2; i++ )
         {
            if( previous_block.empty( ) || !has_file( previous_block ) )
               break;

            if( i >= checkpoint_length
             && previous_block_weight >= checkpoint_tolerance )
               break;

            if( i >= checkpoint_length )
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

            if( previous_block_link.empty( ) || !has_file( previous_block_link ) )
               break;

            string previous_block_data( extract_file( previous_block_link, "", c_file_type_char_core_blob ) );

            string::size_type pos = previous_block_data.find( ':' );
            if( pos == string::npos || previous_block_data.substr( 0, pos ) != string( c_file_type_core_block_object ) )
               throw runtime_error( "invalid previous cchain file" );

            block_info info;
            verify_block( previous_block_data.substr( pos + 1 ), false, 0, &info );

            previous_block = previous_block_link;
            previous_block_link = info.previous_block;
            previous_block_height = info.block_height;
            previous_block_weight = info.block_weight;
         }

         if( num_found >= checkpoint_length )
         {
            map< string, pair< int, int > > account_mint_info;

            unsigned long last_height = block_height - 1;
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

                  string last_block_data( extract_file( last_block_hash, "", c_file_type_char_core_blob ) );

                  string::size_type pos = last_block_data.find( ':' );
                  if( pos == string::npos || last_block_data.substr( 0, pos ) != string( c_file_type_core_block_object ) )
                     throw runtime_error( "invalid last block file" );

                  block_info info;
                  verify_block( last_block_data.substr( pos + 1 ), false, 0, &info );

                  if( block_links.count( last_block_hash ) )
                     ++account_mint_info[ info.mint_id ].first;
                  else
                     ++account_mint_info[ info.mint_id ].second;
               }

               --last_height;
            }

            string all_accounts( list_file_tags( "c" + chain + ".a*" ) );

            vector< string > accounts;
            split( all_accounts, accounts, '\n' );

            // NOTE: All accounts have their balance set to the default (apart from
            // the current minter and those minters of blocks that occurred between
            // the blockchain blockchain head and the new checkpoint).
            for( size_t i = 0; i < accounts.size( ); i++ )
            {
               string next_account_tag( accounts[ i ] );

               string::size_type pos = next_account_tag.find( ".b" );
               string::size_type hpos = next_account_tag.find( ".h" );

               if( pos == string::npos || hpos == string::npos )
                  continue;

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

                  int num_consensus = account_mint_info[ "c" + chain + ".a" + account ].first;
                  int num_non_consensus = account_mint_info[ "c" + chain + ".a" + account ].second;

                  uint64_t balance = ( mint_reward - mint_charge ) * 2;

                  balance += ( mint_reward - mint_charge ) * num_consensus;

                  if( mint_charge * num_non_consensus > balance )
                     balance = 0;
                  else
                     balance -= ( mint_charge * num_non_consensus );

                  account_tags += "c" + chain + ".a" + account
                   + ".h*" + to_string( block_height ) + ".b" + to_string( balance );

                  ( *p_extras )[ account_extra_offset ].second = account_tags;
               }
               else
               {
                  string account_file_hash( tag_file_hash( next_account_tag ) );

                  next_account_tag.erase( pos );

                  int num_consensus = account_mint_info[ next_account_tag.substr( 0, hpos ) ].first;
                  int num_non_consensus = account_mint_info[ next_account_tag.substr( 0, hpos ) ].second;

                  uint64_t balance = ( mint_reward - mint_charge );

                  balance += ( mint_reward - mint_charge ) * num_consensus;

                  if( mint_charge * num_non_consensus > balance )
                     balance = 0;
                  else
                     balance -= ( mint_charge * num_non_consensus );

                  next_account_tag += ".b*" + to_string( balance );

                  p_extras->push_back( make_pair( account_file_hash, next_account_tag ) );
              }
            }

            // NOTE: Add checkpoint files.
            string checkpoint_block_data( c_file_type_str_core_blob );
            checkpoint_block_data += string( c_file_type_core_checkpoint_blocks_object ) + ':';

            for( size_t i = 0; i < checkpoint_blocks.size( ); i++ )
            {
               if( i > 0 )
                  checkpoint_block_data += '\n';
               checkpoint_block_data += checkpoint_blocks[ i ];
            }

            p_extras->push_back( make_pair( checkpoint_block_data, "c" + chain + ".checkpoint.h"
             + to_string( checkpoint_height ) + ".w" + to_string( checkpoint_weight ) + ".blocks" ) );

            string checkpoint_transaction_data( c_file_type_str_core_blob );
            checkpoint_transaction_data += string( c_file_type_core_checkpoint_transactions_object ) + ':';

            checkpoint_transaction_data += "@" + to_string( p_extras->size( ) );

            // FUTURE: Transactions need to be appended here...

            p_extras->push_back( make_pair( checkpoint_transaction_data, "c" + chain + ".checkpoint.h"
             + to_string( checkpoint_height ) + ".w" + to_string( checkpoint_weight ) + ".transactions" ) );

            // NOTE: Remove all block blobs that are at or below the new checkpoint height.
            while( true )
            {
               string all_block_tags( list_file_tags( "c" + chain + ".b" + to_string( checkpoint_height ) + "-*" ) );

               if( all_block_tags.empty( ) )
                  break;

               vector< string > block_tags;
               split( all_block_tags, block_tags, '\n' );

               for( size_t i = 0; i < block_tags.size( ); i++ )
                  p_extras->push_back( make_pair( "", tag_file_hash( block_tags[ i ] ) ) );

               if( checkpoint_height == 0 )
                  break;

               --checkpoint_height;
            }
         }
      }
   }

   return make_pair( block_height, block_weight );
}

void verify_rewind( const string& content, vector< pair< string, string > >* p_extras )
{
   guard g( g_mutex );

   if( !p_extras )
      throw runtime_error( "unexpected null extras in verify_rewind" );

   string::size_type pos = content.find( '.' );
   if( pos == 0 || pos == string::npos )
      throw runtime_error( "invalid rewind content '" + content + "'" );

   string block_hash( tag_file_hash( content ) );
   string destination_block( extract_file( block_hash, "", c_file_type_char_core_blob ) );

   string chain( content.substr( 0, pos ) );
   string chain_id( chain.substr( 1 ) );

   string chain_tag( chain + ".head" );
   string chain_hash( tag_file_hash( chain_tag ) );
   string chain_head_block_data( extract_file( chain_hash, "", c_file_type_char_core_blob ) );

   string restore_data( c_file_type_str_core_blob );
   restore_data += chain_hash + '=' + chain_tag;

   pos = chain_head_block_data.find( ':' );

   if( pos == string::npos || chain_head_block_data.substr( 0, pos ) != string( c_file_type_core_block_object ) )
      throw runtime_error( "invalid chain head block file" );

   block_info info;
   verify_block( chain_head_block_data.substr( pos + 1 ), false, 0, &info );

   string chain_account_hash( tag_file_hash( chain + ".a" + chain_id ) );
   string chain_account_info( extract_file( chain_account_hash, "", c_file_type_char_core_blob ) );

   vector< string > chain_account_lines;
   split( chain_account_info, chain_account_lines, '\n' );

   if( !chain_account_lines.size( ) )
      throw runtime_error( "unexpected chain_account_info '" + chain_account_info + "' in verify_rewind" );

   string reward_and_return( chain_account_lines[ 0 ] );

   uint64_t mint_charge = 0;
   uint64_t mint_reward = 0;
   uint64_t block_reward = 0;

   pos = reward_and_return.find( '~' );
   if( pos != string::npos )
      reward_and_return.erase( pos );

   pos = reward_and_return.find( '!' );
   if( pos != string::npos )
      reward_and_return.erase( pos );

   pos = reward_and_return.find( '-' );
   if( pos != string::npos )
   {
      mint_charge = from_string< uint64_t >( reward_and_return.substr( pos + 1 ) );
      reward_and_return.erase( pos );
   }

   mint_reward = from_string< uint64_t >( reward_and_return );

   block_reward = mint_reward - mint_charge;

   set< string > block_minter_hashes;

   while( true )
   {
      string block_minter_tag( list_file_tags( info.mint_id + "*" ) );
      string block_minter_hash( tag_file_hash( block_minter_tag ) );

      if( !block_minter_hashes.count( block_minter_hash ) )
      {
         block_minter_hashes.insert( block_minter_hash );
         restore_data += '\n' + block_minter_hash + '=' + block_minter_tag;
      }

      pos = block_minter_tag.find( ".b" );
      if( pos == string::npos )
         throw runtime_error( "unexptected block_minter_tag '" + block_minter_tag  + "' in verify_rewind" );

      uint64_t previous_balance = from_string< uint64_t >( block_minter_tag.substr( pos + 2 ) );

      if( !info.had_secondary_account )
      {
         if( previous_balance > block_reward )
            previous_balance -= block_reward;

         string new_account_blob( c_file_type_str_core_blob );
         new_account_blob += extract_file( block_minter_hash, "", c_file_type_char_core_blob );

         pos = block_minter_tag.find( ".b" );
         if( pos == string::npos )
            throw runtime_error( "unexptected block_minter_tag '" + block_minter_tag  + "' in verify_rewind" );

         block_minter_tag.erase( pos );

         p_extras->push_back( make_pair( new_account_blob,
          block_minter_tag + ".b*" + to_string( previous_balance ) ) );
      }

      if( info.previous_block == block_hash )
         break;

      string next_block_data( extract_file( info.previous_block, "", c_file_type_char_core_blob ) );
      pos = next_block_data.find( ':' );

      if( pos == string::npos || next_block_data.substr( 0, pos ) != string( c_file_type_core_block_object ) )
         throw runtime_error( "invalid next_block_data in verify_rewind" );

      block_info new_info;
      if( verify_block( next_block_data.substr( pos + 1 ), false, 0, &new_info ).first != info.block_height - 1 )
         throw runtime_error( "chain height is not one below current cchain height in verify_rewind" );

      info = new_info;
   }

   string new_head( c_file_type_str_core_blob );
   new_head += destination_block;

   p_extras->push_back( make_pair( new_head, chain + ".head" ) );
   p_extras->push_back( make_pair( restore_data, chain + ".head.restore" ) );
}

void verify_restore( const string& content, vector< pair< string, string > >* p_extras )
{
   guard g( g_mutex );

   if( !p_extras )
      throw runtime_error( "unexpected null extras in verify_restore" );

   string restore_data( extract_file( content, "", c_file_type_char_core_blob ) );

   vector< string > lines;
   split( restore_data, lines, '\n' );

   if( !lines.size( ) )
      throw runtime_error( "unexpected missing restore_data for verify_restore" );

   for( size_t i = 0; i < lines.size( ); i++ )
   {
      string next_line( lines[ i ] );

      string::size_type pos = next_line.find( '=' );
      if( pos == string::npos )
         throw runtime_error( "unexpected next_line '" + next_line + "' in verify_restore" );

      string data( c_file_type_str_core_blob );
      data += extract_file( next_line.substr( 0, pos ), "", c_file_type_char_core_blob );

      string tag( next_line.substr( pos + 1 ) );

      if( i > 0 )
      {
         pos = tag.find( ".b" );
         if( pos == string::npos )
            throw runtime_error( "unexpected account tag '" + tag + "' in verify_restore" );

         tag.insert( pos + 2, "*" );
      }

      p_extras->push_back( make_pair( data, tag ) );
   }
}

void verify_transaction( const string& content, bool check_sigs, vector< pair< string, string > >* p_extras )
{
   guard g( g_mutex );

   vector< string > lines;
   split( content, lines, '\n' );

   if( lines.empty( ) )
      throw runtime_error( "unexpected empty transaction content" );

   string chain, account, application, transaction_hash, transaction_lock, previous_transaction, public_key_base64;

   string transaction_signature;

   unsigned long transaction_number = 0;

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
      bool has_tnumber = false;
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
         }
         else if( !has_tnumber )
         {
            if( next_attribute.find( c_file_type_core_transaction_header_tnumber_prefix ) != 0 )
               throw runtime_error( "unexpected missing transaction number attribute in transaction header '" + header + "'" );

            has_tnumber = true;
            transaction_number = atoi( next_attribute.substr(
             string( c_file_type_core_transaction_header_tnumber_prefix ).length( ) ).c_str( ) );
         }
         else if( !has_application )
         {
            if( next_attribute.find( c_file_type_core_transaction_header_application_prefix ) != 0 )
               throw runtime_error( "unexpected missing application attribute in transaction header '" + header + "'" );

            has_application = true;
            application = next_attribute.substr(
             string( c_file_type_core_transaction_header_application_prefix ).length( ) );
         }
         else if( transaction_number && !has_previous_transaction )
         {
            if( next_attribute.find( c_file_type_core_transaction_header_previous_tchain_prefix ) != 0 )
               throw runtime_error( "unexpected missing previous transaction attribute in transaction header '" + header + "'" );

            has_previous_transaction = true;
            previous_transaction = next_attribute.substr(
             string( c_file_type_core_transaction_header_previous_tchain_prefix ).length( ) );
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
         else if( !has_public_key )
         {
            if( next_attribute.find( c_file_type_core_transaction_header_public_key_prefix ) != 0 )
               throw runtime_error( "unexpected missing public key attribute in transaction header '" + header + "'" );

            has_public_key = true;

            public_key_base64 = next_attribute.substr(
             string( c_file_type_core_transaction_header_public_key_prefix ).length( ) );
         }
         else
            throw runtime_error( "unexpected extra attribute '" + next_attribute + "' in transaction header" );
      }

      if( !has_public_key )
         throw runtime_error( "invalid incompleted transaction header '" + header + "'" );
   }

   string verify( string( c_file_type_core_transaction_object ) + ':' + header );

   string transaction_address, transaction_test_address,
    transaction_address_uncompressed, transaction_test_address_uncompressed;

   bool had_signature = false;

   for( size_t i = 1; i < lines.size( ); i++ )
   {
      string next_line( lines[ i ] );

      if( next_line.size( ) < 3 )
         throw runtime_error( "unexpected line '" + next_line + "' in verify_transaction" );

      string prefix( next_line.substr( 0, 2 ) );
      next_line.erase( 0, 2 );

      if( prefix == string( c_file_type_core_transaction_detail_log_prefix ) )
      {
         verify += "\n" + lines[ i ];

         // FUTURE: Need to validate the log file line formatting here...
      }
      else if( !had_signature
       && prefix == string( c_file_type_core_transaction_detail_signature_prefix ) )
      {
         had_signature = true;
         transaction_signature = next_line;
#ifdef SSL_SUPPORT
         public_key pkey( public_key_base64, true );

         transaction_address = pkey.get_address( );
         transaction_test_address = pkey.get_address( true, true );

         transaction_address_uncompressed = pkey.get_address( false );
         transaction_test_address_uncompressed = pkey.get_address( false, true );

         if( check_sigs && !pkey.verify_signature( verify, transaction_signature ) )
            throw runtime_error( "invalid transaction signature" );
#endif
      }
      else
         throw runtime_error( "unexpected line '" + lines[ i ] + "' in verify_block" );
   }

   if( p_extras && !had_signature )
      throw runtime_error( "transaction signature missing" );

   if( p_extras )
   {
      string raw_transaction_data( c_file_type_str_core_blob );
      raw_transaction_data += verify;

      string transaction_file_tag( "c" + chain + ".a" + account + ".t" + to_string( transaction_number ) );

      transaction_file_tag += ".s" + base64_to_tag_name( transaction_signature );

      p_extras->push_back( make_pair( raw_transaction_data, transaction_file_tag ) );

      string transaction_account( "c" + chain + ".a" + account );
      string transaction_account_tag( list_file_tags( transaction_account + ".h*" ) );

      string::size_type pos = transaction_account_tag.find( ".h" );
      string::size_type rpos = transaction_account_tag.find( ".b" );

      if( pos == string::npos || rpos == string::npos || rpos < pos )
         throw runtime_error( "unexpected transaction_account_tag '" + transaction_account_tag + "'" );

      string transaction_account_hash( tag_file_hash( transaction_account_tag ) );
      string transaction_account_data( extract_file( transaction_account_hash, "", c_file_type_char_core_blob ) );

      vector< string > transaction_account_items;
      split( transaction_account_data, transaction_account_items, '\n' );

      if( transaction_account_items.size( ) < 5 )
         throw runtime_error( "unexpected invalid transaction_account_data '" + transaction_account_data + "'" );

      string previous_hash( transaction_account_items[ 3 ] );
      string previous_lock( transaction_account_items[ 4 ] );

      if( !check_if_valid_hash_pair( transaction_hash, previous_hash, true ) )
         throw runtime_error( "invalid hash in transaction" );

      if( previous_lock != transaction_address && previous_lock != transaction_address_uncompressed
       && previous_lock != transaction_test_address && previous_lock != transaction_test_address_uncompressed )
         throw runtime_error( "invalid public key in transaction" );

      string extra( c_file_type_str_core_blob );

      extra += transaction_account_items[ 0 ]
       + '\n' + transaction_account_items[ 1 ] + '\n' + transaction_account_hash;

      extra += '\n' + transaction_hash + '\n' + transaction_lock;

      for( size_t i = 5; i < transaction_account_items.size( ); i++ )
         extra += '\n' + transaction_account_items[ i ];

      p_extras->push_back( make_pair( extra, transaction_account_tag ) );

      if( !previous_transaction.empty( ) && !has_file( previous_transaction ) )
         throw runtime_error( "previous transaction '" + previous_transaction + "' does not exist" );

      if( !previous_transaction.empty( ) )
      {
         string previous_transaction_tag( get_hash_tags( previous_transaction ) );

         if( previous_transaction_tag.empty( ) )
            throw runtime_error( "previous transaction '" + previous_transaction + "' tag was not found" );

         pos = previous_transaction_tag.find( ".t" );
         if( pos == string::npos )
            throw runtime_error( "invalid previous transaction '" + previous_transaction + "' tag '" + previous_transaction_tag + "'" );

         unsigned long previous_transaction_number = atol( previous_transaction_tag.substr( pos + 2 ).c_str( ) );

         if( transaction_number != previous_transaction_number + 1 )
            throw runtime_error( "expecting transaction number "
             + to_string( previous_transaction_number + 1 ) + " but found transaction number " + to_string( transaction_number ) );
      }
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
         else if( type == string( c_file_type_core_restore_object ) )
            verify_restore( content.substr( pos + 1 ), p_extras );
         else if( type == string( c_file_type_core_transaction_object ) )
            verify_transaction( content.substr( pos + 1 ), check_sigs, p_extras );
         else
            throw runtime_error( "unknown type '" + type + "' for core file" );
      }
      else
         throw runtime_error( "unable to verify non-blob core files" );
   }
}

