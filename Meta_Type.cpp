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

#include "Meta_Type.h"

#include "Meta_Field.h"
#include "Meta_Workgroup.h"

#include "ciyam_base.h"
#include "class_domains.h"
#include "module_strings.h"
#include "class_utilities.h"
#include "command_handler.h"
#include "module_interface.h"

// [<start includes>]
//idk
#include "numeric.h"
#include "date_time.h"
#include "mtime_helper.h"
#include "udate_helper.h"
#include "numeric_helper.h"
#include "date_time_helper.h"
// [<finish includes>]

using namespace std;

// [<start namespaces>]
// [<finish namespaces>]

template< > inline string to_string( const Meta_Workgroup& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Workgroup& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline int system( const string& cmd ) { return exec_system( cmd ); }

namespace
{

#include "Meta_Type.cmh"

const int32_t c_version = 1;

const char* const c_okay = "okay";

const char* const c_field_id_Date_Precision = "102112";
const char* const c_field_id_Fraction_Limit = "102115";
const char* const c_field_id_Id = "102111";
const char* const c_field_id_Int_Type = "102116";
const char* const c_field_id_Internal = "102109";
const char* const c_field_id_Max_Size = "102103";
const char* const c_field_id_Max_Value = "102105";
const char* const c_field_id_Min_Value = "102104";
const char* const c_field_id_Name = "102101";
const char* const c_field_id_Numeric_Decimals = "102107";
const char* const c_field_id_Numeric_Digits = "102106";
const char* const c_field_id_Numeric_Type = "102117";
const char* const c_field_id_Primitive = "102102";
const char* const c_field_id_Show_Plus_Sign = "102113";
const char* const c_field_id_String_Domain = "102108";
const char* const c_field_id_Time_Precision = "102110";
const char* const c_field_id_Workgroup = "300200";
const char* const c_field_id_Zero_Padding = "102114";

const char* const c_field_name_Date_Precision = "Date_Precision";
const char* const c_field_name_Fraction_Limit = "Fraction_Limit";
const char* const c_field_name_Id = "Id";
const char* const c_field_name_Int_Type = "Int_Type";
const char* const c_field_name_Internal = "Internal";
const char* const c_field_name_Max_Size = "Max_Size";
const char* const c_field_name_Max_Value = "Max_Value";
const char* const c_field_name_Min_Value = "Min_Value";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Numeric_Decimals = "Numeric_Decimals";
const char* const c_field_name_Numeric_Digits = "Numeric_Digits";
const char* const c_field_name_Numeric_Type = "Numeric_Type";
const char* const c_field_name_Primitive = "Primitive";
const char* const c_field_name_Show_Plus_Sign = "Show_Plus_Sign";
const char* const c_field_name_String_Domain = "String_Domain";
const char* const c_field_name_Time_Precision = "Time_Precision";
const char* const c_field_name_Workgroup = "Workgroup";
const char* const c_field_name_Zero_Padding = "Zero_Padding";

const char* const c_field_display_name_Date_Precision = "field_type_date_precision";
const char* const c_field_display_name_Fraction_Limit = "field_type_fraction_limit";
const char* const c_field_display_name_Id = "field_type_id";
const char* const c_field_display_name_Int_Type = "field_type_int_type";
const char* const c_field_display_name_Internal = "field_type_internal";
const char* const c_field_display_name_Max_Size = "field_type_max_size";
const char* const c_field_display_name_Max_Value = "field_type_max_value";
const char* const c_field_display_name_Min_Value = "field_type_min_value";
const char* const c_field_display_name_Name = "field_type_name";
const char* const c_field_display_name_Numeric_Decimals = "field_type_numeric_decimals";
const char* const c_field_display_name_Numeric_Digits = "field_type_numeric_digits";
const char* const c_field_display_name_Numeric_Type = "field_type_numeric_type";
const char* const c_field_display_name_Primitive = "field_type_primitive";
const char* const c_field_display_name_Show_Plus_Sign = "field_type_show_plus_sign";
const char* const c_field_display_name_String_Domain = "field_type_string_domain";
const char* const c_field_display_name_Time_Precision = "field_type_time_precision";
const char* const c_field_display_name_Workgroup = "field_type_workgroup";
const char* const c_field_display_name_Zero_Padding = "field_type_zero_padding";

const int c_num_fields = 18;

const char* const c_all_sorted_field_ids[ ] =
{
   "102101",
   "102102",
   "102103",
   "102104",
   "102105",
   "102106",
   "102107",
   "102108",
   "102109",
   "102110",
   "102111",
   "102112",
   "102113",
   "102114",
   "102115",
   "102116",
   "102117",
   "300200"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Date_Precision",
   "Fraction_Limit",
   "Id",
   "Int_Type",
   "Internal",
   "Max_Size",
   "Max_Value",
   "Min_Value",
   "Name",
   "Numeric_Decimals",
   "Numeric_Digits",
   "Numeric_Type",
   "Primitive",
   "Show_Plus_Sign",
   "String_Domain",
   "Time_Precision",
   "Workgroup",
   "Zero_Padding"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool has_field( const string& field )
{
   return binary_search( c_all_sorted_field_ids, c_all_sorted_field_ids + c_num_fields, field.c_str( ), compare )
    || binary_search( c_all_sorted_field_names, c_all_sorted_field_names + c_num_fields, field.c_str( ), compare );
}

const int c_num_transient_fields = 0;

bool is_transient_field( const string& ) { static bool false_value( false ); return false_value; }

const uint64_t c_modifier_Is_Bool = UINT64_C( 0x100 );
const uint64_t c_modifier_Is_Date = UINT64_C( 0x200 );
const uint64_t c_modifier_Is_Datetime = UINT64_C( 0x400 );
const uint64_t c_modifier_Is_Int = UINT64_C( 0x800 );
const uint64_t c_modifier_Is_Internal = UINT64_C( 0x1000 );
const uint64_t c_modifier_Is_Not_Normal_Int = UINT64_C( 0x2000 );
const uint64_t c_modifier_Is_Not_Normal_Numeric = UINT64_C( 0x4000 );
const uint64_t c_modifier_Is_Numeric = UINT64_C( 0x8000 );
const uint64_t c_modifier_Is_String = UINT64_C( 0x10000 );
const uint64_t c_modifier_Is_Time = UINT64_C( 0x20000 );

aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Id_domain;
domain_int_range< 0, 999999 > g_Max_Size_domain;
domain_string_max_size< 30 > g_Max_Value_domain;
domain_string_max_size< 30 > g_Min_Value_domain;
domain_string_max_size< 100 > g_Name_domain;
domain_int_range< 0, 16 > g_Numeric_Decimals_domain;
domain_int_range< 0, 16 > g_Numeric_Digits_domain;

set< string > g_derivations;

typedef map< string, Meta_Type* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Type* > external_aliases_lookup_container;
typedef external_aliases_lookup_container::const_iterator external_aliases_lookup_const_iterator;

external_aliases_container g_external_aliases;
external_aliases_lookup_container g_external_aliases_lookup;

int gv_default_Date_Precision = int( 0 );
int gv_default_Fraction_Limit = int( 0 );
string gv_default_Id = string( );
int gv_default_Int_Type = int( 0 );
bool gv_default_Internal = bool( 0 );
int gv_default_Max_Size = int( 0 );
string gv_default_Max_Value = string( );
string gv_default_Min_Value = string( );
string gv_default_Name = string( );
int gv_default_Numeric_Decimals = int( 0 );
int gv_default_Numeric_Digits = int( 0 );
int gv_default_Numeric_Type = int( 0 );
int gv_default_Primitive = int( 0 );
bool gv_default_Show_Plus_Sign = bool( 0 );
int gv_default_String_Domain = int( 0 );
int gv_default_Time_Precision = int( 0 );
int gv_default_Zero_Padding = int( 0 );

set< int > g_date_precision_enum;
set< int > g_fraction_limit_enum;
set< int > g_int_type_enum;
set< int > g_numeric_type_enum;
set< int > g_primitive_enum;
set< int > g_string_domain_enum;
set< int > g_time_precision_enum;
set< int > g_zero_padding_enum;

const int c_enum_date_precision_default( 0 );
const int c_enum_date_precision_months( 1 );
const int c_enum_date_precision_decades( 2 );

string get_enum_string_date_precision( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for date_precision" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_date_precision_default";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_date_precision_months";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_date_precision_decades";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for date_precision" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_fraction_limit_none( 0 );
const int c_enum_fraction_limit_halves( 2 );
const int c_enum_fraction_limit_quarters( 4 );

string get_enum_string_fraction_limit( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for fraction_limit" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_fraction_limit_none";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_fraction_limit_halves";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_fraction_limit_quarters";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for fraction_limit" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_int_type_normal( 0 );
const int c_enum_int_type_bytes( 3 );
const int c_enum_int_type_duration_dhm( 1 );
const int c_enum_int_type_duration_hms( 2 );

string get_enum_string_int_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for int_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_int_type_normal";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_int_type_bytes";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_int_type_duration_dhm";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_int_type_duration_hms";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for int_type" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_numeric_type_normal( 0 );
const int c_enum_numeric_type_bytes( 1 );

string get_enum_string_numeric_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for numeric_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_numeric_type_normal";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_numeric_type_bytes";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for numeric_type" );

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

const int c_enum_string_domain_none( 0 );
const int c_enum_string_domain_label( 2 );
const int c_enum_string_domain_filename( 3 );
const int c_enum_string_domain_identifier( 1 );

string get_enum_string_string_domain( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for string_domain" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_string_domain_none";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_string_domain_label";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_string_domain_filename";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_string_domain_identifier";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for string_domain" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_time_precision_default( 0 );
const int c_enum_time_precision_minutes( 1 );
const int c_enum_time_precision_seconds( 2 );

string get_enum_string_time_precision( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for time_precision" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_time_precision_default";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_time_precision_minutes";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_time_precision_seconds";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for time_precision" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_zero_padding_none( 0 );
const int c_enum_zero_padding_decimals( 1 );
const int c_enum_zero_padding_all_digits( 2 );

string get_enum_string_zero_padding( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for zero_padding" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_zero_padding_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_zero_padding_decimals";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_zero_padding_all_digits";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for zero_padding" );

   return get_module_string( lower( string_name ) );
}

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Type > Type_registration( get_class_registry( ), "102100" );

class Meta_Type_command_functor;

class Meta_Type_command_handler : public command_handler
{
   friend class Meta_Type_command_functor;

   public:
   Meta_Type_command_handler( ) : p_Meta_Type( 0 ) { }

   void set_Meta_Type( Meta_Type* p_new_Meta_Type ) { p_Meta_Type = p_new_Meta_Type; }

   void handle_unknown_command( const string& command )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Type* p_Meta_Type;

   protected:
   string retval;
};

class Meta_Type_command_functor : public command_functor
{
   public:
   Meta_Type_command_functor( Meta_Type_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Type_command_handler& cmd_handler;
};

command_functor* Meta_Type_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Type_command_functor( dynamic_cast< Meta_Type_command_handler& >( handler ) );
}

void Meta_Type_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Type_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_parm_Meta_Type_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Type->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Type->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Type_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Type->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Type->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Type_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Type_get_field_name ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );
      else if( field_name == c_field_id_Date_Precision || field_name == c_field_name_Date_Precision )
         string_getter< int >( cmd_handler.p_Meta_Type->Date_Precision( ), cmd_handler.retval );
      else if( field_name == c_field_id_Fraction_Limit || field_name == c_field_name_Fraction_Limit )
         string_getter< int >( cmd_handler.p_Meta_Type->Fraction_Limit( ), cmd_handler.retval );
      else if( field_name == c_field_id_Id || field_name == c_field_name_Id )
         string_getter< string >( cmd_handler.p_Meta_Type->Id( ), cmd_handler.retval );
      else if( field_name == c_field_id_Int_Type || field_name == c_field_name_Int_Type )
         string_getter< int >( cmd_handler.p_Meta_Type->Int_Type( ), cmd_handler.retval );
      else if( field_name == c_field_id_Internal || field_name == c_field_name_Internal )
         string_getter< bool >( cmd_handler.p_Meta_Type->Internal( ), cmd_handler.retval );
      else if( field_name == c_field_id_Max_Size || field_name == c_field_name_Max_Size )
         string_getter< int >( cmd_handler.p_Meta_Type->Max_Size( ), cmd_handler.retval );
      else if( field_name == c_field_id_Max_Value || field_name == c_field_name_Max_Value )
         string_getter< string >( cmd_handler.p_Meta_Type->Max_Value( ), cmd_handler.retval );
      else if( field_name == c_field_id_Min_Value || field_name == c_field_name_Min_Value )
         string_getter< string >( cmd_handler.p_Meta_Type->Min_Value( ), cmd_handler.retval );
      else if( field_name == c_field_id_Name || field_name == c_field_name_Name )
         string_getter< string >( cmd_handler.p_Meta_Type->Name( ), cmd_handler.retval );
      else if( field_name == c_field_id_Numeric_Decimals || field_name == c_field_name_Numeric_Decimals )
         string_getter< int >( cmd_handler.p_Meta_Type->Numeric_Decimals( ), cmd_handler.retval );
      else if( field_name == c_field_id_Numeric_Digits || field_name == c_field_name_Numeric_Digits )
         string_getter< int >( cmd_handler.p_Meta_Type->Numeric_Digits( ), cmd_handler.retval );
      else if( field_name == c_field_id_Numeric_Type || field_name == c_field_name_Numeric_Type )
         string_getter< int >( cmd_handler.p_Meta_Type->Numeric_Type( ), cmd_handler.retval );
      else if( field_name == c_field_id_Primitive || field_name == c_field_name_Primitive )
         string_getter< int >( cmd_handler.p_Meta_Type->Primitive( ), cmd_handler.retval );
      else if( field_name == c_field_id_Show_Plus_Sign || field_name == c_field_name_Show_Plus_Sign )
         string_getter< bool >( cmd_handler.p_Meta_Type->Show_Plus_Sign( ), cmd_handler.retval );
      else if( field_name == c_field_id_String_Domain || field_name == c_field_name_String_Domain )
         string_getter< int >( cmd_handler.p_Meta_Type->String_Domain( ), cmd_handler.retval );
      else if( field_name == c_field_id_Time_Precision || field_name == c_field_name_Time_Precision )
         string_getter< int >( cmd_handler.p_Meta_Type->Time_Precision( ), cmd_handler.retval );
      else if( field_name == c_field_id_Workgroup || field_name == c_field_name_Workgroup )
         string_getter< Meta_Workgroup >( cmd_handler.p_Meta_Type->Workgroup( ), cmd_handler.retval );
      else if( field_name == c_field_id_Zero_Padding || field_name == c_field_name_Zero_Padding )
         string_getter< int >( cmd_handler.p_Meta_Type->Zero_Padding( ), cmd_handler.retval );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Type_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Type_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_parm_Meta_Type_set_field_value ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );
      else if( field_name == c_field_id_Date_Precision || field_name == c_field_name_Date_Precision )
         func_string_setter< Meta_Type, int >(
          *cmd_handler.p_Meta_Type, &Meta_Type::Date_Precision, field_value );
      else if( field_name == c_field_id_Fraction_Limit || field_name == c_field_name_Fraction_Limit )
         func_string_setter< Meta_Type, int >(
          *cmd_handler.p_Meta_Type, &Meta_Type::Fraction_Limit, field_value );
      else if( field_name == c_field_id_Id || field_name == c_field_name_Id )
         func_string_setter< Meta_Type, string >(
          *cmd_handler.p_Meta_Type, &Meta_Type::Id, field_value );
      else if( field_name == c_field_id_Int_Type || field_name == c_field_name_Int_Type )
         func_string_setter< Meta_Type, int >(
          *cmd_handler.p_Meta_Type, &Meta_Type::Int_Type, field_value );
      else if( field_name == c_field_id_Internal || field_name == c_field_name_Internal )
         func_string_setter< Meta_Type, bool >(
          *cmd_handler.p_Meta_Type, &Meta_Type::Internal, field_value );
      else if( field_name == c_field_id_Max_Size || field_name == c_field_name_Max_Size )
         func_string_setter< Meta_Type, int >(
          *cmd_handler.p_Meta_Type, &Meta_Type::Max_Size, field_value );
      else if( field_name == c_field_id_Max_Value || field_name == c_field_name_Max_Value )
         func_string_setter< Meta_Type, string >(
          *cmd_handler.p_Meta_Type, &Meta_Type::Max_Value, field_value );
      else if( field_name == c_field_id_Min_Value || field_name == c_field_name_Min_Value )
         func_string_setter< Meta_Type, string >(
          *cmd_handler.p_Meta_Type, &Meta_Type::Min_Value, field_value );
      else if( field_name == c_field_id_Name || field_name == c_field_name_Name )
         func_string_setter< Meta_Type, string >(
          *cmd_handler.p_Meta_Type, &Meta_Type::Name, field_value );
      else if( field_name == c_field_id_Numeric_Decimals || field_name == c_field_name_Numeric_Decimals )
         func_string_setter< Meta_Type, int >(
          *cmd_handler.p_Meta_Type, &Meta_Type::Numeric_Decimals, field_value );
      else if( field_name == c_field_id_Numeric_Digits || field_name == c_field_name_Numeric_Digits )
         func_string_setter< Meta_Type, int >(
          *cmd_handler.p_Meta_Type, &Meta_Type::Numeric_Digits, field_value );
      else if( field_name == c_field_id_Numeric_Type || field_name == c_field_name_Numeric_Type )
         func_string_setter< Meta_Type, int >(
          *cmd_handler.p_Meta_Type, &Meta_Type::Numeric_Type, field_value );
      else if( field_name == c_field_id_Primitive || field_name == c_field_name_Primitive )
         func_string_setter< Meta_Type, int >(
          *cmd_handler.p_Meta_Type, &Meta_Type::Primitive, field_value );
      else if( field_name == c_field_id_Show_Plus_Sign || field_name == c_field_name_Show_Plus_Sign )
         func_string_setter< Meta_Type, bool >(
          *cmd_handler.p_Meta_Type, &Meta_Type::Show_Plus_Sign, field_value );
      else if( field_name == c_field_id_String_Domain || field_name == c_field_name_String_Domain )
         func_string_setter< Meta_Type, int >(
          *cmd_handler.p_Meta_Type, &Meta_Type::String_Domain, field_value );
      else if( field_name == c_field_id_Time_Precision || field_name == c_field_name_Time_Precision )
         func_string_setter< Meta_Type, int >(
          *cmd_handler.p_Meta_Type, &Meta_Type::Time_Precision, field_value );
      else if( field_name == c_field_id_Workgroup || field_name == c_field_name_Workgroup )
         func_string_setter< Meta_Type, Meta_Workgroup >(
          *cmd_handler.p_Meta_Type, &Meta_Type::Workgroup, field_value );
      else if( field_name == c_field_id_Zero_Padding || field_name == c_field_name_Zero_Padding )
         func_string_setter< Meta_Type, int >(
          *cmd_handler.p_Meta_Type, &Meta_Type::Zero_Padding, field_value );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Type_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Type_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_parm_Meta_Type_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( field_name == c_field_id_Workgroup || field_name == c_field_name_Workgroup )
         cmd_handler.retval = cmd_handler.p_Meta_Type->Workgroup( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
}

struct Meta_Type::impl : public Meta_Type_command_handler
{
   impl( Meta_Type& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Type( &o );

      add_commands( 0, Meta_Type_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Type_command_definitions ) );
   }

   Meta_Type& get_obj( ) const
   {
      return *cp_obj;
   }

   int impl_Date_Precision( ) const { return lazy_fetch( p_obj ), v_Date_Precision; }
   void impl_Date_Precision( int Date_Precision ) { v_Date_Precision = Date_Precision; }

   int impl_Fraction_Limit( ) const { return lazy_fetch( p_obj ), v_Fraction_Limit; }
   void impl_Fraction_Limit( int Fraction_Limit ) { v_Fraction_Limit = Fraction_Limit; }

   const string& impl_Id( ) const { return lazy_fetch( p_obj ), v_Id; }
   void impl_Id( const string& Id ) { v_Id = Id; }

   int impl_Int_Type( ) const { return lazy_fetch( p_obj ), v_Int_Type; }
   void impl_Int_Type( int Int_Type ) { v_Int_Type = Int_Type; }

   bool impl_Internal( ) const { return lazy_fetch( p_obj ), v_Internal; }
   void impl_Internal( bool Internal ) { v_Internal = Internal; }

   int impl_Max_Size( ) const { return lazy_fetch( p_obj ), v_Max_Size; }
   void impl_Max_Size( int Max_Size ) { v_Max_Size = Max_Size; }

   const string& impl_Max_Value( ) const { return lazy_fetch( p_obj ), v_Max_Value; }
   void impl_Max_Value( const string& Max_Value ) { v_Max_Value = Max_Value; }

   const string& impl_Min_Value( ) const { return lazy_fetch( p_obj ), v_Min_Value; }
   void impl_Min_Value( const string& Min_Value ) { v_Min_Value = Min_Value; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { v_Name = Name; }

   int impl_Numeric_Decimals( ) const { return lazy_fetch( p_obj ), v_Numeric_Decimals; }
   void impl_Numeric_Decimals( int Numeric_Decimals ) { v_Numeric_Decimals = Numeric_Decimals; }

   int impl_Numeric_Digits( ) const { return lazy_fetch( p_obj ), v_Numeric_Digits; }
   void impl_Numeric_Digits( int Numeric_Digits ) { v_Numeric_Digits = Numeric_Digits; }

   int impl_Numeric_Type( ) const { return lazy_fetch( p_obj ), v_Numeric_Type; }
   void impl_Numeric_Type( int Numeric_Type ) { v_Numeric_Type = Numeric_Type; }

   int impl_Primitive( ) const { return lazy_fetch( p_obj ), v_Primitive; }
   void impl_Primitive( int Primitive ) { v_Primitive = Primitive; }

   bool impl_Show_Plus_Sign( ) const { return lazy_fetch( p_obj ), v_Show_Plus_Sign; }
   void impl_Show_Plus_Sign( bool Show_Plus_Sign ) { v_Show_Plus_Sign = Show_Plus_Sign; }

   int impl_String_Domain( ) const { return lazy_fetch( p_obj ), v_String_Domain; }
   void impl_String_Domain( int String_Domain ) { v_String_Domain = String_Domain; }

   int impl_Time_Precision( ) const { return lazy_fetch( p_obj ), v_Time_Precision; }
   void impl_Time_Precision( int Time_Precision ) { v_Time_Precision = Time_Precision; }

   int impl_Zero_Padding( ) const { return lazy_fetch( p_obj ), v_Zero_Padding; }
   void impl_Zero_Padding( int Zero_Padding ) { v_Zero_Padding = Zero_Padding; }

   Meta_Workgroup& impl_Workgroup( )
   {
      if( !cp_Workgroup )
      {
         cp_Workgroup.init( );

         p_obj->setup_graph_parent( *cp_Workgroup, c_field_id_Workgroup, v_Workgroup );
      }
      return *cp_Workgroup;
   }

   const Meta_Workgroup& impl_Workgroup( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Workgroup )
      {
         cp_Workgroup.init( );

         p_obj->setup_graph_parent( *cp_Workgroup, c_field_id_Workgroup, v_Workgroup );
      }
      return *cp_Workgroup;
   }

   void impl_Workgroup( const string& key )
   {
      class_base_accessor cba( impl_Workgroup( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_child_Field( )
   {
      if( !cp_child_Field )
      {
         cp_child_Field.init( );

         p_obj->setup_graph_parent( *cp_child_Field, "300710" );
      }
      return *cp_child_Field;
   }

   const Meta_Field& impl_child_Field( ) const
   {
      if( !cp_child_Field )
      {
         cp_child_Field.init( );

         p_obj->setup_graph_parent( *cp_child_Field, "300710" );
      }
      return *cp_child_Field;
   }

   string get_field_value( int field ) const;
   void set_field_value( int field, const string& value );

   uint64_t get_state( ) const;

   const string& execute( const string& cmd_and_args );

   void clear_foreign_key( const string& field );

   void set_foreign_key_value( const string& field, const string& value );

   const string& get_foreign_key_value( const string& field );

   void get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const;

   void clear( );

   bool value_will_be_provided( const string& field_name );

   void validate( unsigned state, bool is_internal, validation_error_container* p_validation_errors );

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

   Meta_Type* p_obj;
   class_pointer< Meta_Type > cp_obj;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   int v_Date_Precision;
   int v_Fraction_Limit;
   string v_Id;
   int v_Int_Type;
   bool v_Internal;
   int v_Max_Size;
   string v_Max_Value;
   string v_Min_Value;
   string v_Name;
   int v_Numeric_Decimals;
   int v_Numeric_Digits;
   int v_Numeric_Type;
   int v_Primitive;
   bool v_Show_Plus_Sign;
   int v_String_Domain;
   int v_Time_Precision;
   int v_Zero_Padding;

   string v_Workgroup;
   mutable class_pointer< Meta_Workgroup > cp_Workgroup;

   mutable class_pointer< Meta_Field > cp_child_Field;
};

string Meta_Type::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Date_Precision( ) );
      break;

      case 1:
      retval = to_string( impl_Fraction_Limit( ) );
      break;

      case 2:
      retval = to_string( impl_Id( ) );
      break;

      case 3:
      retval = to_string( impl_Int_Type( ) );
      break;

      case 4:
      retval = to_string( impl_Internal( ) );
      break;

      case 5:
      retval = to_string( impl_Max_Size( ) );
      break;

      case 6:
      retval = to_string( impl_Max_Value( ) );
      break;

      case 7:
      retval = to_string( impl_Min_Value( ) );
      break;

      case 8:
      retval = to_string( impl_Name( ) );
      break;

      case 9:
      retval = to_string( impl_Numeric_Decimals( ) );
      break;

      case 10:
      retval = to_string( impl_Numeric_Digits( ) );
      break;

      case 11:
      retval = to_string( impl_Numeric_Type( ) );
      break;

      case 12:
      retval = to_string( impl_Primitive( ) );
      break;

      case 13:
      retval = to_string( impl_Show_Plus_Sign( ) );
      break;

      case 14:
      retval = to_string( impl_String_Domain( ) );
      break;

      case 15:
      retval = to_string( impl_Time_Precision( ) );
      break;

      case 16:
      retval = to_string( impl_Workgroup( ) );
      break;

      case 17:
      retval = to_string( impl_Zero_Padding( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }

   return retval;
}

void Meta_Type::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Type::impl, int >( *this, &Meta_Type::impl::impl_Date_Precision, value );
      break;

      case 1:
      func_string_setter< Meta_Type::impl, int >( *this, &Meta_Type::impl::impl_Fraction_Limit, value );
      break;

      case 2:
      func_string_setter< Meta_Type::impl, string >( *this, &Meta_Type::impl::impl_Id, value );
      break;

      case 3:
      func_string_setter< Meta_Type::impl, int >( *this, &Meta_Type::impl::impl_Int_Type, value );
      break;

      case 4:
      func_string_setter< Meta_Type::impl, bool >( *this, &Meta_Type::impl::impl_Internal, value );
      break;

      case 5:
      func_string_setter< Meta_Type::impl, int >( *this, &Meta_Type::impl::impl_Max_Size, value );
      break;

      case 6:
      func_string_setter< Meta_Type::impl, string >( *this, &Meta_Type::impl::impl_Max_Value, value );
      break;

      case 7:
      func_string_setter< Meta_Type::impl, string >( *this, &Meta_Type::impl::impl_Min_Value, value );
      break;

      case 8:
      func_string_setter< Meta_Type::impl, string >( *this, &Meta_Type::impl::impl_Name, value );
      break;

      case 9:
      func_string_setter< Meta_Type::impl, int >( *this, &Meta_Type::impl::impl_Numeric_Decimals, value );
      break;

      case 10:
      func_string_setter< Meta_Type::impl, int >( *this, &Meta_Type::impl::impl_Numeric_Digits, value );
      break;

      case 11:
      func_string_setter< Meta_Type::impl, int >( *this, &Meta_Type::impl::impl_Numeric_Type, value );
      break;

      case 12:
      func_string_setter< Meta_Type::impl, int >( *this, &Meta_Type::impl::impl_Primitive, value );
      break;

      case 13:
      func_string_setter< Meta_Type::impl, bool >( *this, &Meta_Type::impl::impl_Show_Plus_Sign, value );
      break;

      case 14:
      func_string_setter< Meta_Type::impl, int >( *this, &Meta_Type::impl::impl_String_Domain, value );
      break;

      case 15:
      func_string_setter< Meta_Type::impl, int >( *this, &Meta_Type::impl::impl_Time_Precision, value );
      break;

      case 16:
      func_string_setter< Meta_Type::impl, Meta_Workgroup >( *this, &Meta_Type::impl::impl_Workgroup, value );
      break;

      case 17:
      func_string_setter< Meta_Type::impl, int >( *this, &Meta_Type::impl::impl_Zero_Padding, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }
}

uint64_t Meta_Type::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start modifier_field_value)]
   if( get_obj( ).Primitive( ) == 0 ) // i.e. string
      state |= c_modifier_Is_String;
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   if( get_obj( ).Primitive( ) == 1 ) // i.e. datetime
      state |= c_modifier_Is_Datetime;
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   if( get_obj( ).Primitive( ) == 2 ) // i.e. date
      state |= c_modifier_Is_Date;
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   if( get_obj( ).Primitive( ) == 3 ) // i.e. time
      state |= c_modifier_Is_Time;
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   if( get_obj( ).Primitive( ) == 4 ) // i.e. numeric
      state |= c_modifier_Is_Numeric;
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   if( get_obj( ).Primitive( ) == 5 ) // i.e. int
      state |= c_modifier_Is_Int;
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   if( get_obj( ).Primitive( ) == 6 ) // i.e. bool
      state |= c_modifier_Is_Bool;
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   if( get_obj( ).Internal( ) == true )
      state |= c_modifier_Is_Internal;
   // [(finish modifier_field_value)]

   // [(start protect_equal)]
   if( check_equal( get_obj( ).Internal( ), true ) )
      state |= ( c_state_uneditable | c_state_undeletable );
   // [(finish protect_equal)]

   // [(start modifier_field_value)]
   if( get_obj( ).Int_Type( ) != 0 )
      state |= c_modifier_Is_Not_Normal_Int;
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   if( get_obj( ).Numeric_Type( ) != 0 )
      state |= c_modifier_Is_Not_Normal_Numeric;
   // [(finish modifier_field_value)]

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

const string& Meta_Type::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Type::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Workgroup || field == c_field_name_Workgroup )
      impl_Workgroup( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Type::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( field == c_field_id_Workgroup || field == c_field_name_Workgroup )
      v_Workgroup = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Type::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Workgroup || field == c_field_name_Workgroup )
      return v_Workgroup;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Type::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Workgroup, v_Workgroup ) );
}

