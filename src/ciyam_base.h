// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2019 CIYAM Developers
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

#  include "macros.h"
#  include "ptypes.h"
#  include "threads.h"
#  include "ciyam_common.h"

#  ifdef CIYAM_BASE_IMPL
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_IMPORT
#  endif

#  define GS( s ) get_string( STRINGIZE( s ) )

class ods;
class class_base;
class command_handler;
class ods_file_system;

#  define TRACE_COMMANDS   0x00000001
#  define TRACE_SQLSTMTS   0x00000002
#  define TRACE_CLASSOPS   0x00000004
#  define TRACE_MODS_GEN   0x00000008
#  define TRACE_SQLCLSET   0x00000010
#  define TRACE_FLD_VALS   0x00000020
#  define TRACE_LOCK_OPS   0x00000040
#  define TRACE_CTR_DTRS   0x00000080
#  define TRACE_SESSIONS   0x00000100
#  define TRACE_MAIL_OPS   0x00000200
#  define TRACE_PDF_VALS   0x00000400
#  define TRACE_SOCK_OPS   0x00000800
#  define TRACE_CORE_FLS   0x00001000
#  define TRACE_SYNC_OPS   0x00002000
#  define TRACE_ANYTHING   0xffffffff

#  define IF_IS_TRACING( flags )\
if( get_trace_flags( ) & ( flags ) )

#  define TRACE_LOG( flag, message )\
if( ( flag == TRACE_ANYTHING ) || ( get_trace_flags( ) & flag ) )\
   log_trace_message( flag, message )

#  define TEMP_TRACE( message ) TRACE_LOG( TRACE_ANYTHING, message )

size_t CIYAM_BASE_DECL_SPEC get_trace_flags( );
void CIYAM_BASE_DECL_SPEC set_trace_flags( size_t flags );

extern "C" void CIYAM_BASE_DECL_SPEC trace_flags( unsigned flags );

typedef void ( *fp_trace_flags )( unsigned );

void CIYAM_BASE_DECL_SPEC list_trace_flags( std::vector< std::string >& flag_names );

void CIYAM_BASE_DECL_SPEC log_trace_message( int flag, const std::string& message );

extern "C" void CIYAM_BASE_DECL_SPEC log_trace_string( int flag, const char* p_message );

typedef void ( *fp_log_trace_string )( int, const char* );

class CIYAM_BASE_DECL_SPEC trace_mutex : public mutex
{
   protected:
   void pre_acquire( const guard* p_guard, const char* p_msg );
   void post_acquire( const guard* p_guard, const char* p_msg );

   void has_released( const guard* p_guard, const char* p_msg );
};

extern "C" void CIYAM_BASE_DECL_SPEC register_listener( int port, const char* p_info );
extern "C" void CIYAM_BASE_DECL_SPEC unregister_listener( int port );

typedef void ( *fp_register_listener )( int, const char* );
typedef void ( *fp_unregister_listener )( int );

class CIYAM_BASE_DECL_SPEC listener_registration
{
   public:
   listener_registration( int port, const std::string& info );
   ~listener_registration( );

   private:
   int port;
};

bool CIYAM_BASE_DECL_SPEC has_registered_listener( int port );

void CIYAM_BASE_DECL_SPEC list_listeners( std::ostream& os );

extern "C" void CIYAM_BASE_DECL_SPEC init_globals( );
extern "C" void CIYAM_BASE_DECL_SPEC term_globals( );

typedef void ( *fp_init_globals )( );
typedef void ( *fp_term_globals )( );

extern "C" void CIYAM_BASE_DECL_SPEC check_timezone_info( );

typedef void ( *fp_check_timezone_info )( );

std::string CIYAM_BASE_DECL_SPEC get_string( const std::string& key );

std::string CIYAM_BASE_DECL_SPEC get_string_message(
 const std::string& string_message, const std::pair< std::string, std::string >& parm1 );

std::string CIYAM_BASE_DECL_SPEC get_string_message( const std::string& string_message,
 const std::pair< std::string, std::string >& parm1, const std::pair< std::string, std::string >& parm2 );

std::string CIYAM_BASE_DECL_SPEC get_string_message( const std::string& string_message,
 const std::pair< std::string, std::string >& parm1, const std::pair< std::string, std::string >& parm2,
 const std::pair< std::string, std::string >& parm3 );

std::string CIYAM_BASE_DECL_SPEC get_string_message( const std::string& string_message,
 const std::pair< std::string, std::string >& parm1, const std::pair< std::string, std::string >& parm2,
 const std::pair< std::string, std::string >& parm3, const std::pair< std::string, std::string >& parm4 );

unsigned int CIYAM_BASE_DECL_SPEC get_max_peers( );

unsigned int CIYAM_BASE_DECL_SPEC get_max_user_limit( );
void CIYAM_BASE_DECL_SPEC set_max_user_limit( unsigned int new_limit );

