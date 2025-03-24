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

#include "Meta_Module.h"

#include "Meta_Application.h"
#include "Meta_Model.h"

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

template< > inline string to_string( const Meta_Application& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Model& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Application& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Model& c, const string& s )
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

#include "Meta_Module.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Application = "302210";
const char* const c_field_id_Model = "302200";
const char* const c_field_id_Order = "126101";

const char* const c_field_name_Application = "Application";
const char* const c_field_name_Model = "Model";
const char* const c_field_name_Order = "Order";

const char* const c_field_display_name_Application = "field_module_application";
const char* const c_field_display_name_Model = "field_module_model";
const char* const c_field_display_name_Order = "field_module_order";

const int c_num_fields = 3;

const char* const c_all_sorted_field_ids[ ] =
{
   "126101",
   "302200",
   "302210"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Application",
   "Model",
   "Order"
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

const char* const c_procedure_id_Move_Down = "126420";
const char* const c_procedure_id_Move_Up = "126410";

string g_group_field_name;
string g_level_field_name;
string g_order_field_name( "Order" );
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_Module* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Module* > external_aliases_lookup_container;
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

string g_default_Application = string( );
string g_default_Model = string( );
string g_default_Order = string( );

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Module > Module_registration( get_class_registry( ), "126100" );

class Meta_Module_command_functor;

class Meta_Module_command_handler : public command_handler
{
   friend class Meta_Module_command_functor;

   public:
   Meta_Module_command_handler( ) : p_Meta_Module( 0 ) { }

   void set_Meta_Module( Meta_Module* p_new_Meta_Module ) { p_Meta_Module = p_new_Meta_Module; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Module* p_Meta_Module;

   protected:
   string retval;
};

class Meta_Module_command_functor : public command_functor
{
   public:
   Meta_Module_command_functor( Meta_Module_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Module_command_handler& cmd_handler;
};

command_functor* Meta_Module_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Module_command_functor( dynamic_cast< Meta_Module_command_handler& >( handler ) );
}

void Meta_Module_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Module_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_Module_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Module->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Module->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Module_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Module->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Module->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Module_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Module_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Application ) || ( field_name == c_field_name_Application ) ) )
      {
         handled = true;

         string_getter< Meta_Application >( cmd_handler.p_Meta_Module->Application( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) ) )
      {
         handled = true;

         string_getter< Meta_Model >( cmd_handler.p_Meta_Module->Model( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Order ) || ( field_name == c_field_name_Order ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Module->Order( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Module_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Module_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_Module_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Application ) || ( field_name == c_field_name_Application ) ) )
      {
         handled = true;

         func_string_setter< Meta_Module, Meta_Application >(
          *cmd_handler.p_Meta_Module, &Meta_Module::Application, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) ) )
      {
         handled = true;

         func_string_setter< Meta_Module, Meta_Model >(
          *cmd_handler.p_Meta_Module, &Meta_Module::Model, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Order ) || ( field_name == c_field_name_Order ) ) )
      {
         handled = true;

         func_string_setter< Meta_Module, string >(
          *cmd_handler.p_Meta_Module, &Meta_Module::Order, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Module_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Module_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_Module_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_Application ) || ( field_name == c_field_name_Application ) )
         cmd_handler.retval = cmd_handler.p_Meta_Module->Application( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) )
         cmd_handler.retval = cmd_handler.p_Meta_Module->Model( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
   else if( command == c_cmd_Meta_Module_Move_Down )
   {
      string Restrict_Fields( get_parm_val_from_string< string >( parameters, c_cmd_Meta_Module_Move_Down_Restrict_Fields ) );
      string Restrict_Values( get_parm_val_from_string< string >( parameters, c_cmd_Meta_Module_Move_Down_Restrict_Values ) );

      cmd_handler.p_Meta_Module->Move_Down( Restrict_Fields, Restrict_Values );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Module_Move_Up )
   {
      string Restrict_Fields( get_parm_val_from_string< string >( parameters, c_cmd_Meta_Module_Move_Up_Restrict_Fields ) );
      string Restrict_Values( get_parm_val_from_string< string >( parameters, c_cmd_Meta_Module_Move_Up_Restrict_Values ) );

      cmd_handler.p_Meta_Module->Move_Up( Restrict_Fields, Restrict_Values );

      cmd_handler.retval.erase( );

   }
}

