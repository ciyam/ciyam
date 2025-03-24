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

#include "Meta_Procedure.h"

#include "Meta_Specification.h"
#include "Meta_List_Field.h"
#include "Meta_Package_Option.h"
#include "Meta_Procedure_Arg.h"
#include "Meta_Permission.h"
#include "Meta_Class.h"
#include "Meta_Procedure.h"

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

// [(start clone_children_for_create)] 610090
#include "Meta_Class.h"
// [(finish clone_children_for_create)] 610090

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

template< > inline string to_string( const Meta_Procedure& c )
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

inline void from_string( Meta_Procedure& c, const string& s )
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

#include "Meta_Procedure.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Access_Permission = "301120";
const char* const c_field_id_Access_Restriction = "111104";
const char* const c_field_id_Class = "301100";
const char* const c_field_id_Id = "111103";
const char* const c_field_id_Internal = "111102";
const char* const c_field_id_Name = "111101";
const char* const c_field_id_Source_Procedure = "301110";

const char* const c_field_name_Access_Permission = "Access_Permission";
const char* const c_field_name_Access_Restriction = "Access_Restriction";
const char* const c_field_name_Class = "Class";
const char* const c_field_name_Id = "Id";
const char* const c_field_name_Internal = "Internal";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Source_Procedure = "Source_Procedure";

const char* const c_field_display_name_Access_Permission = "field_procedure_access_permission";
const char* const c_field_display_name_Access_Restriction = "field_procedure_access_restriction";
const char* const c_field_display_name_Class = "field_procedure_class";
const char* const c_field_display_name_Id = "field_procedure_id";
const char* const c_field_display_name_Internal = "field_procedure_internal";
const char* const c_field_display_name_Name = "field_procedure_name";
const char* const c_field_display_name_Source_Procedure = "field_procedure_source_procedure";

const int c_num_fields = 7;

