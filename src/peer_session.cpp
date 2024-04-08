// Copyright (c) 2014-2024 CIYAM Developers
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
#  include <iomanip>
#  include <sstream>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#define CIYAM_BASE_IMPL

#include "peer_session.h"

#include "regex.h"
#include "base64.h"
#include "config.h"
#include "format.h"
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

trace_mutex g_mutex;

const char c_blob_separator = '&';

const char c_local_prefix = '!';
const char c_external_prefix = '@';

const char c_repository_suffix = '!';
const char c_tree_files_suffix = '+';

const char c_error_message_prefix = '#';
const char c_progress_output_prefix = '%';

const char* const c_hello = "hello";

const char* const c_dummy_suffix = ".dummy";

const char* const c_dummy_peer_tag = "peer";
const char* const c_dummy_support_tag = "support";

const char* const c_dummy_message_data = "utf8:test";

const char* const c_percentage_separator = " - ";

const size_t c_prefix_length = 4;

const size_t c_dummy_num_for_support = 999;
const size_t c_default_progress_seconds = 2;

const int c_accept_timeout = 250;
const int c_max_line_length = 500;

const int c_max_num_for_support = 11;

const size_t c_max_pubkey_size = 256;
const size_t c_max_greeting_size = 256;
const size_t c_max_put_blob_size = 256;

const size_t c_num_lamport_lines = 256;

const size_t c_num_base64_key_chars = 44;
const size_t c_key_pair_separator_pos = 44;

const size_t c_wait_sleep_time = 250;
const size_t c_start_sleep_time = 2500;

const size_t c_initial_timeout = 25000;
const size_t c_request_timeout = 60000;
const size_t c_support_timeout = 15000;

const size_t c_num_hash_rounds = 1000000;

const size_t c_num_check_disconnected = 8;

enum op
{
   e_op_chk,
   e_op_msg,
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

enum public_key_scale
{
   e_public_key_scale_primary,
   e_public_key_scale_secondary,
   e_public_key_scale_tertiary
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

string get_hub_identity( const string& own_identity )
{
   string hub_identity_var_name( get_special_var_name( e_special_var_blockchain_peer_hub_identity ) );
   string backup_identity_var_name( get_special_var_name( e_special_var_blockchain_backup_identity ) );

   if( own_identity != get_system_variable( backup_identity_var_name ) )
   {
      string extra( get_identity_variable_extra( backup_identity_var_name, own_identity ) );

      if( !extra.empty( ) )
      {
         string hub_identity_name_prefix, hub_identity_name_suffix;

         identity_variable_name_prefix_and_suffix(
          hub_identity_var_name, hub_identity_name_prefix, hub_identity_name_suffix );

         hub_identity_var_name = hub_identity_name_prefix + extra + hub_identity_name_suffix;
      }
   }

   return get_system_variable( hub_identity_var_name );
}

string get_own_identity( bool is_shared, const string* p_extra )
{
   string own_identity;

   if( !p_extra )
      own_identity = get_system_variable( get_special_var_name(
       ( !is_shared ) ? e_special_var_blockchain_backup_identity : e_special_var_blockchain_shared_identity ) );
   else
      own_identity = get_extra_identity_variable( get_special_var_name(
       ( !is_shared ) ? e_special_var_blockchain_backup_identity : e_special_var_blockchain_shared_identity ), *p_extra );

   return own_identity;
}

const size_t c_max_map_keys = 1000;

bool g_keys_populated = false;

deque< string > g_peer_map_keys;

string obtain_peer_map_key( )
{
   guard g( g_mutex );

   string retval;

   if( !g_keys_populated )
   {
      g_keys_populated = true;

      size_t key_len = log10( c_max_map_keys );

      for( size_t i = 0; i < c_max_map_keys; i++ )
         g_peer_map_keys.push_back( to_comparable_string( i, false, key_len ) );
   }

   if( g_peer_map_keys.empty( ) )
      throw runtime_error( "unable to locate an unused peer map key" );

   retval = g_peer_map_keys.front( );
   g_peer_map_keys.pop_front( );

   return retval;
}

void restore_peer_map_key( const string& key )
{
   guard g( g_mutex );

   g_peer_map_keys.push_back( key );
}

string get_peer_map_key( bool mandatory = true )
{
   string peer_map_key_var_name(
    get_special_var_name( e_special_var_peer_map_key ) );

   string retval( get_raw_session_variable( peer_map_key_var_name ) );

   if( retval.empty( ) )
   {
      set_session_variable( peer_map_key_var_name,
       get_session_variable_from_matching_blockchain( peer_map_key_var_name,
       get_special_var_name( e_special_var_blockchain_peer_supporter ), "", true ) );

      retval = get_raw_session_variable( peer_map_key_var_name );
   }

   if( mandatory && retval.empty( ) )
      throw runtime_error( "unexpected empty peer map key value" );

   return retval;
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

void add_to_blockchain_tree_item( const string& blockchain, size_t num_to_add, size_t upper_limit = 0 )
{
   guard g( g_mutex );

   g_blockchain_tree_item[ blockchain ] += num_to_add;

   if( upper_limit && g_blockchain_tree_item[ blockchain ] > upper_limit )
      g_blockchain_tree_item[ blockchain ] = upper_limit;
}

map< string, size_t > g_peer_total_items;

map< string, size_t > g_peer_found_bounding;
map< string, size_t > g_peer_found_prefixed;

map< string, map< string, size_t > > g_peer_first_prefixed;

void save_first_prefixed( map< string, size_t >& first_prefixed, size_t total_items )
{
   guard g( g_mutex );

   string peer_map_key( get_peer_map_key( ) );

   g_peer_total_items[ peer_map_key ] = total_items;

   g_peer_found_bounding[ peer_map_key ] = 0;
   g_peer_found_prefixed[ peer_map_key ] = 0;

   g_peer_first_prefixed[ peer_map_key ].swap( first_prefixed );

   map< string, size_t >::const_iterator ci = g_peer_first_prefixed[ peer_map_key ].begin( );
   map< string, size_t >::const_iterator cend = g_peer_first_prefixed[ peer_map_key ].end( );

   size_t first = ( total_items + 1 );

   while( ci != cend )
   {
      if( ci->second < first )
         first = ci->second;

      ++ci;
   }

   if( first > 1 )
      g_peer_found_bounding[ peer_map_key ] = ( first - 1 );
}

void check_found_prefixed( const string& hash )
{
   guard g( g_mutex );

   string peer_map_key( get_peer_map_key( ) );

   if( g_peer_first_prefixed.count( peer_map_key ) )
   {
      string prefix( c_prefix_length, '\0' );
      hex_decode( hash.substr( 0, c_prefix_length * 2 ), ( unsigned char* )prefix.data( ), c_prefix_length );

      bool increment = false;
      size_t last_found = g_peer_found_prefixed[ peer_map_key ];

      if( !g_peer_first_prefixed[ peer_map_key ].count( prefix ) )
         increment = true;
      else
      {
         size_t next_found = g_peer_first_prefixed[ peer_map_key ][ prefix ];

         if( next_found > last_found )
         {
            g_peer_found_prefixed[ peer_map_key ] = next_found;

            // NOTE: As there can be large gaps between unique prefixes when jumping to the next item
            // number will also find the item number that follows in order to permit incrementing the
            // count by one whilst less than the next mapped item (or to the total number of items).
            map< string, size_t >::const_iterator ci = g_peer_first_prefixed[ peer_map_key ].begin( );
            map< string, size_t >::const_iterator cend = g_peer_first_prefixed[ peer_map_key ].end( );

            size_t following = ( g_peer_total_items[ peer_map_key ] + 1 );

            while( ci != cend )
            {
               if( ( ci->second > next_found ) && ( ci->second < following ) )
                  following = ci->second;

               ++ci;
            }

            if( following <= next_found )
               g_peer_found_bounding[ peer_map_key ] = next_found;
            else
               g_peer_found_bounding[ peer_map_key ] = ( following - 1 );

            if( get_raw_session_variable(
             get_special_var_name( e_special_var_blockchain_peer_supporter ) ).empty( ) )
               set_session_variable( get_special_var_name( e_special_var_tree_count ), to_string( next_found ) );
         }
         else
            increment = true;
      }

      if( increment )
      {
         if( last_found < g_peer_found_bounding[ peer_map_key ] )
            g_peer_found_prefixed[ peer_map_key ] = ++last_found;

         if( get_raw_session_variable(
          get_special_var_name( e_special_var_blockchain_peer_supporter ) ).empty( ) )
            set_session_variable( get_special_var_name( e_special_var_tree_count ), to_string( last_found ) );
      }
   }
}

void clear_first_prefixed( )
{
   guard g( g_mutex );

   string peer_map_key( get_peer_map_key( ) );

   g_peer_total_items[ peer_map_key ] = 0;

   g_peer_found_bounding[ peer_map_key ] = 0;
   g_peer_found_prefixed[ peer_map_key ] = 0;

   g_peer_first_prefixed[ peer_map_key ].clear( );
}

void clear_first_prefixed( const string& peer_map_key )
{
   guard g( g_mutex );

   g_peer_total_items[ peer_map_key ] = 0;

   g_peer_found_bounding[ peer_map_key ] = 0;
   g_peer_found_prefixed[ peer_map_key ] = 0;

   g_peer_first_prefixed[ peer_map_key ].clear( );
}

void add_to_num_puts_value( size_t num_to_add )
{
   size_t num_puts = 0;

   string num_puts_name( get_special_var_name( e_special_var_blockchain_num_puts ) );

   string num_puts_value( get_raw_session_variable( num_puts_name ) );

   if( !num_puts_value.empty( ) )
      num_puts = from_string< size_t >( num_puts_value );

   num_puts += num_to_add;

   if( num_puts )
      set_session_variable( num_puts_name, to_string( num_puts ) );
}

void set_waiting_for_hub_progress( const string& identity, const string& hub_identity )
{
   set_session_variable(
    get_special_var_name( e_special_var_blockchain_waiting_for_hub ), c_true_value );

   // FUTURE: This message should be handled as a server string message.
   string progress_message( "Waiting for hub peer sync..." );

   set_session_progress_message( progress_message );
   set_system_variable( c_progress_output_prefix + identity, progress_message );
}

void system_identity_progress_message( const string& identity )
{
   bool is_blockchain_owner = !get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_is_owner ) ).empty( );

   string blockchain_height_other_name(
    get_special_var_name( e_special_var_blockchain_height_other ) );

   string blockchain_zenith_height_name(
    get_special_var_name( e_special_var_blockchain_zenith_height ) );

   string blockchain_height_procesing_name(
    get_special_var_name( e_special_var_blockchain_height_processing ) );

   string progress_value_name( get_special_var_name( e_special_var_progress_value ) );
   string progress_message_name( get_special_var_name( e_special_var_progress_message ) );

   size_t base_height = from_string< size_t >(
    get_raw_session_variable( blockchain_height_procesing_name ) );

   size_t zenith_height = from_string< size_t >(
    get_raw_session_variable( blockchain_zenith_height_name ) );

   if( !base_height )
      base_height = zenith_height;

   size_t other_height = from_string< size_t >(
    get_raw_session_variable( blockchain_height_other_name ) );

   string percentage_value( get_raw_session_variable( progress_value_name ) );
   string progress_message( get_raw_session_variable( progress_message_name ) );

   size_t paired_base_height = 0;
   size_t paired_other_height = 0;

   bool is_changing = ( progress_message.find( c_ellipsis ) != string::npos );

   // FUTURE: This message should be handled as a server string message.
   string prefix( "Currently at height " + to_string( zenith_height ) );

   if( is_changing )
   {
      string::size_type pos = progress_message.find( c_percentage_separator );

      if( pos != string::npos )
      {
         prefix = progress_message.substr( 0, pos );
         percentage_value = progress_message.substr( pos + strlen( c_percentage_separator ) );
      }
   }

   bool has_paired_session = false;
   bool paired_is_changing = false;

   string paired_identity_name(
    get_special_var_name( e_special_var_paired_identity ) );

   string own_paired_identity( get_raw_session_variable( paired_identity_name ) );

   if( !is_blockchain_owner && !own_paired_identity.empty( ) )
   {
      size_t paired_session_id = first_other_session_id( paired_identity_name, own_paired_identity );

      if( paired_session_id )
      {
         has_paired_session = true;

         string paired_progress_message( get_raw_session_variable( progress_message_name, paired_session_id ) );

         paired_is_changing = ( paired_progress_message.find( c_ellipsis ) != string::npos );

         paired_base_height = from_string< size_t >(
          get_raw_session_variable( blockchain_height_procesing_name, paired_session_id ) );

         if( !paired_base_height )
            paired_base_height = from_string< size_t >(
             get_raw_session_variable( blockchain_zenith_height_name, paired_session_id ) );

         paired_other_height = from_string< size_t >(
          get_raw_session_variable( blockchain_height_other_name, paired_session_id ) );

         if( paired_is_changing )
         {
            if( get_raw_session_variable( get_special_var_name(
             e_special_var_blockchain_is_fetching ), paired_session_id ).empty( ) )
               ++paired_other_height;
         }

         if( !is_changing && paired_is_changing )
         {
            string::size_type pos = paired_progress_message.find( c_percentage_separator );

            if( pos == string::npos )
               percentage_value = get_raw_session_variable( progress_value_name, paired_session_id );
            else
            {
               prefix = paired_progress_message.substr( 0, pos );
               percentage_value = paired_progress_message.substr( pos + strlen( c_percentage_separator ) );

               pos = prefix.rfind( ' ' );

               if( pos != string::npos )
               {
                  prefix.erase( pos + 1 );

                  prefix += to_string( base_height );

                  if( other_height > base_height )
                     prefix += '/' + to_string( other_height );
               }
            }
         }
      }
   }

   string identity_progress_message( prefix );

   if( has_paired_session )
   {
      identity_progress_message += " (" + to_string( paired_other_height );

      if( paired_base_height > paired_other_height )
      {
         paired_is_changing = true;
         identity_progress_message += '/' + to_string( paired_base_height );
      }

      identity_progress_message += ')';
   }

   if( is_changing || paired_is_changing )
   {
      if( !percentage_value.empty( ) )
      {
         identity_progress_message += c_percentage_separator;
         identity_progress_message += percentage_value;
      }

      identity_progress_message += c_ellipsis;
   }

   set_system_variable( c_progress_output_prefix + identity, identity_progress_message );
}

void output_sync_progress_message( const string& identity,
 size_t blockchain_height, size_t blockchain_height_other = 0, bool setting_new_zenith = false )
{
   string backup_identity( get_system_variable(
    get_special_var_name( e_special_var_blockchain_backup_identity ) ) );

   string shared_identity( get_system_variable(
    get_special_var_name( e_special_var_blockchain_shared_identity ) ) );

   string peer_hub_identity( get_system_variable(
    get_special_var_name( e_special_var_blockchain_peer_hub_identity ) ) );

   string progress_message;

   bool is_backup = ( identity == backup_identity );
   bool is_shared = ( identity == shared_identity );
   bool is_peer_hub = ( identity == peer_hub_identity );

   // FUTURE: This message should be handled as a server string message.
   progress_message = "Currently at height ";

   // NOTE: In general it would not be expected for this to occur as the
   // only blockchains that should be exposed publicly are "extra" ones.
   if( setting_new_zenith && ( is_backup || is_shared || is_peer_hub ) )
   {
      string current_height( to_string( blockchain_height ) );

      string blockchain_height_name;

      if( is_backup )
         blockchain_height_name = get_special_var_name( e_special_var_blockchain_backup_height );
      else if( is_shared )
         blockchain_height_name = get_special_var_name( e_special_var_blockchain_shared_height );
      else
         blockchain_height_name = get_special_var_name( e_special_var_blockchain_peer_hub_height );

      if( get_system_variable( blockchain_height_name ) != current_height )
         set_system_variable( ">" + blockchain_height_name, current_height );
   }

   progress_message += to_string( blockchain_height );

   if( blockchain_height_other > blockchain_height )
   {
      progress_message += '/' + to_string( blockchain_height_other );
      progress_message += c_ellipsis;
   }

   bool has_zenith_height = !get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_zenith_height ) ).empty( );

   if( has_zenith_height )
   {
      set_session_progress_message( progress_message );

      system_identity_progress_message( identity );
   }
   else
      // NOTE: If no '@blockchain_zenith_height' session variable exists then
      // assume that the call was from a listener rather than a peer session.
      set_system_variable( c_progress_output_prefix + identity, progress_message );
}

string get_hello_data( string& hello_hash )
{
   string data( c_file_type_str_blob );
   data += string( c_hello );

   hello_hash = sha256( data ).get_digest_as_string( );

   return data;
}

string xor_hashes( const string& hash1, const string& hash2 )
{
   string hash;

   unsigned char buf1[ c_sha256_digest_size ];
   unsigned char buf2[ c_sha256_digest_size ];

   hex_decode( hash1, buf1, sizeof( buf1 ) );
   hex_decode( hash2, buf2, sizeof( buf2 ) );

   for( size_t i = 0; i < c_sha256_digest_size; i++ )
      buf1[ i ] ^= buf2[ i ];

   hex_encode( hash, buf1, sizeof( buf1 ) );

   return hash;
}

string file_hash_for_read( const tcp_socket& socket, const string& hash_input )
{
   string hash( hash_input );

   string secret( get_session_secret( ) );

   if( !secret.empty( ) && are_hex_nibbles( hash ) )
   {
      string dummy( sha256( to_string(
       socket.get_num_read_lines( ) ) + secret ).get_digest_as_string( ) );

      hash = xor_hashes( hash, dummy );

      clear_key( secret );
   }

   return hash;
}

string file_hash_for_write( const tcp_socket& socket, const string& hash_or_tag )
{
   string hash( hash_or_tag );

   if( has_tag( hash_or_tag ) )
      hash = tag_file_hash( hash_or_tag );

   string secret( get_session_secret( ) );

   if( !secret.empty( ) && !hash.empty( ) && are_hex_nibbles( hash ) )
   {
      string dummy( sha256( to_string(
       socket.get_num_write_lines( ) + 1 ) + secret ).get_digest_as_string( ) );

      hash = xor_hashes( hash, dummy );

      clear_key( secret );
   }

   return hash;
}

void parse_peer_mapped_info( const string& peer_mapped_info, string& local_hash,
 string& local_public_key, string& master_public_key, bool base64_encode_pubkeys, bool is_test_session = false )
{
   if( !is_test_session && peer_mapped_info.length( ) != 197 )
      throw runtime_error( "unexpected peer_mapped_info length != 197" );

   // NOTE: Mapped info is '<local_hash>:<local_pub_key><master_pub_key>'.
   local_hash = peer_mapped_info.substr( 0, 64 );
   local_public_key = peer_mapped_info.substr( 65, 66 );

   if( !is_test_session )
      master_public_key = peer_mapped_info.substr( 131 );

   if( base64_encode_pubkeys )
   {
      local_public_key = base64::encode( hex_decode( local_public_key ) );

      if( !is_test_session )
         master_public_key = base64::encode( hex_decode( master_public_key ) );
   }
}

bool terminate_peer_session( bool is_for_support, const string& identity )
{
   if( !is_for_support && !identity.empty( ) )
   {
      condemn_matching_sessions( );

      while( has_any_matching_session( true ) )
         msleep( c_wait_sleep_time );

      string peer_map_key( get_raw_session_variable(
       get_special_var_name( e_special_var_peer_map_key ) ) );

      if( !peer_map_key.empty( ) )
      {
         restore_peer_map_key( peer_map_key );

         clear_first_prefixed( peer_map_key );

         clear_all_peer_mapped_hashes( peer_map_key );
         clear_all_peer_mapped_hashes( c_local_prefix + peer_map_key );
         clear_all_peer_mapped_hashes( c_external_prefix + peer_map_key );
      }
   }

   term_session( );

   return true;
}

string determine_identity( const string& concatenated_pubkey_hashes )
{
   sha256 hash( concatenated_pubkey_hashes );

   string identity( hash.get_digest_as_string( ).substr( 0, c_bc_identity_length ) );

   // NOTE: Shared blockchains will use the backup identity in reverse so if the
   // identity value is palendromic then increment (or zero) the last character.
   string reversed( identity );
   reverse( reversed.begin( ), reversed.end( ) );

   if( identity == reversed )
   {
      char ch = identity[ identity.length( ) - 1 ];

      ++ch;

      if( ch == 'g' )
         ch = '0';

      identity[ identity.length( ) - 1 ] = ch;
   }

   return identity;
}

bool is_targeted_identity( const string& identity,
 const string& targeted_identity, const string& tree_root_hash, size_t blockchain_height )
{
   string password;
   password.reserve( 256 );

   get_peerchain_info( identity, 0, &password );

   // NOTE: The following needs to be equivalent to the application protocol command:
   // .crypto_hash -x=$NUM_ROUNDS @encrypted_password -s=<tree_root_hash><height>
   // as is currently executed in the application protocol script "bc_gen_block.cin".
   decrypt_data( password, password );

   password += tree_root_hash + to_string( blockchain_height );

   sha256 hash( password );
   string digest( hash.get_digest_as_string( ) );

   for( size_t i = 0; i < c_num_hash_rounds; i++ )
   {
      hash.update( digest + password );
      hash.get_digest_as_string( digest );
   }

   clear_key( password );

   return ( digest.find( targeted_identity ) == 0 );
}

peerchain_type get_blockchain_type( const string& blockchain )
{
   peerchain_type chain_type = e_peerchain_type_any;

   string genesis_tag( blockchain );

   genesis_tag += ".0" + string( c_blk_suffix );

   if( has_tag( genesis_tag ) )
   {
      string genesis_hash( tag_file_hash( genesis_tag ) );

      // NOTE: As this can be called when constructing the peer session
      // the usual 'verify_core_file' method is not usable (as it would
      // require the use of session variables).
      string genesis_info( extract_file( genesis_hash, "" ) );

      string::size_type pos = genesis_info.find( '\n' );

      if( pos != string::npos )
         genesis_info.erase( pos );

      string prefix( "," );
      prefix += string( c_file_type_core_block_header_targeted_ident_prefix );

      bool has_target = false;
      bool has_hub_target = false;
      bool has_user_target = false;

      if( genesis_info.find( prefix ) != string::npos )
         has_target = true;

      string hub_prefix( prefix );

      hub_prefix += string( get_special_var_name( e_special_var_peer_hub ) );

      if( genesis_info.find( hub_prefix ) != string::npos )
         has_hub_target = true;
      else
      {
         string user_prefix( prefix );

         user_prefix += string( get_special_var_name( e_special_var_peer_user ) );

         if( genesis_info.find( user_prefix ) != string::npos )
            has_user_target = true;
      }

      if( has_hub_target )
         chain_type = e_peerchain_type_hub;
      else if( has_user_target )
         chain_type = e_peerchain_type_user;
      else if( !has_target )
         chain_type = e_peerchain_type_backup;
      else
         chain_type = e_peerchain_type_shared;
   }

   return chain_type;
}

