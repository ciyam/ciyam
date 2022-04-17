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
#  include <csignal>
#  include <cstdlib>
#  include <set>
#  include <deque>
#  include <memory>
#  include <sstream>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#define CIYAM_BASE_IMPL

#include "peer_session.h"

#include "base64.h"
#include "config.h"
#include "sha256.h"
#include "sockets.h"
#include "threads.h"
#include "utilities.h"
#include "date_time.h"
#include "ciyam_base.h"
#include "ciyam_files.h"
#ifdef SSL_SUPPORT
#  include "crypto_keys.h"
#endif
#include "crypt_stream.h"
#include "ciyam_session.h"
#include "ciyam_strings.h"
#include "command_parser.h"
#include "ciyam_constants.h"
#include "ciyam_variables.h"
#include "command_handler.h"
#include "ciyam_core_files.h"
#include "command_processor.h"

//#define DEBUG
//#define USE_THROTTLING
//#define DEBUG_PEER_HANDSHAKE

using namespace std;

extern size_t g_active_listeners;

extern volatile sig_atomic_t g_server_shutdown;

namespace
{

#include "ciyam_constants.h"

#include "peer_session.cmh"

#include "trace_progress.cpp"

mutex g_mutex;

const char c_blob_separator = '&';

const char c_repository_suffix = '!';

const char c_error_message_prefix = '#';
const char c_ignore_all_puts_prefix = '$';

const char* const c_hello = "hello";
const char* const c_bc_prefix = "bc.";
const char* const c_blk_suffix = ".blk";
const char* const c_dat_suffix = ".dat";
const char* const c_key_suffix = ".key";
const char* const c_pub_suffix = ".pub";
const char* const c_sig_suffix = ".sig";
const char* const c_zenith_suffix = ".zenith";

const char* const c_dummy_peer_tag = "peer";
const char* const c_dummy_support_tag = "support";

const int c_accept_timeout = 250;
const int c_max_line_length = 500;

const int c_max_num_for_support = 9;
const int c_min_block_wait_passes = 8;

const size_t c_max_pubkey_size = 256;
const size_t c_max_greeting_size = 256;
const size_t c_max_put_blob_size = 256;

const size_t c_num_signature_lines = 256;
const size_t c_num_public_key_lines = 256;

const size_t c_num_base64_key_chars = 44;
const size_t c_key_pair_separator_pos = 44;

const size_t c_sleep_time = 250;

const size_t c_initial_timeout = 8000;
const size_t c_request_timeout = 3000;
const size_t c_support_timeout = 3000;

const size_t c_main_session_sleep_time = 150;
const size_t c_support_session_sleep_time = 100;
const size_t c_support_session_sleep_repeats = 10;

#ifdef USE_THROTTLING
const size_t c_request_throttle_sleep_time = 250;
#endif

enum op
{
   e_op_chk,
   e_op_pip,
   e_op_init
};

enum peer_state
{
   e_peer_state_invalid,
   e_peer_state_initiator,
   e_peer_state_responder,
   e_peer_state_waiting_for_get,
   e_peer_state_waiting_for_put,
   e_peer_state_waiting_for_get_or_put
};

enum peer_trust_level
{
   e_peer_trust_level_none,
   e_peer_trust_level_normal
};

size_t g_num_peers = 0;

bool has_max_peers( )
{
   guard g( g_mutex );

   return g_num_peers >= get_max_peers( );
}

inline void issue_error( const string& message, bool possibly_expected = false )
{
   TRACE_LOG( ( possibly_expected ? TRACE_SESSIONS : TRACE_ANYTHING ), string( "peer session error: " ) + message );
}

inline void issue_warning( const string& message )
{
   TRACE_LOG( TRACE_SESSIONS, string( "peer session warning: " ) + message );
}

void increment_active_listeners( )
{
   guard g( g_mutex );
   ++g_active_listeners;
}

void decrement_active_listeners( )
{
   guard g( g_mutex );
   --g_active_listeners;
}

map< string, size_t > g_blockchain_tree_item;

size_t get_blockchain_tree_item( const string& blockchain )
{
   guard g( g_mutex );

   return g_blockchain_tree_item[ blockchain ];
}

void set_blockchain_tree_item( const string& blockchain, size_t num )
{
   guard g( g_mutex );

   g_blockchain_tree_item[ blockchain ] = num;
}

void add_to_blockchain_tree_item( const string& blockchain, size_t num_to_add )
{
   guard g( g_mutex );

   g_blockchain_tree_item[ blockchain ] += num_to_add;
}

string get_hello_data( string& hello_hash )
{
   string data( c_file_type_str_blob );
   data += string( c_hello );

   hello_hash = sha256( data ).get_digest_as_string( );

   return data;
}

void process_core_file( const string& hash, const string& blockchain )
{
   guard g( g_mutex );

   TRACE_LOG( TRACE_PEER_OPS, "(process_core_file) hash: " + hash );

   string::size_type pos = hash.find( ':' );

   string file_info( file_type_info( hash.substr( 0, pos ) ) );

   vector< string > info_parts;
   split( file_info, info_parts, ' ' );

   // NOTE: A core file will return three parts in the form of: <type> <hash> <core_type>
   // (as non-core files don't have a "core type" only two parts will be found for them).
   if( info_parts.size( ) == 3 && get_hash_tags( hash.substr( 0, pos ) ).empty( ) )
   {
      string core_type( info_parts[ 2 ] );

      if( is_block( core_type ) )
      {
         try
         {
            vector< pair< string, string > > extras;

            string block_content( construct_blob_for_block_content( extract_file( hash, "" ) ) );

            verify_core_file( block_content, false, &extras );

            if( !extras.empty( ) )
               create_raw_file_with_extras( "", extras );

            string block_height( get_session_variable(
             get_special_var_name( e_special_var_blockchain_height ) ) );

            string primary_pubkey_hash( get_session_variable(
             get_special_var_name( e_special_var_blockchain_primary_pubkey_hash ) ) );

            if( !primary_pubkey_hash.empty( ) )
            {
               if( block_height == "0"
                && primary_pubkey_hash.find( blockchain.substr( strlen( c_bc_prefix ) ) ) != 0 )
                  throw runtime_error( "invalid genesis primary public key hash '"
                   + primary_pubkey_hash + "' for blockchain '" + blockchain + "'" );

               if( !has_file( primary_pubkey_hash ) )
                  add_peer_file_hash_for_get( primary_pubkey_hash );

               string file_tag( blockchain + '.' + block_height + c_blk_suffix );

               tag_file( file_tag, hash );
            }

            set_session_variable(
             get_special_var_name( e_special_var_blockchain_height ), "" );

            string secondary_pubkey_hash( get_session_variable(
             get_special_var_name( e_special_var_blockchain_secondary_pubkey_hash ) ) );

            if( !secondary_pubkey_hash.empty( ) && !has_file( secondary_pubkey_hash ) )
               add_peer_file_hash_for_get( secondary_pubkey_hash );

            string signature_file_hash( get_session_variable(
             get_special_var_name( e_special_var_blockchain_signature_file_hash ) ) );

            if( !signature_file_hash.empty( ) && !has_file( signature_file_hash ) )
               add_peer_file_hash_for_get( signature_file_hash );
         }
         catch( ... )
         {
            delete_file( hash.substr( 0, pos ), false );
            throw;
         }
      }
      else
      {
         delete_file( hash.substr( 0, pos ), false );
         throw runtime_error( "unexpected core file type '" + core_type + "' found in process_core_file" );
      }
   }
}

#ifdef SSL_SUPPORT
void process_repository_file( const string& blockchain,
 const string& hash_info, bool is_test_session, const string* p_file_data = 0 )
{
   guard g( g_mutex );

   TRACE_LOG( TRACE_PEER_OPS, "(process_repository_file) hash_info: " + hash_info );

   string::size_type pos = hash_info.find( ':' );

   if( pos == string::npos )
      throw runtime_error( "unexpected missing public key for process_repository_file with: " + hash_info );

   string src_hash( hash_info.substr( 0, pos ) );
   string extra_info( hash_info.substr( pos + 1 ) );

   unsigned char type_and_extra = '\0';

   bool has_archive = false;
   bool was_extracted = false;

   string archive( replaced( blockchain, c_bc_prefix, "" ) );

   if( !get_session_variable( get_special_var_name(
    e_special_var_blockchain_archive_path ) ).empty( ) )
      has_archive = true;

   string file_data, file_content;

   if( p_file_data && !p_file_data->empty( ) )
   {
      file_data = *p_file_data;
      type_and_extra = file_data[ 0 ];

      file_content = file_data.substr( 1 );
   }
   else
   {
      was_extracted = true;
      file_content = extract_file( src_hash, "", 0, 0, &type_and_extra, 0, true );
   }

   pos = extra_info.find( ';' );

   if( pos != string::npos )
   {
      string hex_master;

      string hex_pub_key( extra_info.substr( 0, pos ) );
      string target_hash( extra_info.substr( pos + 1 ) );

      pos = hex_pub_key.find( '-' );

      if( pos != string::npos )
      {
         hex_master = hex_pub_key.substr( pos + 1 );
         hex_pub_key.erase( pos );
      }

      // NOTE: If this file is the result of a repository pull request
      // then when testing tag it with both the target hash and public
      // key (in hex) so it can be then detected by a standard session
      // to be decrypted. If not testing then if is not the owner will
      // store the repository entry otherwise will decrypt the initial
      // file content.
      if( is_test_session )
      {
         tag_file( '~' + hex_pub_key, src_hash );
         tag_file( '@' + target_hash, src_hash );
      }
      else
      {
         string dummy;

         bool is_blockchain_owner = !get_session_variable(
          get_special_var_name( e_special_var_blockchain_is_owner ) ).empty( );

         if( !is_blockchain_owner )
         {
            if( !fetch_repository_entry_record( target_hash, dummy, dummy, dummy, false ) )
               store_repository_entry_record( target_hash, src_hash, hex_pub_key, hex_master );

            if( file_data.empty( ) )
               file_data = ( char )type_and_extra + file_content;

            if( has_archive )
               create_raw_file_in_archive( archive, src_hash, file_data );
            else
               create_raw_file( file_data, true, 0, 0, src_hash.c_str( ), true, true );
         }
         else
         {
            string password;
            get_identity( password, false, true );

            decrypt_pulled_peer_file( target_hash, src_hash, password, hex_pub_key, false, p_file_data );

            string repo_hash( create_peer_repository_entry_push_info( target_hash, password ) );

            clear_key( password );

            if( !fetch_repository_entry_record( target_hash, dummy, dummy, dummy, false ) )
               store_repository_entry_record( target_hash, "", hex_master, hex_master );

            if( was_extracted )
               delete_file( src_hash );

            delete_file( repo_hash );
         }
      }
   }
   else
   {
      string dummy, local_hash;

      bool has_local_file = false;
      bool has_repo_entry = fetch_repository_entry_record( src_hash, local_hash, dummy, dummy, false );

      if( has_repo_entry && has_file( local_hash ) )
         has_local_file = true;

      if( has_repo_entry && has_local_file )
      {
         if( was_extracted )
            delete_file( src_hash );
      }
      else
      {
         public_key pub_key( extra_info );

         auto_ptr< private_key > ap_priv_key;

         if( !is_test_session )
         {
            string password;
            get_identity( password, false, true );

            // NOTE: The first nibble is zeroed out to ensure that the hash value is always valid to use
            // as a Bitcoin address "secret" (as the range of its EC is smaller than the full 256 bits).
            ap_priv_key.reset( new private_key( "0"
             + sha256( src_hash + password ).get_digest_as_string( ).substr( 1 ) ) );

            clear_key( password );
         }
         else
         {
            // NOTE: If using interactive testing the file may not be encrypted so to ensure
            // the file system doesn't get confused flag the file as being encrypted anyway.
            type_and_extra |= c_file_type_char_encrypted;

            ap_priv_key.reset( new private_key( sha256( c_dummy ).get_digest_as_string( ) ) );
         }

         stringstream ss( file_content );
         crypt_stream( ss, ap_priv_key->construct_shared( pub_key ) );

         file_data = ( char )type_and_extra + ss.str( );

         if( was_extracted )
            delete_file( src_hash );

         if( !has_archive )
            local_hash = create_raw_file( file_data, false, 0, 0, 0, false );
         else
            create_raw_file_in_archive( archive, "", file_data, &local_hash );

         store_repository_entry_record( src_hash, local_hash, ap_priv_key->get_public( ), pub_key.get_public( ) );
      }
   }
}

string get_file_hash_from_put_data( const string& encoded_master,
 const string& encoded_pubkey, const string& encoded_source_hash, const string& encoded_target_hash )
{
   string retval;

   try
   {
      if( !encoded_master.empty( ) )
         base64::validate( encoded_master );

      base64::validate( encoded_pubkey );
      base64::validate( encoded_source_hash );

      // NOTE: Construct a public key object for the purpose of validation.
      public_key pubkey( encoded_pubkey, true );

      string extra( hex_encode( base64::decode( encoded_pubkey ) ) );

      if( !encoded_master.empty( ) )
      {
         public_key pubkey( encoded_master, true );
         extra += '-' + hex_encode( base64::decode( encoded_master ) );
      }

      if( !encoded_target_hash.empty( ) )
      {
         base64::validate( encoded_target_hash );
         extra += ';' + hex_encode( base64::decode( encoded_target_hash ) );
      }

      retval = hex_encode( base64::decode( encoded_source_hash ) ) + ':' + extra + c_repository_suffix;
   }
   catch( ... )
   {
      // FUTURE: Perhaps add a trace here so specific errors aren't being completely ignored.
   }

   return retval;
}
#endif

size_t process_put_file( const string& blockchain,
 const string& file_data, bool check_for_supporters, bool is_test_session, set< string >& list_items_to_ignore )
{
   vector< string > blobs;
   split( file_data, blobs, c_blob_separator );

   size_t num_skipped = 0;

   for( size_t i = 0; i < blobs.size( ); i++ )
   {
      string next_blob( blobs[ i ] );

      vector< string > lines;
      split( next_blob, lines, '\n' );

      bool okay = false;

      if( lines.size( ) >= 3 )
      {
         string::size_type pos = lines[ 0 ].find( c_file_repository_meta_data_line_prefix );

         if( pos == 0 )
         {
            string meta_data_info(
             lines[ 0 ].substr( strlen( c_file_repository_meta_data_line_prefix ) ) );

            if( meta_data_info == c_file_repository_meta_data_info_type_raw )
            {
               if( lines[ 1 ].find( c_file_repository_public_key_line_prefix ) == 0 )
               {
                  string master_key;

                  string public_key(
                   lines[ 1 ].substr( strlen( c_file_repository_public_key_line_prefix ) ) );

                  pos = public_key.find( '-' );

                  if( pos != string::npos )
                  {
                     master_key = public_key.substr( pos + 1 );
                     public_key.erase( pos );
                  }

                  if( lines[ 2 ].find( c_file_repository_source_hash_line_prefix ) == 0 )
                  {
                     string source_hash(
                      lines[ 2 ].substr( strlen( c_file_repository_source_hash_line_prefix ) ) );

                     string target_hash;

                     if( lines.size( ) > 3 && lines[ 3 ].find( c_file_repository_target_hash_line_prefix ) == 0 )
                        target_hash = lines[ 3 ].substr( strlen( c_file_repository_target_hash_line_prefix ) );

#ifndef SSL_SUPPORT
                     okay = true;
#else
                     string hash_info( get_file_hash_from_put_data( master_key, public_key, source_hash, target_hash ) );

                     if( !hash_info.empty( ) )
                     {
                        okay = true;
                        pos = hash_info.find( ':' );

                        if( !has_file( hash_info.substr( 0, pos ) ) )
                        {
                           if( target_hash.empty( ) )
                              target_hash = hash_info.substr( 0, pos );
                           else
                              target_hash = hex_encode( base64::decode( target_hash ) );

                           if( has_file( target_hash ) )
                              ++num_skipped;
                           else
                           {
                              list_items_to_ignore.insert( target_hash );

                              // NOTE: Pull information target (if had already been queued) will be removed.
                              add_peer_file_hash_for_get(
                               hash_info, check_for_supporters, false, target_hash.empty( ) ? 0 : &target_hash );
                           }
                        }
                        else
                           process_repository_file( blockchain,
                            hash_info.substr( 0, hash_info.length( ) - 1 ), is_test_session );
                     }
#endif
                  }
               }
            }
         }

         if( !okay )
            throw runtime_error( "invalid file content for put" );
      }
   }

   return num_skipped;
}

void process_list_items( const string& hash, bool recurse,
 string* p_blob_data = 0, size_t* p_num_items_found = 0, set< string >* p_list_items_to_ignore = 0 )
{
   string all_list_items( extract_file( hash, "" ) );

   vector< string > list_items;
   split( all_list_items, list_items, '\n' );

   string file_data( c_file_type_str_blob );

   bool new_blob = false;

   if( !p_blob_data )
   {
      new_blob = true;
      p_blob_data = &file_data;
   }

   size_t max_blob_file_data = get_files_area_item_max_size( ) - c_max_put_blob_size;

   string blockchain_is_owner_name( get_special_var_name( e_special_var_blockchain_is_owner ) );
   string blockchain_is_fetching_name( get_special_var_name( e_special_var_blockchain_is_fetching ) );
   string blockchain_both_are_owners_name( get_special_var_name( e_special_var_blockchain_both_are_owners ) );

   string first_hash_name( get_special_var_name( e_special_var_hash ) );
   string first_hash_to_get( get_session_variable( first_hash_name ) );

   for( size_t i = 0; i < list_items.size( ); i++ )
   {
      if( p_blob_data->size( ) >= max_blob_file_data )
      {
         string file_hash( create_raw_file( *p_blob_data ) );

         set_session_variable( file_hash, c_true_value );
         add_peer_file_hash_for_put( file_hash );

         *p_blob_data = string( c_file_type_str_blob );
      }

      string next_item( list_items[ i ] );

      if( !next_item.empty( ) )
      {
         string next_hash( next_item.substr( 0, next_item.find( ' ' ) ) );

         if( p_list_items_to_ignore && p_list_items_to_ignore->count( next_hash ) )
            continue;

         string local_hash, local_public_key, master_public_key;

         if( next_hash == first_hash_to_get )
         {
            first_hash_to_get.erase( );
            set_session_variable( first_hash_name, "" );
         }

         if( !has_file( next_hash ) )
         {
            if( !fetch_repository_entry_record( next_hash,
             local_hash, local_public_key, master_public_key, false ) )
               add_peer_file_hash_for_get( next_hash );
            else if( first_hash_to_get.empty( ) && ( local_public_key != master_public_key ) )
            {
               if( get_session_variable( blockchain_is_fetching_name ).empty( )
                && get_session_variable( blockchain_both_are_owners_name ).empty( ) )
               {
                  if( p_blob_data->size( ) > 1 )
                     *p_blob_data += c_blob_separator;

                  *p_blob_data += create_peer_repository_entry_pull_info(
                   next_hash, local_hash, local_public_key, master_public_key, false );
               }
            }
         }
         else if( recurse && is_list_file( next_hash ) )
         {
            if( p_num_items_found )
               ++( *p_num_items_found );

            process_list_items( next_hash, recurse, p_blob_data, p_num_items_found, p_list_items_to_ignore );

            // NOTE: Recursive processing may have already located this.
            first_hash_to_get = get_session_variable( first_hash_name );
         }
         else if( recurse )
         {
            bool has_repository_entry = false;
            bool put_info_and_store_repository_entry = false;

            if( p_num_items_found )
               ++( *p_num_items_found );

            if( fetch_repository_entry_record( next_hash,
             local_hash, local_public_key, master_public_key, false ) )
            {
               has_repository_entry = true;

               if( local_public_key == master_public_key )
                  put_info_and_store_repository_entry = true;
            }
            else if( !get_session_variable( blockchain_is_owner_name ).empty( ) )
               put_info_and_store_repository_entry = true;

            if( first_hash_to_get.empty( ) && put_info_and_store_repository_entry )
            {
               if( get_session_variable( blockchain_is_fetching_name ).empty( )
                && get_session_variable( blockchain_both_are_owners_name ).empty( ) )
               {
                  if( local_hash.empty( ) || !has_file( local_hash ) )
                  {
                     string password;
                     get_identity( password, false, true );

                     if( p_blob_data->size( ) > 1 )
                        *p_blob_data += c_blob_separator;

                     *p_blob_data += create_peer_repository_entry_push_info( next_hash, password, &master_public_key, false );

                     clear_key( password );
                  }

                  if( !has_repository_entry )
                     store_repository_entry_record( next_hash, "", master_public_key, master_public_key );
               }
            }
         }
      }
   }

   if( new_blob && p_blob_data->size( ) > 1 )
   {
      string file_hash( create_raw_file( *p_blob_data ) );

      set_session_variable( file_hash, c_true_value );
      add_peer_file_hash_for_put( file_hash );
   }
}

void process_data_file( const string& blockchain, const string& hash, size_t height, size_t* p_num_items_found = 0 )
{
   guard g( g_mutex );

   TRACE_LOG( TRACE_PEER_OPS, "(process_data_file) hash: " + hash + " height: " + to_string( height ) );

   try
   {
      string block_tag( blockchain + '.' + to_string( height ) + c_blk_suffix );

      string block_hash( tag_file_hash( block_tag ) );

      string block_content( construct_blob_for_block_content( extract_file( block_hash, "" ) ) );

      verify_core_file( block_content, false );

      string data_tag( blockchain + '.' );

      data_tag += to_string( height );

      data_tag += c_dat_suffix;

      tag_file( data_tag, hash );

      bool tag_new_zenith = false;
      bool wait_to_tag_zenith = false;

      string first_hash_name( get_special_var_name( e_special_var_hash ) );
      string first_hash_to_get( get_session_variable( first_hash_name ) );

      bool is_blockchain_owner = !get_session_variable(
       get_special_var_name( e_special_var_blockchain_is_owner ) ).empty( );

      string tree_root_hash( get_session_variable(
       get_special_var_name( e_special_var_blockchain_tree_root_hash ) ) );

      string blockchain_height_processed( get_session_variable(
       get_special_var_name( e_special_var_blockchain_height_processed ) ) );

      if( blockchain_height_processed.empty( )
       || from_string< size_t >( blockchain_height_processed ) < height )
      {
         if( !tree_root_hash.empty( ) )
         {
            if( !has_file( tree_root_hash ) )
            {
               wait_to_tag_zenith = true;
               add_peer_file_hash_for_get( tree_root_hash );
            }
            else
            {
               if( is_blockchain_owner )
                  tag_file( c_ciyam_tag, tree_root_hash );

               if( first_hash_to_get == tree_root_hash )
                  set_session_variable( first_hash_name, "" );

               process_list_items( tree_root_hash, true, 0, p_num_items_found );

               if( top_next_peer_file_hash_to_get( ).empty( ) )
                  tag_new_zenith = true;
               else
               {
                  wait_to_tag_zenith = true;

                  set_session_variable(
                   get_special_var_name( e_special_var_list_hash ), tree_root_hash );
               }

               set_session_variable(
                get_special_var_name( e_special_var_blockchain_tree_root_hash ), "" );
            }
         }
         else
            tag_new_zenith = true;

         if( tag_new_zenith )
         {
            string zenith_height( get_session_variable(
             get_special_var_name( e_special_var_blockchain_zenith_height ) ) );

            if( zenith_height.empty( )
             || ( height > from_string< size_t >( zenith_height ) ) )
            {
               tag_file( blockchain + c_zenith_suffix, block_hash );

               TRACE_LOG( TRACE_PEER_OPS, "--- new zenith hash: "
                + block_hash + " height: " + to_string( height ) );

               set_session_variable(
                get_special_var_name( e_special_var_blockchain_zenith_height ), to_string( height ) );
            }
         }
         else if( wait_to_tag_zenith )
            set_session_variable(
             get_special_var_name( e_special_var_blockchain_zenith_hash ), block_hash );

         set_session_variable(
          get_special_var_name( e_special_var_blockchain_height_processed ), to_string( height ) );
      }

      set_session_variable(
       get_special_var_name( e_special_var_blockchain_height ), "" );

      set_session_variable(
       get_special_var_name( e_special_var_blockchain_data_file_hash ), "" );

      set_session_variable(
       get_special_var_name( e_special_var_blockchain_primary_pubkey_hash ), "" );

      set_session_variable(
       get_special_var_name( e_special_var_blockchain_signature_file_hash ), "" );

      set_session_variable(
       get_special_var_name( e_special_var_blockchain_secondary_pubkey_hash ), "" );
   }
   catch( ... )
   {
      delete_file( hash );
      throw;
   }
}

void process_signature_file( const string& blockchain, const string& hash, size_t height )
{
   guard g( g_mutex );

   TRACE_LOG( TRACE_PEER_OPS, "(process_signature_file) hash: " + hash + " height: " + to_string( height ) );

   if( !height )
      throw runtime_error( "invalid zero height for process_signature_file" );

   try
   {
      string block_tag( blockchain + '.' + to_string( height ) + c_blk_suffix );

      string block_content(
       construct_blob_for_block_content( extract_file( tag_file_hash( block_tag ), "" ) ) );

      verify_core_file( block_content, true );

      string block_height( get_session_variable(
       get_special_var_name( e_special_var_blockchain_height ) ) );

      if( !block_height.empty( ) && ( block_height != to_string( height ) ) )
         throw runtime_error( "specified height does not match that found in the block itself (sig)" );

      string signature_tag( blockchain + '.' );

      signature_tag += to_string( height - 1 );

      signature_tag += c_sig_suffix;

      tag_file( signature_tag, hash );

      set_session_variable(
       get_special_var_name( e_special_var_blockchain_height ), "" );

      set_session_variable(
       get_special_var_name( e_special_var_blockchain_primary_pubkey_hash ), "" );

      set_session_variable(
       get_special_var_name( e_special_var_blockchain_signature_file_hash ), "" );

      set_session_variable(
       get_special_var_name( e_special_var_blockchain_secondary_pubkey_hash ), "" );
   }
   catch( ... )
   {
      delete_file( hash );
      throw;
   }
}

void process_public_key_file( const string& blockchain, const string& hash, size_t height, bool is_primary = true )
{
   guard g( g_mutex );

   string pubkey_tag( blockchain );

   if( is_primary )
      pubkey_tag += ".p";
   else
      pubkey_tag += ".s";

   pubkey_tag += to_string( height );

   pubkey_tag += c_pub_suffix;

   tag_file( pubkey_tag, hash );

   string zenith_height_name(
    get_special_var_name( e_special_var_blockchain_zenith_height ) );

   string zenith_height( get_session_variable( zenith_height_name ) );

   if( !height && !is_primary && zenith_height.empty( ) )
   {
      string block_hash( tag_file_hash(
       blockchain + '.' + to_string( height ) + c_blk_suffix ) );

      tag_file( blockchain + c_zenith_suffix, block_hash );

      TRACE_LOG( TRACE_PEER_OPS, "::: new zenith hash: "
       + block_hash + " height: " + to_string( height ) );

      set_session_variable( zenith_height_name, to_string( height ) );
   }

   if( is_primary )
      set_session_variable(
       get_special_var_name( e_special_var_blockchain_primary_pubkey_hash ), "" );
   else
      set_session_variable(
       get_special_var_name( e_special_var_blockchain_secondary_pubkey_hash ), "" );
}

void validate_signature_file( const string& file_data )
{
   if( file_data.empty( ) )
      throw runtime_error( "unexpected empty file data in 'validate_signature_file'" );

   string content( file_data.substr( 1 ) );

   if( content.empty( ) )
      throw runtime_error( "unexpected empty file content in 'validate_signature_file'" );

   vector< string > lines;
   split( content, lines, '\n' );

   if( !lines.empty( ) && lines[ lines.size( ) - 1 ].empty( ) )
      lines.pop_back( );

   if( lines.size( ) != c_num_signature_lines )
      throw runtime_error( "unexpected number of lines is " + to_string( lines.size( ) )
       + " rather than " + to_string( c_num_public_key_lines ) + " in  'validate_signature_file'" );

   for( size_t i = 0; i < c_num_signature_lines; i++ )
   {
      string next_line( lines[ i ] );

      bool is_invalid = false;

      if( next_line.size( ) != c_num_base64_key_chars )
         is_invalid = true;
      else
      {
         bool rc = false;

         base64::validate( next_line, &rc );

         if( !rc )
            is_invalid = true;
      }

      if( is_invalid )
         throw runtime_error( "unexpected invalid signature line: " + next_line );
   }
}

void validate_public_key_file( const string& file_data )
{
   if( file_data.empty( ) )
      throw runtime_error( "unexpected empty file data in 'validate_public_key_file'" );

   string content( file_data.substr( 1 ) );

   if( content.empty( ) )
      throw runtime_error( "unexpected empty file content in 'validate_public_key_file'" );

   vector< string > lines;
   split( content, lines, '\n' );

   if( !lines.empty( ) && lines[ lines.size( ) - 1 ].empty( ) )
      lines.pop_back( );

   if( lines.size( ) != c_num_public_key_lines )
      throw runtime_error( "unexpected number of lines is " + to_string( lines.size( ) )
       + " rather than " + to_string( c_num_public_key_lines ) + " in  'validate_public_key_file'" );

   for( size_t i = 0; i < c_num_public_key_lines; i++ )
   {
      string next_line( lines[ i ] );

      string::size_type pos = next_line.find( ' ' );

      bool is_invalid = false;

      if( ( pos == string::npos ) || ( pos != c_key_pair_separator_pos ) )
         is_invalid = true;
      else
      {
         string pair_0( next_line.substr( 0, pos ) );
         string pair_1( next_line.substr( pos + 1 ) );

         if( ( pair_0.size( ) != c_num_base64_key_chars )
          || ( pair_1.size( ) != c_num_base64_key_chars ) )
            is_invalid = true;
         else
         {
            bool rc = 0;

            base64::validate( pair_0, &rc );

            if( !rc )
               is_invalid = true;
            else
            {
               base64::validate( pair_1, &rc );

               if( !rc )
                  is_invalid = true;
            }
         }
      }

      if( is_invalid )
         throw runtime_error( "unexpected invalid public key pair line: " + next_line );
   }
}

bool process_block_for_height( const string& blockchain,
 const string& hash, size_t height, size_t* p_num_items_found = 0 )
{
   bool retval = false;

   TRACE_LOG( TRACE_PEER_OPS, "(process_block_for_height) hash: " + hash + " height: " + to_string( height ) );

   string block_content(
    construct_blob_for_block_content( extract_file( hash, "" ) ) );

   verify_core_file( block_content, false );

   string block_height( get_session_variable(
    get_special_var_name( e_special_var_blockchain_height ) ) );

   if( !block_height.empty( ) && ( block_height != to_string( height ) ) )
      throw runtime_error( "specified height does not match that found in the block itself (blk)" );
   else
   {
      bool waiting_for_pubkey = false;
      bool waiting_for_signature = false;

      string primary_pubkey_hash( get_session_variable(
       get_special_var_name( e_special_var_blockchain_primary_pubkey_hash ) ) );

      if( !primary_pubkey_hash.empty( ) )
      {
         if( !height
          && primary_pubkey_hash.find( blockchain.substr( strlen( c_bc_prefix ) ) ) != 0 )
            throw runtime_error( "invalid primary public key hash '"
             + primary_pubkey_hash + "' for blockchain '" + blockchain + "'" );

         if( !has_file( primary_pubkey_hash ) )
         {
            waiting_for_pubkey = true;
            add_peer_file_hash_for_get( primary_pubkey_hash );
         }
         else
            process_public_key_file( blockchain, primary_pubkey_hash, height );
      }

      string secondary_pubkey_hash( get_session_variable(
       get_special_var_name( e_special_var_blockchain_secondary_pubkey_hash ) ) );

      if( !secondary_pubkey_hash.empty( ) )
      {
         if( !has_file( secondary_pubkey_hash ) )
            add_peer_file_hash_for_get( secondary_pubkey_hash );
         else
            process_public_key_file( blockchain, secondary_pubkey_hash, height, false );
      }

      string signature_file_hash( get_session_variable(
       get_special_var_name( e_special_var_blockchain_signature_file_hash ) ) );

      if( !signature_file_hash.empty( ) )
      {
         if( !has_file( signature_file_hash ) )
         {
            waiting_for_signature = true;
            add_peer_file_hash_for_get( signature_file_hash );
         }
         else if( !waiting_for_pubkey )
            process_signature_file( blockchain, signature_file_hash, height );
      }

      string data_file_hash( get_session_variable(
       get_special_var_name( e_special_var_blockchain_data_file_hash ) ) );

      if( !data_file_hash.empty( ) )
      {
         string first_hash_name( get_special_var_name( e_special_var_hash ) );
         string first_hash_to_get( get_session_variable( first_hash_name ) );

         if( ( first_hash_to_get == data_file_hash )
          || ( first_hash_to_get == primary_pubkey_hash )
          || ( first_hash_to_get == signature_file_hash ) )
            set_session_variable( first_hash_name, "" );

         if( !has_file( data_file_hash ) )
            add_peer_file_hash_for_get( data_file_hash );
         else if( !waiting_for_pubkey && !waiting_for_signature )
         {
            retval = true;

            if( !first_hash_to_get.empty( ) )
               set_system_variable( c_ignore_all_puts_prefix + replaced( blockchain, c_bc_prefix, "" ), "" );

            process_data_file( blockchain, data_file_hash, height, p_num_items_found );
         }
      }
   }

   return retval;
}

bool get_block_height_from_tags( const string& blockchain, const string& hash, size_t& block_height )
{
   guard g( g_mutex );

   string all_tags( get_hash_tags( hash ) );

   vector< string > tags;

   if( !all_tags.empty( ) )
      split( all_tags, tags, '\n' );

   bool found = false;

   for( size_t i = 0; i < tags.size( ); i++ )
   {
      string next_tag( tags[ i ] );

      string::size_type pos = next_tag.find( blockchain );
      string::size_type spos = next_tag.find( c_blk_suffix );

      // NOTE: Determine the block height from its block tag.
      if( pos == 0 && spos != string::npos )
      {
         found = true;

         next_tag.erase( spos );

         block_height = from_string< size_t >( next_tag.substr( blockchain.length( ) + 1 ) );

         break;
      }
   }

   return found;
}

class socket_command_handler : public command_handler
{
   public:
#ifdef SSL_SUPPORT
   socket_command_handler(
    ssl_socket& socket, peer_state session_state, int64_t time_val,
    bool is_local, bool is_owner, const string& blockchain, bool is_for_support = false )
#else
   socket_command_handler(
    tcp_socket& socket, peer_state session_state, int64_t time_val,
    bool is_local, bool is_owner, const string& blockchain, bool is_for_support = false )
#endif
    :
    socket( socket ),
    is_local( is_local ),
    is_owner( is_owner ),
    time_val( time_val ),
    last_num_tree_item( 0 ),
    blockchain( blockchain ),
    blockchain_height( 0 ),
    blockchain_height_pending( 0 ),
    is_time_for_check( false ),
    has_identity_archive( false ),
    is_for_support( is_for_support ),
    session_state( session_state ),
    session_op_state( session_state ),
    session_trust_level( e_peer_trust_level_none )
   {
      had_usage = false;

      dtm_rcvd_not_found = dtm_sent_not_found
       = ( date_time::standard( ) - ( seconds )( c_peer_sleep_time / 1000.0 ) );

      is_responder = ( session_state == e_peer_state_responder );

      if( get_is_test_session( ) )
         want_to_do_op( e_op_init );

      if( !blockchain.empty( ) )
      {
         if( !is_for_support )
            set_blockchain_tree_item( blockchain, 0 );

         identity = replaced( blockchain, c_bc_prefix, "" );
         has_identity_archive = has_file_archive( identity );
      }

      last_issued_was_put = !is_responder;
   }

#ifdef SSL_SUPPORT
   ssl_socket& get_socket( ) { return socket; }
#else
   tcp_socket& get_socket( ) { return socket; }
#endif

