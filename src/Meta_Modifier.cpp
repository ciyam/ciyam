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

#include "Meta_Modifier.h"

#include "Meta_List_Field.h"
#include "Meta_List.h"
#include "Meta_Specification.h"
#include "Meta_Modifier_Affect.h"
#include "Meta_Package_Option.h"
#include "Meta_Class.h"
#include "Meta_Modifier.h"

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

// [(start clone_children_for_create)] 610079
#include "Meta_Class.h"
// [(finish clone_children_for_create)] 610079

// [<start includes>]
// [<finish includes>]

using namespace std;

// [<start namespaces>]
// [<finish namespaces>]

template< > inline string to_string( const Meta_Class& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Modifier& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Class& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Modifier& c, const string& s )
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

#include "Meta_Modifier.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Class = "300900";
const char* const c_field_id_Internal = "109102";
const char* const c_field_id_Name = "109101";
const char* const c_field_id_Source_Modifier = "300910";

const char* const c_field_name_Class = "Class";
const char* const c_field_name_Internal = "Internal";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Source_Modifier = "Source_Modifier";

const char* const c_field_display_name_Class = "field_modifier_class";
const char* const c_field_display_name_Internal = "field_modifier_internal";
const char* const c_field_display_name_Name = "field_modifier_name";
const char* const c_field_display_name_Source_Modifier = "field_modifier_source_modifier";

const int c_num_fields = 4;