void check_blockchain_type( const string& blockchain, peerchain_type chain_type, string* p_error = 0 )
{
   string genesis_tag( blockchain );

   genesis_tag += ".0" + string( c_blk_suffix );

   if( has_tag( genesis_tag ) )
   {
      string genesis_hash( tag_file_hash( genesis_tag ) );

      // NOTE: As this can be called when constructing the peer session
      // the usual 'verify_core_file' method is not usable (as it would
      // require the use of session variables).
      string genesis_info( extract_file( genesis_hash, "" ) );

      string::size_type pos = genesis_info.find( '\n' );

      if( pos != string::npos )
         genesis_info.erase( pos );

      string prefix( "," );
      prefix += string( c_file_type_core_block_header_targeted_ident_prefix );

      bool has_target = false;
      bool has_hub_target = false;
      bool has_user_target = false;

      if( genesis_info.find( prefix ) != string::npos )
         has_target = true;

      string hub_prefix( prefix );

      hub_prefix += string( get_special_var_name( e_special_var_peer_hub ) );

      if( genesis_info.find( hub_prefix ) != string::npos )
         has_hub_target = true;
      else
      {
         string user_prefix( prefix );

         user_prefix += string( get_special_var_name( e_special_var_peer_user ) );

         if( genesis_info.find( user_prefix ) != string::npos )
            has_user_target = true;
      }

      bool okay = true;

      if( chain_type == e_peerchain_type_any )
         ; // i.e. do nothing
      else if( chain_type == e_peerchain_type_hub )
      {
         if( !has_hub_target )
            okay = false;
      }
      else if( chain_type == e_peerchain_type_user )
      {
         if( !has_user_target )
            okay = true;
      }
      else if( chain_type == e_peerchain_type_backup )
      {
         if( has_target )
            okay = false;
      }
      else if( chain_type == e_peerchain_type_shared )
      {
         if( !has_target || has_hub_target )
            okay = false;
      }
      else
         okay = false;

      string error;

      if( !okay )
         // FUTURE: This message should be handled as a server string message.
         error = "Incorrect or unsupported Peer Type.";

      if( !error.empty( ) )
      {
         if( p_error )
            *p_error = error;
         else
            throw runtime_error( error );
      }
   }
}

void check_shared_for_support_session( const string& blockchain )
{
   vector< string > identities;

   if( num_have_session_variable(
    get_special_var_name( e_special_var_peer ), blockchain, &identities, true ) )
   {
      size_t index = 0;
      size_t lowest = 0;

      string own_sess_id( to_string( session_id( ) ) );

      string blockchain_targeted_identity_name(
       get_special_var_name( e_special_var_blockchain_targeted_identity ) );

      // NOTE: As the main session is started first (and session ids start
      // from one and always increment) find the lowest (non-zero) session
      // id's index in order to check if the main session is "shared".
      for( size_t i = 0; i < identities.size( ); i++ )
      {
         string next_id( identities[ i ] );

         if( next_id != own_sess_id )
         {
            size_t next_value = from_string< size_t >( next_id );

            if( !lowest || ( next_value < lowest ) )
            {
               index = i;
               lowest = next_value;
            }
         }
      }

      if( lowest && !get_session_variable( blockchain_targeted_identity_name, &identities[ index ] ).empty( ) )
         set_session_variable( blockchain_targeted_identity_name, c_true_value );

      set_session_variable( get_special_var_name( e_special_var_blockchain_checked_shared ), c_true_value );
   }
}

void set_hub_system_variable_if_required( const string& identity, const string& hub_identity )
{
   if( get_system_variable( "@" + identity ).empty( ) )
   {
      string hub_genesis_tag( c_bc_prefix + hub_identity + string( ".0" ) + c_blk_suffix );
      string hub_genesis_hash;

      if( has_tag( hub_genesis_tag ) )
         hub_genesis_hash = tag_file_hash( hub_genesis_tag );

      if( !hub_genesis_hash.empty( ) )
         set_system_variable( ">@" + identity, hub_genesis_hash );
      else
         set_system_variable( get_special_var_name( e_special_var_queue_hub_users ), identity );
   }
}

void process_queued_hub_using_peerchains( const string& hub_identity )
{
   string identities;

   while( true )
   {
      string next_identity( get_system_variable( get_special_var_name( e_special_var_queue_hub_users ) ) );

      if( next_identity.empty( ) )
         break;

      if( !identities.empty( ) )
         identities += ',';

      identities += next_identity;
   }

   if( !identities.empty( ) )
   {
      vector< string > all_identities;

      split( identities, all_identities );

      for( size_t i = 0; i < all_identities.size( ); i++ )
      {
         string next_identity( all_identities[ i ] );

         set_hub_system_variable_if_required( next_identity, hub_identity );
      }
   }
}

void process_core_file( const string& hash, const string& blockchain )
{
   guard g( g_mutex, "process_core_file" );

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

            verify_core_file( block_content, false );

            string block_height( get_raw_session_variable(
             get_special_var_name( e_special_var_blockchain_height ) ) );

            string primary_pubkey_hash( get_raw_session_variable(
             get_special_var_name( e_special_var_blockchain_primary_pubkey_hash ) ) );

            string secondary_pubkey_hash( get_raw_session_variable(
             get_special_var_name( e_special_var_blockchain_secondary_pubkey_hash ) ) );

            string tertiary_pubkey_hash( get_raw_session_variable(
             get_special_var_name( e_special_var_blockchain_tertiary_pubkey_hash ) ) );

            string targeted_identity( get_raw_session_variable(
             get_special_var_name( e_special_var_blockchain_targeted_identity ) ) );

            if( !primary_pubkey_hash.empty( ) )
            {
               bool is_user = false;
               bool is_shared = false;

               if( targeted_identity == get_special_var_name( e_special_var_peer_user ) )
                  is_user = true;
               else if( !targeted_identity.empty( ) && targeted_identity[ 0 ] != '@' )
                  is_shared = true;

               if( !is_user && !is_shared && ( block_height == "0" ) )
               {
                  string identity( determine_identity(
                   primary_pubkey_hash + secondary_pubkey_hash + tertiary_pubkey_hash ) );

                  if( identity.find( blockchain.substr( strlen( c_bc_prefix ) ) ) != 0 )
                     throw runtime_error( "invalid identity value '" + identity + "' for blockchain '" + blockchain + "'" );
               }

               if( !has_file( primary_pubkey_hash ) )
                  add_peer_file_hash_for_get( primary_pubkey_hash );

               string file_tag( blockchain + '.' + block_height + c_blk_suffix );

               tag_file( file_tag, hash );
            }

            set_session_variable(
             get_special_var_name( e_special_var_blockchain_height ), "" );

            if( !secondary_pubkey_hash.empty( ) && !has_file( secondary_pubkey_hash ) )
               add_peer_file_hash_for_get( secondary_pubkey_hash );

            if( !tertiary_pubkey_hash.empty( ) && !has_file( tertiary_pubkey_hash ) )
               add_peer_file_hash_for_get( tertiary_pubkey_hash );
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
   guard g( g_mutex, "process_repository_file" );

   TRACE_LOG( TRACE_PEER_OPS, "(process_repository_file) hash_info: " + hash_info );

   string::size_type pos = hash_info.find( ':' );

   if( pos == string::npos )
      throw runtime_error( "unexpected missing public key for process_repository_file with: " + hash_info );

   string src_hash( hash_info.substr( 0, pos ) );
   string extra_info( hash_info.substr( pos + 1 ) );

   unsigned char type_and_extra = '\0';

   string identity( replaced( blockchain, c_bc_prefix, "" ) );

   string non_extra_identity( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_non_extra_identity ) ) );

   if( !non_extra_identity.empty( ) )
      identity = non_extra_identity;

   bool is_blockchain_owner = !get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_is_owner ) ).empty( );

   bool has_archive = false;
   bool was_extracted = false;

   if( !get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_archive_path ) ).empty( ) )
      has_archive = true;

   string peer_map_key( get_peer_map_key( !is_test_session ) );

   string file_data, file_content, file_data_hash, peer_mapped_hash;

   if( p_file_data && !p_file_data->empty( ) )
   {
      file_data = *p_file_data;
      type_and_extra = file_data[ 0 ];

      peer_mapped_hash = get_peer_mapped_hash_info( peer_map_key, src_hash );

      if( !peer_mapped_hash.empty( ) )
      {
         file_data_hash = sha256( file_data ).get_digest_as_string( );

         if( is_blockchain_owner && ( peer_mapped_hash != file_data_hash ) )
            throw runtime_error( "found invalid encrypted file content" );

         clear_peer_mapped_hash( peer_map_key, src_hash );
      }

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

      if( !are_hex_nibbles( target_hash ) )
         target_hash = hex_encode( base64::decode( target_hash ) );

      pos = hex_pub_key.find( '-' );

      if( pos != string::npos )
      {
         hex_master = hex_pub_key.substr( pos + 1 );
         hex_pub_key.erase( pos );

         if( !are_hex_nibbles( hex_master ) )
            hex_master = hex_encode( base64::decode( hex_master ) );
      }

      if( !are_hex_nibbles( hex_pub_key ) )
         hex_pub_key = hex_encode( base64::decode( hex_pub_key ) );

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
         if( !is_blockchain_owner )
         {
            if( !has_repository_entry_record( identity, target_hash ) )
               store_repository_entry_record( identity, target_hash, src_hash, hex_pub_key, hex_master );

            if( file_data.empty( ) )
               file_data = ( char )type_and_extra + file_content;

            if( has_archive )
               create_raw_file_in_archive( identity, src_hash, file_data );
            else
               create_raw_file( file_data, true, 0, 0, src_hash.c_str( ), true, true );
         }
         else
         {
            string password;
            get_identity( password, false, true );

            peer_mapped_hash = get_peer_mapped_hash_info( peer_map_key, target_hash );

            decrypt_pulled_peer_file( target_hash, src_hash, password, hex_pub_key,
             false, p_file_data, &peer_mapped_hash, ( has_archive ? &identity : 0 ) );

            clear_key( password );

            if( !has_repository_entry_record( identity, target_hash ) )
               store_repository_entry_record( identity, target_hash, "", hex_master, hex_master );

            if( was_extracted )
               delete_file( src_hash );
         }
      }
   }
   else
   {
      string local_hash;

      bool has_local_file = false;
      bool has_repo_entry = fetch_repository_entry_record( identity, src_hash, local_hash, false );

      if( has_repo_entry && has_file( local_hash ) )
         has_local_file = true;

      if( has_repo_entry && has_local_file )
      {
         if( was_extracted )
            delete_file( src_hash );
      }
      else
      {
         public_key pub_key( extra_info, !are_hex_nibbles( extra_info ) );

         auto_ptr< private_key > ap_priv_key;

         if( !is_test_session )
         {
            string password;
            get_identity( password, false, true );

            // NOTE: The first nibble is zeroed out to ensure that the hash value is always valid to use
            // as a Bitcoin address "secret" (as the range of its EC is smaller than the full 256 bits).
            ap_priv_key.reset( new private_key( "0"
             + sha256( combined_clear_key( src_hash, password ) ).get_digest_as_string( ).substr( 1 ) ) );

            clear_key( password );
         }
         else
         {
            // NOTE: If using interactive testing the file may not be encrypted so to ensure
            // the file system doesn't get confused flag the file as being encrypted anyway.
            type_and_extra |= c_file_type_char_encrypted;

            ap_priv_key.reset( new private_key( sha256( c_dummy ).get_digest_as_string( ) ) );
         }

         string stream_cipher( get_raw_session_variable(
          get_special_var_name( e_special_var_blockchain_stream_cipher ) ) );

         if( stream_cipher.empty( ) )
         {
            if( is_test_session )
               stream_cipher = stream_cipher_string( );
            else
               throw runtime_error( "unexpected missing @stream_cipher in process_repository_file" );
         }

         stringstream ss( file_content );

         crypt_stream( ss, ap_priv_key->construct_shared( pub_key ), stream_cipher_value( stream_cipher ) );

         file_data = ( char )type_and_extra + ss.str( );

         if( was_extracted )
            delete_file( src_hash );

         if( !has_archive )
            local_hash = create_raw_file( file_data, false, 0, 0, 0, false );
         else
            create_raw_file_in_archive( identity, "", file_data, &local_hash );

         store_repository_entry_record( identity, src_hash,
          local_hash, ap_priv_key->get_public( ), pub_key.get_public( ) );

         add_peer_mapped_hash_info( c_local_prefix + peer_map_key, src_hash,
          local_hash + ':' + ap_priv_key->get_public( ) + pub_key.get_public( ) );
      }
   }
}

string get_peer_mapped_info_from_put_data(
 const string& encoded_master_pubkey, const string& encoded_local_pubkey, const string& encoded_source_hash )
{
   string retval;

   try
   {
      if( !encoded_master_pubkey.empty( ) )
         base64::validate( encoded_master_pubkey );

      base64::validate( encoded_local_pubkey );
      base64::validate( encoded_source_hash );

      // NOTE: Construct a public key object for the purpose of validation.
      public_key pubkey( encoded_local_pubkey, true );

      string extra( hex_encode( base64::decode( encoded_local_pubkey ) ) );

      if( !encoded_master_pubkey.empty( ) )
      {
         public_key pubkey( encoded_master_pubkey, true );

         extra += hex_encode( base64::decode( encoded_master_pubkey ) );
      }

      retval = hex_encode( base64::decode( encoded_source_hash ) ) + ':' + extra;
   }
   catch( ... )
   {
      // FUTURE: Perhaps add a trace here so specific errors aren't being completely ignored.
   }

   return retval;
}
#endif

void check_for_missing_other_sessions( const date_time& now )
{
   string paired_identity( get_raw_session_variable(
    get_special_var_name( e_special_var_paired_identity ) ) );

   if( !paired_identity.empty( ) && get_system_variable( paired_identity ).empty( ) )
   {
      string time_value( get_raw_session_variable(
       get_special_var_name( e_special_var_blockchain_time_value ) ) );

      if( !time_value.empty( ) )
      {
         int64_t time_val = from_string< int64_t >( time_value );

         if( unix_time( now ) >= time_val )
         {
            if( num_have_session_variable( paired_identity, true ) < 2 )
            {
               condemn_this_session( );

               throw runtime_error( "peer session has been condemned due to missing paired session" );
            }

            string backup_identity( get_raw_session_variable(
             get_special_var_name( e_special_var_blockchain_backup_identity ) ) );

            if( !backup_identity.empty( ) && num_have_session_variable( backup_identity, true ) < 2 )
            {
               condemn_this_session( );

               throw runtime_error( "peer session has been condemned due to missing backup session" );
            }
         }
      }
   }

   if( !get_raw_session_variable( get_special_var_name( e_special_var_peer_is_dependent ) ).empty( )
    && !get_raw_session_variable( get_special_var_name( e_special_var_blockchain_is_hub ) ).empty( ) )
   {
      string blockchain( get_raw_session_variable( get_special_var_name( e_special_var_peer ) ) );

      string identity( replaced( blockchain, c_bc_prefix, "" ) );

      if( !num_have_session_variable(
       get_special_var_name( e_special_var_blockchain_peer_hub_identity ), identity, 0, true ) )
      {
         condemn_this_session( );

         throw runtime_error( "peer session has been condemned due to missing any hub using sessions" );
      }
   }
}

bool get_block_height_from_tags( const string& blockchain, const string& hash, size_t& block_height )
{
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

bool add_put_list_if_available( const string& hub_identity, const string& blockchain, size_t blockchain_height )
{
   string hub_blockchain( c_bc_prefix + hub_identity );
   string hub_zenith_tag( hub_blockchain + c_zenith_suffix );

   bool found = false;

   if( has_tag( hub_zenith_tag ) )
   {
      string hub_zenith_hash( tag_file_hash( hub_zenith_tag ) );

      size_t hub_height = 0;

      if( get_block_height_from_tags( hub_blockchain, hub_zenith_hash, hub_height ) )
      {
         string hub_opl_tag( hub_blockchain + '.' + to_string( hub_height ) + c_opl_suffix );

         if( has_tag( hub_opl_tag ) )
         {
            string hub_opl_hash( tag_file_hash( hub_opl_tag ) );

            string hub_opl_info( extract_file( hub_opl_hash, "" ) );

            vector< string > lines;
            split( hub_opl_info, lines, '\n' );

            for( size_t i = 0; i < lines.size( ); i++ )
            {
               string next_line( lines[ i ] );

               if( next_line.find( blockchain ) == 0 )
               {
                  next_line.erase( 0, blockchain.length( ) + 1 );

                  string::size_type pos = next_line.find( ' ' );

                  if( pos == string::npos )
                     throw runtime_error( "unexpected opl line " + lines[ i ] );

                  string encoded_hash( next_line.substr( pos + 1 ) );
                  next_line.erase( pos );

                  pos = next_line.find( c_put_suffix );

                  if( pos == string::npos )
                     throw runtime_error( "unexpected opl line " + lines[ i ] );

                  next_line.erase( pos );

                  size_t put_height = from_string< size_t >( next_line );

                  if( put_height >= blockchain_height )
                  {
                     found = true;

                     if( put_height != blockchain_height )
                        set_session_variable( get_special_var_name( e_special_var_blockchain_zenith_tree_hash ), "" );
                     else
                     {
                        string put_file_hash( hex_encode( base64::decode( encoded_hash ) ) );

                        add_peer_file_hash_for_get( put_file_hash );
                        set_session_variable( get_special_var_name( e_special_var_blockchain_put_list_hash ), put_file_hash );
                     }
                  }

                  break;
               }
            }
         }
      }
   }

   return found;
}

void process_put_file( const string& blockchain,
 const string& file_data, bool check_for_supporters, bool is_test_session,
 set< string >& target_hashes, date_time* p_dtm = 0, progress* p_progress = 0 )
{
   TRACE_LOG( TRACE_PEER_OPS, "(process_put_file) blockchain: " + blockchain );

   vector< string > blobs;
   split( file_data, blobs, c_blob_separator );

   string identity( replaced( blockchain, c_bc_prefix, "" ) );

   string non_extra_identity( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_non_extra_identity ) ) );

   if( !non_extra_identity.empty( ) )
      identity = non_extra_identity;

   size_t num_skipped = 0;

   deque< string > repo_files_to_get;
   deque< string > non_repo_files_to_get;

   string peer_map_key( get_peer_map_key( !is_test_session ) );

   string num_tree_items( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_num_tree_items ) ) );

   string blockchain_height_other( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_height_other ) ) );

   string blockchain_height_processing( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_height_processing ) ) );

   string progress_count_name( get_special_var_name( e_special_var_progress_count ) );
   string progress_total_name( get_special_var_name( e_special_var_progress_total ) );
   string progress_value_name( get_special_var_name( e_special_var_progress_value ) );

   // NOTE: Extract non-repo files from the session to ensure
   // that none of them will be attempted to be fetched prior
   // to a later put.
   while( true )
   {
      string next( top_next_peer_file_hash_to_get( ) );

      if( next.empty( ) )
         break;

      if( next.find( c_repository_suffix ) != string::npos )
         repo_files_to_get.push_back( next );
      else
         non_repo_files_to_get.push_back( next );

      pop_next_peer_file_hash_to_get( );
   }

   if( !repo_files_to_get.empty( ) )
   {
      for( size_t i = 0; i < repo_files_to_get.size( ); i++ )
         add_peer_file_hash_for_get( repo_files_to_get[ i ], check_for_supporters );
   }

   for( size_t i = 0; i < blobs.size( ); i++ )
   {
      string next_blob( blobs[ i ] );

      vector< string > lines;
      split( next_blob, lines, '\n' );

      bool okay = false;

      if( is_condemned_session( ) )
         throw runtime_error( "peer session has been condemned" );

      if( g_server_shutdown )
         throw runtime_error( "application server is being shutdown" );

      if( p_dtm && p_progress )
      {
         date_time now( date_time::local( ) );

         uint64_t elapsed = seconds_between( *p_dtm, now );

         if( elapsed >= c_default_progress_seconds )
         {
            string progress( "." );

            if( num_tree_items.empty( ) || blockchain_height_processing.empty( ) )
               // FUTURE: This message should be handled as a server string message.
               progress = "Processed " + to_string( i ) + " items" + to_string( c_ellipsis );
            else
            {
               string count( to_string( get_blockchain_tree_item( blockchain ) ) );

               size_t next_height = from_string< size_t >( blockchain_height_processing );

               if( !num_tree_items.empty( ) )
               {
                  size_t upper_limit = from_string< size_t >( num_tree_items );

                  // NOTE: Ensure that the upper limit is not exceeded.
                  if( next_height > upper_limit )
                  {
                     next_height = upper_limit;
                     add_to_blockchain_tree_item( blockchain, 0, upper_limit );
                  }
               }

               // FUTURE: This message should be handled as a server string message.
               string progress_message( "Syncing at height " + to_string( next_height ) );

               if( !blockchain_height_other.empty( ) )
               {
                  size_t other_height = from_string< size_t >( blockchain_height_other );

                  if( next_height < other_height )
                     progress_message += '/' + blockchain_height_other;
               }

               progress_message += c_percentage_separator;

               if( num_tree_items.empty( ) )
                  progress_message += count;
               else
               {
                  set_session_variable( progress_count_name, count );
                  set_session_variable( progress_total_name, num_tree_items );

                  progress_message += get_raw_session_variable( progress_value_name );
               }

               progress_message += to_string( c_ellipsis );

               set_session_progress_message( progress_message );
            }

            *p_dtm = now;

            check_for_missing_other_sessions( now );

            p_progress->output_progress( progress );
         }
      }

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
                     else if( !is_test_session )
                        throw runtime_error( "unexpected missing target hash in put file info" );

#ifndef SSL_SUPPORT
                     okay = true;
#else
                     string peer_mapped_info( get_peer_mapped_info_from_put_data( master_key, public_key, source_hash ) );

                     string local_hash, local_public_key, master_public_key;

                     parse_peer_mapped_info( peer_mapped_info, local_hash, local_public_key, master_public_key, true, is_test_session );

                     if( !peer_mapped_info.empty( ) )
                     {
                        guard g( g_mutex, "process_put_file" );

                        okay = true;
                        pos = peer_mapped_info.find( ':' );

                        string hex_target_hash( hex_encode( base64::decode( target_hash ) ) );

                        if( !has_file( peer_mapped_info.substr( 0, pos ) ) )
                        {
                           if( has_file( hex_target_hash ) )
                           {
                              target_hashes.insert( hex_target_hash );

                              if( !has_repository_entry_record( identity, hex_target_hash ) )
                                 store_repository_entry_record( identity, hex_target_hash, "", master_key, master_key );
                           }
                           else
                           {
                              bool has_repo_entry = fetch_repository_entry_record( identity, hex_target_hash, local_hash, false );

                              if( has_repo_entry && has_file( local_hash ) )
                                 touch_file( local_hash, identity, false );
                              else
                              {
                                 if( !target_hashes.count( hex_target_hash ) )
                                 {
                                    target_hashes.insert( hex_target_hash );

                                    string mapped_hash( get_peer_mapped_hash_info( peer_map_key, hex_target_hash ) );

                                    // NOTE: Either now adds the file to be fetched or maps information
                                    // depending on whether or not "process_list_items" had been called
                                    // (if was called the mapped hash will already exist).
                                    if( is_test_session )
                                       add_peer_file_hash_for_get( local_hash + ':'
                                        + local_public_key + c_repository_suffix, false );
                                    else if( !mapped_hash.empty( ) )
                                       add_peer_file_hash_for_get( local_hash + ':' + local_public_key + '-'
                                        + master_public_key + ';' + target_hash + c_repository_suffix, check_for_supporters );
                                    else
                                       add_peer_mapped_hash_info( c_external_prefix + peer_map_key, hex_target_hash, peer_mapped_info );
                                 }
                              }
                           }
                        }
                        else
                           process_repository_file( blockchain, local_hash + ':'
                            + local_public_key + '-' + master_public_key + ';' + hex_target_hash, is_test_session );
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

   // NOTE: Now append previous non-repo files to get (that were not target hashes).
   if( !non_repo_files_to_get.empty( ) )
   {
      for( size_t i = 0; i < non_repo_files_to_get.size( ); i++ )
      {
         string next( non_repo_files_to_get[ i ] );

         if( !target_hashes.count( next ) )
            add_peer_file_hash_for_get( next );
      }
   }
}

bool has_all_list_items(
 const string& blockchain, const string& hash, bool recurse,
 bool touch_all_lists = false, date_time* p_dtm = 0, progress* p_progress = 0,
 size_t* p_total_processed = 0, string* p_blob_data = 0, map< string, size_t >* p_first_prefixed = 0 )
{
   TRACE_LOG( TRACE_PEER_OPS, "(has_all_list_items) hash: " + hash );

   bool retval = true;

   string all_list_items( extract_file( hash, "" ) );

   vector< string > list_items;
   vector< string > secondary_values;

   bool prefixed_secondary_values = false;

   split_list_items( all_list_items, list_items, &secondary_values, &prefixed_secondary_values );

   if( ( prefixed_secondary_values || !secondary_values.empty( ) ) && ( list_items.size( ) != secondary_values.size( ) ) )
      throw runtime_error( "unexpected list_items.size( ) != secondary_values.size( )" );

   string identity( replaced( blockchain, c_bc_prefix, "" ) );

   string extra_identity( identity );

   string peer_map_key( get_peer_map_key( ) );

   string non_extra_identity( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_non_extra_identity ) ) );

   if( !non_extra_identity.empty( ) )
      identity = non_extra_identity;

   bool has_targeted_identity = !get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_targeted_identity ) ).empty( );

   bool has_archive = false;

   if( !get_raw_session_variable( get_special_var_name(
    e_special_var_blockchain_archive_path ) ).empty( ) )
      has_archive = true;

   size_t total_processed = 0;

   if( !p_total_processed )
      p_total_processed = &total_processed;

   string last_repo_entry_hash;

   string mapped_info_key( c_local_prefix + peer_map_key );

   string queue_puts_name( get_special_var_name( e_special_var_queue_puts ) );

   string num_tree_items( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_num_tree_items ) ) );

   string blockchain_height_other( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_height_other ) ) );

   string blockchain_height_processing( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_height_processing ) ) );

   string progress_count_name( get_special_var_name( e_special_var_progress_count ) );
   string progress_total_name( get_special_var_name( e_special_var_progress_total ) );
   string progress_value_name( get_special_var_name( e_special_var_progress_value ) );

   bool is_fetching = !get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_is_fetching ) ).empty( );

   size_t max_blob_file_data = get_files_area_item_max_size( ) - c_max_put_blob_size;

   if( has_targeted_identity || secondary_values.empty( ) || prefixed_secondary_values )
   {
      for( size_t i = 0; i < list_items.size( ); i++ )
      {
         string next_item( list_items[ i ] );

         if( p_dtm && p_progress )
         {
            date_time now( date_time::local( ) );

            uint64_t elapsed = seconds_between( *p_dtm, now );

            if( elapsed >= c_default_progress_seconds )
            {
               string progress;

               if( touch_all_lists )
               {
                  size_t next_height = from_string< size_t >( blockchain_height_processing );

                  // FUTURE: This message should be handled as a server string message.
                  progress = "Checking at height " + to_string( next_height );

                  if( !blockchain_height_other.empty( ) )
                  {
                     size_t other_height = from_string< size_t >( blockchain_height_other );

                     if( next_height < other_height )
                        progress += '/' + blockchain_height_other;
                  }

                  progress += c_percentage_separator;

                  string count( to_string( *p_total_processed ) );

                  set_session_variable( progress_count_name, count );
                  set_session_variable( progress_total_name, num_tree_items );

                  progress += get_raw_session_variable( progress_value_name );

                  progress += c_ellipsis;

                  set_session_progress_message( progress );

                  system_identity_progress_message( extra_identity );

                  progress = ".";
               }
               else
               {
                  size_t next_height = from_string< size_t >( blockchain_height_processing );

                  // FUTURE: These messages should be handled as a server string messages.
                  if( is_fetching )
                     progress = "Polling at";
                  else
                     progress = "Mapping for";

                  progress += " height " + to_string( next_height );

                  if( !blockchain_height_other.empty( ) )
                  {
                     size_t other_height = from_string< size_t >( blockchain_height_other );

                     if( next_height < other_height )
                        progress += '/' + blockchain_height_other;
                  }

                  progress += c_percentage_separator;

                  string count( to_string( *p_total_processed ) );

                  set_session_variable( progress_count_name, count );
                  set_session_variable( progress_total_name, num_tree_items );

                  progress += get_raw_session_variable( progress_value_name );

                  progress += c_ellipsis;

                  set_session_progress_message( progress );

                  system_identity_progress_message( extra_identity );

                  if( is_fetching )
                     progress = ".";

                  check_for_missing_other_sessions( now );
               }

               *p_dtm = now;

               p_progress->output_progress( progress );
            }
         }

         ++( *p_total_processed );

         if( !next_item.empty( ) )
         {
            string next_hash( next_item.substr( 0, next_item.find( ' ' ) ) );

            bool has_next_file = false;
            bool has_next_repo_entry = ( next_hash == last_repo_entry_hash );

            string peer_mapped_info;

            // NOTE: Can assume the repository entry exists if the mapped information exists.
            if( !has_next_repo_entry )
            {
               peer_mapped_info = get_peer_mapped_hash_info( mapped_info_key, next_hash );

               if( !peer_mapped_info.empty( ) )
                  has_next_repo_entry = true;
            }

            if( !has_next_repo_entry )
               has_next_file = has_file( next_hash );

            if( p_first_prefixed && ( next_hash != last_repo_entry_hash ) )
            {
               string local_hash( next_hash );

               // NOTE: Omit list files from prefix mapping.
               if( !has_next_file || !is_list_file( next_hash ) )
               {
                  if( !has_next_file && fetch_repository_entry_record( identity, next_hash, local_hash, false ) )
                  {
                     has_next_repo_entry = true;
                     last_repo_entry_hash = next_hash;
                  }

                  string prefix( c_prefix_length, '\0' );
                  hex_decode( local_hash.substr( 0, c_prefix_length * 2 ), ( unsigned char* )prefix.data( ), c_prefix_length );

                  if( !p_first_prefixed->count( prefix ) )
                     p_first_prefixed->insert( make_pair( prefix, *p_total_processed ) );
               }
            }

            if( !has_next_file && !has_next_repo_entry )
            {
               has_next_repo_entry = has_repository_entry_record( identity, next_hash );

               if( has_next_repo_entry )
                  last_repo_entry_hash = next_hash;
            }

            if( p_blob_data && !peer_mapped_info.empty( ) )
            {
               if( p_blob_data->size( ) > 1 )
                  *p_blob_data += c_blob_separator;

               string local_hash, local_public_key, master_public_key;

               parse_peer_mapped_info( peer_mapped_info,
                local_hash, local_public_key, master_public_key, false );

               *p_blob_data += create_peer_repository_entry_pull_info( identity,
                next_hash, local_hash, local_public_key, master_public_key, false );

               clear_peer_mapped_hash( mapped_info_key, next_hash );

               if( p_blob_data->size( ) >= max_blob_file_data )
               {
                  string file_hash;

                  if( !has_archive )
                     file_hash = create_raw_file( *p_blob_data );
                  else
                     create_raw_file_in_archive( identity, "", *p_blob_data, &file_hash );

                  set_session_variable( queue_puts_name, file_hash );

                  *p_blob_data = string( c_file_type_str_blob );
               }
            }

            if( !has_next_file && !has_next_repo_entry )
            {
               retval = false;
               break;
            }
            else if( recurse && !has_next_repo_entry && is_list_file( next_hash ) )
            {
               retval = has_all_list_items( blockchain, next_hash, recurse,
                touch_all_lists, p_dtm, p_progress, p_total_processed, p_blob_data, p_first_prefixed );

               if( !retval )
                  break;
            }
         }
      }

      if( p_blob_data && p_blob_data->size( ) > 1 )
      {
         string file_hash;

         if( !has_archive )
            file_hash = create_raw_file( *p_blob_data );
         else
            create_raw_file_in_archive( identity, "", *p_blob_data, &file_hash );

         set_session_variable( queue_puts_name, file_hash );

         *p_blob_data = string( c_file_type_str_blob );
      }
   }

   if( touch_all_lists )
      touch_file( hash, identity, false );

   return retval;
}