const char* const c_all_sorted_field_ids[ ] =
{
   "111101",
   "111102",
   "111103",
   "111104",
   "301100",
   "301110",
   "301120"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Access_Permission",
   "Access_Restriction",
   "Class",
   "Id",
   "Internal",
   "Name",
   "Source_Procedure"
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

const uint64_t c_modifier_Is_Internal = UINT64_C( 0x100 );

aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Id_domain;
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

typedef map< string, Meta_Procedure* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Procedure* > external_aliases_lookup_container;
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
string g_default_Class = string( );
string g_default_Id = string( );
bool g_default_Internal = bool( 0 );
string g_default_Name = string( );
string g_default_Source_Procedure = string( );

set< int > g_view_access_restrict_enum;

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

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Procedure > Procedure_registration( get_class_registry( ), "111100" );

class Meta_Procedure_command_functor;

class Meta_Procedure_command_handler : public command_handler
{
   friend class Meta_Procedure_command_functor;

   public:
   Meta_Procedure_command_handler( ) : p_Meta_Procedure( 0 ) { }

   void set_Meta_Procedure( Meta_Procedure* p_new_Meta_Procedure ) { p_Meta_Procedure = p_new_Meta_Procedure; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Procedure* p_Meta_Procedure;

   protected:
   string retval;
};

class Meta_Procedure_command_functor : public command_functor
{
   public:
   Meta_Procedure_command_functor( Meta_Procedure_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Procedure_command_handler& cmd_handler;
};

command_functor* Meta_Procedure_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Procedure_command_functor( dynamic_cast< Meta_Procedure_command_handler& >( handler ) );
}

void Meta_Procedure_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Procedure_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_Procedure_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Procedure->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Procedure->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Procedure_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Procedure->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Procedure->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Procedure_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Procedure_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_Procedure->Access_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Restriction ) || ( field_name == c_field_name_Access_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Procedure->Access_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Procedure->Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Id ) || ( field_name == c_field_name_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Procedure->Id( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Internal ) || ( field_name == c_field_name_Internal ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Procedure->Internal( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Procedure->Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Procedure ) || ( field_name == c_field_name_Source_Procedure ) ) )
      {
         handled = true;

         string_getter< Meta_Procedure >( cmd_handler.p_Meta_Procedure->Source_Procedure( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Procedure_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Procedure_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_Procedure_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_Procedure, Meta_Permission >(
          *cmd_handler.p_Meta_Procedure, &Meta_Procedure::Access_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Restriction ) || ( field_name == c_field_name_Access_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_Procedure, int >(
          *cmd_handler.p_Meta_Procedure, &Meta_Procedure::Access_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Procedure, Meta_Class >(
          *cmd_handler.p_Meta_Procedure, &Meta_Procedure::Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Id ) || ( field_name == c_field_name_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_Procedure, string >(
          *cmd_handler.p_Meta_Procedure, &Meta_Procedure::Id, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Internal ) || ( field_name == c_field_name_Internal ) ) )
      {
         handled = true;

         func_string_setter< Meta_Procedure, bool >(
          *cmd_handler.p_Meta_Procedure, &Meta_Procedure::Internal, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Procedure, string >(
          *cmd_handler.p_Meta_Procedure, &Meta_Procedure::Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Procedure ) || ( field_name == c_field_name_Source_Procedure ) ) )
      {
         handled = true;

         func_string_setter< Meta_Procedure, Meta_Procedure >(
          *cmd_handler.p_Meta_Procedure, &Meta_Procedure::Source_Procedure, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Procedure_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Procedure_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_Procedure_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_Procedure->Access_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Procedure->Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Procedure ) || ( field_name == c_field_name_Source_Procedure ) )
         cmd_handler.retval = cmd_handler.p_Meta_Procedure->Source_Procedure( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
}

struct Meta_Procedure::impl : public Meta_Procedure_command_handler
{
   impl( Meta_Procedure& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Procedure( &o );

      add_commands( 0, Meta_Procedure_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Procedure_command_definitions ) );
   }

   Meta_Procedure& get_obj( ) const
   {
      return *cp_obj;
   }

   int impl_Access_Restriction( ) const { return lazy_fetch( p_obj ), v_Access_Restriction; }
   void impl_Access_Restriction( int Access_Restriction ) { v_Access_Restriction = Access_Restriction; }

   const string& impl_Id( ) const { return lazy_fetch( p_obj ), v_Id; }
   void impl_Id( const string& Id ) { sanity_check( Id ); v_Id = Id; }

   bool impl_Internal( ) const { return lazy_fetch( p_obj ), v_Internal; }
   void impl_Internal( bool Internal ) { v_Internal = Internal; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { sanity_check( Name ); v_Name = Name; }

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

   Meta_Procedure& impl_Source_Procedure( )
   {
      if( !cp_Source_Procedure )
      {
         cp_Source_Procedure.init( );

         p_obj->setup_graph_parent( *cp_Source_Procedure, c_field_id_Source_Procedure, v_Source_Procedure );
      }
      return *cp_Source_Procedure;
   }

   const Meta_Procedure& impl_Source_Procedure( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Procedure )
      {
         cp_Source_Procedure.init( );

         p_obj->setup_graph_parent( *cp_Source_Procedure, c_field_id_Source_Procedure, v_Source_Procedure );
      }
      return *cp_Source_Procedure;
   }

   void impl_Source_Procedure( const string& key )
   {
      class_base_accessor cba( impl_Source_Procedure( ) );
      cba.set_key( key );
   }

   Meta_Specification& impl_child_Specification_Other_Procedure_2( )
   {
      if( !cp_child_Specification_Other_Procedure_2 )
      {
         cp_child_Specification_Other_Procedure_2.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other_Procedure_2, "301452" );
      }
      return *cp_child_Specification_Other_Procedure_2;
   }

   const Meta_Specification& impl_child_Specification_Other_Procedure_2( ) const
   {
      if( !cp_child_Specification_Other_Procedure_2 )
      {
         cp_child_Specification_Other_Procedure_2.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other_Procedure_2, "301452" );
      }
      return *cp_child_Specification_Other_Procedure_2;
   }

   Meta_Specification& impl_child_Specification_Other( )
   {
      if( !cp_child_Specification_Other )
      {
         cp_child_Specification_Other.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other, "301451" );
      }
      return *cp_child_Specification_Other;
   }

   const Meta_Specification& impl_child_Specification_Other( ) const
   {
      if( !cp_child_Specification_Other )
      {
         cp_child_Specification_Other.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other, "301451" );
      }
      return *cp_child_Specification_Other;
   }

   Meta_Procedure& impl_child_Procedure_Source( )
   {
      if( !cp_child_Procedure_Source )
      {
         cp_child_Procedure_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Procedure_Source, "301110" );
      }
      return *cp_child_Procedure_Source;
   }

   const Meta_Procedure& impl_child_Procedure_Source( ) const
   {
      if( !cp_child_Procedure_Source )
      {
         cp_child_Procedure_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Procedure_Source, "301110" );
      }
      return *cp_child_Procedure_Source;
   }

   Meta_List_Field& impl_child_List_Field( )
   {
      if( !cp_child_List_Field )
      {
         cp_child_List_Field.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field, "302190" );
      }
      return *cp_child_List_Field;
   }

   const Meta_List_Field& impl_child_List_Field( ) const
   {
      if( !cp_child_List_Field )
      {
         cp_child_List_Field.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field, "302190" );
      }
      return *cp_child_List_Field;
   }

   Meta_Package_Option& impl_child_Package_Option( )
   {
      if( !cp_child_Package_Option )
      {
         cp_child_Package_Option.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option, "302833" );
      }
      return *cp_child_Package_Option;
   }

   const Meta_Package_Option& impl_child_Package_Option( ) const
   {
      if( !cp_child_Package_Option )
      {
         cp_child_Package_Option.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option, "302833" );
      }
      return *cp_child_Package_Option;
   }

   Meta_Procedure_Arg& impl_child_Procedure_Arg( )
   {
      if( !cp_child_Procedure_Arg )
      {
         cp_child_Procedure_Arg.init( );

         p_obj->setup_graph_parent( *cp_child_Procedure_Arg, "301200" );
      }
      return *cp_child_Procedure_Arg;
   }

   const Meta_Procedure_Arg& impl_child_Procedure_Arg( ) const
   {
      if( !cp_child_Procedure_Arg )
      {
         cp_child_Procedure_Arg.init( );

         p_obj->setup_graph_parent( *cp_child_Procedure_Arg, "301200" );
      }
      return *cp_child_Procedure_Arg;
   }

   Meta_Specification& impl_child_Specification( )
   {
      if( !cp_child_Specification )
      {
         cp_child_Specification.init( );

         p_obj->setup_graph_parent( *cp_child_Specification, "301450" );
      }
      return *cp_child_Specification;
   }

   const Meta_Specification& impl_child_Specification( ) const
   {
      if( !cp_child_Specification )
      {
         cp_child_Specification.init( );

         p_obj->setup_graph_parent( *cp_child_Specification, "301450" );
      }
      return *cp_child_Specification;
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

   Meta_Procedure* p_obj;
   class_pointer< Meta_Procedure > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   int v_Access_Restriction;
   string v_Id;
   bool v_Internal;
   string v_Name;

   string v_Access_Permission;
   mutable class_pointer< Meta_Permission > cp_Access_Permission;

   string v_Class;
   mutable class_pointer< Meta_Class > cp_Class;

   string v_Source_Procedure;
   mutable class_pointer< Meta_Procedure > cp_Source_Procedure;

   mutable class_pointer< Meta_Specification > cp_child_Specification_Other_Procedure_2;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Other;
   mutable class_pointer< Meta_Procedure > cp_child_Procedure_Source;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field;
   mutable class_pointer< Meta_Package_Option > cp_child_Package_Option;
   mutable class_pointer< Meta_Procedure_Arg > cp_child_Procedure_Arg;
   mutable class_pointer< Meta_Specification > cp_child_Specification;
};

