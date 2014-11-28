// Copyright (c) 2013-2014 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_FILES_H
#  define CIYAM_FILES_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     include <vector>
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

bool CLASS_BASE_DECL_SPEC has_tag( const std::string& name );

bool CLASS_BASE_DECL_SPEC has_file( const std::string& hash );

int64_t CLASS_BASE_DECL_SPEC file_bytes( const std::string& hash );

enum file_expansion
{
   e_file_expansion_none,
   e_file_expansion_content,
   e_file_expansion_recursive
};

std::string CLASS_BASE_DECL_SPEC file_type_info(
 const std::string& tag_or_hash, file_expansion expansion = e_file_expansion_none, int max_depth = 0, int indent = 0 );

std::string CLASS_BASE_DECL_SPEC create_raw_file( const std::string& data, const char* p_tag = 0, bool* p_is_existing = 0 );

std::string CLASS_BASE_DECL_SPEC create_raw_file_with_extras(
 const std::string& data, std::vector< std::pair< std::string, std::string > >& extras, const char* p_tag = 0 );

void CLASS_BASE_DECL_SPEC tag_del( const std::string& name );
void CLASS_BASE_DECL_SPEC tag_file( const std::string& name, const std::string& hash );

std::string CLASS_BASE_DECL_SPEC get_hash_tags( const std::string& hash );

std::string CLASS_BASE_DECL_SPEC tag_file_hash( const std::string& name );

std::string CLASS_BASE_DECL_SPEC list_file_tags( const std::string& pat );

std::string CLASS_BASE_DECL_SPEC hash_with_nonce( const std::string& hash, const std::string& nonce );

void CLASS_BASE_DECL_SPEC fetch_file( const std::string& hash, tcp_socket& socket );
void CLASS_BASE_DECL_SPEC store_file( const std::string& hash, tcp_socket& socket, const char* p_tag = 0 );

void CLASS_BASE_DECL_SPEC delete_file( const std::string& hash );

void CLASS_BASE_DECL_SPEC fetch_temp_file( const std::string& name, tcp_socket& socket );
void CLASS_BASE_DECL_SPEC store_temp_file( const std::string& name, tcp_socket& socket );

bool CLASS_BASE_DECL_SPEC temp_file_is_identical( const std::string& temp_name, const std::string& hash );

std::string CLASS_BASE_DECL_SPEC extract_file(
 const std::string& hash, const std::string& dest_filename, unsigned char check_file_type_and_extra = '\0' );

#endif