bool last_data_tree_is_identical( const string& blockchain,
 size_t previous_height, string* p_prior_data_tree_hash = 0 )
{
   bool retval = false;

   if( previous_height )
   {
      restorable_session_variables all_session_variables;

      string tree_root_hash( get_raw_session_variable(
       get_special_var_name( e_special_var_blockchain_tree_root_hash ) ) );

      set_session_variable(
       get_special_var_name( e_special_var_blockchain_tree_root_hash ), "" );

      string block_tag( blockchain + '.' );

      block_tag += to_string( previous_height );

      block_tag += c_blk_suffix;

      string prior_block_hash( tag_file_hash( block_tag ) );

      string prior_block_content(
       construct_blob_for_block_content( extract_file( prior_block_hash, "" ) ) );

      verify_core_file( prior_block_content, false );

      string prior_data_tree_hash( get_raw_session_variable(
       get_special_var_name( e_special_var_blockchain_tree_root_hash ) ) );

      if( tree_root_hash == prior_data_tree_hash )
         retval = true;
      else
      {
         if( p_prior_data_tree_hash )
            *p_prior_data_tree_hash = prior_data_tree_hash;
      }
   }

   return retval;
}

void process_list_items( const string& blockchain,
 const string& hash, bool recurse, size_t* p_num_items_found = 0,
 set< string >* p_list_items_to_ignore = 0, date_time* p_dtm = 0, progress* p_progress = 0 )
{
   TRACE_LOG( TRACE_PEER_OPS, "(process_list_items) hash: " + hash );

   string all_list_items( extract_file( hash, "" ) );

   string identity( replaced( blockchain, c_bc_prefix, "" ) );

   string extra_identity( identity );

   string peer_map_key( get_peer_map_key( ) );

   string non_extra_identity( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_non_extra_identity ) ) );

   if( !non_extra_identity.empty( ) )
      identity = non_extra_identity;

   vector< string > list_items;
   vector< string > secondary_values;

   bool has_targeted_identity = !get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_targeted_identity ) ).empty( );

   bool skip_secondary_blobs = false;
   bool prefixed_secondary_values = false;

   split_list_items( all_list_items, list_items, &secondary_values, &prefixed_secondary_values );

   if( !has_targeted_identity && !secondary_values.empty( ) && !prefixed_secondary_values )
      skip_secondary_blobs = true;

   if( !secondary_values.empty( ) && ( list_items.size( ) != secondary_values.size( ) ) )
      throw runtime_error( "unexpected list_items.size( ) != secondary_values.size( )" );

   bool has_archive = false;

   if( !get_raw_session_variable( get_special_var_name(
    e_special_var_blockchain_archive_path ) ).empty( ) )
      has_archive = true;

   string progress_count_name( get_special_var_name( e_special_var_progress_count ) );
   string progress_total_name( get_special_var_name( e_special_var_progress_total ) );
   string progress_value_name( get_special_var_name( e_special_var_progress_value ) );
   string blockchain_is_owner_name( get_special_var_name( e_special_var_blockchain_is_owner ) );
   string blockchain_is_checking_name( get_special_var_name( e_special_var_blockchain_is_checking ) );
   string blockchain_is_fetching_name( get_special_var_name( e_special_var_blockchain_is_fetching ) );
   string blockchain_first_mapped_name( get_special_var_name( e_special_var_blockchain_first_mapped ) );
   string blockchain_other_is_owner_name( get_special_var_name( e_special_var_blockchain_other_is_owner ) );
   string blockchain_both_are_owners_name( get_special_var_name( e_special_var_blockchain_both_are_owners ) );

   string first_hash_name( get_special_var_name( e_special_var_hash ) );
   string first_hash_to_get( get_raw_session_variable( first_hash_name ) );

   if( !skip_secondary_blobs && ( hash == first_hash_to_get ) )
   {
      first_hash_to_get.erase( );
      set_session_variable( first_hash_name, "" );
   }

   bool is_owner = !get_raw_session_variable( blockchain_is_owner_name ).empty( );
   bool is_checking = !get_raw_session_variable( blockchain_is_checking_name ).empty( );
   bool is_fetching = !get_raw_session_variable( blockchain_is_fetching_name ).empty( );
   bool is_other_owner = !get_raw_session_variable( blockchain_other_is_owner_name ).empty( );

   bool is_peer_responder = !get_raw_session_variable( get_special_var_name( e_special_var_peer_responder ) ).empty( );

   bool check_for_supporters = false;

   if( !get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_peer_has_supporters ) ).empty( ) )
      check_for_supporters = true;

   bool has_set_first_mapped = false;

   string num_tree_items( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_num_tree_items ) ) );

   size_t upper_limit = 0;

   if( !num_tree_items.empty( ) )
   {
      size_t num_items = from_string< size_t >( num_tree_items );

      string blockchain_num_puts( get_raw_session_variable(
       get_special_var_name( e_special_var_blockchain_num_puts ) ) );

      upper_limit = ( num_items - from_string< size_t >( blockchain_num_puts ) );
   }

   string blockchain_height_other( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_height_other ) ) );

   string blockchain_height_processing( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_height_processing ) ) );

   // NOTE: If not found then if zenith height exists will add one to that.
   if( blockchain_height_processing.empty( ) )
   {
      blockchain_height_processing = get_raw_session_variable(
       get_special_var_name( e_special_var_blockchain_zenith_height ) );

      if( !blockchain_height_processing.empty( ) )
         blockchain_height_processing = to_string( from_string< size_t >( blockchain_height_processing ) + 1 );
   }

   for( size_t i = 0; i < list_items.size( ); i++ )
   {
      if( is_condemned_session( ) )
         throw runtime_error( "peer session has been condemned" );

      if( g_server_shutdown )
         throw runtime_error( "application server is being shutdown" );

      bool blob_increment = ( has_targeted_identity ? true : !skip_secondary_blobs );

      if( !has_targeted_identity
       && !secondary_values.empty( ) && ( !is_fetching || !is_peer_responder ) )
         blob_increment = ( i % 2 == 0 );

      if( p_dtm && p_progress )
      {
         date_time now( date_time::local( ) );

         uint64_t elapsed = seconds_between( *p_dtm, now );

         if( elapsed >= c_default_progress_seconds )
         {
            string progress;

            // FUTURE: These messages should be handled as server string messages.
            if( !p_num_items_found )
               progress = "Processing: " + hash;
            else
            {
               string count( to_string( *p_num_items_found ) );

               if( is_fetching )
               {
                  progress = "Syncing";

                  if( !blockchain_height_processing.empty( ) )
                  {
                     progress += " at height " + blockchain_height_processing;

                     if( !blockchain_height_other.empty( ) )
                     {
                        size_t next_height = from_string< size_t >( blockchain_height_processing );
                        size_t other_height = from_string< size_t >( blockchain_height_other );

                        if( next_height < other_height )
                           progress += '/' + blockchain_height_other;
                     }
                  }

                  progress += c_percentage_separator;

                  if( num_tree_items.empty( ) )
                     progress += count;
                  else
                  {
                     set_session_variable( progress_count_name, count );
                     set_session_variable( progress_total_name, num_tree_items );

                     progress += get_raw_session_variable( progress_value_name );
                  }
               }
               else
               {
                  progress = "Preparing";

                  if( !blockchain_height_processing.empty( ) )
                     progress += " at height " + blockchain_height_processing;

                  progress += c_percentage_separator;

                  if( num_tree_items.empty( ) )
                     progress += count;
                  else
                  {
                     set_session_variable( progress_count_name, count );
                     set_session_variable( progress_total_name, num_tree_items );

                     progress += get_raw_session_variable( progress_value_name );
                  }
               }
            }

            progress += c_ellipsis;

            *p_dtm = now;

            check_for_missing_other_sessions( now );

            if( is_fetching )
            {
               // NOTE: If "blockchain_height_processing" is empty then will output the
               // message created above so at least a progress message has been issued.
               if( blockchain_height_processing.empty( ) )
                  set_session_progress_message( progress );
               else
               {
                  if( !num_tree_items.empty( ) )
                  {
                     size_t upper_limit = from_string< size_t >( num_tree_items );

                     // NOTE: Ensure that the upper limit is not exceeded.
                     add_to_blockchain_tree_item( blockchain, 0, upper_limit );
                  }

                  string progress_message;

                  // FUTURE: This message should be handled as a server string message.
                  progress_message = "Syncing at height ";

                  progress_message += blockchain_height_processing;

                  if( !blockchain_height_other.empty( ) )
                  {
                     size_t next_height = from_string< size_t >( blockchain_height_processing );
                     size_t other_height = from_string< size_t >( blockchain_height_other );

                     if( next_height < other_height )
                        progress_message += '/' + blockchain_height_other;
                  }

                  progress_message += c_percentage_separator;

                  string count( to_string( get_blockchain_tree_item( blockchain ) ) );

                  if( num_tree_items.empty( ) )
                     progress_message += count;
                  else
                  {
                     set_session_variable( progress_count_name, count );
                     set_session_variable( progress_total_name, num_tree_items );

                     progress_message += get_raw_session_variable( progress_value_name );
                  }

                  progress_message += c_ellipsis;

                  set_session_progress_message( progress_message );
                  set_system_variable( c_progress_output_prefix + extra_identity, progress_message );
               }

               progress = ".";
            }

            p_progress->output_progress( progress );
         }
      }

      string next_item( list_items[ i ] );
      string next_secondary;

      if( !secondary_values.empty( ) )
         next_secondary = secondary_values[ i ];

      if( !next_item.empty( ) )
      {
         string next_hash( next_item.substr( 0, next_item.find( ' ' ) ) );

         if( !skip_secondary_blobs && ( next_hash == first_hash_to_get ) )
         {
            first_hash_to_get.erase( );
            set_session_variable( first_hash_name, "" );

            // NOTE: If the next hash requested was an item in the previous tree then
            // will assume that something is wrong at the requesting end and will not
            // ignore any further items.
            if( p_list_items_to_ignore && p_list_items_to_ignore->count( next_hash ) )
               p_list_items_to_ignore = 0;
         }

         if( p_list_items_to_ignore && p_list_items_to_ignore->count( next_hash ) )
         {
            if( blob_increment && p_num_items_found )
               ++( *p_num_items_found );

            if( is_fetching && blob_increment )
               add_to_blockchain_tree_item( blockchain, 1, upper_limit );

            continue;
         }

         bool add_to_tree_items = false;

         string local_hash, local_public_key, master_public_key;

         if( !has_file( next_hash ) )
         {
            bool check_blob_increment = false;

            if( !has_repository_entry_record( identity, next_hash ) )
            {
               if( is_fetching )
               {
                  bool added = false;

                  if( next_secondary.empty( ) )
                  {
                     added = true;
                     add_peer_file_hash_for_get( next_hash );
                  }
                  else
                  {
                     if( !prefixed_secondary_values )
                     {
                        string peer_mapped_info( get_peer_mapped_hash_info( c_external_prefix + peer_map_key, next_hash ) );

                        if( peer_mapped_info.empty( ) )
                           add_peer_mapped_hash_info( peer_map_key, next_hash, next_secondary );
                        else
                        {
                           added = true;

                           string local_hash, local_public_key, master_public_key;

                           parse_peer_mapped_info( peer_mapped_info,
                            local_hash, local_public_key, master_public_key, true );

                           clear_peer_mapped_hash( c_external_prefix + peer_map_key, next_hash );

                           add_peer_file_hash_for_get( local_hash + ':'
                            + local_public_key + '-' + master_public_key + ';'
                            + base64::encode( hex_decode( next_hash ) ) + c_repository_suffix, check_for_supporters );
                        }

                        if( has_targeted_identity )
                        {
                           added = true;
                           add_peer_file_hash_for_get( next_hash, check_for_supporters );
                        }
                        else if( !has_set_first_mapped )
                           has_set_first_mapped = set_session_variable( blockchain_first_mapped_name, next_hash, "" );
                     }
                     else if( is_other_owner )
                     {
                        added = true;

                        add_peer_file_hash_for_get( next_hash + ':'
                         + base64::encode( hex_decode( next_secondary ) ) + c_repository_suffix, check_for_supporters );
                     }
                  }

                  if( added && p_list_items_to_ignore )
                     p_list_items_to_ignore->insert( next_hash );
               }
            }
            else if( !recurse && is_fetching )
            {
               check_blob_increment = true;

               // NOTE: For repository entries need to touch the local file.
               fetch_repository_entry_record( identity, next_hash, local_hash );

               touch_file( local_hash, identity, false );
            }
            else
               check_blob_increment = true;

            if( check_blob_increment )
            {
               if( is_fetching && blob_increment )
                  add_to_tree_items = true;

               if( blob_increment && p_num_items_found )
                  ++( *p_num_items_found );
            }
         }
         else if( recurse && is_list_file( next_hash ) )
         {
            if( is_fetching )
               add_to_tree_items = true;

            if( p_num_items_found )
               ++( *p_num_items_found );

            process_list_items( blockchain, next_hash, recurse,
             p_num_items_found, p_list_items_to_ignore, p_dtm, p_progress );

            // NOTE: Recursive processing may have already located this.
            first_hash_to_get = get_raw_session_variable( first_hash_name );
         }
         else if( recurse )
         {
            if( is_fetching && blob_increment )
               add_to_tree_items = true;

            if( blob_increment && p_num_items_found )
               ++( *p_num_items_found );
         }
         else
         {
            // NOTE: Although recursion was not requested in order to ensure
            // that all blob items will be "touched" need to do this anyway.
            if( !is_list_file( next_hash ) )
            {
               if( is_fetching && blob_increment )
                  add_to_tree_items = true;

               if( blob_increment && p_num_items_found )
                  ++( *p_num_items_found );

               touch_file( next_hash, identity, false );
            }
            else
               process_list_items( blockchain, next_hash, false,
                p_num_items_found, p_list_items_to_ignore, p_dtm, p_progress );
         }

         if( add_to_tree_items )
            add_to_blockchain_tree_item( blockchain, 1, upper_limit );
      }
   }
}

void process_signature_file( const string& blockchain, const string& hash, size_t height )
{
   guard g( g_mutex, "process_signature_file" );

   TRACE_LOG( TRACE_PEER_OPS, "(process_signature_file) hash: " + hash + " height: " + to_string( height ) );

   if( !height )
      throw runtime_error( "invalid zero height for process_signature_file" );

   bool is_fetching = !get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_is_fetching ) ).empty( );

   try
   {
      string pubkey_tag( blockchain + '.' + to_string( height - 1 ) + c_pub_suffix );

      string pubkey_hash( tag_file_hash( pubkey_tag ) );

      string block_file_hash( verified_hash_from_signature( pubkey_hash, hash ) );

      if( !has_file( block_file_hash ) )
         add_peer_file_hash_for_get( block_file_hash );

      set_session_variable( get_special_var_name(
       e_special_var_blockchain_block_file_hash ), block_file_hash );

      string signature_tag( blockchain + '.' );

      signature_tag += to_string( height - 1 );

      signature_tag += c_sig_suffix;

      tag_file( signature_tag, hash );

      set_session_variable(
       get_special_var_name( e_special_var_blockchain_signature_file_hash ), "" );
   }
   catch( ... )
   {
      if( is_fetching )
         delete_file( hash );

      throw;
   }
}

void process_op_list_file( const string& blockchain, const string& hash, size_t height )
{
   guard g( g_mutex, "process_op_list_file" );

   string op_list_tag( blockchain );

   op_list_tag += ".";

   op_list_tag += to_string( height );

   op_list_tag += c_opl_suffix;

   tag_file( op_list_tag, hash );
}

void process_put_list_file( const string& blockchain,
 const string& hash, size_t height, const string& file_data, bool is_new_file = true )
{
   guard g( g_mutex, "process_put_list_file" );

   if( is_new_file )
   {
      string put_list_tag( blockchain );

      put_list_tag += ".";

      put_list_tag += to_string( height );

      put_list_tag += c_put_suffix;

      tag_file( put_list_tag, hash );
   }

   vector< string > lines;
   split( file_data, lines, '\n' );

   for( size_t i = 0; i < lines.size( ); i++ )
   {
      string next_line( lines[ i ] );

      add_peer_file_hash_for_get( hex_encode( base64::decode( next_line ) ) );

      set_session_variable( get_special_var_name(
       e_special_var_num_put_files ), get_special_var_name( e_special_var_increment ) );
   }

   string tree_root_hash( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_tree_root_hash ) ) );

   if( !tree_root_hash.empty( ) && !has_file( tree_root_hash ) )
      add_peer_file_hash_for_get( tree_root_hash );
}

