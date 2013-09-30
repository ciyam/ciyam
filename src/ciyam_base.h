// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2013 CIYAM Open Developers
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

#  define TRACE_COMMANDS   0x0001
#  define TRACE_SQLSTMTS   0x0002
#  define TRACE_CLASSOPS   0x0004
#  define TRACE_MODS_GEN   0x0008
#  define TRACE_SQLCLSET   0x0010
#  define TRACE_FLD_VALS   0x0020
#  define TRACE_LOCK_OPS   0x0040
#  define TRACE_CTR_DTRS   0x0080
#  define TRACE_SESSIONS   0x0100
#  define TRACE_MAIL_OPS   0x0200
#  define TRACE_ANYTHING   0xffff

#  define IF_IS_TRACING( flags )\
if( get_trace_flags( ) & ( flags ) )

#  define TRACE_LOG( flag, message )\
if( ( flag == TRACE_ANYTHING ) || ( get_trace_flags( ) & flag ) )\
   log_trace_message( flag, message )

#  define TEMP_TRACE( message ) TRACE_LOG( TRACE_ANYTHING, message )

int CIYAM_BASE_DECL_SPEC get_trace_flags( );
void CIYAM_BASE_DECL_SPEC set_trace_flags( int flags );

void CIYAM_BASE_DECL_SPEC list_trace_flags( std::vector< std::string >& flag_names );

void CIYAM_BASE_DECL_SPEC log_trace_message( int flag, const std::string& message );

void CIYAM_BASE_DECL_SPEC init_globals( );
void CIYAM_BASE_DECL_SPEC term_globals( );

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

int CIYAM_BASE_DECL_SPEC get_max_user_limit( );
void CIYAM_BASE_DECL_SPEC set_max_user_limit( int new_limit );

std::string CIYAM_BASE_DECL_SPEC get_prefix( );
std::string CIYAM_BASE_DECL_SPEC get_domain( );

std::string CIYAM_BASE_DECL_SPEC get_app_url( const std::string& suffix );

std::string CIYAM_BASE_DECL_SPEC get_identity( bool prepend_sid = false, bool append_max_user_limit = false );
void CIYAM_BASE_DECL_SPEC set_identity( const std::string& identity_info );

std::string CIYAM_BASE_DECL_SPEC get_checksum( const std::string& data, bool use_reg_key = false );

std::string CIYAM_BASE_DECL_SPEC get_timezone( );
std::string CIYAM_BASE_DECL_SPEC get_web_root( );

bool CIYAM_BASE_DECL_SPEC get_is_accepted_ip_addr( const std::string& ip_addr );

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

std::string CIYAM_BASE_DECL_SPEC encrypt_password(
 const std::string& password, bool no_ssl = false, bool no_salt = false, bool hash_only = false );

std::string CIYAM_BASE_DECL_SPEC decrypt_password(
 const std::string& password, bool no_ssl = false, bool no_salt = false, bool hash_only = false );

std::string CIYAM_BASE_DECL_SPEC totp_secret_key( const std::string& unique );

int CIYAM_BASE_DECL_SPEC exec_system( const std::string& cmd, bool async = false );

int CIYAM_BASE_DECL_SPEC run_script( const std::string& script_name, bool async = true );

std::string CIYAM_BASE_DECL_SPEC process_script_args( const std::string& raw_args, bool is_script_arg = true );

void CIYAM_BASE_DECL_SPEC generate_new_script_sio_files( );

size_t CIYAM_BASE_DECL_SPEC init_session( command_handler& cmd_handler );
void CIYAM_BASE_DECL_SPEC term_session( );

size_t CIYAM_BASE_DECL_SPEC session_id( );

void CIYAM_BASE_DECL_SPEC list_sessions( std::ostream& os, bool inc_dtms = true );

command_handler CIYAM_BASE_DECL_SPEC& get_session_command_handler( );

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
std::string CIYAM_BASE_DECL_SPEC get_sql_password( );

std::string CIYAM_BASE_DECL_SPEC get_encrypted_gpg_password( );
std::string CIYAM_BASE_DECL_SPEC get_encrypted_pem_password( );
std::string CIYAM_BASE_DECL_SPEC get_encrypted_sql_password( );
std::string CIYAM_BASE_DECL_SPEC get_encrypted_pop3_password( );
std::string CIYAM_BASE_DECL_SPEC get_encrypted_smtp_password( );

std::string CIYAM_BASE_DECL_SPEC get_default_storage( );
void CIYAM_BASE_DECL_SPEC set_default_storage( const std::string& name );

