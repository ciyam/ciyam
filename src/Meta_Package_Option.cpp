// Copyright (c) 2008-2013 CIYAM Open Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

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

#include "Meta_Package_Option.h"

#include "Meta_Model.h"
#include "Meta_Package.h"

#include "ciyam_base.h"
#include "class_domains.h"
#include "module_strings.h"
#include "class_utilities.h"
#include "command_handler.h"
#include "module_interface.h"

// [<start includes>]
// [<finish includes>]

using namespace std;

// [<start namespaces>]
// [<finish namespaces>]

template< > inline string to_string( const Meta_Model& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Package& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Model& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Package& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline int system( const string& cmd ) { return exec_system( cmd ); }

namespace
{

#include "Meta_Package_Option.cmh"

const int32_t c_version = 1;

const char* const c_okay = "okay";

const char* const c_field_id_Actions = "137116";
const char* const c_field_id_Date = "137111";
const char* const c_field_id_Datetime = "137110";
const char* const c_field_id_Id = "137101";
const char* const c_field_id_Installed = "137103";
const char* const c_field_id_Integer = "137114";
const char* const c_field_id_Is_Other_Package = "137104";
const char* const c_field_id_Model = "302825";
const char* const c_field_id_Name = "137115";
const char* const c_field_id_Numeric = "137113";
const char* const c_field_id_Other_Package = "302830";
const char* const c_field_id_Other_Package_Required = "137105";
const char* const c_field_id_Other_Package_Type = "137106";
const char* const c_field_id_Package = "302820";
const char* const c_field_id_Primitive = "137108";
const char* const c_field_id_String = "137109";
const char* const c_field_id_Time = "137112";
const char* const c_field_id_Use_Option = "137102";
const char* const c_field_id_Value = "137107";

const char* const c_field_name_Actions = "Actions";
const char* const c_field_name_Date = "Date";
const char* const c_field_name_Datetime = "Datetime";
const char* const c_field_name_Id = "Id";
const char* const c_field_name_Installed = "Installed";
const char* const c_field_name_Integer = "Integer";
const char* const c_field_name_Is_Other_Package = "Is_Other_Package";
const char* const c_field_name_Model = "Model";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Numeric = "Numeric";
const char* const c_field_name_Other_Package = "Other_Package";
const char* const c_field_name_Other_Package_Required = "Other_Package_Required";
const char* const c_field_name_Other_Package_Type = "Other_Package_Type";
const char* const c_field_name_Package = "Package";
const char* const c_field_name_Primitive = "Primitive";
const char* const c_field_name_String = "String";
const char* const c_field_name_Time = "Time";
const char* const c_field_name_Use_Option = "Use_Option";
const char* const c_field_name_Value = "Value";

const char* const c_field_display_name_Actions = "field_package_option_actions";
const char* const c_field_display_name_Date = "field_package_option_date";
const char* const c_field_display_name_Datetime = "field_package_option_datetime";
const char* const c_field_display_name_Id = "field_package_option_id";
const char* const c_field_display_name_Installed = "field_package_option_installed";
const char* const c_field_display_name_Integer = "field_package_option_integer";
const char* const c_field_display_name_Is_Other_Package = "field_package_option_is_other_package";
const char* const c_field_display_name_Model = "field_package_option_model";
const char* const c_field_display_name_Name = "field_package_option_name";
const char* const c_field_display_name_Numeric = "field_package_option_numeric";
const char* const c_field_display_name_Other_Package = "field_package_option_other_package";
const char* const c_field_display_name_Other_Package_Required = "field_package_option_other_package_required";
const char* const c_field_display_name_Other_Package_Type = "field_package_option_other_package_type";
const char* const c_field_display_name_Package = "field_package_option_package";
const char* const c_field_display_name_Primitive = "field_package_option_primitive";
const char* const c_field_display_name_String = "field_package_option_string";
const char* const c_field_display_name_Time = "field_package_option_time";
const char* const c_field_display_name_Use_Option = "field_package_option_use_option";
const char* const c_field_display_name_Value = "field_package_option_value";

const int c_num_fields = 19;

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
   "302820",
   "302825",
   "302830"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Actions",
   "Date",
   "Datetime",
   "Id",
   "Installed",
   "Integer",
   "Is_Other_Package",
   "Model",
   "Name",
   "Numeric",
   "Other_Package",
   "Other_Package_Required",
   "Other_Package_Type",
   "Package",
   "Primitive",
   "String",
   "Time",
   "Use_Option",
   "Value"
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
   "137107",
   "137116"
};

const char* const c_transient_sorted_field_names[ ] =
{
   "Actions",
   "Value"
};

inline bool transient_compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool is_transient_field( const string& field )
{
   return binary_search( c_transient_sorted_field_ids,
    c_transient_sorted_field_ids + c_num_transient_fields, field.c_str( ), transient_compare )
    || binary_search( c_transient_sorted_field_names,
    c_transient_sorted_field_names + c_num_transient_fields, field.c_str( ), transient_compare );
}

const char* const c_procedure_id_Clear = "137430";
const char* const c_procedure_id_Edit = "137410";
const char* const c_procedure_id_Select = "137420";

const uint64_t c_modifier_Is_Bool = UINT64_C( 0x100 );
const uint64_t c_modifier_Is_Date = UINT64_C( 0x200 );
const uint64_t c_modifier_Is_Datetime = UINT64_C( 0x400 );
const uint64_t c_modifier_Is_Int = UINT64_C( 0x800 );
const uint64_t c_modifier_Is_Not_Other_Package_Option = UINT64_C( 0x1000 );
const uint64_t c_modifier_Is_Numeric = UINT64_C( 0x2000 );
const uint64_t c_modifier_Is_Other_Package_Option = UINT64_C( 0x4000 );
const uint64_t c_modifier_Is_String = UINT64_C( 0x8000 );
const uint64_t c_modifier_Is_Time = UINT64_C( 0x10000 );

domain_string_max_size< 100 > g_Id_domain;
domain_string_max_size< 100 > g_Name_domain;

string g_order_field_name;

set< string > g_derivations;

set< string > g_file_field_names;

typedef map< string, Meta_Package_Option* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Package_Option* > external_aliases_lookup_container;
typedef external_aliases_lookup_container::const_iterator external_aliases_lookup_const_iterator;

external_aliases_container g_external_aliases;
external_aliases_lookup_container g_external_aliases_lookup;

string gv_default_Actions = string( "137410$137100*@id" );
string gv_default_Date = string( );
string gv_default_Datetime = string( );
string gv_default_Id = string( );
bool gv_default_Installed = bool( 0 );
int gv_default_Integer = int( 0 );
bool gv_default_Is_Other_Package = bool( 0 );
string gv_default_Model = string( );
string gv_default_Name = string( );
string gv_default_Numeric = string( );
string gv_default_Other_Package = string( );
bool gv_default_Other_Package_Required = bool( 0 );
string gv_default_Other_Package_Type = string( );
string gv_default_Package = string( );
int gv_default_Primitive = int( 6 );
string gv_default_String = string( );
string gv_default_Time = string( );
bool gv_default_Use_Option = bool( 0 );
string gv_default_Value = string( );

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

   void handle_unknown_command( const string& command )
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
      bool want_fixed( has_parm_val( parameters, c_cmd_parm_Meta_Package_Option_key_fixed ) );

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
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Package_Option_get_field_name ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );
      else if( field_name == c_field_id_Actions || field_name == c_field_name_Actions )
         string_getter< string >( cmd_handler.p_Meta_Package_Option->Actions( ), cmd_handler.retval );
      else if( field_name == c_field_id_Date || field_name == c_field_name_Date )
         string_getter< string >( cmd_handler.p_Meta_Package_Option->Date( ), cmd_handler.retval );
      else if( field_name == c_field_id_Datetime || field_name == c_field_name_Datetime )
         string_getter< string >( cmd_handler.p_Meta_Package_Option->Datetime( ), cmd_handler.retval );
      else if( field_name == c_field_id_Id || field_name == c_field_name_Id )
         string_getter< string >( cmd_handler.p_Meta_Package_Option->Id( ), cmd_handler.retval );
      else if( field_name == c_field_id_Installed || field_name == c_field_name_Installed )
         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Installed( ), cmd_handler.retval );
      else if( field_name == c_field_id_Integer || field_name == c_field_name_Integer )
         string_getter< int >( cmd_handler.p_Meta_Package_Option->Integer( ), cmd_handler.retval );
      else if( field_name == c_field_id_Is_Other_Package || field_name == c_field_name_Is_Other_Package )
         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Is_Other_Package( ), cmd_handler.retval );
      else if( field_name == c_field_id_Model || field_name == c_field_name_Model )
         string_getter< Meta_Model >( cmd_handler.p_Meta_Package_Option->Model( ), cmd_handler.retval );
      else if( field_name == c_field_id_Name || field_name == c_field_name_Name )
         string_getter< string >( cmd_handler.p_Meta_Package_Option->Name( ), cmd_handler.retval );
      else if( field_name == c_field_id_Numeric || field_name == c_field_name_Numeric )
         string_getter< string >( cmd_handler.p_Meta_Package_Option->Numeric( ), cmd_handler.retval );
      else if( field_name == c_field_id_Other_Package || field_name == c_field_name_Other_Package )
         string_getter< Meta_Package >( cmd_handler.p_Meta_Package_Option->Other_Package( ), cmd_handler.retval );
      else if( field_name == c_field_id_Other_Package_Required || field_name == c_field_name_Other_Package_Required )
         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Other_Package_Required( ), cmd_handler.retval );
      else if( field_name == c_field_id_Other_Package_Type || field_name == c_field_name_Other_Package_Type )
         string_getter< string >( cmd_handler.p_Meta_Package_Option->Other_Package_Type( ), cmd_handler.retval );
      else if( field_name == c_field_id_Package || field_name == c_field_name_Package )
         string_getter< Meta_Package >( cmd_handler.p_Meta_Package_Option->Package( ), cmd_handler.retval );
      else if( field_name == c_field_id_Primitive || field_name == c_field_name_Primitive )
         string_getter< int >( cmd_handler.p_Meta_Package_Option->Primitive( ), cmd_handler.retval );
      else if( field_name == c_field_id_String || field_name == c_field_name_String )
         string_getter< string >( cmd_handler.p_Meta_Package_Option->String( ), cmd_handler.retval );
      else if( field_name == c_field_id_Time || field_name == c_field_name_Time )
         string_getter< string >( cmd_handler.p_Meta_Package_Option->Time( ), cmd_handler.retval );
      else if( field_name == c_field_id_Use_Option || field_name == c_field_name_Use_Option )
         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Use_Option( ), cmd_handler.retval );
      else if( field_name == c_field_id_Value || field_name == c_field_name_Value )
         string_getter< string >( cmd_handler.p_Meta_Package_Option->Value( ), cmd_handler.retval );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Package_Option_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Package_Option_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_parm_Meta_Package_Option_set_field_value ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );
      else if( field_name == c_field_id_Actions || field_name == c_field_name_Actions )
         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Actions, field_value );
      else if( field_name == c_field_id_Date || field_name == c_field_name_Date )
         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Date, field_value );
      else if( field_name == c_field_id_Datetime || field_name == c_field_name_Datetime )
         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Datetime, field_value );
      else if( field_name == c_field_id_Id || field_name == c_field_name_Id )
         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Id, field_value );
      else if( field_name == c_field_id_Installed || field_name == c_field_name_Installed )
         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Installed, field_value );
      else if( field_name == c_field_id_Integer || field_name == c_field_name_Integer )
         func_string_setter< Meta_Package_Option, int >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Integer, field_value );
      else if( field_name == c_field_id_Is_Other_Package || field_name == c_field_name_Is_Other_Package )
         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Is_Other_Package, field_value );
      else if( field_name == c_field_id_Model || field_name == c_field_name_Model )
         func_string_setter< Meta_Package_Option, Meta_Model >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Model, field_value );
      else if( field_name == c_field_id_Name || field_name == c_field_name_Name )
         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Name, field_value );
      else if( field_name == c_field_id_Numeric || field_name == c_field_name_Numeric )
         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Numeric, field_value );
      else if( field_name == c_field_id_Other_Package || field_name == c_field_name_Other_Package )
         func_string_setter< Meta_Package_Option, Meta_Package >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Other_Package, field_value );
      else if( field_name == c_field_id_Other_Package_Required || field_name == c_field_name_Other_Package_Required )
         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Other_Package_Required, field_value );
      else if( field_name == c_field_id_Other_Package_Type || field_name == c_field_name_Other_Package_Type )
         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Other_Package_Type, field_value );
      else if( field_name == c_field_id_Package || field_name == c_field_name_Package )
         func_string_setter< Meta_Package_Option, Meta_Package >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Package, field_value );
      else if( field_name == c_field_id_Primitive || field_name == c_field_name_Primitive )
         func_string_setter< Meta_Package_Option, int >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Primitive, field_value );
      else if( field_name == c_field_id_String || field_name == c_field_name_String )
         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::String, field_value );
      else if( field_name == c_field_id_Time || field_name == c_field_name_Time )
         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Time, field_value );
      else if( field_name == c_field_id_Use_Option || field_name == c_field_name_Use_Option )
         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Use_Option, field_value );
      else if( field_name == c_field_id_Value || field_name == c_field_name_Value )
         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Value, field_value );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Package_Option_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Package_Option_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_parm_Meta_Package_Option_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( field_name == c_field_id_Model || field_name == c_field_name_Model )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->Model( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Other_Package || field_name == c_field_name_Other_Package )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->Other_Package( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Package || field_name == c_field_name_Package )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->Package( ).execute( cmd_and_args );
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
   void impl_Actions( const string& Actions ) { v_Actions = Actions; }

   const string& impl_Date( ) const { return lazy_fetch( p_obj ), v_Date; }
   void impl_Date( const string& Date ) { v_Date = Date; }

   const string& impl_Datetime( ) const { return lazy_fetch( p_obj ), v_Datetime; }
   void impl_Datetime( const string& Datetime ) { v_Datetime = Datetime; }

   const string& impl_Id( ) const { return lazy_fetch( p_obj ), v_Id; }
   void impl_Id( const string& Id ) { v_Id = Id; }

   bool impl_Installed( ) const { return lazy_fetch( p_obj ), v_Installed; }
   void impl_Installed( bool Installed ) { v_Installed = Installed; }

   int impl_Integer( ) const { return lazy_fetch( p_obj ), v_Integer; }
   void impl_Integer( int Integer ) { v_Integer = Integer; }

   bool impl_Is_Other_Package( ) const { return lazy_fetch( p_obj ), v_Is_Other_Package; }
   void impl_Is_Other_Package( bool Is_Other_Package ) { v_Is_Other_Package = Is_Other_Package; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { v_Name = Name; }

   const string& impl_Numeric( ) const { return lazy_fetch( p_obj ), v_Numeric; }
   void impl_Numeric( const string& Numeric ) { v_Numeric = Numeric; }

   bool impl_Other_Package_Required( ) const { return lazy_fetch( p_obj ), v_Other_Package_Required; }
   void impl_Other_Package_Required( bool Other_Package_Required ) { v_Other_Package_Required = Other_Package_Required; }

   const string& impl_Other_Package_Type( ) const { return lazy_fetch( p_obj ), v_Other_Package_Type; }
   void impl_Other_Package_Type( const string& Other_Package_Type ) { v_Other_Package_Type = Other_Package_Type; }

   int impl_Primitive( ) const { return lazy_fetch( p_obj ), v_Primitive; }
   void impl_Primitive( int Primitive ) { v_Primitive = Primitive; }

   const string& impl_String( ) const { return lazy_fetch( p_obj ), v_String; }
   void impl_String( const string& String ) { v_String = String; }

   const string& impl_Time( ) const { return lazy_fetch( p_obj ), v_Time; }
   void impl_Time( const string& Time ) { v_Time = Time; }

   bool impl_Use_Option( ) const { return lazy_fetch( p_obj ), v_Use_Option; }
   void impl_Use_Option( bool Use_Option ) { v_Use_Option = Use_Option; }

   const string& impl_Value( ) const { return lazy_fetch( p_obj ), v_Value; }
   void impl_Value( const string& Value ) { v_Value = Value; }

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

   void impl_Clear( );

   void impl_Edit( );

   void impl_Select( );

   string get_field_value( int field ) const;
   void set_field_value( int field, const string& value );

   uint64_t get_state( ) const;

   const string& execute( const string& cmd_and_args );

   void clear_foreign_key( const string& field );

   void set_foreign_key_value( const string& field, const string& value );

   const string& get_foreign_key_value( const string& field );

   void get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const;

   void add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const;
   void add_extra_paging_info( vector< pair< string, string > >& paging_info ) const;

   void clear( );

   bool value_will_be_provided( const string& field_name );

   void validate( unsigned state, bool is_internal, validation_error_container* p_validation_errors );
   void validate_set_fields( set< string >& fields_set, validation_error_container* p_validation_errors );

   void after_fetch( );
   void finalise_fetch( );

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
   string v_Id;
   bool v_Installed;
   int v_Integer;
   bool v_Is_Other_Package;
   string v_Name;
   string v_Numeric;
   bool v_Other_Package_Required;
   string v_Other_Package_Type;
   int v_Primitive;
   string v_String;
   string v_Time;
   bool v_Use_Option;
   string v_Value;

   string v_Model;
   mutable class_pointer< Meta_Model > cp_Model;

   string v_Other_Package;
   mutable class_pointer< Meta_Package > cp_Other_Package;

   string v_Package;
   mutable class_pointer< Meta_Package > cp_Package;
};

