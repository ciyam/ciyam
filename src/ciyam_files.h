// Copyright (c) 2013 CIYAM Open Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_FILES_H
#  define CIYAM_FILES_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "ptypes.h"

#  ifdef CIYAM_BASE_IMPL
#     define CLASS_BASE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define CLASS_BASE_DECL_SPEC DYNAMIC_IMPORT
#  endif

class tcp_socket;

size_t CLASS_BASE_DECL_SPEC get_total_files( );
int64_t CLASS_BASE_DECL_SPEC get_total_bytes( );

std::string CLASS_BASE_DECL_SPEC get_file_stats( );

void CLASS_BASE_DECL_SPEC init_files_area( );

void CLASS_BASE_DECL_SPEC init_file( const std::string& name, const std::string& data );

void CLASS_BASE_DECL_SPEC fetch_file( const std::string& name, tcp_socket& socket );
void CLASS_BASE_DECL_SPEC store_file( const std::string& name, tcp_socket& socket );

#endif
