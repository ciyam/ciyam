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

#define CAT_BASE_LIB
#define MODULE_META_IMPL

// [<start macros>]
// [<finish macros>]

#include "Meta_Enum.h"

#include "Meta_Enum_Item.h"
#include "Meta_Field.h"
#include "Meta_Specification.h"
#include "Meta_Workgroup.h"

#include "cat_base.h"
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

#include "Meta_Enum.cmh"

const int32_t c_version = 1;

const char* const c_okay = "okay";

const char* const c_field_id_Id = "103104";
const char* const c_field_id_Internal = "103103";
const char* const c_field_id_Name = "103101";
const char* const c_field_id_Primitive = "103102";
const char* const c_field_id_Workgroup = "300300";

const char* const c_field_name_Id = "Id";
const char* const c_field_name_Internal = "Internal";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Primitive = "Primitive";
const char* const c_field_name_Workgroup = "Workgroup";

const char* const c_field_display_name_Id = "field_enum_id";
const char* const c_field_display_name_Internal = "field_enum_internal";
const char* const c_field_display_name_Name = "field_enum_name";
const char* const c_field_display_name_Primitive = "field_enum_primitive";
const char* const c_field_display_name_Workgroup = "field_enum_workgroup";

const int c_num_fields = 5;

const char* const c_all_sorted_field_ids[ ] =
{
   "103101",
   "103102",
   "103103",
   "103104",
   "300300"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Id",
   "Internal",
   "Name",
   "Primitive",
   "Workgroup"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool has_field( const string& field )
{
   return binary_search( c_all_sorted_field_ids, c_all_sorted_field_ids + c_num_fields, field.c_str( ), compare )
    || binary_search( c_all_sorted_field_names, c_all_sorted_field_names + c_num_fields, field.c_str( ), compare );
}

const int c_num_transient_fields = 0;

bool is_transient_field( const string& ) { static bool false_value( false ); return false_value; }

const uint64_t c_modifier_Is_Internal = UINT64_C( 0x100 );

aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Id_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Name_domain;

set< string > g_derivations;

typedef map< string, Meta_Enum* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Enum* > external_aliases_lookup_container;
typedef external_aliases_lookup_container::const_iterator external_aliases_lookup_const_iterator;

external_aliases_container g_external_aliases;
external_aliases_lookup_container g_external_aliases_lookup;

string gv_default_Id = string( );
bool gv_default_Internal = bool( 0 );
string gv_default_Name = string( );
int gv_default_Primitive = int( 5 );

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

registration< Meta_Enum > Enum_registration( get_class_registry( ), "103100" );

class Meta_Enum_command_functor;

class Meta_Enum_command_handler : public command_handler
{
   friend class Meta_Enum_command_functor;

   public:
   Meta_Enum_command_handler( ) : p_Meta_Enum( 0 ) { }

   void set_Meta_Enum( Meta_Enum* p_new_Meta_Enum ) { p_Meta_Enum = p_new_Meta_Enum; }

   void handle_unknown_command( const string& command )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Enum* p_Meta_Enum;

   protected:
   string retval;
};

class Meta_Enum_command_functor : public command_functor
{
   public:
   Meta_Enum_command_functor( Meta_Enum_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Enum_command_handler& cmd_handler;
};

command_functor* Meta_Enum_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Enum_command_functor( dynamic_cast< Meta_Enum_command_handler& >( handler ) );
}

void Meta_Enum_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Enum_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_parm_Meta_Enum_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Enum->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Enum->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Enum_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Enum->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Enum->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Enum_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Enum_get_field_name ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );
      else if( field_name == c_field_id_Id || field_name == c_field_name_Id )
         string_getter< string >( cmd_handler.p_Meta_Enum->Id( ), cmd_handler.retval );
      else if( field_name == c_field_id_Internal || field_name == c_field_name_Internal )
         string_getter< bool >( cmd_handler.p_Meta_Enum->Internal( ), cmd_handler.retval );
      else if( field_name == c_field_id_Name || field_name == c_field_name_Name )
         string_getter< string >( cmd_handler.p_Meta_Enum->Name( ), cmd_handler.retval );
      else if( field_name == c_field_id_Primitive || field_name == c_field_name_Primitive )
         string_getter< int >( cmd_handler.p_Meta_Enum->Primitive( ), cmd_handler.retval );
      else if( field_name == c_field_id_Workgroup || field_name == c_field_name_Workgroup )
         string_getter< Meta_Workgroup >( cmd_handler.p_Meta_Enum->Workgroup( ), cmd_handler.retval );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Enum_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Enum_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_parm_Meta_Enum_set_field_value ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );
      else if( field_name == c_field_id_Id || field_name == c_field_name_Id )
         func_string_setter< Meta_Enum, string >(
          *cmd_handler.p_Meta_Enum, &Meta_Enum::Id, field_value );
      else if( field_name == c_field_id_Internal || field_name == c_field_name_Internal )
         func_string_setter< Meta_Enum, bool >(
          *cmd_handler.p_Meta_Enum, &Meta_Enum::Internal, field_value );
      else if( field_name == c_field_id_Name || field_name == c_field_name_Name )
         func_string_setter< Meta_Enum, string >(
          *cmd_handler.p_Meta_Enum, &Meta_Enum::Name, field_value );
      else if( field_name == c_field_id_Primitive || field_name == c_field_name_Primitive )
         func_string_setter< Meta_Enum, int >(
          *cmd_handler.p_Meta_Enum, &Meta_Enum::Primitive, field_value );
      else if( field_name == c_field_id_Workgroup || field_name == c_field_name_Workgroup )
         func_string_setter< Meta_Enum, Meta_Workgroup >(
          *cmd_handler.p_Meta_Enum, &Meta_Enum::Workgroup, field_value );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Enum_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Enum_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_parm_Meta_Enum_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( field_name == c_field_id_Workgroup || field_name == c_field_name_Workgroup )
         cmd_handler.retval = cmd_handler.p_Meta_Enum->Workgroup( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
}

