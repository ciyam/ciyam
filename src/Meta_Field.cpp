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

#include "Meta_Field.h"

#include "Meta_List_Field.h"
#include "Meta_Index.h"
#include "Meta_Specification_Field_Action.h"
#include "Meta_Specification.h"
#include "Meta_Package_Option.h"
#include "Meta_Relationship.h"
#include "Meta_List.h"
#include "Meta_View_Field.h"
#include "Meta_Initial_Record_Value.h"
#include "Meta_Modifier_Affect.h"
#include "Meta_Permission.h"
#include "Meta_Class.h"
#include "Meta_Enum.h"
#include "Meta_Field.h"
#include "Meta_Type.h"

#include "numeric_helper.h"

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

// [(start modifier_field_null)] 600052a
#include "Meta_View.h"
// [(finish modifier_field_null)] 600052a

// [(start modifier_field_null)] 600052b
#include "Meta_List.h"
// [(finish modifier_field_null)] 600052b

// [(start clone_children_for_create)] 600064
#include "Meta_Class.h"
// [(finish clone_children_for_create)] 600064

// [(start clone_children_for_create)] 630065
#include "Meta_Initial_Record.h"
#include "Meta_Initial_Record_Value.h"
// [(finish clone_children_for_create)] 630065

// [<start includes>]
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

template< > inline string to_string( const Meta_Enum& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Field& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Type& c )
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

inline void from_string( Meta_Enum& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Field& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Type& c, const string& s )
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

#include "Meta_Field.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Access_Permission = "300750";
const char* const c_field_id_Access_Restriction = "107121";
const char* const c_field_id_Access_Scope = "107123";
const char* const c_field_id_Change_Permission = "300760";
const char* const c_field_id_Change_Restriction = "107122";
const char* const c_field_id_Change_Scope = "107124";
const char* const c_field_id_Class = "300700";
const char* const c_field_id_Create_List_Field = "107120";
const char* const c_field_id_Create_View_Field = "107119";
const char* const c_field_id_Def_Value = "107115";
const char* const c_field_id_Default = "107102";
const char* const c_field_id_Dummy_1 = "107118";
const char* const c_field_id_Encrypted = "107125";
const char* const c_field_id_Enum = "300720";
const char* const c_field_id_Enum_Filter = "300725";
const char* const c_field_id_Extra = "107103";
const char* const c_field_id_Extra_Filter = "107126";
const char* const c_field_id_Id = "107110";
const char* const c_field_id_Internal = "107106";
const char* const c_field_id_Is_Foreign_Key = "107108";
const char* const c_field_id_Is_Text_Type = "107109";
const char* const c_field_id_Mandatory = "107105";
const char* const c_field_id_Name = "107101";
const char* const c_field_id_Numeric_Decimals = "107114";
const char* const c_field_id_Parent_Class = "300730";
const char* const c_field_id_Parent_Class_Name = "107113";
const char* const c_field_id_Primitive = "107107";
const char* const c_field_id_Source_Field = "300740";
const char* const c_field_id_Transient = "107112";
const char* const c_field_id_Type = "300710";
const char* const c_field_id_UOM = "107104";
const char* const c_field_id_UOM_Name = "107116";
const char* const c_field_id_UOM_Symbol = "107117";
const char* const c_field_id_Use_In_Text_Search = "107111";

const char* const c_field_name_Access_Permission = "Access_Permission";
const char* const c_field_name_Access_Restriction = "Access_Restriction";
const char* const c_field_name_Access_Scope = "Access_Scope";
const char* const c_field_name_Change_Permission = "Change_Permission";
const char* const c_field_name_Change_Restriction = "Change_Restriction";
const char* const c_field_name_Change_Scope = "Change_Scope";
const char* const c_field_name_Class = "Class";
const char* const c_field_name_Create_List_Field = "Create_List_Field";
const char* const c_field_name_Create_View_Field = "Create_View_Field";
const char* const c_field_name_Def_Value = "Def_Value";
const char* const c_field_name_Default = "Default";
const char* const c_field_name_Dummy_1 = "Dummy_1";
const char* const c_field_name_Encrypted = "Encrypted";
const char* const c_field_name_Enum = "Enum";
const char* const c_field_name_Enum_Filter = "Enum_Filter";
const char* const c_field_name_Extra = "Extra";
const char* const c_field_name_Extra_Filter = "Extra_Filter";
const char* const c_field_name_Id = "Id";
const char* const c_field_name_Internal = "Internal";
const char* const c_field_name_Is_Foreign_Key = "Is_Foreign_Key";
const char* const c_field_name_Is_Text_Type = "Is_Text_Type";
const char* const c_field_name_Mandatory = "Mandatory";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Numeric_Decimals = "Numeric_Decimals";
const char* const c_field_name_Parent_Class = "Parent_Class";
const char* const c_field_name_Parent_Class_Name = "Parent_Class_Name";
const char* const c_field_name_Primitive = "Primitive";
const char* const c_field_name_Source_Field = "Source_Field";
const char* const c_field_name_Transient = "Transient";
const char* const c_field_name_Type = "Type";
const char* const c_field_name_UOM = "UOM";
const char* const c_field_name_UOM_Name = "UOM_Name";
const char* const c_field_name_UOM_Symbol = "UOM_Symbol";
const char* const c_field_name_Use_In_Text_Search = "Use_In_Text_Search";

const char* const c_field_display_name_Access_Permission = "field_field_access_permission";
const char* const c_field_display_name_Access_Restriction = "field_field_access_restriction";
const char* const c_field_display_name_Access_Scope = "field_field_access_scope";
const char* const c_field_display_name_Change_Permission = "field_field_change_permission";
const char* const c_field_display_name_Change_Restriction = "field_field_change_restriction";
const char* const c_field_display_name_Change_Scope = "field_field_change_scope";
const char* const c_field_display_name_Class = "field_field_class";
const char* const c_field_display_name_Create_List_Field = "field_field_create_list_field";
const char* const c_field_display_name_Create_View_Field = "field_field_create_view_field";
const char* const c_field_display_name_Def_Value = "field_field_def_value";
const char* const c_field_display_name_Default = "field_field_default";
const char* const c_field_display_name_Dummy_1 = "field_field_dummy_1";
const char* const c_field_display_name_Encrypted = "field_field_encrypted";
const char* const c_field_display_name_Enum = "field_field_enum";
const char* const c_field_display_name_Enum_Filter = "field_field_enum_filter";
const char* const c_field_display_name_Extra = "field_field_extra";
const char* const c_field_display_name_Extra_Filter = "field_field_extra_filter";
const char* const c_field_display_name_Id = "field_field_id";
const char* const c_field_display_name_Internal = "field_field_internal";
const char* const c_field_display_name_Is_Foreign_Key = "field_field_is_foreign_key";
const char* const c_field_display_name_Is_Text_Type = "field_field_is_text_type";
const char* const c_field_display_name_Mandatory = "field_field_mandatory";
const char* const c_field_display_name_Name = "field_field_name";
const char* const c_field_display_name_Numeric_Decimals = "field_field_numeric_decimals";
const char* const c_field_display_name_Parent_Class = "field_field_parent_class";
const char* const c_field_display_name_Parent_Class_Name = "field_field_parent_class_name";
const char* const c_field_display_name_Primitive = "field_field_primitive";
const char* const c_field_display_name_Source_Field = "field_field_source_field";
const char* const c_field_display_name_Transient = "field_field_transient";
const char* const c_field_display_name_Type = "field_field_type";
const char* const c_field_display_name_UOM = "field_field_uom";
const char* const c_field_display_name_UOM_Name = "field_field_uom_name";
const char* const c_field_display_name_UOM_Symbol = "field_field_uom_symbol";
const char* const c_field_display_name_Use_In_Text_Search = "field_field_use_in_text_search";

const int c_num_fields = 34;