string Meta_Procedure::impl::get_field_value( int field ) const
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
      retval = to_string( impl_Class( ) );
      break;

      case 3:
      retval = to_string( impl_Id( ) );
      break;

      case 4:
      retval = to_string( impl_Internal( ) );
      break;

      case 5:
      retval = to_string( impl_Name( ) );
      break;

      case 6:
      retval = to_string( impl_Source_Procedure( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_Procedure::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Procedure::impl, Meta_Permission >( *this, &Meta_Procedure::impl::impl_Access_Permission, value );
      break;

      case 1:
      func_string_setter< Meta_Procedure::impl, int >( *this, &Meta_Procedure::impl::impl_Access_Restriction, value );
      break;

      case 2:
      func_string_setter< Meta_Procedure::impl, Meta_Class >( *this, &Meta_Procedure::impl::impl_Class, value );
      break;

      case 3:
      func_string_setter< Meta_Procedure::impl, string >( *this, &Meta_Procedure::impl::impl_Id, value );
      break;

      case 4:
      func_string_setter< Meta_Procedure::impl, bool >( *this, &Meta_Procedure::impl::impl_Internal, value );
      break;

      case 5:
      func_string_setter< Meta_Procedure::impl, string >( *this, &Meta_Procedure::impl::impl_Name, value );
      break;

      case 6:
      func_string_setter< Meta_Procedure::impl, Meta_Procedure >( *this, &Meta_Procedure::impl::impl_Source_Procedure, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_Procedure::impl::set_field_default( int field )
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
      impl_Class( g_default_Class );
      break;

      case 3:
      impl_Id( g_default_Id );
      break;

      case 4:
      impl_Internal( g_default_Internal );
      break;

      case 5:
      impl_Name( g_default_Name );
      break;

      case 6:
      impl_Source_Procedure( g_default_Source_Procedure );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_Procedure::impl::is_field_default( int field ) const
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
      retval = ( v_Class == g_default_Class );
      break;

      case 3:
      retval = ( v_Id == g_default_Id );
      break;

      case 4:
      retval = ( v_Internal == g_default_Internal );
      break;

      case 5:
      retval = ( v_Name == g_default_Name );
      break;

      case 6:
      retval = ( v_Source_Procedure == g_default_Source_Procedure );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_Procedure::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start modifier_field_value)] 600086
   if( get_obj( ).Internal( ) == true )
      state |= c_modifier_Is_Internal;
   // [(finish modifier_field_value)] 600086

   // [(start protect_equal)] 600087
   if( check_equal( get_obj( ).Internal( ), true ) )
      state |= ( c_state_uneditable | c_state_undeletable );
   // [(finish protect_equal)] 600087

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

string Meta_Procedure::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   if( state & c_modifier_Is_Internal )
      state_names += "|" + string( "Is_Internal" );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_Procedure::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Procedure::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      impl_Access_Permission( "" );
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      impl_Class( "" );
   else if( ( field == c_field_id_Source_Procedure ) || ( field == c_field_name_Source_Procedure ) )
      impl_Source_Procedure( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Procedure::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      v_Access_Permission = value;
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      v_Class = value;
   else if( ( field == c_field_id_Source_Procedure ) || ( field == c_field_name_Source_Procedure ) )
      v_Source_Procedure = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Procedure::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      return v_Access_Permission;
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      return v_Class;
   else if( ( field == c_field_id_Source_Procedure ) || ( field == c_field_name_Source_Procedure ) )
      return v_Source_Procedure;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Procedure::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Access_Permission, v_Access_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Class, v_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Procedure, v_Source_Procedure ) );
}

