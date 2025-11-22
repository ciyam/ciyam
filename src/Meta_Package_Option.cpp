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

// [<start macros>]
// [<finish macros>]

#include "Meta_Package_Option.h"

#include "Meta_Class.h"
#include "Meta_Field.h"
#include "Meta_List.h"
#include "Meta_Model.h"
#include "Meta_Modifier.h"
#include "Meta_Package.h"
#include "Meta_Procedure.h"
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

// [<start includes>]
// [<finish includes>]

using namespace std;

// [<start namespaces>]
// [<finish namespaces>]

template< > inline string to_string( const Meta_Class& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Field& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_List& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Model& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Modifier& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Package& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Procedure& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_View& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Class& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Field& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_List& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Model& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Modifier& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Package& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Procedure& c, const string& s )
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

#include "Meta_Package_Option.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Actions = "137116";
const char* const c_field_id_Class = "302830a";
const char* const c_field_id_Date = "137111";
const char* const c_field_id_Datetime = "137110";
const char* const c_field_id_Field = "302830b";
const char* const c_field_id_Has_Field = "137119";
const char* const c_field_id_Has_List = "137128";
const char* const c_field_id_Has_Modifier = "137130";
const char* const c_field_id_Has_Other_Field = "137124";
const char* const c_field_id_Has_Other_Field_2 = "137126";
const char* const c_field_id_Has_Other_Source_Field = "137136";
const char* const c_field_id_Has_Procedure = "137132";
const char* const c_field_id_Has_Source_Field = "137134";
const char* const c_field_id_Has_View = "137122";
const char* const c_field_id_Id = "137101";
const char* const c_field_id_Installed = "137103";
const char* const c_field_id_Integer = "137114";
const char* const c_field_id_Is_Class = "137117";
const char* const c_field_id_Is_Mandatory_Class = "137118";
const char* const c_field_id_Is_Mandatory_Field = "137120";
const char* const c_field_id_Is_Mandatory_List = "137129";
const char* const c_field_id_Is_Mandatory_Modifier = "137131";
const char* const c_field_id_Is_Mandatory_Other_Field = "137125";
const char* const c_field_id_Is_Mandatory_Other_Field_2 = "137127";
const char* const c_field_id_Is_Mandatory_Other_Source_Field = "137137";
const char* const c_field_id_Is_Mandatory_Procedure = "137133";
const char* const c_field_id_Is_Mandatory_Source_Field = "137135";
const char* const c_field_id_Is_Mandatory_View = "137123";
const char* const c_field_id_Is_Other_Package = "137104";
const char* const c_field_id_List = "302831";
const char* const c_field_id_Model = "302825";
const char* const c_field_id_Modifier = "302832";
const char* const c_field_id_Name = "137115";
const char* const c_field_id_Notes = "137121";
const char* const c_field_id_Numeric = "137113";
const char* const c_field_id_Other_Field = "302830d";
const char* const c_field_id_Other_Field_2 = "302830e";
const char* const c_field_id_Other_Package = "302830";
const char* const c_field_id_Other_Package_Required = "137105";
const char* const c_field_id_Other_Package_Type = "137106";
const char* const c_field_id_Other_Source_Field = "302830g";
const char* const c_field_id_Package = "302820";
const char* const c_field_id_Primitive = "137108";
const char* const c_field_id_Procedure = "302833";
const char* const c_field_id_Source_Field = "302830f";
const char* const c_field_id_String = "137109";
const char* const c_field_id_Time = "137112";
const char* const c_field_id_Use_Option = "137102";
const char* const c_field_id_Value = "137107";
const char* const c_field_id_View = "302830c";

const char* const c_field_name_Actions = "Actions";
const char* const c_field_name_Class = "Class";
const char* const c_field_name_Date = "Date";
const char* const c_field_name_Datetime = "Datetime";
const char* const c_field_name_Field = "Field";
const char* const c_field_name_Has_Field = "Has_Field";
const char* const c_field_name_Has_List = "Has_List";
const char* const c_field_name_Has_Modifier = "Has_Modifier";
const char* const c_field_name_Has_Other_Field = "Has_Other_Field";
const char* const c_field_name_Has_Other_Field_2 = "Has_Other_Field_2";
const char* const c_field_name_Has_Other_Source_Field = "Has_Other_Source_Field";
const char* const c_field_name_Has_Procedure = "Has_Procedure";
const char* const c_field_name_Has_Source_Field = "Has_Source_Field";
const char* const c_field_name_Has_View = "Has_View";
const char* const c_field_name_Id = "Id";
const char* const c_field_name_Installed = "Installed";
const char* const c_field_name_Integer = "Integer";
const char* const c_field_name_Is_Class = "Is_Class";
const char* const c_field_name_Is_Mandatory_Class = "Is_Mandatory_Class";
const char* const c_field_name_Is_Mandatory_Field = "Is_Mandatory_Field";
const char* const c_field_name_Is_Mandatory_List = "Is_Mandatory_List";
const char* const c_field_name_Is_Mandatory_Modifier = "Is_Mandatory_Modifier";
const char* const c_field_name_Is_Mandatory_Other_Field = "Is_Mandatory_Other_Field";
const char* const c_field_name_Is_Mandatory_Other_Field_2 = "Is_Mandatory_Other_Field_2";
const char* const c_field_name_Is_Mandatory_Other_Source_Field = "Is_Mandatory_Other_Source_Field";
const char* const c_field_name_Is_Mandatory_Procedure = "Is_Mandatory_Procedure";
const char* const c_field_name_Is_Mandatory_Source_Field = "Is_Mandatory_Source_Field";
const char* const c_field_name_Is_Mandatory_View = "Is_Mandatory_View";
const char* const c_field_name_Is_Other_Package = "Is_Other_Package";
const char* const c_field_name_List = "List";
const char* const c_field_name_Model = "Model";
const char* const c_field_name_Modifier = "Modifier";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Notes = "Notes";
const char* const c_field_name_Numeric = "Numeric";
const char* const c_field_name_Other_Field = "Other_Field";
const char* const c_field_name_Other_Field_2 = "Other_Field_2";
const char* const c_field_name_Other_Package = "Other_Package";
const char* const c_field_name_Other_Package_Required = "Other_Package_Required";
const char* const c_field_name_Other_Package_Type = "Other_Package_Type";
const char* const c_field_name_Other_Source_Field = "Other_Source_Field";
const char* const c_field_name_Package = "Package";
const char* const c_field_name_Primitive = "Primitive";
const char* const c_field_name_Procedure = "Procedure";
const char* const c_field_name_Source_Field = "Source_Field";
const char* const c_field_name_String = "String";
const char* const c_field_name_Time = "Time";
const char* const c_field_name_Use_Option = "Use_Option";
const char* const c_field_name_Value = "Value";
const char* const c_field_name_View = "View";

const char* const c_field_display_name_Actions = "field_package_option_actions";
const char* const c_field_display_name_Class = "field_package_option_class";
const char* const c_field_display_name_Date = "field_package_option_date";
const char* const c_field_display_name_Datetime = "field_package_option_datetime";
const char* const c_field_display_name_Field = "field_package_option_field";
const char* const c_field_display_name_Has_Field = "field_package_option_has_field";
const char* const c_field_display_name_Has_List = "field_package_option_has_list";
const char* const c_field_display_name_Has_Modifier = "field_package_option_has_modifier";
const char* const c_field_display_name_Has_Other_Field = "field_package_option_has_other_field";
const char* const c_field_display_name_Has_Other_Field_2 = "field_package_option_has_other_field_2";
const char* const c_field_display_name_Has_Other_Source_Field = "field_package_option_has_other_source_field";
const char* const c_field_display_name_Has_Procedure = "field_package_option_has_procedure";
const char* const c_field_display_name_Has_Source_Field = "field_package_option_has_source_field";
const char* const c_field_display_name_Has_View = "field_package_option_has_view";
const char* const c_field_display_name_Id = "field_package_option_id";
const char* const c_field_display_name_Installed = "field_package_option_installed";
const char* const c_field_display_name_Integer = "field_package_option_integer";
const char* const c_field_display_name_Is_Class = "field_package_option_is_class";
const char* const c_field_display_name_Is_Mandatory_Class = "field_package_option_is_mandatory_class";
const char* const c_field_display_name_Is_Mandatory_Field = "field_package_option_is_mandatory_field";
const char* const c_field_display_name_Is_Mandatory_List = "field_package_option_is_mandatory_list";
const char* const c_field_display_name_Is_Mandatory_Modifier = "field_package_option_is_mandatory_modifier";
const char* const c_field_display_name_Is_Mandatory_Other_Field = "field_package_option_is_mandatory_other_field";
const char* const c_field_display_name_Is_Mandatory_Other_Field_2 = "field_package_option_is_mandatory_other_field_2";
const char* const c_field_display_name_Is_Mandatory_Other_Source_Field = "field_package_option_is_mandatory_other_source_field";
const char* const c_field_display_name_Is_Mandatory_Procedure = "field_package_option_is_mandatory_procedure";
const char* const c_field_display_name_Is_Mandatory_Source_Field = "field_package_option_is_mandatory_source_field";
const char* const c_field_display_name_Is_Mandatory_View = "field_package_option_is_mandatory_view";
const char* const c_field_display_name_Is_Other_Package = "field_package_option_is_other_package";
const char* const c_field_display_name_List = "field_package_option_list";
const char* const c_field_display_name_Model = "field_package_option_model";
const char* const c_field_display_name_Modifier = "field_package_option_modifier";
const char* const c_field_display_name_Name = "field_package_option_name";
const char* const c_field_display_name_Notes = "field_package_option_notes";
const char* const c_field_display_name_Numeric = "field_package_option_numeric";
const char* const c_field_display_name_Other_Field = "field_package_option_other_field";
const char* const c_field_display_name_Other_Field_2 = "field_package_option_other_field_2";
const char* const c_field_display_name_Other_Package = "field_package_option_other_package";
const char* const c_field_display_name_Other_Package_Required = "field_package_option_other_package_required";
const char* const c_field_display_name_Other_Package_Type = "field_package_option_other_package_type";
const char* const c_field_display_name_Other_Source_Field = "field_package_option_other_source_field";
const char* const c_field_display_name_Package = "field_package_option_package";
const char* const c_field_display_name_Primitive = "field_package_option_primitive";
const char* const c_field_display_name_Procedure = "field_package_option_procedure";
const char* const c_field_display_name_Source_Field = "field_package_option_source_field";
const char* const c_field_display_name_String = "field_package_option_string";
const char* const c_field_display_name_Time = "field_package_option_time";
const char* const c_field_display_name_Use_Option = "field_package_option_use_option";
const char* const c_field_display_name_Value = "field_package_option_value";
const char* const c_field_display_name_View = "field_package_option_view";

const int c_num_fields = 50;