struct Meta_Module::impl : public Meta_Module_command_handler
{
   impl( Meta_Module& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Module( &o );

      add_commands( 0, Meta_Module_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Module_command_definitions ) );
   }

   Meta_Module& get_obj( ) const
   {
      return *cp_obj;
   }

   const string& impl_Order( ) const { return lazy_fetch( p_obj ), v_Order; }
   void impl_Order( const string& Order ) { sanity_check( Order ); v_Order = Order; }

   Meta_Application& impl_Application( )
   {
      if( !cp_Application )
      {
         cp_Application.init( );

         p_obj->setup_graph_parent( *cp_Application, c_field_id_Application, v_Application );
      }
      return *cp_Application;
   }

   const Meta_Application& impl_Application( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Application )
      {
         cp_Application.init( );

         p_obj->setup_graph_parent( *cp_Application, c_field_id_Application, v_Application );
      }
      return *cp_Application;
   }

   void impl_Application( const string& key )
   {
      class_base_accessor cba( impl_Application( ) );
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

   void impl_Move_Down( const string& Restrict_Fields, const string& Restrict_Values );

   void impl_Move_Up( const string& Restrict_Fields, const string& Restrict_Values );

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

   Meta_Module* p_obj;
   class_pointer< Meta_Module > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   string v_Order;

   string v_Application;
   mutable class_pointer< Meta_Application > cp_Application;

   string v_Model;
   mutable class_pointer< Meta_Model > cp_Model;
};

void Meta_Module::impl::impl_Move_Down( const string& Restrict_Fields, const string& Restrict_Values )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start move_up_and_down)] 600701
   transaction_start( );
   try
   {
      // NOTE: Empty code block for scope purposes.
      {
         get_obj( ).op_update( get_obj( ).get_key( ), c_field_name_Order );

         class_pointer< Meta_Module > cp_other( e_create_instance );

         int num_fixed = 0;
         if( !Restrict_Fields.empty( ) )
            num_fixed = split_count( Restrict_Fields );

         string key_info( Restrict_Fields );
         if( num_fixed )
            key_info += ',';
         key_info += string( c_field_id_Order );

         if( !num_fixed )
            key_info += ' ' + get_obj( ).Order( );
         else
         {
            key_info += '#' + to_string( num_fixed );
            key_info += ' ' + Restrict_Values + ',' + get_obj( ).Order( );
         }

         // NOTE: Only the first record is required so set the row limit to 1.
         if( cp_other->iterate_forwards( key_info, string( c_field_name_Order ), false, 1 ) )
         {
            string old_val( get_obj( ).Order( ) );
            string new_val( cp_other->Order( ) );

            get_obj( ).Order( gen_key( ) );
            get_obj( ).op_apply( );

            get_obj( ).op_update( get_obj( ).get_key( ), c_field_name_Order );

            cp_other->op_update( cp_other->get_key( ), c_field_name_Order );
            cp_other->Order( old_val );
            cp_other->op_apply( );

            get_obj( ).Order( new_val );
            get_obj( ).op_apply( );

            cp_other->iterate_stop( );
         }
         else
            get_obj( ).op_cancel( );
      }

      transaction_commit( );
   }
   catch( ... )
   {
      transaction_rollback( );
      throw;
   }
   // [(finish move_up_and_down)] 600701

   // [<start Move_Down_impl>]
   // [<finish Move_Down_impl>]
}

