// Copyright (c) 2017-2023 CIYAM Developers
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

#define CIYAM_BASE_IMPL

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

#include "ciyam_constants.h"

const int c_max_lock_attempts = 20;
const int c_lock_attempt_sleep_time = 200;

const char c_persist_variable_prefix = '>';
const char c_restore_variable_prefix = '<';

const char* const c_invalid_name_chars = "<>\"|&\\";

const char* const c_special_variable_bh = "@bh";
const char* const c_special_variable_id = "@id";
const char* const c_special_variable_os = "@os";
const char* const c_special_variable_dtm = "@dtm";
const char* const c_special_variable_grp = "@grp";
const char* const c_special_variable_key = "@key";
const char* const c_special_variable_sec = "@sec";
const char* const c_special_variable_set = "@set";
const char* const c_special_variable_tag = "@tag";
const char* const c_special_variable_uid = "@uid";
const char* const c_special_variable_sid = "@sid";
const char* const c_special_variable_arg1 = "@arg1";
const char* const c_special_variable_arg2 = "@arg2";
const char* const c_special_variable_cube = "@cube";
const char* const c_special_variable_hash = "@hash";
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
const char* const c_special_variable_do_exec = "@do_exec";
const char* const c_special_variable_ip_addr = "@ip_addr";
const char* const c_special_variable_is_last = "@is_last";
const char* const c_special_variable_message = "@message";
const char* const c_special_variable_opening = "@opening";
const char* const c_special_variable_package = "@package";
const char* const c_special_variable_pubkeyx = "@pubkeyx";
const char* const c_special_variable_restore = "@restore";
const char* const c_special_variable_slowest = "@slowest";
const char* const c_special_variable_storage = "@storage";
const char* const c_special_variable_tz_name = "@tz_name";
const char* const c_special_variable_trigger = "@trigger";
const char* const c_special_variable_executed = "@executed";
const char* const c_special_variable_identity = "@identity";
const char* const c_special_variable_key_info = "@key_info";
const char* const c_special_variable_notifier = "@notifier";
const char* const c_special_variable_peer_hub = "@peer_hub";
const char* const c_special_variable_progress = "@progress";
const char* const c_special_variable_updating = "@updating";
const char* const c_special_variable_args_file = "@args_file";
const char* const c_special_variable_crypt_key = "@crypt_key";
const char* const c_special_variable_decrement = "@decrement";
const char* const c_special_variable_image_dir = "@image_dir";
const char* const c_special_variable_increment = "@increment";
const char* const c_special_variable_list_hash = "@list_hash";
const char* const c_special_variable_peer_port = "@peer_port";
const char* const c_special_variable_peer_user = "@peer_user";
const char* const c_special_variable_preparing = "@preparing";
const char* const c_special_variable_val_error = "@val_error";
const char* const c_special_variable_blockchain = "@blockchain";
const char* const c_special_variable_extra_info = "@extra_info";
const char* const c_special_variable_permission = "@permission";
const char* const c_special_variable_queue_puts = "@queue_puts";
const char* const c_special_variable_session_id = "@session_id";
const char* const c_special_variable_tag_prefix = "@tag_prefix";
const char* const c_special_variable_allow_async = "@allow_async";
const char* const c_special_variable_application = "@application";
const char* const c_special_variable_errors_only = "@errors_only";
const char* const c_special_variable_init_log_id = "@init_log_id";
const char* const c_special_variable_opened_user = "@opened_user";
const char* const c_special_variable_output_file = "@output_file";
const char* const c_special_variable_path_prefix = "@path_prefix";
const char* const c_special_variable_permissions = "@permissions";
const char* const c_special_variable_queue_peers = "@queue_peers";
const char* const c_special_variable_skip_update = "@skip_update";
const char* const c_special_variable_state_names = "@state_names";
const char* const c_special_variable_style_brief = "@style_brief";
const char* const c_special_variable_transaction = "@transaction";
const char* const c_special_variable_backup_files = "@backup_files";
const char* const c_special_variable_block_height = "@block_height";
const char* const c_special_variable_opened_files = "@opened_files";
const char* const c_special_variable_shared_files = "@shared_files";
const char* const c_special_variable_synchronised = "@synchronised";
const char* const c_special_variable_app_directory = "@app_directory";
const char* const c_special_variable_export_needed = "@export_needed";
const char* const c_special_variable_import_needed = "@import_needed";
const char* const c_special_variable_last_file_put = "@last_file_put";
const char* const c_special_variable_num_put_files = "@num_put_files";
const char* const c_special_variable_rewind_height = "@rewind_height";
const char* const c_special_variable_shared_secret = "@shared_secret";
const char* const c_special_variable_sub_directory = "@sub_directory";
const char* const c_special_variable_update_fields = "@update_fields";
const char* const c_special_variable_files_area_dir = "@files_area_dir";
const char* const c_special_variable_peer_initiator = "@peer_initiator";
const char* const c_special_variable_peer_responder = "@peer_responder";
const char* const c_special_variable_style_extended = "@style_extended";
const char* const c_special_variable_sys_var_prefix = "@sys_var_prefix";
const char* const c_special_variable_blockchain_user = "@blockchain_user";
const char* const c_special_variable_ciyam_list_hash = "@ciyam_list_hash";
const char* const c_special_variable_notifier_events = "@notifier_events";
const char* const c_special_variable_paired_identity = "@paired_identity";
const char* const c_special_variable_queue_hub_users = "@queue_hub_users";
const char* const c_special_variable_row_cache_limit = "@row_cache_limit";
const char* const c_special_variable_check_if_changed = "@check_if_changed";
const char* const c_special_variable_dummy_time_stamp = "@dummy_time_stamp";
const char* const c_special_variable_skip_after_fetch = "@skip_after_fetch";
const char* const c_special_variable_skip_persistance = "@skip_persistance";
const char* const c_special_variable_trace_session_id = "@trace_session_id";
const char* const c_special_variable_blockchain_height = "@blockchain_height";
const char* const c_special_variable_blockchain_is_hub = "@blockchain_is_hub";
const char* const c_special_variable_fields_and_values = "@fields_and_values";
const char* const c_special_variable_package_type_path = "@package_type_path";
const char* const c_special_variable_peer_is_dependent = "@peer_is_dependent";
const char* const c_special_variable_attached_file_path = "@attached_file_path";
const char* const c_special_variable_check_script_error = "@check_script_error";
const char* const c_special_variable_encrypted_password = "@encrypted_password";
const char* const c_special_variable_extra_field_values = "@extra_field_values";
const char* const c_special_variable_file_info_buffered = "@file_info_buffered";
const char* const c_special_variable_generate_hub_block = "@generate_hub_block";
const char* const c_special_variable_repo_entry_missing = "@repo_entry_missing";
const char* const c_special_variable_blockchain_is_owner = "@blockchain_is_owner";
const char* const c_special_variable_blockchain_num_puts = "@blockchain_num_puts";
const char* const c_special_variable_repo_crypt_password = "@repo_crypt_password";
const char* const c_special_variable_blockchain_hind_hash = "@blockchain_hind_hash";
const char* const c_special_variable_ods_cache_hit_ratios = "@ods_cache_hit_ratios";
const char* const c_special_variable_secondary_validation = "@secondary_validation";
const char* const c_special_variable_blockchain_time_value = "@blockchain_time_value";
const char* const c_special_variable_keep_user_peers_alive = "@keep_user_peers_alive";
const char* const c_special_variable_package_install_extra = "@package_install_extra";
const char* const c_special_variable_peer_is_synchronising = "@peer_is_synchronising";
const char* const c_special_variable_blockchain_is_checking = "@blockchain_is_checking";
const char* const c_special_variable_blockchain_is_fetching = "@blockchain_is_fetching";
const char* const c_special_variable_single_string_response = "@single_string_response";
const char* const c_special_variable_skip_tagging_if_exists = "@skip_tagging_if_exists";
const char* const c_special_variable_blockchain_archive_path = "@blockchain_archive_path";
const char* const c_special_variable_blockchain_first_mapped = "@blockchain_first_mapped";
const char* const c_special_variable_blockchain_op_list_hash = "@blockchain_op_list_hash";
const char* const c_special_variable_blockchain_backup_height = "@blockchain_backup_height";
const char* const c_special_variable_blockchain_put_list_hash = "@blockchain_put_list_hash";
const char* const c_special_variable_blockchain_shared_height = "@blockchain_shared_height";
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
const char* const c_special_variable_blockchain_shared_identity = "@blockchain_shared_identity";
const char* const c_special_variable_blockchain_waiting_for_hub = "@blockchain_waiting_for_hub";
const char* const c_special_variable_blockchain_block_processing = "@blockchain_block_processing";
const char* const c_special_variable_blockchain_zenith_tree_hash = "@blockchain_zenith_tree_hash";
const char* const c_special_variable_total_child_field_in_parent = "@total_child_field_in_parent";
const char* const c_special_variable_totals_exclude_hidden_blobs = "@totals_exclude_hidden_blobs";
const char* const c_special_variable_blockchain_height_processing = "@blockchain_height_processing";
const char* const c_special_variable_blockchain_peer_hub_identity = "@blockchain_peer_hub_identity";
const char* const c_special_variable_blockchain_targeted_identity = "@blockchain_targeted_identity";
const char* const c_special_variable_blockchain_peer_has_supporters = "@blockchain_peer_has_supporters";
const char* const c_special_variable_blockchain_primary_pubkey_hash = "@blockchain_primary_pubkey_hash";
const char* const c_special_variable_blockchain_signature_file_hash = "@blockchain_signature_file_hash";
const char* const c_special_variable_blockchain_tertiary_pubkey_hash = "@blockchain_tertiary_pubkey_hash";
const char* const c_special_variable_blockchain_secondary_pubkey_hash = "@blockchain_secondary_pubkey_hash";

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

}

