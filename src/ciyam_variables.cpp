// Copyright (c) 2017-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <map>
#  include <deque>
#  include <iostream>
#endif

#include "ciyam_variables.h"

#include "ods.h"
#include "threads.h"
#include "date_time.h"
#include "utilities.h"
#include "ciyam_base.h"
#include "class_base.h"
#include "ciyam_files.h"
#include "ods_file_system.h"

using namespace std;

extern size_t g_active_sessions;

namespace
{

#include "module_strings.h"
#include "ciyam_constants.h"

const int c_max_lock_attempts = 20;
const int c_lock_attempt_sleep_time = 100;

const size_t c_secret_truncate_length = 9;

const char* const c_variable_expression = "variable_expression";

const char* const c_special_variable_id = "@id";
const char* const c_special_variable_os = "@os";
const char* const c_special_variable_dtm = "@dtm";
const char* const c_special_variable_grp = "@grp";
const char* const c_special_variable_hub = "@hub";
const char* const c_special_variable_key = "@key";
const char* const c_special_variable_sec = "@sec";
const char* const c_special_variable_set = "@set";
const char* const c_special_variable_tag = "@tag";
const char* const c_special_variable_tls = "@tls";
const char* const c_special_variable_uid = "@uid";
const char* const c_special_variable_sid = "@sid";
const char* const c_special_variable_arg1 = "@arg1";
const char* const c_special_variable_arg2 = "@arg2";
const char* const c_special_variable_arg3 = "@arg3";
const char* const c_special_variable_auto = "@auto";
const char* const c_special_variable_cube = "@cube";
const char* const c_special_variable_gids = "@gids";
const char* const c_special_variable_hash = "@hash";
const char* const c_special_variable_null = "@null";
const char* const c_special_variable_val1 = "@val1";
const char* const c_special_variable_val2 = "@val2";
const char* const c_special_variable_file = "@file";
const char* const c_special_variable_loop = "@loop";
const char* const c_special_variable_name = "@name";
const char* const c_special_variable_none = "@none";
const char* const c_special_variable_path = "@path";
const char* const c_special_variable_peer = "@peer";
const char* const c_special_variable_port = "@port";
const char* const c_special_variable_size = "@size";
const char* const c_special_variable_slot = "@slot";
const char* const c_special_variable_type = "@type";
const char* const c_special_variable_user = "@user";
const char* const c_special_variable_uuid = "@uuid";
const char* const c_special_variable_algos = "@algos";
const char* const c_special_variable_array = "@array";
const char* const c_special_variable_async = "@async";
const char* const c_special_variable_bytes = "@bytes";
const char* const c_special_variable_class = "@class";
const char* const c_special_variable_deque = "@deque";
const char* const c_special_variable_embed = "@embed";
const char* const c_special_variable_files = "@files";
const char* const c_special_variable_print = "@print";
const char* const c_special_variable_quiet = "@quiet";
const char* const c_special_variable_slotx = "@slotx";
const char* const c_special_variable_title = "@title";
const char* const c_special_variable_branch = "@branch";
const char* const c_special_variable_cloned = "@cloned";
const char* const c_special_variable_images = "@images";
const char* const c_special_variable_module = "@module";
const char* const c_special_variable_opened = "@opened";
const char* const c_special_variable_pubkey = "@pubkey";
const char* const c_special_variable_return = "@return";
const char* const c_special_variable_script = "@script";
const char* const c_special_variable_command = "@command";
const char* const c_special_variable_do_exec = "@do_exec";
const char* const c_special_variable_ip_addr = "@ip_addr";
const char* const c_special_variable_is_last = "@is_last";
const char* const c_special_variable_message = "@message";
const char* const c_special_variable_opening = "@opening";
const char* const c_special_variable_package = "@package";
const char* const c_special_variable_pending = "@pending";
const char* const c_special_variable_pubkeyx = "@pubkeyx";
const char* const c_special_variable_restore = "@restore";
const char* const c_special_variable_slowest = "@slowest";
const char* const c_special_variable_storage = "@storage";
const char* const c_special_variable_tz_name = "@tz_name";
const char* const c_special_variable_trigger = "@trigger";
const char* const c_special_variable_waiting = "@waiting";
const char* const c_special_variable_executed = "@executed";
const char* const c_special_variable_identity = "@identity";
const char* const c_special_variable_key_info = "@key_info";
const char* const c_special_variable_notifier = "@notifier";
const char* const c_special_variable_peer_hub = "@peer_hub";
const char* const c_special_variable_progress = "@progress";
const char* const c_special_variable_unlocked = "@unlocked";
const char* const c_special_variable_updating = "@updating";
const char* const c_special_variable_args_file = "@args_file";
const char* const c_special_variable_cmd_delay = "@cmd_delay";
const char* const c_special_variable_crypt_key = "@crypt_key";
const char* const c_special_variable_datachain = "@datachain";
const char* const c_special_variable_decrement = "@decrement";
const char* const c_special_variable_executing = "@executing";
const char* const c_special_variable_file_list = "@file_list";
const char* const c_special_variable_image_dir = "@image_dir";
const char* const c_special_variable_increment = "@increment";
const char* const c_special_variable_list_hash = "@list_hash";
const char* const c_special_variable_peer_data = "@peer_data";
const char* const c_special_variable_peer_port = "@peer_port";
const char* const c_special_variable_peer_user = "@peer_user";
const char* const c_special_variable_preparing = "@preparing";
const char* const c_special_variable_repo_lock = "@repo_lock";
const char* const c_special_variable_uid_check = "@uid_check";
const char* const c_special_variable_val_error = "@val_error";
const char* const c_special_variable_blockchain = "@blockchain";
const char* const c_special_variable_chain_type = "@chain_type";
const char* const c_special_variable_extra_info = "@extra_info";
const char* const c_special_variable_key_suffix = "@key_suffix";
const char* const c_special_variable_permission = "@permission";
const char* const c_special_variable_queue_puts = "@queue_puts";
const char* const c_special_variable_session_id = "@session_id";
const char* const c_special_variable_sid_locked = "@sid_locked";
const char* const c_special_variable_sid_secure = "@sid_secure";
const char* const c_special_variable_tag_prefix = "@tag_prefix";
const char* const c_special_variable_tree_count = "@tree_count";
const char* const c_special_variable_tree_match = "@tree_match";
const char* const c_special_variable_tree_total = "@tree_total";
const char* const c_special_variable_allow_async = "@allow_async";
const char* const c_special_variable_application = "@application";
const char* const c_special_variable_errors_only = "@errors_only";
const char* const c_special_variable_init_log_id = "@init_log_id";
const char* const c_special_variable_opened_user = "@opened_user";
const char* const c_special_variable_output_file = "@output_file";
const char* const c_special_variable_paired_sync = "@paired_sync";
const char* const c_special_variable_path_prefix = "@path_prefix";
const char* const c_special_variable_peer_msleep = "@peer_msleep";
const char* const c_special_variable_permissions = "@permissions";
const char* const c_special_variable_queue_peers = "@queue_peers";
const char* const c_special_variable_secret_hash = "@secret_hash";
const char* const c_special_variable_skip_update = "@skip_update";
const char* const c_special_variable_sleep_after = "@sleep_after";
const char* const c_special_variable_state_names = "@state_names";
const char* const c_special_variable_style_brief = "@style_brief";
const char* const c_special_variable_submit_type = "@submit_type";
const char* const c_special_variable_transaction = "@transaction";
const char* const c_special_variable_backup_files = "@backup_files";
const char* const c_special_variable_block_height = "@block_height";
const char* const c_special_variable_cmd_and_args = "@cmd_and_args";
const char* const c_special_variable_dump_minimal = "@dump_minimal";
const char* const c_special_variable_local_folder = "@local_folder";
const char* const c_special_variable_local_origin = "@local_origin";
const char* const c_special_variable_local_prefix = "@local_prefix";
const char* const c_special_variable_local_starts = "@local_starts";
const char* const c_special_variable_local_suffix = "@local_suffix";
const char* const c_special_variable_opened_files = "@opened_files";
const char* const c_special_variable_peer_map_key = "@peer_map_key";
const char* const c_special_variable_shared_files = "@shared_files";
const char* const c_special_variable_app_directory = "@app_directory";
const char* const c_special_variable_backup_needed = "@backup_needed";
const char* const c_special_variable_export_needed = "@export_needed";
const char* const c_special_variable_import_needed = "@import_needed";
const char* const c_special_variable_last_file_put = "@last_file_put";
const char* const c_special_variable_log_files_dir = "@log_files_dir";
const char* const c_special_variable_num_put_files = "@num_put_files";
const char* const c_special_variable_peer_sec_hash = "@peer_sec_hash";
const char* const c_special_variable_prior_storage = "@prior_storage";
const char* const c_special_variable_progress_pipe = "@progress_pipe";
const char* const c_special_variable_rewind_height = "@rewind_height";
const char* const c_special_variable_shared_secret = "@shared_secret";
const char* const c_special_variable_skip_set_vars = "@skip_set_vars";
const char* const c_special_variable_stream_cipher = "@stream_cipher";
const char* const c_special_variable_sub_directory = "@sub_directory";
const char* const c_special_variable_trace_filters = "@trace_filters";
const char* const c_special_variable_update_fields = "@update_fields";
const char* const c_special_variable_cmd_delay_wait = "@cmd_delay_wait";
const char* const c_special_variable_files_area_dir = "@files_area_dir";
const char* const c_special_variable_force_internal = "@force_internal";
const char* const c_special_variable_peer_clone_key = "@peer_clone_key";
const char* const c_special_variable_peer_initiator = "@peer_initiator";
const char* const c_special_variable_peer_responder = "@peer_responder";
const char* const c_special_variable_prepare_system = "@prepare_system";
const char* const c_special_variable_progress_clear = "@progress_clear";
const char* const c_special_variable_progress_count = "@progress_count";
const char* const c_special_variable_progress_fracs = "@progress_fracs";
const char* const c_special_variable_progress_prior = "@progress_prior";
const char* const c_special_variable_progress_total = "@progress_total";
const char* const c_special_variable_progress_value = "@progress_value";
const char* const c_special_variable_restore_needed = "@restore_needed";
const char* const c_special_variable_restore_system = "@restore_system";
const char* const c_special_variable_stop_condemned = "@stop_condemned";
const char* const c_special_variable_sys_var_prefix = "@sys_var_prefix";
const char* const c_special_variable_blockchain_data = "@blockchain_data";
const char* const c_special_variable_blockchain_user = "@blockchain_user";
const char* const c_special_variable_ciyam_list_hash = "@ciyam_list_hash";
const char* const c_special_variable_cmd_delay_after = "@cmd_delay_after";
const char* const c_special_variable_force_db_create = "@force_db_create";
const char* const c_special_variable_log_all_scripts = "@log_all_scripts";
const char* const c_special_variable_notifier_events = "@notifier_events";
const char* const c_special_variable_paired_identity = "@paired_identity";
const char* const c_special_variable_queue_hub_users = "@queue_hub_users";
const char* const c_special_variable_row_cache_limit = "@row_cache_limit";
const char* const c_special_variable_system_identity = "@system_identity";
const char* const c_special_variable_total_put_files = "@total_put_files";
const char* const c_special_variable_can_omit_prepare = "@can_omit_prepare";
const char* const c_special_variable_check_if_changed = "@check_if_changed";
const char* const c_special_variable_dummy_time_stamp = "@dummy_time_stamp";
const char* const c_special_variable_preparing_backup = "@preparing_backup";
const char* const c_special_variable_progress_message = "@progress_message";
const char* const c_special_variable_progress_seconds = "@progress_seconds";
const char* const c_special_variable_protocol_handler = "@protocol_handler";
const char* const c_special_variable_session_id_owner = "@session_id_owner";
const char* const c_special_variable_skip_after_fetch = "@skip_after_fetch";
const char* const c_special_variable_skip_persistence = "@skip_persistence";
const char* const c_special_variable_skip_submit_file = "@skip_submit_file";
const char* const c_special_variable_style_full_blown = "@style_full_blown";
const char* const c_special_variable_trigger_variable = "@trigger_variable";
const char* const c_special_variable_autoscript_reload = "@autoscript_reload";
const char* const c_special_variable_blockchain_height = "@blockchain_height";
const char* const c_special_variable_blockchain_is_hub = "@blockchain_is_hub";
const char* const c_special_variable_fields_and_values = "@fields_and_values";
const char* const c_special_variable_generate_log_file = "@generate_log_file";
const char* const c_special_variable_last_suffixed_key = "@last_suffixed_key";
const char* const c_special_variable_package_type_path = "@package_type_path";
const char* const c_special_variable_peer_data_created = "@peer_data_created";
const char* const c_special_variable_peer_is_dependent = "@peer_is_dependent";
const char* const c_special_variable_preparing_restore = "@preparing_restore";
const char* const c_special_variable_trace_session_ids = "@trace_session_ids";
const char* const c_special_variable_attached_file_path = "@attached_file_path";
const char* const c_special_variable_check_script_error = "@check_script_error";
const char* const c_special_variable_encrypted_password = "@encrypted_password";
const char* const c_special_variable_exec_system_append = "@exec_system_append";
const char* const c_special_variable_extra_field_values = "@extra_field_values";
const char* const c_special_variable_file_info_buffered = "@file_info_buffered";
const char* const c_special_variable_fixed_field_values = "@fixed_field_values";
const char* const c_special_variable_generate_hub_block = "@generate_hub_block";
const char* const c_special_variable_repo_entry_missing = "@repo_entry_missing";
const char* const c_special_variable_system_is_for_demo = "@system_is_for_demo";
const char* const c_special_variable_system_is_for_devt = "@system_is_for_devt";
const char* const c_special_variable_blockchain_data_key = "@blockchain_data_key";
const char* const c_special_variable_blockchain_identity = "@blockchain_identity";
const char* const c_special_variable_blockchain_is_owner = "@blockchain_is_owner";
const char* const c_special_variable_blockchain_num_puts = "@blockchain_num_puts";
const char* const c_special_variable_key_prefix_is_first = "@key_prefix_is_first";
const char* const c_special_variable_peer_identity_alias = "@peer_identity_alias";
const char* const c_special_variable_repo_crypt_password = "@repo_crypt_password";
const char* const c_special_variable_skip_parent_updates = "@skip_parent_updates";
const char* const c_special_variable_blockchain_hind_hash = "@blockchain_hind_hash";
const char* const c_special_variable_blockchain_unix_time = "@blockchain_unix_time";
const char* const c_special_variable_disallow_connections = "@disallow_connections";
const char* const c_special_variable_ods_cache_hit_ratios = "@ods_cache_hit_ratios";
const char* const c_special_variable_regex_search_expense = "@regex_search_expense";
const char* const c_special_variable_secondary_validation = "@secondary_validation";
const char* const c_special_variable_blockchain_force_skip = "@blockchain_force_skip";
const char* const c_special_variable_blockchain_next_extra = "@blockchain_next_extra";
const char* const c_special_variable_blockchain_time_value = "@blockchain_time_value";
const char* const c_special_variable_keep_user_peers_alive = "@keep_user_peers_alive";
const char* const c_special_variable_package_install_extra = "@package_install_extra";
const char* const c_special_variable_peer_is_synchronising = "@peer_is_synchronising";
const char* const c_special_variable_prepare_backup_needed = "@prepare_backup_needed";
const char* const c_special_variable_blockchain_check_dummy = "@blockchain_check_dummy";
const char* const c_special_variable_blockchain_is_checking = "@blockchain_is_checking";
const char* const c_special_variable_blockchain_is_combined = "@blockchain_is_combined";
const char* const c_special_variable_blockchain_is_fetching = "@blockchain_is_fetching";
const char* const c_special_variable_exporting_for_identity = "@exporting_for_identity";
const char* const c_special_variable_peer_limited_arguments = "@peer_limited_arguments";
const char* const c_special_variable_single_string_response = "@single_string_response";
const char* const c_special_variable_skip_tagging_if_exists = "@skip_tagging_if_exists";
const char* const c_special_variable_blockchain_archive_path = "@blockchain_archive_path";
const char* const c_special_variable_blockchain_checked_skip = "@blockchain_checked_skip";
const char* const c_special_variable_blockchain_first_mapped = "@blockchain_first_mapped";
const char* const c_special_variable_blockchain_height_other = "@blockchain_height_other";
const char* const c_special_variable_blockchain_op_list_hash = "@blockchain_op_list_hash";
const char* const c_special_variable_complete_restore_needed = "@complete_restore_needed";
const char* const c_special_variable_blockchain_backup_height = "@blockchain_backup_height";
const char* const c_special_variable_blockchain_put_list_hash = "@blockchain_put_list_hash";
const char* const c_special_variable_blockchain_shared_height = "@blockchain_shared_height";
const char* const c_special_variable_blockchain_stream_cipher = "@blockchain_stream_cipher";
const char* const c_special_variable_blockchain_zenith_height = "@blockchain_zenith_height";
const char* const c_special_variable_blockchain_checked_shared = "@blockchain_checked_shared";
const char* const c_special_variable_blockchain_get_tree_files = "@blockchain_get_tree_files";
const char* const c_special_variable_blockchain_num_tree_items = "@blockchain_num_tree_items";
const char* const c_special_variable_blockchain_other_is_owner = "@blockchain_other_is_owner";
const char* const c_special_variable_blockchain_peer_supporter = "@blockchain_peer_supporter";
const char* const c_special_variable_blockchain_tree_root_hash = "@blockchain_tree_root_hash";
const char* const c_special_variable_blockchain_backup_identity = "@blockchain_backup_identity";
const char* const c_special_variable_blockchain_block_file_hash = "@blockchain_block_file_hash";
const char* const c_special_variable_blockchain_both_are_owners = "@blockchain_both_are_owners";
const char* const c_special_variable_blockchain_found_secondary = "@blockchain_found_secondary";
const char* const c_special_variable_blockchain_num_for_support = "@blockchain_num_for_support";
const char* const c_special_variable_blockchain_peer_hub_height = "@blockchain_peer_hub_height";
const char* const c_special_variable_blockchain_shared_identity = "@blockchain_shared_identity";
const char* const c_special_variable_use_insecure_peer_protocol = "@use_insecure_peer_protocol";
const char* const c_special_variable_blockchain_waiting_for_hub = "@blockchain_waiting_for_hub";
const char* const c_special_variable_blockchain_block_processing = "@blockchain_block_processing";
const char* const c_special_variable_blockchain_zenith_tree_hash = "@blockchain_zenith_tree_hash";
const char* const c_special_variable_totals_exclude_hidden_blobs = "@totals_exclude_hidden_blobs";
const char* const c_special_variable_blockchain_height_processing = "@blockchain_height_processing";
const char* const c_special_variable_blockchain_peer_hub_identity = "@blockchain_peer_hub_identity";
const char* const c_special_variable_blockchain_targeted_identity = "@blockchain_targeted_identity";
const char* const c_special_variable_blockchain_non_extra_identity = "@blockchain_non_extra_identity";
const char* const c_special_variable_queue_archive_info_for_resize = "@queue_archive_info_for_resize";
const char* const c_special_variable_blockchain_peer_has_supporters = "@blockchain_peer_has_supporters";
const char* const c_special_variable_blockchain_primary_pubkey_hash = "@blockchain_primary_pubkey_hash";
const char* const c_special_variable_blockchain_signature_file_hash = "@blockchain_signature_file_hash";
const char* const c_special_variable_display_last_two_for_duplicate = "@display_last_two_for_duplicate";
const char* const c_special_variable_blockchain_tertiary_pubkey_hash = "@blockchain_tertiary_pubkey_hash";
const char* const c_special_variable_blockchain_secondary_pubkey_hash = "@blockchain_secondary_pubkey_hash";
const char* const c_special_variable_skip_total_child_field_in_parent = "@skip_total_child_field_in_parent";

trace_mutex g_mutex;

map< string, string > g_variables;
map< string, deque< string > > g_deque_variables;

inline string quote_if_contains_white_space( const string& name )
{
   string retval( name );

   if( retval.find_first_of( c_whitespace_chars ) != string::npos )
      retval = "\"" + retval + "\"";

   return retval;
}

string g_secret_hash_prefix;

// NOTE: System variable names that begin with "@secret_hash_" will have their values
// truncated to the first five characters with an ellipsis appended when they are not
// retrieved internally (i.e. via the "system_variable" protocol command). Whilst not
// a foolproof method to prevent discovery this should prevent simple access to these
// from either an application protocol script or the FCGI UI.
void truncate_value_for_secret_hash_prefixed_name( const string& name, string& value )
{
   if( name.find( g_secret_hash_prefix ) == 0 )
   {
      if( value.length( ) > c_secret_truncate_length )
      {
         value.erase( c_secret_truncate_length );

         value += c_ellipsis;
      }
   }
}

vector< string > g_special_variable_names;

void init_special_variable_names( )
{
   guard g( g_mutex );

   g_secret_hash_prefix = string( c_special_variable_secret_hash ) + "_";

   // NOTE: These must align with the enums in "ciyam_variable_names.h".
   if( g_special_variable_names.empty( ) )
   {
      g_special_variable_names.push_back( c_special_variable_id );
      g_special_variable_names.push_back( c_special_variable_os );
      g_special_variable_names.push_back( c_special_variable_dtm );
      g_special_variable_names.push_back( c_special_variable_grp );
      g_special_variable_names.push_back( c_special_variable_hub );
      g_special_variable_names.push_back( c_special_variable_key );
      g_special_variable_names.push_back( c_special_variable_sec );
      g_special_variable_names.push_back( c_special_variable_set );
      g_special_variable_names.push_back( c_special_variable_tag );
      g_special_variable_names.push_back( c_special_variable_tls );
      g_special_variable_names.push_back( c_special_variable_uid );
      g_special_variable_names.push_back( c_special_variable_sid );
      g_special_variable_names.push_back( c_special_variable_arg1 );
      g_special_variable_names.push_back( c_special_variable_arg2 );
      g_special_variable_names.push_back( c_special_variable_arg3 );
      g_special_variable_names.push_back( c_special_variable_auto );
      g_special_variable_names.push_back( c_special_variable_cube );
      g_special_variable_names.push_back( c_special_variable_gids );
      g_special_variable_names.push_back( c_special_variable_hash );
      g_special_variable_names.push_back( c_special_variable_null );
      g_special_variable_names.push_back( c_special_variable_val1 );
      g_special_variable_names.push_back( c_special_variable_val2 );
      g_special_variable_names.push_back( c_special_variable_file );
      g_special_variable_names.push_back( c_special_variable_loop );
      g_special_variable_names.push_back( c_special_variable_name );
      g_special_variable_names.push_back( c_special_variable_none );
      g_special_variable_names.push_back( c_special_variable_path );
      g_special_variable_names.push_back( c_special_variable_peer );
      g_special_variable_names.push_back( c_special_variable_port );
      g_special_variable_names.push_back( c_special_variable_size );
      g_special_variable_names.push_back( c_special_variable_slot );
      g_special_variable_names.push_back( c_special_variable_type );
      g_special_variable_names.push_back( c_special_variable_user );
      g_special_variable_names.push_back( c_special_variable_uuid );
      g_special_variable_names.push_back( c_special_variable_algos );
      g_special_variable_names.push_back( c_special_variable_array );
      g_special_variable_names.push_back( c_special_variable_async );
      g_special_variable_names.push_back( c_special_variable_bytes );
      g_special_variable_names.push_back( c_special_variable_class );
      g_special_variable_names.push_back( c_special_variable_deque );
      g_special_variable_names.push_back( c_special_variable_embed );
      g_special_variable_names.push_back( c_special_variable_files );
      g_special_variable_names.push_back( c_special_variable_print );
      g_special_variable_names.push_back( c_special_variable_quiet );
      g_special_variable_names.push_back( c_special_variable_slotx );
      g_special_variable_names.push_back( c_special_variable_title );
      g_special_variable_names.push_back( c_special_variable_branch );
      g_special_variable_names.push_back( c_special_variable_cloned );
      g_special_variable_names.push_back( c_special_variable_images );
      g_special_variable_names.push_back( c_special_variable_module );
      g_special_variable_names.push_back( c_special_variable_opened );
      g_special_variable_names.push_back( c_special_variable_pubkey );
      g_special_variable_names.push_back( c_special_variable_return );
      g_special_variable_names.push_back( c_special_variable_script );
      g_special_variable_names.push_back( c_special_variable_command );
      g_special_variable_names.push_back( c_special_variable_do_exec );
      g_special_variable_names.push_back( c_special_variable_ip_addr );
      g_special_variable_names.push_back( c_special_variable_is_last );
      g_special_variable_names.push_back( c_special_variable_message );
      g_special_variable_names.push_back( c_special_variable_opening );
      g_special_variable_names.push_back( c_special_variable_package );
      g_special_variable_names.push_back( c_special_variable_pending );
      g_special_variable_names.push_back( c_special_variable_pubkeyx );
      g_special_variable_names.push_back( c_special_variable_restore );
      g_special_variable_names.push_back( c_special_variable_slowest );
      g_special_variable_names.push_back( c_special_variable_storage );
      g_special_variable_names.push_back( c_special_variable_tz_name );
      g_special_variable_names.push_back( c_special_variable_trigger );
      g_special_variable_names.push_back( c_special_variable_waiting );
      g_special_variable_names.push_back( c_special_variable_executed );
      g_special_variable_names.push_back( c_special_variable_identity );
      g_special_variable_names.push_back( c_special_variable_key_info );
      g_special_variable_names.push_back( c_special_variable_notifier );
      g_special_variable_names.push_back( c_special_variable_peer_hub );
      g_special_variable_names.push_back( c_special_variable_progress );
      g_special_variable_names.push_back( c_special_variable_unlocked );
      g_special_variable_names.push_back( c_special_variable_updating );
      g_special_variable_names.push_back( c_special_variable_args_file );
      g_special_variable_names.push_back( c_special_variable_cmd_delay );
      g_special_variable_names.push_back( c_special_variable_crypt_key );
      g_special_variable_names.push_back( c_special_variable_datachain );
      g_special_variable_names.push_back( c_special_variable_decrement );
      g_special_variable_names.push_back( c_special_variable_executing );
      g_special_variable_names.push_back( c_special_variable_file_list );
      g_special_variable_names.push_back( c_special_variable_image_dir );
      g_special_variable_names.push_back( c_special_variable_increment );
      g_special_variable_names.push_back( c_special_variable_list_hash );
      g_special_variable_names.push_back( c_special_variable_peer_data );
      g_special_variable_names.push_back( c_special_variable_peer_port );
      g_special_variable_names.push_back( c_special_variable_peer_user );
      g_special_variable_names.push_back( c_special_variable_preparing );
      g_special_variable_names.push_back( c_special_variable_repo_lock );
      g_special_variable_names.push_back( c_special_variable_uid_check );
      g_special_variable_names.push_back( c_special_variable_val_error );
      g_special_variable_names.push_back( c_special_variable_blockchain );
      g_special_variable_names.push_back( c_special_variable_chain_type );
      g_special_variable_names.push_back( c_special_variable_extra_info );
      g_special_variable_names.push_back( c_special_variable_key_suffix );
      g_special_variable_names.push_back( c_special_variable_permission );
      g_special_variable_names.push_back( c_special_variable_queue_puts );
      g_special_variable_names.push_back( c_special_variable_session_id );
      g_special_variable_names.push_back( c_special_variable_sid_locked );
      g_special_variable_names.push_back( c_special_variable_sid_secure );
      g_special_variable_names.push_back( c_special_variable_tag_prefix );
      g_special_variable_names.push_back( c_special_variable_tree_count );
      g_special_variable_names.push_back( c_special_variable_tree_match );
      g_special_variable_names.push_back( c_special_variable_tree_total );
      g_special_variable_names.push_back( c_special_variable_allow_async );
      g_special_variable_names.push_back( c_special_variable_application );
      g_special_variable_names.push_back( c_special_variable_errors_only );
      g_special_variable_names.push_back( c_special_variable_init_log_id );
      g_special_variable_names.push_back( c_special_variable_opened_user );
      g_special_variable_names.push_back( c_special_variable_output_file );
      g_special_variable_names.push_back( c_special_variable_paired_sync );
      g_special_variable_names.push_back( c_special_variable_path_prefix );
      g_special_variable_names.push_back( c_special_variable_peer_msleep );
      g_special_variable_names.push_back( c_special_variable_permissions );
      g_special_variable_names.push_back( c_special_variable_queue_peers );
      g_special_variable_names.push_back( c_special_variable_secret_hash );
      g_special_variable_names.push_back( c_special_variable_skip_update );
      g_special_variable_names.push_back( c_special_variable_sleep_after );
      g_special_variable_names.push_back( c_special_variable_state_names );
      g_special_variable_names.push_back( c_special_variable_style_brief );
      g_special_variable_names.push_back( c_special_variable_submit_type );
      g_special_variable_names.push_back( c_special_variable_transaction );
      g_special_variable_names.push_back( c_special_variable_backup_files );
      g_special_variable_names.push_back( c_special_variable_block_height );
      g_special_variable_names.push_back( c_special_variable_cmd_and_args );
      g_special_variable_names.push_back( c_special_variable_dump_minimal );
      g_special_variable_names.push_back( c_special_variable_local_folder );
      g_special_variable_names.push_back( c_special_variable_local_origin );
      g_special_variable_names.push_back( c_special_variable_local_prefix );
      g_special_variable_names.push_back( c_special_variable_local_starts );
      g_special_variable_names.push_back( c_special_variable_local_suffix );
      g_special_variable_names.push_back( c_special_variable_opened_files );
      g_special_variable_names.push_back( c_special_variable_peer_map_key );
      g_special_variable_names.push_back( c_special_variable_shared_files );
      g_special_variable_names.push_back( c_special_variable_app_directory );
      g_special_variable_names.push_back( c_special_variable_backup_needed );
      g_special_variable_names.push_back( c_special_variable_export_needed );
      g_special_variable_names.push_back( c_special_variable_import_needed );
      g_special_variable_names.push_back( c_special_variable_last_file_put );
      g_special_variable_names.push_back( c_special_variable_log_files_dir );
      g_special_variable_names.push_back( c_special_variable_num_put_files );
      g_special_variable_names.push_back( c_special_variable_peer_sec_hash );
      g_special_variable_names.push_back( c_special_variable_prior_storage );
      g_special_variable_names.push_back( c_special_variable_progress_pipe );
      g_special_variable_names.push_back( c_special_variable_rewind_height );
      g_special_variable_names.push_back( c_special_variable_shared_secret );
      g_special_variable_names.push_back( c_special_variable_skip_set_vars );
      g_special_variable_names.push_back( c_special_variable_stream_cipher );
      g_special_variable_names.push_back( c_special_variable_sub_directory );
      g_special_variable_names.push_back( c_special_variable_trace_filters );
      g_special_variable_names.push_back( c_special_variable_update_fields );
      g_special_variable_names.push_back( c_special_variable_cmd_delay_wait );
      g_special_variable_names.push_back( c_special_variable_files_area_dir );
      g_special_variable_names.push_back( c_special_variable_force_internal );
      g_special_variable_names.push_back( c_special_variable_peer_clone_key );
      g_special_variable_names.push_back( c_special_variable_peer_initiator );
      g_special_variable_names.push_back( c_special_variable_peer_responder );
      g_special_variable_names.push_back( c_special_variable_prepare_system );
      g_special_variable_names.push_back( c_special_variable_progress_clear );
      g_special_variable_names.push_back( c_special_variable_progress_count );
      g_special_variable_names.push_back( c_special_variable_progress_fracs );
      g_special_variable_names.push_back( c_special_variable_progress_prior );
      g_special_variable_names.push_back( c_special_variable_progress_total );
      g_special_variable_names.push_back( c_special_variable_progress_value );
      g_special_variable_names.push_back( c_special_variable_restore_needed );
      g_special_variable_names.push_back( c_special_variable_restore_system );
      g_special_variable_names.push_back( c_special_variable_stop_condemned );
      g_special_variable_names.push_back( c_special_variable_sys_var_prefix );
      g_special_variable_names.push_back( c_special_variable_blockchain_data );
      g_special_variable_names.push_back( c_special_variable_blockchain_user );
      g_special_variable_names.push_back( c_special_variable_ciyam_list_hash );
      g_special_variable_names.push_back( c_special_variable_cmd_delay_after );
      g_special_variable_names.push_back( c_special_variable_force_db_create );
      g_special_variable_names.push_back( c_special_variable_log_all_scripts );
      g_special_variable_names.push_back( c_special_variable_notifier_events );
      g_special_variable_names.push_back( c_special_variable_paired_identity );
      g_special_variable_names.push_back( c_special_variable_queue_hub_users );
      g_special_variable_names.push_back( c_special_variable_row_cache_limit );
      g_special_variable_names.push_back( c_special_variable_system_identity );
      g_special_variable_names.push_back( c_special_variable_total_put_files );
      g_special_variable_names.push_back( c_special_variable_can_omit_prepare );
      g_special_variable_names.push_back( c_special_variable_check_if_changed );
      g_special_variable_names.push_back( c_special_variable_dummy_time_stamp );
      g_special_variable_names.push_back( c_special_variable_preparing_backup );
      g_special_variable_names.push_back( c_special_variable_progress_message );
      g_special_variable_names.push_back( c_special_variable_progress_seconds );
      g_special_variable_names.push_back( c_special_variable_protocol_handler );
      g_special_variable_names.push_back( c_special_variable_session_id_owner );
      g_special_variable_names.push_back( c_special_variable_skip_after_fetch );
      g_special_variable_names.push_back( c_special_variable_skip_persistence );
      g_special_variable_names.push_back( c_special_variable_skip_submit_file );
      g_special_variable_names.push_back( c_special_variable_style_full_blown );
      g_special_variable_names.push_back( c_special_variable_trigger_variable );
      g_special_variable_names.push_back( c_special_variable_autoscript_reload );
      g_special_variable_names.push_back( c_special_variable_blockchain_height );
      g_special_variable_names.push_back( c_special_variable_blockchain_is_hub );
      g_special_variable_names.push_back( c_special_variable_fields_and_values );
      g_special_variable_names.push_back( c_special_variable_generate_log_file );
      g_special_variable_names.push_back( c_special_variable_last_suffixed_key );
      g_special_variable_names.push_back( c_special_variable_package_type_path );
      g_special_variable_names.push_back( c_special_variable_peer_data_created );
      g_special_variable_names.push_back( c_special_variable_peer_is_dependent );
      g_special_variable_names.push_back( c_special_variable_preparing_restore );
      g_special_variable_names.push_back( c_special_variable_trace_session_ids );
      g_special_variable_names.push_back( c_special_variable_attached_file_path );
      g_special_variable_names.push_back( c_special_variable_check_script_error );
      g_special_variable_names.push_back( c_special_variable_encrypted_password );
      g_special_variable_names.push_back( c_special_variable_exec_system_append );
      g_special_variable_names.push_back( c_special_variable_extra_field_values );
      g_special_variable_names.push_back( c_special_variable_file_info_buffered );
      g_special_variable_names.push_back( c_special_variable_fixed_field_values );
      g_special_variable_names.push_back( c_special_variable_generate_hub_block );
      g_special_variable_names.push_back( c_special_variable_repo_entry_missing );
      g_special_variable_names.push_back( c_special_variable_system_is_for_demo );
      g_special_variable_names.push_back( c_special_variable_system_is_for_devt );
      g_special_variable_names.push_back( c_special_variable_blockchain_data_key );
      g_special_variable_names.push_back( c_special_variable_blockchain_identity );
      g_special_variable_names.push_back( c_special_variable_blockchain_is_owner );
      g_special_variable_names.push_back( c_special_variable_blockchain_num_puts );
      g_special_variable_names.push_back( c_special_variable_key_prefix_is_first );
      g_special_variable_names.push_back( c_special_variable_peer_identity_alias );
      g_special_variable_names.push_back( c_special_variable_repo_crypt_password );
      g_special_variable_names.push_back( c_special_variable_skip_parent_updates );
      g_special_variable_names.push_back( c_special_variable_blockchain_hind_hash );
      g_special_variable_names.push_back( c_special_variable_blockchain_unix_time );
      g_special_variable_names.push_back( c_special_variable_disallow_connections );
      g_special_variable_names.push_back( c_special_variable_ods_cache_hit_ratios );
      g_special_variable_names.push_back( c_special_variable_regex_search_expense );
      g_special_variable_names.push_back( c_special_variable_secondary_validation );
      g_special_variable_names.push_back( c_special_variable_blockchain_force_skip );
      g_special_variable_names.push_back( c_special_variable_blockchain_next_extra );
      g_special_variable_names.push_back( c_special_variable_blockchain_time_value );
      g_special_variable_names.push_back( c_special_variable_keep_user_peers_alive );
      g_special_variable_names.push_back( c_special_variable_package_install_extra );
      g_special_variable_names.push_back( c_special_variable_peer_is_synchronising );
      g_special_variable_names.push_back( c_special_variable_prepare_backup_needed );
      g_special_variable_names.push_back( c_special_variable_blockchain_check_dummy );
      g_special_variable_names.push_back( c_special_variable_blockchain_is_checking );
      g_special_variable_names.push_back( c_special_variable_blockchain_is_combined );
      g_special_variable_names.push_back( c_special_variable_blockchain_is_fetching );
      g_special_variable_names.push_back( c_special_variable_exporting_for_identity );
      g_special_variable_names.push_back( c_special_variable_peer_limited_arguments );
      g_special_variable_names.push_back( c_special_variable_single_string_response );
      g_special_variable_names.push_back( c_special_variable_skip_tagging_if_exists );
      g_special_variable_names.push_back( c_special_variable_blockchain_archive_path );
      g_special_variable_names.push_back( c_special_variable_blockchain_checked_skip );
      g_special_variable_names.push_back( c_special_variable_blockchain_first_mapped );
      g_special_variable_names.push_back( c_special_variable_blockchain_height_other );
      g_special_variable_names.push_back( c_special_variable_blockchain_op_list_hash );
      g_special_variable_names.push_back( c_special_variable_complete_restore_needed );
      g_special_variable_names.push_back( c_special_variable_blockchain_backup_height );
      g_special_variable_names.push_back( c_special_variable_blockchain_put_list_hash );
      g_special_variable_names.push_back( c_special_variable_blockchain_shared_height );
      g_special_variable_names.push_back( c_special_variable_blockchain_stream_cipher );
      g_special_variable_names.push_back( c_special_variable_blockchain_zenith_height );
      g_special_variable_names.push_back( c_special_variable_blockchain_checked_shared );
      g_special_variable_names.push_back( c_special_variable_blockchain_get_tree_files );
      g_special_variable_names.push_back( c_special_variable_blockchain_num_tree_items );
      g_special_variable_names.push_back( c_special_variable_blockchain_other_is_owner );
      g_special_variable_names.push_back( c_special_variable_blockchain_peer_supporter );
      g_special_variable_names.push_back( c_special_variable_blockchain_tree_root_hash );
      g_special_variable_names.push_back( c_special_variable_blockchain_backup_identity );
      g_special_variable_names.push_back( c_special_variable_blockchain_block_file_hash );
      g_special_variable_names.push_back( c_special_variable_blockchain_both_are_owners );
      g_special_variable_names.push_back( c_special_variable_blockchain_found_secondary );
      g_special_variable_names.push_back( c_special_variable_blockchain_num_for_support );
      g_special_variable_names.push_back( c_special_variable_blockchain_peer_hub_height );
      g_special_variable_names.push_back( c_special_variable_blockchain_shared_identity );
      g_special_variable_names.push_back( c_special_variable_use_insecure_peer_protocol );
      g_special_variable_names.push_back( c_special_variable_blockchain_waiting_for_hub );
      g_special_variable_names.push_back( c_special_variable_blockchain_block_processing );
      g_special_variable_names.push_back( c_special_variable_blockchain_zenith_tree_hash );
      g_special_variable_names.push_back( c_special_variable_totals_exclude_hidden_blobs );
      g_special_variable_names.push_back( c_special_variable_blockchain_height_processing );
      g_special_variable_names.push_back( c_special_variable_blockchain_peer_hub_identity );
      g_special_variable_names.push_back( c_special_variable_blockchain_targeted_identity );
      g_special_variable_names.push_back( c_special_variable_blockchain_non_extra_identity );
      g_special_variable_names.push_back( c_special_variable_queue_archive_info_for_resize );
      g_special_variable_names.push_back( c_special_variable_blockchain_peer_has_supporters );
      g_special_variable_names.push_back( c_special_variable_blockchain_primary_pubkey_hash );
      g_special_variable_names.push_back( c_special_variable_blockchain_signature_file_hash );
      g_special_variable_names.push_back( c_special_variable_display_last_two_for_duplicate );
      g_special_variable_names.push_back( c_special_variable_blockchain_tertiary_pubkey_hash );
      g_special_variable_names.push_back( c_special_variable_blockchain_secondary_pubkey_hash );
      g_special_variable_names.push_back( c_special_variable_skip_total_child_field_in_parent );

      // NOTE: This must always be pushed last.
      g_special_variable_names.push_back( c_dummy );
   }
}

void touch_or_remove( const string& var_name, bool remove )
{
   string file_name( c_hidden_file_prefix + var_name.substr( 1 ) );

   if( remove )
      file_remove( file_name );
   else if( !file_exists( file_name ) )
      file_touch( file_name, 0, true );
}

void set_file_var_name( const string& var_name )
{
   string file_name( c_hidden_file_prefix + var_name.substr( 1 ) );

   if( file_exists( file_name ) )
      g_variables[ var_name ] = c_true_value;
   else if( g_variables.count( var_name ) )
      g_variables.erase( var_name );
}

void set_backup_needed( bool check = true )
{
   string var_name( c_special_variable_backup_needed );

   if( !check )
      touch_or_remove( var_name, true );

   set_file_var_name( var_name );
}

void set_restore_needed( bool check = true )
{
   string var_name( c_special_variable_restore_needed );

   if( !check )
      touch_or_remove( var_name, true );

   set_file_var_name( var_name );
}

void set_generate_hub_block( bool check = true )
{
   string var_name( c_special_variable_generate_hub_block );

   if( !check )
      touch_or_remove( var_name, true );

   set_file_var_name( var_name );
}

void set_system_is_for_devt( )
{
   if( file_exists( "config.info" ) )
      g_variables[ c_special_variable_system_is_for_devt ] = c_true_value;
}

void set_ods_cache_hit_ratios( )
{
   g_variables[ c_special_variable_ods_cache_hit_ratios ] = system_ods_instance( ).get_cache_hit_ratios( );
}

void set_prepare_backup_needed( bool check = true )
{
   string var_name( c_special_variable_prepare_backup_needed );

   if( !check )
      touch_or_remove( var_name, true );

   set_file_var_name( var_name );
}

void set_complete_restore_needed( bool check = true )
{
   string var_name( c_special_variable_complete_restore_needed );

   if( !check )
      touch_or_remove( var_name, true );

   set_file_var_name( var_name );
}

void check_system_variable_can_be_set( const string& var_name )
{
   bool okay = true;

   if( ( var_name == c_special_variable_os )
    || ( var_name == c_special_variable_peer_port )
    || ( var_name == c_special_variable_sid_locked )
    || ( var_name == c_special_variable_sid_secure )
    || ( var_name == c_special_variable_backup_files )
    || ( var_name == c_special_variable_opened_files )
    || ( var_name == c_special_variable_shared_files )
    || ( var_name == c_special_variable_system_identity )
    || ( var_name == c_special_variable_system_is_for_devt ) )
      okay = false;

   if( !okay )
      throw runtime_error( "invalid attempt to change system variable '" + var_name + "'" );
}

}