const char* const c_all_sorted_field_ids[ ] =
{
   "109101",
   "109102",
   "300900",
   "300910"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Class",
   "Internal",
   "Name",
   "Source_Modifier"
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
 domain_string_max_size< 30 > > g_Name_domain;

string g_group_field_name;
string g_level_field_name;
string g_order_field_name;
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_Modifier* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Modifier* > external_aliases_lookup_container;
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

string g_default_Class = string( );
bool g_default_Internal = bool( 0 );
string g_default_Name = string( );
string g_default_Source_Modifier = string( );

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Modifier > Modifier_registration( get_class_registry( ), "109100" );

class Meta_Modifier_command_functor;

class Meta_Modifier_command_handler : public command_handler
{
   friend class Meta_Modifier_command_functor;

   public:
   Meta_Modifier_command_handler( ) : p_Meta_Modifier( 0 ) { }

   void set_Meta_Modifier( Meta_Modifier* p_new_Meta_Modifier ) { p_Meta_Modifier = p_new_Meta_Modifier; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Modifier* p_Meta_Modifier;

   protected:
   string retval;
};

class Meta_Modifier_command_functor : public command_functor
{
   public:
   Meta_Modifier_command_functor( Meta_Modifier_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Modifier_command_handler& cmd_handler;
};

command_functor* Meta_Modifier_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Modifier_command_functor( dynamic_cast< Meta_Modifier_command_handler& >( handler ) );
}

void Meta_Modifier_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Modifier_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_Modifier_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Modifier->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Modifier->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Modifier_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Modifier->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Modifier->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Modifier_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Modifier_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Modifier->Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Internal ) || ( field_name == c_field_name_Internal ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Modifier->Internal( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Modifier->Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Modifier ) || ( field_name == c_field_name_Source_Modifier ) ) )
      {
         handled = true;

         string_getter< Meta_Modifier >( cmd_handler.p_Meta_Modifier->Source_Modifier( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Modifier_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Modifier_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_Modifier_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Modifier, Meta_Class >(
          *cmd_handler.p_Meta_Modifier, &Meta_Modifier::Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Internal ) || ( field_name == c_field_name_Internal ) ) )
      {
         handled = true;

         func_string_setter< Meta_Modifier, bool >(
          *cmd_handler.p_Meta_Modifier, &Meta_Modifier::Internal, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Modifier, string >(
          *cmd_handler.p_Meta_Modifier, &Meta_Modifier::Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Modifier ) || ( field_name == c_field_name_Source_Modifier ) ) )
      {
         handled = true;

         func_string_setter< Meta_Modifier, Meta_Modifier >(
          *cmd_handler.p_Meta_Modifier, &Meta_Modifier::Source_Modifier, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Modifier_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Modifier_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_Modifier_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Modifier->Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Modifier ) || ( field_name == c_field_name_Source_Modifier ) )
         cmd_handler.retval = cmd_handler.p_Meta_Modifier->Source_Modifier( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
}

struct Meta_Modifier::impl : public Meta_Modifier_command_handler
{
   impl( Meta_Modifier& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Modifier( &o );

      add_commands( 0, Meta_Modifier_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Modifier_command_definitions ) );
   }

   Meta_Modifier& get_obj( ) const
   {
      return *cp_obj;
   }

   bool impl_Internal( ) const { return lazy_fetch( p_obj ), v_Internal; }
   void impl_Internal( bool Internal ) { v_Internal = Internal; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { sanity_check( Name ); v_Name = Name; }

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

   Meta_Modifier& impl_Source_Modifier( )
   {
      if( !cp_Source_Modifier )
      {
         cp_Source_Modifier.init( );

         p_obj->setup_graph_parent( *cp_Source_Modifier, c_field_id_Source_Modifier, v_Source_Modifier );
      }
      return *cp_Source_Modifier;
   }

   const Meta_Modifier& impl_Source_Modifier( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Modifier )
      {
         cp_Source_Modifier.init( );

         p_obj->setup_graph_parent( *cp_Source_Modifier, c_field_id_Source_Modifier, v_Source_Modifier );
      }
      return *cp_Source_Modifier;
   }

   void impl_Source_Modifier( const string& key )
   {
      class_base_accessor cba( impl_Source_Modifier( ) );
      cba.set_key( key );
   }

   Meta_List_Field& impl_child_List_Field_Access_Parent( )
   {
      if( !cp_child_List_Field_Access_Parent )
      {
         cp_child_List_Field_Access_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Access_Parent, "302199" );
      }
      return *cp_child_List_Field_Access_Parent;
   }

   const Meta_List_Field& impl_child_List_Field_Access_Parent( ) const
   {
      if( !cp_child_List_Field_Access_Parent )
      {
         cp_child_List_Field_Access_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Access_Parent, "302199" );
      }
      return *cp_child_List_Field_Access_Parent;
   }

   Meta_List& impl_child_List_Access_Parent( )
   {
      if( !cp_child_List_Access_Parent )
      {
         cp_child_List_Access_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Access_Parent, "301998" );
      }
      return *cp_child_List_Access_Parent;
   }

   const Meta_List& impl_child_List_Access_Parent( ) const
   {
      if( !cp_child_List_Access_Parent )
      {
         cp_child_List_Access_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Access_Parent, "301998" );
      }
      return *cp_child_List_Access_Parent;
   }

   Meta_List& impl_child_List_Create_Parent( )
   {
      if( !cp_child_List_Create_Parent )
      {
         cp_child_List_Create_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Create_Parent, "301998a" );
      }
      return *cp_child_List_Create_Parent;
   }

   const Meta_List& impl_child_List_Create_Parent( ) const
   {
      if( !cp_child_List_Create_Parent )
      {
         cp_child_List_Create_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Create_Parent, "301998a" );
      }
      return *cp_child_List_Create_Parent;
   }

   Meta_List& impl_child_List_Destroy_Parent( )
   {
      if( !cp_child_List_Destroy_Parent )
      {
         cp_child_List_Destroy_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Destroy_Parent, "301998b" );
      }
      return *cp_child_List_Destroy_Parent;
   }

   const Meta_List& impl_child_List_Destroy_Parent( ) const
   {
      if( !cp_child_List_Destroy_Parent )
      {
         cp_child_List_Destroy_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Destroy_Parent, "301998b" );
      }
      return *cp_child_List_Destroy_Parent;
   }

   Meta_Specification& impl_child_Specification_Other_Modifier_2( )
   {
      if( !cp_child_Specification_Other_Modifier_2 )
      {
         cp_child_Specification_Other_Modifier_2.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other_Modifier_2, "301442" );
      }
      return *cp_child_Specification_Other_Modifier_2;
   }

   const Meta_Specification& impl_child_Specification_Other_Modifier_2( ) const
   {
      if( !cp_child_Specification_Other_Modifier_2 )
      {
         cp_child_Specification_Other_Modifier_2.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other_Modifier_2, "301442" );
      }
      return *cp_child_Specification_Other_Modifier_2;
   }

   Meta_Specification& impl_child_Specification_Other( )
   {
      if( !cp_child_Specification_Other )
      {
         cp_child_Specification_Other.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other, "301441" );
      }
      return *cp_child_Specification_Other;
   }

   const Meta_Specification& impl_child_Specification_Other( ) const
   {
      if( !cp_child_Specification_Other )
      {
         cp_child_Specification_Other.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other, "301441" );
      }
      return *cp_child_Specification_Other;
   }

   Meta_Modifier& impl_child_Modifier_Source( )
   {
      if( !cp_child_Modifier_Source )
      {
         cp_child_Modifier_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Modifier_Source, "300910" );
      }
      return *cp_child_Modifier_Source;
   }

   const Meta_Modifier& impl_child_Modifier_Source( ) const
   {
      if( !cp_child_Modifier_Source )
      {
         cp_child_Modifier_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Modifier_Source, "300910" );
      }
      return *cp_child_Modifier_Source;
   }

   Meta_Modifier_Affect& impl_child_Modifier_Affect( )
   {
      if( !cp_child_Modifier_Affect )
      {
         cp_child_Modifier_Affect.init( );

         p_obj->setup_graph_parent( *cp_child_Modifier_Affect, "301000" );
      }
      return *cp_child_Modifier_Affect;
   }

   const Meta_Modifier_Affect& impl_child_Modifier_Affect( ) const
   {
      if( !cp_child_Modifier_Affect )
      {
         cp_child_Modifier_Affect.init( );

         p_obj->setup_graph_parent( *cp_child_Modifier_Affect, "301000" );
      }
      return *cp_child_Modifier_Affect;
   }

   Meta_Package_Option& impl_child_Package_Option( )
   {
      if( !cp_child_Package_Option )
      {
         cp_child_Package_Option.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option, "302832" );
      }
      return *cp_child_Package_Option;
   }

   const Meta_Package_Option& impl_child_Package_Option( ) const
   {
      if( !cp_child_Package_Option )
      {
         cp_child_Package_Option.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option, "302832" );
      }
      return *cp_child_Package_Option;
   }

   Meta_Specification& impl_child_Specification( )
   {
      if( !cp_child_Specification )
      {
         cp_child_Specification.init( );

         p_obj->setup_graph_parent( *cp_child_Specification, "301440" );
      }
      return *cp_child_Specification;
   }

   const Meta_Specification& impl_child_Specification( ) const
   {
      if( !cp_child_Specification )
      {
         cp_child_Specification.init( );

         p_obj->setup_graph_parent( *cp_child_Specification, "301440" );
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

   Meta_Modifier* p_obj;
   class_pointer< Meta_Modifier > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   bool v_Internal;
   string v_Name;

   string v_Class;
   mutable class_pointer< Meta_Class > cp_Class;

   string v_Source_Modifier;
   mutable class_pointer< Meta_Modifier > cp_Source_Modifier;

   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Access_Parent;
   mutable class_pointer< Meta_List > cp_child_List_Access_Parent;
   mutable class_pointer< Meta_List > cp_child_List_Create_Parent;
   mutable class_pointer< Meta_List > cp_child_List_Destroy_Parent;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Other_Modifier_2;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Other;
   mutable class_pointer< Meta_Modifier > cp_child_Modifier_Source;
   mutable class_pointer< Meta_Modifier_Affect > cp_child_Modifier_Affect;
   mutable class_pointer< Meta_Package_Option > cp_child_Package_Option;
   mutable class_pointer< Meta_Specification > cp_child_Specification;
};

string Meta_Modifier::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Class( ) );
      break;

      case 1:
      retval = to_string( impl_Internal( ) );
      break;

      case 2:
      retval = to_string( impl_Name( ) );
      break;

      case 3:
      retval = to_string( impl_Source_Modifier( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_Modifier::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Modifier::impl, Meta_Class >( *this, &Meta_Modifier::impl::impl_Class, value );
      break;

      case 1:
      func_string_setter< Meta_Modifier::impl, bool >( *this, &Meta_Modifier::impl::impl_Internal, value );
      break;

      case 2:
      func_string_setter< Meta_Modifier::impl, string >( *this, &Meta_Modifier::impl::impl_Name, value );
      break;

      case 3:
      func_string_setter< Meta_Modifier::impl, Meta_Modifier >( *this, &Meta_Modifier::impl::impl_Source_Modifier, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_Modifier::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Class( g_default_Class );
      break;

      case 1:
      impl_Internal( g_default_Internal );
      break;

      case 2:
      impl_Name( g_default_Name );
      break;

      case 3:
      impl_Source_Modifier( g_default_Source_Modifier );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_Modifier::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Class == g_default_Class );
      break;

      case 1:
      retval = ( v_Internal == g_default_Internal );
      break;

      case 2:
      retval = ( v_Name == g_default_Name );
      break;

      case 3:
      retval = ( v_Source_Modifier == g_default_Source_Modifier );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_Modifier::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start modifier_field_value)] 600076
   if( get_obj( ).Internal( ) == true )
      state |= c_modifier_Is_Internal;
   // [(finish modifier_field_value)] 600076

   // [(start protect_equal)] 600077
   if( check_equal( get_obj( ).Internal( ), true ) )
      state |= ( c_state_uneditable | c_state_undeletable );
   // [(finish protect_equal)] 600077

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