void Meta_Package_Option::impl::impl_Clear( )
{
   // [<start Clear_impl>]
//nyi
   get_obj( ).op_update( );
   get_obj( ).Use_Option( false );
   get_obj( ).op_apply( );
   // [<finish Clear_impl>]
}

void Meta_Package_Option::impl::impl_Edit( )
{
   // [<start Edit_impl>]
   // [<finish Edit_impl>]
}

void Meta_Package_Option::impl::impl_Select( )
{
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
      retval = to_string( impl_Date( ) );
      break;

      case 2:
      retval = to_string( impl_Datetime( ) );
      break;

      case 3:
      retval = to_string( impl_Id( ) );
      break;

      case 4:
      retval = to_string( impl_Installed( ) );
      break;

      case 5:
      retval = to_string( impl_Integer( ) );
      break;

      case 6:
      retval = to_string( impl_Is_Other_Package( ) );
      break;

      case 7:
      retval = to_string( impl_Model( ) );
      break;

      case 8:
      retval = to_string( impl_Name( ) );
      break;

      case 9:
      retval = to_string( impl_Numeric( ) );
      break;

      case 10:
      retval = to_string( impl_Other_Package( ) );
      break;

      case 11:
      retval = to_string( impl_Other_Package_Required( ) );
      break;

      case 12:
      retval = to_string( impl_Other_Package_Type( ) );
      break;

      case 13:
      retval = to_string( impl_Package( ) );
      break;

      case 14:
      retval = to_string( impl_Primitive( ) );
      break;

      case 15:
      retval = to_string( impl_String( ) );
      break;

      case 16:
      retval = to_string( impl_Time( ) );
      break;

      case 17:
      retval = to_string( impl_Use_Option( ) );
      break;

      case 18:
      retval = to_string( impl_Value( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
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
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Date, value );
      break;

      case 2:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Datetime, value );
      break;

      case 3:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Id, value );
      break;

      case 4:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Installed, value );
      break;

      case 5:
      func_string_setter< Meta_Package_Option::impl, int >( *this, &Meta_Package_Option::impl::impl_Integer, value );
      break;

      case 6:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Is_Other_Package, value );
      break;

      case 7:
      func_string_setter< Meta_Package_Option::impl, Meta_Model >( *this, &Meta_Package_Option::impl::impl_Model, value );
      break;

      case 8:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Name, value );
      break;

      case 9:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Numeric, value );
      break;

      case 10:
      func_string_setter< Meta_Package_Option::impl, Meta_Package >( *this, &Meta_Package_Option::impl::impl_Other_Package, value );
      break;

      case 11:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Other_Package_Required, value );
      break;

      case 12:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Other_Package_Type, value );
      break;

      case 13:
      func_string_setter< Meta_Package_Option::impl, Meta_Package >( *this, &Meta_Package_Option::impl::impl_Package, value );
      break;

      case 14:
      func_string_setter< Meta_Package_Option::impl, int >( *this, &Meta_Package_Option::impl::impl_Primitive, value );
      break;

      case 15:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_String, value );
      break;

      case 16:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Time, value );
      break;

      case 17:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Use_Option, value );
      break;

      case 18:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Value, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }
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

   // [<start get_state>]
