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

#include "Meta_Relationship.h"

#include "Meta_List_Field.h"
#include "Meta_Specification.h"
#include "Meta_Permission.h"
#include "Meta_Class.h"
#include "Meta_Model.h"
#include "Meta_Field.h"
#include "Meta_Relationship.h"

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

// [(start meta_relationship_field)] 600095
#include "Meta_Field.h"
#include "Meta_Workgroup.h"
// [(finish meta_relationship_field)] 600095

// [(start modifier_field_null)] 640101
#include "Meta_View.h"
// [(finish modifier_field_null)] 640101

// [(start modifier_field_null)] 640102
#include "Meta_List.h"
// [(finish modifier_field_null)] 640102

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

template< > inline string to_string( const Meta_Model& c )
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

inline void from_string( Meta_Permission& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Class& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Model& c, const string& s )
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

inline int system( const string& cmd ) { return exec_system( cmd ); }

namespace
{

template< typename T > inline void sanity_check( const T& t ) { }

inline void sanity_check( const string& s )
{
   if( s.length( ) > c_max_string_length_limit )
      throw runtime_error( "unexpected max string length limit exceeded with: " + s );
}

#include "Meta_Relationship.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Access_Permission = "301330c";
const char* const c_field_id_Access_Restriction = "113114";
const char* const c_field_id_Access_Scope = "113112";
const char* const c_field_id_Cascade_Op = "113103";
const char* const c_field_id_Change_Permission = "301330d";
const char* const c_field_id_Change_Restriction = "113115";
const char* const c_field_id_Change_Scope = "113113";
const char* const c_field_id_Child_Class = "301310";
const char* const c_field_id_Child_Class_Id = "113116";
const char* const c_field_id_Child_Class_Name = "113108";
const char* const c_field_id_Child_Name = "113105";
const char* const c_field_id_Extra = "113104";
const char* const c_field_id_Field_Id = "113106";
const char* const c_field_id_Field_Key = "113107";
const char* const c_field_id_Internal = "113110";
const char* const c_field_id_Mandatory = "113102";
const char* const c_field_id_Model = "301300";
const char* const c_field_id_Name = "113101";
const char* const c_field_id_Parent_Class = "301320";
const char* const c_field_id_Parent_Field_For_List = "301330b";
const char* const c_field_id_Parent_Field_For_View = "301330a";
const char* const c_field_id_Source_Relationship = "301330";
const char* const c_field_id_Transient = "113111";

const char* const c_field_name_Access_Permission = "Access_Permission";
const char* const c_field_name_Access_Restriction = "Access_Restriction";
const char* const c_field_name_Access_Scope = "Access_Scope";
const char* const c_field_name_Cascade_Op = "Cascade_Op";
const char* const c_field_name_Change_Permission = "Change_Permission";
const char* const c_field_name_Change_Restriction = "Change_Restriction";
const char* const c_field_name_Change_Scope = "Change_Scope";
const char* const c_field_name_Child_Class = "Child_Class";
const char* const c_field_name_Child_Class_Id = "Child_Class_Id";
const char* const c_field_name_Child_Class_Name = "Child_Class_Name";
const char* const c_field_name_Child_Name = "Child_Name";
const char* const c_field_name_Extra = "Extra";
const char* const c_field_name_Field_Id = "Field_Id";
const char* const c_field_name_Field_Key = "Field_Key";
const char* const c_field_name_Internal = "Internal";
const char* const c_field_name_Mandatory = "Mandatory";
const char* const c_field_name_Model = "Model";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Parent_Class = "Parent_Class";
const char* const c_field_name_Parent_Field_For_List = "Parent_Field_For_List";
const char* const c_field_name_Parent_Field_For_View = "Parent_Field_For_View";
const char* const c_field_name_Source_Relationship = "Source_Relationship";
const char* const c_field_name_Transient = "Transient";

const char* const c_field_display_name_Access_Permission = "field_relationship_access_permission";
const char* const c_field_display_name_Access_Restriction = "field_relationship_access_restriction";
const char* const c_field_display_name_Access_Scope = "field_relationship_access_scope";
const char* const c_field_display_name_Cascade_Op = "field_relationship_cascade_op";
const char* const c_field_display_name_Change_Permission = "field_relationship_change_permission";
const char* const c_field_display_name_Change_Restriction = "field_relationship_change_restriction";
const char* const c_field_display_name_Change_Scope = "field_relationship_change_scope";
const char* const c_field_display_name_Child_Class = "field_relationship_child_class";
const char* const c_field_display_name_Child_Class_Id = "field_relationship_child_class_id";
const char* const c_field_display_name_Child_Class_Name = "field_relationship_child_class_name";
const char* const c_field_display_name_Child_Name = "field_relationship_child_name";
const char* const c_field_display_name_Extra = "field_relationship_extra";
const char* const c_field_display_name_Field_Id = "field_relationship_field_id";
const char* const c_field_display_name_Field_Key = "field_relationship_field_key";
const char* const c_field_display_name_Internal = "field_relationship_internal";
const char* const c_field_display_name_Mandatory = "field_relationship_mandatory";
const char* const c_field_display_name_Model = "field_relationship_model";
const char* const c_field_display_name_Name = "field_relationship_name";
const char* const c_field_display_name_Parent_Class = "field_relationship_parent_class";
const char* const c_field_display_name_Parent_Field_For_List = "field_relationship_parent_field_for_list";
const char* const c_field_display_name_Parent_Field_For_View = "field_relationship_parent_field_for_view";
const char* const c_field_display_name_Source_Relationship = "field_relationship_source_relationship";
const char* const c_field_display_name_Transient = "field_relationship_transient";

const int c_num_fields = 23;

const char* const c_all_sorted_field_ids[ ] =
{
   "113101",
   "113102",
   "113103",
   "113104",
   "113105",
   "113106",
   "113107",
   "113108",
   "113110",
   "113111",
   "113112",
   "113113",
   "113114",
   "113115",
   "113116",
   "301300",
   "301310",
   "301320",
   "301330",
   "301330a",
   "301330b",
   "301330c",
   "301330d"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Access_Permission",
   "Access_Restriction",
   "Access_Scope",
   "Cascade_Op",
   "Change_Permission",
   "Change_Restriction",
   "Change_Scope",
   "Child_Class",
   "Child_Class_Id",
   "Child_Class_Name",
   "Child_Name",
   "Extra",
   "Field_Id",
   "Field_Key",
   "Internal",
   "Mandatory",
   "Model",
   "Name",
   "Parent_Class",
   "Parent_Field_For_List",
   "Parent_Field_For_View",
   "Source_Relationship",
   "Transient"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool has_field( const string& field )
{
   return binary_search( c_all_sorted_field_ids, c_all_sorted_field_ids + c_num_fields, field.c_str( ), compare )
    || binary_search( c_all_sorted_field_names, c_all_sorted_field_names + c_num_fields, field.c_str( ), compare );
}

const int c_num_encrypted_fields = 0;

bool is_encrypted_field( const string& ) { static bool false_value( false ); return false_value; }

const int c_num_transient_fields = 5;

const char* const c_transient_sorted_field_ids[ ] =
{
   "113105",
   "113108",
   "113116",
   "301330a",
   "301330b"
};

const char* const c_transient_sorted_field_names[ ] =
{
   "Child_Class_Id",
   "Child_Class_Name",
   "Child_Name",
   "Parent_Field_For_List",
   "Parent_Field_For_View"
};

inline bool is_transient_field( const string& field )
{
   return binary_search( c_transient_sorted_field_ids,
    c_transient_sorted_field_ids + c_num_transient_fields, field.c_str( ), compare )
    || binary_search( c_transient_sorted_field_names,
    c_transient_sorted_field_names + c_num_transient_fields, field.c_str( ), compare );
}

const uint64_t c_modifier_Hide_Parent_Field_For_List = UINT64_C( 0x100 );
const uint64_t c_modifier_Hide_Parent_Field_For_View = UINT64_C( 0x200 );
const uint64_t c_modifier_Is_Internal = UINT64_C( 0x400 );
const uint64_t c_modifier_Is_Transient = UINT64_C( 0x800 );

aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Child_Class_Id_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Child_Class_Name_domain;
domain_string_max_size< 100 > g_Child_Name_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Field_Id_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Name_domain;

string g_group_field_name;
string g_level_field_name;
string g_order_field_name;
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_Relationship* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Relationship* > external_aliases_lookup_container;
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
int g_default_Cascade_Op = int( 0 );
string g_default_Change_Permission = string( );
int g_default_Change_Restriction = int( 0 );
int g_default_Change_Scope = int( 0 );
string g_default_Child_Class = string( );
string g_default_Child_Class_Id = string( );
string g_default_Child_Class_Name = string( );
string g_default_Child_Name = string( );
int g_default_Extra = int( 0 );
string g_default_Field_Id = string( );
string g_default_Field_Key = string( );
bool g_default_Internal = bool( 0 );
bool g_default_Mandatory = bool( 1 );
string g_default_Model = string( );
string g_default_Name = string( );
string g_default_Parent_Class = string( );
string g_default_Parent_Field_For_List = string( );
string g_default_Parent_Field_For_View = string( );
string g_default_Source_Relationship = string( );
bool g_default_Transient = bool( 0 );

set< int > g_view_access_restrict_enum;
set< int > g_view_field_access_scope_enum;
set< int > g_cascade_op_enum;
set< int > g_view_change_restrict_enum;
set< int > g_view_field_change_scope_enum;
set< int > g_relationship_extra_enum;

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

const int c_enum_cascade_op_restrict( 0 );
const int c_enum_cascade_op_unlink( 1 );
const int c_enum_cascade_op_destroy( 2 );
const int c_enum_cascade_op_not_applicable( -1 );

string get_enum_string_cascade_op( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for cascade_op" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_cascade_op_restrict";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_cascade_op_unlink";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_cascade_op_destroy";
   else if( to_string( val ) == to_string( "-1" ) )
      string_name = "enum_cascade_op_not_applicable";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for cascade_op" );

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

const int c_enum_relationship_extra_none( 0 );
const int c_enum_relationship_extra_user_group( 2 );
const int c_enum_relationship_extra_user_other( 3 );
const int c_enum_relationship_extra_owning_user( 1 );
const int c_enum_relationship_extra_create_user_key( 4 );
const int c_enum_relationship_extra_modify_user_key( 5 );

string get_enum_string_relationship_extra( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for relationship_extra" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_relationship_extra_none";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_relationship_extra_user_group";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_relationship_extra_user_other";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_relationship_extra_owning_user";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_relationship_extra_create_user_key";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_relationship_extra_modify_user_key";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for relationship_extra" );

   return get_module_string( lower( string_name ) );
}

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Relationship > Relationship_registration( get_class_registry( ), "113100" );

class Meta_Relationship_command_functor;

class Meta_Relationship_command_handler : public command_handler
{
   friend class Meta_Relationship_command_functor;

   public:
   Meta_Relationship_command_handler( ) : p_Meta_Relationship( 0 ) { }

   void set_Meta_Relationship( Meta_Relationship* p_new_Meta_Relationship ) { p_Meta_Relationship = p_new_Meta_Relationship; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Relationship* p_Meta_Relationship;

   protected:
   string retval;
};

class Meta_Relationship_command_functor : public command_functor
{
   public:
   Meta_Relationship_command_functor( Meta_Relationship_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Relationship_command_handler& cmd_handler;
};

command_functor* Meta_Relationship_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Relationship_command_functor( dynamic_cast< Meta_Relationship_command_handler& >( handler ) );
}

void Meta_Relationship_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Relationship_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_Relationship_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Relationship->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Relationship->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Relationship_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Relationship->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Relationship->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Relationship_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Relationship_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_Relationship->Access_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Restriction ) || ( field_name == c_field_name_Access_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Relationship->Access_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Scope ) || ( field_name == c_field_name_Access_Scope ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Relationship->Access_Scope( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Cascade_Op ) || ( field_name == c_field_name_Cascade_Op ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Relationship->Cascade_Op( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Permission ) || ( field_name == c_field_name_Change_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_Relationship->Change_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Restriction ) || ( field_name == c_field_name_Change_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Relationship->Change_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Scope ) || ( field_name == c_field_name_Change_Scope ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Relationship->Change_Scope( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Class ) || ( field_name == c_field_name_Child_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Relationship->Child_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Class_Id ) || ( field_name == c_field_name_Child_Class_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Relationship->Child_Class_Id( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Class_Name ) || ( field_name == c_field_name_Child_Class_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Relationship->Child_Class_Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Name ) || ( field_name == c_field_name_Child_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Relationship->Child_Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Extra ) || ( field_name == c_field_name_Extra ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Relationship->Extra( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Field_Id ) || ( field_name == c_field_name_Field_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Relationship->Field_Id( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Field_Key ) || ( field_name == c_field_name_Field_Key ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Relationship->Field_Key( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Internal ) || ( field_name == c_field_name_Internal ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Relationship->Internal( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Mandatory ) || ( field_name == c_field_name_Mandatory ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Relationship->Mandatory( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) ) )
      {
         handled = true;

         string_getter< Meta_Model >( cmd_handler.p_Meta_Relationship->Model( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Relationship->Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Class ) || ( field_name == c_field_name_Parent_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Relationship->Parent_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Field_For_List ) || ( field_name == c_field_name_Parent_Field_For_List ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Relationship->Parent_Field_For_List( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Field_For_View ) || ( field_name == c_field_name_Parent_Field_For_View ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Relationship->Parent_Field_For_View( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Relationship ) || ( field_name == c_field_name_Source_Relationship ) ) )
      {
         handled = true;

         string_getter< Meta_Relationship >( cmd_handler.p_Meta_Relationship->Source_Relationship( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Transient ) || ( field_name == c_field_name_Transient ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Relationship->Transient( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Relationship_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Relationship_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_Relationship_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, Meta_Permission >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Access_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Restriction ) || ( field_name == c_field_name_Access_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, int >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Access_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Scope ) || ( field_name == c_field_name_Access_Scope ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, int >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Access_Scope, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Cascade_Op ) || ( field_name == c_field_name_Cascade_Op ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, int >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Cascade_Op, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Permission ) || ( field_name == c_field_name_Change_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, Meta_Permission >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Change_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Restriction ) || ( field_name == c_field_name_Change_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, int >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Change_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Scope ) || ( field_name == c_field_name_Change_Scope ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, int >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Change_Scope, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Class ) || ( field_name == c_field_name_Child_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, Meta_Class >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Child_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Class_Id ) || ( field_name == c_field_name_Child_Class_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, string >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Child_Class_Id, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Class_Name ) || ( field_name == c_field_name_Child_Class_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, string >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Child_Class_Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Name ) || ( field_name == c_field_name_Child_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, string >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Child_Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Extra ) || ( field_name == c_field_name_Extra ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, int >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Extra, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Field_Id ) || ( field_name == c_field_name_Field_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, string >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Field_Id, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Field_Key ) || ( field_name == c_field_name_Field_Key ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, string >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Field_Key, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Internal ) || ( field_name == c_field_name_Internal ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, bool >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Internal, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Mandatory ) || ( field_name == c_field_name_Mandatory ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, bool >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Mandatory, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, Meta_Model >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Model, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, string >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Class ) || ( field_name == c_field_name_Parent_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, Meta_Class >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Parent_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Field_For_List ) || ( field_name == c_field_name_Parent_Field_For_List ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, Meta_Field >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Parent_Field_For_List, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Field_For_View ) || ( field_name == c_field_name_Parent_Field_For_View ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, Meta_Field >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Parent_Field_For_View, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Relationship ) || ( field_name == c_field_name_Source_Relationship ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, Meta_Relationship >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Source_Relationship, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Transient ) || ( field_name == c_field_name_Transient ) ) )
      {
         handled = true;

         func_string_setter< Meta_Relationship, bool >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Transient, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Relationship_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Relationship_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_Relationship_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_Relationship->Access_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Change_Permission ) || ( field_name == c_field_name_Change_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_Relationship->Change_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Child_Class ) || ( field_name == c_field_name_Child_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Relationship->Child_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) )
         cmd_handler.retval = cmd_handler.p_Meta_Relationship->Model( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Parent_Class ) || ( field_name == c_field_name_Parent_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Relationship->Parent_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Parent_Field_For_List ) || ( field_name == c_field_name_Parent_Field_For_List ) )
         cmd_handler.retval = cmd_handler.p_Meta_Relationship->Parent_Field_For_List( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Parent_Field_For_View ) || ( field_name == c_field_name_Parent_Field_For_View ) )
         cmd_handler.retval = cmd_handler.p_Meta_Relationship->Parent_Field_For_View( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Relationship ) || ( field_name == c_field_name_Source_Relationship ) )
         cmd_handler.retval = cmd_handler.p_Meta_Relationship->Source_Relationship( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
}

struct Meta_Relationship::impl : public Meta_Relationship_command_handler
{
   impl( Meta_Relationship& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Relationship( &o );

      add_commands( 0, Meta_Relationship_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Relationship_command_definitions ) );
   }

   Meta_Relationship& get_obj( ) const
   {
      return *cp_obj;
   }

   int impl_Access_Restriction( ) const { return lazy_fetch( p_obj ), v_Access_Restriction; }
   void impl_Access_Restriction( int Access_Restriction ) { v_Access_Restriction = Access_Restriction; }

   int impl_Access_Scope( ) const { return lazy_fetch( p_obj ), v_Access_Scope; }
   void impl_Access_Scope( int Access_Scope ) { v_Access_Scope = Access_Scope; }

   int impl_Cascade_Op( ) const { return lazy_fetch( p_obj ), v_Cascade_Op; }
   void impl_Cascade_Op( int Cascade_Op ) { v_Cascade_Op = Cascade_Op; }

   int impl_Change_Restriction( ) const { return lazy_fetch( p_obj ), v_Change_Restriction; }
   void impl_Change_Restriction( int Change_Restriction ) { v_Change_Restriction = Change_Restriction; }

   int impl_Change_Scope( ) const { return lazy_fetch( p_obj ), v_Change_Scope; }
   void impl_Change_Scope( int Change_Scope ) { v_Change_Scope = Change_Scope; }

   const string& impl_Child_Class_Id( ) const { return lazy_fetch( p_obj ), v_Child_Class_Id; }
   void impl_Child_Class_Id( const string& Child_Class_Id ) { sanity_check( Child_Class_Id ); v_Child_Class_Id = Child_Class_Id; }

   const string& impl_Child_Class_Name( ) const { return lazy_fetch( p_obj ), v_Child_Class_Name; }
   void impl_Child_Class_Name( const string& Child_Class_Name ) { sanity_check( Child_Class_Name ); v_Child_Class_Name = Child_Class_Name; }

   const string& impl_Child_Name( ) const { return lazy_fetch( p_obj ), v_Child_Name; }
   void impl_Child_Name( const string& Child_Name ) { sanity_check( Child_Name ); v_Child_Name = Child_Name; }

   int impl_Extra( ) const { return lazy_fetch( p_obj ), v_Extra; }
   void impl_Extra( int Extra ) { v_Extra = Extra; }

   const string& impl_Field_Id( ) const { return lazy_fetch( p_obj ), v_Field_Id; }
   void impl_Field_Id( const string& Field_Id ) { sanity_check( Field_Id ); v_Field_Id = Field_Id; }

   const string& impl_Field_Key( ) const { return lazy_fetch( p_obj ), v_Field_Key; }
   void impl_Field_Key( const string& Field_Key ) { sanity_check( Field_Key ); v_Field_Key = Field_Key; }

   bool impl_Internal( ) const { return lazy_fetch( p_obj ), v_Internal; }
   void impl_Internal( bool Internal ) { v_Internal = Internal; }

   bool impl_Mandatory( ) const { return lazy_fetch( p_obj ), v_Mandatory; }
   void impl_Mandatory( bool Mandatory ) { v_Mandatory = Mandatory; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { sanity_check( Name ); v_Name = Name; }

   bool impl_Transient( ) const { return lazy_fetch( p_obj ), v_Transient; }
   void impl_Transient( bool Transient ) { v_Transient = Transient; }

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

   Meta_Class& impl_Child_Class( )
   {
      if( !cp_Child_Class )
      {
         cp_Child_Class.init( );

         p_obj->setup_graph_parent( *cp_Child_Class, c_field_id_Child_Class, v_Child_Class );
      }
      return *cp_Child_Class;
   }

   const Meta_Class& impl_Child_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Child_Class )
      {
         cp_Child_Class.init( );

         p_obj->setup_graph_parent( *cp_Child_Class, c_field_id_Child_Class, v_Child_Class );
      }
      return *cp_Child_Class;
   }

   void impl_Child_Class( const string& key )
   {
      class_base_accessor cba( impl_Child_Class( ) );
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

   Meta_Field& impl_Parent_Field_For_List( )
   {
      if( !cp_Parent_Field_For_List )
      {
         cp_Parent_Field_For_List.init( );

         p_obj->setup_graph_parent( *cp_Parent_Field_For_List, c_field_id_Parent_Field_For_List, v_Parent_Field_For_List );
      }
      return *cp_Parent_Field_For_List;
   }

   const Meta_Field& impl_Parent_Field_For_List( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Parent_Field_For_List )
      {
         cp_Parent_Field_For_List.init( );

         p_obj->setup_graph_parent( *cp_Parent_Field_For_List, c_field_id_Parent_Field_For_List, v_Parent_Field_For_List );
      }
      return *cp_Parent_Field_For_List;
   }

   void impl_Parent_Field_For_List( const string& key )
   {
      class_base_accessor cba( impl_Parent_Field_For_List( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Parent_Field_For_View( )
   {
      if( !cp_Parent_Field_For_View )
      {
         cp_Parent_Field_For_View.init( );

         p_obj->setup_graph_parent( *cp_Parent_Field_For_View, c_field_id_Parent_Field_For_View, v_Parent_Field_For_View );
      }
      return *cp_Parent_Field_For_View;
   }

   const Meta_Field& impl_Parent_Field_For_View( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Parent_Field_For_View )
      {
         cp_Parent_Field_For_View.init( );

         p_obj->setup_graph_parent( *cp_Parent_Field_For_View, c_field_id_Parent_Field_For_View, v_Parent_Field_For_View );
      }
      return *cp_Parent_Field_For_View;
   }

   void impl_Parent_Field_For_View( const string& key )
   {
      class_base_accessor cba( impl_Parent_Field_For_View( ) );
      cba.set_key( key );
   }

   Meta_Relationship& impl_Source_Relationship( )
   {
      if( !cp_Source_Relationship )
      {
         cp_Source_Relationship.init( );

         p_obj->setup_graph_parent( *cp_Source_Relationship, c_field_id_Source_Relationship, v_Source_Relationship );
      }
      return *cp_Source_Relationship;
   }

   const Meta_Relationship& impl_Source_Relationship( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Relationship )
      {
         cp_Source_Relationship.init( );

         p_obj->setup_graph_parent( *cp_Source_Relationship, c_field_id_Source_Relationship, v_Source_Relationship );
      }
      return *cp_Source_Relationship;
   }

   void impl_Source_Relationship( const string& key )
   {
      class_base_accessor cba( impl_Source_Relationship( ) );
      cba.set_key( key );
   }

   Meta_List_Field& impl_child_List_Field_Child( )
   {
      if( !cp_child_List_Field_Child )
      {
         cp_child_List_Field_Child.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Child, "302193" );
      }
      return *cp_child_List_Field_Child;
   }

   const Meta_List_Field& impl_child_List_Field_Child( ) const
   {
      if( !cp_child_List_Field_Child )
      {
         cp_child_List_Field_Child.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Child, "302193" );
      }
      return *cp_child_List_Field_Child;
   }

   Meta_Specification& impl_child_Specification_Child( )
   {
      if( !cp_child_Specification_Child )
      {
         cp_child_Specification_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Child, "301475" );
      }
      return *cp_child_Specification_Child;
   }

   const Meta_Specification& impl_child_Specification_Child( ) const
   {
      if( !cp_child_Specification_Child )
      {
         cp_child_Specification_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Child, "301475" );
      }
      return *cp_child_Specification_Child;
   }

   Meta_Relationship& impl_child_Relationship_Source( )
   {
      if( !cp_child_Relationship_Source )
      {
         cp_child_Relationship_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Relationship_Source, "301330" );
      }
      return *cp_child_Relationship_Source;
   }

   const Meta_Relationship& impl_child_Relationship_Source( ) const
   {
      if( !cp_child_Relationship_Source )
      {
         cp_child_Relationship_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Relationship_Source, "301330" );
      }
      return *cp_child_Relationship_Source;
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

   Meta_Relationship* p_obj;
   class_pointer< Meta_Relationship > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   int v_Access_Restriction;
   int v_Access_Scope;
   int v_Cascade_Op;
   int v_Change_Restriction;
   int v_Change_Scope;
   string v_Child_Class_Id;
   string v_Child_Class_Name;
   string v_Child_Name;
   int v_Extra;
   string v_Field_Id;
   string v_Field_Key;
   bool v_Internal;
   bool v_Mandatory;
   string v_Name;
   bool v_Transient;

   string v_Access_Permission;
   mutable class_pointer< Meta_Permission > cp_Access_Permission;

   string v_Change_Permission;
   mutable class_pointer< Meta_Permission > cp_Change_Permission;

   string v_Child_Class;
   mutable class_pointer< Meta_Class > cp_Child_Class;

   string v_Model;
   mutable class_pointer< Meta_Model > cp_Model;

   string v_Parent_Class;
   mutable class_pointer< Meta_Class > cp_Parent_Class;

   string v_Parent_Field_For_List;
   mutable class_pointer< Meta_Field > cp_Parent_Field_For_List;

   string v_Parent_Field_For_View;
   mutable class_pointer< Meta_Field > cp_Parent_Field_For_View;

   string v_Source_Relationship;
   mutable class_pointer< Meta_Relationship > cp_Source_Relationship;

   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Child;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Child;
   mutable class_pointer< Meta_Relationship > cp_child_Relationship_Source;
};

string Meta_Relationship::impl::get_field_value( int field ) const
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
      retval = to_string( impl_Cascade_Op( ) );
      break;

      case 4:
      retval = to_string( impl_Change_Permission( ) );
      break;

      case 5:
      retval = to_string( impl_Change_Restriction( ) );
      break;

      case 6:
      retval = to_string( impl_Change_Scope( ) );
      break;

      case 7:
      retval = to_string( impl_Child_Class( ) );
      break;

      case 8:
      retval = to_string( impl_Child_Class_Id( ) );
      break;

      case 9:
      retval = to_string( impl_Child_Class_Name( ) );
      break;

      case 10:
      retval = to_string( impl_Child_Name( ) );
      break;

      case 11:
      retval = to_string( impl_Extra( ) );
      break;

      case 12:
      retval = to_string( impl_Field_Id( ) );
      break;

      case 13:
      retval = to_string( impl_Field_Key( ) );
      break;

      case 14:
      retval = to_string( impl_Internal( ) );
      break;

      case 15:
      retval = to_string( impl_Mandatory( ) );
      break;

      case 16:
      retval = to_string( impl_Model( ) );
      break;

      case 17:
      retval = to_string( impl_Name( ) );
      break;

      case 18:
      retval = to_string( impl_Parent_Class( ) );
      break;

      case 19:
      retval = to_string( impl_Parent_Field_For_List( ) );
      break;

      case 20:
      retval = to_string( impl_Parent_Field_For_View( ) );
      break;

      case 21:
      retval = to_string( impl_Source_Relationship( ) );
      break;

      case 22:
      retval = to_string( impl_Transient( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_Relationship::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Relationship::impl, Meta_Permission >( *this, &Meta_Relationship::impl::impl_Access_Permission, value );
      break;

      case 1:
      func_string_setter< Meta_Relationship::impl, int >( *this, &Meta_Relationship::impl::impl_Access_Restriction, value );
      break;

      case 2:
      func_string_setter< Meta_Relationship::impl, int >( *this, &Meta_Relationship::impl::impl_Access_Scope, value );
      break;

      case 3:
      func_string_setter< Meta_Relationship::impl, int >( *this, &Meta_Relationship::impl::impl_Cascade_Op, value );
      break;

      case 4:
      func_string_setter< Meta_Relationship::impl, Meta_Permission >( *this, &Meta_Relationship::impl::impl_Change_Permission, value );
      break;

      case 5:
      func_string_setter< Meta_Relationship::impl, int >( *this, &Meta_Relationship::impl::impl_Change_Restriction, value );
      break;

      case 6:
      func_string_setter< Meta_Relationship::impl, int >( *this, &Meta_Relationship::impl::impl_Change_Scope, value );
      break;

      case 7:
      func_string_setter< Meta_Relationship::impl, Meta_Class >( *this, &Meta_Relationship::impl::impl_Child_Class, value );
      break;

      case 8:
      func_string_setter< Meta_Relationship::impl, string >( *this, &Meta_Relationship::impl::impl_Child_Class_Id, value );
      break;

      case 9:
      func_string_setter< Meta_Relationship::impl, string >( *this, &Meta_Relationship::impl::impl_Child_Class_Name, value );
      break;

      case 10:
      func_string_setter< Meta_Relationship::impl, string >( *this, &Meta_Relationship::impl::impl_Child_Name, value );
      break;

      case 11:
      func_string_setter< Meta_Relationship::impl, int >( *this, &Meta_Relationship::impl::impl_Extra, value );
      break;

      case 12:
      func_string_setter< Meta_Relationship::impl, string >( *this, &Meta_Relationship::impl::impl_Field_Id, value );
      break;

      case 13:
      func_string_setter< Meta_Relationship::impl, string >( *this, &Meta_Relationship::impl::impl_Field_Key, value );
      break;

      case 14:
      func_string_setter< Meta_Relationship::impl, bool >( *this, &Meta_Relationship::impl::impl_Internal, value );
      break;

      case 15:
      func_string_setter< Meta_Relationship::impl, bool >( *this, &Meta_Relationship::impl::impl_Mandatory, value );
      break;

      case 16:
      func_string_setter< Meta_Relationship::impl, Meta_Model >( *this, &Meta_Relationship::impl::impl_Model, value );
      break;

      case 17:
      func_string_setter< Meta_Relationship::impl, string >( *this, &Meta_Relationship::impl::impl_Name, value );
      break;

      case 18:
      func_string_setter< Meta_Relationship::impl, Meta_Class >( *this, &Meta_Relationship::impl::impl_Parent_Class, value );
      break;

      case 19:
      func_string_setter< Meta_Relationship::impl, Meta_Field >( *this, &Meta_Relationship::impl::impl_Parent_Field_For_List, value );
      break;

      case 20:
      func_string_setter< Meta_Relationship::impl, Meta_Field >( *this, &Meta_Relationship::impl::impl_Parent_Field_For_View, value );
      break;

      case 21:
      func_string_setter< Meta_Relationship::impl, Meta_Relationship >( *this, &Meta_Relationship::impl::impl_Source_Relationship, value );
      break;

      case 22:
      func_string_setter< Meta_Relationship::impl, bool >( *this, &Meta_Relationship::impl::impl_Transient, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_Relationship::impl::set_field_default( int field )
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
      impl_Cascade_Op( g_default_Cascade_Op );
      break;

      case 4:
      impl_Change_Permission( g_default_Change_Permission );
      break;

      case 5:
      impl_Change_Restriction( g_default_Change_Restriction );
      break;

      case 6:
      impl_Change_Scope( g_default_Change_Scope );
      break;

      case 7:
      impl_Child_Class( g_default_Child_Class );
      break;

      case 8:
      impl_Child_Class_Id( g_default_Child_Class_Id );
      break;

      case 9:
      impl_Child_Class_Name( g_default_Child_Class_Name );
      break;

      case 10:
      impl_Child_Name( g_default_Child_Name );
      break;

      case 11:
      impl_Extra( g_default_Extra );
      break;

      case 12:
      impl_Field_Id( g_default_Field_Id );
      break;

      case 13:
      impl_Field_Key( g_default_Field_Key );
      break;

      case 14:
      impl_Internal( g_default_Internal );
      break;

      case 15:
      impl_Mandatory( g_default_Mandatory );
      break;

      case 16:
      impl_Model( g_default_Model );
      break;

      case 17:
      impl_Name( g_default_Name );
      break;

      case 18:
      impl_Parent_Class( g_default_Parent_Class );
      break;

      case 19:
      impl_Parent_Field_For_List( g_default_Parent_Field_For_List );
      break;

      case 20:
      impl_Parent_Field_For_View( g_default_Parent_Field_For_View );
      break;

      case 21:
      impl_Source_Relationship( g_default_Source_Relationship );
      break;

      case 22:
      impl_Transient( g_default_Transient );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_Relationship::impl::is_field_default( int field ) const
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
      retval = ( v_Cascade_Op == g_default_Cascade_Op );
      break;

      case 4:
      retval = ( v_Change_Permission == g_default_Change_Permission );
      break;

      case 5:
      retval = ( v_Change_Restriction == g_default_Change_Restriction );
      break;

      case 6:
      retval = ( v_Change_Scope == g_default_Change_Scope );
      break;

      case 7:
      retval = ( v_Child_Class == g_default_Child_Class );
      break;

      case 8:
      retval = ( v_Child_Class_Id == g_default_Child_Class_Id );
      break;

      case 9:
      retval = ( v_Child_Class_Name == g_default_Child_Class_Name );
      break;

      case 10:
      retval = ( v_Child_Name == g_default_Child_Name );
      break;

      case 11:
      retval = ( v_Extra == g_default_Extra );
      break;

      case 12:
      retval = ( v_Field_Id == g_default_Field_Id );
      break;

      case 13:
      retval = ( v_Field_Key == g_default_Field_Key );
      break;

      case 14:
      retval = ( v_Internal == g_default_Internal );
      break;

      case 15:
      retval = ( v_Mandatory == g_default_Mandatory );
      break;

      case 16:
      retval = ( v_Model == g_default_Model );
      break;

      case 17:
      retval = ( v_Name == g_default_Name );
      break;

      case 18:
      retval = ( v_Parent_Class == g_default_Parent_Class );
      break;

      case 19:
      retval = ( v_Parent_Field_For_List == g_default_Parent_Field_For_List );
      break;

      case 20:
      retval = ( v_Parent_Field_For_View == g_default_Parent_Field_For_View );
      break;

      case 21:
      retval = ( v_Source_Relationship == g_default_Source_Relationship );
      break;

      case 22:
      retval = ( v_Transient == g_default_Transient );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_Relationship::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start modifier_field_value)] 610100
   if( get_obj( ).Internal( ) == true )
      state |= c_modifier_Is_Internal;
   // [(finish modifier_field_value)] 610100

   // [(start protect_equal)] 620100
   if( check_equal( get_obj( ).Internal( ), true ) )
      state |= ( c_state_uneditable | c_state_undeletable );
   // [(finish protect_equal)] 620100

   // [(start modifier_field_value)] 640100
   if( get_obj( ).Transient( ) == true )
      state |= c_modifier_Is_Transient;
   // [(finish modifier_field_value)] 640100

   // [(start modifier_field_null)] 640101
   if( is_null( get_obj( ).Child_Class( ).Created_View( ) ) )
      state |= c_modifier_Hide_Parent_Field_For_View;
   // [(finish modifier_field_null)] 640101

   // [(start modifier_field_null)] 640102
   if( is_null( get_obj( ).Child_Class( ).Created_List( ) ) )
      state |= c_modifier_Hide_Parent_Field_For_List;
   // [(finish modifier_field_null)] 640102

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

string Meta_Relationship::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   if( state & c_modifier_Hide_Parent_Field_For_List )
      state_names += "|" + string( "Hide_Parent_Field_For_List" );
   if( state & c_modifier_Hide_Parent_Field_For_View )
      state_names += "|" + string( "Hide_Parent_Field_For_View" );
   if( state & c_modifier_Is_Internal )
      state_names += "|" + string( "Is_Internal" );
   if( state & c_modifier_Is_Transient )
      state_names += "|" + string( "Is_Transient" );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_Relationship::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Relationship::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      impl_Access_Permission( "" );
   else if( ( field == c_field_id_Change_Permission ) || ( field == c_field_name_Change_Permission ) )
      impl_Change_Permission( "" );
   else if( ( field == c_field_id_Child_Class ) || ( field == c_field_name_Child_Class ) )
      impl_Child_Class( "" );
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      impl_Model( "" );
   else if( ( field == c_field_id_Parent_Class ) || ( field == c_field_name_Parent_Class ) )
      impl_Parent_Class( "" );
   else if( ( field == c_field_id_Parent_Field_For_List ) || ( field == c_field_name_Parent_Field_For_List ) )
      impl_Parent_Field_For_List( "" );
   else if( ( field == c_field_id_Parent_Field_For_View ) || ( field == c_field_name_Parent_Field_For_View ) )
      impl_Parent_Field_For_View( "" );
   else if( ( field == c_field_id_Source_Relationship ) || ( field == c_field_name_Source_Relationship ) )
      impl_Source_Relationship( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Relationship::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      v_Access_Permission = value;
   else if( ( field == c_field_id_Change_Permission ) || ( field == c_field_name_Change_Permission ) )
      v_Change_Permission = value;
   else if( ( field == c_field_id_Child_Class ) || ( field == c_field_name_Child_Class ) )
      v_Child_Class = value;
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      v_Model = value;
   else if( ( field == c_field_id_Parent_Class ) || ( field == c_field_name_Parent_Class ) )
      v_Parent_Class = value;
   else if( ( field == c_field_id_Parent_Field_For_List ) || ( field == c_field_name_Parent_Field_For_List ) )
      v_Parent_Field_For_List = value;
   else if( ( field == c_field_id_Parent_Field_For_View ) || ( field == c_field_name_Parent_Field_For_View ) )
      v_Parent_Field_For_View = value;
   else if( ( field == c_field_id_Source_Relationship ) || ( field == c_field_name_Source_Relationship ) )
      v_Source_Relationship = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Relationship::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      return v_Access_Permission;
   else if( ( field == c_field_id_Change_Permission ) || ( field == c_field_name_Change_Permission ) )
      return v_Change_Permission;
   else if( ( field == c_field_id_Child_Class ) || ( field == c_field_name_Child_Class ) )
      return v_Child_Class;
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      return v_Model;
   else if( ( field == c_field_id_Parent_Class ) || ( field == c_field_name_Parent_Class ) )
      return v_Parent_Class;
   else if( ( field == c_field_id_Parent_Field_For_List ) || ( field == c_field_name_Parent_Field_For_List ) )
      return v_Parent_Field_For_List;
   else if( ( field == c_field_id_Parent_Field_For_View ) || ( field == c_field_name_Parent_Field_For_View ) )
      return v_Parent_Field_For_View;
   else if( ( field == c_field_id_Source_Relationship ) || ( field == c_field_name_Source_Relationship ) )
      return v_Source_Relationship;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Relationship::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Access_Permission, v_Access_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Change_Permission, v_Change_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Child_Class, v_Child_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Model, v_Model ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Parent_Class, v_Parent_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Parent_Field_For_List, v_Parent_Field_For_List ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Parent_Field_For_View, v_Parent_Field_For_View ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Relationship, v_Source_Relationship ) );
}

void Meta_Relationship::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_Relationship::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Relationship::impl::clear( )
{
   v_Access_Restriction = g_default_Access_Restriction;
   v_Access_Scope = g_default_Access_Scope;
   v_Cascade_Op = g_default_Cascade_Op;
   v_Change_Restriction = g_default_Change_Restriction;
   v_Change_Scope = g_default_Change_Scope;
   v_Child_Class_Id = g_default_Child_Class_Id;
   v_Child_Class_Name = g_default_Child_Class_Name;
   v_Child_Name = g_default_Child_Name;
   v_Extra = g_default_Extra;
   v_Field_Id = g_default_Field_Id;
   v_Field_Key = g_default_Field_Key;
   v_Internal = g_default_Internal;
   v_Mandatory = g_default_Mandatory;
   v_Name = g_default_Name;
   v_Transient = g_default_Transient;

   v_Access_Permission = string( );
   if( cp_Access_Permission )
      p_obj->setup_foreign_key( *cp_Access_Permission, v_Access_Permission );

   v_Change_Permission = string( );
   if( cp_Change_Permission )
      p_obj->setup_foreign_key( *cp_Change_Permission, v_Change_Permission );

   v_Child_Class = string( );
   if( cp_Child_Class )
      p_obj->setup_foreign_key( *cp_Child_Class, v_Child_Class );

   v_Model = string( );
   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   v_Parent_Class = string( );
   if( cp_Parent_Class )
      p_obj->setup_foreign_key( *cp_Parent_Class, v_Parent_Class );

   v_Parent_Field_For_List = string( );
   if( cp_Parent_Field_For_List )
      p_obj->setup_foreign_key( *cp_Parent_Field_For_List, v_Parent_Field_For_List );

   v_Parent_Field_For_View = string( );
   if( cp_Parent_Field_For_View )
      p_obj->setup_foreign_key( *cp_Parent_Field_For_View, v_Parent_Field_For_View );

   v_Source_Relationship = string( );
   if( cp_Source_Relationship )
      p_obj->setup_foreign_key( *cp_Source_Relationship, v_Source_Relationship );
}

bool Meta_Relationship::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Relationship::impl::validate(
 uint64_t state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( v_Child_Class.empty( ) && !value_will_be_provided( c_field_name_Child_Class ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Child_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Child_Class ) ) ) ) );

   if( v_Model.empty( ) && !value_will_be_provided( c_field_name_Model ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Model,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Model ) ) ) ) );

   if( v_Parent_Class.empty( ) && !value_will_be_provided( c_field_name_Parent_Class ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Parent_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Parent_Class ) ) ) ) );

   if( !is_null( v_Child_Class_Id )
    && ( v_Child_Class_Id != g_default_Child_Class_Id
    || !value_will_be_provided( c_field_name_Child_Class_Id ) )
    && !g_Child_Class_Id_domain.is_valid( v_Child_Class_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Child_Class_Id,
       get_module_string( c_field_display_name_Child_Class_Id ) + " " + error_message ) );

   if( !is_null( v_Child_Class_Name )
    && ( v_Child_Class_Name != g_default_Child_Class_Name
    || !value_will_be_provided( c_field_name_Child_Class_Name ) )
    && !g_Child_Class_Name_domain.is_valid( v_Child_Class_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Child_Class_Name,
       get_module_string( c_field_display_name_Child_Class_Name ) + " " + error_message ) );

   if( !is_null( v_Child_Name )
    && ( v_Child_Name != g_default_Child_Name
    || !value_will_be_provided( c_field_name_Child_Name ) )
    && !g_Child_Name_domain.is_valid( v_Child_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Child_Name,
       get_module_string( c_field_display_name_Child_Name ) + " " + error_message ) );

   if( !is_null( v_Field_Id )
    && ( v_Field_Id != g_default_Field_Id
    || !value_will_be_provided( c_field_name_Field_Id ) )
    && !g_Field_Id_domain.is_valid( v_Field_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Field_Id,
       get_module_string( c_field_display_name_Field_Id ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( v_Name != g_default_Name
    || !value_will_be_provided( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   if( !g_view_access_restrict_enum.count( v_Access_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Access_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Access_Restriction ) ) ) ) );

   if( !g_view_field_access_scope_enum.count( v_Access_Scope ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Access_Scope,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Access_Scope ) ) ) ) );

   if( !g_cascade_op_enum.count( v_Cascade_Op ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Cascade_Op,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Cascade_Op ) ) ) ) );

   if( !g_view_change_restrict_enum.count( v_Change_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Change_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Change_Restriction ) ) ) ) );

   if( !g_view_field_change_scope_enum.count( v_Change_Scope ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Change_Scope,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Change_Scope ) ) ) ) );

   if( !g_relationship_extra_enum.count( v_Extra ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Extra,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Extra ) ) ) ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Relationship::impl::validate_set_fields(
 set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( !is_null( v_Child_Class_Id )
    && ( fields_set.count( c_field_id_Child_Class_Id ) || fields_set.count( c_field_name_Child_Class_Id ) )
    && !g_Child_Class_Id_domain.is_valid( v_Child_Class_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Child_Class_Id,
       get_module_string( c_field_display_name_Child_Class_Id ) + " " + error_message ) );

   if( !is_null( v_Child_Class_Name )
    && ( fields_set.count( c_field_id_Child_Class_Name ) || fields_set.count( c_field_name_Child_Class_Name ) )
    && !g_Child_Class_Name_domain.is_valid( v_Child_Class_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Child_Class_Name,
       get_module_string( c_field_display_name_Child_Class_Name ) + " " + error_message ) );

   if( !is_null( v_Child_Name )
    && ( fields_set.count( c_field_id_Child_Name ) || fields_set.count( c_field_name_Child_Name ) )
    && !g_Child_Name_domain.is_valid( v_Child_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Child_Name,
       get_module_string( c_field_display_name_Child_Name ) + " " + error_message ) );

   if( !is_null( v_Field_Id )
    && ( fields_set.count( c_field_id_Field_Id ) || fields_set.count( c_field_name_Field_Id ) )
    && !g_Field_Id_domain.is_valid( v_Field_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Field_Id,
       get_module_string( c_field_display_name_Field_Id ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( fields_set.count( c_field_id_Name ) || fields_set.count( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );
}

void Meta_Relationship::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Access_Permission )
      p_obj->setup_foreign_key( *cp_Access_Permission, v_Access_Permission );

   if( cp_Change_Permission )
      p_obj->setup_foreign_key( *cp_Change_Permission, v_Change_Permission );

   if( cp_Child_Class )
      p_obj->setup_foreign_key( *cp_Child_Class, v_Child_Class );

   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   if( cp_Parent_Class )
      p_obj->setup_foreign_key( *cp_Parent_Class, v_Parent_Class );

   if( cp_Parent_Field_For_List )
      p_obj->setup_foreign_key( *cp_Parent_Field_For_List, v_Parent_Field_For_List );

   if( cp_Parent_Field_For_View )
      p_obj->setup_foreign_key( *cp_Parent_Field_For_View, v_Parent_Field_For_View );

   if( cp_Source_Relationship )
      p_obj->setup_foreign_key( *cp_Source_Relationship, v_Source_Relationship );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start meta_relationship_child_name)] 600096
   if( get_obj( ).needs_field_value( "Child_Name" )
    || required_transients.count( "Child_Name" ) )
   {
      string str( get_obj( ).Name( ) );
      meta_relationship_child_name( str, get_obj( ).Child_Class( ).Name( ), get_obj( ).Parent_Class( ).Name( ), "_" );

      get_obj( ).Child_Name( str );
   }
   // [(finish meta_relationship_child_name)] 600096

   // [(start transient_field_alias)] 610096
   if( get_obj( ).needs_field_value( "Child_Class_Name" )
    || required_transients.count( "Child_Class_Name" ) )
      get_obj( ).Child_Class_Name( get_obj( ).Child_Class( ).Name( ) );
   // [(finish transient_field_alias)] 610096

   // [(start transient_field_alias)] 620096
   if( get_obj( ).needs_field_value( "Child_Class_Id" )
    || required_transients.count( "Child_Class_Id" ) )
      get_obj( ).Child_Class_Id( get_obj( ).Child_Class( ).Id( ) );
   // [(finish transient_field_alias)] 620096

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Relationship::impl::finalise_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Relationship::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Relationship::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_Relationship::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_from_changed_fk)] 600094
   if( get_obj( ).get_key( ).empty( ) && get_obj( ).Parent_Class( ).has_changed( ) )
      get_obj( ).Name( get_obj( ).Parent_Class( ).Name( ) );
   // [(finish field_from_changed_fk)] 600094

   // [(start default_to_global)] 600098
   if( is_create
    && get_obj( ).get_key( ).empty( )
    && get_obj( ).get_clone_key( ).empty( )
    && get_obj( ).Parent_Class( ).Type( ) == 1
    && get_obj( ).Extra( ) == g_default_Extra )
      get_obj( ).Extra( 1 );
   // [(finish default_to_global)] 600098

   // [(start default_to_global)] 600099
   if( is_create
    && get_obj( ).get_key( ).empty( )
    && get_obj( ).get_clone_key( ).empty( )
    && get_obj( ).Parent_Class( ).Type( ) == 2
    && get_obj( ).Extra( ) == g_default_Extra )
      get_obj( ).Extra( 2 );
   // [(finish default_to_global)] 600099

   // [(start default_to_global)] 600100
   if( is_create
    && get_obj( ).get_key( ).empty( )
    && get_obj( ).get_clone_key( ).empty( )
    && get_obj( ).Parent_Class( ).Type( ) == 3
    && get_obj( ).Extra( ) == g_default_Extra )
      get_obj( ).Extra( 3 );
   // [(finish default_to_global)] 600100

   // [(start field_empty_action)] 630100
   if( !get_obj( ).get_key( ).empty( ) )
   {
      if( !is_null( get_obj( ).Source_Relationship( ) ) )
         get_obj( ).Internal( true );
   }
   // [(finish field_empty_action)] 630100

   // [(start modifier_set_field)] 650100
   if( ( state & c_modifier_Is_Transient ) )
      get_obj( ).Cascade_Op( -1 );
   // [(finish modifier_set_field)] 650100

   // [<start to_store>]
