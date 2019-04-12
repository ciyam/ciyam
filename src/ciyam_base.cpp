// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2019 CIYAM Developers
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
#endif

#define CIYAM_BASE_IMPL

#include "ciyam_base.h"

#include "md5.h"
#include "ods.h"
#include "sio.h"
#include "cube.h"
#include "salt.h"
#include "sha1.h"
#include "array.h"
#include "base64.h"
#include "config.h"
#include "format.h"
#include "sha256.h"
#include "sql_db.h"
#include "pointers.h"
#include "date_time.h"
#include "utilities.h"
#include "class_base.h"
#include "ciyam_files.h"
#ifdef SSL_SUPPORT
#  include "ssl_socket.h"
#  include "crypto_keys.h"
#endif
#include "fs_iterator.h"
#include "oid_pointer.h"
#include "crypt_stream.h"
#include "ciyam_strings.h"
#include "ciyam_session.h"
#include "ciyam_variables.h"
#include "command_handler.h"
#include "dynamic_library.h"
#include "ods_file_system.h"
#include "module_interface.h"
#include "module_management.h"
#include "read_write_stream.h"

using namespace std;

namespace
{

#include "ciyam_constants.h"

const string c_nul_key( 1, '\0' );

const char c_module_prefix_separator = '_';
const char c_module_order_prefix_separator = '.';

const char c_global_storage_file_not_folder_suffix = '!';

const int c_identity_burn = 100;

const unsigned int c_default_max_peers = 100;

// NOTE: This limit is supplied (along with the identity information) to
// client interfaces and is not the max # of concurrent server sessions.
const unsigned int c_default_max_user_limit = 1000;

const size_t c_default_cache_limit = 1000;

const size_t c_iteration_row_cache_limit = 100;

const int c_max_lock_attempts = 20;
const int c_lock_attempt_sleep_time = 200;

const int c_loop_variable_digits = 8;

const int c_storable_file_pad_len = 32;

// NOTE: Limit the buffer to twice the maximum file size (if a compression
// call returns buffer too small then the file can be stored uncompressed).
const int c_max_file_buffer_expansion = 2;

const char* const c_ciyam_server = "ciyam_server";

const char* const c_server_sid_file = "ciyam_server.sid";
const char* const c_server_config_file = "ciyam_server.sio";

const char* const c_section_mbox = "mbox";
const char* const c_section_pop3 = "pop3";
const char* const c_section_smtp = "smtp";
const char* const c_section_email = "email";
const char* const c_section_script = "script";

const char* const c_attribute_name = "name";
const char* const c_attribute_path = "path";
const char* const c_attribute_domain = "domain";
const char* const c_attribute_server = "server";
const char* const c_attribute_sender = "sender";
const char* const c_attribute_suffix = "suffix";
const char* const c_attribute_reg_key = "license";
const char* const c_attribute_filename = "filename";
const char* const c_attribute_ip_addrs = "ip_addrs";
const char* const c_attribute_na_addrs = "na_addrs";
const char* const c_attribute_password = "password";
const char* const c_attribute_security = "security";
const char* const c_attribute_timezone = "timezone";
const char* const c_attribute_username = "username";
const char* const c_attribute_web_root = "web_root";
const char* const c_attribute_arguments = "arguments";
const char* const c_attribute_max_peers = "max_peers";
const char* const c_attribute_set_trace = "set_trace";
const char* const c_attribute_use_https = "use_https";
const char* const c_attribute_local_hash = "local_hash";
const char* const c_attribute_blockchains = "blockchains";
const char* const c_attribute_gpg_password = "gpg_password";
const char* const c_attribute_max_sessions = "max_sessions";
const char* const c_attribute_pem_password = "pem_password";
const char* const c_attribute_rpc_password = "rpc_password";
const char* const c_attribute_sql_password = "sql_password";
const char* const c_attribute_default_storage = "default_storage";
const char* const c_attribute_peer_ips_direct = "peer_ips_direct";
const char* const c_attribute_peer_ips_permit = "peer_ips_permit";
const char* const c_attribute_peer_ips_reject = "peer_ips_reject";
const char* const c_attribute_script_reconfig = "script_reconfig";
const char* const c_attribute_session_timeout = "session_timeout";
const char* const c_attribute_local_public_key = "local_public_key";
const char* const c_attribute_master_public_key = "master_public_key";
const char* const c_attribute_max_send_attempts = "max_send_attempts";
const char* const c_attribute_max_attached_data = "max_attached_data";
const char* const c_attribute_max_storage_handlers = "max_storage_handlers";
const char* const c_attribute_files_area_item_max_num = "files_area_item_max_num";
const char* const c_attribute_files_area_item_max_size = "files_area_item_max_size";

const char* const c_section_client = "client";
const char* const c_section_extern = "extern";

const char* const c_attribute_port = "port";
const char* const c_attribute_label = "label";
const char* const c_attribute_is_local = "is_local";
const char* const c_attribute_protocol = "protocol";
const char* const c_attribute_extra_info = "extra_info";
const char* const c_attribute_script_name = "script_name";

const char* const c_default_web_root = "%root%/%store%";

const char* const c_expand_root = "%root%";
const char* const c_expand_store = "%store%";

const char* const c_uid_anon = "anon";
const char* const c_uid_unknown = "<unknown>";

const char* const c_dead_keys_ext = ".dead_keys.lst";

const char* const c_default_pem_password = "password";

const char* const c_script_dummy_filename = "*script*";

const char* const c_storable_file_name_id = "id";
const char* const c_storable_file_name_limit = "limit";
const char* const c_storable_file_name_log_id = "log_id";
const char* const c_storable_file_name_mod_dir = "mod_dir";
const char* const c_storable_file_name_trunc_n = "trunc_n";
const char* const c_storable_file_name_version = "version";
const char* const c_storable_file_name_web_root = "web_root";

const char* const c_storable_folder_name_modules = "modules";

const char* const c_temporary_special_variable_suffix = "_temporary";

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

mutex g_mutex;
mutex g_trace_mutex;

string g_storage_name_lock;

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
   session( size_t id, size_t slot,
    command_handler& cmd_handler, storage_handler* p_storage_handler,
    bool is_peer_session, const string* p_ip_addr, const string* p_blockchain )
    :
    id( id ),
    slot( slot ),
    sql_count( 0 ),
    cache_count( 0 ),
    next_handle( 0 ),
    p_tx_helper( 0 ),
    ods_tlg_size( 0 ),
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
    p_storage_handler( p_storage_handler )
   {
      if( p_ip_addr )
         ip_addr = *p_ip_addr;

      if( p_blockchain )
         blockchain = *p_blockchain;

      dtm_created = date_time::local( );
      dtm_last_cmd = date_time::local( );

      variables.insert( make_pair( get_special_var_name( e_special_var_uuid ), uuid( ).as_string( ) ) );

#ifndef SSL_SUPPORT
      variables.insert( make_pair( get_special_var_name( e_special_var_pubkey ), "n/a" ) );
#else
      variables.insert( make_pair( get_special_var_name( e_special_var_pubkey ), priv_key.get_public( ) ) );
#endif
   }

   size_t id;
   size_t slot;

   string dtm;
   string grp;
   string uid;
   string sec;

   string ip_addr;
   string tz_name;

   string last_cmd;

   string secret;
   string account;
   string blockchain;

   set< string > perms;

   string tmp_directory;

   size_t sql_count;
   size_t cache_count;
   size_t next_handle;

   bool is_captured;
   bool running_script;

   bool is_peer_session;

   int64_t ods_tlg_size;

   bool skip_fk_fetches;
   bool skip_validation;
   bool skip_is_constrained;

   date_time dtm_created;
   date_time dtm_last_cmd;

   size_t peer_files_uploaded;
   int64_t peer_bytes_uploaded;

   size_t peer_files_downloaded;
   int64_t peer_bytes_downloaded;

   size_t session_commands_executed;

   module_container modules_by_id;
   module_container modules_by_name;

   auto_ptr< sql_db > ap_db;

#ifdef SSL_SUPPORT
   private_key priv_key;
#endif

   bool buffer_is_locked;
   auto_ptr< unsigned char > ap_buffer;

   auto_ptr< global_storage_name_lock > ap_storage_name_lock;

   command_handler& cmd_handler;

   storage_handler* p_storage_handler;

   vector< string > storage_controlled_modules;

   string transaction_log_command;
   transaction_commit_helper* p_tx_helper;

   string last_set_item;
   set< string > set_items;

   string last_deque_item;
   deque< string > deque_items;

   map< string, string > variables;

   deque< string > file_hashs_to_get;
   deque< string > file_hashs_to_put;

   set< string > tx_key_info;
   stack< ods::transaction* > transactions;

   vector< string > sql_undo_statements;

   string async_or_delayed_temp_file;

   vector< string > async_or_delayed_temp_files;
   vector< string > async_or_delayed_system_commands;

   set< size_t > release_sessions;
   map< size_t, date_time > condemned_sessions;

   module_commands_registry_container commands_registry;
   object_instance_registry_container instance_registry;
};

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
      e_lock_type_propagate,
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
   else if( type == e_lock_type_propagate )
      retval = "propagate";
   else
      retval = "**unknown**";

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
   //   V  L  R  O  U  C  D  P
      { 1, 1, 1, 1, 1, 1, 1, 1 }, // V
      { 1, 1, 1, 1, 1, 0, 0, 0 }, // L
      { 1, 1, 1, 1, 0, 0, 0, 0 }, // R
      { 1, 1, 1, 0, 0, 0, 0, 0 }, // O
      { 1, 1, 0, 0, 0, 0, 0, 0 }, // U
      { 1, 0, 0, 0, 0, 0, 0, 0 }, // C
      { 1, 0, 0, 0, 0, 0, 0, 0 }, // D
      { 1, 0, 0, 0, 0, 0, 0, 1 }, // P
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

struct storage_root
{
   storage_root( )
    :
    version( c_storage_format_version ),
    cache_limit( c_default_cache_limit ),
    identity( uuid( ).as_string( ) ),
    web_root( c_default_web_root ),
    truncation_count( 0 )
   {
   }

   int32_t version;
   int32_t cache_limit;

   string identity;
   string web_root;

   string module_directory;

   int32_t truncation_count;

   vector< string > module_list;

   log_identity log_id;

   void store_as_text_files( ods_file_system& ofs );
   void fetch_from_text_files( ods_file_system& ods );
};

void storage_root::store_as_text_files( ods_file_system& ofs )
{
   ofs.store_as_text_file( c_storable_file_name_id, identity );
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
    p_bulk_write( 0 ),
    next_lock_handle( 1 ),
    p_alternative_log_file( 0 ),
    is_locked_for_admin( false )
   {
   }

   size_t get_slot( ) const { return slot; }
   const string& get_name( ) const { return name; }

   ods* get_ods( ) const { return p_ods; }

   void release_ods( ) { delete p_ods; p_ods = 0; }

   void obtain_bulk_write( ) { p_bulk_write = new ods::bulk_write( *p_ods ); }
   void release_bulk_write( ) { delete p_bulk_write; p_bulk_write = 0; }

   size_t get_ref_count( ) const { return ref_count; }

   size_t inc_ref_count( ) { return ++ref_count; }
   size_t dec_ref_count( ) { return --ref_count; }

   ofstream& get_log_file( ) { return log_file; }

   ofstream* get_alternative_log_file( ) { return p_alternative_log_file; }
   void set_alternative_log_file( ofstream* p_log_file ) { p_alternative_log_file = p_log_file; }

   storage_root& get_root( ) { return root; }
   const storage_root& get_root( ) const { return root; }

   bool is_using_blockchain( ) const { return root.identity.find( ':' ) != string::npos; }

   bool get_is_locked_for_admin( ) const { return is_locked_for_admin; }
   void set_is_locked_for_admin( bool lock_for_admin = true ) { is_locked_for_admin = lock_for_admin; }

   void dump_cache( ostream& os ) const;
   void dump_locks( ostream& os ) const;

   bool obtain_lock( size_t& handle, const string& lock_class, const string& lock_instance,
    op_lock::lock_type type, session* p_session, class_base* p_class_base = 0, class_base* p_root_class_base = 0 );

   void transform_lock( size_t handle, op_lock::lock_type new_type );

   void release_lock( size_t handle, bool force_removal = false );

   bool has_lock_info( size_t handle ) const;
   op_lock get_lock_info( size_t handle ) const;

   op_lock get_lock_info( const string& lock_class, const string& lock_instance ) const;

   op_lock get_lock_info_for_owner( const string& lock_class, const string& lock_instance, class_base& owner ) const;

   void release_locks_for_owner( class_base& owner, bool force_removal = false );

   void release_locks_for_commit( session* p_session );
   void release_locks_for_rollback( session* p_session );

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
   ods::bulk_write* p_bulk_write;

   ofstream log_file;
   ofstream* p_alternative_log_file;

   storage_root root;

   size_t next_lock_handle;

   bool is_locked_for_admin;

   mutable mutex lock_mutex;
   mutable mutex cache_mutex;

   lock_container locks;
   lock_index_container lock_index;

   set< size_t > lock_duplicates;

   set< string > dead_keys;

   time_info_container key_for_time;
   map< string, time_t > time_for_key;

   map< string, vector< string > > record_cache;

   storage_handler( const storage_handler& );
   storage_handler& operator ==( const storage_handler& );
};

void storage_handler::dump_cache( ostream& os ) const
{
   os << "date_time_accessed  key (class_id:instance)                                          ver.rev\n";
   os << "------------------- ---------------------------------------------------------------- -------\n";
   for( time_info_const_iterator ci = key_for_time.begin( ); ci != key_for_time.end( ); ++ci )
   {
      time_t t = ci->first;
      struct tm* p_t = localtime( &t );

      date_time dt( p_t->tm_year + 1900, ( month )( p_t->tm_mon + 1 ),
       p_t->tm_mday, p_t->tm_hour, p_t->tm_min, ( second )p_t->tm_sec );

      os.setf( ios::left );

      os << dt.as_string( e_time_format_hhmmss, true ) << ' ' << setw( 64 ) << ci->second << ' ';

      const vector< string >& columns( record_cache.find( ci->second )->second );
      if( columns.size( ) > 2 )
         os << columns[ 1 ] << '.' << columns[ 2 ];

      os << '\n';
   }
}

void storage_handler::dump_locks( ostream& os ) const
{
   os << "handle key (lock_class:instance)                     type       tx_type    tran_id    tran_level p_session      p_class_base   p_root_class\n";
   os << "------ --------------------------------------------- ---------- ---------- ---------- ---------- -------------- -------------- --------------\n";

   lock_index_const_iterator lici;
   for( lici = lock_index.begin( ); lici != lock_index.end( ); ++lici )
   {
      op_lock& next_lock( lici->second->second );

      os.setf( ios::left );

      os << setw( 6 ) << lici->first
       << ' ' << setw( 45 ) << lici->second->first
       << ' ' << setw( 10 ) << op_lock::lock_type_name( next_lock.type )
       << ' ' << setw( 10 ) << op_lock::lock_type_name( next_lock.tx_type )
       << ' ' << setw( 10 ) << next_lock.transaction_id
       << ' ' << setw( 10 ) << next_lock.transaction_level
       << ' ' << setw( 14 ) << next_lock.p_session
       << ' ' << setw( 14 ) << next_lock.p_class_base << ' ' << next_lock.p_root_class << '\n';
   }
}

bool storage_handler::obtain_lock( size_t& handle,
 const string& lock_class, const string& lock_instance,
 op_lock::lock_type type, session* p_session, class_base* p_class_base, class_base* p_root_class )
{
   int attempts = c_max_lock_attempts;

   TRACE_LOG( TRACE_LOCK_OPS, "[obtain lock] class = " + lock_class
    + ", instance = " + lock_instance + ", type = " + to_string( type ) + " (" + op_lock::lock_type_name( type ) + ")" );

   bool found = false;
   while( attempts-- )
   {
      // NOTE: Empty scope for lock object.
      {
         guard g( lock_mutex );

         ods* p_ods( ods::instance( ) );

         string key( lock_class );
         key += ':';
         key += lock_instance;

         lock_iterator li( locks.lower_bound( lock_class ) );

         // NOTE: Check existing locks of the same class for a conflicting lock
         // (an empty lock instance is treated as a class-wide lock).
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

            if( !lock_instance.empty( ) && !next_lock_instance.empty( ) && lock_instance != next_lock_instance )
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

            // NOTE: Locks that are effectively "dead" (i.e. awaiting cleanup when the tx completes) can
            // result in increasingly poorer performance if duplicates are allowed to exist (due to lock
            // clash scanning) therefore remove any duplicates as they are discovered.
            if( next_lock.type == op_lock::e_lock_type_none && next_lock == last_lock )
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

            // NOTE: Cascade locks will be ignored within the same session as it can sometimes be necessary for
            // update or delete operations to occur on an already cascade locked instance. Locks that are being
            // held for the duration of a transaction are ignored within the same session.
            if( ( lock_instance.empty( ) || next_lock_instance.empty( ) || lock_instance == next_lock_instance )
             && ( ( !locks_can_coexist( type, next_lock.type )
             && ( p_session != next_lock.p_session || p_root_class || p_root_class == next_lock.p_root_class ) )
             || ( next_lock.tx_type && p_session != next_lock.p_session && !locks_can_coexist( type, next_lock.tx_type ) ) ) )
            {
               // NOTE: Allow "update" locks to be obtained for an already "update" locked instance provided
               // that the instances are separate but owned by the same session and that the existing locked
               // instance is in the "after_store" trigger. Allow "review" locks for separate instances that
               // are owned by the same session (even if class locked).
               if( p_session != next_lock.p_session
                || ( p_class_base == next_lock.p_class_base )
                || ( type != next_lock.type && type != op_lock::e_lock_type_review )
                || ( !next_lock.p_class_base && type != op_lock::e_lock_type_review )
                || ( type != op_lock::e_lock_type_review && type != op_lock::e_lock_type_update )
                || ( type != op_lock::e_lock_type_review && !next_lock.p_class_base->get_is_after_store( ) ) )
               {
                  lock_conflict = true;
                  if( !attempts )
                     TRACE_LOG( TRACE_LOCK_OPS, "*** failed to acquire lock ***" );
                  break;
               }
            }

            last_lock = next_lock;
            ++li;
         }

         if( !lock_conflict )
         {
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

      msleep( c_lock_attempt_sleep_time );
   }

   IF_IS_TRACING( TRACE_LOCK_OPS )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_LOCK_OPS, "[dumping locks]\n" + osstr.str( ) );
   }

   return found;
}

void storage_handler::transform_lock( size_t handle, op_lock::lock_type new_type )
{
   guard g( lock_mutex );

   TRACE_LOG( TRACE_LOCK_OPS, "[transform lock] handle = "
    + to_string( handle ) + ", new_type = " + to_string( new_type ) );

   if( handle && lock_duplicates.find( handle ) == lock_duplicates.end( ) )
   {
      lock_index_iterator lii = lock_index.find( handle );
      if( lii != lock_index.end( ) )
      {
         lock_iterator li( lii->second );
         li->second.type = new_type;
      }
   }

   IF_IS_TRACING( TRACE_LOCK_OPS )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_LOCK_OPS, "[dump_locks]\n" + osstr.str( ) );
   }
}

void storage_handler::release_lock( size_t handle, bool force_removal )
{
   guard g( lock_mutex );

   TRACE_LOG( TRACE_LOCK_OPS, "[release lock] handle = "
    + to_string( handle ) + ", force_removal = " + to_string( force_removal ) );

   if( handle && lock_duplicates.find( handle ) == lock_duplicates.end( ) )
   {
      lock_index_iterator lii = lock_index.find( handle );
      if( lii != lock_index.end( ) )
      {
         lock_iterator li( lii->second );
         if( !force_removal && li->second.transaction_level > 0 )
            li->second.type = op_lock::e_lock_type_none;
         else
         {
            lock_index.erase( lii );
            locks.erase( li );
         }
      }
   }

   IF_IS_TRACING( TRACE_LOCK_OPS )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_LOCK_OPS, "[dump_locks]\n" + osstr.str( ) );
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

      // NOTE: If more than one matching lock exists then return the strongest lock type.
      if( lci->second.type > lock.type )
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

void storage_handler::release_locks_for_owner( class_base& owner, bool force_removal )
{
   guard g( lock_mutex );

   TRACE_LOG( TRACE_LOCK_OPS, "[release locks for owner] owner = "
    + to_string( &owner ) + ", force_removal = " + to_string( force_removal ) );

   lock_index_iterator lii;
   for( lii = lock_index.begin( ); lii != lock_index.end( ); )
   {
      op_lock& next_lock( lii->second->second );

      if( next_lock.p_root_class == &owner )
      {
         if( !force_removal && next_lock.transaction_level > 0 )
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

   IF_IS_TRACING( TRACE_LOCK_OPS )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_LOCK_OPS, "[dump_locks]\n" + osstr.str( ) );
   }
}

void storage_handler::release_locks_for_commit( session* p_session )
{
   guard g( lock_mutex );

   TRACE_LOG( TRACE_LOCK_OPS, "[release locks for commit] p_session = " + to_string( p_session ) );

   ods* p_ods( ods::instance( ) );

   lock_index_iterator lii;
   for( lii = lock_index.begin( ); lii != lock_index.end( ); )
   {
      op_lock& next_lock( lii->second->second );

      if( next_lock.p_session == p_session
       && next_lock.transaction_level >= p_ods->get_transaction_level( ) )
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

   lock_duplicates.clear( );

   IF_IS_TRACING( TRACE_LOCK_OPS )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_LOCK_OPS, "[dump_locks]\n" + osstr.str( ) );
   }
}

void storage_handler::release_locks_for_rollback( session* p_session )
{
   guard g( lock_mutex );

   TRACE_LOG( TRACE_LOCK_OPS, "[release locks for rollback] p_session = " + to_string( p_session ) );

   ods* p_ods( ods::instance( ) );

   lock_index_iterator lii;
   for( lii = lock_index.begin( ); lii != lock_index.end( ); )
   {
      op_lock& next_lock( lii->second->second );

      if( next_lock.p_session == p_session
       && next_lock.transaction_id == p_ods->get_transaction_id( )
       && next_lock.transaction_level >= p_ods->get_transaction_level( ) )
      {
         locks.erase( lii->second );
         lock_index.erase( lii++ );
      }
      else
         ++lii;
   }

   lock_duplicates.clear( );

   IF_IS_TRACING( TRACE_LOCK_OPS )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_LOCK_OPS, "[dump_locks]\n" + osstr.str( ) );
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
       && get_record_cache( ).size( ) > new_limit )
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

// NOTE: If max sessions is set to a value greater than 1000 then would need to change "gen_key".
const size_t c_max_sessions_limit = 1000;

const size_t c_max_sessions_default = 100;
const size_t c_max_storage_handlers_default = 10;

const size_t c_files_area_item_max_num_default = 10000;
const size_t c_files_area_item_max_size_default = 1000000; // i.e. 1MB

string g_empty_string;

size_t g_max_sessions = c_max_sessions_default;
size_t g_max_storage_handlers = c_max_storage_handlers_default + 1; // i.e. extra for <none>

size_t g_files_area_item_max_num = c_files_area_item_max_num_default;
size_t g_files_area_item_max_size = c_files_area_item_max_size_default;

const char* const c_ciyam_server_tlg = "ciyam_server.tlg";

const char* const c_default_storage_name = "<none>";
const char* const c_default_storage_identity = "<default>";

map< string, map< string, string > > g_crypt_keys;

auto_ptr< ods > gap_ods;
auto_ptr< ods_file_system > gap_ofs;

size_t get_last_raw_file_data_chunk( const string& log_blob_file_prefix, string& raw_file_data, string* p_last_hash = 0, bool remove_existing_blobs = false )
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

         create_list_tree( "", log_blob_file_prefix, true,
          c_ciyam_tag + separator + c_variables_branch, c_ciyam_tag, "" );
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

void append_ciyam_ods_transaction_log_files( )
{
   string separator( ":" );

   string raw_file_data, old_raw_file_data;
   size_t raw_file_chunk = get_last_raw_file_data_chunk( c_ciyam_server_tlg, old_raw_file_data );

   size_t max_chunk_size = g_files_area_item_max_size - 1;

   // NOTE: The initial "chunk" of the ODS transaction log needs to always be re-written as the
   // ODS transaction log header is being updated every time that the log is being appended to.
   raw_file_data = string( c_file_type_str_blob ) + buffer_file( c_ciyam_server_tlg, max_chunk_size );

   string log_raw_file_tag_name( string( c_ciyam_server_tlg ) + "." + to_comparable_string( 0, false, 5 ) );

   create_raw_file( raw_file_data, true, log_raw_file_tag_name.c_str( ) );

   create_list_tree( log_raw_file_tag_name, log_raw_file_tag_name, true,
    c_ciyam_tag + separator + c_variables_branch + separator + c_ciyam_server_tlg, c_ciyam_tag, "" );

   if( raw_file_chunk > 0 )
   {
      raw_file_data = old_raw_file_data;

      size_t bytes_buffered = raw_file_data.size( );

      int64_t total_size = file_size( c_ciyam_server_tlg );

      while( true )
      {
         raw_file_data.erase( );

         if( bytes_buffered < max_chunk_size )
         {
            raw_file_data += buffer_file( c_ciyam_server_tlg,
             max_chunk_size - bytes_buffered, 0, ( raw_file_chunk * max_chunk_size ) + bytes_buffered );

            // NOTE: Each "line" is a chunk of the transaction log data.
            vector< string > lines;
            lines.push_back( raw_file_data );

            append_transaction_log_lines_to_blob_files( c_ciyam_server_tlg, lines );
         }

         if( ++raw_file_chunk * max_chunk_size >= total_size )
            break;

         bytes_buffered = 0;
      }
   }
}

void init_ciyam_ods( )
{
   gap_ods.reset( new ods( c_ciyam_server,
    ods::e_open_mode_create_if_not_exist, ods::e_write_mode_exclusive, true ) );

   ods::bulk_write bulk_write( *gap_ods );
   scoped_ods_instance ods_instance( *gap_ods );

   bool was_just_created = false;

   if( gap_ods->is_new( ) )
      was_just_created = true;
   else
      gap_ods->repair_if_corrupt( );

   gap_ofs.reset( new ods_file_system( *gap_ods ) );

   auto_ptr< ods::transaction > ap_tx;

   if( was_just_created
    || !gap_ofs->has_folder( c_file_archives_folder )
    || !gap_ofs->has_folder( c_file_blacklist_folder )
    || !gap_ofs->has_folder( c_file_repository_folder )
    || !gap_ofs->has_folder( c_system_variables_folder ) )
      ap_tx.reset( new ods::transaction( *gap_ods ) );

   if( !gap_ofs->has_folder( c_file_archives_folder ) )
      gap_ofs->add_folder( c_file_archives_folder );

   if( !gap_ofs->has_folder( c_file_blacklist_folder ) )
      gap_ofs->add_folder( c_file_blacklist_folder );

   if( !gap_ofs->has_folder( c_file_repository_folder ) )
      gap_ofs->add_folder( c_file_repository_folder );

   if( !gap_ofs->has_folder( c_system_variables_folder ) )
      gap_ofs->add_folder( c_system_variables_folder );

   if( ap_tx.get( ) )
      ap_tx->commit( );

   if( !was_just_created )
   {
      // NOTE: Restore all persistent system variable values.
      vector< string > variable_files;

      gap_ofs->set_root_folder( c_system_variables_folder );

      gap_ofs->list_files( variable_files );

      for( size_t i = 0; i < variable_files.size( ); i++ )
      {
         string value;
         gap_ofs->fetch_from_text_file( variable_files[ i ], value );

         set_system_variable( variable_files[ i ], value );
      }
   }
}

void term_ciyam_ods( )
{
   gap_ofs.reset( );
   gap_ods.reset( );
}

typedef map< int, string > listener_container;

typedef listener_container::const_iterator listener_const_iterator;

listener_container g_listeners;

size_t g_next_session_id;

typedef vector< session* > session_container;

session_container g_sessions;

map< size_t, date_time > g_condemned_sessions;