void Meta_Module::impl::impl_Move_Up( const string& Restrict_Fields, const string& Restrict_Values )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start move_up_and_down)] 600701
   transaction_start( );
   try
   {
      // NOTE: Empty code block for scope purposes.
      {
         get_obj( ).op_update( get_obj( ).get_key( ), c_field_name_Order );

         class_pointer< Meta_Module > cp_other( e_create_instance );

         int num_fixed = 0;
         if( !Restrict_Fields.empty( ) )
            num_fixed = split_count( Restrict_Fields );

         string key_info( Restrict_Fields );
         if( num_fixed )
            key_info += ',';
         key_info += string( c_field_id_Order );

         if( !num_fixed )
            key_info += ' ' + get_obj( ).Order( );
         else
         {
            key_info += '#' + to_string( num_fixed );
            key_info += ' ' + Restrict_Values + ',' + get_obj( ).Order( );
         }

         // NOTE: Only the first record is required so set the row limit to 1.
         if( cp_other->iterate_backwards( key_info, string( c_field_name_Order ), false, 1 ) )
         {
            string old_val( get_obj( ).Order( ) );
            string new_val( cp_other->Order( ) );

            get_obj( ).Order( gen_key( ) );
            get_obj( ).op_apply( );

            get_obj( ).op_update( get_obj( ).get_key( ), c_field_name_Order );

            cp_other->op_update( cp_other->get_key( ), c_field_name_Order );
            cp_other->Order( old_val );
            cp_other->op_apply( );

            get_obj( ).Order( new_val );
            get_obj( ).op_apply( );

            cp_other->iterate_stop( );
         }
         else
            get_obj( ).op_cancel( );
      }

      transaction_commit( );
   }
   catch( ... )
   {
      transaction_rollback( );
      throw;
   }
   // [(finish move_up_and_down)] 600701

   // [<start Move_Up_impl>]
   // [<finish Move_Up_impl>]
}

string Meta_Module::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Application( ) );
      break;

      case 1:
      retval = to_string( impl_Model( ) );
      break;

      case 2:
      retval = to_string( impl_Order( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_Module::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Module::impl, Meta_Application >( *this, &Meta_Module::impl::impl_Application, value );
      break;

      case 1:
      func_string_setter< Meta_Module::impl, Meta_Model >( *this, &Meta_Module::impl::impl_Model, value );
      break;

      case 2:
      func_string_setter< Meta_Module::impl, string >( *this, &Meta_Module::impl::impl_Order, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_Module::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Application( g_default_Application );
      break;

      case 1:
      impl_Model( g_default_Model );
      break;

      case 2:
      impl_Order( g_default_Order );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_Module::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Application == g_default_Application );
      break;

      case 1:
      retval = ( v_Model == g_default_Model );
      break;

      case 2:
      retval = ( v_Order == g_default_Order );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_Module::impl::get_state( ) const
{
   uint64_t state = 0;

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

string Meta_Module::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_Module::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Module::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Application ) || ( field == c_field_name_Application ) )
      impl_Application( "" );
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      impl_Model( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Module::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_Application ) || ( field == c_field_name_Application ) )
      v_Application = value;
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      v_Model = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Module::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Application ) || ( field == c_field_name_Application ) )
      return v_Application;
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      return v_Model;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Module::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Application, v_Application ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Model, v_Model ) );
}

void Meta_Module::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_Module::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Module::impl::clear( )
{
   v_Order = g_default_Order;

   v_Application = string( );
   if( cp_Application )
      p_obj->setup_foreign_key( *cp_Application, v_Application );

   v_Model = string( );
   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );
}

bool Meta_Module::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Module::impl::validate(
 uint64_t state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( is_null( v_Order ) && !value_will_be_provided( c_field_name_Order ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Order,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Order ) ) ) ) );

   if( v_Application.empty( ) && !value_will_be_provided( c_field_name_Application ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Application,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Application ) ) ) ) );

   if( v_Model.empty( ) && !value_will_be_provided( c_field_name_Model ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Model,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Model ) ) ) ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Module::impl::validate_set_fields(
 set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;
}

void Meta_Module::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Application )
      p_obj->setup_foreign_key( *cp_Application, v_Application );

   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Module::impl::finalise_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Module::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Module::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_Module::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start default_from_key)] 600700
   if( !get_obj( ).get_clone_key( ).empty( ) || ( is_create && ( get_obj( ).Order( ) == g_default_Order ) ) )
      get_obj( ).Order( get_obj( ).get_key( ) );
   // [(finish default_from_key)] 600700

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Module::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Module::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Module::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Module::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Module::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Module::impl::set_default_values( )
{
   clear( );
}

bool Meta_Module::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_Module::impl::get_required_transients( ) const
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