struct Meta_Enum::impl : public Meta_Enum_command_handler
{
   impl( Meta_Enum& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Enum( &o );

      add_commands( 0, Meta_Enum_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Enum_command_definitions ) );
   }

   Meta_Enum& get_obj( ) const
   {
      return *cp_obj;
   }

   const string& impl_Id( ) const { return lazy_fetch( p_obj ), v_Id; }
   void impl_Id( const string& Id ) { v_Id = Id; }

   bool impl_Internal( ) const { return lazy_fetch( p_obj ), v_Internal; }
   void impl_Internal( bool Internal ) { v_Internal = Internal; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { v_Name = Name; }

   int impl_Primitive( ) const { return lazy_fetch( p_obj ), v_Primitive; }
   void impl_Primitive( int Primitive ) { v_Primitive = Primitive; }

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

   Meta_Enum_Item& impl_child_Enum_Item( )
   {
      if( !cp_child_Enum_Item )
      {
         cp_child_Enum_Item.init( );

         p_obj->setup_graph_parent( *cp_child_Enum_Item, "300400" );
      }
      return *cp_child_Enum_Item;
   }

   const Meta_Enum_Item& impl_child_Enum_Item( ) const
   {
      if( !cp_child_Enum_Item )
      {
         cp_child_Enum_Item.init( );

         p_obj->setup_graph_parent( *cp_child_Enum_Item, "300400" );
      }
      return *cp_child_Enum_Item;
   }

   Meta_Field& impl_child_Field( )
   {
      if( !cp_child_Field )
      {
         cp_child_Field.init( );

         p_obj->setup_graph_parent( *cp_child_Field, "300720" );
      }
      return *cp_child_Field;
   }

   const Meta_Field& impl_child_Field( ) const
   {
      if( !cp_child_Field )
      {
         cp_child_Field.init( );

         p_obj->setup_graph_parent( *cp_child_Field, "300720" );
      }
      return *cp_child_Field;
   }

   Meta_Specification& impl_child_Specification( )
   {
      if( !cp_child_Specification )
      {
         cp_child_Specification.init( );

         p_obj->setup_graph_parent( *cp_child_Specification, "301460" );
      }
      return *cp_child_Specification;
   }

   const Meta_Specification& impl_child_Specification( ) const
   {
      if( !cp_child_Specification )
      {
         cp_child_Specification.init( );

         p_obj->setup_graph_parent( *cp_child_Specification, "301460" );
      }
      return *cp_child_Specification;
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

   bool is_filtered( const set< string >& filters ) const;

   Meta_Enum* p_obj;
   class_pointer< Meta_Enum > cp_obj;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   string v_Id;
   bool v_Internal;
   string v_Name;
   int v_Primitive;

   string v_Workgroup;
   mutable class_pointer< Meta_Workgroup > cp_Workgroup;

   mutable class_pointer< Meta_Enum_Item > cp_child_Enum_Item;
   mutable class_pointer< Meta_Field > cp_child_Field;
   mutable class_pointer< Meta_Specification > cp_child_Specification;
};

string Meta_Enum::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Id( ) );
      break;

      case 1:
      retval = to_string( impl_Internal( ) );
      break;

      case 2:
      retval = to_string( impl_Name( ) );
      break;

      case 3:
      retval = to_string( impl_Primitive( ) );
      break;

      case 4:
      retval = to_string( impl_Workgroup( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }

   return retval;
}

