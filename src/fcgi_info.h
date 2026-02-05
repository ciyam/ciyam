// Copyright (c) 2011-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef FCGI_INFO_H
#  define FCGI_INFO_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <ctime>
#     include <map>
#     include <set>
#     include <list>
#     include <deque>
#     include <string>
#     include <vector>
#     include <utility>
#  endif

#  include "config.h"
#  include "ptypes.h"
#  include "threads.h"

#  ifdef SSL_SUPPORT
class ssl_socket;
#  else
class tcp_socket;
#  endif

const uint64_t c_state_modifier_00 = UINT64_C( 0x0000000000000100 );
const uint64_t c_state_modifier_01 = UINT64_C( 0x0000000000000200 );
const uint64_t c_state_modifier_02 = UINT64_C( 0x0000000000000400 );
const uint64_t c_state_modifier_03 = UINT64_C( 0x0000000000000800 );
const uint64_t c_state_modifier_04 = UINT64_C( 0x0000000000001000 );
const uint64_t c_state_modifier_05 = UINT64_C( 0x0000000000002000 );
const uint64_t c_state_modifier_06 = UINT64_C( 0x0000000000004000 );
const uint64_t c_state_modifier_07 = UINT64_C( 0x0000000000008000 );
const uint64_t c_state_modifier_08 = UINT64_C( 0x0000000000010000 );
const uint64_t c_state_modifier_09 = UINT64_C( 0x0000000000020000 );
const uint64_t c_state_modifier_10 = UINT64_C( 0x0000000000040000 );
const uint64_t c_state_modifier_11 = UINT64_C( 0x0000000000080000 );
const uint64_t c_state_modifier_12 = UINT64_C( 0x0000000000100000 );
const uint64_t c_state_modifier_13 = UINT64_C( 0x0000000000200000 );
const uint64_t c_state_modifier_14 = UINT64_C( 0x0000000000400000 );
const uint64_t c_state_modifier_15 = UINT64_C( 0x0000000000800000 );
const uint64_t c_state_modifier_16 = UINT64_C( 0x0000000001000000 );
const uint64_t c_state_modifier_17 = UINT64_C( 0x0000000002000000 );
const uint64_t c_state_modifier_18 = UINT64_C( 0x0000000004000000 );
const uint64_t c_state_modifier_19 = UINT64_C( 0x0000000008000000 );
const uint64_t c_state_modifier_20 = UINT64_C( 0x0000000010000000 );
const uint64_t c_state_modifier_21 = UINT64_C( 0x0000000020000000 );
const uint64_t c_state_modifier_22 = UINT64_C( 0x0000000040000000 );
const uint64_t c_state_modifier_23 = UINT64_C( 0x0000000080000000 );
const uint64_t c_state_modifier_24 = UINT64_C( 0x0000000100000000 );
const uint64_t c_state_modifier_25 = UINT64_C( 0x0000000200000000 );
const uint64_t c_state_modifier_26 = UINT64_C( 0x0000000400000000 );
const uint64_t c_state_modifier_27 = UINT64_C( 0x0000000800000000 );
const uint64_t c_state_modifier_28 = UINT64_C( 0x0000001000000000 );
const uint64_t c_state_modifier_29 = UINT64_C( 0x0000002000000000 );
const uint64_t c_state_modifier_30 = UINT64_C( 0x0000004000000000 );
const uint64_t c_state_modifier_31 = UINT64_C( 0x0000008000000000 );
const uint64_t c_state_modifier_32 = UINT64_C( 0x0000010000000000 );
const uint64_t c_state_modifier_33 = UINT64_C( 0x0000020000000000 );
const uint64_t c_state_modifier_34 = UINT64_C( 0x0000040000000000 );
const uint64_t c_state_modifier_35 = UINT64_C( 0x0000080000000000 );
const uint64_t c_state_modifier_36 = UINT64_C( 0x0000100000000000 );
const uint64_t c_state_modifier_37 = UINT64_C( 0x0000200000000000 );
const uint64_t c_state_modifier_38 = UINT64_C( 0x0000400000000000 );
const uint64_t c_state_modifier_39 = UINT64_C( 0x0000800000000000 );
const uint64_t c_state_modifier_40 = UINT64_C( 0x0001000000000000 );
const uint64_t c_state_modifier_41 = UINT64_C( 0x0002000000000000 );
const uint64_t c_state_modifier_42 = UINT64_C( 0x0004000000000000 );
const uint64_t c_state_modifier_43 = UINT64_C( 0x0008000000000000 );
const uint64_t c_state_modifier_44 = UINT64_C( 0x0010000000000000 );
const uint64_t c_state_modifier_45 = UINT64_C( 0x0020000000000000 );
const uint64_t c_state_modifier_46 = UINT64_C( 0x0040000000000000 );
const uint64_t c_state_modifier_47 = UINT64_C( 0x0080000000000000 );
const uint64_t c_state_modifier_48 = UINT64_C( 0x0100000000000000 );
const uint64_t c_state_modifier_49 = UINT64_C( 0x0200000000000000 );
const uint64_t c_state_modifier_50 = UINT64_C( 0x0400000000000000 );
const uint64_t c_state_modifier_51 = UINT64_C( 0x0800000000000000 );
const uint64_t c_state_modifier_52 = UINT64_C( 0x1000000000000000 );
const uint64_t c_state_modifier_53 = UINT64_C( 0x2000000000000000 );
const uint64_t c_state_modifier_54 = UINT64_C( 0x4000000000000000 );
const uint64_t c_state_modifier_55 = UINT64_C( 0x8000000000000000 );

