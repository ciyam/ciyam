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

#include "Meta_Modifier_Affect.h"

#include "Meta_Class.h"
#include "Meta_Field.h"
#include "Meta_Modifier.h"
#include "Meta_Modifier_Affect.h"

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

// [(start clone_children_for_create)] 610084
#include "Meta_Modifier.h"
// [(finish clone_children_for_create)] 610084

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

template< > inline string to_string( const Meta_Modifier& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Modifier_Affect& c )
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

inline void from_string( Meta_Modifier& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Modifier_Affect& c, const string& s )
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

#include "Meta_Modifier_Affect.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Class = "301010";
const char* const c_field_id_Extra = "110103";
const char* const c_field_id_Field = "301020";
const char* const c_field_id_Internal = "110104";
const char* const c_field_id_Modifier = "301000";
const char* const c_field_id_Scope = "110101";
const char* const c_field_id_Source_Modifier_Affect = "301030";
const char* const c_field_id_Type = "110102";

const char* const c_field_name_Class = "Class";
const char* const c_field_name_Extra = "Extra";
const char* const c_field_name_Field = "Field";
const char* const c_field_name_Internal = "Internal";
const char* const c_field_name_Modifier = "Modifier";
const char* const c_field_name_Scope = "Scope";
const char* const c_field_name_Source_Modifier_Affect = "Source_Modifier_Affect";
const char* const c_field_name_Type = "Type";

const char* const c_field_display_name_Class = "field_modifier_affect_class";
const char* const c_field_display_name_Extra = "field_modifier_affect_extra";
const char* const c_field_display_name_Field = "field_modifier_affect_field";
const char* const c_field_display_name_Internal = "field_modifier_affect_internal";
const char* const c_field_display_name_Modifier = "field_modifier_affect_modifier";
const char* const c_field_display_name_Scope = "field_modifier_affect_scope";
const char* const c_field_display_name_Source_Modifier_Affect = "field_modifier_affect_source_modifier_affect";
const char* const c_field_display_name_Type = "field_modifier_affect_type";

const int c_num_fields = 8;