const char* const c_all_sorted_field_ids[ ] =
{
   "107101",
   "107102",
   "107103",
   "107104",
   "107105",
   "107106",
   "107107",
   "107108",
   "107109",
   "107110",
   "107111",
   "107112",
   "107113",
   "107114",
   "107115",
   "107116",
   "107117",
   "107118",
   "107119",
   "107120",
   "107121",
   "107122",
   "107123",
   "107124",
   "107125",
   "107126",
   "300700",
   "300710",
   "300720",
   "300725",
   "300730",
   "300740",
   "300750",
   "300760"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Access_Permission",
   "Access_Restriction",
   "Access_Scope",
   "Change_Permission",
   "Change_Restriction",
   "Change_Scope",
   "Class",
   "Create_List_Field",
   "Create_View_Field",
   "Def_Value",
   "Default",
   "Dummy_1",
   "Encrypted",
   "Enum",
   "Enum_Filter",
   "Extra",
   "Extra_Filter",
   "Id",
   "Internal",
   "Is_Foreign_Key",
   "Is_Text_Type",
   "Mandatory",
   "Name",
   "Numeric_Decimals",
   "Parent_Class",
   "Parent_Class_Name",
   "Primitive",
   "Source_Field",
   "Transient",
   "Type",
   "UOM",
   "UOM_Name",
   "UOM_Symbol",
   "Use_In_Text_Search"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool has_field( const string& field )
{
   return binary_search( c_all_sorted_field_ids, c_all_sorted_field_ids + c_num_fields, field.c_str( ), compare )
    || binary_search( c_all_sorted_field_names, c_all_sorted_field_names + c_num_fields, field.c_str( ), compare );
}

const int c_num_encrypted_fields = 0;

bool is_encrypted_field( const string& ) { static bool false_value( false ); return false_value; }

const int c_num_transient_fields = 7;

const char* const c_transient_sorted_field_ids[ ] =
{
   "107113",
   "107114",
   "107115",
   "107118",
   "107119",
   "107120",
   "107126"
};

const char* const c_transient_sorted_field_names[ ] =
{
   "Create_List_Field",
   "Create_View_Field",
   "Def_Value",
   "Dummy_1",
   "Extra_Filter",
   "Numeric_Decimals",
   "Parent_Class_Name"
};

inline bool is_transient_field( const string& field )
{
   return binary_search( c_transient_sorted_field_ids,
    c_transient_sorted_field_ids + c_num_transient_fields, field.c_str( ), compare )
    || binary_search( c_transient_sorted_field_names,
    c_transient_sorted_field_names + c_num_transient_fields, field.c_str( ), compare );
}

const char* const c_procedure_id_Get_Text_Type = "107410";

const uint64_t c_modifier_Has_Enum = UINT64_C( 0x100 );
const uint64_t c_modifier_Hide_Create_List_Field = UINT64_C( 0x200 );
const uint64_t c_modifier_Hide_Create_View_Field = UINT64_C( 0x400 );
const uint64_t c_modifier_Is_Any_Non_Text_Type = UINT64_C( 0x800 );
const uint64_t c_modifier_Is_Encrypted = UINT64_C( 0x1000 );
const uint64_t c_modifier_Is_Internal = UINT64_C( 0x2000 );
const uint64_t c_modifier_Is_Non_Custom_UOM = UINT64_C( 0x4000 );
const uint64_t c_modifier_Is_Not_Type_string = UINT64_C( 0x8000 );
const uint64_t c_modifier_Is_Transient = UINT64_C( 0x10000 );
const uint64_t c_modifier_Is_Type_bool = UINT64_C( 0x20000 );
const uint64_t c_modifier_Is_Type_date = UINT64_C( 0x40000 );
const uint64_t c_modifier_Is_Type_datetime = UINT64_C( 0x80000 );
const uint64_t c_modifier_Is_Type_string = UINT64_C( 0x100000 );
const uint64_t c_modifier_Is_Type_time = UINT64_C( 0x200000 );

domain_string_max_size< 200 > g_Default_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Id_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 50 > > g_Name_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Parent_Class_Name_domain;
domain_string_max_size< 100 > g_UOM_Name_domain;
domain_string_max_size< 10 > g_UOM_Symbol_domain;

string g_group_field_name;
string g_level_field_name;
string g_order_field_name;
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_Field* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Field* > external_aliases_lookup_container;
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
string g_default_Change_Permission = string( );
int g_default_Change_Restriction = int( 0 );
int g_default_Change_Scope = int( 0 );
string g_default_Class = string( );
bool g_default_Create_List_Field = bool( 0 );
bool g_default_Create_View_Field = bool( 0 );
string g_default_Def_Value = string( );
string g_default_Default = string( );
bool g_default_Dummy_1 = bool( 1 );
bool g_default_Encrypted = bool( 0 );
string g_default_Enum = string( );
string g_default_Enum_Filter = string( );
int g_default_Extra = int( 0 );
string g_default_Extra_Filter = string( );
string g_default_Id = string( );
bool g_default_Internal = bool( 0 );
bool g_default_Is_Foreign_Key = bool( 0 );
bool g_default_Is_Text_Type = bool( 1 );
bool g_default_Mandatory = bool( 1 );
string g_default_Name = string( );
numeric g_default_Numeric_Decimals = numeric( 0 );
string g_default_Parent_Class = string( );
string g_default_Parent_Class_Name = string( );
int g_default_Primitive = int( 0 );
string g_default_Source_Field = string( );
bool g_default_Transient = bool( 0 );
string g_default_Type = string( );
int g_default_UOM = int( 0 );
string g_default_UOM_Name = string( );
string g_default_UOM_Symbol = string( );
bool g_default_Use_In_Text_Search = bool( 0 );

set< int > g_view_access_restrict_enum;
set< int > g_view_field_access_scope_enum;
set< int > g_view_change_restrict_enum;
set< int > g_view_field_change_scope_enum;
set< int > g_field_extra_enum;
set< int > g_primitive_enum;
set< int > g_uom_enum;

const int c_enum_view_access_restrict_none( 0 );
const int c_enum_view_access_restrict_owner_only( 1 );
const int c_enum_view_access_restrict_admin_only( 2 );
const int c_enum_view_access_restrict_admin_owner( 3 );
const int c_enum_view_access_restrict_denied_always( 4 );

string get_enum_string_view_access_restrict( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_access_restrict" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_access_restrict_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_view_access_restrict_owner_only";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_access_restrict_admin_only";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_view_access_restrict_admin_owner";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_view_access_restrict_denied_always";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_access_restrict" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_view_field_access_scope_all( 0 );
const int c_enum_view_field_access_scope_create_only( 1 );
const int c_enum_view_field_access_scope_post_create( 2 );
const int c_enum_view_field_access_scope_editing_only( 3 );
const int c_enum_view_field_access_scope_viewing_only( 4 );
const int c_enum_view_field_access_scope_updating_only( 5 );
const int c_enum_view_field_access_scope_viewing_non_null( 6 );
const int c_enum_view_field_access_scope_always_if_non_null( 7 );

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
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_view_field_access_scope_viewing_non_null";
   else if( to_string( val ) == to_string( "7" ) )
      string_name = "enum_view_field_access_scope_always_if_non_null";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_field_access_scope" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_view_change_restrict_none( 0 );
const int c_enum_view_change_restrict_owner_only( 1 );
const int c_enum_view_change_restrict_admin_only( 2 );
const int c_enum_view_change_restrict_admin_owner( 3 );
const int c_enum_view_change_restrict_denied_always( 4 );

string get_enum_string_view_change_restrict( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_change_restrict" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_change_restrict_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_view_change_restrict_owner_only";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_change_restrict_admin_only";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_view_change_restrict_admin_owner";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_view_change_restrict_denied_always";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_change_restrict" );

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

const int c_enum_field_extra_none( 0 );
const int c_enum_field_extra_file( 1 );
const int c_enum_field_extra_href( 2 );
const int c_enum_field_extra_html( 9 );
const int c_enum_field_extra_text( 23 );
const int c_enum_field_extra_notes( 4 );
const int c_enum_field_extra_order( 5 );
const int c_enum_field_extra_active( 6 );
const int c_enum_field_extra_image( 3 );
const int c_enum_field_extra_mailto( 7 );
const int c_enum_field_extra_file_link( 21 );
const int c_enum_field_extra_actions( 8 );
const int c_enum_field_extra_special( 31 );
const int c_enum_field_extra_qr_code( 29 );
const int c_enum_field_extra_qr_scan( 35 );
const int c_enum_field_extra_filename( 22 );
const int c_enum_field_extra_field_hash( 25 );
const int c_enum_field_extra_password( 10 );
const int c_enum_field_extra_key_name( 34 );
const int c_enum_field_extra_key_suffix( 37 );
const int c_enum_field_extra_defcurrent( 11 );
const int c_enum_field_extra_permission( 17 );
const int c_enum_field_extra_orientation( 24 );
const int c_enum_field_extra_hpassword( 26 );
const int c_enum_field_extra_manual_link( 13 );
const int c_enum_field_extra_user_perms( 12 );
const int c_enum_field_extra_qr_scan_key( 36 );
const int c_enum_field_extra_current_year( 14 );
const int c_enum_field_extra_security_level( 18 );
const int c_enum_field_extra_prefix_special( 32 );
const int c_enum_field_extra_suffix_special( 33 );
const int c_enum_field_extra_key_from_hash( 38 );
const int c_enum_field_extra_hpassword_salt( 27 );
const int c_enum_field_extra_create_datetime( 15 );
const int c_enum_field_extra_modify_datetime( 16 );
const int c_enum_field_extra_ignore_encrypted( 30 );
const int c_enum_field_extra_is_effective_owner( 28 );
const int c_enum_field_extra_replace_underbars( 19 );
const int c_enum_field_extra_enforce_uppercase( 20 );
const int c_enum_field_extra_user_group( -2 );
const int c_enum_field_extra_user_other( -3 );
const int c_enum_field_extra_owning_user( -1 );
const int c_enum_field_extra_create_user_key( -4 );
const int c_enum_field_extra_modify_user_key( -5 );

string get_enum_string_field_extra( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for field_extra" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_field_extra_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_field_extra_file";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_field_extra_href";
   else if( to_string( val ) == to_string( "9" ) )
      string_name = "enum_field_extra_html";
   else if( to_string( val ) == to_string( "23" ) )
      string_name = "enum_field_extra_text";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_field_extra_notes";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_field_extra_order";
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_field_extra_active";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_field_extra_image";
   else if( to_string( val ) == to_string( "7" ) )
      string_name = "enum_field_extra_mailto";
   else if( to_string( val ) == to_string( "21" ) )
      string_name = "enum_field_extra_file_link";
   else if( to_string( val ) == to_string( "8" ) )
      string_name = "enum_field_extra_actions";
   else if( to_string( val ) == to_string( "31" ) )
      string_name = "enum_field_extra_special";
   else if( to_string( val ) == to_string( "29" ) )
      string_name = "enum_field_extra_qr_code";
   else if( to_string( val ) == to_string( "35" ) )
      string_name = "enum_field_extra_qr_scan";
   else if( to_string( val ) == to_string( "22" ) )
      string_name = "enum_field_extra_filename";
   else if( to_string( val ) == to_string( "25" ) )
      string_name = "enum_field_extra_field_hash";
   else if( to_string( val ) == to_string( "10" ) )
      string_name = "enum_field_extra_password";
   else if( to_string( val ) == to_string( "34" ) )
      string_name = "enum_field_extra_key_name";
   else if( to_string( val ) == to_string( "37" ) )
      string_name = "enum_field_extra_key_suffix";
   else if( to_string( val ) == to_string( "11" ) )
      string_name = "enum_field_extra_defcurrent";
   else if( to_string( val ) == to_string( "17" ) )
      string_name = "enum_field_extra_permission";
   else if( to_string( val ) == to_string( "24" ) )
      string_name = "enum_field_extra_orientation";
   else if( to_string( val ) == to_string( "26" ) )
      string_name = "enum_field_extra_hpassword";
   else if( to_string( val ) == to_string( "13" ) )
      string_name = "enum_field_extra_manual_link";
   else if( to_string( val ) == to_string( "12" ) )
      string_name = "enum_field_extra_user_perms";
   else if( to_string( val ) == to_string( "36" ) )
      string_name = "enum_field_extra_qr_scan_key";
   else if( to_string( val ) == to_string( "14" ) )
      string_name = "enum_field_extra_current_year";
   else if( to_string( val ) == to_string( "18" ) )
      string_name = "enum_field_extra_security_level";
   else if( to_string( val ) == to_string( "32" ) )
      string_name = "enum_field_extra_prefix_special";
   else if( to_string( val ) == to_string( "33" ) )
      string_name = "enum_field_extra_suffix_special";
   else if( to_string( val ) == to_string( "38" ) )
      string_name = "enum_field_extra_key_from_hash";
   else if( to_string( val ) == to_string( "27" ) )
      string_name = "enum_field_extra_hpassword_salt";
   else if( to_string( val ) == to_string( "15" ) )
      string_name = "enum_field_extra_create_datetime";
   else if( to_string( val ) == to_string( "16" ) )
      string_name = "enum_field_extra_modify_datetime";
   else if( to_string( val ) == to_string( "30" ) )
      string_name = "enum_field_extra_ignore_encrypted";
   else if( to_string( val ) == to_string( "28" ) )
      string_name = "enum_field_extra_is_effective_owner";
   else if( to_string( val ) == to_string( "19" ) )
      string_name = "enum_field_extra_replace_underbars";
   else if( to_string( val ) == to_string( "20" ) )
      string_name = "enum_field_extra_enforce_uppercase";
   else if( to_string( val ) == to_string( "-2" ) )
      string_name = "enum_field_extra_user_group";
   else if( to_string( val ) == to_string( "-3" ) )
      string_name = "enum_field_extra_user_other";
   else if( to_string( val ) == to_string( "-1" ) )
      string_name = "enum_field_extra_owning_user";
   else if( to_string( val ) == to_string( "-4" ) )
      string_name = "enum_field_extra_create_user_key";
   else if( to_string( val ) == to_string( "-5" ) )
      string_name = "enum_field_extra_modify_user_key";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for field_extra" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_primitive_string( 0 );
const int c_enum_primitive_datetime( 1 );
const int c_enum_primitive_date( 2 );
const int c_enum_primitive_time( 3 );
const int c_enum_primitive_numeric( 4 );
const int c_enum_primitive_int( 5 );
const int c_enum_primitive_bool( 6 );

string get_enum_string_primitive( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for primitive" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_primitive_string";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_primitive_datetime";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_primitive_date";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_primitive_time";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_primitive_numeric";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_primitive_int";
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_primitive_bool";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for primitive" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_uom_none( 0 );
const int c_enum_uom_sqm( 1 );
const int c_enum_uom_km( 2 );
const int c_enum_uom_m( 3 );
const int c_enum_uom_cm( 4 );
const int c_enum_uom_mm( 5 );
const int c_enum_uom_kg( 6 );
const int c_enum_uom_g( 7 );
const int c_enum_uom_mg( 8 );
const int c_enum_uom_number( 900 );
const int c_enum_uom_dollars( 901 );
const int c_enum_uom_percent( 902 );
const int c_enum_uom_customised( 999 );

string get_enum_string_uom( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for uom" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_uom_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_uom_sqm";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_uom_km";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_uom_m";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_uom_cm";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_uom_mm";
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_uom_kg";
   else if( to_string( val ) == to_string( "7" ) )
      string_name = "enum_uom_g";
   else if( to_string( val ) == to_string( "8" ) )
      string_name = "enum_uom_mg";
   else if( to_string( val ) == to_string( "900" ) )
      string_name = "enum_uom_number";
   else if( to_string( val ) == to_string( "901" ) )
      string_name = "enum_uom_dollars";
   else if( to_string( val ) == to_string( "902" ) )
      string_name = "enum_uom_percent";
   else if( to_string( val ) == to_string( "999" ) )
      string_name = "enum_uom_customised";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for uom" );

   return get_module_string( lower( string_name ) );
}

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Field > Field_registration( get_class_registry( ), "107100" );

class Meta_Field_command_functor;

class Meta_Field_command_handler : public command_handler
{
   friend class Meta_Field_command_functor;

   public:
   Meta_Field_command_handler( ) : p_Meta_Field( 0 ) { }

   void set_Meta_Field( Meta_Field* p_new_Meta_Field ) { p_Meta_Field = p_new_Meta_Field; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Field* p_Meta_Field;

   protected:
   string retval;
};

class Meta_Field_command_functor : public command_functor
{
   public:
   Meta_Field_command_functor( Meta_Field_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Field_command_handler& cmd_handler;
};

command_functor* Meta_Field_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Field_command_functor( dynamic_cast< Meta_Field_command_handler& >( handler ) );
}

void Meta_Field_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Field_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_Field_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Field->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Field->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Field_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Field->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Field->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Field_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Field_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_Field->Access_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Restriction ) || ( field_name == c_field_name_Access_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Field->Access_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Scope ) || ( field_name == c_field_name_Access_Scope ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Field->Access_Scope( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Permission ) || ( field_name == c_field_name_Change_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_Field->Change_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Restriction ) || ( field_name == c_field_name_Change_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Field->Change_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Scope ) || ( field_name == c_field_name_Change_Scope ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Field->Change_Scope( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Field->Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Create_List_Field ) || ( field_name == c_field_name_Create_List_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Field->Create_List_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Create_View_Field ) || ( field_name == c_field_name_Create_View_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Field->Create_View_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Def_Value ) || ( field_name == c_field_name_Def_Value ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Field->Def_Value( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Default ) || ( field_name == c_field_name_Default ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Field->Default( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Dummy_1 ) || ( field_name == c_field_name_Dummy_1 ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Field->Dummy_1( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Encrypted ) || ( field_name == c_field_name_Encrypted ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Field->Encrypted( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Enum ) || ( field_name == c_field_name_Enum ) ) )
      {
         handled = true;

         string_getter< Meta_Enum >( cmd_handler.p_Meta_Field->Enum( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Enum_Filter ) || ( field_name == c_field_name_Enum_Filter ) ) )
      {
         handled = true;

         string_getter< Meta_Enum >( cmd_handler.p_Meta_Field->Enum_Filter( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Extra ) || ( field_name == c_field_name_Extra ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Field->Extra( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Extra_Filter ) || ( field_name == c_field_name_Extra_Filter ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Field->Extra_Filter( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Id ) || ( field_name == c_field_name_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Field->Id( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Internal ) || ( field_name == c_field_name_Internal ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Field->Internal( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Foreign_Key ) || ( field_name == c_field_name_Is_Foreign_Key ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Field->Is_Foreign_Key( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Text_Type ) || ( field_name == c_field_name_Is_Text_Type ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Field->Is_Text_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Mandatory ) || ( field_name == c_field_name_Mandatory ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Field->Mandatory( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Field->Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Numeric_Decimals ) || ( field_name == c_field_name_Numeric_Decimals ) ) )
      {
         handled = true;

         string_getter< numeric >( cmd_handler.p_Meta_Field->Numeric_Decimals( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Class ) || ( field_name == c_field_name_Parent_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Field->Parent_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Class_Name ) || ( field_name == c_field_name_Parent_Class_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Field->Parent_Class_Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Primitive ) || ( field_name == c_field_name_Primitive ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Field->Primitive( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Field ) || ( field_name == c_field_name_Source_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Field->Source_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Transient ) || ( field_name == c_field_name_Transient ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Field->Transient( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         string_getter< Meta_Type >( cmd_handler.p_Meta_Field->Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_UOM ) || ( field_name == c_field_name_UOM ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Field->UOM( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_UOM_Name ) || ( field_name == c_field_name_UOM_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Field->UOM_Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_UOM_Symbol ) || ( field_name == c_field_name_UOM_Symbol ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Field->UOM_Symbol( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_In_Text_Search ) || ( field_name == c_field_name_Use_In_Text_Search ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Field->Use_In_Text_Search( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Field_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Field_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_Field_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, Meta_Permission >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Access_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Restriction ) || ( field_name == c_field_name_Access_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, int >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Access_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Scope ) || ( field_name == c_field_name_Access_Scope ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, int >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Access_Scope, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Permission ) || ( field_name == c_field_name_Change_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, Meta_Permission >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Change_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Restriction ) || ( field_name == c_field_name_Change_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, int >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Change_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Scope ) || ( field_name == c_field_name_Change_Scope ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, int >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Change_Scope, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, Meta_Class >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Create_List_Field ) || ( field_name == c_field_name_Create_List_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, bool >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Create_List_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Create_View_Field ) || ( field_name == c_field_name_Create_View_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, bool >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Create_View_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Def_Value ) || ( field_name == c_field_name_Def_Value ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, string >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Def_Value, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Default ) || ( field_name == c_field_name_Default ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, string >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Default, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Dummy_1 ) || ( field_name == c_field_name_Dummy_1 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, bool >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Dummy_1, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Encrypted ) || ( field_name == c_field_name_Encrypted ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, bool >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Encrypted, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Enum ) || ( field_name == c_field_name_Enum ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, Meta_Enum >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Enum, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Enum_Filter ) || ( field_name == c_field_name_Enum_Filter ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, Meta_Enum >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Enum_Filter, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Extra ) || ( field_name == c_field_name_Extra ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, int >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Extra, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Extra_Filter ) || ( field_name == c_field_name_Extra_Filter ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, string >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Extra_Filter, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Id ) || ( field_name == c_field_name_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, string >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Id, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Internal ) || ( field_name == c_field_name_Internal ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, bool >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Internal, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Foreign_Key ) || ( field_name == c_field_name_Is_Foreign_Key ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, bool >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Is_Foreign_Key, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Text_Type ) || ( field_name == c_field_name_Is_Text_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, bool >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Is_Text_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Mandatory ) || ( field_name == c_field_name_Mandatory ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, bool >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Mandatory, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, string >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Numeric_Decimals ) || ( field_name == c_field_name_Numeric_Decimals ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, numeric >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Numeric_Decimals, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Class ) || ( field_name == c_field_name_Parent_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, Meta_Class >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Parent_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Class_Name ) || ( field_name == c_field_name_Parent_Class_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, string >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Parent_Class_Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Primitive ) || ( field_name == c_field_name_Primitive ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, int >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Primitive, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Field ) || ( field_name == c_field_name_Source_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, Meta_Field >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Source_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Transient ) || ( field_name == c_field_name_Transient ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, bool >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Transient, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, Meta_Type >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_UOM ) || ( field_name == c_field_name_UOM ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, int >(
          *cmd_handler.p_Meta_Field, &Meta_Field::UOM, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_UOM_Name ) || ( field_name == c_field_name_UOM_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, string >(
          *cmd_handler.p_Meta_Field, &Meta_Field::UOM_Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_UOM_Symbol ) || ( field_name == c_field_name_UOM_Symbol ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, string >(
          *cmd_handler.p_Meta_Field, &Meta_Field::UOM_Symbol, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_In_Text_Search ) || ( field_name == c_field_name_Use_In_Text_Search ) ) )
      {
         handled = true;

         func_string_setter< Meta_Field, bool >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Use_In_Text_Search, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Field_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Field_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_Field_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_Numeric_Decimals ) || ( field_name == c_field_name_Numeric_Decimals ) )
      {
         numeric Numeric_Decimals( cmd_handler.p_Meta_Field->Numeric_Decimals( ) );

         execute_command( Numeric_Decimals, cmd_and_args, cmd_handler.retval );

         cmd_handler.p_Meta_Field->Numeric_Decimals( Numeric_Decimals );
      }
      else if( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_Field->Access_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Change_Permission ) || ( field_name == c_field_name_Change_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_Field->Change_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Field->Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Enum ) || ( field_name == c_field_name_Enum ) )
         cmd_handler.retval = cmd_handler.p_Meta_Field->Enum( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Enum_Filter ) || ( field_name == c_field_name_Enum_Filter ) )
         cmd_handler.retval = cmd_handler.p_Meta_Field->Enum_Filter( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Parent_Class ) || ( field_name == c_field_name_Parent_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Field->Parent_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Field ) || ( field_name == c_field_name_Source_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_Field->Source_Field( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) )
         cmd_handler.retval = cmd_handler.p_Meta_Field->Type( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
   else if( command == c_cmd_Meta_Field_Get_Text_Type )
   {
      int Result;

      cmd_handler.p_Meta_Field->Get_Text_Type( Result );

      cmd_handler.retval.erase( );

      append_value( cmd_handler.retval, Result );
   }
}

struct Meta_Field::impl : public Meta_Field_command_handler
{
   impl( Meta_Field& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Field( &o );

      add_commands( 0, Meta_Field_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Field_command_definitions ) );
   }

   Meta_Field& get_obj( ) const
   {
      return *cp_obj;
   }

   int impl_Access_Restriction( ) const { return lazy_fetch( p_obj ), v_Access_Restriction; }
   void impl_Access_Restriction( int Access_Restriction ) { v_Access_Restriction = Access_Restriction; }

   int impl_Access_Scope( ) const { return lazy_fetch( p_obj ), v_Access_Scope; }
   void impl_Access_Scope( int Access_Scope ) { v_Access_Scope = Access_Scope; }

   int impl_Change_Restriction( ) const { return lazy_fetch( p_obj ), v_Change_Restriction; }
   void impl_Change_Restriction( int Change_Restriction ) { v_Change_Restriction = Change_Restriction; }

   int impl_Change_Scope( ) const { return lazy_fetch( p_obj ), v_Change_Scope; }
   void impl_Change_Scope( int Change_Scope ) { v_Change_Scope = Change_Scope; }

   bool impl_Create_List_Field( ) const { return lazy_fetch( p_obj ), v_Create_List_Field; }
   void impl_Create_List_Field( bool Create_List_Field ) { v_Create_List_Field = Create_List_Field; }

   bool impl_Create_View_Field( ) const { return lazy_fetch( p_obj ), v_Create_View_Field; }
   void impl_Create_View_Field( bool Create_View_Field ) { v_Create_View_Field = Create_View_Field; }

   const string& impl_Def_Value( ) const { return lazy_fetch( p_obj ), v_Def_Value; }
   void impl_Def_Value( const string& Def_Value ) { sanity_check( Def_Value ); v_Def_Value = Def_Value; }

   const string& impl_Default( ) const { return lazy_fetch( p_obj ), v_Default; }
   void impl_Default( const string& Default ) { sanity_check( Default ); v_Default = Default; }

   bool impl_Dummy_1( ) const { return lazy_fetch( p_obj ), v_Dummy_1; }
   void impl_Dummy_1( bool Dummy_1 ) { v_Dummy_1 = Dummy_1; }

   bool impl_Encrypted( ) const { return lazy_fetch( p_obj ), v_Encrypted; }
   void impl_Encrypted( bool Encrypted ) { v_Encrypted = Encrypted; }

   int impl_Extra( ) const { return lazy_fetch( p_obj ), v_Extra; }
   void impl_Extra( int Extra ) { v_Extra = Extra; }

   const string& impl_Extra_Filter( ) const { return lazy_fetch( p_obj ), v_Extra_Filter; }
   void impl_Extra_Filter( const string& Extra_Filter ) { sanity_check( Extra_Filter ); v_Extra_Filter = Extra_Filter; }

   const string& impl_Id( ) const { return lazy_fetch( p_obj ), v_Id; }
   void impl_Id( const string& Id ) { sanity_check( Id ); v_Id = Id; }

   bool impl_Internal( ) const { return lazy_fetch( p_obj ), v_Internal; }
   void impl_Internal( bool Internal ) { v_Internal = Internal; }

   bool impl_Is_Foreign_Key( ) const { return lazy_fetch( p_obj ), v_Is_Foreign_Key; }
   void impl_Is_Foreign_Key( bool Is_Foreign_Key ) { v_Is_Foreign_Key = Is_Foreign_Key; }

   bool impl_Is_Text_Type( ) const { return lazy_fetch( p_obj ), v_Is_Text_Type; }
   void impl_Is_Text_Type( bool Is_Text_Type ) { v_Is_Text_Type = Is_Text_Type; }

   bool impl_Mandatory( ) const { return lazy_fetch( p_obj ), v_Mandatory; }
   void impl_Mandatory( bool Mandatory ) { v_Mandatory = Mandatory; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { sanity_check( Name ); v_Name = Name; }

   const numeric& impl_Numeric_Decimals( ) const { return lazy_fetch( p_obj ), v_Numeric_Decimals; }
   void impl_Numeric_Decimals( const numeric& Numeric_Decimals ) { sanity_check( Numeric_Decimals ); v_Numeric_Decimals = Numeric_Decimals; }

   const string& impl_Parent_Class_Name( ) const { return lazy_fetch( p_obj ), v_Parent_Class_Name; }
   void impl_Parent_Class_Name( const string& Parent_Class_Name ) { sanity_check( Parent_Class_Name ); v_Parent_Class_Name = Parent_Class_Name; }

   int impl_Primitive( ) const { return lazy_fetch( p_obj ), v_Primitive; }
   void impl_Primitive( int Primitive ) { v_Primitive = Primitive; }

   bool impl_Transient( ) const { return lazy_fetch( p_obj ), v_Transient; }
   void impl_Transient( bool Transient ) { v_Transient = Transient; }

   int impl_UOM( ) const { return lazy_fetch( p_obj ), v_UOM; }
   void impl_UOM( int UOM ) { v_UOM = UOM; }

   const string& impl_UOM_Name( ) const { return lazy_fetch( p_obj ), v_UOM_Name; }
   void impl_UOM_Name( const string& UOM_Name ) { sanity_check( UOM_Name ); v_UOM_Name = UOM_Name; }

   const string& impl_UOM_Symbol( ) const { return lazy_fetch( p_obj ), v_UOM_Symbol; }
   void impl_UOM_Symbol( const string& UOM_Symbol ) { sanity_check( UOM_Symbol ); v_UOM_Symbol = UOM_Symbol; }

   bool impl_Use_In_Text_Search( ) const { return lazy_fetch( p_obj ), v_Use_In_Text_Search; }
   void impl_Use_In_Text_Search( bool Use_In_Text_Search ) { v_Use_In_Text_Search = Use_In_Text_Search; }

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

   Meta_Enum& impl_Enum( )
   {
      if( !cp_Enum )
      {
         cp_Enum.init( );

         p_obj->setup_graph_parent( *cp_Enum, c_field_id_Enum, v_Enum );
      }
      return *cp_Enum;
   }

   const Meta_Enum& impl_Enum( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Enum )
      {
         cp_Enum.init( );

         p_obj->setup_graph_parent( *cp_Enum, c_field_id_Enum, v_Enum );
      }
      return *cp_Enum;
   }

   void impl_Enum( const string& key )
   {
      class_base_accessor cba( impl_Enum( ) );
      cba.set_key( key );
   }

   Meta_Enum& impl_Enum_Filter( )
   {
      if( !cp_Enum_Filter )
      {
         cp_Enum_Filter.init( );

         p_obj->setup_graph_parent( *cp_Enum_Filter, c_field_id_Enum_Filter, v_Enum_Filter );
      }
      return *cp_Enum_Filter;
   }

   const Meta_Enum& impl_Enum_Filter( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Enum_Filter )
      {
         cp_Enum_Filter.init( );

         p_obj->setup_graph_parent( *cp_Enum_Filter, c_field_id_Enum_Filter, v_Enum_Filter );
      }
      return *cp_Enum_Filter;
   }

   void impl_Enum_Filter( const string& key )
   {
      class_base_accessor cba( impl_Enum_Filter( ) );
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

   Meta_Type& impl_Type( )
   {
      if( !cp_Type )
      {
         cp_Type.init( );

         p_obj->setup_graph_parent( *cp_Type, c_field_id_Type, v_Type );
      }
      return *cp_Type;
   }

   const Meta_Type& impl_Type( ) const
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

   Meta_List_Field& impl_child_List_Field_Child_Rel_Source_Child( )
   {
      if( !cp_child_List_Field_Child_Rel_Source_Child )
      {
         cp_child_List_Field_Child_Rel_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Child_Rel_Source_Child, "302198" );
      }
      return *cp_child_List_Field_Child_Rel_Source_Child;
   }

   const Meta_List_Field& impl_child_List_Field_Child_Rel_Source_Child( ) const
   {
      if( !cp_child_List_Field_Child_Rel_Source_Child )
      {
         cp_child_List_Field_Child_Rel_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Child_Rel_Source_Child, "302198" );
      }
      return *cp_child_List_Field_Child_Rel_Source_Child;
   }

   Meta_List_Field& impl_child_List_Field_Child_Rel_Source( )
   {
      if( !cp_child_List_Field_Child_Rel_Source )
      {
         cp_child_List_Field_Child_Rel_Source.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Child_Rel_Source, "302195" );
      }
      return *cp_child_List_Field_Child_Rel_Source;
   }

   const Meta_List_Field& impl_child_List_Field_Child_Rel_Source( ) const
   {
      if( !cp_child_List_Field_Child_Rel_Source )
      {
         cp_child_List_Field_Child_Rel_Source.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Child_Rel_Source, "302195" );
      }
      return *cp_child_List_Field_Child_Rel_Source;
   }

   Meta_List_Field& impl_child_List_Field_Child_Rel_Source_Parent( )
   {
      if( !cp_child_List_Field_Child_Rel_Source_Parent )
      {
         cp_child_List_Field_Child_Rel_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Child_Rel_Source_Parent, "302196" );
      }
      return *cp_child_List_Field_Child_Rel_Source_Parent;
   }

   const Meta_List_Field& impl_child_List_Field_Child_Rel_Source_Parent( ) const
   {
      if( !cp_child_List_Field_Child_Rel_Source_Parent )
      {
         cp_child_List_Field_Child_Rel_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Child_Rel_Source_Parent, "302196" );
      }
      return *cp_child_List_Field_Child_Rel_Source_Parent;
   }

   Meta_Index& impl_child_Index_Field_1( )
   {
      if( !cp_child_Index_Field_1 )
      {
         cp_child_Index_Field_1.init( );

         p_obj->setup_graph_parent( *cp_child_Index_Field_1, "300811" );
      }
      return *cp_child_Index_Field_1;
   }

   const Meta_Index& impl_child_Index_Field_1( ) const
   {
      if( !cp_child_Index_Field_1 )
      {
         cp_child_Index_Field_1.init( );

         p_obj->setup_graph_parent( *cp_child_Index_Field_1, "300811" );
      }
      return *cp_child_Index_Field_1;
   }

   Meta_Index& impl_child_Index_Field_2( )
   {
      if( !cp_child_Index_Field_2 )
      {
         cp_child_Index_Field_2.init( );

         p_obj->setup_graph_parent( *cp_child_Index_Field_2, "300812" );
      }
      return *cp_child_Index_Field_2;
   }

   const Meta_Index& impl_child_Index_Field_2( ) const
   {
      if( !cp_child_Index_Field_2 )
      {
         cp_child_Index_Field_2.init( );

         p_obj->setup_graph_parent( *cp_child_Index_Field_2, "300812" );
      }
      return *cp_child_Index_Field_2;
   }

   Meta_Index& impl_child_Index_Field_3( )
   {
      if( !cp_child_Index_Field_3 )
      {
         cp_child_Index_Field_3.init( );

         p_obj->setup_graph_parent( *cp_child_Index_Field_3, "300813" );
      }
      return *cp_child_Index_Field_3;
   }

   const Meta_Index& impl_child_Index_Field_3( ) const
   {
      if( !cp_child_Index_Field_3 )
      {
         cp_child_Index_Field_3.init( );

         p_obj->setup_graph_parent( *cp_child_Index_Field_3, "300813" );
      }
      return *cp_child_Index_Field_3;
   }

   Meta_Index& impl_child_Index_Field_4( )
   {
      if( !cp_child_Index_Field_4 )
      {
         cp_child_Index_Field_4.init( );

         p_obj->setup_graph_parent( *cp_child_Index_Field_4, "300814" );
      }
      return *cp_child_Index_Field_4;
   }

   const Meta_Index& impl_child_Index_Field_4( ) const
   {
      if( !cp_child_Index_Field_4 )
      {
         cp_child_Index_Field_4.init( );

         p_obj->setup_graph_parent( *cp_child_Index_Field_4, "300814" );
      }
      return *cp_child_Index_Field_4;
   }

   Meta_Index& impl_child_Index_Field_5( )
   {
      if( !cp_child_Index_Field_5 )
      {
         cp_child_Index_Field_5.init( );

         p_obj->setup_graph_parent( *cp_child_Index_Field_5, "300815" );
      }
      return *cp_child_Index_Field_5;
   }

   const Meta_Index& impl_child_Index_Field_5( ) const
   {
      if( !cp_child_Index_Field_5 )
      {
         cp_child_Index_Field_5.init( );

         p_obj->setup_graph_parent( *cp_child_Index_Field_5, "300815" );
      }
      return *cp_child_Index_Field_5;
   }

   Meta_Specification_Field_Action& impl_child_Specification_Field_Action_New_Record_FK( )
   {
      if( !cp_child_Specification_Field_Action_New_Record_FK )
      {
         cp_child_Specification_Field_Action_New_Record_FK.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Field_Action_New_Record_FK, "302310" );
      }
      return *cp_child_Specification_Field_Action_New_Record_FK;
   }

   const Meta_Specification_Field_Action& impl_child_Specification_Field_Action_New_Record_FK( ) const
   {
      if( !cp_child_Specification_Field_Action_New_Record_FK )
      {
         cp_child_Specification_Field_Action_New_Record_FK.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Field_Action_New_Record_FK, "302310" );
      }
      return *cp_child_Specification_Field_Action_New_Record_FK;
   }

   Meta_Specification& impl_child_Specification_Other_Class( )
   {
      if( !cp_child_Specification_Other_Class )
      {
         cp_child_Specification_Other_Class.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other_Class, "301437" );
      }
      return *cp_child_Specification_Other_Class;
   }

   const Meta_Specification& impl_child_Specification_Other_Class( ) const
   {
      if( !cp_child_Specification_Other_Class )
      {
         cp_child_Specification_Other_Class.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other_Class, "301437" );
      }
      return *cp_child_Specification_Other_Class;
   }

   Meta_Package_Option& impl_child_Package_Option_Other_Field_2( )
   {
      if( !cp_child_Package_Option_Other_Field_2 )
      {
         cp_child_Package_Option_Other_Field_2.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option_Other_Field_2, "302830e" );
      }
      return *cp_child_Package_Option_Other_Field_2;
   }

   const Meta_Package_Option& impl_child_Package_Option_Other_Field_2( ) const
   {
      if( !cp_child_Package_Option_Other_Field_2 )
      {
         cp_child_Package_Option_Other_Field_2.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option_Other_Field_2, "302830e" );
      }
      return *cp_child_Package_Option_Other_Field_2;
   }

   Meta_Specification& impl_child_Specification_Other_Field_2( )
   {
      if( !cp_child_Specification_Other_Field_2 )
      {
         cp_child_Specification_Other_Field_2.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other_Field_2, "301436" );
      }
      return *cp_child_Specification_Other_Field_2;
   }

   const Meta_Specification& impl_child_Specification_Other_Field_2( ) const
   {
      if( !cp_child_Specification_Other_Field_2 )
      {
         cp_child_Specification_Other_Field_2.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other_Field_2, "301436" );
      }
      return *cp_child_Specification_Other_Field_2;
   }

   Meta_Package_Option& impl_child_Package_Option_Other( )
   {
      if( !cp_child_Package_Option_Other )
      {
         cp_child_Package_Option_Other.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option_Other, "302830d" );
      }
      return *cp_child_Package_Option_Other;
   }

   const Meta_Package_Option& impl_child_Package_Option_Other( ) const
   {
      if( !cp_child_Package_Option_Other )
      {
         cp_child_Package_Option_Other.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option_Other, "302830d" );
      }
      return *cp_child_Package_Option_Other;
   }

   Meta_Specification& impl_child_Specification_Other( )
   {
      if( !cp_child_Specification_Other )
      {
         cp_child_Specification_Other.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other, "301435" );
      }
      return *cp_child_Specification_Other;
   }

   const Meta_Specification& impl_child_Specification_Other( ) const
   {
      if( !cp_child_Specification_Other )
      {
         cp_child_Specification_Other.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other, "301435" );
      }
      return *cp_child_Specification_Other;
   }

   Meta_Specification& impl_child_Specification_Other_Source_Child_2( )
   {
      if( !cp_child_Specification_Other_Source_Child_2 )
      {
         cp_child_Specification_Other_Source_Child_2.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other_Source_Child_2, "301497" );
      }
      return *cp_child_Specification_Other_Source_Child_2;
   }

   const Meta_Specification& impl_child_Specification_Other_Source_Child_2( ) const
   {
      if( !cp_child_Specification_Other_Source_Child_2 )
      {
         cp_child_Specification_Other_Source_Child_2.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other_Source_Child_2, "301497" );
      }
      return *cp_child_Specification_Other_Source_Child_2;
   }

   Meta_Specification& impl_child_Specification_Other_Source_Child( )
   {
      if( !cp_child_Specification_Other_Source_Child )
      {
         cp_child_Specification_Other_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other_Source_Child, "301496" );
      }
      return *cp_child_Specification_Other_Source_Child;
   }

   const Meta_Specification& impl_child_Specification_Other_Source_Child( ) const
   {
      if( !cp_child_Specification_Other_Source_Child )
      {
         cp_child_Specification_Other_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other_Source_Child, "301496" );
      }
      return *cp_child_Specification_Other_Source_Child;
   }

   Meta_Package_Option& impl_child_Package_Option_Other_Source( )
   {
      if( !cp_child_Package_Option_Other_Source )
      {
         cp_child_Package_Option_Other_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option_Other_Source, "302830g" );
      }
      return *cp_child_Package_Option_Other_Source;
   }

   const Meta_Package_Option& impl_child_Package_Option_Other_Source( ) const
   {
      if( !cp_child_Package_Option_Other_Source )
      {
         cp_child_Package_Option_Other_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option_Other_Source, "302830g" );
      }
      return *cp_child_Package_Option_Other_Source;
   }

   Meta_Relationship& impl_child_Relationship_Parent_Field_For_List( )
   {
      if( !cp_child_Relationship_Parent_Field_For_List )
      {
         cp_child_Relationship_Parent_Field_For_List.init( );

         p_obj->setup_graph_parent( *cp_child_Relationship_Parent_Field_For_List, "301330b" );
      }
      return *cp_child_Relationship_Parent_Field_For_List;
   }

   const Meta_Relationship& impl_child_Relationship_Parent_Field_For_List( ) const
   {
      if( !cp_child_Relationship_Parent_Field_For_List )
      {
         cp_child_Relationship_Parent_Field_For_List.init( );

         p_obj->setup_graph_parent( *cp_child_Relationship_Parent_Field_For_List, "301330b" );
      }
      return *cp_child_Relationship_Parent_Field_For_List;
   }

   Meta_Relationship& impl_child_Relationship_Parent_Field_For_View( )
   {
      if( !cp_child_Relationship_Parent_Field_For_View )
      {
         cp_child_Relationship_Parent_Field_For_View.init( );

         p_obj->setup_graph_parent( *cp_child_Relationship_Parent_Field_For_View, "301330a" );
      }
      return *cp_child_Relationship_Parent_Field_For_View;
   }

   const Meta_Relationship& impl_child_Relationship_Parent_Field_For_View( ) const
   {
      if( !cp_child_Relationship_Parent_Field_For_View )
      {
         cp_child_Relationship_Parent_Field_For_View.init( );

         p_obj->setup_graph_parent( *cp_child_Relationship_Parent_Field_For_View, "301330a" );
      }
      return *cp_child_Relationship_Parent_Field_For_View;
   }

   Meta_List& impl_child_List_Parent( )
   {
      if( !cp_child_List_Parent )
      {
         cp_child_List_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Parent, "301996" );
      }
      return *cp_child_List_Parent;
   }

   const Meta_List& impl_child_List_Parent( ) const
   {
      if( !cp_child_List_Parent )
      {
         cp_child_List_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Parent, "301996" );
      }
      return *cp_child_List_Parent;
   }

   Meta_Class& impl_child_Class_Quick_Link( )
   {
      if( !cp_child_Class_Quick_Link )
      {
         cp_child_Class_Quick_Link.init( );

         p_obj->setup_graph_parent( *cp_child_Class_Quick_Link, "300630" );
      }
      return *cp_child_Class_Quick_Link;
   }

   const Meta_Class& impl_child_Class_Quick_Link( ) const
   {
      if( !cp_child_Class_Quick_Link )
      {
         cp_child_Class_Quick_Link.init( );

         p_obj->setup_graph_parent( *cp_child_Class_Quick_Link, "300630" );
      }
      return *cp_child_Class_Quick_Link;
   }

   Meta_List_Field& impl_child_List_Field_Restriction( )
   {
      if( !cp_child_List_Field_Restriction )
      {
         cp_child_List_Field_Restriction.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Restriction, "302185" );
      }
      return *cp_child_List_Field_Restriction;
   }

   const Meta_List_Field& impl_child_List_Field_Restriction( ) const
   {
      if( !cp_child_List_Field_Restriction )
      {
         cp_child_List_Field_Restriction.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Restriction, "302185" );
      }
      return *cp_child_List_Field_Restriction;
   }

   Meta_List_Field& impl_child_List_Field_Source_Child( )
   {
      if( !cp_child_List_Field_Source_Child )
      {
         cp_child_List_Field_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Source_Child, "302160" );
      }
      return *cp_child_List_Field_Source_Child;
   }

   const Meta_List_Field& impl_child_List_Field_Source_Child( ) const
   {
      if( !cp_child_List_Field_Source_Child )
      {
         cp_child_List_Field_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Source_Child, "302160" );
      }
      return *cp_child_List_Field_Source_Child;
   }

   Meta_Specification& impl_child_Specification_Source_Child( )
   {
      if( !cp_child_Specification_Source_Child )
      {
         cp_child_Specification_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Source_Child, "301495" );
      }
      return *cp_child_Specification_Source_Child;
   }

   const Meta_Specification& impl_child_Specification_Source_Child( ) const
   {
      if( !cp_child_Specification_Source_Child )
      {
         cp_child_Specification_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Source_Child, "301495" );
      }
      return *cp_child_Specification_Source_Child;
   }

   Meta_View_Field& impl_child_View_Field_Source_Child( )
   {
      if( !cp_child_View_Field_Source_Child )
      {
         cp_child_View_Field_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_child_View_Field_Source_Child, "301960" );
      }
      return *cp_child_View_Field_Source_Child;
   }

   const Meta_View_Field& impl_child_View_Field_Source_Child( ) const
   {
      if( !cp_child_View_Field_Source_Child )
      {
         cp_child_View_Field_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_child_View_Field_Source_Child, "301960" );
      }
      return *cp_child_View_Field_Source_Child;
   }

   Meta_View_Field& impl_child_View_Field_Source_Edit_Child( )
   {
      if( !cp_child_View_Field_Source_Edit_Child )
      {
         cp_child_View_Field_Source_Edit_Child.init( );

         p_obj->setup_graph_parent( *cp_child_View_Field_Source_Edit_Child, "301965" );
      }
      return *cp_child_View_Field_Source_Edit_Child;
   }

   const Meta_View_Field& impl_child_View_Field_Source_Edit_Child( ) const
   {
      if( !cp_child_View_Field_Source_Edit_Child )
      {
         cp_child_View_Field_Source_Edit_Child.init( );

         p_obj->setup_graph_parent( *cp_child_View_Field_Source_Edit_Child, "301965" );
      }
      return *cp_child_View_Field_Source_Edit_Child;
   }

   Meta_List_Field& impl_child_List_Field_Source( )
   {
      if( !cp_child_List_Field_Source )
      {
         cp_child_List_Field_Source.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Source, "302140" );
      }
      return *cp_child_List_Field_Source;
   }

   const Meta_List_Field& impl_child_List_Field_Source( ) const
   {
      if( !cp_child_List_Field_Source )
      {
         cp_child_List_Field_Source.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Source, "302140" );
      }
      return *cp_child_List_Field_Source;
   }

   Meta_Package_Option& impl_child_Package_Option_Source( )
   {
      if( !cp_child_Package_Option_Source )
      {
         cp_child_Package_Option_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option_Source, "302830f" );
      }
      return *cp_child_Package_Option_Source;
   }

   const Meta_Package_Option& impl_child_Package_Option_Source( ) const
   {
      if( !cp_child_Package_Option_Source )
      {
         cp_child_Package_Option_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option_Source, "302830f" );
      }
      return *cp_child_Package_Option_Source;
   }

   Meta_Specification& impl_child_Specification_Source( )
   {
      if( !cp_child_Specification_Source )
      {
         cp_child_Specification_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Source, "301481" );
      }
      return *cp_child_Specification_Source;
   }

   const Meta_Specification& impl_child_Specification_Source( ) const
   {
      if( !cp_child_Specification_Source )
      {
         cp_child_Specification_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Source, "301481" );
      }
      return *cp_child_Specification_Source;
   }

   Meta_View_Field& impl_child_View_Field_Source( )
   {
      if( !cp_child_View_Field_Source )
      {
         cp_child_View_Field_Source.init( );

         p_obj->setup_graph_parent( *cp_child_View_Field_Source, "301940" );
      }
      return *cp_child_View_Field_Source;
   }

   const Meta_View_Field& impl_child_View_Field_Source( ) const
   {
      if( !cp_child_View_Field_Source )
      {
         cp_child_View_Field_Source.init( );

         p_obj->setup_graph_parent( *cp_child_View_Field_Source, "301940" );
      }
      return *cp_child_View_Field_Source;
   }

   Meta_List_Field& impl_child_List_Field_Source_Grandchild( )
   {
      if( !cp_child_List_Field_Source_Grandchild )
      {
         cp_child_List_Field_Source_Grandchild.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Source_Grandchild, "302165" );
      }
      return *cp_child_List_Field_Source_Grandchild;
   }

   const Meta_List_Field& impl_child_List_Field_Source_Grandchild( ) const
   {
      if( !cp_child_List_Field_Source_Grandchild )
      {
         cp_child_List_Field_Source_Grandchild.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Source_Grandchild, "302165" );
      }
      return *cp_child_List_Field_Source_Grandchild;
   }

   Meta_Specification& impl_child_Specification_Source_Grandchild( )
   {
      if( !cp_child_Specification_Source_Grandchild )
      {
         cp_child_Specification_Source_Grandchild.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Source_Grandchild, "301476" );
      }
      return *cp_child_Specification_Source_Grandchild;
   }

   const Meta_Specification& impl_child_Specification_Source_Grandchild( ) const
   {
      if( !cp_child_Specification_Source_Grandchild )
      {
         cp_child_Specification_Source_Grandchild.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Source_Grandchild, "301476" );
      }
      return *cp_child_Specification_Source_Grandchild;
   }

   Meta_List_Field& impl_child_List_Field_Source_Parent( )
   {
      if( !cp_child_List_Field_Source_Parent )
      {
         cp_child_List_Field_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Source_Parent, "302150" );
      }
      return *cp_child_List_Field_Source_Parent;
   }

   const Meta_List_Field& impl_child_List_Field_Source_Parent( ) const
   {
      if( !cp_child_List_Field_Source_Parent )
      {
         cp_child_List_Field_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Source_Parent, "302150" );
      }
      return *cp_child_List_Field_Source_Parent;
   }

   Meta_Specification& impl_child_Specification_Source_Parent( )
   {
      if( !cp_child_Specification_Source_Parent )
      {
         cp_child_Specification_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Source_Parent, "301480" );
      }
      return *cp_child_Specification_Source_Parent;
   }

   const Meta_Specification& impl_child_Specification_Source_Parent( ) const
   {
      if( !cp_child_Specification_Source_Parent )
      {
         cp_child_Specification_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Source_Parent, "301480" );
      }
      return *cp_child_Specification_Source_Parent;
   }

   Meta_View_Field& impl_child_View_Field_Source_Parent( )
   {
      if( !cp_child_View_Field_Source_Parent )
      {
         cp_child_View_Field_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_View_Field_Source_Parent, "301950" );
      }
      return *cp_child_View_Field_Source_Parent;
   }

   const Meta_View_Field& impl_child_View_Field_Source_Parent( ) const
   {
      if( !cp_child_View_Field_Source_Parent )
      {
         cp_child_View_Field_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_View_Field_Source_Parent, "301950" );
      }
      return *cp_child_View_Field_Source_Parent;
   }

   Meta_Field& impl_child_Field_Source( )
   {
      if( !cp_child_Field_Source )
      {
         cp_child_Field_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Field_Source, "300740" );
      }
      return *cp_child_Field_Source;
   }

   const Meta_Field& impl_child_Field_Source( ) const
   {
      if( !cp_child_Field_Source )
      {
         cp_child_Field_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Field_Source, "300740" );
      }
      return *cp_child_Field_Source;
   }

   Meta_Specification& impl_child_Specification_Test_Child( )
   {
      if( !cp_child_Specification_Test_Child )
      {
         cp_child_Specification_Test_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Test_Child, "301488" );
      }
      return *cp_child_Specification_Test_Child;
   }

   const Meta_Specification& impl_child_Specification_Test_Child( ) const
   {
      if( !cp_child_Specification_Test_Child )
      {
         cp_child_Specification_Test_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Test_Child, "301488" );
      }
      return *cp_child_Specification_Test_Child;
   }

   Meta_Specification& impl_child_Specification_Test( )
   {
      if( !cp_child_Specification_Test )
      {
         cp_child_Specification_Test.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Test, "301485" );
      }
      return *cp_child_Specification_Test;
   }

   const Meta_Specification& impl_child_Specification_Test( ) const
   {
      if( !cp_child_Specification_Test )
      {
         cp_child_Specification_Test.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Test, "301485" );
      }
      return *cp_child_Specification_Test;
   }

   Meta_Specification& impl_child_Specification_Test_Parent( )
   {
      if( !cp_child_Specification_Test_Parent )
      {
         cp_child_Specification_Test_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Test_Parent, "301486" );
      }
      return *cp_child_Specification_Test_Parent;
   }

   const Meta_Specification& impl_child_Specification_Test_Parent( ) const
   {
      if( !cp_child_Specification_Test_Parent )
      {
         cp_child_Specification_Test_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Test_Parent, "301486" );
      }
      return *cp_child_Specification_Test_Parent;
   }

   Meta_List_Field& impl_child_List_Field_Use_Type( )
   {
      if( !cp_child_List_Field_Use_Type )
      {
         cp_child_List_Field_Use_Type.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Use_Type, "302176" );
      }
      return *cp_child_List_Field_Use_Type;
   }

   const Meta_List_Field& impl_child_List_Field_Use_Type( ) const
   {
      if( !cp_child_List_Field_Use_Type )
      {
         cp_child_List_Field_Use_Type.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Use_Type, "302176" );
      }
      return *cp_child_List_Field_Use_Type;
   }

   Meta_Initial_Record_Value& impl_child_Initial_Record_Value( )
   {
      if( !cp_child_Initial_Record_Value )
      {
         cp_child_Initial_Record_Value.init( );

         p_obj->setup_graph_parent( *cp_child_Initial_Record_Value, "302520" );
      }
      return *cp_child_Initial_Record_Value;
   }

   const Meta_Initial_Record_Value& impl_child_Initial_Record_Value( ) const
   {
      if( !cp_child_Initial_Record_Value )
      {
         cp_child_Initial_Record_Value.init( );

         p_obj->setup_graph_parent( *cp_child_Initial_Record_Value, "302520" );
      }
      return *cp_child_Initial_Record_Value;
   }

   Meta_Modifier_Affect& impl_child_Modifier_Affect( )
   {
      if( !cp_child_Modifier_Affect )
      {
         cp_child_Modifier_Affect.init( );

         p_obj->setup_graph_parent( *cp_child_Modifier_Affect, "301020" );
      }
      return *cp_child_Modifier_Affect;
   }

   const Meta_Modifier_Affect& impl_child_Modifier_Affect( ) const
   {
      if( !cp_child_Modifier_Affect )
      {
         cp_child_Modifier_Affect.init( );

         p_obj->setup_graph_parent( *cp_child_Modifier_Affect, "301020" );
      }
      return *cp_child_Modifier_Affect;
   }

   Meta_Package_Option& impl_child_Package_Option( )
   {
      if( !cp_child_Package_Option )
      {
         cp_child_Package_Option.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option, "302830b" );
      }
      return *cp_child_Package_Option;
   }

   const Meta_Package_Option& impl_child_Package_Option( ) const
   {
      if( !cp_child_Package_Option )
      {
         cp_child_Package_Option.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option, "302830b" );
      }
      return *cp_child_Package_Option;
   }

   Meta_Specification& impl_child_Specification( )
   {
      if( !cp_child_Specification )
      {
         cp_child_Specification.init( );

         p_obj->setup_graph_parent( *cp_child_Specification, "301430" );
      }
      return *cp_child_Specification;
   }

   const Meta_Specification& impl_child_Specification( ) const
   {
      if( !cp_child_Specification )
      {
         cp_child_Specification.init( );

         p_obj->setup_graph_parent( *cp_child_Specification, "301430" );
      }
      return *cp_child_Specification;
   }

   void impl_Get_Text_Type( int& Result );

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

   Meta_Field* p_obj;
   class_pointer< Meta_Field > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   int v_Access_Restriction;
   int v_Access_Scope;
   int v_Change_Restriction;
   int v_Change_Scope;
   bool v_Create_List_Field;
   bool v_Create_View_Field;
   string v_Def_Value;
   string v_Default;
   bool v_Dummy_1;
   bool v_Encrypted;
   int v_Extra;
   string v_Extra_Filter;
   string v_Id;
   bool v_Internal;
   bool v_Is_Foreign_Key;
   bool v_Is_Text_Type;
   bool v_Mandatory;
   string v_Name;
   numeric v_Numeric_Decimals;
   string v_Parent_Class_Name;
   int v_Primitive;
   bool v_Transient;
   int v_UOM;
   string v_UOM_Name;
   string v_UOM_Symbol;
   bool v_Use_In_Text_Search;

   string v_Access_Permission;
   mutable class_pointer< Meta_Permission > cp_Access_Permission;

   string v_Change_Permission;
   mutable class_pointer< Meta_Permission > cp_Change_Permission;

   string v_Class;
   mutable class_pointer< Meta_Class > cp_Class;

   string v_Enum;
   mutable class_pointer< Meta_Enum > cp_Enum;

   string v_Enum_Filter;
   mutable class_pointer< Meta_Enum > cp_Enum_Filter;

   string v_Parent_Class;
   mutable class_pointer< Meta_Class > cp_Parent_Class;

   string v_Source_Field;
   mutable class_pointer< Meta_Field > cp_Source_Field;

   string v_Type;
   mutable class_pointer< Meta_Type > cp_Type;

   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Child_Rel_Source_Child;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Child_Rel_Source;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Child_Rel_Source_Parent;
   mutable class_pointer< Meta_Index > cp_child_Index_Field_1;
   mutable class_pointer< Meta_Index > cp_child_Index_Field_2;
   mutable class_pointer< Meta_Index > cp_child_Index_Field_3;
   mutable class_pointer< Meta_Index > cp_child_Index_Field_4;
   mutable class_pointer< Meta_Index > cp_child_Index_Field_5;
   mutable class_pointer< Meta_Specification_Field_Action > cp_child_Specification_Field_Action_New_Record_FK;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Other_Class;
   mutable class_pointer< Meta_Package_Option > cp_child_Package_Option_Other_Field_2;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Other_Field_2;
   mutable class_pointer< Meta_Package_Option > cp_child_Package_Option_Other;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Other;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Other_Source_Child_2;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Other_Source_Child;
   mutable class_pointer< Meta_Package_Option > cp_child_Package_Option_Other_Source;
   mutable class_pointer< Meta_Relationship > cp_child_Relationship_Parent_Field_For_List;
   mutable class_pointer< Meta_Relationship > cp_child_Relationship_Parent_Field_For_View;
   mutable class_pointer< Meta_List > cp_child_List_Parent;
   mutable class_pointer< Meta_Class > cp_child_Class_Quick_Link;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Restriction;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Source_Child;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Source_Child;
   mutable class_pointer< Meta_View_Field > cp_child_View_Field_Source_Child;
   mutable class_pointer< Meta_View_Field > cp_child_View_Field_Source_Edit_Child;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Source;
   mutable class_pointer< Meta_Package_Option > cp_child_Package_Option_Source;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Source;
   mutable class_pointer< Meta_View_Field > cp_child_View_Field_Source;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Source_Grandchild;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Source_Grandchild;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Source_Parent;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Source_Parent;
   mutable class_pointer< Meta_View_Field > cp_child_View_Field_Source_Parent;
   mutable class_pointer< Meta_Field > cp_child_Field_Source;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Test_Child;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Test;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Test_Parent;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Use_Type;
   mutable class_pointer< Meta_Initial_Record_Value > cp_child_Initial_Record_Value;
   mutable class_pointer< Meta_Modifier_Affect > cp_child_Modifier_Affect;
   mutable class_pointer< Meta_Package_Option > cp_child_Package_Option;
   mutable class_pointer< Meta_Specification > cp_child_Specification;
};