void Meta_Procedure::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_Procedure::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Procedure::impl::clear( )
{
   v_Access_Restriction = g_default_Access_Restriction;
   v_Id = g_default_Id;
   v_Internal = g_default_Internal;
   v_Name = g_default_Name;

   v_Access_Permission = string( );
   if( cp_Access_Permission )
      p_obj->setup_foreign_key( *cp_Access_Permission, v_Access_Permission );

   v_Class = string( );
   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   v_Source_Procedure = string( );
   if( cp_Source_Procedure )
      p_obj->setup_foreign_key( *cp_Source_Procedure, v_Source_Procedure );
}

bool Meta_Procedure::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [(start parent_auto_int_inc)] 600090
   if( field_name == "Id" )
      return true;
   // [(finish parent_auto_int_inc)] 600090

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Procedure::impl::validate(
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
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Id ) ) ) ) );

   if( is_null( v_Name ) && !value_will_be_provided( c_field_name_Name ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Name ) ) ) ) );

   if( v_Class.empty( ) && !value_will_be_provided( c_field_name_Class ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Class ) ) ) ) );

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

   if( !g_view_access_restrict_enum.count( v_Access_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Access_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Access_Restriction ) ) ) ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Procedure::impl::validate_set_fields(
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

void Meta_Procedure::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Access_Permission )
      p_obj->setup_foreign_key( *cp_Access_Permission, v_Access_Permission );

   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   if( cp_Source_Procedure )
      p_obj->setup_foreign_key( *cp_Source_Procedure, v_Source_Procedure );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Procedure::impl::finalise_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Procedure::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Procedure::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_Procedure::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_empty_action)] 600088
   if( !get_obj( ).get_key( ).empty( ) )
   {
      if( !is_null( get_obj( ).Class( ).Source_Class( ) ) )
         get_obj( ).Internal( true );
   }
   // [(finish field_empty_action)] 600088

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Procedure::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start parent_auto_int_inc)] 600090
   if( is_create && is_null( get_obj( ).Id( ) ) )
   {
      if( is_null( get_obj( ).Class( ) ) )
         throw runtime_error( "unexpected empty Class" );

      get_obj( ).Class( ).op_update( get_obj( ).Class( ), FIELD_NAME( Meta, Class, Next_Procedure_Id ) );

      get_obj( ).Id( get_obj( ).Class( ).Next_Procedure_Id( ) );

      get_obj( ).Class( ).Next_Procedure_Id( auto_int_increment( get_obj( ).Class( ).Next_Procedure_Id( ) ) );
      get_obj( ).Class( ).op_apply( );
   }
   // [(finish parent_auto_int_inc)] 600090

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Procedure::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start update_children)] 600085
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
   // [(finish update_children)] 600085

   // [(start update_children)] 600085a
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
   // [(finish update_children)] 600085a

   // [(start update_children)] 600085b
   if( !is_create
    && !get_obj( ).get_is_for_peer( )
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Other_Procedure_2( ).iterate_forwards( ) )
   {
      do
      {
         if( !is_update_or_destroy_locked_by_own_session( get_obj( ).child_Specification_Other_Procedure_2( ) ) )
         {
            get_obj( ).child_Specification_Other_Procedure_2( ).op_update( );
            get_obj( ).child_Specification_Other_Procedure_2( ).op_apply( );
         }
      } while( get_obj( ).child_Specification_Other_Procedure_2( ).iterate_next( ) );
   }
   // [(finish update_children)] 600085b

   // [(start clone_children_from_fk)] 600089
   if( is_create && !is_null( get_obj( ).Source_Procedure( ) ) )
   {
      if( get_obj( ).Source_Procedure( ).child_Procedure_Arg( ).iterate_forwards( ) )
      {
         int child_num = 0;
         do
         {
            string key_info( construct_key_from_int( get_obj( ).get_key( ), ++child_num ) );
            key_info += ' ';
            key_info += get_obj( ).Source_Procedure( ).child_Procedure_Arg( ).get_key( );

            get_obj( ).child_Procedure_Arg( ).op_create( key_info );
            get_obj( ).child_Procedure_Arg( ).Procedure( get_obj( ).get_key( ) );
            get_obj( ).child_Procedure_Arg( ).Source_Procedure_Arg( get_obj( ).Source_Procedure( ).child_Procedure_Arg( ).get_key( ) );
            get_obj( ).child_Procedure_Arg( ).op_apply( );

         } while( get_obj( ).Source_Procedure( ).child_Procedure_Arg( ).iterate_next( ) );
      }
   }
   // [(finish clone_children_from_fk)] 600089

   // [(start clone_children_for_create)] 610090
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

         get_obj( ).Class( ).child_Class_Source( ).child_Procedure( ).op_create( key_info );
         get_obj( ).Class( ).child_Class_Source( ).child_Procedure( ).Class( get_obj( ).Class( ).child_Class_Source( ).get_key( ) );
         get_obj( ).Class( ).child_Class_Source( ).child_Procedure( ).Source_Procedure( get_obj( ).get_key( ) );

         get_obj( ).Class( ).child_Class_Source( ).child_Procedure( ).op_apply( );
      } while( get_obj( ).Class( ).child_Class_Source( ).iterate_next( ) );
   }
   // [(finish clone_children_for_create)] 610090

   // [(start clone_children_for_update)] 620090
   if( !is_create && get_obj( ).child_Procedure_Source( ).iterate_forwards( ) )
   {
      do
      {
         get_obj( ).child_Procedure_Source( ).op_update( );
         string parent( get_obj( ).child_Procedure_Source( ).Class( ) );

         get_obj( ).child_Procedure_Source( ).copy_all_field_values( get_obj( ) );

         get_obj( ).child_Procedure_Source( ).Class( parent );
         get_obj( ).child_Procedure_Source( ).Source_Procedure( get_obj( ).get_key( ) );

         get_obj( ).child_Procedure_Source( ).op_apply( );
      } while( get_obj( ).child_Procedure_Source( ).iterate_next( ) );
   }
   // [(finish clone_children_for_update)] 620090

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Procedure::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Procedure::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Procedure::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Procedure::impl::set_default_values( )
{
   clear( );
}