const char* const c_all_sorted_field_ids[ ] =
{
   "110101",
   "110102",
   "110103",
   "110104",
   "301000",
   "301010",
   "301020",
   "301030"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Class",
   "Extra",
   "Field",
   "Internal",
   "Modifier",
   "Scope",
   "Source_Modifier_Affect",
   "Type"
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

const uint64_t c_modifier_Is_Class = UINT64_C( 0x100 );
const uint64_t c_modifier_Is_Internal = UINT64_C( 0x200 );

string g_group_field_name;
string g_level_field_name;
string g_order_field_name;
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_Modifier_Affect* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Modifier_Affect* > external_aliases_lookup_container;
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
int g_default_Extra = int( 0 );
string g_default_Field = string( );
bool g_default_Internal = bool( 0 );
string g_default_Modifier = string( );
int g_default_Scope = int( 0 );
string g_default_Source_Modifier_Affect = string( );
int g_default_Type = int( 1 );

set< int > g_modifier_affect_extra_enum;
set< int > g_modifier_affect_scope_enum;
set< int > g_modifier_affect_type_enum;

const int c_enum_modifier_affect_extra_none( 0 );
const int c_enum_modifier_affect_extra_list_only( 1 );
const int c_enum_modifier_affect_extra_view_only( 2 );

string get_enum_string_modifier_affect_extra( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for modifier_affect_extra" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_modifier_affect_extra_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_modifier_affect_extra_list_only";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_modifier_affect_extra_view_only";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for modifier_affect_extra" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_modifier_affect_scope_field( 0 );
const int c_enum_modifier_affect_scope_class( 1 );

string get_enum_string_modifier_affect_scope( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for modifier_affect_scope" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_modifier_affect_scope_field";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_modifier_affect_scope_class";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for modifier_affect_scope" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_modifier_affect_type_protect( 0 );
const int c_enum_modifier_affect_type_relegate( 1 );
const int c_enum_modifier_affect_type_lowlight( 2 );
const int c_enum_modifier_affect_type_lowlight1( 3 );
const int c_enum_modifier_affect_type_highlight( 4 );
const int c_enum_modifier_affect_type_highlight1( 5 );
const int c_enum_modifier_affect_type_extralight( 6 );
const int c_enum_modifier_affect_type_extralight1( 7 );

string get_enum_string_modifier_affect_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for modifier_affect_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_modifier_affect_type_protect";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_modifier_affect_type_relegate";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_modifier_affect_type_lowlight";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_modifier_affect_type_lowlight1";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_modifier_affect_type_highlight";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_modifier_affect_type_highlight1";
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_modifier_affect_type_extralight";
   else if( to_string( val ) == to_string( "7" ) )
      string_name = "enum_modifier_affect_type_extralight1";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for modifier_affect_type" );

   return get_module_string( lower( string_name ) );
}

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Modifier_Affect > Modifier_Affect_registration( get_class_registry( ), "110100" );

class Meta_Modifier_Affect_command_functor;

class Meta_Modifier_Affect_command_handler : public command_handler
{
   friend class Meta_Modifier_Affect_command_functor;

   public:
   Meta_Modifier_Affect_command_handler( ) : p_Meta_Modifier_Affect( 0 ) { }

   void set_Meta_Modifier_Affect( Meta_Modifier_Affect* p_new_Meta_Modifier_Affect ) { p_Meta_Modifier_Affect = p_new_Meta_Modifier_Affect; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Modifier_Affect* p_Meta_Modifier_Affect;

   protected:
   string retval;
};

class Meta_Modifier_Affect_command_functor : public command_functor
{
   public:
   Meta_Modifier_Affect_command_functor( Meta_Modifier_Affect_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Modifier_Affect_command_handler& cmd_handler;
};

command_functor* Meta_Modifier_Affect_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Modifier_Affect_command_functor( dynamic_cast< Meta_Modifier_Affect_command_handler& >( handler ) );
}

void Meta_Modifier_Affect_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Modifier_Affect_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_Modifier_Affect_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Modifier_Affect->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Modifier_Affect->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Modifier_Affect_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Modifier_Affect->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Modifier_Affect->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Modifier_Affect_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Modifier_Affect_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Modifier_Affect->Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Extra ) || ( field_name == c_field_name_Extra ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Modifier_Affect->Extra( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Field ) || ( field_name == c_field_name_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Modifier_Affect->Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Internal ) || ( field_name == c_field_name_Internal ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Modifier_Affect->Internal( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Modifier ) || ( field_name == c_field_name_Modifier ) ) )
      {
         handled = true;

         string_getter< Meta_Modifier >( cmd_handler.p_Meta_Modifier_Affect->Modifier( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Scope ) || ( field_name == c_field_name_Scope ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Modifier_Affect->Scope( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Modifier_Affect ) || ( field_name == c_field_name_Source_Modifier_Affect ) ) )
      {
         handled = true;

         string_getter< Meta_Modifier_Affect >( cmd_handler.p_Meta_Modifier_Affect->Source_Modifier_Affect( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Modifier_Affect->Type( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Modifier_Affect_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Modifier_Affect_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_Modifier_Affect_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Modifier_Affect, Meta_Class >(
          *cmd_handler.p_Meta_Modifier_Affect, &Meta_Modifier_Affect::Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Extra ) || ( field_name == c_field_name_Extra ) ) )
      {
         handled = true;

         func_string_setter< Meta_Modifier_Affect, int >(
          *cmd_handler.p_Meta_Modifier_Affect, &Meta_Modifier_Affect::Extra, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Field ) || ( field_name == c_field_name_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Modifier_Affect, Meta_Field >(
          *cmd_handler.p_Meta_Modifier_Affect, &Meta_Modifier_Affect::Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Internal ) || ( field_name == c_field_name_Internal ) ) )
      {
         handled = true;

         func_string_setter< Meta_Modifier_Affect, bool >(
          *cmd_handler.p_Meta_Modifier_Affect, &Meta_Modifier_Affect::Internal, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Modifier ) || ( field_name == c_field_name_Modifier ) ) )
      {
         handled = true;

         func_string_setter< Meta_Modifier_Affect, Meta_Modifier >(
          *cmd_handler.p_Meta_Modifier_Affect, &Meta_Modifier_Affect::Modifier, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Scope ) || ( field_name == c_field_name_Scope ) ) )
      {
         handled = true;

         func_string_setter< Meta_Modifier_Affect, int >(
          *cmd_handler.p_Meta_Modifier_Affect, &Meta_Modifier_Affect::Scope, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Modifier_Affect ) || ( field_name == c_field_name_Source_Modifier_Affect ) ) )
      {
         handled = true;

         func_string_setter< Meta_Modifier_Affect, Meta_Modifier_Affect >(
          *cmd_handler.p_Meta_Modifier_Affect, &Meta_Modifier_Affect::Source_Modifier_Affect, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Modifier_Affect, int >(
          *cmd_handler.p_Meta_Modifier_Affect, &Meta_Modifier_Affect::Type, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Modifier_Affect_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Modifier_Affect_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_Modifier_Affect_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Modifier_Affect->Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Field ) || ( field_name == c_field_name_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_Modifier_Affect->Field( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Modifier ) || ( field_name == c_field_name_Modifier ) )
         cmd_handler.retval = cmd_handler.p_Meta_Modifier_Affect->Modifier( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Modifier_Affect ) || ( field_name == c_field_name_Source_Modifier_Affect ) )
         cmd_handler.retval = cmd_handler.p_Meta_Modifier_Affect->Source_Modifier_Affect( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
}

struct Meta_Modifier_Affect::impl : public Meta_Modifier_Affect_command_handler
{
   impl( Meta_Modifier_Affect& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Modifier_Affect( &o );

      add_commands( 0, Meta_Modifier_Affect_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Modifier_Affect_command_definitions ) );
   }

   Meta_Modifier_Affect& get_obj( ) const
   {
      return *cp_obj;
   }

   int impl_Extra( ) const { return lazy_fetch( p_obj ), v_Extra; }
   void impl_Extra( int Extra ) { v_Extra = Extra; }

   bool impl_Internal( ) const { return lazy_fetch( p_obj ), v_Internal; }
   void impl_Internal( bool Internal ) { v_Internal = Internal; }

   int impl_Scope( ) const { return lazy_fetch( p_obj ), v_Scope; }
   void impl_Scope( int Scope ) { v_Scope = Scope; }

   int impl_Type( ) const { return lazy_fetch( p_obj ), v_Type; }
   void impl_Type( int Type ) { v_Type = Type; }

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

   Meta_Modifier_Affect& impl_Source_Modifier_Affect( )
   {
      if( !cp_Source_Modifier_Affect )
      {
         cp_Source_Modifier_Affect.init( );

         p_obj->setup_graph_parent( *cp_Source_Modifier_Affect, c_field_id_Source_Modifier_Affect, v_Source_Modifier_Affect );
      }
      return *cp_Source_Modifier_Affect;
   }

   const Meta_Modifier_Affect& impl_Source_Modifier_Affect( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Modifier_Affect )
      {
         cp_Source_Modifier_Affect.init( );

         p_obj->setup_graph_parent( *cp_Source_Modifier_Affect, c_field_id_Source_Modifier_Affect, v_Source_Modifier_Affect );
      }
      return *cp_Source_Modifier_Affect;
   }

   void impl_Source_Modifier_Affect( const string& key )
   {
      class_base_accessor cba( impl_Source_Modifier_Affect( ) );
      cba.set_key( key );
   }

   Meta_Modifier_Affect& impl_child_Modifier_Affect_Source( )
   {
      if( !cp_child_Modifier_Affect_Source )
      {
         cp_child_Modifier_Affect_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Modifier_Affect_Source, "301030" );
      }
      return *cp_child_Modifier_Affect_Source;
   }

   const Meta_Modifier_Affect& impl_child_Modifier_Affect_Source( ) const
   {
      if( !cp_child_Modifier_Affect_Source )
      {
         cp_child_Modifier_Affect_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Modifier_Affect_Source, "301030" );
      }
      return *cp_child_Modifier_Affect_Source;
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

   Meta_Modifier_Affect* p_obj;
   class_pointer< Meta_Modifier_Affect > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   int v_Extra;
   bool v_Internal;
   int v_Scope;
   int v_Type;

   string v_Class;
   mutable class_pointer< Meta_Class > cp_Class;

   string v_Field;
   mutable class_pointer< Meta_Field > cp_Field;

   string v_Modifier;
   mutable class_pointer< Meta_Modifier > cp_Modifier;

   string v_Source_Modifier_Affect;
   mutable class_pointer< Meta_Modifier_Affect > cp_Source_Modifier_Affect;

   mutable class_pointer< Meta_Modifier_Affect > cp_child_Modifier_Affect_Source;
};