string get_special_var_name( special_var var )
{
   string s;

   if( g_special_variable_names.empty( ) )
      init_special_variable_names( );

   size_t offset = ( size_t )var;

   if( offset >= g_special_variable_names.size( ) )
      throw runtime_error( "unexpected special var offset " + to_string( offset ) );

   s = g_special_variable_names[ offset ];

   return s;
}

system_variable_lock::system_variable_lock( const string& name )
 :
 name( name )
{
   acquire_lock( name );
}

system_variable_lock::system_variable_lock( const string& name, const string& display )
 :
 name( name )
{
   acquire_lock( name, display.c_str( ) );
}

void system_variable_lock::acquire_lock( const string& name, const char* p_display_name_str )
{
   bool acquired = false;

   for( size_t i = 0; i < c_max_lock_attempts; i++ )
   {
      // NOTE: Empty code block for scope purposes.
      {
         guard g( g_mutex );

         if( set_system_variable( name, c_true_value, string( "" ) ) )
         {
            acquired = true;
            break;
         }
      }

      msleep( c_lock_attempt_sleep_time );
   }

   if( !acquired )
   {
      if( p_display_name_str )
         throw runtime_error( get_string_message( GS( c_str_class_is_locked ),
          make_pair( c_str_class_is_locked_class, p_display_name_str ) ) );
      else
         throw runtime_error( "unable to acquire lock for system variable '" + name + "'" );
   }
}