bool Meta_Procedure::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_Procedure::impl::get_required_transients( ) const
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

Meta_Procedure::Meta_Procedure( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Procedure::~Meta_Procedure( )
{
   cleanup( );
   delete p_impl;
}

int Meta_Procedure::Access_Restriction( ) const
{
   return p_impl->impl_Access_Restriction( );
}

void Meta_Procedure::Access_Restriction( int Access_Restriction )
{
   p_impl->impl_Access_Restriction( Access_Restriction );
}

const string& Meta_Procedure::Id( ) const
{
   return p_impl->impl_Id( );
}

void Meta_Procedure::Id( const string& Id )
{
   p_impl->impl_Id( Id );
}

bool Meta_Procedure::Internal( ) const
{
   return p_impl->impl_Internal( );
}

void Meta_Procedure::Internal( bool Internal )
{
   p_impl->impl_Internal( Internal );
}

const string& Meta_Procedure::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_Procedure::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

Meta_Permission& Meta_Procedure::Access_Permission( )
{
   return p_impl->impl_Access_Permission( );
}

const Meta_Permission& Meta_Procedure::Access_Permission( ) const
{
   return p_impl->impl_Access_Permission( );
}

void Meta_Procedure::Access_Permission( const string& key )
{
   p_impl->impl_Access_Permission( key );
}

Meta_Class& Meta_Procedure::Class( )
{
   return p_impl->impl_Class( );
}

const Meta_Class& Meta_Procedure::Class( ) const
{
   return p_impl->impl_Class( );
}

void Meta_Procedure::Class( const string& key )
{
   p_impl->impl_Class( key );
}

Meta_Procedure& Meta_Procedure::Source_Procedure( )
{
   return p_impl->impl_Source_Procedure( );
}

const Meta_Procedure& Meta_Procedure::Source_Procedure( ) const
{
   return p_impl->impl_Source_Procedure( );
}

void Meta_Procedure::Source_Procedure( const string& key )
{
   p_impl->impl_Source_Procedure( key );
}

Meta_Specification& Meta_Procedure::child_Specification_Other_Procedure_2( )
{
   return p_impl->impl_child_Specification_Other_Procedure_2( );
}

const Meta_Specification& Meta_Procedure::child_Specification_Other_Procedure_2( ) const
{
   return p_impl->impl_child_Specification_Other_Procedure_2( );
}

Meta_Specification& Meta_Procedure::child_Specification_Other( )
{
   return p_impl->impl_child_Specification_Other( );
}

const Meta_Specification& Meta_Procedure::child_Specification_Other( ) const
{
   return p_impl->impl_child_Specification_Other( );
}

Meta_Procedure& Meta_Procedure::child_Procedure_Source( )
{
   return p_impl->impl_child_Procedure_Source( );
}

const Meta_Procedure& Meta_Procedure::child_Procedure_Source( ) const
{
   return p_impl->impl_child_Procedure_Source( );
}

Meta_List_Field& Meta_Procedure::child_List_Field( )
{
   return p_impl->impl_child_List_Field( );
}

const Meta_List_Field& Meta_Procedure::child_List_Field( ) const
{
   return p_impl->impl_child_List_Field( );
}

Meta_Package_Option& Meta_Procedure::child_Package_Option( )
{
   return p_impl->impl_child_Package_Option( );
}

const Meta_Package_Option& Meta_Procedure::child_Package_Option( ) const
{
   return p_impl->impl_child_Package_Option( );
}

Meta_Procedure_Arg& Meta_Procedure::child_Procedure_Arg( )
{
   return p_impl->impl_child_Procedure_Arg( );
}

const Meta_Procedure_Arg& Meta_Procedure::child_Procedure_Arg( ) const
{
   return p_impl->impl_child_Procedure_Arg( );
}

Meta_Specification& Meta_Procedure::child_Specification( )
{
   return p_impl->impl_child_Specification( );
}

const Meta_Specification& Meta_Procedure::child_Specification( ) const
{
   return p_impl->impl_child_Specification( );
}

string Meta_Procedure::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Procedure::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_Procedure::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_Procedure::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_Procedure::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_Procedure::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_Procedure::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_Procedure::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_Procedure::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_Procedure::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Procedure::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_Procedure::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Procedure::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Procedure::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Procedure::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Procedure::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Procedure::clear( )
{
   p_impl->clear( );
}

void Meta_Procedure::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Procedure::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Procedure::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Procedure::finalise_fetch( bool skip_set_original )
{
   p_impl->finalise_fetch( skip_set_original );
}

void Meta_Procedure::at_create( )
{
   p_impl->at_create( );
}

void Meta_Procedure::post_init( )
{
   p_impl->post_init( );
}

void Meta_Procedure::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Procedure::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Procedure::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Procedure::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Procedure::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Procedure::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Procedure::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Procedure::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Procedure::get_field_id(
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
   else if( name == c_field_name_Class )
   {
      p_id = c_field_id_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

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
   else if( name == c_field_name_Name )
   {
      p_id = c_field_id_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_Procedure )
   {
      p_id = c_field_id_Source_Procedure;

      if( p_type_name )
         *p_type_name = "Meta_Procedure";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_id;
}

const char* Meta_Procedure::get_field_name(
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
   else if( id == c_field_id_Class )
   {
      p_name = c_field_name_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

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
   else if( id == c_field_id_Name )
   {
      p_name = c_field_name_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_Procedure )
   {
      p_name = c_field_name_Source_Procedure;

      if( p_type_name )
         *p_type_name = "Meta_Procedure";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_name;
}

string& Meta_Procedure::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_Procedure::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_Procedure::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_Procedure::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_Procedure::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_Procedure::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Procedure::get_field_uom_symbol( const string& id_or_name ) const
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
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
   {
      name = string( c_field_display_name_Class );
      get_module_string( c_field_display_name_Class, &next );
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
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
   {
      name = string( c_field_display_name_Name );
      get_module_string( c_field_display_name_Name, &next );
   }
   if( ( id_or_name == c_field_id_Source_Procedure ) || ( id_or_name == c_field_name_Source_Procedure ) )
   {
      name = string( c_field_display_name_Source_Procedure );
      get_module_string( c_field_display_name_Source_Procedure, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_Procedure::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Access_Permission ) || ( id_or_name == c_field_name_Access_Permission ) )
      display_name = get_module_string( c_field_display_name_Access_Permission );
   if( ( id_or_name == c_field_id_Access_Restriction ) || ( id_or_name == c_field_name_Access_Restriction ) )
      display_name = get_module_string( c_field_display_name_Access_Restriction );
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
      display_name = get_module_string( c_field_display_name_Class );
   if( ( id_or_name == c_field_id_Id ) || ( id_or_name == c_field_name_Id ) )
      display_name = get_module_string( c_field_display_name_Id );
   if( ( id_or_name == c_field_id_Internal ) || ( id_or_name == c_field_name_Internal ) )
      display_name = get_module_string( c_field_display_name_Internal );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
      display_name = get_module_string( c_field_display_name_Name );
   if( ( id_or_name == c_field_id_Source_Procedure ) || ( id_or_name == c_field_name_Source_Procedure ) )
      display_name = get_module_string( c_field_display_name_Source_Procedure );

   return display_name;
}

void Meta_Procedure::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Procedure::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Procedure::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Procedure::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Procedure::setup_foreign_key( Meta_Permission& o, const string& value )
{
   static_cast< Meta_Permission& >( o ).set_key( value );
}

void Meta_Procedure::setup_foreign_key( Meta_Class& o, const string& value )
{
   static_cast< Meta_Class& >( o ).set_key( value );
}

void Meta_Procedure::setup_foreign_key( Meta_Procedure& o, const string& value )
{
   static_cast< Meta_Procedure& >( o ).set_key( value );
}

void Meta_Procedure::setup_graph_parent( Meta_Specification& o, const string& foreign_key_field )
{
   static_cast< Meta_Specification& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Procedure::setup_graph_parent( Meta_Procedure& o, const string& foreign_key_field )
{
   static_cast< Meta_Procedure& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Procedure::setup_graph_parent( Meta_List_Field& o, const string& foreign_key_field )
{
   static_cast< Meta_List_Field& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Procedure::setup_graph_parent( Meta_Package_Option& o, const string& foreign_key_field )
{
   static_cast< Meta_Package_Option& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Procedure::setup_graph_parent( Meta_Procedure_Arg& o, const string& foreign_key_field )
{
   static_cast< Meta_Procedure_Arg& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Procedure::setup_graph_parent(
 Meta_Permission& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Permission& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Permission& >( o ).set_key( init_value );
}

void Meta_Procedure::setup_graph_parent(
 Meta_Class& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Class& >( o ).set_key( init_value );
}

void Meta_Procedure::setup_graph_parent(
 Meta_Procedure& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Procedure& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Procedure& >( o ).set_key( init_value );
}

size_t Meta_Procedure::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Procedure::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Procedure::get_num_foreign_key_children( bool is_internal ) const
{
   size_t rc = 7;

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

class_base* Meta_Procedure::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   if( child_num >= 7 )
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
            next_child_field = "301452";
            p_class_base = &child_Specification_Other_Procedure_2( );
         }
         break;

         case 1:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301451";
            p_class_base = &child_Specification_Other( );
         }
         break;

         case 2:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301110";
            p_class_base = &child_Procedure_Source( );
         }
         break;

         case 3:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302190";
            p_class_base = &child_List_Field( );
         }
         break;

         case 4:
         if( op == e_cascade_op_unlink )
         {
            next_child_field = "302833";
            p_class_base = &child_Package_Option( );
         }
         break;

         case 5:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301200";
            p_class_base = &child_Procedure_Arg( );
         }
         break;

         case 6:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301450";
            p_class_base = &child_Specification( );
         }
         break;
      }
   }

   return p_class_base;
}

void Meta_Procedure::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_Procedure::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_Procedure::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_Procedure::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_Procedure::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_Procedure::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_Procedure::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_Procedure::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "procedure";

   return get_module_string( key );
}

string Meta_Procedure::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_Procedure::get_create_instance_info( ) const
{
   return "";
}

string Meta_Procedure::get_update_instance_info( ) const
{
   return "";
}

string Meta_Procedure::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_Procedure::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );

   return retval;
}

