// Copyright (c) 2011
//
// CIYAM Pty. Ltd.
// ACN 093 704 539
//
// ALL RIGHTS RESERVED
//
// Permission to use this software for non-commercial purposes is hereby granted. Permission to
// distribute this software privately is granted provided that the source code is unaltered and
// complete or that any alterations and omissions have been first approved by CIYAM. Commercial
// usage of this software is not permitted without first obtaining a license for such a purpose
// from CIYAM. This software may not be publicly distributed unless written permission to do so
// has been obtained from CIYAM.

#ifndef FCGI_CMDS_H
#  define FCGI_CMDS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <map>
#     include <set>
#     include <string>
#     include <vector>
#     include <utility>
#  endif

#  include "fcgi_info.h"

class tcp_socket;

void read_module_strings( module_info& info, tcp_socket& socket );

bool simple_command( session_info& sess_info, const std::string& cmd, std::string* p_response = 0 );

bool perform_update( const std::string& module, const std::string& class_id,
 const std::string& key, const std::vector< std::pair< std::string, std::string > >& field_value_pairs,
 tcp_socket& socket, const std::string& user_key, const std::string& tz_abbr );

bool perform_update( const std::string& module,
 const std::string& class_id, const std::string& key, const std::string& field,
 const std::string& old_value, const std::string& new_value, tcp_socket& socket,
 const std::string& user_key, std::string& error );

bool perform_action( const std::string& module_name,
 const std::string& class_id, const std::string& act, const std::string& app,
 const std::string& field, const std::string& fieldlist, const std::string& exec,
 const std::string& extra, row_error_container& row_errors, tcp_socket& socket,
 const std::string& user_key, const std::string& tz_abbr, int gmt_offset );

bool fetch_item_info( const std::string& module,
 const std::string& class_id, const std::string& item_key, const std::string& field_list,
 const std::string& set_field_values, const session_info& sess_info, std::pair< std::string, std::string >& item_info,
 const std::string& uinfo, const std::string* p_owner = 0, const std::string* p_pdf_spec_name = 0,
 const std::string* p_pdf_title = 0, const std::string* p_pdf_link_filename = 0, std::string* p_pdf_view_file_name = 0 );

bool fetch_list_info( const std::string& module,
 const std::string& class_id, const std::string& uinfo, const session_info& sess_info,
 bool is_reverse, int row_limit, const std::string& key_info, const std::string& field_list,
 const std::string& filters, const std::string& search_text, const std::string& search_query,
 tcp_socket& socket, data_container& rows, const std::string& exclude_key, bool* p_prev = 0,
 std::string* p_perms = 0, const std::string* p_security_info = 0, const std::string* p_extra_debug = 0,
 const std::set< std::string >* p_exclude_keys = 0, const std::string* p_pdf_spec_name = 0,
 const std::string* p_pdf_link_filename = 0, std::string* p_pdf_view_file_name = 0 );

bool fetch_parent_row_data( const std::string& module,
 const std::string& record_key, const std::string& field_id, const std::string& pclass_id,
 const std::string& parent_field, const std::string& parent_extras, const session_info& sess_info,
 const std::string& parent_key, data_container& parent_row_data, tcp_socket& socket,
 const values* p_key_values = 0, const values* p_fkey_values = 0, const values* p_skey_values = 0,
 bool *p_has_view_id = 0, const std::set< std::string >* p_exclude_keys = 0, std::string* p_skey_required = 0 );

bool populate_list_info( list_source& list,
 const std::map< std::string, std::string >& list_selections,
 const std::map< std::string, std::string >& list_search_text,
 const std::map< std::string, std::string >& list_search_values,
 const std::string& listinfo, const std::string& listsort,
 const std::string& parent_key, bool is_printable, const std::string& view_cid,
 const std::string& view_pfield, const std::set< std::string >* p_specials, const session_info& sess_info,
 const std::string* p_pdf_spec_name = 0, const std::string* p_pdf_link_filename = 0, std::string* p_pdf_view_file_name = 0 );

void fetch_user_quick_links( const module_info& mod_info, session_info& sess_info );

void add_quick_link( const std::string& module_ref,
 const std::string& cmd, const std::string& data, const std::string& extra,
 const std::string& listsrch, const std::string& listsort, const std::string& oident,
 const std::string& uselect, std::string& error_message, bool& had_send_or_recv_error,
 const module_info& mod_info, session_info& sess_info, const std::map< std::string, std::string >* p_list_selections = 0 );

void save_record( const std::string& module_id,
 const std::string& flags, const std::string& app, const std::string& chk,
 const std::string& field, const std::string& extra, const std::string& exec, const std::string& cont,
 const std::string& extrafields, bool is_new_record, const std::map< std::string, std::string >& new_field_and_values,
 const std::map< std::string, std::string >& extra_field_info, view_info_const_iterator& vici,
 const view_source& view, session_info& sess_info, std::string& act, std::string& data, std::string& new_key,
 std::string& error_message, bool& was_invalid, bool& had_send_or_recv_error );

#endif