void Meta_Field::impl::impl_Get_Text_Type( int& Result )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Get_Text_Type_impl>]
//nyi
   Result = e_sql_char_type_large;

   if( get_obj( ).Extra( ) == c_enum_field_extra_html )
      Result = e_sql_char_type_very_large;

   if( get_obj( ).Extra( ) == c_enum_field_extra_text )
      Result = e_sql_char_type_very_large;

   if( get_obj( ).Extra( ) == c_enum_field_extra_notes )
      Result = e_sql_char_type_very_large;

   if( get_obj( ).Extra( ) == c_enum_field_extra_manual_link )
      Result = e_sql_char_type_very_large;

   if( get_obj( ).Type( ).Max_Size( )
    && get_obj( ).Type( ).Max_Size( ) <= c_sql_std_char_size )
      Result = e_sql_char_type_std;

   if( get_obj( ).Type( ).Max_Size( )
    && get_obj( ).Type( ).Max_Size( ) <= c_sql_small_char_size )
      Result = e_sql_char_type_small;

   if( get_obj( ).Extra( ) == c_enum_field_extra_security_level )
      Result = e_sql_char_type_security;

   if( !get_obj( ).Parent_Class( ).get_key( ).empty( ) )
      Result = e_sql_char_type_foreign_key;
   // [<finish Get_Text_Type_impl>]
}