system_variable_lock::~system_variable_lock( )
{
   set_system_variable( name, "" );
}

string get_raw_system_variable( const string& name, bool is_internal )
{
   guard g( g_mutex );

   string retval;
   string var_name( name );

   bool had_persist_prefix = false;
   bool had_restore_prefix = false;

   if( !var_name.empty( ) )
   {
      if( name[ 0 ] == c_persist_variable_prefix )
         had_persist_prefix = true;
      else if( name[ 0 ] == c_restore_variable_prefix )
         had_restore_prefix = true;

      if( had_persist_prefix || had_restore_prefix )
         var_name.erase( 0, 1 );
   }

   // NOTE: The special system variable prefix is only intended for
   // testing purposes and is only applicable to unrestricted lists.
   string sys_var_prefix;

   if( g_variables.count( c_special_variable_sys_var_prefix ) )
      sys_var_prefix = g_variables[ c_special_variable_sys_var_prefix ];

   // NOTE: One or more persistent variables can have their values
   // either stored or restored depending upon the prefix used and
   // optional wildcard characters. If the name was the prefix and
   // nothing else then output all persistent variable names along
   // with their values (for the persist prefix) or instead output
   // just those whose values now differ (for the restore prefix).
   if( had_persist_prefix || had_restore_prefix )
   {
      bool output_all_persistent_variables = false;

      if( var_name.empty( ) && had_persist_prefix )
         output_all_persistent_variables = true;

      ods::bulk_write bulk_write( system_ods_instance( ) );

      scoped_ods_instance ods_instance( system_ods_instance( ) );

      system_ods_file_system( ).set_root_folder( c_system_variables_folder );

      if( !var_name.empty( ) && had_persist_prefix
       && var_name.find_first_of( "?*" ) == string::npos )
      {
         string value;

         if( g_variables.count( var_name ) )
            value = g_variables[ var_name ];

         if( value.empty( ) )
         {
            if( system_ods_file_system( ).has_file( var_name ) )
               system_ods_file_system( ).remove_file( var_name );
         }
         else
            system_ods_file_system( ).store_as_text_file( var_name, value );
      }
      else
      {
         vector< string > variable_files;

         string expr( sys_var_prefix );

         if( var_name.empty( ) || ( var_name == "*" ) )
            expr += "*";
         else
            expr = var_name;

         system_ods_file_system( ).list_files( expr, variable_files );

         for( size_t i = 0; i < variable_files.size( ); i++ )
         {
            string next( variable_files[ i ] );

            string value;

            if( had_restore_prefix || output_all_persistent_variables )
            {
               system_ods_file_system( ).fetch_from_text_file( next, value );

               if( !var_name.empty( ) )
                  g_variables[ next ] = value;
               else
               {
                  string next_value;

                  if( g_variables.count( next ) )
                     next_value = g_variables[ next ];

                  if( output_all_persistent_variables || ( value != next_value ) )
                  {
                     if( !retval.empty( ) )
                        retval += "\n";

                     if( !is_internal )
                        truncate_value_for_secret_hash_prefixed_name( next, value );

                     retval += quote_if_contains_white_space( next ) + ' ' + value;
                  }
               }
            }
            else
            {
               if( g_variables.count( next ) )
                  value = g_variables[ next ];

               if( value.empty( ) )
                  system_ods_file_system( ).remove_file( next );
               else
                  system_ods_file_system( ).store_as_text_file( next, value );
            }
         }
      }
   }
   else if( var_name.find_first_of( "?*" ) != string::npos )
   {
      if( var_name == "*" )
         var_name = sys_var_prefix + var_name;

      map< string, string >::const_iterator ci;

      if( wildcard_match( var_name, c_special_variable_backup_needed ) )
         set_backup_needed( );

      if( wildcard_match( var_name, c_special_variable_trace_filters ) )
      {
         string trace_filters( get_trace_filters( ) );

         if( trace_filters.empty( ) )
            g_variables.erase( c_special_variable_trace_filters );
         else
            g_variables[ c_special_variable_trace_filters ] = trace_filters;
      }

      if( wildcard_match( var_name, c_special_variable_restore_needed ) )
         set_restore_needed( );

      if( wildcard_match( var_name, c_special_variable_generate_hub_block ) )
         set_generate_hub_block( );

      if( wildcard_match( var_name, c_special_variable_trace_session_ids ) )
      {
         string trace_session_ids( get_trace_session_ids( ) );

         if( trace_session_ids.empty( ) )
            g_variables.erase( c_special_variable_trace_session_ids );
         else
            g_variables[ c_special_variable_trace_session_ids ] = trace_session_ids;
      }

      if( wildcard_match( var_name, c_special_variable_system_is_for_devt ) )
         set_system_is_for_devt( );

      if( wildcard_match( var_name, c_special_variable_ods_cache_hit_ratios ) )
         set_ods_cache_hit_ratios( );

      if( wildcard_match( var_name, c_special_variable_prepare_backup_needed ) )
         set_prepare_backup_needed( );

      if( wildcard_match( var_name, c_special_variable_complete_restore_needed ) )
         set_complete_restore_needed( );

      for( ci = g_variables.begin( ); ci != g_variables.end( ); ++ci )
      {
         if( wildcard_match( var_name, ci->first ) )
         {
            if( !retval.empty( ) )
               retval += "\n";

            string next( ci->first );
            string value( ci->second );

            if( !is_internal )
               truncate_value_for_secret_hash_prefixed_name( next, value );

            retval += quote_if_contains_white_space( next ) + ' ' + value;
         }
      }

      map< string, deque< string > >::const_iterator dci;

      for( dci = g_deque_variables.begin( ); dci != g_deque_variables.end( ); ++dci )
      {
         if( wildcard_match( var_name, dci->first ) )
         {
            if( !retval.empty( ) )
               retval += "\n";

            retval += quote_if_contains_white_space( dci->first ) + ' ' + dci->second.front( );

            if( dci->second.size( ) > 1 )
               retval += " (+" + to_string( dci->second.size( ) - 1 ) + ")";
         }
      }
   }
   else
   {
      if( var_name == c_special_variable_backup_needed )
         set_backup_needed( );

      if( var_name == c_special_variable_trace_filters )
      {
         string trace_filters( get_trace_filters( ) );

         if( trace_filters.empty( ) )
            g_variables.erase( c_special_variable_trace_filters );
         else
            g_variables[ c_special_variable_trace_filters ] = trace_filters;
      }

      if( var_name == c_special_variable_restore_needed )
         set_restore_needed( );

      if( var_name == c_special_variable_generate_hub_block )
         set_generate_hub_block( );

      if( var_name == c_special_variable_trace_session_ids )
      {
         string trace_session_ids( get_trace_session_ids( ) );

         if( trace_session_ids.empty( ) )
            g_variables.erase( c_special_variable_trace_session_ids );
         else
            g_variables[ c_special_variable_trace_session_ids ] = trace_session_ids;
      }

      if( var_name == c_special_variable_system_is_for_devt )
         set_system_is_for_devt( );

      if( var_name == c_special_variable_ods_cache_hit_ratios )
         set_ods_cache_hit_ratios( );

      if( var_name == c_special_variable_prepare_backup_needed )
         set_prepare_backup_needed( );

      if( var_name == c_special_variable_complete_restore_needed )
         set_complete_restore_needed( );

      if( var_name.find( c_special_variable_queue_prefix ) == 0 )
      {
         if( g_deque_variables.count( var_name ) )
         {
            if( g_deque_variables[ var_name ].size( ) )
            {
               retval = g_deque_variables[ var_name ].front( );
               g_deque_variables[ var_name ].pop_front( );
            }

            if( !g_deque_variables[ var_name ].size( ) )
               g_deque_variables.erase( var_name );
         }
      }
      else if( g_variables.count( var_name ) )
      {
         retval = g_variables[ var_name ];

         if( !is_internal )
            truncate_value_for_secret_hash_prefixed_name( var_name, retval );
      }
      else if( name == get_special_var_name( e_special_var_none ) )
         retval = " ";
      else if( var_name == c_special_variable_files_area_dir )
         retval = c_ciyam_files_directory;
   }

   return retval;
}

