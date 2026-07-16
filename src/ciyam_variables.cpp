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
#  include <atomic>
#  include <iostream>
#  include <algorithm>
#endif

#include "ciyam_variables.h"

#include "ods.h"
#include "sha256.h"
#include "threads.h"
#include "date_time.h"
#include "utilities.h"
#include "ciyam_base.h"
#include "class_base.h"
#include "ciyam_files.h"
#include "ods_file_system.h"

using namespace std;

extern atomic< size_t > g_active_sessions;

namespace
{

#include "module_strings.h"
#include "ciyam_constants.h"

const int c_max_lock_attempts = 20;
const int c_lock_attempt_sleep_time = 100;

const size_t c_secret_truncate_length = 9;

const size_t c_default_max_deque_item_size = 1000;

const size_t c_default_max_deque_size_limit = 10000;

constexpr const char* c_double_asterisk = "**";

constexpr const char* c_qs_max_chars_prefix = "@qs_mc_";
constexpr const char* c_qs_num_items_prefix = "@qs_ni_";
constexpr const char* c_qs_pop_front_prefix = "@qs_pf_";

constexpr const char* c_secret_hash_suffix = "\t\1\t";

constexpr const char* c_special_variable_id = "@id";
constexpr const char* c_special_variable_os = "@os";
constexpr const char* c_special_variable_cmd = "@cmd";
constexpr const char* c_special_variable_dtm = "@dtm";
constexpr const char* c_special_variable_get = "@get";
constexpr const char* c_special_variable_grp = "@grp";
constexpr const char* c_special_variable_hub = "@hub";
constexpr const char* c_special_variable_key = "@key";
constexpr const char* c_special_variable_sec = "@sec";
constexpr const char* c_special_variable_set = "@set";
constexpr const char* c_special_variable_tag = "@tag";
constexpr const char* c_special_variable_tls = "@tls";
constexpr const char* c_special_variable_uid = "@uid";
constexpr const char* c_special_variable_web = "@web";
constexpr const char* c_special_variable_sid = "@sid";
constexpr const char* c_special_variable_arg1 = "@arg1";
constexpr const char* c_special_variable_arg2 = "@arg2";
constexpr const char* c_special_variable_arg3 = "@arg3";
constexpr const char* c_special_variable_auto = "@auto";
constexpr const char* c_special_variable_cube = "@cube";
constexpr const char* c_special_variable_gids = "@gids";
constexpr const char* c_special_variable_hash = "@hash";
constexpr const char* c_special_variable_null = "@null";
constexpr const char* c_special_variable_val1 = "@val1";
constexpr const char* c_special_variable_val2 = "@val2";
constexpr const char* c_special_variable_file = "@file";
constexpr const char* c_special_variable_loop = "@loop";
constexpr const char* c_special_variable_name = "@name";
constexpr const char* c_special_variable_none = "@none";
constexpr const char* c_special_variable_path = "@path";
constexpr const char* c_special_variable_peer = "@peer";
constexpr const char* c_special_variable_port = "@port";
constexpr const char* c_special_variable_size = "@size";
constexpr const char* c_special_variable_slot = "@slot";
constexpr const char* c_special_variable_type = "@type";
constexpr const char* c_special_variable_user = "@user";
constexpr const char* c_special_variable_uuid = "@uuid";
constexpr const char* c_special_variable_algos = "@algos";
constexpr const char* c_special_variable_array = "@array";
constexpr const char* c_special_variable_async = "@async";
constexpr const char* c_special_variable_bytes = "@bytes";
constexpr const char* c_special_variable_class = "@class";
constexpr const char* c_special_variable_deque = "@deque";
constexpr const char* c_special_variable_embed = "@embed";
constexpr const char* c_special_variable_files = "@files";
constexpr const char* c_special_variable_print = "@print";
constexpr const char* c_special_variable_quiet = "@quiet";
constexpr const char* c_special_variable_slotx = "@slotx";
constexpr const char* c_special_variable_title = "@title";
constexpr const char* c_special_variable_branch = "@branch";
constexpr const char* c_special_variable_cloned = "@cloned";
constexpr const char* c_special_variable_images = "@images";
constexpr const char* c_special_variable_module = "@module";
constexpr const char* c_special_variable_opened = "@opened";
constexpr const char* c_special_variable_pubkey = "@pubkey";
constexpr const char* c_special_variable_return = "@return";
constexpr const char* c_special_variable_script = "@script";
constexpr const char* c_special_variable_source = "@source";
constexpr const char* c_special_variable_system = "@system";
constexpr const char* c_special_variable_do_exec = "@do_exec";
constexpr const char* c_special_variable_ip_addr = "@ip_addr";
constexpr const char* c_special_variable_is_last = "@is_last";
constexpr const char* c_special_variable_message = "@message";
constexpr const char* c_special_variable_opening = "@opening";
constexpr const char* c_special_variable_package = "@package";
constexpr const char* c_special_variable_pending = "@pending";
constexpr const char* c_special_variable_pubkeyx = "@pubkeyx";
constexpr const char* c_special_variable_restore = "@restore";
constexpr const char* c_special_variable_session = "@session";
constexpr const char* c_special_variable_slowest = "@slowest";
constexpr const char* c_special_variable_storage = "@storage";
constexpr const char* c_special_variable_timeout = "@timeout";
constexpr const char* c_special_variable_tz_name = "@tz_name";
constexpr const char* c_special_variable_trigger = "@trigger";
constexpr const char* c_special_variable_updates = "@updates";
constexpr const char* c_special_variable_version = "@version";
constexpr const char* c_special_variable_waiting = "@waiting";
constexpr const char* c_special_variable_executed = "@executed";
constexpr const char* c_special_variable_identity = "@identity";
constexpr const char* c_special_variable_key_info = "@key_info";
constexpr const char* c_special_variable_notifier = "@notifier";
constexpr const char* c_special_variable_peer_hub = "@peer_hub";
constexpr const char* c_special_variable_progress = "@progress";
constexpr const char* c_special_variable_timezone = "@timezone";
constexpr const char* c_special_variable_unlocked = "@unlocked";
constexpr const char* c_special_variable_updating = "@updating";
constexpr const char* c_special_variable_args_file = "@args_file";
constexpr const char* c_special_variable_cmd_delay = "@cmd_delay";
constexpr const char* c_special_variable_crypt_key = "@crypt_key";
constexpr const char* c_special_variable_cws_token = "@cws_token";
constexpr const char* c_special_variable_datachain = "@datachain";
constexpr const char* c_special_variable_decrement = "@decrement";
constexpr const char* c_special_variable_executing = "@executing";
constexpr const char* c_special_variable_file_list = "@file_list";
constexpr const char* c_special_variable_image_dir = "@image_dir";
constexpr const char* c_special_variable_increment = "@increment";
constexpr const char* c_special_variable_list_hash = "@list_hash";
constexpr const char* c_special_variable_peer_data = "@peer_data";
constexpr const char* c_special_variable_peer_port = "@peer_port";
constexpr const char* c_special_variable_peer_user = "@peer_user";
constexpr const char* c_special_variable_preparing = "@preparing";
constexpr const char* c_special_variable_repo_lock = "@repo_lock";
constexpr const char* c_special_variable_uid_check = "@uid_check";
constexpr const char* c_special_variable_val_error = "@val_error";
constexpr const char* c_special_variable_blockchain = "@blockchain";
constexpr const char* c_special_variable_chain_type = "@chain_type";
constexpr const char* c_special_variable_cws_styles = "@cws_styles";
constexpr const char* c_special_variable_extra_info = "@extra_info";
constexpr const char* c_special_variable_key_suffix = "@key_suffix";
constexpr const char* c_special_variable_permission = "@permission";
constexpr const char* c_special_variable_queue_puts = "@queue_puts";
constexpr const char* c_special_variable_session_id = "@session_id";
constexpr const char* c_special_variable_sid_locked = "@sid_locked";
constexpr const char* c_special_variable_sid_secure = "@sid_secure";
constexpr const char* c_special_variable_tag_prefix = "@tag_prefix";
constexpr const char* c_special_variable_tls_cipher = "@tls_cipher";
constexpr const char* c_special_variable_tree_count = "@tree_count";
constexpr const char* c_special_variable_tree_match = "@tree_match";
constexpr const char* c_special_variable_tree_total = "@tree_total";
constexpr const char* c_special_variable_allow_async = "@allow_async";
constexpr const char* c_special_variable_application = "@application";
constexpr const char* c_special_variable_auto_update = "@auto_update";
constexpr const char* c_special_variable_errors_only = "@errors_only";
constexpr const char* c_special_variable_init_log_id = "@init_log_id";
constexpr const char* c_special_variable_ip_ext_addr = "@ip_ext_addr";
constexpr const char* c_special_variable_log_restore = "@log_restore";
constexpr const char* c_special_variable_opened_user = "@opened_user";
constexpr const char* c_special_variable_output_file = "@output_file";
constexpr const char* c_special_variable_paired_sync = "@paired_sync";
constexpr const char* c_special_variable_path_prefix = "@path_prefix";
constexpr const char* c_special_variable_peer_msleep = "@peer_msleep";
constexpr const char* c_special_variable_permissions = "@permissions";
constexpr const char* c_special_variable_queue_peers = "@queue_peers";
constexpr const char* c_special_variable_secret_hash = "@secret_hash";
constexpr const char* c_special_variable_skip_update = "@skip_update";
constexpr const char* c_special_variable_sleep_after = "@sleep_after";
constexpr const char* c_special_variable_state_names = "@state_names";
constexpr const char* c_special_variable_style_brief = "@style_brief";
constexpr const char* c_special_variable_submit_type = "@submit_type";
constexpr const char* c_special_variable_transaction = "@transaction";
constexpr const char* c_special_variable_backup_files = "@backup_files";
constexpr const char* c_special_variable_block_height = "@block_height";
constexpr const char* c_special_variable_cmd_and_args = "@cmd_and_args";
constexpr const char* c_special_variable_dump_minimal = "@dump_minimal";
constexpr const char* c_special_variable_local_folder = "@local_folder";
constexpr const char* c_special_variable_local_origin = "@local_origin";
constexpr const char* c_special_variable_local_prefix = "@local_prefix";
constexpr const char* c_special_variable_local_starts = "@local_starts";
constexpr const char* c_special_variable_local_suffix = "@local_suffix";
constexpr const char* c_special_variable_opened_files = "@opened_files";
constexpr const char* c_special_variable_peer_map_key = "@peer_map_key";
constexpr const char* c_special_variable_shared_files = "@shared_files";
constexpr const char* c_special_variable_utm_at_start = "@utm_at_start";
constexpr const char* c_special_variable_app_directory = "@app_directory";
constexpr const char* c_special_variable_backup_needed = "@backup_needed";
constexpr const char* c_special_variable_export_needed = "@export_needed";
constexpr const char* c_special_variable_import_needed = "@import_needed";
constexpr const char* c_special_variable_last_file_put = "@last_file_put";
constexpr const char* c_special_variable_no_auto_peers = "@no_auto_peers";
constexpr const char* c_special_variable_num_put_files = "@num_put_files";
constexpr const char* c_special_variable_peer_sec_hash = "@peer_sec_hash";
constexpr const char* c_special_variable_prior_storage = "@prior_storage";
constexpr const char* c_special_variable_progress_pipe = "@progress_pipe";
constexpr const char* c_special_variable_rewind_height = "@rewind_height";
constexpr const char* c_special_variable_shared_secret = "@shared_secret";
constexpr const char* c_special_variable_skip_set_vars = "@skip_set_vars";
constexpr const char* c_special_variable_stream_cipher = "@stream_cipher";
constexpr const char* c_special_variable_sub_directory = "@sub_directory";
constexpr const char* c_special_variable_trace_filters = "@trace_filters";
constexpr const char* c_special_variable_update_fields = "@update_fields";
constexpr const char* c_special_variable_utm_init_base = "@utm_init_base";
constexpr const char* c_special_variable_cmd_delay_wait = "@cmd_delay_wait";
constexpr const char* c_special_variable_force_internal = "@force_internal";
constexpr const char* c_special_variable_log_files_path = "@log_files_path";
constexpr const char* c_special_variable_peer_clone_key = "@peer_clone_key";
constexpr const char* c_special_variable_peer_initiator = "@peer_initiator";
constexpr const char* c_special_variable_peer_responder = "@peer_responder";
constexpr const char* c_special_variable_prepare_system = "@prepare_system";
constexpr const char* c_special_variable_progress_clear = "@progress_clear";
constexpr const char* c_special_variable_progress_count = "@progress_count";
constexpr const char* c_special_variable_progress_fracs = "@progress_fracs";
constexpr const char* c_special_variable_progress_prior = "@progress_prior";
constexpr const char* c_special_variable_progress_total = "@progress_total";
constexpr const char* c_special_variable_progress_value = "@progress_value";
constexpr const char* c_special_variable_restore_needed = "@restore_needed";
constexpr const char* c_special_variable_restore_system = "@restore_system";
constexpr const char* c_special_variable_stop_condemned = "@stop_condemned";
constexpr const char* c_special_variable_sys_var_prefix = "@sys_var_prefix";
constexpr const char* c_special_variable_blockchain_data = "@blockchain_data";
constexpr const char* c_special_variable_blockchain_user = "@blockchain_user";
constexpr const char* c_special_variable_ciyam_list_hash = "@ciyam_list_hash";
constexpr const char* c_special_variable_cmd_delay_after = "@cmd_delay_after";
constexpr const char* c_special_variable_files_area_path = "@files_area_path";
constexpr const char* c_special_variable_force_db_create = "@force_db_create";
constexpr const char* c_special_variable_log_all_scripts = "@log_all_scripts";
constexpr const char* c_special_variable_notifier_events = "@notifier_events";
constexpr const char* c_special_variable_paired_identity = "@paired_identity";
constexpr const char* c_special_variable_queue_hub_users = "@queue_hub_users";
constexpr const char* c_special_variable_row_cache_limit = "@row_cache_limit";
constexpr const char* c_special_variable_script_executed = "@script_executed";
constexpr const char* c_special_variable_ssl_cert_issuer = "@ssl_cert_issuer";
constexpr const char* c_special_variable_system_identity = "@system_identity";
constexpr const char* c_special_variable_total_put_files = "@total_put_files";
constexpr const char* c_special_variable_can_omit_prepare = "@can_omit_prepare";
constexpr const char* c_special_variable_check_if_changed = "@check_if_changed";
constexpr const char* c_special_variable_cws_username_for = "@cws_username_for";
constexpr const char* c_special_variable_dummy_time_stamp = "@dummy_time_stamp";
constexpr const char* c_special_variable_preparing_backup = "@preparing_backup";
constexpr const char* c_special_variable_progress_message = "@progress_message";
constexpr const char* c_special_variable_progress_seconds = "@progress_seconds";
constexpr const char* c_special_variable_protocol_handler = "@protocol_handler";
constexpr const char* c_special_variable_rpc_script_group = "@rpc_script_group";
constexpr const char* c_special_variable_session_id_owner = "@session_id_owner";
constexpr const char* c_special_variable_skip_after_fetch = "@skip_after_fetch";
constexpr const char* c_special_variable_skip_persistence = "@skip_persistence";
constexpr const char* c_special_variable_skip_submit_file = "@skip_submit_file";
constexpr const char* c_special_variable_ssl_cert_subject = "@ssl_cert_subject";
constexpr const char* c_special_variable_style_full_blown = "@style_full_blown";
constexpr const char* c_special_variable_trigger_variable = "@trigger_variable";
constexpr const char* c_special_variable_autoscript_reload = "@autoscript_reload";
constexpr const char* c_special_variable_blockchain_height = "@blockchain_height";
constexpr const char* c_special_variable_blockchain_is_hub = "@blockchain_is_hub";
constexpr const char* c_special_variable_fields_and_values = "@fields_and_values";
constexpr const char* c_special_variable_generate_log_file = "@generate_log_file";
constexpr const char* c_special_variable_last_suffixed_key = "@last_suffixed_key";
constexpr const char* c_special_variable_package_type_path = "@package_type_path";
constexpr const char* c_special_variable_peer_data_created = "@peer_data_created";
constexpr const char* c_special_variable_peer_is_dependent = "@peer_is_dependent";
constexpr const char* c_special_variable_preparing_restore = "@preparing_restore";
constexpr const char* c_special_variable_trace_session_ids = "@trace_session_ids";
constexpr const char* c_special_variable_web_session_check = "@web_session_check";
constexpr const char* c_special_variable_attached_file_path = "@attached_file_path";
constexpr const char* c_special_variable_check_script_error = "@check_script_error";
constexpr const char* c_special_variable_encrypted_password = "@encrypted_password";
constexpr const char* c_special_variable_exec_system_append = "@exec_system_append";
constexpr const char* c_special_variable_extra_field_values = "@extra_field_values";
constexpr const char* c_special_variable_file_info_buffered = "@file_info_buffered";
constexpr const char* c_special_variable_fixed_field_values = "@fixed_field_values";
constexpr const char* c_special_variable_generate_hub_block = "@generate_hub_block";
constexpr const char* c_special_variable_gteq_session_queue = "@gteq_session_queue";
constexpr const char* c_special_variable_repo_entry_missing = "@repo_entry_missing";
constexpr const char* c_special_variable_system_is_for_demo = "@system_is_for_demo";
constexpr const char* c_special_variable_system_is_for_devt = "@system_is_for_devt";
constexpr const char* c_special_variable_blockchain_data_key = "@blockchain_data_key";
constexpr const char* c_special_variable_blockchain_identity = "@blockchain_identity";
constexpr const char* c_special_variable_blockchain_is_owner = "@blockchain_is_owner";
constexpr const char* c_special_variable_blockchain_num_puts = "@blockchain_num_puts";
constexpr const char* c_special_variable_key_prefix_is_first = "@key_prefix_is_first";
constexpr const char* c_special_variable_no_support_sessions = "@no_support_sessions";
constexpr const char* c_special_variable_peer_identity_alias = "@peer_identity_alias";
constexpr const char* c_special_variable_repo_crypt_password = "@repo_crypt_password";
constexpr const char* c_special_variable_skip_parent_updates = "@skip_parent_updates";
constexpr const char* c_special_variable_supplied_expression = "@supplied_expression";
constexpr const char* c_special_variable_variable_expression = "@variable_expression";
constexpr const char* c_special_variable_blockchain_hind_hash = "@blockchain_hind_hash";
constexpr const char* c_special_variable_blockchain_unix_time = "@blockchain_unix_time";
constexpr const char* c_special_variable_disallow_connections = "@disallow_connections";
constexpr const char* c_special_variable_ods_cache_hit_ratios = "@ods_cache_hit_ratios";
constexpr const char* c_special_variable_regex_search_expense = "@regex_search_expense";
constexpr const char* c_special_variable_secondary_validation = "@secondary_validation";
constexpr const char* c_special_variable_blockchain_force_skip = "@blockchain_force_skip";
constexpr const char* c_special_variable_blockchain_next_extra = "@blockchain_next_extra";
constexpr const char* c_special_variable_blockchain_time_value = "@blockchain_time_value";
constexpr const char* c_special_variable_keep_user_peers_alive = "@keep_user_peers_alive";
constexpr const char* c_special_variable_package_install_extra = "@package_install_extra";
constexpr const char* c_special_variable_peer_is_synchronising = "@peer_is_synchronising";
constexpr const char* c_special_variable_prepare_backup_needed = "@prepare_backup_needed";
constexpr const char* c_special_variable_blockchain_check_dummy = "@blockchain_check_dummy";
constexpr const char* c_special_variable_blockchain_is_checking = "@blockchain_is_checking";
constexpr const char* c_special_variable_blockchain_is_combined = "@blockchain_is_combined";
constexpr const char* c_special_variable_blockchain_is_fetching = "@blockchain_is_fetching";
constexpr const char* c_special_variable_exporting_for_identity = "@exporting_for_identity";
constexpr const char* c_special_variable_peer_limited_arguments = "@peer_limited_arguments";
constexpr const char* c_special_variable_single_string_response = "@single_string_response";
constexpr const char* c_special_variable_skip_tagging_if_exists = "@skip_tagging_if_exists";
constexpr const char* c_special_variable_blockchain_archive_path = "@blockchain_archive_path";
constexpr const char* c_special_variable_blockchain_checked_skip = "@blockchain_checked_skip";
constexpr const char* c_special_variable_blockchain_first_mapped = "@blockchain_first_mapped";
constexpr const char* c_special_variable_blockchain_height_other = "@blockchain_height_other";
constexpr const char* c_special_variable_blockchain_op_list_hash = "@blockchain_op_list_hash";
constexpr const char* c_special_variable_complete_restore_needed = "@complete_restore_needed";
constexpr const char* c_special_variable_blockchain_backup_height = "@blockchain_backup_height";
constexpr const char* c_special_variable_blockchain_put_list_hash = "@blockchain_put_list_hash";
constexpr const char* c_special_variable_blockchain_shared_height = "@blockchain_shared_height";
constexpr const char* c_special_variable_blockchain_stream_cipher = "@blockchain_stream_cipher";
constexpr const char* c_special_variable_blockchain_zenith_height = "@blockchain_zenith_height";
constexpr const char* c_special_variable_blockchain_checked_shared = "@blockchain_checked_shared";
constexpr const char* c_special_variable_blockchain_get_tree_files = "@blockchain_get_tree_files";
constexpr const char* c_special_variable_blockchain_num_tree_items = "@blockchain_num_tree_items";
constexpr const char* c_special_variable_blockchain_other_is_owner = "@blockchain_other_is_owner";
constexpr const char* c_special_variable_blockchain_peer_supporter = "@blockchain_peer_supporter";
constexpr const char* c_special_variable_blockchain_tree_root_hash = "@blockchain_tree_root_hash";
constexpr const char* c_special_variable_blockchain_backup_identity = "@blockchain_backup_identity";
constexpr const char* c_special_variable_blockchain_block_file_hash = "@blockchain_block_file_hash";
constexpr const char* c_special_variable_blockchain_both_are_owners = "@blockchain_both_are_owners";
constexpr const char* c_special_variable_blockchain_found_secondary = "@blockchain_found_secondary";
constexpr const char* c_special_variable_blockchain_num_for_support = "@blockchain_num_for_support";
constexpr const char* c_special_variable_blockchain_peer_hub_height = "@blockchain_peer_hub_height";
constexpr const char* c_special_variable_blockchain_shared_identity = "@blockchain_shared_identity";
constexpr const char* c_special_variable_use_insecure_peer_protocol = "@use_insecure_peer_protocol";
constexpr const char* c_special_variable_blockchain_waiting_for_hub = "@blockchain_waiting_for_hub";
constexpr const char* c_special_variable_blockchain_block_processing = "@blockchain_block_processing";
constexpr const char* c_special_variable_blockchain_zenith_tree_hash = "@blockchain_zenith_tree_hash";
constexpr const char* c_special_variable_totals_exclude_hidden_blobs = "@totals_exclude_hidden_blobs";
constexpr const char* c_special_variable_blockchain_height_processing = "@blockchain_height_processing";
constexpr const char* c_special_variable_blockchain_peer_hub_identity = "@blockchain_peer_hub_identity";
constexpr const char* c_special_variable_blockchain_targeted_identity = "@blockchain_targeted_identity";
constexpr const char* c_special_variable_blockchain_non_extra_identity = "@blockchain_non_extra_identity";
constexpr const char* c_special_variable_queue_archive_info_for_resize = "@queue_archive_info_for_resize";
constexpr const char* c_special_variable_blockchain_peer_has_supporters = "@blockchain_peer_has_supporters";
constexpr const char* c_special_variable_blockchain_primary_pubkey_hash = "@blockchain_primary_pubkey_hash";
constexpr const char* c_special_variable_blockchain_signature_file_hash = "@blockchain_signature_file_hash";
constexpr const char* c_special_variable_display_last_two_for_duplicate = "@display_last_two_for_duplicate";
constexpr const char* c_special_variable_blockchain_tertiary_pubkey_hash = "@blockchain_tertiary_pubkey_hash";
constexpr const char* c_special_variable_blockchain_secondary_pubkey_hash = "@blockchain_secondary_pubkey_hash";
constexpr const char* c_special_variable_skip_total_child_field_in_parent = "@skip_total_child_field_in_parent";

trace_mutex g_mutex;

set< string > g_read_only_variables;

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

void set_system_is_for_devt( )
{
   if( file_exists( "config.info" ) )
      g_variables[ c_special_variable_system_is_for_devt ] = c_true_value;
}

void init_special_variable_names( )
{
   guard g( g_mutex );

   g_secret_hash_prefix = string( c_special_variable_secret_hash ) + "_";

   // NOTE: These must align with the enums in "ciyam_variable_names.h".
   if( g_special_variable_names.empty( ) )
   {
      g_special_variable_names.push_back( c_special_variable_id );
      g_special_variable_names.push_back( c_special_variable_os );
      g_special_variable_names.push_back( c_special_variable_cmd );
      g_special_variable_names.push_back( c_special_variable_dtm );
      g_special_variable_names.push_back( c_special_variable_get );
      g_special_variable_names.push_back( c_special_variable_grp );
      g_special_variable_names.push_back( c_special_variable_hub );
      g_special_variable_names.push_back( c_special_variable_key );
      g_special_variable_names.push_back( c_special_variable_sec );
      g_special_variable_names.push_back( c_special_variable_set );
      g_special_variable_names.push_back( c_special_variable_tag );
      g_special_variable_names.push_back( c_special_variable_tls );
      g_special_variable_names.push_back( c_special_variable_uid );
      g_special_variable_names.push_back( c_special_variable_web );
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
      g_special_variable_names.push_back( c_special_variable_source );
      g_special_variable_names.push_back( c_special_variable_system );
      g_special_variable_names.push_back( c_special_variable_do_exec );
      g_special_variable_names.push_back( c_special_variable_ip_addr );
      g_special_variable_names.push_back( c_special_variable_is_last );
      g_special_variable_names.push_back( c_special_variable_message );
      g_special_variable_names.push_back( c_special_variable_opening );
      g_special_variable_names.push_back( c_special_variable_package );
      g_special_variable_names.push_back( c_special_variable_pending );
      g_special_variable_names.push_back( c_special_variable_pubkeyx );
      g_special_variable_names.push_back( c_special_variable_restore );
      g_special_variable_names.push_back( c_special_variable_session );
      g_special_variable_names.push_back( c_special_variable_slowest );
      g_special_variable_names.push_back( c_special_variable_storage );
      g_special_variable_names.push_back( c_special_variable_timeout );
      g_special_variable_names.push_back( c_special_variable_tz_name );
      g_special_variable_names.push_back( c_special_variable_trigger );
      g_special_variable_names.push_back( c_special_variable_updates );
      g_special_variable_names.push_back( c_special_variable_version );
      g_special_variable_names.push_back( c_special_variable_waiting );
      g_special_variable_names.push_back( c_special_variable_executed );
      g_special_variable_names.push_back( c_special_variable_identity );
      g_special_variable_names.push_back( c_special_variable_key_info );
      g_special_variable_names.push_back( c_special_variable_notifier );
      g_special_variable_names.push_back( c_special_variable_peer_hub );
      g_special_variable_names.push_back( c_special_variable_progress );
      g_special_variable_names.push_back( c_special_variable_timezone );
      g_special_variable_names.push_back( c_special_variable_unlocked );
      g_special_variable_names.push_back( c_special_variable_updating );
      g_special_variable_names.push_back( c_special_variable_args_file );
      g_special_variable_names.push_back( c_special_variable_cmd_delay );
      g_special_variable_names.push_back( c_special_variable_crypt_key );
      g_special_variable_names.push_back( c_special_variable_cws_token );
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
      g_special_variable_names.push_back( c_special_variable_cws_styles );
      g_special_variable_names.push_back( c_special_variable_extra_info );
      g_special_variable_names.push_back( c_special_variable_key_suffix );
      g_special_variable_names.push_back( c_special_variable_permission );
      g_special_variable_names.push_back( c_special_variable_queue_puts );
      g_special_variable_names.push_back( c_special_variable_session_id );
      g_special_variable_names.push_back( c_special_variable_sid_locked );
      g_special_variable_names.push_back( c_special_variable_sid_secure );
      g_special_variable_names.push_back( c_special_variable_tag_prefix );
      g_special_variable_names.push_back( c_special_variable_tls_cipher );
      g_special_variable_names.push_back( c_special_variable_tree_count );
      g_special_variable_names.push_back( c_special_variable_tree_match );
      g_special_variable_names.push_back( c_special_variable_tree_total );
      g_special_variable_names.push_back( c_special_variable_allow_async );
      g_special_variable_names.push_back( c_special_variable_application );
      g_special_variable_names.push_back( c_special_variable_auto_update );
      g_special_variable_names.push_back( c_special_variable_errors_only );
      g_special_variable_names.push_back( c_special_variable_init_log_id );
      g_special_variable_names.push_back( c_special_variable_ip_ext_addr );
      g_special_variable_names.push_back( c_special_variable_log_restore );
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
      g_special_variable_names.push_back( c_special_variable_utm_at_start );
      g_special_variable_names.push_back( c_special_variable_app_directory );
      g_special_variable_names.push_back( c_special_variable_backup_needed );
      g_special_variable_names.push_back( c_special_variable_export_needed );
      g_special_variable_names.push_back( c_special_variable_import_needed );
      g_special_variable_names.push_back( c_special_variable_last_file_put );
      g_special_variable_names.push_back( c_special_variable_no_auto_peers );
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
      g_special_variable_names.push_back( c_special_variable_utm_init_base );
      g_special_variable_names.push_back( c_special_variable_cmd_delay_wait );
      g_special_variable_names.push_back( c_special_variable_force_internal );
      g_special_variable_names.push_back( c_special_variable_log_files_path );
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
      g_special_variable_names.push_back( c_special_variable_files_area_path );
      g_special_variable_names.push_back( c_special_variable_force_db_create );
      g_special_variable_names.push_back( c_special_variable_log_all_scripts );
      g_special_variable_names.push_back( c_special_variable_notifier_events );
      g_special_variable_names.push_back( c_special_variable_paired_identity );
      g_special_variable_names.push_back( c_special_variable_queue_hub_users );
      g_special_variable_names.push_back( c_special_variable_row_cache_limit );
      g_special_variable_names.push_back( c_special_variable_script_executed );
      g_special_variable_names.push_back( c_special_variable_ssl_cert_issuer );
      g_special_variable_names.push_back( c_special_variable_system_identity );
      g_special_variable_names.push_back( c_special_variable_total_put_files );
      g_special_variable_names.push_back( c_special_variable_can_omit_prepare );
      g_special_variable_names.push_back( c_special_variable_check_if_changed );
      g_special_variable_names.push_back( c_special_variable_cws_username_for );
      g_special_variable_names.push_back( c_special_variable_dummy_time_stamp );
      g_special_variable_names.push_back( c_special_variable_preparing_backup );
      g_special_variable_names.push_back( c_special_variable_progress_message );
      g_special_variable_names.push_back( c_special_variable_progress_seconds );
      g_special_variable_names.push_back( c_special_variable_protocol_handler );
      g_special_variable_names.push_back( c_special_variable_rpc_script_group );
      g_special_variable_names.push_back( c_special_variable_session_id_owner );
      g_special_variable_names.push_back( c_special_variable_skip_after_fetch );
      g_special_variable_names.push_back( c_special_variable_skip_persistence );
      g_special_variable_names.push_back( c_special_variable_skip_submit_file );
      g_special_variable_names.push_back( c_special_variable_ssl_cert_subject );
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
      g_special_variable_names.push_back( c_special_variable_web_session_check );
      g_special_variable_names.push_back( c_special_variable_attached_file_path );
      g_special_variable_names.push_back( c_special_variable_check_script_error );
      g_special_variable_names.push_back( c_special_variable_encrypted_password );
      g_special_variable_names.push_back( c_special_variable_exec_system_append );
      g_special_variable_names.push_back( c_special_variable_extra_field_values );
      g_special_variable_names.push_back( c_special_variable_file_info_buffered );
      g_special_variable_names.push_back( c_special_variable_fixed_field_values );
      g_special_variable_names.push_back( c_special_variable_generate_hub_block );
      g_special_variable_names.push_back( c_special_variable_gteq_session_queue );
      g_special_variable_names.push_back( c_special_variable_repo_entry_missing );
      g_special_variable_names.push_back( c_special_variable_system_is_for_demo );
      g_special_variable_names.push_back( c_special_variable_system_is_for_devt );
      g_special_variable_names.push_back( c_special_variable_blockchain_data_key );
      g_special_variable_names.push_back( c_special_variable_blockchain_identity );
      g_special_variable_names.push_back( c_special_variable_blockchain_is_owner );
      g_special_variable_names.push_back( c_special_variable_blockchain_num_puts );
      g_special_variable_names.push_back( c_special_variable_key_prefix_is_first );
      g_special_variable_names.push_back( c_special_variable_no_support_sessions );
      g_special_variable_names.push_back( c_special_variable_peer_identity_alias );
      g_special_variable_names.push_back( c_special_variable_repo_crypt_password );
      g_special_variable_names.push_back( c_special_variable_skip_parent_updates );
      g_special_variable_names.push_back( c_special_variable_supplied_expression );
      g_special_variable_names.push_back( c_special_variable_variable_expression );
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

   g_read_only_variables.insert( c_special_variable_os );
   g_read_only_variables.insert( c_special_variable_updates );
   g_read_only_variables.insert( c_special_variable_version );
   g_read_only_variables.insert( c_special_variable_timezone );
   g_read_only_variables.insert( c_special_variable_peer_port );
   g_read_only_variables.insert( c_special_variable_sid_locked );
   g_read_only_variables.insert( c_special_variable_sid_secure );
   g_read_only_variables.insert( c_special_variable_ip_ext_addr );
   g_read_only_variables.insert( c_special_variable_backup_files );
   g_read_only_variables.insert( c_special_variable_opened_files );
   g_read_only_variables.insert( c_special_variable_shared_files );
   g_read_only_variables.insert( c_special_variable_utm_at_start );
   g_read_only_variables.insert( c_special_variable_utm_init_base );
   g_read_only_variables.insert( c_special_variable_ssl_cert_issuer );
   g_read_only_variables.insert( c_special_variable_system_identity );
   g_read_only_variables.insert( c_special_variable_ssl_cert_subject );
   g_read_only_variables.insert( c_special_variable_system_is_for_demo );
   g_read_only_variables.insert( c_special_variable_system_is_for_devt );

   set_system_is_for_devt( );
}

void touch_or_remove( const string& variable, bool remove )
{
   string file_name( c_hidden_file_prefix + variable.substr( 1 ) );

   if( remove )
      file_remove( file_name );
   else if( !file_exists( file_name ) )
      file_touch( file_name, 0, true );
}

void set_file_variable( const string& variable )
{
   string file_name( c_hidden_file_prefix + variable.substr( 1 ) );

   if( file_exists( file_name ) )
      g_variables[ variable ] = c_true_value;
   else if( g_variables.count( variable ) )
      g_variables.erase( variable );
}

void set_backup_needed( bool check = true )
{
   string variable( c_special_variable_backup_needed );

   if( !check )
      touch_or_remove( variable, true );

   set_file_variable( variable );
}

void set_restore_needed( bool check = true )
{
   string variable( c_special_variable_restore_needed );

   if( !check )
      touch_or_remove( variable, true );

   set_file_variable( variable );
}

void set_generate_hub_block( bool check = true )
{
   string variable( c_special_variable_generate_hub_block );

   if( !check )
      touch_or_remove( variable, true );

   set_file_variable( variable );
}

void set_ods_cache_hit_ratios( )
{
   g_variables[ c_special_variable_ods_cache_hit_ratios ] = system_ods_instance( ).get_cache_hit_ratios( );
}

void set_prepare_backup_needed( bool check = true )
{
   string variable( c_special_variable_prepare_backup_needed );

   if( !check )
      touch_or_remove( variable, true );

   set_file_variable( variable );
}

void set_complete_restore_needed( bool check = true )
{
   string variable( c_special_variable_complete_restore_needed );

   if( !check )
      touch_or_remove( variable, true );

   set_file_variable( variable );
}

void check_system_variable_can_be_set( const string& variable, bool to_persist = false )
{
   bool okay = true;

   if( g_read_only_variables.count( variable ) )
      okay = false;
   else
   {
      // NOTE: The following are more nuanced checks that are
      // expected to involve checking other system variables.
      if( variable == c_special_variable_blockchain_force_skip )
      {
         if( to_persist || !g_variables.count( c_special_variable_system_is_for_devt ) )
            okay = false;
      }
   }

   if( !okay )
   {
      // FUTURE: These should both be module strings.
      if( to_persist )
         throw runtime_error( "Invalid attempt to store read-only system variable '" + variable + "'." );
      else
         throw runtime_error( "Invalid attempt to change read-only system variable '" + variable + "'." );
   }
}

void fetch_persistent( ods_file_system& ods_fs, const string& variable, string& value )
{
   ods_fs.fetch_from_text_file( variable, value );

   // NOTE: If is a "@secret_hash_<suffix>" variable
   // then the value stored was XORed using the hash
   // of the variable name (just to make it a little
   // more difficult to easily discover this value).
   if( variable.find( g_secret_hash_prefix ) == 0 )
   {
      sha256 hash( variable + c_secret_hash_suffix );

      string xor_hash( hash.get_digest_as_string( ) );

      size_t length = value.length( );

      if( length != xor_hash.length( ) )
         throw runtime_error( "unexpected length = "
          + to_string( length ) + " for variable '" + variable + "' in fetch_persistent" );

      for( size_t i = 0; i < length; i++ )
      {
         unsigned char val1 = hex_nibble( value[ i ] );
         unsigned char val2 = hex_nibble( xor_hash[ i ] );

         value[ i ] = ascii_digit( val1 ^ val2 );
      }
   }
}

void store_persistent( ods_file_system& ods_fs, const string& variable, const string& value )
{
   string final_value( value );

   // NOTE: See the note above.
   if( variable.find( g_secret_hash_prefix ) == 0 )
   {
      sha256 hash( variable + c_secret_hash_suffix );

      string xor_hash( hash.get_digest_as_string( ) );

      size_t length = final_value.length( );

      if( length != xor_hash.length( ) )
         throw runtime_error( "unexpected length = "
          + to_string( length ) + " for variable '" + variable + "' in store_persistent" );

      for( size_t i = 0; i < length; i++ )
      {
         unsigned char val1 = hex_nibble( value[ i ] );
         unsigned char val2 = hex_nibble( xor_hash[ i ] );

         final_value[ i ] = ascii_digit( val1 ^ val2 );
      }
   }

   ods_fs.store_as_text_file( variable, final_value );
}

}

string get_special_var_name( special_var var )
{
   string s;

   if( g_special_variable_names.empty( ) )
      init_special_variable_names( );

   size_t offset = ( size_t )var;

   if( ( offset == 0 ) || ( offset > g_special_variable_names.size( ) ) )
      throw runtime_error( "unexpected special var offset " + to_string( offset ) );

   s = g_special_variable_names[ offset - 1 ];

   return s;
}

system_variable_lock::system_variable_lock( const string& name )
 :
 var( name )
{
   acquire_lock( );
}

system_variable_lock::system_variable_lock( const string& name, const string& display )
 :
 var( name )
{
   acquire_lock( display.c_str( ) );
}

void system_variable_lock::acquire_lock( const char* p_display_name_str )
{
   bool acquired = false;

   for( size_t i = 0; i < c_max_lock_attempts; i++ )
   {
      // NOTE: Empty code block for scope purposes.
      {
         guard g( g_mutex );

         if( set_system_variable( var, c_true_value, string( "" ) ) )
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
         throw runtime_error( "unable to acquire lock for system variable '" + var.name + "'" );
   }
}

system_variable_lock::~system_variable_lock( )
{
   set_system_variable( var, "" );
}

bool has_system_variable( const var_name& var )
{
   guard g( g_mutex );

   bool retval = false;

   string name( var.name );

   if( name == c_special_variable_backup_needed )
      set_backup_needed( );
   else if( name == c_special_variable_restore_needed )
      set_restore_needed( );
   else if( name == c_special_variable_generate_hub_block )
      set_generate_hub_block( );
   else if( name == c_special_variable_prepare_backup_needed )
      set_prepare_backup_needed( );
   else if( name == c_special_variable_complete_restore_needed )
      set_complete_restore_needed( );

   if( g_variables.count( name ) )
      retval = true;
   else if( name.find( c_special_variable_queue_prefix ) == 0 )
      retval = g_deque_variables.count( name );

   return retval;
}

string get_system_variable( const var_name& var, bool is_internal )
{
   guard g( g_mutex );

   string name( var.name );

   string retval;

   string variable( name );

   bool skip_name_output = false;

   if( variable.find( c_double_asterisk ) != string::npos )
   {
      skip_name_output = true;

      replace( variable, c_double_asterisk, "*" );
   }

   bool had_persist_prefix = false;
   bool had_restore_prefix = false;

   if( !variable.empty( ) )
   {
      if( variable[ 0 ] == c_persist_variable_prefix )
         had_persist_prefix = true;
      else if( variable[ 0 ] == c_restore_variable_prefix )
         had_restore_prefix = true;

      if( had_persist_prefix || had_restore_prefix )
         variable.erase( 0, 1 );
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

      if( variable.empty( ) && had_persist_prefix )
         output_all_persistent_variables = true;

      unique_ptr< ods::bulk_base > up_bulk_base;

      if( !system_ods_instance( ).is_thread_bulk_locked( ) )
      {
         if( had_restore_prefix )
            up_bulk_base.reset( new ods::bulk_read( system_ods_instance( ) ) );
         else
            up_bulk_base.reset( new ods::bulk_write( system_ods_instance( ) ) );
      }

      if( had_persist_prefix && !up_bulk_base.get( )
       && system_ods_instance( ).is_thread_bulk_read_locked( ) )
         throw runtime_error( "attempt to store system variable '" + variable + "' whilst bulk read locked" );

      scoped_ods_instance ods_instance( system_ods_instance( ) );

      ods_file_system& ods_fs( system_ods_file_system( ) );

      ods_fs.set_root_folder( c_system_variables_folder );

      if( !variable.empty( ) && had_persist_prefix
       && variable.find_first_of( "?*" ) == string::npos )
      {
         check_system_variable_can_be_set( variable, true );

         string value;

         if( g_variables.count( variable ) )
            value = g_variables[ variable ];

         if( value.empty( ) )
         {
            if( ods_fs.has_file( variable ) )
               ods_fs.remove_file( variable );
         }
         else
            store_persistent( ods_fs, variable, value );
      }
      else
      {
         vector< string > variable_files;

         string expr( sys_var_prefix );

         if( variable.empty( ) || ( variable == "*" ) )
            expr += "*";
         else
            expr = variable;

         ods_fs.list_files( expr, variable_files );

         for( size_t i = 0; i < variable_files.size( ); i++ )
         {
            string next( variable_files[ i ] );

            string value;

            if( had_restore_prefix || output_all_persistent_variables )
            {
               fetch_persistent( ods_fs, next, value );

               if( !variable.empty( ) )
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

                     if( skip_name_output )
                        retval += value;
                     else
                        retval += quote_if_contains_white_space( next ) + ' ' + value;
                  }
               }
            }
            else
            {
               check_system_variable_can_be_set( next, true );

               if( g_variables.count( next ) )
                  value = g_variables[ next ];

               if( value.empty( ) )
                  ods_fs.remove_file( next );
               else
                  store_persistent( ods_fs, next, value );
            }
         }
      }
   }
   else if( variable.find_first_of( "?*" ) != string::npos )
   {
      if( variable == "*" )
         variable = sys_var_prefix + variable;

      map< string, string >::const_iterator ci;

      if( wildcard_match( variable, c_special_variable_backup_needed ) )
         set_backup_needed( );

      if( wildcard_match( variable, c_special_variable_trace_filters ) )
      {
         string trace_filters( get_trace_filters( ) );

         if( trace_filters.empty( ) )
            g_variables.erase( c_special_variable_trace_filters );
         else
            g_variables[ c_special_variable_trace_filters ] = trace_filters;
      }

      if( wildcard_match( variable, c_special_variable_restore_needed ) )
         set_restore_needed( );

      if( wildcard_match( variable, c_special_variable_generate_hub_block ) )
         set_generate_hub_block( );

      if( wildcard_match( variable, c_special_variable_trace_session_ids ) )
      {
         string trace_session_ids( get_trace_session_ids( ) );

         if( trace_session_ids.empty( ) )
            g_variables.erase( c_special_variable_trace_session_ids );
         else
            g_variables[ c_special_variable_trace_session_ids ] = trace_session_ids;
      }

      if( wildcard_match( variable, c_special_variable_ods_cache_hit_ratios ) )
         set_ods_cache_hit_ratios( );

      if( wildcard_match( variable, c_special_variable_prepare_backup_needed ) )
         set_prepare_backup_needed( );

      if( wildcard_match( variable, c_special_variable_complete_restore_needed ) )
         set_complete_restore_needed( );

      for( ci = g_variables.begin( ); ci != g_variables.end( ); ++ci )
      {
         if( wildcard_match( variable, ci->first ) )
         {
            if( !retval.empty( ) )
               retval += "\n";

            string next( ci->first );
            string value( ci->second );

            if( !is_internal )
               truncate_value_for_secret_hash_prefixed_name( next, value );

            if( skip_name_output )
               retval += value;
            else
               retval += quote_if_contains_white_space( next ) + ' ' + value;
         }
      }

      map< string, deque< string > >::const_iterator dci;

      for( dci = g_deque_variables.begin( ); dci != g_deque_variables.end( ); ++dci )
      {
         if( wildcard_match( variable, dci->first ) )
         {
            if( !retval.empty( ) )
               retval += "\n";

            if( skip_name_output )
               retval += dci->second.front( );
            else
               retval += quote_if_contains_white_space( dci->first ) + ' ' + dci->second.front( );

            retval += " [+" + to_string( dci->second.size( ) - 1 ) + "]";
         }
      }
   }
   else
   {
      if( variable == c_special_variable_backup_needed )
         set_backup_needed( );

      if( variable == c_special_variable_trace_filters )
      {
         string trace_filters( get_trace_filters( ) );

         if( trace_filters.empty( ) )
            g_variables.erase( c_special_variable_trace_filters );
         else
            g_variables[ c_special_variable_trace_filters ] = trace_filters;
      }

      if( variable == c_special_variable_restore_needed )
         set_restore_needed( );

      if( variable == c_special_variable_generate_hub_block )
         set_generate_hub_block( );

      if( variable == c_special_variable_trace_session_ids )
      {
         string trace_session_ids( get_trace_session_ids( ) );

         if( trace_session_ids.empty( ) )
            g_variables.erase( c_special_variable_trace_session_ids );
         else
            g_variables[ c_special_variable_trace_session_ids ] = trace_session_ids;
      }

      if( variable == c_special_variable_ods_cache_hit_ratios )
         set_ods_cache_hit_ratios( );

      if( variable == c_special_variable_prepare_backup_needed )
         set_prepare_backup_needed( );

      if( variable == c_special_variable_complete_restore_needed )
         set_complete_restore_needed( );

      if( variable.find( c_special_variable_queue_prefix ) == 0 )
      {
         if( g_deque_variables.count( variable ) )
         {
            if( g_deque_variables[ variable ].size( ) )
            {
               retval = g_deque_variables[ variable ].front( );

               g_deque_variables[ variable ].pop_front( );
            }

            if( !g_deque_variables[ variable ].size( ) )
               g_deque_variables.erase( variable );
         }
      }
      else if( g_variables.count( variable ) )
      {
         retval = g_variables[ variable ];

         // NOTE: After fetching "@command" will erase its value (as this is
         // not intended to be visible like other standard system variables).
         if( variable == c_special_variable_cmd )
            g_variables.erase( c_special_variable_cmd );
         else if( !is_internal )
            truncate_value_for_secret_hash_prefixed_name( variable, retval );
      }
      else if( name == c_special_variable_none )
         retval = " ";
      else if( variable == c_special_variable_files_area_path )
         retval = c_ciyam_files_directory;
   }