std::string CIYAM_BASE_DECL_SPEC get_prefix( );
std::string CIYAM_BASE_DECL_SPEC get_domain( );

std::string CIYAM_BASE_DECL_SPEC get_app_url( const std::string& suffix );

std::string CIYAM_BASE_DECL_SPEC get_identity( bool prepend_sid = false, bool append_max_user_limit = false );
void CIYAM_BASE_DECL_SPEC set_identity( const std::string& identity_info );

std::string CIYAM_BASE_DECL_SPEC get_checksum( const std::string& data, bool use_reg_key = false );

std::string CIYAM_BASE_DECL_SPEC get_timezone( );
std::string CIYAM_BASE_DECL_SPEC get_web_root( );

void CIYAM_BASE_DECL_SPEC get_initial_peer_ips( std::map< std::string, std::string >& ips );

bool CIYAM_BASE_DECL_SPEC get_is_accepted_ip_addr( const std::string& ip_addr );
bool CIYAM_BASE_DECL_SPEC get_is_accepted_peer_ip_addr( const std::string& ip_addr );

extern "C" int CIYAM_BASE_DECL_SPEC is_accepted_ip_addr( const char* p_addr );

typedef int ( *fp_is_accepted_ip_addr )( const char* );

void CIYAM_BASE_DECL_SPEC add_peer_ip_addr_for_rejection( const std::string& ip_addr );
void CIYAM_BASE_DECL_SPEC remove_peer_ip_addr_from_rejection( const std::string& ip_addr );

std::string CIYAM_BASE_DECL_SPEC list_peer_ip_addrs_for_rejection( );

void CIYAM_BASE_DECL_SPEC get_blockchains( std::map< int, std::string >& blockchains );

int CIYAM_BASE_DECL_SPEC get_blockchain_port( const std::string& blockchain );
bool CIYAM_BASE_DECL_SPEC get_is_known_blockchain( const std::string& blockchain );

std::string CIYAM_BASE_DECL_SPEC get_blockchain_for_port( int port );

void CIYAM_BASE_DECL_SPEC register_blockchain( int port, const std::string& blockchain );

bool CIYAM_BASE_DECL_SPEC get_using_ssl( );

size_t CIYAM_BASE_DECL_SPEC get_files_area_item_max_num( );
size_t CIYAM_BASE_DECL_SPEC get_files_area_item_max_size( );

std::string CIYAM_BASE_DECL_SPEC get_mbox_path( );
std::string CIYAM_BASE_DECL_SPEC get_mbox_username( );

std::string CIYAM_BASE_DECL_SPEC get_pop3_server( );
std::string CIYAM_BASE_DECL_SPEC get_pop3_suffix( );
std::string CIYAM_BASE_DECL_SPEC get_pop3_username( );
std::string CIYAM_BASE_DECL_SPEC get_pop3_password( );
std::string CIYAM_BASE_DECL_SPEC get_pop3_security( );

std::string CIYAM_BASE_DECL_SPEC get_smtp_server( );
std::string CIYAM_BASE_DECL_SPEC get_smtp_sender( );
std::string CIYAM_BASE_DECL_SPEC get_smtp_suffix( );
std::string CIYAM_BASE_DECL_SPEC get_smtp_username( );
std::string CIYAM_BASE_DECL_SPEC get_smtp_password( );
std::string CIYAM_BASE_DECL_SPEC get_smtp_security( );

int CIYAM_BASE_DECL_SPEC get_smtp_max_send_attempts( );
int64_t CIYAM_BASE_DECL_SPEC get_smtp_max_attached_data( );

struct external_client
{
   external_client( ) : port( 0 ), is_local( false ) { }

   int port;
   bool is_local;

   std::string protocol;
   std::string extra_info;
   std::string script_name;
};

std::string CIYAM_BASE_DECL_SPEC list_externals( );

bool CIYAM_BASE_DECL_SPEC is_local_external_client( const std::string& key );

void CIYAM_BASE_DECL_SPEC get_external_client_info( const std::string& key, external_client& info );

void CIYAM_BASE_DECL_SPEC verify_active_external_service( const std::string& ext_key );

std::string CIYAM_BASE_DECL_SPEC decrypt_data(
 const std::string& password, bool no_ssl = false, bool no_salt = false, bool hash_only = false );

std::string CIYAM_BASE_DECL_SPEC encrypt_data(
 const std::string& password, bool no_ssl = false, bool no_salt = false, bool hash_only = false );

std::string CIYAM_BASE_DECL_SPEC totp_secret_key( const std::string& unique );

int CIYAM_BASE_DECL_SPEC exec_system( const std::string& cmd, bool async = false, bool delay = false );

int CIYAM_BASE_DECL_SPEC run_script(
 const std::string& script_name, bool async = true, bool delay = false, bool no_logging = false );

