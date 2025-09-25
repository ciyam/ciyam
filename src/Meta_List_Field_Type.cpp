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

#include "Meta_List_Field_Type.h"

#include "Meta_List_Field.h"

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

inline int system( const string& cmd ) { return exec_system( cmd ); }

namespace
{

template< typename T > inline void sanity_check( const T& t ) { }

inline void sanity_check( const string& s )
{
   if( s.length( ) > c_max_string_length_limit )
      throw runtime_error( "unexpected max string length limit exceeded with: " + s );
}

#include "Meta_List_Field_Type.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Allow_Child_Rel_Select_Specifics = "123113";
const char* const c_field_id_Allow_Exclude_In_Use_FK = "123106";
const char* const c_field_id_Allow_Link_Empty_Restriction = "123121";
const char* const c_field_id_Allow_Link_Permission = "123119";
const char* const c_field_id_Allow_Link_Specifics = "123112";
const char* const c_field_id_Allow_Link_Type = "123107";
const char* const c_field_id_Allow_Procedure = "123108";
const char* const c_field_id_Allow_Restriction_Field = "123116";
const char* const c_field_id_Allow_Restriction_Spec = "123103";
const char* const c_field_id_Allow_Restriction_Value = "123104";
const char* const c_field_id_Allow_Search_Option_Limit = "123115";
const char* const c_field_id_Allow_Select_Specifics = "123111";
const char* const c_field_id_Allow_Sort_Manually = "123122";
const char* const c_field_id_Allow_View_Parent_Extra = "123120";
const char* const c_field_id_Is_Restrict_Search = "123118";
const char* const c_field_id_List_Field_Name = "123102";
const char* const c_field_id_Name = "123101";
const char* const c_field_id_Needs_Restriction_Field = "123117";
const char* const c_field_id_Needs_Source = "123114";
const char* const c_field_id_Needs_Switch_Type = "123105";
const char* const c_field_id_Non_Simple_Field = "123109";
const char* const c_field_id_Trivial_Field_Only = "123110";

const char* const c_field_name_Allow_Child_Rel_Select_Specifics = "Allow_Child_Rel_Select_Specifics";
const char* const c_field_name_Allow_Exclude_In_Use_FK = "Allow_Exclude_In_Use_FK";
const char* const c_field_name_Allow_Link_Empty_Restriction = "Allow_Link_Empty_Restriction";
const char* const c_field_name_Allow_Link_Permission = "Allow_Link_Permission";
const char* const c_field_name_Allow_Link_Specifics = "Allow_Link_Specifics";
const char* const c_field_name_Allow_Link_Type = "Allow_Link_Type";
const char* const c_field_name_Allow_Procedure = "Allow_Procedure";
const char* const c_field_name_Allow_Restriction_Field = "Allow_Restriction_Field";
const char* const c_field_name_Allow_Restriction_Spec = "Allow_Restriction_Spec";
const char* const c_field_name_Allow_Restriction_Value = "Allow_Restriction_Value";
const char* const c_field_name_Allow_Search_Option_Limit = "Allow_Search_Option_Limit";
const char* const c_field_name_Allow_Select_Specifics = "Allow_Select_Specifics";
const char* const c_field_name_Allow_Sort_Manually = "Allow_Sort_Manually";
const char* const c_field_name_Allow_View_Parent_Extra = "Allow_View_Parent_Extra";
const char* const c_field_name_Is_Restrict_Search = "Is_Restrict_Search";
const char* const c_field_name_List_Field_Name = "List_Field_Name";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Needs_Restriction_Field = "Needs_Restriction_Field";
const char* const c_field_name_Needs_Source = "Needs_Source";
const char* const c_field_name_Needs_Switch_Type = "Needs_Switch_Type";
const char* const c_field_name_Non_Simple_Field = "Non_Simple_Field";
const char* const c_field_name_Trivial_Field_Only = "Trivial_Field_Only";

const char* const c_field_display_name_Allow_Child_Rel_Select_Specifics = "field_list_field_type_allow_child_rel_select_specifics";
const char* const c_field_display_name_Allow_Exclude_In_Use_FK = "field_list_field_type_allow_exclude_in_use_fk";
const char* const c_field_display_name_Allow_Link_Empty_Restriction = "field_list_field_type_allow_link_empty_restriction";
const char* const c_field_display_name_Allow_Link_Permission = "field_list_field_type_allow_link_permission";
const char* const c_field_display_name_Allow_Link_Specifics = "field_list_field_type_allow_link_specifics";
const char* const c_field_display_name_Allow_Link_Type = "field_list_field_type_allow_link_type";
const char* const c_field_display_name_Allow_Procedure = "field_list_field_type_allow_procedure";
const char* const c_field_display_name_Allow_Restriction_Field = "field_list_field_type_allow_restriction_field";
const char* const c_field_display_name_Allow_Restriction_Spec = "field_list_field_type_allow_restriction_spec";
const char* const c_field_display_name_Allow_Restriction_Value = "field_list_field_type_allow_restriction_value";
const char* const c_field_display_name_Allow_Search_Option_Limit = "field_list_field_type_allow_search_option_limit";
const char* const c_field_display_name_Allow_Select_Specifics = "field_list_field_type_allow_select_specifics";
const char* const c_field_display_name_Allow_Sort_Manually = "field_list_field_type_allow_sort_manually";
const char* const c_field_display_name_Allow_View_Parent_Extra = "field_list_field_type_allow_view_parent_extra";
const char* const c_field_display_name_Is_Restrict_Search = "field_list_field_type_is_restrict_search";
const char* const c_field_display_name_List_Field_Name = "field_list_field_type_list_field_name";
const char* const c_field_display_name_Name = "field_list_field_type_name";
const char* const c_field_display_name_Needs_Restriction_Field = "field_list_field_type_needs_restriction_field";
const char* const c_field_display_name_Needs_Source = "field_list_field_type_needs_source";
const char* const c_field_display_name_Needs_Switch_Type = "field_list_field_type_needs_switch_type";
const char* const c_field_display_name_Non_Simple_Field = "field_list_field_type_non_simple_field";
const char* const c_field_display_name_Trivial_Field_Only = "field_list_field_type_trivial_field_only";

const int c_num_fields = 22;

const char* const c_all_sorted_field_ids[ ] =
{
   "123101",
   "123102",
   "123103",
   "123104",
   "123105",
   "123106",
   "123107",
   "123108",
   "123109",
   "123110",
   "123111",
   "123112",
   "123113",
   "123114",
   "123115",
   "123116",
   "123117",
   "123118",
   "123119",
   "123120",
   "123121",
   "123122"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Allow_Child_Rel_Select_Specifics",
   "Allow_Exclude_In_Use_FK",
   "Allow_Link_Empty_Restriction",
   "Allow_Link_Permission",
   "Allow_Link_Specifics",
   "Allow_Link_Type",
   "Allow_Procedure",
   "Allow_Restriction_Field",
   "Allow_Restriction_Spec",
   "Allow_Restriction_Value",
   "Allow_Search_Option_Limit",
   "Allow_Select_Specifics",
   "Allow_Sort_Manually",
   "Allow_View_Parent_Extra",
   "Is_Restrict_Search",
   "List_Field_Name",
   "Name",
   "Needs_Restriction_Field",
   "Needs_Source",
   "Needs_Switch_Type",
   "Non_Simple_Field",
   "Trivial_Field_Only"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool has_field( const string& field )
{
   return binary_search( c_all_sorted_field_ids, c_all_sorted_field_ids + c_num_fields, field.c_str( ), compare )
    || binary_search( c_all_sorted_field_names, c_all_sorted_field_names + c_num_fields, field.c_str( ), compare );
}

const int c_num_encrypted_fields = 0;

bool is_encrypted_field( const string& ) { static bool false_value( false ); return false_value; }

const int c_num_transient_fields = 0;

bool is_transient_field( const string& ) { static bool false_value( false ); return false_value; }

domain_string_max_size< 100 > g_List_Field_Name_domain;
domain_string_max_size< 30 > g_Name_domain;

string g_group_field_name;
string g_level_field_name;
string g_order_field_name;
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_List_Field_Type* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_List_Field_Type* > external_aliases_lookup_container;
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

bool g_default_Allow_Child_Rel_Select_Specifics = bool( 0 );
bool g_default_Allow_Exclude_In_Use_FK = bool( 0 );
bool g_default_Allow_Link_Empty_Restriction = bool( 0 );
bool g_default_Allow_Link_Permission = bool( 0 );
bool g_default_Allow_Link_Specifics = bool( 0 );
bool g_default_Allow_Link_Type = bool( 0 );
bool g_default_Allow_Procedure = bool( 0 );
bool g_default_Allow_Restriction_Field = bool( 0 );
bool g_default_Allow_Restriction_Spec = bool( 0 );
bool g_default_Allow_Restriction_Value = bool( 0 );
bool g_default_Allow_Search_Option_Limit = bool( 0 );
bool g_default_Allow_Select_Specifics = bool( 0 );
bool g_default_Allow_Sort_Manually = bool( 0 );
bool g_default_Allow_View_Parent_Extra = bool( 0 );
bool g_default_Is_Restrict_Search = bool( 0 );
string g_default_List_Field_Name = string( );
string g_default_Name = string( );
bool g_default_Needs_Restriction_Field = bool( 0 );
bool g_default_Needs_Source = bool( 0 );
bool g_default_Needs_Switch_Type = bool( 0 );
bool g_default_Non_Simple_Field = bool( 0 );
bool g_default_Trivial_Field_Only = bool( 0 );

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_List_Field_Type > List_Field_Type_registration( get_class_registry( ), "123100" );

class Meta_List_Field_Type_command_functor;

class Meta_List_Field_Type_command_handler : public command_handler
{
   friend class Meta_List_Field_Type_command_functor;

   public:
   Meta_List_Field_Type_command_handler( ) : p_Meta_List_Field_Type( 0 ) { }

   void set_Meta_List_Field_Type( Meta_List_Field_Type* p_new_Meta_List_Field_Type ) { p_Meta_List_Field_Type = p_new_Meta_List_Field_Type; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_List_Field_Type* p_Meta_List_Field_Type;

   protected:
   string retval;
};

class Meta_List_Field_Type_command_functor : public command_functor
{
   public:
   Meta_List_Field_Type_command_functor( Meta_List_Field_Type_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_List_Field_Type_command_handler& cmd_handler;
};

command_functor* Meta_List_Field_Type_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_List_Field_Type_command_functor( dynamic_cast< Meta_List_Field_Type_command_handler& >( handler ) );
}

void Meta_List_Field_Type_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_List_Field_Type_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_List_Field_Type_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_List_Field_Type->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_List_Field_Type->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_List_Field_Type_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_List_Field_Type->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_List_Field_Type->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_List_Field_Type_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_List_Field_Type_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Allow_Child_Rel_Select_Specifics ) || ( field_name == c_field_name_Allow_Child_Rel_Select_Specifics ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Allow_Child_Rel_Select_Specifics( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Exclude_In_Use_FK ) || ( field_name == c_field_name_Allow_Exclude_In_Use_FK ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Allow_Exclude_In_Use_FK( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Link_Empty_Restriction ) || ( field_name == c_field_name_Allow_Link_Empty_Restriction ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Allow_Link_Empty_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Link_Permission ) || ( field_name == c_field_name_Allow_Link_Permission ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Allow_Link_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Link_Specifics ) || ( field_name == c_field_name_Allow_Link_Specifics ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Allow_Link_Specifics( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Link_Type ) || ( field_name == c_field_name_Allow_Link_Type ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Allow_Link_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Procedure ) || ( field_name == c_field_name_Allow_Procedure ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Allow_Procedure( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Restriction_Field ) || ( field_name == c_field_name_Allow_Restriction_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Allow_Restriction_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Restriction_Spec ) || ( field_name == c_field_name_Allow_Restriction_Spec ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Allow_Restriction_Spec( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Restriction_Value ) || ( field_name == c_field_name_Allow_Restriction_Value ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Allow_Restriction_Value( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Search_Option_Limit ) || ( field_name == c_field_name_Allow_Search_Option_Limit ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Allow_Search_Option_Limit( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Select_Specifics ) || ( field_name == c_field_name_Allow_Select_Specifics ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Allow_Select_Specifics( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Sort_Manually ) || ( field_name == c_field_name_Allow_Sort_Manually ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Allow_Sort_Manually( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_View_Parent_Extra ) || ( field_name == c_field_name_Allow_View_Parent_Extra ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Allow_View_Parent_Extra( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Restrict_Search ) || ( field_name == c_field_name_Is_Restrict_Search ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Is_Restrict_Search( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_List_Field_Name ) || ( field_name == c_field_name_List_Field_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_List_Field_Type->List_Field_Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_List_Field_Type->Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Restriction_Field ) || ( field_name == c_field_name_Needs_Restriction_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Needs_Restriction_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Source ) || ( field_name == c_field_name_Needs_Source ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Needs_Source( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Switch_Type ) || ( field_name == c_field_name_Needs_Switch_Type ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Needs_Switch_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Non_Simple_Field ) || ( field_name == c_field_name_Non_Simple_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Non_Simple_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Trivial_Field_Only ) || ( field_name == c_field_name_Trivial_Field_Only ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List_Field_Type->Trivial_Field_Only( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_List_Field_Type_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_List_Field_Type_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_List_Field_Type_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Allow_Child_Rel_Select_Specifics ) || ( field_name == c_field_name_Allow_Child_Rel_Select_Specifics ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Allow_Child_Rel_Select_Specifics, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Exclude_In_Use_FK ) || ( field_name == c_field_name_Allow_Exclude_In_Use_FK ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Allow_Exclude_In_Use_FK, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Link_Empty_Restriction ) || ( field_name == c_field_name_Allow_Link_Empty_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Allow_Link_Empty_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Link_Permission ) || ( field_name == c_field_name_Allow_Link_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Allow_Link_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Link_Specifics ) || ( field_name == c_field_name_Allow_Link_Specifics ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Allow_Link_Specifics, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Link_Type ) || ( field_name == c_field_name_Allow_Link_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Allow_Link_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Procedure ) || ( field_name == c_field_name_Allow_Procedure ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Allow_Procedure, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Restriction_Field ) || ( field_name == c_field_name_Allow_Restriction_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Allow_Restriction_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Restriction_Spec ) || ( field_name == c_field_name_Allow_Restriction_Spec ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Allow_Restriction_Spec, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Restriction_Value ) || ( field_name == c_field_name_Allow_Restriction_Value ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Allow_Restriction_Value, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Search_Option_Limit ) || ( field_name == c_field_name_Allow_Search_Option_Limit ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Allow_Search_Option_Limit, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Select_Specifics ) || ( field_name == c_field_name_Allow_Select_Specifics ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Allow_Select_Specifics, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Sort_Manually ) || ( field_name == c_field_name_Allow_Sort_Manually ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Allow_Sort_Manually, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_View_Parent_Extra ) || ( field_name == c_field_name_Allow_View_Parent_Extra ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Allow_View_Parent_Extra, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Restrict_Search ) || ( field_name == c_field_name_Is_Restrict_Search ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Is_Restrict_Search, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_List_Field_Name ) || ( field_name == c_field_name_List_Field_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, string >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::List_Field_Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, string >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Restriction_Field ) || ( field_name == c_field_name_Needs_Restriction_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Needs_Restriction_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Source ) || ( field_name == c_field_name_Needs_Source ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Needs_Source, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Switch_Type ) || ( field_name == c_field_name_Needs_Switch_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Needs_Switch_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Non_Simple_Field ) || ( field_name == c_field_name_Non_Simple_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Non_Simple_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Trivial_Field_Only ) || ( field_name == c_field_name_Trivial_Field_Only ) ) )
      {
         handled = true;

         func_string_setter< Meta_List_Field_Type, bool >(
          *cmd_handler.p_Meta_List_Field_Type, &Meta_List_Field_Type::Trivial_Field_Only, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_List_Field_Type_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_List_Field_Type_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_List_Field_Type_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
}

struct Meta_List_Field_Type::impl : public Meta_List_Field_Type_command_handler
{
   impl( Meta_List_Field_Type& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_List_Field_Type( &o );

      add_commands( 0, Meta_List_Field_Type_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_List_Field_Type_command_definitions ) );
   }

   Meta_List_Field_Type& get_obj( ) const
   {
      return *cp_obj;
   }

   bool impl_Allow_Child_Rel_Select_Specifics( ) const { return lazy_fetch( p_obj ), v_Allow_Child_Rel_Select_Specifics; }
   void impl_Allow_Child_Rel_Select_Specifics( bool Allow_Child_Rel_Select_Specifics ) { v_Allow_Child_Rel_Select_Specifics = Allow_Child_Rel_Select_Specifics; }

   bool impl_Allow_Exclude_In_Use_FK( ) const { return lazy_fetch( p_obj ), v_Allow_Exclude_In_Use_FK; }
   void impl_Allow_Exclude_In_Use_FK( bool Allow_Exclude_In_Use_FK ) { v_Allow_Exclude_In_Use_FK = Allow_Exclude_In_Use_FK; }

   bool impl_Allow_Link_Empty_Restriction( ) const { return lazy_fetch( p_obj ), v_Allow_Link_Empty_Restriction; }
   void impl_Allow_Link_Empty_Restriction( bool Allow_Link_Empty_Restriction ) { v_Allow_Link_Empty_Restriction = Allow_Link_Empty_Restriction; }

   bool impl_Allow_Link_Permission( ) const { return lazy_fetch( p_obj ), v_Allow_Link_Permission; }
   void impl_Allow_Link_Permission( bool Allow_Link_Permission ) { v_Allow_Link_Permission = Allow_Link_Permission; }

   bool impl_Allow_Link_Specifics( ) const { return lazy_fetch( p_obj ), v_Allow_Link_Specifics; }
   void impl_Allow_Link_Specifics( bool Allow_Link_Specifics ) { v_Allow_Link_Specifics = Allow_Link_Specifics; }

   bool impl_Allow_Link_Type( ) const { return lazy_fetch( p_obj ), v_Allow_Link_Type; }
   void impl_Allow_Link_Type( bool Allow_Link_Type ) { v_Allow_Link_Type = Allow_Link_Type; }

   bool impl_Allow_Procedure( ) const { return lazy_fetch( p_obj ), v_Allow_Procedure; }
   void impl_Allow_Procedure( bool Allow_Procedure ) { v_Allow_Procedure = Allow_Procedure; }

   bool impl_Allow_Restriction_Field( ) const { return lazy_fetch( p_obj ), v_Allow_Restriction_Field; }
   void impl_Allow_Restriction_Field( bool Allow_Restriction_Field ) { v_Allow_Restriction_Field = Allow_Restriction_Field; }

   bool impl_Allow_Restriction_Spec( ) const { return lazy_fetch( p_obj ), v_Allow_Restriction_Spec; }
   void impl_Allow_Restriction_Spec( bool Allow_Restriction_Spec ) { v_Allow_Restriction_Spec = Allow_Restriction_Spec; }

   bool impl_Allow_Restriction_Value( ) const { return lazy_fetch( p_obj ), v_Allow_Restriction_Value; }
   void impl_Allow_Restriction_Value( bool Allow_Restriction_Value ) { v_Allow_Restriction_Value = Allow_Restriction_Value; }

   bool impl_Allow_Search_Option_Limit( ) const { return lazy_fetch( p_obj ), v_Allow_Search_Option_Limit; }
   void impl_Allow_Search_Option_Limit( bool Allow_Search_Option_Limit ) { v_Allow_Search_Option_Limit = Allow_Search_Option_Limit; }

   bool impl_Allow_Select_Specifics( ) const { return lazy_fetch( p_obj ), v_Allow_Select_Specifics; }
   void impl_Allow_Select_Specifics( bool Allow_Select_Specifics ) { v_Allow_Select_Specifics = Allow_Select_Specifics; }

   bool impl_Allow_Sort_Manually( ) const { return lazy_fetch( p_obj ), v_Allow_Sort_Manually; }
   void impl_Allow_Sort_Manually( bool Allow_Sort_Manually ) { v_Allow_Sort_Manually = Allow_Sort_Manually; }

   bool impl_Allow_View_Parent_Extra( ) const { return lazy_fetch( p_obj ), v_Allow_View_Parent_Extra; }
   void impl_Allow_View_Parent_Extra( bool Allow_View_Parent_Extra ) { v_Allow_View_Parent_Extra = Allow_View_Parent_Extra; }

   bool impl_Is_Restrict_Search( ) const { return lazy_fetch( p_obj ), v_Is_Restrict_Search; }
   void impl_Is_Restrict_Search( bool Is_Restrict_Search ) { v_Is_Restrict_Search = Is_Restrict_Search; }

   const string& impl_List_Field_Name( ) const { return lazy_fetch( p_obj ), v_List_Field_Name; }
   void impl_List_Field_Name( const string& List_Field_Name ) { sanity_check( List_Field_Name ); v_List_Field_Name = List_Field_Name; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { sanity_check( Name ); v_Name = Name; }

   bool impl_Needs_Restriction_Field( ) const { return lazy_fetch( p_obj ), v_Needs_Restriction_Field; }
   void impl_Needs_Restriction_Field( bool Needs_Restriction_Field ) { v_Needs_Restriction_Field = Needs_Restriction_Field; }

   bool impl_Needs_Source( ) const { return lazy_fetch( p_obj ), v_Needs_Source; }
   void impl_Needs_Source( bool Needs_Source ) { v_Needs_Source = Needs_Source; }

   bool impl_Needs_Switch_Type( ) const { return lazy_fetch( p_obj ), v_Needs_Switch_Type; }
   void impl_Needs_Switch_Type( bool Needs_Switch_Type ) { v_Needs_Switch_Type = Needs_Switch_Type; }

   bool impl_Non_Simple_Field( ) const { return lazy_fetch( p_obj ), v_Non_Simple_Field; }
   void impl_Non_Simple_Field( bool Non_Simple_Field ) { v_Non_Simple_Field = Non_Simple_Field; }

   bool impl_Trivial_Field_Only( ) const { return lazy_fetch( p_obj ), v_Trivial_Field_Only; }
   void impl_Trivial_Field_Only( bool Trivial_Field_Only ) { v_Trivial_Field_Only = Trivial_Field_Only; }

   Meta_List_Field& impl_child_List_Field_Type( )
   {
      if( !cp_child_List_Field_Type )
      {
         cp_child_List_Field_Type.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Type, "302120" );
      }
      return *cp_child_List_Field_Type;
   }

   const Meta_List_Field& impl_child_List_Field_Type( ) const
   {
      if( !cp_child_List_Field_Type )
      {
         cp_child_List_Field_Type.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Type, "302120" );
      }
      return *cp_child_List_Field_Type;
   }

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

   Meta_List_Field_Type* p_obj;
   class_pointer< Meta_List_Field_Type > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   bool v_Allow_Child_Rel_Select_Specifics;
   bool v_Allow_Exclude_In_Use_FK;
   bool v_Allow_Link_Empty_Restriction;
   bool v_Allow_Link_Permission;
   bool v_Allow_Link_Specifics;
   bool v_Allow_Link_Type;
   bool v_Allow_Procedure;
   bool v_Allow_Restriction_Field;
   bool v_Allow_Restriction_Spec;
   bool v_Allow_Restriction_Value;
   bool v_Allow_Search_Option_Limit;
   bool v_Allow_Select_Specifics;
   bool v_Allow_Sort_Manually;
   bool v_Allow_View_Parent_Extra;
   bool v_Is_Restrict_Search;
   string v_List_Field_Name;
   string v_Name;
   bool v_Needs_Restriction_Field;
   bool v_Needs_Source;
   bool v_Needs_Switch_Type;
   bool v_Non_Simple_Field;
   bool v_Trivial_Field_Only;

   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Type;
};

string Meta_List_Field_Type::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Allow_Child_Rel_Select_Specifics( ) );
      break;

      case 1:
      retval = to_string( impl_Allow_Exclude_In_Use_FK( ) );
      break;

      case 2:
      retval = to_string( impl_Allow_Link_Empty_Restriction( ) );
      break;

      case 3:
      retval = to_string( impl_Allow_Link_Permission( ) );
      break;

      case 4:
      retval = to_string( impl_Allow_Link_Specifics( ) );
      break;

      case 5:
      retval = to_string( impl_Allow_Link_Type( ) );
      break;

      case 6:
      retval = to_string( impl_Allow_Procedure( ) );
      break;

      case 7:
      retval = to_string( impl_Allow_Restriction_Field( ) );
      break;

      case 8:
      retval = to_string( impl_Allow_Restriction_Spec( ) );
      break;

      case 9:
      retval = to_string( impl_Allow_Restriction_Value( ) );
      break;

      case 10:
      retval = to_string( impl_Allow_Search_Option_Limit( ) );
      break;

      case 11:
      retval = to_string( impl_Allow_Select_Specifics( ) );
      break;

      case 12:
      retval = to_string( impl_Allow_Sort_Manually( ) );
      break;

      case 13:
      retval = to_string( impl_Allow_View_Parent_Extra( ) );
      break;

      case 14:
      retval = to_string( impl_Is_Restrict_Search( ) );
      break;

      case 15:
      retval = to_string( impl_List_Field_Name( ) );
      break;

      case 16:
      retval = to_string( impl_Name( ) );
      break;

      case 17:
      retval = to_string( impl_Needs_Restriction_Field( ) );
      break;

      case 18:
      retval = to_string( impl_Needs_Source( ) );
      break;

      case 19:
      retval = to_string( impl_Needs_Switch_Type( ) );
      break;

      case 20:
      retval = to_string( impl_Non_Simple_Field( ) );
      break;

      case 21:
      retval = to_string( impl_Trivial_Field_Only( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_List_Field_Type::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Allow_Child_Rel_Select_Specifics, value );
      break;

      case 1:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Allow_Exclude_In_Use_FK, value );
      break;

      case 2:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Allow_Link_Empty_Restriction, value );
      break;

      case 3:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Allow_Link_Permission, value );
      break;

      case 4:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Allow_Link_Specifics, value );
      break;

      case 5:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Allow_Link_Type, value );
      break;

      case 6:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Allow_Procedure, value );
      break;

      case 7:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Allow_Restriction_Field, value );
      break;

      case 8:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Allow_Restriction_Spec, value );
      break;

      case 9:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Allow_Restriction_Value, value );
      break;

      case 10:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Allow_Search_Option_Limit, value );
      break;

      case 11:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Allow_Select_Specifics, value );
      break;