bool Meta_Procedure::get_is_alias( ) const
{
   return false;
}

void Meta_Procedure::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Procedure::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Procedure::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == "_301452" ) || ( sub_context == "child_Specification_Other_Procedure_2" ) )
      p_class_base = &child_Specification_Other_Procedure_2( );
   else if( ( sub_context == "_301451" ) || ( sub_context == "child_Specification_Other" ) )
      p_class_base = &child_Specification_Other( );
   else if( ( sub_context == "_301110" ) || ( sub_context == "child_Procedure_Source" ) )
      p_class_base = &child_Procedure_Source( );
   else if( ( sub_context == "_302190" ) || ( sub_context == "child_List_Field" ) )
      p_class_base = &child_List_Field( );
   else if( ( sub_context == "_302833" ) || ( sub_context == "child_Package_Option" ) )
      p_class_base = &child_Package_Option( );
   else if( ( sub_context == "_301200" ) || ( sub_context == "child_Procedure_Arg" ) )
      p_class_base = &child_Procedure_Arg( );
   else if( ( sub_context == "_301450" ) || ( sub_context == "child_Specification" ) )
      p_class_base = &child_Specification( );
   else if( ( sub_context == c_field_id_Access_Permission ) || ( sub_context == c_field_name_Access_Permission ) )
      p_class_base = &Access_Permission( );
   else if( ( sub_context == c_field_id_Class ) || ( sub_context == c_field_name_Class ) )
      p_class_base = &Class( );
   else if( ( sub_context == c_field_id_Source_Procedure ) || ( sub_context == c_field_name_Source_Procedure ) )
      p_class_base = &Source_Procedure( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Procedure::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Access_Permission" );
   names.push_back( "C_Access_Restriction" );
   names.push_back( "C_Class" );
   names.push_back( "C_Id" );
   names.push_back( "C_Internal" );
   names.push_back( "C_Name" );
   names.push_back( "C_Source_Procedure" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Procedure::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Access_Permission( ) ) ) );
   values.push_back( to_string( Access_Restriction( ) ) );
   values.push_back( sql_quote( to_string( Class( ) ) ) );
   values.push_back( sql_quote( to_string( Id( ) ) ) );
   values.push_back( to_string( Internal( ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Procedure( ) ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Procedure::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Procedure::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start modifier_field_value)] 600086
   dependents.insert( "Internal" ); // (for Is_Internal modifier)

   if( ( use_transients && is_field_transient( e_field_id_Internal ) )
    || ( !use_transients && !is_field_transient( e_field_id_Internal ) ) )
      names.insert( "Internal" );
   // [(finish modifier_field_value)] 600086

   // [(start protect_equal)] 600087
   dependents.insert( "Internal" );

   if( ( use_transients && is_field_transient( e_field_id_Internal ) )
    || ( !use_transients && !is_field_transient( e_field_id_Internal ) ) )
      names.insert( "Internal" );
   // [(finish protect_equal)] 600087

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Procedure::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Procedure::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_Procedure::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Procedure::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Procedure::static_lock_class_id( )
{
   return "111100";
}

const char* Meta_Procedure::static_check_class_name( )
{
   return "Procedure";
}

const char* Meta_Procedure::static_persistence_extra( )
{
   return "";
}

bool Meta_Procedure::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Procedure::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.111100" );
}