string Meta_Field::impl::get_field_value( int field ) const
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
      retval = to_string( impl_Change_Permission( ) );
      break;

      case 4:
      retval = to_string( impl_Change_Restriction( ) );
      break;

      case 5:
      retval = to_string( impl_Change_Scope( ) );
      break;

      case 6:
      retval = to_string( impl_Class( ) );
      break;

      case 7:
      retval = to_string( impl_Create_List_Field( ) );
      break;

      case 8:
      retval = to_string( impl_Create_View_Field( ) );
      break;

      case 9:
      retval = to_string( impl_Def_Value( ) );
      break;

      case 10:
      retval = to_string( impl_Default( ) );
      break;

      case 11:
      retval = to_string( impl_Dummy_1( ) );
      break;

      case 12:
      retval = to_string( impl_Encrypted( ) );
      break;

      case 13:
      retval = to_string( impl_Enum( ) );
      break;

      case 14:
      retval = to_string( impl_Enum_Filter( ) );
      break;

      case 15:
      retval = to_string( impl_Extra( ) );
      break;

      case 16:
      retval = to_string( impl_Extra_Filter( ) );
      break;

      case 17:
      retval = to_string( impl_Id( ) );
      break;

      case 18:
      retval = to_string( impl_Internal( ) );
      break;

      case 19:
      retval = to_string( impl_Is_Foreign_Key( ) );
      break;

      case 20:
      retval = to_string( impl_Is_Text_Type( ) );
      break;

      case 21:
      retval = to_string( impl_Mandatory( ) );
      break;

      case 22:
      retval = to_string( impl_Name( ) );
      break;

      case 23:
      retval = to_string( impl_Numeric_Decimals( ) );
      break;

      case 24:
      retval = to_string( impl_Parent_Class( ) );
      break;

      case 25:
      retval = to_string( impl_Parent_Class_Name( ) );
      break;

      case 26:
      retval = to_string( impl_Primitive( ) );
      break;

      case 27:
      retval = to_string( impl_Source_Field( ) );
      break;

      case 28:
      retval = to_string( impl_Transient( ) );
      break;

      case 29:
      retval = to_string( impl_Type( ) );
      break;

      case 30:
      retval = to_string( impl_UOM( ) );
      break;

      case 31:
      retval = to_string( impl_UOM_Name( ) );
      break;

      case 32:
      retval = to_string( impl_UOM_Symbol( ) );
      break;

      case 33:
      retval = to_string( impl_Use_In_Text_Search( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_Field::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Field::impl, Meta_Permission >( *this, &Meta_Field::impl::impl_Access_Permission, value );
      break;

      case 1:
      func_string_setter< Meta_Field::impl, int >( *this, &Meta_Field::impl::impl_Access_Restriction, value );
      break;

      case 2:
      func_string_setter< Meta_Field::impl, int >( *this, &Meta_Field::impl::impl_Access_Scope, value );
      break;

      case 3:
      func_string_setter< Meta_Field::impl, Meta_Permission >( *this, &Meta_Field::impl::impl_Change_Permission, value );
      break;

      case 4:
      func_string_setter< Meta_Field::impl, int >( *this, &Meta_Field::impl::impl_Change_Restriction, value );
      break;

      case 5:
      func_string_setter< Meta_Field::impl, int >( *this, &Meta_Field::impl::impl_Change_Scope, value );
      break;

      case 6:
      func_string_setter< Meta_Field::impl, Meta_Class >( *this, &Meta_Field::impl::impl_Class, value );
      break;

      case 7:
      func_string_setter< Meta_Field::impl, bool >( *this, &Meta_Field::impl::impl_Create_List_Field, value );
      break;

      case 8:
      func_string_setter< Meta_Field::impl, bool >( *this, &Meta_Field::impl::impl_Create_View_Field, value );
      break;

      case 9:
      func_string_setter< Meta_Field::impl, string >( *this, &Meta_Field::impl::impl_Def_Value, value );
      break;

      case 10:
      func_string_setter< Meta_Field::impl, string >( *this, &Meta_Field::impl::impl_Default, value );
      break;

      case 11:
      func_string_setter< Meta_Field::impl, bool >( *this, &Meta_Field::impl::impl_Dummy_1, value );
      break;

      case 12:
      func_string_setter< Meta_Field::impl, bool >( *this, &Meta_Field::impl::impl_Encrypted, value );
      break;

      case 13:
      func_string_setter< Meta_Field::impl, Meta_Enum >( *this, &Meta_Field::impl::impl_Enum, value );
      break;

      case 14:
      func_string_setter< Meta_Field::impl, Meta_Enum >( *this, &Meta_Field::impl::impl_Enum_Filter, value );
      break;

      case 15:
      func_string_setter< Meta_Field::impl, int >( *this, &Meta_Field::impl::impl_Extra, value );
      break;

      case 16:
      func_string_setter< Meta_Field::impl, string >( *this, &Meta_Field::impl::impl_Extra_Filter, value );
      break;

      case 17:
      func_string_setter< Meta_Field::impl, string >( *this, &Meta_Field::impl::impl_Id, value );
      break;

      case 18:
      func_string_setter< Meta_Field::impl, bool >( *this, &Meta_Field::impl::impl_Internal, value );
      break;

      case 19:
      func_string_setter< Meta_Field::impl, bool >( *this, &Meta_Field::impl::impl_Is_Foreign_Key, value );
      break;

      case 20:
      func_string_setter< Meta_Field::impl, bool >( *this, &Meta_Field::impl::impl_Is_Text_Type, value );
      break;

      case 21:
      func_string_setter< Meta_Field::impl, bool >( *this, &Meta_Field::impl::impl_Mandatory, value );
      break;

      case 22:
      func_string_setter< Meta_Field::impl, string >( *this, &Meta_Field::impl::impl_Name, value );
      break;

      case 23:
      func_string_setter< Meta_Field::impl, numeric >( *this, &Meta_Field::impl::impl_Numeric_Decimals, value );
      break;

      case 24:
      func_string_setter< Meta_Field::impl, Meta_Class >( *this, &Meta_Field::impl::impl_Parent_Class, value );
      break;

      case 25:
      func_string_setter< Meta_Field::impl, string >( *this, &Meta_Field::impl::impl_Parent_Class_Name, value );
      break;

      case 26:
      func_string_setter< Meta_Field::impl, int >( *this, &Meta_Field::impl::impl_Primitive, value );
      break;

      case 27:
      func_string_setter< Meta_Field::impl, Meta_Field >( *this, &Meta_Field::impl::impl_Source_Field, value );
      break;

      case 28:
      func_string_setter< Meta_Field::impl, bool >( *this, &Meta_Field::impl::impl_Transient, value );
      break;

      case 29:
      func_string_setter< Meta_Field::impl, Meta_Type >( *this, &Meta_Field::impl::impl_Type, value );
      break;

      case 30:
      func_string_setter< Meta_Field::impl, int >( *this, &Meta_Field::impl::impl_UOM, value );
      break;

      case 31:
      func_string_setter< Meta_Field::impl, string >( *this, &Meta_Field::impl::impl_UOM_Name, value );
      break;

      case 32:
      func_string_setter< Meta_Field::impl, string >( *this, &Meta_Field::impl::impl_UOM_Symbol, value );
      break;

      case 33:
      func_string_setter< Meta_Field::impl, bool >( *this, &Meta_Field::impl::impl_Use_In_Text_Search, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_Field::impl::set_field_default( int field )
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
      impl_Change_Permission( g_default_Change_Permission );
      break;

      case 4:
      impl_Change_Restriction( g_default_Change_Restriction );
      break;

      case 5:
      impl_Change_Scope( g_default_Change_Scope );
      break;

      case 6:
      impl_Class( g_default_Class );
      break;

      case 7:
      impl_Create_List_Field( g_default_Create_List_Field );
      break;

      case 8:
      impl_Create_View_Field( g_default_Create_View_Field );
      break;

      case 9:
      impl_Def_Value( g_default_Def_Value );
      break;

      case 10:
      impl_Default( g_default_Default );
      break;

      case 11:
      impl_Dummy_1( g_default_Dummy_1 );
      break;

      case 12:
      impl_Encrypted( g_default_Encrypted );
      break;

      case 13:
      impl_Enum( g_default_Enum );
      break;

      case 14:
      impl_Enum_Filter( g_default_Enum_Filter );
      break;

      case 15:
      impl_Extra( g_default_Extra );
      break;

      case 16:
      impl_Extra_Filter( g_default_Extra_Filter );
      break;

      case 17:
      impl_Id( g_default_Id );
      break;

      case 18:
      impl_Internal( g_default_Internal );
      break;

      case 19:
      impl_Is_Foreign_Key( g_default_Is_Foreign_Key );
      break;

      case 20:
      impl_Is_Text_Type( g_default_Is_Text_Type );
      break;

      case 21:
      impl_Mandatory( g_default_Mandatory );
      break;

      case 22:
      impl_Name( g_default_Name );
      break;

      case 23:
      impl_Numeric_Decimals( g_default_Numeric_Decimals );
      break;

      case 24:
      impl_Parent_Class( g_default_Parent_Class );
      break;

      case 25:
      impl_Parent_Class_Name( g_default_Parent_Class_Name );
      break;

      case 26:
      impl_Primitive( g_default_Primitive );
      break;

      case 27:
      impl_Source_Field( g_default_Source_Field );
      break;

      case 28:
      impl_Transient( g_default_Transient );
      break;

      case 29:
      impl_Type( g_default_Type );
      break;

      case 30:
      impl_UOM( g_default_UOM );
      break;

      case 31:
      impl_UOM_Name( g_default_UOM_Name );
      break;

      case 32:
      impl_UOM_Symbol( g_default_UOM_Symbol );
      break;

      case 33:
      impl_Use_In_Text_Search( g_default_Use_In_Text_Search );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_Field::impl::is_field_default( int field ) const
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
      retval = ( v_Change_Permission == g_default_Change_Permission );
      break;

      case 4:
      retval = ( v_Change_Restriction == g_default_Change_Restriction );
      break;

      case 5:
      retval = ( v_Change_Scope == g_default_Change_Scope );
      break;

      case 6:
      retval = ( v_Class == g_default_Class );
      break;

      case 7:
      retval = ( v_Create_List_Field == g_default_Create_List_Field );
      break;

      case 8:
      retval = ( v_Create_View_Field == g_default_Create_View_Field );
      break;

      case 9:
      retval = ( v_Def_Value == g_default_Def_Value );
      break;

      case 10:
      retval = ( v_Default == g_default_Default );
      break;

      case 11:
      retval = ( v_Dummy_1 == g_default_Dummy_1 );
      break;

      case 12:
      retval = ( v_Encrypted == g_default_Encrypted );
      break;

      case 13:
      retval = ( v_Enum == g_default_Enum );
      break;

      case 14:
      retval = ( v_Enum_Filter == g_default_Enum_Filter );
      break;

      case 15:
      retval = ( v_Extra == g_default_Extra );
      break;

      case 16:
      retval = ( v_Extra_Filter == g_default_Extra_Filter );
      break;

      case 17:
      retval = ( v_Id == g_default_Id );
      break;

      case 18:
      retval = ( v_Internal == g_default_Internal );
      break;

      case 19:
      retval = ( v_Is_Foreign_Key == g_default_Is_Foreign_Key );
      break;

      case 20:
      retval = ( v_Is_Text_Type == g_default_Is_Text_Type );
      break;

      case 21:
      retval = ( v_Mandatory == g_default_Mandatory );
      break;

      case 22:
      retval = ( v_Name == g_default_Name );
      break;

      case 23:
      retval = ( v_Numeric_Decimals == g_default_Numeric_Decimals );
      break;

      case 24:
      retval = ( v_Parent_Class == g_default_Parent_Class );
      break;

      case 25:
      retval = ( v_Parent_Class_Name == g_default_Parent_Class_Name );
      break;

      case 26:
      retval = ( v_Primitive == g_default_Primitive );
      break;

      case 27:
      retval = ( v_Source_Field == g_default_Source_Field );
      break;

      case 28:
      retval = ( v_Transient == g_default_Transient );
      break;

      case 29:
      retval = ( v_Type == g_default_Type );
      break;

      case 30:
      retval = ( v_UOM == g_default_UOM );
      break;

      case 31:
      retval = ( v_UOM_Name == g_default_UOM_Name );
      break;

      case 32:
      retval = ( v_UOM_Symbol == g_default_UOM_Symbol );
      break;

      case 33:
      retval = ( v_Use_In_Text_Search == g_default_Use_In_Text_Search );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_Field::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start modifier_field_value)] 600047
   if( get_obj( ).UOM( ) != 999 )
      state |= c_modifier_Is_Non_Custom_UOM;
   // [(finish modifier_field_value)] 600047

   // [(start modifier_field_value)] 600048
   if( get_obj( ).Type( ).Primitive( ) == 5 ) // i.e. int
      state |= c_modifier_Is_Any_Non_Text_Type;
   // [(finish modifier_field_value)] 600048

   // [(start modifier_field_value)] 600049
   if( get_obj( ).Type( ).Primitive( ) == 6 ) // i.e. bool
      state |= c_modifier_Is_Any_Non_Text_Type;
   // [(finish modifier_field_value)] 600049

   // [(start modifier_field_value)] 600050
   if( get_obj( ).Type( ).Primitive( ) == 4 ) // i.e. numeric
      state |= c_modifier_Is_Any_Non_Text_Type;
   // [(finish modifier_field_value)] 600050

   // [(start protect_equal)] 600051
   if( check_equal( get_obj( ).Internal( ), true ) )
      state |= ( c_state_uneditable | c_state_undeletable );
   // [(finish protect_equal)] 600051

   // [(start modifier_field_value)] 600052
   if( get_obj( ).Internal( ) == true )
      state |= c_modifier_Is_Internal;
   // [(finish modifier_field_value)] 600052

   // [(start modifier_field_null)] 600052a
   if( is_null( get_obj( ).Class( ).Created_View( ) ) )
      state |= c_modifier_Hide_Create_View_Field;
   // [(finish modifier_field_null)] 600052a

   // [(start modifier_field_null)] 600052b
   if( is_null( get_obj( ).Class( ).Created_List( ) ) )
      state |= c_modifier_Hide_Create_List_Field;
   // [(finish modifier_field_null)] 600052b

   // [(start modifier_field_value)] 610050
   if( get_obj( ).Transient( ) == true )
      state |= c_modifier_Is_Transient;
   // [(finish modifier_field_value)] 610050

   // [(start modifier_field_value)] 620050
   if( get_obj( ).Type( ).Primitive( ) == 6 ) // i.e. bool
      state |= c_modifier_Is_Type_bool;
   // [(finish modifier_field_value)] 620050

   // [(start modifier_field_value)] 630050
   if( get_obj( ).Type( ).Primitive( ) == 2 ) // i.e. date
      state |= c_modifier_Is_Type_date;
   // [(finish modifier_field_value)] 630050

   // [(start modifier_field_value)] 640050
   if( get_obj( ).Type( ).Primitive( ) == 3 ) // i.e. time
      state |= c_modifier_Is_Type_time;
   // [(finish modifier_field_value)] 640050

   // [(start modifier_field_value)] 650050
   if( get_obj( ).Type( ).Primitive( ) == 1 ) // i.e. datetime
      state |= c_modifier_Is_Type_datetime;
   // [(finish modifier_field_value)] 650050

   // [(start modifier_field_value)] 660050
   if( get_obj( ).Type( ).Primitive( ) == 0 ) // i.e. string
      state |= c_modifier_Is_Type_string;
   // [(finish modifier_field_value)] 660050

   // [(start modifier_field_value)] 670050
   if( get_obj( ).Type( ).Primitive( ) != 0 ) // i.e. string
      state |= c_modifier_Is_Not_Type_string;
   // [(finish modifier_field_value)] 670050

   // [(start modifier_field_null)] 680050
   if( !is_null( get_obj( ).Enum( ) ) )
      state |= c_modifier_Has_Enum;
   // [(finish modifier_field_null)] 680050

   // [(start modifier_field_value)] 690050
   if( get_obj( ).Encrypted( ) == true )
      state |= c_modifier_Is_Encrypted;
   // [(finish modifier_field_value)] 690050

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

string Meta_Field::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   if( state & c_modifier_Has_Enum )
      state_names += "|" + string( "Has_Enum" );
   if( state & c_modifier_Hide_Create_List_Field )
      state_names += "|" + string( "Hide_Create_List_Field" );
   if( state & c_modifier_Hide_Create_View_Field )
      state_names += "|" + string( "Hide_Create_View_Field" );
   if( state & c_modifier_Is_Any_Non_Text_Type )
      state_names += "|" + string( "Is_Any_Non_Text_Type" );
   if( state & c_modifier_Is_Encrypted )
      state_names += "|" + string( "Is_Encrypted" );
   if( state & c_modifier_Is_Internal )
      state_names += "|" + string( "Is_Internal" );
   if( state & c_modifier_Is_Non_Custom_UOM )
      state_names += "|" + string( "Is_Non_Custom_UOM" );
   if( state & c_modifier_Is_Not_Type_string )
      state_names += "|" + string( "Is_Not_Type_string" );
   if( state & c_modifier_Is_Transient )
      state_names += "|" + string( "Is_Transient" );
   if( state & c_modifier_Is_Type_bool )
      state_names += "|" + string( "Is_Type_bool" );
   if( state & c_modifier_Is_Type_date )
      state_names += "|" + string( "Is_Type_date" );
   if( state & c_modifier_Is_Type_datetime )
      state_names += "|" + string( "Is_Type_datetime" );
   if( state & c_modifier_Is_Type_string )
      state_names += "|" + string( "Is_Type_string" );
   if( state & c_modifier_Is_Type_time )
      state_names += "|" + string( "Is_Type_time" );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_Field::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Field::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      impl_Access_Permission( "" );
   else if( ( field == c_field_id_Change_Permission ) || ( field == c_field_name_Change_Permission ) )
      impl_Change_Permission( "" );
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      impl_Class( "" );
   else if( ( field == c_field_id_Enum ) || ( field == c_field_name_Enum ) )
      impl_Enum( "" );
   else if( ( field == c_field_id_Enum_Filter ) || ( field == c_field_name_Enum_Filter ) )
      impl_Enum_Filter( "" );
   else if( ( field == c_field_id_Parent_Class ) || ( field == c_field_name_Parent_Class ) )
      impl_Parent_Class( "" );
   else if( ( field == c_field_id_Source_Field ) || ( field == c_field_name_Source_Field ) )
      impl_Source_Field( "" );
   else if( ( field == c_field_id_Type ) || ( field == c_field_name_Type ) )
      impl_Type( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Field::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      v_Access_Permission = value;
   else if( ( field == c_field_id_Change_Permission ) || ( field == c_field_name_Change_Permission ) )
      v_Change_Permission = value;
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      v_Class = value;
   else if( ( field == c_field_id_Enum ) || ( field == c_field_name_Enum ) )
      v_Enum = value;
   else if( ( field == c_field_id_Enum_Filter ) || ( field == c_field_name_Enum_Filter ) )
      v_Enum_Filter = value;
   else if( ( field == c_field_id_Parent_Class ) || ( field == c_field_name_Parent_Class ) )
      v_Parent_Class = value;
   else if( ( field == c_field_id_Source_Field ) || ( field == c_field_name_Source_Field ) )
      v_Source_Field = value;
   else if( ( field == c_field_id_Type ) || ( field == c_field_name_Type ) )
      v_Type = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Field::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      return v_Access_Permission;
   else if( ( field == c_field_id_Change_Permission ) || ( field == c_field_name_Change_Permission ) )
      return v_Change_Permission;
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      return v_Class;
   else if( ( field == c_field_id_Enum ) || ( field == c_field_name_Enum ) )
      return v_Enum;
   else if( ( field == c_field_id_Enum_Filter ) || ( field == c_field_name_Enum_Filter ) )
      return v_Enum_Filter;
   else if( ( field == c_field_id_Parent_Class ) || ( field == c_field_name_Parent_Class ) )
      return v_Parent_Class;
   else if( ( field == c_field_id_Source_Field ) || ( field == c_field_name_Source_Field ) )
      return v_Source_Field;
   else if( ( field == c_field_id_Type ) || ( field == c_field_name_Type ) )
      return v_Type;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Field::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Access_Permission, v_Access_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Change_Permission, v_Change_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Class, v_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Enum, v_Enum ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Enum_Filter, v_Enum_Filter ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Parent_Class, v_Parent_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Field, v_Source_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Type, v_Type ) );
}

void Meta_Field::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_Field::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Field::impl::clear( )
{
   v_Access_Restriction = g_default_Access_Restriction;
   v_Access_Scope = g_default_Access_Scope;
   v_Change_Restriction = g_default_Change_Restriction;
   v_Change_Scope = g_default_Change_Scope;
   v_Create_List_Field = g_default_Create_List_Field;
   v_Create_View_Field = g_default_Create_View_Field;
   v_Def_Value = g_default_Def_Value;
   v_Default = g_default_Default;
   v_Dummy_1 = g_default_Dummy_1;
   v_Encrypted = g_default_Encrypted;
   v_Extra = g_default_Extra;
   v_Extra_Filter = g_default_Extra_Filter;
   v_Id = g_default_Id;
   v_Internal = g_default_Internal;
   v_Is_Foreign_Key = g_default_Is_Foreign_Key;
   v_Is_Text_Type = g_default_Is_Text_Type;
   v_Mandatory = g_default_Mandatory;
   v_Name = g_default_Name;
   v_Numeric_Decimals = g_default_Numeric_Decimals;
   v_Parent_Class_Name = g_default_Parent_Class_Name;
   v_Primitive = g_default_Primitive;
   v_Transient = g_default_Transient;
   v_UOM = g_default_UOM;
   v_UOM_Name = g_default_UOM_Name;
   v_UOM_Symbol = g_default_UOM_Symbol;
   v_Use_In_Text_Search = g_default_Use_In_Text_Search;

   v_Access_Permission = string( );
   if( cp_Access_Permission )
      p_obj->setup_foreign_key( *cp_Access_Permission, v_Access_Permission );

   v_Change_Permission = string( );
   if( cp_Change_Permission )
      p_obj->setup_foreign_key( *cp_Change_Permission, v_Change_Permission );

   v_Class = string( );
   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   v_Enum = string( );
   if( cp_Enum )
      p_obj->setup_foreign_key( *cp_Enum, v_Enum );

   v_Enum_Filter = string( );
   if( cp_Enum_Filter )
      p_obj->setup_foreign_key( *cp_Enum_Filter, v_Enum_Filter );

   v_Parent_Class = string( );
   if( cp_Parent_Class )
      p_obj->setup_foreign_key( *cp_Parent_Class, v_Parent_Class );

   v_Source_Field = string( );
   if( cp_Source_Field )
      p_obj->setup_foreign_key( *cp_Source_Field, v_Source_Field );

   v_Type = string( );
   if( cp_Type )
      p_obj->setup_foreign_key( *cp_Type, v_Type );
}

bool Meta_Field::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [(start parent_auto_int_inc)] 620065
   if( field_name == "Id" )
      return true;
   // [(finish parent_auto_int_inc)] 620065

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Field::impl::validate(
 uint64_t state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( is_null( v_Id ) && !value_will_be_provided( c_field_name_Id ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Id,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Id ) ) ) ) );

   if( is_null( v_Name ) && !value_will_be_provided( c_field_name_Name ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Name ) ) ) ) );

   if( v_Class.empty( ) && !value_will_be_provided( c_field_name_Class ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Class ) ) ) ) );

   if( v_Type.empty( ) && !value_will_be_provided( c_field_name_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Type,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Type ) ) ) ) );

   if( !is_null( v_Default )
    && ( v_Default != g_default_Default
    || !value_will_be_provided( c_field_name_Default ) )
    && !g_Default_domain.is_valid( v_Default, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Default,
       get_module_string( c_field_display_name_Default ) + " " + error_message ) );

   if( !is_null( v_Id )
    && ( v_Id != g_default_Id
    || !value_will_be_provided( c_field_name_Id ) )
    && !g_Id_domain.is_valid( v_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Id,
       get_module_string( c_field_display_name_Id ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( v_Name != g_default_Name
    || !value_will_be_provided( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   if( !is_null( v_Parent_Class_Name )
    && ( v_Parent_Class_Name != g_default_Parent_Class_Name
    || !value_will_be_provided( c_field_name_Parent_Class_Name ) )
    && !g_Parent_Class_Name_domain.is_valid( v_Parent_Class_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Parent_Class_Name,
       get_module_string( c_field_display_name_Parent_Class_Name ) + " " + error_message ) );

   if( !is_null( v_UOM_Name )
    && ( v_UOM_Name != g_default_UOM_Name
    || !value_will_be_provided( c_field_name_UOM_Name ) )
    && !g_UOM_Name_domain.is_valid( v_UOM_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_UOM_Name,
       get_module_string( c_field_display_name_UOM_Name ) + " " + error_message ) );

   if( !is_null( v_UOM_Symbol )
    && ( v_UOM_Symbol != g_default_UOM_Symbol
    || !value_will_be_provided( c_field_name_UOM_Symbol ) )
    && !g_UOM_Symbol_domain.is_valid( v_UOM_Symbol, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_UOM_Symbol,
       get_module_string( c_field_display_name_UOM_Symbol ) + " " + error_message ) );

   if( !g_view_access_restrict_enum.count( v_Access_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Access_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Access_Restriction ) ) ) ) );

   if( !g_view_field_access_scope_enum.count( v_Access_Scope ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Access_Scope,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Access_Scope ) ) ) ) );

   if( !g_view_change_restrict_enum.count( v_Change_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Change_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Change_Restriction ) ) ) ) );

   if( !g_view_field_change_scope_enum.count( v_Change_Scope ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Change_Scope,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Change_Scope ) ) ) ) );

   if( !g_field_extra_enum.count( v_Extra ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Extra,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Extra ) ) ) ) );

   if( !g_primitive_enum.count( v_Primitive ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Primitive,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Primitive ) ) ) ) );

   if( !g_uom_enum.count( v_UOM ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_UOM,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_UOM ) ) ) ) );

   // [(start check_cond_non_null)] 600045
   if( get_obj( ).UOM( ) == 999 && get_obj( ).Dummy_1( ) && is_null( get_obj( ).UOM_Name( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_UOM_Name, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_UOM_Name ) ) ) ) );
   // [(finish check_cond_non_null)] 600045

   // [<start validate>]
//nyi
   if( get_obj( ).get_is_updating( ) && get_obj( ).has_field_changed( c_field_id_Type ) )
   {
      bool okay = true;

      int original_primitive
       = atoi( get_obj( ).get_original_field_value( get_obj( ).get_field_num( "Primitive" ) ).c_str( ) );

      if( original_primitive != get_obj( ).Type( ).Primitive( ) )
      {
         switch( get_obj( ).Type( ).Primitive( ) )
         {
            case 0: // i.e. string (NOTE: Any primitive is allowed to become a string.)
            break;

            case 1: // i.e. datetime
            if( original_primitive != 2 )
               okay = false;
            break;

            case 2: // i.e. date
            okay = false;
            break;

            case 3: // i.e. time
            okay = false;
            break;

            case 4: // i.e. numeric
            if( original_primitive < 4 )
               okay = false;
            break;

            case 5: // i.e. int
            if( original_primitive < 5 )
               okay = false;
            break;

            case 6: // i.e. bool
            okay = false;
            break;

            default:
            throw runtime_error( "unexpected primitive value " + to_string( get_obj( ).Type( ).Primitive( ) ) + " in Meta_Field::validate" );
         }
      }

      if( !okay )
         p_validation_errors->insert( validation_error_value_type( c_field_name_Type,
          get_string_message( GS( c_str_field_and_original_mismatch ), make_pair(
          c_str_field_and_original_mismatch_pfield, get_module_string( c_field_display_name_Type ) ),
          make_pair( c_str_field_and_original_mismatch_field, get_module_string( c_field_display_name_Primitive ) ) ) ) );
   }
   // [<finish validate>]
}

void Meta_Field::impl::validate_set_fields(
 set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( !is_null( v_Default )
    && ( fields_set.count( c_field_id_Default ) || fields_set.count( c_field_name_Default ) )
    && !g_Default_domain.is_valid( v_Default, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Default,
       get_module_string( c_field_display_name_Default ) + " " + error_message ) );

   if( !is_null( v_Id )
    && ( fields_set.count( c_field_id_Id ) || fields_set.count( c_field_name_Id ) )
    && !g_Id_domain.is_valid( v_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Id,
       get_module_string( c_field_display_name_Id ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( fields_set.count( c_field_id_Name ) || fields_set.count( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   if( !is_null( v_Parent_Class_Name )
    && ( fields_set.count( c_field_id_Parent_Class_Name ) || fields_set.count( c_field_name_Parent_Class_Name ) )
    && !g_Parent_Class_Name_domain.is_valid( v_Parent_Class_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Parent_Class_Name,
       get_module_string( c_field_display_name_Parent_Class_Name ) + " " + error_message ) );

   if( !is_null( v_UOM_Name )
    && ( fields_set.count( c_field_id_UOM_Name ) || fields_set.count( c_field_name_UOM_Name ) )
    && !g_UOM_Name_domain.is_valid( v_UOM_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_UOM_Name,
       get_module_string( c_field_display_name_UOM_Name ) + " " + error_message ) );

   if( !is_null( v_UOM_Symbol )
    && ( fields_set.count( c_field_id_UOM_Symbol ) || fields_set.count( c_field_name_UOM_Symbol ) )
    && !g_UOM_Symbol_domain.is_valid( v_UOM_Symbol, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_UOM_Symbol,
       get_module_string( c_field_display_name_UOM_Symbol ) + " " + error_message ) );
}

void Meta_Field::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Access_Permission )
      p_obj->setup_foreign_key( *cp_Access_Permission, v_Access_Permission );

   if( cp_Change_Permission )
      p_obj->setup_foreign_key( *cp_Change_Permission, v_Change_Permission );

   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   if( cp_Enum )
      p_obj->setup_foreign_key( *cp_Enum, v_Enum );

   if( cp_Enum_Filter )
      p_obj->setup_foreign_key( *cp_Enum_Filter, v_Enum_Filter );

   if( cp_Parent_Class )
      p_obj->setup_foreign_key( *cp_Parent_Class, v_Parent_Class );

   if( cp_Source_Field )
      p_obj->setup_foreign_key( *cp_Source_Field, v_Source_Field );

   if( cp_Type )
      p_obj->setup_foreign_key( *cp_Type, v_Type );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start transient_field_alias)] 600053b
   if( get_obj( ).needs_field_value( "Parent_Class_Name" )
    || required_transients.count( "Parent_Class_Name" ) )
      get_obj( ).Parent_Class_Name( get_obj( ).Parent_Class( ).Name( ) );
   // [(finish transient_field_alias)] 600053b

   // [(start transient_field_alias)] 600054a
   if( get_obj( ).needs_field_value( "Def_Value" )
    || required_transients.count( "Def_Value" ) )
      get_obj( ).Def_Value( get_obj( ).Default( ) );
   // [(finish transient_field_alias)] 600054a

   // [(start transient_field_alias)] 640065
   if( get_obj( ).needs_field_value( "Numeric_Decimals" )
    || required_transients.count( "Numeric_Decimals" ) )
      get_obj( ).Numeric_Decimals( get_obj( ).Type( ).Numeric_Decimals( ) );
   // [(finish transient_field_alias)] 640065

   // [<start after_fetch>]
//nyi
   if( get_obj( ).Def_Value( ).size( ) > 5 )
   {
      string str( get_obj( ).Def_Value( ) );

      size_t truncate_at = 5;
      for( size_t i = 0; i < 5; i++ )
      {
         if( str[ i ] == ' ' || str[ i ] == '_'
          || str[ i ] == '-' || str[ i ] == '+' || str[ i ] == '/' || str[ i ] == '|' )
         {
            truncate_at = i;
            break;
         }
      }

      str.erase( truncate_at );
      str += "...";

      get_obj( ).Def_Value( str );
   }

   if( get_obj( ).Primitive( ) == 0 )
      get_obj( ).Extra_Filter( "str" );
   else if( get_obj( ).Primitive( ) == 1 )
      get_obj( ).Extra_Filter( "dtm" );
   else if( get_obj( ).Primitive( ) == 2 )
      get_obj( ).Extra_Filter( "dte" );
   else if( get_obj( ).Primitive( ) == 3 )
      get_obj( ).Extra_Filter( "tme" );
   else if( get_obj( ).Primitive( ) == 4 )
      get_obj( ).Extra_Filter( "num" );
   else if( get_obj( ).Primitive( ) == 5 )
      get_obj( ).Extra_Filter( "int" );
   else if( get_obj( ).Primitive( ) == 6 )
      get_obj( ).Extra_Filter( "bln" );
   // [<finish after_fetch>]
}

void Meta_Field::impl::finalise_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Field::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Field::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_Field::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start default_to_global)] 600046a
   if( is_create
    && get_obj( ).get_key( ).empty( )
    && get_obj( ).get_clone_key( ).empty( )
    && get_obj( ).Create_View_Field( ) == g_default_Create_View_Field )
      get_obj( ).Create_View_Field( 1 );
   // [(finish default_to_global)] 600046a

   // [(start default_to_global)] 600046b
   if( is_create
    && get_obj( ).get_key( ).empty( )
    && get_obj( ).get_clone_key( ).empty( )
    && get_obj( ).Create_List_Field( ) == g_default_Create_List_Field )
      get_obj( ).Create_List_Field( 1 );
   // [(finish default_to_global)] 600046b

   // [(start field_from_other_field)] 600053
   get_obj( ).Primitive( get_obj( ).Type( ).Primitive( ) );
   // [(finish field_from_other_field)] 600053

   // [(start default_to_field)] 600053a
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).UOM( ) == g_default_UOM ) )
      get_obj( ).UOM( get_obj( ).Type( ).Default_UOM( ) );
   // [(finish default_to_field)] 600053a

   // [(start field_from_other_field)] 600054
   if( !is_null( get_obj( ).Parent_Class( ) ) )
      get_obj( ).Is_Foreign_Key( get_obj( ).Parent_Class( ).get_is_singular( ) );
   else
      get_obj( ).Is_Foreign_Key( false );
   // [(finish field_from_other_field)] 600054

   // [(start modifier_set_field)] 600062
   if( ( state & c_modifier_Is_Any_Non_Text_Type ) )
      get_obj( ).Mandatory( true );
   // [(finish modifier_set_field)] 600062

   // [(start field_empty_action)] 600063
   if( !get_obj( ).get_key( ).empty( ) )
   {
      if( !is_null( get_obj( ).Class( ).Source_Class( ) ) )
         get_obj( ).Internal( true );
   }
   // [(finish field_empty_action)] 600063

   // [(start modifier_set_field)] 610065
   if( ( state & c_modifier_Is_Any_Non_Text_Type ) )
      get_obj( ).Is_Text_Type( false );
   // [(finish modifier_set_field)] 610065

   // [<start to_store>]
