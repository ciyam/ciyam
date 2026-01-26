// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_BASE_H
#  define CIYAM_BASE_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <map>
#     include <set>
#     include <vector>
#     include <iosfwd>
#     include <string>
#     include <utility>
#  endif

#  include "common.h"
#  include "macros.h"
#  include "ptypes.h"
#  include "threads.h"
#  include "ciyam_core.h"

#  define GS( s ) get_string( STRINGIZE( s ), s )

class ods;
class sql_db;
class date_time;
class class_base;
class command_handler;
class ods_file_system;

struct progress;

#  define TRACE_GENERAL 0x00000000U
#  define TRACE_LOCKING 0x00000001U
#  define TRACE_OBJECTS 0x00000002U
#  define TRACE_QUERIES 0x00000004U
#  define TRACE_SESSION 0x00000008U
#  define TRACE_SOCKETS 0x00000010U
#  define TRACE_VARIOUS 0x00000020U

#  define TRACE_COLLAGE 0x0000ffffU

#  define TRACE_MINIMAL 0x00010000U
#  define TRACE_INITIAL 0x00020000U
#  define TRACE_DETAILS 0x00040000U
#  define TRACE_VERBOSE 0x00080000U

#  define TRACE_WAITING 0x80000000U

#  define IF_IS_TRACING( flags )\
if( ( get_trace_flags( ) & ( flags ) ) == ( flags ) )

#  define IF_NOT_IS_TRACING( flags )\
if( !( ( get_trace_flags( ) & ( flags ) ) == ( flags ) ) )

#  define TRACE_LOG( flags, message )\
if( ( flags ) && ( ( get_trace_flags( ) & ( flags ) ) == ( flags ) ) )\
   log_trace_message( flags, message )

#  define TEMP_TRACE( message ) TRACE_LOG( TRACE_MINIMAL, message )

uint32_t get_trace_flags( );

void set_trace_flags( uint32_t flags );

extern "C" void trace_flags( uint32_t );

typedef void ( *fp_trace_flags )( uint32_t );

struct temp_trace_flags
{
   temp_trace_flags( uint32_t temp_flags )
   {
      flags = get_trace_flags( );

      set_trace_flags( temp_flags );
   }

   ~temp_trace_flags( )
   {
      set_trace_flags( flags );
   }

   uint32_t flags;
};

void list_trace_flags( std::ostream& os );

void trace_flag_names( const std::string& names, bool unset = false );

std::string get_trace_flag_names( );

inline void set_trace_flag_names( const std::string& names )
{
   trace_flag_names( names );
}

inline void unset_trace_flag_names( const std::string& names )
{
   trace_flag_names( names, true );
}

std::string get_trace_level( );

void set_trace_level( const std::string& level_name );

void list_trace_levels( std::vector< std::string >& level_names );

std::string get_trace_filters( );

void set_trace_filters( const std::string& filters );

std::string get_trace_session_ids( );

void set_trace_session_ids( const std::string& ids );

void log_trace_message( uint32_t flag, const std::string& message );

extern "C" void log_trace_string( uint32_t flag, const char* p_message );

typedef void ( *fp_log_trace_string )( uint32_t, const char* );

class trace_mutex : public mutex
{
   protected:
   void pre_acquire( const guard* p_guard, const char* p_msg );
   void post_acquire( const guard* p_guard, const char* p_msg );

   void has_released( const guard* p_guard, const char* p_msg );
};

void list_trace_mutex_lock_ids(
 std::ostream& os, mutex* p_mutex = 0, const char* p_mutex_name = 0 );

unsigned int get_server_port( );

extern "C" void set_server_port( unsigned int p );

typedef void ( *fp_set_server_port )( unsigned int );

unsigned int get_stream_port( );
unsigned int get_stream_sock( );

extern "C" void set_stream_socket( unsigned int p, unsigned int s );

typedef void ( *fp_set_stream_socket )( unsigned int, unsigned int );

extern "C" void register_listener( unsigned int port, const char* p_info, const char* p_id_info = 0 );
extern "C" void unregister_listener( unsigned int port, const char* p_id_info = 0 );

typedef void ( *fp_register_listener )( unsigned int, const char*, const char* );
typedef void ( *fp_unregister_listener )( unsigned int, const char* );

class listener_registration
{
   public:
   listener_registration( unsigned int port, const std::string& info, const char* p_id_info = 0 );

   ~listener_registration( );

   void insert_id( const std::string& id );
   void remove_id( const std::string& id );

   private:
   unsigned int port;

   std::string id_info;
};

bool has_registered_listener( unsigned int port );
bool has_registered_listener_id( const std::string& id, unsigned int* p_port = 0 );

void list_listeners( std::ostream& os );

extern "C" void init_globals( const char* p_sid = 0, int* p_use_udp = 0 );
extern "C" void term_globals( );

typedef void ( *fp_init_globals )( const char*, int* );
typedef void ( *fp_term_globals )( );

void resync_system_ods( progress* p_progress );

extern "C" void check_timezone_info( );

typedef void ( *fp_check_timezone_info )( );