string Meta_Modifier_Affect::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Class( ) );
      break;

      case 1:
      retval = to_string( impl_Extra( ) );
      break;

      case 2:
      retval = to_string( impl_Field( ) );
      break;

      case 3:
      retval = to_string( impl_Internal( ) );
      break;

      case 4:
      retval = to_string( impl_Modifier( ) );
      break;

      case 5:
      retval = to_string( impl_Scope( ) );
      break;

      case 6:
      retval = to_string( impl_Source_Modifier_Affect( ) );
      break;

      case 7:
      retval = to_string( impl_Type( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_Modifier_Affect::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Modifier_Affect::impl, Meta_Class >( *this, &Meta_Modifier_Affect::impl::impl_Class, value );
      break;

      case 1:
      func_string_setter< Meta_Modifier_Affect::impl, int >( *this, &Meta_Modifier_Affect::impl::impl_Extra, value );
      break;

      case 2:
      func_string_setter< Meta_Modifier_Affect::impl, Meta_Field >( *this, &Meta_Modifier_Affect::impl::impl_Field, value );
      break;

      case 3:
      func_string_setter< Meta_Modifier_Affect::impl, bool >( *this, &Meta_Modifier_Affect::impl::impl_Internal, value );
      break;

      case 4:
      func_string_setter< Meta_Modifier_Affect::impl, Meta_Modifier >( *this, &Meta_Modifier_Affect::impl::impl_Modifier, value );
      break;

      case 5:
      func_string_setter< Meta_Modifier_Affect::impl, int >( *this, &Meta_Modifier_Affect::impl::impl_Scope, value );
      break;

      case 6:
      func_string_setter< Meta_Modifier_Affect::impl, Meta_Modifier_Affect >( *this, &Meta_Modifier_Affect::impl::impl_Source_Modifier_Affect, value );
      break;

      case 7:
      func_string_setter< Meta_Modifier_Affect::impl, int >( *this, &Meta_Modifier_Affect::impl::impl_Type, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_Modifier_Affect::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Class( g_default_Class );
      break;

      case 1:
      impl_Extra( g_default_Extra );
      break;

      case 2:
      impl_Field( g_default_Field );
      break;

      case 3:
      impl_Internal( g_default_Internal );
      break;

      case 4:
      impl_Modifier( g_default_Modifier );
      break;

      case 5:
      impl_Scope( g_default_Scope );
      break;

      case 6:
      impl_Source_Modifier_Affect( g_default_Source_Modifier_Affect );
      break;

      case 7:
      impl_Type( g_default_Type );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_Modifier_Affect::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Class == g_default_Class );
      break;

      case 1:
      retval = ( v_Extra == g_default_Extra );
      break;

      case 2:
      retval = ( v_Field == g_default_Field );
      break;

      case 3:
      retval = ( v_Internal == g_default_Internal );
      break;

      case 4:
      retval = ( v_Modifier == g_default_Modifier );
      break;

      case 5:
      retval = ( v_Scope == g_default_Scope );
      break;

      case 6:
      retval = ( v_Source_Modifier_Affect == g_default_Source_Modifier_Affect );
      break;

      case 7:
      retval = ( v_Type == g_default_Type );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_Modifier_Affect::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start modifier_field_value)] 600081
   if( get_obj( ).Scope( ) == 1 ) // i.e. class
      state |= c_modifier_Is_Class;
   // [(finish modifier_field_value)] 600081

   // [(start modifier_field_value)] 600082
   if( get_obj( ).Internal( ) == true )
      state |= c_modifier_Is_Internal;
   // [(finish modifier_field_value)] 600082

   // [(start protect_equal)] 600083
   if( check_equal( get_obj( ).Internal( ), true ) )
      state |= ( c_state_uneditable | c_state_undeletable );
   // [(finish protect_equal)] 600083

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