void Meta_Type::impl::clear( )
{
   v_Date_Precision = gv_default_Date_Precision;
   v_Fraction_Limit = gv_default_Fraction_Limit;
   v_Id = gv_default_Id;
   v_Int_Type = gv_default_Int_Type;
   v_Internal = gv_default_Internal;
   v_Max_Size = gv_default_Max_Size;
   v_Max_Value = gv_default_Max_Value;
   v_Min_Value = gv_default_Min_Value;
   v_Name = gv_default_Name;
   v_Numeric_Decimals = gv_default_Numeric_Decimals;
   v_Numeric_Digits = gv_default_Numeric_Digits;
   v_Numeric_Type = gv_default_Numeric_Type;
   v_Primitive = gv_default_Primitive;
   v_Show_Plus_Sign = gv_default_Show_Plus_Sign;
   v_String_Domain = gv_default_String_Domain;
   v_Time_Precision = gv_default_Time_Precision;
   v_Zero_Padding = gv_default_Zero_Padding;

   v_Workgroup = string( );
   if( cp_Workgroup )
      p_obj->setup_foreign_key( *cp_Workgroup, v_Workgroup );
}

bool Meta_Type::impl::value_will_be_provided( const string& field_name )
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

void Meta_Type::impl::validate( unsigned state, bool is_internal, validation_error_container* p_validation_errors )
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

   string error_message;
   if( !is_null( v_Id )
    && ( v_Id != gv_default_Id
    || !value_will_be_provided( c_field_name_Id ) )
    && !g_Id_domain.is_valid( v_Id, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Id,
       get_module_string( c_field_display_name_Id ) + " " + error_message ) );

   if( !is_null( v_Max_Size )
    && ( v_Max_Size != gv_default_Max_Size
    || !value_will_be_provided( c_field_name_Max_Size ) )
    && !g_Max_Size_domain.is_valid( v_Max_Size, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Max_Size,
       get_module_string( c_field_display_name_Max_Size ) + " " + error_message ) );

   if( !is_null( v_Max_Value )
    && ( v_Max_Value != gv_default_Max_Value
    || !value_will_be_provided( c_field_name_Max_Value ) )
    && !g_Max_Value_domain.is_valid( v_Max_Value, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Max_Value,
       get_module_string( c_field_display_name_Max_Value ) + " " + error_message ) );

   if( !is_null( v_Min_Value )
    && ( v_Min_Value != gv_default_Min_Value
    || !value_will_be_provided( c_field_name_Min_Value ) )
    && !g_Min_Value_domain.is_valid( v_Min_Value, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Min_Value,
       get_module_string( c_field_display_name_Min_Value ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( v_Name != gv_default_Name
    || !value_will_be_provided( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   if( !is_null( v_Numeric_Decimals )
    && ( v_Numeric_Decimals != gv_default_Numeric_Decimals
    || !value_will_be_provided( c_field_name_Numeric_Decimals ) )
    && !g_Numeric_Decimals_domain.is_valid( v_Numeric_Decimals, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Numeric_Decimals,
       get_module_string( c_field_display_name_Numeric_Decimals ) + " " + error_message ) );

   if( !is_null( v_Numeric_Digits )
    && ( v_Numeric_Digits != gv_default_Numeric_Digits
    || !value_will_be_provided( c_field_name_Numeric_Digits ) )
    && !g_Numeric_Digits_domain.is_valid( v_Numeric_Digits, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Numeric_Digits,
       get_module_string( c_field_display_name_Numeric_Digits ) + " " + error_message ) );

   if( !g_date_precision_enum.count( v_Date_Precision ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Date_Precision,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Date_Precision ) ) ) ) );

   if( !g_fraction_limit_enum.count( v_Fraction_Limit ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Fraction_Limit,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Fraction_Limit ) ) ) ) );

   if( !g_int_type_enum.count( v_Int_Type ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Int_Type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Int_Type ) ) ) ) );

   if( !g_numeric_type_enum.count( v_Numeric_Type ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Numeric_Type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Numeric_Type ) ) ) ) );

   if( !g_primitive_enum.count( v_Primitive ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Primitive,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Primitive ) ) ) ) );

   if( !g_string_domain_enum.count( v_String_Domain ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_String_Domain,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_String_Domain ) ) ) ) );

   if( !g_time_precision_enum.count( v_Time_Precision ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Time_Precision,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Time_Precision ) ) ) ) );

   if( !g_zero_padding_enum.count( v_Zero_Padding ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Zero_Padding,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Zero_Padding ) ) ) ) );

   // [<start validate>]
