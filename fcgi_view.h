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

#ifndef FCGI_VIEW_H
#  define FCGI_VIEW_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <map>
#     include <set>
#     include <string>
#     include <iosfwd>
#  endif

struct view_info;
struct module_info;
struct view_source;
struct session_info;

void setup_view_fields( view_source& view,
 const view_info& vinfo, const module_info& mod_info,
 const session_info& sess_info, const std::string& ident, const std::set< std::string >& login_opts,
 const std::string& module_id, const std::string& module_ref, bool is_in_edit, bool is_new_record );

bool output_view_form( std::ostream& os, const std::string& act,
 bool was_invalid, const std::map< std::string, std::string >& user_values,
 const std::string& exec, const std::string& cont, const std::string& data,
 const std::string& error_message, const std::string& extra, const std::string& pfield,
 const view_source& source, int vtab_num, const std::string& session_id,
 const std::string& user_select_key, bool using_session_cookie, bool embed_images,
 const std::map< std::string, std::string >& new_field_and_values, const session_info& sess_info,
 std::string& field_list, std::string& edit_timeout_func, std::string& extra_content_func, bool use_url_checksum,
 bool is_quick_link_view, const std::map< std::string, std::string >& show_opts, bool is_printable, int back_count,
 const std::string& pdf_view_file_name, bool& is_record_owner, std::string& extra_html_content, bool& is_changing );

#endif

