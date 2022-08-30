// Copyright (c) 2013-2022 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_CONSTANTS_H
#  define CIYAM_CONSTANTS_H

const int c_tx_id_module = 1;
const int c_tx_id_initial = 2;
const int c_tx_id_standard = 5;

const size_t c_bc_identity_length = 9;

const size_t c_bc_scaling_value = 50;
const size_t c_bc_scaling_demo_value = 5;

const char* const c_none = "none";
const char* const c_okay = "okay";

const char* const c_true = "true";
const char* const c_false = "false";

const char* const c_true_value = "1";
const char* const c_false_value = "0";

const char* const c_dummy = "dummy";

const char* const c_ciyam_tag = "ciyam";
const char* const c_ciyam_logs_tag = "ciyam.logs";

const char* const c_bc_prefix = "bc.";

const char* const c_blk_suffix = ".blk";
const char* const c_dat_suffix = ".dat";
const char* const c_key_suffix = ".key";
const char* const c_pub_suffix = ".pub";
const char* const c_sig_suffix = ".sig";
const char* const c_zenith_suffix = ".zenith";

const char* const c_repository_info_suffix = ".info";

const char* const c_log_file_ext = ".log";
const char* const c_txs_file_ext = ".txs";

const char* const c_ciyam_script = "ciyam_script";
const char* const c_ciyam_server = "ciyam_server";

const char* const c_demo_identity = "e3733d121";

const char* const c_variables_branch = "Variables";

const char* const c_meta_model_name = "Meta";

const char* const c_meta_class_name_class = "Class";
const char* const c_meta_class_name_model = "Model";
const char* const c_meta_class_name_application = "Application";

const char* const c_ignore_field = "@ignore";

const char* const c_null_ip_addr = "0.0.0.0";
const char* const c_null_ip_addr_for_ipv6 = "::/0.";

const char* const c_local_host = "localhost";

const char* const c_local_ip_addr = "127.0.0.1";
const char* const c_local_ip_addr_for_ipv6 = "::1";

const char* const c_timezones_file = "timezones.sio";

const char* const c_autoscript_file = "autoscript.sio";
const char* const c_manuscript_file = "manuscript.sio";

const char* const c_file_zero_length = "*";

const char* const c_file_archives_folder = "arch";
const char* const c_file_blacklist_folder = "blst";
const char* const c_file_peerchain_folder = "peer";
const char* const c_file_repository_folder = "repo";
const char* const c_system_variables_folder = "vars";

const char* const c_file_repository_meta_data_line_prefix = "md:";
const char* const c_file_repository_public_key_line_prefix = "pk:";
const char* const c_file_repository_source_hash_line_prefix = "sh:";
const char* const c_file_repository_target_hash_line_prefix = "th:";

const char* const c_file_repository_meta_data_info_type_raw = "raw";

const char* const c_file_repository_local_hash_attribute = "local_hash";

const char* const c_storage_identity_tx_id = "[0]";

const char* const c_block_prefix = "block";
const char* const c_files_directory = "files";

const char* const c_file_copy_command = "file_copy";
const char* const c_file_kill_command = "file_kill";

const char* const c_application_script_prefix = "app_script:";

const size_t c_peer_sleep_time = 500; // i.e. 0.5 secs

const size_t c_file_transfer_line_timeout = 20000; // i.e. 20 secs
const size_t c_file_transfer_initial_timeout = 25000; // i.e. 25 secs

const size_t c_file_transfer_max_line_size = 100000; // i.e. 100K

const size_t c_files_area_item_max_num_default = 100000; // i.e. 100K
const size_t c_files_area_item_max_size_default = 1000000; // i.e. 1MB

const int c_file_type_val_blob = 0x01;
const int c_file_type_val_list = 0x02;

const int c_file_type_val_mask = 0x03;

const int c_file_type_val_encrypted = 0x04;
const int c_file_type_val_compressed = 0x08;

const int c_file_type_val_extra_core = 0x10;
const int c_file_type_val_extra_mime = 0x20;

const int c_file_type_val_extra_mask = 0x30;

const int c_file_type_val_no_encrypt = 0x40;
const int c_file_type_val_no_compress = 0x80;

const char c_file_type_char_blob = '\x01';
const char c_file_type_char_list = '\x02';

const char c_file_type_char_core_blob = '\x11';
const char c_file_type_char_core_list = '\x12';

const char c_file_type_char_mime_blob = '\x21';

const char c_file_type_char_encrypted = '\x04';
const char c_file_type_char_compressed = '\x08';

const char c_file_type_char_blob_encrypted = '\x05';

const char c_file_type_char_blob_compressed = '\x09';
const char c_file_type_char_list_compressed = '\x0a';

const char c_file_type_char_blob_no_compress = '\x81';

const char c_file_type_char_core_blob_compressed = '\x19';
const char c_file_type_char_core_list_compressed = '\x1a';

const char c_file_type_char_mime_blob_compressed = '\x29';

const char* const c_file_type_str_blob = "\x01";
const char* const c_file_type_str_list = "\x02";

const char* const c_file_type_str_core_blob = "\x11";
const char* const c_file_type_str_core_list = "\x12";

const char* const c_file_type_str_blob_encrypted = "\x05";
const char* const c_file_type_str_blob_no_compress = "\x81";

const char* const c_file_type_core_block_object = "blk";

const char* const c_file_type_core_block_header_height_prefix = "h=";
const char* const c_file_type_core_block_header_identity_prefix = "i=";
const char* const c_file_type_core_block_header_num_tree_items_prefix = "n=";
const char* const c_file_type_core_block_header_version_number_prefix = "v=";

const char* const c_file_type_core_block_detail_hind_hash_prefix = "h:";
const char* const c_file_type_core_block_detail_last_hashes_prefix = "l:";
const char* const c_file_type_core_block_detail_pubkey_hashes_prefix = "p:";
const char* const c_file_type_core_block_detail_tree_root_hash_prefix = "t:";
const char* const c_file_type_core_block_detail_signature_file_hash_prefix = "s:";
const char* const c_file_type_core_block_detail_unix_block_time_value_prefix = "u:";

const char* const c_sys = "sys";
const char* const c_auto = "auto";
const char* const c_init = "init";
const char* const c_admin = "admin";

const char* const c_both = "both";
const char* const c_create = "create";
const char* const c_update = "update";

const char* const c_anyone = "anyone";
const char* const c_admin_only = "admin_only";
const char* const c_owner_only = "owner_only";
const char* const c_admin_owner = "admin_owner";
const char* const c_denied_always = "denied_always";

const char* const c_user_class_suffix = "_user_class";

const char* const c_response_okay = "(okay)";
const char* const c_response_okay_more = "(okay more)";
const char* const c_response_okay_skip = "(okay skip)";

const char* const c_response_not_found = "(not found)";

const char* const c_response_error_prefix = "(error)";
const char* const c_response_message_prefix = "(message)";

const char* const c_special_variable_queue_prefix = "@queue_";
const char* const c_special_variable_mapped_prefix = "@mapped_";

#endif