//nyi
   set_session_variable( "meta_Parent_Field_For_View", get_obj( ).Parent_Field_For_View( ).get_key( ) );
   set_session_variable( "meta_Parent_Field_For_List", get_obj( ).Parent_Field_For_List( ).get_key( ) );
   // [<finish to_store>]
}

void Meta_Relationship::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start meta_relationship_field)] 600095
   if( is_null( get_obj( ).Source_Relationship( ) ) )
   {
      Meta_Field rel_field;

      string field_key( get_obj( ).get_key( ) + "_C" );

      if( !is_destroy_locked_by_own_session( rel_field, field_key.c_str( ) ) )
      {
         if( is_create )
            rel_field.op_create( field_key );
         else
            rel_field.op_update( field_key );

         rel_field.Class( get_obj( ).Child_Class( ) );
         rel_field.Id( get_obj( ).Field_Id( ) );
         rel_field.Name( get_obj( ).Name( ) );
         rel_field.Mandatory( get_obj( ).Mandatory( ) );
         rel_field.Transient( get_obj( ).Transient( ) );
         rel_field.Parent_Class( get_obj( ).Parent_Class( ) );
         rel_field.Access_Scope( get_obj( ).Access_Scope( ) );
         rel_field.Change_Scope( get_obj( ).Change_Scope( ) );
         rel_field.Access_Restriction( get_obj( ).Access_Restriction( ) );
         rel_field.Change_Restriction( get_obj( ).Change_Restriction( ) );
         rel_field.Access_Permission( get_obj( ).Access_Permission( ) );
         rel_field.Change_Permission( get_obj( ).Change_Permission( ) );

         rel_field.Type( get_obj( ).Model( ).Workgroup( ).get_key( ) + "_foreign_key" );
         rel_field.Extra( get_obj( ).Extra( ) * -1 );
         rel_field.Internal( true );

         rel_field.op_apply( );

         get_obj( ).Field_Id( rel_field.Id( ) );
         get_obj( ).Field_Key( rel_field.get_key( ) );
      }
   }
   // [(finish meta_relationship_field)] 600095

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Relationship::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start update_children)] 600097
   if( !is_create
    && !get_obj( ).get_is_for_peer( )
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Child( ).iterate_forwards( ) )
   {
      do
      {
         if( !is_update_or_destroy_locked_by_own_session( get_obj( ).child_Specification_Child( ) ) )
         {
            get_obj( ).child_Specification_Child( ).op_update( );
            get_obj( ).child_Specification_Child( ).op_apply( );
         }
      } while( get_obj( ).child_Specification_Child( ).iterate_next( ) );
   }
   // [(finish update_children)] 600097

   // [<start after_store>]