void CIYAM_BASE_DECL_SPEC list_scripts( const std::string& pattern, std::ostream& os );

std::string CIYAM_BASE_DECL_SPEC process_script_args( const std::string& raw_args );

void CIYAM_BASE_DECL_SPEC generate_new_script_sio_files( );

void CIYAM_BASE_DECL_SPEC init_session(
 command_handler& cmd_handler, bool is_peer_session = false,
 const std::string* p_ip_addr = 0, const std::string* p_blockchain = 0, int port = 0 );

void CIYAM_BASE_DECL_SPEC term_session( );

size_t CIYAM_BASE_DECL_SPEC session_id( );

bool CIYAM_BASE_DECL_SPEC has_session_with_ip_addr( const std::string& ip_addr );

std::string CIYAM_BASE_DECL_SPEC get_random_same_port_peer_ip_addr( const std::string& empty_value );

void CIYAM_BASE_DECL_SPEC list_sessions( std::ostream& os, bool inc_dtms = true );

command_handler CIYAM_BASE_DECL_SPEC& get_session_command_handler( );

class CIYAM_BASE_DECL_SPEC session_file_buffer_access
{
   public:
   session_file_buffer_access( );
   ~session_file_buffer_access( );

   unsigned int get_size( ) { return size; }
   unsigned char* get_buffer( ) { return p_buffer; }

   private:
   unsigned int size;
   unsigned char* p_buffer;
};

void CIYAM_BASE_DECL_SPEC increment_peer_files_uploaded( int64_t bytes );
void CIYAM_BASE_DECL_SPEC increment_peer_files_downloaded( int64_t bytes );

void CIYAM_BASE_DECL_SPEC increment_session_commands_executed( );

void CIYAM_BASE_DECL_SPEC set_slowest_if_applicable( );
void CIYAM_BASE_DECL_SPEC set_last_session_cmd_and_hash( const std::string& cmd, const std::string& parameter_info );

void CIYAM_BASE_DECL_SPEC condemn_session(
 size_t sess_id, int num_seconds, bool force_uncapture, bool wait_until_term );

void CIYAM_BASE_DECL_SPEC condemn_this_session( );

void CIYAM_BASE_DECL_SPEC condemn_all_other_sessions( int num_seconds, bool force_uncapture, bool wait_until_term );

bool CIYAM_BASE_DECL_SPEC is_condemned_session( );

void CIYAM_BASE_DECL_SPEC capture_session( size_t sess_id );
void CIYAM_BASE_DECL_SPEC capture_all_other_sessions( );

bool CIYAM_BASE_DECL_SPEC is_captured_session( );

void CIYAM_BASE_DECL_SPEC release_session( size_t sess_id, bool wait_until_term );
void CIYAM_BASE_DECL_SPEC release_all_other_sessions( bool wait_until_term );

bool CIYAM_BASE_DECL_SPEC session_skip_fk_fetches( );
void CIYAM_BASE_DECL_SPEC session_skip_fk_fetches( bool skip_fk_fetches );

bool CIYAM_BASE_DECL_SPEC session_skip_validation( );
void CIYAM_BASE_DECL_SPEC session_skip_validation( bool skip_validation );

bool CIYAM_BASE_DECL_SPEC session_skip_is_constained( );
void CIYAM_BASE_DECL_SPEC session_skip_is_constained( bool skip_fk_fetches );

bool CIYAM_BASE_DECL_SPEC get_script_reconfig( );

std::string CIYAM_BASE_DECL_SPEC get_pem_password( );
std::string CIYAM_BASE_DECL_SPEC get_rpc_password( );
std::string CIYAM_BASE_DECL_SPEC get_sql_password( );

std::string CIYAM_BASE_DECL_SPEC get_encrypted_gpg_password( );
std::string CIYAM_BASE_DECL_SPEC get_encrypted_pem_password( );
std::string CIYAM_BASE_DECL_SPEC get_encrypted_sql_password( );
std::string CIYAM_BASE_DECL_SPEC get_encrypted_rpc_password( );
std::string CIYAM_BASE_DECL_SPEC get_encrypted_pop3_password( );
std::string CIYAM_BASE_DECL_SPEC get_encrypted_smtp_password( );

std::string CIYAM_BASE_DECL_SPEC get_default_storage( );
void CIYAM_BASE_DECL_SPEC set_default_storage( const std::string& name );

std::string CIYAM_BASE_DECL_SPEC get_session_blockchain( );
bool CIYAM_BASE_DECL_SPEC get_session_is_using_blockchain( );

unsigned int CIYAM_BASE_DECL_SPEC get_num_sessions_for_blockchain( const std::string& blockchain );

unsigned int CIYAM_BASE_DECL_SPEC get_session_timeout( );
void CIYAM_BASE_DECL_SPEC set_session_timeout( unsigned int seconds );

