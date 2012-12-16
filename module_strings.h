// Copyright (c) 2007
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

#ifndef MODULE_STRINGS_H
#  define MODULE_STRINGS_H

const char* const c_str_error = "{@error}.";
const char* const c_str_parm_error_error = "{@error}";
const char* const c_str_field_mismatch = "{@field} does not match {@field2}.";
const char* const c_str_parm_field_mismatch_field = "{@field}";
const char* const c_str_parm_field_mismatch_field2 = "{@field2}";
const char* const c_str_field_must_not_be_empty = "{@field} must not be empty.";
const char* const c_str_parm_field_must_not_be_empty_field = "{@field}";
const char* const c_str_field_must_be_eq_other = "{@field} must be equal to {@field2}.";
const char* const c_str_parm_field_must_be_eq_other_field = "{@field}";
const char* const c_str_parm_field_must_be_eq_other_field2 = "{@field2}";
const char* const c_str_field_must_be_neq_other = "{@field} must be not equal to {@field2}.";
const char* const c_str_parm_field_must_be_neq_other_field = "{@field}";
const char* const c_str_parm_field_must_be_neq_other_field2 = "{@field2}";
const char* const c_str_field_must_be_lt_other = "{@field} must be less than {@field2}.";
const char* const c_str_parm_field_must_be_lt_other_field = "{@field}";
const char* const c_str_parm_field_must_be_lt_other_field2 = "{@field2}";
const char* const c_str_field_must_be_gt_other = "{@field} must be greater than {@field2}.";
const char* const c_str_parm_field_must_be_gt_other_field = "{@field}";
const char* const c_str_parm_field_must_be_gt_other_field2 = "{@field2}";
const char* const c_str_field_must_be_lteq_other = "{@field} must be less than or equal to {@field2}.";
const char* const c_str_parm_field_must_be_lteq_other_field = "{@field}";
const char* const c_str_parm_field_must_be_lteq_other_field2 = "{@field2}";
const char* const c_str_field_must_be_gteq_other = "{@field} must be greater than or equal to {@field2}.";
const char* const c_str_parm_field_must_be_gteq_other_field = "{@field}";
const char* const c_str_parm_field_must_be_gteq_other_field2 = "{@field2}";
const char* const c_str_field_must_be_eq_value = "{@field} must be equal to {@value}.";
const char* const c_str_parm_field_must_be_eq_value_field = "{@field}";
const char* const c_str_parm_field_must_be_eq_value_value = "{@value}";
const char* const c_str_field_must_be_neq_value = "{@field} must be not equal to {@value}.";
const char* const c_str_parm_field_must_be_neq_value_field = "{@field}";
const char* const c_str_parm_field_must_be_neq_value_value = "{@value}";
const char* const c_str_field_must_be_lt_value = "{@field} must be less than {@value}.";
const char* const c_str_parm_field_must_be_lt_value_field = "{@field}";
const char* const c_str_parm_field_must_be_lt_value_value = "{@value}";
const char* const c_str_field_must_be_gt_value = "{@field} must be greater than {@value}.";
const char* const c_str_parm_field_must_be_gt_value_field = "{@field}";
const char* const c_str_parm_field_must_be_gt_value_value = "{@value}";
const char* const c_str_field_must_be_lteq_value = "{@field} must be less than or equal to {@value}.";
const char* const c_str_parm_field_must_be_lteq_value_field = "{@field}";
const char* const c_str_parm_field_must_be_lteq_value_value = "{@value}";
const char* const c_str_field_must_be_gteq_value = "{@field} must be greater than or equal to {@value}.";
const char* const c_str_parm_field_must_be_gteq_value_field = "{@field}";
const char* const c_str_parm_field_must_be_gteq_value_value = "{@value}";
const char* const c_str_field_has_invalid_value = "{@field} has not been set to a permitted value.";
const char* const c_str_parm_field_has_invalid_value_field = "{@field}";
const char* const c_str_field_must_be_empty_for_root = "{@field} must be empty for the root folder.";
const char* const c_str_parm_field_must_be_empty_for_root_field = "{@field}";
const char* const c_str_field_must_be_empty_match = "{@field2} must be empty when {@field1} is empty.";
const char* const c_str_parm_field_must_be_empty_match_field2 = "{@field2}";
const char* const c_str_parm_field_must_be_empty_match_field1 = "{@field1}";
const char* const c_str_field_must_be_greater_or_equal = "{@field} must be greater than or equal to {@field2}.";
const char* const c_str_parm_field_must_be_greater_or_equal_field = "{@field}";
const char* const c_str_parm_field_must_be_greater_or_equal_field2 = "{@field2}";
const char* const c_str_field_must_contain_no_more = "{@field} must contain no more than {@value} items.";
const char* const c_str_parm_field_must_contain_no_more_field = "{@field}";
const char* const c_str_parm_field_must_contain_no_more_limit = "{@limit}";
const char* const c_str_field_must_match_parents = "{@field} must match that of the {@pfield}.";
const char* const c_str_parm_field_must_match_parents_field = "{@field}";
const char* const c_str_parm_field_must_match_parents_pfield = "{@pfield}";
const char* const c_str_field_types_must_match = "{@field1} and {@field2} types must match.";
const char* const c_str_parm_field_types_must_match_field1 = "{@field1}";
const char* const c_str_parm_field_types_must_match_field2 = "{@field2}";
const char* const c_str_field_dep_must_be_changed = "{@field1} must be changed as {@field2} was changed.";
const char* const c_str_parm_field_dep_must_be_changed_field1 = "{@field1}";
const char* const c_str_parm_field_dep_must_be_changed_field2 = "{@field2}";
const char* const c_str_field_and_original_mismatch = "{@pfield} is not compatible with the original's {@field}.";
const char* const c_str_parm_field_and_original_mismatch_pfield = "{@pfield}";
const char* const c_str_parm_field_and_original_mismatch_field = "{@field}";
const char* const c_str_cannot_link_to_self = "{@class} cannot be linked to itself.";
const char* const c_str_parm_cannot_link_to_self_class = "{@class}";
const char* const c_str_linking_to_class_prohibited = "Linking to this {@class} is prohibited.";
const char* const c_str_parm_linking_to_class_prohibited_class = "{@class}";
const char* const c_str_value_must_be_in_range = "must contain a value in the range {@from} to {@to}.";
const char* const c_str_parm_value_must_be_in_range_from = "{@from}";
const char* const c_str_parm_value_must_be_in_range_to = "{@to}";
const char* const c_str_incorrect_numeric_format = "must have no more than {@wholes} whole number(s) and no more than {@decimals} decimal(s).";
const char* const c_str_parm_incorrect_numeric_format_wholes = "{@wholes}";
const char* const c_str_parm_incorrect_numeric_format_decimals = "{@decimals}";
const char* const c_str_incorrect_numeric_fraction_2 = "must only have .0 or .5 as a fraction (i.e. round to the nearest half).";
const char* const c_str_incorrect_numeric_fraction_4 = "must only have .0, .25, .5 or .75 as a fraction (i.e. round to the nearest quarter).";
const char* const c_str_exceeded_maximum = "has exceeded the maximum size limit of {@limit} characters.";
const char* const c_str_invalid_type = "{@field} is not a valid {@label}.";
const char* const c_str_parm_invalid_type_field = "{@field}";
const char* const c_str_parm_invalid_type_label = "{@label}";
const char* const c_str_invalid_label = "is not a valid label ([_0-9a-zA-Z]+).";
const char* const c_str_invalid_filename = "is not a valid filename (cannot use '\"', ':', '?', '*', '<', '>', '|', '/', '\\' or extended ASCII characters).";
const char* const c_str_invalid_identifier = "is not a valid identifier ([a-zA-Z][_0-9a-zA-Z]*).";
const char* const c_str_invalid_day_type = "{@field1} is not valid for this {@field2}.";
const char* const c_str_parm_invalid_day_type_field = "{@field1}";
const char* const c_str_parm_invalid_day_type_field2 = "{@field2}";
const char* const c_str_invalid_dom_for_yearly = "{@field} is not valid for this month.";
const char* const c_str_parm_invalid_dom_for_yearly_field = "{@field}";
const char* const c_str_invalid_duplicate_field_value = "{@field} must not contain duplicates.";
const char* const c_str_parm_invalid_duplicate_field_value_field = "{@field}";
const char* const c_str_parm_exceeded_maximum_limit = "{@limit}";
const char* const c_str_home_stats = "You currently have {@num} {@type}";
const char* const c_str_parm_home_stats_num = "{@num}";
const char* const c_str_parm_home_stats_type = "{@type}";
const char* const c_str_home_stats_extra = " and {@num} {@type}";
const char* const c_str_parm_home_stats_extra_num = "{@num}";
const char* const c_str_parm_home_stats_extra_type = "{@type}";
const char* const c_str_home_group_stats = "Your {@group} currently has {@num} {@type}";
const char* const c_str_parm_home_group_stats_group = "{@group}";
const char* const c_str_parm_home_group_stats_num = "{@num}";
const char* const c_str_parm_home_group_stats_type = "{@type}";
const char* const c_str_found_child_record = "Found {@class} for {@field}.";
const char* const c_str_parm_found_child_record_class = "{@class}";
const char* const c_str_parm_found_child_record_field = "{@field}";
const char* const c_str_has_one_or_more_active_children = "This {@class} has one or more {@cfield} child {@children}.";
const char* const c_str_parm_has_one_or_more_active_children_class = "{@class}";
const char* const c_str_parm_has_one_or_more_active_children_cfield = "{@cfield}";
const char* const c_str_parm_has_one_or_more_active_children_children = "{@children}";
const char* const c_str_no_reply_email_text = "IMPORTANT: Remember, this is just a notification. Please do not reply to this email.";

#endif

