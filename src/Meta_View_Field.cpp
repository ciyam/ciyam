// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstring>
#  include <fstream>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#define CIYAM_BASE_LIB
#define MODULE_META_IMPL

// [<start macros>]
// [<finish macros>]

#include "Meta_View_Field.h"

#include "Meta_Permission.h"
#include "Meta_Class.h"
#include "Meta_Specification.h"
#include "Meta_Field.h"
#include "Meta_View_Field_Type.h"
#include "Meta_View.h"

#include "ciyam_base.h"
#include "ciyam_core.h"
#include "class_domains.h"
#include "ciyam_channels.h"
#include "module_strings.h"
#include "ciyam_constants.h"
#include "ciyam_variables.h"
#include "class_utilities.h"
#include "command_handler.h"
#include "module_interface.h"

// [(start modifier_field_null)] 600398
#include "Meta_Enum.h"
// [(finish modifier_field_null)] 600398

// [<start includes>]
//nyi
#include "Meta_Enum.h"
// [<finish includes>]

using namespace std;

// [<start namespaces>]
// [<finish namespaces>]

template< > inline string to_string( const Meta_Permission& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Class& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Specification& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Field& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_View_Field_Type& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_View& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Permission& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Class& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Specification& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Field& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_View_Field_Type& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_View& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline int system( const string& cmd ) { return exec_system( cmd ); }

namespace
{

template< typename T > inline void sanity_check( const T& t ) { }

inline void sanity_check( const string& s )
{
   if( s.length( ) > c_max_string_length_limit )
      throw runtime_error( "unexpected max string length limit exceeded with: " + s );
}

#include "Meta_View_Field.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Access_Permission = "301920";
const char* const c_field_id_Access_Restriction = "120105";
const char* const c_field_id_Access_Scope = "120119";
const char* const c_field_id_Alignment = "120131";
const char* const c_field_id_Allow_Anonymous_Access = "120129";
const char* const c_field_id_Change_Permission = "301930";
const char* const c_field_id_Change_Restriction = "120106";
const char* const c_field_id_Change_Scope = "120120";
const char* const c_field_id_Child_List_Extra_Option = "120127";
const char* const c_field_id_Class = "301905";
const char* const c_field_id_Date_Precision_Option = "120118";
const char* const c_field_id_Enum_Finishes_At = "120124";
const char* const c_field_id_Enum_Starts_At = "120123";
const char* const c_field_id_FK_Trigger_Behaviour = "120112";
const char* const c_field_id_FK_Trigger_Option = "120111";
const char* const c_field_id_Font_Size = "120125";
const char* const c_field_id_Ignore_Manual_Links = "120126";
const char* const c_field_id_Label_Source_Child = "120128";
const char* const c_field_id_Link_Permission = "301915";
const char* const c_field_id_Link_Restriction = "120108";
const char* const c_field_id_Mandatory_Option = "120114";
const char* const c_field_id_Name = "120102";
const char* const c_field_id_New_Source = "120109";
const char* const c_field_id_New_Value = "120117";
const char* const c_field_id_Order = "120101";
const char* const c_field_id_Orientation = "120132";
const char* const c_field_id_Restriction_Spec = "301980";
const char* const c_field_id_Show_Hide_Start_Point = "120116";
const char* const c_field_id_Sort_Manually = "120115";
const char* const c_field_id_Source_Child = "301960";
const char* const c_field_id_Source_Edit_Child = "301965";
const char* const c_field_id_Source_Field = "301940";
const char* const c_field_id_Source_Parent = "301950";
const char* const c_field_id_Source_Parent_Class = "301970";
const char* const c_field_id_Tab_Name = "120103";
const char* const c_field_id_Trigger_Behaviour = "120110";
const char* const c_field_id_Trigger_For_State = "120122";
const char* const c_field_id_Trigger_Option = "120113";
const char* const c_field_id_Type = "301910";
const char* const c_field_id_Use_Full_Height = "120130";
const char* const c_field_id_Use_Full_Width = "120121";
const char* const c_field_id_Use_Source_Parent = "120107";
const char* const c_field_id_View = "301900";

const char* const c_field_name_Access_Permission = "Access_Permission";
const char* const c_field_name_Access_Restriction = "Access_Restriction";
const char* const c_field_name_Access_Scope = "Access_Scope";
const char* const c_field_name_Alignment = "Alignment";
const char* const c_field_name_Allow_Anonymous_Access = "Allow_Anonymous_Access";
const char* const c_field_name_Change_Permission = "Change_Permission";
const char* const c_field_name_Change_Restriction = "Change_Restriction";
const char* const c_field_name_Change_Scope = "Change_Scope";
const char* const c_field_name_Child_List_Extra_Option = "Child_List_Extra_Option";
const char* const c_field_name_Class = "Class";
const char* const c_field_name_Date_Precision_Option = "Date_Precision_Option";
const char* const c_field_name_Enum_Finishes_At = "Enum_Finishes_At";
const char* const c_field_name_Enum_Starts_At = "Enum_Starts_At";
const char* const c_field_name_FK_Trigger_Behaviour = "FK_Trigger_Behaviour";
const char* const c_field_name_FK_Trigger_Option = "FK_Trigger_Option";
const char* const c_field_name_Font_Size = "Font_Size";
const char* const c_field_name_Ignore_Manual_Links = "Ignore_Manual_Links";
const char* const c_field_name_Label_Source_Child = "Label_Source_Child";
const char* const c_field_name_Link_Permission = "Link_Permission";
const char* const c_field_name_Link_Restriction = "Link_Restriction";
const char* const c_field_name_Mandatory_Option = "Mandatory_Option";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_New_Source = "New_Source";
const char* const c_field_name_New_Value = "New_Value";
const char* const c_field_name_Order = "Order";
const char* const c_field_name_Orientation = "Orientation";
const char* const c_field_name_Restriction_Spec = "Restriction_Spec";
const char* const c_field_name_Show_Hide_Start_Point = "Show_Hide_Start_Point";
const char* const c_field_name_Sort_Manually = "Sort_Manually";
const char* const c_field_name_Source_Child = "Source_Child";
const char* const c_field_name_Source_Edit_Child = "Source_Edit_Child";
const char* const c_field_name_Source_Field = "Source_Field";
const char* const c_field_name_Source_Parent = "Source_Parent";
const char* const c_field_name_Source_Parent_Class = "Source_Parent_Class";
const char* const c_field_name_Tab_Name = "Tab_Name";
const char* const c_field_name_Trigger_Behaviour = "Trigger_Behaviour";
const char* const c_field_name_Trigger_For_State = "Trigger_For_State";
const char* const c_field_name_Trigger_Option = "Trigger_Option";
const char* const c_field_name_Type = "Type";
const char* const c_field_name_Use_Full_Height = "Use_Full_Height";
const char* const c_field_name_Use_Full_Width = "Use_Full_Width";
const char* const c_field_name_Use_Source_Parent = "Use_Source_Parent";
const char* const c_field_name_View = "View";

const char* const c_field_display_name_Access_Permission = "field_view_field_access_permission";
const char* const c_field_display_name_Access_Restriction = "field_view_field_access_restriction";
const char* const c_field_display_name_Access_Scope = "field_view_field_access_scope";
const char* const c_field_display_name_Alignment = "field_view_field_alignment";
const char* const c_field_display_name_Allow_Anonymous_Access = "field_view_field_allow_anonymous_access";
const char* const c_field_display_name_Change_Permission = "field_view_field_change_permission";
const char* const c_field_display_name_Change_Restriction = "field_view_field_change_restriction";
const char* const c_field_display_name_Change_Scope = "field_view_field_change_scope";
const char* const c_field_display_name_Child_List_Extra_Option = "field_view_field_child_list_extra_option";
const char* const c_field_display_name_Class = "field_view_field_class";
const char* const c_field_display_name_Date_Precision_Option = "field_view_field_date_precision_option";
const char* const c_field_display_name_Enum_Finishes_At = "field_view_field_enum_finishes_at";
const char* const c_field_display_name_Enum_Starts_At = "field_view_field_enum_starts_at";
const char* const c_field_display_name_FK_Trigger_Behaviour = "field_view_field_fk_trigger_behaviour";
const char* const c_field_display_name_FK_Trigger_Option = "field_view_field_fk_trigger_option";
const char* const c_field_display_name_Font_Size = "field_view_field_font_size";
const char* const c_field_display_name_Ignore_Manual_Links = "field_view_field_ignore_manual_links";
const char* const c_field_display_name_Label_Source_Child = "field_view_field_label_source_child";
const char* const c_field_display_name_Link_Permission = "field_view_field_link_permission";
const char* const c_field_display_name_Link_Restriction = "field_view_field_link_restriction";
const char* const c_field_display_name_Mandatory_Option = "field_view_field_mandatory_option";
const char* const c_field_display_name_Name = "field_view_field_name";
const char* const c_field_display_name_New_Source = "field_view_field_new_source";
const char* const c_field_display_name_New_Value = "field_view_field_new_value";
const char* const c_field_display_name_Order = "field_view_field_order";
const char* const c_field_display_name_Orientation = "field_view_field_orientation";
const char* const c_field_display_name_Restriction_Spec = "field_view_field_restriction_spec";
const char* const c_field_display_name_Show_Hide_Start_Point = "field_view_field_show_hide_start_point";
const char* const c_field_display_name_Sort_Manually = "field_view_field_sort_manually";
const char* const c_field_display_name_Source_Child = "field_view_field_source_child";
const char* const c_field_display_name_Source_Edit_Child = "field_view_field_source_edit_child";
const char* const c_field_display_name_Source_Field = "field_view_field_source_field";
const char* const c_field_display_name_Source_Parent = "field_view_field_source_parent";
const char* const c_field_display_name_Source_Parent_Class = "field_view_field_source_parent_class";
const char* const c_field_display_name_Tab_Name = "field_view_field_tab_name";
const char* const c_field_display_name_Trigger_Behaviour = "field_view_field_trigger_behaviour";
const char* const c_field_display_name_Trigger_For_State = "field_view_field_trigger_for_state";
const char* const c_field_display_name_Trigger_Option = "field_view_field_trigger_option";
const char* const c_field_display_name_Type = "field_view_field_type";
const char* const c_field_display_name_Use_Full_Height = "field_view_field_use_full_height";
const char* const c_field_display_name_Use_Full_Width = "field_view_field_use_full_width";
const char* const c_field_display_name_Use_Source_Parent = "field_view_field_use_source_parent";
const char* const c_field_display_name_View = "field_view_field_view";

const int c_num_fields = 43;

const char* const c_all_sorted_field_ids[ ] =
{
   "120101",
   "120102",
   "120103",
   "120105",
   "120106",
   "120107",
   "120108",
   "120109",
   "120110",
   "120111",
   "120112",
   "120113",
   "120114",
   "120115",
   "120116",
   "120117",
   "120118",
   "120119",
   "120120",
   "120121",
   "120122",
   "120123",
   "120124",
   "120125",
   "120126",
   "120127",
   "120128",
   "120129",
   "120130",
   "120131",
   "120132",
   "301900",
   "301905",
   "301910",
   "301915",
   "301920",
   "301930",
   "301940",
   "301950",
   "301960",
   "301965",
   "301970",
   "301980"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Access_Permission",
   "Access_Restriction",
   "Access_Scope",
   "Alignment",
   "Allow_Anonymous_Access",
   "Change_Permission",
   "Change_Restriction",
   "Change_Scope",
   "Child_List_Extra_Option",
   "Class",
   "Date_Precision_Option",
   "Enum_Finishes_At",
   "Enum_Starts_At",
   "FK_Trigger_Behaviour",
   "FK_Trigger_Option",
   "Font_Size",
   "Ignore_Manual_Links",
   "Label_Source_Child",
   "Link_Permission",
   "Link_Restriction",
   "Mandatory_Option",
   "Name",
   "New_Source",
   "New_Value",
   "Order",
   "Orientation",
   "Restriction_Spec",
   "Show_Hide_Start_Point",
   "Sort_Manually",
   "Source_Child",
   "Source_Edit_Child",
   "Source_Field",
   "Source_Parent",
   "Source_Parent_Class",
   "Tab_Name",
   "Trigger_Behaviour",
   "Trigger_For_State",
   "Trigger_Option",
   "Type",
   "Use_Full_Height",
   "Use_Full_Width",
   "Use_Source_Parent",
   "View"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool has_field( const string& field )
{
   return binary_search( c_all_sorted_field_ids, c_all_sorted_field_ids + c_num_fields, field.c_str( ), compare )
    || binary_search( c_all_sorted_field_names, c_all_sorted_field_names + c_num_fields, field.c_str( ), compare );
}

const int c_num_encrypted_fields = 0;

bool is_encrypted_field( const string& ) { static bool false_value( false ); return false_value; }

const int c_num_transient_fields = 1;

const char* const c_transient_sorted_field_ids[ ] =
{
   "120102"
};

const char* const c_transient_sorted_field_names[ ] =
{
   "Name"
};

inline bool is_transient_field( const string& field )
{
   return binary_search( c_transient_sorted_field_ids,
    c_transient_sorted_field_ids + c_num_transient_fields, field.c_str( ), compare )
    || binary_search( c_transient_sorted_field_names,
    c_transient_sorted_field_names + c_num_transient_fields, field.c_str( ), compare );
}

const char* const c_procedure_id_Move_Down = "120420";
const char* const c_procedure_id_Move_Up = "120410";

const uint64_t c_modifier_Hide_FK_Fields = UINT64_C( 0x100 );
const uint64_t c_modifier_Hide_New_Value = UINT64_C( 0x200 );
const uint64_t c_modifier_Hide_Non_FK_Fields = UINT64_C( 0x400 );
const uint64_t c_modifier_Is_Field = UINT64_C( 0x800 );
const uint64_t c_modifier_Is_File_Or_Image = UINT64_C( 0x1000 );
const uint64_t c_modifier_Is_Key = UINT64_C( 0x2000 );
const uint64_t c_modifier_Is_Not_Date = UINT64_C( 0x4000 );
const uint64_t c_modifier_Is_Not_Enum = UINT64_C( 0x8000 );
const uint64_t c_modifier_Is_Not_Image = UINT64_C( 0x10000 );
const uint64_t c_modifier_Is_Print_Version = UINT64_C( 0x20000 );
const uint64_t c_modifier_Is_Tab = UINT64_C( 0x40000 );
const uint64_t c_modifier_Protect_Access_Restriction = UINT64_C( 0x80000 );
const uint64_t c_modifier_Protect_Access_Scope = UINT64_C( 0x100000 );
const uint64_t c_modifier_Protect_Change_Restriction = UINT64_C( 0x200000 );
const uint64_t c_modifier_Protect_Change_Scope = UINT64_C( 0x400000 );

domain_string_max_size< 100 > g_Name_domain;
domain_string_max_size< 100 > g_New_Value_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 100 > > g_Tab_Name_domain;

string g_group_field_name;
string g_level_field_name;
string g_order_field_name( "Order" );
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_View_Field* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_View_Field* > external_aliases_lookup_container;
typedef external_aliases_lookup_container::const_iterator external_aliases_lookup_const_iterator;

external_aliases_container g_external_aliases;
external_aliases_lookup_container g_external_aliases_lookup;

struct validate_formatter
{
   validate_formatter( ) : num( 0 ) { }

   string get( const string& name ) { return masks[ name ]; }

   void set( const string& name, const string& mask )
   {
      masks.insert( make_pair( name, mask ) );
   }

   int num;

   map< string, string > masks;
};

inline validation_error_value_type
 construct_validation_error( int& num, const string& field_name, const string& error_message )
{
   return validation_error_value_type(
    construct_key_from_int( "", ++num, 4 ) + ':' + field_name, error_message );
}

string g_default_Access_Permission = string( );
int g_default_Access_Restriction = int( 0 );
int g_default_Access_Scope = int( 0 );
int g_default_Alignment = int( 0 );
bool g_default_Allow_Anonymous_Access = bool( 1 );
string g_default_Change_Permission = string( );
int g_default_Change_Restriction = int( 0 );
int g_default_Change_Scope = int( 0 );
int g_default_Child_List_Extra_Option = int( 0 );
string g_default_Class = string( );
int g_default_Date_Precision_Option = int( 0 );
int g_default_Enum_Finishes_At = int( 0 );
int g_default_Enum_Starts_At = int( 0 );
int g_default_FK_Trigger_Behaviour = int( 0 );
int g_default_FK_Trigger_Option = int( 0 );
int g_default_Font_Size = int( 0 );
bool g_default_Ignore_Manual_Links = bool( 0 );
int g_default_Label_Source_Child = int( 0 );
string g_default_Link_Permission = string( );
int g_default_Link_Restriction = int( 4 );
int g_default_Mandatory_Option = int( 0 );
string g_default_Name = string( );
int g_default_New_Source = int( 0 );
string g_default_New_Value = string( );
string g_default_Order = string( );
int g_default_Orientation = int( 0 );
string g_default_Restriction_Spec = string( );
bool g_default_Show_Hide_Start_Point = bool( 0 );
bool g_default_Sort_Manually = bool( 0 );
string g_default_Source_Child = string( );
string g_default_Source_Edit_Child = string( );
string g_default_Source_Field = string( );
string g_default_Source_Parent = string( );
string g_default_Source_Parent_Class = string( );
string g_default_Tab_Name = string( );
int g_default_Trigger_Behaviour = int( 0 );
int g_default_Trigger_For_State = int( 0 );
int g_default_Trigger_Option = int( 0 );
string g_default_Type = string( );
bool g_default_Use_Full_Height = bool( 0 );
bool g_default_Use_Full_Width = bool( 0 );
bool g_default_Use_Source_Parent = bool( 0 );
string g_default_View = string( );

set< int > g_view_field_restrict_enum;
set< int > g_view_field_access_scope_enum;
set< int > g_field_alignment_enum;
set< int > g_view_field_change_scope_enum;
set< int > g_view_field_child_list_extra_option_enum;
set< int > g_view_field_date_precision_option_enum;
set< int > g_view_field_enum_at_enum;
set< int > g_view_field_fk_trigger_behaviour_enum;
set< int > g_view_field_fk_trigger_option_enum;
set< int > g_font_size_enum;
set< int > g_label_source_child_enum;
set< int > g_view_field_link_restrict_enum;
set< int > g_view_field_mandatory_option_enum;
set< int > g_view_field_new_source_enum;
set< int > g_orientation_enum;
set< int > g_view_field_trigger_behaviour_enum;
set< int > g_view_field_trigger_for_state_enum;
set< int > g_view_field_trigger_option_enum;

const int c_enum_view_field_restrict_none( 0 );
const int c_enum_view_field_restrict_owner_only( 1 );
const int c_enum_view_field_restrict_admin_only( 2 );
const int c_enum_view_field_restrict_admin_owner( 3 );
const int c_enum_view_field_restrict_denied_always( 4 );

string get_enum_string_view_field_restrict( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_field_restrict" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_field_restrict_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_view_field_restrict_owner_only";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_field_restrict_admin_only";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_view_field_restrict_admin_owner";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_view_field_restrict_denied_always";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_field_restrict" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_view_field_access_scope_all( 0 );
const int c_enum_view_field_access_scope_create_only( 1 );
const int c_enum_view_field_access_scope_post_create( 2 );
const int c_enum_view_field_access_scope_editing_only( 3 );
const int c_enum_view_field_access_scope_viewing_only( 4 );
const int c_enum_view_field_access_scope_updating_only( 5 );

string get_enum_string_view_field_access_scope( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_field_access_scope" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_field_access_scope_all";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_view_field_access_scope_create_only";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_field_access_scope_post_create";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_view_field_access_scope_editing_only";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_view_field_access_scope_viewing_only";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_view_field_access_scope_updating_only";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_field_access_scope" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_field_alignment_default( 0 );
const int c_enum_field_alignment_left( 1 );
const int c_enum_field_alignment_right( 2 );
const int c_enum_field_alignment_center( 3 );
const int c_enum_field_alignment_justify( 4 );

string get_enum_string_field_alignment( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for field_alignment" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_field_alignment_default";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_field_alignment_left";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_field_alignment_right";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_field_alignment_center";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_field_alignment_justify";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for field_alignment" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_view_field_change_scope_all( 0 );
const int c_enum_view_field_change_scope_create_only( 1 );
const int c_enum_view_field_change_scope_update_only( 2 );
const int c_enum_view_field_change_scope_always_editable( 3 );

string get_enum_string_view_field_change_scope( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_field_change_scope" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_field_change_scope_all";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_view_field_change_scope_create_only";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_field_change_scope_update_only";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_view_field_change_scope_always_editable";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_field_change_scope" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_view_field_child_list_extra_option_none( 0 );
const int c_enum_view_field_child_list_extra_option_extra1( 1 );
const int c_enum_view_field_child_list_extra_option_extra2( 2 );

string get_enum_string_view_field_child_list_extra_option( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_field_child_list_extra_option" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_field_child_list_extra_option_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_view_field_child_list_extra_option_extra1";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_field_child_list_extra_option_extra2";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_field_child_list_extra_option" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_view_field_date_precision_option_from_field( 0 );
const int c_enum_view_field_date_precision_option_force_months( 1 );
const int c_enum_view_field_date_precision_option_force_decades( 2 );

string get_enum_string_view_field_date_precision_option( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_field_date_precision_option" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_field_date_precision_option_from_field";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_view_field_date_precision_option_force_months";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_field_date_precision_option_force_decades";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_field_date_precision_option" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_view_field_enum_at_normal( 0 );
const int c_enum_view_field_enum_at_key1( 1 );
const int c_enum_view_field_enum_at_key2( 2 );
const int c_enum_view_field_enum_at_key3( 3 );
const int c_enum_view_field_enum_at_key4( 4 );
const int c_enum_view_field_enum_at_key5( 5 );
const int c_enum_view_field_enum_at_key6( 6 );
const int c_enum_view_field_enum_at_key7( 7 );
const int c_enum_view_field_enum_at_key8( 8 );
const int c_enum_view_field_enum_at_key9( 9 );
const int c_enum_view_field_enum_at_key0( 10 );
const int c_enum_view_field_enum_at_fkey1( 11 );
const int c_enum_view_field_enum_at_fkey2( 12 );
const int c_enum_view_field_enum_at_fkey3( 13 );
const int c_enum_view_field_enum_at_fkey4( 14 );
const int c_enum_view_field_enum_at_fkey5( 15 );
const int c_enum_view_field_enum_at_fkey6( 16 );
const int c_enum_view_field_enum_at_fkey7( 17 );
const int c_enum_view_field_enum_at_fkey8( 18 );
const int c_enum_view_field_enum_at_fkey9( 19 );
const int c_enum_view_field_enum_at_fkey0( 20 );

string get_enum_string_view_field_enum_at( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_field_enum_at" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_field_enum_at_normal";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_view_field_enum_at_key1";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_field_enum_at_key2";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_view_field_enum_at_key3";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_view_field_enum_at_key4";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_view_field_enum_at_key5";
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_view_field_enum_at_key6";
   else if( to_string( val ) == to_string( "7" ) )
      string_name = "enum_view_field_enum_at_key7";
   else if( to_string( val ) == to_string( "8" ) )
      string_name = "enum_view_field_enum_at_key8";
   else if( to_string( val ) == to_string( "9" ) )
      string_name = "enum_view_field_enum_at_key9";
   else if( to_string( val ) == to_string( "10" ) )
      string_name = "enum_view_field_enum_at_key0";
   else if( to_string( val ) == to_string( "11" ) )
      string_name = "enum_view_field_enum_at_fkey1";
   else if( to_string( val ) == to_string( "12" ) )
      string_name = "enum_view_field_enum_at_fkey2";
   else if( to_string( val ) == to_string( "13" ) )
      string_name = "enum_view_field_enum_at_fkey3";
   else if( to_string( val ) == to_string( "14" ) )
      string_name = "enum_view_field_enum_at_fkey4";
   else if( to_string( val ) == to_string( "15" ) )
      string_name = "enum_view_field_enum_at_fkey5";
   else if( to_string( val ) == to_string( "16" ) )
      string_name = "enum_view_field_enum_at_fkey6";
   else if( to_string( val ) == to_string( "17" ) )
      string_name = "enum_view_field_enum_at_fkey7";
   else if( to_string( val ) == to_string( "18" ) )
      string_name = "enum_view_field_enum_at_fkey8";
   else if( to_string( val ) == to_string( "19" ) )
      string_name = "enum_view_field_enum_at_fkey9";
   else if( to_string( val ) == to_string( "20" ) )
      string_name = "enum_view_field_enum_at_fkey0";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_field_enum_at" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_view_field_fk_trigger_behaviour_normal( 0 );
const int c_enum_view_field_fk_trigger_behaviour_send_always( 1 );
const int c_enum_view_field_fk_trigger_behaviour_fetch_always( 2 );
const int c_enum_view_field_fk_trigger_behaviour_send_and_fetch_always( 3 );

string get_enum_string_view_field_fk_trigger_behaviour( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_field_fk_trigger_behaviour" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_field_fk_trigger_behaviour_normal";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_view_field_fk_trigger_behaviour_send_always";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_field_fk_trigger_behaviour_fetch_always";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_view_field_fk_trigger_behaviour_send_and_fetch_always";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_field_fk_trigger_behaviour" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_view_field_fk_trigger_option_none( 0 );
const int c_enum_view_field_fk_trigger_option_fkey1( 1 );
const int c_enum_view_field_fk_trigger_option_fkey2( 2 );
const int c_enum_view_field_fk_trigger_option_fkey3( 3 );
const int c_enum_view_field_fk_trigger_option_fkey4( 4 );
const int c_enum_view_field_fk_trigger_option_fkey5( 5 );
const int c_enum_view_field_fk_trigger_option_fkey6( 6 );
const int c_enum_view_field_fk_trigger_option_fkey7( 7 );
const int c_enum_view_field_fk_trigger_option_fkey8( 8 );
const int c_enum_view_field_fk_trigger_option_fkey9( 9 );
const int c_enum_view_field_fk_trigger_option_fkey0( 10 );

string get_enum_string_view_field_fk_trigger_option( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_field_fk_trigger_option" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_field_fk_trigger_option_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_view_field_fk_trigger_option_fkey1";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_field_fk_trigger_option_fkey2";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_view_field_fk_trigger_option_fkey3";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_view_field_fk_trigger_option_fkey4";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_view_field_fk_trigger_option_fkey5";
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_view_field_fk_trigger_option_fkey6";
   else if( to_string( val ) == to_string( "7" ) )
      string_name = "enum_view_field_fk_trigger_option_fkey7";
   else if( to_string( val ) == to_string( "8" ) )
      string_name = "enum_view_field_fk_trigger_option_fkey8";
   else if( to_string( val ) == to_string( "9" ) )
      string_name = "enum_view_field_fk_trigger_option_fkey9";
   else if( to_string( val ) == to_string( "10" ) )
      string_name = "enum_view_field_fk_trigger_option_fkey0";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_field_fk_trigger_option" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_font_size_default( 0 );
const int c_enum_font_size_large( 1 );
const int c_enum_font_size_larger( 2 );
const int c_enum_font_size_small( 5 );
const int c_enum_font_size_smaller( 6 );

string get_enum_string_font_size( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for font_size" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_font_size_default";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_font_size_large";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_font_size_larger";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_font_size_small";
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_font_size_smaller";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for font_size" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_label_source_child_default( 0 );
const int c_enum_label_source_child_non_prefixed( 1 );
const int c_enum_label_source_child_just_child_always( 2 );

string get_enum_string_label_source_child( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for label_source_child" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_label_source_child_default";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_label_source_child_non_prefixed";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_label_source_child_just_child_always";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for label_source_child" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_view_field_link_restrict_none( 0 );
const int c_enum_view_field_link_restrict_owner_only( 1 );
const int c_enum_view_field_link_restrict_admin_only( 2 );
const int c_enum_view_field_link_restrict_admin_owner( 3 );
const int c_enum_view_field_link_restrict_denied_always( 4 );

string get_enum_string_view_field_link_restrict( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_field_link_restrict" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_field_link_restrict_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_view_field_link_restrict_owner_only";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_field_link_restrict_admin_only";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_view_field_link_restrict_admin_owner";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_view_field_link_restrict_denied_always";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_field_link_restrict" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_view_field_mandatory_option_from_field( 0 );
const int c_enum_view_field_mandatory_option_force_optional( 1 );
const int c_enum_view_field_mandatory_option_force_mandatory( 2 );

string get_enum_string_view_field_mandatory_option( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_field_mandatory_option" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_field_mandatory_option_from_field";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_view_field_mandatory_option_force_optional";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_field_mandatory_option_force_mandatory";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_field_mandatory_option" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_view_field_new_source_none( 0 );
const int c_enum_view_field_new_source_user( 1 );
const int c_enum_view_field_new_source_group( 2 );
const int c_enum_view_field_new_source_other( 3 );
const int c_enum_view_field_new_source_value( 4 );

string get_enum_string_view_field_new_source( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_field_new_source" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_field_new_source_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_view_field_new_source_user";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_field_new_source_group";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_view_field_new_source_other";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_view_field_new_source_value";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_field_new_source" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_orientation_landscape( 0 );
const int c_enum_orientation_portrait( 1 );
const int c_enum_orientation_neither( 2 );

string get_enum_string_orientation( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for orientation" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_orientation_landscape";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_orientation_portrait";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_orientation_neither";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for orientation" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_view_field_trigger_behaviour_normal( 0 );
const int c_enum_view_field_trigger_behaviour_send_always( 1 );
const int c_enum_view_field_trigger_behaviour_fetch_always( 2 );
const int c_enum_view_field_trigger_behaviour_send_and_fetch_always( 3 );

string get_enum_string_view_field_trigger_behaviour( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_field_trigger_behaviour" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_field_trigger_behaviour_normal";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_view_field_trigger_behaviour_send_always";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_field_trigger_behaviour_fetch_always";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_view_field_trigger_behaviour_send_and_fetch_always";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_field_trigger_behaviour" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_view_field_trigger_for_state_editable( 0 );
const int c_enum_view_field_trigger_for_state_protected( 1 );
const int c_enum_view_field_trigger_for_state_relegated( 2 );

string get_enum_string_view_field_trigger_for_state( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_field_trigger_for_state" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_field_trigger_for_state_editable";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_view_field_trigger_for_state_protected";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_field_trigger_for_state_relegated";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_field_trigger_for_state" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_view_field_trigger_option_none( 0 );
const int c_enum_view_field_trigger_option_key1( 1 );
const int c_enum_view_field_trigger_option_key2( 2 );
const int c_enum_view_field_trigger_option_key3( 3 );
const int c_enum_view_field_trigger_option_key4( 4 );
const int c_enum_view_field_trigger_option_key5( 5 );
const int c_enum_view_field_trigger_option_key6( 6 );
const int c_enum_view_field_trigger_option_key7( 7 );
const int c_enum_view_field_trigger_option_key8( 8 );
const int c_enum_view_field_trigger_option_key9( 9 );
const int c_enum_view_field_trigger_option_key0( 10 );

string get_enum_string_view_field_trigger_option( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_field_trigger_option" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_field_trigger_option_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_view_field_trigger_option_key1";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_field_trigger_option_key2";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_view_field_trigger_option_key3";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_view_field_trigger_option_key4";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_view_field_trigger_option_key5";
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_view_field_trigger_option_key6";
   else if( to_string( val ) == to_string( "7" ) )
      string_name = "enum_view_field_trigger_option_key7";
   else if( to_string( val ) == to_string( "8" ) )
      string_name = "enum_view_field_trigger_option_key8";
   else if( to_string( val ) == to_string( "9" ) )
      string_name = "enum_view_field_trigger_option_key9";
   else if( to_string( val ) == to_string( "10" ) )
      string_name = "enum_view_field_trigger_option_key0";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_field_trigger_option" );

   return get_module_string( lower( string_name ) );
}

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_View_Field > View_Field_registration( get_class_registry( ), "120100" );

class Meta_View_Field_command_functor;

class Meta_View_Field_command_handler : public command_handler
{
   friend class Meta_View_Field_command_functor;

   public:
   Meta_View_Field_command_handler( ) : p_Meta_View_Field( 0 ) { }

   void set_Meta_View_Field( Meta_View_Field* p_new_Meta_View_Field ) { p_Meta_View_Field = p_new_Meta_View_Field; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_View_Field* p_Meta_View_Field;

   protected:
   string retval;
};

class Meta_View_Field_command_functor : public command_functor
{
   public:
   Meta_View_Field_command_functor( Meta_View_Field_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_View_Field_command_handler& cmd_handler;
};

command_functor* Meta_View_Field_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_View_Field_command_functor( dynamic_cast< Meta_View_Field_command_handler& >( handler ) );
}

void Meta_View_Field_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_View_Field_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_View_Field_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_View_Field->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_View_Field->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_View_Field_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_View_Field->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_View_Field->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_View_Field_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_View_Field_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_View_Field->Access_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Restriction ) || ( field_name == c_field_name_Access_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->Access_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Scope ) || ( field_name == c_field_name_Access_Scope ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->Access_Scope( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Alignment ) || ( field_name == c_field_name_Alignment ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->Alignment( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Anonymous_Access ) || ( field_name == c_field_name_Allow_Anonymous_Access ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_View_Field->Allow_Anonymous_Access( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Permission ) || ( field_name == c_field_name_Change_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_View_Field->Change_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Restriction ) || ( field_name == c_field_name_Change_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->Change_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Scope ) || ( field_name == c_field_name_Change_Scope ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->Change_Scope( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_List_Extra_Option ) || ( field_name == c_field_name_Child_List_Extra_Option ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->Child_List_Extra_Option( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_View_Field->Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Date_Precision_Option ) || ( field_name == c_field_name_Date_Precision_Option ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->Date_Precision_Option( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Enum_Finishes_At ) || ( field_name == c_field_name_Enum_Finishes_At ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->Enum_Finishes_At( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Enum_Starts_At ) || ( field_name == c_field_name_Enum_Starts_At ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->Enum_Starts_At( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_FK_Trigger_Behaviour ) || ( field_name == c_field_name_FK_Trigger_Behaviour ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->FK_Trigger_Behaviour( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_FK_Trigger_Option ) || ( field_name == c_field_name_FK_Trigger_Option ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->FK_Trigger_Option( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Font_Size ) || ( field_name == c_field_name_Font_Size ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->Font_Size( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Ignore_Manual_Links ) || ( field_name == c_field_name_Ignore_Manual_Links ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_View_Field->Ignore_Manual_Links( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Label_Source_Child ) || ( field_name == c_field_name_Label_Source_Child ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->Label_Source_Child( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Link_Permission ) || ( field_name == c_field_name_Link_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_View_Field->Link_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Link_Restriction ) || ( field_name == c_field_name_Link_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->Link_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Mandatory_Option ) || ( field_name == c_field_name_Mandatory_Option ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->Mandatory_Option( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_View_Field->Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_New_Source ) || ( field_name == c_field_name_New_Source ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->New_Source( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_New_Value ) || ( field_name == c_field_name_New_Value ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_View_Field->New_Value( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Order ) || ( field_name == c_field_name_Order ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_View_Field->Order( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Orientation ) || ( field_name == c_field_name_Orientation ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->Orientation( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Restriction_Spec ) || ( field_name == c_field_name_Restriction_Spec ) ) )
      {
         handled = true;

         string_getter< Meta_Specification >( cmd_handler.p_Meta_View_Field->Restriction_Spec( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Show_Hide_Start_Point ) || ( field_name == c_field_name_Show_Hide_Start_Point ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_View_Field->Show_Hide_Start_Point( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Sort_Manually ) || ( field_name == c_field_name_Sort_Manually ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_View_Field->Sort_Manually( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Child ) || ( field_name == c_field_name_Source_Child ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_View_Field->Source_Child( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Edit_Child ) || ( field_name == c_field_name_Source_Edit_Child ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_View_Field->Source_Edit_Child( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Field ) || ( field_name == c_field_name_Source_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_View_Field->Source_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Parent ) || ( field_name == c_field_name_Source_Parent ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_View_Field->Source_Parent( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Parent_Class ) || ( field_name == c_field_name_Source_Parent_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_View_Field->Source_Parent_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Tab_Name ) || ( field_name == c_field_name_Tab_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_View_Field->Tab_Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Trigger_Behaviour ) || ( field_name == c_field_name_Trigger_Behaviour ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->Trigger_Behaviour( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Trigger_For_State ) || ( field_name == c_field_name_Trigger_For_State ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->Trigger_For_State( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Trigger_Option ) || ( field_name == c_field_name_Trigger_Option ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View_Field->Trigger_Option( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         string_getter< Meta_View_Field_Type >( cmd_handler.p_Meta_View_Field->Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Full_Height ) || ( field_name == c_field_name_Use_Full_Height ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_View_Field->Use_Full_Height( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Full_Width ) || ( field_name == c_field_name_Use_Full_Width ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_View_Field->Use_Full_Width( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Source_Parent ) || ( field_name == c_field_name_Use_Source_Parent ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_View_Field->Use_Source_Parent( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_View ) || ( field_name == c_field_name_View ) ) )
      {
         handled = true;

         string_getter< Meta_View >( cmd_handler.p_Meta_View_Field->View( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_View_Field_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_View_Field_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_View_Field_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, Meta_Permission >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Access_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Restriction ) || ( field_name == c_field_name_Access_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Access_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Scope ) || ( field_name == c_field_name_Access_Scope ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Access_Scope, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Alignment ) || ( field_name == c_field_name_Alignment ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Alignment, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Anonymous_Access ) || ( field_name == c_field_name_Allow_Anonymous_Access ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, bool >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Allow_Anonymous_Access, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Permission ) || ( field_name == c_field_name_Change_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, Meta_Permission >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Change_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Restriction ) || ( field_name == c_field_name_Change_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Change_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Scope ) || ( field_name == c_field_name_Change_Scope ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Change_Scope, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_List_Extra_Option ) || ( field_name == c_field_name_Child_List_Extra_Option ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Child_List_Extra_Option, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, Meta_Class >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Date_Precision_Option ) || ( field_name == c_field_name_Date_Precision_Option ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Date_Precision_Option, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Enum_Finishes_At ) || ( field_name == c_field_name_Enum_Finishes_At ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Enum_Finishes_At, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Enum_Starts_At ) || ( field_name == c_field_name_Enum_Starts_At ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Enum_Starts_At, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_FK_Trigger_Behaviour ) || ( field_name == c_field_name_FK_Trigger_Behaviour ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::FK_Trigger_Behaviour, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_FK_Trigger_Option ) || ( field_name == c_field_name_FK_Trigger_Option ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::FK_Trigger_Option, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Font_Size ) || ( field_name == c_field_name_Font_Size ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Font_Size, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Ignore_Manual_Links ) || ( field_name == c_field_name_Ignore_Manual_Links ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, bool >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Ignore_Manual_Links, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Label_Source_Child ) || ( field_name == c_field_name_Label_Source_Child ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Label_Source_Child, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Link_Permission ) || ( field_name == c_field_name_Link_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, Meta_Permission >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Link_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Link_Restriction ) || ( field_name == c_field_name_Link_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Link_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Mandatory_Option ) || ( field_name == c_field_name_Mandatory_Option ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Mandatory_Option, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, string >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_New_Source ) || ( field_name == c_field_name_New_Source ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::New_Source, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_New_Value ) || ( field_name == c_field_name_New_Value ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, string >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::New_Value, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Order ) || ( field_name == c_field_name_Order ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, string >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Order, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Orientation ) || ( field_name == c_field_name_Orientation ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Orientation, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Restriction_Spec ) || ( field_name == c_field_name_Restriction_Spec ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, Meta_Specification >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Restriction_Spec, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Show_Hide_Start_Point ) || ( field_name == c_field_name_Show_Hide_Start_Point ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, bool >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Show_Hide_Start_Point, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Sort_Manually ) || ( field_name == c_field_name_Sort_Manually ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, bool >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Sort_Manually, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Child ) || ( field_name == c_field_name_Source_Child ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, Meta_Field >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Source_Child, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Edit_Child ) || ( field_name == c_field_name_Source_Edit_Child ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, Meta_Field >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Source_Edit_Child, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Field ) || ( field_name == c_field_name_Source_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, Meta_Field >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Source_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Parent ) || ( field_name == c_field_name_Source_Parent ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, Meta_Field >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Source_Parent, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Parent_Class ) || ( field_name == c_field_name_Source_Parent_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, Meta_Class >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Source_Parent_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Tab_Name ) || ( field_name == c_field_name_Tab_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, string >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Tab_Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Trigger_Behaviour ) || ( field_name == c_field_name_Trigger_Behaviour ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Trigger_Behaviour, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Trigger_For_State ) || ( field_name == c_field_name_Trigger_For_State ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Trigger_For_State, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Trigger_Option ) || ( field_name == c_field_name_Trigger_Option ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, int >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Trigger_Option, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, Meta_View_Field_Type >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Full_Height ) || ( field_name == c_field_name_Use_Full_Height ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, bool >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Use_Full_Height, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Full_Width ) || ( field_name == c_field_name_Use_Full_Width ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, bool >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Use_Full_Width, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Source_Parent ) || ( field_name == c_field_name_Use_Source_Parent ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, bool >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::Use_Source_Parent, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_View ) || ( field_name == c_field_name_View ) ) )
      {
         handled = true;

         func_string_setter< Meta_View_Field, Meta_View >(
          *cmd_handler.p_Meta_View_Field, &Meta_View_Field::View, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_View_Field_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_View_Field_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_View_Field_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_View_Field->Access_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Change_Permission ) || ( field_name == c_field_name_Change_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_View_Field->Change_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_View_Field->Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Link_Permission ) || ( field_name == c_field_name_Link_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_View_Field->Link_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Restriction_Spec ) || ( field_name == c_field_name_Restriction_Spec ) )
         cmd_handler.retval = cmd_handler.p_Meta_View_Field->Restriction_Spec( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Child ) || ( field_name == c_field_name_Source_Child ) )
         cmd_handler.retval = cmd_handler.p_Meta_View_Field->Source_Child( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Edit_Child ) || ( field_name == c_field_name_Source_Edit_Child ) )
         cmd_handler.retval = cmd_handler.p_Meta_View_Field->Source_Edit_Child( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Field ) || ( field_name == c_field_name_Source_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_View_Field->Source_Field( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Parent ) || ( field_name == c_field_name_Source_Parent ) )
         cmd_handler.retval = cmd_handler.p_Meta_View_Field->Source_Parent( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Parent_Class ) || ( field_name == c_field_name_Source_Parent_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_View_Field->Source_Parent_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) )
         cmd_handler.retval = cmd_handler.p_Meta_View_Field->Type( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_View ) || ( field_name == c_field_name_View ) )
         cmd_handler.retval = cmd_handler.p_Meta_View_Field->View( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
   else if( command == c_cmd_Meta_View_Field_Move_Down )
   {
      string Restrict_Fields( get_parm_val_from_string< string >( parameters, c_cmd_Meta_View_Field_Move_Down_Restrict_Fields ) );
      string Restrict_Values( get_parm_val_from_string< string >( parameters, c_cmd_Meta_View_Field_Move_Down_Restrict_Values ) );

      cmd_handler.p_Meta_View_Field->Move_Down( Restrict_Fields, Restrict_Values );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_View_Field_Move_Up )
   {
      string Restrict_Fields( get_parm_val_from_string< string >( parameters, c_cmd_Meta_View_Field_Move_Up_Restrict_Fields ) );
      string Restrict_Values( get_parm_val_from_string< string >( parameters, c_cmd_Meta_View_Field_Move_Up_Restrict_Values ) );

      cmd_handler.p_Meta_View_Field->Move_Up( Restrict_Fields, Restrict_Values );

      cmd_handler.retval.erase( );

   }
}

struct Meta_View_Field::impl : public Meta_View_Field_command_handler
{
   impl( Meta_View_Field& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_View_Field( &o );

      add_commands( 0, Meta_View_Field_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_View_Field_command_definitions ) );
   }

   Meta_View_Field& get_obj( ) const
   {
      return *cp_obj;
   }

   int impl_Access_Restriction( ) const { return lazy_fetch( p_obj ), v_Access_Restriction; }
   void impl_Access_Restriction( int Access_Restriction ) { v_Access_Restriction = Access_Restriction; }

   int impl_Access_Scope( ) const { return lazy_fetch( p_obj ), v_Access_Scope; }
   void impl_Access_Scope( int Access_Scope ) { v_Access_Scope = Access_Scope; }

   int impl_Alignment( ) const { return lazy_fetch( p_obj ), v_Alignment; }
   void impl_Alignment( int Alignment ) { v_Alignment = Alignment; }

   bool impl_Allow_Anonymous_Access( ) const { return lazy_fetch( p_obj ), v_Allow_Anonymous_Access; }
   void impl_Allow_Anonymous_Access( bool Allow_Anonymous_Access ) { v_Allow_Anonymous_Access = Allow_Anonymous_Access; }

   int impl_Change_Restriction( ) const { return lazy_fetch( p_obj ), v_Change_Restriction; }
   void impl_Change_Restriction( int Change_Restriction ) { v_Change_Restriction = Change_Restriction; }

   int impl_Change_Scope( ) const { return lazy_fetch( p_obj ), v_Change_Scope; }
   void impl_Change_Scope( int Change_Scope ) { v_Change_Scope = Change_Scope; }

   int impl_Child_List_Extra_Option( ) const { return lazy_fetch( p_obj ), v_Child_List_Extra_Option; }
   void impl_Child_List_Extra_Option( int Child_List_Extra_Option ) { v_Child_List_Extra_Option = Child_List_Extra_Option; }

   int impl_Date_Precision_Option( ) const { return lazy_fetch( p_obj ), v_Date_Precision_Option; }
   void impl_Date_Precision_Option( int Date_Precision_Option ) { v_Date_Precision_Option = Date_Precision_Option; }

   int impl_Enum_Finishes_At( ) const { return lazy_fetch( p_obj ), v_Enum_Finishes_At; }
   void impl_Enum_Finishes_At( int Enum_Finishes_At ) { v_Enum_Finishes_At = Enum_Finishes_At; }

   int impl_Enum_Starts_At( ) const { return lazy_fetch( p_obj ), v_Enum_Starts_At; }
   void impl_Enum_Starts_At( int Enum_Starts_At ) { v_Enum_Starts_At = Enum_Starts_At; }

   int impl_FK_Trigger_Behaviour( ) const { return lazy_fetch( p_obj ), v_FK_Trigger_Behaviour; }
   void impl_FK_Trigger_Behaviour( int FK_Trigger_Behaviour ) { v_FK_Trigger_Behaviour = FK_Trigger_Behaviour; }

   int impl_FK_Trigger_Option( ) const { return lazy_fetch( p_obj ), v_FK_Trigger_Option; }
   void impl_FK_Trigger_Option( int FK_Trigger_Option ) { v_FK_Trigger_Option = FK_Trigger_Option; }

   int impl_Font_Size( ) const { return lazy_fetch( p_obj ), v_Font_Size; }
   void impl_Font_Size( int Font_Size ) { v_Font_Size = Font_Size; }

   bool impl_Ignore_Manual_Links( ) const { return lazy_fetch( p_obj ), v_Ignore_Manual_Links; }
   void impl_Ignore_Manual_Links( bool Ignore_Manual_Links ) { v_Ignore_Manual_Links = Ignore_Manual_Links; }

   int impl_Label_Source_Child( ) const { return lazy_fetch( p_obj ), v_Label_Source_Child; }
   void impl_Label_Source_Child( int Label_Source_Child ) { v_Label_Source_Child = Label_Source_Child; }

   int impl_Link_Restriction( ) const { return lazy_fetch( p_obj ), v_Link_Restriction; }
   void impl_Link_Restriction( int Link_Restriction ) { v_Link_Restriction = Link_Restriction; }

   int impl_Mandatory_Option( ) const { return lazy_fetch( p_obj ), v_Mandatory_Option; }
   void impl_Mandatory_Option( int Mandatory_Option ) { v_Mandatory_Option = Mandatory_Option; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { sanity_check( Name ); v_Name = Name; }

   int impl_New_Source( ) const { return lazy_fetch( p_obj ), v_New_Source; }
   void impl_New_Source( int New_Source ) { v_New_Source = New_Source; }

   const string& impl_New_Value( ) const { return lazy_fetch( p_obj ), v_New_Value; }
   void impl_New_Value( const string& New_Value ) { sanity_check( New_Value ); v_New_Value = New_Value; }

   const string& impl_Order( ) const { return lazy_fetch( p_obj ), v_Order; }
   void impl_Order( const string& Order ) { sanity_check( Order ); v_Order = Order; }

   int impl_Orientation( ) const { return lazy_fetch( p_obj ), v_Orientation; }
   void impl_Orientation( int Orientation ) { v_Orientation = Orientation; }

   bool impl_Show_Hide_Start_Point( ) const { return lazy_fetch( p_obj ), v_Show_Hide_Start_Point; }
   void impl_Show_Hide_Start_Point( bool Show_Hide_Start_Point ) { v_Show_Hide_Start_Point = Show_Hide_Start_Point; }

   bool impl_Sort_Manually( ) const { return lazy_fetch( p_obj ), v_Sort_Manually; }
   void impl_Sort_Manually( bool Sort_Manually ) { v_Sort_Manually = Sort_Manually; }

   const string& impl_Tab_Name( ) const { return lazy_fetch( p_obj ), v_Tab_Name; }
   void impl_Tab_Name( const string& Tab_Name ) { sanity_check( Tab_Name ); v_Tab_Name = Tab_Name; }

   int impl_Trigger_Behaviour( ) const { return lazy_fetch( p_obj ), v_Trigger_Behaviour; }
   void impl_Trigger_Behaviour( int Trigger_Behaviour ) { v_Trigger_Behaviour = Trigger_Behaviour; }

   int impl_Trigger_For_State( ) const { return lazy_fetch( p_obj ), v_Trigger_For_State; }
   void impl_Trigger_For_State( int Trigger_For_State ) { v_Trigger_For_State = Trigger_For_State; }

   int impl_Trigger_Option( ) const { return lazy_fetch( p_obj ), v_Trigger_Option; }
   void impl_Trigger_Option( int Trigger_Option ) { v_Trigger_Option = Trigger_Option; }

   bool impl_Use_Full_Height( ) const { return lazy_fetch( p_obj ), v_Use_Full_Height; }
   void impl_Use_Full_Height( bool Use_Full_Height ) { v_Use_Full_Height = Use_Full_Height; }

   bool impl_Use_Full_Width( ) const { return lazy_fetch( p_obj ), v_Use_Full_Width; }
   void impl_Use_Full_Width( bool Use_Full_Width ) { v_Use_Full_Width = Use_Full_Width; }

   bool impl_Use_Source_Parent( ) const { return lazy_fetch( p_obj ), v_Use_Source_Parent; }
   void impl_Use_Source_Parent( bool Use_Source_Parent ) { v_Use_Source_Parent = Use_Source_Parent; }

   Meta_Permission& impl_Access_Permission( )
   {
      if( !cp_Access_Permission )
      {
         cp_Access_Permission.init( );

         p_obj->setup_graph_parent( *cp_Access_Permission, c_field_id_Access_Permission, v_Access_Permission );
      }
      return *cp_Access_Permission;
   }

   const Meta_Permission& impl_Access_Permission( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Access_Permission )
      {
         cp_Access_Permission.init( );

         p_obj->setup_graph_parent( *cp_Access_Permission, c_field_id_Access_Permission, v_Access_Permission );
      }
      return *cp_Access_Permission;
   }

   void impl_Access_Permission( const string& key )
   {
      class_base_accessor cba( impl_Access_Permission( ) );
      cba.set_key( key );
   }

   Meta_Permission& impl_Change_Permission( )
   {
      if( !cp_Change_Permission )
      {
         cp_Change_Permission.init( );

         p_obj->setup_graph_parent( *cp_Change_Permission, c_field_id_Change_Permission, v_Change_Permission );
      }
      return *cp_Change_Permission;
   }

   const Meta_Permission& impl_Change_Permission( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Change_Permission )
      {
         cp_Change_Permission.init( );

         p_obj->setup_graph_parent( *cp_Change_Permission, c_field_id_Change_Permission, v_Change_Permission );
      }
      return *cp_Change_Permission;
   }

   void impl_Change_Permission( const string& key )
   {
      class_base_accessor cba( impl_Change_Permission( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Class( )
   {
      if( !cp_Class )
      {
         cp_Class.init( );

         p_obj->setup_graph_parent( *cp_Class, c_field_id_Class, v_Class );
      }
      return *cp_Class;
   }

   const Meta_Class& impl_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Class )
      {
         cp_Class.init( );

         p_obj->setup_graph_parent( *cp_Class, c_field_id_Class, v_Class );
      }
      return *cp_Class;
   }

   void impl_Class( const string& key )
   {
      class_base_accessor cba( impl_Class( ) );
      cba.set_key( key );
   }

   Meta_Permission& impl_Link_Permission( )
   {
      if( !cp_Link_Permission )
      {
         cp_Link_Permission.init( );

         p_obj->setup_graph_parent( *cp_Link_Permission, c_field_id_Link_Permission, v_Link_Permission );
      }
      return *cp_Link_Permission;
   }

   const Meta_Permission& impl_Link_Permission( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Link_Permission )
      {
         cp_Link_Permission.init( );

         p_obj->setup_graph_parent( *cp_Link_Permission, c_field_id_Link_Permission, v_Link_Permission );
      }
      return *cp_Link_Permission;
   }

   void impl_Link_Permission( const string& key )
   {
      class_base_accessor cba( impl_Link_Permission( ) );
      cba.set_key( key );
   }

   Meta_Specification& impl_Restriction_Spec( )
   {
      if( !cp_Restriction_Spec )
      {
         cp_Restriction_Spec.init( );

         p_obj->setup_graph_parent( *cp_Restriction_Spec, c_field_id_Restriction_Spec, v_Restriction_Spec );
      }
      return *cp_Restriction_Spec;
   }

   const Meta_Specification& impl_Restriction_Spec( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Restriction_Spec )
      {
         cp_Restriction_Spec.init( );

         p_obj->setup_graph_parent( *cp_Restriction_Spec, c_field_id_Restriction_Spec, v_Restriction_Spec );
      }
      return *cp_Restriction_Spec;
   }

   void impl_Restriction_Spec( const string& key )
   {
      class_base_accessor cba( impl_Restriction_Spec( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Source_Child( )
   {
      if( !cp_Source_Child )
      {
         cp_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_Source_Child, c_field_id_Source_Child, v_Source_Child );
      }
      return *cp_Source_Child;
   }

   const Meta_Field& impl_Source_Child( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Child )
      {
         cp_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_Source_Child, c_field_id_Source_Child, v_Source_Child );
      }
      return *cp_Source_Child;
   }

   void impl_Source_Child( const string& key )
   {
      class_base_accessor cba( impl_Source_Child( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Source_Edit_Child( )
   {
      if( !cp_Source_Edit_Child )
      {
         cp_Source_Edit_Child.init( );

         p_obj->setup_graph_parent( *cp_Source_Edit_Child, c_field_id_Source_Edit_Child, v_Source_Edit_Child );
      }
      return *cp_Source_Edit_Child;
   }

   const Meta_Field& impl_Source_Edit_Child( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Edit_Child )
      {
         cp_Source_Edit_Child.init( );

         p_obj->setup_graph_parent( *cp_Source_Edit_Child, c_field_id_Source_Edit_Child, v_Source_Edit_Child );
      }
      return *cp_Source_Edit_Child;
   }

   void impl_Source_Edit_Child( const string& key )
   {
      class_base_accessor cba( impl_Source_Edit_Child( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Source_Field( )
   {
      if( !cp_Source_Field )
      {
         cp_Source_Field.init( );

         p_obj->setup_graph_parent( *cp_Source_Field, c_field_id_Source_Field, v_Source_Field );
      }
      return *cp_Source_Field;
   }

   const Meta_Field& impl_Source_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Field )
      {
         cp_Source_Field.init( );

         p_obj->setup_graph_parent( *cp_Source_Field, c_field_id_Source_Field, v_Source_Field );
      }
      return *cp_Source_Field;
   }

   void impl_Source_Field( const string& key )
   {
      class_base_accessor cba( impl_Source_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Source_Parent( )
   {
      if( !cp_Source_Parent )
      {
         cp_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_Source_Parent, c_field_id_Source_Parent, v_Source_Parent );
      }
      return *cp_Source_Parent;
   }

   const Meta_Field& impl_Source_Parent( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Parent )
      {
         cp_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_Source_Parent, c_field_id_Source_Parent, v_Source_Parent );
      }
      return *cp_Source_Parent;
   }

   void impl_Source_Parent( const string& key )
   {
      class_base_accessor cba( impl_Source_Parent( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Source_Parent_Class( )
   {
      if( !cp_Source_Parent_Class )
      {
         cp_Source_Parent_Class.init( );

         p_obj->setup_graph_parent( *cp_Source_Parent_Class, c_field_id_Source_Parent_Class, v_Source_Parent_Class );
      }
      return *cp_Source_Parent_Class;
   }

   const Meta_Class& impl_Source_Parent_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Parent_Class )
      {
         cp_Source_Parent_Class.init( );

         p_obj->setup_graph_parent( *cp_Source_Parent_Class, c_field_id_Source_Parent_Class, v_Source_Parent_Class );
      }
      return *cp_Source_Parent_Class;
   }

   void impl_Source_Parent_Class( const string& key )
   {
      class_base_accessor cba( impl_Source_Parent_Class( ) );
      cba.set_key( key );
   }

   Meta_View_Field_Type& impl_Type( )
   {
      if( !cp_Type )
      {
         cp_Type.init( );

         p_obj->setup_graph_parent( *cp_Type, c_field_id_Type, v_Type );
      }
      return *cp_Type;
   }

   const Meta_View_Field_Type& impl_Type( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Type )
      {
         cp_Type.init( );

         p_obj->setup_graph_parent( *cp_Type, c_field_id_Type, v_Type );
      }
      return *cp_Type;
   }

   void impl_Type( const string& key )
   {
      class_base_accessor cba( impl_Type( ) );
      cba.set_key( key );
   }

   Meta_View& impl_View( )
   {
      if( !cp_View )
      {
         cp_View.init( );

         p_obj->setup_graph_parent( *cp_View, c_field_id_View, v_View );
      }
      return *cp_View;
   }

   const Meta_View& impl_View( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_View )
      {
         cp_View.init( );

         p_obj->setup_graph_parent( *cp_View, c_field_id_View, v_View );
      }
      return *cp_View;
   }

   void impl_View( const string& key )
   {
      class_base_accessor cba( impl_View( ) );
      cba.set_key( key );
   }

   void impl_Move_Down( const string& Restrict_Fields, const string& Restrict_Values );

   void impl_Move_Up( const string& Restrict_Fields, const string& Restrict_Values );

   string get_field_value( int field ) const;
   void set_field_value( int field, const string& value );
   void set_field_default( int field );

   bool is_field_default( int field ) const;

   uint64_t get_state( ) const;

   string get_state_names( ) const;

   const string& execute( const string& cmd_and_args );

   void clear_foreign_key( const string& field );

   void set_foreign_key_value( const string& field, const string& value );

   const string& get_foreign_key_value( const string& field );

   void get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const;

   void add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const;
   void add_extra_paging_info( vector< pair< string, string > >& paging_info ) const;

   void clear( );

   bool value_will_be_provided( const string& field_name );

   void validate( uint64_t state, bool is_internal, validation_error_container* p_validation_errors );
   void validate_set_fields( set< string >& fields_set, validation_error_container* p_validation_errors );

   void after_fetch( );
   void finalise_fetch( bool skip_set_original );

   void at_create( );
   void post_init( );

   void to_store( bool is_create, bool is_internal );
   void for_store( bool is_create, bool is_internal );
   void after_store( bool is_create, bool is_internal );

   bool can_destroy( bool is_internal );
   void for_destroy( bool is_internal );
   void after_destroy( bool is_internal );

   void set_default_values( );

   bool is_filtered( ) const;

   void get_required_transients( ) const;

   Meta_View_Field* p_obj;
   class_pointer< Meta_View_Field > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   int v_Access_Restriction;
   int v_Access_Scope;
   int v_Alignment;
   bool v_Allow_Anonymous_Access;
   int v_Change_Restriction;
   int v_Change_Scope;
   int v_Child_List_Extra_Option;
   int v_Date_Precision_Option;
   int v_Enum_Finishes_At;
   int v_Enum_Starts_At;
   int v_FK_Trigger_Behaviour;
   int v_FK_Trigger_Option;
   int v_Font_Size;
   bool v_Ignore_Manual_Links;
   int v_Label_Source_Child;
   int v_Link_Restriction;
   int v_Mandatory_Option;
   string v_Name;
   int v_New_Source;
   string v_New_Value;
   string v_Order;
   int v_Orientation;
   bool v_Show_Hide_Start_Point;
   bool v_Sort_Manually;
   string v_Tab_Name;
   int v_Trigger_Behaviour;
   int v_Trigger_For_State;
   int v_Trigger_Option;
   bool v_Use_Full_Height;
   bool v_Use_Full_Width;
   bool v_Use_Source_Parent;

   string v_Access_Permission;
   mutable class_pointer< Meta_Permission > cp_Access_Permission;

   string v_Change_Permission;
   mutable class_pointer< Meta_Permission > cp_Change_Permission;

   string v_Class;
   mutable class_pointer< Meta_Class > cp_Class;

   string v_Link_Permission;
   mutable class_pointer< Meta_Permission > cp_Link_Permission;

   string v_Restriction_Spec;
   mutable class_pointer< Meta_Specification > cp_Restriction_Spec;

   string v_Source_Child;
   mutable class_pointer< Meta_Field > cp_Source_Child;

   string v_Source_Edit_Child;
   mutable class_pointer< Meta_Field > cp_Source_Edit_Child;

   string v_Source_Field;
   mutable class_pointer< Meta_Field > cp_Source_Field;

   string v_Source_Parent;
   mutable class_pointer< Meta_Field > cp_Source_Parent;

   string v_Source_Parent_Class;
   mutable class_pointer< Meta_Class > cp_Source_Parent_Class;

   string v_Type;
   mutable class_pointer< Meta_View_Field_Type > cp_Type;

   string v_View;
   mutable class_pointer< Meta_View > cp_View;
};

void Meta_View_Field::impl::impl_Move_Down( const string& Restrict_Fields, const string& Restrict_Values )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start move_up_and_down)] 600370
   transaction_start( );
   try
   {
      // NOTE: Empty code block for scope purposes.
      {
         get_obj( ).op_update( get_obj( ).get_key( ), c_field_name_Order );

         class_pointer< Meta_View_Field > cp_other( e_create_instance );

         int num_fixed = 0;
         if( !Restrict_Fields.empty( ) )
            num_fixed = split_count( Restrict_Fields );

         string key_info( Restrict_Fields );
         if( num_fixed )
            key_info += ',';
         key_info += string( c_field_id_Order );

         if( !num_fixed )
            key_info += ' ' + get_obj( ).Order( );
         else
         {
            key_info += '#' + to_string( num_fixed );
            key_info += ' ' + Restrict_Values + ',' + get_obj( ).Order( );
         }

         // NOTE: Only the first record is required so set the row limit to 1.
         if( cp_other->iterate_forwards( key_info, string( c_field_name_Order ), false, 1 ) )
         {
            string old_val( get_obj( ).Order( ) );
            string new_val( cp_other->Order( ) );

            get_obj( ).Order( gen_key( ) );
            get_obj( ).op_apply( );

            get_obj( ).op_update( get_obj( ).get_key( ), c_field_name_Order );

            cp_other->op_update( cp_other->get_key( ), c_field_name_Order );
            cp_other->Order( old_val );
            cp_other->op_apply( );

            get_obj( ).Order( new_val );
            get_obj( ).op_apply( );

            cp_other->iterate_stop( );
         }
         else
            get_obj( ).op_cancel( );
      }

      transaction_commit( );
   }
   catch( ... )
   {
      transaction_rollback( );
      throw;
   }
   // [(finish move_up_and_down)] 600370

   // [<start Move_Down_impl>]
   // [<finish Move_Down_impl>]
}

void Meta_View_Field::impl::impl_Move_Up( const string& Restrict_Fields, const string& Restrict_Values )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start move_up_and_down)] 600370
   transaction_start( );
   try
   {
      // NOTE: Empty code block for scope purposes.
      {
         get_obj( ).op_update( get_obj( ).get_key( ), c_field_name_Order );

         class_pointer< Meta_View_Field > cp_other( e_create_instance );

         int num_fixed = 0;
         if( !Restrict_Fields.empty( ) )
            num_fixed = split_count( Restrict_Fields );

         string key_info( Restrict_Fields );
         if( num_fixed )
            key_info += ',';
         key_info += string( c_field_id_Order );

         if( !num_fixed )
            key_info += ' ' + get_obj( ).Order( );
         else
         {
            key_info += '#' + to_string( num_fixed );
            key_info += ' ' + Restrict_Values + ',' + get_obj( ).Order( );
         }

         // NOTE: Only the first record is required so set the row limit to 1.
         if( cp_other->iterate_backwards( key_info, string( c_field_name_Order ), false, 1 ) )
         {
            string old_val( get_obj( ).Order( ) );
            string new_val( cp_other->Order( ) );

            get_obj( ).Order( gen_key( ) );
            get_obj( ).op_apply( );

            get_obj( ).op_update( get_obj( ).get_key( ), c_field_name_Order );

            cp_other->op_update( cp_other->get_key( ), c_field_name_Order );
            cp_other->Order( old_val );
            cp_other->op_apply( );

            get_obj( ).Order( new_val );
            get_obj( ).op_apply( );

            cp_other->iterate_stop( );
         }
         else
            get_obj( ).op_cancel( );
      }

      transaction_commit( );
   }
   catch( ... )
   {
      transaction_rollback( );
      throw;
   }
   // [(finish move_up_and_down)] 600370

   // [<start Move_Up_impl>]
   // [<finish Move_Up_impl>]
}

string Meta_View_Field::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Access_Permission( ) );
      break;

      case 1:
      retval = to_string( impl_Access_Restriction( ) );
      break;

      case 2:
      retval = to_string( impl_Access_Scope( ) );
      break;

      case 3:
      retval = to_string( impl_Alignment( ) );
      break;

      case 4:
      retval = to_string( impl_Allow_Anonymous_Access( ) );
      break;

      case 5:
      retval = to_string( impl_Change_Permission( ) );
      break;

      case 6:
      retval = to_string( impl_Change_Restriction( ) );
      break;

      case 7:
      retval = to_string( impl_Change_Scope( ) );
      break;

      case 8:
      retval = to_string( impl_Child_List_Extra_Option( ) );
      break;

      case 9:
      retval = to_string( impl_Class( ) );
      break;

      case 10:
      retval = to_string( impl_Date_Precision_Option( ) );
      break;

      case 11:
      retval = to_string( impl_Enum_Finishes_At( ) );
      break;

      case 12:
      retval = to_string( impl_Enum_Starts_At( ) );
      break;

      case 13:
      retval = to_string( impl_FK_Trigger_Behaviour( ) );
      break;

      case 14:
      retval = to_string( impl_FK_Trigger_Option( ) );
      break;

      case 15:
      retval = to_string( impl_Font_Size( ) );
      break;

      case 16:
      retval = to_string( impl_Ignore_Manual_Links( ) );
      break;

      case 17:
      retval = to_string( impl_Label_Source_Child( ) );
      break;

      case 18:
      retval = to_string( impl_Link_Permission( ) );
      break;

      case 19:
      retval = to_string( impl_Link_Restriction( ) );
      break;

      case 20:
      retval = to_string( impl_Mandatory_Option( ) );
      break;

      case 21:
      retval = to_string( impl_Name( ) );
      break;

      case 22:
      retval = to_string( impl_New_Source( ) );
      break;

      case 23:
      retval = to_string( impl_New_Value( ) );
      break;

      case 24:
      retval = to_string( impl_Order( ) );
      break;

      case 25:
      retval = to_string( impl_Orientation( ) );
      break;

      case 26:
      retval = to_string( impl_Restriction_Spec( ) );
      break;

      case 27:
      retval = to_string( impl_Show_Hide_Start_Point( ) );
      break;

      case 28:
      retval = to_string( impl_Sort_Manually( ) );
      break;

      case 29:
      retval = to_string( impl_Source_Child( ) );
      break;

      case 30:
      retval = to_string( impl_Source_Edit_Child( ) );
      break;

      case 31:
      retval = to_string( impl_Source_Field( ) );
      break;

      case 32:
      retval = to_string( impl_Source_Parent( ) );
      break;

      case 33:
      retval = to_string( impl_Source_Parent_Class( ) );
      break;

      case 34:
      retval = to_string( impl_Tab_Name( ) );
      break;

      case 35:
      retval = to_string( impl_Trigger_Behaviour( ) );
      break;

      case 36:
      retval = to_string( impl_Trigger_For_State( ) );
      break;

      case 37:
      retval = to_string( impl_Trigger_Option( ) );
      break;

      case 38:
      retval = to_string( impl_Type( ) );
      break;

      case 39:
      retval = to_string( impl_Use_Full_Height( ) );
      break;

      case 40:
      retval = to_string( impl_Use_Full_Width( ) );
      break;

      case 41:
      retval = to_string( impl_Use_Source_Parent( ) );
      break;

      case 42:
      retval = to_string( impl_View( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_View_Field::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_View_Field::impl, Meta_Permission >( *this, &Meta_View_Field::impl::impl_Access_Permission, value );
      break;

      case 1:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_Access_Restriction, value );
      break;

      case 2:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_Access_Scope, value );
      break;

      case 3:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_Alignment, value );
      break;

      case 4:
      func_string_setter< Meta_View_Field::impl, bool >( *this, &Meta_View_Field::impl::impl_Allow_Anonymous_Access, value );
      break;

      case 5:
      func_string_setter< Meta_View_Field::impl, Meta_Permission >( *this, &Meta_View_Field::impl::impl_Change_Permission, value );
      break;

      case 6:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_Change_Restriction, value );
      break;

      case 7:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_Change_Scope, value );
      break;

      case 8:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_Child_List_Extra_Option, value );
      break;

      case 9:
      func_string_setter< Meta_View_Field::impl, Meta_Class >( *this, &Meta_View_Field::impl::impl_Class, value );
      break;

      case 10:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_Date_Precision_Option, value );
      break;

      case 11:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_Enum_Finishes_At, value );
      break;

      case 12:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_Enum_Starts_At, value );
      break;

      case 13:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_FK_Trigger_Behaviour, value );
      break;

      case 14:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_FK_Trigger_Option, value );
      break;

      case 15:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_Font_Size, value );
      break;

      case 16:
      func_string_setter< Meta_View_Field::impl, bool >( *this, &Meta_View_Field::impl::impl_Ignore_Manual_Links, value );
      break;

      case 17:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_Label_Source_Child, value );
      break;

      case 18:
      func_string_setter< Meta_View_Field::impl, Meta_Permission >( *this, &Meta_View_Field::impl::impl_Link_Permission, value );
      break;

      case 19:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_Link_Restriction, value );
      break;

      case 20:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_Mandatory_Option, value );
      break;

      case 21:
      func_string_setter< Meta_View_Field::impl, string >( *this, &Meta_View_Field::impl::impl_Name, value );
      break;

      case 22:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_New_Source, value );
      break;

      case 23:
      func_string_setter< Meta_View_Field::impl, string >( *this, &Meta_View_Field::impl::impl_New_Value, value );
      break;

      case 24:
      func_string_setter< Meta_View_Field::impl, string >( *this, &Meta_View_Field::impl::impl_Order, value );
      break;

      case 25:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_Orientation, value );
      break;

      case 26:
      func_string_setter< Meta_View_Field::impl, Meta_Specification >( *this, &Meta_View_Field::impl::impl_Restriction_Spec, value );
      break;

      case 27:
      func_string_setter< Meta_View_Field::impl, bool >( *this, &Meta_View_Field::impl::impl_Show_Hide_Start_Point, value );
      break;

      case 28:
      func_string_setter< Meta_View_Field::impl, bool >( *this, &Meta_View_Field::impl::impl_Sort_Manually, value );
      break;

      case 29:
      func_string_setter< Meta_View_Field::impl, Meta_Field >( *this, &Meta_View_Field::impl::impl_Source_Child, value );
      break;

      case 30:
      func_string_setter< Meta_View_Field::impl, Meta_Field >( *this, &Meta_View_Field::impl::impl_Source_Edit_Child, value );
      break;

      case 31:
      func_string_setter< Meta_View_Field::impl, Meta_Field >( *this, &Meta_View_Field::impl::impl_Source_Field, value );
      break;

      case 32:
      func_string_setter< Meta_View_Field::impl, Meta_Field >( *this, &Meta_View_Field::impl::impl_Source_Parent, value );
      break;

      case 33:
      func_string_setter< Meta_View_Field::impl, Meta_Class >( *this, &Meta_View_Field::impl::impl_Source_Parent_Class, value );
      break;

      case 34:
      func_string_setter< Meta_View_Field::impl, string >( *this, &Meta_View_Field::impl::impl_Tab_Name, value );
      break;

      case 35:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_Trigger_Behaviour, value );
      break;

      case 36:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_Trigger_For_State, value );
      break;

      case 37:
      func_string_setter< Meta_View_Field::impl, int >( *this, &Meta_View_Field::impl::impl_Trigger_Option, value );
      break;

      case 38:
      func_string_setter< Meta_View_Field::impl, Meta_View_Field_Type >( *this, &Meta_View_Field::impl::impl_Type, value );
      break;

      case 39:
      func_string_setter< Meta_View_Field::impl, bool >( *this, &Meta_View_Field::impl::impl_Use_Full_Height, value );
      break;

      case 40:
      func_string_setter< Meta_View_Field::impl, bool >( *this, &Meta_View_Field::impl::impl_Use_Full_Width, value );
      break;

      case 41:
      func_string_setter< Meta_View_Field::impl, bool >( *this, &Meta_View_Field::impl::impl_Use_Source_Parent, value );
      break;

      case 42:
      func_string_setter< Meta_View_Field::impl, Meta_View >( *this, &Meta_View_Field::impl::impl_View, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_View_Field::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Access_Permission( g_default_Access_Permission );
      break;

      case 1:
      impl_Access_Restriction( g_default_Access_Restriction );
      break;

      case 2:
      impl_Access_Scope( g_default_Access_Scope );
      break;

      case 3:
      impl_Alignment( g_default_Alignment );
      break;

      case 4:
      impl_Allow_Anonymous_Access( g_default_Allow_Anonymous_Access );
      break;

      case 5:
      impl_Change_Permission( g_default_Change_Permission );
      break;

      case 6:
      impl_Change_Restriction( g_default_Change_Restriction );
      break;

      case 7:
      impl_Change_Scope( g_default_Change_Scope );
      break;

      case 8:
      impl_Child_List_Extra_Option( g_default_Child_List_Extra_Option );
      break;

      case 9:
      impl_Class( g_default_Class );
      break;

      case 10:
      impl_Date_Precision_Option( g_default_Date_Precision_Option );
      break;

      case 11:
      impl_Enum_Finishes_At( g_default_Enum_Finishes_At );
      break;

      case 12:
      impl_Enum_Starts_At( g_default_Enum_Starts_At );
      break;

      case 13:
      impl_FK_Trigger_Behaviour( g_default_FK_Trigger_Behaviour );
      break;

      case 14:
      impl_FK_Trigger_Option( g_default_FK_Trigger_Option );
      break;

      case 15:
      impl_Font_Size( g_default_Font_Size );
      break;

      case 16:
      impl_Ignore_Manual_Links( g_default_Ignore_Manual_Links );
      break;

      case 17:
      impl_Label_Source_Child( g_default_Label_Source_Child );
      break;

      case 18:
      impl_Link_Permission( g_default_Link_Permission );
      break;

      case 19:
      impl_Link_Restriction( g_default_Link_Restriction );
      break;

      case 20:
      impl_Mandatory_Option( g_default_Mandatory_Option );
      break;

      case 21:
      impl_Name( g_default_Name );
      break;

      case 22:
      impl_New_Source( g_default_New_Source );
      break;

      case 23:
      impl_New_Value( g_default_New_Value );
      break;

      case 24:
      impl_Order( g_default_Order );
      break;

      case 25:
      impl_Orientation( g_default_Orientation );
      break;

      case 26:
      impl_Restriction_Spec( g_default_Restriction_Spec );
      break;

      case 27:
      impl_Show_Hide_Start_Point( g_default_Show_Hide_Start_Point );
      break;

      case 28:
      impl_Sort_Manually( g_default_Sort_Manually );
      break;

      case 29:
      impl_Source_Child( g_default_Source_Child );
      break;

      case 30:
      impl_Source_Edit_Child( g_default_Source_Edit_Child );
      break;

      case 31:
      impl_Source_Field( g_default_Source_Field );
      break;

      case 32:
      impl_Source_Parent( g_default_Source_Parent );
      break;

      case 33:
      impl_Source_Parent_Class( g_default_Source_Parent_Class );
      break;

      case 34:
      impl_Tab_Name( g_default_Tab_Name );
      break;

      case 35:
      impl_Trigger_Behaviour( g_default_Trigger_Behaviour );
      break;

      case 36:
      impl_Trigger_For_State( g_default_Trigger_For_State );
      break;

      case 37:
      impl_Trigger_Option( g_default_Trigger_Option );
      break;

      case 38:
      impl_Type( g_default_Type );
      break;

      case 39:
      impl_Use_Full_Height( g_default_Use_Full_Height );
      break;

      case 40:
      impl_Use_Full_Width( g_default_Use_Full_Width );
      break;

      case 41:
      impl_Use_Source_Parent( g_default_Use_Source_Parent );
      break;

      case 42:
      impl_View( g_default_View );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_View_Field::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Access_Permission == g_default_Access_Permission );
      break;

      case 1:
      retval = ( v_Access_Restriction == g_default_Access_Restriction );
      break;

      case 2:
      retval = ( v_Access_Scope == g_default_Access_Scope );
      break;

      case 3:
      retval = ( v_Alignment == g_default_Alignment );
      break;

      case 4:
      retval = ( v_Allow_Anonymous_Access == g_default_Allow_Anonymous_Access );
      break;

      case 5:
      retval = ( v_Change_Permission == g_default_Change_Permission );
      break;

      case 6:
      retval = ( v_Change_Restriction == g_default_Change_Restriction );
      break;

      case 7:
      retval = ( v_Change_Scope == g_default_Change_Scope );
      break;

      case 8:
      retval = ( v_Child_List_Extra_Option == g_default_Child_List_Extra_Option );
      break;

      case 9:
      retval = ( v_Class == g_default_Class );
      break;

      case 10:
      retval = ( v_Date_Precision_Option == g_default_Date_Precision_Option );
      break;

      case 11:
      retval = ( v_Enum_Finishes_At == g_default_Enum_Finishes_At );
      break;

      case 12:
      retval = ( v_Enum_Starts_At == g_default_Enum_Starts_At );
      break;

      case 13:
      retval = ( v_FK_Trigger_Behaviour == g_default_FK_Trigger_Behaviour );
      break;

      case 14:
      retval = ( v_FK_Trigger_Option == g_default_FK_Trigger_Option );
      break;

      case 15:
      retval = ( v_Font_Size == g_default_Font_Size );
      break;

      case 16:
      retval = ( v_Ignore_Manual_Links == g_default_Ignore_Manual_Links );
      break;

      case 17:
      retval = ( v_Label_Source_Child == g_default_Label_Source_Child );
      break;

      case 18:
      retval = ( v_Link_Permission == g_default_Link_Permission );
      break;

      case 19:
      retval = ( v_Link_Restriction == g_default_Link_Restriction );
      break;

      case 20:
      retval = ( v_Mandatory_Option == g_default_Mandatory_Option );
      break;

      case 21:
      retval = ( v_Name == g_default_Name );
      break;

      case 22:
      retval = ( v_New_Source == g_default_New_Source );
      break;

      case 23:
      retval = ( v_New_Value == g_default_New_Value );
      break;

      case 24:
      retval = ( v_Order == g_default_Order );
      break;

      case 25:
      retval = ( v_Orientation == g_default_Orientation );
      break;

      case 26:
      retval = ( v_Restriction_Spec == g_default_Restriction_Spec );
      break;

      case 27:
      retval = ( v_Show_Hide_Start_Point == g_default_Show_Hide_Start_Point );
      break;

      case 28:
      retval = ( v_Sort_Manually == g_default_Sort_Manually );
      break;

      case 29:
      retval = ( v_Source_Child == g_default_Source_Child );
      break;

      case 30:
      retval = ( v_Source_Edit_Child == g_default_Source_Edit_Child );
      break;

      case 31:
      retval = ( v_Source_Field == g_default_Source_Field );
      break;

      case 32:
      retval = ( v_Source_Parent == g_default_Source_Parent );
      break;

      case 33:
      retval = ( v_Source_Parent_Class == g_default_Source_Parent_Class );
      break;

      case 34:
      retval = ( v_Tab_Name == g_default_Tab_Name );
      break;

      case 35:
      retval = ( v_Trigger_Behaviour == g_default_Trigger_Behaviour );
      break;

      case 36:
      retval = ( v_Trigger_For_State == g_default_Trigger_For_State );
      break;

      case 37:
      retval = ( v_Trigger_Option == g_default_Trigger_Option );
      break;

      case 38:
      retval = ( v_Type == g_default_Type );
      break;

      case 39:
      retval = ( v_Use_Full_Height == g_default_Use_Full_Height );
      break;

      case 40:
      retval = ( v_Use_Full_Width == g_default_Use_Full_Width );
      break;

      case 41:
      retval = ( v_Use_Source_Parent == g_default_Use_Source_Parent );
      break;

      case 42:
      retval = ( v_View == g_default_View );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_View_Field::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start modifier_field_value)] 600351
   if( get_obj( ).View( ).Type_Key( ) == "print" )
      state |= c_modifier_Is_Print_Version;
   // [(finish modifier_field_value)] 600351

   // [(start modifier_field_value)] 600390
   if( get_obj( ).Type( ).Name( ) == "key" )
      state |= c_modifier_Is_Key;
   // [(finish modifier_field_value)] 600390

   // [(start modifier_field_value)] 600391
   if( get_obj( ).Type( ).Name( ) == "tab" )
      state |= c_modifier_Is_Tab;
   // [(finish modifier_field_value)] 600391

   // [(start modifier_field_value)] 600392
   if( get_obj( ).Type( ).Name( ) == "field" )
      state |= c_modifier_Is_Field;
   // [(finish modifier_field_value)] 600392

   // [(start modifier_field_value)] 600393
   if( get_obj( ).Use_Source_Parent( ) == false )
      state |= c_modifier_Hide_FK_Fields;
   // [(finish modifier_field_value)] 600393

   // [(start modifier_field_value)] 600394
   if( get_obj( ).Use_Source_Parent( ) == true )
      state |= c_modifier_Hide_Non_FK_Fields;
   // [(finish modifier_field_value)] 600394

   // [(start modifier_field_value)] 600396
   if( get_obj( ).Use_Source_Parent( ) == 0 && get_obj( ).Source_Field( ).Primitive( ) != 2 ) // i.e. date
      state |= c_modifier_Is_Not_Date;
   // [(finish modifier_field_value)] 600396

   // [(start modifier_field_value)] 600397
   if( get_obj( ).Use_Source_Parent( ) == 1 && get_obj( ).Source_Child( ).Primitive( ) != 2 ) // i.e. date
      state |= c_modifier_Is_Not_Date;
   // [(finish modifier_field_value)] 600397

   // [(start modifier_field_null)] 600398
   if( is_null( get_obj( ).Source_Field( ).Enum( ) ) )
      state |= c_modifier_Is_Not_Enum;
   // [(finish modifier_field_null)] 600398

   // [(start modifier_field_value)] 610394
   if( get_obj( ).New_Source( ) != 4 ) // i.e. value
      state |= c_modifier_Hide_New_Value;
   // [(finish modifier_field_value)] 610394

   // [<start get_state>]
//nyi
   if( get_obj( ).Source_Field( ).Access_Scope( ) != 0
    || get_obj( ).Source_Parent( ).Access_Scope( ) != 0 )
      state |= c_modifier_Protect_Access_Scope;

   if( get_obj( ).Source_Field( ).Access_Restriction( ) != 0
    || get_obj( ).Source_Parent( ).Access_Restriction( ) != 0 )
      state |= c_modifier_Protect_Access_Restriction;

   if( get_obj( ).Source_Field( ).Change_Scope( ) != 0
    || get_obj( ).Source_Parent( ).Change_Scope( ) != 0 )
      state |= c_modifier_Protect_Change_Scope;

   if( get_obj( ).Source_Field( ).Change_Restriction( ) != 0
    || get_obj( ).Source_Parent( ).Change_Restriction( ) != 0 )
      state |= c_modifier_Protect_Change_Restriction;

   if( get_obj( ).Source_Field( ).Extra( ) != 3 ) // i.e. image
      state |= c_modifier_Is_Not_Image;

   if( get_obj( ).Source_Field( ).Extra( ) == 1 // i.e. file
    || get_obj( ).Source_Field( ).Extra( ) == 3 ) // i.e. image
      state |= c_modifier_Is_File_Or_Image;
   // [<finish get_state>]

   return state;
}

string Meta_View_Field::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   if( state & c_modifier_Hide_FK_Fields )
      state_names += "|" + string( "Hide_FK_Fields" );
   if( state & c_modifier_Hide_New_Value )
      state_names += "|" + string( "Hide_New_Value" );
   if( state & c_modifier_Hide_Non_FK_Fields )
      state_names += "|" + string( "Hide_Non_FK_Fields" );
   if( state & c_modifier_Is_Field )
      state_names += "|" + string( "Is_Field" );
   if( state & c_modifier_Is_File_Or_Image )
      state_names += "|" + string( "Is_File_Or_Image" );
   if( state & c_modifier_Is_Key )
      state_names += "|" + string( "Is_Key" );
   if( state & c_modifier_Is_Not_Date )
      state_names += "|" + string( "Is_Not_Date" );
   if( state & c_modifier_Is_Not_Enum )
      state_names += "|" + string( "Is_Not_Enum" );
   if( state & c_modifier_Is_Not_Image )
      state_names += "|" + string( "Is_Not_Image" );
   if( state & c_modifier_Is_Print_Version )
      state_names += "|" + string( "Is_Print_Version" );
   if( state & c_modifier_Is_Tab )
      state_names += "|" + string( "Is_Tab" );
   if( state & c_modifier_Protect_Access_Restriction )
      state_names += "|" + string( "Protect_Access_Restriction" );
   if( state & c_modifier_Protect_Access_Scope )
      state_names += "|" + string( "Protect_Access_Scope" );
   if( state & c_modifier_Protect_Change_Restriction )
      state_names += "|" + string( "Protect_Change_Restriction" );
   if( state & c_modifier_Protect_Change_Scope )
      state_names += "|" + string( "Protect_Change_Scope" );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_View_Field::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_View_Field::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      impl_Access_Permission( "" );
   else if( ( field == c_field_id_Change_Permission ) || ( field == c_field_name_Change_Permission ) )
      impl_Change_Permission( "" );
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      impl_Class( "" );
   else if( ( field == c_field_id_Link_Permission ) || ( field == c_field_name_Link_Permission ) )
      impl_Link_Permission( "" );
   else if( ( field == c_field_id_Restriction_Spec ) || ( field == c_field_name_Restriction_Spec ) )
      impl_Restriction_Spec( "" );
   else if( ( field == c_field_id_Source_Child ) || ( field == c_field_name_Source_Child ) )
      impl_Source_Child( "" );
   else if( ( field == c_field_id_Source_Edit_Child ) || ( field == c_field_name_Source_Edit_Child ) )
      impl_Source_Edit_Child( "" );
   else if( ( field == c_field_id_Source_Field ) || ( field == c_field_name_Source_Field ) )
      impl_Source_Field( "" );
   else if( ( field == c_field_id_Source_Parent ) || ( field == c_field_name_Source_Parent ) )
      impl_Source_Parent( "" );
   else if( ( field == c_field_id_Source_Parent_Class ) || ( field == c_field_name_Source_Parent_Class ) )
      impl_Source_Parent_Class( "" );
   else if( ( field == c_field_id_Type ) || ( field == c_field_name_Type ) )
      impl_Type( "" );
   else if( ( field == c_field_id_View ) || ( field == c_field_name_View ) )
      impl_View( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_View_Field::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      v_Access_Permission = value;
   else if( ( field == c_field_id_Change_Permission ) || ( field == c_field_name_Change_Permission ) )
      v_Change_Permission = value;
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      v_Class = value;
   else if( ( field == c_field_id_Link_Permission ) || ( field == c_field_name_Link_Permission ) )
      v_Link_Permission = value;
   else if( ( field == c_field_id_Restriction_Spec ) || ( field == c_field_name_Restriction_Spec ) )
      v_Restriction_Spec = value;
   else if( ( field == c_field_id_Source_Child ) || ( field == c_field_name_Source_Child ) )
      v_Source_Child = value;
   else if( ( field == c_field_id_Source_Edit_Child ) || ( field == c_field_name_Source_Edit_Child ) )
      v_Source_Edit_Child = value;
   else if( ( field == c_field_id_Source_Field ) || ( field == c_field_name_Source_Field ) )
      v_Source_Field = value;
   else if( ( field == c_field_id_Source_Parent ) || ( field == c_field_name_Source_Parent ) )
      v_Source_Parent = value;
   else if( ( field == c_field_id_Source_Parent_Class ) || ( field == c_field_name_Source_Parent_Class ) )
      v_Source_Parent_Class = value;
   else if( ( field == c_field_id_Type ) || ( field == c_field_name_Type ) )
      v_Type = value;
   else if( ( field == c_field_id_View ) || ( field == c_field_name_View ) )
      v_View = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_View_Field::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      return v_Access_Permission;
   else if( ( field == c_field_id_Change_Permission ) || ( field == c_field_name_Change_Permission ) )
      return v_Change_Permission;
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      return v_Class;
   else if( ( field == c_field_id_Link_Permission ) || ( field == c_field_name_Link_Permission ) )
      return v_Link_Permission;
   else if( ( field == c_field_id_Restriction_Spec ) || ( field == c_field_name_Restriction_Spec ) )
      return v_Restriction_Spec;
   else if( ( field == c_field_id_Source_Child ) || ( field == c_field_name_Source_Child ) )
      return v_Source_Child;
   else if( ( field == c_field_id_Source_Edit_Child ) || ( field == c_field_name_Source_Edit_Child ) )
      return v_Source_Edit_Child;
   else if( ( field == c_field_id_Source_Field ) || ( field == c_field_name_Source_Field ) )
      return v_Source_Field;
   else if( ( field == c_field_id_Source_Parent ) || ( field == c_field_name_Source_Parent ) )
      return v_Source_Parent;
   else if( ( field == c_field_id_Source_Parent_Class ) || ( field == c_field_name_Source_Parent_Class ) )
      return v_Source_Parent_Class;
   else if( ( field == c_field_id_Type ) || ( field == c_field_name_Type ) )
      return v_Type;
   else if( ( field == c_field_id_View ) || ( field == c_field_name_View ) )
      return v_View;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_View_Field::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Access_Permission, v_Access_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Change_Permission, v_Change_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Class, v_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Link_Permission, v_Link_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Restriction_Spec, v_Restriction_Spec ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Child, v_Source_Child ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Edit_Child, v_Source_Edit_Child ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Field, v_Source_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Parent, v_Source_Parent ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Parent_Class, v_Source_Parent_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Type, v_Type ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_View, v_View ) );
}

void Meta_View_Field::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_View_Field::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_View_Field::impl::clear( )
{
   v_Access_Restriction = g_default_Access_Restriction;
   v_Access_Scope = g_default_Access_Scope;
   v_Alignment = g_default_Alignment;
   v_Allow_Anonymous_Access = g_default_Allow_Anonymous_Access;
   v_Change_Restriction = g_default_Change_Restriction;
   v_Change_Scope = g_default_Change_Scope;
   v_Child_List_Extra_Option = g_default_Child_List_Extra_Option;
   v_Date_Precision_Option = g_default_Date_Precision_Option;
   v_Enum_Finishes_At = g_default_Enum_Finishes_At;
   v_Enum_Starts_At = g_default_Enum_Starts_At;
   v_FK_Trigger_Behaviour = g_default_FK_Trigger_Behaviour;
   v_FK_Trigger_Option = g_default_FK_Trigger_Option;
   v_Font_Size = g_default_Font_Size;
   v_Ignore_Manual_Links = g_default_Ignore_Manual_Links;
   v_Label_Source_Child = g_default_Label_Source_Child;
   v_Link_Restriction = g_default_Link_Restriction;
   v_Mandatory_Option = g_default_Mandatory_Option;
   v_Name = g_default_Name;
   v_New_Source = g_default_New_Source;
   v_New_Value = g_default_New_Value;
   v_Order = g_default_Order;
   v_Orientation = g_default_Orientation;
   v_Show_Hide_Start_Point = g_default_Show_Hide_Start_Point;
   v_Sort_Manually = g_default_Sort_Manually;
   v_Tab_Name = g_default_Tab_Name;
   v_Trigger_Behaviour = g_default_Trigger_Behaviour;
   v_Trigger_For_State = g_default_Trigger_For_State;
   v_Trigger_Option = g_default_Trigger_Option;
   v_Use_Full_Height = g_default_Use_Full_Height;
   v_Use_Full_Width = g_default_Use_Full_Width;
   v_Use_Source_Parent = g_default_Use_Source_Parent;

   v_Access_Permission = string( );
   if( cp_Access_Permission )
      p_obj->setup_foreign_key( *cp_Access_Permission, v_Access_Permission );

   v_Change_Permission = string( );
   if( cp_Change_Permission )
      p_obj->setup_foreign_key( *cp_Change_Permission, v_Change_Permission );

   v_Class = string( );
   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   v_Link_Permission = string( );
   if( cp_Link_Permission )
      p_obj->setup_foreign_key( *cp_Link_Permission, v_Link_Permission );

   v_Restriction_Spec = string( );
   if( cp_Restriction_Spec )
      p_obj->setup_foreign_key( *cp_Restriction_Spec, v_Restriction_Spec );

   v_Source_Child = string( );
   if( cp_Source_Child )
      p_obj->setup_foreign_key( *cp_Source_Child, v_Source_Child );

   v_Source_Edit_Child = string( );
   if( cp_Source_Edit_Child )
      p_obj->setup_foreign_key( *cp_Source_Edit_Child, v_Source_Edit_Child );

   v_Source_Field = string( );
   if( cp_Source_Field )
      p_obj->setup_foreign_key( *cp_Source_Field, v_Source_Field );

   v_Source_Parent = string( );
   if( cp_Source_Parent )
      p_obj->setup_foreign_key( *cp_Source_Parent, v_Source_Parent );

   v_Source_Parent_Class = string( );
   if( cp_Source_Parent_Class )
      p_obj->setup_foreign_key( *cp_Source_Parent_Class, v_Source_Parent_Class );

   v_Type = string( );
   if( cp_Type )
      p_obj->setup_foreign_key( *cp_Type, v_Type );

   v_View = string( );
   if( cp_View )
      p_obj->setup_foreign_key( *cp_View, v_View );
}

bool Meta_View_Field::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_View_Field::impl::validate(
 uint64_t state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( is_null( v_Order ) && !value_will_be_provided( c_field_name_Order ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Order,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Order ) ) ) ) );

   if( v_Class.empty( ) && !value_will_be_provided( c_field_name_Class ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Class ) ) ) ) );

   if( v_Type.empty( ) && !value_will_be_provided( c_field_name_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Type,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Type ) ) ) ) );

   if( v_View.empty( ) && !value_will_be_provided( c_field_name_View ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_View,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_View ) ) ) ) );

   if( !is_null( v_Name )
    && ( v_Name != g_default_Name
    || !value_will_be_provided( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   if( !is_null( v_New_Value )
    && ( v_New_Value != g_default_New_Value
    || !value_will_be_provided( c_field_name_New_Value ) )
    && !g_New_Value_domain.is_valid( v_New_Value, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_New_Value,
       get_module_string( c_field_display_name_New_Value ) + " " + error_message ) );

   if( !is_null( v_Tab_Name )
    && ( v_Tab_Name != g_default_Tab_Name
    || !value_will_be_provided( c_field_name_Tab_Name ) )
    && !g_Tab_Name_domain.is_valid( v_Tab_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Tab_Name,
       get_module_string( c_field_display_name_Tab_Name ) + " " + error_message ) );

   if( !g_view_field_restrict_enum.count( v_Access_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Access_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Access_Restriction ) ) ) ) );

   if( !g_view_field_access_scope_enum.count( v_Access_Scope ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Access_Scope,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Access_Scope ) ) ) ) );

   if( !g_field_alignment_enum.count( v_Alignment ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Alignment,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Alignment ) ) ) ) );

   if( !g_view_field_restrict_enum.count( v_Change_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Change_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Change_Restriction ) ) ) ) );

   if( !g_view_field_change_scope_enum.count( v_Change_Scope ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Change_Scope,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Change_Scope ) ) ) ) );

   if( !g_view_field_child_list_extra_option_enum.count( v_Child_List_Extra_Option ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Child_List_Extra_Option,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Child_List_Extra_Option ) ) ) ) );

   if( !g_view_field_date_precision_option_enum.count( v_Date_Precision_Option ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Date_Precision_Option,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Date_Precision_Option ) ) ) ) );

   if( !g_view_field_enum_at_enum.count( v_Enum_Finishes_At ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Enum_Finishes_At,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Enum_Finishes_At ) ) ) ) );

   if( !g_view_field_enum_at_enum.count( v_Enum_Starts_At ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Enum_Starts_At,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Enum_Starts_At ) ) ) ) );

   if( !g_view_field_fk_trigger_behaviour_enum.count( v_FK_Trigger_Behaviour ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_FK_Trigger_Behaviour,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_FK_Trigger_Behaviour ) ) ) ) );

   if( !g_view_field_fk_trigger_option_enum.count( v_FK_Trigger_Option ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_FK_Trigger_Option,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_FK_Trigger_Option ) ) ) ) );

   if( !g_font_size_enum.count( v_Font_Size ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Font_Size,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Font_Size ) ) ) ) );

   if( !g_label_source_child_enum.count( v_Label_Source_Child ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Label_Source_Child,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Label_Source_Child ) ) ) ) );

   if( !g_view_field_link_restrict_enum.count( v_Link_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Link_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Link_Restriction ) ) ) ) );

   if( !g_view_field_mandatory_option_enum.count( v_Mandatory_Option ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Mandatory_Option,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Mandatory_Option ) ) ) ) );

   if( !g_view_field_new_source_enum.count( v_New_Source ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_New_Source,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_New_Source ) ) ) ) );

   if( !g_orientation_enum.count( v_Orientation ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Orientation,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Orientation ) ) ) ) );

   if( !g_view_field_trigger_behaviour_enum.count( v_Trigger_Behaviour ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Trigger_Behaviour,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Trigger_Behaviour ) ) ) ) );

   if( !g_view_field_trigger_for_state_enum.count( v_Trigger_For_State ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Trigger_For_State,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Trigger_For_State ) ) ) ) );

   if( !g_view_field_trigger_option_enum.count( v_Trigger_Option ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Trigger_Option,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Trigger_Option ) ) ) ) );

   // [(start check_cond_non_null)] 600395
   if( get_obj( ).Use_Source_Parent( ) && is_null( get_obj( ).Source_Parent( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Source_Parent, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Source_Parent ) ) ) ) );
   // [(finish check_cond_non_null)] 600395

   // [(start check_cond_non_null)] 610395
   if( get_obj( ).Use_Source_Parent( ) && is_null( get_obj( ).Source_Child( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Source_Child, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Source_Child ) ) ) ) );
   // [(finish check_cond_non_null)] 610395

   // [(start check_cond_non_null)] 620395
   if( get_obj( ).Type( ).Name( ) == "field" && !get_obj( ).Use_Source_Parent( ) && is_null( get_obj( ).Source_Field( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Source_Field, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Source_Field ) ) ) ) );
   // [(finish check_cond_non_null)] 620395

   // [<start validate>]
   // [<finish validate>]
}

