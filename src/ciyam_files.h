// Copyright (c) 2013-2026 CIYAM Developers
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

#  include "common.h"
#  include "ptypes.h"
#  include "ciyam_core.h"

struct progress;

class date_time;
class tcp_socket;

class mutex;

size_t get_total_files( );
int64_t get_total_bytes( );

std::string get_file_stats( );

mutex& get_mutex_for_ciyam_files( );

void init_files_area( progress* p_progress = 0, bool remove_invalid_tags = false );

void term_files_area( );

void resync_files_area( progress* p_progress = 0, bool remove_invalid_tags = false );

void init_archive_info( progress* p_progress = 0, bool has_restored_system = false );

void resync_archive_info( progress* p_progress = 0 );

std::string current_time_stamp_tag( bool truncated = false, size_t days_ahead = 0 );

bool has_tag( const std::string& name, file_type type = e_file_type_any );

bool has_file( const std::string& hash, bool check_is_hash = true, bool* p_is_in_archive = 0 );

bool is_core_file( const std::string& hash );

bool is_list_file( unsigned char ch );

bool is_list_file( const std::string& hash, bool* p_is_encrypted = 0 );

bool is_encrypted_file( const std::string& hash, bool* p_is_list = 0 );

int64_t file_bytes( const std::string& hash, bool blobs_for_lists = false );

enum file_expansion
{
   e_file_expansion_none,
   e_file_expansion_content,
   e_file_expansion_recursive,
   e_file_expansion_recursive_hashes
};

std::string file_type_info(
 const std::string& tag_or_hash, file_expansion expansion = e_file_expansion_none,
 int max_depth = 0, int indent = 0, bool add_size = false, const char* p_prefix = 0,
 bool allow_all_after = true, bool output_total_blob_size = false, progress* p_progress = 0,
 date_time* p_dtm = 0, size_t* p_total = 0, bool ignore_not_found = false );

enum file_total_type
{
   e_file_total_type_all_items,
   e_file_total_type_blobs_only,
   e_file_total_type_encrypted_only,
   e_file_total_type_repository_entries
};

void file_list_item_pos(
 const std::string& repository, const std::string& tag_or_hash, size_t& total,
 file_total_type total_type, const std::string& item_hash, size_t& item_pos, bool recurse = false,
 progress* p_progress = 0, date_time* p_dtm = 0, bool is_hidden = false, bool is_top_level = true );

std::string create_raw_file(
 const std::string& data, bool compress = true, const char* p_tag = 0,
 bool* p_is_existing = 0, const char* p_hash = 0, bool allow_uncompress = true, bool allow_missing_items = false );

std::string create_list_file( const std::string& add_tags, const std::string& del_items,
 bool sort_items, const std::string& tag_or_hash, const std::string& new_tag, const std::string& old_tag );

std::string create_list_tree( const std::string& add_tags, const std::string& del_items,
 bool sort_items, const std::string& tag_and_branches, const std::string& new_tag, const std::string& old_tag );

void tag_del( const std::string& name, bool unlink = false, bool auto_tag_with_time = true );

void tag_file( const std::string& name,
 const std::string& hash, bool skip_tag_del = false, bool is_external = false );

void touch_file( const std::string& hash,
 const std::string& archive, bool set_archive_path = true, bool* p_has_updated_archive = 0 );

std::string get_hash( const std::string& prefix );

bool has_any_tags( const std::string& hash );

std::string get_hash_tags( const std::string& hash );

std::string tag_file_hash( const std::string& name, bool* p_rc = 0 );

std::string extract_tags_from_lists(
 const std::string& tag_or_hash, const std::string& prefix,
 int depth = 1, int level = 0, progress* p_progress = 0, date_time* p_dtm = 0, size_t* p_total = 0 );

std::string list_file_tags(
 const std::string& pat, const char* p_excludes = 0,
 size_t max_tags = 0, int64_t max_bytes = 0, int64_t* p_min_bytes = 0,
 std::deque< std::string >* p_hashes = 0, bool include_multiples = true,
 progress* p_progress = 0, date_time* p_dtm = 0 );

void remove_file_tags( const std::string& hash, const std::string& pat );

std::string hash_with_nonce( const std::string& hash, const std::string& nonce );

enum crypt_target
{
   e_crypt_target_all,
   e_crypt_target_blobs_only,
   e_crypt_target_blobs_only_repo
};

enum crypt_operation
{
   e_crypt_operation_decrypt,
   e_crypt_operation_encrypt,
   e_crypt_operation_recrypt,
   e_crypt_operation_reverse
};