void process_public_key_file( const string& blockchain,
 const string& hash, size_t height, public_key_scale key_scale, size_t height_other = 0 )
{
   guard g( g_mutex, "process_public_key_file" );

   string pubkey_tag( blockchain );

   pubkey_tag += ".";

   string demo_backup_blockchain( c_bc_prefix );
   demo_backup_blockchain += c_test_backup_identity;

   string demo_shared_blockchain( c_bc_prefix );
   demo_shared_blockchain += c_test_shared_identity;

   size_t scaling_value = c_bc_scaling_value;

   if( ( blockchain == demo_backup_blockchain )
    || ( blockchain == demo_shared_blockchain ) )
      scaling_value = c_bc_scaling_demo_value;

   size_t scaling_squared = ( scaling_value * scaling_value );

   if( key_scale == e_public_key_scale_primary )
      pubkey_tag += to_string( height );
   else if( key_scale == e_public_key_scale_secondary )
      pubkey_tag += to_string( height + scaling_value - 1 );
   else
      pubkey_tag += to_string( height + scaling_squared - 1 );

   pubkey_tag += c_pub_suffix;

   tag_file( pubkey_tag, hash );

   string zenith_height_name(
    get_special_var_name( e_special_var_blockchain_zenith_height ) );

   string zenith_height( get_raw_session_variable( zenith_height_name ) );

   bool has_all_pub_keys = false;

   if( key_scale == e_public_key_scale_tertiary )
      has_all_pub_keys = true;

   if( !height && has_all_pub_keys && zenith_height.empty( ) )
   {
      string block_hash( tag_file_hash(
       blockchain + '.' + to_string( height ) + c_blk_suffix ) );

      tag_file( blockchain + c_zenith_suffix, block_hash );

      string identity( replaced( blockchain, c_bc_prefix, "" ) );

      set_session_variable( zenith_height_name, to_string( height ) );

      output_sync_progress_message( identity, height, height_other, true );

      TRACE_LOG( TRACE_PEER_OPS, "::: new zenith hash: " + block_hash + " height: " + to_string( height ) );

      if( !get_raw_session_variable( get_special_var_name( e_special_var_blockchain_is_hub ) ).empty( ) )
         process_queued_hub_using_peerchains( identity );
   }

   if( key_scale == e_public_key_scale_primary )
      set_session_variable(
       get_special_var_name( e_special_var_blockchain_primary_pubkey_hash ), "" );
   else if( key_scale == e_public_key_scale_secondary )
      set_session_variable(
       get_special_var_name( e_special_var_blockchain_secondary_pubkey_hash ), "" );
   else
      set_session_variable(
       get_special_var_name( e_special_var_blockchain_tertiary_pubkey_hash ), "" );
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

   if( lines.size( ) != c_num_lamport_lines )
      throw runtime_error( "unexpected number of lines is " + to_string( lines.size( ) )
       + " rather than " + to_string( c_num_lamport_lines ) + " in  'validate_signature_file'" );

   for( size_t i = 0; i < c_num_lamport_lines; i++ )
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

void validate_op_list_file( const string& file_data )
{
   if( file_data.empty( ) )
      throw runtime_error( "unexpected empty file data in 'validate_op_list_file'" );

   string content( file_data.substr( 1 ) );

   if( content.empty( ) )
      throw runtime_error( "unexpected empty file content in 'validate_op_list_file'" );

   vector< string > lines;
   split( content, lines, '\n' );

   if( !lines.empty( ) && lines[ lines.size( ) - 1 ].empty( ) )
      lines.pop_back( );

   for( size_t i = 0; i < lines.size( ); i++ )
   {
      string next_line( lines[ i ] );

      string::size_type pos = next_line.find( ' ' );

      bool is_invalid = false;

      if( pos == string::npos )
         is_invalid = true;
      else
      {
         string tag_parts( next_line.substr( 0, pos ) );
         next_line.erase( 0, pos + 1 );

         vector< string > parts;
         split( tag_parts, parts, '.' );

         // NOTE: Tag should be in the form "bc.<ident>.<height>.put".
         if( parts.size( ) != 4 )
            is_invalid = true;
         else
         {
            if( ( parts[ 0 ] != "bc" ) || ( parts[ 3 ] != "put" ) )
               is_invalid = true;
            else
            {
               string ident( parts[ 1 ] );

               string re( "^" + string( c_regex_peerchain_identity ) + "$" );

               regex expr( re );

               if( expr.search( ident ) == string::npos )
                  is_invalid = true;
               else
               {
                  string height( parts[ 2 ] );

                  string re( "^" + string( c_regex_height ) + "$" );

                  regex expr( re );

                  if( expr.search( height ) == string::npos )
                     is_invalid = true;
               }
            }
         }

         if( !is_invalid )
         {
            if( next_line.length( ) != 44 )
               is_invalid = true;
            else
            {
               bool rc = false;
               base64::validate( next_line, &rc );

               is_invalid = !rc;
            }
         }
      }

      if( is_invalid )
         throw runtime_error( "unexpected invalid op list line: " + next_line );
   }
}

void validate_put_list_file( const string& file_data )
{
   if( file_data.empty( ) )
      throw runtime_error( "unexpected empty file data in 'validate_put_list_file'" );

   string content( file_data.substr( 1 ) );

   if( content.empty( ) )
      throw runtime_error( "unexpected empty file content in 'validate_put_list_file'" );

   vector< string > lines;
   split( content, lines, '\n' );

   bool is_invalid = false;

   for( size_t i = 0; i < lines.size( ); i++ )
   {
      string next_line( lines[ i ] );

      if( next_line.length( ) != 44 )
         is_invalid = true;
      else
      {
         bool rc = false;
         base64::validate( next_line, &rc );

         is_invalid = !rc;
      }

      if( is_invalid )
         throw runtime_error( "unexpected invalid put list line: " + next_line );
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

   if( lines.size( ) != c_num_lamport_lines )
      throw runtime_error( "unexpected number of lines is " + to_string( lines.size( ) )
       + " rather than " + to_string( c_num_lamport_lines ) + " in  'validate_public_key_file'" );

   for( size_t i = 0; i < c_num_lamport_lines; i++ )
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

void process_block_for_height( const string& blockchain, const string& hash, size_t height,
 set< string >& list_items_to_ignore, size_t* p_num_items_found = 0, progress* p_progress = 0 )
{
   bool has_hind_hash = false;

   bool is_user = !get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_user ) ).empty( );

   bool is_owner = !get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_is_owner ) ).empty( );

   bool is_fetching = !get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_is_fetching ) ).empty( );

   bool peer_has_tree_items = !get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_get_tree_files ) ).empty( );

   TRACE_LOG( TRACE_PEER_OPS, "(process_block_for_height) hash: " + hash + " height: " + to_string( height ) );

   string identity( replaced( blockchain, c_bc_prefix, "" ) );

   string non_extra_identity( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_non_extra_identity ) ) );

   if( non_extra_identity.empty( ) )
      non_extra_identity = identity;

   string hub_identity( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_peer_hub_identity ) ) );

   string block_content(
    construct_blob_for_block_content( extract_file( hash, "" ) ) );

   set_session_variable(
    get_special_var_name( e_special_var_blockchain_tree_root_hash ), "" );

   verify_core_file( block_content, false );

   string block_height( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_height ) ) );

   set_session_variable(
    get_special_var_name( e_special_var_blockchain_height ), "" );

   if( !get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_hind_hash ) ).empty( ) )
      has_hind_hash = true;

   string targeted_identity( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_targeted_identity ) ) );

   bool is_shared = false;

   if( !targeted_identity.empty( ) && ( targeted_identity[ 0 ] != '@' ) )
      is_shared = true;

   if( !is_user && !is_shared && is_fetching )
   {
      string stream_cipher_var_name( get_special_var_name( e_special_var_blockchain_stream_cipher ) );

      set_session_variable_for_matching_blockchains(
       stream_cipher_var_name, get_raw_session_variable( stream_cipher_var_name ),
       get_special_var_name( e_special_var_blockchain_peer_supporter ), c_true_value, true );
   }

   if( !block_height.empty( ) && ( block_height != to_string( height ) ) )
      throw runtime_error( "specified height does not match that found in the block itself (blk)" );
   else
   {
      string primary_pubkey_hash( get_raw_session_variable(
       get_special_var_name( e_special_var_blockchain_primary_pubkey_hash ) ) );

      string secondary_pubkey_hash( get_raw_session_variable(
       get_special_var_name( e_special_var_blockchain_secondary_pubkey_hash ) ) );

      string tertiary_pubkey_hash( get_raw_session_variable(
       get_special_var_name( e_special_var_blockchain_tertiary_pubkey_hash ) ) );

      bool has_tertiary = !tertiary_pubkey_hash.empty( );

      if( !primary_pubkey_hash.empty( ) )
      {
         if( !height && !is_shared )
         {
            string identity( determine_identity(
             primary_pubkey_hash + secondary_pubkey_hash + tertiary_pubkey_hash ) );

            if( targeted_identity != get_special_var_name( e_special_var_peer_user )
             && ( identity.find( blockchain.substr( strlen( c_bc_prefix ) ) ) != 0 ) )
               throw runtime_error( "invalid identity value '" + identity + "' for blockchain '" + blockchain + "'" );
         }

         if( is_fetching )
         {
            if( !has_file( primary_pubkey_hash ) )
               add_peer_file_hash_for_get( primary_pubkey_hash );
            else
               process_public_key_file( blockchain,
                primary_pubkey_hash, height, e_public_key_scale_primary );
         }
      }

      if( is_fetching && !secondary_pubkey_hash.empty( ) )
      {
         if( !has_file( secondary_pubkey_hash ) )
            add_peer_file_hash_for_get( secondary_pubkey_hash );
         else
            process_public_key_file( blockchain,
             secondary_pubkey_hash, height, e_public_key_scale_secondary );
      }

      if( is_fetching && !tertiary_pubkey_hash.empty( ) )
      {
         if( !has_file( tertiary_pubkey_hash ) )
            add_peer_file_hash_for_get( tertiary_pubkey_hash );
         else
            process_public_key_file( blockchain,
             tertiary_pubkey_hash, height, e_public_key_scale_tertiary );
      }

      string first_hash_to_get(
       get_raw_session_variable( get_special_var_name( e_special_var_hash ) ) );

      if( ( first_hash_to_get == primary_pubkey_hash )
       || ( first_hash_to_get == secondary_pubkey_hash )
       || ( first_hash_to_get == tertiary_pubkey_hash ) )
         set_session_variable( get_special_var_name( e_special_var_hash ), "" );

      string op_list_hash( get_raw_session_variable(
       get_special_var_name( e_special_var_blockchain_op_list_hash ) ) );

      string tree_root_hash( get_raw_session_variable(
       get_special_var_name( e_special_var_blockchain_tree_root_hash ) ) );

      string blockchain_zenith_height( get_raw_session_variable(
       get_special_var_name( e_special_var_blockchain_zenith_height ) ) );

      if( is_fetching )
         set_session_variable(
          get_special_var_name( e_special_var_blockchain_zenith_tree_hash ), tree_root_hash );

      if( !op_list_hash.empty( ) )
      {
         if( is_fetching && !has_file( op_list_hash ) )
            add_peer_file_hash_for_get( op_list_hash );
         else
            process_op_list_file( blockchain, op_list_hash, height );

         if( first_hash_to_get == op_list_hash )
            set_session_variable( get_special_var_name( e_special_var_hash ), "" );
      }
      else if( !tree_root_hash.empty( ) )
      {
         date_time dtm( date_time::local( ) );

         if( !has_file( tree_root_hash ) )
         {
            if( is_fetching )
            {
               bool fetch_tree_root = peer_has_tree_items;

               if( fetch_tree_root )
               {
                  if( !hub_identity.empty( ) )
                  {
                     fetch_tree_root = false;

                     if( !add_put_list_if_available( hub_identity, blockchain, height ) )
                        set_waiting_for_hub_progress( identity, hub_identity );
                  }
               }

               if( fetch_tree_root )
                  add_peer_file_hash_for_get( tree_root_hash );
            }
         }
         else
         {
            bool has_all_tree_items = false;

            if( !is_fetching )
            {
               size_t total_items = 0;
               map< string, size_t > first_prefixed;

               temporary_session_variable temp_progress(
                get_special_var_name( e_special_var_progress_message ) );

               temporary_session_variable temp_height( get_special_var_name(
                e_special_var_blockchain_height_processing ), to_string( height ) );

               has_all_tree_items = has_all_list_items( blockchain,
                tree_root_hash, true, false, &dtm, p_progress, &total_items, 0, &first_prefixed );

               if( !has_all_tree_items )
               {
                  first_prefixed.clear( );

                  set_session_variable( get_special_var_name( e_special_var_tree_count ), "" );
                  set_session_variable( get_special_var_name( e_special_var_tree_total ), "" );
               }
               else
               {
                  set_session_variable( get_special_var_name( e_special_var_tree_count ), "0" );
                  set_session_variable( get_special_var_name( e_special_var_tree_total ), to_string( total_items ) );
               }

               save_first_prefixed( first_prefixed, total_items );
            }

            if( is_fetching && !has_all_tree_items && !hub_identity.empty( ) )
            {
               if( !add_put_list_if_available( hub_identity, blockchain, height ) )
                  set_waiting_for_hub_progress( identity, hub_identity );
               else
               {
                  string put_list_hash( top_next_peer_file_hash_to_get( ) );

                  // NOTE: If the top file is a previously fetched "put list" then process it now.
                  if( has_file( put_list_hash ) )
                  {
                     pop_next_peer_file_hash_to_get( );

                     process_put_list_file( blockchain,
                      put_list_hash, height, extract_file( put_list_hash, "" ), false );

                     set_session_variable( get_special_var_name( e_special_var_blockchain_put_list_hash ), "" );
                  }
               }
            }

            if( is_fetching || has_all_tree_items )
            {
               // NOTE: If is owner and not fetching (and has all tree items) then no need to
               // call "process_list_items" but still need to set "p_num_items_found" so that
               // the tree files suffix will be appended.
               if( is_owner && !is_fetching && has_all_tree_items )
               {
                  if( p_num_items_found )
                     *p_num_items_found = 1;
               }
               else
               {
                  if( !last_data_tree_is_identical( blockchain, height - 1 ) )
                     process_list_items( blockchain,
                      tree_root_hash, true, p_num_items_found, &list_items_to_ignore, &dtm, p_progress );
               }
            }

            set_session_variable(
             get_special_var_name( e_special_var_blockchain_tree_root_hash ), "" );
         }
      }
   }
}

struct scoped_pop_peer_get_hash
{
   scoped_pop_peer_get_hash( bool pop_immediately = false )
   {
      if( !pop_immediately )
         has_popped = false;
      else
      {
         has_popped = true;
         pop_next_peer_file_hash_to_get( );
      }
   }

   ~scoped_pop_peer_get_hash( )
   {
      if( !has_popped )
         pop_next_peer_file_hash_to_get( );
   }

   bool has_popped;
};

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
    blockchain_height_other( 0 ),
    blockchain_height_pending( 0 ),
    is_for_support( is_for_support ),
    is_time_for_check( false ),
    has_identity_archive( false ),
    has_checked_stream_cipher( false ),
    session_state( session_state ),
    session_op_state( session_state ),
    session_trust_level( e_peer_trust_level_none )
   {
      had_usage = false;

      dtm_last_get = dtm_last_issued = dtm_rcvd_not_found
       = dtm_sent_not_found = ( date_time::standard( ) - ( seconds )1.0 );

      is_responder = ( session_state == e_peer_state_responder );

      if( get_is_test_session( ) )
         want_to_do_op( e_op_init );

      if( !blockchain.empty( ) )
      {
         if( !is_for_support )
            set_blockchain_tree_item( blockchain, 0 );

         identity = replaced( blockchain, c_bc_prefix, "" );
      }

      last_issued_was_put = !is_responder;
   }

   ~socket_command_handler( )
   {
      // NOTE: Replace the current progress message with the last zenith height.
      if( !is_for_support && !get_identity( ).empty( ) )
      {
         // FUTURE: This message should be handled as server string messages.
         string progress_message( "Currently at height " );

         progress_message += to_string( blockchain_height );

         set_system_variable( c_progress_output_prefix + get_identity( ), progress_message );
      }

      TRACE_LOG( TRACE_SESSIONS, get_blockchain( ).empty( )
       ? "finished peer session" : "finished peer session for blockchain " + get_blockchain( ) );
   }

#ifdef SSL_SUPPORT
   ssl_socket& get_socket( ) { return socket; }
#else
   tcp_socket& get_socket( ) { return socket; }
#endif

   void init_for_archive( const string& new_archive_identity )
   {
      archive_identity = new_archive_identity;

      has_identity_archive = has_file_archive( archive_identity );
   }

   bool get_is_time_for_check( )
   {
      if( is_time_for_check )
         return true;

      int64_t current = unix_time( date_time::local( ) );

      if( current >= time_val )
         is_time_for_check = true;

      return is_time_for_check;
   }

   void output_progress( const string& message, unsigned long num = 0, unsigned long total = 0 )
   {
      progress* p_progress = 0;
      trace_progress progress( TRACE_SOCK_OPS );

      if( get_trace_flags( ) & TRACE_SOCK_OPS )
         p_progress = &progress;

      string extra;

      if( num || total )
      {
         extra += to_string( num );

         if( total )
            extra += '/' + to_string( total );
      }

      if( socket.write_line( string( c_response_message_prefix )
       + message + extra, c_request_timeout, p_progress ) <= 0 )
         throw runtime_error( "unexpected peer socket write failure" );
   }

   const date_time& get_dtm_last_get( ) { return dtm_last_get; }
   void set_dtm_last_get( const date_time& dtm ) { dtm_last_get = dtm; }

   const date_time& get_dtm_last_issued( ) { return dtm_last_issued; }
   void set_dtm_last_issued( const date_time& dtm ) { dtm_last_issued = dtm; }

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
   const string& get_archive_identity( ) const { return archive_identity; }

   size_t get_blockchain_height( ) const { return blockchain_height; }
   size_t get_blockchain_height_other( ) const { return blockchain_height_other; }

   void set_blockchain_height( size_t new_height )
   {
      blockchain_height_pending = blockchain_height = new_height;
   }

   void set_blockchain_height_other( size_t new_height )
   {
      blockchain_height_other = new_height;

      set_session_variable( get_special_var_name(
       e_special_var_blockchain_height_other ), to_string( new_height ) );
   }

   set< string >& get_list_items_to_ignore( ) { return list_items_to_ignore; }

   bool get_is_test_session( ) const { return is_local && is_responder && blockchain.empty( ); }

   string& prior_file( ) { return prior_file_hash; }

   void get_hello( );
   void put_hello( );

   void get_file( const string& hash, string* p_file_data );
   void put_file( const string& hash );

   void msg_peer( const string& data );

   bool chk_file( const string& hash, string* p_response = 0 );

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

      if( !is_condemned_session( ) )
         condemn_this_session( );
   }

   private:
   void preprocess_command_and_args( string& str, const string& cmd_and_args, bool /*skip_command_usage*/ );

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
   bool has_checked_stream_cipher;

   set< string > list_items_to_ignore;

   int64_t time_val;

   size_t last_num_tree_item;

   string identity;
   string blockchain;

   string archive_identity;

   size_t blockchain_height;
   size_t blockchain_height_other;
   size_t blockchain_height_pending;

   string last_command;
   string next_command;

   string prior_file_hash;

   date_time dtm_last_get;
   date_time dtm_last_issued;

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

   socket.set_no_delay( );

   socket.write_line( string( c_cmd_peer_session_get ) + " "
    + file_hash_for_write( socket, hello_hash ), c_request_timeout, p_progress );

   try
   {
      store_temp_file( hello_hash, temp_file_name, socket, p_progress );

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

   progress* p_sock_progress = 0;
   trace_progress sock_progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_sock_progress = &sock_progress;

   string data, hello_hash;
   data = get_hello_data( hello_hash );

   if( !has_file( hello_hash ) )
      create_raw_file( data, false );

   socket.set_delay( );
   socket.write_line( string( c_cmd_peer_session_put ) + " " + hello_hash, c_request_timeout, p_sock_progress );

   fetch_file( hello_hash, socket, p_sock_progress );
}

void socket_command_handler::get_file( const string& hash_info, string* p_file_data )
{
   last_issued_was_put = false;

   progress* p_sock_progress = 0;
   trace_progress sock_progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_sock_progress = &sock_progress;

   string::size_type pos = hash_info.find( ':' );

   string hash( hash_info.substr( 0, pos ) );

   socket.set_no_delay( );

   socket.write_line( string( c_cmd_peer_session_get ) + " "
    + file_hash_for_write( socket, hash ), c_request_timeout, p_sock_progress );

   bool is_list = false;
   size_t num_bytes = 0;

   if( !p_file_data )
      throw runtime_error( "unexpected null p_file_data in socket_command_handler::get_file" );
   else
   {
      string file_data, raw_file_data;

      if( !store_file( hash, socket, 0, p_sock_progress, true, 0, true, &file_data, &num_bytes, 0, &raw_file_data ) )
         throw runtime_error( "unexpected file '" + hash + "' already exists" );

      *p_file_data = file_data;

      if( is_list_file( file_data[ 0 ] ) )
      {
         is_list = true;

         if( !has_identity_archive )
            create_raw_file( file_data, true, 0, 0, 0, true, true );
         else
         {
            guard g( g_mutex, "get_file" );

            create_raw_file_in_archive( archive_identity, hash, raw_file_data );
         }
      }
   }

   // NOTE: If the file is a list then also need to get all of its items.
   if( is_list )
   {
      string tree_root_hash( get_raw_session_variable(
       get_special_var_name( e_special_var_blockchain_tree_root_hash ) ) );

      if( is_owner && !has_identity_archive && !tree_root_hash.empty( ) )
         tag_file( c_ciyam_tag, tree_root_hash );

      set_session_variable(
       get_special_var_name( e_special_var_blockchain_tree_root_hash ), "" );

      date_time dtm( date_time::local( ) );

      size_t num_items_found = get_blockchain_tree_item( blockchain );

      process_list_items( blockchain, hash, false, &num_items_found, &list_items_to_ignore, &dtm, this );
   }

   increment_peer_files_downloaded( num_bytes );
}

void socket_command_handler::put_file( const string& hash )
{
   last_issued_was_put = true;

   progress* p_sock_progress = 0;
   trace_progress sock_progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_sock_progress = &sock_progress;

   socket.set_delay( );
   socket.write_line( string( c_cmd_peer_session_put ) + " " + hash, c_request_timeout, p_sock_progress );

   fetch_file( hash, socket, p_sock_progress );

   increment_peer_files_uploaded( file_bytes( hash ) );
}