const char* const c_all_sorted_field_ids[ ] =
{
   "137101",
   "137102",
   "137103",
   "137104",
   "137105",
   "137106",
   "137107",
   "137108",
   "137109",
   "137110",
   "137111",
   "137112",
   "137113",
   "137114",
   "137115",
   "137116",
   "137117",
   "137118",
   "137119",
   "137120",
   "137121",
   "137122",
   "137123",
   "137124",
   "137125",
   "137126",
   "137127",
   "137128",
   "137129",
   "137130",
   "137131",
   "137132",
   "137133",
   "137134",
   "137135",
   "137136",
   "137137",
   "302820",
   "302825",
   "302830",
   "302830a",
   "302830b",
   "302830c",
   "302830d",
   "302830e",
   "302830f",
   "302830g",
   "302831",
   "302832",
   "302833"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Actions",
   "Class",
   "Date",
   "Datetime",
   "Field",
   "Has_Field",
   "Has_List",
   "Has_Modifier",
   "Has_Other_Field",
   "Has_Other_Field_2",
   "Has_Other_Source_Field",
   "Has_Procedure",
   "Has_Source_Field",
   "Has_View",
   "Id",
   "Installed",
   "Integer",
   "Is_Class",
   "Is_Mandatory_Class",
   "Is_Mandatory_Field",
   "Is_Mandatory_List",
   "Is_Mandatory_Modifier",
   "Is_Mandatory_Other_Field",
   "Is_Mandatory_Other_Field_2",
   "Is_Mandatory_Other_Source_Field",
   "Is_Mandatory_Procedure",
   "Is_Mandatory_Source_Field",
   "Is_Mandatory_View",
   "Is_Other_Package",
   "List",
   "Model",
   "Modifier",
   "Name",
   "Notes",
   "Numeric",
   "Other_Field",
   "Other_Field_2",
   "Other_Package",
   "Other_Package_Required",
   "Other_Package_Type",
   "Other_Source_Field",
   "Package",
   "Primitive",
   "Procedure",
   "Source_Field",
   "String",
   "Time",
   "Use_Option",
   "Value",
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

const int c_num_transient_fields = 2;

const char* const c_transient_sorted_field_ids[ ] =
{
   "137107",
   "137116"
};

const char* const c_transient_sorted_field_names[ ] =
{
   "Actions",
   "Value"
};

inline bool is_transient_field( const string& field )
{
   return binary_search( c_transient_sorted_field_ids,
    c_transient_sorted_field_ids + c_num_transient_fields, field.c_str( ), compare )
    || binary_search( c_transient_sorted_field_names,
    c_transient_sorted_field_names + c_num_transient_fields, field.c_str( ), compare );
}

const char* const c_procedure_id_Clear = "137430";
const char* const c_procedure_id_Edit = "137410";
const char* const c_procedure_id_Select = "137420";

const uint64_t c_modifier_Hide_Field = UINT64_C( 0x100 );
const uint64_t c_modifier_Hide_List = UINT64_C( 0x200 );
const uint64_t c_modifier_Hide_Modifier = UINT64_C( 0x400 );
const uint64_t c_modifier_Hide_Other_Field = UINT64_C( 0x800 );
const uint64_t c_modifier_Hide_Other_Field_2 = UINT64_C( 0x1000 );
const uint64_t c_modifier_Hide_Other_Source_Field = UINT64_C( 0x2000 );
const uint64_t c_modifier_Hide_Procedure = UINT64_C( 0x4000 );
const uint64_t c_modifier_Hide_Source_Field = UINT64_C( 0x8000 );
const uint64_t c_modifier_Hide_View = UINT64_C( 0x10000 );
const uint64_t c_modifier_Is_Bool = UINT64_C( 0x20000 );
const uint64_t c_modifier_Is_Class_Option = UINT64_C( 0x40000 );
const uint64_t c_modifier_Is_Date = UINT64_C( 0x80000 );
const uint64_t c_modifier_Is_Datetime = UINT64_C( 0x100000 );
const uint64_t c_modifier_Is_Int = UINT64_C( 0x200000 );
const uint64_t c_modifier_Is_Not_Class_Option = UINT64_C( 0x400000 );
const uint64_t c_modifier_Is_Not_Other_Package_Option = UINT64_C( 0x800000 );
const uint64_t c_modifier_Is_Numeric = UINT64_C( 0x1000000 );
const uint64_t c_modifier_Is_Other_Package_Option = UINT64_C( 0x2000000 );
const uint64_t c_modifier_Is_String = UINT64_C( 0x4000000 );
const uint64_t c_modifier_Is_Time = UINT64_C( 0x8000000 );

domain_string_max_size< 100 > g_Id_domain;
domain_string_max_size< 100 > g_Name_domain;

string g_group_field_name;
string g_level_field_name;
string g_order_field_name;
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_Package_Option* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Package_Option* > external_aliases_lookup_container;
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

string g_default_Actions = string( "137410$137100*@id" );
string g_default_Class = string( );
string g_default_Date = string( );
string g_default_Datetime = string( );
string g_default_Field = string( );
bool g_default_Has_Field = bool( 0 );
bool g_default_Has_List = bool( 0 );
bool g_default_Has_Modifier = bool( 0 );
bool g_default_Has_Other_Field = bool( 0 );
bool g_default_Has_Other_Field_2 = bool( 0 );
bool g_default_Has_Other_Source_Field = bool( 0 );
bool g_default_Has_Procedure = bool( 0 );
bool g_default_Has_Source_Field = bool( 0 );
bool g_default_Has_View = bool( 0 );
string g_default_Id = string( );
bool g_default_Installed = bool( 0 );
int g_default_Integer = int( 0 );
bool g_default_Is_Class = bool( 0 );
bool g_default_Is_Mandatory_Class = bool( 0 );
bool g_default_Is_Mandatory_Field = bool( 0 );
bool g_default_Is_Mandatory_List = bool( 0 );
bool g_default_Is_Mandatory_Modifier = bool( 0 );
bool g_default_Is_Mandatory_Other_Field = bool( 0 );
bool g_default_Is_Mandatory_Other_Field_2 = bool( 0 );
bool g_default_Is_Mandatory_Other_Source_Field = bool( 0 );
bool g_default_Is_Mandatory_Procedure = bool( 0 );
bool g_default_Is_Mandatory_Source_Field = bool( 0 );
bool g_default_Is_Mandatory_View = bool( 0 );
bool g_default_Is_Other_Package = bool( 0 );
string g_default_List = string( );
string g_default_Model = string( );
string g_default_Modifier = string( );
string g_default_Name = string( );
string g_default_Notes = string( );
string g_default_Numeric = string( );
string g_default_Other_Field = string( );
string g_default_Other_Field_2 = string( );
string g_default_Other_Package = string( );
bool g_default_Other_Package_Required = bool( 0 );
string g_default_Other_Package_Type = string( );
string g_default_Other_Source_Field = string( );
string g_default_Package = string( );
int g_default_Primitive = int( 6 );
string g_default_Procedure = string( );
string g_default_Source_Field = string( );
string g_default_String = string( );
string g_default_Time = string( );
bool g_default_Use_Option = bool( 0 );
string g_default_Value = string( );
string g_default_View = string( );

set< int > g_primitive_enum;

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

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Package_Option > Package_Option_registration( get_class_registry( ), "137100" );

class Meta_Package_Option_command_functor;

class Meta_Package_Option_command_handler : public command_handler
{
   friend class Meta_Package_Option_command_functor;

   public:
   Meta_Package_Option_command_handler( ) : p_Meta_Package_Option( 0 ) { }

   void set_Meta_Package_Option( Meta_Package_Option* p_new_Meta_Package_Option ) { p_Meta_Package_Option = p_new_Meta_Package_Option; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Package_Option* p_Meta_Package_Option;

   protected:
   string retval;
};

class Meta_Package_Option_command_functor : public command_functor
{
   public:
   Meta_Package_Option_command_functor( Meta_Package_Option_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Package_Option_command_handler& cmd_handler;
};

command_functor* Meta_Package_Option_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Package_Option_command_functor( dynamic_cast< Meta_Package_Option_command_handler& >( handler ) );
}

void Meta_Package_Option_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Package_Option_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_Package_Option_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Package_Option_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Package_Option->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Package_Option->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Package_Option_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Package_Option_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Actions ) || ( field_name == c_field_name_Actions ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package_Option->Actions( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Package_Option->Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Date ) || ( field_name == c_field_name_Date ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package_Option->Date( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Datetime ) || ( field_name == c_field_name_Datetime ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package_Option->Datetime( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Field ) || ( field_name == c_field_name_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Package_Option->Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Has_Field ) || ( field_name == c_field_name_Has_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Has_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Has_List ) || ( field_name == c_field_name_Has_List ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Has_List( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Has_Modifier ) || ( field_name == c_field_name_Has_Modifier ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Has_Modifier( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Has_Other_Field ) || ( field_name == c_field_name_Has_Other_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Has_Other_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Has_Other_Field_2 ) || ( field_name == c_field_name_Has_Other_Field_2 ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Has_Other_Field_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Has_Other_Source_Field ) || ( field_name == c_field_name_Has_Other_Source_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Has_Other_Source_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Has_Procedure ) || ( field_name == c_field_name_Has_Procedure ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Has_Procedure( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Has_Source_Field ) || ( field_name == c_field_name_Has_Source_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Has_Source_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Has_View ) || ( field_name == c_field_name_Has_View ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Has_View( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Id ) || ( field_name == c_field_name_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package_Option->Id( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Installed ) || ( field_name == c_field_name_Installed ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Installed( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Integer ) || ( field_name == c_field_name_Integer ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Package_Option->Integer( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Class ) || ( field_name == c_field_name_Is_Class ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Is_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_Class ) || ( field_name == c_field_name_Is_Mandatory_Class ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Is_Mandatory_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_Field ) || ( field_name == c_field_name_Is_Mandatory_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Is_Mandatory_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_List ) || ( field_name == c_field_name_Is_Mandatory_List ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Is_Mandatory_List( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_Modifier ) || ( field_name == c_field_name_Is_Mandatory_Modifier ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Is_Mandatory_Modifier( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_Other_Field ) || ( field_name == c_field_name_Is_Mandatory_Other_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Is_Mandatory_Other_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_Other_Field_2 ) || ( field_name == c_field_name_Is_Mandatory_Other_Field_2 ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Is_Mandatory_Other_Field_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_Other_Source_Field ) || ( field_name == c_field_name_Is_Mandatory_Other_Source_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Is_Mandatory_Other_Source_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_Procedure ) || ( field_name == c_field_name_Is_Mandatory_Procedure ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Is_Mandatory_Procedure( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_Source_Field ) || ( field_name == c_field_name_Is_Mandatory_Source_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Is_Mandatory_Source_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_View ) || ( field_name == c_field_name_Is_Mandatory_View ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Is_Mandatory_View( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Other_Package ) || ( field_name == c_field_name_Is_Other_Package ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Is_Other_Package( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_List ) || ( field_name == c_field_name_List ) ) )
      {
         handled = true;

         string_getter< Meta_List >( cmd_handler.p_Meta_Package_Option->List( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) ) )
      {
         handled = true;

         string_getter< Meta_Model >( cmd_handler.p_Meta_Package_Option->Model( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Modifier ) || ( field_name == c_field_name_Modifier ) ) )
      {
         handled = true;

         string_getter< Meta_Modifier >( cmd_handler.p_Meta_Package_Option->Modifier( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package_Option->Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Notes ) || ( field_name == c_field_name_Notes ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package_Option->Notes( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Numeric ) || ( field_name == c_field_name_Numeric ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package_Option->Numeric( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Field ) || ( field_name == c_field_name_Other_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Package_Option->Other_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Field_2 ) || ( field_name == c_field_name_Other_Field_2 ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Package_Option->Other_Field_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Package ) || ( field_name == c_field_name_Other_Package ) ) )
      {
         handled = true;

         string_getter< Meta_Package >( cmd_handler.p_Meta_Package_Option->Other_Package( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Package_Required ) || ( field_name == c_field_name_Other_Package_Required ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Other_Package_Required( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Package_Type ) || ( field_name == c_field_name_Other_Package_Type ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package_Option->Other_Package_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Source_Field ) || ( field_name == c_field_name_Other_Source_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Package_Option->Other_Source_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Package ) || ( field_name == c_field_name_Package ) ) )
      {
         handled = true;

         string_getter< Meta_Package >( cmd_handler.p_Meta_Package_Option->Package( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Primitive ) || ( field_name == c_field_name_Primitive ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Package_Option->Primitive( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Procedure ) || ( field_name == c_field_name_Procedure ) ) )
      {
         handled = true;

         string_getter< Meta_Procedure >( cmd_handler.p_Meta_Package_Option->Procedure( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Field ) || ( field_name == c_field_name_Source_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Package_Option->Source_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_String ) || ( field_name == c_field_name_String ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package_Option->String( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Time ) || ( field_name == c_field_name_Time ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package_Option->Time( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Option ) || ( field_name == c_field_name_Use_Option ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Use_Option( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Value ) || ( field_name == c_field_name_Value ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package_Option->Value( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_View ) || ( field_name == c_field_name_View ) ) )
      {
         handled = true;

         string_getter< Meta_View >( cmd_handler.p_Meta_Package_Option->View( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Package_Option_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Package_Option_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_Package_Option_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Actions ) || ( field_name == c_field_name_Actions ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Actions, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, Meta_Class >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Date ) || ( field_name == c_field_name_Date ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Date, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Datetime ) || ( field_name == c_field_name_Datetime ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Datetime, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Field ) || ( field_name == c_field_name_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, Meta_Field >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Has_Field ) || ( field_name == c_field_name_Has_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Has_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Has_List ) || ( field_name == c_field_name_Has_List ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Has_List, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Has_Modifier ) || ( field_name == c_field_name_Has_Modifier ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Has_Modifier, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Has_Other_Field ) || ( field_name == c_field_name_Has_Other_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Has_Other_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Has_Other_Field_2 ) || ( field_name == c_field_name_Has_Other_Field_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Has_Other_Field_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Has_Other_Source_Field ) || ( field_name == c_field_name_Has_Other_Source_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Has_Other_Source_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Has_Procedure ) || ( field_name == c_field_name_Has_Procedure ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Has_Procedure, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Has_Source_Field ) || ( field_name == c_field_name_Has_Source_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Has_Source_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Has_View ) || ( field_name == c_field_name_Has_View ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Has_View, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Id ) || ( field_name == c_field_name_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Id, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Installed ) || ( field_name == c_field_name_Installed ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Installed, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Integer ) || ( field_name == c_field_name_Integer ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, int >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Integer, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Class ) || ( field_name == c_field_name_Is_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Is_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_Class ) || ( field_name == c_field_name_Is_Mandatory_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Is_Mandatory_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_Field ) || ( field_name == c_field_name_Is_Mandatory_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Is_Mandatory_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_List ) || ( field_name == c_field_name_Is_Mandatory_List ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Is_Mandatory_List, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_Modifier ) || ( field_name == c_field_name_Is_Mandatory_Modifier ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Is_Mandatory_Modifier, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_Other_Field ) || ( field_name == c_field_name_Is_Mandatory_Other_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Is_Mandatory_Other_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_Other_Field_2 ) || ( field_name == c_field_name_Is_Mandatory_Other_Field_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Is_Mandatory_Other_Field_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_Other_Source_Field ) || ( field_name == c_field_name_Is_Mandatory_Other_Source_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Is_Mandatory_Other_Source_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_Procedure ) || ( field_name == c_field_name_Is_Mandatory_Procedure ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Is_Mandatory_Procedure, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_Source_Field ) || ( field_name == c_field_name_Is_Mandatory_Source_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Is_Mandatory_Source_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Mandatory_View ) || ( field_name == c_field_name_Is_Mandatory_View ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Is_Mandatory_View, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Other_Package ) || ( field_name == c_field_name_Is_Other_Package ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Is_Other_Package, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_List ) || ( field_name == c_field_name_List ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, Meta_List >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::List, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, Meta_Model >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Model, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Modifier ) || ( field_name == c_field_name_Modifier ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, Meta_Modifier >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Modifier, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Notes ) || ( field_name == c_field_name_Notes ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Notes, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Numeric ) || ( field_name == c_field_name_Numeric ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Numeric, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Field ) || ( field_name == c_field_name_Other_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, Meta_Field >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Other_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Field_2 ) || ( field_name == c_field_name_Other_Field_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, Meta_Field >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Other_Field_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Package ) || ( field_name == c_field_name_Other_Package ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, Meta_Package >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Other_Package, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Package_Required ) || ( field_name == c_field_name_Other_Package_Required ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Other_Package_Required, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Package_Type ) || ( field_name == c_field_name_Other_Package_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Other_Package_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Source_Field ) || ( field_name == c_field_name_Other_Source_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, Meta_Field >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Other_Source_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Package ) || ( field_name == c_field_name_Package ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, Meta_Package >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Package, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Primitive ) || ( field_name == c_field_name_Primitive ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, int >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Primitive, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Procedure ) || ( field_name == c_field_name_Procedure ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, Meta_Procedure >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Procedure, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Field ) || ( field_name == c_field_name_Source_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, Meta_Field >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Source_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_String ) || ( field_name == c_field_name_String ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::String, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Time ) || ( field_name == c_field_name_Time ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Time, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Option ) || ( field_name == c_field_name_Use_Option ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Use_Option, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Value ) || ( field_name == c_field_name_Value ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Value, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_View ) || ( field_name == c_field_name_View ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Option, Meta_View >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::View, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Package_Option_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Package_Option_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_Package_Option_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Field ) || ( field_name == c_field_name_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->Field( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_List ) || ( field_name == c_field_name_List ) )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->List( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->Model( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Modifier ) || ( field_name == c_field_name_Modifier ) )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->Modifier( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Other_Field ) || ( field_name == c_field_name_Other_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->Other_Field( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Other_Field_2 ) || ( field_name == c_field_name_Other_Field_2 ) )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->Other_Field_2( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Other_Package ) || ( field_name == c_field_name_Other_Package ) )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->Other_Package( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Other_Source_Field ) || ( field_name == c_field_name_Other_Source_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->Other_Source_Field( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Package ) || ( field_name == c_field_name_Package ) )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->Package( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Procedure ) || ( field_name == c_field_name_Procedure ) )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->Procedure( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Field ) || ( field_name == c_field_name_Source_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->Source_Field( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_View ) || ( field_name == c_field_name_View ) )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->View( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
   else if( command == c_cmd_Meta_Package_Option_Clear )
   {
      cmd_handler.p_Meta_Package_Option->Clear( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Package_Option_Edit )
   {
      cmd_handler.p_Meta_Package_Option->Edit( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Package_Option_Select )
   {
      cmd_handler.p_Meta_Package_Option->Select( );

      cmd_handler.retval.erase( );

   }
}

struct Meta_Package_Option::impl : public Meta_Package_Option_command_handler
{
   impl( Meta_Package_Option& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Package_Option( &o );

      add_commands( 0, Meta_Package_Option_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Package_Option_command_definitions ) );
   }

   Meta_Package_Option& get_obj( ) const
   {
      return *cp_obj;
   }

   const string& impl_Actions( ) const { return lazy_fetch( p_obj ), v_Actions; }
   void impl_Actions( const string& Actions ) { sanity_check( Actions ); v_Actions = Actions; }

   const string& impl_Date( ) const { return lazy_fetch( p_obj ), v_Date; }
   void impl_Date( const string& Date ) { sanity_check( Date ); v_Date = Date; }

   const string& impl_Datetime( ) const { return lazy_fetch( p_obj ), v_Datetime; }
   void impl_Datetime( const string& Datetime ) { sanity_check( Datetime ); v_Datetime = Datetime; }

   bool impl_Has_Field( ) const { return lazy_fetch( p_obj ), v_Has_Field; }
   void impl_Has_Field( bool Has_Field ) { v_Has_Field = Has_Field; }

   bool impl_Has_List( ) const { return lazy_fetch( p_obj ), v_Has_List; }
   void impl_Has_List( bool Has_List ) { v_Has_List = Has_List; }

   bool impl_Has_Modifier( ) const { return lazy_fetch( p_obj ), v_Has_Modifier; }
   void impl_Has_Modifier( bool Has_Modifier ) { v_Has_Modifier = Has_Modifier; }

   bool impl_Has_Other_Field( ) const { return lazy_fetch( p_obj ), v_Has_Other_Field; }
   void impl_Has_Other_Field( bool Has_Other_Field ) { v_Has_Other_Field = Has_Other_Field; }

   bool impl_Has_Other_Field_2( ) const { return lazy_fetch( p_obj ), v_Has_Other_Field_2; }
   void impl_Has_Other_Field_2( bool Has_Other_Field_2 ) { v_Has_Other_Field_2 = Has_Other_Field_2; }

   bool impl_Has_Other_Source_Field( ) const { return lazy_fetch( p_obj ), v_Has_Other_Source_Field; }
   void impl_Has_Other_Source_Field( bool Has_Other_Source_Field ) { v_Has_Other_Source_Field = Has_Other_Source_Field; }

   bool impl_Has_Procedure( ) const { return lazy_fetch( p_obj ), v_Has_Procedure; }
   void impl_Has_Procedure( bool Has_Procedure ) { v_Has_Procedure = Has_Procedure; }

   bool impl_Has_Source_Field( ) const { return lazy_fetch( p_obj ), v_Has_Source_Field; }
   void impl_Has_Source_Field( bool Has_Source_Field ) { v_Has_Source_Field = Has_Source_Field; }

   bool impl_Has_View( ) const { return lazy_fetch( p_obj ), v_Has_View; }
   void impl_Has_View( bool Has_View ) { v_Has_View = Has_View; }

   const string& impl_Id( ) const { return lazy_fetch( p_obj ), v_Id; }
   void impl_Id( const string& Id ) { sanity_check( Id ); v_Id = Id; }

   bool impl_Installed( ) const { return lazy_fetch( p_obj ), v_Installed; }
   void impl_Installed( bool Installed ) { v_Installed = Installed; }

   int impl_Integer( ) const { return lazy_fetch( p_obj ), v_Integer; }
   void impl_Integer( int Integer ) { v_Integer = Integer; }

   bool impl_Is_Class( ) const { return lazy_fetch( p_obj ), v_Is_Class; }
   void impl_Is_Class( bool Is_Class ) { v_Is_Class = Is_Class; }

   bool impl_Is_Mandatory_Class( ) const { return lazy_fetch( p_obj ), v_Is_Mandatory_Class; }
   void impl_Is_Mandatory_Class( bool Is_Mandatory_Class ) { v_Is_Mandatory_Class = Is_Mandatory_Class; }

   bool impl_Is_Mandatory_Field( ) const { return lazy_fetch( p_obj ), v_Is_Mandatory_Field; }
   void impl_Is_Mandatory_Field( bool Is_Mandatory_Field ) { v_Is_Mandatory_Field = Is_Mandatory_Field; }

   bool impl_Is_Mandatory_List( ) const { return lazy_fetch( p_obj ), v_Is_Mandatory_List; }
   void impl_Is_Mandatory_List( bool Is_Mandatory_List ) { v_Is_Mandatory_List = Is_Mandatory_List; }

   bool impl_Is_Mandatory_Modifier( ) const { return lazy_fetch( p_obj ), v_Is_Mandatory_Modifier; }
   void impl_Is_Mandatory_Modifier( bool Is_Mandatory_Modifier ) { v_Is_Mandatory_Modifier = Is_Mandatory_Modifier; }

   bool impl_Is_Mandatory_Other_Field( ) const { return lazy_fetch( p_obj ), v_Is_Mandatory_Other_Field; }
   void impl_Is_Mandatory_Other_Field( bool Is_Mandatory_Other_Field ) { v_Is_Mandatory_Other_Field = Is_Mandatory_Other_Field; }

   bool impl_Is_Mandatory_Other_Field_2( ) const { return lazy_fetch( p_obj ), v_Is_Mandatory_Other_Field_2; }
   void impl_Is_Mandatory_Other_Field_2( bool Is_Mandatory_Other_Field_2 ) { v_Is_Mandatory_Other_Field_2 = Is_Mandatory_Other_Field_2; }

   bool impl_Is_Mandatory_Other_Source_Field( ) const { return lazy_fetch( p_obj ), v_Is_Mandatory_Other_Source_Field; }
   void impl_Is_Mandatory_Other_Source_Field( bool Is_Mandatory_Other_Source_Field ) { v_Is_Mandatory_Other_Source_Field = Is_Mandatory_Other_Source_Field; }

   bool impl_Is_Mandatory_Procedure( ) const { return lazy_fetch( p_obj ), v_Is_Mandatory_Procedure; }
   void impl_Is_Mandatory_Procedure( bool Is_Mandatory_Procedure ) { v_Is_Mandatory_Procedure = Is_Mandatory_Procedure; }

   bool impl_Is_Mandatory_Source_Field( ) const { return lazy_fetch( p_obj ), v_Is_Mandatory_Source_Field; }
   void impl_Is_Mandatory_Source_Field( bool Is_Mandatory_Source_Field ) { v_Is_Mandatory_Source_Field = Is_Mandatory_Source_Field; }

   bool impl_Is_Mandatory_View( ) const { return lazy_fetch( p_obj ), v_Is_Mandatory_View; }
   void impl_Is_Mandatory_View( bool Is_Mandatory_View ) { v_Is_Mandatory_View = Is_Mandatory_View; }

   bool impl_Is_Other_Package( ) const { return lazy_fetch( p_obj ), v_Is_Other_Package; }
   void impl_Is_Other_Package( bool Is_Other_Package ) { v_Is_Other_Package = Is_Other_Package; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { sanity_check( Name ); v_Name = Name; }

   const string& impl_Notes( ) const { return lazy_fetch( p_obj ), v_Notes; }
   void impl_Notes( const string& Notes ) { sanity_check( Notes ); v_Notes = Notes; }

   const string& impl_Numeric( ) const { return lazy_fetch( p_obj ), v_Numeric; }
   void impl_Numeric( const string& Numeric ) { sanity_check( Numeric ); v_Numeric = Numeric; }

   bool impl_Other_Package_Required( ) const { return lazy_fetch( p_obj ), v_Other_Package_Required; }
   void impl_Other_Package_Required( bool Other_Package_Required ) { v_Other_Package_Required = Other_Package_Required; }

   const string& impl_Other_Package_Type( ) const { return lazy_fetch( p_obj ), v_Other_Package_Type; }
   void impl_Other_Package_Type( const string& Other_Package_Type ) { sanity_check( Other_Package_Type ); v_Other_Package_Type = Other_Package_Type; }

   int impl_Primitive( ) const { return lazy_fetch( p_obj ), v_Primitive; }
   void impl_Primitive( int Primitive ) { v_Primitive = Primitive; }

   const string& impl_String( ) const { return lazy_fetch( p_obj ), v_String; }
   void impl_String( const string& String ) { sanity_check( String ); v_String = String; }

   const string& impl_Time( ) const { return lazy_fetch( p_obj ), v_Time; }
   void impl_Time( const string& Time ) { sanity_check( Time ); v_Time = Time; }

   bool impl_Use_Option( ) const { return lazy_fetch( p_obj ), v_Use_Option; }
   void impl_Use_Option( bool Use_Option ) { v_Use_Option = Use_Option; }

   const string& impl_Value( ) const { return lazy_fetch( p_obj ), v_Value; }
   void impl_Value( const string& Value ) { sanity_check( Value ); v_Value = Value; }

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

   Meta_Field& impl_Field( )
   {
      if( !cp_Field )
      {
         cp_Field.init( );

         p_obj->setup_graph_parent( *cp_Field, c_field_id_Field, v_Field );
      }
      return *cp_Field;
   }

   const Meta_Field& impl_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Field )
      {
         cp_Field.init( );

         p_obj->setup_graph_parent( *cp_Field, c_field_id_Field, v_Field );
      }
      return *cp_Field;
   }

   void impl_Field( const string& key )
   {
      class_base_accessor cba( impl_Field( ) );
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

   Meta_Model& impl_Model( )
   {
      if( !cp_Model )
      {
         cp_Model.init( );

         p_obj->setup_graph_parent( *cp_Model, c_field_id_Model, v_Model );
      }
      return *cp_Model;
   }

   const Meta_Model& impl_Model( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Model )
      {
         cp_Model.init( );

         p_obj->setup_graph_parent( *cp_Model, c_field_id_Model, v_Model );
      }
      return *cp_Model;
   }

   void impl_Model( const string& key )
   {
      class_base_accessor cba( impl_Model( ) );
      cba.set_key( key );
   }

   Meta_Modifier& impl_Modifier( )
   {
      if( !cp_Modifier )
      {
         cp_Modifier.init( );

         p_obj->setup_graph_parent( *cp_Modifier, c_field_id_Modifier, v_Modifier );
      }
      return *cp_Modifier;
   }

   const Meta_Modifier& impl_Modifier( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Modifier )
      {
         cp_Modifier.init( );

         p_obj->setup_graph_parent( *cp_Modifier, c_field_id_Modifier, v_Modifier );
      }
      return *cp_Modifier;
   }

   void impl_Modifier( const string& key )
   {
      class_base_accessor cba( impl_Modifier( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Other_Field( )
   {
      if( !cp_Other_Field )
      {
         cp_Other_Field.init( );

         p_obj->setup_graph_parent( *cp_Other_Field, c_field_id_Other_Field, v_Other_Field );
      }
      return *cp_Other_Field;
   }

   const Meta_Field& impl_Other_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Other_Field )
      {
         cp_Other_Field.init( );

         p_obj->setup_graph_parent( *cp_Other_Field, c_field_id_Other_Field, v_Other_Field );
      }
      return *cp_Other_Field;
   }

   void impl_Other_Field( const string& key )
   {
      class_base_accessor cba( impl_Other_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Other_Field_2( )
   {
      if( !cp_Other_Field_2 )
      {
         cp_Other_Field_2.init( );

         p_obj->setup_graph_parent( *cp_Other_Field_2, c_field_id_Other_Field_2, v_Other_Field_2 );
      }
      return *cp_Other_Field_2;
   }

   const Meta_Field& impl_Other_Field_2( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Other_Field_2 )
      {
         cp_Other_Field_2.init( );

         p_obj->setup_graph_parent( *cp_Other_Field_2, c_field_id_Other_Field_2, v_Other_Field_2 );
      }
      return *cp_Other_Field_2;
   }

   void impl_Other_Field_2( const string& key )
   {
      class_base_accessor cba( impl_Other_Field_2( ) );
      cba.set_key( key );
   }

   Meta_Package& impl_Other_Package( )
   {
      if( !cp_Other_Package )
      {
         cp_Other_Package.init( );

         p_obj->setup_graph_parent( *cp_Other_Package, c_field_id_Other_Package, v_Other_Package );
      }
      return *cp_Other_Package;
   }

   const Meta_Package& impl_Other_Package( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Other_Package )
      {
         cp_Other_Package.init( );

         p_obj->setup_graph_parent( *cp_Other_Package, c_field_id_Other_Package, v_Other_Package );
      }
      return *cp_Other_Package;
   }

   void impl_Other_Package( const string& key )
   {
      class_base_accessor cba( impl_Other_Package( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Other_Source_Field( )
   {
      if( !cp_Other_Source_Field )
      {
         cp_Other_Source_Field.init( );

         p_obj->setup_graph_parent( *cp_Other_Source_Field, c_field_id_Other_Source_Field, v_Other_Source_Field );
      }
      return *cp_Other_Source_Field;
   }

   const Meta_Field& impl_Other_Source_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Other_Source_Field )
      {
         cp_Other_Source_Field.init( );

         p_obj->setup_graph_parent( *cp_Other_Source_Field, c_field_id_Other_Source_Field, v_Other_Source_Field );
      }
      return *cp_Other_Source_Field;
   }

   void impl_Other_Source_Field( const string& key )
   {
      class_base_accessor cba( impl_Other_Source_Field( ) );
      cba.set_key( key );
   }

   Meta_Package& impl_Package( )
   {
      if( !cp_Package )
      {
         cp_Package.init( );

         p_obj->setup_graph_parent( *cp_Package, c_field_id_Package, v_Package );
      }
      return *cp_Package;
   }

   const Meta_Package& impl_Package( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Package )
      {
         cp_Package.init( );

         p_obj->setup_graph_parent( *cp_Package, c_field_id_Package, v_Package );
      }
      return *cp_Package;
   }

   void impl_Package( const string& key )
   {
      class_base_accessor cba( impl_Package( ) );
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

   void impl_Clear( );

   void impl_Edit( );

   void impl_Select( );

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
   void completed_fetch( bool skip_set_original );

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

   Meta_Package_Option* p_obj;
   class_pointer< Meta_Package_Option > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   string v_Actions;
   string v_Date;
   string v_Datetime;
   bool v_Has_Field;
   bool v_Has_List;
   bool v_Has_Modifier;
   bool v_Has_Other_Field;
   bool v_Has_Other_Field_2;
   bool v_Has_Other_Source_Field;
   bool v_Has_Procedure;
   bool v_Has_Source_Field;
   bool v_Has_View;
   string v_Id;
   bool v_Installed;
   int v_Integer;
   bool v_Is_Class;
   bool v_Is_Mandatory_Class;
   bool v_Is_Mandatory_Field;
   bool v_Is_Mandatory_List;
   bool v_Is_Mandatory_Modifier;
   bool v_Is_Mandatory_Other_Field;
   bool v_Is_Mandatory_Other_Field_2;
   bool v_Is_Mandatory_Other_Source_Field;
   bool v_Is_Mandatory_Procedure;
   bool v_Is_Mandatory_Source_Field;
   bool v_Is_Mandatory_View;
   bool v_Is_Other_Package;
   string v_Name;
   string v_Notes;
   string v_Numeric;
   bool v_Other_Package_Required;
   string v_Other_Package_Type;
   int v_Primitive;
   string v_String;
   string v_Time;
   bool v_Use_Option;
   string v_Value;

   string v_Class;
   mutable class_pointer< Meta_Class > cp_Class;

   string v_Field;
   mutable class_pointer< Meta_Field > cp_Field;

   string v_List;
   mutable class_pointer< Meta_List > cp_List;

   string v_Model;
   mutable class_pointer< Meta_Model > cp_Model;

   string v_Modifier;
   mutable class_pointer< Meta_Modifier > cp_Modifier;

   string v_Other_Field;
   mutable class_pointer< Meta_Field > cp_Other_Field;

   string v_Other_Field_2;
   mutable class_pointer< Meta_Field > cp_Other_Field_2;

   string v_Other_Package;
   mutable class_pointer< Meta_Package > cp_Other_Package;

   string v_Other_Source_Field;
   mutable class_pointer< Meta_Field > cp_Other_Source_Field;

   string v_Package;
   mutable class_pointer< Meta_Package > cp_Package;

   string v_Procedure;
   mutable class_pointer< Meta_Procedure > cp_Procedure;

   string v_Source_Field;
   mutable class_pointer< Meta_Field > cp_Source_Field;

   string v_View;
   mutable class_pointer< Meta_View > cp_View;
};

void Meta_Package_Option::impl::impl_Clear( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Clear_impl>]
//nyi
   get_obj( ).op_update( );
   get_obj( ).Use_Option( false );
   get_obj( ).op_apply( );
   // [<finish Clear_impl>]
}

void Meta_Package_Option::impl::impl_Edit( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Edit_impl>]
   // [<finish Edit_impl>]
}

void Meta_Package_Option::impl::impl_Select( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Select_impl>]
//nyi
   get_obj( ).op_update( );
   get_obj( ).Use_Option( true );
   get_obj( ).op_apply( );
   // [<finish Select_impl>]
}

string Meta_Package_Option::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Actions( ) );
      break;

      case 1:
      retval = to_string( impl_Class( ) );
      break;

      case 2:
      retval = to_string( impl_Date( ) );
      break;

      case 3:
      retval = to_string( impl_Datetime( ) );
      break;

      case 4:
      retval = to_string( impl_Field( ) );
      break;

      case 5:
      retval = to_string( impl_Has_Field( ) );
      break;

      case 6:
      retval = to_string( impl_Has_List( ) );
      break;

      case 7:
      retval = to_string( impl_Has_Modifier( ) );
      break;

      case 8:
      retval = to_string( impl_Has_Other_Field( ) );
      break;

      case 9:
      retval = to_string( impl_Has_Other_Field_2( ) );
      break;

      case 10:
      retval = to_string( impl_Has_Other_Source_Field( ) );
      break;

      case 11:
      retval = to_string( impl_Has_Procedure( ) );
      break;

      case 12:
      retval = to_string( impl_Has_Source_Field( ) );
      break;

      case 13:
      retval = to_string( impl_Has_View( ) );
      break;

      case 14:
      retval = to_string( impl_Id( ) );
      break;

      case 15:
      retval = to_string( impl_Installed( ) );
      break;

      case 16:
      retval = to_string( impl_Integer( ) );
      break;

      case 17:
      retval = to_string( impl_Is_Class( ) );
      break;

      case 18:
      retval = to_string( impl_Is_Mandatory_Class( ) );
      break;

      case 19:
      retval = to_string( impl_Is_Mandatory_Field( ) );
      break;

      case 20:
      retval = to_string( impl_Is_Mandatory_List( ) );
      break;

      case 21:
      retval = to_string( impl_Is_Mandatory_Modifier( ) );
      break;

      case 22:
      retval = to_string( impl_Is_Mandatory_Other_Field( ) );
      break;

      case 23:
      retval = to_string( impl_Is_Mandatory_Other_Field_2( ) );
      break;

      case 24:
      retval = to_string( impl_Is_Mandatory_Other_Source_Field( ) );
      break;

      case 25:
      retval = to_string( impl_Is_Mandatory_Procedure( ) );
      break;

      case 26:
      retval = to_string( impl_Is_Mandatory_Source_Field( ) );
      break;

      case 27:
      retval = to_string( impl_Is_Mandatory_View( ) );
      break;

      case 28:
      retval = to_string( impl_Is_Other_Package( ) );
      break;

      case 29:
      retval = to_string( impl_List( ) );
      break;

      case 30:
      retval = to_string( impl_Model( ) );
      break;

      case 31:
      retval = to_string( impl_Modifier( ) );
      break;

      case 32:
      retval = to_string( impl_Name( ) );
      break;

      case 33:
      retval = to_string( impl_Notes( ) );
      break;

      case 34:
      retval = to_string( impl_Numeric( ) );
      break;

      case 35:
      retval = to_string( impl_Other_Field( ) );
      break;

      case 36:
      retval = to_string( impl_Other_Field_2( ) );
      break;

      case 37:
      retval = to_string( impl_Other_Package( ) );
      break;

      case 38:
      retval = to_string( impl_Other_Package_Required( ) );
      break;

      case 39:
      retval = to_string( impl_Other_Package_Type( ) );
      break;

      case 40:
      retval = to_string( impl_Other_Source_Field( ) );
      break;

      case 41:
      retval = to_string( impl_Package( ) );
      break;

      case 42:
      retval = to_string( impl_Primitive( ) );
      break;

      case 43:
      retval = to_string( impl_Procedure( ) );
      break;

      case 44:
      retval = to_string( impl_Source_Field( ) );
      break;

      case 45:
      retval = to_string( impl_String( ) );
      break;

      case 46:
      retval = to_string( impl_Time( ) );
      break;

      case 47:
      retval = to_string( impl_Use_Option( ) );
      break;

      case 48:
      retval = to_string( impl_Value( ) );
      break;

      case 49:
      retval = to_string( impl_View( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_Package_Option::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Actions, value );
      break;

      case 1:
      func_string_setter< Meta_Package_Option::impl, Meta_Class >( *this, &Meta_Package_Option::impl::impl_Class, value );
      break;

      case 2:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Date, value );
      break;

      case 3:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Datetime, value );
      break;

      case 4:
      func_string_setter< Meta_Package_Option::impl, Meta_Field >( *this, &Meta_Package_Option::impl::impl_Field, value );
      break;

      case 5:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Has_Field, value );
      break;

      case 6:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Has_List, value );
      break;

      case 7:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Has_Modifier, value );
      break;

      case 8:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Has_Other_Field, value );
      break;

      case 9:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Has_Other_Field_2, value );
      break;

      case 10:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Has_Other_Source_Field, value );
      break;

      case 11:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Has_Procedure, value );
      break;

      case 12:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Has_Source_Field, value );
      break;

      case 13:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Has_View, value );
      break;

      case 14:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Id, value );
      break;

      case 15:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Installed, value );
      break;

      case 16:
      func_string_setter< Meta_Package_Option::impl, int >( *this, &Meta_Package_Option::impl::impl_Integer, value );
      break;

      case 17:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Is_Class, value );
      break;

      case 18:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Is_Mandatory_Class, value );
      break;

      case 19:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Is_Mandatory_Field, value );
      break;

      case 20:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Is_Mandatory_List, value );
      break;

      case 21:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Is_Mandatory_Modifier, value );
      break;

      case 22:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Is_Mandatory_Other_Field, value );
      break;

      case 23:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Is_Mandatory_Other_Field_2, value );
      break;

      case 24:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Is_Mandatory_Other_Source_Field, value );
      break;

      case 25:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Is_Mandatory_Procedure, value );
      break;

      case 26:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Is_Mandatory_Source_Field, value );
      break;

      case 27:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Is_Mandatory_View, value );
      break;

      case 28:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Is_Other_Package, value );
      break;

      case 29:
      func_string_setter< Meta_Package_Option::impl, Meta_List >( *this, &Meta_Package_Option::impl::impl_List, value );
      break;

      case 30:
      func_string_setter< Meta_Package_Option::impl, Meta_Model >( *this, &Meta_Package_Option::impl::impl_Model, value );
      break;

      case 31:
      func_string_setter< Meta_Package_Option::impl, Meta_Modifier >( *this, &Meta_Package_Option::impl::impl_Modifier, value );
      break;

      case 32:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Name, value );
      break;

      case 33:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Notes, value );
      break;

      case 34:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Numeric, value );
      break;

      case 35:
      func_string_setter< Meta_Package_Option::impl, Meta_Field >( *this, &Meta_Package_Option::impl::impl_Other_Field, value );
      break;

      case 36:
      func_string_setter< Meta_Package_Option::impl, Meta_Field >( *this, &Meta_Package_Option::impl::impl_Other_Field_2, value );
      break;

      case 37:
      func_string_setter< Meta_Package_Option::impl, Meta_Package >( *this, &Meta_Package_Option::impl::impl_Other_Package, value );
      break;

      case 38:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Other_Package_Required, value );
      break;

      case 39:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Other_Package_Type, value );
      break;

      case 40:
      func_string_setter< Meta_Package_Option::impl, Meta_Field >( *this, &Meta_Package_Option::impl::impl_Other_Source_Field, value );
      break;

      case 41:
      func_string_setter< Meta_Package_Option::impl, Meta_Package >( *this, &Meta_Package_Option::impl::impl_Package, value );
      break;

      case 42:
      func_string_setter< Meta_Package_Option::impl, int >( *this, &Meta_Package_Option::impl::impl_Primitive, value );
      break;

      case 43:
      func_string_setter< Meta_Package_Option::impl, Meta_Procedure >( *this, &Meta_Package_Option::impl::impl_Procedure, value );
      break;

      case 44:
      func_string_setter< Meta_Package_Option::impl, Meta_Field >( *this, &Meta_Package_Option::impl::impl_Source_Field, value );
      break;

      case 45:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_String, value );
      break;

      case 46:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Time, value );
      break;

      case 47:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Use_Option, value );
      break;

      case 48:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Value, value );
      break;

      case 49:
      func_string_setter< Meta_Package_Option::impl, Meta_View >( *this, &Meta_Package_Option::impl::impl_View, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_Package_Option::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Actions( g_default_Actions );
      break;

      case 1:
      impl_Class( g_default_Class );
      break;

      case 2:
      impl_Date( g_default_Date );
      break;

      case 3:
      impl_Datetime( g_default_Datetime );
      break;

      case 4:
      impl_Field( g_default_Field );
      break;

      case 5:
      impl_Has_Field( g_default_Has_Field );
      break;

      case 6:
      impl_Has_List( g_default_Has_List );
      break;

      case 7:
      impl_Has_Modifier( g_default_Has_Modifier );
      break;

      case 8:
      impl_Has_Other_Field( g_default_Has_Other_Field );
      break;

      case 9:
      impl_Has_Other_Field_2( g_default_Has_Other_Field_2 );
      break;

      case 10:
      impl_Has_Other_Source_Field( g_default_Has_Other_Source_Field );
      break;

      case 11:
      impl_Has_Procedure( g_default_Has_Procedure );
      break;

      case 12:
      impl_Has_Source_Field( g_default_Has_Source_Field );
      break;

      case 13:
      impl_Has_View( g_default_Has_View );
      break;

      case 14:
      impl_Id( g_default_Id );
      break;

      case 15:
      impl_Installed( g_default_Installed );
      break;

      case 16:
      impl_Integer( g_default_Integer );
      break;

      case 17:
      impl_Is_Class( g_default_Is_Class );
      break;

      case 18:
      impl_Is_Mandatory_Class( g_default_Is_Mandatory_Class );
      break;

      case 19:
      impl_Is_Mandatory_Field( g_default_Is_Mandatory_Field );
      break;

      case 20:
      impl_Is_Mandatory_List( g_default_Is_Mandatory_List );
      break;

      case 21:
      impl_Is_Mandatory_Modifier( g_default_Is_Mandatory_Modifier );
      break;

      case 22:
      impl_Is_Mandatory_Other_Field( g_default_Is_Mandatory_Other_Field );
      break;

      case 23:
      impl_Is_Mandatory_Other_Field_2( g_default_Is_Mandatory_Other_Field_2 );
      break;

      case 24:
      impl_Is_Mandatory_Other_Source_Field( g_default_Is_Mandatory_Other_Source_Field );
      break;

      case 25:
      impl_Is_Mandatory_Procedure( g_default_Is_Mandatory_Procedure );
      break;

      case 26:
      impl_Is_Mandatory_Source_Field( g_default_Is_Mandatory_Source_Field );
      break;

      case 27:
      impl_Is_Mandatory_View( g_default_Is_Mandatory_View );
      break;

      case 28:
      impl_Is_Other_Package( g_default_Is_Other_Package );
      break;

      case 29:
      impl_List( g_default_List );
      break;

      case 30:
      impl_Model( g_default_Model );
      break;

      case 31:
      impl_Modifier( g_default_Modifier );
      break;

      case 32:
      impl_Name( g_default_Name );
      break;

      case 33:
      impl_Notes( g_default_Notes );
      break;

      case 34:
      impl_Numeric( g_default_Numeric );
      break;

      case 35:
      impl_Other_Field( g_default_Other_Field );
      break;

      case 36:
      impl_Other_Field_2( g_default_Other_Field_2 );
      break;

      case 37:
      impl_Other_Package( g_default_Other_Package );
      break;

      case 38:
      impl_Other_Package_Required( g_default_Other_Package_Required );
      break;

      case 39:
      impl_Other_Package_Type( g_default_Other_Package_Type );
      break;

      case 40:
      impl_Other_Source_Field( g_default_Other_Source_Field );
      break;

      case 41:
      impl_Package( g_default_Package );
      break;

      case 42:
      impl_Primitive( g_default_Primitive );
      break;

      case 43:
      impl_Procedure( g_default_Procedure );
      break;

      case 44:
      impl_Source_Field( g_default_Source_Field );
      break;

      case 45:
      impl_String( g_default_String );
      break;

      case 46:
      impl_Time( g_default_Time );
      break;

      case 47:
      impl_Use_Option( g_default_Use_Option );
      break;

      case 48:
      impl_Value( g_default_Value );
      break;

      case 49:
      impl_View( g_default_View );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_Package_Option::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Actions == g_default_Actions );
      break;

      case 1:
      retval = ( v_Class == g_default_Class );
      break;

      case 2:
      retval = ( v_Date == g_default_Date );
      break;

      case 3:
      retval = ( v_Datetime == g_default_Datetime );
      break;

      case 4:
      retval = ( v_Field == g_default_Field );
      break;

      case 5:
      retval = ( v_Has_Field == g_default_Has_Field );
      break;

      case 6:
      retval = ( v_Has_List == g_default_Has_List );
      break;

      case 7:
      retval = ( v_Has_Modifier == g_default_Has_Modifier );
      break;

      case 8:
      retval = ( v_Has_Other_Field == g_default_Has_Other_Field );
      break;

      case 9:
      retval = ( v_Has_Other_Field_2 == g_default_Has_Other_Field_2 );
      break;

      case 10:
      retval = ( v_Has_Other_Source_Field == g_default_Has_Other_Source_Field );
      break;

      case 11:
      retval = ( v_Has_Procedure == g_default_Has_Procedure );
      break;

      case 12:
      retval = ( v_Has_Source_Field == g_default_Has_Source_Field );
      break;

      case 13:
      retval = ( v_Has_View == g_default_Has_View );
      break;

      case 14:
      retval = ( v_Id == g_default_Id );
      break;

      case 15:
      retval = ( v_Installed == g_default_Installed );
      break;

      case 16:
      retval = ( v_Integer == g_default_Integer );
      break;

      case 17:
      retval = ( v_Is_Class == g_default_Is_Class );
      break;

      case 18:
      retval = ( v_Is_Mandatory_Class == g_default_Is_Mandatory_Class );
      break;

      case 19:
      retval = ( v_Is_Mandatory_Field == g_default_Is_Mandatory_Field );
      break;

      case 20:
      retval = ( v_Is_Mandatory_List == g_default_Is_Mandatory_List );
      break;

      case 21:
      retval = ( v_Is_Mandatory_Modifier == g_default_Is_Mandatory_Modifier );
      break;

      case 22:
      retval = ( v_Is_Mandatory_Other_Field == g_default_Is_Mandatory_Other_Field );
      break;

      case 23:
      retval = ( v_Is_Mandatory_Other_Field_2 == g_default_Is_Mandatory_Other_Field_2 );
      break;

      case 24:
      retval = ( v_Is_Mandatory_Other_Source_Field == g_default_Is_Mandatory_Other_Source_Field );
      break;

      case 25:
      retval = ( v_Is_Mandatory_Procedure == g_default_Is_Mandatory_Procedure );
      break;

      case 26:
      retval = ( v_Is_Mandatory_Source_Field == g_default_Is_Mandatory_Source_Field );
      break;

      case 27:
      retval = ( v_Is_Mandatory_View == g_default_Is_Mandatory_View );
      break;

      case 28:
      retval = ( v_Is_Other_Package == g_default_Is_Other_Package );
      break;

      case 29:
      retval = ( v_List == g_default_List );
      break;

      case 30:
      retval = ( v_Model == g_default_Model );
      break;

      case 31:
      retval = ( v_Modifier == g_default_Modifier );
      break;

      case 32:
      retval = ( v_Name == g_default_Name );
      break;

      case 33:
      retval = ( v_Notes == g_default_Notes );
      break;

      case 34:
      retval = ( v_Numeric == g_default_Numeric );
      break;

      case 35:
      retval = ( v_Other_Field == g_default_Other_Field );
      break;

      case 36:
      retval = ( v_Other_Field_2 == g_default_Other_Field_2 );
      break;

      case 37:
      retval = ( v_Other_Package == g_default_Other_Package );
      break;

      case 38:
      retval = ( v_Other_Package_Required == g_default_Other_Package_Required );
      break;

      case 39:
      retval = ( v_Other_Package_Type == g_default_Other_Package_Type );
      break;

      case 40:
      retval = ( v_Other_Source_Field == g_default_Other_Source_Field );
      break;

      case 41:
      retval = ( v_Package == g_default_Package );
      break;

      case 42:
      retval = ( v_Primitive == g_default_Primitive );
      break;

      case 43:
      retval = ( v_Procedure == g_default_Procedure );
      break;

      case 44:
      retval = ( v_Source_Field == g_default_Source_Field );
      break;

      case 45:
      retval = ( v_String == g_default_String );
      break;

      case 46:
      retval = ( v_Time == g_default_Time );
      break;

      case 47:
      retval = ( v_Use_Option == g_default_Use_Option );
      break;

      case 48:
      retval = ( v_Value == g_default_Value );
      break;

      case 49:
      retval = ( v_View == g_default_View );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_Package_Option::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start protect_equal)] 600880
   if( check_equal( get_obj( ).Installed( ), true ) )
      state |= ( c_state_uneditable | c_state_undeletable );
   // [(finish protect_equal)] 600880

   // [(start modifier_field_value)] 600882
   if( get_obj( ).Is_Other_Package( ) == true )
      state |= c_modifier_Is_Other_Package_Option;
   // [(finish modifier_field_value)] 600882

   // [(start modifier_field_value)] 600883
   if( get_obj( ).Is_Other_Package( ) == false )
      state |= c_modifier_Is_Not_Other_Package_Option;
   // [(finish modifier_field_value)] 600883

   // [(start modifier_field_value)] 600884
   if( get_obj( ).Primitive( ) == 0 ) // i.e. string
      state |= c_modifier_Is_String;
   // [(finish modifier_field_value)] 600884

   // [(start modifier_field_value)] 600885
   if( get_obj( ).Primitive( ) == 1 ) // i.e. datetime
      state |= c_modifier_Is_Datetime;
   // [(finish modifier_field_value)] 600885

   // [(start modifier_field_value)] 600886
   if( get_obj( ).Primitive( ) == 2 ) // i.e. date
      state |= c_modifier_Is_Date;
   // [(finish modifier_field_value)] 600886

   // [(start modifier_field_value)] 600887
   if( get_obj( ).Primitive( ) == 3 ) // i.e. time
      state |= c_modifier_Is_Time;
   // [(finish modifier_field_value)] 600887

   // [(start modifier_field_value)] 600888
   if( get_obj( ).Primitive( ) == 4 ) // i.e. numeric
      state |= c_modifier_Is_Numeric;
   // [(finish modifier_field_value)] 600888

   // [(start modifier_field_value)] 600889
   if( get_obj( ).Primitive( ) == 5 ) // i.e. int
      state |= c_modifier_Is_Int;
   // [(finish modifier_field_value)] 600889

   // [(start modifier_field_value)] 600890
   if( get_obj( ).Primitive( ) == 6 ) // i.e. bool
      state |= c_modifier_Is_Bool;
   // [(finish modifier_field_value)] 600890

   // [(start modifier_field_value)] 600891
   if( get_obj( ).Is_Class( ) == true )
      state |= c_modifier_Is_Class_Option;
   // [(finish modifier_field_value)] 600891

   // [(start modifier_field_value)] 600892
   if( get_obj( ).Is_Class( ) == false )
      state |= c_modifier_Is_Not_Class_Option;
   // [(finish modifier_field_value)] 600892

   // [(start modifier_field_value)] 600893a
   if( get_obj( ).Has_Field( ) == false )
      state |= c_modifier_Hide_Field;
   // [(finish modifier_field_value)] 600893a

   // [(start modifier_field_value)] 600893b
   if( get_obj( ).Has_Other_Field( ) == false )
      state |= c_modifier_Hide_Other_Field;
   // [(finish modifier_field_value)] 600893b

   // [(start modifier_field_value)] 600893c
   if( get_obj( ).Has_Other_Field_2( ) == false )
      state |= c_modifier_Hide_Other_Field_2;
   // [(finish modifier_field_value)] 600893c

   // [(start modifier_field_value)] 600893d
   if( get_obj( ).Has_Source_Field( ) == false )
      state |= c_modifier_Hide_Source_Field;
   // [(finish modifier_field_value)] 600893d

   // [(start modifier_field_value)] 600893e
   if( get_obj( ).Has_Other_Source_Field( ) == false )
      state |= c_modifier_Hide_Other_Source_Field;
   // [(finish modifier_field_value)] 600893e

   // [(start modifier_field_value)] 600894
   if( get_obj( ).Has_View( ) == false )
      state |= c_modifier_Hide_View;
   // [(finish modifier_field_value)] 600894

   // [(start modifier_field_value)] 600895
   if( get_obj( ).Has_List( ) == false )
      state |= c_modifier_Hide_List;
   // [(finish modifier_field_value)] 600895

   // [(start modifier_field_value)] 600896
   if( get_obj( ).Has_Modifier( ) == false )
      state |= c_modifier_Hide_Modifier;
   // [(finish modifier_field_value)] 600896

   // [(start modifier_field_value)] 600897
   if( get_obj( ).Has_Procedure( ) == false )
      state |= c_modifier_Hide_Procedure;
   // [(finish modifier_field_value)] 600897

   // [<start get_state>]
