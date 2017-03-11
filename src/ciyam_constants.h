// Copyright (c) 2013-2017 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_CONSTANTS_H
#  define CIYAM_CONSTANTS_H

const int c_tx_id_module = 1;
const int c_tx_id_initial = 2;
const int c_tx_id_standard = 5;

const char* const c_okay = "okay";

const char* const c_meta_model = "Meta";

const char* const c_file_archives_folder = "arch";
const char* const c_system_variables_folder = "vars";

const char* const c_storage_identity_tx_id = "[0]";

const char* const c_block_prefix = "block";
const char* const c_files_directory = "files";

const char* const c_file_copy_command = "file_copy";
const char* const c_file_kill_command = "file_kill";

const size_t c_max_file_transfer_size = 102400;

const size_t c_file_transfer_line_timeout = 10000;
const size_t c_file_transfer_initial_timeout = 60000;

const size_t c_file_transfer_max_line_size = 100000;

const int c_file_type_val_blob = 0x01;
const int c_file_type_val_list = 0x02;

const int c_file_type_val_mask = 0x03;

// NOTE: The 0x04 bit is available for future usage.

const int c_file_type_val_compressed = 0x08;

const int c_file_type_val_extra_core = 0x10;
const int c_file_type_val_extra_mime = 0x20;

const int c_file_type_val_extra_mask = 0x30;

// NOTE: The 0x40 and 0x80 bits are available for future usage.

const char c_file_type_char_blob = '\x01';
const char c_file_type_char_list = '\x02';

const char c_file_type_char_core_blob = '\x11';
const char c_file_type_char_core_list = '\x12';

const char c_file_type_char_mime_blob = '\x21';

const char c_file_type_char_compressed = '\x08';

const char c_file_type_char_blob_compressed = '\x09';
const char c_file_type_char_list_compressed = '\x0a';

const char c_file_type_char_core_blob_compressed = '\x19';
const char c_file_type_char_core_list_compressed = '\x1a';

const char c_file_type_char_mime_blob_compressed = '\x29';

const char* const c_file_type_str_blob = "\x01";
const char* const c_file_type_str_list = "\x02";

const char* const c_file_type_str_core_blob = "\x11";
const char* const c_file_type_str_core_list = "\x12";

const char* const c_file_type_core_block_object = "blk";
const char* const c_file_type_core_rewind_object = "rwd";
const char* const c_file_type_core_account_object = "acc";
const char* const c_file_type_core_checkpoint_object = "chk";
const char* const c_file_type_core_transaction_object = "txn";
const char* const c_file_type_core_blockchain_info_object = "bci";
const char* const c_file_type_core_blockchain_meta_object = "bcm";
const char* const c_file_type_core_checkpoint_info_object = "cpi";
const char* const c_file_type_core_checkpoint_prune_object = "cpr";
const char* const c_file_type_core_checkpoint_blocks_object = "cbk";
const char* const c_file_type_core_transactions_info_object = "txi";
const char* const c_file_type_core_checkpoint_transactions_object = "ctx";

const char* const c_file_type_core_block_header_height_prefix = "h=";
const char* const c_file_type_core_block_header_weight_prefix = "w=";
const char* const c_file_type_core_block_header_account_prefix = "a=";
const char* const c_file_type_core_block_header_chain_meta_prefix = "cm=";
const char* const c_file_type_core_block_header_public_key_prefix = "pk=";
const char* const c_file_type_core_block_header_account_hash_prefix = "ah=";
const char* const c_file_type_core_block_header_account_lock_prefix = "al=";
const char* const c_file_type_core_block_header_total_weight_prefix = "tw=";
const char* const c_file_type_core_block_header_previous_block_prefix = "pb=";

const char* const c_file_type_core_block_detail_account_prefix = "a:";
const char* const c_file_type_core_block_detail_signature_prefix = "s:";
const char* const c_file_type_core_block_detail_transaction_prefix = "t:";
const char* const c_file_type_core_block_detail_proof_of_work_prefix = "w:";

const char* const c_file_type_core_block_header_chain_meta_requisite_prefix = "r:";

const char* const c_file_type_core_block_detail_account_exp_prefix = "e=";
const char* const c_file_type_core_block_detail_account_hash_prefix = "h=";
const char* const c_file_type_core_block_detail_account_lock_prefix = "l=";
const char* const c_file_type_core_block_detail_account_msg_key_prefix = "mk=";
const char* const c_file_type_core_block_detail_account_tx_hash_prefix = "th=";
const char* const c_file_type_core_block_detail_account_tx_lock_prefix = "tl=";

const char* const c_file_type_core_transaction_header_account_prefix = "a=";
const char* const c_file_type_core_transaction_header_sequence_prefix = "s=";
const char* const c_file_type_core_transaction_header_public_key_prefix = "pk=";
const char* const c_file_type_core_transaction_header_application_prefix = "ap=";
const char* const c_file_type_core_transaction_header_previous_tchain_prefix = "pt=";
const char* const c_file_type_core_transaction_header_transaction_hash_prefix = "th=";
const char* const c_file_type_core_transaction_header_transaction_lock_prefix = "tl=";

const char* const c_file_type_core_transaction_detail_log_prefix = "l:";
const char* const c_file_type_core_transaction_detail_signature_prefix = "s:";

const char* const c_file_type_core_transaction_special_file_extract_prefix = "fe ";

const char* const c_file_type_core_blockchain_info_header_chain_prefix = "c=";
const char* const c_file_type_core_blockchain_info_header_height_prefix = "h=";

const char* const c_file_type_core_checkpoint_info_header_chain_prefix = "c=";
const char* const c_file_type_core_checkpoint_info_header_height_prefix = "h=";
const char* const c_file_type_core_checkpoint_info_header_checkpoint_prefix = "cp=";

const char* const c_file_type_core_transactions_info_header_chain_prefix = "c=";

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
const char* const c_response_not_found = "(not found)";

const char* const c_response_error_prefix = "(error)";
const char* const c_response_message_prefix = "(message)";

#endif