void list_strings( std::ostream& os );

std::string get_string( const std::string& key, const char* p_default = 0 );

std::string get_string_message(
 const std::string& string_message, const std::pair< std::string, std::string >& parm1 );

std::string get_string_message( const std::string& string_message,
 const std::pair< std::string, std::string >& parm1, const std::pair< std::string, std::string >& parm2 );

std::string get_string_message( const std::string& string_message,
 const std::pair< std::string, std::string >& parm1, const std::pair< std::string, std::string >& parm2,
 const std::pair< std::string, std::string >& parm3 );

std::string get_string_message( const std::string& string_message,
 const std::pair< std::string, std::string >& parm1, const std::pair< std::string, std::string >& parm2,
 const std::pair< std::string, std::string >& parm3, const std::pair< std::string, std::string >& parm4 );

unsigned int get_max_peers( );

unsigned int get_max_user_limit( );
void set_max_user_limit( unsigned int new_limit );

std::string get_prefix( );
std::string get_domain( );

std::string get_app_url( const std::string& suffix );

void get_identity( std::string& s,
 bool append_max_user_limit = false, bool use_max_sid_entropy = false, const char* p_pubkey = 0 );

inline std::string get_identity(
 bool append_max_user_limit = false,
 bool use_max_sid_entropy = false, const char* p_pubkey = 0 )
{
   std::string s;

   get_identity( s, append_max_user_limit, use_max_sid_entropy, p_pubkey );

   return s;
}

bool has_identity( bool* p_is_encrypted = 0 );

void set_identity( const std::string& info, const char* p_encrypted_sid = 0 );

bool unlock_create_allowed( );

std::string create_unlock_sid_hash_key( bool for_web_ui = true );

std::string get_checksum( const std::string& data );

std::string get_timezone( );
std::string get_web_root( );

std::string get_ntfy_server( );

bool get_is_accepted_ip_addr( const std::string& ip_addr );
bool get_is_accepted_peer_ip_addr( const std::string& ip_addr );

extern "C" int is_accepted_ip_addr( const char* p_addr );

typedef int ( *fp_is_accepted_ip_addr )( const char* );

void add_peer_ip_addr_for_rejection( const std::string& ip_addr );
void remove_peer_ip_addr_from_rejection( const std::string& ip_addr );

std::string list_peer_ip_addrs_for_rejection( );

std::string get_non_extra_identity( const std::string& extra_identity );

inline bool is_extra_non_hub_identity( const std::string& identity )
{
   return ( get_non_extra_identity( identity ) != identity );
}

void get_extra_identities(
 const std::string& identity, std::vector< std::string >& extras );

std::string get_extra_identity_variable(
 const std::string& identity_variable_name, const std::string& extra );

std::string get_identity_variable_extra(
 const std::string& identity_variable_name, const std::string& identity_value, bool force_extra_match = false );

void identity_variable_name_prefix_and_suffix(
 const std::string& identity_variable_name, std::string& prefix, std::string& suffix );

void create_peerchain( const std::string& identity,
 const std::string& host_and_port, const std::string& description, const std::string& extra_value,
 const std::string& shared_secret, bool auto_start = true, size_t num_helpers = 5, int peer_type = 0 );

void review_peerchain( std::ostream& os, const std::string& identity, const std::string& attribute );

void update_peerchain( const std::string& identity,
 const std::string* p_host_and_port = 0, const std::string* p_description = 0,
 const std::string* p_shared_secret = 0, bool* p_auto_start = 0, size_t* p_num_helpers = 0 );

void destroy_peerchain( const std::string& identity, progress* p_progress = 0 );

std::string get_peerchain_info(
 const std::string& identity, bool* p_is_listener = 0, std::string* p_shared_secret = 0 );

void get_peerchain_externals(
 std::vector< std::string >& peerchain_externals, bool auto_start_only = true );

void get_peerchain_listeners(
 std::multimap< int, std::string >& peerchain_listeners, bool auto_start_only = true );

bool get_use_udp( );

bool get_using_ssl( );

std::string get_log_files_dir( );

extern "C" void set_log_files_dir( const char* p_dir_name );

typedef void ( *fp_set_log_files_dir )( const char* );

inline void set_log_files_dir( const std::string& log_files_dir )
{
   set_log_files_dir( log_files_dir.c_str( ) );
}

std::string get_files_area_dir( );

extern "C" void set_files_area_dir( const char* p_files_area_dir );

typedef void ( *fp_set_files_area_dir )( const char* );

inline void set_files_area_dir( const std::string& files_area_dir )
{
   set_files_area_dir( files_area_dir.c_str( ) );
}

size_t get_notifier_ignore_secs( );

size_t get_files_area_item_max_num( );
size_t get_files_area_item_max_size( );

size_t get_num_recv_stream_sessions( );
size_t get_num_send_stream_sessions( );

std::string get_mbox_path( );
std::string get_mbox_username( );

std::string get_pop3_server( );
std::string get_pop3_suffix( );
std::string get_pop3_username( );
std::string get_pop3_password( );
std::string get_pop3_security( );

