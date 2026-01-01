// Copyright (c) 2025-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_BASE_OPS_H
#  define CIYAM_BASE_OPS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <set>
#     include <string>
#  endif

#  include "ptypes.h"
#  include "ciyam_core.h"

class class_base;
class command_handler;

enum instance_op
{
   e_instance_op_none,
   e_instance_op_review,
   e_instance_op_create,
   e_instance_op_update,
   e_instance_op_destroy
};

enum instance_op_rc
{
   e_instance_op_rc_okay,
   e_instance_op_rc_locked,
   e_instance_op_rc_invalid,
   e_instance_op_rc_not_found,
   e_instance_op_rc_constrained,
   e_instance_op_rc_child_locked,
   e_instance_op_rc_already_exists
};

class mutex;

mutex& get_mutex_for_ciyam_base_ops( );

std::string exec_bulk_ops( const std::string& module,
 const std::string& uid, const std::string& dtm, const std::string& mclass,
 const std::string& filename, const std::string& export_fields, const std::string& tz_name, bool destroy_records,
 const std::string& search_text, const std::string& search_query, const std::string& fixed_field_values, command_handler& handler );

void begin_instance_op( instance_op op,
 class_base& instance, const std::string& key, bool internal_operation = true, instance_op_rc* p_rc = 0 );

void finish_instance_op( class_base& instance, bool apply_changes,
 bool internal_operation = true, instance_op_rc* p_rc = 0, std::set< std::string >* p_fields_set = 0 );

bool perform_instance_check( class_base& instance, const std::string& key );

void perform_instance_fetch(
 class_base& instance, const std::string& key_info,
 instance_fetch_rc* p_rc = 0, bool only_sys_fields = false, bool do_not_use_cache = false );

bool perform_instance_iterate( class_base& instance,
 const std::string& key_info, const std::string& fields, const std::string& text,
 const std::string& query, iter_direction direction, bool inclusive = true, int row_limit = 0,
 sql_optimisation optimisation = e_sql_optimisation_none, const std::set< std::string >* p_filters = 0 );

bool perform_instance_iterate_next( class_base& instance );

#endif
