// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2018 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_COMMON_H
#  define CIYAM_COMMON_H

const int c_max_key_length = 75;
const int c_max_fields_per_index = 5;
const int c_max_indexes_per_class = 100;
const int c_max_string_length_limit = 5000000;

enum primitive
{
   e_primitive_string,
   e_primitive_datetime,
   e_primitive_date,
   e_primitive_time,
   e_primitive_numeric,
   e_primitive_int,
   e_primitive_bool
};

const uint64_t c_state_normal = 0;
const uint64_t c_state_uneditable = UINT64_C( 0x01 );
const uint64_t c_state_undeletable = UINT64_C( 0x02 );
const uint64_t c_state_is_changing = UINT64_C( 0x04 );
const uint64_t c_state_unactionable = UINT64_C( 0x08 );
const uint64_t c_state_ignore_uneditable = UINT64_C( 0x10 );

const uint64_t c_sys_state_flags = UINT64_C( 0x00000000000000ff );
const uint64_t c_user_state_flags = UINT64_C( 0xffffffffffffff00 );

const uint64_t c_unconfirmed_revision = UINT64_C( 999999999999999999 );

enum quote_style
{
   e_quote_style_both_linux,
   e_quote_style_both_windows,
   e_quote_style_windows_linux
};

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
   e_special_var_uid,
   e_special_var_arg1,
   e_special_var_arg2,
   e_special_var_cube,
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
   e_special_var_title,
   e_special_var_cloned,
   e_special_var_images,
   e_special_var_module,
   e_special_var_pubkey,
   e_special_var_return,
   e_special_var_script,
   e_special_var_do_exec,
   e_special_var_is_last,
   e_special_var_message,
   e_special_var_package,
   e_special_var_restore,
   e_special_var_slowest,
   e_special_var_storage,
   e_special_var_tz_name,
   e_special_var_trigger,
   e_special_var_cmd_hash,
   e_special_var_key_info,
   e_special_var_executed,
   e_special_var_identity,
   e_special_var_progress,
   e_special_var_args_file,
   e_special_var_crypt_key,
   e_special_var_decrement,
   e_special_var_image_dir,
   e_special_var_increment,
   e_special_var_val_error,
   e_special_var_blockchain,
   e_special_var_extra_info,
   e_special_var_file_names,
   e_special_var_permission,
   e_special_var_allow_async,
   e_special_var_application,
   e_special_var_errors_only,
   e_special_var_file_hashes,
   e_special_var_init_log_id,
   e_special_var_output_file,
   e_special_var_path_prefix,
   e_special_var_permissions,
   e_special_var_skip_update,
   e_special_var_state_names,
   e_special_var_transaction,
   e_special_var_block_height,
   e_special_var_rewind_height,
   e_special_var_update_fields,
   e_special_var_peer_initiator,
   e_special_var_peer_responder,
   e_special_var_unix_timestamp,
   e_special_var_dummy_timestamp,
   e_special_var_row_cache_limit,
   e_special_var_check_if_changed,
   e_special_var_skip_after_fetch,
   e_special_var_skip_persistance,
   e_special_var_fields_and_values,
   e_special_var_package_type_path,
   e_special_var_attached_file_path,
   e_special_var_check_script_error,
   e_special_var_blockchain_head_hash,
   e_special_var_blockchain_info_hash,
   e_special_var_locally_minted_block,
   e_special_var_secondary_validation,
   e_special_var_skip_blockchain_lock,
   e_special_var_peer_is_synchronising,
   e_special_var_total_child_field_in_parent
};

enum compare_op
{
   e_compare_op_equal,
   e_compare_op_lesser,
   e_compare_op_greater
};

enum op_apply_rc
{
   e_op_apply_rc_okay,
   e_op_apply_rc_locked,
   e_op_apply_rc_invalid
};

enum op_create_rc
{
   e_op_create_rc_okay,
   e_op_create_rc_locked,
   e_op_create_rc_exists
};

enum op_update_rc
{
   e_op_update_rc_okay,
   e_op_update_rc_locked,
   e_op_update_rc_not_found
};

enum op_destroy_rc
{
   e_op_destroy_rc_okay,
   e_op_destroy_rc_locked,
   e_op_destroy_rc_not_found,
   e_op_destroy_rc_constrained
};

enum begin_review_rc
{
   e_begin_review_rc_okay,
   e_begin_review_rc_locked,
   e_begin_review_rc_not_found
};

enum perform_fetch_rc
{
   e_perform_fetch_rc_okay,
   e_perform_fetch_rc_not_found
};

enum sql_optimisation
{
   e_sql_optimisation_none,
   e_sql_optimisation_unordered
};

#endif

