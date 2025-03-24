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

#include "Meta_List_Field.h"

#include "Meta_Modifier.h"
#include "Meta_Permission.h"
#include "Meta_Class.h"
#include "Meta_Field.h"
#include "Meta_Relationship.h"
#include "Meta_List.h"
#include "Meta_Procedure.h"
#include "Meta_Specification.h"
#include "Meta_List_Field_Type.h"

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

// [<start includes>]
// [<finish includes>]

using namespace std;

// [<start namespaces>]
// [<finish namespaces>]

template< > inline string to_string( const Meta_Modifier& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Permission& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Class& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Field& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Relationship& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_List& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Procedure& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Specification& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_List_Field_Type& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Modifier& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Permission& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Class& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Field& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Relationship& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_List& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Procedure& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Specification& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_List_Field_Type& c, const string& s )
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

#include "Meta_List_Field.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Access_Parent_Modifier = "302199";
const char* const c_field_id_Access_Permission = "302130";
const char* const c_field_id_Access_Restriction = "124103";
const char* const c_field_id_Alignment = "124119";
const char* const c_field_id_Allow_Anonymous_Access = "124133";
const char* const c_field_id_Child_Rel_Child_Class = "302194";
const char* const c_field_id_Child_Rel_Source_Child = "302198";
const char* const c_field_id_Child_Rel_Source_Field = "302195";
const char* const c_field_id_Child_Rel_Source_Parent = "302196";
const char* const c_field_id_Child_Rel_Source_Parent_Class = "302197";
const char* const c_field_id_Child_Relationship = "302193";
const char* const c_field_id_Class = "302110";
const char* const c_field_id_Exact_Match_Only = "124128";
const char* const c_field_id_Exclude_In_Use_FK = "124107";
const char* const c_field_id_Font_Size = "124130";
const char* const c_field_id_Hide_If_Is_Changing = "124137";
const char* const c_field_id_Include_Key_Additions = "124122";
const char* const c_field_id_Label_Class = "124131";
const char* const c_field_id_Label_Source = "124120";
const char* const c_field_id_Label_Source_Child = "124129";
const char* const c_field_id_Link_Empty_Restriction = "124134";
const char* const c_field_id_Link_Permission = "302135";
const char* const c_field_id_Link_Restriction = "124108";
const char* const c_field_id_Link_Type = "124118";
const char* const c_field_id_List = "302100";
const char* const c_field_id_Name = "124102";
const char* const c_field_id_Non_Instance_Procedure = "124127";
const char* const c_field_id_Notes_Truncation = "124125";
const char* const c_field_id_Omit_Versions = "124110";
const char* const c_field_id_Order = "124101";
const char* const c_field_id_Orientation = "124136";
const char* const c_field_id_Parent_Class = "302115";
const char* const c_field_id_Print_Type = "124115";
const char* const c_field_id_Procedure = "302190";
const char* const c_field_id_Procedure_Args = "124109";
const char* const c_field_id_Restriction_Field = "302185";
const char* const c_field_id_Restriction_Spec = "302180";
const char* const c_field_id_Restriction_Value = "124106";
const char* const c_field_id_Retain_Selected_Rows = "124126";
const char* const c_field_id_Reverse_Order = "124111";
const char* const c_field_id_Search_Option_Limit = "124124";
const char* const c_field_id_Select_Key_Exclusions = "124117";
const char* const c_field_id_Sort_Manually = "124135";
const char* const c_field_id_Source_Child = "302160";
const char* const c_field_id_Source_Child_Class = "302175";
const char* const c_field_id_Source_Field = "302140";
const char* const c_field_id_Source_Grandchild = "302165";
const char* const c_field_id_Source_Parent = "302150";
const char* const c_field_id_Source_Parent_Class = "302170";
const char* const c_field_id_Switch_Type = "124105";
const char* const c_field_id_Trigger_Option = "124121";
const char* const c_field_id_Type = "302120";
const char* const c_field_id_Use_Child_Rel_Source_Parent = "124123";
const char* const c_field_id_Use_In_Text_Search_Title = "124116";
const char* const c_field_id_Use_Source_Parent = "124104";
const char* const c_field_id_Use_Type_Field = "302176";
const char* const c_field_id_View_Parent_Extra = "124132";

const char* const c_field_name_Access_Parent_Modifier = "Access_Parent_Modifier";
const char* const c_field_name_Access_Permission = "Access_Permission";
const char* const c_field_name_Access_Restriction = "Access_Restriction";
const char* const c_field_name_Alignment = "Alignment";
const char* const c_field_name_Allow_Anonymous_Access = "Allow_Anonymous_Access";
const char* const c_field_name_Child_Rel_Child_Class = "Child_Rel_Child_Class";
const char* const c_field_name_Child_Rel_Source_Child = "Child_Rel_Source_Child";
const char* const c_field_name_Child_Rel_Source_Field = "Child_Rel_Source_Field";
const char* const c_field_name_Child_Rel_Source_Parent = "Child_Rel_Source_Parent";
const char* const c_field_name_Child_Rel_Source_Parent_Class = "Child_Rel_Source_Parent_Class";
const char* const c_field_name_Child_Relationship = "Child_Relationship";
const char* const c_field_name_Class = "Class";
const char* const c_field_name_Exact_Match_Only = "Exact_Match_Only";
const char* const c_field_name_Exclude_In_Use_FK = "Exclude_In_Use_FK";
const char* const c_field_name_Font_Size = "Font_Size";
const char* const c_field_name_Hide_If_Is_Changing = "Hide_If_Is_Changing";
const char* const c_field_name_Include_Key_Additions = "Include_Key_Additions";
const char* const c_field_name_Label_Class = "Label_Class";
const char* const c_field_name_Label_Source = "Label_Source";
const char* const c_field_name_Label_Source_Child = "Label_Source_Child";
const char* const c_field_name_Link_Empty_Restriction = "Link_Empty_Restriction";
const char* const c_field_name_Link_Permission = "Link_Permission";
const char* const c_field_name_Link_Restriction = "Link_Restriction";
const char* const c_field_name_Link_Type = "Link_Type";
const char* const c_field_name_List = "List";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Non_Instance_Procedure = "Non_Instance_Procedure";
const char* const c_field_name_Notes_Truncation = "Notes_Truncation";
const char* const c_field_name_Omit_Versions = "Omit_Versions";
const char* const c_field_name_Order = "Order";
const char* const c_field_name_Orientation = "Orientation";
const char* const c_field_name_Parent_Class = "Parent_Class";
const char* const c_field_name_Print_Type = "Print_Type";
const char* const c_field_name_Procedure = "Procedure";
const char* const c_field_name_Procedure_Args = "Procedure_Args";
const char* const c_field_name_Restriction_Field = "Restriction_Field";
const char* const c_field_name_Restriction_Spec = "Restriction_Spec";
const char* const c_field_name_Restriction_Value = "Restriction_Value";
const char* const c_field_name_Retain_Selected_Rows = "Retain_Selected_Rows";
const char* const c_field_name_Reverse_Order = "Reverse_Order";
const char* const c_field_name_Search_Option_Limit = "Search_Option_Limit";
const char* const c_field_name_Select_Key_Exclusions = "Select_Key_Exclusions";
const char* const c_field_name_Sort_Manually = "Sort_Manually";
const char* const c_field_name_Source_Child = "Source_Child";
const char* const c_field_name_Source_Child_Class = "Source_Child_Class";
const char* const c_field_name_Source_Field = "Source_Field";
const char* const c_field_name_Source_Grandchild = "Source_Grandchild";
const char* const c_field_name_Source_Parent = "Source_Parent";
const char* const c_field_name_Source_Parent_Class = "Source_Parent_Class";
const char* const c_field_name_Switch_Type = "Switch_Type";
const char* const c_field_name_Trigger_Option = "Trigger_Option";
const char* const c_field_name_Type = "Type";
const char* const c_field_name_Use_Child_Rel_Source_Parent = "Use_Child_Rel_Source_Parent";
const char* const c_field_name_Use_In_Text_Search_Title = "Use_In_Text_Search_Title";
const char* const c_field_name_Use_Source_Parent = "Use_Source_Parent";
const char* const c_field_name_Use_Type_Field = "Use_Type_Field";
const char* const c_field_name_View_Parent_Extra = "View_Parent_Extra";

const char* const c_field_display_name_Access_Parent_Modifier = "field_list_field_access_parent_modifier";
const char* const c_field_display_name_Access_Permission = "field_list_field_access_permission";
const char* const c_field_display_name_Access_Restriction = "field_list_field_access_restriction";
const char* const c_field_display_name_Alignment = "field_list_field_alignment";
const char* const c_field_display_name_Allow_Anonymous_Access = "field_list_field_allow_anonymous_access";
const char* const c_field_display_name_Child_Rel_Child_Class = "field_list_field_child_rel_child_class";
const char* const c_field_display_name_Child_Rel_Source_Child = "field_list_field_child_rel_source_child";
const char* const c_field_display_name_Child_Rel_Source_Field = "field_list_field_child_rel_source_field";
const char* const c_field_display_name_Child_Rel_Source_Parent = "field_list_field_child_rel_source_parent";
const char* const c_field_display_name_Child_Rel_Source_Parent_Class = "field_list_field_child_rel_source_parent_class";
const char* const c_field_display_name_Child_Relationship = "field_list_field_child_relationship";
const char* const c_field_display_name_Class = "field_list_field_class";
const char* const c_field_display_name_Exact_Match_Only = "field_list_field_exact_match_only";
const char* const c_field_display_name_Exclude_In_Use_FK = "field_list_field_exclude_in_use_fk";
const char* const c_field_display_name_Font_Size = "field_list_field_font_size";
const char* const c_field_display_name_Hide_If_Is_Changing = "field_list_field_hide_if_is_changing";
const char* const c_field_display_name_Include_Key_Additions = "field_list_field_include_key_additions";
const char* const c_field_display_name_Label_Class = "field_list_field_label_class";
const char* const c_field_display_name_Label_Source = "field_list_field_label_source";
const char* const c_field_display_name_Label_Source_Child = "field_list_field_label_source_child";
const char* const c_field_display_name_Link_Empty_Restriction = "field_list_field_link_empty_restriction";
const char* const c_field_display_name_Link_Permission = "field_list_field_link_permission";
const char* const c_field_display_name_Link_Restriction = "field_list_field_link_restriction";
const char* const c_field_display_name_Link_Type = "field_list_field_link_type";
const char* const c_field_display_name_List = "field_list_field_list";
const char* const c_field_display_name_Name = "field_list_field_name";
const char* const c_field_display_name_Non_Instance_Procedure = "field_list_field_non_instance_procedure";
const char* const c_field_display_name_Notes_Truncation = "field_list_field_notes_truncation";
const char* const c_field_display_name_Omit_Versions = "field_list_field_omit_versions";
const char* const c_field_display_name_Order = "field_list_field_order";
const char* const c_field_display_name_Orientation = "field_list_field_orientation";
const char* const c_field_display_name_Parent_Class = "field_list_field_parent_class";
const char* const c_field_display_name_Print_Type = "field_list_field_print_type";
const char* const c_field_display_name_Procedure = "field_list_field_procedure";
const char* const c_field_display_name_Procedure_Args = "field_list_field_procedure_args";
const char* const c_field_display_name_Restriction_Field = "field_list_field_restriction_field";
const char* const c_field_display_name_Restriction_Spec = "field_list_field_restriction_spec";
const char* const c_field_display_name_Restriction_Value = "field_list_field_restriction_value";
const char* const c_field_display_name_Retain_Selected_Rows = "field_list_field_retain_selected_rows";
const char* const c_field_display_name_Reverse_Order = "field_list_field_reverse_order";
const char* const c_field_display_name_Search_Option_Limit = "field_list_field_search_option_limit";
const char* const c_field_display_name_Select_Key_Exclusions = "field_list_field_select_key_exclusions";
const char* const c_field_display_name_Sort_Manually = "field_list_field_sort_manually";
const char* const c_field_display_name_Source_Child = "field_list_field_source_child";
const char* const c_field_display_name_Source_Child_Class = "field_list_field_source_child_class";
const char* const c_field_display_name_Source_Field = "field_list_field_source_field";
const char* const c_field_display_name_Source_Grandchild = "field_list_field_source_grandchild";
const char* const c_field_display_name_Source_Parent = "field_list_field_source_parent";
const char* const c_field_display_name_Source_Parent_Class = "field_list_field_source_parent_class";
const char* const c_field_display_name_Switch_Type = "field_list_field_switch_type";
const char* const c_field_display_name_Trigger_Option = "field_list_field_trigger_option";
const char* const c_field_display_name_Type = "field_list_field_type";
const char* const c_field_display_name_Use_Child_Rel_Source_Parent = "field_list_field_use_child_rel_source_parent";
const char* const c_field_display_name_Use_In_Text_Search_Title = "field_list_field_use_in_text_search_title";
const char* const c_field_display_name_Use_Source_Parent = "field_list_field_use_source_parent";
const char* const c_field_display_name_Use_Type_Field = "field_list_field_use_type_field";
const char* const c_field_display_name_View_Parent_Extra = "field_list_field_view_parent_extra";

const int c_num_fields = 57;