//nyi
   if( !is_create && get_obj( ).child_Relationship_Source( ).iterate_forwards( ) )
   {
      do
      {
         get_obj( ).child_Relationship_Source( ).op_update( );
         string parent( get_obj( ).child_Relationship_Source( ).Model( ) );

         string child_class( get_obj( ).child_Relationship_Source( ).Child_Class( ) );
         string parent_class( get_obj( ).child_Relationship_Source( ).Parent_Class( ) );

         get_obj( ).child_Relationship_Source( ).copy_all_field_values( get_obj( ) );
         get_obj( ).child_Relationship_Source( ).Model( parent );
         get_obj( ).child_Relationship_Source( ).Source_Relationship( get_obj( ).get_key( ) );

         get_obj( ).child_Relationship_Source( ).Child_Class( child_class );
         get_obj( ).child_Relationship_Source( ).Parent_Class( parent_class );

         get_obj( ).child_Relationship_Source( ).op_apply( );

      } while( get_obj( ).child_Relationship_Source( ).iterate_next( ) );
   }

   if( is_create && is_null( get_obj( ).Source_Relationship( ) )
    && get_obj( ).Child_Class( ).child_Class_Source( ).iterate_forwards( ) )
   {
      int child_num = 0;
      do
      {
         map< string, string > all_classes;

         if( get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Class( ).iterate_forwards( ) )
         {
            do
            {
               all_classes.insert( make_pair(
                get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Class( ).Id( ),
                get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Class( ).get_key( ) ) );
            } while( get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Class( ).iterate_next( ) );
         }

         // NOTE: Only create a relationship for another model if it contains both child and parent aliases.
         if( all_classes.count( get_obj( ).Child_Class( ).Id( ) )
          && all_classes.count( get_obj( ).Parent_Class( ).Id( ) ) )
         {
            string key_info( construct_key_from_int( get_obj( ).get_key( ), ++child_num ) );

            key_info += ' ';
            key_info += get_obj( ).get_key( );

            get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Relationship( ).op_create( key_info );
            get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Relationship( ).Model( get_obj( ).Child_Class( ).child_Class_Source( ).Model( ) );
            get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Relationship( ).Source_Relationship( get_obj( ).get_key( ) );

            get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Relationship( ).Child_Class( all_classes[ get_obj( ).Child_Class( ).Id( ) ] );
            get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Relationship( ).Parent_Class( all_classes[ get_obj( ).Parent_Class( ).Id( ) ] );

            key_info = FIELD_ID( Meta, Field, Id );
            key_info += "#1 " + get_obj( ).Field_Id( );

            Meta_Class child_class;
            child_class.perform_fetch( all_classes[ get_obj( ).Child_Class( ).Id( ) ] );

            // NOTE: When initially cloned the Field in the Relationship's Child Class has its Parent Class linked
            // to the Class from the source model so locate the field (via the Id which is stored in the Relationship)
            // and change the Parent Class to link to the correct model here.
            if( child_class.child_Field( ).iterate_forwards( key_info, true, 1, e_sql_optimisation_unordered ) )
            {
               if( !is_destroy_locked_by_own_session( child_class.child_Field( ) ) )
               {
                  child_class.child_Field( ).op_update( );
                  child_class.child_Field( ).Parent_Class( all_classes[ get_obj( ).Parent_Class( ).Id( ) ] );
                  child_class.child_Field( ).op_apply( );
               }

               get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Relationship( ).Field_Key( child_class.child_Field( ).get_key( ) );
            }

            get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Relationship( ).op_apply( );
         }
      } while( get_obj( ).Child_Class( ).child_Class_Source( ).iterate_next( ) );
   }
   // [<finish after_store>]
}

