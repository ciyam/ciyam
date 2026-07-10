// Copyright (c) 2013-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_CONSTANTS_H
#  define CIYAM_CONSTANTS_H

enum logging_type
{
   e_logging_type_never,
   e_logging_type_always,
   e_logging_type_errors,
   e_logging_type_standard
};

const int c_tx_id_module = 1;
const int c_tx_id_initial = 2;
const int c_tx_id_standard = 5;

const int c_num_sys_field_names = 5;

const int c_peer_type_any = -3;
const int c_peer_type_hub = -2;
const int c_peer_type_user = -1;
const int c_peer_type_combined = 0;
const int c_peer_type_local_only = 1;
const int c_peer_type_backup_only = 2;
const int c_peer_type_shared_only = 3;

const size_t c_max_extras = 9;
const size_t c_chunk_digits = 6;

const size_t c_identity_length = 7;
const size_t c_encrypted_length = 32;

const size_t c_bc_identity_length = 9;

const size_t c_bc_scaling_value = 50;
const size_t c_bc_scaling_test_value = 5;

const size_t c_key_reserve_size = 128;

const size_t c_secret_reserve_size = 256;

const size_t c_key_rounds_multiplier = 30;

const size_t c_auto_script_msleep = 1000; // i.e. 1 sec

const char c_persist_variable_prefix = '>';
const char c_restore_variable_prefix = '<';

constexpr const char* c_UTC = "UTC";

constexpr const char* c_none = "none";
constexpr const char* c_okay = "okay";

constexpr const char* c_true = "true";
constexpr const char* c_false = "false";

constexpr const char* c_true_value = "1";
constexpr const char* c_false_value = "0";

constexpr const char* c_dummy = "dummy";
constexpr const char* c_error = "error";

constexpr const char* c_ellipsis = "...";

constexpr const char* c_watching = "[watching]";
constexpr const char* c_finishing = "[finishing]";
constexpr const char* c_suspended = "[suspended]";

constexpr const char* c_cmd_fetch = "pf";
constexpr const char* c_cmd_create = "pc";
constexpr const char* c_cmd_update = "pu";
constexpr const char* c_cmd_destroy = "pd";
constexpr const char* c_cmd_execute = "pe";

constexpr const char* c_perms_r_r = "r--r-----";
constexpr const char* c_perms_rw_rw_r = "rw-rw-r--";

const char c_hidden_file_prefix = '.';

const char c_key_exchange_suffix = '*';

constexpr const char* c_key_field = "@key";

constexpr const char* c_group_field = "@group";
constexpr const char* c_level_field = "@level";
constexpr const char* c_order_field = "@order";
constexpr const char* c_owner_field = "@owner";

constexpr const char* c_tz_loc = "@loc";

constexpr const char* c_dtm_now = "@now";

constexpr const char* c_date_today = "@today";
constexpr const char* c_date_tomorrow = "@tomorrow";

constexpr const char* c_object_variable_skip_fk_handling = "@skip_fk_handling";

const char c_notifier_ignore_char = '!';
const char c_notifier_select_char = '#';
const char c_notifier_unselect_char = '=';
const char c_notifier_mod_ignore_char = '+';
const char c_notifier_new_ignore_char = '-';
const char c_notifier_mod_sel_ignore_char = '*';
const char c_notifier_new_sel_ignore_char = '^';

constexpr const char* c_notifier_prefix = "@:";
constexpr const char* c_notifier_selection = "#";

constexpr const char* c_notifier_viewed_suffix = "viewed";

const size_t c_notifer_check_wait = 50; // i.e. 1/20 sec
const size_t c_max_notifer_checks = 100;

constexpr const char* c_CIYAM = "CIYAM";

constexpr const char* c_ciyam_tag = "ciyam";

constexpr const char* c_bc_prefix = "bc.";

constexpr const char* c_blk_suffix = ".blk";
constexpr const char* c_key_suffix = ".key";
constexpr const char* c_opl_suffix = ".opl";
constexpr const char* c_pub_suffix = ".pub";
constexpr const char* c_put_suffix = ".put";
constexpr const char* c_sig_suffix = ".sig";

constexpr const char* c_locked_suffix = ".locked";
constexpr const char* c_master_suffix = ".master";
constexpr const char* c_pinned_suffix = ".pinned";
constexpr const char* c_shared_suffix = ".shared";
constexpr const char* c_zenith_suffix = ".zenith";

constexpr const char* c_genesis_suffix = ".0.blk";

constexpr const char* c_identity_suffix = "identity";

constexpr const char* c_dead_keys_suffix = ".dead_keys.lst";
constexpr const char* c_demo_keys_suffix = ".demo_keys.lst";

constexpr const char* c_repository_info_suffix = ".info";

constexpr const char* c_two_spaces = "  ";

constexpr const char* c_backup = "backup";
constexpr const char* c_restore = "restore";

constexpr const char* c_files_ext = ".files";