//idk
   if( get_obj( ).Min_Value( ).empty( ) && !get_obj( ).Max_Value( ).empty( ) )
      p_validation_errors->insert( validation_error_value_type( // FUTURE: Should be in module_strings...
       c_field_name_Min_Value, get_module_string( c_field_display_name_Min_Value ) + " must not be empty." ) );

   if( !get_obj( ).Min_Value( ).empty( ) && get_obj( ).Max_Value( ).empty( ) )
      p_validation_errors->insert( validation_error_value_type( // FUTURE: Should be in module_strings...
       c_field_name_Max_Value, get_module_string( c_field_display_name_Max_Value ) + " must not be empty." ) );

   if( get_obj( ).Primitive( ) == c_enum_primitive_numeric )
   {
      if( get_obj( ).Numeric_Digits( ) < get_obj( ).Numeric_Decimals( ) )
         p_validation_errors->insert( validation_error_value_type( // FUTURE: Should be in module_strings...
          c_field_name_Numeric_Decimals, get_module_string( c_field_display_name_Numeric_Decimals )
          + " must be <= " + get_module_string( c_field_display_name_Numeric_Digits ) + "." ) );

      bool has_invalid = false;
      if( !get_obj( ).Min_Value( ).empty( ) && !is_valid_str_val< numeric >( get_obj( ).Min_Value( ) ) )
      {
         has_invalid = true;
         p_validation_errors->insert( validation_error_value_type( // FUTURE: Should be in module_strings...
          c_field_name_Min_Value, get_module_string( c_field_display_name_Min_Value ) + " must be a valid numeric." ) );
      }

      if( !get_obj( ).Max_Value( ).empty( ) && !is_valid_str_val< numeric >( get_obj( ).Max_Value( ) ) )
      {
         has_invalid = true;
         p_validation_errors->insert( validation_error_value_type( // FUTURE: Should be in module_strings...
          c_field_name_Max_Value, get_module_string( c_field_display_name_Max_Value ) + " must be a valid numeric." ) );
      }

      if( !has_invalid && !get_obj( ).Min_Value( ).empty( ) && !get_obj( ).Max_Value( ).empty( ) )
      {
         numeric min = from_string< numeric >( get_obj( ).Min_Value( ) );
         numeric max = from_string< numeric >( get_obj( ).Max_Value( ) );

         if( min >= max )
            p_validation_errors->insert( validation_error_value_type( // FUTURE: Should be in module_strings...
             c_field_name_Min_Value, get_module_string( c_field_display_name_Min_Value )
             + " must be less than " + get_module_string( c_field_display_name_Max_Value ) + "." ) );
      }
   }
   else if( get_obj( ).Primitive( ) == c_enum_primitive_int )
   {
      if( get_obj( ).Max_Size( ) > c_int_digits10 )
      {
         p_validation_errors->insert(
          validation_error_value_type( c_field_name_Max_Size, // FUTURE: Should be in module_strings...
          get_module_string( c_field_display_name_Max_Size ) + " must be <= " + to_string( c_int_digits10 ) + "." ) );
      }

      bool has_invalid = false;
      if( !get_obj( ).Min_Value( ).empty( ) && !is_valid_str_val< int >( get_obj( ).Min_Value( ) ) )
      {
         has_invalid = true;
         p_validation_errors->insert( validation_error_value_type( // FUTURE: Should be in module_strings...
          c_field_name_Min_Value, get_module_string( c_field_display_name_Min_Value ) + " must be a valid int." ) );
      }

      if( !get_obj( ).Max_Value( ).empty( ) && !is_valid_str_val< int >( get_obj( ).Max_Value( ) ) )
      {
         has_invalid = true;
         p_validation_errors->insert( validation_error_value_type( // FUTURE: Should be in module_strings...
          c_field_name_Max_Value, get_module_string( c_field_display_name_Max_Value ) + " must be a valid int." ) );
      }

      if( !has_invalid && !get_obj( ).Min_Value( ).empty( ) && !get_obj( ).Max_Value( ).empty( ) )
      {
         int min = from_string< int >( get_obj( ).Min_Value( ) );
         int max = from_string< int >( get_obj( ).Max_Value( ) );

         if( min >= max )
            p_validation_errors->insert( validation_error_value_type( // FUTURE: Should be in module_strings...
             c_field_name_Min_Value, get_module_string( c_field_display_name_Min_Value )
             + " must be less than " + get_module_string( c_field_display_name_Max_Value ) + "." ) );
      }
   }
   else if( get_obj( ).Primitive( ) == c_enum_primitive_date )
   {
      bool has_invalid = false;
      if( !get_obj( ).Min_Value( ).empty( ) && !is_valid_str_val< udate >( get_obj( ).Min_Value( ) ) )
      {
         has_invalid = true;
         p_validation_errors->insert( validation_error_value_type( // FUTURE: Should be in module_strings...
          c_field_name_Min_Value, get_module_string( c_field_display_name_Min_Value ) + " must be a valid date." ) );
      }

      if( !get_obj( ).Max_Value( ).empty( ) && !is_valid_str_val< udate >( get_obj( ).Max_Value( ) ) )
      {
         has_invalid = true;
         p_validation_errors->insert( validation_error_value_type( // FUTURE: Should be in module_strings...
          c_field_name_Max_Value, get_module_string( c_field_display_name_Max_Value ) + " must be a valid date." ) );
      }

      if( !has_invalid && !get_obj( ).Min_Value( ).empty( ) && !get_obj( ).Max_Value( ).empty( ) )
      {
         udate min = from_string< udate >( get_obj( ).Min_Value( ) );
         udate max = from_string< udate >( get_obj( ).Max_Value( ) );

         if( min >= max )
            p_validation_errors->insert( validation_error_value_type( // FUTURE: Should be in module_strings...
             c_field_name_Min_Value, get_module_string( c_field_display_name_Min_Value )
             + " must be less than " + get_module_string( c_field_display_name_Max_Value ) + "." ) );
      }
   }
   else if( get_obj( ).Primitive( ) == c_enum_primitive_time )
   {
      bool has_invalid = false;
      if( !get_obj( ).Min_Value( ).empty( ) && !is_valid_str_val< mtime >( get_obj( ).Min_Value( ) ) )
      {
         has_invalid = true;
         p_validation_errors->insert( validation_error_value_type( // FUTURE: Should be in module_strings...
          c_field_name_Min_Value, get_module_string( c_field_display_name_Min_Value ) + " must be a valid time." ) );
      }

      if( !get_obj( ).Max_Value( ).empty( ) && !is_valid_str_val< mtime >( get_obj( ).Max_Value( ) ) )
      {
         has_invalid = true;
         p_validation_errors->insert( validation_error_value_type( // FUTURE: Should be in module_strings...
          c_field_name_Max_Value, get_module_string( c_field_display_name_Max_Value ) + " must be a valid time." ) );
      }

      if( !has_invalid && !get_obj( ).Min_Value( ).empty( ) && !get_obj( ).Max_Value( ).empty( ) )
      {
         mtime min = from_string< mtime >( get_obj( ).Min_Value( ) );
         mtime max = from_string< mtime >( get_obj( ).Max_Value( ) );

         if( min >= max )
            p_validation_errors->insert( validation_error_value_type( // FUTURE: Should be in module_strings...
             c_field_name_Min_Value, get_module_string( c_field_display_name_Min_Value )
             + " must be less than " + get_module_string( c_field_display_name_Max_Value ) + "." ) );
      }
   }
   else if( get_obj( ).Primitive( ) == c_enum_primitive_datetime )
   {
      bool has_invalid = false;
      if( !get_obj( ).Min_Value( ).empty( ) && !is_valid_str_val< date_time >( get_obj( ).Min_Value( ) ) )
      {
         has_invalid = true;
         p_validation_errors->insert( validation_error_value_type( // FUTURE: Should be in module_strings...
          c_field_name_Min_Value, get_module_string( c_field_display_name_Min_Value ) + " must be a valid datetime." ) );
      }

      if( !get_obj( ).Max_Value( ).empty( ) && !is_valid_str_val< date_time >( get_obj( ).Max_Value( ) ) )
      {
         has_invalid = true;
         p_validation_errors->insert( validation_error_value_type( // FUTURE: Should be in module_strings...
          c_field_name_Max_Value, get_module_string( c_field_display_name_Max_Value ) + " must be a valid datetime." ) );
      }

      if( !has_invalid && !get_obj( ).Min_Value( ).empty( ) && !get_obj( ).Max_Value( ).empty( ) )
      {
         date_time min = from_string< date_time >( get_obj( ).Min_Value( ) );
         date_time max = from_string< date_time >( get_obj( ).Max_Value( ) );

         if( min >= max )
            p_validation_errors->insert( validation_error_value_type( // FUTURE: Should be in module_strings...
             c_field_name_Min_Value, get_module_string( c_field_display_name_Min_Value )
             + " must be less than " + get_module_string( c_field_display_name_Max_Value ) + "." ) );
      }
   }
   // [<finish validate>]
}

