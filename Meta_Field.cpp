// Copyright (c) 2008
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

#ifdef __BORLANDC__
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

#include "Meta_Specification_Content_Page.h"
#include "Meta_Specification_Copy_Child_Links.h"
#include "Meta_List_Field.h"
#include "Meta_Index.h"
#include "Meta_Specification_Field_Action.h"
#include "Meta_Specification.h"
#include "Meta_List.h"
#include "Meta_View_Field.h"
#include "Meta_Initial_Record_Value.h"
#include "Meta_Modifier_Affect.h"
#include "Meta_Class.h"
#include "Meta_Enum.h"
#include "Meta_Field.h"
#include "Meta_Type.h"

#include "numeric_helper.h"

#include "ciyam_base.h"
#include "class_domains.h"
#include "module_strings.h"
#include "class_utilities.h"
#include "command_handler.h"
#include "module_interface.h"

// [(start clone_children_for_create)]
#include "Meta_Class.h"
// [(finish clone_children_for_create)]

// [(start clone_children_for_create)]
#include "Meta_Initial_Record.h"
#include "Meta_Initial_Record_Value.h"
// [(finish clone_children_for_create)]

// [<start includes>]
// [<finish includes>]

using namespace std;

// [<start namespaces>]
// [<finish namespaces>]

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

#include "Meta_Field.cmh"

const int32_t c_version = 1;

const char* const c_okay = "okay";

const char* const c_field_id_Class = "300700";
const char* const c_field_id_Default = "107102";
const char* const c_field_id_Enum = "300720";
const char* const c_field_id_Extra = "107103";
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
const char* const c_field_id_Use_In_Text_Search = "107111";

const char* const c_field_name_Class = "Class";
const char* const c_field_name_Default = "Default";
const char* const c_field_name_Enum = "Enum";
const char* const c_field_name_Extra = "Extra";
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
const char* const c_field_name_Use_In_Text_Search = "Use_In_Text_Search";

const char* const c_field_display_name_Class = "field_field_class";
const char* const c_field_display_name_Default = "field_field_default";
const char* const c_field_display_name_Enum = "field_field_enum";
const char* const c_field_display_name_Extra = "field_field_extra";
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
const char* const c_field_display_name_Use_In_Text_Search = "field_field_use_in_text_search";

const int c_num_fields = 19;

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
   "300700",
   "300710",
   "300720",
   "300730",
   "300740"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Class",
   "Default",
   "Enum",
   "Extra",
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
   "Use_In_Text_Search"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool has_field( const string& field )
{
   return binary_search( c_all_sorted_field_ids, c_all_sorted_field_ids + c_num_fields, field.c_str( ), compare )
    || binary_search( c_all_sorted_field_names, c_all_sorted_field_names + c_num_fields, field.c_str( ), compare );
}

const int c_num_transient_fields = 2;

const char* const c_transient_sorted_field_ids[ ] =
{
   "107113",
   "107114"
};

const char* const c_transient_sorted_field_names[ ] =
{
   "Numeric_Decimals",
   "Parent_Class_Name"
};

inline bool transient_compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool is_transient_field( const string& field )
{
   return binary_search( c_transient_sorted_field_ids,
    c_transient_sorted_field_ids + c_num_transient_fields, field.c_str( ), transient_compare )
    || binary_search( c_transient_sorted_field_names,
    c_transient_sorted_field_names + c_num_transient_fields, field.c_str( ), transient_compare );
}

const char* const c_procedure_id_Get_Text_Type = "107410";

const uint64_t c_modifier_Is_Any_Non_Text_Type = UINT64_C( 0x100 );
const uint64_t c_modifier_Is_Internal = UINT64_C( 0x200 );
const uint64_t c_modifier_Is_Transient = UINT64_C( 0x400 );
const uint64_t c_modifier_Is_Type_bool = UINT64_C( 0x800 );

domain_string_max_size< 100 > g_Default_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Id_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 50 > > g_Name_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Parent_Class_Name_domain;

set< string > g_derivations;

typedef map< string, Meta_Field* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Field* > external_aliases_lookup_container;
typedef external_aliases_lookup_container::const_iterator external_aliases_lookup_const_iterator;

external_aliases_container g_external_aliases;
external_aliases_lookup_container g_external_aliases_lookup;

string gv_default_Class = string( );
string gv_default_Default = string( );
string gv_default_Enum = string( );
int gv_default_Extra = int( 0 );
string gv_default_Id = string( );
bool gv_default_Internal = bool( 0 );
bool gv_default_Is_Foreign_Key = bool( 0 );
bool gv_default_Is_Text_Type = bool( 1 );
bool gv_default_Mandatory = bool( 1 );
string gv_default_Name = string( );
numeric gv_default_Numeric_Decimals = numeric( 0 );
string gv_default_Parent_Class = string( );
string gv_default_Parent_Class_Name = string( );
int gv_default_Primitive = int( 0 );
string gv_default_Source_Field = string( );
bool gv_default_Transient = bool( 0 );
string gv_default_Type = string( );
int gv_default_UOM = int( 0 );
bool gv_default_Use_In_Text_Search = bool( 0 );