void Meta_Procedure::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "301120", "Access_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "111104", "Access_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "301100", "Class", "Meta_Class", true, "", "" ) );
   all_field_info.push_back( field_info( "111103", "Id", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "111102", "Internal", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "111101", "Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "301110", "Source_Procedure", "Meta_Procedure", false, "", "" ) );
}

void Meta_Procedure::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Access_Permission, make_pair( "Meta.111100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Class, make_pair( "Meta.111100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Procedure, make_pair( "Meta.111100", "Meta_Procedure" ) ) );
}

int Meta_Procedure::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Procedure::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_Procedure::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Procedure::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "301120";
      break;

      case 2:
      p_id = "111104";
      break;

      case 3:
      p_id = "301100";
      break;

      case 4:
      p_id = "111103";
      break;

      case 5:
      p_id = "111102";
      break;

      case 6:
      p_id = "111101";
      break;

      case 7:
      p_id = "301110";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Procedure" );

   return p_id;
}

const char* Meta_Procedure::static_get_field_name( field_id id )
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
      p_id = "Class";
      break;

      case 4:
      p_id = "Id";
      break;

      case 5:
      p_id = "Internal";
      break;

      case 6:
      p_id = "Name";
      break;

      case 7:
      p_id = "Source_Procedure";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Procedure" );

   return p_id;
}

