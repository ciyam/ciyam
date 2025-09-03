// Copyright (c) 2013-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_CONSTANTS_H
#  define CIYAM_CONSTANTS_H

const int c_tx_id_module = 1;
const int c_tx_id_initial = 2;
const int c_tx_id_standard = 5;

const int c_num_sys_field_names = 5;

const size_t c_max_extras = 9;
const size_t c_chunk_digits = 6;

const size_t c_identity_length = 7;
const size_t c_encrypted_length = 32;

const size_t c_bc_identity_length = 9;

const size_t c_bc_scaling_value = 50;
const size_t c_bc_scaling_test_value = 5;

const size_t c_secret_reserve_size = 256;

const size_t c_key_rounds_multiplier = 30;

const size_t c_auto_script_msleep = 1000; // i.e. 1 sec

const char c_persist_variable_prefix = '>';
const char c_restore_variable_prefix = '<';

const char* const c_none = "none";
const char* const c_okay = "okay";

const char* const c_true = "true";
const char* const c_false = "false";

const char* const c_true_value = "1";
const char* const c_false_value = "0";

const char* const c_dummy = "dummy";
const char* const c_error = "error";

const char* const c_ellipsis = "...";

const char* const c_watching = "[watching]";
const char* const c_finishing = "[finishing]";
const char* const c_suspended = "[suspended]";

const char* const c_cmd_fetch = "pf";
const char* const c_cmd_create = "pc";
const char* const c_cmd_update = "pu";
const char* const c_cmd_destroy = "pd";
const char* const c_cmd_execute = "pe";

const char* const c_perms_r_r = "r--r-----";
const char* const c_perms_rw_rw_r = "rw-rw-r--";

const char c_hidden_file_prefix = '.';

const char c_key_exchange_suffix = '*';

const char c_notifier_ignore_char = '!';
const char c_notifier_select_char = '#';
const char c_notifier_unselect_char = '=';
const char c_notifier_mod_ignore_char = '+';
const char c_notifier_new_ignore_char = '-';
const char c_notifier_mod_sel_ignore_char = '*';
const char c_notifier_new_sel_ignore_char = '^';

const char* const c_notifier_prefix = "@:";
const char* const c_notifier_selection = "#";

const char* const c_notifier_viewed_suffix = "viewed";

const size_t c_notifer_check_wait = 50; // i.e. 1/20 sec
const size_t c_max_notifer_checks = 100;

const char* const c_ciyam_tag = "ciyam";

const char* const c_bc_prefix = "bc.";

const char* const c_blk_suffix = ".blk";
const char* const c_key_suffix = ".key";
const char* const c_opl_suffix = ".opl";
const char* const c_pub_suffix = ".pub";
const char* const c_put_suffix = ".put";
const char* const c_sig_suffix = ".sig";

const char* const c_master_suffix = ".master";
const char* const c_shared_suffix = ".shared";
const char* const c_zenith_suffix = ".zenith";

const char* const c_identity_suffix = "identity";

const char* const c_dead_keys_suffix = ".dead_keys.lst";
const char* const c_demo_keys_suffix = ".demo_keys.lst";

const char* const c_repository_info_suffix = ".info";

const char* const c_two_spaces = "  ";

const char* const c_backup = "backup";
const char* const c_restore = "restore";

const char* const c_files_ext = ".files";

const char* const c_csv_file_ext = ".csv";
const char* const c_log_file_ext = ".log";
const char* const c_old_file_ext = ".old";
const char* const c_sav_file_ext = ".sav";

const char* const c_ciyam_pem = "ciyam.pem";

const char* const c_ciyam_script = "ciyam_script";
const char* const c_ciyam_server = "ciyam_server";

const char* const c_ciyam_interface = "ciyam_interface";

const char* const c_ciyam_dummy_date = "20211202";

const char* const c_ciyam_dummy_unix_time = "9999999999";

const char* const c_ui_class_marker = "CLASS";
const char* const c_ui_group_marker = "GROUP";
const char* const c_ui_class_changing = "ui_CLASS_changing";
const char* const c_ui_group_changing = "ui_GROUP_changing";

const char* const c_test_backup_identity = "e3733d121";
const char* const c_test_shared_identity = "121d3373e";

const char* const c_variables_branch = "Variables";
const char* const c_web_files_branch = "Web_Files";

const char* const c_invalid_name_chars = "<>\"|&\\";

const char* const c_key_field_name = "Key_";
const char* const c_rev_field_name = "Rev_";
const char* const c_sec_field_name = "Sec_";
const char* const c_typ_field_name = "Typ_";
const char* const c_ver_field_name = "Ver_";

const char* const c_create_lock_name = "create";
const char* const c_review_lock_name = "review";
const char* const c_update_lock_name = "update";
const char* const c_destroy_lock_name = "destroy";

const char* const c_meta_model_name = "Meta";

const char* const c_meta_storage_name = "Meta";
const char* const c_ciyam_storage_name = "ciyam";

const char* const c_meta_app_directory = "meta";

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

const char* const c_password_info_file = "password.info";

const char* const c_env_var_ciyam_user = "CIYAM_USER";