   bool get_is_time_for_check( )
   {
      if( is_time_for_check )
         return true;

      int64_t current = unix_time_stamp( date_time::local( ) );

      if( current >= time_val )
         is_time_for_check = true;

      return is_time_for_check;
   }

   const string& get_last_command( ) { return last_command; }
   const string& get_next_command( ) { return next_command; }

   bool get_is_local( ) const { return is_local; }
   bool get_is_owner( ) const { return is_owner; }

   bool get_is_responder( ) const { return is_responder; }
   bool get_is_for_support( ) const { return is_for_support; }

   bool get_last_issued_was_put( ) const { return last_issued_was_put; }
   void set_last_issued_was_put( bool val ) { last_issued_was_put = val; }

   const string& get_identity( ) const { return identity; }
   const string& get_blockchain( ) const { return blockchain; }

   size_t get_blockchain_height( ) const { return blockchain_height; }

   void set_blockchain_height( size_t new_height )
   {
      blockchain_height_pending = blockchain_height = new_height;
   }

   set< string >& get_list_items_to_ignore( ) { return list_items_to_ignore; }

   bool get_is_test_session( ) const { return is_local && is_responder && blockchain.empty( ); }

   string& prior_put( ) { return prior_put_hash; }

   void get_hello( );
   void put_hello( );