static TLS( session )* gtp_session;

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
   guard g( g_mutex );

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
         throw runtime_error( "storage '" + name + "' cannot be created as it's already in use" );

      if( lock_for_admin )
         throw runtime_error( "storage '" + name + "' cannot be administered as it's already in use" );

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

      if( !slot || slot == g_max_storage_handlers )
         throw runtime_error( "max. permitted concurrent storage handlers already active" );

      if( gtp_session->ap_storage_name_lock.get( ) )
         gtp_session->ap_storage_name_lock.reset( );

      if( name == g_storage_name_lock )
         throw runtime_error( "storage '" + name + "' cannot be administered as it's already in use" );

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
         auto_ptr< ods > ap_ods( new ods( name.c_str( ), open_mode, ods::e_write_mode_exclusive, true, &file_not_found ) );
         auto_ptr< storage_handler > ap_handler( new storage_handler( slot, name, ap_ods.get( ) ) );

         ap_handler->obtain_bulk_write( );

         // NOTE: In case a server shutdown had occurred whilst an ODS transaction was still active.
         if( !ap_ods->is_new( ) )
            ap_ods->repair_if_corrupt( );

         ods::instance( ap_ods.get( ) );

         if( ap_ods->is_new( ) )
         {
            ods::transaction tx( *ap_ods );

            ap_handler->get_root( ).module_directory = directory;

            string blockchain( get_raw_session_variable( get_special_var_name( e_special_var_blockchain ) ) );

            if( !blockchain.empty( ) )
               ap_handler->get_root( ).identity += ":" + blockchain;

            ods_file_system ofs( *ap_ods );

            ofs.add_folder( c_storable_folder_name_modules );
            ap_handler->get_root( ).store_as_text_files( ofs );

            tx.commit( );
         }
         else
         {
            if( ap_handler->get_root( ).version != c_storage_format_version )
               throw runtime_error( "found incorrect storage format version " + to_string( ap_handler->get_root( ).version ) );

            ods_file_system ofs( *ap_ods );
            ap_handler->get_root( ).fetch_from_text_files( ofs );

            ofs.set_folder( c_storable_folder_name_modules );

            vector< string > order_prefixed_module_list;
            ofs.list_folders( order_prefixed_module_list );

            for( size_t i = 0; i < order_prefixed_module_list.size( ); i++ )
            {
               string next( order_prefixed_module_list[ i ] );
               string::size_type pos = next.find( c_module_order_prefix_separator );

               if( pos != string::npos )
                  next.erase( 0, pos + 1 );

               ap_handler->get_root( ).module_list.push_back( next );
            }

            // NOTE: Force an identity write to occur when the first transaction is logged.
            ap_handler->get_root( ).log_id.ceiling = ap_handler->get_root( ).log_id.next_id;
         }

         string dead_keys_file( ap_handler->get_name( ) + c_dead_keys_ext );
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


                  string class_id = next.substr( 0, pos );

                  ap_handler->get_dead_keys( ).insert( next );
               }
            }

            buffer_file_lines( dead_keys_file, ap_handler->get_dead_keys( ) );
         }

         p_new_handler = ap_handler.release( );

         was_constructed = true;

         g_storage_handlers[ slot ] = p_new_handler;
         g_storage_handler_index.insert( make_pair( name, slot ) );

         // NOTE: The ODS instance now belongs to the storage handler (and not to this session).
         ap_ods.release( );
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

   if( p_new_handler && p_new_handler != gtp_session->p_storage_handler )
   {
      if( p_new_handler->get_is_locked_for_admin( ) )
         throw runtime_error( "storage '" + name + "' is currently locked for administration" );

      bool created_ods_instance = false;
      storage_handler* p_old_handler = gtp_session->p_storage_handler;
      try
      {
         gtp_session->ap_db.reset( new sql_db( p_new_handler->get_name( ), p_new_handler->get_name( ) ) );

         ods::instance( new ods( *p_new_handler->get_ods( ) ) );
         created_ods_instance = true;

         gtp_session->p_storage_handler = p_new_handler;
         gtp_session->p_storage_handler->inc_ref_count( );

         if( module_count( ) && !p_new_handler->get_root( ).module_directory.empty( ) )
            throw runtime_error( "storages with a module directory cannot be initialised whilst modules are already loaded" );

         module_directory( &p_new_handler->get_root( ).module_directory );

         // NOTE: Modules that have been registered to this storage are now automatically loaded (if not already present).
         // This is performed in the same order that the modules were registered in as dependencies may exist between them.
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
            p_new_handler->release_bulk_write( );
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

bool fetch_instance_from_cache( class_base& instance, const string& key, bool sys_only_fields = false )
{
   guard g( g_mutex );

   bool found = false;
   class_base_accessor instance_accessor( instance );

   string key_info( instance.get_class_id( ) + ":" + key );

   storage_handler& handler( *gtp_session->p_storage_handler );

   found = handler.get_record_cache( ).count( key_info );

   if( found )
   {
      time_info_iterator tii = handler.get_key_for_time( ).lower_bound( handler.get_time_for_key( ).find( key_info )->second );
      while( tii->second != key_info )
         ++tii;

      time_t tm( time( 0 ) );
      handler.get_time_for_key( )[ key_info ] = tm;

      handler.get_key_for_time( ).erase( tii );
      handler.get_key_for_time( ).insert( make_pair( tm, key_info ) );
   }

   if( found )
   {
      found = true;
      ++gtp_session->cache_count;

      if( !sys_only_fields )
         instance_accessor.clear( );

      TRACE_LOG( TRACE_SQLSTMTS, "*** fetching '" + key_info + "' from cache ***" );

      vector< string >& columns( handler.get_record_cache( )[ key_info ] );

      instance_accessor.set_key( columns[ 0 ], true );
      instance_accessor.set_version( from_string< uint16_t >( columns[ 1 ] ) );
      instance_accessor.set_revision( from_string< uint64_t >( columns[ 2 ] ) );

      instance_accessor.set_original_revision( instance.get_revision( ) );
      instance_accessor.set_original_identity( columns[ 3 ] );

      if( !sys_only_fields )
      {
         int fnum = 4;
         TRACE_LOG( TRACE_SQLCLSET, "(from cache)" );
         for( int i = fnum; i < columns.size( ); i++, fnum++ )
         {
            while( instance.is_field_transient( fnum - 4 ) )
               fnum++;

            TRACE_LOG( TRACE_SQLCLSET, "setting field #" + to_string( fnum - 4 + 1 ) + " to " + columns[ i ] );
            instance.set_field_value( fnum - 4, columns[ i ] );
         }

         instance_accessor.after_fetch_from_db( );

         string skip_after_fetch_var(
          instance.get_raw_variable( get_special_var_name( e_special_var_skip_after_fetch ) ) );

         if( skip_after_fetch_var == "1" || skip_after_fetch_var == c_true )
            ; // i.e. do nothing
         else
            instance_accessor.perform_after_fetch( );
      }
   }

   return found;
}

bool fetch_instance_from_db( class_base& instance,
 const map< int, int >& fields, const vector< int >& columns, bool skip_after_fetch )
{
   class_base_accessor instance_accessor( instance );
   sql_data& sd( *instance_accessor.p_sql_data( ) );

   bool found = sd.next( );
   instance_accessor.clear( );

   if( !found )
   {
      delete instance_accessor.p_sql_data( );
      instance_accessor.p_sql_data( ) = 0;
   }
   else
   {
      TRACE_LOG( TRACE_SQLCLSET, "(from instance dataset)" );

      instance_accessor.set_key( sd.as_string( 0 ), true );
      instance_accessor.set_version( from_string< uint16_t >( sd.as_string( 1 ) ) );
      instance_accessor.set_revision( from_string< uint64_t >( sd.as_string( 2 ) ) );

      instance_accessor.set_original_revision( instance.get_revision( ) );
      instance_accessor.set_original_identity( sd.as_string( 3 ) );

      int fcount = sd.get_fieldcount( );

      for( int i = 4; i < fcount; i++ )
      {
         if( !fields.count( columns[ i - 4 ] ) )
            throw runtime_error( "unexpected field # not found for column #" + to_string( i - 4 ) );

         int fnum( fields.find( columns[ i - 4 ] )->second );

         TRACE_LOG( TRACE_SQLCLSET, "setting field #" + to_string( fnum + 1 ) + " to " + sd.as_string( i ) );
         instance.set_field_value( fnum, sd.as_string( i ) );
      }
   }

   instance_accessor.after_fetch_from_db( );

   if( !skip_after_fetch )
      instance_accessor.perform_after_fetch( );

   return found;
}

bool fetch_instance_from_db( class_base& instance,
 const string& sql, bool sys_only_fields = false, bool is_minimal_fetch = false, bool allow_caching = false )
{
   bool found = false;
   class_base_accessor instance_accessor( instance );

   if( sys_only_fields && allow_caching )
      found = fetch_instance_from_cache( instance, instance_accessor.get_lazy_fetch_key( ), true );

   if( !found && gtp_session && gtp_session->ap_db.get( ) )
   {
      TRACE_LOG( TRACE_SQLSTMTS, sql );

      sql_dataset ds( *gtp_session->ap_db.get( ), sql );
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

            instance_accessor.set_original_revision( instance.get_revision( ) );
            instance_accessor.set_original_identity( ds.as_string( 3 ) );

            if( !sys_only_fields )
            {
               TRACE_LOG( TRACE_SQLCLSET, "(from temporary dataset)" );

               int fnum = 4;
               for( int i = fnum; i < ds.get_fieldcount( ); i++, fnum++ )
               {
                  while( instance.is_field_transient( fnum - 4 ) )
                     fnum++;

                  TRACE_LOG( TRACE_SQLCLSET, "setting field #" + to_string( fnum - 4 + 1 ) + " to " + ds.as_string( i ) );
                  instance.set_field_value( fnum - 4, ds.as_string( i ) );
               }

               if( allow_caching && !is_minimal_fetch )
               {
                  guard g( g_mutex );

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

                     time_t tm( time( 0 ) );
                     handler.get_key_for_time( ).insert( make_pair( tm, key_info ) );
                     handler.get_time_for_key( ).insert( make_pair( key_info, tm ) );
                     handler.get_record_cache( ).insert( make_pair( key_info, columns ) );

                     if( handler.get_key_for_time( ).size( ) != handler.get_time_for_key( ).size( )
                      || handler.get_key_for_time( ).size( ) != handler.get_record_cache( ).size( ) )
                        throw runtime_error( "*** record cache maps size mismatch ***" );
                  }
               }
            }

            instance_accessor.after_fetch_from_db( );

            string skip_after_fetch_var(
             instance.get_raw_variable( get_special_var_name( e_special_var_skip_after_fetch ) ) );

            if( skip_after_fetch_var == "1" || skip_after_fetch_var == c_true )
               ; // i.e. do nothing
            else
               instance_accessor.perform_after_fetch( is_minimal_fetch );
         }
      }
   }

   return found;
}

bool global_storage_persistence_is_file( string& root_child_folder )
{
   bool is_file_not_folder = false;

   if( !root_child_folder.empty( )
    && root_child_folder[ root_child_folder.length( ) - 1 ] == c_global_storage_file_not_folder_suffix )
   {
      is_file_not_folder = true;
      root_child_folder.erase( root_child_folder.length( ) - 1 );
   }

   return is_file_not_folder;
}

bool has_instance_in_global_storage( class_base& instance, const string& key )
{
   string persistence_extra( instance.get_persistence_extra( ) );

   string root_child_folder( persistence_extra );
   bool is_file_not_folder( global_storage_persistence_is_file( root_child_folder ) );

   ods::bulk_read bulk_read( *gap_ods );
   scoped_ods_instance ods_instance( *gap_ods );

   gap_ofs->set_root_folder( root_child_folder );

   if( is_file_not_folder )
      return gap_ofs->has_file( key );
   else
      return gap_ofs->has_folder( key );
}

void fetch_keys_from_global_storage( class_base& instance,
 const string& start_from, bool inclusive, size_t limit, vector< string >& global_keys, bool in_reverse_order )
{
   string persistence_extra( instance.get_persistence_extra( ) );

   string root_child_folder( persistence_extra );
   bool is_file_not_folder( global_storage_persistence_is_file( root_child_folder ) );

   ods::bulk_read bulk_read( *gap_ods );
   scoped_ods_instance ods_instance( *gap_ods );

   gap_ofs->set_root_folder( root_child_folder );

   if( limit == 1 )
   {
      if( ( is_file_not_folder && gap_ofs->has_file( start_from ) )
       || ( !is_file_not_folder && gap_ofs->has_folder( start_from ) ) )
         global_keys.push_back( start_from );
   }
   else
   {
      if( is_file_not_folder )
         gap_ofs->list_files( global_keys, start_from, inclusive, limit, in_reverse_order );
      else
         gap_ofs->list_folders( global_keys, start_from, inclusive, limit, in_reverse_order );
   }
}

bool fetch_instance_from_global_storage( class_base& instance, const string& key,
 const vector< string >& field_names, vector< string >* p_columns = 0, bool skip_after_fetch = false )
{
   bool found = false;

   field_info_container field_info;
   instance.get_field_info( field_info );

   class_base_accessor instance_accessor( instance );
   string persistence_extra( instance.get_persistence_extra( ) );

   string root_child_folder( persistence_extra );
   bool is_file_not_folder( global_storage_persistence_is_file( root_child_folder ) );

   ods::bulk_read bulk_read( *gap_ods );
   scoped_ods_instance ods_instance( *gap_ods );

   gap_ofs->set_root_folder( root_child_folder );

   if( is_file_not_folder )
      found = gap_ofs->has_file( key );
   else
      found = gap_ofs->has_folder( key );

   if( found )
   {
      if( !is_file_not_folder )
         gap_ofs->set_folder( key );

      if( p_columns )
      {
         p_columns->push_back( key );
         p_columns->push_back( "1" );
         p_columns->push_back( "0" );
         p_columns->push_back( instance.get_module_id( ) + ':' + instance.get_class_id( ) );
      }
      else
      {
         instance_accessor.set_key( key, true );

         instance_accessor.set_version( 1 );
         instance_accessor.set_revision( 0 );

         instance_accessor.set_original_revision( instance.get_revision( ) );
         instance_accessor.set_original_identity( instance.get_module_id( ) + ':' + instance.get_class_id( ) );
      }

      // NOTE: If is stored as a file then attributes are expected to be
      // in the format of a structured I/O file otherwise each attribute
      // is expected to be a file within the record's folder.
      if( !field_names.empty( ) )
      {
         stringstream sio_data;
         auto_ptr< sio_reader > ap_sio_reader;

         if( is_file_not_folder )
         {
            gap_ofs->get_file( key, &sio_data, true );
            ap_sio_reader.reset( new sio_reader( sio_data ) );
         }

         for( size_t i = 0; i < field_names.size( ); i++ )
         {
            string data, attribute_name( lower( field_names[ i ] ) );

            if( is_file_not_folder )
               data = ap_sio_reader->read_opt_attribute( attribute_name );
            else if( gap_ofs->has_file( attribute_name ) )
               gap_ofs->fetch_from_text_file( attribute_name, data );

            if( p_columns )
               p_columns->push_back( data );
            else
               instance.set_field_value( instance.get_field_num( field_names[ i ] ), data );
         }
      }

      if( !p_columns )
      {
         instance_accessor.after_fetch_from_db( );

         if( !skip_after_fetch )
            instance_accessor.perform_after_fetch( );
      }
   }

   return found;
}

bool fetch_instance_from_global_storage( class_base& instance, const string& key )
{
   field_info_container field_info;
   instance.get_field_info( field_info );

   vector< string > field_names;

   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      if( !field_info[ i ].is_transient )
         field_names.push_back( field_info[ i ].name );
   }

   return fetch_instance_from_global_storage( instance, key, field_names );
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

            auto_ptr< class_cascade > ap_tmp_cascading;
            if( p_class_base )
               ap_tmp_cascading.reset( new class_cascade( *p_class_base ) );

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

            auto_ptr< class_cascade > ap_tmp_cascading;
            if( p_class_base )
               ap_tmp_cascading.reset( new class_cascade( *p_class_base ) );

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
                     // FUTURE: Although extremely unlinkely it's possible for the destroy lock to not be
                     // that of the original operation (i.e. instead part of a completely separate op for
                     // the same session). As the key protocol commands only deal with a single operation
                     // the assumption that the lock is part of the same operation is fine, however, were
                     // it to become desirable for multiple operations (belonging to the same session) to
                     // be supported then this assumption could be incorrect and lead to potential issues.
                     lock = handler.get_lock_info( p_class_base->get_lock_class_id( ), p_class_base->get_key( ) );
                     if( lock.p_session == gtp_session && lock.type == op_lock::e_lock_type_destroy )
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

size_t obtain_keyed_lock( const string& lock_class, const string& key, op_lock::lock_type lock_type )
{
   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   storage_handler& handler( *gtp_session->p_storage_handler );

   size_t lock_handle( 0 );
   handler.obtain_lock( lock_handle, lock_class, key, lock_type, gtp_session );
   return lock_handle;
}

inline void perform_op_cancel( storage_handler& handler, class_base& instance, class_base::op_type op )
{
   class_base_accessor instance_accessor( instance );

   // FUTURE: It would be more efficient not to call "release_locks_for_owner" unless
   // it is known that cascade locks had actually been obtained when the destroy began.
   if( op == class_base::e_op_type_destroy && !instance.get_is_being_cascaded( ) )
      handler.release_locks_for_owner( instance, true );

   instance_accessor.cancel( );
}