int CIYAM_BASE_DECL_SPEC get_session_timeout( );
void CIYAM_BASE_DECL_SPEC set_session_timeout( int seconds );

std::string CIYAM_BASE_DECL_SPEC get_session_variable( const std::string& name );
void CIYAM_BASE_DECL_SPEC set_session_variable( const std::string& name, const std::string& value );
bool CIYAM_BASE_DECL_SPEC set_session_variable( const std::string& name, const std::string& value, const std::string& current );

struct temporary_session_variable
{
   temporary_session_variable( const std::string& name, const std::string& value )
    :
    name( name )
   {
      original_value = get_session_variable( name );
      set_session_variable( name, value );
   }

   ~temporary_session_variable( )
   {
      set_session_variable( name, original_value );
   }

   std::string name;
   std::string original_value;
};

std::string CIYAM_BASE_DECL_SPEC get_special_var_name( special_var var );

void CIYAM_BASE_DECL_SPEC set_default_session_variables( );

std::string CIYAM_BASE_DECL_SPEC get_system_variable( const std::string& name );
void CIYAM_BASE_DECL_SPEC set_system_variable( const std::string& name, const std::string& value );
bool CIYAM_BASE_DECL_SPEC set_system_variable( const std::string& name, const std::string& value, const std::string& current );

void CIYAM_BASE_DECL_SPEC init_storage( const std::string& name,
 const std::string& directory, command_handler& cmd_handler, bool lock_for_admin );
void CIYAM_BASE_DECL_SPEC create_storage( const std::string& name,
 const std::string& directory, command_handler& cmd_handler, bool lock_for_admin );
void CIYAM_BASE_DECL_SPEC attach_storage( const std::string& name, command_handler& cmd_handler, bool lock_for_admin );

void CIYAM_BASE_DECL_SPEC backup_storage( command_handler& cmd_handler, int* p_truncation_count = 0 );
void CIYAM_BASE_DECL_SPEC restore_storage( command_handler& cmd_handler );

void CIYAM_BASE_DECL_SPEC upgrade_storage( command_handler& cmd_handler );

void CIYAM_BASE_DECL_SPEC term_storage( command_handler& cmd_handler );

void CIYAM_BASE_DECL_SPEC storage_admin_name_lock( const std::string& name );

void CIYAM_BASE_DECL_SPEC storage_cache_clear( );
size_t CIYAM_BASE_DECL_SPEC storage_cache_limit( );
size_t CIYAM_BASE_DECL_SPEC storage_cache_limit( size_t new_limit );

void CIYAM_BASE_DECL_SPEC slice_storage_log( command_handler& cmd_handler,
 const std::string& name, const std::vector< std::string >& module_list );
void CIYAM_BASE_DECL_SPEC splice_storage_log( command_handler& cmd_handler,
 const std::string& name, const std::vector< std::string >& module_list );

bool CIYAM_BASE_DECL_SPEC storage_is_dead_key( const std::string& cid, const std::string& key );

void CIYAM_BASE_DECL_SPEC storage_add_dead_key( const std::string& cid,
 const std::string& key, bool is_prefix = false );

struct CIYAM_BASE_DECL_SPEC storage_bulk_write_pause
{
   storage_bulk_write_pause( );
   ~storage_bulk_write_pause( );
};

std::string CIYAM_BASE_DECL_SPEC storage_name( );
std::string CIYAM_BASE_DECL_SPEC storage_identity( );
std::string CIYAM_BASE_DECL_SPEC storage_module_directory( );

std::string CIYAM_BASE_DECL_SPEC storage_web_root( bool expand );
void CIYAM_BASE_DECL_SPEC storage_web_root( const std::string& new_root );

void CIYAM_BASE_DECL_SPEC storage_lock_all_tables( );
void CIYAM_BASE_DECL_SPEC storage_unlock_all_tables( );

bool CIYAM_BASE_DECL_SPEC storage_locked_for_admin( );

ods CIYAM_BASE_DECL_SPEC& storage_instance( );

std::string CIYAM_BASE_DECL_SPEC gen_key( const char* p_suffix = 0 );

std::string CIYAM_BASE_DECL_SPEC get_uid( bool remove_display_name = true );
void CIYAM_BASE_DECL_SPEC set_uid( const std::string& uid );

bool CIYAM_BASE_DECL_SPEC is_sys_uid( );
bool CIYAM_BASE_DECL_SPEC is_auto_uid( );
bool CIYAM_BASE_DECL_SPEC is_init_uid( );
bool CIYAM_BASE_DECL_SPEC is_admin_uid( );
bool CIYAM_BASE_DECL_SPEC is_script_uid( );

