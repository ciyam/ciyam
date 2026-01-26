// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <ctime>
#  include <cassert>
#  include <climits>
#  include <map>
#  include <set>
#  include <stack>
#  include <vector>
#  include <fstream>
#  include <sstream>
#  include <iomanip>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#  ifdef __GNUG__
#     include <unistd.h>
#  endif
#endif

#include "ciyam_base.h"

#include "md5.h"
#include "ods.h"
#include "sio.h"
#include "cube.h"
#include "sha1.h"
#include "base64.h"
#include "config.h"
#include "format.h"
#include "sha256.h"
#include "sql_db.h"
#include "numeric.h"
#include "pointers.h"
#include "progress.h"
#include "date_time.h"
#include "utilities.h"
#include "char_array.h"
#include "class_base.h"
#ifdef SSL_SUPPORT
#  include "ssl_socket.h"
#  include "crypto_keys.h"
#endif
#include "file_utils.h"
#include "ciyam_files.h"
#include "fs_iterator.h"
#include "oid_pointer.h"
#include "crypt_stream.h"
#include "peer_session.h"
#include "ciyam_strings.h"
#include "ciyam_session.h"
#include "ciyam_base_ops.h"
#include "ciyam_channels.h"
#include "ciyam_variables.h"
#include "command_handler.h"
#include "dynamic_library.h"
#include "ods_file_system.h"
#include "ciyam_core_files.h"
#include "module_interface.h"
#include "module_management.h"
#include "read_write_stream.h"

using namespace std;

namespace
{

#include "ciyam_constants.h"

const size_t c_identity_burn = 100;

const size_t c_default_seconds = 1;

const size_t c_default_max_peers = 100;

// NOTE: This limit is supplied (along with the identity information) to
// client interfaces and is not the max # of concurrent server sessions.
const size_t c_default_max_user_limit = 1000;

const size_t c_default_cache_limit = 1000;

const size_t c_lock_file_sleep_time = 100;

const size_t c_max_lock_attempts = 20;
const size_t c_lock_attempt_sleep_time = 200;

const size_t c_log_cycle_seconds = 5;

const size_t c_log_num_excessive = 5000;

const size_t c_num_txs_for_reset = 250000;

const size_t c_storable_file_pad_len = 32;

const size_t c_num_extra_entropy_chars = 10;

const size_t c_unlock_key_length = 11;
const size_t c_unlock_ext_key_length = 17;

const size_t c_unlock_key_multiplier = 5;

const size_t c_unlock_file_name_start = 32;
const size_t c_unlock_file_name_length = 8;

const size_t c_sleep_after_script_time = 1500;

const size_t c_min_smtp_max_send_attempts = 1;

const size_t c_minimum_encrypted_password_size = 10;

const uint64_t c_unset_trace_flags = TRACE_WAITING;

// NOTE: Limit the buffer to twice the maximum file size (if a compression
// call returns buffer too small then the file can be stored uncompressed).
const int c_max_file_buffer_expansion = 2;

const int c_peerchain_type_hub = -2;
const int c_peerchain_type_user = -1;
const int c_peerchain_type_combined = 0;
const int c_peerchain_type_local_only = 1;
const int c_peerchain_type_backup_only = 2;
const int c_peerchain_type_shared_only = 3;

const char c_module_prefix_separator = '_';
const char c_module_order_prefix_separator = '.';

const char* const c_str_none = "(none)";
const char* const c_str_peer = "(peer)";

const char* const c_str_unknown = "unknown";

const char* const c_script_arg_opt = "opt";

const char* const c_dummy_host_name = "ciyam.peer";

const char* const c_at_init_script = "./at_init";
const char* const c_at_term_script = "./at_term";

const char* const c_server_log_file = "ciyam_server.log";
const char* const c_server_sid_file = "ciyam_server.sid";
const char* const c_server_config_file = "ciyam_server.sio";
const char* const c_server_tx_log_file = "ciyam_server.tlg";

const char* const c_server_demo_identities_list = "ciyam_demo_identities.lst";

const char* const c_default_trace_flags = "10000";

const char* const c_server_folder_backup = "backup";
const char* const c_server_folder_opened = "opened";
const char* const c_server_folder_shared = "shared";

const char* const c_server_command_mutexes = "mutexes";
const char* const c_server_command_sessions = "sessions";

const char* const c_section_mbox = "mbox";
const char* const c_section_pop3 = "pop3";
const char* const c_section_smtp = "smtp";
const char* const c_section_email = "email";
const char* const c_section_files = "files";
const char* const c_section_script = "script";

const char* const c_attribute_name = "name";
const char* const c_attribute_path = "path";
const char* const c_attribute_domain = "domain";
const char* const c_attribute_server = "server";
const char* const c_attribute_sender = "sender";
const char* const c_attribute_suffix = "suffix";
const char* const c_attribute_use_udp = "use_udp";
const char* const c_attribute_filename = "filename";
const char* const c_attribute_ip_addrs = "ip_addrs";
const char* const c_attribute_na_addrs = "na_addrs";
const char* const c_attribute_password = "password";
const char* const c_attribute_security = "security";
const char* const c_attribute_timezone = "timezone";
const char* const c_attribute_username = "username";
const char* const c_attribute_web_root = "web_root";
const char* const c_attribute_arguments = "arguments";
const char* const c_attribute_directory = "directory";
const char* const c_attribute_max_peers = "max_peers";
const char* const c_attribute_use_https = "use_https";
const char* const c_attribute_ntfy_server = "ntfy_server";
const char* const c_attribute_trace_flags = "trace_flags";
const char* const c_attribute_gpg_password = "gpg_password";
const char* const c_attribute_max_sessions = "max_sessions";
const char* const c_attribute_pem_password = "pem_password";
const char* const c_attribute_rpc_password = "rpc_password";
const char* const c_attribute_sql_password = "sql_password";
const char* const c_attribute_max_file_size = "max_file_size";
const char* const c_attribute_max_num_files = "max_num_files";
const char* const c_attribute_log_files_path = "log_files_path";
const char* const c_attribute_test_peer_port = "test_peer_port";
const char* const c_attribute_user_home_path = "user_home_path";
const char* const c_attribute_default_storage = "default_storage";
const char* const c_attribute_peer_ips_permit = "peer_ips_permit";
const char* const c_attribute_peer_ips_reject = "peer_ips_reject";
const char* const c_attribute_script_reconfig = "script_reconfig";
const char* const c_attribute_session_timeout = "session_timeout";
const char* const c_attribute_protocol_handler = "protocol_handler";
const char* const c_attribute_system_variables = "system_variables";
const char* const c_attribute_max_send_attempts = "max_send_attempts";
const char* const c_attribute_max_attached_data = "max_attached_data";
const char* const c_attribute_ods_use_encrypted = "ods_use_encrypted";
const char* const c_attribute_ods_use_sync_write = "ods_use_sync_write";
const char* const c_attribute_max_storage_handlers = "max_storage_handlers";
const char* const c_attribute_notifier_ignore_secs = "notifier_ignore_secs";
const char* const c_attribute_num_recv_stream_sessions = "num_recv_stream_sessions";
const char* const c_attribute_num_send_stream_sessions = "num_send_stream_sessions";

const char* const c_section_client = "client";
const char* const c_section_extern = "extern";

const char* const c_attribute_port = "port";
const char* const c_attribute_label = "label";
const char* const c_attribute_is_local = "is_local";
const char* const c_attribute_protocol = "protocol";
const char* const c_attribute_extra_info = "extra_info";
const char* const c_attribute_script_name = "script_name";

const char* const c_peerchain_attribute_auto_start = "auto_start";
const char* const c_peerchain_attribute_description = "description";
const char* const c_peerchain_attribute_extra_value = "extra_value";
const char* const c_peerchain_attribute_host_name = "host_name";
const char* const c_peerchain_attribute_host_port = "host_port";
const char* const c_peerchain_attribute_local_port = "local_port";
const char* const c_peerchain_attribute_num_helpers = "num_helpers";
const char* const c_peerchain_attribute_peer_type = "peer_type";
const char* const c_peerchain_attribute_shared_secret = "shared_secret";

const char* const c_default_web_root = "%root%/%store%";

const char* const c_expand_root = "%root%";
const char* const c_expand_store = "%store%";

const char* const c_uid_anon = "anon";
const char* const c_uid_unknown = "<unknown>";

const char* const c_script_dummy_filename = "*script*";

const char* const c_default_protocol_handler = "file://";

const char* const c_trace_level_minimal = "minimal";
const char* const c_trace_level_initial = "initial";
const char* const c_trace_level_details = "details";
const char* const c_trace_level_verbose = "verbose";

const char* const c_trace_flag_general = "general";
const char* const c_trace_flag_locking = "locking";
const char* const c_trace_flag_objects = "objects";
const char* const c_trace_flag_queries = "queries";
const char* const c_trace_flag_session = "session";
const char* const c_trace_flag_sockets = "sockets";
const char* const c_trace_flag_various = "various";

const char* const c_storable_file_name_id = "id";
const char* const c_storable_file_name_limit = "limit";
const char* const c_storable_file_name_log_id = "log_id";
const char* const c_storable_file_name_mod_dir = "mod_dir";
const char* const c_storable_file_name_num_gid = "num_gid";
const char* const c_storable_file_name_trunc_n = "trunc_n";
const char* const c_storable_file_name_version = "version";
const char* const c_storable_file_name_web_root = "web_root";

const char* const c_temporary_special_variable_suffix = "_temporary";

const char* const c_demo_sid_1 = "729c82d85a1da11074ea60a42ded01ece34928592aa8620a67c364d7fc70a3fb";
const char* const c_demo_sid_2 = "b7560eb88fdbc58bd76a6b1a33e07dc39f22e1b834cf470398a0ecf0fc16ca78";

string g_sid;

#include "sid.enc"

struct instance_info
{
   instance_info( class_base* p_class_base, dynamic_library* p_dynamic_library )
    :
    p_class_base( p_class_base ),
    p_dynamic_library( p_dynamic_library )
   {
   }

   class_base* p_class_base;
   dynamic_library* p_dynamic_library;
};

struct module_commands
{
   module_commands( size_t num_commands,
    command_definition* p_command_definitions, command_functor_creator* p_command_functor_creator )
    :
    num_commands( num_commands ),
    p_command_definitions( p_command_definitions ),
    p_command_functor_creator( p_command_functor_creator )
   {
   }

   size_t num_commands;
   command_definition* p_command_definitions;
   command_functor_creator* p_command_functor_creator;
};

typedef map< size_t, instance_info > object_instance_registry_container;
typedef object_instance_registry_container::iterator object_instance_registry_iterator;
typedef object_instance_registry_container::const_iterator object_instance_registry_const_iterator;
typedef object_instance_registry_container::value_type object_instance_registry_value_type;

typedef map< string, string > module_container;
typedef module_container::iterator module_iterator;
typedef module_container::const_iterator module_const_iterator;
typedef module_container::value_type module_value_type;

typedef map< string, module_commands > module_commands_registry_container;
typedef module_commands_registry_container::iterator module_commands_registry_iterator;
typedef module_commands_registry_container::const_iterator module_commands_registry_const_iterator;
typedef module_commands_registry_container::value_type module_commands_registry_value_type;

class storage_handler;

trace_mutex g_mutex;

mutex g_gid_mutex;
mutex g_uid_mutex;

mutex g_trace_mutex;
mutex g_mapping_mutex;
mutex g_session_mutex;

size_t g_key_count;
int64_t g_key_tm_val;

int64_t g_key_unlock_tm_val;

string g_identity_suffix;

bool g_secure_identity;
bool g_hardened_identity;
bool g_encrypted_identity;

string g_storage_name_lock;

set< string > g_read_only_var_names;

inline string empty_or_value( const string& empty, const string& value )
{
   return value.empty( ) ? empty : value;
}

struct global_storage_name_lock
{
   global_storage_name_lock( const string& name )
   {
      guard g( g_mutex );

      if( !g_storage_name_lock.empty( ) )
         throw runtime_error( "global storage name lock already in use" );

      g_storage_name_lock = name;
   }

   ~global_storage_name_lock( )
   {
      guard g( g_mutex );

      g_storage_name_lock.erase( );
   }
};

struct session
{
   session( size_t id, size_t slot, command_handler& cmd_handler,
    storage_handler* p_storage_handler, bool is_peer_session, const string* p_ip_addr,
    const string* p_blockchain, bool is_support_session, bool add_pubkey_variable )
    :
    id( id ),
    slot( slot ),
    sql_count( 0 ),
    sync_time( 0 ),
    cache_count( 0 ),
    next_handle( 0 ),
    p_tx_helper( 0 ),
    using_tls( false ),
    is_captured( false ),
    running_script( false ),
    skip_fk_fetches( false ),
    skip_validation( false ),
    peer_bytes_uploaded( 0 ),
    peer_files_uploaded( 0 ),
    buffer_is_locked( false ),
    peer_bytes_downloaded( 0 ),
    peer_files_downloaded( 0 ),
    cmd_handler( cmd_handler ),
    skip_is_constrained( false ),
    session_commands_executed( 0 ),
    is_peer_session( is_peer_session ),
    p_storage_handler( p_storage_handler ),
    is_support_session( is_support_session )
   {
      if( p_ip_addr )
      {
         ip_addr = *p_ip_addr;

         // NOTE: Always use the IPv4 version for compatibility.
         if( ip_addr == c_local_ip_addr_for_ipv6 )
            ip_addr = c_local_ip_addr;

         variables.insert( make_pair(
          get_special_var_name( e_special_var_ip_addr ), ip_addr ) );
      }

      if( p_blockchain )
         blockchain = *p_blockchain;

      identity_suffix = g_identity_suffix;

      last_cmd = string( c_str_none );

      dtm_created = dtm_last_cmd = date_time::local( );

      variables.insert( make_pair(
       get_special_var_name( e_special_var_session_id ), to_string( id ) ) );

      variables.insert( make_pair(
       get_special_var_name( e_special_var_slot ), 'S' + to_comparable_string( slot, false, 3 ) ) );

      variables.insert( make_pair( get_special_var_name( e_special_var_uuid ), uuid( ).as_string( ) ) );

      if( add_pubkey_variable )
      {
#ifndef SSL_SUPPORT
         variables.insert( make_pair( get_special_var_name( e_special_var_pubkey ), c_none ) );
#else
         variables.insert( make_pair( get_special_var_name( e_special_var_pubkey ), priv_key.get_public( ) ) );
#endif
      }
   }

   ~session( )
   {
      clear_key( secret );
   }

   size_t id;
   size_t slot;

   string dtm;
   string grp;
   string gid;
   string uid;
   string sec;

   string ip_addr;
   string tz_name;

   string last_cmd;

   string secret;

   string blockchain;

   string key_date_time;

   string identity_suffix;

   set< string > perms;

   string tmp_directory;

   size_t sql_count;
   size_t cache_count;
   size_t next_handle;

   int64_t sync_time;

   bool using_tls;
   bool is_captured;
   bool running_script;

   bool is_peer_session;
   bool is_support_session;

   bool skip_fk_fetches;
   bool skip_validation;
   bool skip_is_constrained;

   date_time dtm_created;
   date_time dtm_last_cmd;
   date_time dtm_last_recv;

   size_t peer_files_uploaded;
   int64_t peer_bytes_uploaded;

   size_t peer_files_downloaded;
   int64_t peer_bytes_downloaded;

   size_t session_commands_executed;

   module_container modules_by_id;
   module_container modules_by_name;

   unique_ptr< sql_db > up_db;

#ifdef SSL_SUPPORT
   private_key priv_key;
#endif

   bool buffer_is_locked;

   unique_ptr< unsigned char > up_buffer;

   unique_ptr< global_storage_name_lock > up_storage_name_lock;

   command_handler& cmd_handler;

   storage_handler* p_storage_handler;

   vector< string > storage_controlled_modules;

   string transaction_log_command;

   transaction_commit_helper* p_tx_helper;

   vector< string > peerchain_log_commands;

   string last_set_item;

   set< string > set_items;
   deque< set< string > > kept_set_items;

   string last_deque_item;

   deque< string > deque_items;
   deque< deque< string > > kept_deque_items;

   map< string, string > variables;

   map< string, deque< string > > deque_variables;
   map< string, map< string, string > > mapped_variables;

   deque< string > file_hashes_to_get;
   deque< string > file_hashes_to_put;

   unique_ptr< storage_ods_bulk_read > up_bulk_read;
   unique_ptr< storage_ods_bulk_write > up_bulk_write;

   set< string > tx_key_info;

   stack< ods::transaction* > transactions;

   vector< string > sql_undo_statements;

   string script_temp_args_file;

   vector< string > async_or_delayed_args_files;
   vector< string > async_or_delayed_system_commands;

   set< size_t > release_sessions;

   map< size_t, date_time > condemned_sessions;

   map< size_t, string > udp_recv_file_chunks;
   map< size_t, string > udp_send_file_chunks;

   module_commands_registry_container commands_registry;
   object_instance_registry_container instance_registry;
};

static TLS( session )* gtp_session;

struct op_lock
{
   enum lock_type
   {
      e_lock_type_none,
      e_lock_type_view,
      e_lock_type_link,
      e_lock_type_review,
      e_lock_type_obtain,
      e_lock_type_update,
      e_lock_type_create,
      e_lock_type_destroy,
      e_lock_type_dummy_value
   };

   op_lock( )
    :
    handle( 0 ),
    type( e_lock_type_none ),
    tx_type( e_lock_type_none ),
    transaction_id( 0 ),
    transaction_level( 0 ),
    p_session( 0 ),
    p_class_base( 0 ),
    p_root_class( 0 )
   {
   }

   op_lock( size_t handle, lock_type type, int64_t transaction_id,
    int64_t transaction_level, session* p_session, class_base* p_class_base, class_base* p_root_class )
    :
    handle( handle ),
    type( type ),
    transaction_id( transaction_id ),
    transaction_level( transaction_level ),
    p_session( p_session ),
    p_class_base( p_class_base ),
    p_root_class( p_root_class )
   {
      if( transaction_level )
         tx_type = type;
      else
         tx_type = e_lock_type_none;
   }

   static string lock_type_name( lock_type type );

   static lock_type lock_type_for_name( const string& name );

   size_t handle;

   lock_type type;
   lock_type tx_type;

   int64_t transaction_id;
   int64_t transaction_level;

   session* p_session;

   class_base* p_class_base;
   class_base* p_root_class;
};

bool operator ==( op_lock& lhs, op_lock& rhs )
{
   return lhs.type == rhs.type && lhs.tx_type == rhs.tx_type
    && lhs.transaction_id == rhs.transaction_id && lhs.transaction_level == rhs.transaction_level && lhs.p_session == rhs.p_session;
}

string op_lock::lock_type_name( lock_type type )
{
   string retval;

   if( type == e_lock_type_none )
      retval = "none";
   else if( type == e_lock_type_view )
      retval = "view";
   else if( type == e_lock_type_link )
      retval = "link";
   else if( type == e_lock_type_review )
      retval = "review";
   else if( type == e_lock_type_obtain )
      retval = "obtain";
   else if( type == e_lock_type_update )
      retval = "update";
   else if( type == e_lock_type_create )
      retval = "create";
   else if( type == e_lock_type_destroy )
      retval = "destroy";
   else
      retval = "**unknown**";

   return retval;
}

op_lock::lock_type op_lock::lock_type_for_name( const string& name )
{
   lock_type retval = e_lock_type_none;

   if( name == "view" )
      retval = e_lock_type_view;
   else if( name == "link" )
      retval = e_lock_type_link;
   else if( name == "review" )
      retval = e_lock_type_review;
   else if( name == "obtain" )
      retval = e_lock_type_obtain;
   else if( name == "update" )
      retval = e_lock_type_update;
   else if( name == "create" )
      retval = e_lock_type_create;
   else if( name == "destroy" )
      retval = e_lock_type_destroy;

   return retval;
}

const int c_num_lock_types = op_lock::e_lock_type_dummy_value - 1;

bool locks_can_coexist( op_lock::lock_type lhs, op_lock::lock_type rhs )
{
   struct
   {
      bool values[ c_num_lock_types ];
   }
   g_locks_can_coexist[ ] =
   {
   //   V  L  R  O  U  C  D
      { 1, 1, 1, 1, 1, 1, 1 }, // V
      { 1, 1, 1, 1, 1, 0, 0 }, // L
      { 1, 1, 1, 1, 0, 0, 0 }, // R
      { 1, 1, 1, 0, 0, 0, 0 }, // O
      { 1, 1, 0, 0, 0, 0, 0 }, // U
      { 1, 0, 0, 0, 0, 0, 0 }, // C
      { 1, 0, 0, 0, 0, 0, 0 }, // D
   };

   if( lhs == op_lock::e_lock_type_none || rhs == op_lock::e_lock_type_none )
      return true;

   return g_locks_can_coexist[ lhs - 1 ].values[ rhs - 1 ];
}

typedef multimap< string, op_lock > lock_container;
typedef lock_container::iterator lock_iterator;
typedef lock_container::const_iterator lock_const_iterator;
typedef lock_container::value_type lock_value_type;

typedef map< size_t, lock_iterator > lock_index_container;
typedef lock_index_container::iterator lock_index_iterator;
typedef lock_index_container::const_iterator lock_index_const_iterator;
typedef lock_index_container::value_type lock_index_value_type;

const int c_storage_format_version = 1;

// NOTE: Log identity values less than standard are reserved for system purposes.
struct log_identity
{
   log_identity( ) : next_id( c_tx_id_standard - 1 ), ceiling( 0 ) { }

   int32_t next_id;
   int32_t ceiling;
};

enum storage_type
{
   e_storage_type_standard,
   e_storage_type_peerchain,
};

struct storage_root
{
   storage_root( )
    :
    version( c_storage_format_version ),
    cache_limit( c_default_cache_limit ),
    identity( uuid( ).as_string( ) ),
    web_root( c_default_web_root ),
    type( e_storage_type_standard ),
    truncation_count( 0 ),
    group_identity_new( 0 ),
    group_identity_old( 0 )
   {
   }

   int32_t version;
   int32_t cache_limit;

   string identity;
   string web_root;

   storage_type type;

   string module_directory;

   int32_t truncation_count;

   vector< string > module_list;

   int32_t group_identity_new;
   int32_t group_identity_old;

   log_identity log_id;

   void store_as_text_files( ods_file_system& ofs );
   void fetch_from_text_files( ods_file_system& ods );
};

void storage_root::store_as_text_files( ods_file_system& ofs )
{
   string prefix( ( type == e_storage_type_peerchain ) ? "@" : "" );

   ofs.store_as_text_file( c_storable_file_name_id, prefix + identity );

   if( group_identity_old != group_identity_new )
   {
      group_identity_old = group_identity_new;

      ofs.store_as_text_file( c_storable_file_name_num_gid, group_identity_new );
   }

   ofs.store_as_text_file( c_storable_file_name_limit, cache_limit );
   ofs.store_as_text_file( c_storable_file_name_log_id, log_id.next_id );
   ofs.store_as_text_file( c_storable_file_name_mod_dir, module_directory, c_storable_file_pad_len );
   ofs.store_as_text_file( c_storable_file_name_trunc_n, truncation_count );
   ofs.store_as_text_file( c_storable_file_name_version, version );
   ofs.store_as_text_file( c_storable_file_name_web_root, web_root, c_storable_file_pad_len );
}

void storage_root::fetch_from_text_files( ods_file_system& ofs )
{
   ofs.fetch_from_text_file( c_storable_file_name_id, identity );

   if( identity.empty( ) || ( identity[ 0 ] != '@' ) )
      type = e_storage_type_standard;
   else
   {
      identity.erase( 0, 1 );
      type = e_storage_type_peerchain;
   }

   if( ofs.has_file( c_storable_file_name_num_gid ) )
   {
      ofs.fetch_from_text_file( c_storable_file_name_num_gid, group_identity_new );

      group_identity_old = group_identity_new;
   }

   ofs.fetch_from_text_file( c_storable_file_name_limit, cache_limit );
   ofs.fetch_from_text_file( c_storable_file_name_log_id, log_id.next_id );
   ofs.fetch_from_text_file( c_storable_file_name_mod_dir, module_directory, true );
   ofs.fetch_from_text_file( c_storable_file_name_trunc_n, truncation_count );
   ofs.fetch_from_text_file( c_storable_file_name_version, version );
   ofs.fetch_from_text_file( c_storable_file_name_web_root, web_root, true );

   // NOTE: Force an identity write to occur when the first transaction is logged.
   log_id.ceiling = log_id.next_id;

   if( version != c_storage_format_version )
      throw runtime_error( "found incorrect storage format version " + to_string( version ) );
}

typedef multimap< time_t, string > time_info_container;
typedef time_info_container::iterator time_info_iterator;
typedef time_info_container::const_iterator time_info_const_iterator;

class storage_handler
{
   public:
   storage_handler( size_t slot, const string& name, ods* p_ods = 0 )
    :
    slot( slot ),
    name( name ),
    p_ods( p_ods ),
    ref_count( 0 ),
    next_lock_handle( 0 ),
    bulk_lock_sess_id( 0 ),
    p_alternative_log_file( 0 ),
    is_locked_for_admin( false ),
    has_sql_undo_support( false )
   {
   }

   size_t get_slot( ) const { return slot; }
   const string& get_name( ) const { return name; }

   ods* get_ods( ) const { return p_ods; }

   void release_ods( ) { delete p_ods; p_ods = 0; }

   size_t get_ref_count( ) const { return ref_count; }

   size_t inc_ref_count( ) { return ++ref_count; }
   size_t dec_ref_count( ) { return --ref_count; }

   ofstream& get_log_file( ) { return log_file; }

   ofstream* get_alternative_log_file( ) { return p_alternative_log_file; }
   void set_alternative_log_file( ofstream* p_log_file ) { p_alternative_log_file = p_log_file; }

   storage_root& get_root( ) { return root; }
   const storage_root& get_root( ) const { return root; }

   bool supports_sql_undo( ) const { return has_sql_undo_support; }

   bool is_special_storage_name( ) { return ( name == c_meta_storage_name ) || ( name == c_ciyam_storage_name ); }

   bool is_using_verbose_logging( ) const { return ( name == c_meta_storage_name ); }

   void set_bulk_lock_sess_id( );

   size_t get_bulk_lock_sess_id( ) const { return bulk_lock_sess_id; }

   date_time get_bulk_lock_date_time( ) const { return dtm_bulk_locked; }

   bool get_is_locked_for_admin( ) const { return is_locked_for_admin; }
   void set_is_locked_for_admin( bool lock_for_admin = true ) { is_locked_for_admin = lock_for_admin; }

   string get_variable( const string& var_name );
   void set_variable( const string& var_name, const string& new_value );

   void dump_cache( ostream& os ) const;
   void dump_locks( ostream& os ) const;

   bool obtain_lock( size_t& handle,
    const string& lock_class, const string& lock_instance,
    op_lock::lock_type type, session* p_session, class_base* p_class_base = 0,
    class_base* p_root_class_base = 0, size_t num_attempts = 0, size_t check_handle = 0 );

   void transform_lock(
    size_t handle, op_lock::lock_type new_type,
    session* p_session = 0, size_t num_attempts = 0 );

   void release_lock( size_t handle, bool force_removal = false );

   bool has_lock_info( size_t handle ) const;
   op_lock get_lock_info( size_t handle ) const;

   op_lock get_lock_info( const string& lock_class, const string& lock_instance ) const;

   op_lock get_lock_info_for_owner( const string& lock_class, const string& lock_instance, class_base& owner ) const;

   void update_session_locks_for_transaction( session* p_session );

   void release_locks_for_owner( class_base& owner, bool force_removal = false );

   void release_locks_for_commit( session* p_session );
   void release_locks_for_rollback( session* p_session );

   void release_all_locks_for_session( session* p_session );

   set< string >& get_dead_keys( ) { return dead_keys; }

   time_info_container& get_key_for_time( ) { return key_for_time; }
   map< string, time_t >& get_time_for_key( ) { return time_for_key; }

   void clear_cache( );
   void set_cache_limit( size_t new_limit );

   map< string, vector< string > >& get_record_cache( ) { return record_cache; }

   private:
   size_t slot;
   string name;

   ods* p_ods;
   size_t ref_count;

   ofstream log_file;
   ofstream* p_alternative_log_file;

   storage_root root;

   size_t next_lock_handle;

   size_t bulk_lock_sess_id;

   date_time dtm_bulk_locked;

   bool is_locked_for_admin;
   bool has_sql_undo_support;

   mutable mutex lock_mutex;
   mutable mutex cache_mutex;

   mutable mutex variable_mutex;

   lock_container locks;
   lock_index_container lock_index;

   set< size_t > lock_duplicates;

   set< string > dead_keys;

   map< string, string > variables;

   time_info_container key_for_time;
   map< string, time_t > time_for_key;

   map< string, vector< string > > record_cache;

   map< string, pair< size_t, string > > user_level_and_gids;

   storage_handler( const storage_handler& );
   storage_handler& operator ==( const storage_handler& );
};

void storage_handler::set_bulk_lock_sess_id( )
{
   if( gtp_session )
      bulk_lock_sess_id = gtp_session->id;

   dtm_bulk_locked = date_time::local( );
}

string storage_handler::get_variable( const string& var_name )
{
   guard g( variable_mutex );

   string retval;

   string name( var_name );

   bool persist = false;
   bool restore = false;

   if( !name.empty( ) )
   {
      if( name[ 0 ] == c_persist_variable_prefix )
         persist = true;
      else if( name[ 0 ] == c_restore_variable_prefix )
         restore = true;

      if( persist || restore )
         name.erase( 0, 1 );
   }

   // NOTE: If either the persist or restore prefix was provided
   // with nothing else then will either output all of the names
   // and values of persistent variables (if was persist prefix)
   // or all that would be restored (if was the restore prefix).
   // If a name or wildcard expression was provided then applies
   // only to current matching persistent variables. If the name
   // does not contain any wildcards and was persist prefixed if
   // the variable exists (in memory) then it will be persisted.
   if( persist || restore )
   {
      if( !gtp_session )
         throw runtime_error( "unexpected missing session in storage_handler::get_variable" );

      if( !ods::instance( ) )
         throw runtime_error( "no ODS DB exists for this session in storage_handler::get_variable" );

      if( persist )
      {
         if( gtp_session->up_bulk_read.get( ) )
            throw runtime_error( "storage is bulk locked for read by this session" );

         unique_ptr< storage_ods_bulk_write > up_bulk_write;

         if( !gtp_session->up_bulk_write.get( ) )
            up_bulk_write.reset( new storage_ods_bulk_write( ) );

         ods_file_system ofs( *ods::instance( ) );

         if( ofs.has_folder( c_storage_folder_name_variables ) )
         {
            ofs.set_folder( c_storage_folder_name_variables );

            string expr;

            if( name.empty( ) || ( name == "*" ) )
               expr += "*";
            else
               expr = name;

            vector< string > variable_files;

            ofs.list_files( expr, variable_files );

            if( variable_files.empty( ) && ( name.find_first_of( "?*" ) == string::npos ) )
            {
               if( variables.count( name ) )
               {
                  retval = variables[ name ];

                  ofs.store_as_text_file( name, retval );
               }
            }
            else
            {
               for( size_t i = 0; i < variable_files.size( ); i++ )
               {
                  string next( variable_files[ i ] );

                  string value;

                  ofs.fetch_from_text_file( next, value );

                  bool output = false;

                  if( name.empty( ) )
                     output = true;
                  else
                  {
                     if( !variables.count( next ) )
                        ofs.remove_file( next );
                     else
                     {
                        if( value != variables[ next ] )
                        {
                           output = true;
                           value = variables[ next ];

                           ofs.store_as_text_file( next, value );
                        }
                     }
                  }

                  if( output )
                  {
                     if( !retval.empty( ) )
                        retval += '\n';

                     retval += next + ' ' + value;
                  }
               }
            }
         }
      }
      else
      {
         unique_ptr< storage_ods_bulk_read > up_bulk_read;

         if( !gtp_session->up_bulk_read.get( )
          && !gtp_session->up_bulk_write.get( ) )
            up_bulk_read.reset( new storage_ods_bulk_read( ) );

         ods_file_system ofs( *ods::instance( ) );

         if( ofs.has_folder( c_storage_folder_name_variables ) )
         {
            ofs.set_folder( c_storage_folder_name_variables );

            string expr;

            if( name.empty( ) || ( name == "*" ) )
               expr += "*";
            else
               expr = name;

            vector< string > variable_files;

            ofs.list_files( expr, variable_files );

            for( size_t i = 0; i < variable_files.size( ); i++ )
            {
               string next( variable_files[ i ] );

               string value, current;

               bool output = false;

               ofs.fetch_from_text_file( next, value );

               if( !name.empty( ) )
               {
                  if( !variables.count( next )
                   || ( variables[ next ] != value ) )
                  {
                     output = true;
                     variables[ next ] = value;
                  }
               }
               else
               {
                  if( variables.count( next ) )
                     current = variables[ next ];

                  if( value != current )
                     output = true;
               }

               if( output )
               {
                  if( !retval.empty( ) )
                     retval += '\n';

                  retval += next + ' ' + value;
               }
            }
         }
      }
   }

   if( !persist && !restore )
   {
      if( name.find_first_of( "?*" ) == string::npos )
      {
         if( variables.count( name ) )
            retval = variables[ name ];
      }
      else
      {
         map< string, string >::const_iterator ci;

         for( ci = variables.begin( ); ci != variables.end( ); ++ci )
         {
            if( wildcard_match( name, ci->first ) )
            {
               if( !retval.empty( ) )
                  retval += '\n';

               retval += ci->first + ' ' + ci->second;
            }
         }
      }
   }

   return retval;
}

void storage_handler::set_variable( const string& var_name, const string& new_value )
{
   guard g( variable_mutex );

   string name( var_name );

   bool persist = false;

   if( !name.empty( ) && ( name[ 0 ] == c_persist_variable_prefix ) )
   {
      persist = true;
      name.erase( 0, 1 );
   }

   if( name.find_first_of( c_invalid_name_chars ) != string::npos )
      throw runtime_error( "invalid storage variable name '" + name + "'" );

   if( !name.empty( ) )
   {
      if( !new_value.empty( ) )
         variables[ name ] = new_value;
      else
      {
         if( variables.count( name ) )
            variables.erase( name );
      }

      if( persist )
      {
         if( !gtp_session )
            throw runtime_error( "unexpected missing session in storage_handler::set_variable" );

          if( !ods::instance( ) )
            throw runtime_error( "no ODS DB exists for this session in storage_handler::set_variable" );

        if( gtp_session->up_bulk_read.get( ) )
            throw runtime_error( "storage is bulk locked for read by this session" );

         unique_ptr< storage_ods_bulk_write > up_bulk_write;

         if( !gtp_session->up_bulk_write.get( ) )
            up_bulk_write.reset( new storage_ods_bulk_write( ) );

         ods_file_system ofs( *ods::instance( ) );

         if( !new_value.empty( )
          && !ofs.has_folder( c_storage_folder_name_variables ) )
            ofs.add_folder( c_storage_folder_name_variables );

         if( ofs.has_folder( c_storage_folder_name_variables ) )
         {
            ofs.set_folder( c_storage_folder_name_variables );

            if( new_value.empty( ) )
            {
               if( ofs.has_file( name ) )
                  ofs.remove_file( name );
            }
            else
               ofs.store_as_text_file( name, new_value );
         }
      }
   }
}

void storage_handler::dump_cache( ostream& os ) const
{
   guard g( cache_mutex );

   os << "date_time_accessed  key (class_id:instance)                                          ver.rev\n";
   os << "------------------- ---------------------------------------------------------------- -------";

   for( time_info_const_iterator ci = key_for_time.begin( ); ci != key_for_time.end( ); ++ci )
   {
      os << '\n';

      time_t t = ci->first;
      struct tm* p_t = localtime( &t );

      date_time dt( p_t->tm_year + 1900, ( month )( p_t->tm_mon + 1 ),
       p_t->tm_mday, p_t->tm_hour, p_t->tm_min, ( second )p_t->tm_sec );

      os.setf( ios::left );

      os << dt.as_string( e_time_format_hhmmss, true ) << ' ' << setw( 64 ) << ci->second << ' ';

      const vector< string >& columns( record_cache.find( ci->second )->second );

      if( columns.size( ) > 2 )
         os << columns[ 1 ] << '.' << columns[ 2 ];
   }
}

void storage_handler::dump_locks( ostream& os ) const
{
   guard g( lock_mutex );

   lock_index_const_iterator lici;

   bool minimal_output = !get_raw_session_variable(
    get_special_var_name( e_special_var_dump_minimal ) ).empty( );

   if( minimal_output )
   {
      os << "handle   key (class_id:instance)                                          type\n";
      os << "-------- ---------------------------------------------------------------- ----------";
   }
   else
   {
      os << "handle   key (class_id:instance)                                          type       ";
      os << "tx_type    tran_id    tran_level p_session      p_class_base   p_root_class\n";

      os << "-------- ---------------------------------------------------------------- ---------- ";
      os << "---------- ---------- ---------- -------------- -------------- --------------";
   }

   for( lici = lock_index.begin( ); lici != lock_index.end( ); ++lici )
   {
      os << '\n';

      op_lock& next_lock( lici->second->second );

      os.setf( ios::left );

      os << setw( 8 ) << lici->first
       << ' ' << setw( 64 ) << lici->second->first
       << ' ' << setw( 10 ) << op_lock::lock_type_name( next_lock.type );

      if( minimal_output )
         continue;

      os << ' ' << setw( 10 ) << op_lock::lock_type_name( next_lock.tx_type )
       << ' ' << setw( 10 ) << next_lock.transaction_id << ' ' << setw( 10 ) << next_lock.transaction_level
       << ' ' << setw( 14 ) << next_lock.p_session << ' ' << setw( 14 ) << next_lock.p_class_base << ' ' << next_lock.p_root_class;
   }
}

bool storage_handler::obtain_lock( size_t& handle,
 const string& lock_class, const string& lock_instance, op_lock::lock_type type,
 session* p_session, class_base* p_class_base, class_base* p_root_class, size_t num_attempts, size_t check_handle )
{
   int attempts = c_max_lock_attempts;

   if( num_attempts )
      attempts = num_attempts;

   TRACE_LOG( TRACE_DETAILS | TRACE_LOCKING, "[obtain lock] class = " + lock_class
    + ", instance = " + lock_instance + ", type = " + to_string( type ) + " (" + op_lock::lock_type_name( type ) + ")"
    + ", p_session = " + to_string( p_session ) + ", p_class_base = " + to_string( p_class_base ) + ", p_root_class = " + to_string( p_root_class ) );

   bool found = false;

   while( attempts-- )
   {
      // NOTE: Empty scope for guard.
      {
         guard g( lock_mutex );

         ods* p_ods( ods::instance( ) );

         string key( lock_class );
         key += ':';
         key += lock_instance;

         lock_iterator li( locks.lower_bound( lock_class ) );

         // NOTE: Check existing locks of the same class for a conflicting lock
         // (and an empty lock instance is being treated as a class-wide lock).
         op_lock last_lock;

         bool lock_conflict = false;

         while( li != locks.end( ) )
         {
            string next_lock_class, next_lock_instance;

            string::size_type pos = li->first.find( ':' );

            if( pos == string::npos )
               throw runtime_error( "lock key '" + li->first + "' is missing class/instance separator" );

            next_lock_class = li->first.substr( 0, pos );
            next_lock_instance = li->first.substr( pos + 1 );

            if( lock_class != next_lock_class )
               break;

            if( !lock_instance.empty( )
             && !next_lock_instance.empty( ) && ( lock_instance != next_lock_instance ) )
            {
               // NOTE: If both locks being compared are instance locks then if greater finish or if less then
               // skip to the first instance that is equal or greater (as only equal instance locks can clash).
               if( next_lock_instance > lock_instance )
                  break;
               else
               {
                  li = locks.lower_bound( key );
                  continue;
               }
            }

            op_lock& next_lock( li->second );

            if( check_handle && ( next_lock.handle == check_handle ) )
            {
               ++li;
               continue;
            }

            // NOTE: Locks that are effectively "dead" (i.e. awaiting cleanup when the tx completes) can
            // result in increasingly poorer performance if duplicates are allowed to exist (due to lock
            // clash scanning) therefore remove any duplicates as they are discovered.
            if( ( next_lock.type == op_lock::e_lock_type_none ) && ( next_lock == last_lock ) )
            {
               lock_duplicates.insert( next_lock.handle );

               lock_index.erase( next_lock.handle );
               locks.erase( li++ );

               continue;
            }

            // NOTE: No lock conflicts can be permitted to occur during a storage restore (but due to the way
            // that DB txs are performed during a restore they could occur without this check being present).
            if( storage_locked_for_admin( ) )
               break;

            bool is_review = ( type == op_lock::e_lock_type_review ) || ( next_lock.type == op_lock::e_lock_type_review );

            bool can_coexist = locks_can_coexist( type, next_lock.type );

            bool same_session = ( p_session == next_lock.p_session );

            bool same_class_base = ( p_class_base == next_lock.p_class_base );
            bool same_root_class = ( p_root_class == next_lock.p_root_class );

            // NOTE: Cascade locks will be ignored within the same session as it can sometimes be necessary for
            // update or delete operations to occur on an already cascade locked instance. Locks that are being
            // held for the duration of a transaction are only being checked here if it's not the same session.
            if( ( lock_instance.empty( )
             || next_lock_instance.empty( ) || ( lock_instance == next_lock_instance ) )
             && ( ( !can_coexist && ( !same_session || !p_root_class || !same_root_class ) )
             || ( !same_session && next_lock.tx_type && !locks_can_coexist( type, next_lock.tx_type ) ) ) )
            {
               // NOTE: Will allow "review" locks for separate instances that are owned by the same session.
               // Also allows "update" locks to be obtained for an already "update" locked instance provided
               // that the instances are separate but owned by the same session and that the existing locked
               // instance is in the "after_store" trigger.
               bool ignore_conflict = false;

               if( same_session && is_review && !same_class_base )
                  ignore_conflict = true;
               else if( same_session
                && ( type == next_lock.type ) && !same_class_base && next_lock.p_class_base
                && ( type == op_lock::e_lock_type_update ) && next_lock.p_class_base->get_is_after_store( ) )
                  ignore_conflict = true;

               if( !ignore_conflict )
               {
                  lock_conflict = true;

                  if( !attempts )
                     TRACE_LOG( TRACE_DETAILS | TRACE_LOCKING, "*** failed to acquire lock due to lock #" + to_string( next_lock.handle ) + " ***" );

                  break;
               }
            }

            last_lock = next_lock;

            ++li;
         }

         if( !lock_conflict )
         {
            if( check_handle )
            {
               found = true;
               break;
            }

            int64_t tran_id( p_ods->get_transaction_id( ) );
            int64_t tran_level( p_ods->get_transaction_level( ) );

            li = locks.insert( lock_value_type( key,
             op_lock( ++next_lock_handle, type, tran_id, tran_level, p_session, p_class_base, p_root_class ) ) );

            lock_index.insert( lock_index_value_type( next_lock_handle, li ) );

            handle = next_lock_handle;
            found = true;

            break;
         }
      }

      if( attempts )
         msleep( c_lock_attempt_sleep_time );
   }

   IF_IS_TRACING( TRACE_DETAILS | TRACE_LOCKING )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_DETAILS | TRACE_LOCKING, "[dumping locks]\n" + osstr.str( ) );
   }

   return found;
}

void storage_handler::transform_lock( size_t handle,
 op_lock::lock_type new_type, session* p_session, size_t num_attempts )
{
   guard g( lock_mutex );

   TRACE_LOG( TRACE_DETAILS | TRACE_LOCKING, "[transform lock] handle = "
    + to_string( handle ) + ", new_type = " + to_string( new_type ) );

   if( handle && ( lock_duplicates.find( handle ) == lock_duplicates.end( ) ) )
   {
      lock_index_iterator lii = lock_index.find( handle );

      if( lii != lock_index.end( ) )
      {
         lock_iterator li( lii->second );

         if( new_type > li->second.type )
         {
            string key( li->first );

            string::size_type pos = key.find( ':' );

            if( pos == string::npos )
               throw runtime_error( "lock key '" + key + "' is missing class/instance separator" );

            string lock_class( key.substr( 0, pos ) );
            string lock_instance( key.substr( pos + 1 ) );

            if( !obtain_lock( handle,
             lock_class, lock_instance, new_type, p_session, 0, 0, num_attempts, handle ) )
               throw runtime_error( "unable to transform lock due to locking conflict" );
         }

         li->second.type = new_type;
      }
   }

   IF_IS_TRACING( TRACE_DETAILS | TRACE_LOCKING )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_DETAILS | TRACE_LOCKING, "[dump_locks]\n" + osstr.str( ) );
   }
}

void storage_handler::release_lock( size_t handle, bool force_removal )
{
   guard g( lock_mutex );

   TRACE_LOG( TRACE_DETAILS | TRACE_LOCKING, "[release lock] handle = "
    + to_string( handle ) + ", force_removal = " + to_string( force_removal ) );

   if( lock_duplicates.find( handle ) != lock_duplicates.end( ) )
      lock_duplicates.erase( handle );
   else
   {
      lock_index_iterator lii = lock_index.find( handle );

      if( lii != lock_index.end( ) )
      {
         lock_iterator li( lii->second );

         if( !force_removal && ( li->second.transaction_level > 0 ) )
            li->second.type = op_lock::e_lock_type_none;
         else
         {
            lock_index.erase( lii );
            locks.erase( li );
         }
      }
   }

   IF_IS_TRACING( TRACE_DETAILS | TRACE_LOCKING )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_DETAILS | TRACE_LOCKING, "[dump_locks]\n" + osstr.str( ) );
   }
}

bool storage_handler::has_lock_info( size_t handle ) const
{
   guard g( lock_mutex );

   lock_index_const_iterator lici = lock_index.find( handle );

   if( lici == lock_index.end( ) )
      return false;

   return true;
}

op_lock storage_handler::get_lock_info( size_t handle ) const
{
   guard g( lock_mutex );

   lock_index_const_iterator lici = lock_index.find( handle );

   if( lici == lock_index.end( ) )
      throw runtime_error( "unable to locate lock handle #" + to_string( handle ) );

   return ( lici->second )->second;
}

op_lock storage_handler::get_lock_info( const string& lock_class, const string& lock_instance ) const
{
   guard g( lock_mutex );

   string key( lock_class );
   key += ':';
   key += lock_instance;

   op_lock lock;

   for( lock_const_iterator lci = locks.lower_bound( key ), end = locks.end( ); lci != end; ++lci )
   {
      if( lci->first != key )
         break;

      // NOTE: If more than one matching lock exists then return the strongest lock type
      // (including any remaining "tx_type" locks which might still need to be checked).
      if( ( lci->second.type > lock.type )
       || ( ( lock.type == op_lock::e_lock_type_none ) && ( lci->second.tx_type > lock.tx_type ) ) )
         lock = lci->second;
   }

   return lock;
}

op_lock storage_handler::get_lock_info_for_owner( const string& lock_class, const string& lock_instance, class_base& owner ) const
{
   guard g( lock_mutex );

   string key( lock_class );
   key += ':';
   key += lock_instance;

   op_lock lock;

   for( lock_const_iterator lci = locks.lower_bound( key ), end = locks.end( ); lci != end; ++lci )
   {
      if( lci->first != key )
         break;

      if( lci->second.p_root_class == &owner )
      {
         lock = lci->second;
         break;
      }
   }

   return lock;
}

void storage_handler::update_session_locks_for_transaction( session* p_session )
{
   guard g( lock_mutex );

   TRACE_LOG( TRACE_DETAILS | TRACE_LOCKING, "[update session locks for transaction] p_session = " + to_string( p_session ) );

   lock_index_iterator lii;

   ods* p_ods( ods::instance( ) );

   int64_t tran_id( p_ods->get_transaction_id( ) );
   int64_t tran_level( p_ods->get_transaction_level( ) );

   for( lii = lock_index.begin( ); lii != lock_index.end( ); ++lii )
   {
      op_lock& next_lock( lii->second->second );

      if( next_lock.p_session == p_session )
      {
         next_lock.tx_type = next_lock.type;

         next_lock.transaction_id = tran_id;
         next_lock.transaction_level = tran_level;
      }
   }

   IF_IS_TRACING( TRACE_DETAILS | TRACE_LOCKING )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_DETAILS | TRACE_LOCKING, "[dump_locks]\n" + osstr.str( ) );
   }
}

void storage_handler::release_locks_for_owner( class_base& owner, bool force_removal )
{
   guard g( lock_mutex );

   TRACE_LOG( TRACE_DETAILS | TRACE_LOCKING, "[release locks for owner] owner = "
    + to_string( &owner ) + ", force_removal = " + to_string( force_removal ) );

   lock_index_iterator lii;

   for( lii = lock_index.begin( ); lii != lock_index.end( ); )
   {
      op_lock& next_lock( lii->second->second );

      if( next_lock.p_root_class == &owner )
      {
         if( !force_removal && ( next_lock.transaction_level > 0 ) )
         {
            next_lock.type = op_lock::e_lock_type_none;
            next_lock.p_root_class = 0;
         }
         else
         {
            locks.erase( lii->second );
            lock_index.erase( lii++ );
         }
      }
      else
         ++lii;
   }

   IF_IS_TRACING( TRACE_DETAILS | TRACE_LOCKING )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_DETAILS | TRACE_LOCKING, "[dump_locks]\n" + osstr.str( ) );
   }
}

void storage_handler::release_locks_for_commit( session* p_session )
{
   guard g( lock_mutex );

   TRACE_LOG( TRACE_DETAILS | TRACE_LOCKING, "[release locks for commit] p_session = " + to_string( p_session ) );

   ods* p_ods( ods::instance( ) );

   lock_index_iterator lii;

   for( lii = lock_index.begin( ); lii != lock_index.end( ); )
   {
      op_lock& next_lock( lii->second->second );

      if( ( next_lock.p_session == p_session )
       && ( next_lock.transaction_level >= p_ods->get_transaction_level( ) ) )
      {
         if( p_ods->get_transaction_level( ) > 1 )
         {
            next_lock.transaction_level = p_ods->get_transaction_level( ) - 1;
            next_lock.type = op_lock::e_lock_type_none;

            ++lii;
         }
         else
         {
            locks.erase( lii->second );

            size_t id( lii->first );

            lock_index.erase( lii );

            lii = lock_index.lower_bound( id );
         }
      }
      else
         ++lii;
   }

   IF_IS_TRACING( TRACE_DETAILS | TRACE_LOCKING )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_DETAILS | TRACE_LOCKING, "[dump_locks]\n" + osstr.str( ) );
   }
}

void storage_handler::release_locks_for_rollback( session* p_session )
{
   guard g( lock_mutex );

   TRACE_LOG( TRACE_DETAILS | TRACE_LOCKING, "[release locks for rollback] p_session = " + to_string( p_session ) );

   ods* p_ods( ods::instance( ) );

   lock_index_iterator lii;

   for( lii = lock_index.begin( ); lii != lock_index.end( ); )
   {
      op_lock& next_lock( lii->second->second );

      if( ( next_lock.p_session == p_session )
       && ( next_lock.transaction_id == p_ods->get_transaction_id( ) )
       && ( next_lock.transaction_level >= p_ods->get_transaction_level( ) ) )
      {
         locks.erase( lii->second );
         lock_index.erase( lii++ );
      }
      else
         ++lii;
   }

   IF_IS_TRACING( TRACE_DETAILS | TRACE_LOCKING )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_DETAILS | TRACE_LOCKING, "[dump_locks]\n" + osstr.str( ) );
   }
}

void storage_handler::release_all_locks_for_session( session* p_session )
{
   guard g( lock_mutex );

   TRACE_LOG( TRACE_DETAILS | TRACE_LOCKING, "[release all locks for session] p_session = " + to_string( p_session ) );

   lock_index_iterator lii;

   for( lii = lock_index.begin( ); lii != lock_index.end( ); )
   {
      op_lock& next_lock( lii->second->second );

      if( next_lock.p_session == p_session )
      {
         locks.erase( lii->second );
         lock_index.erase( lii++ );
      }
      else
         ++lii;
   }

   IF_IS_TRACING( TRACE_DETAILS | TRACE_LOCKING )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_DETAILS | TRACE_LOCKING, "[dump_locks]\n" + osstr.str( ) );
   }
}

void storage_handler::clear_cache( )
{
   get_record_cache( ).clear( );
   get_time_for_key( ).clear( );
   get_key_for_time( ).clear( );
}

void storage_handler::set_cache_limit( size_t new_limit )
{
   if( !new_limit )
      clear_cache( );
   else
   {
      while( get_record_cache( ).size( )
       && ( get_record_cache( ).size( ) > new_limit ) )
      {
         string oldest_key_info = get_key_for_time( ).begin( )->second;

         get_record_cache( ).erase( oldest_key_info );
         get_time_for_key( ).erase( oldest_key_info );

         get_key_for_time( ).erase( get_key_for_time( ).begin( ) );
      }
   }

   get_root( ).cache_limit = new_limit;
}

struct scoped_lock_holder
{
   scoped_lock_holder( storage_handler& handler, size_t lock_handle )
    :
    handler( handler ),
    lock_handle( lock_handle )
   {
   }

   ~scoped_lock_holder( )
   {
      if( lock_handle )
         handler.release_lock( lock_handle, true );
   }

   void release( )
   {
      lock_handle = 0;
   }

   storage_handler& handler;

   size_t lock_handle;
};

const size_t c_max_sessions_limit = 1000;
const size_t c_min_sessions_limit = 10;

const size_t c_max_storage_handlers_limit = 100;
const size_t c_min_storage_handlers_limit = 1;

const size_t c_max_sessions_default = 100;
const size_t c_max_storage_handlers_default = 10;

const size_t c_notifier_ignore_secs_default = 2;

const size_t c_num_recv_stream_sessions_default = 10;
const size_t c_num_send_stream_sessions_default = 1;

string g_empty_string;

string_container g_strings;

unsigned int g_server_port = c_default_ciyam_port;

unsigned int g_stream_port = 0;
unsigned int g_stream_sock = 0;

size_t g_max_sessions = c_max_sessions_default;
size_t g_max_storage_handlers = c_max_storage_handlers_default + 1; // i.e. extra for <none>

size_t g_notifier_ignore_secs = c_notifier_ignore_secs_default;

string g_log_files_dir;

string g_files_area_dir;
string g_files_area_dir_default;

size_t g_files_area_item_max_num = c_files_area_item_max_num_default;
size_t g_files_area_item_max_size = c_files_area_item_max_size_default;

size_t g_num_recv_stream_sessions = c_num_recv_stream_sessions_default;
size_t g_num_send_stream_sessions = c_num_send_stream_sessions_default;

const char* const c_default_storage_name = "<none>";
const char* const c_default_storage_identity = "<default>";

unique_ptr< ods > gup_ods;

unique_ptr< ods_file_system > gup_ofs;

string g_domain;
string g_timezone;

string g_web_root;

string g_ntfy_server;

bool g_use_udp = false;
bool g_use_https = true;
bool g_using_ssl = false;

string g_gpg_password;
string g_pem_password;
string g_rpc_password;
string g_sql_password;

string g_user_home_path;

string g_default_storage;

uint32_t g_trace_flags = c_unset_trace_flags;

unsigned int g_test_peer_port = 0;

unsigned int g_session_timeout = 0;

unsigned int g_max_peers = c_default_max_peers;
unsigned int g_max_user_limit = c_default_max_user_limit;

bool g_script_reconfig = false;

bool g_ods_use_encrypted = true;
bool g_ods_use_sync_write = true;

set< string > g_checked_script_lock_files;

set< string > g_accepted_ip_addrs;
set< string > g_rejected_ip_addrs;

set< string > g_accepted_peer_ip_addrs;
set< string > g_rejected_peer_ip_addrs;

map< string, map< string, pair< string, string > > > g_mapped_hash_values;

string g_mbox_path;
string g_mbox_username;

string g_pop3_server;
string g_pop3_suffix;
string g_pop3_username;
string g_pop3_password;
string g_pop3_security;

string g_smtp_server;
string g_smtp_sender;
string g_smtp_suffix;
string g_smtp_username;
string g_smtp_password;
string g_smtp_security;

int g_smtp_max_send_attempts = c_min_smtp_max_send_attempts;

int64_t g_smtp_max_attached_data = INT64_C( 100000 );

typedef map< string, external_client > external_client_container;
typedef external_client_container::iterator external_client_iterator;
typedef external_client_container::const_iterator external_client_const_iterator;
typedef external_client_container::value_type external_client_value_type;

external_client_container g_external_client_info;

const int c_take_total_divided_by_ten = -10;

// NOTE: Takes N (or size / abs(N)) items from the front of a source deque and appends them to
// the back of a destination deque retaining the source deque's front item (when using abs(N))
// and preserving the order of all items that are moved to the destination deque.
string take_front_from_deque( deque< string >& src, deque< string >& dest, int take_or_divide )
{
   size_t num_to_take = ( take_or_divide >= 0 ? take_or_divide
    : max( ( size_t )1, ( src.size( ) / abs( take_or_divide ) ) ) );

   string retval;

   if( !src.empty( ) )
   {
      string src_front( src.front( ) );

      if( take_or_divide < 0 )
         src.pop_front( );

      for( size_t i = 0; i < num_to_take; i++ )
      {
         dest.push_back( src.front( ) );
         src.pop_front( );
      }

      if( take_or_divide < 0 )
         src.push_front( src_front );
   }

   if( !dest.empty( ) )
      retval = dest.front( );

   return retval;
}

size_t get_last_raw_file_data_chunk(
 const string& tree_tag, const string& log_blob_file_prefix,
 string& raw_file_data, string* p_last_hash = 0, bool remove_existing_blobs = false )
{
   size_t raw_file_chunk = 0;

   string separator( ":" );
   string last_tag, last_hash;

   string all_tags( list_file_tags( log_blob_file_prefix + ".*" ) );

   if( !all_tags.empty( ) )
   {
      vector< string > tags;
      split( all_tags, tags, '\n' );

      if( remove_existing_blobs )
      {
         for( size_t i = 0; i < tags.size( ); i++ )
            tag_del( tags[ i ], true );

         create_list_tree( "", log_blob_file_prefix, true, tree_tag, tree_tag, "" );
      }
      else
      {
         last_tag = tags.back( );
         last_hash = tag_file_hash( last_tag );

         raw_file_data += extract_file( last_hash, "" );

         raw_file_chunk = from_string< size_t >( last_tag.substr( log_blob_file_prefix.length( ) + 1 ) );
      }
   }

   if( p_last_hash )
      *p_last_hash = last_hash;

   return raw_file_chunk;
}

struct reconstruct_trace_progress : progress
{
   reconstruct_trace_progress( const string& name, bool also_to_cout = false )
    :
    name( name ),
    dtm( date_time::local( ) ),
    also_to_cout( also_to_cout ),
    num_chars_to_cout( 0 )
   {
      if( !g_web_root.empty( ) )
         stop_file = g_web_root + '/' + c_ciyam_ui_stop_file;

      if( !stop_file.empty( ) )
         file_touch( stop_file, 0, true );

      // FUTURE: This message should be handled as a server string message.
      string message( "(starting restore for ODS DB '" + name + "'..." );

      TRACE_LOG( TRACE_MINIMAL, message );
   }

   void clear_num_cout_chars( )
   {
      if( num_chars_to_cout )
      {
         cout << string( num_chars_to_cout, '\b' );
         cout << string( num_chars_to_cout, ' ' ) << '\r';

         cout.flush( );
      }
   }

   ~reconstruct_trace_progress( )
   {
      // FUTURE: This message should be handled as a server string message.
      string message( "...finished restore for ODS DB '" + name + "')" );

      TRACE_LOG( TRACE_MINIMAL, message );

      clear_num_cout_chars( );

      if( !stop_file.empty( ) )
         file_remove( stop_file );
   }

   void output_progress( const string& message, unsigned long num, unsigned long total )
   {
      date_time now( date_time::local( ) );
      uint64_t elapsed = seconds_between( dtm, now );

      string extra;

      if( num )
      {
         extra += to_string( num );

         if( total )
            extra += '/' + to_string( total );
      }

      if( also_to_cout )
      {
         if( message == "." )
         {
            cout << '.';
            cout.flush( );

            ++num_chars_to_cout;
         }
         else
         {
            clear_num_cout_chars( );

            cout << message << extra;
            cout.flush( );

            num_chars_to_cout = ( message.length( ) + extra.length( ) );
         }
      }

      // NOTE: Avoid filling the log with a large number of progress messages.
      if( elapsed >= 60 )
      {
         dtm = now;

         string final_message( message );

         if( final_message != "." )
            final_message += extra;
         else
            // FUTURE: This message should be handled as a server string message.
            final_message = "...restore for '" + name + "' is continuing...";

         TRACE_LOG( TRACE_MINIMAL, final_message );
      }
   }

   string name;
   string stop_file;

   date_time dtm;

   bool also_to_cout;

   size_t num_chars_to_cout;
};

void init_system_ods( bool* p_restored = 0 )
{
   string ods_db_name( get_files_area_dir( ) );
   ods_db_name += '/' + string( c_ciyam_server );

   string system_db_files = ods_file_names( ods_db_name );

   vector< string > all_system_db_files;

   if( !system_db_files.empty( ) )
      split( system_db_files, all_system_db_files );

   size_t num_system_db_files = 0;

   for( size_t i = 0; i < all_system_db_files.size( ); i++ )
   {
      if( file_exists( all_system_db_files[ i ] ) )
         ++num_system_db_files;
   }

   string system_backup_files( ods_backup_file_names( c_ciyam_server, c_sav_file_ext ) );

   vector< string > all_system_backup_files;

   if( !system_backup_files.empty( ) )
      split( system_backup_files, all_system_backup_files );

   size_t num_system_backup_files = 0;

   for( size_t i = 0; i < all_system_backup_files.size( ); i++ )
   {
      if( file_exists( all_system_backup_files[ i ] ) )
         ++num_system_backup_files;
   }

   // NOTE: If the system ODS DB is being restored from backup then first check that either
   // the number of system ODS DB files currently existing is zero or is an exact match for
   // the number of backup files that have been found (logging if a mismatch is found).
   if( num_system_backup_files )
   {
      bool restore_system_db = true;

      if( num_system_db_files )
      {
         if( num_system_db_files != num_system_backup_files )
         {
            restore_system_db = false;

            TRACE_LOG( TRACE_MINIMAL, "*** mismatched number of system/backup ODS DB files ("
             + to_string( num_system_db_files ) + '/' + to_string( num_system_backup_files ) + ") ***" );
         }
      }

      if( restore_system_db )
      {
         for( size_t i = 0; i < all_system_backup_files.size( ); i++ )
         {
            string backup_file_name( all_system_backup_files[ i ] );

            string stored_file_name( replaced( backup_file_name, c_sav_file_ext, c_old_file_ext ) );

            string system_file_name( get_files_area_dir( ) + '/' + replaced( backup_file_name, c_sav_file_ext, "" ) );

            if( file_exists( backup_file_name ) )
            {
               if( num_system_db_files )
                  file_rename( system_file_name, stored_file_name );

               file_rename( backup_file_name, system_file_name );
            }
         }

         if( p_restored )
            *p_restored = true;
      }
   }

   gup_ods.reset(
    new ods( ods_db_name.c_str( ),
    ods::e_open_mode_create_if_not_exist,
    ods::e_write_mode_exclusive, true, 0, 0, g_ods_use_sync_write ) );

   bool was_just_created = false;

   if( gup_ods->is_corrupt( ) )
   {
      reconstruct_trace_progress progress( ods_db_name, true );

      gup_ods->repair_corrupt_database( &progress );
   }
   else if( gup_ods->is_new( ) )
      was_just_created = true;

   if( !was_just_created
    && ( gup_ods->get_next_transaction_id( ) >= c_num_txs_for_reset ) )
   {
      reconstruct_trace_progress progress( ods_db_name, true );

      gup_ods->compress_and_reset_tx_log( &progress );
   }

   system_ods_bulk_write ods_bulk_write;

   gup_ofs.reset( new ods_file_system( *gup_ods ) );

   unique_ptr< ods::transaction > up_tx;

   if( was_just_created
    || !gup_ofs->has_folder( c_system_archives_folder )
    || !gup_ofs->has_folder( c_system_blacklist_folder )
    || !gup_ofs->has_folder( c_system_datachain_folder )
    || !gup_ofs->has_folder( c_system_peerchain_folder )
    || !gup_ofs->has_folder( c_system_variables_folder )
    || !gup_ofs->has_folder( c_system_repository_folder ) )
      up_tx.reset( new ods::transaction( *gup_ods ) );

   if( !gup_ofs->has_folder( c_system_archives_folder ) )
      gup_ofs->add_folder( c_system_archives_folder );

   if( !gup_ofs->has_folder( c_system_blacklist_folder ) )
      gup_ofs->add_folder( c_system_blacklist_folder );

   if( !gup_ofs->has_folder( c_system_datachain_folder ) )
      gup_ofs->add_folder( c_system_datachain_folder );

   if( !gup_ofs->has_folder( c_system_peerchain_folder ) )
      gup_ofs->add_folder( c_system_peerchain_folder );

   if( !gup_ofs->has_folder( c_system_variables_folder ) )
      gup_ofs->add_folder( c_system_variables_folder );

   if( !gup_ofs->has_folder( c_system_repository_folder ) )
      gup_ofs->add_folder( c_system_repository_folder );

   if( num_system_backup_files )
   {
      if( dir_exists( c_system_repository_folder ) )
      {
         gup_ofs->set_folder( c_system_repository_folder );

         import_objects( *gup_ofs, c_system_repository_folder );

         gup_ofs->set_folder( ".." );

         delete_directory_files( c_system_repository_folder, true );
      }
   }

   if( up_tx.get( ) )
      up_tx->commit( );

   if( !was_just_created )
   {
      // NOTE: Restore all persistent system variable values.
      vector< string > variable_files;

      gup_ofs->set_root_folder( c_system_variables_folder );

      gup_ofs->list_files( variable_files );

      for( size_t i = 0; i < variable_files.size( ); i++ )
      {
         string value;
         gup_ofs->fetch_from_text_file( variable_files[ i ], value );

         set_system_variable( variable_files[ i ], value, true );
      }
   }
}

void term_system_ods( )
{
   if( gup_ofs.get( ) )
   {
      // NOTE: Clear all persistent system variable values.
      vector< string > variable_files;

      gup_ofs->set_root_folder( c_system_variables_folder );

      gup_ofs->list_files( variable_files );

      for( size_t i = 0; i < variable_files.size( ); i++ )
         set_system_variable( variable_files[ i ], "" );
   }

   gup_ofs.reset( );
   gup_ods.reset( );
}

typedef map< unsigned int, string > listener_container;
typedef map< string, unsigned int > listener_id_container;

typedef listener_container::const_iterator listener_const_iterator;
typedef listener_id_container::const_iterator listener_id_const_iterator;

listener_container g_listeners;
listener_id_container g_listener_ids;

size_t g_next_session_id;

typedef vector< session* > session_container;

session_container g_sessions;

map< size_t, date_time > g_condemned_sessions;

typedef vector< storage_handler* > storage_handler_container;

typedef map< string, size_t > storage_handler_index_container;
typedef storage_handler_index_container::iterator storage_handler_index_iterator;
typedef storage_handler_index_container::const_iterator storage_handler_index_const_iterator;
typedef storage_handler_index_container::value_type storage_handler_index_value_type;

storage_handler_container g_storage_handlers;
storage_handler_index_container g_storage_handler_index;

enum storage_op
{
   e_storage_op_init,
   e_storage_op_create,
   e_storage_op_attach
};

void perform_storage_op( storage_op op,
 const string& name, const string& directory, command_handler& cmd_handler, bool lock_for_admin )
{
   guard g( g_mutex, "perform_storage_op" );

   storage_handler* p_new_handler = 0;

   if( name == c_default_storage_name )
      throw runtime_error( "storage name '" + name + "' cannot be used explicitly" );

   string new_log_name( name + ".log.new" );

   if( !lock_for_admin && file_exists( new_log_name ) )
      throw runtime_error( "storage is under administration (i.e. found " + new_log_name + ")" );

   if( !gtp_session->instance_registry.empty( ) )
      throw runtime_error( "all object instances must be destroyed before linking to a storage" );

   if( gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "current storage needs to be terminated before another can be linked to" );

   bool was_constructed = false;

   if( g_storage_handler_index.find( name ) != g_storage_handler_index.end( ) )
   {
      if( op == e_storage_op_create )
         throw runtime_error( "storage '" + name + "' cannot be created as it is currently in use" );

      if( lock_for_admin )
         throw runtime_error( "storage '" + name + "' cannot be administered as it is currently in use" );

      p_new_handler = g_storage_handlers[ g_storage_handler_index[ name ] ];
   }
   else
   {
      size_t slot = 0;

      for( slot = 1; slot < g_max_storage_handlers; slot++ )
      {
         if( !g_storage_handlers[ slot ] )
            break;
      }

      if( !slot || ( slot == g_max_storage_handlers ) )
         throw runtime_error( "max. permitted concurrent storage handlers currently active" );

      if( gtp_session->up_storage_name_lock.get( ) )
         gtp_session->up_storage_name_lock.reset( );

      if( name == g_storage_name_lock )
         throw runtime_error( "storage '" + name + "' cannot be administered as it is currently in use" );

      bool file_not_found = false;

      ods::open_mode open_mode;

      if( op == e_storage_op_attach )
         open_mode = ods::e_open_mode_exist;
      else if( op == e_storage_op_create )
         open_mode = ods::e_open_mode_not_exist;
      else if( !lock_for_admin )
         open_mode = ods::e_open_mode_exist;
      else
         open_mode = ods::e_open_mode_create_if_not_exist;

      try
      {
         string sid;

         const char* p_password = 0;

         bool has_exported_objects = false;
         bool has_imported_channels = false;
         bool has_imported_datachains = false;

         if( name != c_ciyam_storage_name )
         {
            if( dir_exists( name ) )
            {
               has_exported_objects = true;

               open_mode = ods::e_open_mode_create_if_not_exist;
            }

            if( g_ods_use_encrypted && ( name != c_meta_storage_name ) )
            {
               get_sid( sid );

               sha256 hash( sid + name );

               sid = hash.get_digest_as_string( );

               p_password = sid.c_str( );
            }
         }

         unique_ptr< ods > up_ods( new ods( name.c_str( ), open_mode,
          ods::e_write_mode_exclusive, true, &file_not_found, p_password, g_ods_use_sync_write ) );

         clear_key( sid );

         unique_ptr< storage_handler > up_handler( new storage_handler( slot, name, up_ods.get( ) ) );

         if( up_ods->is_corrupt( ) )
         {
            reconstruct_trace_progress progress( name );

            up_ods->repair_corrupt_database( &progress );
         }

         ods::instance( up_ods.get( ) );

         if( up_ods->is_new( ) && has_exported_objects )
         {
            ods::bulk_write bulk_write( *up_ods );

            ods::transaction tx( *up_ods );

            ods_file_system ofs( *up_ods );

            import_objects( ofs, name );

            tx.commit( );

            delete_directory_files( name, true );

            if( !ofs.has_folder( c_storage_folder_name_modules ) )
            {
               has_exported_objects = false;

               if( ofs.has_folder( c_storage_folder_name_channels ) )
                  has_imported_channels = true;

               if( ofs.has_folder( c_storage_folder_name_datachains ) )
                  has_imported_datachains = true;
            }
         }

         if( up_ods->is_new( ) && !has_exported_objects )
         {
            ods::bulk_write bulk_write( *up_ods );

            ods::transaction tx( *up_ods );

            up_handler->get_root( ).module_directory = directory;

            string blockchain( get_raw_system_variable(
             get_special_var_name( e_special_var_blockchain_backup_identity ) ) );

            bool is_peerchain_application = false;

            if( !blockchain.empty( ) && ( name != c_meta_storage_name ) && ( name != c_ciyam_storage_name ) )
               is_peerchain_application = true;

            if( is_peerchain_application )
               up_handler->get_root( ).type = e_storage_type_peerchain;

            ods_file_system ofs( *up_ods );

            ofs.add_folder( c_storage_folder_name_modules );

            if( name == c_meta_storage_name )
            {
               ofs.add_folder( c_storage_folder_name_dot_dat );
               ofs.add_folder( c_storage_folder_name_dot_idx );
            }

            if( is_peerchain_application )
            {
               if( !has_imported_channels )
                  ofs.add_folder( c_storage_folder_name_channels );

               if( !has_imported_datachains )
                  ofs.add_folder( c_storage_folder_name_datachains );
            }

            ofs.add_folder( c_storage_folder_name_variables );

            up_handler->get_root( ).store_as_text_files( ofs );

            tx.commit( );
         }
         else
         {
            if( up_handler->get_root( ).version != c_storage_format_version )
               throw runtime_error( "found incorrect storage format version " + to_string( up_handler->get_root( ).version ) );

            ods::bulk_read bulk_read( *up_ods );

            ods_file_system ofs( *up_ods );

            up_handler->get_root( ).fetch_from_text_files( ofs );

            ofs.set_folder( c_storage_folder_name_modules );

            vector< string > order_prefixed_module_list;

            ofs.list_folders( order_prefixed_module_list );

            for( size_t i = 0; i < order_prefixed_module_list.size( ); i++ )
            {
               string next( order_prefixed_module_list[ i ] );

               string::size_type pos = next.find( c_module_order_prefix_separator );

               if( pos != string::npos )
                  next.erase( 0, pos + 1 );

               up_handler->get_root( ).module_list.push_back( next );
            }

            ofs.set_folder( ".." );

            if( ofs.has_folder( c_storage_folder_name_variables ) )
            {
               ofs.set_folder( c_storage_folder_name_variables );

               // NOTE: Restore all persistent storage variable values.
               vector< string > variable_files;

               ofs.list_files( variable_files );

               for( size_t i = 0; i < variable_files.size( ); i++ )
               {
                  string value;

                  ofs.fetch_from_text_file( variable_files[ i ], value );

                  up_handler->set_variable( variable_files[ i ], value );
               }
            }

            // NOTE: Force an identity write to occur when the first transaction is logged.
            up_handler->get_root( ).log_id.ceiling = up_handler->get_root( ).log_id.next_id;
         }

         string dead_keys_file( up_handler->get_name( ) + c_dead_keys_suffix );

         if( exists_file( dead_keys_file ) )
         {
            vector< string > lines;
            buffer_file_lines( dead_keys_file, lines );

            for( size_t i = 0; i < lines.size( ); i++ )
            {
               if( !lines[ i ].empty( ) )
               {
                  string next( lines[ i ] );

                  string::size_type pos = next.find( ':' );

                  if( pos == string::npos )
                     throw runtime_error( "unexpected invalid dead key format '" + next + "'" );

                  up_handler->get_dead_keys( ).insert( next );
               }
            }

            buffer_file_lines( dead_keys_file, up_handler->get_dead_keys( ) );
         }

         p_new_handler = up_handler.release( );

         was_constructed = true;

         g_storage_handlers[ slot ] = p_new_handler;
         g_storage_handler_index.insert( make_pair( name, slot ) );

         // NOTE: The ODS instance now belongs to the storage handler (and not to this session).
         up_ods.release( );
         ods::instance( 0, true );
      }
      catch( ... )
      {
         ods::instance( 0, true );

         // NOTE: Swallow this exception to output a simpler error message below.
         if( !file_not_found )
            throw;
      }

      if( file_not_found )
         throw runtime_error( "storage '" + name + "' was not found" );
   }

   if( p_new_handler && ( p_new_handler != gtp_session->p_storage_handler ) )
   {
      if( p_new_handler->get_is_locked_for_admin( ) )
         // FUTURE: This message should be handled as a server string message.
         throw runtime_error( "Application is currently locked for Administration." );

      bool created_ods_instance = false;
      storage_handler* p_old_handler = gtp_session->p_storage_handler;

      try
      {
         gtp_session->up_db.reset( new sql_db( p_new_handler->get_name( ), p_new_handler->get_name( ) ) );

         ods::instance( new ods( *p_new_handler->get_ods( ) ) );

         created_ods_instance = true;

         gtp_session->p_storage_handler = p_new_handler;
         gtp_session->p_storage_handler->inc_ref_count( );

         if( module_count( ) && !p_new_handler->get_root( ).module_directory.empty( ) )
            throw runtime_error( "storages with a module directory cannot be initialised whilst modules are already loaded" );

         module_directory( &p_new_handler->get_root( ).module_directory );

         // NOTE: Modules that have been registered to this storage are now automatically loaded (if not already present).
         // This is performed in the same order that the modules were registered (as dependencies may exist between them).
         size_t num_modules( p_new_handler->get_root( ).module_list.size( ) );

         for( size_t i = 0; i < num_modules; i++ )
         {
            string next_module( p_new_handler->get_root( ).module_list[ i ] );

            if( gtp_session->modules_by_name.find( next_module ) == gtp_session->modules_by_name.end( ) )
            {
               module_load( next_module, cmd_handler );
               gtp_session->storage_controlled_modules.push_back( next_module );
            }
         }

         string algos_file( gtp_session->p_storage_handler->get_name( ) + ".algos.lst" );

         if( lock_for_admin )
            gtp_session->p_storage_handler->set_is_locked_for_admin( );
         else if( file_exists( algos_file ) )
         {
            temporary_algo_prefix tmp_algo_prefix( gtp_session->p_storage_handler->get_name( ) );

            exec_algos_action( "load", algos_file, "" );
         }

         set_session_variable( get_special_var_name( e_special_var_prior_storage ),
          get_raw_session_variable( get_special_var_name( e_special_var_storage ) ) );

         set_session_variable( get_special_var_name( e_special_var_storage ), gtp_session->p_storage_handler->get_name( ) );
      }
      catch( ... )
      {
         while( !gtp_session->storage_controlled_modules.empty( ) )
         {
            module_unload( gtp_session->storage_controlled_modules.back( ), cmd_handler );

            gtp_session->storage_controlled_modules.pop_back( );
         }

         if( created_ods_instance )
         {
            delete ods::instance( );
            ods::instance( 0, true );
         }

         if( was_constructed )
         {
            p_new_handler->release_ods( );

            g_storage_handler_index.erase( p_new_handler->get_name( ) );
            g_storage_handlers[ p_new_handler->get_slot( ) ] = 0;

            delete p_new_handler;
         }

         gtp_session->p_storage_handler = p_old_handler;

         throw;
      }
   }
}

void remove_tx_info_from_cache( )
{
   storage_handler& handler( *gtp_session->p_storage_handler );

   for( set< string >::iterator i = gtp_session->tx_key_info.begin( ), e = gtp_session->tx_key_info.end( ); i != e; ++i )
   {
      if( handler.get_record_cache( ).count( *i ) )
      {
         time_info_iterator tii = handler.get_key_for_time( ).lower_bound( handler.get_time_for_key( ).find( *i )->second );

         while( tii->second != *i )
            ++tii;

         handler.get_key_for_time( ).erase( tii );
         handler.get_time_for_key( ).erase( *i );

         handler.get_record_cache( ).erase( *i );
      }
   }
}

bool is_child_constrained( class_base& instance,
 class_base& root_instance, string& constraining_class, map< string, set< string > >& instance_keys )
{
   class_base_accessor instance_accessor( instance );

   class_base* p_class_base;
   string sql, next_child_field;

   size_t num_children = instance_accessor.get_num_foreign_key_children( );

   if( num_children && !instance_keys[ instance.get_class_id( ) ].count( instance.get_key( ) ) )
   {
      for( int pass = 0; pass < 2; ++pass )
      {
         cascade_op next_op;

         if( pass == 0 )
            next_op = e_cascade_op_restrict;
         else
            next_op = e_cascade_op_destroy;

         for( size_t i = 0; i < num_children; i++ )
         {
            p_class_base = instance_accessor.get_next_foreign_key_child( i, next_child_field, next_op );

            unique_ptr< class_cascade > up_tmp_cascading;

            if( p_class_base )
               up_tmp_cascading.reset( new class_cascade( *p_class_base ) );

            // FUTURE: The handling of cascades needs to be revisited at some stage to improve performance
            // as currently iteration occurs three times (firstly to lock, secondly to test if constrained
            // and thirdly for the actual deletions).
            if( p_class_base
             && p_class_base->iterate_forwards( "", c_key_field, true, 0, e_sql_optimisation_unordered ) )
            {
               class_base_accessor child_instance_accessor( *p_class_base );

               if( next_op == e_cascade_op_restrict )
               {
                  storage_handler& handler( *gtp_session->p_storage_handler );

                  do
                  {
                     // NOTE: If the instance that would restrict has already been locked for destroy then
                     // it can be safely ignored (i.e. continue iteration to find the next instance). Once
                     // an actual restricting instance is found then no further iteration is required.
                     op_lock lock = handler.get_lock_info_for_owner(
                      p_class_base->get_lock_class_id( ), p_class_base->get_key( ), root_instance );

                     if( lock.type != op_lock::e_lock_type_destroy )
                     {
                        constraining_class = p_class_base->get_display_name( );

                        p_class_base->iterate_stop( );

                        return true;
                     }
                  } while( p_class_base->iterate_next( ) );
               }
               else
               {
                  do
                  {
                     // NOTE: If the child instance is actually a derived class then get the derived object.
                     sql.erase( );
                     class_base* p_dyn_class_base( child_instance_accessor.fetch_dynamic_instance( sql, false ) );

                     // FUTURE: This fetch should not actually be necessary as long as the key has been set.
                     if( !sql.empty( ) )
                        fetch_instance_from_db( *p_dyn_class_base, sql );

                     if( next_op == e_cascade_op_destroy )
                        instance_keys[ p_class_base->get_class_id( ) ].insert( p_class_base->get_key( ) );

                     if( is_child_constrained( *p_dyn_class_base, root_instance, constraining_class, instance_keys ) )
                     {
                        p_class_base->iterate_stop( );

                        return true;
                     }

                     child_instance_accessor.destroy_dynamic_instance( );

                  } while( p_class_base->iterate_next( ) );
               }
            }
         }
      }
   }

   return false;
}

bool obtain_cascade_locks_for_destroy( class_base& instance,
 class_base& root_instance, map< string, set< string > >& instance_keys )
{
   class_base_accessor instance_accessor( instance );

   size_t lock_handle;
   class_base* p_class_base;
   string sql, next_child_field;

   storage_handler& handler( *gtp_session->p_storage_handler );

   size_t num_children = instance_accessor.get_num_foreign_key_children( );

   if( num_children && !instance_keys[ instance.get_class_id( ) ].count( instance.get_key( ) ) )
   {
      for( int pass = 0; pass < 2; ++pass )
      {
         cascade_op next_op;

         if( pass == 0 )
            next_op = e_cascade_op_destroy;
         else
            next_op = e_cascade_op_unlink;

         for( size_t i = 0; i < num_children; i++ )
         {
            p_class_base = instance_accessor.get_next_foreign_key_child( i, next_child_field, next_op );

            unique_ptr< class_cascade > up_tmp_cascading;

            if( p_class_base )
               up_tmp_cascading.reset( new class_cascade( *p_class_base ) );

            // FUTURE: The handling of cascades needs to be revisited at some stage to improve performance
            // as currently iteration occurs three times (firstly to lock, secondly to test if constrained
            // and thirdly for the actual deletions).
            if( p_class_base
             && p_class_base->iterate_forwards( "", c_key_field, true, 0, e_sql_optimisation_unordered ) )
            {
               class_base_accessor child_instance_accessor( *p_class_base );

               do
               {
                  // NOTE: A lock may have already been obtained during the recursion due to multiple
                  // relationship paths leading to the same instance - if found then simply continue.
                  op_lock lock = handler.get_lock_info_for_owner(
                   p_class_base->get_lock_class_id( ), p_class_base->get_key( ), root_instance );

                  if( lock.type != op_lock::e_lock_type_none )
                     continue;

                  // NOTE: It is possible that the original instance being destroyed (for which the lock
                  // is not marked class "owned") may also be subject to unlinking (due to the existence
                  // of a relationship to a child instance) so it's being assumed here that if a destroy
                  // lock belonging to the same session is found for this instance then this is the case.
                  if( pass == 1 )
                  {
                     lock = handler.get_lock_info(
                      p_class_base->get_lock_class_id( ), p_class_base->get_key( ) );

                     if( ( lock.p_session == gtp_session )
                      && ( lock.type == op_lock::e_lock_type_destroy ) )
                        continue;
                  }

                  if( next_op == e_cascade_op_unlink )
                  {
                     if( !handler.obtain_lock( lock_handle, p_class_base->get_lock_class_id( ),
                      p_class_base->get_key( ), op_lock::e_lock_type_update, gtp_session, &instance, &root_instance ) )
                     {
                        p_class_base->iterate_stop( );

                        return false;
                     }
                  }
                  else
                  {
                     if( handler.obtain_lock( lock_handle, p_class_base->get_lock_class_id( ),
                      p_class_base->get_key( ), op_lock::e_lock_type_destroy, gtp_session, &instance, &root_instance ) )
                     {
                        // NOTE: If the child instance is actually a derived class then get the derived object.
                        sql.erase( );
                        class_base* p_dyn_class_base( child_instance_accessor.fetch_dynamic_instance( sql, false ) );

                        // FUTURE: This fetch should not actually be necessary as long as the key has been set.
                        if( !sql.empty( ) )
                           fetch_instance_from_db( *p_dyn_class_base, sql );

                        if( next_op == e_cascade_op_destroy )
                           instance_keys[ p_class_base->get_class_id( ) ].insert( p_class_base->get_key( ) );

                        if( !obtain_cascade_locks_for_destroy( *p_dyn_class_base, root_instance, instance_keys ) )
                        {
                           p_class_base->iterate_stop( );

                           return false;
                        }

                        child_instance_accessor.destroy_dynamic_instance( );
                     }
                     else
                     {
                        p_class_base->iterate_stop( );

                        return false;
                     }
                  }
               } while( p_class_base->iterate_next( ) );
            }
         }
      }
   }

   return true;
}

size_t obtain_keyed_lock( const string& lock_class, const string& key, op_lock::lock_type lock_type, size_t num_attempts = 0 )
{
   if( !gtp_session
    || !gtp_session->p_storage_handler
    || !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   storage_handler& handler( *gtp_session->p_storage_handler );

   size_t lock_handle = 0;

   handler.obtain_lock( lock_handle, lock_class, key, lock_type, gtp_session, 0, 0, num_attempts );

   return lock_handle;
}

inline string get_field_name( class_base& instance,
 string& field_name, bool* p_sql_numeric = 0, string* p_type_name = 0 )
{
   const char* p_str;
   bool was_name = true;
   class_base_accessor instance_accessor( instance );

   if( field_name == c_key_field )
      field_name = c_key_field_name;

   if( ( field_name == c_key_field_name ) || ( field_name == c_typ_field_name ) )
   {
      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( ( field_name == c_ver_field_name )
    || ( field_name == c_rev_field_name ) || ( field_name == c_sec_field_name ) )
   {
      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else
   {
      p_str = instance_accessor.get_field_name( field_name, p_sql_numeric, p_type_name );

      if( p_str )
         field_name = p_str;
      else
         p_str = instance_accessor.get_field_id( field_name, p_sql_numeric, p_type_name );

      if( !p_str )
         throw runtime_error( "field name/id " + field_name + " is invalid" );
   }

   return field_name;
}

void append_undo_sql_statements( storage_handler& handler )
{
   if( !gtp_session->sql_undo_statements.empty( ) )
   {
      string undo_sql_filename( handler.get_name( ) );
      undo_sql_filename += ".undo.sql";

      ofstream outf( undo_sql_filename.c_str( ), ios::out | ios::app );
      if( !outf )
         throw runtime_error( "unable to open '" + undo_sql_filename + "' for output" );

      for( size_t i = 0; i < gtp_session->sql_undo_statements.size( ); i++ )
         outf << escaped( gtp_session->sql_undo_statements[ i ], "\"", c_nul, "rn\r\n" ) << '\n';

      outf.flush( );

      if( !outf.good( ) )
         throw runtime_error( "*** unexpected error occurred writing to undo sql ***" );

      outf.close( );
   }
}

string extract_mod_cls_and_key_from_command(
 const string& command, string* p_fields_and_values = 0, string* p_command_dtm_value = 0 )
{
   string retval;

   // NOTE: Expected format is "<cmd> <uid> <dtm> <mod> <cls> <key>[ ...]"
   // so will initially set the pos to just before the key: " <key>[ ...]"
   string::size_type pos = find_nth_occurrence( command, ' ', 5 );

   if( pos == string::npos )
      throw runtime_error( "unexpected command format '" + command + "' in extract_key_from_command" );

   string::size_type npos = command.find( ' ', pos + 1 );

   if( p_fields_and_values && ( npos != string::npos ) && ( command.length( ) > npos + 1 ) )
   {
      size_t skip = 1;

      if( command[ npos + skip ] == '"' )
         ++skip;

      size_t total = command.length( ) - ( npos + skip );

      if( ( skip > 1 ) && ( command[ command.length( ) - 1 ] == '"' ) )
         --total;

      *p_fields_and_values = command.substr( npos + skip, total );
   }

   // NOTE: Now set pos back to before the mod: " <mod> <cls> <key >[ ...]"
   pos = find_nth_occurrence( command, ' ', 3 );

   if( npos != string::npos )
      npos -= ( pos + 1 );

   // NOTE: Finally reduce to: "<mod> <cls> <key>"
   retval = command.substr( pos + 1, npos );

   if( p_command_dtm_value )
   {
      string::size_type dpos = pos;

      while( dpos > 0 )
      {
         if( command[ --dpos ] == ' ' )
            break;
      }

      *p_command_dtm_value = command.substr( dpos + 1, ( pos - dpos - 1 ) );
   }

   return retval;
}

void append_peerchain_log_commands( )
{
   size_t num_commands = gtp_session->peerchain_log_commands.size( );

   if( num_commands )
   {
      guard g( g_mutex );
   
      string first_command( gtp_session->peerchain_log_commands[ 0 ] );

      string::size_type pos = first_command.find( '=' );

      if( pos == string::npos )
         throw runtime_error( "unexpected format for peerchain tx log command '" + first_command + "'" );

      string identity( first_command.substr( 0, pos ) );

      string identity_log( identity + c_log_file_ext );

      bool had_log_file = false;

      vector< string > log_lines;

      if( file_exists( identity_log ) )
      {
         had_log_file = true;
         buffer_file_lines( identity_log, log_lines );
      }

      for( size_t i = 0; i < num_commands; i++ )
      {
         string next_command( gtp_session->peerchain_log_commands[ i ] );

         string::size_type pos = next_command.find( '=' );

         if( pos == string::npos )
            throw runtime_error( "unexpected format for peerchain tx log command '" + next_command + "'" );

         if( next_command.substr( 0, pos ) != identity )
            throw runtime_error( "found identity '" + next_command.substr( 0, pos ) + "' when '" + identity + "' was expected" );

         next_command.erase( 0, pos + 1 );

         bool append_command = true;

         pos = next_command.find( ' ' );

         if( pos == string::npos )
            throw runtime_error( "unexpected missing command in peerchain tx log command '" + next_command + "'" );

         string command( next_command.substr( 0, pos ) );

         // NOTE: If a record is being updated then check whether it had already been created
         // or updated in the identity log file and if so then will try to merge the changes.
         if( command == c_cmd_update )
         {
            string original_dtm_value;

            string update_fields_and_values;

            string update_info( extract_mod_cls_and_key_from_command( next_command, &update_fields_and_values ) );

            if( !log_lines.empty( ) )
            {
               bool had_created = false;
               bool has_changed_any_values = false;

               size_t num_updates = 0;

               size_t log_line_created = 0;
               size_t log_line_updated = 0;

               set< string > field_names;
               map< int, string > ordered_fields;

               map< string, string > field_and_value_info;

               for( size_t i = 0; i < log_lines.size( ); i++ )
               {
                  string next_line( log_lines[ i ] );

                  pos = next_line.find( ' ' );

                  if( pos != string::npos )
                  {
                     command = next_line.substr( 0, pos );

                     if( command == c_cmd_create )
                     {
                        string create_fields_and_values;

                        string create_info( extract_mod_cls_and_key_from_command(
                         next_line, &create_fields_and_values, &original_dtm_value ) );

                        if( !create_fields_and_values.empty( ) && ( create_info == update_info ) )
                        {
                           had_created = true;

                           log_line_created = i;

                           vector< string > field_and_value_pairs;

                           raw_split( create_fields_and_values, field_and_value_pairs );

                           for( size_t j = 0; j < field_and_value_pairs.size( ); j++ )
                           {
                              string next_field_and_value( field_and_value_pairs[ j ] );

                              string::size_type pos = next_field_and_value.find( '=' );

                              if( pos != string::npos )
                              {
                                 string field( next_field_and_value.substr( 0, pos ) );
                                 string value( next_field_and_value.substr( pos + 1 ) );

                                 field_and_value_info[ field ] = value;

                                 if( !field_names.count( field ) )
                                 {
                                    field_names.insert( field );

                                    ordered_fields[ field_names.size( ) ] = field;
                                 }
                              }
                           }
                        }
                     }
                     else if( command == c_cmd_update )
                     {
                        string prior_update_fields_and_values;

                        string prior_update_info( extract_mod_cls_and_key_from_command(
                         next_line, &prior_update_fields_and_values, &original_dtm_value ) );

                        if( !prior_update_fields_and_values.empty( ) && ( prior_update_info == update_info ) )
                        {
                           ++num_updates;

                           log_line_updated = i;

                           vector< string > field_and_value_pairs;

                           raw_split( prior_update_fields_and_values, field_and_value_pairs );

                           for( size_t j = 0; j < field_and_value_pairs.size( ); j++ )
                           {
                              string next_field_and_value( field_and_value_pairs[ j ] );

                              string::size_type pos = next_field_and_value.find( '=' );

                              if( pos != string::npos )
                              {
                                 string field( next_field_and_value.substr( 0, pos ) );
                                 string value( next_field_and_value.substr( pos + 1 ) );

                                 field_and_value_info[ field ] = value;

                                 if( !field_names.count( field ) )
                                 {
                                    field_names.insert( field );

                                    ordered_fields[ field_names.size( ) ] = field;
                                 }
                              }
                           }
                        }
                     }
                  }
               }

               vector< string > field_and_value_pairs;

               raw_split( update_fields_and_values, field_and_value_pairs );

               for( size_t i = 0; i < field_and_value_pairs.size( ); i++ )
               {
                  string next_field_and_value( field_and_value_pairs[ i ] );

                  string::size_type pos = next_field_and_value.find( '=' );

                  if( pos != string::npos )
                  {
                     string field( next_field_and_value.substr( 0, pos ) );
                     string value( next_field_and_value.substr( pos + 1 ) );

                     if( field_and_value_info[ field ] != value )
                     {
                        has_changed_any_values = true;

                        field_and_value_info[ field ] = value;

                        if( !field_names.count( field ) )
                        {
                           field_names.insert( field );

                           ordered_fields[ field_names.size( ) ] = field;
                        }
                     }
                  }
               }

               if( had_created )
               {
                  if( !has_changed_any_values )
                     append_command = false;
                  else if( !num_updates )
                  {
                     // NOTE: If only found a create then will replace the log line so that
                     // changed field values and/or new field value pairs are now included.
                     append_command = false;

                     string new_create_line( c_cmd_create );

                     new_create_line += ' ' + string( c_peer ) + ' ' + original_dtm_value + ' ' + update_info + ' ';

                     new_create_line += '"';

                     for( map< int, string >::iterator i = ordered_fields.begin( ); i != ordered_fields.end( ); ++i )
                     {
                        if( i != ordered_fields.begin( ) )
                           new_create_line += ',';

                        new_create_line += ( i->second + '=' + field_and_value_info[ i->second ] );
                     }

                     new_create_line += '"';

                     log_lines[ log_line_created ] = new_create_line;
                  }
               }
               else if( num_updates )
               {
                  if( !has_changed_any_values )
                     append_command = false;
                  else if( num_updates == 1 )
                  {
                     // NOTE: If only the one update was found then will replace the log line so
                     // that changed field values and/or new field value pairs are also included.
                     append_command = false;

                     string new_update_line( c_cmd_update );

                     new_update_line += ' ' + string( c_peer ) + ' ' + original_dtm_value + ' ' + update_info + ' ';

                     new_update_line += '"';

                     for( map< int, string >::iterator i = ordered_fields.begin( ); i != ordered_fields.end( ); ++i )
                     {
                        if( i != ordered_fields.begin( ) )
                           new_update_line += ',';

                        new_update_line += ( i->second + '=' + field_and_value_info[ i->second ] );
                     }

                     new_update_line += '"';

                     log_lines[ log_line_updated ] = new_update_line;
                  }
               }
            }
         }

         // NOTE: If the record being destroyed had also previously been created in the identity log
         // file will remove that line from the file (or remove the identity log file itself if this
         // was the only line found). Any matching update lines that are found after the create will
         // now also be removed.
         if( command == c_cmd_destroy )
         {
            string destroy_info( extract_mod_cls_and_key_from_command( next_command ) );

            if( !log_lines.empty( ) )
            {
               bool removed_create = false;

               for( size_t i = 0; i < log_lines.size( ); i++ )
               {
                  string next_line( log_lines[ i ] );

                  pos = next_line.find( ' ' );

                  if( pos != string::npos )
                  {
                     command = next_line.substr( 0, pos );

                     if( command == c_cmd_create )
                     {
                        string create_info( extract_mod_cls_and_key_from_command( next_line ) );

                        if( create_info == destroy_info )
                        {
                           removed_create = true;

                           log_lines.erase( log_lines.begin( ) + i-- );
                        }
                     }
                     else if( removed_create && ( command == c_cmd_update ) )
                     {
                        string update_info( extract_mod_cls_and_key_from_command( next_line ) );

                        if( update_info == destroy_info )
                           log_lines.erase( log_lines.begin( ) + i-- );
                     }
                  }
               }

               if( removed_create )
                  append_command = false;
            }
         }

         if( append_command )
            log_lines.push_back( next_command );
      }

      if( had_log_file && log_lines.empty( ) )
         file_remove( identity_log );

      if( !log_lines.empty( ) )
         write_file_lines( identity_log, log_lines );

      gtp_session->peerchain_log_commands.clear( );
   }
}

void append_transaction_log_command( storage_handler& handler,
 bool log_even_when_locked = false, size_t load_module_id = 0, int32_t use_tx_id = 0 )
{
   if( !gtp_session->transaction_log_command.empty( )
    && ( log_even_when_locked || handler.get_alternative_log_file( ) || !handler.get_is_locked_for_admin( ) ) )
   {
      string log_filename( handler.get_name( ) );

      log_filename += c_log_file_ext;

      bool is_new = false;

      if( !file_exists( log_filename ) )
         is_new = true;

      string extra_info_name( get_special_var_name( e_special_var_extra_field_values ) );
      string extra_field_values( get_session_variable( extra_info_name ) );

      if( !extra_field_values.empty( ) )
      {
         string::size_type len = extra_info_name.length( );
         string::size_type pos = gtp_session->transaction_log_command.find( extra_info_name );

         if( extra_field_values == "!" )
         {
            ++len; // i.e. to erase the comma as well
            extra_field_values.erase( );
         }

         if( pos != string::npos )
         {
            gtp_session->transaction_log_command.erase( pos, len );

            if( !extra_field_values.empty( ) )
               gtp_session->transaction_log_command.insert( pos, extra_field_values );
         }
      }

      vector< string > tx_log_lines;

      ofstream& log_file( handler.get_alternative_log_file( )
       ? *handler.get_alternative_log_file( ) : handler.get_log_file( ) );

      if( !log_file.is_open( ) )
         log_file.open( log_filename.c_str( ), ios::out | ios::app );

      if( is_new )
         log_file << c_storage_identity_tx_id << handler.get_root( ).identity << '\n';

      int32_t tx_id = 0;

      bool use_init_tx_id = false;

      string init_log_id( get_raw_session_variable( get_special_var_name( e_special_var_init_log_id ) ) );

      if( ( init_log_id == c_true ) || ( init_log_id == c_true_value ) )
         use_init_tx_id = true;

      string skip_persistence_name( get_special_var_name( e_special_var_skip_persistence ) );

      bool skip_persistence = has_session_variable( skip_persistence_name );

      // NOTE: When log file is truncated during a backup no transaction is active so
      // change the tx id to 'initial' to ensure that a restore is able to understand
      // it is dealing with a partial log file.
      if( use_tx_id )
         tx_id = use_tx_id;
      else if( load_module_id )
         tx_id = c_tx_id_module;
      else if( use_init_tx_id
       || !ods::instance( )->get_transaction_id( ) || handler.get_alternative_log_file( ) )
         tx_id = c_tx_id_initial;
      else
      {
         log_identity& identity( handler.get_root( ).log_id );

         // NOTE: If skipped persistence then will
         // simply use the last tx identity value.
         if( skip_persistence )
            tx_id = identity.next_id;
         else
         {

            if( identity.next_id == identity.ceiling )
            {
               identity.ceiling += c_identity_burn;

               ods_file_system ofs( *ods::instance( ) );

               // NOTE: Store the "ceiling" rather than "next_id" to avoid having to
               // perform extra I/O for every transaction. If storage termination is
               // to occur normally then the actual "next_id" will be written and no
               // identity value is lost. If for some reason normal termination does
               // not occur then up to the "burn" number of identities will be lost.
               ofs.store_as_text_file( c_storable_file_name_log_id, identity.ceiling );
            }

            tx_id = ++identity.next_id;
         }
      }

      string next_line;

      vector< string > lines;

      string prefix;

      if( skip_persistence )
      {
         prefix = ";";

         set_session_variable( skip_persistence_name, "" );
      }

      raw_split( gtp_session->transaction_log_command, lines, '\n' );

      for( size_t i = 0; i < lines.size( ); i++ )
      {
         next_line = "[" + to_string( tx_id ) + "]" + prefix + lines[ i ] + "\n";

         log_file << next_line;
      }

      bool is_restoring = handler.get_is_locked_for_admin( );

      if( !get_session_variable( get_special_var_name( e_special_var_package_install_extra ) ).empty( ) )
         is_restoring = true;

      log_file.flush( );

      if( !log_file.good( ) )
         throw runtime_error( "*** unexpected error occurred writing to transaction log ***" );
   }

   gtp_session->transaction_log_command.erase( );
}

void sid_hash( string& s )
{
   string sid;
   get_sid( sid );

   sha1 hash( sid );

   clear_key( sid );

   hash.get_digest_as_string( s );
}

void restore_saved_and_keep_as_older( const string& file_name )
{
   string saved_file( file_name + c_sav_file_ext );
   string older_file( file_name + c_old_file_ext );

   if( file_exists( saved_file ) )
   {
      if( file_exists( file_name ) )
         file_copy( file_name, older_file );

      file_copy( saved_file, file_name );

      file_remove( saved_file );
   }
}

struct script_info
{
   script_info( ) : check_lock_only( false ) { }

   string filename;
   string arguments;

   string lock_filename;
   bool check_lock_only;
};

time_t g_scripts_mod;

map< string, script_info > g_scripts;

bool scripts_file_has_changed( )
{
   bool changed = false;

   time_t t = 0;

   if( file_exists( c_manuscript_file ) )
      t = last_modification_time( c_manuscript_file );

   if( t != g_scripts_mod )
      changed = true;

   g_scripts_mod = t;

   return changed;
}

void read_script_info( )
{
   try
   {
      ifstream inpf( c_manuscript_file );

      g_scripts.clear( );

      if( inpf )
      {
         sio_reader reader( inpf );

         string name;

         while( reader.has_started_section( c_section_script ) )
         {
            script_info info;

            string name = reader.read_attribute( c_attribute_name );

            info.filename = reader.read_attribute( c_attribute_filename );

            if( !info.filename.empty( ) )
            {
               string::size_type pos = info.filename.find( ':' );

               if( pos != string::npos )
               {
                  info.lock_filename = info.filename.substr( pos + 1 );
                  info.filename.erase( pos );

                  if( !info.lock_filename.empty( ) )
                  {
                     if( info.lock_filename[ 0 ] == '?' )
                     {
                        info.check_lock_only = true;
                        info.lock_filename.erase( 0, 1 );
                     }
                  }
               }
            }

            info.arguments = reader.read_opt_attribute( c_attribute_arguments );

            g_scripts.insert( make_pair( name, info ) );

            reader.finish_section( c_section_script );
         }

         reader.verify_finished_sections( );
      }
   }
   catch( exception& x )
   {
      throw runtime_error( x.what( ) + string( " in " ) + string( c_manuscript_file ) );
   }
}

void output_script_info( const string& pat, ostream& os )
{
   for( map< string, script_info >::iterator i = g_scripts.begin( ); i!= g_scripts.end( ); ++i )
   {
      if( !pat.empty( ) && !wildcard_match( pat, i->first ) )
         continue;

      os << i->first;

      string args( i->second.arguments );

      if( i->second.filename == c_script_dummy_filename )
      {
         string script_name( i->first + ".cin" );

         if( args.find( script_name ) == 0 )
            args.erase( 0, script_name.length( ) );

         if( !args.empty( ) && args[ 0 ] == ' ' )
            args.erase( 0, 1 );
      }

      if( !args.empty( ) )
         os << ' ' << args;

      string lock_filename( i->second.lock_filename );

      if( !lock_filename.empty( ) && file_exists( lock_filename ) )
         os << " [ *** busy *** ]";

      os << '\n';
   }
}

void read_server_configuration( )
{
   restore_saved_and_keep_as_older( c_server_config_file );

   ifstream inpf( c_server_config_file );

   if( inpf )
   {
      sio_reader reader( inpf );

      g_domain = reader.read_opt_attribute( c_attribute_domain, c_local_host );

      g_use_udp = ( lower( reader.read_opt_attribute( c_attribute_use_udp, c_false ) ) == c_true );

      string ip_addrs( reader.read_opt_attribute( c_attribute_ip_addrs ) );

      if( !ip_addrs.empty( ) )
         split( ip_addrs, g_accepted_ip_addrs, ' ' );

      string na_addrs( reader.read_opt_attribute( c_attribute_na_addrs ) );

      if( !na_addrs.empty( ) )
         split( na_addrs, g_rejected_ip_addrs, ' ' );

      g_timezone = upper( reader.read_opt_attribute( c_attribute_timezone ) );

      g_web_root = reader.read_attribute( c_attribute_web_root );

      if( !g_web_root.empty( ) )
      {
         replace_quoted_environment_variables( g_web_root );
         replace_unquoted_environment_variables( g_web_root );
      }

      g_max_peers = atoi( reader.read_opt_attribute(
       c_attribute_max_peers, to_string( c_default_max_peers ) ).c_str( ) );

      g_use_https = ( lower( reader.read_opt_attribute( c_attribute_use_https, c_true ) ) == c_true );

      g_ntfy_server = reader.read_opt_attribute( c_attribute_ntfy_server );

      string trace_flags( reader.read_opt_attribute( c_attribute_trace_flags, c_default_trace_flags ) );

      // NOTE: Only set the trace flags if they
      // had not been set via the command-line.
      if( g_trace_flags == c_unset_trace_flags )
      {
         uint32_t flags = 0;

         istringstream isstr( trace_flags );

         isstr >> hex >> flags;

         set_trace_flags( flags );
      }

      g_max_sessions = atoi( reader.read_opt_attribute(
       c_attribute_max_sessions, to_string( c_max_sessions_default ) ).c_str( ) );

      if( g_max_sessions < c_min_sessions_limit )
         throw runtime_error( "max. sessions value " + to_string( g_max_sessions )
          + " is less than min. sessions limit " + to_string( c_min_sessions_limit ) );

      if( g_max_sessions > c_max_sessions_limit )
         throw runtime_error( "max. sessions value " + to_string( g_max_sessions )
          + " is greater than max. sessions limit " + to_string( c_max_sessions_limit ) );

      g_gpg_password = reader.read_opt_attribute( c_attribute_gpg_password );
      g_pem_password = reader.read_opt_attribute( c_attribute_pem_password );
      g_rpc_password = reader.read_opt_attribute( c_attribute_rpc_password );
      g_sql_password = reader.read_opt_attribute( c_attribute_sql_password );

      string log_files_dir( reader.read_opt_attribute( c_attribute_log_files_path ) );

      // NOTE: Don't override if was provided as a startup option.
      if( g_log_files_dir.empty( ) )
         set_log_files_dir( log_files_dir );

      set_system_variable( get_special_var_name( e_special_var_log_files_dir ), g_log_files_dir, true );

      int test_peer_port = atoi( reader.read_opt_attribute( c_attribute_test_peer_port, "0" ).c_str( ) );

      // NOTE: Don't override if was provided as a startup option.
      if( !g_test_peer_port )
         set_test_peer_port( test_peer_port );

      g_user_home_path = reader.read_attribute( c_attribute_user_home_path );

      if( !g_user_home_path.empty( ) )
      {
         replace_quoted_environment_variables( g_user_home_path );
         replace_unquoted_environment_variables( g_user_home_path );
      }

      string user_prefix( g_user_home_path );

      if( !user_prefix.empty( ) )
         user_prefix += '/';

      set_system_variable( get_special_var_name(
       e_special_var_backup_files ), user_prefix + c_server_folder_backup, true );

      set_system_variable( get_special_var_name(
       e_special_var_opened_files ), user_prefix + c_server_folder_opened, true );

      set_system_variable( get_special_var_name(
       e_special_var_shared_files ), user_prefix + c_server_folder_shared, true );

      g_default_storage = reader.read_opt_attribute( c_attribute_default_storage, c_meta_storage_name );
      set_system_variable( get_special_var_name( e_special_var_storage ), g_default_storage );

      string peer_ips_permit( reader.read_opt_attribute( c_attribute_peer_ips_permit ) );

      if( !peer_ips_permit.empty( ) )
         split( peer_ips_permit, g_accepted_peer_ip_addrs, ' ' );

      string peer_ips_reject( reader.read_opt_attribute( c_attribute_peer_ips_reject ) );

      if( !peer_ips_reject.empty( ) )
         split( peer_ips_reject, g_rejected_peer_ip_addrs, ' ' );

      g_script_reconfig = ( lower( reader.read_opt_attribute( c_attribute_script_reconfig, c_false ) ) == c_true );

      g_session_timeout = atoi( reader.read_opt_attribute( c_attribute_session_timeout, "0" ).c_str( ) );

      string protocol_handler( reader.read_opt_attribute( c_attribute_protocol_handler, c_default_protocol_handler ) );

      set_system_variable( get_special_var_name( e_special_var_protocol_handler ), protocol_handler );

      string system_variables( reader.read_opt_attribute( c_attribute_system_variables ) );

      if( !system_variables.empty( ) )
      {
         vector< string > sys_var_pairs;

         split( system_variables, sys_var_pairs );

         for( size_t i = 0; i < sys_var_pairs.size( ); i++ )
         {
            string next_pair( sys_var_pairs[ i ] );

            string::size_type pos = next_pair.find( '=' );

            if( pos != string::npos )
               set_system_variable( next_pair.substr( 0, pos ), next_pair.substr( pos + 1 ) );
         }
      }

      g_ods_use_encrypted = ( lower( reader.read_opt_attribute( c_attribute_ods_use_encrypted, c_true ) ) == c_true );

      g_ods_use_sync_write = ( lower( reader.read_opt_attribute( c_attribute_ods_use_sync_write, c_true ) ) == c_true );

      g_max_storage_handlers = atoi( reader.read_opt_attribute(
       c_attribute_max_storage_handlers, to_string( c_max_storage_handlers_default ) ).c_str( ) ) + 1;

      if( g_max_storage_handlers < c_min_storage_handlers_limit )
         throw runtime_error( "max. storage handlers value " + to_string( g_max_storage_handlers )
          + " is less than min. storage handlers limit " + to_string( c_min_storage_handlers_limit ) );

      if( g_max_storage_handlers > c_max_storage_handlers_limit )
         throw runtime_error( "max. storage handlers value " + to_string( g_max_storage_handlers )
          + " is greater than max. storage handlers limit " + to_string( c_max_storage_handlers_limit ) );

      g_notifier_ignore_secs = atoi( reader.read_opt_attribute(
       c_attribute_notifier_ignore_secs, to_string( c_notifier_ignore_secs_default ) ).c_str( ) );

      g_num_recv_stream_sessions = from_string< size_t >( reader.read_opt_attribute(
       c_attribute_num_recv_stream_sessions, to_string( c_num_recv_stream_sessions_default ) ) );

      reader.start_section( c_section_email );

      if( reader.has_started_section( c_section_mbox ) )
      {
         g_mbox_path = reader.read_attribute( c_attribute_path );
         g_mbox_username = reader.read_opt_attribute( c_attribute_username );

         reader.finish_section( c_section_mbox );
      }

      if( reader.has_started_section( c_section_pop3 ) )
      {
         g_pop3_server = reader.read_attribute( c_attribute_server );
         g_pop3_suffix = reader.read_attribute( c_attribute_suffix );
         g_pop3_username = reader.read_attribute( c_attribute_username );
         g_pop3_password = reader.read_opt_attribute( c_attribute_password );
         g_pop3_security = reader.read_opt_attribute( c_attribute_security );

         reader.finish_section( c_section_pop3 );
      }

      if( reader.has_started_section( c_section_smtp ) )
      {
         g_smtp_server = reader.read_attribute( c_attribute_server );
         g_smtp_sender = reader.read_opt_attribute( c_attribute_sender );
         g_smtp_suffix = reader.read_attribute( c_attribute_suffix );
         g_smtp_username = reader.read_attribute( c_attribute_username );
         g_smtp_password = reader.read_opt_attribute( c_attribute_password );
         g_smtp_security = reader.read_opt_attribute( c_attribute_security );

         if( g_smtp_sender.empty( ) )
            g_smtp_sender = g_smtp_username;

         string max_send_attempts = reader.read_opt_attribute( c_attribute_max_send_attempts );
         if( !max_send_attempts.empty( ) )
            g_smtp_max_send_attempts = atoi( max_send_attempts.c_str( ) );

         if( g_smtp_max_send_attempts < c_min_smtp_max_send_attempts )
            throw runtime_error( "max. smtp send attempts " + to_string( g_smtp_max_send_attempts )
             + " is less than min. smtp send attempts " + to_string( c_min_smtp_max_send_attempts ) );

         string max_attached_data = reader.read_opt_attribute( c_attribute_max_attached_data );
         if( !max_attached_data.empty( ) )
            g_smtp_max_attached_data = unformat_bytes( max_attached_data );

         if( g_smtp_max_attached_data < 0 )
            throw runtime_error( "invalid max. smtp attached data value " + to_string( g_smtp_max_attached_data ) );

         reader.finish_section( c_section_smtp );
      }

      reader.finish_section( c_section_email );

      reader.start_section( c_section_files );

      string files_area_dir( reader.read_opt_attribute( c_attribute_directory ) );

      // NOTE: Don't override if was provided as a startup option.
      if( !has_system_variable( get_special_var_name( e_special_var_files_area_dir ) ) )
         set_files_area_dir( files_area_dir );

      g_files_area_item_max_size = ( size_t )unformat_bytes( reader.read_opt_attribute(
       c_attribute_max_file_size, to_string( c_files_area_item_max_size_default ) ).c_str( ) );

      // NOTE: Use "unformat_bytes" here also so 100K (instead of 100000) can be used.
      g_files_area_item_max_num = ( size_t )unformat_bytes( reader.read_opt_attribute(
       c_attribute_max_num_files, to_string( c_files_area_item_max_num_default ) ).c_str( ) );

      reader.finish_section( c_section_files );

      if( reader.has_started_section( c_section_extern ) )
      {
         while( reader.has_started_section( c_section_client ) )
         {
            external_client client;

            client.port = atoi( reader.read_opt_attribute( c_attribute_port, "0" ).c_str( ) );

            string key = reader.read_attribute( c_attribute_label );
            client.is_local = ( lower( reader.read_opt_attribute( c_attribute_is_local, c_false ) ) == c_true );

            client.protocol = reader.read_attribute( c_attribute_protocol );
            client.extra_info = reader.read_opt_attribute( c_attribute_extra_info );
            client.script_name = reader.read_opt_attribute( c_attribute_script_name );

            reader.finish_section( c_section_client );

            g_external_client_info.insert( external_client_value_type( key, client ) );
         }

         reader.finish_section( c_section_extern );
      }

      reader.verify_finished_sections( );
   }
}

set< string > g_demo_identities;

void check_if_is_known_demo_identity( )
{
   if( g_demo_identities.empty( ) )
      buffer_file_lines( c_server_demo_identities_list, g_demo_identities );

   if( g_demo_identities.count( get_raw_system_variable(
    get_special_var_name( e_special_var_system_identity ) ) ) )
      set_system_variable( get_special_var_name( e_special_var_system_is_for_demo ), c_true_value );
}

void fetch_instance_from_row_cache( class_base& instance, bool skip_after_fetch )
{
   class_base_accessor instance_accessor( instance );

   if( instance_accessor.row_cache( ).empty( ) )
      throw runtime_error( "unexpected empty row cache" );

   instance_accessor.clear( );

   instance_accessor.set_key( instance_accessor.row_cache( )[ 0 ][ 0 ], true );

   instance_accessor.set_version( from_string< uint16_t >( instance_accessor.row_cache( )[ 0 ][ 1 ] ) );
   instance_accessor.set_revision( from_string< uint64_t >( instance_accessor.row_cache( )[ 0 ][ 2 ] ) );

   instance_accessor.set_security( from_string< uint64_t >( instance_accessor.row_cache( )[ 0 ][ 3 ] ) );

   instance_accessor.set_original_identity( instance_accessor.row_cache( )[ 0 ][ 4 ] );
   instance_accessor.set_original_revision( instance.get_revision( ) );

   if( instance.get_persistence_type( ) == 0 ) // i.e. SQL persistence
   {
      TRACE_LOG( TRACE_DETAILS | TRACE_QUERIES,
       "(row cache for '" + instance.get_class_id( ) + ")"
       + string( !skip_after_fetch ? "" : " *** skip_after_fetch ***" ) );

      const map< int, int >& fields( instance_accessor.select_fields( ) );
      const vector< int >& columns( instance_accessor.select_columns( ) );

      TRACE_LOG( TRACE_VERBOSE | TRACE_QUERIES, "(from row cache)" );

      for( int i = c_num_sys_field_names; i < instance_accessor.row_cache( )[ 0 ].size( ); i++ )
      {
         if( !fields.count( columns[ i - c_num_sys_field_names ] ) )
            throw runtime_error( "unexpected field # not found for column #" + to_string( i - c_num_sys_field_names ) );

         int fnum = fields.find( columns[ i - c_num_sys_field_names ] )->second;

         TRACE_LOG( TRACE_VERBOSE | TRACE_QUERIES,
          "setting field #" + to_string( fnum + 1 ) + " to " + instance_accessor.row_cache( )[ 0 ][ i ] );

         instance.set_field_value( fnum, instance_accessor.row_cache( )[ 0 ][ i ] );
      }
   }
   else
   {
      for( int i = c_num_sys_field_names; i < instance_accessor.row_cache( )[ 0 ].size( ); i++ )
      {
         if( instance_accessor.field_nums( ).size( ) < ( i - c_num_sys_field_names ) )
            throw runtime_error( "unexpected field # not found for for column #" + to_string( i - c_num_sys_field_names ) );

         int fnum = instance_accessor.field_nums( )[ i - c_num_sys_field_names ];

         instance.set_field_value( fnum, instance_accessor.row_cache( )[ 0 ][ i ] );
      }
   }

   instance_accessor.row_cache( ).pop_front( );

   instance_accessor.after_fetch_from_db( );

   if( !skip_after_fetch )
      instance_accessor.perform_after_fetch( );
}

string hex_level( uint32_t level )
{
   ostringstream osstr;

   osstr << setw( 5 ) << setfill( '0' ) << hex << level;

   return osstr.str( );
}

time_t g_timezones_mod;

bool timezones_file_has_changed( )
{
   bool changed = false;

   time_t t = 0;

   if( file_exists( c_timezones_file ) )
      t = last_modification_time( c_timezones_file );

   if( t != g_timezones_mod )
      changed = true;

   g_timezones_mod = t;

   return changed;
}

uint64_t g_log_num_messages = 0;

int64_t g_log_unix_check_time = 0;

// FUTURE: Could use system variables and/or server
// startup options in order to change the following.
bool g_log_milliseconds = false;
bool g_log_check_excessive = true;

set< string > g_trace_filters;

set< size_t > g_trace_session_ids;

// IMPORTANT: Any changes made to this function could
// prevent the "admin" account from using the FCGI UI
// (so will need to carefully check before changing).
void hash_sid_val( string& sid )
{
   sha256 hash( sid );

   hash.get_digest_as_string( sid );
}

}

uint32_t get_trace_flags( )
{
   return g_trace_flags;
}

void set_trace_flags( uint32_t flags )
{
   g_trace_flags = flags;
}

void trace_flags( uint32_t flags )
{
   set_trace_flags( flags );
}

void list_trace_flags( ostream& os )
{
   os << setw( 5 ) << setfill( '0' ) << hex << TRACE_GENERAL << ' ' << c_trace_flag_general;
   os << '\n';
   os << setw( 5 ) << setfill( '0' ) << hex << TRACE_LOCKING << ' ' << c_trace_flag_locking;
   os << '\n';
   os << setw( 5 ) << setfill( '0' ) << hex << TRACE_OBJECTS << ' ' << c_trace_flag_objects;
   os << '\n';
   os << setw( 5 ) << setfill( '0' ) << hex << TRACE_QUERIES << ' ' << c_trace_flag_queries;
   os << "\n";
   os << setw( 5 ) << setfill( '0' ) << hex << TRACE_SESSION << ' ' << c_trace_flag_session;
   os << '\n';
   os << setw( 5 ) << setfill( '0' ) << hex << TRACE_SOCKETS << ' ' << c_trace_flag_sockets;
   os << '\n';
   os << setw( 5 ) << setfill( '0' ) << hex << TRACE_VARIOUS << ' ' << c_trace_flag_various;
}

void trace_flag_names( const string& names, bool unset )
{
   uint32_t new_flags = 0;

   if( names == "all" )
      new_flags = TRACE_COLLAGE;
   else
   {
      vector< string > all_names;

      split( names, all_names );

      for( size_t i = 0; i < all_names.size( ); i++ )
      {
         string next_name( all_names[ i ] );

         if( next_name == c_trace_flag_general )
            new_flags |= TRACE_GENERAL;
         else if( next_name == c_trace_flag_locking )
            new_flags |= TRACE_LOCKING;
         else if( next_name == c_trace_flag_objects )
            new_flags |= TRACE_OBJECTS;
         else if( next_name == c_trace_flag_queries )
            new_flags |= TRACE_QUERIES;
         else if( next_name == c_trace_flag_session )
            new_flags |= TRACE_SESSION;
         else if( next_name == c_trace_flag_sockets )
            new_flags |= TRACE_SOCKETS;
         else if( next_name == c_trace_flag_various )
            new_flags |= TRACE_VARIOUS;
         else
            throw runtime_error( "invalid trace flag name '" + next_name + "'" );
      }
   }

   if( !unset )
      g_trace_flags |= new_flags;
   else
      g_trace_flags &= ~new_flags;
}

string get_trace_flag_names( )
{
   string retval( c_trace_flag_general );

   if( g_trace_flags & TRACE_LOCKING )
      retval += ',' + string( c_trace_flag_locking );

   if( g_trace_flags & TRACE_OBJECTS )
      retval += ',' + string( c_trace_flag_objects );

   if( g_trace_flags & TRACE_QUERIES )
      retval += ',' + string( c_trace_flag_queries );

   if( g_trace_flags & TRACE_SESSION )
      retval += ',' + string( c_trace_flag_session );

   if( g_trace_flags & TRACE_SOCKETS )
      retval += ',' + string( c_trace_flag_sockets );

   if( g_trace_flags & TRACE_VARIOUS )
      retval += ',' + string( c_trace_flag_various );

   return retval;
}

string get_trace_level( )
{
   string retval( strlen( c_trace_level_minimal ), '*' );

   if( g_trace_flags & TRACE_VERBOSE )
      retval = c_trace_level_verbose;
   else if( g_trace_flags & TRACE_DETAILS )
      retval = c_trace_level_details;
   else if( g_trace_flags & TRACE_INITIAL )
      retval = c_trace_level_initial;
   else if( g_trace_flags & TRACE_MINIMAL )
      retval = c_trace_level_minimal;

   return retval;
}

void set_trace_level( const string& level_name )
{
   uint32_t flags = ( g_trace_flags & TRACE_COLLAGE );

   flags |= TRACE_MINIMAL;

   if( level_name != c_trace_level_minimal )
   {
      flags |= TRACE_INITIAL;

      if( level_name != c_trace_level_initial )
      {
         flags |= TRACE_DETAILS;

         if( level_name != c_trace_level_details )
         {
            if( level_name != c_trace_level_verbose )
               throw runtime_error( "invalid trace level name '" + level_name + "'" );

            flags |= TRACE_VERBOSE;
         }
      }
   }

   set_trace_flags( flags );
}

void list_trace_levels( vector< string >& level_names )
{
   uint32_t level = 0;

   // NOTE: As "set_trace_level" switches on each prior level is doing
   // exactly the same with the hex values that are being output here.

   level |= TRACE_MINIMAL;
   level_names.push_back( hex_level( level ) + ' ' + string( c_trace_level_minimal ) );

   level |= TRACE_INITIAL;
   level_names.push_back( hex_level( level ) + ' ' + string( c_trace_level_initial ) );

   level |= TRACE_DETAILS;
   level_names.push_back( hex_level( level ) + ' ' + string( c_trace_level_details ) );

   level |= TRACE_VERBOSE;
   level_names.push_back( hex_level( level ) + ' ' + string( c_trace_level_verbose ) );
}

string get_trace_filters( )
{
   guard g( g_trace_mutex );

   string retval;

   for( set< string >::const_iterator ci = g_trace_filters.begin( ); ci != g_trace_filters.end( ); ++ci )
   {
      if( !retval.empty( ) )
         retval += ',';

      retval += to_string( *ci );
   }

   return retval;
}

void set_trace_filters( const string& filters )
{
   guard g( g_trace_mutex );

   if( filters.empty( ) )
      g_trace_filters.clear( );
   else
   {
      vector< string > all_filters;

      split( filters, all_filters );

      bool set_assign = false;

      string::size_type pos = filters.find_first_of( "+-" );

      // NOTE: For simplicity accepts "X,Y..." in order to
      // mean first clear the set then insert each filter.
      if( pos == string::npos )
      {
         set_assign = true;

         g_trace_filters.clear( );
      }

      for( size_t i = 0; i < all_filters.size( ); i++ )
      {
         string next( all_filters[ i ] );

         if( !next.empty( ) )
         {
            bool erase = false;
            bool insert = false;

            if( !set_assign )
            {
               if( next[ 0 ] == '-' )
                  erase = true;
               else if( next[ 0 ] == '+' )
                  insert = true;

               if( erase || insert )
                  next.erase( 0, 1 );
            }

            if( !next.empty( ) )
            {
               if( !erase && !insert && !set_assign )
                  g_trace_filters.clear( );

               if( erase )
                  g_trace_filters.erase( next );
               else
                  g_trace_filters.insert( next );
            }
         }
      }
   }
}

string get_trace_session_ids( )
{
   guard g( g_trace_mutex );

   string retval;

   for( set< size_t >::const_iterator ci = g_trace_session_ids.begin( ); ci != g_trace_session_ids.end( ); ++ci )
   {
      if( !retval.empty( ) )
         retval += ',';

      retval += to_string( *ci );
   }

   return retval;
}

void set_trace_session_ids( const string& ids )
{
   guard g( g_trace_mutex );

   if( ids.empty( ) )
      g_trace_session_ids.clear( );
   else
   {
      vector< string > all_ids;

      split( ids, all_ids );

      bool set_assign = false;

      string::size_type pos = ids.find_first_of( "+-" );

      // NOTE: For simplicity will accept "X,Y..." to
      // mean first clear the set then insert each id.
      if( pos == string::npos )
      {
         set_assign = true;

         g_trace_session_ids.clear( );
      }

      for( size_t i = 0; i < all_ids.size( ); i++ )
      {
         string next( all_ids[ i ] );

         if( !next.empty( ) )
         {
            bool erase = false;
            bool insert = false;

            if( !set_assign )
            {
               if( next[ 0 ] == '-' )
                  erase = true;
               else if( next[ 0 ] == '+' )
                  insert = true;

               if( erase || insert )
                  next.erase( 0, 1 );
            }

            size_t id = from_string< size_t >( next );

            // NOTE: If the id was empty then will
            // instead use the current session id.
            if( next.empty( ) && gtp_session )
               id = gtp_session->id;

            if( !erase && !insert && !set_assign )
               g_trace_session_ids.clear( );

            if( erase )
               g_trace_session_ids.erase( id );
            else
               g_trace_session_ids.insert( id );
         }
      }
   }
}

void log_trace_message( uint32_t flag, const string& message )
{
   guard g( g_trace_mutex );

   bool ignore = false;

   string prefix;

   if( flag != TRACE_MINIMAL )
   {
      if( !g_trace_filters.empty( ) )
      {
         for( set< string >::const_iterator ci = g_trace_filters.begin( ); ci != g_trace_filters.end( ); ++ci )
         {
            if( message.find( *ci ) != string::npos )
            {
               ignore = true;
               break;
            }
         }
      }

      if( !g_trace_session_ids.empty( ) )
      {
         size_t session_id = ( gtp_session ? gtp_session->id : 0 );

         if( !g_trace_session_ids.count( session_id ) )
            ignore = true;
      }

      if( flag & TRACE_INITIAL )
         prefix = " :";
      else if( flag & TRACE_DETAILS )
         prefix = " ::";
      else if( flag & TRACE_VERBOSE )
         prefix = " :::";

      flag &= ~TRACE_COLLAGE;
   }

   date_time now( date_time::local( ) );

   uint64_t unix_now = unix_time( now );

   if( g_log_check_excessive )
   {
      // NOTE: Resets the number of log messages counted towards being
      // potentially excessive every "cycle" seconds (thus the logging
      // pause only occurs if the excessive log message count has been
      // reached since the start of the last cycle). Although not very
      // sophisticated it will help prevent unexpected dramatic growth
      // of the server log file).
      if( ( unix_now - g_log_unix_check_time ) >= c_log_cycle_seconds )
      {
         g_log_num_messages = 0;
         g_log_unix_check_time = unix_now;
      }
      else if( !ignore )
      {
         if( ++g_log_num_messages >= c_log_num_excessive )
         {
            // NOTE: Sets "flag" to zero in order to ensure
            // a special "excessive logging" message can be
            // found in the log itself.
            if( g_log_num_messages == c_log_num_excessive )
               flag = 0;
            else
               ignore = true;
         }
      }
   }

   if( !ignore )
   {
      string type( c_trace_flag_general );

      switch( flag )
      {
         case TRACE_LOCKING:
         type = c_trace_flag_locking;
         break;

         case TRACE_OBJECTS:
         type = c_trace_flag_objects;
         break;

         case TRACE_QUERIES:
         type = c_trace_flag_queries;
         break;

         case TRACE_SESSION:
         type = c_trace_flag_session;
         break;

         case TRACE_SOCKETS:
         type = c_trace_flag_sockets;
         break;

         case TRACE_VARIOUS:
         type = c_trace_flag_various;
         break;
      }

      string log_file_dir( get_log_files_dir( ) );

      // NOTE: If no directory was provided
      // then will instead use the current.
      if( log_file_dir.empty( ) )
         log_file_dir = ".";

      ofstream outf( log_file_dir + '/' + c_server_log_file, ios::out | ios::app );

      string time_stamp( now.as_string( true, g_log_milliseconds ) );

      outf << '[' << time_stamp
       << "] [" << setw( 6 ) << setfill( '0' )
       << ( gtp_session ? gtp_session->id : 0 )
       << "] [" << type << "]" << prefix << ' ';

      if( flag )
         outf << message << '\n';
      else
         outf << "****** skipped excessive logging ******\n";
   }
}

void log_trace_string( uint32_t flag, const char* p_message )
{
   if( g_trace_flags & flag )
      log_trace_message( flag, p_message );
}

void trace_mutex::pre_acquire( const guard* p_guard, const char* p_msg )
{
   string extra;

   if( p_msg )
      extra = " " + string( p_msg );

   TRACE_LOG( TRACE_VERBOSE | TRACE_LOCKING, "pre_acquire: mutex = "
    + to_string( this ) + ", guard = " + to_string( p_guard ) + extra );
}

void trace_mutex::post_acquire( const guard* p_guard, const char* p_msg )
{
   string extra;

   if( p_msg )
      extra = " " + string( p_msg );

   TRACE_LOG( TRACE_VERBOSE | TRACE_LOCKING, "post_acquire: mutex = "
    + to_string( this ) + ", guard = " + to_string( p_guard ) + extra );
}

void trace_mutex::has_released( const guard* p_guard, const char* p_msg )
{
   string extra;

   if( p_msg )
      extra = " " + string( p_msg );

   TRACE_LOG( TRACE_VERBOSE | TRACE_LOCKING, "has_released: mutex = "
    + to_string( this ) + ", guard = " + to_string( p_guard ) + extra );
}

void dump_mutex_info( ostream& os, mutex& m, const char* p_mutex_name )
{
   if( p_mutex_name )
      os << p_mutex_name;

   os << m.get_lock_id( );

   const char* p_info = m.get_lock_info( );

   if( p_info )
      os << " (" << p_info << ")";

   os << '\n';
}

void list_trace_mutex_lock_ids( ostream& os, mutex* p_mutex, const char* p_mutex_name )
{
   dump_mutex_info( os, g_mutex, "ciyam_base::g_mutex = " );

   dump_mutex_info( os, get_mutex_for_class_base( ), "class_base::g_mutex = " );
   dump_mutex_info( os, get_mutex_for_ciyam_files( ), "ciyam_files::g_mutex = " );
   dump_mutex_info( os, get_mutex_for_peer_session( ), "peer_session::g_mutex = " );

   // NOTE: This is expected to be for "ciyam_session" (to support interactive testing).
   if( p_mutex )
   {
      if( p_mutex_name )
         os << p_mutex_name;

      os << p_mutex->get_lock_id( );

      const char* p_info = p_mutex->get_lock_info( );

      if( p_info )
         os << " (" << p_info << ")";

      os << '\n';
   }

   dump_mutex_info( os, get_mutex_for_ciyam_base_ops( ), "ciyam_base_ops::g_mutex = " );
   dump_mutex_info( os, get_mutex_for_ciyam_channels( ), "ciyam_channels::g_mutex = " );
   dump_mutex_info( os, get_mutex_for_ciyam_variables( ), "ciyam_variables::g_mutex = " );
   dump_mutex_info( os, get_mutex_for_ciyam_core_files( ), "ciyam_core_files::g_mutex = " );

   dump_mutex_info( os, g_mapping_mutex, "ciyam_base::g_mapping_mutex = " );
   dump_mutex_info( os, g_session_mutex, "ciyam_base::g_session_mutex = " );

   if( !p_mutex )
      os.flush( );
}

unsigned int get_server_port( )
{
   return g_server_port;
}

void set_server_port( unsigned int p )
{
   g_server_port = p;
}

unsigned int get_stream_port( )
{
   return g_stream_port;
}

unsigned int get_stream_sock( )
{
   return g_stream_sock;
}

void set_stream_socket( unsigned int p, unsigned int s )
{
   g_stream_port = p;
   g_stream_sock = s;
}

void register_listener( unsigned int port, const char* p_info, const char* p_id_info )
{
   g_listeners.insert( make_pair( port, p_info ) );

   if( p_id_info )
   {
      string ids( p_id_info );

      if( !ids.empty( ) )
      {
         vector< string > all_ids;
         split( ids, all_ids );

         for( size_t i = 0; i < all_ids.size( ); i++ )
            g_listener_ids.insert( make_pair( all_ids[ i ], port ) );
      }
   }
}

void unregister_listener( unsigned int port, const char* p_id_info )
{
   if( p_id_info )
   {
      string ids( p_id_info );

      if( !ids.empty( ) )
      {
         vector< string > all_ids;
         split( ids, all_ids );

         for( size_t i = 0; i < all_ids.size( ); i++ )
            g_listener_ids.erase( all_ids[ i ] );
      }
   }

   bool found = false;

   for( listener_id_const_iterator lidci = g_listener_ids.begin( ); lidci != g_listener_ids.end( ); ++lidci )
   {
      if( lidci->second == port )
      {
         found = true;
         break;
      }
   }

   if( !found )
      g_listeners.erase( port );
}

listener_registration::listener_registration( unsigned int port, const string& info, const char* p_id_info )
 :
 port( port )
{
   guard g( g_mutex );

   if( p_id_info )
      id_info = string( p_id_info );

   register_listener( port, info.c_str( ), p_id_info );
}

listener_registration::~listener_registration( )
{
   guard g( g_mutex );

   unregister_listener( port, id_info.c_str( ) );
}

void listener_registration::insert_id( const string& id )
{
   g_listener_ids.insert( make_pair( id, port ) );

   if( id_info.empty( ) )
      id_info = id;
   else
   {
      set< string > ids;
      split( id_info, ids );

      ids.insert( id );

      id_info.erase( );

      for( set< string >::iterator i = ids.begin( ); i != ids.end( ); ++i )
      {
         if( !id_info.empty( ) )
            id_info += ',';
         id_info += *i;
      }
   }
}

void listener_registration::remove_id( const string& id )
{
   if( !id_info.empty( ) )
   {
      g_listener_ids.erase( id );

      set< string > ids;
      split( id_info, ids );

      id_info.erase( );

      for( set< string >::iterator i = ids.begin( ); i != ids.end( ); ++i )
      {
         string next( *i );

         if( id != next )
         {
            if( !id_info.empty( ) )
               id_info += ',';
            id_info += next;
         }
      }
   }
}

bool has_registered_listener( unsigned int port )
{
   guard g( g_mutex );

   return g_listeners.count( port );
}

bool has_registered_listener_id( const string& id, unsigned int* p_port )
{
   guard g( g_mutex );

   bool retval = false;

   vector< string > ids;

   ids.push_back( id );

   get_extra_identities( id, ids );

   for( size_t i = 0; i < ids.size( ); i++ )
   {
      string next_id( ids[ i ] );

      if( g_listener_ids.count( next_id ) )
      {
         retval = true;

         if( p_port )
            *p_port = g_listener_ids[ next_id ];

         break;
      }
   }

   return retval;
}

void list_listeners( ostream& os )
{
   guard g( g_mutex );

   for( listener_const_iterator lci = g_listeners.begin( ); lci != g_listeners.end( ); ++lci )
   {
      string ids;

      for( listener_id_const_iterator lidci = g_listener_ids.begin( ); lidci != g_listener_ids.end( ); ++lidci )
      {
         if( lidci->second == lci->first )
         {
            if( !ids.empty( ) )
               ids += ',';
            else
               ids += " (";

            ids += lidci->first;
         }
      }

      if( !ids.empty( ) )
         ids += ')';

      os << lci->first << ' ' << lci->second << ids << '\n';
   }
}

void init_globals( const char* p_sid, int* p_use_udp )
{
   guard g( g_mutex );

   // NOTE: Forces special variables to be populated then verify that final name is "dummy" as would be expected.
   if( get_special_var_name( e_special_var_NOTE_THIS_MUST_ALWAYS_BE_THE_LAST_ENUM_FOR_VERIFICATION ) != c_dummy )
      throw runtime_error( "unexpected special variable names have not been correctly populated" );

   try
   {
      string sid;
      sid.reserve( c_key_reserve_size );

      if( p_sid && strlen( p_sid ) )
         sid = string( p_sid );
      else if( file_exists( c_server_sid_file ) )
         buffer_file( sid, c_server_sid_file );

      g_sid.reserve( c_key_reserve_size );

      if( !sid.empty( ) && ( sid.find( ':' ) == string::npos ) )
         hash_sid_val( sid );

      set_sid( sid );
      clear_key( sid );

      has_identity( &g_encrypted_identity );

      g_secure_identity = g_encrypted_identity;

      if( g_encrypted_identity )
      {
         set_system_variable( get_special_var_name( e_special_var_sid_locked ), c_true_value, true );
         set_system_variable( get_special_var_name( e_special_var_sid_secure ), c_true_value, true );
      }

      read_server_configuration( );

      // NOTE: Remember special read only variable names for later checks.
      g_read_only_var_names.insert( get_special_var_name( e_special_var_tls ) );
      g_read_only_var_names.insert( get_special_var_name( e_special_var_slot ) );
      g_read_only_var_names.insert( get_special_var_name( e_special_var_uuid ) );
      g_read_only_var_names.insert( get_special_var_name( e_special_var_pubkey ) );
      g_read_only_var_names.insert( get_special_var_name( e_special_var_ip_addr ) );
      g_read_only_var_names.insert( get_special_var_name( e_special_var_session_id ) );

      if( p_use_udp )
         *p_use_udp = g_use_udp;

      if( file_exists( c_at_init_script ) )
      {
         int rc = system( c_at_init_script );
         ( void )rc;
      }

      init_files_area( );

      bool restored = false;

      init_system_ods( &restored );

      init_archive_info( 0, restored );

      if( restored )
      {
         while( true )
         {
            string next_info( get_raw_system_variable(
             get_special_var_name( e_special_var_queue_archive_info_for_resize ) ) );

            if( next_info.empty( ) )
               break;

            vector< string > columns;

            split( next_info, columns );

            if( columns.size( ) != 2 )
               throw runtime_error( "unexpected queued archive for repair info found in init_globals" );

            string archive( columns[ 0 ] );

            int64_t bytes_used = from_string< int64_t >( columns[ 1 ] );

            resize_file_archive( archive, 0, 0, &bytes_used );
         }
      }

      check_timezone_info( );

      set_system_variable( get_special_var_name( e_special_var_os ), "Linux", true );

      set_system_variable(
       get_special_var_name( e_special_var_peer_port ), to_string( c_default_ciyam_peer_port ), true );

      string identity( c_str_unknown );

      string blockchain_backup_identity( get_raw_system_variable(
       get_special_var_name( e_special_var_blockchain_backup_identity ) ) );

      if( !blockchain_backup_identity.empty( ) )
         identity = g_identity_suffix = blockchain_backup_identity;
      else if( !g_sid.empty( ) && !g_encrypted_identity )
         identity = get_identity( ).substr( 0, c_identity_length );

      set_system_variable( get_special_var_name( e_special_var_system_identity ), identity, true );

      check_if_is_known_demo_identity( );

      restore_saved_and_keep_as_older( c_autoscript_file );
      restore_saved_and_keep_as_older( c_manuscript_file );

      // NOTE: The manuscript info doesn't actually need to be read until a script is attempted
      // to be run, however, it is been read at startup just to ensure that the .sio file isn't
      // initially malformed.
      if( scripts_file_has_changed( ) )
         read_script_info( );

      // NOTE: This "get_tz_info" call is performed to verify that the server timezone is valid.
      float offset = 0.0;

      string tz_name( get_timezone( ) );

      date_time now( date_time::standard( ) );

      get_tz_info( now, tz_name, offset );

      read_strings( "ciyam_strings.txt", g_strings, "c_str_" );
      read_strings( "module_strings.txt", g_strings, "c_str_" );

      for( size_t i = 0; i < g_max_sessions; i++ )
         g_sessions.push_back( 0 );

      for( size_t i = 0; i < g_max_storage_handlers; i++ )
         g_storage_handlers.push_back( 0 );

      g_storage_handlers[ 0 ] = new storage_handler( 0, c_default_storage_name );

      g_storage_handlers[ 0 ]->get_root( ).identity = c_default_storage_identity;

      g_storage_handler_index.insert( make_pair( c_default_storage_name, 0 ) );

#ifdef SSL_SUPPORT
      if( !file_exists( c_ciyam_pem ) )
      {
         string cmd( "./create_ciyam_pem" );

         int rc = system( cmd.c_str( ) );
         ( void )rc;
      }

      if( file_exists( c_ciyam_pem ) )
      {
         // NOTE: If using an encrypted PEM then the FCGI UI
         // would fail its "init_ssl" call so until this has
         // been resolved it is only recommended for systems
         // that do not use the FCGI UI.
         if( g_pem_password.empty( ) )
            init_ssl( c_ciyam_pem, 0, 0, true );
         else
            init_ssl( c_ciyam_pem, g_pem_password.c_str( ), 0, true );

         g_using_ssl = true;
      }
#endif

      if( !g_web_root.empty( ) )
      {
         // NOTE: If completed restore now remove the FCGI UI stop file.
         string ui_stop_file( g_web_root + '/' + c_ciyam_ui_stop_file );

         if( file_exists( ui_stop_file ) )
            file_remove( ui_stop_file );
      }
   }
   catch( exception& x )
   {
      TRACE_LOG( TRACE_MINIMAL, x.what( ) );
      throw;
   }
}

void term_globals( )
{
   guard g( g_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] )
      {
         delete g_sessions[ i ];

         g_sessions[ i ] = 0;
      }
   }

   for( size_t i = 0; i < g_max_storage_handlers; i++ )
   {
      if( g_storage_handlers[ i ] )
      {
         delete g_storage_handlers[ i ];

         g_storage_handlers[ i ] = 0;
      }
   }

   term_system_ods( );
   term_files_area( );

   if( file_exists( c_at_term_script ) )
   {
      int rc = system( c_at_term_script );
      ( void )rc;
   }

   // NOTE: Need to force the manuscript information to be reloaded.
   g_scripts_mod = 0;

   g_scripts.clear( );

   g_strings.clear( );

#ifdef SSL_SUPPORT
   if( g_using_ssl )
      term_ssl( );
#endif
}

void resync_system_ods( progress* p_progress )
{
   ( void )p_progress;

   // NOTE: These calls are required when the files area directory
   // is changed as the server ODS DB is stored in the files area.
   term_system_ods( );

   init_system_ods( );
}

void check_timezone_info( )
{
   if( timezones_file_has_changed( ) )
      setup_time_zones( );
}

void list_strings( ostream& os )
{
   for( string_const_iterator sci = g_strings.begin( ); sci != g_strings.end( ); ++sci )
      os << sci->first.substr( strlen( "c_str_" ) ) << ' ' << sci->second << '\n';
}

string get_string( const string& key, const char* p_default )
{
   string str( !p_default ? key : p_default );

   string_const_iterator sci = g_strings.find( key );

   if( sci != g_strings.end( ) )
      str = sci->second;

   return str;
}

string get_string_message( const string& string_message, const pair< string, string >& parm1 )
{
   string message;

   string::size_type pos = string_message.find( parm1.first );

   if( pos == string::npos )
      throw runtime_error( "parameter '" + parm1.first + "' not found in '" + string_message + "'" );

   message = string_message.substr( 0, pos );

   message += parm1.second;

   message += string_message.substr( pos + parm1.first.length( ) );

   return message;
}

string get_string_message( const string& string_message,
 const pair< string, string >& parm1, const pair< string, string >& parm2 )
{
   string message( get_string_message( string_message, parm1 ) );

   message = get_string_message( message, parm2 );

   return message;
}

string get_string_message( const string& string_message,
 const pair< string, string >& parm1, const pair< string, string >& parm2,
 const pair< string, string >& parm3 )
{
   string message( get_string_message( string_message, parm1, parm2 ) );

   message = get_string_message( message, parm3 );

   return message;
}

string get_string_message( const string& string_message,
 const pair< string, string >& parm1, const pair< string, string >& parm2,
 const pair< string, string >& parm3, const pair< string, string >& parm4 )
{
   string message( get_string_message( string_message, parm1, parm2, parm3 ) );

   message = get_string_message( message, parm4 );

   return message;
}

unsigned int get_max_peers( )
{
   return g_max_peers;
}

unsigned int get_max_user_limit( )
{
   guard g( g_mutex );

   return g_max_user_limit;
}

void set_max_user_limit( unsigned int new_limit )
{
   guard g( g_mutex );

   g_max_user_limit = new_limit;
}

string get_prefix( )
{
   return g_use_https ? "https" : "http";
}

string get_domain( )
{
   return g_domain;
}

string get_app_url( const string& suffix )
{
   string url( get_prefix( ) );

   url += "://";

   url += get_domain( );

   url += "/" + lower( storage_name( ) );

   if( !suffix.empty( ) )
      url += "/" + suffix;

   return url;
}

void get_identity( string& s,
 bool append_max_user_limit, bool use_max_sid_entropy, const char* p_pubkey )
{
   guard g( g_mutex );

   s.reserve( c_key_reserve_size );

   string sid;

   get_sid( sid );

   bool encrypted = ( sid.find( ':' ) != string::npos );

   if( !sid.empty( ) )
   {
      if( encrypted || !use_max_sid_entropy )
         sid_hash( s );
      else
      {
         // NOTE: Unless the identity had been hardened
         // then will shrink the size (as assuming only
         // twelve mnemonics were used then the entropy
         // was only 128 bits in the first place).
         if( g_hardened_identity )
            s = sid;
         else
            s = sid.substr( 0, c_sha1_digest_size * 2 );
      }
   }
   else
   {
      string seed;
      get_mnemonics_or_hex_seed( s, seed );

      if( p_pubkey )
         session_shared_encrypt( s, p_pubkey, s );
   }

   if( append_max_user_limit )
      s += "!" + to_string( g_max_user_limit );

   if( p_pubkey && !encrypted )
   {
#ifndef SSL_SUPPORT
      s += ' ' + sid;
#else
      string data;
      session_shared_encrypt( data, p_pubkey, sid );

      s += ' ' + data;
#endif
   }

   clear_key( sid );
}

bool has_identity( bool* p_is_encrypted )
{
   guard g( g_mutex );

   string sid;

   get_sid( sid );

   bool retval = !sid.empty( );

   if( p_is_encrypted )
   {
      if( sid.find( ':' ) != string::npos )
         *p_is_encrypted = true;
      else
         *p_is_encrypted = false;
   }

   clear_key( sid );

   return retval;
}

void set_identity( const string& info, const char* p_encrypted_sid )
{
   bool run_init_script = false;

   // NOTE: Empty code block for scope purposes.
   {
      guard g( g_mutex );

      bool is_encrypted = false;

      string sid;
      sid.reserve( c_key_reserve_size );
 
      get_sid( sid );

      if( sid.find( ':' ) != string::npos )
         is_encrypted = true;

      // NOTE: Checks for an encrypted identity.
      if( info.length( ) >= c_encrypted_length )
      {
         sid = info;

         if( info.find( ':' ) != string::npos )
         {
            g_hardened_identity = false;
            g_encrypted_identity = true;
         }
         else if( is_encrypted && p_encrypted_sid )
         {
            g_secure_identity = false;
            g_hardened_identity = false;
            g_encrypted_identity = false;

            temp_umask tum( 077 );

            write_file( c_server_sid_file, ( unsigned char* )info.c_str( ), info.length( ) );

            hash_sid_val( sid );

            set_system_variable( get_special_var_name( e_special_var_sid_locked ), "", true );
            set_system_variable( get_special_var_name( e_special_var_sid_secure ), "", true );

            if( get_system_variable( get_special_var_name( e_special_var_blockchain_backup_identity ) ).empty( ) )
               run_init_script = true;
         }

         set_sid( sid );
      }
      else if( is_encrypted )
      {
         string encrypted( sid );

         string key;
         key.reserve( c_key_reserve_size );

         key = info;

         bool was_key_unlocked = false;
         bool possible_unlock_key = false;

         string key_file_name;

         // NOTE: Unlock keys are expected to be base64 URL
         // characters in three groups of five (using space
         // separators).
         if( ( key.length( ) == c_unlock_ext_key_length )
           && ( ( key[ 5 ] == ' ' ) && ( key[ 11 ] == ' ' ) ) )
         {
            possible_unlock_key = true;

            key.erase( 11, 1 );
            key.erase( 5, 1 );

            if( base64::valid_characters( key, true ) )
               key = base64::decode( key, true );
            else
            {
               key = info;
               possible_unlock_key = false;
            }
         }

         harden_key_with_hash_rounds( key, key, key,
          c_key_rounds_multiplier * ( !possible_unlock_key ? 1 : c_unlock_key_multiplier ) );

         if( possible_unlock_key )
         {
            key_file_name = key.substr( c_unlock_file_name_start, c_unlock_file_name_length ) + c_key_suffix;

            if( file_exists( key_file_name ) )
            {
               was_key_unlocked = true;

               sid = buffer_file( key_file_name );
            }
            else
            {
               key = info;

               // NOTE: If not actually a key then needs to harden as per normal.
               harden_key_with_hash_rounds( key, key, key, c_key_rounds_multiplier );
            }
         }

         data_decrypt( sid, sid, key );

         size_t num_spaces = count( sid.begin( ), sid.end( ), ' ' );

         string extra;

         // If there are additional spaces then treat all additional words
         // as being additional entropy (rather than validated mnemonics).
         if( num_spaces > 11 )
         {
            string::size_type pos = 0;

            for( size_t n = 0; n < 12; n++ )
               pos = sid.find( ' ', pos + 1 );

            extra = sid.substr( pos + 1 );

            num_spaces = 11;

            sid.erase( pos );
         }

         if( num_spaces == 11 )
         {
            try
            {
               get_mnemonics_or_hex_seed( sid, sid );
            }
            catch( ... )
            {
               g_sid.erase( );

               throw;
            }
         }

         if( are_hex_nibbles( sid ) )
         {
            // NOTE: Either now simply store the hash of the entropy or if any extra
            // entropy had been supplied then use this in order to harden the value.
            if( extra.empty( ) )
            {
               if( !was_key_unlocked )
                  hash_sid_val( sid );
               else
               {
                  file_remove( key_file_name );

                  if( !g_default_storage.empty( ) )
                  {
                     string web_app_dir( get_web_root( ) + '/' + lower( g_default_storage ) );

                     if( dir_exists( web_app_dir ) )
                        file_touch( web_app_dir + '/' + c_unlocked_with_key_file, 0, true );
                  }
               }
            }
            else
            {
               g_hardened_identity = true;

               harden_key_with_hash_rounds( sid, info, extra, c_key_rounds_multiplier );
            }

            set_sid( sid );

            set_system_variable( get_special_var_name( e_special_var_sid_locked ), "", true );

            if( get_raw_system_variable(
             get_special_var_name( e_special_var_blockchain_backup_identity ) ).empty( ) )
            {
               string identity( get_identity( ) );

               g_identity_suffix = identity.substr( 0, c_bc_identity_length );

               set_system_variable( get_special_var_name( e_special_var_system_identity ), g_identity_suffix, true );
            }
         }
      }

      string extra;

      if( ( sid == c_demo_sid_1 ) || ( sid == c_demo_sid_2 ) )
         extra = "-demo ";

      clear_key( sid );

      if( p_encrypted_sid && g_encrypted_identity )
      {
         string user( get_environment_variable( c_env_var_ciyam_user ) );

         temp_umask tum( 077 );

         if( !user.empty( ) )
         {
            string cmd;

            // NOTE: The ".<usser>_password_protected" file name
            // is also found in the "set_password" bash script.
            if( file_exists( "." + user + "_password_protected" ) )
            {
               if( !file_exists( c_server_sid_file ) )
                  cmd = "./resize_fs_img_files " + extra + "\"" + user + "\"";
            }
            else
            {
               write_file( c_password_info_file, info );

               cmd = "./set_password " + extra + "\"" + user + "\" " + string( c_password_info_file );
            }

            if( !cmd.empty( ) )
            {
               int rc = system( cmd.c_str( ) );
               ( void )rc;
            }
         }

         write_file( c_server_sid_file, ( unsigned char* )p_encrypted_sid, strlen( p_encrypted_sid ) );

         set_system_variable( get_special_var_name( e_special_var_sid_secure ), c_true_value, true );

         if( get_system_variable( get_special_var_name( e_special_var_blockchain_backup_identity ) ).empty( ) )
            run_init_script = true;
      }
   }

   if( run_init_script )
   {
      run_script( "init_ciyam", false );

      string blockchain_backup_identity( get_raw_system_variable(
       get_special_var_name( e_special_var_blockchain_backup_identity ) ) );

      if( !blockchain_backup_identity.empty( ) )
      {
         g_identity_suffix = blockchain_backup_identity;

         set_system_variable( get_special_var_name(
          e_special_var_system_identity ), blockchain_backup_identity, true );
      }

      check_if_is_known_demo_identity( );
   }
}

bool unlock_create_allowed( )
{
   guard g( g_mutex );

   bool retval = false;

   int64_t now = unix_time( );

   if( !g_key_unlock_tm_val || ( abs( now - g_key_unlock_tm_val ) >= 3 ) )
      retval = true;

   return retval;
}

string create_unlock_sid_hash_key( bool for_web_ui )
{
   guard g( g_mutex );

   if( !unlock_create_allowed( ) )
      throw runtime_error( "*** attempt to create another unlock key too quickly ***" );

   if( !has_system_variable( get_special_var_name( e_special_var_sid_secure ) ) )
      throw runtime_error( "invalid attempt to create unlock key for unencrypted identity" );

   string unlock_key( random_characters( c_unlock_key_length, 0, e_printable_type_alpha_numeric ) );

   string key;
   key.reserve( c_key_reserve_size );

   key = unlock_key;

   // NOTE: As the key uses only alpha numerics increase the number of hash rounds multiplier.
   harden_key_with_hash_rounds( key, key, key, c_key_rounds_multiplier * c_unlock_key_multiplier );

   // NOTE: The key file name is obtained from the hash (after all rounds) to ensure that
   // having access to the file system does not make reversing the identity hash straight
   // forward (assuming that the relevant key value is not known).
   string key_file_name( key.substr( c_unlock_file_name_start, c_unlock_file_name_length ) + c_key_suffix );

   string sid_hash;
   sid_hash.reserve( c_key_reserve_size );

   get_sid( sid_hash );

   data_encrypt( sid_hash, sid_hash, key );

   temp_umask tum( 077 );

   ofstream outf( key_file_name.c_str( ) );

   outf << sid_hash;

   outf.flush( );

   if( !outf.good( ) )
      throw runtime_error( "unexpected error occurred writing to unlock key file" );

   outf.close( );

   g_key_unlock_tm_val = unix_time( );

   unlock_key = base64::encode( unlock_key, true );

   if( unlock_key.size( ) != 15 )
      throw runtime_error( "unexpected b64 encoded unlock key size != 15" );

   unlock_key = unlock_key.substr( 0, 5 ) + ' ' + unlock_key.substr( 5, 5 ) + ' ' + unlock_key.substr( 10 );

   if( !for_web_ui )
      return unlock_key;
   else
      return get_string_message( GS( c_str_copy_unlock_key_to_clipboard ),
       make_pair( c_str_copy_unlock_key_to_clipboard_value, unlock_key ) );
}

string get_checksum( const string& data )
{
   guard g( g_mutex );

   string sid;

   get_sid( sid );

   sha1 hash( sid + data );

   vector< string > sha1_quads;
   split( hash.get_digest_as_string( ',' ), sha1_quads );

   if( sha1_quads.size( ) != 5 )
      throw runtime_error( "unexpected hash result" );

   return sha1_quads[ 2 ];
}

string get_timezone( )
{
   string tz_name( g_timezone );

   if( tz_name.empty( ) )
      tz_name = "UTC";

   return g_timezone;
}

string get_web_root( )
{
   return g_web_root;
}

string get_ntfy_server( )
{
   return g_ntfy_server;
}

bool get_is_accepted_ip_addr( const string& ip_addr )
{
   guard g( g_mutex );

   if( has_raw_system_variable( get_special_var_name( e_special_var_disallow_connections ) ) )
      return false;

   return ( g_rejected_ip_addrs.empty( ) || g_rejected_ip_addrs.count( ip_addr ) == 0 )
    && ( g_accepted_ip_addrs.empty( ) || ( g_accepted_ip_addrs.count( ip_addr ) > 0 ) );
}

bool get_is_accepted_peer_ip_addr( const string& ip_addr )
{
   guard g( g_mutex );

   if( has_raw_system_variable( get_special_var_name( e_special_var_disallow_connections ) ) )
      return false;

   return ( g_rejected_peer_ip_addrs.empty( ) || g_rejected_peer_ip_addrs.count( ip_addr ) == 0 )
    && ( g_accepted_peer_ip_addrs.empty( ) || ( g_accepted_peer_ip_addrs.count( ip_addr ) > 0 ) );
}

int is_accepted_ip_addr( const char* p_addr )
{
   return get_is_accepted_ip_addr( p_addr );
}

void add_peer_ip_addr_for_rejection( const string& ip_addr )
{
   guard g( g_mutex );

   g_rejected_peer_ip_addrs.insert( ip_addr );
}

void remove_peer_ip_addr_from_rejection( const string& ip_addr )
{
   guard g( g_mutex );

   g_rejected_peer_ip_addrs.erase( ip_addr );
}

string list_peer_ip_addrs_for_rejection( )
{
   guard g( g_mutex );

   string retval;

   for( set< string >::iterator
    i = g_rejected_peer_ip_addrs.begin( ); i != g_rejected_peer_ip_addrs.end( ); ++i )
   {
      if( !retval.empty( ) )
         retval += '\n';

      retval += *i;
   }

   return retval;
}

string get_non_extra_identity( const string& extra_identity )
{
   string extra, retval( extra_identity );

   string blockchain_backup_prefix, blockchain_backup_suffix;
   string blockchain_shared_prefix, blockchain_shared_suffix;

   string blockchain_backup_identity_name( get_special_var_name( e_special_var_blockchain_backup_identity ) );
   string blockchain_shared_identity_name( get_special_var_name( e_special_var_blockchain_shared_identity ) );

   string backup_identity( get_system_variable( blockchain_backup_identity_name ) );
   string shared_identity( get_system_variable( blockchain_shared_identity_name ) );

   identity_variable_name_prefix_and_suffix(
    blockchain_backup_identity_name, blockchain_backup_prefix, blockchain_backup_suffix );

   identity_variable_name_prefix_and_suffix(
    blockchain_shared_identity_name, blockchain_shared_prefix, blockchain_shared_suffix );

   extra = get_identity_variable_extra( blockchain_backup_identity_name, extra_identity );

   if( !extra.empty( ) )
      retval = backup_identity;
   else
   {
      extra = get_identity_variable_extra( blockchain_shared_identity_name, extra_identity );

      if( !extra.empty( ) )
         retval = shared_identity;
   }

   return retval;
}

void get_extra_identities( const string& identity, vector< string >& extras )
{
   string blockchain_backup_identity_name( get_special_var_name( e_special_var_blockchain_backup_identity ) );
   string blockchain_shared_identity_name( get_special_var_name( e_special_var_blockchain_shared_identity ) );
   string blockchain_peer_hub_identity_name( get_special_var_name( e_special_var_blockchain_peer_hub_identity ) );

   string variable_name_prefix, variable_name_suffix;

   if( identity == get_system_variable( blockchain_backup_identity_name ) )
      identity_variable_name_prefix_and_suffix( blockchain_backup_identity_name, variable_name_prefix, variable_name_suffix );
   else if( identity == get_system_variable( blockchain_shared_identity_name ) )
      identity_variable_name_prefix_and_suffix( blockchain_shared_identity_name, variable_name_prefix, variable_name_suffix );
   else if( identity == get_system_variable( blockchain_peer_hub_identity_name ) )
      identity_variable_name_prefix_and_suffix( blockchain_peer_hub_identity_name, variable_name_prefix, variable_name_suffix );

   if( !variable_name_prefix.empty( ) )
   {
      for( size_t i = 1; i <= c_max_extras; i++ )
      {
         string next_extra( get_system_variable( variable_name_prefix + to_string( i ) + variable_name_suffix ) );

         if( next_extra.empty( ) )
            break;

         extras.push_back( next_extra );
      }
   }
}

string get_extra_identity_variable( const string& identity_variable_name, const string& extra )
{
   string variable_name_prefix, variable_name_suffix;

   identity_variable_name_prefix_and_suffix( identity_variable_name, variable_name_prefix, variable_name_suffix );

   return get_system_variable( variable_name_prefix + extra + variable_name_suffix );
}

string get_identity_variable_extra( const string& identity_variable_name, const string& identity_value, bool force_extra_match )
{
   string extra, closest, variable_name_prefix, variable_name_suffix;

   identity_variable_name_prefix_and_suffix( identity_variable_name, variable_name_prefix, variable_name_suffix );

   for( size_t i = 1; i <= c_max_extras; i++ )
   {
      string next_extra( get_system_variable( variable_name_prefix + to_string( i ) + variable_name_suffix ) );

      if( next_extra == identity_value )
      {
         extra = to_string( i );

         break;
      }
      else if( force_extra_match && !next_extra.empty( ) )
      {
         if( closest.empty( ) || ( identity_value > closest ) )
         {
            closest = next_extra;
            extra = to_string( i );
         }
      }
   }

   return extra;
}

void identity_variable_name_prefix_and_suffix(
 const string& identity_variable_name, string& prefix, string& suffix )
{
   string::size_type pos = identity_variable_name.rfind( c_identity_suffix );

   if( ( pos == 0 ) || ( pos == string::npos ) )
      throw runtime_error( "unexpected '" + identity_variable_name + "' does not contain suffix '" + string( c_identity_suffix ) + "'" );

   prefix = identity_variable_name.substr( 0, pos );
   suffix = identity_variable_name.substr( pos - 1 );
}

void create_peerchain(
 const string& identity, const string& host_and_port, const string& description,
 const string& extra_value, const string& shared_secret, bool auto_start, size_t num_helpers, int peer_type )
{
   system_ods_fs_guard ods_fs_guard;

   system_ods_bulk_write ods_bulk_write;

   gup_ofs->set_root_folder( c_system_peerchain_folder );

   if( gup_ofs->has_file( identity ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Peerchain '" + identity + "' already exists." );

   int port = 0;

   string host_name;

   parse_host_and_or_port( host_and_port, host_name, port );

   string peer_port( get_raw_system_variable( get_special_var_name( e_special_var_peer_port ) ) );

   if( peer_port.empty( ) )
      throw runtime_error( "unexpect missing 'peer_port' system variable" );

   if( ( peer_type == c_peerchain_type_hub )
    || ( peer_type == c_peerchain_type_local_only ) )
   {
      port = 0;
      num_helpers = 0;
   }

   if( !port
    && ( ( peer_type == c_peerchain_type_combined )
    || ( peer_type == c_peerchain_type_backup_only )
    || ( peer_type == c_peerchain_type_shared_only ) ) )
      port = from_string< int >( peer_port );

   if( host_name.empty( )
    || ( peer_type == c_peerchain_type_hub )
    || ( peer_type == c_peerchain_type_local_only ) )
      host_name = c_local_host;

   if( ( host_name == c_local_host )
    && ( peer_type != c_peerchain_type_hub )
    && ( peer_type != c_peerchain_type_local_only ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Peerchain '" + identity + "' host name '" + host_name + "' is invalid." );

   stringstream sio_data;

   sio_writer writer( sio_data );

   writer.write_attribute( c_peerchain_attribute_auto_start, to_string( auto_start ) );
   writer.write_attribute( c_peerchain_attribute_description, description );
   writer.write_attribute( c_peerchain_attribute_extra_value, extra_value );
   writer.write_attribute( c_peerchain_attribute_host_name, host_name );
   writer.write_attribute( c_peerchain_attribute_host_port, to_string( port ) );
   writer.write_attribute( c_peerchain_attribute_local_port, peer_port );
   writer.write_attribute( c_peerchain_attribute_num_helpers, to_string( num_helpers ) );
   writer.write_attribute( c_peerchain_attribute_peer_type, to_string( peer_type ) );
   writer.write_attribute( c_peerchain_attribute_shared_secret, shared_secret );

   writer.finish_sections( );

   gup_ofs->store_file( identity, 0, &sio_data );
}

void review_peerchain( ostream& os, const string& identity, const string& attribute )
{
   system_ods_fs_guard ods_fs_guard;

   system_ods_bulk_read ods_bulk_read;

   gup_ofs->set_root_folder( c_system_peerchain_folder );

   if( !identity.empty( ) )
   {
      if( !gup_ofs->has_file( identity ) )
         throw runtime_error( "peerchain '" + identity + "' was not found" );

      if( attribute.empty( ) )
         gup_ofs->get_file( identity, "", &os );
      else
      {
         ostringstream osstr;

         gup_ofs->get_file( identity, "", &osstr );

         string lines( osstr.str( ) );

         if( !lines.empty( ) )
         {
            vector< string > all_lines;

            split( lines, all_lines, '\n' );

            string search( '<' + attribute + '>' );

            for( size_t i = 0; i < all_lines.size( ); i++ )
            {
               string next_line( all_lines[ i ] );

               string::size_type pos = next_line.find( search );

               if( pos != string::npos )
               {
                  os << next_line.substr( pos + search.length( ) );

                  break;
               }
            }
         }
      }
   }
   else
      gup_ofs->list_files( "*", os, "", ods_file_system::e_list_style_brief );
}

void update_peerchain( const string& identity,
 const string* p_host_and_port, const string* p_description,
 const string* p_shared_secret, bool* p_auto_start, size_t* p_num_helpers )
{
   system_ods_fs_guard ods_fs_guard;

   system_ods_bulk_write ods_bulk_write;

   gup_ofs->set_root_folder( c_system_peerchain_folder );

   if( !gup_ofs->has_file( identity ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Peerchain '" + identity + "' was not found." );

   stringstream sio_data;

   gup_ofs->get_file( identity, &sio_data );

   sio_reader reader( sio_data );

   string old_auto_start( reader.read_attribute( c_peerchain_attribute_auto_start ) );
   string old_description( reader.read_attribute( c_peerchain_attribute_description ) );
   string old_extra_value( reader.read_opt_attribute( c_peerchain_attribute_extra_value ) );
   string old_host_name( reader.read_attribute( c_peerchain_attribute_host_name ) );
   string old_host_port( reader.read_attribute( c_peerchain_attribute_host_port ) );
   string old_local_port( reader.read_attribute( c_peerchain_attribute_local_port ) );
   string old_num_helpers( reader.read_attribute( c_peerchain_attribute_num_helpers ) );
   string old_peer_type( reader.read_attribute( c_peerchain_attribute_peer_type ) );
   string old_shared_secret( reader.read_attribute( c_peerchain_attribute_shared_secret ) );

   int peer_type = from_string< int >( old_peer_type );

   string new_description( old_description );

   if( p_description && !p_description->empty( ) )
      new_description = *p_description;

   string new_host_name( old_host_name );
   string new_host_port( old_host_port );

   if( p_host_and_port
    && ( ( peer_type != c_peerchain_type_hub )
    && ( peer_type != c_peerchain_type_local_only ) ) )
   {
      int port = 0;

      parse_host_and_or_port( *p_host_and_port, new_host_name, port );

      new_host_port = to_string( port );
   }

   if( new_host_name.empty( )
    || ( ( new_host_name == c_local_host )
    && ( peer_type != c_peerchain_type_hub )
    && ( peer_type != c_peerchain_type_local_only ) ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Peerchain '" + identity + "' host name '" + new_host_name + "' is invalid." );

   string new_shared_secret( old_shared_secret );

   if( p_shared_secret )
      new_shared_secret = *p_shared_secret;

   string new_auto_start( old_auto_start );

   if( p_auto_start )
      new_auto_start = to_string( *p_auto_start );

   string new_num_helpers( old_num_helpers );

   if( p_num_helpers
    && ( ( peer_type != c_peerchain_type_hub )
    && ( peer_type != c_peerchain_type_local_only ) ) )
      new_num_helpers = to_string( *p_num_helpers );

   // NOTE: Reset the stringstream.
   sio_data.str( "" );
   sio_data.clear( );

   sio_writer writer( sio_data );

   writer.write_attribute( c_peerchain_attribute_auto_start, new_auto_start );
   writer.write_attribute( c_peerchain_attribute_description, new_description );
   writer.write_attribute( c_peerchain_attribute_extra_value, old_extra_value );
   writer.write_attribute( c_peerchain_attribute_host_name, new_host_name );
   writer.write_attribute( c_peerchain_attribute_host_port, new_host_port );
   writer.write_attribute( c_peerchain_attribute_local_port, old_local_port );
   writer.write_attribute( c_peerchain_attribute_num_helpers, new_num_helpers );
   writer.write_attribute( c_peerchain_attribute_peer_type, old_peer_type );
   writer.write_attribute( c_peerchain_attribute_shared_secret, new_shared_secret );

   writer.finish_sections( );

   gup_ofs->store_file( identity, 0, &sio_data );
}

void destroy_peerchain( const string& identity, progress* p_progress )
{
   system_ods_fs_guard ods_fs_guard;

   system_ods_bulk_write ods_bulk_write;

   gup_ofs->set_root_folder( c_system_peerchain_folder );

   string backup_identity( get_raw_system_variable(
    get_special_var_name( e_special_var_blockchain_backup_identity ) ) );

   if( identity == backup_identity )
      throw runtime_error( "invalid attempt to destroy system backup identity" );

   string peer_hub_identity( get_raw_system_variable(
    get_special_var_name( e_special_var_blockchain_peer_hub_identity ) ) );

   if( identity == peer_hub_identity )
      throw runtime_error( "invalid attempt to destroy system peer hub identity" );

   if( !gup_ofs->has_file( identity ) )
      throw runtime_error( "invalid attempt to destroy unknown peerchain '" + identity + "'" );

   if( has_system_variable( identity )
    || has_system_variable( "~" + identity )
    || any_session_has_blockchain( identity )
    || has_registered_listener_id( identity ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Peerchain '" + identity + "' is currently in use." );

   stringstream sio_data;

   gup_ofs->get_file( identity, &sio_data );

   sio_reader reader( sio_data );

   string auto_start( reader.read_attribute( c_peerchain_attribute_auto_start ) );
   string description( reader.read_attribute( c_peerchain_attribute_description ) );
   string extra_value( reader.read_opt_attribute( c_peerchain_attribute_extra_value ) );
   string host_name( reader.read_attribute( c_peerchain_attribute_host_name ) );
   string host_port( reader.read_attribute( c_peerchain_attribute_host_port ) );
   string local_port( reader.read_attribute( c_peerchain_attribute_local_port ) );
   string num_helpers( reader.read_attribute( c_peerchain_attribute_num_helpers ) );
   string peer_type( reader.read_attribute( c_peerchain_attribute_peer_type ) );
   string shared_secret( reader.read_attribute( c_peerchain_attribute_shared_secret ) );

   gup_ofs->remove_file( identity );

   int type = from_string< int >( peer_type );

   bool is_backup_type = false;

   string reversed( identity );
   reverse( reversed.begin( ), reversed.end( ) );

   bool has_reversed = false;

   if( ( type == c_peerchain_type_combined )
    || ( type == c_peerchain_type_local_only ) )
      has_reversed = true;

   if( ( type == c_peerchain_type_combined )
    || ( type == c_peerchain_type_backup_only ) )
   {
      is_backup_type = true;

      string extra_identity( get_extra_identity_variable(
       get_special_var_name( e_special_var_blockchain_backup_identity ), extra_value ) );

      string hub_hash_variable;

      if( auto_start == c_true_value )
         hub_hash_variable = "@" + extra_identity;
      else
         hub_hash_variable = get_special_var_name( e_special_var_hub ) + '_' + identity;

      if( has_system_variable( hub_hash_variable ) )
      {
         string all_tags( get_hash_tags( get_raw_system_variable( hub_hash_variable ) ) );

         if( !all_tags.empty( ) )
         {
            vector< string > tags;

            split( all_tags, tags, '\n' );

            string hub_tags_prefix;

            for( size_t i = 0; i < tags.size( ); i++ )
            {
               string next_tag( tags[ i ] );

               string::size_type pos = next_tag.find( c_genesis_suffix );

               if( pos != string::npos )
               {
                  next_tag.erase( pos );

                  hub_tags_prefix = next_tag;

                  break;
               }
            }

            if( !hub_tags_prefix.empty( ) )
            {
               delete_files_for_tags( hub_tags_prefix + ".*", p_progress );

               string hub_identity( hub_tags_prefix );

               replace( hub_identity, c_bc_prefix, "" );

               set_system_variable( "#" + hub_identity, "" );
               set_system_variable( "%" + hub_identity, "" );
            }
         }

         set_system_variable( ">" + hub_hash_variable, "" );
      }
   }

   set_system_variable( "#" + identity, "" );
   set_system_variable( "%" + identity, "" );

   set_system_variable( ">@" + identity, "" );

   set_system_variable( get_special_var_name(
    e_special_var_auto ) + '_' + identity, "" );

   set_system_variable( ">" + get_special_var_name(
    e_special_var_hub ) + '_' + identity, "" );

   set_system_variable( ">" + get_special_var_name(
    e_special_var_secret_hash ) + '_' + identity, "" );

   set_system_variable( get_special_var_name(
    e_special_var_export_needed ) + '_' + identity, "" );

   if( has_files_area_archive( identity ) )
   {
      remove_files_area_archive( identity, true, true );

      if( has_reversed && has_files_area_archive( reversed ) )
      {
         set_system_variable( "#" + reversed, "" );
         set_system_variable( "%" + reversed, "" );

         remove_files_area_archive( reversed, true, true );
      }

      if( is_backup_type )
      {
         date_time dtm( date_time::local( ) );

         remove_all_repository_entries( identity, &dtm, p_progress );
      }
   }

   if( has_reversed )
      delete_files_area_files_for_pat( c_bc_prefix + reversed + ".*" );

   delete_files_area_files_for_pat( c_bc_prefix + identity + ".*" );
}

string get_peerchain_info( const string& identity, bool* p_is_listener, string* p_shared_secret )
{
   system_ods_fs_guard ods_fs_guard;

   system_ods_bulk_read ods_bulk_read;

   string retval;

   gup_ofs->set_root_folder( c_system_peerchain_folder );

   vector< string > peerchains;

   gup_ofs->list_files( "", peerchains );

   string reversed( identity );
   reverse( reversed.begin( ), reversed.end( ) );

   for( size_t i = 0; i < peerchains.size( ); i++ )
   {
      bool is_reversed = false;

      if( reversed == peerchains[ i ] )
         is_reversed = true;

      if( is_reversed || ( identity == peerchains[ i ] ) )
      {
         stringstream sio_data;

         gup_ofs->get_file( !is_reversed ? identity : reversed, &sio_data );

         sio_reader reader( sio_data );

         string auto_start( reader.read_attribute( c_peerchain_attribute_auto_start ) );
         string description( reader.read_attribute( c_peerchain_attribute_description ) );
         string extra_value( reader.read_opt_attribute( c_peerchain_attribute_extra_value ) );
         string host_name( reader.read_attribute( c_peerchain_attribute_host_name ) );
         string host_port( reader.read_attribute( c_peerchain_attribute_host_port ) );
         string local_port( reader.read_attribute( c_peerchain_attribute_local_port ) );
         string num_helpers( reader.read_attribute( c_peerchain_attribute_num_helpers ) );
         string peer_type( reader.read_attribute( c_peerchain_attribute_peer_type ) );
         string shared_secret( reader.read_attribute( c_peerchain_attribute_shared_secret ) );

         if( p_shared_secret )
            *p_shared_secret = shared_secret;

         if( !p_is_listener || ( host_name == string( c_local_host ) ) )
         {
            if( p_is_listener )
               *p_is_listener = true;

            retval = identity + '=' + local_port;
         }
         else
         {
            if( p_is_listener )
               *p_is_listener = false;

            retval = identity;

            if( !extra_value.empty( ) )
               retval += '-' + extra_value;

            retval += '+' + num_helpers + '=' + host_name + '-' + host_port + ':' + peer_type;
         }

         // NOTE: Only break if not reversed in case both "backup only" and "shared only" entries exist.
         if( !is_reversed )
            break;
      }
   }

   return retval;
}

void get_peerchain_externals( vector< string >& peerchain_externals, bool auto_start_only )
{
   system_ods_fs_guard ods_fs_guard;

   system_ods_bulk_read  ods_bulk_read;

   gup_ofs->set_root_folder( c_system_peerchain_folder );

   vector< string > peerchains;

   gup_ofs->list_files( "", peerchains );

   for( size_t i = 0; i < peerchains.size( ); i++ )
   {
      string identity( peerchains[ i ] );

      stringstream sio_data;

      gup_ofs->get_file( identity, &sio_data );

      sio_reader reader( sio_data );

      string auto_start( reader.read_attribute( c_peerchain_attribute_auto_start ) );
      string description( reader.read_attribute( c_peerchain_attribute_description ) );
      string extra_value( reader.read_opt_attribute( c_peerchain_attribute_extra_value ) );
      string host_name( reader.read_attribute( c_peerchain_attribute_host_name ) );
      string host_port( reader.read_attribute( c_peerchain_attribute_host_port ) );
      string local_port( reader.read_attribute( c_peerchain_attribute_local_port ) );
      string num_helpers( reader.read_attribute( c_peerchain_attribute_num_helpers ) );
      string peer_type( reader.read_attribute( c_peerchain_attribute_peer_type ) );
      string shared_secret( reader.read_attribute( c_peerchain_attribute_shared_secret ) );

      if( auto_start_only && ( host_name == c_dummy_host_name ) )
         continue;

      if( ( host_name != string( c_local_host ) )
       && ( !auto_start_only || ( auto_start == c_true_value ) ) )
      {
         string peer_info( identity );

         if( !extra_value.empty( ) )
            peer_info += '-' + extra_value;

         peer_info += '+' + num_helpers + '=' + host_name + '-' + host_port + ':' + peer_type;

         peerchain_externals.push_back( peer_info );
      }
   }
}

void get_peerchain_listeners( multimap< int, string >& peerchain_listeners, bool auto_start_only )
{
   system_ods_fs_guard ods_fs_guard;

   system_ods_bulk_read ods_bulk_read;

   gup_ofs->set_root_folder( c_system_peerchain_folder );

   vector< string > peerchains;

   gup_ofs->list_files( "", peerchains );

   string blockchain_backup_prefix, blockchain_backup_suffix;

   string blockchain_backup_identity_name( get_special_var_name( e_special_var_blockchain_backup_identity ) );

   identity_variable_name_prefix_and_suffix(
    blockchain_backup_identity_name, blockchain_backup_prefix, blockchain_backup_suffix );

   string blockchain_peer_hub_prefix, blockchain_peer_hub_suffix;

   string blockchain_peer_hub_identity_name( get_special_var_name( e_special_var_blockchain_peer_hub_identity ) );

   identity_variable_name_prefix_and_suffix(
    blockchain_peer_hub_identity_name, blockchain_peer_hub_prefix, blockchain_peer_hub_suffix );

   for( size_t i = 0; i < peerchains.size( ); i++ )
   {
      string identity( peerchains[ i ] );

      stringstream sio_data;

      gup_ofs->get_file( identity, &sio_data );

      sio_reader reader( sio_data );

      string auto_start( reader.read_attribute( c_peerchain_attribute_auto_start ) );
      string description( reader.read_attribute( c_peerchain_attribute_description ) );
      string extra_value( reader.read_opt_attribute( c_peerchain_attribute_extra_value ) );
      string host_name( reader.read_attribute( c_peerchain_attribute_host_name ) );
      string host_port( reader.read_attribute( c_peerchain_attribute_host_port ) );
      string local_port( reader.read_attribute( c_peerchain_attribute_local_port ) );
      string num_helpers( reader.read_attribute( c_peerchain_attribute_num_helpers ) );
      string peer_type( reader.read_attribute( c_peerchain_attribute_peer_type ) );
      string shared_secret( reader.read_attribute( c_peerchain_attribute_shared_secret ) );

      int port = atoi( local_port.c_str( ) );

      int type = from_string< int >( peer_type );

      if( ( port > 0 )
       && ( !auto_start_only || ( auto_start == c_true_value ) ) )
      {
         string suffix;

         if( ( type == c_peerchain_type_combined ) || ( type == c_peerchain_type_local_only ) )
            suffix = '!';

         if( ( type == c_peerchain_type_local_only )
          && ( identity == get_system_variable( blockchain_backup_identity_name ) ) )
         {
            for( size_t i = 1; i <= c_max_extras; i++ )
            {
               string next_extra( get_system_variable( blockchain_backup_prefix + to_string( i ) + blockchain_backup_suffix ) );

               if( next_extra.empty( ) )
                  break;

               peerchain_listeners.insert( make_pair( port, next_extra + suffix ) );
            }
         }
         else if( ( type == c_peerchain_type_hub ) && ( identity == get_system_variable( blockchain_peer_hub_identity_name ) ) )
         {
            size_t num_listeners = peerchain_listeners.size( );

            for( size_t i = 1; i <= c_max_extras; i++ )
            {
               string next_extra( get_system_variable( blockchain_peer_hub_prefix + to_string( i ) + blockchain_peer_hub_suffix ) );

               if( next_extra.empty( ) )
                  break;

               peerchain_listeners.insert( make_pair( port, next_extra + suffix ) );
            }

            // NOTE: If no extra peer hub chains are found then will instead listen
            // for the hub identity itself (i.e. assume this must be a "hub node").
            if( num_listeners == peerchain_listeners.size( ) )
               peerchain_listeners.insert( make_pair( port, identity + suffix ) );
         }
         else
            peerchain_listeners.insert( make_pair( port, identity + suffix ) );
      }
   }
}

bool get_use_udp( )
{
   return g_use_udp;
}

bool get_using_ssl( )
{
   return g_using_ssl;
}

string get_log_files_dir( )
{
   guard g( g_mutex );

   return g_log_files_dir;
}

void set_log_files_dir( const char* p_dir_name )
{
   guard g( g_mutex );

   if( !p_dir_name )
      g_log_files_dir.erase( );
   else
      g_log_files_dir = p_dir_name;
}

string get_files_area_dir( )
{
   guard g( g_mutex );

   return g_files_area_dir;
}

void set_files_area_dir( const char* p_files_area_dir )
{
   guard g( g_mutex );

   bool was_first = false;

   if( p_files_area_dir )
   {
      string files_area_dir( p_files_area_dir );

      if( !files_area_dir.empty( ) )
      {
         g_files_area_dir = files_area_dir;

         if( g_files_area_dir_default.empty( ) )
         {
            was_first = true;

            g_files_area_dir_default = g_files_area_dir;
         }
      }
      else
      {
         if( g_files_area_dir_default.empty( ) )
         {
            was_first = true;

            g_files_area_dir_default = string( c_ciyam_files_directory );
         }

         g_files_area_dir = g_files_area_dir_default;
      }
   }

   if( was_first )
      set_system_variable( get_special_var_name( e_special_var_files_area_dir ), g_files_area_dir, true );
}

size_t get_notifier_ignore_secs( )
{
   return g_notifier_ignore_secs;
}

size_t get_files_area_item_max_num( )
{
   return g_files_area_item_max_num;
}

size_t get_files_area_item_max_size( )
{
   return g_files_area_item_max_size;
}

size_t get_num_recv_stream_sessions( )
{
   return g_num_recv_stream_sessions;
}

size_t get_num_send_stream_sessions( )
{
   return g_num_send_stream_sessions;
}

string get_mbox_path( )
{
   return g_mbox_path;
}

string get_mbox_username( )
{
   return g_mbox_username;
}

string get_pop3_server( )
{
   return g_pop3_server;
}

string get_pop3_suffix( )
{
   return g_pop3_suffix;
}

string get_pop3_username( )
{
   return g_pop3_username;
}

string get_pop3_password( )
{
   return decrypt_data( g_pop3_password );
}

string get_pop3_security( )
{
   return g_pop3_security;
}

string get_pop3_suffix_or_domain( )
{
   string retval( get_pop3_suffix( ) );

   if( retval.empty( ) )
      retval = get_domain( );

   return retval;
}

string get_smtp_server( )
{
   return g_smtp_server;
}

string get_smtp_sender( )
{
   return g_smtp_sender;
}

string get_smtp_suffix( )
{
   return g_smtp_suffix;
}

string get_smtp_username( )
{
   return g_smtp_username;
}

string get_smtp_password( )
{
   return decrypt_data( g_smtp_password );
}

string get_smtp_security( )
{
   return g_smtp_security;
}

int get_smtp_max_send_attempts( )
{
   return g_smtp_max_send_attempts;
}

int64_t get_smtp_max_attached_data( )
{
   return g_smtp_max_attached_data;
}

string get_smtp_suffix_or_domain( )
{
   string retval( get_smtp_suffix( ) );

   if( retval.empty( ) )
      retval = get_domain( );

   return retval;
}

string list_externals( )
{
   string s;

   for( external_client_iterator eci = g_external_client_info.begin( ); eci != g_external_client_info.end( ); ++eci )
   {
      if( !s.empty( ) )
         s += '\n';

      s += eci->first;

      s += " (" + eci->second.protocol + ")";
   }

   return s;
}

bool is_local_external_client( const string& key )
{
   if( !g_external_client_info.count( key ) )
      throw runtime_error( "unknown external client key '" + key + "'" );

   return g_external_client_info[ key ].is_local;
}

void get_external_client_info( const string& key, external_client& info )
{
   if( !g_external_client_info.count( key ) )
      throw runtime_error( "unknown external client key '" + key + "'" );

   info = g_external_client_info[ key ];
}

void verify_active_external_service( const string& ext_key )
{
   if( !active_external_service( ext_key ) )
      throw runtime_error(
       get_string_message( GS( c_str_external_service_unavailable ),
       make_pair( c_str_external_service_unavailable_symbol, ext_key ) ) );
}

void decrypt_data( string& s, const string& data,
 bool empty_key, bool harden_key, bool is_pwd_and_data )
{
   string key;
   string str( data );

   key.reserve( c_key_reserve_size );

   string::size_type pos = 0;

   // NOTE: Password and data are space (or double space) separated
   // (the latter being necessary if the password includes spaces).
   if( is_pwd_and_data )
   {
      bool is_double = false;

      pos = str.find( c_two_spaces );

      if( pos != string::npos )
         is_double = true;
      else
         pos = str.find( ' ' );

      if( pos == string::npos )
         pos = 0;
      else
      {
         key.resize( pos );

         memcpy( &key[ 0 ], &str[ 0 ], pos );

         str.erase( 0, pos + 1 + is_double );
      }
   }

   // NOTE: If "empty_key" was specified then an empty key is used (so
   // should only set "true" when performing simple regression tests).
   if( !empty_key && ( pos == 0 ) )
      get_sid( key );

   if( harden_key )
      harden_key_with_hash_rounds( key, key, key, c_key_rounds_multiplier );

   data_decrypt( s, str, key );

   clear_key( key );
   clear_key( str );
}

void encrypt_data( string& s, const string& data,
 bool no_ssl, bool empty_key, bool use_sid_only, bool harden_key, bool is_pwd_and_data )
{
   string key;

   string str( data );

   key.reserve( c_key_reserve_size );

   string::size_type pos = 0;

   // NOTE: Password and data are space (or double space) separated
   // (the latter being necessary if the password includes spaces).
   if( is_pwd_and_data )
   {
      bool is_double = false;

      pos = str.find( c_two_spaces );

      if( pos != string::npos )
         is_double = true;
      else
         pos = str.find( ' ' );

      // NOTE: If no data has been provided after the space (or double space)
      // separator will treat as though no space has been found but if double
      // spaces had been provided then will add extra entropy characters.
      if( pos != string::npos && ( str.length( ) == ( pos + 1 + is_double ) ) )
      {
         str.erase( pos );
         pos = string::npos;
      }

      if( pos == string::npos )
      {
         key = str;
         str.erase( );

         string mnemonics;

         mnemonics.reserve( c_key_reserve_size );

         get_mnemonics_or_hex_seed( mnemonics, string( ) );

         str += mnemonics;

         if( is_double )
         {
            str += ' ';

            for( size_t i = 0; i < c_num_extra_entropy_chars; i++ )
               str += ( char )( 32 + ( rand( ) % 95 ) );
         }

         clear_key( mnemonics );
      }
      else
      {
         key.resize( pos );

         memcpy( &key[ 0 ], &str[ 0 ], pos );

         str.erase( 0, pos + 1 + is_double );
      }
   }

   // NOTE: (refer to "decrypt_data")
   if( !empty_key && !is_pwd_and_data )
      get_sid( key );

   if( harden_key )
      harden_key_with_hash_rounds( key, key, key, c_key_rounds_multiplier );

   data_encrypt( s, str, key, !no_ssl, ( !empty_key && !use_sid_only ) );

   clear_key( key );
   clear_key( str );
}

string totp_secret_key( const string& unique )
{
   string key, retval;

   key.reserve( c_key_reserve_size );

   string crypt_key( get_raw_session_variable( get_special_var_name( e_special_var_crypt_key ) ) );

   if( crypt_key.empty( ) )
      get_sid( key );

   retval = get_totp_secret( unique, crypt_key.empty( ) ? key : crypt_key );

   if( crypt_key.empty( ) )
      clear_key( key );

   return retval;
}

int exec_system( const string& cmd, bool async, bool delay, bool* p_delayed )
{
   int rc = 0;

   string exec_cmd( cmd );
   string async_cmd( cmd );

   // NOTE: Prevent command substition from being able to occur.
   string::size_type pos = cmd.find( "$(" );

   if( pos == string::npos )
      pos = cmd.find( '`' );

   if( pos != string::npos )
      throw runtime_error( "disallowed characters found in cmd: " + cmd );

   string append_file( get_raw_session_variable(
    get_special_var_name( e_special_var_exec_system_append ) ) );

   if( !append_file.empty( ) )
   {
      exec_cmd += " >> " + append_file;
      async_cmd += " >> " + append_file;
   }

   async_cmd += " &";

   // NOTE: For security against potentially malicious module code only permit system calls
   // from the autoscript session, via "run_script", or from specific server storages.
   if( gtp_session && !gtp_session->running_script
    && ( gtp_session->p_storage_handler->get_name( ) != c_meta_storage_name )
    && ( gtp_session->p_storage_handler->get_name( ) != c_ciyam_storage_name )
    && ( gtp_session->p_storage_handler->get_name( ) != c_default_storage_name ) )
      throw runtime_error( "invalid exec_system: " + exec_cmd );

   string async_var( get_raw_session_variable( get_special_var_name( e_special_var_allow_async ) ) );

   // NOTE: The session variable @allow_async can be used to force non-async execution.
   if( ( async_var == "0" ) || ( async_var == c_false ) )
      async = false;

   if( async || delay )
   {
      // NOTE: It is expected that synchronous system calls are needed as a part
      // of the transaction itself, however, for async calls they will only ever
      // be issued after the transaction is successfully committed. If one wants
      // to not issue the scripts until the commit itself but actually want them
      // to be issued synchronously then use "delay".
      if( gtp_session && !gtp_session->transactions.empty( ) )
      {
         TRACE_LOG( TRACE_INITIAL | TRACE_SESSION, "(system exec delayed due to session tx)" );

         gtp_session->async_or_delayed_system_commands.push_back( async ? async_cmd : exec_cmd );

         if( !gtp_session->script_temp_args_file.empty( ) )
            gtp_session->async_or_delayed_args_files.push_back( gtp_session->script_temp_args_file );

         if( p_delayed )
            *p_delayed = true;

         return rc;
      }
   }

   TRACE_LOG( TRACE_INITIAL | TRACE_SESSION, async ? async_cmd : exec_cmd );

   rc = system( async ? async_cmd.c_str( ) : exec_cmd.c_str( ) );

   string error_message;

   if( gtp_session && !gtp_session->script_temp_args_file.empty( ) )
   {
      error_message = get_raw_system_variable( gtp_session->script_temp_args_file );

      set_system_variable( gtp_session->script_temp_args_file, "" );
   }

   // NOTE: If the script had an error and the caller should throw an exception then do so.
   string check_script_error(
    get_raw_session_variable( get_special_var_name( e_special_var_check_script_error ) ) );

   if( ( check_script_error == c_true ) || ( check_script_error == c_true_value ) )
   {
      if( !error_message.empty( ) && ( error_message != c_true_value ) )
      {
         // NOTE: If the error starts with '@' then assume that it is actually
         // intended to be an execute "return" message rather than an error.
         if( error_message[ 0 ] == '@' )
            set_session_variable( get_special_var_name( e_special_var_return ), error_message.substr( 1 ) );
         else
            throw runtime_error( error_message );
      }
   }

   return rc;
}

int run_script( const string& script_name, bool async, bool delay, bool no_logging, bool sleep_after )
{
   int rc = -1;

   if( get_script_reconfig( ) && scripts_file_has_changed( ) )
   {
      read_script_info( );
      TRACE_LOG( TRACE_MINIMAL, "[manuscript.sio] updated" );
   }

   // NOTE: If the name '@none" is provided then just returns zero.
   if( script_name == get_special_var_name( e_special_var_none ) )
      return 0;

   if( !g_scripts.count( script_name ) )
   {
      TRACE_LOG( TRACE_MINIMAL, "attempt to run unknown script '" + script_name + "'" );

      throw runtime_error( "unknown script '" + script_name + "'" );
   }

   string filename( g_scripts[ script_name ].filename );

   bool is_script = ( filename == c_script_dummy_filename );

   bool is_busy = false;

   string lock_filename( g_scripts[ script_name ].lock_filename );

   if( !lock_filename.empty( ) && !can_create_script_lock_file( lock_filename ) )
      is_busy = true;

   if( !is_busy )
   {
      bool locked_script = false;

      if( !lock_filename.empty( ) && !g_scripts[ script_name ].check_lock_only )
         locked_script = true;

      string arguments( process_script_args( g_scripts[ script_name ].arguments ) );

      unique_ptr< restorable< bool > > up_running_script;

      if( gtp_session )
      {
         gtp_session->script_temp_args_file.erase( );

         up_running_script.reset( new restorable< bool >( gtp_session->running_script, true ) );
      }

      string cmd_and_args;

      if( is_script )
      {
         string args_file( "~" + uuid( ).as_string( ) );

         // NOTE: Empty code block for scope purposes.
         {
            ofstream outf( args_file.c_str( ) );

            if( !outf )
               throw runtime_error( "unable to open '" + args_file + "' for output" );

            string rpc_password( get_rpc_password( ) );

            if( !rpc_password.empty( ) )
               outf << ".session_rpc_unlock " << rpc_password << endl;

            // NOTE: Underbar prefix is used in case arguments contain '$' characters.
            outf << "_<<" << arguments << endl;

            outf.flush( );

            if( !outf.good( ) )
               throw runtime_error( "*** unexpected bad file '" + args_file + "' in run_script ***" );
         }

         if( gtp_session )
         {
            gtp_session->script_temp_args_file = args_file;

            string check_script_error(
             get_raw_session_variable( get_special_var_name( e_special_var_check_script_error ) ) );

            // NOTE: If the script is intended to be synchronous and the "no_logging" argument is true
            // then the first error to occur in an external script (or scripts if the "delay" argument
            // has being used) will be thrown as an exception after the calling session's "system" (in
            // the case of delayed scripts that will occur at the end of transaction commit). Thus set
            // special variables for that here. As "ciyam_client" will set a session variable for each
            // script to the "script args" unique file name (via the "-args_file" command-line option)
            // a system variable with the same name is being set here (to "c_true_value"). If an error
            // occurs in a command being executed by the script and system variable with the same name
            // as the "args_file" session variable then the value will be changed to that of the error
            // message.
            if( !async && no_logging
             && ( check_script_error != c_false ) && ( check_script_error != c_false_value ) )
            {
               set_system_variable( args_file, c_true_value );

               set_session_variable( get_special_var_name(
                e_special_var_check_script_error ), c_true_value );
            }
         }

         string script_args( args_file );

         if( !has_raw_system_variable( get_special_var_name( e_special_var_log_all_scripts ) ) )
         {
            // NOTE: If the "no_logging" argument is set true then make sure that "script" execution
            // won't be logged (even in the case of an error). For synchronous scripts an error will
            // be handled (per the special variables set above) as though it had happened within the
            // caller's session but if the script is intended to be asynchronously executed (even if
            // it ends up being finally called synchronously) this will result in no record (without
            // using specific tracing flags) of the script's execution or errors (so it would not be
            // generally advisable to use it in this manner).
            if( no_logging )
               script_args = "-do_not_log " + script_args;
            else
            {
               string errors_only( get_raw_session_variable(
                get_special_var_name( e_special_var_errors_only ) ) );

               if( ( errors_only == c_true ) || ( errors_only == c_true_value ) )
                  script_args = "-log_on_error " + script_args;
            }
         }

         // NOTE: For cases where one script may end up calling numerous others (i.e.
         // such as a scan across records) this special session variable is available
         // to prevent excess log entries appearing in the script log file.
         string quiet( get_raw_session_variable( get_special_var_name( e_special_var_quiet ) ) );

         // NOTE: If making any change to "script_args" then it likely will also need
         // to be done in "auto_script.cpp" (as it also executes the 'script' script).
         if( ( quiet != c_true ) && ( quiet != c_true_value ) )
            script_args += " " + script_name;

         cmd_and_args = "./script " + script_args;
      }
      else
      {
         cmd_and_args = filename;

         if( cmd_and_args.find( '/' ) == string::npos )
            cmd_and_args = "./" + cmd_and_args;

         if( !arguments.empty( ) )
            cmd_and_args += " " + arguments;
      }

      if( locked_script )
      {
         is_busy = !create_script_lock_file( lock_filename );

         if( !is_busy )
            cmd_and_args = "./locked.sh \"" + lock_filename + "\" " + cmd_and_args;
      }

      if( !is_busy )
      {
         bool was_delayed = false;

         rc = exec_system( cmd_and_args, async, delay, &was_delayed );

         if( sleep_after )
         {
            // NOTE: If has executed the system command will perform the "msleep"
            // now otherwise will set the "@sleep_after" session variable so that
            // it can occur when the system command is later executed.
            if( !was_delayed )
               msleep( c_sleep_after_script_time );
            else
               set_session_variable( get_special_var_name(
                e_special_var_sleep_after ), to_string( c_sleep_after_script_time ) );
         }
      }
   }

   if( is_busy )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Script '" + script_name + "' appears to be busy." );

   return rc;
}

void list_scripts( const string& pattern, ostream& os )
{
   output_script_info( pattern, os );
}

void check_script_args( const string& script_name, bool* p_rc )
{
   if( !g_scripts.count( script_name ) )
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( "Unknown script name '" + script_name + "'." );

   if( p_rc )
      *p_rc = true;

   string arguments( g_scripts[ script_name ].arguments );

   vector< string > all_args;

   if( !arguments.empty( ) )
   {
      split( arguments, all_args, ' ' );

      size_t adjust = 0;

      for( size_t i = 0; i < all_args.size( ); i++ )
      {
         string next_arg( all_args[ i ] );

         if( next_arg == c_script_arg_opt )
            break;

         if( !next_arg.empty( ) && ( next_arg[ 0 ] == '@' ) )
         {
            if( i == 0 )
               ++adjust;

            string next_argx( "@arg" + to_string( i + adjust ) );

            if( !has_session_variable( next_arg ) && !has_session_variable( next_argx ) )
            {
               if( !p_rc )
                  // FUTURE: This message should be handled as a server string message.
                  throw runtime_error( "Script '" + script_name + "' missing argument '" + next_arg + "'." );
               else
               {
                  *p_rc = false;

                  break;
               }
            }
         }
      }
   }
}

string process_script_args( const string& raw_args, bool use_system_variables )
{
   string retval;

   if( !raw_args.empty( ) )
   {
      vector< string > all_args;
      split( raw_args, all_args, ' ' );

      size_t adjust = 0;
      size_t skipped = 0;

      for( size_t i = 0; i < all_args.size( ); i++ )
      {
         string next_arg( all_args[ i ] );

         if( next_arg == c_script_arg_opt )
         {
            ++skipped;

            continue;
         }

         if( !next_arg.empty( ) && next_arg[ 0 ] == '@' )
         {
            // NOTE: For application protocol scripts the first argument is expected
            // to be the script name (and thus not start with a '@' character) so if
            // the first argument does begin with an '@' then adjust the numbering.
            if( i == 0 )
               ++adjust;

            if( use_system_variables )
               next_arg = get_raw_system_variable( next_arg );
            else
            {
               // NOTE: Always use "@argX" value if found otherwise will use the value
               // of the session variable that matches the name of the script argument.
               string arg_value(
                get_raw_session_variable( "@arg" + to_string( i + adjust - skipped ) ) );

               if( !arg_value.empty( ) )
                  next_arg = arg_value;
               else
                  next_arg = get_raw_session_variable( next_arg );
            }
         }

         // NOTE: Use 'escape' to prevent any variable replacements.
         if( !next_arg.empty( ) )
            next_arg = escaped_shell_arg( escape( next_arg, "$" ) );

         if( !retval.empty( ) )
            retval += " ";

         if( next_arg.empty( ) )
            next_arg = "\"\"";

         retval += next_arg;
      }
   }

   if( !use_system_variables )
   {
      // NOTE: Any "@argX" session variables are now cleared
      // (so they are not accidentally used in another call).
      for( size_t i = 1; i < 10; i++ )
         set_session_variable( "@arg" + to_string( i ), "" );
   }

   return retval;
}

void generate_new_script_sio( bool for_autoscript )
{
   string file_name( for_autoscript ? c_autoscript_file : c_manuscript_file );

   if( file_exists( file_name ) )
   {
      vector< string > new_lines;
      vector< string > original_lines;

      buffer_file_lines( file_name, original_lines );

      size_t finish_line = 0;

      bool found_start = false;

      for( size_t i = 0; i < original_lines.size( ); i++ )
      {
         string next( original_lines[ i ] );

         if( !found_start )
            new_lines.push_back( next );

         if( next == "### [<start generated>]" )
            found_start = true;
         else if( found_start && next == "### [<finish generated>]" )
         {
            finish_line = i;

            break;
         }
      }

      file_filter ff;

      fs_iterator fs( ".", &ff );

      while( fs.has_next( ) )
      {
         if( wildcard_match( "*.extra.lst", fs.get_name( ) ) )
         {
            vector< string > lines;

            buffer_file_lines( fs.get_name( ), lines );

            for( size_t i = 0; i < lines.size( ); i++ )
            {
               if( file_exists( lines[ i ] ) )
               {
                  vector< string > script_lines;

                  buffer_file_lines( lines[ i ], script_lines );

                  bool is_script = false;

                  for( size_t j = 0; j < script_lines.size( ); j++ )
                  {
                     string next( script_lines[ j ] );

                     // NOTE: Remove comment prefixes for console script files.
                     if( next.find( "#;" ) == 0 )
                        next.erase( 0, 1 );
                     else if( next.find( "REM;" ) == 0 )
                        next.erase( 0, 3 );

                     if( next.empty( ) || next[ 0 ] != ';' )
                     {
                        if( is_script )
                           break;
                     }
                     else if( is_script )
                        new_lines.push_back( next.substr( 1 ) );
                     else if( next.find( "; NOTE: " ) == 0 )
                     {
                        if( next.find( "'" + file_name + "'" ) != string::npos )
                           is_script = true;
                     }
                  }
               }
            }
         }
      }

      if( finish_line > 0 )
      {
         while( finish_line < original_lines.size( ) )
            new_lines.push_back( original_lines[ finish_line++ ] );
      }

      ofstream outf( ( file_name + ".new" ).c_str( ) );

      for( size_t i = 0; i < new_lines.size( ); i++ )
         outf << new_lines[ i ] << '\n';
   }
}

bool create_script_lock_file( const string& name )
{
   guard g( g_mutex );

   bool okay = can_create_script_lock_file( name );

   if( okay )
   {
      file_touch( name, 0, true, true );

      okay = file_exists( name );
   }

   return okay;
}

bool can_create_script_lock_file( const string& name )
{
   guard g( g_mutex );

   bool okay = !file_exists( name );

   // NOTE: If found first sleep and check
   // again (instead of an immediate fail).
   if( !okay )
   {
      msleep( c_lock_file_sleep_time );

      okay = !file_exists( name );
   }

   if( okay )
      g_checked_script_lock_files.erase( name );
   else
   {
      string pid( buffer_file( name ) );

      // NOTE: If the file is empty (rather than containing a PID)
      // then will delete the file (assuming it was left over from
      // an unexpected application server termination).
      if( pid.empty( ) )
         okay = true;
      else
      {
         if( getpgid( atoi( pid.c_str( ) ) ) < 0 )
            okay = true;
      }

      if( okay )
      {
         file_remove( name );

         if( !file_exists( name ) )
            g_checked_script_lock_files.erase( name );
         else
         {
            okay = false;

            // NOTE: If unable to delete the lock file then
            // will log this (but only once per file name).
            if( !g_checked_script_lock_files.count( name ) )
            {
               g_checked_script_lock_files.insert( name );

               TRACE_LOG( TRACE_MINIMAL, "(unexpected lock file '" + name + "' could not be removed)" );
            }
         }
      }
   }

   return okay;
}

void generate_new_script_sio_files( )
{
   generate_new_script_sio( true );
   generate_new_script_sio( false );
}

void init_session(
 command_handler& cmd_handler, bool is_peer_session,
 const string* p_ip_addr, const string* p_blockchain, int port,
 bool is_support_session, bool add_pubkey_variable, size_t session_id )
{
   // NOTE: Scope for guard object.
   {
      guard g( g_session_mutex, "init_session" );

      gtp_session = 0;

      string ip_addr;
      string blockchain;

      if( p_ip_addr )
         ip_addr = *p_ip_addr;

      if( p_blockchain )
         blockchain = *p_blockchain;

      size_t offset = 0;

      for( size_t i = 0; i < g_max_sessions; i++ )
      {
         if( !gtp_session && !g_sessions[ i ] )
         {
            if( !session_id )
               session_id = ++g_next_session_id;

            g_sessions[ i ] = new session( session_id,
             i, cmd_handler, g_storage_handlers[ 0 ], is_peer_session,
             p_ip_addr, p_blockchain, is_support_session, add_pubkey_variable );

            offset = i;

            gtp_session = g_sessions[ i ];

            ods::instance( 0, true );

            if( !is_peer_session || is_support_session )
               break;
         }
         else if( is_peer_session && !is_support_session )
         {
            session* p_next_session = g_sessions[ i ];

            if( p_next_session && !p_next_session->is_support_session )
            {
               if( ( ip_addr == p_next_session->ip_addr )
                && ( blockchain == p_next_session->blockchain ) )
               {
                  if( gtp_session )
                  {
                     delete g_sessions[ offset ];
                     g_sessions[ offset ] = 0;

                     gtp_session = 0;
                  }

                  throw runtime_error( "found existing main peer session for blockchain '" + blockchain + "'" );
               }
            }
         }
      }
   }

   if( !gtp_session )
      throw runtime_error( "max. permitted concurrent sessions already active" );

   set_default_session_variables( port );
}

void term_session( )
{
   guard g( g_session_mutex, "term_session" );

   if( gtp_session )
   {
      session* p_main_session = 0;

      vector< session* > other_supporters;

      // NOTE: If a support session is being terminated then any file hashes
      // remaining to "get" or "put" are now distributed equally between the
      // other supporters or returned to the main session (according to what
      // is found here).
      if( gtp_session->is_support_session )
      {
         for( size_t i = 0; i < g_max_sessions; i++ )
         {
            session* p_next = g_sessions[ i ];

            if( p_next && ( p_next != gtp_session )
             && ( p_next->ip_addr == gtp_session->ip_addr )
             && ( p_next->blockchain == gtp_session->blockchain ) )
            {
               if( !p_next->is_support_session )
                  p_main_session = p_next;
               else
                  other_supporters.push_back( p_next );
            }
         }

         if( !other_supporters.empty( ) )
         {
            size_t num_to_get = gtp_session->file_hashes_to_get.size( );

            if( num_to_get )
            {
               size_t supporter = 0;

               for( size_t i = 0; i < num_to_get; i++ )
               {
                  other_supporters[ supporter ]->file_hashes_to_get.push_back( gtp_session->file_hashes_to_get.front( ) );
                  gtp_session->file_hashes_to_get.pop_front( );

                  if( ++supporter >= other_supporters.size( ) )
                     supporter = 0;
               }
            }

            size_t num_to_put = gtp_session->file_hashes_to_put.size( );

            if( num_to_put )
            {
               size_t supporter = 0;

               for( size_t i = 0; i < num_to_put; i++ )
               {
                  other_supporters[ supporter ]->file_hashes_to_put.push_back( gtp_session->file_hashes_to_put.front( ) );
                  gtp_session->file_hashes_to_put.pop_front( );

                  if( ++supporter >= other_supporters.size( ) )
                     supporter = 0;
               }
            }
         }
         else if( p_main_session )
         {
            size_t num_to_get = gtp_session->file_hashes_to_get.size( );

            if( num_to_get )
            {
               for( size_t i = 0; i < num_to_get; i++ )
               {
                  p_main_session->file_hashes_to_get.push_back( gtp_session->file_hashes_to_get.front( ) );
                  gtp_session->file_hashes_to_get.pop_front( );
               }
            }

            size_t num_to_put = gtp_session->file_hashes_to_put.size( );

            if( num_to_put )
            {
               for( size_t i = 0; i < num_to_put; i++ )
               {
                  p_main_session->file_hashes_to_put.push_back( gtp_session->file_hashes_to_put.front( ) );
                  gtp_session->file_hashes_to_put.pop_front( );
               }
            }
         }
      }

      if( gtp_session->p_storage_handler )
         gtp_session->p_storage_handler->release_all_locks_for_session( gtp_session );

      for( size_t i = 0; i < g_max_sessions; i++ )
      {
         if( gtp_session == g_sessions[ i ] )
         {
            set< size_t >::iterator j;

            for( j = gtp_session->release_sessions.begin( ); j != gtp_session->release_sessions.end( ); ++j )
               release_session( *j, false );

            g_condemned_sessions.insert(
             gtp_session->condemned_sessions.begin( ), gtp_session->condemned_sessions.end( ) );

            delete g_sessions[ i ];
            g_sessions[ i ] = 0;
            gtp_session = 0;

            break;
         }
      }
   }

   if( gtp_session )
      throw runtime_error( "unable to terminate session" );
}

size_t reserve_session_id( size_t num_extras )
{
   guard g( g_session_mutex );

   size_t next_session_id = ++g_next_session_id;

   g_next_session_id += num_extras;

   return next_session_id;
}

size_t session_id( )
{
   size_t rc = 0;

   if( gtp_session )
      rc = gtp_session->id;

   return rc;
}

string session_ip_addr( )
{
   string retval;

   if( gtp_session )
      retval = gtp_session->ip_addr;

   return retval;
}

string session_ip_addr( size_t slot )
{
   guard g( g_session_mutex );

   string retval;

   if( slot < g_max_sessions )
   {
      if( g_sessions[ slot ] )
         retval = g_sessions[ slot ]->ip_addr;
   }

   return retval;
}

void session_is_using_tls( )
{
   if( gtp_session )
      gtp_session->using_tls = true;
}

void session_inc_sql_count( )
{
   if( gtp_session )
      ++gtp_session->sql_count;
}

size_t first_other_session_id( const string& var_name, const string& value )
{
   guard g( g_session_mutex );

   size_t id = 0;

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      session* p_session = g_sessions[ i ];

      if( p_session && ( p_session != gtp_session ) )
      {
         if( value.empty( ) )
         {
            if( !p_session->variables.count( var_name ) )
            {
               id = p_session->id;

               break;
            }
         }
         else
         {
            if( p_session->variables.count( var_name )
             && p_session->variables[ var_name ] == value )
            {
               id = p_session->id;

               break;
            }
         }
      }
   }

   return id;
}

bool has_session_with_ip_addr( const string& ip_addr, const string& blockchain )
{
   guard g( g_session_mutex );

   // NOTE: Need to ignore suffix if was included.
   string::size_type pos = blockchain.find( '_' );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ]
       && ( g_sessions[ i ]->ip_addr == ip_addr )
       && ( g_sessions[ i ]->blockchain == blockchain.substr( 0, pos ) ) )
         return true;
   }

   return false;
}

session* get_session_pointer( size_t sess_id )
{
   guard g( g_session_mutex );

   session* p_session = 0;

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && g_sessions[ i ]->id == sess_id )
      {
         p_session = g_sessions[ i ];

         break;
      }
   }

   return p_session;
}

string get_random_same_port_peer_ip_addr( const string& empty_value )
{
   guard g( g_session_mutex );

   string retval( empty_value );
   vector< string > peer_ip_addresses;

   string peer_var_name( get_special_var_name( e_special_var_peer ) );
   string port_var_name( get_special_var_name( e_special_var_port ) );

   string port( get_session_variable( port_var_name ) );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && gtp_session != g_sessions[ i ]
       && g_sessions[ i ]->variables.count( peer_var_name )
       && g_sessions[ i ]->variables.count( port_var_name )
       && port == g_sessions[ i ]->variables[ port_var_name ] )
         peer_ip_addresses.push_back( g_sessions[ i ]->ip_addr );
   }

   if( !peer_ip_addresses.empty( ) )
   {
      if( peer_ip_addresses.size( ) == 1 )
         retval = peer_ip_addresses[ 0 ];
      else
         retval = peer_ip_addresses[ rand( ) % peer_ip_addresses.size( ) ];
   }

   return retval;
}

void list_all_sessions( ostream& os, bool inc_dtms,
 bool include_progress, const string* p_blockchains = 0 )
{
   guard g( g_session_mutex );

   map< size_t, string > sessions;

   set< string > blockchains;

   if( p_blockchains )
      split( *p_blockchains, blockchains );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] )
      {
         if( blockchains.size( ) )
         {
            if( !blockchains.count( g_sessions[ i ]->blockchain ) )
               continue;
         }

         stringstream ss;

         ss << g_sessions[ i ]->id;

         if( gtp_session && gtp_session->id == g_sessions[ i ]->id )
            ss << '*';
         else if( g_condemned_sessions.count( g_sessions[ i ]->id )
          || ( gtp_session && gtp_session->condemned_sessions.count( g_sessions[ i ]->id ) ) )
            ss << '~';

         if( inc_dtms )
         {
            ss << ' ' << g_sessions[ i ]->dtm_created.as_string( true, false )
             << ' ' << g_sessions[ i ]->dtm_last_cmd.as_string( true, false );
         }

         ss << ' ' << g_sessions[ i ]->last_cmd;

         if( include_progress && g_sessions[ i ]->is_peer_session )
            ss << '<' << g_sessions[ i ]->file_hashes_to_get.size( ) << ':' << g_sessions[ i ]->file_hashes_to_put.size( ) << '>';

         ss << ' ';

         if( !g_sessions[ i ]->is_peer_session )
            ss << g_sessions[ i ]->p_storage_handler->get_name( );
         else
            ss << ( g_sessions[ i ]->blockchain.empty( ) ? string( c_str_peer ) : g_sessions[ i ]->blockchain );

         if( g_sessions[ i ]->is_support_session )
            ss << '+';
         else if( g_sessions[ i ]->p_storage_handler->get_is_locked_for_admin( ) )
            ss << '*';

         string uid( g_sessions[ i ]->uid );

         if( uid.empty( ) )
            uid = g_sessions[ i ]->ip_addr.empty( ) ? c_uid_unknown : g_sessions[ i ]->ip_addr;
         else
         {
            // NOTE: If the session's "uid" is in the form <key>:<uid> then strip off the key
            // so that just the uid is displayed (with quotes surrounding so it's clearly not
            // a key). This format is not expected to be found for ops that are stored in the
            // transaction log (as only keys are stored in the log) but makes it a bit easier
            // for an administrator to identify "who is logged in" when it's used for some of
            // the more common non-logged operations such as "perform_fetch".
            string::size_type pos = uid.find( ':' );

            if( pos != string::npos )
               uid = '"' + uid.substr( pos + 1 ) + '"';
         }

         ss << ' ' << uid;

         if( !g_sessions[ i ]->is_peer_session )
            ss << ' ' << g_sessions[ i ]->instance_registry.size( ) << ':' << g_sessions[ i ]->next_handle;
         else
            ss << ' ' << g_sessions[ i ]->peer_files_uploaded << ':' << g_sessions[ i ]->peer_bytes_uploaded;

         if( !g_sessions[ i ]->is_peer_session )
            ss << ' ' << g_sessions[ i ]->sql_count << ':' << g_sessions[ i ]->cache_count;
         else
            ss << ' ' << g_sessions[ i ]->peer_files_downloaded << ':' << g_sessions[ i ]->peer_bytes_downloaded;

         ss << ' ';

         if( !g_sessions[ i ]->using_tls )
            ss << '=';
         else
            ss << '#';

         ss << g_sessions[ i ]->session_commands_executed;

         if( g_sessions[ i ]->is_captured )
            ss << '*';

         if( include_progress )
         {
            string progress_message;
            string progress_message_name( get_special_var_name( e_special_var_progress_message ) );

            if( g_sessions[ i ]->variables.count( progress_message_name ) )
               progress_message = g_sessions[ i ]->variables[ progress_message_name ];

            if( !progress_message.empty( ) )
               ss << ' ' << progress_message;
         }

         sessions.insert( make_pair( g_sessions[ i ]->id, ss.str( ) ) );
      }
   }

   // NOTE: The 'sessions' map is used to ensure that the output is ordered by session id.
   for( map< size_t, string >::iterator i = sessions.begin( ); i != sessions.end( ); ++i )
      os << i->second << '\n';
}

void list_sessions( ostream& os, bool inc_dtms,
 bool include_progress, const string* p_blockchains )
{
   guard g( g_session_mutex, "list_sessions" );

   list_all_sessions( os, inc_dtms, include_progress, p_blockchains );
}

command_handler& get_session_command_handler( )
{
   if( !gtp_session )
      throw runtime_error( "no session to return command handler for" );

   return gtp_session->cmd_handler;
}

void server_command( const char* p_cmd )
{
   string cmd;

   if( p_cmd )
      cmd = string( p_cmd );

   if( cmd == c_server_command_mutexes )
   {
      cerr << "[mutexes]" << endl;

      list_trace_mutex_lock_ids( cerr );
   }
   else if( cmd == c_server_command_sessions )
   {
      cerr << "[sessions]" << endl;

      list_all_sessions( cerr, true, true );
   }
   else
      cerr << "available commands: " << c_server_command_mutexes << " and " << c_server_command_sessions << endl;
}

session_file_buffer_access::session_file_buffer_access( )
 :
 size( 0 ),
 p_buffer( 0 )
{
   if( gtp_session && gtp_session->buffer_is_locked )
      throw runtime_error( "unable to access session buffer" );

   unsigned int bufsize = ( get_files_area_item_max_size( ) * c_max_file_buffer_expansion );

   // NOTE: If is not a session then will simply allocate a buffer
   // (which might be necessary when constructing session objects).
   if( !gtp_session )
      p_buffer = new unsigned char[ bufsize ];
   else
   {
      if( gtp_session->up_buffer.get( ) )
         memset( gtp_session->up_buffer.get( ), 0, bufsize );
      else
         gtp_session->up_buffer.reset( new unsigned char[ bufsize ] );

      gtp_session->buffer_is_locked = true;

      p_buffer = gtp_session->up_buffer.get( );
   }

   size = bufsize;
}

session_file_buffer_access::~session_file_buffer_access( )
{
   if( !gtp_session )
      delete p_buffer;
   else
      gtp_session->buffer_is_locked = false;
}

void session_file_buffer_access::copy_to_string( string& str, size_t offset, size_t length ) const
{
   if( length )
   {
      str.resize( length );
      memcpy( &str[ 0 ], p_buffer + offset, length );
   }
   else
   {
      str.resize( size - offset );
      memcpy( &str[ 0 ], p_buffer + offset, size - offset );
   }
}

void session_file_buffer_access::copy_from_string( const string& str, size_t offset, bool allow_shrink )
{
   unsigned int bufsize = ( get_files_area_item_max_size( ) * c_max_file_buffer_expansion );

   size_t str_size = str.size( );

   if( str_size + offset > bufsize )
      throw runtime_error( "copy_from_string too large for session_file_buffer_access" );

   if( allow_shrink || ( str_size > size ) )
      size = str_size;

   memcpy( p_buffer + offset, str.data( ), str_size );
}

void increment_peer_files_uploaded( int64_t bytes )
{
   if( gtp_session )
   {
      ++gtp_session->peer_files_uploaded;
      gtp_session->peer_bytes_uploaded += bytes;
   }
}

void increment_peer_files_downloaded( int64_t bytes )
{
   if( gtp_session )
   {
      ++gtp_session->peer_files_downloaded;
      gtp_session->peer_bytes_downloaded += bytes;
   }
}

void increment_session_commands_executed( )
{
   if( gtp_session )
      ++gtp_session->session_commands_executed;
}

void set_slowest_if_applicable( )
{
   guard g( g_session_mutex );

   if( gtp_session )
   {
      date_time now( date_time::local( ) );

      uint64_t elapsed = seconds_between( gtp_session->dtm_last_cmd, now );

      string previous( get_session_variable( get_special_var_name( e_special_var_slowest ) ) );

      uint64_t prev_secs = 0;

      if( !previous.empty( ) )
         prev_secs = from_string< uint64_t >( previous );

      if( elapsed > prev_secs )
         set_session_variable( get_special_var_name( e_special_var_slowest ), to_string( elapsed ) );
   }
}

void session_progress_settings( size_t& seconds, progress* p_progress )
{
   guard g( g_session_mutex );

   seconds = c_default_seconds;

   if( gtp_session )
   {
      string progress_secs_name( get_special_var_name( e_special_var_progress_seconds ) );

      if( gtp_session->variables.count( progress_secs_name ) )
         seconds = from_string< size_t >( gtp_session->variables[ progress_secs_name ] );
   }

   if( p_progress )
      p_progress->num_seconds = seconds;
}

void set_session_progress_message( const string& progress_message )
{
   guard g( g_session_mutex );

   if( gtp_session )
   {
      string progress_message_name( get_special_var_name( e_special_var_progress_message ) );

      if( progress_message.empty( ) )
         gtp_session->variables.erase( progress_message_name );
      else
         gtp_session->variables[ progress_message_name ] = progress_message;
   }
}

void set_last_session_cmd( const string& cmd )
{
   guard g( g_session_mutex );

   if( gtp_session )
   {
      gtp_session->last_cmd = cmd;
      gtp_session->dtm_last_cmd = date_time::local( );
   }
}

bool set_session_sync_time( const string* p_check_blockchain,
 bool matching_own_ip_address, int num_seconds, const string* p_sync_var_name )
{
   guard g( g_session_mutex );

   bool retval = false;

   if( gtp_session )
   {
      string blockchain( gtp_session->blockchain );

      gtp_session->sync_time = unix_time( );

      if( p_check_blockchain )
      {
         for( size_t i = 0; i < g_max_sessions; i++ )
         {
            if( g_sessions[ i ]
             && ( g_sessions[ i ]->id != gtp_session->id )
             && ( *p_check_blockchain == g_sessions[ i ]->blockchain )
             && ( !matching_own_ip_address || ( g_sessions[ i ]->ip_addr == gtp_session->ip_addr ) ) )
            {
               int64_t other_sync_time = g_sessions[ i ]->sync_time;

               if( other_sync_time )
               {
                  int64_t time_diff = 0;

                  if( gtp_session->sync_time > other_sync_time )
                     time_diff = gtp_session->sync_time - other_sync_time;
                  else
                     time_diff = other_sync_time - gtp_session->sync_time;

                  if( time_diff <= num_seconds )
                  {
                     retval = true;

                     if( p_sync_var_name && !p_sync_var_name->empty( ) )
                     {
                        gtp_session->variables[ *p_sync_var_name ] = c_true_value;
                        g_sessions[ i ]->variables[ *p_sync_var_name ] = c_true_value;
                     }

                     break;
                  }
               }
            }
         }
      }
   }

   return retval;
}

void condemn_session( size_t sess_id, int num_seconds, bool force_uncapture, bool wait_until_term )
{
   guard g( g_session_mutex );

   // NOTE: This function is not intended to be called for self
   // termination ("condemn_this_session" is for that purpose).
   if( gtp_session && ( sess_id == gtp_session->id ) )
      return;

   date_time dtm( date_time::local( ) + ( seconds )num_seconds );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && ( g_sessions[ i ]->id == sess_id ) )
      {
         if( force_uncapture )
            g_sessions[ i ]->is_captured = false;

         if( !wait_until_term )
            g_condemned_sessions.insert( make_pair( g_sessions[ i ]->id, dtm ) );
         else
            gtp_session->condemned_sessions.insert( make_pair( g_sessions[ i ]->id, dtm ) );
      }
   }
}

void condemn_this_session( )
{
   guard g( g_session_mutex );

   if( gtp_session )
      g_condemned_sessions.insert( make_pair( gtp_session->id, date_time::local( ) ) );
}

void condemn_matching_sessions( int num_seconds, bool wait_until_term )
{
   guard g( g_session_mutex );

   if( gtp_session )
   {
      size_t sess_id = gtp_session->id;

      string ip_addr( gtp_session->ip_addr );
      string blockchain( gtp_session->blockchain );

      date_time dtm( date_time::local( ) + ( seconds )num_seconds );

      for( size_t i = 0; i < g_max_sessions; i++ )
      {
         if( g_sessions[ i ] && ( g_sessions[ i ]->id != sess_id )
          && ( g_sessions[ i ]->ip_addr == ip_addr ) && ( g_sessions[ i ]->blockchain == blockchain ) )
         {
            if( !wait_until_term )
               g_condemned_sessions.insert( make_pair( g_sessions[ i ]->id, dtm ) );
            else
               gtp_session->condemned_sessions.insert( make_pair( g_sessions[ i ]->id, dtm ) );
         }
      }
   }
}

void condemn_all_other_sessions( int num_seconds, bool force_uncapture, bool wait_until_term )
{
   guard g( g_session_mutex );

   size_t sess_id = 0;

   if( gtp_session )
      sess_id = gtp_session->id;

   date_time dtm( date_time::local( ) + ( seconds )num_seconds );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && ( g_sessions[ i ]->id != sess_id ) )
      {
         if( force_uncapture )
            g_sessions[ i ]->is_captured = false;

         if( !wait_until_term )
            g_condemned_sessions.insert( make_pair( g_sessions[ i ]->id, dtm ) );
         else
            gtp_session->condemned_sessions.insert( make_pair( g_sessions[ i ]->id, dtm ) );
      }
   }
}

bool is_condemned_session( )
{
   guard g( g_session_mutex );

   return gtp_session
    && ( ( g_condemned_sessions.count( gtp_session->id )
    && g_condemned_sessions[ gtp_session->id ] <= date_time::local( ) )
    || ( g_session_timeout && ( date_time::local( ) - gtp_session->dtm_last_cmd ) > g_session_timeout ) );
}

void capture_session( size_t sess_id )
{
   guard g( g_session_mutex );

   if( gtp_session && !sess_id )
      sess_id = gtp_session->id;

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && ( g_sessions[ i ]->id == sess_id ) )
      {
         g_sessions[ i ]->is_captured = true;

         break;
      }
   }
}

void capture_all_other_sessions( )
{
   guard g( g_session_mutex );

   size_t sess_id = 0;

   if( gtp_session )
      sess_id = gtp_session->id;

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && ( g_sessions[ i ]->id != sess_id ) )
         g_sessions[ i ]->is_captured = true;
   }
}

bool is_captured_session( )
{
   return ( gtp_session && gtp_session->is_captured );
}

bool has_any_matching_session( bool support_only )
{
   guard g( g_session_mutex );

   bool retval = false;

   if( gtp_session )
   {
      size_t sess_id = gtp_session->id;

      string ip_addr( gtp_session->ip_addr );

      string blockchain( gtp_session->blockchain );

      for( size_t i = 0; i < g_max_sessions; i++ )
      {
         if( g_sessions[ i ]
          && ( g_sessions[ i ]->id != sess_id )
          && ( g_sessions[ i ]->blockchain == blockchain )
          && ( ( support_only && g_sessions[ i ]->is_support_session )
          || ( !support_only && !g_sessions[ i ]->is_support_session ) )
          && ( ( support_only && ( g_sessions[ i ]->ip_addr == ip_addr ) )
          || ( !support_only && ( g_sessions[ i ]->ip_addr != ip_addr ) ) ) )
         {
            retval = true;

            break;
         }
      }
   }

   return retval;
}

void release_session( size_t sess_id, bool wait_until_term )
{
   guard g( g_session_mutex );

   if( !sess_id && gtp_session )
   {
      wait_until_term = false;

      sess_id = gtp_session->id;
   }

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && ( g_sessions[ i ]->id == sess_id ) )
      {
         if( !wait_until_term )
            g_sessions[ i ]->is_captured = false;
         else
            gtp_session->release_sessions.insert( sess_id );

         break;
      }
   }
}

void release_all_other_sessions( bool wait_until_term )
{
   guard g( g_session_mutex );

   size_t sess_id = 0;

   if( gtp_session )
      sess_id = gtp_session->id;

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && ( g_sessions[ i ]->id != sess_id ) )
      {
         if( !wait_until_term )
            g_sessions[ i ]->is_captured = false;
         else
            gtp_session->release_sessions.insert( sess_id );
      }
   }
}

bool session_skip_fk_fetches( )
{
   return gtp_session && gtp_session->skip_fk_fetches;
}

void session_skip_fk_fetches( bool skip_fk_fetches )
{
   if( gtp_session )
      gtp_session->skip_fk_fetches = skip_fk_fetches;
}

bool session_skip_validation( )
{
   return gtp_session && gtp_session->skip_validation;
}

void session_skip_validation( bool skip_validation )
{
   if( gtp_session )
      gtp_session->skip_validation = skip_validation;
}

bool session_skip_is_constained( )
{
   return gtp_session && gtp_session->skip_is_constrained;
}

void session_skip_is_constained( bool skip_is_constrained )
{
   if( gtp_session )
      gtp_session->skip_is_constrained = skip_is_constrained;
}

bool get_script_reconfig( )
{
   guard g( g_mutex );

   return g_script_reconfig;
}

string get_gpg_password( )
{
   if( g_gpg_password.length( ) < c_minimum_encrypted_password_size )
      return g_gpg_password;
   else
      return decrypt_data( g_gpg_password );
}

string get_rpc_password( )
{
   if( g_rpc_password.length( ) < c_minimum_encrypted_password_size )
      return g_rpc_password;
   else
      return decrypt_data( g_rpc_password );
}

string get_sql_password( )
{
   string pwd;

   if( gtp_session
    && ( gtp_session->p_storage_handler->get_name( ) == c_meta_storage_name ) )
   {
      if( g_sql_password.empty( ) )
         pwd = "."; // i.e. used to pass scripts a non-empty password value
      else if( g_sql_password.length( ) < c_minimum_encrypted_password_size )
         pwd = g_sql_password;
      else
         pwd = decrypt_data( g_sql_password );
   }

   return pwd;
}

unsigned int get_test_peer_port( )
{
   return g_test_peer_port;
}

void set_test_peer_port( unsigned int port )
{
   g_test_peer_port = port;
}

string get_encrypted_gpg_password( )
{
   return g_gpg_password;
}

string get_encrypted_pem_password( )
{
   return g_pem_password;
}

string get_encrypted_rpc_password( )
{
   return g_rpc_password;
}

string get_encrypted_sql_password( )
{
   return g_sql_password;
}

string get_encrypted_pop3_password( )
{
   return g_pop3_password;
}

string get_encrypted_smtp_password( )
{
   return g_smtp_password;
}

unsigned int get_session_timeout( )
{
   guard g( g_mutex );

   return g_session_timeout;
}

void set_session_timeout( unsigned int seconds )
{
   guard g( g_mutex );

   g_session_timeout = seconds;
}

string get_session_blockchain( )
{
   if( !gtp_session )
      return g_empty_string;

   return gtp_session->blockchain;
}

string get_session_identity_suffix( )
{
   if( !gtp_session )
      return g_empty_string;

   return gtp_session->identity_suffix;
}

size_t get_num_sessions_for_blockchain(
 const string& blockchain, bool matching_own_ip_address, bool excluding_own_session )
{
   guard g( g_session_mutex );

   size_t num_sessions = 0;

   if( !gtp_session && matching_own_ip_address )
      throw runtime_error( "unexpected call to 'get_num_sessions_for_blockchain' from non-session" );

   string own_ip_addr;

   if( gtp_session )
      own_ip_addr = gtp_session->ip_addr;

   if( !blockchain.empty( ) )
   {
      for( size_t i = 0; i < g_max_sessions; i++ )
      {
         if( !g_sessions[ i ] )
            continue;

         if( excluding_own_session && ( gtp_session == g_sessions[ i ] ) )
            continue;

         if( blockchain == g_sessions[ i ]->blockchain )
         {
            if( matching_own_ip_address && ( own_ip_addr != g_sessions[ i ]->ip_addr ) )
               continue;

            ++num_sessions;
         }
      }
   }

   return num_sessions;
}

bool get_storage_using_verbose_logging( )
{
   return gtp_session->p_storage_handler->is_using_verbose_logging( );
}

void add_peer_file_hash_for_get( const string& hash,
 bool check_for_supporters, bool add_at_front, const string* p_hash_to_remove )
{
   guard g( g_session_mutex );

   if( !gtp_session )
      throw runtime_error( "invalid call to add_peer_file_hash_for_get from non-session" );

   string hash_to_remove;

   if( p_hash_to_remove )
      hash_to_remove = *p_hash_to_remove;

   if( !hash_to_remove.empty( ) )
   {
      deque< string >::iterator i = find(
       gtp_session->file_hashes_to_get.begin( ),
       gtp_session->file_hashes_to_get.end( ), hash_to_remove );

      if( i != gtp_session->file_hashes_to_get.end( ) )
         gtp_session->file_hashes_to_get.erase( i );
   }

   if( !check_for_supporters )
   {
      if( !add_at_front )
         gtp_session->file_hashes_to_get.push_back( hash );
      else
         gtp_session->file_hashes_to_get.push_front( hash );
   }
   else
   {
      session* p_least_full = 0;

      for( size_t i = 0; i < g_max_sessions; i++ )
      {
         session* p_next = g_sessions[ i ];

         if( p_next
          && p_next->is_support_session
          && ( p_next->ip_addr == gtp_session->ip_addr )
          && ( p_next->blockchain == gtp_session->blockchain ) )
         {
            if( !p_least_full
             || ( p_next->file_hashes_to_get.size( ) < p_least_full->file_hashes_to_get.size( ) ) )
               p_least_full = p_next;
         }
      }

      if( !p_least_full )
         p_least_full = gtp_session;

      if( !add_at_front )
         p_least_full->file_hashes_to_get.push_back( hash );
      else
         p_least_full->file_hashes_to_get.push_front( hash );
   }
}

string top_next_peer_file_hash_to_get( bool take_from_supporter, bool* p_any_supporter_has )
{
   guard g( g_session_mutex );

   string hash;

   if( !gtp_session )
      throw runtime_error( "invalid call to top_next_peer_file_hash_to_get from non-session" );

   if( !gtp_session->file_hashes_to_get.empty( ) )
      hash = gtp_session->file_hashes_to_get.front( );

   if( hash.empty( ) && ( take_from_supporter || p_any_supporter_has ) )
   {
      session* p_most_full = 0;
      size_t most_full_num = 0;

      for( size_t i = 0; i < g_max_sessions; i++ )
      {
         session* p_next = g_sessions[ i ];

         // NOTE: Need to ensure it is not self as support sessions
         // can also take file hashes from other support sessions.
         if( p_next
          && ( p_next != gtp_session )
          && p_next->is_support_session
          && ( p_next->ip_addr == gtp_session->ip_addr )
          && ( p_next->blockchain == gtp_session->blockchain ) )
         {
            size_t next_num = p_next->file_hashes_to_get.size( );

            if( next_num )
            {
               // NOTE: As a support session won't "pop" the front hash
               // until it has finished both fetching and processing of
               // this file it can only take file hashes from the front
               // of the deque if there is more than one present (as it
               // must restore the current front file hash).
               if( ( next_num > 1 ) && take_from_supporter )
               {
                  if( !p_most_full || ( most_full_num < next_num ) )
                  {
                     p_most_full = p_next;
                     most_full_num = next_num;
                  }
               }

               if( p_any_supporter_has )
                  *p_any_supporter_has = true;

               if( !take_from_supporter )
                  break;
            }
         }
      }

      if( p_most_full )
         hash = take_front_from_deque( p_most_full->file_hashes_to_get,
          gtp_session->file_hashes_to_get, c_take_total_divided_by_ten );
   }

   return hash;
}

void pop_next_peer_file_hash_to_get( )
{
   guard g( g_session_mutex );

   if( !gtp_session )
      throw runtime_error( "invalid call to pop_next_peer_file_hash_to_get from non-session" );

   if( !gtp_session->file_hashes_to_get.empty( ) )
      gtp_session->file_hashes_to_get.pop_front( );
}

void add_peer_file_hash_for_put( const string& hash, bool check_for_supporters )
{
   guard g( g_session_mutex );

   if( !gtp_session )
      throw runtime_error( "invalid call to add_peer_file_hash_for_put from non-session" );

   if( find( gtp_session->file_hashes_to_put.begin( ),
    gtp_session->file_hashes_to_put.end( ), hash ) == gtp_session->file_hashes_to_put.end( ) )
   {
      if( !check_for_supporters )
         gtp_session->file_hashes_to_put.push_back( hash );
      else
      {
         session* p_least_full = 0;

         for( size_t i = 0; i < g_max_sessions; i++ )
         {
            session* p_next = g_sessions[ i ];

            if( p_next
             && p_next->is_support_session
             && ( p_next->ip_addr == gtp_session->ip_addr )
             && ( p_next->blockchain == gtp_session->blockchain ) )
            {
               if( !p_least_full
                || ( p_next->file_hashes_to_put.size( ) < p_least_full->file_hashes_to_put.size( ) ) )
                  p_least_full = p_next;
            }
         }

         if( !p_least_full )
            p_least_full = gtp_session;

         p_least_full->file_hashes_to_put.push_back( hash );
      }
   }
}

void add_peer_file_hash_for_put_for_all_peers( const string& hash,
 const string* p_blockchain, const string* p_session_variable, bool include_self, size_t session_id_to_skip )
{
   guard g( g_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ]
       && g_sessions[ i ]->is_peer_session
       && ( g_sessions[ i ]->id != session_id_to_skip )
       && ( include_self || g_sessions[ i ] != gtp_session )
       && ( !p_blockchain || ( *p_blockchain == g_sessions[ i ]->blockchain ) )
       && ( !p_session_variable || g_sessions[ i ]->variables.count( *p_session_variable ) ) )
         g_sessions[ i ]->file_hashes_to_put.push_back( hash );
   }
}

string top_next_peer_file_hash_to_put( bool take_from_supporter, bool* p_any_supporter_has )
{
   guard g( g_session_mutex );

   string hash;

   if( !gtp_session )
      throw runtime_error( "invalid call to top_next_peer_file_hash_to_put from non-session" );

   if( !gtp_session->file_hashes_to_put.empty( ) )
      hash = gtp_session->file_hashes_to_put.front( );

   if( hash.empty( ) && ( take_from_supporter || p_any_supporter_has ) )
   {
      session* p_most_full = 0;
      size_t most_full_num = 0;

      for( size_t i = 0; i < g_max_sessions; i++ )
      {
         session* p_next = g_sessions[ i ];

         if( p_next
          && ( p_next != gtp_session )
          && p_next->is_support_session
          && ( p_next->ip_addr == gtp_session->ip_addr )
          && ( p_next->blockchain == gtp_session->blockchain ) )
         {
            size_t next_num = p_next->file_hashes_to_put.size( );

            if( next_num )
            {
               if( ( next_num > 1 ) && take_from_supporter )
               {
                  if( !p_most_full || ( most_full_num < next_num ) )
                  {
                     p_most_full = p_next;
                     most_full_num = next_num;
                  }
               }

               if( p_any_supporter_has )
                  *p_any_supporter_has = true;

               if( !take_from_supporter )
                  break;
            }
         }
      }

      if( p_most_full )
         hash = take_front_from_deque( p_most_full->file_hashes_to_put,
          gtp_session->file_hashes_to_put, c_take_total_divided_by_ten );
   }

   return hash;
}

void pop_next_peer_file_hash_to_put( )
{
   guard g( g_session_mutex );

   if( !gtp_session )
      throw runtime_error( "invalid call to pop_next_peer_file_hash_to_put from non-session" );

   if( !gtp_session->file_hashes_to_put.empty( ) )
      gtp_session->file_hashes_to_put.pop_front( );
}

bool any_peer_still_has_file_hash_to_put(
 const string& hash, const string* p_blockchain )
{
   guard g( g_session_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ]
       && g_sessions[ i ]->is_peer_session
       && ( !p_blockchain || ( *p_blockchain == g_sessions[ i ]->blockchain ) ) )
      {
         for( size_t j = 0; j < g_sessions[ i ]->file_hashes_to_put.size( ); j++ )
         {
            if( g_sessions[ i ]->file_hashes_to_put[ j ] == hash )
               return true;
         }
      }
   }

   return false;
}

void add_peer_mapped_hash_info( const string& key, const string& hash, const string& info )
{
   guard g( g_mapping_mutex );

   pair< string, string > mapped_pair;

   string::size_type pos = info.find( ':' );

   mapped_pair.first = hex_decode( info.substr( 0, pos ) );

   if( pos != string::npos )
      mapped_pair.second = hex_decode( info.substr( pos + 1 ) );

   g_mapped_hash_values[ key ].insert( make_pair( hex_decode( hash ), mapped_pair ) );
}

string get_peer_mapped_hash_info( const string& key, const string& hash )
{
   guard g( g_mapping_mutex );

   string retval, decoded( hex_decode( hash ) );

   if( g_mapped_hash_values[ key ].count( decoded ) )
   {
      pair< string, string > mapped_pair;

      mapped_pair = g_mapped_hash_values[ key ][ decoded ];

      if( !mapped_pair.first.empty( ) )
      {
         retval += hex_encode( mapped_pair.first );

         if( !mapped_pair.second.empty( ) )
            retval += ':' + hex_encode( mapped_pair.second );
      }
   }

   return retval;
}

void clear_peer_mapped_hash( const string& key, const string& hash )
{
   guard g( g_mapping_mutex );

   g_mapped_hash_values[ key ].erase( hex_decode( hash ) );
}

void clear_all_peer_mapped_hashes( const string& key )
{
   guard g( g_mapping_mutex );

   g_mapped_hash_values[ key ].clear( );
}

void set_default_session_variables( int port )
{
   guard g( g_session_mutex );

   if( port )
      set_session_variable( get_special_var_name( e_special_var_port ), to_string( port ) );

   set_session_variable( get_special_var_name( e_special_var_storage ), g_default_storage );
}

bool has_raw_session_variable( const string& name, size_t sess_id )
{
   bool retval = false;

   unique_ptr< guard > up_guard;
   unique_ptr< restorable< session* > > up_temp_session;

   if( sess_id )
   {
      up_guard.reset( new guard( g_session_mutex, "has_raw_session_variable" ) );
      up_temp_session.reset( new restorable< session* >( gtp_session, get_session_pointer( sess_id ) ) );
   }

   if( gtp_session )
      retval = gtp_session->variables.count( name );

   return retval;
}

string get_raw_session_variable( const string& name, size_t sess_id )
{
   string retval;

   bool found = false;

   unique_ptr< guard > up_guard;
   unique_ptr< restorable< session* > > up_temp_session;

   if( sess_id )
   {
      up_guard.reset( new guard( g_session_mutex, "get_raw_session_variable" ) );
      up_temp_session.reset( new restorable< session* >( gtp_session, get_session_pointer( sess_id ) ) );
   }

   if( gtp_session )
   {
      string progress_clear_name( get_special_var_name( e_special_var_progress_clear ) );
      string progress_value_name( get_special_var_name( e_special_var_progress_value ) );

      if( gtp_session->using_tls )
      {
         string tls_name( get_special_var_name( e_special_var_tls ) );

         if( !gtp_session->variables.count( tls_name ) )
            gtp_session->variables.insert( make_pair( tls_name, c_true_value ) );
      }

      if( name == progress_clear_name )
      {
         string progress_count_name( get_special_var_name( e_special_var_progress_count ) );
         string progress_fracs_name( get_special_var_name( e_special_var_progress_fracs ) );
         string progress_prior_name( get_special_var_name( e_special_var_progress_prior ) );
         string progress_total_name( get_special_var_name( e_special_var_progress_total ) );

         gtp_session->variables[ progress_count_name ] = "0";
         gtp_session->variables[ progress_fracs_name ] = "0";
         gtp_session->variables[ progress_prior_name ] = "0";
         gtp_session->variables[ progress_total_name ] = "0";
      }
      else if( name == progress_value_name )
      {
         string progress_count_name( get_special_var_name( e_special_var_progress_count ) );
         string progress_total_name( get_special_var_name( e_special_var_progress_total ) );

         if( gtp_session->variables.count( progress_count_name ) && gtp_session->variables.count( progress_total_name ) )
         {
            size_t fracs = 0;
            size_t prior = 0;

            unsigned long count = 0;
            unsigned long total = 0;

            string progress_fracs_name( get_special_var_name( e_special_var_progress_fracs ) );

            if( gtp_session->variables.count( progress_fracs_name ) )
               fracs = from_string< size_t >( gtp_session->variables[ progress_fracs_name ] );

            string progress_prior_name( get_special_var_name( e_special_var_progress_prior ) );

            if( gtp_session->variables.count( progress_prior_name ) )
               prior = from_string< size_t >( gtp_session->variables[ progress_prior_name ] );

            if( gtp_session->variables.count( progress_count_name ) )
               count = from_string< unsigned long >( gtp_session->variables[ progress_count_name ] );

            if( gtp_session->variables.count( progress_total_name ) )
               total = from_string< unsigned long >( gtp_session->variables[ progress_total_name ] );

            gtp_session->variables[ progress_value_name ] = format_percentage( fracs, prior, count, total );

            gtp_session->variables[ progress_fracs_name ] = to_string( fracs );
            gtp_session->variables[ progress_prior_name ] = to_string( prior );
         }
      }

      if( name.find( c_special_variable_queue_prefix ) == 0 )
      {
         if( gtp_session->deque_variables.count( name ) )
         {
            found = true;

            if( gtp_session->deque_variables[ name ].size( ) )
            {
               retval = gtp_session->deque_variables[ name ].front( );
               gtp_session->deque_variables[ name ].pop_front( );
            }

            if( !gtp_session->deque_variables[ name ].size( ) )
               gtp_session->deque_variables.erase( name );
         }
      }
      else if( name.find( c_special_variable_mapped_prefix ) == 0 )
      {
         string::size_type pos = name.find( ':' );

         if( gtp_session->mapped_variables.count( name.substr( 0, pos ) ) )
         {
            found = true;

            if( pos == string::npos )
            {
               map< string, string >::const_iterator ci;

               for( ci = gtp_session->mapped_variables[ name ].begin( ); ci != gtp_session->mapped_variables[ name ].end( ); ++ci )
               {
                  if( !retval.empty( ) )
                     retval += '\n';
                  retval += ci->first + ' ' + ci->second;
               }
            }
            else
            {
               if( gtp_session->mapped_variables[ name.substr( 0, pos ) ].count( name.substr( pos + 1 ) ) )
                  retval = gtp_session->mapped_variables[ name.substr( 0, pos ) ][ name.substr( pos + 1 ) ];
            }
         }
      }
      else if( gtp_session->variables.count( name ) )
      {
         found = true;

         // NOTE: Prevents access to "@shared_secret" value if is not the owning session.
         if( !sess_id || ( name != get_special_var_name( e_special_var_shared_secret ) ) )
            retval = gtp_session->variables[ name ];
      }
      else if( name.find_first_of( "?*" ) != string::npos )
      {
         found = true;

         map< string, string >::const_iterator ci;

         for( ci = gtp_session->variables.begin( ); ci != gtp_session->variables.end( ); ++ci )
         {
            if( wildcard_match( name, ci->first ) )
            {
               // NOTE: Prevents access to "@shared_secret" value if is not the owning session.
               if( !sess_id || ( ci->first != get_special_var_name( e_special_var_shared_secret ) ) )
               {
                  if( !retval.empty( ) )
                     retval += "\n";

                  retval += ci->first + ' ' + ci->second;
               }
            }
         }

         map< string, deque< string > >::const_iterator dci;

         for( dci = gtp_session->deque_variables.begin( ); dci != gtp_session->deque_variables.end( ); ++dci )
         {
            if( wildcard_match( name, dci->first ) )
            {
               if( !retval.empty( ) )
                  retval += "\n";

               retval += dci->first + ' ' + dci->second.front( );

               if( dci->second.size( ) > 1 )
                  retval += " (+" + to_string( dci->second.size( ) - 1 ) + ")";
            }
         }

         map< string, map< string, string > >::const_iterator mci;

         for( mci = gtp_session->mapped_variables.begin( ); mci != gtp_session->mapped_variables.end( ); ++mci )
         {
            if( wildcard_match( name, mci->first ) )
            {
               if( !retval.empty( ) )
                  retval += "\n";

               retval += mci->first + " (" + to_string( mci->second.size( ) ) + ")";
            }
         }
      }
   }

   if( !found )
   {
      if( name == get_special_var_name( e_special_var_set ) )
      {
         if( gtp_session )
         {
            if( !gtp_session->last_set_item.empty( ) )
            {
               retval = gtp_session->last_set_item;

               gtp_session->last_set_item.erase( );
            }
            else
            {
               for( set< string >::const_iterator
                ci = gtp_session->set_items.begin( ); ci != gtp_session->set_items.end( ); ++ci )
               {
                  if( !retval.empty( ) )
                     retval += '\n';

                  retval += *ci;
               }
            }
         }
      }
      else if( name == get_special_var_name( e_special_var_none ) )
         retval = " ";
      else if( name == get_special_var_name( e_special_var_deque ) )
      {
         if( gtp_session )
         {
            if( !gtp_session->last_deque_item.empty( ) )
            {
               retval = gtp_session->last_deque_item;

               gtp_session->last_deque_item.erase( );
            }
            else
            {
               for( deque< string >::const_iterator
                ci = gtp_session->deque_items.begin( ); ci != gtp_session->deque_items.end( ); ++ci )
               {
                  if( !retval.empty( ) )
                     retval += '\n';

                  retval += *ci;
               }
            }
         }
      }
      else if( name == get_special_var_name( e_special_var_algos ) )
      {
         if( gtp_session )
         {
            guard g( g_mutex );
            temporary_algo_prefix tmp_algo_prefix( gtp_session->p_storage_handler->get_name( ) );

            ostringstream osstr;
            output_algos( osstr );
            retval = osstr.str( );
         }
      }
   }

   if( gtp_session && !name.empty( ) && ( name[ 0 ] == '@' ) )
   {
      string temporary_special_name( name + c_temporary_special_variable_suffix );

      if( gtp_session->variables.count( temporary_special_name ) )
      {
         gtp_session->variables[ name ] = gtp_session->variables[ temporary_special_name ];

         if( gtp_session->variables[ name ].empty( ) )
            gtp_session->variables.erase( name );

         gtp_session->variables.erase( temporary_special_name );
      }
   }

   return retval;
}

struct raw_session_variable_getter : variable_getter
{
   raw_session_variable_getter( size_t sess_id ) : sess_id( sess_id ) { }
    
   string get_value( const string& name ) const { return get_raw_session_variable( name, sess_id ); }

   size_t sess_id;
};

bool has_session_variable( const string& name_or_expr, const string* p_sess_id )
{
   return !get_session_variable( name_or_expr, p_sess_id ).empty( );
}

string get_session_variable( const string& name, size_t slot )
{
   guard g( g_session_mutex, "get_session_variable" );

   string retval;

   if( ( slot < g_max_sessions ) && g_sessions[ slot ] )
   {
      if( g_sessions[ slot ]->variables.count( name ) )
         retval = g_sessions[ slot ]->variables[ name ];
   }

   return retval;
}

string get_session_variable( const string& name_or_expr, const string* p_sess_id )
{
   size_t sess_id = 0;

   if( p_sess_id )
      sess_id = from_string< size_t >( *p_sess_id );

   raw_session_variable_getter raw_getter( sess_id );
   variable_expression expr( name_or_expr, raw_getter );

   return expr.get_value( );
}

string get_session_variable_from_matching_blockchain( const string& name,
 const string& check_name, const string& check_value, bool matching_own_ip_address )
{
   guard g( g_session_mutex );

   if( !gtp_session )
      throw runtime_error( "unexpected call to 'get_session_variable_from_matching_blockchain' from non-session" );

   string retval;

   string own_ip_addr( gtp_session->ip_addr );
   string own_blockchain( gtp_session->blockchain );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && ( g_sessions[ i ] != gtp_session ) )
      {
         if( g_sessions[ i ]->blockchain != own_blockchain )
            continue;

         if( matching_own_ip_address && ( g_sessions[ i ]->ip_addr != own_ip_addr ) )
            continue;

         if( !check_name.empty( ) )
         {
            if( check_value.empty( ) )
            {
               if( g_sessions[ i ]->variables.count( check_name ) )
                  continue;
            }
            else if( !g_sessions[ i ]->variables.count( check_name )
             || ( g_sessions[ i ]->variables[ check_name ] != check_value ) )
               continue;
         }

         retval = g_sessions[ i ]->variables[ name ];
         break;
      }
   }

   return retval;
}

bool has_mismatched_variables_for_matching_blockchains(
 const string& name, const string& value, const string* p_name, const string* p_value )
{
   guard g( g_session_mutex );

   if( !gtp_session )
      throw runtime_error( "unexpected call to 'has_mismatched_variables_for_matching_blockchains' from non-session" );

   bool retval = false;

   string own_blockchain( gtp_session->blockchain );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && ( g_sessions[ i ] != gtp_session ) )
      {
         if( g_sessions[ i ]->blockchain != own_blockchain )
            continue;

         bool has_name = g_sessions[ i ]->variables.count( name );

         if( has_name && value.empty( ) )
         {
            retval = true;
            break;
         }

         if( has_name && ( g_sessions[ i ]->variables[ name ] != value ) )
         {
            retval = true;
            break;
         }

         if( p_name )
         {
            has_name = g_sessions[ i ]->variables.count( *p_name );

            if( !has_name && ( !p_value || p_value->empty( ) ) )
               continue;

            if( has_name && ( !p_value || p_value->empty( ) ) )
            {
               retval = true;
               break;
            }

            if( g_sessions[ i ]->variables[ *p_name ] != *p_value )
            {
               retval = true;
               break;
            }
         }
      }
   }

   return retval;
}

void set_session_variable( const string& name, const string& value,
 bool* p_set_special_temporary, command_handler* p_command_handler, const string* p_sess_id )
{
   size_t sess_id = 0;

   if( p_sess_id )
      sess_id = from_string< size_t >( *p_sess_id );

   unique_ptr< guard > up_guard;
   unique_ptr< restorable< session* > > up_temp_session;

   if( sess_id )
   {
      up_guard.reset( new guard( g_session_mutex, "set_session_variable" ) );
      up_temp_session.reset( new restorable< session* >( gtp_session, get_session_pointer( sess_id ) ) );
   }

   if( gtp_session )
   {
      string val( value );

      string old_val;

      if( gtp_session->variables.count( name ) )
         old_val = gtp_session->variables[ name ];

      if( val == get_special_var_name( e_special_var_increment )
       || val == get_special_var_name( e_special_var_decrement ) )
      {
         int num_value = !gtp_session->variables.count( name )
          ? 0 : from_string< int >( gtp_session->variables[ name ] );

         if( val == get_special_var_name( e_special_var_increment ) )
            ++num_value;
         else if( num_value > 0 )
            --num_value;

         if( num_value == 0 )
            val.clear( );
         else
            val = to_string( num_value );
      }

      bool skip_standard_variable = false;

      if( name == get_special_var_name( e_special_var_set ) )
      {
         skip_standard_variable = true;

         if( val.empty( ) )
            gtp_session->set_items.clear( );
         else
         {
            string::size_type pos = val.find( ' ' );

            if( val[ 0 ] == '@' )
            {
               if( val != "@deque" )
                  buffer_file_lines( val.substr( 1 ), gtp_session->set_items );
               else
                  copy( gtp_session->deque_items.begin( ),
                   gtp_session->deque_items.end( ), inserter( gtp_session->set_items, gtp_session->set_items.end( ) ) );
            }
            else if( val[ 0 ] == '~' )
               split( val.substr( 1 ), gtp_session->set_items, '\n' );
            else if( val[ 0 ] == '?' )
            {
               vector< string > check_items;

               split( val.substr( 1 ), check_items );

               bool all_found = true;

               for( size_t i = 0; i < check_items.size( ); i++ )
               {
                  if( !gtp_session->set_items.count( check_items[ i ] ) )
                  {
                     all_found = false;
                     break;
                  }
               }

               if( all_found )
                  gtp_session->last_set_item = c_true_value;
               else
                  gtp_session->last_set_item = c_false_value;

               if( p_set_special_temporary )
                  *p_set_special_temporary = true;
            }
            else if( val == "size" )
            {
               val = to_string( gtp_session->set_items.size( ) );

               gtp_session->last_set_item = val;

               if( p_set_special_temporary )
                  *p_set_special_temporary = true;
            }
            else if( val == "commence" )
            {
               gtp_session->kept_set_items.push_back( gtp_session->set_items );

               gtp_session->set_items.clear( );
            }
            else if( val == "complete" )
            {
               if( gtp_session->kept_set_items.empty( ) )
                  gtp_session->set_items.clear( );
               else
               {
                  gtp_session->set_items = gtp_session->kept_set_items.back( );
                  gtp_session->kept_set_items.pop_back( );
               }
            }
            else
            {
               if( gtp_session->set_items.count( val ) )
               {
                  gtp_session->last_set_item = val;

                  if( p_set_special_temporary )
                     *p_set_special_temporary = true;
               }
               else
               {
                  gtp_session->last_set_item.erase( );
                  gtp_session->set_items.insert( val );
               }
            }
         }
      }
      else if( name == get_special_var_name( e_special_var_cube ) )
      {
         temporary_algo_prefix tmp_algo_prefix( gtp_session->p_storage_handler->get_name( ) );

         bool new_cube = false;

         bool has_space_or_comma = ( val.find_first_of( " ," ) != string::npos );

         if( !has_space_or_comma && ( val.length( ) >= 24 ||
          ( val.length( ) == 5 && val[ 0 ] >= '2' && val[ 0 ] <= '5' ) ) )
         {
            new_cube = true;
            cube tmp_cube( val );
            val = tmp_cube.get_state( );
         }

         if( !new_cube && !old_val.empty( ) )
         {
            string::size_type pos = val.find( ' ' );

            cube tmp_cube( old_val );

            if( val == "flip" )
            {
               tmp_cube.flip( );
               val = tmp_cube.get_state( );
            }
            else if( val == "show" )
            {
               ostringstream osstr;
               tmp_cube.output_sides( osstr );

               val = osstr.str( );

               if( p_set_special_temporary )
                  *p_set_special_temporary = true;
            }
            else if( val.substr( 0, pos ) == "show" )
            {
               ostringstream osstr;
               tmp_cube.output_side( osstr, val.substr( pos + 1 ) );

               val = osstr.str( );

               if( p_set_special_temporary )
                  *p_set_special_temporary = true;
            }
            else if( val == "algos" )
            {
               ostringstream osstr;
               tmp_cube.output_algos( osstr );

               val = osstr.str( );

               if( p_set_special_temporary )
                  *p_set_special_temporary = true;
            }
            else if( val.substr( 0, pos ) == "cubie" )
            {
               ostringstream osstr;
               tmp_cube.output_matching_cubie( osstr, val.substr( pos + 1 ) );

               val = osstr.str( );

               if( p_set_special_temporary )
                  *p_set_special_temporary = true;
            }
            else if( val == "reset" )
            {
               tmp_cube.reset( );
               val = tmp_cube.get_state( );
            }
            else if( val == "state" )
            {
               val = tmp_cube.get_state( false );

               if( p_set_special_temporary )
                  *p_set_special_temporary = true;
            }
            else if( val.substr( 0, pos ) == "train" )
            {
               p_command_handler->output_progress( tmp_cube.type_key( ) + " training..." );
               tmp_cube.train( val.substr( pos + 1 ) );

               val.erase( );

               if( p_set_special_temporary )
                  *p_set_special_temporary = true;
            }
            else if( val.substr( 0, pos ) == "cubies" )
            {
               ostringstream osstr;
               tmp_cube.output_matching_cubies( osstr, val.substr( pos + 1 ) );

               val = osstr.str( );

               if( p_set_special_temporary )
                  *p_set_special_temporary = true;
            }
            else if( val.substr( 0, pos ) == "attempt" )
            {
               p_command_handler->output_progress( "attempting..." );

               ostringstream osstr;
               tmp_cube.attempt( osstr, val.substr( pos + 1 ) );

               val = osstr.str( );

               if( p_set_special_temporary )
                  *p_set_special_temporary = true;
            }
            else if( val == "scramble" )
            {
               ostringstream osstr;
               tmp_cube.scramble( &osstr );

               old_val = tmp_cube.get_state( );

               val = osstr.str( );

               if( p_set_special_temporary )
                  *p_set_special_temporary = true;
            }
            else if( val.substr( 0, pos ) == "suggest" )
            {
               ostringstream osstr;
               tmp_cube.suggest( osstr, val.substr( pos + 1 ) );

               old_val = tmp_cube.get_state( );

               val = osstr.str( );

               if( p_set_special_temporary )
                  *p_set_special_temporary = true;
            }
            else if( !val.empty( ) )
            {
               tmp_cube.perform_moves( val );
               val = tmp_cube.get_state( );
            }

            if( p_set_special_temporary && *p_set_special_temporary )
               gtp_session->variables[ name + c_temporary_special_variable_suffix ] = old_val;
         }
      }
      else if( name == get_special_var_name( e_special_var_algos ) )
      {
         temporary_algo_prefix tmp_algo_prefix( gtp_session->p_storage_handler->get_name( ) );

         ostringstream osstr;

         string::size_type pos = val.find( ' ' );

         if( pos == string::npos )
         {
            output_algos( osstr, val );

            val = osstr.str( );

            if( p_set_special_temporary )
            {
               *p_set_special_temporary = true;

               gtp_session->variables[ name + c_temporary_special_variable_suffix ] = old_val;
            }
         }
         else
         {
            vector< string > args;

            split( val, args, ' ' );

            val.erase( );

            if( args.size( ) < 3 )
               exec_algos_action( args[ 0 ], args[ 1 ], "" );
            else
               exec_algos_action( args[ 0 ], args[ 1 ], args[ 2 ] );
         }
      }
      else if( name == get_special_var_name( e_special_var_array ) )
      {
         bool new_array = false;

         string::size_type pos = val.find( 'x' );

         if( pos != string::npos )
         {
            new_array = true;

            char_array tmp_array( val );
            val = tmp_array.get_state( );
         }

         if( !new_array && !old_val.empty( ) )
         {
            string::size_type pos = val.find( ' ' );

            char_array tmp_array( old_val );

            if( val == "show" )
            {
               ostringstream osstr;

               tmp_array.output_data( osstr );

               val = osstr.str( );

               if( p_set_special_temporary )
                  *p_set_special_temporary = true;
            }
            else if( val.substr( 0, pos ) == "place" )
            {
               tmp_array.place_chars( val.substr( pos + 1 ) );

               val = tmp_array.get_state( );
            }
            else if( !val.empty( ) )
            {
               tmp_array.perform_moves( val );

               val = tmp_array.get_state( );
            }

            if( p_set_special_temporary && *p_set_special_temporary )
                gtp_session->variables[ name + c_temporary_special_variable_suffix ] = old_val;
         }
      }
      else if( name == get_special_var_name( e_special_var_deque ) )
      {
         skip_standard_variable = true;

         if( val.empty( ) )
            gtp_session->deque_items.clear( );
         else
         {
            string::size_type pos = val.find( ' ' );

            if( val[ 0 ] == '@' )
            {
               if( val != "@set" )
                  buffer_file_lines( val.substr( 1 ), gtp_session->deque_items );
               else
                  copy( gtp_session->set_items.begin( ),
                   gtp_session->set_items.end( ), inserter( gtp_session->deque_items, gtp_session->deque_items.end( ) ) );
            }
            else if( val[ 0 ] == '~' )
               split( val.substr( 1 ), gtp_session->deque_items, '\n' );
            else if( val == "back" )
            {
               if( gtp_session->deque_items.empty( ) )
                  val.erase( );
               else
                  val = gtp_session->deque_items.back( );

               gtp_session->last_deque_item = val;

               if( p_set_special_temporary )
                  *p_set_special_temporary = true;
            }
            else if( val == "size" )
            {
               val = to_string( gtp_session->deque_items.size( ) );

               gtp_session->last_deque_item = val;

               if( p_set_special_temporary )
                  *p_set_special_temporary = true;
            }
            else if( val == "front" )
            {
               if( gtp_session->deque_items.empty( ) )
                  val.erase( );
               else
                  val = gtp_session->deque_items.front( );

               gtp_session->last_deque_item = val;

               if( p_set_special_temporary )
                  *p_set_special_temporary = true;
            }
            else if( ( pos != string::npos ) && ( val.substr( 0, pos ) == "take" ) )
            {
               deque< string > dest;

               take_front_from_deque( gtp_session->deque_items, dest, from_string< int >( val.substr( pos + 1 ) ) );

               if( dest.empty( ) )
                  val.erase( );
               else
                  val = join( dest, '\n' );

               gtp_session->last_deque_item = val;

               if( !val.empty( ) && p_set_special_temporary )
                  *p_set_special_temporary = true;
            }
            else if( pos != string::npos && val.substr( 0, pos ) == "remove" )
            {
               string search( val.substr( pos + 1 ) );

               if( !gtp_session->deque_items.empty( ) )
               {
                  deque< string > new_deque_items;

                  size_t num_items = gtp_session->deque_items.size( );

                  for( size_t i = 0; i < num_items; i++ )
                  {
                     string next_item ( gtp_session->deque_items[ i ] );
                     string::size_type pos = next_item.find( search );

                     if( pos == string::npos )
                        new_deque_items.push_back( next_item );
                  }

                  gtp_session->deque_items = new_deque_items;
               }
            }
            else if( pos != string::npos && val.substr( 0, pos ) == "retain" )
            {
               string search( val.substr( pos + 1 ) );

               if( !gtp_session->deque_items.empty( ) )
               {
                  deque< string > new_deque_items;

                  size_t num_items = gtp_session->deque_items.size( );

                  for( size_t i = 0; i < num_items; i++ )
                  {
                     string next_item ( gtp_session->deque_items[ i ] );

                     string::size_type pos = next_item.find( search );

                     if( pos != string::npos )
                        new_deque_items.push_back( next_item );
                  }

                  gtp_session->deque_items = new_deque_items;
               }
            }
            else if( val == "reverse" )
            {
               if( !gtp_session->deque_items.empty( ) )
                  reverse( gtp_session->deque_items.begin( ), gtp_session->deque_items.end( ) );
            }
            else if( val == "commence" )
            {
               gtp_session->kept_deque_items.push_back( gtp_session->deque_items );

               gtp_session->deque_items.clear( );
            }
            else if( val == "complete" )
            {
               if( gtp_session->kept_deque_items.empty( ) )
                  gtp_session->deque_items.clear( );
               else
               {
                  gtp_session->deque_items = gtp_session->kept_deque_items.back( );
                  gtp_session->kept_deque_items.pop_back( );
               }
            }
            else if( val.substr( 0, pos ) == "pop_back" )
            {
               if( !gtp_session->deque_items.empty( ) )
                  gtp_session->deque_items.pop_back( );
            }
            else if( val.substr( 0, pos ) == "pop_front" )
            {
               if( !gtp_session->deque_items.empty( ) )
                  gtp_session->deque_items.pop_front( );
            }
            else if( pos != string::npos && val.substr( 0, pos ) == "push_back" )
               gtp_session->deque_items.push_back( val.substr( pos + 1 ) );
            else if( pos != string::npos && val.substr( 0, pos ) == "push_front" )
               gtp_session->deque_items.push_front( val.substr( pos + 1 ) );
         }
      }
      else if( name.find( c_special_variable_queue_prefix ) == 0 )
      {
         skip_standard_variable = true;

         if( val.empty( ) )
            gtp_session->deque_variables.erase( name );
         else
            gtp_session->deque_variables[ name ].push_back( val );
      }
      else if( name.find( c_special_variable_mapped_prefix ) == 0 )
      {
         skip_standard_variable = true;

         string::size_type pos = name.find( ':' );

         if( pos == string::npos )
         {
            if( !val.empty( ) )
               throw runtime_error( "invalid mapped variable missing key separator ':'" );

            gtp_session->mapped_variables.erase( name );
         }
         else
         {
            if( val.empty( ) )
               gtp_session->mapped_variables[ name.substr( 0, pos ) ].erase( name.substr( pos + 1 ) );
            else
               gtp_session->mapped_variables[ name.substr( 0, pos ) ][ name.substr( pos + 1 ) ] = val;
         }
      }
      else if( name == get_special_var_name( e_special_var_progress_clear ) )
      {
         skip_standard_variable = true;

         get_raw_session_variable( get_special_var_name( e_special_var_progress_clear ) );
      }
      else if( g_read_only_var_names.count( name ) )
         skip_standard_variable = true;

      if( !skip_standard_variable )
      {
         if( val.empty( ) )
         {
            if( gtp_session->variables.count( name ) )
               gtp_session->variables.erase( name );
         }
         else
         {
            if( gtp_session->variables.count( name ) )
               gtp_session->variables[ name ] = val;
            else
               gtp_session->variables.insert( make_pair( name, val ) );
         }
      }
   }
}

bool set_session_variable( const string& name, const string& value, const string& current )
{
   guard g( g_session_mutex );

   bool retval = false;

   if( gtp_session && ( value != current ) )
   {
      if( !gtp_session->variables.count( name ) )
      {
         if( current.empty( ) )
            retval = true;
      }
      else if( current == gtp_session->variables[ name ] )
      {
         retval = true;

         if( !g_read_only_var_names.count( name ) )
            gtp_session->variables.erase( name );
      }

      if( retval && !value.empty( ) )
      {
         if( gtp_session->variables.count( name ) )
         {
            if( !g_read_only_var_names.count( name ) )
               gtp_session->variables[ name ] = value;
         }
         else
            gtp_session->variables.insert( make_pair( name, value ) );
      }
   }

   return retval;
}

void set_session_variable_for_matching_blockchains( const string& name,
 const string& value, const string& check_name, const string& check_value, bool matching_own_ip_address )
{
   guard g( g_session_mutex );

   if( !gtp_session )
      throw runtime_error( "unexpected call to 'set_session_variable_for_matching_blockchains' from non-session" );

   string own_ip_addr( gtp_session->ip_addr );
   string own_blockchain( gtp_session->blockchain );

   if( g_read_only_var_names.count( name ) )
      throw runtime_error( "invalid 'set_session_variable_for_matching_blockchains' with read-only variable '" + name + "'" );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && ( g_sessions[ i ] != gtp_session ) )
      {
         if( g_sessions[ i ]->blockchain != own_blockchain )
            continue;

         if( matching_own_ip_address && ( g_sessions[ i ]->ip_addr != own_ip_addr ) )
            continue;

         if( !check_name.empty( ) )
         {
            if( check_value.empty( ) )
            {
               if( g_sessions[ i ]->variables.count( check_name ) )
                  continue;
            }
            else if( !g_sessions[ i ]->variables.count( check_name )
             || ( g_sessions[ i ]->variables[ check_name ] != check_value ) )
               continue;
         }

         g_sessions[ i ]->variables[ name ] = value;
      }
   }
}

bool has_any_session_variable( const string& name )
{
   guard g( g_session_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ]
       && ( ( g_sessions[ i ]->variables.count( name ) )
       || ( g_sessions[ i ]->deque_variables.count( name ) ) ) )
         return true;
   }

   return false;
}

bool has_any_session_variable( const string& name, const string& value )
{
   guard g( g_session_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ]
       && g_sessions[ i ]->variables.count( name )
       && ( g_sessions[ i ]->variables[ name ] == value ) )
         return true;
   }

   return false;
}

size_t num_have_session_variable(
 const string& name, bool matching_own_ip_address,
 bool include_condemned, size_t check_session_id_less_than )
{
   guard g( g_session_mutex );

   string own_ip_addr;

   if( gtp_session && matching_own_ip_address )
      own_ip_addr = gtp_session->ip_addr;

   size_t total = 0;

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ]
       && ( !matching_own_ip_address
       || ( g_sessions[ i ]->ip_addr == own_ip_addr ) )
       && ( ( g_sessions[ i ]->variables.count( name ) )
       || ( g_sessions[ i ]->deque_variables.count( name ) ) )
       && ( include_condemned || !g_condemned_sessions.count( g_sessions[ i ]->id ) ) )
      {
         if( !check_session_id_less_than
          || ( g_sessions[ i ]->id < check_session_id_less_than ) )
            ++total;
      }
   }

   return total;
}

size_t num_have_session_variable( const string& name, const string& value,
 vector< string >* p_identities, bool matching_own_ip_address, bool include_condemned )
{
   guard g( g_session_mutex );

   string own_ip_addr;

   if( gtp_session && matching_own_ip_address )
      own_ip_addr = gtp_session->ip_addr;

   size_t total = 0;

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ]
       && ( !matching_own_ip_address
       || ( g_sessions[ i ]->ip_addr == own_ip_addr ) )
       && g_sessions[ i ]->variables.count( name )
       && ( g_sessions[ i ]->variables[ name ] == value )
       && ( include_condemned || !g_condemned_sessions.count( g_sessions[ i ]->id ) ) )
      {
         ++total;

         if( p_identities )
            p_identities->push_back( to_string( g_sessions[ i ]->id ) );
      }
   }

   return total;
}

bool is_first_using_session_variable( const string& name )
{
   guard g( g_session_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ]
       && g_sessions[ i ]->variables.count( name ) )
         return ( g_sessions[ i ] == gtp_session );
   }

   return false;
}

bool is_first_using_session_variable( const string& name, const string& value )
{
   guard g( g_session_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ]
       && g_sessions[ i ]->variables.count( name )
       && g_sessions[ i ]->variables[ name ] == value )
         return ( g_sessions[ i ] == gtp_session );
   }

   return false;
}

void copy_session_variables( map< string, string >& variables )
{
   guard g( g_session_mutex );

   if( gtp_session )
      variables = gtp_session->variables;
}

void restore_session_variables( const map< string, string >& variables )
{
   guard g( g_session_mutex );

   if( gtp_session )
   {
      map< string, string > read_only_pairs;

      for( set< string >::const_iterator ci = g_read_only_var_names.begin( ); ci != g_read_only_var_names.end( ); ++ci )
         read_only_pairs[ *ci ] = gtp_session->variables[ *ci ];

      gtp_session->variables = variables;

      // NOTE: Enusres that special "read only" variables are not able to be changed through using this function.
      for( map< string, string >::const_iterator ci = read_only_pairs.begin( ); ci != read_only_pairs.end( ); ++ci )
         gtp_session->variables[ ci->first ] = ci->second;
   }
}

temporary_identity_suffix::temporary_identity_suffix( const string& temporary_suffix )
{
   if( !gtp_session )
      throw runtime_error( "unexpected 'temporary_identity_suffix' object in non-session" );

   current_suffix = original_suffix = gtp_session->identity_suffix;

   if( !temporary_suffix.empty( ) )
      current_suffix = gtp_session->identity_suffix = temporary_suffix;
}

temporary_identity_suffix::~temporary_identity_suffix( )
{
   gtp_session->identity_suffix = original_suffix;
}

void add_udp_recv_file_chunk_info( size_t slot, size_t chunk, const string& info_and_data )
{
   guard g( g_session_mutex );

   if( slot < g_max_sessions )
   {
      if( g_sessions[ slot ] )
      {
         g_sessions[ slot ]->dtm_last_recv = date_time::local( );
         g_sessions[ slot ]->udp_recv_file_chunks.insert( make_pair( chunk, info_and_data ) );
      }
   }
}

void add_udp_send_file_chunk_info( size_t slot, size_t chunk, const string& info_and_data )
{
   guard g( g_session_mutex );

   if( slot < g_max_sessions )
   {
      if( g_sessions[ slot ] )
         g_sessions[ slot ]->udp_send_file_chunks[ chunk ] = info_and_data;
   }
}

void clear_udp_recv_file_chunks( )
{
   guard g( g_session_mutex );

   if( gtp_session )
      gtp_session->udp_recv_file_chunks.clear( );
}

size_t elapsed_since_last_recv( const date_time& dtm, const date_time* p_dtm )
{
   guard g( g_session_mutex );

   size_t retval = 0;

   if( gtp_session )
   {
      int64_t secs_1 = unix_time( dtm );
      int64_t secs_2 = unix_time( p_dtm ? *p_dtm : gtp_session->dtm_last_recv );

      millisecond ms1 = dtm.get_millisecond( );
      millisecond ms2 = ( p_dtm ? p_dtm->get_millisecond( ) : gtp_session->dtm_last_recv.get_millisecond( ) );

      int64_t total_ms1 = ( secs_1 * 1000 ) + ms1;
      int64_t total_ms2 = ( secs_2 * 1000 ) + ms2;

      if( total_ms1 > total_ms2 )
         retval = ( total_ms1 - total_ms2 );
      else
         retval = ( total_ms2 - total_ms1 );
   }

   return retval;
}

bool has_udp_recv_file_chunk_info( size_t* p_num_chunks )
{
   guard g( g_session_mutex );

   bool retval = false;

   if( gtp_session && !gtp_session->udp_recv_file_chunks.empty( ) )
   {
      retval = true;

      if( p_num_chunks )
         *p_num_chunks = gtp_session->udp_recv_file_chunks.size( );
   }

   return retval;
}

string get_udp_recv_file_chunk_info( size_t& chunk, bool chunk_specified, size_t* p_first_chunk, size_t* p_num_chunks )
{
   guard g( g_session_mutex );

   string retval;

   if( gtp_session && !gtp_session->udp_recv_file_chunks.empty( ) )
   {
      size_t first_chunk = gtp_session->udp_recv_file_chunks.begin( )->first;

      if( p_first_chunk )
         *p_first_chunk = first_chunk;

      if( !chunk_specified || ( chunk == first_chunk ) )
      {
         chunk = first_chunk;
         retval = gtp_session->udp_recv_file_chunks.begin( )->second;

         gtp_session->udp_recv_file_chunks.erase( gtp_session->udp_recv_file_chunks.begin( ) );
      }

      if( p_num_chunks )
         *p_num_chunks = gtp_session->udp_recv_file_chunks.size( );
   }

   return retval;
}

void init_storage( const string& name,
 const string& directory, command_handler& cmd_handler, bool lock_for_admin )
{
   perform_storage_op( e_storage_op_init, name, directory, cmd_handler, lock_for_admin );
}

void create_storage( const string& name,
 const string& directory, command_handler& cmd_handler, bool lock_for_admin )
{
   perform_storage_op( e_storage_op_create, name, directory, cmd_handler, lock_for_admin );
}

void attach_storage( const string& name, command_handler& cmd_handler, bool lock_for_admin )
{
   perform_storage_op( e_storage_op_attach, name, "", cmd_handler, lock_for_admin );
}

void backup_storage( command_handler& cmd_handler, int* p_truncation_count, string* p_sav_db_file_names )
{
   if( ods::instance( ) && gtp_session->p_storage_handler->get_ods( ) )
   {
      if( ods::instance( )->get_transaction_level( ) )
         throw runtime_error( "cannot perform a backup whilst a transaction is active" );

      date_time dtm( date_time::local( ) );

      ods* p_ods( ods::instance( ) );

      storage_handler& handler( *gtp_session->p_storage_handler );

      if( !handler.get_is_locked_for_admin( ) )
         throw runtime_error( "cannot backup a storage unless it has been locked for admin" );

      if( p_sav_db_file_names )
         *p_sav_db_file_names = p_ods->backup_database( ".sav", ' ' );

      // NOTE: Create a SQL file (which is the storage name with a ".backup.sql" extension).
      string sql_file_name( handler.get_name( ) );

      sql_file_name += ".backup.sql";

      ofstream outf( sql_file_name.c_str( ) );

      if( !outf )
         throw runtime_error( "unable to open file '" + sql_file_name + "' for output in backup_storage" );

      vector< string >::const_iterator mci;

      for( mci = handler.get_root( ).module_list.begin( ); mci != handler.get_root( ).module_list.end( ); ++mci )
      {
         vector< string > class_list;

         list_module_classes( *mci, class_list );

         map< string, string > class_ids_and_names;

         list_module_classes( *mci, class_ids_and_names, true );

         outf << "BEGIN;\n";

         for( size_t i = 0; i < class_list.size( ); i++ )
         {
            vector< string > columns;
            string sql_columns( get_sql_columns_for_module_class( *mci, class_list[ i ] ) );

            if( !sql_columns.empty( ) )
            {
               split( sql_columns, columns );

               string table_name( "T_" + *mci + "_" + class_ids_and_names[ class_list[ i ] ] );

               date_time now( date_time::local( ) );

               uint64_t elapsed = seconds_between( dtm, now );

               if( elapsed >= 1 )
               {
                  dtm = now;
                  cmd_handler.output_progress( "." );
               }

               outf << "\n#Creating table " << table_name << "\n";

               outf << "\nDROP TABLE IF EXISTS " << table_name << ";\n";

               outf << "\nCREATE TABLE " << table_name << '\n';
               outf << "(\n";

               for( size_t j = 0; j < columns.size( ); j++ )
               {
                  if( j > 0 )
                     outf << ",\n";
                  outf << " " << columns[ j ];
               }

               outf << "\n);\n\n";

               size_t num_rows = 0;

               if( gtp_session->up_db.get( ) )
               {
                  string select_sql( "SELECT * FROM " + table_name );

                  size_t handle = create_object_instance( *mci, class_list[ i ], 0, false );

                  class_base& instance( get_class_base_from_handle( handle, "" ) );

                  vector< string > sql_column_names;

                  sql_column_names.push_back( "C_Key_" );
                  sql_column_names.push_back( "C_Ver_" );
                  sql_column_names.push_back( "C_Rev_" );
                  sql_column_names.push_back( "C_Sec_" );
                  sql_column_names.push_back( "C_Typ_" );

                  instance.get_sql_column_names( sql_column_names );

                  sql_dataset ds( *gtp_session->up_db.get( ), select_sql );

                  while( ds.next( ) )
                  {
                     if( ds.get_fieldcount( ) != sql_column_names.size( ) )
                        throw runtime_error( "unexpected SQL columns mismatch" );

                     string insert_sql( "INSERT INTO " + table_name );

                     insert_sql += " VALUES (";

                     for( int col = 0; col < ds.get_fieldcount( ); col++ )
                     {
                        bool is_sql_numeric;

                        string field_name( sql_column_names[ col ].substr( 2 ) );

                        get_field_name( instance, field_name, &is_sql_numeric );

                        if( col > 0 )
                           insert_sql += ",";

                        if( is_sql_numeric )
                           insert_sql += ds.as_string( col );
                        else
                        {
                           string data( ds.as_string( col ) );
                           insert_sql += sql_quote( escaped( data, 0, '\\', "rn\r\n" ) );
                        }
                     }

                     outf << insert_sql << ");\n";

                     if( ( ++num_rows % 1000 ) == 0 )
                     {
                        // FUTURE: These messages should be handled as a server string messages.
                        if( ( num_rows % 10000 ) == 0 )
                           cmd_handler.output_progress( "Processed "
                            + to_string( num_rows ) + " rows for " + table_name + "..." );

                        outf << "\n#Inserted " << num_rows
                         << " rows into table " << table_name << "\n\n";

                        outf << "COMMIT;\n";
                        outf << "BEGIN;\n";
                     }
                  }

                  // FUTURE: This message should be handled as a server string message.
                  if( ( num_rows % 1000 ) != 0 )
                     outf << "\n#Inserted " << num_rows
                      << " rows into table " << table_name << "\n";

                  destroy_object_instance( handle );
               }

               vector< string > sql_indexes;

               get_sql_indexes_for_module_class( *mci, class_list[ i ], sql_indexes );

               // FUTURE: This message should be handled as a server string message.
               if( num_rows < 1000 )
                  outf << "\n#Creating indexes for table " << table_name << "\n";

               string index_prefix( "I_" + *mci + "_" + class_ids_and_names[ class_list[ i ] ] );

               for( size_t j = 0; j < sql_indexes.size( ); j++ )
               {
                  vector< string > index_columns;

                  split( sql_indexes[ j ], index_columns );

                  if( num_rows >= 1000 )
                  {
                     // FUTURE: This message should be handled as a server string message.
                     outf << "\n#Creating index #";
                     if( j < 10 )
                        outf << '0';
                     outf << j << " for table " << table_name << "\n";
                  }

                  outf << "\nCREATE UNIQUE INDEX " << index_prefix << "_";
                  if( j < 10 )
                     outf << '0';
                  outf << j << " ON " << table_name << '\n';
                  outf << "(\n";
                  outf << " C_Sec_";
                  for( size_t k = 0; k < index_columns.size( ); k++ )
                  {
                     outf << ",\n";
                     outf << " " << index_columns[ k ];
                  }
                  outf << "\n);\n";
               }
            }
         }
      }

      outf << "\nCOMMIT;\n";

      outf.flush( );

      if( !outf.good( ) )
         throw runtime_error( "unexpected bad output stream" );

      if( p_truncation_count )
      {
         *p_truncation_count = ++gtp_session->p_storage_handler->get_root( ).truncation_count;

         ostringstream osstr;
         osstr << "." << setw( 3 ) << setfill( '0' ) << *p_truncation_count;

         ods::instance( )->truncate_log( osstr.str( ).c_str( ) );

         ods_file_system ofs( *ods::instance( ) );

         ofs.store_as_text_file( c_storable_file_name_trunc_n,
          gtp_session->p_storage_handler->get_root( ).truncation_count );

         string truncated_log_name( handler.get_name( ) + c_log_file_ext + osstr.str( ) );

         file_remove( truncated_log_name );
         file_rename( handler.get_name( ) + c_log_file_ext, truncated_log_name );

         transaction_log_command( ";truncated at "
          + date_time::local( ).as_string( e_time_format_hhmmss, true ) );

         append_transaction_log_command( handler, true );
      }
   }
}

bool export_storage( command_handler& cmd_handler )
{
   bool has_exported_any = false;

   if( ods::instance( ) && gtp_session->p_storage_handler->get_ods( ) )
   {
      if( ods::instance( )->get_transaction_level( ) )
         throw runtime_error( "cannot perform storage export whilst a transaction is active" );

      storage_handler& handler( *gtp_session->p_storage_handler );

      if( !handler.get_is_locked_for_admin( ) )
         throw runtime_error( "cannot export from storage unless it has been locked for admin" );

      ods_file_system ofs( *ods::instance( ) );

      string name( handler.get_name( ) );

      if( ofs.has_folder( c_storage_folder_name_channels ) )
      {
         ofs.set_folder( c_storage_folder_name_channels );

         create_dir( name );

         string child_folder_name( name + '/' );
         child_folder_name += c_storage_folder_name_channels;

         create_dir( child_folder_name );

         export_objects( ofs, child_folder_name, 0, 0, &cmd_handler );

         ofs.set_folder( ".." );
         has_exported_any = true;
      }

      if( ofs.has_folder( c_storage_folder_name_datachains ) )
      {
         if( !has_exported_any )
            create_dir( name );

         ofs.set_folder( c_storage_folder_name_datachains );

         string child_folder_name( name + '/' );
         child_folder_name += c_storage_folder_name_datachains;

         create_dir( child_folder_name );

         export_objects( ofs, child_folder_name, 0, 0, &cmd_handler );

         ofs.set_folder( ".." );
         has_exported_any = true;
      }
   }

   return has_exported_any;
}

void restore_storage( command_handler& cmd_handler )
{
   if( ods::instance( ) && gtp_session->p_storage_handler->get_ods( ) )
   {
      if( ods::instance( )->get_transaction_level( ) )
         throw runtime_error( "cannot perform a restore whilst a transaction is active" );

      storage_handler& handler( *gtp_session->p_storage_handler );

      if( !handler.get_is_locked_for_admin( ) )
         throw runtime_error( "cannot restore a storage unless it has been locked for admin" );

      string sql_file_name( handler.get_name( ) );

      sql_file_name += ".backup.sql";

      if( !file_exists( sql_file_name ) )
         throw runtime_error( "did not find backup file '" + sql_file_name + "'" );

      if( gtp_session->up_db.get( ) )
         exec_sql_from_file( *gtp_session->up_db, sql_file_name, &cmd_handler, true );
   }
}

void upgrade_storage( command_handler& cmd_handler )
{
   if( ods::instance( ) && gtp_session->p_storage_handler->get_ods( ) )
   {
      if( ods::instance( )->get_transaction_level( ) )
         throw runtime_error( "cannot perform an upgrade whilst a transaction is active" );

      storage_handler& handler( *gtp_session->p_storage_handler );

      if( !handler.get_is_locked_for_admin( ) )
         throw runtime_error( "cannot upgrade a storage unless it has been locked for admin" );

      string sql_file_name( handler.get_name( ) );

      sql_file_name += ".upgrade.sql";

      if( file_exists( sql_file_name ) && gtp_session->up_db.get( ) )
         exec_sql_from_file( *gtp_session->up_db, sql_file_name, &cmd_handler );
   }
}

void term_storage( command_handler& cmd_handler )
{
   guard g( g_mutex, "term_storage" );

   if( ods::instance( ) )
   {
      gtp_session->up_bulk_read.reset( );

      while( !gtp_session->transactions.empty( ) )
         transaction_rollback( );

      gtp_session->up_bulk_write.reset( );

      if( gtp_session->p_storage_handler->get_ref_count( ) == 1 )
      {
         // NOTE: Now store the "next_id" (rather than the "ceiling" value).
         ods_file_system ofs( *ods::instance( ) );

         ofs.store_as_text_file( c_storable_file_name_log_id,
          gtp_session->p_storage_handler->get_root( ).log_id.next_id );
      }

      gtp_session->p_storage_handler->release_all_locks_for_session( gtp_session );

      delete ods::instance( );
      ods::instance( 0, true );

      destroy_all_object_instances( );

      while( !gtp_session->storage_controlled_modules.empty( ) )
      {
         module_unload( gtp_session->storage_controlled_modules.back( ), cmd_handler );
         gtp_session->storage_controlled_modules.pop_back( );
      }

      module_directory( &g_empty_string );

      if( gtp_session->p_storage_handler->dec_ref_count( ) == 0 )
      {
         g_storage_handler_index.erase( gtp_session->p_storage_handler->get_name( ) );
         g_storage_handlers[ gtp_session->p_storage_handler->get_slot( ) ] = 0;

         gtp_session->p_storage_handler->release_ods( );

         if( gtp_session->p_storage_handler->get_log_file( ).is_open( ) )
            gtp_session->p_storage_handler->get_log_file( ).close( );

         temporary_algo_prefix tmp_algo_prefix( gtp_session->p_storage_handler->get_name( ) );
         exec_algos_action( "kill", "*", "" );

         delete gtp_session->p_storage_handler;
      }

      set_session_variable( get_special_var_name( e_special_var_storage ),
       get_raw_session_variable( get_special_var_name( e_special_var_prior_storage ) ) );

      gtp_session->p_storage_handler = g_storage_handlers[ 0 ];
   }
}

struct storage_scoped_lock_holder::impl
{
   impl( storage_handler& handler, size_t lock_handle )
    :
    lock_holder( handler, lock_handle )
   {
   }

   void release( )
   {
      lock_holder.release( );
   }

   scoped_lock_holder lock_holder;
};

storage_scoped_lock_holder::storage_scoped_lock_holder( size_t lock_handle )
{
   storage_handler& handler( *gtp_session->p_storage_handler );

   p_impl = new storage_scoped_lock_holder::impl( handler, lock_handle );
}

storage_scoped_lock_holder::~storage_scoped_lock_holder( )
{
   delete p_impl;
}

void storage_scoped_lock_holder::release( )
{
   return p_impl->release( );
}

void storage_admin_name_lock( const string& name )
{
   guard g( g_mutex );

   if( gtp_session )
   {
      if( g_storage_handler_index.find( name ) != g_storage_handler_index.end( ) )
         throw runtime_error( "storage '" + name + "' cannot be administered as it is currently in use" );

      // NOTE: If this session has an existing lock name then that will need to be removed first.
      if( gtp_session->up_storage_name_lock.get( ) )
         gtp_session->up_storage_name_lock.reset( );

      gtp_session->up_storage_name_lock.reset( new global_storage_name_lock( name ) );
   }
}

void storage_comment( const string& comment )
{
   guard g( g_mutex );

   ods* p_ods( ods::instance( ) );

   if( p_ods && gtp_session )
   {
      gtp_session->transaction_log_command = ";" + comment;

      storage_handler& handler( *gtp_session->p_storage_handler );

      log_identity& identity( handler.get_root( ).log_id );

      bool use_init_tx_id = false;

      string init_log_id( get_raw_session_variable( get_special_var_name( e_special_var_init_log_id ) ) );

      if( ( init_log_id == c_true ) || ( init_log_id == c_true_value ) )
         use_init_tx_id = true;

      // NOTE: During a "restore" the comment does not need to be logged unless it follows or is a part of
      // the initial data records (such comments prior are appended by the "storage restore" code itself).
      if( use_init_tx_id || !storage_locked_for_admin( ) || ( identity.next_id >= c_tx_id_initial ) )
      {
         if( use_init_tx_id || ( identity.next_id == c_tx_id_initial ) )
            append_transaction_log_command( *gtp_session->p_storage_handler, false, 0, c_tx_id_initial );
         else
            append_transaction_log_command( *gtp_session->p_storage_handler, false, 0, identity.next_id );
      }

      if( handler.supports_sql_undo( ) )
      {
         string label_comment( string( c_label_prefix ) + ' ' );

         string new_comment( comment );

         if( comment.find( label_comment ) == 0 )
         {
            if( storage_locked_for_admin( ) && ( ( identity.next_id + 1 ) >= c_tx_id_standard ) )
               gtp_session->sql_undo_statements.push_back( "#" + comment );
            else
            {
               string undo_sql_filename( handler.get_name( ) );

               undo_sql_filename += ".undo.sql";

               ofstream outf( undo_sql_filename.c_str( ), ios::out | ios::app );

               if( !outf )
                  throw runtime_error( "unable to open '" + undo_sql_filename + "' for output" );

               outf << "#" << comment << '\n';

               outf.flush( );

               if( !outf.good( ) )
                  throw runtime_error( "*** unexpected error occurred writing to undo sql ***" );
            }
         }
      }
   }
}

void storage_process_rewind( const string& label, map< string, string >& file_info )
{
   guard g( g_mutex );

   storage_handler& handler( *gtp_session->p_storage_handler );

   string undo_sql( gtp_session->p_storage_handler->get_name( ) + ".undo.sql" );

   string new_undo_sql( undo_sql + ".new" );

   bool okay = true;

   if( okay && gtp_session->up_db.get( ) )
   {
      ifstream inpf( undo_sql.c_str( ) );
      if( !inpf )
         throw runtime_error( "unable to open file '" + undo_sql + "' for input in storage_process_rewind" );

      ofstream outf( new_undo_sql.c_str( ) );
      if( !outf )
         throw runtime_error( "unable to open file '" + new_undo_sql + "' for output in storage_process_rewind" );

      deque< string > undo_statements;

      string block_marker( "#" + string( c_label_prefix ) + " " );

      string next;
      bool found_rewind_point = false;

      while( getline( inpf, next ) )
      {
         string::size_type pos = next.find( block_marker );
         if( pos == 0 )
         {
            if( !found_rewind_point )
            {
               if( label == next.substr( pos + block_marker.size( ) ) )
                  found_rewind_point = true;
            }
         }
         else if( found_rewind_point )
            undo_statements.push_front( next );

         if( !found_rewind_point )
            outf << next << '\n';
      }

      if( !found_rewind_point )
         throw runtime_error( "unexpected rewind point " + label + " not found" );

      outf.flush( );

      if( !outf.good( ) )
         throw runtime_error( "*** unexpected error occurred writing to new undo sql ***" );

      inpf.close( );
      outf.close( );

      string storage_files_dir( get_web_root( ) );

      storage_files_dir += '/' + lower( gtp_session->p_storage_handler->get_name( ) ) + '/' + string( c_files_directory );

      for( size_t i = 0; i < undo_statements.size( ); i++ )
      {
         string next_statement( unescaped( undo_statements[ i ], "rn\r\n" ) );

         if( !next_statement.empty( ) )
         {
            TRACE_LOG( TRACE_DETAILS | TRACE_QUERIES, next_statement );

            exec_sql( *gtp_session->up_db, next_statement );
         }
      }
   }

   string log_name( gtp_session->p_storage_handler->get_name( ) + c_log_file_ext );

   string new_log_name( log_name + ".new" );

   if( file_exists( new_undo_sql ) )
   {
      remove_file( undo_sql );
      rename_file( new_undo_sql, undo_sql );

      if( handler.get_log_file( ).is_open( ) )
         handler.get_log_file( ).close( );

      ifstream inpf( log_name.c_str( ) );

      if( !inpf )
         throw runtime_error( "unable to open '" + log_name + "' for input" );

      ofstream outf( new_log_name.c_str( ) );

      if( !outf )
         throw runtime_error( "unable to open '" + new_log_name + "' for output" );

      string block_marker( ";" + string( c_label_prefix ) + " " );

      string next;

      bool finished = false;

      while( getline( inpf, next ) )
      {
         string::size_type pos = next.find( ']' );

         if( pos != string::npos && next.find( block_marker ) == pos + 1 )
         {
            if( label == next.substr( pos + block_marker.length( ) ) )
               finished = true;
         }

         if( finished )
         {
            if( pos != string::npos )
               outf << next.substr( 0, pos + 1 ) << ";rewind " << label << "\n";
            break;
         }

         outf << next << '\n';
      }

      outf.flush( );

      if( !outf.good( ) )
         throw runtime_error( "*** unexpected error occurred writing to new transaction log ***" );

      inpf.close( );
      outf.close( );
   }

   if( file_exists( new_log_name ) )
   {
      remove_file( log_name );
      rename_file( new_log_name, log_name );
   }
}

void storage_cache_clear( )
{
   guard g( g_mutex );

   storage_handler& handler( *gtp_session->p_storage_handler );

   handler.clear_cache( );
}

size_t storage_cache_limit( )
{
   guard g( g_mutex );

   storage_handler& handler( *gtp_session->p_storage_handler );

   return handler.get_root( ).cache_limit;
}

size_t storage_cache_limit( size_t new_limit )
{
   guard g( g_mutex );

   storage_handler& handler( *gtp_session->p_storage_handler );

   handler.set_cache_limit( new_limit );

   ods* p_ods( ods::instance( ) );

   if( p_ods )
   {
      ods_file_system ofs( *p_ods );

      ofs.store_as_text_file( c_storable_file_name_limit,
       gtp_session->p_storage_handler->get_root( ).cache_limit );

      log_identity& identity( handler.get_root( ).log_id );

      gtp_session->transaction_log_command = ";cache_limit ==> " + to_string( new_limit );

      append_transaction_log_command( *gtp_session->p_storage_handler, false, 0, identity.next_id );
   }

   return new_limit;
}

void slice_storage_log( command_handler& cmd_handler, const string& name, const vector< string >& module_list )
{
   perform_storage_op( e_storage_op_attach, name, "", cmd_handler, true );

   try
   {
      for( size_t i = 0; i < module_list.size( ); i++ )
      {
         if( !gtp_session->modules_by_name.count( module_list[ i ] ) )
            throw runtime_error( "unknown module '" + module_list[ i ] + "' for storage '" + name + "'" );
      }

      set< string > sorted_module_list( module_list.begin( ), module_list.end( ) );

      vector< pair< string, string > > modules_for_slice;

      size_t num_modules( gtp_session->p_storage_handler->get_root( ).module_list.size( ) );

      for( size_t i = 0; i < num_modules; i++ )
      {
         string next_module( gtp_session->p_storage_handler->get_root( ).module_list[ i ] );

         if( sorted_module_list.empty( ) || sorted_module_list.count( next_module ) )
            modules_for_slice.push_back( make_pair( next_module, gtp_session->modules_by_name.find( next_module )->second ) );
      }

      map< string, string > module_aliases;

      // NOTE: For each module being extracted create an .ltf file from the storage .ltf file (if exists).
      string ltf_file_name( name + ".ltf" );

      if( file_exists( ltf_file_name ) )
      {
         vector< string > ltf_lines;

         buffer_file_lines( ltf_file_name, ltf_lines );

         for( size_t i = 0; i < modules_for_slice.size( ); i++ )
         {
            string module_ltf_file_name( modules_for_slice[ i ].first + ".ltf.new" );

            ofstream outs( module_ltf_file_name.c_str( ) );

            if( !outs )
               throw runtime_error( "unexpected error opening '" + module_ltf_file_name + "' for output" );

            size_t num_lines = 0;

            for( size_t j = 0; j < ltf_lines.size( ); j++ )
            {
               vector< string > ltf_line_info;

               split( ltf_lines[ j ], ltf_line_info, ' ' );

               if( ltf_line_info.size( ) >= 3 )
               {
                  bool is_alias = false;

                  if( ltf_line_info[ 2 ] == "map_module" && ltf_line_info[ 3 ] == modules_for_slice[ i ].second )
                  {
                     is_alias = true;

                     module_aliases.insert( make_pair( ltf_line_info[ 1 ], ltf_line_info[ 3 ] ) );
                  }

                  if( is_alias || ltf_line_info[ 1 ] == modules_for_slice[ i ].second )
                  {
                     ++num_lines;

                     outs << ltf_lines[ j ] << '\n';
                  }
               }
            }

            outs.flush( );

            if( !outs.good( ) )
               throw runtime_error( "unexpected error occurred writing to '" + module_ltf_file_name + "'" );

            outs.close( );

            if( !num_lines )
               remove_file( module_ltf_file_name );
         }
      }

      vector< ofstream* > output_log_files;

      // NOTE: Iterate through the storage log writing matching log lines for each module being extracted
      // (taking into account any module mapping determined from the .ltf processing above).
      try
      {
         for( size_t i = 0; i < modules_for_slice.size( ); i++ )
         {
            string next_log_file_name( modules_for_slice[ i ].first + ".log.new" );

            output_log_files.push_back( new ofstream( next_log_file_name.c_str( ) ) );

            if( !*output_log_files.back( ) )
               throw runtime_error( "unable to open '" + next_log_file_name + "' for output" );
         }

         string log_file_name( name + c_log_file_ext );

         ifstream inpf( log_file_name.c_str( ) );

         if( !inpf )
            throw runtime_error( "unexpected error attempting to open '" + log_file_name + "' for reading" );

         string next;

         size_t line = 0;

         time_t ts = time( 0 );

         while( getline( inpf, next ) )
         {
            ++line;

            if( next.empty( ) )
               continue;

            if( next[ 0 ] != '[' )
               throw runtime_error( "unexpected formatting in log line #" + to_string( line ) + " ==> " + next );

            string::size_type pos = next.find( ']' );

            if( pos == string::npos )
               throw runtime_error( "unexpected formatting in log line #" + to_string( line ) + " ==> " + next );

            size_t tran_id = from_string< size_t >( next.substr( 1, pos - 1 ) );

            if( ( time( 0 ) - ts ) >= 10 )
            {
               ts = time( 0 );

               cmd_handler.output_progress( "Processed " + to_string( line ) + " lines..." );

               if( is_condemned_session( ) )
                  break;
            }

            if( tran_id == 0 )
            {
               for( size_t i = 0; i < output_log_files.size( ); i++ )
               {
                  *output_log_files[ i ] << "[0]" << uuid( ).as_string( ) << '\n';
                  *output_log_files[ i ] << "[1];module ==> " << modules_for_slice[ i ].first << '\n';
               }
            }
            else if( tran_id > 1 )
            {
               vector< string > next_items;

               split( next, next_items, ' ' );

               if( next_items.size( ) < 5 )
                  throw runtime_error( "unexpected formatting in log line #" + to_string( line ) + " ==> " + next );

               string module( next_items[ 3 ] );

               if( module_aliases.count( module ) )
                  module = module_aliases.find( module )->second;

               for( size_t i = 0; i < output_log_files.size( ); i++ )
               {
                  if( module == modules_for_slice[ i ].second )
                     *output_log_files[ i ] << next << '\n';
               }
            }
         }

         for( size_t i = 0; i < output_log_files.size( ); i++ )
         {
            output_log_files[ i ]->close( );

            delete output_log_files[ i ];
         }
      }
      catch( ... )
      {
         for( size_t i = 0; i < output_log_files.size( ); i++ )
            delete output_log_files[ i ];

         throw;
      }
   }
   catch( ... )
   {
      term_storage( cmd_handler );

      throw;
   }

   term_storage( cmd_handler );
}

void splice_storage_log( command_handler& cmd_handler, const string& name, const vector< string >& module_list )
{
   perform_storage_op( e_storage_op_attach, name, "", cmd_handler, true );

   try
   {
      for( size_t i = 0; i < module_list.size( ); i++ )
      {
         if( gtp_session->modules_by_name.count( module_list[ i ] ) )
            throw runtime_error( "module '" + module_list[ i ] + "' already exists in storage '" + name + "'" );
      }

      string ltf_file_name( name + ".ltf" );
      string new_ltf_file_name( name + ".ltf.new" );

      bool has_ltf_entries = false;

      if( file_exists( ltf_file_name ) )
      {
         has_ltf_entries = true;

         file_copy( ltf_file_name, new_ltf_file_name );
      }

      ofstream ltfs( new_ltf_file_name.c_str( ), ios::out | ios::app );

      if( !ltfs )
         throw runtime_error( "unable to open file '" + new_ltf_file_name + "' for output in splice_storage_log" );

      // NOTE: Append any .ltf files for the new modules to the main .ltf file.
      for( size_t i = 0; i < module_list.size( ); i++ )
      {
         string next_ltf_file_name( module_list[ i ] + ".ltf" );

         if( file_exists( next_ltf_file_name ) )
         {
            ifstream inpf( next_ltf_file_name.c_str( ) );
            copy_stream( inpf, ltfs );
            has_ltf_entries = true;
         }
      }

      ltfs.flush( );

      if( !ltfs.good( ) )
         throw runtime_error( "unexpected error occurred writing to '" + new_ltf_file_name + "' in splice_storage_log" );

      ltfs.close( );

      if( !has_ltf_entries )
         remove_file( new_ltf_file_name );

      string log_file_name( name + c_log_file_ext );
      string new_log_file_name( log_file_name + ".new" );

      ifstream logs( log_file_name.c_str( ) );

      if( !logs )
         throw runtime_error( "unable to open file '" + log_file_name + "' for input in splice_storage_log" );

      ofstream new_logs( new_log_file_name.c_str( ) );

      if( !new_logs )
         throw runtime_error( "unable to open file '" + new_log_file_name + "' for output in splice_storage_log" );

      new_logs << "[0]" << uuid( ).as_string( ) << '\n';

      vector< pair< ifstream*, string > > module_log_files;

      // NOTE: Splice together a new .log file using the existing .log file and those of each
      // module being added. The lines are ordered according to the transaction id numbering.
      try
      {
         for( size_t i = 0; i < module_list.size( ); i++ )
         {
            string next_log_file_name( module_list[ i ] + c_log_file_ext );

            module_log_files.push_back( make_pair( new ifstream( next_log_file_name.c_str( ) ), "" ) );
         }

         string next;

         size_t line = 0;

         time_t ts = time( 0 );

         while( getline( logs, next ) )
         {
            ++line;

            if( next.empty( ) )
               continue;

            if( next[ 0 ] != '[' )
               throw runtime_error( "unexpected formatting in log line #" + to_string( line ) + " ==> " + next );

            string::size_type pos = next.find( ']' );

            if( pos == string::npos )
               throw runtime_error( "unexpected formatting in log line #" + to_string( line ) + " ==> " + next );

            size_t tran_id = from_string< size_t >( next.substr( 1, pos - 1 ) );

            if( tran_id == 0 )
               continue;

            if( ( time( 0 ) - ts ) >= 10 )
            {
               ts = time( 0 );

               cmd_handler.output_progress( "Processed " + to_string( line ) + " lines..." );

               if( is_condemned_session( ) )
                  break;
            }

            multimap< size_t, string > new_lines;

            for( size_t i = 0; i < module_list.size( ); i++ )
            {
               while( *module_log_files[ i ].first )
               {
                  string next_module_line( module_log_files[ i ].second );

                  size_t module_tran_id = 0;

                  if( !next_module_line.empty( ) )
                  {
                     if( next_module_line[ 0 ] != '[' )
                        throw runtime_error( "unexpected formatting in ==> " + next_module_line );

                     string::size_type pos = next_module_line.find( ']' );

                     if( pos == string::npos )
                        throw runtime_error( "unexpected formatting in log line ==> " + next_module_line );

                     module_tran_id = from_string< size_t >( next_module_line.substr( 1, pos - 1 ) );

                     if( module_tran_id && module_tran_id < tran_id )
                        new_lines.insert( make_pair( module_tran_id, next_module_line ) );
                  }

                  if( module_tran_id >= tran_id )
                     break;

                  getline( *module_log_files[ i ].first, module_log_files[ i ].second );
               }
            }

            for( multimap< size_t, string >::iterator i = new_lines.begin( ); i!= new_lines.end( ); ++i )
               new_logs << i->second << '\n';

            new_logs << next << '\n';
         }

         for( size_t i = 0; i < module_log_files.size( ); i++ )
            delete module_log_files[ i ].first;

         string modules_file_name( name + ".modules.lst" );
         string new_modules_file_name( name + ".modules.lst.new" );

         if( file_exists( modules_file_name ) )
            file_copy( modules_file_name, new_modules_file_name );

         ofstream new_mods( new_modules_file_name.c_str( ), ios::out | ios::app );

         for( size_t i = 0; i < module_list.size( ); i++ )
            new_mods << module_list[ i ] << '\n';
      }
      catch( ... )
      {
         for( size_t i = 0; i < module_log_files.size( ); i++ )
            delete module_log_files[ i ].first;

         throw;
      }
   }
   catch( ... )
   {
      term_storage( cmd_handler );

      throw;
   }

   term_storage( cmd_handler );
}

bool storage_is_dead_key( const string& cid, const string& key, string* p_key_found )
{
   guard g( g_mutex );

   bool found = false;

   string dead_key( cid + ':' + key );

   const set< string >& dead_keys( gtp_session->p_storage_handler->get_dead_keys( ) );

   if( !dead_keys.empty( ) )
   {
      set< string >::const_iterator ci = dead_keys.lower_bound( dead_key );

      if( ci != dead_keys.end( ) )
         found = wildcard_match( *ci, dead_key );

      if( !found && ci != dead_keys.begin( ) )
      {
         --ci;
         found = wildcard_match( *ci, dead_key );
      }

      if( found && p_key_found )
         *p_key_found = *ci;
   }

   return found;
}

void storage_add_dead_key( const string& cid, const string& key )
{
   guard g( g_mutex );

   string dead_key( cid + ':' + key );

   string dead_keys_file( gtp_session->p_storage_handler->get_name( ) + c_dead_keys_suffix );

   ofstream outf( dead_keys_file.c_str( ), ios::out | ios::app );

   if( !outf )
      throw runtime_error( "unable to open '" + dead_keys_file + "' for output" );

   outf << dead_key << '\n';

   outf.flush( );

   if( !outf.good( ) )
      throw runtime_error( "*** unexpected error occurred writing to '" + dead_keys_file + " ***" );

   gtp_session->p_storage_handler->get_dead_keys( ).insert( dead_key );
}

void check_storage( bool* p_rc )
{
   bool okay = true;

   if( !gtp_session
    || !gtp_session->p_storage_handler
    || !gtp_session->p_storage_handler->get_ods( ) )
      okay = false;

   if( p_rc )
      *p_rc = okay;
   else
   {
      if( !okay )
         throw runtime_error( "no storage is currently linked" );
   }
}

string storage_name( bool* p_is_standard )
{
   if( p_is_standard )
      *p_is_standard = ( gtp_session->p_storage_handler->get_root( ).type == e_storage_type_standard );

   return gtp_session->p_storage_handler->get_name( );
}

string storage_identity( )
{
   return gtp_session->p_storage_handler->get_root( ).identity;
}

void storage_identity( const string& new_identity )
{
   if( !storage_locked_for_admin( ) )
      throw runtime_error( "cannot change identity unless locked for administration" );

   ods_file_system ofs( *ods::instance( ) );

   storage_type type = gtp_session->p_storage_handler->get_root( ).type;

   string prefix( ( type == e_storage_type_peerchain ) ? "@" : "" );

   ofs.store_as_text_file( c_storable_file_name_id, prefix + new_identity );

   gtp_session->p_storage_handler->get_root( ).identity = new_identity;
}

string storage_module_directory( )
{
   return gtp_session->p_storage_handler->get_root( ).module_directory;
}

bool storage_supports_sql_undo( )
{
   return gtp_session->p_storage_handler->supports_sql_undo( );
}

struct raw_storage_variable_getter : variable_getter
{
   string get_value( const string& name ) const { return get_raw_storage_variable( name ); }
};

bool has_storage_variable( const string& name_or_expr )
{
   return !get_storage_variable( name_or_expr ).empty( );
}

string get_storage_variable( const string& name_or_expr )
{
   raw_storage_variable_getter raw_getter;
   variable_expression expr( name_or_expr, raw_getter );

   return expr.get_value( );
}

string get_raw_storage_variable( const string& var_name )
{
   return gtp_session->p_storage_handler->get_variable( var_name );
}

void set_storage_variable( const string& var_name, const string& new_value )
{
   gtp_session->p_storage_handler->set_variable( var_name, new_value );
}

string storage_web_root( bool expand, bool check_is_linked )
{
   guard g( g_mutex );

   if( check_is_linked && !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   string path( gtp_session->p_storage_handler->get_root( ).web_root );

   if( expand )
   {
      path = search_replace( path, c_expand_root, get_web_root( ) );
      path = search_replace( path, c_expand_store, lower( storage_name( ) ) );
   }

   return path;
}

void storage_web_root( const string& new_root )
{
   guard g( g_mutex );

   ods* p_ods( ods::instance( ) );

   gtp_session->p_storage_handler->get_root( ).web_root = new_root;

   if( p_ods )
   {
      ods_file_system ofs( *p_ods );

      ofs.store_as_text_file( c_storable_file_name_web_root,
       gtp_session->p_storage_handler->get_root( ).web_root, c_storable_file_pad_len );

      gtp_session->transaction_log_command = ";web_root ==> " + new_root;

      append_transaction_log_command( *gtp_session->p_storage_handler );
   }
}

void storage_bulk_start( bool is_write )
{
   if( gtp_session )
   {
      if( !gtp_session->p_storage_handler->get_ods( ) )
         throw runtime_error( "no storage is currently linked" );

      if( gtp_session->up_bulk_read.get( ) )
         throw runtime_error( "storage is already bulk locked for read by this session" );

      if( gtp_session->up_bulk_write.get( ) )
         throw runtime_error( "storage is already bulk locked for write by this session" );

      if( !is_write )
         gtp_session->up_bulk_read.reset( new storage_ods_bulk_read( ) );
      else
         gtp_session->up_bulk_write.reset( new storage_ods_bulk_write( ) );
   }
}

void storage_bulk_finish( )
{
   if( gtp_session )
   {
      if( !gtp_session->p_storage_handler->get_ods( ) )
         throw runtime_error( "no storage is currently linked" );

      gtp_session->up_bulk_read.reset( );
      gtp_session->up_bulk_write.reset( );
   }
}

void storage_lock_all_tables( )
{
   if( ods::instance( ) && gtp_session->p_storage_handler->get_ods( ) )
   {
      if( ods::instance( )->get_transaction_level( ) )
         throw runtime_error( "cannot lock tables whilst a transaction is active" );

      ods* p_ods( ods::instance( ) );

      storage_handler& handler( *gtp_session->p_storage_handler );

      try
      {
         string all_table_info;

         vector< string >::const_iterator mci;

         for( mci = handler.get_root( ).module_list.begin( ); mci != handler.get_root( ).module_list.end( ); ++mci )
         {
            vector< string > class_list;

            list_module_classes( *mci, class_list );

            map< string, string > class_ids_and_names;

            list_module_classes( *mci, class_ids_and_names, true );

            for( size_t i = 0; i < class_list.size( ); i++ )
            {
               vector< string > columns;

               string sql_columns( get_sql_columns_for_module_class( *mci, class_list[ i ] ) );

               if( !sql_columns.empty( ) )
               {
                  string table_name( "T_" + *mci + "_" + class_ids_and_names[ class_list[ i ] ] );

                  if( !all_table_info.empty( ) )
                     all_table_info += ",";

                  all_table_info += table_name + " WRITE";
               }
            }
         }

         if( !all_table_info.empty( ) )
            exec_sql( *gtp_session->up_db, "LOCK TABLES " + all_table_info );
      }
      catch( ... )
      {
         exec_sql( *gtp_session->up_db, "UNLOCK TABLES" );

         throw;
      }
   }
}

void storage_unlock_all_tables( )
{
   if( ods::instance( ) && gtp_session->p_storage_handler->get_ods( ) )
      exec_sql( *gtp_session->up_db, "UNLOCK TABLES" );
}

bool storage_locked_for_admin( )
{
   return gtp_session->p_storage_handler->get_is_locked_for_admin( );
}

ods& storage_ods_instance( )
{
   if( !ods::instance( ) )
      throw runtime_error( "storage has not been initialised" );

   return *ods::instance( );
}

struct storage_ods_bulk_read::impl
{
   impl( )
   {
      ods* p_ods = ods::instance( );

      if( !p_ods )
         throw runtime_error( "unexpected null ods::instance( ) in storage_ods_bulk_read" );

      if( !p_ods->is_thread_bulk_locked( ) )
      {
         try
         {
            up_ods_bulk_read.reset( new ods::bulk_read( *p_ods ) );

            if( gtp_session )
               gtp_session->p_storage_handler->set_bulk_lock_sess_id( );

            TRACE_LOG( TRACE_INITIAL | TRACE_LOCKING, "[bulk_read] storage_ods (obtained)" );
         }
         catch( ... )
         {
            size_t other_sess_id = 0;
            date_time dtm_bulk_locked;

            if( gtp_session )
            {
               other_sess_id = gtp_session->p_storage_handler->get_bulk_lock_sess_id( );
               dtm_bulk_locked = gtp_session->p_storage_handler->get_bulk_lock_date_time( );
            }

            TRACE_LOG( TRACE_MINIMAL, "[bulk_read] storage_ods **failed** (locked by session #"
             + to_string( other_sess_id ) + " " + dtm_bulk_locked.as_string( true, false ) + ")" );

            throw;
         }
      }
   }

   unique_ptr< ods::bulk_read > up_ods_bulk_read;
};

storage_ods_bulk_read::storage_ods_bulk_read( )
{
   p_impl = new impl;
}

storage_ods_bulk_read::~storage_ods_bulk_read( )
{
   uint64_t trace_flag = 0;

   if( p_impl && p_impl->up_ods_bulk_read.get( ) )
      trace_flag = ( TRACE_INITIAL | TRACE_LOCKING );

   TRACE_LOG( trace_flag, "[bulk_read] storage_ods (released)" );

   delete p_impl;
}

struct storage_ods_bulk_write::impl
{
   impl( progress* p_progress )
   {
      ods* p_ods = ods::instance( );

      if( !p_ods )
         throw runtime_error( "unexpected null ods::instance( ) in storage_ods_bulk_write" );

      if( !p_ods->is_thread_bulk_write_locked( ) )
      {
         try
         {
            up_ods_bulk_write.reset( new ods::bulk_write( *p_ods, p_progress ) );

            if( gtp_session )
               gtp_session->p_storage_handler->set_bulk_lock_sess_id( );

            TRACE_LOG( TRACE_INITIAL | TRACE_LOCKING, "[bulk_write] storage_ods (obtained)" );
         }
         catch( ... )
         {
            size_t other_sess_id = 0;

            date_time dtm_bulk_locked;

            if( gtp_session )
            {
               other_sess_id = gtp_session->p_storage_handler->get_bulk_lock_sess_id( );
               dtm_bulk_locked = gtp_session->p_storage_handler->get_bulk_lock_date_time( );
            }

            TRACE_LOG( TRACE_MINIMAL, "[bulk_write] storage_ods **failed** (locked by session #"
             + to_string( other_sess_id ) + " " + dtm_bulk_locked.as_string( true, false ) + ")" );

            throw;
         }
      }
   }

   unique_ptr< ods::bulk_write > up_ods_bulk_write;
};

storage_ods_bulk_write::storage_ods_bulk_write( progress* p_progress )
{
   p_impl = new impl( p_progress );
}

storage_ods_bulk_write::~storage_ods_bulk_write( )
{
   uint64_t trace_flag = 0;

   if( p_impl && p_impl->up_ods_bulk_write.get( ) )
      trace_flag = ( TRACE_INITIAL | TRACE_LOCKING );

   TRACE_LOG( trace_flag, "[bulk_write] storage_ods (released)" );

   delete p_impl;
}

ods& system_ods_instance( )
{
   return *gup_ods;
}

ods_file_system& system_ods_file_system( )
{
   return *gup_ofs;
}

void export_repository_entries( )
{
   system_ods_fs_guard ods_fs_guard;

   system_ods_bulk_read ods_bulk_read;

   gup_ofs->set_root_folder( c_system_repository_folder );

   export_objects( *gup_ofs, c_system_repository_folder );
}

struct system_ods_bulk_read::impl
{
   impl( )
   {
      if( !gup_ods->is_thread_bulk_locked( ) )
      {
         try
         {
            up_ods_bulk_read.reset( new ods::bulk_read( *gup_ods ) );

            up_scoped_instance.reset( new scoped_ods_instance( *gup_ods ) );

            TRACE_LOG( TRACE_INITIAL | TRACE_LOCKING, "[bulk_read] system_ods (obtained)" );
         }
         catch( ... )
         {
            TRACE_LOG( TRACE_MINIMAL, "[bulk_read] system_ods **failed**" );

            throw;
         }
      }
   }

   unique_ptr< ods::bulk_read > up_ods_bulk_read;
   unique_ptr< scoped_ods_instance > up_scoped_instance;
};

system_ods_bulk_read::system_ods_bulk_read( )
{
   p_impl = new impl;
}

system_ods_bulk_read::~system_ods_bulk_read( )
{
   uint64_t trace_flag = 0;

   if( p_impl && p_impl->up_ods_bulk_read.get( ) )
      trace_flag = ( TRACE_INITIAL | TRACE_LOCKING );

   TRACE_LOG( trace_flag, "[bulk_read] system_ods (released)" );

   delete p_impl;
}

struct system_ods_bulk_write::impl
{
   impl( progress* p_progress )
   {
      if( !gup_ods->is_thread_bulk_write_locked( ) )
      {
         try
         {
            up_ods_bulk_write.reset( new ods::bulk_write( *gup_ods, p_progress ) );

            up_scoped_instance.reset( new scoped_ods_instance( *gup_ods ) );

            TRACE_LOG( TRACE_INITIAL | TRACE_LOCKING, "[bulk_write] system_ods (obtained)" );
         }
         catch( ... )
         {
            TRACE_LOG( TRACE_MINIMAL, "[bulk_write] system_ods **failed**" );

            throw;
         }
      }
   }

   unique_ptr< ods::bulk_write > up_ods_bulk_write;
   unique_ptr< scoped_ods_instance > up_scoped_instance;
};

system_ods_bulk_write::system_ods_bulk_write( progress* p_progress )
{
   p_impl = new impl( p_progress );
}

system_ods_bulk_write::~system_ods_bulk_write( )
{
   uint64_t trace_flag = 0;

   if( p_impl && p_impl->up_ods_bulk_write.get( ) )
      trace_flag = ( TRACE_INITIAL | TRACE_LOCKING );

   TRACE_LOG( trace_flag, "[bulk_write] system_ods (released)" );

   delete p_impl;
}

string gen_key( const char* p_suffix, bool use_get_dtm )
{
   string key( 20, '\0' );

   if( !gtp_session )
      throw runtime_error( "unexpected non-session call made to 'gen_key'" );
   else
   {
      guard g( g_mutex );

      if( gtp_session->p_storage_handler->get_root( ).type == e_storage_type_standard )
      {
         if( use_get_dtm )
            key = get_dtm( );
         else
         {
            // NOTE: For non-peerchain storages uses human readable format
            // (rather than hexadeciaml) for keys (which limits the number
            // of keys that can be generated to 1000 per second).
            while( true )
            {
               int64_t now = unix_time( );

               if( now != g_key_tm_val )
               {
                  g_key_count = 0;
                  g_key_tm_val = now;

                  break;
               }
               else
               {
                  // NOTE: Supports 000-999 suffixes to the unix time value
                  // (and will reset it after waiting for the next second).
                  if( g_key_count >= 999 )
                     msleep( 10 );
                  else
                  {
                     ++g_key_count;

                     break;
                  }
               }
            }

            date_time dtm( g_key_tm_val );

            key = dtm.as_string( e_time_format_hhmmss );
         }

         // NOTE: Leave three trailing zeroes so that
         // each key can have 999 further derivations
         // (such as automatic creation of of records
         // in Meta for Classes and Fields).
         char ccc[ ] = "ccc000";

         size_t count = ( use_get_dtm ? 0 : g_key_count );

         ccc[ 0 ] = '0' + ( count / 100 );
         ccc[ 1 ] = '0' + ( ( count % 100 ) / 10 );
         ccc[ 2 ] = '0' + ( count % 10 );

         key += string( ccc );
      }
      else
      {
         if( !use_get_dtm )
         {
            while( true )
            {
               int64_t now = unix_time( );

               if( now != g_key_tm_val )
               {
                  g_key_count = 0;
                  g_key_tm_val = now;

                  break;
               }
               else
               {
                  // NOTE: Supports 000-fff suffixes to the unix time value
                  // (and will reset it after waiting for the next second).
                  if( g_key_count >= 4095 )
                     msleep( 10 );
                  else
                  {
                     ++g_key_count;

                     break;
                  }
               }
            }
         }

         size_t count = ( use_get_dtm ? 0 : g_key_count );

         int64_t tm_val = ( use_get_dtm ? unix_time( date_time( get_dtm( ) ) ) : g_key_tm_val );

         ostringstream osstr;

         osstr << hex << tm_val << setw( 3 ) << setfill( '0' ) << count;

         key = osstr.str( ) + gtp_session->identity_suffix;
      }

      if( p_suffix )
         key += string( p_suffix );
   }

   return key;
}

string get_uid( bool remove_display_name )
{
   string uid( gtp_session->uid );

   string::size_type pos = string::npos;

   if( remove_display_name )
      pos = uid.find( ':' );

   return uid.substr( 0, pos );
}

void set_uid( const string& uid, bool do_not_erase_sec )
{
   string s( uid );

   string::size_type pos = uid.find( ':' );
   string::size_type spos = uid.find( '!' );

   if( !do_not_erase_sec )
   {
      gtp_session->sec.erase( );

      set_session_variable( get_special_var_name( e_special_var_sec ), "" );
   }

   if( spos != string::npos )
   {
      if( ( pos == string::npos ) || ( pos > spos ) )
      {
         string sec = uid.substr( spos + 1, pos == string::npos ? pos : pos - spos - 1 );

         gtp_session->sec = sec;

         set_session_variable( get_special_var_name( e_special_var_sec ), sec );

         s = uid.substr( 0, spos );

         if( pos != string::npos )
            s += uid.substr( pos );
      }
   }

   pos = s.find( ':' );

   string user_key( s.substr( 0, pos ) );

   if( user_key == c_uid_anon )
   {
      gtp_session->uid.erase( );

      set_session_variable( get_special_var_name( e_special_var_uid ), "" );
   }
   else
   {
      gtp_session->uid = s;

      set_session_variable( get_special_var_name( e_special_var_uid ), user_key );

      if( pos != string::npos )
         set_session_variable( get_special_var_name( e_special_var_user ), s.substr( pos + 1 ) );
   }
}

bool is_sys_uid( )
{
   return get_uid( ) == c_sys;
}

bool is_anon_uid( )
{
   return get_uid( ).empty( );
}

bool is_auto_uid( )
{
   return get_uid( ) == c_auto;
}

bool is_init_uid( )
{
   return get_uid( ) == c_init;
}

bool is_peer_uid( )
{
   return get_uid( ) == c_peer;
}

bool is_admin_uid( )
{
   return get_uid( ) == c_admin;
}

bool is_system_uid( )
{
   bool rc = false;

   string uid( get_uid( ) );

   if( ( uid == c_sys ) || ( uid == c_auto ) || ( uid == c_init ) || ( uid == c_peer ) )
      rc = true;

   return rc;
}

bool is_admin_uid_key( const string& key )
{
   return key == c_admin;
}

bool is_uid_not_self_and_not_in_set( const string& key, const string& key_set )
{
   bool rc = false;

   if( !is_admin_uid_key( key ) )
   {
      if( key_set.empty( ) )
         rc = ( key != get_uid( ) );
      else if( key != get_uid( ) )
      {
         set< string > keys;

         split( key_set, keys );

         rc = !keys.count( key );
      }
   }

   return rc;
}

bool has_sec_level( const string& level )
{
   // NOTE: Security level strings (e.g. XXXX) are shorter for higher.
   if( gtp_session->sec.empty( ) || ( gtp_session->sec <= level ) )
      return true;
   else
      return false;
}

bool has_tx_key_info( const string& key_info )
{
   bool retval = false;

   if( gtp_session )
      retval = gtp_session->tx_key_info.count( key_info );

   return retval;
}

string get_grp( )
{
   string grp;

   if( gtp_session )
      grp = gtp_session->grp;

   return grp;
}

void set_grp( const string& grp )
{
   if( gtp_session )
   {
      string::size_type pos = grp.find( ':' );

      gtp_session->grp = grp.substr( 0, pos );

      set_session_variable( get_special_var_name( e_special_var_grp ), grp.substr( 0, pos ) );

      gtp_session->gid = convert_group_keys_to_numbers( gtp_session->grp );

      string gids;

      if( pos != string::npos )
         gids = convert_group_keys_to_numbers( grp.substr( pos + 1 ) );

      set_session_variable( get_special_var_name( e_special_var_gids ), gids );
   }
}

string get_dtm( )
{
   if( gtp_session->dtm.empty( ) )
      gtp_session->dtm = date_time::standard( ).as_string( );

   return gtp_session->dtm;
}

void set_dtm( const string& dtm )
{
   gtp_session->dtm = dtm;

   set_session_variable( get_special_var_name( e_special_var_dtm ), dtm );
}

string get_gid( )
{
   return gtp_session->gid;
}

bool has_sql_db( )
{
   return gtp_session->up_db.get( );
}

sql_db& get_sql_db( )
{
   return *gtp_session->up_db;
}

void set_class( const string& mclass )
{
   set_session_variable( get_special_var_name( e_special_var_class ), mclass );
}

void set_module( const string& module )
{
   set_session_variable( get_special_var_name( e_special_var_module ), module );
}

string get_tz_name( )
{
   return gtp_session->tz_name;
}

void set_tz_name( const string& tz_name )
{
   string tz( tz_name );

   if( tz.empty( ) )
      tz = get_timezone( );

   gtp_session->tz_name = tz;

   set_session_variable( get_special_var_name( e_special_var_tz_name ), tz );
}

void clear_perms( )
{
   gtp_session->perms.clear( );
}

const set< string >& get_perms( )
{
   return gtp_session->perms;
}

void set_perms( const set< string >& perms )
{
   gtp_session->perms = perms;
}

string get_tmp_directory( )
{
   return gtp_session->tmp_directory;
}

void set_tmp_directory( const string& tmp_directory )
{
   gtp_session->tmp_directory = tmp_directory;
}

bool has_session_secret( )
{
   bool retval = false;

   if( gtp_session )
      retval = !gtp_session->secret.empty( );

   return retval;
}

string get_session_secret( )
{
   return gtp_session->secret;
}

void set_session_secret( const string& secret )
{
   gtp_session->secret = secret;
}

void session_shared_decrypt( string& data, const string& pubkey, const string& message )
{
   if( pubkey.empty( ) )
      return;

#ifdef SSL_SUPPORT
   public_key pub_key( pubkey );
   gtp_session->priv_key.decrypt_message( data, pub_key, message );
#else
   throw runtime_error( "session_shared_decrypt requires SSL support" );
#endif
}

void session_shared_encrypt( string& data, const string& pubkey, const string& message )
{
#ifdef SSL_SUPPORT
   if( pubkey.empty( ) )
      throw runtime_error( "invalid empty key value in session_shared_encrypt" );

   public_key pub_key( pubkey );
   gtp_session->priv_key.encrypt_message( data, pub_key, message, 0, true );
#else
   throw runtime_error( "session_shared_encrypt requires SSL support" );
#endif
}

void add_security_group( const string& group_key )
{
   guard g( g_gid_mutex );

   storage_ods_bulk_write ods_bulk_write;

   ods_file_system ofs( *ods::instance( ) );

   if( !ofs.has_folder( c_storage_folder_name_gid_data ) )
      ofs.add_folder( c_storage_folder_name_gid_data );

   ofs.set_folder( c_storage_folder_name_gid_data );

   string gid_file_name( group_key + '.'
    + to_string( ++gtp_session->p_storage_handler->get_root( ).group_identity_new ) );

   ofs.add_file( gid_file_name, "*" );
}

int64_t group_security_value( const string& group_key_value )
{
   guard g( g_gid_mutex );

   storage_ods_bulk_read ods_bulk_read;

   ods_file_system ofs( *ods::instance( ) );

   ofs.set_folder( c_storage_folder_name_gid_data );

   string group_number;

   if( !ofs.has_file( group_key_value + '.', true, &group_number ) )
      throw runtime_error( "unexpected gid_data for '" + group_key_value + "' not found" );

   return from_string< int64_t >( group_number ) * 10;
}

string convert_group_keys_to_numbers( const string& group_keys )
{
   string group_numbers;

   if( !group_keys.empty( ) )
   {
      guard g( g_gid_mutex );

      vector< string > groups;

      split( group_keys, groups, '|' );

      storage_ods_bulk_read ods_bulk_read;

      ods_file_system ofs( *ods::instance( ) );

      if( ofs.has_folder( c_storage_folder_name_gid_data ) )
      {
         ofs.set_folder( c_storage_folder_name_gid_data );

         for( size_t i = 0; i < groups.size( ); i++ )
         {
            string next( groups[ i ] );

            string group_number;

            if( !ofs.has_file( next + '.', true, &group_number ) )
               throw runtime_error( "unexpected gid_data for '" + next + "' not found" );

            if( !group_numbers.empty( ) )
               group_numbers += '|';

            group_numbers += group_number;
         }
      }
   }

   return group_numbers;
}

bool get_uid_data( const string& uid, size_t& level, string& gids )
{
   guard g( g_uid_mutex );

   bool retval = false;

   storage_ods_bulk_read ods_bulk_read;

   ods_file_system ofs( *ods::instance( ) );

   if( ofs.has_folder( c_storage_folder_name_uid_data ) )
   {
      ofs.set_folder( c_storage_folder_name_uid_data );

      string level_suffix;

      if( ofs.has_file( uid + '.', true, &level_suffix ) )
      {
         retval = true;

         level = from_string< size_t >( level_suffix );

         ofs.fetch_from_text_file( uid + '.' + level_suffix, gids );
      }
   }

   return retval;
}

void set_uid_data( const string& uid, const string& level, const string& group_keys )
{
   guard g( g_uid_mutex );

   string gids( convert_group_keys_to_numbers( group_keys ) );

   storage_ods_bulk_write ods_bulk_write;

   ods::transaction tx( *ods::instance( ) );

   ods_file_system ofs( *ods::instance( ) );

   if( !ofs.has_folder( c_storage_folder_name_uid_data ) )
      ofs.add_folder( c_storage_folder_name_uid_data );

   ofs.set_folder( c_storage_folder_name_uid_data );

   size_t level_value = 0;

   if( level.length( ) )
      level_value = 10 - level.length( );

   string level_suffix;

   // NOTE: If the access level has changed need to remove
   // the current record (as level is appended to the uid).
   if( ofs.has_file( uid + '.', true, &level_suffix ) )
   {
      if( level_suffix != to_string( level_value ) )
         ofs.remove_file( uid + '.' + level_suffix );
   }

   string file_name( uid + '.' + to_string( level_value ) );

   ofs.store_as_text_file( file_name, gids );

   tx.commit( );
}

size_t get_next_handle( )
{
   return ++gtp_session->next_handle;
}

void module_list( ostream& os )
{
   module_const_iterator mci = gtp_session->modules_by_name.begin( );
   module_const_iterator end = gtp_session->modules_by_name.end( );

   for( ; mci != end; ++mci )
      os << mci->second << ' ' << mci->first << '\n';
}

size_t module_count( )
{
   return gtp_session->modules_by_name.size( );
}

string module_identity( const string& module )
{
   string module_id( module );

   if( !gtp_session->modules_by_id.count( module ) )
   {
      if( !gtp_session->modules_by_name.count( module ) )
         throw runtime_error( "unable to resolve module id/name '" + module + "'" );

      module_id = gtp_session->modules_by_name.find( module )->second;
   }

   return module_id;
}

void module_class_list( const string& module, ostream& os, const char* p_pat )
{
   module_const_iterator mci = gtp_session->modules_by_id.find( module );

   if( mci == gtp_session->modules_by_id.end( ) )
   {
      mci = gtp_session->modules_by_name.find( module );

      if( mci == gtp_session->modules_by_name.end( ) )
         throw runtime_error( get_string_message( GS( c_str_module_not_loaded ),
          make_pair( c_str_module_not_loaded_module, module ) ) );
   }

   module_class_list_error rc = list_module_classes( module, os, -1, p_pat );

   if( rc != e_module_class_list_error_none )
      throw runtime_error( "unexpected module class list error #" + to_string( rc ) );
}

void module_strings_list( const string& module, ostream& os )
{
   module_const_iterator mci = gtp_session->modules_by_id.find( module );

   if( mci == gtp_session->modules_by_id.end( ) )
   {
      mci = gtp_session->modules_by_name.find( module );

      if( mci == gtp_session->modules_by_name.end( ) )
         throw runtime_error( get_string_message( GS( c_str_module_not_loaded ),
          make_pair( c_str_module_not_loaded_module, module ) ) );
   }

   module_string_list_error rc = list_module_strings( module, os );

   if( rc != e_module_string_list_error_none )
      throw runtime_error( "unexpected module string list error #" + to_string( rc ) );
}

void module_class_fields_list( const string& module, const string& class_id_or_name, ostream& os )
{
   module_const_iterator mci = gtp_session->modules_by_id.find( module );

   if( mci == gtp_session->modules_by_id.end( ) )
   {
      mci = gtp_session->modules_by_name.find( module );

      if( mci == gtp_session->modules_by_name.end( ) )
         throw runtime_error( get_string_message( GS( c_str_module_not_loaded ),
          make_pair( c_str_module_not_loaded_module, module ) ) );
   }

   module_class_field_list_error rc = list_module_class_fields( module, class_id_or_name, os );

   if( rc != e_module_class_field_list_error_none )
      throw runtime_error( "unexpected module class field list error #" + to_string( rc ) );
}

void module_class_procedures_list( const string& module, const string& class_id_or_name, ostream& os )
{
   module_const_iterator mci = gtp_session->modules_by_id.find( module );

   if( mci == gtp_session->modules_by_id.end( ) )
   {
      mci = gtp_session->modules_by_name.find( module );

      if( mci == gtp_session->modules_by_name.end( ) )
         throw runtime_error( get_string_message( GS( c_str_module_not_loaded ),
          make_pair( c_str_module_not_loaded_module, module ) ) );
   }

   module_class_procedure_list_error rc = list_module_class_procedures( module, class_id_or_name, os );

   if( rc != e_module_class_procedure_list_error_none )
      throw runtime_error( "unexpected module class procedure list error #" + to_string( rc ) );
}

void module_load( const string& module_name,
 command_handler& cmd_handler, bool log_tx_comment, bool append_to_module_list )
{
   if( gtp_session->modules_by_name.find( module_name ) != gtp_session->modules_by_name.end( ) )
      throw runtime_error( get_string_message( GS( c_str_module_is_loaded ),
       make_pair( c_str_module_is_loaded_module, module_name ) ) );

   module_load_error rc = load_module( module_name );

   if( rc != e_module_load_error_none )
   {
      if( rc == e_module_load_error_file_does_not_exist )
         throw runtime_error( get_string_message( GS( c_str_module_not_exists ),
          make_pair( c_str_module_not_exists_module, module_name ) ) );
      else if( rc == e_module_load_error_external_module_failure )
         throw runtime_error( get_string_message( GS( c_str_module_depends_reqd ),
          make_pair( c_str_module_depends_reqd_module, module_name ) ) );
      else
         throw runtime_error( "unexpected module load error #" + to_string( rc ) );
   }

   if( ods::instance( ) && gtp_session->p_storage_handler->get_ods( ) )
   {
      if( ods::instance( )->get_transaction_level( ) )
      {
         unload_module( module_name );
         throw runtime_error( "cannot load a module whilst a transaction is active" );
      }

      unique_ptr< guard > up_guard( new guard( g_mutex ) );

      ods* p_ods( ods::instance( ) );

      storage_handler& handler( *gtp_session->p_storage_handler );

      if( find( handler.get_root( ).module_list.begin( ),
       handler.get_root( ).module_list.end( ), module_name ) == handler.get_root( ).module_list.end( ) )
      {
         if( !handler.get_is_locked_for_admin( ) )
         {
            unload_module( module_name );
            throw runtime_error( "modules can only be added to a storage during storage administration" );
         }

         gtp_session->storage_controlled_modules.push_back( module_name );

         handler.get_root( ).module_list.push_back( module_name );

         try
         {
            string temp_sql_file_name;

            vector< string > class_list;

            list_module_classes( module_name, class_list );

            map< string, string > class_ids_and_names;

            list_module_classes( module_name, class_ids_and_names, true );

            // NOTE: Create/append to a DDL file (which is the storage name with a ".sql" extension).
            string sql_file_name( handler.get_name( ) );

            sql_file_name += ".sql";

            temp_sql_file_name = "~" + sql_file_name;

            bool is_first = true;

            bool file_existed = file_exists( sql_file_name );

            // NOTE: Empty code block for scope purposes.
            {
               ofstream out1( sql_file_name.c_str( ), ios::out | ios::app );

               if( !out1 )
                  throw runtime_error(
                   "unable to open file '" + sql_file_name + "' for output/append in module_load" );

               ofstream out2( temp_sql_file_name.c_str( ) );

               if( !out2 )
                  throw runtime_error(
                   "unable to open file '" + temp_sql_file_name + "' for output in module_load" );

               if( file_existed )
                  out1 << '\n';

               tee_stream outf( out1, out2 );

               outf << "BEGIN;\n";

               for( size_t i = 0; i < class_list.size( ); i++ )
               {
                  vector< string > columns;

                  string sql_columns( get_sql_columns_for_module_class( module_name, class_list[ i ] ) );

                  if( !sql_columns.empty( ) )
                  {
                     split( sql_columns, columns );

                     string table_name( "T_" + module_name + "_" + class_ids_and_names[ class_list[ i ] ] );

                     outf << "\nDROP TABLE IF EXISTS " << table_name << ";\n";

                     // FUTURE: This message should be handled as a server string message.
                     outf << "\n#Creating table and indexes for " << module_name
                      << "_" << class_ids_and_names[ class_list[ i ] ] << "\n";

                     outf << "\nCREATE TABLE " << table_name << '\n';
                     outf << "(\n";

                     for( size_t j = 0; j < columns.size( ); j++ )
                     {
                        if( j > 0 )
                           outf << ",\n";

                        outf << " " << columns[ j ];
                     }

                     outf << "\n);\n";

                     vector< string > sql_indexes;
                     get_sql_indexes_for_module_class( module_name, class_list[ i ], sql_indexes );

                     string index_prefix( "I_" + module_name + "_" + class_ids_and_names[ class_list[ i ] ] );

                     for( size_t j = 0; j < sql_indexes.size( ); j++ )
                     {
                        vector< string > index_columns;

                        split( sql_indexes[ j ], index_columns );

                        outf << "\nCREATE UNIQUE INDEX " << index_prefix << "_";

                        if( j < 10 )
                           outf << '0';

                        outf << j << " ON " << table_name << '\n';
                        outf << "(\n";
                        outf << " C_Sec_";

                        for( size_t k = 0; k < index_columns.size( ); k++ )
                        {
                           outf << ",\n";
                           outf << " " << index_columns[ k ];
                        }

                        outf << "\n);\n";
                     }
                  }
               }

               outf << "\nCOMMIT;\n";

               outf.flush( );

               if( gtp_session->up_db.get( ) )
               {
                  // NOTE: As MySQL DDL operations with InnoDB can be very slow the global
                  // thread lock is released whilst performing the DDL. As the storage has
                  // already been locked for administration this should be of no concern.
                  up_guard.reset( );

                  try
                  {
                     exec_sql_from_file( *gtp_session->up_db, temp_sql_file_name, &cmd_handler );
                  }
                  catch( ... )
                  {
                     gtp_session->storage_controlled_modules.pop_back( );

                     // NOTE: Restore the storage state (otherwise a SQL error can be lost
                     // due to the fact that the storage is left in an inconsistent state).
                     ods_file_system ofs( *p_ods );
                     handler.get_root( ).fetch_from_text_files( ofs );

                     throw;
                  }

                  up_guard.reset( new guard( g_mutex ) );
               }

               storage_ods_bulk_write ods_bulk_write;

               ods_file_system ofs( *p_ods );

               ofs.set_folder( c_storage_folder_name_modules );

               string prefix( to_comparable_string( handler.get_root( ).module_list.size( ), false, 3 ) );

               ofs.add_folder( prefix + c_module_order_prefix_separator + module_name );

               if( log_tx_comment )
               {
                  gtp_session->transaction_log_command = ";module ==> " + module_name;

                  append_transaction_log_command( handler, true, handler.get_root( ).module_list.size( ) );
               }

               if( append_to_module_list )
               {
                  string module_list_file( handler.get_name( ) + ".modules.lst" );

                  set< string > existing_modules;

                  if( file_exists( module_list_file ) )
                     buffer_file_lines( module_list_file, existing_modules );

                  if( !existing_modules.count( module_name ) )
                  {
                     ofstream modf( module_list_file.c_str( ), ios::out | ios::app );

                     if( !modf )
                        throw runtime_error( "unexpected error opening '" + module_list_file + "' for output/append" );

                     modf << module_name << endl;
                  }
               }
            }

            if( !temp_sql_file_name.empty( ) )
               remove( temp_sql_file_name.c_str( ) );
         }
         catch( ... )
         {
            unload_module( module_name );
            throw;
         }
      }
   }

   // NOTE: If a "user" class exists for the module then create
   // two session variables to allow the system to identify it.
   vector< string > class_list;

   list_module_classes( module_name, class_list, 1 );

   if( !class_list.empty( ) )
   {
      gtp_session->variables.insert( make_pair(
       string( "@" + module_name + c_user_class_suffix ), class_list[ 0 ] ) );

      gtp_session->variables.insert( make_pair(
       string( "@" + get_module_id( module_name ) + c_user_class_suffix ), class_list[ 0 ] ) );
   }

   gtp_session->modules_by_id.insert( module_value_type( get_module_id( module_name ), module_name ) );
   gtp_session->modules_by_name.insert( module_value_type( module_name, get_module_id( module_name ) ) );

   register_module_commands( module_name, cmd_handler );
}

void module_unload( const string& module_name, command_handler& cmd_handler, bool check_controlled )
{
   if( check_controlled )
   {
      for( size_t i = 0; i < gtp_session->storage_controlled_modules.size( ); i++ )
      {
         if( gtp_session->storage_controlled_modules[ i ] == module_name )
            throw runtime_error( "cannot unload storage controlled module '" + module_name + "'" );
      }
   }

   module_iterator mi = gtp_session->modules_by_name.find( module_name );

   if( mi == gtp_session->modules_by_name.end( ) )
      throw runtime_error( get_string_message( GS( c_str_module_not_loaded ),
       make_pair( c_str_module_not_loaded_module, module_name ) ) );

   destroy_object_instances( module_name );

   unregister_module_commands( module_name, cmd_handler );

   module_unload_error rc = unload_module( module_name );

   if( rc != e_module_unload_error_none )
   {
      if( rc == e_module_unload_error_name_unknown )
         throw runtime_error( "module '" + module_name + "' was not found to be loaded" );
      else
         throw runtime_error( "unexpected module unload error #" + to_string( rc ) );
   }

   gtp_session->modules_by_name.erase( mi );

   mi = gtp_session->modules_by_id.find( get_module_id( module_name ) );

   if( mi != gtp_session->modules_by_id.end( ) )
      gtp_session->modules_by_id.erase( mi );
}

void module_unload_all( command_handler& cmd_handler )
{
   vector< string > module_names;

   for( module_iterator mi = gtp_session->modules_by_name.begin( ); mi != gtp_session->modules_by_name.end( ); ++mi )
      module_names.push_back( mi->first );

   for( size_t i = 0; i < module_names.size( ); i++ )
      module_unload( module_names[ i ], cmd_handler );
}

string loaded_module_id( const string& module )
{
   string module_id( module );

   if( !gtp_session->modules_by_id.count( module ) )
   {
      if( !gtp_session->modules_by_name.count( module ) )
         throw runtime_error( "unable to resolve module id/name '" + module + "'" );

      module_id = gtp_session->modules_by_name.find( module )->second;
   }

   return module_id;
}

string loaded_module_name( const string& module )
{
   string module_name( module );

   if( !gtp_session->modules_by_name.count( module ) )
   {
      if( !gtp_session->modules_by_id.count( module ) )
         throw runtime_error( "unable to resolve module id/name '" + module + "'" );

      module_name = gtp_session->modules_by_id.find( module )->second;
   }

   return module_name;
}

void register_module_commands( const string& module_name, command_handler& handler )
{
   dynamic_library* p_dynamic_library = get_module_ptr( module_name );

   fp_obtain_module_commands obtain_module_commands;

   obtain_module_commands =
    ( fp_obtain_module_commands )p_dynamic_library->bind_to_function( c_obtain_module_commands_func_name );

   if( obtain_module_commands )
   {
      size_t num_commands( 0 );
      command_definition* p_command_definitions;
      command_functor_creator* p_command_functor_creator;

      ( *obtain_module_commands )( p_command_functor_creator, p_command_definitions, &num_commands );

      if( num_commands )
      {
         handler.add_commands( 0, module_name + c_module_prefix_separator,
          *p_command_functor_creator, p_command_definitions, num_commands );

         gtp_session->commands_registry.insert( module_commands_registry_value_type( module_name,
          module_commands( num_commands, p_command_definitions, p_command_functor_creator ) ) );
      }
   }
}

void unregister_module_commands( const string& module_name, command_handler& handler )
{
   module_commands_registry_container& commands_registry( gtp_session->commands_registry );

   module_commands_registry_iterator i = commands_registry.find( module_name );

   if( i != commands_registry.end( ) )
   {
      handler.remove_commands( module_name + c_module_prefix_separator,
       i->second.p_command_definitions, i->second.num_commands );

      commands_registry.erase( i );
   }
}

void unregister_all_module_commands( command_handler& handler )
{
   module_commands_registry_container& commands_registry( gtp_session->commands_registry );

   module_commands_registry_iterator i( commands_registry.begin( ) );

   while( i != commands_registry.end( ) )
   {
      handler.remove_commands( i->first + c_module_prefix_separator,
       i->second.p_command_definitions, i->second.num_commands );

      commands_registry.erase( i++ );
   }
}

void list_object_instances( ostream& os )
{
   object_instance_registry_iterator oiri, end;
   object_instance_registry_container& instance_registry( gtp_session->instance_registry );

   for( oiri = instance_registry.begin( ), end = instance_registry.end( ); oiri != end; ++oiri )
   {
      os << oiri->first
       << ' ' << ( oiri->second ).p_dynamic_library->get_module_name( )
       << ' ' << ( oiri->second ).p_class_base->get_class_name( ) << '\n';
   }
}

size_t create_object_instance( const string& module, const string& class_id, size_t handle, bool dynamic_enabled )
{
   dynamic_library* p_dynamic_library = get_module_ptr( module );

   if( !p_dynamic_library )
      throw runtime_error( "unknown module '" + module + "'" );

   class_base* p_class_base = construct_object( module, class_id );

   if( !p_class_base )
      throw runtime_error( "unknown class id '" + class_id + "'" );

   class_base_accessor cba( *p_class_base );

   cba.set_default_values( );

   if( dynamic_enabled )
      cba.set_is_dynamic_enabled( true );

   object_instance_registry_container& instance_registry( gtp_session->instance_registry );

   if( handle == 0 )
      handle = get_next_handle( );
   else
   {
      if( instance_registry.find( handle ) != instance_registry.end( ) )
         throw runtime_error( "object instance handle #" + to_string( handle ) + " is currently in use" );
   }

   instance_registry.insert(
    object_instance_registry_value_type( handle, instance_info( p_class_base, p_dynamic_library ) ) );

   return handle;
}

string execute_object_command( size_t handle, const string& context, const string& method_name_and_args )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   TRACE_LOG( TRACE_INITIAL | TRACE_OBJECTS,
    "execute_object_command( ) [class: " + instance.get_class_name( ) + "] " + method_name_and_args );

   return instance.execute( method_name_and_args );
}

void init_object_instance( size_t handle, const string& context, bool for_create )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   instance.init( for_create );
}

void prepare_object_instance( size_t handle,
 const string& context, bool for_create, bool call_to_store, bool starting_iteration )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   instance.prepare( for_create, call_to_store, starting_iteration );
}

void validate_object_instance( size_t handle, const string& context )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   if( !instance.is_valid( false ) )
   {
      string validation_error( instance.get_validation_errors( class_base::e_validation_errors_type_first_only ) );
      set_session_variable( get_special_var_name( e_special_var_val_error ), validation_error );
      throw runtime_error( validation_error );
   }
}

void destroy_object_instance( size_t handle )
{
   object_instance_registry_container& instance_registry( gtp_session->instance_registry );
   object_instance_registry_iterator oiri = instance_registry.find( handle );

   if( oiri == instance_registry.end( ) )
      throw runtime_error( "invalid object instance handle #" + to_string( handle ) );

   class_base* p_class_base( ( oiri->second ).p_class_base );

   dynamic_library* p_dynamic_library( ( oiri->second ).p_dynamic_library );

   destroy_object( p_dynamic_library->get_module_name( ), p_class_base->get_class_id( ), p_class_base );

   assert( p_class_base == 0 );

   instance_registry.erase( oiri );
}

void destroy_object_instances( const string& module_name )
{
   dynamic_library* p_dynamic_library = get_module_ptr( module_name );

   if( !p_dynamic_library )
      throw runtime_error( "unknown module name '" + module_name + "'" );

   object_instance_registry_iterator oiri, end;
   object_instance_registry_container& instance_registry( gtp_session->instance_registry );

   for( oiri = instance_registry.begin( ), end = instance_registry.end( ); oiri != end; )
   {
      if( ( oiri->second ).p_dynamic_library == p_dynamic_library )
      {
         class_base* p_class_base( ( oiri->second ).p_class_base );

         dynamic_library* p_dynamic_library( ( oiri->second ).p_dynamic_library );

         destroy_object( p_dynamic_library->get_module_name( ), p_class_base->get_class_id( ), p_class_base );

         assert( p_class_base == 0 );

         instance_registry.erase( oiri++ );
      }
      else
         oiri++;
   }
}

void destroy_all_object_instances( )
{
   object_instance_registry_iterator oiri, end;
   object_instance_registry_container& instance_registry( gtp_session->instance_registry );

   for( oiri = instance_registry.begin( ), end = instance_registry.end( ); oiri != end; ++oiri )
   {
      class_base* p_class_base( ( oiri->second ).p_class_base );

      dynamic_library* p_dynamic_library( ( oiri->second ).p_dynamic_library );

      destroy_object( p_dynamic_library->get_module_name( ), p_class_base->get_class_id( ), p_class_base );

      assert( p_class_base == 0 );
   }

   instance_registry.clear( );
}

string get_class_id( size_t handle, const string& context )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.get_class_id( );
}

string get_class_name( size_t handle, const string& context )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.get_class_name( );
}

string get_class_display_name( size_t handle, const string& context, bool plural )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.get_display_name( plural );
}

string get_create_instance_info( size_t handle, const string& context )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.get_create_instance_info( );
}

string get_update_instance_info( size_t handle, const string& context )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.get_update_instance_info( );
}

string get_destroy_instance_info( size_t handle, const string& context )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.get_destroy_instance_info( );
}

string get_execute_procedure_info( size_t handle, const string& context, const string& procedure_id )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.get_execute_procedure_info( procedure_id );
}

string get_instance_owner( size_t handle, const string& context )
{
   string owner;

   class_base& instance( get_class_base_from_handle( handle, context ) );

   if( instance.get_class_type( ) == 1 ) // i.e. user
      owner = instance.get_key( );
   else
   {
      string owner_field( instance.get_owner_field_name( ) );

      if( !owner_field.empty( ) )
      {
         string method_name_and_args( "get " );
         method_name_and_args += owner_field;

         owner = instance.execute( method_name_and_args );
      }
   }

   return owner;
}

void get_all_enum_pairs( size_t handle, const string& context, vector< pair< string, string > >& pairs )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   instance.get_all_enum_pairs( pairs );
}

void get_all_field_names( size_t handle, const string& context, vector< string >& fields, bool include_transients )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   for( int i = 0; i < instance.get_num_fields( ); i++ )
   {
      if( !include_transients && instance.is_field_transient( i ) )
         continue;

      fields.push_back( instance.get_field_name( i ) );
   }
}

void get_all_field_scope_and_permission_info( size_t handle,
 const string& context, map< string, string >& field_scope_and_perm_info, bool by_name )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   field_info_container field_info;

   instance.get_field_info( field_info );

   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      field_scope_and_perm_info.insert( make_pair(
       ( !by_name ) ? field_info[ i ].id : field_info[ i ].name,
       field_info[ i ].scope + "," + field_info[ i ].change ) );
   }
}

string get_field_name_for_id( size_t handle, const string& context, const string& id, bool no_throw )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   map< string, string > ids_to_names;

   field_info_container field_info;

   instance.get_field_info( field_info );

   for( size_t i = 0; i < field_info.size( ); i++ )
      ids_to_names.insert( make_pair( field_info[ i ].id, field_info[ i ].name ) );

   if( !ids_to_names.count( id ) )
   {
      if( no_throw )
         return string( );
      else
         throw runtime_error( "unknown field id '" + id + "' in get_field_name_for_id" );
   }

   return ids_to_names[ id ];
}

string get_field_id_for_name( size_t handle, const string& context, const string& name, bool no_throw )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   map< string, string > names_to_ids;

   field_info_container field_info;

   instance.get_field_info( field_info );

   for( size_t i = 0; i < field_info.size( ); i++ )
      names_to_ids.insert( make_pair( field_info[ i ].name, field_info[ i ].id ) );

   if( !names_to_ids.count( name ) )
   {
      if( no_throw )
         return string( );
      else
         throw runtime_error( "unknown field name '" + name + "' in get_field_id_for_name" );
   }

   return names_to_ids[ name ];
}

string get_field_type_name( size_t handle,
 const string& context, const string& id_or_name,
 bool* p_is_encrypted, bool* p_is_transient, bool* p_is_file_field )
{
   string type_name;

   class_base& instance( get_class_base_from_handle( handle, context ) );

   field_info_container field_info;

   instance.get_field_info( field_info );

   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      if( ( field_info[ i ].id == id_or_name ) || ( field_info[ i ].name == id_or_name ) )
      {
         type_name = field_info[ i ].type_name;

         if( p_is_encrypted )
            *p_is_encrypted = field_info[ i ].is_encrypted;

         if( p_is_transient )
            *p_is_transient = field_info[ i ].is_transient;

         if( p_is_file_field )
            *p_is_file_field = instance.is_file_field( id_or_name );

         break;
      }
   }

   if( type_name.empty( ) )
      throw runtime_error( "unknown field '" + id_or_name + "' in get_field_type_name " );

   return type_name;
}

string get_field_uom_symbol( size_t handle, const string& context, const string& id_or_name )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   string field( id_or_name );

   if( !instance_accessor.get_field_name( field ) )
   {
      const char* p_id = instance_accessor.get_field_id( field );

      if( p_id )
         field = string( p_id );
   }

   return instance.get_field_uom_symbol( field );
}

string get_field_display_name( size_t handle, const string& context, const string& id_or_name )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   string field( id_or_name );

   if( !instance_accessor.get_field_name( field ) )
   {
      const char* p_id = instance_accessor.get_field_id( field );

      if( p_id )
         field = string( p_id );
   }

   return instance.get_field_display_name( field );
}

void inline add_next_value( bool as_csv, const string& next_value, string& field_values )
{
   if( !as_csv )
      field_values += escaped( next_value, ",\"", c_esc, "rn\r\n" );
   else
   {
      string next_csv_value;

      if( next_value.find_first_of( ",\"\r\n" ) == string::npos )
         next_csv_value = next_value;
      else
         next_csv_value = "\"" + escaped( replaced( next_value, "\r\n", "\n" ), "\"", '"' ) + "\"";

      field_values += next_csv_value;
   }
}

string get_field_value( size_t handle, const string& context, const string& field )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.get_field_value( instance.get_field_num( field ) );
}

string get_field_values( size_t handle,
 const string& parent_context, const vector< string >& field_list,
 const string& tz_name, bool is_default, bool as_csv, vector< string >* p_raw_values,
 const multimap< size_t, string >* p_inserts, const map< string, string >* p_replace_map,
 const vector< string >* p_omit_matching )
{
   string field_values;

   string key_value( instance_key_info( handle, parent_context, true ) );

   string group_field_name, level_field_name, order_field_name, owner_field_name;

   instance_special_field_names( handle, parent_context, c_ignore_field,
    &group_field_name, &level_field_name, &order_field_name, &owner_field_name );

   map< string, string > special_field_names;

   special_field_names.insert( make_pair( c_group_field, group_field_name ) );
   special_field_names.insert( make_pair( c_level_field, level_field_name ) );
   special_field_names.insert( make_pair( c_order_field, order_field_name ) );
   special_field_names.insert( make_pair( c_owner_field, owner_field_name ) );

   if( p_omit_matching && ( p_omit_matching->size( ) != field_list.size( ) ) )
      throw runtime_error( "unexpected 'omit matching' vector size mismatch" );

   for( size_t i = 0; i < field_list.size( ); i++ )
   {
      string next_field( field_list[ i ] );

      size_t pos = next_field.find_last_of( "." );

      string field, field_context;

      if( pos != string::npos )
      {
         field = next_field.substr( pos + 1 );
         field_context = next_field.substr( 0, pos );
      }
      else
         field = next_field;

      string context( parent_context );

      if( !field_context.empty( ) )
      {
         if( !context.empty( ) )
            context += ".";

         context += field_context;
      }

      string next_value;

      size_t num_inserts = 0;

      if( p_inserts )
         num_inserts = p_inserts->count( i );

      for( size_t n = 0; n < num_inserts; n++ )
      {
         if( !field_values.empty( ) || ( ( i > 0 ) && !p_omit_matching ) )
            field_values += ',';

         multimap< size_t, string >::const_iterator ci = p_inserts->find( i );

         advance( ci, n );

         string insert_name( ci->second );

         bool is_special = false;

         if( special_field_names.count( insert_name ) )
         {
            is_special = true;
            insert_name = special_field_names[ insert_name ];
         }

         if( insert_name == c_key_field )
         {
            if( !p_replace_map || ( p_replace_map->find( key_value ) == p_replace_map->end( ) ) )
               add_next_value( as_csv, key_value, field_values );
            else
               add_next_value( as_csv, p_replace_map->find( key_value )->second, field_values );
         }
         else if( is_special && ( insert_name != c_ignore_field ) )
            add_next_value( as_csv,
             get_field_value( handle, parent_context, insert_name ), field_values );
         else if( !insert_name.empty( ) && ( insert_name[ 0 ] == '@' ) )
            add_next_value( as_csv,
             instance_get_variable( handle, parent_context, insert_name ), field_values );
         else
            add_next_value( as_csv, insert_name, field_values );
      }

      // NOTE: Use the parent special field names map to check if the field
      // name is special then obtain the actual name with the field context.
      if( special_field_names.count( field ) )
      {
         class_base& instance( get_class_base_from_handle( handle, context ) );

         if( field == c_group_field )
            field = instance.get_group_field_name( );
         else if( field == c_level_field )
            field = instance.get_level_field_name( );
         else if( field == c_order_field )
            field = instance.get_order_field_name( );
         else if( field == c_owner_field )
            field = instance.get_owner_field_name( );
      }

      if( field == c_key_field )
         next_value = key_value;
      else if( !field.empty( ) )
      {
         if( field[ 0 ] == '@' )
            next_value = instance_get_variable( handle, context, field );
         else if( field != c_ignore_field )
            next_value = execute_object_command( handle, context, "get " + field );
      }

      if( p_replace_map && !next_value.empty( ) )
      {
         for( map< string, string >::const_iterator ci = p_replace_map->begin( ); ci != p_replace_map->end( ); ++ci )
         {
            while( true )
            {
               string::size_type pos = next_value.find( ci->first );

               if( pos == string::npos )
                  break;

               // NOTE: Ignore replacements that are key "variations".
               if( ( ci->second.find( ci->first ) == 0 )
                && ( ci->second.length( ) == ( ci->first.length( ) + 2 ) )
                && ( ci->second[ ci->second.length( ) - 2 ] == '_' )
                && ( ci->second[ ci->second.length( ) - 1 ] >= 'a' )
                && ( ci->second[ ci->second.length( ) - 1 ] <= 'z' ) )
                  break;

               next_value.replace( pos, ci->first.length( ), ci->second );

               // NOTE: If the replacement string contains the key then must
               // continue to avoid infinite expansion of the output string.
               if( ci->second.find( ci->first ) != string::npos )
                  break;
            }
         }
      }

      if( p_raw_values )
         p_raw_values->push_back( next_value );

      if( !p_omit_matching || ( ( *p_omit_matching )[ i ] != next_value ) )
      {
         if( !field_values.empty( ) || ( ( i > 0 ) && !p_omit_matching ) )
            field_values += ',';

         if( !next_value.empty( ) && ( field != c_key_field ) )
         {
            bool is_encrypted = false;
            string type_name = get_field_type_name( handle, context, field, &is_encrypted );

            if( type_name == "date_time" || type_name == "tdatetime" )
            {
               date_time dt( next_value );

               if( tz_name.empty( ) )
                  next_value = 'U' + dt.as_string( );
               else
               {
                  // NOTE: For mandatory date_time's the "default" date_time (for default or
                  // "new" records) is not adjusted so it can be used in order to identify a
                  // "default" value (which may in a UI be displayed as blank instead).
                  if( !is_default || dt != date_time( ) )
                     next_value = utc_to_local( dt, tz_name ).as_string( );
               }
            }
         }

         if( p_omit_matching )
            field_values += field_list[ i ] + '=';

         add_next_value( as_csv, next_value, field_values );
      }
   }

   size_t num_inserts = 0;

   if( p_inserts )
      num_inserts = p_inserts->count( field_list.size( ) );

   for( size_t n = 0; n < num_inserts; n++ )
   {
      if( !field_values.empty( ) )
         field_values += ',';

      multimap< size_t, string >::const_iterator ci = p_inserts->find( field_list.size( ) );

      advance( ci, n );

      string insert_name( ci->second );

      bool is_special = false;

      if( special_field_names.count( insert_name ) )
      {
         is_special = true;
         insert_name = special_field_names[ insert_name ];
      }

      if( insert_name == c_key_field )
      {
         if( !p_replace_map || ( p_replace_map->find( key_value ) == p_replace_map->end( ) ) )
            add_next_value( as_csv, key_value, field_values );
         else
            add_next_value( as_csv, p_replace_map->find( key_value )->second, field_values );
      }
      else if( is_special && ( insert_name != c_ignore_field ) )
         add_next_value( as_csv,
          get_field_value( handle, parent_context, insert_name ), field_values );
      else if( !insert_name.empty( ) && ( insert_name[ 0 ] == '@' ) )
         add_next_value( as_csv,
          instance_get_variable( handle, parent_context, insert_name ), field_values );
      else
         add_next_value( as_csv, insert_name, field_values );
   }

   return field_values;
}

void set_any_field_ids_to_names( size_t handle, const string& context, vector< string >& fields )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   map< string, string > ids_to_names;

   field_info_container field_info;

   instance.get_field_info( field_info );

   for( size_t i = 0; i < field_info.size( ); i++ )
      ids_to_names.insert( make_pair( field_info[ i ].id, field_info[ i ].name ) );

   for( size_t i = 0; i < fields.size( ); i++ )
   {
      if( ids_to_names.count( fields[ i ] ) )
         fields[ i ] = ids_to_names[ fields[ i ] ];
   }
}

void set_any_field_names_to_ids( size_t handle, const string& context, vector< string >& fields )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   map< string, string > names_to_ids;

   field_info_container field_info;

   instance.get_field_info( field_info );

   for( size_t i = 0; i < field_info.size( ); i++ )
      names_to_ids.insert( make_pair( field_info[ i ].name, field_info[ i ].id ) );

   for( size_t i = 0; i < fields.size( ); i++ )
   {
      if( names_to_ids.count( fields[ i ] ) )
         fields[ i ] = names_to_ids[ fields[ i ] ];
   }
}

bool determine_alternative_key_fields( size_t handle, const string& context,
 const vector< string > available_fields, vector< string >& selected_fields )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   set< string > sorted_available_fields( available_fields.begin( ), available_fields.end( ) );

   vector< key_field_info_container > all_key_field_info;

   instance.get_alternative_key_field_info( all_key_field_info );

   vector< key_field_info_container > candidates_key_field_info;

   for( size_t i = 0; i < all_key_field_info.size( ); i++ )
   {
      bool is_candidate = true;

      for( size_t j = 0; j < all_key_field_info[ i ].size( ); j++ )
      {
         if( !sorted_available_fields.count( all_key_field_info[ i ][ j ].id )
          && !sorted_available_fields.count( all_key_field_info[ i ][ j ].name ) )
         {
            is_candidate = false;

            break;
         }
      }

      if( is_candidate )
         candidates_key_field_info.push_back( all_key_field_info[ i ] );
   }

   if( !candidates_key_field_info.empty( ) )
   {
      size_t best = 0;

      for( size_t i = 0; i < candidates_key_field_info.size( ); i++ )
      {
         if( candidates_key_field_info[ i ].size( ) < candidates_key_field_info[ best ].size( ) )
            best = i;
      }

      for( size_t j = 0; j < candidates_key_field_info[ best ].size( ); j++ )
      {
         if( sorted_available_fields.count( candidates_key_field_info[ best ][ j ].id ) )
            selected_fields.push_back( *sorted_available_fields.find( candidates_key_field_info[ best ][ j ].id ) );
         else if( sorted_available_fields.count( candidates_key_field_info[ best ][ j ].name ) )
            selected_fields.push_back( *sorted_available_fields.find( candidates_key_field_info[ best ][ j ].name ) );
      }

      return true;
   }

   return false;
}

void get_foreign_field_and_class_ids( size_t handle,
 const string& context, map< string, string >& foreign_field_and_class_ids )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   vector< field_data > all_field_data;

   get_all_field_data( handle, "", "", all_field_data );

   for( size_t i = 0; i < all_field_data.size( ); i++ )
   {
      if( !all_field_data[ i ].class_id.empty( ) )
         foreign_field_and_class_ids.insert( make_pair( all_field_data[ i ].id, all_field_data[ i ].class_id ) );
   }
}

void get_base_class_info( size_t handle,
 const string& context, vector< pair< string, string > >& base_class_info )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   class_base& dcb( *instance.get_dynamic_instance( ) );

   dcb.get_base_class_info( base_class_info );
}

bool has_locked_instance( const string& lock_class_id, const string& key )
{
   storage_handler& handler( *gtp_session->p_storage_handler );

   const op_lock& lock_info = handler.get_lock_info( lock_class_id, key );

   return ( lock_info.handle
    && ( lock_info.type >= op_lock::e_lock_type_review ) && ( lock_info.p_session == gtp_session ) );
}

bool is_child_constrained( class_base& instance, string& constraining_class )
{
   bool retval = false;

   map< string, set< string > > instance_keys;

   storage_handler& handler( *gtp_session->p_storage_handler );

   retval = is_child_constrained( instance, instance, constraining_class, instance_keys );

   if( retval )
      handler.release_locks_for_owner( instance, true );

   return retval;
}

size_t obtain_storage_lock( const string& type, const string& lock_class_id, const string& key, size_t num_attempts )
{
   op_lock::lock_type type_val = op_lock::lock_type_for_name( type );

   if( type_val == 0 )
      throw runtime_error( "invalid lock type '" + type + "'" );

   size_t lock_handle = obtain_keyed_lock( lock_class_id, key, type_val, num_attempts );

   if( !lock_handle )
      throw runtime_error( "unable to obtain lock due to locking conflict" );

   return lock_handle;
}

bool obtain_cascade_locks( class_base& instance )
{
   bool retval = true;

   map< string, set< string > > instance_keys;

   storage_handler& handler( *gtp_session->p_storage_handler );

   retval = obtain_cascade_locks_for_destroy( instance, instance, instance_keys );

   if( !retval )
      handler.release_locks_for_owner( instance, true );

   return retval;
}

size_t obtain_instance_lock( const string& type, class_base& instance, size_t num_attempts, string* p_key_value )
{
   size_t lock_handle;

   op_lock::lock_type type_val = op_lock::lock_type_for_name( type );

   if( type_val == 0 )
      throw runtime_error( "invalid lock type '" + type + "'" );

   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   storage_handler& handler( *gtp_session->p_storage_handler );

   string key_value( instance.get_key( ) );

   if( p_key_value )
      key_value = *p_key_value;

   if( !handler.obtain_lock( lock_handle, instance.get_lock_class_id( ),
    key_value, type_val, gtp_session, &instance, &instance, num_attempts ) )
      throw runtime_error( "unable to obtain lock due to locking conflict" );

   return lock_handle;
}

size_t obtain_instance_fk_lock( const string& lock_class_id, const string& key, bool review_required )
{
   if( lock_class_id.empty( ) || key.empty( ) )
      throw runtime_error( "lock_class_id and key must both have non-empty values" );

   size_t lock_handle = obtain_keyed_lock( lock_class_id,
    key, !review_required ? op_lock::e_lock_type_link : op_lock::e_lock_type_review );

   return lock_handle;
}

void release_obtained_lock( size_t lock_handle )
{
   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   storage_handler& handler( *gtp_session->p_storage_handler );

   handler.release_lock( lock_handle );
}

void release_instance_locks( class_base& instance, bool force_removal )
{
   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   storage_handler& handler( *gtp_session->p_storage_handler );

   handler.release_locks_for_owner( instance, force_removal );
}

void transform_obtained_lock( size_t lock_handle, const string& type, size_t num_attempts )
{
   op_lock::lock_type type_val = op_lock::lock_type_for_name( type );

   if( type_val == 0 )
      throw runtime_error( "invalid lock type '" + type + "'" );

   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   storage_handler& handler( *gtp_session->p_storage_handler );

   handler.transform_lock( lock_handle, type_val, gtp_session, num_attempts );
}

void dump_storage_cache( ostream& os )
{
   guard g( g_mutex );

   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   gtp_session->p_storage_handler->dump_cache( os );
}

void dump_storage_locks( ostream& os )
{
   guard g( g_mutex );

   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   gtp_session->p_storage_handler->dump_locks( os );
}

string resolve_class_id( const string& module,
 const string& id_or_name, const string& exception_context )
{
   string class_id( id_or_name );

   class_id = get_class_id_for_id_or_name( module, class_id );

   if( class_id == get_class_name_for_id_or_name( module, class_id ) )
      throw runtime_error( "unknown class '" + class_id + "' " + exception_context );

   return class_id;
}

string resolve_field_id( const string& module,
 const string& mclass, const string& id_or_name, const string& exception_context )
{
   string field_id( id_or_name );

   field_id = get_field_id_for_id_or_name( module, mclass, field_id );

   if( field_id == get_field_name_for_id_or_name( module, mclass, field_id ) )
      throw runtime_error( "unknown field '" + field_id + "' " + exception_context );

   return field_id;
}

void get_all_field_data( size_t handle, const string& context,
 const string& key, vector< field_data >& all_field_data, string* p_class_id,
 string* p_class_name, vector< pair< string, string > >* p_base_class_info )
{
   class_base& cb( get_class_base_from_handle_for_op( handle, context ) );

   if( !key.empty( ) )
      instance_review_begin( handle, context, key );

   class_base& dcb( *cb.get_dynamic_instance( ) );

   if( p_class_id )
      *p_class_id = string( dcb.get_class_id( ) );

   if( p_class_name )
      *p_class_name = string( dcb.get_class_name( ) );

   if( p_base_class_info )
      dcb.get_base_class_info( *p_base_class_info );

   foreign_key_info_container foreign_key_info;

   get_foreign_key_info_for_module_class( dcb.get_module_id( ), dcb.get_class_id( ), foreign_key_info );

   field_info_container field_info;

   dcb.get_field_info( field_info );

   // FUTURE: It would make things easier (and improve performance) if both the fk class id and
   // the transient indicator were a part of the "field_info" data structure itself.
   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      string class_id;

      if( foreign_key_info.count( field_info[ i ].id ) )
      {
         size_t offset = 0;

         if( foreign_key_info[ field_info[ i ].id ].second.find( dcb.get_module_name( ) ) == 0 )
            offset = dcb.get_module_name( ).length( ) + 1;

         class_id = get_class_id_for_id_or_name( dcb.get_module_id( ), foreign_key_info[ field_info[ i ].id ].second.substr( offset ) );
      }

      string field_value;

      if( !key.empty( ) )
         field_value = dcb.get_field_value( i );

      all_field_data.push_back( field_data( field_info[ i ].id, field_info[ i ].name,
       field_value, field_info[ i ].mandatory, dcb.is_field_transient( i ), class_id, field_info[ i ].type_name ) );
   }

   if( !key.empty( ) )
      instance_review_finish( handle, context );
}

class_base& get_class_base_from_handle( size_t handle, const string& context )
{
   object_instance_registry_container& instance_registry( gtp_session->instance_registry );

   object_instance_registry_iterator oiri = instance_registry.find( handle );

   if( oiri == instance_registry.end( ) )
      throw runtime_error( "invalid object instance handle #" + to_string( handle ) );

   class_base* p_class_base( ( oiri->second ).p_class_base );

   p_class_base = p_class_base->get_dynamic_instance( );

   if( !context.empty( ) )
      p_class_base = &p_class_base->get_or_create_graph_child( context );

   return *p_class_base;
}

class_base& get_class_base_from_handle_for_op( size_t handle,
 const string& context, permit_op_type_value permit, bool use_dynamic_context )
{
   object_instance_registry_container& instance_registry( gtp_session->instance_registry );
   object_instance_registry_iterator oiri = instance_registry.find( handle );

   if( oiri == instance_registry.end( ) )
      throw runtime_error( "invalid object instance handle #" + to_string( handle ) );

   class_base* p_class_base( ( oiri->second ).p_class_base );

   if( !context.empty( ) )
      p_class_base = &p_class_base->get_or_create_graph_child( context );

   // NOTE: If not wanting to use a dynamic instance then provided it isn't already in use for an
   // operation then discard it (if one exists). This could be used to support "dynamic" instance
   // iteration, however, unless a second SELECT can be issued to get the derived instance's data
   // there is probably no real benefit to supporting it (but perhaps some other future use might
   // benefit from this behaviour).
   class_base* p_dynamic_base = p_class_base->get_dynamic_instance( );

   if( !use_dynamic_context )
   {
      if( ( p_class_base != p_dynamic_base )
       && ( p_dynamic_base->get_op( ) == class_base::e_op_type_none ) )
         class_base_accessor( *p_class_base ).destroy_dynamic_instance( );
      else
         p_class_base = p_dynamic_base;
   }
   else
      p_class_base = p_dynamic_base;

   class_base::op_type op( p_class_base->get_op( ) );

   if( ( permit == e_permit_op_type_value_none ) && ( op != class_base::e_op_type_none ) )
      throw runtime_error( "object instance #" + to_string( handle ) + " is currently involved in another operation" );

   if( ( permit != e_permit_op_type_value_none ) && ( op == class_base::e_op_type_none ) )
      throw runtime_error( "object instance #" + to_string( handle ) + " is not currently involved in an operation" );

   if( ( permit == e_permit_op_type_value_review ) && ( op != class_base::e_op_type_review ) )
      throw runtime_error( "object instance #" + to_string( handle ) + " is currently involved in another operation" );

   if( ( permit == e_permit_op_type_value_create_update_destroy )
    && ( op != class_base::e_op_type_create ) && ( op != class_base::e_op_type_update ) && ( op != class_base::e_op_type_destroy ) )
      throw runtime_error( "object instance #" + to_string( handle ) + " is currently involved in another operation" );

   return *p_class_base;
}

bool fetch_instance_from_db( class_base& instance,
 const string& sql, bool sys_only_fields, bool is_minimal_fetch, bool allow_caching )
{
   bool found = false;
   class_base_accessor instance_accessor( instance );

   if( sys_only_fields && allow_caching )
      found = fetch_instance_from_cache( instance, instance_accessor.get_lazy_fetch_key( ), true );

   if( !found && gtp_session && gtp_session->up_db.get( ) )
   {
      TRACE_LOG( TRACE_DETAILS | TRACE_QUERIES, sql );

      sql_dataset ds( *gtp_session->up_db.get( ), sql );

      found = ds.next( );

      ++gtp_session->sql_count;

      if( !sys_only_fields )
         instance_accessor.clear( );

      if( found )
      {
         instance_accessor.set_key( ds.as_string( 0 ), true );

         if( ds.get_fieldcount( ) > 1 )
         {
            instance_accessor.set_version( from_string< uint16_t >( ds.as_string( 1 ) ) );
            instance_accessor.set_revision( from_string< uint64_t >( ds.as_string( 2 ) ) );

            instance_accessor.set_security( from_string< uint64_t >( ds.as_string( 3 ) ) );

            instance_accessor.set_original_identity( ds.as_string( 4 ) );

            instance_accessor.set_original_revision( instance.get_revision( ) );

            if( !sys_only_fields )
            {
               TRACE_LOG( TRACE_VERBOSE | TRACE_QUERIES, "(from temporary dataset)" );

               int fnum = c_num_sys_field_names;

               for( int i = fnum; i < ds.get_fieldcount( ); i++, fnum++ )
               {
                  while( instance.is_field_transient( fnum - c_num_sys_field_names ) )
                     fnum++;

                  TRACE_LOG( TRACE_VERBOSE | TRACE_QUERIES,
                   "setting field #" + to_string( fnum - c_num_sys_field_names + 1 ) + " to " + ds.as_string( i ) );

                  instance.set_field_value( fnum - c_num_sys_field_names, ds.as_string( i ) );
               }

               if( allow_caching && !is_minimal_fetch )
               {
                  guard g( g_mutex, "fetch_instance_from_db" );

                  storage_handler& handler( *gtp_session->p_storage_handler );

                  if( handler.get_root( ).cache_limit )
                  {
                     string key_info( instance.get_class_id( ) + ":" + ds.as_string( 0 ) );

                     vector< string > columns;

                     for( size_t i = 0; i < ds.get_fieldcount( ); i++ )
                        columns.push_back( ds.as_string( i ) );

                     if( handler.get_record_cache( ).count( key_info ) )
                     {
                        time_info_iterator tii
                         = handler.get_key_for_time( ).lower_bound(
                         handler.get_time_for_key( ).find( key_info )->second );

                        while( tii->second != key_info )
                           ++tii;

                        handler.get_key_for_time( ).erase( tii );
                        handler.get_time_for_key( ).erase( key_info );

                        handler.get_record_cache( ).erase( key_info );
                     }

                     if( handler.get_record_cache( ).size( )
                      && handler.get_record_cache( ).size( ) >= handler.get_root( ).cache_limit )
                     {
                        string oldest_key_info = handler.get_key_for_time( ).begin( )->second;

                        handler.get_record_cache( ).erase( oldest_key_info );
                        handler.get_time_for_key( ).erase( oldest_key_info );

                        handler.get_key_for_time( ).erase( handler.get_key_for_time( ).begin( ) );
                     }

                     time_t tm = time( 0 );

                     handler.get_key_for_time( ).insert( make_pair( tm, key_info ) );
                     handler.get_time_for_key( ).insert( make_pair( key_info, tm ) );

                     handler.get_record_cache( ).insert( make_pair( key_info, columns ) );

                     if( ( handler.get_key_for_time( ).size( ) != handler.get_time_for_key( ).size( ) )
                      || ( handler.get_key_for_time( ).size( ) != handler.get_record_cache( ).size( ) ) )
                        throw runtime_error( "*** record cache maps size mismatch ***" );
                  }
               }
            }

            instance_accessor.after_fetch_from_db( );

            string skip_after_fetch_var(
             instance.get_raw_variable( get_special_var_name( e_special_var_skip_after_fetch ) ) );

            if( ( skip_after_fetch_var == c_true ) || ( skip_after_fetch_var == c_true_value ) )
               ; // i.e. do nothing
            else
               instance_accessor.perform_after_fetch( is_minimal_fetch );
         }
      }
   }

   return found;
}

bool fetch_instance_from_cache( class_base& instance, const string& key, bool sys_only_fields )
{
   bool found = false;

   class_base_accessor instance_accessor( instance );

   string key_info( instance.get_class_id( ) + ":" + key );

   storage_handler& handler( *gtp_session->p_storage_handler );

   // NOTE: Empty code block for scope purposes.
   {
      guard g( g_mutex, "fetch_instance_from_cache" );

      found = handler.get_record_cache( ).count( key_info );

      if( found )
      {
         time_info_iterator tii = handler.get_key_for_time( ).lower_bound( handler.get_time_for_key( ).find( key_info )->second );

         while( tii->second != key_info )
            ++tii;

         time_t tm = time( 0 );

         // NOTE: Update the time information for the key
         // (and replaces the matching reversed entry) if
         // the current time is not identical.
         if( tm != tii->first )
         {
            handler.get_time_for_key( )[ key_info ] = tm;

            handler.get_key_for_time( ).erase( tii );
            handler.get_key_for_time( ).insert( make_pair( tm, key_info ) );
         }

         ++gtp_session->cache_count;

         if( !sys_only_fields )
            instance_accessor.clear( );

         TRACE_LOG( TRACE_DETAILS | TRACE_QUERIES, "*** fetching '" + key_info + "' from cache ***" );

         vector< string >& columns( handler.get_record_cache( )[ key_info ] );

         instance_accessor.set_key( columns[ 0 ], true );

         instance_accessor.set_version( from_string< uint16_t >( columns[ 1 ] ) );
         instance_accessor.set_revision( from_string< uint64_t >( columns[ 2 ] ) );

         instance_accessor.set_security( from_string< uint64_t >( columns[ 3 ] ) );

         instance_accessor.set_original_identity( columns[ 4 ] );

         instance_accessor.set_original_revision( instance.get_revision( ) );

         if( !sys_only_fields )
         {
            TRACE_LOG( TRACE_VERBOSE | TRACE_QUERIES, "(from cache)" );

            int fnum = c_num_sys_field_names;

            for( int i = fnum; i < columns.size( ); i++, fnum++ )
            {
               while( instance.is_field_transient( fnum - c_num_sys_field_names ) )
                  fnum++;

               TRACE_LOG( TRACE_VERBOSE | TRACE_QUERIES,
                "setting field #" + to_string( fnum - c_num_sys_field_names + 1 ) + " to " + columns[ i ] );

               instance.set_field_value( fnum - c_num_sys_field_names, columns[ i ] );
            }

            instance_accessor.after_fetch_from_db( );
         }
      }
   }

   if( found && !sys_only_fields )
   {
      string skip_after_fetch_var(
       instance.get_raw_variable( get_special_var_name( e_special_var_skip_after_fetch ) ) );

      if( skip_after_fetch_var == c_true || skip_after_fetch_var == c_true_value )
         ; // i.e. do nothing
      else
         instance_accessor.perform_after_fetch( );
   }

   return found;
}

void get_instance_sql_stmts( class_base& instance,
 vector< string >& sql_stmts, vector< string >* p_sql_undo_stmts )
{
   class_base_accessor instance_accessor( instance );

   if( !instance_accessor.get_sql_stmts( sql_stmts, gtp_session->tx_key_info, p_sql_undo_stmts ) )
      throw runtime_error( "unexpected get_sql_stmts failure" );
}

void append_undo_sql_stmts( const std::vector< std::string >& sql_undo_stmts )
{
   gtp_session->sql_undo_statements.insert(
    gtp_session->sql_undo_statements.end( ), sql_undo_stmts.begin( ), sql_undo_stmts.end( ) );
}

string instance_class( size_t handle, const string& context )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.get_display_name( );
}

string instance_key_info( size_t handle, const string& context, bool key_only )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   string retval( instance.get_key( ) );

   if( !key_only )
   {
      uint64_t state = instance.get_state( );

      // NOTE: In order to prevent potential issues during a "generate" in Meta
      // a "system variable" is being used to prevent editing/deleting records.
      if( gtp_session->p_storage_handler->get_name( ) == c_meta_storage_name
       && !get_raw_system_variable( "@Meta_protect" ).empty( ) )
      {
         state |= c_state_uneditable;
         state |= c_state_undeletable;

         state &= ~c_state_ignore_uneditable;
      }

      retval += " =" + instance.get_version_info( )
       + " " + to_string( state ) + " " + instance.get_original_identity( );
   }

   return retval;
}

string instance_get_variable( size_t handle, const string& context, const string& vname )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.get_variable( vname );
}

void instance_set_variable( size_t handle, const string& context, const string& vname, const string& value )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   instance.set_variable( vname, value );
}

void instance_special_field_names(
 size_t handle, const string& context, const string& empty,
 string* p_group_name, string* p_level_name, string* p_order_name, string* p_owner_name )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   if( p_group_name )
      *p_group_name = empty_or_value( empty, instance.get_group_field_name( ) );

   if( p_level_name )
      *p_level_name = empty_or_value( empty, instance.get_level_field_name( ) );

   if( p_order_name )
      *p_order_name = empty_or_value( empty, instance.get_order_field_name( ) );

   if( p_owner_name )
      *p_owner_name = empty_or_value( empty, instance.get_owner_field_name( ) );
}

string instance_group_field_name( size_t handle, const string& context )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.get_group_field_name( );
}

string instance_level_field_name( size_t handle, const string& context )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.get_level_field_name( );
}

string instance_order_field_name( size_t handle, const string& context )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.get_order_field_name( );
}

string instance_owner_field_name( size_t handle, const string& context )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.get_owner_field_name( );
}

void instance_check( class_base& instance, instance_check_rc* p_rc )
{
   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   class_base_accessor instance_accessor( instance );

   bool found = perform_instance_check( instance, instance_accessor.get_lazy_fetch_key( ) );

   if( !found )
   {
      if( p_rc )
         *p_rc = e_instance_check_rc_not_found;
      else
         throw runtime_error( get_string_message( GS( c_str_record_not_found ),
          make_pair( c_str_record_not_found_class, instance.get_display_name( ) ),
          make_pair( c_str_record_not_found_key, instance_accessor.get_lazy_fetch_key( ) ) ) );
   }
}

void instance_tx_check( class_base& instance )
{
   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   class_base_accessor instance_accessor( instance );

   storage_handler& handler( *gtp_session->p_storage_handler );

   int64_t transaction_level = ods::instance( )->get_transaction_level( );

   if( instance_accessor.get_lock_handle( ) && ( !handler.has_lock_info( instance_accessor.get_lock_handle( ) )
    || ( handler.get_lock_info( instance_accessor.get_lock_handle( ) ).transaction_level != transaction_level ) ) )
      throw runtime_error( "attempt to perform apply for operation commenced outside the current transaction scope" );
}

void update_session_locks_for_transaction( )
{
   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   gtp_session->p_storage_handler->update_session_locks_for_transaction( gtp_session );
}

bool is_change_locked( class_base& instance, bool include_cascades )
{
   op_lock lock = gtp_session->p_storage_handler->get_lock_info( instance.get_lock_class_id( ), instance.get_key( ) );

   return ( lock.type >= op_lock::e_lock_type_update ) && ( include_cascades || !lock.p_root_class );
}

bool is_destroy_locked( class_base& instance, bool include_cascades )
{
   op_lock lock = gtp_session->p_storage_handler->get_lock_info( instance.get_lock_class_id( ), instance.get_key( ) );

   return ( lock.type == op_lock::e_lock_type_destroy ) && ( include_cascades || !lock.p_root_class );
}

bool is_create_locked_by_own_session(
 class_base& instance, const char* p_key, bool copy_field_values, bool also_check_tx_lock_type )
{
   op_lock lock;
   bool rc = false;

   lock = gtp_session->p_storage_handler->get_lock_info(
    instance.get_lock_class_id( ), p_key ? string( p_key ) : instance.get_key( ) );

   bool is_create_locked = ( lock.type == op_lock::e_lock_type_create );

   if( !is_create_locked && also_check_tx_lock_type )
      is_create_locked = ( lock.tx_type == op_lock::e_lock_type_create );

   if( is_create_locked && ( lock.p_session == gtp_session ) )
   {
      rc = true;

      if( copy_field_values )
      {
         if( !lock.p_class_base )
            rc = false;
         else
            instance.copy_all_field_values( *lock.p_class_base );
      }
   }

   return rc;
}

bool was_create_locked_by_own_session( class_base& instance, const char* p_key, bool copy_field_values )
{
   return is_create_locked_by_own_session( instance, p_key, copy_field_values, true );
}

bool is_update_locked_by_own_session( class_base& instance, const char* p_key )
{
   op_lock lock;
   bool rc = false;

   lock = gtp_session->p_storage_handler->get_lock_info(
    instance.get_lock_class_id( ), p_key ? string( p_key ) : instance.get_key( ) );

   return ( lock.p_session == gtp_session ) && ( lock.type == op_lock::e_lock_type_update );
}

bool is_destroy_locked_by_own_session( class_base& instance, const char* p_key )
{
   op_lock lock;
   bool rc = false;

   lock = gtp_session->p_storage_handler->get_lock_info(
    instance.get_lock_class_id( ), p_key ? string( p_key ) : instance.get_key( ) );

   return ( lock.p_session == gtp_session )
    && ( ( lock.type == op_lock::e_lock_type_destroy ) || ( lock.tx_type == op_lock::e_lock_type_destroy ) );
}

bool is_update_or_destroy_locked_by_own_session( class_base& instance, const char* p_key )
{
   op_lock lock;
   bool rc = false;

   lock = gtp_session->p_storage_handler->get_lock_info(
    instance.get_lock_class_id( ), p_key ? string( p_key ) : instance.get_key( ) );

   return ( lock.p_session == gtp_session ) && ( ( lock.type == op_lock::e_lock_type_update )
    || ( lock.type == op_lock::e_lock_type_destroy ) || ( lock.tx_type == op_lock::e_lock_type_destroy ) );
}

void instance_fetch( size_t handle, const string& context, const string& key_info, instance_fetch_rc* p_rc )
{
   perform_instance_fetch( get_class_base_from_handle_for_op( handle, context ), key_info, p_rc );
}

void instance_set_parent( size_t handle, const string& context, const string& new_key )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   instance_accessor.set_key( new_key );
}

void instance_prepare_execute( size_t handle,
 const string& context, const string& key, const string& ver_info, bool skip_after_fetch )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   instance_accessor.set_is_executing( true );

   if( key.empty( ) )
      instance_accessor.clear( );
   else
   {
      unique_ptr< temporary_object_variable > up_tmp_skip_after_fetch;

      if( skip_after_fetch )
         up_tmp_skip_after_fetch.reset( new temporary_object_variable(
          instance, get_special_var_name( e_special_var_skip_after_fetch ), c_true_value ) );

      instance.perform_fetch( key );

      instance_accessor.set_ver_exp( ver_info );
   }
}

string instance_execute( size_t handle,
 const string& context, const string& key, const string& method_name_and_args )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   TRACE_LOG( TRACE_INITIAL | TRACE_OBJECTS,
    "instance_execute( ) [class: " + instance.get_class_name( ) + "] " + method_name_and_args );

   return instance.execute( method_name_and_args );
}

void instance_get_field_values( size_t handle, const string& context, vector< string >& field_values )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   size_t num_fields = instance.get_num_fields( );

   for( size_t i = 0; i < num_fields; i++ )
      field_values.push_back( instance.get_field_value( i ) );
}

string instance_get_fields_and_values( size_t handle,
 const string& context, const vector< string >* p_initial_field_values )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   bool using_verbose_logging = get_storage_using_verbose_logging( );

   class_base::field_label_type label_type = ( using_verbose_logging
    ? class_base::e_field_label_type_full_id : class_base::e_field_label_type_short_id );

   return instance.get_fields_and_values( label_type, class_base::e_field_include_type_modified, p_initial_field_values );
}

bool instance_has_changed( size_t handle, const string& context )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.has_changed( );
}

bool instance_persistence_uses_log( size_t handle )
{
   class_base& instance( get_class_base_from_handle( handle, "" ) );

   return ( instance.get_persistence_type( ) <= 2 ); // i.e. SQL or ODS persistence
}

bool instance_iterate( size_t handle, const string& context,
 const string& key_info, const string& fields, const string& text,
 const string& query, iter_direction direction, bool inclusive,
 int row_limit, sql_optimisation optimisation, const set< string >* p_filters )
{
   return perform_instance_iterate(
    get_class_base_from_handle_for_op( handle, context, e_permit_op_type_value_none, false ),
    key_info, fields, text, query, direction, inclusive, row_limit, optimisation, p_filters );
}

bool instance_iterate_next( size_t handle, const string& context )
{
   return perform_instance_iterate_next(
    get_class_base_from_handle_for_op( handle, context, e_permit_op_type_value_none, false ) );
}

void instance_iterate_stop( size_t handle, const string& context )
{
   get_class_base_from_handle_for_op( handle, context, e_permit_op_type_value_none, false ).iterate_stop( );
}

void instance_review_begin( size_t handle, const string& context, const string& key )
{
   get_class_base_from_handle_for_op( handle, context ).begin_review( key );
}

void instance_review_finish( size_t handle, const string& context )
{
   get_class_base_from_handle_for_op( handle, context, e_permit_op_type_value_review ).finish_review( );
}

bool instance_filtered( size_t handle, const string& context )
{
   return get_class_base_from_handle( handle, context ).filtered( );
}

bool instance_uid_filtered( size_t handle, const string& context )
{
   bool retval = false;

   class_base& instance( get_class_base_from_handle( handle, context ) );

   if( !get_session_variable( get_special_var_name( e_special_var_uid ) ).empty( )
    && ( instance.get_key( ) == c_admin ) && ( instance.get_class_type( ) == 1 ) ) // i.e. user
      retval = true;

   return retval;
}

bool instance_has_transient_filter_fields( size_t handle, const string& context )
{
   return get_class_base_from_handle( handle, context ).has_transient_filter_fields( );
}

void op_instance_create( size_t handle,
 const string& context, const string& key, bool internal_operation, op_create_rc* p_rc )
{
   get_class_base_from_handle_for_op( handle, context ).op_create( key, p_rc, internal_operation );
}

void op_instance_update( size_t handle,
 const string& context, const string& key, const string& ver_info, bool internal_operation, op_update_rc* p_rc )
{
   class_base& instance( get_class_base_from_handle_for_op( handle, context ) );

   class_base_accessor( instance ).set_ver_exp( ver_info );

   instance.op_update( key, p_rc, internal_operation );
}

void op_instance_destroy( size_t handle,
 const string& context, const string& key, const string& ver_info, bool internal_operation, op_destroy_rc* p_rc )
{
   class_base& instance( get_class_base_from_handle_for_op( handle, context ) );

   class_base_accessor( instance ).set_ver_exp( ver_info );

   instance.op_destroy( key, p_rc, internal_operation );
}

void op_instance_apply( size_t handle, const string& context,
 bool internal_operation, op_apply_rc* p_rc, set< string >* p_fields_set )
{
   class_base& instance(
    get_class_base_from_handle_for_op( handle, context, e_permit_op_type_value_create_update_destroy ) );

   op_apply_rc rc;

   instance.op_apply( &rc, internal_operation, p_fields_set );

   if( p_rc )
      *p_rc = rc;
   else
   {
      if( rc == e_op_apply_rc_invalid )
         throw runtime_error( instance.get_validation_errors( class_base::e_validation_errors_type_first_only ) );
      else if( rc != e_op_apply_rc_okay )
         throw runtime_error( "unexpected rc value " + to_string( rc ) + " from op_apply" );
   }
}

void op_instance_cancel( size_t handle, const string& context, bool internal_operation )
{
   get_class_base_from_handle_for_op( handle, context, e_permit_op_type_value_any ).op_cancel( internal_operation );
}

void output_progress_message( const string& message )
{
   if( gtp_session )
      gtp_session->cmd_handler.output_progress( message );
}

bool is_in_transaction( )
{
   return !gtp_session->transactions.empty( );
}

void transaction_start( )
{
   if( !ods::instance( ) )
      throw runtime_error( "no storage is currently attached" );

   if( gtp_session->up_db.get( ) && gtp_session->transactions.empty( ) )
   {
      TRACE_LOG( TRACE_DETAILS | TRACE_QUERIES, "BEGIN" );

      exec_sql( *gtp_session->up_db, "BEGIN" );
   }

   gtp_session->transactions.push( new ods::transaction( *ods::instance( ) ) );
}

void transaction_commit( )
{
   if( gtp_session->transactions.empty( ) )
      throw runtime_error( "no active transaction exists" );

   storage_handler& handler( *gtp_session->p_storage_handler );

   bool supports_sql_undo = handler.supports_sql_undo( );

   // NOTE: Scope for guard object.
   {
      guard g( g_mutex, "transaction_commit" );

      gtp_session->transactions.top( )->commit( );

      delete gtp_session->transactions.top( );

      gtp_session->transactions.pop( );

      if( !gtp_session->transactions.size( ) )
      {
         if( gtp_session->p_tx_helper )
            gtp_session->p_tx_helper->at_commit( );

         if( supports_sql_undo && !is_init_uid( ) )
            append_undo_sql_statements( handler );

         if( handler.get_root( ).type == e_storage_type_peerchain )
            append_peerchain_log_commands( );

         if( handler.get_root( ).group_identity_old != handler.get_root( ).group_identity_new )
         {
            ods_file_system ofs( *ods::instance( ) );

            ofs.store_as_text_file( c_storable_file_name_num_gid, handler.get_root( ).group_identity_new );

            handler.get_root( ).group_identity_old = handler.get_root( ).group_identity_new;
         }

         append_transaction_log_command( handler );

         set_session_variable( get_special_var_name( e_special_var_skip_persistence ), "" );

         if( gtp_session->up_db.get( ) )
         {
            TRACE_LOG( TRACE_DETAILS | TRACE_QUERIES, "COMMIT" );

            exec_sql( *gtp_session->up_db, "COMMIT" );
         }

         if( gtp_session->p_tx_helper )
         {
            gtp_session->p_tx_helper->after_commit( );
            gtp_session->p_tx_helper = 0;
         }

         remove_tx_info_from_cache( );

         gtp_session->tx_key_info.clear( );
         gtp_session->sql_undo_statements.clear( );

         handler.release_locks_for_commit( gtp_session );
      }
   }

   if( gtp_session->transactions.empty( ) )
   {
      for( size_t i = 0; i < gtp_session->async_or_delayed_system_commands.size( ); i++ )
      {
         TRACE_LOG( TRACE_INITIAL | TRACE_SESSION, gtp_session->async_or_delayed_system_commands[ i ] );

         int rc = system( gtp_session->async_or_delayed_system_commands[ i ].c_str( ) );
         ( void )rc;
      }

      string sleep_after_var_name( get_special_var_name( e_special_var_sleep_after ) );

      // NOTE: The "@sleep_after" session variable is set by "run_script" to ensure that
      // an msleep will occur only after script "system" command has occurred (otherwise
      // the msleep would actually occur prior to execution).
      if( has_session_variable( sleep_after_var_name ) )
      {
         msleep( from_string< unsigned long >( get_raw_session_variable( sleep_after_var_name ) ) );

         set_session_variable( sleep_after_var_name, "" );
      }

      // NOTE: If the "args_file" session variable exists and a system variable with a name matching
      // its value is found then it is expected that the first (if any) error reported for a command
      // in the executed script will have replaced its value (initially set to "c_true_value"). When
      // such an error message is found it will be thrown as an exception from here (even though the
      // transaction commit has completed and the command for this session has already been logged).
      string script_error;

      string check_script_error(
       get_raw_session_variable( get_special_var_name( e_special_var_check_script_error ) ) );

      for( size_t i = 0; i < gtp_session->async_or_delayed_args_files.size( ); i++ )
      {
         string next( gtp_session->async_or_delayed_args_files[ i ] );

         if( script_error.empty( )
          && ( check_script_error == c_true || check_script_error == c_true_value ) )
         {
            string value( get_raw_system_variable( next ) );

            if( !value.empty( ) && value != c_true_value )
               script_error = value;
         }

         set_system_variable( next, "" );
      }

      gtp_session->async_or_delayed_args_files.clear( );
      gtp_session->async_or_delayed_system_commands.clear( );

      set_session_variable( get_special_var_name( e_special_var_check_script_error ), "" );

      if( !script_error.empty( ) )
      {
         // NOTE: If the error starts with '@' then assume that it is actually
         // intended to be an execute "return" message rather than an error.
         if( script_error[ 0 ] == '@' )
            set_session_variable( get_special_var_name( e_special_var_return ), script_error.substr( 1 ) );
         else
            throw runtime_error( script_error );
      }
   }
}

void transaction_rollback( )
{
   if( !gtp_session->transactions.empty( ) )
   {
      gtp_session->transactions.top( )->rollback( );

      delete gtp_session->transactions.top( );

      gtp_session->transactions.pop( );

      if( gtp_session->up_db.get( ) && gtp_session->transactions.empty( ) )
      {
         TRACE_LOG( TRACE_DETAILS | TRACE_QUERIES, "ROLLBACK" );

         exec_sql( *gtp_session->up_db, "ROLLBACK" );

         gtp_session->transaction_log_command.erase( );
      }

      if( gtp_session->transactions.empty( ) )
      {
         if( gtp_session->p_tx_helper )
         {
            gtp_session->p_tx_helper->after_rollback( );
            gtp_session->p_tx_helper = 0;
         }

         gtp_session->tx_key_info.clear( );
         gtp_session->sql_undo_statements.clear( );

         gtp_session->p_storage_handler->release_locks_for_rollback( gtp_session );

         for( size_t i = 0; i < gtp_session->async_or_delayed_args_files.size( ); i++ )
         {
            string next( gtp_session->async_or_delayed_args_files[ i ] );

            if( file_exists( next ) )
               file_remove( next );

            set_system_variable( next, "" );
         }

         gtp_session->async_or_delayed_args_files.clear( );
         gtp_session->async_or_delayed_system_commands.clear( );

         set_session_variable( get_special_var_name( e_special_var_skip_persistence ), "" );
         set_session_variable( get_special_var_name( e_special_var_check_script_error ), "" );
      }
   }
}

size_t transaction_id( )
{
   return ods::instance( )->get_transaction_id( );
}

size_t transaction_level( )
{
   return gtp_session->transactions.size( );
}

size_t set_transaction_id( size_t tx_id )
{
   guard g( g_mutex );

   storage_handler& handler( *gtp_session->p_storage_handler );

   log_identity& identity( handler.get_root( ).log_id );

   identity.next_id = tx_id;

   if( identity.next_id >= identity.ceiling )
   {
      while( identity.next_id >= identity.ceiling )
         identity.ceiling += c_identity_burn;

      // NOTE: Store the "ceiling" rather than "next_id" to avoid having to
      // perform extra I/O for every transaction. If storage termination is
      // to occur normally then the actual "next_id" will be written and no
      // identity value is lost. If for some reason normal termination does
      // not occur then up to the "burn" number of identities will be lost.
      restorable< int32_t > tmp_identity( identity.next_id, identity.ceiling );

      ods_file_system ofs( *ods::instance( ) );

      ofs.store_as_text_file( c_storable_file_name_log_id, identity.next_id );
   }

   return identity.next_id;
}

size_t next_transaction_id( )
{
   guard g( g_mutex );

   storage_handler& handler( *gtp_session->p_storage_handler );

   log_identity& identity( handler.get_root( ).log_id );

   return identity.next_id;
}

string transaction_log_command( )
{
   return gtp_session->transaction_log_command;
}

void transaction_log_command( const string& log_command,
 transaction_commit_helper* p_tx_helper, bool replace_current )
{
   if( log_command.empty( ) )
   {
      gtp_session->transaction_log_command.erase( );

      if( gtp_session->p_tx_helper )
         gtp_session->p_tx_helper = 0;
   }
   else
   {
      if( !replace_current )
         append_transaction_log_command( log_command );
      else
         gtp_session->transaction_log_command = log_command;

      if( p_tx_helper )
         gtp_session->p_tx_helper = p_tx_helper;
   }
}

void append_peerchain_log_command( const string& identity, const string& log_command )
{
   if( !log_command.empty( ) )
      gtp_session->peerchain_log_commands.push_back( identity + '=' + log_command );
}

void append_transaction_log_command( const string& log_command )
{
   if( !log_command.empty( ) )
   {
      if( !gtp_session->transaction_log_command.empty( ) )
         gtp_session->transaction_log_command += '\n';

      gtp_session->transaction_log_command += log_command;
   }
}

transaction::transaction( bool is_not_dummy )
 :
 is_dummy( !is_not_dummy ),
 can_commit( true )
{
   if( !is_dummy )
      transaction_start( );
}

transaction::~transaction( )
{
   if( !is_dummy && can_commit )
      transaction_rollback( );
}

void transaction::commit( )
{
   if( !is_dummy )
   {
      if( !can_commit )
         throw runtime_error( "cannot commit already completed transaction" );

      can_commit = false;
      transaction_commit( );
   }
}

void transaction::rollback( )
{
   if( !is_dummy )
   {
      if( !can_commit )
         throw runtime_error( "cannot rollback already completed transaction" );

      can_commit = false;
      transaction_rollback( );
   }
}

record_initialiser::record_initialiser( ofstream& log_file )
{
   storage_handler& handler( *gtp_session->p_storage_handler );

   handler.set_alternative_log_file( &log_file );
}

record_initialiser::~record_initialiser( )
{
   storage_handler& handler( *gtp_session->p_storage_handler );

   handler.set_alternative_log_file( 0 );
}