std::string get_pop3_suffix_or_domain( );

std::string get_smtp_server( );
std::string get_smtp_sender( );
std::string get_smtp_suffix( );
std::string get_smtp_username( );
std::string get_smtp_password( );
std::string get_smtp_security( );

int get_smtp_max_send_attempts( );

int64_t get_smtp_max_attached_data( );

std::string get_smtp_suffix_or_domain( );

struct external_client
{
   external_client( ) : port( 0 ), is_local( false ) { }

   unsigned int port;

   bool is_local;

   std::string protocol;
   std::string extra_info;
   std::string script_name;
};

std::string list_externals( );

bool is_local_external_client( const std::string& key );

void get_external_client_info( const std::string& key, external_client& info );

void verify_active_external_service( const std::string& ext_key );

void decrypt_data( std::string& s, const std::string& data,
 bool empty_key = false, bool harden_key = false, bool is_pwd_and_data = false );

inline std::string decrypt_data( const std::string& data,
 bool empty_key = false, bool harden_key = false, bool is_pwd_and_data = false )
{
   std::string s;

   decrypt_data( s, data, empty_key, harden_key, is_pwd_and_data );

   return s;
}

void encrypt_data( std::string& s, const std::string& data, bool no_ssl = false,
 bool empty_key = false, bool use_sid_only = false, bool harden_key = false, bool is_pwd_and_data = false );

inline std::string encrypt_data( const std::string& data, bool no_ssl = false,
 bool empty_key = false, bool use_sid_only = false, bool harden_key = false, bool is_pwd_and_data = false )
{
   std::string s;

   encrypt_data( s, data, no_ssl, empty_key, use_sid_only, harden_key, is_pwd_and_data );

   return s;
}

std::string totp_secret_key( const std::string& unique );

int exec_system( const std::string& cmd, bool async = false, bool delay = false, bool* p_delayed = 0 );

int run_script( const std::string& script_name,
 bool async = true, bool delay = false, bool no_logging = false, bool sleep_after = false );

void list_scripts( const std::string& pattern, std::ostream& os );

void check_script_args( const std::string& script_name, bool* p_rc = 0 );

std::string process_script_args(
 const std::string& raw_args, bool use_system_variables = false );

bool create_script_lock_file( const std::string& name );

bool can_create_script_lock_file( const std::string& name );

void generate_new_script_sio_files( );

void init_session(
 command_handler& cmd_handler, bool is_peer_session = false,
 const std::string* p_ip_addr = 0, const std::string* p_blockchain = 0,
 int port = 0, bool is_support_session = false, bool add_pubkey_variable = true, size_t session_id = 0 );

void term_session( );

size_t reserve_session_id( size_t num_extras = 0 );

size_t session_id( );

std::string session_ip_addr( );

std::string session_ip_addr( size_t slot );

void session_is_using_tls( );

void session_inc_sql_count( );

size_t first_other_session_id( const std::string& var_name, const std::string& value );

bool has_session_with_ip_addr( const std::string& ip_addr, const std::string& blockchain );

std::string get_random_same_port_peer_ip_addr( const std::string& empty_value );

void list_sessions( std::ostream& os, bool inc_dtms = true,
 bool include_progress = true, const std::string* p_blockchains = 0 );

command_handler& get_session_command_handler( );

extern "C" void server_command( const char* p_cmd );

typedef void ( *fp_server_command )( const char* );

class session_file_buffer_access
{
   public:
   session_file_buffer_access( );

   ~session_file_buffer_access( );

   void copy_to_string(
    std::string& str, size_t offset = 0, size_t length = 0 ) const;

   void copy_from_string( const std::string& str, size_t offset = 0, bool allow_shrink = true );

   unsigned int get_size( ) { return size; }
   unsigned char* get_buffer( ) { return p_buffer; }

   private:
   unsigned int size;
   unsigned char* p_buffer;
};

void increment_peer_files_uploaded( int64_t bytes );
void increment_peer_files_downloaded( int64_t bytes );

void increment_session_commands_executed( );

void set_slowest_if_applicable( );

void session_progress_settings( size_t& seconds, progress* p_progress = 0 );

void set_session_progress_message( const std::string& progress_message );

void set_last_session_cmd( const std::string& cmd );

bool set_session_sync_time( const std::string* p_check_blockchain = 0,
 bool matching_own_ip_address = false, int num_seconds = 1, const std::string* p_sync_var_name = 0 );

void condemn_session(
 size_t sess_id, int num_seconds, bool force_uncapture, bool wait_until_term );

void condemn_this_session( );

void condemn_matching_sessions( int num_seconds = 0, bool wait_until_term = false );

void condemn_all_other_sessions( int num_seconds, bool force_uncapture, bool wait_until_term );

bool is_condemned_session( );

void capture_session( size_t sess_id );

void capture_all_other_sessions( );

bool is_captured_session( );

bool has_any_matching_session( bool support_only = true );

inline bool has_any_support_session( ) { return has_any_matching_session( true ); }
inline bool has_matching_peer_session( ) { return has_any_matching_session( false ); }