void Meta_Type::impl::after_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   if( cp_Workgroup )
      p_obj->setup_foreign_key( *cp_Workgroup, v_Workgroup );

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Type::impl::finalise_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Type::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Type::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Type::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [(start parent_auto_int_inc)]
   if( is_create && is_null( get_obj( ).Id( ) ) )
   {
      get_obj( ).Workgroup( ).op_update( get_obj( ).Workgroup( ), FIELD_NAME( Meta, Workgroup, Next_Type_Id ) );

      get_obj( ).Id( get_obj( ).Workgroup( ).Next_Type_Id( ) );

      get_obj( ).Workgroup( ).Next_Type_Id( auto_int_increment( get_obj( ).Workgroup( ).Next_Type_Id( ) ) );
      get_obj( ).Workgroup( ).op_apply( );
   }
   // [(finish parent_auto_int_inc)]

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Type::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Type::impl::can_destroy( bool is_internal )
{
   bool retval = is_internal || !( get_state( ) & c_state_undeletable );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Type::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Type::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Type::impl::set_default_values( )
{
   clear( );
}

bool Meta_Type::impl::is_filtered( ) const
{
   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

#undef MODULE_TRACE
#define MODULE_TRACE( x ) trace( x )

Meta_Type::Meta_Type( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Type::~Meta_Type( )
{
   cleanup( );
   delete p_impl;
}

int Meta_Type::Date_Precision( ) const
{
   return p_impl->impl_Date_Precision( );
}

void Meta_Type::Date_Precision( int Date_Precision )
{
   p_impl->impl_Date_Precision( Date_Precision );
}

int Meta_Type::Fraction_Limit( ) const
{
   return p_impl->impl_Fraction_Limit( );
}

void Meta_Type::Fraction_Limit( int Fraction_Limit )
{
   p_impl->impl_Fraction_Limit( Fraction_Limit );
}

const string& Meta_Type::Id( ) const
{
   return p_impl->impl_Id( );
}

void Meta_Type::Id( const string& Id )
{
   p_impl->impl_Id( Id );
}

int Meta_Type::Int_Type( ) const
{
   return p_impl->impl_Int_Type( );
}

void Meta_Type::Int_Type( int Int_Type )
{
   p_impl->impl_Int_Type( Int_Type );
}

bool Meta_Type::Internal( ) const
{
   return p_impl->impl_Internal( );
}

void Meta_Type::Internal( bool Internal )
{
   p_impl->impl_Internal( Internal );
}

int Meta_Type::Max_Size( ) const
{
   return p_impl->impl_Max_Size( );
}

void Meta_Type::Max_Size( int Max_Size )
{
   p_impl->impl_Max_Size( Max_Size );
}

const string& Meta_Type::Max_Value( ) const
{
   return p_impl->impl_Max_Value( );
}

void Meta_Type::Max_Value( const string& Max_Value )
{
   p_impl->impl_Max_Value( Max_Value );
}

const string& Meta_Type::Min_Value( ) const
{
   return p_impl->impl_Min_Value( );
}

void Meta_Type::Min_Value( const string& Min_Value )
{
   p_impl->impl_Min_Value( Min_Value );
}

const string& Meta_Type::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_Type::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

int Meta_Type::Numeric_Decimals( ) const
{
   return p_impl->impl_Numeric_Decimals( );
}

void Meta_Type::Numeric_Decimals( int Numeric_Decimals )
{
   p_impl->impl_Numeric_Decimals( Numeric_Decimals );
}

int Meta_Type::Numeric_Digits( ) const
{
   return p_impl->impl_Numeric_Digits( );
}

void Meta_Type::Numeric_Digits( int Numeric_Digits )
{
   p_impl->impl_Numeric_Digits( Numeric_Digits );
}

int Meta_Type::Numeric_Type( ) const
{
   return p_impl->impl_Numeric_Type( );
}

void Meta_Type::Numeric_Type( int Numeric_Type )
{
   p_impl->impl_Numeric_Type( Numeric_Type );
}

int Meta_Type::Primitive( ) const
{
   return p_impl->impl_Primitive( );
}

void Meta_Type::Primitive( int Primitive )
{
   p_impl->impl_Primitive( Primitive );
}

bool Meta_Type::Show_Plus_Sign( ) const
{
   return p_impl->impl_Show_Plus_Sign( );
}

void Meta_Type::Show_Plus_Sign( bool Show_Plus_Sign )
{
   p_impl->impl_Show_Plus_Sign( Show_Plus_Sign );
}

int Meta_Type::String_Domain( ) const
{
   return p_impl->impl_String_Domain( );
}

void Meta_Type::String_Domain( int String_Domain )
{
   p_impl->impl_String_Domain( String_Domain );
}

int Meta_Type::Time_Precision( ) const
{
   return p_impl->impl_Time_Precision( );
}

void Meta_Type::Time_Precision( int Time_Precision )
{
   p_impl->impl_Time_Precision( Time_Precision );
}

int Meta_Type::Zero_Padding( ) const
{
   return p_impl->impl_Zero_Padding( );
}

void Meta_Type::Zero_Padding( int Zero_Padding )
{
   p_impl->impl_Zero_Padding( Zero_Padding );
}

Meta_Workgroup& Meta_Type::Workgroup( )
{
   return p_impl->impl_Workgroup( );
}

const Meta_Workgroup& Meta_Type::Workgroup( ) const
{
   return p_impl->impl_Workgroup( );
}

void Meta_Type::Workgroup( const string& key )
{
   p_impl->impl_Workgroup( key );
}

Meta_Field& Meta_Type::child_Field( )
{
   return p_impl->impl_child_Field( );
}

const Meta_Field& Meta_Type::child_Field( ) const
{
   return p_impl->impl_child_Field( );
}

string Meta_Type::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Type::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

bool Meta_Type::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Type::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Type::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Type::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Type::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Type::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Type::clear( )
{
   p_impl->clear( );
}

void Meta_Type::validate( unsigned state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Type::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Type::finalise_fetch( )
{
   p_impl->finalise_fetch( );
}

void Meta_Type::at_create( )
{
   p_impl->at_create( );
}

void Meta_Type::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Type::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Type::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Type::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Type::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Type::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Type::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Type::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Type::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( 0 );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Date_Precision )
   {
      p_id = c_field_id_Date_Precision;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Fraction_Limit )
   {
      p_id = c_field_id_Fraction_Limit;

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
   else if( name == c_field_name_Int_Type )
   {
      p_id = c_field_id_Int_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Internal )
   {
      p_id = c_field_id_Internal;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Max_Size )
   {
      p_id = c_field_id_Max_Size;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Max_Value )
   {
      p_id = c_field_id_Max_Value;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Min_Value )
   {
      p_id = c_field_id_Min_Value;

      if( p_type_name )
         *p_type_name = "string";

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
   else if( name == c_field_name_Numeric_Decimals )
   {
      p_id = c_field_id_Numeric_Decimals;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Numeric_Digits )
   {
      p_id = c_field_id_Numeric_Digits;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Numeric_Type )
   {
      p_id = c_field_id_Numeric_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Primitive )
   {
      p_id = c_field_id_Primitive;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Show_Plus_Sign )
   {
      p_id = c_field_id_Show_Plus_Sign;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_String_Domain )
   {
      p_id = c_field_id_String_Domain;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Time_Precision )
   {
      p_id = c_field_id_Time_Precision;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Workgroup )
   {
      p_id = c_field_id_Workgroup;

      if( p_type_name )
         *p_type_name = "Meta_Workgroup";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Zero_Padding )
   {
      p_id = c_field_id_Zero_Padding;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_id;
}

const char* Meta_Type::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( 0 );

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Date_Precision )
   {
      p_name = c_field_name_Date_Precision;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Fraction_Limit )
   {
      p_name = c_field_name_Fraction_Limit;

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
   else if( id == c_field_id_Int_Type )
   {
      p_name = c_field_name_Int_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Internal )
   {
      p_name = c_field_name_Internal;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Max_Size )
   {
      p_name = c_field_name_Max_Size;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Max_Value )
   {
      p_name = c_field_name_Max_Value;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Min_Value )
   {
      p_name = c_field_name_Min_Value;

      if( p_type_name )
         *p_type_name = "string";

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
   else if( id == c_field_id_Numeric_Decimals )
   {
      p_name = c_field_name_Numeric_Decimals;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Numeric_Digits )
   {
      p_name = c_field_name_Numeric_Digits;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Numeric_Type )
   {
      p_name = c_field_name_Numeric_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Primitive )
   {
      p_name = c_field_name_Primitive;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Show_Plus_Sign )
   {
      p_name = c_field_name_Show_Plus_Sign;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_String_Domain )
   {
      p_name = c_field_name_String_Domain;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Time_Precision )
   {
      p_name = c_field_name_Time_Precision;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Workgroup )
   {
      p_name = c_field_name_Workgroup;

      if( p_type_name )
         *p_type_name = "Meta_Workgroup";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Zero_Padding )
   {
      p_name = c_field_name_Zero_Padding;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_name;
}

string Meta_Type::get_field_display_name( const string& id ) const
{
   string display_name;

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_display_name" );
   else if( id == c_field_id_Date_Precision )
      display_name = get_module_string( c_field_display_name_Date_Precision );
   else if( id == c_field_id_Fraction_Limit )
      display_name = get_module_string( c_field_display_name_Fraction_Limit );
   else if( id == c_field_id_Id )
      display_name = get_module_string( c_field_display_name_Id );
   else if( id == c_field_id_Int_Type )
      display_name = get_module_string( c_field_display_name_Int_Type );
   else if( id == c_field_id_Internal )
      display_name = get_module_string( c_field_display_name_Internal );
   else if( id == c_field_id_Max_Size )
      display_name = get_module_string( c_field_display_name_Max_Size );
   else if( id == c_field_id_Max_Value )
      display_name = get_module_string( c_field_display_name_Max_Value );
   else if( id == c_field_id_Min_Value )
      display_name = get_module_string( c_field_display_name_Min_Value );
   else if( id == c_field_id_Name )
      display_name = get_module_string( c_field_display_name_Name );
   else if( id == c_field_id_Numeric_Decimals )
      display_name = get_module_string( c_field_display_name_Numeric_Decimals );
   else if( id == c_field_id_Numeric_Digits )
      display_name = get_module_string( c_field_display_name_Numeric_Digits );
   else if( id == c_field_id_Numeric_Type )
      display_name = get_module_string( c_field_display_name_Numeric_Type );
   else if( id == c_field_id_Primitive )
      display_name = get_module_string( c_field_display_name_Primitive );
   else if( id == c_field_id_Show_Plus_Sign )
      display_name = get_module_string( c_field_display_name_Show_Plus_Sign );
   else if( id == c_field_id_String_Domain )
      display_name = get_module_string( c_field_display_name_String_Domain );
   else if( id == c_field_id_Time_Precision )
      display_name = get_module_string( c_field_display_name_Time_Precision );
   else if( id == c_field_id_Workgroup )
      display_name = get_module_string( c_field_display_name_Workgroup );
   else if( id == c_field_id_Zero_Padding )
      display_name = get_module_string( c_field_display_name_Zero_Padding );

   return display_name;
}

void Meta_Type::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Type::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Type::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Type::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Type::setup_foreign_key( Meta_Workgroup& o, const string& value )
{
   static_cast< Meta_Workgroup& >( o ).set_key( value );
}

void Meta_Type::setup_graph_parent( Meta_Field& o, const string& foreign_key_field )
{
   static_cast< Meta_Field& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Type::setup_graph_parent(
 Meta_Workgroup& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Workgroup& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Workgroup& >( o ).set_key( init_value );
}

size_t Meta_Type::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Type::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Type::get_num_foreign_key_children( bool is_internal ) const
{
   size_t rc = 1;

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

class_base* Meta_Type::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   if( child_num >= 1 )
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
            next_child_field = "300710";
            p_class_base = &child_Field( );
         }
         break;
      }
   }

   return p_class_base;
}

const char* Meta_Type::class_id( ) const
{
   return static_class_id( );
}

const char* Meta_Type::class_name( ) const
{
   return static_class_name( );
}

const char* Meta_Type::plural_name( ) const
{
   return static_plural_name( );
}

const char* Meta_Type::module_id( ) const
{
   return static_module_id( );
}

const char* Meta_Type::module_name( ) const
{
   return static_module_name( );
}

string Meta_Type::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );
   key += "type";

   return get_module_string( key );
}

bool Meta_Type::get_is_alias( ) const
{
   return false;
}

void Meta_Type::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Type::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Type::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( sub_context == "_300710" || sub_context == "child_Field" )
      p_class_base = &child_Field( );
   else if( sub_context == c_field_id_Workgroup || sub_context == c_field_name_Workgroup )
      p_class_base = &Workgroup( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Type::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Date_Precision" );
   names.push_back( "C_Fraction_Limit" );
   names.push_back( "C_Id" );
   names.push_back( "C_Int_Type" );
   names.push_back( "C_Internal" );
   names.push_back( "C_Max_Size" );
   names.push_back( "C_Max_Value" );
   names.push_back( "C_Min_Value" );
   names.push_back( "C_Name" );
   names.push_back( "C_Numeric_Decimals" );
   names.push_back( "C_Numeric_Digits" );
   names.push_back( "C_Numeric_Type" );
   names.push_back( "C_Primitive" );
   names.push_back( "C_Show_Plus_Sign" );
   names.push_back( "C_String_Domain" );
   names.push_back( "C_Time_Precision" );
   names.push_back( "C_Workgroup" );
   names.push_back( "C_Zero_Padding" );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Type::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( to_string( Date_Precision( ) ) );
   values.push_back( to_string( Fraction_Limit( ) ) );
   values.push_back( sql_quote( to_string( Id( ) ) ) );
   values.push_back( to_string( Int_Type( ) ) );
   values.push_back( to_string( Internal( ) ) );
   values.push_back( to_string( Max_Size( ) ) );
   values.push_back( sql_quote( to_string( Max_Value( ) ) ) );
   values.push_back( sql_quote( to_string( Min_Value( ) ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( to_string( Numeric_Decimals( ) ) );
   values.push_back( to_string( Numeric_Digits( ) ) );
   values.push_back( to_string( Numeric_Type( ) ) );
   values.push_back( to_string( Primitive( ) ) );
   values.push_back( to_string( Show_Plus_Sign( ) ) );
   values.push_back( to_string( String_Domain( ) ) );
   values.push_back( to_string( Time_Precision( ) ) );
   values.push_back( sql_quote( to_string( Workgroup( ) ) ) );
   values.push_back( to_string( Zero_Padding( ) ) );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Type::get_required_field_names(
 set< string >& names, bool required_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, required_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Type::get_always_required_field_names(
 set< string >& names, bool required_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )required_transients;

   // [(start modifier_field_value)]
   dependents.insert( "Primitive" ); // (for Is_String modifier)

   if( ( required_transients && is_field_transient( e_field_id_Primitive ) )
    || ( !required_transients && !is_field_transient( e_field_id_Primitive ) ) )
      names.insert( "Primitive" );
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   dependents.insert( "Primitive" ); // (for Is_Datetime modifier)

   if( ( required_transients && is_field_transient( e_field_id_Primitive ) )
    || ( !required_transients && !is_field_transient( e_field_id_Primitive ) ) )
      names.insert( "Primitive" );
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   dependents.insert( "Primitive" ); // (for Is_Date modifier)

   if( ( required_transients && is_field_transient( e_field_id_Primitive ) )
    || ( !required_transients && !is_field_transient( e_field_id_Primitive ) ) )
      names.insert( "Primitive" );
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   dependents.insert( "Primitive" ); // (for Is_Time modifier)

   if( ( required_transients && is_field_transient( e_field_id_Primitive ) )
    || ( !required_transients && !is_field_transient( e_field_id_Primitive ) ) )
      names.insert( "Primitive" );
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   dependents.insert( "Primitive" ); // (for Is_Numeric modifier)

   if( ( required_transients && is_field_transient( e_field_id_Primitive ) )
    || ( !required_transients && !is_field_transient( e_field_id_Primitive ) ) )
      names.insert( "Primitive" );
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   dependents.insert( "Primitive" ); // (for Is_Int modifier)

   if( ( required_transients && is_field_transient( e_field_id_Primitive ) )
    || ( !required_transients && !is_field_transient( e_field_id_Primitive ) ) )
      names.insert( "Primitive" );
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   dependents.insert( "Primitive" ); // (for Is_Bool modifier)

   if( ( required_transients && is_field_transient( e_field_id_Primitive ) )
    || ( !required_transients && !is_field_transient( e_field_id_Primitive ) ) )
      names.insert( "Primitive" );
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   dependents.insert( "Internal" ); // (for Is_Internal modifier)

   if( ( required_transients && is_field_transient( e_field_id_Internal ) )
    || ( !required_transients && !is_field_transient( e_field_id_Internal ) ) )
      names.insert( "Internal" );
   // [(finish modifier_field_value)]

   // [(start protect_equal)]
   dependents.insert( "Internal" );

   if( ( required_transients && is_field_transient( e_field_id_Internal ) )
    || ( !required_transients && !is_field_transient( e_field_id_Internal ) ) )
      names.insert( "Internal" );
   // [(finish protect_equal)]

   // [(start modifier_field_value)]
   dependents.insert( "Int_Type" ); // (for Is_Not_Normal_Int modifier)

   if( ( required_transients && is_field_transient( e_field_id_Int_Type ) )
    || ( !required_transients && !is_field_transient( e_field_id_Int_Type ) ) )
      names.insert( "Int_Type" );
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   dependents.insert( "Numeric_Type" ); // (for Is_Not_Normal_Numeric modifier)

   if( ( required_transients && is_field_transient( e_field_id_Numeric_Type ) )
    || ( !required_transients && !is_field_transient( e_field_id_Numeric_Type ) ) )
      names.insert( "Numeric_Type" );
   // [(finish modifier_field_value)]

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Type::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Type::do_generate_sql( generate_sql_type type, vector< string >& sql_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts );
}

const char* Meta_Type::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Type::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Type::static_lock_class_id( )
{
   return "102100";
}

const char* Meta_Type::static_check_class_name( )
{
   return "Type";
}

bool Meta_Type::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Type::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.102100" );
}

void Meta_Type::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "102112", "Date_Precision", "int", false ) );
   all_field_info.push_back( field_info( "102115", "Fraction_Limit", "int", false ) );
   all_field_info.push_back( field_info( "102111", "Id", "string", false ) );
   all_field_info.push_back( field_info( "102116", "Int_Type", "int", false ) );
   all_field_info.push_back( field_info( "102109", "Internal", "bool", false ) );
   all_field_info.push_back( field_info( "102103", "Max_Size", "int", false ) );
   all_field_info.push_back( field_info( "102105", "Max_Value", "string", false ) );
   all_field_info.push_back( field_info( "102104", "Min_Value", "string", false ) );
   all_field_info.push_back( field_info( "102101", "Name", "string", false ) );
   all_field_info.push_back( field_info( "102107", "Numeric_Decimals", "int", false ) );
   all_field_info.push_back( field_info( "102106", "Numeric_Digits", "int", false ) );
   all_field_info.push_back( field_info( "102117", "Numeric_Type", "int", false ) );
   all_field_info.push_back( field_info( "102102", "Primitive", "int", false ) );
   all_field_info.push_back( field_info( "102113", "Show_Plus_Sign", "bool", false ) );
   all_field_info.push_back( field_info( "102108", "String_Domain", "int", false ) );
   all_field_info.push_back( field_info( "102110", "Time_Precision", "int", false ) );
   all_field_info.push_back( field_info( "300200", "Workgroup", "Meta_Workgroup", false ) );
   all_field_info.push_back( field_info( "102114", "Zero_Padding", "int", false ) );
}

void Meta_Type::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Workgroup, make_pair( "Meta.102100", "Meta_Workgroup" ) ) );
}

