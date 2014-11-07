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
const int c_file_type_val_extra_other_1 = 0x40;
const int c_file_type_val_extra_other_2 = 0x80;

const int c_file_type_val_extra_mask = 0xf0;

const char c_file_type_char_blob = '\x01';
const char c_file_type_char_item = '\x02';
const char c_file_type_char_tree = '\x03';

const char c_file_type_char_blob_compressed = '\x09';
const char c_file_type_char_item_compressed = '\x0a';
const char c_file_type_char_tree_compressed = '\x0b';

const char* const c_file_type_str_blob = "\x01";
const char* const c_file_type_str_item = "\x02";
const char* const c_file_type_str_tree = "\x03";

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