int Meta_Procedure::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Access_Permission || field == c_field_name_Access_Permission )
      rc += 1;
   else if( field == c_field_id_Access_Restriction || field == c_field_name_Access_Restriction )
      rc += 2;
   else if( field == c_field_id_Class || field == c_field_name_Class )
      rc += 3;
   else if( field == c_field_id_Id || field == c_field_name_Id )
      rc += 4;
   else if( field == c_field_id_Internal || field == c_field_name_Internal )
      rc += 5;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 6;
   else if( field == c_field_id_Source_Procedure || field == c_field_name_Source_Procedure )
      rc += 7;

   return rc - 1;
}

procedure_info_container& Meta_Procedure::static_get_procedure_info( )
{
   static procedure_info_container procedures;

   return procedures;
}

string Meta_Procedure::static_get_sql_columns( )
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
    "C_Class VARCHAR(75) NOT NULL,"
    "C_Id VARCHAR(200) NOT NULL,"
    "C_Internal INTEGER NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Source_Procedure VARCHAR(75) NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Procedure::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Procedure::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "enum_view_access_restrict_0", get_enum_string_view_access_restrict( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_access_restrict_1", get_enum_string_view_access_restrict( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_access_restrict_2", get_enum_string_view_access_restrict( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_access_restrict_3", get_enum_string_view_access_restrict( 3 ) ) );
   pairs.push_back( make_pair( "enum_view_access_restrict_4", get_enum_string_view_access_restrict( 4 ) ) );
}

void Meta_Procedure::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "Class,Id", "string,string" ) );
   pairs.push_back( make_pair( "Class,Name", "string,string" ) );
}

void Meta_Procedure::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "Class,Id" );
   unique_indexes.push_back( "Class,Name" );
}

void Meta_Procedure::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Class,C_Id" );
   indexes.push_back( "C_Class,C_Name" );
}

void Meta_Procedure::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Class,C_Id" );
   indexes.push_back( "C_Class,C_Name" );
}

void Meta_Procedure::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Procedure::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Procedure::static_insert_external_alias( const string& module_and_class_id, Meta_Procedure* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Procedure::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Procedure::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   g_view_access_restrict_enum.insert( 0 );
   g_view_access_restrict_enum.insert( 1 );
   g_view_access_restrict_enum.insert( 2 );
   g_view_access_restrict_enum.insert( 3 );
   g_view_access_restrict_enum.insert( 4 );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Procedure::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