struct raw_system_variable_getter : variable_getter
{
   raw_system_variable_getter( bool is_internal ) : is_internal( is_internal ) { }

   string get_value( const string& name ) const { return get_raw_system_variable( name, is_internal ); }

   bool is_internal;
};

string get_system_variable( const string& name_or_expr, bool is_internal )
{
   raw_system_variable_getter raw_getter( is_internal );

   variable_expression expr( name_or_expr, raw_getter );

   return expr.get_value( );
}

bool has_raw_system_variable( const string& name )
{
   return g_variables.count( name );
}

bool has_system_variable( const string& name_or_expr )
{
   return !get_system_variable( name_or_expr ).empty( );
}

void set_system_variable( const string& name, const string& value, bool is_init, progress* p_progress )
{
   // NOTE: The special variable "@command" is used to run and hold the results of a "ciyam_command".
   if( name == c_special_variable_command )
   {
      string val( value );
      string tmp_file_name( "~" + uuid( ).as_string( ) );

      if( !val.empty( ) )
      {
         string cmd( "./ciyam_command " + val + " >" + tmp_file_name );

         int rc = system( cmd.c_str( ) );
         ( void )rc;

         val = buffer_file( tmp_file_name );

         if( val.length( ) && val[ val.length( ) - 1 ] == '\n' )
            val.erase( val.length( ) - 1 );
      }

      if( val.empty( ) )
      {
         if( g_variables.count( name ) )
            g_variables.erase( name );
      }
      else
         g_variables[ name ] = val;

      file_remove( tmp_file_name );
   }
   else if( name == c_special_variable_backup_needed )
   {
      guard g( g_mutex );

      set_restore_needed( );
      set_prepare_backup_needed( );
      set_complete_restore_needed( );

      bool remove = value.empty( );

      if( g_variables.count( c_special_variable_restore_needed )
       || g_variables.count( c_special_variable_prepare_backup_needed )
       || g_variables.count( c_special_variable_complete_restore_needed ) )
         remove = true;

      touch_or_remove( name, remove );

      set_backup_needed( );
   }
   else if( name == c_special_variable_trace_filters )
      set_trace_filters( value );
   else if( name == c_special_variable_restore_needed )
   {
      guard g( g_mutex );

      set_backup_needed( );
      set_complete_restore_needed( );

      bool remove = value.empty( );

      if( g_variables.count( c_special_variable_complete_restore_needed ) )
         remove = true;

      touch_or_remove( name, remove );

      if( !remove )
      {
         if( g_variables.count( c_special_variable_backup_needed ) )
            set_backup_needed( false );

         if( g_variables.count( c_special_variable_prepare_backup_needed ) )
            set_prepare_backup_needed( false );
      }

      set_restore_needed( );
   }
   else if( name == c_special_variable_trace_session_ids )
      set_trace_session_ids( value );
   else if( name == c_special_variable_generate_hub_block )
   {
      guard g( g_mutex );

      string file_name( c_hidden_file_prefix + name.substr( 1 ) );

      touch_or_remove( name, value.empty( ) );

      set_generate_hub_block( );
   }
   else if( name == c_special_variable_prepare_backup_needed )
   {
      guard g( g_mutex );

      set_backup_needed( );
      set_restore_needed( );
      set_complete_restore_needed( );

      bool remove = value.empty( );

      if( g_variables.count( c_special_variable_restore_needed )
       || g_variables.count( c_special_variable_complete_restore_needed ) )
         remove = true;

      touch_or_remove( name, remove );

      set_prepare_backup_needed( );

      if( !remove )
      {
         if( g_variables.count( c_special_variable_backup_needed ) )
            set_backup_needed( false );
      }
   }
   else if( name == c_special_variable_complete_restore_needed )
   {
      guard g( g_mutex );

      set_backup_needed( );
      set_restore_needed( );
      set_prepare_backup_needed( );

      touch_or_remove( name, value.empty( ) );

      set_complete_restore_needed( );

      if( !value.empty( ) )
      {
         if( g_variables.count( c_special_variable_backup_needed ) )
            set_backup_needed( false );

         if( g_variables.count( c_special_variable_restore_needed ) )
            set_restore_needed( false );

         if( g_variables.count( c_special_variable_prepare_backup_needed ) )
            set_prepare_backup_needed( false );
      }
   }
   else
   {
      guard g( g_mutex );

      string val( value );

      bool persist = false;

      if( !name.empty( ) && name[ 0 ] == c_persist_variable_prefix )
         persist = true;

      if( !name.empty( ) )
      {
         string::size_type from = 0;

         if( persist )
            ++from;

         if( name.find_first_of( c_invalid_name_chars, from ) != string::npos )
            throw runtime_error( "invalid system variable name '" + name.substr( from ) + "'" );
      }

      string var_name( !persist ? name : name.substr( 1 ) );

      if( val == string( c_special_variable_increment )
       || val == string( c_special_variable_decrement ) )
      {
         int num_value = !g_variables.count( var_name )
          ? 0 : from_string< int >( g_variables[ var_name ] );

         if( val == string( c_special_variable_increment ) )
            ++num_value;
         else if( num_value > 0 )
            --num_value;

         if( num_value == 0 )
            val.clear( );
         else
            val = to_string( num_value );
      }

      string::size_type pos = var_name.find_first_of( "?*" );

      // NOTE: All "@queue_" prefixed variables are handled with
      // deques and their values cannot currently be persisted.
      if( var_name.find( c_special_variable_queue_prefix ) == 0 )
      {
         if( persist )
            throw runtime_error( "cannot persist '" + string( c_special_variable_queue_prefix ) + "' prefixed variables" );

         if( val.empty( ) )
            g_deque_variables.erase( var_name );
         else
            g_deque_variables[ var_name ].push_back( val );
      }
      else if( persist
       && ( ( var_name == c_special_variable_os )
       || ( var_name == c_special_variable_peer_port )
       || ( var_name == c_special_variable_sid_locked )
       || ( var_name == c_special_variable_sid_secure )
       || ( var_name == c_special_variable_log_files_dir )
       || ( var_name == c_special_variable_trace_filters )
       || ( var_name == c_special_variable_files_area_dir )
       || ( var_name == c_special_variable_trace_session_ids )
       || ( var_name == c_special_variable_generate_hub_block )
       || ( var_name == c_special_variable_disallow_connections )
       || ( var_name == c_special_variable_ods_cache_hit_ratios )
       || ( var_name == c_special_variable_complete_restore_needed ) ) )
         throw runtime_error( "cannot persist variable '" + var_name + "'" );
      else if( pos != string::npos )
      {
         if( persist )
            throw runtime_error( "cannot persist wildcard variables for '" + var_name + "'" );

         vector< string > vars_to_change;

         string prefix( var_name.substr( 0, pos ) );

         map< string, string >::iterator vi = g_variables.begin( );

         if( !prefix.empty( ) )
            vi = g_variables.lower_bound( prefix );

         for( ; vi != g_variables.end( ); ++vi )
         {
            if( wildcard_match( var_name, vi->first ) )
            {
               vars_to_change.push_back( vi->first );

               check_system_variable_can_be_set( vi->first );
            }

            if( !prefix.empty( ) && ( vi->first.find( prefix ) != 0 ) )
               break;
         }

         for( size_t i = 0; i < vars_to_change.size( ); i++ )
         {
            string next_name( vars_to_change[ i ] );

            if( !val.empty( ) )
               g_variables[ vars_to_change[ i ] ] = val;
            else
               g_variables.erase( vars_to_change[ i ] );
         }
      }
      else
      {
         if( val.empty( ) && ( var_name == string( c_special_variable_files_area_dir ) ) )
            val = string( c_ciyam_files_directory );

         if( !is_init )
            check_system_variable_can_be_set( var_name );

         if( !val.empty( ) )
            g_variables[ var_name ] = val;
         else
         {
            if( g_variables.count( var_name ) )
               g_variables.erase( var_name );
         }
      }

      if( var_name == string( c_special_variable_log_files_dir ) )
      {
         if( !is_init )
            set_log_files_dir( val );
      }
      else if( var_name == string( c_special_variable_files_area_dir ) )
      {
         if( !is_init )
         {
            if( g_active_sessions > 1 )
               throw runtime_error( "invalid switch files area whilst has other active sessions (including autostarted)" );

            // NOTE: Although already thread locked adds a mutex name here
            // just in case a deadlock could arise during the resync calls.
            guard g( g_mutex, "set_system_variable" );

            string from( get_files_area_dir( ) );

            set_files_area_dir( val );

            resync_files_area( p_progress );
            resync_system_ods( p_progress );

            resync_archive_info( p_progress );

            TRACE_LOG( TRACE_MINIMAL, "*** switched files area over from '" + from + "' to '" + val + "' ***" );
         }
      }
      else if( var_name == string( c_special_variable_ods_cache_hit_ratios ) )
         system_ods_instance( ).clear_cache_statistics( );

      if( persist )
      {
         ods::bulk_write bulk_write( system_ods_instance( ) );

         scoped_ods_instance ods_instance( system_ods_instance( ) );

         system_ods_file_system( ).set_root_folder( c_system_variables_folder );

         if( !val.empty( ) )
            system_ods_file_system( ).store_as_text_file( var_name, val );
         else if( system_ods_file_system( ).has_file( var_name ) )
            system_ods_file_system( ).remove_file( var_name );
      }
   }
}