set< int > g_field_extra_enum;
set< int > g_primitive_enum;
set< int > g_uom_enum;

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
const int c_enum_field_extra_filename( 22 );
const int c_enum_field_extra_password( 10 );
const int c_enum_field_extra_defcurrent( 11 );
const int c_enum_field_extra_permission( 17 );
const int c_enum_field_extra_orientation( 24 );
const int c_enum_field_extra_epassword( 25 );
const int c_enum_field_extra_hpassword( 26 );
const int c_enum_field_extra_hpassword_salt( 27 );
const int c_enum_field_extra_manual_link( 13 );
const int c_enum_field_extra_user_perms( 12 );
const int c_enum_field_extra_current_year( 14 );
const int c_enum_field_extra_security_level( 18 );
const int c_enum_field_extra_create_datetime( 15 );
const int c_enum_field_extra_modify_datetime( 16 );
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
   else if( to_string( val ) == to_string( "22" ) )
      string_name = "enum_field_extra_filename";
   else if( to_string( val ) == to_string( "10" ) )
      string_name = "enum_field_extra_password";
   else if( to_string( val ) == to_string( "11" ) )
      string_name = "enum_field_extra_defcurrent";
   else if( to_string( val ) == to_string( "17" ) )
      string_name = "enum_field_extra_permission";
   else if( to_string( val ) == to_string( "24" ) )
      string_name = "enum_field_extra_orientation";
   else if( to_string( val ) == to_string( "25" ) )
      string_name = "enum_field_extra_epassword";
   else if( to_string( val ) == to_string( "26" ) )
      string_name = "enum_field_extra_hpassword";
   else if( to_string( val ) == to_string( "27" ) )
      string_name = "enum_field_extra_hpassword_salt";
   else if( to_string( val ) == to_string( "13" ) )
      string_name = "enum_field_extra_manual_link";
   else if( to_string( val ) == to_string( "12" ) )
      string_name = "enum_field_extra_user_perms";
   else if( to_string( val ) == to_string( "14" ) )
      string_name = "enum_field_extra_current_year";
   else if( to_string( val ) == to_string( "18" ) )
      string_name = "enum_field_extra_security_level";
   else if( to_string( val ) == to_string( "15" ) )
      string_name = "enum_field_extra_create_datetime";
   else if( to_string( val ) == to_string( "16" ) )
      string_name = "enum_field_extra_modify_datetime";
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

   void handle_unknown_command( const string& command )
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
      bool want_fixed( has_parm_val( parameters, c_cmd_parm_Meta_Field_key_fixed ) );

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
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Field_get_field_name ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );
      else if( field_name == c_field_id_Class || field_name == c_field_name_Class )
         string_getter< Meta_Class >( cmd_handler.p_Meta_Field->Class( ), cmd_handler.retval );
      else if( field_name == c_field_id_Default || field_name == c_field_name_Default )
         string_getter< string >( cmd_handler.p_Meta_Field->Default( ), cmd_handler.retval );
      else if( field_name == c_field_id_Enum || field_name == c_field_name_Enum )
         string_getter< Meta_Enum >( cmd_handler.p_Meta_Field->Enum( ), cmd_handler.retval );
      else if( field_name == c_field_id_Extra || field_name == c_field_name_Extra )
         string_getter< int >( cmd_handler.p_Meta_Field->Extra( ), cmd_handler.retval );
      else if( field_name == c_field_id_Id || field_name == c_field_name_Id )
         string_getter< string >( cmd_handler.p_Meta_Field->Id( ), cmd_handler.retval );
      else if( field_name == c_field_id_Internal || field_name == c_field_name_Internal )
         string_getter< bool >( cmd_handler.p_Meta_Field->Internal( ), cmd_handler.retval );
      else if( field_name == c_field_id_Is_Foreign_Key || field_name == c_field_name_Is_Foreign_Key )
         string_getter< bool >( cmd_handler.p_Meta_Field->Is_Foreign_Key( ), cmd_handler.retval );
      else if( field_name == c_field_id_Is_Text_Type || field_name == c_field_name_Is_Text_Type )
         string_getter< bool >( cmd_handler.p_Meta_Field->Is_Text_Type( ), cmd_handler.retval );
      else if( field_name == c_field_id_Mandatory || field_name == c_field_name_Mandatory )
         string_getter< bool >( cmd_handler.p_Meta_Field->Mandatory( ), cmd_handler.retval );
      else if( field_name == c_field_id_Name || field_name == c_field_name_Name )
         string_getter< string >( cmd_handler.p_Meta_Field->Name( ), cmd_handler.retval );
      else if( field_name == c_field_id_Numeric_Decimals || field_name == c_field_name_Numeric_Decimals )
         string_getter< numeric >( cmd_handler.p_Meta_Field->Numeric_Decimals( ), cmd_handler.retval );
      else if( field_name == c_field_id_Parent_Class || field_name == c_field_name_Parent_Class )
         string_getter< Meta_Class >( cmd_handler.p_Meta_Field->Parent_Class( ), cmd_handler.retval );
      else if( field_name == c_field_id_Parent_Class_Name || field_name == c_field_name_Parent_Class_Name )
         string_getter< string >( cmd_handler.p_Meta_Field->Parent_Class_Name( ), cmd_handler.retval );
      else if( field_name == c_field_id_Primitive || field_name == c_field_name_Primitive )
         string_getter< int >( cmd_handler.p_Meta_Field->Primitive( ), cmd_handler.retval );
      else if( field_name == c_field_id_Source_Field || field_name == c_field_name_Source_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Field->Source_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Transient || field_name == c_field_name_Transient )
         string_getter< bool >( cmd_handler.p_Meta_Field->Transient( ), cmd_handler.retval );
      else if( field_name == c_field_id_Type || field_name == c_field_name_Type )
         string_getter< Meta_Type >( cmd_handler.p_Meta_Field->Type( ), cmd_handler.retval );
      else if( field_name == c_field_id_UOM || field_name == c_field_name_UOM )
         string_getter< int >( cmd_handler.p_Meta_Field->UOM( ), cmd_handler.retval );
      else if( field_name == c_field_id_Use_In_Text_Search || field_name == c_field_name_Use_In_Text_Search )
         string_getter< bool >( cmd_handler.p_Meta_Field->Use_In_Text_Search( ), cmd_handler.retval );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Field_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Field_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_parm_Meta_Field_set_field_value ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );
      else if( field_name == c_field_id_Class || field_name == c_field_name_Class )
         func_string_setter< Meta_Field, Meta_Class >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Class, field_value );
      else if( field_name == c_field_id_Default || field_name == c_field_name_Default )
         func_string_setter< Meta_Field, string >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Default, field_value );
      else if( field_name == c_field_id_Enum || field_name == c_field_name_Enum )
         func_string_setter< Meta_Field, Meta_Enum >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Enum, field_value );
      else if( field_name == c_field_id_Extra || field_name == c_field_name_Extra )
         func_string_setter< Meta_Field, int >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Extra, field_value );
      else if( field_name == c_field_id_Id || field_name == c_field_name_Id )
         func_string_setter< Meta_Field, string >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Id, field_value );
      else if( field_name == c_field_id_Internal || field_name == c_field_name_Internal )
         func_string_setter< Meta_Field, bool >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Internal, field_value );
      else if( field_name == c_field_id_Is_Foreign_Key || field_name == c_field_name_Is_Foreign_Key )
         func_string_setter< Meta_Field, bool >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Is_Foreign_Key, field_value );
      else if( field_name == c_field_id_Is_Text_Type || field_name == c_field_name_Is_Text_Type )
         func_string_setter< Meta_Field, bool >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Is_Text_Type, field_value );
      else if( field_name == c_field_id_Mandatory || field_name == c_field_name_Mandatory )
         func_string_setter< Meta_Field, bool >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Mandatory, field_value );
      else if( field_name == c_field_id_Name || field_name == c_field_name_Name )
         func_string_setter< Meta_Field, string >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Name, field_value );
      else if( field_name == c_field_id_Numeric_Decimals || field_name == c_field_name_Numeric_Decimals )
         func_string_setter< Meta_Field, numeric >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Numeric_Decimals, field_value );
      else if( field_name == c_field_id_Parent_Class || field_name == c_field_name_Parent_Class )
         func_string_setter< Meta_Field, Meta_Class >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Parent_Class, field_value );
      else if( field_name == c_field_id_Parent_Class_Name || field_name == c_field_name_Parent_Class_Name )
         func_string_setter< Meta_Field, string >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Parent_Class_Name, field_value );
      else if( field_name == c_field_id_Primitive || field_name == c_field_name_Primitive )
         func_string_setter< Meta_Field, int >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Primitive, field_value );
      else if( field_name == c_field_id_Source_Field || field_name == c_field_name_Source_Field )
         func_string_setter< Meta_Field, Meta_Field >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Source_Field, field_value );
      else if( field_name == c_field_id_Transient || field_name == c_field_name_Transient )
         func_string_setter< Meta_Field, bool >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Transient, field_value );
      else if( field_name == c_field_id_Type || field_name == c_field_name_Type )
         func_string_setter< Meta_Field, Meta_Type >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Type, field_value );
      else if( field_name == c_field_id_UOM || field_name == c_field_name_UOM )
         func_string_setter< Meta_Field, int >(
          *cmd_handler.p_Meta_Field, &Meta_Field::UOM, field_value );
      else if( field_name == c_field_id_Use_In_Text_Search || field_name == c_field_name_Use_In_Text_Search )
         func_string_setter< Meta_Field, bool >(
          *cmd_handler.p_Meta_Field, &Meta_Field::Use_In_Text_Search, field_value );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Field_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Field_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_parm_Meta_Field_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( field_name == c_field_id_Numeric_Decimals || field_name == c_field_name_Numeric_Decimals )
      {
         numeric Numeric_Decimals( cmd_handler.p_Meta_Field->Numeric_Decimals( ) );
         execute_command( Numeric_Decimals, cmd_and_args, cmd_handler.retval );
         cmd_handler.p_Meta_Field->Numeric_Decimals( Numeric_Decimals );
      }
      else if( field_name == c_field_id_Class || field_name == c_field_name_Class )
         cmd_handler.retval = cmd_handler.p_Meta_Field->Class( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Enum || field_name == c_field_name_Enum )
         cmd_handler.retval = cmd_handler.p_Meta_Field->Enum( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Parent_Class || field_name == c_field_name_Parent_Class )
         cmd_handler.retval = cmd_handler.p_Meta_Field->Parent_Class( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Source_Field || field_name == c_field_name_Source_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Field->Source_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Type || field_name == c_field_name_Type )
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

   const string& impl_Default( ) const { return lazy_fetch( p_obj ), v_Default; }
   void impl_Default( const string& Default ) { v_Default = Default; }

   int impl_Extra( ) const { return lazy_fetch( p_obj ), v_Extra; }
   void impl_Extra( int Extra ) { v_Extra = Extra; }

   const string& impl_Id( ) const { return lazy_fetch( p_obj ), v_Id; }
   void impl_Id( const string& Id ) { v_Id = Id; }

   bool impl_Internal( ) const { return lazy_fetch( p_obj ), v_Internal; }
   void impl_Internal( bool Internal ) { v_Internal = Internal; }

   bool impl_Is_Foreign_Key( ) const { return lazy_fetch( p_obj ), v_Is_Foreign_Key; }
   void impl_Is_Foreign_Key( bool Is_Foreign_Key ) { v_Is_Foreign_Key = Is_Foreign_Key; }

   bool impl_Is_Text_Type( ) const { return lazy_fetch( p_obj ), v_Is_Text_Type; }
   void impl_Is_Text_Type( bool Is_Text_Type ) { v_Is_Text_Type = Is_Text_Type; }

   bool impl_Mandatory( ) const { return lazy_fetch( p_obj ), v_Mandatory; }
   void impl_Mandatory( bool Mandatory ) { v_Mandatory = Mandatory; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { v_Name = Name; }

   const numeric& impl_Numeric_Decimals( ) const { return lazy_fetch( p_obj ), v_Numeric_Decimals; }
   void impl_Numeric_Decimals( const numeric& Numeric_Decimals ) { v_Numeric_Decimals = Numeric_Decimals; }

   const string& impl_Parent_Class_Name( ) const { return lazy_fetch( p_obj ), v_Parent_Class_Name; }
   void impl_Parent_Class_Name( const string& Parent_Class_Name ) { v_Parent_Class_Name = Parent_Class_Name; }

   int impl_Primitive( ) const { return lazy_fetch( p_obj ), v_Primitive; }
   void impl_Primitive( int Primitive ) { v_Primitive = Primitive; }

   bool impl_Transient( ) const { return lazy_fetch( p_obj ), v_Transient; }
   void impl_Transient( bool Transient ) { v_Transient = Transient; }

   int impl_UOM( ) const { return lazy_fetch( p_obj ), v_UOM; }
   void impl_UOM( int UOM ) { v_UOM = UOM; }

   bool impl_Use_In_Text_Search( ) const { return lazy_fetch( p_obj ), v_Use_In_Text_Search; }
   void impl_Use_In_Text_Search( bool Use_In_Text_Search ) { v_Use_In_Text_Search = Use_In_Text_Search; }

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

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Actions( )
   {
      if( !cp_child_Specification_Content_Page_Actions )
      {
         cp_child_Specification_Content_Page_Actions.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Actions, "302629" );
      }
      return *cp_child_Specification_Content_Page_Actions;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Actions( ) const
   {
      if( !cp_child_Specification_Content_Page_Actions )
      {
         cp_child_Specification_Content_Page_Actions.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Actions, "302629" );
      }
      return *cp_child_Specification_Content_Page_Actions;
   }

   Meta_Specification_Copy_Child_Links& impl_child_Specification_Copy_Child_Links_Child_File( )
   {
      if( !cp_child_Specification_Copy_Child_Links_Child_File )
      {
         cp_child_Specification_Copy_Child_Links_Child_File.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Copy_Child_Links_Child_File, "302650" );
      }
      return *cp_child_Specification_Copy_Child_Links_Child_File;
   }

   const Meta_Specification_Copy_Child_Links& impl_child_Specification_Copy_Child_Links_Child_File( ) const
   {
      if( !cp_child_Specification_Copy_Child_Links_Child_File )
      {
         cp_child_Specification_Copy_Child_Links_Child_File.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Copy_Child_Links_Child_File, "302650" );
      }
      return *cp_child_Specification_Copy_Child_Links_Child_File;
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

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Clone_Permitted( )
   {
      if( !cp_child_Specification_Content_Page_Clone_Permitted )
      {
         cp_child_Specification_Content_Page_Clone_Permitted.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Clone_Permitted, "302607" );
      }
      return *cp_child_Specification_Content_Page_Clone_Permitted;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Clone_Permitted( ) const
   {
      if( !cp_child_Specification_Content_Page_Clone_Permitted )
      {
         cp_child_Specification_Content_Page_Clone_Permitted.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Clone_Permitted, "302607" );
      }
      return *cp_child_Specification_Content_Page_Clone_Permitted;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Code( )
   {
      if( !cp_child_Specification_Content_Page_Code )
      {
         cp_child_Specification_Content_Page_Code.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Code, "302610" );
      }
      return *cp_child_Specification_Content_Page_Code;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Code( ) const
   {
      if( !cp_child_Specification_Content_Page_Code )
      {
         cp_child_Specification_Content_Page_Code.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Code, "302610" );
      }
      return *cp_child_Specification_Content_Page_Code;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Content_Type( )
   {
      if( !cp_child_Specification_Content_Page_Content_Type )
      {
         cp_child_Specification_Content_Page_Content_Type.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Content_Type, "302605" );
      }
      return *cp_child_Specification_Content_Page_Content_Type;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Content_Type( ) const
   {
      if( !cp_child_Specification_Content_Page_Content_Type )
      {
         cp_child_Specification_Content_Page_Content_Type.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Content_Type, "302605" );
      }
      return *cp_child_Specification_Content_Page_Content_Type;
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

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_File_Name( )
   {
      if( !cp_child_Specification_Content_Page_File_Name )
      {
         cp_child_Specification_Content_Page_File_Name.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_File_Name, "302612" );
      }
      return *cp_child_Specification_Content_Page_File_Name;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_File_Name( ) const
   {
      if( !cp_child_Specification_Content_Page_File_Name )
      {
         cp_child_Specification_Content_Page_File_Name.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_File_Name, "302612" );
      }
      return *cp_child_Specification_Content_Page_File_Name;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_File_Name_Gen( )
   {
      if( !cp_child_Specification_Content_Page_File_Name_Gen )
      {
         cp_child_Specification_Content_Page_File_Name_Gen.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_File_Name_Gen, "302608" );
      }
      return *cp_child_Specification_Content_Page_File_Name_Gen;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_File_Name_Gen( ) const
   {
      if( !cp_child_Specification_Content_Page_File_Name_Gen )
      {
         cp_child_Specification_Content_Page_File_Name_Gen.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_File_Name_Gen, "302608" );
      }
      return *cp_child_Specification_Content_Page_File_Name_Gen;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Frag_Code( )
   {
      if( !cp_child_Specification_Content_Page_Frag_Code )
      {
         cp_child_Specification_Content_Page_Frag_Code.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Frag_Code, "302640" );
      }
      return *cp_child_Specification_Content_Page_Frag_Code;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Frag_Code( ) const
   {
      if( !cp_child_Specification_Content_Page_Frag_Code )
      {
         cp_child_Specification_Content_Page_Frag_Code.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Frag_Code, "302640" );
      }
      return *cp_child_Specification_Content_Page_Frag_Code;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Frag_Content( )
   {
      if( !cp_child_Specification_Content_Page_Frag_Content )
      {
         cp_child_Specification_Content_Page_Frag_Content.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Frag_Content, "302642" );
      }
      return *cp_child_Specification_Content_Page_Frag_Content;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Frag_Content( ) const
   {
      if( !cp_child_Specification_Content_Page_Frag_Content )
      {
         cp_child_Specification_Content_Page_Frag_Content.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Frag_Content, "302642" );
      }
      return *cp_child_Specification_Content_Page_Frag_Content;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Frag_Group( )
   {
      if( !cp_child_Specification_Content_Page_Frag_Group )
      {
         cp_child_Specification_Content_Page_Frag_Group.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Frag_Group, "302641" );
      }
      return *cp_child_Specification_Content_Page_Frag_Group;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Frag_Group( ) const
   {
      if( !cp_child_Specification_Content_Page_Frag_Group )
      {
         cp_child_Specification_Content_Page_Frag_Group.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Frag_Group, "302641" );
      }
      return *cp_child_Specification_Content_Page_Frag_Group;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Generate_Children( )
   {
      if( !cp_child_Specification_Content_Page_Generate_Children )
      {
         cp_child_Specification_Content_Page_Generate_Children.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Generate_Children, "302619" );
      }
      return *cp_child_Specification_Content_Page_Generate_Children;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Generate_Children( ) const
   {
      if( !cp_child_Specification_Content_Page_Generate_Children )
      {
         cp_child_Specification_Content_Page_Generate_Children.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Generate_Children, "302619" );
      }
      return *cp_child_Specification_Content_Page_Generate_Children;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Group_Base_Path( )
   {
      if( !cp_child_Specification_Content_Page_Group_Base_Path )
      {
         cp_child_Specification_Content_Page_Group_Base_Path.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Group_Base_Path, "302647" );
      }
      return *cp_child_Specification_Content_Page_Group_Base_Path;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Group_Base_Path( ) const
   {
      if( !cp_child_Specification_Content_Page_Group_Base_Path )
      {
         cp_child_Specification_Content_Page_Group_Base_Path.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Group_Base_Path, "302647" );
      }
      return *cp_child_Specification_Content_Page_Group_Base_Path;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Group_Extra( )
   {
      if( !cp_child_Specification_Content_Page_Group_Extra )
      {
         cp_child_Specification_Content_Page_Group_Extra.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Group_Extra, "302646" );
      }
      return *cp_child_Specification_Content_Page_Group_Extra;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Group_Extra( ) const
   {
      if( !cp_child_Specification_Content_Page_Group_Extra )
      {
         cp_child_Specification_Content_Page_Group_Extra.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Group_Extra, "302646" );
      }
      return *cp_child_Specification_Content_Page_Group_Extra;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Group( )
   {
      if( !cp_child_Specification_Content_Page_Group )
      {
         cp_child_Specification_Content_Page_Group.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Group, "302609" );
      }
      return *cp_child_Specification_Content_Page_Group;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Group( ) const
   {
      if( !cp_child_Specification_Content_Page_Group )
      {
         cp_child_Specification_Content_Page_Group.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Group, "302609" );
      }
      return *cp_child_Specification_Content_Page_Group;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Group_Name( )
   {
      if( !cp_child_Specification_Content_Page_Group_Name )
      {
         cp_child_Specification_Content_Page_Group_Name.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Group_Name, "302645" );
      }
      return *cp_child_Specification_Content_Page_Group_Name;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Group_Name( ) const
   {
      if( !cp_child_Specification_Content_Page_Group_Name )
      {
         cp_child_Specification_Content_Page_Group_Name.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Group_Name, "302645" );
      }
      return *cp_child_Specification_Content_Page_Group_Name;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Hyperlink_Title( )
   {
      if( !cp_child_Specification_Content_Page_Hyperlink_Title )
      {
         cp_child_Specification_Content_Page_Hyperlink_Title.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Hyperlink_Title, "302618" );
      }
      return *cp_child_Specification_Content_Page_Hyperlink_Title;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Hyperlink_Title( ) const
   {
      if( !cp_child_Specification_Content_Page_Hyperlink_Title )
      {
         cp_child_Specification_Content_Page_Hyperlink_Title.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Hyperlink_Title, "302618" );
      }
      return *cp_child_Specification_Content_Page_Hyperlink_Title;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Link_Content( )
   {
      if( !cp_child_Specification_Content_Page_Link_Content )
      {
         cp_child_Specification_Content_Page_Link_Content.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Link_Content, "302617" );
      }
      return *cp_child_Specification_Content_Page_Link_Content;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Link_Content( ) const
   {
      if( !cp_child_Specification_Content_Page_Link_Content )
      {
         cp_child_Specification_Content_Page_Link_Content.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Link_Content, "302617" );
      }
      return *cp_child_Specification_Content_Page_Link_Content;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Link_Group( )
   {
      if( !cp_child_Specification_Content_Page_Link_Group )
      {
         cp_child_Specification_Content_Page_Link_Group.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Link_Group, "302622" );
      }
      return *cp_child_Specification_Content_Page_Link_Group;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Link_Group( ) const
   {
      if( !cp_child_Specification_Content_Page_Link_Group )
      {
         cp_child_Specification_Content_Page_Link_Group.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Link_Group, "302622" );
      }
      return *cp_child_Specification_Content_Page_Link_Group;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Link_Name( )
   {
      if( !cp_child_Specification_Content_Page_Link_Name )
      {
         cp_child_Specification_Content_Page_Link_Name.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Link_Name, "302621" );
      }
      return *cp_child_Specification_Content_Page_Link_Name;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Link_Name( ) const
   {
      if( !cp_child_Specification_Content_Page_Link_Name )
      {
         cp_child_Specification_Content_Page_Link_Name.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Link_Name, "302621" );
      }
      return *cp_child_Specification_Content_Page_Link_Name;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Meta_Content( )
   {
      if( !cp_child_Specification_Content_Page_Meta_Content )
      {
         cp_child_Specification_Content_Page_Meta_Content.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Meta_Content, "302616" );
      }
      return *cp_child_Specification_Content_Page_Meta_Content;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Meta_Content( ) const
   {
      if( !cp_child_Specification_Content_Page_Meta_Content )
      {
         cp_child_Specification_Content_Page_Meta_Content.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Meta_Content, "302616" );
      }
      return *cp_child_Specification_Content_Page_Meta_Content;
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

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Next_Child_Num( )
   {
      if( !cp_child_Specification_Content_Page_Next_Child_Num )
      {
         cp_child_Specification_Content_Page_Next_Child_Num.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Next_Child_Num, "302643" );
      }
      return *cp_child_Specification_Content_Page_Next_Child_Num;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Next_Child_Num( ) const
   {
      if( !cp_child_Specification_Content_Page_Next_Child_Num )
      {
         cp_child_Specification_Content_Page_Next_Child_Num.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Next_Child_Num, "302643" );
      }
      return *cp_child_Specification_Content_Page_Next_Child_Num;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Order( )
   {
      if( !cp_child_Specification_Content_Page_Order )
      {
         cp_child_Specification_Content_Page_Order.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Order, "302630" );
      }
      return *cp_child_Specification_Content_Page_Order;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Order( ) const
   {
      if( !cp_child_Specification_Content_Page_Order )
      {
         cp_child_Specification_Content_Page_Order.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Order, "302630" );
      }
      return *cp_child_Specification_Content_Page_Order;
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

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Owned_Links( )
   {
      if( !cp_child_Specification_Content_Page_Owned_Links )
      {
         cp_child_Specification_Content_Page_Owned_Links.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Owned_Links, "302623" );
      }
      return *cp_child_Specification_Content_Page_Owned_Links;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Owned_Links( ) const
   {
      if( !cp_child_Specification_Content_Page_Owned_Links )
      {
         cp_child_Specification_Content_Page_Owned_Links.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Owned_Links, "302623" );
      }
      return *cp_child_Specification_Content_Page_Owned_Links;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Page( )
   {
      if( !cp_child_Specification_Content_Page_Page )
      {
         cp_child_Specification_Content_Page_Page.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Page, "302631" );
      }
      return *cp_child_Specification_Content_Page_Page;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Page( ) const
   {
      if( !cp_child_Specification_Content_Page_Page )
      {
         cp_child_Specification_Content_Page_Page.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Page, "302631" );
      }
      return *cp_child_Specification_Content_Page_Page;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Page_File( )
   {
      if( !cp_child_Specification_Content_Page_Page_File )
      {
         cp_child_Specification_Content_Page_Page_File.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Page_File, "302635" );
      }
      return *cp_child_Specification_Content_Page_Page_File;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Page_File( ) const
   {
      if( !cp_child_Specification_Content_Page_Page_File )
      {
         cp_child_Specification_Content_Page_Page_File.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Page_File, "302635" );
      }
      return *cp_child_Specification_Content_Page_Page_File;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Page_File_Name( )
   {
      if( !cp_child_Specification_Content_Page_Page_File_Name )
      {
         cp_child_Specification_Content_Page_Page_File_Name.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Page_File_Name, "302636" );
      }
      return *cp_child_Specification_Content_Page_Page_File_Name;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Page_File_Name( ) const
   {
      if( !cp_child_Specification_Content_Page_Page_File_Name )
      {
         cp_child_Specification_Content_Page_Page_File_Name.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Page_File_Name, "302636" );
      }
      return *cp_child_Specification_Content_Page_Page_File_Name;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Page_File_Name_Gen( )
   {
      if( !cp_child_Specification_Content_Page_Page_File_Name_Gen )
      {
         cp_child_Specification_Content_Page_Page_File_Name_Gen.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Page_File_Name_Gen, "302606" );
      }
      return *cp_child_Specification_Content_Page_Page_File_Name_Gen;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Page_File_Name_Gen( ) const
   {
      if( !cp_child_Specification_Content_Page_Page_File_Name_Gen )
      {
         cp_child_Specification_Content_Page_Page_File_Name_Gen.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Page_File_Name_Gen, "302606" );
      }
      return *cp_child_Specification_Content_Page_Page_File_Name_Gen;
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

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Parent_Page( )
   {
      if( !cp_child_Specification_Content_Page_Parent_Page )
      {
         cp_child_Specification_Content_Page_Parent_Page.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Parent_Page, "302614" );
      }
      return *cp_child_Specification_Content_Page_Parent_Page;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Parent_Page( ) const
   {
      if( !cp_child_Specification_Content_Page_Parent_Page )
      {
         cp_child_Specification_Content_Page_Parent_Page.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Parent_Page, "302614" );
      }
      return *cp_child_Specification_Content_Page_Parent_Page;
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

   Meta_Specification& impl_child_Specification_Source_Child_2( )
   {
      if( !cp_child_Specification_Source_Child_2 )
      {
         cp_child_Specification_Source_Child_2.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Source_Child_2, "301496" );
      }
      return *cp_child_Specification_Source_Child_2;
   }

   const Meta_Specification& impl_child_Specification_Source_Child_2( ) const
   {
      if( !cp_child_Specification_Source_Child_2 )
      {
         cp_child_Specification_Source_Child_2.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Source_Child_2, "301496" );
      }
      return *cp_child_Specification_Source_Child_2;
   }

   Meta_Specification_Copy_Child_Links& impl_child_Specification_Copy_Child_Links_Source_Child_File( )
   {
      if( !cp_child_Specification_Copy_Child_Links_Source_Child_File )
      {
         cp_child_Specification_Copy_Child_Links_Source_Child_File.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Copy_Child_Links_Source_Child_File, "302653" );
      }
      return *cp_child_Specification_Copy_Child_Links_Source_Child_File;
   }

   const Meta_Specification_Copy_Child_Links& impl_child_Specification_Copy_Child_Links_Source_Child_File( ) const
   {
      if( !cp_child_Specification_Copy_Child_Links_Source_Child_File )
      {
         cp_child_Specification_Copy_Child_Links_Source_Child_File.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Copy_Child_Links_Source_Child_File, "302653" );
      }
      return *cp_child_Specification_Copy_Child_Links_Source_Child_File;
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

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Sub_Title( )
   {
      if( !cp_child_Specification_Content_Page_Sub_Title )
      {
         cp_child_Specification_Content_Page_Sub_Title.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Sub_Title, "302613" );
      }
      return *cp_child_Specification_Content_Page_Sub_Title;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Sub_Title( ) const
   {
      if( !cp_child_Specification_Content_Page_Sub_Title )
      {
         cp_child_Specification_Content_Page_Sub_Title.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Sub_Title, "302613" );
      }
      return *cp_child_Specification_Content_Page_Sub_Title;
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

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Text_Content( )
   {
      if( !cp_child_Specification_Content_Page_Text_Content )
      {
         cp_child_Specification_Content_Page_Text_Content.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Text_Content, "302615" );
      }
      return *cp_child_Specification_Content_Page_Text_Content;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Text_Content( ) const
   {
      if( !cp_child_Specification_Content_Page_Text_Content )
      {
         cp_child_Specification_Content_Page_Text_Content.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Text_Content, "302615" );
      }
      return *cp_child_Specification_Content_Page_Text_Content;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Title( )
   {
      if( !cp_child_Specification_Content_Page_Title )
      {
         cp_child_Specification_Content_Page_Title.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Title, "302611" );
      }
      return *cp_child_Specification_Content_Page_Title;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Title( ) const
   {
      if( !cp_child_Specification_Content_Page_Title )
      {
         cp_child_Specification_Content_Page_Title.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Title, "302611" );
      }
      return *cp_child_Specification_Content_Page_Title;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Variable_Name( )
   {
      if( !cp_child_Specification_Content_Page_Variable_Name )
      {
         cp_child_Specification_Content_Page_Variable_Name.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Variable_Name, "302638" );
      }
      return *cp_child_Specification_Content_Page_Variable_Name;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Variable_Name( ) const
   {
      if( !cp_child_Specification_Content_Page_Variable_Name )
      {
         cp_child_Specification_Content_Page_Variable_Name.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Variable_Name, "302638" );
      }
      return *cp_child_Specification_Content_Page_Variable_Name;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Variable_Value( )
   {
      if( !cp_child_Specification_Content_Page_Variable_Value )
      {
         cp_child_Specification_Content_Page_Variable_Value.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Variable_Value, "302639" );
      }
      return *cp_child_Specification_Content_Page_Variable_Value;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Variable_Value( ) const
   {
      if( !cp_child_Specification_Content_Page_Variable_Value )
      {
         cp_child_Specification_Content_Page_Variable_Value.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Variable_Value, "302639" );
      }
      return *cp_child_Specification_Content_Page_Variable_Value;
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

   uint64_t get_state( ) const;

   const string& execute( const string& cmd_and_args );

   void clear_foreign_key( const string& field );

   void set_foreign_key_value( const string& field, const string& value );

   const string& get_foreign_key_value( const string& field );

   void get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const;

   void add_extra_paging_info( vector< pair< string, string > >& paging_info ) const;

   void clear( );

   bool value_will_be_provided( const string& field_name );

   void validate( unsigned state, bool is_internal, validation_error_container* p_validation_errors );
   void validate_set_fields( set< string >& fields_set, validation_error_container* p_validation_errors );

   void after_fetch( );
   void finalise_fetch( );

   void at_create( );

   void to_store( bool is_create, bool is_internal );
   void for_store( bool is_create, bool is_internal );
   void after_store( bool is_create, bool is_internal );

   bool can_destroy( bool is_internal );
   void for_destroy( bool is_internal );
   void after_destroy( bool is_internal );

   void set_default_values( );

   bool is_filtered( ) const;

   Meta_Field* p_obj;
   class_pointer< Meta_Field > cp_obj;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   string v_Default;
   int v_Extra;
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
   bool v_Use_In_Text_Search;

   string v_Class;
   mutable class_pointer< Meta_Class > cp_Class;

   string v_Enum;
   mutable class_pointer< Meta_Enum > cp_Enum;

   string v_Parent_Class;
   mutable class_pointer< Meta_Class > cp_Parent_Class;

   string v_Source_Field;
   mutable class_pointer< Meta_Field > cp_Source_Field;

   string v_Type;
   mutable class_pointer< Meta_Type > cp_Type;

   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Actions;
   mutable class_pointer< Meta_Specification_Copy_Child_Links > cp_child_Specification_Copy_Child_Links_Child_File;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Child_Rel_Source_Child;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Child_Rel_Source;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Child_Rel_Source_Parent;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Clone_Permitted;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Code;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Content_Type;
   mutable class_pointer< Meta_Index > cp_child_Index_Field_1;
   mutable class_pointer< Meta_Index > cp_child_Index_Field_2;
   mutable class_pointer< Meta_Index > cp_child_Index_Field_3;
   mutable class_pointer< Meta_Index > cp_child_Index_Field_4;
   mutable class_pointer< Meta_Index > cp_child_Index_Field_5;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_File_Name;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_File_Name_Gen;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Frag_Code;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Frag_Content;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Frag_Group;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Generate_Children;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Group_Base_Path;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Group_Extra;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Group;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Group_Name;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Hyperlink_Title;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Link_Content;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Link_Group;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Link_Name;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Meta_Content;
   mutable class_pointer< Meta_Specification_Field_Action > cp_child_Specification_Field_Action_New_Record_FK;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Next_Child_Num;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Order;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Other_Field_2;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Other;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Owned_Links;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Page;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Page_File;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Page_File_Name;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Page_File_Name_Gen;
   mutable class_pointer< Meta_List > cp_child_List_Parent;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Parent_Page;
   mutable class_pointer< Meta_Class > cp_child_Class_Quick_Link;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Restriction;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Source_Child_2;
   mutable class_pointer< Meta_Specification_Copy_Child_Links > cp_child_Specification_Copy_Child_Links_Source_Child_File;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Source_Child;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Source_Child;
   mutable class_pointer< Meta_View_Field > cp_child_View_Field_Source_Child;
   mutable class_pointer< Meta_View_Field > cp_child_View_Field_Source_Edit_Child;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Source;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Source;
   mutable class_pointer< Meta_View_Field > cp_child_View_Field_Source;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Source_Grandchild;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Source_Grandchild;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Source_Parent;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Source_Parent;
   mutable class_pointer< Meta_View_Field > cp_child_View_Field_Source_Parent;
   mutable class_pointer< Meta_Field > cp_child_Field_Source;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Sub_Title;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Test_Child;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Test;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Test_Parent;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Text_Content;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Title;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Variable_Name;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Variable_Value;
   mutable class_pointer< Meta_Initial_Record_Value > cp_child_Initial_Record_Value;
   mutable class_pointer< Meta_Modifier_Affect > cp_child_Modifier_Affect;
   mutable class_pointer< Meta_Specification > cp_child_Specification;
};