constexpr const char* c_csv_file_ext = ".csv";
constexpr const char* c_log_file_ext = ".log";
constexpr const char* c_old_file_ext = ".old";
constexpr const char* c_sav_file_ext = ".sav";
constexpr const char* c_tmp_file_ext = ".tmp";

constexpr const char* c_ciyam_pem = "ciyam.pem";

constexpr const char* c_logging_never = "never";
constexpr const char* c_logging_always = "always";
constexpr const char* c_logging_errors = "errors";
constexpr const char* c_logging_standard = "standard";

constexpr const char* c_ciyam_script = "ciyam_script";
constexpr const char* c_ciyam_server = "ciyam_server";
constexpr const char* c_ciyam_update = "ciyam_update";

constexpr const char* c_ciyam_interface = "ciyam_interface";

constexpr const char* c_ciyam_dummy_date = "20211202";

constexpr const char* c_ciyam_dummy_unix_time = "9999999999";

constexpr const char* c_ui_class_marker = "CLASS";
constexpr const char* c_ui_group_marker = "GROUP";
constexpr const char* c_ui_class_changing = "ui_CLASS_changing";
constexpr const char* c_ui_group_changing = "ui_GROUP_changing";

constexpr const char* c_json_escape_specials = "bfnrt\b\f\n\r\t";

constexpr const char* c_ciyam_ui_stop_file = "ciyam_interface.stop";

constexpr const char* c_ciyam_server_sid_file = "ciyam_server.sid";
constexpr const char* c_ciyam_server_sid_chk_file = "ciyam_server.sid.chk";

constexpr const char* c_test_backup_identity = "e3733d121";
constexpr const char* c_test_shared_identity = "121d3373e";

constexpr const char* c_variables_branch = "Variables";
constexpr const char* c_web_files_branch = "Web_Files";

constexpr const char* c_query_param_name_access = "access";
constexpr const char* c_query_param_name_device = "device";
constexpr const char* c_query_param_name_passwd = "passwd";
constexpr const char* c_query_param_name_options = "options";
constexpr const char* c_query_param_name_payload = "payload";
constexpr const char* c_query_param_name_request = "request";
constexpr const char* c_query_param_name_session = "session";

constexpr const char* c_web_access_prefix = ".web_access_";

constexpr const char* c_web_device_name_admin = ".web_device_admin";

constexpr const char* c_invalid_name_chars = "<>\"|&\\";

constexpr const char* c_key_field_name = "Key_";
constexpr const char* c_rev_field_name = "Rev_";
constexpr const char* c_sec_field_name = "Sec_";
constexpr const char* c_typ_field_name = "Typ_";
constexpr const char* c_ver_field_name = "Ver_";

constexpr const char* c_create_lock_name = "create";
constexpr const char* c_review_lock_name = "review";
constexpr const char* c_update_lock_name = "update";
constexpr const char* c_destroy_lock_name = "destroy";

constexpr const char* c_meta_model_name = "Meta";

constexpr const char* c_meta_storage_name = "Meta";
constexpr const char* c_ciyam_storage_name = "ciyam";

constexpr const char* c_meta_app_directory = "meta";

constexpr const char* c_meta_class_name_class = "Class";
constexpr const char* c_meta_class_name_model = "Model";
constexpr const char* c_meta_class_name_application = "Application";

constexpr const char* c_ignore_field = "@ignore";

constexpr const char* c_local_host = "localhost";

constexpr const char* c_null_ip_addr = "::";
constexpr const char* c_local_ip_addr = "::1";

constexpr const char* c_local_ip_addrs = "::1 127.0.0.1";

constexpr const char* c_timezones_file = "timezones.sio";

constexpr const char* c_autoscript_file = "autoscript.sio";
constexpr const char* c_manuscript_file = "manuscript.sio";

constexpr const char* c_password_info_file = "password.info";

constexpr const char* c_unlocked_with_key_file = ".unlocked_with_key";

constexpr const char* c_demo_entropy_1 = "22d5cd3ff652620a739677323079e3dd";
constexpr const char* c_demo_entropy_2 = "3c6eede8b0c9717370546cd446427499";
constexpr const char* c_demo_entropy_3 = "de604cee0755a3d81944ea96aed12681";

constexpr const char* c_env_var_ciyam_user = "CIYAM_USER";

constexpr const char* c_notifier_none = "none";
constexpr const char* c_notifier_created = "created";
constexpr const char* c_notifier_deleted = "deleted";
constexpr const char* c_notifier_modified = "modified";
constexpr const char* c_notifier_moved_from = "moved_from";
constexpr const char* c_notifier_modified_from = "modified_from";

constexpr const char* c_file_zero_length = "*";

constexpr const char* c_system_archives_folder = "arch";
constexpr const char* c_system_blacklist_folder = "blst";
constexpr const char* c_system_datachain_folder = "data";
constexpr const char* c_system_peerchain_folder = "peer";
constexpr const char* c_system_user_info_folder = "user";
constexpr const char* c_system_variables_folder = "vars";
constexpr const char* c_system_repository_folder = "repo";