const uint64_t state_modifiers[ ] =
{
   c_state_modifier_00,
   c_state_modifier_01,
   c_state_modifier_02,
   c_state_modifier_03,
   c_state_modifier_04,
   c_state_modifier_05,
   c_state_modifier_06,
   c_state_modifier_07,
   c_state_modifier_08,
   c_state_modifier_09,
   c_state_modifier_10,
   c_state_modifier_11,
   c_state_modifier_12,
   c_state_modifier_13,
   c_state_modifier_14,
   c_state_modifier_15,
   c_state_modifier_16,
   c_state_modifier_17,
   c_state_modifier_18,
   c_state_modifier_19,
   c_state_modifier_20,
   c_state_modifier_21,
   c_state_modifier_22,
   c_state_modifier_23,
   c_state_modifier_24,
   c_state_modifier_25,
   c_state_modifier_26,
   c_state_modifier_27,
   c_state_modifier_28,
   c_state_modifier_29,
   c_state_modifier_30,
   c_state_modifier_31,
   c_state_modifier_32,
   c_state_modifier_33,
   c_state_modifier_34,
   c_state_modifier_35,
   c_state_modifier_36,
   c_state_modifier_37,
   c_state_modifier_38,
   c_state_modifier_39,
   c_state_modifier_40,
   c_state_modifier_41,
   c_state_modifier_42,
   c_state_modifier_43,
   c_state_modifier_44,
   c_state_modifier_45,
   c_state_modifier_46,
   c_state_modifier_47,
   c_state_modifier_48,
   c_state_modifier_49,
   c_state_modifier_50,
   c_state_modifier_51,
   c_state_modifier_52,
   c_state_modifier_53,
   c_state_modifier_54,
   c_state_modifier_55
};

typedef std::multimap< uint64_t, std::string > modifier_container;
typedef modifier_container::iterator modifier_iterator;
typedef modifier_container::const_iterator modifier_const_iterator;
typedef modifier_container::value_type modifier_value_type;

struct fld_info
{
   fld_info( ) : tab_id( 0 ), unique( false ), indexed( false ), mandatory( false ), index_count( 0 ) { }

   std::string name;
   std::string field;
   std::string ftype;
   std::string extra;

   int tab_id;

   bool unique;
   bool indexed;
   bool mandatory;

   int index_count;

   std::string pclass;
   std::string pfield;
   std::string pfname;
   std::string pdname;
   std::string pextra;

   modifier_container modifiers;
};

struct par_info
{
   par_info( ) : folder( false ), mandatory( false ) { }