      case 12:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Allow_Sort_Manually, value );
      break;

      case 13:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Allow_View_Parent_Extra, value );
      break;

      case 14:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Is_Restrict_Search, value );
      break;

      case 15:
      func_string_setter< Meta_List_Field_Type::impl, string >( *this, &Meta_List_Field_Type::impl::impl_List_Field_Name, value );
      break;

      case 16:
      func_string_setter< Meta_List_Field_Type::impl, string >( *this, &Meta_List_Field_Type::impl::impl_Name, value );
      break;

      case 17:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Needs_Restriction_Field, value );
      break;

      case 18:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Needs_Source, value );
      break;

      case 19:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Needs_Switch_Type, value );
      break;

      case 20:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Non_Simple_Field, value );
      break;

      case 21:
      func_string_setter< Meta_List_Field_Type::impl, bool >( *this, &Meta_List_Field_Type::impl::impl_Trivial_Field_Only, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_List_Field_Type::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Allow_Child_Rel_Select_Specifics( g_default_Allow_Child_Rel_Select_Specifics );
      break;

      case 1:
      impl_Allow_Exclude_In_Use_FK( g_default_Allow_Exclude_In_Use_FK );
      break;

      case 2:
      impl_Allow_Link_Empty_Restriction( g_default_Allow_Link_Empty_Restriction );
      break;

      case 3:
      impl_Allow_Link_Permission( g_default_Allow_Link_Permission );
      break;

      case 4:
      impl_Allow_Link_Specifics( g_default_Allow_Link_Specifics );
      break;

      case 5:
      impl_Allow_Link_Type( g_default_Allow_Link_Type );
      break;

      case 6:
      impl_Allow_Procedure( g_default_Allow_Procedure );
      break;

      case 7:
      impl_Allow_Restriction_Field( g_default_Allow_Restriction_Field );
      break;

      case 8:
      impl_Allow_Restriction_Spec( g_default_Allow_Restriction_Spec );
      break;

      case 9:
      impl_Allow_Restriction_Value( g_default_Allow_Restriction_Value );
      break;

      case 10:
      impl_Allow_Search_Option_Limit( g_default_Allow_Search_Option_Limit );
      break;

      case 11:
      impl_Allow_Select_Specifics( g_default_Allow_Select_Specifics );
      break;

      case 12:
      impl_Allow_Sort_Manually( g_default_Allow_Sort_Manually );
      break;

      case 13:
      impl_Allow_View_Parent_Extra( g_default_Allow_View_Parent_Extra );
      break;

      case 14:
      impl_Is_Restrict_Search( g_default_Is_Restrict_Search );
      break;

      case 15:
      impl_List_Field_Name( g_default_List_Field_Name );
      break;

      case 16:
      impl_Name( g_default_Name );
      break;

      case 17:
      impl_Needs_Restriction_Field( g_default_Needs_Restriction_Field );
      break;

      case 18:
      impl_Needs_Source( g_default_Needs_Source );
      break;

      case 19:
      impl_Needs_Switch_Type( g_default_Needs_Switch_Type );
      break;

      case 20:
      impl_Non_Simple_Field( g_default_Non_Simple_Field );
      break;

      case 21:
      impl_Trivial_Field_Only( g_default_Trivial_Field_Only );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_List_Field_Type::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Allow_Child_Rel_Select_Specifics == g_default_Allow_Child_Rel_Select_Specifics );
      break;

      case 1:
      retval = ( v_Allow_Exclude_In_Use_FK == g_default_Allow_Exclude_In_Use_FK );
      break;

      case 2:
      retval = ( v_Allow_Link_Empty_Restriction == g_default_Allow_Link_Empty_Restriction );
      break;

      case 3:
      retval = ( v_Allow_Link_Permission == g_default_Allow_Link_Permission );
      break;

      case 4:
      retval = ( v_Allow_Link_Specifics == g_default_Allow_Link_Specifics );
      break;

      case 5:
      retval = ( v_Allow_Link_Type == g_default_Allow_Link_Type );
      break;

      case 6:
      retval = ( v_Allow_Procedure == g_default_Allow_Procedure );
      break;

      case 7:
      retval = ( v_Allow_Restriction_Field == g_default_Allow_Restriction_Field );
      break;

      case 8:
      retval = ( v_Allow_Restriction_Spec == g_default_Allow_Restriction_Spec );
      break;

      case 9:
      retval = ( v_Allow_Restriction_Value == g_default_Allow_Restriction_Value );
      break;

      case 10:
      retval = ( v_Allow_Search_Option_Limit == g_default_Allow_Search_Option_Limit );
      break;

      case 11:
      retval = ( v_Allow_Select_Specifics == g_default_Allow_Select_Specifics );
      break;

      case 12:
      retval = ( v_Allow_Sort_Manually == g_default_Allow_Sort_Manually );
      break;

      case 13:
      retval = ( v_Allow_View_Parent_Extra == g_default_Allow_View_Parent_Extra );
      break;

      case 14:
      retval = ( v_Is_Restrict_Search == g_default_Is_Restrict_Search );
      break;

      case 15:
      retval = ( v_List_Field_Name == g_default_List_Field_Name );
      break;

      case 16:
      retval = ( v_Name == g_default_Name );
      break;

      case 17:
      retval = ( v_Needs_Restriction_Field == g_default_Needs_Restriction_Field );
      break;

      case 18:
      retval = ( v_Needs_Source == g_default_Needs_Source );
      break;

      case 19:
      retval = ( v_Needs_Switch_Type == g_default_Needs_Switch_Type );
      break;

      case 20:
      retval = ( v_Non_Simple_Field == g_default_Non_Simple_Field );
      break;

      case 21:
      retval = ( v_Trivial_Field_Only == g_default_Trivial_Field_Only );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_List_Field_Type::impl::get_state( ) const
{
   uint64_t state = 0;

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

string Meta_List_Field_Type::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_List_Field_Type::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_List_Field_Type::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_List_Field_Type::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_List_Field_Type::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_List_Field_Type::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   ( void )foreign_key_values;
}

