// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2022 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_STRINGS_H
#  define CIYAM_STRINGS_H

const char* const c_str_page = "Page";
const char* const c_str_true = "True";
const char* const c_str_false = "False";
const char* const c_str_total = "Total";
const char* const c_str_footer = "© Copyright (c) 2012-2022 CIYAM Developers";
const char* const c_str_records = "record(s)";
const char* const c_str_subtotal = "Subtotal";
const char* const c_str_module_is_loaded = "Module '{@module}' is already loaded.";
const char* const c_str_parm_module_is_loaded_module = "{@module}";
const char* const c_str_module_not_loaded = "Module '{@module}' is not currently loaded.";
const char* const c_str_parm_module_not_loaded_module = "{@module}";
const char* const c_str_module_not_exists = "Module '{@module}' does not appear to exist.";
const char* const c_str_parm_module_not_exists_module = "{@module}";
const char* const c_str_module_depends_reqd = "Module '{@module}' requires external dependencies to be loaded first.";
const char* const c_str_parm_module_depends_reqd_module = "{@module}";
const char* const c_str_key_invalid = "Key '{@key}' contains invalid characters.";
const char* const c_str_parm_key_invalid_key = "{@key}";
const char* const c_str_record_exists = "An existing {@class} record with the key '{@key}' was found.";
const char* const c_str_parm_record_exists_class = "{@class}";
const char* const c_str_parm_record_exists_key = "{@key}";
const char* const c_str_cannot_update = "Update not permitted for this {@class} record.";
const char* const c_str_parm_cannot_update_class = "{@class}";
const char* const c_str_cannot_destroy = "Destroy not permitted for this record.";
const char* const c_str_parm_cannot_destroy_class = "{@class}";
const char* const c_str_version_mismatch = "Version mismatch (found {@found} but expected {@expected}).";
const char* const c_str_parm_version_mismatch_found = "{@found}";
const char* const c_str_parm_version_mismatch_expected = "{@expected}";
const char* const c_str_permission_denied = "Permission denied.";
const char* const c_str_record_constrained = "This record cannot be destroyed as it is being used by one or more {@class} records.";
const char* const c_str_parm_record_constrained_class = "{@class}";
const char* const c_str_index_duplicate = "This {@field} [key: {@value}] is already being used by another {@class} record.";
const char* const c_str_parm_index_duplicate_field = "{@field}";
const char* const c_str_parm_index_duplicate_value = "{@value}";
const char* const c_str_parm_index_duplicate_class = "{@class}";
const char* const c_str_record_not_found = "{@class} record '{@key}' was not found.";
const char* const c_str_parm_record_not_found_class = "{@class}";
const char* const c_str_parm_record_not_found_key = "{@key}";
const char* const c_str_field_is_incorrect = "{@field} is incorrect.";
const char* const c_str_parm_field_is_incorrect_field = "{@field}";
const char* const c_str_external_service_unavailable = "External service {@symbol} does not appear to be currently available.";
const char* const c_str_parm_external_service_unavailable_symbol = "{@symbol}";

#endif