void release_session( size_t sess_id, bool wait_until_term );

void release_all_other_sessions( bool wait_until_term );

bool session_skip_fk_fetches( );
void session_skip_fk_fetches( bool skip_fk_fetches );

bool session_skip_validation( );
void session_skip_validation( bool skip_validation );

bool session_skip_is_constained( );
void session_skip_is_constained( bool skip_fk_fetches );

bool get_script_reconfig( );

std::string get_gpg_password( );
std::string get_rpc_password( );
std::string get_sql_password( );

unsigned int get_test_peer_port( );

extern "C" void set_test_peer_port( unsigned int port );

typedef int ( *fp_set_test_peer_port )( unsigned int );

std::string get_encrypted_gpg_password( );
std::string get_encrypted_pem_password( );
std::string get_encrypted_sql_password( );
std::string get_encrypted_rpc_password( );

std::string get_encrypted_pop3_password( );
std::string get_encrypted_smtp_password( );

std::string get_session_blockchain( );

std::string get_session_identity_suffix( );

size_t get_num_sessions_for_blockchain(
 const std::string& blockchain, bool matching_own_ip_address = false, bool excluding_own_session = false );

inline bool any_session_has_blockchain( const std::string& blockchain )
{
   return ( get_num_sessions_for_blockchain( blockchain ) > 0 );
}

bool get_storage_using_verbose_logging( );

unsigned int get_session_timeout( );
void set_session_timeout( unsigned int seconds );

void add_peer_file_hash_for_get( const std::string& hash,
 bool check_for_supporters = false, bool add_at_front = false, const std::string* p_hash_to_remove = 0 );

std::string top_next_peer_file_hash_to_get(
 bool take_from_supporter = false, bool* p_any_supporter_has = 0 );

void pop_next_peer_file_hash_to_get( );

void add_peer_file_hash_for_put( const std::string& hash, bool check_for_supporters = false );

void add_peer_file_hash_for_put_for_all_peers(
 const std::string& hash, const std::string* p_blockchain = 0,
 const std::string* p_session_variable = 0, bool include_self = true, size_t session_id_to_skip = 0 );

std::string top_next_peer_file_hash_to_put(
 bool take_from_supporter = false, bool* p_any_supporter_has = 0 );

void pop_next_peer_file_hash_to_put( );

bool any_peer_still_has_file_hash_to_put(
 const std::string& hash, const std::string* p_blockchain = 0 );

void add_peer_mapped_hash_info(
 const std::string& key, const std::string& hash, const std::string& info );

std::string get_peer_mapped_hash_info( const std::string& key, const std::string& hash );

void clear_peer_mapped_hash( const std::string& key, const std::string& hash );

void clear_all_peer_mapped_hashes( const std::string& key );

void set_default_session_variables( int port = 0 );

bool has_raw_session_variable( const std::string& name, size_t sess_id = 0 );

std::string get_raw_session_variable( const std::string& name, size_t sess_id = 0 );

bool has_session_variable( const std::string& name_or_expr, const std::string* p_sess_id = 0 );

std::string get_session_variable( const std::string& name, size_t slot );
std::string get_session_variable( const std::string& name_or_expr, const std::string* p_sess_id = 0 );

std::string get_session_variable_from_matching_blockchain( const std::string& name,
 const std::string& check_name, const std::string& check_value, bool matching_own_ip_address = false );

bool has_mismatched_variables_for_matching_blockchains( const std::string& name,
 const std::string& value, const std::string* p_name = 0, const std::string* p_value = 0 );

void set_session_variable(
 const std::string& name, const std::string& value,
 bool* p_set_special_temporary = 0, command_handler* p_command_handler = 0, const std::string* p_sess_id = 0 );

bool set_session_variable(
 const std::string& name, const std::string& value, const std::string& current );

void set_session_variable_for_matching_blockchains( const std::string& name,
 const std::string& value, const std::string& check_name, const std::string& check_value, bool matching_own_ip_address = false );

inline void set_session_variable_for_matching_blockchains(
 const std::string& name, const std::string& value, bool matching_own_ip_address = false )
{
   set_session_variable_for_matching_blockchains( name, value, "", "", matching_own_ip_address );
}

bool has_any_session_variable( const std::string& name );
bool has_any_session_variable( const std::string& name, const std::string& value );

size_t num_have_session_variable(
 const std::string& name, bool matching_own_ip_address = false,
 bool include_condemned = true, size_t check_session_id_less_than = 0 );

size_t num_have_session_variable( const std::string& name,
 const std::string& value, std::vector< std::string >* p_identities = 0,
 bool matching_own_ip_address = false, bool include_condemned = true );

bool is_first_using_session_variable( const std::string& name );
bool is_first_using_session_variable( const std::string& name, const std::string& value );

struct temporary_session_variable
{
   temporary_session_variable( const std::string& name )
    :
    name( name )
   {
      original_value = get_raw_session_variable( name );
   }

