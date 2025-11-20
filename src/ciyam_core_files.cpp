// Copyright (c) 2014-2025 CIYAM Developers
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
#include "ciyam_files.h"
#ifdef SSL_SUPPORT
#  include "crypto_keys.h"
#endif
#include "crypt_stream.h"
#include "ciyam_variables.h"

using namespace std;

trace_mutex g_mutex;

namespace
{

#include "ciyam_constants.h"

const unsigned int c_max_core_line_size = 1000;

void verify_block( const string& content, bool check_dependents )
{
   guard g( g_mutex, "verify_block" );

   vector< string > lines;

   if( !content.empty( ) )
      split( content, lines, '\n', c_esc, false );

   if( lines.empty( ) )
      throw runtime_error( "unexpected empty block content" );

   string identity, stream_cipher, targeted_identity;

   size_t version = 0;
   size_t num_tree_items = 0;

   bool is_secondary = false;
   bool is_tertiary = false;

   uint64_t block_height = 0;
   uint64_t unix_time_value = 0;
   uint64_t last_unix_time_value = 0;

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
      bool has_stream_cipher = false;
      bool has_num_tree_items = false;
      bool has_targeted_identity = false;

      for( size_t i = 0; i < attributes.size( ); i++ )
      {
         string next_attribute( attributes[ i ] );
         if( next_attribute.empty( ) )
            throw runtime_error( "unexpected empty attribute in block header '" + header + "'" );

         if( i > 1 && ( !has_num_tree_items || !has_stream_cipher || !has_targeted_identity ) )
         {
            if( next_attribute.find( c_file_type_core_block_header_num_items_prefix ) == 0 )
            {
               has_num_tree_items = true;

               num_tree_items = from_string< size_t >(
                next_attribute.substr( strlen( c_file_type_core_block_header_num_items_prefix ) ) );

               set_session_variable(
                get_special_var_name( e_special_var_blockchain_num_tree_items ), to_string( num_tree_items ) );

               continue;
            }
            else if( next_attribute.find( c_file_type_core_block_header_stream_cipher_prefix ) == 0 )
            {
               has_stream_cipher = true;

               stream_cipher = next_attribute.substr( strlen( c_file_type_core_block_header_stream_cipher_prefix ) );

               set_session_variable( get_special_var_name( e_special_var_blockchain_stream_cipher ), stream_cipher );

               continue;
            }
            else if( next_attribute.find( c_file_type_core_block_header_targeted_ident_prefix ) == 0 )
            {
               has_targeted_identity = true;

               targeted_identity = next_attribute.substr( strlen( c_file_type_core_block_header_targeted_ident_prefix ) );

               set_session_variable( get_special_var_name( e_special_var_blockchain_targeted_identity ), targeted_identity );

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
         }
         else
            throw runtime_error( "unexpected extraneous attribute in block header '" + header + "'" );
      }

      if( !has_stream_cipher )
      {
         if( !block_height )
            set_session_variable( get_special_var_name( e_special_var_blockchain_stream_cipher ), "" );
         else
            set_session_variable( get_special_var_name( e_special_var_blockchain_stream_cipher ), stream_cipher_string( ) );
      }

      if( identity.length( ) != c_bc_identity_length )
         throw runtime_error( "unexpected missing or incorrect identity attribute in block header '" + header + "'" );
   }

   string hind_hash, last_block_hash, op_list_hash, public_key_hash, tree_root_hash, signature_file_hash;

   bool has_primary_pubkey = false;
   bool has_secondary_pubkey = false;
   bool has_tertiary_pubkey = false;

   size_t scaling_value = c_bc_scaling_value;

   string blockchain_identity( get_session_blockchain( ) );

   if( blockchain_identity.empty( ) )
      blockchain_identity = get_raw_session_variable(
       get_special_var_name( e_special_var_blockchain_identity ) );

   if( blockchain_identity.empty( ) )
      throw runtime_error( "unexpected empty 'blockchain_identity' in verify_block" );

   if( ( blockchain_identity == string( c_test_backup_identity ) )
    || ( blockchain_identity == string( c_test_shared_identity ) ) )
      scaling_value = c_bc_scaling_test_value;

   size_t scaling_squared = ( scaling_value * scaling_value );

   if( identity != blockchain_identity )
   {
      string genesis_tag( c_bc_prefix + blockchain_identity );

      genesis_tag += ".0" + string( c_blk_suffix );

      if( !has_tag( genesis_tag ) )
         throw runtime_error( "unexpected genesis block for '" + blockchain_identity + "' not found" );

      string hash_info( tag_file_hash( genesis_tag ) );

      hash_info += '-' + to_string( block_height );

      sha256 hash( hash_info );

      hash_info = hash.get_digest_as_string( );

      if( hash_info.find( identity ) != 0 )
         throw runtime_error( "invalid identity '" + identity + "' in block at height "
          + to_string( block_height ) + " for blockchain '" + blockchain_identity + "'" );
   }

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

            set_session_variable(
             get_special_var_name( e_special_var_blockchain_primary_pubkey_hash ), primary_pubkey_hash );

            if( pos != string::npos )
            {
               next_attribute.erase( 0, pos + 1 );

               has_secondary_pubkey = true;

               pos = next_attribute.find( ',' );

               string secondary_pubkey_hash( hex_encode( base64::decode( next_attribute.substr( 0, pos ) ) ) );

               set_session_variable(
                get_special_var_name( e_special_var_blockchain_secondary_pubkey_hash ), secondary_pubkey_hash );

               if( pos != string::npos )
               {
                  next_attribute.erase( 0, pos + 1 );

                  has_tertiary_pubkey = true;

                  string tertiary_pubkey_hash( hex_encode( base64::decode( next_attribute ) ) );

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

         if( !last_block_hash.empty( ) && op_list_hash.empty( ) )
         {
            string prefix( c_file_type_core_block_detail_op_list_hash_prefix );

            size_t len = prefix.length( );

            if( ( next_attribute.length( ) > len )
             && ( prefix == next_attribute.substr( 0, len ) ) )
            {
               next_attribute.erase( 0, len );

               op_list_hash = hex_encode( base64::decode( next_attribute ) );

               if( check_dependents && !has_file( op_list_hash ) )
                  throw runtime_error( "op list file '" + op_list_hash + "' not found" );

               set_session_variable(
                get_special_var_name( e_special_var_blockchain_op_list_hash ), op_list_hash );

               continue;
            }
         }

         if( !public_key_hash.empty( ) && tree_root_hash.empty( ) )
         {
            string prefix( c_file_type_core_block_detail_tree_root_hash_prefix );

            size_t len = prefix.length( );

            if( ( next_attribute.length( ) > len )
             && ( prefix == next_attribute.substr( 0, len ) ) )
            {
               next_attribute.erase( 0, len );

               tree_root_hash = hex_encode( base64::decode( next_attribute ) );

               if( check_dependents && !has_file( tree_root_hash ) )
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

            string last_block_tag( c_bc_prefix + blockchain_identity + "." + to_string( block_height - 1 ) + c_blk_suffix );

            bool found_last = false;

            if( has_tag( last_block_tag ) )
            {
               if( tag_file_hash( last_block_tag ) != last_block_hash )
                  throw runtime_error( "incorrect last block hash '" + last_block_hash + "'" );

               found_last = true;

               if( block_height > 1 )
               {
                  string last_block_info( extract_file( last_block_hash, "" ) );

                  vector< string > last_block_lines;

                  split( last_block_info, last_block_lines, '\n', c_esc, false );

                  string last_block_final_attribute( last_block_lines[ last_block_lines.size( ) - 1 ] );

                  // NOTE: If the last block ended with a "c_ciyam_dummy_unix_time" unix time attribute then this
                  // block will be considered as invalid (as no further block can be permitted after this point).
                  string dummy_unix_time_attribute( c_file_type_core_block_detail_unix_block_time_value_prefix );

                  dummy_unix_time_attribute += string( c_ciyam_dummy_unix_time );

                  if( last_block_final_attribute == dummy_unix_time_attribute )
                     throw runtime_error( "invalid attempt to extend blockchain after final '"
                      + blockchain_identity + "' block at height " + to_string( block_height - 1 ) );

                  last_unix_time_value = from_string< uint64_t >(
                   last_block_final_attribute.substr( strlen( c_file_type_core_block_detail_unix_block_time_value_prefix ) ) );
               }
            }

            if( last_block_hashes.size( ) > 1 )
            {
               last_block_hash = hex_encode( base64::decode( last_block_hashes[ 1 ] ) );

               last_block_tag = c_bc_prefix + blockchain_identity + "." + to_string( block_height - scaling_value ) + c_blk_suffix;

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

               last_block_tag = c_bc_prefix + blockchain_identity + "." + to_string( block_height - scaling_squared ) + c_blk_suffix;

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

            if( check_dependents && !has_file( public_key_hash ) )
               throw runtime_error( "public key file '" + public_key_hash + "' not found" );

            set_session_variable(
             get_special_var_name( e_special_var_blockchain_primary_pubkey_hash ), public_key_hash );

            if( public_key_hashes.size( ) > 1 )
            {
               string secondary_pubkey_hash( hex_encode( base64::decode( public_key_hashes[ 1 ] ) ) );

               set_session_variable(
                get_special_var_name( e_special_var_blockchain_secondary_pubkey_hash ), secondary_pubkey_hash );

               if( public_key_hashes.size( ) > 2 )
               {
                  string tertiary_pubkey_hash( hex_encode( base64::decode( public_key_hashes[ 2 ] ) ) );

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

            if( unix_time_value < last_unix_time_value )
               throw runtime_error( "unexpected unix time value less than prior '"
                + blockchain_identity + "' value at height " + to_string( block_height - 1 ) );

            set_session_variable(
             get_special_var_name( e_special_var_blockchain_unix_time ), next_attribute );
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

mutex& get_mutex_for_ciyam_core_files( )
{
   return g_mutex;
}

struct core_file_data::impl
{
   string type;

   map< string, string > headers;
   multimap< string, string > attributes;
};

core_file_data::core_file_data( const string& raw_data )
{
   p_impl = new impl;

   vector< string > lines;

   split( raw_data, lines, '\n' );

   for( size_t i = 0; i < lines.size( ); i++ )
   {
      string next_line( lines[ i ] );

      string::size_type pos = next_line.find( ':' );

      if( i == 0 )
      {
         if( pos == string::npos )
            throw runtime_error( "unexpected core_file_data type missing in '" + next_line + "'" );

         p_impl->type = next_line.substr( 0, pos );

         next_line.erase( 0, pos + 1 );

         vector< string > header_attributes;
         split( next_line, header_attributes );

         for( size_t j = 0; j < header_attributes.size( ); j++ )
         {
            string next_header( header_attributes[ j ] );

            pos = next_header.find( '=' );

            if( pos == string::npos )
               throw runtime_error( "unexpected invalid core_file_data header attribute '" + next_header + "'" );

            p_impl->headers.insert( make_pair( next_header.substr( 0, pos ), next_header.substr( pos + 1 ) ) );
         }
      }
      else
      {
         if( pos == string::npos )
            throw runtime_error( "unexpected core_file_data attribute type missing in '" + next_line + "'" );

         p_impl->attributes.insert( make_pair( next_line.substr( 0, pos ), next_line.substr( pos + 1 ) ) );
      }
   }
}

core_file_data::~core_file_data( )
{
   delete p_impl;
}

string core_file_data::get_type( ) const
{
   return p_impl->type;
}

string core_file_data::get_header( const string& header_key ) const
{
   string retval;

   if( p_impl->headers.count( header_key ) )
      retval = p_impl->headers[ header_key ];

   return retval;
}

string core_file_data::get_attribute( const string& attribute_key ) const
{
   string retval;

   string::size_type pos = attribute_key.find( ':' );

   multimap< string, string >::const_iterator ci =
    p_impl->attributes.lower_bound( attribute_key.substr( 0, pos ) );

   while( ci != p_impl->attributes.end( ) )
   {
      if( ci->first != attribute_key.substr( 0, pos ) )
         break;

      // NOTE: If more than one matching attribute then separate with LFs.
      if( !retval.empty( ) )
         retval += '\n';

      retval += ci->second;

      ++ci;
   }

   return retval;
}

void verify_core_file( const string& content, bool check_dependents )
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
            verify_block( content.substr( pos + 1 ), check_dependents );
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
 const string& repository, const string& hash, bool store_as_blob )
{
   string retval, local_hash, local_public_key, master_public_key;

   if( fetch_repository_entry_record( repository, hash,
    local_hash, local_public_key, master_public_key, false ) && has_file( local_hash ) )
      retval = create_peer_repository_entry_pull_info( repository,
       hash, local_hash, local_public_key, master_public_key, store_as_blob );

   return retval;
}

string create_peer_repository_entry_pull_info(
 const string& repository, const string& hash, const string& local_hash,
 const string& local_public_key, const string& master_public_key, bool store_as_blob )
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
 string* p_pub_key, bool store_as_blob, bool is_for_testing )
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

   unique_ptr< private_key > up_priv_key;

   if( is_for_testing )
      up_priv_key.reset( new private_key( sha256( c_dummy ).get_digest_as_string( ) ) );
   else
      // NOTE: The first nibble is zeroed out to ensure that the hash value is always valid to use
      // as a Bitcoin address "secret" (as the range of its EC is smaller than the full 256 bits).
      up_priv_key.reset(
       new private_key( "0" + sha256( file_hash + password ).get_digest_as_string( ).substr( 1 ) ) );

   if( p_pub_key )
      *p_pub_key = up_priv_key->get_public( );

   file_data += c_file_repository_public_key_line_prefix;
   file_data += up_priv_key->get_public( true, true );

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

void decrypt_pulled_peer_file( const string& dest_hash,
 const string& src_hash, const string& password, bool is_for_testing,
 const string* p_file_data, const string* p_encrypted_hash, const string* p_archive )
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

void decrypt_pulled_peer_file( const string& dest_hash,
 const string& src_hash, const string& password, const string& public_key_in_hex,
 bool is_for_testing, const string* p_file_data, const string* p_encrypted_hash, const string* p_archive )
{
#ifndef SSL_SUPPORT
   throw runtime_error( "decrypt_pulled_peer_file requires SSL support" );
#else
   public_key pub_key( public_key_in_hex );

   unique_ptr< private_key > up_priv_key;

   if( is_for_testing )
      up_priv_key.reset( new private_key( sha256( c_dummy ).get_digest_as_string( ) ) );
   else
      // NOTE: The first nibble is zeroed out to ensure that the hash value is always valid to use
      // as a Bitcoin address "secret" (as the range of its EC is smaller than the full 256 bits).
      up_priv_key.reset(
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

   string stream_cipher( get_session_variable(
    get_special_var_name( e_special_var_blockchain_stream_cipher ) ) );

   if( stream_cipher.empty( ) )
   {
      if( is_for_testing )
         stream_cipher = stream_cipher_string( );
      else
         throw runtime_error( "unexpected missing @stream_cipher in decrypt_pulled_peer_file" );
   }

   stringstream ss( file_data );

   crypt_stream( ss, up_priv_key->construct_shared( pub_key ), stream_cipher_value( stream_cipher ) );

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
   {
      if( !p_archive )
         create_raw_file( raw_file_data, true, 0, 0, dest_hash.c_str( ) );
      else
         create_raw_file_in_archive( *p_archive, dest_hash, raw_file_data );
   }

   if( !is_encrypted && ( hash != dest_hash ) )
   {
      delete_file( hash );
      throw runtime_error( "failed to decrypt (bad password or incorrect content)" );
   }
#endif
}
