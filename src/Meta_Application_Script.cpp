// Copyright (c) 2012-2026 CIYAM Developers
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

#include "Meta_Application_Script.h"

#include "Meta_Application.h"
#include "Meta_Package_Type.h"

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

template< > inline string to_string( const Meta_Package_Type& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Package_Type& c, const string& s )
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

#include "Meta_Application_Script.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Name = "138101";
const char* const c_field_id_Other_Package_Types = "138105";
const char* const c_field_id_Package_Type = "302840";
const char* const c_field_id_Script_Name = "138103";
const char* const c_field_id_Version = "138102";

const char* const c_field_name_Name = "Name";
const char* const c_field_name_Other_Package_Types = "Other_Package_Types";
const char* const c_field_name_Package_Type = "Package_Type";
const char* const c_field_name_Script_Name = "Script_Name";
const char* const c_field_name_Version = "Version";

const char* const c_field_display_name_Name = "field_application_script_name";
const char* const c_field_display_name_Other_Package_Types = "field_application_script_other_package_types";
const char* const c_field_display_name_Package_Type = "field_application_script_package_type";
const char* const c_field_display_name_Script_Name = "field_application_script_script_name";
const char* const c_field_display_name_Version = "field_application_script_version";

const int c_num_fields = 5;

const char* const c_all_sorted_field_ids[ ] =
{
   "138101",
   "138102",
   "138103",
   "138105",
   "302840"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Name",
   "Other_Package_Types",
   "Package_Type",
   "Script_Name",
   "Version"
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

domain_string_max_size< 100 > g_Name_domain;
domain_string_max_size< 200 > g_Other_Package_Types_domain;
domain_string_max_size< 100 > g_Script_Name_domain;
domain_string_max_size< 5 > g_Version_domain;

string g_group_field_name;
string g_level_field_name;
string g_order_field_name;
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_Application_Script* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Application_Script* > external_aliases_lookup_container;
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

string g_default_Name = string( );
string g_default_Other_Package_Types = string( );
string g_default_Package_Type = string( );
string g_default_Script_Name = string( );
string g_default_Version = string( "0.1" );

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Application_Script > Application_Script_registration( get_class_registry( ), "138100" );

class Meta_Application_Script_command_functor;

class Meta_Application_Script_command_handler : public command_handler
{
   friend class Meta_Application_Script_command_functor;

   public:
   Meta_Application_Script_command_handler( ) : p_Meta_Application_Script( 0 ) { }

   void set_Meta_Application_Script( Meta_Application_Script* p_new_Meta_Application_Script ) { p_Meta_Application_Script = p_new_Meta_Application_Script; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Application_Script* p_Meta_Application_Script;

   protected:
   string retval;
};

class Meta_Application_Script_command_functor : public command_functor
{
   public:
   Meta_Application_Script_command_functor( Meta_Application_Script_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Application_Script_command_handler& cmd_handler;
};

command_functor* Meta_Application_Script_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Application_Script_command_functor( dynamic_cast< Meta_Application_Script_command_handler& >( handler ) );
}

void Meta_Application_Script_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Application_Script_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_Application_Script_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Application_Script->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Application_Script->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Application_Script_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Application_Script->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Application_Script->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Application_Script_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Application_Script_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Application_Script->Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Package_Types ) || ( field_name == c_field_name_Other_Package_Types ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Application_Script->Other_Package_Types( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Package_Type ) || ( field_name == c_field_name_Package_Type ) ) )
      {
         handled = true;

         string_getter< Meta_Package_Type >( cmd_handler.p_Meta_Application_Script->Package_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Script_Name ) || ( field_name == c_field_name_Script_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Application_Script->Script_Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Version ) || ( field_name == c_field_name_Version ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Application_Script->Version( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Application_Script_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Application_Script_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_Application_Script_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application_Script, string >(
          *cmd_handler.p_Meta_Application_Script, &Meta_Application_Script::Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Package_Types ) || ( field_name == c_field_name_Other_Package_Types ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application_Script, string >(
          *cmd_handler.p_Meta_Application_Script, &Meta_Application_Script::Other_Package_Types, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Package_Type ) || ( field_name == c_field_name_Package_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application_Script, Meta_Package_Type >(
          *cmd_handler.p_Meta_Application_Script, &Meta_Application_Script::Package_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Script_Name ) || ( field_name == c_field_name_Script_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application_Script, string >(
          *cmd_handler.p_Meta_Application_Script, &Meta_Application_Script::Script_Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Version ) || ( field_name == c_field_name_Version ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application_Script, string >(
          *cmd_handler.p_Meta_Application_Script, &Meta_Application_Script::Version, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Application_Script_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Application_Script_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_Application_Script_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_Package_Type ) || ( field_name == c_field_name_Package_Type ) )
         cmd_handler.retval = cmd_handler.p_Meta_Application_Script->Package_Type( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
}

struct Meta_Application_Script::impl : public Meta_Application_Script_command_handler
{
   impl( Meta_Application_Script& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Application_Script( &o );

      add_commands( 0, Meta_Application_Script_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Application_Script_command_definitions ) );
   }

   Meta_Application_Script& get_obj( ) const
   {
      return *cp_obj;
   }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { sanity_check( Name ); v_Name = Name; }

   const string& impl_Other_Package_Types( ) const { return lazy_fetch( p_obj ), v_Other_Package_Types; }
   void impl_Other_Package_Types( const string& Other_Package_Types ) { sanity_check( Other_Package_Types ); v_Other_Package_Types = Other_Package_Types; }

   const string& impl_Script_Name( ) const { return lazy_fetch( p_obj ), v_Script_Name; }
   void impl_Script_Name( const string& Script_Name ) { sanity_check( Script_Name ); v_Script_Name = Script_Name; }

   const string& impl_Version( ) const { return lazy_fetch( p_obj ), v_Version; }
   void impl_Version( const string& Version ) { sanity_check( Version ); v_Version = Version; }

   Meta_Package_Type& impl_Package_Type( )
   {
      if( !cp_Package_Type )
      {
         cp_Package_Type.init( );

         p_obj->setup_graph_parent( *cp_Package_Type, c_field_id_Package_Type, v_Package_Type );
      }
      return *cp_Package_Type;
   }

   const Meta_Package_Type& impl_Package_Type( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Package_Type )
      {
         cp_Package_Type.init( );

         p_obj->setup_graph_parent( *cp_Package_Type, c_field_id_Package_Type, v_Package_Type );
      }
      return *cp_Package_Type;
   }

   void impl_Package_Type( const string& key )
   {
      class_base_accessor cba( impl_Package_Type( ) );
      cba.set_key( key );
   }

   Meta_Application& impl_child_Application_Creation_Script( )
   {
      if( !cp_child_Application_Creation_Script )
      {
         cp_child_Application_Creation_Script.init( );

         p_obj->setup_graph_parent( *cp_child_Application_Creation_Script, "302225" );
      }
      return *cp_child_Application_Creation_Script;
   }

   const Meta_Application& impl_child_Application_Creation_Script( ) const
   {
      if( !cp_child_Application_Creation_Script )
      {
         cp_child_Application_Creation_Script.init( );

         p_obj->setup_graph_parent( *cp_child_Application_Creation_Script, "302225" );
      }
      return *cp_child_Application_Creation_Script;
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

   Meta_Application_Script* p_obj;
   class_pointer< Meta_Application_Script > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   string v_Name;
   string v_Other_Package_Types;
   string v_Script_Name;
   string v_Version;

   string v_Package_Type;
   mutable class_pointer< Meta_Package_Type > cp_Package_Type;

   mutable class_pointer< Meta_Application > cp_child_Application_Creation_Script;
};

string Meta_Application_Script::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Name( ) );
      break;

      case 1:
      retval = to_string( impl_Other_Package_Types( ) );
      break;

      case 2:
      retval = to_string( impl_Package_Type( ) );
      break;

      case 3:
      retval = to_string( impl_Script_Name( ) );
      break;

      case 4:
      retval = to_string( impl_Version( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_Application_Script::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Application_Script::impl, string >( *this, &Meta_Application_Script::impl::impl_Name, value );
      break;

      case 1:
      func_string_setter< Meta_Application_Script::impl, string >( *this, &Meta_Application_Script::impl::impl_Other_Package_Types, value );
      break;

      case 2:
      func_string_setter< Meta_Application_Script::impl, Meta_Package_Type >( *this, &Meta_Application_Script::impl::impl_Package_Type, value );
      break;

      case 3:
      func_string_setter< Meta_Application_Script::impl, string >( *this, &Meta_Application_Script::impl::impl_Script_Name, value );
      break;

      case 4:
      func_string_setter< Meta_Application_Script::impl, string >( *this, &Meta_Application_Script::impl::impl_Version, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_Application_Script::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Name( g_default_Name );
      break;

      case 1:
      impl_Other_Package_Types( g_default_Other_Package_Types );
      break;

      case 2:
      impl_Package_Type( g_default_Package_Type );
      break;

      case 3:
      impl_Script_Name( g_default_Script_Name );
      break;

      case 4:
      impl_Version( g_default_Version );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_Application_Script::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Name == g_default_Name );
      break;

      case 1:
      retval = ( v_Other_Package_Types == g_default_Other_Package_Types );
      break;

      case 2:
      retval = ( v_Package_Type == g_default_Package_Type );
      break;

      case 3:
      retval = ( v_Script_Name == g_default_Script_Name );
      break;

      case 4:
      retval = ( v_Version == g_default_Version );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_Application_Script::impl::get_state( ) const
{
   uint64_t state = 0;

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

string Meta_Application_Script::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_Application_Script::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Application_Script::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Package_Type ) || ( field == c_field_name_Package_Type ) )
      impl_Package_Type( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Application_Script::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_Package_Type ) || ( field == c_field_name_Package_Type ) )
      v_Package_Type = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Application_Script::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Package_Type ) || ( field == c_field_name_Package_Type ) )
      return v_Package_Type;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Application_Script::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Package_Type, v_Package_Type ) );
}