void Meta_List_Field_Type::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_List_Field_Type::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_List_Field_Type::impl::clear( )
{
   v_Allow_Child_Rel_Select_Specifics = g_default_Allow_Child_Rel_Select_Specifics;
   v_Allow_Exclude_In_Use_FK = g_default_Allow_Exclude_In_Use_FK;
   v_Allow_Link_Empty_Restriction = g_default_Allow_Link_Empty_Restriction;
   v_Allow_Link_Permission = g_default_Allow_Link_Permission;
   v_Allow_Link_Specifics = g_default_Allow_Link_Specifics;
   v_Allow_Link_Type = g_default_Allow_Link_Type;
   v_Allow_Procedure = g_default_Allow_Procedure;
   v_Allow_Restriction_Field = g_default_Allow_Restriction_Field;
   v_Allow_Restriction_Spec = g_default_Allow_Restriction_Spec;
   v_Allow_Restriction_Value = g_default_Allow_Restriction_Value;
   v_Allow_Search_Option_Limit = g_default_Allow_Search_Option_Limit;
   v_Allow_Select_Specifics = g_default_Allow_Select_Specifics;
   v_Allow_Sort_Manually = g_default_Allow_Sort_Manually;
   v_Allow_View_Parent_Extra = g_default_Allow_View_Parent_Extra;
   v_Is_Restrict_Search = g_default_Is_Restrict_Search;
   v_List_Field_Name = g_default_List_Field_Name;
   v_Name = g_default_Name;
   v_Needs_Restriction_Field = g_default_Needs_Restriction_Field;
   v_Needs_Source = g_default_Needs_Source;
   v_Needs_Switch_Type = g_default_Needs_Switch_Type;
   v_Non_Simple_Field = g_default_Non_Simple_Field;
   v_Trivial_Field_Only = g_default_Trivial_Field_Only;
}