void Meta_Field::impl::impl_Get_Text_Type( int& Result )
{
   // [<start Get_Text_Type_impl>]
//nyi
   Result = e_sql_char_type_std;

   if( get_obj( ).Extra( ) == c_enum_field_extra_html )
      Result = e_sql_char_type_large;

   if( get_obj( ).Extra( ) == c_enum_field_extra_text )
      Result = e_sql_char_type_large;

   if( get_obj( ).Extra( ) == c_enum_field_extra_notes )
      Result = e_sql_char_type_large;

   if( get_obj( ).Type( ).Max_Size( )
    && get_obj( ).Type( ).Max_Size( ) <= c_sql_small_char_size )
      Result = e_sql_char_type_small;

   if( get_obj( ).Type( ).Max_Size( ) > c_sql_std_char_size )
      Result = e_sql_char_type_large;

   if( get_obj( ).Extra( ) == c_enum_field_extra_user_perms )
      Result = e_sql_char_type_large;

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
      retval = to_string( impl_Class( ) );
      break;

      case 1:
      retval = to_string( impl_Default( ) );
      break;

      case 2:
      retval = to_string( impl_Enum( ) );
      break;

      case 3:
      retval = to_string( impl_Extra( ) );
      break;

      case 4:
      retval = to_string( impl_Id( ) );
      break;

      case 5:
      retval = to_string( impl_Internal( ) );
      break;

      case 6:
      retval = to_string( impl_Is_Foreign_Key( ) );
      break;

      case 7:
      retval = to_string( impl_Is_Text_Type( ) );
      break;

      case 8:
      retval = to_string( impl_Mandatory( ) );
      break;

      case 9:
      retval = to_string( impl_Name( ) );
      break;

      case 10:
      retval = to_string( impl_Numeric_Decimals( ) );
      break;

      case 11:
      retval = to_string( impl_Parent_Class( ) );
      break;

      case 12:
      retval = to_string( impl_Parent_Class_Name( ) );
      break;

      case 13:
      retval = to_string( impl_Primitive( ) );
      break;

      case 14:
      retval = to_string( impl_Source_Field( ) );
      break;

      case 15:
      retval = to_string( impl_Transient( ) );
      break;

      case 16:
      retval = to_string( impl_Type( ) );
      break;

      case 17:
      retval = to_string( impl_UOM( ) );
      break;

      case 18:
      retval = to_string( impl_Use_In_Text_Search( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }

   return retval;
}

void Meta_Field::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Field::impl, Meta_Class >( *this, &Meta_Field::impl::impl_Class, value );
      break;

      case 1:
      func_string_setter< Meta_Field::impl, string >( *this, &Meta_Field::impl::impl_Default, value );
      break;

      case 2:
      func_string_setter< Meta_Field::impl, Meta_Enum >( *this, &Meta_Field::impl::impl_Enum, value );
      break;

      case 3:
      func_string_setter< Meta_Field::impl, int >( *this, &Meta_Field::impl::impl_Extra, value );
      break;

      case 4:
      func_string_setter< Meta_Field::impl, string >( *this, &Meta_Field::impl::impl_Id, value );
      break;

      case 5:
      func_string_setter< Meta_Field::impl, bool >( *this, &Meta_Field::impl::impl_Internal, value );
      break;

      case 6:
      func_string_setter< Meta_Field::impl, bool >( *this, &Meta_Field::impl::impl_Is_Foreign_Key, value );
      break;

      case 7:
      func_string_setter< Meta_Field::impl, bool >( *this, &Meta_Field::impl::impl_Is_Text_Type, value );
      break;

      case 8:
      func_string_setter< Meta_Field::impl, bool >( *this, &Meta_Field::impl::impl_Mandatory, value );
      break;

      case 9:
      func_string_setter< Meta_Field::impl, string >( *this, &Meta_Field::impl::impl_Name, value );
      break;

      case 10:
      func_string_setter< Meta_Field::impl, numeric >( *this, &Meta_Field::impl::impl_Numeric_Decimals, value );
      break;

      case 11:
      func_string_setter< Meta_Field::impl, Meta_Class >( *this, &Meta_Field::impl::impl_Parent_Class, value );
      break;

      case 12:
      func_string_setter< Meta_Field::impl, string >( *this, &Meta_Field::impl::impl_Parent_Class_Name, value );
      break;

      case 13:
      func_string_setter< Meta_Field::impl, int >( *this, &Meta_Field::impl::impl_Primitive, value );
      break;

      case 14:
      func_string_setter< Meta_Field::impl, Meta_Field >( *this, &Meta_Field::impl::impl_Source_Field, value );
      break;

      case 15:
      func_string_setter< Meta_Field::impl, bool >( *this, &Meta_Field::impl::impl_Transient, value );
      break;

      case 16:
      func_string_setter< Meta_Field::impl, Meta_Type >( *this, &Meta_Field::impl::impl_Type, value );
      break;

      case 17:
      func_string_setter< Meta_Field::impl, int >( *this, &Meta_Field::impl::impl_UOM, value );
      break;

      case 18:
      func_string_setter< Meta_Field::impl, bool >( *this, &Meta_Field::impl::impl_Use_In_Text_Search, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }
}