string Meta_Modifier::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   if( state & c_modifier_Is_Internal )
      state_names += "|" + string( "Is_Internal" );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_Modifier::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Modifier::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      impl_Class( "" );
   else if( ( field == c_field_id_Source_Modifier ) || ( field == c_field_name_Source_Modifier ) )
      impl_Source_Modifier( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Modifier::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      v_Class = value;
   else if( ( field == c_field_id_Source_Modifier ) || ( field == c_field_name_Source_Modifier ) )
      v_Source_Modifier = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Modifier::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      return v_Class;
   else if( ( field == c_field_id_Source_Modifier ) || ( field == c_field_name_Source_Modifier ) )
      return v_Source_Modifier;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Modifier::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Class, v_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Modifier, v_Source_Modifier ) );
}

void Meta_Modifier::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_Modifier::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Modifier::impl::clear( )
{
   v_Internal = g_default_Internal;
   v_Name = g_default_Name;

   v_Class = string( );
   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   v_Source_Modifier = string( );
   if( cp_Source_Modifier )
      p_obj->setup_foreign_key( *cp_Source_Modifier, v_Source_Modifier );
}

bool Meta_Modifier::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Modifier::impl::validate(
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
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Name ) ) ) ) );

   if( v_Class.empty( ) && !value_will_be_provided( c_field_name_Class ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Class ) ) ) ) );

   if( !is_null( v_Name )
    && ( v_Name != g_default_Name
    || !value_will_be_provided( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Modifier::impl::validate_set_fields(
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
}

void Meta_Modifier::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   if( cp_Source_Modifier )
      p_obj->setup_foreign_key( *cp_Source_Modifier, v_Source_Modifier );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Modifier::impl::finalise_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Modifier::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Modifier::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_Modifier::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_empty_action)] 600078
   if( !get_obj( ).get_key( ).empty( ) )
   {
      if( !is_null( get_obj( ).Class( ).Source_Class( ) ) )
         get_obj( ).Internal( true );
   }
   // [(finish field_empty_action)] 600078

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Modifier::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Modifier::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start update_children)] 600075
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
   // [(finish update_children)] 600075

   // [(start update_children)] 600075a
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
   // [(finish update_children)] 600075a

   // [(start update_children)] 600075b
   if( !is_create
    && !get_obj( ).get_is_for_peer( )
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Other_Modifier_2( ).iterate_forwards( ) )
   {
      do
      {
         if( !is_update_or_destroy_locked_by_own_session( get_obj( ).child_Specification_Other_Modifier_2( ) ) )
         {
            get_obj( ).child_Specification_Other_Modifier_2( ).op_update( );
            get_obj( ).child_Specification_Other_Modifier_2( ).op_apply( );
         }
      } while( get_obj( ).child_Specification_Other_Modifier_2( ).iterate_next( ) );
   }
   // [(finish update_children)] 600075b

   // [(start clone_children_from_fk)] 600079
   if( is_create && !is_null( get_obj( ).Source_Modifier( ) ) )
   {
      if( get_obj( ).Source_Modifier( ).child_Modifier_Affect( ).iterate_forwards( ) )
      {
         int child_num = 0;
         do
         {
            string key_info( construct_key_from_int( get_obj( ).get_key( ), ++child_num ) );
            key_info += ' ';
            key_info += get_obj( ).Source_Modifier( ).child_Modifier_Affect( ).get_key( );

            get_obj( ).child_Modifier_Affect( ).op_create( key_info );
            get_obj( ).child_Modifier_Affect( ).Modifier( get_obj( ).get_key( ) );
            get_obj( ).child_Modifier_Affect( ).Source_Modifier_Affect( get_obj( ).Source_Modifier( ).child_Modifier_Affect( ).get_key( ) );
            get_obj( ).child_Modifier_Affect( ).op_apply( );

         } while( get_obj( ).Source_Modifier( ).child_Modifier_Affect( ).iterate_next( ) );
      }
   }
   // [(finish clone_children_from_fk)] 600079

   // [(start clone_children_for_create)] 610079
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

         get_obj( ).Class( ).child_Class_Source( ).child_Modifier( ).op_create( key_info );
         get_obj( ).Class( ).child_Class_Source( ).child_Modifier( ).Class( get_obj( ).Class( ).child_Class_Source( ).get_key( ) );
         get_obj( ).Class( ).child_Class_Source( ).child_Modifier( ).Source_Modifier( get_obj( ).get_key( ) );

         get_obj( ).Class( ).child_Class_Source( ).child_Modifier( ).op_apply( );
      } while( get_obj( ).Class( ).child_Class_Source( ).iterate_next( ) );
   }
   // [(finish clone_children_for_create)] 610079

   // [(start clone_children_for_update)] 620079
   if( !is_create && get_obj( ).child_Modifier_Source( ).iterate_forwards( ) )
   {
      do
      {
         get_obj( ).child_Modifier_Source( ).op_update( );
         string parent( get_obj( ).child_Modifier_Source( ).Class( ) );

         get_obj( ).child_Modifier_Source( ).copy_all_field_values( get_obj( ) );

         get_obj( ).child_Modifier_Source( ).Class( parent );
         get_obj( ).child_Modifier_Source( ).Source_Modifier( get_obj( ).get_key( ) );

         get_obj( ).child_Modifier_Source( ).op_apply( );
      } while( get_obj( ).child_Modifier_Source( ).iterate_next( ) );
   }
   // [(finish clone_children_for_update)] 620079

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Modifier::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Modifier::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Modifier::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Modifier::impl::set_default_values( )
{
   clear( );
}