void Meta_View_Field::impl::validate_set_fields(
 set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( !is_null( v_Name )
    && ( fields_set.count( c_field_id_Name ) || fields_set.count( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   if( !is_null( v_New_Value )
    && ( fields_set.count( c_field_id_New_Value ) || fields_set.count( c_field_name_New_Value ) )
    && !g_New_Value_domain.is_valid( v_New_Value, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_New_Value,
       get_module_string( c_field_display_name_New_Value ) + " " + error_message ) );

   if( !is_null( v_Tab_Name )
    && ( fields_set.count( c_field_id_Tab_Name ) || fields_set.count( c_field_name_Tab_Name ) )
    && !g_Tab_Name_domain.is_valid( v_Tab_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Tab_Name,
       get_module_string( c_field_display_name_Tab_Name ) + " " + error_message ) );
}

void Meta_View_Field::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Access_Permission )
      p_obj->setup_foreign_key( *cp_Access_Permission, v_Access_Permission );

   if( cp_Change_Permission )
      p_obj->setup_foreign_key( *cp_Change_Permission, v_Change_Permission );

   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   if( cp_Link_Permission )
      p_obj->setup_foreign_key( *cp_Link_Permission, v_Link_Permission );

   if( cp_Restriction_Spec )
      p_obj->setup_foreign_key( *cp_Restriction_Spec, v_Restriction_Spec );

   if( cp_Source_Child )
      p_obj->setup_foreign_key( *cp_Source_Child, v_Source_Child );

   if( cp_Source_Edit_Child )
      p_obj->setup_foreign_key( *cp_Source_Edit_Child, v_Source_Edit_Child );

   if( cp_Source_Field )
      p_obj->setup_foreign_key( *cp_Source_Field, v_Source_Field );

   if( cp_Source_Parent )
      p_obj->setup_foreign_key( *cp_Source_Parent, v_Source_Parent );

   if( cp_Source_Parent_Class )
      p_obj->setup_foreign_key( *cp_Source_Parent_Class, v_Source_Parent_Class );

   if( cp_Type )
      p_obj->setup_foreign_key( *cp_Type, v_Type );

   if( cp_View )
      p_obj->setup_foreign_key( *cp_View, v_View );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_from_search_replace)] 600385
   if( !get_obj( ).get_key( ).empty( )
    && !get_obj( ).get_is_for_peer( )
    && ( get_obj( ).needs_field_value( "Name" )
    || required_transients.count( "Name" ) ) )
   {
      string str( get_obj( ).Type( ).View_Field_Name( ) );

      get_obj( ).Name( str );

      get_obj( ).add_search_replacement( "Name", "{tab}", to_rep_string( get_obj( ).Tab_Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{sfield}", to_rep_string( get_obj( ).Source_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{sparent}", to_rep_string( get_obj( ).Source_Parent( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{schild}", to_rep_string( get_obj( ).Source_Child( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{sechild}", to_rep_string( get_obj( ).Source_Edit_Child( ).Name( ) ) );

      get_obj( ).set_search_replace_separator( "Name", '_' );

      get_obj( ).set_search_replace_has_opt_prefixing( "Name" );
   }
   // [(finish field_from_search_replace)] 600385

   // [<start after_fetch>]
//nyi
   if( get_obj( ).Source_Field( ).Access_Scope( ) != 0 )
      get_obj( ).Access_Scope( get_obj( ).Source_Field( ).Access_Scope( ) );

   if( get_obj( ).Source_Parent( ).Access_Scope( ) != 0 )
      get_obj( ).Access_Scope( get_obj( ).Source_Parent( ).Access_Scope( ) );

   if( get_obj( ).Source_Field( ).Access_Restriction( ) != 0 )
      get_obj( ).Access_Restriction( get_obj( ).Source_Field( ).Access_Restriction( ) );

   if( get_obj( ).Source_Parent( ).Access_Restriction( ) != 0 )
      get_obj( ).Access_Restriction( get_obj( ).Source_Parent( ).Access_Restriction( ) );

   if( get_obj( ).Source_Field( ).Change_Scope( ) != 0 )
      get_obj( ).Change_Scope( get_obj( ).Source_Field( ).Change_Scope( ) );

   if( get_obj( ).Source_Parent( ).Change_Scope( ) != 0 )
      get_obj( ).Change_Scope( get_obj( ).Source_Parent( ).Change_Scope( ) );

   if( get_obj( ).Source_Field( ).Change_Restriction( ) != 0 )
      get_obj( ).Change_Restriction( get_obj( ).Source_Field( ).Change_Restriction( ) );

   if( get_obj( ).Source_Parent( ).Change_Restriction( ) != 0 )
      get_obj( ).Change_Restriction( get_obj( ).Source_Parent( ).Change_Restriction( ) );
   // [<finish after_fetch>]
}

void Meta_View_Field::impl::finalise_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_View_Field::impl::at_create( )
{
   // [(start fk_default)] 600350
   if( is_null( get_obj( ).Type( ) ) )
      get_obj( ).Type( "field" );
   // [(finish fk_default)] 600350

   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_View_Field::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_View_Field::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start default_to_field)] 600355
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Class( ) == g_default_Class ) )
      get_obj( ).Class( get_obj( ).View( ).Class( ) );
   // [(finish default_to_field)] 600355

   // [(start default_from_key)] 600360
   if( !get_obj( ).get_clone_key( ).empty( ) || ( is_create && ( get_obj( ).Order( ) == g_default_Order ) ) )
      get_obj( ).Order( get_obj( ).get_key( ) );
   // [(finish default_from_key)] 600360

   // [(start field_clear)] 600375
   if( get_obj( ).Use_Source_Parent( ) )
      get_obj( ).Source_Field( string( ) );
   // [(finish field_clear)] 600375

   // [(start field_clear)] 600376
   if( !get_obj( ).Use_Source_Parent( ) )
      get_obj( ).Source_Parent( string( ) );
   // [(finish field_clear)] 600376

   // [(start field_clear)] 600377
   if( !get_obj( ).Use_Source_Parent( ) )
      get_obj( ).Source_Child( string( ) );
   // [(finish field_clear)] 600377

   // [(start field_clear)] 600378
   if( !get_obj( ).Use_Source_Parent( ) )
      get_obj( ).Source_Edit_Child( string( ) );
   // [(finish field_clear)] 600378

   // [(start field_from_changed_fk)] 630395
   if( get_obj( ).Source_Parent( ).has_changed( ) )
      get_obj( ).Source_Parent_Class( get_obj( ).Source_Parent( ).Parent_Class( ) );
   // [(finish field_from_changed_fk)] 630395

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_View_Field::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_View_Field::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_store>]
//nyi
   // NOTE: Due to being transient detecting a duplicate Name can
   // only done after the record is stored (as its value is given
   // to it in after_fetch).
   if( !session_skip_validation( ) )
   {
      class_pointer< Meta_View > cp_parent( e_create_instance );

      if( !get_obj( ).View( ).get_key( ).empty( ) )
      {
         cp_parent->perform_fetch( get_obj( ).View( ).get_key( ) );

         if( cp_parent->child_View_Field( ).iterate_forwards( ) )
         {
            set< string > names;
            do
            {
               if( names.count( cp_parent->child_View_Field( ).Name( ) ) )
                  throw runtime_error( "This View Field has already been created." );

               names.insert( cp_parent->child_View_Field( ).Name( ) );

            } while( cp_parent->child_View_Field( ).iterate_next( ) );
         }
      }
   }
   // [<finish after_store>]
}