//nyi
   if( get_obj( ).Package( ).Actions( ).empty( )
    || ( get_obj( ).Name( ).find( "@" ) == 0 ) )
      state |= ( c_state_uneditable | c_state_undeletable );
   // [<finish get_state>]

   return state;
}

string Meta_Package_Option::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   if( state & c_modifier_Hide_Field )
      state_names += "|" + string( "Hide_Field" );
   if( state & c_modifier_Hide_List )
      state_names += "|" + string( "Hide_List" );
   if( state & c_modifier_Hide_Modifier )
      state_names += "|" + string( "Hide_Modifier" );
   if( state & c_modifier_Hide_Other_Field )
      state_names += "|" + string( "Hide_Other_Field" );
   if( state & c_modifier_Hide_Other_Field_2 )
      state_names += "|" + string( "Hide_Other_Field_2" );
   if( state & c_modifier_Hide_Other_Source_Field )
      state_names += "|" + string( "Hide_Other_Source_Field" );
   if( state & c_modifier_Hide_Procedure )
      state_names += "|" + string( "Hide_Procedure" );
   if( state & c_modifier_Hide_Source_Field )
      state_names += "|" + string( "Hide_Source_Field" );
   if( state & c_modifier_Hide_View )
      state_names += "|" + string( "Hide_View" );
   if( state & c_modifier_Is_Bool )
      state_names += "|" + string( "Is_Bool" );
   if( state & c_modifier_Is_Class_Option )
      state_names += "|" + string( "Is_Class_Option" );
   if( state & c_modifier_Is_Date )
      state_names += "|" + string( "Is_Date" );
   if( state & c_modifier_Is_Datetime )
      state_names += "|" + string( "Is_Datetime" );
   if( state & c_modifier_Is_Int )
      state_names += "|" + string( "Is_Int" );
   if( state & c_modifier_Is_Not_Class_Option )
      state_names += "|" + string( "Is_Not_Class_Option" );
   if( state & c_modifier_Is_Not_Other_Package_Option )
      state_names += "|" + string( "Is_Not_Other_Package_Option" );
   if( state & c_modifier_Is_Numeric )
      state_names += "|" + string( "Is_Numeric" );
   if( state & c_modifier_Is_Other_Package_Option )
      state_names += "|" + string( "Is_Other_Package_Option" );
   if( state & c_modifier_Is_String )
      state_names += "|" + string( "Is_String" );
   if( state & c_modifier_Is_Time )
      state_names += "|" + string( "Is_Time" );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_Package_Option::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Package_Option::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      impl_Class( "" );
   else if( ( field == c_field_id_Field ) || ( field == c_field_name_Field ) )
      impl_Field( "" );
   else if( ( field == c_field_id_List ) || ( field == c_field_name_List ) )
      impl_List( "" );
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      impl_Model( "" );
   else if( ( field == c_field_id_Modifier ) || ( field == c_field_name_Modifier ) )
      impl_Modifier( "" );
   else if( ( field == c_field_id_Other_Field ) || ( field == c_field_name_Other_Field ) )
      impl_Other_Field( "" );
   else if( ( field == c_field_id_Other_Field_2 ) || ( field == c_field_name_Other_Field_2 ) )
      impl_Other_Field_2( "" );
   else if( ( field == c_field_id_Other_Package ) || ( field == c_field_name_Other_Package ) )
      impl_Other_Package( "" );
   else if( ( field == c_field_id_Other_Source_Field ) || ( field == c_field_name_Other_Source_Field ) )
      impl_Other_Source_Field( "" );
   else if( ( field == c_field_id_Package ) || ( field == c_field_name_Package ) )
      impl_Package( "" );
   else if( ( field == c_field_id_Procedure ) || ( field == c_field_name_Procedure ) )
      impl_Procedure( "" );
   else if( ( field == c_field_id_Source_Field ) || ( field == c_field_name_Source_Field ) )
      impl_Source_Field( "" );
   else if( ( field == c_field_id_View ) || ( field == c_field_name_View ) )
      impl_View( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Package_Option::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      v_Class = value;
   else if( ( field == c_field_id_Field ) || ( field == c_field_name_Field ) )
      v_Field = value;
   else if( ( field == c_field_id_List ) || ( field == c_field_name_List ) )
      v_List = value;
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      v_Model = value;
   else if( ( field == c_field_id_Modifier ) || ( field == c_field_name_Modifier ) )
      v_Modifier = value;
   else if( ( field == c_field_id_Other_Field ) || ( field == c_field_name_Other_Field ) )
      v_Other_Field = value;
   else if( ( field == c_field_id_Other_Field_2 ) || ( field == c_field_name_Other_Field_2 ) )
      v_Other_Field_2 = value;
   else if( ( field == c_field_id_Other_Package ) || ( field == c_field_name_Other_Package ) )
      v_Other_Package = value;
   else if( ( field == c_field_id_Other_Source_Field ) || ( field == c_field_name_Other_Source_Field ) )
      v_Other_Source_Field = value;
   else if( ( field == c_field_id_Package ) || ( field == c_field_name_Package ) )
      v_Package = value;
   else if( ( field == c_field_id_Procedure ) || ( field == c_field_name_Procedure ) )
      v_Procedure = value;
   else if( ( field == c_field_id_Source_Field ) || ( field == c_field_name_Source_Field ) )
      v_Source_Field = value;
   else if( ( field == c_field_id_View ) || ( field == c_field_name_View ) )
      v_View = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Package_Option::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      return v_Class;
   else if( ( field == c_field_id_Field ) || ( field == c_field_name_Field ) )
      return v_Field;
   else if( ( field == c_field_id_List ) || ( field == c_field_name_List ) )
      return v_List;
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      return v_Model;
   else if( ( field == c_field_id_Modifier ) || ( field == c_field_name_Modifier ) )
      return v_Modifier;
   else if( ( field == c_field_id_Other_Field ) || ( field == c_field_name_Other_Field ) )
      return v_Other_Field;
   else if( ( field == c_field_id_Other_Field_2 ) || ( field == c_field_name_Other_Field_2 ) )
      return v_Other_Field_2;
   else if( ( field == c_field_id_Other_Package ) || ( field == c_field_name_Other_Package ) )
      return v_Other_Package;
   else if( ( field == c_field_id_Other_Source_Field ) || ( field == c_field_name_Other_Source_Field ) )
      return v_Other_Source_Field;
   else if( ( field == c_field_id_Package ) || ( field == c_field_name_Package ) )
      return v_Package;
   else if( ( field == c_field_id_Procedure ) || ( field == c_field_name_Procedure ) )
      return v_Procedure;
   else if( ( field == c_field_id_Source_Field ) || ( field == c_field_name_Source_Field ) )
      return v_Source_Field;
   else if( ( field == c_field_id_View ) || ( field == c_field_name_View ) )
      return v_View;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Package_Option::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Class, v_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Field, v_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_List, v_List ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Model, v_Model ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Modifier, v_Modifier ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Other_Field, v_Other_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Other_Field_2, v_Other_Field_2 ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Other_Package, v_Other_Package ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Other_Source_Field, v_Other_Source_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Package, v_Package ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Procedure, v_Procedure ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Field, v_Source_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_View, v_View ) );
}