//nyi
   if( get_obj( ).Package( ).Actions( ).empty( ) )
      state |= ( c_state_uneditable | c_state_undeletable );
   // [<finish get_state>]

   return state;
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
   else if( field == c_field_id_Model || field == c_field_name_Model )
      impl_Model( "" );
   else if( field == c_field_id_Other_Package || field == c_field_name_Other_Package )
      impl_Other_Package( "" );
   else if( field == c_field_id_Package || field == c_field_name_Package )
      impl_Package( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Package_Option::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( field == c_field_id_Model || field == c_field_name_Model )
      v_Model = value;
   else if( field == c_field_id_Other_Package || field == c_field_name_Other_Package )
      v_Other_Package = value;
   else if( field == c_field_id_Package || field == c_field_name_Package )
      v_Package = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Package_Option::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Model || field == c_field_name_Model )
      return v_Model;
   else if( field == c_field_id_Other_Package || field == c_field_name_Other_Package )
      return v_Other_Package;
   else if( field == c_field_id_Package || field == c_field_name_Package )
      return v_Package;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Package_Option::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Model, v_Model ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Other_Package, v_Other_Package ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Package, v_Package ) );
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
   v_Actions = gv_default_Actions;
   v_Date = gv_default_Date;
   v_Datetime = gv_default_Datetime;
   v_Id = gv_default_Id;
   v_Installed = gv_default_Installed;
   v_Integer = gv_default_Integer;
   v_Is_Other_Package = gv_default_Is_Other_Package;
   v_Name = gv_default_Name;
   v_Numeric = gv_default_Numeric;
   v_Other_Package_Required = gv_default_Other_Package_Required;
   v_Other_Package_Type = gv_default_Other_Package_Type;
   v_Primitive = gv_default_Primitive;
   v_String = gv_default_String;
   v_Time = gv_default_Time;
   v_Use_Option = gv_default_Use_Option;
   v_Value = gv_default_Value;

   v_Model = string( );
   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   v_Other_Package = string( );
   if( cp_Other_Package )
      p_obj->setup_foreign_key( *cp_Other_Package, v_Other_Package );

   v_Package = string( );
   if( cp_Package )
      p_obj->setup_foreign_key( *cp_Package, v_Package );
}

