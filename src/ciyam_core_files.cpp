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
#include "utilities.h"
#include "ciyam_base.h"
#ifdef SSL_SUPPORT
#  include "crypto_keys.h"
#endif
#include "ciyam_files.h"

using namespace std;

namespace
{

#include "ciyam_constants.h"

void verify_block( const string& content, bool check_sigs, vector< pair< string, string > >* p_extras )
{
   vector< string > lines;
   split( content, lines, '\n' );

   if( lines.empty( ) )
      throw runtime_error( "unexpected empty block content" );

   unsigned long block_height = 0;
   unsigned long block_weight = 0;
   unsigned long total_weight = 0;

   string chain, account, previous_block, public_key_base64;

   uint64_t unit_reward = 0;
   uint64_t unit_supply = 0;

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
      bool has_total_weight = false;
      bool has_previous_block = false;
      bool has_public_key = false;

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
         else if( !has_previous_block )
         {
            if( !block_height )
            {
               if( next_attribute.find( c_file_type_core_block_header_chain_meta_prefix ) != 0 )
                  throw runtime_error( "unexpected missing chain_meta attribute in block header '" + header + "'" );

               next_attribute.erase( 0, string( c_file_type_core_block_header_chain_meta_prefix ).length( ) );

               vector< string > meta_data;
               split( next_attribute, meta_data, ';' );

               bool has_reward = false;
               bool has_supply = false;

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

                     has_reward = true;
                     unit_reward = from_string< uint64_t >( next_meta );
                  }
                  else if( !has_supply )
                  {
                     if( next_meta.find( c_file_type_core_block_header_chain_meta_supply_prefix ) != 0 )
                        throw runtime_error( "unexpected missing supply meta item in block header '" + header + "'" );

                     next_meta.erase( 0, string( c_file_type_core_block_header_chain_meta_supply_prefix ).length( ) );

                     has_supply = true;
                     unit_supply = from_string< uint64_t >( next_meta );
                  }
                  else
                     throw runtime_error( "unexpected chain_meta item '" + next_meta + "' in block header '" + header + "'" );
               }

               if( !has_supply )
                  throw runtime_error( "unexpected missing supply meta item in block header '" + header + "'" );

               has_previous_block = true;
            }
            else
            {
               pos = next_attribute.find( c_file_type_core_block_header_previous_block_prefix );
               if( pos != 0 )
                  throw runtime_error( "unexpected missing previous_block attribute in block header '" + header + "'" );

               has_previous_block = true;
               previous_block = next_attribute.substr( pos
                + string( c_file_type_core_block_header_previous_block_prefix ).length( ) );

               if( !has_file( previous_block ) )
                  throw runtime_error( "previous block '" + previous_block + "' does not exist" );
            }
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

      if( block_height && !has_tag( "c" + chain + ".a" + account ) )
         throw runtime_error( "unknown account '" + account + "' for block header" );

      if( block_height == 0 && !get_is_known_blockchain( account ) )
         throw runtime_error( "invalid unrecognised blockchain '" + account + "'" );
   }

   string verify( string( c_file_type_core_block_object ) + ':' + header );

   string tree_info( c_file_type_str_core_tree );
   tree_info += "@0";

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

         if( attributes.size( ) < 4 )
            throw runtime_error( "invalid account format '" + next_line + "'" );

         bool has_id = false;
         bool has_hash = false;
         bool has_hash_address = false;
         bool has_next_address = false;

         string id, hash;
         string hash_address;
         string next_address;

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
            else if( !has_hash_address )
            {
               size_t len = string( c_file_type_core_block_detail_account_hash_address_prefix ).length( );

               if( next_attribute.length( ) < len + 1
                || next_attribute.substr( 0, len ) != string( c_file_type_core_block_detail_account_hash_address_prefix ) )
                  throw runtime_error( "invalid account hash address attribute '" + next_attribute + "'" );

               regex expr( c_regex_bitcoin_address );

               next_attribute.erase( 0, len );

               if( expr.search( next_attribute ) != 0 )
                  throw runtime_error( "invalid account hash address '" + next_attribute + "'" );

               has_hash_address = true;
               hash_address = next_attribute;
            }
            else if( !has_next_address )
            {
               size_t len = string( c_file_type_core_block_detail_account_next_address_prefix ).length( );

               if( next_attribute.length( ) < len + 1
                || next_attribute.substr( 0, len ) != string( c_file_type_core_block_detail_account_next_address_prefix ) )
                  throw runtime_error( "invalid account next address attribute '" + next_attribute + "'" );

               regex expr( c_regex_bitcoin_address );

               next_attribute.erase( 0, len );

               if( expr.search( next_attribute ) != 0 )
                  throw runtime_error( "invalid account next address '" + next_attribute + "'" );

               has_next_address = true;
               next_address = next_attribute;
            }
            else
               throw runtime_error( "unexpected account attribute '" + next_attribute + "'" );
         }

         if( !has_next_address )
            throw runtime_error( "unexpected incomplete account information '" + next_line + "'" );

         if( p_extras )
         {
            if( !block_height )
            {
               string extra( c_file_type_str_core_blob );
               extra += hash + '\n' + hash_address + '\n' + next_address + '\n' + to_string( unit_reward );

               if( unit_supply < unit_reward )
                  throw runtime_error( "invalid insufficient supply to create accounts" );

               unit_supply -= unit_reward;

               string tags( "c" + chain + ".a" + id );
               p_extras->push_back( make_pair( extra, tags ) );
            }
            else
               throw runtime_error( "account extra for non-root block nyi" );

            tree_info += "\n@" + to_string( i );
         }
      }
      else if( prefix == string( c_file_type_core_block_detail_signature_prefix ) )