//nyi
   if( !( state & c_modifier_Is_Any_Non_Text_Type ) )
      get_obj( ).Is_Text_Type( true );

   if( is_create )
   {
      if( get_obj( ).Primitive( ) == 0 )
         get_obj( ).Extra_Filter( "str" );
      else if( get_obj( ).Primitive( ) == 1 )
         get_obj( ).Extra_Filter( "dtm" );
      else if( get_obj( ).Primitive( ) == 2 )
         get_obj( ).Extra_Filter( "dte" );
      else if( get_obj( ).Primitive( ) == 3 )
         get_obj( ).Extra_Filter( "tme" );
      else if( get_obj( ).Primitive( ) == 4 )
         get_obj( ).Extra_Filter( "num" );
      else if( get_obj( ).Primitive( ) == 5 )
         get_obj( ).Extra_Filter( "int" );
      else if( get_obj( ).Primitive( ) == 6 )
         get_obj( ).Extra_Filter( "bln" );
   }
   // [<finish to_store>]
}

void Meta_Field::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start parent_auto_int_inc)] 620065
   if( is_create && is_null( get_obj( ).Id( ) ) )
   {
      if( is_null( get_obj( ).Class( ) ) )
         throw runtime_error( "unexpected empty Class" );

      get_obj( ).Class( ).op_update( get_obj( ).Class( ), FIELD_NAME( Meta, Class, Next_Field_Id ) );

      get_obj( ).Id( get_obj( ).Class( ).Next_Field_Id( ) );

      get_obj( ).Class( ).Next_Field_Id( auto_int_increment( get_obj( ).Class( ).Next_Field_Id( ) ) );
      get_obj( ).Class( ).op_apply( );
   }
   // [(finish parent_auto_int_inc)] 620065

   // [<start for_store>]
//nyi
   string view_field_key( incremented_key_val( get_obj( ).get_key( ), 1 ) );
   string list_field_key( incremented_key_val( get_obj( ).get_key( ), 2 ) );

   if( is_create
    && !is_internal && get_obj( ).Create_View_Field( )
    && !is_null( get_obj( ).Class( ).Created_View( ) ) )
   {
      get_obj( ).child_View_Field_Source( ).op_create( view_field_key );
      get_obj( ).child_View_Field_Source( ).Source_Field( get_obj( ).get_key( ) );
      get_obj( ).child_View_Field_Source( ).Class( get_obj( ).Class( ) );
      get_obj( ).child_View_Field_Source( ).View( get_obj( ).Class( ).Created_View( ) );
      get_obj( ).child_View_Field_Source( ).op_apply( );
   }

   if( is_create
    && !is_internal && get_obj( ).Create_List_Field( )
    && !is_null( get_obj( ).Class( ).Created_List( ) ) )
   {
      get_obj( ).child_List_Field_Source( ).op_create( list_field_key );
      get_obj( ).child_List_Field_Source( ).Source_Field( get_obj( ).get_key( ) );
      get_obj( ).child_List_Field_Source( ).Class( get_obj( ).Class( ) );
      get_obj( ).child_List_Field_Source( ).List( get_obj( ).Class( ).Created_List( ) );
      get_obj( ).child_List_Field_Source( ).op_apply( );
   }

   if( is_create && is_internal )
   {
      string parent_field_for_view = get_session_variable( "meta_Parent_Field_For_View" );

      if( !parent_field_for_view.empty( ) )
      {
         get_obj( ).child_View_Field_Source( ).op_create( view_field_key );
         get_obj( ).child_View_Field_Source( ).Use_Source_Parent( true );
         get_obj( ).child_View_Field_Source( ).Source_Parent( get_obj( ).get_key( ) );
         get_obj( ).child_View_Field_Source( ).Source_Child( parent_field_for_view );
         get_obj( ).child_View_Field_Source( ).Class( get_obj( ).Class( ) );
         get_obj( ).child_View_Field_Source( ).View( get_obj( ).Class( ).Created_View( ) );
         get_obj( ).child_View_Field_Source( ).op_apply( );
      }

      string parent_field_for_list = get_session_variable( "meta_Parent_Field_For_List" );

      if( !parent_field_for_list.empty( ) )
      {
         get_obj( ).child_List_Field_Source( ).op_create( list_field_key );
         get_obj( ).child_List_Field_Source( ).Use_Source_Parent( true );
         get_obj( ).child_List_Field_Source( ).Source_Parent( get_obj( ).get_key( ) );
         get_obj( ).child_List_Field_Source( ).Source_Child( parent_field_for_list );
         get_obj( ).child_List_Field_Source( ).Class( get_obj( ).Class( ) );
         get_obj( ).child_List_Field_Source( ).List( get_obj( ).Class( ).Created_List( ) );
         get_obj( ).child_List_Field_Source( ).op_apply( );
      }
   }

   if( !is_create && get_obj( ).Transient( ) && get_obj( ).has_field_changed( c_field_id_Transient ) )
   {
      bool okay = true;
      if( get_obj( ).child_Index_Field_1( ).iterate_forwards( ) )
      {
         okay = false;
         get_obj( ).child_Index_Field_1( ).iterate_stop( );
      }

      if( okay && get_obj( ).child_Index_Field_2( ).iterate_forwards( ) )
      {
         okay = false;
         get_obj( ).child_Index_Field_2( ).iterate_stop( );
      }

      if( okay && get_obj( ).child_Index_Field_3( ).iterate_forwards( ) )
      {
         okay = false;
         get_obj( ).child_Index_Field_3( ).iterate_stop( );
      }

      if( okay && get_obj( ).child_Index_Field_4( ).iterate_forwards( ) )
      {
         okay = false;
         get_obj( ).child_Index_Field_4( ).iterate_stop( );
      }

      if( okay && get_obj( ).child_Index_Field_5( ).iterate_forwards( ) )
      {
         okay = false;
         get_obj( ).child_Index_Field_5( ).iterate_stop( );
      }

      if( !okay )
         throw runtime_error( "Cannot make this field transient as it is being used by one or more indexes." );
   }
   // [<finish for_store>]
}

void Meta_Field::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start update_children)] 600055
   if( !is_create
    && !get_obj( ).get_is_for_peer( )
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification( ).iterate_forwards( ) )
   {
      do
      {
         if( !is_update_or_destroy_locked_by_own_session( get_obj( ).child_Specification( ) ) )
         {
            get_obj( ).child_Specification( ).op_update( );
            get_obj( ).child_Specification( ).op_apply( );
         }
      } while( get_obj( ).child_Specification( ).iterate_next( ) );
   }
   // [(finish update_children)] 600055

   // [(start update_children)] 600055a
   if( !is_create
    && !get_obj( ).get_is_for_peer( )
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Other( ).iterate_forwards( ) )
   {
      do
      {
         if( !is_update_or_destroy_locked_by_own_session( get_obj( ).child_Specification_Other( ) ) )
         {
            get_obj( ).child_Specification_Other( ).op_update( );
            get_obj( ).child_Specification_Other( ).op_apply( );
         }
      } while( get_obj( ).child_Specification_Other( ).iterate_next( ) );
   }
   // [(finish update_children)] 600055a

   // [(start update_children)] 600055b
   if( !is_create
    && !get_obj( ).get_is_for_peer( )
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Other_Field_2( ).iterate_forwards( ) )
   {
      do
      {
         if( !is_update_or_destroy_locked_by_own_session( get_obj( ).child_Specification_Other_Field_2( ) ) )
         {
            get_obj( ).child_Specification_Other_Field_2( ).op_update( );
            get_obj( ).child_Specification_Other_Field_2( ).op_apply( );
         }
      } while( get_obj( ).child_Specification_Other_Field_2( ).iterate_next( ) );
   }
   // [(finish update_children)] 600055b

   // [(start update_children)] 600055c
   if( !is_create
    && !get_obj( ).get_is_for_peer( )
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Other_Class( ).iterate_forwards( ) )
   {
      do
      {
         if( !is_update_or_destroy_locked_by_own_session( get_obj( ).child_Specification_Other_Class( ) ) )
         {
            get_obj( ).child_Specification_Other_Class( ).op_update( );
            get_obj( ).child_Specification_Other_Class( ).op_apply( );
         }
      } while( get_obj( ).child_Specification_Other_Class( ).iterate_next( ) );
   }
   // [(finish update_children)] 600055c

   // [(start update_children)] 600056
   if( !is_create
    && !get_obj( ).get_is_for_peer( )
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Source( ).iterate_forwards( ) )
   {
      do
      {
         if( !is_update_or_destroy_locked_by_own_session( get_obj( ).child_Specification_Source( ) ) )
         {
            get_obj( ).child_Specification_Source( ).op_update( );
            get_obj( ).child_Specification_Source( ).op_apply( );
         }
      } while( get_obj( ).child_Specification_Source( ).iterate_next( ) );
   }
   // [(finish update_children)] 600056

   // [(start update_children)] 600057
   if( !is_create
    && !get_obj( ).get_is_for_peer( )
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Source_Parent( ).iterate_forwards( ) )
   {
      do
      {
         if( !is_update_or_destroy_locked_by_own_session( get_obj( ).child_Specification_Source_Parent( ) ) )
         {
            get_obj( ).child_Specification_Source_Parent( ).op_update( );
            get_obj( ).child_Specification_Source_Parent( ).op_apply( );
         }
      } while( get_obj( ).child_Specification_Source_Parent( ).iterate_next( ) );
   }
   // [(finish update_children)] 600057

   // [(start update_children)] 600058
   if( !is_create
    && !get_obj( ).get_is_for_peer( )
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Source_Child( ).iterate_forwards( ) )
   {
      do
      {
         if( !is_update_or_destroy_locked_by_own_session( get_obj( ).child_Specification_Source_Child( ) ) )
         {
            get_obj( ).child_Specification_Source_Child( ).op_update( );
            get_obj( ).child_Specification_Source_Child( ).op_apply( );
         }
      } while( get_obj( ).child_Specification_Source_Child( ).iterate_next( ) );
   }
   // [(finish update_children)] 600058

   // [(start update_children)] 600059
   if( !is_create
    && !get_obj( ).get_is_for_peer( )
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Test( ).iterate_forwards( ) )
   {
      do
      {
         if( !is_update_or_destroy_locked_by_own_session( get_obj( ).child_Specification_Test( ) ) )
         {
            get_obj( ).child_Specification_Test( ).op_update( );
            get_obj( ).child_Specification_Test( ).op_apply( );
         }
      } while( get_obj( ).child_Specification_Test( ).iterate_next( ) );
   }
   // [(finish update_children)] 600059

   // [(start update_children)] 600060
   if( !is_create
    && !get_obj( ).get_is_for_peer( )
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Test_Parent( ).iterate_forwards( ) )
   {
      do
      {
         if( !is_update_or_destroy_locked_by_own_session( get_obj( ).child_Specification_Test_Parent( ) ) )
         {
            get_obj( ).child_Specification_Test_Parent( ).op_update( );
            get_obj( ).child_Specification_Test_Parent( ).op_apply( );
         }
      } while( get_obj( ).child_Specification_Test_Parent( ).iterate_next( ) );
   }
   // [(finish update_children)] 600060

   // [(start update_children)] 600061
   if( !is_create
    && !get_obj( ).get_is_for_peer( )
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Test_Child( ).iterate_forwards( ) )
   {
      do
      {
         if( !is_update_or_destroy_locked_by_own_session( get_obj( ).child_Specification_Test_Child( ) ) )
         {
            get_obj( ).child_Specification_Test_Child( ).op_update( );
            get_obj( ).child_Specification_Test_Child( ).op_apply( );
         }
      } while( get_obj( ).child_Specification_Test_Child( ).iterate_next( ) );
   }
   // [(finish update_children)] 600061

   // [(start clone_children_for_create)] 600064
   if( is_create
    && get_obj( ).get_clone_key( ).empty( )
    && get_obj( ).Class( ).child_Class_Source( ).iterate_forwards( ) )
   {
      int child_num = 0;
      do
      {
         string key_info( construct_key_from_int( get_obj( ).get_key( ), ++child_num ) );
         key_info += ' ';
         key_info += get_obj( ).get_key( );

         get_obj( ).Class( ).child_Class_Source( ).child_Field( ).op_create( key_info );
         get_obj( ).Class( ).child_Class_Source( ).child_Field( ).Class( get_obj( ).Class( ).child_Class_Source( ).get_key( ) );
         get_obj( ).Class( ).child_Class_Source( ).child_Field( ).Source_Field( get_obj( ).get_key( ) );

         get_obj( ).Class( ).child_Class_Source( ).child_Field( ).op_apply( );
      } while( get_obj( ).Class( ).child_Class_Source( ).iterate_next( ) );
   }
   // [(finish clone_children_for_create)] 600064

   // [(start clone_children_for_update)] 600065
   if( !is_create && get_obj( ).child_Field_Source( ).iterate_forwards( ) )
   {
      do
      {
         get_obj( ).child_Field_Source( ).op_update( );
         string parent( get_obj( ).child_Field_Source( ).Class( ) );
         string extra( get_obj( ).child_Field_Source( ).Parent_Class( ) );

         get_obj( ).child_Field_Source( ).copy_all_field_values( get_obj( ) );

         get_obj( ).child_Field_Source( ).Class( parent );
         get_obj( ).child_Field_Source( ).Parent_Class( extra );
         get_obj( ).child_Field_Source( ).Source_Field( get_obj( ).get_key( ) );

         get_obj( ).child_Field_Source( ).op_apply( );
      } while( get_obj( ).child_Field_Source( ).iterate_next( ) );
   }
   // [(finish clone_children_for_update)] 600065

   // [(start update_children)] 610058
   if( !is_create
    && !get_obj( ).get_is_for_peer( )
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Other_Source_Child( ).iterate_forwards( ) )
   {
      do
      {
         if( !is_update_or_destroy_locked_by_own_session( get_obj( ).child_Specification_Other_Source_Child( ) ) )
         {
            get_obj( ).child_Specification_Other_Source_Child( ).op_update( );
            get_obj( ).child_Specification_Other_Source_Child( ).op_apply( );
         }
      } while( get_obj( ).child_Specification_Other_Source_Child( ).iterate_next( ) );
   }
   // [(finish update_children)] 610058

   // [(start update_children)] 620058
   if( !is_create
    && !get_obj( ).get_is_for_peer( )
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Other_Source_Child_2( ).iterate_forwards( ) )
   {
      do
      {
         if( !is_update_or_destroy_locked_by_own_session( get_obj( ).child_Specification_Other_Source_Child_2( ) ) )
         {
            get_obj( ).child_Specification_Other_Source_Child_2( ).op_update( );
            get_obj( ).child_Specification_Other_Source_Child_2( ).op_apply( );
         }
      } while( get_obj( ).child_Specification_Other_Source_Child_2( ).iterate_next( ) );
   }
   // [(finish update_children)] 620058

   // [(start update_children)] 630058
   if( !is_create
    && !get_obj( ).get_is_for_peer( )
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Source_Grandchild( ).iterate_forwards( ) )
   {
      do
      {
         if( !is_update_or_destroy_locked_by_own_session( get_obj( ).child_Specification_Source_Grandchild( ) ) )
         {
            get_obj( ).child_Specification_Source_Grandchild( ).op_update( );
            get_obj( ).child_Specification_Source_Grandchild( ).op_apply( );
         }
      } while( get_obj( ).child_Specification_Source_Grandchild( ).iterate_next( ) );
   }
   // [(finish update_children)] 630058

   // [(start clone_children_for_create)] 630065
   if( is_create
    && get_obj( ).Class( ).child_Initial_Record( ).iterate_forwards( ) )
   {
      do
      {
         string key_info( get_obj( ).Class( ).child_Initial_Record( ).get_key( ) + "_" + get_obj( ).get_key( ) );

         get_obj( ).Class( ).child_Initial_Record( ).child_Initial_Record_Value( ).op_create( key_info );
         get_obj( ).Class( ).child_Initial_Record( ).child_Initial_Record_Value( ).Initial_Record( get_obj( ).Class( ).child_Initial_Record( ).get_key( ) );
         get_obj( ).Class( ).child_Initial_Record( ).child_Initial_Record_Value( ).Field( get_obj( ).get_key( ) );
         get_obj( ).Class( ).child_Initial_Record( ).child_Initial_Record_Value( ).Value( get_obj( ).Default( ) );
         get_obj( ).Class( ).child_Initial_Record( ).child_Initial_Record_Value( ).op_apply( );
      } while( get_obj( ).Class( ).child_Initial_Record( ).iterate_next( ) );
   }
   // [(finish clone_children_for_create)] 630065

   // [<start after_store>]
//nyi
   if( !is_create && get_obj( ).child_Initial_Record_Value( ).iterate_forwards( ) )
   {
      do
      {
         int field_num = get_obj( ).get_field_num( c_field_id_Default );

         get_obj( ).child_Initial_Record_Value( ).op_update( );

         if( get_obj( ).has_field_changed( c_field_id_Default )
          && get_obj( ).child_Initial_Record_Value( ).Value( ) == get_obj( ).get_original_field_value( field_num ) )
         {
            get_obj( ).child_Initial_Record_Value( ).Value( get_obj( ).Default( ) );
            get_obj( ).child_Initial_Record_Value( ).op_apply( );
         }
         else
            get_obj( ).child_Initial_Record_Value( ).op_cancel( );

      } while( get_obj( ).child_Initial_Record_Value( ).iterate_next( ) );
   }
   // [<finish after_store>]
}

bool Meta_Field::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Field::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Field::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Field::impl::set_default_values( )
{
   clear( );
}

bool Meta_Field::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_Field::impl::get_required_transients( ) const
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