void crypt_file( const std::string& repository,
 const std::string& tag_or_hash, const std::string& password, bool recurse = false,
 crypt_target target = e_crypt_target_all, progress* p_progress = 0, date_time* p_dtm = 0,
 size_t* p_total = 0, crypt_operation operation = e_crypt_operation_reverse,
 std::set< std::string >* p_files_processed = 0, stream_cipher cipher = e_stream_cipher_chacha20 );

void fetch_file( const std::string& hash,
 tcp_socket& socket, progress* p_sock_progress = 0, unsigned char* p_file_type = 0 );

bool store_file( const std::string& hash,
 tcp_socket& socket, const char* p_tag = 0, progress* p_sock_progress = 0,
 bool allow_core_file = true, size_t max_bytes = 0, bool allow_missing_items = false,
 std::string* p_file_data = 0, size_t* p_total_bytes = 0, progress* p_progress = 0, std::string* p_raw_file_data = 0 );

void delete_file( const std::string& hash, bool even_if_tagged = true, bool ignore_not_found = false );

void delete_file_tree( const std::string& hash, progress* p_progress = 0 );

void delete_files_for_tags( const std::string& pat, progress* p_progress = 0, const char* p_excludes = 0 );

void copy_raw_file( const std::string& hash, const std::string& dest_file_name );

void store_temp_file( const std::string& hash,
 const std::string& name, tcp_socket& socket, progress* p_progress = 0, bool is_existing = false );

bool temp_file_is_identical( const std::string& temp_name, const std::string& hash );

std::string extract_file( const std::string& hash,
 const std::string& dest_file_name, unsigned char check_file_type_and_extra = '\0',
 bool* p_is_list = 0, unsigned char* p_type_and_extra = 0, bool* p_is_encrypted = 0, bool set_is_encrypted = false );

class system_ods_fs_guard
{
   public:
   system_ods_fs_guard( );
   ~system_ods_fs_guard( );

   private:
   struct impl;
   impl* p_impl;
};

void add_file_archive(
 const std::string& name, const std::string& path, int64_t size_limit );

void clear_file_archive( const std::string& name );

void remove_file_archive( const std::string& name,
 bool destroy_files = false, bool remove_directory = false, progress* p_progress = 0 );

void repair_file_archive( const std::string& name, progress* p_progress = 0 );

void resize_file_archive( const std::string& name,
 int64_t new_size_limit, progress* p_progress = 0, int64_t* p_bytes_used = 0 );

void archives_status_update( const std::string& name );

bool file_has_been_blacklisted( const std::string& hash );

bool has_file_archive( const std::string& archive, std::string* p_path = 0 );

enum archive_list_type
{
   e_archive_list_type_full,
   e_archive_list_type_minimal,
   e_archive_list_type_path_only
};

std::string list_file_archives( archive_list_type list_type = e_archive_list_type_full,
 std::vector< std::string >* p_paths = 0, int64_t min_avail = 0, bool stop_after_first = false, const std::string* p_name = 0 );

void create_raw_file_in_archive(
 const std::string& archive, const std::string& hash, const std::string& file_data, std::string* p_hash = 0 );

std::string relegate_one_or_num_oldest_files( const std::string& hash,
 const std::string& archive, uint32_t max_files, int64_t max_bytes, bool delete_files_always = false );

size_t files_in_archive( const std::string& archive );

bool has_file_been_archived( const std::string& hash,
 std::string* p_archive_name = 0, bool in_specific_archive_only = false );

std::string retrieve_file_from_archive(
 const std::string& hash, const std::string& tag, size_t days_ahead = 0 );

bool touch_file_in_archive( const std::string& hash, const std::string& archive );

void delete_file_from_archive(
 const std::string& hash, const std::string& archive, bool add_to_blacklist = false );

bool has_repository_entry_record( const std::string& repository, const std::string& hash );

bool fetch_repository_entry_record(
 const std::string& repository, const std::string& hash, std::string& local_hash, bool must_exist = true );

bool fetch_repository_entry_record( const std::string& repository, const std::string& hash,
 std::string& local_hash, std::string& local_public_key, std::string& master_public_key, bool must_exist = true );

void store_repository_entry_record( const std::string& repository, const std::string& hash,
 const std::string& local_hash, const std::string& local_public_key, const std::string& master_public_key );

bool destroy_repository_entry_record(
 const std::string& repository, const std::string& hash, bool must_exist = true );

size_t count_total_repository_entries(
 const std::string& repository, date_time* p_dtm = 0, progress* p_progress = 0 );

size_t remove_all_repository_entries(
 const std::string& repository, date_time* p_dtm, progress* p_progress = 0, bool set_session_progress = false );

size_t remove_obsolete_repository_entries(
 const std::string& repository, date_time* p_dtm = 0, progress* p_progress = 0, bool set_session_progress = false );

#endif
