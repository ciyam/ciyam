// Copyright (c) 2011-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef FCGI_LIST_H
#  define FCGI_LIST_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <map>
#     include <set>
#     include <string>
#     include <iosfwd>
#  endif

#  include "ptypes.h"

struct list_source;
struct session_info;

void setup_list_fields( list_source& list,
 const std::string& pkey, const std::string& module_name, const session_info& sess_info, bool is_printable );

void output_list_form( std::ostream& os,
 const list_source& source, const std::string& session_id,
 const std::string& user_select_key, const std::string& error_message,
 bool is_printable, bool using_session_cookie, bool allow_list_actions,
 bool is_parent_edit, const std::map< std::string, std::string >& list_selections,
 const std::map< std::string, std::string >& list_search_text, const std::map< std::string, std::string >& list_search_values,
 uint64_t parent_state, bool is_child_list, const std::string& parent_key, bool keep_checks,
 const std::string& pident, const std::string& oident, const session_info& sess_info, std::string& extra_content_func,
 const std::set< std::string >& specials, bool use_url_checksum, const std::string& qlink,
 const std::string& findinfo_and_listsrch, const std::set< std::string >& selected_records, bool embed_images,
 bool has_hashval, bool has_owner_parent, bool& has_any_changing, int back_count, const std::string* p_pdf_file_name = 0 );

#endif