#ifdef SSL_SUPPORT
      {
         public_key pkey( public_key_base64, true );

         // FUTURE: If not a root block then need to verify that the public key matches the account's address.

         if( check_sigs && !pkey.verify_signature( verify, next_line ) )
            throw runtime_error( "invalid block signature" );
      }
#else
         ;
#endif
      else
         throw runtime_error( "unexpected line '" + lines[ i ] + "' in verify_block" );
   }

   if( p_extras )
   {
      if( !block_height )
      {
         string extra( c_file_type_str_core_blob );
         extra += to_string( unit_reward ) + '\n' + to_string( unit_supply );

         string tags( "c" + chain + ".a" + account );
         p_extras->push_back( make_pair( extra, tags ) );

         tree_info += "\n@" + to_string( p_extras->size( ) );
      }
      else
      {
         string chain_account_tag( "c" + chain + ".a" + chain );

         if( !has_tag( chain_account_tag ) )
            throw runtime_error( "unable able to find find root chain account for '" + chain_account_tag + "'" );

         string chain_account_hash( tag_file_hash( chain_account_tag ) );
         string chain_account_info( extract_file( chain_account_hash, "" ) );

         vector< string > chain_account_items;
         split( chain_account_info, chain_account_items, '\n' );

         if( chain_account_items.size( ) < 2 )
            throw runtime_error( "unexpected invalid chain_account_info '" + chain_account_info + "'" );

         unit_reward = from_string< uint64_t >( chain_account_items[ 0 ] );
         unit_supply = from_string< uint64_t >( chain_account_items[ 1 ] );

         if( unit_supply < unit_reward )
            unit_reward = unit_supply;

         unit_supply -= unit_reward;

         string extra( c_file_type_str_core_blob );
         extra += to_string( unit_reward ) + '\n' + to_string( unit_supply );

         for( size_t i = 2; i < chain_account_items.size( ); i++ )
            extra += '\n' + chain_account_items[ i ];

         string tags( chain_account_tag + "."
          + to_string( block_height ) + "-" + to_string( total_weight ) + "\n" + chain_account_tag );

         p_extras->push_back( make_pair( extra, tags ) );
         tree_info += "\n@" + to_string( p_extras->size( ) );

         string mint_account_tag( "c" + chain + ".a" + account );
         string mint_account_hash( tag_file_hash( mint_account_tag ) );
         string mint_account_info( extract_file( mint_account_hash, "" ) );

         vector< string > mint_account_items;
         split( mint_account_info, mint_account_items, '\n' );

         if( mint_account_items.size( ) < 4 )
            throw runtime_error( "unexpected invalid mint_account_info '" + mint_account_info + "'" );

         uint64_t balance = from_string< uint64_t >( mint_account_items[ 3 ] );
         balance += unit_reward;

         extra = string( c_file_type_str_core_blob );
         extra += mint_account_items[ 0 ] + '\n' + mint_account_items[ 1 ] + '\n' + mint_account_items[ 2 ];
         extra += '\n' + to_string( balance );

         tags = mint_account_tag + "." + to_string( block_height )
          + "-" + to_string( total_weight ) + "\n" + mint_account_tag;

         p_extras->push_back( make_pair( extra, tags ) );
         tree_info += "\n@" + to_string( p_extras->size( ) );
      }

      if( !tree_info.empty( ) )
      {
         if( !block_height )
            p_extras->push_back( make_pair( tree_info, "c" + chain + ".b0\nc" + account + ".head" ) );
         else
            p_extras->push_back( make_pair( tree_info,
             "c" + chain + ".b" + to_string( block_height )
             + "-" + to_string( total_weight ) + "\nc" + chain
             + ".b" + to_string( block_height ) + "\nc" + chain + ".head" ) );
      }
   }
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