uint64_t Meta_Field::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start modifier_field_value)]
   if( get_obj( ).Type( ).Primitive( ) == 5 ) // i.e. int
      state |= c_modifier_Is_Any_Non_Text_Type;
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   if( get_obj( ).Type( ).Primitive( ) == 6 ) // i.e. bool
      state |= c_modifier_Is_Any_Non_Text_Type;
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   if( get_obj( ).Type( ).Primitive( ) == 4 ) // i.e. numeric
      state |= c_modifier_Is_Any_Non_Text_Type;
   // [(finish modifier_field_value)]

   // [(start protect_equal)]
   if( check_equal( get_obj( ).Internal( ), true ) )
      state |= ( c_state_uneditable | c_state_undeletable );
   // [(finish protect_equal)]

   // [(start modifier_field_value)]
   if( get_obj( ).Internal( ) == true )
      state |= c_modifier_Is_Internal;
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   if( get_obj( ).Transient( ) == true )
      state |= c_modifier_Is_Transient;
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   if( get_obj( ).Type( ).Primitive( ) == 6 ) // i.e. bool
      state |= c_modifier_Is_Type_bool;
   // [(finish modifier_field_value)]

   // [<start get_state>]
   // [<finish get_state>]

   return state;
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
   else if( field == c_field_id_Class || field == c_field_name_Class )
      impl_Class( "" );
   else if( field == c_field_id_Enum || field == c_field_name_Enum )
      impl_Enum( "" );
   else if( field == c_field_id_Parent_Class || field == c_field_name_Parent_Class )
      impl_Parent_Class( "" );
   else if( field == c_field_id_Source_Field || field == c_field_name_Source_Field )
      impl_Source_Field( "" );
   else if( field == c_field_id_Type || field == c_field_name_Type )
      impl_Type( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Field::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( field == c_field_id_Class || field == c_field_name_Class )
      v_Class = value;
   else if( field == c_field_id_Enum || field == c_field_name_Enum )
      v_Enum = value;
   else if( field == c_field_id_Parent_Class || field == c_field_name_Parent_Class )
      v_Parent_Class = value;
   else if( field == c_field_id_Source_Field || field == c_field_name_Source_Field )
      v_Source_Field = value;
   else if( field == c_field_id_Type || field == c_field_name_Type )
      v_Type = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Field::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Class || field == c_field_name_Class )
      return v_Class;
   else if( field == c_field_id_Enum || field == c_field_name_Enum )
      return v_Enum;
   else if( field == c_field_id_Parent_Class || field == c_field_name_Parent_Class )
      return v_Parent_Class;
   else if( field == c_field_id_Source_Field || field == c_field_name_Source_Field )
      return v_Source_Field;
   else if( field == c_field_id_Type || field == c_field_name_Type )
      return v_Type;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Field::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Class, v_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Enum, v_Enum ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Parent_Class, v_Parent_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Field, v_Source_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Type, v_Type ) );
}

void Meta_Field::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Field::impl::clear( )
{
   v_Default = gv_default_Default;
   v_Extra = gv_default_Extra;
   v_Id = gv_default_Id;
   v_Internal = gv_default_Internal;
   v_Is_Foreign_Key = gv_default_Is_Foreign_Key;
   v_Is_Text_Type = gv_default_Is_Text_Type;
   v_Mandatory = gv_default_Mandatory;
   v_Name = gv_default_Name;
   v_Numeric_Decimals = gv_default_Numeric_Decimals;
   v_Parent_Class_Name = gv_default_Parent_Class_Name;
   v_Primitive = gv_default_Primitive;
   v_Transient = gv_default_Transient;
   v_UOM = gv_default_UOM;
   v_Use_In_Text_Search = gv_default_Use_In_Text_Search;

   v_Class = string( );
   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   v_Enum = string( );
   if( cp_Enum )
      p_obj->setup_foreign_key( *cp_Enum, v_Enum );

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

   // [(start parent_auto_int_inc)]
   if( field_name == "Id" )
      return true;
   // [(finish parent_auto_int_inc)]

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Field::impl::validate( unsigned state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   if( is_null( v_Id ) && !value_will_be_provided( c_field_name_Id ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Id,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Id ) ) ) ) );

   if( is_null( v_Name ) && !value_will_be_provided( c_field_name_Name ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Name,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Name ) ) ) ) );

   if( v_Class.empty( ) && !value_will_be_provided( c_field_name_Class ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Class ) ) ) ) );

   if( v_Type.empty( ) && !value_will_be_provided( c_field_name_Type ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Type,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Type ) ) ) ) );

   string error_message;
   if( !is_null( v_Default )
    && ( v_Default != gv_default_Default
    || !value_will_be_provided( c_field_name_Default ) )
    && !g_Default_domain.is_valid( v_Default, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Default,
       get_module_string( c_field_display_name_Default ) + " " + error_message ) );

   if( !is_null( v_Id )
    && ( v_Id != gv_default_Id
    || !value_will_be_provided( c_field_name_Id ) )
    && !g_Id_domain.is_valid( v_Id, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Id,
       get_module_string( c_field_display_name_Id ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( v_Name != gv_default_Name
    || !value_will_be_provided( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   if( !is_null( v_Parent_Class_Name )
    && ( v_Parent_Class_Name != gv_default_Parent_Class_Name
    || !value_will_be_provided( c_field_name_Parent_Class_Name ) )
    && !g_Parent_Class_Name_domain.is_valid( v_Parent_Class_Name, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Parent_Class_Name,
       get_module_string( c_field_display_name_Parent_Class_Name ) + " " + error_message ) );

   if( !g_field_extra_enum.count( v_Extra ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Extra,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Extra ) ) ) ) );

   if( !g_primitive_enum.count( v_Primitive ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Primitive,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Primitive ) ) ) ) );

   if( !g_uom_enum.count( v_UOM ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_UOM,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_UOM ) ) ) ) );

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
          c_str_parm_field_and_original_mismatch_pfield, get_module_string( c_field_display_name_Type ) ),
          make_pair( c_str_parm_field_and_original_mismatch_field, get_module_string( c_field_display_name_Primitive ) ) ) ) );
   }
   // [<finish validate>]
}