   temporary_session_variable( const std::string& name, const std::string& value )
    :
    name( name )
   {
      original_value = get_raw_session_variable( name );
      set_session_variable( name, value );
   }

   ~temporary_session_variable( )
   {
      set_session_variable( name, original_value );
   }

   std::string name;
   std::string original_value;
};

void copy_session_variables( std::map< std::string, std::string >& variables );

void restore_session_variables( const std::map< std::string, std::string >& variables );

struct restorable_session_variables
{
   std::map< std::string, std::string > variables;

   restorable_session_variables( )
   {
      copy_session_variables( variables );
   }

   ~restorable_session_variables( )
   {
      restore_session_variables( variables );
   }
};

struct temporary_identity_suffix
{
   temporary_identity_suffix( const std::string& temporary_suffix );

   ~temporary_identity_suffix( );

   std::string current_suffix;
   std::string original_suffix;
};

void add_udp_recv_file_chunk_info( size_t slot, size_t chunk, const std::string& info_and_data );
void add_udp_send_file_chunk_info( size_t slot, size_t chunk, const std::string& info_and_data );

void clear_udp_recv_file_chunks( );

size_t elapsed_since_last_recv( const date_time& dtm, const date_time* p_dtm = 0 );

bool has_udp_recv_file_chunk_info( size_t* p_num_chunks = 0 );

std::string get_udp_recv_file_chunk_info( size_t& chunk,
 bool chunk_specified = false, size_t* p_first_chunk = 0, size_t* p_num_chunks = 0 );

void init_storage( const std::string& name,
 const std::string& directory, command_handler& cmd_handler, bool lock_for_admin );

void create_storage( const std::string& name,
 const std::string& directory, command_handler& cmd_handler, bool lock_for_admin );

void attach_storage( const std::string& name, command_handler& cmd_handler, bool lock_for_admin );

void backup_storage(
 command_handler& cmd_handler, int* p_truncation_count = 0, std::string* p_sav_db_file_names = 0 );

bool export_storage( command_handler& cmd_handler );

void restore_storage( command_handler& cmd_handler );

void upgrade_storage( command_handler& cmd_handler );

void term_storage( command_handler& cmd_handler );

struct storage_scoped_lock_holder
{
   storage_scoped_lock_holder( size_t lock_handle );

   ~storage_scoped_lock_holder( );

   void release( );

   private:
   struct impl;
   impl* p_impl;
};

void storage_admin_name_lock( const std::string& name );

void storage_comment( const std::string& comment );

void storage_cache_clear( );

size_t storage_cache_limit( );
size_t storage_cache_limit( size_t new_limit );

void slice_storage_log( command_handler& cmd_handler,
 const std::string& name, const std::vector< std::string >& module_list );

void splice_storage_log( command_handler& cmd_handler,
 const std::string& name, const std::vector< std::string >& module_list );

bool storage_is_dead_key(
 const std::string& cid, const std::string& key, std::string* p_key_found = 0 );

void storage_add_dead_key( const std::string& cid, const std::string& key );

void check_storage( bool* p_rc = 0 );

std::string storage_name( bool* p_is_standard = 0 );

std::string storage_identity( );

void storage_identity( const std::string& new_identity );

std::string storage_blockchain( );

std::string storage_module_directory( );

bool storage_supports_sql_undo( );

bool has_storage_variable( const std::string& name_or_expr );

std::string get_storage_variable( const std::string& name_or_expr );
std::string get_raw_storage_variable( const std::string& var_name );

void set_storage_variable( const std::string& var_name, const std::string& new_value );

std::string storage_web_root( bool expand, bool check_is_linked = false );

void storage_web_root( const std::string& new_root );

void storage_bulk_start( bool is_write = false );

void storage_bulk_finish( );

void storage_lock_all_tables( );
void storage_unlock_all_tables( );

bool storage_locked_for_admin( );

ods& storage_ods_instance( );

class storage_ods_bulk_read
{
   public:
   storage_ods_bulk_read( );

   ~storage_ods_bulk_read( );

   private:
   struct impl;
   impl* p_impl;
};

class storage_ods_bulk_write
{
   public:
   storage_ods_bulk_write( progress* p_progress = 0 );

   ~storage_ods_bulk_write( );

   private:
   struct impl;
   impl* p_impl;
};

ods& system_ods_instance( );

ods_file_system& system_ods_file_system( );

void export_repository_entries( );

class system_ods_bulk_read
{
   public:
   system_ods_bulk_read( );

   ~system_ods_bulk_read( );

   private:
   struct impl;
   impl* p_impl;
};

class system_ods_bulk_write
{
   public:
   system_ods_bulk_write( progress* p_progress = 0 );

   ~system_ods_bulk_write( );

   private:
   struct impl;
   impl* p_impl;
};

std::string gen_key( const char* p_suffix = 0, bool use_get_dtm = false );

std::string get_uid( bool remove_display_name = true );

void set_uid( const std::string& uid, bool do_not_erase_sec = false );

struct temporary_uid
{
   temporary_uid( const std::string& new_uid, bool do_not_erase_sec = false )
   {
      old_uid = get_uid( false );

      set_uid( new_uid, do_not_erase_sec );

      this->do_not_erase_sec = do_not_erase_sec;
   }