inline string get_field_name( class_base& instance,
 string& field_name, bool* p_sql_numeric = 0, string* p_type_name = 0 )
{
   const char* p_str;
   bool was_name = true;
   class_base_accessor instance_accessor( instance );

   if( field_name == c_key_field )
      field_name = "Key_";

   if( field_name == "Key_" || field_name == "Typ_" )
   {
      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( field_name == "Ver_" || field_name == "Rev_" )
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

void setup_select_columns( class_base& instance, const vector< string >& field_info )
{
   class_base_accessor instance_accessor( instance );

   vector< int > column_nums;
   vector< string > column_names;

   map< int, int > field_nums;
   map< string, int > field_names;
   int num_fields = instance.get_num_fields( );
   for( int i = 0, c = 0; i < num_fields; i++ )
   {
      if( instance.is_field_transient( i ) )
         continue;

      field_nums.insert( make_pair( c++, i ) );
      field_names.insert( make_pair( instance.get_field_name( i ), i ) );
   }

   instance.get_sql_column_names( column_names );

   if( field_info.empty( ) )
   {
      for( size_t i = 0; i < column_names.size( ); i++ )
         column_nums.push_back( i );
   }
   else
   {
      field_nums.clear( );

      for( size_t i = 0; i < field_info.size( ); i++ )
      {
         string next_field( field_info[ i ] );
         get_field_name( instance, next_field );

         string column_name( "C_" + next_field );

         for( size_t j = 0; j < column_names.size( ); j++ )
         {
            if( column_name == column_names[ j ] )
            {
               column_nums.push_back( j );
               field_nums.insert( make_pair( j, field_names[ next_field ] ) );
            }
         }
      }

      if( column_nums.size( ) != field_info.size( ) )
         throw runtime_error( "unexpected fields/columns size mismatch" );
   }

   instance_accessor.select_fields( ) = field_nums;
   instance_accessor.select_columns( ) = column_nums;
}

void split_key_info( const string& key_info,
 vector< pair< string, string > >& fixed_info,
 vector< pair< string, string > >& paging_info,
 vector< string >& order_info, bool do_not_add_key_to_order )
{
   string::size_type pos = key_info.find( ' ' );
   if( pos == string::npos )
   {
      if( !key_info.empty( ) )
         fixed_info.push_back( make_pair( "Key_", key_info ) );

      // NOTE: The primary key is always being added to the end of the order info as it
      // is not known here if the query will have repeatably ordered results without it.
      if( !do_not_add_key_to_order )
         order_info.push_back( "Key_" );
   }
   else
   {
      int num_fixed = 0;
      string fields( key_info.substr( 0, pos ) );
      string values( key_info.substr( pos + 1 ) );

      pos = fields.find( '#' );
      if( pos != string::npos )
      {
         num_fixed = atoi( fields.substr( pos + 1 ).c_str( ) );
         fields.erase( pos );
      }

      size_t order_provided = order_info.size( );
      split( fields, order_info );

      // NOTE: The primary key is always being added to the end of the order info as it
      // is not known here if the query will have repeatably ordered results without it.
      if( !do_not_add_key_to_order )
         order_info.push_back( "Key_" );

      if( values.empty( ) )
      {
         if( num_fixed > 1 )
            throw runtime_error( "specified # of fixed values were not provided" );
         else if( num_fixed == 1 )
         {
            if( order_info.size( ) <= order_provided + 1 )
               throw runtime_error( "missing expected ordering field" );
            fixed_info.push_back( make_pair( order_info[ order_provided ], string( ) ) );
         }
      }
      else
      {
         vector< string > all_values;
         split( values, all_values );

         if( all_values.size( ) < num_fixed )
            throw runtime_error( "specified # of fixed values were not provided" );

         if( order_info.size( ) < all_values.size( ) + order_provided )
            throw runtime_error( "not enough order info for all provided values" );

         for( size_t i = 0; i < all_values.size( ); i++ )
         {
            if( i < num_fixed )
               fixed_info.push_back( make_pair( order_info[ order_provided + i ], all_values[ i ] ) );
            else
               paging_info.push_back( make_pair( order_info[ order_provided + i ], all_values[ i ] ) );
         }
      }
   }
}

void split_text_search( const string& text_search, vector< string >& words )
{
   string next_word;
   bool in_quotes = false;
   bool had_escape = false;
   for( size_t i = 0; i < text_search.size( ); i++ )
   {
      char ch = text_search[ i ];

      if( had_escape )
      {
         had_escape = false;

         // NOTE: Backslashes can be used in order to escape quote characters
         // but if not being used for this purpose then they are instead just
         // included in the query.
         if( ch != '"' )
            next_word += '\\';

         next_word += ch;
      }
      else if( ch == '\\' )
         had_escape = true;
      else
      {
         if( ch == '"' )
         {
            in_quotes = !in_quotes;
            if( !in_quotes )
            {
               if( !next_word.empty( ) )
               {
                  words.push_back( next_word );
                  next_word.erase( );
               }
            }
            else if( i < text_search.size( ) - 1 && text_search[ i + 1 ] == '"' )
            {
               in_quotes = false;
               next_word += ch;
               ++i;
            }
         }
         else
         {
            if( ch == ' ' && !in_quotes )
            {
               if( !next_word.empty( ) )
               {
                  words.push_back( next_word );
                  next_word.erase( );
               }
            }
            else
               next_word += ch;
         }
      }
   }

   // NOTE: If the one and only character provided was a backslash then escape it
   // so that at least the query won't be empty.
   if( words.empty( ) && had_escape )
      next_word = "\\";

   if( !next_word.empty( ) )
      words.push_back( next_word );
}

size_t split_csv_values( const string& line,
 vector< string >& values, bool& last_value_incomplete, size_t continuation_offset )
{
   string next_value;

   // NOTE: If the last value was continued into the next line then only process the
   // portion that has not already been processed (i.e. continue where it left off).
   if( continuation_offset )
      next_value = line.substr( 0, continuation_offset );

   bool in_quotes = last_value_incomplete;
   for( size_t i = continuation_offset; i < line.size( ); i++ )
   {
      if( in_quotes )
      {
         if( line[ i ] == '"' )
         {
            if( i < line.size( ) - 1 && line[ i + 1 ] != ',' )
            {
               i++;
               next_value += '"';
            }
            else
               in_quotes = false;
         }
         else
            next_value += line[ i ];
      }
      else if( line[ i ] == '"' )
         in_quotes = true;
      else if( line[ i ] != ',' )
         next_value += line[ i ];
      else
      {
         values.push_back( next_value );
         next_value.erase( );
      }
   }

   values.push_back( next_value );
   last_value_incomplete = in_quotes;

   return values.size( ) - last_value_incomplete;
}

string construct_paging_sql( const vector< pair< string, string > >& paging_info, bool reverse, bool inclusive )
{
   string page_exclusive( reverse ? " < " : " > " );
   string page_inclusive( reverse ? " <= " : " >= " );

   string sql( "(" );
   for( size_t i = 0; i < paging_info.size( ); i++ )
   {
      if( i == 0 )
      {
         if( !inclusive || i != paging_info.size( ) - 1 )
            sql += "C_" + paging_info[ i ].first + page_exclusive + paging_info[ i ].second;
         else
            sql += "C_" + paging_info[ i ].first + page_inclusive + paging_info[ i ].second;
      }
      else
      {
         sql += " OR (";
         for( size_t j = 0; j < i; j++ )
         {
            if( j > 0 )
               sql += " AND ";
            sql += "C_" + paging_info[ j ].first + " = " + paging_info[ j ].second;
         }

         sql += " AND ";
         if( !inclusive || i != paging_info.size( ) - 1 )
            sql += "C_" + paging_info[ i ].first + page_exclusive + paging_info[ i ].second;
         else
            sql += "C_" + paging_info[ i ].first + page_inclusive + paging_info[ i ].second;

         sql += ")";
      }
   }
   sql += ")";

   return sql;
}

string formatted_value( const string& value, const string& field_type )
{
   string formatted_value( value );

   if( !formatted_value.empty( ) )
   {
      if( field_type == "udate" || field_type == "tdate" )
      {
         udate today( udate::standard( ) );

         if( formatted_value == c_date_today )
            formatted_value == today.as_string( );
         else if( formatted_value == c_date_tomorrow )
            formatted_value == ( ++today ).as_string( );

         formatted_value = udate( formatted_value ).as_string( true );
      }
      else if( field_type == "mtime" || field_type == "ttime" )
      {
         if( formatted_value == c_dtm_now )
            formatted_value = mtime::standard( ).as_string( );

         formatted_value = mtime( formatted_value ).as_string( true );
      }
      else if( field_type == "datetime" || field_type == "tdatetime"
       || field_type == "date_time" || field_type == "tdate_time" )
      {
         date_time now( date_time::standard( ) );

         if( formatted_value == c_dtm_now || formatted_value == c_date_today )
            formatted_value = now.as_string( );
         else if( formatted_value == c_date_tomorrow )
            formatted_value = ( ++now ).as_string( );
         // NOTE: If a date_time string starts with 'U' then it is considered as already being UTC.
         else if( formatted_value[ 0 ] == 'U' )
            formatted_value.erase( 0, 1 );
         else
         {
            string tz_name( get_tz_name( ) );
            if( tz_name.empty( ) )
               tz_name = get_timezone( );

            formatted_value = local_to_utc( date_time( formatted_value ), tz_name ).as_string( );
         }

         formatted_value = date_time( formatted_value ).as_string( true );
      }
   }

   return formatted_value;
}

string construct_sql_select(
 class_base& instance,
 const vector< string >& field_info,
 const vector< string >& order_info,
 const vector< pair< string, string > >& query_info,
 const vector< pair< string, string > >& fixed_info,
 const vector< pair< string, string > >& paging_info, const string& security_info,
 bool is_reverse, bool is_inclusive, int row_limit, bool only_sys_fields, const string& text_search )
{
   string sql, sql_fields_and_table( "SELECT " );

   class_base_accessor instance_accessor( instance );

   vector< string > use_index_fields;

   if( field_info.empty( ) )
   {
      if( !only_sys_fields )
         sql_fields_and_table += "*";
      else
         sql_fields_and_table += "C_Key_,C_Ver_,C_Rev_,C_Typ_";
   }
   else
   {
      sql_fields_and_table += "C_Key_,C_Ver_,C_Rev_,C_Typ_";
      for( size_t i = 0; i < field_info.size( ); i++ )
      {
         string next_field( field_info[ i ] );

         if( next_field.empty( ) )
            throw runtime_error( "unexpected empty field found in sql select preparation" );

         get_field_name( instance, next_field );

         sql_fields_and_table += ",C_" + next_field;
      }
   }

   sql_fields_and_table += " FROM T_"
    + string( instance.get_module_name( ) ) + "_" + string( instance.get_class_name( ) );

   if( !query_info.empty( ) || !text_search.empty( )
    || !fixed_info.empty( ) || !paging_info.empty( ) || !security_info.empty( ) )
      sql += " WHERE ";

   if( !query_info.empty( ) )
   {
      for( size_t i = 0; i < query_info.size( ); i++ )
      {
         string field_name( query_info[ i ].first );
         string field_values( query_info[ i ].second );

         if( i > 0 )
            sql += " AND ";

         bool invert = false;
         string invert_prefix;

         if( field_values.length( ) && field_values[ 0 ] == '!' )
         {
            invert = true;
            invert_prefix = "NOT ";
            field_values.erase( 0, 1 );
         }

         bool is_sub_select = false;
         string sub_select_sql_prefix, sub_select_sql_suffix;
         class_base* p_instance( &instance );

         // NOTE: If a child sub-context & field has been provided then one or more sub-selects
         // (depending upon the operator used) will be included in the generated SQL expression.
         if( !field_name.empty( ) && field_name[ 0 ] == '_' )
         {
            string::size_type pos = field_name.find( '.' );
            if( pos == string::npos )
               throw runtime_error( "unexpected field name format '" + field_name + "'" );

            class_base& child_instance( instance.get_or_create_graph_child( field_name.substr( 0, pos ) ) );

            is_sub_select = true;
            p_instance = &child_instance;
            field_name.erase( 0, pos + 1 );

            string graph_parent_fk_field( p_instance->get_graph_parent_fk_field( ) );
            get_field_name( *p_instance, graph_parent_fk_field );

            sub_select_sql_prefix = "C_Key_ " + invert_prefix + "IN (SELECT C_"
             + graph_parent_fk_field + " FROM T_" + to_string( p_instance->get_module_name( ) )
             + "_" + p_instance->get_class_name( ) + " WHERE ";

            sub_select_sql_suffix = " GROUP BY C_" + graph_parent_fk_field + ")";

            sql += sub_select_sql_prefix;
         }

         string field_type;
         bool is_sql_numeric;

         get_field_name( *p_instance, field_name, &is_sql_numeric, &field_type );

         bool was_like = false;
         if( field_values.length( ) >= 2 )
         {
            if( field_values[ 0 ] == '*' && field_values[ field_values.length( ) - 1 ] == '*' )
            {
               was_like = true;

               field_values.erase( 0, 1 );
               field_values.erase( field_values.size( ) - 1 );

               field_values = '%' + field_values + '%';

               // NOTE: For end-user convenience LIKE queries are performed case-insensitively.
               sql += "LOWER(C_" + field_name + ") " + invert_prefix + "LIKE " + sql_quote( lower( field_values ) );
            }
            else if( field_values[ field_values.length( ) - 1 ] == '*' )
            {
               was_like = true;

               field_values.erase( field_values.size( ) - 1 );

               field_values += '%';

               sql += "C_" + field_name + " " + invert_prefix + "LIKE " + sql_quote( field_values );
            }
         }

         if( !was_like )
         {
            string::size_type pos = field_values.find( ".." );
            if( pos != string::npos )
            {
               sql += "C_" + field_name + invert_prefix + " BETWEEN ";
               if( is_sql_numeric )
               {
                  sql += field_values.substr( 0, pos );
                  sql += " AND ";
                  sql += field_values.substr( pos + 2 );
               }
               else
               {
                  sql += sql_quote( formatted_value( field_values.substr( 0, pos ), field_type ) );
                  sql += " AND ";
                  sql += sql_quote( formatted_value( field_values.substr( pos + 2 ), field_type ) );
               }
            }
            else
            {
               pos = field_values.find( '|' );
               if( pos == string::npos )
               {
                  pos = field_values.find( '&' );
                  if( pos == string::npos )
                  {
                     sql += "C_" + field_name + ( invert ? " <> " : " = " );

                     sql += is_sql_numeric ? field_values : sql_quote( formatted_value( field_values, field_type ) );
                  }
                  else
                  {
                     if( !is_sub_select )
                        throw runtime_error( "unexpected '&' operator found in '" + field_values + "'" );

                     bool is_first = true;
                     while( true )
                     {
                        if( is_first )
                           is_first = false;
                        else
                        {
                           sql += sub_select_sql_suffix;
                           sql += " AND ";
                           sql += sub_select_sql_prefix;
                        }

                        sql += "C_" + field_name + ( invert ? " <> " : " = " );

                        string next_value( field_values.substr( 0, pos ) );
                        sql += is_sql_numeric ? next_value : sql_quote( formatted_value( next_value, field_type ) );

                        if( pos == string::npos )
                           break;

                        field_values.erase( 0, pos + 1 );
                        pos = field_values.find( '&' );
                     }
                  }
               }
               else
               {
                  sql += "C_" + field_name + " " + invert_prefix + "IN (";
                  while( true )
                  {
                     string next_value( field_values.substr( 0, pos ) );
                     sql += is_sql_numeric ? next_value : sql_quote( formatted_value( next_value, field_type ) );

                     if( pos == string::npos )
                        break;

                     sql += ",";
                     field_values.erase( 0, pos + 1 );
                     pos = field_values.find( '|' );
                  }

                  sql += ")";
               }
            }
         }

         if( is_sub_select )
            sql += sub_select_sql_suffix;
      }

      if( !text_search.empty( )
       || !fixed_info.empty( ) || !paging_info.empty( ) || !security_info.empty( ) )
         sql += " AND ";
   }

   if( !text_search.empty( ) )
   {
      vector< string > text_search_fields;
      instance.get_text_search_fields( text_search_fields );

      if( text_search_fields.empty( ) )
         throw runtime_error( "no text search fields defined in class '" + string( instance.get_class_name( ) ) + "'" );

      vector< string > text_search_words;
      split_text_search( text_search, text_search_words );

      if( text_search_words.size( ) > 1 )
         sql += "(";

      for( size_t i = 0; i < text_search_words.size( ); i++ )
      {
         if( i > 0 )
            sql += " AND ";

         sql += "(";

         for( size_t j = 0; j < text_search_fields.size( ); j++ )
         {
            if( j > 0 )
               sql += " OR ";

            string like_expr( escaped( text_search_words[ i ], "'%_" ) );
            like_expr = "%" + like_expr + "%";

            // NOTE: Due to issues with UTF-8 and MySQL the query needs to use both the case
            // sensitive and insensitive tests in order to get the correct results.
            sql += "C_" + text_search_fields[ j ] + " LIKE '" + like_expr + "'";
            sql += " OR LOWER(C_" + text_search_fields[ j ] + ") LIKE LOWER('" + like_expr + "')";
         }

         sql += ")";
      }

      if( text_search_words.size( ) > 1 )
         sql += ")";

      if( !fixed_info.empty( ) || !paging_info.empty( ) || !security_info.empty( ) )
         sql += " AND ";
   }

   if( !fixed_info.empty( ) )
   {
      for( size_t i = 0; i < fixed_info.size( ); i++ )
      {
         string next_field( fixed_info[ i ].first );
         string next_value( fixed_info[ i ].second );

         if( i > 0 )
            sql += " AND ";

         string field_type;
         bool is_sql_numeric;

         get_field_name( instance, next_field, &is_sql_numeric, &field_type );

         if( query_info.empty( ) && text_search.empty( ) )
            use_index_fields.push_back( "C_" + next_field );

         bool invert = false;
         bool has_multiple_values = false;

         string invert_prefix;

         // NOTE: If a fixed key value contains one or more "+val" suffixes then these
         // are stripped off to behave as though they are in an IN list (although "ORs"
         // are actually used in the SQL generated here).
         string::size_type pos = next_value.find( '+' );
         if( pos != string::npos )
         {
            has_multiple_values = true;

            sql += "(";

            string::size_type spos( pos + 1 );

            while( true )
            {
               string::size_type npos = next_value.find( '+', spos );

               string next_opt_value;

               if( npos == string::npos )
                  next_opt_value = next_value.substr( spos );
               else
                  next_opt_value = next_value.substr( spos, npos - spos );

               if( !next_opt_value.empty( ) && next_opt_value[ 0 ] == '!' )
                  sql += "C_" + next_field + " <> " + next_opt_value.substr( 1 );
               else
                  sql += "C_" + next_field + " = " + next_opt_value;

               sql += " OR ";

               if( npos == string::npos )
                  break;

               spos = npos + 1;
            }

            next_value.erase( pos );
         }
         else if( !next_value.empty( ) && next_value[ 0 ] == '!' )
         {
            invert = true;
            invert_prefix = "NOT ";
            next_value.erase( 0, 1 );
         }

         // NOTE: If a fixed key value ends with a '*' then the value preceeding
         // it will become a LIKE prefix and the '*' is changed to a SQL wildcard.
         bool value_is_like_prefix = false;
         if( !next_value.empty( ) && next_value[ next_value.length( ) - 1 ] == '*' )
         {
            value_is_like_prefix = true;
            next_value[ next_value.length( ) - 1 ] = '%';
         }

         string lhs, rhs;
         bool is_between = false;
         if( !value_is_like_prefix )
         {
            string::size_type pos = next_value.find( ".." );
            if( pos != string::npos )
            {
               is_between = true;
               if( is_sql_numeric )
               {
                  lhs = next_value.substr( 0, pos );
                  rhs = next_value.substr( pos + 2 );
               }
               else
               {
                  lhs = sql_quote( formatted_value( next_value.substr( 0, pos ), field_type ) );
                  rhs = sql_quote( formatted_value( next_value.substr( pos + 2 ), field_type ) );
               }
            }
         }

         if( !is_between && !is_sql_numeric )
            next_value = sql_quote( formatted_value( next_value, field_type ) );

         if( next_value == "NULL" )
            sql += "C_" + next_field + " " + invert_prefix + " IS NULL";
         else if( !value_is_like_prefix )
         {
            if( !is_between )
               sql += "C_" + next_field + ( invert ? " <> " : " = " ) + next_value;
            else
               sql += "C_" + next_field + " " + invert_prefix + "BETWEEN " + lhs + " AND " + rhs;
         }
         else
            sql += "C_" + next_field + " " + invert_prefix + "LIKE " + next_value;

         if( has_multiple_values )
            sql += ")";
      }

      if( !paging_info.empty( ) || !security_info.empty( ) )
         sql += " AND ";
   }

   if( !paging_info.empty( ) )
   {
      vector< pair< string, string > > modified_paging_info;

      for( size_t i = 0; i < paging_info.size( ); i++ )
      {
         string next_field( paging_info[ i ].first );
         string next_value( paging_info[ i ].second );

         string field_type;
         bool is_sql_numeric;

         get_field_name( instance, next_field, &is_sql_numeric, &field_type );

         if( query_info.empty( ) && text_search.empty( ) )
            use_index_fields.push_back( "C_" + next_field );

         if( !is_sql_numeric )
            next_value = sql_quote( formatted_value( next_value, field_type ) );

         modified_paging_info.push_back( make_pair( next_field, next_value ) );
      }

      sql += construct_paging_sql( modified_paging_info, is_reverse, is_inclusive );

      if( !security_info.empty( ) )
         sql += " AND ";
   }

   string security_field;
   if( !security_info.empty( ) )
   {
      // NOTE: The 'security_info' can either be just a field id (in which case the security level
      // is taken from the session variable 'e_special_var_sec') or alternatively a field id and a
      // security level value separated by a colon.
      string::size_type pos = security_info.find( ':' );

      string security_level( get_raw_session_variable( get_special_var_name( e_special_var_sec ) ) );
      if( pos != string::npos )
         security_level = security_info.substr( pos + 1 );

      security_field = security_info.substr( 0, pos );

      bool is_sql_numeric;
      get_field_name( instance, security_field, &is_sql_numeric );

      sql += "C_" + security_field + " LIKE '" + security_level + "%'";
   }

   if( !order_info.empty( ) )
   {
      sql += " ORDER BY ";

      vector< string > unique_indexes;
      instance.get_sql_unique_indexes( unique_indexes );

      set< string > sorted_unique_indexes( unique_indexes.begin( ), unique_indexes.end( ) );

      string index;
      for( size_t i = 0; i < order_info.size( ); i++ )
      {
         string next_field( order_info[ i ] );
         get_field_name( instance, next_field );

         // NOTE: As the "key" is always appended to the ordering (in case no unique index found)
         // here if a unique index is found on all but the last order field then skip this field.
         if( i == order_info.size( ) - 1 && sorted_unique_indexes.count( index ) )
            break;

         if( i > 0 )
         {
            sql += ',';
            index += ',';
         }

         sql += "C_" + next_field;
         index += "C_" + next_field;

         if( is_reverse )
            sql += " DESC";
      }
   }

   if( row_limit )
      sql += " LIMIT " + to_string( row_limit );

   // NOTE: If no query or text search info was supplied then the fixed and paging fields will be
   // checked against each table index to determine if an index covers all of them. If one is found
   // then force the query to use this index (to ensure the RDBMS optimiser does not get it wrong).
   if( !use_index_fields.empty( ) )
   {
      vector< string > indexes;
      instance.get_sql_indexes( indexes );

      vector< string > unique_indexes;
      instance.get_sql_unique_indexes( unique_indexes );

      set< string > sorted_unique_indexes( unique_indexes.begin( ), unique_indexes.end( ) );

      for( size_t i = use_index_fields.size( ); i < order_info.size( ); i++ )
      {
         string next_field( order_info[ i ] );
         get_field_name( instance, next_field );

         use_index_fields.push_back( "C_" + next_field );
      }

      bool found = false;
      string index_to_use;

      for( size_t i = 0; i < indexes.size( ); i++ )
      {
         vector< string > index_fields;
         split( indexes[ i ], index_fields );

         // NOTE: As the "key" is always appended to the ordering (in case no unique index found)
         // the key is appended to unique indexes so the field matching will operate as expected.
         if( sorted_unique_indexes.count( indexes[ i ] ) )
            index_fields.push_back( "C_Key_" );

         if( index_fields.size( ) == use_index_fields.size( ) )
         {
            for( size_t j = 0; j < use_index_fields.size( ); j++ )
            {
               if( index_fields[ j ] != use_index_fields[ j ] )
                  break;

               if( j == use_index_fields.size( ) - 1 )
                  found = true;
            }
         }

         if( found )
         {
            index_to_use = "I_" + string( instance.get_module_name( ) ) + "_"
             + string( instance.get_class_name( ) ) + "_" + ( i < 10 ? "0" : "" ) + to_string( i );
            break;
         }
      }

      if( !index_to_use.empty( ) )
         sql_fields_and_table += " USE INDEX (" + index_to_use + ")";
   }

   return sql_fields_and_table + sql;
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

      string blockchain( get_raw_session_variable( get_special_var_name( e_special_var_blockchain ) ) );

      if( !blockchain.empty( ) && !storage_locked_for_admin( ) )
      {
         string filename( handler.get_name( ) + ".txs.log" );

         if( !file_exists( filename ) )
            outf << "#" << c_block_prefix << ' ' << c_unconfirmed_revision << '\n';
      }

      for( size_t i = 0; i < gtp_session->sql_undo_statements.size( ); i++ )
         outf << escaped( gtp_session->sql_undo_statements[ i ], "\"", c_nul, "rn\r\n" ) << '\n';

      outf.flush( );
      if( !outf.good( ) )
         throw runtime_error( "*** unexpected error occurred writing to undo sql ***" );

      outf.close( );
   }
}

void append_transaction_log_command( storage_handler& handler,
 bool log_even_when_locked = false, size_t load_module_id = 0, int32_t use_tx_id = 0 )
{
   if( !gtp_session->transaction_log_command.empty( )
    && ( log_even_when_locked || handler.get_alternative_log_file( ) || !handler.get_is_locked_for_admin( ) ) )
   {
      string log_filename( handler.get_name( ) );
      log_filename += ".log";

      bool is_new = false;
      if( !file_exists( log_filename ) )
         is_new = true;

      bool append_to_log_blob_files = false;

      if( has_files_area_tag( c_ciyam_tag, e_file_type_list ) )
         append_to_log_blob_files = true;

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
      {
         log_file << c_storage_identity_tx_id << handler.get_root( ).identity << '\n';

         if( append_to_log_blob_files )
            tx_log_lines.push_back( c_storage_identity_tx_id + handler.get_root( ).identity + "\n" );
      }

      int32_t tx_id;

      bool use_init_tx_id = false;
      string init_log_id( get_raw_session_variable( get_special_var_name( e_special_var_init_log_id ) ) );

      if( init_log_id == "1" || init_log_id == c_true )
         use_init_tx_id = true;

      // NOTE: When log file is truncated during a backup no transaction is active, so
      // change the tx id to 'initial' to ensure that a restore is able to understand
      // it's dealing with a partial log file.
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

         if( identity.next_id == identity.ceiling )
         {
            identity.ceiling += c_identity_burn;

            // NOTE: Store the "ceiling" rather than "next_id" to avoid having to
            // perform extra I/O for every transaction. If storage termination is
            // to occur normally then the actual "next_id" will be written and no
            // identity value is lost. If for some reason normal termination does
            // not occur then up to the "burn" number of identities will be lost.
            restorable< int32_t > tmp_identity( identity.next_id, identity.ceiling );

            ods_file_system ofs( *gtp_session->p_storage_handler->get_ods( ) );
            ofs.store_as_text_file( c_storable_file_name_log_id, identity.next_id );
         }

         tx_id = ++identity.next_id;
      }

      vector< string > lines;
      raw_split( gtp_session->transaction_log_command, lines, '\n' );

      string next_line;
      for( size_t i = 0; i < lines.size( ); i++ )
      {
         next_line = "[" + to_string( tx_id ) + "]" + lines[ i ] + "\n";

         log_file << next_line;

         if( append_to_log_blob_files )
            tx_log_lines.push_back( next_line );
      }

      bool is_restoring = handler.get_is_locked_for_admin( );

      if( !get_session_variable( get_special_var_name( e_special_var_package_install_extra ) ).empty( ) )
         is_restoring = true;

      if( append_to_log_blob_files )
         append_transaction_log_lines_to_blob_files( handler.get_name( ) + ".log", tx_log_lines, is_restoring );

      log_file.flush( );
      if( !log_file.good( ) )
         throw runtime_error( "*** unexpected error occurred writing to transaction log ***" );
   }

   gtp_session->transaction_log_command.erase( );
}

string_container g_strings;

string g_sid;
string g_domain;
string g_reg_key;
string g_timezone;
string g_web_root;

string g_set_trace;

bool g_use_https = false;
bool g_using_ssl = false;

string g_gpg_password;
string g_pem_password;
string g_rpc_password;
string g_sql_password;

string g_default_storage;

unsigned int g_session_timeout = 0;

unsigned int g_max_peers = c_default_max_peers;
unsigned int g_max_user_limit = c_default_max_user_limit;

bool g_script_reconfig = false;

set< string > g_accepted_ip_addrs;
set< string > g_rejected_ip_addrs;
set< string > g_accepted_peer_ip_addrs;
set< string > g_rejected_peer_ip_addrs;

map< int, string > g_blockchains;
map< string, int > g_blockchain_ids;

map< string, string > g_initial_peer_ips;

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
int g_smtp_max_send_attempts = 1;
int64_t g_smtp_max_attached_data = INT64_C( 100000 );

typedef map< string, external_client > external_client_container;
typedef external_client_container::iterator external_client_iterator;
typedef external_client_container::const_iterator external_client_const_iterator;
typedef external_client_container::value_type external_client_value_type;

external_client_container g_external_client_info;

#include "sid.enc"

void sid_hash( string& s )
{
   guard g( g_mutex );

   sha256 hash1( string( c_salt_value ) + get_sid( ) );

   // NOTE: Fix the initial size so that no temporary string is used.
   s.resize( c_sha256_digest_size * 2 );

   hash1.get_digest_as_string( s );

   sha1 hash2( s );

   hash2.get_digest_as_string( s );
}

struct script_info
{
   string filename;
   string arguments;
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
         string::size_type pos = args.find( ' ' );
         args.erase( 0, pos );
      }

      if( !args.empty( ) )
         os << ' ' << args;

      os << '\n';
   }
}

