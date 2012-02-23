// Copyright (c) 2006
//
// CIYAM Pty. Ltd.
// ACN 093 704 539
//
// ALL RIGHTS RESERVED
//
// Permission to use this software for non-commercial purposes is hereby granted. Permission to
// distribute this software privately is granted provided that the source code is unaltered and
// complete or that any alterations and omissions have been first approved by CIYAM. Commercial
// usage of this software is not permitted without first obtaining a license for such a purpose
// from CIYAM. This software may not be publicly distributed unless written permission to do so
// has been obtained from CIYAM.

#ifndef CIYAM_COMMON_H
#  define CIYAM_COMMON_H

const int c_max_fields_per_index = 5;

const int c_max_indexes_per_class = 100;

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