int Meta_Type::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Type::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Type::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "102112";
      break;

      case 2:
      p_id = "102115";
      break;

      case 3:
      p_id = "102111";
      break;

      case 4:
      p_id = "102116";
      break;

      case 5:
      p_id = "102109";
      break;

      case 6:
      p_id = "102103";
      break;

      case 7:
      p_id = "102105";
      break;

      case 8:
      p_id = "102104";
      break;

      case 9:
      p_id = "102101";
      break;

      case 10:
      p_id = "102107";
      break;

      case 11:
      p_id = "102106";
      break;

      case 12:
      p_id = "102117";
      break;

      case 13:
      p_id = "102102";
      break;

      case 14:
      p_id = "102113";
      break;

      case 15:
      p_id = "102108";
      break;

      case 16:
      p_id = "102110";
      break;

      case 17:
      p_id = "300200";
      break;

      case 18:
      p_id = "102114";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Type" );

   return p_id;
}

const char* Meta_Type::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Date_Precision";
      break;

      case 2:
      p_id = "Fraction_Limit";
      break;

      case 3:
      p_id = "Id";
      break;

      case 4:
      p_id = "Int_Type";
      break;

      case 5:
      p_id = "Internal";
      break;

      case 6:
      p_id = "Max_Size";
      break;

      case 7:
      p_id = "Max_Value";
      break;

      case 8:
      p_id = "Min_Value";
      break;

      case 9:
      p_id = "Name";
      break;

      case 10:
      p_id = "Numeric_Decimals";
      break;

      case 11:
      p_id = "Numeric_Digits";
      break;

      case 12:
      p_id = "Numeric_Type";
      break;

      case 13:
      p_id = "Primitive";
      break;

      case 14:
      p_id = "Show_Plus_Sign";
      break;

      case 15:
      p_id = "String_Domain";
      break;

      case 16:
      p_id = "Time_Precision";
      break;

      case 17:
      p_id = "Workgroup";
      break;

      case 18:
      p_id = "Zero_Padding";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Type" );

   return p_id;
}

