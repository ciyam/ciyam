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