void Meta_Package_Option::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_Package_Option::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Package_Option::impl::clear( )
{
   v_Actions = g_default_Actions;
   v_Date = g_default_Date;
   v_Datetime = g_default_Datetime;
   v_Has_Field = g_default_Has_Field;
   v_Has_List = g_default_Has_List;
   v_Has_Modifier = g_default_Has_Modifier;
   v_Has_Other_Field = g_default_Has_Other_Field;
   v_Has_Other_Field_2 = g_default_Has_Other_Field_2;
   v_Has_Other_Source_Field = g_default_Has_Other_Source_Field;
   v_Has_Procedure = g_default_Has_Procedure;
   v_Has_Source_Field = g_default_Has_Source_Field;
   v_Has_View = g_default_Has_View;
   v_Id = g_default_Id;
   v_Installed = g_default_Installed;
   v_Integer = g_default_Integer;
   v_Is_Class = g_default_Is_Class;
   v_Is_Mandatory_Class = g_default_Is_Mandatory_Class;
   v_Is_Mandatory_Field = g_default_Is_Mandatory_Field;
   v_Is_Mandatory_List = g_default_Is_Mandatory_List;
   v_Is_Mandatory_Modifier = g_default_Is_Mandatory_Modifier;
   v_Is_Mandatory_Other_Field = g_default_Is_Mandatory_Other_Field;
   v_Is_Mandatory_Other_Field_2 = g_default_Is_Mandatory_Other_Field_2;
   v_Is_Mandatory_Other_Source_Field = g_default_Is_Mandatory_Other_Source_Field;
   v_Is_Mandatory_Procedure = g_default_Is_Mandatory_Procedure;
   v_Is_Mandatory_Source_Field = g_default_Is_Mandatory_Source_Field;
   v_Is_Mandatory_View = g_default_Is_Mandatory_View;
   v_Is_Other_Package = g_default_Is_Other_Package;
   v_Name = g_default_Name;
   v_Notes = g_default_Notes;
   v_Numeric = g_default_Numeric;
   v_Other_Package_Required = g_default_Other_Package_Required;
   v_Other_Package_Type = g_default_Other_Package_Type;
   v_Primitive = g_default_Primitive;
   v_String = g_default_String;
   v_Time = g_default_Time;
   v_Use_Option = g_default_Use_Option;
   v_Value = g_default_Value;

   v_Class = string( );
   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   v_Field = string( );
   if( cp_Field )
      p_obj->setup_foreign_key( *cp_Field, v_Field );

   v_List = string( );
   if( cp_List )
      p_obj->setup_foreign_key( *cp_List, v_List );

   v_Model = string( );
   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   v_Modifier = string( );
   if( cp_Modifier )
      p_obj->setup_foreign_key( *cp_Modifier, v_Modifier );

   v_Other_Field = string( );
   if( cp_Other_Field )
      p_obj->setup_foreign_key( *cp_Other_Field, v_Other_Field );

   v_Other_Field_2 = string( );
   if( cp_Other_Field_2 )
      p_obj->setup_foreign_key( *cp_Other_Field_2, v_Other_Field_2 );

   v_Other_Package = string( );
   if( cp_Other_Package )
      p_obj->setup_foreign_key( *cp_Other_Package, v_Other_Package );

   v_Other_Source_Field = string( );
   if( cp_Other_Source_Field )
      p_obj->setup_foreign_key( *cp_Other_Source_Field, v_Other_Source_Field );

   v_Package = string( );
   if( cp_Package )
      p_obj->setup_foreign_key( *cp_Package, v_Package );

   v_Procedure = string( );
   if( cp_Procedure )
      p_obj->setup_foreign_key( *cp_Procedure, v_Procedure );

   v_Source_Field = string( );
   if( cp_Source_Field )
      p_obj->setup_foreign_key( *cp_Source_Field, v_Source_Field );

   v_View = string( );
   if( cp_View )
      p_obj->setup_foreign_key( *cp_View, v_View );
}

bool Meta_Package_Option::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Package_Option::impl::validate(
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

   if( v_Model.empty( ) && !value_will_be_provided( c_field_name_Model ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Model,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Model ) ) ) ) );

   if( v_Package.empty( ) && !value_will_be_provided( c_field_name_Package ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Package,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Package ) ) ) ) );

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

   if( !g_primitive_enum.count( v_Primitive ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Primitive,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Primitive ) ) ) ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Package_Option::impl::validate_set_fields(
 set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

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
}

void Meta_Package_Option::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   if( cp_Field )
      p_obj->setup_foreign_key( *cp_Field, v_Field );

   if( cp_List )
      p_obj->setup_foreign_key( *cp_List, v_List );

   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   if( cp_Modifier )
      p_obj->setup_foreign_key( *cp_Modifier, v_Modifier );

   if( cp_Other_Field )
      p_obj->setup_foreign_key( *cp_Other_Field, v_Other_Field );

   if( cp_Other_Field_2 )
      p_obj->setup_foreign_key( *cp_Other_Field_2, v_Other_Field_2 );

   if( cp_Other_Package )
      p_obj->setup_foreign_key( *cp_Other_Package, v_Other_Package );

   if( cp_Other_Source_Field )
      p_obj->setup_foreign_key( *cp_Other_Source_Field, v_Other_Source_Field );

   if( cp_Package )
      p_obj->setup_foreign_key( *cp_Package, v_Package );

   if( cp_Procedure )
      p_obj->setup_foreign_key( *cp_Procedure, v_Procedure );

   if( cp_Source_Field )
      p_obj->setup_foreign_key( *cp_Source_Field, v_Source_Field );

   if( cp_View )
      p_obj->setup_foreign_key( *cp_View, v_View );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_fetch>]
//nyi
   if( get_state( ) & c_state_uneditable || get_obj( ).Package( ).Installed( ) )
      get_obj( ).Actions( "" );
   else if( !get_obj( ).Is_Class( )
    && !get_obj( ).Is_Other_Package( ) && get_obj( ).Primitive( ) == e_primitive_bool )
   {
      if( get_obj( ).Use_Option( ) )
         get_obj( ).Actions( c_procedure_id_Clear );
      else
         get_obj( ).Actions( c_procedure_id_Select );
   }

   if( get_obj( ).Is_Class( ) )
      get_obj( ).Value( get_obj( ).Class( ).Name( ) );
   else if( get_obj( ).Is_Other_Package( ) )
      get_obj( ).Value( get_obj( ).Other_Package( ).Name( ) );
   else
   {
      switch( get_obj( ).Primitive( ) )
      {
         case e_primitive_string:
         get_obj( ).Value( get_obj( ).String( ) );
         break;

         case e_primitive_datetime:
         get_obj( ).Value( to_string( get_obj( ).Datetime( ) ) );
         break;

         case e_primitive_date:
         get_obj( ).Value( to_string( get_obj( ).Date( ) ) );
         break;

         case e_primitive_time:
         get_obj( ).Value( to_string( get_obj( ).Time( ) ) );
         break;

         case e_primitive_numeric:
         get_obj( ).Value( to_string( get_obj( ).Numeric( ) ) );
         break;

         case e_primitive_int:
         get_obj( ).Value( to_string( get_obj( ).Integer( ) ) );
         break;

         case e_primitive_bool:
         if( get_obj( ).Use_Option( ) )
            get_obj( ).Value( "yes" );
         else
            get_obj( ).Value( "---" );
         break;

         default:
         throw runtime_error( "unexpected primitive value #"
          + to_string( get_obj( ).Primitive( ) ) + "in Meta_Package_Option::after_fetch" );
      }
   }
   // [<finish after_fetch>]
}

void Meta_Package_Option::impl::completed_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start completed_fetch>]
   // [<finish completed_fetch>]
}

void Meta_Package_Option::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Package_Option::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_Package_Option::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start default_to_field)] 600881
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Model( ) == g_default_Model ) )
      get_obj( ).Model( get_obj( ).Package( ).Model( ) );
   // [(finish default_to_field)] 600881

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Package_Option::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_store>]
//nyi
   if( get_obj( ).Is_Other_Package( ) )
      get_obj( ).Use_Option( !is_null( get_obj( ).Other_Package( ) ) );
   // [<finish for_store>]
}

void Meta_Package_Option::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Package_Option::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Package_Option::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Package_Option::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Package_Option::impl::set_default_values( )
{
   clear( );
}

bool Meta_Package_Option::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
//nyi
   if( !get_obj( ).Package( ).Model( ).Type( )
    && ( get_obj( ).Name( ) == "@use_demo_data" ) )
      return true;
   // [<finish is_filtered>]

   return false;
}

void Meta_Package_Option::impl::get_required_transients( ) const
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

