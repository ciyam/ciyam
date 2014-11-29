// Copyright (c) 2014 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
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
   block_info( ) : block_height( 0 ), block_weight( 0 ), total_weight( 0 ) { }

   string mint_id;
   string mint_hash;
   string mint_lock;

   string achain_block;
   string cchain_block;

   unsigned long block_height;
   unsigned long block_weight;
   unsigned long total_weight;
};

string base64_to_tag_name( const string& base64 )
{
   return replaced( base64, "/", "$", "=", "-" );
}

string tag_name_to_base64( const string& tag_name )
{
   return replaced( tag_name, "$", "/", "-", "=" );
}

pair< unsigned long, unsigned long > verify_block( const string& content,
 bool check_sigs, vector< pair< string, string > >* p_extras, block_info* p_block_info = 0 )
{
   guard g( g_mutex );

   vector< string > lines;
   split( content, lines, '\n' );

   if( lines.empty( ) )
      throw runtime_error( "unexpected empty block content" );

   unsigned long block_height = 0;
   unsigned long block_weight = 0;
   unsigned long total_weight = 0;

   string chain, account, account_hash, account_lock, previous_achain, previous_cchain, public_key_base64;

   string block_signature, prior_block_minter_id;

   uint64_t unit_return = 0;
   uint64_t unit_reward = 0;

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
      bool has_previous_achain = false;
      bool has_previous_cchain = false;

      for( size_t i = 0; i < attributes.size( ); i++ )
      {
         string next_attribute( attributes[ i ] );
         if( next_attribute.empty( ) )
            throw runtime_error( "unexpected empty attribute in block header '" + header + "'" );

         string::size_type pos = string::npos;

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
            block_weight = atoi( next_attribute.substr(
             string( c_file_type_core_block_header_weight_prefix ).length( ) ).c_str( ) );
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
         else if( !has_previous_achain )
         {
            if( !block_height )
            {
               if( next_attribute.find( c_file_type_core_block_header_chain_meta_prefix ) != 0 )
                  throw runtime_error( "unexpected missing chain_meta attribute in block header '" + header + "'" );

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
                     if( next_meta.find( c_file_type_core_block_header_chain_meta_reward_prefix ) != 0 )
                        throw runtime_error( "unexpected missing reward meta item in block header '" + header + "'" );

                     next_meta.erase( 0, string( c_file_type_core_block_header_chain_meta_reward_prefix ).length( ) );

                     string::size_type pos = next_meta.find( '-' );
                     if( pos != string::npos )
                     {
                        unit_return = from_string< uint64_t >( next_meta.substr( pos + 1 ) );
                        next_meta.erase( pos );
                     }

                     has_reward = true;
                     unit_reward = from_string< uint64_t >( next_meta );
                  }
                  else
                     throw runtime_error( "unexpected chain_meta item '" + next_meta + "' in block header '" + header + "'" );
               }

               if( !has_reward )
                  throw runtime_error( "unexpected missing reward meta item in block header '" + header + "'" );

               has_previous_achain = true;
               has_previous_cchain = true;
            }
            else
            {
               pos = next_attribute.find( c_file_type_core_block_header_previous_achain_prefix );
               if( pos != 0 )
               {
                  pos = next_attribute.find( c_file_type_core_block_header_previous_cchain_prefix );
                  if( pos == 0 )
                  {
                     has_previous_achain = true;

                     --i;
                     continue;
                  }
                  else
                     throw runtime_error( "unxpected missing previous_achain/previous_cchain item in block header '" + header + "'" );
               }

               has_previous_achain = true;
               previous_achain = next_attribute.substr( pos
                + string( c_file_type_core_block_header_previous_achain_prefix ).length( ) );
            }
         }
         else if( !has_previous_cchain )
         {
            pos = next_attribute.find( c_file_type_core_block_header_previous_cchain_prefix );
            if( pos != 0 )
            {
               if( !previous_achain.empty( ) )
               {
                  previous_cchain = previous_achain;
                  has_previous_cchain = true;

                  --i;
                  continue;
               }
               else
                  throw runtime_error( "unexpected missing previous_cchain item in block header '" + header + "'" );
            }

            has_previous_cchain = true;
            previous_cchain = next_attribute.substr( pos
             + string( c_file_type_core_block_header_previous_cchain_prefix ).length( ) );
         }
         else if( !has_public_key )
         {
            pos = next_attribute.find( c_file_type_core_block_header_public_key_prefix );
            if( pos != 0 )
               throw runtime_error( "unexpected missing public_key attribute in block header '" + header + "'" );

            has_public_key = true;
            public_key_base64 = next_attribute.substr( pos
             + string( c_file_type_core_block_header_public_key_prefix ).length( ) );
         }
         else if( !has_total_weight )
         {
            pos = next_attribute.find( c_file_type_core_block_header_total_weight_prefix );
            if( pos != 0 )
               throw runtime_error( "unexpected missing total weight attribute in block header '" + header + "'" );

            has_total_weight = true;
            total_weight = atoi( next_attribute.substr( pos
             + string( c_file_type_core_block_header_total_weight_prefix ).length( ) ).c_str( ) );
         }
         else
            throw runtime_error( "unexpected extra attribute '" + next_attribute + "' in block header" );
      }

      if( block_height && !has_tag( "c" + chain + ".a" + account + "*" ) )
         throw runtime_error( "unknown account '" + account + "' for block header" );

      if( block_height == 0 && !get_is_known_blockchain( account ) )
         throw runtime_error( "invalid unrecognised blockchain '" + account + "'" );
   }

   if( !previous_achain.empty( ) && !has_file( previous_achain ) )
      throw runtime_error( "previous achain block '" + previous_achain + "' does not exist" );

   if( p_extras && block_height )
   {
      if( !has_file( previous_cchain ) )
         throw runtime_error( "previous chain '" + previous_cchain + "' does not exist" );

      string previous_cchain_data( extract_file( previous_cchain, "", c_file_type_char_core_blob ) );

      string::size_type pos = previous_cchain_data.find( ':' );
      if( pos == string::npos || previous_cchain_data.substr( 0, pos ) != string( c_file_type_core_block_object ) )
         throw runtime_error( "invalid previous cchain file" );

      block_info info;
      if( p_extras && verify_block( previous_cchain_data.substr( pos + 1 ), false, 0, &info ).first != block_height - 1 )
         throw runtime_error( "chain height is not one above previous cchain height" );

      if( info.total_weight + block_weight != total_weight )
         throw runtime_error( "incorrect total weight specified in block (expecting "
          + to_string( info.total_weight + block_weight ) + " but found " + to_string( total_weight ) + ")" );
   }

   string verify( string( c_file_type_core_block_object ) + ':' + header );

   string tree_info;

   bool is_new_chain_head = false;
   string mint_address, mint_test_address, mint_address_uncompressed, mint_test_address_uncompressed;

   if( p_extras && block_height )
   {
      string block_tag( "c" + chain + ".b" + to_string( block_height ) );

      if( !has_tag( block_tag ) )
         is_new_chain_head = true;
      else
      {
         string current_block_tree( extract_file( tag_file_hash( block_tag ), "" ) );

         vector< string > hashes;
         split( current_block_tree, hashes, '\n' );

         if( hashes.empty( ) || !has_file( hashes[ 0 ] ) )
            throw runtime_error( "unexpected invalid current block tree" );

         string current_block_data( extract_file( hashes[ 0 ], "" ) );
         string::size_type pos = current_block_data.find( ':' );

         if( pos == string::npos || current_block_data.substr( 0, pos ) != string( c_file_type_core_block_object ) )
            throw runtime_error( "invalid current block file" );

         block_info info;
         if( verify_block( current_block_data.substr( pos + 1 ), false, 0, &info ).second > block_weight )
         {
            is_new_chain_head = true;
            prior_block_minter_id = info.mint_id;
         }
      }
   }

   bool had_signature = false;

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

         string id, hash, lock;

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
                  throw runtime_error( "invalid account next address attribute '" + next_attribute + "'" );

               regex expr( c_regex_bitcoin_address );

               next_attribute.erase( 0, len );

               if( expr.search( next_attribute ) != 0 )
                  throw runtime_error( "invalid account next address '" + next_attribute + "'" );

               has_lock = true;
               lock = next_attribute;
            }
            else
               throw runtime_error( "unexpected account attribute '" + next_attribute + "'" );
         }

         if( !has_lock )
            throw runtime_error( "unexpected incomplete account information '" + next_line + "'" );

         if( p_extras )
         {
            if( !block_height )
            {
               string extra( c_file_type_str_core_blob );
               extra += hash + '\n' + lock + "\n0";

               string tags( "c" + chain + ".a" + id + ".b" + to_string( unit_reward - unit_return ) );
               p_extras->push_back( make_pair( extra, tags ) );
            }
            else
               throw runtime_error( "account extra for non-root block nyi" );

            tree_info += "\n@" + to_string( p_extras->size( ) );
         }
      }
      else if( prefix == string( c_file_type_core_block_detail_signature_prefix ) )
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

      p_block_info->achain_block = previous_achain;
      p_block_info->cchain_block = previous_cchain;

      p_block_info->block_height = block_height;
      p_block_info->block_weight = block_weight;
      p_block_info->total_weight = total_weight;
   }

   if( p_extras )
   {
      string raw_block_data( c_file_type_str_core_blob );
      raw_block_data += verify;

      p_extras->push_back( make_pair( raw_block_data, "" ) );
      tree_info = string( c_file_type_str_core_tree ) + "@" + to_string( p_extras->size( ) ) + tree_info;

      if( !block_height )
      {
         string extra( c_file_type_str_core_blob );
         extra += to_string( unit_reward );

         if( unit_return )
            extra += "-" + to_string( unit_return );

         string chain_account_tag( "c" + chain + ".a" + account );
         p_extras->push_back( make_pair( extra, chain_account_tag ) );

         tree_info += "\n@" + to_string( p_extras->size( ) );
      }
      else
      {
         string chain_account_tag( "c" + chain + ".a" + chain );

         if( !has_tag( chain_account_tag ) )
            throw runtime_error( "unable to find root chain account for '" + chain_account_tag + "'" );

         string chain_account_hash( tag_file_hash( chain_account_tag ) );
         string chain_account_info( extract_file( chain_account_hash, "" ) );

         vector< string > chain_account_items;
         split( chain_account_info, chain_account_items, '\n' );

         if( chain_account_items.size( ) < 1 )
            throw runtime_error( "unexpected invalid chain_account_info '" + chain_account_info + "'" );

         string reward_info( chain_account_items[ 0 ] );
         string::size_type pos = reward_info.find( '-' );

         if( pos != string::npos )
         {
            unit_return = from_string< uint64_t >( reward_info.substr( pos + 1 ) );
            reward_info.erase( pos );
         }

         unit_reward = from_string< uint64_t >( reward_info );

         string tags, extra;

         if( !prior_block_minter_id.empty( ) )
         {
            string prior_block_minter_tag( list_file_tags( prior_block_minter_id + "*" ) );

            string prior_block_minter_hash( tag_file_hash( prior_block_minter_tag ) );
            string prior_block_minter_data( extract_file( prior_block_minter_hash, "" ) );

            vector< string > prior_block_minter_items;
            split( prior_block_minter_data, prior_block_minter_items, '\n' );

            if( prior_block_minter_items.size( ) < 2 )
               throw runtime_error( "unexpected invalid prior_block_minter_data '" + prior_block_minter_data + "'" );

            string::size_type pos = prior_block_minter_tag.find( ".b" );
            if( pos == string::npos )
               throw runtime_error( "unable to find account balance in '" + prior_block_minter_tag + "'" );

            uint64_t balance = from_string< uint64_t >( prior_block_minter_tag.substr( pos + 2 ) );

            if( balance < unit_reward )
               balance = 0;
            else
               balance -= unit_reward;

            extra = string( c_file_type_str_core_blob );
            extra += prior_block_minter_items[ 0 ] + '\n' + prior_block_minter_items[ 1 ];

            for( size_t i = 3; i < prior_block_minter_items.size( ); i++ )
               extra += "\n" + prior_block_minter_items[ i ];

            p_extras->push_back( make_pair( extra, prior_block_minter_id + ".b*" + to_string( balance ) ) );
         }

         string minter_account( "c" + chain + ".a" + account );
         string minter_account_tag( list_file_tags( minter_account + "*" ) );

         string minter_account_hash( tag_file_hash( minter_account_tag ) );
         string minter_account_data( extract_file( minter_account_hash, "" ) );

         vector< string > minter_account_items;
         split( minter_account_data, minter_account_items, '\n' );

         unsigned long last_height = 0;
         if( minter_account_items.size( ) < 2 )
            throw runtime_error( "unexpected invalid minter_account_data '" + minter_account_data + "'" );

         string previous_hash( minter_account_items[ 0 ] );
         string previous_lock( minter_account_items[ 1 ] );

         if( !check_if_valid_hash_pair( account_hash, previous_hash, true ) )
            throw runtime_error( "invalid hash from minter" );

         if( previous_lock != mint_address && previous_lock != mint_address_uncompressed
          && previous_lock != mint_test_address && previous_lock != mint_test_address_uncompressed )
            throw runtime_error( "invalid public key from minter" );

         // NOTE: If an account has already minted then make sure that this block
         // is more recent than the last one.
         if( minter_account_items.size( ) > 2 )
         {
            last_height = from_string< unsigned long >( minter_account_items[ 2 ] );

            if( block_height <= last_height )
               throw runtime_error( "invalid block height for minting account" );
         }

         pos = minter_account_tag.find( ".b" );
         if( pos == string::npos )
            throw runtime_error( "unable to find account balance in '" + minter_account_tag + "'" );

         uint64_t balance = from_string< uint64_t >( minter_account_tag.substr( pos + 2 ) );

         if( balance < unit_return )
            throw runtime_error( "unsufficient balance to mint" );

         if( last_height && previous_achain.empty( ) )
            throw runtime_error( "missing previous achain link" );

         if( !previous_achain.empty( ) )
         {
            string previous_achain_data( extract_file( previous_achain, "" ) );

            string::size_type pos = previous_achain_data.find( ':' );
            if( pos == string::npos || previous_achain_data.substr( 0, pos ) != string( c_file_type_core_block_object ) )
               throw runtime_error( "invalid previous achain file" );

            block_info info;
            verify_block( previous_achain_data.substr( pos + 1 ), false, 0, &info );

            if( info.mint_id != minter_account
             || !check_if_valid_hash_pair( account_hash, info.mint_hash, true ) )
               throw runtime_error( "previous achain block does not match the current achain" );
         }

         balance -= unit_return;

         pos = minter_account_tag.find( ".b" );
         if( pos != string::npos )
            minter_account_tag.erase( pos );

         if( is_new_chain_head )
            balance += unit_reward;

         extra = string( c_file_type_str_core_blob );
         extra += account_hash + '\n' + account_lock;
         extra += '\n' + to_string( block_height );

         for( size_t i = 3; i < minter_account_items.size( ); i++ )
            extra += '\n' + minter_account_items[ i ];

         tags += "\n" + minter_account_tag + ".b*" + to_string( balance );

         p_extras->push_back( make_pair( extra, tags ) );
         tree_info += "\n@" + to_string( p_extras->size( ) );

         extra = string( c_file_type_str_core_item );
         extra += minter_account_tag + ".undo\n" + minter_account_hash;

         p_extras->push_back( make_pair( extra, "" ) );
         tree_info += "\n@" + to_string( p_extras->size( ) );
      }

      if( !tree_info.empty( ) )
      {
         string block_file_tag( "c" + chain + ".b" + to_string( block_height ) );

         string block_file_with_weight( block_file_tag + "-" + to_string( block_weight ) );
         string block_file_with_weight_and_sig_tag( block_file_with_weight + ".s" + base64_to_tag_name( block_signature ) );

         if( !block_height )
         {
            string tags( block_file_tag );
            tags += "\n" + block_file_with_weight_and_sig_tag + "\nc" + account + ".head";

            p_extras->push_back( make_pair( tree_info, tags ) );
         }
         else
         {
            string block_file_old_tag( "c" + chain + ".b" + to_string( block_height - 1 ) );

            string extra( c_file_type_str_core_item );
            extra += block_file_tag + ".undo\n" + tag_file_hash( block_file_old_tag );

            p_extras->push_back( make_pair( extra, "" ) );
            tree_info += "\n@" + to_string( p_extras->size( ) );

            string tags( block_file_with_weight_and_sig_tag );

            if( is_new_chain_head )
               tags += "\n" + block_file_tag + "\nc" + chain + ".head";

            p_extras->push_back( make_pair( tree_info, tags ) );
         }
      }
   }

   return make_pair( block_height, block_weight );
}

void verify_message( const string& content, bool check_sigs )
{
}

void verify_transaction( const string& content, bool check_sigs )
{
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
         else if( type == string( c_file_type_core_message_object ) )
            verify_message( content.substr( pos + 1 ), check_sigs );
         else if( type == string( c_file_type_core_transaction_object ) )
            verify_transaction( content.substr( pos + 1 ), check_sigs );
         else
            throw runtime_error( "unknown type '" + type + "' for core file" );
      }
      else
         throw runtime_error( "unable to verify non-blob core files" );
   }
}

