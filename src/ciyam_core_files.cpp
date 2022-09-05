// Copyright (c) 2014-2022 CIYAM Developers
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
#  include <vector>
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

using namespace std;

trace_mutex g_mutex;

namespace
{

#include "ciyam_constants.h"

const unsigned int c_max_core_line_size = 1000;

struct block_info
{
   block_info( ) : version( 0 ), unix_time_value( 0 ) { }

   size_t version;

   string data_file_hash;

   string public_key_hash;
   string secondary_key_hash;
   string tertiary_key_hash;

   uint64_t unix_time_value;
};

void verify_block( const string& content, bool check_sigs, block_info* p_block_info = 0 )
{
   guard g( g_mutex, "verify_block" );

   vector< string > lines;

   if( !content.empty( ) )
      split( content, lines, '\n', c_esc, false );

   if( lines.empty( ) )
      throw runtime_error( "unexpected empty block content" );

   string identity;

   size_t version = 0;
   size_t num_tree_items = 0;

   bool is_secondary = false;
   bool is_tertiary = false;

   uint64_t block_height = 0;
   uint64_t unix_time_value = 0;

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
      bool has_identity = false;
      bool has_num_tree_items = false;

      for( size_t i = 0; i < attributes.size( ); i++ )
      {
         string next_attribute( attributes[ i ] );
         if( next_attribute.empty( ) )
            throw runtime_error( "unexpected empty attribute in block header '" + header + "'" );

         if( i > 1 && !has_num_tree_items )
         {
            if( next_attribute.find( c_file_type_core_block_header_num_tree_items_prefix ) == 0 )
            {
               has_num_tree_items = true;

               num_tree_items = from_string< size_t >( next_attribute.substr(
                strlen( c_file_type_core_block_header_num_tree_items_prefix ) ) );

               // NOTE: The total tree items attribute does not include the tree root itself but in order to match
               // the number of files being processed in a peer session the value is being incremented by one here.
               if( !p_block_info )
               {
                  set_session_variable(
                   get_special_var_name( e_special_var_blockchain_num_tree_items ), to_string( num_tree_items + 1 ) );
               }

               continue;
            }
         }

         if( !has_height )
         {
            if( next_attribute.find( c_file_type_core_block_header_height_prefix ) != 0 )
               throw runtime_error( "unexpected missing height attribute in block header '" + header + "'" );

            string value( next_attribute.substr( strlen( c_file_type_core_block_header_height_prefix ) ) );

            regex expr( c_regex_integer, true, true );

            if( ( value.length( ) > 1 && value[ 0 ] == '0' ) || expr.search( value ) != 0 )
               throw runtime_error( "invalid height value '" + value + "'" );

            has_height = true;

            block_height = from_string< uint64_t >( value );

            if( !p_block_info )
               set_session_variable( get_special_var_name( e_special_var_blockchain_height ), value );
         }
         else if( !has_identity )
         {
            if( next_attribute.find( c_file_type_core_block_header_identity_prefix ) != 0 )
               throw runtime_error( "unexpected missing identity attribute in block header '" + header + "'" );

            has_identity = true;

            identity = next_attribute.substr( strlen( c_file_type_core_block_header_identity_prefix ) );
         }
         else if( !version )
         {
            if( next_attribute.find( c_file_type_core_block_header_version_number_prefix ) != 0 )
               throw runtime_error( "unexpected missing version number attribute in block header '" + header + "'" );

            string version_str( next_attribute.substr( strlen( c_file_type_core_block_header_version_number_prefix ) ) );

            version = from_string< size_t >( version_str );

            if( p_block_info )
               p_block_info->version = version;
         }
         else
            throw runtime_error( "unexpected extraneous attribute in block header '" + header + "'" );
      }

      if( identity.length( ) != c_bc_identity_length )
         throw runtime_error( "unexpected missing or incorrect identity attribute in block header '" + header + "'" );
   }

   block_info info;

   string hind_hash, last_block_hash, public_key_hash, tree_root_hash, signature_file_hash;

   bool has_primary_pubkey = false;
   bool has_secondary_pubkey = false;
   bool has_tertiary_pubkey = false;

   size_t scaling_value = c_bc_scaling_value;

   if( identity == string( c_demo_identity ) )
      scaling_value = c_bc_scaling_demo_value;