   std::string name;
   std::string field;
   std::string extra;
   std::string pclass;
   std::string pfield;
   std::string pextra;

   bool folder;
   bool mandatory;

   std::string skey;

   std::set< std::string > exclude_keys;

   std::map< std::string, std::string > operations;
};

struct rest_info
{
   rest_info( ) : mandatory( false ) { }

   std::string name;

   std::string field;
   std::string ftype;

   std::string extra;

   bool mandatory;

   std::map< std::string, std::string > operations;
};

struct enum_info
{
   std::string id;

   std::string name;

   std::map< std::string, std::string > filters;

   std::vector< std::pair< std::string, std::string > > values;
};

struct view_info
{
   std::string id;
   std::string cid;
   std::string name;
   std::string type;
   std::string perm;

   std::string vlink;

   std::string module;
   std::string mclass;
   std::string bclass;

   std::string pdf_spec;

   std::string filename_field_id;

   std::string static_instance_key;

   std::map< std::string, std::string > extras;

   modifier_container modifiers;

   std::vector< fld_info > fields;
   std::vector< std::string > file_ids;

   std::vector< std::pair< std::string, std::string > > tabs;
};

struct list_info
{
   list_info( ) : nexclude( false ) { }

   std::string id;
   std::string cid;
   std::string pid;

   std::string name;
   std::string type;
   std::string perm;

   std::string extra;

   std::string view;

   std::string module;
   std::string mclass;
   std::string bclass;

   std::string order;

   bool nexclude;

   std::string nclass;
   std::string nfield;
   std::string nextra;

   std::string dfenum;
   std::string dfield;
   std::string dvalue;

   std::string pclass;
   std::string pfield;
   std::string pfldnm;

   std::string ufield;

   std::string filters;

   std::string pdf_spec;

   std::vector< std::string > var_ids;

   std::map< std::string, std::string > extras;

   modifier_container modifiers;

   std::vector< std::string > actions;

   std::vector< fld_info > fields;

   std::vector< par_info > parents;

   std::vector< rest_info > restricts;
};

typedef std::map< std::string, enum_info > enum_container;
typedef enum_container::iterator enum_iterator;
typedef enum_container::const_iterator enum_const_iterator;

typedef std::vector< view_info > view_container;
typedef view_container::iterator view_iterator;
typedef view_container::const_iterator view_const_iterator;

typedef std::vector< list_info > list_container;
typedef list_container::iterator list_iterator;
typedef list_container::const_iterator list_const_iterator;

typedef std::map< std::string, view_info* > view_info_container;
typedef view_info_container::iterator view_info_iterator;
typedef view_info_container::const_iterator view_info_const_iterator;

typedef std::map< std::string, list_info* > list_info_container;
typedef list_info_container::iterator list_info_iterator;
typedef list_info_container::const_iterator list_info_const_iterator;

typedef std::map< std::string, list_info* > list_menu_container;
typedef list_menu_container::iterator list_menu_iterator;
typedef list_menu_container::const_iterator list_menu_const_iterator;

struct module_info
{
   module_info( )
    :
    user_select_is_strict( false ),
    allows_anonymous_access( false )
   {
   }

   time_t sio_mod;

   std::string id;
   std::string name;
   std::string perm;
   std::string title;

   std::string home_info;

   std::string anon_ginfo;

   std::string sys_class_id;
   std::string sys_name_field_id;
   std::string sys_vendor_field_id;
   std::string sys_actions_field_id;
   std::string sys_message_field_id;
   std::string sys_identity_field_id;

   std::string user_class_id;
   std::string user_class_name;

   std::string user_info_view_id;

   std::string user_uid_field_id;
   std::string user_hdl_field_id;
   std::string user_pwd_field_id;
   std::string user_hash_field_id;
   std::string user_name_field_id;
   std::string user_crypt_field_id;
   std::string user_email_field_id;

