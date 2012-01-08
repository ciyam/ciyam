// Copyright (c) 2010
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

#ifndef CAT_STRINGS_H
#  define CAT_STRINGS_H

const char* const c_str_page = "Page";
const char* const c_str_true = "True";
const char* const c_str_false = "False";
const char* const c_str_total = "Total";
const char* const c_str_footer = "© Copyright 2010 CIYAM Pty. Ltd.";
const char* const c_str_records = "records";
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

#endif