string Meta_Modifier_Affect::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   if( state & c_modifier_Is_Class )
      state_names += "|" + string( "Is_Class" );
   if( state & c_modifier_Is_Internal )
      state_names += "|" + string( "Is_Internal" );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_Modifier_Affect::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Modifier_Affect::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      impl_Class( "" );
   else if( ( field == c_field_id_Field ) || ( field == c_field_name_Field ) )
      impl_Field( "" );
   else if( ( field == c_field_id_Modifier ) || ( field == c_field_name_Modifier ) )
      impl_Modifier( "" );
   else if( ( field == c_field_id_Source_Modifier_Affect ) || ( field == c_field_name_Source_Modifier_Affect ) )
      impl_Source_Modifier_Affect( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Modifier_Affect::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      v_Class = value;
   else if( ( field == c_field_id_Field ) || ( field == c_field_name_Field ) )
      v_Field = value;
   else if( ( field == c_field_id_Modifier ) || ( field == c_field_name_Modifier ) )
      v_Modifier = value;
   else if( ( field == c_field_id_Source_Modifier_Affect ) || ( field == c_field_name_Source_Modifier_Affect ) )
      v_Source_Modifier_Affect = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Modifier_Affect::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      return v_Class;
   else if( ( field == c_field_id_Field ) || ( field == c_field_name_Field ) )
      return v_Field;
   else if( ( field == c_field_id_Modifier ) || ( field == c_field_name_Modifier ) )
      return v_Modifier;
   else if( ( field == c_field_id_Source_Modifier_Affect ) || ( field == c_field_name_Source_Modifier_Affect ) )
      return v_Source_Modifier_Affect;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Modifier_Affect::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Class, v_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Field, v_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Modifier, v_Modifier ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Modifier_Affect, v_Source_Modifier_Affect ) );
}

void Meta_Modifier_Affect::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_Modifier_Affect::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Modifier_Affect::impl::clear( )
{
   v_Extra = g_default_Extra;
   v_Internal = g_default_Internal;
   v_Scope = g_default_Scope;
   v_Type = g_default_Type;

   v_Class = string( );
   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   v_Field = string( );
   if( cp_Field )
      p_obj->setup_foreign_key( *cp_Field, v_Field );

   v_Modifier = string( );
   if( cp_Modifier )
      p_obj->setup_foreign_key( *cp_Modifier, v_Modifier );

   v_Source_Modifier_Affect = string( );
   if( cp_Source_Modifier_Affect )
      p_obj->setup_foreign_key( *cp_Source_Modifier_Affect, v_Source_Modifier_Affect );
}

bool Meta_Modifier_Affect::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Modifier_Affect::impl::validate(
 uint64_t state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( v_Class.empty( ) && !value_will_be_provided( c_field_name_Class ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Class ) ) ) ) );

   if( v_Modifier.empty( ) && !value_will_be_provided( c_field_name_Modifier ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Modifier,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Modifier ) ) ) ) );

   if( !g_modifier_affect_extra_enum.count( v_Extra ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Extra,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Extra ) ) ) ) );

   if( !g_modifier_affect_scope_enum.count( v_Scope ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Scope,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Scope ) ) ) ) );

   if( !g_modifier_affect_type_enum.count( v_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Type ) ) ) ) );

   // [(start check_cond_non_null)] 600081a
   if( !get_obj( ).Scope( ) && is_null( get_obj( ).Field( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Field, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Field ) ) ) ) );
   // [(finish check_cond_non_null)] 600081a

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Modifier_Affect::impl::validate_set_fields(
 set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;
}

void Meta_Modifier_Affect::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   if( cp_Field )
      p_obj->setup_foreign_key( *cp_Field, v_Field );

   if( cp_Modifier )
      p_obj->setup_foreign_key( *cp_Modifier, v_Modifier );

   if( cp_Source_Modifier_Affect )
      p_obj->setup_foreign_key( *cp_Source_Modifier_Affect, v_Source_Modifier_Affect );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Modifier_Affect::impl::finalise_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Modifier_Affect::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Modifier_Affect::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_Modifier_Affect::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start default_to_field)] 600080
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Class( ) == g_default_Class ) )
      get_obj( ).Class( get_obj( ).Modifier( ).Class( ) );
   // [(finish default_to_field)] 600080

   // [(start field_empty_action)] 600084
   if( !get_obj( ).get_key( ).empty( ) )
   {
      if( !is_null( get_obj( ).Source_Modifier_Affect( ) ) )
         get_obj( ).Internal( true );
   }
   // [(finish field_empty_action)] 600084

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Modifier_Affect::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Modifier_Affect::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start clone_children_for_create)] 610084
   if( is_create
    && get_obj( ).get_clone_key( ).empty( )
    && get_obj( ).Modifier( ).child_Modifier_Source( ).iterate_forwards( ) )
   {
      int child_num = 0;
      do
      {
         string key_info( construct_key_from_int( get_obj( ).get_key( ), ++child_num ) );
         key_info += ' ';
         key_info += get_obj( ).get_key( );

         get_obj( ).Modifier( ).child_Modifier_Source( ).child_Modifier_Affect( ).op_create( key_info );
         get_obj( ).Modifier( ).child_Modifier_Source( ).child_Modifier_Affect( ).Modifier( get_obj( ).Modifier( ).child_Modifier_Source( ).get_key( ) );
         get_obj( ).Modifier( ).child_Modifier_Source( ).child_Modifier_Affect( ).Source_Modifier_Affect( get_obj( ).get_key( ) );

         get_obj( ).Modifier( ).child_Modifier_Source( ).child_Modifier_Affect( ).op_apply( );
      } while( get_obj( ).Modifier( ).child_Modifier_Source( ).iterate_next( ) );
   }
   // [(finish clone_children_for_create)] 610084

   // [(start clone_children_for_update)] 620084
   if( !is_create && get_obj( ).child_Modifier_Affect_Source( ).iterate_forwards( ) )
   {
      do
      {
         get_obj( ).child_Modifier_Affect_Source( ).op_update( );
         string parent( get_obj( ).child_Modifier_Affect_Source( ).Modifier( ) );

         get_obj( ).child_Modifier_Affect_Source( ).copy_all_field_values( get_obj( ) );

         get_obj( ).child_Modifier_Affect_Source( ).Modifier( parent );
         get_obj( ).child_Modifier_Affect_Source( ).Source_Modifier_Affect( get_obj( ).get_key( ) );

         get_obj( ).child_Modifier_Affect_Source( ).op_apply( );
      } while( get_obj( ).child_Modifier_Affect_Source( ).iterate_next( ) );
   }
   // [(finish clone_children_for_update)] 620084

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Modifier_Affect::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Modifier_Affect::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Modifier_Affect::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Modifier_Affect::impl::set_default_values( )
{
   clear( );
}