string get_special_var_name( special_var var )
{
   string s;

   switch( var )
   {
      case e_special_var_bh:
      s = string( c_special_variable_bh );
      break;

      case e_special_var_id:
      s = string( c_special_variable_id );
      break;

      case  e_special_var_os:
      s = string( c_special_variable_os );
      break;

      case e_special_var_dtm:
      s = string( c_special_variable_dtm );
      break;

      case e_special_var_grp:
      s = string( c_special_variable_grp );
      break;

      case e_special_var_key:
      s = string( c_special_variable_key );
      break;

      case e_special_var_sec:
      s = string( c_special_variable_sec );
      break;

      case e_special_var_set:
      s = string( c_special_variable_set );
      break;

      case e_special_var_tag:
      s = string( c_special_variable_tag );
      break;

      case e_special_var_uid:
      s = string( c_special_variable_uid );
      break;

      case e_special_var_sid:
      s = string( c_special_variable_sid );
      break;

      case e_special_var_arg1:
      s = string( c_special_variable_arg1 );
      break;

      case e_special_var_arg2:
      s = string( c_special_variable_arg2 );
      break;

      case e_special_var_cube:
      s = string( c_special_variable_cube );
      break;

      case e_special_var_hash:
      s = string( c_special_variable_hash );
      break;

      case e_special_var_val1:
      s = string( c_special_variable_val1 );
      break;

      case e_special_var_val2:
      s = string( c_special_variable_val2 );
      break;

      case e_special_var_file:
      s = string( c_special_variable_file );
      break;

      case e_special_var_loop:
      s = string( c_special_variable_loop );
      break;

      case e_special_var_name:
      s = string( c_special_variable_name );
      break;

      case e_special_var_none:
      s = string( c_special_variable_none );
      break;

      case e_special_var_path:
      s = string( c_special_variable_path );
      break;

      case e_special_var_peer:
      s = string( c_special_variable_peer );
      break;

      case e_special_var_port:
      s = string( c_special_variable_port );
      break;

      case e_special_var_size:
      s = string( c_special_variable_size );
      break;

      case e_special_var_slot:
      s = string( c_special_variable_slot );
      break;

      case e_special_var_type:
      s = string( c_special_variable_type );
      break;

      case e_special_var_user:
      s = string( c_special_variable_user );
      break;

      case e_special_var_uuid:
      s = string( c_special_variable_uuid );
      break;

      case e_special_var_algos:
      s = string( c_special_variable_algos );
      break;

      case e_special_var_array:
      s = string( c_special_variable_array );
      break;

      case e_special_var_async:
      s = string( c_special_variable_async );
      break;

      case e_special_var_bytes:
      s = string( c_special_variable_bytes );
      break;

      case e_special_var_class:
      s = string( c_special_variable_class );
      break;

      case e_special_var_deque:
      s = string( c_special_variable_deque );
      break;

      case e_special_var_embed:
      s = string( c_special_variable_embed );
      break;

      case e_special_var_print:
      s = string( c_special_variable_print );
      break;

      case e_special_var_quiet:
      s = string( c_special_variable_quiet );
      break;

      case e_special_var_slotx:
      s = string( c_special_variable_slotx );
      break;

      case e_special_var_title:
      s = string( c_special_variable_title );
      break;

      case e_special_var_branch:
      s = string( c_special_variable_branch );
      break;

      case e_special_var_cloned:
      s = string( c_special_variable_cloned );
      break;

      case e_special_var_images:
      s = string( c_special_variable_images );
      break;

      case e_special_var_module:
      s = string( c_special_variable_module );
      break;

      case e_special_var_opened:
      s = string( c_special_variable_opened );
      break;

      case e_special_var_pubkey:
      s = string( c_special_variable_pubkey );
      break;

      case e_special_var_return:
      s = string( c_special_variable_return );
      break;

      case e_special_var_script:
      s = string( c_special_variable_script );
      break;

      case e_special_var_do_exec:
      s = string( c_special_variable_do_exec );
      break;

      case e_special_var_ip_addr:
      s = string( c_special_variable_ip_addr );
      break;

      case e_special_var_is_last:
      s = string( c_special_variable_is_last );
      break;

      case e_special_var_message:
      s = string( c_special_variable_message );
      break;

      case e_special_var_opening:
      s = string( c_special_variable_opening );
      break;

      case e_special_var_package:
      s = string( c_special_variable_package );
      break;

      case e_special_var_pubkeyx:
      s = string( c_special_variable_pubkeyx );
      break;

      case e_special_var_restore:
      s = string( c_special_variable_restore );
      break;

      case e_special_var_slowest:
      s = string( c_special_variable_slowest );
      break;

      case e_special_var_storage:
      s = string( c_special_variable_storage );
      break;

      case e_special_var_tz_name:
      s = string( c_special_variable_tz_name );
      break;

      case e_special_var_trigger:
      s = string( c_special_variable_trigger );
      break;

      case e_special_var_executed:
      s = string( c_special_variable_executed );
      break;

      case e_special_var_identity:
      s = string( c_special_variable_identity );
      break;

      case e_special_var_key_info:
      s = string( c_special_variable_key_info );
      break;

      case e_special_var_notifier:
      s = string( c_special_variable_notifier );
      break;

      case e_special_var_peer_hub:
      s = string( c_special_variable_peer_hub );
      break;

      case e_special_var_progress:
      s = string( c_special_variable_progress );
      break;

      case e_special_var_updating:
      s = string( c_special_variable_updating );
      break;

      case e_special_var_args_file:
      s = string( c_special_variable_args_file );
      break;

      case e_special_var_crypt_key:
      s = string( c_special_variable_crypt_key );
      break;

      case e_special_var_decrement:
      s = string( c_special_variable_decrement );
      break;

      case e_special_var_image_dir:
      s = string( c_special_variable_image_dir );
      break;

      case e_special_var_increment:
      s = string( c_special_variable_increment );
      break;

      case e_special_var_list_hash:
      s = string( c_special_variable_list_hash );
      break;

      case e_special_var_peer_port:
      s = string( c_special_variable_peer_port );
      break;

      case e_special_var_peer_user:
      s = string( c_special_variable_peer_user );
      break;

      case e_special_var_preparing:
      s = string( c_special_variable_preparing );
      break;

      case e_special_var_val_error:
      s = string( c_special_variable_val_error );
      break;

      case e_special_var_blockchain:
      s = string( c_special_variable_blockchain );
      break;

      case e_special_var_extra_info:
      s = string( c_special_variable_extra_info );
      break;

      case e_special_var_permission:
      s = string( c_special_variable_permission );
      break;

      case e_special_var_queue_puts:
      s = string( c_special_variable_queue_puts );
      break;

      case e_special_var_session_id:
      s = string( c_special_variable_session_id );
      break;

      case e_special_var_tag_prefix:
      s = string( c_special_variable_tag_prefix );
      break;

      case e_special_var_allow_async:
      s = string( c_special_variable_allow_async );
      break;

      case e_special_var_application:
      s = string( c_special_variable_application );
      break;

      case e_special_var_errors_only:
      s = string( c_special_variable_errors_only );
      break;

      case e_special_var_init_log_id:
      s = string( c_special_variable_init_log_id );
      break;

      case e_special_var_opened_user:
      s = string( c_special_variable_opened_user );
      break;

      case e_special_var_output_file:
      s = string( c_special_variable_output_file );
      break;

      case e_special_var_path_prefix:
      s = string( c_special_variable_path_prefix );
      break;

      case e_special_var_permissions:
      s = string( c_special_variable_permissions );
      break;

      case e_special_var_queue_peers:
      s = string( c_special_variable_queue_peers );
      break;

      case e_special_var_skip_update:
      s = string( c_special_variable_skip_update );
      break;

      case e_special_var_state_names:
      s = string( c_special_variable_state_names );
      break;

      case e_special_var_style_brief:
      s = string( c_special_variable_style_brief );
      break;

      case e_special_var_transaction:
      s = string( c_special_variable_transaction );
      break;

      case e_special_var_backup_files:
      s = string( c_special_variable_backup_files );
      break;

      case e_special_var_block_height:
      s = string( c_special_variable_block_height );
      break;

      case e_special_var_opened_files:
      s = string( c_special_variable_opened_files );
      break;

      case e_special_var_shared_files:
      s = string( c_special_variable_shared_files );
      break;

      case e_special_var_synchronised:
      s = string( c_special_variable_synchronised );
      break;

      case e_special_var_app_directory:
      s = string( c_special_variable_app_directory );
      break;

      case e_special_var_export_needed:
      s = string( c_special_variable_export_needed );
      break;

      case e_special_var_import_needed:
      s = string( c_special_variable_import_needed );
      break;

      case e_special_var_last_file_put:
      s = string( c_special_variable_last_file_put );
      break;

      case e_special_var_num_put_files:
      s = string( c_special_variable_num_put_files );
      break;

      case e_special_var_rewind_height:
      s = string( c_special_variable_rewind_height );
      break;

      case e_special_var_shared_secret:
      s = string( c_special_variable_shared_secret );
      break;

      case e_special_var_sub_directory:
      s = string( c_special_variable_sub_directory );
      break;

      case e_special_var_update_fields:
      s = string( c_special_variable_update_fields );
      break;

      case e_special_var_files_area_dir:
      s = string( c_special_variable_files_area_dir );
      break;

      case e_special_var_peer_initiator:
      s = string( c_special_variable_peer_initiator );
      break;

      case e_special_var_peer_responder:
      s = string( c_special_variable_peer_responder );
      break;

      case e_special_var_style_extended:
      s = string( c_special_variable_style_extended );
      break;

      case e_special_var_blockchain_user:
      s = string( c_special_variable_blockchain_user );
      break;

      case e_special_var_ciyam_list_hash:
      s = string( c_special_variable_ciyam_list_hash );
      break;

      case e_special_var_notifier_events:
      s = string( c_special_variable_notifier_events );
      break;

      case e_special_var_paired_identity:
      s = string( c_special_variable_paired_identity );
      break;

      case e_special_var_queue_hub_users:
      s = string( c_special_variable_queue_hub_users );
      break;

      case e_special_var_row_cache_limit:
      s = string( c_special_variable_row_cache_limit );
      break;

      case e_special_var_check_if_changed:
      s = string( c_special_variable_check_if_changed );
      break;

      case e_special_var_dummy_time_stamp:
      s = string( c_special_variable_dummy_time_stamp );
      break;

      case e_special_var_skip_after_fetch:
      s = string( c_special_variable_skip_after_fetch );
      break;

      case e_special_var_skip_persistance:
      s = string( c_special_variable_skip_persistance );
      break;

      case e_special_var_trace_session_id:
      s = string( c_special_variable_trace_session_id );
      break;

      case e_special_var_blockchain_height:
      s = string( c_special_variable_blockchain_height );
      break;

      case e_special_var_blockchain_is_hub:
      s = string( c_special_variable_blockchain_is_hub );
      break;

      case e_special_var_fields_and_values:
      s = string( c_special_variable_fields_and_values );
      break;

      case e_special_var_package_type_path:
      s = string( c_special_variable_package_type_path );
      break;

      case e_special_var_peer_is_dependent:
      s = string( c_special_variable_peer_is_dependent );
      break;

      case e_special_var_attached_file_path:
      s = string( c_special_variable_attached_file_path );
      break;

      case e_special_var_check_script_error:
      s = string( c_special_variable_check_script_error );
      break;

      case e_special_var_encrypted_password:
      s = string( c_special_variable_encrypted_password );
      break;

      case e_special_var_extra_field_values:
      s = string( c_special_variable_extra_field_values );
      break;

      case e_special_var_file_info_buffered:
      s = string( c_special_variable_file_info_buffered );
      break;

      case e_special_var_generate_hub_block:
      s = string( c_special_variable_generate_hub_block );
      break;

      case e_special_var_repo_entry_missing:
      s = string( c_special_variable_repo_entry_missing );
      break;

      case e_special_var_blockchain_is_owner:
      s = string( c_special_variable_blockchain_is_owner );
      break;

      case e_special_var_blockchain_num_puts:
      s = string( c_special_variable_blockchain_num_puts );
      break;

      case e_special_var_repo_crypt_password:
      s = string( c_special_variable_repo_crypt_password );
      break;

      case e_special_var_blockchain_hind_hash:
      s = string( c_special_variable_blockchain_hind_hash );
      break;

      case e_special_var_ods_cache_hit_ratios:
      s = string( c_special_variable_ods_cache_hit_ratios );
      break;

      case e_special_var_secondary_validation:
      s = string( c_special_variable_secondary_validation );
      break;

      case e_special_var_blockchain_time_value:
      s = string( c_special_variable_blockchain_time_value );
      break;

      case e_special_var_keep_user_peers_alive:
      s = string( c_special_variable_keep_user_peers_alive );
      break;

      case e_special_var_package_install_extra:
      s = string( c_special_variable_package_install_extra );
      break;

      case e_special_var_peer_is_synchronising:
      s = string( c_special_variable_peer_is_synchronising );
      break;

      case e_special_var_blockchain_is_checking:
      s = string( c_special_variable_blockchain_is_checking );
      break;

      case e_special_var_blockchain_is_fetching:
      s = string( c_special_variable_blockchain_is_fetching );
      break;

      case e_special_var_single_string_response:
      s = string( c_special_variable_single_string_response );
      break;

      case e_special_var_skip_tagging_if_exists:
      s = string( c_special_variable_skip_tagging_if_exists );
      break;

      case e_special_var_blockchain_archive_path:
      s = string( c_special_variable_blockchain_archive_path );
      break;

      case e_special_var_blockchain_first_mapped:
      s = string( c_special_variable_blockchain_first_mapped );
      break;

      case e_special_var_blockchain_op_list_hash:
      s = string( c_special_variable_blockchain_op_list_hash );
      break;

      case e_special_var_blockchain_backup_height:
      s = string( c_special_variable_blockchain_backup_height );
      break;

      case e_special_var_blockchain_put_list_hash:
      s = string( c_special_variable_blockchain_put_list_hash );
      break;

      case e_special_var_blockchain_shared_height:
      s = string( c_special_variable_blockchain_shared_height );
      break;

      case e_special_var_blockchain_zenith_height:
      s = string( c_special_variable_blockchain_zenith_height );
      break;

      case e_special_var_blockchain_checked_shared:
      s = string( c_special_variable_blockchain_checked_shared );
      break;

      case e_special_var_blockchain_get_tree_files:
      s = string( c_special_variable_blockchain_get_tree_files );
      break;

      case e_special_var_blockchain_num_tree_items:
      s = string( c_special_variable_blockchain_num_tree_items );
      break;

      case e_special_var_blockchain_other_is_owner:
      s = string( c_special_variable_blockchain_other_is_owner );
      break;

      case e_special_var_blockchain_peer_supporter:
      s = string( c_special_variable_blockchain_peer_supporter );
      break;

      case e_special_var_blockchain_tree_root_hash:
      s = string( c_special_variable_blockchain_tree_root_hash );
      break;

      case e_special_var_blockchain_backup_identity:
      s = string( c_special_variable_blockchain_backup_identity );
      break;

      case e_special_var_blockchain_block_file_hash:
      s = string( c_special_variable_blockchain_block_file_hash );
      break;

      case e_special_var_blockchain_both_are_owners:
      s = string( c_special_variable_blockchain_both_are_owners );
      break;

      case e_special_var_blockchain_shared_identity:
      s = string( c_special_variable_blockchain_shared_identity );
      break;

      case e_special_var_blockchain_waiting_for_hub:
      s = string( c_special_variable_blockchain_waiting_for_hub );
      break;

      case e_special_var_blockchain_block_processing:
      s = string( c_special_variable_blockchain_block_processing );
      break;

      case e_special_var_blockchain_zenith_tree_hash:
      s = string( c_special_variable_blockchain_zenith_tree_hash );
      break;

      case e_special_var_total_child_field_in_parent:
      s = string( c_special_variable_total_child_field_in_parent );
      break;

      case e_special_var_totals_exclude_hidden_blobs:
      s = string( c_special_variable_totals_exclude_hidden_blobs );
      break;

      case e_special_var_blockchain_height_processing:
      s = string( c_special_variable_blockchain_height_processing );
      break;

      case e_special_var_blockchain_peer_hub_identity:
      s = string( c_special_variable_blockchain_peer_hub_identity );
      break;

      case e_special_var_blockchain_targeted_identity:
      s = string( c_special_variable_blockchain_targeted_identity );
      break;

      case e_special_var_blockchain_peer_has_supporters:
      s = string( c_special_variable_blockchain_peer_has_supporters );
      break;

      case e_special_var_blockchain_primary_pubkey_hash:
      s = string( c_special_variable_blockchain_primary_pubkey_hash );
      break;

      case e_special_var_blockchain_signature_file_hash:
      s = string( c_special_variable_blockchain_signature_file_hash );
      break;

      case e_special_var_blockchain_tertiary_pubkey_hash:
      s = string( c_special_variable_blockchain_tertiary_pubkey_hash );
      break;

      case e_special_var_blockchain_secondary_pubkey_hash:
      s = string( c_special_variable_blockchain_secondary_pubkey_hash );
      break;

      default:
      throw runtime_error( "unexpected special var value #" + to_string( var ) );
   }

   return s;
}