   size_t scaling_squared = ( scaling_value * scaling_value );

   for( size_t i = 1; i < lines.size( ); i++ )
   {
      string next_line( lines[ i ] );

      if( next_line.size( ) < 3 )
         throw runtime_error( "unexpected line < 3 '" + next_line + "' in verify_block" );

      if( next_line.length( ) > c_max_core_line_size )
         throw runtime_error( "unexpected line length exceeds "
          + to_string( c_max_core_line_size ) + " in '" + next_line + "' in verify_block" );

      string next_attribute( next_line );

      if( !block_height )
      {
         if( !has_primary_pubkey )
         {
            size_t len = strlen( c_file_type_core_block_detail_pubkey_hashes_prefix );

            if( next_attribute.substr( 0, len ) != string( c_file_type_core_block_detail_pubkey_hashes_prefix ) )
               throw runtime_error( "invalid genesis block public key hashes attribute '" + next_attribute + "'" );

            next_attribute.erase( 0, len );

            has_primary_pubkey = true;

            string::size_type pos = next_attribute.find( ',' );

            string primary_pubkey_hash( hex_encode( base64::decode( next_attribute.substr( 0, pos ) ) ) );

            if( p_block_info )
               p_block_info->public_key_hash = primary_pubkey_hash;
            else
               set_session_variable(
                get_special_var_name( e_special_var_blockchain_primary_pubkey_hash ), primary_pubkey_hash );

            if( pos != string::npos )
            {
               next_attribute.erase( 0, pos + 1 );

               has_secondary_pubkey = true;

               pos = next_attribute.find( ',' );

               string secondary_pubkey_hash( hex_encode( base64::decode( next_attribute.substr( 0, pos ) ) ) );

               if( p_block_info )
                  p_block_info->secondary_key_hash = secondary_pubkey_hash;
               else
                  set_session_variable(
                   get_special_var_name( e_special_var_blockchain_secondary_pubkey_hash ), secondary_pubkey_hash );

               if( pos != string::npos )
               {
                  next_attribute.erase( 0, pos + 1 );

                  has_tertiary_pubkey = true;

                  string tertiary_pubkey_hash( hex_encode( base64::decode( next_attribute ) ) );

                  if( p_block_info )
                     p_block_info->tertiary_key_hash = tertiary_pubkey_hash;
                  else
                     set_session_variable(
                      get_special_var_name( e_special_var_blockchain_tertiary_pubkey_hash ), tertiary_pubkey_hash );
               }
            }
         }
         else
            throw runtime_error( "unexpected extraneous genesis block attribute '" + next_attribute + "'" );
      }
      else
      {
         if( i == 1 && hind_hash.empty( ) )
         {
            string prefix( c_file_type_core_block_detail_hind_hash_prefix );

            size_t len = prefix.length( );

            if( ( next_attribute.length( ) > len )
             && ( prefix == next_attribute.substr( 0, len ) ) )
            {
               next_attribute.erase( 0, len );

               hind_hash = hex_encode( base64::decode( next_attribute ) );

               set_session_variable(
                get_special_var_name( e_special_var_blockchain_hind_hash ), hind_hash );

               continue;
            }
         }

         if( i > 3 && tree_root_hash.empty( ) )
         {
            string prefix( c_file_type_core_block_detail_tree_root_hash_prefix );

            size_t len = prefix.length( );

            if( ( next_attribute.length( ) > len )
             && ( prefix == next_attribute.substr( 0, len ) ) )
            {
               next_attribute.erase( 0, len );

               tree_root_hash = hex_encode( base64::decode( next_attribute ) );

               if( check_sigs && !has_file( tree_root_hash ) )
                  throw runtime_error( "tree root file '" + tree_root_hash + "' not found" );

               set_session_variable(
                get_special_var_name( e_special_var_blockchain_tree_root_hash ), tree_root_hash );

               continue;
            }
         }

         if( last_block_hash.empty( ) )
         {
            size_t len = strlen( c_file_type_core_block_detail_last_hashes_prefix );

            if( next_attribute.substr( 0, len ) != string( c_file_type_core_block_detail_last_hashes_prefix ) )
               throw runtime_error( "invalid block last block hashes attribute '" + next_attribute + "'" );

            next_attribute.erase( 0, len );

            vector< string > last_block_hashes;
            split( next_attribute, last_block_hashes );

            last_block_hash = hex_encode( base64::decode( last_block_hashes[ 0 ] ) );

            string last_block_tag( c_bc_prefix + identity + "." + to_string( block_height - 1 ) + c_blk_suffix );

            bool found_last = false;

            if( has_tag( last_block_tag ) )
            {
               if( tag_file_hash( last_block_tag ) != last_block_hash )
                  throw runtime_error( "incorrect last block hash '" + last_block_hash + "'" );

               found_last = true;
            }

            if( last_block_hashes.size( ) > 1 )
            {
               last_block_hash = hex_encode( base64::decode( last_block_hashes[ 1 ] ) );

               last_block_tag = c_bc_prefix + identity + "." + to_string( block_height - scaling_value ) + c_blk_suffix;

               if( has_tag( last_block_tag ) )
               {
                  if( tag_file_hash( last_block_tag ) != last_block_hash )
                     throw runtime_error( "incorrect last block hash '" + last_block_hash + "'" );

                  found_last = true;
               }
            }

            if( last_block_hashes.size( ) > 2 )
            {
               last_block_hash = hex_encode( base64::decode( last_block_hashes[ 2 ] ) );

               last_block_tag = c_bc_prefix + identity + "." + to_string( block_height - scaling_squared ) + c_blk_suffix;

               if( has_tag( last_block_tag ) )
               {
                  if( tag_file_hash( last_block_tag ) != last_block_hash )
                     throw runtime_error( "incorrect last block hash '" + last_block_hash + "'" );

                  found_last = true;
               }
            }

            if( !found_last )
               throw runtime_error( "no last block file was found" );
         }
         else if( public_key_hash.empty( ) )
         {
            size_t len = strlen( c_file_type_core_block_detail_pubkey_hashes_prefix );

            if( next_attribute.substr( 0, len ) != string( c_file_type_core_block_detail_pubkey_hashes_prefix ) )
               throw runtime_error( "invalid data public key hashes attribute '" + next_attribute + "'" );

            next_attribute.erase( 0, len );

            vector< string > public_key_hashes;
            split( next_attribute, public_key_hashes );

            public_key_hash = hex_encode( base64::decode( public_key_hashes[ 0 ] ) );

            if( check_sigs && !has_file( public_key_hash ) )
               throw runtime_error( "public key file '" + public_key_hash + "' not found" );

            if( p_block_info )
               p_block_info->public_key_hash = public_key_hash;
            else
               set_session_variable(
                get_special_var_name( e_special_var_blockchain_primary_pubkey_hash ), public_key_hash );

            if( public_key_hashes.size( ) > 1 )
            {
               string secondary_pubkey_hash( hex_encode( base64::decode( public_key_hashes[ 1 ] ) ) );

               if( p_block_info )
                  p_block_info->secondary_key_hash = secondary_pubkey_hash;
               else
                  set_session_variable(
                   get_special_var_name( e_special_var_blockchain_secondary_pubkey_hash ), secondary_pubkey_hash );

               if( public_key_hashes.size( ) > 2 )
               {
                  string tertiary_pubkey_hash( hex_encode( base64::decode( public_key_hashes[ 2 ] ) ) );

                  if( p_block_info )
                     p_block_info->tertiary_key_hash = tertiary_pubkey_hash;
                  else
                     set_session_variable(
                      get_special_var_name( e_special_var_blockchain_tertiary_pubkey_hash ), tertiary_pubkey_hash );
               }
            }
         }
         else if( !unix_time_value )
         {
            size_t len = strlen( c_file_type_core_block_detail_unix_block_time_value_prefix );

            if( next_attribute.substr( 0, len ) != string( c_file_type_core_block_detail_unix_block_time_value_prefix ) )
               throw runtime_error( "invalid unix block time value attribute '" + next_attribute + "'" );

            next_attribute.erase( 0, len );

            unix_time_value = from_string< uint64_t >( next_attribute );

            if( p_block_info )
               p_block_info->unix_time_value = unix_time_value;

            if( check_sigs && block_height > 1 )
            {
               if( version < info.version )
                  throw runtime_error( "invalid block version value is less than last" );

               if( unix_time_value <= info.unix_time_value )
                  throw runtime_error( "invalid unix block time value not more recent than last" );
            }
         }
         else
            throw runtime_error( "unexpected extraneous block attribute '" + next_attribute + "'" );
      }
   }