   void get_file( const string& hash, string* p_file_data = 0 );
   void put_file( const string& hash );

   void pip_peer( const string& ip_address );

   void chk_file( const string& hash, string* p_response = 0 );

   bool want_to_do_op( op o ) const;

   void issue_cmd_for_peer( bool check_for_supporters );

   void set_dtm_sent_not_found( const date_time& dtm )
   {
      dtm_sent_not_found = dtm;
   }

   peer_state& state( ) { return session_state; }
   peer_state& op_state( ) { return session_op_state; }

   peer_trust_level& trust_level( ) { return session_trust_level; }

   void kill_session( )
   {
      if( !is_captured_session( ) )
         set_finished( );
      else if( !is_condemned_session( ) )
         condemn_this_session( );
   }

   private:
   void preprocess_command_and_args( string& str, const string& cmd_and_args );

   void postprocess_command_and_args( const string& cmd_and_args );

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      ( void )cmd_and_args;

      socket.write_line( string( c_response_error_prefix ) + "unknown command '" + command + "'", c_request_timeout );
      kill_session( );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      socket.write_line( string( c_response_error_prefix ) + "invalid command usage '" + cmd_and_args + "'", c_request_timeout );
      kill_session( );
   }

   void handle_command_response( const string& response, bool is_special );

#ifdef SSL_SUPPORT
   ssl_socket& socket;
#else
   tcp_socket& socket;
#endif

   bool is_local;
   bool is_owner;
   bool had_usage;

   bool is_responder;
   bool is_for_support;

   bool is_time_for_check;
   bool last_issued_was_put;
   bool has_identity_archive;

   set< string > list_items_to_ignore;

   int64_t time_val;

   size_t last_num_tree_item;

   string identity;
   string blockchain;

   size_t blockchain_height;
   size_t blockchain_height_pending;

   string last_command;
   string next_command;

   string prior_put_hash;

   date_time dtm_rcvd_not_found;
   date_time dtm_sent_not_found;

   peer_state session_state;
   peer_state session_op_state;

   peer_trust_level session_trust_level;
};

void socket_command_handler::get_hello( )
{
   last_issued_was_put = false;

   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   string data, hello_hash;
   data = get_hello_data( hello_hash );

   if( !has_file( hello_hash ) )
      create_raw_file( data, false );

   string temp_file_name( "~" + uuid( ).as_string( ) );

   socket.set_delay( );
   socket.write_line( string( c_cmd_peer_session_get ) + " " + hello_hash, c_request_timeout, p_progress );

   try
   {
      store_temp_file( temp_file_name, socket, p_progress );

      if( !temp_file_is_identical( temp_file_name, hello_hash ) )
      {
         file_remove( temp_file_name );
         throw runtime_error( "invalid get_hello" );
      }

      file_remove( temp_file_name );
   }
   catch( ... )
   {
      file_remove( temp_file_name );
      throw;
   }
}

void socket_command_handler::put_hello( )
{
   last_issued_was_put = true;

   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   string data, hello_hash;
   data = get_hello_data( hello_hash );

   if( !has_file( hello_hash ) )
      create_raw_file( data, false );

   socket.set_delay( );
   socket.write_line( string( c_cmd_peer_session_put ) + " " + hello_hash, c_request_timeout, p_progress );

   fetch_file( hello_hash, socket, p_progress );
}

void socket_command_handler::get_file( const string& hash_info, string* p_file_data )
{
   last_issued_was_put = false;

   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   string::size_type pos = hash_info.find( ':' );

   string hash( hash_info.substr( 0, pos ) );

   socket.set_delay( );
   socket.write_line( string( c_cmd_peer_session_get ) + " " + hash, c_request_timeout, p_progress );

   bool is_list = false;
   size_t num_bytes = 0;

   if( !p_file_data )
   {
      store_file( hash, socket, 0, p_progress, true, 0, true );

      is_list = is_list_file( hash );
      num_bytes = file_bytes( hash );
   }
   else
   {
      string file_data;

      store_file( hash, socket, 0, p_progress, true, 0, true, &file_data );

      *p_file_data = file_data;

      num_bytes = file_data.size( );

      if( is_list_file( file_data[ 0 ] ) )
      {
         is_list = true;

         if( !has_identity_archive )
            create_raw_file( file_data, true, 0, 0, 0, true, true );
         else
            create_raw_file_in_archive( identity, hash, file_data );
      }
   }

   // NOTE: If the file is a list then also need to get all of its items.
   if( is_list )
   {
      string tree_root_hash( get_session_variable(
       get_special_var_name( e_special_var_blockchain_tree_root_hash ) ) );

      if( is_owner && !tree_root_hash.empty( ) )
         tag_file( c_ciyam_tag, tree_root_hash );

      set_session_variable(
       get_special_var_name( e_special_var_blockchain_tree_root_hash ), "" );

      process_list_items( hash, false, 0, 0, &list_items_to_ignore );
   }

   increment_peer_files_downloaded( num_bytes );
}

