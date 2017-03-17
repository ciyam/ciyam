// Copyright (c) 2013-2017 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_FILES_H
#  define CIYAM_FILES_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iosfwd>
#     include <deque>
#     include <string>
#     include <vector>
#  endif

#  include "ptypes.h"

#  ifdef CIYAM_BASE_IMPL
#     define CLASS_BASE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define CLASS_BASE_DECL_SPEC DYNAMIC_IMPORT
#  endif

struct progress;
class tcp_socket;

void CLASS_BASE_DECL_SPEC list_mutex_lock_ids_for_ciyam_files( std::ostream& outs );

size_t CLASS_BASE_DECL_SPEC get_total_files( );
int64_t CLASS_BASE_DECL_SPEC get_total_bytes( );

std::string CLASS_BASE_DECL_SPEC get_file_stats( );

void CLASS_BASE_DECL_SPEC init_files_area( std::vector< std::string >* p_untagged = 0 );

void CLASS_BASE_DECL_SPEC resync_files_area( std::vector< std::string >* p_untagged = 0 );

void CLASS_BASE_DECL_SPEC term_files_area( );

std::string CLASS_BASE_DECL_SPEC current_timestamp_tag( bool truncated = false );

bool CLASS_BASE_DECL_SPEC has_tag( const std::string& name );

bool CLASS_BASE_DECL_SPEC has_file( const std::string& hash, bool check_is_hash = true );

int64_t CLASS_BASE_DECL_SPEC file_bytes( const std::string& hash );

enum file_expansion
{
   e_file_expansion_none,
   e_file_expansion_content,
   e_file_expansion_recursive,
   e_file_expansion_recursive_hashes
};

std::string CLASS_BASE_DECL_SPEC file_type_info( const std::string& tag_or_hash,
 file_expansion expansion = e_file_expansion_none, int max_depth = 0, int indent = 0, bool add_size = false );

std::string CLASS_BASE_DECL_SPEC create_raw_file(
 const std::string& data, bool compress = true, const char* p_tag = 0, bool* p_is_existing = 0 );

std::string CLASS_BASE_DECL_SPEC create_raw_file_with_extras( const std::string& data,
 std::vector< std::pair< std::string, std::string > >& extras, bool compress = true, const char* p_tag = 0 );

void CLASS_BASE_DECL_SPEC tag_del( const std::string& name, bool unlink = false );
void CLASS_BASE_DECL_SPEC tag_file( const std::string& name, const std::string& hash );

std::string CLASS_BASE_DECL_SPEC get_hash_tags( const std::string& hash );

std::string CLASS_BASE_DECL_SPEC tag_file_hash( const std::string& name );

std::string CLASS_BASE_DECL_SPEC list_file_tags( const std::string& pat, size_t max_tags = 0,
 int64_t max_bytes = 0, int64_t* p_min_bytes = 0, std::deque< std::string >* p_hashes = 0 );

void CLASS_BASE_DECL_SPEC remove_file_tags( const std::string& hash, const std::string& pat );

std::string CLASS_BASE_DECL_SPEC hash_with_nonce( const std::string& hash, const std::string& nonce );

void CLASS_BASE_DECL_SPEC fetch_file( const std::string& hash, tcp_socket& socket, progress* p_progress = 0 );

void CLASS_BASE_DECL_SPEC store_file( const std::string& hash,
 tcp_socket& socket, const char* p_tag = 0, progress* p_progress = 0, bool allow_core_file = true );

void CLASS_BASE_DECL_SPEC delete_file( const std::string& hash, bool even_if_tagged = true );

void CLASS_BASE_DECL_SPEC delete_file_tree( const std::string& hash );

void CLASS_BASE_DECL_SPEC delete_files_for_tags( const std::string& pat );

void CLASS_BASE_DECL_SPEC copy_raw_file( const std::string& hash, const std::string& dest_filename );

void CLASS_BASE_DECL_SPEC fetch_temp_file(
 const std::string& name, tcp_socket& socket, progress* p_progress = 0 );

void CLASS_BASE_DECL_SPEC store_temp_file(
 const std::string& name, tcp_socket& socket, progress* p_progress = 0 );

bool CLASS_BASE_DECL_SPEC temp_file_is_identical( const std::string& temp_name, const std::string& hash );

std::string CLASS_BASE_DECL_SPEC extract_file(
 const std::string& hash, const std::string& dest_filename, unsigned char check_file_type_and_extra = '\0' );

void CLASS_BASE_DECL_SPEC add_file_archive(
 const std::string& name, const std::string& path, int64_t size_limit );

void CLASS_BASE_DECL_SPEC remove_file_archive( const std::string& name, bool destroy_files = false );
void CLASS_BASE_DECL_SPEC repair_file_archive( const std::string& name );

void CLASS_BASE_DECL_SPEC archives_status_update( );

std::string CLASS_BASE_DECL_SPEC list_file_archives( bool minimal = false,
 std::vector< std::string >* p_paths = 0, int64_t min_avail = 0, bool stop_after_first = false );

std::string CLASS_BASE_DECL_SPEC relegate_timestamped_files( const std::string& hash,
 const std::string& archive, uint32_t max_files, int64_t max_bytes, bool delete_files_always = false );

std::string CLASS_BASE_DECL_SPEC retrieve_file_from_archive( const std::string& hash, const std::string& tag );

#endif