   if( block_height && !unix_time_value )
      throw runtime_error( "unexpected missing unix block time stamp attribute" );

   if( !block_height )
   {
      if( !has_primary_pubkey )
         throw runtime_error( "unexpected missing genesis block primary pubkey attribute" );

      if( !has_secondary_pubkey )
         throw runtime_error( "unexpected missing genesis block secondary pubkey attribute" );
   }
   else
   {
      if( last_block_hash.empty( ) )
         throw runtime_error( "unexpected missing block last block hash attribute" );

      if( hind_hash.empty( ) && signature_file_hash.empty( ) )
         throw runtime_error( "unexpected missing block signature file hash attribute" );
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

void verify_core_file( const string& content, bool check_sigs )
{
   if( content.empty( ) )
      throw runtime_error( "invalid empty core file content" );
   else
   {
      unsigned char file_type_and_extra = content[ 0 ];

      unsigned char file_type = ( file_type_and_extra & c_file_type_val_mask );

      if( file_type == c_file_type_val_blob )
      {
         string::size_type pos = content.find( ':', 1 );

         if( pos == string::npos )
            throw runtime_error( "invalid content '" + content + "' for core file" );

         string type( content.substr( 1, pos - 1 ) );

         if( type == string( c_file_type_core_block_object ) )
            verify_block( content.substr( pos + 1 ), check_sigs );
         else
            throw runtime_error( "unknown type '" + type + "' for core file" );
      }
      else
         throw runtime_error( "unable to verify non-blob core files" );
   }
}

string verified_hash_from_signature( const string& public_key_hash, const string& signature_hash )
{
   string file_hash_info( public_key_hash + ':' + signature_hash );
   string verified_file_hash( crypto_lamport( file_hash_info, "", false, true ) );

   return verified_file_hash;
}

bool is_block( const string& core_type )
{
   return ( core_type == string( c_file_type_core_block_object ) );
}

string construct_blob_for_block_content( const string& block_content )
{
   return string( c_file_type_str_core_blob ) + block_content;
}

string create_peer_repository_entry_pull_info(
 const string& repository, const string& hash, bool store_as_blob, size_t num_items_skipped )
{
   string retval, local_hash, local_public_key, master_public_key;

   if( fetch_repository_entry_record( repository, hash,
    local_hash, local_public_key, master_public_key, false ) && has_file( local_hash ) )
      retval = create_peer_repository_entry_pull_info( repository,
       hash, local_hash, local_public_key, master_public_key, store_as_blob, num_items_skipped );

   return retval;
}

string create_peer_repository_entry_pull_info( const string& repository,
 const string& hash, const string& local_hash, const string& local_public_key,
 const string& master_public_key, bool store_as_blob, size_t num_items_skipped )
{
   string retval;

   string file_data;

   if( store_as_blob )
      file_data += string( c_file_type_str_blob );

   file_data += c_file_repository_meta_data_line_prefix;
   file_data += c_file_repository_meta_data_info_type_raw;
   file_data += '\n';

   file_data += c_file_repository_public_key_line_prefix;
   file_data += base64::encode( hex_decode( local_public_key ) );

   if( !master_public_key.empty( ) )
      file_data += '-' + base64::encode( hex_decode( master_public_key ) );

   if( num_items_skipped )
      file_data += ':' + to_string( num_items_skipped );

   file_data += '\n';

   file_data += c_file_repository_source_hash_line_prefix;
   file_data += base64::encode( hex_decode( local_hash ) );
   file_data += '\n';

   file_data += c_file_repository_target_hash_line_prefix;
   file_data += base64::encode( hex_decode( hash ) );

   if( !store_as_blob )
      retval = file_data;
   else
      // NOTE: Don't allow compression to assist with interactive testing and/or debugging.
      retval = create_raw_file( file_data, false );

   return retval;
}

string create_peer_repository_entry_push_info(
 const string& file_hash, const string& password,
 string* p_pub_key, bool store_as_blob, bool is_for_testing, size_t num_items_skipped )
{
   string retval;

#ifndef SSL_SUPPORT
   throw runtime_error( "create_peer_repository_entry_push_info requires SSL support" );
#else
   string file_data;

   if( store_as_blob )
      file_data += string( c_file_type_str_blob );

   file_data += c_file_repository_meta_data_line_prefix;
   file_data += c_file_repository_meta_data_info_type_raw;
   file_data += '\n';

   auto_ptr< private_key > ap_priv_key;

   if( is_for_testing )
      ap_priv_key.reset( new private_key( sha256( c_dummy ).get_digest_as_string( ) ) );
   else
      // NOTE: The first nibble is zeroed out to ensure that the hash value is always valid to use
      // as a Bitcoin address "secret" (as the range of its EC is smaller than the full 256 bits).
      ap_priv_key.reset(
       new private_key( "0" + sha256( file_hash + password ).get_digest_as_string( ).substr( 1 ) ) );

   if( p_pub_key )
      *p_pub_key = ap_priv_key->get_public( );

   file_data += c_file_repository_public_key_line_prefix;
   file_data += ap_priv_key->get_public( true, true );

   if( num_items_skipped )
      file_data += ':' + to_string( num_items_skipped );

   file_data += '\n';

   file_data += c_file_repository_source_hash_line_prefix;
   file_data += base64::encode( hex_decode( file_hash ) );

   if( !store_as_blob )
      retval = file_data;
   else
      // NOTE: Don't allow compression to assist with interactive testing and/or debugging.
      retval = create_raw_file( file_data, false );
#endif

   return retval;
}

void decrypt_pulled_peer_file(
 const string& dest_hash, const string& src_hash, const string& password,
 bool is_for_testing, const string* p_file_data, const string* p_encrypted_hash )
{
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
         decrypt_pulled_peer_file( dest_hash, src_hash,
          password, public_key_in_hex, is_for_testing, p_file_data, p_encrypted_hash );
   }
}

void decrypt_pulled_peer_file(
 const string& dest_hash, const string& src_hash,
 const string& password, const string& public_key_in_hex,
 bool is_for_testing, const string* p_file_data, const string* p_encrypted_hash )
{
#ifndef SSL_SUPPORT
   throw runtime_error( "decrypt_pulled_peer_file requires SSL support" );
#else
   public_key pub_key( public_key_in_hex );

   auto_ptr< private_key > ap_priv_key;

   if( is_for_testing )
      ap_priv_key.reset( new private_key( sha256( c_dummy ).get_digest_as_string( ) ) );
   else
      // NOTE: The first nibble is zeroed out to ensure that the hash value is always valid to use
      // as a Bitcoin address "secret" (as the range of its EC is smaller than the full 256 bits).
      ap_priv_key.reset(
       new private_key( "0" + sha256( dest_hash + password ).get_digest_as_string( ).substr( 1 ) ) );

   bool is_encrypted = false;
   unsigned char type_and_extra = '\0';

   string file_data;

   if( p_file_data && !p_file_data->empty( ) )
   {
      file_data = *p_file_data;
      type_and_extra = file_data[ 0 ];

      is_encrypted = ( file_data[ 0 ] & c_file_type_val_encrypted );

      file_data.erase( 0, 1 );
   }
   else
   {
      if( !is_for_testing )
         file_data = extract_file( src_hash, "", 0, 0, &type_and_extra, &is_encrypted );
      else
         file_data = extract_file( src_hash, "", 0, 0, &type_and_extra, &is_encrypted, true );
   }

   stringstream ss( file_data );
   crypt_stream( ss, ap_priv_key->construct_shared( pub_key ) );

   string raw_file_data( ( char )type_and_extra + ss.str( ) );

   if( is_encrypted && p_encrypted_hash && !p_encrypted_hash->empty( ) )
   {
      sha256 encrypted_hash( raw_file_data );

      if( *p_encrypted_hash != encrypted_hash.get_digest_as_string( ) )
         throw runtime_error( "failed to decrypt (incorrect content)" );
   }

   string hash;

   if( !is_encrypted )
      hash = create_raw_file( raw_file_data );
   else
      create_raw_file( raw_file_data, true, 0, 0, dest_hash.c_str( ) );

   if( !is_encrypted && ( hash != dest_hash ) )
   {
      delete_file( hash );
      throw runtime_error( "failed to decrypt (bad password or incorrect content)" );
   }
#endif
}
