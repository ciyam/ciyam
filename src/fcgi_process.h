// Copyright (c) 2013-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef FCGI_PROCESS_H
#  define FCGI_PROCESS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <map>
#     include <set>
#     include <string>
#     include <iosfwd>
#  endif

#  include "fcgi_info.h"

void process_fcgi_request( module_info& mod_info, session_info* p_session_info,
 std::string& cmd, std::string& act, std::string& data, std::string& field, std::string& extra,
 std::string& qlink, std::string& password, const std::string& pin, const std::string& raddr,
 const std::string& uselect, const std::string& userhash, const std::string& userfetch,
 const std::string& exec, const std::string& ident, const std::string& oident, const std::string& special,
 const std::string& back, const std::string& vtabc, const std::string& bcount, const std::string& hashval,
 const std::string& cont, const std::string& listarg, const std::string& listvar, const std::string& listinfo,
 const std::string& listsort, const std::string& listsrch, const std::string& listextra,
 const std::string& findinfo, const std::string& app, const std::string& keepchecks, const std::string& new_key,
 const std::string& session_id, const std::string& module_id, const std::string& module_ref, const std::string& module_name,
 const std::string& scrollx, const std::string& scrolly, std::string& server_command, const std::set< std::string >& menu_opts,
 const std::set< std::string >& selected_records, std::map< std::string, std::string >& input_data,
 view_source& view, view_info_const_iterator& vici, std::map< std::string, std::string >& view_extra_vars,
 list_source& list, list_source& olist, list_info_const_iterator& lici, std::map< std::string, std::string >& list_selections,
 std::map< std::string, std::string >& list_search_text, std::map< std::string, std::string >& list_search_values,
 std::map< std::string, list_source >& home_lists, std::string& user_home_info, std::map< std::string, std::string >& user_field_info,
 std::map< std::string, std::string >& new_field_and_values, std::map< std::string, row_error_container >& child_row_errors,
 std::string& error_message, std::ostringstream& extra_content, std::string& extra_content_func, std::string& activation_file,
 bool& okay, bool& had_send_or_recv_error, bool& record_not_found_error, bool& finished_session, bool module_access_denied,
 bool cookies_permitted, bool temp_session, bool using_anonymous, bool use_url_checksum, bool is_ssl, bool is_sign_in, bool is_in_edit,
 bool is_new_record, bool was_invalid, bool skip_force_fields, bool created_session, bool allow_module_switching, int vtab_num );

#endif
