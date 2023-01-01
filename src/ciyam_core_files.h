// Copyright (c) 2014-2023 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_CORE_FILES_H
#  define CIYAM_CORE_FILES_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iosfwd>
#     include <string>
#  endif

#  include "ptypes.h"

#  ifdef CIYAM_BASE_IMPL
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_IMPORT
#  endif

void CIYAM_BASE_DECL_SPEC list_mutex_lock_ids_for_ciyam_core_files( std::ostream& outs );

void CIYAM_BASE_DECL_SPEC verify_core_file( const std::string& content, bool check_sigs = true );

std::string CIYAM_BASE_DECL_SPEC verified_hash_from_signature(
 const std::string& public_key_hash, const std::string& signature_hash );

bool CIYAM_BASE_DECL_SPEC is_block( const std::string& core_type );

std::string CIYAM_BASE_DECL_SPEC construct_blob_for_block_content( const std::string& block_content );

std::string CIYAM_BASE_DECL_SPEC create_peer_repository_entry_pull_info(
 const std::string& repository, const std::string& hash, bool store_as_blob = true );

std::string CIYAM_BASE_DECL_SPEC create_peer_repository_entry_pull_info(
 const std::string& repository, const std::string& hash, const std::string& local_hash,
 const std::string& local_public_key, const std::string& master_public_key, bool store_as_blob = true );

std::string CIYAM_BASE_DECL_SPEC create_peer_repository_entry_push_info(
 const std::string& file_hash, const std::string& password,
 std::string* p_pub_key = 0, bool store_as_blob = true, bool is_for_testing = false );

void CIYAM_BASE_DECL_SPEC decrypt_pulled_peer_file(
 const std::string& dest_hash, const std::string& src_hash,
 const std::string& password, bool is_for_testing = false, const std::string* p_file_data = 0,
 const std::string* p_encrypted_hash = 0, const std::string* p_archive = 0 );

void CIYAM_BASE_DECL_SPEC decrypt_pulled_peer_file(
 const std::string& dest_hash, const std::string& src_hash,
 const std::string& password, const std::string& public_key_in_hex, bool is_for_testing = false,
 const std::string* p_file_data = 0, const std::string* p_encrypted_hash = 0, const std::string* p_archive = 0 );

#endif