const char* const c_all_sorted_field_ids[ ] =
{
   "124101",
   "124102",
   "124103",
   "124104",
   "124105",
   "124106",
   "124107",
   "124108",
   "124109",
   "124110",
   "124111",
   "124115",
   "124116",
   "124117",
   "124118",
   "124119",
   "124120",
   "124121",
   "124122",
   "124123",
   "124124",
   "124125",
   "124126",
   "124127",
   "124128",
   "124129",
   "124130",
   "124131",
   "124132",
   "124133",
   "124134",
   "124135",
   "124136",
   "124137",
   "302100",
   "302110",
   "302115",
   "302120",
   "302130",
   "302135",
   "302140",
   "302150",
   "302160",
   "302165",
   "302170",
   "302175",
   "302176",
   "302180",
   "302185",
   "302190",
   "302193",
   "302194",
   "302195",
   "302196",
   "302197",
   "302198",
   "302199"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Access_Parent_Modifier",
   "Access_Permission",
   "Access_Restriction",
   "Alignment",
   "Allow_Anonymous_Access",
   "Child_Rel_Child_Class",
   "Child_Rel_Source_Child",
   "Child_Rel_Source_Field",
   "Child_Rel_Source_Parent",
   "Child_Rel_Source_Parent_Class",
   "Child_Relationship",
   "Class",
   "Exact_Match_Only",
   "Exclude_In_Use_FK",
   "Font_Size",
   "Hide_If_Is_Changing",
   "Include_Key_Additions",
   "Label_Class",
   "Label_Source",
   "Label_Source_Child",
   "Link_Empty_Restriction",
   "Link_Permission",
   "Link_Restriction",
   "Link_Type",
   "List",
   "Name",
   "Non_Instance_Procedure",
   "Notes_Truncation",
   "Omit_Versions",
   "Order",
   "Orientation",
   "Parent_Class",
   "Print_Type",
   "Procedure",
   "Procedure_Args",
   "Restriction_Field",
   "Restriction_Spec",
   "Restriction_Value",
   "Retain_Selected_Rows",
   "Reverse_Order",
   "Search_Option_Limit",
   "Select_Key_Exclusions",
   "Sort_Manually",
   "Source_Child",
   "Source_Child_Class",
   "Source_Field",
   "Source_Grandchild",
   "Source_Parent",
   "Source_Parent_Class",
   "Switch_Type",
   "Trigger_Option",
   "Type",
   "Use_Child_Rel_Source_Parent",
   "Use_In_Text_Search_Title",
   "Use_Source_Parent",
   "Use_Type_Field",
   "View_Parent_Extra"
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
   "124102"
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

const char* const c_procedure_id_Move_Down = "124420";
const char* const c_procedure_id_Move_Up = "124410";

const uint64_t c_modifier_Hide_Child_Rel_FK_Fields = UINT64_C( 0x100 );
const uint64_t c_modifier_Hide_Child_Rel_Non_FK_Field = UINT64_C( 0x200 );
const uint64_t c_modifier_Hide_Child_Rel_Select_Specifics = UINT64_C( 0x400 );
const uint64_t c_modifier_Hide_Exclude_In_Use_FK = UINT64_C( 0x800 );
const uint64_t c_modifier_Hide_FK_Fields = UINT64_C( 0x1000 );
const uint64_t c_modifier_Hide_Fields_For_Trivial_Field = UINT64_C( 0x2000 );
const uint64_t c_modifier_Hide_For_Child_Rel_Select = UINT64_C( 0x4000 );
const uint64_t c_modifier_Hide_For_Restriction_Field = UINT64_C( 0x8000 );
const uint64_t c_modifier_Hide_Link_Empty_Restriction = UINT64_C( 0x10000 );
const uint64_t c_modifier_Hide_Link_Permission_Field = UINT64_C( 0x20000 );
const uint64_t c_modifier_Hide_Link_Specifics = UINT64_C( 0x40000 );
const uint64_t c_modifier_Hide_Link_Type = UINT64_C( 0x80000 );
const uint64_t c_modifier_Hide_Non_FK_Field = UINT64_C( 0x100000 );
const uint64_t c_modifier_Hide_Non_Procedure_Fields = UINT64_C( 0x200000 );
const uint64_t c_modifier_Hide_Non_Simple_Fields = UINT64_C( 0x400000 );
const uint64_t c_modifier_Hide_Procedure_Fields = UINT64_C( 0x800000 );
const uint64_t c_modifier_Hide_Restriction_Field = UINT64_C( 0x1000000 );
const uint64_t c_modifier_Hide_Restriction_Spec = UINT64_C( 0x2000000 );
const uint64_t c_modifier_Hide_Restriction_Value = UINT64_C( 0x4000000 );
const uint64_t c_modifier_Hide_Search_Option_Limit = UINT64_C( 0x8000000 );
const uint64_t c_modifier_Hide_Select_Specifics = UINT64_C( 0x10000000 );
const uint64_t c_modifier_Hide_Sort_Manually = UINT64_C( 0x20000000 );
const uint64_t c_modifier_Hide_Switch_Type = UINT64_C( 0x40000000 );
const uint64_t c_modifier_Hide_View_Parent_Extra = UINT64_C( 0x80000000 );
const uint64_t c_modifier_Is_File_Or_Image = UINT64_C( 0x100000000 );
const uint64_t c_modifier_Is_Non_Instance_Procedure = UINT64_C( 0x200000000 );
const uint64_t c_modifier_Is_Not_Image = UINT64_C( 0x400000000 );
const uint64_t c_modifier_Is_Not_Restrict_Search = UINT64_C( 0x800000000 );
const uint64_t c_modifier_Is_Not_View_Child = UINT64_C( 0x1000000000 );
const uint64_t c_modifier_Is_Restrict_Search = UINT64_C( 0x2000000000 );
const uint64_t c_modifier_Protect_Access = UINT64_C( 0x4000000000 );

domain_string_max_size< 100 > g_Include_Key_Additions_domain;
domain_string_max_size< 100 > g_Name_domain;
domain_string_max_size< 100 > g_Procedure_Args_domain;
domain_string_max_size< 100 > g_Restriction_Value_domain;
domain_string_max_size< 100 > g_Select_Key_Exclusions_domain;

string g_group_field_name;
string g_level_field_name;
string g_order_field_name( "Order" );
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_List_Field* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_List_Field* > external_aliases_lookup_container;
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

string g_default_Access_Parent_Modifier = string( );
string g_default_Access_Permission = string( );
int g_default_Access_Restriction = int( 0 );
int g_default_Alignment = int( 0 );
bool g_default_Allow_Anonymous_Access = bool( 1 );
string g_default_Child_Rel_Child_Class = string( );
string g_default_Child_Rel_Source_Child = string( );
string g_default_Child_Rel_Source_Field = string( );
string g_default_Child_Rel_Source_Parent = string( );
string g_default_Child_Rel_Source_Parent_Class = string( );
string g_default_Child_Relationship = string( );
string g_default_Class = string( );
bool g_default_Exact_Match_Only = bool( 0 );
bool g_default_Exclude_In_Use_FK = bool( 0 );
int g_default_Font_Size = int( 0 );
bool g_default_Hide_If_Is_Changing = bool( 0 );
string g_default_Include_Key_Additions = string( );
int g_default_Label_Class = int( 0 );
int g_default_Label_Source = int( 0 );
int g_default_Label_Source_Child = int( 0 );
int g_default_Link_Empty_Restriction = int( 0 );
string g_default_Link_Permission = string( );
int g_default_Link_Restriction = int( 0 );
int g_default_Link_Type = int( 0 );
string g_default_List = string( );
string g_default_Name = string( );
bool g_default_Non_Instance_Procedure = bool( 0 );
int g_default_Notes_Truncation = int( 0 );
bool g_default_Omit_Versions = bool( 0 );
string g_default_Order = string( );
int g_default_Orientation = int( 0 );
string g_default_Parent_Class = string( );
int g_default_Print_Type = int( 0 );
string g_default_Procedure = string( );
string g_default_Procedure_Args = string( );
string g_default_Restriction_Field = string( );
string g_default_Restriction_Spec = string( );
string g_default_Restriction_Value = string( );
bool g_default_Retain_Selected_Rows = bool( 1 );
bool g_default_Reverse_Order = bool( 0 );
int g_default_Search_Option_Limit = int( 0 );
string g_default_Select_Key_Exclusions = string( );
bool g_default_Sort_Manually = bool( 0 );
string g_default_Source_Child = string( );
string g_default_Source_Child_Class = string( );
string g_default_Source_Field = string( );
string g_default_Source_Grandchild = string( );
string g_default_Source_Parent = string( );
string g_default_Source_Parent_Class = string( );
int g_default_Switch_Type = int( 0 );
int g_default_Trigger_Option = int( 0 );
string g_default_Type = string( );
bool g_default_Use_Child_Rel_Source_Parent = bool( 0 );
bool g_default_Use_In_Text_Search_Title = bool( 0 );
bool g_default_Use_Source_Parent = bool( 0 );
string g_default_Use_Type_Field = string( );
int g_default_View_Parent_Extra = int( 0 );

set< int > g_list_field_restrict_enum;
set< int > g_field_alignment_enum;
set< int > g_font_size_enum;
set< int > g_list_field_label_class_enum;
set< int > g_list_field_label_source_enum;
set< int > g_label_source_child_enum;
set< int > g_list_field_link_restrict_enum;
set< int > g_list_field_link_type_enum;
set< int > g_list_field_notes_truncation_enum;
set< int > g_orientation_enum;
set< int > g_list_field_print_type_enum;
set< int > g_list_search_opt_limit_enum;
set< int > g_list_field_switch_type_enum;
set< int > g_list_field_trigger_option_enum;
set< int > g_list_field_view_parent_extra_enum;

const int c_enum_list_field_restrict_none( 0 );
const int c_enum_list_field_restrict_owner_only( 1 );
const int c_enum_list_field_restrict_admin_only( 2 );
const int c_enum_list_field_restrict_admin_owner( 3 );
const int c_enum_list_field_restrict_denied_always( 4 );

string get_enum_string_list_field_restrict( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_field_restrict" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_field_restrict_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_field_restrict_owner_only";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_list_field_restrict_admin_only";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_list_field_restrict_admin_owner";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_list_field_restrict_denied_always";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_field_restrict" );

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

const int c_enum_list_field_label_class_child( 0 );
const int c_enum_list_field_label_class_parent( 1 );

string get_enum_string_list_field_label_class( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_field_label_class" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_field_label_class_child";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_field_label_class_parent";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_field_label_class" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_list_field_label_source_field( 0 );
const int c_enum_list_field_label_source_list( 1 );
const int c_enum_list_field_label_source_omit_label( 2 );

string get_enum_string_list_field_label_source( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_field_label_source" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_field_label_source_field";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_field_label_source_list";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_list_field_label_source_omit_label";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_field_label_source" );

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

const int c_enum_list_field_link_restrict_none( 0 );
const int c_enum_list_field_link_restrict_owner_only( 1 );
const int c_enum_list_field_link_restrict_admin_only( 2 );
const int c_enum_list_field_link_restrict_admin_owner( 3 );
const int c_enum_list_field_link_restrict_denied_always( 4 );

string get_enum_string_list_field_link_restrict( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_field_link_restrict" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_field_link_restrict_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_field_link_restrict_owner_only";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_list_field_link_restrict_admin_only";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_list_field_link_restrict_admin_owner";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_list_field_link_restrict_denied_always";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_field_link_restrict" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_list_field_link_type_foreign( 0 );
const int c_enum_list_field_link_type_non_foreign( 1 );

string get_enum_string_list_field_link_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_field_link_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_field_link_type_foreign";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_field_link_type_non_foreign";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_field_link_type" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_list_field_notes_truncation_none( 1 );
const int c_enum_list_field_notes_truncation_standard( 0 );
const int c_enum_list_field_notes_truncation_30( 30 );
const int c_enum_list_field_notes_truncation_40( 40 );
const int c_enum_list_field_notes_truncation_50( 50 );
const int c_enum_list_field_notes_truncation_80( 80 );
const int c_enum_list_field_notes_truncation_100( 100 );

string get_enum_string_list_field_notes_truncation( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_field_notes_truncation" );
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_field_notes_truncation_none";
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_field_notes_truncation_standard";
   else if( to_string( val ) == to_string( "30" ) )
      string_name = "enum_list_field_notes_truncation_30";
   else if( to_string( val ) == to_string( "40" ) )
      string_name = "enum_list_field_notes_truncation_40";
   else if( to_string( val ) == to_string( "50" ) )
      string_name = "enum_list_field_notes_truncation_50";
   else if( to_string( val ) == to_string( "80" ) )
      string_name = "enum_list_field_notes_truncation_80";
   else if( to_string( val ) == to_string( "100" ) )
      string_name = "enum_list_field_notes_truncation_100";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_field_notes_truncation" );

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

const int c_enum_list_field_print_type_standard( 0 );
const int c_enum_list_field_print_type_non_print( 1 );
const int c_enum_list_field_print_type_print_only( 2 );
const int c_enum_list_field_print_type_print_total( 3 );
const int c_enum_list_field_print_type_print_summary( 4 );
const int c_enum_list_field_print_type_print_only_total( 5 );
const int c_enum_list_field_print_type_print_only_summary( 6 );

string get_enum_string_list_field_print_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_field_print_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_field_print_type_standard";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_field_print_type_non_print";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_list_field_print_type_print_only";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_list_field_print_type_print_total";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_list_field_print_type_print_summary";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_list_field_print_type_print_only_total";
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_list_field_print_type_print_only_summary";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_field_print_type" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_list_search_opt_limit_default( 0 );
const int c_enum_list_search_opt_limit_1( 1 );
const int c_enum_list_search_opt_limit_2( 2 );
const int c_enum_list_search_opt_limit_3( 3 );
const int c_enum_list_search_opt_limit_4( 4 );
const int c_enum_list_search_opt_limit_5( 5 );
const int c_enum_list_search_opt_limit_6( 6 );
const int c_enum_list_search_opt_limit_7( 7 );

string get_enum_string_list_search_opt_limit( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_search_opt_limit" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_search_opt_limit_default";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_search_opt_limit_1";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_list_search_opt_limit_2";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_list_search_opt_limit_3";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_list_search_opt_limit_4";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_list_search_opt_limit_5";
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_list_search_opt_limit_6";
   else if( to_string( val ) == to_string( "7" ) )
      string_name = "enum_list_search_opt_limit_7";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_search_opt_limit" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_list_field_switch_type_checked( 0 );
const int c_enum_list_field_switch_type_unchecked( 1 );
const int c_enum_list_field_switch_type_checked_reversing( 2 );
const int c_enum_list_field_switch_type_unchecked_reversing( 3 );

string get_enum_string_list_field_switch_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_field_switch_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_field_switch_type_checked";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_field_switch_type_unchecked";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_list_field_switch_type_checked_reversing";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_list_field_switch_type_unchecked_reversing";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_field_switch_type" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_list_field_trigger_option_none( 0 );
const int c_enum_list_field_trigger_option_skey1( 1 );
const int c_enum_list_field_trigger_option_skey2( 2 );
const int c_enum_list_field_trigger_option_skey3( 3 );
const int c_enum_list_field_trigger_option_skey4( 4 );
const int c_enum_list_field_trigger_option_skey5( 5 );
const int c_enum_list_field_trigger_option_skey6( 6 );
const int c_enum_list_field_trigger_option_skey7( 7 );
const int c_enum_list_field_trigger_option_skey8( 8 );
const int c_enum_list_field_trigger_option_skey9( 9 );
const int c_enum_list_field_trigger_option_skey0( 10 );

string get_enum_string_list_field_trigger_option( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_field_trigger_option" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_field_trigger_option_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_field_trigger_option_skey1";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_list_field_trigger_option_skey2";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_list_field_trigger_option_skey3";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_list_field_trigger_option_skey4";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_list_field_trigger_option_skey5";
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_list_field_trigger_option_skey6";
   else if( to_string( val ) == to_string( "7" ) )
      string_name = "enum_list_field_trigger_option_skey7";
   else if( to_string( val ) == to_string( "8" ) )
      string_name = "enum_list_field_trigger_option_skey8";
   else if( to_string( val ) == to_string( "9" ) )
      string_name = "enum_list_field_trigger_option_skey9";
   else if( to_string( val ) == to_string( "10" ) )
      string_name = "enum_list_field_trigger_option_skey0";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_field_trigger_option" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_list_field_view_parent_extra_none( 0 );
const int c_enum_list_field_view_parent_extra_vext1( 1 );
const int c_enum_list_field_view_parent_extra_vext2( 2 );
const int c_enum_list_field_view_parent_extra_vext3( 3 );
const int c_enum_list_field_view_parent_extra_vext4( 4 );
const int c_enum_list_field_view_parent_extra_vext5( 5 );
const int c_enum_list_field_view_parent_extra_vext6( 6 );
const int c_enum_list_field_view_parent_extra_vext7( 7 );
const int c_enum_list_field_view_parent_extra_vext8( 8 );
const int c_enum_list_field_view_parent_extra_vext9( 9 );
const int c_enum_list_field_view_parent_extra_vext0( 10 );

string get_enum_string_list_field_view_parent_extra( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_field_view_parent_extra" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_field_view_parent_extra_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_field_view_parent_extra_vext1";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_list_field_view_parent_extra_vext2";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_list_field_view_parent_extra_vext3";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_list_field_view_parent_extra_vext4";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_list_field_view_parent_extra_vext5";
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_list_field_view_parent_extra_vext6";
   else if( to_string( val ) == to_string( "7" ) )
      string_name = "enum_list_field_view_parent_extra_vext7";
   else if( to_string( val ) == to_string( "8" ) )
      string_name = "enum_list_field_view_parent_extra_vext8";
   else if( to_string( val ) == to_string( "9" ) )
      string_name = "enum_list_field_view_parent_extra_vext9";
   else if( to_string( val ) == to_string( "10" ) )
      string_name = "enum_list_field_view_parent_extra_vext0";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_field_view_parent_extra" );

   return get_module_string( lower( string_name ) );
}

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_List_Field > List_Field_registration( get_class_registry( ), "124100" );

class Meta_List_Field_command_functor;

class Meta_List_Field_command_handler : public command_handler
{
   friend class Meta_List_Field_command_functor;

   public:
   Meta_List_Field_command_handler( ) : p_Meta_List_Field( 0 ) { }

   void set_Meta_List_Field( Meta_List_Field* p_new_Meta_List_Field ) { p_Meta_List_Field = p_new_Meta_List_Field; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_List_Field* p_Meta_List_Field;

   protected:
   string retval;
};

class Meta_List_Field_command_functor : public command_functor
{
   public:
   Meta_List_Field_command_functor( Meta_List_Field_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_List_Field_command_handler& cmd_handler;
};

command_functor* Meta_List_Field_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_List_Field_command_functor( dynamic_cast< Meta_List_Field_command_handler& >( handler ) );
}

void Meta_List_Field_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_List_Field_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_List_Field_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_List_Field_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_List_Field->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_List_Field->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_List_Field_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_List_Field_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Access_Parent_Modifier ) || ( field_name == c_field_name_Access_Parent_Modifier ) ) )
      {
         handled = true;

         string_getter< Meta_Modifier >( cmd_handler.p_Meta_List_Field->Access_Parent_Modifier( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_List_Field->Access_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Restriction ) || ( field_name == c_field_name_Access_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List_Field->Access_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Alignment ) || ( field_name == c_field_name_Alignment ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List_Field->Alignment( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Anonymous_Access ) || ( field_name == c_field_name_Allow_Anonymous_Access ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field->Allow_Anonymous_Access( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Rel_Child_Class ) || ( field_name == c_field_name_Child_Rel_Child_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_List_Field->Child_Rel_Child_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Rel_Source_Child ) || ( field_name == c_field_name_Child_Rel_Source_Child ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_List_Field->Child_Rel_Source_Child( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Rel_Source_Field ) || ( field_name == c_field_name_Child_Rel_Source_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_List_Field->Child_Rel_Source_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Rel_Source_Parent ) || ( field_name == c_field_name_Child_Rel_Source_Parent ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_List_Field->Child_Rel_Source_Parent( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Rel_Source_Parent_Class ) || ( field_name == c_field_name_Child_Rel_Source_Parent_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_List_Field->Child_Rel_Source_Parent_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Relationship ) || ( field_name == c_field_name_Child_Relationship ) ) )
      {
         handled = true;

         string_getter< Meta_Relationship >( cmd_handler.p_Meta_List_Field->Child_Relationship( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_List_Field->Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Exact_Match_Only ) || ( field_name == c_field_name_Exact_Match_Only ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field->Exact_Match_Only( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Exclude_In_Use_FK ) || ( field_name == c_field_name_Exclude_In_Use_FK ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field->Exclude_In_Use_FK( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Font_Size ) || ( field_name == c_field_name_Font_Size ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List_Field->Font_Size( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Hide_If_Is_Changing ) || ( field_name == c_field_name_Hide_If_Is_Changing ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field->Hide_If_Is_Changing( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Include_Key_Additions ) || ( field_name == c_field_name_Include_Key_Additions ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_List_Field->Include_Key_Additions( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Label_Class ) || ( field_name == c_field_name_Label_Class ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List_Field->Label_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Label_Source ) || ( field_name == c_field_name_Label_Source ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List_Field->Label_Source( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Label_Source_Child ) || ( field_name == c_field_name_Label_Source_Child ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List_Field->Label_Source_Child( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Link_Empty_Restriction ) || ( field_name == c_field_name_Link_Empty_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List_Field->Link_Empty_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Link_Permission ) || ( field_name == c_field_name_Link_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_List_Field->Link_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Link_Restriction ) || ( field_name == c_field_name_Link_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List_Field->Link_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Link_Type ) || ( field_name == c_field_name_Link_Type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List_Field->Link_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_List ) || ( field_name == c_field_name_List ) ) )
      {
         handled = true;

         string_getter< Meta_List >( cmd_handler.p_Meta_List_Field->List( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_List_Field->Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Non_Instance_Procedure ) || ( field_name == c_field_name_Non_Instance_Procedure ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field->Non_Instance_Procedure( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Notes_Truncation ) || ( field_name == c_field_name_Notes_Truncation ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List_Field->Notes_Truncation( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Omit_Versions ) || ( field_name == c_field_name_Omit_Versions ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field->Omit_Versions( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Order ) || ( field_name == c_field_name_Order ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_List_Field->Order( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Orientation ) || ( field_name == c_field_name_Orientation ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List_Field->Orientation( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Class ) || ( field_name == c_field_name_Parent_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_List_Field->Parent_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Print_Type ) || ( field_name == c_field_name_Print_Type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List_Field->Print_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Procedure ) || ( field_name == c_field_name_Procedure ) ) )
      {
         handled = true;

         string_getter< Meta_Procedure >( cmd_handler.p_Meta_List_Field->Procedure( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Procedure_Args ) || ( field_name == c_field_name_Procedure_Args ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_List_Field->Procedure_Args( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Restriction_Field ) || ( field_name == c_field_name_Restriction_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_List_Field->Restriction_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Restriction_Spec ) || ( field_name == c_field_name_Restriction_Spec ) ) )
      {
         handled = true;

         string_getter< Meta_Specification >( cmd_handler.p_Meta_List_Field->Restriction_Spec( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Restriction_Value ) || ( field_name == c_field_name_Restriction_Value ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_List_Field->Restriction_Value( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Retain_Selected_Rows ) || ( field_name == c_field_name_Retain_Selected_Rows ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field->Retain_Selected_Rows( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Reverse_Order ) || ( field_name == c_field_name_Reverse_Order ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field->Reverse_Order( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Search_Option_Limit ) || ( field_name == c_field_name_Search_Option_Limit ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List_Field->Search_Option_Limit( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Select_Key_Exclusions ) || ( field_name == c_field_name_Select_Key_Exclusions ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_List_Field->Select_Key_Exclusions( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Sort_Manually ) || ( field_name == c_field_name_Sort_Manually ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field->Sort_Manually( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Child ) || ( field_name == c_field_name_Source_Child ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_List_Field->Source_Child( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Child_Class ) || ( field_name == c_field_name_Source_Child_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_List_Field->Source_Child_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Field ) || ( field_name == c_field_name_Source_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_List_Field->Source_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Grandchild ) || ( field_name == c_field_name_Source_Grandchild ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_List_Field->Source_Grandchild( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Parent ) || ( field_name == c_field_name_Source_Parent ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_List_Field->Source_Parent( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Parent_Class ) || ( field_name == c_field_name_Source_Parent_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_List_Field->Source_Parent_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Switch_Type ) || ( field_name == c_field_name_Switch_Type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List_Field->Switch_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Trigger_Option ) || ( field_name == c_field_name_Trigger_Option ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List_Field->Trigger_Option( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         string_getter< Meta_List_Field_Type >( cmd_handler.p_Meta_List_Field->Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Child_Rel_Source_Parent ) || ( field_name == c_field_name_Use_Child_Rel_Source_Parent ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field->Use_Child_Rel_Source_Parent( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_In_Text_Search_Title ) || ( field_name == c_field_name_Use_In_Text_Search_Title ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field->Use_In_Text_Search_Title( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Source_Parent ) || ( field_name == c_field_name_Use_Source_Parent ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field->Use_Source_Parent( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Type_Field ) || ( field_name == c_field_name_Use_Type_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_List_Field->Use_Type_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_View_Parent_Extra ) || ( field_name == c_field_name_View_Parent_Extra ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List_Field->View_Parent_Extra( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_List_Field_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_List_Field_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_List_Field_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Access_Parent_Modifier ) || ( field_name == c_field_name_Access_Parent_Modifier ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Modifier >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Access_Parent_Modifier, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Permission >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Access_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Restriction ) || ( field_name == c_field_name_Access_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, int >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Access_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Alignment ) || ( field_name == c_field_name_Alignment ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, int >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Alignment, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Anonymous_Access ) || ( field_name == c_field_name_Allow_Anonymous_Access ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, bool >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Allow_Anonymous_Access, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Rel_Child_Class ) || ( field_name == c_field_name_Child_Rel_Child_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Class >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Child_Rel_Child_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Rel_Source_Child ) || ( field_name == c_field_name_Child_Rel_Source_Child ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Field >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Child_Rel_Source_Child, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Rel_Source_Field ) || ( field_name == c_field_name_Child_Rel_Source_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Field >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Child_Rel_Source_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Rel_Source_Parent ) || ( field_name == c_field_name_Child_Rel_Source_Parent ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Field >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Child_Rel_Source_Parent, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Rel_Source_Parent_Class ) || ( field_name == c_field_name_Child_Rel_Source_Parent_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Class >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Child_Rel_Source_Parent_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Relationship ) || ( field_name == c_field_name_Child_Relationship ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Relationship >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Child_Relationship, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Class >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Exact_Match_Only ) || ( field_name == c_field_name_Exact_Match_Only ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, bool >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Exact_Match_Only, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Exclude_In_Use_FK ) || ( field_name == c_field_name_Exclude_In_Use_FK ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, bool >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Exclude_In_Use_FK, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Font_Size ) || ( field_name == c_field_name_Font_Size ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, int >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Font_Size, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Hide_If_Is_Changing ) || ( field_name == c_field_name_Hide_If_Is_Changing ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, bool >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Hide_If_Is_Changing, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Include_Key_Additions ) || ( field_name == c_field_name_Include_Key_Additions ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, string >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Include_Key_Additions, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Label_Class ) || ( field_name == c_field_name_Label_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, int >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Label_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Label_Source ) || ( field_name == c_field_name_Label_Source ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, int >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Label_Source, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Label_Source_Child ) || ( field_name == c_field_name_Label_Source_Child ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, int >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Label_Source_Child, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Link_Empty_Restriction ) || ( field_name == c_field_name_Link_Empty_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, int >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Link_Empty_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Link_Permission ) || ( field_name == c_field_name_Link_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Permission >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Link_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Link_Restriction ) || ( field_name == c_field_name_Link_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, int >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Link_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Link_Type ) || ( field_name == c_field_name_Link_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, int >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Link_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_List ) || ( field_name == c_field_name_List ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_List >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::List, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, string >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Non_Instance_Procedure ) || ( field_name == c_field_name_Non_Instance_Procedure ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, bool >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Non_Instance_Procedure, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Notes_Truncation ) || ( field_name == c_field_name_Notes_Truncation ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, int >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Notes_Truncation, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Omit_Versions ) || ( field_name == c_field_name_Omit_Versions ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, bool >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Omit_Versions, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Order ) || ( field_name == c_field_name_Order ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, string >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Order, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Orientation ) || ( field_name == c_field_name_Orientation ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, int >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Orientation, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Class ) || ( field_name == c_field_name_Parent_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Class >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Parent_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Print_Type ) || ( field_name == c_field_name_Print_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, int >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Print_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Procedure ) || ( field_name == c_field_name_Procedure ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Procedure >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Procedure, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Procedure_Args ) || ( field_name == c_field_name_Procedure_Args ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, string >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Procedure_Args, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Restriction_Field ) || ( field_name == c_field_name_Restriction_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Field >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Restriction_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Restriction_Spec ) || ( field_name == c_field_name_Restriction_Spec ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Specification >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Restriction_Spec, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Restriction_Value ) || ( field_name == c_field_name_Restriction_Value ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, string >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Restriction_Value, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Retain_Selected_Rows ) || ( field_name == c_field_name_Retain_Selected_Rows ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, bool >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Retain_Selected_Rows, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Reverse_Order ) || ( field_name == c_field_name_Reverse_Order ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, bool >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Reverse_Order, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Search_Option_Limit ) || ( field_name == c_field_name_Search_Option_Limit ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, int >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Search_Option_Limit, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Select_Key_Exclusions ) || ( field_name == c_field_name_Select_Key_Exclusions ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, string >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Select_Key_Exclusions, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Sort_Manually ) || ( field_name == c_field_name_Sort_Manually ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, bool >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Sort_Manually, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Child ) || ( field_name == c_field_name_Source_Child ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Field >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Source_Child, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Child_Class ) || ( field_name == c_field_name_Source_Child_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Class >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Source_Child_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Field ) || ( field_name == c_field_name_Source_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Field >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Source_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Grandchild ) || ( field_name == c_field_name_Source_Grandchild ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Field >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Source_Grandchild, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Parent ) || ( field_name == c_field_name_Source_Parent ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Field >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Source_Parent, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Parent_Class ) || ( field_name == c_field_name_Source_Parent_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Class >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Source_Parent_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Switch_Type ) || ( field_name == c_field_name_Switch_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, int >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Switch_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Trigger_Option ) || ( field_name == c_field_name_Trigger_Option ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, int >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Trigger_Option, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_List_Field_Type >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Child_Rel_Source_Parent ) || ( field_name == c_field_name_Use_Child_Rel_Source_Parent ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, bool >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Use_Child_Rel_Source_Parent, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_In_Text_Search_Title ) || ( field_name == c_field_name_Use_In_Text_Search_Title ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, bool >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Use_In_Text_Search_Title, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Source_Parent ) || ( field_name == c_field_name_Use_Source_Parent ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, bool >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Use_Source_Parent, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Type_Field ) || ( field_name == c_field_name_Use_Type_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, Meta_Field >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::Use_Type_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_View_Parent_Extra ) || ( field_name == c_field_name_View_Parent_Extra ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field, int >(
          *cmd_handler.p_Meta_List_Field, &Meta_List_Field::View_Parent_Extra, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_List_Field_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_List_Field_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_List_Field_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_Access_Parent_Modifier ) || ( field_name == c_field_name_Access_Parent_Modifier ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Access_Parent_Modifier( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Access_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Child_Rel_Child_Class ) || ( field_name == c_field_name_Child_Rel_Child_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Child_Rel_Child_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Child_Rel_Source_Child ) || ( field_name == c_field_name_Child_Rel_Source_Child ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Child_Rel_Source_Child( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Child_Rel_Source_Field ) || ( field_name == c_field_name_Child_Rel_Source_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Child_Rel_Source_Field( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Child_Rel_Source_Parent ) || ( field_name == c_field_name_Child_Rel_Source_Parent ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Child_Rel_Source_Parent( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Child_Rel_Source_Parent_Class ) || ( field_name == c_field_name_Child_Rel_Source_Parent_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Child_Rel_Source_Parent_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Child_Relationship ) || ( field_name == c_field_name_Child_Relationship ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Child_Relationship( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Link_Permission ) || ( field_name == c_field_name_Link_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Link_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_List ) || ( field_name == c_field_name_List ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->List( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Parent_Class ) || ( field_name == c_field_name_Parent_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Parent_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Procedure ) || ( field_name == c_field_name_Procedure ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Procedure( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Restriction_Field ) || ( field_name == c_field_name_Restriction_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Restriction_Field( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Restriction_Spec ) || ( field_name == c_field_name_Restriction_Spec ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Restriction_Spec( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Child ) || ( field_name == c_field_name_Source_Child ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Source_Child( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Child_Class ) || ( field_name == c_field_name_Source_Child_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Source_Child_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Field ) || ( field_name == c_field_name_Source_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Source_Field( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Grandchild ) || ( field_name == c_field_name_Source_Grandchild ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Source_Grandchild( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Parent ) || ( field_name == c_field_name_Source_Parent ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Source_Parent( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Parent_Class ) || ( field_name == c_field_name_Source_Parent_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Source_Parent_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Type( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Use_Type_Field ) || ( field_name == c_field_name_Use_Type_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field->Use_Type_Field( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
   else if( command == c_cmd_Meta_List_Field_Move_Down )
   {
      string Restrict_Fields( get_parm_val_from_string< string >( parameters, c_cmd_Meta_List_Field_Move_Down_Restrict_Fields ) );
      string Restrict_Values( get_parm_val_from_string< string >( parameters, c_cmd_Meta_List_Field_Move_Down_Restrict_Values ) );

      cmd_handler.p_Meta_List_Field->Move_Down( Restrict_Fields, Restrict_Values );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_List_Field_Move_Up )
   {
      string Restrict_Fields( get_parm_val_from_string< string >( parameters, c_cmd_Meta_List_Field_Move_Up_Restrict_Fields ) );
      string Restrict_Values( get_parm_val_from_string< string >( parameters, c_cmd_Meta_List_Field_Move_Up_Restrict_Values ) );

      cmd_handler.p_Meta_List_Field->Move_Up( Restrict_Fields, Restrict_Values );

      cmd_handler.retval.erase( );

   }
}

struct Meta_List_Field::impl : public Meta_List_Field_command_handler
{
   impl( Meta_List_Field& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_List_Field( &o );

      add_commands( 0, Meta_List_Field_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_List_Field_command_definitions ) );
   }

   Meta_List_Field& get_obj( ) const
   {
      return *cp_obj;
   }

   int impl_Access_Restriction( ) const { return lazy_fetch( p_obj ), v_Access_Restriction; }
   void impl_Access_Restriction( int Access_Restriction ) { v_Access_Restriction = Access_Restriction; }

   int impl_Alignment( ) const { return lazy_fetch( p_obj ), v_Alignment; }
   void impl_Alignment( int Alignment ) { v_Alignment = Alignment; }

   bool impl_Allow_Anonymous_Access( ) const { return lazy_fetch( p_obj ), v_Allow_Anonymous_Access; }
   void impl_Allow_Anonymous_Access( bool Allow_Anonymous_Access ) { v_Allow_Anonymous_Access = Allow_Anonymous_Access; }

   bool impl_Exact_Match_Only( ) const { return lazy_fetch( p_obj ), v_Exact_Match_Only; }
   void impl_Exact_Match_Only( bool Exact_Match_Only ) { v_Exact_Match_Only = Exact_Match_Only; }

   bool impl_Exclude_In_Use_FK( ) const { return lazy_fetch( p_obj ), v_Exclude_In_Use_FK; }
   void impl_Exclude_In_Use_FK( bool Exclude_In_Use_FK ) { v_Exclude_In_Use_FK = Exclude_In_Use_FK; }

   int impl_Font_Size( ) const { return lazy_fetch( p_obj ), v_Font_Size; }
   void impl_Font_Size( int Font_Size ) { v_Font_Size = Font_Size; }

   bool impl_Hide_If_Is_Changing( ) const { return lazy_fetch( p_obj ), v_Hide_If_Is_Changing; }
   void impl_Hide_If_Is_Changing( bool Hide_If_Is_Changing ) { v_Hide_If_Is_Changing = Hide_If_Is_Changing; }

   const string& impl_Include_Key_Additions( ) const { return lazy_fetch( p_obj ), v_Include_Key_Additions; }
   void impl_Include_Key_Additions( const string& Include_Key_Additions ) { sanity_check( Include_Key_Additions ); v_Include_Key_Additions = Include_Key_Additions; }

   int impl_Label_Class( ) const { return lazy_fetch( p_obj ), v_Label_Class; }
   void impl_Label_Class( int Label_Class ) { v_Label_Class = Label_Class; }

   int impl_Label_Source( ) const { return lazy_fetch( p_obj ), v_Label_Source; }
   void impl_Label_Source( int Label_Source ) { v_Label_Source = Label_Source; }

   int impl_Label_Source_Child( ) const { return lazy_fetch( p_obj ), v_Label_Source_Child; }
   void impl_Label_Source_Child( int Label_Source_Child ) { v_Label_Source_Child = Label_Source_Child; }

   int impl_Link_Empty_Restriction( ) const { return lazy_fetch( p_obj ), v_Link_Empty_Restriction; }
   void impl_Link_Empty_Restriction( int Link_Empty_Restriction ) { v_Link_Empty_Restriction = Link_Empty_Restriction; }

   int impl_Link_Restriction( ) const { return lazy_fetch( p_obj ), v_Link_Restriction; }
   void impl_Link_Restriction( int Link_Restriction ) { v_Link_Restriction = Link_Restriction; }

   int impl_Link_Type( ) const { return lazy_fetch( p_obj ), v_Link_Type; }
   void impl_Link_Type( int Link_Type ) { v_Link_Type = Link_Type; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { sanity_check( Name ); v_Name = Name; }

   bool impl_Non_Instance_Procedure( ) const { return lazy_fetch( p_obj ), v_Non_Instance_Procedure; }
   void impl_Non_Instance_Procedure( bool Non_Instance_Procedure ) { v_Non_Instance_Procedure = Non_Instance_Procedure; }

   int impl_Notes_Truncation( ) const { return lazy_fetch( p_obj ), v_Notes_Truncation; }
   void impl_Notes_Truncation( int Notes_Truncation ) { v_Notes_Truncation = Notes_Truncation; }

   bool impl_Omit_Versions( ) const { return lazy_fetch( p_obj ), v_Omit_Versions; }
   void impl_Omit_Versions( bool Omit_Versions ) { v_Omit_Versions = Omit_Versions; }

   const string& impl_Order( ) const { return lazy_fetch( p_obj ), v_Order; }
   void impl_Order( const string& Order ) { sanity_check( Order ); v_Order = Order; }

   int impl_Orientation( ) const { return lazy_fetch( p_obj ), v_Orientation; }
   void impl_Orientation( int Orientation ) { v_Orientation = Orientation; }

   int impl_Print_Type( ) const { return lazy_fetch( p_obj ), v_Print_Type; }
   void impl_Print_Type( int Print_Type ) { v_Print_Type = Print_Type; }

   const string& impl_Procedure_Args( ) const { return lazy_fetch( p_obj ), v_Procedure_Args; }
   void impl_Procedure_Args( const string& Procedure_Args ) { sanity_check( Procedure_Args ); v_Procedure_Args = Procedure_Args; }

   const string& impl_Restriction_Value( ) const { return lazy_fetch( p_obj ), v_Restriction_Value; }
   void impl_Restriction_Value( const string& Restriction_Value ) { sanity_check( Restriction_Value ); v_Restriction_Value = Restriction_Value; }

   bool impl_Retain_Selected_Rows( ) const { return lazy_fetch( p_obj ), v_Retain_Selected_Rows; }
   void impl_Retain_Selected_Rows( bool Retain_Selected_Rows ) { v_Retain_Selected_Rows = Retain_Selected_Rows; }

   bool impl_Reverse_Order( ) const { return lazy_fetch( p_obj ), v_Reverse_Order; }
   void impl_Reverse_Order( bool Reverse_Order ) { v_Reverse_Order = Reverse_Order; }

   int impl_Search_Option_Limit( ) const { return lazy_fetch( p_obj ), v_Search_Option_Limit; }
   void impl_Search_Option_Limit( int Search_Option_Limit ) { v_Search_Option_Limit = Search_Option_Limit; }

   const string& impl_Select_Key_Exclusions( ) const { return lazy_fetch( p_obj ), v_Select_Key_Exclusions; }
   void impl_Select_Key_Exclusions( const string& Select_Key_Exclusions ) { sanity_check( Select_Key_Exclusions ); v_Select_Key_Exclusions = Select_Key_Exclusions; }

   bool impl_Sort_Manually( ) const { return lazy_fetch( p_obj ), v_Sort_Manually; }
   void impl_Sort_Manually( bool Sort_Manually ) { v_Sort_Manually = Sort_Manually; }

   int impl_Switch_Type( ) const { return lazy_fetch( p_obj ), v_Switch_Type; }
   void impl_Switch_Type( int Switch_Type ) { v_Switch_Type = Switch_Type; }

   int impl_Trigger_Option( ) const { return lazy_fetch( p_obj ), v_Trigger_Option; }
   void impl_Trigger_Option( int Trigger_Option ) { v_Trigger_Option = Trigger_Option; }

   bool impl_Use_Child_Rel_Source_Parent( ) const { return lazy_fetch( p_obj ), v_Use_Child_Rel_Source_Parent; }
   void impl_Use_Child_Rel_Source_Parent( bool Use_Child_Rel_Source_Parent ) { v_Use_Child_Rel_Source_Parent = Use_Child_Rel_Source_Parent; }

   bool impl_Use_In_Text_Search_Title( ) const { return lazy_fetch( p_obj ), v_Use_In_Text_Search_Title; }
   void impl_Use_In_Text_Search_Title( bool Use_In_Text_Search_Title ) { v_Use_In_Text_Search_Title = Use_In_Text_Search_Title; }

   bool impl_Use_Source_Parent( ) const { return lazy_fetch( p_obj ), v_Use_Source_Parent; }
   void impl_Use_Source_Parent( bool Use_Source_Parent ) { v_Use_Source_Parent = Use_Source_Parent; }

   int impl_View_Parent_Extra( ) const { return lazy_fetch( p_obj ), v_View_Parent_Extra; }
   void impl_View_Parent_Extra( int View_Parent_Extra ) { v_View_Parent_Extra = View_Parent_Extra; }

   Meta_Modifier& impl_Access_Parent_Modifier( )
   {
      if( !cp_Access_Parent_Modifier )
      {
         cp_Access_Parent_Modifier.init( );

         p_obj->setup_graph_parent( *cp_Access_Parent_Modifier, c_field_id_Access_Parent_Modifier, v_Access_Parent_Modifier );
      }
      return *cp_Access_Parent_Modifier;
   }

   const Meta_Modifier& impl_Access_Parent_Modifier( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Access_Parent_Modifier )
      {
         cp_Access_Parent_Modifier.init( );

         p_obj->setup_graph_parent( *cp_Access_Parent_Modifier, c_field_id_Access_Parent_Modifier, v_Access_Parent_Modifier );
      }
      return *cp_Access_Parent_Modifier;
   }

   void impl_Access_Parent_Modifier( const string& key )
   {
      class_base_accessor cba( impl_Access_Parent_Modifier( ) );
      cba.set_key( key );
   }

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

   Meta_Class& impl_Child_Rel_Child_Class( )
   {
      if( !cp_Child_Rel_Child_Class )
      {
         cp_Child_Rel_Child_Class.init( );

         p_obj->setup_graph_parent( *cp_Child_Rel_Child_Class, c_field_id_Child_Rel_Child_Class, v_Child_Rel_Child_Class );
      }
      return *cp_Child_Rel_Child_Class;
   }

   const Meta_Class& impl_Child_Rel_Child_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Child_Rel_Child_Class )
      {
         cp_Child_Rel_Child_Class.init( );

         p_obj->setup_graph_parent( *cp_Child_Rel_Child_Class, c_field_id_Child_Rel_Child_Class, v_Child_Rel_Child_Class );
      }
      return *cp_Child_Rel_Child_Class;
   }

   void impl_Child_Rel_Child_Class( const string& key )
   {
      class_base_accessor cba( impl_Child_Rel_Child_Class( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Child_Rel_Source_Child( )
   {
      if( !cp_Child_Rel_Source_Child )
      {
         cp_Child_Rel_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_Child_Rel_Source_Child, c_field_id_Child_Rel_Source_Child, v_Child_Rel_Source_Child );
      }
      return *cp_Child_Rel_Source_Child;
   }

   const Meta_Field& impl_Child_Rel_Source_Child( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Child_Rel_Source_Child )
      {
         cp_Child_Rel_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_Child_Rel_Source_Child, c_field_id_Child_Rel_Source_Child, v_Child_Rel_Source_Child );
      }
      return *cp_Child_Rel_Source_Child;
   }

   void impl_Child_Rel_Source_Child( const string& key )
   {
      class_base_accessor cba( impl_Child_Rel_Source_Child( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Child_Rel_Source_Field( )
   {
      if( !cp_Child_Rel_Source_Field )
      {
         cp_Child_Rel_Source_Field.init( );

         p_obj->setup_graph_parent( *cp_Child_Rel_Source_Field, c_field_id_Child_Rel_Source_Field, v_Child_Rel_Source_Field );
      }
      return *cp_Child_Rel_Source_Field;
   }

   const Meta_Field& impl_Child_Rel_Source_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Child_Rel_Source_Field )
      {
         cp_Child_Rel_Source_Field.init( );

         p_obj->setup_graph_parent( *cp_Child_Rel_Source_Field, c_field_id_Child_Rel_Source_Field, v_Child_Rel_Source_Field );
      }
      return *cp_Child_Rel_Source_Field;
   }

   void impl_Child_Rel_Source_Field( const string& key )
   {
      class_base_accessor cba( impl_Child_Rel_Source_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Child_Rel_Source_Parent( )
   {
      if( !cp_Child_Rel_Source_Parent )
      {
         cp_Child_Rel_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_Child_Rel_Source_Parent, c_field_id_Child_Rel_Source_Parent, v_Child_Rel_Source_Parent );
      }
      return *cp_Child_Rel_Source_Parent;
   }

   const Meta_Field& impl_Child_Rel_Source_Parent( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Child_Rel_Source_Parent )
      {
         cp_Child_Rel_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_Child_Rel_Source_Parent, c_field_id_Child_Rel_Source_Parent, v_Child_Rel_Source_Parent );
      }
      return *cp_Child_Rel_Source_Parent;
   }

   void impl_Child_Rel_Source_Parent( const string& key )
   {
      class_base_accessor cba( impl_Child_Rel_Source_Parent( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Child_Rel_Source_Parent_Class( )
   {
      if( !cp_Child_Rel_Source_Parent_Class )
      {
         cp_Child_Rel_Source_Parent_Class.init( );

         p_obj->setup_graph_parent( *cp_Child_Rel_Source_Parent_Class, c_field_id_Child_Rel_Source_Parent_Class, v_Child_Rel_Source_Parent_Class );
      }
      return *cp_Child_Rel_Source_Parent_Class;
   }

   const Meta_Class& impl_Child_Rel_Source_Parent_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Child_Rel_Source_Parent_Class )
      {
         cp_Child_Rel_Source_Parent_Class.init( );

         p_obj->setup_graph_parent( *cp_Child_Rel_Source_Parent_Class, c_field_id_Child_Rel_Source_Parent_Class, v_Child_Rel_Source_Parent_Class );
      }
      return *cp_Child_Rel_Source_Parent_Class;
   }

   void impl_Child_Rel_Source_Parent_Class( const string& key )
   {
      class_base_accessor cba( impl_Child_Rel_Source_Parent_Class( ) );
      cba.set_key( key );
   }

   Meta_Relationship& impl_Child_Relationship( )
   {
      if( !cp_Child_Relationship )
      {
         cp_Child_Relationship.init( );

         p_obj->setup_graph_parent( *cp_Child_Relationship, c_field_id_Child_Relationship, v_Child_Relationship );
      }
      return *cp_Child_Relationship;
   }

   const Meta_Relationship& impl_Child_Relationship( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Child_Relationship )
      {
         cp_Child_Relationship.init( );

         p_obj->setup_graph_parent( *cp_Child_Relationship, c_field_id_Child_Relationship, v_Child_Relationship );
      }
      return *cp_Child_Relationship;
   }

   void impl_Child_Relationship( const string& key )
   {
      class_base_accessor cba( impl_Child_Relationship( ) );
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

   Meta_List& impl_List( )
   {
      if( !cp_List )
      {
         cp_List.init( );

         p_obj->setup_graph_parent( *cp_List, c_field_id_List, v_List );
      }
      return *cp_List;
   }

   const Meta_List& impl_List( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_List )
      {
         cp_List.init( );

         p_obj->setup_graph_parent( *cp_List, c_field_id_List, v_List );
      }
      return *cp_List;
   }

   void impl_List( const string& key )
   {
      class_base_accessor cba( impl_List( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Parent_Class( )
   {
      if( !cp_Parent_Class )
      {
         cp_Parent_Class.init( );

         p_obj->setup_graph_parent( *cp_Parent_Class, c_field_id_Parent_Class, v_Parent_Class );
      }
      return *cp_Parent_Class;
   }

   const Meta_Class& impl_Parent_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Parent_Class )
      {
         cp_Parent_Class.init( );

         p_obj->setup_graph_parent( *cp_Parent_Class, c_field_id_Parent_Class, v_Parent_Class );
      }
      return *cp_Parent_Class;
   }

   void impl_Parent_Class( const string& key )
   {
      class_base_accessor cba( impl_Parent_Class( ) );
      cba.set_key( key );
   }

   Meta_Procedure& impl_Procedure( )
   {
      if( !cp_Procedure )
      {
         cp_Procedure.init( );

         p_obj->setup_graph_parent( *cp_Procedure, c_field_id_Procedure, v_Procedure );
      }
      return *cp_Procedure;
   }

   const Meta_Procedure& impl_Procedure( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Procedure )
      {
         cp_Procedure.init( );

         p_obj->setup_graph_parent( *cp_Procedure, c_field_id_Procedure, v_Procedure );
      }
      return *cp_Procedure;
   }

   void impl_Procedure( const string& key )
   {
      class_base_accessor cba( impl_Procedure( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Restriction_Field( )
   {
      if( !cp_Restriction_Field )
      {
         cp_Restriction_Field.init( );

         p_obj->setup_graph_parent( *cp_Restriction_Field, c_field_id_Restriction_Field, v_Restriction_Field );
      }
      return *cp_Restriction_Field;
   }

   const Meta_Field& impl_Restriction_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Restriction_Field )
      {
         cp_Restriction_Field.init( );

         p_obj->setup_graph_parent( *cp_Restriction_Field, c_field_id_Restriction_Field, v_Restriction_Field );
      }
      return *cp_Restriction_Field;
   }

   void impl_Restriction_Field( const string& key )
   {
      class_base_accessor cba( impl_Restriction_Field( ) );
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

   Meta_Class& impl_Source_Child_Class( )
   {
      if( !cp_Source_Child_Class )
      {
         cp_Source_Child_Class.init( );

         p_obj->setup_graph_parent( *cp_Source_Child_Class, c_field_id_Source_Child_Class, v_Source_Child_Class );
      }
      return *cp_Source_Child_Class;
   }

   const Meta_Class& impl_Source_Child_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Child_Class )
      {
         cp_Source_Child_Class.init( );

         p_obj->setup_graph_parent( *cp_Source_Child_Class, c_field_id_Source_Child_Class, v_Source_Child_Class );
      }
      return *cp_Source_Child_Class;
   }

   void impl_Source_Child_Class( const string& key )
   {
      class_base_accessor cba( impl_Source_Child_Class( ) );
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

   Meta_Field& impl_Source_Grandchild( )
   {
      if( !cp_Source_Grandchild )
      {
         cp_Source_Grandchild.init( );

         p_obj->setup_graph_parent( *cp_Source_Grandchild, c_field_id_Source_Grandchild, v_Source_Grandchild );
      }
      return *cp_Source_Grandchild;
   }

   const Meta_Field& impl_Source_Grandchild( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Grandchild )
      {
         cp_Source_Grandchild.init( );

         p_obj->setup_graph_parent( *cp_Source_Grandchild, c_field_id_Source_Grandchild, v_Source_Grandchild );
      }
      return *cp_Source_Grandchild;
   }

   void impl_Source_Grandchild( const string& key )
   {
      class_base_accessor cba( impl_Source_Grandchild( ) );
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

   Meta_List_Field_Type& impl_Type( )
   {
      if( !cp_Type )
      {
         cp_Type.init( );

         p_obj->setup_graph_parent( *cp_Type, c_field_id_Type, v_Type );
      }
      return *cp_Type;
   }

   const Meta_List_Field_Type& impl_Type( ) const
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

   Meta_Field& impl_Use_Type_Field( )
   {
      if( !cp_Use_Type_Field )
      {
         cp_Use_Type_Field.init( );

         p_obj->setup_graph_parent( *cp_Use_Type_Field, c_field_id_Use_Type_Field, v_Use_Type_Field );
      }
      return *cp_Use_Type_Field;
   }

   const Meta_Field& impl_Use_Type_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Use_Type_Field )
      {
         cp_Use_Type_Field.init( );

         p_obj->setup_graph_parent( *cp_Use_Type_Field, c_field_id_Use_Type_Field, v_Use_Type_Field );
      }
      return *cp_Use_Type_Field;
   }

   void impl_Use_Type_Field( const string& key )
   {
      class_base_accessor cba( impl_Use_Type_Field( ) );
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

   Meta_List_Field* p_obj;
   class_pointer< Meta_List_Field > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   int v_Access_Restriction;
   int v_Alignment;
   bool v_Allow_Anonymous_Access;
   bool v_Exact_Match_Only;
   bool v_Exclude_In_Use_FK;
   int v_Font_Size;
   bool v_Hide_If_Is_Changing;
   string v_Include_Key_Additions;
   int v_Label_Class;
   int v_Label_Source;
   int v_Label_Source_Child;
   int v_Link_Empty_Restriction;
   int v_Link_Restriction;
   int v_Link_Type;
   string v_Name;
   bool v_Non_Instance_Procedure;
   int v_Notes_Truncation;
   bool v_Omit_Versions;
   string v_Order;
   int v_Orientation;
   int v_Print_Type;
   string v_Procedure_Args;
   string v_Restriction_Value;
   bool v_Retain_Selected_Rows;
   bool v_Reverse_Order;
   int v_Search_Option_Limit;
   string v_Select_Key_Exclusions;
   bool v_Sort_Manually;
   int v_Switch_Type;
   int v_Trigger_Option;
   bool v_Use_Child_Rel_Source_Parent;
   bool v_Use_In_Text_Search_Title;
   bool v_Use_Source_Parent;
   int v_View_Parent_Extra;

   string v_Access_Parent_Modifier;
   mutable class_pointer< Meta_Modifier > cp_Access_Parent_Modifier;

   string v_Access_Permission;
   mutable class_pointer< Meta_Permission > cp_Access_Permission;

   string v_Child_Rel_Child_Class;
   mutable class_pointer< Meta_Class > cp_Child_Rel_Child_Class;

   string v_Child_Rel_Source_Child;
   mutable class_pointer< Meta_Field > cp_Child_Rel_Source_Child;

   string v_Child_Rel_Source_Field;
   mutable class_pointer< Meta_Field > cp_Child_Rel_Source_Field;

   string v_Child_Rel_Source_Parent;
   mutable class_pointer< Meta_Field > cp_Child_Rel_Source_Parent;

   string v_Child_Rel_Source_Parent_Class;
   mutable class_pointer< Meta_Class > cp_Child_Rel_Source_Parent_Class;

   string v_Child_Relationship;
   mutable class_pointer< Meta_Relationship > cp_Child_Relationship;

   string v_Class;
   mutable class_pointer< Meta_Class > cp_Class;

   string v_Link_Permission;
   mutable class_pointer< Meta_Permission > cp_Link_Permission;

   string v_List;
   mutable class_pointer< Meta_List > cp_List;

   string v_Parent_Class;
   mutable class_pointer< Meta_Class > cp_Parent_Class;

   string v_Procedure;
   mutable class_pointer< Meta_Procedure > cp_Procedure;

   string v_Restriction_Field;
   mutable class_pointer< Meta_Field > cp_Restriction_Field;

   string v_Restriction_Spec;
   mutable class_pointer< Meta_Specification > cp_Restriction_Spec;

   string v_Source_Child;
   mutable class_pointer< Meta_Field > cp_Source_Child;

   string v_Source_Child_Class;
   mutable class_pointer< Meta_Class > cp_Source_Child_Class;

   string v_Source_Field;
   mutable class_pointer< Meta_Field > cp_Source_Field;

   string v_Source_Grandchild;
   mutable class_pointer< Meta_Field > cp_Source_Grandchild;

   string v_Source_Parent;
   mutable class_pointer< Meta_Field > cp_Source_Parent;

   string v_Source_Parent_Class;
   mutable class_pointer< Meta_Class > cp_Source_Parent_Class;

   string v_Type;
   mutable class_pointer< Meta_List_Field_Type > cp_Type;

   string v_Use_Type_Field;
   mutable class_pointer< Meta_Field > cp_Use_Type_Field;
};

void Meta_List_Field::impl::impl_Move_Down( const string& Restrict_Fields, const string& Restrict_Values )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start move_up_and_down)] 600448
   transaction_start( );
   try
   {
      // NOTE: Empty code block for scope purposes.
      {
         get_obj( ).op_update( get_obj( ).get_key( ), c_field_name_Order );

         class_pointer< Meta_List_Field > cp_other( e_create_instance );

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
   // [(finish move_up_and_down)] 600448

   // [<start Move_Down_impl>]
   // [<finish Move_Down_impl>]
}

void Meta_List_Field::impl::impl_Move_Up( const string& Restrict_Fields, const string& Restrict_Values )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start move_up_and_down)] 600448
   transaction_start( );
   try
   {
      // NOTE: Empty code block for scope purposes.
      {
         get_obj( ).op_update( get_obj( ).get_key( ), c_field_name_Order );

         class_pointer< Meta_List_Field > cp_other( e_create_instance );

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
   // [(finish move_up_and_down)] 600448

   // [<start Move_Up_impl>]
   // [<finish Move_Up_impl>]
}

string Meta_List_Field::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Access_Parent_Modifier( ) );
      break;

      case 1:
      retval = to_string( impl_Access_Permission( ) );
      break;

      case 2:
      retval = to_string( impl_Access_Restriction( ) );
      break;

      case 3:
      retval = to_string( impl_Alignment( ) );
      break;

      case 4:
      retval = to_string( impl_Allow_Anonymous_Access( ) );
      break;

      case 5:
      retval = to_string( impl_Child_Rel_Child_Class( ) );
      break;

      case 6:
      retval = to_string( impl_Child_Rel_Source_Child( ) );
      break;

      case 7:
      retval = to_string( impl_Child_Rel_Source_Field( ) );
      break;

      case 8:
      retval = to_string( impl_Child_Rel_Source_Parent( ) );
      break;

      case 9:
      retval = to_string( impl_Child_Rel_Source_Parent_Class( ) );
      break;

      case 10:
      retval = to_string( impl_Child_Relationship( ) );
      break;

      case 11:
      retval = to_string( impl_Class( ) );
      break;

      case 12:
      retval = to_string( impl_Exact_Match_Only( ) );
      break;

      case 13:
      retval = to_string( impl_Exclude_In_Use_FK( ) );
      break;

      case 14:
      retval = to_string( impl_Font_Size( ) );
      break;

      case 15:
      retval = to_string( impl_Hide_If_Is_Changing( ) );
      break;

      case 16:
      retval = to_string( impl_Include_Key_Additions( ) );
      break;

      case 17:
      retval = to_string( impl_Label_Class( ) );
      break;

      case 18:
      retval = to_string( impl_Label_Source( ) );
      break;

      case 19:
      retval = to_string( impl_Label_Source_Child( ) );
      break;

      case 20:
      retval = to_string( impl_Link_Empty_Restriction( ) );
      break;

      case 21:
      retval = to_string( impl_Link_Permission( ) );
      break;

      case 22:
      retval = to_string( impl_Link_Restriction( ) );
      break;

      case 23:
      retval = to_string( impl_Link_Type( ) );
      break;

      case 24:
      retval = to_string( impl_List( ) );
      break;

      case 25:
      retval = to_string( impl_Name( ) );
      break;

      case 26:
      retval = to_string( impl_Non_Instance_Procedure( ) );
      break;

      case 27:
      retval = to_string( impl_Notes_Truncation( ) );
      break;

      case 28:
      retval = to_string( impl_Omit_Versions( ) );
      break;

      case 29:
      retval = to_string( impl_Order( ) );
      break;

      case 30:
      retval = to_string( impl_Orientation( ) );
      break;

      case 31:
      retval = to_string( impl_Parent_Class( ) );
      break;

      case 32:
      retval = to_string( impl_Print_Type( ) );
      break;

      case 33:
      retval = to_string( impl_Procedure( ) );
      break;

      case 34:
      retval = to_string( impl_Procedure_Args( ) );
      break;

      case 35:
      retval = to_string( impl_Restriction_Field( ) );
      break;

      case 36:
      retval = to_string( impl_Restriction_Spec( ) );
      break;

      case 37:
      retval = to_string( impl_Restriction_Value( ) );
      break;

      case 38:
      retval = to_string( impl_Retain_Selected_Rows( ) );
      break;

      case 39:
      retval = to_string( impl_Reverse_Order( ) );
      break;

      case 40:
      retval = to_string( impl_Search_Option_Limit( ) );
      break;

      case 41:
      retval = to_string( impl_Select_Key_Exclusions( ) );
      break;

      case 42:
      retval = to_string( impl_Sort_Manually( ) );
      break;

      case 43:
      retval = to_string( impl_Source_Child( ) );
      break;

      case 44:
      retval = to_string( impl_Source_Child_Class( ) );
      break;

      case 45:
      retval = to_string( impl_Source_Field( ) );
      break;

      case 46:
      retval = to_string( impl_Source_Grandchild( ) );
      break;

      case 47:
      retval = to_string( impl_Source_Parent( ) );
      break;

      case 48:
      retval = to_string( impl_Source_Parent_Class( ) );
      break;

      case 49:
      retval = to_string( impl_Switch_Type( ) );
      break;

      case 50:
      retval = to_string( impl_Trigger_Option( ) );
      break;

      case 51:
      retval = to_string( impl_Type( ) );
      break;

      case 52:
      retval = to_string( impl_Use_Child_Rel_Source_Parent( ) );
      break;

      case 53:
      retval = to_string( impl_Use_In_Text_Search_Title( ) );
      break;

      case 54:
      retval = to_string( impl_Use_Source_Parent( ) );
      break;

      case 55:
      retval = to_string( impl_Use_Type_Field( ) );
      break;

      case 56:
      retval = to_string( impl_View_Parent_Extra( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_List_Field::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_List_Field::impl, Meta_Modifier >( *this, &Meta_List_Field::impl::impl_Access_Parent_Modifier, value );
      break;

      case 1:
      func_string_setter< Meta_List_Field::impl, Meta_Permission >( *this, &Meta_List_Field::impl::impl_Access_Permission, value );
      break;

      case 2:
      func_string_setter< Meta_List_Field::impl, int >( *this, &Meta_List_Field::impl::impl_Access_Restriction, value );
      break;

      case 3:
      func_string_setter< Meta_List_Field::impl, int >( *this, &Meta_List_Field::impl::impl_Alignment, value );
      break;

      case 4:
      func_string_setter< Meta_List_Field::impl, bool >( *this, &Meta_List_Field::impl::impl_Allow_Anonymous_Access, value );
      break;

      case 5:
      func_string_setter< Meta_List_Field::impl, Meta_Class >( *this, &Meta_List_Field::impl::impl_Child_Rel_Child_Class, value );
      break;

      case 6:
      func_string_setter< Meta_List_Field::impl, Meta_Field >( *this, &Meta_List_Field::impl::impl_Child_Rel_Source_Child, value );
      break;

      case 7:
      func_string_setter< Meta_List_Field::impl, Meta_Field >( *this, &Meta_List_Field::impl::impl_Child_Rel_Source_Field, value );
      break;

      case 8:
      func_string_setter< Meta_List_Field::impl, Meta_Field >( *this, &Meta_List_Field::impl::impl_Child_Rel_Source_Parent, value );
      break;

      case 9:
      func_string_setter< Meta_List_Field::impl, Meta_Class >( *this, &Meta_List_Field::impl::impl_Child_Rel_Source_Parent_Class, value );
      break;

      case 10:
      func_string_setter< Meta_List_Field::impl, Meta_Relationship >( *this, &Meta_List_Field::impl::impl_Child_Relationship, value );
      break;

      case 11:
      func_string_setter< Meta_List_Field::impl, Meta_Class >( *this, &Meta_List_Field::impl::impl_Class, value );
      break;

      case 12:
      func_string_setter< Meta_List_Field::impl, bool >( *this, &Meta_List_Field::impl::impl_Exact_Match_Only, value );
      break;

      case 13:
      func_string_setter< Meta_List_Field::impl, bool >( *this, &Meta_List_Field::impl::impl_Exclude_In_Use_FK, value );
      break;

      case 14:
      func_string_setter< Meta_List_Field::impl, int >( *this, &Meta_List_Field::impl::impl_Font_Size, value );
      break;

      case 15:
      func_string_setter< Meta_List_Field::impl, bool >( *this, &Meta_List_Field::impl::impl_Hide_If_Is_Changing, value );
      break;

      case 16:
      func_string_setter< Meta_List_Field::impl, string >( *this, &Meta_List_Field::impl::impl_Include_Key_Additions, value );
      break;

      case 17:
      func_string_setter< Meta_List_Field::impl, int >( *this, &Meta_List_Field::impl::impl_Label_Class, value );
      break;

      case 18:
      func_string_setter< Meta_List_Field::impl, int >( *this, &Meta_List_Field::impl::impl_Label_Source, value );
      break;

      case 19:
      func_string_setter< Meta_List_Field::impl, int >( *this, &Meta_List_Field::impl::impl_Label_Source_Child, value );
      break;

      case 20:
      func_string_setter< Meta_List_Field::impl, int >( *this, &Meta_List_Field::impl::impl_Link_Empty_Restriction, value );
      break;

      case 21:
      func_string_setter< Meta_List_Field::impl, Meta_Permission >( *this, &Meta_List_Field::impl::impl_Link_Permission, value );
      break;

      case 22:
      func_string_setter< Meta_List_Field::impl, int >( *this, &Meta_List_Field::impl::impl_Link_Restriction, value );
      break;

      case 23:
      func_string_setter< Meta_List_Field::impl, int >( *this, &Meta_List_Field::impl::impl_Link_Type, value );
      break;

      case 24:
      func_string_setter< Meta_List_Field::impl, Meta_List >( *this, &Meta_List_Field::impl::impl_List, value );
      break;

      case 25:
      func_string_setter< Meta_List_Field::impl, string >( *this, &Meta_List_Field::impl::impl_Name, value );
      break;

      case 26:
      func_string_setter< Meta_List_Field::impl, bool >( *this, &Meta_List_Field::impl::impl_Non_Instance_Procedure, value );
      break;

      case 27:
      func_string_setter< Meta_List_Field::impl, int >( *this, &Meta_List_Field::impl::impl_Notes_Truncation, value );
      break;

      case 28:
      func_string_setter< Meta_List_Field::impl, bool >( *this, &Meta_List_Field::impl::impl_Omit_Versions, value );
      break;

      case 29:
      func_string_setter< Meta_List_Field::impl, string >( *this, &Meta_List_Field::impl::impl_Order, value );
      break;

      case 30:
      func_string_setter< Meta_List_Field::impl, int >( *this, &Meta_List_Field::impl::impl_Orientation, value );
      break;

      case 31:
      func_string_setter< Meta_List_Field::impl, Meta_Class >( *this, &Meta_List_Field::impl::impl_Parent_Class, value );
      break;

      case 32:
      func_string_setter< Meta_List_Field::impl, int >( *this, &Meta_List_Field::impl::impl_Print_Type, value );
      break;

      case 33:
      func_string_setter< Meta_List_Field::impl, Meta_Procedure >( *this, &Meta_List_Field::impl::impl_Procedure, value );
      break;

      case 34:
      func_string_setter< Meta_List_Field::impl, string >( *this, &Meta_List_Field::impl::impl_Procedure_Args, value );
      break;

      case 35:
      func_string_setter< Meta_List_Field::impl, Meta_Field >( *this, &Meta_List_Field::impl::impl_Restriction_Field, value );
      break;

      case 36:
      func_string_setter< Meta_List_Field::impl, Meta_Specification >( *this, &Meta_List_Field::impl::impl_Restriction_Spec, value );
      break;

      case 37:
      func_string_setter< Meta_List_Field::impl, string >( *this, &Meta_List_Field::impl::impl_Restriction_Value, value );
      break;

      case 38:
      func_string_setter< Meta_List_Field::impl, bool >( *this, &Meta_List_Field::impl::impl_Retain_Selected_Rows, value );
      break;

      case 39:
      func_string_setter< Meta_List_Field::impl, bool >( *this, &Meta_List_Field::impl::impl_Reverse_Order, value );
      break;

      case 40:
      func_string_setter< Meta_List_Field::impl, int >( *this, &Meta_List_Field::impl::impl_Search_Option_Limit, value );
      break;

      case 41:
      func_string_setter< Meta_List_Field::impl, string >( *this, &Meta_List_Field::impl::impl_Select_Key_Exclusions, value );
      break;

      case 42:
      func_string_setter< Meta_List_Field::impl, bool >( *this, &Meta_List_Field::impl::impl_Sort_Manually, value );
      break;

      case 43:
      func_string_setter< Meta_List_Field::impl, Meta_Field >( *this, &Meta_List_Field::impl::impl_Source_Child, value );
      break;

      case 44:
      func_string_setter< Meta_List_Field::impl, Meta_Class >( *this, &Meta_List_Field::impl::impl_Source_Child_Class, value );
      break;

      case 45:
      func_string_setter< Meta_List_Field::impl, Meta_Field >( *this, &Meta_List_Field::impl::impl_Source_Field, value );
      break;

      case 46:
      func_string_setter< Meta_List_Field::impl, Meta_Field >( *this, &Meta_List_Field::impl::impl_Source_Grandchild, value );
      break;

      case 47:
      func_string_setter< Meta_List_Field::impl, Meta_Field >( *this, &Meta_List_Field::impl::impl_Source_Parent, value );
      break;

      case 48:
      func_string_setter< Meta_List_Field::impl, Meta_Class >( *this, &Meta_List_Field::impl::impl_Source_Parent_Class, value );
      break;

      case 49:
      func_string_setter< Meta_List_Field::impl, int >( *this, &Meta_List_Field::impl::impl_Switch_Type, value );
      break;

      case 50:
      func_string_setter< Meta_List_Field::impl, int >( *this, &Meta_List_Field::impl::impl_Trigger_Option, value );
      break;

      case 51:
      func_string_setter< Meta_List_Field::impl, Meta_List_Field_Type >( *this, &Meta_List_Field::impl::impl_Type, value );
      break;

      case 52:
      func_string_setter< Meta_List_Field::impl, bool >( *this, &Meta_List_Field::impl::impl_Use_Child_Rel_Source_Parent, value );
      break;

      case 53:
      func_string_setter< Meta_List_Field::impl, bool >( *this, &Meta_List_Field::impl::impl_Use_In_Text_Search_Title, value );
      break;

      case 54:
      func_string_setter< Meta_List_Field::impl, bool >( *this, &Meta_List_Field::impl::impl_Use_Source_Parent, value );
      break;

      case 55:
      func_string_setter< Meta_List_Field::impl, Meta_Field >( *this, &Meta_List_Field::impl::impl_Use_Type_Field, value );
      break;

      case 56:
      func_string_setter< Meta_List_Field::impl, int >( *this, &Meta_List_Field::impl::impl_View_Parent_Extra, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_List_Field::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Access_Parent_Modifier( g_default_Access_Parent_Modifier );
      break;

      case 1:
      impl_Access_Permission( g_default_Access_Permission );
      break;

      case 2:
      impl_Access_Restriction( g_default_Access_Restriction );
      break;

      case 3:
      impl_Alignment( g_default_Alignment );
      break;

      case 4:
      impl_Allow_Anonymous_Access( g_default_Allow_Anonymous_Access );
      break;

      case 5:
      impl_Child_Rel_Child_Class( g_default_Child_Rel_Child_Class );
      break;

      case 6:
      impl_Child_Rel_Source_Child( g_default_Child_Rel_Source_Child );
      break;

      case 7:
      impl_Child_Rel_Source_Field( g_default_Child_Rel_Source_Field );
      break;

      case 8:
      impl_Child_Rel_Source_Parent( g_default_Child_Rel_Source_Parent );
      break;

      case 9:
      impl_Child_Rel_Source_Parent_Class( g_default_Child_Rel_Source_Parent_Class );
      break;

      case 10:
      impl_Child_Relationship( g_default_Child_Relationship );
      break;

      case 11:
      impl_Class( g_default_Class );
      break;

      case 12:
      impl_Exact_Match_Only( g_default_Exact_Match_Only );
      break;

      case 13:
      impl_Exclude_In_Use_FK( g_default_Exclude_In_Use_FK );
      break;

      case 14:
      impl_Font_Size( g_default_Font_Size );
      break;

      case 15:
      impl_Hide_If_Is_Changing( g_default_Hide_If_Is_Changing );
      break;

      case 16:
      impl_Include_Key_Additions( g_default_Include_Key_Additions );
      break;

      case 17:
      impl_Label_Class( g_default_Label_Class );
      break;

      case 18:
      impl_Label_Source( g_default_Label_Source );
      break;

      case 19:
      impl_Label_Source_Child( g_default_Label_Source_Child );
      break;

      case 20:
      impl_Link_Empty_Restriction( g_default_Link_Empty_Restriction );
      break;

      case 21:
      impl_Link_Permission( g_default_Link_Permission );
      break;

      case 22:
      impl_Link_Restriction( g_default_Link_Restriction );
      break;

      case 23:
      impl_Link_Type( g_default_Link_Type );
      break;

      case 24:
      impl_List( g_default_List );
      break;

      case 25:
      impl_Name( g_default_Name );
      break;

      case 26:
      impl_Non_Instance_Procedure( g_default_Non_Instance_Procedure );
      break;

      case 27:
      impl_Notes_Truncation( g_default_Notes_Truncation );
      break;

      case 28:
      impl_Omit_Versions( g_default_Omit_Versions );
      break;

      case 29:
      impl_Order( g_default_Order );
      break;

      case 30:
      impl_Orientation( g_default_Orientation );
      break;

      case 31:
      impl_Parent_Class( g_default_Parent_Class );
      break;

      case 32:
      impl_Print_Type( g_default_Print_Type );
      break;

      case 33:
      impl_Procedure( g_default_Procedure );
      break;

      case 34:
      impl_Procedure_Args( g_default_Procedure_Args );
      break;

      case 35:
      impl_Restriction_Field( g_default_Restriction_Field );
      break;

      case 36:
      impl_Restriction_Spec( g_default_Restriction_Spec );
      break;

      case 37:
      impl_Restriction_Value( g_default_Restriction_Value );
      break;

      case 38:
      impl_Retain_Selected_Rows( g_default_Retain_Selected_Rows );
      break;

      case 39:
      impl_Reverse_Order( g_default_Reverse_Order );
      break;

      case 40:
      impl_Search_Option_Limit( g_default_Search_Option_Limit );
      break;

      case 41:
      impl_Select_Key_Exclusions( g_default_Select_Key_Exclusions );
      break;

      case 42:
      impl_Sort_Manually( g_default_Sort_Manually );
      break;

      case 43:
      impl_Source_Child( g_default_Source_Child );
      break;

      case 44:
      impl_Source_Child_Class( g_default_Source_Child_Class );
      break;

      case 45:
      impl_Source_Field( g_default_Source_Field );
      break;

      case 46:
      impl_Source_Grandchild( g_default_Source_Grandchild );
      break;

      case 47:
      impl_Source_Parent( g_default_Source_Parent );
      break;

      case 48:
      impl_Source_Parent_Class( g_default_Source_Parent_Class );
      break;

      case 49:
      impl_Switch_Type( g_default_Switch_Type );
      break;

      case 50:
      impl_Trigger_Option( g_default_Trigger_Option );
      break;

      case 51:
      impl_Type( g_default_Type );
      break;

      case 52:
      impl_Use_Child_Rel_Source_Parent( g_default_Use_Child_Rel_Source_Parent );
      break;

      case 53:
      impl_Use_In_Text_Search_Title( g_default_Use_In_Text_Search_Title );
      break;

      case 54:
      impl_Use_Source_Parent( g_default_Use_Source_Parent );
      break;

      case 55:
      impl_Use_Type_Field( g_default_Use_Type_Field );
      break;

      case 56:
      impl_View_Parent_Extra( g_default_View_Parent_Extra );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_List_Field::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Access_Parent_Modifier == g_default_Access_Parent_Modifier );
      break;

      case 1:
      retval = ( v_Access_Permission == g_default_Access_Permission );
      break;

      case 2:
      retval = ( v_Access_Restriction == g_default_Access_Restriction );
      break;

      case 3:
      retval = ( v_Alignment == g_default_Alignment );
      break;

      case 4:
      retval = ( v_Allow_Anonymous_Access == g_default_Allow_Anonymous_Access );
      break;

      case 5:
      retval = ( v_Child_Rel_Child_Class == g_default_Child_Rel_Child_Class );
      break;

      case 6:
      retval = ( v_Child_Rel_Source_Child == g_default_Child_Rel_Source_Child );
      break;

      case 7:
      retval = ( v_Child_Rel_Source_Field == g_default_Child_Rel_Source_Field );
      break;

      case 8:
      retval = ( v_Child_Rel_Source_Parent == g_default_Child_Rel_Source_Parent );
      break;

      case 9:
      retval = ( v_Child_Rel_Source_Parent_Class == g_default_Child_Rel_Source_Parent_Class );
      break;

      case 10:
      retval = ( v_Child_Relationship == g_default_Child_Relationship );
      break;

      case 11:
      retval = ( v_Class == g_default_Class );
      break;

      case 12:
      retval = ( v_Exact_Match_Only == g_default_Exact_Match_Only );
      break;

      case 13:
      retval = ( v_Exclude_In_Use_FK == g_default_Exclude_In_Use_FK );
      break;

      case 14:
      retval = ( v_Font_Size == g_default_Font_Size );
      break;

      case 15:
      retval = ( v_Hide_If_Is_Changing == g_default_Hide_If_Is_Changing );
      break;

      case 16:
      retval = ( v_Include_Key_Additions == g_default_Include_Key_Additions );
      break;

      case 17:
      retval = ( v_Label_Class == g_default_Label_Class );
      break;

      case 18:
      retval = ( v_Label_Source == g_default_Label_Source );
      break;

      case 19:
      retval = ( v_Label_Source_Child == g_default_Label_Source_Child );
      break;

      case 20:
      retval = ( v_Link_Empty_Restriction == g_default_Link_Empty_Restriction );
      break;

      case 21:
      retval = ( v_Link_Permission == g_default_Link_Permission );
      break;

      case 22:
      retval = ( v_Link_Restriction == g_default_Link_Restriction );
      break;

      case 23:
      retval = ( v_Link_Type == g_default_Link_Type );
      break;

      case 24:
      retval = ( v_List == g_default_List );
      break;

      case 25:
      retval = ( v_Name == g_default_Name );
      break;

      case 26:
      retval = ( v_Non_Instance_Procedure == g_default_Non_Instance_Procedure );
      break;

      case 27:
      retval = ( v_Notes_Truncation == g_default_Notes_Truncation );
      break;

      case 28:
      retval = ( v_Omit_Versions == g_default_Omit_Versions );
      break;

      case 29:
      retval = ( v_Order == g_default_Order );
      break;

      case 30:
      retval = ( v_Orientation == g_default_Orientation );
      break;

      case 31:
      retval = ( v_Parent_Class == g_default_Parent_Class );
      break;

      case 32:
      retval = ( v_Print_Type == g_default_Print_Type );
      break;

      case 33:
      retval = ( v_Procedure == g_default_Procedure );
      break;

      case 34:
      retval = ( v_Procedure_Args == g_default_Procedure_Args );
      break;

      case 35:
      retval = ( v_Restriction_Field == g_default_Restriction_Field );
      break;

      case 36:
      retval = ( v_Restriction_Spec == g_default_Restriction_Spec );
      break;

      case 37:
      retval = ( v_Restriction_Value == g_default_Restriction_Value );
      break;

      case 38:
      retval = ( v_Retain_Selected_Rows == g_default_Retain_Selected_Rows );
      break;

      case 39:
      retval = ( v_Reverse_Order == g_default_Reverse_Order );
      break;

      case 40:
      retval = ( v_Search_Option_Limit == g_default_Search_Option_Limit );
      break;

      case 41:
      retval = ( v_Select_Key_Exclusions == g_default_Select_Key_Exclusions );
      break;

      case 42:
      retval = ( v_Sort_Manually == g_default_Sort_Manually );
      break;

      case 43:
      retval = ( v_Source_Child == g_default_Source_Child );
      break;

      case 44:
      retval = ( v_Source_Child_Class == g_default_Source_Child_Class );
      break;

      case 45:
      retval = ( v_Source_Field == g_default_Source_Field );
      break;

      case 46:
      retval = ( v_Source_Grandchild == g_default_Source_Grandchild );
      break;

      case 47:
      retval = ( v_Source_Parent == g_default_Source_Parent );
      break;

      case 48:
      retval = ( v_Source_Parent_Class == g_default_Source_Parent_Class );
      break;

      case 49:
      retval = ( v_Switch_Type == g_default_Switch_Type );
      break;

      case 50:
      retval = ( v_Trigger_Option == g_default_Trigger_Option );
      break;

      case 51:
      retval = ( v_Type == g_default_Type );
      break;

      case 52:
      retval = ( v_Use_Child_Rel_Source_Parent == g_default_Use_Child_Rel_Source_Parent );
      break;

      case 53:
      retval = ( v_Use_In_Text_Search_Title == g_default_Use_In_Text_Search_Title );
      break;

      case 54:
      retval = ( v_Use_Source_Parent == g_default_Use_Source_Parent );
      break;

      case 55:
      retval = ( v_Use_Type_Field == g_default_Use_Type_Field );
      break;

      case 56:
      retval = ( v_View_Parent_Extra == g_default_View_Parent_Extra );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_List_Field::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start modifier_field_value)] 600460
   if( get_obj( ).Use_Source_Parent( ) == false )
      state |= c_modifier_Hide_FK_Fields;
   // [(finish modifier_field_value)] 600460

   // [(start modifier_field_value)] 600461
   if( get_obj( ).Use_Source_Parent( ) == true )
      state |= c_modifier_Hide_Non_FK_Field;
   // [(finish modifier_field_value)] 600461

   // [(start modifier_field_value)] 600471
   if( get_obj( ).Type( ).Needs_Switch_Type( ) == false )
      state |= c_modifier_Hide_Switch_Type;
   // [(finish modifier_field_value)] 600471

   // [(start modifier_field_value)] 600472
   if( get_obj( ).Type( ).Allow_Restriction_Spec( ) == false )
      state |= c_modifier_Hide_Restriction_Spec;
   // [(finish modifier_field_value)] 600472

   // [(start modifier_field_value)] 600473
   if( get_obj( ).Type( ).Allow_Restriction_Value( ) == false )
      state |= c_modifier_Hide_Restriction_Value;
   // [(finish modifier_field_value)] 600473

   // [(start modifier_field_value)] 600474
   if( get_obj( ).Type( ).Allow_Exclude_In_Use_FK( ) == false )
      state |= c_modifier_Hide_Exclude_In_Use_FK;
   // [(finish modifier_field_value)] 600474

   // [(start modifier_field_value)] 600475
   if( get_obj( ).Type( ).Allow_Link_Type( ) == false )
      state |= c_modifier_Hide_Link_Type;
   // [(finish modifier_field_value)] 600475

   // [(start modifier_field_value)] 600476
   if( get_obj( ).Type( ).Allow_Procedure( ) == false )
      state |= c_modifier_Hide_Procedure_Fields;
   // [(finish modifier_field_value)] 600476

   // [(start modifier_field_value)] 600477
   if( get_obj( ).Type( ).Allow_Procedure( ) == true )
      state |= c_modifier_Hide_Non_Procedure_Fields;
   // [(finish modifier_field_value)] 600477

   // [(start modifier_field_value)] 600478
   if( get_obj( ).Type( ).Non_Simple_Field( ) == true )
      state |= c_modifier_Hide_Non_Simple_Fields;
   // [(finish modifier_field_value)] 600478

   // [(start modifier_field_value)] 600479
   if( get_obj( ).Type( ).Trivial_Field_Only( ) == true )
      state |= c_modifier_Hide_Fields_For_Trivial_Field;
   // [(finish modifier_field_value)] 600479

   // [(start modifier_field_value)] 600480
   if( get_obj( ).Type( ).Allow_Select_Specifics( ) == false )
      state |= c_modifier_Hide_Select_Specifics;
   // [(finish modifier_field_value)] 600480

   // [(start modifier_field_value)] 600481
   if( get_obj( ).Type( ).Allow_Link_Specifics( ) == false )
      state |= c_modifier_Hide_Link_Specifics;
   // [(finish modifier_field_value)] 600481

   // [(start modifier_field_value)] 600481a
   if( get_obj( ).Type( ).Allow_Link_Empty_Restriction( ) == false )
      state |= c_modifier_Hide_Link_Empty_Restriction;
   // [(finish modifier_field_value)] 600481a

   // [(start modifier_field_value)] 600481b
   if( get_obj( ).Type( ).Allow_Sort_Manually( ) == false )
      state |= c_modifier_Hide_Sort_Manually;
   // [(finish modifier_field_value)] 600481b

   // [(start modifier_field_value)] 600482
   if( get_obj( ).Use_Child_Rel_Source_Parent( ) == false )
      state |= c_modifier_Hide_Child_Rel_FK_Fields;
   // [(finish modifier_field_value)] 600482

   // [(start modifier_field_value)] 600483
   if( get_obj( ).Use_Child_Rel_Source_Parent( ) == true )
      state |= c_modifier_Hide_Child_Rel_Non_FK_Field;
   // [(finish modifier_field_value)] 600483

   // [(start modifier_field_value)] 600486
   if( get_obj( ).Type( ).Allow_Child_Rel_Select_Specifics( ) == false )
      state |= c_modifier_Hide_Child_Rel_Select_Specifics;
   // [(finish modifier_field_value)] 600486

   // [(start modifier_field_value)] 600487
   if( get_obj( ).Type( ).Allow_Child_Rel_Select_Specifics( ) == true )
      state |= c_modifier_Hide_For_Child_Rel_Select;
   // [(finish modifier_field_value)] 600487

   // [(start modifier_field_value)] 600488
   if( get_obj( ).Type( ).Allow_Search_Option_Limit( ) == false )
      state |= c_modifier_Hide_Search_Option_Limit;
   // [(finish modifier_field_value)] 600488

   // [(start modifier_field_value)] 600489
   if( get_obj( ).Type( ).Allow_Restriction_Field( ) == false )
      state |= c_modifier_Hide_Restriction_Field;
   // [(finish modifier_field_value)] 600489

   // [(start modifier_field_value)] 600490
   if( get_obj( ).Type( ).Allow_Restriction_Field( ) == true )
      state |= c_modifier_Hide_For_Restriction_Field;
   // [(finish modifier_field_value)] 600490

   // [(start modifier_field_null)] 600495
   if( is_null( get_obj( ).Parent_Class( ) ) )
      state |= c_modifier_Is_Not_View_Child;
   // [(finish modifier_field_null)] 600495

   // [(start modifier_field_value)] 600496
   if( get_obj( ).Type( ).Is_Restrict_Search( ) == true )
      state |= c_modifier_Is_Restrict_Search;
   // [(finish modifier_field_value)] 600496

   // [(start modifier_field_value)] 600496a
   if( get_obj( ).Type( ).Is_Restrict_Search( ) == false )
      state |= c_modifier_Is_Not_Restrict_Search;
   // [(finish modifier_field_value)] 600496a

   // [(start modifier_field_value)] 600498
   if( get_obj( ).Type( ).Allow_Link_Permission( ) == false )
      state |= c_modifier_Hide_Link_Permission_Field;
   // [(finish modifier_field_value)] 600498

   // [(start modifier_field_value)] 600499
   if( get_obj( ).Type( ).Allow_View_Parent_Extra( ) == false )
      state |= c_modifier_Hide_View_Parent_Extra;
   // [(finish modifier_field_value)] 600499

   // [(start modifier_field_value)] 600499a
   if( get_obj( ).Non_Instance_Procedure( ) == true )
      state |= c_modifier_Is_Non_Instance_Procedure;
   // [(finish modifier_field_value)] 600499a

   // [<start get_state>]
//nyi
   if( get_obj( ).Procedure( ).Access_Restriction( ) != 0
    || !is_null( get_obj( ).Procedure( ).Access_Permission( ) )
    || get_obj( ).Source_Field( ).Access_Restriction( ) != 0
    || !is_null( get_obj( ).Source_Field( ).Access_Permission( ) )
    || get_obj( ).Source_Parent( ).Access_Restriction( ) != 0
    || !is_null( get_obj( ).Source_Parent( ).Access_Permission( ) ) )
      state |= c_modifier_Protect_Access;

   if( get_obj( ).Source_Field( ).Extra( ) != 3 ) // i.e. image
      state |= c_modifier_Is_Not_Image;

   if( get_obj( ).Source_Field( ).Extra( ) == 1 // i.e. file
    || get_obj( ).Source_Field( ).Extra( ) == 3 ) // i.e. image
      state |= c_modifier_Is_File_Or_Image;
   // [<finish get_state>]

   return state;
}

string Meta_List_Field::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   if( state & c_modifier_Hide_Child_Rel_FK_Fields )
      state_names += "|" + string( "Hide_Child_Rel_FK_Fields" );
   if( state & c_modifier_Hide_Child_Rel_Non_FK_Field )
      state_names += "|" + string( "Hide_Child_Rel_Non_FK_Field" );
   if( state & c_modifier_Hide_Child_Rel_Select_Specifics )
      state_names += "|" + string( "Hide_Child_Rel_Select_Specifics" );
   if( state & c_modifier_Hide_Exclude_In_Use_FK )
      state_names += "|" + string( "Hide_Exclude_In_Use_FK" );
   if( state & c_modifier_Hide_FK_Fields )
      state_names += "|" + string( "Hide_FK_Fields" );
   if( state & c_modifier_Hide_Fields_For_Trivial_Field )
      state_names += "|" + string( "Hide_Fields_For_Trivial_Field" );
   if( state & c_modifier_Hide_For_Child_Rel_Select )
      state_names += "|" + string( "Hide_For_Child_Rel_Select" );
   if( state & c_modifier_Hide_For_Restriction_Field )
      state_names += "|" + string( "Hide_For_Restriction_Field" );
   if( state & c_modifier_Hide_Link_Empty_Restriction )
      state_names += "|" + string( "Hide_Link_Empty_Restriction" );
   if( state & c_modifier_Hide_Link_Permission_Field )
      state_names += "|" + string( "Hide_Link_Permission_Field" );
   if( state & c_modifier_Hide_Link_Specifics )
      state_names += "|" + string( "Hide_Link_Specifics" );
   if( state & c_modifier_Hide_Link_Type )
      state_names += "|" + string( "Hide_Link_Type" );
   if( state & c_modifier_Hide_Non_FK_Field )
      state_names += "|" + string( "Hide_Non_FK_Field" );
   if( state & c_modifier_Hide_Non_Procedure_Fields )
      state_names += "|" + string( "Hide_Non_Procedure_Fields" );
   if( state & c_modifier_Hide_Non_Simple_Fields )
      state_names += "|" + string( "Hide_Non_Simple_Fields" );
   if( state & c_modifier_Hide_Procedure_Fields )
      state_names += "|" + string( "Hide_Procedure_Fields" );
   if( state & c_modifier_Hide_Restriction_Field )
      state_names += "|" + string( "Hide_Restriction_Field" );
   if( state & c_modifier_Hide_Restriction_Spec )
      state_names += "|" + string( "Hide_Restriction_Spec" );
   if( state & c_modifier_Hide_Restriction_Value )
      state_names += "|" + string( "Hide_Restriction_Value" );
   if( state & c_modifier_Hide_Search_Option_Limit )
      state_names += "|" + string( "Hide_Search_Option_Limit" );
   if( state & c_modifier_Hide_Select_Specifics )
      state_names += "|" + string( "Hide_Select_Specifics" );
   if( state & c_modifier_Hide_Sort_Manually )
      state_names += "|" + string( "Hide_Sort_Manually" );
   if( state & c_modifier_Hide_Switch_Type )
      state_names += "|" + string( "Hide_Switch_Type" );
   if( state & c_modifier_Hide_View_Parent_Extra )
      state_names += "|" + string( "Hide_View_Parent_Extra" );
   if( state & c_modifier_Is_File_Or_Image )
      state_names += "|" + string( "Is_File_Or_Image" );
   if( state & c_modifier_Is_Non_Instance_Procedure )
      state_names += "|" + string( "Is_Non_Instance_Procedure" );
   if( state & c_modifier_Is_Not_Image )
      state_names += "|" + string( "Is_Not_Image" );
   if( state & c_modifier_Is_Not_Restrict_Search )
      state_names += "|" + string( "Is_Not_Restrict_Search" );
   if( state & c_modifier_Is_Not_View_Child )
      state_names += "|" + string( "Is_Not_View_Child" );
   if( state & c_modifier_Is_Restrict_Search )
      state_names += "|" + string( "Is_Restrict_Search" );
   if( state & c_modifier_Protect_Access )
      state_names += "|" + string( "Protect_Access" );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_List_Field::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_List_Field::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Access_Parent_Modifier ) || ( field == c_field_name_Access_Parent_Modifier ) )
      impl_Access_Parent_Modifier( "" );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      impl_Access_Permission( "" );
   else if( ( field == c_field_id_Child_Rel_Child_Class ) || ( field == c_field_name_Child_Rel_Child_Class ) )
      impl_Child_Rel_Child_Class( "" );
   else if( ( field == c_field_id_Child_Rel_Source_Child ) || ( field == c_field_name_Child_Rel_Source_Child ) )
      impl_Child_Rel_Source_Child( "" );
   else if( ( field == c_field_id_Child_Rel_Source_Field ) || ( field == c_field_name_Child_Rel_Source_Field ) )
      impl_Child_Rel_Source_Field( "" );
   else if( ( field == c_field_id_Child_Rel_Source_Parent ) || ( field == c_field_name_Child_Rel_Source_Parent ) )
      impl_Child_Rel_Source_Parent( "" );
   else if( ( field == c_field_id_Child_Rel_Source_Parent_Class ) || ( field == c_field_name_Child_Rel_Source_Parent_Class ) )
      impl_Child_Rel_Source_Parent_Class( "" );
   else if( ( field == c_field_id_Child_Relationship ) || ( field == c_field_name_Child_Relationship ) )
      impl_Child_Relationship( "" );
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      impl_Class( "" );
   else if( ( field == c_field_id_Link_Permission ) || ( field == c_field_name_Link_Permission ) )
      impl_Link_Permission( "" );
   else if( ( field == c_field_id_List ) || ( field == c_field_name_List ) )
      impl_List( "" );
   else if( ( field == c_field_id_Parent_Class ) || ( field == c_field_name_Parent_Class ) )
      impl_Parent_Class( "" );
   else if( ( field == c_field_id_Procedure ) || ( field == c_field_name_Procedure ) )
      impl_Procedure( "" );
   else if( ( field == c_field_id_Restriction_Field ) || ( field == c_field_name_Restriction_Field ) )
      impl_Restriction_Field( "" );
   else if( ( field == c_field_id_Restriction_Spec ) || ( field == c_field_name_Restriction_Spec ) )
      impl_Restriction_Spec( "" );
   else if( ( field == c_field_id_Source_Child ) || ( field == c_field_name_Source_Child ) )
      impl_Source_Child( "" );
   else if( ( field == c_field_id_Source_Child_Class ) || ( field == c_field_name_Source_Child_Class ) )
      impl_Source_Child_Class( "" );
   else if( ( field == c_field_id_Source_Field ) || ( field == c_field_name_Source_Field ) )
      impl_Source_Field( "" );
   else if( ( field == c_field_id_Source_Grandchild ) || ( field == c_field_name_Source_Grandchild ) )
      impl_Source_Grandchild( "" );
   else if( ( field == c_field_id_Source_Parent ) || ( field == c_field_name_Source_Parent ) )
      impl_Source_Parent( "" );
   else if( ( field == c_field_id_Source_Parent_Class ) || ( field == c_field_name_Source_Parent_Class ) )
      impl_Source_Parent_Class( "" );
   else if( ( field == c_field_id_Type ) || ( field == c_field_name_Type ) )
      impl_Type( "" );
   else if( ( field == c_field_id_Use_Type_Field ) || ( field == c_field_name_Use_Type_Field ) )
      impl_Use_Type_Field( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_List_Field::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_Access_Parent_Modifier ) || ( field == c_field_name_Access_Parent_Modifier ) )
      v_Access_Parent_Modifier = value;
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      v_Access_Permission = value;
   else if( ( field == c_field_id_Child_Rel_Child_Class ) || ( field == c_field_name_Child_Rel_Child_Class ) )
      v_Child_Rel_Child_Class = value;
   else if( ( field == c_field_id_Child_Rel_Source_Child ) || ( field == c_field_name_Child_Rel_Source_Child ) )
      v_Child_Rel_Source_Child = value;
   else if( ( field == c_field_id_Child_Rel_Source_Field ) || ( field == c_field_name_Child_Rel_Source_Field ) )
      v_Child_Rel_Source_Field = value;
   else if( ( field == c_field_id_Child_Rel_Source_Parent ) || ( field == c_field_name_Child_Rel_Source_Parent ) )
      v_Child_Rel_Source_Parent = value;
   else if( ( field == c_field_id_Child_Rel_Source_Parent_Class ) || ( field == c_field_name_Child_Rel_Source_Parent_Class ) )
      v_Child_Rel_Source_Parent_Class = value;
   else if( ( field == c_field_id_Child_Relationship ) || ( field == c_field_name_Child_Relationship ) )
      v_Child_Relationship = value;
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      v_Class = value;
   else if( ( field == c_field_id_Link_Permission ) || ( field == c_field_name_Link_Permission ) )
      v_Link_Permission = value;
   else if( ( field == c_field_id_List ) || ( field == c_field_name_List ) )
      v_List = value;
   else if( ( field == c_field_id_Parent_Class ) || ( field == c_field_name_Parent_Class ) )
      v_Parent_Class = value;
   else if( ( field == c_field_id_Procedure ) || ( field == c_field_name_Procedure ) )
      v_Procedure = value;
   else if( ( field == c_field_id_Restriction_Field ) || ( field == c_field_name_Restriction_Field ) )
      v_Restriction_Field = value;
   else if( ( field == c_field_id_Restriction_Spec ) || ( field == c_field_name_Restriction_Spec ) )
      v_Restriction_Spec = value;
   else if( ( field == c_field_id_Source_Child ) || ( field == c_field_name_Source_Child ) )
      v_Source_Child = value;
   else if( ( field == c_field_id_Source_Child_Class ) || ( field == c_field_name_Source_Child_Class ) )
      v_Source_Child_Class = value;
   else if( ( field == c_field_id_Source_Field ) || ( field == c_field_name_Source_Field ) )
      v_Source_Field = value;
   else if( ( field == c_field_id_Source_Grandchild ) || ( field == c_field_name_Source_Grandchild ) )
      v_Source_Grandchild = value;
   else if( ( field == c_field_id_Source_Parent ) || ( field == c_field_name_Source_Parent ) )
      v_Source_Parent = value;
   else if( ( field == c_field_id_Source_Parent_Class ) || ( field == c_field_name_Source_Parent_Class ) )
      v_Source_Parent_Class = value;
   else if( ( field == c_field_id_Type ) || ( field == c_field_name_Type ) )
      v_Type = value;
   else if( ( field == c_field_id_Use_Type_Field ) || ( field == c_field_name_Use_Type_Field ) )
      v_Use_Type_Field = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_List_Field::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Access_Parent_Modifier ) || ( field == c_field_name_Access_Parent_Modifier ) )
      return v_Access_Parent_Modifier;
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      return v_Access_Permission;
   else if( ( field == c_field_id_Child_Rel_Child_Class ) || ( field == c_field_name_Child_Rel_Child_Class ) )
      return v_Child_Rel_Child_Class;
   else if( ( field == c_field_id_Child_Rel_Source_Child ) || ( field == c_field_name_Child_Rel_Source_Child ) )
      return v_Child_Rel_Source_Child;
   else if( ( field == c_field_id_Child_Rel_Source_Field ) || ( field == c_field_name_Child_Rel_Source_Field ) )
      return v_Child_Rel_Source_Field;
   else if( ( field == c_field_id_Child_Rel_Source_Parent ) || ( field == c_field_name_Child_Rel_Source_Parent ) )
      return v_Child_Rel_Source_Parent;
   else if( ( field == c_field_id_Child_Rel_Source_Parent_Class ) || ( field == c_field_name_Child_Rel_Source_Parent_Class ) )
      return v_Child_Rel_Source_Parent_Class;
   else if( ( field == c_field_id_Child_Relationship ) || ( field == c_field_name_Child_Relationship ) )
      return v_Child_Relationship;
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      return v_Class;
   else if( ( field == c_field_id_Link_Permission ) || ( field == c_field_name_Link_Permission ) )
      return v_Link_Permission;
   else if( ( field == c_field_id_List ) || ( field == c_field_name_List ) )
      return v_List;
   else if( ( field == c_field_id_Parent_Class ) || ( field == c_field_name_Parent_Class ) )
      return v_Parent_Class;
   else if( ( field == c_field_id_Procedure ) || ( field == c_field_name_Procedure ) )
      return v_Procedure;
   else if( ( field == c_field_id_Restriction_Field ) || ( field == c_field_name_Restriction_Field ) )
      return v_Restriction_Field;
   else if( ( field == c_field_id_Restriction_Spec ) || ( field == c_field_name_Restriction_Spec ) )
      return v_Restriction_Spec;
   else if( ( field == c_field_id_Source_Child ) || ( field == c_field_name_Source_Child ) )
      return v_Source_Child;
   else if( ( field == c_field_id_Source_Child_Class ) || ( field == c_field_name_Source_Child_Class ) )
      return v_Source_Child_Class;
   else if( ( field == c_field_id_Source_Field ) || ( field == c_field_name_Source_Field ) )
      return v_Source_Field;
   else if( ( field == c_field_id_Source_Grandchild ) || ( field == c_field_name_Source_Grandchild ) )
      return v_Source_Grandchild;
   else if( ( field == c_field_id_Source_Parent ) || ( field == c_field_name_Source_Parent ) )
      return v_Source_Parent;
   else if( ( field == c_field_id_Source_Parent_Class ) || ( field == c_field_name_Source_Parent_Class ) )
      return v_Source_Parent_Class;
   else if( ( field == c_field_id_Type ) || ( field == c_field_name_Type ) )
      return v_Type;
   else if( ( field == c_field_id_Use_Type_Field ) || ( field == c_field_name_Use_Type_Field ) )
      return v_Use_Type_Field;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_List_Field::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Access_Parent_Modifier, v_Access_Parent_Modifier ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Access_Permission, v_Access_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Child_Rel_Child_Class, v_Child_Rel_Child_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Child_Rel_Source_Child, v_Child_Rel_Source_Child ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Child_Rel_Source_Field, v_Child_Rel_Source_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Child_Rel_Source_Parent, v_Child_Rel_Source_Parent ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Child_Rel_Source_Parent_Class, v_Child_Rel_Source_Parent_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Child_Relationship, v_Child_Relationship ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Class, v_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Link_Permission, v_Link_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_List, v_List ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Parent_Class, v_Parent_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Procedure, v_Procedure ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Restriction_Field, v_Restriction_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Restriction_Spec, v_Restriction_Spec ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Child, v_Source_Child ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Child_Class, v_Source_Child_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Field, v_Source_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Grandchild, v_Source_Grandchild ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Parent, v_Source_Parent ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Parent_Class, v_Source_Parent_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Type, v_Type ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Use_Type_Field, v_Use_Type_Field ) );
}

void Meta_List_Field::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_List_Field::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_List_Field::impl::clear( )
{
   v_Access_Restriction = g_default_Access_Restriction;
   v_Alignment = g_default_Alignment;
   v_Allow_Anonymous_Access = g_default_Allow_Anonymous_Access;
   v_Exact_Match_Only = g_default_Exact_Match_Only;
   v_Exclude_In_Use_FK = g_default_Exclude_In_Use_FK;
   v_Font_Size = g_default_Font_Size;
   v_Hide_If_Is_Changing = g_default_Hide_If_Is_Changing;
   v_Include_Key_Additions = g_default_Include_Key_Additions;
   v_Label_Class = g_default_Label_Class;
   v_Label_Source = g_default_Label_Source;
   v_Label_Source_Child = g_default_Label_Source_Child;
   v_Link_Empty_Restriction = g_default_Link_Empty_Restriction;
   v_Link_Restriction = g_default_Link_Restriction;
   v_Link_Type = g_default_Link_Type;
   v_Name = g_default_Name;
   v_Non_Instance_Procedure = g_default_Non_Instance_Procedure;
   v_Notes_Truncation = g_default_Notes_Truncation;
   v_Omit_Versions = g_default_Omit_Versions;
   v_Order = g_default_Order;
   v_Orientation = g_default_Orientation;
   v_Print_Type = g_default_Print_Type;
   v_Procedure_Args = g_default_Procedure_Args;
   v_Restriction_Value = g_default_Restriction_Value;
   v_Retain_Selected_Rows = g_default_Retain_Selected_Rows;
   v_Reverse_Order = g_default_Reverse_Order;
   v_Search_Option_Limit = g_default_Search_Option_Limit;
   v_Select_Key_Exclusions = g_default_Select_Key_Exclusions;
   v_Sort_Manually = g_default_Sort_Manually;
   v_Switch_Type = g_default_Switch_Type;
   v_Trigger_Option = g_default_Trigger_Option;
   v_Use_Child_Rel_Source_Parent = g_default_Use_Child_Rel_Source_Parent;
   v_Use_In_Text_Search_Title = g_default_Use_In_Text_Search_Title;
   v_Use_Source_Parent = g_default_Use_Source_Parent;
   v_View_Parent_Extra = g_default_View_Parent_Extra;

   v_Access_Parent_Modifier = string( );
   if( cp_Access_Parent_Modifier )
      p_obj->setup_foreign_key( *cp_Access_Parent_Modifier, v_Access_Parent_Modifier );

   v_Access_Permission = string( );
   if( cp_Access_Permission )
      p_obj->setup_foreign_key( *cp_Access_Permission, v_Access_Permission );

   v_Child_Rel_Child_Class = string( );
   if( cp_Child_Rel_Child_Class )
      p_obj->setup_foreign_key( *cp_Child_Rel_Child_Class, v_Child_Rel_Child_Class );

   v_Child_Rel_Source_Child = string( );
   if( cp_Child_Rel_Source_Child )
      p_obj->setup_foreign_key( *cp_Child_Rel_Source_Child, v_Child_Rel_Source_Child );

   v_Child_Rel_Source_Field = string( );
   if( cp_Child_Rel_Source_Field )
      p_obj->setup_foreign_key( *cp_Child_Rel_Source_Field, v_Child_Rel_Source_Field );

   v_Child_Rel_Source_Parent = string( );
   if( cp_Child_Rel_Source_Parent )
      p_obj->setup_foreign_key( *cp_Child_Rel_Source_Parent, v_Child_Rel_Source_Parent );

   v_Child_Rel_Source_Parent_Class = string( );
   if( cp_Child_Rel_Source_Parent_Class )
      p_obj->setup_foreign_key( *cp_Child_Rel_Source_Parent_Class, v_Child_Rel_Source_Parent_Class );

   v_Child_Relationship = string( );
   if( cp_Child_Relationship )
      p_obj->setup_foreign_key( *cp_Child_Relationship, v_Child_Relationship );

   v_Class = string( );
   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   v_Link_Permission = string( );
   if( cp_Link_Permission )
      p_obj->setup_foreign_key( *cp_Link_Permission, v_Link_Permission );

   v_List = string( );
   if( cp_List )
      p_obj->setup_foreign_key( *cp_List, v_List );

   v_Parent_Class = string( );
   if( cp_Parent_Class )
      p_obj->setup_foreign_key( *cp_Parent_Class, v_Parent_Class );

   v_Procedure = string( );
   if( cp_Procedure )
      p_obj->setup_foreign_key( *cp_Procedure, v_Procedure );

   v_Restriction_Field = string( );
   if( cp_Restriction_Field )
      p_obj->setup_foreign_key( *cp_Restriction_Field, v_Restriction_Field );

   v_Restriction_Spec = string( );
   if( cp_Restriction_Spec )
      p_obj->setup_foreign_key( *cp_Restriction_Spec, v_Restriction_Spec );

   v_Source_Child = string( );
   if( cp_Source_Child )
      p_obj->setup_foreign_key( *cp_Source_Child, v_Source_Child );

   v_Source_Child_Class = string( );
   if( cp_Source_Child_Class )
      p_obj->setup_foreign_key( *cp_Source_Child_Class, v_Source_Child_Class );

   v_Source_Field = string( );
   if( cp_Source_Field )
      p_obj->setup_foreign_key( *cp_Source_Field, v_Source_Field );

   v_Source_Grandchild = string( );
   if( cp_Source_Grandchild )
      p_obj->setup_foreign_key( *cp_Source_Grandchild, v_Source_Grandchild );

   v_Source_Parent = string( );
   if( cp_Source_Parent )
      p_obj->setup_foreign_key( *cp_Source_Parent, v_Source_Parent );

   v_Source_Parent_Class = string( );
   if( cp_Source_Parent_Class )
      p_obj->setup_foreign_key( *cp_Source_Parent_Class, v_Source_Parent_Class );

   v_Type = string( );
   if( cp_Type )
      p_obj->setup_foreign_key( *cp_Type, v_Type );

   v_Use_Type_Field = string( );
   if( cp_Use_Type_Field )
      p_obj->setup_foreign_key( *cp_Use_Type_Field, v_Use_Type_Field );
}

bool Meta_List_Field::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_List_Field::impl::validate(
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

   if( v_List.empty( ) && !value_will_be_provided( c_field_name_List ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_List,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_List ) ) ) ) );

   if( v_Type.empty( ) && !value_will_be_provided( c_field_name_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Type,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Type ) ) ) ) );

   if( !is_null( v_Include_Key_Additions )
    && ( v_Include_Key_Additions != g_default_Include_Key_Additions
    || !value_will_be_provided( c_field_name_Include_Key_Additions ) )
    && !g_Include_Key_Additions_domain.is_valid( v_Include_Key_Additions, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Include_Key_Additions,
       get_module_string( c_field_display_name_Include_Key_Additions ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( v_Name != g_default_Name
    || !value_will_be_provided( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   if( !is_null( v_Procedure_Args )
    && ( v_Procedure_Args != g_default_Procedure_Args
    || !value_will_be_provided( c_field_name_Procedure_Args ) )
    && !g_Procedure_Args_domain.is_valid( v_Procedure_Args, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Procedure_Args,
       get_module_string( c_field_display_name_Procedure_Args ) + " " + error_message ) );

   if( !is_null( v_Restriction_Value )
    && ( v_Restriction_Value != g_default_Restriction_Value
    || !value_will_be_provided( c_field_name_Restriction_Value ) )
    && !g_Restriction_Value_domain.is_valid( v_Restriction_Value, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Restriction_Value,
       get_module_string( c_field_display_name_Restriction_Value ) + " " + error_message ) );

   if( !is_null( v_Select_Key_Exclusions )
    && ( v_Select_Key_Exclusions != g_default_Select_Key_Exclusions
    || !value_will_be_provided( c_field_name_Select_Key_Exclusions ) )
    && !g_Select_Key_Exclusions_domain.is_valid( v_Select_Key_Exclusions, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Select_Key_Exclusions,
       get_module_string( c_field_display_name_Select_Key_Exclusions ) + " " + error_message ) );

   if( !g_list_field_restrict_enum.count( v_Access_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Access_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Access_Restriction ) ) ) ) );

   if( !g_field_alignment_enum.count( v_Alignment ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Alignment,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Alignment ) ) ) ) );

   if( !g_font_size_enum.count( v_Font_Size ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Font_Size,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Font_Size ) ) ) ) );

   if( !g_list_field_label_class_enum.count( v_Label_Class ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Label_Class,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Label_Class ) ) ) ) );

   if( !g_list_field_label_source_enum.count( v_Label_Source ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Label_Source,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Label_Source ) ) ) ) );

   if( !g_label_source_child_enum.count( v_Label_Source_Child ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Label_Source_Child,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Label_Source_Child ) ) ) ) );

   if( !g_list_field_link_restrict_enum.count( v_Link_Empty_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Link_Empty_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Link_Empty_Restriction ) ) ) ) );

   if( !g_list_field_link_restrict_enum.count( v_Link_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Link_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Link_Restriction ) ) ) ) );

   if( !g_list_field_link_type_enum.count( v_Link_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Link_Type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Link_Type ) ) ) ) );

   if( !g_list_field_notes_truncation_enum.count( v_Notes_Truncation ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Notes_Truncation,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Notes_Truncation ) ) ) ) );

   if( !g_orientation_enum.count( v_Orientation ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Orientation,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Orientation ) ) ) ) );

   if( !g_list_field_print_type_enum.count( v_Print_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Print_Type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Print_Type ) ) ) ) );

   if( !g_list_search_opt_limit_enum.count( v_Search_Option_Limit ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Search_Option_Limit,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Search_Option_Limit ) ) ) ) );

   if( !g_list_field_switch_type_enum.count( v_Switch_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Switch_Type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Switch_Type ) ) ) ) );

   if( !g_list_field_trigger_option_enum.count( v_Trigger_Option ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Trigger_Option,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Trigger_Option ) ) ) ) );

   if( !g_list_field_view_parent_extra_enum.count( v_View_Parent_Extra ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_View_Parent_Extra,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_View_Parent_Extra ) ) ) ) );

   // [(start check_cond_non_null)] 600462
   if( get_obj( ).Use_Source_Parent( ) && is_null( get_obj( ).Source_Parent( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Source_Parent, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Source_Parent ) ) ) ) );
   // [(finish check_cond_non_null)] 600462

   // [(start check_cond_non_null)] 600463
   if( get_obj( ).Use_Source_Parent( ) && is_null( get_obj( ).Source_Child( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Source_Child, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Source_Child ) ) ) ) );
   // [(finish check_cond_non_null)] 600463

   // [(start check_cond_non_null)] 600464
   if( get_obj( ).Type( ).Needs_Source( ) && !get_obj( ).Use_Source_Parent( ) && is_null( get_obj( ).Source_Field( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Source_Field, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Source_Field ) ) ) ) );
   // [(finish check_cond_non_null)] 600464

   // [(start check_cond_non_null)] 600465
   if( get_obj( ).Source_Child( ).Is_Foreign_Key( ) == true && get_obj( ).Use_Source_Parent( ) && is_null( get_obj( ).Source_Grandchild( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Source_Grandchild, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Source_Grandchild ) ) ) ) );
   // [(finish check_cond_non_null)] 600465

   // [(start check_cond_non_null)] 600466
   if( get_obj( ).Use_Child_Rel_Source_Parent( ) && is_null( get_obj( ).Child_Rel_Source_Parent( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Child_Rel_Source_Parent, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Child_Rel_Source_Parent ) ) ) ) );
   // [(finish check_cond_non_null)] 600466

   // [(start check_cond_non_null)] 600467
   if( get_obj( ).Use_Child_Rel_Source_Parent( ) && is_null( get_obj( ).Child_Rel_Source_Child( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Child_Rel_Source_Child, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Child_Rel_Source_Child ) ) ) ) );
   // [(finish check_cond_non_null)] 600467

   // [(start check_cond_non_null)] 600468
   if( get_obj( ).Type( ).Name( ) == "restrict_select_child_rel" && !get_obj( ).Use_Child_Rel_Source_Parent( ) && is_null( get_obj( ).Child_Rel_Source_Field( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Child_Rel_Source_Field, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Child_Rel_Source_Field ) ) ) ) );
   // [(finish check_cond_non_null)] 600468

   // [(start check_cond_non_null)] 600491
   if( get_obj( ).Type( ).Needs_Restriction_Field( ) && is_null( get_obj( ).Restriction_Field( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Restriction_Field, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Restriction_Field ) ) ) ) );
   // [(finish check_cond_non_null)] 600491

   // [<start validate>]
   // [<finish validate>]
}