void Meta_Enum::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Enum::impl, string >( *this, &Meta_Enum::impl::impl_Id, value );
      break;

      case 1:
      func_string_setter< Meta_Enum::impl, bool >( *this, &Meta_Enum::impl::impl_Internal, value );
      break;

      case 2:
      func_string_setter< Meta_Enum::impl, string >( *this, &Meta_Enum::impl::impl_Name, value );
      break;

      case 3:
      func_string_setter< Meta_Enum::impl, int >( *this, &Meta_Enum::impl::impl_Primitive, value );
      break;

      case 4:
      func_string_setter< Meta_Enum::impl, Meta_Workgroup >( *this, &Meta_Enum::impl::impl_Workgroup, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }
}

uint64_t Meta_Enum::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start modifier_field_value)]
   if( get_obj( ).Internal( ) == true )
      state |= c_modifier_Is_Internal;
   // [(finish modifier_field_value)]

   // [(start protect_equal)]
   if( check_equal( get_obj( ).Internal( ), true ) )
      state |= ( c_state_uneditable | c_state_undeletable );
   // [(finish protect_equal)]

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

const string& Meta_Enum::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Enum::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Workgroup || field == c_field_name_Workgroup )
      impl_Workgroup( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Enum::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( field == c_field_id_Workgroup || field == c_field_name_Workgroup )
      v_Workgroup = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Enum::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Workgroup || field == c_field_name_Workgroup )
      return v_Workgroup;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Enum::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Workgroup, v_Workgroup ) );
}

void Meta_Enum::impl::clear( )
{
   v_Id = gv_default_Id;
   v_Internal = gv_default_Internal;
   v_Name = gv_default_Name;
   v_Primitive = gv_default_Primitive;

   v_Workgroup = string( );
   if( cp_Workgroup )
      p_obj->setup_foreign_key( *cp_Workgroup, v_Workgroup );
}

bool Meta_Enum::impl::value_will_be_provided( const string& field_name )
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

void Meta_Enum::impl::validate( unsigned state, bool is_internal, validation_error_container* p_validation_errors )
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