bool set_system_variable( const string& name, const string& value,
 const string& current, progress* p_progress, const char append_separator )
{
   guard g( g_mutex );

   bool retval = false;

   if( name.find_first_of( c_invalid_name_chars ) != string::npos )
      throw runtime_error( "invalid system variable name '" + name + "'" );

   check_system_variable_can_be_set( name );

   if( name == string( c_special_variable_files_area_dir ) )
      throw runtime_error( "invalid attempt to change the files area dir" );

   string new_value( value );

   // NOTE: If an append separator is provided then variable will be
   // set to the new value if currently empty or it will be appended
   // to the current value after the separator.
   if( !g_variables.count( name ) )
   {
      if( current.empty( ) || append_separator )
         retval = true;
   }
   else if( current == g_variables[ name ] )
   {
      retval = true;

      g_variables.erase( name );
   }
   else if( append_separator )
   {
      retval = true;

      new_value = g_variables[ name ] + append_separator + value; 
   }

   if( retval )
   {
      if( !new_value.empty( ) )
         g_variables[ name ] = new_value;
      else
      {
         if( g_variables.count( name ) )
            g_variables.erase( name );
      }
   }

   return retval;
}

void rename_system_variable( const string& old_name, const string& new_name )
{
   guard g( g_mutex );

   if( old_name.empty( ) )
      throw runtime_error( "incorrect empty 'old_name' for rename_system_variable" );

   if( new_name.empty( ) )
      throw runtime_error( "incorrect empty 'new_name' for rename_system_variable" );

   check_system_variable_can_be_set( old_name );
   check_system_variable_can_be_set( new_name );

   if( g_variables.count( old_name ) )
   {
      if( g_variables.count( new_name ) )
         // FUTURE: This should be a module string.
         throw runtime_error( "System variable '" + new_name + "' is already in use." );

      string value( g_variables[ old_name ] );

      ods::bulk_write bulk_write( system_ods_instance( ) );

      scoped_ods_instance ods_instance( system_ods_instance( ) );

      system_ods_file_system( ).set_root_folder( c_system_variables_folder );

      if( system_ods_file_system( ).has_file( old_name ) )
      {
         ods::transaction ods_tx( system_ods_instance( ) );

         system_ods_file_system( ).remove_file( old_name );
         system_ods_file_system( ).store_as_text_file( new_name, value );

         ods_tx.commit( );
      }

      g_variables.erase( old_name );

      g_variables[ new_name ] = value;
   }
}