   std::string user_perm_field_id;
   std::string user_group_field_id;
   std::string user_other_field_id;
   std::string user_extra1_field_id;
   std::string user_extra2_field_id;
   std::string user_parent_field_id;
   std::string user_active_field_id;
   std::string user_slevel_field_id;
   std::string user_unique_field_id;
   std::string user_tz_name_field_id;
   std::string user_has_auth_field_id;
   std::string user_pin_value_field_id;
   std::string user_read_only_field_id;
   std::string user_gpg_install_proc_id;
   std::string user_change_pwd_tm_field_id;

   std::string user_select_perm;
   std::string user_select_field;
   std::string user_select_cfield;
   std::string user_select_ofield;
   std::string user_select_pfield;
   std::string user_select_str_key;
   std::string user_select_sl_field;
   std::string user_select_uo_field;

   bool user_select_is_strict;
   bool allows_anonymous_access;

   std::string user_qlink_list_id;
   std::string user_qlink_class_id;
   std::string user_qlink_pfield_id;
   std::string user_qlink_url_field_id;
   std::string user_qlink_name_field_id;
   std::string user_qlink_test_field_id;
   std::string user_qlink_test_field_val;
   std::string user_qlink_order_field_id;
   std::string user_qlink_checksum_field_id;

   std::string user_qlink_permission;

   view_container views;
   list_container lists;

   view_info_container view_info;
   list_info_container list_info;

   list_menu_container list_menus;

   std::map< std::string, std::string > view_cids;
   std::map< std::string, std::string > view_classes;

   std::map< std::string, std::string > strings;

   void clear( );

   const std::string& get_string( const std::string& key ) const;
};

typedef std::list< module_info > module_container;
typedef module_container::iterator module_iterator;
typedef module_container::const_iterator module_const_iterator;
typedef module_container::value_type module_value_type;

typedef std::map< std::string, module_info* > module_index_container;
typedef module_index_container::iterator module_index_iterator;
typedef module_index_container::const_iterator module_index_const_iterator;
typedef module_index_container::value_type module_index_value_type;

typedef std::deque< std::pair< std::string, std::string > > data_container;

typedef std::vector< data_container > parent_data_container;
typedef std::map< std::string, data_container > rel_data_container;

typedef std::map< std::string, std::string > row_error_container;

struct storage_info
{
   storage_info( );

   int row_limit;
   int notes_rmin;
   int notes_rmax;
   int notes_trunc;
   int notes_limit;
   int print_limit;

   int image_width;
   int image_height;

   bool is_devt;
   bool use_tls;
   bool embed_images;
   bool encrypt_data;
   bool view_show_key;
   bool checkbox_bools;
   bool hidden_userhash;

   size_t filesize_limit;

   size_t quick_link_limit;

   time_t sio_mod;
   time_t extkeys_mod;

   std::string url_opts;
   std::string menu_opts;

   int login_days;

   std::string login_opts;

   std::string blockchain;

   std::string rpc_password;

   std::string storage_name;
   std::string module_prefix;

   std::string print_list_opts;

   std::string user_info_view_id;
   std::string user_info_module_ref;

   std::string web_root;

   enum_container enums;

   module_container modules;
   module_index_container modules_index;

   mutex lock_mutex;

   void clear( );

   bool has_web_root( );

   std::string get_module_ref( const std::string& module_name );
};

struct session_info
{
   session_info( storage_info& si );

   bool is_default_other( ) const;

   bool locked;

   bool logged_in;
   bool needs_pin;

   bool is_meta_module;
   bool was_file_remove;

#  ifdef SSL_SUPPORT
   ssl_socket* p_socket;
#  else
   tcp_socket* p_socket;
#  endif

   std::string ip_addr;

   std::string session_id;

   std::string sys_message;

   std::string user_id;
   std::string user_key;
   std::string user_name;
   std::string user_crypt;
   std::string user_group;
   std::string user_mgrps;
   std::string user_other;
   std::string user_extra1;
   std::string user_extra2;
   std::string user_handle;
   std::string user_parent;
   std::string user_module;
   std::string user_slevel;
   std::string user_has_auth;
   std::string user_pin_value;

   bool pwd_encrypted;

   std::string user_pwd_hash;
   std::string user_key_hash;

   std::string content_key_hash;