bool Meta_Package_Option::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Package_Option::impl::validate( unsigned state, bool is_internal, validation_error_container* p_validation_errors )
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

   if( v_Model.empty( ) && !value_will_be_provided( c_field_name_Model ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Model,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Model ) ) ) ) );

   if( v_Package.empty( ) && !value_will_be_provided( c_field_name_Package ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Package,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Package ) ) ) ) );

   string error_message;
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

   if( !g_primitive_enum.count( v_Primitive ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Primitive,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Primitive ) ) ) ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Package_Option::impl::validate_set_fields( set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;

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
}

void Meta_Package_Option::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   if( cp_Other_Package )
      p_obj->setup_foreign_key( *cp_Other_Package, v_Other_Package );

   if( cp_Package )
      p_obj->setup_foreign_key( *cp_Package, v_Package );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_fetch>]
//nyi
   if( get_state( ) & c_state_uneditable || get_obj( ).Package( ).Installed( ) )
      get_obj( ).Actions( "" );
   else if( !get_obj( ).Is_Other_Package( ) && get_obj( ).Primitive( ) == e_primitive_bool )
   {
      if( get_obj( ).Use_Option( ) )
         get_obj( ).Actions( c_procedure_id_Clear );
      else
         get_obj( ).Actions( c_procedure_id_Select );
   }

   if( !get_obj( ).Is_Other_Package( ) )
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
   else
      get_obj( ).Value( get_obj( ).Other_Package( ).Name( ) );
   // [<finish after_fetch>]
}