void read_server_configuration( )
{
   ifstream inpf( c_server_config_file );

   if( inpf )
   {
      sio_reader reader( inpf );

      g_domain = reader.read_opt_attribute( c_attribute_domain, "localhost" );

      string ip_addrs( reader.read_opt_attribute( c_attribute_ip_addrs ) );
      if( !ip_addrs.empty( ) )
         split( ip_addrs, g_accepted_ip_addrs, ' ' );

      string na_addrs( reader.read_opt_attribute( c_attribute_na_addrs ) );
      if( !na_addrs.empty( ) )
         split( na_addrs, g_rejected_ip_addrs, ' ' );

      g_reg_key = upper( reader.read_opt_attribute( c_attribute_reg_key ) );

      g_timezone = upper( reader.read_opt_attribute( c_attribute_timezone ) );

      g_web_root = reader.read_attribute( c_attribute_web_root );

      if( !g_web_root.empty( ) )
      {
         if( g_web_root[ 0 ] == '$' )
         {
            const char* p_env = getenv( g_web_root.substr( 1 ).c_str( ) );
            g_web_root = string( p_env ? p_env : "" );
         }
         else if( g_web_root[ 0 ] == '%'
          && g_web_root.size( ) > 2 && g_web_root[ g_web_root.size( ) - 1 ] == '%' )
         {
            const char* p_env = getenv( g_web_root.substr( 1, g_web_root.size( ) - 2 ).c_str( ) );
            g_web_root = string( p_env ? p_env : "" );
         }
#ifdef _WIN32
         replace( g_web_root, "\\", "/" );
#endif
      }

      g_max_peers = atoi( reader.read_opt_attribute(
       c_attribute_max_peers, to_string( c_default_max_peers ) ).c_str( ) );

      g_set_trace = reader.read_opt_attribute( c_attribute_set_trace );

      if( !g_set_trace.empty( ) )
      {
         int trace_flags;
         istringstream isstr( g_set_trace );
         isstr >> hex >> trace_flags;

         set_trace_flags( get_trace_flags( ) | trace_flags );
      }

      g_use_https = ( lower( reader.read_opt_attribute( c_attribute_use_https, c_false ) ) == c_true );

      string all_blockchains( reader.read_opt_attribute( c_attribute_blockchains ) );

      if( !all_blockchains.empty( ) )
      {
         vector< string > blockchains;
         split( all_blockchains, blockchains, ' ' );

         for( size_t i = 0; i < blockchains.size( ); i++ )
         {
            string next( blockchains[ i ] );
            string::size_type pos = next.find( '=' );
            if( pos == string::npos )
               throw runtime_error( "invalid format '" + next + "' for blockchains entry" );

            if( g_blockchain_ids.count( next.substr( pos + 1 ) ) )
               throw runtime_error( "invalid repeated blockchain id: " + next.substr( pos + 1 ) );

            g_blockchains.insert( make_pair( atoi( next.substr( 0, pos ).c_str( ) ), next.substr( pos + 1 ) ) );
            g_blockchain_ids.insert( make_pair( next.substr( pos + 1 ), atoi( next.substr( 0, pos ).c_str( ) ) ) );
         }
      }

      g_max_sessions = atoi( reader.read_opt_attribute(
       c_attribute_max_sessions, to_string( c_max_sessions_default ) ).c_str( ) );

      g_gpg_password = reader.read_opt_attribute( c_attribute_gpg_password );
      g_pem_password = reader.read_opt_attribute( c_attribute_pem_password );
      g_rpc_password = reader.read_opt_attribute( c_attribute_rpc_password );
      g_sql_password = reader.read_opt_attribute( c_attribute_sql_password );

      g_default_storage = reader.read_opt_attribute( c_attribute_default_storage );
      set_system_variable( get_special_var_name( e_special_var_storage ), g_default_storage );

      string peer_ips_direct( reader.read_opt_attribute( c_attribute_peer_ips_direct ) );

      if( !peer_ips_direct.empty( ) )
      {
         vector< string > ips_direct;
         split( peer_ips_direct, ips_direct, ' ' );

         for( size_t i = 0; i < ips_direct.size( ); i++ )
         {
            string next( ips_direct[ i ] );
            string::size_type pos = next.find( '=' );
            if( pos == string::npos )
               throw runtime_error( "invalid format '" + next + "' for peer_ips_direct entry" );

            g_initial_peer_ips.insert( make_pair( next.substr( 0, pos ), next.substr( pos + 1 ) ) );
         }
      }

      string peer_ips_permit( reader.read_opt_attribute( c_attribute_peer_ips_permit ) );
      if( !peer_ips_permit.empty( ) )
         split( peer_ips_permit, g_accepted_peer_ip_addrs, ' ' );

      string peer_ips_reject( reader.read_opt_attribute( c_attribute_peer_ips_reject ) );
      if( !peer_ips_reject.empty( ) )
         split( peer_ips_reject, g_rejected_peer_ip_addrs, ' ' );

      g_script_reconfig = ( lower( reader.read_opt_attribute( c_attribute_script_reconfig, c_false ) ) == c_true );

      g_session_timeout = atoi( reader.read_opt_attribute( c_attribute_session_timeout, "0" ).c_str( ) );

      g_max_storage_handlers = atoi( reader.read_opt_attribute(
       c_attribute_max_storage_handlers, to_string( c_max_storage_handlers_default ) ).c_str( ) ) + 1;

      // NOTE: Use "unformat_bytes" here as well so 10K (instead of 10000) can be used in the config file.
      g_files_area_item_max_num = ( size_t )unformat_bytes( reader.read_opt_attribute(
       c_attribute_files_area_item_max_num, to_string( c_files_area_item_max_num_default ) ).c_str( ) );

      g_files_area_item_max_size = ( size_t )unformat_bytes( reader.read_opt_attribute(
       c_attribute_files_area_item_max_size, to_string( c_files_area_item_max_size_default ) ).c_str( ) );

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

         string max_attached_data = reader.read_opt_attribute( c_attribute_max_attached_data );
         if( !max_attached_data.empty( ) )
            g_smtp_max_attached_data = unformat_bytes( max_attached_data );

         reader.finish_section( c_section_smtp );
      }

      reader.finish_section( c_section_email );

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

void fetch_instance_from_row_cache( class_base& instance, bool skip_after_fetch )
{
   class_base_accessor instance_accessor( instance );

   if( instance_accessor.row_cache( ).empty( ) )
      throw runtime_error( "unexpected empty row cache" );

   instance_accessor.clear( );

   instance_accessor.set_key( instance_accessor.row_cache( )[ 0 ][ 0 ], true );
   instance_accessor.set_version( from_string< uint16_t >( instance_accessor.row_cache( )[ 0 ][ 1 ] ) );
   instance_accessor.set_revision( from_string< uint64_t >( instance_accessor.row_cache( )[ 0 ][ 2 ] ) );

   instance_accessor.set_original_revision( instance.get_revision( ) );
   instance_accessor.set_original_identity( instance_accessor.row_cache( )[ 0 ][ 3 ] );

   if( instance.get_persistence_type( ) == 0 ) // i.e. SQL persistence
   {
      TRACE_LOG( TRACE_SQLSTMTS, "(row cache for '" + instance.get_class_id( ) + "')" );

      const map< int, int >& fields( instance_accessor.select_fields( ) );
      const vector< int >& columns( instance_accessor.select_columns( ) );

      TRACE_LOG( TRACE_SQLCLSET, "(from row cache)" );

      for( int i = 4; i < instance_accessor.row_cache( )[ 0 ].size( ); i++ )
      {
         if( !fields.count( columns[ i - 4 ] ) )
            throw runtime_error( "unexpected field # not found for column #" + to_string( i - 4 ) );

         int fnum = fields.find( columns[ i - 4 ] )->second;

         TRACE_LOG( TRACE_SQLCLSET, "setting field #" + to_string( fnum + 1 ) + " to " + instance_accessor.row_cache( )[ 0 ][ i ] );
         instance.set_field_value( fnum, instance_accessor.row_cache( )[ 0 ][ i ] );
      }
   }
   else if( instance.get_persistence_type( ) == 1 ) // i.e. ODS global persistence
   {
      for( int i = 4; i < instance_accessor.row_cache( )[ 0 ].size( ); i++ )
      {
         if( instance_accessor.field_nums( ).size( ) < ( i - 4 ) )
            throw runtime_error( "unexpected field # not found for for column #" + to_string( i - 4 ) );

         int fnum = instance_accessor.field_nums( )[ i - 4 ];

         instance.set_field_value( fnum, instance_accessor.row_cache( )[ 0 ][ i ] );
      }
   }

   instance_accessor.row_cache( ).pop_front( );

   instance_accessor.after_fetch_from_db( );

   if( !skip_after_fetch )
      instance_accessor.perform_after_fetch( );
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

size_t g_trace_flags;

string instance_op_name( instance_op op )
{
   string retval( "unknown op " + to_string( op ) );

   switch( op )
   {
      case e_instance_op_none:
      retval = "none";
      break;

      case e_instance_op_review:
      retval = "review";
      break;

      case e_instance_op_create:
      retval = "create";
      break;

      case e_instance_op_update:
      retval = "update";
      break;

      case e_instance_op_destroy:
      retval = "destroy";
      break;
   }

   return retval;
}

string instance_op_name( class_base::op_type op )
{
   string retval( "unknown op " + to_string( op ) );

   switch( op )
   {
      case class_base::e_op_type_none:
      retval = "none";
      break;

      case class_base::e_op_type_review:
      retval = "review";
      break;

      case class_base::e_op_type_create:
      retval = "create";
      break;

      case class_base::e_op_type_update:
      retval = "update";
      break;

      case class_base::e_op_type_destroy:
      retval = "destroy";
      break;
   }

   return retval;
}

}

size_t get_trace_flags( )
{
   return g_trace_flags;
}

void set_trace_flags( size_t flags )
{
   g_trace_flags = flags;
}

void trace_flags( unsigned flags )
{
   set_trace_flags( flags );
}

void list_trace_flags( vector< string >& flag_names )
{
   flag_names.push_back( "commands" ); // TRACE_COMMANDS
   flag_names.push_back( "sqlstmts" ); // TRACE_SQLSTMTS
   flag_names.push_back( "classops" ); // TRACE_CLASSOPS
   flag_names.push_back( "mods_gen" ); // TRACE_MODS_GEN
   flag_names.push_back( "sql_sets" ); // TRACE_SQLCLSET
   flag_names.push_back( "fld_vals" ); // TRACE_FLD_VALS
   flag_names.push_back( "lock_ops" ); // TRACE_LOCK_OPS
   flag_names.push_back( "ctr_dtrs" ); // TRACE_CTR_DTRS
   flag_names.push_back( "sessions" ); // TRACE_SESSIONS
   flag_names.push_back( "mail_ops" ); // TRACE_MAIL_OPS
   flag_names.push_back( "pdf_vals" ); // TRACE_PDF_VALS
   flag_names.push_back( "sock_ops" ); // TRACE_SOCK_OPS
   flag_names.push_back( "core_fls" ); // TRACE_CORE_FLS
   flag_names.push_back( "sync_ops" ); // TRACE_SYNC_OPS
}

void log_trace_message( int flag, const string& message )
{
   guard g( g_trace_mutex );

   string type( "unknown" );
   switch( flag )
   {
      case TRACE_COMMANDS:
      type = "command";
      break;

      case TRACE_SQLSTMTS:
      type = "sqlstmt";
      break;

      case TRACE_CLASSOPS:
      type = "classop";
      break;

      case TRACE_MODS_GEN:
      type = "mod_gen";
      break;

      case TRACE_SQLCLSET:
      type = "sql_set";
      break;

      case TRACE_FLD_VALS:
      type = "fld_val";
      break;

      case TRACE_LOCK_OPS:
      type = "lock_op";
      break;

      case TRACE_CTR_DTRS:
      type = "ctr_dtr";
      break;

      case TRACE_SESSIONS:
      type = "session";
      break;

      case TRACE_MAIL_OPS:
      type = "mail_op";
      break;

      case TRACE_PDF_VALS:
      type = "pdf_val";
      break;

      case TRACE_SOCK_OPS:
      type = "sock_op";
      break;

      case TRACE_CORE_FLS:
      type = "core_fs";
      break;

      case TRACE_SYNC_OPS:
      type = "sync_op";
      break;

      case TRACE_ANYTHING:
      type = "general";
      break;
   }

   ofstream outf( "ciyam_server.log", ios::out | ios::app );

   outf << '[' << date_time::local( ).as_string( true, false ) << "] [" << setw( 6 )
    << setfill( '0' ) << ( gtp_session ? gtp_session->id : 0 ) << "] [" << type << "] " << message << '\n';
}

void log_trace_string( int flag, const char* p_message )
{
   log_trace_message( flag, p_message );
}

void trace_mutex::pre_acquire( const guard* p_guard, const char* p_msg )
{
   string extra;

   if( p_msg )
      extra = " " + string( p_msg );

   TRACE_LOG( TRACE_SYNC_OPS, "pre_acquire: mutex = "
    + to_string( this ) + ", guard = " + to_string( p_guard ) + extra );
}

void trace_mutex::post_acquire( const guard* p_guard, const char* p_msg )
{
   string extra;

   if( p_msg )
      extra = " " + string( p_msg );

   TRACE_LOG( TRACE_SYNC_OPS, "post_acquire: mutex = "
    + to_string( this ) + ", guard = " + to_string( p_guard ) + extra );
}

void trace_mutex::has_released( const guard* p_guard, const char* p_msg )
{
   string extra;

   if( p_msg )
      extra = " " + string( p_msg );

   TRACE_LOG( TRACE_SYNC_OPS, "has_released: mutex = "
    + to_string( this ) + ", guard = " + to_string( p_guard ) + extra );
}

void register_listener( int port, const char* p_info )
{
   g_listeners.insert( make_pair( port, p_info ) );
}

void unregister_listener( int port )
{
   g_listeners.erase( port );
}

listener_registration::listener_registration( int port, const string& info )
 :
 port( port )
{
   guard g( g_mutex );

   register_listener( port, info.c_str( ) );
}

listener_registration::~listener_registration( )
{
   guard g( g_mutex );

   unregister_listener( port );
}

bool has_registered_listener( int port )
{
   guard g( g_mutex );

   return g_listeners.count( port );
}

void list_listeners( ostream& os )
{
   guard g( g_mutex );

   for( listener_const_iterator lci = g_listeners.begin( ); lci != g_listeners.end( ); ++lci )
      os << lci->first << ' ' << lci->second << '\n';
}

void init_globals( )
{
   guard g( g_mutex );

   try
   {
      if( !file_exists( c_server_sid_file ) )
      {
         g_sid = upper( uuid( ).as_string( ) );
         write_file( c_server_sid_file, g_sid );
      }
      else
         g_sid = buffer_file( c_server_sid_file );

      read_server_configuration( );

      init_ciyam_ods( );

      init_files_area( );

      check_timezone_info( );

#ifndef _WIN32
      set_system_variable( get_special_var_name( e_special_var_os ), "Linux" );
#else
      set_system_variable( get_special_var_name( e_special_var_os ), "Windows" );
#endif

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
      if( file_exists( "ciyam_server.pem" ) )
      {
         string password( c_default_pem_password );
         if( !g_pem_password.empty( ) )
            password = get_pem_password( );

         init_ssl( "ciyam_server.pem", password.c_str( ), 0, true );
         g_using_ssl = true;
      }
#endif
   }
   catch( exception& x )
   {
      TRACE_LOG( TRACE_ANYTHING, x.what( ) );
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

   term_files_area( );

   term_ciyam_ods( );

   g_scripts.clear( );

   g_strings.clear( );

#ifdef SSL_SUPPORT
   if( g_using_ssl )
      term_ssl( );
#endif
}

void check_timezone_info( )
{
   if( timezones_file_has_changed( ) )
      setup_timezones( );
}

string get_string( const string& key )
{
   string str( key );

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

string get_identity( bool prepend_sid, bool append_max_user_limit )
{
   guard g( g_mutex );

   if( !prepend_sid && !append_max_user_limit )
      return g_reg_key;

   string s( g_reg_key );

   if( prepend_sid )
   {
      // NOTE: Reload just in case it has been overwritten.
      g_sid = buffer_file( c_server_sid_file );

      string suffix( s );

      sid_hash( s );

      if( !suffix.empty( ) )
         s += "-" + suffix;
   }

   if( append_max_user_limit )
      s += ":" + to_string( g_max_user_limit );

   return s;
}

void set_identity( const string& identity_info )
{
   guard g( g_mutex );

   string::size_type pos = identity_info.find( '-' );
   if( pos == string::npos )
      g_reg_key.erase( );
   else
      g_reg_key = identity_info.substr( pos + 1 );

   string s( identity_info.substr( 0, pos ) );
   set_sid( s );
}

string get_checksum( const string& data, bool use_reg_key )
{
   guard g( g_mutex );

   string prefix( !use_reg_key ? get_sid( ) : g_reg_key );

   sha1 hash( prefix + data );

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

void get_initial_peer_ips( map< string, string >& ips )
{
   ips = g_initial_peer_ips;
}

bool get_is_accepted_ip_addr( const string& ip_addr )
{
   return ( g_rejected_ip_addrs.empty( ) || g_rejected_ip_addrs.count( ip_addr ) == 0 )
    && ( g_accepted_ip_addrs.empty( ) || ( g_accepted_ip_addrs.count( ip_addr ) > 0 ) );
}

bool get_is_accepted_peer_ip_addr( const string& ip_addr )
{
   guard g( g_mutex );

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

void get_blockchains( map< int, string >& blockchains )
{
   guard g( g_mutex );

   blockchains = g_blockchains;
}

int get_blockchain_port( const string& blockchain )
{
   guard g( g_mutex );

   if( !g_blockchain_ids.count( blockchain ) )
      throw runtime_error( "unknown blockchain: " + blockchain );

   return g_blockchain_ids[ blockchain ];
}

bool get_is_known_blockchain( const string& blockchain )
{
   guard g( g_mutex );

   return g_blockchain_ids.count( blockchain );
}

string get_blockchain_for_port( int port )
{
   guard g( g_mutex );

   string retval;

   if( g_blockchains.count( port ) )
      retval = g_blockchains[ port ];

   return retval;
}

void register_blockchain( int port, const string& blockchain )
{
   guard g( g_mutex );

   if( g_blockchain_ids.count( blockchain ) && g_blockchain_ids[ blockchain ] != port )
      throw runtime_error( "blockchain "
       + blockchain + " is already registered to port " + to_string( g_blockchain_ids[ blockchain ] ) );

   g_blockchains[ port ] = blockchain;
   g_blockchain_ids[ blockchain ] = port;
}

bool get_using_ssl( )
{
   return g_using_ssl;
}

size_t get_files_area_item_max_num( )
{
   return g_files_area_item_max_num;
}

size_t get_files_area_item_max_size( )
{
   return g_files_area_item_max_size;
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
       make_pair( c_str_parm_external_service_unavailable_symbol, ext_key ) ) );
}

string decrypt_data( const string& data, bool no_ssl, bool no_salt, bool hash_only )
{
   string key, retval;

   // NOTE: If "no_salt" was specified then an empty key is used (so "no_salt" should
   // only ever be set "true" for the purpose of performing simple regression tests).
   if( !no_salt )
   {
      sid_hash( key );

      if( !hash_only )
         key += string( c_salt_value );
   }

   retval = data_decrypt( data, key, !no_ssl );

   clear_key( key );

   return retval;
}

string encrypt_data( const string& data, bool no_ssl, bool no_salt, bool hash_only )
{
   string key, retval;

   // NOTE: (see above)
   if( !no_salt )
   {
      sid_hash( key );

      if( !hash_only )
         key += string( c_salt_value );
   }

   retval = data_encrypt( data, key, !no_ssl, !no_salt );

   clear_key( key );

   return retval;
}

string totp_secret_key( const string& unique )
{
   string key, retval;

   string crypt_key( get_raw_session_variable( get_special_var_name( e_special_var_crypt_key ) ) );

   if( crypt_key.empty( ) )
      sid_hash( key );

   retval = get_totp_secret( unique, crypt_key.empty( ) ? key : crypt_key );

   if( crypt_key.empty( ) )
      clear_key( key );

   return retval;
}

int exec_system( const string& cmd, bool async, bool delay )
{
   int rc = 0;

   string async_cmd( cmd );

#ifdef _WIN32
   async_cmd = "start /min " + async_cmd;
#else
   async_cmd += " &";
#endif

   // NOTE: For security against potentially malicious module code only permit system calls
   // from the autoscript session, via "run_script", or from either the "Meta" or "default"
   // (or "ciyam") storages.
   if( gtp_session && !gtp_session->running_script
    && gtp_session->p_storage_handler->get_name( ) != "Meta"
    && gtp_session->p_storage_handler->get_name( ) != "ciyam"
    && gtp_session->p_storage_handler->get_name( ) != c_default_storage_name )
      throw runtime_error( "invalid exec_system: " + cmd );

   string async_var( get_raw_session_variable( get_special_var_name( e_special_var_allow_async ) ) );

   // NOTE: The session variable @allow_async can be used to force non-async execution.
   if( async_var == "0" || async_var == c_false )
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
         gtp_session->async_or_delayed_system_commands.push_back( async ? async_cmd : cmd );

         if( !gtp_session->async_or_delayed_temp_file.empty( ) )
            gtp_session->async_or_delayed_temp_files.push_back( gtp_session->async_or_delayed_temp_file );

         return rc;
      }
   }

   TRACE_LOG( TRACE_SESSIONS, async ? async_cmd : cmd );

   rc = system( async ? async_cmd.c_str( ) : cmd.c_str( ) );

   // NOTE: If the script had an error and the caller should throw this as an error then do so.
   string check_script_error(
    get_raw_session_variable( get_special_var_name( e_special_var_check_script_error ) ) );

   if( check_script_error == "1" || check_script_error == c_true )
   {
      set_session_variable( get_special_var_name( e_special_var_check_script_error ), "" );

      if( gtp_session && !gtp_session->async_or_delayed_temp_file.empty( ) )
      {
         string value( get_raw_system_variable( gtp_session->async_or_delayed_temp_file ) );

         set_system_variable( gtp_session->async_or_delayed_temp_file, "" );

         if( !value.empty( ) && value != string( "1" ) )
         {
            // NOTE: If the error starts with '@' then assume that it is actually
            // intended to be an execute "return" message rather than an error.
            if( value[ 0 ] == '@' )
               set_session_variable( get_special_var_name( e_special_var_return ), value.substr( 1 ) );
            else
               throw runtime_error( value );
         }
      }
   }

   return rc;
}

int run_script( const string& script_name, bool async, bool delay, bool no_logging )
{
   int rc = -1;

   if( get_script_reconfig( ) && scripts_file_has_changed( ) )
   {
      read_script_info( );
      TRACE_LOG( TRACE_ANYTHING, "[manuscript.sio] updated" );
   }

   if( !g_scripts.count( script_name ) )
      throw runtime_error( "unknown script '" + script_name + "'" );

   string filename( g_scripts[ script_name ].filename );
   bool is_script = ( filename == c_script_dummy_filename );

   string arguments( process_script_args( g_scripts[ script_name ].arguments ) );

   auto_ptr< restorable< bool > > ap_running_script;

   if( gtp_session )
   {
      gtp_session->async_or_delayed_temp_file.erase( );
      ap_running_script.reset( new restorable< bool >( gtp_session->running_script, true ) );
   }

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

         outf << "<<" << arguments << endl;

         outf.flush( );
         if( !outf.good( ) )
            throw runtime_error( "*** unexpected bad file '" + args_file + "' in run_script ***" );
      }

      if( gtp_session )
      {
         gtp_session->async_or_delayed_temp_file = args_file;

         string check_script_error(
          get_raw_session_variable( get_special_var_name( e_special_var_check_script_error ) ) );

         // NOTE: If the script is intended to be synchronous and "no_logging" argument is set true
         // then the first error that occurs in the the external script (or scripts if multiple are
         // run using "delay") will thrown as an exception after the calling session's "system" (in
         // the case of delayed scripts that will occur at the end of transaction commit). Thus set
         // special variables for that here. As "ciyam_client" will set a session variable for each
         // script to the "script args" unique file name (via the "-args_file" command-line option)
         // a system variable with this same name is being set here (to the value "1"). If an error
         // occurs in a command being executed by the script and system variable with the same name
         // as the "args_file" session variable (and whose value is still "1") (i.e. in the case of
         // multiple delayed scripts will only record the first such error) then this value will be
         // changed to that of the error message.
         if( !async && no_logging
          && check_script_error != "0" && check_script_error != c_false )
         {
            set_system_variable( args_file, "1" );
            set_session_variable( get_special_var_name( e_special_var_check_script_error ), "1" );
         }
      }

      string script_args( args_file );

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
         string errors_only( get_raw_session_variable( get_special_var_name( e_special_var_errors_only ) ) );

         if( errors_only == "1" || errors_only == "true" )
            script_args = "-log_on_error " + script_args;
      }

      // NOTE: For cases where one script may end up calling numerous others (i.e.
      // such as a scan across records) this special session variable is available
      // to prevent excess log entries appearing in the script log file.
      string quiet( get_raw_session_variable( get_special_var_name( e_special_var_quiet ) ) );

      if( quiet != "1" && quiet != "true" )
         script_args += " " + script_name;

#ifdef _WIN32
      rc = exec_system( "script " + script_args, async, delay );
#else
      rc = exec_system( "./script " + script_args, async, delay );
#endif
   }
   else
   {
      string cmd_and_args( filename );

#ifndef _WIN32
      if( cmd_and_args.find( '/' ) == string::npos )
         cmd_and_args = "./" + cmd_and_args;
#endif

      if( !arguments.empty( ) )
         cmd_and_args += " " + arguments;

      rc = exec_system( cmd_and_args, async, delay );
   }

   return rc;
}

void list_scripts( const string& pattern, ostream& os )
{
   output_script_info( pattern, os );
}

string process_script_args( const string& raw_args )
{
   string retval;

   if( !raw_args.empty( ) )
   {
      vector< string > all_args;
      split( raw_args, all_args, ' ' );

      for( size_t i = 0; i < all_args.size( ); i++ )
      {
         string next_arg( all_args[ i ] );

         if( !next_arg.empty( ) && next_arg[ 0 ] == '@' )
            next_arg = get_raw_session_variable( next_arg );

         if( !next_arg.empty( ) )
            next_arg = escaped_shell_arg( next_arg );

         if( !retval.empty( ) )
            retval += " ";

         if( next_arg.empty( ) )
            next_arg = "\"\"";

         retval += next_arg;
      }
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

      bool found_start = false;
      size_t finish_line = 0;
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

void generate_new_script_sio_files( )
{
   generate_new_script_sio( true );
   generate_new_script_sio( false );
}

void init_session( command_handler& cmd_handler,
 bool is_peer_session, const string* p_ip_addr, const string* p_blockchain, int port )
{
   guard g( g_mutex );

   gtp_session = 0;
   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( !g_sessions[ i ] )
      {
         g_sessions[ i ] = new session( ++g_next_session_id,
          i, cmd_handler, g_storage_handlers[ 0 ], is_peer_session, p_ip_addr, p_blockchain );

         gtp_session = g_sessions[ i ];
         ods::instance( 0, true );

         break;
      }
   }

   if( !gtp_session )
      throw runtime_error( "max. permitted concurrent sessions already active" );

   set_default_session_variables( port );
}

void term_session( )
{
   guard g( g_mutex );

   if( gtp_session )
   {
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

size_t session_id( )
{
   size_t rc = 0;

   if( gtp_session )
      rc = gtp_session->id;

   return rc;
}

bool has_session_with_ip_addr( const string& ip_addr )
{
   guard g( g_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && g_sessions[ i ]->ip_addr == ip_addr )
         return true;
   }

   return false;
}

string get_random_same_port_peer_ip_addr( const string& empty_value )
{
   guard g( g_mutex );

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

void list_sessions( ostream& os, bool inc_dtms )
{
   guard g( g_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] )
      {
         os << g_sessions[ i ]->id;

         if( gtp_session && gtp_session->id == g_sessions[ i ]->id )
            os << '*';
         else if( g_condemned_sessions.count( g_sessions[ i ]->id )
          || gtp_session->condemned_sessions.count( g_sessions[ i ]->id ) )
            os << '~';

         if( inc_dtms )
         {
            os << ' ' << g_sessions[ i ]->dtm_created.as_string( true, false )
             << ' ' << g_sessions[ i ]->dtm_last_cmd.as_string( true, false );
         }

         os << ' ' << g_sessions[ i ]->last_cmd << ' ';

         if( !g_sessions[ i ]->is_peer_session )
            os << g_sessions[ i ]->p_storage_handler->get_name( );
         else
            os << ( g_sessions[ i ]->blockchain.empty( ) ? string( "(peer" ) : g_sessions[ i ]->blockchain );

         if( g_sessions[ i ]->p_storage_handler->get_is_locked_for_admin( ) )
            os << '*';

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
            // the more common non-logged operations such as "fetch".
            string::size_type pos = uid.find( ':' );

            if( pos != string::npos )
               uid = '"' + uid.substr( pos + 1 ) + '"';
         }

         os << ' ' << uid;

         if( !g_sessions[ i ]->is_peer_session )
            os << ' ' << g_sessions[ i ]->instance_registry.size( ) << ':' << g_sessions[ i ]->next_handle;
         else
            os << ' ' << g_sessions[ i ]->peer_files_uploaded << ':' << g_sessions[ i ]->peer_bytes_uploaded;

         if( !g_sessions[ i ]->is_peer_session )
            os << ' ' << g_sessions[ i ]->sql_count << ':' << g_sessions[ i ]->cache_count;
         else
            os << ' ' << g_sessions[ i ]->peer_files_downloaded << ':' << g_sessions[ i ]->peer_bytes_downloaded;

         os << ' ' << g_sessions[ i ]->session_commands_executed;

         if( g_sessions[ i ]->is_captured )
            os << '*';

         os << '\n';
      }
   }
}

command_handler& get_session_command_handler( )
{
   if( !gtp_session )
      throw runtime_error( "no session to return command handler for" );

   return gtp_session->cmd_handler;
}

session_file_buffer_access::session_file_buffer_access( )
 :
 size( 0 ),
 p_buffer( 0 )
{
   guard g( g_mutex );

   if( !gtp_session || gtp_session->buffer_is_locked )
      throw runtime_error( "unable to access session buffer" );

   unsigned int bufsize = get_files_area_item_max_size( ) * c_max_file_buffer_expansion;

   if( gtp_session->ap_buffer.get( ) )
      memset( gtp_session->ap_buffer.get( ), 0, bufsize );
   else
      gtp_session->ap_buffer.reset( new unsigned char[ bufsize ] );

   gtp_session->buffer_is_locked = true;

   size = bufsize;
   p_buffer = gtp_session->ap_buffer.get( );
}

session_file_buffer_access::~session_file_buffer_access( )
{
   guard g( g_mutex );

   gtp_session->buffer_is_locked = false;
}

void increment_peer_files_uploaded( int64_t bytes )
{
   guard g( g_mutex );

   if( gtp_session )
   {
      ++gtp_session->peer_files_uploaded;
      gtp_session->peer_bytes_uploaded += bytes;
   }
}

void increment_peer_files_downloaded( int64_t bytes )
{
   guard g( g_mutex );

   if( gtp_session )
   {
      ++gtp_session->peer_files_downloaded;
      gtp_session->peer_bytes_downloaded += bytes;
   }
}

void increment_session_commands_executed( )
{
   guard g( g_mutex );

   if( gtp_session )
      ++gtp_session->session_commands_executed;
}

void set_slowest_if_applicable( )
{
   guard g( g_mutex );

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

void set_last_session_cmd_and_hash( const string& cmd, const string& parameter_info )
{
   guard g( g_mutex );

   if( gtp_session )
   {
      gtp_session->last_cmd = cmd;
      gtp_session->dtm_last_cmd = date_time::local( );

      string::size_type pos = parameter_info.find( cmd );
      if( pos != 0 )
         pos = 0;
      else
         pos += cmd.length( );

      string s( cmd + parameter_info.substr( pos ) );
      s = sha1( s ).get_digest_as_string( );

      set_session_variable( get_special_var_name( e_special_var_cmd_hash ), s.substr( 0, 20 ) );
   }
}

void condemn_session( size_t sess_id, int num_seconds, bool force_uncapture, bool wait_until_term )
{
   guard g( g_mutex );

   // NOTE: This function is not designed to be used to self terminate (use condemn_this_session).
   if( gtp_session && sess_id == gtp_session->id )
      return;

   date_time dtm( date_time::local( ) + ( seconds )num_seconds );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && g_sessions[ i ]->id == sess_id )
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
   guard g( g_mutex );

   if( gtp_session )
      g_condemned_sessions.insert( make_pair( gtp_session->id, date_time::local( ) ) );
}

void condemn_all_other_sessions( int num_seconds, bool force_uncapture, bool wait_until_term )
{
   guard g( g_mutex );

   size_t sess_id = 0;
   if( gtp_session )
      sess_id = gtp_session->id;

   date_time dtm( date_time::local( ) + ( seconds )num_seconds );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && g_sessions[ i ]->id != sess_id )
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
   guard g( g_mutex );

   return gtp_session
    && ( ( g_condemned_sessions.count( gtp_session->id )
    && g_condemned_sessions[ gtp_session->id ] <= date_time::local( ) )
    || ( g_session_timeout && ( date_time::local( ) - gtp_session->dtm_last_cmd ) > g_session_timeout ) );
}

void capture_session( size_t sess_id )
{
   guard g( g_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && g_sessions[ i ]->id == sess_id )
      {
         g_sessions[ i ]->is_captured = true;
         break;
      }
   }
}

void capture_all_other_sessions( )
{
   guard g( g_mutex );

   size_t sess_id = 0;
   if( gtp_session )
      sess_id = gtp_session->id;

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && g_sessions[ i ]->id != sess_id )
         g_sessions[ i ]->is_captured = true;
   }
}

bool is_captured_session( )
{
   guard g( g_mutex );
   return gtp_session && gtp_session->is_captured;
}

void release_session( size_t sess_id, bool wait_until_term )
{
   guard g( g_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && g_sessions[ i ]->id == sess_id )
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
   guard g( g_mutex );

   size_t sess_id = 0;
   if( gtp_session )
      sess_id = gtp_session->id;

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && g_sessions[ i ]->id != sess_id )
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
   guard g( g_mutex );
   return gtp_session && gtp_session->skip_fk_fetches;
}

void session_skip_fk_fetches( bool skip_fk_fetches )
{
   guard g( g_mutex );

   if( gtp_session )
      gtp_session->skip_fk_fetches = skip_fk_fetches;
}

bool session_skip_validation( )
{
   guard g( g_mutex );
   return gtp_session && gtp_session->skip_validation;
}

void session_skip_validation( bool skip_validation )
{
   guard g( g_mutex );

   if( gtp_session )
      gtp_session->skip_validation = skip_validation;
}

bool session_skip_is_constained( )
{
   guard g( g_mutex );
   return gtp_session && gtp_session->skip_is_constrained;
}

void session_skip_is_constained( bool skip_is_constrained )
{
   guard g( g_mutex );

   if( gtp_session )
      gtp_session->skip_is_constrained = skip_is_constrained;
}

bool get_script_reconfig( )
{
   guard g( g_mutex );
   return g_script_reconfig;
}

string get_pem_password( )
{
   guard g( g_mutex );
   return decrypt_data( g_pem_password );
}

string get_rpc_password( )
{
   guard g( g_mutex );
   return decrypt_data( g_rpc_password );
}

string get_sql_password( )
{
   guard g( g_mutex );

   string pwd;

   if( gtp_session
    && gtp_session->p_storage_handler->get_name( ) == "Meta" ) // i.e. only allow the Meta to do this
   {
      if( g_sql_password.empty( ) )
         pwd = "."; // i.e. used to give batch scripts a non-empty password argument
      else
         pwd = decrypt_data( g_sql_password );
   }

   return pwd;
}

string get_encrypted_gpg_password( )
{
   guard g( g_mutex );
   return g_gpg_password;
}

string get_encrypted_pem_password( )
{
   guard g( g_mutex );
   return g_pem_password;
}

string get_encrypted_rpc_password( )
{
   guard g( g_mutex );
   return g_rpc_password;
}

string get_encrypted_sql_password( )
{
   guard g( g_mutex );
   return g_sql_password;
}

string get_encrypted_pop3_password( )
{
   guard g( g_mutex );
   return g_pop3_password;
}

string get_encrypted_smtp_password( )
{
   guard g( g_mutex );
   return g_smtp_password;
}