bool Meta_Modifier_Affect::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_Modifier_Affect::impl::get_required_transients( ) const
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

Meta_Modifier_Affect::Meta_Modifier_Affect( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Modifier_Affect::~Meta_Modifier_Affect( )
{
   cleanup( );
   delete p_impl;
}

int Meta_Modifier_Affect::Extra( ) const
{
   return p_impl->impl_Extra( );
}

void Meta_Modifier_Affect::Extra( int Extra )
{
   p_impl->impl_Extra( Extra );
}

bool Meta_Modifier_Affect::Internal( ) const
{
   return p_impl->impl_Internal( );
}

void Meta_Modifier_Affect::Internal( bool Internal )
{
   p_impl->impl_Internal( Internal );
}

int Meta_Modifier_Affect::Scope( ) const
{
   return p_impl->impl_Scope( );
}

void Meta_Modifier_Affect::Scope( int Scope )
{
   p_impl->impl_Scope( Scope );
}

int Meta_Modifier_Affect::Type( ) const
{
   return p_impl->impl_Type( );
}

void Meta_Modifier_Affect::Type( int Type )
{
   p_impl->impl_Type( Type );
}

Meta_Class& Meta_Modifier_Affect::Class( )
{
   return p_impl->impl_Class( );
}

const Meta_Class& Meta_Modifier_Affect::Class( ) const
{
   return p_impl->impl_Class( );
}

void Meta_Modifier_Affect::Class( const string& key )
{
   p_impl->impl_Class( key );
}

Meta_Field& Meta_Modifier_Affect::Field( )
{
   return p_impl->impl_Field( );
}

const Meta_Field& Meta_Modifier_Affect::Field( ) const
{
   return p_impl->impl_Field( );
}

void Meta_Modifier_Affect::Field( const string& key )
{
   p_impl->impl_Field( key );
}

Meta_Modifier& Meta_Modifier_Affect::Modifier( )
{
   return p_impl->impl_Modifier( );
}

const Meta_Modifier& Meta_Modifier_Affect::Modifier( ) const
{
   return p_impl->impl_Modifier( );
}

void Meta_Modifier_Affect::Modifier( const string& key )
{
   p_impl->impl_Modifier( key );
}

Meta_Modifier_Affect& Meta_Modifier_Affect::Source_Modifier_Affect( )
{
   return p_impl->impl_Source_Modifier_Affect( );
}

const Meta_Modifier_Affect& Meta_Modifier_Affect::Source_Modifier_Affect( ) const
{
   return p_impl->impl_Source_Modifier_Affect( );
}

void Meta_Modifier_Affect::Source_Modifier_Affect( const string& key )
{
   p_impl->impl_Source_Modifier_Affect( key );
}

Meta_Modifier_Affect& Meta_Modifier_Affect::child_Modifier_Affect_Source( )
{
   return p_impl->impl_child_Modifier_Affect_Source( );
}

const Meta_Modifier_Affect& Meta_Modifier_Affect::child_Modifier_Affect_Source( ) const
{
   return p_impl->impl_child_Modifier_Affect_Source( );
}

string Meta_Modifier_Affect::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Modifier_Affect::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_Modifier_Affect::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_Modifier_Affect::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_Modifier_Affect::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_Modifier_Affect::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_Modifier_Affect::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_Modifier_Affect::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_Modifier_Affect::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_Modifier_Affect::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Modifier_Affect::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_Modifier_Affect::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Modifier_Affect::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Modifier_Affect::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Modifier_Affect::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Modifier_Affect::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Modifier_Affect::clear( )
{
   p_impl->clear( );
}

void Meta_Modifier_Affect::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Modifier_Affect::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Modifier_Affect::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Modifier_Affect::finalise_fetch( bool skip_set_original )
{
   p_impl->finalise_fetch( skip_set_original );
}

void Meta_Modifier_Affect::at_create( )
{
   p_impl->at_create( );
}

void Meta_Modifier_Affect::post_init( )
{
   p_impl->post_init( );
}

void Meta_Modifier_Affect::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Modifier_Affect::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Modifier_Affect::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Modifier_Affect::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Modifier_Affect::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Modifier_Affect::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Modifier_Affect::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Modifier_Affect::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Modifier_Affect::get_field_id(
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
   else if( name == c_field_name_Extra )
   {
      p_id = c_field_id_Extra;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Field )
   {
      p_id = c_field_id_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

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
   else if( name == c_field_name_Modifier )
   {
      p_id = c_field_id_Modifier;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Scope )
   {
      p_id = c_field_id_Scope;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Source_Modifier_Affect )
   {
      p_id = c_field_id_Source_Modifier_Affect;

      if( p_type_name )
         *p_type_name = "Meta_Modifier_Affect";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Type )
   {
      p_id = c_field_id_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_id;
}

const char* Meta_Modifier_Affect::get_field_name(
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
   else if( id == c_field_id_Extra )
   {
      p_name = c_field_name_Extra;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Field )
   {
      p_name = c_field_name_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

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
   else if( id == c_field_id_Modifier )
   {
      p_name = c_field_name_Modifier;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Scope )
   {
      p_name = c_field_name_Scope;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Source_Modifier_Affect )
   {
      p_name = c_field_name_Source_Modifier_Affect;

      if( p_type_name )
         *p_type_name = "Meta_Modifier_Affect";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Type )
   {
      p_name = c_field_name_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_name;
}

string& Meta_Modifier_Affect::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_Modifier_Affect::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_Modifier_Affect::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_Modifier_Affect::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_Modifier_Affect::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_Modifier_Affect::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Modifier_Affect::get_field_uom_symbol( const string& id_or_name ) const
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
   if( ( id_or_name == c_field_id_Extra ) || ( id_or_name == c_field_name_Extra ) )
   {
      name = string( c_field_display_name_Extra );
      get_module_string( c_field_display_name_Extra, &next );
   }
   if( ( id_or_name == c_field_id_Field ) || ( id_or_name == c_field_name_Field ) )
   {
      name = string( c_field_display_name_Field );
      get_module_string( c_field_display_name_Field, &next );
   }
   if( ( id_or_name == c_field_id_Internal ) || ( id_or_name == c_field_name_Internal ) )
   {
      name = string( c_field_display_name_Internal );
      get_module_string( c_field_display_name_Internal, &next );
   }
   if( ( id_or_name == c_field_id_Modifier ) || ( id_or_name == c_field_name_Modifier ) )
   {
      name = string( c_field_display_name_Modifier );
      get_module_string( c_field_display_name_Modifier, &next );
   }
   if( ( id_or_name == c_field_id_Scope ) || ( id_or_name == c_field_name_Scope ) )
   {
      name = string( c_field_display_name_Scope );
      get_module_string( c_field_display_name_Scope, &next );
   }
   if( ( id_or_name == c_field_id_Source_Modifier_Affect ) || ( id_or_name == c_field_name_Source_Modifier_Affect ) )
   {
      name = string( c_field_display_name_Source_Modifier_Affect );
      get_module_string( c_field_display_name_Source_Modifier_Affect, &next );
   }
   if( ( id_or_name == c_field_id_Type ) || ( id_or_name == c_field_name_Type ) )
   {
      name = string( c_field_display_name_Type );
      get_module_string( c_field_display_name_Type, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_Modifier_Affect::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
      display_name = get_module_string( c_field_display_name_Class );
   if( ( id_or_name == c_field_id_Extra ) || ( id_or_name == c_field_name_Extra ) )
      display_name = get_module_string( c_field_display_name_Extra );
   if( ( id_or_name == c_field_id_Field ) || ( id_or_name == c_field_name_Field ) )
      display_name = get_module_string( c_field_display_name_Field );
   if( ( id_or_name == c_field_id_Internal ) || ( id_or_name == c_field_name_Internal ) )
      display_name = get_module_string( c_field_display_name_Internal );
   if( ( id_or_name == c_field_id_Modifier ) || ( id_or_name == c_field_name_Modifier ) )
      display_name = get_module_string( c_field_display_name_Modifier );
   if( ( id_or_name == c_field_id_Scope ) || ( id_or_name == c_field_name_Scope ) )
      display_name = get_module_string( c_field_display_name_Scope );
   if( ( id_or_name == c_field_id_Source_Modifier_Affect ) || ( id_or_name == c_field_name_Source_Modifier_Affect ) )
      display_name = get_module_string( c_field_display_name_Source_Modifier_Affect );
   if( ( id_or_name == c_field_id_Type ) || ( id_or_name == c_field_name_Type ) )
      display_name = get_module_string( c_field_display_name_Type );

   return display_name;
}

void Meta_Modifier_Affect::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Modifier_Affect::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Modifier_Affect::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Modifier_Affect::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Modifier_Affect::setup_foreign_key( Meta_Class& o, const string& value )
{
   static_cast< Meta_Class& >( o ).set_key( value );
}

void Meta_Modifier_Affect::setup_foreign_key( Meta_Field& o, const string& value )
{
   static_cast< Meta_Field& >( o ).set_key( value );
}

void Meta_Modifier_Affect::setup_foreign_key( Meta_Modifier& o, const string& value )
{
   static_cast< Meta_Modifier& >( o ).set_key( value );
}

void Meta_Modifier_Affect::setup_foreign_key( Meta_Modifier_Affect& o, const string& value )
{
   static_cast< Meta_Modifier_Affect& >( o ).set_key( value );
}

void Meta_Modifier_Affect::setup_graph_parent( Meta_Modifier_Affect& o, const string& foreign_key_field )
{
   static_cast< Meta_Modifier_Affect& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Modifier_Affect::setup_graph_parent(
 Meta_Class& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Class& >( o ).set_key( init_value );
}

void Meta_Modifier_Affect::setup_graph_parent(
 Meta_Field& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Field& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Field& >( o ).set_key( init_value );
}

void Meta_Modifier_Affect::setup_graph_parent(
 Meta_Modifier& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Modifier& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Modifier& >( o ).set_key( init_value );
}

void Meta_Modifier_Affect::setup_graph_parent(
 Meta_Modifier_Affect& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Modifier_Affect& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Modifier_Affect& >( o ).set_key( init_value );
}

size_t Meta_Modifier_Affect::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Modifier_Affect::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Modifier_Affect::get_num_foreign_key_children( bool is_internal ) const
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

class_base* Meta_Modifier_Affect::get_next_foreign_key_child(
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
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301030";
            p_class_base = &child_Modifier_Affect_Source( );
         }
         break;
      }
   }

   return p_class_base;
}

void Meta_Modifier_Affect::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_Modifier_Affect::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_Modifier_Affect::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_Modifier_Affect::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_Modifier_Affect::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_Modifier_Affect::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_Modifier_Affect::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_Modifier_Affect::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "modifier_affect";

   return get_module_string( key );
}

string Meta_Modifier_Affect::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_Modifier_Affect::get_create_instance_info( ) const
{
   return "";
}

string Meta_Modifier_Affect::get_update_instance_info( ) const
{
   return "";
}

string Meta_Modifier_Affect::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_Modifier_Affect::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );

   return retval;
}

bool Meta_Modifier_Affect::get_is_alias( ) const
{
   return false;
}

void Meta_Modifier_Affect::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Modifier_Affect::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Modifier_Affect::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == "_301030" ) || ( sub_context == "child_Modifier_Affect_Source" ) )
      p_class_base = &child_Modifier_Affect_Source( );
   else if( ( sub_context == c_field_id_Class ) || ( sub_context == c_field_name_Class ) )
      p_class_base = &Class( );
   else if( ( sub_context == c_field_id_Field ) || ( sub_context == c_field_name_Field ) )
      p_class_base = &Field( );
   else if( ( sub_context == c_field_id_Modifier ) || ( sub_context == c_field_name_Modifier ) )
      p_class_base = &Modifier( );
   else if( ( sub_context == c_field_id_Source_Modifier_Affect ) || ( sub_context == c_field_name_Source_Modifier_Affect ) )
      p_class_base = &Source_Modifier_Affect( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Modifier_Affect::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Class" );
   names.push_back( "C_Extra" );
   names.push_back( "C_Field" );
   names.push_back( "C_Internal" );
   names.push_back( "C_Modifier" );
   names.push_back( "C_Scope" );
   names.push_back( "C_Source_Modifier_Affect" );
   names.push_back( "C_Type" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Modifier_Affect::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Class( ) ) ) );
   values.push_back( to_string( Extra( ) ) );
   values.push_back( sql_quote( to_string( Field( ) ) ) );
   values.push_back( to_string( Internal( ) ) );
   values.push_back( sql_quote( to_string( Modifier( ) ) ) );
   values.push_back( to_string( Scope( ) ) );
   values.push_back( sql_quote( to_string( Source_Modifier_Affect( ) ) ) );
   values.push_back( to_string( Type( ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Modifier_Affect::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Modifier_Affect::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start modifier_field_value)] 600081
   dependents.insert( "Scope" ); // (for Is_Class modifier)

   if( ( use_transients && is_field_transient( e_field_id_Scope ) )
    || ( !use_transients && !is_field_transient( e_field_id_Scope ) ) )
      names.insert( "Scope" );
   // [(finish modifier_field_value)] 600081

   // [(start modifier_field_value)] 600082
   dependents.insert( "Internal" ); // (for Is_Internal modifier)

   if( ( use_transients && is_field_transient( e_field_id_Internal ) )
    || ( !use_transients && !is_field_transient( e_field_id_Internal ) ) )
      names.insert( "Internal" );
   // [(finish modifier_field_value)] 600082

   // [(start protect_equal)] 600083
   dependents.insert( "Internal" );

   if( ( use_transients && is_field_transient( e_field_id_Internal ) )
    || ( !use_transients && !is_field_transient( e_field_id_Internal ) ) )
      names.insert( "Internal" );
   // [(finish protect_equal)] 600083

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Modifier_Affect::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Modifier_Affect::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_Modifier_Affect::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Modifier_Affect::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Modifier_Affect::static_lock_class_id( )
{
   return "110100";
}

const char* Meta_Modifier_Affect::static_check_class_name( )
{
   return "Modifier_Affect";
}

const char* Meta_Modifier_Affect::static_persistence_extra( )
{
   return "";
}

bool Meta_Modifier_Affect::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Modifier_Affect::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.110100" );
}

