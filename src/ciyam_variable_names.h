// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_SPECIAL_VARS_H
#  define CIYAM_SPECIAL_VARS_H

enum special_var
{
   e_special_var_bh,
   e_special_var_id,
   e_special_var_os,
   e_special_var_dtm,
   e_special_var_grp,
   e_special_var_key,
   e_special_var_sec,
   e_special_var_set,
   e_special_var_tag,
   e_special_var_tls,
   e_special_var_uid,
   e_special_var_sid,
   e_special_var_arg1,
   e_special_var_arg2,
   e_special_var_arg3,
   e_special_var_auto,
   e_special_var_cube,
   e_special_var_gids,
   e_special_var_hash,
   e_special_var_val1,
   e_special_var_val2,
   e_special_var_file,
   e_special_var_loop,
   e_special_var_name,
   e_special_var_none,
   e_special_var_path,
   e_special_var_peer,
   e_special_var_port,
   e_special_var_size,
   e_special_var_slot,
   e_special_var_type,
   e_special_var_user,
   e_special_var_uuid,
   e_special_var_algos,
   e_special_var_array,
   e_special_var_async,
   e_special_var_bytes,
   e_special_var_class,
   e_special_var_deque,
   e_special_var_embed,
   e_special_var_print,
   e_special_var_quiet,
   e_special_var_slotx,
   e_special_var_title,
   e_special_var_branch,
   e_special_var_cloned,
   e_special_var_images,
   e_special_var_module,
   e_special_var_opened,
   e_special_var_pubkey,
   e_special_var_return,
   e_special_var_script,
   e_special_var_command,
   e_special_var_do_exec,
   e_special_var_ip_addr,
   e_special_var_is_last,
   e_special_var_message,
   e_special_var_opening,
   e_special_var_package,
   e_special_var_pending,
   e_special_var_pubkeyx,
   e_special_var_restore,
   e_special_var_slowest,
   e_special_var_storage,
   e_special_var_tz_name,
   e_special_var_trigger,
   e_special_var_waiting,
   e_special_var_executed,
   e_special_var_identity,
   e_special_var_key_info,
   e_special_var_notifier,
   e_special_var_peer_hub,
   e_special_var_progress,
   e_special_var_updating,
   e_special_var_args_file,
   e_special_var_cmd_delay,
   e_special_var_crypt_key,
   e_special_var_datachain,
   e_special_var_decrement,
   e_special_var_executing,
   e_special_var_file_list,
   e_special_var_image_dir,
   e_special_var_increment,
   e_special_var_list_hash,
   e_special_var_peer_data,
   e_special_var_peer_port,
   e_special_var_peer_user,
   e_special_var_preparing,
   e_special_var_repo_lock,
   e_special_var_uid_check,
   e_special_var_val_error,
   e_special_var_blockchain,
   e_special_var_chain_type,
   e_special_var_extra_info,
   e_special_var_key_suffix,
   e_special_var_permission,
   e_special_var_queue_puts,
   e_special_var_session_id,
   e_special_var_tag_prefix,
   e_special_var_tree_count,
   e_special_var_tree_match,
   e_special_var_tree_total,
   e_special_var_allow_async,
   e_special_var_application,
   e_special_var_errors_only,
   e_special_var_init_log_id,
   e_special_var_opened_user,
   e_special_var_output_file,
   e_special_var_paired_sync,
   e_special_var_path_prefix,
   e_special_var_peer_msleep,
   e_special_var_permissions,
   e_special_var_queue_peers,
   e_special_var_secret_hash,
   e_special_var_skip_update,
   e_special_var_state_names,
   e_special_var_style_brief,
   e_special_var_submit_type,
   e_special_var_transaction,
   e_special_var_backup_files,
   e_special_var_block_height,
   e_special_var_dump_minimal,
   e_special_var_opened_files,
   e_special_var_peer_map_key,
   e_special_var_shared_files,
   e_special_var_app_directory,
   e_special_var_export_needed,
   e_special_var_import_needed,
   e_special_var_last_file_put,
   e_special_var_num_put_files,
   e_special_var_peer_sec_hash,
   e_special_var_prior_storage,
   e_special_var_rewind_height,
   e_special_var_shared_secret,
   e_special_var_skip_set_vars,
   e_special_var_stream_cipher,
   e_special_var_sub_directory,
   e_special_var_update_fields,
   e_special_var_cmd_delay_wait,
   e_special_var_files_area_dir,
   e_special_var_peer_clone_key,
   e_special_var_peer_initiator,
   e_special_var_peer_responder,
   e_special_var_prepare_system,
   e_special_var_progress_clear,
   e_special_var_progress_count,
   e_special_var_progress_fracs,
   e_special_var_progress_prior,
   e_special_var_progress_total,
   e_special_var_progress_value,
   e_special_var_restore_needed,
   e_special_var_restore_system,
   e_special_var_style_extended,
   e_special_var_sys_var_prefix,
   e_special_var_blockchain_data,
   e_special_var_blockchain_user,
   e_special_var_ciyam_list_hash,
   e_special_var_cmd_delay_after,
   e_special_var_force_db_create,
   e_special_var_notifier_events,
   e_special_var_paired_identity,
   e_special_var_queue_hub_users,
   e_special_var_row_cache_limit,
   e_special_var_system_identity,
   e_special_var_check_if_changed,
   e_special_var_dummy_time_stamp,
   e_special_var_preparing_backup,
   e_special_var_progress_message,
   e_special_var_progress_seconds,
   e_special_var_protocol_handler,
   e_special_var_session_id_owner,
   e_special_var_skip_after_fetch,
   e_special_var_skip_persistance,
   e_special_var_skip_submit_file,
   e_special_var_trace_session_id,
   e_special_var_trigger_variable,
   e_special_var_autoscript_reload,
   e_special_var_blockchain_height,
   e_special_var_blockchain_is_hub,
   e_special_var_fields_and_values,
   e_special_var_generate_log_file,
   e_special_var_last_suffixed_key,
   e_special_var_package_type_path,
   e_special_var_peer_data_created,
   e_special_var_peer_is_dependent,
   e_special_var_preparing_restore,
   e_special_var_attached_file_path,
   e_special_var_check_script_error,
   e_special_var_encrypted_password,
   e_special_var_exec_system_append,
   e_special_var_extra_field_values,
   e_special_var_file_info_buffered,
   e_special_var_fixed_field_values,
   e_special_var_generate_hub_block,
   e_special_var_repo_entry_missing,
   e_special_var_system_is_for_demo,
   e_special_var_blockchain_data_key,
   e_special_var_blockchain_identity,
   e_special_var_blockchain_is_owner,
   e_special_var_blockchain_num_puts,
   e_special_var_key_prefix_is_first,
   e_special_var_peer_identity_alias,
   e_special_var_repo_crypt_password,
   e_special_var_skip_parent_updates,
   e_special_var_blockchain_hind_hash,
   e_special_var_blockchain_unix_time,
   e_special_var_disallow_connections,
   e_special_var_ods_cache_hit_ratios,
   e_special_var_secondary_validation,
   e_special_var_blockchain_next_extra,
   e_special_var_blockchain_time_value,
   e_special_var_keep_user_peers_alive,
   e_special_var_package_install_extra,
   e_special_var_peer_is_synchronising,
   e_special_var_blockchain_is_checking,
   e_special_var_blockchain_is_combined,
   e_special_var_blockchain_is_fetching,
   e_special_var_peer_limited_arguments,
   e_special_var_single_string_response,
   e_special_var_skip_tagging_if_exists,
   e_special_var_blockchain_archive_path,
   e_special_var_blockchain_first_mapped,
   e_special_var_blockchain_height_other,
   e_special_var_blockchain_op_list_hash,
   e_special_var_complete_restore_needed,
   e_special_var_blockchain_backup_height,
   e_special_var_blockchain_put_list_hash,
   e_special_var_blockchain_shared_height,
   e_special_var_blockchain_stream_cipher,
   e_special_var_blockchain_zenith_height,
   e_special_var_blockchain_checked_shared,
   e_special_var_blockchain_get_tree_files,
   e_special_var_blockchain_num_tree_items,
   e_special_var_blockchain_other_is_owner,
   e_special_var_blockchain_peer_supporter,
   e_special_var_blockchain_tree_root_hash,
   e_special_var_blockchain_backup_identity,
   e_special_var_blockchain_block_file_hash,
   e_special_var_blockchain_both_are_owners,
   e_special_var_blockchain_found_secondary,
   e_special_var_blockchain_num_for_support,
   e_special_var_blockchain_peer_hub_height,
   e_special_var_blockchain_shared_identity,
   e_special_var_use_insecure_peer_protocol,
   e_special_var_blockchain_waiting_for_hub,
   e_special_var_blockchain_block_processing,
   e_special_var_blockchain_zenith_tree_hash,
   e_special_var_totals_exclude_hidden_blobs,
   e_special_var_blockchain_height_processing,
   e_special_var_blockchain_peer_hub_identity,
   e_special_var_blockchain_targeted_identity,
   e_special_var_blockchain_non_extra_identity,
   e_special_var_queue_archive_info_for_resize,
   e_special_var_blockchain_peer_has_supporters,
   e_special_var_blockchain_primary_pubkey_hash,
   e_special_var_blockchain_signature_file_hash,
   e_special_var_display_last_two_for_duplicate,
   e_special_var_blockchain_tertiary_pubkey_hash,
   e_special_var_blockchain_secondary_pubkey_hash,
   e_special_var_skip_total_child_field_in_parent,
   e_special_var_NOTE_THIS_MUST_ALWAYS_BE_THE_LAST_ENUM_FOR_VERIFICATION
};

#endif