   ~temporary_uid( )
   {
      set_uid( old_uid, do_not_erase_sec );
   }

   std::string old_uid;

   bool do_not_erase_sec;
};

bool is_sys_uid( );
bool is_anon_uid( );
bool is_auto_uid( );
bool is_init_uid( );
bool is_peer_uid( );
bool is_admin_uid( );

bool is_system_uid( );

bool is_admin_uid_key( const std::string& key );

bool is_uid_not_self_and_not_in_set( const std::string& key, const std::string& key_set );

bool has_sec_level( const std::string& level );

bool has_tx_key_info( const std::string& key_info );

std::string get_grp( );

void set_grp( const std::string& grp );

struct temporary_grp_and_uid
{
   temporary_grp_and_uid( const std::string& new_grp, const std::string& new_uid, bool do_not_erase_sec = false )
   {
      old_grp = get_grp( );
      old_uid = get_uid( do_not_erase_sec );

      this->do_not_erase_sec = do_not_erase_sec;

      set_grp( new_grp );
      set_uid( new_uid, true );
   }

   ~temporary_grp_and_uid( )
   {
      set_grp( old_grp );
      set_uid( old_uid, do_not_erase_sec );
   }

   std::string old_grp;
   std::string old_uid;

   bool do_not_erase_sec;
};

std::string get_dtm( );

void set_dtm( const std::string& dtm );

std::string get_gid( );

bool has_sql_db( );

sql_db& get_sql_db( );

void set_class( const std::string& mclass );
void set_module( const std::string& module );

std::string get_tz_name( );

void set_tz_name( const std::string& tz_name );

void clear_perms( );

const std::set< std::string >& get_perms( );

void set_perms( const std::set< std::string >& perms );

std::string get_tmp_directory( );

void set_tmp_directory( const std::string& tmp_directory );

bool has_session_secret( );

std::string get_session_secret( );

void set_session_secret( const std::string& secret );

void session_shared_decrypt(
 std::string& data, const std::string& pubkey, const std::string& message );

void session_shared_encrypt(
 std::string& data, const std::string& pubkey, const std::string& message );

void add_security_group( const std::string& group_key );

int64_t group_security_value( const std::string& group_key_value );

std::string convert_group_keys_to_numbers( const std::string& group_keys );

bool get_uid_data( const std::string& uid, size_t& level, std::string& gids );

void set_uid_data( const std::string& uid, const std::string& level, const std::string& group_keys );

size_t get_next_handle( );

void module_list( std::ostream& os );

size_t module_count( );

std::string module_identity( const std::string& module );

void module_class_list( const std::string& module, std::ostream& os, const char* p_pat = 0 );

void module_strings_list( const std::string& module, std::ostream& os );

void module_class_fields_list(
 const std::string& module, const std::string& class_id_or_name, std::ostream& os );

void module_class_procedures_list(
 const std::string& module, const std::string& class_id_or_name, std::ostream& os );

void module_load( const std::string& module_name,
 command_handler& cmd_handler, bool log_tx_comment = false, bool append_to_module_list = true );

void module_unload(
 const std::string& module_name, command_handler& cmd_handler, bool check_controlled = false );

void module_unload_all( command_handler& cmd_handler );

std::string loaded_module_id( const std::string& module );
std::string loaded_module_name( const std::string& module );

void register_module_commands( const std::string& module_name, command_handler& handler );
void unregister_module_commands( const std::string& module_name, command_handler& handler );
void unregister_all_module_commands( command_handler& handler );

void list_object_instances( std::ostream& os );

size_t create_object_instance(
 const std::string& module, const std::string& class_id, size_t handle = 0, bool dynamic_enabled = true );

std::string execute_object_command(
 size_t handle, const std::string& context, const std::string& method_name_and_args );

void init_object_instance( size_t handle, const std::string& context, bool for_create = false );

void prepare_object_instance( size_t handle,
 const std::string& context, bool for_create = false, bool call_to_store = true, bool starting_iteration = false );

void validate_object_instance( size_t handle, const std::string& context );

void destroy_object_instance( size_t handle );
void destroy_object_instances( const std::string& module_name );

void destroy_all_object_instances( );

std::string get_class_id( size_t handle, const std::string& context );
std::string get_class_name( size_t handle, const std::string& context );
std::string get_class_display_name( size_t handle, const std::string& context, bool plural = false );

std::string get_create_instance_info( size_t handle, const std::string& context );
std::string get_update_instance_info( size_t handle, const std::string& context );
std::string get_destroy_instance_info( size_t handle, const std::string& context );

std::string get_execute_procedure_info(
 size_t handle, const std::string& context, const std::string& procedure_id );

std::string get_instance_owner( size_t handle, const std::string& context );

void get_all_enum_pairs( size_t handle,
 const std::string& context, std::vector< std::pair< std::string, std::string > >& pairs );

void get_all_field_names( size_t handle,
 const std::string& context, std::vector< std::string >& fields, bool include_transients = true );