void Meta_Modifier_Affect::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "301010", "Class", "Meta_Class", true, "", "" ) );
   all_field_info.push_back( field_info( "110103", "Extra", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "301020", "Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "110104", "Internal", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "301000", "Modifier", "Meta_Modifier", true, "", "" ) );
   all_field_info.push_back( field_info( "110101", "Scope", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "301030", "Source_Modifier_Affect", "Meta_Modifier_Affect", false, "", "" ) );
   all_field_info.push_back( field_info( "110102", "Type", "int", false, "", "" ) );
}

void Meta_Modifier_Affect::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Class, make_pair( "Meta.110100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Field, make_pair( "Meta.110100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Modifier, make_pair( "Meta.110100", "Meta_Modifier" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Modifier_Affect, make_pair( "Meta.110100", "Meta_Modifier_Affect" ) ) );
}

int Meta_Modifier_Affect::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Modifier_Affect::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_Modifier_Affect::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Modifier_Affect::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "301010";
      break;

      case 2:
      p_id = "110103";
      break;

      case 3:
      p_id = "301020";
      break;

      case 4:
      p_id = "110104";
      break;

      case 5:
      p_id = "301000";
      break;

      case 6:
      p_id = "110101";
      break;

      case 7:
      p_id = "301030";
      break;

      case 8:
      p_id = "110102";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Modifier_Affect" );

   return p_id;
}