bool Meta_Modifier::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_Modifier::impl::get_required_transients( ) const
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

Meta_Modifier::Meta_Modifier( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Modifier::~Meta_Modifier( )
{
   cleanup( );
   delete p_impl;
}

bool Meta_Modifier::Internal( ) const
{
   return p_impl->impl_Internal( );
}

void Meta_Modifier::Internal( bool Internal )
{
   p_impl->impl_Internal( Internal );
}

const string& Meta_Modifier::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_Modifier::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

Meta_Class& Meta_Modifier::Class( )
{
   return p_impl->impl_Class( );
}

const Meta_Class& Meta_Modifier::Class( ) const
{
   return p_impl->impl_Class( );
}

void Meta_Modifier::Class( const string& key )
{
   p_impl->impl_Class( key );
}

Meta_Modifier& Meta_Modifier::Source_Modifier( )
{
   return p_impl->impl_Source_Modifier( );
}

const Meta_Modifier& Meta_Modifier::Source_Modifier( ) const
{
   return p_impl->impl_Source_Modifier( );
}

void Meta_Modifier::Source_Modifier( const string& key )
{
   p_impl->impl_Source_Modifier( key );
}

Meta_List_Field& Meta_Modifier::child_List_Field_Access_Parent( )
{
   return p_impl->impl_child_List_Field_Access_Parent( );
}

const Meta_List_Field& Meta_Modifier::child_List_Field_Access_Parent( ) const
{
   return p_impl->impl_child_List_Field_Access_Parent( );
}

Meta_List& Meta_Modifier::child_List_Access_Parent( )
{
   return p_impl->impl_child_List_Access_Parent( );
}

const Meta_List& Meta_Modifier::child_List_Access_Parent( ) const
{
   return p_impl->impl_child_List_Access_Parent( );
}

Meta_List& Meta_Modifier::child_List_Create_Parent( )
{
   return p_impl->impl_child_List_Create_Parent( );
}

const Meta_List& Meta_Modifier::child_List_Create_Parent( ) const
{
   return p_impl->impl_child_List_Create_Parent( );
}

Meta_List& Meta_Modifier::child_List_Destroy_Parent( )
{
   return p_impl->impl_child_List_Destroy_Parent( );
}

const Meta_List& Meta_Modifier::child_List_Destroy_Parent( ) const
{
   return p_impl->impl_child_List_Destroy_Parent( );
}

Meta_Specification& Meta_Modifier::child_Specification_Other_Modifier_2( )
{
   return p_impl->impl_child_Specification_Other_Modifier_2( );
}

const Meta_Specification& Meta_Modifier::child_Specification_Other_Modifier_2( ) const
{
   return p_impl->impl_child_Specification_Other_Modifier_2( );
}

Meta_Specification& Meta_Modifier::child_Specification_Other( )
{
   return p_impl->impl_child_Specification_Other( );
}

const Meta_Specification& Meta_Modifier::child_Specification_Other( ) const
{
   return p_impl->impl_child_Specification_Other( );
}

Meta_Modifier& Meta_Modifier::child_Modifier_Source( )
{
   return p_impl->impl_child_Modifier_Source( );
}

const Meta_Modifier& Meta_Modifier::child_Modifier_Source( ) const
{
   return p_impl->impl_child_Modifier_Source( );
}

Meta_Modifier_Affect& Meta_Modifier::child_Modifier_Affect( )
{
   return p_impl->impl_child_Modifier_Affect( );
}

const Meta_Modifier_Affect& Meta_Modifier::child_Modifier_Affect( ) const
{
   return p_impl->impl_child_Modifier_Affect( );
}

Meta_Package_Option& Meta_Modifier::child_Package_Option( )
{
   return p_impl->impl_child_Package_Option( );
}

const Meta_Package_Option& Meta_Modifier::child_Package_Option( ) const
{
   return p_impl->impl_child_Package_Option( );
}

Meta_Specification& Meta_Modifier::child_Specification( )
{
   return p_impl->impl_child_Specification( );
}

const Meta_Specification& Meta_Modifier::child_Specification( ) const
{
   return p_impl->impl_child_Specification( );
}

string Meta_Modifier::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Modifier::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_Modifier::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_Modifier::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_Modifier::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_Modifier::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_Modifier::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_Modifier::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_Modifier::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_Modifier::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Modifier::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_Modifier::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Modifier::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Modifier::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Modifier::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Modifier::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Modifier::clear( )
{
   p_impl->clear( );
}

void Meta_Modifier::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Modifier::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Modifier::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Modifier::finalise_fetch( bool skip_set_original )
{
   p_impl->finalise_fetch( skip_set_original );
}

void Meta_Modifier::at_create( )
{
   p_impl->at_create( );
}

void Meta_Modifier::post_init( )
{
   p_impl->post_init( );
}

void Meta_Modifier::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Modifier::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Modifier::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Modifier::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Modifier::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Modifier::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Modifier::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Modifier::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Modifier::get_field_id(
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
   else if( name == c_field_name_Source_Modifier )
   {
      p_id = c_field_id_Source_Modifier;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_id;
}

const char* Meta_Modifier::get_field_name(
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
   else if( id == c_field_id_Source_Modifier )
   {
      p_name = c_field_name_Source_Modifier;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_name;
}

string& Meta_Modifier::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_Modifier::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_Modifier::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_Modifier::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_Modifier::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_Modifier::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Modifier::get_field_uom_symbol( const string& id_or_name ) const
{
   string uom_symbol;

   string name;
   pair< string, string > next;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_uom_symbol" );
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
   {
      name = string( c_field_display_name_Class );
      get_module_string( c_field_display_name_Class, &next );
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
   if( ( id_or_name == c_field_id_Source_Modifier ) || ( id_or_name == c_field_name_Source_Modifier ) )
   {
      name = string( c_field_display_name_Source_Modifier );
      get_module_string( c_field_display_name_Source_Modifier, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_Modifier::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
      display_name = get_module_string( c_field_display_name_Class );
   if( ( id_or_name == c_field_id_Internal ) || ( id_or_name == c_field_name_Internal ) )
      display_name = get_module_string( c_field_display_name_Internal );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
      display_name = get_module_string( c_field_display_name_Name );
   if( ( id_or_name == c_field_id_Source_Modifier ) || ( id_or_name == c_field_name_Source_Modifier ) )
      display_name = get_module_string( c_field_display_name_Source_Modifier );

   return display_name;
}

void Meta_Modifier::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Modifier::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Modifier::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Modifier::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Modifier::setup_foreign_key( Meta_Class& o, const string& value )
{
   static_cast< Meta_Class& >( o ).set_key( value );
}

void Meta_Modifier::setup_foreign_key( Meta_Modifier& o, const string& value )
{
   static_cast< Meta_Modifier& >( o ).set_key( value );
}

void Meta_Modifier::setup_graph_parent( Meta_List_Field& o, const string& foreign_key_field )
{
   static_cast< Meta_List_Field& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Modifier::setup_graph_parent( Meta_List& o, const string& foreign_key_field )
{
   static_cast< Meta_List& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Modifier::setup_graph_parent( Meta_Specification& o, const string& foreign_key_field )
{
   static_cast< Meta_Specification& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Modifier::setup_graph_parent( Meta_Modifier& o, const string& foreign_key_field )
{
   static_cast< Meta_Modifier& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Modifier::setup_graph_parent( Meta_Modifier_Affect& o, const string& foreign_key_field )
{
   static_cast< Meta_Modifier_Affect& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Modifier::setup_graph_parent( Meta_Package_Option& o, const string& foreign_key_field )
{
   static_cast< Meta_Package_Option& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Modifier::setup_graph_parent(
 Meta_Class& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Class& >( o ).set_key( init_value );
}

void Meta_Modifier::setup_graph_parent(
 Meta_Modifier& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Modifier& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Modifier& >( o ).set_key( init_value );
}

size_t Meta_Modifier::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Modifier::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Modifier::get_num_foreign_key_children( bool is_internal ) const
{
   size_t rc = 10;

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

class_base* Meta_Modifier::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   if( child_num >= 10 )
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
            next_child_field = "302199";
            p_class_base = &child_List_Field_Access_Parent( );
         }
         break;

         case 1:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301998";
            p_class_base = &child_List_Access_Parent( );
         }
         break;

         case 2:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301998a";
            p_class_base = &child_List_Create_Parent( );
         }
         break;

         case 3:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301998b";
            p_class_base = &child_List_Destroy_Parent( );
         }
         break;

         case 4:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301442";
            p_class_base = &child_Specification_Other_Modifier_2( );
         }
         break;

         case 5:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301441";
            p_class_base = &child_Specification_Other( );
         }
         break;

         case 6:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "300910";
            p_class_base = &child_Modifier_Source( );
         }
         break;

         case 7:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301000";
            p_class_base = &child_Modifier_Affect( );
         }
         break;

         case 8:
         if( op == e_cascade_op_unlink )
         {
            next_child_field = "302832";
            p_class_base = &child_Package_Option( );
         }
         break;

         case 9:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301440";
            p_class_base = &child_Specification( );
         }
         break;
      }
   }

   return p_class_base;
}

void Meta_Modifier::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_Modifier::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_Modifier::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_Modifier::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_Modifier::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_Modifier::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_Modifier::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_Modifier::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "modifier";

   return get_module_string( key );
}