Meta_Module::Meta_Module( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Module::~Meta_Module( )
{
   cleanup( );
   delete p_impl;
}

const string& Meta_Module::Order( ) const
{
   return p_impl->impl_Order( );
}

void Meta_Module::Order( const string& Order )
{
   p_impl->impl_Order( Order );
}

Meta_Application& Meta_Module::Application( )
{
   return p_impl->impl_Application( );
}

const Meta_Application& Meta_Module::Application( ) const
{
   return p_impl->impl_Application( );
}

void Meta_Module::Application( const string& key )
{
   p_impl->impl_Application( key );
}

Meta_Model& Meta_Module::Model( )
{
   return p_impl->impl_Model( );
}

const Meta_Model& Meta_Module::Model( ) const
{
   return p_impl->impl_Model( );
}

void Meta_Module::Model( const string& key )
{
   p_impl->impl_Model( key );
}

void Meta_Module::Move_Down( const string& Restrict_Fields, const string& Restrict_Values )
{
   p_impl->impl_Move_Down( Restrict_Fields, Restrict_Values );
}

void Meta_Module::Move_Up( const string& Restrict_Fields, const string& Restrict_Values )
{
   p_impl->impl_Move_Up( Restrict_Fields, Restrict_Values );
}

string Meta_Module::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Module::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_Module::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_Module::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_Module::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_Module::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_Module::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_Module::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_Module::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_Module::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Module::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_Module::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Module::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Module::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Module::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Module::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Module::clear( )
{
   p_impl->clear( );
}

void Meta_Module::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Module::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Module::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Module::finalise_fetch( bool skip_set_original )
{
   p_impl->finalise_fetch( skip_set_original );
}

void Meta_Module::at_create( )
{
   p_impl->at_create( );
}

void Meta_Module::post_init( )
{
   p_impl->post_init( );
}

void Meta_Module::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Module::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Module::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Module::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Module::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Module::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Module::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Module::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Module::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( 0 );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Application )
   {
      p_id = c_field_id_Application;

      if( p_type_name )
         *p_type_name = "Meta_Application";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Model )
   {
      p_id = c_field_id_Model;

      if( p_type_name )
         *p_type_name = "Meta_Model";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Order )
   {
      p_id = c_field_id_Order;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_id;
}

const char* Meta_Module::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( 0 );

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Application )
   {
      p_name = c_field_name_Application;

      if( p_type_name )
         *p_type_name = "Meta_Application";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Model )
   {
      p_name = c_field_name_Model;

      if( p_type_name )
         *p_type_name = "Meta_Model";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Order )
   {
      p_name = c_field_name_Order;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_name;
}

string& Meta_Module::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_Module::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_Module::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_Module::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_Module::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_Module::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Module::get_field_uom_symbol( const string& id_or_name ) const
{
   string uom_symbol;

   string name;
   pair< string, string > next;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_uom_symbol" );
   if( ( id_or_name == c_field_id_Application ) || ( id_or_name == c_field_name_Application ) )
   {
      name = string( c_field_display_name_Application );
      get_module_string( c_field_display_name_Application, &next );
   }
   if( ( id_or_name == c_field_id_Model ) || ( id_or_name == c_field_name_Model ) )
   {
      name = string( c_field_display_name_Model );
      get_module_string( c_field_display_name_Model, &next );
   }
   if( ( id_or_name == c_field_id_Order ) || ( id_or_name == c_field_name_Order ) )
   {
      name = string( c_field_display_name_Order );
      get_module_string( c_field_display_name_Order, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_Module::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Application ) || ( id_or_name == c_field_name_Application ) )
      display_name = get_module_string( c_field_display_name_Application );
   if( ( id_or_name == c_field_id_Model ) || ( id_or_name == c_field_name_Model ) )
      display_name = get_module_string( c_field_display_name_Model );
   if( ( id_or_name == c_field_id_Order ) || ( id_or_name == c_field_name_Order ) )
      display_name = get_module_string( c_field_display_name_Order );

   return display_name;
}

void Meta_Module::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Module::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Module::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Module::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Module::setup_foreign_key( Meta_Application& o, const string& value )
{
   static_cast< Meta_Application& >( o ).set_key( value );
}

void Meta_Module::setup_foreign_key( Meta_Model& o, const string& value )
{
   static_cast< Meta_Model& >( o ).set_key( value );
}

void Meta_Module::setup_graph_parent(
 Meta_Application& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Application& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Application& >( o ).set_key( init_value );
}

void Meta_Module::setup_graph_parent(
 Meta_Model& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Model& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Model& >( o ).set_key( init_value );
}

size_t Meta_Module::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Module::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Module::get_num_foreign_key_children( bool is_internal ) const
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

class_base* Meta_Module::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   ( void )child_num;
   ( void )next_child_field;
   ( void )op;

   return p_class_base;
}