void socket_command_handler::put_file( const string& hash )
{
   last_issued_was_put = true;

   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   socket.set_delay( );
   socket.write_line( string( c_cmd_peer_session_put ) + " " + hash, c_request_timeout, p_progress );

   fetch_file( hash, socket, p_progress );

   increment_peer_files_uploaded( file_bytes( hash ) );
}

void socket_command_handler::pip_peer( const string& ip_address )
{
   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   socket.set_no_delay( );
   socket.write_line( string( c_cmd_peer_session_pip ) + " " + ip_address, c_request_timeout, p_progress );

   string response;
   if( socket.read_line( response, c_request_timeout, 0, p_progress ) <= 0 )
   {
      string error;

      if( !socket.had_timeout( ) )
         error = "peer has terminated this connection";
      else
         error = "timeout occurred getting peer response";

      socket.close( );
      throw runtime_error( error );
   }
}

void socket_command_handler::chk_file( const string& hash_or_tag, string* p_response )
{
   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   string expected;

   socket.set_no_delay( );

   if( p_response )
      socket.write_line(
       string( c_cmd_peer_session_chk ) + " " + hash_or_tag, c_request_timeout, p_progress );
   else
   {
      string nonce( uuid( ).as_string( ) );

      expected = hash_with_nonce( hash_or_tag, nonce );

      socket.write_line( string( c_cmd_peer_session_chk )
       + " " + hash_or_tag + " " + nonce, c_request_timeout, p_progress );
   }

   string response;
   if( socket.read_line( response, c_request_timeout, 0, p_progress ) <= 0 )
   {
      string error;

      if( !socket.had_timeout( ) )
         error = "peer has terminated this connection";
      else
         error = "timeout occurred getting peer response";

      socket.close( );
      throw runtime_error( error );
   }

   if( response == string( c_response_not_found ) )
      response.erase( );

   if( p_response )
      *p_response = response;
   else if( response != expected )
      throw runtime_error( "unexpected invalid chk response: " + response );
}

bool socket_command_handler::want_to_do_op( op o ) const
{
   // NOTE: These statics are only to provide consistent behaviour for a
   // single interactive test session (and are not used by normal peers).
   static unsigned int chk_count = 0;
   static unsigned int pip_count = 0;

   bool retval = false;

   if( get_is_test_session( ) )
   {
      if( o == e_op_init )
         chk_count = pip_count = 0;
      else if( o == e_op_pip )
         retval = ( ++pip_count % 3 == 0 );
      else
         retval = ( ++chk_count % 5 == 0 );
   }
   else
   {
      string hash_to_get( top_next_peer_file_hash_to_get( ) );
      string hash_to_put( top_next_peer_file_hash_to_put( ) );

      if( o == e_op_chk )
      {
         if( hash_to_get.empty( ) && hash_to_put.empty( ) )
            retval = true;
      }
      else
      {
         if( o == e_op_pip )
         {
            if( hash_to_get.empty( ) && hash_to_put.empty( ) )
               retval = true;
         }
         else
         {
            // KLUDGE: For now just randomly decide (this should instead
            // be based upon the actual needs of the peer).
            retval = ( rand( ) % 10 == 0 );
         }
      }
   }

   return retval;
}

void socket_command_handler::issue_cmd_for_peer( bool check_for_supporters )
{
   // NOTE: If a prior put no longer exists locally then it is to be expected
   // it would not exist in the peer either.
   if( !prior_put( ).empty( ) && !has_file( prior_put( ) ) )
      prior_put( ).erase( );

   bool ignore_blockchain_puts = false;

   if( !identity.empty( )
    && !get_system_variable( c_ignore_all_puts_prefix + identity ).empty( ) )
      ignore_blockchain_puts = true;

   bool any_supporter_has_top_get = false;
   bool any_supporter_has_top_put = false;

   string next_hash_to_get, next_hash_to_put;

   if( !is_for_support )
   {
      size_t num_tree_item = get_blockchain_tree_item( blockchain );

      if( num_tree_item != last_num_tree_item )
      {
         last_num_tree_item = num_tree_item;

         string num_tree_items( get_session_variable(
          get_special_var_name( e_special_var_blockchain_num_tree_items ) ) );

         string progress_message( "Processing " + to_string( num_tree_item ) );

         if( !num_tree_items.empty( ) )
            progress_message += '/' + num_tree_items;

         progress_message += " tree items...";

         set_session_progress_output( progress_message );
      }
   }

   // NOTE: If a support session is not given a file hash to get/put then sleep for a while.
   for( size_t i = 0; i < c_support_session_sleep_repeats; i++ )
   {
      next_hash_to_get = top_next_peer_file_hash_to_get(
       ( i == 0 && check_for_supporters ) ? &any_supporter_has_top_get : 0 );

      if( !ignore_blockchain_puts )
         next_hash_to_put = top_next_peer_file_hash_to_put(
          ( i == 0 && check_for_supporters ) ? &any_supporter_has_top_put : 0 );

      if( is_for_support
       && next_hash_to_get.empty( ) && next_hash_to_put.empty( ) )
         msleep( c_support_session_sleep_time );

      if( !is_for_support || !next_hash_to_get.empty( ) || !next_hash_to_put.empty( ) )
         break;
   }

   bool set_new_zenith = false;

   string zenith_hash( get_session_variable(
    get_special_var_name( e_special_var_blockchain_zenith_hash ) ) );

   if( !zenith_hash.empty( ) && next_hash_to_get.empty( )
    && !any_supporter_has_top_get && !any_supporter_has_top_put )
      set_new_zenith = true;

   // NOTE: If main session has nothing to do while support sessions
   // are still processing get/put file hashes then will sleep here.
   if( !is_for_support
    && next_hash_to_get.empty( ) && next_hash_to_put.empty( )
    && ( any_supporter_has_top_get || any_supporter_has_top_put ) )
      msleep( c_main_session_sleep_time );

   if( !any_supporter_has_top_get
    && !any_supporter_has_top_put && !prior_put( ).empty( ) && want_to_do_op( e_op_chk ) )
   {
      bool has_issued_chk = false;

      if( !is_for_support && !blockchain.empty( ) )
      {
         string genesis_block_tag( blockchain + ".0" + string( c_blk_suffix ) );

         // NOTE: If the genesis block is not present then check if it now exists.
         if( !has_tag( genesis_block_tag ) )
         {
            string genesis_block_hash;
            chk_file( genesis_block_tag, &genesis_block_hash );

            has_issued_chk = true;

            if( !genesis_block_hash.empty( ) )
               add_peer_file_hash_for_get( genesis_block_hash );
         }
         else
         {
            // NOTE: Check whether a new block has been created either locally or remotely.
            string next_block_tag( blockchain
             + '.' + to_string( blockchain_height + 1 ) + c_blk_suffix );

            if( has_tag( next_block_tag ) )
            {
               // NOTE: Use "blockchain_height_pending" here to ensure that only
               // one "process_block_for_height" call will occur for each block.
               // If all of the block related files were found locally then must
               // simply increase the height.
               if( blockchain_height == blockchain_height_pending )
               {
                  string next_block_hash( tag_file_hash( next_block_tag ) );

                  // NOTE: If zenith height is not greater than the blockchain height
                  // then block was not created locally and will assume that fetching
                  // must have not been previously completed.
                  string zenith_hash( tag_file_hash( blockchain + c_zenith_suffix ) );

                  size_t zenith_height = 0;

                  if( !get_block_height_from_tags( blockchain, zenith_hash, zenith_height ) )
                     throw runtime_error( "unexpected error determining zenith height" );

                  bool need_to_check = false;

                  if( zenith_height > blockchain_height )
                     set_session_variable(
                      get_special_var_name( e_special_var_blockchain_is_fetching ), "" );
                  else
                  {
                     need_to_check = true;

                     set_session_variable(
                      get_special_var_name( e_special_var_blockchain_is_fetching ), c_true_value );
                  }

                  size_t num_items_found = 0;

                  bool has_block_data = process_block_for_height(
                   blockchain, next_block_hash, blockchain_height + 1, &num_items_found );

                  if( !need_to_check && has_block_data )
                     blockchain_height = ++blockchain_height_pending;
                  else
                  {
                     if( need_to_check )
                     {
                        string file_hash( top_next_peer_file_hash_to_get( ) );

                        // NOTE: Use the "nonce" argument to indicate the first
                        // file to be fetched (so that pull requests will start
                        // from the correct point).
                        if( !file_hash.empty( ) )
                        {
                           next_block_tag += string( " " ) + '@' + file_hash;

                           set_blockchain_tree_item( blockchain, num_items_found );
                        }

                        chk_file( next_block_tag, &next_block_hash );
                        has_issued_chk = true;
                     }

                     blockchain_height_pending = blockchain_height + 1;
                  }
               }
            }
            else
            {
               string next_block_hash;
               chk_file( next_block_tag, &next_block_hash );

               has_issued_chk = true;

               if( next_block_hash.empty( ) )
               {
                  date_time now( date_time::standard( ) );

                  seconds elapsed = ( seconds )( now - dtm_rcvd_not_found );

                  dtm_rcvd_not_found = now;

                  // NOTE: If neither peer has had a new block within one
                  // second then sleep now to avoid unnecessary CPU usage.
                  if( elapsed < 1.0 )
                  {
                     elapsed = ( seconds )( now - dtm_sent_not_found );

                     if( elapsed < 1.0 )
                        msleep( c_peer_sleep_time );
                  }
               }
               else if( !has_file( next_block_hash ) )
               {
                  if( !zenith_hash.empty( ) )
                     set_new_zenith = true;

                  add_peer_file_hash_for_get( next_block_hash );
                  blockchain_height_pending = blockchain_height + 1;

                  set_session_variable(
                   get_special_var_name( e_special_var_blockchain_is_fetching ), c_true_value );
               }
            }
         }
      }

      if( !has_issued_chk )
         chk_file( prior_put( ) );
   }
   else if( want_to_do_op( e_op_pip ) )
      pip_peer( get_random_same_port_peer_ip_addr( c_local_ip_addr ) );
   else if( next_hash_to_put.empty( ) )
   {
      string next_hash( next_hash_to_get );

      while( !next_hash.empty( ) && has_file( next_hash.substr( 0, next_hash.find( ':' ) ) ) )
      {
         pop_next_peer_file_hash_to_get( );
         next_hash = top_next_peer_file_hash_to_get( );
      }

      if( !next_hash.empty( ) )
      {
         string file_data;

         get_file( next_hash, &file_data );
         pop_next_peer_file_hash_to_get( );

         if( is_for_support || last_num_tree_item )
            add_to_blockchain_tree_item( blockchain, 1 );

         bool is_list = false;

         if( !file_data.empty( ) )
            is_list = is_list_file( file_data[ 0 ] ); 
         else
            is_list = is_list_file( next_hash.substr( 0, next_hash.find( ':' ) ) );

         string data_file_hash( get_session_variable(
          get_special_var_name( e_special_var_blockchain_data_file_hash ) ) );

         string primary_pubkey_hash( get_session_variable(
          get_special_var_name( e_special_var_blockchain_primary_pubkey_hash ) ) );

         string signature_file_hash( get_session_variable(
          get_special_var_name( e_special_var_blockchain_signature_file_hash ) ) );

         string secondary_pubkey_hash( get_session_variable(
          get_special_var_name( e_special_var_blockchain_secondary_pubkey_hash ) ) );

         if( next_hash == data_file_hash )
         {
            verify_core_file( file_data, false );

            create_raw_file( file_data, true, 0, 0, next_hash.c_str( ), true, true );

            process_data_file( blockchain, data_file_hash, blockchain_height_pending );

            string num_tree_items( get_session_variable(
             get_special_var_name( e_special_var_blockchain_num_tree_items ) ) );

            if( !num_tree_items.empty( ) )
               set_blockchain_tree_item( blockchain, 1 );

            set_session_variable(
             get_special_var_name( e_special_var_blockchain_data_file_hash ), "" );
         }
         else if( next_hash == primary_pubkey_hash )
         {
            validate_public_key_file( file_data );

            create_raw_file( file_data, true, 0, 0, next_hash.c_str( ), true, true );

            process_public_key_file( blockchain, primary_pubkey_hash, blockchain_height_pending, true );
         }
         else if( next_hash == signature_file_hash )
         {
            validate_signature_file( file_data );

            create_raw_file( file_data, true, 0, 0, next_hash.c_str( ), true, true );

            if( blockchain_height != blockchain_height_pending )
            {
               process_signature_file( blockchain, signature_file_hash, blockchain_height_pending );

               string data_file_hash( get_session_variable(
                get_special_var_name( e_special_var_blockchain_data_file_hash ) ) );

               if( !data_file_hash.empty( ) && !has_file( data_file_hash ) )
                  add_peer_file_hash_for_get( data_file_hash );
            }

            set_session_variable(
             get_special_var_name( e_special_var_blockchain_signature_file_hash ), "" );
         }
         else if( next_hash == secondary_pubkey_hash )
         {
            validate_public_key_file( file_data );

            create_raw_file( file_data, true, 0, 0, next_hash.c_str( ), true, true );

            process_public_key_file( blockchain, secondary_pubkey_hash, blockchain_height_pending, false );
         }
         else if( next_hash[ next_hash.length( ) - 1 ] != c_repository_suffix )
         {
            if( !is_list )
            {
               // NOTE: Unles both are owners only core block files should be found here.
               if( get_session_variable( 
                get_special_var_name( e_special_var_blockchain_both_are_owners ) ).empty( ) )
                  verify_core_file( file_data, false );

               create_raw_file( file_data, true, 0, 0, next_hash.c_str( ), true, true );

               process_core_file( next_hash, blockchain );
            }
         }
#ifdef SSL_SUPPORT
         else
         {
            if( get_session_variable( 
             get_special_var_name( e_special_var_blockchain_both_are_owners ) ).empty( ) )
               process_repository_file( blockchain,
                next_hash.substr( 0, next_hash.length( ) - 1 ), get_is_test_session( ), &file_data );
            else if( !is_list )
            {
               if( !blockchain.empty( ) )
                  throw runtime_error( "unexpected blob hash: " + next_hash );
               else
                  create_raw_file( file_data, true, 0, 0, next_hash.c_str( ), true, true );
            }
         }
#endif
      }
      else
         get_hello( );
   }
   else
   {
      string next_hash( next_hash_to_put );

      bool had_hash = !next_hash.empty( );

      if( next_hash.empty( ) || !has_file( next_hash ) )
         put_hello( );
      else
         put_file( next_hash );

      if( had_hash )
      {
         pop_next_peer_file_hash_to_put( );

         if( !get_session_variable( next_hash ).empty( ) )
         {
            delete_file( next_hash );
            set_session_variable( next_hash, "" );
         }
      }
   }

   if( set_new_zenith )
   {
      tag_file( blockchain + c_zenith_suffix, zenith_hash );

      last_num_tree_item = 0;
      set_blockchain_tree_item( blockchain, 0 );

      list_items_to_ignore.clear( );

      set_session_progress_output( "" );

      set_session_variable( get_special_var_name(
       e_special_var_blockchain_num_tree_items ), "" );

      blockchain_height = blockchain_height_pending;

      TRACE_LOG( TRACE_PEER_OPS, "=== new zenith hash: "
       + zenith_hash + " height: " + to_string( blockchain_height ) );

      set_session_variable(
       get_special_var_name( e_special_var_blockchain_zenith_hash ), "" );

      set_session_variable(
       get_special_var_name( e_special_var_blockchain_zenith_height ), to_string( blockchain_height ) );
   }
}