void Meta_Enum::impl::after_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   if( cp_Workgroup )
      p_obj->setup_foreign_key( *cp_Workgroup, v_Workgroup );

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Enum::impl::finalise_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Enum::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Enum::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Enum::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [(start parent_auto_int_inc)]
   if( is_create && is_null( get_obj( ).Id( ) ) )
   {
      get_obj( ).Workgroup( ).op_update( get_obj( ).Workgroup( ), FIELD_NAME( Meta, Workgroup, Next_Enum_Id ) );

      get_obj( ).Id( get_obj( ).Workgroup( ).Next_Enum_Id( ) );

      get_obj( ).Workgroup( ).Next_Enum_Id( auto_int_increment( get_obj( ).Workgroup( ).Next_Enum_Id( ) ) );
      get_obj( ).Workgroup( ).op_apply( );
   }
   // [(finish parent_auto_int_inc)]

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Enum::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [(start clone_children)]
   if( !get_obj( ).get_clone_key( ).empty( ) )
   {
      class_pointer< Meta_Enum > cp_source( e_create_instance );

      cp_source->begin_review( get_obj( ).get_clone_key( ) );

      if( cp_source->child_Enum_Item( ).iterate_forwards( ) )
      {
         int child_num = 0;
         do
         {
            string child_key( construct_key_from_int( get_obj( ).get_key( ), ++child_num ) );
            child_key += ' ';
            child_key += cp_source->child_Enum_Item( ).get_key( );

            get_obj( ).child_Enum_Item( ).op_create( child_key );
            get_obj( ).child_Enum_Item( ).Enum( get_obj( ).get_key( ) );

            get_obj( ).child_Enum_Item( ).op_apply( );

         } while( cp_source->child_Enum_Item( ).iterate_next( ) );
      }

      cp_source->finish_review( );
   }
   // [(finish clone_children)]

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

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Enum::impl::can_destroy( bool is_internal )
{
   bool retval = is_internal || !( get_state( ) & c_state_undeletable );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Enum::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Enum::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Enum::impl::set_default_values( )
{
   clear( );
}

bool Meta_Enum::impl::is_filtered( const set< string >& filters ) const
{
   ( void )filters;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

#undef MODULE_TRACE
#define MODULE_TRACE( x ) trace( x )

Meta_Enum::Meta_Enum( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Enum::~Meta_Enum( )
{
   cleanup( );
   delete p_impl;
}

const string& Meta_Enum::Id( ) const
{
   return p_impl->impl_Id( );
}

void Meta_Enum::Id( const string& Id )
{
   p_impl->impl_Id( Id );
}

bool Meta_Enum::Internal( ) const
{
   return p_impl->impl_Internal( );
}

void Meta_Enum::Internal( bool Internal )
{
   p_impl->impl_Internal( Internal );
}

const string& Meta_Enum::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_Enum::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

int Meta_Enum::Primitive( ) const
{
   return p_impl->impl_Primitive( );
}

void Meta_Enum::Primitive( int Primitive )
{
   p_impl->impl_Primitive( Primitive );
}

Meta_Workgroup& Meta_Enum::Workgroup( )
{
   return p_impl->impl_Workgroup( );
}

const Meta_Workgroup& Meta_Enum::Workgroup( ) const
{
   return p_impl->impl_Workgroup( );
}

void Meta_Enum::Workgroup( const string& key )
{
   p_impl->impl_Workgroup( key );
}

Meta_Enum_Item& Meta_Enum::child_Enum_Item( )
{
   return p_impl->impl_child_Enum_Item( );
}

const Meta_Enum_Item& Meta_Enum::child_Enum_Item( ) const
{
   return p_impl->impl_child_Enum_Item( );
}

Meta_Field& Meta_Enum::child_Field( )
{
   return p_impl->impl_child_Field( );
}

const Meta_Field& Meta_Enum::child_Field( ) const
{
   return p_impl->impl_child_Field( );
}

Meta_Specification& Meta_Enum::child_Specification( )
{
   return p_impl->impl_child_Specification( );
}

const Meta_Specification& Meta_Enum::child_Specification( ) const
{
   return p_impl->impl_child_Specification( );
}

string Meta_Enum::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Enum::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

bool Meta_Enum::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Enum::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Enum::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Enum::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Enum::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Enum::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Enum::clear( )
{
   p_impl->clear( );
}

void Meta_Enum::validate( unsigned state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Enum::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Enum::finalise_fetch( )
{
   p_impl->finalise_fetch( );
}

void Meta_Enum::at_create( )
{
   p_impl->at_create( );
}

void Meta_Enum::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Enum::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Enum::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Enum::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Enum::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Enum::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Enum::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Enum::is_filtered( const set< string >& filters ) const
{
   return p_impl->is_filtered( filters );
}

const char* Meta_Enum::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( 0 );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
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
   else if( name == c_field_name_Primitive )
   {
      p_id = c_field_id_Primitive;

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

   return p_id;
}

const char* Meta_Enum::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( 0 );

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
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
   else if( id == c_field_id_Primitive )
   {
      p_name = c_field_name_Primitive;

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

   return p_name;
}

string Meta_Enum::get_field_display_name( const string& id ) const
{
   string display_name;

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_display_name" );
   else if( id == c_field_id_Id )
      display_name = get_module_string( c_field_display_name_Id );
   else if( id == c_field_id_Internal )
      display_name = get_module_string( c_field_display_name_Internal );
   else if( id == c_field_id_Name )
      display_name = get_module_string( c_field_display_name_Name );
   else if( id == c_field_id_Primitive )
      display_name = get_module_string( c_field_display_name_Primitive );
   else if( id == c_field_id_Workgroup )
      display_name = get_module_string( c_field_display_name_Workgroup );

   return display_name;
}

void Meta_Enum::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Enum::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Enum::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Enum::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Enum::setup_foreign_key( Meta_Workgroup& o, const string& value )
{
   static_cast< Meta_Workgroup& >( o ).set_key( value );
}

void Meta_Enum::setup_graph_parent( Meta_Enum_Item& o, const string& foreign_key_field )
{
   static_cast< Meta_Enum_Item& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Enum::setup_graph_parent( Meta_Field& o, const string& foreign_key_field )
{
   static_cast< Meta_Field& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Enum::setup_graph_parent( Meta_Specification& o, const string& foreign_key_field )
{
   static_cast< Meta_Specification& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Enum::setup_graph_parent(
 Meta_Workgroup& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Workgroup& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Workgroup& >( o ).set_key( init_value );
}

size_t Meta_Enum::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Enum::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Enum::get_num_foreign_key_children( bool is_internal ) const
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

class_base* Meta_Enum::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   if( child_num >= 3 )
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
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "300400";
            p_class_base = &child_Enum_Item( );
         }
         break;

         case 1:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "300720";
            p_class_base = &child_Field( );
         }
         break;

         case 2:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301460";
            p_class_base = &child_Specification( );
         }
         break;
      }
   }

   return p_class_base;
}

const char* Meta_Enum::class_id( ) const
{
   return static_class_id( );
}

const char* Meta_Enum::class_name( ) const
{
   return static_class_name( );
}

const char* Meta_Enum::plural_name( ) const
{
   return static_plural_name( );
}

const char* Meta_Enum::module_id( ) const
{
   return static_module_id( );
}

const char* Meta_Enum::module_name( ) const
{
   return static_module_name( );
}

string Meta_Enum::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );
   key += "enum";

   return get_module_string( key );
}

