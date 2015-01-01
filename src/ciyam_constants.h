// Copyright (c) 2013-2014 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_CONSTANTS_H
#  define CIYAM_CONSTANTS_H

const size_t c_max_file_transfer_size = 102400;

const size_t c_file_transfer_line_timeout = 5000;
const size_t c_file_transfer_initial_timeout = 10000;

const int c_file_transfer_max_line_size = 500;

const int c_file_type_val_blob = 0x01;
const int c_file_type_val_item = 0x02;
const int c_file_type_val_tree = 0x03;

const int c_file_type_val_mask = 0x07;

const int c_file_type_val_compressed = 0x08;

const int c_file_type_val_extra_core = 0x10;
const int c_file_type_val_extra_mime = 0x20;

const int c_file_type_val_extra_mask = 0x30;

const char c_file_type_char_blob = '\x01';
const char c_file_type_char_item = '\x02';
const char c_file_type_char_tree = '\x03';

const char c_file_type_char_core_blob = '\x11';
const char c_file_type_char_core_item = '\x12';
const char c_file_type_char_core_tree = '\x13';

const char c_file_type_char_mime_blob = '\x21';

const char c_file_type_char_compressed = '\x08';

const char c_file_type_char_blob_compressed = '\x09';
const char c_file_type_char_item_compressed = '\x0a';
const char c_file_type_char_tree_compressed = '\x0b';

const char c_file_type_char_core_blob_compressed = '\x91';
const char c_file_type_char_core_item_compressed = '\x92';
const char c_file_type_char_core_tree_compressed = '\x93';

const char c_file_type_char_mime_blob_compressed = '\xa1';

const char* const c_file_type_str_blob = "\x01";
const char* const c_file_type_str_item = "\x02";
const char* const c_file_type_str_tree = "\x03";

const char* const c_file_type_str_core_blob = "\x11";
const char* const c_file_type_str_core_item = "\x12";
const char* const c_file_type_str_core_tree = "\x13";

const char* const c_file_type_core_block_object = "blk";
const char* const c_file_type_core_rewind_object = "rwd";
const char* const c_file_type_core_restore_object = "rst";
const char* const c_file_type_core_transaction_object = "txn";
const char* const c_file_type_core_checkpoint_blocks_object = "cbk";
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

const char* const c_file_type_core_block_header_chain_meta_requisite_prefix = "r:";

const char* const c_file_type_core_block_detail_account_hash_prefix = "h=";
const char* const c_file_type_core_block_detail_account_lock_prefix = "l=";
const char* const c_file_type_core_block_detail_account_tx_hash_prefix = "th=";
const char* const c_file_type_core_block_detail_account_tx_lock_prefix = "tl=";

const char* const c_file_type_core_transaction_header_account_prefix = "a=";
const char* const c_file_type_core_transaction_header_tnumber_prefix = "t=";
const char* const c_file_type_core_transaction_header_public_key_prefix = "pk=";
const char* const c_file_type_core_transaction_header_application_prefix = "ap=";
const char* const c_file_type_core_transaction_header_previous_tchain_prefix = "pt=";
const char* const c_file_type_core_transaction_header_transaction_hash_prefix = "th=";
const char* const c_file_type_core_transaction_header_transaction_lock_prefix = "tl=";

const char* const c_file_type_core_transaction_detail_log_prefix = "l:";
const char* const c_file_type_core_transaction_detail_signature_prefix = "s:";

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