bool Meta_View_Field::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_View_Field::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_View_Field::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_View_Field::impl::set_default_values( )
{
   clear( );
}

bool Meta_View_Field::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_View_Field::impl::get_required_transients( ) const
{
   required_transients.clear( );

   get_obj( ).add_required_transients( required_transients );

   set< string > dependents( required_transients.begin( ), required_transients.end( ) );

   p_obj->get_required_field_names( required_transients, true, &dependents );

   int iterations = 0;

   // NOTE: It is possible that due to "interdependent" required fields
   // some required fields may not have been added in the first or even
   // later calls to "get_required_field_names" so continue calling the
   // function until no further field names have been added.
   size_t num_required = required_transients.size( );
   while( num_required )
   {
      p_obj->get_required_field_names( required_transients, true, &dependents );

      if( required_transients.size( ) == num_required )
         break;

      if( ++iterations > 100 )
         throw runtime_error( "unexpected excessive get_required_field_names( ) iterations in get_required_transients( )" );

      num_required = required_transients.size( );
   }
}

#undef MODULE_TRACE
#define MODULE_TRACE( x ) trace( x )

Meta_View_Field::Meta_View_Field( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_View_Field::~Meta_View_Field( )
{
   cleanup( );
   delete p_impl;
}

int Meta_View_Field::Access_Restriction( ) const
{
   return p_impl->impl_Access_Restriction( );
}

void Meta_View_Field::Access_Restriction( int Access_Restriction )
{
   p_impl->impl_Access_Restriction( Access_Restriction );
}

int Meta_View_Field::Access_Scope( ) const
{
   return p_impl->impl_Access_Scope( );
}

void Meta_View_Field::Access_Scope( int Access_Scope )
{
   p_impl->impl_Access_Scope( Access_Scope );
}

int Meta_View_Field::Alignment( ) const
{
   return p_impl->impl_Alignment( );
}

void Meta_View_Field::Alignment( int Alignment )
{
   p_impl->impl_Alignment( Alignment );
}

bool Meta_View_Field::Allow_Anonymous_Access( ) const
{
   return p_impl->impl_Allow_Anonymous_Access( );
}

void Meta_View_Field::Allow_Anonymous_Access( bool Allow_Anonymous_Access )
{
   p_impl->impl_Allow_Anonymous_Access( Allow_Anonymous_Access );
}

int Meta_View_Field::Change_Restriction( ) const
{
   return p_impl->impl_Change_Restriction( );
}

void Meta_View_Field::Change_Restriction( int Change_Restriction )
{
   p_impl->impl_Change_Restriction( Change_Restriction );
}

int Meta_View_Field::Change_Scope( ) const
{
   return p_impl->impl_Change_Scope( );
}

void Meta_View_Field::Change_Scope( int Change_Scope )
{
   p_impl->impl_Change_Scope( Change_Scope );
}

int Meta_View_Field::Child_List_Extra_Option( ) const
{
   return p_impl->impl_Child_List_Extra_Option( );
}

void Meta_View_Field::Child_List_Extra_Option( int Child_List_Extra_Option )
{
   p_impl->impl_Child_List_Extra_Option( Child_List_Extra_Option );
}

int Meta_View_Field::Date_Precision_Option( ) const
{
   return p_impl->impl_Date_Precision_Option( );
}

void Meta_View_Field::Date_Precision_Option( int Date_Precision_Option )
{
   p_impl->impl_Date_Precision_Option( Date_Precision_Option );
}

int Meta_View_Field::Enum_Finishes_At( ) const
{
   return p_impl->impl_Enum_Finishes_At( );
}

void Meta_View_Field::Enum_Finishes_At( int Enum_Finishes_At )
{
   p_impl->impl_Enum_Finishes_At( Enum_Finishes_At );
}

int Meta_View_Field::Enum_Starts_At( ) const
{
   return p_impl->impl_Enum_Starts_At( );
}

void Meta_View_Field::Enum_Starts_At( int Enum_Starts_At )
{
   p_impl->impl_Enum_Starts_At( Enum_Starts_At );
}

int Meta_View_Field::FK_Trigger_Behaviour( ) const
{
   return p_impl->impl_FK_Trigger_Behaviour( );
}

void Meta_View_Field::FK_Trigger_Behaviour( int FK_Trigger_Behaviour )
{
   p_impl->impl_FK_Trigger_Behaviour( FK_Trigger_Behaviour );
}

int Meta_View_Field::FK_Trigger_Option( ) const
{
   return p_impl->impl_FK_Trigger_Option( );
}

void Meta_View_Field::FK_Trigger_Option( int FK_Trigger_Option )
{
   p_impl->impl_FK_Trigger_Option( FK_Trigger_Option );
}

int Meta_View_Field::Font_Size( ) const
{
   return p_impl->impl_Font_Size( );
}

void Meta_View_Field::Font_Size( int Font_Size )
{
   p_impl->impl_Font_Size( Font_Size );
}

bool Meta_View_Field::Ignore_Manual_Links( ) const
{
   return p_impl->impl_Ignore_Manual_Links( );
}

void Meta_View_Field::Ignore_Manual_Links( bool Ignore_Manual_Links )
{
   p_impl->impl_Ignore_Manual_Links( Ignore_Manual_Links );
}

int Meta_View_Field::Label_Source_Child( ) const
{
   return p_impl->impl_Label_Source_Child( );
}

void Meta_View_Field::Label_Source_Child( int Label_Source_Child )
{
   p_impl->impl_Label_Source_Child( Label_Source_Child );
}

int Meta_View_Field::Link_Restriction( ) const
{
   return p_impl->impl_Link_Restriction( );
}

void Meta_View_Field::Link_Restriction( int Link_Restriction )
{
   p_impl->impl_Link_Restriction( Link_Restriction );
}

int Meta_View_Field::Mandatory_Option( ) const
{
   return p_impl->impl_Mandatory_Option( );
}

void Meta_View_Field::Mandatory_Option( int Mandatory_Option )
{
   p_impl->impl_Mandatory_Option( Mandatory_Option );
}

const string& Meta_View_Field::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_View_Field::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

int Meta_View_Field::New_Source( ) const
{
   return p_impl->impl_New_Source( );
}

void Meta_View_Field::New_Source( int New_Source )
{
   p_impl->impl_New_Source( New_Source );
}

const string& Meta_View_Field::New_Value( ) const
{
   return p_impl->impl_New_Value( );
}

void Meta_View_Field::New_Value( const string& New_Value )
{
   p_impl->impl_New_Value( New_Value );
}

const string& Meta_View_Field::Order( ) const
{
   return p_impl->impl_Order( );
}

void Meta_View_Field::Order( const string& Order )
{
   p_impl->impl_Order( Order );
}

int Meta_View_Field::Orientation( ) const
{
   return p_impl->impl_Orientation( );
}

void Meta_View_Field::Orientation( int Orientation )
{
   p_impl->impl_Orientation( Orientation );
}

bool Meta_View_Field::Show_Hide_Start_Point( ) const
{
   return p_impl->impl_Show_Hide_Start_Point( );
}

void Meta_View_Field::Show_Hide_Start_Point( bool Show_Hide_Start_Point )
{
   p_impl->impl_Show_Hide_Start_Point( Show_Hide_Start_Point );
}

bool Meta_View_Field::Sort_Manually( ) const
{
   return p_impl->impl_Sort_Manually( );
}

void Meta_View_Field::Sort_Manually( bool Sort_Manually )
{
   p_impl->impl_Sort_Manually( Sort_Manually );
}

const string& Meta_View_Field::Tab_Name( ) const
{
   return p_impl->impl_Tab_Name( );
}

void Meta_View_Field::Tab_Name( const string& Tab_Name )
{
   p_impl->impl_Tab_Name( Tab_Name );
}

int Meta_View_Field::Trigger_Behaviour( ) const
{
   return p_impl->impl_Trigger_Behaviour( );
}

void Meta_View_Field::Trigger_Behaviour( int Trigger_Behaviour )
{
   p_impl->impl_Trigger_Behaviour( Trigger_Behaviour );
}

int Meta_View_Field::Trigger_For_State( ) const
{
   return p_impl->impl_Trigger_For_State( );
}

void Meta_View_Field::Trigger_For_State( int Trigger_For_State )
{
   p_impl->impl_Trigger_For_State( Trigger_For_State );
}

int Meta_View_Field::Trigger_Option( ) const
{
   return p_impl->impl_Trigger_Option( );
}

void Meta_View_Field::Trigger_Option( int Trigger_Option )
{
   p_impl->impl_Trigger_Option( Trigger_Option );
}

bool Meta_View_Field::Use_Full_Height( ) const
{
   return p_impl->impl_Use_Full_Height( );
}

void Meta_View_Field::Use_Full_Height( bool Use_Full_Height )
{
   p_impl->impl_Use_Full_Height( Use_Full_Height );
}

bool Meta_View_Field::Use_Full_Width( ) const
{
   return p_impl->impl_Use_Full_Width( );
}

void Meta_View_Field::Use_Full_Width( bool Use_Full_Width )
{
   p_impl->impl_Use_Full_Width( Use_Full_Width );
}

bool Meta_View_Field::Use_Source_Parent( ) const
{
   return p_impl->impl_Use_Source_Parent( );
}

void Meta_View_Field::Use_Source_Parent( bool Use_Source_Parent )
{
   p_impl->impl_Use_Source_Parent( Use_Source_Parent );
}

Meta_Permission& Meta_View_Field::Access_Permission( )
{
   return p_impl->impl_Access_Permission( );
}

const Meta_Permission& Meta_View_Field::Access_Permission( ) const
{
   return p_impl->impl_Access_Permission( );
}

void Meta_View_Field::Access_Permission( const string& key )
{
   p_impl->impl_Access_Permission( key );
}

Meta_Permission& Meta_View_Field::Change_Permission( )
{
   return p_impl->impl_Change_Permission( );
}

const Meta_Permission& Meta_View_Field::Change_Permission( ) const
{
   return p_impl->impl_Change_Permission( );
}

void Meta_View_Field::Change_Permission( const string& key )
{
   p_impl->impl_Change_Permission( key );
}

Meta_Class& Meta_View_Field::Class( )
{
   return p_impl->impl_Class( );
}

const Meta_Class& Meta_View_Field::Class( ) const
{
   return p_impl->impl_Class( );
}

void Meta_View_Field::Class( const string& key )
{
   p_impl->impl_Class( key );
}

Meta_Permission& Meta_View_Field::Link_Permission( )
{
   return p_impl->impl_Link_Permission( );
}

const Meta_Permission& Meta_View_Field::Link_Permission( ) const
{
   return p_impl->impl_Link_Permission( );
}

void Meta_View_Field::Link_Permission( const string& key )
{
   p_impl->impl_Link_Permission( key );
}

Meta_Specification& Meta_View_Field::Restriction_Spec( )
{
   return p_impl->impl_Restriction_Spec( );
}

const Meta_Specification& Meta_View_Field::Restriction_Spec( ) const
{
   return p_impl->impl_Restriction_Spec( );
}

void Meta_View_Field::Restriction_Spec( const string& key )
{
   p_impl->impl_Restriction_Spec( key );
}

Meta_Field& Meta_View_Field::Source_Child( )
{
   return p_impl->impl_Source_Child( );
}

const Meta_Field& Meta_View_Field::Source_Child( ) const
{
   return p_impl->impl_Source_Child( );
}

void Meta_View_Field::Source_Child( const string& key )
{
   p_impl->impl_Source_Child( key );
}

Meta_Field& Meta_View_Field::Source_Edit_Child( )
{
   return p_impl->impl_Source_Edit_Child( );
}

const Meta_Field& Meta_View_Field::Source_Edit_Child( ) const
{
   return p_impl->impl_Source_Edit_Child( );
}

void Meta_View_Field::Source_Edit_Child( const string& key )
{
   p_impl->impl_Source_Edit_Child( key );
}

Meta_Field& Meta_View_Field::Source_Field( )
{
   return p_impl->impl_Source_Field( );
}

const Meta_Field& Meta_View_Field::Source_Field( ) const
{
   return p_impl->impl_Source_Field( );
}

void Meta_View_Field::Source_Field( const string& key )
{
   p_impl->impl_Source_Field( key );
}

Meta_Field& Meta_View_Field::Source_Parent( )
{
   return p_impl->impl_Source_Parent( );
}

const Meta_Field& Meta_View_Field::Source_Parent( ) const
{
   return p_impl->impl_Source_Parent( );
}

void Meta_View_Field::Source_Parent( const string& key )
{
   p_impl->impl_Source_Parent( key );
}

Meta_Class& Meta_View_Field::Source_Parent_Class( )
{
   return p_impl->impl_Source_Parent_Class( );
}

const Meta_Class& Meta_View_Field::Source_Parent_Class( ) const
{
   return p_impl->impl_Source_Parent_Class( );
}

void Meta_View_Field::Source_Parent_Class( const string& key )
{
   p_impl->impl_Source_Parent_Class( key );
}

Meta_View_Field_Type& Meta_View_Field::Type( )
{
   return p_impl->impl_Type( );
}

const Meta_View_Field_Type& Meta_View_Field::Type( ) const
{
   return p_impl->impl_Type( );
}

void Meta_View_Field::Type( const string& key )
{
   p_impl->impl_Type( key );
}

Meta_View& Meta_View_Field::View( )
{
   return p_impl->impl_View( );
}

const Meta_View& Meta_View_Field::View( ) const
{
   return p_impl->impl_View( );
}

void Meta_View_Field::View( const string& key )
{
   p_impl->impl_View( key );
}

void Meta_View_Field::Move_Down( const string& Restrict_Fields, const string& Restrict_Values )
{
   p_impl->impl_Move_Down( Restrict_Fields, Restrict_Values );
}

void Meta_View_Field::Move_Up( const string& Restrict_Fields, const string& Restrict_Values )
{
   p_impl->impl_Move_Up( Restrict_Fields, Restrict_Values );
}

string Meta_View_Field::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_View_Field::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_View_Field::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_View_Field::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_View_Field::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_View_Field::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_View_Field::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_View_Field::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_View_Field::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_View_Field::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_View_Field::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_View_Field::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_View_Field::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_View_Field::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_View_Field::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_View_Field::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_View_Field::clear( )
{
   p_impl->clear( );
}

void Meta_View_Field::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_View_Field::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_View_Field::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_View_Field::finalise_fetch( bool skip_set_original )
{
   p_impl->finalise_fetch( skip_set_original );
}

void Meta_View_Field::at_create( )
{
   p_impl->at_create( );
}

void Meta_View_Field::post_init( )
{
   p_impl->post_init( );
}

void Meta_View_Field::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_View_Field::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_View_Field::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_View_Field::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_View_Field::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_View_Field::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_View_Field::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_View_Field::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_View_Field::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( 0 );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Access_Permission )
   {
      p_id = c_field_id_Access_Permission;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Access_Restriction )
   {
      p_id = c_field_id_Access_Restriction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Access_Scope )
   {
      p_id = c_field_id_Access_Scope;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Alignment )
   {
      p_id = c_field_id_Alignment;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Anonymous_Access )
   {
      p_id = c_field_id_Allow_Anonymous_Access;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Change_Permission )
   {
      p_id = c_field_id_Change_Permission;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Change_Restriction )
   {
      p_id = c_field_id_Change_Restriction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Change_Scope )
   {
      p_id = c_field_id_Change_Scope;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Child_List_Extra_Option )
   {
      p_id = c_field_id_Child_List_Extra_Option;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Class )
   {
      p_id = c_field_id_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Date_Precision_Option )
   {
      p_id = c_field_id_Date_Precision_Option;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Enum_Finishes_At )
   {
      p_id = c_field_id_Enum_Finishes_At;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Enum_Starts_At )
   {
      p_id = c_field_id_Enum_Starts_At;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_FK_Trigger_Behaviour )
   {
      p_id = c_field_id_FK_Trigger_Behaviour;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_FK_Trigger_Option )
   {
      p_id = c_field_id_FK_Trigger_Option;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Font_Size )
   {
      p_id = c_field_id_Font_Size;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Ignore_Manual_Links )
   {
      p_id = c_field_id_Ignore_Manual_Links;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Label_Source_Child )
   {
      p_id = c_field_id_Label_Source_Child;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Link_Permission )
   {
      p_id = c_field_id_Link_Permission;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Link_Restriction )
   {
      p_id = c_field_id_Link_Restriction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Mandatory_Option )
   {
      p_id = c_field_id_Mandatory_Option;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Name )
   {
      p_id = c_field_id_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_New_Source )
   {
      p_id = c_field_id_New_Source;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_New_Value )
   {
      p_id = c_field_id_New_Value;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Order )
   {
      p_id = c_field_id_Order;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Orientation )
   {
      p_id = c_field_id_Orientation;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Restriction_Spec )
   {
      p_id = c_field_id_Restriction_Spec;

      if( p_type_name )
         *p_type_name = "Meta_Specification";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Show_Hide_Start_Point )
   {
      p_id = c_field_id_Show_Hide_Start_Point;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Sort_Manually )
   {
      p_id = c_field_id_Sort_Manually;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Source_Child )
   {
      p_id = c_field_id_Source_Child;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_Edit_Child )
   {
      p_id = c_field_id_Source_Edit_Child;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_Field )
   {
      p_id = c_field_id_Source_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_Parent )
   {
      p_id = c_field_id_Source_Parent;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_Parent_Class )
   {
      p_id = c_field_id_Source_Parent_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Tab_Name )
   {
      p_id = c_field_id_Tab_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Trigger_Behaviour )
   {
      p_id = c_field_id_Trigger_Behaviour;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Trigger_For_State )
   {
      p_id = c_field_id_Trigger_For_State;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Trigger_Option )
   {
      p_id = c_field_id_Trigger_Option;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Type )
   {
      p_id = c_field_id_Type;

      if( p_type_name )
         *p_type_name = "Meta_View_Field_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Use_Full_Height )
   {
      p_id = c_field_id_Use_Full_Height;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Full_Width )
   {
      p_id = c_field_id_Use_Full_Width;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Source_Parent )
   {
      p_id = c_field_id_Use_Source_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_View )
   {
      p_id = c_field_id_View;

      if( p_type_name )
         *p_type_name = "Meta_View";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_id;
}

const char* Meta_View_Field::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( 0 );

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Access_Permission )
   {
      p_name = c_field_name_Access_Permission;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Access_Restriction )
   {
      p_name = c_field_name_Access_Restriction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Access_Scope )
   {
      p_name = c_field_name_Access_Scope;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Alignment )
   {
      p_name = c_field_name_Alignment;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Anonymous_Access )
   {
      p_name = c_field_name_Allow_Anonymous_Access;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Change_Permission )
   {
      p_name = c_field_name_Change_Permission;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Change_Restriction )
   {
      p_name = c_field_name_Change_Restriction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Change_Scope )
   {
      p_name = c_field_name_Change_Scope;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Child_List_Extra_Option )
   {
      p_name = c_field_name_Child_List_Extra_Option;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Class )
   {
      p_name = c_field_name_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Date_Precision_Option )
   {
      p_name = c_field_name_Date_Precision_Option;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Enum_Finishes_At )
   {
      p_name = c_field_name_Enum_Finishes_At;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Enum_Starts_At )
   {
      p_name = c_field_name_Enum_Starts_At;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_FK_Trigger_Behaviour )
   {
      p_name = c_field_name_FK_Trigger_Behaviour;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_FK_Trigger_Option )
   {
      p_name = c_field_name_FK_Trigger_Option;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Font_Size )
   {
      p_name = c_field_name_Font_Size;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Ignore_Manual_Links )
   {
      p_name = c_field_name_Ignore_Manual_Links;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Label_Source_Child )
   {
      p_name = c_field_name_Label_Source_Child;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Link_Permission )
   {
      p_name = c_field_name_Link_Permission;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Link_Restriction )
   {
      p_name = c_field_name_Link_Restriction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Mandatory_Option )
   {
      p_name = c_field_name_Mandatory_Option;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Name )
   {
      p_name = c_field_name_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_New_Source )
   {
      p_name = c_field_name_New_Source;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_New_Value )
   {
      p_name = c_field_name_New_Value;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Order )
   {
      p_name = c_field_name_Order;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Orientation )
   {
      p_name = c_field_name_Orientation;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Restriction_Spec )
   {
      p_name = c_field_name_Restriction_Spec;

      if( p_type_name )
         *p_type_name = "Meta_Specification";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Show_Hide_Start_Point )
   {
      p_name = c_field_name_Show_Hide_Start_Point;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Sort_Manually )
   {
      p_name = c_field_name_Sort_Manually;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Source_Child )
   {
      p_name = c_field_name_Source_Child;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_Edit_Child )
   {
      p_name = c_field_name_Source_Edit_Child;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_Field )
   {
      p_name = c_field_name_Source_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_Parent )
   {
      p_name = c_field_name_Source_Parent;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_Parent_Class )
   {
      p_name = c_field_name_Source_Parent_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Tab_Name )
   {
      p_name = c_field_name_Tab_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Trigger_Behaviour )
   {
      p_name = c_field_name_Trigger_Behaviour;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Trigger_For_State )
   {
      p_name = c_field_name_Trigger_For_State;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Trigger_Option )
   {
      p_name = c_field_name_Trigger_Option;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Type )
   {
      p_name = c_field_name_Type;

      if( p_type_name )
         *p_type_name = "Meta_View_Field_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Use_Full_Height )
   {
      p_name = c_field_name_Use_Full_Height;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Full_Width )
   {
      p_name = c_field_name_Use_Full_Width;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Source_Parent )
   {
      p_name = c_field_name_Use_Source_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_View )
   {
      p_name = c_field_name_View;

      if( p_type_name )
         *p_type_name = "Meta_View";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_name;
}

string& Meta_View_Field::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_View_Field::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_View_Field::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_View_Field::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_View_Field::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_View_Field::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_View_Field::get_field_uom_symbol( const string& id_or_name ) const
{
   string uom_symbol;

   string name;
   pair< string, string > next;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_uom_symbol" );
   if( ( id_or_name == c_field_id_Access_Permission ) || ( id_or_name == c_field_name_Access_Permission ) )
   {
      name = string( c_field_display_name_Access_Permission );
      get_module_string( c_field_display_name_Access_Permission, &next );
   }
   if( ( id_or_name == c_field_id_Access_Restriction ) || ( id_or_name == c_field_name_Access_Restriction ) )
   {
      name = string( c_field_display_name_Access_Restriction );
      get_module_string( c_field_display_name_Access_Restriction, &next );
   }
   if( ( id_or_name == c_field_id_Access_Scope ) || ( id_or_name == c_field_name_Access_Scope ) )
   {
      name = string( c_field_display_name_Access_Scope );
      get_module_string( c_field_display_name_Access_Scope, &next );
   }
   if( ( id_or_name == c_field_id_Alignment ) || ( id_or_name == c_field_name_Alignment ) )
   {
      name = string( c_field_display_name_Alignment );
      get_module_string( c_field_display_name_Alignment, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Anonymous_Access ) || ( id_or_name == c_field_name_Allow_Anonymous_Access ) )
   {
      name = string( c_field_display_name_Allow_Anonymous_Access );
      get_module_string( c_field_display_name_Allow_Anonymous_Access, &next );
   }
   if( ( id_or_name == c_field_id_Change_Permission ) || ( id_or_name == c_field_name_Change_Permission ) )
   {
      name = string( c_field_display_name_Change_Permission );
      get_module_string( c_field_display_name_Change_Permission, &next );
   }
   if( ( id_or_name == c_field_id_Change_Restriction ) || ( id_or_name == c_field_name_Change_Restriction ) )
   {
      name = string( c_field_display_name_Change_Restriction );
      get_module_string( c_field_display_name_Change_Restriction, &next );
   }
   if( ( id_or_name == c_field_id_Change_Scope ) || ( id_or_name == c_field_name_Change_Scope ) )
   {
      name = string( c_field_display_name_Change_Scope );
      get_module_string( c_field_display_name_Change_Scope, &next );
   }
   if( ( id_or_name == c_field_id_Child_List_Extra_Option ) || ( id_or_name == c_field_name_Child_List_Extra_Option ) )
   {
      name = string( c_field_display_name_Child_List_Extra_Option );
      get_module_string( c_field_display_name_Child_List_Extra_Option, &next );
   }
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
   {
      name = string( c_field_display_name_Class );
      get_module_string( c_field_display_name_Class, &next );
   }
   if( ( id_or_name == c_field_id_Date_Precision_Option ) || ( id_or_name == c_field_name_Date_Precision_Option ) )
   {
      name = string( c_field_display_name_Date_Precision_Option );
      get_module_string( c_field_display_name_Date_Precision_Option, &next );
   }
   if( ( id_or_name == c_field_id_Enum_Finishes_At ) || ( id_or_name == c_field_name_Enum_Finishes_At ) )
   {
      name = string( c_field_display_name_Enum_Finishes_At );
      get_module_string( c_field_display_name_Enum_Finishes_At, &next );
   }
   if( ( id_or_name == c_field_id_Enum_Starts_At ) || ( id_or_name == c_field_name_Enum_Starts_At ) )
   {
      name = string( c_field_display_name_Enum_Starts_At );
      get_module_string( c_field_display_name_Enum_Starts_At, &next );
   }
   if( ( id_or_name == c_field_id_FK_Trigger_Behaviour ) || ( id_or_name == c_field_name_FK_Trigger_Behaviour ) )
   {
      name = string( c_field_display_name_FK_Trigger_Behaviour );
      get_module_string( c_field_display_name_FK_Trigger_Behaviour, &next );
   }
   if( ( id_or_name == c_field_id_FK_Trigger_Option ) || ( id_or_name == c_field_name_FK_Trigger_Option ) )
   {
      name = string( c_field_display_name_FK_Trigger_Option );
      get_module_string( c_field_display_name_FK_Trigger_Option, &next );
   }
   if( ( id_or_name == c_field_id_Font_Size ) || ( id_or_name == c_field_name_Font_Size ) )
   {
      name = string( c_field_display_name_Font_Size );
      get_module_string( c_field_display_name_Font_Size, &next );
   }
   if( ( id_or_name == c_field_id_Ignore_Manual_Links ) || ( id_or_name == c_field_name_Ignore_Manual_Links ) )
   {
      name = string( c_field_display_name_Ignore_Manual_Links );
      get_module_string( c_field_display_name_Ignore_Manual_Links, &next );
   }
   if( ( id_or_name == c_field_id_Label_Source_Child ) || ( id_or_name == c_field_name_Label_Source_Child ) )
   {
      name = string( c_field_display_name_Label_Source_Child );
      get_module_string( c_field_display_name_Label_Source_Child, &next );
   }
   if( ( id_or_name == c_field_id_Link_Permission ) || ( id_or_name == c_field_name_Link_Permission ) )
   {
      name = string( c_field_display_name_Link_Permission );
      get_module_string( c_field_display_name_Link_Permission, &next );
   }
   if( ( id_or_name == c_field_id_Link_Restriction ) || ( id_or_name == c_field_name_Link_Restriction ) )
   {
      name = string( c_field_display_name_Link_Restriction );
      get_module_string( c_field_display_name_Link_Restriction, &next );
   }
   if( ( id_or_name == c_field_id_Mandatory_Option ) || ( id_or_name == c_field_name_Mandatory_Option ) )
   {
      name = string( c_field_display_name_Mandatory_Option );
      get_module_string( c_field_display_name_Mandatory_Option, &next );
   }
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
   {
      name = string( c_field_display_name_Name );
      get_module_string( c_field_display_name_Name, &next );
   }
   if( ( id_or_name == c_field_id_New_Source ) || ( id_or_name == c_field_name_New_Source ) )
   {
      name = string( c_field_display_name_New_Source );
      get_module_string( c_field_display_name_New_Source, &next );
   }
   if( ( id_or_name == c_field_id_New_Value ) || ( id_or_name == c_field_name_New_Value ) )
   {
      name = string( c_field_display_name_New_Value );
      get_module_string( c_field_display_name_New_Value, &next );
   }
   if( ( id_or_name == c_field_id_Order ) || ( id_or_name == c_field_name_Order ) )
   {
      name = string( c_field_display_name_Order );
      get_module_string( c_field_display_name_Order, &next );
   }
   if( ( id_or_name == c_field_id_Orientation ) || ( id_or_name == c_field_name_Orientation ) )
   {
      name = string( c_field_display_name_Orientation );
      get_module_string( c_field_display_name_Orientation, &next );
   }
   if( ( id_or_name == c_field_id_Restriction_Spec ) || ( id_or_name == c_field_name_Restriction_Spec ) )
   {
      name = string( c_field_display_name_Restriction_Spec );
      get_module_string( c_field_display_name_Restriction_Spec, &next );
   }
   if( ( id_or_name == c_field_id_Show_Hide_Start_Point ) || ( id_or_name == c_field_name_Show_Hide_Start_Point ) )
   {
      name = string( c_field_display_name_Show_Hide_Start_Point );
      get_module_string( c_field_display_name_Show_Hide_Start_Point, &next );
   }
   if( ( id_or_name == c_field_id_Sort_Manually ) || ( id_or_name == c_field_name_Sort_Manually ) )
   {
      name = string( c_field_display_name_Sort_Manually );
      get_module_string( c_field_display_name_Sort_Manually, &next );
   }
   if( ( id_or_name == c_field_id_Source_Child ) || ( id_or_name == c_field_name_Source_Child ) )
   {
      name = string( c_field_display_name_Source_Child );
      get_module_string( c_field_display_name_Source_Child, &next );
   }
   if( ( id_or_name == c_field_id_Source_Edit_Child ) || ( id_or_name == c_field_name_Source_Edit_Child ) )
   {
      name = string( c_field_display_name_Source_Edit_Child );
      get_module_string( c_field_display_name_Source_Edit_Child, &next );
   }
   if( ( id_or_name == c_field_id_Source_Field ) || ( id_or_name == c_field_name_Source_Field ) )
   {
      name = string( c_field_display_name_Source_Field );
      get_module_string( c_field_display_name_Source_Field, &next );
   }
   if( ( id_or_name == c_field_id_Source_Parent ) || ( id_or_name == c_field_name_Source_Parent ) )
   {
      name = string( c_field_display_name_Source_Parent );
      get_module_string( c_field_display_name_Source_Parent, &next );
   }
   if( ( id_or_name == c_field_id_Source_Parent_Class ) || ( id_or_name == c_field_name_Source_Parent_Class ) )
   {
      name = string( c_field_display_name_Source_Parent_Class );
      get_module_string( c_field_display_name_Source_Parent_Class, &next );
   }
   if( ( id_or_name == c_field_id_Tab_Name ) || ( id_or_name == c_field_name_Tab_Name ) )
   {
      name = string( c_field_display_name_Tab_Name );
      get_module_string( c_field_display_name_Tab_Name, &next );
   }
   if( ( id_or_name == c_field_id_Trigger_Behaviour ) || ( id_or_name == c_field_name_Trigger_Behaviour ) )
   {
      name = string( c_field_display_name_Trigger_Behaviour );
      get_module_string( c_field_display_name_Trigger_Behaviour, &next );
   }
   if( ( id_or_name == c_field_id_Trigger_For_State ) || ( id_or_name == c_field_name_Trigger_For_State ) )
   {
      name = string( c_field_display_name_Trigger_For_State );
      get_module_string( c_field_display_name_Trigger_For_State, &next );
   }
   if( ( id_or_name == c_field_id_Trigger_Option ) || ( id_or_name == c_field_name_Trigger_Option ) )
   {
      name = string( c_field_display_name_Trigger_Option );
      get_module_string( c_field_display_name_Trigger_Option, &next );
   }
   if( ( id_or_name == c_field_id_Type ) || ( id_or_name == c_field_name_Type ) )
   {
      name = string( c_field_display_name_Type );
      get_module_string( c_field_display_name_Type, &next );
   }
   if( ( id_or_name == c_field_id_Use_Full_Height ) || ( id_or_name == c_field_name_Use_Full_Height ) )
   {
      name = string( c_field_display_name_Use_Full_Height );
      get_module_string( c_field_display_name_Use_Full_Height, &next );
   }
   if( ( id_or_name == c_field_id_Use_Full_Width ) || ( id_or_name == c_field_name_Use_Full_Width ) )
   {
      name = string( c_field_display_name_Use_Full_Width );
      get_module_string( c_field_display_name_Use_Full_Width, &next );
   }
   if( ( id_or_name == c_field_id_Use_Source_Parent ) || ( id_or_name == c_field_name_Use_Source_Parent ) )
   {
      name = string( c_field_display_name_Use_Source_Parent );
      get_module_string( c_field_display_name_Use_Source_Parent, &next );
   }
   if( ( id_or_name == c_field_id_View ) || ( id_or_name == c_field_name_View ) )
   {
      name = string( c_field_display_name_View );
      get_module_string( c_field_display_name_View, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_View_Field::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Access_Permission ) || ( id_or_name == c_field_name_Access_Permission ) )
      display_name = get_module_string( c_field_display_name_Access_Permission );
   if( ( id_or_name == c_field_id_Access_Restriction ) || ( id_or_name == c_field_name_Access_Restriction ) )
      display_name = get_module_string( c_field_display_name_Access_Restriction );
   if( ( id_or_name == c_field_id_Access_Scope ) || ( id_or_name == c_field_name_Access_Scope ) )
      display_name = get_module_string( c_field_display_name_Access_Scope );
   if( ( id_or_name == c_field_id_Alignment ) || ( id_or_name == c_field_name_Alignment ) )
      display_name = get_module_string( c_field_display_name_Alignment );
   if( ( id_or_name == c_field_id_Allow_Anonymous_Access ) || ( id_or_name == c_field_name_Allow_Anonymous_Access ) )
      display_name = get_module_string( c_field_display_name_Allow_Anonymous_Access );
   if( ( id_or_name == c_field_id_Change_Permission ) || ( id_or_name == c_field_name_Change_Permission ) )
      display_name = get_module_string( c_field_display_name_Change_Permission );
   if( ( id_or_name == c_field_id_Change_Restriction ) || ( id_or_name == c_field_name_Change_Restriction ) )
      display_name = get_module_string( c_field_display_name_Change_Restriction );
   if( ( id_or_name == c_field_id_Change_Scope ) || ( id_or_name == c_field_name_Change_Scope ) )
      display_name = get_module_string( c_field_display_name_Change_Scope );
   if( ( id_or_name == c_field_id_Child_List_Extra_Option ) || ( id_or_name == c_field_name_Child_List_Extra_Option ) )
      display_name = get_module_string( c_field_display_name_Child_List_Extra_Option );
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
      display_name = get_module_string( c_field_display_name_Class );
   if( ( id_or_name == c_field_id_Date_Precision_Option ) || ( id_or_name == c_field_name_Date_Precision_Option ) )
      display_name = get_module_string( c_field_display_name_Date_Precision_Option );
   if( ( id_or_name == c_field_id_Enum_Finishes_At ) || ( id_or_name == c_field_name_Enum_Finishes_At ) )
      display_name = get_module_string( c_field_display_name_Enum_Finishes_At );
   if( ( id_or_name == c_field_id_Enum_Starts_At ) || ( id_or_name == c_field_name_Enum_Starts_At ) )
      display_name = get_module_string( c_field_display_name_Enum_Starts_At );
   if( ( id_or_name == c_field_id_FK_Trigger_Behaviour ) || ( id_or_name == c_field_name_FK_Trigger_Behaviour ) )
      display_name = get_module_string( c_field_display_name_FK_Trigger_Behaviour );
   if( ( id_or_name == c_field_id_FK_Trigger_Option ) || ( id_or_name == c_field_name_FK_Trigger_Option ) )
      display_name = get_module_string( c_field_display_name_FK_Trigger_Option );
   if( ( id_or_name == c_field_id_Font_Size ) || ( id_or_name == c_field_name_Font_Size ) )
      display_name = get_module_string( c_field_display_name_Font_Size );
   if( ( id_or_name == c_field_id_Ignore_Manual_Links ) || ( id_or_name == c_field_name_Ignore_Manual_Links ) )
      display_name = get_module_string( c_field_display_name_Ignore_Manual_Links );
   if( ( id_or_name == c_field_id_Label_Source_Child ) || ( id_or_name == c_field_name_Label_Source_Child ) )
      display_name = get_module_string( c_field_display_name_Label_Source_Child );
   if( ( id_or_name == c_field_id_Link_Permission ) || ( id_or_name == c_field_name_Link_Permission ) )
      display_name = get_module_string( c_field_display_name_Link_Permission );
   if( ( id_or_name == c_field_id_Link_Restriction ) || ( id_or_name == c_field_name_Link_Restriction ) )
      display_name = get_module_string( c_field_display_name_Link_Restriction );
   if( ( id_or_name == c_field_id_Mandatory_Option ) || ( id_or_name == c_field_name_Mandatory_Option ) )
      display_name = get_module_string( c_field_display_name_Mandatory_Option );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
      display_name = get_module_string( c_field_display_name_Name );
   if( ( id_or_name == c_field_id_New_Source ) || ( id_or_name == c_field_name_New_Source ) )
      display_name = get_module_string( c_field_display_name_New_Source );
   if( ( id_or_name == c_field_id_New_Value ) || ( id_or_name == c_field_name_New_Value ) )
      display_name = get_module_string( c_field_display_name_New_Value );
   if( ( id_or_name == c_field_id_Order ) || ( id_or_name == c_field_name_Order ) )
      display_name = get_module_string( c_field_display_name_Order );
   if( ( id_or_name == c_field_id_Orientation ) || ( id_or_name == c_field_name_Orientation ) )
      display_name = get_module_string( c_field_display_name_Orientation );
   if( ( id_or_name == c_field_id_Restriction_Spec ) || ( id_or_name == c_field_name_Restriction_Spec ) )
      display_name = get_module_string( c_field_display_name_Restriction_Spec );
   if( ( id_or_name == c_field_id_Show_Hide_Start_Point ) || ( id_or_name == c_field_name_Show_Hide_Start_Point ) )
      display_name = get_module_string( c_field_display_name_Show_Hide_Start_Point );
   if( ( id_or_name == c_field_id_Sort_Manually ) || ( id_or_name == c_field_name_Sort_Manually ) )
      display_name = get_module_string( c_field_display_name_Sort_Manually );
   if( ( id_or_name == c_field_id_Source_Child ) || ( id_or_name == c_field_name_Source_Child ) )
      display_name = get_module_string( c_field_display_name_Source_Child );
   if( ( id_or_name == c_field_id_Source_Edit_Child ) || ( id_or_name == c_field_name_Source_Edit_Child ) )
      display_name = get_module_string( c_field_display_name_Source_Edit_Child );
   if( ( id_or_name == c_field_id_Source_Field ) || ( id_or_name == c_field_name_Source_Field ) )
      display_name = get_module_string( c_field_display_name_Source_Field );
   if( ( id_or_name == c_field_id_Source_Parent ) || ( id_or_name == c_field_name_Source_Parent ) )
      display_name = get_module_string( c_field_display_name_Source_Parent );
   if( ( id_or_name == c_field_id_Source_Parent_Class ) || ( id_or_name == c_field_name_Source_Parent_Class ) )
      display_name = get_module_string( c_field_display_name_Source_Parent_Class );
   if( ( id_or_name == c_field_id_Tab_Name ) || ( id_or_name == c_field_name_Tab_Name ) )
      display_name = get_module_string( c_field_display_name_Tab_Name );
   if( ( id_or_name == c_field_id_Trigger_Behaviour ) || ( id_or_name == c_field_name_Trigger_Behaviour ) )
      display_name = get_module_string( c_field_display_name_Trigger_Behaviour );
   if( ( id_or_name == c_field_id_Trigger_For_State ) || ( id_or_name == c_field_name_Trigger_For_State ) )
      display_name = get_module_string( c_field_display_name_Trigger_For_State );
   if( ( id_or_name == c_field_id_Trigger_Option ) || ( id_or_name == c_field_name_Trigger_Option ) )
      display_name = get_module_string( c_field_display_name_Trigger_Option );
   if( ( id_or_name == c_field_id_Type ) || ( id_or_name == c_field_name_Type ) )
      display_name = get_module_string( c_field_display_name_Type );
   if( ( id_or_name == c_field_id_Use_Full_Height ) || ( id_or_name == c_field_name_Use_Full_Height ) )
      display_name = get_module_string( c_field_display_name_Use_Full_Height );
   if( ( id_or_name == c_field_id_Use_Full_Width ) || ( id_or_name == c_field_name_Use_Full_Width ) )
      display_name = get_module_string( c_field_display_name_Use_Full_Width );
   if( ( id_or_name == c_field_id_Use_Source_Parent ) || ( id_or_name == c_field_name_Use_Source_Parent ) )
      display_name = get_module_string( c_field_display_name_Use_Source_Parent );
   if( ( id_or_name == c_field_id_View ) || ( id_or_name == c_field_name_View ) )
      display_name = get_module_string( c_field_display_name_View );

   return display_name;
}

void Meta_View_Field::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_View_Field::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_View_Field::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_View_Field::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_View_Field::setup_foreign_key( Meta_Permission& o, const string& value )
{
   static_cast< Meta_Permission& >( o ).set_key( value );
}

void Meta_View_Field::setup_foreign_key( Meta_Class& o, const string& value )
{
   static_cast< Meta_Class& >( o ).set_key( value );
}

void Meta_View_Field::setup_foreign_key( Meta_Specification& o, const string& value )
{
   static_cast< Meta_Specification& >( o ).set_key( value );
}

void Meta_View_Field::setup_foreign_key( Meta_Field& o, const string& value )
{
   static_cast< Meta_Field& >( o ).set_key( value );
}

void Meta_View_Field::setup_foreign_key( Meta_View_Field_Type& o, const string& value )
{
   static_cast< Meta_View_Field_Type& >( o ).set_key( value );
}

void Meta_View_Field::setup_foreign_key( Meta_View& o, const string& value )
{
   static_cast< Meta_View& >( o ).set_key( value );
}

void Meta_View_Field::setup_graph_parent(
 Meta_Permission& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Permission& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Permission& >( o ).set_key( init_value );
}

void Meta_View_Field::setup_graph_parent(
 Meta_Class& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Class& >( o ).set_key( init_value );
}

void Meta_View_Field::setup_graph_parent(
 Meta_Specification& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Specification& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Specification& >( o ).set_key( init_value );
}

void Meta_View_Field::setup_graph_parent(
 Meta_Field& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Field& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Field& >( o ).set_key( init_value );
}

void Meta_View_Field::setup_graph_parent(
 Meta_View_Field_Type& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_View_Field_Type& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_View_Field_Type& >( o ).set_key( init_value );
}

void Meta_View_Field::setup_graph_parent(
 Meta_View& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_View& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_View& >( o ).set_key( init_value );
}

size_t Meta_View_Field::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_View_Field::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_View_Field::get_num_foreign_key_children( bool is_internal ) const
{
   size_t rc = 0;

   if( !is_internal )
   {
      g_external_aliases_lookup.clear( );

      for( external_aliases_const_iterator
       eaci = g_external_aliases.begin( ), end = g_external_aliases.end( ); eaci != end; ++eaci )
      {
         size_t num_extra = eaci->second->get_num_foreign_key_children( true );

         if( num_extra )
         {
            eaci->second->set_key( get_key( ) );
            eaci->second->copy_all_field_values( *this );

            g_external_aliases_lookup.insert( make_pair( rc, eaci->second ) );

            rc += num_extra;
         }
      }
   }

   set_total_child_relationships( rc );

   return rc;
}

class_base* Meta_View_Field::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   ( void )child_num;
   ( void )next_child_field;
   ( void )op;

   return p_class_base;
}