void CIYAM_BASE_DECL_SPEC add_peer_file_hash_for_get( const std::string& hash );

void CIYAM_BASE_DECL_SPEC store_repository_entry_record( const std::string& key,
 const std::string& local_hash, const std::string& local_public_key, const std::string& master_public_key );

bool CIYAM_BASE_DECL_SPEC fetch_repository_entry_record( const std::string& key,
 std::string& local_hash, std::string& local_public_key, std::string& master_public_key, bool must_exist = true );

std::string CIYAM_BASE_DECL_SPEC top_next_peer_file_hash_to_get( );
void CIYAM_BASE_DECL_SPEC pop_next_peer_file_hash_to_get( );

void CIYAM_BASE_DECL_SPEC add_peer_file_hash_for_put( const std::string& hash );

void CIYAM_BASE_DECL_SPEC add_peer_file_hash_for_put_for_all_peers(
 const std::string& hash, const std::string* p_blockchain = 0,
 const std::string* p_session_variable = 0, bool include_self = true, size_t session_id_to_skip = 0 );

std::string CIYAM_BASE_DECL_SPEC top_next_peer_file_hash_to_put( );
void CIYAM_BASE_DECL_SPEC pop_next_peer_file_hash_to_put( );

bool CIYAM_BASE_DECL_SPEC any_peer_still_has_file_hash_to_put(
 const std::string& hash, const std::string* p_blockchain = 0 );

void CIYAM_BASE_DECL_SPEC set_default_session_variables( int port = 0 );

std::string CIYAM_BASE_DECL_SPEC get_raw_session_variable( const std::string& name );
std::string CIYAM_BASE_DECL_SPEC get_session_variable( const std::string& name_or_expr );

void CIYAM_BASE_DECL_SPEC set_session_variable(
 const std::string& name, const std::string& value,
 bool* p_set_special_temporary = 0, command_handler* p_command_handler = 0 );

bool CIYAM_BASE_DECL_SPEC set_session_variable(
 const std::string& name, const std::string& value, const std::string& current );

bool CIYAM_BASE_DECL_SPEC has_any_session_variable( const std::string& name );
bool CIYAM_BASE_DECL_SPEC has_any_session_variable( const std::string& name, const std::string& value );

bool CIYAM_BASE_DECL_SPEC is_first_using_session_variable( const std::string& name );
bool CIYAM_BASE_DECL_SPEC is_first_using_session_variable( const std::string& name, const std::string& value );

