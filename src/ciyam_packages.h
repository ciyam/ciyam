// Copyright (c) 2017-2018 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_PACKAGES_H
#  define CIYAM_PACKAGES_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "macros.h"

#  ifdef CIYAM_BASE_IMPL
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_IMPORT
#  endif

void CIYAM_BASE_DECL_SPEC export_package( const std::string& module,
 const std::string& mclass, const std::string& key, const std::string& exclude_info,
 const std::string& test_info, const std::string& include_info, const std::string& filename );

void CIYAM_BASE_DECL_SPEC import_package(
 const std::string& module, const std::string& uid,
 const std::string& dtm, const std::string& filename,
 const std::string& key_prefix, const std::string& replace_info,
 const std::string& skip_field_info, bool new_only = false, bool for_remove = false );

#endif