void Meta_Application_Script::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_Application_Script::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Application_Script::impl::clear( )
{
   v_Name = g_default_Name;
   v_Other_Package_Types = g_default_Other_Package_Types;
   v_Script_Name = g_default_Script_Name;
   v_Version = g_default_Version;

   v_Package_Type = string( );
   if( cp_Package_Type )
      p_obj->setup_foreign_key( *cp_Package_Type, v_Package_Type );
}

bool Meta_Application_Script::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Application_Script::impl::validate(
 uint64_t state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( is_null( v_Name ) && !value_will_be_provided( c_field_name_Name ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Name ) ) ) ) );

   if( is_null( v_Script_Name ) && !value_will_be_provided( c_field_name_Script_Name ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Script_Name,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Script_Name ) ) ) ) );

   if( is_null( v_Version ) && !value_will_be_provided( c_field_name_Version ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Version,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Version ) ) ) ) );

   if( v_Package_Type.empty( ) && !value_will_be_provided( c_field_name_Package_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Package_Type,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Package_Type ) ) ) ) );

   if( !is_null( v_Name )
    && ( v_Name != g_default_Name
    || !value_will_be_provided( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   if( !is_null( v_Other_Package_Types )
    && ( v_Other_Package_Types != g_default_Other_Package_Types
    || !value_will_be_provided( c_field_name_Other_Package_Types ) )
    && !g_Other_Package_Types_domain.is_valid( v_Other_Package_Types, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Other_Package_Types,
       get_module_string( c_field_display_name_Other_Package_Types ) + " " + error_message ) );

   if( !is_null( v_Script_Name )
    && ( v_Script_Name != g_default_Script_Name
    || !value_will_be_provided( c_field_name_Script_Name ) )
    && !g_Script_Name_domain.is_valid( v_Script_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Script_Name,
       get_module_string( c_field_display_name_Script_Name ) + " " + error_message ) );

   if( !is_null( v_Version )
    && ( v_Version != g_default_Version
    || !value_will_be_provided( c_field_name_Version ) )
    && !g_Version_domain.is_valid( v_Version, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Version,
       get_module_string( c_field_display_name_Version ) + " " + error_message ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Application_Script::impl::validate_set_fields(
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

   if( !is_null( v_Other_Package_Types )
    && ( fields_set.count( c_field_id_Other_Package_Types ) || fields_set.count( c_field_name_Other_Package_Types ) )
    && !g_Other_Package_Types_domain.is_valid( v_Other_Package_Types, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Other_Package_Types,
       get_module_string( c_field_display_name_Other_Package_Types ) + " " + error_message ) );

   if( !is_null( v_Script_Name )
    && ( fields_set.count( c_field_id_Script_Name ) || fields_set.count( c_field_name_Script_Name ) )
    && !g_Script_Name_domain.is_valid( v_Script_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Script_Name,
       get_module_string( c_field_display_name_Script_Name ) + " " + error_message ) );

   if( !is_null( v_Version )
    && ( fields_set.count( c_field_id_Version ) || fields_set.count( c_field_name_Version ) )
    && !g_Version_domain.is_valid( v_Version, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Version,
       get_module_string( c_field_display_name_Version ) + " " + error_message ) );
}

void Meta_Application_Script::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Package_Type )
      p_obj->setup_foreign_key( *cp_Package_Type, v_Package_Type );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Application_Script::impl::completed_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start completed_fetch>]
   // [<finish completed_fetch>]
}

void Meta_Application_Script::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Application_Script::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_Application_Script::impl::to_store( bool is_create, bool is_internal )
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

void Meta_Application_Script::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Application_Script::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Application_Script::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Application_Script::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Application_Script::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Application_Script::impl::set_default_values( )
{
   clear( );
}

bool Meta_Application_Script::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_Application_Script::impl::get_required_transients( ) const
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

Meta_Application_Script::Meta_Application_Script( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Application_Script::~Meta_Application_Script( )
{
   cleanup( );
   delete p_impl;
}

const string& Meta_Application_Script::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_Application_Script::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

const string& Meta_Application_Script::Other_Package_Types( ) const
{
   return p_impl->impl_Other_Package_Types( );
}

void Meta_Application_Script::Other_Package_Types( const string& Other_Package_Types )
{
   p_impl->impl_Other_Package_Types( Other_Package_Types );
}

const string& Meta_Application_Script::Script_Name( ) const
{
   return p_impl->impl_Script_Name( );
}

void Meta_Application_Script::Script_Name( const string& Script_Name )
{
   p_impl->impl_Script_Name( Script_Name );
}

const string& Meta_Application_Script::Version( ) const
{
   return p_impl->impl_Version( );
}

void Meta_Application_Script::Version( const string& Version )
{
   p_impl->impl_Version( Version );
}

Meta_Package_Type& Meta_Application_Script::Package_Type( )
{
   return p_impl->impl_Package_Type( );
}

const Meta_Package_Type& Meta_Application_Script::Package_Type( ) const
{
   return p_impl->impl_Package_Type( );
}

void Meta_Application_Script::Package_Type( const string& key )
{
   p_impl->impl_Package_Type( key );
}

Meta_Application& Meta_Application_Script::child_Application_Creation_Script( )
{
   return p_impl->impl_child_Application_Creation_Script( );
}

const Meta_Application& Meta_Application_Script::child_Application_Creation_Script( ) const
{
   return p_impl->impl_child_Application_Creation_Script( );
}

string Meta_Application_Script::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Application_Script::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_Application_Script::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_Application_Script::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_Application_Script::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_Application_Script::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_Application_Script::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_Application_Script::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_Application_Script::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_Application_Script::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Application_Script::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_Application_Script::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Application_Script::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Application_Script::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Application_Script::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Application_Script::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Application_Script::clear( )
{
   p_impl->clear( );
}

void Meta_Application_Script::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Application_Script::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Application_Script::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Application_Script::completed_fetch( bool skip_set_original )
{
   p_impl->completed_fetch( skip_set_original );
}

void Meta_Application_Script::at_create( )
{
   p_impl->at_create( );
}

void Meta_Application_Script::post_init( )
{
   p_impl->post_init( );
}

void Meta_Application_Script::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Application_Script::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Application_Script::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Application_Script::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Application_Script::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Application_Script::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Application_Script::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Application_Script::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Application_Script::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( 0 );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Name )
   {
      p_id = c_field_id_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Package_Types )
   {
      p_id = c_field_id_Other_Package_Types;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Package_Type )
   {
      p_id = c_field_id_Package_Type;

      if( p_type_name )
         *p_type_name = "Meta_Package_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Script_Name )
   {
      p_id = c_field_id_Script_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Version )
   {
      p_id = c_field_id_Version;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_id;
}

const char* Meta_Application_Script::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( 0 );

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Name )
   {
      p_name = c_field_name_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Package_Types )
   {
      p_name = c_field_name_Other_Package_Types;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Package_Type )
   {
      p_name = c_field_name_Package_Type;

      if( p_type_name )
         *p_type_name = "Meta_Package_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Script_Name )
   {
      p_name = c_field_name_Script_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Version )
   {
      p_name = c_field_name_Version;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_name;
}

string& Meta_Application_Script::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_Application_Script::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_Application_Script::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_Application_Script::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_Application_Script::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_Application_Script::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Application_Script::get_field_uom_symbol( const string& id_or_name ) const
{
   string uom_symbol;

   string name;
   pair< string, string > next;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_uom_symbol" );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
   {
      name = string( c_field_display_name_Name );
      get_module_string( c_field_display_name_Name, &next );
   }
   if( ( id_or_name == c_field_id_Other_Package_Types ) || ( id_or_name == c_field_name_Other_Package_Types ) )
   {
      name = string( c_field_display_name_Other_Package_Types );
      get_module_string( c_field_display_name_Other_Package_Types, &next );
   }
   if( ( id_or_name == c_field_id_Package_Type ) || ( id_or_name == c_field_name_Package_Type ) )
   {
      name = string( c_field_display_name_Package_Type );
      get_module_string( c_field_display_name_Package_Type, &next );
   }
   if( ( id_or_name == c_field_id_Script_Name ) || ( id_or_name == c_field_name_Script_Name ) )
   {
      name = string( c_field_display_name_Script_Name );
      get_module_string( c_field_display_name_Script_Name, &next );
   }
   if( ( id_or_name == c_field_id_Version ) || ( id_or_name == c_field_name_Version ) )
   {
      name = string( c_field_display_name_Version );
      get_module_string( c_field_display_name_Version, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_Application_Script::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
      display_name = get_module_string( c_field_display_name_Name );
   if( ( id_or_name == c_field_id_Other_Package_Types ) || ( id_or_name == c_field_name_Other_Package_Types ) )
      display_name = get_module_string( c_field_display_name_Other_Package_Types );
   if( ( id_or_name == c_field_id_Package_Type ) || ( id_or_name == c_field_name_Package_Type ) )
      display_name = get_module_string( c_field_display_name_Package_Type );
   if( ( id_or_name == c_field_id_Script_Name ) || ( id_or_name == c_field_name_Script_Name ) )
      display_name = get_module_string( c_field_display_name_Script_Name );
   if( ( id_or_name == c_field_id_Version ) || ( id_or_name == c_field_name_Version ) )
      display_name = get_module_string( c_field_display_name_Version );

   return display_name;
}

void Meta_Application_Script::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Application_Script::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Application_Script::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Application_Script::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Application_Script::setup_foreign_key( Meta_Package_Type& o, const string& value )
{
   static_cast< Meta_Package_Type& >( o ).set_key( value );
}

void Meta_Application_Script::setup_graph_parent( Meta_Application& o, const string& foreign_key_field )
{
   static_cast< Meta_Application& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Application_Script::setup_graph_parent(
 Meta_Package_Type& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Package_Type& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Package_Type& >( o ).set_key( init_value );
}

size_t Meta_Application_Script::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Application_Script::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Application_Script::get_num_foreign_key_children( bool is_internal ) const
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

class_base* Meta_Application_Script::get_next_foreign_key_child(
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
            next_child_field = "302225";
            p_class_base = &child_Application_Creation_Script( );
         }
         break;
      }
   }

   return p_class_base;
}

void Meta_Application_Script::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_Application_Script::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_Application_Script::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_Application_Script::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_Application_Script::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_Application_Script::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_Application_Script::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_Application_Script::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "application_script";

   return get_module_string( key );
}

string Meta_Application_Script::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_Application_Script::get_create_instance_info( ) const
{
   return "";
}

string Meta_Application_Script::get_update_instance_info( ) const
{
   return "";
}

string Meta_Application_Script::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_Application_Script::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );

   return retval;
}