struct temporary_session_variable
{
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

void CIYAM_BASE_DECL_SPEC list_mutex_lock_ids_for_ciyam_base( std::ostream& outs );

bool CIYAM_BASE_DECL_SPEC has_crypt_key_for_blockchain_account( const std::string& blockchain, const std::string& account );
std::string CIYAM_BASE_DECL_SPEC get_crypt_key_for_blockchain_account( const std::string& blockchain, const std::string& account );

void CIYAM_BASE_DECL_SPEC set_crypt_key_for_blockchain_account(
 const std::string& blockchain, const std::string& account, const std::string& crypt_key );

void CIYAM_BASE_DECL_SPEC init_storage( const std::string& name,
 const std::string& directory, command_handler& cmd_handler, bool lock_for_admin );
void CIYAM_BASE_DECL_SPEC create_storage( const std::string& name,
 const std::string& directory, command_handler& cmd_handler, bool lock_for_admin );
void CIYAM_BASE_DECL_SPEC attach_storage( const std::string& name, command_handler& cmd_handler, bool lock_for_admin );

void CIYAM_BASE_DECL_SPEC backup_storage(
 command_handler& cmd_handler, int* p_truncation_count = 0, std::string* p_sav_db_file_names = 0 );

void CIYAM_BASE_DECL_SPEC restore_storage( command_handler& cmd_handler );

void CIYAM_BASE_DECL_SPEC upgrade_storage( command_handler& cmd_handler );

void CIYAM_BASE_DECL_SPEC term_storage( command_handler& cmd_handler );

void CIYAM_BASE_DECL_SPEC storage_admin_name_lock( const std::string& name );

void CIYAM_BASE_DECL_SPEC storage_comment( const std::string& comment );

void CIYAM_BASE_DECL_SPEC storage_cache_clear( );
size_t CIYAM_BASE_DECL_SPEC storage_cache_limit( );
size_t CIYAM_BASE_DECL_SPEC storage_cache_limit( size_t new_limit );

void CIYAM_BASE_DECL_SPEC slice_storage_log( command_handler& cmd_handler,
 const std::string& name, const std::vector< std::string >& module_list );
void CIYAM_BASE_DECL_SPEC splice_storage_log( command_handler& cmd_handler,
 const std::string& name, const std::vector< std::string >& module_list );

bool CIYAM_BASE_DECL_SPEC storage_is_dead_key(
 const std::string& cid, const std::string& key, std::string* p_key_found = 0 );

void CIYAM_BASE_DECL_SPEC storage_add_dead_key( const std::string& cid, const std::string& key );

std::string CIYAM_BASE_DECL_SPEC storage_name( );
std::string CIYAM_BASE_DECL_SPEC storage_identity( );
std::string CIYAM_BASE_DECL_SPEC storage_blockchain( );
std::string CIYAM_BASE_DECL_SPEC storage_module_directory( );

std::string CIYAM_BASE_DECL_SPEC storage_web_root( bool expand, bool check_is_linked = false );
void CIYAM_BASE_DECL_SPEC storage_web_root( const std::string& new_root );

void CIYAM_BASE_DECL_SPEC storage_process_undo(
 uint64_t new_height, std::map< std::string, std::string >& file_info );

void CIYAM_BASE_DECL_SPEC storage_lock_all_tables( );
void CIYAM_BASE_DECL_SPEC storage_unlock_all_tables( );

bool CIYAM_BASE_DECL_SPEC storage_locked_for_admin( );

ods CIYAM_BASE_DECL_SPEC& storage_instance( );

ods CIYAM_BASE_DECL_SPEC& ciyam_ods_instance( );

ods_file_system CIYAM_BASE_DECL_SPEC& ciyam_ods_file_system( );

std::string CIYAM_BASE_DECL_SPEC gen_key( const char* p_suffix = 0, bool append_slot_num = true );

std::string CIYAM_BASE_DECL_SPEC get_uid( bool remove_display_name = true );
void CIYAM_BASE_DECL_SPEC set_uid( const std::string& uid );

bool CIYAM_BASE_DECL_SPEC is_sys_uid( );
bool CIYAM_BASE_DECL_SPEC is_anon_uid( );
bool CIYAM_BASE_DECL_SPEC is_auto_uid( );
bool CIYAM_BASE_DECL_SPEC is_init_uid( );
bool CIYAM_BASE_DECL_SPEC is_admin_uid( );

bool CIYAM_BASE_DECL_SPEC is_system_uid( );

bool CIYAM_BASE_DECL_SPEC is_admin_uid_key( const std::string& key );

bool CIYAM_BASE_DECL_SPEC is_uid_not_self_and_not_in_set( const std::string& key, const std::string& key_set );

bool CIYAM_BASE_DECL_SPEC has_sec_level( const std::string& level );

std::string CIYAM_BASE_DECL_SPEC get_grp( );
void CIYAM_BASE_DECL_SPEC set_grp( const std::string& grp );

std::string CIYAM_BASE_DECL_SPEC get_dtm( );
void CIYAM_BASE_DECL_SPEC set_dtm( const std::string& dtm );

void CIYAM_BASE_DECL_SPEC set_class( const std::string& mclass );
void CIYAM_BASE_DECL_SPEC set_module( const std::string& module );

std::string CIYAM_BASE_DECL_SPEC get_tz_name( );
void CIYAM_BASE_DECL_SPEC set_tz_name( const std::string& tz_name );

void CIYAM_BASE_DECL_SPEC clear_perms( );
const std::set< std::string > CIYAM_BASE_DECL_SPEC& get_perms( );
void CIYAM_BASE_DECL_SPEC set_perms( const std::set< std::string >& perms );

std::string CIYAM_BASE_DECL_SPEC get_tmp_directory( );
void CIYAM_BASE_DECL_SPEC set_tmp_directory( const std::string& tmp_directory );

std::string CIYAM_BASE_DECL_SPEC get_session_secret( );
void CIYAM_BASE_DECL_SPEC set_session_secret( const std::string& secret );

void CIYAM_BASE_DECL_SPEC set_session_mint_account( const std::string& account );
bool CIYAM_BASE_DECL_SPEC uid_matches_session_mint_account( );

std::string CIYAM_BASE_DECL_SPEC session_shared_decrypt( const std::string& pubkey, const std::string& message );

size_t CIYAM_BASE_DECL_SPEC get_next_handle( );

void CIYAM_BASE_DECL_SPEC module_list( std::ostream& os );
size_t CIYAM_BASE_DECL_SPEC module_count( );

void CIYAM_BASE_DECL_SPEC module_class_list( const std::string& module, std::ostream& os );
void CIYAM_BASE_DECL_SPEC module_strings_list( const std::string& module, std::ostream& os );

void CIYAM_BASE_DECL_SPEC module_class_fields_list(
 const std::string& module, const std::string& class_id_or_name, std::ostream& os );

void CIYAM_BASE_DECL_SPEC module_class_procedures_list(
 const std::string& module, const std::string& class_id_or_name, std::ostream& os );

void CIYAM_BASE_DECL_SPEC module_load( const std::string& module_name,
 command_handler& cmd_handler, bool log_tx_comment = false, bool append_to_module_list = true );

void CIYAM_BASE_DECL_SPEC module_unload(
 const std::string& module_name, command_handler& cmd_handler, bool check_controlled = false );

void CIYAM_BASE_DECL_SPEC module_unload_all( command_handler& cmd_handler );

std::string CIYAM_BASE_DECL_SPEC loaded_module_id( const std::string& module );
std::string CIYAM_BASE_DECL_SPEC loaded_module_name( const std::string& module );

void CIYAM_BASE_DECL_SPEC register_module_commands( const std::string& module_name, command_handler& handler );
void CIYAM_BASE_DECL_SPEC unregister_module_commands( const std::string& module_name, command_handler& handler );
void CIYAM_BASE_DECL_SPEC unregister_all_module_commands( command_handler& handler );

void CIYAM_BASE_DECL_SPEC list_object_instances( std::ostream& os );

size_t CIYAM_BASE_DECL_SPEC create_object_instance(
 const std::string& module, const std::string& class_id, size_t handle = 0, bool dynamic_enabled = true );

std::string CIYAM_BASE_DECL_SPEC execute_object_command(
 size_t handle, const std::string& context, const std::string& method_name_and_args );

void CIYAM_BASE_DECL_SPEC init_object_instance( size_t handle, const std::string& context, bool for_create = false );

void CIYAM_BASE_DECL_SPEC prepare_object_instance( size_t handle,
 const std::string& context, bool for_create = false, bool call_to_store = true );

void CIYAM_BASE_DECL_SPEC validate_object_instance( size_t handle, const std::string& context );

void CIYAM_BASE_DECL_SPEC destroy_object_instance( size_t handle );
void CIYAM_BASE_DECL_SPEC destroy_object_instances( const std::string& module_name );

void CIYAM_BASE_DECL_SPEC destroy_all_object_instances( );

std::string CIYAM_BASE_DECL_SPEC get_class_id( size_t handle, const std::string& context );
std::string CIYAM_BASE_DECL_SPEC get_class_name( size_t handle, const std::string& context );
std::string CIYAM_BASE_DECL_SPEC get_class_display_name( size_t handle, const std::string& context, bool plural = false );

std::string CIYAM_BASE_DECL_SPEC get_create_instance_info( size_t handle, const std::string& context );
std::string CIYAM_BASE_DECL_SPEC get_update_instance_info( size_t handle, const std::string& context );
std::string CIYAM_BASE_DECL_SPEC get_destroy_instance_info( size_t handle, const std::string& context );

std::string CIYAM_BASE_DECL_SPEC get_execute_procedure_info(
 size_t handle, const std::string& context, const std::string& procedure_id );

std::string CIYAM_BASE_DECL_SPEC get_instance_owner( size_t handle, const std::string& context );

void CIYAM_BASE_DECL_SPEC get_all_enum_pairs( size_t handle,
 const std::string& context, std::vector< std::pair< std::string, std::string > >& pairs );

void CIYAM_BASE_DECL_SPEC get_all_field_names( size_t handle,
 const std::string& context, std::vector< std::string >& fields, bool include_transients = true );

void CIYAM_BASE_DECL_SPEC get_all_field_scope_and_permission_info( size_t handle,
 const std::string& context, std::map< std::string, std::string >& field_scope_and_perm_info, bool by_name = false );

std::string CIYAM_BASE_DECL_SPEC get_field_name_for_id(
 size_t handle, const std::string& context, const std::string& id, bool no_throw = false );

std::string CIYAM_BASE_DECL_SPEC get_field_id_for_name(
 size_t handle, const std::string& context, const std::string& name, bool no_throw = false );

std::string CIYAM_BASE_DECL_SPEC get_field_type_name(
 size_t handle, const std::string& context, const std::string& id_or_name,
 bool* p_is_encrypted = 0, bool* p_is_transient = 0, bool* p_is_file_field = 0 );

std::string CIYAM_BASE_DECL_SPEC get_field_uom_symbol(
 size_t handle, const std::string& context, const std::string& id_or_name );

std::string CIYAM_BASE_DECL_SPEC get_field_display_name(
 size_t handle, const std::string& context, const std::string& id_or_name );

std::string CIYAM_BASE_DECL_SPEC get_field_value( size_t handle, const std::string& context, const std::string& field );

std::string CIYAM_BASE_DECL_SPEC get_field_values( size_t handle,
 const std::string& parent_context, const std::vector< std::string >& field_list,
 const std::string& tz_name, bool is_default = false, bool as_csv = false,
 std::vector< std::string >* p_raw_values = 0,
 const std::multimap< size_t, std::string >* p_inserts = 0,
 const std::map< std::string, std::string >* p_replace_map = 0,
 const std::vector< std::string >* p_omit_matching = 0, bool decrypt_for_blockchain_minter = false );

void CIYAM_BASE_DECL_SPEC set_any_field_ids_to_names( size_t handle,
 const std::string& context, std::vector< std::string >& fields );

void CIYAM_BASE_DECL_SPEC set_any_field_names_to_ids( size_t handle,
 const std::string& context, std::vector< std::string >& fields );

bool CIYAM_BASE_DECL_SPEC determine_alternative_key_fields( size_t handle, const std::string& context,
 const std::vector< std::string > available_fields, std::vector< std::string >& selected_fields );

void CIYAM_BASE_DECL_SPEC get_foreign_field_and_class_ids( size_t handle,
 const std::string& context, std::map< std::string, std::string >& foreign_field_and_class_ids );

void CIYAM_BASE_DECL_SPEC get_base_class_info( size_t handle,
 const std::string& context, std::vector< std::pair< std::string, std::string > >& base_class_info );

size_t CIYAM_BASE_DECL_SPEC obtain_instance_fk_lock( const std::string& lock_class_id, const std::string& key, bool review_required );

void CIYAM_BASE_DECL_SPEC release_obtained_lock( size_t lock_handle );

void CIYAM_BASE_DECL_SPEC dump_storage_cache( std::ostream& os );
void CIYAM_BASE_DECL_SPEC dump_storage_locks( std::ostream& os );

std::string CIYAM_BASE_DECL_SPEC exec_bulk_ops( const std::string& module,
 const std::string& uid, const std::string& dtm, const std::string& mclass,
 const std::string& filename, const std::string& export_fields, const std::string& tz_name, bool destroy_records,
 const std::string& search_text, const std::string& search_query, const std::string& fixed_field_values, command_handler& handler );

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

std::string CIYAM_BASE_DECL_SPEC instance_class( size_t handle, const std::string& context );
std::string CIYAM_BASE_DECL_SPEC instance_key_info( size_t handle, const std::string& context, bool key_only = false );

std::string CIYAM_BASE_DECL_SPEC instance_get_variable( size_t handle, const std::string& context, const std::string& vname );
void CIYAM_BASE_DECL_SPEC instance_set_variable( size_t handle, const std::string& context, const std::string& vname, const std::string& value );

enum instance_check_rc
{
   e_instance_check_rc_okay,
   e_instance_check_rc_not_found
};

void CIYAM_BASE_DECL_SPEC instance_check( class_base& instance, instance_check_rc* p_rc = 0 );

bool CIYAM_BASE_DECL_SPEC is_change_locked( class_base& instance, bool include_cascades = false );
bool CIYAM_BASE_DECL_SPEC is_destroy_locked( class_base& instance, bool include_cascades = false );

bool CIYAM_BASE_DECL_SPEC is_create_locked_by_own_session( class_base& instance,
 const char* p_key = 0, bool copy_field_values = false, bool also_check_tx_lock_type = false );

bool CIYAM_BASE_DECL_SPEC was_create_locked_by_own_session(
 class_base& instance, const char* p_key = 0, bool copy_field_values = false );

bool CIYAM_BASE_DECL_SPEC is_update_locked_by_own_session( class_base& instance, const char* p_key = 0 );
bool CIYAM_BASE_DECL_SPEC is_destroy_locked_by_own_session( class_base& instance, const char* p_key = 0 );
bool CIYAM_BASE_DECL_SPEC is_update_or_destroy_locked_by_own_session( class_base& instance, const char* p_key = 0 );

enum instance_fetch_rc
{
   e_instance_fetch_rc_okay,
   e_instance_fetch_rc_not_found
};

void CIYAM_BASE_DECL_SPEC instance_fetch( size_t handle,
 const std::string& context, const std::string& key_info, instance_fetch_rc* p_rc = 0 );

void CIYAM_BASE_DECL_SPEC instance_set_parent( size_t handle,
 const std::string& context, const std::string& new_key );

void CIYAM_BASE_DECL_SPEC instance_prepare_execute( size_t handle,
 const std::string& context, const std::string& key, const std::string& ver_info, bool skip_after_fetch = false );

std::string CIYAM_BASE_DECL_SPEC instance_execute( size_t handle,
 const std::string& context, const std::string& key, const std::string& method_name_and_args );

std::string CIYAM_BASE_DECL_SPEC instance_get_fields_and_values(
 size_t handle, const std::string& context, const std::string& key );

bool CIYAM_BASE_DECL_SPEC instance_persistence_type_is_sql( size_t handle );

enum iter_direction
{
   e_iter_direction_forwards,
   e_iter_direction_backwards
};

bool CIYAM_BASE_DECL_SPEC instance_iterate( size_t handle,
 const std::string& context, const std::string& key_info,
 const std::string& fields, const std::string& text, const std::string& query,
 const std::string& security_info, iter_direction direction, bool inclusive, int row_limit = 0,
 sql_optimisation optimisation = e_sql_optimisation_none, const std::set< std::string >* p_filters = 0 );

bool CIYAM_BASE_DECL_SPEC instance_iterate_next( size_t handle, const std::string& context );
void CIYAM_BASE_DECL_SPEC instance_iterate_stop( size_t handle, const std::string& context );

bool CIYAM_BASE_DECL_SPEC instance_filtered( size_t handle, const std::string& context );
bool CIYAM_BASE_DECL_SPEC instance_has_transient_filter_fields( size_t handle, const std::string& context );

void CIYAM_BASE_DECL_SPEC instance_review_begin( size_t handle, const std::string& context, const std::string& key );
void CIYAM_BASE_DECL_SPEC instance_review_finish( size_t handle, const std::string& context );

void CIYAM_BASE_DECL_SPEC op_instance_create( size_t handle,
 const std::string& context, const std::string& key, bool internal_operation = true, op_create_rc* p_rc = 0 );

void CIYAM_BASE_DECL_SPEC op_instance_update( size_t handle, const std::string& context,
 const std::string& key, const std::string& ver_info, bool internal_operation = true, op_update_rc* p_rc = 0 );

void CIYAM_BASE_DECL_SPEC op_instance_destroy( size_t handle, const std::string& context,
 const std::string& key, const std::string& ver_info, bool internal_operation = true, op_destroy_rc* p_rc = 0 );

void CIYAM_BASE_DECL_SPEC op_instance_apply( size_t handle, const std::string& context,
 bool internal_operation = true, op_apply_rc* p_rc = 0, std::set< std::string >* p_fields_set = 0 );

void CIYAM_BASE_DECL_SPEC op_instance_cancel( size_t handle, const std::string& context, bool internal_operation = true );

void CIYAM_BASE_DECL_SPEC output_progress_message( const std::string& message );

void CIYAM_BASE_DECL_SPEC transaction_start( );
void CIYAM_BASE_DECL_SPEC transaction_commit( );
void CIYAM_BASE_DECL_SPEC transaction_rollback( );

size_t CIYAM_BASE_DECL_SPEC transaction_id( );
size_t CIYAM_BASE_DECL_SPEC transaction_level( );

size_t CIYAM_BASE_DECL_SPEC set_transaction_id( size_t tx_id );
size_t CIYAM_BASE_DECL_SPEC next_transaction_id( );

struct transaction_commit_helper
{
   virtual void at_commit( ) { }
   virtual void after_commit( ) { }
   virtual void after_rollback( ) { }
};

std::string CIYAM_BASE_DECL_SPEC transaction_log_command( );

void CIYAM_BASE_DECL_SPEC transaction_log_command(
 const std::string& log_command, transaction_commit_helper* p_tx_helper = 0, bool replace_current = false );

void CIYAM_BASE_DECL_SPEC append_transaction_log_command( const std::string& log_command );

void CIYAM_BASE_DECL_SPEC append_transaction_for_blockchain_application(
 const std::string& application, const std::string& transaction_hash );

class CIYAM_BASE_DECL_SPEC transaction
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

struct CIYAM_BASE_DECL_SPEC record_initialiser
{
   record_initialiser( std::ofstream& log_file );
   ~record_initialiser( );
};

enum instance_op
{
   e_instance_op_none,
   e_instance_op_review,
   e_instance_op_create,
   e_instance_op_update,
   e_instance_op_destroy
};

enum instance_op_rc
{
   e_instance_op_rc_okay,
   e_instance_op_rc_locked,
   e_instance_op_rc_invalid,
   e_instance_op_rc_not_found,
   e_instance_op_rc_constrained,
   e_instance_op_rc_child_locked,
   e_instance_op_rc_already_exists
};

void CIYAM_BASE_DECL_SPEC begin_instance_op( instance_op op,
 class_base& instance, const std::string& key, bool internal_operation = true, instance_op_rc* p_rc = 0 );

void CIYAM_BASE_DECL_SPEC finish_instance_op( class_base& instance, bool apply_changes,
 bool internal_operation = true, instance_op_rc* p_rc = 0, std::set< std::string >* p_fields_set = 0 );

void CIYAM_BASE_DECL_SPEC perform_instance_fetch(
 class_base& instance, const std::string& key_info,
 instance_fetch_rc* p_rc = 0, bool only_sys_fields = false, bool do_not_use_cache = false );

bool CIYAM_BASE_DECL_SPEC perform_instance_iterate(
 class_base& instance, const std::string& key_info,
 const std::string& fields, const std::string& text,
 const std::string& query, const std::string& security_info,
 iter_direction direction, bool inclusive = true, int row_limit = 0,
 sql_optimisation optimisation = e_sql_optimisation_none, const std::set< std::string >* p_filters = 0 );

bool CIYAM_BASE_DECL_SPEC perform_instance_iterate_next( class_base& instance );

#endif