Meta_Field::Meta_Field( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Field::~Meta_Field( )
{
   cleanup( );
   delete p_impl;
}

int Meta_Field::Access_Restriction( ) const
{
   return p_impl->impl_Access_Restriction( );
}

void Meta_Field::Access_Restriction( int Access_Restriction )
{
   p_impl->impl_Access_Restriction( Access_Restriction );
}

int Meta_Field::Access_Scope( ) const
{
   return p_impl->impl_Access_Scope( );
}

void Meta_Field::Access_Scope( int Access_Scope )
{
   p_impl->impl_Access_Scope( Access_Scope );
}

int Meta_Field::Change_Restriction( ) const
{
   return p_impl->impl_Change_Restriction( );
}

void Meta_Field::Change_Restriction( int Change_Restriction )
{
   p_impl->impl_Change_Restriction( Change_Restriction );
}

int Meta_Field::Change_Scope( ) const
{
   return p_impl->impl_Change_Scope( );
}

void Meta_Field::Change_Scope( int Change_Scope )
{
   p_impl->impl_Change_Scope( Change_Scope );
}

bool Meta_Field::Create_List_Field( ) const
{
   return p_impl->impl_Create_List_Field( );
}

void Meta_Field::Create_List_Field( bool Create_List_Field )
{
   p_impl->impl_Create_List_Field( Create_List_Field );
}

bool Meta_Field::Create_View_Field( ) const
{
   return p_impl->impl_Create_View_Field( );
}

void Meta_Field::Create_View_Field( bool Create_View_Field )
{
   p_impl->impl_Create_View_Field( Create_View_Field );
}

const string& Meta_Field::Def_Value( ) const
{
   return p_impl->impl_Def_Value( );
}

void Meta_Field::Def_Value( const string& Def_Value )
{
   p_impl->impl_Def_Value( Def_Value );
}

const string& Meta_Field::Default( ) const
{
   return p_impl->impl_Default( );
}

void Meta_Field::Default( const string& Default )
{
   p_impl->impl_Default( Default );
}

bool Meta_Field::Dummy_1( ) const
{
   return p_impl->impl_Dummy_1( );
}

void Meta_Field::Dummy_1( bool Dummy_1 )
{
   p_impl->impl_Dummy_1( Dummy_1 );
}

bool Meta_Field::Encrypted( ) const
{
   return p_impl->impl_Encrypted( );
}

void Meta_Field::Encrypted( bool Encrypted )
{
   p_impl->impl_Encrypted( Encrypted );
}

int Meta_Field::Extra( ) const
{
   return p_impl->impl_Extra( );
}

void Meta_Field::Extra( int Extra )
{
   p_impl->impl_Extra( Extra );
}

const string& Meta_Field::Extra_Filter( ) const
{
   return p_impl->impl_Extra_Filter( );
}

void Meta_Field::Extra_Filter( const string& Extra_Filter )
{
   p_impl->impl_Extra_Filter( Extra_Filter );
}

const string& Meta_Field::Id( ) const
{
   return p_impl->impl_Id( );
}

void Meta_Field::Id( const string& Id )
{
   p_impl->impl_Id( Id );
}

bool Meta_Field::Internal( ) const
{
   return p_impl->impl_Internal( );
}

void Meta_Field::Internal( bool Internal )
{
   p_impl->impl_Internal( Internal );
}

bool Meta_Field::Is_Foreign_Key( ) const
{
   return p_impl->impl_Is_Foreign_Key( );
}

void Meta_Field::Is_Foreign_Key( bool Is_Foreign_Key )
{
   p_impl->impl_Is_Foreign_Key( Is_Foreign_Key );
}

bool Meta_Field::Is_Text_Type( ) const
{
   return p_impl->impl_Is_Text_Type( );
}

void Meta_Field::Is_Text_Type( bool Is_Text_Type )
{
   p_impl->impl_Is_Text_Type( Is_Text_Type );
}

bool Meta_Field::Mandatory( ) const
{
   return p_impl->impl_Mandatory( );
}

void Meta_Field::Mandatory( bool Mandatory )
{
   p_impl->impl_Mandatory( Mandatory );
}

const string& Meta_Field::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_Field::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

const numeric& Meta_Field::Numeric_Decimals( ) const
{
   return p_impl->impl_Numeric_Decimals( );
}

void Meta_Field::Numeric_Decimals( const numeric& Numeric_Decimals )
{
   p_impl->impl_Numeric_Decimals( Numeric_Decimals );
}

const string& Meta_Field::Parent_Class_Name( ) const
{
   return p_impl->impl_Parent_Class_Name( );
}

void Meta_Field::Parent_Class_Name( const string& Parent_Class_Name )
{
   p_impl->impl_Parent_Class_Name( Parent_Class_Name );
}

int Meta_Field::Primitive( ) const
{
   return p_impl->impl_Primitive( );
}

void Meta_Field::Primitive( int Primitive )
{
   p_impl->impl_Primitive( Primitive );
}

bool Meta_Field::Transient( ) const
{
   return p_impl->impl_Transient( );
}

void Meta_Field::Transient( bool Transient )
{
   p_impl->impl_Transient( Transient );
}

int Meta_Field::UOM( ) const
{
   return p_impl->impl_UOM( );
}

void Meta_Field::UOM( int UOM )
{
   p_impl->impl_UOM( UOM );
}

const string& Meta_Field::UOM_Name( ) const
{
   return p_impl->impl_UOM_Name( );
}

void Meta_Field::UOM_Name( const string& UOM_Name )
{
   p_impl->impl_UOM_Name( UOM_Name );
}

const string& Meta_Field::UOM_Symbol( ) const
{
   return p_impl->impl_UOM_Symbol( );
}

void Meta_Field::UOM_Symbol( const string& UOM_Symbol )
{
   p_impl->impl_UOM_Symbol( UOM_Symbol );
}

bool Meta_Field::Use_In_Text_Search( ) const
{
   return p_impl->impl_Use_In_Text_Search( );
}

void Meta_Field::Use_In_Text_Search( bool Use_In_Text_Search )
{
   p_impl->impl_Use_In_Text_Search( Use_In_Text_Search );
}

Meta_Permission& Meta_Field::Access_Permission( )
{
   return p_impl->impl_Access_Permission( );
}

const Meta_Permission& Meta_Field::Access_Permission( ) const
{
   return p_impl->impl_Access_Permission( );
}

void Meta_Field::Access_Permission( const string& key )
{
   p_impl->impl_Access_Permission( key );
}

Meta_Permission& Meta_Field::Change_Permission( )
{
   return p_impl->impl_Change_Permission( );
}

const Meta_Permission& Meta_Field::Change_Permission( ) const
{
   return p_impl->impl_Change_Permission( );
}

void Meta_Field::Change_Permission( const string& key )
{
   p_impl->impl_Change_Permission( key );
}

Meta_Class& Meta_Field::Class( )
{
   return p_impl->impl_Class( );
}

const Meta_Class& Meta_Field::Class( ) const
{
   return p_impl->impl_Class( );
}

void Meta_Field::Class( const string& key )
{
   p_impl->impl_Class( key );
}

Meta_Enum& Meta_Field::Enum( )
{
   return p_impl->impl_Enum( );
}

const Meta_Enum& Meta_Field::Enum( ) const
{
   return p_impl->impl_Enum( );
}

void Meta_Field::Enum( const string& key )
{
   p_impl->impl_Enum( key );
}

Meta_Enum& Meta_Field::Enum_Filter( )
{
   return p_impl->impl_Enum_Filter( );
}

const Meta_Enum& Meta_Field::Enum_Filter( ) const
{
   return p_impl->impl_Enum_Filter( );
}

void Meta_Field::Enum_Filter( const string& key )
{
   p_impl->impl_Enum_Filter( key );
}

Meta_Class& Meta_Field::Parent_Class( )
{
   return p_impl->impl_Parent_Class( );
}

const Meta_Class& Meta_Field::Parent_Class( ) const
{
   return p_impl->impl_Parent_Class( );
}

void Meta_Field::Parent_Class( const string& key )
{
   p_impl->impl_Parent_Class( key );
}

Meta_Field& Meta_Field::Source_Field( )
{
   return p_impl->impl_Source_Field( );
}

const Meta_Field& Meta_Field::Source_Field( ) const
{
   return p_impl->impl_Source_Field( );
}

void Meta_Field::Source_Field( const string& key )
{
   p_impl->impl_Source_Field( key );
}

Meta_Type& Meta_Field::Type( )
{
   return p_impl->impl_Type( );
}

const Meta_Type& Meta_Field::Type( ) const
{
   return p_impl->impl_Type( );
}

void Meta_Field::Type( const string& key )
{
   p_impl->impl_Type( key );
}

Meta_List_Field& Meta_Field::child_List_Field_Child_Rel_Source_Child( )
{
   return p_impl->impl_child_List_Field_Child_Rel_Source_Child( );
}

const Meta_List_Field& Meta_Field::child_List_Field_Child_Rel_Source_Child( ) const
{
   return p_impl->impl_child_List_Field_Child_Rel_Source_Child( );
}

Meta_List_Field& Meta_Field::child_List_Field_Child_Rel_Source( )
{
   return p_impl->impl_child_List_Field_Child_Rel_Source( );
}

const Meta_List_Field& Meta_Field::child_List_Field_Child_Rel_Source( ) const
{
   return p_impl->impl_child_List_Field_Child_Rel_Source( );
}

Meta_List_Field& Meta_Field::child_List_Field_Child_Rel_Source_Parent( )
{
   return p_impl->impl_child_List_Field_Child_Rel_Source_Parent( );
}

const Meta_List_Field& Meta_Field::child_List_Field_Child_Rel_Source_Parent( ) const
{
   return p_impl->impl_child_List_Field_Child_Rel_Source_Parent( );
}

Meta_Index& Meta_Field::child_Index_Field_1( )
{
   return p_impl->impl_child_Index_Field_1( );
}

const Meta_Index& Meta_Field::child_Index_Field_1( ) const
{
   return p_impl->impl_child_Index_Field_1( );
}

Meta_Index& Meta_Field::child_Index_Field_2( )
{
   return p_impl->impl_child_Index_Field_2( );
}

const Meta_Index& Meta_Field::child_Index_Field_2( ) const
{
   return p_impl->impl_child_Index_Field_2( );
}

Meta_Index& Meta_Field::child_Index_Field_3( )
{
   return p_impl->impl_child_Index_Field_3( );
}

const Meta_Index& Meta_Field::child_Index_Field_3( ) const
{
   return p_impl->impl_child_Index_Field_3( );
}

Meta_Index& Meta_Field::child_Index_Field_4( )
{
   return p_impl->impl_child_Index_Field_4( );
}

const Meta_Index& Meta_Field::child_Index_Field_4( ) const
{
   return p_impl->impl_child_Index_Field_4( );
}

Meta_Index& Meta_Field::child_Index_Field_5( )
{
   return p_impl->impl_child_Index_Field_5( );
}

const Meta_Index& Meta_Field::child_Index_Field_5( ) const
{
   return p_impl->impl_child_Index_Field_5( );
}

Meta_Specification_Field_Action& Meta_Field::child_Specification_Field_Action_New_Record_FK( )
{
   return p_impl->impl_child_Specification_Field_Action_New_Record_FK( );
}

const Meta_Specification_Field_Action& Meta_Field::child_Specification_Field_Action_New_Record_FK( ) const
{
   return p_impl->impl_child_Specification_Field_Action_New_Record_FK( );
}

Meta_Specification& Meta_Field::child_Specification_Other_Class( )
{
   return p_impl->impl_child_Specification_Other_Class( );
}

const Meta_Specification& Meta_Field::child_Specification_Other_Class( ) const
{
   return p_impl->impl_child_Specification_Other_Class( );
}

Meta_Package_Option& Meta_Field::child_Package_Option_Other_Field_2( )
{
   return p_impl->impl_child_Package_Option_Other_Field_2( );
}

const Meta_Package_Option& Meta_Field::child_Package_Option_Other_Field_2( ) const
{
   return p_impl->impl_child_Package_Option_Other_Field_2( );
}

Meta_Specification& Meta_Field::child_Specification_Other_Field_2( )
{
   return p_impl->impl_child_Specification_Other_Field_2( );
}

const Meta_Specification& Meta_Field::child_Specification_Other_Field_2( ) const
{
   return p_impl->impl_child_Specification_Other_Field_2( );
}

Meta_Package_Option& Meta_Field::child_Package_Option_Other( )
{
   return p_impl->impl_child_Package_Option_Other( );
}

const Meta_Package_Option& Meta_Field::child_Package_Option_Other( ) const
{
   return p_impl->impl_child_Package_Option_Other( );
}

Meta_Specification& Meta_Field::child_Specification_Other( )
{
   return p_impl->impl_child_Specification_Other( );
}

const Meta_Specification& Meta_Field::child_Specification_Other( ) const
{
   return p_impl->impl_child_Specification_Other( );
}

Meta_Specification& Meta_Field::child_Specification_Other_Source_Child_2( )
{
   return p_impl->impl_child_Specification_Other_Source_Child_2( );
}

const Meta_Specification& Meta_Field::child_Specification_Other_Source_Child_2( ) const
{
   return p_impl->impl_child_Specification_Other_Source_Child_2( );
}

Meta_Specification& Meta_Field::child_Specification_Other_Source_Child( )
{
   return p_impl->impl_child_Specification_Other_Source_Child( );
}

const Meta_Specification& Meta_Field::child_Specification_Other_Source_Child( ) const
{
   return p_impl->impl_child_Specification_Other_Source_Child( );
}

Meta_Package_Option& Meta_Field::child_Package_Option_Other_Source( )
{
   return p_impl->impl_child_Package_Option_Other_Source( );
}

const Meta_Package_Option& Meta_Field::child_Package_Option_Other_Source( ) const
{
   return p_impl->impl_child_Package_Option_Other_Source( );
}

Meta_Relationship& Meta_Field::child_Relationship_Parent_Field_For_List( )
{
   return p_impl->impl_child_Relationship_Parent_Field_For_List( );
}

const Meta_Relationship& Meta_Field::child_Relationship_Parent_Field_For_List( ) const
{
   return p_impl->impl_child_Relationship_Parent_Field_For_List( );
}

Meta_Relationship& Meta_Field::child_Relationship_Parent_Field_For_View( )
{
   return p_impl->impl_child_Relationship_Parent_Field_For_View( );
}

const Meta_Relationship& Meta_Field::child_Relationship_Parent_Field_For_View( ) const
{
   return p_impl->impl_child_Relationship_Parent_Field_For_View( );
}

Meta_List& Meta_Field::child_List_Parent( )
{
   return p_impl->impl_child_List_Parent( );
}

const Meta_List& Meta_Field::child_List_Parent( ) const
{
   return p_impl->impl_child_List_Parent( );
}

Meta_Class& Meta_Field::child_Class_Quick_Link( )
{
   return p_impl->impl_child_Class_Quick_Link( );
}

const Meta_Class& Meta_Field::child_Class_Quick_Link( ) const
{
   return p_impl->impl_child_Class_Quick_Link( );
}

Meta_List_Field& Meta_Field::child_List_Field_Restriction( )
{
   return p_impl->impl_child_List_Field_Restriction( );
}

const Meta_List_Field& Meta_Field::child_List_Field_Restriction( ) const
{
   return p_impl->impl_child_List_Field_Restriction( );
}

Meta_List_Field& Meta_Field::child_List_Field_Source_Child( )
{
   return p_impl->impl_child_List_Field_Source_Child( );
}

const Meta_List_Field& Meta_Field::child_List_Field_Source_Child( ) const
{
   return p_impl->impl_child_List_Field_Source_Child( );
}

Meta_Specification& Meta_Field::child_Specification_Source_Child( )
{
   return p_impl->impl_child_Specification_Source_Child( );
}

const Meta_Specification& Meta_Field::child_Specification_Source_Child( ) const
{
   return p_impl->impl_child_Specification_Source_Child( );
}

Meta_View_Field& Meta_Field::child_View_Field_Source_Child( )
{
   return p_impl->impl_child_View_Field_Source_Child( );
}

const Meta_View_Field& Meta_Field::child_View_Field_Source_Child( ) const
{
   return p_impl->impl_child_View_Field_Source_Child( );
}

Meta_View_Field& Meta_Field::child_View_Field_Source_Edit_Child( )
{
   return p_impl->impl_child_View_Field_Source_Edit_Child( );
}

const Meta_View_Field& Meta_Field::child_View_Field_Source_Edit_Child( ) const
{
   return p_impl->impl_child_View_Field_Source_Edit_Child( );
}

Meta_List_Field& Meta_Field::child_List_Field_Source( )
{
   return p_impl->impl_child_List_Field_Source( );
}

const Meta_List_Field& Meta_Field::child_List_Field_Source( ) const
{
   return p_impl->impl_child_List_Field_Source( );
}

Meta_Package_Option& Meta_Field::child_Package_Option_Source( )
{
   return p_impl->impl_child_Package_Option_Source( );
}

const Meta_Package_Option& Meta_Field::child_Package_Option_Source( ) const
{
   return p_impl->impl_child_Package_Option_Source( );
}

Meta_Specification& Meta_Field::child_Specification_Source( )
{
   return p_impl->impl_child_Specification_Source( );
}

const Meta_Specification& Meta_Field::child_Specification_Source( ) const
{
   return p_impl->impl_child_Specification_Source( );
}

Meta_View_Field& Meta_Field::child_View_Field_Source( )
{
   return p_impl->impl_child_View_Field_Source( );
}

const Meta_View_Field& Meta_Field::child_View_Field_Source( ) const
{
   return p_impl->impl_child_View_Field_Source( );
}

Meta_List_Field& Meta_Field::child_List_Field_Source_Grandchild( )
{
   return p_impl->impl_child_List_Field_Source_Grandchild( );
}

const Meta_List_Field& Meta_Field::child_List_Field_Source_Grandchild( ) const
{
   return p_impl->impl_child_List_Field_Source_Grandchild( );
}

Meta_Specification& Meta_Field::child_Specification_Source_Grandchild( )
{
   return p_impl->impl_child_Specification_Source_Grandchild( );
}

const Meta_Specification& Meta_Field::child_Specification_Source_Grandchild( ) const
{
   return p_impl->impl_child_Specification_Source_Grandchild( );
}

Meta_List_Field& Meta_Field::child_List_Field_Source_Parent( )
{
   return p_impl->impl_child_List_Field_Source_Parent( );
}

const Meta_List_Field& Meta_Field::child_List_Field_Source_Parent( ) const
{
   return p_impl->impl_child_List_Field_Source_Parent( );
}

Meta_Specification& Meta_Field::child_Specification_Source_Parent( )
{
   return p_impl->impl_child_Specification_Source_Parent( );
}

const Meta_Specification& Meta_Field::child_Specification_Source_Parent( ) const
{
   return p_impl->impl_child_Specification_Source_Parent( );
}

Meta_View_Field& Meta_Field::child_View_Field_Source_Parent( )
{
   return p_impl->impl_child_View_Field_Source_Parent( );
}

const Meta_View_Field& Meta_Field::child_View_Field_Source_Parent( ) const
{
   return p_impl->impl_child_View_Field_Source_Parent( );
}

Meta_Field& Meta_Field::child_Field_Source( )
{
   return p_impl->impl_child_Field_Source( );
}

const Meta_Field& Meta_Field::child_Field_Source( ) const
{
   return p_impl->impl_child_Field_Source( );
}

Meta_Specification& Meta_Field::child_Specification_Test_Child( )
{
   return p_impl->impl_child_Specification_Test_Child( );
}

const Meta_Specification& Meta_Field::child_Specification_Test_Child( ) const
{
   return p_impl->impl_child_Specification_Test_Child( );
}

Meta_Specification& Meta_Field::child_Specification_Test( )
{
   return p_impl->impl_child_Specification_Test( );
}

const Meta_Specification& Meta_Field::child_Specification_Test( ) const
{
   return p_impl->impl_child_Specification_Test( );
}

Meta_Specification& Meta_Field::child_Specification_Test_Parent( )
{
   return p_impl->impl_child_Specification_Test_Parent( );
}

const Meta_Specification& Meta_Field::child_Specification_Test_Parent( ) const
{
   return p_impl->impl_child_Specification_Test_Parent( );
}

Meta_List_Field& Meta_Field::child_List_Field_Use_Type( )
{
   return p_impl->impl_child_List_Field_Use_Type( );
}

const Meta_List_Field& Meta_Field::child_List_Field_Use_Type( ) const
{
   return p_impl->impl_child_List_Field_Use_Type( );
}

Meta_Initial_Record_Value& Meta_Field::child_Initial_Record_Value( )
{
   return p_impl->impl_child_Initial_Record_Value( );
}

const Meta_Initial_Record_Value& Meta_Field::child_Initial_Record_Value( ) const
{
   return p_impl->impl_child_Initial_Record_Value( );
}

Meta_Modifier_Affect& Meta_Field::child_Modifier_Affect( )
{
   return p_impl->impl_child_Modifier_Affect( );
}

const Meta_Modifier_Affect& Meta_Field::child_Modifier_Affect( ) const
{
   return p_impl->impl_child_Modifier_Affect( );
}

Meta_Package_Option& Meta_Field::child_Package_Option( )
{
   return p_impl->impl_child_Package_Option( );
}

const Meta_Package_Option& Meta_Field::child_Package_Option( ) const
{
   return p_impl->impl_child_Package_Option( );
}

Meta_Specification& Meta_Field::child_Specification( )
{
   return p_impl->impl_child_Specification( );
}

const Meta_Specification& Meta_Field::child_Specification( ) const
{
   return p_impl->impl_child_Specification( );
}

void Meta_Field::Get_Text_Type( int& Result )
{
   p_impl->impl_Get_Text_Type( Result );
}

string Meta_Field::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Field::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_Field::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_Field::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_Field::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_Field::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_Field::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_Field::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_Field::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_Field::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Field::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_Field::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Field::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Field::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Field::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Field::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Field::clear( )
{
   p_impl->clear( );
}

void Meta_Field::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Field::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Field::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Field::finalise_fetch( bool skip_set_original )
{
   p_impl->finalise_fetch( skip_set_original );
}

void Meta_Field::at_create( )
{
   p_impl->at_create( );
}

void Meta_Field::post_init( )
{
   p_impl->post_init( );
}

void Meta_Field::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Field::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Field::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Field::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Field::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Field::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Field::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Field::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Field::get_field_id(
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
   else if( name == c_field_name_Class )
   {
      p_id = c_field_id_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Create_List_Field )
   {
      p_id = c_field_id_Create_List_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Create_View_Field )
   {
      p_id = c_field_id_Create_View_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Def_Value )
   {
      p_id = c_field_id_Def_Value;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Default )
   {
      p_id = c_field_id_Default;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Dummy_1 )
   {
      p_id = c_field_id_Dummy_1;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Encrypted )
   {
      p_id = c_field_id_Encrypted;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Enum )
   {
      p_id = c_field_id_Enum;

      if( p_type_name )
         *p_type_name = "Meta_Enum";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Enum_Filter )
   {
      p_id = c_field_id_Enum_Filter;

      if( p_type_name )
         *p_type_name = "Meta_Enum";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Extra )
   {
      p_id = c_field_id_Extra;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Extra_Filter )
   {
      p_id = c_field_id_Extra_Filter;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Id )
   {
      p_id = c_field_id_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Internal )
   {
      p_id = c_field_id_Internal;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Foreign_Key )
   {
      p_id = c_field_id_Is_Foreign_Key;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Text_Type )
   {
      p_id = c_field_id_Is_Text_Type;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Mandatory )
   {
      p_id = c_field_id_Mandatory;

      if( p_type_name )
         *p_type_name = "bool";

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
   else if( name == c_field_name_Numeric_Decimals )
   {
      p_id = c_field_id_Numeric_Decimals;

      if( p_type_name )
         *p_type_name = "numeric";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Parent_Class )
   {
      p_id = c_field_id_Parent_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Parent_Class_Name )
   {
      p_id = c_field_id_Parent_Class_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Primitive )
   {
      p_id = c_field_id_Primitive;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Source_Field )
   {
      p_id = c_field_id_Source_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Transient )
   {
      p_id = c_field_id_Transient;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Type )
   {
      p_id = c_field_id_Type;

      if( p_type_name )
         *p_type_name = "Meta_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_UOM )
   {
      p_id = c_field_id_UOM;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_UOM_Name )
   {
      p_id = c_field_id_UOM_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_UOM_Symbol )
   {
      p_id = c_field_id_UOM_Symbol;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Use_In_Text_Search )
   {
      p_id = c_field_id_Use_In_Text_Search;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_id;
}

const char* Meta_Field::get_field_name(
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
   else if( id == c_field_id_Class )
   {
      p_name = c_field_name_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Create_List_Field )
   {
      p_name = c_field_name_Create_List_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Create_View_Field )
   {
      p_name = c_field_name_Create_View_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Def_Value )
   {
      p_name = c_field_name_Def_Value;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Default )
   {
      p_name = c_field_name_Default;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Dummy_1 )
   {
      p_name = c_field_name_Dummy_1;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Encrypted )
   {
      p_name = c_field_name_Encrypted;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Enum )
   {
      p_name = c_field_name_Enum;

      if( p_type_name )
         *p_type_name = "Meta_Enum";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Enum_Filter )
   {
      p_name = c_field_name_Enum_Filter;

      if( p_type_name )
         *p_type_name = "Meta_Enum";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Extra )
   {
      p_name = c_field_name_Extra;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Extra_Filter )
   {
      p_name = c_field_name_Extra_Filter;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Id )
   {
      p_name = c_field_name_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Internal )
   {
      p_name = c_field_name_Internal;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Foreign_Key )
   {
      p_name = c_field_name_Is_Foreign_Key;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Text_Type )
   {
      p_name = c_field_name_Is_Text_Type;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Mandatory )
   {
      p_name = c_field_name_Mandatory;

      if( p_type_name )
         *p_type_name = "bool";

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
   else if( id == c_field_id_Numeric_Decimals )
   {
      p_name = c_field_name_Numeric_Decimals;

      if( p_type_name )
         *p_type_name = "numeric";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Parent_Class )
   {
      p_name = c_field_name_Parent_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Parent_Class_Name )
   {
      p_name = c_field_name_Parent_Class_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Primitive )
   {
      p_name = c_field_name_Primitive;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Source_Field )
   {
      p_name = c_field_name_Source_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Transient )
   {
      p_name = c_field_name_Transient;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Type )
   {
      p_name = c_field_name_Type;

      if( p_type_name )
         *p_type_name = "Meta_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_UOM )
   {
      p_name = c_field_name_UOM;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_UOM_Name )
   {
      p_name = c_field_name_UOM_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_UOM_Symbol )
   {
      p_name = c_field_name_UOM_Symbol;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Use_In_Text_Search )
   {
      p_name = c_field_name_Use_In_Text_Search;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_name;
}

string& Meta_Field::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_Field::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_Field::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_Field::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_Field::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_Field::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Field::get_field_uom_symbol( const string& id_or_name ) const
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
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
   {
      name = string( c_field_display_name_Class );
      get_module_string( c_field_display_name_Class, &next );
   }
   if( ( id_or_name == c_field_id_Create_List_Field ) || ( id_or_name == c_field_name_Create_List_Field ) )
   {
      name = string( c_field_display_name_Create_List_Field );
      get_module_string( c_field_display_name_Create_List_Field, &next );
   }
   if( ( id_or_name == c_field_id_Create_View_Field ) || ( id_or_name == c_field_name_Create_View_Field ) )
   {
      name = string( c_field_display_name_Create_View_Field );
      get_module_string( c_field_display_name_Create_View_Field, &next );
   }
   if( ( id_or_name == c_field_id_Def_Value ) || ( id_or_name == c_field_name_Def_Value ) )
   {
      name = string( c_field_display_name_Def_Value );
      get_module_string( c_field_display_name_Def_Value, &next );
   }
   if( ( id_or_name == c_field_id_Default ) || ( id_or_name == c_field_name_Default ) )
   {
      name = string( c_field_display_name_Default );
      get_module_string( c_field_display_name_Default, &next );
   }
   if( ( id_or_name == c_field_id_Dummy_1 ) || ( id_or_name == c_field_name_Dummy_1 ) )
   {
      name = string( c_field_display_name_Dummy_1 );
      get_module_string( c_field_display_name_Dummy_1, &next );
   }
   if( ( id_or_name == c_field_id_Encrypted ) || ( id_or_name == c_field_name_Encrypted ) )
   {
      name = string( c_field_display_name_Encrypted );
      get_module_string( c_field_display_name_Encrypted, &next );
   }
   if( ( id_or_name == c_field_id_Enum ) || ( id_or_name == c_field_name_Enum ) )
   {
      name = string( c_field_display_name_Enum );
      get_module_string( c_field_display_name_Enum, &next );
   }
   if( ( id_or_name == c_field_id_Enum_Filter ) || ( id_or_name == c_field_name_Enum_Filter ) )
   {
      name = string( c_field_display_name_Enum_Filter );
      get_module_string( c_field_display_name_Enum_Filter, &next );
   }
   if( ( id_or_name == c_field_id_Extra ) || ( id_or_name == c_field_name_Extra ) )
   {
      name = string( c_field_display_name_Extra );
      get_module_string( c_field_display_name_Extra, &next );
   }
   if( ( id_or_name == c_field_id_Extra_Filter ) || ( id_or_name == c_field_name_Extra_Filter ) )
   {
      name = string( c_field_display_name_Extra_Filter );
      get_module_string( c_field_display_name_Extra_Filter, &next );
   }
   if( ( id_or_name == c_field_id_Id ) || ( id_or_name == c_field_name_Id ) )
   {
      name = string( c_field_display_name_Id );
      get_module_string( c_field_display_name_Id, &next );
   }
   if( ( id_or_name == c_field_id_Internal ) || ( id_or_name == c_field_name_Internal ) )
   {
      name = string( c_field_display_name_Internal );
      get_module_string( c_field_display_name_Internal, &next );
   }
   if( ( id_or_name == c_field_id_Is_Foreign_Key ) || ( id_or_name == c_field_name_Is_Foreign_Key ) )
   {
      name = string( c_field_display_name_Is_Foreign_Key );
      get_module_string( c_field_display_name_Is_Foreign_Key, &next );
   }
   if( ( id_or_name == c_field_id_Is_Text_Type ) || ( id_or_name == c_field_name_Is_Text_Type ) )
   {
      name = string( c_field_display_name_Is_Text_Type );
      get_module_string( c_field_display_name_Is_Text_Type, &next );
   }
   if( ( id_or_name == c_field_id_Mandatory ) || ( id_or_name == c_field_name_Mandatory ) )
   {
      name = string( c_field_display_name_Mandatory );
      get_module_string( c_field_display_name_Mandatory, &next );
   }
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
   {
      name = string( c_field_display_name_Name );
      get_module_string( c_field_display_name_Name, &next );
   }
   if( ( id_or_name == c_field_id_Numeric_Decimals ) || ( id_or_name == c_field_name_Numeric_Decimals ) )
   {
      name = string( c_field_display_name_Numeric_Decimals );
      get_module_string( c_field_display_name_Numeric_Decimals, &next );
   }
   if( ( id_or_name == c_field_id_Parent_Class ) || ( id_or_name == c_field_name_Parent_Class ) )
   {
      name = string( c_field_display_name_Parent_Class );
      get_module_string( c_field_display_name_Parent_Class, &next );
   }
   if( ( id_or_name == c_field_id_Parent_Class_Name ) || ( id_or_name == c_field_name_Parent_Class_Name ) )
   {
      name = string( c_field_display_name_Parent_Class_Name );
      get_module_string( c_field_display_name_Parent_Class_Name, &next );
   }
   if( ( id_or_name == c_field_id_Primitive ) || ( id_or_name == c_field_name_Primitive ) )
   {
      name = string( c_field_display_name_Primitive );
      get_module_string( c_field_display_name_Primitive, &next );
   }
   if( ( id_or_name == c_field_id_Source_Field ) || ( id_or_name == c_field_name_Source_Field ) )
   {
      name = string( c_field_display_name_Source_Field );
      get_module_string( c_field_display_name_Source_Field, &next );
   }
   if( ( id_or_name == c_field_id_Transient ) || ( id_or_name == c_field_name_Transient ) )
   {
      name = string( c_field_display_name_Transient );
      get_module_string( c_field_display_name_Transient, &next );
   }
   if( ( id_or_name == c_field_id_Type ) || ( id_or_name == c_field_name_Type ) )
   {
      name = string( c_field_display_name_Type );
      get_module_string( c_field_display_name_Type, &next );
   }
   if( ( id_or_name == c_field_id_UOM ) || ( id_or_name == c_field_name_UOM ) )
   {
      name = string( c_field_display_name_UOM );
      get_module_string( c_field_display_name_UOM, &next );
   }
   if( ( id_or_name == c_field_id_UOM_Name ) || ( id_or_name == c_field_name_UOM_Name ) )
   {
      name = string( c_field_display_name_UOM_Name );
      get_module_string( c_field_display_name_UOM_Name, &next );
   }
   if( ( id_or_name == c_field_id_UOM_Symbol ) || ( id_or_name == c_field_name_UOM_Symbol ) )
   {
      name = string( c_field_display_name_UOM_Symbol );
      get_module_string( c_field_display_name_UOM_Symbol, &next );
   }
   if( ( id_or_name == c_field_id_Use_In_Text_Search ) || ( id_or_name == c_field_name_Use_In_Text_Search ) )
   {
      name = string( c_field_display_name_Use_In_Text_Search );
      get_module_string( c_field_display_name_Use_In_Text_Search, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_Field::get_field_display_name( const string& id_or_name ) const
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
   if( ( id_or_name == c_field_id_Change_Permission ) || ( id_or_name == c_field_name_Change_Permission ) )
      display_name = get_module_string( c_field_display_name_Change_Permission );
   if( ( id_or_name == c_field_id_Change_Restriction ) || ( id_or_name == c_field_name_Change_Restriction ) )
      display_name = get_module_string( c_field_display_name_Change_Restriction );
   if( ( id_or_name == c_field_id_Change_Scope ) || ( id_or_name == c_field_name_Change_Scope ) )
      display_name = get_module_string( c_field_display_name_Change_Scope );
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
      display_name = get_module_string( c_field_display_name_Class );
   if( ( id_or_name == c_field_id_Create_List_Field ) || ( id_or_name == c_field_name_Create_List_Field ) )
      display_name = get_module_string( c_field_display_name_Create_List_Field );
   if( ( id_or_name == c_field_id_Create_View_Field ) || ( id_or_name == c_field_name_Create_View_Field ) )
      display_name = get_module_string( c_field_display_name_Create_View_Field );
   if( ( id_or_name == c_field_id_Def_Value ) || ( id_or_name == c_field_name_Def_Value ) )
      display_name = get_module_string( c_field_display_name_Def_Value );
   if( ( id_or_name == c_field_id_Default ) || ( id_or_name == c_field_name_Default ) )
      display_name = get_module_string( c_field_display_name_Default );
   if( ( id_or_name == c_field_id_Dummy_1 ) || ( id_or_name == c_field_name_Dummy_1 ) )
      display_name = get_module_string( c_field_display_name_Dummy_1 );
   if( ( id_or_name == c_field_id_Encrypted ) || ( id_or_name == c_field_name_Encrypted ) )
      display_name = get_module_string( c_field_display_name_Encrypted );
   if( ( id_or_name == c_field_id_Enum ) || ( id_or_name == c_field_name_Enum ) )
      display_name = get_module_string( c_field_display_name_Enum );
   if( ( id_or_name == c_field_id_Enum_Filter ) || ( id_or_name == c_field_name_Enum_Filter ) )
      display_name = get_module_string( c_field_display_name_Enum_Filter );
   if( ( id_or_name == c_field_id_Extra ) || ( id_or_name == c_field_name_Extra ) )
      display_name = get_module_string( c_field_display_name_Extra );
   if( ( id_or_name == c_field_id_Extra_Filter ) || ( id_or_name == c_field_name_Extra_Filter ) )
      display_name = get_module_string( c_field_display_name_Extra_Filter );
   if( ( id_or_name == c_field_id_Id ) || ( id_or_name == c_field_name_Id ) )
      display_name = get_module_string( c_field_display_name_Id );
   if( ( id_or_name == c_field_id_Internal ) || ( id_or_name == c_field_name_Internal ) )
      display_name = get_module_string( c_field_display_name_Internal );
   if( ( id_or_name == c_field_id_Is_Foreign_Key ) || ( id_or_name == c_field_name_Is_Foreign_Key ) )
      display_name = get_module_string( c_field_display_name_Is_Foreign_Key );
   if( ( id_or_name == c_field_id_Is_Text_Type ) || ( id_or_name == c_field_name_Is_Text_Type ) )
      display_name = get_module_string( c_field_display_name_Is_Text_Type );
   if( ( id_or_name == c_field_id_Mandatory ) || ( id_or_name == c_field_name_Mandatory ) )
      display_name = get_module_string( c_field_display_name_Mandatory );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
      display_name = get_module_string( c_field_display_name_Name );
   if( ( id_or_name == c_field_id_Numeric_Decimals ) || ( id_or_name == c_field_name_Numeric_Decimals ) )
      display_name = get_module_string( c_field_display_name_Numeric_Decimals );
   if( ( id_or_name == c_field_id_Parent_Class ) || ( id_or_name == c_field_name_Parent_Class ) )
      display_name = get_module_string( c_field_display_name_Parent_Class );
   if( ( id_or_name == c_field_id_Parent_Class_Name ) || ( id_or_name == c_field_name_Parent_Class_Name ) )
      display_name = get_module_string( c_field_display_name_Parent_Class_Name );
   if( ( id_or_name == c_field_id_Primitive ) || ( id_or_name == c_field_name_Primitive ) )
      display_name = get_module_string( c_field_display_name_Primitive );
   if( ( id_or_name == c_field_id_Source_Field ) || ( id_or_name == c_field_name_Source_Field ) )
      display_name = get_module_string( c_field_display_name_Source_Field );
   if( ( id_or_name == c_field_id_Transient ) || ( id_or_name == c_field_name_Transient ) )
      display_name = get_module_string( c_field_display_name_Transient );
   if( ( id_or_name == c_field_id_Type ) || ( id_or_name == c_field_name_Type ) )
      display_name = get_module_string( c_field_display_name_Type );
   if( ( id_or_name == c_field_id_UOM ) || ( id_or_name == c_field_name_UOM ) )
      display_name = get_module_string( c_field_display_name_UOM );
   if( ( id_or_name == c_field_id_UOM_Name ) || ( id_or_name == c_field_name_UOM_Name ) )
      display_name = get_module_string( c_field_display_name_UOM_Name );
   if( ( id_or_name == c_field_id_UOM_Symbol ) || ( id_or_name == c_field_name_UOM_Symbol ) )
      display_name = get_module_string( c_field_display_name_UOM_Symbol );
   if( ( id_or_name == c_field_id_Use_In_Text_Search ) || ( id_or_name == c_field_name_Use_In_Text_Search ) )
      display_name = get_module_string( c_field_display_name_Use_In_Text_Search );

   return display_name;
}

void Meta_Field::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Field::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Field::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Field::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Field::setup_foreign_key( Meta_Permission& o, const string& value )
{
   static_cast< Meta_Permission& >( o ).set_key( value );
}

void Meta_Field::setup_foreign_key( Meta_Class& o, const string& value )
{
   static_cast< Meta_Class& >( o ).set_key( value );
}

void Meta_Field::setup_foreign_key( Meta_Enum& o, const string& value )
{
   static_cast< Meta_Enum& >( o ).set_key( value );
}

void Meta_Field::setup_foreign_key( Meta_Field& o, const string& value )
{
   static_cast< Meta_Field& >( o ).set_key( value );
}

void Meta_Field::setup_foreign_key( Meta_Type& o, const string& value )
{
   static_cast< Meta_Type& >( o ).set_key( value );
}

void Meta_Field::setup_graph_parent( Meta_List_Field& o, const string& foreign_key_field )
{
   static_cast< Meta_List_Field& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Field::setup_graph_parent( Meta_Index& o, const string& foreign_key_field )
{
   static_cast< Meta_Index& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Field::setup_graph_parent( Meta_Specification_Field_Action& o, const string& foreign_key_field )
{
   static_cast< Meta_Specification_Field_Action& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Field::setup_graph_parent( Meta_Specification& o, const string& foreign_key_field )
{
   static_cast< Meta_Specification& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Field::setup_graph_parent( Meta_Package_Option& o, const string& foreign_key_field )
{
   static_cast< Meta_Package_Option& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Field::setup_graph_parent( Meta_Relationship& o, const string& foreign_key_field )
{
   static_cast< Meta_Relationship& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Field::setup_graph_parent( Meta_List& o, const string& foreign_key_field )
{
   static_cast< Meta_List& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Field::setup_graph_parent( Meta_Class& o, const string& foreign_key_field )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Field::setup_graph_parent( Meta_View_Field& o, const string& foreign_key_field )
{
   static_cast< Meta_View_Field& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Field::setup_graph_parent( Meta_Field& o, const string& foreign_key_field )
{
   static_cast< Meta_Field& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Field::setup_graph_parent( Meta_Initial_Record_Value& o, const string& foreign_key_field )
{
   static_cast< Meta_Initial_Record_Value& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Field::setup_graph_parent( Meta_Modifier_Affect& o, const string& foreign_key_field )
{
   static_cast< Meta_Modifier_Affect& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Field::setup_graph_parent(
 Meta_Permission& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Permission& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Permission& >( o ).set_key( init_value );
}

void Meta_Field::setup_graph_parent(
 Meta_Class& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Class& >( o ).set_key( init_value );
}

void Meta_Field::setup_graph_parent(
 Meta_Enum& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Enum& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Enum& >( o ).set_key( init_value );
}

void Meta_Field::setup_graph_parent(
 Meta_Field& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Field& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Field& >( o ).set_key( init_value );
}

void Meta_Field::setup_graph_parent(
 Meta_Type& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Type& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Type& >( o ).set_key( init_value );
}

size_t Meta_Field::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Field::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Field::get_num_foreign_key_children( bool is_internal ) const
{
   size_t rc = 44;

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

class_base* Meta_Field::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   if( child_num >= 44 )
   {
      external_aliases_lookup_const_iterator ealci = g_external_aliases_lookup.lower_bound( child_num );

      if( ( ealci == g_external_aliases_lookup.end( ) ) || ( ealci->first > child_num ) )
         --ealci;

      p_class_base = ealci->second->get_next_foreign_key_child( child_num - ealci->first, next_child_field, op, true );
   }
   else
   {
      switch( child_num )
      {
         case 0:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302198";
            p_class_base = &child_List_Field_Child_Rel_Source_Child( );
         }
         break;

         case 1:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302195";
            p_class_base = &child_List_Field_Child_Rel_Source( );
         }
         break;

         case 2:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302196";
            p_class_base = &child_List_Field_Child_Rel_Source_Parent( );
         }
         break;

         case 3:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "300811";
            p_class_base = &child_Index_Field_1( );
         }
         break;

         case 4:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "300812";
            p_class_base = &child_Index_Field_2( );
         }
         break;

         case 5:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "300813";
            p_class_base = &child_Index_Field_3( );
         }
         break;

         case 6:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "300814";
            p_class_base = &child_Index_Field_4( );
         }
         break;

         case 7:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "300815";
            p_class_base = &child_Index_Field_5( );
         }
         break;

         case 8:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302310";
            p_class_base = &child_Specification_Field_Action_New_Record_FK( );
         }
         break;

         case 9:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301437";
            p_class_base = &child_Specification_Other_Class( );
         }
         break;

         case 10:
         if( op == e_cascade_op_unlink )
         {
            next_child_field = "302830e";
            p_class_base = &child_Package_Option_Other_Field_2( );
         }
         break;

         case 11:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301436";
            p_class_base = &child_Specification_Other_Field_2( );
         }
         break;

         case 12:
         if( op == e_cascade_op_unlink )
         {
            next_child_field = "302830d";
            p_class_base = &child_Package_Option_Other( );
         }
         break;

         case 13:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301435";
            p_class_base = &child_Specification_Other( );
         }
         break;

         case 14:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301497";
            p_class_base = &child_Specification_Other_Source_Child_2( );
         }
         break;

         case 15:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301496";
            p_class_base = &child_Specification_Other_Source_Child( );
         }
         break;

         case 16:
         if( op == e_cascade_op_unlink )
         {
            next_child_field = "302830g";
            p_class_base = &child_Package_Option_Other_Source( );
         }
         break;

         case 17:
         if( op == e_cascade_op_unlink )
         {
            next_child_field = "301330b";
            p_class_base = &child_Relationship_Parent_Field_For_List( );
         }
         break;

         case 18:
         if( op == e_cascade_op_unlink )
         {
            next_child_field = "301330a";
            p_class_base = &child_Relationship_Parent_Field_For_View( );
         }
         break;

         case 19:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301996";
            p_class_base = &child_List_Parent( );
         }
         break;

         case 20:
         if( op == e_cascade_op_unlink )
         {
            next_child_field = "300630";
            p_class_base = &child_Class_Quick_Link( );
         }
         break;

         case 21:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302185";
            p_class_base = &child_List_Field_Restriction( );
         }
         break;

         case 22:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302160";
            p_class_base = &child_List_Field_Source_Child( );
         }
         break;

         case 23:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301495";
            p_class_base = &child_Specification_Source_Child( );
         }
         break;

         case 24:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301960";
            p_class_base = &child_View_Field_Source_Child( );
         }
         break;

         case 25:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301965";
            p_class_base = &child_View_Field_Source_Edit_Child( );
         }
         break;

         case 26:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302140";
            p_class_base = &child_List_Field_Source( );
         }
         break;

         case 27:
         if( op == e_cascade_op_unlink )
         {
            next_child_field = "302830f";
            p_class_base = &child_Package_Option_Source( );
         }
         break;

         case 28:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301481";
            p_class_base = &child_Specification_Source( );
         }
         break;

         case 29:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301940";
            p_class_base = &child_View_Field_Source( );
         }
         break;

         case 30:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302165";
            p_class_base = &child_List_Field_Source_Grandchild( );
         }
         break;

         case 31:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301476";
            p_class_base = &child_Specification_Source_Grandchild( );
         }
         break;

         case 32:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302150";
            p_class_base = &child_List_Field_Source_Parent( );
         }
         break;

         case 33:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301480";
            p_class_base = &child_Specification_Source_Parent( );
         }
         break;

         case 34:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301950";
            p_class_base = &child_View_Field_Source_Parent( );
         }
         break;

         case 35:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "300740";
            p_class_base = &child_Field_Source( );
         }
         break;

         case 36:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301488";
            p_class_base = &child_Specification_Test_Child( );
         }
         break;

         case 37:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301485";
            p_class_base = &child_Specification_Test( );
         }
         break;

         case 38:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301486";
            p_class_base = &child_Specification_Test_Parent( );
         }
         break;

         case 39:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302176";
            p_class_base = &child_List_Field_Use_Type( );
         }
         break;

         case 40:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302520";
            p_class_base = &child_Initial_Record_Value( );
         }
         break;

         case 41:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301020";
            p_class_base = &child_Modifier_Affect( );
         }
         break;

         case 42:
         if( op == e_cascade_op_unlink )
         {
            next_child_field = "302830b";
            p_class_base = &child_Package_Option( );
         }
         break;

         case 43:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301430";
            p_class_base = &child_Specification( );
         }
         break;
      }
   }

   return p_class_base;
}