const char* const c_notifier_none = "none";
const char* const c_notifier_created = "created";
const char* const c_notifier_deleted = "deleted";
const char* const c_notifier_modified = "modified";
const char* const c_notifier_moved_from = "moved_from";
const char* const c_notifier_modified_from = "modified_from";

const char* const c_file_zero_length = "*";

const char* const c_system_archives_folder = "arch";
const char* const c_system_blacklist_folder = "blst";
const char* const c_system_datachain_folder = "data";
const char* const c_system_peerchain_folder = "peer";
const char* const c_system_variables_folder = "vars";
const char* const c_system_repository_folder = "repo";

const char* const c_storage_folder_name_dot_dat = ".dat";
const char* const c_storage_folder_name_dot_idx = ".idx";
const char* const c_storage_folder_name_modules = "modules";
const char* const c_storage_folder_name_channels = "channels";
const char* const c_storage_folder_name_gid_data = "gid_data";
const char* const c_storage_folder_name_uid_data = "uid_data";
const char* const c_storage_folder_name_variables = "variables";
const char* const c_storage_folder_name_datachains = "datachains";

const char* const c_special_regex_for_domain_name = "@domain_name";
const char* const c_special_regex_for_email_address = "@email_address";
const char* const c_special_regex_for_bitcoin_address = "@bitcoin_address";
const char* const c_special_regex_for_peerchain_identity = "@peerchain_identity";
const char* const c_special_regex_for_peerchain_description = "@peerchain_description";

const char* const c_file_repository_meta_data_line_prefix = "md:";
const char* const c_file_repository_public_key_line_prefix = "pk:";
const char* const c_file_repository_source_hash_line_prefix = "sh:";
const char* const c_file_repository_target_hash_line_prefix = "th:";

const char* const c_file_repository_meta_data_info_type_raw = "raw";

const char* const c_file_repository_local_hash_attribute = "local_hash";

const char* const c_storage_identity_tx_id = "[0]";

const char* const c_label_prefix = "label";

const char* const c_files_directory = "files";

const char* const c_ciyam_files_directory = ".files";

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

const unsigned char c_file_type_char_blob = '\x01';
const unsigned char c_file_type_char_list = '\x02';

const unsigned char c_file_type_char_core_blob = '\x11';
const unsigned char c_file_type_char_core_list = '\x12';

const unsigned char c_file_type_char_mime_blob = '\x21';

const unsigned char c_file_type_char_encrypted = '\x04';
const unsigned char c_file_type_char_compressed = '\x08';

const unsigned char c_file_type_char_blob_encrypted = '\x05';

const unsigned char c_file_type_char_blob_compressed = '\x09';
const unsigned char c_file_type_char_list_compressed = '\x0a';

const unsigned char c_file_type_char_blob_no_compress = '\x81';

const unsigned char c_file_type_char_core_blob_compressed = '\x19';
const unsigned char c_file_type_char_core_list_compressed = '\x1a';

const unsigned char c_file_type_char_mime_blob_compressed = '\x29';

const char* const c_file_type_str_blob = "\x01";
const char* const c_file_type_str_list = "\x02";

const char* const c_file_type_str_core_blob = "\x11";
const char* const c_file_type_str_core_list = "\x12";

const char* const c_file_type_str_mime_blob = "\x21";

const char* const c_file_type_str_blob_encrypted = "\x05";
const char* const c_file_type_str_blob_no_compress = "\x81";

const char* const c_file_type_core_block_object = "blk";

const char* const c_file_type_core_block_header_height = "h";
const char* const c_file_type_core_block_header_identity = "i";
const char* const c_file_type_core_block_header_num_items = "n";
const char* const c_file_type_core_block_header_stream_cipher = "s";
const char* const c_file_type_core_block_header_targeted_ident = "t";
const char* const c_file_type_core_block_header_version_number = "v";

const char* const c_file_type_core_block_header_height_prefix = "h=";
const char* const c_file_type_core_block_header_identity_prefix = "i=";
const char* const c_file_type_core_block_header_num_items_prefix = "n=";
const char* const c_file_type_core_block_header_stream_cipher_prefix = "s=";
const char* const c_file_type_core_block_header_targeted_ident_prefix = "t=";
const char* const c_file_type_core_block_header_version_number_prefix = "v=";

const char* const c_file_type_core_block_detail_hind_hash_prefix = "h:";
const char* const c_file_type_core_block_detail_last_hashes_prefix = "l:";
const char* const c_file_type_core_block_detail_op_list_hash_prefix = "o:";
const char* const c_file_type_core_block_detail_pubkey_hashes_prefix = "p:";
const char* const c_file_type_core_block_detail_tree_root_hash_prefix = "t:";
const char* const c_file_type_core_block_detail_signature_file_hash_prefix = "s:";
const char* const c_file_type_core_block_detail_unix_block_time_value_prefix = "u:";

const char* const c_sys = "sys";
const char* const c_auto = "auto";
const char* const c_init = "init";
const char* const c_peer = "peer";
const char* const c_admin = "admin";

const char* const c_everyone = "everyone";
const char* const c_visitors = "visitors";
const char* const c_watchers = "watchers";

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