void get_all_field_scope_and_permission_info( size_t handle,
 const std::string& context, std::map< std::string, std::string >& field_scope_and_perm_info, bool by_name = false );

std::string get_field_name_for_id(
 size_t handle, const std::string& context, const std::string& id, bool no_throw = false );

std::string get_field_id_for_name(
 size_t handle, const std::string& context, const std::string& name, bool no_throw = false );

std::string get_field_type_name(
 size_t handle, const std::string& context, const std::string& id_or_name,
 bool* p_is_encrypted = 0, bool* p_is_transient = 0, bool* p_is_file_field = 0 );

std::string get_field_uom_symbol(
 size_t handle, const std::string& context, const std::string& id_or_name );

std::string get_field_display_name(
 size_t handle, const std::string& context, const std::string& id_or_name );

std::string get_field_value( size_t handle, const std::string& context, const std::string& field );

std::string get_field_values( size_t handle,
 const std::string& parent_context, const std::vector< std::string >& field_list,
 const std::string& tz_name, bool is_default = false, bool as_csv = false,
 std::vector< std::string >* p_raw_values = 0, const std::multimap< size_t, std::string >* p_inserts = 0,
 const std::map< std::string, std::string >* p_replace_map = 0, const std::vector< std::string >* p_omit_matching = 0 );

void set_any_field_ids_to_names( size_t handle,
 const std::string& context, std::vector< std::string >& fields );

void set_any_field_names_to_ids( size_t handle,
 const std::string& context, std::vector< std::string >& fields );

bool determine_alternative_key_fields( size_t handle, const std::string& context,
 const std::vector< std::string > available_fields, std::vector< std::string >& selected_fields );

void get_foreign_field_and_class_ids( size_t handle,
 const std::string& context, std::map< std::string, std::string >& foreign_field_and_class_ids );

void get_base_class_info( size_t handle,
 const std::string& context, std::vector< std::pair< std::string, std::string > >& base_class_info );

bool has_locked_instance( const std::string& lock_class_id, const std::string& key );

bool is_child_constrained( class_base& instance, std::string& constraining_class );

size_t obtain_storage_lock(
 const std::string& type, const std::string& lock_class_id, const std::string& key, size_t num_attempts = 0 );

bool obtain_cascade_locks( class_base& instance );

size_t obtain_instance_lock(
 const std::string& type, class_base& instance, size_t num_attempts = 0, std::string* p_key_value = 0 );

size_t obtain_instance_fk_lock( const std::string& lock_class_id, const std::string& key, bool review_required );

void release_obtained_lock( size_t lock_handle );

void release_instance_locks( class_base& instance, bool force_removal = false );

void transform_obtained_lock( size_t lock_handle, const std::string& type, size_t num_attempts = 0 );

void dump_storage_cache( std::ostream& os );
void dump_storage_locks( std::ostream& os );

std::string resolve_class_id( const std::string& module,
 const std::string& id_or_name, const std::string& exception_context );

std::string resolve_field_id( const std::string& module,
 const std::string& mclass, const std::string& id_or_name, const std::string& exception_context );

struct field_data
{
   field_data( const std::string& id,
    const std::string& name, const std::string& value,
    bool mandatory, bool transient, const std::string& class_id, const std::string& type_name )
    :
    id( id ),
    name( name ),
    value( value ),
    mandatory( mandatory ),
    transient( transient ),
    class_id( class_id ),
    type_name( type_name )
   {
   }

   std::string id;
   std::string name;
   std::string value;

   bool mandatory;
   bool transient;

   std::string class_id;
   std::string type_name;
};

void get_all_field_data( size_t handle, const std::string& context,
 const std::string& key, std::vector< field_data >& all_field_data, std::string* p_class_id = 0,
 std::string* p_class_name = 0, std::vector< std::pair< std::string, std::string > >* p_base_class_info = 0 );

class_base& get_class_base_from_handle( size_t handle, const std::string& context );

enum permit_op_type_value
{
   e_permit_op_type_value_any,
   e_permit_op_type_value_none,
   e_permit_op_type_value_review,
   e_permit_op_type_value_create_update_destroy
};

class_base& get_class_base_from_handle_for_op( size_t handle,
 const std::string& context, permit_op_type_value permit = e_permit_op_type_value_none, bool use_dynamic_context = true );

bool fetch_instance_from_db( class_base& instance,
 const std::string& sql, bool sys_only_fields = false, bool is_minimal_fetch = false, bool allow_caching = false );

bool fetch_instance_from_cache( class_base& instance, const std::string& key, bool sys_only_fields = false );

void get_instance_sql_stmts( class_base& instance,
 std::vector< std::string >& sql_stmts, std::vector< std::string >* p_sql_undo_stmts = 0 );

void append_undo_sql_stmts( const std::vector< std::string >& sql_undo_stmts );

std::string instance_class( size_t handle, const std::string& context );

std::string instance_key_info( size_t handle, const std::string& context, bool key_only = false );

std::string instance_get_variable( size_t handle, const std::string& context, const std::string& vname );