   std::string default_user_group;
   std::string default_user_other;

   int checksum_serial;

   std::string hashval_prefix;
   std::string checksum_prefix;

   std::string last_uselextra;
   std::string last_user_pin_value;

   data_container select_data;

   data_container quick_link_data;

   std::map< std::string, std::string > user_perms;
   std::map< std::string, std::string > other_aliases;
   std::map< std::string, std::string > other_slevels;

   bool is_openid;
   bool is_read_only;

   bool is_admin_user;
   bool is_persistent;

   int64_t change_pwd_tm;

   int row_limit;

   int notes_rmin;
   int notes_rmax;
   int notes_trunc;
   int notes_limit;

   int print_limit;

   int image_width;
   int image_height;

   bool view_show_key;
   bool checkbox_bools;

   std::string print_list_opts;

   size_t quick_link_limit;

   std::string tz_abbr;
   std::string tz_name;

   int gmt_offset;
   int dtm_offset;

   time_t tm_created;
   time_t tm_last_request;

   std::string current_dtm;
};

typedef std::map< std::string, session_info* > session_container;
typedef session_container::value_type session_value_type;
typedef session_container::iterator session_iterator;
typedef session_container::const_iterator session_const_iterator;

struct source
{
   std::string id;
   std::string cid;
   std::string sid;
   std::string name;
   std::string type;
   std::string perm;

   std::string mclass;
   std::string bclass;
   std::string module;
   std::string module_id;
   std::string module_ref;

   std::string field_list;

   std::vector< std::string > field_ids;
   std::vector< std::string > value_ids;
   std::vector< std::string > fk_field_ids;
   std::vector< std::string > display_names;
   std::vector< std::string > edit_display_names;

   std::vector< std::string > text_search_title_fields;

   std::set< std::string > url_fields;
   std::set< std::string > link_fields;
   std::set< std::string > href_fields;
   std::set< std::string > html_fields;
   std::set< std::string > file_fields;
   std::set< std::string > text_fields;
   std::set< std::string > image_fields;
   std::set< std::string > large_fields;
   std::set< std::string > notes_fields;
   std::set< std::string > owner_fields;
   std::set< std::string > small_fields;
   std::set< std::string > hidden_fields;
   std::set< std::string > larger_fields;
   std::set< std::string > mailto_fields;
   std::set< std::string > qr_code_fields;
   std::set< std::string > smaller_fields;
   std::set< std::string > non_link_fields;
   std::set< std::string > encrypted_fields;
   std::set< std::string > non_print_fields;
   std::set< std::string > admin_link_fields;
   std::set< std::string > owner_link_fields;
   std::set< std::string > print_only_fields;
   std::set< std::string > manual_link_fields;
   std::set< std::string > non_fk_link_fields;
   std::set< std::string > manual_link_ignores;
   std::set< std::string > replace_underbar_fields;
   std::set< std::string > default_other_link_fields;

   std::map< std::string, std::string > uom_fields;
   std::map< std::string, std::string > enum_fields;
   std::map< std::string, std::string > mask_fields;
   std::map< std::string, std::string > pmask_fields;

   std::string pdf_spec_name;

   std::string special_field;
   std::string filename_field;
   std::string orientation_field;

   std::string permission_field;
   std::string security_level_field;
   std::string ignore_encrypted_field;
   std::string is_effective_owner_field;

   std::set< std::string > bool_fields;
   std::set< std::string > date_fields;
   std::set< std::string > time_fields;
   std::set< std::string > datetime_fields;
   std::set< std::string > int_fields;
   std::set< std::string > numeric_fields;

   std::set< std::string > force_left_fields;
   std::set< std::string > force_right_fields;
   std::set< std::string > force_center_fields;
   std::set< std::string > force_justify_fields;

   std::map< std::string, std::string > fk_field_classes;

   std::map< std::string, std::string > int_type_fields;
   std::map< std::string, std::string > numeric_type_fields;
   std::map< std::string, std::string > date_precision_fields;
   std::map< std::string, std::string > time_precision_fields;
};

