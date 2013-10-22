// Copyright (c) 2011-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2013 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef FCGI_UTILS_H
#  define FCGI_UTILS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <map>
#     include <string>
#     include <iosfwd>
#  endif

#  include "macros.h"
#  include "ptypes.h"

class mtime;
class udate;
class guard;
class date_time;

struct source;
struct list_source;
struct session_info;

//#  define DEBUG

#  define GDS( s ) get_string( STRINGIZE( s ) )

#  define LOG_TRACE( message ) log_trace_message( message )
#  define TEMP_TRACE( message ) LOG_TRACE( message )

#  ifdef DEBUG
#     define DEBUG_TRACE( message ) LOG_TRACE( message )
#  else
#     define DEBUG_TRACE( message )
#  endif

const char* const c_extkeys_file = "extkeys.txt";

void init_log( );
void log_trace_message( const std::string& message );

const char* get_server_id( );
void set_server_id( std::string& id );

std::string get_id_from_server_id( );

void init_strings( );
void init_extkeys( );

std::string get_string( const char* p_id );
std::string get_extkey( const std::string& id );

std::string get_app_name( );

std::string get_display_string( const std::string& key );

std::string get_module_id( const std::string& module_name );
std::string get_module_id_for_attached_file( const source& source );

std::string get_module_page_name( const std::string& module_ref, bool check_for_index = false );

#  ifdef _WIN32
bool delete_files( const char* p_dir, bool recycle = false );
#  endif

std::string get_hash( const std::string& values );

std::string get_user_hash( const std::string& user_id );

std::string get_checksum( const std::string& values );

std::string get_checksum( const session_info& sess_info, const std::string& values );

void get_session_dtm( const session_info& sess_info, date_time& dt );

std::string get_cookie_value( const std::string& session_id,
 const std::string& user_id, bool is_login_or_logout = false, int dtm_offset = 0, int gmt_offset = 0 );

void setup_gmt_and_dtm_offset( std::map< std::string, std::string >& input_data, session_info& sess_info );

std::string hash_password( const std::string& salted_password );

bool is_non_persistent( const std::string& session_id );
void add_non_persistent( const std::string& session_id );
void remove_non_persistent( const std::string& session_id );

void remove_session_temp_directory( const std::string& session_id, const char* p_prefix = 0 );

std::string double_escaped( const std::string& src, const std::string& chars );

void str_replace( std::string& src, const char* p_find, const std::string& replace );

std::string remove_key( const std::string& src );

std::string string_message( const std::string& display_string,
 const std::pair< std::string, std::string >& parm1, const char* p_closing = 0 );

std::string string_message( const std::string& display_string,
 const std::pair< std::string, std::string >& parm1, const char* p_close1,
 const std::pair< std::string, std::string >& parm2, const char* p_close2 = 0 );

std::string replace_underbars( const std::string& src, char with = ' ' );

std::string action_text( const std::string& s );

std::string format_date( const udate& ud, const char* p_dp = 0 );
std::string format_time( const mtime& mt, const char* p_tp = 0 );
std::string format_date_time( const date_time& dt, const char* p_tp = 0 );

std::string escape_markup( const std::string& input );
std::string escape_specials( const std::string& input );

std::string replace_spaces( const std::string& input, const char* p_rep, size_t spos );

std::string replace_crlfs_and_spaces( const std::string& input, const char* p_rep, const char* p_srep );

void force_html_tags_to_lower_case( std::string& html );

void create_tmp_file_link( std::string& tmp_link_path,
 const std::string& file_name, const std::string& file_ext, const std::string& dest_file_name );

std::string exec_args( const std::string& input );

void setup_directories( );

bool has_permission( const std::string& perm, const session_info& sess_info );

bool has_perm_extra( const std::string& perm_extra,
 const std::map< std::string, std::string >& source_extras, const session_info& sess_info );

void replace_action_parms( std::string& id, std::string& action,
 const std::string& restrict_fields, const std::string& restrict_values );

void replace_links_and_output( const std::string& s, const std::string& id,
 const std::string& module, const std::string& module_ref, std::ostream& os,
 bool is_content, bool output_hrefs, const std::string& session_id, const session_info& sess_info,
 const std::string& user_select_key, bool using_session_cookie, bool use_url_checksum );

void output_login_logout( const std::string& module_name, std::ostream& os,
 const std::string& extra_details, const std::string& msg = "", bool is_activation = false );

void output_actions( std::ostream& os,
 const source& src, const std::string& cmd, const std::string& data,
 const session_info& sess_info, const std::string& ident, const std::string& key_and_version,
 const std::string& class_id, const std::string& class_name, const std::string& actions_value,
 const std::string& owner, const std::string& session_id, const std::string& user_select_key,
 bool using_session_cookie, bool use_url_checksum, bool has_hashval, std::string* p_default = 0 );

void parse_field_extra( const std::string& extra, std::map< std::string, std::string >& extra_data );

void parse_key_ver_rev_state_and_type_info(
 const std::string& key_ver_rev_state_and_type_info,
 std::string& key_and_version, uint64_t& state, std::string& type_info );

void determine_fixed_query_info( std::string& fixed_fields,
 std::string& fixed_key_values, int& num_fixed_key_values, bool& is_reverse,
 const list_source& list, const std::string& fixed_parent_field, const std::string& fixed_parent_keyval,
 const std::map< std::string, std::string >& list_selections, const session_info& sess_info, std::string* p_set_field_values = 0 );

#endif