string get_default_storage( )
{
   guard g( g_mutex );
   return g_default_storage;
}

void set_default_storage( const string& name )
{
   guard g( g_mutex );
   g_default_storage = name;
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
   guard g( g_mutex );
   return gtp_session->blockchain;
}

bool get_session_is_using_blockchain( )
{
   return gtp_session->p_storage_handler->is_using_blockchain( );
}

unsigned int get_num_sessions_for_blockchain( const string& blockchain )
{
   guard g( g_mutex );

   unsigned int num_sessions = 0;

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && blockchain == g_sessions[ i ]->blockchain )
         ++num_sessions;
   }

   return num_sessions;
}

void add_peer_file_hash_for_get( const string& hash )
{
   guard g( g_mutex );

   gtp_session->file_hashs_to_get.push_back( hash );
}

void store_repository_entry_record( const string& key,
 const string& local_hash, const string& local_public_key, const string& master_public_key )
{
   ods::bulk_write bulk_write( *gap_ods );
   scoped_ods_instance ods_instance( *gap_ods );

   gap_ofs->set_root_folder( c_file_repository_folder );

   stringstream sio_data;
   sio_writer writer( sio_data );

   writer.write_attribute( c_attribute_local_hash, local_hash );
   writer.write_attribute( c_attribute_local_public_key, local_public_key );
   writer.write_attribute( c_attribute_master_public_key, master_public_key );

   writer.finish_sections( );

   gap_ofs->store_file( key, 0, &sio_data );
}

bool fetch_repository_entry_record( const string& key,
 string& local_hash, string& local_public_key, string& master_public_key, bool must_exist )
{
   ods::bulk_read bulk_read( *gap_ods );
   scoped_ods_instance ods_instance( *gap_ods );

   gap_ofs->set_root_folder( c_file_repository_folder );

   if( !must_exist && !gap_ofs->has_file( key ) )
      return false;

   stringstream sio_data;
   gap_ofs->get_file( key, &sio_data, true );

   sio_reader reader( sio_data );

   local_hash = reader.read_attribute( c_attribute_local_hash );
   local_public_key = reader.read_attribute( c_attribute_local_public_key );
   master_public_key = reader.read_attribute( c_attribute_master_public_key );

   return true;
}

string top_next_peer_file_hash_to_get( )
{
   guard g( g_mutex );

   string hash;

   if( !gtp_session->file_hashs_to_get.empty( ) )
      hash = gtp_session->file_hashs_to_get.front( );

   return hash;
}

void pop_next_peer_file_hash_to_get( )
{
   guard g( g_mutex );

   if( !gtp_session->file_hashs_to_get.empty( ) )
      gtp_session->file_hashs_to_get.pop_front( );
}

void add_peer_file_hash_for_put( const string& hash )
{
   guard g( g_mutex );

   gtp_session->file_hashs_to_put.push_back( hash );
}

void add_peer_file_hash_for_put_for_all_peers( const string& hash,
 const string* p_blockchain, const string* p_session_variable, bool include_self, size_t session_id_to_skip )
{
   guard g( g_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ]
       && g_sessions[ i ]->is_peer_session
       && g_sessions[ i ]->id != session_id_to_skip
       && ( include_self || g_sessions[ i ] != gtp_session )
       && ( !p_blockchain || *p_blockchain == g_sessions[ i ]->blockchain )
       && ( !p_session_variable || g_sessions[ i ]->variables.count( *p_session_variable ) ) )
         g_sessions[ i ]->file_hashs_to_put.push_back( hash );
   }
}

string top_next_peer_file_hash_to_put( )
{
   guard g( g_mutex );

   string hash;

   if( !gtp_session->file_hashs_to_put.empty( ) )
      hash = gtp_session->file_hashs_to_put.front( );

   return hash;
}

void pop_next_peer_file_hash_to_put( )
{
   guard g( g_mutex );

   if( !gtp_session->file_hashs_to_put.empty( ) )
      gtp_session->file_hashs_to_put.pop_front( );
}

bool any_peer_still_has_file_hash_to_put(
 const string& hash, const string* p_blockchain )
{
   guard g( g_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ]
       && g_sessions[ i ]->is_peer_session
       && ( !p_blockchain || *p_blockchain == g_sessions[ i ]->blockchain ) )
      {
         for( size_t j = 0; j < g_sessions[ i ]->file_hashs_to_put.size( ); j++ )
         {
            if( g_sessions[ i ]->file_hashs_to_put[ j ] == hash )
               return true;
         }
      }
   }

   return false;
}

void set_default_session_variables( int port )
{
   if( port )
      set_session_variable( get_special_var_name( e_special_var_port ), to_string( port ) );

   set_session_variable( get_special_var_name( e_special_var_storage ), get_default_storage( ) );
}

string get_raw_session_variable( const string& name )
{
   string retval;

   bool found = false;

   if( gtp_session )
   {
      if( gtp_session->variables.count( name ) )
      {
         found = true;
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
               if( !retval.empty( ) )
                  retval += "\n";
               retval += ci->first + ' ' + ci->second;
            }
         }
      }
   }

   if( !found )
   {
      if( name == get_special_var_name( e_special_var_set ) )
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
      else if( name == get_special_var_name( e_special_var_none ) )
         retval = " ";
      else if( name == get_special_var_name( e_special_var_deque ) )
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
      else if( name == get_special_var_name( e_special_var_algos ) )
      {
         guard g( g_mutex );
         temporary_algo_prefix tmp_algo_prefix( gtp_session->p_storage_handler->get_name( ) );

         ostringstream osstr;
         output_algos( osstr );
         retval = osstr.str( );
      }
      else if( name == get_special_var_name( e_special_var_storage ) )
         retval = get_default_storage( );
      else if( name == get_special_var_name( e_special_var_crypt_key ) )
      {
         if( gtp_session
          && gtp_session->variables.count( get_special_var_name( e_special_var_uid ) )
          && gtp_session->variables.count( get_special_var_name( e_special_var_blockchain ) )
          && has_crypt_key_for_blockchain_account(
          gtp_session->variables[ get_special_var_name( e_special_var_blockchain ) ],
          gtp_session->variables[ get_special_var_name( e_special_var_uid ) ] ) )
            retval = "1";
      }
   }

   if( gtp_session && !name.empty( ) && name[ 0 ] == '@' )
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
   string get_value( const string& name ) const { return get_raw_session_variable( name ); }
};

string get_session_variable( const string& name_or_expr )
{
   raw_session_variable_getter raw_getter;
   variable_expression expr( name_or_expr, raw_getter );

   return expr.get_value( );
}

void set_session_variable( const string& name,
 const string& value, bool* p_set_special_temporary, command_handler* p_command_handler )
{
   guard g( g_mutex );

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

      bool skip_actual_variable = false;

      if( name == get_special_var_name( e_special_var_set ) )
      {
         skip_actual_variable = true;

         if( val.empty( ) )
            gtp_session->set_items.clear( );
         else
         {
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
            else if( val == "size" )
            {
               val = to_string( gtp_session->set_items.size( ) );

               gtp_session->last_set_item = val;

               if( p_set_special_temporary )
                  *p_set_special_temporary = true;
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
               p_command_handler->output_progress( "training..." );
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

            array tmp_array( val );
            val = tmp_array.get_state( );
         }

         if( !new_array && !old_val.empty( ) )
         {
            string::size_type pos = val.find( ' ' );

            array tmp_array( old_val );

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
         skip_actual_variable = true;

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

      if( !skip_actual_variable )
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
   guard g( g_mutex );

   bool retval = false;

   if( gtp_session )
   {
      if( !gtp_session->variables.count( name ) )
      {
         if( current.empty( ) )
            retval = true;
      }
      else if( current == gtp_session->variables[ name ] )
      {
         retval = true;
         gtp_session->variables.erase( name );
      }

      if( retval && !value.empty( ) )
      {
         if( gtp_session->variables.count( name ) )
            gtp_session->variables[ name ] = value;
         else
            gtp_session->variables.insert( make_pair( name, value ) );
      }
   }

   return retval;
}

bool has_any_session_variable( const string& name )
{
   guard g( g_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ]
       && g_sessions[ i ]->variables.count( name ) )
         return true;
   }

   return false;
}

bool has_any_session_variable( const string& name, const string& value )
{
   guard g( g_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ]
       && g_sessions[ i ]->variables.count( name )
       && g_sessions[ i ]->variables[ name ] == value )
         return true;
   }

   return false;
}

bool is_first_using_session_variable( const string& name )
{
   guard g( g_mutex );

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
   guard g( g_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ]
       && g_sessions[ i ]->variables.count( name )
       && g_sessions[ i ]->variables[ name ] == value )
         return ( g_sessions[ i ] == gtp_session );
   }

   return false;
}

void list_mutex_lock_ids_for_ciyam_base( ostream& outs )
{
   outs << "ciyam_base::g_mutex = " << g_mutex.get_lock_id( ) << '\n';
}

bool has_crypt_key_for_blockchain_account( const string& blockchain, const string& account )
{
   guard g( g_mutex );

   return g_crypt_keys[ blockchain ].count( account );
}

string get_crypt_key_for_blockchain_account( const string& blockchain, const string& account )
{
   guard g( g_mutex );

   return g_crypt_keys[ blockchain ][ account ];
}

void set_crypt_key_for_blockchain_account(
 const string& blockchain, const string& account, const string& crypt_key )
{
   guard g( g_mutex );

   if( crypt_key.empty( ) && g_crypt_keys[ blockchain ].count( account ) )
      g_crypt_keys[ blockchain ].erase( account );

   g_crypt_keys[ blockchain ][ account ] = crypt_key;
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

               // FUTURE: These messages should be handled as a server string messages.
               cmd_handler.output_progress(
                "Processing DDL and row data for " + table_name + "..." );

               outf << "\n#Creating table " << table_name << "...\n";

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
               if( gtp_session->ap_db.get( ) )
               {
                  string select_sql( "SELECT * FROM " + table_name );

                  size_t handle = create_object_instance( *mci, class_list[ i ], 0, false );
                  class_base& instance( get_class_base_from_handle( handle, "" ) );

                  vector< string > sql_column_names;
                  sql_column_names.push_back( "C_Key_" );
                  sql_column_names.push_back( "C_Ver_" );
                  sql_column_names.push_back( "C_Rev_" );
                  sql_column_names.push_back( "C_Typ_" );
                  instance.get_sql_column_names( sql_column_names );

                  sql_dataset ds( *gtp_session->ap_db.get( ), select_sql );
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

                     if( ++num_rows % 1000 == 0 )
                     {
                        // FUTURE: These messages should be handled as a server string messages.
                        if( num_rows % 10000 == 0 )
                           cmd_handler.output_progress( "Processed "
                            + to_string( num_rows ) + " rows for " + table_name + "..." );

                        outf << "\n#Inserted " << num_rows
                         << " rows into table " << table_name << "...\n\n";

                        outf << "COMMIT;\n";
                        outf << "BEGIN;\n";
                     }
                  }

                  // FUTURE: This message should be handled as a server string message.
                  if( num_rows % 1000 != 0 )
                     outf << "\n#Inserted " << num_rows
                      << " rows into table " << table_name << "...\n";

                  destroy_object_instance( handle );
               }

               vector< string > sql_indexes;
               get_sql_indexes_for_module_class( *mci, class_list[ i ], sql_indexes );

               // FUTURE: This message should be handled as a server string message.
               if( num_rows < 1000 )
                  outf << "\n#Creating indexes for table " << table_name << "...\n";

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
                     outf << j << " for table " << table_name << "...\n";
                  }

                  outf << "\nCREATE UNIQUE INDEX " << index_prefix << "_";
                  if( j < 10 )
                     outf << '0';
                  outf << j << " ON " << table_name << '\n';
                  outf << "(\n";
                  for( size_t k = 0; k < index_columns.size( ); k++ )
                  {
                     if( k > 0 )
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

         gtp_session->p_storage_handler->get_ods( )->truncate_log( osstr.str( ).c_str( ) );

         ods_file_system ofs( *gtp_session->p_storage_handler->get_ods( ) );

         ofs.store_as_text_file( c_storable_file_name_trunc_n,
          gtp_session->p_storage_handler->get_root( ).truncation_count );

         string truncated_log_name( handler.get_name( ) + ".log" + osstr.str( ) );

         file_remove( truncated_log_name );
         file_rename( handler.get_name( ) + ".log", truncated_log_name );

         transaction_log_command( ";truncated at "
          + date_time::local( ).as_string( e_time_format_hhmmss, true ) );

         append_transaction_log_command( handler, true );
      }
   }
}

void restore_storage( command_handler& cmd_handler )
{
   if( ods::instance( ) && gtp_session->p_storage_handler->get_ods( ) )
   {
      if( ods::instance( )->get_transaction_level( ) )
         throw runtime_error( "cannot perform a restore whilst a transaction is active" );

      ods* p_ods( ods::instance( ) );
      storage_handler& handler( *gtp_session->p_storage_handler );

      if( !handler.get_is_locked_for_admin( ) )
         throw runtime_error( "cannot restore a storage unless it has been locked for admin" );

      string sql_file_name( handler.get_name( ) );
      sql_file_name += ".backup.sql";

      if( !file_exists( sql_file_name ) )
         throw runtime_error( "did not find backup file '" + sql_file_name + "'" );

      if( gtp_session->ap_db.get( ) )
         exec_sql_from_file( *gtp_session->ap_db, sql_file_name, &cmd_handler, true );
   }
}

void upgrade_storage( command_handler& cmd_handler )
{
   if( ods::instance( ) && gtp_session->p_storage_handler->get_ods( ) )
   {
      if( ods::instance( )->get_transaction_level( ) )
         throw runtime_error( "cannot perform an upgrade whilst a transaction is active" );

      ods* p_ods( ods::instance( ) );
      storage_handler& handler( *gtp_session->p_storage_handler );

      if( !handler.get_is_locked_for_admin( ) )
         throw runtime_error( "cannot upgrade a storage unless it has been locked for admin" );

      string sql_file_name( handler.get_name( ) );
      sql_file_name += ".upgrade.sql";

      if( file_exists( sql_file_name ) && gtp_session->ap_db.get( ) )
         exec_sql_from_file( *gtp_session->ap_db, sql_file_name, &cmd_handler );
   }
}

void term_storage( command_handler& cmd_handler )
{
   guard g( g_mutex );

   if( ods::instance( ) )
   {
      while( !gtp_session->transactions.empty( ) )
         transaction_rollback( );

      // NOTE: Now store the "next_id" (rather than the "ceiling" value).
      ods_file_system ofs( *ods::instance( ) );

      ofs.store_as_text_file( c_storable_file_name_log_id,
       gtp_session->p_storage_handler->get_root( ).log_id.next_id );

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

         gtp_session->p_storage_handler->release_bulk_write( );
         gtp_session->p_storage_handler->release_ods( );

         if( gtp_session->p_storage_handler->get_log_file( ).is_open( ) )
            gtp_session->p_storage_handler->get_log_file( ).close( );

         temporary_algo_prefix tmp_algo_prefix( gtp_session->p_storage_handler->get_name( ) );
         exec_algos_action( "kill", "*", "" );

         delete gtp_session->p_storage_handler;
      }

      set_session_variable( get_special_var_name( e_special_var_storage ), "" );
      gtp_session->p_storage_handler = g_storage_handlers[ 0 ];
   }
}

void storage_admin_name_lock( const string& name )
{
   guard g( g_mutex );

   if( gtp_session )
   {
      if( g_storage_handler_index.find( name ) != g_storage_handler_index.end( ) )
         throw runtime_error( "storage '" + name + "' cannot be administered as it's already in use" );

      // NOTE: If this session has an existing lock name then that will need to be removed first.
      if( gtp_session->ap_storage_name_lock.get( ) )
         gtp_session->ap_storage_name_lock.reset( );

      gtp_session->ap_storage_name_lock.reset( new global_storage_name_lock( name ) );
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

      if( init_log_id == "1" || init_log_id == c_true )
         use_init_tx_id = true;

      // NOTE: During a "restore" the comment does not need to be logged unless it follows or is a part of
      // the initial data records (such comments prior are appended by the "storage restore" code itself).
      if( use_init_tx_id || !storage_locked_for_admin( ) || identity.next_id >= c_tx_id_initial )
      {
         if( use_init_tx_id || identity.next_id == c_tx_id_initial )
            append_transaction_log_command( *gtp_session->p_storage_handler, false, 0, c_tx_id_initial );
         else
            append_transaction_log_command( *gtp_session->p_storage_handler, false, 0, identity.next_id + 1 );
      }

      if( handler.is_using_blockchain( ) )
      {
         string block_comment( string( c_block_prefix ) + ' ' );
         string file_copy_comment( string( c_file_copy_command ) + ' ' );
         string file_kill_comment( string( c_file_kill_command ) + ' ' );

         string new_comment( comment );

         if( comment.find( block_comment ) == 0 )
         {
            if( storage_locked_for_admin( ) && identity.next_id + 1 >= c_tx_id_standard )
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
         else if( comment.find( file_copy_comment ) == 0 )
         {
            gtp_session->sql_undo_statements.push_back( "#"
             + replace( new_comment, file_copy_comment, file_kill_comment ) );
         }
         else if( comment.find( file_kill_comment ) == 0 )
         {
            gtp_session->sql_undo_statements.push_back( "#"
             + replace( new_comment, file_kill_comment, file_copy_comment ) );
         }
      }
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

         string log_file_name( name + ".log" );
         ifstream inpf( log_file_name.c_str( ) );

         if( !inpf )
            throw runtime_error( "unexpected error attempting to open '" + log_file_name + "' for reading" );

         string next;
         size_t line = 0;
         time_t ts( time( 0 ) );
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

            if( time( 0 ) - ts >= 10 )
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

      string log_file_name( name + ".log" );
      string new_log_file_name( name + ".log.new" );

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
            string next_log_file_name( module_list[ i ] + ".log" );
            module_log_files.push_back( make_pair( new ifstream( next_log_file_name.c_str( ) ), "" ) );
         }

         string next;
         size_t line = 0;
         time_t ts( time( 0 ) );

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

            if( time( 0 ) - ts >= 10 )
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

   string dead_keys_file( gtp_session->p_storage_handler->get_name( ) + c_dead_keys_ext );

   ofstream outf( dead_keys_file.c_str( ), ios::out | ios::app );
   if( !outf )
      throw runtime_error( "unable to open '" + dead_keys_file + "' for output" );

   outf << dead_key << '\n';
   outf.flush( );

   if( !outf.good( ) )
      throw runtime_error( "*** unexpected error occurred writing to '" + dead_keys_file + " ***" );

   gtp_session->p_storage_handler->get_dead_keys( ).insert( dead_key );
}

string storage_name( )
{
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

   ods_file_system ofs( *gtp_session->p_storage_handler->get_ods( ) );
   ofs.store_as_text_file( c_storable_file_name_id, new_identity );

   gtp_session->p_storage_handler->get_root( ).identity = new_identity;
}

string storage_blockchain( )
{
   string s, identity( storage_identity( ) );

   string::size_type pos = identity.find( ':' );

   if( pos != string::npos )
      s = identity.substr( pos + 1 );

   return s;
}

string storage_module_directory( )
{
   return gtp_session->p_storage_handler->get_root( ).module_directory;
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

void storage_process_undo( uint64_t new_height, map< string, string >& file_info )
{
   guard g( g_mutex );

   storage_handler& handler( *gtp_session->p_storage_handler );

   string undo_sql( gtp_session->p_storage_handler->get_name( ) + ".undo.sql" );
   string local_txs( gtp_session->p_storage_handler->get_name( ) + ".txs.log" );

   string new_undo_sql( undo_sql + ".new" );

   bool okay = true;
   bool local_only = false;

   // NOTE: If the request was to undo local txs then only proceed if there are any.
   if( new_height == c_unconfirmed_revision )
   {
      if( !file_exists( local_txs ) )
         okay = false;
      else
         local_only = true;
   }

   if( okay && gtp_session->ap_db.get( ) )
   {
      ifstream inpf( undo_sql.c_str( ) );
      if( !inpf )
         throw runtime_error( "unable to open file '" + undo_sql + "' for input in storage_process_undo" );

      ofstream outf( new_undo_sql.c_str( ) );
      if( !outf )
         throw runtime_error( "unable to open file '" + new_undo_sql + "' for output in storage_process_undo" );

      deque< string > undo_statements;

      string block_marker( "#" + string( c_block_prefix ) + " " );

      string next;
      bool found_rewind_point = false;
      while( getline( inpf, next ) )
      {
         string::size_type pos = next.find( block_marker );
         if( pos == 0 )
         {
            if( !found_rewind_point )
            {
               uint64_t height = from_string< uint64_t >( next.substr( pos + block_marker.size( ) ) );

               if( height >= new_height )
                  found_rewind_point = true;
            }
         }
         else if( found_rewind_point )
            undo_statements.push_front( next );

         if( !found_rewind_point )
            outf << next << '\n';
      }

      if( !found_rewind_point )
         throw runtime_error( "unexpected rewind point " + to_string( new_height ) + " not found" );

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

         if( !next_statement.empty( ) && next_statement[ 0 ] == '#' )
         {
            if( next_statement.find( c_file_kill_command ) == 1 )
            {
               // NOTE: Expected format is: #file_kill <hash> <module_id> <class_id> <filename>
               vector< string > parts;
               split( next_statement, parts, ' ' );

               if( parts.size( ) != 5 )
                  throw runtime_error( "invalid file_kill: " + next_statement );

               file_info[ storage_files_dir + '/' + parts[ 2 ] + '/' + parts[ 3 ] + '/' + parts[ 4 ] ] = string( );
            }
            else if( next_statement.find( c_file_copy_command ) == 1 )
            {
               // NOTE: Expected format is: #file_copy <hash> <module_id> <class_id> <filename>
               vector< string > parts;
               split( next_statement, parts, ' ' );

               if( parts.size( ) != 5 )
                  throw runtime_error( "invalid file_copy: " + next_statement );

               file_info[ storage_files_dir + '/' + parts[ 2 ] + '/' + parts[ 3 ] + '/' + parts[ 4 ] ] = parts[ 1 ];
            }

            continue;
         }

         TRACE_LOG( TRACE_SQLSTMTS, next_statement );
         exec_sql( *gtp_session->ap_db, next_statement );
      }
   }

   string log_name( gtp_session->p_storage_handler->get_name( ) + ".log" );
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

      string block_marker( ";" + string( c_block_prefix ) + " " );

      string next;
      bool finished = false;
      while( getline( inpf, next ) )
      {
         string::size_type pos = next.find( ']' );
         if( pos != string::npos && next.find( block_marker ) == pos + 1 )
         {
            uint64_t block = from_string< uint64_t >( next.substr( pos + block_marker.length( ) ) );

            if( block >= new_height )
               finished = true;
         }

         if( finished )
         {
            if( pos != string::npos && !local_only )
               outf << next.substr( 0, pos + 1 ) << ";rewind " << to_string( new_height ) << "\n";
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
#ifdef _WIN32
      // NOTE: Due to file locking inheritence in Win32 if this function is called from a script
      // then it may not be possible to delete or rename the application log file so instead the
      // file is truncated then the new content copied.
      ofstream outf( log_name.c_str( ), ios::out | ios::trunc );
      if( !outf )
         throw runtime_error( "unable to open '" + log_name + "' for output" );

      ifstream inpf( new_log_name.c_str( ) );

      string next;
      while( getline( inpf, next ) )
         outf << next << '\n';

      outf.flush( );
      if( !outf.good( ) )
         throw runtime_error( "*** unexpected error occurred writing to application log ***" );

      inpf.close( );
      outf.close( );

      remove_file( new_log_name );
#else
      remove_file( log_name );
      rename_file( new_log_name, log_name );
#endif
   }

   if( file_exists( local_txs ) )
      remove_file( local_txs );

   if( file_exists( local_txs + ".new" ) )
      rename_file( local_txs + ".new", local_txs );
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
            exec_sql( *gtp_session->ap_db, "LOCK TABLES " + all_table_info );
      }
      catch( ... )
      {
         exec_sql( *gtp_session->ap_db, "UNLOCK TABLES" );
         throw;
      }
   }
}

void storage_unlock_all_tables( )
{
   if( ods::instance( ) && gtp_session->p_storage_handler->get_ods( ) )
      exec_sql( *gtp_session->ap_db, "UNLOCK TABLES" );
}

bool storage_locked_for_admin( )
{
   return gtp_session->p_storage_handler->get_is_locked_for_admin( );
}

ods& storage_instance( )
{
   if( !ods::instance( ) )
      throw runtime_error( "storage has not been initialised" );

   return *ods::instance( );
}

ods& ciyam_ods_instance( )
{
   return *gap_ods;
}

ods_file_system& ciyam_ods_file_system( )
{
   return *gap_ofs;
}

string gen_key( const char* p_suffix, bool append_slot_num )
{
   string key;

   if( gtp_session )
   {
      date_time dtm( date_time::standard( ) );

      if( !append_slot_num )
         key = dtm.as_string( );
      else
      {
         size_t num( gtp_session->slot );

         char sss[ ] = "sss";
         sss[ 0 ] = '0' + ( num / 100 );
         sss[ 1 ] = '0' + ( ( num % 100 ) / 10 );
         sss[ 2 ] = '0' + ( num % 10 );

         key = dtm.as_string( ) + string( sss );
      }

      if( p_suffix )
         key += string( p_suffix );

      // KLUDGE: Time granularity restrictions (at least under Win32) prevent more than one key being created
      // every 20 milliseconds within each session so add a delay to ensure duplicates will not be generated.
      msleep( 20 );
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

void set_uid( const string& uid )
{
   string s( uid );

   string::size_type pos = uid.find( ':' );
   string::size_type spos = uid.find( '!' );

   gtp_session->sec.erase( );
   set_session_variable( get_special_var_name( e_special_var_sec ), "" );

   if( spos != string::npos )
   {
      if( pos == string::npos || pos > spos )
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

bool is_admin_uid( )
{
   return get_uid( ) == c_admin;
}

bool is_system_uid( )
{
   bool rc = false;
   string uid( get_uid( ) );

   if( uid == c_sys || uid == c_auto || uid == c_init )
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
   if( gtp_session->sec.empty( ) || gtp_session->sec <= level )
      return true;
   else
      return false;
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
      gtp_session->grp = grp;
      set_session_variable( get_special_var_name( e_special_var_grp ), grp );
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

string get_session_secret( )
{
   return gtp_session->secret;
}

void set_session_secret( const string& secret )
{
   gtp_session->secret = secret;
}

void set_session_mint_account( const string& account )
{
   gtp_session->account = account;
}

bool uid_matches_session_mint_account( )
{
   if( get_uid( ) == gtp_session->account )
      return true;
   else
      return false;
}

string session_shared_decrypt( const string& pubkey, const string& message )
{
   if( pubkey.empty( ) )
      return message;

#ifdef SSL_SUPPORT
   public_key pub_key( pubkey );
   return gtp_session->priv_key.decrypt_message( pub_key, message );
#else
   throw runtime_error( "session_shared_decrypt requires SSL support" );
#endif
}

size_t get_next_handle( )
{
   return ++gtp_session->next_handle;
}

void module_list( ostream& os )
{
   for( module_const_iterator
    mci = gtp_session->modules_by_name.begin( ), end = gtp_session->modules_by_name.end( ); mci != end; ++mci )
      os << mci->second << ' ' << mci->first << '\n';
}

size_t module_count( )
{
   return gtp_session->modules_by_name.size( );
}

void module_class_list( const string& module, ostream& os )
{
   module_const_iterator mci = gtp_session->modules_by_id.find( module );
   if( mci == gtp_session->modules_by_id.end( ) )
   {
      mci = gtp_session->modules_by_name.find( module );

      if( mci == gtp_session->modules_by_name.end( ) )
         throw runtime_error( get_string_message( GS( c_str_module_not_loaded ),
          make_pair( c_str_parm_module_not_loaded_module, module ) ) );
   }

   module_class_list_error rc = list_module_classes( module, os );
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
          make_pair( c_str_parm_module_not_loaded_module, module ) ) );
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
          make_pair( c_str_parm_module_not_loaded_module, module ) ) );
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
          make_pair( c_str_parm_module_not_loaded_module, module ) ) );
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
       make_pair( c_str_parm_module_is_loaded_module, module_name ) ) );

   module_load_error rc = load_module( module_name );

   if( rc != e_module_load_error_none )
   {
      if( rc == e_module_load_error_file_does_not_exist )
         throw runtime_error( get_string_message( GS( c_str_module_not_exists ),
          make_pair( c_str_parm_module_not_exists_module, module_name ) ) );
      else if( rc == e_module_load_error_external_module_failure )
         throw runtime_error( get_string_message( GS( c_str_module_depends_reqd ),
          make_pair( c_str_parm_module_depends_reqd_module, module_name ) ) );
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

      auto_ptr< guard > ap_guard( new guard( g_mutex ) );

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
            bool file_existed( file_exists( sql_file_name ) );

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
                      << "_" << class_ids_and_names[ class_list[ i ] ] << "...\n";

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
                        for( size_t k = 0; k < index_columns.size( ); k++ )
                        {
                           if( k > 0 )
                              outf << ",\n";
                           outf << " " << index_columns[ k ];
                        }
                        outf << "\n);\n";
                     }
                  }
               }

               outf << "\nCOMMIT;\n";
               outf.flush( );

               if( gtp_session->ap_db.get( ) )
               {
                  // NOTE: As MySQL DDL operations with InnoDB can be very slow the global
                  // thread lock is released whilst performing the DDL. As the storage has
                  // already been locked for administration this should be of no concern.
                  ap_guard.reset( );
                  try
                  {
                     exec_sql_from_file( *gtp_session->ap_db, temp_sql_file_name, &cmd_handler );
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

                  ap_guard.reset( new guard( g_mutex ) );
               }

               ods_file_system ofs( *p_ods );

               ofs.set_folder( c_storable_folder_name_modules );

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
       make_pair( c_str_parm_module_not_loaded_module, module_name ) ) );

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

   TRACE_LOG( TRACE_CLASSOPS, "execute_object_command( ) [class: " + instance.get_class_name( ) + "] " + method_name_and_args );

   return instance.execute( method_name_and_args );
}