void socket_command_handler::msg_peer( const string& data )
{
   progress* p_sock_progress = 0;
   trace_progress sock_progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_sock_progress = &sock_progress;

   socket.set_no_delay( );
   socket.write_line( string( c_cmd_peer_session_msg ) + " " + data, c_request_timeout, p_sock_progress );

   string response;
   if( socket.read_line( response, c_request_timeout, 0, p_sock_progress ) <= 0 )
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

bool socket_command_handler::chk_file( const string& hash_or_tag, string* p_response )
{
   bool has_extra = false;

   progress* p_sock_progress = 0;
   trace_progress sock_progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_sock_progress = &sock_progress;

   string expected;

   socket.set_no_delay( );

   bool added_nonce = false;

   if( p_response )
   {
      string request( hash_or_tag );

      replace( request, blockchain, get_special_var_name( e_special_var_blockchain ) );

      socket.write_line(
       string( c_cmd_peer_session_chk ) + " " + request, c_request_timeout, p_sock_progress );
   }
   else
   {
      added_nonce = true;

      string nonce( uuid( ).as_string( ) );

      expected = hash_with_nonce( hash_or_tag, nonce );

      socket.write_line( string( c_cmd_peer_session_chk )
       + " " + hash_or_tag + " " + nonce, c_request_timeout, p_sock_progress );
   }

   string response;

   string paired_identity( get_raw_session_variable(
    get_special_var_name( e_special_var_paired_identity ) ) );

   string backup_identity( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_backup_identity ) ) );

   while( true )
   {
      response.erase( );

      if( socket.read_line( response, c_request_timeout, 0, p_sock_progress ) <= 0 )
      {
         string error;

         if( !socket.had_timeout( ) )
            error = "peer has terminated this connection";
         else
            error = "timeout occurred getting peer response";

         socket.close( );
         throw runtime_error( error );
      }

      string::size_type pos = response.find( c_response_message_prefix );

      if( pos == 0 )
      {
         if( is_condemned_session( ) )
            throw runtime_error( "peer session has been condemned" );

         if( g_server_shutdown )
            throw runtime_error( "application server is being shutdown" );

         if( !paired_identity.empty( ) && get_system_variable( paired_identity ).empty( ) )
         {
            bool is_only_session = false;
            bool is_missing_backup = false;

            if( get_is_time_for_check( ) )
            {
               is_only_session = ( num_have_session_variable( paired_identity ) < 2 );

               if( !is_only_session && !backup_identity.empty( ) 
                && ( num_have_session_variable( backup_identity, true ) < 2 ) )
                  is_missing_backup = true;
            }

            bool is_disconnecting = !get_system_variable( '~' + identity ).empty( );

            if( !is_disconnecting )
               is_disconnecting = !get_system_variable( '~' + paired_identity ).empty( );

            if( is_only_session || is_missing_backup || is_disconnecting )
            {
               set_session_variable( paired_identity, "" );

               throw runtime_error( "peer session disconnected" );
            }
         }

         response.erase( 0, strlen( c_response_message_prefix ) );

         if( response != "." )
            set_session_progress_message( response );

         continue;
      }

      if( !response.empty( ) && response[ response.length( ) - 1 ] == c_tree_files_suffix )
      {
         has_extra = true;
         response.erase( response.length( ) - 1 );
      }

      if( response == string( c_response_not_found ) )
         response.erase( );
      else if( !added_nonce )
         response = file_hash_for_read( socket, response );

      break;
   }

   if( p_response )
      *p_response = response;
   else if( response != expected )
      throw runtime_error( "unexpected invalid chk response: " + response );

   return has_extra;
}

bool socket_command_handler::want_to_do_op( op o ) const
{
   // NOTE: These statics are only to provide consistent behaviour for a
   // single interactive test session (and are not used by normal peers).
   static unsigned int chk_count = 0;
   static unsigned int msg_count = 0;

   bool retval = false;

   if( get_is_test_session( ) )
   {
      if( o == e_op_init )
         chk_count = msg_count = 0;
      else if( o == e_op_msg )
         retval = ( ++msg_count % 3 == 0 );
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
         if( o == e_op_msg )
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
   // NOTE: If a prior file no longer exists locally then it is to be expected
   // it would not exist in the peer either.
   if( !prior_file( ).empty( ) && !has_file( prior_file( ) ) )
      prior_file( ).erase( );

   string next_hash_to_get, next_hash_to_put;

   string peer_map_key( get_peer_map_key( false ) );

   string progress_count_name( get_special_var_name( e_special_var_progress_count ) );
   string progress_total_name( get_special_var_name( e_special_var_progress_total ) );
   string progress_value_name( get_special_var_name( e_special_var_progress_value ) );

   string blockchain_is_owner_name( get_special_var_name( e_special_var_blockchain_is_owner ) );
   string blockchain_is_fetching_name( get_special_var_name( e_special_var_blockchain_is_fetching ) );
   string blockchain_first_mapped_name( get_special_var_name( e_special_var_blockchain_first_mapped ) );
   string blockchain_zenith_height_name( get_special_var_name( e_special_var_blockchain_zenith_height ) );
   string blockchain_block_processing_name( get_special_var_name( e_special_var_blockchain_block_processing ) );
   string blockchain_height_processing_name( get_special_var_name( e_special_var_blockchain_height_processing ) );

   string block_processing( get_raw_session_variable( blockchain_block_processing_name ) );

   bool is_waiting_for_hub = !get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_waiting_for_hub ) ).empty( );

   // FUTURE: Should use the hub session's X/Y height for identity progress.
   if( !is_for_support && !is_waiting_for_hub )
   {
      date_time now( date_time::local( ) );
      date_time dtm( get_dtm_last_issued( ) );

      uint64_t elapsed = seconds_between( dtm, now );

      if( elapsed >= c_default_progress_seconds )
      {
         set_dtm_last_issued( now );

         size_t num_tree_item = get_blockchain_tree_item( blockchain );

         if( num_tree_item != last_num_tree_item )
         {
            last_num_tree_item = num_tree_item;

            string num_tree_items( get_raw_session_variable(
             get_special_var_name( e_special_var_blockchain_num_tree_items ) ) );

            if( !num_tree_items.empty( ) )
            {
               size_t upper_limit = from_string< size_t >( num_tree_items );

               // NOTE: Ensure that the upper limit is not exceeded.
               if( num_tree_item > upper_limit )
               {
                  last_num_tree_item = num_tree_item = upper_limit;
                  add_to_blockchain_tree_item( blockchain, 0, upper_limit );
               }
            }

            // FUTURE: This message should be handled as a server string message.
            string progress_message( "Syncing at height " + to_string( blockchain_height + 1 ) );

            if( blockchain_height_other > ( blockchain_height + 1 ) )
               progress_message += '/' + to_string( blockchain_height_other );

            progress_message += c_percentage_separator;

            string count( to_string( num_tree_item ) );

            if( num_tree_items.empty( ) )
               progress_message += count;
            else
            {
               set_session_variable( progress_count_name, count );
               set_session_variable( progress_total_name, num_tree_items );

               progress_message += get_raw_session_variable( progress_value_name );
            }

            progress_message += to_string( c_ellipsis );

            set_session_progress_message( progress_message );
         }

         system_identity_progress_message( identity );
      }
   }

   string hub_identity( get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_peer_hub_identity ) ) );

   if( is_waiting_for_hub
    && add_put_list_if_available( hub_identity, blockchain, blockchain_height_pending ) )
   {
      is_waiting_for_hub = false;
      set_session_variable( get_special_var_name( e_special_var_blockchain_waiting_for_hub ), "" );
   }

   bool set_new_zenith = false;
   bool any_supporter_has = false;

   next_hash_to_get = top_next_peer_file_hash_to_get(
    ( !is_for_support && check_for_supporters ), !is_for_support ? &any_supporter_has : 0 );

   next_hash_to_put = top_next_peer_file_hash_to_put(
    ( !is_for_support && check_for_supporters ), !is_for_support ? &any_supporter_has : 0 );

   if( !any_supporter_has && !is_waiting_for_hub
    && !block_processing.empty( ) && next_hash_to_get.empty( ) )
      set_new_zenith = true;

   bool no_top_hash = ( next_hash_to_get.empty( ) && next_hash_to_put.empty( ) );

   if( no_top_hash && !any_supporter_has && want_to_do_op( e_op_chk ) )
   {
      bool was_not_found = false;
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
         else if( !is_waiting_for_hub )
         {
            // NOTE: Check whether a new block has been created either locally or remotely.
            string next_block_tag( blockchain
             + '.' + to_string( blockchain_height + 1 ) + c_blk_suffix );

            bool has_tree_files = false;
            bool force_tree_root_add = false;

            if( has_tag( next_block_tag ) )
            {
               // NOTE: Use "blockchain_height_pending" here to ensure that only
               // one "process_block_for_height" call will occur for each block.
               // If all of the block related files were found locally then will
               // simply increase the height.
               if( blockchain_height == blockchain_height_pending )
               {
                  string next_block_hash( tag_file_hash( next_block_tag ) );

                  string current_zenith_hash( tag_file_hash( blockchain + c_zenith_suffix ) );

                  size_t current_zenith_height = 0;

                  if( !get_block_height_from_tags( blockchain, current_zenith_hash, current_zenith_height ) )
                     throw runtime_error( "unexpected error determining current zenith height" );

                  bool need_to_check = false;

                  if( current_zenith_height > blockchain_height )
                  {
                     blockchain_height = blockchain_height_pending = current_zenith_height;

                     set_session_variable( blockchain_is_fetching_name, "" );
                     set_session_variable( blockchain_block_processing_name, "" );
                     set_session_variable( blockchain_height_processing_name, "" );

                     output_sync_progress_message( identity, blockchain_height, blockchain_height_other );

                     set_session_variable( blockchain_zenith_height_name, to_string( current_zenith_height ) );
                  }
                  else
                  {
                     // NOTE: If zenith height is not greater than the blockchain height
                     // then assume that fetching had not been previously completed.
                     need_to_check = true;

                     size_t num_items_found = 0;

                     set_blockchain_tree_item( blockchain, 0 );

                     blockchain_height_pending = blockchain_height + 1;

                     temporary_session_variable temp_is_checking(
                      get_special_var_name( e_special_var_blockchain_is_checking ), c_true_value );

                     set_session_variable( blockchain_is_fetching_name, c_true_value );
                     set_session_variable( blockchain_block_processing_name, next_block_hash );
                     set_session_variable( blockchain_height_processing_name, to_string( blockchain_height_pending ) );

                     process_block_for_height( blockchain,
                      next_block_hash, blockchain_height_pending, list_items_to_ignore, &num_items_found, this );

                     // NOTE: This session variable may have been set via the call above so recheck it now.
                     is_waiting_for_hub = !get_raw_session_variable(
                      get_special_var_name( e_special_var_blockchain_waiting_for_hub ) ).empty( );

                     string first_mapped( get_raw_session_variable( blockchain_first_mapped_name ) );
                     set_session_variable( blockchain_first_mapped_name, "" );

                     string tree_root_hash( get_raw_session_variable(
                      get_special_var_name( e_special_var_blockchain_zenith_tree_hash ) ) );

                     bool wants_tree_root = false;

                     if( !tree_root_hash.empty( ) && !has_file( tree_root_hash ) )
                        wants_tree_root = true;

                     if( first_mapped.empty( ) && top_next_peer_file_hash_to_get( ).empty( ) && !wants_tree_root )
                     {
                        set_new_zenith = true;
                        block_processing = next_block_hash;
                     }
                     else
                     {
                        string file_hash( top_next_peer_file_hash_to_get( ) );

                        if( file_hash.empty( ) )
                           file_hash = first_mapped;

                        // NOTE: Use the "nonce" argument to identify the first file needing to
                        // be fetched (so that pull requests are commenced at the right point).
                        if( !file_hash.empty( ) && ( file_hash.find( ':' ) == string::npos ) )
                           next_block_tag += string( " " ) + '@' + file_hash_for_write( socket, file_hash );

                        has_tree_files = chk_file( next_block_tag, &next_block_hash );

                        // NOTE: If fetching was incomplete "@blockchain_get_tree_files" will not have
                        // been set (and the tree root file will not have been added) so if tree files
                        // exist then will force the tree root to be added below.
                        if( has_tree_files && get_raw_session_variable(
                         get_special_var_name( e_special_var_blockchain_get_tree_files ) ).empty( ) )
                           force_tree_root_add = true;

                        has_issued_chk = true;
                     }
                  }
               }
            }
            else
            {
               if( !set_new_zenith )
               {
                  string next_sig_tag( blockchain
                   + '.' + to_string( blockchain_height ) + c_sig_suffix );

                  temporary_session_variable temp_is_checking(
                   get_special_var_name( e_special_var_blockchain_is_checking ), c_true_value );

                  string next_sig_hash;
                  has_tree_files = chk_file( next_sig_tag, &next_sig_hash );

                  has_issued_chk = true;

                  if( next_sig_hash.empty( ) )
                     was_not_found = true;
                  else if( !has_file( next_sig_hash ) )
                  {
                     if( !block_processing.empty( ) )
                        set_new_zenith = true;

                     add_peer_file_hash_for_get( next_sig_hash );
                     blockchain_height_pending = blockchain_height + 1;

                     set_session_variable( blockchain_is_fetching_name, c_true_value );

                     set_session_variable(
                      get_special_var_name( e_special_var_blockchain_signature_file_hash ), next_sig_hash );
                  }
               }
            }

            if( has_issued_chk )
            {
               if( !has_tree_files )
               {
                  check_for_missing_other_sessions( date_time::local( ) );

                  set_session_variable( get_special_var_name( e_special_var_blockchain_get_tree_files ), "" );
                  set_session_variable( get_special_var_name( e_special_var_blockchain_waiting_for_hub ), "" );
               }
               else
               {
                  if( force_tree_root_add || top_next_peer_file_hash_to_get( ).empty( ) )
                  {
                     string tree_root_hash( get_raw_session_variable(
                      get_special_var_name( e_special_var_blockchain_zenith_tree_hash ) ) );

                     if( !is_waiting_for_hub && !tree_root_hash.empty( ) )
                        add_peer_file_hash_for_get( tree_root_hash );
                  }

                  set_session_variable( get_special_var_name( e_special_var_blockchain_get_tree_files ), c_true_value );
               }
            }
         }
      }

      if( !has_issued_chk )
      {
         string tag_or_hash( file_hash_for_write( socket, prior_file( ) ) );

         if( tag_or_hash.empty( ) || is_waiting_for_hub )
            tag_or_hash = blockchain + c_dummy_suffix;

         string chk_hash;
         chk_file( tag_or_hash, &chk_hash );

         if( chk_hash.empty( ) )
            was_not_found = true;
      }

      if( !is_local && was_not_found )
      {
         date_time now( date_time::standard( ) );

         seconds elapsed = ( seconds )( now - dtm_rcvd_not_found );

         dtm_rcvd_not_found = now;

         // NOTE: If has sent and received "not found" within one
         // second then sleep now to avoid unnecessary CPU usage.
         if( elapsed < 1.0 )
         {
            elapsed = ( seconds )( now - dtm_sent_not_found );

            if( elapsed < 1.0 )
            {
               msleep( c_peer_sleep_time );

               // NOTE: When blockchain type is "user" then both this and the paired session can
               // automatically be terminated if both were found sleeping here (within a second).
               if( !get_raw_session_variable(
                get_special_var_name( e_special_var_blockchain_user ) ).empty( ) )
               {
                  string paired_identity( get_raw_session_variable(
                   get_special_var_name( e_special_var_paired_identity ) ) );

                  string paired_sync( get_special_var_name( e_special_var_paired_sync ) );

                  set_session_sync_time( ( identity != paired_identity ? &paired_identity : 0 ), true, 1, &paired_sync );
               }
            }
         }
      }
   }
   else if( want_to_do_op( e_op_msg ) )
      msg_peer( c_dummy_message_data );
   else if( next_hash_to_put.empty( ) )
   {
      string next_hash( next_hash_to_get );

      while( !next_hash.empty( ) && has_file( next_hash.substr( 0, next_hash.find( ':' ) ) ) )
      {
         pop_next_peer_file_hash_to_get( );

         string num_put_files( get_raw_session_variable(
          get_special_var_name( e_special_var_num_put_files ) ) );

         if( !num_put_files.empty( ) )
         {
            date_time dtm( date_time::local( ) );

            set< string > target_hashes;

            process_put_file( blockchain, extract_file( next_hash, "" ),
             check_for_supporters, get_is_test_session( ), target_hashes, &dtm, this );

            if( !target_hashes.empty( ) )
               add_to_num_puts_value( target_hashes.size( ) );

            set_session_variable(
             get_special_var_name( e_special_var_num_put_files ), get_special_var_name( e_special_var_decrement ) );
         }

         next_hash = top_next_peer_file_hash_to_get( );
      }

      if( !next_hash.empty( ) )
      {
         string file_data;

         get_file( next_hash, &file_data );

         // NOTE: Delay popping for support sessions so that the main
         // session won't attempt to create a new zenith whilst files
         // fetched by support sessions are still being processed.
         scoped_pop_peer_get_hash pop_peer_get_hash( !is_for_support );

         bool is_list = false;

         if( !file_data.empty( ) )
            is_list = is_list_file( file_data[ 0 ] ); 
         else
            is_list = is_list_file( next_hash.substr( 0, next_hash.find( ':' ) ) );

         bool has_archive = false;

         if( !get_raw_session_variable( get_special_var_name(
          e_special_var_blockchain_archive_path ) ).empty( ) )
            has_archive = true;

         if( is_for_support && get_raw_session_variable(
          get_special_var_name( e_special_var_blockchain_checked_shared ) ).empty( ) )
            check_shared_for_support_session( blockchain );

         string op_list_hash( get_raw_session_variable(
          get_special_var_name( e_special_var_blockchain_op_list_hash ) ) );

         string put_list_hash( get_raw_session_variable(
          get_special_var_name( e_special_var_blockchain_put_list_hash ) ) );

         string block_file_hash( get_raw_session_variable(
          get_special_var_name( e_special_var_blockchain_block_file_hash ) ) );

         string primary_pubkey_hash( get_raw_session_variable(
          get_special_var_name( e_special_var_blockchain_primary_pubkey_hash ) ) );

         string signature_file_hash( get_raw_session_variable(
          get_special_var_name( e_special_var_blockchain_signature_file_hash ) ) );

         string secondary_pubkey_hash( get_raw_session_variable(
          get_special_var_name( e_special_var_blockchain_secondary_pubkey_hash ) ) );

         string tertiary_pubkey_hash( get_raw_session_variable(
          get_special_var_name( e_special_var_blockchain_tertiary_pubkey_hash ) ) );

         if( ( is_list || is_for_support ) || ( last_num_tree_item
          && ( next_hash != op_list_hash ) && ( next_hash != put_list_hash )
          && ( next_hash != block_file_hash ) && ( next_hash != primary_pubkey_hash )
          && ( next_hash != signature_file_hash ) && ( next_hash != tertiary_pubkey_hash )
          && ( next_hash != secondary_pubkey_hash ) ) )
            add_to_blockchain_tree_item( blockchain, 1 );

         bool has_tertiary = !tertiary_pubkey_hash.empty( );

         string targeted_identity( get_raw_session_variable(
          get_special_var_name( e_special_var_blockchain_targeted_identity ) ) );

         bool has_targeted_identity = false;

         if( !targeted_identity.empty( ) && ( targeted_identity[ 0 ] != '@' ) )
            has_targeted_identity = true;

         string num_put_files( get_raw_session_variable(
          get_special_var_name( e_special_var_num_put_files ) ) );

         if( !num_put_files.empty( ) )
         {
            date_time dtm( date_time::local( ) );

            set< string > target_hashes;

            process_put_file( blockchain, file_data.substr( 1 ),
             check_for_supporters, get_is_test_session( ), target_hashes, &dtm, this );

            if( !has_archive )
               create_raw_file( file_data );
            else
               create_raw_file_in_archive( archive_identity, "", file_data );

            if( !target_hashes.empty( ) )
               add_to_num_puts_value( target_hashes.size( ) );

            set_session_variable(
             get_special_var_name( e_special_var_num_put_files ), get_special_var_name( e_special_var_decrement ) );
         }
         else if( next_hash == op_list_hash )
         {
            validate_op_list_file( file_data );

            set_session_variable(
             get_special_var_name( e_special_var_blockchain_op_list_hash ), "" );

            create_raw_file( file_data, true, 0, 0, next_hash.c_str( ), true, true );

            process_op_list_file( blockchain, next_hash, blockchain_height_pending );
         }
         else if( next_hash == put_list_hash )
         {
            validate_put_list_file( file_data );

            set_session_variable(
             get_special_var_name( e_special_var_blockchain_put_list_hash ), "" );

            create_raw_file( file_data, true, 0, 0, next_hash.c_str( ), true, true );

            process_put_list_file( blockchain, next_hash, blockchain_height_pending, file_data.substr( 1 ) );
         }
         else if( next_hash == block_file_hash )
         {
            set_session_variable(
             get_special_var_name( e_special_var_blockchain_block_file_hash ), "" );

            create_raw_file( file_data, true, 0, 0, next_hash.c_str( ), true, true );

            set_session_variable( blockchain_block_processing_name, block_file_hash );
            set_session_variable( blockchain_height_processing_name, to_string( blockchain_height_pending ) );

            process_block_for_height( blockchain, block_file_hash,
             blockchain_height_pending, list_items_to_ignore, 0, this );

            tag_file( blockchain + '.' + to_string( blockchain_height_pending ) + c_blk_suffix, block_file_hash );
         }
         else if( next_hash == primary_pubkey_hash )
         {
            validate_public_key_file( file_data );

            create_raw_file( file_data, true, 0, 0, next_hash.c_str( ), true, true );

            process_public_key_file( blockchain, primary_pubkey_hash,
             blockchain_height_pending, e_public_key_scale_primary, blockchain_height_other );
         }
         else if( next_hash == signature_file_hash )
         {
            validate_signature_file( file_data );

            create_raw_file( file_data, true, 0, 0, next_hash.c_str( ), true, true );

            if( blockchain_height != blockchain_height_pending )
               process_signature_file( blockchain, signature_file_hash, blockchain_height_pending );
            else
               set_session_variable( get_special_var_name( e_special_var_blockchain_signature_file_hash ), "" );
         }
         else if( next_hash == tertiary_pubkey_hash )
         {
            validate_public_key_file( file_data );

            create_raw_file( file_data, true, 0, 0, next_hash.c_str( ), true, true );

            process_public_key_file( blockchain, tertiary_pubkey_hash,
             blockchain_height_pending, e_public_key_scale_tertiary, blockchain_height_other );
         }
         else if( next_hash == secondary_pubkey_hash )
         {
            validate_public_key_file( file_data );

            create_raw_file( file_data, true, 0, 0, next_hash.c_str( ), true, true );

            process_public_key_file( blockchain, secondary_pubkey_hash,
             blockchain_height_pending, e_public_key_scale_secondary, blockchain_height_other );
         }
         else if( next_hash[ next_hash.length( ) - 1 ] != c_repository_suffix )
         {
            if( !is_list )
            {
               if( has_targeted_identity )
               {
                  string file_data_hash( sha256( file_data ).get_digest_as_string( ) );

                  string peer_mapped_hash( get_peer_mapped_hash_info( peer_map_key, next_hash ) );

                  if( peer_mapped_hash.empty( ) )
                     throw runtime_error( "unexpected unmapped file hash '" + next_hash + "'" );
                  else if( file_data_hash != peer_mapped_hash )
                     throw runtime_error( "found invalid encrypted file content for hash '" + next_hash + "'" );

                  if( has_archive )
                     create_raw_file_in_archive( archive_identity, next_hash, file_data );
                  else
                     create_raw_file( file_data, true, 0, 0, next_hash.c_str( ), true, true );
               }
               else
               {
                  // NOTE: Unless both are owners only core block files should be found here.
                  if( get_raw_session_variable(
                   get_special_var_name( e_special_var_blockchain_both_are_owners ) ).empty( ) )
                     verify_core_file( file_data, false );

                  create_raw_file( file_data, true, 0, 0, next_hash.c_str( ), true, true );

                  process_core_file( next_hash, blockchain );
               }
            }
         }
#ifdef SSL_SUPPORT
         else
         {
            if( get_raw_session_variable(
             get_special_var_name( e_special_var_blockchain_both_are_owners ) ).empty( ) )
            {
               // NOTE: The first call to "set_session_variable_for_matching_blockchains" (made
               // by a main session to set "@blockchain_stream_cipher" for its matching support
               // sessions) could occur prior to the "init_session" call in the support session
               // (in which case that support session would be skipped) so if this is the first
               // time that "process_repository_file" is being called by a support session then
               // will try to get this session variable from the main session.
               if( is_for_support && !has_checked_stream_cipher )
               {
                  has_checked_stream_cipher = true;

                  string stream_cipher_var_name(
                   get_special_var_name( e_special_var_blockchain_stream_cipher ) );

                  set_session_variable( stream_cipher_var_name,
                   get_session_variable_from_matching_blockchain( stream_cipher_var_name,
                   get_special_var_name( e_special_var_blockchain_peer_supporter ), "", true ) );
               }

               process_repository_file( blockchain,
                next_hash.substr( 0, next_hash.length( ) - 1 ), get_is_test_session( ), &file_data );
            }
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

         if( !get_raw_session_variable( next_hash ).empty( ) )
         {
            delete_file( next_hash );
            set_session_variable( next_hash, "" );
         }
      }
   }

   if( set_new_zenith )
   {
      date_time dtm( date_time::local( ) );

      string zenith_tree_hash( get_raw_session_variable(
       get_special_var_name( e_special_var_blockchain_zenith_tree_hash ) ) );

      set_session_variable(
       get_special_var_name( e_special_var_blockchain_zenith_tree_hash ), "" );

      bool peer_has_tree_items = !get_raw_session_variable(
       get_special_var_name( e_special_var_blockchain_get_tree_files ) ).empty( );

      if( peer_has_tree_items && !zenith_tree_hash.empty( ) )
      {
         bool is_in_archive = false;

         if( has_file( zenith_tree_hash, true, &is_in_archive ) )
         {
            string put_file_data( c_file_type_str_blob );

            // NOTE: Will touch all tree lists and create "put" files if the tree items were present.
            if( !has_all_list_items( blockchain, zenith_tree_hash, true, true, &dtm, this, 0, &put_file_data ) )
               throw runtime_error( "unexpected missing tree files" );

            string put_tag_name( c_bc_prefix + identity
             + '.' + to_string( blockchain_height_pending ) + c_put_suffix );

            string queue_puts_name( get_special_var_name( e_special_var_queue_puts ) );

            string next_put( get_raw_session_variable( queue_puts_name ) );

            string put_hashes;

            while( !next_put.empty( ) )
            {
               if( !put_hashes.empty( ) )
                  put_hashes += '\n';
               else
                  put_hashes += c_file_type_str_blob;

               put_hashes += base64::encode( hex_decode( next_put ) );

               next_put = get_raw_session_variable( queue_puts_name );
            }

            if( !put_hashes.empty( ) )
               create_raw_file( put_hashes, false, put_tag_name.c_str( ) );

            // NOTE: If a "put" list had been created and the system has a hub blockchain then will
            // set "@generate_hub_block" so that the next hub block can be automatically generated.
            if( !is_owner && has_tag( put_tag_name ) && !get_system_variable(
             get_special_var_name( e_special_var_blockchain_peer_hub_identity ) ).empty( ) )
               set_system_variable( get_special_var_name( e_special_var_generate_hub_block ), c_true_value );

            if( !is_in_archive
             && !get_raw_session_variable( blockchain_is_owner_name ).empty( )
             && !get_raw_session_variable( blockchain_is_fetching_name ).empty( ) )
               tag_file( c_ciyam_tag, zenith_tree_hash );
         }
         else
            throw runtime_error( "unexpected missing tree root" );
      }

      remove_obsolete_repository_entries( identity, &dtm, this, true );

      last_num_tree_item = 0;
      set_blockchain_tree_item( blockchain, 0 );

      list_items_to_ignore.clear( );

      blockchain_height = blockchain_height_pending;

      tag_file( blockchain + c_zenith_suffix, block_processing );

      set_session_variable( get_special_var_name(
       e_special_var_blockchain_num_tree_items ), "" );

      set_session_variable( get_special_var_name(
       e_special_var_blockchain_zenith_height ), to_string( blockchain_height ) );

      output_sync_progress_message( identity, blockchain_height, blockchain_height_other, true );

      TRACE_LOG( TRACE_PEER_OPS, "=== new zenith hash: "
       + block_processing + " height: " + to_string( blockchain_height ) );

      if( !peer_map_key.empty( ) )
         clear_all_peer_mapped_hashes( peer_map_key );

      string genesis_key_tag( blockchain + ".0" + string( c_key_suffix ) );

      if( has_tag( genesis_key_tag ) )
         delete_file( tag_file_hash( genesis_key_tag ) );

      set_session_variable( blockchain_first_mapped_name, "" );
      set_session_variable( blockchain_block_processing_name, "" );

      if( blockchain_height == blockchain_height_other )
      {
         set_session_variable( blockchain_is_fetching_name, "" );
         set_session_variable( blockchain_height_processing_name, "" );
      }

      get_raw_session_variable( get_special_var_name( e_special_var_progress_clear ) );

      set_session_variable( get_special_var_name( e_special_var_blockchain_num_puts ), "" );

      set_session_variable(
       get_special_var_name( e_special_var_blockchain_zenith_height ), to_string( blockchain_height ) );

      if( !get_raw_session_variable( get_special_var_name( e_special_var_blockchain_is_hub ) ).empty( ) )
         process_queued_hub_using_peerchains( identity );
      else if( is_extra_non_hub_identity( identity ) )
      {
         temporary_session_variable tmp_identity(
          get_special_var_name( e_special_var_identity ), identity );

         bool is_shared = !get_raw_session_variable(
          get_special_var_name( e_special_var_blockchain_targeted_identity ) ).empty( );

         temporary_session_variable tmp_chain_type(
          get_special_var_name( e_special_var_chain_type ), !is_shared ? "-backup" : "-shared" );

         run_script( "generate_parallel_chains", false, false, true );
      }

      if( !zenith_tree_hash.empty( )
       && get_raw_session_variable( get_special_var_name( e_special_var_blockchain_user ) ).empty( ) )
      {
         string targeted_identity( get_raw_session_variable(
          get_special_var_name( e_special_var_blockchain_targeted_identity ) ) );

         if( !targeted_identity.empty( )
          && ( targeted_identity[ 0 ] != '@' ) && ( blockchain_height == blockchain_height_other ) )
         {
            if( is_targeted_identity( identity, targeted_identity, zenith_tree_hash, blockchain_height ) )
            {
               tag_file( blockchain + c_shared_suffix, block_processing );

               string backup_identity( get_raw_session_variable(
                get_special_var_name( e_special_var_blockchain_backup_identity ) ) );

               if( backup_identity.empty( ) )
                  set_system_variable( get_special_var_name(
                   e_special_var_export_needed ) + '_' + identity, identity );
               else
                  set_system_variable( get_special_var_name(
                   e_special_var_export_needed ) + '_' + backup_identity, identity );
            }
         }
      }
   }
}