void socket_command_handler::preprocess_command_and_args( string& str, const string& cmd_and_args )
{
   str = cmd_and_args;

   if( !str.empty( ) )
   {
      TRACE_LOG( TRACE_COMMANDS, cmd_and_args );

      if( str[ 0 ] == '?' || str.find( "help" ) == 0 )
      {
         if( !had_usage )
         {
            had_usage = true;

            string::size_type pos = str.find( ' ' );

            string wildcard_match_expr;
            if( pos != string::npos )
               wildcard_match_expr = str.substr( pos + 1 );

            if( get_command_processor( ) )
               get_command_processor( )->output_command_usage( wildcard_match_expr );

            str.erase( );
         }
         else
            str = c_cmd_peer_session_bye;
      }
   }

   if( !str.empty( ) )
      next_command = str;
}

void socket_command_handler::postprocess_command_and_args( const string& cmd_and_args )
{
   string::size_type pos = cmd_and_args.find( ' ' );
   last_command = cmd_and_args.substr( 0, pos );

   if( has_finished( ) )
   {
      TRACE_LOG( TRACE_SESSIONS, get_blockchain( ).empty( )
       ? "finished peer session" : "finished peer session for blockchain " + get_blockchain( ) );
   }
}

void socket_command_handler::handle_command_response( const string& response, bool is_special )
{
   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   if( !response.empty( ) )
   {
      if( !is_special && is_responder )
         socket.set_delay( );
      else
         socket.set_no_delay( );

      socket.write_line( response, c_request_timeout, p_progress );
   }

   if( !is_special && is_responder )
   {
      socket.set_no_delay( );
      socket.write_line( c_response_okay, c_request_timeout, p_progress );
   }
}

class peer_session_command_functor : public command_functor
{
   public:
   peer_session_command_functor( command_handler& handler )
    : command_functor( handler ),
    socket_handler( dynamic_cast< socket_command_handler& >( handler ) )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   socket_command_handler& socket_handler;
};

command_functor* peer_session_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new peer_session_command_functor( handler );
}

void peer_session_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   string response;

   bool send_okay_response = true;
   bool possibly_expected_error = ( socket_handler.state( ) >= e_peer_state_waiting_for_get );

#ifdef SSL_SUPPORT
   ssl_socket& socket( socket_handler.get_socket( ) );
#else
   tcp_socket& socket( socket_handler.get_socket( ) );
#endif

   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   if( command != c_cmd_peer_session_bye )
      socket.set_delay( );

   string hello_data, hello_hash;
   hello_data = get_hello_data( hello_hash );

   set_last_session_cmd( command );

   string identity( socket_handler.get_identity( ) );
   string blockchain( socket_handler.get_blockchain( ) );

   size_t blockchain_height = socket_handler.get_blockchain_height( );

   bool check_for_supporters = false;

   if( !get_session_variable( get_special_var_name(
    e_special_var_blockchain_peer_has_supporters ) ).empty( ) )
      check_for_supporters = true;

   try
   {
      ostringstream osstr;

      if( command == c_cmd_peer_session_chk )
      {
         string tag_or_hash( get_parm_val( parameters, c_cmd_peer_session_chk_tag_or_hash ) );
         string nonce( get_parm_val( parameters, c_cmd_peer_session_chk_nonce ) );

         if( socket_handler.state( ) != e_peer_state_responder
          && socket_handler.state( ) != e_peer_state_waiting_for_get
          && socket_handler.state( ) != e_peer_state_waiting_for_put
          && socket_handler.state( ) != e_peer_state_waiting_for_get_or_put )
            throw runtime_error( "invalid state for chk" );

         // NOTE: Create the dummy "hello" blob as it will be required.
         if( !has_file( hello_hash ) )
            create_raw_file( hello_data, false );

         string hash( tag_or_hash );

         if( hash == hello_hash )
            response = hello_hash;

         if( has_tag( tag_or_hash ) )
            response = hash = tag_file_hash( tag_or_hash );

         // NOTE: Unless doing interactive testing any peer "chk tag" request must start
         // with the blockchain tag prefix (otherwise a peer might be able to find files
         // that are not intended for their discovery).
         if( !blockchain.empty( )
          && ( tag_or_hash.find( blockchain ) != 0 )
          && ( tag_or_hash.length( ) != ( c_sha256_digest_size * 2 ) ) )
            throw runtime_error( "invalid non-blockchain prefixed tag" );

         if( tag_or_hash.find( c_key_suffix ) != string::npos )
            throw runtime_error( "invalid suspiciouus tag '" + tag_or_hash + "'" );

         bool has = has_file( hash, false );
         bool was_initial_state = ( socket_handler.state( ) == e_peer_state_responder );

         if( !has )
         {
            response = c_response_not_found;

            socket_handler.set_dtm_sent_not_found( date_time::standard( ) );

            if( was_initial_state )
            {
               if( !blockchain.empty( ) )
               {
                  if( tag_or_hash.find( c_bc_prefix ) != 0 )
                     socket_handler.state( ) = e_peer_state_invalid;
                  else
                  {
                     socket_handler.op_state( ) = e_peer_state_waiting_for_get;
                     socket_handler.state( ) = e_peer_state_waiting_for_get_or_put;

                     socket_handler.trust_level( ) = e_peer_trust_level_normal;

                     if( !nonce.empty( ) )
                        add_peer_file_hash_for_get( nonce );
                  }
               }
               else
               {
                  handler.issue_command_response( response, true );

                  handler.issue_command_response( "put " + hello_hash, true );

                  socket.set_delay( );
                  fetch_file( hello_hash, socket, p_progress );

                  string temp_file_name( "~" + uuid( ).as_string( ) );

                  try
                  {
                     store_temp_file( temp_file_name, socket, p_progress );

                     response.erase( );

                     if( !temp_file_is_identical( temp_file_name, hello_hash ) )
                        socket_handler.state( ) = e_peer_state_invalid;
                     else
                     {
                        socket_handler.state( ) = e_peer_state_waiting_for_put;
                        socket_handler.op_state( ) = e_peer_state_waiting_for_put;

                        socket_handler.trust_level( ) = e_peer_trust_level_normal;
                     }

                     file_remove( temp_file_name );
                  }
                  catch( ... )
                  {
                     file_remove( temp_file_name );
                     throw;
                  }
               }
            }
            else if( blockchain.empty( ) )
            {
               string local_hash, local_public_key, master_public_key;

               if( fetch_repository_entry_record( hash,
                local_hash, local_public_key, master_public_key, false ) )
               {
                  string pull_hash(
                   create_peer_repository_entry_pull_info( hash, local_hash, local_public_key, "" ) );

                  add_peer_file_hash_for_put( pull_hash );

                  set_session_variable( pull_hash, c_true_value );
               }
            }
         }
         else
         {
            if( was_initial_state
             && tag_or_hash.find( c_bc_prefix ) == 0 )
               nonce.erase( );

            if( !nonce.empty( ) && nonce[ 0 ] != '@' )
               response = hash_with_nonce( hash, nonce );

            if( was_initial_state )
            {
               socket_handler.op_state( ) = e_peer_state_waiting_for_get;

               if( blockchain.empty( ) )
                  socket_handler.state( ) = e_peer_state_waiting_for_get;
               else
               {
                  socket_handler.state( ) = e_peer_state_waiting_for_get_or_put;

                  string zenith_height( get_session_variable(
                   get_special_var_name( e_special_var_blockchain_zenith_height ) ) );

                  if( zenith_height.empty( )
                   && get_block_height_from_tags( blockchain, hash, blockchain_height ) )
                     process_block_for_height( blockchain, hash, blockchain_height );
               }
            }
            else if( !socket_handler.get_is_for_support( ) && ( tag_or_hash.find( c_bc_prefix ) == 0 ) )
            {
               if( get_block_height_from_tags( blockchain, hash, blockchain_height ) )
               {
                  string first_item_hash;

                  // NOTE: In the case where a peer had not completed fetching
                  // all files for a new block height the nonce value (using a
                  // '@' prefix) identifies the next "get" file which (if this
                  // is a repository entry) needs to be the first to be "put".
                  if( !nonce.empty( ) && nonce[ 0 ] == '@' )
                     first_item_hash = nonce.substr( 1 );

                  temporary_session_variable temp_hash( get_special_var_name( e_special_var_hash ), first_item_hash );

                  set_system_variable( c_ignore_all_puts_prefix + identity, c_true_value );

                  if( socket_handler.get_is_responder( ) )
                  {
                     if( blockchain_height > socket_handler.get_blockchain_height( ) )
                        socket_handler.set_blockchain_height( blockchain_height );

                     process_block_for_height( blockchain, hash, blockchain_height );
                  }
                  else
                  {
                     if( !first_item_hash.empty( )
                      || get_session_variable( get_special_var_name( e_special_var_blockchain_is_fetching ) ).empty( ) )
                        process_block_for_height( blockchain, hash, blockchain_height );
                  }
               }
            }
         }

         if( !was_initial_state && socket_handler.get_is_responder( ) )
         {
            handler.issue_command_response( response, true );
            response.erase( );

            socket_handler.issue_cmd_for_peer( check_for_supporters );
         }
      }
      else if( command == c_cmd_peer_session_get )
      {
         string tag_or_hash( get_parm_val( parameters, c_cmd_peer_session_get_tag_or_hash ) );

         if( socket_handler.state( ) != e_peer_state_waiting_for_get
          && socket_handler.state( ) != e_peer_state_waiting_for_get_or_put )
         {
            if( !socket_handler.get_is_responder( ) )
               throw runtime_error( "invalid state for get (initiator)" );
            else
               throw runtime_error( "invalid state for get (responder)" );
         }

         string hash( tag_or_hash );

         if( has_tag( tag_or_hash ) )
            hash = tag_file_hash( tag_or_hash );

         socket.set_delay( );

         if( !identity.empty( ) )
         {
            if( !get_system_variable( c_ignore_all_puts_prefix + identity ).empty( ) )
            {
               if( get_hash_tags( hash ).find( c_dat_suffix ) != string::npos )
                  set_system_variable( c_ignore_all_puts_prefix + identity, "" );
            }
         }

         fetch_file( hash, socket, p_progress );
         increment_peer_files_uploaded( file_bytes( hash ) );

         socket_handler.op_state( ) = e_peer_state_waiting_for_put;

         if( blockchain.empty( ) )
            socket_handler.state( ) = e_peer_state_waiting_for_put;
         else
            socket_handler.state( ) = e_peer_state_waiting_for_get_or_put;

         if( socket_handler.get_is_responder( ) )
         {
            handler.issue_command_response( response, true );
            response.erase( );

            if( blockchain.empty( ) && top_next_peer_file_hash_to_put( ).empty( ) )
               add_peer_file_hash_for_put( hello_hash );

            socket_handler.issue_cmd_for_peer( check_for_supporters );
         }
      }
      else if( command == c_cmd_peer_session_put )
      {
         string hash( get_parm_val( parameters, c_cmd_peer_session_put_hash ) );

         if( socket_handler.state( ) < e_peer_state_waiting_for_put )
         {
            if( !socket_handler.get_is_responder( ) )
               throw runtime_error( "invalid state for put (initiator)" );
            else
               throw runtime_error( "invalid state for put (responder)" );
         }

         socket.set_delay( );

         int64_t bytes = 0;

         if( !has_file( hash ) )
         {
            string file_data;

            store_file( hash, socket, 0, p_progress, false, 0, false, &file_data );

            bytes = file_data.length( );

            if( hash != hello_hash )
            {
               size_t num_skipped = process_put_file(
                blockchain, file_data.substr( 1 ), check_for_supporters,
                socket_handler.get_is_test_session( ), socket_handler.get_list_items_to_ignore( ) );

               if( num_skipped )
                  add_to_blockchain_tree_item( blockchain, num_skipped );
            }
         }
         else
         {
            bytes = file_bytes( hash );

            store_temp_file( "", socket, p_progress, true );
         }

         if( hash != hello_hash )
            increment_peer_files_downloaded( bytes );

         if( has_file( hash ) )
         {
            if( socket_handler.prior_put( ).empty( ) )
               socket_handler.prior_put( ) = hash;
         }

         socket_handler.op_state( ) = e_peer_state_waiting_for_get;

         if( blockchain.empty( ) )
            socket_handler.state( ) = e_peer_state_waiting_for_get;
         else
            socket_handler.state( ) = e_peer_state_waiting_for_get_or_put;

         if( socket_handler.get_is_responder( ) )
         {
            handler.issue_command_response( response, true );
            response.erase( );

            socket_handler.issue_cmd_for_peer( check_for_supporters );
         }
      }
      else if( command == c_cmd_peer_session_pip )
      {
         string addr( get_parm_val( parameters, c_cmd_peer_session_pip_addr ) );

         response = get_random_same_port_peer_ip_addr( c_local_ip_addr );

         if( socket_handler.state( ) != e_peer_state_waiting_for_get
          && socket_handler.state( ) != e_peer_state_waiting_for_put
          && socket_handler.state( ) != e_peer_state_waiting_for_get_or_put )
            throw runtime_error( "invalid state for pip" );

         if( socket_handler.get_is_responder( ) )
         {
            handler.issue_command_response( response, true );
            response.erase( );

            socket_handler.issue_cmd_for_peer( check_for_supporters );
         }
      }
      else if( command == c_cmd_peer_session_tls )
      {
         if( socket_handler.state( ) != e_peer_state_responder )
            throw runtime_error( "invalid state for tls" );

#ifdef SSL_SUPPORT
         if( socket.is_secure( ) )
            throw runtime_error( "TLS is already active" );

         if( !get_using_ssl( ) )
            throw runtime_error( "SSL has not been initialised" );

         socket.ssl_accept( );
#else
         throw runtime_error( "SSL support not available" );
#endif

         socket_handler.state( ) = e_peer_state_waiting_for_get;
         socket_handler.op_state( ) = e_peer_state_waiting_for_get;
      }
      else if( command == c_cmd_peer_session_bye )
      {
         if( !is_captured_session( ) )
            handler.set_finished( );
         else if( !is_condemned_session( ) )
            condemn_this_session( );

         return;
      }
   }
   catch( exception& x )
   {
      TRACE_LOG( ( possibly_expected_error ? TRACE_SESSIONS : 0 ), string( "peer session error: " ) + x.what( ) );

      send_okay_response = false;
      response = string( c_response_error_prefix ) + x.what( );

      socket_handler.state( ) = e_peer_state_invalid;
   }
   catch( ... )
   {
      TRACE_LOG( TRACE_ANYTHING, "peer session error: unexpected unknown exception caught" );

      send_okay_response = false;
      response = string( c_response_error_prefix ) + "unexpected unknown exception caught";

      socket_handler.state( ) = e_peer_state_invalid;
   }

   handler.issue_command_response( response, !send_okay_response );

   bool has_issued_bye = false;

   // NOTE: If a disconnect has been actioned for the identity then issue a "bye" after
   // condemning this session and (unless still connecting) do the same if the matching
   // identity session is not found.
   if( !socket_handler.get_is_for_support( ) )
   {
      string identity( get_session_variable(
       get_special_var_name( e_special_var_identity ) ) );

      if( !identity.empty( ) && get_system_variable( identity ).empty( ) )
      {
         bool is_only_session = false;

         if( socket_handler.get_is_time_for_check( ) )
            is_only_session = ( num_have_session_variable( identity ) < 2 );

         bool is_disconnecting = !get_system_variable( '~' + identity ).empty( );
         bool had_connect_error = !get_system_variable( c_error_message_prefix + identity ).empty( );

         if( is_only_session || is_disconnecting || had_connect_error )
         {
            set_session_variable( identity, "" );

            if( !is_condemned_session( ) )
            {
               condemn_this_session( );

               if( is_only_session )
               {
                  TRACE_LOG( TRACE_SESSIONS, "Ending session for identity '"
                   + identity + "' due to matching session not being found." );
               }

               if( !is_captured_session( ) )
                  handler.set_finished( );

               socket.write_line( c_cmd_peer_session_bye, c_request_timeout, p_progress );

               has_issued_bye = true;
            }
         }
      }
   }

   if( !send_okay_response || is_condemned_session( ) )
   {
      socket_handler.state( ) = e_peer_state_invalid;

      if( !is_captured_session( ) )
      {
         if( is_condemned_session( ) && !has_issued_bye )
            socket.write_line( c_cmd_peer_session_bye, c_request_timeout, p_progress );

         handler.set_finished( );
      }
      else if( !is_condemned_session( ) )
         condemn_this_session( );
   }
   else if( !socket_handler.get_is_responder( ) && !g_server_shutdown
    && ( socket_handler.op_state( ) == e_peer_state_waiting_for_get ) )
   {
      string response;

      if( socket.read_line( response, c_request_timeout, 0, p_progress ) <= 0 )
      {
         string error;

         if( !socket.had_timeout( ) )
            error = "peer has terminated this connection";
         else
            error = "timeout occurred getting peer response";

         socket.close( );

         if( !is_captured_session( ) )
            handler.set_finished( );
         else if( !is_condemned_session( ) )
            condemn_this_session( );

         throw runtime_error( error );
      }

      if( response != string( c_response_okay ) )
      {
         socket.close( );

         if( !is_captured_session( ) )
            handler.set_finished( );
         else if( !is_condemned_session( ) )
            condemn_this_session( );

         throw runtime_error( "unexpected non-okay response from peer" );
      }

      socket_handler.issue_cmd_for_peer( check_for_supporters );
   }
}