void Meta_Package_Option::impl::finalise_fetch( )
{
   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Package_Option::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Package_Option::impl::post_init( )
{
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
   if( is_create
    && get_obj( ).get_clone_key( ).empty( )
    && get_obj( ).Model( ) == gv_default_Model )
      get_obj( ).Model( get_obj( ).Package( ).Model( ) );
   // [(finish default_to_field)] 600881

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Package_Option::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

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

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Package_Option::impl::can_destroy( bool is_internal )
{
   bool retval = is_internal || !( get_state( ) & c_state_undeletable );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Package_Option::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Package_Option::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Package_Option::impl::set_default_values( )
{
   clear( );
}

bool Meta_Package_Option::impl::is_filtered( ) const
{
   // [<start is_filtered>]
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

bool Meta_Package_Option::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
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

void Meta_Package_Option::validate( unsigned state, bool is_internal )
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

void Meta_Package_Option::finalise_fetch( )
{
   p_impl->finalise_fetch( );
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
   else if( name == c_field_name_Is_Other_Package )
   {
      p_id = c_field_id_Is_Other_Package;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Model )
   {
      p_id = c_field_id_Model;

      if( p_type_name )
         *p_type_name = "Meta_Model";

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
   else if( name == c_field_name_Numeric )
   {
      p_id = c_field_id_Numeric;

      if( p_type_name )
         *p_type_name = "string";

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
   else if( id == c_field_id_Is_Other_Package )
   {
      p_name = c_field_name_Is_Other_Package;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Model )
   {
      p_name = c_field_name_Model;

      if( p_type_name )
         *p_type_name = "Meta_Model";

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
   else if( id == c_field_id_Numeric )
   {
      p_name = c_field_name_Numeric;

      if( p_type_name )
         *p_type_name = "string";

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

   return p_name;
}

string& Meta_Package_Option::get_order_field_name( ) const
{
   return g_order_field_name;
}

bool Meta_Package_Option::is_file_field_name( const string& name ) const
{
   return g_file_field_names.count( name );
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
   else if( id_or_name == c_field_id_Actions || id_or_name == c_field_name_Actions )
   {
      name = string( c_field_display_name_Actions );
      get_module_string( c_field_display_name_Actions, &next );
   }
   else if( id_or_name == c_field_id_Date || id_or_name == c_field_name_Date )
   {
      name = string( c_field_display_name_Date );
      get_module_string( c_field_display_name_Date, &next );
   }
   else if( id_or_name == c_field_id_Datetime || id_or_name == c_field_name_Datetime )
   {
      name = string( c_field_display_name_Datetime );
      get_module_string( c_field_display_name_Datetime, &next );
   }
   else if( id_or_name == c_field_id_Id || id_or_name == c_field_name_Id )
   {
      name = string( c_field_display_name_Id );
      get_module_string( c_field_display_name_Id, &next );
   }
   else if( id_or_name == c_field_id_Installed || id_or_name == c_field_name_Installed )
   {
      name = string( c_field_display_name_Installed );
      get_module_string( c_field_display_name_Installed, &next );
   }
   else if( id_or_name == c_field_id_Integer || id_or_name == c_field_name_Integer )
   {
      name = string( c_field_display_name_Integer );
      get_module_string( c_field_display_name_Integer, &next );
   }
   else if( id_or_name == c_field_id_Is_Other_Package || id_or_name == c_field_name_Is_Other_Package )
   {
      name = string( c_field_display_name_Is_Other_Package );
      get_module_string( c_field_display_name_Is_Other_Package, &next );
   }
   else if( id_or_name == c_field_id_Model || id_or_name == c_field_name_Model )
   {
      name = string( c_field_display_name_Model );
      get_module_string( c_field_display_name_Model, &next );
   }
   else if( id_or_name == c_field_id_Name || id_or_name == c_field_name_Name )
   {
      name = string( c_field_display_name_Name );
      get_module_string( c_field_display_name_Name, &next );
   }
   else if( id_or_name == c_field_id_Numeric || id_or_name == c_field_name_Numeric )
   {
      name = string( c_field_display_name_Numeric );
      get_module_string( c_field_display_name_Numeric, &next );
   }
   else if( id_or_name == c_field_id_Other_Package || id_or_name == c_field_name_Other_Package )
   {
      name = string( c_field_display_name_Other_Package );
      get_module_string( c_field_display_name_Other_Package, &next );
   }
   else if( id_or_name == c_field_id_Other_Package_Required || id_or_name == c_field_name_Other_Package_Required )
   {
      name = string( c_field_display_name_Other_Package_Required );
      get_module_string( c_field_display_name_Other_Package_Required, &next );
   }
   else if( id_or_name == c_field_id_Other_Package_Type || id_or_name == c_field_name_Other_Package_Type )
   {
      name = string( c_field_display_name_Other_Package_Type );
      get_module_string( c_field_display_name_Other_Package_Type, &next );
   }
   else if( id_or_name == c_field_id_Package || id_or_name == c_field_name_Package )
   {
      name = string( c_field_display_name_Package );
      get_module_string( c_field_display_name_Package, &next );
   }
   else if( id_or_name == c_field_id_Primitive || id_or_name == c_field_name_Primitive )
   {
      name = string( c_field_display_name_Primitive );
      get_module_string( c_field_display_name_Primitive, &next );
   }
   else if( id_or_name == c_field_id_String || id_or_name == c_field_name_String )
   {
      name = string( c_field_display_name_String );
      get_module_string( c_field_display_name_String, &next );
   }
   else if( id_or_name == c_field_id_Time || id_or_name == c_field_name_Time )
   {
      name = string( c_field_display_name_Time );
      get_module_string( c_field_display_name_Time, &next );
   }
   else if( id_or_name == c_field_id_Use_Option || id_or_name == c_field_name_Use_Option )
   {
      name = string( c_field_display_name_Use_Option );
      get_module_string( c_field_display_name_Use_Option, &next );
   }
   else if( id_or_name == c_field_id_Value || id_or_name == c_field_name_Value )
   {
      name = string( c_field_display_name_Value );
      get_module_string( c_field_display_name_Value, &next );
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
   else if( id_or_name == c_field_id_Actions || id_or_name == c_field_name_Actions )
      display_name = get_module_string( c_field_display_name_Actions );
   else if( id_or_name == c_field_id_Date || id_or_name == c_field_name_Date )
      display_name = get_module_string( c_field_display_name_Date );
   else if( id_or_name == c_field_id_Datetime || id_or_name == c_field_name_Datetime )
      display_name = get_module_string( c_field_display_name_Datetime );
   else if( id_or_name == c_field_id_Id || id_or_name == c_field_name_Id )
      display_name = get_module_string( c_field_display_name_Id );
   else if( id_or_name == c_field_id_Installed || id_or_name == c_field_name_Installed )
      display_name = get_module_string( c_field_display_name_Installed );
   else if( id_or_name == c_field_id_Integer || id_or_name == c_field_name_Integer )
      display_name = get_module_string( c_field_display_name_Integer );
   else if( id_or_name == c_field_id_Is_Other_Package || id_or_name == c_field_name_Is_Other_Package )
      display_name = get_module_string( c_field_display_name_Is_Other_Package );
   else if( id_or_name == c_field_id_Model || id_or_name == c_field_name_Model )
      display_name = get_module_string( c_field_display_name_Model );
   else if( id_or_name == c_field_id_Name || id_or_name == c_field_name_Name )
      display_name = get_module_string( c_field_display_name_Name );
   else if( id_or_name == c_field_id_Numeric || id_or_name == c_field_name_Numeric )
      display_name = get_module_string( c_field_display_name_Numeric );
   else if( id_or_name == c_field_id_Other_Package || id_or_name == c_field_name_Other_Package )
      display_name = get_module_string( c_field_display_name_Other_Package );
   else if( id_or_name == c_field_id_Other_Package_Required || id_or_name == c_field_name_Other_Package_Required )
      display_name = get_module_string( c_field_display_name_Other_Package_Required );
   else if( id_or_name == c_field_id_Other_Package_Type || id_or_name == c_field_name_Other_Package_Type )
      display_name = get_module_string( c_field_display_name_Other_Package_Type );
   else if( id_or_name == c_field_id_Package || id_or_name == c_field_name_Package )
      display_name = get_module_string( c_field_display_name_Package );
   else if( id_or_name == c_field_id_Primitive || id_or_name == c_field_name_Primitive )
      display_name = get_module_string( c_field_display_name_Primitive );
   else if( id_or_name == c_field_id_String || id_or_name == c_field_name_String )
      display_name = get_module_string( c_field_display_name_String );
   else if( id_or_name == c_field_id_Time || id_or_name == c_field_name_Time )
      display_name = get_module_string( c_field_display_name_Time );
   else if( id_or_name == c_field_id_Use_Option || id_or_name == c_field_name_Use_Option )
      display_name = get_module_string( c_field_display_name_Use_Option );
   else if( id_or_name == c_field_id_Value || id_or_name == c_field_name_Value )
      display_name = get_module_string( c_field_display_name_Value );

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

void Meta_Package_Option::setup_foreign_key( Meta_Model& o, const string& value )
{
   static_cast< Meta_Model& >( o ).set_key( value );
}

void Meta_Package_Option::setup_foreign_key( Meta_Package& o, const string& value )
{
   static_cast< Meta_Package& >( o ).set_key( value );
}

void Meta_Package_Option::setup_graph_parent(
 Meta_Model& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Model& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Model& >( o ).set_key( init_value );
}

void Meta_Package_Option::setup_graph_parent(
 Meta_Package& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Package& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Package& >( o ).set_key( init_value );
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
   else if( sub_context == c_field_id_Model || sub_context == c_field_name_Model )
      p_class_base = &Model( );
   else if( sub_context == c_field_id_Other_Package || sub_context == c_field_name_Other_Package )
      p_class_base = &Other_Package( );
   else if( sub_context == c_field_id_Package || sub_context == c_field_name_Package )
      p_class_base = &Package( );

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

   names.push_back( "C_Date" );
   names.push_back( "C_Datetime" );
   names.push_back( "C_Id" );
   names.push_back( "C_Installed" );
   names.push_back( "C_Integer" );
   names.push_back( "C_Is_Other_Package" );
   names.push_back( "C_Model" );
   names.push_back( "C_Name" );
   names.push_back( "C_Numeric" );
   names.push_back( "C_Other_Package" );
   names.push_back( "C_Other_Package_Required" );
   names.push_back( "C_Other_Package_Type" );
   names.push_back( "C_Package" );
   names.push_back( "C_Primitive" );
   names.push_back( "C_String" );
   names.push_back( "C_Time" );
   names.push_back( "C_Use_Option" );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Package_Option::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Date( ) ) ) );
   values.push_back( sql_quote( to_string( Datetime( ) ) ) );
   values.push_back( sql_quote( to_string( Id( ) ) ) );
   values.push_back( to_string( Installed( ) ) );
   values.push_back( to_string( Integer( ) ) );
   values.push_back( to_string( Is_Other_Package( ) ) );
   values.push_back( sql_quote( to_string( Model( ) ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( sql_quote( to_string( Numeric( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Package( ) ) ) );
   values.push_back( to_string( Other_Package_Required( ) ) );
   values.push_back( sql_quote( to_string( Other_Package_Type( ) ) ) );
   values.push_back( sql_quote( to_string( Package( ) ) ) );
   values.push_back( to_string( Primitive( ) ) );
   values.push_back( sql_quote( to_string( String( ) ) ) );
   values.push_back( sql_quote( to_string( Time( ) ) ) );
   values.push_back( to_string( Use_Option( ) ) );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
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

void Meta_Package_Option::do_generate_sql(
 generate_sql_type type, vector< string >& sql_stmts, set< string >& tx_key_info ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info );
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
   all_field_info.push_back( field_info( "137116", "Actions", "string", false ) );
   all_field_info.push_back( field_info( "137111", "Date", "string", false ) );
   all_field_info.push_back( field_info( "137110", "Datetime", "string", false ) );
   all_field_info.push_back( field_info( "137101", "Id", "string", false ) );
   all_field_info.push_back( field_info( "137103", "Installed", "bool", false ) );
   all_field_info.push_back( field_info( "137114", "Integer", "int", false ) );
   all_field_info.push_back( field_info( "137104", "Is_Other_Package", "bool", false ) );
   all_field_info.push_back( field_info( "302825", "Model", "Meta_Model", true ) );
   all_field_info.push_back( field_info( "137115", "Name", "string", false ) );
   all_field_info.push_back( field_info( "137113", "Numeric", "string", false ) );
   all_field_info.push_back( field_info( "302830", "Other_Package", "Meta_Package", false ) );
   all_field_info.push_back( field_info( "137105", "Other_Package_Required", "bool", false ) );
   all_field_info.push_back( field_info( "137106", "Other_Package_Type", "string", false ) );
   all_field_info.push_back( field_info( "302820", "Package", "Meta_Package", true ) );
   all_field_info.push_back( field_info( "137108", "Primitive", "int", false ) );
   all_field_info.push_back( field_info( "137109", "String", "string", false ) );
   all_field_info.push_back( field_info( "137112", "Time", "string", false ) );
   all_field_info.push_back( field_info( "137102", "Use_Option", "bool", false ) );
   all_field_info.push_back( field_info( "137107", "Value", "string", false ) );
}

void Meta_Package_Option::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Model, make_pair( "Meta.137100", "Meta_Model" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Other_Package, make_pair( "Meta.137100", "Meta_Package" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Package, make_pair( "Meta.137100", "Meta_Package" ) ) );
}

int Meta_Package_Option::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;

   return c_num_fields;
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
      p_id = "137111";
      break;

      case 3:
      p_id = "137110";
      break;

      case 4:
      p_id = "137101";
      break;

      case 5:
      p_id = "137103";
      break;

      case 6:
      p_id = "137114";
      break;

      case 7:
      p_id = "137104";
      break;

      case 8:
      p_id = "302825";
      break;

      case 9:
      p_id = "137115";
      break;

      case 10:
      p_id = "137113";
      break;

      case 11:
      p_id = "302830";
      break;

      case 12:
      p_id = "137105";
      break;

      case 13:
      p_id = "137106";
      break;

      case 14:
      p_id = "302820";
      break;

      case 15:
      p_id = "137108";
      break;

      case 16:
      p_id = "137109";
      break;

      case 17:
      p_id = "137112";
      break;

      case 18:
      p_id = "137102";
      break;

      case 19:
      p_id = "137107";
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
      p_id = "Date";
      break;

      case 3:
      p_id = "Datetime";
      break;

      case 4:
      p_id = "Id";
      break;

      case 5:
      p_id = "Installed";
      break;

      case 6:
      p_id = "Integer";
      break;

      case 7:
      p_id = "Is_Other_Package";
      break;

      case 8:
      p_id = "Model";
      break;

      case 9:
      p_id = "Name";
      break;

      case 10:
      p_id = "Numeric";
      break;

      case 11:
      p_id = "Other_Package";
      break;

      case 12:
      p_id = "Other_Package_Required";
      break;

      case 13:
      p_id = "Other_Package_Type";
      break;

      case 14:
      p_id = "Package";
      break;

      case 15:
      p_id = "Primitive";
      break;

      case 16:
      p_id = "String";
      break;

      case 17:
      p_id = "Time";
      break;

      case 18:
      p_id = "Use_Option";
      break;

      case 19:
      p_id = "Value";
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
   else if( field == c_field_id_Actions || field == c_field_name_Actions )
      rc += 1;
   else if( field == c_field_id_Date || field == c_field_name_Date )
      rc += 2;
   else if( field == c_field_id_Datetime || field == c_field_name_Datetime )
      rc += 3;
   else if( field == c_field_id_Id || field == c_field_name_Id )
      rc += 4;
   else if( field == c_field_id_Installed || field == c_field_name_Installed )
      rc += 5;
   else if( field == c_field_id_Integer || field == c_field_name_Integer )
      rc += 6;
   else if( field == c_field_id_Is_Other_Package || field == c_field_name_Is_Other_Package )
      rc += 7;
   else if( field == c_field_id_Model || field == c_field_name_Model )
      rc += 8;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 9;
   else if( field == c_field_id_Numeric || field == c_field_name_Numeric )
      rc += 10;
   else if( field == c_field_id_Other_Package || field == c_field_name_Other_Package )
      rc += 11;
   else if( field == c_field_id_Other_Package_Required || field == c_field_name_Other_Package_Required )
      rc += 12;
   else if( field == c_field_id_Other_Package_Type || field == c_field_name_Other_Package_Type )
      rc += 13;
   else if( field == c_field_id_Package || field == c_field_name_Package )
      rc += 14;
   else if( field == c_field_id_Primitive || field == c_field_name_Primitive )
      rc += 15;
   else if( field == c_field_id_String || field == c_field_name_String )
      rc += 16;
   else if( field == c_field_id_Time || field == c_field_name_Time )
      rc += 17;
   else if( field == c_field_id_Use_Option || field == c_field_name_Use_Option )
      rc += 18;
   else if( field == c_field_id_Value || field == c_field_name_Value )
      rc += 19;

   return rc - 1;
}

procedure_info_container& Meta_Package_Option::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "137430", "Clear" ) );
      procedures.insert( make_pair( "137410", "Edit" ) );
      procedures.insert( make_pair( "137420", "Select" ) );
   }

   return procedures;
}

string Meta_Package_Option::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(64),"
    "C_Ver_ INTEGER NOT NULL,"
    "C_Rev_ INTEGER NOT NULL,"
    "C_Typ_ VARCHAR(24) NOT NULL,"
    "C_Date VARCHAR(200) NOT NULL,"
    "C_Datetime VARCHAR(200) NOT NULL,"
    "C_Id VARCHAR(200) NOT NULL,"
    "C_Installed INTEGER NOT NULL,"
    "C_Integer INTEGER NOT NULL,"
    "C_Is_Other_Package INTEGER NOT NULL,"
    "C_Model VARCHAR(64) NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Numeric VARCHAR(200) NOT NULL,"
    "C_Other_Package VARCHAR(64) NOT NULL,"
    "C_Other_Package_Required INTEGER NOT NULL,"
    "C_Other_Package_Type VARCHAR(200) NOT NULL,"
    "C_Package VARCHAR(64) NOT NULL,"
    "C_Primitive INTEGER NOT NULL,"
    "C_String VARCHAR(200) NOT NULL,"
    "C_Time VARCHAR(200) NOT NULL,"
    "C_Use_Option INTEGER NOT NULL,"
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

void Meta_Package_Option::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Package,C_Name" );
}

void Meta_Package_Option::static_get_sql_unique_indexes( vector< string >& indexes )
{
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