void init_object_instance( size_t handle, const string& context, bool for_create )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   instance.init( for_create );
}

void prepare_object_instance( size_t handle, const string& context, bool for_create, bool call_to_store )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   instance.prepare( for_create, call_to_store );
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
      if( field_info[ i ].id == id_or_name || field_info[ i ].name == id_or_name )
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
 const vector< string >* p_omit_matching, bool decrypt_for_blockchain_minter )
{
   string field_values;
   string key_value( instance_key_info( handle, parent_context, true ) );

   if( p_omit_matching && p_omit_matching->size( ) != field_list.size( ) )
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
         if( !field_values.empty( ) || ( i > 0 && !p_omit_matching ) )
            field_values += ',';

         multimap< size_t, string >::const_iterator ci = p_inserts->find( i );

         advance( ci, n );

         string insert_name( ci->second );

         if( insert_name == c_key_field )
         {
            if( !p_replace_map || p_replace_map->find( key_value ) == p_replace_map->end( ) )
               add_next_value( as_csv, key_value, field_values );
            else
               add_next_value( as_csv, p_replace_map->find( key_value )->second, field_values );
         }
         else if( !insert_name.empty( ) && insert_name[ 0 ] == '@' )
            add_next_value( as_csv,
             instance_get_variable( handle, parent_context, insert_name ), field_values );
         else
            add_next_value( as_csv, insert_name, field_values );
      }

      if( field == c_key_field )
         next_value = key_value;
      else if( !field.empty( ) && field[ 0 ] == '@' )
         next_value = instance_get_variable( handle, context, field );
      else if( field != c_ignore_field )
         next_value = execute_object_command( handle, context, "get " + field );

      if( p_replace_map && !next_value.empty( ) )
      {
         for( map< string, string >::const_iterator ci = p_replace_map->begin( ); ci != p_replace_map->end( ); ++ci )
         {
            while( true )
            {
               string::size_type pos = next_value.find( ci->first );
               if( pos == string::npos )
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

      if( !p_omit_matching || ( *p_omit_matching )[ i ] != next_value )
      {
         if( !field_values.empty( ) || ( i > 0 && !p_omit_matching ) )
            field_values += ',';

         if( field != c_key_field && !next_value.empty( ) )
         {
            bool is_encrypted = false;
            string type_name = get_field_type_name( handle, context, field, &is_encrypted );

            if( is_encrypted
             && decrypt_for_blockchain_minter
             && uid_matches_session_mint_account( ) )
               next_value = decrypt( next_value );

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

      if( insert_name == c_key_field )
      {
         if( !p_replace_map || p_replace_map->find( key_value ) == p_replace_map->end( ) )
            add_next_value( as_csv, key_value, field_values );
         else
            add_next_value( as_csv, p_replace_map->find( key_value )->second, field_values );
      }
      else if( !insert_name.empty( ) && insert_name[ 0 ] == '@' )
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

void dump_storage_cache( ostream& os )
{
   guard g( g_mutex );

   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   os << '\n';
   gtp_session->p_storage_handler->dump_cache( os );
}

void dump_storage_locks( ostream& os )
{
   guard g( g_mutex );

   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   os << '\n';
   gtp_session->p_storage_handler->dump_locks( os );
}

string exec_bulk_ops( const string& module,
 const string& uid, const string& dtm, const string& mclass,
 const string& filename, const string& export_fields, const string& tz_name, bool destroy_records,
 const string& search_text, const string& search_query, const string& fixed_field_values, command_handler& handler )
{
   string response;

   string module_id( module );
   if( !gtp_session->modules_by_id.count( module ) )
   {
      if( !gtp_session->modules_by_name.count( module ) )
         throw runtime_error( "unable to resolve module id/name '" + module + "'" );

      module_id = gtp_session->modules_by_name.find( module )->second;
   }

   bool is_using_blockchain = gtp_session->p_storage_handler->is_using_blockchain( );

   string class_id = get_class_id_for_id_or_name( module_id, mclass );

   ifstream inpf;
   ofstream outf;

   size_t line = 1;
   time_t ts( time( 0 ) );
   bool in_trans = false;
   bool is_export = false;

   if( !export_fields.empty( ) )
   {
      is_export = true;

      outf.open( filename.c_str( ), ios::out );
      if( !outf )
         throw runtime_error( "unable to open '" + filename + "' for output" );
   }
   else
   {
      inpf.open( filename.c_str( ), ios::in );

      if( !inpf )
         throw runtime_error( "unable to open '" + filename + "' for input" );

      string log_file_name( filename.substr( 0, filename.find_last_of( "." ) ) + ".log" );
      outf.open( log_file_name.c_str( ), ios::out );
      if( !outf )
         throw runtime_error( "unable to open '" + log_file_name + "' for output" );
   }

   size_t handle = create_object_instance( module_id, class_id, 0, false );
   try
   {
      vector< string > fields;
      vector< string > original_fields;

      if( is_export )
      {
         if( export_fields == "*" )
         {
            fields.push_back( c_key_field );
            original_fields.push_back( c_key_field );

            get_all_field_names( handle, "", fields );
            get_all_field_names( handle, "", original_fields );
         }
         else
         {
            split( export_fields, fields );
            split( export_fields, original_fields );
         }

         set_any_field_ids_to_names( handle, "", fields );

         string fields_for_iteration;

         string key_info;
         for( size_t i = 0; i < fields.size( ); i++ )
         {
            if( original_fields[ i ] != c_key_field )
            {
               // NOTE: Use the first field to determine the order (if the first
               // field was the key then no need to add any ordering information).
               if( i == 0 )
                  key_info = original_fields[ i ] + " ";

               if( !fields_for_iteration.empty( ) )
                  fields_for_iteration += ",";
               fields_for_iteration += original_fields[ i ];
            }

            if( i > 0 )
               outf << ",";
            outf << fields[ i ];
         }

         outf << "\n";

         if( instance_iterate( handle, "", key_info,
          fields_for_iteration, search_text, search_query, "", e_iter_direction_forwards, true ) )
         {
            do
            {
               outf << get_field_values( handle, "", fields, tz_name, false, true ) << "\n";
            } while( instance_iterate_next( handle, "" ) );
         }
      }
      else
      {
         set_uid( uid );
         set_dtm( dtm );
         set_tz_name( tz_name );

         string log_lines;

         vector< string > key_fields;
         set< string > sorted_key_fields;

         vector< string > fixed_fields;
         vector< string > fixed_values;

         if( !fixed_field_values.empty( ) )
         {
            vector< string > fixed_field_value_pairs;
            split( fixed_field_values, fixed_field_value_pairs );

            string::size_type pos;
            for( size_t i = 0; i < fixed_field_value_pairs.size( ); i++ )
            {
               pos = fixed_field_value_pairs[ i ].find( '=' );
               if( pos == string::npos )
                  throw runtime_error( "unexpected field=value pair format '" + fixed_field_value_pairs[ i ] + "'" );

               fixed_fields.push_back( fixed_field_value_pairs[ i ].substr( 0, pos ) );
               fixed_values.push_back( fixed_field_value_pairs[ i ].substr( pos + 1 ) );
            }
         }

         string next;
         size_t errors = 0;
         bool is_first = true;
         bool can_fetch = false;
         bool has_key_field = false;

         size_t num_created = 0;
         size_t num_updated = 0;
         size_t num_destroyed = 0;
         size_t key_field_num = 0;
         size_t transaction_id = 0;

         while( getline( inpf, next ) )
         {
            remove_trailing_cr_from_text_file_line( next, is_first );

            if( is_first )
            {
               is_first = false;

               // NOTE: UTF-8 text files will often begin with an identifying sequence "EF BB BF" as the
               // first three characters of the file so if the first byte is "EF" assume UTF-8 and strip.
               if( next.size( ) >= 3 && next[ 0 ] == ( char )0xef )
                  next.erase( 0, 3 );

               if( split( next, fields ) < 1 )
                  throw runtime_error( "cannot import without at least one field name" );

               set_any_field_names_to_ids( handle, "", fields );
               set_any_field_names_to_ids( handle, "", fixed_fields );

               // NOTE: Incorrerct field names are detected by attempting to convert
               // all the names that were changed to ids back to names and comparing.
               vector< string > test_fields( fields );
               set_any_field_ids_to_names( handle, "", test_fields );

               for( size_t i = 0; i < fields.size( ); i++ )
               {
                  if( fields[ i ] == c_key_field )
                  {
                     key_field_num = i;
                     has_key_field = true;
                  }
                  else if( fields[ i ] == c_ignore_field )
                     continue;
                  else if( fields[ i ] == test_fields[ i ] )
                     // FUTURE: This needs to be implemented as a string message.
                     throw runtime_error( "Unknown field '"
                      + fields[ i ] + "' for '" + instance_class( handle, "" ) + "' records." );
               }

               vector< string > test_fixed_fields( fixed_fields );
               set_any_field_ids_to_names( handle, "", test_fixed_fields );

               for( size_t i = 0; i < fixed_fields.size( ); i++ )
               {
                  // FUTURE: This needs to be implemented as a string message.
                  if( fixed_fields[ i ] == test_fixed_fields[ i ] )
                     throw runtime_error( "Unknown field '"
                      + fixed_fields[ i ] + "' for " + instance_class( handle, "" ) + " records." );
               }

               if( has_key_field )
                  can_fetch = true;
               else if( determine_alternative_key_fields( handle, "", fields, key_fields ) )
               {
                  can_fetch = true;
                  for( size_t i = 0; i < key_fields.size( ); i++ )
                     sorted_key_fields.insert( key_fields[ i ] );
               }

               continue;
            }

            // FUTURE: This needs to be implemented as a string message.
            if( !can_fetch && destroy_records )
               throw runtime_error( "Cannot destroy records without providing their key fields." );

            ++line;

            if( next.empty( ) )
               continue;

            bool destroy_record( destroy_records );

            size_t num_values = 0;
            vector< string > values;

            size_t continuation_offset = 0;
            bool last_value_incomplete = false;
            while( true )
            {
               num_values = split_csv_values( next, values, last_value_incomplete, continuation_offset );

               if( !last_value_incomplete )
                  break;

               next = values.back( );
               values.pop_back( );

               string continuation;
               if( !getline( inpf, continuation ) )
                  throw runtime_error( "unexpected incomplete record found at line #" + to_string( line ) );

               // NOTE: In case the string file had been treated as binary during an FTP transfer remove trailing CR.
               if( continuation.size( ) && continuation[ continuation.size( ) - 1 ] == '\r' )
                  continuation.erase( continuation.size( ) - 1 );

               continuation_offset = next.size( ) + 1;

               ++line;
               next += '\n' + continuation;
            }

            if( num_values != fields.size( ) )
            {
               // FUTURE: This needs to be implemented as a string message.
               string message( "Wrong number of field values (expected "
                + to_string( fields.size( ) ) + " but found " + to_string( num_values )
                + ") in import file line #" + to_string( line ) + "." );

               // NOTE: If the second line is incorrect then is assuming all will fail.
               if( line == 2 )
                  throw runtime_error( message );
               else
               {
                  outf << "Error: " << message << endl;

                  ++errors;
                  continue;
               }
            }

            string key;

            if( !in_trans )
            {
               in_trans = true;
               transaction_start( );
               transaction_id = next_transaction_id( );
            }

            string next_log_line;
            bool found_instance = false;
            bool skipping_fk_checks = false;
            if( can_fetch )
            {
               string key_info;

               if( has_key_field )
                  key_info = values[ key_field_num ];
               else
               {
                  size_t num_fixed = 0;
                  string key_info_fields;
                  string key_info_values;
                  for( size_t i = 0; i < num_values; i++ )
                  {
                     if( sorted_key_fields.count( fields[ i ] ) )
                     {
                        ++num_fixed;

                        if( !key_info_fields.empty( ) )
                           key_info_fields += ",";
                        key_info_fields += fields[ i ];

                        if( !key_info_values.empty( ) )
                           key_info_values += ",";
                        key_info_values += escape( values[ i ], "," );
                     }
                  }

                  key_info = key_info_fields + "#" + to_string( num_fixed ) + " " + key_info_values;
               }

               instance_fetch_rc rc;
               instance_fetch( handle, "", key_info, &rc );

               if( rc == e_instance_fetch_rc_okay )
               {
                  found_instance = true;

                  string key_info( instance_key_info( handle, "" ) );
                  key = key_info.substr( 0, key_info.find( ' ' ) );

                  if( !destroy_record )
                  {
                     op_update_rc rc;
                     op_instance_update( handle, "", key, "", false, &rc );

                     if( rc == e_op_update_rc_okay )
                        next_log_line = "pu";
                     else
                     {
                        // FUTURE: These need to be implemented as string messages.
                        if( rc == e_op_update_rc_locked )
                           outf << "Error: Processing line #" << line << " - unable to lock for update." << endl;
                        else if( rc == e_op_update_rc_not_found )
                           outf << "Error: Processing line #" << line << " - record not found for update." << endl;
                        else
                           throw runtime_error( "unexpected op_update rc #" + to_string( rc ) );

                        ++errors;
                        continue;
                     }
                  }
                  else
                  {
                     op_destroy_rc rc;
                     op_instance_destroy( handle, "", key, "", false, &rc );

                     if( rc == e_op_destroy_rc_okay )
                        next_log_line = "pd";
                     else
                     {
                        // FUTURE: These need to be implemented as string messages.
                        if( rc == e_op_destroy_rc_locked )
                           outf << "Error: Processing line #" << line << " - unable to lock for destroy." << endl;
                        else if( rc == e_op_destroy_rc_not_found )
                           outf << "Error: Processing line #" << line << " - record not found for destroy." << endl;
                        else if( rc == e_op_destroy_rc_constrained )
                           outf << "Error: Processing line #" << line << " - record cannot be destroyed due to being used by other records." << endl;
                        else
                           throw runtime_error( "unexpected op_destroy rc #" + to_string( rc ) );

                        ++errors;
                        continue;
                     }
                  }
               }
            }

            if( !destroy_record && !found_instance )
            {
               if( has_key_field )
                  key = values[ key_field_num ];
               else
                  key = gen_key( "", !is_using_blockchain );

               op_create_rc rc;
               op_instance_create( handle, "", key, false, &rc );

               if( rc == e_op_create_rc_okay )
                  next_log_line = "pc";
               else
               {
                  // FUTURE: These need to be implemented as string messages.
                  if( rc == e_op_create_rc_locked )
                     outf << "Error: Processing line #" << line << " - unable to lock for create." << endl;
                  else if( rc == e_op_create_rc_exists )
                     outf << "Error: Processing line #" << line << " - cannot create duplicate record." << endl;
                  else
                     throw runtime_error( "unexpected op_create rc #" + to_string( rc ) );

                  ++errors;
                  continue;
               }
            }

            string class_id_to_log( class_id );

            if( is_using_blockchain && class_id_to_log.find( module_id ) == 0 )
               class_id_to_log.erase( 0, module_id.length( ) );

            next_log_line += " " + uid + " " + dtm + " " + module_id + " " + class_id_to_log + " " + key;

            if( !destroy_record )
            {
               next_log_line += " ";

               string log_field_value_pairs;
               for( size_t i = 0; i < num_values; i++ )
               {
                  if( ( has_key_field && i == key_field_num ) || fields[ i ] == c_ignore_field )
                     continue;

                  bool is_transient = false;

                  string type_name = get_field_type_name( handle, "", fields[ i ], &is_transient );

                  if( !values[ i ].empty( )
                   && ( type_name == "date_time" || type_name == "tdatetime" ) )
                  {
                     // NOTE: If a date_time string starts with 'U' then it is considered as already being UTC.
                     if( values[ i ][ 0 ] == 'U' )
                        values[ i ].erase( 0, 1 );
                     else if( !tz_name.empty( ) )
                        values[ i ] = local_to_utc( date_time( values[ i ] ), tz_name ).as_string( );
                  }

                  string value;

                  if( is_using_blockchain )
                  {
                     string method_name_and_args( "get " );
                     method_name_and_args += fields[ i ];

                     value = execute_object_command( handle, "", method_name_and_args );
                  }

                  // NOTE: Field values that are unchanged are omitted from the log as are values
                  // for all transient fields (unless used with initial data).
                  if( !is_using_blockchain
                   || ( ( !is_transient || is_init_uid( ) ) && value != values[ i ] ) )
                  {
                     string method_name_and_args( "set " );
                     method_name_and_args += fields[ i ] + " ";
                     method_name_and_args += "\"" + escaped( escaped( values[ i ] ), "\"", c_nul ) + "\"";

                     execute_object_command( handle, "", method_name_and_args );

                     if( !log_field_value_pairs.empty( ) )
                        log_field_value_pairs += ",";

                     string field_id_to_log( fields[ i ] );

                     if( is_using_blockchain && field_id_to_log.find( module_id ) == 0 )
                     {
                        field_id_to_log.erase( 0, module_id.length( ) );
                        if( field_id_to_log.find( class_id_to_log ) == 0 )
                           field_id_to_log.erase( 0, class_id_to_log.length( ) );
                     }

                     log_field_value_pairs += field_id_to_log + "="
                      + search_replace( escaped( escaped( values[ i ] ), "\"", c_esc, "rn\r\n" ), ",", "\\\\," );
                  }
               }

               for( size_t i = 0; i < fixed_fields.size( ); i++ )
               {
                  string method_name_and_args( "set " );
                  method_name_and_args += fixed_fields[ i ] + " ";
                  method_name_and_args += "\"" + escaped( escaped( fixed_values[ i ] ), "\"", c_nul ) + "\"";

                  if( !log_field_value_pairs.empty( ) )
                     log_field_value_pairs += ",";

                  string field_id_to_log( fixed_fields[ i ] );

                  if( is_using_blockchain && field_id_to_log.find( module_id ) == 0 )
                  {
                     field_id_to_log.erase( 0, module_id.length( ) );
                     if( field_id_to_log.find( class_id_to_log ) == 0 )
                        field_id_to_log.erase( 0, class_id_to_log.length( ) );
                  }

                  log_field_value_pairs += field_id_to_log + "="
                   + search_replace( escaped( escaped( fixed_values[ i ] ), "\"", c_esc, "rn\r\n" ), ",", "\\\\," );

                  execute_object_command( handle, "", method_name_and_args );
               }

               next_log_line += "\"" + log_field_value_pairs + "\"";
            }

            op_apply_rc rc;
            op_instance_apply( handle, "", false, &rc );

            if( rc != e_op_apply_rc_okay )
            {
               class_base& instance( get_class_base_from_handle( handle, "" ) );
               string validation_error( instance.get_validation_errors( class_base::e_validation_errors_type_first_only ) );

               op_instance_cancel( handle, "", false );

               // FUTURE: These need to be implemented as string messages.
               if( rc == e_op_apply_rc_locked )
                  outf << "Error: Processing line #" << line << " - record was locked." << endl;
               else if( rc == e_op_apply_rc_invalid )
               {
                  outf << "Error: Processing line #" << line << " - record was invalid.";
                  if( !validation_error.empty( ) )
                     outf << ' ' << validation_error;

                  outf << endl;
               }
               else
                  throw runtime_error( "unexpected op_apply rc #" + to_string( rc ) );

               ++errors;
               continue;
            }

            if( !log_lines.empty( ) )
               log_lines += "\n";
            log_lines += next_log_line;

            if( time( 0 ) - ts >= 10 )
            {
               ts = time( 0 );

               transaction_log_command( log_lines );
               transaction_commit( );

               in_trans = false;
               log_lines.clear( );

               // FUTURE: This message should be handled as a server string message.
               handler.output_progress( "Processed " + to_string( line ) + " lines..." );

               if( is_condemned_session( ) )
                  break;
            }

            if( found_instance )
            {
               if( !destroy_record )
                  ++num_updated;
               else
                  ++num_destroyed;
            }
            else
               ++num_created;
         }

         if( in_trans )
         {
            transaction_log_command( log_lines );
            transaction_commit( );
         }

         // FUTURE: These should be handled as string messages.
         if( num_created )
            outf << "Created " << num_created << " new record(s)." << endl;
         if( num_updated )
            outf << "Updated " << num_updated << " existing record(s)." << endl;
         if( num_destroyed )
            outf << "Removed " << num_destroyed << " existing record(s)." << endl;

         response = "Processed " + to_string( line ) + " lines with " + to_string( errors ) + " error(s).";
      }

      destroy_object_instance( handle );
   }
   catch( exception& x )
   {
      if( !is_export )
         // FUTURE: This message should be handled as a server string message.
         outf << "Error: Processing line #" << line << " - " << x.what( ) << endl;

      if( in_trans )
         transaction_rollback( );
      destroy_object_instance( handle );
      throw;
   }
   catch( ... )
   {
      if( !is_export )
         // FUTURE: This message should be handled as a server string message.
         outf << "Error: Processing line #" << line << " - unknown exception caught" << endl;

      if( in_trans )
         transaction_rollback( );
      destroy_object_instance( handle );
      throw;
   }

   return response;
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
      if( p_class_base != p_dynamic_base && p_dynamic_base->get_op( ) == class_base::e_op_type_none )
         class_base_accessor( *p_class_base ).destroy_dynamic_instance( );
      else
         p_class_base = p_dynamic_base;
   }
   else
      p_class_base = p_dynamic_base;

   class_base::op_type op( p_class_base->get_op( ) );

   if( permit == e_permit_op_type_value_none && op != class_base::e_op_type_none )
      throw runtime_error( "object instance #" + to_string( handle ) + " is currently involved in another operation" );

   if( permit != e_permit_op_type_value_none && op == class_base::e_op_type_none )
      throw runtime_error( "object instance #" + to_string( handle ) + " is not currently involved in an operation" );

   if( permit == e_permit_op_type_value_review && op != class_base::e_op_type_review )
      throw runtime_error( "object instance #" + to_string( handle ) + " is currently involved in another operation" );

   if( permit == e_permit_op_type_value_create_update_destroy
    && op != class_base::e_op_type_create && op != class_base::e_op_type_update && op != class_base::e_op_type_destroy )
      throw runtime_error( "object instance #" + to_string( handle ) + " is currently involved in another operation" );

   return *p_class_base;
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
      if( gtp_session->p_storage_handler->get_name( ) == "Meta"
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

void instance_check( class_base& instance, instance_check_rc* p_rc )
{
   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   class_base_accessor instance_accessor( instance );

   string sql;
   instance_accessor.fetch( sql, true, true );

   bool found = fetch_instance_from_db( instance, sql, true,
    false, !gtp_session->p_storage_handler->get_is_locked_for_admin( ) );

   if( !found )
   {
      if( p_rc )
         *p_rc = e_instance_check_rc_not_found;
      else
         throw runtime_error( get_string_message( GS( c_str_record_not_found ),
          make_pair( c_str_parm_record_not_found_class, instance.get_display_name( ) ),
          make_pair( c_str_parm_record_not_found_key, instance_accessor.get_lazy_fetch_key( ) ) ) );
   }
}

bool is_change_locked( class_base& instance, bool include_cascades )
{
   op_lock lock = gtp_session->p_storage_handler->get_lock_info( instance.get_lock_class_id( ), instance.get_key( ) );

   return lock.type >= op_lock::e_lock_type_update && ( include_cascades || !lock.p_root_class );
}

bool is_destroy_locked( class_base& instance, bool include_cascades )
{
   op_lock lock = gtp_session->p_storage_handler->get_lock_info( instance.get_lock_class_id( ), instance.get_key( ) );

   return lock.type == op_lock::e_lock_type_destroy && ( include_cascades || !lock.p_root_class );
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

   if( lock.p_session == gtp_session && is_create_locked )
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

   return lock.p_session == gtp_session && lock.type == op_lock::e_lock_type_update;
}

bool is_destroy_locked_by_own_session( class_base& instance, const char* p_key )
{
   op_lock lock;
   bool rc = false;

   lock = gtp_session->p_storage_handler->get_lock_info(
    instance.get_lock_class_id( ), p_key ? string( p_key ) : instance.get_key( ) );

   return lock.p_session == gtp_session && lock.type == op_lock::e_lock_type_destroy;
}

bool is_update_or_destroy_locked_by_own_session( class_base& instance, const char* p_key )
{
   op_lock lock;
   bool rc = false;

   lock = gtp_session->p_storage_handler->get_lock_info(
    instance.get_lock_class_id( ), p_key ? string( p_key ) : instance.get_key( ) );

   return lock.p_session == gtp_session
    && ( lock.type == op_lock::e_lock_type_update || lock.type == op_lock::e_lock_type_destroy );
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
      auto_ptr< temporary_object_variable > ap_tmp_skip_after_fetch;

      if( skip_after_fetch )
         ap_tmp_skip_after_fetch.reset( new temporary_object_variable(
          instance, get_special_var_name( e_special_var_skip_after_fetch ), "1" ) );

      instance.perform_fetch( key );
      instance_accessor.set_ver_exp( ver_info );
   }
}

string instance_execute( size_t handle,
 const string& context, const string& key, const string& method_name_and_args )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   TRACE_LOG( TRACE_CLASSOPS, "instance_execute( ) [class: " + instance.get_class_name( ) + "] " + method_name_and_args );

   return instance.execute( method_name_and_args );
}

string instance_get_fields_and_values( size_t handle, const string& context, const string& key )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   bool is_traditional_app( storage_blockchain( ).empty( ) );

   return instance.get_fields_and_values(
    is_traditional_app ? class_base::e_field_label_type_full_id : class_base::e_field_label_type_short_id );
}

bool instance_persistence_type_is_sql( size_t handle )
{
   class_base& instance( get_class_base_from_handle( handle, "" ) );

   return ( instance.get_persistence_type( ) == 0 ); // i.e. SQL persistence
}

bool instance_iterate( size_t handle, const string& context,
 const string& key_info, const string& fields, const string& text,
 const string& query, const string& security_info, iter_direction direction,
 bool inclusive, int row_limit, sql_optimisation optimisation, const set< string >* p_filters )
{
   return perform_instance_iterate(
    get_class_base_from_handle_for_op( handle, context, e_permit_op_type_value_none, false ),
    key_info, fields, text, query, security_info, direction, inclusive, row_limit, optimisation, p_filters );
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

void transaction_start( )
{
   if( !ods::instance( ) )
      throw runtime_error( "no storage is currently attached" );

   if( gtp_session->ap_db.get( ) && gtp_session->transactions.empty( ) )
   {
      TRACE_LOG( TRACE_SQLSTMTS, "BEGIN" );
      exec_sql( *gtp_session->ap_db, "BEGIN" );
   }

   if( has_files_area_tag( c_ciyam_tag, e_file_type_list ) )
      gtp_session->ods_tlg_size = file_size( c_ciyam_server_tlg );

   gtp_session->transactions.push( new ods::transaction( *ods::instance( ) ) );
}

void transaction_commit( )
{
   if( gtp_session->transactions.empty( ) )
      throw runtime_error( "no active transaction exists" );

   storage_handler& handler( *gtp_session->p_storage_handler );

   bool is_using_blockchain = handler.is_using_blockchain( );

   // NOTE: Scope for guard object.
   {
      guard g( g_mutex );

      gtp_session->transactions.top( )->commit( );

      delete gtp_session->transactions.top( );
      gtp_session->transactions.pop( );

      if( !gtp_session->transactions.size( ) )
      {
         if( gtp_session->p_tx_helper )
            gtp_session->p_tx_helper->at_commit( );

         if( is_using_blockchain && !is_init_uid( ) )
            append_undo_sql_statements( handler );

         append_transaction_log_command( handler );

         if( gtp_session->ap_db.get( ) )
         {
            TRACE_LOG( TRACE_SQLSTMTS, "COMMIT" );
            exec_sql( *gtp_session->ap_db, "COMMIT" );
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

   if( has_files_area_tag( c_ciyam_tag, e_file_type_list ) )
   {
      string all_tags( list_file_tags( string( c_ciyam_server_tlg ) + ".*" ) );

      if( all_tags.empty( )
       || ( file_size( c_ciyam_server_tlg ) > gtp_session->ods_tlg_size ) )
      {
         append_ciyam_ods_transaction_log_files( );
         insert_log_blobs_into_tree( c_ciyam_server_tlg );
      }
   }

   if( gtp_session->transactions.empty( ) )
   {
      for( size_t i = 0; i < gtp_session->async_or_delayed_system_commands.size( ); i++ )
      {
         TRACE_LOG( TRACE_SESSIONS, gtp_session->async_or_delayed_system_commands[ i ] );

         int rc = system( gtp_session->async_or_delayed_system_commands[ i ].c_str( ) );
         ( void )rc;
      }

      // NOTE: If the "args_file" session variable exists and a system variable with a name matching
      // its value is found then it is expected that the first (if any) error reported for a command
      // in the executed script will have replaced its value (which was initially set to "1"). Where
      // such an error message is found it will be thrown as an exception from here (even though the
      // transaction commit has completed and the command for this session has already been logged).
      string script_error;
      string check_script_error(
       get_raw_session_variable( get_special_var_name( e_special_var_check_script_error ) ) );

      for( size_t i = 0; i < gtp_session->async_or_delayed_temp_files.size( ); i++ )
      {
         string next( gtp_session->async_or_delayed_temp_files[ i ] );

         if( script_error.empty( )
          && ( check_script_error == "1" || check_script_error == c_true ) )
         {
            string value( get_raw_system_variable( next ) );

            if( !value.empty( ) && value != string( "1" ) )
               script_error = value;
         }

         set_system_variable( next, "" );
      }

      gtp_session->async_or_delayed_temp_files.clear( );
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

      if( gtp_session->ap_db.get( ) && gtp_session->transactions.empty( ) )
      {
         TRACE_LOG( TRACE_SQLSTMTS, "ROLLBACK" );
         exec_sql( *gtp_session->ap_db, "ROLLBACK" );

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

         for( size_t i = 0; i < gtp_session->async_or_delayed_temp_files.size( ); i++ )
         {
            string next( gtp_session->async_or_delayed_temp_files[ i ] );

            if( file_exists( next ) )
               file_remove( next );

            set_system_variable( next, "" );
         }

         gtp_session->async_or_delayed_temp_files.clear( );
         gtp_session->async_or_delayed_system_commands.clear( );

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

      ods_file_system ofs( *gtp_session->p_storage_handler->get_ods( ) );
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
      string blockchain( get_raw_session_variable( get_special_var_name( e_special_var_blockchain ) ) );

      if( !blockchain.empty( ) && !storage_locked_for_admin( ) )
      {
         string filename( gtp_session->p_storage_handler->get_name( ) + ".txs.log" );

         if( !file_exists( filename ) )
            gtp_session->transaction_log_command = ';'
             + string( c_block_prefix ) + ' ' + to_string( c_unconfirmed_revision );
      }

      if( replace_current )
         gtp_session->transaction_log_command = log_command;
      else
      {
         if( !gtp_session->transaction_log_command.empty( ) )
            gtp_session->transaction_log_command += '\n';

         gtp_session->transaction_log_command += log_command;
      }

      if( p_tx_helper )
         gtp_session->p_tx_helper = p_tx_helper;
   }
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

void insert_log_blobs_into_tree( const string& log_blob_file_prefix )
{
   guard g( g_mutex );

   if( !log_blob_file_prefix.empty( ) )
   {
      string separator( ":" );

      string all_tags( list_file_tags( log_blob_file_prefix + ".*" ) );

      if( !all_tags.empty( ) )
      {
         vector< string > tags;
         split( all_tags, tags, '\n' );

         for( size_t i = 0; i < tags.size( ); i++ )
         {
            string next_tag( tags[ i ] );

            create_list_tree( next_tag, next_tag, true,
             c_ciyam_tag + separator + c_variables_branch + separator + log_blob_file_prefix, c_ciyam_tag, "" );

            if( i != tags.size( ) - 1 )
               tag_del( tags[ i ] );
         }
      }
   }
}

void append_transaction_log_lines_to_blob_files(
 const string& log_blob_file_prefix, const vector< string >& log_lines, bool is_restoring, bool remove_existing_blobs )
{
   guard g( g_mutex );

   if( !log_blob_file_prefix.empty( ) && !log_lines.empty( ) )
   {
      string log_raw_file_tag;
      string raw_file_data( c_file_type_str_blob );

      string separator( ":" );
      string last_tag, last_hash;

      size_t raw_file_chunk = get_last_raw_file_data_chunk(
       log_blob_file_prefix, raw_file_data, &last_hash, remove_existing_blobs );

      log_raw_file_tag = log_blob_file_prefix + "." + to_comparable_string( raw_file_chunk, false, 5 );

      string next_line;
      for( size_t i = 0; i < log_lines.size( ); i++ )
      {
         next_line = log_lines[ i ];

         if( raw_file_data.size( ) + next_line.size( ) > g_files_area_item_max_size )
         {
            // NOTE: Don't re-create if existing file wasn't actually appended to.
            if( i > 0 )
            {
               create_raw_file( raw_file_data, true, log_raw_file_tag.c_str( ) );

               if( is_restoring )
               {
                  if( !last_hash.empty( ) )
                     delete_file( last_hash, true );
               }
               else
                  create_list_tree( log_raw_file_tag, log_raw_file_tag, true,
                   c_ciyam_tag + separator + c_variables_branch + separator + log_blob_file_prefix, c_ciyam_tag, "" );
            }

            ++raw_file_chunk;

            raw_file_data.erase( 1 );
            log_raw_file_tag = log_blob_file_prefix + "." + to_comparable_string( raw_file_chunk, false, 5 );
         }

         raw_file_data += next_line;
      }

      if( raw_file_data.size( ) > 1 )
      {
         create_raw_file( raw_file_data, true, log_raw_file_tag.c_str( ) );

         if( is_restoring )
         {
            if( !last_hash.empty( ) )
               delete_file( last_hash, true );
         }
         else
            create_list_tree( log_raw_file_tag, log_raw_file_tag, true,
             c_ciyam_tag + separator + c_variables_branch + separator + log_blob_file_prefix, c_ciyam_tag, "" );
      }
   }
}

void append_transaction_for_blockchain_application(
 const string& application, const string& transaction_hash )
{
   guard g( g_mutex );

   string filename( application + ".txs.log" );

   ofstream outf( filename.c_str( ), ios::out | ios::app );
   if( !outf )
      throw runtime_error( "unable to open '" + filename + "' for append" );

   outf << transaction_hash << '\n';

   outf.flush( );
   if( !outf.good( ) )
      throw runtime_error( "*** unexpected error occurred appending local tx hash ***" );
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

      transaction_commit( );
      can_commit = false;
   }
}

void transaction::rollback( )
{
   if( !is_dummy )
   {
      if( !can_commit )
         throw runtime_error( "cannot rollback already completed transaction" );

      transaction_rollback( );
      can_commit = false;
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

void begin_instance_op( instance_op op, class_base& instance,
 const string& key, bool internal_modification, instance_op_rc* p_rc )
{
   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "No storage is currently linked." );

   if( p_rc )
      *p_rc = e_instance_op_rc_okay;

   if( op == e_instance_op_none )
      return;

   TRACE_LOG( TRACE_CLASSOPS, "begin (enter) op = "
    + instance_op_name( op ) + ", class = " + instance.get_class_name( )
    + ", internal = " + to_string( internal_modification ) + ", key = " + key );

   string sql;
   bool is_cascade_op = false;

   string key_in_use( instance.get_key( ) );

   string key_for_op( key );
   if( key_for_op.empty( ) )
      key_for_op = key_in_use;

   if( key_for_op.empty( ) )
      throw runtime_error( "cannot perform an instance operation without a key or current instance" );

   string clone_key;
   if( op == e_instance_op_create )
   {
      string::size_type pos = key_for_op.find( ' ' );
      if( pos != string::npos )
      {
         clone_key = key.substr( pos + 1 );
         key_for_op = key.substr( 0, pos );
      }

      if( key_for_op.find_first_of( "`~!@#$%^&*<>()[]{}/\\?|-+=.,;:'\"" ) != string::npos )
         throw runtime_error( get_string_message( GS( c_str_key_invalid ),
          make_pair( c_str_parm_key_invalid_key, key_for_op ) ) );
   }

   size_t lock_handle( 0 );
   size_t xlock_handle( 0 );
   class_base_accessor instance_accessor( instance );

   if( instance.get_is_in_op( ) && !instance_accessor.get_in_op_begin( ) )
      throw runtime_error( "cannot begin an instance operation whilst already perfoming an instance operation" );

   storage_handler& handler( *gtp_session->p_storage_handler );

   string class_id( instance.get_class_id( ) );
   string module_name( instance.get_module_name( ) );
   string lock_class_id( instance.get_lock_class_id( ) );

   bool is_minimal_update( op == e_instance_op_update && !instance_accessor.fetch_field_names( ).empty( ) );

   if( is_minimal_update )
      TRACE_LOG( TRACE_CLASSOPS, "*** minimal update ***" );

   // NOTE: A create op can be started (but not applied) without an instance key (this is to help with record
   // preparation when cloning in order to create a new instance).
   if( op != e_instance_op_create || !key_for_op.empty( ) )
   {
      bool obtained_lock = false;

      if( op == e_instance_op_review
       && handler.obtain_lock( lock_handle, lock_class_id, key_for_op, op_lock::e_lock_type_review, gtp_session, &instance ) )
         obtained_lock = true;

      if( op == e_instance_op_create
       && handler.obtain_lock( lock_handle, lock_class_id, key_for_op, op_lock::e_lock_type_create, gtp_session, &instance ) )
         obtained_lock = true;

      if( op == e_instance_op_update || op == e_instance_op_destroy )
      {
         if( instance.get_is_being_cascaded( ) )
         {
            obtained_lock = true;
            is_cascade_op = true;
         }

         if( !obtained_lock )
         {
            if( op == e_instance_op_update
             && handler.obtain_lock( lock_handle, lock_class_id, key_for_op, op_lock::e_lock_type_update, gtp_session, &instance ) )
               obtained_lock = true;

            if( op == e_instance_op_destroy
             && handler.obtain_lock( lock_handle, lock_class_id, key_for_op, op_lock::e_lock_type_destroy, gtp_session, &instance ) )
               obtained_lock = true;
         }
      }

      if( !obtained_lock )
      {
         if( p_rc )
         {
            *p_rc = e_instance_op_rc_locked;
            return;
         }
         else
            throw runtime_error( "unable to obtain lock for '" + key_for_op + "'" );
      }
   }

   if( op == e_instance_op_create )
   {
      // NOTE: If cloning then retrieve the instance values of the clone source under a review lock. This review lock
      // will be held until the create operation is finished so that link locks to any foreign keys are not required.
      if( !clone_key.empty( ) )
      {
         const op_lock& lock_info = handler.get_lock_info( lock_class_id, clone_key );

         if( ( lock_info.handle && lock_info.type >= op_lock::e_lock_type_review && lock_info.p_session == gtp_session )
          || handler.obtain_lock( xlock_handle, lock_class_id, clone_key, op_lock::e_lock_type_review, gtp_session, &instance ) )
         {
            guard g( g_mutex );

            scoped_lock_holder xlock_holder( handler, xlock_handle );

            instance_accessor.set_key( clone_key, true );

            bool found = false;

            if( instance.get_persistence_type( ) == 0 ) // i.e. SQL persistence
            {
               instance_accessor.fetch( sql, false );
               found = fetch_instance_from_db( instance, sql );
            }
            else if( instance.get_persistence_type( ) == 1 ) // i.e. ODS global persistence
               found = has_instance_in_global_storage( instance, clone_key );

            xlock_holder.release( );

            if( !found )
            {
               if( p_rc )
               {
                  *p_rc = e_instance_op_rc_not_found;
                  return;
               }
               else
                  throw runtime_error( get_string_message( GS( c_str_record_not_found ),
                   make_pair( c_str_parm_record_not_found_class, instance.get_display_name( ) ),
                   make_pair( c_str_parm_record_not_found_key, instance.get_key( ) ) ) );
            }
         }
         else
         {
            if( p_rc )
            {
               *p_rc = e_instance_op_rc_locked;
               return;
            }
            else
               throw runtime_error( "unable to obtain lock for '" + clone_key + "'" );
         }
      }
      else
         instance.init( true );
   }

   instance_accessor.set_key( key_for_op, true );
   instance_accessor.set_clone_key( clone_key );

   instance_accessor.set_in_op_begin( true );

   if( op != e_instance_op_update )
      instance_accessor.fetch_field_names( ).clear( );

   class_base::op_type old_op( instance.get_op( ) );

   if( op == e_instance_op_review )
      instance_accessor.set_op( class_base::e_op_type_review, key_for_op != key_in_use );
   else if( op == e_instance_op_create )
      instance_accessor.set_op( class_base::e_op_type_create, true );
   else if( op == e_instance_op_update )
      instance_accessor.set_op( class_base::e_op_type_update, key_for_op != key_in_use );
   else if( op == e_instance_op_destroy )
      instance_accessor.set_op( class_base::e_op_type_destroy, key_for_op != key_in_use );

   try
   {
      scoped_lock_holder lock_holder( handler, lock_handle );
      scoped_lock_holder xlock_holder( handler, xlock_handle );

      if( op == e_instance_op_create )
      {
         string sql;

         // NOTE: Skip this check during a restore as an optimsation to reduce SQL (iff requested).
         if( !storage_locked_for_admin( ) || !session_skip_fk_fetches( ) )
         {
            bool found = false;

            if( instance.get_persistence_type( ) == 0 ) // i.e. SQL persistence
            {
               instance_accessor.fetch( sql, true );
               found = fetch_instance_from_db( instance, sql, true );
            }
            else if( instance.get_persistence_type( ) == 1 ) // i.e. ODS global persistence
               found = has_instance_in_global_storage( instance, key_for_op );

            if( found )
            {
               if( p_rc )
               {
                  *p_rc = e_instance_op_rc_already_exists;

                  instance_accessor.set_op( old_op, false );
                  instance_accessor.set_in_op_begin( false );

                  return;
               }
               else
                  throw runtime_error( get_string_message( GS( c_str_record_exists ),
                   make_pair( c_str_parm_record_exists_key, instance.get_key( ) ),
                   make_pair( c_str_parm_record_exists_class, instance.get_display_name( ) ) ) );
            }
         }

         instance_accessor.at_create( );
      }
      else if( op == e_instance_op_review || op == e_instance_op_update )
      {
         string sql;

         bool found = false;

         if( instance.get_persistence_type( ) == 0 ) // i.e. SQL persistence
         {
            instance_accessor.fetch( sql, false, false );

            found = fetch_instance_from_db( instance, sql,
             false, is_minimal_update && op == e_instance_op_update );
         }
         else if( instance.get_persistence_type( ) == 1 ) // i.e. ODS global persistence
         {
            found = fetch_instance_from_global_storage( instance, key_for_op );
            instance_accessor.set_original_identity( instance.get_current_identity( ) );
         }

         if( !found )
         {
            if( p_rc )
            {
               *p_rc = e_instance_op_rc_not_found;

               instance_accessor.set_op( old_op, false );
               instance_accessor.set_in_op_begin( false );

               return;
            }
            else
               throw runtime_error( get_string_message( GS( c_str_record_not_found ),
                make_pair( c_str_parm_record_not_found_class, instance.get_display_name( ) ),
                make_pair( c_str_parm_record_not_found_key, instance.get_key( ) ) ) );
         }

         string ver_expected( instance_accessor.get_ver_exp( ) );
         if( !ver_expected.empty( ) && ver_expected != instance.get_version_info( ) )
            throw runtime_error( get_string_message( GS( c_str_version_mismatch ),
             make_pair( c_str_parm_version_mismatch_found, instance.get_version_info( ) ),
             make_pair( c_str_parm_version_mismatch_expected, ver_expected ) ) );

         if( op == e_instance_op_update && !storage_locked_for_admin( )
          && ( !internal_modification
          && ( instance.get_state( ) & c_state_uneditable )
          && !( instance.get_state( ) & c_state_ignore_uneditable ) ) )
            throw runtime_error( get_string_message( GS( c_str_cannot_update ),
             make_pair( c_str_parm_cannot_update_class, instance.get_display_name( ) ) ) );

         if( op == e_instance_op_update
          && instance.get_current_identity( ) != instance.get_original_identity( ) )
            throw runtime_error( "cannot update '" + instance.get_original_identity( )
             + "' stored instance using '" + instance.get_current_identity( ) + "' object instance" );
      }
      else if( op == e_instance_op_destroy )
      {
         // NOTE: In order to correctly determine whether an instance is constrained it must be first fetched.
         bool found = false;

         if( instance.get_persistence_type( ) == 0 ) // i.e. SQL persistence
         {
            instance_accessor.fetch( sql, false );
            found = fetch_instance_from_db( instance, sql );
         }
         else if( instance.get_persistence_type( ) == 1 ) // i.e. ODS global persistence
         {
            found = fetch_instance_from_global_storage( instance, key_for_op );
            instance_accessor.set_original_identity( instance.get_current_identity( ) );
         }

         if( !found )
         {
            if( p_rc )
            {
               *p_rc = e_instance_op_rc_not_found;

               instance_accessor.set_op( old_op, false );
               instance_accessor.set_in_op_begin( false );

               return;
            }
            else
               throw runtime_error( get_string_message( GS( c_str_record_not_found ),
                make_pair( c_str_parm_record_not_found_class, instance.get_display_name( ) ),
                make_pair( c_str_parm_record_not_found_key, instance.get_key( ) ) ) );
         }

         string ver_expected( instance_accessor.get_ver_exp( ) );
         if( !ver_expected.empty( ) && ver_expected != instance.get_version_info( ) )
            throw runtime_error( get_string_message( GS( c_str_version_mismatch ),
             make_pair( c_str_parm_version_mismatch_found, instance.get_version_info( ) ),
             make_pair( c_str_parm_version_mismatch_expected, ver_expected ) ) );

         if( !instance_accessor.can_destroy( internal_modification ) )
            throw runtime_error( get_string_message( GS( c_str_cannot_destroy ),
             make_pair( c_str_parm_cannot_destroy_class, instance.get_display_name( ) ) ) );

         if( instance.get_current_identity( ) != instance.get_original_identity( ) )
            throw runtime_error( "cannot destroy '" + instance.get_original_identity( )
             + "' stored instance using '" + instance.get_current_identity( ) + "' object instance" );

         string constraining_class;
         bool is_constrained = false;

         // NOTE: Unless performing the cascade obtain locks to all children (which are held in
         // each child's graph parent) and then check for the existence of restricting children.
         if( !is_cascade_op )
         {
            map< string, set< string > > instance_keys;
            if( !obtain_cascade_locks_for_destroy( instance, instance, instance_keys ) )
            {
               handler.release_locks_for_owner( instance, true );

               if( p_rc )
               {
                  *p_rc = e_instance_op_rc_child_locked;

                  instance_accessor.set_op( old_op, false );
                  instance_accessor.set_in_op_begin( false );

                  return;
               }
               else
                  throw runtime_error( "instance '" + key_for_op
                   + "' cannot be destroyed as a lock to a dependent child could not be obtained" );
            }

            instance_keys.clear( );

            if( !session_skip_is_constained( ) )
               is_constrained = is_child_constrained( instance, instance, constraining_class, instance_keys );
         }

         if( is_constrained )
         {
            handler.release_locks_for_owner( instance, true );

            if( p_rc )
            {
               *p_rc = e_instance_op_rc_constrained;

               instance_accessor.set_op( old_op, false );
               instance_accessor.set_in_op_begin( false );

               return;
            }
            else
               throw runtime_error( get_string_message( GS( c_str_record_constrained ),
                make_pair( c_str_parm_record_constrained_class, constraining_class ) ) );
         }
      }

      lock_holder.release( );
      xlock_holder.release( );
   }
   catch( ... )
   {
      instance_accessor.set_op( old_op, false );
      instance_accessor.set_in_op_begin( false );

      if( op == e_instance_op_destroy && !is_cascade_op )
         handler.release_locks_for_owner( instance, true );

      throw;
   }

   instance_accessor.set_in_op_begin( false );
   instance_accessor.set_lock_handle( lock_handle );

   if( xlock_handle )
      instance_accessor.set_xlock_handle( xlock_handle );

   TRACE_LOG( TRACE_CLASSOPS, "begin (leave) op = "
    + instance_op_name( op ) + ", class = " + instance.get_class_name( )
    + ", internal = " + to_string( internal_modification ) + ", key = " + key );
}

void finish_instance_op( class_base& instance, bool apply_changes,
 bool internal_operation, instance_op_rc* p_rc, set< string >* p_fields_set )
{
   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   if( p_rc )
      *p_rc = e_instance_op_rc_okay;

   class_base::op_type op = instance.get_op( );

   if( op == class_base::e_op_type_none )
      return;

   TRACE_LOG( TRACE_CLASSOPS, "finish (enter) op = "
    + instance_op_name( op ) + ", class = " + instance.get_class_name( )
    + ", internal = " + to_string( internal_operation )
    + ", apply_changes = " + to_string( apply_changes ) + ", key = " + instance.get_key( ) );

   class_base_accessor instance_accessor( instance );
   storage_handler& handler( *gtp_session->p_storage_handler );

   if( !apply_changes || op == class_base::e_op_type_review )
      perform_op_cancel( handler, instance, op );
   else
   {
      // NOTE: In order to make "minimal" updates as minimal as possible both the "to_store" trigger function
      // and validation is being skipped, so such updates must only be used in circumstances when it is known
      // that they will not affect the record's validity (i.e. not to be used lightly).
      if( op == class_base::e_op_type_create
       || ( op == class_base::e_op_type_update && !instance.get_is_minimal_update( ) ) )
      {
         bool valid = true;
         // NOTE: Validation can be switched off as an optimisation during a "storage restore".
         if( p_fields_set && !session_skip_validation( ) && !instance.is_valid( internal_operation, p_fields_set ) )
            valid = false;

         if( valid )
         {
            instance_accessor.perform_to_store( op == class_base::e_op_type_create, internal_operation );

            if( !session_skip_validation( ) && !instance.is_valid( internal_operation ) )
               valid = false;
         }

         if( !valid )
         {
            if( p_rc )
            {
               *p_rc = e_instance_op_rc_invalid;
               return;
            }
            else
            {
               string validation_error( instance.get_validation_errors( class_base::e_validation_errors_type_first_only ) );

               perform_op_cancel( handler, instance, op );
               set_session_variable( get_special_var_name( e_special_var_val_error ), validation_error );

               throw runtime_error( validation_error );
            }
         }
      }

      ods* p_ods( ods::instance( ) );

      bool op_is_in_transaction( !gtp_session->transactions.empty( ) );

      if( instance_accessor.get_lock_handle( )
       && ( !handler.has_lock_info( instance_accessor.get_lock_handle( ) ) || handler.get_lock_info(
       instance_accessor.get_lock_handle( ) ).transaction_level != p_ods->get_transaction_level( ) ) )
         throw runtime_error( "attempt to perform apply for operation commenced outside the current transaction scope" );

      // NOTE: If a "clone" lock had been obtained when the op was started then release it
      // now in order to allow "for_store" or "for_destroy" to perform operations with the
      // clone record itself if required.
      size_t xlock_handle( instance_accessor.get_xlock_handle( ) );
      if( xlock_handle )
      {
         release_obtained_lock( xlock_handle );
         instance_accessor.set_xlock_handle( 0 );
      }

      bool executing_sql = false;
      try
      {
         // NOTE: The "for_store" or "for_destroy" triggers may result in further create, update
         // or destroy operations so a transaction must be commenced here (unless already in one).
         transaction tx( !op_is_in_transaction );

         if( op == class_base::e_op_type_destroy )
            instance_accessor.for_destroy( internal_operation );
         else if( op == class_base::e_op_type_create
          || ( op == class_base::e_op_type_update && !instance.get_is_minimal_update( ) ) )
         {
            instance_accessor.for_store( op == class_base::e_op_type_create, internal_operation );

            // NOTE: As it's possible that "for_store" might inadvertantly have made the record invalid
            // the validation call is repeated now and the first error (if any is found) will be thrown
            // (this is referred to as "secondary validation" and an object variable is being set first
            // so that the rules code can detect this in situations such as when a foreign key record's
            // field might have needed to be a certain value in "to_store" but has been changed by code
            // that was executed in "for_store").
            temporary_object_variable tmp_object_secondary_validation(
             instance_accessor.get_obj( ), get_special_var_name( e_special_var_secondary_validation ), "1" );

            if( !session_skip_validation( ) && !instance.is_valid( internal_operation ) )
            {
               string validation_error( instance.get_validation_errors( class_base::e_validation_errors_type_first_only ) );

               perform_op_cancel( handler, instance, op );
               set_session_variable( get_special_var_name( e_special_var_val_error ), validation_error );

               throw runtime_error( validation_error );
            }
         }

         bool is_using_blockchain = handler.is_using_blockchain( );

         if( instance.get_persistence_type( ) == 0 ) // i.e. SQL persistence
         {
            vector< string > sql_stmts;
            vector< string > sql_undo_stmts;

            vector< string >* p_sql_undo_stmts = 0;

            if( is_using_blockchain )
               p_sql_undo_stmts = &sql_undo_stmts;

            if( !instance_accessor.get_sql_stmts( sql_stmts, gtp_session->tx_key_info, p_sql_undo_stmts ) )
               throw runtime_error( "unexpected get_sql_stmts failure" );

            // NOTE: If updating but no fields apart from the revision one were changed (by any
            // derivation) then all update statements are discarded to skip the unnecessary SQL.
            if( op == class_base::e_op_type_update && instance_accessor.has_skipped_empty_update( ) )
               sql_stmts.clear( );

            if( !sql_stmts.empty( ) && gtp_session->ap_db.get( ) )
            {
               executing_sql = true;

               for( size_t i = 0; i < sql_stmts.size( ); i++ )
               {
                  if( sql_stmts[ i ].empty( ) )
                     continue;

                  TRACE_LOG( TRACE_SQLSTMTS, sql_stmts[ i ] );
                  exec_sql( *gtp_session->ap_db, sql_stmts[ i ] );

                  ++gtp_session->sql_count;
               }
            }

            executing_sql = false;

            if( is_using_blockchain )
               gtp_session->sql_undo_statements.insert(
                gtp_session->sql_undo_statements.end( ), sql_undo_stmts.begin( ), sql_undo_stmts.end( ) );
         }
         else if( instance.get_persistence_type( ) == 1 // i.e. ODS global persistence
          && instance.get_variable( get_special_var_name( e_special_var_skip_persistance ) ).empty( ) )
         {
            if( op == class_base::e_op_type_create
             || op == class_base::e_op_type_update || op == class_base::e_op_type_destroy )
            {
               string persistence_extra( instance.get_persistence_extra( ) );

               string root_child_folder( persistence_extra );
               bool is_file_not_folder( global_storage_persistence_is_file( root_child_folder ) );

               ods::bulk_write bulk_write( *gap_ods );
               scoped_ods_instance ods_instance( *gap_ods );

               gap_ofs->set_root_folder( root_child_folder );

               if( op == class_base::e_op_type_destroy )
               {
                  if( is_file_not_folder && gap_ofs->has_file( instance.get_key( ) ) )
                     gap_ofs->remove_file( instance.get_key( ) );
                  else if( !is_file_not_folder && gap_ofs->has_folder( instance.get_key( ) ) )
                     gap_ofs->remove_folder( instance.get_key( ), 0, true );
               }
               else
               {
                  stringstream sio_data;
                  auto_ptr< sio_writer > ap_sio_writer;

                  if( is_file_not_folder )
                     ap_sio_writer.reset( new sio_writer( sio_data ) );

                  bool had_any_non_transients = false;
                  int num_fields = instance.get_num_fields( );

                  if( !is_file_not_folder )
                  {
                     if( !gap_ofs->has_folder( instance.get_key( ) ) )
                        gap_ofs->add_folder( instance.get_key( ) );

                     gap_ofs->set_folder( instance.get_key( ) );
                  }

                  // NOTE: If is stored as a file then attributes are expected to be
                  // in the format of a structured I/O file otherwise each attribute
                  // is expected to be a file within the record's folder.
                  for( int i = 0; i < num_fields; i++ )
                  {
                     if( instance.is_field_transient( i ) )
                        continue;

                     had_any_non_transients = true;

                     string data( instance.get_field_value( i ) );
                     string attribute_name( lower( instance.get_field_name( i ) ) );

                     if( !is_file_not_folder )
                        gap_ofs->store_as_text_file( attribute_name, data );
                     else
                        ap_sio_writer->write_attribute( attribute_name, data );
                  }

                  if( is_file_not_folder )
                  {
                     if( had_any_non_transients )
                     {
                        ap_sio_writer->finish_sections( );
                        gap_ofs->store_file( instance.get_key( ), 0, &sio_data );
                     }
                     else
                        gap_ofs->store_file( instance.get_key( ), c_file_zero_length );
                  }
               }
            }
         }

         // NOTE: In order to be able to create child records (or to review the just created instance)
         // the "create" lock is downgraded to an "update" lock after the SQL is executed but prior to
         // the transaction being committed. If the SQL engine allows "phantom" reads then it could be
         // possible for data to become corrupted if the transaction does not end up being committed.
         if( op == class_base::e_op_type_create )
            handler.transform_lock( instance_accessor.get_lock_handle( ), op_lock::e_lock_type_update );

         // NOTE: Although "after_store" is normally skipped for "minimal" updates in the case of Meta
         // it must still be called so that "aliased" class artifacts will behave as would be expected.
         if( op == class_base::e_op_type_destroy )
            instance_accessor.after_destroy( internal_operation );
         else if( op == class_base::e_op_type_create || ( op == class_base::e_op_type_update
          && ( handler.get_name( ) == "Meta" || !instance.get_is_minimal_update( ) ) ) )
         {
            class_after_store cas( instance );
            instance_accessor.after_store( op == class_base::e_op_type_create, internal_operation );
         }

         const string& key( instance.get_key( ) );
         storage_handler& handler( *gtp_session->p_storage_handler );

         if( !op_is_in_transaction )
            tx.commit( );

         // FUTURE: It would be more efficient not to call "release_locks_for_owner" unless
         // it is known that cascade locks had actually been obtained when the destroy began.
         // Note that this call also occurs in "perform_op_cancel" (when revisited it should
         // be refactored into "class_base" where the other locks are being released).
         if( op == class_base::e_op_type_destroy && !instance.get_is_being_cascaded( ) )
            handler.release_locks_for_owner( instance );

         instance_accessor.finish( );
      }
      catch( exception& e )
      {
         string field;

         perform_op_cancel( handler, instance, op );

         // NOTE: A failed INSERT or UPDATE could be due to a unique index violation
         // so query all unique indexes to determine if indeed this has occurred and
         // if so then report the error against the last field of the smallest index.
         if( executing_sql
          && ( op == class_base::e_op_type_create || op == class_base::e_op_type_update ) )
         {
            vector< string > all_unique_indexes;
            instance.get_sql_unique_indexes( all_unique_indexes );

            vector< string > all_column_names;
            instance.get_sql_column_names( all_column_names );

            vector< string > all_column_values;
            instance.get_sql_column_values( all_column_values );

            map< string, size_t > column_numbers;
            for( size_t i = 0; i < all_column_names.size( ); i++ )
               column_numbers.insert( make_pair( all_column_names[ i ], i ) );

            size_t num_columns = 0;
            for( size_t i = 0; i < all_unique_indexes.size( ); i++ )
            {
               vector< string > unique_index_columns;
               split( all_unique_indexes[ i ], unique_index_columns );

               string sql( "SELECT C_Key_ FROM T_" + string( instance.get_module_name( ) )
                + "_" + instance.get_class_name( ) + " WHERE " );

               for( size_t j = 0; j < unique_index_columns.size( ); j++ )
               {
                  if( column_numbers.count( unique_index_columns[ j ] ) == 0 )
                     throw runtime_error( "unexpected column name '" + unique_index_columns[ j ] + "'" );

                  if( j > 0 )
                     sql += " AND ";
                  sql += all_column_names[ column_numbers[ unique_index_columns[ j ] ] ];
                  sql += " = " + all_column_values[ column_numbers[ unique_index_columns[ j ] ] ];
               }

               TRACE_LOG( TRACE_SQLSTMTS, sql );

               sql_dataset ds( *gtp_session->ap_db.get( ), sql );
               if( ds.next( ) && ds.as_string( 0 ) != instance.get_key( ) )
               {
                  if( field.empty( ) || unique_index_columns.size( ) < num_columns )
                  {
                     num_columns = unique_index_columns.size( );
                     field = unique_index_columns[ unique_index_columns.size( ) - 1 ].substr( 2 );
                  }
               }
            }
         }

         if( field.empty( ) )
            throw;
         else
         {
            TRACE_LOG( TRACE_SQLSTMTS, e.what( ) );

            throw runtime_error( get_string_message(
             GS( c_str_index_duplicate ), make_pair( c_str_parm_index_duplicate_field,
             instance.get_field_display_name( instance_accessor.get_field_id( field ) ) ),
             make_pair( c_str_parm_index_duplicate_value,
             instance.get_field_value( instance.get_field_num( field ) ) ),
             make_pair( c_str_parm_index_duplicate_class, instance.get_display_name( ) ) ) );
         }
      }
   }

   instance_accessor.set_ver_exp( "" );

   TRACE_LOG( TRACE_CLASSOPS, "finish (leave) op = "
    + instance_op_name( op ) + ", class = " + instance.get_class_name( )
    + ", internal = " + to_string( internal_operation )
    + ", apply_changes = " + to_string( apply_changes ) + ", key = " + instance.get_key( ) );
}

void perform_instance_fetch( class_base& instance,
 const string& key_info, instance_fetch_rc* p_rc, bool only_sys_fields, bool do_not_use_cache )
{
   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   if( key_info.empty( ) )
      throw runtime_error( "cannot fetch "
       + instance.get_class_name( ) + " record without key information" );

   if( p_rc )
      *p_rc = e_instance_fetch_rc_okay;

   string class_id( instance.get_class_id( ) );
   string module_name( instance.get_module_name( ) );

   class_base_accessor instance_accessor( instance );
   storage_handler& handler( *gtp_session->p_storage_handler );

   if( instance.get_is_in_op( ) && !instance_accessor.get_in_op_begin( ) )
      throw runtime_error( "cannot fetch "
       + instance.get_class_name( ) + " record whilst currently perfoming an instance operation" );

   bool found_in_cache = false;
   bool has_simple_keyinfo = ( key_info.find( ' ' ) == string::npos );

   bool has_tx_key_info = false;

   if( has_simple_keyinfo )
      has_tx_key_info = gtp_session->tx_key_info.count( instance.get_class_id( ) + ":" + key_info );

   if( !has_tx_key_info && !do_not_use_cache )
      found_in_cache = fetch_instance_from_cache( instance, key_info, only_sys_fields );

   bool found = found_in_cache;

   if( !found )
   {
      if( instance.get_persistence_type( ) == 0 ) // i.e. SQL persistence
      {
         string sql;
         vector< string > field_info;
         vector< string > order_info;
         vector< pair< string, string > > query_info;
         vector< pair< string, string > > fixed_info;
         vector< pair< string, string > > paging_info;

         split_key_info( key_info, fixed_info, paging_info, order_info, true );

         sql = construct_sql_select( instance, field_info, order_info,
          query_info, fixed_info, paging_info, "", false, true, 1, only_sys_fields, "" );

         found = fetch_instance_from_db( instance, sql,
          only_sys_fields, false, has_simple_keyinfo && !has_tx_key_info );
      }
      else if( instance.get_persistence_type( ) == 1 ) // i.e. ODS global persistence
         found = fetch_instance_from_global_storage( instance, key_info );
   }

   if( !found )
   {
      if( p_rc )
         *p_rc = e_instance_fetch_rc_not_found;
      else
      {
         string keys( key_info );

         string::size_type pos = keys.find( ' ' );
         if( pos != string::npos )
            keys.erase( 0, pos + 1 );

         throw runtime_error( get_string_message( GS( c_str_record_not_found ),
          make_pair( c_str_parm_record_not_found_class, instance.get_display_name( ) ),
          make_pair( c_str_parm_record_not_found_key, keys ) ) );
      }
   }
}

bool perform_instance_iterate( class_base& instance,
 const string& key_info, const string& fields, const string& text,
 const string& query, const string& security_info, iter_direction direction,
 bool inclusive, int row_limit, sql_optimisation optimisation, const set< string >* p_filters )
{
   bool found = false;

   TRACE_LOG( TRACE_CLASSOPS, "[iterate] class = '" + instance.get_class_name( )
    + "', key_info = '" + key_info + "', fields = '" + fields + "', direction = "
    + to_string( direction ) + ", text = '" + text + "', query = '" + query + "'" );

   oid id;
   string sql, key;
   size_t num_fields = 0;

   string class_id( instance.get_class_id( ) );
   string module_name( instance.get_module_name( ) );

   class_base_accessor instance_accessor( instance );
   storage_handler& handler( *gtp_session->p_storage_handler );

   // NOTE: If row_limit < 0 then iteration is being continued.
   if( row_limit >= 0 && instance.get_is_iterating( ) )
   {
      string class_name( instance.get_class_name( ) );
      class_base* p_parent = instance.get_graph_parent( );

      while( p_parent )
      {
         class_name += string( " <- " ) + p_parent->get_class_name( );
         p_parent = p_parent->get_graph_parent( );
      }

      throw runtime_error( "iterate called whilst already iterating (class: " + class_name + ")" );
   }

   // NOTE: Because filtering can exclude records from the DB fetch the limit must be
   // omitted if any filters have been supplied.
   if( p_filters )
   {
      row_limit = 0;
      instance_accessor.filters( ) = *p_filters;
   }

   if( instance.get_is_in_op( ) && !instance_accessor.get_in_op_begin( ) )
      throw runtime_error( "cannot begin iteration whilst currently perfoming an instance operation" );
   else
   {
      if( row_limit >= 0 && key_info != c_nul_key )
      {
         vector< string > field_info;
         vector< string > order_info;
         vector< pair< string, string > > query_info;
         vector< pair< string, string > > fixed_info;
         vector< pair< string, string > > paging_info;

         string group_key_marker;
         vector< string > group_keys;

         set< string > supplied_fields;
         set< string > transient_field_names;

         int num_fields = instance.get_num_fields( );
         for( int i = 0; i < num_fields; i++ )
         {
            if( instance.is_field_transient( i ) )
               transient_field_names.insert( instance.get_field_name( i ) );
         }

         instance_accessor.transient_filter_field_values( ).clear( );

         if( !fields.empty( ) && fields != c_key_field )
         {
            set< string > fetch_field_names;

            vector< string > tmp_field_info;
            split( fields, tmp_field_info );

            for( size_t i = 0; i < tmp_field_info.size( ); i++ )
            {
               if( tmp_field_info[ i ].find( '.' ) == string::npos )
               {
                  string next( tmp_field_info[ i ] );

                  if( next != c_ignore_field )
                  {
                     get_field_name( instance, next );

                     fetch_field_names.insert( next );

                     if( !transient_field_names.count( next ) )
                     {
                        field_info.push_back( tmp_field_info[ i ] );
                        supplied_fields.insert( tmp_field_info[ i ] );
                     }
                  }
               }
            }

            instance_accessor.fetch_field_names( ) = fetch_field_names;

            // NOTE: If there are fields that are required in order to determine
            // state correctly then these fields are appended to the field list.
            set< string > required_fields;
            set< string > field_dependents;
            instance.get_required_field_names( required_fields, false, &field_dependents );

            int iterations = 0;

            // NOTE: It is possible that due to "interdependent" required fields
            // some required fields may not have been added in the first or even
            // later calls to "get_required_field_names" so continue calling the
            // function until no further field names (or deps) have been added.
            size_t required_fields_size = required_fields.size( );
            size_t field_dependents_size = field_dependents.size( );

            while( required_fields_size )
            {
               instance.get_required_field_names( required_fields, false, &field_dependents );

               if( required_fields.size( ) == required_fields_size
                && field_dependents_size == field_dependents.size( ) )
                  break;

               if( ++iterations > 100 )
                  throw runtime_error( "unexpected excessive get_required_field_names( ) iterations" );

               required_fields_size = required_fields.size( );
               field_dependents_size = field_dependents.size( );
            }

            for( set< string >::iterator i = required_fields.begin( ); i != required_fields.end( ); ++i )
            {
               if( !supplied_fields.count( *i ) )
               {
                  field_info.push_back( *i );
                  instance_accessor.fetch_field_names( ).insert( *i );
               }
            }
         }

         // NOTE: If this class instance is its graph parent's "child" then the iteration is
         // restricted to those instances that belong to the parent (via a non-primary index).
         if( instance.get_graph_parent( ) && !instance.get_is_singular( ) )
         {
            string fk_field = instance.get_graph_parent_fk_field( );

            bool is_transient = instance.is_field_transient( instance.get_field_num( fk_field ) );

            if( !is_transient )
            {
               order_info.push_back( fk_field );
               string parent_key_value( instance.get_graph_parent( )->get_key( ) );

               // NOTE: If the parent key value has one or more pipe separators then
               // it will instead considered to be multiple keys in a "query group".
               if( parent_key_value.find( '|' ) != string::npos )
               {
                  split( parent_key_value, group_keys, '|' );
                  parent_key_value = group_key_marker = uuid( ).as_string( );
               }

               fixed_info.push_back( make_pair( fk_field, parent_key_value ) );
            }
            else
            {
               vector< string > replacements;
               instance.get_transient_replacement_field_names( fk_field, replacements );

               for( size_t i = 0; i < replacements.size( ); i++ )
                  fixed_info.push_back( make_pair( replacements[ i ], instance.get_graph_parent( )->get_key( ) ) );
            }
         }

         instance_accessor.add_extra_fixed_info( fixed_info );
         instance_accessor.add_extra_paging_info( paging_info );

         // NOTE: If the key info contains any transient field names (for ordering)
         // then these need to be replaced by zero or more persistent field names.
         string keys( key_info );

         string extra_key_info;
         string final_key_info;

         size_t pos = keys.find_first_of( " #" );
         if( pos != string::npos )
         {
            extra_key_info = keys.substr( pos );
            keys.erase( pos );
         }

         if( pos == string::npos )
            final_key_info = key_info;
         else
         {
            vector< string > all_keys;
            vector< string > extra_key_values;

            if( !keys.empty( ) )
            {
               split( keys, all_keys );

               pos = extra_key_info.find( ' ' );
               if( pos != string::npos && pos != extra_key_info.size( ) - 1 )
               {
                  split( extra_key_info.substr( pos + 1 ), extra_key_values );
                  extra_key_info.erase( pos + 1 );
               }
            }

            // NOTE: If we have one more extra key value then assume it is that of the primary key.
            if( extra_key_values.size( ) == all_keys.size( ) + 1 )
               all_keys.push_back( get_special_var_name( e_special_var_key ) );

            bool first_extra = true;
            vector< string > final_keys;
            for( size_t i = 0; i < all_keys.size( ); i++ )
            {
               string next( all_keys[ i ] );
               get_field_name( instance, next );
   
               bool add_extra = false;
               if( !transient_field_names.count( next ) )
               {
                  add_extra = true;
                  final_keys.push_back( next );
               }
               else
               {
                  size_t size = final_keys.size( );
                  instance.get_transient_replacement_field_names( next, final_keys );

                  if( size != final_keys.size( ) )
                     add_extra = true;
                  else
                  {
                     string next_value;
                     if( i < extra_key_values.size( ) )
                     {
                        next_value = escaped( extra_key_values[ i ], "," );

                        // NOTE: If transients have been used as restricts, and no non-transient replacement
                        // was provided, then tests for record filtering based upon each and every transient
                        // restriction will need to occur after the record has been fetched from the DB. The
                        // row limit therefore must be cleared as it is not known which records may later be
                        // filtered.
                        row_limit = 0;
                        instance_accessor.transient_filter_field_values( ).insert( make_pair( next, next_value ) );
                     }
                  }
               }

               if( add_extra )
               {
                  if( i < extra_key_values.size( ) )
                  {
                     if( !first_extra )
                        extra_key_info += ',';

                     first_extra = false;
                     extra_key_info += escaped( extra_key_values[ i ], "," );
                  }
               }
            }

            if( !extra_key_info.empty( ) && extra_key_info[ 0 ] == '#'
             && !instance_accessor.transient_filter_field_values( ).empty( ) )
            {
               extra_key_info[ 1 ] -= ( unsigned char )instance_accessor.transient_filter_field_values( ).size( );
            }

            for( size_t i = 0; i < final_keys.size( ); i++ )
            {
               if( !final_key_info.empty( ) )
                  final_key_info += ',';
               final_key_info += final_keys[ i ];
            }

            final_key_info += extra_key_info;
         }

         split_key_info( final_key_info, fixed_info,
          paging_info, order_info, ( optimisation == e_sql_optimisation_unordered ) );

         if( !query.empty( ) )
         {
            vector< string > query_parts;
            split( query, query_parts );

            for( size_t i = 0; i < query_parts.size( ); i++ )
            {
               string::size_type pos = query_parts[ i ].find( ':' );
               if( pos == string::npos )
                  pos = query_parts[ i ].find( '=' ); // i.e. allow for either <field>:<value_info> or <field>=<value_info>

               if( pos == string::npos )
                  throw runtime_error( "unexpected query part format '" + query_parts[ i ] + "'" );

               string field_name( query_parts[ i ].substr( 0, pos ) );
               string field_values( query_parts[ i ].substr( pos + 1 ) );

               query_info.push_back( make_pair( field_name, field_values ) );
            }
         }

         if( instance.get_persistence_type( ) == 0 ) // i.e. SQL persistence
         {
            sql = construct_sql_select( instance,
             field_info, order_info, query_info, fixed_info, paging_info, security_info,
             ( direction == e_iter_direction_backwards ), inclusive, row_limit, ( fields == c_key_field ), text );

            if( instance_accessor.p_sql_data( ) )
               delete instance_accessor.p_sql_data( );

            if( !group_keys.empty( ) )
            {
               vector< string > sql_stmts;

               for( size_t i = 0; i < group_keys.size( ); i++ )
               {
                  sql_stmts.push_back( replaced( sql, group_key_marker, group_keys[ i ] ) );

                  TRACE_LOG( TRACE_SQLSTMTS, sql_stmts.back( ) );
               }

               instance_accessor.p_sql_data( ) = new sql_dataset_group(
                *gtp_session->ap_db, sql_stmts, ( direction == e_iter_direction_backwards ), true );
            }
            else
            {
               TRACE_LOG( TRACE_SQLSTMTS, sql );

               instance_accessor.p_sql_data( ) = new sql_dataset( *gtp_session->ap_db, sql );
            }

            setup_select_columns( instance, field_info );
         }
      }

      vector< string > global_keys;

      bool skip_after_fetch = false;

      string skip_after_fetch_var(
       instance.get_raw_variable( get_special_var_name( e_special_var_skip_after_fetch ) ) );

      if( skip_after_fetch_var == "1" || skip_after_fetch_var == "true" )
         skip_after_fetch = true;

      size_t row_cache_limit = c_iteration_row_cache_limit;

      string row_cache_limit_value(
       get_session_variable( get_special_var_name( e_special_var_row_cache_limit ) ) );

      if( !row_cache_limit_value.empty( ) )
         row_cache_limit = from_string< size_t >( row_cache_limit_value );

      if( row_cache_limit < 2 )
         throw runtime_error( "unexpected invalid < 2 row_cache_limit" );

      if( row_limit < 0 && instance.get_persistence_type( ) != 0 ) // i.e. SQL persistence
         row_limit = 0;

      if( row_limit < 0 )
         found = true;
      else
      {
         // NOTE: Unless a single row limit was specified (which is an alternate way of performing an
         // instance fetch) then iteration is flagged so that "after_fetch" triggers can detect this.
         if( row_limit != 1 )
         {
            if( !instance.get_is_iterating( ) )
            {
               instance.set_variable(
                get_special_var_name( e_special_var_loop ),
                to_comparable_string( 0, false, c_loop_variable_digits ) );

               instance_accessor.set_is_in_iteration( true, direction == e_iter_direction_forwards );
            }
         }

         if( instance.get_persistence_type( ) == 0 ) // i.e. SQL persistence
         {
            found = fetch_instance_from_db( instance,
             instance_accessor.select_fields( ), instance_accessor.select_columns( ), skip_after_fetch );
         }
         else if( instance.get_persistence_type( ) == 1 ) // i.e. ODS global persistence
         {
            size_t limit = row_limit > 0 ? row_limit : row_cache_limit;

            if( limit > row_cache_limit )
               limit = row_cache_limit;

            string start_from( key_info );
            if( key_info == c_nul_key )
               start_from = instance.get_key( );

            fetch_keys_from_global_storage( instance,
             start_from, inclusive, limit, global_keys, ( direction == e_iter_direction_backwards ) );

            found = ( global_keys.size( ) > 0 );
         }

         // NOTE: It is expected that the "after_fetch" trigger will be being skipped due to a later
         // "prepare" call which will call the trigger and then clear this flag (otherwise dependent
         // fields might be missed when the trigger is actually called).
         if( !skip_after_fetch )
            instance_accessor.set_iteration_starting( false );

         if( instance.get_persistence_type( ) == 0 ) // i.e. SQL persistence
         {
            ++gtp_session->sql_count;

            IF_IS_TRACING( TRACE_SQLSTMTS )
            {
               string sql_plan( "EXPLAIN " + sql );
               sql_dataset ds( *gtp_session->ap_db, sql_plan );

               sql_plan = "QUERY PLAN:";
               while( ds.next( ) )
               {
                  sql_plan += '\n';

                  for( size_t i = 0; i < ds.get_fieldcount( ); i++ )
                  {
                     if( i > 0 )
                        sql_plan += " | ";
                     sql_plan += ds.as_string( i );
                  }
               }

               TRACE_LOG( TRACE_SQLSTMTS, sql_plan );
            }
         }
      }

      if( found )
      {
         bool found_next = false;
         bool query_finished = true;

         deque< vector< string > > rows;

         if( instance.get_persistence_type( ) == 0 ) // i.e. SQL persistence
         {
            if( !instance_accessor.p_sql_data( ) )
               throw runtime_error( "unexpected null sql_data in perform_instance_iterate" );

            sql_data& sd( *instance_accessor.p_sql_data( ) );

            while( sd.next( ) )
            {
               found_next = true;

               vector< string > columns;
               int fcount = sd.get_fieldcount( );

               for( int i = 0; i < fcount; i++ )
                  columns.push_back( sd.as_string( i ) );

               rows.push_back( columns );

               if( rows.size( ) == row_cache_limit )
               {
                  query_finished = false;
                  break;
               }
            }
         }
         else if( instance.get_persistence_type( ) == 1 ) // i.e. ODS global persistence
         {
            if( !global_keys.empty( ) )
            {
               vector< int > field_nums;
               vector< string > field_names;
               int num_fields = instance.get_num_fields( );

               for( int i = 0; i < num_fields; i++ )
               {
                  if( !instance.is_field_transient( i ) )
                  {
                     field_nums.push_back( i );
                     field_names.push_back( instance.get_field_name( i ) );
                  }
               }

               instance_accessor.field_nums( ) = field_nums;

               for( size_t i = 0; i < global_keys.size( ); i++ )
               {
                  if( i == 0 )
                  {
                     fetch_instance_from_global_storage(
                      instance, global_keys[ i ], field_names, 0, skip_after_fetch );
                  }
                  else
                  {
                     vector< string > columns;

                     if( !fetch_instance_from_global_storage( instance, global_keys[ i ], field_names, &columns ) )
                        break;

                     found_next = true;
                     rows.push_back( columns );
                  }

                  if( rows.size( ) == row_cache_limit - 1 )
                  {
                     query_finished = false;
                     break;
                  }
               }
            }
         }

         // NOTE: Put a dummy row at the end to stop iteration.
         if( query_finished && ( found_next || key_info != c_nul_key ) )
            rows.push_back( vector< string >( ) );

         instance_accessor.row_cache( ) = rows;

         if( key_info == c_nul_key )
         {
            if( !found_next )
               found = false;
            else
               fetch_instance_from_row_cache( instance, skip_after_fetch );
         }

         if( query_finished )
         {
            if( instance.get_persistence_type( ) == 0 ) // i.e. SQL persistence
            {
               delete instance_accessor.p_sql_data( );
               instance_accessor.p_sql_data( ) = 0;
            }
         }
      }

      if( !found )
      {
         if( instance.get_persistence_type( ) == 0 ) // i.e. SQL persistence
         {
            if( instance_accessor.p_sql_data( ) )
            {
               delete instance_accessor.p_sql_data( );
               instance_accessor.p_sql_data( ) = 0;
            }
         }

         instance_accessor.transient_filter_field_values( ).clear( );

         instance_accessor.set_is_in_iteration( false );
      }
   }

   return found;
}

bool perform_instance_iterate_next( class_base& instance )
{
   if( !instance.get_is_iterating( ) )
      return false;

   string class_id( instance.get_class_id( ) );
   string module_name( instance.get_module_name( ) );

   class_base_accessor instance_accessor( instance );
   storage_handler& handler( *gtp_session->p_storage_handler );

   if( instance.get_is_in_op( ) && !instance_accessor.get_in_op_begin( ) )
      throw runtime_error( "cannot continue iteration whilst currently perfoming an instance operation" );

   bool found = false, cache_depleted = false;
   if( !instance_accessor.row_cache( ).empty( ) )
   {
      if( instance_accessor.row_cache( ).size( ) == 1 && instance_accessor.row_cache( )[ 0 ].empty( ) )
      {
         cache_depleted = true;
         instance.iterate_stop( );
      }
      else
      {
         found = true;

         bool skip_after_fetch = false;

         string skip_after_fetch_var(
          instance.get_raw_variable( get_special_var_name( e_special_var_skip_after_fetch ) ) );

         if( skip_after_fetch_var == "1" || skip_after_fetch_var == "true" )
            skip_after_fetch = true;

         fetch_instance_from_row_cache( instance, skip_after_fetch );
      }
   }

   int loop_num = atoi( instance.get_raw_variable( get_special_var_name( e_special_var_loop ) ).c_str( ) );

   instance.set_variable( get_special_var_name( e_special_var_loop ),
    to_comparable_string( ++loop_num, false, c_loop_variable_digits ) );

   if( found || cache_depleted )
      return found;
   else
      return perform_instance_iterate( instance, c_nul_key, "", "", "", "",
       instance.get_is_in_forwards_iteration( ) ? e_iter_direction_forwards : e_iter_direction_backwards, false, -1 );
}