void Meta_Field::impl::validate_set_fields( set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;

   if( !is_null( v_Default )
    && ( fields_set.count( c_field_id_Default ) || fields_set.count( c_field_name_Default ) )
    && !g_Default_domain.is_valid( v_Default, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Default,
       get_module_string( c_field_display_name_Default ) + " " + error_message ) );

   if( !is_null( v_Id )
    && ( fields_set.count( c_field_id_Id ) || fields_set.count( c_field_name_Id ) )
    && !g_Id_domain.is_valid( v_Id, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Id,
       get_module_string( c_field_display_name_Id ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( fields_set.count( c_field_id_Name ) || fields_set.count( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   if( !is_null( v_Parent_Class_Name )
    && ( fields_set.count( c_field_id_Parent_Class_Name ) || fields_set.count( c_field_name_Parent_Class_Name ) )
    && !g_Parent_Class_Name_domain.is_valid( v_Parent_Class_Name, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Parent_Class_Name,
       get_module_string( c_field_display_name_Parent_Class_Name ) + " " + error_message ) );
}

void Meta_Field::impl::after_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   if( cp_Enum )
      p_obj->setup_foreign_key( *cp_Enum, v_Enum );

   if( cp_Parent_Class )
      p_obj->setup_foreign_key( *cp_Parent_Class, v_Parent_Class );

   if( cp_Source_Field )
      p_obj->setup_foreign_key( *cp_Source_Field, v_Source_Field );

   if( cp_Type )
      p_obj->setup_foreign_key( *cp_Type, v_Type );

   // [(start transient_field_alias)]
   if( get_obj( ).needs_field_value( "Parent_Class_Name" )
    || required_transients.count( "Parent_Class_Name" ) )
      get_obj( ).Parent_Class_Name( get_obj( ).Parent_Class( ).Name( ) );
   // [(finish transient_field_alias)]

   // [(start transient_field_alias)]
   if( get_obj( ).needs_field_value( "Numeric_Decimals" )
    || required_transients.count( "Numeric_Decimals" ) )
      get_obj( ).Numeric_Decimals( get_obj( ).Type( ).Numeric_Decimals( ) );
   // [(finish transient_field_alias)]

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Field::impl::finalise_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Field::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Field::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_from_other_field)]
   get_obj( ).Primitive( get_obj( ).Type( ).Primitive( ) );
   // [(finish field_from_other_field)]

   // [(start field_from_other_field)]
   if( !is_null( get_obj( ).Parent_Class( ) ) )
      get_obj( ).Is_Foreign_Key( get_obj( ).Parent_Class( ).get_is_singular( ) );
   else
      get_obj( ).Is_Foreign_Key( false );
   // [(finish field_from_other_field)]

   // [(start modifier_set_field)]
   if( state & c_modifier_Is_Any_Non_Text_Type )
      get_obj( ).Mandatory( true );
   // [(finish modifier_set_field)]

   // [(start field_empty_action)]
   if( !get_obj( ).get_key( ).empty( ) )
   {
      if( !is_null( get_obj( ).Class( ).Source_Class( ) ) )
         get_obj( ).Internal( true );
   }
   // [(finish field_empty_action)]

   // [(start modifier_set_field)]
   if( state & c_modifier_Is_Any_Non_Text_Type )
      get_obj( ).Is_Text_Type( false );
   // [(finish modifier_set_field)]

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Field::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [(start parent_auto_int_inc)]
   if( is_create && is_null( get_obj( ).Id( ) ) )
   {
      if( is_null( get_obj( ).Class( ) ) )
         throw runtime_error( "unexpected empty Class" );

      get_obj( ).Class( ).op_update( get_obj( ).Class( ), FIELD_NAME( Meta, Class, Next_Field_Id ) );

      get_obj( ).Id( get_obj( ).Class( ).Next_Field_Id( ) );

      get_obj( ).Class( ).Next_Field_Id( auto_int_increment( get_obj( ).Class( ).Next_Field_Id( ) ) );
      get_obj( ).Class( ).op_apply( );
   }
   // [(finish parent_auto_int_inc)]

   // [<start for_store>]
//nyi
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

   // [(start update_children)]
   if( !is_create
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification( ).iterate_forwards( ) )
   {
      do
      {
         get_obj( ).child_Specification( ).op_update( );
         get_obj( ).child_Specification( ).op_apply( );
      } while( get_obj( ).child_Specification( ).iterate_next( ) );
   }
   // [(finish update_children)]

   // [(start update_children)]
   if( !is_create
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Source( ).iterate_forwards( ) )
   {
      do
      {
         get_obj( ).child_Specification_Source( ).op_update( );
         get_obj( ).child_Specification_Source( ).op_apply( );
      } while( get_obj( ).child_Specification_Source( ).iterate_next( ) );
   }
   // [(finish update_children)]

   // [(start update_children)]
   if( !is_create
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Source_Parent( ).iterate_forwards( ) )
   {
      do
      {
         get_obj( ).child_Specification_Source_Parent( ).op_update( );
         get_obj( ).child_Specification_Source_Parent( ).op_apply( );
      } while( get_obj( ).child_Specification_Source_Parent( ).iterate_next( ) );
   }
   // [(finish update_children)]

   // [(start update_children)]
   if( !is_create
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Source_Child( ).iterate_forwards( ) )
   {
      do
      {
         get_obj( ).child_Specification_Source_Child( ).op_update( );
         get_obj( ).child_Specification_Source_Child( ).op_apply( );
      } while( get_obj( ).child_Specification_Source_Child( ).iterate_next( ) );
   }
   // [(finish update_children)]

   // [(start update_children)]
   if( !is_create
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Test( ).iterate_forwards( ) )
   {
      do
      {
         get_obj( ).child_Specification_Test( ).op_update( );
         get_obj( ).child_Specification_Test( ).op_apply( );
      } while( get_obj( ).child_Specification_Test( ).iterate_next( ) );
   }
   // [(finish update_children)]

   // [(start update_children)]
   if( !is_create
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Test_Parent( ).iterate_forwards( ) )
   {
      do
      {
         get_obj( ).child_Specification_Test_Parent( ).op_update( );
         get_obj( ).child_Specification_Test_Parent( ).op_apply( );
      } while( get_obj( ).child_Specification_Test_Parent( ).iterate_next( ) );
   }
   // [(finish update_children)]

   // [(start update_children)]
   if( !is_create
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Test_Child( ).iterate_forwards( ) )
   {
      do
      {
         get_obj( ).child_Specification_Test_Child( ).op_update( );
         get_obj( ).child_Specification_Test_Child( ).op_apply( );
      } while( get_obj( ).child_Specification_Test_Child( ).iterate_next( ) );
   }
   // [(finish update_children)]

   // [(start clone_children_for_create)]
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
   // [(finish clone_children_for_create)]

   // [(start clone_children_for_update)]
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
   // [(finish clone_children_for_update)]

   // [(start update_children)]
   if( !is_create
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Source_Child_2( ).iterate_forwards( ) )
   {
      do
      {
         get_obj( ).child_Specification_Source_Child_2( ).op_update( );
         get_obj( ).child_Specification_Source_Child_2( ).op_apply( );
      } while( get_obj( ).child_Specification_Source_Child_2( ).iterate_next( ) );
   }
   // [(finish update_children)]

   // [(start clone_children_for_create)]
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
   // [(finish clone_children_for_create)]

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
   bool retval = is_internal || !( get_state( ) & c_state_undeletable );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Field::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Field::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Field::impl::set_default_values( )
{
   clear( );
}

bool Meta_Field::impl::is_filtered( ) const
{
   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
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

const string& Meta_Field::Default( ) const
{
   return p_impl->impl_Default( );
}

void Meta_Field::Default( const string& Default )
{
   p_impl->impl_Default( Default );
}

int Meta_Field::Extra( ) const
{
   return p_impl->impl_Extra( );
}

void Meta_Field::Extra( int Extra )
{
   p_impl->impl_Extra( Extra );
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

bool Meta_Field::Use_In_Text_Search( ) const
{
   return p_impl->impl_Use_In_Text_Search( );
}

void Meta_Field::Use_In_Text_Search( bool Use_In_Text_Search )
{
   p_impl->impl_Use_In_Text_Search( Use_In_Text_Search );
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

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Actions( )
{
   return p_impl->impl_child_Specification_Content_Page_Actions( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Actions( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Actions( );
}

Meta_Specification_Copy_Child_Links& Meta_Field::child_Specification_Copy_Child_Links_Child_File( )
{
   return p_impl->impl_child_Specification_Copy_Child_Links_Child_File( );
}

const Meta_Specification_Copy_Child_Links& Meta_Field::child_Specification_Copy_Child_Links_Child_File( ) const
{
   return p_impl->impl_child_Specification_Copy_Child_Links_Child_File( );
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

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Clone_Permitted( )
{
   return p_impl->impl_child_Specification_Content_Page_Clone_Permitted( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Clone_Permitted( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Clone_Permitted( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Code( )
{
   return p_impl->impl_child_Specification_Content_Page_Code( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Code( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Code( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Content_Type( )
{
   return p_impl->impl_child_Specification_Content_Page_Content_Type( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Content_Type( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Content_Type( );
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

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_File_Name( )
{
   return p_impl->impl_child_Specification_Content_Page_File_Name( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_File_Name( ) const
{
   return p_impl->impl_child_Specification_Content_Page_File_Name( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_File_Name_Gen( )
{
   return p_impl->impl_child_Specification_Content_Page_File_Name_Gen( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_File_Name_Gen( ) const
{
   return p_impl->impl_child_Specification_Content_Page_File_Name_Gen( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Frag_Code( )
{
   return p_impl->impl_child_Specification_Content_Page_Frag_Code( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Frag_Code( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Frag_Code( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Frag_Content( )
{
   return p_impl->impl_child_Specification_Content_Page_Frag_Content( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Frag_Content( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Frag_Content( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Frag_Group( )
{
   return p_impl->impl_child_Specification_Content_Page_Frag_Group( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Frag_Group( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Frag_Group( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Generate_Children( )
{
   return p_impl->impl_child_Specification_Content_Page_Generate_Children( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Generate_Children( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Generate_Children( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Group_Base_Path( )
{
   return p_impl->impl_child_Specification_Content_Page_Group_Base_Path( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Group_Base_Path( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Group_Base_Path( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Group_Extra( )
{
   return p_impl->impl_child_Specification_Content_Page_Group_Extra( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Group_Extra( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Group_Extra( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Group( )
{
   return p_impl->impl_child_Specification_Content_Page_Group( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Group( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Group( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Group_Name( )
{
   return p_impl->impl_child_Specification_Content_Page_Group_Name( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Group_Name( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Group_Name( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Hyperlink_Title( )
{
   return p_impl->impl_child_Specification_Content_Page_Hyperlink_Title( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Hyperlink_Title( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Hyperlink_Title( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Link_Content( )
{
   return p_impl->impl_child_Specification_Content_Page_Link_Content( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Link_Content( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Link_Content( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Link_Group( )
{
   return p_impl->impl_child_Specification_Content_Page_Link_Group( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Link_Group( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Link_Group( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Link_Name( )
{
   return p_impl->impl_child_Specification_Content_Page_Link_Name( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Link_Name( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Link_Name( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Meta_Content( )
{
   return p_impl->impl_child_Specification_Content_Page_Meta_Content( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Meta_Content( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Meta_Content( );
}

Meta_Specification_Field_Action& Meta_Field::child_Specification_Field_Action_New_Record_FK( )
{
   return p_impl->impl_child_Specification_Field_Action_New_Record_FK( );
}

const Meta_Specification_Field_Action& Meta_Field::child_Specification_Field_Action_New_Record_FK( ) const
{
   return p_impl->impl_child_Specification_Field_Action_New_Record_FK( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Next_Child_Num( )
{
   return p_impl->impl_child_Specification_Content_Page_Next_Child_Num( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Next_Child_Num( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Next_Child_Num( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Order( )
{
   return p_impl->impl_child_Specification_Content_Page_Order( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Order( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Order( );
}

Meta_Specification& Meta_Field::child_Specification_Other_Field_2( )
{
   return p_impl->impl_child_Specification_Other_Field_2( );
}

const Meta_Specification& Meta_Field::child_Specification_Other_Field_2( ) const
{
   return p_impl->impl_child_Specification_Other_Field_2( );
}

Meta_Specification& Meta_Field::child_Specification_Other( )
{
   return p_impl->impl_child_Specification_Other( );
}

const Meta_Specification& Meta_Field::child_Specification_Other( ) const
{
   return p_impl->impl_child_Specification_Other( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Owned_Links( )
{
   return p_impl->impl_child_Specification_Content_Page_Owned_Links( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Owned_Links( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Owned_Links( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Page( )
{
   return p_impl->impl_child_Specification_Content_Page_Page( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Page( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Page( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Page_File( )
{
   return p_impl->impl_child_Specification_Content_Page_Page_File( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Page_File( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Page_File( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Page_File_Name( )
{
   return p_impl->impl_child_Specification_Content_Page_Page_File_Name( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Page_File_Name( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Page_File_Name( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Page_File_Name_Gen( )
{
   return p_impl->impl_child_Specification_Content_Page_Page_File_Name_Gen( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Page_File_Name_Gen( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Page_File_Name_Gen( );
}

Meta_List& Meta_Field::child_List_Parent( )
{
   return p_impl->impl_child_List_Parent( );
}

const Meta_List& Meta_Field::child_List_Parent( ) const
{
   return p_impl->impl_child_List_Parent( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Parent_Page( )
{
   return p_impl->impl_child_Specification_Content_Page_Parent_Page( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Parent_Page( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Parent_Page( );
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

Meta_Specification& Meta_Field::child_Specification_Source_Child_2( )
{
   return p_impl->impl_child_Specification_Source_Child_2( );
}

const Meta_Specification& Meta_Field::child_Specification_Source_Child_2( ) const
{
   return p_impl->impl_child_Specification_Source_Child_2( );
}

Meta_Specification_Copy_Child_Links& Meta_Field::child_Specification_Copy_Child_Links_Source_Child_File( )
{
   return p_impl->impl_child_Specification_Copy_Child_Links_Source_Child_File( );
}

const Meta_Specification_Copy_Child_Links& Meta_Field::child_Specification_Copy_Child_Links_Source_Child_File( ) const
{
   return p_impl->impl_child_Specification_Copy_Child_Links_Source_Child_File( );
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

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Sub_Title( )
{
   return p_impl->impl_child_Specification_Content_Page_Sub_Title( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Sub_Title( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Sub_Title( );
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

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Text_Content( )
{
   return p_impl->impl_child_Specification_Content_Page_Text_Content( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Text_Content( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Text_Content( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Title( )
{
   return p_impl->impl_child_Specification_Content_Page_Title( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Title( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Title( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Variable_Name( )
{
   return p_impl->impl_child_Specification_Content_Page_Variable_Name( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Variable_Name( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Variable_Name( );
}

Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Variable_Value( )
{
   return p_impl->impl_child_Specification_Content_Page_Variable_Value( );
}

const Meta_Specification_Content_Page& Meta_Field::child_Specification_Content_Page_Variable_Value( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Variable_Value( );
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

bool Meta_Field::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
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

void Meta_Field::validate( unsigned state, bool is_internal )
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

void Meta_Field::finalise_fetch( )
{
   p_impl->finalise_fetch( );
}

void Meta_Field::at_create( )
{
   p_impl->at_create( );
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
   else if( name == c_field_name_Class )
   {
      p_id = c_field_id_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

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
   else if( name == c_field_name_Enum )
   {
      p_id = c_field_id_Enum;

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
   else if( id == c_field_id_Class )
   {
      p_name = c_field_name_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

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
   else if( id == c_field_id_Enum )
   {
      p_name = c_field_name_Enum;

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

string Meta_Field::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   else if( id_or_name == c_field_id_Class || id_or_name == c_field_name_Class )
      display_name = get_module_string( c_field_display_name_Class );
   else if( id_or_name == c_field_id_Default || id_or_name == c_field_name_Default )
      display_name = get_module_string( c_field_display_name_Default );
   else if( id_or_name == c_field_id_Enum || id_or_name == c_field_name_Enum )
      display_name = get_module_string( c_field_display_name_Enum );
   else if( id_or_name == c_field_id_Extra || id_or_name == c_field_name_Extra )
      display_name = get_module_string( c_field_display_name_Extra );
   else if( id_or_name == c_field_id_Id || id_or_name == c_field_name_Id )
      display_name = get_module_string( c_field_display_name_Id );
   else if( id_or_name == c_field_id_Internal || id_or_name == c_field_name_Internal )
      display_name = get_module_string( c_field_display_name_Internal );
   else if( id_or_name == c_field_id_Is_Foreign_Key || id_or_name == c_field_name_Is_Foreign_Key )
      display_name = get_module_string( c_field_display_name_Is_Foreign_Key );
   else if( id_or_name == c_field_id_Is_Text_Type || id_or_name == c_field_name_Is_Text_Type )
      display_name = get_module_string( c_field_display_name_Is_Text_Type );
   else if( id_or_name == c_field_id_Mandatory || id_or_name == c_field_name_Mandatory )
      display_name = get_module_string( c_field_display_name_Mandatory );
   else if( id_or_name == c_field_id_Name || id_or_name == c_field_name_Name )
      display_name = get_module_string( c_field_display_name_Name );
   else if( id_or_name == c_field_id_Numeric_Decimals || id_or_name == c_field_name_Numeric_Decimals )
      display_name = get_module_string( c_field_display_name_Numeric_Decimals );
   else if( id_or_name == c_field_id_Parent_Class || id_or_name == c_field_name_Parent_Class )
      display_name = get_module_string( c_field_display_name_Parent_Class );
   else if( id_or_name == c_field_id_Parent_Class_Name || id_or_name == c_field_name_Parent_Class_Name )
      display_name = get_module_string( c_field_display_name_Parent_Class_Name );
   else if( id_or_name == c_field_id_Primitive || id_or_name == c_field_name_Primitive )
      display_name = get_module_string( c_field_display_name_Primitive );
   else if( id_or_name == c_field_id_Source_Field || id_or_name == c_field_name_Source_Field )
      display_name = get_module_string( c_field_display_name_Source_Field );
   else if( id_or_name == c_field_id_Transient || id_or_name == c_field_name_Transient )
      display_name = get_module_string( c_field_display_name_Transient );
   else if( id_or_name == c_field_id_Type || id_or_name == c_field_name_Type )
      display_name = get_module_string( c_field_display_name_Type );
   else if( id_or_name == c_field_id_UOM || id_or_name == c_field_name_UOM )
      display_name = get_module_string( c_field_display_name_UOM );
   else if( id_or_name == c_field_id_Use_In_Text_Search || id_or_name == c_field_name_Use_In_Text_Search )
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

void Meta_Field::setup_graph_parent( Meta_Specification_Content_Page& o, const string& foreign_key_field )
{
   static_cast< Meta_Specification_Content_Page& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Field::setup_graph_parent( Meta_Specification_Copy_Child_Links& o, const string& foreign_key_field )
{
   static_cast< Meta_Specification_Copy_Child_Links& >( o ).set_graph_parent( this, foreign_key_field );
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
   size_t rc = 68;

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

   if( child_num >= 68 )
   {
      external_aliases_lookup_const_iterator ealci = g_external_aliases_lookup.lower_bound( child_num );
      if( ealci == g_external_aliases_lookup.end( ) || ealci->first > child_num )
         --ealci;

      p_class_base = ealci->second->get_next_foreign_key_child( child_num - ealci->first, next_child_field, op, true );
   }
   else
   {
      switch( child_num )
      {
         case 0:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302629";
            p_class_base = &child_Specification_Content_Page_Actions( );
         }
         break;

         case 1:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302650";
            p_class_base = &child_Specification_Copy_Child_Links_Child_File( );
         }
         break;

         case 2:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302198";
            p_class_base = &child_List_Field_Child_Rel_Source_Child( );
         }
         break;

         case 3:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302195";
            p_class_base = &child_List_Field_Child_Rel_Source( );
         }
         break;

         case 4:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302196";
            p_class_base = &child_List_Field_Child_Rel_Source_Parent( );
         }
         break;

         case 5:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302607";
            p_class_base = &child_Specification_Content_Page_Clone_Permitted( );
         }
         break;

         case 6:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302610";
            p_class_base = &child_Specification_Content_Page_Code( );
         }
         break;

         case 7:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302605";
            p_class_base = &child_Specification_Content_Page_Content_Type( );
         }
         break;

         case 8:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "300811";
            p_class_base = &child_Index_Field_1( );
         }
         break;

         case 9:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "300812";
            p_class_base = &child_Index_Field_2( );
         }
         break;

         case 10:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "300813";
            p_class_base = &child_Index_Field_3( );
         }
         break;

         case 11:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "300814";
            p_class_base = &child_Index_Field_4( );
         }
         break;

         case 12:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "300815";
            p_class_base = &child_Index_Field_5( );
         }
         break;

         case 13:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302612";
            p_class_base = &child_Specification_Content_Page_File_Name( );
         }
         break;

         case 14:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302608";
            p_class_base = &child_Specification_Content_Page_File_Name_Gen( );
         }
         break;

         case 15:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302640";
            p_class_base = &child_Specification_Content_Page_Frag_Code( );
         }
         break;

         case 16:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302642";
            p_class_base = &child_Specification_Content_Page_Frag_Content( );
         }
         break;

         case 17:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302641";
            p_class_base = &child_Specification_Content_Page_Frag_Group( );
         }
         break;

         case 18:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302619";
            p_class_base = &child_Specification_Content_Page_Generate_Children( );
         }
         break;

         case 19:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302647";
            p_class_base = &child_Specification_Content_Page_Group_Base_Path( );
         }
         break;

         case 20:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302646";
            p_class_base = &child_Specification_Content_Page_Group_Extra( );
         }
         break;

         case 21:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302609";
            p_class_base = &child_Specification_Content_Page_Group( );
         }
         break;

         case 22:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302645";
            p_class_base = &child_Specification_Content_Page_Group_Name( );
         }
         break;

         case 23:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302618";
            p_class_base = &child_Specification_Content_Page_Hyperlink_Title( );
         }
         break;

         case 24:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302617";
            p_class_base = &child_Specification_Content_Page_Link_Content( );
         }
         break;

         case 25:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302622";
            p_class_base = &child_Specification_Content_Page_Link_Group( );
         }
         break;

         case 26:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302621";
            p_class_base = &child_Specification_Content_Page_Link_Name( );
         }
         break;

         case 27:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302616";
            p_class_base = &child_Specification_Content_Page_Meta_Content( );
         }
         break;

         case 28:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302310";
            p_class_base = &child_Specification_Field_Action_New_Record_FK( );
         }
         break;

         case 29:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302643";
            p_class_base = &child_Specification_Content_Page_Next_Child_Num( );
         }
         break;

         case 30:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302630";
            p_class_base = &child_Specification_Content_Page_Order( );
         }
         break;

         case 31:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301436";
            p_class_base = &child_Specification_Other_Field_2( );
         }
         break;

         case 32:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301435";
            p_class_base = &child_Specification_Other( );
         }
         break;

         case 33:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302623";
            p_class_base = &child_Specification_Content_Page_Owned_Links( );
         }
         break;

         case 34:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302631";
            p_class_base = &child_Specification_Content_Page_Page( );
         }
         break;

         case 35:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302635";
            p_class_base = &child_Specification_Content_Page_Page_File( );
         }
         break;

         case 36:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302636";
            p_class_base = &child_Specification_Content_Page_Page_File_Name( );
         }
         break;

         case 37:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302606";
            p_class_base = &child_Specification_Content_Page_Page_File_Name_Gen( );
         }
         break;

         case 38:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301996";
            p_class_base = &child_List_Parent( );
         }
         break;

         case 39:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302614";
            p_class_base = &child_Specification_Content_Page_Parent_Page( );
         }
         break;

         case 40:
         if( op == e_cascade_op_unlink )
         {
            next_child_field = "300630";
            p_class_base = &child_Class_Quick_Link( );
         }
         break;

         case 41:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302185";
            p_class_base = &child_List_Field_Restriction( );
         }
         break;

         case 42:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301496";
            p_class_base = &child_Specification_Source_Child_2( );
         }
         break;

         case 43:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302653";
            p_class_base = &child_Specification_Copy_Child_Links_Source_Child_File( );
         }
         break;

         case 44:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302160";
            p_class_base = &child_List_Field_Source_Child( );
         }
         break;

         case 45:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301495";
            p_class_base = &child_Specification_Source_Child( );
         }
         break;

         case 46:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301960";
            p_class_base = &child_View_Field_Source_Child( );
         }
         break;

         case 47:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301965";
            p_class_base = &child_View_Field_Source_Edit_Child( );
         }
         break;

         case 48:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302140";
            p_class_base = &child_List_Field_Source( );
         }
         break;

         case 49:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301481";
            p_class_base = &child_Specification_Source( );
         }
         break;

         case 50:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301940";
            p_class_base = &child_View_Field_Source( );
         }
         break;

         case 51:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302165";
            p_class_base = &child_List_Field_Source_Grandchild( );
         }
         break;

         case 52:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301476";
            p_class_base = &child_Specification_Source_Grandchild( );
         }
         break;

         case 53:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302150";
            p_class_base = &child_List_Field_Source_Parent( );
         }
         break;

         case 54:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301480";
            p_class_base = &child_Specification_Source_Parent( );
         }
         break;

         case 55:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301950";
            p_class_base = &child_View_Field_Source_Parent( );
         }
         break;

         case 56:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "300740";
            p_class_base = &child_Field_Source( );
         }
         break;

         case 57:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302613";
            p_class_base = &child_Specification_Content_Page_Sub_Title( );
         }
         break;

         case 58:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301488";
            p_class_base = &child_Specification_Test_Child( );
         }
         break;

         case 59:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301485";
            p_class_base = &child_Specification_Test( );
         }
         break;

         case 60:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301486";
            p_class_base = &child_Specification_Test_Parent( );
         }
         break;

         case 61:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302615";
            p_class_base = &child_Specification_Content_Page_Text_Content( );
         }
         break;

         case 62:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302611";
            p_class_base = &child_Specification_Content_Page_Title( );
         }
         break;

         case 63:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302638";
            p_class_base = &child_Specification_Content_Page_Variable_Name( );
         }
         break;

         case 64:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302639";
            p_class_base = &child_Specification_Content_Page_Variable_Value( );
         }
         break;

         case 65:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302520";
            p_class_base = &child_Initial_Record_Value( );
         }
         break;

         case 66:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301020";
            p_class_base = &child_Modifier_Affect( );
         }
         break;

         case 67:
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

void Meta_Field::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

const char* Meta_Field::class_id( ) const
{
   return static_class_id( );
}

const char* Meta_Field::class_name( ) const
{
   return static_class_name( );
}

const char* Meta_Field::plural_name( ) const
{
   return static_plural_name( );
}

const char* Meta_Field::module_id( ) const
{
   return static_module_id( );
}

const char* Meta_Field::module_name( ) const
{
   return static_module_name( );
}

string Meta_Field::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );
   key += "field";

   return get_module_string( key );
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
   else if( sub_context == "_302629" || sub_context == "child_Specification_Content_Page_Actions" )
      p_class_base = &child_Specification_Content_Page_Actions( );
   else if( sub_context == "_302650" || sub_context == "child_Specification_Copy_Child_Links_Child_File" )
      p_class_base = &child_Specification_Copy_Child_Links_Child_File( );
   else if( sub_context == "_302198" || sub_context == "child_List_Field_Child_Rel_Source_Child" )
      p_class_base = &child_List_Field_Child_Rel_Source_Child( );
   else if( sub_context == "_302195" || sub_context == "child_List_Field_Child_Rel_Source" )
      p_class_base = &child_List_Field_Child_Rel_Source( );
   else if( sub_context == "_302196" || sub_context == "child_List_Field_Child_Rel_Source_Parent" )
      p_class_base = &child_List_Field_Child_Rel_Source_Parent( );
   else if( sub_context == "_302607" || sub_context == "child_Specification_Content_Page_Clone_Permitted" )
      p_class_base = &child_Specification_Content_Page_Clone_Permitted( );
   else if( sub_context == "_302610" || sub_context == "child_Specification_Content_Page_Code" )
      p_class_base = &child_Specification_Content_Page_Code( );
   else if( sub_context == "_302605" || sub_context == "child_Specification_Content_Page_Content_Type" )
      p_class_base = &child_Specification_Content_Page_Content_Type( );
   else if( sub_context == "_300811" || sub_context == "child_Index_Field_1" )
      p_class_base = &child_Index_Field_1( );
   else if( sub_context == "_300812" || sub_context == "child_Index_Field_2" )
      p_class_base = &child_Index_Field_2( );
   else if( sub_context == "_300813" || sub_context == "child_Index_Field_3" )
      p_class_base = &child_Index_Field_3( );
   else if( sub_context == "_300814" || sub_context == "child_Index_Field_4" )
      p_class_base = &child_Index_Field_4( );
   else if( sub_context == "_300815" || sub_context == "child_Index_Field_5" )
      p_class_base = &child_Index_Field_5( );
   else if( sub_context == "_302612" || sub_context == "child_Specification_Content_Page_File_Name" )
      p_class_base = &child_Specification_Content_Page_File_Name( );
   else if( sub_context == "_302608" || sub_context == "child_Specification_Content_Page_File_Name_Gen" )
      p_class_base = &child_Specification_Content_Page_File_Name_Gen( );
   else if( sub_context == "_302640" || sub_context == "child_Specification_Content_Page_Frag_Code" )
      p_class_base = &child_Specification_Content_Page_Frag_Code( );
   else if( sub_context == "_302642" || sub_context == "child_Specification_Content_Page_Frag_Content" )
      p_class_base = &child_Specification_Content_Page_Frag_Content( );
   else if( sub_context == "_302641" || sub_context == "child_Specification_Content_Page_Frag_Group" )
      p_class_base = &child_Specification_Content_Page_Frag_Group( );
   else if( sub_context == "_302619" || sub_context == "child_Specification_Content_Page_Generate_Children" )
      p_class_base = &child_Specification_Content_Page_Generate_Children( );
   else if( sub_context == "_302647" || sub_context == "child_Specification_Content_Page_Group_Base_Path" )
      p_class_base = &child_Specification_Content_Page_Group_Base_Path( );
   else if( sub_context == "_302646" || sub_context == "child_Specification_Content_Page_Group_Extra" )
      p_class_base = &child_Specification_Content_Page_Group_Extra( );
   else if( sub_context == "_302609" || sub_context == "child_Specification_Content_Page_Group" )
      p_class_base = &child_Specification_Content_Page_Group( );
   else if( sub_context == "_302645" || sub_context == "child_Specification_Content_Page_Group_Name" )
      p_class_base = &child_Specification_Content_Page_Group_Name( );
   else if( sub_context == "_302618" || sub_context == "child_Specification_Content_Page_Hyperlink_Title" )
      p_class_base = &child_Specification_Content_Page_Hyperlink_Title( );
   else if( sub_context == "_302617" || sub_context == "child_Specification_Content_Page_Link_Content" )
      p_class_base = &child_Specification_Content_Page_Link_Content( );
   else if( sub_context == "_302622" || sub_context == "child_Specification_Content_Page_Link_Group" )
      p_class_base = &child_Specification_Content_Page_Link_Group( );
   else if( sub_context == "_302621" || sub_context == "child_Specification_Content_Page_Link_Name" )
      p_class_base = &child_Specification_Content_Page_Link_Name( );
   else if( sub_context == "_302616" || sub_context == "child_Specification_Content_Page_Meta_Content" )
      p_class_base = &child_Specification_Content_Page_Meta_Content( );
   else if( sub_context == "_302310" || sub_context == "child_Specification_Field_Action_New_Record_FK" )
      p_class_base = &child_Specification_Field_Action_New_Record_FK( );
   else if( sub_context == "_302643" || sub_context == "child_Specification_Content_Page_Next_Child_Num" )
      p_class_base = &child_Specification_Content_Page_Next_Child_Num( );
   else if( sub_context == "_302630" || sub_context == "child_Specification_Content_Page_Order" )
      p_class_base = &child_Specification_Content_Page_Order( );
   else if( sub_context == "_301436" || sub_context == "child_Specification_Other_Field_2" )
      p_class_base = &child_Specification_Other_Field_2( );
   else if( sub_context == "_301435" || sub_context == "child_Specification_Other" )
      p_class_base = &child_Specification_Other( );
   else if( sub_context == "_302623" || sub_context == "child_Specification_Content_Page_Owned_Links" )
      p_class_base = &child_Specification_Content_Page_Owned_Links( );
   else if( sub_context == "_302631" || sub_context == "child_Specification_Content_Page_Page" )
      p_class_base = &child_Specification_Content_Page_Page( );
   else if( sub_context == "_302635" || sub_context == "child_Specification_Content_Page_Page_File" )
      p_class_base = &child_Specification_Content_Page_Page_File( );
   else if( sub_context == "_302636" || sub_context == "child_Specification_Content_Page_Page_File_Name" )
      p_class_base = &child_Specification_Content_Page_Page_File_Name( );
   else if( sub_context == "_302606" || sub_context == "child_Specification_Content_Page_Page_File_Name_Gen" )
      p_class_base = &child_Specification_Content_Page_Page_File_Name_Gen( );
   else if( sub_context == "_301996" || sub_context == "child_List_Parent" )
      p_class_base = &child_List_Parent( );
   else if( sub_context == "_302614" || sub_context == "child_Specification_Content_Page_Parent_Page" )
      p_class_base = &child_Specification_Content_Page_Parent_Page( );
   else if( sub_context == "_300630" || sub_context == "child_Class_Quick_Link" )
      p_class_base = &child_Class_Quick_Link( );
   else if( sub_context == "_302185" || sub_context == "child_List_Field_Restriction" )
      p_class_base = &child_List_Field_Restriction( );
   else if( sub_context == "_301496" || sub_context == "child_Specification_Source_Child_2" )
      p_class_base = &child_Specification_Source_Child_2( );
   else if( sub_context == "_302653" || sub_context == "child_Specification_Copy_Child_Links_Source_Child_File" )
      p_class_base = &child_Specification_Copy_Child_Links_Source_Child_File( );
   else if( sub_context == "_302160" || sub_context == "child_List_Field_Source_Child" )
      p_class_base = &child_List_Field_Source_Child( );
   else if( sub_context == "_301495" || sub_context == "child_Specification_Source_Child" )
      p_class_base = &child_Specification_Source_Child( );
   else if( sub_context == "_301960" || sub_context == "child_View_Field_Source_Child" )
      p_class_base = &child_View_Field_Source_Child( );
   else if( sub_context == "_301965" || sub_context == "child_View_Field_Source_Edit_Child" )
      p_class_base = &child_View_Field_Source_Edit_Child( );
   else if( sub_context == "_302140" || sub_context == "child_List_Field_Source" )
      p_class_base = &child_List_Field_Source( );
   else if( sub_context == "_301481" || sub_context == "child_Specification_Source" )
      p_class_base = &child_Specification_Source( );
   else if( sub_context == "_301940" || sub_context == "child_View_Field_Source" )
      p_class_base = &child_View_Field_Source( );
   else if( sub_context == "_302165" || sub_context == "child_List_Field_Source_Grandchild" )
      p_class_base = &child_List_Field_Source_Grandchild( );
   else if( sub_context == "_301476" || sub_context == "child_Specification_Source_Grandchild" )
      p_class_base = &child_Specification_Source_Grandchild( );
   else if( sub_context == "_302150" || sub_context == "child_List_Field_Source_Parent" )
      p_class_base = &child_List_Field_Source_Parent( );
   else if( sub_context == "_301480" || sub_context == "child_Specification_Source_Parent" )
      p_class_base = &child_Specification_Source_Parent( );
   else if( sub_context == "_301950" || sub_context == "child_View_Field_Source_Parent" )
      p_class_base = &child_View_Field_Source_Parent( );
   else if( sub_context == "_300740" || sub_context == "child_Field_Source" )
      p_class_base = &child_Field_Source( );
   else if( sub_context == "_302613" || sub_context == "child_Specification_Content_Page_Sub_Title" )
      p_class_base = &child_Specification_Content_Page_Sub_Title( );
   else if( sub_context == "_301488" || sub_context == "child_Specification_Test_Child" )
      p_class_base = &child_Specification_Test_Child( );
   else if( sub_context == "_301485" || sub_context == "child_Specification_Test" )
      p_class_base = &child_Specification_Test( );
   else if( sub_context == "_301486" || sub_context == "child_Specification_Test_Parent" )
      p_class_base = &child_Specification_Test_Parent( );
   else if( sub_context == "_302615" || sub_context == "child_Specification_Content_Page_Text_Content" )
      p_class_base = &child_Specification_Content_Page_Text_Content( );
   else if( sub_context == "_302611" || sub_context == "child_Specification_Content_Page_Title" )
      p_class_base = &child_Specification_Content_Page_Title( );
   else if( sub_context == "_302638" || sub_context == "child_Specification_Content_Page_Variable_Name" )
      p_class_base = &child_Specification_Content_Page_Variable_Name( );
   else if( sub_context == "_302639" || sub_context == "child_Specification_Content_Page_Variable_Value" )
      p_class_base = &child_Specification_Content_Page_Variable_Value( );
   else if( sub_context == "_302520" || sub_context == "child_Initial_Record_Value" )
      p_class_base = &child_Initial_Record_Value( );
   else if( sub_context == "_301020" || sub_context == "child_Modifier_Affect" )
      p_class_base = &child_Modifier_Affect( );
   else if( sub_context == "_301430" || sub_context == "child_Specification" )
      p_class_base = &child_Specification( );
   else if( sub_context == c_field_id_Class || sub_context == c_field_name_Class )
      p_class_base = &Class( );
   else if( sub_context == c_field_id_Enum || sub_context == c_field_name_Enum )
      p_class_base = &Enum( );
   else if( sub_context == c_field_id_Parent_Class || sub_context == c_field_name_Parent_Class )
      p_class_base = &Parent_Class( );
   else if( sub_context == c_field_id_Source_Field || sub_context == c_field_name_Source_Field )
      p_class_base = &Source_Field( );
   else if( sub_context == c_field_id_Type || sub_context == c_field_name_Type )
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

   names.push_back( "C_Class" );
   names.push_back( "C_Default" );
   names.push_back( "C_Enum" );
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
   names.push_back( "C_Use_In_Text_Search" );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Field::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Class( ) ) ) );
   values.push_back( sql_quote( to_string( Default( ) ) ) );
   values.push_back( sql_quote( to_string( Enum( ) ) ) );
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
   values.push_back( to_string( Use_In_Text_Search( ) ) );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Field::get_required_field_names(
 set< string >& names, bool required_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, required_transients, dependents );

   // [(start field_from_other_field)]
   if( needs_field_value( "Primitive", dependents ) )
   {
      dependents.insert( "Type" );

      if( ( required_transients && is_field_transient( e_field_id_Type ) )
       || ( !required_transients && !is_field_transient( e_field_id_Type ) ) )
         names.insert( "Type" );
   }
   // [(finish field_from_other_field)]

   // [(start transient_field_alias)]
   if( needs_field_value( "Parent_Class_Name", dependents ) )
   {
      dependents.insert( "Parent_Class" );

      if( ( required_transients && is_field_transient( e_field_id_Parent_Class ) )
       || ( !required_transients && !is_field_transient( e_field_id_Parent_Class ) ) )
         names.insert( "Parent_Class" );
   }
   // [(finish transient_field_alias)]

   // [(start field_from_other_field)]
   if( needs_field_value( "Is_Foreign_Key", dependents ) )
   {
      dependents.insert( "Parent_Class" );

      if( ( required_transients && is_field_transient( e_field_id_Parent_Class ) )
       || ( !required_transients && !is_field_transient( e_field_id_Parent_Class ) ) )
         names.insert( "Parent_Class" );
   }
   // [(finish field_from_other_field)]

   // [(start transient_field_alias)]
   if( needs_field_value( "Numeric_Decimals", dependents ) )
   {
      dependents.insert( "Type" );

      if( ( required_transients && is_field_transient( e_field_id_Type ) )
       || ( !required_transients && !is_field_transient( e_field_id_Type ) ) )
         names.insert( "Type" );
   }
   // [(finish transient_field_alias)]

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Field::get_always_required_field_names(
 set< string >& names, bool required_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )required_transients;

   // [(start modifier_field_value)]
   dependents.insert( "Type" ); // (for Is_Any_Non_Text_Type modifier)

   if( ( required_transients && is_field_transient( e_field_id_Type ) )
    || ( !required_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   dependents.insert( "Type" ); // (for Is_Any_Non_Text_Type modifier)

   if( ( required_transients && is_field_transient( e_field_id_Type ) )
    || ( !required_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   dependents.insert( "Type" ); // (for Is_Any_Non_Text_Type modifier)

   if( ( required_transients && is_field_transient( e_field_id_Type ) )
    || ( !required_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)]

   // [(start protect_equal)]
   dependents.insert( "Internal" );

   if( ( required_transients && is_field_transient( e_field_id_Internal ) )
    || ( !required_transients && !is_field_transient( e_field_id_Internal ) ) )
      names.insert( "Internal" );
   // [(finish protect_equal)]

   // [(start modifier_field_value)]
   dependents.insert( "Internal" ); // (for Is_Internal modifier)

   if( ( required_transients && is_field_transient( e_field_id_Internal ) )
    || ( !required_transients && !is_field_transient( e_field_id_Internal ) ) )
      names.insert( "Internal" );
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   dependents.insert( "Transient" ); // (for Is_Transient modifier)

   if( ( required_transients && is_field_transient( e_field_id_Transient ) )
    || ( !required_transients && !is_field_transient( e_field_id_Transient ) ) )
      names.insert( "Transient" );
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   dependents.insert( "Type" ); // (for Is_Type_bool modifier)

   if( ( required_transients && is_field_transient( e_field_id_Type ) )
    || ( !required_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)]

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Field::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Field::do_generate_sql( generate_sql_type type, vector< string >& sql_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts );
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
   all_field_info.push_back( field_info( "300700", "Class", "Meta_Class", true ) );
   all_field_info.push_back( field_info( "107102", "Default", "string", false ) );
   all_field_info.push_back( field_info( "300720", "Enum", "Meta_Enum", false ) );
   all_field_info.push_back( field_info( "107103", "Extra", "int", false ) );
   all_field_info.push_back( field_info( "107110", "Id", "string", false ) );
   all_field_info.push_back( field_info( "107106", "Internal", "bool", false ) );
   all_field_info.push_back( field_info( "107108", "Is_Foreign_Key", "bool", false ) );
   all_field_info.push_back( field_info( "107109", "Is_Text_Type", "bool", false ) );
   all_field_info.push_back( field_info( "107105", "Mandatory", "bool", false ) );
   all_field_info.push_back( field_info( "107101", "Name", "string", false ) );
   all_field_info.push_back( field_info( "107114", "Numeric_Decimals", "numeric", false ) );
   all_field_info.push_back( field_info( "300730", "Parent_Class", "Meta_Class", false ) );
   all_field_info.push_back( field_info( "107113", "Parent_Class_Name", "string", false ) );
   all_field_info.push_back( field_info( "107107", "Primitive", "int", false ) );
   all_field_info.push_back( field_info( "300740", "Source_Field", "Meta_Field", false ) );
   all_field_info.push_back( field_info( "107112", "Transient", "bool", false ) );
   all_field_info.push_back( field_info( "300710", "Type", "Meta_Type", true ) );
   all_field_info.push_back( field_info( "107104", "UOM", "int", false ) );
   all_field_info.push_back( field_info( "107111", "Use_In_Text_Search", "bool", false ) );
}

void Meta_Field::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Class, make_pair( "Meta.107100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Enum, make_pair( "Meta.107100", "Meta_Enum" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Parent_Class, make_pair( "Meta.107100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Field, make_pair( "Meta.107100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Type, make_pair( "Meta.107100", "Meta_Type" ) ) );
}

int Meta_Field::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;

   return c_num_fields;
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
      p_id = "300700";
      break;

      case 2:
      p_id = "107102";
      break;

      case 3:
      p_id = "300720";
      break;

      case 4:
      p_id = "107103";
      break;

      case 5:
      p_id = "107110";
      break;

      case 6:
      p_id = "107106";
      break;

      case 7:
      p_id = "107108";
      break;

      case 8:
      p_id = "107109";
      break;

      case 9:
      p_id = "107105";
      break;

      case 10:
      p_id = "107101";
      break;

      case 11:
      p_id = "107114";
      break;

      case 12:
      p_id = "300730";
      break;

      case 13:
      p_id = "107113";
      break;

      case 14:
      p_id = "107107";
      break;

      case 15:
      p_id = "300740";
      break;

      case 16:
      p_id = "107112";
      break;

      case 17:
      p_id = "300710";
      break;

      case 18:
      p_id = "107104";
      break;

      case 19:
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
      p_id = "Class";
      break;

      case 2:
      p_id = "Default";
      break;

      case 3:
      p_id = "Enum";
      break;

      case 4:
      p_id = "Extra";
      break;

      case 5:
      p_id = "Id";
      break;

      case 6:
      p_id = "Internal";
      break;

      case 7:
      p_id = "Is_Foreign_Key";
      break;

      case 8:
      p_id = "Is_Text_Type";
      break;

      case 9:
      p_id = "Mandatory";
      break;

      case 10:
      p_id = "Name";
      break;

      case 11:
      p_id = "Numeric_Decimals";
      break;

      case 12:
      p_id = "Parent_Class";
      break;

      case 13:
      p_id = "Parent_Class_Name";
      break;

      case 14:
      p_id = "Primitive";
      break;

      case 15:
      p_id = "Source_Field";
      break;

      case 16:
      p_id = "Transient";
      break;

      case 17:
      p_id = "Type";
      break;

      case 18:
      p_id = "UOM";
      break;

      case 19:
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
   else if( field == c_field_id_Class || field == c_field_name_Class )
      rc += 1;
   else if( field == c_field_id_Default || field == c_field_name_Default )
      rc += 2;
   else if( field == c_field_id_Enum || field == c_field_name_Enum )
      rc += 3;
   else if( field == c_field_id_Extra || field == c_field_name_Extra )
      rc += 4;
   else if( field == c_field_id_Id || field == c_field_name_Id )
      rc += 5;
   else if( field == c_field_id_Internal || field == c_field_name_Internal )
      rc += 6;
   else if( field == c_field_id_Is_Foreign_Key || field == c_field_name_Is_Foreign_Key )
      rc += 7;
   else if( field == c_field_id_Is_Text_Type || field == c_field_name_Is_Text_Type )
      rc += 8;
   else if( field == c_field_id_Mandatory || field == c_field_name_Mandatory )
      rc += 9;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 10;
   else if( field == c_field_id_Numeric_Decimals || field == c_field_name_Numeric_Decimals )
      rc += 11;
   else if( field == c_field_id_Parent_Class || field == c_field_name_Parent_Class )
      rc += 12;
   else if( field == c_field_id_Parent_Class_Name || field == c_field_name_Parent_Class_Name )
      rc += 13;
   else if( field == c_field_id_Primitive || field == c_field_name_Primitive )
      rc += 14;
   else if( field == c_field_id_Source_Field || field == c_field_name_Source_Field )
      rc += 15;
   else if( field == c_field_id_Transient || field == c_field_name_Transient )
      rc += 16;
   else if( field == c_field_id_Type || field == c_field_name_Type )
      rc += 17;
   else if( field == c_field_id_UOM || field == c_field_name_UOM )
      rc += 18;
   else if( field == c_field_id_Use_In_Text_Search || field == c_field_name_Use_In_Text_Search )
      rc += 19;

   return rc - 1;
}

procedure_info_container& Meta_Field::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "107410", "Get_Text_Type" ) );
   }

   return procedures;
}

string Meta_Field::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(64),"
    "C_Ver_ INTEGER NOT NULL,"
    "C_Rev_ INTEGER NOT NULL,"
    "C_Typ_ VARCHAR(24) NOT NULL,"
    "C_Class VARCHAR(64) NOT NULL,"
    "C_Default VARCHAR(200) NOT NULL,"
    "C_Enum VARCHAR(64) NOT NULL,"
    "C_Extra INTEGER NOT NULL,"
    "C_Id VARCHAR(200) NOT NULL,"
    "C_Internal INTEGER NOT NULL,"
    "C_Is_Foreign_Key INTEGER NOT NULL,"
    "C_Is_Text_Type INTEGER NOT NULL,"
    "C_Mandatory INTEGER NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Parent_Class VARCHAR(64) NOT NULL,"
    "C_Primitive INTEGER NOT NULL,"
    "C_Source_Field VARCHAR(64) NOT NULL,"
    "C_Transient INTEGER NOT NULL,"
    "C_Type VARCHAR(64) NOT NULL,"
    "C_UOM INTEGER NOT NULL,"
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
   pairs.push_back( make_pair( "enum_field_extra_22", get_enum_string_field_extra( 22 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_10", get_enum_string_field_extra( 10 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_11", get_enum_string_field_extra( 11 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_17", get_enum_string_field_extra( 17 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_24", get_enum_string_field_extra( 24 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_25", get_enum_string_field_extra( 25 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_26", get_enum_string_field_extra( 26 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_27", get_enum_string_field_extra( 27 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_13", get_enum_string_field_extra( 13 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_12", get_enum_string_field_extra( 12 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_14", get_enum_string_field_extra( 14 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_18", get_enum_string_field_extra( 18 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_15", get_enum_string_field_extra( 15 ) ) );
   pairs.push_back( make_pair( "enum_field_extra_16", get_enum_string_field_extra( 16 ) ) );
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
}

void Meta_Field::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Class,C_Id" );
   indexes.push_back( "C_Class,C_Name" );
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
   g_field_extra_enum.insert( 22 );
   g_field_extra_enum.insert( 10 );
   g_field_extra_enum.insert( 11 );
   g_field_extra_enum.insert( 17 );
   g_field_extra_enum.insert( 24 );
   g_field_extra_enum.insert( 25 );
   g_field_extra_enum.insert( 26 );
   g_field_extra_enum.insert( 27 );
   g_field_extra_enum.insert( 13 );
   g_field_extra_enum.insert( 12 );
   g_field_extra_enum.insert( 14 );
   g_field_extra_enum.insert( 18 );
   g_field_extra_enum.insert( 15 );
   g_field_extra_enum.insert( 16 );
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