bool Meta_Enum::get_is_alias( ) const
{
   return false;
}

void Meta_Enum::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Enum::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Enum::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( sub_context == "_300400" || sub_context == "child_Enum_Item" )
      p_class_base = &child_Enum_Item( );
   else if( sub_context == "_300720" || sub_context == "child_Field" )
      p_class_base = &child_Field( );
   else if( sub_context == "_301460" || sub_context == "child_Specification" )
      p_class_base = &child_Specification( );
   else if( sub_context == c_field_id_Workgroup || sub_context == c_field_name_Workgroup )
      p_class_base = &Workgroup( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Enum::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Id" );
   names.push_back( "C_Internal" );
   names.push_back( "C_Name" );
   names.push_back( "C_Primitive" );
   names.push_back( "C_Workgroup" );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Enum::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Id( ) ) ) );
   values.push_back( to_string( Internal( ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( to_string( Primitive( ) ) );
   values.push_back( sql_quote( to_string( Workgroup( ) ) ) );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Enum::get_required_field_names(
 set< string >& names, bool required_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, required_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Enum::get_always_required_field_names(
 set< string >& names, bool required_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )required_transients;

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

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Enum::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Enum::do_generate_sql( generate_sql_type type, vector< string >& sql_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts );
}

const char* Meta_Enum::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Enum::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Enum::static_lock_class_id( )
{
   return "103100";
}

const char* Meta_Enum::static_check_class_name( )
{
   return "Enum";
}

bool Meta_Enum::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Enum::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.103100" );
}

void Meta_Enum::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "103104", "Id", "string", false ) );
   all_field_info.push_back( field_info( "103103", "Internal", "bool", false ) );
   all_field_info.push_back( field_info( "103101", "Name", "string", false ) );
   all_field_info.push_back( field_info( "103102", "Primitive", "int", false ) );
   all_field_info.push_back( field_info( "300300", "Workgroup", "Meta_Workgroup", false ) );
}