bool Meta_List_Field_Type::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_List_Field_Type::impl::validate(
 uint64_t state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( is_null( v_List_Field_Name ) && !value_will_be_provided( c_field_name_List_Field_Name ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_List_Field_Name,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_List_Field_Name ) ) ) ) );

   if( is_null( v_Name ) && !value_will_be_provided( c_field_name_Name ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Name ) ) ) ) );

   if( !is_null( v_List_Field_Name )
    && ( v_List_Field_Name != g_default_List_Field_Name
    || !value_will_be_provided( c_field_name_List_Field_Name ) )
    && !g_List_Field_Name_domain.is_valid( v_List_Field_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_List_Field_Name,
       get_module_string( c_field_display_name_List_Field_Name ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( v_Name != g_default_Name
    || !value_will_be_provided( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_List_Field_Type::impl::validate_set_fields(
 set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( !is_null( v_List_Field_Name )
    && ( fields_set.count( c_field_id_List_Field_Name ) || fields_set.count( c_field_name_List_Field_Name ) )
    && !g_List_Field_Name_domain.is_valid( v_List_Field_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_List_Field_Name,
       get_module_string( c_field_display_name_List_Field_Name ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( fields_set.count( c_field_id_Name ) || fields_set.count( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );
}

void Meta_List_Field_Type::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_List_Field_Type::impl::finalise_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_List_Field_Type::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_List_Field_Type::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_List_Field_Type::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_List_Field_Type::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_List_Field_Type::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_List_Field_Type::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_List_Field_Type::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_List_Field_Type::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_List_Field_Type::impl::set_default_values( )
{
   clear( );
}

bool Meta_List_Field_Type::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_List_Field_Type::impl::get_required_transients( ) const
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

Meta_List_Field_Type::Meta_List_Field_Type( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_List_Field_Type::~Meta_List_Field_Type( )
{
   cleanup( );
   delete p_impl;
}

bool Meta_List_Field_Type::Allow_Child_Rel_Select_Specifics( ) const
{
   return p_impl->impl_Allow_Child_Rel_Select_Specifics( );
}

void Meta_List_Field_Type::Allow_Child_Rel_Select_Specifics( bool Allow_Child_Rel_Select_Specifics )
{
   p_impl->impl_Allow_Child_Rel_Select_Specifics( Allow_Child_Rel_Select_Specifics );
}

bool Meta_List_Field_Type::Allow_Exclude_In_Use_FK( ) const
{
   return p_impl->impl_Allow_Exclude_In_Use_FK( );
}

void Meta_List_Field_Type::Allow_Exclude_In_Use_FK( bool Allow_Exclude_In_Use_FK )
{
   p_impl->impl_Allow_Exclude_In_Use_FK( Allow_Exclude_In_Use_FK );
}

bool Meta_List_Field_Type::Allow_Link_Empty_Restriction( ) const
{
   return p_impl->impl_Allow_Link_Empty_Restriction( );
}

void Meta_List_Field_Type::Allow_Link_Empty_Restriction( bool Allow_Link_Empty_Restriction )
{
   p_impl->impl_Allow_Link_Empty_Restriction( Allow_Link_Empty_Restriction );
}

bool Meta_List_Field_Type::Allow_Link_Permission( ) const
{
   return p_impl->impl_Allow_Link_Permission( );
}

void Meta_List_Field_Type::Allow_Link_Permission( bool Allow_Link_Permission )
{
   p_impl->impl_Allow_Link_Permission( Allow_Link_Permission );
}

bool Meta_List_Field_Type::Allow_Link_Specifics( ) const
{
   return p_impl->impl_Allow_Link_Specifics( );
}

void Meta_List_Field_Type::Allow_Link_Specifics( bool Allow_Link_Specifics )
{
   p_impl->impl_Allow_Link_Specifics( Allow_Link_Specifics );
}

bool Meta_List_Field_Type::Allow_Link_Type( ) const
{
   return p_impl->impl_Allow_Link_Type( );
}

void Meta_List_Field_Type::Allow_Link_Type( bool Allow_Link_Type )
{
   p_impl->impl_Allow_Link_Type( Allow_Link_Type );
}

bool Meta_List_Field_Type::Allow_Procedure( ) const
{
   return p_impl->impl_Allow_Procedure( );
}

void Meta_List_Field_Type::Allow_Procedure( bool Allow_Procedure )
{
   p_impl->impl_Allow_Procedure( Allow_Procedure );
}

bool Meta_List_Field_Type::Allow_Restriction_Field( ) const
{
   return p_impl->impl_Allow_Restriction_Field( );
}

void Meta_List_Field_Type::Allow_Restriction_Field( bool Allow_Restriction_Field )
{
   p_impl->impl_Allow_Restriction_Field( Allow_Restriction_Field );
}

bool Meta_List_Field_Type::Allow_Restriction_Spec( ) const
{
   return p_impl->impl_Allow_Restriction_Spec( );
}

void Meta_List_Field_Type::Allow_Restriction_Spec( bool Allow_Restriction_Spec )
{
   p_impl->impl_Allow_Restriction_Spec( Allow_Restriction_Spec );
}

bool Meta_List_Field_Type::Allow_Restriction_Value( ) const
{
   return p_impl->impl_Allow_Restriction_Value( );
}

void Meta_List_Field_Type::Allow_Restriction_Value( bool Allow_Restriction_Value )
{
   p_impl->impl_Allow_Restriction_Value( Allow_Restriction_Value );
}

bool Meta_List_Field_Type::Allow_Search_Option_Limit( ) const
{
   return p_impl->impl_Allow_Search_Option_Limit( );
}

void Meta_List_Field_Type::Allow_Search_Option_Limit( bool Allow_Search_Option_Limit )
{
   p_impl->impl_Allow_Search_Option_Limit( Allow_Search_Option_Limit );
}

bool Meta_List_Field_Type::Allow_Select_Specifics( ) const
{
   return p_impl->impl_Allow_Select_Specifics( );
}

void Meta_List_Field_Type::Allow_Select_Specifics( bool Allow_Select_Specifics )
{
   p_impl->impl_Allow_Select_Specifics( Allow_Select_Specifics );
}

bool Meta_List_Field_Type::Allow_Sort_Manually( ) const
{
   return p_impl->impl_Allow_Sort_Manually( );
}

void Meta_List_Field_Type::Allow_Sort_Manually( bool Allow_Sort_Manually )
{
   p_impl->impl_Allow_Sort_Manually( Allow_Sort_Manually );
}

bool Meta_List_Field_Type::Allow_View_Parent_Extra( ) const
{
   return p_impl->impl_Allow_View_Parent_Extra( );
}

void Meta_List_Field_Type::Allow_View_Parent_Extra( bool Allow_View_Parent_Extra )
{
   p_impl->impl_Allow_View_Parent_Extra( Allow_View_Parent_Extra );
}

bool Meta_List_Field_Type::Is_Restrict_Search( ) const
{
   return p_impl->impl_Is_Restrict_Search( );
}

void Meta_List_Field_Type::Is_Restrict_Search( bool Is_Restrict_Search )
{
   p_impl->impl_Is_Restrict_Search( Is_Restrict_Search );
}

const string& Meta_List_Field_Type::List_Field_Name( ) const
{
   return p_impl->impl_List_Field_Name( );
}

void Meta_List_Field_Type::List_Field_Name( const string& List_Field_Name )
{
   p_impl->impl_List_Field_Name( List_Field_Name );
}

const string& Meta_List_Field_Type::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_List_Field_Type::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

bool Meta_List_Field_Type::Needs_Restriction_Field( ) const
{
   return p_impl->impl_Needs_Restriction_Field( );
}

void Meta_List_Field_Type::Needs_Restriction_Field( bool Needs_Restriction_Field )
{
   p_impl->impl_Needs_Restriction_Field( Needs_Restriction_Field );
}

bool Meta_List_Field_Type::Needs_Source( ) const
{
   return p_impl->impl_Needs_Source( );
}

void Meta_List_Field_Type::Needs_Source( bool Needs_Source )
{
   p_impl->impl_Needs_Source( Needs_Source );
}

bool Meta_List_Field_Type::Needs_Switch_Type( ) const
{
   return p_impl->impl_Needs_Switch_Type( );
}

void Meta_List_Field_Type::Needs_Switch_Type( bool Needs_Switch_Type )
{
   p_impl->impl_Needs_Switch_Type( Needs_Switch_Type );
}

bool Meta_List_Field_Type::Non_Simple_Field( ) const
{
   return p_impl->impl_Non_Simple_Field( );
}

void Meta_List_Field_Type::Non_Simple_Field( bool Non_Simple_Field )
{
   p_impl->impl_Non_Simple_Field( Non_Simple_Field );
}

bool Meta_List_Field_Type::Trivial_Field_Only( ) const
{
   return p_impl->impl_Trivial_Field_Only( );
}

void Meta_List_Field_Type::Trivial_Field_Only( bool Trivial_Field_Only )
{
   p_impl->impl_Trivial_Field_Only( Trivial_Field_Only );
}

Meta_List_Field& Meta_List_Field_Type::child_List_Field_Type( )
{
   return p_impl->impl_child_List_Field_Type( );
}

const Meta_List_Field& Meta_List_Field_Type::child_List_Field_Type( ) const
{
   return p_impl->impl_child_List_Field_Type( );
}

string Meta_List_Field_Type::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_List_Field_Type::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_List_Field_Type::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_List_Field_Type::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_List_Field_Type::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_List_Field_Type::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_List_Field_Type::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_List_Field_Type::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_List_Field_Type::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_List_Field_Type::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_List_Field_Type::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_List_Field_Type::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_List_Field_Type::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_List_Field_Type::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_List_Field_Type::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_List_Field_Type::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_List_Field_Type::clear( )
{
   p_impl->clear( );
}

void Meta_List_Field_Type::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_List_Field_Type::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_List_Field_Type::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_List_Field_Type::finalise_fetch( bool skip_set_original )
{
   p_impl->finalise_fetch( skip_set_original );
}

void Meta_List_Field_Type::at_create( )
{
   p_impl->at_create( );
}

void Meta_List_Field_Type::post_init( )
{
   p_impl->post_init( );
}

void Meta_List_Field_Type::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_List_Field_Type::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_List_Field_Type::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_List_Field_Type::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_List_Field_Type::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_List_Field_Type::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_List_Field_Type::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_List_Field_Type::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_List_Field_Type::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( 0 );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Allow_Child_Rel_Select_Specifics )
   {
      p_id = c_field_id_Allow_Child_Rel_Select_Specifics;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Exclude_In_Use_FK )
   {
      p_id = c_field_id_Allow_Exclude_In_Use_FK;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Link_Empty_Restriction )
   {
      p_id = c_field_id_Allow_Link_Empty_Restriction;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Link_Permission )
   {
      p_id = c_field_id_Allow_Link_Permission;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Link_Specifics )
   {
      p_id = c_field_id_Allow_Link_Specifics;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Link_Type )
   {
      p_id = c_field_id_Allow_Link_Type;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Procedure )
   {
      p_id = c_field_id_Allow_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Restriction_Field )
   {
      p_id = c_field_id_Allow_Restriction_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Restriction_Spec )
   {
      p_id = c_field_id_Allow_Restriction_Spec;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Restriction_Value )
   {
      p_id = c_field_id_Allow_Restriction_Value;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Search_Option_Limit )
   {
      p_id = c_field_id_Allow_Search_Option_Limit;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Select_Specifics )
   {
      p_id = c_field_id_Allow_Select_Specifics;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Sort_Manually )
   {
      p_id = c_field_id_Allow_Sort_Manually;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_View_Parent_Extra )
   {
      p_id = c_field_id_Allow_View_Parent_Extra;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Restrict_Search )
   {
      p_id = c_field_id_Is_Restrict_Search;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_List_Field_Name )
   {
      p_id = c_field_id_List_Field_Name;

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
   else if( name == c_field_name_Needs_Restriction_Field )
   {
      p_id = c_field_id_Needs_Restriction_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Source )
   {
      p_id = c_field_id_Needs_Source;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Switch_Type )
   {
      p_id = c_field_id_Needs_Switch_Type;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Non_Simple_Field )
   {
      p_id = c_field_id_Non_Simple_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Trivial_Field_Only )
   {
      p_id = c_field_id_Trivial_Field_Only;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_id;
}

const char* Meta_List_Field_Type::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( 0 );

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Allow_Child_Rel_Select_Specifics )
   {
      p_name = c_field_name_Allow_Child_Rel_Select_Specifics;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Exclude_In_Use_FK )
   {
      p_name = c_field_name_Allow_Exclude_In_Use_FK;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Link_Empty_Restriction )
   {
      p_name = c_field_name_Allow_Link_Empty_Restriction;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Link_Permission )
   {
      p_name = c_field_name_Allow_Link_Permission;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Link_Specifics )
   {
      p_name = c_field_name_Allow_Link_Specifics;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Link_Type )
   {
      p_name = c_field_name_Allow_Link_Type;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Procedure )
   {
      p_name = c_field_name_Allow_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Restriction_Field )
   {
      p_name = c_field_name_Allow_Restriction_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Restriction_Spec )
   {
      p_name = c_field_name_Allow_Restriction_Spec;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Restriction_Value )
   {
      p_name = c_field_name_Allow_Restriction_Value;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Search_Option_Limit )
   {
      p_name = c_field_name_Allow_Search_Option_Limit;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Select_Specifics )
   {
      p_name = c_field_name_Allow_Select_Specifics;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Sort_Manually )
   {
      p_name = c_field_name_Allow_Sort_Manually;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_View_Parent_Extra )
   {
      p_name = c_field_name_Allow_View_Parent_Extra;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Restrict_Search )
   {
      p_name = c_field_name_Is_Restrict_Search;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_List_Field_Name )
   {
      p_name = c_field_name_List_Field_Name;

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
   else if( id == c_field_id_Needs_Restriction_Field )
   {
      p_name = c_field_name_Needs_Restriction_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Source )
   {
      p_name = c_field_name_Needs_Source;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Switch_Type )
   {
      p_name = c_field_name_Needs_Switch_Type;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Non_Simple_Field )
   {
      p_name = c_field_name_Non_Simple_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Trivial_Field_Only )
   {
      p_name = c_field_name_Trivial_Field_Only;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_name;
}

string& Meta_List_Field_Type::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_List_Field_Type::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_List_Field_Type::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_List_Field_Type::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_List_Field_Type::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_List_Field_Type::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_List_Field_Type::get_field_uom_symbol( const string& id_or_name ) const
{
   string uom_symbol;

   string name;
   pair< string, string > next;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_uom_symbol" );
   if( ( id_or_name == c_field_id_Allow_Child_Rel_Select_Specifics ) || ( id_or_name == c_field_name_Allow_Child_Rel_Select_Specifics ) )
   {
      name = string( c_field_display_name_Allow_Child_Rel_Select_Specifics );
      get_module_string( c_field_display_name_Allow_Child_Rel_Select_Specifics, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Exclude_In_Use_FK ) || ( id_or_name == c_field_name_Allow_Exclude_In_Use_FK ) )
   {
      name = string( c_field_display_name_Allow_Exclude_In_Use_FK );
      get_module_string( c_field_display_name_Allow_Exclude_In_Use_FK, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Link_Empty_Restriction ) || ( id_or_name == c_field_name_Allow_Link_Empty_Restriction ) )
   {
      name = string( c_field_display_name_Allow_Link_Empty_Restriction );
      get_module_string( c_field_display_name_Allow_Link_Empty_Restriction, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Link_Permission ) || ( id_or_name == c_field_name_Allow_Link_Permission ) )
   {
      name = string( c_field_display_name_Allow_Link_Permission );
      get_module_string( c_field_display_name_Allow_Link_Permission, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Link_Specifics ) || ( id_or_name == c_field_name_Allow_Link_Specifics ) )
   {
      name = string( c_field_display_name_Allow_Link_Specifics );
      get_module_string( c_field_display_name_Allow_Link_Specifics, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Link_Type ) || ( id_or_name == c_field_name_Allow_Link_Type ) )
   {
      name = string( c_field_display_name_Allow_Link_Type );
      get_module_string( c_field_display_name_Allow_Link_Type, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Procedure ) || ( id_or_name == c_field_name_Allow_Procedure ) )
   {
      name = string( c_field_display_name_Allow_Procedure );
      get_module_string( c_field_display_name_Allow_Procedure, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Restriction_Field ) || ( id_or_name == c_field_name_Allow_Restriction_Field ) )
   {
      name = string( c_field_display_name_Allow_Restriction_Field );
      get_module_string( c_field_display_name_Allow_Restriction_Field, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Restriction_Spec ) || ( id_or_name == c_field_name_Allow_Restriction_Spec ) )
   {
      name = string( c_field_display_name_Allow_Restriction_Spec );
      get_module_string( c_field_display_name_Allow_Restriction_Spec, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Restriction_Value ) || ( id_or_name == c_field_name_Allow_Restriction_Value ) )
   {
      name = string( c_field_display_name_Allow_Restriction_Value );
      get_module_string( c_field_display_name_Allow_Restriction_Value, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Search_Option_Limit ) || ( id_or_name == c_field_name_Allow_Search_Option_Limit ) )
   {
      name = string( c_field_display_name_Allow_Search_Option_Limit );
      get_module_string( c_field_display_name_Allow_Search_Option_Limit, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Select_Specifics ) || ( id_or_name == c_field_name_Allow_Select_Specifics ) )
   {
      name = string( c_field_display_name_Allow_Select_Specifics );
      get_module_string( c_field_display_name_Allow_Select_Specifics, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Sort_Manually ) || ( id_or_name == c_field_name_Allow_Sort_Manually ) )
   {
      name = string( c_field_display_name_Allow_Sort_Manually );
      get_module_string( c_field_display_name_Allow_Sort_Manually, &next );
   }
   if( ( id_or_name == c_field_id_Allow_View_Parent_Extra ) || ( id_or_name == c_field_name_Allow_View_Parent_Extra ) )
   {
      name = string( c_field_display_name_Allow_View_Parent_Extra );
      get_module_string( c_field_display_name_Allow_View_Parent_Extra, &next );
   }
   if( ( id_or_name == c_field_id_Is_Restrict_Search ) || ( id_or_name == c_field_name_Is_Restrict_Search ) )
   {
      name = string( c_field_display_name_Is_Restrict_Search );
      get_module_string( c_field_display_name_Is_Restrict_Search, &next );
   }
   if( ( id_or_name == c_field_id_List_Field_Name ) || ( id_or_name == c_field_name_List_Field_Name ) )
   {
      name = string( c_field_display_name_List_Field_Name );
      get_module_string( c_field_display_name_List_Field_Name, &next );
   }
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
   {
      name = string( c_field_display_name_Name );
      get_module_string( c_field_display_name_Name, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Restriction_Field ) || ( id_or_name == c_field_name_Needs_Restriction_Field ) )
   {
      name = string( c_field_display_name_Needs_Restriction_Field );
      get_module_string( c_field_display_name_Needs_Restriction_Field, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Source ) || ( id_or_name == c_field_name_Needs_Source ) )
   {
      name = string( c_field_display_name_Needs_Source );
      get_module_string( c_field_display_name_Needs_Source, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Switch_Type ) || ( id_or_name == c_field_name_Needs_Switch_Type ) )
   {
      name = string( c_field_display_name_Needs_Switch_Type );
      get_module_string( c_field_display_name_Needs_Switch_Type, &next );
   }
   if( ( id_or_name == c_field_id_Non_Simple_Field ) || ( id_or_name == c_field_name_Non_Simple_Field ) )
   {
      name = string( c_field_display_name_Non_Simple_Field );
      get_module_string( c_field_display_name_Non_Simple_Field, &next );
   }
   if( ( id_or_name == c_field_id_Trivial_Field_Only ) || ( id_or_name == c_field_name_Trivial_Field_Only ) )
   {
      name = string( c_field_display_name_Trivial_Field_Only );
      get_module_string( c_field_display_name_Trivial_Field_Only, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_List_Field_Type::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Allow_Child_Rel_Select_Specifics ) || ( id_or_name == c_field_name_Allow_Child_Rel_Select_Specifics ) )
      display_name = get_module_string( c_field_display_name_Allow_Child_Rel_Select_Specifics );
   if( ( id_or_name == c_field_id_Allow_Exclude_In_Use_FK ) || ( id_or_name == c_field_name_Allow_Exclude_In_Use_FK ) )
      display_name = get_module_string( c_field_display_name_Allow_Exclude_In_Use_FK );
   if( ( id_or_name == c_field_id_Allow_Link_Empty_Restriction ) || ( id_or_name == c_field_name_Allow_Link_Empty_Restriction ) )
      display_name = get_module_string( c_field_display_name_Allow_Link_Empty_Restriction );
   if( ( id_or_name == c_field_id_Allow_Link_Permission ) || ( id_or_name == c_field_name_Allow_Link_Permission ) )
      display_name = get_module_string( c_field_display_name_Allow_Link_Permission );
   if( ( id_or_name == c_field_id_Allow_Link_Specifics ) || ( id_or_name == c_field_name_Allow_Link_Specifics ) )
      display_name = get_module_string( c_field_display_name_Allow_Link_Specifics );
   if( ( id_or_name == c_field_id_Allow_Link_Type ) || ( id_or_name == c_field_name_Allow_Link_Type ) )
      display_name = get_module_string( c_field_display_name_Allow_Link_Type );
   if( ( id_or_name == c_field_id_Allow_Procedure ) || ( id_or_name == c_field_name_Allow_Procedure ) )
      display_name = get_module_string( c_field_display_name_Allow_Procedure );
   if( ( id_or_name == c_field_id_Allow_Restriction_Field ) || ( id_or_name == c_field_name_Allow_Restriction_Field ) )
      display_name = get_module_string( c_field_display_name_Allow_Restriction_Field );
   if( ( id_or_name == c_field_id_Allow_Restriction_Spec ) || ( id_or_name == c_field_name_Allow_Restriction_Spec ) )
      display_name = get_module_string( c_field_display_name_Allow_Restriction_Spec );
   if( ( id_or_name == c_field_id_Allow_Restriction_Value ) || ( id_or_name == c_field_name_Allow_Restriction_Value ) )
      display_name = get_module_string( c_field_display_name_Allow_Restriction_Value );
   if( ( id_or_name == c_field_id_Allow_Search_Option_Limit ) || ( id_or_name == c_field_name_Allow_Search_Option_Limit ) )
      display_name = get_module_string( c_field_display_name_Allow_Search_Option_Limit );
   if( ( id_or_name == c_field_id_Allow_Select_Specifics ) || ( id_or_name == c_field_name_Allow_Select_Specifics ) )
      display_name = get_module_string( c_field_display_name_Allow_Select_Specifics );
   if( ( id_or_name == c_field_id_Allow_Sort_Manually ) || ( id_or_name == c_field_name_Allow_Sort_Manually ) )
      display_name = get_module_string( c_field_display_name_Allow_Sort_Manually );
   if( ( id_or_name == c_field_id_Allow_View_Parent_Extra ) || ( id_or_name == c_field_name_Allow_View_Parent_Extra ) )
      display_name = get_module_string( c_field_display_name_Allow_View_Parent_Extra );
   if( ( id_or_name == c_field_id_Is_Restrict_Search ) || ( id_or_name == c_field_name_Is_Restrict_Search ) )
      display_name = get_module_string( c_field_display_name_Is_Restrict_Search );
   if( ( id_or_name == c_field_id_List_Field_Name ) || ( id_or_name == c_field_name_List_Field_Name ) )
      display_name = get_module_string( c_field_display_name_List_Field_Name );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
      display_name = get_module_string( c_field_display_name_Name );
   if( ( id_or_name == c_field_id_Needs_Restriction_Field ) || ( id_or_name == c_field_name_Needs_Restriction_Field ) )
      display_name = get_module_string( c_field_display_name_Needs_Restriction_Field );
   if( ( id_or_name == c_field_id_Needs_Source ) || ( id_or_name == c_field_name_Needs_Source ) )
      display_name = get_module_string( c_field_display_name_Needs_Source );
   if( ( id_or_name == c_field_id_Needs_Switch_Type ) || ( id_or_name == c_field_name_Needs_Switch_Type ) )
      display_name = get_module_string( c_field_display_name_Needs_Switch_Type );
   if( ( id_or_name == c_field_id_Non_Simple_Field ) || ( id_or_name == c_field_name_Non_Simple_Field ) )
      display_name = get_module_string( c_field_display_name_Non_Simple_Field );
   if( ( id_or_name == c_field_id_Trivial_Field_Only ) || ( id_or_name == c_field_name_Trivial_Field_Only ) )
      display_name = get_module_string( c_field_display_name_Trivial_Field_Only );

   return display_name;
}

void Meta_List_Field_Type::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_List_Field_Type::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_List_Field_Type::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_List_Field_Type::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_List_Field_Type::setup_graph_parent( Meta_List_Field& o, const string& foreign_key_field )
{
   static_cast< Meta_List_Field& >( o ).set_graph_parent( this, foreign_key_field );
}

size_t Meta_List_Field_Type::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_List_Field_Type::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_List_Field_Type::get_num_foreign_key_children( bool is_internal ) const
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

class_base* Meta_List_Field_Type::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   if( child_num >= 1 )
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
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302120";
            p_class_base = &child_List_Field_Type( );
         }
         break;
      }
   }

   return p_class_base;
}

void Meta_List_Field_Type::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_List_Field_Type::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_List_Field_Type::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_List_Field_Type::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_List_Field_Type::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_List_Field_Type::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_List_Field_Type::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_List_Field_Type::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "list_field_type";

   return get_module_string( key );
}

string Meta_List_Field_Type::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_List_Field_Type::get_create_instance_info( ) const
{
   return "";
}

string Meta_List_Field_Type::get_update_instance_info( ) const
{
   return "";
}

string Meta_List_Field_Type::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_List_Field_Type::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );

   return retval;
}

bool Meta_List_Field_Type::get_is_alias( ) const
{
   return false;
}

void Meta_List_Field_Type::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_List_Field_Type::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_List_Field_Type::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == "_302120" ) || ( sub_context == "child_List_Field_Type" ) )
      p_class_base = &child_List_Field_Type( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_List_Field_Type::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Allow_Child_Rel_Select_Specifics" );
   names.push_back( "C_Allow_Exclude_In_Use_FK" );
   names.push_back( "C_Allow_Link_Empty_Restriction" );
   names.push_back( "C_Allow_Link_Permission" );
   names.push_back( "C_Allow_Link_Specifics" );
   names.push_back( "C_Allow_Link_Type" );
   names.push_back( "C_Allow_Procedure" );
   names.push_back( "C_Allow_Restriction_Field" );
   names.push_back( "C_Allow_Restriction_Spec" );
   names.push_back( "C_Allow_Restriction_Value" );
   names.push_back( "C_Allow_Search_Option_Limit" );
   names.push_back( "C_Allow_Select_Specifics" );
   names.push_back( "C_Allow_Sort_Manually" );
   names.push_back( "C_Allow_View_Parent_Extra" );
   names.push_back( "C_Is_Restrict_Search" );
   names.push_back( "C_List_Field_Name" );
   names.push_back( "C_Name" );
   names.push_back( "C_Needs_Restriction_Field" );
   names.push_back( "C_Needs_Source" );
   names.push_back( "C_Needs_Switch_Type" );
   names.push_back( "C_Non_Simple_Field" );
   names.push_back( "C_Trivial_Field_Only" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_List_Field_Type::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( to_string( Allow_Child_Rel_Select_Specifics( ) ) );
   values.push_back( to_string( Allow_Exclude_In_Use_FK( ) ) );
   values.push_back( to_string( Allow_Link_Empty_Restriction( ) ) );
   values.push_back( to_string( Allow_Link_Permission( ) ) );
   values.push_back( to_string( Allow_Link_Specifics( ) ) );
   values.push_back( to_string( Allow_Link_Type( ) ) );
   values.push_back( to_string( Allow_Procedure( ) ) );
   values.push_back( to_string( Allow_Restriction_Field( ) ) );
   values.push_back( to_string( Allow_Restriction_Spec( ) ) );
   values.push_back( to_string( Allow_Restriction_Value( ) ) );
   values.push_back( to_string( Allow_Search_Option_Limit( ) ) );
   values.push_back( to_string( Allow_Select_Specifics( ) ) );
   values.push_back( to_string( Allow_Sort_Manually( ) ) );
   values.push_back( to_string( Allow_View_Parent_Extra( ) ) );
   values.push_back( to_string( Is_Restrict_Search( ) ) );
   values.push_back( sql_quote( to_string( List_Field_Name( ) ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( to_string( Needs_Restriction_Field( ) ) );
   values.push_back( to_string( Needs_Source( ) ) );
   values.push_back( to_string( Needs_Switch_Type( ) ) );
   values.push_back( to_string( Non_Simple_Field( ) ) );
   values.push_back( to_string( Trivial_Field_Only( ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_List_Field_Type::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_List_Field_Type::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_List_Field_Type::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_List_Field_Type::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_List_Field_Type::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_List_Field_Type::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_List_Field_Type::static_lock_class_id( )
{
   return "123100";
}

const char* Meta_List_Field_Type::static_check_class_name( )
{
   return "List_Field_Type";
}

const char* Meta_List_Field_Type::static_persistence_extra( )
{
   return "";
}

bool Meta_List_Field_Type::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_List_Field_Type::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.123100" );
}

void Meta_List_Field_Type::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "123113", "Allow_Child_Rel_Select_Specifics", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123106", "Allow_Exclude_In_Use_FK", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123121", "Allow_Link_Empty_Restriction", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123119", "Allow_Link_Permission", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123112", "Allow_Link_Specifics", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123107", "Allow_Link_Type", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123108", "Allow_Procedure", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123116", "Allow_Restriction_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123103", "Allow_Restriction_Spec", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123104", "Allow_Restriction_Value", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123115", "Allow_Search_Option_Limit", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123111", "Allow_Select_Specifics", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123122", "Allow_Sort_Manually", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123120", "Allow_View_Parent_Extra", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123118", "Is_Restrict_Search", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123102", "List_Field_Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "123101", "Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "123117", "Needs_Restriction_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123114", "Needs_Source", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123105", "Needs_Switch_Type", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123109", "Non_Simple_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "123110", "Trivial_Field_Only", "bool", false, "", "" ) );
}

void Meta_List_Field_Type::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;
}

int Meta_List_Field_Type::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_List_Field_Type::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_List_Field_Type::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_List_Field_Type::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "123113";
      break;

      case 2:
      p_id = "123106";
      break;

      case 3:
      p_id = "123121";
      break;

      case 4:
      p_id = "123119";
      break;

      case 5:
      p_id = "123112";
      break;

      case 6:
      p_id = "123107";
      break;

      case 7:
      p_id = "123108";
      break;

      case 8:
      p_id = "123116";
      break;

      case 9:
      p_id = "123103";
      break;

      case 10:
      p_id = "123104";
      break;

      case 11:
      p_id = "123115";
      break;

      case 12:
      p_id = "123111";
      break;

      case 13:
      p_id = "123122";
      break;

      case 14:
      p_id = "123120";
      break;

      case 15:
      p_id = "123118";
      break;

      case 16:
      p_id = "123102";
      break;

      case 17:
      p_id = "123101";
      break;

      case 18:
      p_id = "123117";
      break;

      case 19:
      p_id = "123114";
      break;

      case 20:
      p_id = "123105";
      break;

      case 21:
      p_id = "123109";
      break;

      case 22:
      p_id = "123110";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class List_Field_Type" );

   return p_id;
}

const char* Meta_List_Field_Type::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Allow_Child_Rel_Select_Specifics";
      break;

      case 2:
      p_id = "Allow_Exclude_In_Use_FK";
      break;

      case 3:
      p_id = "Allow_Link_Empty_Restriction";
      break;

      case 4:
      p_id = "Allow_Link_Permission";
      break;

      case 5:
      p_id = "Allow_Link_Specifics";
      break;

      case 6:
      p_id = "Allow_Link_Type";
      break;

      case 7:
      p_id = "Allow_Procedure";
      break;

      case 8:
      p_id = "Allow_Restriction_Field";
      break;

      case 9:
      p_id = "Allow_Restriction_Spec";
      break;

      case 10:
      p_id = "Allow_Restriction_Value";
      break;

      case 11:
      p_id = "Allow_Search_Option_Limit";
      break;

      case 12:
      p_id = "Allow_Select_Specifics";
      break;

      case 13:
      p_id = "Allow_Sort_Manually";
      break;

      case 14:
      p_id = "Allow_View_Parent_Extra";
      break;

      case 15:
      p_id = "Is_Restrict_Search";
      break;

      case 16:
      p_id = "List_Field_Name";
      break;

      case 17:
      p_id = "Name";
      break;

      case 18:
      p_id = "Needs_Restriction_Field";
      break;

      case 19:
      p_id = "Needs_Source";
      break;

      case 20:
      p_id = "Needs_Switch_Type";
      break;

      case 21:
      p_id = "Non_Simple_Field";
      break;

      case 22:
      p_id = "Trivial_Field_Only";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class List_Field_Type" );

   return p_id;
}

int Meta_List_Field_Type::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Allow_Child_Rel_Select_Specifics || field == c_field_name_Allow_Child_Rel_Select_Specifics )
      rc += 1;
   else if( field == c_field_id_Allow_Exclude_In_Use_FK || field == c_field_name_Allow_Exclude_In_Use_FK )
      rc += 2;
   else if( field == c_field_id_Allow_Link_Empty_Restriction || field == c_field_name_Allow_Link_Empty_Restriction )
      rc += 3;
   else if( field == c_field_id_Allow_Link_Permission || field == c_field_name_Allow_Link_Permission )
      rc += 4;
   else if( field == c_field_id_Allow_Link_Specifics || field == c_field_name_Allow_Link_Specifics )
      rc += 5;
   else if( field == c_field_id_Allow_Link_Type || field == c_field_name_Allow_Link_Type )
      rc += 6;
   else if( field == c_field_id_Allow_Procedure || field == c_field_name_Allow_Procedure )
      rc += 7;
   else if( field == c_field_id_Allow_Restriction_Field || field == c_field_name_Allow_Restriction_Field )
      rc += 8;
   else if( field == c_field_id_Allow_Restriction_Spec || field == c_field_name_Allow_Restriction_Spec )
      rc += 9;
   else if( field == c_field_id_Allow_Restriction_Value || field == c_field_name_Allow_Restriction_Value )
      rc += 10;
   else if( field == c_field_id_Allow_Search_Option_Limit || field == c_field_name_Allow_Search_Option_Limit )
      rc += 11;
   else if( field == c_field_id_Allow_Select_Specifics || field == c_field_name_Allow_Select_Specifics )
      rc += 12;
   else if( field == c_field_id_Allow_Sort_Manually || field == c_field_name_Allow_Sort_Manually )
      rc += 13;
   else if( field == c_field_id_Allow_View_Parent_Extra || field == c_field_name_Allow_View_Parent_Extra )
      rc += 14;
   else if( field == c_field_id_Is_Restrict_Search || field == c_field_name_Is_Restrict_Search )
      rc += 15;
   else if( field == c_field_id_List_Field_Name || field == c_field_name_List_Field_Name )
      rc += 16;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 17;
   else if( field == c_field_id_Needs_Restriction_Field || field == c_field_name_Needs_Restriction_Field )
      rc += 18;
   else if( field == c_field_id_Needs_Source || field == c_field_name_Needs_Source )
      rc += 19;
   else if( field == c_field_id_Needs_Switch_Type || field == c_field_name_Needs_Switch_Type )
      rc += 20;
   else if( field == c_field_id_Non_Simple_Field || field == c_field_name_Non_Simple_Field )
      rc += 21;
   else if( field == c_field_id_Trivial_Field_Only || field == c_field_name_Trivial_Field_Only )
      rc += 22;

   return rc - 1;
}

procedure_info_container& Meta_List_Field_Type::static_get_procedure_info( )
{
   static procedure_info_container procedures;

   return procedures;
}

string Meta_List_Field_Type::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(75),"
    "C_Ver_ SMALLINT UNSIGNED NOT NULL,"
    "C_Rev_ BIGINT UNSIGNED NOT NULL,"
    "C_Sec_ BIGINT UNSIGNED NOT NULL,"
    "C_Typ_ VARCHAR(16) NOT NULL,"
    "C_Allow_Child_Rel_Select_Specifics INTEGER NOT NULL,"
    "C_Allow_Exclude_In_Use_FK INTEGER NOT NULL,"
    "C_Allow_Link_Empty_Restriction INTEGER NOT NULL,"
    "C_Allow_Link_Permission INTEGER NOT NULL,"
    "C_Allow_Link_Specifics INTEGER NOT NULL,"
    "C_Allow_Link_Type INTEGER NOT NULL,"
    "C_Allow_Procedure INTEGER NOT NULL,"
    "C_Allow_Restriction_Field INTEGER NOT NULL,"
    "C_Allow_Restriction_Spec INTEGER NOT NULL,"
    "C_Allow_Restriction_Value INTEGER NOT NULL,"
    "C_Allow_Search_Option_Limit INTEGER NOT NULL,"
    "C_Allow_Select_Specifics INTEGER NOT NULL,"
    "C_Allow_Sort_Manually INTEGER NOT NULL,"
    "C_Allow_View_Parent_Extra INTEGER NOT NULL,"
    "C_Is_Restrict_Search INTEGER NOT NULL,"
    "C_List_Field_Name VARCHAR(200) NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Needs_Restriction_Field INTEGER NOT NULL,"
    "C_Needs_Source INTEGER NOT NULL,"
    "C_Needs_Switch_Type INTEGER NOT NULL,"
    "C_Non_Simple_Field INTEGER NOT NULL,"
    "C_Trivial_Field_Only INTEGER NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_List_Field_Type::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_List_Field_Type::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   ( void )pairs;
}

void Meta_List_Field_Type::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "Name", "string" ) );
}

void Meta_List_Field_Type::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "Name" );
}

void Meta_List_Field_Type::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Name" );
}

void Meta_List_Field_Type::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Name" );
}

void Meta_List_Field_Type::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_List_Field_Type::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_List_Field_Type::static_insert_external_alias( const string& module_and_class_id, Meta_List_Field_Type* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_List_Field_Type::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_List_Field_Type::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_List_Field_Type::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