bool Meta_Relationship::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Relationship::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start meta_relationship_field)] 600095
   if( !get_obj( ).get_is_being_cascaded( ) && is_null( get_obj( ).Source_Relationship( ) ) )
   {
      Meta_Field rel_field;

      string field_key( get_obj( ).get_key( ) + "_C" );

      if( !is_destroy_locked_by_own_session( rel_field, field_key.c_str( ) ) )
      {
         rel_field.op_destroy( field_key );

         rel_field.op_apply( );
      }
   }
   // [(finish meta_relationship_field)] 600095

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Relationship::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Relationship::impl::set_default_values( )
{
   clear( );
}

bool Meta_Relationship::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_Relationship::impl::get_required_transients( ) const
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

Meta_Relationship::Meta_Relationship( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Relationship::~Meta_Relationship( )
{
   cleanup( );
   delete p_impl;
}

int Meta_Relationship::Access_Restriction( ) const
{
   return p_impl->impl_Access_Restriction( );
}

void Meta_Relationship::Access_Restriction( int Access_Restriction )
{
   p_impl->impl_Access_Restriction( Access_Restriction );
}

int Meta_Relationship::Access_Scope( ) const
{
   return p_impl->impl_Access_Scope( );
}

void Meta_Relationship::Access_Scope( int Access_Scope )
{
   p_impl->impl_Access_Scope( Access_Scope );
}

int Meta_Relationship::Cascade_Op( ) const
{
   return p_impl->impl_Cascade_Op( );
}

void Meta_Relationship::Cascade_Op( int Cascade_Op )
{
   p_impl->impl_Cascade_Op( Cascade_Op );
}

int Meta_Relationship::Change_Restriction( ) const
{
   return p_impl->impl_Change_Restriction( );
}

void Meta_Relationship::Change_Restriction( int Change_Restriction )
{
   p_impl->impl_Change_Restriction( Change_Restriction );
}

int Meta_Relationship::Change_Scope( ) const
{
   return p_impl->impl_Change_Scope( );
}

void Meta_Relationship::Change_Scope( int Change_Scope )
{
   p_impl->impl_Change_Scope( Change_Scope );
}

const string& Meta_Relationship::Child_Class_Id( ) const
{
   return p_impl->impl_Child_Class_Id( );
}

void Meta_Relationship::Child_Class_Id( const string& Child_Class_Id )
{
   p_impl->impl_Child_Class_Id( Child_Class_Id );
}

const string& Meta_Relationship::Child_Class_Name( ) const
{
   return p_impl->impl_Child_Class_Name( );
}

void Meta_Relationship::Child_Class_Name( const string& Child_Class_Name )
{
   p_impl->impl_Child_Class_Name( Child_Class_Name );
}

const string& Meta_Relationship::Child_Name( ) const
{
   return p_impl->impl_Child_Name( );
}

void Meta_Relationship::Child_Name( const string& Child_Name )
{
   p_impl->impl_Child_Name( Child_Name );
}

int Meta_Relationship::Extra( ) const
{
   return p_impl->impl_Extra( );
}

void Meta_Relationship::Extra( int Extra )
{
   p_impl->impl_Extra( Extra );
}

const string& Meta_Relationship::Field_Id( ) const
{
   return p_impl->impl_Field_Id( );
}

void Meta_Relationship::Field_Id( const string& Field_Id )
{
   p_impl->impl_Field_Id( Field_Id );
}

const string& Meta_Relationship::Field_Key( ) const
{
   return p_impl->impl_Field_Key( );
}

void Meta_Relationship::Field_Key( const string& Field_Key )
{
   p_impl->impl_Field_Key( Field_Key );
}

bool Meta_Relationship::Internal( ) const
{
   return p_impl->impl_Internal( );
}

void Meta_Relationship::Internal( bool Internal )
{
   p_impl->impl_Internal( Internal );
}

bool Meta_Relationship::Mandatory( ) const
{
   return p_impl->impl_Mandatory( );
}

void Meta_Relationship::Mandatory( bool Mandatory )
{
   p_impl->impl_Mandatory( Mandatory );
}

const string& Meta_Relationship::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_Relationship::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

bool Meta_Relationship::Transient( ) const
{
   return p_impl->impl_Transient( );
}

void Meta_Relationship::Transient( bool Transient )
{
   p_impl->impl_Transient( Transient );
}

Meta_Permission& Meta_Relationship::Access_Permission( )
{
   return p_impl->impl_Access_Permission( );
}

const Meta_Permission& Meta_Relationship::Access_Permission( ) const
{
   return p_impl->impl_Access_Permission( );
}

void Meta_Relationship::Access_Permission( const string& key )
{
   p_impl->impl_Access_Permission( key );
}

Meta_Permission& Meta_Relationship::Change_Permission( )
{
   return p_impl->impl_Change_Permission( );
}

const Meta_Permission& Meta_Relationship::Change_Permission( ) const
{
   return p_impl->impl_Change_Permission( );
}

void Meta_Relationship::Change_Permission( const string& key )
{
   p_impl->impl_Change_Permission( key );
}

Meta_Class& Meta_Relationship::Child_Class( )
{
   return p_impl->impl_Child_Class( );
}

const Meta_Class& Meta_Relationship::Child_Class( ) const
{
   return p_impl->impl_Child_Class( );
}

void Meta_Relationship::Child_Class( const string& key )
{
   p_impl->impl_Child_Class( key );
}

Meta_Model& Meta_Relationship::Model( )
{
   return p_impl->impl_Model( );
}

const Meta_Model& Meta_Relationship::Model( ) const
{
   return p_impl->impl_Model( );
}

void Meta_Relationship::Model( const string& key )
{
   p_impl->impl_Model( key );
}

Meta_Class& Meta_Relationship::Parent_Class( )
{
   return p_impl->impl_Parent_Class( );
}

const Meta_Class& Meta_Relationship::Parent_Class( ) const
{
   return p_impl->impl_Parent_Class( );
}

void Meta_Relationship::Parent_Class( const string& key )
{
   p_impl->impl_Parent_Class( key );
}

Meta_Field& Meta_Relationship::Parent_Field_For_List( )
{
   return p_impl->impl_Parent_Field_For_List( );
}

const Meta_Field& Meta_Relationship::Parent_Field_For_List( ) const
{
   return p_impl->impl_Parent_Field_For_List( );
}

void Meta_Relationship::Parent_Field_For_List( const string& key )
{
   p_impl->impl_Parent_Field_For_List( key );
}

Meta_Field& Meta_Relationship::Parent_Field_For_View( )
{
   return p_impl->impl_Parent_Field_For_View( );
}

const Meta_Field& Meta_Relationship::Parent_Field_For_View( ) const
{
   return p_impl->impl_Parent_Field_For_View( );
}

void Meta_Relationship::Parent_Field_For_View( const string& key )
{
   p_impl->impl_Parent_Field_For_View( key );
}

Meta_Relationship& Meta_Relationship::Source_Relationship( )
{
   return p_impl->impl_Source_Relationship( );
}

const Meta_Relationship& Meta_Relationship::Source_Relationship( ) const
{
   return p_impl->impl_Source_Relationship( );
}

void Meta_Relationship::Source_Relationship( const string& key )
{
   p_impl->impl_Source_Relationship( key );
}

Meta_List_Field& Meta_Relationship::child_List_Field_Child( )
{
   return p_impl->impl_child_List_Field_Child( );
}

const Meta_List_Field& Meta_Relationship::child_List_Field_Child( ) const
{
   return p_impl->impl_child_List_Field_Child( );
}

Meta_Specification& Meta_Relationship::child_Specification_Child( )
{
   return p_impl->impl_child_Specification_Child( );
}

const Meta_Specification& Meta_Relationship::child_Specification_Child( ) const
{
   return p_impl->impl_child_Specification_Child( );
}

Meta_Relationship& Meta_Relationship::child_Relationship_Source( )
{
   return p_impl->impl_child_Relationship_Source( );
}

const Meta_Relationship& Meta_Relationship::child_Relationship_Source( ) const
{
   return p_impl->impl_child_Relationship_Source( );
}

string Meta_Relationship::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Relationship::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_Relationship::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_Relationship::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_Relationship::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_Relationship::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_Relationship::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_Relationship::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_Relationship::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_Relationship::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Relationship::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_Relationship::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Relationship::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Relationship::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Relationship::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Relationship::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Relationship::clear( )
{
   p_impl->clear( );
}

void Meta_Relationship::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Relationship::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Relationship::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Relationship::finalise_fetch( bool skip_set_original )
{
   p_impl->finalise_fetch( skip_set_original );
}

void Meta_Relationship::at_create( )
{
   p_impl->at_create( );
}

void Meta_Relationship::post_init( )
{
   p_impl->post_init( );
}

void Meta_Relationship::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Relationship::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Relationship::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Relationship::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Relationship::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Relationship::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Relationship::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Relationship::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Relationship::get_field_id(
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
   else if( name == c_field_name_Cascade_Op )
   {
      p_id = c_field_id_Cascade_Op;

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
   else if( name == c_field_name_Child_Class )
   {
      p_id = c_field_id_Child_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Child_Class_Id )
   {
      p_id = c_field_id_Child_Class_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Child_Class_Name )
   {
      p_id = c_field_id_Child_Class_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Child_Name )
   {
      p_id = c_field_id_Child_Name;

      if( p_type_name )
         *p_type_name = "string";

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
   else if( name == c_field_name_Field_Id )
   {
      p_id = c_field_id_Field_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Field_Key )
   {
      p_id = c_field_id_Field_Key;

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
   else if( name == c_field_name_Mandatory )
   {
      p_id = c_field_id_Mandatory;

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
   else if( name == c_field_name_Parent_Class )
   {
      p_id = c_field_id_Parent_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Parent_Field_For_List )
   {
      p_id = c_field_id_Parent_Field_For_List;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Parent_Field_For_View )
   {
      p_id = c_field_id_Parent_Field_For_View;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_Relationship )
   {
      p_id = c_field_id_Source_Relationship;

      if( p_type_name )
         *p_type_name = "Meta_Relationship";

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

   return p_id;
}

const char* Meta_Relationship::get_field_name(
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
   else if( id == c_field_id_Cascade_Op )
   {
      p_name = c_field_name_Cascade_Op;

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
   else if( id == c_field_id_Child_Class )
   {
      p_name = c_field_name_Child_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Child_Class_Id )
   {
      p_name = c_field_name_Child_Class_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Child_Class_Name )
   {
      p_name = c_field_name_Child_Class_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Child_Name )
   {
      p_name = c_field_name_Child_Name;

      if( p_type_name )
         *p_type_name = "string";

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
   else if( id == c_field_id_Field_Id )
   {
      p_name = c_field_name_Field_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Field_Key )
   {
      p_name = c_field_name_Field_Key;

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
   else if( id == c_field_id_Mandatory )
   {
      p_name = c_field_name_Mandatory;

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
   else if( id == c_field_id_Parent_Class )
   {
      p_name = c_field_name_Parent_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Parent_Field_For_List )
   {
      p_name = c_field_name_Parent_Field_For_List;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Parent_Field_For_View )
   {
      p_name = c_field_name_Parent_Field_For_View;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_Relationship )
   {
      p_name = c_field_name_Source_Relationship;

      if( p_type_name )
         *p_type_name = "Meta_Relationship";

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

   return p_name;
}

string& Meta_Relationship::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_Relationship::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_Relationship::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_Relationship::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_Relationship::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_Relationship::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Relationship::get_field_uom_symbol( const string& id_or_name ) const
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
   if( ( id_or_name == c_field_id_Cascade_Op ) || ( id_or_name == c_field_name_Cascade_Op ) )
   {
      name = string( c_field_display_name_Cascade_Op );
      get_module_string( c_field_display_name_Cascade_Op, &next );
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
   if( ( id_or_name == c_field_id_Child_Class ) || ( id_or_name == c_field_name_Child_Class ) )
   {
      name = string( c_field_display_name_Child_Class );
      get_module_string( c_field_display_name_Child_Class, &next );
   }
   if( ( id_or_name == c_field_id_Child_Class_Id ) || ( id_or_name == c_field_name_Child_Class_Id ) )
   {
      name = string( c_field_display_name_Child_Class_Id );
      get_module_string( c_field_display_name_Child_Class_Id, &next );
   }
   if( ( id_or_name == c_field_id_Child_Class_Name ) || ( id_or_name == c_field_name_Child_Class_Name ) )
   {
      name = string( c_field_display_name_Child_Class_Name );
      get_module_string( c_field_display_name_Child_Class_Name, &next );
   }
   if( ( id_or_name == c_field_id_Child_Name ) || ( id_or_name == c_field_name_Child_Name ) )
   {
      name = string( c_field_display_name_Child_Name );
      get_module_string( c_field_display_name_Child_Name, &next );
   }
   if( ( id_or_name == c_field_id_Extra ) || ( id_or_name == c_field_name_Extra ) )
   {
      name = string( c_field_display_name_Extra );
      get_module_string( c_field_display_name_Extra, &next );
   }
   if( ( id_or_name == c_field_id_Field_Id ) || ( id_or_name == c_field_name_Field_Id ) )
   {
      name = string( c_field_display_name_Field_Id );
      get_module_string( c_field_display_name_Field_Id, &next );
   }
   if( ( id_or_name == c_field_id_Field_Key ) || ( id_or_name == c_field_name_Field_Key ) )
   {
      name = string( c_field_display_name_Field_Key );
      get_module_string( c_field_display_name_Field_Key, &next );
   }
   if( ( id_or_name == c_field_id_Internal ) || ( id_or_name == c_field_name_Internal ) )
   {
      name = string( c_field_display_name_Internal );
      get_module_string( c_field_display_name_Internal, &next );
   }
   if( ( id_or_name == c_field_id_Mandatory ) || ( id_or_name == c_field_name_Mandatory ) )
   {
      name = string( c_field_display_name_Mandatory );
      get_module_string( c_field_display_name_Mandatory, &next );
   }
   if( ( id_or_name == c_field_id_Model ) || ( id_or_name == c_field_name_Model ) )
   {
      name = string( c_field_display_name_Model );
      get_module_string( c_field_display_name_Model, &next );
   }
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
   {
      name = string( c_field_display_name_Name );
      get_module_string( c_field_display_name_Name, &next );
   }
   if( ( id_or_name == c_field_id_Parent_Class ) || ( id_or_name == c_field_name_Parent_Class ) )
   {
      name = string( c_field_display_name_Parent_Class );
      get_module_string( c_field_display_name_Parent_Class, &next );
   }
   if( ( id_or_name == c_field_id_Parent_Field_For_List ) || ( id_or_name == c_field_name_Parent_Field_For_List ) )
   {
      name = string( c_field_display_name_Parent_Field_For_List );
      get_module_string( c_field_display_name_Parent_Field_For_List, &next );
   }
   if( ( id_or_name == c_field_id_Parent_Field_For_View ) || ( id_or_name == c_field_name_Parent_Field_For_View ) )
   {
      name = string( c_field_display_name_Parent_Field_For_View );
      get_module_string( c_field_display_name_Parent_Field_For_View, &next );
   }
   if( ( id_or_name == c_field_id_Source_Relationship ) || ( id_or_name == c_field_name_Source_Relationship ) )
   {
      name = string( c_field_display_name_Source_Relationship );
      get_module_string( c_field_display_name_Source_Relationship, &next );
   }
   if( ( id_or_name == c_field_id_Transient ) || ( id_or_name == c_field_name_Transient ) )
   {
      name = string( c_field_display_name_Transient );
      get_module_string( c_field_display_name_Transient, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_Relationship::get_field_display_name( const string& id_or_name ) const
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
   if( ( id_or_name == c_field_id_Cascade_Op ) || ( id_or_name == c_field_name_Cascade_Op ) )
      display_name = get_module_string( c_field_display_name_Cascade_Op );
   if( ( id_or_name == c_field_id_Change_Permission ) || ( id_or_name == c_field_name_Change_Permission ) )
      display_name = get_module_string( c_field_display_name_Change_Permission );
   if( ( id_or_name == c_field_id_Change_Restriction ) || ( id_or_name == c_field_name_Change_Restriction ) )
      display_name = get_module_string( c_field_display_name_Change_Restriction );
   if( ( id_or_name == c_field_id_Change_Scope ) || ( id_or_name == c_field_name_Change_Scope ) )
      display_name = get_module_string( c_field_display_name_Change_Scope );
   if( ( id_or_name == c_field_id_Child_Class ) || ( id_or_name == c_field_name_Child_Class ) )
      display_name = get_module_string( c_field_display_name_Child_Class );
   if( ( id_or_name == c_field_id_Child_Class_Id ) || ( id_or_name == c_field_name_Child_Class_Id ) )
      display_name = get_module_string( c_field_display_name_Child_Class_Id );
   if( ( id_or_name == c_field_id_Child_Class_Name ) || ( id_or_name == c_field_name_Child_Class_Name ) )
      display_name = get_module_string( c_field_display_name_Child_Class_Name );
   if( ( id_or_name == c_field_id_Child_Name ) || ( id_or_name == c_field_name_Child_Name ) )
      display_name = get_module_string( c_field_display_name_Child_Name );
   if( ( id_or_name == c_field_id_Extra ) || ( id_or_name == c_field_name_Extra ) )
      display_name = get_module_string( c_field_display_name_Extra );
   if( ( id_or_name == c_field_id_Field_Id ) || ( id_or_name == c_field_name_Field_Id ) )
      display_name = get_module_string( c_field_display_name_Field_Id );
   if( ( id_or_name == c_field_id_Field_Key ) || ( id_or_name == c_field_name_Field_Key ) )
      display_name = get_module_string( c_field_display_name_Field_Key );
   if( ( id_or_name == c_field_id_Internal ) || ( id_or_name == c_field_name_Internal ) )
      display_name = get_module_string( c_field_display_name_Internal );
   if( ( id_or_name == c_field_id_Mandatory ) || ( id_or_name == c_field_name_Mandatory ) )
      display_name = get_module_string( c_field_display_name_Mandatory );
   if( ( id_or_name == c_field_id_Model ) || ( id_or_name == c_field_name_Model ) )
      display_name = get_module_string( c_field_display_name_Model );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
      display_name = get_module_string( c_field_display_name_Name );
   if( ( id_or_name == c_field_id_Parent_Class ) || ( id_or_name == c_field_name_Parent_Class ) )
      display_name = get_module_string( c_field_display_name_Parent_Class );
   if( ( id_or_name == c_field_id_Parent_Field_For_List ) || ( id_or_name == c_field_name_Parent_Field_For_List ) )
      display_name = get_module_string( c_field_display_name_Parent_Field_For_List );
   if( ( id_or_name == c_field_id_Parent_Field_For_View ) || ( id_or_name == c_field_name_Parent_Field_For_View ) )
      display_name = get_module_string( c_field_display_name_Parent_Field_For_View );
   if( ( id_or_name == c_field_id_Source_Relationship ) || ( id_or_name == c_field_name_Source_Relationship ) )
      display_name = get_module_string( c_field_display_name_Source_Relationship );
   if( ( id_or_name == c_field_id_Transient ) || ( id_or_name == c_field_name_Transient ) )
      display_name = get_module_string( c_field_display_name_Transient );

   return display_name;
}

void Meta_Relationship::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Relationship::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Relationship::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Relationship::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Relationship::setup_foreign_key( Meta_Permission& o, const string& value )
{
   static_cast< Meta_Permission& >( o ).set_key( value );
}

void Meta_Relationship::setup_foreign_key( Meta_Class& o, const string& value )
{
   static_cast< Meta_Class& >( o ).set_key( value );
}

void Meta_Relationship::setup_foreign_key( Meta_Model& o, const string& value )
{
   static_cast< Meta_Model& >( o ).set_key( value );
}

void Meta_Relationship::setup_foreign_key( Meta_Field& o, const string& value )
{
   static_cast< Meta_Field& >( o ).set_key( value );
}

void Meta_Relationship::setup_foreign_key( Meta_Relationship& o, const string& value )
{
   static_cast< Meta_Relationship& >( o ).set_key( value );
}

void Meta_Relationship::setup_graph_parent( Meta_List_Field& o, const string& foreign_key_field )
{
   static_cast< Meta_List_Field& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Relationship::setup_graph_parent( Meta_Specification& o, const string& foreign_key_field )
{
   static_cast< Meta_Specification& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Relationship::setup_graph_parent( Meta_Relationship& o, const string& foreign_key_field )
{
   static_cast< Meta_Relationship& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Relationship::setup_graph_parent(
 Meta_Permission& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Permission& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Permission& >( o ).set_key( init_value );
}

void Meta_Relationship::setup_graph_parent(
 Meta_Class& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Class& >( o ).set_key( init_value );
}

void Meta_Relationship::setup_graph_parent(
 Meta_Model& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Model& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Model& >( o ).set_key( init_value );
}

void Meta_Relationship::setup_graph_parent(
 Meta_Field& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Field& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Field& >( o ).set_key( init_value );
}

void Meta_Relationship::setup_graph_parent(
 Meta_Relationship& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Relationship& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Relationship& >( o ).set_key( init_value );
}

size_t Meta_Relationship::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Relationship::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Relationship::get_num_foreign_key_children( bool is_internal ) const
{
   size_t rc = 3;

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

class_base* Meta_Relationship::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   if( child_num >= 3 )
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
            next_child_field = "302193";
            p_class_base = &child_List_Field_Child( );
         }
         break;

         case 1:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301475";
            p_class_base = &child_Specification_Child( );
         }
         break;

         case 2:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301330";
            p_class_base = &child_Relationship_Source( );
         }
         break;
      }
   }

   return p_class_base;
}

void Meta_Relationship::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_Relationship::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_Relationship::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_Relationship::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_Relationship::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_Relationship::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_Relationship::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_Relationship::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "relationship";

   return get_module_string( key );
}

string Meta_Relationship::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_Relationship::get_create_instance_info( ) const
{
   return "";
}

string Meta_Relationship::get_update_instance_info( ) const
{
   return "";
}

string Meta_Relationship::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_Relationship::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );

   return retval;
}

bool Meta_Relationship::get_is_alias( ) const
{
   return false;
}

void Meta_Relationship::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Relationship::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Relationship::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == "_302193" ) || ( sub_context == "child_List_Field_Child" ) )
      p_class_base = &child_List_Field_Child( );
   else if( ( sub_context == "_301475" ) || ( sub_context == "child_Specification_Child" ) )
      p_class_base = &child_Specification_Child( );
   else if( ( sub_context == "_301330" ) || ( sub_context == "child_Relationship_Source" ) )
      p_class_base = &child_Relationship_Source( );
   else if( ( sub_context == c_field_id_Access_Permission ) || ( sub_context == c_field_name_Access_Permission ) )
      p_class_base = &Access_Permission( );
   else if( ( sub_context == c_field_id_Change_Permission ) || ( sub_context == c_field_name_Change_Permission ) )
      p_class_base = &Change_Permission( );
   else if( ( sub_context == c_field_id_Child_Class ) || ( sub_context == c_field_name_Child_Class ) )
      p_class_base = &Child_Class( );
   else if( ( sub_context == c_field_id_Model ) || ( sub_context == c_field_name_Model ) )
      p_class_base = &Model( );
   else if( ( sub_context == c_field_id_Parent_Class ) || ( sub_context == c_field_name_Parent_Class ) )
      p_class_base = &Parent_Class( );
   else if( ( sub_context == c_field_id_Parent_Field_For_List ) || ( sub_context == c_field_name_Parent_Field_For_List ) )
      p_class_base = &Parent_Field_For_List( );
   else if( ( sub_context == c_field_id_Parent_Field_For_View ) || ( sub_context == c_field_name_Parent_Field_For_View ) )
      p_class_base = &Parent_Field_For_View( );
   else if( ( sub_context == c_field_id_Source_Relationship ) || ( sub_context == c_field_name_Source_Relationship ) )
      p_class_base = &Source_Relationship( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Relationship::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Access_Permission" );
   names.push_back( "C_Access_Restriction" );
   names.push_back( "C_Access_Scope" );
   names.push_back( "C_Cascade_Op" );
   names.push_back( "C_Change_Permission" );
   names.push_back( "C_Change_Restriction" );
   names.push_back( "C_Change_Scope" );
   names.push_back( "C_Child_Class" );
   names.push_back( "C_Extra" );
   names.push_back( "C_Field_Id" );
   names.push_back( "C_Field_Key" );
   names.push_back( "C_Internal" );
   names.push_back( "C_Mandatory" );
   names.push_back( "C_Model" );
   names.push_back( "C_Name" );
   names.push_back( "C_Parent_Class" );
   names.push_back( "C_Source_Relationship" );
   names.push_back( "C_Transient" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Relationship::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Access_Permission( ) ) ) );
   values.push_back( to_string( Access_Restriction( ) ) );
   values.push_back( to_string( Access_Scope( ) ) );
   values.push_back( to_string( Cascade_Op( ) ) );
   values.push_back( sql_quote( to_string( Change_Permission( ) ) ) );
   values.push_back( to_string( Change_Restriction( ) ) );
   values.push_back( to_string( Change_Scope( ) ) );
   values.push_back( sql_quote( to_string( Child_Class( ) ) ) );
   values.push_back( to_string( Extra( ) ) );
   values.push_back( sql_quote( to_string( Field_Id( ) ) ) );
   values.push_back( sql_quote( to_string( Field_Key( ) ) ) );
   values.push_back( to_string( Internal( ) ) );
   values.push_back( to_string( Mandatory( ) ) );
   values.push_back( sql_quote( to_string( Model( ) ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( sql_quote( to_string( Parent_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Relationship( ) ) ) );
   values.push_back( to_string( Transient( ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Relationship::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [(start meta_relationship_child_name)] 600096
   if( needs_field_value( "Child_Name", dependents ) )
   {
      dependents.insert( "Name" );

      if( ( use_transients && is_field_transient( e_field_id_Name ) )
       || ( !use_transients && !is_field_transient( e_field_id_Name ) ) )
         names.insert( "Name" );
   }

   if( needs_field_value( "Child_Name", dependents ) )
   {
      dependents.insert( "Child_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Child_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Child_Class ) ) )
         names.insert( "Child_Class" );
   }

   if( needs_field_value( "Child_Name", dependents ) )
   {
      dependents.insert( "Parent_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Parent_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Parent_Class ) ) )
         names.insert( "Parent_Class" );
   }
   // [(finish meta_relationship_child_name)] 600096

   // [(start transient_field_alias)] 610096
   if( needs_field_value( "Child_Class_Name", dependents ) )
   {
      dependents.insert( "Child_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Child_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Child_Class ) ) )
         names.insert( "Child_Class" );
   }
   // [(finish transient_field_alias)] 610096

   // [(start transient_field_alias)] 620096
   if( needs_field_value( "Child_Class_Id", dependents ) )
   {
      dependents.insert( "Child_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Child_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Child_Class ) ) )
         names.insert( "Child_Class" );
   }
   // [(finish transient_field_alias)] 620096

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Relationship::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start modifier_field_value)] 610100
   dependents.insert( "Internal" ); // (for Is_Internal modifier)

   if( ( use_transients && is_field_transient( e_field_id_Internal ) )
    || ( !use_transients && !is_field_transient( e_field_id_Internal ) ) )
      names.insert( "Internal" );
   // [(finish modifier_field_value)] 610100

   // [(start protect_equal)] 620100
   dependents.insert( "Internal" );

   if( ( use_transients && is_field_transient( e_field_id_Internal ) )
    || ( !use_transients && !is_field_transient( e_field_id_Internal ) ) )
      names.insert( "Internal" );
   // [(finish protect_equal)] 620100

   // [(start modifier_field_value)] 640100
   dependents.insert( "Transient" ); // (for Is_Transient modifier)

   if( ( use_transients && is_field_transient( e_field_id_Transient ) )
    || ( !use_transients && !is_field_transient( e_field_id_Transient ) ) )
      names.insert( "Transient" );
   // [(finish modifier_field_value)] 640100

   // [(start modifier_field_null)] 640101
   dependents.insert( "Child_Class" ); // (for Hide_Parent_Field_For_View modifier)

   if( ( use_transients && is_field_transient( e_field_id_Child_Class ) )
    || ( !use_transients && !is_field_transient( e_field_id_Child_Class ) ) )
      names.insert( "Child_Class" );
   // [(finish modifier_field_null)] 640101

   // [(start modifier_field_null)] 640102
   dependents.insert( "Child_Class" ); // (for Hide_Parent_Field_For_List modifier)

   if( ( use_transients && is_field_transient( e_field_id_Child_Class ) )
    || ( !use_transients && !is_field_transient( e_field_id_Child_Class ) ) )
      names.insert( "Child_Class" );
   // [(finish modifier_field_null)] 640102

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Relationship::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Relationship::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_Relationship::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Relationship::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Relationship::static_lock_class_id( )
{
   return "113100";
}

const char* Meta_Relationship::static_check_class_name( )
{
   return "Relationship";
}

const char* Meta_Relationship::static_persistence_extra( )
{
   return "";
}

bool Meta_Relationship::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Relationship::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.113100" );
}

void Meta_Relationship::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "301330c", "Access_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "113114", "Access_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "113112", "Access_Scope", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "113103", "Cascade_Op", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "301330d", "Change_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "113115", "Change_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "113113", "Change_Scope", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "301310", "Child_Class", "Meta_Class", true, "", "" ) );
   all_field_info.push_back( field_info( "113116", "Child_Class_Id", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "113108", "Child_Class_Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "113105", "Child_Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "113104", "Extra", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "113106", "Field_Id", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "113107", "Field_Key", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "113110", "Internal", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "113102", "Mandatory", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "301300", "Model", "Meta_Model", true, "", "" ) );
   all_field_info.push_back( field_info( "113101", "Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "301320", "Parent_Class", "Meta_Class", true, "", "" ) );
   all_field_info.push_back( field_info( "301330b", "Parent_Field_For_List", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301330a", "Parent_Field_For_View", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301330", "Source_Relationship", "Meta_Relationship", false, "", "" ) );
   all_field_info.push_back( field_info( "113111", "Transient", "bool", false, "", "" ) );
}

void Meta_Relationship::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Access_Permission, make_pair( "Meta.113100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Change_Permission, make_pair( "Meta.113100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Child_Class, make_pair( "Meta.113100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Model, make_pair( "Meta.113100", "Meta_Model" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Parent_Class, make_pair( "Meta.113100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Parent_Field_For_List, make_pair( "Meta.113100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Parent_Field_For_View, make_pair( "Meta.113100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Relationship, make_pair( "Meta.113100", "Meta_Relationship" ) ) );
}

int Meta_Relationship::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Relationship::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_Relationship::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Relationship::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "301330c";
      break;

      case 2:
      p_id = "113114";
      break;

      case 3:
      p_id = "113112";
      break;

      case 4:
      p_id = "113103";
      break;

      case 5:
      p_id = "301330d";
      break;

      case 6:
      p_id = "113115";
      break;

      case 7:
      p_id = "113113";
      break;

      case 8:
      p_id = "301310";
      break;

      case 9:
      p_id = "113116";
      break;

      case 10:
      p_id = "113108";
      break;

      case 11:
      p_id = "113105";
      break;

      case 12:
      p_id = "113104";
      break;

      case 13:
      p_id = "113106";
      break;

      case 14:
      p_id = "113107";
      break;

      case 15:
      p_id = "113110";
      break;

      case 16:
      p_id = "113102";
      break;

      case 17:
      p_id = "301300";
      break;

      case 18:
      p_id = "113101";
      break;

      case 19:
      p_id = "301320";
      break;

      case 20:
      p_id = "301330b";
      break;

      case 21:
      p_id = "301330a";
      break;

      case 22:
      p_id = "301330";
      break;

      case 23:
      p_id = "113111";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Relationship" );

   return p_id;
}

const char* Meta_Relationship::static_get_field_name( field_id id )
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
      p_id = "Cascade_Op";
      break;

      case 5:
      p_id = "Change_Permission";
      break;

      case 6:
      p_id = "Change_Restriction";
      break;

      case 7:
      p_id = "Change_Scope";
      break;

      case 8:
      p_id = "Child_Class";
      break;

      case 9:
      p_id = "Child_Class_Id";
      break;

      case 10:
      p_id = "Child_Class_Name";
      break;

      case 11:
      p_id = "Child_Name";
      break;

      case 12:
      p_id = "Extra";
      break;

      case 13:
      p_id = "Field_Id";
      break;

      case 14:
      p_id = "Field_Key";
      break;

      case 15:
      p_id = "Internal";
      break;

      case 16:
      p_id = "Mandatory";
      break;

      case 17:
      p_id = "Model";
      break;

      case 18:
      p_id = "Name";
      break;

      case 19:
      p_id = "Parent_Class";
      break;

      case 20:
      p_id = "Parent_Field_For_List";
      break;

      case 21:
      p_id = "Parent_Field_For_View";
      break;

      case 22:
      p_id = "Source_Relationship";
      break;

      case 23:
      p_id = "Transient";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Relationship" );

   return p_id;
}

int Meta_Relationship::static_get_field_num( const string& field )
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
   else if( field == c_field_id_Cascade_Op || field == c_field_name_Cascade_Op )
      rc += 4;
   else if( field == c_field_id_Change_Permission || field == c_field_name_Change_Permission )
      rc += 5;
   else if( field == c_field_id_Change_Restriction || field == c_field_name_Change_Restriction )
      rc += 6;
   else if( field == c_field_id_Change_Scope || field == c_field_name_Change_Scope )
      rc += 7;
   else if( field == c_field_id_Child_Class || field == c_field_name_Child_Class )
      rc += 8;
   else if( field == c_field_id_Child_Class_Id || field == c_field_name_Child_Class_Id )
      rc += 9;
   else if( field == c_field_id_Child_Class_Name || field == c_field_name_Child_Class_Name )
      rc += 10;
   else if( field == c_field_id_Child_Name || field == c_field_name_Child_Name )
      rc += 11;
   else if( field == c_field_id_Extra || field == c_field_name_Extra )
      rc += 12;
   else if( field == c_field_id_Field_Id || field == c_field_name_Field_Id )
      rc += 13;
   else if( field == c_field_id_Field_Key || field == c_field_name_Field_Key )
      rc += 14;
   else if( field == c_field_id_Internal || field == c_field_name_Internal )
      rc += 15;
   else if( field == c_field_id_Mandatory || field == c_field_name_Mandatory )
      rc += 16;
   else if( field == c_field_id_Model || field == c_field_name_Model )
      rc += 17;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 18;
   else if( field == c_field_id_Parent_Class || field == c_field_name_Parent_Class )
      rc += 19;
   else if( field == c_field_id_Parent_Field_For_List || field == c_field_name_Parent_Field_For_List )
      rc += 20;
   else if( field == c_field_id_Parent_Field_For_View || field == c_field_name_Parent_Field_For_View )
      rc += 21;
   else if( field == c_field_id_Source_Relationship || field == c_field_name_Source_Relationship )
      rc += 22;
   else if( field == c_field_id_Transient || field == c_field_name_Transient )
      rc += 23;

   return rc - 1;
}

procedure_info_container& Meta_Relationship::static_get_procedure_info( )
{
   static procedure_info_container procedures;

   return procedures;
}

string Meta_Relationship::static_get_sql_columns( )
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
    "C_Cascade_Op INTEGER NOT NULL,"
    "C_Change_Permission VARCHAR(75) NOT NULL,"
    "C_Change_Restriction INTEGER NOT NULL,"
    "C_Change_Scope INTEGER NOT NULL,"
    "C_Child_Class VARCHAR(75) NOT NULL,"
    "C_Extra INTEGER NOT NULL,"
    "C_Field_Id VARCHAR(200) NOT NULL,"
    "C_Field_Key VARCHAR(200) NOT NULL,"
    "C_Internal INTEGER NOT NULL,"
    "C_Mandatory INTEGER NOT NULL,"
    "C_Model VARCHAR(75) NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Parent_Class VARCHAR(75) NOT NULL,"
    "C_Source_Relationship VARCHAR(75) NOT NULL,"
    "C_Transient INTEGER NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Relationship::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Relationship::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
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

   pairs.push_back( make_pair( "enum_cascade_op_0", get_enum_string_cascade_op( 0 ) ) );
   pairs.push_back( make_pair( "enum_cascade_op_1", get_enum_string_cascade_op( 1 ) ) );
   pairs.push_back( make_pair( "enum_cascade_op_2", get_enum_string_cascade_op( 2 ) ) );
   pairs.push_back( make_pair( "enum_cascade_op_-1", get_enum_string_cascade_op( -1 ) ) );

   pairs.push_back( make_pair( "enum_view_change_restrict_0", get_enum_string_view_change_restrict( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_change_restrict_1", get_enum_string_view_change_restrict( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_change_restrict_2", get_enum_string_view_change_restrict( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_change_restrict_3", get_enum_string_view_change_restrict( 3 ) ) );
   pairs.push_back( make_pair( "enum_view_change_restrict_4", get_enum_string_view_change_restrict( 4 ) ) );

   pairs.push_back( make_pair( "enum_view_field_change_scope_0", get_enum_string_view_field_change_scope( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_field_change_scope_1", get_enum_string_view_field_change_scope( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_field_change_scope_2", get_enum_string_view_field_change_scope( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_field_change_scope_3", get_enum_string_view_field_change_scope( 3 ) ) );

   pairs.push_back( make_pair( "enum_relationship_extra_0", get_enum_string_relationship_extra( 0 ) ) );
   pairs.push_back( make_pair( "enum_relationship_extra_2", get_enum_string_relationship_extra( 2 ) ) );
   pairs.push_back( make_pair( "enum_relationship_extra_3", get_enum_string_relationship_extra( 3 ) ) );
   pairs.push_back( make_pair( "enum_relationship_extra_1", get_enum_string_relationship_extra( 1 ) ) );
   pairs.push_back( make_pair( "enum_relationship_extra_4", get_enum_string_relationship_extra( 4 ) ) );
   pairs.push_back( make_pair( "enum_relationship_extra_5", get_enum_string_relationship_extra( 5 ) ) );
}

void Meta_Relationship::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "Model,Child_Class,Name", "string,string,string" ) );
   pairs.push_back( make_pair( "Parent_Class,@pk", "string,string" ) );
   pairs.push_back( make_pair( "Source_Relationship,@pk", "string,string" ) );
}

void Meta_Relationship::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "Model,Child_Class,Name" );
}

void Meta_Relationship::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Model,C_Child_Class,C_Name" );
   indexes.push_back( "C_Parent_Class,C_Key_" );
   indexes.push_back( "C_Source_Relationship,C_Key_" );
}

void Meta_Relationship::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Model,C_Child_Class,C_Name" );
}

void Meta_Relationship::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Relationship::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Relationship::static_insert_external_alias( const string& module_and_class_id, Meta_Relationship* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Relationship::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Relationship::static_class_init( const char* p_module_name )
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

   g_cascade_op_enum.insert( 0 );
   g_cascade_op_enum.insert( 1 );
   g_cascade_op_enum.insert( 2 );
   g_cascade_op_enum.insert( -1 );

   g_view_change_restrict_enum.insert( 0 );
   g_view_change_restrict_enum.insert( 1 );
   g_view_change_restrict_enum.insert( 2 );
   g_view_change_restrict_enum.insert( 3 );
   g_view_change_restrict_enum.insert( 4 );

   g_view_field_change_scope_enum.insert( 0 );
   g_view_field_change_scope_enum.insert( 1 );
   g_view_field_change_scope_enum.insert( 2 );
   g_view_field_change_scope_enum.insert( 3 );

   g_relationship_extra_enum.insert( 0 );
   g_relationship_extra_enum.insert( 2 );
   g_relationship_extra_enum.insert( 3 );
   g_relationship_extra_enum.insert( 1 );
   g_relationship_extra_enum.insert( 4 );
   g_relationship_extra_enum.insert( 5 );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Relationship::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