struct ids
{
   std::string id0;
   std::string id1;
   std::string id2;
   std::string id3;
   std::string id4;
   std::string id5;
   std::string id6;
   std::string id7;
   std::string id8;
   std::string id9;
};

struct values
{
   std::string value0;
   std::string value1;
   std::string value2;
   std::string value3;
   std::string value4;
   std::string value5;
   std::string value6;
   std::string value7;
   std::string value8;
   std::string value9;
};

struct view_source : source
{
   view_source( ) : state( 0 ), has_quick_link( false ), has_file_attachments( false ) { }

   std::string root_folder;
   std::string new_file_name;

   std::string actions_extra;
   std::string actions_field;
   std::string qr_scan_field;
   std::string filename_field;
   std::string key_suffix_field;
   std::string owning_user_field;
   std::string qr_scan_key_field;
   std::string key_from_hash_field;
   std::string hpassword_salt_field;
   std::string create_user_key_field;
   std::string modify_user_key_field;
   std::string create_datetime_field;
   std::string modify_datetime_field;

   view_info_const_iterator vici;

   std::set< std::string > upper_fields;
   std::set< std::string > password_fields;
   std::set< std::string > hpassword_fields;
   std::set< std::string > protected_fields;
   std::set< std::string > mandatory_fields;
   std::set< std::string > defcurrent_fields;
   std::set< std::string > field_hash_fields;
   std::set< std::string > self_relationships;
   std::set< std::string > defcurrentyear_fields;

   std::map< std::string, std::string > enum_filter_fields;

   rel_data_container parent_lists;

   uint64_t state;
   std::string key_info;

   ids key_ids;
   values key_values;

   ids fkey_ids;
   values fkey_values;

   std::string vextra1_id;
   std::string vextra2_id;
   std::string vextra1_value;
   std::string vextra2_value;

   bool has_quick_link;
   std::string quick_link_value_id;

   bool has_file_attachments;

   std::string actions_value;

   std::vector< std::string > tab_names;
   std::vector< std::string > tab_extras;

   std::vector< int > field_tab_ids;

   std::vector< std::string > user_force_fields;
   std::vector< std::string > server_forced_fields;

   std::map< std::string, std::string > field_values;
   std::map< std::string, std::string > fk_field_values;
   std::map< std::string, std::string > new_field_values;
};

struct list_source : source
{
   list_source( )
    :
    sort_field( 0 ),
    is_reverse( false ),
    unique_index( false ),
    print_limited( false ),
    can_action_any( false ),
    can_delete_any( false ),
    new_record_list_has_view_id( false )
   {
   }

   std::string view;
   std::string pfield;

   int sort_field;
   bool is_reverse;

   bool print_limited;

   bool can_action_any;
   bool can_delete_any;

   std::string pfield_list;

   bool unique_index;
   std::string all_index_fields;
   std::string first_index_field;

   std::string new_pkey;
   std::string new_pfield;

   std::string next_key_info;
   std::string prev_key_info;

   list_info_const_iterator lici;

   data_container row_data;
   data_container new_record_list;

   bool new_record_list_has_view_id;

   parent_data_container parent_lists;

   row_error_container row_errors;

   std::set< std::string > print_total_fields;

   std::map< std::string, uint64_t > pstate_fields;

   std::map< std::string, int > notes_character_trunc;

   std::set< std::string > omit_label_fields;
   std::set< std::string > use_list_title_fields;
   std::set< std::string > special_prefixed_fields;
   std::set< std::string > special_suffixed_fields;

   std::set< std::string > print_summary_fields;
   std::vector< std::string > print_summary_field_ids;

   std::set< std::string > sort_fields;
   std::vector< std::pair< std::string, bool > > index_fields;
};

storage_info& get_storage_info( );

inline bool is_blockchain_application( ) { return !get_storage_info( ).blockchain.empty( ); }

void read_storage_info( storage_info& info, std::vector< std::string > log_messages );

bool read_module_info( const std::string& name, module_info& info, storage_info& sinfo );

void sort_row_data_manually( data_container& row_data, bool remove_manual_links = false );

#endif