class socket_command_processor : public command_processor
{
   public:
   socket_command_processor( tcp_socket& socket,
    command_handler& handler, bool is_local, bool is_owner, bool is_responder )
    : command_processor( handler ),
    socket( socket ),
    handler( handler ),
    is_local( is_local ),
    is_owner( is_owner ),
    is_responder( is_responder )
   {
      peer_special_variable = get_special_var_name( e_special_var_peer );

      initiator_special_variable = get_special_var_name( e_special_var_peer_initiator );
      responder_special_variable = get_special_var_name( e_special_var_peer_responder );

      string value( c_true_value );

      socket_command_handler& socket_handler = dynamic_cast< socket_command_handler& >( handler );

      if( !socket_handler.get_blockchain( ).empty( ) )
         value = socket_handler.get_blockchain( );

      set_session_variable( peer_special_variable, value );

      if( !is_responder )
         set_session_variable( initiator_special_variable, value );
      else
         set_session_variable( responder_special_variable, value );
   }

   private:
   tcp_socket& socket;
   command_handler& handler;

   string peer_special_variable;
   string initiator_special_variable;
   string responder_special_variable;

   bool is_local;
   bool is_owner;

   bool is_responder;

   bool is_still_processing( ) { return is_captured_session( ) || socket.okay( ); }

   void get_cmd_and_args( string& cmd_and_args );

   void output_command_usage( const string& wildcard_match_expr ) const;
};

void socket_command_processor::get_cmd_and_args( string& cmd_and_args )
{
   socket_command_handler& socket_handler = dynamic_cast< socket_command_handler& >( handler );

   string blockchain( socket_handler.get_blockchain( ) );

   bool check_for_supporters = false;

   string identity( get_session_variable(
    get_special_var_name( e_special_var_identity ) ) );

   string ip_addr( get_session_variable( get_special_var_name( e_special_var_ip_addr ) ) );

   while( true )
   {
      progress* p_progress = 0;
      trace_progress progress( TRACE_SOCK_OPS );

      if( get_trace_flags( ) & TRACE_SOCK_OPS )
         p_progress = &progress;

      if( !check_for_supporters && !get_session_variable(
       get_special_var_name( e_special_var_blockchain_peer_has_supporters ) ).empty( ) )
         check_for_supporters = true;

      if( !is_responder && !g_server_shutdown && !is_condemned_session( ) )
      {
         if( socket_handler.op_state( ) == e_peer_state_waiting_for_put )
         {
            string response;
            if( socket.read_line( response, c_request_timeout, 0, p_progress ) <= 0 )
            {
               cmd_and_args = "bye";
               break;
            }

            if( response != string( c_response_okay ) )
            {
               cmd_and_args = "bye";
               break;
            }

            if( blockchain.empty( ) )
               socket_handler.state( ) = e_peer_state_waiting_for_get;
            else
               socket_handler.state( ) = e_peer_state_waiting_for_get_or_put;

            socket_handler.issue_cmd_for_peer( check_for_supporters );
         }
      }

      if( socket.read_line( cmd_and_args, c_request_timeout, c_max_line_length, p_progress ) <= 0 )
      {
         if( !is_captured_session( )
          && ( is_condemned_session( ) || g_server_shutdown || !socket.had_timeout( ) ) )
         {
            if( is_condemned_session( ) && !socket_handler.get_is_for_support( ) )
               condemn_matching_sessions( );

            // NOTE: If the session is not captured and it has either been condemned or
            // the server is shutting down, or its socket has died then force a "bye".
            cmd_and_args = "bye";
            break;
         }

         // NOTE: If the socket is dead then the "read_line" will return instantly so for
         // a "captured" session manually perform a timeout sleep so CPU is not overused.
         if( is_captured_session( ) && !socket.had_timeout( ) )
         {
            msleep( c_request_timeout );
            continue;
         }

         if( !is_local || !socket.had_timeout( ) )
         {
            // NOTE: Don't allow zombies to hang around unless they are local.
            cmd_and_args = "bye";
            break;
         }
      }
      else
      {
         if( g_server_shutdown )
            condemn_this_session( );

         if( is_condemned_session( ) && !socket_handler.get_is_for_support( ) )
            condemn_matching_sessions( );

#ifdef USE_THROTTLING
         if( cmd_and_args != "bye" )
            msleep( c_request_throttle_sleep_time );
#endif
         if( cmd_and_args == c_response_okay || cmd_and_args == c_response_okay_more )
            cmd_and_args = "bye";

         break;
      }
   }
}

void socket_command_processor::output_command_usage( const string& wildcard_match_expr ) const
{
   socket.set_delay( );

   string cmds( "commands:" );
   if( !wildcard_match_expr.empty( ) )
      cmds += ' ' + wildcard_match_expr;

   socket.write_line( cmds, c_request_timeout );
   socket.write_line( "=========", c_request_timeout );

   socket.write_line( get_usage_for_commands( wildcard_match_expr ), c_request_timeout );

   socket.set_no_delay( );
   socket.write_line( c_response_okay, c_request_timeout );
}

#ifdef SSL_SUPPORT
peer_session* construct_session( const date_time& dtm,
 bool is_responder, auto_ptr< ssl_socket >& ap_socket, const string& addr_info,
 bool is_for_support = false, peer_extra extra = e_peer_extra_none, const char* p_identity = 0 )
#else
peer_session* construct_session( const date_time& dtm,
 bool is_responder, auto_ptr< tcp_socket >& ap_socket, const string& addr_info,
 bool is_for_support = false, peer_extra extra = e_peer_extra_none, const char* p_identity = 0 )
#endif
{
   peer_session* p_session = 0;

   string::size_type pos = addr_info.find( '=' );

   string blockchains;

   bool already_has_session = false;

   if( pos != string::npos )
   {
      blockchains = addr_info.substr( pos + 1 );
      string::size_type ppos = blockchains.find( ':' );

      if( ppos != string::npos )
         blockchains.erase( ppos );

      if( blockchains.find( ',' ) == string::npos )
         already_has_session = has_session_with_ip_addr( addr_info.substr( 0, pos ), blockchains );
   }

   if( is_for_support || !already_has_session 
    || addr_info.substr( 0, pos ) == c_local_ip_addr
    || addr_info.substr( 0, pos ) == c_local_ip_addr_for_ipv6 )
      p_session = new peer_session( unix_time_stamp( dtm ),
       is_responder, ap_socket, addr_info, is_for_support, extra, p_identity );

   return p_session;
}

}

#ifdef SSL_SUPPORT
peer_session::peer_session( int64_t time_val,
 bool is_responder, auto_ptr< ssl_socket >& ap_socket,
 const string& addr_info, bool is_for_support, peer_extra extra, const char* p_identity )
#else
peer_session::peer_session( int64_t time_val,
 bool is_responder, auto_ptr< tcp_socket >& ap_socket,
 const string& addr_info, bool is_for_support, peer_extra extra, const char* p_identity )