bool CIYAM_BASE_DECL_SPEC is_system_uid( );

bool CIYAM_BASE_DECL_SPEC is_admin_uid_key( const std::string& key );

bool CIYAM_BASE_DECL_SPEC is_uid_not_self_and_not_in_set( const std::string& key, const std::string& key_set );

bool CIYAM_BASE_DECL_SPEC has_sec_level( const std::string& level );

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

size_t CIYAM_BASE_DECL_SPEC get_next_handle( );

void CIYAM_BASE_DECL_SPEC module_list( std::ostream& os );
size_t CIYAM_BASE_DECL_SPEC module_count( );

void CIYAM_BASE_DECL_SPEC module_class_list( const std::string& module, std::ostream& os );
void CIYAM_BASE_DECL_SPEC module_strings_list( const std::string& module, std::ostream& os );
void CIYAM_BASE_DECL_SPEC module_class_fields_list(
 const std::string& module, const std::string& class_id_or_name, std::ostream& os );

void CIYAM_BASE_DECL_SPEC module_load( const std::string& module_name,
 command_handler& cmd_handler, bool log_tx_comment = false, bool append_to_module_list = true );
void CIYAM_BASE_DECL_SPEC module_unload( const std::string& module_name, command_handler& cmd_handler, bool check_controlled = false );
void CIYAM_BASE_DECL_SPEC module_unload_all( command_handler& cmd_handler );

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

void CIYAM_BASE_DECL_SPEC get_all_enum_pairs( size_t handle,
 const std::string& context, std::vector< std::pair< std::string, std::string > >& pairs );

void CIYAM_BASE_DECL_SPEC get_all_field_names( size_t handle,
 const std::string& context, std::vector< std::string >& fields );

std::string CIYAM_BASE_DECL_SPEC get_field_name_for_id(
 size_t handle, const std::string& context, const std::string& id, bool no_throw = false );

std::string CIYAM_BASE_DECL_SPEC get_field_id_for_name(
 size_t handle, const std::string& context, const std::string& name, bool no_throw = false );

std::string CIYAM_BASE_DECL_SPEC get_field_type_name( size_t handle, const std::string& context, const std::string& id_or_name );

std::string CIYAM_BASE_DECL_SPEC get_field_uom_symbol( size_t handle, const std::string& context, const std::string& id_or_name );
std::string CIYAM_BASE_DECL_SPEC get_field_display_name( size_t handle, const std::string& context, const std::string& id_or_name );

std::string CIYAM_BASE_DECL_SPEC get_field_values(
 size_t handle, const std::string& parent_context,
 const std::vector< std::string >& field_list, const std::string& tz_name,
 bool is_default = false, bool as_csv = false, std::vector< std::string >* p_raw_values = 0,
 const std::map< int, std::string >* p_inserts = 0, const std::map< std::string, std::string >* p_package_map = 0 );

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

void CIYAM_BASE_DECL_SPEC export_package( const std::string& module,
 const std::string& mclass, const std::string& key, const std::string& exclude_info,
 const std::string& test_info, const std::string& include_info, const std::string& filename );

void CIYAM_BASE_DECL_SPEC import_package(
 const std::string& module, const std::string& uid,
 const std::string& dtm, const std::string& filename,
 const std::string& key_prefix, const std::string& replace_info,
 const std::string& skip_field_info, bool new_only = false, bool for_remove = false );

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

bool CIYAM_BASE_DECL_SPEC is_create_locked_by_own_session( class_base& instance, const char* p_key = 0, bool copy_field_values = false );
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

void CIYAM_BASE_DECL_SPEC transaction_log_command( const std::string& log_command );

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
 class_base& instance, const std::string& key_info, instance_fetch_rc* p_rc = 0, bool only_sys_fields = false );

bool CIYAM_BASE_DECL_SPEC perform_instance_iterate(
 class_base& instance, const std::string& key_info,
 const std::string& fields, const std::string& text,
 const std::string& query, const std::string& security_info,
 iter_direction direction, bool inclusive = true, int row_limit = 0,
 sql_optimisation optimisation = e_sql_optimisation_none, const std::set< std::string >* p_filters = 0 );

bool CIYAM_BASE_DECL_SPEC perform_instance_iterate_next( class_base& instance );
void CIYAM_BASE_DECL_SPEC perform_instance_iterate_stop( class_base& instance );

#endif