void Meta_Enum::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Workgroup, make_pair( "Meta.103100", "Meta_Workgroup" ) ) );
}

int Meta_Enum::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Enum::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Enum::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "103104";
      break;

      case 2:
      p_id = "103103";
      break;

      case 3:
      p_id = "103101";
      break;

      case 4:
      p_id = "103102";
      break;

      case 5:
      p_id = "300300";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Enum" );

   return p_id;
}

const char* Meta_Enum::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Id";
      break;

      case 2:
      p_id = "Internal";
      break;

      case 3:
      p_id = "Name";
      break;

      case 4:
      p_id = "Primitive";
      break;

      case 5:
      p_id = "Workgroup";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Enum" );

   return p_id;
}

int Meta_Enum::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Id || field == c_field_name_Id )
      rc += 1;
   else if( field == c_field_id_Internal || field == c_field_name_Internal )
      rc += 2;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 3;
   else if( field == c_field_id_Primitive || field == c_field_name_Primitive )
      rc += 4;
   else if( field == c_field_id_Workgroup || field == c_field_name_Workgroup )
      rc += 5;

   return rc - 1;
}

procedure_info_container& Meta_Enum::static_get_procedure_info( )
{
   static procedure_info_container procedures;

   return procedures;
}

string Meta_Enum::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(64),"
    "C_Ver_ INTEGER NOT NULL,"
    "C_Rev_ INTEGER NOT NULL,"
    "C_Typ_ VARCHAR(24) NOT NULL,"
    "C_Id VARCHAR(128) NOT NULL,"
    "C_Internal INTEGER NOT NULL,"
    "C_Name VARCHAR(128) NOT NULL,"
    "C_Primitive INTEGER NOT NULL,"
    "C_Workgroup VARCHAR(64) NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Enum::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Enum::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "enum_primitive_0", get_enum_string_primitive( 0 ) ) );
   pairs.push_back( make_pair( "enum_primitive_1", get_enum_string_primitive( 1 ) ) );
   pairs.push_back( make_pair( "enum_primitive_2", get_enum_string_primitive( 2 ) ) );
   pairs.push_back( make_pair( "enum_primitive_3", get_enum_string_primitive( 3 ) ) );
   pairs.push_back( make_pair( "enum_primitive_4", get_enum_string_primitive( 4 ) ) );
   pairs.push_back( make_pair( "enum_primitive_5", get_enum_string_primitive( 5 ) ) );
   pairs.push_back( make_pair( "enum_primitive_6", get_enum_string_primitive( 6 ) ) );
}

void Meta_Enum::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Workgroup,C_Id" );
   indexes.push_back( "C_Workgroup,C_Name" );
}

void Meta_Enum::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Workgroup,C_Id" );
   indexes.push_back( "C_Workgroup,C_Name" );
}

void Meta_Enum::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Enum::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Enum::static_insert_external_alias( const string& module_and_class_id, Meta_Enum* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Enum::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Enum::static_class_init( const char* p_module_name )
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

void Meta_Enum::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}