#endif
 :
 is_local( false ),
 is_owner( false ),
 ip_addr( addr_info ),
 time_val( time_val ),
 ap_socket( ap_socket ),
 is_responder( is_responder ),
 is_for_support( is_for_support ),
 peer_is_owner( false ),
 has_support_sessions( false ),
 has_found_both_are_owners( false )
{
   if( !( *this->ap_socket ) )
      throw runtime_error( "unexpected invalid socket in peer_session::peer_session" );

   if( p_identity )
      identity = string( p_identity );

   string::size_type pos = addr_info.find( '=' );
   if( pos != string::npos )
   {
      blockchain = addr_info.substr( pos + 1 );
      ip_addr.erase( pos );
   }

   pos = blockchain.find( ':' );
   if( pos != string::npos )
   {
      port = blockchain.substr( pos + 1 );
      blockchain.erase( pos );
   }

   if( !blockchain.empty( ) && ( blockchain.find( c_bc_prefix ) != 0 ) )
      throw runtime_error( "invalid blockchain tag prefix '" + blockchain + "'" );

   if( port.empty( ) && blockchain.empty( ) )
      port = get_test_peer_port( );

   if( ip_addr == c_local_ip_addr || ip_addr == c_local_ip_addr_for_ipv6 )
      is_local = true;

   if( !is_responder && !blockchain.empty( )
    && !list_file_tags( blockchain + string( ".p*" ) + c_key_suffix ).empty( ) )
      is_owner = true;

   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

#ifdef DEBUG_PEER_HANDSHAKE
   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;
#endif

   this->ap_socket->set_no_delay( );

   // NOTE: A dummy PID is being written/read here so that the standard general
   // purpose client can be used to connect as a peer (for interactive testing).
   string pid( c_dummy_peer_tag );

   bool needs_initiator = false;

   if( !is_responder )
   {
      if( is_for_support )
         pid = string( c_dummy_support_tag );

      if( !blockchain.empty( ) )
      {
         pid += ':' + blockchain;

         if( extra != e_peer_extra_none )
         {
            string own_identity( get_system_variable(
             get_special_var_name( e_special_var_blockchain ) ) );

            pid += '@' + own_identity;
         }
      }

      if( is_owner && !is_for_support )
         pid += '!';

      this->ap_socket->write_line( pid, c_request_timeout, p_progress );
   }
   else
   {
      pid.erase( );

      this->ap_socket->read_line( pid, c_request_timeout, c_max_greeting_size, p_progress );

      string::size_type pos = pid.find( '!' );

      if( pos != string::npos )
      {
         pid.erase( pos );
         peer_is_owner = true;
      }

      pos = pid.find( ':' );

      if( pos != string::npos )
      {
         set< string > blockchains;
         split( blockchain, blockchains );

         blockchain = pid.substr( pos + 1 );

         string::size_type spos = blockchain.find( '@' );

         if( spos != string::npos )
         {
            identity = blockchain.substr( spos + 1 );

            blockchain.erase( spos );
         }

         if( !blockchains.count( blockchain ) )
         {
            string identity( replaced( blockchain, c_bc_prefix, "" ) );

            // FUTURE: This message should be handled as a server string message.
            string error( "Unsupported peerchain identity '" + identity + "'." );

            this->ap_socket->write_line( string( c_response_error_prefix ) + error, c_request_timeout );

            throw runtime_error( error );
         }

         pid.erase( pos );
      }

      if( !blockchain.empty( )
       && !list_file_tags( blockchain + string( ".p*" ) + c_key_suffix ).empty( ) )
         is_owner = true;

      if( is_owner && peer_is_owner )
         has_found_both_are_owners = true;

      if( pid == string( c_dummy_support_tag ) )
         this->is_for_support = true;
      else if( pid == string( c_dummy_peer_tag ) )
         this->is_for_support = false;
      else if( blockchain.empty( ) )
         this->is_for_support = false;
      else
         throw runtime_error( "unexpected peer handshake" );
   }

   // NOTE: This check is necessary because listener created sessions set "is_for_support" true but
   //  whether it is actually a support session is only knowable after the first line has been read.
   if( !this->is_for_support && has_session_with_ip_addr( ip_addr, blockchain ) )
      throw runtime_error( "cannot create a non-support peer when has an existing non-support peer session" );

   increment_session_count( );
}

peer_session::~peer_session( )
{
   decrement_session_count( );
}

void peer_session::on_start( )
{
   bool okay = false;
   bool has_terminated = false;
   bool was_initialised = false;

   peer_state state = ( is_responder ? e_peer_state_responder : e_peer_state_initiator );

   int64_t current = unix_time_stamp( date_time::local( ) );

   int64_t difference = ( current - time_val ) + 1;

   try
   {
      socket_command_handler cmd_handler( *ap_socket, state,
       current + ( difference * 2 ), is_local, is_owner, blockchain, is_for_support );

      cmd_handler.add_commands( 0,
       peer_session_command_functor_factory, ARRAY_PTR_AND_SIZE( peer_session_command_definitions ) );

      progress* p_progress = 0;
      trace_progress progress( TRACE_SOCK_OPS );

#ifdef DEBUG_PEER_HANDSHAKE
      if( get_trace_flags( ) & TRACE_SOCK_OPS )
         p_progress = &progress;
#endif

      string unprefixed_blockchain( blockchain );
      replace( unprefixed_blockchain, c_bc_prefix, "" );

      if( is_responder )
      {
         string extra( to_string( get_files_area_item_max_size( ) ) );

         if( is_owner )
            extra += '!';

         ap_socket->write_line( string( c_protocol_version )
          + ':' + extra + '\n' + string( c_response_okay ), c_request_timeout, p_progress );
      }
      else
      {
         string greeting;

         if( ap_socket->read_line( greeting, c_request_timeout, c_max_greeting_size, p_progress ) <= 0 )
         {
            // FUTURE: This message should be handled as a server string message.
            string error;
            if( ap_socket->had_timeout( ) )
               error = "Timeout occurred trying to connect to peer.";
            else
               error = "Peer has unexpectedly terminated this connection.";

            if( !unprefixed_blockchain.empty( ) )
               set_system_variable( c_error_message_prefix + unprefixed_blockchain, error );

            ap_socket->close( );
            throw runtime_error( error );
         }

         if( greeting.find( c_response_error_prefix ) == 0 )
         {
            ap_socket->close( );

            greeting.erase( 0, strlen( c_response_error_prefix ) );

            if( !unprefixed_blockchain.empty( ) )
               set_system_variable( c_error_message_prefix + unprefixed_blockchain, greeting );

            throw runtime_error( greeting );
         }

         version_info ver_info;
         if( get_version_info( greeting, ver_info ) != string( c_response_okay ) )
         {
            ap_socket->close( );

            if( !unprefixed_blockchain.empty( ) )
               set_system_variable( c_error_message_prefix + unprefixed_blockchain, greeting );

            throw runtime_error( greeting );
         }

         if( !check_version_info( ver_info, c_protocol_major_version, c_protocol_minor_version ) )
         {
            ap_socket->close( );

            // FUTURE: This message should be handled as a server string message.
            string error( "Incompatible protocol version "
             + ver_info.ver + " (was expecting " + string( c_protocol_version ) + ")." );

            if( !unprefixed_blockchain.empty( ) )
               set_system_variable( c_error_message_prefix + unprefixed_blockchain, error );

            throw runtime_error( error );
         }

         if( !ver_info.extra.empty( ) )
         {
            string::size_type pos = ver_info.extra.find( '!' );

            if( pos != string::npos )
            {
               ver_info.extra.erase( pos );

               if( is_owner )
                  has_found_both_are_owners = true;
            }

            if( from_string< size_t >( ver_info.extra ) != get_files_area_item_max_size( ) )
            {
               ap_socket->close( );

               // FUTURE: This message should be handled as a server string message.
               string error( "Unexpected files area item size mismatch." );

               if( !unprefixed_blockchain.empty( ) )
                  set_system_variable( c_error_message_prefix + unprefixed_blockchain, error );

               throw runtime_error( error );
            }
         }
      }

      init_session( cmd_handler, true, &ip_addr, &unprefixed_blockchain, from_string< int >( port ), is_for_support );

      string slot_and_pubkey( get_session_variable( get_special_var_name( e_special_var_slot ) ) );
      slot_and_pubkey += '-' + get_session_variable( get_special_var_name( e_special_var_pubkey ) );

      string slotx, pubkeyx, slotx_and_pubkeyx;

      // NOTE: After handshake exchange public keys then commence peer protocol.
      if( is_responder )
      {
         ap_socket->write_line( slot_and_pubkey, c_request_timeout, p_progress );

         ap_socket->read_line( slotx_and_pubkeyx, c_request_timeout, c_max_pubkey_size, p_progress );
      }
      else
      {
         ap_socket->read_line( slotx_and_pubkeyx,
          c_request_timeout, c_max_pubkey_size, p_progress );

         ap_socket->write_line( slot_and_pubkey, c_request_timeout, p_progress );
      }

      string::size_type pos = slotx_and_pubkeyx.find( '-' );

      if( pos != string::npos )
      {
         slotx = slotx_and_pubkeyx.substr( 0, pos );
         pubkeyx = slotx_and_pubkeyx.substr( pos + 1 );
      }

      if( slotx.empty( ) )
         slotx = string( c_none );

      set_session_variable(
       get_special_var_name( e_special_var_slotx ), slotx );

      set_session_variable(
       get_special_var_name( e_special_var_pubkeyx ), pubkeyx );

      string archive_path;

      if( has_file_archive( unprefixed_blockchain, &archive_path ) )
      {
         set_session_variable( get_special_var_name(
          e_special_var_blockchain_archive_path ), archive_path );
      }

      if( is_owner )
         set_session_variable( get_special_var_name( e_special_var_blockchain_is_owner ), c_true_value );

      if( peer_is_owner )
         set_session_variable( get_special_var_name( e_special_var_blockchain_peer_is_owner ), c_true_value );

      if( has_found_both_are_owners )
         set_session_variable( get_special_var_name( e_special_var_blockchain_both_are_owners ), c_true_value );

      if( is_for_support || has_support_sessions )
         set_session_variable( get_special_var_name( e_special_var_blockchain_peer_has_supporters ), c_true_value );

      okay = true;
      was_initialised = true;

      bool has_zenith = false;
      size_t blockchain_height = 0;

      string hello_data, hello_hash;
      hello_data = get_hello_data( hello_hash );

      // NOTE: Create the dummy "hello" blob as it will be required.
      if( !has_file( hello_hash ) )
         create_raw_file( hello_data, false );

      cmd_handler.prior_put( ) = hello_hash;

      if( !blockchain.empty( ) )
      {
         if( has_tag( blockchain + c_zenith_suffix ) )
         {
            string zenith_hash( tag_file_hash( blockchain + c_zenith_suffix ) );

            if( get_block_height_from_tags( blockchain, zenith_hash, blockchain_height ) )
            {
               has_zenith = true;
               cmd_handler.set_blockchain_height( blockchain_height );

               set_session_variable( get_special_var_name(
                e_special_var_blockchain_zenith_height ), to_string( blockchain_height ) );
            }
         }

         if( !identity.empty( ) )
         {
            set_session_variable( identity, c_true_value );
            set_session_variable( get_special_var_name( e_special_var_identity ), identity );
         }
      }

      if( !is_responder )
      {
         string hash_or_tag;

         if( !is_for_support && !blockchain.empty( ) )
         {
            hash_or_tag = blockchain + '.' + to_string( blockchain_height ) + string( c_blk_suffix );

            // NOTE: In case the responder does not have the genesis block include
            // its hash as a dummy "nonce" (to be used by the responder for "get").
            string genesis_block_tag( blockchain + ".0" + string( c_blk_suffix ) );

            if( has_tag( genesis_block_tag ) )
               hash_or_tag += ' ' + tag_file_hash( genesis_block_tag );
         }

         if( hash_or_tag.empty( ) )
            hash_or_tag = hello_hash;

         ap_socket->write_line( string( c_cmd_peer_session_chk ) + " " + hash_or_tag, c_request_timeout, p_progress );

         cmd_handler.state( ) = e_peer_state_waiting_for_put;
         cmd_handler.op_state( ) = e_peer_state_waiting_for_put;

         if( !blockchain.empty( ) )
         {
            string block_hash;

            if( ap_socket->read_line( block_hash, c_request_timeout, c_max_line_length, p_progress ) <= 0 )
               okay = false;
            else if( !is_for_support && ( block_hash != string( c_response_not_found ) ) )
               add_peer_file_hash_for_get( block_hash );
         }
      }

      if( okay )
      {
         TRACE_LOG( TRACE_SESSIONS,
          string( "started peer session " )
          + ( !is_responder ? "(as initiator)" : "(as responder)" )
          + ( blockchain.empty( ) ? "" : " for blockchain " + blockchain )
          + ( !is_owner ? "" : " owner" )
          + ( !is_for_support ? "" : " support" )
          + " (tid = " + to_string( current_thread_id( ) ) + ")" );

         socket_command_processor processor( *ap_socket, cmd_handler, is_local, is_owner, is_responder );
         processor.process_commands( );
      }

      ap_socket->close( );

      if( was_initialised )
      {
         term_session( );
         has_terminated = true;
      }
   }
   catch( exception& x )
   {
      issue_error( x.what( ), true );

      ap_socket->write_line( string( c_response_error_prefix ) + x.what( ), c_request_timeout );
      ap_socket->close( );

      if( was_initialised )
      {
         if( !is_for_support && !blockchain.empty( ) )
            condemn_matching_sessions( );

         term_session( );
         has_terminated = true;
      }
   }
   catch( ... )
   {
      issue_error( "unexpected unknown exception occurred" );

      ap_socket->write_line( string( c_response_error_prefix ) + "unexpected exception occurred", c_request_timeout );
      ap_socket->close( );

      if( was_initialised )
      {
         if( !is_for_support && !blockchain.empty( ) )
            condemn_matching_sessions( );

         term_session( );
         has_terminated = true;
      }
   }

   if( has_terminated && !is_for_support && !identity.empty( ) )
   {
      set_variable_checker check_no_other_session(
       e_variable_check_type_no_session_has, identity );

      set_system_variable( '~' + identity, "", check_no_other_session );
   }

   delete this;
}

void peer_session::increment_session_count( )
{
   guard g( g_mutex );

   ++g_num_peers;
   ciyam_session::increment_session_count( );
}

void peer_session::decrement_session_count( )
{
   guard g( g_mutex );

   --g_num_peers;
   ciyam_session::decrement_session_count( );
}