   return retval;
}

struct system_variable_getter : variable_getter
{
   system_variable_getter( bool is_internal ) : is_internal( is_internal ) { }

   string get_value( const string& name ) const { return get_system_variable( name, is_internal ); }

   bool is_internal;
};

string get_system_variable( const expression& expr, bool is_internal )
{
   system_variable_getter getter( is_internal );

   variable_expression var_expr( expr.expr, getter );

   return var_expr.get_value( );
}

void set_system_variable( const var_name& var,
 const string& value, bool is_init, progress* p_progress )
{
   string name( var.name );

   // NOTE: The special variable "@cmd" is used to run and hold the results of a "ciyam_command".
   if( name == c_special_variable_cmd )
   {
      string val( value );

      string tmp_file_name( "~" + uuid( ).as_string( ) );

      if( !val.empty( ) )
      {
         string cmd( "./ciyam_command " + val + " >" + tmp_file_name );

         int rc = system( cmd.c_str( ) );
         ( void )rc;

         val = buffer_file( tmp_file_name );

         if( val.length( ) && ( val[ val.length( ) - 1 ] == '\n' ) )
            val.erase( val.length( ) - 1 );
      }

      // NOTE: Issue the system
      // before thread locking.
      guard g( g_mutex );

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

      if( !name.empty( ) && ( name[ 0 ] == c_persist_variable_prefix ) )
         persist = true;

      if( !name.empty( ) )
      {
         string::size_type from = 0;

         if( persist )
            ++from;

         if( name.find_first_of( c_invalid_name_chars, from ) != string::npos )
            throw runtime_error( "invalid system variable name '" + name.substr( from ) + "'" );
      }

      string variable( !persist ? name : name.substr( 1 ) );

      if( val == string( c_special_variable_increment )
       || val == string( c_special_variable_decrement ) )
      {
         int num_value = !g_variables.count( variable )
          ? 0 : from_string< int >( g_variables[ variable ] );

         if( val == string( c_special_variable_increment ) )
            ++num_value;
         else if( num_value > 0 )
            --num_value;

         if( num_value == 0 )
            val.clear( );
         else
            val = to_string( num_value );
      }

      string::size_type pos = variable.find_first_of( "?*" );

      if( variable.find( c_special_variable_queue_prefix ) == 0 )
      {
         if( persist )
            throw runtime_error( "cannot persist '"
             + string( c_special_variable_queue_prefix ) + "' prefixed variables" );

         if( val.empty( ) )
            g_deque_variables.erase( variable );
         else
         {
            string suffix( variable.substr( CONST_LENGTH( c_special_variable_queue_prefix ) ) );

            size_t max_item_size = c_default_max_deque_item_size;

            string max_item_size_var_name( c_qs_max_chars_prefix + suffix );

            if( g_variables.count( max_item_size_var_name ) )
               max_item_size = from_string< size_t >( g_variables[ max_item_size_var_name ] );

            if( max_item_size && ( val.size( ) > max_item_size ) )
               // FUTURE: This should be a module string.
               throw runtime_error( "Maximum size for '" + suffix + "' items may not be exceeded." );

            size_t max_items = c_default_max_deque_size_limit;

            string max_items_var_name( c_qs_num_items_prefix + suffix );

            if( g_variables.count( max_items_var_name ) )
               max_items = from_string< size_t >( g_variables[ max_items_var_name ] );

            // NOTE: If there is a maximum number of items then if the
            // "@qs_pf_<name>" variable has been set (and has the max.
            // number of items currently) will pop the front item from
            // the deque before pushing back the new item.
            if( max_items )
            {
               if( g_variables.count( c_qs_pop_front_prefix + suffix ) )
               {
                  if( g_deque_variables.count( variable )
                   && ( g_deque_variables[ variable ].size( ) == max_items ) )
                     g_deque_variables[ variable ].pop_front( );
               }
            }

            if( max_items && ( g_deque_variables[ variable ].size( ) >= max_items ) )
               // FUTURE: This should be a module string.
               throw runtime_error( "Maximum number of items for '" + suffix + "' are already queued." );

            g_deque_variables[ variable ].push_back( val );

            // NOTE: If a system variable "@session_<variable>" is found then will also add
            // the item to sessions where a "@system_<variable>" session variable is found.
            if( g_variables.count( string( c_special_variable_session ) + '_' + variable ) )
               add_queue_item_for_linked_sessions( variable, val, max_items );
         }
      }
      else if( persist
       && ( ( variable == c_special_variable_args_file )
       || ( variable == c_special_variable_trace_filters )
       || ( variable == c_special_variable_backup_needed )
       || ( variable == c_special_variable_log_files_path )
       || ( variable == c_special_variable_restore_needed )
       || ( variable == c_special_variable_files_area_path )
       || ( variable == c_special_variable_trace_session_ids )
       || ( variable == c_special_variable_generate_hub_block )
       || ( variable == c_special_variable_disallow_connections )
       || ( variable == c_special_variable_ods_cache_hit_ratios )
       || ( variable == c_special_variable_prepare_backup_needed )
       || ( variable == c_special_variable_complete_restore_needed ) ) )
         // FUTURE: This should be a module string.
         throw runtime_error( "Invalid attempt to store dynamic system variable '" + variable + "'." );
      else if( pos != string::npos )
      {
         if( persist )
            throw runtime_error( "cannot persist wildcard variables for '" + variable + "'" );

         vector< string > vars_to_change;

         string prefix( variable.substr( 0, pos ) );

         map< string, string >::iterator vi = g_variables.begin( );

         if( !prefix.empty( ) )
            vi = g_variables.lower_bound( prefix );

         for( ; vi != g_variables.end( ); ++vi )
         {
            if( wildcard_match( variable, vi->first ) )
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
         if( val.empty( ) && ( variable == string( c_special_variable_files_area_path ) ) )
            val = string( c_ciyam_files_directory );

         if( !is_init )
            check_system_variable_can_be_set( variable );

         if( !val.empty( ) )
            g_variables[ variable ] = val;
         else
         {
            if( g_variables.count( variable ) )
               g_variables.erase( variable );
         }
      }

      if( variable == string( c_special_variable_log_files_path ) )
      {
         if( !is_init )
            set_log_files_path( val );
      }
      else if( variable == string( c_special_variable_files_area_path ) )
      {
         if( !is_init )
         {
            if( g_active_sessions > 1 )
               throw runtime_error( "invalid switch files area whilst has other active sessions (including autostarted)" );

            // NOTE: Although already thread locked adds a mutex name here
            // just in case a deadlock could arise during the resync calls.
            guard g( g_mutex, "set_system_variable" );

            string from( get_files_area_path( ) );

            set_files_area_path( val );

            resync_files_area( p_progress );
            resync_system_ods( p_progress );

            resync_archive_info( p_progress );

            TRACE_LOG( TRACE_MINIMAL, "*** switched files area over from '" + from + "' to '" + val + "' ***" );
         }
      }
      else if( variable == string( c_special_variable_ods_cache_hit_ratios ) )
         system_ods_instance( ).clear_cache_statistics( );

      if( persist )
      {
         check_system_variable_can_be_set( variable, true );

         ods::bulk_write bulk_write( system_ods_instance( ) );

         scoped_ods_instance ods_instance( system_ods_instance( ) );

         ods_file_system& ods_fs( system_ods_file_system( ) );

         ods_fs.set_root_folder( c_system_variables_folder );

         if( !val.empty( ) )
            store_persistent( ods_fs, variable, val );
         else if( ods_fs.has_file( variable ) )
            ods_fs.remove_file( variable );
      }
   }
}

bool set_system_variable( const var_name& var, const string& value,
 const string& current, progress* p_progress, const char append_separator )
{
   guard g( g_mutex );

   bool retval = false;

   string name( var.name );

   if( name.find_first_of( c_invalid_name_chars ) != string::npos )
      throw runtime_error( "invalid system variable name '" + name + "'" );

   check_system_variable_can_be_set( name );

   if( name == string( c_special_variable_files_area_path ) )
      throw runtime_error( "invalid attempt to change the files area path" );

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

void rename_system_variable( const var_name& old_var, const var_name& new_var )
{
   string old_name( old_var.name );
   string new_name( new_var.name );

   if( old_name.empty( ) )
      throw runtime_error( "incorrect empty 'old_name' for rename_system_variable" );

   if( new_name.empty( ) )
      throw runtime_error( "incorrect empty 'new_name' for rename_system_variable" );

   guard g( g_mutex );

   check_system_variable_can_be_set( old_name, true );
   check_system_variable_can_be_set( new_name, true );

   if( g_variables.count( old_name ) )
   {
      if( g_variables.count( new_name ) )
         // FUTURE: This should be a module string.
         throw runtime_error( "System variable '" + new_name + "' is already in use." );

      string value( g_variables[ old_name ] );

      ods::bulk_write bulk_write( system_ods_instance( ) );

      scoped_ods_instance ods_instance( system_ods_instance( ) );

      ods_file_system& ods_fs( system_ods_file_system( ) );

      ods_fs.set_root_folder( c_system_variables_folder );

      if( ods_fs.has_file( old_name ) )
      {
         ods::transaction ods_tx( system_ods_instance( ) );

         ods_fs.remove_file( old_name );

         store_persistent( ods_fs, new_name, value );

         ods_tx.commit( );
      }

      g_variables.erase( old_name );

      g_variables[ new_name ] = value;
   }
}

void copy_queue_system_variable( const string& var, deque< string >& dest_queue )
{
   if( var.find( c_special_variable_queue_prefix ) != 0 )
      throw runtime_error( "invalid queue name '" + var + "' for copy_queue_system_variable" );

   guard g( g_mutex );

   if( g_deque_variables.count( var ) )
      copy( g_deque_variables[ var ].begin( ), g_deque_variables[ var ].end( ), back_inserter( dest_queue ) );
}


bool set_variable_checker::can_set( ) const
{
   bool retval = false;

   if( check_type == e_variable_check_type_no_session_has )
      retval = !has_any_session_variable( var.name );
   else if( check_type == e_variable_check_type_any_session_has )
      retval = has_any_session_variable( var.name );
   else if( check_type == e_variable_check_type_has_other_system )
      retval = has_system_variable( var.name );
   else if( check_type == e_variable_check_type_not_has_other_system )
      retval = !has_system_variable( var.name );

   if( retval && p_additional_check )
      retval = p_additional_check->can_set( );

   return retval;
}

bool set_system_variable( const var_name& var, const string& value,
 set_variable_checker& checker, bool is_init, progress* p_progress )
{
   guard g( g_mutex );

   if( !checker.can_set( ) )
      return false;

   if( var.name != c_special_variable_none )
      set_system_variable( var, value, is_init, p_progress );

   return true;
}

struct temporary_system_variable::impl
{
   impl( const var_name& var )
    :
    var( var )
   {
      original_value = get_system_variable( var );
   }

   impl( const var_name& var, const string& value )
    :
    var( var )
   {
      guard g( g_mutex );

      original_value = get_system_variable( var );

      set_system_variable( var, value );
   }

   ~impl( )
   {
      set_system_variable( var, original_value );
   }

   var_name var;

   string original_value;
};

temporary_system_variable::temporary_system_variable( const var_name& var )
{
   p_impl = new impl( var );
}

temporary_system_variable::temporary_system_variable( const var_name& var, const string& value )
{
   p_impl = new impl( var, value );
}

temporary_system_variable::~temporary_system_variable( )
{
   delete p_impl;
}

void system_variable_expression( const string& expr )
{
   string expression( expr );

   string::size_type pos = expression.find( ' ' );

   if( pos != string::npos )
   {
      string name( expression.substr( 0, pos ) );
      string value( expression.substr( pos + 1 ) );

      if( value == c_special_variable_none )
         value.erase( );

      expression.erase( pos );

      set_system_variable( name, value );
   }

   if( !expression.empty( )
    && ( ( expression[ 0 ] == '<' ) || ( expression[ 0 ] == '>' ) ) )
      expression.erase( 0, 1 );

   set_session_variable( c_special_variable_supplied_expression, expr );
   set_session_variable( c_special_variable_variable_expression, expression );
}

string variable_name_from_name_and_value( const string& name_and_value, string* p_value )
{
   string name;

   string::size_type from = 0;

   if( !name_and_value.empty( ) && ( name_and_value[ 0 ] == '"' ) )
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