const char* Meta_Modifier_Affect::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Class";
      break;

      case 2:
      p_id = "Extra";
      break;

      case 3:
      p_id = "Field";
      break;

      case 4:
      p_id = "Internal";
      break;

      case 5:
      p_id = "Modifier";
      break;

      case 6:
      p_id = "Scope";
      break;

      case 7:
      p_id = "Source_Modifier_Affect";
      break;

      case 8:
      p_id = "Type";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Modifier_Affect" );

   return p_id;
}

int Meta_Modifier_Affect::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Class || field == c_field_name_Class )
      rc += 1;
   else if( field == c_field_id_Extra || field == c_field_name_Extra )
      rc += 2;
   else if( field == c_field_id_Field || field == c_field_name_Field )
      rc += 3;
   else if( field == c_field_id_Internal || field == c_field_name_Internal )
      rc += 4;
   else if( field == c_field_id_Modifier || field == c_field_name_Modifier )
      rc += 5;
   else if( field == c_field_id_Scope || field == c_field_name_Scope )
      rc += 6;
   else if( field == c_field_id_Source_Modifier_Affect || field == c_field_name_Source_Modifier_Affect )
      rc += 7;
   else if( field == c_field_id_Type || field == c_field_name_Type )
      rc += 8;

   return rc - 1;
}