void instance_set_variable( size_t handle, const std::string& context, const std::string& vname, const std::string& value );

void instance_special_field_names( size_t handle, const std::string& context, const std::string& empty,
 std::string* p_group_name = 0, std::string* p_level_name = 0, std::string* p_order_name = 0, std::string* p_owner_name = 0 );

std::string instance_group_field_name( size_t handle, const std::string& context );
std::string instance_level_field_name( size_t handle, const std::string& context );
std::string instance_order_field_name( size_t handle, const std::string& context );
std::string instance_owner_field_name( size_t handle, const std::string& context );

enum instance_check_rc
{
   e_instance_check_rc_okay,
   e_instance_check_rc_not_found
};

void instance_check( class_base& instance, instance_check_rc* p_rc = 0 );

void instance_tx_check( class_base& instance );

void update_session_locks_for_transaction( );

bool is_change_locked( class_base& instance, bool include_cascades = false );
bool is_destroy_locked( class_base& instance, bool include_cascades = false );

bool is_create_locked_by_own_session( class_base& instance,
 const char* p_key = 0, bool copy_field_values = false, bool also_check_tx_lock_type = false );

bool was_create_locked_by_own_session(
 class_base& instance, const char* p_key = 0, bool copy_field_values = false );

bool is_update_locked_by_own_session( class_base& instance, const char* p_key = 0 );
bool is_destroy_locked_by_own_session( class_base& instance, const char* p_key = 0 );
bool is_update_or_destroy_locked_by_own_session( class_base& instance, const char* p_key = 0 );

void instance_fetch( size_t handle,
 const std::string& context, const std::string& key_info, instance_fetch_rc* p_rc = 0 );

void instance_set_parent( size_t handle,
 const std::string& context, const std::string& new_key );

void instance_prepare_execute( size_t handle,
 const std::string& context, const std::string& key, const std::string& ver_info, bool skip_after_fetch = false );

std::string instance_execute( size_t handle,
 const std::string& context, const std::string& key, const std::string& method_name_and_args );

void instance_get_field_values( size_t handle,
 const std::string& context, std::vector< std::string >& field_values );

std::string instance_get_fields_and_values( size_t handle,
 const std::string& context, const std::vector< std::string >* p_initial_field_values = 0 );

bool instance_has_changed( size_t handle, const std::string& context );

bool instance_persistence_uses_log( size_t handle );

bool instance_iterate( size_t handle,
 const std::string& context, const std::string& key_info, const std::string& fields,
 const std::string& text, const std::string& query, iter_direction direction, bool inclusive,
 int row_limit = 0, sql_optimisation optimisation = e_sql_optimisation_none, const std::set< std::string >* p_filters = 0 );

bool instance_iterate_next( size_t handle, const std::string& context );
void instance_iterate_stop( size_t handle, const std::string& context );

bool instance_filtered( size_t handle, const std::string& context );
bool instance_uid_filtered( size_t handle, const std::string& context );
bool instance_has_transient_filter_fields( size_t handle, const std::string& context );

void instance_review_begin( size_t handle, const std::string& context, const std::string& key );
void instance_review_finish( size_t handle, const std::string& context );

void op_instance_create( size_t handle,
 const std::string& context, const std::string& key, bool internal_operation = true, op_create_rc* p_rc = 0 );

void op_instance_update( size_t handle, const std::string& context,
 const std::string& key, const std::string& ver_info, bool internal_operation = true, op_update_rc* p_rc = 0 );

void op_instance_destroy( size_t handle, const std::string& context,
 const std::string& key, const std::string& ver_info, bool internal_operation = true, op_destroy_rc* p_rc = 0 );

void op_instance_apply( size_t handle, const std::string& context,
 bool internal_operation = true, op_apply_rc* p_rc = 0, std::set< std::string >* p_fields_set = 0 );

void op_instance_cancel( size_t handle, const std::string& context, bool internal_operation = true );

void output_progress_message( const std::string& message );

bool is_in_transaction( );

void transaction_start( );
void transaction_commit( );
void transaction_rollback( );

size_t transaction_id( );
size_t transaction_level( );

size_t set_transaction_id( size_t tx_id );

size_t next_transaction_id( );

struct transaction_commit_helper
{
   virtual void at_commit( ) { }
   virtual void after_commit( ) { }
   virtual void after_rollback( ) { }
};

std::string transaction_log_command( );

void transaction_log_command(
 const std::string& log_command, transaction_commit_helper* p_tx_helper = 0, bool replace_current = false );

void append_peerchain_log_command( const std::string& identity, const std::string& log_command );

void append_transaction_log_command( const std::string& log_command );

class transaction
{
   public:
   transaction( bool is_not_dummy = true );

   ~transaction( );

   void commit( );
   void rollback( );

   private:
   bool is_dummy;
   bool can_commit;

   transaction( const transaction& );
   transaction& operator =( const transaction& );
};

struct record_initialiser
{
   record_initialiser( std::ofstream& log_file );

   ~record_initialiser( );
};

#endif