void Meta_List_Field::impl::validate_set_fields(
 set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( !is_null( v_Include_Key_Additions )
    && ( fields_set.count( c_field_id_Include_Key_Additions ) || fields_set.count( c_field_name_Include_Key_Additions ) )
    && !g_Include_Key_Additions_domain.is_valid( v_Include_Key_Additions, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Include_Key_Additions,
       get_module_string( c_field_display_name_Include_Key_Additions ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( fields_set.count( c_field_id_Name ) || fields_set.count( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   if( !is_null( v_Procedure_Args )
    && ( fields_set.count( c_field_id_Procedure_Args ) || fields_set.count( c_field_name_Procedure_Args ) )
    && !g_Procedure_Args_domain.is_valid( v_Procedure_Args, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Procedure_Args,
       get_module_string( c_field_display_name_Procedure_Args ) + " " + error_message ) );

   if( !is_null( v_Restriction_Value )
    && ( fields_set.count( c_field_id_Restriction_Value ) || fields_set.count( c_field_name_Restriction_Value ) )
    && !g_Restriction_Value_domain.is_valid( v_Restriction_Value, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Restriction_Value,
       get_module_string( c_field_display_name_Restriction_Value ) + " " + error_message ) );

   if( !is_null( v_Select_Key_Exclusions )
    && ( fields_set.count( c_field_id_Select_Key_Exclusions ) || fields_set.count( c_field_name_Select_Key_Exclusions ) )
    && !g_Select_Key_Exclusions_domain.is_valid( v_Select_Key_Exclusions, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Select_Key_Exclusions,
       get_module_string( c_field_display_name_Select_Key_Exclusions ) + " " + error_message ) );
}

void Meta_List_Field::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Access_Parent_Modifier )
      p_obj->setup_foreign_key( *cp_Access_Parent_Modifier, v_Access_Parent_Modifier );

   if( cp_Access_Permission )
      p_obj->setup_foreign_key( *cp_Access_Permission, v_Access_Permission );

   if( cp_Child_Rel_Child_Class )
      p_obj->setup_foreign_key( *cp_Child_Rel_Child_Class, v_Child_Rel_Child_Class );

   if( cp_Child_Rel_Source_Child )
      p_obj->setup_foreign_key( *cp_Child_Rel_Source_Child, v_Child_Rel_Source_Child );

   if( cp_Child_Rel_Source_Field )
      p_obj->setup_foreign_key( *cp_Child_Rel_Source_Field, v_Child_Rel_Source_Field );

   if( cp_Child_Rel_Source_Parent )
      p_obj->setup_foreign_key( *cp_Child_Rel_Source_Parent, v_Child_Rel_Source_Parent );

   if( cp_Child_Rel_Source_Parent_Class )
      p_obj->setup_foreign_key( *cp_Child_Rel_Source_Parent_Class, v_Child_Rel_Source_Parent_Class );

   if( cp_Child_Relationship )
      p_obj->setup_foreign_key( *cp_Child_Relationship, v_Child_Relationship );

   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   if( cp_Link_Permission )
      p_obj->setup_foreign_key( *cp_Link_Permission, v_Link_Permission );

   if( cp_List )
      p_obj->setup_foreign_key( *cp_List, v_List );

   if( cp_Parent_Class )
      p_obj->setup_foreign_key( *cp_Parent_Class, v_Parent_Class );

   if( cp_Procedure )
      p_obj->setup_foreign_key( *cp_Procedure, v_Procedure );

   if( cp_Restriction_Field )
      p_obj->setup_foreign_key( *cp_Restriction_Field, v_Restriction_Field );

   if( cp_Restriction_Spec )
      p_obj->setup_foreign_key( *cp_Restriction_Spec, v_Restriction_Spec );

   if( cp_Source_Child )
      p_obj->setup_foreign_key( *cp_Source_Child, v_Source_Child );

   if( cp_Source_Child_Class )
      p_obj->setup_foreign_key( *cp_Source_Child_Class, v_Source_Child_Class );

   if( cp_Source_Field )
      p_obj->setup_foreign_key( *cp_Source_Field, v_Source_Field );

   if( cp_Source_Grandchild )
      p_obj->setup_foreign_key( *cp_Source_Grandchild, v_Source_Grandchild );

   if( cp_Source_Parent )
      p_obj->setup_foreign_key( *cp_Source_Parent, v_Source_Parent );

   if( cp_Source_Parent_Class )
      p_obj->setup_foreign_key( *cp_Source_Parent_Class, v_Source_Parent_Class );

   if( cp_Type )
      p_obj->setup_foreign_key( *cp_Type, v_Type );

   if( cp_Use_Type_Field )
      p_obj->setup_foreign_key( *cp_Use_Type_Field, v_Use_Type_Field );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_from_search_replace)] 600459
   if( !get_obj( ).get_key( ).empty( )
    && !get_obj( ).get_is_for_peer( )
    && ( get_obj( ).needs_field_value( "Name" )
    || required_transients.count( "Name" ) ) )
   {
      string str( get_obj( ).Type( ).List_Field_Name( ) );

      get_obj( ).Name( str );

      get_obj( ).add_search_replacement( "Name", "{sfield}", to_rep_string( get_obj( ).Source_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{sparent}", to_rep_string( get_obj( ).Source_Parent( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{schild}", to_rep_string( get_obj( ).Source_Child( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{sgchild}", to_rep_string( get_obj( ).Source_Grandchild( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{child}", to_rep_string( get_obj( ).Child_Relationship( ).Child_Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{crsfield}", to_rep_string( get_obj( ).Child_Rel_Source_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{crsparent}", to_rep_string( get_obj( ).Child_Rel_Source_Parent( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{crschild}", to_rep_string( get_obj( ).Child_Rel_Source_Child( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{rfield}", to_rep_string( get_obj( ).Restriction_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{procedure}", to_rep_string( get_obj( ).Procedure( ).Name( ) ) );

      get_obj( ).set_search_replace_separator( "Name", '_' );

      get_obj( ).set_search_replace_has_opt_prefixing( "Name" );
   }
   // [(finish field_from_search_replace)] 600459

   // [<start after_fetch>]
//nyi
   if( get_obj( ).Procedure( ).Access_Restriction( ) != 0 )
      get_obj( ).Access_Restriction( get_obj( ).Procedure( ).Access_Restriction( ) );

   if( !is_null( get_obj( ).Procedure( ).Access_Permission( ) ) )
      get_obj( ).Access_Permission( get_obj( ).Procedure( ).Access_Permission( ) );

   if( get_obj( ).Source_Field( ).Access_Restriction( ) != 0 )
      get_obj( ).Access_Restriction( get_obj( ).Source_Field( ).Access_Restriction( ) );

   if( !is_null( get_obj( ).Source_Field( ).Access_Permission( ) ) )
      get_obj( ).Access_Permission( get_obj( ).Source_Field( ).Access_Permission( ) );

   if( get_obj( ).Source_Parent( ).Access_Restriction( ) != 0 )
      get_obj( ).Access_Restriction( get_obj( ).Source_Parent( ).Access_Restriction( ) );

   if( !is_null( get_obj( ).Source_Parent( ).Access_Permission( ) ) )
      get_obj( ).Access_Permission( get_obj( ).Source_Parent( ).Access_Permission( ) );
   // [<finish after_fetch>]
}

void Meta_List_Field::impl::finalise_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_List_Field::impl::at_create( )
{
   // [(start fk_default)] 600445
   if( is_null( get_obj( ).Type( ) ) )
      get_obj( ).Type( "field" );
   // [(finish fk_default)] 600445

   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_List_Field::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_List_Field::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start default_to_field)] 600446
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Class( ) == g_default_Class ) )
      get_obj( ).Class( get_obj( ).List( ).Class( ) );
   // [(finish default_to_field)] 600446

   // [(start default_from_key)] 600447
   if( !get_obj( ).get_clone_key( ).empty( ) || ( is_create && ( get_obj( ).Order( ) == g_default_Order ) ) )
      get_obj( ).Order( get_obj( ).get_key( ) );
   // [(finish default_from_key)] 600447

   // [(start field_clear)] 600449
   if( get_obj( ).Use_Source_Parent( ) )
      get_obj( ).Source_Field( string( ) );
   // [(finish field_clear)] 600449

   // [(start field_clear)] 600450
   if( !get_obj( ).Use_Source_Parent( ) )
      get_obj( ).Source_Parent( string( ) );
   // [(finish field_clear)] 600450

   // [(start field_clear)] 600451
   if( !get_obj( ).Use_Source_Parent( ) )
      get_obj( ).Source_Child( string( ) );
   // [(finish field_clear)] 600451

   // [(start field_clear)] 600452
   if( !get_obj( ).Use_Source_Parent( ) )
      get_obj( ).Source_Grandchild( string( ) );
   // [(finish field_clear)] 600452

   // [(start field_clear)] 600453
   if( get_obj( ).Use_Child_Rel_Source_Parent( ) )
      get_obj( ).Child_Rel_Source_Field( string( ) );
   // [(finish field_clear)] 600453

   // [(start field_clear)] 600454
   if( !get_obj( ).Use_Child_Rel_Source_Parent( ) )
      get_obj( ).Child_Rel_Source_Parent( string( ) );
   // [(finish field_clear)] 600454

   // [(start field_clear)] 600455
   if( !get_obj( ).Use_Child_Rel_Source_Parent( ) )
      get_obj( ).Child_Rel_Source_Child( string( ) );
   // [(finish field_clear)] 600455

   // [(start field_from_changed_fk)] 600469
   if( get_obj( ).Source_Parent( ).has_changed( ) )
      get_obj( ).Source_Parent_Class( get_obj( ).Source_Parent( ).Parent_Class( ) );
   // [(finish field_from_changed_fk)] 600469

   // [(start field_from_changed_fk)] 600470
   if( get_obj( ).Source_Child( ).has_changed( ) )
      get_obj( ).Source_Child_Class( get_obj( ).Source_Child( ).Parent_Class( ) );
   // [(finish field_from_changed_fk)] 600470

   // [(start field_from_changed_fk)] 600484
   if( get_obj( ).Child_Relationship( ).has_changed( ) )
      get_obj( ).Child_Rel_Child_Class( get_obj( ).Child_Relationship( ).Child_Class( ) );
   // [(finish field_from_changed_fk)] 600484

   // [(start field_from_changed_fk)] 600485
   if( get_obj( ).Child_Rel_Source_Parent( ).has_changed( ) )
      get_obj( ).Child_Rel_Source_Parent_Class( get_obj( ).Child_Rel_Source_Parent( ).Parent_Class( ) );
   // [(finish field_from_changed_fk)] 600485

   // [(start default_to_global)] 600492
   if( is_create
    && get_obj( ).get_key( ).empty( )
    && get_obj( ).get_clone_key( ).empty( )
    && is_null( get_obj( ).Source_Field( ) )
    && get_obj( ).Use_Source_Parent( ) == 0
    && get_obj( ).Link_Restriction( ) == g_default_Link_Restriction )
      get_obj( ).Link_Restriction( 0 );
   // [(finish default_to_global)] 600492

   // [(start default_to_global)] 600493
   if( is_create
    && get_obj( ).get_key( ).empty( )
    && get_obj( ).get_clone_key( ).empty( )
    && is_null( get_obj( ).Source_Parent( ) )
    && get_obj( ).Use_Source_Parent( ) == 1
    && get_obj( ).Link_Restriction( ) == g_default_Link_Restriction )
      get_obj( ).Link_Restriction( 2 );
   // [(finish default_to_global)] 600493

   // [(start field_from_other_field)] 600494
   get_obj( ).Parent_Class( get_obj( ).List( ).Parent_Class( ) );
   // [(finish field_from_other_field)] 600494

   // [(start field_from_other_field)] 600497
   if( !is_null( get_obj( ).Child_Rel_Source_Parent_Class( ) ) )
      get_obj( ).Source_Parent_Class( get_obj( ).Child_Rel_Source_Parent_Class( ) );
   // [(finish field_from_other_field)] 600497

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_List_Field::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_store>]
//nyi
   if( get_obj( ).Type( ).get_key( ) == "user_action_event" && get_obj( ).Access_Restriction( ) > 3 )
      throw runtime_error( "Invalid Access Restriction for 'user_action_event' field." );
   // [<finish for_store>]
}

void Meta_List_Field::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_List_Field::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_List_Field::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_List_Field::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_List_Field::impl::set_default_values( )
{
   clear( );
}

bool Meta_List_Field::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_List_Field::impl::get_required_transients( ) const
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

Meta_List_Field::Meta_List_Field( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_List_Field::~Meta_List_Field( )
{
   cleanup( );
   delete p_impl;
}

int Meta_List_Field::Access_Restriction( ) const
{
   return p_impl->impl_Access_Restriction( );
}

void Meta_List_Field::Access_Restriction( int Access_Restriction )
{
   p_impl->impl_Access_Restriction( Access_Restriction );
}

int Meta_List_Field::Alignment( ) const
{
   return p_impl->impl_Alignment( );
}

void Meta_List_Field::Alignment( int Alignment )
{
   p_impl->impl_Alignment( Alignment );
}

bool Meta_List_Field::Allow_Anonymous_Access( ) const
{
   return p_impl->impl_Allow_Anonymous_Access( );
}

void Meta_List_Field::Allow_Anonymous_Access( bool Allow_Anonymous_Access )
{
   p_impl->impl_Allow_Anonymous_Access( Allow_Anonymous_Access );
}

bool Meta_List_Field::Exact_Match_Only( ) const
{
   return p_impl->impl_Exact_Match_Only( );
}

void Meta_List_Field::Exact_Match_Only( bool Exact_Match_Only )
{
   p_impl->impl_Exact_Match_Only( Exact_Match_Only );
}

bool Meta_List_Field::Exclude_In_Use_FK( ) const
{
   return p_impl->impl_Exclude_In_Use_FK( );
}

void Meta_List_Field::Exclude_In_Use_FK( bool Exclude_In_Use_FK )
{
   p_impl->impl_Exclude_In_Use_FK( Exclude_In_Use_FK );
}

int Meta_List_Field::Font_Size( ) const
{
   return p_impl->impl_Font_Size( );
}

void Meta_List_Field::Font_Size( int Font_Size )
{
   p_impl->impl_Font_Size( Font_Size );
}

bool Meta_List_Field::Hide_If_Is_Changing( ) const
{
   return p_impl->impl_Hide_If_Is_Changing( );
}

void Meta_List_Field::Hide_If_Is_Changing( bool Hide_If_Is_Changing )
{
   p_impl->impl_Hide_If_Is_Changing( Hide_If_Is_Changing );
}

const string& Meta_List_Field::Include_Key_Additions( ) const
{
   return p_impl->impl_Include_Key_Additions( );
}

void Meta_List_Field::Include_Key_Additions( const string& Include_Key_Additions )
{
   p_impl->impl_Include_Key_Additions( Include_Key_Additions );
}

int Meta_List_Field::Label_Class( ) const
{
   return p_impl->impl_Label_Class( );
}

void Meta_List_Field::Label_Class( int Label_Class )
{
   p_impl->impl_Label_Class( Label_Class );
}

int Meta_List_Field::Label_Source( ) const
{
   return p_impl->impl_Label_Source( );
}

void Meta_List_Field::Label_Source( int Label_Source )
{
   p_impl->impl_Label_Source( Label_Source );
}

int Meta_List_Field::Label_Source_Child( ) const
{
   return p_impl->impl_Label_Source_Child( );
}

void Meta_List_Field::Label_Source_Child( int Label_Source_Child )
{
   p_impl->impl_Label_Source_Child( Label_Source_Child );
}

int Meta_List_Field::Link_Empty_Restriction( ) const
{
   return p_impl->impl_Link_Empty_Restriction( );
}

void Meta_List_Field::Link_Empty_Restriction( int Link_Empty_Restriction )
{
   p_impl->impl_Link_Empty_Restriction( Link_Empty_Restriction );
}

int Meta_List_Field::Link_Restriction( ) const
{
   return p_impl->impl_Link_Restriction( );
}

void Meta_List_Field::Link_Restriction( int Link_Restriction )
{
   p_impl->impl_Link_Restriction( Link_Restriction );
}

int Meta_List_Field::Link_Type( ) const
{
   return p_impl->impl_Link_Type( );
}

void Meta_List_Field::Link_Type( int Link_Type )
{
   p_impl->impl_Link_Type( Link_Type );
}

const string& Meta_List_Field::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_List_Field::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

bool Meta_List_Field::Non_Instance_Procedure( ) const
{
   return p_impl->impl_Non_Instance_Procedure( );
}

void Meta_List_Field::Non_Instance_Procedure( bool Non_Instance_Procedure )
{
   p_impl->impl_Non_Instance_Procedure( Non_Instance_Procedure );
}

int Meta_List_Field::Notes_Truncation( ) const
{
   return p_impl->impl_Notes_Truncation( );
}

void Meta_List_Field::Notes_Truncation( int Notes_Truncation )
{
   p_impl->impl_Notes_Truncation( Notes_Truncation );
}

bool Meta_List_Field::Omit_Versions( ) const
{
   return p_impl->impl_Omit_Versions( );
}

void Meta_List_Field::Omit_Versions( bool Omit_Versions )
{
   p_impl->impl_Omit_Versions( Omit_Versions );
}

const string& Meta_List_Field::Order( ) const
{
   return p_impl->impl_Order( );
}

void Meta_List_Field::Order( const string& Order )
{
   p_impl->impl_Order( Order );
}

int Meta_List_Field::Orientation( ) const
{
   return p_impl->impl_Orientation( );
}

void Meta_List_Field::Orientation( int Orientation )
{
   p_impl->impl_Orientation( Orientation );
}

int Meta_List_Field::Print_Type( ) const
{
   return p_impl->impl_Print_Type( );
}

void Meta_List_Field::Print_Type( int Print_Type )
{
   p_impl->impl_Print_Type( Print_Type );
}

const string& Meta_List_Field::Procedure_Args( ) const
{
   return p_impl->impl_Procedure_Args( );
}

void Meta_List_Field::Procedure_Args( const string& Procedure_Args )
{
   p_impl->impl_Procedure_Args( Procedure_Args );
}

const string& Meta_List_Field::Restriction_Value( ) const
{
   return p_impl->impl_Restriction_Value( );
}

void Meta_List_Field::Restriction_Value( const string& Restriction_Value )
{
   p_impl->impl_Restriction_Value( Restriction_Value );
}

bool Meta_List_Field::Retain_Selected_Rows( ) const
{
   return p_impl->impl_Retain_Selected_Rows( );
}

void Meta_List_Field::Retain_Selected_Rows( bool Retain_Selected_Rows )
{
   p_impl->impl_Retain_Selected_Rows( Retain_Selected_Rows );
}

bool Meta_List_Field::Reverse_Order( ) const
{
   return p_impl->impl_Reverse_Order( );
}

void Meta_List_Field::Reverse_Order( bool Reverse_Order )
{
   p_impl->impl_Reverse_Order( Reverse_Order );
}

int Meta_List_Field::Search_Option_Limit( ) const
{
   return p_impl->impl_Search_Option_Limit( );
}

void Meta_List_Field::Search_Option_Limit( int Search_Option_Limit )
{
   p_impl->impl_Search_Option_Limit( Search_Option_Limit );
}

const string& Meta_List_Field::Select_Key_Exclusions( ) const
{
   return p_impl->impl_Select_Key_Exclusions( );
}

void Meta_List_Field::Select_Key_Exclusions( const string& Select_Key_Exclusions )
{
   p_impl->impl_Select_Key_Exclusions( Select_Key_Exclusions );
}

bool Meta_List_Field::Sort_Manually( ) const
{
   return p_impl->impl_Sort_Manually( );
}

void Meta_List_Field::Sort_Manually( bool Sort_Manually )
{
   p_impl->impl_Sort_Manually( Sort_Manually );
}

int Meta_List_Field::Switch_Type( ) const
{
   return p_impl->impl_Switch_Type( );
}

void Meta_List_Field::Switch_Type( int Switch_Type )
{
   p_impl->impl_Switch_Type( Switch_Type );
}

int Meta_List_Field::Trigger_Option( ) const
{
   return p_impl->impl_Trigger_Option( );
}

void Meta_List_Field::Trigger_Option( int Trigger_Option )
{
   p_impl->impl_Trigger_Option( Trigger_Option );
}

bool Meta_List_Field::Use_Child_Rel_Source_Parent( ) const
{
   return p_impl->impl_Use_Child_Rel_Source_Parent( );
}

void Meta_List_Field::Use_Child_Rel_Source_Parent( bool Use_Child_Rel_Source_Parent )
{
   p_impl->impl_Use_Child_Rel_Source_Parent( Use_Child_Rel_Source_Parent );
}

bool Meta_List_Field::Use_In_Text_Search_Title( ) const
{
   return p_impl->impl_Use_In_Text_Search_Title( );
}

void Meta_List_Field::Use_In_Text_Search_Title( bool Use_In_Text_Search_Title )
{
   p_impl->impl_Use_In_Text_Search_Title( Use_In_Text_Search_Title );
}

bool Meta_List_Field::Use_Source_Parent( ) const
{
   return p_impl->impl_Use_Source_Parent( );
}

void Meta_List_Field::Use_Source_Parent( bool Use_Source_Parent )
{
   p_impl->impl_Use_Source_Parent( Use_Source_Parent );
}

int Meta_List_Field::View_Parent_Extra( ) const
{
   return p_impl->impl_View_Parent_Extra( );
}

void Meta_List_Field::View_Parent_Extra( int View_Parent_Extra )
{
   p_impl->impl_View_Parent_Extra( View_Parent_Extra );
}

Meta_Modifier& Meta_List_Field::Access_Parent_Modifier( )
{
   return p_impl->impl_Access_Parent_Modifier( );
}

const Meta_Modifier& Meta_List_Field::Access_Parent_Modifier( ) const
{
   return p_impl->impl_Access_Parent_Modifier( );
}

void Meta_List_Field::Access_Parent_Modifier( const string& key )
{
   p_impl->impl_Access_Parent_Modifier( key );
}

Meta_Permission& Meta_List_Field::Access_Permission( )
{
   return p_impl->impl_Access_Permission( );
}

const Meta_Permission& Meta_List_Field::Access_Permission( ) const
{
   return p_impl->impl_Access_Permission( );
}

void Meta_List_Field::Access_Permission( const string& key )
{
   p_impl->impl_Access_Permission( key );
}

Meta_Class& Meta_List_Field::Child_Rel_Child_Class( )
{
   return p_impl->impl_Child_Rel_Child_Class( );
}

const Meta_Class& Meta_List_Field::Child_Rel_Child_Class( ) const
{
   return p_impl->impl_Child_Rel_Child_Class( );
}

void Meta_List_Field::Child_Rel_Child_Class( const string& key )
{
   p_impl->impl_Child_Rel_Child_Class( key );
}

Meta_Field& Meta_List_Field::Child_Rel_Source_Child( )
{
   return p_impl->impl_Child_Rel_Source_Child( );
}

const Meta_Field& Meta_List_Field::Child_Rel_Source_Child( ) const
{
   return p_impl->impl_Child_Rel_Source_Child( );
}

void Meta_List_Field::Child_Rel_Source_Child( const string& key )
{
   p_impl->impl_Child_Rel_Source_Child( key );
}

Meta_Field& Meta_List_Field::Child_Rel_Source_Field( )
{
   return p_impl->impl_Child_Rel_Source_Field( );
}

const Meta_Field& Meta_List_Field::Child_Rel_Source_Field( ) const
{
   return p_impl->impl_Child_Rel_Source_Field( );
}

void Meta_List_Field::Child_Rel_Source_Field( const string& key )
{
   p_impl->impl_Child_Rel_Source_Field( key );
}

Meta_Field& Meta_List_Field::Child_Rel_Source_Parent( )
{
   return p_impl->impl_Child_Rel_Source_Parent( );
}

const Meta_Field& Meta_List_Field::Child_Rel_Source_Parent( ) const
{
   return p_impl->impl_Child_Rel_Source_Parent( );
}

void Meta_List_Field::Child_Rel_Source_Parent( const string& key )
{
   p_impl->impl_Child_Rel_Source_Parent( key );
}

Meta_Class& Meta_List_Field::Child_Rel_Source_Parent_Class( )
{
   return p_impl->impl_Child_Rel_Source_Parent_Class( );
}

const Meta_Class& Meta_List_Field::Child_Rel_Source_Parent_Class( ) const
{
   return p_impl->impl_Child_Rel_Source_Parent_Class( );
}

void Meta_List_Field::Child_Rel_Source_Parent_Class( const string& key )
{
   p_impl->impl_Child_Rel_Source_Parent_Class( key );
}

Meta_Relationship& Meta_List_Field::Child_Relationship( )
{
   return p_impl->impl_Child_Relationship( );
}

const Meta_Relationship& Meta_List_Field::Child_Relationship( ) const
{
   return p_impl->impl_Child_Relationship( );
}

void Meta_List_Field::Child_Relationship( const string& key )
{
   p_impl->impl_Child_Relationship( key );
}

Meta_Class& Meta_List_Field::Class( )
{
   return p_impl->impl_Class( );
}

const Meta_Class& Meta_List_Field::Class( ) const
{
   return p_impl->impl_Class( );
}

void Meta_List_Field::Class( const string& key )
{
   p_impl->impl_Class( key );
}

Meta_Permission& Meta_List_Field::Link_Permission( )
{
   return p_impl->impl_Link_Permission( );
}

const Meta_Permission& Meta_List_Field::Link_Permission( ) const
{
   return p_impl->impl_Link_Permission( );
}

void Meta_List_Field::Link_Permission( const string& key )
{
   p_impl->impl_Link_Permission( key );
}

Meta_List& Meta_List_Field::List( )
{
   return p_impl->impl_List( );
}

const Meta_List& Meta_List_Field::List( ) const
{
   return p_impl->impl_List( );
}

void Meta_List_Field::List( const string& key )
{
   p_impl->impl_List( key );
}

Meta_Class& Meta_List_Field::Parent_Class( )
{
   return p_impl->impl_Parent_Class( );
}

const Meta_Class& Meta_List_Field::Parent_Class( ) const
{
   return p_impl->impl_Parent_Class( );
}

void Meta_List_Field::Parent_Class( const string& key )
{
   p_impl->impl_Parent_Class( key );
}

Meta_Procedure& Meta_List_Field::Procedure( )
{
   return p_impl->impl_Procedure( );
}

const Meta_Procedure& Meta_List_Field::Procedure( ) const
{
   return p_impl->impl_Procedure( );
}

void Meta_List_Field::Procedure( const string& key )
{
   p_impl->impl_Procedure( key );
}

Meta_Field& Meta_List_Field::Restriction_Field( )
{
   return p_impl->impl_Restriction_Field( );
}

const Meta_Field& Meta_List_Field::Restriction_Field( ) const
{
   return p_impl->impl_Restriction_Field( );
}

void Meta_List_Field::Restriction_Field( const string& key )
{
   p_impl->impl_Restriction_Field( key );
}

Meta_Specification& Meta_List_Field::Restriction_Spec( )
{
   return p_impl->impl_Restriction_Spec( );
}

const Meta_Specification& Meta_List_Field::Restriction_Spec( ) const
{
   return p_impl->impl_Restriction_Spec( );
}

void Meta_List_Field::Restriction_Spec( const string& key )
{
   p_impl->impl_Restriction_Spec( key );
}

Meta_Field& Meta_List_Field::Source_Child( )
{
   return p_impl->impl_Source_Child( );
}

const Meta_Field& Meta_List_Field::Source_Child( ) const
{
   return p_impl->impl_Source_Child( );
}

void Meta_List_Field::Source_Child( const string& key )
{
   p_impl->impl_Source_Child( key );
}

Meta_Class& Meta_List_Field::Source_Child_Class( )
{
   return p_impl->impl_Source_Child_Class( );
}

const Meta_Class& Meta_List_Field::Source_Child_Class( ) const
{
   return p_impl->impl_Source_Child_Class( );
}

void Meta_List_Field::Source_Child_Class( const string& key )
{
   p_impl->impl_Source_Child_Class( key );
}

Meta_Field& Meta_List_Field::Source_Field( )
{
   return p_impl->impl_Source_Field( );
}

const Meta_Field& Meta_List_Field::Source_Field( ) const
{
   return p_impl->impl_Source_Field( );
}

void Meta_List_Field::Source_Field( const string& key )
{
   p_impl->impl_Source_Field( key );
}

Meta_Field& Meta_List_Field::Source_Grandchild( )
{
   return p_impl->impl_Source_Grandchild( );
}

const Meta_Field& Meta_List_Field::Source_Grandchild( ) const
{
   return p_impl->impl_Source_Grandchild( );
}

void Meta_List_Field::Source_Grandchild( const string& key )
{
   p_impl->impl_Source_Grandchild( key );
}

Meta_Field& Meta_List_Field::Source_Parent( )
{
   return p_impl->impl_Source_Parent( );
}

const Meta_Field& Meta_List_Field::Source_Parent( ) const
{
   return p_impl->impl_Source_Parent( );
}

void Meta_List_Field::Source_Parent( const string& key )
{
   p_impl->impl_Source_Parent( key );
}

Meta_Class& Meta_List_Field::Source_Parent_Class( )
{
   return p_impl->impl_Source_Parent_Class( );
}

const Meta_Class& Meta_List_Field::Source_Parent_Class( ) const
{
   return p_impl->impl_Source_Parent_Class( );
}

void Meta_List_Field::Source_Parent_Class( const string& key )
{
   p_impl->impl_Source_Parent_Class( key );
}

Meta_List_Field_Type& Meta_List_Field::Type( )
{
   return p_impl->impl_Type( );
}

const Meta_List_Field_Type& Meta_List_Field::Type( ) const
{
   return p_impl->impl_Type( );
}

void Meta_List_Field::Type( const string& key )
{
   p_impl->impl_Type( key );
}

Meta_Field& Meta_List_Field::Use_Type_Field( )
{
   return p_impl->impl_Use_Type_Field( );
}

const Meta_Field& Meta_List_Field::Use_Type_Field( ) const
{
   return p_impl->impl_Use_Type_Field( );
}

void Meta_List_Field::Use_Type_Field( const string& key )
{
   p_impl->impl_Use_Type_Field( key );
}

void Meta_List_Field::Move_Down( const string& Restrict_Fields, const string& Restrict_Values )
{
   p_impl->impl_Move_Down( Restrict_Fields, Restrict_Values );
}

void Meta_List_Field::Move_Up( const string& Restrict_Fields, const string& Restrict_Values )
{
   p_impl->impl_Move_Up( Restrict_Fields, Restrict_Values );
}

string Meta_List_Field::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_List_Field::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_List_Field::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_List_Field::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_List_Field::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_List_Field::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_List_Field::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_List_Field::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_List_Field::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_List_Field::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_List_Field::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_List_Field::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_List_Field::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_List_Field::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_List_Field::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_List_Field::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_List_Field::clear( )
{
   p_impl->clear( );
}

void Meta_List_Field::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_List_Field::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_List_Field::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_List_Field::finalise_fetch( bool skip_set_original )
{
   p_impl->finalise_fetch( skip_set_original );
}

void Meta_List_Field::at_create( )
{
   p_impl->at_create( );
}

void Meta_List_Field::post_init( )
{
   p_impl->post_init( );
}

void Meta_List_Field::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_List_Field::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_List_Field::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_List_Field::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_List_Field::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_List_Field::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_List_Field::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_List_Field::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_List_Field::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( 0 );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Access_Parent_Modifier )
   {
      p_id = c_field_id_Access_Parent_Modifier;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
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
   else if( name == c_field_name_Child_Rel_Child_Class )
   {
      p_id = c_field_id_Child_Rel_Child_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Child_Rel_Source_Child )
   {
      p_id = c_field_id_Child_Rel_Source_Child;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Child_Rel_Source_Field )
   {
      p_id = c_field_id_Child_Rel_Source_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Child_Rel_Source_Parent )
   {
      p_id = c_field_id_Child_Rel_Source_Parent;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Child_Rel_Source_Parent_Class )
   {
      p_id = c_field_id_Child_Rel_Source_Parent_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Child_Relationship )
   {
      p_id = c_field_id_Child_Relationship;

      if( p_type_name )
         *p_type_name = "Meta_Relationship";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Class )
   {
      p_id = c_field_id_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Exact_Match_Only )
   {
      p_id = c_field_id_Exact_Match_Only;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Exclude_In_Use_FK )
   {
      p_id = c_field_id_Exclude_In_Use_FK;

      if( p_type_name )
         *p_type_name = "bool";

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
   else if( name == c_field_name_Hide_If_Is_Changing )
   {
      p_id = c_field_id_Hide_If_Is_Changing;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Include_Key_Additions )
   {
      p_id = c_field_id_Include_Key_Additions;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Label_Class )
   {
      p_id = c_field_id_Label_Class;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Label_Source )
   {
      p_id = c_field_id_Label_Source;

      if( p_type_name )
         *p_type_name = "int";

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
   else if( name == c_field_name_Link_Empty_Restriction )
   {
      p_id = c_field_id_Link_Empty_Restriction;

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
   else if( name == c_field_name_Link_Type )
   {
      p_id = c_field_id_Link_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_List )
   {
      p_id = c_field_id_List;

      if( p_type_name )
         *p_type_name = "Meta_List";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Name )
   {
      p_id = c_field_id_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Non_Instance_Procedure )
   {
      p_id = c_field_id_Non_Instance_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Notes_Truncation )
   {
      p_id = c_field_id_Notes_Truncation;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Omit_Versions )
   {
      p_id = c_field_id_Omit_Versions;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
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
   else if( name == c_field_name_Parent_Class )
   {
      p_id = c_field_id_Parent_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Print_Type )
   {
      p_id = c_field_id_Print_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Procedure )
   {
      p_id = c_field_id_Procedure;

      if( p_type_name )
         *p_type_name = "Meta_Procedure";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Procedure_Args )
   {
      p_id = c_field_id_Procedure_Args;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Restriction_Field )
   {
      p_id = c_field_id_Restriction_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Restriction_Spec )
   {
      p_id = c_field_id_Restriction_Spec;

      if( p_type_name )
         *p_type_name = "Meta_Specification";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Restriction_Value )
   {
      p_id = c_field_id_Restriction_Value;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Retain_Selected_Rows )
   {
      p_id = c_field_id_Retain_Selected_Rows;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Reverse_Order )
   {
      p_id = c_field_id_Reverse_Order;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Search_Option_Limit )
   {
      p_id = c_field_id_Search_Option_Limit;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Select_Key_Exclusions )
   {
      p_id = c_field_id_Select_Key_Exclusions;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
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
   else if( name == c_field_name_Source_Child_Class )
   {
      p_id = c_field_id_Source_Child_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

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
   else if( name == c_field_name_Source_Grandchild )
   {
      p_id = c_field_id_Source_Grandchild;

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
   else if( name == c_field_name_Switch_Type )
   {
      p_id = c_field_id_Switch_Type;

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
         *p_type_name = "Meta_List_Field_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Use_Child_Rel_Source_Parent )
   {
      p_id = c_field_id_Use_Child_Rel_Source_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_In_Text_Search_Title )
   {
      p_id = c_field_id_Use_In_Text_Search_Title;

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
   else if( name == c_field_name_Use_Type_Field )
   {
      p_id = c_field_id_Use_Type_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_View_Parent_Extra )
   {
      p_id = c_field_id_View_Parent_Extra;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_id;
}

const char* Meta_List_Field::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( 0 );

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Access_Parent_Modifier )
   {
      p_name = c_field_name_Access_Parent_Modifier;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
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
   else if( id == c_field_id_Child_Rel_Child_Class )
   {
      p_name = c_field_name_Child_Rel_Child_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Child_Rel_Source_Child )
   {
      p_name = c_field_name_Child_Rel_Source_Child;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Child_Rel_Source_Field )
   {
      p_name = c_field_name_Child_Rel_Source_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Child_Rel_Source_Parent )
   {
      p_name = c_field_name_Child_Rel_Source_Parent;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Child_Rel_Source_Parent_Class )
   {
      p_name = c_field_name_Child_Rel_Source_Parent_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Child_Relationship )
   {
      p_name = c_field_name_Child_Relationship;

      if( p_type_name )
         *p_type_name = "Meta_Relationship";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Class )
   {
      p_name = c_field_name_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Exact_Match_Only )
   {
      p_name = c_field_name_Exact_Match_Only;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Exclude_In_Use_FK )
   {
      p_name = c_field_name_Exclude_In_Use_FK;

      if( p_type_name )
         *p_type_name = "bool";

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
   else if( id == c_field_id_Hide_If_Is_Changing )
   {
      p_name = c_field_name_Hide_If_Is_Changing;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Include_Key_Additions )
   {
      p_name = c_field_name_Include_Key_Additions;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Label_Class )
   {
      p_name = c_field_name_Label_Class;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Label_Source )
   {
      p_name = c_field_name_Label_Source;

      if( p_type_name )
         *p_type_name = "int";

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
   else if( id == c_field_id_Link_Empty_Restriction )
   {
      p_name = c_field_name_Link_Empty_Restriction;

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
   else if( id == c_field_id_Link_Type )
   {
      p_name = c_field_name_Link_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_List )
   {
      p_name = c_field_name_List;

      if( p_type_name )
         *p_type_name = "Meta_List";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Name )
   {
      p_name = c_field_name_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Non_Instance_Procedure )
   {
      p_name = c_field_name_Non_Instance_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Notes_Truncation )
   {
      p_name = c_field_name_Notes_Truncation;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Omit_Versions )
   {
      p_name = c_field_name_Omit_Versions;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
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
   else if( id == c_field_id_Parent_Class )
   {
      p_name = c_field_name_Parent_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Print_Type )
   {
      p_name = c_field_name_Print_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Procedure )
   {
      p_name = c_field_name_Procedure;

      if( p_type_name )
         *p_type_name = "Meta_Procedure";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Procedure_Args )
   {
      p_name = c_field_name_Procedure_Args;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Restriction_Field )
   {
      p_name = c_field_name_Restriction_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Restriction_Spec )
   {
      p_name = c_field_name_Restriction_Spec;

      if( p_type_name )
         *p_type_name = "Meta_Specification";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Restriction_Value )
   {
      p_name = c_field_name_Restriction_Value;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Retain_Selected_Rows )
   {
      p_name = c_field_name_Retain_Selected_Rows;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Reverse_Order )
   {
      p_name = c_field_name_Reverse_Order;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Search_Option_Limit )
   {
      p_name = c_field_name_Search_Option_Limit;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Select_Key_Exclusions )
   {
      p_name = c_field_name_Select_Key_Exclusions;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
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
   else if( id == c_field_id_Source_Child_Class )
   {
      p_name = c_field_name_Source_Child_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

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
   else if( id == c_field_id_Source_Grandchild )
   {
      p_name = c_field_name_Source_Grandchild;

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
   else if( id == c_field_id_Switch_Type )
   {
      p_name = c_field_name_Switch_Type;

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
         *p_type_name = "Meta_List_Field_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Use_Child_Rel_Source_Parent )
   {
      p_name = c_field_name_Use_Child_Rel_Source_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_In_Text_Search_Title )
   {
      p_name = c_field_name_Use_In_Text_Search_Title;

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
   else if( id == c_field_id_Use_Type_Field )
   {
      p_name = c_field_name_Use_Type_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_View_Parent_Extra )
   {
      p_name = c_field_name_View_Parent_Extra;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_name;
}

string& Meta_List_Field::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_List_Field::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_List_Field::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_List_Field::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_List_Field::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_List_Field::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_List_Field::get_field_uom_symbol( const string& id_or_name ) const
{
   string uom_symbol;

   string name;
   pair< string, string > next;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_uom_symbol" );
   if( ( id_or_name == c_field_id_Access_Parent_Modifier ) || ( id_or_name == c_field_name_Access_Parent_Modifier ) )
   {
      name = string( c_field_display_name_Access_Parent_Modifier );
      get_module_string( c_field_display_name_Access_Parent_Modifier, &next );
   }
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
   if( ( id_or_name == c_field_id_Child_Rel_Child_Class ) || ( id_or_name == c_field_name_Child_Rel_Child_Class ) )
   {
      name = string( c_field_display_name_Child_Rel_Child_Class );
      get_module_string( c_field_display_name_Child_Rel_Child_Class, &next );
   }
   if( ( id_or_name == c_field_id_Child_Rel_Source_Child ) || ( id_or_name == c_field_name_Child_Rel_Source_Child ) )
   {
      name = string( c_field_display_name_Child_Rel_Source_Child );
      get_module_string( c_field_display_name_Child_Rel_Source_Child, &next );
   }
   if( ( id_or_name == c_field_id_Child_Rel_Source_Field ) || ( id_or_name == c_field_name_Child_Rel_Source_Field ) )
   {
      name = string( c_field_display_name_Child_Rel_Source_Field );
      get_module_string( c_field_display_name_Child_Rel_Source_Field, &next );
   }
   if( ( id_or_name == c_field_id_Child_Rel_Source_Parent ) || ( id_or_name == c_field_name_Child_Rel_Source_Parent ) )
   {
      name = string( c_field_display_name_Child_Rel_Source_Parent );
      get_module_string( c_field_display_name_Child_Rel_Source_Parent, &next );
   }
   if( ( id_or_name == c_field_id_Child_Rel_Source_Parent_Class ) || ( id_or_name == c_field_name_Child_Rel_Source_Parent_Class ) )
   {
      name = string( c_field_display_name_Child_Rel_Source_Parent_Class );
      get_module_string( c_field_display_name_Child_Rel_Source_Parent_Class, &next );
   }
   if( ( id_or_name == c_field_id_Child_Relationship ) || ( id_or_name == c_field_name_Child_Relationship ) )
   {
      name = string( c_field_display_name_Child_Relationship );
      get_module_string( c_field_display_name_Child_Relationship, &next );
   }
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
   {
      name = string( c_field_display_name_Class );
      get_module_string( c_field_display_name_Class, &next );
   }
   if( ( id_or_name == c_field_id_Exact_Match_Only ) || ( id_or_name == c_field_name_Exact_Match_Only ) )
   {
      name = string( c_field_display_name_Exact_Match_Only );
      get_module_string( c_field_display_name_Exact_Match_Only, &next );
   }
   if( ( id_or_name == c_field_id_Exclude_In_Use_FK ) || ( id_or_name == c_field_name_Exclude_In_Use_FK ) )
   {
      name = string( c_field_display_name_Exclude_In_Use_FK );
      get_module_string( c_field_display_name_Exclude_In_Use_FK, &next );
   }
   if( ( id_or_name == c_field_id_Font_Size ) || ( id_or_name == c_field_name_Font_Size ) )
   {
      name = string( c_field_display_name_Font_Size );
      get_module_string( c_field_display_name_Font_Size, &next );
   }
   if( ( id_or_name == c_field_id_Hide_If_Is_Changing ) || ( id_or_name == c_field_name_Hide_If_Is_Changing ) )
   {
      name = string( c_field_display_name_Hide_If_Is_Changing );
      get_module_string( c_field_display_name_Hide_If_Is_Changing, &next );
   }
   if( ( id_or_name == c_field_id_Include_Key_Additions ) || ( id_or_name == c_field_name_Include_Key_Additions ) )
   {
      name = string( c_field_display_name_Include_Key_Additions );
      get_module_string( c_field_display_name_Include_Key_Additions, &next );
   }
   if( ( id_or_name == c_field_id_Label_Class ) || ( id_or_name == c_field_name_Label_Class ) )
   {
      name = string( c_field_display_name_Label_Class );
      get_module_string( c_field_display_name_Label_Class, &next );
   }
   if( ( id_or_name == c_field_id_Label_Source ) || ( id_or_name == c_field_name_Label_Source ) )
   {
      name = string( c_field_display_name_Label_Source );
      get_module_string( c_field_display_name_Label_Source, &next );
   }
   if( ( id_or_name == c_field_id_Label_Source_Child ) || ( id_or_name == c_field_name_Label_Source_Child ) )
   {
      name = string( c_field_display_name_Label_Source_Child );
      get_module_string( c_field_display_name_Label_Source_Child, &next );
   }
   if( ( id_or_name == c_field_id_Link_Empty_Restriction ) || ( id_or_name == c_field_name_Link_Empty_Restriction ) )
   {
      name = string( c_field_display_name_Link_Empty_Restriction );
      get_module_string( c_field_display_name_Link_Empty_Restriction, &next );
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
   if( ( id_or_name == c_field_id_Link_Type ) || ( id_or_name == c_field_name_Link_Type ) )
   {
      name = string( c_field_display_name_Link_Type );
      get_module_string( c_field_display_name_Link_Type, &next );
   }
   if( ( id_or_name == c_field_id_List ) || ( id_or_name == c_field_name_List ) )
   {
      name = string( c_field_display_name_List );
      get_module_string( c_field_display_name_List, &next );
   }
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
   {
      name = string( c_field_display_name_Name );
      get_module_string( c_field_display_name_Name, &next );
   }
   if( ( id_or_name == c_field_id_Non_Instance_Procedure ) || ( id_or_name == c_field_name_Non_Instance_Procedure ) )
   {
      name = string( c_field_display_name_Non_Instance_Procedure );
      get_module_string( c_field_display_name_Non_Instance_Procedure, &next );
   }
   if( ( id_or_name == c_field_id_Notes_Truncation ) || ( id_or_name == c_field_name_Notes_Truncation ) )
   {
      name = string( c_field_display_name_Notes_Truncation );
      get_module_string( c_field_display_name_Notes_Truncation, &next );
   }
   if( ( id_or_name == c_field_id_Omit_Versions ) || ( id_or_name == c_field_name_Omit_Versions ) )
   {
      name = string( c_field_display_name_Omit_Versions );
      get_module_string( c_field_display_name_Omit_Versions, &next );
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
   if( ( id_or_name == c_field_id_Parent_Class ) || ( id_or_name == c_field_name_Parent_Class ) )
   {
      name = string( c_field_display_name_Parent_Class );
      get_module_string( c_field_display_name_Parent_Class, &next );
   }
   if( ( id_or_name == c_field_id_Print_Type ) || ( id_or_name == c_field_name_Print_Type ) )
   {
      name = string( c_field_display_name_Print_Type );
      get_module_string( c_field_display_name_Print_Type, &next );
   }
   if( ( id_or_name == c_field_id_Procedure ) || ( id_or_name == c_field_name_Procedure ) )
   {
      name = string( c_field_display_name_Procedure );
      get_module_string( c_field_display_name_Procedure, &next );
   }
   if( ( id_or_name == c_field_id_Procedure_Args ) || ( id_or_name == c_field_name_Procedure_Args ) )
   {
      name = string( c_field_display_name_Procedure_Args );
      get_module_string( c_field_display_name_Procedure_Args, &next );
   }
   if( ( id_or_name == c_field_id_Restriction_Field ) || ( id_or_name == c_field_name_Restriction_Field ) )
   {
      name = string( c_field_display_name_Restriction_Field );
      get_module_string( c_field_display_name_Restriction_Field, &next );
   }
   if( ( id_or_name == c_field_id_Restriction_Spec ) || ( id_or_name == c_field_name_Restriction_Spec ) )
   {
      name = string( c_field_display_name_Restriction_Spec );
      get_module_string( c_field_display_name_Restriction_Spec, &next );
   }
   if( ( id_or_name == c_field_id_Restriction_Value ) || ( id_or_name == c_field_name_Restriction_Value ) )
   {
      name = string( c_field_display_name_Restriction_Value );
      get_module_string( c_field_display_name_Restriction_Value, &next );
   }
   if( ( id_or_name == c_field_id_Retain_Selected_Rows ) || ( id_or_name == c_field_name_Retain_Selected_Rows ) )
   {
      name = string( c_field_display_name_Retain_Selected_Rows );
      get_module_string( c_field_display_name_Retain_Selected_Rows, &next );
   }
   if( ( id_or_name == c_field_id_Reverse_Order ) || ( id_or_name == c_field_name_Reverse_Order ) )
   {
      name = string( c_field_display_name_Reverse_Order );
      get_module_string( c_field_display_name_Reverse_Order, &next );
   }
   if( ( id_or_name == c_field_id_Search_Option_Limit ) || ( id_or_name == c_field_name_Search_Option_Limit ) )
   {
      name = string( c_field_display_name_Search_Option_Limit );
      get_module_string( c_field_display_name_Search_Option_Limit, &next );
   }
   if( ( id_or_name == c_field_id_Select_Key_Exclusions ) || ( id_or_name == c_field_name_Select_Key_Exclusions ) )
   {
      name = string( c_field_display_name_Select_Key_Exclusions );
      get_module_string( c_field_display_name_Select_Key_Exclusions, &next );
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
   if( ( id_or_name == c_field_id_Source_Child_Class ) || ( id_or_name == c_field_name_Source_Child_Class ) )
   {
      name = string( c_field_display_name_Source_Child_Class );
      get_module_string( c_field_display_name_Source_Child_Class, &next );
   }
   if( ( id_or_name == c_field_id_Source_Field ) || ( id_or_name == c_field_name_Source_Field ) )
   {
      name = string( c_field_display_name_Source_Field );
      get_module_string( c_field_display_name_Source_Field, &next );
   }
   if( ( id_or_name == c_field_id_Source_Grandchild ) || ( id_or_name == c_field_name_Source_Grandchild ) )
   {
      name = string( c_field_display_name_Source_Grandchild );
      get_module_string( c_field_display_name_Source_Grandchild, &next );
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
   if( ( id_or_name == c_field_id_Switch_Type ) || ( id_or_name == c_field_name_Switch_Type ) )
   {
      name = string( c_field_display_name_Switch_Type );
      get_module_string( c_field_display_name_Switch_Type, &next );
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
   if( ( id_or_name == c_field_id_Use_Child_Rel_Source_Parent ) || ( id_or_name == c_field_name_Use_Child_Rel_Source_Parent ) )
   {
      name = string( c_field_display_name_Use_Child_Rel_Source_Parent );
      get_module_string( c_field_display_name_Use_Child_Rel_Source_Parent, &next );
   }
   if( ( id_or_name == c_field_id_Use_In_Text_Search_Title ) || ( id_or_name == c_field_name_Use_In_Text_Search_Title ) )
   {
      name = string( c_field_display_name_Use_In_Text_Search_Title );
      get_module_string( c_field_display_name_Use_In_Text_Search_Title, &next );
   }
   if( ( id_or_name == c_field_id_Use_Source_Parent ) || ( id_or_name == c_field_name_Use_Source_Parent ) )
   {
      name = string( c_field_display_name_Use_Source_Parent );
      get_module_string( c_field_display_name_Use_Source_Parent, &next );
   }
   if( ( id_or_name == c_field_id_Use_Type_Field ) || ( id_or_name == c_field_name_Use_Type_Field ) )
   {
      name = string( c_field_display_name_Use_Type_Field );
      get_module_string( c_field_display_name_Use_Type_Field, &next );
   }
   if( ( id_or_name == c_field_id_View_Parent_Extra ) || ( id_or_name == c_field_name_View_Parent_Extra ) )
   {
      name = string( c_field_display_name_View_Parent_Extra );
      get_module_string( c_field_display_name_View_Parent_Extra, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_List_Field::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Access_Parent_Modifier ) || ( id_or_name == c_field_name_Access_Parent_Modifier ) )
      display_name = get_module_string( c_field_display_name_Access_Parent_Modifier );
   if( ( id_or_name == c_field_id_Access_Permission ) || ( id_or_name == c_field_name_Access_Permission ) )
      display_name = get_module_string( c_field_display_name_Access_Permission );
   if( ( id_or_name == c_field_id_Access_Restriction ) || ( id_or_name == c_field_name_Access_Restriction ) )
      display_name = get_module_string( c_field_display_name_Access_Restriction );
   if( ( id_or_name == c_field_id_Alignment ) || ( id_or_name == c_field_name_Alignment ) )
      display_name = get_module_string( c_field_display_name_Alignment );
   if( ( id_or_name == c_field_id_Allow_Anonymous_Access ) || ( id_or_name == c_field_name_Allow_Anonymous_Access ) )
      display_name = get_module_string( c_field_display_name_Allow_Anonymous_Access );
   if( ( id_or_name == c_field_id_Child_Rel_Child_Class ) || ( id_or_name == c_field_name_Child_Rel_Child_Class ) )
      display_name = get_module_string( c_field_display_name_Child_Rel_Child_Class );
   if( ( id_or_name == c_field_id_Child_Rel_Source_Child ) || ( id_or_name == c_field_name_Child_Rel_Source_Child ) )
      display_name = get_module_string( c_field_display_name_Child_Rel_Source_Child );
   if( ( id_or_name == c_field_id_Child_Rel_Source_Field ) || ( id_or_name == c_field_name_Child_Rel_Source_Field ) )
      display_name = get_module_string( c_field_display_name_Child_Rel_Source_Field );
   if( ( id_or_name == c_field_id_Child_Rel_Source_Parent ) || ( id_or_name == c_field_name_Child_Rel_Source_Parent ) )
      display_name = get_module_string( c_field_display_name_Child_Rel_Source_Parent );
   if( ( id_or_name == c_field_id_Child_Rel_Source_Parent_Class ) || ( id_or_name == c_field_name_Child_Rel_Source_Parent_Class ) )
      display_name = get_module_string( c_field_display_name_Child_Rel_Source_Parent_Class );
   if( ( id_or_name == c_field_id_Child_Relationship ) || ( id_or_name == c_field_name_Child_Relationship ) )
      display_name = get_module_string( c_field_display_name_Child_Relationship );
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
      display_name = get_module_string( c_field_display_name_Class );
   if( ( id_or_name == c_field_id_Exact_Match_Only ) || ( id_or_name == c_field_name_Exact_Match_Only ) )
      display_name = get_module_string( c_field_display_name_Exact_Match_Only );
   if( ( id_or_name == c_field_id_Exclude_In_Use_FK ) || ( id_or_name == c_field_name_Exclude_In_Use_FK ) )
      display_name = get_module_string( c_field_display_name_Exclude_In_Use_FK );
   if( ( id_or_name == c_field_id_Font_Size ) || ( id_or_name == c_field_name_Font_Size ) )
      display_name = get_module_string( c_field_display_name_Font_Size );
   if( ( id_or_name == c_field_id_Hide_If_Is_Changing ) || ( id_or_name == c_field_name_Hide_If_Is_Changing ) )
      display_name = get_module_string( c_field_display_name_Hide_If_Is_Changing );
   if( ( id_or_name == c_field_id_Include_Key_Additions ) || ( id_or_name == c_field_name_Include_Key_Additions ) )
      display_name = get_module_string( c_field_display_name_Include_Key_Additions );
   if( ( id_or_name == c_field_id_Label_Class ) || ( id_or_name == c_field_name_Label_Class ) )
      display_name = get_module_string( c_field_display_name_Label_Class );
   if( ( id_or_name == c_field_id_Label_Source ) || ( id_or_name == c_field_name_Label_Source ) )
      display_name = get_module_string( c_field_display_name_Label_Source );
   if( ( id_or_name == c_field_id_Label_Source_Child ) || ( id_or_name == c_field_name_Label_Source_Child ) )
      display_name = get_module_string( c_field_display_name_Label_Source_Child );
   if( ( id_or_name == c_field_id_Link_Empty_Restriction ) || ( id_or_name == c_field_name_Link_Empty_Restriction ) )
      display_name = get_module_string( c_field_display_name_Link_Empty_Restriction );
   if( ( id_or_name == c_field_id_Link_Permission ) || ( id_or_name == c_field_name_Link_Permission ) )
      display_name = get_module_string( c_field_display_name_Link_Permission );
   if( ( id_or_name == c_field_id_Link_Restriction ) || ( id_or_name == c_field_name_Link_Restriction ) )
      display_name = get_module_string( c_field_display_name_Link_Restriction );
   if( ( id_or_name == c_field_id_Link_Type ) || ( id_or_name == c_field_name_Link_Type ) )
      display_name = get_module_string( c_field_display_name_Link_Type );
   if( ( id_or_name == c_field_id_List ) || ( id_or_name == c_field_name_List ) )
      display_name = get_module_string( c_field_display_name_List );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
      display_name = get_module_string( c_field_display_name_Name );
   if( ( id_or_name == c_field_id_Non_Instance_Procedure ) || ( id_or_name == c_field_name_Non_Instance_Procedure ) )
      display_name = get_module_string( c_field_display_name_Non_Instance_Procedure );
   if( ( id_or_name == c_field_id_Notes_Truncation ) || ( id_or_name == c_field_name_Notes_Truncation ) )
      display_name = get_module_string( c_field_display_name_Notes_Truncation );
   if( ( id_or_name == c_field_id_Omit_Versions ) || ( id_or_name == c_field_name_Omit_Versions ) )
      display_name = get_module_string( c_field_display_name_Omit_Versions );
   if( ( id_or_name == c_field_id_Order ) || ( id_or_name == c_field_name_Order ) )
      display_name = get_module_string( c_field_display_name_Order );
   if( ( id_or_name == c_field_id_Orientation ) || ( id_or_name == c_field_name_Orientation ) )
      display_name = get_module_string( c_field_display_name_Orientation );
   if( ( id_or_name == c_field_id_Parent_Class ) || ( id_or_name == c_field_name_Parent_Class ) )
      display_name = get_module_string( c_field_display_name_Parent_Class );
   if( ( id_or_name == c_field_id_Print_Type ) || ( id_or_name == c_field_name_Print_Type ) )
      display_name = get_module_string( c_field_display_name_Print_Type );
   if( ( id_or_name == c_field_id_Procedure ) || ( id_or_name == c_field_name_Procedure ) )
      display_name = get_module_string( c_field_display_name_Procedure );
   if( ( id_or_name == c_field_id_Procedure_Args ) || ( id_or_name == c_field_name_Procedure_Args ) )
      display_name = get_module_string( c_field_display_name_Procedure_Args );
   if( ( id_or_name == c_field_id_Restriction_Field ) || ( id_or_name == c_field_name_Restriction_Field ) )
      display_name = get_module_string( c_field_display_name_Restriction_Field );
   if( ( id_or_name == c_field_id_Restriction_Spec ) || ( id_or_name == c_field_name_Restriction_Spec ) )
      display_name = get_module_string( c_field_display_name_Restriction_Spec );
   if( ( id_or_name == c_field_id_Restriction_Value ) || ( id_or_name == c_field_name_Restriction_Value ) )
      display_name = get_module_string( c_field_display_name_Restriction_Value );
   if( ( id_or_name == c_field_id_Retain_Selected_Rows ) || ( id_or_name == c_field_name_Retain_Selected_Rows ) )
      display_name = get_module_string( c_field_display_name_Retain_Selected_Rows );
   if( ( id_or_name == c_field_id_Reverse_Order ) || ( id_or_name == c_field_name_Reverse_Order ) )
      display_name = get_module_string( c_field_display_name_Reverse_Order );
   if( ( id_or_name == c_field_id_Search_Option_Limit ) || ( id_or_name == c_field_name_Search_Option_Limit ) )
      display_name = get_module_string( c_field_display_name_Search_Option_Limit );
   if( ( id_or_name == c_field_id_Select_Key_Exclusions ) || ( id_or_name == c_field_name_Select_Key_Exclusions ) )
      display_name = get_module_string( c_field_display_name_Select_Key_Exclusions );
   if( ( id_or_name == c_field_id_Sort_Manually ) || ( id_or_name == c_field_name_Sort_Manually ) )
      display_name = get_module_string( c_field_display_name_Sort_Manually );
   if( ( id_or_name == c_field_id_Source_Child ) || ( id_or_name == c_field_name_Source_Child ) )
      display_name = get_module_string( c_field_display_name_Source_Child );
   if( ( id_or_name == c_field_id_Source_Child_Class ) || ( id_or_name == c_field_name_Source_Child_Class ) )
      display_name = get_module_string( c_field_display_name_Source_Child_Class );
   if( ( id_or_name == c_field_id_Source_Field ) || ( id_or_name == c_field_name_Source_Field ) )
      display_name = get_module_string( c_field_display_name_Source_Field );
   if( ( id_or_name == c_field_id_Source_Grandchild ) || ( id_or_name == c_field_name_Source_Grandchild ) )
      display_name = get_module_string( c_field_display_name_Source_Grandchild );
   if( ( id_or_name == c_field_id_Source_Parent ) || ( id_or_name == c_field_name_Source_Parent ) )
      display_name = get_module_string( c_field_display_name_Source_Parent );
   if( ( id_or_name == c_field_id_Source_Parent_Class ) || ( id_or_name == c_field_name_Source_Parent_Class ) )
      display_name = get_module_string( c_field_display_name_Source_Parent_Class );
   if( ( id_or_name == c_field_id_Switch_Type ) || ( id_or_name == c_field_name_Switch_Type ) )
      display_name = get_module_string( c_field_display_name_Switch_Type );
   if( ( id_or_name == c_field_id_Trigger_Option ) || ( id_or_name == c_field_name_Trigger_Option ) )
      display_name = get_module_string( c_field_display_name_Trigger_Option );
   if( ( id_or_name == c_field_id_Type ) || ( id_or_name == c_field_name_Type ) )
      display_name = get_module_string( c_field_display_name_Type );
   if( ( id_or_name == c_field_id_Use_Child_Rel_Source_Parent ) || ( id_or_name == c_field_name_Use_Child_Rel_Source_Parent ) )
      display_name = get_module_string( c_field_display_name_Use_Child_Rel_Source_Parent );
   if( ( id_or_name == c_field_id_Use_In_Text_Search_Title ) || ( id_or_name == c_field_name_Use_In_Text_Search_Title ) )
      display_name = get_module_string( c_field_display_name_Use_In_Text_Search_Title );
   if( ( id_or_name == c_field_id_Use_Source_Parent ) || ( id_or_name == c_field_name_Use_Source_Parent ) )
      display_name = get_module_string( c_field_display_name_Use_Source_Parent );
   if( ( id_or_name == c_field_id_Use_Type_Field ) || ( id_or_name == c_field_name_Use_Type_Field ) )
      display_name = get_module_string( c_field_display_name_Use_Type_Field );
   if( ( id_or_name == c_field_id_View_Parent_Extra ) || ( id_or_name == c_field_name_View_Parent_Extra ) )
      display_name = get_module_string( c_field_display_name_View_Parent_Extra );

   return display_name;
}

void Meta_List_Field::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_List_Field::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_List_Field::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_List_Field::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_List_Field::setup_foreign_key( Meta_Modifier& o, const string& value )
{
   static_cast< Meta_Modifier& >( o ).set_key( value );
}

void Meta_List_Field::setup_foreign_key( Meta_Permission& o, const string& value )
{
   static_cast< Meta_Permission& >( o ).set_key( value );
}

void Meta_List_Field::setup_foreign_key( Meta_Class& o, const string& value )
{
   static_cast< Meta_Class& >( o ).set_key( value );
}

void Meta_List_Field::setup_foreign_key( Meta_Field& o, const string& value )
{
   static_cast< Meta_Field& >( o ).set_key( value );
}

void Meta_List_Field::setup_foreign_key( Meta_Relationship& o, const string& value )
{
   static_cast< Meta_Relationship& >( o ).set_key( value );
}

void Meta_List_Field::setup_foreign_key( Meta_List& o, const string& value )
{
   static_cast< Meta_List& >( o ).set_key( value );
}

void Meta_List_Field::setup_foreign_key( Meta_Procedure& o, const string& value )
{
   static_cast< Meta_Procedure& >( o ).set_key( value );
}

void Meta_List_Field::setup_foreign_key( Meta_Specification& o, const string& value )
{
   static_cast< Meta_Specification& >( o ).set_key( value );
}

void Meta_List_Field::setup_foreign_key( Meta_List_Field_Type& o, const string& value )
{
   static_cast< Meta_List_Field_Type& >( o ).set_key( value );
}

void Meta_List_Field::setup_graph_parent(
 Meta_Modifier& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Modifier& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Modifier& >( o ).set_key( init_value );
}

void Meta_List_Field::setup_graph_parent(
 Meta_Permission& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Permission& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Permission& >( o ).set_key( init_value );
}

void Meta_List_Field::setup_graph_parent(
 Meta_Class& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Class& >( o ).set_key( init_value );
}

void Meta_List_Field::setup_graph_parent(
 Meta_Field& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Field& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Field& >( o ).set_key( init_value );
}

void Meta_List_Field::setup_graph_parent(
 Meta_Relationship& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Relationship& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Relationship& >( o ).set_key( init_value );
}

void Meta_List_Field::setup_graph_parent(
 Meta_List& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_List& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_List& >( o ).set_key( init_value );
}

void Meta_List_Field::setup_graph_parent(
 Meta_Procedure& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Procedure& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Procedure& >( o ).set_key( init_value );
}

void Meta_List_Field::setup_graph_parent(
 Meta_Specification& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Specification& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Specification& >( o ).set_key( init_value );
}

void Meta_List_Field::setup_graph_parent(
 Meta_List_Field_Type& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_List_Field_Type& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_List_Field_Type& >( o ).set_key( init_value );
}

size_t Meta_List_Field::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_List_Field::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_List_Field::get_num_foreign_key_children( bool is_internal ) const
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

class_base* Meta_List_Field::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   ( void )child_num;
   ( void )next_child_field;
   ( void )op;

   return p_class_base;
}

void Meta_List_Field::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_List_Field::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_List_Field::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_List_Field::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_List_Field::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_List_Field::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_List_Field::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_List_Field::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "list_field";

   return get_module_string( key );
}

string Meta_List_Field::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_List_Field::get_create_instance_info( ) const
{
   return "";
}

string Meta_List_Field::get_update_instance_info( ) const
{
   return "";
}

string Meta_List_Field::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_List_Field::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );
   else if( procedure_id == "124420" ) // i.e. Move_Down
      retval = "";
   else if( procedure_id == "124410" ) // i.e. Move_Up
      retval = "";

   return retval;
}

bool Meta_List_Field::get_is_alias( ) const
{
   return false;
}

void Meta_List_Field::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_List_Field::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_List_Field::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == c_field_id_Access_Parent_Modifier ) || ( sub_context == c_field_name_Access_Parent_Modifier ) )
      p_class_base = &Access_Parent_Modifier( );
   else if( ( sub_context == c_field_id_Access_Permission ) || ( sub_context == c_field_name_Access_Permission ) )
      p_class_base = &Access_Permission( );
   else if( ( sub_context == c_field_id_Child_Rel_Child_Class ) || ( sub_context == c_field_name_Child_Rel_Child_Class ) )
      p_class_base = &Child_Rel_Child_Class( );
   else if( ( sub_context == c_field_id_Child_Rel_Source_Child ) || ( sub_context == c_field_name_Child_Rel_Source_Child ) )
      p_class_base = &Child_Rel_Source_Child( );
   else if( ( sub_context == c_field_id_Child_Rel_Source_Field ) || ( sub_context == c_field_name_Child_Rel_Source_Field ) )
      p_class_base = &Child_Rel_Source_Field( );
   else if( ( sub_context == c_field_id_Child_Rel_Source_Parent ) || ( sub_context == c_field_name_Child_Rel_Source_Parent ) )
      p_class_base = &Child_Rel_Source_Parent( );
   else if( ( sub_context == c_field_id_Child_Rel_Source_Parent_Class ) || ( sub_context == c_field_name_Child_Rel_Source_Parent_Class ) )
      p_class_base = &Child_Rel_Source_Parent_Class( );
   else if( ( sub_context == c_field_id_Child_Relationship ) || ( sub_context == c_field_name_Child_Relationship ) )
      p_class_base = &Child_Relationship( );
   else if( ( sub_context == c_field_id_Class ) || ( sub_context == c_field_name_Class ) )
      p_class_base = &Class( );
   else if( ( sub_context == c_field_id_Link_Permission ) || ( sub_context == c_field_name_Link_Permission ) )
      p_class_base = &Link_Permission( );
   else if( ( sub_context == c_field_id_List ) || ( sub_context == c_field_name_List ) )
      p_class_base = &List( );
   else if( ( sub_context == c_field_id_Parent_Class ) || ( sub_context == c_field_name_Parent_Class ) )
      p_class_base = &Parent_Class( );
   else if( ( sub_context == c_field_id_Procedure ) || ( sub_context == c_field_name_Procedure ) )
      p_class_base = &Procedure( );
   else if( ( sub_context == c_field_id_Restriction_Field ) || ( sub_context == c_field_name_Restriction_Field ) )
      p_class_base = &Restriction_Field( );
   else if( ( sub_context == c_field_id_Restriction_Spec ) || ( sub_context == c_field_name_Restriction_Spec ) )
      p_class_base = &Restriction_Spec( );
   else if( ( sub_context == c_field_id_Source_Child ) || ( sub_context == c_field_name_Source_Child ) )
      p_class_base = &Source_Child( );
   else if( ( sub_context == c_field_id_Source_Child_Class ) || ( sub_context == c_field_name_Source_Child_Class ) )
      p_class_base = &Source_Child_Class( );
   else if( ( sub_context == c_field_id_Source_Field ) || ( sub_context == c_field_name_Source_Field ) )
      p_class_base = &Source_Field( );
   else if( ( sub_context == c_field_id_Source_Grandchild ) || ( sub_context == c_field_name_Source_Grandchild ) )
      p_class_base = &Source_Grandchild( );
   else if( ( sub_context == c_field_id_Source_Parent ) || ( sub_context == c_field_name_Source_Parent ) )
      p_class_base = &Source_Parent( );
   else if( ( sub_context == c_field_id_Source_Parent_Class ) || ( sub_context == c_field_name_Source_Parent_Class ) )
      p_class_base = &Source_Parent_Class( );
   else if( ( sub_context == c_field_id_Type ) || ( sub_context == c_field_name_Type ) )
      p_class_base = &Type( );
   else if( ( sub_context == c_field_id_Use_Type_Field ) || ( sub_context == c_field_name_Use_Type_Field ) )
      p_class_base = &Use_Type_Field( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_List_Field::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Access_Parent_Modifier" );
   names.push_back( "C_Access_Permission" );
   names.push_back( "C_Access_Restriction" );
   names.push_back( "C_Alignment" );
   names.push_back( "C_Allow_Anonymous_Access" );
   names.push_back( "C_Child_Rel_Child_Class" );
   names.push_back( "C_Child_Rel_Source_Child" );
   names.push_back( "C_Child_Rel_Source_Field" );
   names.push_back( "C_Child_Rel_Source_Parent" );
   names.push_back( "C_Child_Rel_Source_Parent_Class" );
   names.push_back( "C_Child_Relationship" );
   names.push_back( "C_Class" );
   names.push_back( "C_Exact_Match_Only" );
   names.push_back( "C_Exclude_In_Use_FK" );
   names.push_back( "C_Font_Size" );
   names.push_back( "C_Hide_If_Is_Changing" );
   names.push_back( "C_Include_Key_Additions" );
   names.push_back( "C_Label_Class" );
   names.push_back( "C_Label_Source" );
   names.push_back( "C_Label_Source_Child" );
   names.push_back( "C_Link_Empty_Restriction" );
   names.push_back( "C_Link_Permission" );
   names.push_back( "C_Link_Restriction" );
   names.push_back( "C_Link_Type" );
   names.push_back( "C_List" );
   names.push_back( "C_Non_Instance_Procedure" );
   names.push_back( "C_Notes_Truncation" );
   names.push_back( "C_Omit_Versions" );
   names.push_back( "C_Order" );
   names.push_back( "C_Orientation" );
   names.push_back( "C_Parent_Class" );
   names.push_back( "C_Print_Type" );
   names.push_back( "C_Procedure" );
   names.push_back( "C_Procedure_Args" );
   names.push_back( "C_Restriction_Field" );
   names.push_back( "C_Restriction_Spec" );
   names.push_back( "C_Restriction_Value" );
   names.push_back( "C_Retain_Selected_Rows" );
   names.push_back( "C_Reverse_Order" );
   names.push_back( "C_Search_Option_Limit" );
   names.push_back( "C_Select_Key_Exclusions" );
   names.push_back( "C_Sort_Manually" );
   names.push_back( "C_Source_Child" );
   names.push_back( "C_Source_Child_Class" );
   names.push_back( "C_Source_Field" );
   names.push_back( "C_Source_Grandchild" );
   names.push_back( "C_Source_Parent" );
   names.push_back( "C_Source_Parent_Class" );
   names.push_back( "C_Switch_Type" );
   names.push_back( "C_Trigger_Option" );
   names.push_back( "C_Type" );
   names.push_back( "C_Use_Child_Rel_Source_Parent" );
   names.push_back( "C_Use_In_Text_Search_Title" );
   names.push_back( "C_Use_Source_Parent" );
   names.push_back( "C_Use_Type_Field" );
   names.push_back( "C_View_Parent_Extra" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_List_Field::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Access_Parent_Modifier( ) ) ) );
   values.push_back( sql_quote( to_string( Access_Permission( ) ) ) );
   values.push_back( to_string( Access_Restriction( ) ) );
   values.push_back( to_string( Alignment( ) ) );
   values.push_back( to_string( Allow_Anonymous_Access( ) ) );
   values.push_back( sql_quote( to_string( Child_Rel_Child_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Child_Rel_Source_Child( ) ) ) );
   values.push_back( sql_quote( to_string( Child_Rel_Source_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Child_Rel_Source_Parent( ) ) ) );
   values.push_back( sql_quote( to_string( Child_Rel_Source_Parent_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Child_Relationship( ) ) ) );
   values.push_back( sql_quote( to_string( Class( ) ) ) );
   values.push_back( to_string( Exact_Match_Only( ) ) );
   values.push_back( to_string( Exclude_In_Use_FK( ) ) );
   values.push_back( to_string( Font_Size( ) ) );
   values.push_back( to_string( Hide_If_Is_Changing( ) ) );
   values.push_back( sql_quote( to_string( Include_Key_Additions( ) ) ) );
   values.push_back( to_string( Label_Class( ) ) );
   values.push_back( to_string( Label_Source( ) ) );
   values.push_back( to_string( Label_Source_Child( ) ) );
   values.push_back( to_string( Link_Empty_Restriction( ) ) );
   values.push_back( sql_quote( to_string( Link_Permission( ) ) ) );
   values.push_back( to_string( Link_Restriction( ) ) );
   values.push_back( to_string( Link_Type( ) ) );
   values.push_back( sql_quote( to_string( List( ) ) ) );
   values.push_back( to_string( Non_Instance_Procedure( ) ) );
   values.push_back( to_string( Notes_Truncation( ) ) );
   values.push_back( to_string( Omit_Versions( ) ) );
   values.push_back( sql_quote( to_string( Order( ) ) ) );
   values.push_back( to_string( Orientation( ) ) );
   values.push_back( sql_quote( to_string( Parent_Class( ) ) ) );
   values.push_back( to_string( Print_Type( ) ) );
   values.push_back( sql_quote( to_string( Procedure( ) ) ) );
   values.push_back( sql_quote( to_string( Procedure_Args( ) ) ) );
   values.push_back( sql_quote( to_string( Restriction_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Restriction_Spec( ) ) ) );
   values.push_back( sql_quote( to_string( Restriction_Value( ) ) ) );
   values.push_back( to_string( Retain_Selected_Rows( ) ) );
   values.push_back( to_string( Reverse_Order( ) ) );
   values.push_back( to_string( Search_Option_Limit( ) ) );
   values.push_back( sql_quote( to_string( Select_Key_Exclusions( ) ) ) );
   values.push_back( to_string( Sort_Manually( ) ) );
   values.push_back( sql_quote( to_string( Source_Child( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Child_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Grandchild( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Parent( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Parent_Class( ) ) ) );
   values.push_back( to_string( Switch_Type( ) ) );
   values.push_back( to_string( Trigger_Option( ) ) );
   values.push_back( sql_quote( to_string( Type( ) ) ) );
   values.push_back( to_string( Use_Child_Rel_Source_Parent( ) ) );
   values.push_back( to_string( Use_In_Text_Search_Title( ) ) );
   values.push_back( to_string( Use_Source_Parent( ) ) );
   values.push_back( sql_quote( to_string( Use_Type_Field( ) ) ) );
   values.push_back( to_string( View_Parent_Extra( ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_List_Field::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [(start field_from_search_replace)] 600459
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
      dependents.insert( "Source_Grandchild" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Grandchild ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Grandchild ) ) )
         names.insert( "Source_Grandchild" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Child_Relationship" );

      if( ( use_transients && is_field_transient( e_field_id_Child_Relationship ) )
       || ( !use_transients && !is_field_transient( e_field_id_Child_Relationship ) ) )
         names.insert( "Child_Relationship" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Child_Rel_Source_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Child_Rel_Source_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Child_Rel_Source_Field ) ) )
         names.insert( "Child_Rel_Source_Field" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Child_Rel_Source_Parent" );

      if( ( use_transients && is_field_transient( e_field_id_Child_Rel_Source_Parent ) )
       || ( !use_transients && !is_field_transient( e_field_id_Child_Rel_Source_Parent ) ) )
         names.insert( "Child_Rel_Source_Parent" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Child_Rel_Source_Child" );

      if( ( use_transients && is_field_transient( e_field_id_Child_Rel_Source_Child ) )
       || ( !use_transients && !is_field_transient( e_field_id_Child_Rel_Source_Child ) ) )
         names.insert( "Child_Rel_Source_Child" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Restriction_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Restriction_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Restriction_Field ) ) )
         names.insert( "Restriction_Field" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Procedure" );

      if( ( use_transients && is_field_transient( e_field_id_Procedure ) )
       || ( !use_transients && !is_field_transient( e_field_id_Procedure ) ) )
         names.insert( "Procedure" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Type" );

      if( ( use_transients && is_field_transient( e_field_id_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
         names.insert( "Type" );
   }
   // [(finish field_from_search_replace)] 600459

   // [(start field_from_other_field)] 600494
   if( needs_field_value( "Parent_Class", dependents ) )
   {
      dependents.insert( "List" );

      if( ( use_transients && is_field_transient( e_field_id_List ) )
       || ( !use_transients && !is_field_transient( e_field_id_List ) ) )
         names.insert( "List" );
   }
   // [(finish field_from_other_field)] 600494

   // [(start field_from_other_field)] 600497
   if( needs_field_value( "Source_Parent_Class", dependents ) )
   {
      dependents.insert( "Child_Rel_Source_Parent_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Child_Rel_Source_Parent_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Child_Rel_Source_Parent_Class ) ) )
         names.insert( "Child_Rel_Source_Parent_Class" );
   }
   // [(finish field_from_other_field)] 600497

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_List_Field::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start move_up_and_down)] 600448
   dependents.insert( "Order" );

   if( ( use_transients && is_field_transient( e_field_id_Order ) )
    || ( !use_transients && !is_field_transient( e_field_id_Order ) ) )
      names.insert( "Order" );
   // [(finish move_up_and_down)] 600448

   // [(start modifier_field_value)] 600460
   dependents.insert( "Use_Source_Parent" ); // (for Hide_FK_Fields modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Source_Parent ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Source_Parent ) ) )
      names.insert( "Use_Source_Parent" );
   // [(finish modifier_field_value)] 600460

   // [(start modifier_field_value)] 600461
   dependents.insert( "Use_Source_Parent" ); // (for Hide_Non_FK_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Source_Parent ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Source_Parent ) ) )
      names.insert( "Use_Source_Parent" );
   // [(finish modifier_field_value)] 600461

   // [(start modifier_field_value)] 600471
   dependents.insert( "Type" ); // (for Hide_Switch_Type modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600471

   // [(start modifier_field_value)] 600472
   dependents.insert( "Type" ); // (for Hide_Restriction_Spec modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600472

   // [(start modifier_field_value)] 600473
   dependents.insert( "Type" ); // (for Hide_Restriction_Value modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600473

   // [(start modifier_field_value)] 600474
   dependents.insert( "Type" ); // (for Hide_Exclude_In_Use_FK modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600474

   // [(start modifier_field_value)] 600475
   dependents.insert( "Type" ); // (for Hide_Link_Type modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600475

   // [(start modifier_field_value)] 600476
   dependents.insert( "Type" ); // (for Hide_Procedure_Fields modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600476

   // [(start modifier_field_value)] 600477
   dependents.insert( "Type" ); // (for Hide_Non_Procedure_Fields modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600477

   // [(start modifier_field_value)] 600478
   dependents.insert( "Type" ); // (for Hide_Non_Simple_Fields modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600478

   // [(start modifier_field_value)] 600479
   dependents.insert( "Type" ); // (for Hide_Fields_For_Trivial_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600479

   // [(start modifier_field_value)] 600480
   dependents.insert( "Type" ); // (for Hide_Select_Specifics modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600480

   // [(start modifier_field_value)] 600481
   dependents.insert( "Type" ); // (for Hide_Link_Specifics modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600481

   // [(start modifier_field_value)] 600481a
   dependents.insert( "Type" ); // (for Hide_Link_Empty_Restriction modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600481a

   // [(start modifier_field_value)] 600481b
   dependents.insert( "Type" ); // (for Hide_Sort_Manually modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600481b

   // [(start modifier_field_value)] 600482
   dependents.insert( "Use_Child_Rel_Source_Parent" ); // (for Hide_Child_Rel_FK_Fields modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Child_Rel_Source_Parent ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Child_Rel_Source_Parent ) ) )
      names.insert( "Use_Child_Rel_Source_Parent" );
   // [(finish modifier_field_value)] 600482

   // [(start modifier_field_value)] 600483
   dependents.insert( "Use_Child_Rel_Source_Parent" ); // (for Hide_Child_Rel_Non_FK_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Child_Rel_Source_Parent ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Child_Rel_Source_Parent ) ) )
      names.insert( "Use_Child_Rel_Source_Parent" );
   // [(finish modifier_field_value)] 600483

   // [(start modifier_field_value)] 600486
   dependents.insert( "Type" ); // (for Hide_Child_Rel_Select_Specifics modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600486

   // [(start modifier_field_value)] 600487
   dependents.insert( "Type" ); // (for Hide_For_Child_Rel_Select modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600487

   // [(start modifier_field_value)] 600488
   dependents.insert( "Type" ); // (for Hide_Search_Option_Limit modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600488

   // [(start modifier_field_value)] 600489
   dependents.insert( "Type" ); // (for Hide_Restriction_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600489

   // [(start modifier_field_value)] 600490
   dependents.insert( "Type" ); // (for Hide_For_Restriction_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600490

   // [(start modifier_field_null)] 600495
   dependents.insert( "Parent_Class" ); // (for Is_Not_View_Child modifier)

   if( ( use_transients && is_field_transient( e_field_id_Parent_Class ) )
    || ( !use_transients && !is_field_transient( e_field_id_Parent_Class ) ) )
      names.insert( "Parent_Class" );
   // [(finish modifier_field_null)] 600495

   // [(start modifier_field_value)] 600496
   dependents.insert( "Type" ); // (for Is_Restrict_Search modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600496

   // [(start modifier_field_value)] 600496a
   dependents.insert( "Type" ); // (for Is_Not_Restrict_Search modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600496a

   // [(start modifier_field_value)] 600498
   dependents.insert( "Type" ); // (for Hide_Link_Permission_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600498

   // [(start modifier_field_value)] 600499
   dependents.insert( "Type" ); // (for Hide_View_Parent_Extra modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600499

   // [(start modifier_field_value)] 600499a
   dependents.insert( "Non_Instance_Procedure" ); // (for Is_Non_Instance_Procedure modifier)

   if( ( use_transients && is_field_transient( e_field_id_Non_Instance_Procedure ) )
    || ( !use_transients && !is_field_transient( e_field_id_Non_Instance_Procedure ) ) )
      names.insert( "Non_Instance_Procedure" );
   // [(finish modifier_field_value)] 600499a

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_List_Field::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_List_Field::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_List_Field::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_List_Field::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_List_Field::static_lock_class_id( )
{
   return "124100";
}

const char* Meta_List_Field::static_check_class_name( )
{
   return "List_Field";
}

const char* Meta_List_Field::static_persistence_extra( )
{
   return "";
}

bool Meta_List_Field::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_List_Field::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.124100" );
}

void Meta_List_Field::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "302199", "Access_Parent_Modifier", "Meta_Modifier", false, "", "" ) );
   all_field_info.push_back( field_info( "302130", "Access_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "124103", "Access_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "124119", "Alignment", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "124133", "Allow_Anonymous_Access", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "302194", "Child_Rel_Child_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "302198", "Child_Rel_Source_Child", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "302195", "Child_Rel_Source_Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "302196", "Child_Rel_Source_Parent", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "302197", "Child_Rel_Source_Parent_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "302193", "Child_Relationship", "Meta_Relationship", false, "", "" ) );
   all_field_info.push_back( field_info( "302110", "Class", "Meta_Class", true, "", "" ) );
   all_field_info.push_back( field_info( "124128", "Exact_Match_Only", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "124107", "Exclude_In_Use_FK", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "124130", "Font_Size", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "124137", "Hide_If_Is_Changing", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "124122", "Include_Key_Additions", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "124131", "Label_Class", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "124120", "Label_Source", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "124129", "Label_Source_Child", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "124134", "Link_Empty_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "302135", "Link_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "124108", "Link_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "124118", "Link_Type", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "302100", "List", "Meta_List", true, "", "" ) );
   all_field_info.push_back( field_info( "124102", "Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "124127", "Non_Instance_Procedure", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "124125", "Notes_Truncation", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "124110", "Omit_Versions", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "124101", "Order", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "124136", "Orientation", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "302115", "Parent_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "124115", "Print_Type", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "302190", "Procedure", "Meta_Procedure", false, "", "" ) );
   all_field_info.push_back( field_info( "124109", "Procedure_Args", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "302185", "Restriction_Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "302180", "Restriction_Spec", "Meta_Specification", false, "", "" ) );
   all_field_info.push_back( field_info( "124106", "Restriction_Value", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "124126", "Retain_Selected_Rows", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "124111", "Reverse_Order", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "124124", "Search_Option_Limit", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "124117", "Select_Key_Exclusions", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "124135", "Sort_Manually", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "302160", "Source_Child", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "302175", "Source_Child_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "302140", "Source_Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "302165", "Source_Grandchild", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "302150", "Source_Parent", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "302170", "Source_Parent_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "124105", "Switch_Type", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "124121", "Trigger_Option", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "302120", "Type", "Meta_List_Field_Type", true, "", "" ) );
   all_field_info.push_back( field_info( "124123", "Use_Child_Rel_Source_Parent", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "124116", "Use_In_Text_Search_Title", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "124104", "Use_Source_Parent", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "302176", "Use_Type_Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "124132", "View_Parent_Extra", "int", false, "", "" ) );
}

void Meta_List_Field::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Access_Parent_Modifier, make_pair( "Meta.124100", "Meta_Modifier" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Access_Permission, make_pair( "Meta.124100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Child_Rel_Child_Class, make_pair( "Meta.124100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Child_Rel_Source_Child, make_pair( "Meta.124100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Child_Rel_Source_Field, make_pair( "Meta.124100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Child_Rel_Source_Parent, make_pair( "Meta.124100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Child_Rel_Source_Parent_Class, make_pair( "Meta.124100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Child_Relationship, make_pair( "Meta.124100", "Meta_Relationship" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Class, make_pair( "Meta.124100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Link_Permission, make_pair( "Meta.124100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_List, make_pair( "Meta.124100", "Meta_List" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Parent_Class, make_pair( "Meta.124100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Procedure, make_pair( "Meta.124100", "Meta_Procedure" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Restriction_Field, make_pair( "Meta.124100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Restriction_Spec, make_pair( "Meta.124100", "Meta_Specification" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Child, make_pair( "Meta.124100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Child_Class, make_pair( "Meta.124100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Field, make_pair( "Meta.124100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Grandchild, make_pair( "Meta.124100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Parent, make_pair( "Meta.124100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Parent_Class, make_pair( "Meta.124100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Type, make_pair( "Meta.124100", "Meta_List_Field_Type" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Use_Type_Field, make_pair( "Meta.124100", "Meta_Field" ) ) );
}

int Meta_List_Field::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_List_Field::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_List_Field::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_List_Field::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "302199";
      break;

      case 2:
      p_id = "302130";
      break;

      case 3:
      p_id = "124103";
      break;

      case 4:
      p_id = "124119";
      break;

      case 5:
      p_id = "124133";
      break;

      case 6:
      p_id = "302194";
      break;

      case 7:
      p_id = "302198";
      break;

      case 8:
      p_id = "302195";
      break;

      case 9:
      p_id = "302196";
      break;

      case 10:
      p_id = "302197";
      break;

      case 11:
      p_id = "302193";
      break;

      case 12:
      p_id = "302110";
      break;

      case 13:
      p_id = "124128";
      break;

      case 14:
      p_id = "124107";
      break;

      case 15:
      p_id = "124130";
      break;

      case 16:
      p_id = "124137";
      break;

      case 17:
      p_id = "124122";
      break;

      case 18:
      p_id = "124131";
      break;

      case 19:
      p_id = "124120";
      break;

      case 20:
      p_id = "124129";
      break;

      case 21:
      p_id = "124134";
      break;

      case 22:
      p_id = "302135";
      break;

      case 23:
      p_id = "124108";
      break;

      case 24:
      p_id = "124118";
      break;

      case 25:
      p_id = "302100";
      break;

      case 26:
      p_id = "124102";
      break;

      case 27:
      p_id = "124127";
      break;

      case 28:
      p_id = "124125";
      break;

      case 29:
      p_id = "124110";
      break;

      case 30:
      p_id = "124101";
      break;

      case 31:
      p_id = "124136";
      break;

      case 32:
      p_id = "302115";
      break;

      case 33:
      p_id = "124115";
      break;

      case 34:
      p_id = "302190";
      break;

      case 35:
      p_id = "124109";
      break;

      case 36:
      p_id = "302185";
      break;

      case 37:
      p_id = "302180";
      break;

      case 38:
      p_id = "124106";
      break;

      case 39:
      p_id = "124126";
      break;

      case 40:
      p_id = "124111";
      break;

      case 41:
      p_id = "124124";
      break;

      case 42:
      p_id = "124117";
      break;

      case 43:
      p_id = "124135";
      break;

      case 44:
      p_id = "302160";
      break;

      case 45:
      p_id = "302175";
      break;

      case 46:
      p_id = "302140";
      break;

      case 47:
      p_id = "302165";
      break;

      case 48:
      p_id = "302150";
      break;

      case 49:
      p_id = "302170";
      break;

      case 50:
      p_id = "124105";
      break;

      case 51:
      p_id = "124121";
      break;

      case 52:
      p_id = "302120";
      break;

      case 53:
      p_id = "124123";
      break;

      case 54:
      p_id = "124116";
      break;

      case 55:
      p_id = "124104";
      break;

      case 56:
      p_id = "302176";
      break;

      case 57:
      p_id = "124132";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class List_Field" );

   return p_id;
}

const char* Meta_List_Field::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Access_Parent_Modifier";
      break;

      case 2:
      p_id = "Access_Permission";
      break;

      case 3:
      p_id = "Access_Restriction";
      break;

      case 4:
      p_id = "Alignment";
      break;

      case 5:
      p_id = "Allow_Anonymous_Access";
      break;

      case 6:
      p_id = "Child_Rel_Child_Class";
      break;

      case 7:
      p_id = "Child_Rel_Source_Child";
      break;

      case 8:
      p_id = "Child_Rel_Source_Field";
      break;

      case 9:
      p_id = "Child_Rel_Source_Parent";
      break;

      case 10:
      p_id = "Child_Rel_Source_Parent_Class";
      break;

      case 11:
      p_id = "Child_Relationship";
      break;

      case 12:
      p_id = "Class";
      break;

      case 13:
      p_id = "Exact_Match_Only";
      break;

      case 14:
      p_id = "Exclude_In_Use_FK";
      break;

      case 15:
      p_id = "Font_Size";
      break;

      case 16:
      p_id = "Hide_If_Is_Changing";
      break;

      case 17:
      p_id = "Include_Key_Additions";
      break;

      case 18:
      p_id = "Label_Class";
      break;

      case 19:
      p_id = "Label_Source";
      break;

      case 20:
      p_id = "Label_Source_Child";
      break;

      case 21:
      p_id = "Link_Empty_Restriction";
      break;

      case 22:
      p_id = "Link_Permission";
      break;

      case 23:
      p_id = "Link_Restriction";
      break;

      case 24:
      p_id = "Link_Type";
      break;

      case 25:
      p_id = "List";
      break;

      case 26:
      p_id = "Name";
      break;

      case 27:
      p_id = "Non_Instance_Procedure";
      break;

      case 28:
      p_id = "Notes_Truncation";
      break;

      case 29:
      p_id = "Omit_Versions";
      break;

      case 30:
      p_id = "Order";
      break;

      case 31:
      p_id = "Orientation";
      break;

      case 32:
      p_id = "Parent_Class";
      break;

      case 33:
      p_id = "Print_Type";
      break;

      case 34:
      p_id = "Procedure";
      break;

      case 35:
      p_id = "Procedure_Args";
      break;

      case 36:
      p_id = "Restriction_Field";
      break;

      case 37:
      p_id = "Restriction_Spec";
      break;

      case 38:
      p_id = "Restriction_Value";
      break;

      case 39:
      p_id = "Retain_Selected_Rows";
      break;

      case 40:
      p_id = "Reverse_Order";
      break;

      case 41:
      p_id = "Search_Option_Limit";
      break;

      case 42:
      p_id = "Select_Key_Exclusions";
      break;

      case 43:
      p_id = "Sort_Manually";
      break;

      case 44:
      p_id = "Source_Child";
      break;

      case 45:
      p_id = "Source_Child_Class";
      break;

      case 46:
      p_id = "Source_Field";
      break;

      case 47:
      p_id = "Source_Grandchild";
      break;

      case 48:
      p_id = "Source_Parent";
      break;

      case 49:
      p_id = "Source_Parent_Class";
      break;

      case 50:
      p_id = "Switch_Type";
      break;

      case 51:
      p_id = "Trigger_Option";
      break;

      case 52:
      p_id = "Type";
      break;

      case 53:
      p_id = "Use_Child_Rel_Source_Parent";
      break;

      case 54:
      p_id = "Use_In_Text_Search_Title";
      break;

      case 55:
      p_id = "Use_Source_Parent";
      break;

      case 56:
      p_id = "Use_Type_Field";
      break;

      case 57:
      p_id = "View_Parent_Extra";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class List_Field" );

   return p_id;
}

int Meta_List_Field::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Access_Parent_Modifier || field == c_field_name_Access_Parent_Modifier )
      rc += 1;
   else if( field == c_field_id_Access_Permission || field == c_field_name_Access_Permission )
      rc += 2;
   else if( field == c_field_id_Access_Restriction || field == c_field_name_Access_Restriction )
      rc += 3;
   else if( field == c_field_id_Alignment || field == c_field_name_Alignment )
      rc += 4;
   else if( field == c_field_id_Allow_Anonymous_Access || field == c_field_name_Allow_Anonymous_Access )
      rc += 5;
   else if( field == c_field_id_Child_Rel_Child_Class || field == c_field_name_Child_Rel_Child_Class )
      rc += 6;
   else if( field == c_field_id_Child_Rel_Source_Child || field == c_field_name_Child_Rel_Source_Child )
      rc += 7;
   else if( field == c_field_id_Child_Rel_Source_Field || field == c_field_name_Child_Rel_Source_Field )
      rc += 8;
   else if( field == c_field_id_Child_Rel_Source_Parent || field == c_field_name_Child_Rel_Source_Parent )
      rc += 9;
   else if( field == c_field_id_Child_Rel_Source_Parent_Class || field == c_field_name_Child_Rel_Source_Parent_Class )
      rc += 10;
   else if( field == c_field_id_Child_Relationship || field == c_field_name_Child_Relationship )
      rc += 11;
   else if( field == c_field_id_Class || field == c_field_name_Class )
      rc += 12;
   else if( field == c_field_id_Exact_Match_Only || field == c_field_name_Exact_Match_Only )
      rc += 13;
   else if( field == c_field_id_Exclude_In_Use_FK || field == c_field_name_Exclude_In_Use_FK )
      rc += 14;
   else if( field == c_field_id_Font_Size || field == c_field_name_Font_Size )
      rc += 15;
   else if( field == c_field_id_Hide_If_Is_Changing || field == c_field_name_Hide_If_Is_Changing )
      rc += 16;
   else if( field == c_field_id_Include_Key_Additions || field == c_field_name_Include_Key_Additions )
      rc += 17;
   else if( field == c_field_id_Label_Class || field == c_field_name_Label_Class )
      rc += 18;
   else if( field == c_field_id_Label_Source || field == c_field_name_Label_Source )
      rc += 19;
   else if( field == c_field_id_Label_Source_Child || field == c_field_name_Label_Source_Child )
      rc += 20;
   else if( field == c_field_id_Link_Empty_Restriction || field == c_field_name_Link_Empty_Restriction )
      rc += 21;
   else if( field == c_field_id_Link_Permission || field == c_field_name_Link_Permission )
      rc += 22;
   else if( field == c_field_id_Link_Restriction || field == c_field_name_Link_Restriction )
      rc += 23;
   else if( field == c_field_id_Link_Type || field == c_field_name_Link_Type )
      rc += 24;
   else if( field == c_field_id_List || field == c_field_name_List )
      rc += 25;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 26;
   else if( field == c_field_id_Non_Instance_Procedure || field == c_field_name_Non_Instance_Procedure )
      rc += 27;
   else if( field == c_field_id_Notes_Truncation || field == c_field_name_Notes_Truncation )
      rc += 28;
   else if( field == c_field_id_Omit_Versions || field == c_field_name_Omit_Versions )
      rc += 29;
   else if( field == c_field_id_Order || field == c_field_name_Order )
      rc += 30;
   else if( field == c_field_id_Orientation || field == c_field_name_Orientation )
      rc += 31;
   else if( field == c_field_id_Parent_Class || field == c_field_name_Parent_Class )
      rc += 32;
   else if( field == c_field_id_Print_Type || field == c_field_name_Print_Type )
      rc += 33;
   else if( field == c_field_id_Procedure || field == c_field_name_Procedure )
      rc += 34;
   else if( field == c_field_id_Procedure_Args || field == c_field_name_Procedure_Args )
      rc += 35;
   else if( field == c_field_id_Restriction_Field || field == c_field_name_Restriction_Field )
      rc += 36;
   else if( field == c_field_id_Restriction_Spec || field == c_field_name_Restriction_Spec )
      rc += 37;
   else if( field == c_field_id_Restriction_Value || field == c_field_name_Restriction_Value )
      rc += 38;
   else if( field == c_field_id_Retain_Selected_Rows || field == c_field_name_Retain_Selected_Rows )
      rc += 39;
   else if( field == c_field_id_Reverse_Order || field == c_field_name_Reverse_Order )
      rc += 40;
   else if( field == c_field_id_Search_Option_Limit || field == c_field_name_Search_Option_Limit )
      rc += 41;
   else if( field == c_field_id_Select_Key_Exclusions || field == c_field_name_Select_Key_Exclusions )
      rc += 42;
   else if( field == c_field_id_Sort_Manually || field == c_field_name_Sort_Manually )
      rc += 43;
   else if( field == c_field_id_Source_Child || field == c_field_name_Source_Child )
      rc += 44;
   else if( field == c_field_id_Source_Child_Class || field == c_field_name_Source_Child_Class )
      rc += 45;
   else if( field == c_field_id_Source_Field || field == c_field_name_Source_Field )
      rc += 46;
   else if( field == c_field_id_Source_Grandchild || field == c_field_name_Source_Grandchild )
      rc += 47;
   else if( field == c_field_id_Source_Parent || field == c_field_name_Source_Parent )
      rc += 48;
   else if( field == c_field_id_Source_Parent_Class || field == c_field_name_Source_Parent_Class )
      rc += 49;
   else if( field == c_field_id_Switch_Type || field == c_field_name_Switch_Type )
      rc += 50;
   else if( field == c_field_id_Trigger_Option || field == c_field_name_Trigger_Option )
      rc += 51;
   else if( field == c_field_id_Type || field == c_field_name_Type )
      rc += 52;
   else if( field == c_field_id_Use_Child_Rel_Source_Parent || field == c_field_name_Use_Child_Rel_Source_Parent )
      rc += 53;
   else if( field == c_field_id_Use_In_Text_Search_Title || field == c_field_name_Use_In_Text_Search_Title )
      rc += 54;
   else if( field == c_field_id_Use_Source_Parent || field == c_field_name_Use_Source_Parent )
      rc += 55;
   else if( field == c_field_id_Use_Type_Field || field == c_field_name_Use_Type_Field )
      rc += 56;
   else if( field == c_field_id_View_Parent_Extra || field == c_field_name_View_Parent_Extra )
      rc += 57;

   return rc - 1;
}

procedure_info_container& Meta_List_Field::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "124420", procedure_info( "Move_Down" ) ) );
      procedures.insert( make_pair( "124410", procedure_info( "Move_Up" ) ) );
   }

   return procedures;
}

string Meta_List_Field::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(75),"
    "C_Ver_ SMALLINT UNSIGNED NOT NULL,"
    "C_Rev_ BIGINT UNSIGNED NOT NULL,"
    "C_Sec_ BIGINT UNSIGNED NOT NULL,"
    "C_Typ_ VARCHAR(16) NOT NULL,"
    "C_Access_Parent_Modifier VARCHAR(75) NOT NULL,"
    "C_Access_Permission VARCHAR(75) NOT NULL,"
    "C_Access_Restriction INTEGER NOT NULL,"
    "C_Alignment INTEGER NOT NULL,"
    "C_Allow_Anonymous_Access INTEGER NOT NULL,"
    "C_Child_Rel_Child_Class VARCHAR(75) NOT NULL,"
    "C_Child_Rel_Source_Child VARCHAR(75) NOT NULL,"
    "C_Child_Rel_Source_Field VARCHAR(75) NOT NULL,"
    "C_Child_Rel_Source_Parent VARCHAR(75) NOT NULL,"
    "C_Child_Rel_Source_Parent_Class VARCHAR(75) NOT NULL,"
    "C_Child_Relationship VARCHAR(75) NOT NULL,"
    "C_Class VARCHAR(75) NOT NULL,"
    "C_Exact_Match_Only INTEGER NOT NULL,"
    "C_Exclude_In_Use_FK INTEGER NOT NULL,"
    "C_Font_Size INTEGER NOT NULL,"
    "C_Hide_If_Is_Changing INTEGER NOT NULL,"
    "C_Include_Key_Additions VARCHAR(200) NOT NULL,"
    "C_Label_Class INTEGER NOT NULL,"
    "C_Label_Source INTEGER NOT NULL,"
    "C_Label_Source_Child INTEGER NOT NULL,"
    "C_Link_Empty_Restriction INTEGER NOT NULL,"
    "C_Link_Permission VARCHAR(75) NOT NULL,"
    "C_Link_Restriction INTEGER NOT NULL,"
    "C_Link_Type INTEGER NOT NULL,"
    "C_List VARCHAR(75) NOT NULL,"
    "C_Non_Instance_Procedure INTEGER NOT NULL,"
    "C_Notes_Truncation INTEGER NOT NULL,"
    "C_Omit_Versions INTEGER NOT NULL,"
    "C_Order VARCHAR(200) NOT NULL,"
    "C_Orientation INTEGER NOT NULL,"
    "C_Parent_Class VARCHAR(75) NOT NULL,"
    "C_Print_Type INTEGER NOT NULL,"
    "C_Procedure VARCHAR(256) NOT NULL,"
    "C_Procedure_Args VARCHAR(256) NOT NULL,"
    "C_Restriction_Field VARCHAR(256) NOT NULL,"
    "C_Restriction_Spec VARCHAR(256) NOT NULL,"
    "C_Restriction_Value VARCHAR(256) NOT NULL,"
    "C_Retain_Selected_Rows INTEGER NOT NULL,"
    "C_Reverse_Order INTEGER NOT NULL,"
    "C_Search_Option_Limit INTEGER NOT NULL,"
    "C_Select_Key_Exclusions VARCHAR(256) NOT NULL,"
    "C_Sort_Manually INTEGER NOT NULL,"
    "C_Source_Child VARCHAR(256) NOT NULL,"
    "C_Source_Child_Class VARCHAR(256) NOT NULL,"
    "C_Source_Field VARCHAR(256) NOT NULL,"
    "C_Source_Grandchild VARCHAR(256) NOT NULL,"
    "C_Source_Parent VARCHAR(256) NOT NULL,"
    "C_Source_Parent_Class VARCHAR(256) NOT NULL,"
    "C_Switch_Type INTEGER NOT NULL,"
    "C_Trigger_Option INTEGER NOT NULL,"
    "C_Type VARCHAR(256) NOT NULL,"
    "C_Use_Child_Rel_Source_Parent INTEGER NOT NULL,"
    "C_Use_In_Text_Search_Title INTEGER NOT NULL,"
    "C_Use_Source_Parent INTEGER NOT NULL,"
    "C_Use_Type_Field VARCHAR(256) NOT NULL,"
    "C_View_Parent_Extra INTEGER NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_List_Field::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_List_Field::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "enum_list_field_restrict_0", get_enum_string_list_field_restrict( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_field_restrict_1", get_enum_string_list_field_restrict( 1 ) ) );
   pairs.push_back( make_pair( "enum_list_field_restrict_2", get_enum_string_list_field_restrict( 2 ) ) );
   pairs.push_back( make_pair( "enum_list_field_restrict_3", get_enum_string_list_field_restrict( 3 ) ) );
   pairs.push_back( make_pair( "enum_list_field_restrict_4", get_enum_string_list_field_restrict( 4 ) ) );

   pairs.push_back( make_pair( "enum_field_alignment_0", get_enum_string_field_alignment( 0 ) ) );
   pairs.push_back( make_pair( "enum_field_alignment_1", get_enum_string_field_alignment( 1 ) ) );
   pairs.push_back( make_pair( "enum_field_alignment_2", get_enum_string_field_alignment( 2 ) ) );
   pairs.push_back( make_pair( "enum_field_alignment_3", get_enum_string_field_alignment( 3 ) ) );
   pairs.push_back( make_pair( "enum_field_alignment_4", get_enum_string_field_alignment( 4 ) ) );

   pairs.push_back( make_pair( "enum_font_size_0", get_enum_string_font_size( 0 ) ) );
   pairs.push_back( make_pair( "enum_font_size_1", get_enum_string_font_size( 1 ) ) );
   pairs.push_back( make_pair( "enum_font_size_2", get_enum_string_font_size( 2 ) ) );
   pairs.push_back( make_pair( "enum_font_size_5", get_enum_string_font_size( 5 ) ) );
   pairs.push_back( make_pair( "enum_font_size_6", get_enum_string_font_size( 6 ) ) );

   pairs.push_back( make_pair( "enum_list_field_label_class_0", get_enum_string_list_field_label_class( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_field_label_class_1", get_enum_string_list_field_label_class( 1 ) ) );

   pairs.push_back( make_pair( "enum_list_field_label_source_0", get_enum_string_list_field_label_source( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_field_label_source_1", get_enum_string_list_field_label_source( 1 ) ) );
   pairs.push_back( make_pair( "enum_list_field_label_source_2", get_enum_string_list_field_label_source( 2 ) ) );

   pairs.push_back( make_pair( "enum_label_source_child_0", get_enum_string_label_source_child( 0 ) ) );
   pairs.push_back( make_pair( "enum_label_source_child_1", get_enum_string_label_source_child( 1 ) ) );
   pairs.push_back( make_pair( "enum_label_source_child_2", get_enum_string_label_source_child( 2 ) ) );

   pairs.push_back( make_pair( "enum_list_field_link_restrict_0", get_enum_string_list_field_link_restrict( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_field_link_restrict_1", get_enum_string_list_field_link_restrict( 1 ) ) );
   pairs.push_back( make_pair( "enum_list_field_link_restrict_2", get_enum_string_list_field_link_restrict( 2 ) ) );
   pairs.push_back( make_pair( "enum_list_field_link_restrict_3", get_enum_string_list_field_link_restrict( 3 ) ) );
   pairs.push_back( make_pair( "enum_list_field_link_restrict_4", get_enum_string_list_field_link_restrict( 4 ) ) );

   pairs.push_back( make_pair( "enum_list_field_link_type_0", get_enum_string_list_field_link_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_field_link_type_1", get_enum_string_list_field_link_type( 1 ) ) );

   pairs.push_back( make_pair( "enum_list_field_notes_truncation_1", get_enum_string_list_field_notes_truncation( 1 ) ) );
   pairs.push_back( make_pair( "enum_list_field_notes_truncation_0", get_enum_string_list_field_notes_truncation( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_field_notes_truncation_30", get_enum_string_list_field_notes_truncation( 30 ) ) );
   pairs.push_back( make_pair( "enum_list_field_notes_truncation_40", get_enum_string_list_field_notes_truncation( 40 ) ) );
   pairs.push_back( make_pair( "enum_list_field_notes_truncation_50", get_enum_string_list_field_notes_truncation( 50 ) ) );
   pairs.push_back( make_pair( "enum_list_field_notes_truncation_80", get_enum_string_list_field_notes_truncation( 80 ) ) );
   pairs.push_back( make_pair( "enum_list_field_notes_truncation_100", get_enum_string_list_field_notes_truncation( 100 ) ) );

   pairs.push_back( make_pair( "enum_orientation_0", get_enum_string_orientation( 0 ) ) );
   pairs.push_back( make_pair( "enum_orientation_1", get_enum_string_orientation( 1 ) ) );
   pairs.push_back( make_pair( "enum_orientation_2", get_enum_string_orientation( 2 ) ) );

   pairs.push_back( make_pair( "enum_list_field_print_type_0", get_enum_string_list_field_print_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_field_print_type_1", get_enum_string_list_field_print_type( 1 ) ) );
   pairs.push_back( make_pair( "enum_list_field_print_type_2", get_enum_string_list_field_print_type( 2 ) ) );
   pairs.push_back( make_pair( "enum_list_field_print_type_3", get_enum_string_list_field_print_type( 3 ) ) );
   pairs.push_back( make_pair( "enum_list_field_print_type_4", get_enum_string_list_field_print_type( 4 ) ) );
   pairs.push_back( make_pair( "enum_list_field_print_type_5", get_enum_string_list_field_print_type( 5 ) ) );
   pairs.push_back( make_pair( "enum_list_field_print_type_6", get_enum_string_list_field_print_type( 6 ) ) );

   pairs.push_back( make_pair( "enum_list_search_opt_limit_0", get_enum_string_list_search_opt_limit( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_search_opt_limit_1", get_enum_string_list_search_opt_limit( 1 ) ) );
   pairs.push_back( make_pair( "enum_list_search_opt_limit_2", get_enum_string_list_search_opt_limit( 2 ) ) );
   pairs.push_back( make_pair( "enum_list_search_opt_limit_3", get_enum_string_list_search_opt_limit( 3 ) ) );
   pairs.push_back( make_pair( "enum_list_search_opt_limit_4", get_enum_string_list_search_opt_limit( 4 ) ) );
   pairs.push_back( make_pair( "enum_list_search_opt_limit_5", get_enum_string_list_search_opt_limit( 5 ) ) );
   pairs.push_back( make_pair( "enum_list_search_opt_limit_6", get_enum_string_list_search_opt_limit( 6 ) ) );
   pairs.push_back( make_pair( "enum_list_search_opt_limit_7", get_enum_string_list_search_opt_limit( 7 ) ) );

   pairs.push_back( make_pair( "enum_list_field_switch_type_0", get_enum_string_list_field_switch_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_field_switch_type_1", get_enum_string_list_field_switch_type( 1 ) ) );
   pairs.push_back( make_pair( "enum_list_field_switch_type_2", get_enum_string_list_field_switch_type( 2 ) ) );
   pairs.push_back( make_pair( "enum_list_field_switch_type_3", get_enum_string_list_field_switch_type( 3 ) ) );

   pairs.push_back( make_pair( "enum_list_field_trigger_option_0", get_enum_string_list_field_trigger_option( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_field_trigger_option_1", get_enum_string_list_field_trigger_option( 1 ) ) );
   pairs.push_back( make_pair( "enum_list_field_trigger_option_2", get_enum_string_list_field_trigger_option( 2 ) ) );
   pairs.push_back( make_pair( "enum_list_field_trigger_option_3", get_enum_string_list_field_trigger_option( 3 ) ) );
   pairs.push_back( make_pair( "enum_list_field_trigger_option_4", get_enum_string_list_field_trigger_option( 4 ) ) );
   pairs.push_back( make_pair( "enum_list_field_trigger_option_5", get_enum_string_list_field_trigger_option( 5 ) ) );
   pairs.push_back( make_pair( "enum_list_field_trigger_option_6", get_enum_string_list_field_trigger_option( 6 ) ) );
   pairs.push_back( make_pair( "enum_list_field_trigger_option_7", get_enum_string_list_field_trigger_option( 7 ) ) );
   pairs.push_back( make_pair( "enum_list_field_trigger_option_8", get_enum_string_list_field_trigger_option( 8 ) ) );
   pairs.push_back( make_pair( "enum_list_field_trigger_option_9", get_enum_string_list_field_trigger_option( 9 ) ) );
   pairs.push_back( make_pair( "enum_list_field_trigger_option_10", get_enum_string_list_field_trigger_option( 10 ) ) );

   pairs.push_back( make_pair( "enum_list_field_view_parent_extra_0", get_enum_string_list_field_view_parent_extra( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_field_view_parent_extra_1", get_enum_string_list_field_view_parent_extra( 1 ) ) );
   pairs.push_back( make_pair( "enum_list_field_view_parent_extra_2", get_enum_string_list_field_view_parent_extra( 2 ) ) );
   pairs.push_back( make_pair( "enum_list_field_view_parent_extra_3", get_enum_string_list_field_view_parent_extra( 3 ) ) );
   pairs.push_back( make_pair( "enum_list_field_view_parent_extra_4", get_enum_string_list_field_view_parent_extra( 4 ) ) );
   pairs.push_back( make_pair( "enum_list_field_view_parent_extra_5", get_enum_string_list_field_view_parent_extra( 5 ) ) );
   pairs.push_back( make_pair( "enum_list_field_view_parent_extra_6", get_enum_string_list_field_view_parent_extra( 6 ) ) );
   pairs.push_back( make_pair( "enum_list_field_view_parent_extra_7", get_enum_string_list_field_view_parent_extra( 7 ) ) );
   pairs.push_back( make_pair( "enum_list_field_view_parent_extra_8", get_enum_string_list_field_view_parent_extra( 8 ) ) );
   pairs.push_back( make_pair( "enum_list_field_view_parent_extra_9", get_enum_string_list_field_view_parent_extra( 9 ) ) );
   pairs.push_back( make_pair( "enum_list_field_view_parent_extra_10", get_enum_string_list_field_view_parent_extra( 10 ) ) );
}

void Meta_List_Field::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "List,Order", "string,string" ) );
}

void Meta_List_Field::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "List,Order" );
}

void Meta_List_Field::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_List,C_Order" );
}

void Meta_List_Field::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_List,C_Order" );
}

void Meta_List_Field::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_List_Field::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_List_Field::static_insert_external_alias( const string& module_and_class_id, Meta_List_Field* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_List_Field::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_List_Field::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   g_list_field_restrict_enum.insert( 0 );
   g_list_field_restrict_enum.insert( 1 );
   g_list_field_restrict_enum.insert( 2 );
   g_list_field_restrict_enum.insert( 3 );
   g_list_field_restrict_enum.insert( 4 );

   g_field_alignment_enum.insert( 0 );
   g_field_alignment_enum.insert( 1 );
   g_field_alignment_enum.insert( 2 );
   g_field_alignment_enum.insert( 3 );
   g_field_alignment_enum.insert( 4 );

   g_font_size_enum.insert( 0 );
   g_font_size_enum.insert( 1 );
   g_font_size_enum.insert( 2 );
   g_font_size_enum.insert( 5 );
   g_font_size_enum.insert( 6 );

   g_list_field_label_class_enum.insert( 0 );
   g_list_field_label_class_enum.insert( 1 );

   g_list_field_label_source_enum.insert( 0 );
   g_list_field_label_source_enum.insert( 1 );
   g_list_field_label_source_enum.insert( 2 );

   g_label_source_child_enum.insert( 0 );
   g_label_source_child_enum.insert( 1 );
   g_label_source_child_enum.insert( 2 );

   g_list_field_link_restrict_enum.insert( 0 );
   g_list_field_link_restrict_enum.insert( 1 );
   g_list_field_link_restrict_enum.insert( 2 );
   g_list_field_link_restrict_enum.insert( 3 );
   g_list_field_link_restrict_enum.insert( 4 );

   g_list_field_link_type_enum.insert( 0 );
   g_list_field_link_type_enum.insert( 1 );

   g_list_field_notes_truncation_enum.insert( 1 );
   g_list_field_notes_truncation_enum.insert( 0 );
   g_list_field_notes_truncation_enum.insert( 30 );
   g_list_field_notes_truncation_enum.insert( 40 );
   g_list_field_notes_truncation_enum.insert( 50 );
   g_list_field_notes_truncation_enum.insert( 80 );
   g_list_field_notes_truncation_enum.insert( 100 );

   g_orientation_enum.insert( 0 );
   g_orientation_enum.insert( 1 );
   g_orientation_enum.insert( 2 );

   g_list_field_print_type_enum.insert( 0 );
   g_list_field_print_type_enum.insert( 1 );
   g_list_field_print_type_enum.insert( 2 );
   g_list_field_print_type_enum.insert( 3 );
   g_list_field_print_type_enum.insert( 4 );
   g_list_field_print_type_enum.insert( 5 );
   g_list_field_print_type_enum.insert( 6 );

   g_list_search_opt_limit_enum.insert( 0 );
   g_list_search_opt_limit_enum.insert( 1 );
   g_list_search_opt_limit_enum.insert( 2 );
   g_list_search_opt_limit_enum.insert( 3 );
   g_list_search_opt_limit_enum.insert( 4 );
   g_list_search_opt_limit_enum.insert( 5 );
   g_list_search_opt_limit_enum.insert( 6 );
   g_list_search_opt_limit_enum.insert( 7 );

   g_list_field_switch_type_enum.insert( 0 );
   g_list_field_switch_type_enum.insert( 1 );
   g_list_field_switch_type_enum.insert( 2 );
   g_list_field_switch_type_enum.insert( 3 );

   g_list_field_trigger_option_enum.insert( 0 );
   g_list_field_trigger_option_enum.insert( 1 );
   g_list_field_trigger_option_enum.insert( 2 );
   g_list_field_trigger_option_enum.insert( 3 );
   g_list_field_trigger_option_enum.insert( 4 );
   g_list_field_trigger_option_enum.insert( 5 );
   g_list_field_trigger_option_enum.insert( 6 );
   g_list_field_trigger_option_enum.insert( 7 );
   g_list_field_trigger_option_enum.insert( 8 );
   g_list_field_trigger_option_enum.insert( 9 );
   g_list_field_trigger_option_enum.insert( 10 );

   g_list_field_view_parent_extra_enum.insert( 0 );
   g_list_field_view_parent_extra_enum.insert( 1 );
   g_list_field_view_parent_extra_enum.insert( 2 );
   g_list_field_view_parent_extra_enum.insert( 3 );
   g_list_field_view_parent_extra_enum.insert( 4 );
   g_list_field_view_parent_extra_enum.insert( 5 );
   g_list_field_view_parent_extra_enum.insert( 6 );
   g_list_field_view_parent_extra_enum.insert( 7 );
   g_list_field_view_parent_extra_enum.insert( 8 );
   g_list_field_view_parent_extra_enum.insert( 9 );
   g_list_field_view_parent_extra_enum.insert( 10 );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_List_Field::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