bool Meta_Application_Script::get_is_alias( ) const
{
   return false;
}

void Meta_Application_Script::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Application_Script::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Application_Script::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == "_302225" ) || ( sub_context == "child_Application_Creation_Script" ) )
      p_class_base = &child_Application_Creation_Script( );
   else if( ( sub_context == c_field_id_Package_Type ) || ( sub_context == c_field_name_Package_Type ) )
      p_class_base = &Package_Type( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Application_Script::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Name" );
   names.push_back( "C_Other_Package_Types" );
   names.push_back( "C_Package_Type" );
   names.push_back( "C_Script_Name" );
   names.push_back( "C_Version" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Application_Script::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Package_Types( ) ) ) );
   values.push_back( sql_quote( to_string( Package_Type( ) ) ) );
   values.push_back( sql_quote( to_string( Script_Name( ) ) ) );
   values.push_back( sql_quote( to_string( Version( ) ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Application_Script::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Application_Script::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Application_Script::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Application_Script::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_Application_Script::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Application_Script::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Application_Script::static_lock_class_id( )
{
   return "138100";
}

const char* Meta_Application_Script::static_check_class_name( )
{
   return "Application_Script";
}

const char* Meta_Application_Script::static_persistence_extra( )
{
   return "";
}

bool Meta_Application_Script::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Application_Script::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.138100" );
}

void Meta_Application_Script::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "138101", "Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "138105", "Other_Package_Types", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "302840", "Package_Type", "Meta_Package_Type", true, "", "" ) );
   all_field_info.push_back( field_info( "138103", "Script_Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "138102", "Version", "string", false, "", "" ) );
}

void Meta_Application_Script::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Package_Type, make_pair( "Meta.138100", "Meta_Package_Type" ) ) );
}

