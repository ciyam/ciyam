// Copyright (c) 2004
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

#ifndef MODULE_MANAGEMENT_H
#  define MODULE_MANAGEMENT_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <map>
#     include <vector>
#     include <string>
#     include <iosfwd>
#  endif

#  include "macros.h"
#  include "class_base.h"

#  ifdef CIYAM_BASE_IMPL
#     define MODULE_MANAGEMENT_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define MODULE_MANAGEMENT_DECL_SPEC DYNAMIC_IMPORT
#  endif

enum module_load_error
{
   e_module_load_error_none,
   e_module_load_error_file_does_not_exist,
   e_module_load_error_external_module_failure
};

enum module_unload_error
{
   e_module_unload_error_none,
   e_module_unload_error_name_unknown,
   e_module_unload_error_external_module_failure
};

enum module_class_list_error
{
   e_module_class_list_error_none,
   e_module_class_list_error_name_unknown
};

enum module_string_list_error
{
   e_module_string_list_error_none,
   e_module_string_list_error_name_unknown
};

enum module_class_field_list_error
{
   e_module_class_field_list_error_none,
   e_module_class_field_list_error_name_unknown,
   e_module_class_field_list_error_class_name_unknown
};

class dynamic_library;

module_load_error MODULE_MANAGEMENT_DECL_SPEC load_module( const std::string& module_name );
module_load_error MODULE_MANAGEMENT_DECL_SPEC load_module( const std::string& module_name, const std::string& prefix_name );
module_unload_error MODULE_MANAGEMENT_DECL_SPEC unload_module( const std::string& module_id_or_name );

void MODULE_MANAGEMENT_DECL_SPEC list_modules( std::ostream& os );

module_class_list_error MODULE_MANAGEMENT_DECL_SPEC
 list_module_classes( const std::string& module_id_or_name, std::ostream& os );
module_class_list_error MODULE_MANAGEMENT_DECL_SPEC
 list_module_classes( const std::string& module_id_or_name, std::vector< std::string >& class_list );

module_class_list_error MODULE_MANAGEMENT_DECL_SPEC
 list_module_classes( const std::string& module_id_or_name,
 std::map< std::string, std::string >& class_map, bool key_by_id = false );

module_string_list_error MODULE_MANAGEMENT_DECL_SPEC
 list_module_strings( const std::string& module_id_or_name, std::ostream& os );

module_class_field_list_error MODULE_MANAGEMENT_DECL_SPEC
 list_module_class_fields( const std::string& module_id_or_name, const std::string& class_id_or_name, std::ostream& os );

std::string MODULE_MANAGEMENT_DECL_SPEC get_module_id( const std::string& module_name );

size_t MODULE_MANAGEMENT_DECL_SPEC get_module_ref_count( const std::string& module_id_or_name );

dynamic_library MODULE_MANAGEMENT_DECL_SPEC* get_module_ptr( const std::string& module_id_or_name );

class_base MODULE_MANAGEMENT_DECL_SPEC*
 construct_object( const std::string& module_id_or_name, const std::string& class_id );

void MODULE_MANAGEMENT_DECL_SPEC
 destroy_object( const std::string& module_id_or_name, const std::string& class_id, class_base*& p_cb );

std::string MODULE_MANAGEMENT_DECL_SPEC get_module_id_for_id_or_name( const std::string& module_id_or_name );
std::string MODULE_MANAGEMENT_DECL_SPEC get_module_name_for_id_or_name( const std::string& module_id_or_name );

std::string MODULE_MANAGEMENT_DECL_SPEC
 get_class_id_for_id_or_name( const std::string& module_id_or_name, const std::string& id_or_name );
std::string MODULE_MANAGEMENT_DECL_SPEC
 get_class_name_for_id_or_name( const std::string& module_id_or_name, const std::string& id_or_name );

std::string MODULE_MANAGEMENT_DECL_SPEC get_field_id_for_id_or_name(
 const std::string& module_id_or_name, const std::string& class_id_or_name, const std::string& id_or_name );
std::string MODULE_MANAGEMENT_DECL_SPEC get_field_name_for_id_or_name(
 const std::string& module_id_or_name, const std::string& class_id_or_name, const std::string& id_or_name );

bool MODULE_MANAGEMENT_DECL_SPEC
 get_module_class_has_derivations( const std::string& module_id_or_name, const std::string& class_id );

void MODULE_MANAGEMENT_DECL_SPEC get_foreign_key_info_for_module_class(
 const std::string& module_id_or_name, const std::string& class_id, foreign_key_info_container& foreign_key_info );

const procedure_info_container MODULE_MANAGEMENT_DECL_SPEC&
 get_procedure_info_for_module_class( const std::string& module_id_or_name, const std::string& class_id );

std::string MODULE_MANAGEMENT_DECL_SPEC
 get_sql_columns_for_module_class( const std::string& module_id_or_name, const std::string& class_id );

void MODULE_MANAGEMENT_DECL_SPEC get_sql_indexes_for_module_class(
 const std::string& module_id_or_name, const std::string& class_id, std::vector< std::string >& indexes );

#endif