void Meta_Field::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_Field::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_Field::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_Field::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_Field::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_Field::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_Field::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_Field::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "field";

   return get_module_string( key );
}

string Meta_Field::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_Field::get_create_instance_info( ) const
{
   return "";
}

string Meta_Field::get_update_instance_info( ) const
{
   return "";
}

string Meta_Field::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_Field::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );
   else if( procedure_id == "107410" ) // i.e. Get_Text_Type
      retval = "";

   return retval;
}

bool Meta_Field::get_is_alias( ) const
{
   return false;
}

void Meta_Field::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Field::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Field::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == "_302198" ) || ( sub_context == "child_List_Field_Child_Rel_Source_Child" ) )
      p_class_base = &child_List_Field_Child_Rel_Source_Child( );
   else if( ( sub_context == "_302195" ) || ( sub_context == "child_List_Field_Child_Rel_Source" ) )
      p_class_base = &child_List_Field_Child_Rel_Source( );
   else if( ( sub_context == "_302196" ) || ( sub_context == "child_List_Field_Child_Rel_Source_Parent" ) )
      p_class_base = &child_List_Field_Child_Rel_Source_Parent( );
   else if( ( sub_context == "_300811" ) || ( sub_context == "child_Index_Field_1" ) )
      p_class_base = &child_Index_Field_1( );
   else if( ( sub_context == "_300812" ) || ( sub_context == "child_Index_Field_2" ) )
      p_class_base = &child_Index_Field_2( );
   else if( ( sub_context == "_300813" ) || ( sub_context == "child_Index_Field_3" ) )
      p_class_base = &child_Index_Field_3( );
   else if( ( sub_context == "_300814" ) || ( sub_context == "child_Index_Field_4" ) )
      p_class_base = &child_Index_Field_4( );
   else if( ( sub_context == "_300815" ) || ( sub_context == "child_Index_Field_5" ) )
      p_class_base = &child_Index_Field_5( );
   else if( ( sub_context == "_302310" ) || ( sub_context == "child_Specification_Field_Action_New_Record_FK" ) )
      p_class_base = &child_Specification_Field_Action_New_Record_FK( );
   else if( ( sub_context == "_301437" ) || ( sub_context == "child_Specification_Other_Class" ) )
      p_class_base = &child_Specification_Other_Class( );
   else if( ( sub_context == "_302830e" ) || ( sub_context == "child_Package_Option_Other_Field_2" ) )
      p_class_base = &child_Package_Option_Other_Field_2( );
   else if( ( sub_context == "_301436" ) || ( sub_context == "child_Specification_Other_Field_2" ) )
      p_class_base = &child_Specification_Other_Field_2( );
   else if( ( sub_context == "_302830d" ) || ( sub_context == "child_Package_Option_Other" ) )
      p_class_base = &child_Package_Option_Other( );
   else if( ( sub_context == "_301435" ) || ( sub_context == "child_Specification_Other" ) )
      p_class_base = &child_Specification_Other( );
   else if( ( sub_context == "_301497" ) || ( sub_context == "child_Specification_Other_Source_Child_2" ) )
      p_class_base = &child_Specification_Other_Source_Child_2( );
   else if( ( sub_context == "_301496" ) || ( sub_context == "child_Specification_Other_Source_Child" ) )
      p_class_base = &child_Specification_Other_Source_Child( );
   else if( ( sub_context == "_302830g" ) || ( sub_context == "child_Package_Option_Other_Source" ) )
      p_class_base = &child_Package_Option_Other_Source( );
   else if( ( sub_context == "_301330b" ) || ( sub_context == "child_Relationship_Parent_Field_For_List" ) )
      p_class_base = &child_Relationship_Parent_Field_For_List( );
   else if( ( sub_context == "_301330a" ) || ( sub_context == "child_Relationship_Parent_Field_For_View" ) )
      p_class_base = &child_Relationship_Parent_Field_For_View( );
   else if( ( sub_context == "_301996" ) || ( sub_context == "child_List_Parent" ) )
      p_class_base = &child_List_Parent( );
   else if( ( sub_context == "_300630" ) || ( sub_context == "child_Class_Quick_Link" ) )
      p_class_base = &child_Class_Quick_Link( );
   else if( ( sub_context == "_302185" ) || ( sub_context == "child_List_Field_Restriction" ) )
      p_class_base = &child_List_Field_Restriction( );
   else if( ( sub_context == "_302160" ) || ( sub_context == "child_List_Field_Source_Child" ) )
      p_class_base = &child_List_Field_Source_Child( );
   else if( ( sub_context == "_301495" ) || ( sub_context == "child_Specification_Source_Child" ) )
      p_class_base = &child_Specification_Source_Child( );
   else if( ( sub_context == "_301960" ) || ( sub_context == "child_View_Field_Source_Child" ) )
      p_class_base = &child_View_Field_Source_Child( );
   else if( ( sub_context == "_301965" ) || ( sub_context == "child_View_Field_Source_Edit_Child" ) )
      p_class_base = &child_View_Field_Source_Edit_Child( );
   else if( ( sub_context == "_302140" ) || ( sub_context == "child_List_Field_Source" ) )
      p_class_base = &child_List_Field_Source( );
   else if( ( sub_context == "_302830f" ) || ( sub_context == "child_Package_Option_Source" ) )
      p_class_base = &child_Package_Option_Source( );
   else if( ( sub_context == "_301481" ) || ( sub_context == "child_Specification_Source" ) )
      p_class_base = &child_Specification_Source( );
   else if( ( sub_context == "_301940" ) || ( sub_context == "child_View_Field_Source" ) )
      p_class_base = &child_View_Field_Source( );
   else if( ( sub_context == "_302165" ) || ( sub_context == "child_List_Field_Source_Grandchild" ) )
      p_class_base = &child_List_Field_Source_Grandchild( );
   else if( ( sub_context == "_301476" ) || ( sub_context == "child_Specification_Source_Grandchild" ) )
      p_class_base = &child_Specification_Source_Grandchild( );
   else if( ( sub_context == "_302150" ) || ( sub_context == "child_List_Field_Source_Parent" ) )
      p_class_base = &child_List_Field_Source_Parent( );
   else if( ( sub_context == "_301480" ) || ( sub_context == "child_Specification_Source_Parent" ) )
      p_class_base = &child_Specification_Source_Parent( );
   else if( ( sub_context == "_301950" ) || ( sub_context == "child_View_Field_Source_Parent" ) )
      p_class_base = &child_View_Field_Source_Parent( );
   else if( ( sub_context == "_300740" ) || ( sub_context == "child_Field_Source" ) )
      p_class_base = &child_Field_Source( );
   else if( ( sub_context == "_301488" ) || ( sub_context == "child_Specification_Test_Child" ) )
      p_class_base = &child_Specification_Test_Child( );
   else if( ( sub_context == "_301485" ) || ( sub_context == "child_Specification_Test" ) )
      p_class_base = &child_Specification_Test( );
   else if( ( sub_context == "_301486" ) || ( sub_context == "child_Specification_Test_Parent" ) )
      p_class_base = &child_Specification_Test_Parent( );
   else if( ( sub_context == "_302176" ) || ( sub_context == "child_List_Field_Use_Type" ) )
      p_class_base = &child_List_Field_Use_Type( );
   else if( ( sub_context == "_302520" ) || ( sub_context == "child_Initial_Record_Value" ) )
      p_class_base = &child_Initial_Record_Value( );
   else if( ( sub_context == "_301020" ) || ( sub_context == "child_Modifier_Affect" ) )
      p_class_base = &child_Modifier_Affect( );
   else if( ( sub_context == "_302830b" ) || ( sub_context == "child_Package_Option" ) )
      p_class_base = &child_Package_Option( );
   else if( ( sub_context == "_301430" ) || ( sub_context == "child_Specification" ) )
      p_class_base = &child_Specification( );
   else if( ( sub_context == c_field_id_Access_Permission ) || ( sub_context == c_field_name_Access_Permission ) )
      p_class_base = &Access_Permission( );
   else if( ( sub_context == c_field_id_Change_Permission ) || ( sub_context == c_field_name_Change_Permission ) )
      p_class_base = &Change_Permission( );
   else if( ( sub_context == c_field_id_Class ) || ( sub_context == c_field_name_Class ) )
      p_class_base = &Class( );
   else if( ( sub_context == c_field_id_Enum ) || ( sub_context == c_field_name_Enum ) )
      p_class_base = &Enum( );
   else if( ( sub_context == c_field_id_Enum_Filter ) || ( sub_context == c_field_name_Enum_Filter ) )
      p_class_base = &Enum_Filter( );
   else if( ( sub_context == c_field_id_Parent_Class ) || ( sub_context == c_field_name_Parent_Class ) )
      p_class_base = &Parent_Class( );
   else if( ( sub_context == c_field_id_Source_Field ) || ( sub_context == c_field_name_Source_Field ) )
      p_class_base = &Source_Field( );
   else if( ( sub_context == c_field_id_Type ) || ( sub_context == c_field_name_Type ) )
      p_class_base = &Type( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Field::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Access_Permission" );
   names.push_back( "C_Access_Restriction" );
   names.push_back( "C_Access_Scope" );
   names.push_back( "C_Change_Permission" );
   names.push_back( "C_Change_Restriction" );
   names.push_back( "C_Change_Scope" );
   names.push_back( "C_Class" );
   names.push_back( "C_Default" );
   names.push_back( "C_Encrypted" );
   names.push_back( "C_Enum" );
   names.push_back( "C_Enum_Filter" );
   names.push_back( "C_Extra" );
   names.push_back( "C_Id" );
   names.push_back( "C_Internal" );
   names.push_back( "C_Is_Foreign_Key" );
   names.push_back( "C_Is_Text_Type" );
   names.push_back( "C_Mandatory" );
   names.push_back( "C_Name" );
   names.push_back( "C_Parent_Class" );
   names.push_back( "C_Primitive" );
   names.push_back( "C_Source_Field" );
   names.push_back( "C_Transient" );
   names.push_back( "C_Type" );
   names.push_back( "C_UOM" );
   names.push_back( "C_UOM_Name" );
   names.push_back( "C_UOM_Symbol" );
   names.push_back( "C_Use_In_Text_Search" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Field::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Access_Permission( ) ) ) );
   values.push_back( to_string( Access_Restriction( ) ) );
   values.push_back( to_string( Access_Scope( ) ) );
   values.push_back( sql_quote( to_string( Change_Permission( ) ) ) );
   values.push_back( to_string( Change_Restriction( ) ) );
   values.push_back( to_string( Change_Scope( ) ) );
   values.push_back( sql_quote( to_string( Class( ) ) ) );
   values.push_back( sql_quote( to_string( Default( ) ) ) );
   values.push_back( to_string( Encrypted( ) ) );
   values.push_back( sql_quote( to_string( Enum( ) ) ) );
   values.push_back( sql_quote( to_string( Enum_Filter( ) ) ) );
   values.push_back( to_string( Extra( ) ) );
   values.push_back( sql_quote( to_string( Id( ) ) ) );
   values.push_back( to_string( Internal( ) ) );
   values.push_back( to_string( Is_Foreign_Key( ) ) );
   values.push_back( to_string( Is_Text_Type( ) ) );
   values.push_back( to_string( Mandatory( ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( sql_quote( to_string( Parent_Class( ) ) ) );
   values.push_back( to_string( Primitive( ) ) );
   values.push_back( sql_quote( to_string( Source_Field( ) ) ) );
   values.push_back( to_string( Transient( ) ) );
   values.push_back( sql_quote( to_string( Type( ) ) ) );
   values.push_back( to_string( UOM( ) ) );
   values.push_back( sql_quote( to_string( UOM_Name( ) ) ) );
   values.push_back( sql_quote( to_string( UOM_Symbol( ) ) ) );
   values.push_back( to_string( Use_In_Text_Search( ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Field::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [(start field_from_other_field)] 600053
   if( needs_field_value( "Primitive", dependents ) )
   {
      dependents.insert( "Type" );

      if( ( use_transients && is_field_transient( e_field_id_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
         names.insert( "Type" );
   }
   // [(finish field_from_other_field)] 600053

   // [(start transient_field_alias)] 600053b
   if( needs_field_value( "Parent_Class_Name", dependents ) )
   {
      dependents.insert( "Parent_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Parent_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Parent_Class ) ) )
         names.insert( "Parent_Class" );
   }
   // [(finish transient_field_alias)] 600053b

   // [(start field_from_other_field)] 600054
   if( needs_field_value( "Is_Foreign_Key", dependents ) )
   {
      dependents.insert( "Parent_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Parent_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Parent_Class ) ) )
         names.insert( "Parent_Class" );
   }
   // [(finish field_from_other_field)] 600054

   // [(start transient_field_alias)] 600054a
   if( needs_field_value( "Def_Value", dependents ) )
   {
      dependents.insert( "Default" );

      if( ( use_transients && is_field_transient( e_field_id_Default ) )
       || ( !use_transients && !is_field_transient( e_field_id_Default ) ) )
         names.insert( "Default" );
   }
   // [(finish transient_field_alias)] 600054a

   // [(start transient_field_alias)] 640065
   if( needs_field_value( "Numeric_Decimals", dependents ) )
   {
      dependents.insert( "Type" );

      if( ( use_transients && is_field_transient( e_field_id_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
         names.insert( "Type" );
   }
   // [(finish transient_field_alias)] 640065

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Field::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start modifier_field_value)] 600047
   dependents.insert( "UOM" ); // (for Is_Non_Custom_UOM modifier)

   if( ( use_transients && is_field_transient( e_field_id_UOM ) )
    || ( !use_transients && !is_field_transient( e_field_id_UOM ) ) )
      names.insert( "UOM" );
   // [(finish modifier_field_value)] 600047

   // [(start modifier_field_value)] 600048
   dependents.insert( "Type" ); // (for Is_Any_Non_Text_Type modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600048

   // [(start modifier_field_value)] 600049
   dependents.insert( "Type" ); // (for Is_Any_Non_Text_Type modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600049

   // [(start modifier_field_value)] 600050
   dependents.insert( "Type" ); // (for Is_Any_Non_Text_Type modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600050

   // [(start protect_equal)] 600051
   dependents.insert( "Internal" );

   if( ( use_transients && is_field_transient( e_field_id_Internal ) )
    || ( !use_transients && !is_field_transient( e_field_id_Internal ) ) )
      names.insert( "Internal" );
   // [(finish protect_equal)] 600051

   // [(start modifier_field_value)] 600052
   dependents.insert( "Internal" ); // (for Is_Internal modifier)

   if( ( use_transients && is_field_transient( e_field_id_Internal ) )
    || ( !use_transients && !is_field_transient( e_field_id_Internal ) ) )
      names.insert( "Internal" );
   // [(finish modifier_field_value)] 600052

   // [(start modifier_field_null)] 600052a
   dependents.insert( "Class" ); // (for Hide_Create_View_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Class ) )
    || ( !use_transients && !is_field_transient( e_field_id_Class ) ) )
      names.insert( "Class" );
   // [(finish modifier_field_null)] 600052a

   // [(start modifier_field_null)] 600052b
   dependents.insert( "Class" ); // (for Hide_Create_List_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Class ) )
    || ( !use_transients && !is_field_transient( e_field_id_Class ) ) )
      names.insert( "Class" );
   // [(finish modifier_field_null)] 600052b

   // [(start modifier_field_value)] 610050
   dependents.insert( "Transient" ); // (for Is_Transient modifier)

   if( ( use_transients && is_field_transient( e_field_id_Transient ) )
    || ( !use_transients && !is_field_transient( e_field_id_Transient ) ) )
      names.insert( "Transient" );
   // [(finish modifier_field_value)] 610050

   // [(start modifier_field_value)] 620050
   dependents.insert( "Type" ); // (for Is_Type_bool modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 620050

   // [(start modifier_field_value)] 630050
   dependents.insert( "Type" ); // (for Is_Type_date modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 630050

   // [(start modifier_field_value)] 640050
   dependents.insert( "Type" ); // (for Is_Type_time modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 640050

   // [(start modifier_field_value)] 650050
   dependents.insert( "Type" ); // (for Is_Type_datetime modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 650050

   // [(start modifier_field_value)] 660050
   dependents.insert( "Type" ); // (for Is_Type_string modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 660050

   // [(start modifier_field_value)] 670050
   dependents.insert( "Type" ); // (for Is_Not_Type_string modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 670050

   // [(start modifier_field_null)] 680050
   dependents.insert( "Enum" ); // (for Has_Enum modifier)

   if( ( use_transients && is_field_transient( e_field_id_Enum ) )
    || ( !use_transients && !is_field_transient( e_field_id_Enum ) ) )
      names.insert( "Enum" );
   // [(finish modifier_field_null)] 680050

   // [(start modifier_field_value)] 690050
   dependents.insert( "Encrypted" ); // (for Is_Encrypted modifier)

   if( ( use_transients && is_field_transient( e_field_id_Encrypted ) )
    || ( !use_transients && !is_field_transient( e_field_id_Encrypted ) ) )
      names.insert( "Encrypted" );
   // [(finish modifier_field_value)] 690050

   // [<start get_always_required_field_names>]
//nyi
   dependents.insert( "Primitive" );

   if( ( use_transients && is_field_transient( e_field_id_Primitive ) )
    || ( !use_transients && !is_field_transient( e_field_id_Primitive ) ) )
      names.insert( "Primitive" );
   // [<finish get_always_required_field_names>]
}

void Meta_Field::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Field::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_Field::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Field::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Field::static_lock_class_id( )
{
   return "107100";
}

const char* Meta_Field::static_check_class_name( )
{
   return "Field";
}

const char* Meta_Field::static_persistence_extra( )
{
   return "";
}

bool Meta_Field::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Field::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.107100" );
}

void Meta_Field::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "300750", "Access_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "107121", "Access_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "107123", "Access_Scope", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "300760", "Change_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "107122", "Change_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "107124", "Change_Scope", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "300700", "Class", "Meta_Class", true, "", "" ) );
   all_field_info.push_back( field_info( "107120", "Create_List_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "107119", "Create_View_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "107115", "Def_Value", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "107102", "Default", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "107118", "Dummy_1", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "107125", "Encrypted", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "300720", "Enum", "Meta_Enum", false, "", "" ) );
   all_field_info.push_back( field_info( "300725", "Enum_Filter", "Meta_Enum", false, "", "" ) );
   all_field_info.push_back( field_info( "107103", "Extra", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "107126", "Extra_Filter", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "107110", "Id", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "107106", "Internal", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "107108", "Is_Foreign_Key", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "107109", "Is_Text_Type", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "107105", "Mandatory", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "107101", "Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "107114", "Numeric_Decimals", "numeric", false, "", "" ) );
   all_field_info.push_back( field_info( "300730", "Parent_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "107113", "Parent_Class_Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "107107", "Primitive", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "300740", "Source_Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "107112", "Transient", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "300710", "Type", "Meta_Type", true, "", "" ) );
   all_field_info.push_back( field_info( "107104", "UOM", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "107116", "UOM_Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "107117", "UOM_Symbol", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "107111", "Use_In_Text_Search", "bool", false, "", "" ) );
}

void Meta_Field::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Access_Permission, make_pair( "Meta.107100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Change_Permission, make_pair( "Meta.107100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Class, make_pair( "Meta.107100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Enum, make_pair( "Meta.107100", "Meta_Enum" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Enum_Filter, make_pair( "Meta.107100", "Meta_Enum" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Parent_Class, make_pair( "Meta.107100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Field, make_pair( "Meta.107100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Type, make_pair( "Meta.107100", "Meta_Type" ) ) );
}

int Meta_Field::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Field::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_Field::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Field::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "300750";
      break;

      case 2:
      p_id = "107121";
      break;

      case 3:
      p_id = "107123";
      break;

      case 4:
      p_id = "300760";
      break;

      case 5:
      p_id = "107122";
      break;

      case 6:
      p_id = "107124";
      break;

      case 7:
      p_id = "300700";
      break;

      case 8:
      p_id = "107120";
      break;

      case 9:
      p_id = "107119";
      break;

      case 10:
      p_id = "107115";
      break;

      case 11:
      p_id = "107102";
      break;

      case 12:
      p_id = "107118";
      break;

      case 13:
      p_id = "107125";
      break;

      case 14:
      p_id = "300720";
      break;

      case 15:
      p_id = "300725";
      break;

      case 16:
      p_id = "107103";
      break;

      case 17:
      p_id = "107126";
      break;

      case 18:
      p_id = "107110";
      break;

      case 19:
      p_id = "107106";
      break;

      case 20:
      p_id = "107108";
      break;

      case 21:
      p_id = "107109";
      break;

      case 22:
      p_id = "107105";
      break;

      case 23:
      p_id = "107101";
      break;

      case 24:
      p_id = "107114";
      break;

      case 25:
      p_id = "300730";
      break;

      case 26:
      p_id = "107113";
      break;

      case 27:
      p_id = "107107";
      break;

      case 28:
      p_id = "300740";
      break;

      case 29:
      p_id = "107112";
      break;

      case 30:
      p_id = "300710";
      break;

      case 31:
      p_id = "107104";
      break;

      case 32:
      p_id = "107116";
      break;

      case 33:
      p_id = "107117";
      break;

      case 34:
      p_id = "107111";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Field" );

   return p_id;
}

const char* Meta_Field::static_get_field_name( field_id id )
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
      p_id = "Change_Permission";
      break;

      case 5:
      p_id = "Change_Restriction";
      break;

      case 6:
      p_id = "Change_Scope";
      break;

      case 7:
      p_id = "Class";
      break;

      case 8:
      p_id = "Create_List_Field";
      break;

      case 9:
      p_id = "Create_View_Field";
      break;

      case 10:
      p_id = "Def_Value";
      break;

      case 11:
      p_id = "Default";
      break;

      case 12:
      p_id = "Dummy_1";
      break;

      case 13:
      p_id = "Encrypted";
      break;

      case 14:
      p_id = "Enum";
      break;

      case 15:
      p_id = "Enum_Filter";
      break;

      case 16:
      p_id = "Extra";
      break;

      case 17:
      p_id = "Extra_Filter";
      break;

      case 18:
      p_id = "Id";
      break;

      case 19:
      p_id = "Internal";
      break;

      case 20:
      p_id = "Is_Foreign_Key";
      break;

      case 21:
      p_id = "Is_Text_Type";
      break;

      case 22:
      p_id = "Mandatory";
      break;

      case 23:
      p_id = "Name";
      break;

      case 24:
      p_id = "Numeric_Decimals";
      break;

      case 25:
      p_id = "Parent_Class";
      break;

      case 26:
      p_id = "Parent_Class_Name";
      break;

      case 27:
      p_id = "Primitive";
      break;

      case 28:
      p_id = "Source_Field";
      break;

      case 29:
      p_id = "Transient";
      break;

      case 30:
      p_id = "Type";
      break;

      case 31:
      p_id = "UOM";
      break;

      case 32:
      p_id = "UOM_Name";
      break;

      case 33:
      p_id = "UOM_Symbol";
      break;

      case 34:
      p_id = "Use_In_Text_Search";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Field" );

   return p_id;
}

int Meta_Field::static_get_field_num( const string& field )
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
   else if( field == c_field_id_Change_Permission || field == c_field_name_Change_Permission )
      rc += 4;
   else if( field == c_field_id_Change_Restriction || field == c_field_name_Change_Restriction )
      rc += 5;
   else if( field == c_field_id_Change_Scope || field == c_field_name_Change_Scope )
      rc += 6;
   else if( field == c_field_id_Class || field == c_field_name_Class )
      rc += 7;
   else if( field == c_field_id_Create_List_Field || field == c_field_name_Create_List_Field )
      rc += 8;
   else if( field == c_field_id_Create_View_Field || field == c_field_name_Create_View_Field )
      rc += 9;
   else if( field == c_field_id_Def_Value || field == c_field_name_Def_Value )
      rc += 10;
   else if( field == c_field_id_Default || field == c_field_name_Default )
      rc += 11;
   else if( field == c_field_id_Dummy_1 || field == c_field_name_Dummy_1 )
      rc += 12;
   else if( field == c_field_id_Encrypted || field == c_field_name_Encrypted )
      rc += 13;
   else if( field == c_field_id_Enum || field == c_field_name_Enum )
      rc += 14;
   else if( field == c_field_id_Enum_Filter || field == c_field_name_Enum_Filter )
      rc += 15;
   else if( field == c_field_id_Extra || field == c_field_name_Extra )
      rc += 16;
   else if( field == c_field_id_Extra_Filter || field == c_field_name_Extra_Filter )
      rc += 17;
   else if( field == c_field_id_Id || field == c_field_name_Id )
      rc += 18;
   else if( field == c_field_id_Internal || field == c_field_name_Internal )
      rc += 19;
   else if( field == c_field_id_Is_Foreign_Key || field == c_field_name_Is_Foreign_Key )
      rc += 20;
   else if( field == c_field_id_Is_Text_Type || field == c_field_name_Is_Text_Type )
      rc += 21;
   else if( field == c_field_id_Mandatory || field == c_field_name_Mandatory )
      rc += 22;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 23;
   else if( field == c_field_id_Numeric_Decimals || field == c_field_name_Numeric_Decimals )
      rc += 24;
   else if( field == c_field_id_Parent_Class || field == c_field_name_Parent_Class )
      rc += 25;
   else if( field == c_field_id_Parent_Class_Name || field == c_field_name_Parent_Class_Name )
      rc += 26;
   else if( field == c_field_id_Primitive || field == c_field_name_Primitive )
      rc += 27;
   else if( field == c_field_id_Source_Field || field == c_field_name_Source_Field )
      rc += 28;
   else if( field == c_field_id_Transient || field == c_field_name_Transient )
      rc += 29;
   else if( field == c_field_id_Type || field == c_field_name_Type )
      rc += 30;
   else if( field == c_field_id_UOM || field == c_field_name_UOM )
      rc += 31;
   else if( field == c_field_id_UOM_Name || field == c_field_name_UOM_Name )
      rc += 32;
   else if( field == c_field_id_UOM_Symbol || field == c_field_name_UOM_Symbol )
      rc += 33;
   else if( field == c_field_id_Use_In_Text_Search || field == c_field_name_Use_In_Text_Search )
      rc += 34;

   return rc - 1;
}

procedure_info_container& Meta_Field::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "107410", procedure_info( "Get_Text_Type" ) ) );
   }

   return procedures;
}

string Meta_Field::static_get_sql_columns( )
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
    "C_Change_Permission VARCHAR(75) NOT NULL,"
    "C_Change_Restriction INTEGER NOT NULL,"
    "C_Change_Scope INTEGER NOT NULL,"
    "C_Class VARCHAR(75) NOT NULL,"
    "C_Default VARCHAR(200) NOT NULL,"
    "C_Encrypted INTEGER NOT NULL,"
    "C_Enum VARCHAR(75) NOT NULL,"
    "C_Enum_Filter VARCHAR(75) NOT NULL,"
    "C_Extra INTEGER NOT NULL,"
    "C_Id VARCHAR(200) NOT NULL,"
    "C_Internal INTEGER NOT NULL,"
    "C_Is_Foreign_Key INTEGER NOT NULL,"
    "C_Is_Text_Type INTEGER NOT NULL,"
    "C_Mandatory INTEGER NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Parent_Class VARCHAR(75) NOT NULL,"
    "C_Primitive INTEGER NOT NULL,"
    "C_Source_Field VARCHAR(256) NOT NULL,"
    "C_Transient INTEGER NOT NULL,"
    "C_Type VARCHAR(256) NOT NULL,"
    "C_UOM INTEGER NOT NULL,"
    "C_UOM_Name VARCHAR(256) NOT NULL,"
    "C_UOM_Symbol VARCHAR(256) NOT NULL,"
    "C_Use_In_Text_Search INTEGER NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Field::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Field::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "enum_view_access_restrict_0", get_enum_string_view_access_restrict( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_access_restrict_1", get_enum_string_view_access_restrict( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_access_restrict_2", get_enum_string_view_access_restrict( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_access_restrict_3", get_enum_string_view_access_restrict( 3 ) ) );
   pairs.push_back( make_pair( "enum_view_access_restrict_4", get_enum_string_view_access_restrict( 4 ) ) );

   pairs.push_back( make_pair( "enum_view_field_access_scope_0", get_enum_string_view_field_access_scope( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_field_access_scope_1", get_enum_string_view_field_access_scope( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_field_access_scope_2", get_enum_string_view_field_access_scope( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_field_access_scope_3", get_enum_string_view_field_access_scope( 3 ) ) );
   pairs.push_back( make_pair( "enum_view_field_access_scope_4", get_enum_string_view_field_access_scope( 4 ) ) );
   pairs.push_back( make_pair( "enum_view_field_access_scope_5", get_enum_string_view_field_access_scope( 5 ) ) );
   pairs.push_back( make_pair( "enum_view_field_access_scope_6", get_enum_string_view_field_access_scope( 6 ) ) );
   pairs.push_back( make_pair( "enum_view_field_access_scope_7", get_enum_string_view_field_access_scope( 7 ) ) );

   pairs.push_back( make_pair( "enum_view_change_restrict_0", get_enum_string_view_change_restrict( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_change_restrict_1", get_enum_string_view_change_restrict( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_change_restrict_2", get_enum_string_view_change_restrict( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_change_restrict_3", get_enum_string_view_change_restrict( 3 ) ) );
   pairs.push_back( make_pair( "enum_view_change_restrict_4", get_enum_string_view_change_restrict( 4 ) ) );

   pairs.push_back( make_pair( "enum_view_field_change_scope_0", get_enum_string_view_field_change_scope( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_field_change_scope_1", get_enum_string_view_field_change_scope( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_field_change_scope_2", get_enum_string_view_field_change_scope( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_field_change_scope_3", get_enum_string_view_field_change_scope( 3 ) ) );

   pairs.push_back( make_pair( "enum_field_extra_0", get_enum_string_field_extra( 0 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_1", get_enum_string_field_extra( 1 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_2", get_enum_string_field_extra( 2 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_9", get_enum_string_field_extra( 9 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_23", get_enum_string_field_extra( 23 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_4", get_enum_string_field_extra( 4 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_5", get_enum_string_field_extra( 5 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_6", get_enum_string_field_extra( 6 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_3", get_enum_string_field_extra( 3 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_7", get_enum_string_field_extra( 7 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_21", get_enum_string_field_extra( 21 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_8", get_enum_string_field_extra( 8 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_31", get_enum_string_field_extra( 31 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_29", get_enum_string_field_extra( 29 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_35", get_enum_string_field_extra( 35 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_22", get_enum_string_field_extra( 22 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_25", get_enum_string_field_extra( 25 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_10", get_enum_string_field_extra( 10 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_34", get_enum_string_field_extra( 34 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_37", get_enum_string_field_extra( 37 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_11", get_enum_string_field_extra( 11 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_17", get_enum_string_field_extra( 17 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_24", get_enum_string_field_extra( 24 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_26", get_enum_string_field_extra( 26 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_13", get_enum_string_field_extra( 13 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_12", get_enum_string_field_extra( 12 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_36", get_enum_string_field_extra( 36 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_14", get_enum_string_field_extra( 14 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_18", get_enum_string_field_extra( 18 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_32", get_enum_string_field_extra( 32 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_33", get_enum_string_field_extra( 33 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_38", get_enum_string_field_extra( 38 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_27", get_enum_string_field_extra( 27 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_15", get_enum_string_field_extra( 15 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_16", get_enum_string_field_extra( 16 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_30", get_enum_string_field_extra( 30 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_28", get_enum_string_field_extra( 28 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_19", get_enum_string_field_extra( 19 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_20", get_enum_string_field_extra( 20 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_-2", get_enum_string_field_extra( -2 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_-3", get_enum_string_field_extra( -3 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_-1", get_enum_string_field_extra( -1 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_-4", get_enum_string_field_extra( -4 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_-5", get_enum_string_field_extra( -5 ) ) );

   pairs.push_back( make_pair( "enum_primitive_0", get_enum_string_primitive( 0 ) ) );
   pairs.push_back( make_pair( "enum_primitive_1", get_enum_string_primitive( 1 ) ) );
   pairs.push_back( make_pair( "enum_primitive_2", get_enum_string_primitive( 2 ) ) );
   pairs.push_back( make_pair( "enum_primitive_3", get_enum_string_primitive( 3 ) ) );
   pairs.push_back( make_pair( "enum_primitive_4", get_enum_string_primitive( 4 ) ) );
   pairs.push_back( make_pair( "enum_primitive_5", get_enum_string_primitive( 5 ) ) );
   pairs.push_back( make_pair( "enum_primitive_6", get_enum_string_primitive( 6 ) ) );

   pairs.push_back( make_pair( "enum_uom_0", get_enum_string_uom( 0 ) ) );
   pairs.push_back( make_pair( "enum_uom_1", get_enum_string_uom( 1 ) ) );
   pairs.push_back( make_pair( "enum_uom_2", get_enum_string_uom( 2 ) ) );
   pairs.push_back( make_pair( "enum_uom_3", get_enum_string_uom( 3 ) ) );
   pairs.push_back( make_pair( "enum_uom_4", get_enum_string_uom( 4 ) ) );
   pairs.push_back( make_pair( "enum_uom_5", get_enum_string_uom( 5 ) ) );
   pairs.push_back( make_pair( "enum_uom_6", get_enum_string_uom( 6 ) ) );
   pairs.push_back( make_pair( "enum_uom_7", get_enum_string_uom( 7 ) ) );
   pairs.push_back( make_pair( "enum_uom_8", get_enum_string_uom( 8 ) ) );
   pairs.push_back( make_pair( "enum_uom_900", get_enum_string_uom( 900 ) ) );
   pairs.push_back( make_pair( "enum_uom_901", get_enum_string_uom( 901 ) ) );
   pairs.push_back( make_pair( "enum_uom_902", get_enum_string_uom( 902 ) ) );
   pairs.push_back( make_pair( "enum_uom_999", get_enum_string_uom( 999 ) ) );
}

void Meta_Field::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "Class,Id", "string,string" ) );
   pairs.push_back( make_pair( "Class,Name", "string,string" ) );
   pairs.push_back( make_pair( "Enum,Name,@pk", "string,string,string" ) );
   pairs.push_back( make_pair( "Source_Field,@pk", "string,string" ) );
}

void Meta_Field::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "Class,Id" );
   unique_indexes.push_back( "Class,Name" );
}

void Meta_Field::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Class,C_Id" );
   indexes.push_back( "C_Class,C_Name" );
   indexes.push_back( "C_Enum,C_Name,C_Key_" );
   indexes.push_back( "C_Source_Field,C_Key_" );
}

void Meta_Field::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Class,C_Id" );
   indexes.push_back( "C_Class,C_Name" );
}

void Meta_Field::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Field::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Field::static_insert_external_alias( const string& module_and_class_id, Meta_Field* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Field::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Field::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   g_view_access_restrict_enum.insert( 0 );
   g_view_access_restrict_enum.insert( 1 );
   g_view_access_restrict_enum.insert( 2 );
   g_view_access_restrict_enum.insert( 3 );
   g_view_access_restrict_enum.insert( 4 );

   g_view_field_access_scope_enum.insert( 0 );
   g_view_field_access_scope_enum.insert( 1 );
   g_view_field_access_scope_enum.insert( 2 );
   g_view_field_access_scope_enum.insert( 3 );
   g_view_field_access_scope_enum.insert( 4 );
   g_view_field_access_scope_enum.insert( 5 );
   g_view_field_access_scope_enum.insert( 6 );
   g_view_field_access_scope_enum.insert( 7 );

   g_view_change_restrict_enum.insert( 0 );
   g_view_change_restrict_enum.insert( 1 );
   g_view_change_restrict_enum.insert( 2 );
   g_view_change_restrict_enum.insert( 3 );
   g_view_change_restrict_enum.insert( 4 );

   g_view_field_change_scope_enum.insert( 0 );
   g_view_field_change_scope_enum.insert( 1 );
   g_view_field_change_scope_enum.insert( 2 );
   g_view_field_change_scope_enum.insert( 3 );

   g_field_extra_enum.insert( 0 );
   g_field_extra_enum.insert( 1 );
   g_field_extra_enum.insert( 2 );
   g_field_extra_enum.insert( 9 );
   g_field_extra_enum.insert( 23 );
   g_field_extra_enum.insert( 4 );
   g_field_extra_enum.insert( 5 );
   g_field_extra_enum.insert( 6 );
   g_field_extra_enum.insert( 3 );
   g_field_extra_enum.insert( 7 );
   g_field_extra_enum.insert( 21 );
   g_field_extra_enum.insert( 8 );
   g_field_extra_enum.insert( 31 );
   g_field_extra_enum.insert( 29 );
   g_field_extra_enum.insert( 35 );
   g_field_extra_enum.insert( 22 );
   g_field_extra_enum.insert( 25 );
   g_field_extra_enum.insert( 10 );
   g_field_extra_enum.insert( 34 );
   g_field_extra_enum.insert( 37 );
   g_field_extra_enum.insert( 11 );
   g_field_extra_enum.insert( 17 );
   g_field_extra_enum.insert( 24 );
   g_field_extra_enum.insert( 26 );
   g_field_extra_enum.insert( 13 );
   g_field_extra_enum.insert( 12 );
   g_field_extra_enum.insert( 36 );
   g_field_extra_enum.insert( 14 );
   g_field_extra_enum.insert( 18 );
   g_field_extra_enum.insert( 32 );
   g_field_extra_enum.insert( 33 );
   g_field_extra_enum.insert( 38 );
   g_field_extra_enum.insert( 27 );
   g_field_extra_enum.insert( 15 );
   g_field_extra_enum.insert( 16 );
   g_field_extra_enum.insert( 30 );
   g_field_extra_enum.insert( 28 );
   g_field_extra_enum.insert( 19 );
   g_field_extra_enum.insert( 20 );
   g_field_extra_enum.insert( -2 );
   g_field_extra_enum.insert( -3 );
   g_field_extra_enum.insert( -1 );
   g_field_extra_enum.insert( -4 );
   g_field_extra_enum.insert( -5 );

   g_primitive_enum.insert( 0 );
   g_primitive_enum.insert( 1 );
   g_primitive_enum.insert( 2 );
   g_primitive_enum.insert( 3 );
   g_primitive_enum.insert( 4 );
   g_primitive_enum.insert( 5 );
   g_primitive_enum.insert( 6 );

   g_uom_enum.insert( 0 );
   g_uom_enum.insert( 1 );
   g_uom_enum.insert( 2 );
   g_uom_enum.insert( 3 );
   g_uom_enum.insert( 4 );
   g_uom_enum.insert( 5 );
   g_uom_enum.insert( 6 );
   g_uom_enum.insert( 7 );
   g_uom_enum.insert( 8 );
   g_uom_enum.insert( 900 );
   g_uom_enum.insert( 901 );
   g_uom_enum.insert( 902 );
   g_uom_enum.insert( 999 );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Field::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