int Meta_Type::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Date_Precision || field == c_field_name_Date_Precision )
      rc += 1;
   else if( field == c_field_id_Fraction_Limit || field == c_field_name_Fraction_Limit )
      rc += 2;
   else if( field == c_field_id_Id || field == c_field_name_Id )
      rc += 3;
   else if( field == c_field_id_Int_Type || field == c_field_name_Int_Type )
      rc += 4;
   else if( field == c_field_id_Internal || field == c_field_name_Internal )
      rc += 5;
   else if( field == c_field_id_Max_Size || field == c_field_name_Max_Size )
      rc += 6;
   else if( field == c_field_id_Max_Value || field == c_field_name_Max_Value )
      rc += 7;
   else if( field == c_field_id_Min_Value || field == c_field_name_Min_Value )
      rc += 8;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 9;
   else if( field == c_field_id_Numeric_Decimals || field == c_field_name_Numeric_Decimals )
      rc += 10;
   else if( field == c_field_id_Numeric_Digits || field == c_field_name_Numeric_Digits )
      rc += 11;
   else if( field == c_field_id_Numeric_Type || field == c_field_name_Numeric_Type )
      rc += 12;
   else if( field == c_field_id_Primitive || field == c_field_name_Primitive )
      rc += 13;
   else if( field == c_field_id_Show_Plus_Sign || field == c_field_name_Show_Plus_Sign )
      rc += 14;
   else if( field == c_field_id_String_Domain || field == c_field_name_String_Domain )
      rc += 15;
   else if( field == c_field_id_Time_Precision || field == c_field_name_Time_Precision )
      rc += 16;
   else if( field == c_field_id_Workgroup || field == c_field_name_Workgroup )
      rc += 17;
   else if( field == c_field_id_Zero_Padding || field == c_field_name_Zero_Padding )
      rc += 18;

   return rc - 1;
}