void peer_listener::on_start( )
{
   try
   {
      increment_active_listeners( );

      tcp_socket s;
      bool okay = s.open( );

      ip_address address( port );

      if( okay )
      {
         s.set_reuse_addr( );

         string listener_name( "peer" );

         string unprefixed_blockchains( blockchains );
         replace( unprefixed_blockchains, c_bc_prefix, "" );

         listener_registration registration( port, listener_name, unprefixed_blockchains.c_str( ) );

         okay = s.bind( address );

         if( okay )
            okay = s.listen( );

         if( okay )
         {
            TRACE_LOG( TRACE_ANYTHING, "peer listener started on tcp port " + to_string( port ) );

            while( s && !g_server_shutdown )
            {
               string identity_changes( get_system_variable( '@' + to_string( port ) ) );

               // NOTE; Uses the system variable "@<port>" in order to insert or remove blockchains from the listener.
               if( !identity_changes.empty( ) )
               {
                  bool is_remove = false;

                  set_system_variable( '@' + to_string( port ), "" );

                  if( identity_changes[ 0 ] == '~' )
                  {
                     is_remove = true;
                     identity_changes.erase( 0, 1 );
                  }

                  if( !identity_changes.empty( ) )
                  {
                     set< string > existing_blockchains;

                     split( unprefixed_blockchains, existing_blockchains );

                     vector< string > all_changes;
                     split( identity_changes, all_changes );

                     for( size_t i = 0; i < all_changes.size( ); i++ )
                     {
                        string next( all_changes[ i ] );

                        if( is_remove )
                        {
                           if( existing_blockchains.count( next ) )
                           {
                              registration.remove_id( next );
                              existing_blockchains.erase( next );

                              set_system_variable( c_error_message_prefix + next, "" );
                           }
                        }
                        else
                        {
                           if( !existing_blockchains.count( next ) )
                           {
                              registration.insert_id( next );
                              existing_blockchains.insert( next );

                              set_system_variable( c_error_message_prefix + next, "" );
                           }
                        }
                     }

                     if( existing_blockchains.empty( ) )
                        break;

                     unprefixed_blockchains.erase( );

                     for( set< string >::iterator i = existing_blockchains.begin( ); i != existing_blockchains.end( ); ++i )
                     {
                        if( !unprefixed_blockchains.empty( ) )
                           unprefixed_blockchains += ',';
                        unprefixed_blockchains += *i;
                     }

                     blockchains = prefixed_blockchains( unprefixed_blockchains );
                  }
               }

               date_time dtm( date_time::local( ) );

               // NOTE: Check for accepts and create new sessions.
#ifdef SSL_SUPPORT
               auto_ptr< ssl_socket > ap_socket( new ssl_socket( s.accept( address, c_accept_timeout ) ) );
#else
               auto_ptr< tcp_socket > ap_socket( new tcp_socket( s.accept( address, c_accept_timeout ) ) );
#endif
               if( !g_server_shutdown && *ap_socket
                && !has_max_peers( ) && get_is_accepted_peer_ip_addr( address.get_addr_string( ) ) )
               {
                  peer_session* p_session = 0;

                  // NOTE: Need to assume it is a support session initially (the first read will
                  // determine if it actually is or not).
                  try
                  {
                     p_session = construct_session( dtm,
                      true, ap_socket, address.get_addr_string( ) + '=' + blockchains, true );
                  }
                  catch( exception& x )
                  {
                     issue_error( x.what( ) );
                  }
                  catch( ... )
                  {
                     issue_error( "unexpected unknown exception occurred constructing session" );
                  }

                  if( p_session )
                     p_session->start( );
               }
            }

            s.close( );
         }
         else
         {
            s.close( );
            issue_error( "unexpected socket error" );

            if( !unprefixed_blockchains.empty( ) )
            {
               vector< string > identities;
               split( unprefixed_blockchains, identities );

               // FUTURE: This message should be handled as a server string message.
               string error( "Unable to start peerchain using port " + to_string( port ) + "." );

               for( size_t i = 0; i < identities.size( ); i++ )
                  set_system_variable( c_error_message_prefix + identities[ i ], error );
            }
         }
      }
   }
   catch( exception& x )
   {
      issue_error( x.what( ) );
   }
   catch( ... )
   {
      issue_error( "unexpected unknown exception occurred" );
   }

   TRACE_LOG( TRACE_ANYTHING, "peer listener finished (tcp port " + to_string( port ) + ")" );

   decrement_active_listeners( );

   delete this;
}

void list_mutex_lock_ids_for_peer_session( std::ostream& outs )
{
   outs << "peer_session::g_mutex = " << g_mutex.get_lock_id( ) << '\n';
}

string prefixed_blockchains( const string& blockchains )
{
   string retval;

   if( !blockchains.empty( ) )
   {
      vector< string > all_blockchains;

      split( blockchains, all_blockchains );

      for( size_t i = 0; i < all_blockchains.size( ); i++ )
      {
         string next( all_blockchains[ i ] );

         if( !retval.empty( ) )
            retval += ',';

         if( next.find( c_bc_prefix ) != 0 )
            retval += c_bc_prefix;

         retval += next;
      }
   }

   return retval;
}

string unprefixed_blockchains( const string& blockchains )
{
   string retval;

   string identities( blockchains );
   replace( identities, c_bc_prefix, "" );

   return identities;
}

void create_peer_listener( int port, const string& blockchains )
{
   if( has_registered_listener( port ) )
      set_system_variable( '@' + to_string( port ), blockchains );
   else
   {
#ifdef __GNUG__
      if( port < 1024 )
         throw runtime_error( "invalid attempt to use port number less than 1024" );
#endif

      peer_listener* p_peer_listener = new peer_listener( port, prefixed_blockchains( blockchains ) );
      p_peer_listener->start( );
   }
}

void create_peer_initiator(
 const string& blockchain, const string& host_and_or_port,
 bool force, size_t num_for_support, bool is_interactive, bool is_secondary )
{
   if( g_server_shutdown )
      return;

   if( blockchain.empty( ) )
      throw runtime_error( "create_peer_initiator called with empty blockchain identity" );

   if( has_max_peers( ) )
      throw runtime_error( "server has reached its maximum peer limit" );

   if( num_for_support > c_max_num_for_support )
      throw runtime_error( "cannot create " + to_string( num_for_support )
       + " sessions for support (max is " + to_string( c_max_num_for_support ) + ")" );

   int port = 0;
   string host( c_local_host );

   parse_host_and_or_port( host_and_or_port, host, port );

   if( !port )
      throw runtime_error( "invalid or missing port in '" + host_and_or_port + "' for create_peer_initiator" );

   size_t total_to_create = 1 + num_for_support;

   peer_session* p_main_session = 0;
   size_t num_supporters_created = 0;

   size_t prefix_length = strlen( c_bc_prefix );

   string identity( blockchain );
   replace( identity, c_bc_prefix, "" );

   if( !is_interactive )
      set_system_variable( c_error_message_prefix + identity, "" );

   ip_address address( host.c_str( ), port );

   string ip_addr( address.get_addr_string( ) );

   if( ip_addr == c_null_ip_addr || ip_addr == c_null_ip_addr_for_ipv6 )
   {
      // FUTURE: This message should be handled as a server string message.
      string error( "Unable to determine address for domain name '" + host + "'." );

      if( is_interactive )
         throw runtime_error( error );
      else
      {
         set_system_variable( c_error_message_prefix + identity, error );
         return;
      }
   }

   string own_identity( get_system_variable(
    get_special_var_name( e_special_var_blockchain ) ) );

   bool has_separate_identity = false;

   if( !is_secondary && !own_identity.empty( ) && ( identity != own_identity ) )
      has_separate_identity = true;

   string session_blockchain( c_bc_prefix );

   if( !has_separate_identity )
      session_blockchain += identity;
   else
      session_blockchain += own_identity;

   date_time dtm( date_time::local( ) );

   temporary_system_variable tmp_blockchain_connect( identity, c_true_value );

   for( size_t i = 0; i < total_to_create; i++ )
   {
#ifdef SSL_SUPPORT
      auto_ptr< ssl_socket > ap_socket( new ssl_socket );
#else
      auto_ptr< tcp_socket > ap_socket( new tcp_socket );
#endif

      if( ap_socket->open( ) )
      {
         if( force )
            remove_peer_ip_addr_from_rejection( ip_addr );
         else if( !get_is_accepted_peer_ip_addr( ip_addr ) )
         {
            // FUTURE: This message should be handled as a server string message.
            string error( "Host '" + host + "' address " + ip_addr + " is not permitted." );

            if( is_interactive )
               throw runtime_error( error );
            else
            {
               set_system_variable( c_error_message_prefix + identity, error );
               return;
            }
         }

         if( ap_socket->connect( address, ( i == 0 ) ? c_initial_timeout : c_support_timeout ) )
         {
            const char* p_identity = 0;
            peer_extra extra = e_peer_extra_none;

            if( i == 0 && ( is_secondary || has_separate_identity ) )
            {
               p_identity = identity.c_str( );
               extra = ( !is_secondary ? e_peer_extra_primary : e_peer_extra_secondary );
            }

            peer_session* p_session = construct_session( dtm, false, ap_socket,
             ip_addr + "=" + session_blockchain + ":" + to_string( port ), i > 0, extra, p_identity );

            if( !p_session )
               break;
            else
            {
               if( !p_main_session && total_to_create > 1 )
               {
                  p_main_session = p_session;
                  p_main_session->set_has_support_sessions( );
               }

               p_session->start( );

               if( i > 0 )
                  ++num_supporters_created;
            }
         }
         else if( i == 0 )
         {
            string error;

            if( !ap_socket->had_timeout( ) )
               // FUTURE: This message should be handled as a server string message.
               error = "Failed trying to connect to '"
                + host + "' using port " + to_string( port ) + ".";
            else
               // FUTURE: This message should be handled as a server string message.
               error = "Timed out trying to connect to '"
                + host + "' using port " + to_string( port ) + ".";

            if( is_interactive )
               throw runtime_error( error );
            else
            {
               set_system_variable( c_error_message_prefix + identity, error );
               return;
            }
         }
      }
   }
}

peer_session_starter::peer_session_starter( )
{
   ciyam_session::increment_session_count( );
}

peer_session_starter::~peer_session_starter( )
{
   ciyam_session::decrement_session_count( );
}

void peer_session_starter::on_start( )
{
   try
   {
      vector< string > peerchain_externals;

      get_peerchain_externals( peerchain_externals );

      for( size_t i = 0; i < peerchain_externals.size( ); i++ )
      {
         if( g_server_shutdown || has_max_peers( ) )
            break;

         start_peer_session( peerchain_externals[ i ] );
      }

      while( true )
      {
         if( g_server_shutdown || has_max_peers( ) )
            break;

         string identity( get_system_variable( get_special_var_name( e_special_var_queue_peers ) ) );

         if( identity.empty( ) )
            msleep( c_sleep_time );
         else
         {
            bool is_listener = false;
            bool is_secondary = false;

            // NOTE: If identity is prefixed with '!' then will always start a listener.
            if( identity[ 0 ] == '!' )
            {
               is_listener = true;
               identity.erase( 0, 1 );
            }

            string peer_info( get_peerchain_info( identity, is_listener ? 0 : &is_listener ) );

            if( !peer_info.empty( ) )
            {
               if( !is_listener )
                  start_peer_session( peer_info );
               else
               {
                  string::size_type pos = peer_info.find( '=' );
                  if( pos != string::npos )
                  {
                     int port = from_string< int >( peer_info.substr( pos + 1 ) );
                     create_peer_listener( port, peer_info.substr( 0, pos ) );
                  }
               }
            }
         }
      }
   }
   catch( exception& x )
   {
#ifdef DEBUG
      cout << "peer_session_starter error: " << x.what( ) << endl;
#endif
      TRACE_LOG( TRACE_ANYTHING, string( "peer_session_starter error: " ) + x.what( ) );
   }
   catch( ... )
   {
#ifdef DEBUG
      cout << "unexpected peer_session_starter exception..." << endl;
#endif
      TRACE_LOG( TRACE_ANYTHING, "peer_session_starter error: unexpected unknown exception caught" );
   }

   delete this;
}

void peer_session_starter::start_peer_session( const string& peer_info )
{
   string info( peer_info );

   string::size_type pos = info.find( '=' );
   if( pos == string::npos )
      throw runtime_error( "unexpected peer_info '" + peer_info + "'" );

   size_t num_for_support = 0;

   string blockchain( c_bc_prefix + info.substr( 0, pos ) );
   info.erase( 0, pos + 1 );

   pos = blockchain.find( '+' );
   if( pos != string::npos )
   {
      num_for_support = from_string< size_t >( blockchain.substr( pos + 1 ) );
      blockchain.erase( pos );
   }

   string identity( replaced( blockchain, c_bc_prefix, "" ) );

   set_system_variable( c_error_message_prefix + identity, "" );

   temporary_system_variable tmp_blockchain_connect( identity, c_true_value );

   // NOTE: Create sessions for both the local and hosted blockchains.
   create_peer_initiator( blockchain, info, false, num_for_support, false );
   create_peer_initiator( blockchain, info, false, num_for_support, false, true );

   // NOTE: As connections are handled by separate threads need to wait some time
   // here to determine whether both connections have been created successfully.
   for( size_t i = 0; i < 10; i++ )
   {
      msleep( c_peer_sleep_time );

      if( !has_any_session_variable( identity ) )
         break;
   }
}

void init_peer_sessions( int start_listeners )
{
   if( start_listeners )
   {
      int test_peer_port = get_test_peer_port( );

      if( test_peer_port > 0 )
         create_peer_listener( test_peer_port, "" );

      multimap< int, string > peerchain_listeners;
      get_peerchain_listeners( peerchain_listeners );

      map< int, string > port_blockchains;

      for( multimap< int, string >::iterator i = peerchain_listeners.begin( ); i != peerchain_listeners.end( ); ++i )
      {
         string next_blockchain( c_bc_prefix + i->second );

         set_system_variable( c_error_message_prefix + i->second, "" );

         string::size_type pos = next_blockchain.find( '_' );

         if( pos != string::npos )
            next_blockchain.erase( pos );

         if( !port_blockchains.count( i->first ) )
            port_blockchains.insert( make_pair( i->first, next_blockchain ) );
         else
         {
            string blockchains( port_blockchains.find( i->first )->second + ',' + next_blockchain );
            port_blockchains[ i->first ] = blockchains;
         }
      }

      for( map< int, string >::iterator i = port_blockchains.begin( ); i != port_blockchains.end( ); ++i )
         create_peer_listener( i->first, i->second );
   }

   peer_session_starter* p_peer_session_start = new peer_session_starter;
   p_peer_session_start->start( );
}
