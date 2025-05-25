// Copyright (c) 2024-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_CORE_H
#  define CIYAM_CORE_H

const int c_max_key_length = 75;
const int c_max_fields_per_index = 5;
const int c_max_indexes_per_class = 100;
const int c_max_string_length_limit = 5000000;

enum file_type
{
   e_file_type_any,
   e_file_type_blob,
   e_file_type_list
};

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

const uint64_t c_state_normal = 0;

const uint64_t c_state_uneditable = UINT64_C( 0x01 );
const uint64_t c_state_undeletable = UINT64_C( 0x02 );
const uint64_t c_state_is_changing = UINT64_C( 0x04 );
const uint64_t c_state_unactionable = UINT64_C( 0x08 );
const uint64_t c_state_ignore_uneditable = UINT64_C( 0x10 );
const uint64_t c_state_force_focus_refresh = UINT64_C( 0x20 );

const uint64_t c_sys_state_flags = UINT64_C( 0x00000000000000ff );
const uint64_t c_user_state_flags = UINT64_C( 0xffffffffffffff00 );

const uint64_t c_unconfirmed_revision = UINT64_C( 999999999999999999 );

#endif
