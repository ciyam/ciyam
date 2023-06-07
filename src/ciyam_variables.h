// Copyright (c) 2017-2023 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_VARIABLES_H
#  define CIYAM_VARIABLES_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iosfwd>
#     include <string>
#  endif

#  include "macros.h"
#  include "ptypes.h"
#  include "ciyam_common.h"

#  ifdef CIYAM_BASE_IMPL
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_IMPORT
#  endif

struct progress;

std::string CIYAM_BASE_DECL_SPEC get_special_var_name( special_var var );

struct CIYAM_BASE_DECL_SPEC system_variable_lock
{
   system_variable_lock( const std::string& name );
   ~system_variable_lock( );

   std::string name;
};

std::string CIYAM_BASE_DECL_SPEC get_raw_system_variable( const std::string& name );
std::string CIYAM_BASE_DECL_SPEC get_system_variable( const std::string& name_or_expr );

void CIYAM_BASE_DECL_SPEC set_system_variable( const std::string& name,
 const std::string& value, bool is_init = false, progress* p_progress = 0 );

bool CIYAM_BASE_DECL_SPEC set_system_variable(
 const std::string& name, const std::string& value,
 const std::string& current, progress* p_progress = 0, const char append_separator = '\0' );

enum variable_check_type
{
   e_variable_check_type_no_session_has,
   e_variable_check_type_any_session_has,
   e_variable_check_type_has_other_system,
   e_variable_check_type_not_has_other_system
};

struct set_variable_checker
{
   set_variable_checker( variable_check_type check_type,
    const std::string& variable_name, set_variable_checker* p_additional_check = 0 )
    :
    check_type( check_type ),
    variable_name( variable_name ),
    p_additional_check( p_additional_check )
   {
   }

   bool can_set( ) const;

   std::string variable_name;
   variable_check_type check_type;
   set_variable_checker* p_additional_check;
};

bool CIYAM_BASE_DECL_SPEC set_system_variable(
 const std::string& name, const std::string& value,
 set_variable_checker& checker, bool is_init = false, progress* p_progress = 0 );

struct temporary_system_variable
{
   temporary_system_variable( const std::string& name, const std::string& value )
    :
    name( name )
   {
      original_value = get_system_variable( name );
      set_system_variable( name, value );
   }

   ~temporary_system_variable( )
   {
      set_system_variable( name, original_value );
   }

   std::string name;
   std::string original_value;
};

std::string CIYAM_BASE_DECL_SPEC variable_name_from_name_and_value( const std::string& name_and_value, std::string* p_value = 0 );

class mutex;

mutex& CIYAM_BASE_DECL_SPEC get_mutex_for_ciyam_variables( );

#endif