int Meta_Application_Script::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Application_Script::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_Application_Script::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Application_Script::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "138101";
      break;

      case 2:
      p_id = "138105";
      break;

      case 3:
      p_id = "302840";
      break;

      case 4:
      p_id = "138103";
      break;

      case 5:
      p_id = "138102";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Application_Script" );

   return p_id;
}

const char* Meta_Application_Script::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Name";
      break;

      case 2:
      p_id = "Other_Package_Types";
      break;

      case 3:
      p_id = "Package_Type";
      break;

      case 4:
      p_id = "Script_Name";
      break;

      case 5:
      p_id = "Version";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Application_Script" );

   return p_id;
}

int Meta_Application_Script::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( ( field == c_field_id_Name ) || ( field == c_field_name_Name ) )
      rc += 1;
   else if( ( field == c_field_id_Other_Package_Types ) || ( field == c_field_name_Other_Package_Types ) )
      rc += 2;
   else if( ( field == c_field_id_Package_Type ) || ( field == c_field_name_Package_Type ) )
      rc += 3;
   else if( ( field == c_field_id_Script_Name ) || ( field == c_field_name_Script_Name ) )
      rc += 4;
   else if( ( field == c_field_id_Version ) || ( field == c_field_name_Version ) )
      rc += 5;

   return rc - 1;
}

procedure_info_container& Meta_Application_Script::static_get_procedure_info( )
{
   static procedure_info_container procedures;

   return procedures;
}

string Meta_Application_Script::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(75),"
    "C_Ver_ SMALLINT UNSIGNED NOT NULL,"
    "C_Rev_ BIGINT UNSIGNED NOT NULL,"
    "C_Sec_ BIGINT UNSIGNED NOT NULL,"
    "C_Typ_ VARCHAR(16) NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Other_Package_Types VARCHAR(200) NOT NULL,"
    "C_Package_Type VARCHAR(75) NOT NULL,"
    "C_Script_Name VARCHAR(200) NOT NULL,"
    "C_Version VARCHAR(200) NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Application_Script::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Application_Script::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   ( void )pairs;
}

void Meta_Application_Script::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "Name", "string" ) );
}

void Meta_Application_Script::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "Name" );
}

void Meta_Application_Script::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Name" );
}

void Meta_Application_Script::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Name" );
}

void Meta_Application_Script::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Application_Script::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Application_Script::static_insert_external_alias( const string& module_and_class_id, Meta_Application_Script* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Application_Script::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Application_Script::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Application_Script::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