system_variable_lock::system_variable_lock( const string& name )
 :
 name( name )
{
   bool acquired = false;

   for( size_t i = 0; i < c_max_lock_attempts; i++ )
   {
      // NOTE: Empty code block for scope purposes.
      {
         guard g( g_mutex );

         if( set_system_variable( name, "<locked>", string( "" ) ) )
         {
            acquired = true;
            break;
         }
      }

      msleep( c_lock_attempt_sleep_time );
   }

   if( !acquired )
      throw runtime_error( "unable to acquire lock for system variable '" + name + "'" );
}

system_variable_lock::~system_variable_lock( )
{
   set_system_variable( name, "" );
}

string get_raw_system_variable( const string& name )
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

         if( var_name.empty( ) || var_name == "*" )
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

                  if( output_all_persistent_variables || value != next_value )
                  {
                     if( !retval.empty( ) )
                        retval += "\n";

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
      if( var_name == "*" || var_name == "**" )
         var_name = sys_var_prefix + var_name;

      // NOTE: Allow use of "**" for reverse iteration.
      if( var_name.find( "**" ) == string::npos )
      {
         map< string, string >::const_iterator ci;
         for( ci = g_variables.begin( ); ci != g_variables.end( ); ++ci )
         {
            if( wildcard_match( var_name, ci->first ) )
            {
               if( !retval.empty( ) )
                  retval += "\n";

               retval += quote_if_contains_white_space( ci->first ) + ' ' + ci->second;
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
         replace( var_name, "**", "*" );

         map< string, string >::const_reverse_iterator cri;
         for( cri = g_variables.rbegin( ); cri != g_variables.rend( ); ++cri )
         {
            if( wildcard_match( var_name, cri->first ) )
            {
               if( !retval.empty( ) )
                  retval += "\n";

               retval += quote_if_contains_white_space( cri->first ) + ' ' + cri->second;
            }
         }

         map< string, deque< string > >::const_reverse_iterator dcri;
         for( dcri = g_deque_variables.rbegin( ); dcri != g_deque_variables.rend( ); ++dcri )
         {
            if( wildcard_match( var_name, dcri->first ) )
            {
               if( !retval.empty( ) )
                  retval += "\n";

               retval += quote_if_contains_white_space( dcri->first ) + ' ' + dcri->second.front( );

               if( dcri->second.size( ) > 1 )
                  retval += " (+" + to_string( dcri->second.size( ) - 1 ) + ")";
            }
         }
      }
   }
   else
   {
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
         retval = g_variables[ var_name ];
      else if( name == get_special_var_name( e_special_var_none ) )
         retval = " ";
      else if( var_name == string( c_special_variable_files_area_dir ) )
         retval = string( c_files_directory );
      else if( var_name == string( c_special_variable_ods_cache_hit_ratios ) )
         retval = system_ods_instance( ).get_cache_hit_ratios( );
   }

   return retval;
}

struct raw_system_variable_getter : variable_getter
{
   string get_value( const string& name ) const { return get_raw_system_variable( name ); }
};

string get_system_variable( const string& name_or_expr )
{
   raw_system_variable_getter raw_getter;
   variable_expression expr( name_or_expr, raw_getter );

   return expr.get_value( );
}

bool has_system_variable( const string& name_or_expr )
{
   return !get_system_variable( name_or_expr ).empty( );
}

void set_system_variable( const string& name, const string& value, bool is_init, progress* p_progress )
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
            vars_to_change.push_back( vi->first );

         if( !prefix.empty( ) && ( vi->first.find( prefix ) != 0 ) )
            break;
      }

      for( size_t i = 0; i < vars_to_change.size( ); i++ )
      {
         if( !val.empty( ) )
            g_variables[ vars_to_change[ i ] ] = val;
         else
            g_variables.erase( vars_to_change[ i ] );
      }
   }
   else
   {
      if( val.empty( ) && var_name == string( c_special_variable_files_area_dir ) )
         val = string( c_files_directory );

      if( !val.empty( ) )
         g_variables[ var_name ] = val;
      else
      {
         if( g_variables.count( var_name ) )
            g_variables.erase( var_name );
      }
   }

   // NOTE: Do not allow "@os" or "@peer_port" to be persisted.
   if( ( var_name == string( c_special_variable_os ) )
    || ( var_name == string( c_special_variable_peer_port ) ) )
      persist = false;

   if( var_name == string( c_special_variable_files_area_dir ) )
   {
      // NOTE: It makes no sense to persist "@files_area_dir"
      // as application server files are being stored there.
      persist = false;

      if( !is_init )
      {
         if( g_active_sessions > 1 )
            throw runtime_error( "invalid switch files area whilst has other active sessions (including autostarted)" );

         // NOTE: Although already thread locked adds a mutex name here
         // just in case a deadlock could arise during the resync calls.
         guard g( g_mutex, "set_system_variable" );

         TRACE_LOG( TRACE_ANYTHING, "*** switched files area across to: " + val + " ***" );

         string from( get_files_area_dir( ) );
         set_files_area_dir( val );

         resync_files_area( p_progress );
         resync_system_ods( p_progress );

         resync_archive_info( p_progress );
 
         TRACE_LOG( TRACE_ANYTHING, "*** switched files area over from: " + from + " ***" );
      }
   }
   else if( var_name == string( c_special_variable_ods_cache_hit_ratios ) )
   {
      persist = false;

      system_ods_instance( ).clear_cache_statistics( );
   }

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

bool set_system_variable( const string& name, const string& value,
 const string& current, progress* p_progress, const char append_separator )
{
   guard g( g_mutex );

   bool retval = false;

   if( name.find_first_of( c_invalid_name_chars ) != string::npos )
      throw runtime_error( "invalid system variable name '" + name + "'" );

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

   if( name == string( c_special_variable_files_area_dir ) )
      throw runtime_error( "invalid attempt to change the files area dir" );

   return retval;
}

bool set_variable_checker::can_set( ) const
{
   bool retval = false;

   if( check_type == e_variable_check_type_no_session_has )
      retval = !has_any_session_variable( variable_name );
   else if( check_type == e_variable_check_type_any_session_has )
      retval = has_any_session_variable( variable_name );
   else if( check_type == e_variable_check_type_has_other_system )
      retval = !get_raw_system_variable( variable_name ).empty( );
   else if( check_type == e_variable_check_type_not_has_other_system )
      retval = get_raw_system_variable( variable_name ).empty( );

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

   set_system_variable( name, value, is_init, p_progress );

   return true;
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