void Meta_View_Field::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_View_Field::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_View_Field::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_View_Field::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_View_Field::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_View_Field::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_View_Field::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_View_Field::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "view_field";

   return get_module_string( key );
}

string Meta_View_Field::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_View_Field::get_create_instance_info( ) const
{
   return "";
}

string Meta_View_Field::get_update_instance_info( ) const
{
   return "";
}

string Meta_View_Field::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_View_Field::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );
   else if( procedure_id == "120420" ) // i.e. Move_Down
      retval = "";
   else if( procedure_id == "120410" ) // i.e. Move_Up
      retval = "";

   return retval;
}

bool Meta_View_Field::get_is_alias( ) const
{
   return false;
}

void Meta_View_Field::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_View_Field::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_View_Field::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == c_field_id_Access_Permission ) || ( sub_context == c_field_name_Access_Permission ) )
      p_class_base = &Access_Permission( );
   else if( ( sub_context == c_field_id_Change_Permission ) || ( sub_context == c_field_name_Change_Permission ) )
      p_class_base = &Change_Permission( );
   else if( ( sub_context == c_field_id_Class ) || ( sub_context == c_field_name_Class ) )
      p_class_base = &Class( );
   else if( ( sub_context == c_field_id_Link_Permission ) || ( sub_context == c_field_name_Link_Permission ) )
      p_class_base = &Link_Permission( );
   else if( ( sub_context == c_field_id_Restriction_Spec ) || ( sub_context == c_field_name_Restriction_Spec ) )
      p_class_base = &Restriction_Spec( );
   else if( ( sub_context == c_field_id_Source_Child ) || ( sub_context == c_field_name_Source_Child ) )
      p_class_base = &Source_Child( );
   else if( ( sub_context == c_field_id_Source_Edit_Child ) || ( sub_context == c_field_name_Source_Edit_Child ) )
      p_class_base = &Source_Edit_Child( );
   else if( ( sub_context == c_field_id_Source_Field ) || ( sub_context == c_field_name_Source_Field ) )
      p_class_base = &Source_Field( );
   else if( ( sub_context == c_field_id_Source_Parent ) || ( sub_context == c_field_name_Source_Parent ) )
      p_class_base = &Source_Parent( );
   else if( ( sub_context == c_field_id_Source_Parent_Class ) || ( sub_context == c_field_name_Source_Parent_Class ) )
      p_class_base = &Source_Parent_Class( );
   else if( ( sub_context == c_field_id_Type ) || ( sub_context == c_field_name_Type ) )
      p_class_base = &Type( );
   else if( ( sub_context == c_field_id_View ) || ( sub_context == c_field_name_View ) )
      p_class_base = &View( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_View_Field::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Access_Permission" );
   names.push_back( "C_Access_Restriction" );
   names.push_back( "C_Access_Scope" );
   names.push_back( "C_Alignment" );
   names.push_back( "C_Allow_Anonymous_Access" );
   names.push_back( "C_Change_Permission" );
   names.push_back( "C_Change_Restriction" );
   names.push_back( "C_Change_Scope" );
   names.push_back( "C_Child_List_Extra_Option" );
   names.push_back( "C_Class" );
   names.push_back( "C_Date_Precision_Option" );
   names.push_back( "C_Enum_Finishes_At" );
   names.push_back( "C_Enum_Starts_At" );
   names.push_back( "C_FK_Trigger_Behaviour" );
   names.push_back( "C_FK_Trigger_Option" );
   names.push_back( "C_Font_Size" );
   names.push_back( "C_Ignore_Manual_Links" );
   names.push_back( "C_Label_Source_Child" );
   names.push_back( "C_Link_Permission" );
   names.push_back( "C_Link_Restriction" );
   names.push_back( "C_Mandatory_Option" );
   names.push_back( "C_New_Source" );
   names.push_back( "C_New_Value" );
   names.push_back( "C_Order" );
   names.push_back( "C_Orientation" );
   names.push_back( "C_Restriction_Spec" );
   names.push_back( "C_Show_Hide_Start_Point" );
   names.push_back( "C_Sort_Manually" );
   names.push_back( "C_Source_Child" );
   names.push_back( "C_Source_Edit_Child" );
   names.push_back( "C_Source_Field" );
   names.push_back( "C_Source_Parent" );
   names.push_back( "C_Source_Parent_Class" );
   names.push_back( "C_Tab_Name" );
   names.push_back( "C_Trigger_Behaviour" );
   names.push_back( "C_Trigger_For_State" );
   names.push_back( "C_Trigger_Option" );
   names.push_back( "C_Type" );
   names.push_back( "C_Use_Full_Height" );
   names.push_back( "C_Use_Full_Width" );
   names.push_back( "C_Use_Source_Parent" );
   names.push_back( "C_View" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_View_Field::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Access_Permission( ) ) ) );
   values.push_back( to_string( Access_Restriction( ) ) );
   values.push_back( to_string( Access_Scope( ) ) );
   values.push_back( to_string( Alignment( ) ) );
   values.push_back( to_string( Allow_Anonymous_Access( ) ) );
   values.push_back( sql_quote( to_string( Change_Permission( ) ) ) );
   values.push_back( to_string( Change_Restriction( ) ) );
   values.push_back( to_string( Change_Scope( ) ) );
   values.push_back( to_string( Child_List_Extra_Option( ) ) );
   values.push_back( sql_quote( to_string( Class( ) ) ) );
   values.push_back( to_string( Date_Precision_Option( ) ) );
   values.push_back( to_string( Enum_Finishes_At( ) ) );
   values.push_back( to_string( Enum_Starts_At( ) ) );
   values.push_back( to_string( FK_Trigger_Behaviour( ) ) );
   values.push_back( to_string( FK_Trigger_Option( ) ) );
   values.push_back( to_string( Font_Size( ) ) );
   values.push_back( to_string( Ignore_Manual_Links( ) ) );
   values.push_back( to_string( Label_Source_Child( ) ) );
   values.push_back( sql_quote( to_string( Link_Permission( ) ) ) );
   values.push_back( to_string( Link_Restriction( ) ) );
   values.push_back( to_string( Mandatory_Option( ) ) );
   values.push_back( to_string( New_Source( ) ) );
   values.push_back( sql_quote( to_string( New_Value( ) ) ) );
   values.push_back( sql_quote( to_string( Order( ) ) ) );
   values.push_back( to_string( Orientation( ) ) );
   values.push_back( sql_quote( to_string( Restriction_Spec( ) ) ) );
   values.push_back( to_string( Show_Hide_Start_Point( ) ) );
   values.push_back( to_string( Sort_Manually( ) ) );
   values.push_back( sql_quote( to_string( Source_Child( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Edit_Child( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Parent( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Parent_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Tab_Name( ) ) ) );
   values.push_back( to_string( Trigger_Behaviour( ) ) );
   values.push_back( to_string( Trigger_For_State( ) ) );
   values.push_back( to_string( Trigger_Option( ) ) );
   values.push_back( sql_quote( to_string( Type( ) ) ) );
   values.push_back( to_string( Use_Full_Height( ) ) );
   values.push_back( to_string( Use_Full_Width( ) ) );
   values.push_back( to_string( Use_Source_Parent( ) ) );
   values.push_back( sql_quote( to_string( View( ) ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_View_Field::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [(start field_from_search_replace)] 600385
   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Tab_Name" );

      if( ( use_transients && is_field_transient( e_field_id_Tab_Name ) )
       || ( !use_transients && !is_field_transient( e_field_id_Tab_Name ) ) )
         names.insert( "Tab_Name" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Source_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Field ) ) )
         names.insert( "Source_Field" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Source_Parent" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Parent ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Parent ) ) )
         names.insert( "Source_Parent" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Source_Child" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Child ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Child ) ) )
         names.insert( "Source_Child" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Source_Edit_Child" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Edit_Child ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Edit_Child ) ) )
         names.insert( "Source_Edit_Child" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Type" );

      if( ( use_transients && is_field_transient( e_field_id_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
         names.insert( "Type" );
   }
   // [(finish field_from_search_replace)] 600385

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_View_Field::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start modifier_field_value)] 600351
   dependents.insert( "View" ); // (for Is_Print_Version modifier)

   if( ( use_transients && is_field_transient( e_field_id_View ) )
    || ( !use_transients && !is_field_transient( e_field_id_View ) ) )
      names.insert( "View" );
   // [(finish modifier_field_value)] 600351

   // [(start move_up_and_down)] 600370
   dependents.insert( "Order" );

   if( ( use_transients && is_field_transient( e_field_id_Order ) )
    || ( !use_transients && !is_field_transient( e_field_id_Order ) ) )
      names.insert( "Order" );
   // [(finish move_up_and_down)] 600370

   // [(start modifier_field_value)] 600390
   dependents.insert( "Type" ); // (for Is_Key modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600390

   // [(start modifier_field_value)] 600391
   dependents.insert( "Type" ); // (for Is_Tab modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600391

   // [(start modifier_field_value)] 600392
   dependents.insert( "Type" ); // (for Is_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600392

   // [(start modifier_field_value)] 600393
   dependents.insert( "Use_Source_Parent" ); // (for Hide_FK_Fields modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Source_Parent ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Source_Parent ) ) )
      names.insert( "Use_Source_Parent" );
   // [(finish modifier_field_value)] 600393

   // [(start modifier_field_value)] 600394
   dependents.insert( "Use_Source_Parent" ); // (for Hide_Non_FK_Fields modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Source_Parent ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Source_Parent ) ) )
      names.insert( "Use_Source_Parent" );
   // [(finish modifier_field_value)] 600394

   // [(start modifier_field_value)] 600396
   dependents.insert( "Source_Field" ); // (for Is_Not_Date modifier)

   if( ( use_transients && is_field_transient( e_field_id_Source_Field ) )
    || ( !use_transients && !is_field_transient( e_field_id_Source_Field ) ) )
      names.insert( "Source_Field" );

   dependents.insert( "Use_Source_Parent" ); // (for Is_Not_Date modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Source_Parent ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Source_Parent ) ) )
      names.insert( "Use_Source_Parent" );
   // [(finish modifier_field_value)] 600396

   // [(start modifier_field_value)] 600397
   dependents.insert( "Source_Child" ); // (for Is_Not_Date modifier)

   if( ( use_transients && is_field_transient( e_field_id_Source_Child ) )
    || ( !use_transients && !is_field_transient( e_field_id_Source_Child ) ) )
      names.insert( "Source_Child" );

   dependents.insert( "Use_Source_Parent" ); // (for Is_Not_Date modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Source_Parent ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Source_Parent ) ) )
      names.insert( "Use_Source_Parent" );
   // [(finish modifier_field_value)] 600397

   // [(start modifier_field_null)] 600398
   dependents.insert( "Source_Field" ); // (for Is_Not_Enum modifier)

   if( ( use_transients && is_field_transient( e_field_id_Source_Field ) )
    || ( !use_transients && !is_field_transient( e_field_id_Source_Field ) ) )
      names.insert( "Source_Field" );
   // [(finish modifier_field_null)] 600398

   // [(start modifier_field_value)] 610394
   dependents.insert( "New_Source" ); // (for Hide_New_Value modifier)

   if( ( use_transients && is_field_transient( e_field_id_New_Source ) )
    || ( !use_transients && !is_field_transient( e_field_id_New_Source ) ) )
      names.insert( "New_Source" );
   // [(finish modifier_field_value)] 610394

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_View_Field::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_View_Field::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_View_Field::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_View_Field::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_View_Field::static_lock_class_id( )
{
   return "120100";
}

const char* Meta_View_Field::static_check_class_name( )
{
   return "View_Field";
}

const char* Meta_View_Field::static_persistence_extra( )
{
   return "";
}

bool Meta_View_Field::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_View_Field::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.120100" );
}

void Meta_View_Field::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "301920", "Access_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "120105", "Access_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "120119", "Access_Scope", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "120131", "Alignment", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "120129", "Allow_Anonymous_Access", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "301930", "Change_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "120106", "Change_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "120120", "Change_Scope", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "120127", "Child_List_Extra_Option", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "301905", "Class", "Meta_Class", true, "", "" ) );
   all_field_info.push_back( field_info( "120118", "Date_Precision_Option", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "120124", "Enum_Finishes_At", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "120123", "Enum_Starts_At", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "120112", "FK_Trigger_Behaviour", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "120111", "FK_Trigger_Option", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "120125", "Font_Size", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "120126", "Ignore_Manual_Links", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "120128", "Label_Source_Child", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "301915", "Link_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "120108", "Link_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "120114", "Mandatory_Option", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "120102", "Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "120109", "New_Source", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "120117", "New_Value", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "120101", "Order", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "120132", "Orientation", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "301980", "Restriction_Spec", "Meta_Specification", false, "", "" ) );
   all_field_info.push_back( field_info( "120116", "Show_Hide_Start_Point", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "120115", "Sort_Manually", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "301960", "Source_Child", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301965", "Source_Edit_Child", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301940", "Source_Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301950", "Source_Parent", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301970", "Source_Parent_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "120103", "Tab_Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "120110", "Trigger_Behaviour", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "120122", "Trigger_For_State", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "120113", "Trigger_Option", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "301910", "Type", "Meta_View_Field_Type", true, "", "" ) );
   all_field_info.push_back( field_info( "120130", "Use_Full_Height", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "120121", "Use_Full_Width", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "120107", "Use_Source_Parent", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "301900", "View", "Meta_View", true, "", "" ) );
}

void Meta_View_Field::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Access_Permission, make_pair( "Meta.120100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Change_Permission, make_pair( "Meta.120100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Class, make_pair( "Meta.120100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Link_Permission, make_pair( "Meta.120100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Restriction_Spec, make_pair( "Meta.120100", "Meta_Specification" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Child, make_pair( "Meta.120100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Edit_Child, make_pair( "Meta.120100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Field, make_pair( "Meta.120100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Parent, make_pair( "Meta.120100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Parent_Class, make_pair( "Meta.120100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Type, make_pair( "Meta.120100", "Meta_View_Field_Type" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_View, make_pair( "Meta.120100", "Meta_View" ) ) );
}

int Meta_View_Field::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_View_Field::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_View_Field::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_View_Field::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "301920";
      break;

      case 2:
      p_id = "120105";
      break;

      case 3:
      p_id = "120119";
      break;

      case 4:
      p_id = "120131";
      break;

      case 5:
      p_id = "120129";
      break;

      case 6:
      p_id = "301930";
      break;

      case 7:
      p_id = "120106";
      break;

      case 8:
      p_id = "120120";
      break;

      case 9:
      p_id = "120127";
      break;

      case 10:
      p_id = "301905";
      break;

      case 11:
      p_id = "120118";
      break;

      case 12:
      p_id = "120124";
      break;

      case 13:
      p_id = "120123";
      break;

      case 14:
      p_id = "120112";
      break;

      case 15:
      p_id = "120111";
      break;

      case 16:
      p_id = "120125";
      break;

      case 17:
      p_id = "120126";
      break;

      case 18:
      p_id = "120128";
      break;

      case 19:
      p_id = "301915";
      break;

      case 20:
      p_id = "120108";
      break;

      case 21:
      p_id = "120114";
      break;

      case 22:
      p_id = "120102";
      break;

      case 23:
      p_id = "120109";
      break;

      case 24:
      p_id = "120117";
      break;

      case 25:
      p_id = "120101";
      break;

      case 26:
      p_id = "120132";
      break;

      case 27:
      p_id = "301980";
      break;

      case 28:
      p_id = "120116";
      break;

      case 29:
      p_id = "120115";
      break;

      case 30:
      p_id = "301960";
      break;

      case 31:
      p_id = "301965";
      break;

      case 32:
      p_id = "301940";
      break;

      case 33:
      p_id = "301950";
      break;

      case 34:
      p_id = "301970";
      break;

      case 35:
      p_id = "120103";
      break;

      case 36:
      p_id = "120110";
      break;

      case 37:
      p_id = "120122";
      break;

      case 38:
      p_id = "120113";
      break;

      case 39:
      p_id = "301910";
      break;

      case 40:
      p_id = "120130";
      break;

      case 41:
      p_id = "120121";
      break;

      case 42:
      p_id = "120107";
      break;

      case 43:
      p_id = "301900";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class View_Field" );

   return p_id;
}

const char* Meta_View_Field::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Access_Permission";
      break;

      case 2:
      p_id = "Access_Restriction";
      break;

      case 3:
      p_id = "Access_Scope";
      break;

      case 4:
      p_id = "Alignment";
      break;

      case 5:
      p_id = "Allow_Anonymous_Access";
      break;

      case 6:
      p_id = "Change_Permission";
      break;

      case 7:
      p_id = "Change_Restriction";
      break;

      case 8:
      p_id = "Change_Scope";
      break;

      case 9:
      p_id = "Child_List_Extra_Option";
      break;

      case 10:
      p_id = "Class";
      break;

      case 11:
      p_id = "Date_Precision_Option";
      break;

      case 12:
      p_id = "Enum_Finishes_At";
      break;

      case 13:
      p_id = "Enum_Starts_At";
      break;

      case 14:
      p_id = "FK_Trigger_Behaviour";
      break;

      case 15:
      p_id = "FK_Trigger_Option";
      break;

      case 16:
      p_id = "Font_Size";
      break;

      case 17:
      p_id = "Ignore_Manual_Links";
      break;

      case 18:
      p_id = "Label_Source_Child";
      break;

      case 19:
      p_id = "Link_Permission";
      break;

      case 20:
      p_id = "Link_Restriction";
      break;

      case 21:
      p_id = "Mandatory_Option";
      break;

      case 22:
      p_id = "Name";
      break;

      case 23:
      p_id = "New_Source";
      break;

      case 24:
      p_id = "New_Value";
      break;

      case 25:
      p_id = "Order";
      break;

      case 26:
      p_id = "Orientation";
      break;

      case 27:
      p_id = "Restriction_Spec";
      break;

      case 28:
      p_id = "Show_Hide_Start_Point";
      break;

      case 29:
      p_id = "Sort_Manually";
      break;

      case 30:
      p_id = "Source_Child";
      break;

      case 31:
      p_id = "Source_Edit_Child";
      break;

      case 32:
      p_id = "Source_Field";
      break;

      case 33:
      p_id = "Source_Parent";
      break;

      case 34:
      p_id = "Source_Parent_Class";
      break;

      case 35:
      p_id = "Tab_Name";
      break;

      case 36:
      p_id = "Trigger_Behaviour";
      break;

      case 37:
      p_id = "Trigger_For_State";
      break;

      case 38:
      p_id = "Trigger_Option";
      break;

      case 39:
      p_id = "Type";
      break;

      case 40:
      p_id = "Use_Full_Height";
      break;

      case 41:
      p_id = "Use_Full_Width";
      break;

      case 42:
      p_id = "Use_Source_Parent";
      break;

      case 43:
      p_id = "View";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class View_Field" );

   return p_id;
}

int Meta_View_Field::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Access_Permission || field == c_field_name_Access_Permission )
      rc += 1;
   else if( field == c_field_id_Access_Restriction || field == c_field_name_Access_Restriction )
      rc += 2;
   else if( field == c_field_id_Access_Scope || field == c_field_name_Access_Scope )
      rc += 3;
   else if( field == c_field_id_Alignment || field == c_field_name_Alignment )
      rc += 4;
   else if( field == c_field_id_Allow_Anonymous_Access || field == c_field_name_Allow_Anonymous_Access )
      rc += 5;
   else if( field == c_field_id_Change_Permission || field == c_field_name_Change_Permission )
      rc += 6;
   else if( field == c_field_id_Change_Restriction || field == c_field_name_Change_Restriction )
      rc += 7;
   else if( field == c_field_id_Change_Scope || field == c_field_name_Change_Scope )
      rc += 8;
   else if( field == c_field_id_Child_List_Extra_Option || field == c_field_name_Child_List_Extra_Option )
      rc += 9;
   else if( field == c_field_id_Class || field == c_field_name_Class )
      rc += 10;
   else if( field == c_field_id_Date_Precision_Option || field == c_field_name_Date_Precision_Option )
      rc += 11;
   else if( field == c_field_id_Enum_Finishes_At || field == c_field_name_Enum_Finishes_At )
      rc += 12;
   else if( field == c_field_id_Enum_Starts_At || field == c_field_name_Enum_Starts_At )
      rc += 13;
   else if( field == c_field_id_FK_Trigger_Behaviour || field == c_field_name_FK_Trigger_Behaviour )
      rc += 14;
   else if( field == c_field_id_FK_Trigger_Option || field == c_field_name_FK_Trigger_Option )
      rc += 15;
   else if( field == c_field_id_Font_Size || field == c_field_name_Font_Size )
      rc += 16;
   else if( field == c_field_id_Ignore_Manual_Links || field == c_field_name_Ignore_Manual_Links )
      rc += 17;
   else if( field == c_field_id_Label_Source_Child || field == c_field_name_Label_Source_Child )
      rc += 18;
   else if( field == c_field_id_Link_Permission || field == c_field_name_Link_Permission )
      rc += 19;
   else if( field == c_field_id_Link_Restriction || field == c_field_name_Link_Restriction )
      rc += 20;
   else if( field == c_field_id_Mandatory_Option || field == c_field_name_Mandatory_Option )
      rc += 21;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 22;
   else if( field == c_field_id_New_Source || field == c_field_name_New_Source )
      rc += 23;
   else if( field == c_field_id_New_Value || field == c_field_name_New_Value )
      rc += 24;
   else if( field == c_field_id_Order || field == c_field_name_Order )
      rc += 25;
   else if( field == c_field_id_Orientation || field == c_field_name_Orientation )
      rc += 26;
   else if( field == c_field_id_Restriction_Spec || field == c_field_name_Restriction_Spec )
      rc += 27;
   else if( field == c_field_id_Show_Hide_Start_Point || field == c_field_name_Show_Hide_Start_Point )
      rc += 28;
   else if( field == c_field_id_Sort_Manually || field == c_field_name_Sort_Manually )
      rc += 29;
   else if( field == c_field_id_Source_Child || field == c_field_name_Source_Child )
      rc += 30;
   else if( field == c_field_id_Source_Edit_Child || field == c_field_name_Source_Edit_Child )
      rc += 31;
   else if( field == c_field_id_Source_Field || field == c_field_name_Source_Field )
      rc += 32;
   else if( field == c_field_id_Source_Parent || field == c_field_name_Source_Parent )
      rc += 33;
   else if( field == c_field_id_Source_Parent_Class || field == c_field_name_Source_Parent_Class )
      rc += 34;
   else if( field == c_field_id_Tab_Name || field == c_field_name_Tab_Name )
      rc += 35;
   else if( field == c_field_id_Trigger_Behaviour || field == c_field_name_Trigger_Behaviour )
      rc += 36;
   else if( field == c_field_id_Trigger_For_State || field == c_field_name_Trigger_For_State )
      rc += 37;
   else if( field == c_field_id_Trigger_Option || field == c_field_name_Trigger_Option )
      rc += 38;
   else if( field == c_field_id_Type || field == c_field_name_Type )
      rc += 39;
   else if( field == c_field_id_Use_Full_Height || field == c_field_name_Use_Full_Height )
      rc += 40;
   else if( field == c_field_id_Use_Full_Width || field == c_field_name_Use_Full_Width )
      rc += 41;
   else if( field == c_field_id_Use_Source_Parent || field == c_field_name_Use_Source_Parent )
      rc += 42;
   else if( field == c_field_id_View || field == c_field_name_View )
      rc += 43;

   return rc - 1;
}

procedure_info_container& Meta_View_Field::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "120420", procedure_info( "Move_Down" ) ) );
      procedures.insert( make_pair( "120410", procedure_info( "Move_Up" ) ) );
   }

   return procedures;
}

string Meta_View_Field::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(75),"
    "C_Ver_ SMALLINT UNSIGNED NOT NULL,"
    "C_Rev_ BIGINT UNSIGNED NOT NULL,"
    "C_Sec_ BIGINT UNSIGNED NOT NULL,"
    "C_Typ_ VARCHAR(16) NOT NULL,"
    "C_Access_Permission VARCHAR(75) NOT NULL,"
    "C_Access_Restriction INTEGER NOT NULL,"
    "C_Access_Scope INTEGER NOT NULL,"
    "C_Alignment INTEGER NOT NULL,"
    "C_Allow_Anonymous_Access INTEGER NOT NULL,"
    "C_Change_Permission VARCHAR(75) NOT NULL,"
    "C_Change_Restriction INTEGER NOT NULL,"
    "C_Change_Scope INTEGER NOT NULL,"
    "C_Child_List_Extra_Option INTEGER NOT NULL,"
    "C_Class VARCHAR(75) NOT NULL,"
    "C_Date_Precision_Option INTEGER NOT NULL,"
    "C_Enum_Finishes_At INTEGER NOT NULL,"
    "C_Enum_Starts_At INTEGER NOT NULL,"
    "C_FK_Trigger_Behaviour INTEGER NOT NULL,"
    "C_FK_Trigger_Option INTEGER NOT NULL,"
    "C_Font_Size INTEGER NOT NULL,"
    "C_Ignore_Manual_Links INTEGER NOT NULL,"
    "C_Label_Source_Child INTEGER NOT NULL,"
    "C_Link_Permission VARCHAR(75) NOT NULL,"
    "C_Link_Restriction INTEGER NOT NULL,"
    "C_Mandatory_Option INTEGER NOT NULL,"
    "C_New_Source INTEGER NOT NULL,"
    "C_New_Value VARCHAR(200) NOT NULL,"
    "C_Order VARCHAR(200) NOT NULL,"
    "C_Orientation INTEGER NOT NULL,"
    "C_Restriction_Spec VARCHAR(256) NOT NULL,"
    "C_Show_Hide_Start_Point INTEGER NOT NULL,"
    "C_Sort_Manually INTEGER NOT NULL,"
    "C_Source_Child VARCHAR(256) NOT NULL,"
    "C_Source_Edit_Child VARCHAR(256) NOT NULL,"
    "C_Source_Field VARCHAR(256) NOT NULL,"
    "C_Source_Parent VARCHAR(256) NOT NULL,"
    "C_Source_Parent_Class VARCHAR(256) NOT NULL,"
    "C_Tab_Name VARCHAR(256) NOT NULL,"
    "C_Trigger_Behaviour INTEGER NOT NULL,"
    "C_Trigger_For_State INTEGER NOT NULL,"
    "C_Trigger_Option INTEGER NOT NULL,"
    "C_Type VARCHAR(256) NOT NULL,"
    "C_Use_Full_Height INTEGER NOT NULL,"
    "C_Use_Full_Width INTEGER NOT NULL,"
    "C_Use_Source_Parent INTEGER NOT NULL,"
    "C_View VARCHAR(75) NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_View_Field::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_View_Field::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "enum_view_field_restrict_0", get_enum_string_view_field_restrict( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_field_restrict_1", get_enum_string_view_field_restrict( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_field_restrict_2", get_enum_string_view_field_restrict( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_field_restrict_3", get_enum_string_view_field_restrict( 3 ) ) );
   pairs.push_back( make_pair( "enum_view_field_restrict_4", get_enum_string_view_field_restrict( 4 ) ) );

   pairs.push_back( make_pair( "enum_view_field_access_scope_0", get_enum_string_view_field_access_scope( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_field_access_scope_1", get_enum_string_view_field_access_scope( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_field_access_scope_2", get_enum_string_view_field_access_scope( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_field_access_scope_3", get_enum_string_view_field_access_scope( 3 ) ) );
   pairs.push_back( make_pair( "enum_view_field_access_scope_4", get_enum_string_view_field_access_scope( 4 ) ) );
   pairs.push_back( make_pair( "enum_view_field_access_scope_5", get_enum_string_view_field_access_scope( 5 ) ) );

   pairs.push_back( make_pair( "enum_field_alignment_0", get_enum_string_field_alignment( 0 ) ) );
   pairs.push_back( make_pair( "enum_field_alignment_1", get_enum_string_field_alignment( 1 ) ) );
   pairs.push_back( make_pair( "enum_field_alignment_2", get_enum_string_field_alignment( 2 ) ) );
   pairs.push_back( make_pair( "enum_field_alignment_3", get_enum_string_field_alignment( 3 ) ) );
   pairs.push_back( make_pair( "enum_field_alignment_4", get_enum_string_field_alignment( 4 ) ) );

   pairs.push_back( make_pair( "enum_view_field_change_scope_0", get_enum_string_view_field_change_scope( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_field_change_scope_1", get_enum_string_view_field_change_scope( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_field_change_scope_2", get_enum_string_view_field_change_scope( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_field_change_scope_3", get_enum_string_view_field_change_scope( 3 ) ) );

   pairs.push_back( make_pair( "enum_view_field_child_list_extra_option_0", get_enum_string_view_field_child_list_extra_option( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_field_child_list_extra_option_1", get_enum_string_view_field_child_list_extra_option( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_field_child_list_extra_option_2", get_enum_string_view_field_child_list_extra_option( 2 ) ) );

   pairs.push_back( make_pair( "enum_view_field_date_precision_option_0", get_enum_string_view_field_date_precision_option( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_field_date_precision_option_1", get_enum_string_view_field_date_precision_option( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_field_date_precision_option_2", get_enum_string_view_field_date_precision_option( 2 ) ) );

   pairs.push_back( make_pair( "enum_view_field_enum_at_0", get_enum_string_view_field_enum_at( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_1", get_enum_string_view_field_enum_at( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_2", get_enum_string_view_field_enum_at( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_3", get_enum_string_view_field_enum_at( 3 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_4", get_enum_string_view_field_enum_at( 4 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_5", get_enum_string_view_field_enum_at( 5 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_6", get_enum_string_view_field_enum_at( 6 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_7", get_enum_string_view_field_enum_at( 7 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_8", get_enum_string_view_field_enum_at( 8 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_9", get_enum_string_view_field_enum_at( 9 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_10", get_enum_string_view_field_enum_at( 10 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_11", get_enum_string_view_field_enum_at( 11 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_12", get_enum_string_view_field_enum_at( 12 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_13", get_enum_string_view_field_enum_at( 13 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_14", get_enum_string_view_field_enum_at( 14 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_15", get_enum_string_view_field_enum_at( 15 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_16", get_enum_string_view_field_enum_at( 16 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_17", get_enum_string_view_field_enum_at( 17 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_18", get_enum_string_view_field_enum_at( 18 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_19", get_enum_string_view_field_enum_at( 19 ) ) );
   pairs.push_back( make_pair( "enum_view_field_enum_at_20", get_enum_string_view_field_enum_at( 20 ) ) );

   pairs.push_back( make_pair( "enum_view_field_fk_trigger_behaviour_0", get_enum_string_view_field_fk_trigger_behaviour( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_field_fk_trigger_behaviour_1", get_enum_string_view_field_fk_trigger_behaviour( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_field_fk_trigger_behaviour_2", get_enum_string_view_field_fk_trigger_behaviour( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_field_fk_trigger_behaviour_3", get_enum_string_view_field_fk_trigger_behaviour( 3 ) ) );

   pairs.push_back( make_pair( "enum_view_field_fk_trigger_option_0", get_enum_string_view_field_fk_trigger_option( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_field_fk_trigger_option_1", get_enum_string_view_field_fk_trigger_option( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_field_fk_trigger_option_2", get_enum_string_view_field_fk_trigger_option( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_field_fk_trigger_option_3", get_enum_string_view_field_fk_trigger_option( 3 ) ) );
   pairs.push_back( make_pair( "enum_view_field_fk_trigger_option_4", get_enum_string_view_field_fk_trigger_option( 4 ) ) );
   pairs.push_back( make_pair( "enum_view_field_fk_trigger_option_5", get_enum_string_view_field_fk_trigger_option( 5 ) ) );
   pairs.push_back( make_pair( "enum_view_field_fk_trigger_option_6", get_enum_string_view_field_fk_trigger_option( 6 ) ) );
   pairs.push_back( make_pair( "enum_view_field_fk_trigger_option_7", get_enum_string_view_field_fk_trigger_option( 7 ) ) );
   pairs.push_back( make_pair( "enum_view_field_fk_trigger_option_8", get_enum_string_view_field_fk_trigger_option( 8 ) ) );
   pairs.push_back( make_pair( "enum_view_field_fk_trigger_option_9", get_enum_string_view_field_fk_trigger_option( 9 ) ) );
   pairs.push_back( make_pair( "enum_view_field_fk_trigger_option_10", get_enum_string_view_field_fk_trigger_option( 10 ) ) );

   pairs.push_back( make_pair( "enum_font_size_0", get_enum_string_font_size( 0 ) ) );
   pairs.push_back( make_pair( "enum_font_size_1", get_enum_string_font_size( 1 ) ) );
   pairs.push_back( make_pair( "enum_font_size_2", get_enum_string_font_size( 2 ) ) );
   pairs.push_back( make_pair( "enum_font_size_5", get_enum_string_font_size( 5 ) ) );
   pairs.push_back( make_pair( "enum_font_size_6", get_enum_string_font_size( 6 ) ) );

   pairs.push_back( make_pair( "enum_label_source_child_0", get_enum_string_label_source_child( 0 ) ) );
   pairs.push_back( make_pair( "enum_label_source_child_1", get_enum_string_label_source_child( 1 ) ) );
   pairs.push_back( make_pair( "enum_label_source_child_2", get_enum_string_label_source_child( 2 ) ) );

   pairs.push_back( make_pair( "enum_view_field_link_restrict_0", get_enum_string_view_field_link_restrict( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_field_link_restrict_1", get_enum_string_view_field_link_restrict( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_field_link_restrict_2", get_enum_string_view_field_link_restrict( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_field_link_restrict_3", get_enum_string_view_field_link_restrict( 3 ) ) );
   pairs.push_back( make_pair( "enum_view_field_link_restrict_4", get_enum_string_view_field_link_restrict( 4 ) ) );

   pairs.push_back( make_pair( "enum_view_field_mandatory_option_0", get_enum_string_view_field_mandatory_option( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_field_mandatory_option_1", get_enum_string_view_field_mandatory_option( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_field_mandatory_option_2", get_enum_string_view_field_mandatory_option( 2 ) ) );

   pairs.push_back( make_pair( "enum_view_field_new_source_0", get_enum_string_view_field_new_source( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_field_new_source_1", get_enum_string_view_field_new_source( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_field_new_source_2", get_enum_string_view_field_new_source( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_field_new_source_3", get_enum_string_view_field_new_source( 3 ) ) );
   pairs.push_back( make_pair( "enum_view_field_new_source_4", get_enum_string_view_field_new_source( 4 ) ) );

   pairs.push_back( make_pair( "enum_orientation_0", get_enum_string_orientation( 0 ) ) );
   pairs.push_back( make_pair( "enum_orientation_1", get_enum_string_orientation( 1 ) ) );
   pairs.push_back( make_pair( "enum_orientation_2", get_enum_string_orientation( 2 ) ) );

   pairs.push_back( make_pair( "enum_view_field_trigger_behaviour_0", get_enum_string_view_field_trigger_behaviour( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_field_trigger_behaviour_1", get_enum_string_view_field_trigger_behaviour( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_field_trigger_behaviour_2", get_enum_string_view_field_trigger_behaviour( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_field_trigger_behaviour_3", get_enum_string_view_field_trigger_behaviour( 3 ) ) );

   pairs.push_back( make_pair( "enum_view_field_trigger_for_state_0", get_enum_string_view_field_trigger_for_state( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_field_trigger_for_state_1", get_enum_string_view_field_trigger_for_state( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_field_trigger_for_state_2", get_enum_string_view_field_trigger_for_state( 2 ) ) );

   pairs.push_back( make_pair( "enum_view_field_trigger_option_0", get_enum_string_view_field_trigger_option( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_field_trigger_option_1", get_enum_string_view_field_trigger_option( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_field_trigger_option_2", get_enum_string_view_field_trigger_option( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_field_trigger_option_3", get_enum_string_view_field_trigger_option( 3 ) ) );
   pairs.push_back( make_pair( "enum_view_field_trigger_option_4", get_enum_string_view_field_trigger_option( 4 ) ) );
   pairs.push_back( make_pair( "enum_view_field_trigger_option_5", get_enum_string_view_field_trigger_option( 5 ) ) );
   pairs.push_back( make_pair( "enum_view_field_trigger_option_6", get_enum_string_view_field_trigger_option( 6 ) ) );
   pairs.push_back( make_pair( "enum_view_field_trigger_option_7", get_enum_string_view_field_trigger_option( 7 ) ) );
   pairs.push_back( make_pair( "enum_view_field_trigger_option_8", get_enum_string_view_field_trigger_option( 8 ) ) );
   pairs.push_back( make_pair( "enum_view_field_trigger_option_9", get_enum_string_view_field_trigger_option( 9 ) ) );
   pairs.push_back( make_pair( "enum_view_field_trigger_option_10", get_enum_string_view_field_trigger_option( 10 ) ) );
}

void Meta_View_Field::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "View,Order", "string,string" ) );
}

void Meta_View_Field::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "View,Order" );
}

void Meta_View_Field::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_View,C_Order" );
}

void Meta_View_Field::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_View,C_Order" );
}

void Meta_View_Field::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_View_Field::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_View_Field::static_insert_external_alias( const string& module_and_class_id, Meta_View_Field* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_View_Field::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_View_Field::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   g_view_field_restrict_enum.insert( 0 );
   g_view_field_restrict_enum.insert( 1 );
   g_view_field_restrict_enum.insert( 2 );
   g_view_field_restrict_enum.insert( 3 );
   g_view_field_restrict_enum.insert( 4 );

   g_view_field_access_scope_enum.insert( 0 );
   g_view_field_access_scope_enum.insert( 1 );
   g_view_field_access_scope_enum.insert( 2 );
   g_view_field_access_scope_enum.insert( 3 );
   g_view_field_access_scope_enum.insert( 4 );
   g_view_field_access_scope_enum.insert( 5 );

   g_field_alignment_enum.insert( 0 );
   g_field_alignment_enum.insert( 1 );
   g_field_alignment_enum.insert( 2 );
   g_field_alignment_enum.insert( 3 );
   g_field_alignment_enum.insert( 4 );

   g_view_field_change_scope_enum.insert( 0 );
   g_view_field_change_scope_enum.insert( 1 );
   g_view_field_change_scope_enum.insert( 2 );
   g_view_field_change_scope_enum.insert( 3 );

   g_view_field_child_list_extra_option_enum.insert( 0 );
   g_view_field_child_list_extra_option_enum.insert( 1 );
   g_view_field_child_list_extra_option_enum.insert( 2 );

   g_view_field_date_precision_option_enum.insert( 0 );
   g_view_field_date_precision_option_enum.insert( 1 );
   g_view_field_date_precision_option_enum.insert( 2 );

   g_view_field_enum_at_enum.insert( 0 );
   g_view_field_enum_at_enum.insert( 1 );
   g_view_field_enum_at_enum.insert( 2 );
   g_view_field_enum_at_enum.insert( 3 );
   g_view_field_enum_at_enum.insert( 4 );
   g_view_field_enum_at_enum.insert( 5 );
   g_view_field_enum_at_enum.insert( 6 );
   g_view_field_enum_at_enum.insert( 7 );
   g_view_field_enum_at_enum.insert( 8 );
   g_view_field_enum_at_enum.insert( 9 );
   g_view_field_enum_at_enum.insert( 10 );
   g_view_field_enum_at_enum.insert( 11 );
   g_view_field_enum_at_enum.insert( 12 );
   g_view_field_enum_at_enum.insert( 13 );
   g_view_field_enum_at_enum.insert( 14 );
   g_view_field_enum_at_enum.insert( 15 );
   g_view_field_enum_at_enum.insert( 16 );
   g_view_field_enum_at_enum.insert( 17 );
   g_view_field_enum_at_enum.insert( 18 );
   g_view_field_enum_at_enum.insert( 19 );
   g_view_field_enum_at_enum.insert( 20 );

   g_view_field_fk_trigger_behaviour_enum.insert( 0 );
   g_view_field_fk_trigger_behaviour_enum.insert( 1 );
   g_view_field_fk_trigger_behaviour_enum.insert( 2 );
   g_view_field_fk_trigger_behaviour_enum.insert( 3 );

   g_view_field_fk_trigger_option_enum.insert( 0 );
   g_view_field_fk_trigger_option_enum.insert( 1 );
   g_view_field_fk_trigger_option_enum.insert( 2 );
   g_view_field_fk_trigger_option_enum.insert( 3 );
   g_view_field_fk_trigger_option_enum.insert( 4 );
   g_view_field_fk_trigger_option_enum.insert( 5 );
   g_view_field_fk_trigger_option_enum.insert( 6 );
   g_view_field_fk_trigger_option_enum.insert( 7 );
   g_view_field_fk_trigger_option_enum.insert( 8 );
   g_view_field_fk_trigger_option_enum.insert( 9 );
   g_view_field_fk_trigger_option_enum.insert( 10 );

   g_font_size_enum.insert( 0 );
   g_font_size_enum.insert( 1 );
   g_font_size_enum.insert( 2 );
   g_font_size_enum.insert( 5 );
   g_font_size_enum.insert( 6 );

   g_label_source_child_enum.insert( 0 );
   g_label_source_child_enum.insert( 1 );
   g_label_source_child_enum.insert( 2 );

   g_view_field_link_restrict_enum.insert( 0 );
   g_view_field_link_restrict_enum.insert( 1 );
   g_view_field_link_restrict_enum.insert( 2 );
   g_view_field_link_restrict_enum.insert( 3 );
   g_view_field_link_restrict_enum.insert( 4 );

   g_view_field_mandatory_option_enum.insert( 0 );
   g_view_field_mandatory_option_enum.insert( 1 );
   g_view_field_mandatory_option_enum.insert( 2 );

   g_view_field_new_source_enum.insert( 0 );
   g_view_field_new_source_enum.insert( 1 );
   g_view_field_new_source_enum.insert( 2 );
   g_view_field_new_source_enum.insert( 3 );
   g_view_field_new_source_enum.insert( 4 );

   g_orientation_enum.insert( 0 );
   g_orientation_enum.insert( 1 );
   g_orientation_enum.insert( 2 );

   g_view_field_trigger_behaviour_enum.insert( 0 );
   g_view_field_trigger_behaviour_enum.insert( 1 );
   g_view_field_trigger_behaviour_enum.insert( 2 );
   g_view_field_trigger_behaviour_enum.insert( 3 );

   g_view_field_trigger_for_state_enum.insert( 0 );
   g_view_field_trigger_for_state_enum.insert( 1 );
   g_view_field_trigger_for_state_enum.insert( 2 );

   g_view_field_trigger_option_enum.insert( 0 );
   g_view_field_trigger_option_enum.insert( 1 );
   g_view_field_trigger_option_enum.insert( 2 );
   g_view_field_trigger_option_enum.insert( 3 );
   g_view_field_trigger_option_enum.insert( 4 );
   g_view_field_trigger_option_enum.insert( 5 );
   g_view_field_trigger_option_enum.insert( 6 );
   g_view_field_trigger_option_enum.insert( 7 );
   g_view_field_trigger_option_enum.insert( 8 );
   g_view_field_trigger_option_enum.insert( 9 );
   g_view_field_trigger_option_enum.insert( 10 );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_View_Field::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
