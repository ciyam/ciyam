// Copyright (c) 2017-2021 CIYAM Developers
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

std::string CIYAM_BASE_DECL_SPEC get_special_var_name( special_var var );

struct CIYAM_BASE_DECL_SPEC system_variable_lock
{
   system_variable_lock( const std::string& name );
   ~system_variable_lock( );

   std::string name;
};

std::string CIYAM_BASE_DECL_SPEC get_raw_system_variable( const std::string& name );
std::string CIYAM_BASE_DECL_SPEC get_system_variable( const std::string& name_or_expr );

void CIYAM_BASE_DECL_SPEC set_system_variable( const std::string& name, const std::string& value );

bool CIYAM_BASE_DECL_SPEC set_system_variable(
 const std::string& name, const std::string& value, const std::string& current );

void CIYAM_BASE_DECL_SPEC list_mutex_lock_ids_for_ciyam_variables( std::ostream& outs );

#endif