constexpr const char* c_storage_folder_name_dot_dat = ".dat";
constexpr const char* c_storage_folder_name_dot_idx = ".idx";
constexpr const char* c_storage_folder_name_modules = "modules";
constexpr const char* c_storage_folder_name_channels = "channels";
constexpr const char* c_storage_folder_name_gid_data = "gid_data";
constexpr const char* c_storage_folder_name_uid_data = "uid_data";
constexpr const char* c_storage_folder_name_variables = "variables";
constexpr const char* c_storage_folder_name_datachains = "datachains";

constexpr const char* c_special_regex_for_domain_name = "@domain_name";
constexpr const char* c_special_regex_for_email_address = "@email_address";
constexpr const char* c_special_regex_for_bitcoin_address = "@bitcoin_address";
constexpr const char* c_special_regex_for_peerchain_identity = "@peerchain_identity";
constexpr const char* c_special_regex_for_peerchain_description = "@peerchain_description";

constexpr const char* c_file_repository_meta_data_line_prefix = "md:";
constexpr const char* c_file_repository_public_key_line_prefix = "pk:";
constexpr const char* c_file_repository_source_hash_line_prefix = "sh:";
constexpr const char* c_file_repository_target_hash_line_prefix = "th:";

constexpr const char* c_file_repository_meta_data_info_type_raw = "raw";

constexpr const char* c_file_repository_local_hash_attribute = "local_hash";

constexpr const char* c_storage_identity_tx_id = "[0]";

constexpr const char* c_label_prefix = "label";

constexpr const char* c_tmp_directory = "tmp";
constexpr const char* c_files_directory = "files";

constexpr const char* c_ciyam_files_directory = ".files";

constexpr const char* c_application_script_prefix = "app_script:";

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

constexpr const char* c_file_type_str_blob = "\x01";
constexpr const char* c_file_type_str_list = "\x02";

constexpr const char* c_file_type_str_core_blob = "\x11";
constexpr const char* c_file_type_str_core_list = "\x12";

constexpr const char* c_file_type_str_mime_blob = "\x21";

constexpr const char* c_file_type_str_blob_encrypted = "\x05";
constexpr const char* c_file_type_str_blob_no_compress = "\x81";

constexpr const char* c_file_type_core_block_object = "blk";

constexpr const char* c_file_type_core_block_header_height = "h";
constexpr const char* c_file_type_core_block_header_identity = "i";
constexpr const char* c_file_type_core_block_header_num_items = "n";
constexpr const char* c_file_type_core_block_header_stream_cipher = "s";
constexpr const char* c_file_type_core_block_header_targeted_ident = "t";
constexpr const char* c_file_type_core_block_header_version_number = "v";

constexpr const char* c_file_type_core_block_header_height_prefix = "h=";
constexpr const char* c_file_type_core_block_header_identity_prefix = "i=";
constexpr const char* c_file_type_core_block_header_num_items_prefix = "n=";
constexpr const char* c_file_type_core_block_header_stream_cipher_prefix = "s=";
constexpr const char* c_file_type_core_block_header_targeted_ident_prefix = "t=";
constexpr const char* c_file_type_core_block_header_version_number_prefix = "v=";

constexpr const char* c_file_type_core_block_detail_hind_hash_prefix = "h:";
constexpr const char* c_file_type_core_block_detail_last_hashes_prefix = "l:";
constexpr const char* c_file_type_core_block_detail_op_list_hash_prefix = "o:";
constexpr const char* c_file_type_core_block_detail_pubkey_hashes_prefix = "p:";
constexpr const char* c_file_type_core_block_detail_tree_root_hash_prefix = "t:";
constexpr const char* c_file_type_core_block_detail_signature_file_hash_prefix = "s:";
constexpr const char* c_file_type_core_block_detail_unix_block_time_value_prefix = "u:";

constexpr const char* c_sys = "sys";
constexpr const char* c_auto = "auto";
constexpr const char* c_init = "init";
constexpr const char* c_peer = "peer";
constexpr const char* c_admin = "admin";

constexpr const char* c_unknown = "unknown";

constexpr const char* c_everyone = "everyone";
constexpr const char* c_visitors = "visitors";
constexpr const char* c_watchers = "watchers";

constexpr const char* c_both = "both";
constexpr const char* c_create = "create";
constexpr const char* c_update = "update";

constexpr const char* c_anyone = "anyone";
constexpr const char* c_admin_only = "admin_only";
constexpr const char* c_owner_only = "owner_only";
constexpr const char* c_admin_owner = "admin_owner";
constexpr const char* c_denied_always = "denied_always";

constexpr const char* c_user_class_suffix = "_user_class";

constexpr const char* c_response_okay = "(okay)";
constexpr const char* c_response_okay_more = "(okay more)";
constexpr const char* c_response_okay_skip = "(okay skip)";

constexpr const char* c_response_not_found = "(not found)";

constexpr const char* c_response_error_prefix = "(error)";
constexpr const char* c_response_message_prefix = "(message)";

constexpr const char* c_special_variable_queue_prefix = "@queue_";
constexpr const char* c_special_variable_mapped_prefix = "@mapped_";

#endif