procedure_info_container& Meta_Modifier_Affect::static_get_procedure_info( )
{
   static procedure_info_container procedures;

   return procedures;
}

string Meta_Modifier_Affect::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(75),"
    "C_Ver_ SMALLINT UNSIGNED NOT NULL,"
    "C_Rev_ BIGINT UNSIGNED NOT NULL,"
    "C_Sec_ BIGINT UNSIGNED NOT NULL,"
    "C_Typ_ VARCHAR(16) NOT NULL,"
    "C_Class VARCHAR(75) NOT NULL,"
    "C_Extra INTEGER NOT NULL,"
    "C_Field VARCHAR(75) NOT NULL,"
    "C_Internal INTEGER NOT NULL,"
    "C_Modifier VARCHAR(75) NOT NULL,"
    "C_Scope INTEGER NOT NULL,"
    "C_Source_Modifier_Affect VARCHAR(75) NOT NULL,"
    "C_Type INTEGER NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Modifier_Affect::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Modifier_Affect::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "enum_modifier_affect_extra_0", get_enum_string_modifier_affect_extra( 0 ) ) );
   pairs.push_back( make_pair( "enum_modifier_affect_extra_1", get_enum_string_modifier_affect_extra( 1 ) ) );
   pairs.push_back( make_pair( "enum_modifier_affect_extra_2", get_enum_string_modifier_affect_extra( 2 ) ) );

   pairs.push_back( make_pair( "enum_modifier_affect_scope_0", get_enum_string_modifier_affect_scope( 0 ) ) );
   pairs.push_back( make_pair( "enum_modifier_affect_scope_1", get_enum_string_modifier_affect_scope( 1 ) ) );

   pairs.push_back( make_pair( "enum_modifier_affect_type_0", get_enum_string_modifier_affect_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_modifier_affect_type_1", get_enum_string_modifier_affect_type( 1 ) ) );
   pairs.push_back( make_pair( "enum_modifier_affect_type_2", get_enum_string_modifier_affect_type( 2 ) ) );
   pairs.push_back( make_pair( "enum_modifier_affect_type_3", get_enum_string_modifier_affect_type( 3 ) ) );
   pairs.push_back( make_pair( "enum_modifier_affect_type_4", get_enum_string_modifier_affect_type( 4 ) ) );
   pairs.push_back( make_pair( "enum_modifier_affect_type_5", get_enum_string_modifier_affect_type( 5 ) ) );
   pairs.push_back( make_pair( "enum_modifier_affect_type_6", get_enum_string_modifier_affect_type( 6 ) ) );
   pairs.push_back( make_pair( "enum_modifier_affect_type_7", get_enum_string_modifier_affect_type( 7 ) ) );
}

void Meta_Modifier_Affect::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   ( void )pairs;
}

void Meta_Modifier_Affect::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   ( void )unique_indexes;
}

void Meta_Modifier_Affect::static_get_sql_indexes( vector< string >& indexes )
{
   ( void )indexes;
}

void Meta_Modifier_Affect::static_get_sql_unique_indexes( vector< string >& indexes )
{
   ( void )indexes;
}

void Meta_Modifier_Affect::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Modifier_Affect::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Modifier_Affect::static_insert_external_alias( const string& module_and_class_id, Meta_Modifier_Affect* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Modifier_Affect::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Modifier_Affect::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   g_modifier_affect_extra_enum.insert( 0 );
   g_modifier_affect_extra_enum.insert( 1 );
   g_modifier_affect_extra_enum.insert( 2 );

   g_modifier_affect_scope_enum.insert( 0 );
   g_modifier_affect_scope_enum.insert( 1 );

   g_modifier_affect_type_enum.insert( 0 );
   g_modifier_affect_type_enum.insert( 1 );
   g_modifier_affect_type_enum.insert( 2 );
   g_modifier_affect_type_enum.insert( 3 );
   g_modifier_affect_type_enum.insert( 4 );
   g_modifier_affect_type_enum.insert( 5 );
   g_modifier_affect_type_enum.insert( 6 );
   g_modifier_affect_type_enum.insert( 7 );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Modifier_Affect::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