bool set_variable_checker::can_set( ) const
{
   bool retval = false;

   if( check_type == e_variable_check_type_no_session_has )
      retval = !has_any_session_variable( variable_name );
   else if( check_type == e_variable_check_type_any_session_has )
      retval = has_any_session_variable( variable_name );
   else if( check_type == e_variable_check_type_has_other_system )
      retval = has_raw_system_variable( variable_name );
   else if( check_type == e_variable_check_type_not_has_other_system )
      retval = !has_raw_system_variable( variable_name );

   if( retval && p_additional_check )
      retval = p_additional_check->can_set( );

   return retval;
}

bool set_system_variable( const string& name,
 const string& value, set_variable_checker& checker, bool is_init, progress* p_progress )
{
   guard g( g_mutex );

   if( !checker.can_set( ) )
      return false;

   if( name != c_special_variable_none )
      set_system_variable( name, value, is_init, p_progress );

   return true;
}

void system_variable_expression( const string& expr )
{
   string expression( expr );

   string::size_type pos = expression.find( ' ' );

   if( pos != string::npos )
   {
      string name( expression.substr( 0, pos ) );
      string value( expression.substr( pos + 1 ) );

      if( value == get_special_var_name( e_special_var_none ) )
         value.erase( );

      expression.erase( pos );

      set_system_variable( name, value );
   }

   if( !expression.empty( )
    && ( ( expression[ 0 ] == '<' ) || ( expression[ 0 ] == '>' ) ) )
      expression.erase( 0, 1 );

   set_session_variable( c_variable_expression, expression );
}

string variable_name_from_name_and_value( const string& name_and_value, string* p_value )
{
   string name;

   string::size_type from = 0;

   if( !name_and_value.empty( ) && name_and_value[ 0 ] == '"' )
      from = name_and_value.find( '"', 1 );

   string::size_type pos = name_and_value.find( ' ', from );

   name = name_and_value.substr( 0, pos );

   // NOTE: Variable names that contain white space
   // are quoted in order to be correctly parsed so
   // now removes the quotes if they were present.
   if( from )
   {
      name.erase( 0, 1 );
      name.erase( name.length( ) - 1 );
   }

   if( p_value && ( pos != string::npos ) )
      *p_value = name_and_value.substr( pos + 1 );

   return name;
}

mutex& get_mutex_for_ciyam_variables( )
{
   return g_mutex;
}