procedure_info_container& Meta_Type::static_get_procedure_info( )
{
   static procedure_info_container procedures;

   return procedures;
}

string Meta_Type::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(64),"
    "C_Ver_ INTEGER NOT NULL,"
    "C_Rev_ INTEGER NOT NULL,"
    "C_Typ_ VARCHAR(24) NOT NULL,"
    "C_Date_Precision INTEGER NOT NULL,"
    "C_Fraction_Limit INTEGER NOT NULL,"
    "C_Id VARCHAR(128) NOT NULL,"
    "C_Int_Type INTEGER NOT NULL,"
    "C_Internal INTEGER NOT NULL,"
    "C_Max_Size INTEGER NOT NULL,"
    "C_Max_Value VARCHAR(128) NOT NULL,"
    "C_Min_Value VARCHAR(128) NOT NULL,"
    "C_Name VARCHAR(128) NOT NULL,"
    "C_Numeric_Decimals INTEGER NOT NULL,"
    "C_Numeric_Digits INTEGER NOT NULL,"
    "C_Numeric_Type INTEGER NOT NULL,"
    "C_Primitive INTEGER NOT NULL,"
    "C_Show_Plus_Sign INTEGER NOT NULL,"
    "C_String_Domain INTEGER NOT NULL,"
    "C_Time_Precision INTEGER NOT NULL,"
    "C_Workgroup VARCHAR(64) NOT NULL,"
    "C_Zero_Padding INTEGER NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Type::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Type::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "enum_date_precision_0", get_enum_string_date_precision( 0 ) ) );
   pairs.push_back( make_pair( "enum_date_precision_1", get_enum_string_date_precision( 1 ) ) );
   pairs.push_back( make_pair( "enum_date_precision_2", get_enum_string_date_precision( 2 ) ) );

   pairs.push_back( make_pair( "enum_fraction_limit_0", get_enum_string_fraction_limit( 0 ) ) );
   pairs.push_back( make_pair( "enum_fraction_limit_2", get_enum_string_fraction_limit( 2 ) ) );
   pairs.push_back( make_pair( "enum_fraction_limit_4", get_enum_string_fraction_limit( 4 ) ) );

   pairs.push_back( make_pair( "enum_int_type_0", get_enum_string_int_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_int_type_3", get_enum_string_int_type( 3 ) ) );
   pairs.push_back( make_pair( "enum_int_type_1", get_enum_string_int_type( 1 ) ) );
   pairs.push_back( make_pair( "enum_int_type_2", get_enum_string_int_type( 2 ) ) );

   pairs.push_back( make_pair( "enum_numeric_type_0", get_enum_string_numeric_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_numeric_type_1", get_enum_string_numeric_type( 1 ) ) );

   pairs.push_back( make_pair( "enum_primitive_0", get_enum_string_primitive( 0 ) ) );
   pairs.push_back( make_pair( "enum_primitive_1", get_enum_string_primitive( 1 ) ) );
   pairs.push_back( make_pair( "enum_primitive_2", get_enum_string_primitive( 2 ) ) );
   pairs.push_back( make_pair( "enum_primitive_3", get_enum_string_primitive( 3 ) ) );
   pairs.push_back( make_pair( "enum_primitive_4", get_enum_string_primitive( 4 ) ) );
   pairs.push_back( make_pair( "enum_primitive_5", get_enum_string_primitive( 5 ) ) );
   pairs.push_back( make_pair( "enum_primitive_6", get_enum_string_primitive( 6 ) ) );

   pairs.push_back( make_pair( "enum_string_domain_0", get_enum_string_string_domain( 0 ) ) );
   pairs.push_back( make_pair( "enum_string_domain_2", get_enum_string_string_domain( 2 ) ) );
   pairs.push_back( make_pair( "enum_string_domain_3", get_enum_string_string_domain( 3 ) ) );
   pairs.push_back( make_pair( "enum_string_domain_1", get_enum_string_string_domain( 1 ) ) );

   pairs.push_back( make_pair( "enum_time_precision_0", get_enum_string_time_precision( 0 ) ) );
   pairs.push_back( make_pair( "enum_time_precision_1", get_enum_string_time_precision( 1 ) ) );
   pairs.push_back( make_pair( "enum_time_precision_2", get_enum_string_time_precision( 2 ) ) );

   pairs.push_back( make_pair( "enum_zero_padding_0", get_enum_string_zero_padding( 0 ) ) );
   pairs.push_back( make_pair( "enum_zero_padding_1", get_enum_string_zero_padding( 1 ) ) );
   pairs.push_back( make_pair( "enum_zero_padding_2", get_enum_string_zero_padding( 2 ) ) );
}

void Meta_Type::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Workgroup,C_Id" );
   indexes.push_back( "C_Workgroup,C_Name" );
}

void Meta_Type::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Workgroup,C_Id" );
   indexes.push_back( "C_Workgroup,C_Name" );
}

void Meta_Type::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Type::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Type::static_insert_external_alias( const string& module_and_class_id, Meta_Type* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Type::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Type::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_date_precision_enum.insert( 0 );
   g_date_precision_enum.insert( 1 );
   g_date_precision_enum.insert( 2 );
   g_fraction_limit_enum.insert( 0 );
   g_fraction_limit_enum.insert( 2 );
   g_fraction_limit_enum.insert( 4 );
   g_int_type_enum.insert( 0 );
   g_int_type_enum.insert( 3 );
   g_int_type_enum.insert( 1 );
   g_int_type_enum.insert( 2 );
   g_numeric_type_enum.insert( 0 );
   g_numeric_type_enum.insert( 1 );
   g_primitive_enum.insert( 0 );
   g_primitive_enum.insert( 1 );
   g_primitive_enum.insert( 2 );
   g_primitive_enum.insert( 3 );
   g_primitive_enum.insert( 4 );
   g_primitive_enum.insert( 5 );
   g_primitive_enum.insert( 6 );
   g_string_domain_enum.insert( 0 );
   g_string_domain_enum.insert( 2 );
   g_string_domain_enum.insert( 3 );
   g_string_domain_enum.insert( 1 );
   g_time_precision_enum.insert( 0 );
   g_time_precision_enum.insert( 1 );
   g_time_precision_enum.insert( 2 );
   g_zero_padding_enum.insert( 0 );
   g_zero_padding_enum.insert( 1 );
   g_zero_padding_enum.insert( 2 );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Type::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}