Meta_Package_Option::Meta_Package_Option( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Package_Option::~Meta_Package_Option( )
{
   cleanup( );
   delete p_impl;
}

const string& Meta_Package_Option::Actions( ) const
{
   return p_impl->impl_Actions( );
}

void Meta_Package_Option::Actions( const string& Actions )
{
   p_impl->impl_Actions( Actions );
}

const string& Meta_Package_Option::Date( ) const
{
   return p_impl->impl_Date( );
}

void Meta_Package_Option::Date( const string& Date )
{
   p_impl->impl_Date( Date );
}

const string& Meta_Package_Option::Datetime( ) const
{
   return p_impl->impl_Datetime( );
}

void Meta_Package_Option::Datetime( const string& Datetime )
{
   p_impl->impl_Datetime( Datetime );
}

bool Meta_Package_Option::Has_Field( ) const
{
   return p_impl->impl_Has_Field( );
}

void Meta_Package_Option::Has_Field( bool Has_Field )
{
   p_impl->impl_Has_Field( Has_Field );
}

bool Meta_Package_Option::Has_List( ) const
{
   return p_impl->impl_Has_List( );
}

void Meta_Package_Option::Has_List( bool Has_List )
{
   p_impl->impl_Has_List( Has_List );
}

bool Meta_Package_Option::Has_Modifier( ) const
{
   return p_impl->impl_Has_Modifier( );
}

void Meta_Package_Option::Has_Modifier( bool Has_Modifier )
{
   p_impl->impl_Has_Modifier( Has_Modifier );
}

bool Meta_Package_Option::Has_Other_Field( ) const
{
   return p_impl->impl_Has_Other_Field( );
}

void Meta_Package_Option::Has_Other_Field( bool Has_Other_Field )
{
   p_impl->impl_Has_Other_Field( Has_Other_Field );
}

bool Meta_Package_Option::Has_Other_Field_2( ) const
{
   return p_impl->impl_Has_Other_Field_2( );
}

void Meta_Package_Option::Has_Other_Field_2( bool Has_Other_Field_2 )
{
   p_impl->impl_Has_Other_Field_2( Has_Other_Field_2 );
}

bool Meta_Package_Option::Has_Other_Source_Field( ) const
{
   return p_impl->impl_Has_Other_Source_Field( );
}

void Meta_Package_Option::Has_Other_Source_Field( bool Has_Other_Source_Field )
{
   p_impl->impl_Has_Other_Source_Field( Has_Other_Source_Field );
}

bool Meta_Package_Option::Has_Procedure( ) const
{
   return p_impl->impl_Has_Procedure( );
}

void Meta_Package_Option::Has_Procedure( bool Has_Procedure )
{
   p_impl->impl_Has_Procedure( Has_Procedure );
}

bool Meta_Package_Option::Has_Source_Field( ) const
{
   return p_impl->impl_Has_Source_Field( );
}

void Meta_Package_Option::Has_Source_Field( bool Has_Source_Field )
{
   p_impl->impl_Has_Source_Field( Has_Source_Field );
}

bool Meta_Package_Option::Has_View( ) const
{
   return p_impl->impl_Has_View( );
}

void Meta_Package_Option::Has_View( bool Has_View )
{
   p_impl->impl_Has_View( Has_View );
}

const string& Meta_Package_Option::Id( ) const
{
   return p_impl->impl_Id( );
}

void Meta_Package_Option::Id( const string& Id )
{
   p_impl->impl_Id( Id );
}

bool Meta_Package_Option::Installed( ) const
{
   return p_impl->impl_Installed( );
}

void Meta_Package_Option::Installed( bool Installed )
{
   p_impl->impl_Installed( Installed );
}

int Meta_Package_Option::Integer( ) const
{
   return p_impl->impl_Integer( );
}

void Meta_Package_Option::Integer( int Integer )
{
   p_impl->impl_Integer( Integer );
}

bool Meta_Package_Option::Is_Class( ) const
{
   return p_impl->impl_Is_Class( );
}

void Meta_Package_Option::Is_Class( bool Is_Class )
{
   p_impl->impl_Is_Class( Is_Class );
}

bool Meta_Package_Option::Is_Mandatory_Class( ) const
{
   return p_impl->impl_Is_Mandatory_Class( );
}

void Meta_Package_Option::Is_Mandatory_Class( bool Is_Mandatory_Class )
{
   p_impl->impl_Is_Mandatory_Class( Is_Mandatory_Class );
}

bool Meta_Package_Option::Is_Mandatory_Field( ) const
{
   return p_impl->impl_Is_Mandatory_Field( );
}

void Meta_Package_Option::Is_Mandatory_Field( bool Is_Mandatory_Field )
{
   p_impl->impl_Is_Mandatory_Field( Is_Mandatory_Field );
}

bool Meta_Package_Option::Is_Mandatory_List( ) const
{
   return p_impl->impl_Is_Mandatory_List( );
}

void Meta_Package_Option::Is_Mandatory_List( bool Is_Mandatory_List )
{
   p_impl->impl_Is_Mandatory_List( Is_Mandatory_List );
}

bool Meta_Package_Option::Is_Mandatory_Modifier( ) const
{
   return p_impl->impl_Is_Mandatory_Modifier( );
}

void Meta_Package_Option::Is_Mandatory_Modifier( bool Is_Mandatory_Modifier )
{
   p_impl->impl_Is_Mandatory_Modifier( Is_Mandatory_Modifier );
}

bool Meta_Package_Option::Is_Mandatory_Other_Field( ) const
{
   return p_impl->impl_Is_Mandatory_Other_Field( );
}

void Meta_Package_Option::Is_Mandatory_Other_Field( bool Is_Mandatory_Other_Field )
{
   p_impl->impl_Is_Mandatory_Other_Field( Is_Mandatory_Other_Field );
}

bool Meta_Package_Option::Is_Mandatory_Other_Field_2( ) const
{
   return p_impl->impl_Is_Mandatory_Other_Field_2( );
}

void Meta_Package_Option::Is_Mandatory_Other_Field_2( bool Is_Mandatory_Other_Field_2 )
{
   p_impl->impl_Is_Mandatory_Other_Field_2( Is_Mandatory_Other_Field_2 );
}

bool Meta_Package_Option::Is_Mandatory_Other_Source_Field( ) const
{
   return p_impl->impl_Is_Mandatory_Other_Source_Field( );
}

void Meta_Package_Option::Is_Mandatory_Other_Source_Field( bool Is_Mandatory_Other_Source_Field )
{
   p_impl->impl_Is_Mandatory_Other_Source_Field( Is_Mandatory_Other_Source_Field );
}

bool Meta_Package_Option::Is_Mandatory_Procedure( ) const
{
   return p_impl->impl_Is_Mandatory_Procedure( );
}

void Meta_Package_Option::Is_Mandatory_Procedure( bool Is_Mandatory_Procedure )
{
   p_impl->impl_Is_Mandatory_Procedure( Is_Mandatory_Procedure );
}

bool Meta_Package_Option::Is_Mandatory_Source_Field( ) const
{
   return p_impl->impl_Is_Mandatory_Source_Field( );
}

void Meta_Package_Option::Is_Mandatory_Source_Field( bool Is_Mandatory_Source_Field )
{
   p_impl->impl_Is_Mandatory_Source_Field( Is_Mandatory_Source_Field );
}

bool Meta_Package_Option::Is_Mandatory_View( ) const
{
   return p_impl->impl_Is_Mandatory_View( );
}

void Meta_Package_Option::Is_Mandatory_View( bool Is_Mandatory_View )
{
   p_impl->impl_Is_Mandatory_View( Is_Mandatory_View );
}

bool Meta_Package_Option::Is_Other_Package( ) const
{
   return p_impl->impl_Is_Other_Package( );
}

void Meta_Package_Option::Is_Other_Package( bool Is_Other_Package )
{
   p_impl->impl_Is_Other_Package( Is_Other_Package );
}

const string& Meta_Package_Option::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_Package_Option::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

const string& Meta_Package_Option::Notes( ) const
{
   return p_impl->impl_Notes( );
}

void Meta_Package_Option::Notes( const string& Notes )
{
   p_impl->impl_Notes( Notes );
}

const string& Meta_Package_Option::Numeric( ) const
{
   return p_impl->impl_Numeric( );
}

void Meta_Package_Option::Numeric( const string& Numeric )
{
   p_impl->impl_Numeric( Numeric );
}

bool Meta_Package_Option::Other_Package_Required( ) const
{
   return p_impl->impl_Other_Package_Required( );
}

void Meta_Package_Option::Other_Package_Required( bool Other_Package_Required )
{
   p_impl->impl_Other_Package_Required( Other_Package_Required );
}

const string& Meta_Package_Option::Other_Package_Type( ) const
{
   return p_impl->impl_Other_Package_Type( );
}

void Meta_Package_Option::Other_Package_Type( const string& Other_Package_Type )
{
   p_impl->impl_Other_Package_Type( Other_Package_Type );
}

int Meta_Package_Option::Primitive( ) const
{
   return p_impl->impl_Primitive( );
}

void Meta_Package_Option::Primitive( int Primitive )
{
   p_impl->impl_Primitive( Primitive );
}

const string& Meta_Package_Option::String( ) const
{
   return p_impl->impl_String( );
}

void Meta_Package_Option::String( const string& String )
{
   p_impl->impl_String( String );
}

const string& Meta_Package_Option::Time( ) const
{
   return p_impl->impl_Time( );
}

void Meta_Package_Option::Time( const string& Time )
{
   p_impl->impl_Time( Time );
}

bool Meta_Package_Option::Use_Option( ) const
{
   return p_impl->impl_Use_Option( );
}

void Meta_Package_Option::Use_Option( bool Use_Option )
{
   p_impl->impl_Use_Option( Use_Option );
}

const string& Meta_Package_Option::Value( ) const
{
   return p_impl->impl_Value( );
}

void Meta_Package_Option::Value( const string& Value )
{
   p_impl->impl_Value( Value );
}

Meta_Class& Meta_Package_Option::Class( )
{
   return p_impl->impl_Class( );
}

const Meta_Class& Meta_Package_Option::Class( ) const
{
   return p_impl->impl_Class( );
}

void Meta_Package_Option::Class( const string& key )
{
   p_impl->impl_Class( key );
}

Meta_Field& Meta_Package_Option::Field( )
{
   return p_impl->impl_Field( );
}

const Meta_Field& Meta_Package_Option::Field( ) const
{
   return p_impl->impl_Field( );
}

void Meta_Package_Option::Field( const string& key )
{
   p_impl->impl_Field( key );
}

Meta_List& Meta_Package_Option::List( )
{
   return p_impl->impl_List( );
}

const Meta_List& Meta_Package_Option::List( ) const
{
   return p_impl->impl_List( );
}

void Meta_Package_Option::List( const string& key )
{
   p_impl->impl_List( key );
}

Meta_Model& Meta_Package_Option::Model( )
{
   return p_impl->impl_Model( );
}

const Meta_Model& Meta_Package_Option::Model( ) const
{
   return p_impl->impl_Model( );
}

void Meta_Package_Option::Model( const string& key )
{
   p_impl->impl_Model( key );
}

Meta_Modifier& Meta_Package_Option::Modifier( )
{
   return p_impl->impl_Modifier( );
}

const Meta_Modifier& Meta_Package_Option::Modifier( ) const
{
   return p_impl->impl_Modifier( );
}

void Meta_Package_Option::Modifier( const string& key )
{
   p_impl->impl_Modifier( key );
}

Meta_Field& Meta_Package_Option::Other_Field( )
{
   return p_impl->impl_Other_Field( );
}

const Meta_Field& Meta_Package_Option::Other_Field( ) const
{
   return p_impl->impl_Other_Field( );
}

void Meta_Package_Option::Other_Field( const string& key )
{
   p_impl->impl_Other_Field( key );
}

Meta_Field& Meta_Package_Option::Other_Field_2( )
{
   return p_impl->impl_Other_Field_2( );
}

const Meta_Field& Meta_Package_Option::Other_Field_2( ) const
{
   return p_impl->impl_Other_Field_2( );
}

void Meta_Package_Option::Other_Field_2( const string& key )
{
   p_impl->impl_Other_Field_2( key );
}

Meta_Package& Meta_Package_Option::Other_Package( )
{
   return p_impl->impl_Other_Package( );
}

const Meta_Package& Meta_Package_Option::Other_Package( ) const
{
   return p_impl->impl_Other_Package( );
}

void Meta_Package_Option::Other_Package( const string& key )
{
   p_impl->impl_Other_Package( key );
}

Meta_Field& Meta_Package_Option::Other_Source_Field( )
{
   return p_impl->impl_Other_Source_Field( );
}

const Meta_Field& Meta_Package_Option::Other_Source_Field( ) const
{
   return p_impl->impl_Other_Source_Field( );
}

void Meta_Package_Option::Other_Source_Field( const string& key )
{
   p_impl->impl_Other_Source_Field( key );
}

Meta_Package& Meta_Package_Option::Package( )
{
   return p_impl->impl_Package( );
}

const Meta_Package& Meta_Package_Option::Package( ) const
{
   return p_impl->impl_Package( );
}

void Meta_Package_Option::Package( const string& key )
{
   p_impl->impl_Package( key );
}

Meta_Procedure& Meta_Package_Option::Procedure( )
{
   return p_impl->impl_Procedure( );
}

const Meta_Procedure& Meta_Package_Option::Procedure( ) const
{
   return p_impl->impl_Procedure( );
}

void Meta_Package_Option::Procedure( const string& key )
{
   p_impl->impl_Procedure( key );
}

Meta_Field& Meta_Package_Option::Source_Field( )
{
   return p_impl->impl_Source_Field( );
}

const Meta_Field& Meta_Package_Option::Source_Field( ) const
{
   return p_impl->impl_Source_Field( );
}

void Meta_Package_Option::Source_Field( const string& key )
{
   p_impl->impl_Source_Field( key );
}

Meta_View& Meta_Package_Option::View( )
{
   return p_impl->impl_View( );
}

const Meta_View& Meta_Package_Option::View( ) const
{
   return p_impl->impl_View( );
}

void Meta_Package_Option::View( const string& key )
{
   p_impl->impl_View( key );
}

void Meta_Package_Option::Clear( )
{
   p_impl->impl_Clear( );
}

void Meta_Package_Option::Edit( )
{
   p_impl->impl_Edit( );
}

void Meta_Package_Option::Select( )
{
   p_impl->impl_Select( );
}

string Meta_Package_Option::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Package_Option::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_Package_Option::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_Package_Option::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_Package_Option::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_Package_Option::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_Package_Option::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_Package_Option::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_Package_Option::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_Package_Option::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Package_Option::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_Package_Option::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Package_Option::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Package_Option::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Package_Option::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Package_Option::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Package_Option::clear( )
{
   p_impl->clear( );
}

void Meta_Package_Option::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Package_Option::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Package_Option::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Package_Option::completed_fetch( bool skip_set_original )
{
   p_impl->completed_fetch( skip_set_original );
}

void Meta_Package_Option::at_create( )
{
   p_impl->at_create( );
}

void Meta_Package_Option::post_init( )
{
   p_impl->post_init( );
}

void Meta_Package_Option::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Package_Option::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Package_Option::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Package_Option::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Package_Option::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Package_Option::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Package_Option::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Package_Option::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Package_Option::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( 0 );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Actions )
   {
      p_id = c_field_id_Actions;

      if( p_type_name )
         *p_type_name = "string";

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
   else if( name == c_field_name_Date )
   {
      p_id = c_field_id_Date;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Datetime )
   {
      p_id = c_field_id_Datetime;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Field )
   {
      p_id = c_field_id_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Has_Field )
   {
      p_id = c_field_id_Has_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Has_List )
   {
      p_id = c_field_id_Has_List;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Has_Modifier )
   {
      p_id = c_field_id_Has_Modifier;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Has_Other_Field )
   {
      p_id = c_field_id_Has_Other_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Has_Other_Field_2 )
   {
      p_id = c_field_id_Has_Other_Field_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Has_Other_Source_Field )
   {
      p_id = c_field_id_Has_Other_Source_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Has_Procedure )
   {
      p_id = c_field_id_Has_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Has_Source_Field )
   {
      p_id = c_field_id_Has_Source_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Has_View )
   {
      p_id = c_field_id_Has_View;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Id )
   {
      p_id = c_field_id_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Installed )
   {
      p_id = c_field_id_Installed;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Integer )
   {
      p_id = c_field_id_Integer;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Class )
   {
      p_id = c_field_id_Is_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Mandatory_Class )
   {
      p_id = c_field_id_Is_Mandatory_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Mandatory_Field )
   {
      p_id = c_field_id_Is_Mandatory_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Mandatory_List )
   {
      p_id = c_field_id_Is_Mandatory_List;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Mandatory_Modifier )
   {
      p_id = c_field_id_Is_Mandatory_Modifier;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Mandatory_Other_Field )
   {
      p_id = c_field_id_Is_Mandatory_Other_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Mandatory_Other_Field_2 )
   {
      p_id = c_field_id_Is_Mandatory_Other_Field_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Mandatory_Other_Source_Field )
   {
      p_id = c_field_id_Is_Mandatory_Other_Source_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Mandatory_Procedure )
   {
      p_id = c_field_id_Is_Mandatory_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Mandatory_Source_Field )
   {
      p_id = c_field_id_Is_Mandatory_Source_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Mandatory_View )
   {
      p_id = c_field_id_Is_Mandatory_View;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Other_Package )
   {
      p_id = c_field_id_Is_Other_Package;

      if( p_type_name )
         *p_type_name = "bool";

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
   else if( name == c_field_name_Model )
   {
      p_id = c_field_id_Model;

      if( p_type_name )
         *p_type_name = "Meta_Model";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Modifier )
   {
      p_id = c_field_id_Modifier;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

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
   else if( name == c_field_name_Notes )
   {
      p_id = c_field_id_Notes;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Numeric )
   {
      p_id = c_field_id_Numeric;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Field )
   {
      p_id = c_field_id_Other_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Field_2 )
   {
      p_id = c_field_id_Other_Field_2;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Package )
   {
      p_id = c_field_id_Other_Package;

      if( p_type_name )
         *p_type_name = "Meta_Package";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Package_Required )
   {
      p_id = c_field_id_Other_Package_Required;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Other_Package_Type )
   {
      p_id = c_field_id_Other_Package_Type;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Source_Field )
   {
      p_id = c_field_id_Other_Source_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Package )
   {
      p_id = c_field_id_Package;

      if( p_type_name )
         *p_type_name = "Meta_Package";

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
   else if( name == c_field_name_Procedure )
   {
      p_id = c_field_id_Procedure;

      if( p_type_name )
         *p_type_name = "Meta_Procedure";

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
   else if( name == c_field_name_String )
   {
      p_id = c_field_id_String;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Time )
   {
      p_id = c_field_id_Time;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Use_Option )
   {
      p_id = c_field_id_Use_Option;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Value )
   {
      p_id = c_field_id_Value;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
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

const char* Meta_Package_Option::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( 0 );

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Actions )
   {
      p_name = c_field_name_Actions;

      if( p_type_name )
         *p_type_name = "string";

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
   else if( id == c_field_id_Date )
   {
      p_name = c_field_name_Date;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Datetime )
   {
      p_name = c_field_name_Datetime;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Field )
   {
      p_name = c_field_name_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Has_Field )
   {
      p_name = c_field_name_Has_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Has_List )
   {
      p_name = c_field_name_Has_List;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Has_Modifier )
   {
      p_name = c_field_name_Has_Modifier;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Has_Other_Field )
   {
      p_name = c_field_name_Has_Other_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Has_Other_Field_2 )
   {
      p_name = c_field_name_Has_Other_Field_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Has_Other_Source_Field )
   {
      p_name = c_field_name_Has_Other_Source_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Has_Procedure )
   {
      p_name = c_field_name_Has_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Has_Source_Field )
   {
      p_name = c_field_name_Has_Source_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Has_View )
   {
      p_name = c_field_name_Has_View;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Id )
   {
      p_name = c_field_name_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Installed )
   {
      p_name = c_field_name_Installed;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Integer )
   {
      p_name = c_field_name_Integer;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Class )
   {
      p_name = c_field_name_Is_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Mandatory_Class )
   {
      p_name = c_field_name_Is_Mandatory_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Mandatory_Field )
   {
      p_name = c_field_name_Is_Mandatory_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Mandatory_List )
   {
      p_name = c_field_name_Is_Mandatory_List;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Mandatory_Modifier )
   {
      p_name = c_field_name_Is_Mandatory_Modifier;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Mandatory_Other_Field )
   {
      p_name = c_field_name_Is_Mandatory_Other_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Mandatory_Other_Field_2 )
   {
      p_name = c_field_name_Is_Mandatory_Other_Field_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Mandatory_Other_Source_Field )
   {
      p_name = c_field_name_Is_Mandatory_Other_Source_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Mandatory_Procedure )
   {
      p_name = c_field_name_Is_Mandatory_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Mandatory_Source_Field )
   {
      p_name = c_field_name_Is_Mandatory_Source_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Mandatory_View )
   {
      p_name = c_field_name_Is_Mandatory_View;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Other_Package )
   {
      p_name = c_field_name_Is_Other_Package;

      if( p_type_name )
         *p_type_name = "bool";

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
   else if( id == c_field_id_Model )
   {
      p_name = c_field_name_Model;

      if( p_type_name )
         *p_type_name = "Meta_Model";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Modifier )
   {
      p_name = c_field_name_Modifier;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

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
   else if( id == c_field_id_Notes )
   {
      p_name = c_field_name_Notes;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Numeric )
   {
      p_name = c_field_name_Numeric;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Field )
   {
      p_name = c_field_name_Other_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Field_2 )
   {
      p_name = c_field_name_Other_Field_2;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Package )
   {
      p_name = c_field_name_Other_Package;

      if( p_type_name )
         *p_type_name = "Meta_Package";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Package_Required )
   {
      p_name = c_field_name_Other_Package_Required;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Other_Package_Type )
   {
      p_name = c_field_name_Other_Package_Type;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Source_Field )
   {
      p_name = c_field_name_Other_Source_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Package )
   {
      p_name = c_field_name_Package;

      if( p_type_name )
         *p_type_name = "Meta_Package";

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
   else if( id == c_field_id_Procedure )
   {
      p_name = c_field_name_Procedure;

      if( p_type_name )
         *p_type_name = "Meta_Procedure";

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
   else if( id == c_field_id_String )
   {
      p_name = c_field_name_String;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Time )
   {
      p_name = c_field_name_Time;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Use_Option )
   {
      p_name = c_field_name_Use_Option;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Value )
   {
      p_name = c_field_name_Value;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
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

string& Meta_Package_Option::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_Package_Option::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_Package_Option::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_Package_Option::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_Package_Option::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_Package_Option::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Package_Option::get_field_uom_symbol( const string& id_or_name ) const
{
   string uom_symbol;

   string name;
   pair< string, string > next;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_uom_symbol" );
   if( ( id_or_name == c_field_id_Actions ) || ( id_or_name == c_field_name_Actions ) )
   {
      name = string( c_field_display_name_Actions );
      get_module_string( c_field_display_name_Actions, &next );
   }
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
   {
      name = string( c_field_display_name_Class );
      get_module_string( c_field_display_name_Class, &next );
   }
   if( ( id_or_name == c_field_id_Date ) || ( id_or_name == c_field_name_Date ) )
   {
      name = string( c_field_display_name_Date );
      get_module_string( c_field_display_name_Date, &next );
   }
   if( ( id_or_name == c_field_id_Datetime ) || ( id_or_name == c_field_name_Datetime ) )
   {
      name = string( c_field_display_name_Datetime );
      get_module_string( c_field_display_name_Datetime, &next );
   }
   if( ( id_or_name == c_field_id_Field ) || ( id_or_name == c_field_name_Field ) )
   {
      name = string( c_field_display_name_Field );
      get_module_string( c_field_display_name_Field, &next );
   }
   if( ( id_or_name == c_field_id_Has_Field ) || ( id_or_name == c_field_name_Has_Field ) )
   {
      name = string( c_field_display_name_Has_Field );
      get_module_string( c_field_display_name_Has_Field, &next );
   }
   if( ( id_or_name == c_field_id_Has_List ) || ( id_or_name == c_field_name_Has_List ) )
   {
      name = string( c_field_display_name_Has_List );
      get_module_string( c_field_display_name_Has_List, &next );
   }
   if( ( id_or_name == c_field_id_Has_Modifier ) || ( id_or_name == c_field_name_Has_Modifier ) )
   {
      name = string( c_field_display_name_Has_Modifier );
      get_module_string( c_field_display_name_Has_Modifier, &next );
   }
   if( ( id_or_name == c_field_id_Has_Other_Field ) || ( id_or_name == c_field_name_Has_Other_Field ) )
   {
      name = string( c_field_display_name_Has_Other_Field );
      get_module_string( c_field_display_name_Has_Other_Field, &next );
   }
   if( ( id_or_name == c_field_id_Has_Other_Field_2 ) || ( id_or_name == c_field_name_Has_Other_Field_2 ) )
   {
      name = string( c_field_display_name_Has_Other_Field_2 );
      get_module_string( c_field_display_name_Has_Other_Field_2, &next );
   }
   if( ( id_or_name == c_field_id_Has_Other_Source_Field ) || ( id_or_name == c_field_name_Has_Other_Source_Field ) )
   {
      name = string( c_field_display_name_Has_Other_Source_Field );
      get_module_string( c_field_display_name_Has_Other_Source_Field, &next );
   }
   if( ( id_or_name == c_field_id_Has_Procedure ) || ( id_or_name == c_field_name_Has_Procedure ) )
   {
      name = string( c_field_display_name_Has_Procedure );
      get_module_string( c_field_display_name_Has_Procedure, &next );
   }
   if( ( id_or_name == c_field_id_Has_Source_Field ) || ( id_or_name == c_field_name_Has_Source_Field ) )
   {
      name = string( c_field_display_name_Has_Source_Field );
      get_module_string( c_field_display_name_Has_Source_Field, &next );
   }
   if( ( id_or_name == c_field_id_Has_View ) || ( id_or_name == c_field_name_Has_View ) )
   {
      name = string( c_field_display_name_Has_View );
      get_module_string( c_field_display_name_Has_View, &next );
   }
   if( ( id_or_name == c_field_id_Id ) || ( id_or_name == c_field_name_Id ) )
   {
      name = string( c_field_display_name_Id );
      get_module_string( c_field_display_name_Id, &next );
   }
   if( ( id_or_name == c_field_id_Installed ) || ( id_or_name == c_field_name_Installed ) )
   {
      name = string( c_field_display_name_Installed );
      get_module_string( c_field_display_name_Installed, &next );
   }
   if( ( id_or_name == c_field_id_Integer ) || ( id_or_name == c_field_name_Integer ) )
   {
      name = string( c_field_display_name_Integer );
      get_module_string( c_field_display_name_Integer, &next );
   }
   if( ( id_or_name == c_field_id_Is_Class ) || ( id_or_name == c_field_name_Is_Class ) )
   {
      name = string( c_field_display_name_Is_Class );
      get_module_string( c_field_display_name_Is_Class, &next );
   }
   if( ( id_or_name == c_field_id_Is_Mandatory_Class ) || ( id_or_name == c_field_name_Is_Mandatory_Class ) )
   {
      name = string( c_field_display_name_Is_Mandatory_Class );
      get_module_string( c_field_display_name_Is_Mandatory_Class, &next );
   }
   if( ( id_or_name == c_field_id_Is_Mandatory_Field ) || ( id_or_name == c_field_name_Is_Mandatory_Field ) )
   {
      name = string( c_field_display_name_Is_Mandatory_Field );
      get_module_string( c_field_display_name_Is_Mandatory_Field, &next );
   }
   if( ( id_or_name == c_field_id_Is_Mandatory_List ) || ( id_or_name == c_field_name_Is_Mandatory_List ) )
   {
      name = string( c_field_display_name_Is_Mandatory_List );
      get_module_string( c_field_display_name_Is_Mandatory_List, &next );
   }
   if( ( id_or_name == c_field_id_Is_Mandatory_Modifier ) || ( id_or_name == c_field_name_Is_Mandatory_Modifier ) )
   {
      name = string( c_field_display_name_Is_Mandatory_Modifier );
      get_module_string( c_field_display_name_Is_Mandatory_Modifier, &next );
   }
   if( ( id_or_name == c_field_id_Is_Mandatory_Other_Field ) || ( id_or_name == c_field_name_Is_Mandatory_Other_Field ) )
   {
      name = string( c_field_display_name_Is_Mandatory_Other_Field );
      get_module_string( c_field_display_name_Is_Mandatory_Other_Field, &next );
   }
   if( ( id_or_name == c_field_id_Is_Mandatory_Other_Field_2 ) || ( id_or_name == c_field_name_Is_Mandatory_Other_Field_2 ) )
   {
      name = string( c_field_display_name_Is_Mandatory_Other_Field_2 );
      get_module_string( c_field_display_name_Is_Mandatory_Other_Field_2, &next );
   }
   if( ( id_or_name == c_field_id_Is_Mandatory_Other_Source_Field ) || ( id_or_name == c_field_name_Is_Mandatory_Other_Source_Field ) )
   {
      name = string( c_field_display_name_Is_Mandatory_Other_Source_Field );
      get_module_string( c_field_display_name_Is_Mandatory_Other_Source_Field, &next );
   }
   if( ( id_or_name == c_field_id_Is_Mandatory_Procedure ) || ( id_or_name == c_field_name_Is_Mandatory_Procedure ) )
   {
      name = string( c_field_display_name_Is_Mandatory_Procedure );
      get_module_string( c_field_display_name_Is_Mandatory_Procedure, &next );
   }
   if( ( id_or_name == c_field_id_Is_Mandatory_Source_Field ) || ( id_or_name == c_field_name_Is_Mandatory_Source_Field ) )
   {
      name = string( c_field_display_name_Is_Mandatory_Source_Field );
      get_module_string( c_field_display_name_Is_Mandatory_Source_Field, &next );
   }
   if( ( id_or_name == c_field_id_Is_Mandatory_View ) || ( id_or_name == c_field_name_Is_Mandatory_View ) )
   {
      name = string( c_field_display_name_Is_Mandatory_View );
      get_module_string( c_field_display_name_Is_Mandatory_View, &next );
   }
   if( ( id_or_name == c_field_id_Is_Other_Package ) || ( id_or_name == c_field_name_Is_Other_Package ) )
   {
      name = string( c_field_display_name_Is_Other_Package );
      get_module_string( c_field_display_name_Is_Other_Package, &next );
   }
   if( ( id_or_name == c_field_id_List ) || ( id_or_name == c_field_name_List ) )
   {
      name = string( c_field_display_name_List );
      get_module_string( c_field_display_name_List, &next );
   }
   if( ( id_or_name == c_field_id_Model ) || ( id_or_name == c_field_name_Model ) )
   {
      name = string( c_field_display_name_Model );
      get_module_string( c_field_display_name_Model, &next );
   }
   if( ( id_or_name == c_field_id_Modifier ) || ( id_or_name == c_field_name_Modifier ) )
   {
      name = string( c_field_display_name_Modifier );
      get_module_string( c_field_display_name_Modifier, &next );
   }
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
   {
      name = string( c_field_display_name_Name );
      get_module_string( c_field_display_name_Name, &next );
   }
   if( ( id_or_name == c_field_id_Notes ) || ( id_or_name == c_field_name_Notes ) )
   {
      name = string( c_field_display_name_Notes );
      get_module_string( c_field_display_name_Notes, &next );
   }
   if( ( id_or_name == c_field_id_Numeric ) || ( id_or_name == c_field_name_Numeric ) )
   {
      name = string( c_field_display_name_Numeric );
      get_module_string( c_field_display_name_Numeric, &next );
   }
   if( ( id_or_name == c_field_id_Other_Field ) || ( id_or_name == c_field_name_Other_Field ) )
   {
      name = string( c_field_display_name_Other_Field );
      get_module_string( c_field_display_name_Other_Field, &next );
   }
   if( ( id_or_name == c_field_id_Other_Field_2 ) || ( id_or_name == c_field_name_Other_Field_2 ) )
   {
      name = string( c_field_display_name_Other_Field_2 );
      get_module_string( c_field_display_name_Other_Field_2, &next );
   }
   if( ( id_or_name == c_field_id_Other_Package ) || ( id_or_name == c_field_name_Other_Package ) )
   {
      name = string( c_field_display_name_Other_Package );
      get_module_string( c_field_display_name_Other_Package, &next );
   }
   if( ( id_or_name == c_field_id_Other_Package_Required ) || ( id_or_name == c_field_name_Other_Package_Required ) )
   {
      name = string( c_field_display_name_Other_Package_Required );
      get_module_string( c_field_display_name_Other_Package_Required, &next );
   }
   if( ( id_or_name == c_field_id_Other_Package_Type ) || ( id_or_name == c_field_name_Other_Package_Type ) )
   {
      name = string( c_field_display_name_Other_Package_Type );
      get_module_string( c_field_display_name_Other_Package_Type, &next );
   }
   if( ( id_or_name == c_field_id_Other_Source_Field ) || ( id_or_name == c_field_name_Other_Source_Field ) )
   {
      name = string( c_field_display_name_Other_Source_Field );
      get_module_string( c_field_display_name_Other_Source_Field, &next );
   }
   if( ( id_or_name == c_field_id_Package ) || ( id_or_name == c_field_name_Package ) )
   {
      name = string( c_field_display_name_Package );
      get_module_string( c_field_display_name_Package, &next );
   }
   if( ( id_or_name == c_field_id_Primitive ) || ( id_or_name == c_field_name_Primitive ) )
   {
      name = string( c_field_display_name_Primitive );
      get_module_string( c_field_display_name_Primitive, &next );
   }
   if( ( id_or_name == c_field_id_Procedure ) || ( id_or_name == c_field_name_Procedure ) )
   {
      name = string( c_field_display_name_Procedure );
      get_module_string( c_field_display_name_Procedure, &next );
   }
   if( ( id_or_name == c_field_id_Source_Field ) || ( id_or_name == c_field_name_Source_Field ) )
   {
      name = string( c_field_display_name_Source_Field );
      get_module_string( c_field_display_name_Source_Field, &next );
   }
   if( ( id_or_name == c_field_id_String ) || ( id_or_name == c_field_name_String ) )
   {
      name = string( c_field_display_name_String );
      get_module_string( c_field_display_name_String, &next );
   }
   if( ( id_or_name == c_field_id_Time ) || ( id_or_name == c_field_name_Time ) )
   {
      name = string( c_field_display_name_Time );
      get_module_string( c_field_display_name_Time, &next );
   }
   if( ( id_or_name == c_field_id_Use_Option ) || ( id_or_name == c_field_name_Use_Option ) )
   {
      name = string( c_field_display_name_Use_Option );
      get_module_string( c_field_display_name_Use_Option, &next );
   }
   if( ( id_or_name == c_field_id_Value ) || ( id_or_name == c_field_name_Value ) )
   {
      name = string( c_field_display_name_Value );
      get_module_string( c_field_display_name_Value, &next );
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

string Meta_Package_Option::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Actions ) || ( id_or_name == c_field_name_Actions ) )
      display_name = get_module_string( c_field_display_name_Actions );
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
      display_name = get_module_string( c_field_display_name_Class );
   if( ( id_or_name == c_field_id_Date ) || ( id_or_name == c_field_name_Date ) )
      display_name = get_module_string( c_field_display_name_Date );
   if( ( id_or_name == c_field_id_Datetime ) || ( id_or_name == c_field_name_Datetime ) )
      display_name = get_module_string( c_field_display_name_Datetime );
   if( ( id_or_name == c_field_id_Field ) || ( id_or_name == c_field_name_Field ) )
      display_name = get_module_string( c_field_display_name_Field );
   if( ( id_or_name == c_field_id_Has_Field ) || ( id_or_name == c_field_name_Has_Field ) )
      display_name = get_module_string( c_field_display_name_Has_Field );
   if( ( id_or_name == c_field_id_Has_List ) || ( id_or_name == c_field_name_Has_List ) )
      display_name = get_module_string( c_field_display_name_Has_List );
   if( ( id_or_name == c_field_id_Has_Modifier ) || ( id_or_name == c_field_name_Has_Modifier ) )
      display_name = get_module_string( c_field_display_name_Has_Modifier );
   if( ( id_or_name == c_field_id_Has_Other_Field ) || ( id_or_name == c_field_name_Has_Other_Field ) )
      display_name = get_module_string( c_field_display_name_Has_Other_Field );
   if( ( id_or_name == c_field_id_Has_Other_Field_2 ) || ( id_or_name == c_field_name_Has_Other_Field_2 ) )
      display_name = get_module_string( c_field_display_name_Has_Other_Field_2 );
   if( ( id_or_name == c_field_id_Has_Other_Source_Field ) || ( id_or_name == c_field_name_Has_Other_Source_Field ) )
      display_name = get_module_string( c_field_display_name_Has_Other_Source_Field );
   if( ( id_or_name == c_field_id_Has_Procedure ) || ( id_or_name == c_field_name_Has_Procedure ) )
      display_name = get_module_string( c_field_display_name_Has_Procedure );
   if( ( id_or_name == c_field_id_Has_Source_Field ) || ( id_or_name == c_field_name_Has_Source_Field ) )
      display_name = get_module_string( c_field_display_name_Has_Source_Field );
   if( ( id_or_name == c_field_id_Has_View ) || ( id_or_name == c_field_name_Has_View ) )
      display_name = get_module_string( c_field_display_name_Has_View );
   if( ( id_or_name == c_field_id_Id ) || ( id_or_name == c_field_name_Id ) )
      display_name = get_module_string( c_field_display_name_Id );
   if( ( id_or_name == c_field_id_Installed ) || ( id_or_name == c_field_name_Installed ) )
      display_name = get_module_string( c_field_display_name_Installed );
   if( ( id_or_name == c_field_id_Integer ) || ( id_or_name == c_field_name_Integer ) )
      display_name = get_module_string( c_field_display_name_Integer );
   if( ( id_or_name == c_field_id_Is_Class ) || ( id_or_name == c_field_name_Is_Class ) )
      display_name = get_module_string( c_field_display_name_Is_Class );
   if( ( id_or_name == c_field_id_Is_Mandatory_Class ) || ( id_or_name == c_field_name_Is_Mandatory_Class ) )
      display_name = get_module_string( c_field_display_name_Is_Mandatory_Class );
   if( ( id_or_name == c_field_id_Is_Mandatory_Field ) || ( id_or_name == c_field_name_Is_Mandatory_Field ) )
      display_name = get_module_string( c_field_display_name_Is_Mandatory_Field );
   if( ( id_or_name == c_field_id_Is_Mandatory_List ) || ( id_or_name == c_field_name_Is_Mandatory_List ) )
      display_name = get_module_string( c_field_display_name_Is_Mandatory_List );
   if( ( id_or_name == c_field_id_Is_Mandatory_Modifier ) || ( id_or_name == c_field_name_Is_Mandatory_Modifier ) )
      display_name = get_module_string( c_field_display_name_Is_Mandatory_Modifier );
   if( ( id_or_name == c_field_id_Is_Mandatory_Other_Field ) || ( id_or_name == c_field_name_Is_Mandatory_Other_Field ) )
      display_name = get_module_string( c_field_display_name_Is_Mandatory_Other_Field );
   if( ( id_or_name == c_field_id_Is_Mandatory_Other_Field_2 ) || ( id_or_name == c_field_name_Is_Mandatory_Other_Field_2 ) )
      display_name = get_module_string( c_field_display_name_Is_Mandatory_Other_Field_2 );
   if( ( id_or_name == c_field_id_Is_Mandatory_Other_Source_Field ) || ( id_or_name == c_field_name_Is_Mandatory_Other_Source_Field ) )
      display_name = get_module_string( c_field_display_name_Is_Mandatory_Other_Source_Field );
   if( ( id_or_name == c_field_id_Is_Mandatory_Procedure ) || ( id_or_name == c_field_name_Is_Mandatory_Procedure ) )
      display_name = get_module_string( c_field_display_name_Is_Mandatory_Procedure );
   if( ( id_or_name == c_field_id_Is_Mandatory_Source_Field ) || ( id_or_name == c_field_name_Is_Mandatory_Source_Field ) )
      display_name = get_module_string( c_field_display_name_Is_Mandatory_Source_Field );
   if( ( id_or_name == c_field_id_Is_Mandatory_View ) || ( id_or_name == c_field_name_Is_Mandatory_View ) )
      display_name = get_module_string( c_field_display_name_Is_Mandatory_View );
   if( ( id_or_name == c_field_id_Is_Other_Package ) || ( id_or_name == c_field_name_Is_Other_Package ) )
      display_name = get_module_string( c_field_display_name_Is_Other_Package );
   if( ( id_or_name == c_field_id_List ) || ( id_or_name == c_field_name_List ) )
      display_name = get_module_string( c_field_display_name_List );
   if( ( id_or_name == c_field_id_Model ) || ( id_or_name == c_field_name_Model ) )
      display_name = get_module_string( c_field_display_name_Model );
   if( ( id_or_name == c_field_id_Modifier ) || ( id_or_name == c_field_name_Modifier ) )
      display_name = get_module_string( c_field_display_name_Modifier );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
      display_name = get_module_string( c_field_display_name_Name );
   if( ( id_or_name == c_field_id_Notes ) || ( id_or_name == c_field_name_Notes ) )
      display_name = get_module_string( c_field_display_name_Notes );
   if( ( id_or_name == c_field_id_Numeric ) || ( id_or_name == c_field_name_Numeric ) )
      display_name = get_module_string( c_field_display_name_Numeric );
   if( ( id_or_name == c_field_id_Other_Field ) || ( id_or_name == c_field_name_Other_Field ) )
      display_name = get_module_string( c_field_display_name_Other_Field );
   if( ( id_or_name == c_field_id_Other_Field_2 ) || ( id_or_name == c_field_name_Other_Field_2 ) )
      display_name = get_module_string( c_field_display_name_Other_Field_2 );
   if( ( id_or_name == c_field_id_Other_Package ) || ( id_or_name == c_field_name_Other_Package ) )
      display_name = get_module_string( c_field_display_name_Other_Package );
   if( ( id_or_name == c_field_id_Other_Package_Required ) || ( id_or_name == c_field_name_Other_Package_Required ) )
      display_name = get_module_string( c_field_display_name_Other_Package_Required );
   if( ( id_or_name == c_field_id_Other_Package_Type ) || ( id_or_name == c_field_name_Other_Package_Type ) )
      display_name = get_module_string( c_field_display_name_Other_Package_Type );
   if( ( id_or_name == c_field_id_Other_Source_Field ) || ( id_or_name == c_field_name_Other_Source_Field ) )
      display_name = get_module_string( c_field_display_name_Other_Source_Field );
   if( ( id_or_name == c_field_id_Package ) || ( id_or_name == c_field_name_Package ) )
      display_name = get_module_string( c_field_display_name_Package );
   if( ( id_or_name == c_field_id_Primitive ) || ( id_or_name == c_field_name_Primitive ) )
      display_name = get_module_string( c_field_display_name_Primitive );
   if( ( id_or_name == c_field_id_Procedure ) || ( id_or_name == c_field_name_Procedure ) )
      display_name = get_module_string( c_field_display_name_Procedure );
   if( ( id_or_name == c_field_id_Source_Field ) || ( id_or_name == c_field_name_Source_Field ) )
      display_name = get_module_string( c_field_display_name_Source_Field );
   if( ( id_or_name == c_field_id_String ) || ( id_or_name == c_field_name_String ) )
      display_name = get_module_string( c_field_display_name_String );
   if( ( id_or_name == c_field_id_Time ) || ( id_or_name == c_field_name_Time ) )
      display_name = get_module_string( c_field_display_name_Time );
   if( ( id_or_name == c_field_id_Use_Option ) || ( id_or_name == c_field_name_Use_Option ) )
      display_name = get_module_string( c_field_display_name_Use_Option );
   if( ( id_or_name == c_field_id_Value ) || ( id_or_name == c_field_name_Value ) )
      display_name = get_module_string( c_field_display_name_Value );
   if( ( id_or_name == c_field_id_View ) || ( id_or_name == c_field_name_View ) )
      display_name = get_module_string( c_field_display_name_View );

   return display_name;
}

void Meta_Package_Option::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Package_Option::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Package_Option::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Package_Option::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Package_Option::setup_foreign_key( Meta_Class& o, const string& value )
{
   static_cast< Meta_Class& >( o ).set_key( value );
}

void Meta_Package_Option::setup_foreign_key( Meta_Field& o, const string& value )
{
   static_cast< Meta_Field& >( o ).set_key( value );
}

void Meta_Package_Option::setup_foreign_key( Meta_List& o, const string& value )
{
   static_cast< Meta_List& >( o ).set_key( value );
}

void Meta_Package_Option::setup_foreign_key( Meta_Model& o, const string& value )
{
   static_cast< Meta_Model& >( o ).set_key( value );
}

void Meta_Package_Option::setup_foreign_key( Meta_Modifier& o, const string& value )
{
   static_cast< Meta_Modifier& >( o ).set_key( value );
}

void Meta_Package_Option::setup_foreign_key( Meta_Package& o, const string& value )
{
   static_cast< Meta_Package& >( o ).set_key( value );
}

void Meta_Package_Option::setup_foreign_key( Meta_Procedure& o, const string& value )
{
   static_cast< Meta_Procedure& >( o ).set_key( value );
}

void Meta_Package_Option::setup_foreign_key( Meta_View& o, const string& value )
{
   static_cast< Meta_View& >( o ).set_key( value );
}

void Meta_Package_Option::setup_graph_parent(
 Meta_Class& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Class& >( o ).set_key( init_value );
}

void Meta_Package_Option::setup_graph_parent(
 Meta_Field& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Field& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Field& >( o ).set_key( init_value );
}

void Meta_Package_Option::setup_graph_parent(
 Meta_List& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_List& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_List& >( o ).set_key( init_value );
}

void Meta_Package_Option::setup_graph_parent(
 Meta_Model& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Model& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Model& >( o ).set_key( init_value );
}

void Meta_Package_Option::setup_graph_parent(
 Meta_Modifier& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Modifier& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Modifier& >( o ).set_key( init_value );
}

void Meta_Package_Option::setup_graph_parent(
 Meta_Package& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Package& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Package& >( o ).set_key( init_value );
}

void Meta_Package_Option::setup_graph_parent(
 Meta_Procedure& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Procedure& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Procedure& >( o ).set_key( init_value );
}

void Meta_Package_Option::setup_graph_parent(
 Meta_View& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_View& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_View& >( o ).set_key( init_value );
}

size_t Meta_Package_Option::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Package_Option::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Package_Option::get_num_foreign_key_children( bool is_internal ) const
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

class_base* Meta_Package_Option::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   ( void )child_num;
   ( void )next_child_field;
   ( void )op;

   return p_class_base;
}

void Meta_Package_Option::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_Package_Option::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_Package_Option::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_Package_Option::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_Package_Option::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_Package_Option::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_Package_Option::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_Package_Option::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "package_option";

   return get_module_string( key );
}

string Meta_Package_Option::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_Package_Option::get_create_instance_info( ) const
{
   return "";
}

string Meta_Package_Option::get_update_instance_info( ) const
{
   return "";
}

string Meta_Package_Option::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_Package_Option::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );
   else if( procedure_id == "137430" ) // i.e. Clear
      retval = "";
   else if( procedure_id == "137410" ) // i.e. Edit
      retval = "";
   else if( procedure_id == "137420" ) // i.e. Select
      retval = "";

   return retval;
}

bool Meta_Package_Option::get_is_alias( ) const
{
   return false;
}

void Meta_Package_Option::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Package_Option::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Package_Option::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == c_field_id_Class ) || ( sub_context == c_field_name_Class ) )
      p_class_base = &Class( );
   else if( ( sub_context == c_field_id_Field ) || ( sub_context == c_field_name_Field ) )
      p_class_base = &Field( );
   else if( ( sub_context == c_field_id_List ) || ( sub_context == c_field_name_List ) )
      p_class_base = &List( );
   else if( ( sub_context == c_field_id_Model ) || ( sub_context == c_field_name_Model ) )
      p_class_base = &Model( );
   else if( ( sub_context == c_field_id_Modifier ) || ( sub_context == c_field_name_Modifier ) )
      p_class_base = &Modifier( );
   else if( ( sub_context == c_field_id_Other_Field ) || ( sub_context == c_field_name_Other_Field ) )
      p_class_base = &Other_Field( );
   else if( ( sub_context == c_field_id_Other_Field_2 ) || ( sub_context == c_field_name_Other_Field_2 ) )
      p_class_base = &Other_Field_2( );
   else if( ( sub_context == c_field_id_Other_Package ) || ( sub_context == c_field_name_Other_Package ) )
      p_class_base = &Other_Package( );
   else if( ( sub_context == c_field_id_Other_Source_Field ) || ( sub_context == c_field_name_Other_Source_Field ) )
      p_class_base = &Other_Source_Field( );
   else if( ( sub_context == c_field_id_Package ) || ( sub_context == c_field_name_Package ) )
      p_class_base = &Package( );
   else if( ( sub_context == c_field_id_Procedure ) || ( sub_context == c_field_name_Procedure ) )
      p_class_base = &Procedure( );
   else if( ( sub_context == c_field_id_Source_Field ) || ( sub_context == c_field_name_Source_Field ) )
      p_class_base = &Source_Field( );
   else if( ( sub_context == c_field_id_View ) || ( sub_context == c_field_name_View ) )
      p_class_base = &View( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Package_Option::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Class" );
   names.push_back( "C_Date" );
   names.push_back( "C_Datetime" );
   names.push_back( "C_Field" );
   names.push_back( "C_Has_Field" );
   names.push_back( "C_Has_List" );
   names.push_back( "C_Has_Modifier" );
   names.push_back( "C_Has_Other_Field" );
   names.push_back( "C_Has_Other_Field_2" );
   names.push_back( "C_Has_Other_Source_Field" );
   names.push_back( "C_Has_Procedure" );
   names.push_back( "C_Has_Source_Field" );
   names.push_back( "C_Has_View" );
   names.push_back( "C_Id" );
   names.push_back( "C_Installed" );
   names.push_back( "C_Integer" );
   names.push_back( "C_Is_Class" );
   names.push_back( "C_Is_Mandatory_Class" );
   names.push_back( "C_Is_Mandatory_Field" );
   names.push_back( "C_Is_Mandatory_List" );
   names.push_back( "C_Is_Mandatory_Modifier" );
   names.push_back( "C_Is_Mandatory_Other_Field" );
   names.push_back( "C_Is_Mandatory_Other_Field_2" );
   names.push_back( "C_Is_Mandatory_Other_Source_Field" );
   names.push_back( "C_Is_Mandatory_Procedure" );
   names.push_back( "C_Is_Mandatory_Source_Field" );
   names.push_back( "C_Is_Mandatory_View" );
   names.push_back( "C_Is_Other_Package" );
   names.push_back( "C_List" );
   names.push_back( "C_Model" );
   names.push_back( "C_Modifier" );
   names.push_back( "C_Name" );
   names.push_back( "C_Notes" );
   names.push_back( "C_Numeric" );
   names.push_back( "C_Other_Field" );
   names.push_back( "C_Other_Field_2" );
   names.push_back( "C_Other_Package" );
   names.push_back( "C_Other_Package_Required" );
   names.push_back( "C_Other_Package_Type" );
   names.push_back( "C_Other_Source_Field" );
   names.push_back( "C_Package" );
   names.push_back( "C_Primitive" );
   names.push_back( "C_Procedure" );
   names.push_back( "C_Source_Field" );
   names.push_back( "C_String" );
   names.push_back( "C_Time" );
   names.push_back( "C_Use_Option" );
   names.push_back( "C_View" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Package_Option::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Class( ) ) ) );
   values.push_back( sql_quote( to_string( Date( ) ) ) );
   values.push_back( sql_quote( to_string( Datetime( ) ) ) );
   values.push_back( sql_quote( to_string( Field( ) ) ) );
   values.push_back( to_string( Has_Field( ) ) );
   values.push_back( to_string( Has_List( ) ) );
   values.push_back( to_string( Has_Modifier( ) ) );
   values.push_back( to_string( Has_Other_Field( ) ) );
   values.push_back( to_string( Has_Other_Field_2( ) ) );
   values.push_back( to_string( Has_Other_Source_Field( ) ) );
   values.push_back( to_string( Has_Procedure( ) ) );
   values.push_back( to_string( Has_Source_Field( ) ) );
   values.push_back( to_string( Has_View( ) ) );
   values.push_back( sql_quote( to_string( Id( ) ) ) );
   values.push_back( to_string( Installed( ) ) );
   values.push_back( to_string( Integer( ) ) );
   values.push_back( to_string( Is_Class( ) ) );
   values.push_back( to_string( Is_Mandatory_Class( ) ) );
   values.push_back( to_string( Is_Mandatory_Field( ) ) );
   values.push_back( to_string( Is_Mandatory_List( ) ) );
   values.push_back( to_string( Is_Mandatory_Modifier( ) ) );
   values.push_back( to_string( Is_Mandatory_Other_Field( ) ) );
   values.push_back( to_string( Is_Mandatory_Other_Field_2( ) ) );
   values.push_back( to_string( Is_Mandatory_Other_Source_Field( ) ) );
   values.push_back( to_string( Is_Mandatory_Procedure( ) ) );
   values.push_back( to_string( Is_Mandatory_Source_Field( ) ) );
   values.push_back( to_string( Is_Mandatory_View( ) ) );
   values.push_back( to_string( Is_Other_Package( ) ) );
   values.push_back( sql_quote( to_string( List( ) ) ) );
   values.push_back( sql_quote( to_string( Model( ) ) ) );
   values.push_back( sql_quote( to_string( Modifier( ) ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( sql_quote( to_string( Notes( ) ) ) );
   values.push_back( sql_quote( to_string( Numeric( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Field_2( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Package( ) ) ) );
   values.push_back( to_string( Other_Package_Required( ) ) );
   values.push_back( sql_quote( to_string( Other_Package_Type( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Source_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Package( ) ) ) );
   values.push_back( to_string( Primitive( ) ) );
   values.push_back( sql_quote( to_string( Procedure( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Field( ) ) ) );
   values.push_back( sql_quote( to_string( String( ) ) ) );
   values.push_back( sql_quote( to_string( Time( ) ) ) );
   values.push_back( to_string( Use_Option( ) ) );
   values.push_back( sql_quote( to_string( View( ) ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Package_Option::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [<start get_required_field_names>]
//nyi
   if( needs_field_value( "Actions", dependents ) )
   {
      dependents.insert( "Package" );

      if( ( use_transients && is_field_transient( e_field_id_Package ) )
       || ( !use_transients && !is_field_transient( e_field_id_Package ) ) )
         names.insert( "Package" );
   }

   if( needs_field_value( "Value", dependents ) )
   {
      dependents.insert( "Use_Option" );

      if( ( use_transients && is_field_transient( e_field_id_Use_Option ) )
       || ( !use_transients && !is_field_transient( e_field_id_Use_Option ) ) )
         names.insert( "Use_Option" );
   }

   if( needs_field_value( "Value", dependents ) )
   {
      dependents.insert( "Class" );

      if( ( use_transients && is_field_transient( e_field_id_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Class ) ) )
         names.insert( "Class" );
   }

   if( needs_field_value( "Value", dependents ) )
   {
      dependents.insert( "Other_Package" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Package ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Package ) ) )
         names.insert( "Other_Package" );
   }

   if( needs_field_value( "Value", dependents ) )
   {
      dependents.insert( "Primitive" );

      if( ( use_transients && is_field_transient( e_field_id_Primitive ) )
       || ( !use_transients && !is_field_transient( e_field_id_Primitive ) ) )
         names.insert( "Primitive" );
   }

   if( needs_field_value( "Value", dependents ) )
   {
      dependents.insert( "String" );

      if( ( use_transients && is_field_transient( e_field_id_String ) )
       || ( !use_transients && !is_field_transient( e_field_id_String ) ) )
         names.insert( "String" );
   }

   if( needs_field_value( "Value", dependents ) )
   {
      dependents.insert( "Datetime" );

      if( ( use_transients && is_field_transient( e_field_id_Datetime ) )
       || ( !use_transients && !is_field_transient( e_field_id_Datetime ) ) )
         names.insert( "Datetime" );
   }

   if( needs_field_value( "Value", dependents ) )
   {
      dependents.insert( "Date" );

      if( ( use_transients && is_field_transient( e_field_id_Date ) )
       || ( !use_transients && !is_field_transient( e_field_id_Date ) ) )
         names.insert( "Date" );
   }

   if( needs_field_value( "Value", dependents ) )
   {
      dependents.insert( "Time" );

      if( ( use_transients && is_field_transient( e_field_id_Time ) )
       || ( !use_transients && !is_field_transient( e_field_id_Time ) ) )
         names.insert( "Time" );
   }

   if( needs_field_value( "Value", dependents ) )
   {
      dependents.insert( "Numeric" );

      if( ( use_transients && is_field_transient( e_field_id_Numeric ) )
       || ( !use_transients && !is_field_transient( e_field_id_Numeric ) ) )
         names.insert( "Numeric" );
   }

   if( needs_field_value( "Value", dependents ) )
   {
      dependents.insert( "Integer" );

      if( ( use_transients && is_field_transient( e_field_id_Integer ) )
       || ( !use_transients && !is_field_transient( e_field_id_Integer ) ) )
         names.insert( "Integer" );
   }
   // [<finish get_required_field_names>]
}

void Meta_Package_Option::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start protect_equal)] 600880
   dependents.insert( "Installed" );

   if( ( use_transients && is_field_transient( e_field_id_Installed ) )
    || ( !use_transients && !is_field_transient( e_field_id_Installed ) ) )
      names.insert( "Installed" );
   // [(finish protect_equal)] 600880

   // [(start modifier_field_value)] 600882
   dependents.insert( "Is_Other_Package" ); // (for Is_Other_Package_Option modifier)

   if( ( use_transients && is_field_transient( e_field_id_Is_Other_Package ) )
    || ( !use_transients && !is_field_transient( e_field_id_Is_Other_Package ) ) )
      names.insert( "Is_Other_Package" );
   // [(finish modifier_field_value)] 600882

   // [(start modifier_field_value)] 600883
   dependents.insert( "Is_Other_Package" ); // (for Is_Not_Other_Package_Option modifier)

   if( ( use_transients && is_field_transient( e_field_id_Is_Other_Package ) )
    || ( !use_transients && !is_field_transient( e_field_id_Is_Other_Package ) ) )
      names.insert( "Is_Other_Package" );
   // [(finish modifier_field_value)] 600883

   // [(start modifier_field_value)] 600884
   dependents.insert( "Primitive" ); // (for Is_String modifier)

   if( ( use_transients && is_field_transient( e_field_id_Primitive ) )
    || ( !use_transients && !is_field_transient( e_field_id_Primitive ) ) )
      names.insert( "Primitive" );
   // [(finish modifier_field_value)] 600884

   // [(start modifier_field_value)] 600885
   dependents.insert( "Primitive" ); // (for Is_Datetime modifier)

   if( ( use_transients && is_field_transient( e_field_id_Primitive ) )
    || ( !use_transients && !is_field_transient( e_field_id_Primitive ) ) )
      names.insert( "Primitive" );
   // [(finish modifier_field_value)] 600885

   // [(start modifier_field_value)] 600886
   dependents.insert( "Primitive" ); // (for Is_Date modifier)

   if( ( use_transients && is_field_transient( e_field_id_Primitive ) )
    || ( !use_transients && !is_field_transient( e_field_id_Primitive ) ) )
      names.insert( "Primitive" );
   // [(finish modifier_field_value)] 600886

   // [(start modifier_field_value)] 600887
   dependents.insert( "Primitive" ); // (for Is_Time modifier)

   if( ( use_transients && is_field_transient( e_field_id_Primitive ) )
    || ( !use_transients && !is_field_transient( e_field_id_Primitive ) ) )
      names.insert( "Primitive" );
   // [(finish modifier_field_value)] 600887

   // [(start modifier_field_value)] 600888
   dependents.insert( "Primitive" ); // (for Is_Numeric modifier)

   if( ( use_transients && is_field_transient( e_field_id_Primitive ) )
    || ( !use_transients && !is_field_transient( e_field_id_Primitive ) ) )
      names.insert( "Primitive" );
   // [(finish modifier_field_value)] 600888

   // [(start modifier_field_value)] 600889
   dependents.insert( "Primitive" ); // (for Is_Int modifier)

   if( ( use_transients && is_field_transient( e_field_id_Primitive ) )
    || ( !use_transients && !is_field_transient( e_field_id_Primitive ) ) )
      names.insert( "Primitive" );
   // [(finish modifier_field_value)] 600889

   // [(start modifier_field_value)] 600890
   dependents.insert( "Primitive" ); // (for Is_Bool modifier)

   if( ( use_transients && is_field_transient( e_field_id_Primitive ) )
    || ( !use_transients && !is_field_transient( e_field_id_Primitive ) ) )
      names.insert( "Primitive" );
   // [(finish modifier_field_value)] 600890

   // [(start modifier_field_value)] 600891
   dependents.insert( "Is_Class" ); // (for Is_Class_Option modifier)

   if( ( use_transients && is_field_transient( e_field_id_Is_Class ) )
    || ( !use_transients && !is_field_transient( e_field_id_Is_Class ) ) )
      names.insert( "Is_Class" );
   // [(finish modifier_field_value)] 600891

   // [(start modifier_field_value)] 600892
   dependents.insert( "Is_Class" ); // (for Is_Not_Class_Option modifier)

   if( ( use_transients && is_field_transient( e_field_id_Is_Class ) )
    || ( !use_transients && !is_field_transient( e_field_id_Is_Class ) ) )
      names.insert( "Is_Class" );
   // [(finish modifier_field_value)] 600892

   // [(start modifier_field_value)] 600893a
   dependents.insert( "Has_Field" ); // (for Hide_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Has_Field ) )
    || ( !use_transients && !is_field_transient( e_field_id_Has_Field ) ) )
      names.insert( "Has_Field" );
   // [(finish modifier_field_value)] 600893a

   // [(start modifier_field_value)] 600893b
   dependents.insert( "Has_Other_Field" ); // (for Hide_Other_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Has_Other_Field ) )
    || ( !use_transients && !is_field_transient( e_field_id_Has_Other_Field ) ) )
      names.insert( "Has_Other_Field" );
   // [(finish modifier_field_value)] 600893b

   // [(start modifier_field_value)] 600893c
   dependents.insert( "Has_Other_Field_2" ); // (for Hide_Other_Field_2 modifier)

   if( ( use_transients && is_field_transient( e_field_id_Has_Other_Field_2 ) )
    || ( !use_transients && !is_field_transient( e_field_id_Has_Other_Field_2 ) ) )
      names.insert( "Has_Other_Field_2" );
   // [(finish modifier_field_value)] 600893c

   // [(start modifier_field_value)] 600893d
   dependents.insert( "Has_Source_Field" ); // (for Hide_Source_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Has_Source_Field ) )
    || ( !use_transients && !is_field_transient( e_field_id_Has_Source_Field ) ) )
      names.insert( "Has_Source_Field" );
   // [(finish modifier_field_value)] 600893d

   // [(start modifier_field_value)] 600893e
   dependents.insert( "Has_Other_Source_Field" ); // (for Hide_Other_Source_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Has_Other_Source_Field ) )
    || ( !use_transients && !is_field_transient( e_field_id_Has_Other_Source_Field ) ) )
      names.insert( "Has_Other_Source_Field" );
   // [(finish modifier_field_value)] 600893e

   // [(start modifier_field_value)] 600894
   dependents.insert( "Has_View" ); // (for Hide_View modifier)

   if( ( use_transients && is_field_transient( e_field_id_Has_View ) )
    || ( !use_transients && !is_field_transient( e_field_id_Has_View ) ) )
      names.insert( "Has_View" );
   // [(finish modifier_field_value)] 600894

   // [(start modifier_field_value)] 600895
   dependents.insert( "Has_List" ); // (for Hide_List modifier)

   if( ( use_transients && is_field_transient( e_field_id_Has_List ) )
    || ( !use_transients && !is_field_transient( e_field_id_Has_List ) ) )
      names.insert( "Has_List" );
   // [(finish modifier_field_value)] 600895

   // [(start modifier_field_value)] 600896
   dependents.insert( "Has_Modifier" ); // (for Hide_Modifier modifier)

   if( ( use_transients && is_field_transient( e_field_id_Has_Modifier ) )
    || ( !use_transients && !is_field_transient( e_field_id_Has_Modifier ) ) )
      names.insert( "Has_Modifier" );
   // [(finish modifier_field_value)] 600896

   // [(start modifier_field_value)] 600897
   dependents.insert( "Has_Procedure" ); // (for Hide_Procedure modifier)

   if( ( use_transients && is_field_transient( e_field_id_Has_Procedure ) )
    || ( !use_transients && !is_field_transient( e_field_id_Has_Procedure ) ) )
      names.insert( "Has_Procedure" );
   // [(finish modifier_field_value)] 600897

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Package_Option::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Package_Option::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_Package_Option::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Package_Option::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Package_Option::static_lock_class_id( )
{
   return "137100";
}

const char* Meta_Package_Option::static_check_class_name( )
{
   return "Package_Option";
}

const char* Meta_Package_Option::static_persistence_extra( )
{
   return "";
}

bool Meta_Package_Option::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Package_Option::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.137100" );
}

void Meta_Package_Option::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "137116", "Actions", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "302830a", "Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "137111", "Date", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "137110", "Datetime", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "302830b", "Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "137119", "Has_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137128", "Has_List", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137130", "Has_Modifier", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137124", "Has_Other_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137126", "Has_Other_Field_2", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137136", "Has_Other_Source_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137132", "Has_Procedure", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137134", "Has_Source_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137122", "Has_View", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137101", "Id", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "137103", "Installed", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137114", "Integer", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "137117", "Is_Class", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137118", "Is_Mandatory_Class", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137120", "Is_Mandatory_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137129", "Is_Mandatory_List", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137131", "Is_Mandatory_Modifier", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137125", "Is_Mandatory_Other_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137127", "Is_Mandatory_Other_Field_2", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137137", "Is_Mandatory_Other_Source_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137133", "Is_Mandatory_Procedure", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137135", "Is_Mandatory_Source_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137123", "Is_Mandatory_View", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137104", "Is_Other_Package", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "302831", "List", "Meta_List", false, "", "" ) );
   all_field_info.push_back( field_info( "302825", "Model", "Meta_Model", true, "", "" ) );
   all_field_info.push_back( field_info( "302832", "Modifier", "Meta_Modifier", false, "", "" ) );
   all_field_info.push_back( field_info( "137115", "Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "137121", "Notes", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "137113", "Numeric", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "302830d", "Other_Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "302830e", "Other_Field_2", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "302830", "Other_Package", "Meta_Package", false, "", "" ) );
   all_field_info.push_back( field_info( "137105", "Other_Package_Required", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137106", "Other_Package_Type", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "302830g", "Other_Source_Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "302820", "Package", "Meta_Package", true, "", "" ) );
   all_field_info.push_back( field_info( "137108", "Primitive", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "302833", "Procedure", "Meta_Procedure", false, "", "" ) );
   all_field_info.push_back( field_info( "302830f", "Source_Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "137109", "String", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "137112", "Time", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "137102", "Use_Option", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "137107", "Value", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "302830c", "View", "Meta_View", false, "", "" ) );
}

void Meta_Package_Option::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Class, make_pair( "Meta.137100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Field, make_pair( "Meta.137100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_List, make_pair( "Meta.137100", "Meta_List" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Model, make_pair( "Meta.137100", "Meta_Model" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Modifier, make_pair( "Meta.137100", "Meta_Modifier" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Other_Field, make_pair( "Meta.137100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Other_Field_2, make_pair( "Meta.137100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Other_Package, make_pair( "Meta.137100", "Meta_Package" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Other_Source_Field, make_pair( "Meta.137100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Package, make_pair( "Meta.137100", "Meta_Package" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Procedure, make_pair( "Meta.137100", "Meta_Procedure" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Field, make_pair( "Meta.137100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_View, make_pair( "Meta.137100", "Meta_View" ) ) );
}

int Meta_Package_Option::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Package_Option::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_Package_Option::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Package_Option::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "137116";
      break;

      case 2:
      p_id = "302830a";
      break;

      case 3:
      p_id = "137111";
      break;

      case 4:
      p_id = "137110";
      break;

      case 5:
      p_id = "302830b";
      break;

      case 6:
      p_id = "137119";
      break;

      case 7:
      p_id = "137128";
      break;

      case 8:
      p_id = "137130";
      break;

      case 9:
      p_id = "137124";
      break;

      case 10:
      p_id = "137126";
      break;

      case 11:
      p_id = "137136";
      break;

      case 12:
      p_id = "137132";
      break;

      case 13:
      p_id = "137134";
      break;

      case 14:
      p_id = "137122";
      break;

      case 15:
      p_id = "137101";
      break;

      case 16:
      p_id = "137103";
      break;

      case 17:
      p_id = "137114";
      break;

      case 18:
      p_id = "137117";
      break;

      case 19:
      p_id = "137118";
      break;

      case 20:
      p_id = "137120";
      break;

      case 21:
      p_id = "137129";
      break;

      case 22:
      p_id = "137131";
      break;

      case 23:
      p_id = "137125";
      break;

      case 24:
      p_id = "137127";
      break;

      case 25:
      p_id = "137137";
      break;

      case 26:
      p_id = "137133";
      break;

      case 27:
      p_id = "137135";
      break;

      case 28:
      p_id = "137123";
      break;

      case 29:
      p_id = "137104";
      break;

      case 30:
      p_id = "302831";
      break;

      case 31:
      p_id = "302825";
      break;

      case 32:
      p_id = "302832";
      break;

      case 33:
      p_id = "137115";
      break;

      case 34:
      p_id = "137121";
      break;

      case 35:
      p_id = "137113";
      break;

      case 36:
      p_id = "302830d";
      break;

      case 37:
      p_id = "302830e";
      break;

      case 38:
      p_id = "302830";
      break;

      case 39:
      p_id = "137105";
      break;

      case 40:
      p_id = "137106";
      break;

      case 41:
      p_id = "302830g";
      break;

      case 42:
      p_id = "302820";
      break;

      case 43:
      p_id = "137108";
      break;

      case 44:
      p_id = "302833";
      break;

      case 45:
      p_id = "302830f";
      break;

      case 46:
      p_id = "137109";
      break;

      case 47:
      p_id = "137112";
      break;

      case 48:
      p_id = "137102";
      break;

      case 49:
      p_id = "137107";
      break;

      case 50:
      p_id = "302830c";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Package_Option" );

   return p_id;
}

const char* Meta_Package_Option::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Actions";
      break;

      case 2:
      p_id = "Class";
      break;

      case 3:
      p_id = "Date";
      break;

      case 4:
      p_id = "Datetime";
      break;

      case 5:
      p_id = "Field";
      break;

      case 6:
      p_id = "Has_Field";
      break;

      case 7:
      p_id = "Has_List";
      break;

      case 8:
      p_id = "Has_Modifier";
      break;

      case 9:
      p_id = "Has_Other_Field";
      break;

      case 10:
      p_id = "Has_Other_Field_2";
      break;

      case 11:
      p_id = "Has_Other_Source_Field";
      break;

      case 12:
      p_id = "Has_Procedure";
      break;

      case 13:
      p_id = "Has_Source_Field";
      break;

      case 14:
      p_id = "Has_View";
      break;

      case 15:
      p_id = "Id";
      break;

      case 16:
      p_id = "Installed";
      break;

      case 17:
      p_id = "Integer";
      break;

      case 18:
      p_id = "Is_Class";
      break;

      case 19:
      p_id = "Is_Mandatory_Class";
      break;

      case 20:
      p_id = "Is_Mandatory_Field";
      break;

      case 21:
      p_id = "Is_Mandatory_List";
      break;

      case 22:
      p_id = "Is_Mandatory_Modifier";
      break;

      case 23:
      p_id = "Is_Mandatory_Other_Field";
      break;

      case 24:
      p_id = "Is_Mandatory_Other_Field_2";
      break;

      case 25:
      p_id = "Is_Mandatory_Other_Source_Field";
      break;

      case 26:
      p_id = "Is_Mandatory_Procedure";
      break;

      case 27:
      p_id = "Is_Mandatory_Source_Field";
      break;

      case 28:
      p_id = "Is_Mandatory_View";
      break;

      case 29:
      p_id = "Is_Other_Package";
      break;

      case 30:
      p_id = "List";
      break;

      case 31:
      p_id = "Model";
      break;

      case 32:
      p_id = "Modifier";
      break;

      case 33:
      p_id = "Name";
      break;

      case 34:
      p_id = "Notes";
      break;

      case 35:
      p_id = "Numeric";
      break;

      case 36:
      p_id = "Other_Field";
      break;

      case 37:
      p_id = "Other_Field_2";
      break;

      case 38:
      p_id = "Other_Package";
      break;

      case 39:
      p_id = "Other_Package_Required";
      break;

      case 40:
      p_id = "Other_Package_Type";
      break;

      case 41:
      p_id = "Other_Source_Field";
      break;

      case 42:
      p_id = "Package";
      break;

      case 43:
      p_id = "Primitive";
      break;

      case 44:
      p_id = "Procedure";
      break;

      case 45:
      p_id = "Source_Field";
      break;

      case 46:
      p_id = "String";
      break;

      case 47:
      p_id = "Time";
      break;

      case 48:
      p_id = "Use_Option";
      break;

      case 49:
      p_id = "Value";
      break;

      case 50:
      p_id = "View";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Package_Option" );

   return p_id;
}

int Meta_Package_Option::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( ( field == c_field_id_Actions ) || ( field == c_field_name_Actions ) )
      rc += 1;
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      rc += 2;
   else if( ( field == c_field_id_Date ) || ( field == c_field_name_Date ) )
      rc += 3;
   else if( ( field == c_field_id_Datetime ) || ( field == c_field_name_Datetime ) )
      rc += 4;
   else if( ( field == c_field_id_Field ) || ( field == c_field_name_Field ) )
      rc += 5;
   else if( ( field == c_field_id_Has_Field ) || ( field == c_field_name_Has_Field ) )
      rc += 6;
   else if( ( field == c_field_id_Has_List ) || ( field == c_field_name_Has_List ) )
      rc += 7;
   else if( ( field == c_field_id_Has_Modifier ) || ( field == c_field_name_Has_Modifier ) )
      rc += 8;
   else if( ( field == c_field_id_Has_Other_Field ) || ( field == c_field_name_Has_Other_Field ) )
      rc += 9;
   else if( ( field == c_field_id_Has_Other_Field_2 ) || ( field == c_field_name_Has_Other_Field_2 ) )
      rc += 10;
   else if( ( field == c_field_id_Has_Other_Source_Field ) || ( field == c_field_name_Has_Other_Source_Field ) )
      rc += 11;
   else if( ( field == c_field_id_Has_Procedure ) || ( field == c_field_name_Has_Procedure ) )
      rc += 12;
   else if( ( field == c_field_id_Has_Source_Field ) || ( field == c_field_name_Has_Source_Field ) )
      rc += 13;
   else if( ( field == c_field_id_Has_View ) || ( field == c_field_name_Has_View ) )
      rc += 14;
   else if( ( field == c_field_id_Id ) || ( field == c_field_name_Id ) )
      rc += 15;
   else if( ( field == c_field_id_Installed ) || ( field == c_field_name_Installed ) )
      rc += 16;
   else if( ( field == c_field_id_Integer ) || ( field == c_field_name_Integer ) )
      rc += 17;
   else if( ( field == c_field_id_Is_Class ) || ( field == c_field_name_Is_Class ) )
      rc += 18;
   else if( ( field == c_field_id_Is_Mandatory_Class ) || ( field == c_field_name_Is_Mandatory_Class ) )
      rc += 19;
   else if( ( field == c_field_id_Is_Mandatory_Field ) || ( field == c_field_name_Is_Mandatory_Field ) )
      rc += 20;
   else if( ( field == c_field_id_Is_Mandatory_List ) || ( field == c_field_name_Is_Mandatory_List ) )
      rc += 21;
   else if( ( field == c_field_id_Is_Mandatory_Modifier ) || ( field == c_field_name_Is_Mandatory_Modifier ) )
      rc += 22;
   else if( ( field == c_field_id_Is_Mandatory_Other_Field ) || ( field == c_field_name_Is_Mandatory_Other_Field ) )
      rc += 23;
   else if( ( field == c_field_id_Is_Mandatory_Other_Field_2 ) || ( field == c_field_name_Is_Mandatory_Other_Field_2 ) )
      rc += 24;
   else if( ( field == c_field_id_Is_Mandatory_Other_Source_Field ) || ( field == c_field_name_Is_Mandatory_Other_Source_Field ) )
      rc += 25;
   else if( ( field == c_field_id_Is_Mandatory_Procedure ) || ( field == c_field_name_Is_Mandatory_Procedure ) )
      rc += 26;
   else if( ( field == c_field_id_Is_Mandatory_Source_Field ) || ( field == c_field_name_Is_Mandatory_Source_Field ) )
      rc += 27;
   else if( ( field == c_field_id_Is_Mandatory_View ) || ( field == c_field_name_Is_Mandatory_View ) )
      rc += 28;
   else if( ( field == c_field_id_Is_Other_Package ) || ( field == c_field_name_Is_Other_Package ) )
      rc += 29;
   else if( ( field == c_field_id_List ) || ( field == c_field_name_List ) )
      rc += 30;
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      rc += 31;
   else if( ( field == c_field_id_Modifier ) || ( field == c_field_name_Modifier ) )
      rc += 32;
   else if( ( field == c_field_id_Name ) || ( field == c_field_name_Name ) )
      rc += 33;
   else if( ( field == c_field_id_Notes ) || ( field == c_field_name_Notes ) )
      rc += 34;
   else if( ( field == c_field_id_Numeric ) || ( field == c_field_name_Numeric ) )
      rc += 35;
   else if( ( field == c_field_id_Other_Field ) || ( field == c_field_name_Other_Field ) )
      rc += 36;
   else if( ( field == c_field_id_Other_Field_2 ) || ( field == c_field_name_Other_Field_2 ) )
      rc += 37;
   else if( ( field == c_field_id_Other_Package ) || ( field == c_field_name_Other_Package ) )
      rc += 38;
   else if( ( field == c_field_id_Other_Package_Required ) || ( field == c_field_name_Other_Package_Required ) )
      rc += 39;
   else if( ( field == c_field_id_Other_Package_Type ) || ( field == c_field_name_Other_Package_Type ) )
      rc += 40;
   else if( ( field == c_field_id_Other_Source_Field ) || ( field == c_field_name_Other_Source_Field ) )
      rc += 41;
   else if( ( field == c_field_id_Package ) || ( field == c_field_name_Package ) )
      rc += 42;
   else if( ( field == c_field_id_Primitive ) || ( field == c_field_name_Primitive ) )
      rc += 43;
   else if( ( field == c_field_id_Procedure ) || ( field == c_field_name_Procedure ) )
      rc += 44;
   else if( ( field == c_field_id_Source_Field ) || ( field == c_field_name_Source_Field ) )
      rc += 45;
   else if( ( field == c_field_id_String ) || ( field == c_field_name_String ) )
      rc += 46;
   else if( ( field == c_field_id_Time ) || ( field == c_field_name_Time ) )
      rc += 47;
   else if( ( field == c_field_id_Use_Option ) || ( field == c_field_name_Use_Option ) )
      rc += 48;
   else if( ( field == c_field_id_Value ) || ( field == c_field_name_Value ) )
      rc += 49;
   else if( ( field == c_field_id_View ) || ( field == c_field_name_View ) )
      rc += 50;

   return rc - 1;
}

procedure_info_container& Meta_Package_Option::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "137430", procedure_info( "Clear" ) ) );
      procedures.insert( make_pair( "137410", procedure_info( "Edit" ) ) );
      procedures.insert( make_pair( "137420", procedure_info( "Select" ) ) );
   }

   return procedures;
}

string Meta_Package_Option::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(75),"
    "C_Ver_ SMALLINT UNSIGNED NOT NULL,"
    "C_Rev_ BIGINT UNSIGNED NOT NULL,"
    "C_Sec_ BIGINT UNSIGNED NOT NULL,"
    "C_Typ_ VARCHAR(16) NOT NULL,"
    "C_Class VARCHAR(75) NOT NULL,"
    "C_Date VARCHAR(200) NOT NULL,"
    "C_Datetime VARCHAR(200) NOT NULL,"
    "C_Field VARCHAR(75) NOT NULL,"
    "C_Has_Field INTEGER NOT NULL,"
    "C_Has_List INTEGER NOT NULL,"
    "C_Has_Modifier INTEGER NOT NULL,"
    "C_Has_Other_Field INTEGER NOT NULL,"
    "C_Has_Other_Field_2 INTEGER NOT NULL,"
    "C_Has_Other_Source_Field INTEGER NOT NULL,"
    "C_Has_Procedure INTEGER NOT NULL,"
    "C_Has_Source_Field INTEGER NOT NULL,"
    "C_Has_View INTEGER NOT NULL,"
    "C_Id VARCHAR(200) NOT NULL,"
    "C_Installed INTEGER NOT NULL,"
    "C_Integer INTEGER NOT NULL,"
    "C_Is_Class INTEGER NOT NULL,"
    "C_Is_Mandatory_Class INTEGER NOT NULL,"
    "C_Is_Mandatory_Field INTEGER NOT NULL,"
    "C_Is_Mandatory_List INTEGER NOT NULL,"
    "C_Is_Mandatory_Modifier INTEGER NOT NULL,"
    "C_Is_Mandatory_Other_Field INTEGER NOT NULL,"
    "C_Is_Mandatory_Other_Field_2 INTEGER NOT NULL,"
    "C_Is_Mandatory_Other_Source_Field INTEGER NOT NULL,"
    "C_Is_Mandatory_Procedure INTEGER NOT NULL,"
    "C_Is_Mandatory_Source_Field INTEGER NOT NULL,"
    "C_Is_Mandatory_View INTEGER NOT NULL,"
    "C_Is_Other_Package INTEGER NOT NULL,"
    "C_List VARCHAR(256) NOT NULL,"
    "C_Model VARCHAR(75) NOT NULL,"
    "C_Modifier VARCHAR(256) NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Notes TEXT NOT NULL,"
    "C_Numeric VARCHAR(256) NOT NULL,"
    "C_Other_Field VARCHAR(256) NOT NULL,"
    "C_Other_Field_2 VARCHAR(256) NOT NULL,"
    "C_Other_Package VARCHAR(256) NOT NULL,"
    "C_Other_Package_Required INTEGER NOT NULL,"
    "C_Other_Package_Type VARCHAR(256) NOT NULL,"
    "C_Other_Source_Field VARCHAR(256) NOT NULL,"
    "C_Package VARCHAR(75) NOT NULL,"
    "C_Primitive INTEGER NOT NULL,"
    "C_Procedure VARCHAR(256) NOT NULL,"
    "C_Source_Field VARCHAR(256) NOT NULL,"
    "C_String VARCHAR(256) NOT NULL,"
    "C_Time VARCHAR(256) NOT NULL,"
    "C_Use_Option INTEGER NOT NULL,"
    "C_View VARCHAR(75) NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Package_Option::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Package_Option::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "enum_primitive_0", get_enum_string_primitive( 0 ) ) );
   pairs.push_back( make_pair( "enum_primitive_1", get_enum_string_primitive( 1 ) ) );
   pairs.push_back( make_pair( "enum_primitive_2", get_enum_string_primitive( 2 ) ) );
   pairs.push_back( make_pair( "enum_primitive_3", get_enum_string_primitive( 3 ) ) );
   pairs.push_back( make_pair( "enum_primitive_4", get_enum_string_primitive( 4 ) ) );
   pairs.push_back( make_pair( "enum_primitive_5", get_enum_string_primitive( 5 ) ) );
   pairs.push_back( make_pair( "enum_primitive_6", get_enum_string_primitive( 6 ) ) );
}

void Meta_Package_Option::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "Package,Id", "string,string" ) );
   pairs.push_back( make_pair( "Package,Name", "string,string" ) );
}

void Meta_Package_Option::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "Package,Id" );
   unique_indexes.push_back( "Package,Name" );
}

void Meta_Package_Option::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Package,C_Id" );
   indexes.push_back( "C_Package,C_Name" );
}

void Meta_Package_Option::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Package,C_Id" );
   indexes.push_back( "C_Package,C_Name" );
}

void Meta_Package_Option::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Package_Option::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Package_Option::static_insert_external_alias( const string& module_and_class_id, Meta_Package_Option* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Package_Option::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Package_Option::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   g_primitive_enum.insert( 0 );
   g_primitive_enum.insert( 1 );
   g_primitive_enum.insert( 2 );
   g_primitive_enum.insert( 3 );
   g_primitive_enum.insert( 4 );
   g_primitive_enum.insert( 5 );
   g_primitive_enum.insert( 6 );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Package_Option::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