string Meta_Modifier::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_Modifier::get_create_instance_info( ) const
{
   return "";
}

string Meta_Modifier::get_update_instance_info( ) const
{
   return "";
}

string Meta_Modifier::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_Modifier::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );

   return retval;
}

bool Meta_Modifier::get_is_alias( ) const
{
   return false;
}

void Meta_Modifier::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Modifier::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Modifier::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == "_302199" ) || ( sub_context == "child_List_Field_Access_Parent" ) )
      p_class_base = &child_List_Field_Access_Parent( );
   else if( ( sub_context == "_301998" ) || ( sub_context == "child_List_Access_Parent" ) )
      p_class_base = &child_List_Access_Parent( );
   else if( ( sub_context == "_301998a" ) || ( sub_context == "child_List_Create_Parent" ) )
      p_class_base = &child_List_Create_Parent( );
   else if( ( sub_context == "_301998b" ) || ( sub_context == "child_List_Destroy_Parent" ) )
      p_class_base = &child_List_Destroy_Parent( );
   else if( ( sub_context == "_301442" ) || ( sub_context == "child_Specification_Other_Modifier_2" ) )
      p_class_base = &child_Specification_Other_Modifier_2( );
   else if( ( sub_context == "_301441" ) || ( sub_context == "child_Specification_Other" ) )
      p_class_base = &child_Specification_Other( );
   else if( ( sub_context == "_300910" ) || ( sub_context == "child_Modifier_Source" ) )
      p_class_base = &child_Modifier_Source( );
   else if( ( sub_context == "_301000" ) || ( sub_context == "child_Modifier_Affect" ) )
      p_class_base = &child_Modifier_Affect( );
   else if( ( sub_context == "_302832" ) || ( sub_context == "child_Package_Option" ) )
      p_class_base = &child_Package_Option( );
   else if( ( sub_context == "_301440" ) || ( sub_context == "child_Specification" ) )
      p_class_base = &child_Specification( );
   else if( ( sub_context == c_field_id_Class ) || ( sub_context == c_field_name_Class ) )
      p_class_base = &Class( );
   else if( ( sub_context == c_field_id_Source_Modifier ) || ( sub_context == c_field_name_Source_Modifier ) )
      p_class_base = &Source_Modifier( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Modifier::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Class" );
   names.push_back( "C_Internal" );
   names.push_back( "C_Name" );
   names.push_back( "C_Source_Modifier" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Modifier::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Class( ) ) ) );
   values.push_back( to_string( Internal( ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Modifier( ) ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Modifier::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Modifier::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start modifier_field_value)] 600076
   dependents.insert( "Internal" ); // (for Is_Internal modifier)

   if( ( use_transients && is_field_transient( e_field_id_Internal ) )
    || ( !use_transients && !is_field_transient( e_field_id_Internal ) ) )
      names.insert( "Internal" );
   // [(finish modifier_field_value)] 600076

   // [(start protect_equal)] 600077
   dependents.insert( "Internal" );

   if( ( use_transients && is_field_transient( e_field_id_Internal ) )
    || ( !use_transients && !is_field_transient( e_field_id_Internal ) ) )
      names.insert( "Internal" );
   // [(finish protect_equal)] 600077

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Modifier::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Modifier::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_Modifier::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Modifier::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Modifier::static_lock_class_id( )
{
   return "109100";
}

const char* Meta_Modifier::static_check_class_name( )
{
   return "Modifier";
}

const char* Meta_Modifier::static_persistence_extra( )
{
   return "";
}

bool Meta_Modifier::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Modifier::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.109100" );
}