void socket_command_handler::preprocess_command_and_args( string& str, const string& cmd_and_args, bool /*skip_command_usage*/ )
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
}

void socket_command_handler::handle_command_response( const string& response, bool is_special )
{
   progress* p_sock_progress = 0;
   trace_progress sock_progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_sock_progress = &sock_progress;

   if( !response.empty( ) )
   {
      if( !is_special && is_responder )
         socket.set_delay( );
      else
         socket.set_no_delay( );

      socket.write_line( response, c_request_timeout, p_sock_progress );
   }

   if( !is_special && is_responder )
   {
      socket.set_no_delay( );
      socket.write_line( c_response_okay, c_request_timeout, p_sock_progress );
   }
}

class peer_session_command_functor : public command_functor
{
   public:
   peer_session_command_functor( command_handler& handler )
    : command_functor( handler ),
    socket_handler( dynamic_cast< socket_command_handler& >( handler ) )
   {
      clear_ignore_count = 0;

      blockchain_is_fetching_name = get_special_var_name( e_special_var_blockchain_is_fetching );
      blockchain_zenith_height_name = get_special_var_name( e_special_var_blockchain_zenith_height );
      blockchain_peer_has_supporters_name = get_special_var_name( e_special_var_blockchain_peer_has_supporters );
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   size_t clear_ignore_count;

   socket_command_handler& socket_handler;

   string blockchain_is_fetching_name;
   string blockchain_zenith_height_name;
   string blockchain_peer_has_supporters_name;
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

   progress* p_sock_progress = 0;
   trace_progress sock_progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_sock_progress = &sock_progress;

   if( command != c_cmd_peer_session_bye )
      socket.set_delay( );

   string hello_data, hello_hash;
   hello_data = get_hello_data( hello_hash );

   set_last_session_cmd( command );

   increment_session_commands_executed( );

   string identity( socket_handler.get_identity( ) );
   string blockchain( socket_handler.get_blockchain( ) );
   string archive_identity( socket_handler.get_archive_identity( ) );

   size_t blockchain_height = socket_handler.get_blockchain_height( );
   size_t blockchain_height_other = socket_handler.get_blockchain_height_other( );

   string blockchain_height_processing_name( get_special_var_name( e_special_var_blockchain_height_processing ) );

   bool had_zenith_hash = false;
   bool check_for_supporters = false;

   bool is_owner = !get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_is_owner ) ).empty( );

   bool other_is_owner = !get_raw_session_variable(
    get_special_var_name( e_special_var_blockchain_other_is_owner ) ).empty( );

   if( !get_raw_session_variable( blockchain_zenith_height_name ).empty( ) )
      had_zenith_hash = true;

   if( !get_raw_session_variable( blockchain_peer_has_supporters_name ).empty( ) )
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

         replace( tag_or_hash, get_special_var_name( e_special_var_blockchain ), blockchain );

         string hash( file_hash_for_read( socket, tag_or_hash ) );

         if( hash == hello_hash )
            response = file_hash_for_write( socket, hello_hash );

         bool is_new_sig = false;

         string new_sig_tag( blockchain + '.' + to_string( blockchain_height ) + c_sig_suffix );

         // NOTE: If a new block was just created the signature can appear before the new block
         // height has been discovered so will simply report as "not found" in order to provide
         // time for this to occur (in "issue_cmd_for_peer").
         if( tag_or_hash == new_sig_tag )
            is_new_sig = true;
         else if( has_tag( tag_or_hash ) )
            hash = tag_file_hash( tag_or_hash );

         bool is_dummy = false;

         if( !blockchain.empty( ) && ( tag_or_hash == ( blockchain + c_dummy_suffix ) ) )
            is_dummy = true;

         // NOTE: Unless doing interactive testing any peer "chk tag" request must start
         // with the blockchain tag prefix (otherwise a peer might be able to find files
         // that are not intended for their discovery).
         if( !blockchain.empty( )
          && ( tag_or_hash.find( blockchain ) != 0 )
          && ( tag_or_hash.length( ) != ( c_sha256_digest_size * 2 ) ) )
            throw runtime_error( "invalid non-blockchain prefixed tag" );

         if( tag_or_hash.find( c_key_suffix ) != string::npos )
            throw runtime_error( "invalid suspicious tag '" + tag_or_hash + "'" );

         bool has = ( is_dummy || is_new_sig ) ? false : has_file( hash, false );
         bool was_initial_state = ( socket_handler.state( ) == e_peer_state_responder );

         size_t blk_offset = 0;
         size_t height_from_tag = 0;

         if( !is_dummy && !blockchain.empty( ) && ( tag_or_hash.find( blockchain ) == 0 ) )
         {
            string height( tag_or_hash.substr( blockchain.length( ) + 1 ) );

            string::size_type pos = height.find( c_blk_suffix );

            if( pos != string::npos )
               blk_offset = 1;
            else
               pos = height.find( c_sig_suffix );

            if( pos != string::npos )
            {
               height.erase( pos );
               height_from_tag = from_string< size_t >( height );

               if( !height_from_tag )
                  blk_offset = 0;

               clear_first_prefixed( );

               set_session_variable( get_special_var_name( e_special_var_tree_count ), "" );
               set_session_variable( get_special_var_name( e_special_var_tree_total ), "" );

               if( has || was_initial_state )
               {
                  size_t old_blockchain_height_other = blockchain_height_other;

                  blockchain_height_other = ( height_from_tag - blk_offset );

                  if( was_initial_state
                   || ( blockchain_height_other != old_blockchain_height_other ) )
                     socket_handler.set_blockchain_height_other( blockchain_height_other );
               }
            }
         }

         if( !has )
         {
            response = c_response_not_found;

            if( height_from_tag )
            {
               size_t old_blockchain_height_other = blockchain_height_other;

               blockchain_height_other = ( height_from_tag - blk_offset );

               if( blockchain_height_other != old_blockchain_height_other )
               {
                  socket_handler.set_blockchain_height_other( blockchain_height_other );

                  if( has_tag( blockchain + c_shared_suffix, e_file_type_blob ) )
                  {
                     string backup_identity( get_raw_session_variable(
                      get_special_var_name( e_special_var_blockchain_backup_identity ) ) );

                     if( backup_identity.empty( ) )
                        set_system_variable( get_special_var_name(
                         e_special_var_export_needed ) + '_' + identity, identity );
                     else
                        set_system_variable( get_special_var_name(
                         e_special_var_export_needed ) + '_' + backup_identity, identity );
                  }

                  output_sync_progress_message( identity, blockchain_height, blockchain_height_other );
               }
            }

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
                        add_peer_file_hash_for_get( file_hash_for_read( socket, nonce ) );
                  }
               }
               else
               {
                  handler.issue_command_response( response, true );

                  handler.issue_command_response( "put " + file_hash_for_write( socket, hello_hash ), true );

                  socket.set_delay( );
                  fetch_file( hello_hash, socket, p_sock_progress );

                  string temp_file_name( "~" + uuid( ).as_string( ) );

                  try
                  {
                     store_temp_file( hello_hash, temp_file_name, socket, p_sock_progress );

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

               if( fetch_repository_entry_record( identity, hash,
                local_hash, local_public_key, master_public_key, false ) )
               {
                  string pull_hash(
                   create_peer_repository_entry_pull_info( identity, hash, local_hash, local_public_key, "" ) );

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
               hash = hash_with_nonce( hash, nonce );

            size_t num_items_found = 0;

            if( was_initial_state )
            {
               socket_handler.op_state( ) = e_peer_state_waiting_for_get;

               if( blockchain.empty( ) )
                  socket_handler.state( ) = e_peer_state_waiting_for_get;
               else
               {
                  socket_handler.state( ) = e_peer_state_waiting_for_get_or_put;

                  string zenith_height( get_raw_session_variable(
                   get_special_var_name( e_special_var_blockchain_zenith_height ) ) );

                  if( zenith_height.empty( )
                   && get_block_height_from_tags( blockchain, hash, blockchain_height ) )
                  {
                     set_session_variable( blockchain_height_processing_name, to_string( blockchain_height ) );

                     process_block_for_height( blockchain, hash, blockchain_height,
                      socket_handler.get_list_items_to_ignore( ), &num_items_found, &socket_handler );
                  }
               }
            }
            else if( !socket_handler.get_is_for_support( ) && ( tag_or_hash.find( c_bc_prefix ) == 0 ) )
            {
               string::size_type pos = tag_or_hash.find( c_sig_suffix );

               string block_hash( hash );

               if( pos != string::npos )
               {
                  string prefix( c_bc_prefix + identity + '.' );
                  size_t height = from_string< size_t >( tag_or_hash.substr( prefix.length( ), pos - prefix.length( ) ) );

                  string block_tag( c_bc_prefix + identity + '.' + to_string( height + 1 ) + c_blk_suffix );

                  block_hash = tag_file_hash( block_tag );
               }

               if( get_block_height_from_tags( blockchain, block_hash, blockchain_height ) )
               {
                  string first_item_hash;

                  bool was_incomplete = false;

                  // NOTE: In the case where a peer had not completed fetching
                  // all files for a new block height the nonce value (using a
                  // '@' prefix) identifies the next "get" file which needs to
                  // be found in the first "put" file.
                  if( !nonce.empty( ) && nonce[ 0 ] == '@' )
                  {
                     was_incomplete = true;
                     first_item_hash = file_hash_for_read( socket, nonce.substr( 1 ) );
                  }

                  temporary_session_variable temp_hash(
                   get_special_var_name( e_special_var_hash ), first_item_hash );

                  temporary_session_variable temp_height(
                   blockchain_height_processing_name, to_string( blockchain_height ) );

                  // NOTE: Set "num_items_found" to a non-zero value
                  // so that the tree files suffix will be appended.
                  num_items_found = 1;

                  vector< string > hex_encoded_hashes;

                  string put_tag_name( c_bc_prefix + identity + '.' + to_string( blockchain_height ) + c_put_suffix );

                  if( has_tag( put_tag_name ) )
                  {
                     string put_hashes( extract_file( tag_file_hash( put_tag_name ), "" ) );

                     vector< string > all_put_hashes;

                     split( put_hashes, all_put_hashes, '\n' );

                     string target_hash( c_file_repository_target_hash_line_prefix );

                     if( !first_item_hash.empty( ) )
                        target_hash += base64::encode( hex_decode( first_item_hash ) );

                     for( size_t i = 0; i < all_put_hashes.size( ); i++ )
                     {
                        string next_put( all_put_hashes[ i ] );

                        string next_hash( hex_encode( base64::decode( next_put ) ) );

                        if( !has_file( next_hash ) )
                        {
                           num_items_found = 0;
                           break;
                        }

                        if( !first_item_hash.empty( ) )
                        {
                           if( extract_file( next_hash, "" ).find( target_hash ) != string::npos )
                              first_item_hash.erase( );
                        }

                        if( first_item_hash.empty( ) )
                           hex_encoded_hashes.push_back( next_hash );
                     }
                  }

                  if( num_items_found )
                  {
                     date_time dtm( date_time::local( ) );

                     string block_tag_name( c_bc_prefix + identity + '.' + to_string( blockchain_height ) + c_blk_suffix );

                     string block_content( construct_blob_for_block_content( extract_file( tag_file_hash( block_tag_name ), "" ) ) );

                     verify_core_file( block_content, false );

                     string tree_root_hash( get_raw_session_variable(
                      get_special_var_name( e_special_var_blockchain_tree_root_hash ) ) );

                     if( tree_root_hash.empty( ) )
                        num_items_found = 0;
                     else
                     {
                        bool is_user = !get_raw_session_variable(
                         get_special_var_name( e_special_var_blockchain_user ) ).empty( );

                        bool is_peerchain_hub = !get_system_variable(
                         get_special_var_name( e_special_var_blockchain_peer_hub_identity ) ).empty( );

                        bool has_all_tree_items = false;

                        size_t total_items = 0;
                        map< string, size_t > first_prefixed;

                        temporary_session_variable temp_progress(
                         get_special_var_name( e_special_var_progress_message ) );

                        has_all_tree_items = has_all_list_items( blockchain,
                         tree_root_hash, true, false, &dtm, &socket_handler, &total_items, 0, &first_prefixed );

                        if( !has_all_tree_items )
                        {
                           first_prefixed.clear( );

                           set_session_variable( get_special_var_name( e_special_var_tree_count ), "" );
                           set_session_variable( get_special_var_name( e_special_var_tree_total ), "" );
                        }
                        else
                        {
                           set_session_variable( get_special_var_name( e_special_var_tree_count ), "0" );
                           set_session_variable( get_special_var_name( e_special_var_tree_total ), to_string( total_items ) );
                        }

                        save_first_prefixed( first_prefixed, total_items );

                        if( !has_all_tree_items )
                           num_items_found = 0;
                        else if( !is_peerchain_hub )
                        {
                           for( size_t i = 0; i < hex_encoded_hashes.size( ); i++ )
                              add_peer_file_hash_for_put( hex_encoded_hashes[ i ] );
                        }
                     }
                  }
               }
            }

            response = file_hash_for_write( socket, hash );

            if( num_items_found )
               response += c_tree_files_suffix;
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
         else
            hash = file_hash_for_read( socket, hash );

         socket.set_delay( );

         fetch_file( hash, socket, p_sock_progress );

         if( hash != hello_hash )
         {
            if( !blockchain.empty( ) )
            {
               check_found_prefixed( hash );

               string tree_count( get_raw_session_variable(
                get_special_var_name( e_special_var_tree_count ) ) );

               string tree_total( get_raw_session_variable(
                get_special_var_name( e_special_var_tree_total ) ) );

               if( !tree_count.empty( ) && !tree_total.empty( ) )
               {
                  date_time now( date_time::local( ) );
                  date_time dtm( socket_handler.get_dtm_last_get( ) );

                  uint64_t elapsed = seconds_between( dtm, now );

                  if( elapsed >= c_default_progress_seconds )
                  {
                     socket_handler.set_dtm_last_get( now );

                     string progress_count_name( get_special_var_name( e_special_var_progress_count ) );
                     string progress_total_name( get_special_var_name( e_special_var_progress_total ) );
                     string progress_value_name( get_special_var_name( e_special_var_progress_value ) );

                     // FUTURE: This message should be handled as a server string message.
                     string progress_message( "Syncing for height " + to_string( blockchain_height_other + 1 ) );

                     if( blockchain_height != ( blockchain_height_other + 1 ) )
                        progress_message += '/' + to_string( blockchain_height );

                     set_session_variable( progress_count_name, tree_count );
                     set_session_variable( progress_total_name, tree_total );

                     progress_message += c_percentage_separator + get_raw_session_variable( progress_value_name );

                     progress_message += to_string( c_ellipsis );

                     set_session_progress_message( progress_message );
                  }
               }
            }

            increment_peer_files_uploaded( file_bytes( hash ) );
         }

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

         bool put_allowed = blockchain.empty( ) || !get_raw_session_variable(
          get_special_var_name( e_special_var_blockchain_is_fetching ) ).empty( );

         if( !put_allowed )
         {
            if( !socket_handler.get_is_responder( ) )
               throw runtime_error( "invalid put when not allowed (initiator)" );
            else
               throw runtime_error( "invalid put when not allowed (responder)" );
         }

         socket.set_delay( );

         size_t num_bytes = 0;

         string file_data;

         if( has_file( hash ) )
         {
            file_data = string( c_file_type_str_blob );
            file_data += extract_file( hash, "" );

            num_bytes = file_data.size( );

            store_temp_file( hash, "", socket, p_sock_progress, true );
         }
         else
            store_file( hash, socket, 0, p_sock_progress, false, 0, false, &file_data, &num_bytes );

         if( hash != hello_hash && ( blockchain.empty( ) || !get_raw_session_variable(
          get_special_var_name( e_special_var_blockchain_get_tree_files ) ).empty( ) ) )
         {
            date_time dtm( date_time::local( ) );

            set< string > target_hashes;

            string queue_puts_name( get_special_var_name( e_special_var_queue_puts ) );

            process_put_file( blockchain, file_data.substr( 1 ), check_for_supporters,
             socket_handler.get_is_test_session( ), target_hashes, &dtm, &socket_handler );

            // NOtE: If is not restoring then will now store the 'put' file data.
            if( !is_owner )
            {
               bool has_archive = false;

               if( !get_raw_session_variable(
                get_special_var_name( e_special_var_blockchain_archive_path ) ).empty( ) )
                  has_archive = true;

               if( !has_archive )
                  create_raw_file( file_data );
               else
                  create_raw_file_in_archive( archive_identity, "", file_data );

               set_session_variable( queue_puts_name, hash );
            }

            if( !target_hashes.empty( ) )
               add_to_num_puts_value( target_hashes.size( ) );
         }

         if( hash != hello_hash )
            increment_peer_files_downloaded( num_bytes );

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
      else if( command == c_cmd_peer_session_msg )
      {
         string data( get_parm_val( parameters, c_cmd_peer_session_msg_data ) );

         // KLUDGE: For now the response is an echo of the data that was provided.
         response = data;

         if( socket_handler.state( ) != e_peer_state_waiting_for_get
          && socket_handler.state( ) != e_peer_state_waiting_for_put
          && socket_handler.state( ) != e_peer_state_waiting_for_get_or_put )
            throw runtime_error( "invalid state for msg" );

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
         socket_handler.kill_session( );

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

   if( !get_raw_session_variable( get_special_var_name( e_special_var_paired_sync ) ).empty( ) )
   {
      // NOTE: For testing purposes setting this system variable will keep these sessions alive.
      if( get_raw_system_variable(
       get_special_var_name( e_special_var_keep_user_peers_alive ) ).empty( ) )
      {
         has_issued_bye = true;

         send_okay_response = false;

         socket.write_line( c_cmd_peer_session_bye, c_request_timeout, p_sock_progress );

         TRACE_LOG( TRACE_SESSIONS, "*** condemning session due to paired session sync ***" );
      }
   }

   // NOTE: It is possible that a support session had been started for
   // a main session which had finished before the support session had
   // been initialised (thus was not condemned by the main session) so
   // condemn own session if is support with no matching main session.
   if( send_okay_response && socket_handler.get_is_for_support( ) )
   {
      if( socket_handler.get_is_time_for_check( )
       && !num_have_session_variable( identity, true ) )
         condemn_this_session( );
   }

   // NOTE: If a disconnect has been actioned for the paired identity then issue
   // a "bye" after condemning this session and (unless still connecting) do the
   // same if the matching identity session is not found.
   if( send_okay_response && !socket_handler.get_is_for_support( ) )
   {
      string paired_identity( get_raw_session_variable(
       get_special_var_name( e_special_var_paired_identity ) ) );

      string backup_identity( get_raw_session_variable(
       get_special_var_name( e_special_var_blockchain_backup_identity ) ) );

      if( paired_identity.empty( ) )
      {
         bool is_disconnecting = !get_system_variable( '~' + identity ).empty( );

         if( is_disconnecting )
         {
            if( !is_condemned_session( ) )
            {
               condemn_this_session( );

               if( !is_captured_session( ) )
                  handler.set_finished( );

               socket.write_line( c_cmd_peer_session_bye, c_request_timeout, p_sock_progress );

               has_issued_bye = true;
            }
         }
      }
      else if( get_system_variable( paired_identity ).empty( ) )
      {
         bool is_only_session = false;
         bool is_missing_backup = false;

         if( socket_handler.get_is_time_for_check( ) )
         {
            is_only_session = ( num_have_session_variable( paired_identity, true ) < 2 );

            if( !is_only_session && !backup_identity.empty( ) 
             && ( num_have_session_variable( backup_identity, true ) < 2 ) )
               is_missing_backup = true;
         }

         bool is_disconnecting = !get_system_variable( '~' + identity ).empty( );

         if( !is_disconnecting )
            is_disconnecting = !get_system_variable( '~' + paired_identity ).empty( );

         bool had_connect_error = !get_system_variable( c_error_message_prefix + paired_identity ).empty( );

         if( is_only_session || is_missing_backup || is_disconnecting || had_connect_error )
         {
            set_session_variable( paired_identity, "" );

            if( !is_condemned_session( ) )
            {
               condemn_this_session( );

               if( is_only_session )
               {
                  TRACE_LOG( TRACE_SESSIONS,
                   "(ending session due to matching paired identity '"
                   + paired_identity + "' session not being found)" );
               }
               else if( is_missing_backup )
               {
                  TRACE_LOG( TRACE_SESSIONS,
                   "(ending session due to backup identity '"
                   + backup_identity + "' session not being found)" );
               }

               if( !is_captured_session( ) )
                  handler.set_finished( );

               socket.write_line( c_cmd_peer_session_bye, c_request_timeout, p_sock_progress );

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
            socket.write_line( c_cmd_peer_session_bye, c_request_timeout, p_sock_progress );

         handler.set_finished( );
      }

      if( !is_condemned_session( ) )
         condemn_this_session( );
   }
   else if( !socket_handler.get_is_responder( ) && !g_server_shutdown
    && ( socket_handler.op_state( ) == e_peer_state_waiting_for_get ) )
   {
      string response;

      if( socket.read_line( response, c_request_timeout, 0, p_sock_progress ) <= 0 )
      {
         string error;

         if( !socket.had_timeout( ) )
            error = "peer has terminated this connection";
         else
            error = "timeout occurred getting peer response";

         socket.close( );

         socket_handler.kill_session( );

         throw runtime_error( error );
      }

      if( response != string( c_response_okay ) )
      {
         socket.close( );

         socket_handler.kill_session( );

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
         set_session_variable( initiator_special_variable, c_true_value );
      else
         set_session_variable( responder_special_variable, c_true_value );
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

   bool has_issued_command = false;
   bool check_for_supporters = false;

   string blockchain_peer_has_supporters_name(
    get_special_var_name( e_special_var_blockchain_peer_has_supporters ) );

   string ip_addr( get_raw_session_variable( get_special_var_name( e_special_var_ip_addr ) ) );

   while( true )
   {
      progress* p_sock_progress = 0;
      trace_progress sock_progress( TRACE_SOCK_OPS );

      if( get_trace_flags( ) & TRACE_SOCK_OPS )
         p_sock_progress = &sock_progress;

      if( !check_for_supporters
       && !get_raw_session_variable( blockchain_peer_has_supporters_name ).empty( ) )
         check_for_supporters = true;

      if( !is_responder && !has_issued_command && !g_server_shutdown && !is_condemned_session( ) )
      {
         if( socket_handler.op_state( ) == e_peer_state_waiting_for_put )
         {
            string response;

            if( socket.read_line( response, c_request_timeout, 0, p_sock_progress ) <= 0 )
            {
               cmd_and_args = "bye";
               break;
            }

            string::size_type pos = response.find( c_response_message_prefix );

            if( pos == 0 )
            {
               response.erase( 0, strlen( c_response_message_prefix ) );

               if( response != "." )
                  set_session_progress_message( response );

               continue;
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

            has_issued_command = true;
         }
      }

      cmd_and_args.erase( );

      if( socket.read_line( cmd_and_args, c_request_timeout, c_max_line_length, p_sock_progress ) <= 0 )
      {
         if( !is_captured_session( )
          && ( is_condemned_session( ) || g_server_shutdown || !socket.had_timeout( ) ) )
         {
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

         if( cmd_and_args == c_response_okay || cmd_and_args == c_response_okay_more )
            cmd_and_args = "bye";

         if( !g_server_shutdown )
         {
            string::size_type pos = cmd_and_args.find( c_response_message_prefix );

            if( pos == 0 )
            {
               if( is_condemned_session( ) )
                  throw runtime_error( "peer session has been condemned" );

               cmd_and_args.erase( 0, strlen( c_response_message_prefix ) );

               if( cmd_and_args != "." )
                  set_session_progress_message( cmd_and_args );

               cmd_and_args.erase( );
               continue;
            }
         }

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
peer_session* construct_session(
 const date_time& dtm, bool is_responder, auto_ptr< ssl_socket >& ap_socket,
 const string& addr_info, bool is_for_support = false, peer_extra extra = e_peer_extra_none,
 const char* p_identity = 0, peerchain_type chain_type = e_peerchain_type_any,
 bool has_support_sessions = false, bool has_set_system_variable = false )
#else
peer_session* construct_session(
 const date_time& dtm, bool is_responder, auto_ptr< tcp_socket >& ap_socket,
 const string& addr_info, bool is_for_support = false, peer_extra extra = e_peer_extra_none,
 const char* p_identity = 0, peerchain_type chain_type = e_peerchain_type_any,
 bool has_support_sessions = false, bool has_set_system_variable = false )
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
      p_session = new peer_session( unix_time( dtm ),
       is_responder, ap_socket, addr_info, is_for_support, extra,
       p_identity, chain_type, has_support_sessions, has_set_system_variable );

   return p_session;
}

}

#ifdef SSL_SUPPORT
peer_session::peer_session( int64_t time_val, bool is_responder,
 auto_ptr< ssl_socket >& ap_socket, const string& addr_info,
 bool is_for_support, peer_extra extra, const char* p_identity,
 peerchain_type chain_type, bool has_support_sessions, bool has_set_system_variable )
#else
peer_session::peer_session( int64_t time_val, bool is_responder,
 auto_ptr< tcp_socket >& ap_socket, const string& addr_info,
 bool is_for_support, peer_extra extra, const char* p_identity,
 peerchain_type chain_type, bool has_support_sessions, bool has_set_system_variable )
#endif
 :
 is_hub( false ),
 is_user( false ),
 is_local( false ),
 is_owner( false ),
 ip_addr( addr_info ),
 time_val( time_val ),
 ap_socket( ap_socket ),
 is_responder( is_responder ),
 is_for_support( is_for_support ),
 other_is_owner( false ),
 both_are_owners( false ),
 needs_key_exchange( false ),
 has_support_sessions( has_support_sessions ),
 has_set_system_variable( has_set_system_variable )
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

   string unprefixed_blockchain( blockchain );
   replace( unprefixed_blockchain, c_bc_prefix, "" );

   if( port.empty( ) && blockchain.empty( ) )
      port = get_test_peer_port( );

   if( chain_type == e_peerchain_type_hub )
      is_hub = true;
   else if( chain_type == e_peerchain_type_user )
      is_user = true;

   if( ip_addr == c_local_ip_addr || ip_addr == c_local_ip_addr_for_ipv6 )
      is_local = true;

   if( !is_responder )
   {
      // NOTE: If a session "@secret_hash" variable exists then it will be
      // used (for "hub" and "user" connections) else a system variable is
      // used (which is set by the "peer_session_starter").
      secret_hash = get_raw_session_variable(
       get_special_var_name( e_special_var_secret_hash ) );

      if( secret_hash.empty( ) )
         secret_hash = get_raw_system_variable(
          get_special_var_name( e_special_var_secret_hash ) );

      if( !blockchain.empty( )
       && !list_file_tags( blockchain + string( ".*" ) + c_key_suffix ).empty( ) )
         is_owner = true;
   }

   progress* p_sock_progress = 0;
   trace_progress sock_progress( TRACE_SOCK_OPS );

#ifdef DEBUG_PEER_HANDSHAKE
   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_sock_progress = &sock_progress;
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
         pid += ':' + unprefixed_blockchain;

         pid += '#' + to_string( ( size_t )chain_type );

         if( !identity.empty( ) && ( extra != e_peer_extra_none ) )
         {
            pid += '@' + identity;

            if( !is_owner && ( chain_type == e_peerchain_type_backup ) )
            {
               string hub_identity( get_hub_identity( unprefixed_blockchain ) );

               if( !hub_identity.empty( ) )
                  pid += '&' + hub_identity;
            }
         }

#ifdef SSL_SUPPORT
         private_key priv_key;

         priv_key.get_secret( secret_key );
         public_loc = priv_key.get_public( );

         pid += '%' + public_loc;
#endif

         if( has_support_sessions )
            pid += '+';

         if( is_owner && !is_for_support )
            pid += '!';

         if( !secret_hash.empty( ) )
         {
            ostringstream osstr;
            osstr << hex << setw( 9 ) << setfill( '0' ) << unix_time( );

            string unix_in_hex( osstr.str( ) );

            sha256 hash( unix_in_hex + secret_hash );

            string::size_type pos = pid.find( ':' );

            string prefix( pid.substr( 0, pos ) );

            stringstream ss( pid.substr( pos + 1 ) );

            crypt_stream( ss, secret_hash, e_stream_cipher_chacha20 );

            pid = prefix + ':' + unix_in_hex + '-'
             + hash.get_digest_as_string( ).substr( 0, 9 ) + '=' + base64::encode( ss.str( ) );
         }
      }

      this->ap_socket->write_line( pid, c_request_timeout, p_sock_progress );
   }
   else
   {
      pid.erase( );

      this->ap_socket->read_line( pid, c_request_timeout, c_max_greeting_size, p_sock_progress );

      string::size_type pos = pid.find( '=' );

      if( pos != string::npos )
      {
         string encrypted_data( pid.substr( pos + 1 ) );

         pid.erase( pos );

         pos = pid.find( ':' );

         if( pos == string::npos )
            throw runtime_error( "invalid peer handshake" );
         else
         {
            string prefix( pid.substr( 0, pos + 1 ) );

            pid.erase( 0, pos + 1 );

            pos = pid.find( '-' );

            if( pos == string::npos )
               throw runtime_error( "invalid peer handshake" );
            else
            {
               string unix_in_hex( pid.substr( 0, pos ) );
               string hash_prefix( pid.substr( pos + 1 ) );

               string hash_secret_lines( get_raw_system_variable(
                get_special_var_name( e_special_var_secret_hash ) + "_*" ) );

               if( !hash_secret_lines.empty( ) )
               {
                  vector< string > all_lines;

                  split( hash_secret_lines, all_lines, '\n' );

                  for( size_t i = 0; i < all_lines.size( ); i++ )
                  {
                     string next_line( all_lines[ i ] );

                     pos = next_line.find( ' ' );

                     if( pos != string::npos )
                     {
                        string next_hash( next_line.substr( pos + 1 ) );

                        sha256 hash( unix_in_hex + next_hash );

                        if( hash_prefix == hash.get_digest_as_string( ).substr( 0, 9 ) )
                        {
                           secret_hash = next_hash;
                           break;
                        }
                     }
                  }

                  if( !secret_hash.empty( ) )
                  {
                     stringstream ss( base64::decode( encrypted_data ) );

                     crypt_stream( ss, secret_hash, e_stream_cipher_chacha20 );

                     pid = prefix + ss.str( );
                  }
               }
            }
         }

         if( secret_hash.empty( ) )
            throw runtime_error( "invalid peer handshake" );
      }

      pos = pid.find( c_key_exchange_suffix );

      if( pos != string::npos )
      {
         pid.erase( pos );
         needs_key_exchange = true;
      }

      pos = pid.find( '!' );

      if( pos != string::npos )
      {
         pid.erase( pos );
         other_is_owner = true;
      }

      pos = pid.find( '+' );

      if( pos != string::npos )
      {
         pid.erase( pos );
         this->has_support_sessions = true;
      }

      pos = pid.find( ':' );

      if( pos != string::npos )
      {
         set< string > blockchains;
         split( blockchain, blockchains );

         string peer_info( pid.substr( pos + 1 ) );

         if( peer_info.find( c_bc_prefix ) == 0 )
            blockchain = peer_info;
         else
            blockchain = string( c_bc_prefix ) + peer_info;

         string::size_type spos = blockchain.find( '%' );

         if( spos != string::npos )
         {
#ifdef SSL_SUPPORT
            private_key priv_key;

            priv_key.get_secret( secret_key );
            public_loc = priv_key.get_public( );

            public_ext = blockchain.substr( spos + 1 );

            if( !secret_hash.empty( ) )
            {
               public_key pub_key( public_ext );

               priv_key.construct_shared( session_secret, pub_key );

               sha256 hash( secret_hash + session_secret );

               session_secret = hash.get_digest_as_string( );
            }
#endif
            blockchain.erase( spos );
         }

         spos = blockchain.find( '&' );

         if( spos != string::npos )
         {
            hub_identity = blockchain.substr( spos + 1 );

            blockchain.erase( spos );
         }

         spos = blockchain.find( '@' );

         if( spos != string::npos )
         {
            identity = blockchain.substr( spos + 1 );

            blockchain.erase( spos );
         }

         spos = blockchain.find( '#' );

         if( spos != string::npos )
         {
            chain_type = ( peerchain_type )atoi( blockchain.substr( spos + 1 ).c_str( ) );

            blockchain.erase( spos );

            if( chain_type != e_peerchain_type_any )
            {
               string error;
               check_blockchain_type( blockchain, chain_type, &error );

               if( !error.empty( ) )
               {
                  this->ap_socket->write_line( string( c_response_error_prefix ) + error, c_request_timeout );

                  throw runtime_error( error );
               }

               if( chain_type == e_peerchain_type_hub )
                  is_hub = true;
               else if( chain_type == e_peerchain_type_user )
                  is_user = true;
            }
         }

         bool has_user = false;

         string unprefixed_blockchain( replaced( blockchain, c_bc_prefix, "" ) );

         if( is_user )
         {
            string forwards_tag( blockchain + c_zenith_suffix );

            string reversed( unprefixed_blockchain );
            reverse( reversed.begin( ), reversed.end( ) );

            string reversed_tag( c_bc_prefix + reversed + c_zenith_suffix );

            if( has_tag( forwards_tag ) || has_tag( reversed_tag )
             || has_file_archive( unprefixed_blockchain ) || has_file_archive( reversed ) )
               has_user = true;
         }

         if( !has_user && !blockchains.count( blockchain ) )
         {
            // FUTURE: This message should be handled as a server string message.
            string error( "Unsupported peerchain identity '" + unprefixed_blockchain + "'." );

            this->ap_socket->write_line( string( c_response_error_prefix ) + error, c_request_timeout );

            throw runtime_error( error );
         }

         pid.erase( pos );
      }

      if( !blockchain.empty( )
       && !list_file_tags( blockchain + string( ".*" ) + c_key_suffix ).empty( ) )
         is_owner = true;

      if( is_owner && other_is_owner )
         both_are_owners = true;

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

   string paired_identity;

   peer_state state = ( is_responder ? e_peer_state_responder : e_peer_state_initiator );

   int64_t current = unix_time( date_time::local( ) );

   int64_t difference = ( current - time_val ) + 2;

   int64_t time_value = current + ( difference * 2 );

   try
   {
      socket_command_handler cmd_handler( *ap_socket, state,
       time_value, is_local, is_owner, blockchain, is_for_support );

      cmd_handler.add_commands( 0,
       peer_session_command_functor_factory, ARRAY_PTR_AND_SIZE( peer_session_command_definitions ) );

      progress* p_sock_progress = 0;
      trace_progress sock_progress( TRACE_SOCK_OPS );

#ifdef DEBUG_PEER_HANDSHAKE
      if( get_trace_flags( ) & TRACE_SOCK_OPS )
         p_sock_progress = &sock_progress;
#endif

      string unprefixed_blockchain( blockchain );
      replace( unprefixed_blockchain, c_bc_prefix, "" );

      if( is_responder )
      {
         string extra( random_characters( 10, 5 ) );

         extra += '[' + format_bytes( get_files_area_item_max_size( ), false, 0, '\0' );

         peerchain_type chain_type = get_blockchain_type( blockchain );

         if( !is_owner && ( chain_type == e_peerchain_type_backup ) )
         {
            string hub_identity( get_hub_identity( unprefixed_blockchain ) );

            if( !hub_identity.empty( ) )
               extra += '&' + hub_identity;
         }

#ifdef SSL_SUPPORT
         if( !public_loc.empty( ) )
            extra += '%' + public_loc;
#endif

         if( is_owner )
            extra += '!';

         extra += ']' + random_characters( 100 - extra.size( ) );

         if( !secret_hash.empty( ) )
         {
            stringstream ss( extra );

            crypt_stream( ss, secret_hash, e_stream_cipher_chacha20 );

            extra = base64::encode( ss.str( ) );
         }

         ap_socket->write_line( string( c_protocol_version )
          + ':' + extra + '\n' + string( c_response_okay ), c_request_timeout, p_sock_progress );
      }
      else
      {
         string greeting;

         if( ap_socket->read_line( greeting, c_request_timeout, c_max_greeting_size, p_sock_progress ) <= 0 )
         {
            // FUTURE: This message should be handled as a server string message.
            string error;
            if( ap_socket->had_timeout( ) )
               error = "Timeout occurred trying to connect to peer.";
            else
               error = "Peer has unexpectedly terminated this connection.";

            if( !identity.empty( ) )
               set_system_variable( c_error_message_prefix + identity, error );

            if( !unprefixed_blockchain.empty( ) )
               set_system_variable( c_error_message_prefix + unprefixed_blockchain, error );

            ap_socket->close( );
            throw runtime_error( error );
         }

         if( greeting.find( c_response_error_prefix ) == 0 )
         {
            ap_socket->close( );

            greeting.erase( 0, strlen( c_response_error_prefix ) );

            if( !identity.empty( ) )
               set_system_variable( c_error_message_prefix + identity, greeting );

            if( !unprefixed_blockchain.empty( ) )
               set_system_variable( c_error_message_prefix + unprefixed_blockchain, greeting );

            throw runtime_error( greeting );
         }

         version_info ver_info;
         if( get_version_info( greeting, ver_info ) != string( c_response_okay ) )
         {
            ap_socket->close( );

            if( !identity.empty( ) )
               set_system_variable( c_error_message_prefix + identity, greeting );

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

            if( !identity.empty( ) )
               set_system_variable( c_error_message_prefix + identity, error );

            if( !unprefixed_blockchain.empty( ) )
               set_system_variable( c_error_message_prefix + unprefixed_blockchain, error );

            throw runtime_error( error );
         }

         if( !ver_info.extra.empty( ) )
         {
            if( !secret_hash.empty( ) )
            {
               stringstream ss( base64::decode( ver_info.extra ) );

               crypt_stream( ss, secret_hash, e_stream_cipher_chacha20 );

               ver_info.extra = ss.str( );
            }

            string::size_type pos = ver_info.extra.rfind( ']' );

            if( pos != string::npos )
               ver_info.extra.erase( pos );

            pos = ver_info.extra.find( '!' );

            if( pos != string::npos )
            {
               ver_info.extra.erase( pos );

               other_is_owner = true;

               if( is_owner )
                  both_are_owners = true;
            }

            pos = ver_info.extra.find( '%' );

            if( pos != string::npos )
            {
#ifdef SSL_SUPPORT
               public_ext = ver_info.extra.substr( pos + 1 );

               if( !secret_hash.empty( ) )
               {
                  public_key pub_key( public_ext );
                  private_key priv_key( secret_key );

                  priv_key.construct_shared( session_secret, pub_key );

                  sha256 hash( secret_hash + session_secret );

                  session_secret = hash.get_digest_as_string( );
               }
#endif
               ver_info.extra.erase( pos );
            }

            pos = ver_info.extra.find( '&' );

            if( pos != string::npos )
            {
               hub_identity = ver_info.extra.substr( pos + 1 );
               ver_info.extra.erase( pos );
            }

            pos = ver_info.extra.find( '[' );

            if( pos != string::npos )
               ver_info.extra.erase( 0, pos + 1 );

            if( unformat_bytes( ver_info.extra ) != get_files_area_item_max_size( ) )
            {
               ap_socket->close( );

               // FUTURE: This message should be handled as a server string message.
               string error( "Unexpected files area item size mismatch." );

               if( !identity.empty( ) )
                  set_system_variable( c_error_message_prefix + identity, error );

               if( !unprefixed_blockchain.empty( ) )
                  set_system_variable( c_error_message_prefix + unprefixed_blockchain, error );

               throw runtime_error( error );
            }
         }
      }

      init_session( cmd_handler, true, &ip_addr,
       &unprefixed_blockchain, from_string< int >( port ), is_for_support, false );

      bool has_session_secret = false;

      if( !session_secret.empty( ) )
      {
         has_session_secret = true;

         set_session_secret( session_secret );

         set_session_variable( get_special_var_name(
          e_special_var_peer_sec_hash ), sha256( session_secret ).get_digest_as_string( ).substr( 0, 12 ) );

         clear_key( session_secret );
      }

      // NOTE: This key exchange is the application protocol method (only supported for interactive testing).
      if( needs_key_exchange )
      {
         string slot_and_pubkey( get_raw_session_variable( get_special_var_name( e_special_var_slot ) ) );
         slot_and_pubkey += '-' + get_raw_session_variable( get_special_var_name( e_special_var_pubkey ) );

         string slotx, pubkeyx, slotx_and_pubkeyx;

         // NOTE: After handshake exchange public keys then commence peer protocol.
         if( is_responder )
         {
            ap_socket->write_line( slot_and_pubkey, c_request_timeout, p_sock_progress );

            ap_socket->read_line( slotx_and_pubkeyx, c_request_timeout, c_max_pubkey_size, p_sock_progress );
         }
         else
         {
            ap_socket->read_line( slotx_and_pubkeyx, c_request_timeout, c_max_pubkey_size, p_sock_progress );

            ap_socket->write_line( slot_and_pubkey, c_request_timeout, p_sock_progress );
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
      }

      if( !is_hub )
      {
         string archive_path, non_extra_identity( get_non_extra_identity( unprefixed_blockchain ) );

         if( !non_extra_identity.empty( ) && has_file_archive( non_extra_identity, &archive_path ) )
         {
            cmd_handler.init_for_archive( non_extra_identity );

            set_session_variable( get_special_var_name( e_special_var_blockchain_archive_path ), archive_path );

            if( non_extra_identity != unprefixed_blockchain )
               set_session_variable( get_special_var_name( e_special_var_blockchain_non_extra_identity ), non_extra_identity );
         }
      }
      else
      {
         set_session_variable( get_special_var_name( e_special_var_blockchain_is_hub ), c_true_value );

         if( num_have_session_variable( get_special_var_name(
          e_special_var_blockchain_peer_hub_identity ), unprefixed_blockchain, 0, true ) )
            set_session_variable( get_special_var_name( e_special_var_peer_is_dependent ), c_true_value );

         string progress_message(
          get_system_variable( c_progress_output_prefix + unprefixed_blockchain ) );

         set_session_progress_message( progress_message );
      }

      if( is_user )
         set_session_variable( get_special_var_name( e_special_var_blockchain_user ), c_true_value );

      if( is_owner )
         set_session_variable( get_special_var_name( e_special_var_blockchain_is_owner ), c_true_value );

      if( other_is_owner )
         set_session_variable( get_special_var_name( e_special_var_blockchain_other_is_owner ), c_true_value );

      if( is_for_support )
         set_session_variable( get_special_var_name( e_special_var_blockchain_peer_supporter ), c_true_value );

      if( both_are_owners )
         set_session_variable( get_special_var_name( e_special_var_blockchain_both_are_owners ), c_true_value );

      if( has_support_sessions )
         set_session_variable( get_special_var_name( e_special_var_blockchain_peer_has_supporters ), c_true_value );

      if( !hub_identity.empty( ) )
         set_session_variable( get_special_var_name( e_special_var_blockchain_peer_hub_identity ), hub_identity );

      if( !backup_identity.empty( ) )
         set_session_variable( get_special_var_name( e_special_var_blockchain_backup_identity ), backup_identity );

      if( !is_for_support )
      {
         if( !blockchain.empty( ) )
            set_session_variable( get_special_var_name( e_special_var_peer_map_key ), obtain_peer_map_key( ) );

         set_session_variable( get_special_var_name( e_special_var_blockchain_time_value ), to_string( time_value ) );
         set_session_variable( get_special_var_name( e_special_var_progress_seconds ), to_string( c_default_progress_seconds ) );
      }

      okay = true;
      was_initialised = true;

      bool has_zenith = false;
      size_t blockchain_height = 0;

      string hello_data, hello_hash;
      hello_data = get_hello_data( hello_hash );

      // NOTE: Create the dummy "hello" blob as it will be required.
      if( !has_file( hello_hash ) )
         create_raw_file( hello_data, false );

      if( !blockchain.empty( ) )
      {
         if( !is_for_support && has_tag( blockchain + c_zenith_suffix ) )
         {
            string zenith_hash( tag_file_hash( blockchain + c_zenith_suffix ) );

            if( get_block_height_from_tags( blockchain, zenith_hash, blockchain_height ) )
            {
               has_zenith = true;
               cmd_handler.set_blockchain_height( blockchain_height );

               string signature_tag( blockchain + '.' + to_string( blockchain_height ) + c_sig_suffix );

               // NOTE: If there is a signature at this height but no next block file then need
               // to remove the signature file so that synchronisation can be cleanly restarted.
               if( has_tag( signature_tag ) )
               {
                  string next_block_tag( blockchain + '.' + to_string( blockchain_height + 1 ) + c_blk_suffix );

                  if( !has_tag( next_block_tag ) )
                     delete_file( tag_file_hash( signature_tag ) );
               }

               set_session_variable( get_special_var_name(
                e_special_var_blockchain_zenith_height ), to_string( blockchain_height ) );
            }
         }

         if( !identity.empty( ) )
         {
            paired_identity = identity;

            set_session_variable( paired_identity, c_true_value );
            set_session_variable( get_special_var_name( e_special_var_paired_identity ), paired_identity );

            if( !is_for_support )
            {
               if( has_zenith )
                  output_sync_progress_message( unprefixed_blockchain, blockchain_height );
               else
               {
                  // FUTURE: This message should be handled as a server string message.
                  string progress_message( "Syncing at height 0" );

                  progress_message += c_ellipsis;

                  set_session_progress_message( progress_message );

                  system_identity_progress_message( unprefixed_blockchain );
               }
            }
         }

         // NOTE: Initially the identity value is that of the paired initiating session which
         // will not be the blockchain identity (for the paired session). In order to prevent
         // mistaken usage set it to be the actual blockchain identity value now.
         identity = unprefixed_blockchain;

         if( !is_for_support )
            set_session_variable( identity, c_true_value );
      }

      if( has_set_system_variable )
         set_system_variable( identity, "" );

      if( !is_responder )
      {
         string hash_or_tag;

         if( !is_for_support && !blockchain.empty( ) )
         {
            hash_or_tag = get_special_var_name( e_special_var_blockchain )
             + '.' + to_string( blockchain_height ) + string( c_blk_suffix );

            // NOTE: In case the responder does not have the genesis block include
            // its hash as a dummy "nonce" (to be used by the responder for "get").
            string genesis_block_tag( blockchain + ".0" + string( c_blk_suffix ) );

            if( has_tag( genesis_block_tag ) )
               hash_or_tag += ' ' + file_hash_for_write( *ap_socket, genesis_block_tag );
         }

         if( hash_or_tag.empty( ) )
            hash_or_tag = file_hash_for_write( *ap_socket, hello_hash );

         ap_socket->write_line( string( c_cmd_peer_session_chk ) + " " + hash_or_tag, c_request_timeout, p_sock_progress );

         cmd_handler.state( ) = e_peer_state_waiting_for_put;
         cmd_handler.op_state( ) = e_peer_state_waiting_for_put;

         if( !blockchain.empty( ) )
         {
            string block_hash;

            if( ap_socket->read_line( block_hash, c_request_timeout, c_max_line_length, p_sock_progress ) <= 0 )
               okay = false;
            else if( !is_for_support && ( block_hash != string( c_response_not_found ) ) )
               add_peer_file_hash_for_get( file_hash_for_read( *ap_socket, block_hash ) );
         }
      }

      // NOTE: If a hub identity has been provided then connect to it (unless already connected).
      if( okay && !is_for_support && !hub_identity.empty( ) )
      {
         string hub_genesis_hash( get_system_variable( "@" + identity ) );

         // NOTE: If a hub identity genesis block is already known then verify the identity.
         if( !hub_genesis_hash.empty( ) )
         {
            core_file_data core_data( extract_file( hub_genesis_hash, "" ) );

            if( hub_identity != core_data.get_header( c_file_type_core_block_header_identity ) )
            {
               // FUTURE: This message should be handled as a server string message.
               string error( "Incorrect hub identity '" + hub_identity + "' was provided." );

               set_system_variable( c_error_message_prefix + identity, error );

               throw runtime_error( error );
            }
         }

         if( !any_session_has_blockchain( hub_identity )
          && set_system_variable( hub_identity, c_true_value, string( "" ) ) )
         {
            bool dummy_is_listener = false;

            // NOTE: Read the "peerchain info" in order to determine which port
            // should be used to initiate a connection (the "dummy_is_listener"
            // is required to read host information).
            string peer_info( get_peerchain_info( identity, &dummy_is_listener ) );

            string::size_type pos = peer_info.rfind( ':' );

            bool created_initiator_for_hub = false;

            if( pos != string::npos )
            {
               peer_info.erase( pos );

               pos = peer_info.rfind( '-' );

               if( pos != string::npos )
               {
                  string host_and_port( ip_addr );

                  host_and_port += peer_info.substr( pos );

                  string hub_blockchain( c_bc_prefix + hub_identity );
                  string hub_zenith_tag( hub_blockchain + c_zenith_suffix );

                  if( has_tag( hub_zenith_tag ) )
                  {
                     string hub_zenith_hash( tag_file_hash( hub_zenith_tag ) );

                     size_t hub_height = 0;

                     if( get_block_height_from_tags( hub_blockchain, hub_zenith_hash, hub_height ) )
                     {
                        // FUTURE: This message should be handled as a server string message.
                        string progress_message( "Currently at height " );

                        progress_message += to_string( hub_height );

                        set_system_variable( c_progress_output_prefix + hub_identity, progress_message );
                     }
                  }

                  temporary_session_variable tmp_secret_hash(
                   get_special_var_name( e_special_var_secret_hash ), secret_hash );

                  create_peer_initiator( hub_blockchain, host_and_port,
                   false, 0, false, false, 0, e_peerchain_type_hub, true );

                  created_initiator_for_hub = true;
               }
            }

            if( !created_initiator_for_hub )
               set_system_variable( hub_identity, "" );
            else
               set_hub_system_variable_if_required( identity, hub_identity );

         }
      }

      if( okay )
      {
         TRACE_LOG( TRACE_SESSIONS,
          string( has_session_secret ? "started secure peer session " : "started *insecure* peer session " )
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
         has_terminated = terminate_peer_session( is_for_support, identity );
   }
   catch( exception& x )
   {
      issue_error( x.what( ), true );

      if( ap_socket->okay( ) )
      {
         ap_socket->write_line( string( c_response_error_prefix ) + x.what( ), c_request_timeout );
         ap_socket->close( );
      }

      if( was_initialised )
         has_terminated = terminate_peer_session( is_for_support, identity );
   }
   catch( ... )
   {
      issue_error( "unexpected unknown exception occurred" );

      if( ap_socket->okay( ) )
      {
         ap_socket->write_line( string( c_response_error_prefix ) + "unexpected exception occurred", c_request_timeout );
         ap_socket->close( );
      }

      if( was_initialised )
         has_terminated = terminate_peer_session( is_for_support, identity );
   }

   if( has_terminated && !is_for_support )
   {
      // NOTE: Wait before clearing the system variable (for the FCGI UI).
      msleep( c_start_sleep_time );

      if( paired_identity.empty( ) )
         set_system_variable( '~' + identity, "" );
      else
      {
         set_variable_checker check_no_other_session(
          e_variable_check_type_no_session_has, paired_identity );

         set_system_variable( '~' + paired_identity, "", check_no_other_session );
      }
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

            size_t num_iterations = 0;

            while( s && !g_server_shutdown )
            {
               string identity_changes( get_system_variable( '@' + to_string( port ) ) );

               // NOTE; Uses the system variable "@<port>" in order to insert or remove blockchains from the listener.
               if( !identity_changes.empty( ) )
               {
                  bool is_remove = false;

                  // NOTE: Only clear the system variable if the value matches.
                  set_system_variable( '@' + to_string( port ), "", identity_changes );

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
               else
               {
                  // NOTE: In case a disconnect system variable has
                  // ended up incorrectly set when no more sessions
                  // are currently active will clear it here (as it
                  // could well occur due to UI refresh behaviour).
                  if( ++num_iterations >= c_num_check_disconnected )
                  {
                     num_iterations = 0;

                     vector< string > all_identities;

                     split( unprefixed_blockchains, all_identities );

                     for( size_t i = 0; i < all_identities.size( ); i++ )
                     {
                        string next_identity( all_identities[ i ] );

                        if( !get_system_variable( '~' + next_identity ).empty( ) )
                        {
                           set_variable_checker check_no_other_session(
                            e_variable_check_type_no_session_has, next_identity );

                           set_system_variable( '~' + next_identity, "", check_no_other_session );
                        }
                     }
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
                     issue_error( x.what( ), true );
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
               string error( "Unable to start peerchain using port number " + to_string( port ) + "." );

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

mutex& get_mutex_for_peer_session( )
{
   return g_mutex;
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

string peer_channel_height( const string& identity, bool minimal, bool reversed, size_t minimum_height )
{
   string extra, retval;

   string channel_identity( get_system_variable( "$" + identity + "_" + c_identity_suffix ) );

   if( !channel_identity.empty( ) )
   {
      if( reversed )
         reverse( channel_identity.begin( ), channel_identity.end( ) );

      string channel_blockchain( c_bc_prefix + channel_identity );

      string channel_zenith_tag( channel_blockchain + c_zenith_suffix );

      if( !minimal )
      {
         extra += string( " " );
         retval = channel_identity;
      }

      if( has_tag( channel_zenith_tag ) )
      {
         size_t blockchain_height = 0;

         string channel_zenith_hash( tag_file_hash( channel_zenith_tag ) );

         if( get_block_height_from_tags( channel_blockchain, channel_zenith_hash, blockchain_height ) )
            retval += extra + to_string( max( minimum_height, blockchain_height ) );
      }
   }

   return retval;
}

void create_peer_listener( int port, const string& blockchains )
{
   // FUTURE: This function should probably not return until all blockchains
   // have be registered (which would require a waiting loop and detection).

   if( has_registered_listener( port ) )
      set_system_variable( '@' + to_string( port ), blockchains, "", 0, ',' );
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

peer_session* create_peer_initiator( const string& blockchain,
 const string& host_and_or_port, bool force, size_t num_for_support,
 bool is_interactive, bool is_secondary, peer_session* p_main_session,
 peerchain_type chain_type, bool has_set_system_variable, const string* p_extra_value )
{
   if( blockchain.empty( ) )
      throw runtime_error( "create_peer_initiator called with empty blockchain identity" );

   if( has_max_peers( ) )
      throw runtime_error( "server has reached its maximum peer limit" );

   bool is_paired = false;

   if( ( chain_type == e_peerchain_type_backup )
    || ( chain_type == e_peerchain_type_shared ) )
      is_paired = true;

   bool is_shared = false;

   if( chain_type == e_peerchain_type_shared )
      is_shared = true;

   bool will_have_support = false;

   if( num_for_support == c_dummy_num_for_support )
   {
      num_for_support = 0;
      will_have_support = true;
   }

   if( num_for_support > c_max_num_for_support )
      throw runtime_error( "cannot create " + to_string( num_for_support )
       + " sessions for support (max is " + to_string( c_max_num_for_support ) + ")" );

   int port = 0;
   string host( c_local_host );

   parse_host_and_or_port( host_and_or_port, host, port );

   if( !port )
      throw runtime_error( "invalid or missing port in '" + host_and_or_port + "' for create_peer_initiator" );

   size_t total_to_create = ( p_main_session ? 0 : 1 ) + num_for_support;

   string identity( blockchain );
   replace( identity, c_bc_prefix, "" );

   string::size_type pos = identity.find( ':' );

   string explicit_paired_identity;

   if( pos != string::npos )
   {
      explicit_paired_identity = identity.substr( pos + 1 );
      identity.erase( pos );

      replace( explicit_paired_identity, c_bc_prefix, "" );
   }

   if( !is_interactive && !p_main_session )
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

         return p_main_session;
      }
   }

   string own_identity( get_own_identity( is_shared, p_extra_value ) );

   bool has_separate_identity = false;

   if( is_paired && !is_secondary && !is_interactive
    && !own_identity.empty( ) && ( identity != own_identity ) )
      has_separate_identity = true;

   string session_blockchain( c_bc_prefix );

   if( !has_separate_identity )
      session_blockchain += identity;
   else
      session_blockchain += own_identity;

   date_time dtm( date_time::local( ) );

   auto_ptr< temporary_system_variable > ap_blockchain_connect;

   if( !has_set_system_variable )
      ap_blockchain_connect.reset( new temporary_system_variable( identity, c_true_value ) );

   for( size_t i = 0; i < total_to_create; i++ )
   {
      if( g_server_shutdown )
         break;

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

               break;
            }
         }

         if( ap_socket->connect( address, !p_main_session ? c_initial_timeout : c_support_timeout ) )
         {
            const char* p_identity = 0;
            peer_extra extra = e_peer_extra_none;

            if( !p_main_session && ( is_secondary || has_separate_identity ) )
            {
               p_identity = identity.c_str( );
               extra = ( !is_secondary ? e_peer_extra_primary : e_peer_extra_secondary );
            }
            else if( !explicit_paired_identity.empty( ) )
               p_identity = explicit_paired_identity.c_str( );

            bool has_support_sessions = false;

            if( !p_main_session && ( num_for_support || will_have_support ) )
               has_support_sessions = true;

            peer_session* p_session = construct_session( dtm, false, ap_socket,
             ip_addr + "=" + session_blockchain + ":" + to_string( port ), p_main_session,
             extra, p_identity, chain_type, has_support_sessions, has_set_system_variable );

            if( !p_session )
               break;
            else
            {
               if( !p_main_session )
                  p_main_session = p_session;

               p_session->start( );
            }
         }
         else if( !p_main_session )
         {
            string error;

            if( !ap_socket->had_timeout( ) )
               // FUTURE: This message should be handled as a server string message.
               error = "Failed trying to connect to '"
                + host + "' using port number " + to_string( port ) + ".";
            else
               // FUTURE: This message should be handled as a server string message.
               error = "Timed out trying to connect to '"
                + host + "' using port number " + to_string( port ) + ".";

            if( is_interactive )
               throw runtime_error( error );
            else
            {
               set_system_variable( c_error_message_prefix + identity, error );
               break;
            }
         }
      }
   }

   return p_main_session;
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

      msleep( c_start_sleep_time );

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

         string entries( get_system_variable( get_special_var_name( e_special_var_queue_peers ) ) );

         if( entries.empty( ) )
            msleep( c_wait_sleep_time );
         else
         {
            bool is_listener = false;
            vector< string > all_entries;

            // NOTE: If first entry is prefixed with '!' then will start all as listeners.
            if( entries[ 0 ] == '!' )
            {
               is_listener = true;
               entries.erase( 0, 1 );
            }

            split( entries, all_entries );

            for( size_t i = 0; i < all_entries.size( ); i++ )
            {
               string next_entry( all_entries[ i ] );

               string peer_info( next_entry );

               if( peer_info.find( '=' ) == string::npos )
                  peer_info = get_peerchain_info( next_entry, is_listener ? 0 : &is_listener );

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
      cout << "unexpected peer_session_starter exception" << endl;
#endif
      TRACE_LOG( TRACE_ANYTHING, "peer_session_starter error: unexpected unknown exception caught" );
   }

   delete this;
}

void peer_session_starter::start_peer_session( const string& peer_info )
{
   string info( peer_info );

   string::size_type pos = info.rfind( ':' );
   if( pos == string::npos )
      throw runtime_error( "unexpected peer_info '" + peer_info + "'" );

   int peer_type = from_string< int >( info.substr( pos + 1 ) );

   bool create_reversed = ( ( peer_type == -1 ) || ( peer_type == 0 ) );

   peerchain_type chain_type = e_peerchain_type_backup;
   peerchain_type reversed_chain_type = e_peerchain_type_shared;

   if( peer_type == -1 )
      chain_type = reversed_chain_type = e_peerchain_type_user;
   else if( peer_type == 3 )
      chain_type = e_peerchain_type_shared;

   info.erase( pos );

   pos = info.find( '=' );
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

   string extra_value;
   string* p_extra_value = 0;

   pos = blockchain.find( '-' );
   if( pos != string::npos )
   {
      extra_value = blockchain.substr( pos + 1 );
      p_extra_value = &extra_value;

      blockchain.erase( pos );
   }

   string identity( replaced( blockchain, c_bc_prefix, "" ) );

   string secret_hash_name( get_special_var_name( e_special_var_secret_hash ) );

   auto_ptr< temporary_system_variable > ap_temp_secret_hash;

   string secret_hash( get_raw_system_variable( secret_hash_name + '_' + identity ) );

   if( !secret_hash.empty( ) )
      ap_temp_secret_hash.reset( new temporary_system_variable( secret_hash_name, secret_hash ) );

   string paired_suffix;

   if( chain_type == e_peerchain_type_user )
      paired_suffix = ':' + identity;

   set_system_variable( c_error_message_prefix + identity, "" );

   temporary_system_variable tmp_blockchain_connect( identity, c_true_value );

   // NOTE: First create main sessions for both the local and hosted blockchains.
   peer_session* p_local_main = create_peer_initiator( blockchain + paired_suffix, info, false,
    ( !num_for_support ? 0 : c_dummy_num_for_support ), false, false, 0, chain_type, false, p_extra_value );

   if( p_local_main && ( peer_type >= 0 ) )
   {
      peer_session* p_hosted_main = create_peer_initiator( blockchain, info, false,
       ( !num_for_support ? 0 : c_dummy_num_for_support ), false, true, 0, chain_type, false, p_extra_value );

      if( p_hosted_main )
      {
         // NOTE: If the two main sessions were successfully created then
         // will now create the requested support sessions for them both.
         if( num_for_support )
         {
            create_peer_initiator( blockchain, info,
             false, num_for_support, false, false, p_local_main, chain_type, false, p_extra_value );

            create_peer_initiator( blockchain, info,
             false, num_for_support, false, true, p_hosted_main, chain_type, false, p_extra_value );
         }
      }
   }

   // NOTE: If peer type is user or combined then will also create reversed identity sessions.
   if( create_reversed )
   {
      string reversed( identity );
      reverse( reversed.begin( ), reversed.end( ) );

      string reversed_chain( c_bc_prefix + reversed );

      peer_session* p_local_shared = create_peer_initiator( reversed_chain + paired_suffix, info, false,
       ( !num_for_support ? 0 : c_dummy_num_for_support ), false, false, 0, reversed_chain_type, false, p_extra_value );

      if( p_local_shared && ( peer_type >= 0 ) )
      {
         p_local_shared->set_backup_identity( identity );

         peer_session* p_hosted_shared = create_peer_initiator( reversed_chain, info, false,
          ( !num_for_support ? 0 : c_dummy_num_for_support ), false, true, 0, reversed_chain_type, false, p_extra_value );

         if( p_hosted_shared )
         {
            p_hosted_shared->set_backup_identity( identity );

            if( num_for_support )
            {
               create_peer_initiator( reversed_chain, info, false,
                num_for_support, false, false, p_local_shared, reversed_chain_type, false, p_extra_value );

               create_peer_initiator( reversed_chain, info, false,
                num_for_support, false, true, p_hosted_shared, reversed_chain_type, false, p_extra_value );
            }
         }
      }
   }
}

void init_peer_sessions( int start_listeners )
{
   string all_tags( list_file_tags( c_bc_prefix + string( "*" ) + c_zenith_suffix ) );

   // NOTE: Set system variables for the current height blockchains with a zenith tag.
   if( !all_tags.empty( ) )
   {
      vector< string > tags;
      split( all_tags, tags, '\n' );

      for( size_t i = 0; i < tags.size( ); i++ )
      {
         string next_tag( tags[ i ] );

         string::size_type pos = next_tag.find( c_zenith_suffix );

         if( pos != string::npos )
         {
            string next_blockchain( next_tag.substr( 0, pos ) );

            string next_identity( replaced( next_blockchain, c_bc_prefix, "" ) );

            size_t blockchain_height = 0;

            string zenith_hash( tag_file_hash( next_tag ) );

            if( get_block_height_from_tags( next_blockchain, zenith_hash, blockchain_height ) )
               output_sync_progress_message( next_identity, blockchain_height );
         }
      }
   }

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
         string next_identity( i->second );

         bool add_reversed = false;

         string::size_type pos = next_identity.find( '!' );

         if( pos != string::npos )
         {
            add_reversed = true;
            next_identity.erase( pos );
         }

         set_system_variable( c_error_message_prefix + next_identity, "" );

         string next_blockchains( c_bc_prefix + next_identity );

         if( add_reversed )
         {
            string reversed( next_identity );
            reverse( reversed.begin( ), reversed.end( ) );

            reversed = c_bc_prefix + reversed;

            next_blockchains += ',' + reversed;
         }

         if( !port_blockchains.count( i->first ) )
            port_blockchains.insert( make_pair( i->first, next_blockchains ) );
         else
         {
            string all_blockchains( port_blockchains.find( i->first )->second + ',' + next_blockchains );

            port_blockchains[ i->first ] = all_blockchains;
         }
      }

      for( map< int, string >::iterator i = port_blockchains.begin( ); i != port_blockchains.end( ); ++i )
         create_peer_listener( i->first, i->second );
   }

   peer_session_starter* p_peer_session_start = new peer_session_starter;
   p_peer_session_start->start( );
}