void Meta_Module::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_Module::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_Module::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_Module::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_Module::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_Module::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_Module::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_Module::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "module";

   return get_module_string( key );
}

string Meta_Module::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_Module::get_create_instance_info( ) const
{
   return "";
}

string Meta_Module::get_update_instance_info( ) const
{
   return "";
}

string Meta_Module::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_Module::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );
   else if( procedure_id == "126420" ) // i.e. Move_Down
      retval = "";
   else if( procedure_id == "126410" ) // i.e. Move_Up
      retval = "";

   return retval;
}

bool Meta_Module::get_is_alias( ) const
{
   return false;
}

void Meta_Module::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Module::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Module::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == c_field_id_Application ) || ( sub_context == c_field_name_Application ) )
      p_class_base = &Application( );
   else if( ( sub_context == c_field_id_Model ) || ( sub_context == c_field_name_Model ) )
      p_class_base = &Model( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Module::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Application" );
   names.push_back( "C_Model" );
   names.push_back( "C_Order" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Module::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Application( ) ) ) );
   values.push_back( sql_quote( to_string( Model( ) ) ) );
   values.push_back( sql_quote( to_string( Order( ) ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Module::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Module::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start move_up_and_down)] 600701
   dependents.insert( "Order" );

   if( ( use_transients && is_field_transient( e_field_id_Order ) )
    || ( !use_transients && !is_field_transient( e_field_id_Order ) ) )
      names.insert( "Order" );
   // [(finish move_up_and_down)] 600701

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Module::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Module::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_Module::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Module::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Module::static_lock_class_id( )
{
   return "126100";
}

const char* Meta_Module::static_check_class_name( )
{
   return "Module";
}

const char* Meta_Module::static_persistence_extra( )
{
   return "";
}

bool Meta_Module::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Module::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.126100" );
}

void Meta_Module::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "302210", "Application", "Meta_Application", true, "", "" ) );
   all_field_info.push_back( field_info( "302200", "Model", "Meta_Model", true, "", "" ) );
   all_field_info.push_back( field_info( "126101", "Order", "string", false, "", "" ) );
}

void Meta_Module::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Application, make_pair( "Meta.126100", "Meta_Application" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Model, make_pair( "Meta.126100", "Meta_Model" ) ) );
}

int Meta_Module::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Module::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_Module::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Module::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "302210";
      break;

      case 2:
      p_id = "302200";
      break;

      case 3:
      p_id = "126101";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Module" );

   return p_id;
}

const char* Meta_Module::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Application";
      break;

      case 2:
      p_id = "Model";
      break;

      case 3:
      p_id = "Order";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Module" );

   return p_id;
}

int Meta_Module::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Application || field == c_field_name_Application )
      rc += 1;
   else if( field == c_field_id_Model || field == c_field_name_Model )
      rc += 2;
   else if( field == c_field_id_Order || field == c_field_name_Order )
      rc += 3;

   return rc - 1;
}

procedure_info_container& Meta_Module::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "126420", procedure_info( "Move_Down" ) ) );
      procedures.insert( make_pair( "126410", procedure_info( "Move_Up" ) ) );
   }

   return procedures;
}

string Meta_Module::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(75),"
    "C_Ver_ SMALLINT UNSIGNED NOT NULL,"
    "C_Rev_ BIGINT UNSIGNED NOT NULL,"
    "C_Sec_ BIGINT UNSIGNED NOT NULL,"
    "C_Typ_ VARCHAR(16) NOT NULL,"
    "C_Application VARCHAR(75) NOT NULL,"
    "C_Model VARCHAR(75) NOT NULL,"
    "C_Order VARCHAR(200) NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Module::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Module::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   ( void )pairs;
}

void Meta_Module::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "Application,Model", "string,string" ) );
   pairs.push_back( make_pair( "Application,Order", "string,string" ) );
}

void Meta_Module::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "Application,Model" );
   unique_indexes.push_back( "Application,Order" );
}

void Meta_Module::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Application,C_Model" );
   indexes.push_back( "C_Application,C_Order" );
}

void Meta_Module::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Application,C_Model" );
   indexes.push_back( "C_Application,C_Order" );
}

void Meta_Module::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Module::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Module::static_insert_external_alias( const string& module_and_class_id, Meta_Module* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Module::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Module::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Module::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