void Meta_Modifier::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "300900", "Class", "Meta_Class", true, "", "" ) );
   all_field_info.push_back( field_info( "109102", "Internal", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "109101", "Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "300910", "Source_Modifier", "Meta_Modifier", false, "", "" ) );
}

void Meta_Modifier::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Class, make_pair( "Meta.109100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Modifier, make_pair( "Meta.109100", "Meta_Modifier" ) ) );
}

int Meta_Modifier::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Modifier::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_Modifier::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Modifier::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "300900";
      break;

      case 2:
      p_id = "109102";
      break;

      case 3:
      p_id = "109101";
      break;

      case 4:
      p_id = "300910";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Modifier" );

   return p_id;
}

const char* Meta_Modifier::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Class";
      break;

      case 2:
      p_id = "Internal";
      break;

      case 3:
      p_id = "Name";
      break;

      case 4:
      p_id = "Source_Modifier";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Modifier" );

   return p_id;
}

int Meta_Modifier::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Class || field == c_field_name_Class )
      rc += 1;
   else if( field == c_field_id_Internal || field == c_field_name_Internal )
      rc += 2;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 3;
   else if( field == c_field_id_Source_Modifier || field == c_field_name_Source_Modifier )
      rc += 4;

   return rc - 1;
}

procedure_info_container& Meta_Modifier::static_get_procedure_info( )
{
   static procedure_info_container procedures;

   return procedures;
}

string Meta_Modifier::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(75),"
    "C_Ver_ SMALLINT UNSIGNED NOT NULL,"
    "C_Rev_ BIGINT UNSIGNED NOT NULL,"
    "C_Sec_ BIGINT UNSIGNED NOT NULL,"
    "C_Typ_ VARCHAR(16) NOT NULL,"
    "C_Class VARCHAR(75) NOT NULL,"
    "C_Internal INTEGER NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Source_Modifier VARCHAR(75) NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Modifier::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Modifier::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   ( void )pairs;
}

void Meta_Modifier::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "Class,Name", "string,string" ) );
}

void Meta_Modifier::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "Class,Name" );
}

void Meta_Modifier::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Class,C_Name" );
}

void Meta_Modifier::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Class,C_Name" );
}

void Meta_Modifier::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Modifier::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Modifier::static_insert_external_alias( const string& module_and_class_id, Meta_Modifier* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Modifier::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Modifier::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Modifier::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
