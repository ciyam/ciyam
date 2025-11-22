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

// [<start macros>]
// [<finish macros>]

#include "Meta_Auto_Code.h"

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

#include "Meta_Auto_Code.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Exhausted = "125103";
const char* const c_field_id_Mask = "125101";
const char* const c_field_id_Next = "125102";

const char* const c_field_name_Exhausted = "Exhausted";
const char* const c_field_name_Mask = "Mask";
const char* const c_field_name_Next = "Next";

const char* const c_field_display_name_Exhausted = "field_auto_code_exhausted";
const char* const c_field_display_name_Mask = "field_auto_code_mask";
const char* const c_field_display_name_Next = "field_auto_code_next";

const int c_num_fields = 3;

const char* const c_all_sorted_field_ids[ ] =
{
   "125101",
   "125102",
   "125103"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Exhausted",
   "Mask",
   "Next"
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

const char* const c_procedure_id_Increment = "125410";

domain_string_max_size< 30 > g_Mask_domain;
domain_string_max_size< 30 > g_Next_domain;

string g_group_field_name;
string g_level_field_name;
string g_order_field_name;
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_Auto_Code* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Auto_Code* > external_aliases_lookup_container;
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

bool g_default_Exhausted = bool( 0 );
string g_default_Mask = string( );
string g_default_Next = string( );

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Auto_Code > Auto_Code_registration( get_class_registry( ), "125100" );

class Meta_Auto_Code_command_functor;

class Meta_Auto_Code_command_handler : public command_handler
{
   friend class Meta_Auto_Code_command_functor;

   public:
   Meta_Auto_Code_command_handler( ) : p_Meta_Auto_Code( 0 ) { }

   void set_Meta_Auto_Code( Meta_Auto_Code* p_new_Meta_Auto_Code ) { p_Meta_Auto_Code = p_new_Meta_Auto_Code; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Auto_Code* p_Meta_Auto_Code;

   protected:
   string retval;
};

class Meta_Auto_Code_command_functor : public command_functor
{
   public:
   Meta_Auto_Code_command_functor( Meta_Auto_Code_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Auto_Code_command_handler& cmd_handler;
};

command_functor* Meta_Auto_Code_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Auto_Code_command_functor( dynamic_cast< Meta_Auto_Code_command_handler& >( handler ) );
}

void Meta_Auto_Code_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Auto_Code_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_Auto_Code_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Auto_Code->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Auto_Code->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Auto_Code_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Auto_Code->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Auto_Code->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Auto_Code_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Auto_Code_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Exhausted ) || ( field_name == c_field_name_Exhausted ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Auto_Code->Exhausted( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Mask ) || ( field_name == c_field_name_Mask ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Auto_Code->Mask( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Next ) || ( field_name == c_field_name_Next ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Auto_Code->Next( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Auto_Code_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Auto_Code_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_Auto_Code_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Exhausted ) || ( field_name == c_field_name_Exhausted ) ) )
      {
         handled = true;

         func_string_setter< Meta_Auto_Code, bool >(
          *cmd_handler.p_Meta_Auto_Code, &Meta_Auto_Code::Exhausted, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Mask ) || ( field_name == c_field_name_Mask ) ) )
      {
         handled = true;

         func_string_setter< Meta_Auto_Code, string >(
          *cmd_handler.p_Meta_Auto_Code, &Meta_Auto_Code::Mask, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Next ) || ( field_name == c_field_name_Next ) ) )
      {
         handled = true;

         func_string_setter< Meta_Auto_Code, string >(
          *cmd_handler.p_Meta_Auto_Code, &Meta_Auto_Code::Next, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Auto_Code_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Auto_Code_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_Auto_Code_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
   else if( command == c_cmd_Meta_Auto_Code_Increment )
   {
      string Next_Value;

      cmd_handler.p_Meta_Auto_Code->Increment( Next_Value );

      cmd_handler.retval.erase( );

      append_value( cmd_handler.retval, Next_Value );
   }
}

struct Meta_Auto_Code::impl : public Meta_Auto_Code_command_handler
{
   impl( Meta_Auto_Code& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Auto_Code( &o );

      add_commands( 0, Meta_Auto_Code_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Auto_Code_command_definitions ) );
   }

   Meta_Auto_Code& get_obj( ) const
   {
      return *cp_obj;
   }

   bool impl_Exhausted( ) const { return lazy_fetch( p_obj ), v_Exhausted; }
   void impl_Exhausted( bool Exhausted ) { v_Exhausted = Exhausted; }

   const string& impl_Mask( ) const { return lazy_fetch( p_obj ), v_Mask; }
   void impl_Mask( const string& Mask ) { sanity_check( Mask ); v_Mask = Mask; }

   const string& impl_Next( ) const { return lazy_fetch( p_obj ), v_Next; }
   void impl_Next( const string& Next ) { sanity_check( Next ); v_Next = Next; }

   void impl_Increment( string& Next_Value );

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

   Meta_Auto_Code* p_obj;
   class_pointer< Meta_Auto_Code > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   bool v_Exhausted;
   string v_Mask;
   string v_Next;
};

void Meta_Auto_Code::impl::impl_Increment( string& Next_Value )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start for_auto_code)] 600295
   get_obj( ).op_update( );

   if( !get_obj( ).Exhausted( ) )
   {
      string mask( get_obj( ).Mask( ) );

      if( is_null( get_obj( ).Next( ) ) )
      {
         string str;
         for( size_t i = 0; i < mask.size( ); i++ )
         {
            if( mask[ i ] == '?' )
               str += 'A';
            else if( mask[ i ] == '#' )
               str += '0';
            else
               str += mask[ i ];
         }

         get_obj( ).Next( str );
      }

      Next_Value = get_obj( ).Next( );

      if( mask.size( ) != get_obj( ).Next( ).size( ) )
      {
         get_obj( ).op_cancel( );

         throw runtime_error( "unexpected mask/next size mismatch" );
      }

      bool finished = false;

      string str( get_obj( ).Next( ) );

      for( size_t i = mask.size( ); i > 0; i-- )
      {
         if( mask[ i - 1 ] == '?' )
         {
            if( str[ i - 1 ] == 'Z' )
               str[ i - 1 ] = 'A';
            else
            {
               ++str[ i - 1 ];
               finished = true;
            }
         }
         else if( mask[ i - 1 ] == '#' )
         {
            if( str[ i - 1 ] == '9' )
               str[ i - 1 ] = '0';
            else
            {
               ++str[ i - 1 ];
               finished = true;
            }
         }

         if( finished )
         {
            get_obj( ).Next( str );
            break;
         }
      }

      if( !finished )
      {
         get_obj( ).Next( "" );
         get_obj( ).Exhausted( true );
      }

      get_obj( ).op_apply( );
   }
   else
      get_obj( ).op_cancel( );
   // [(finish for_auto_code)] 600295

   // [<start Increment_impl>]
   // [<finish Increment_impl>]
}

string Meta_Auto_Code::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Exhausted( ) );
      break;

      case 1:
      retval = to_string( impl_Mask( ) );
      break;

      case 2:
      retval = to_string( impl_Next( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_Auto_Code::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Auto_Code::impl, bool >( *this, &Meta_Auto_Code::impl::impl_Exhausted, value );
      break;

      case 1:
      func_string_setter< Meta_Auto_Code::impl, string >( *this, &Meta_Auto_Code::impl::impl_Mask, value );
      break;

      case 2:
      func_string_setter< Meta_Auto_Code::impl, string >( *this, &Meta_Auto_Code::impl::impl_Next, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_Auto_Code::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Exhausted( g_default_Exhausted );
      break;

      case 1:
      impl_Mask( g_default_Mask );
      break;

      case 2:
      impl_Next( g_default_Next );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_Auto_Code::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Exhausted == g_default_Exhausted );
      break;

      case 1:
      retval = ( v_Mask == g_default_Mask );
      break;

      case 2:
      retval = ( v_Next == g_default_Next );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_Auto_Code::impl::get_state( ) const
{
   uint64_t state = 0;

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

string Meta_Auto_Code::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_Auto_Code::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Auto_Code::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Auto_Code::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Auto_Code::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Auto_Code::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   ( void )foreign_key_values;
}

void Meta_Auto_Code::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_Auto_Code::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Auto_Code::impl::clear( )
{
   v_Exhausted = g_default_Exhausted;
   v_Mask = g_default_Mask;
   v_Next = g_default_Next;
}

bool Meta_Auto_Code::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Auto_Code::impl::validate(
 uint64_t state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( is_null( v_Mask ) && !value_will_be_provided( c_field_name_Mask ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Mask,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Mask ) ) ) ) );

   if( !is_null( v_Mask )
    && ( v_Mask != g_default_Mask
    || !value_will_be_provided( c_field_name_Mask ) )
    && !g_Mask_domain.is_valid( v_Mask, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Mask,
       get_module_string( c_field_display_name_Mask ) + " " + error_message ) );

   if( !is_null( v_Next )
    && ( v_Next != g_default_Next
    || !value_will_be_provided( c_field_name_Next ) )
    && !g_Next_domain.is_valid( v_Next, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Next,
       get_module_string( c_field_display_name_Next ) + " " + error_message ) );

   // [(start for_auto_code)] 600295
   if( !get_obj( ).Next( ).empty( ) )
   {
      bool okay = true;

      string next( get_obj( ).Next( ) );
      string mask( get_obj( ).Mask( ) );

      if( next.length( ) != mask.length( ) )
         okay = false;
      else
      {
         for( size_t i = 0; i < next.length( ); i++ )
         {
            if( next[ i ] != mask[ i ] )
            {
               if( mask[ i ] == '?' )
               {
                  if( next[ i ] < 'A' || next[ i ] > 'Z' )
                     okay = false;
               }
               else if( mask[ i ] == '#' )
               {
                  if( next[ i ] < '0' || next[ i ] > '9' )
                     okay = false;
               }
               else
                  okay = false;
            }

            if( !okay )
               break;
         }
      }

      if( !okay )
         p_validation_errors->insert( validation_error_value_type( c_field_name_Next,
          get_string_message( GS( c_str_field_mismatch ), make_pair(
          c_str_field_mismatch_field1, get_module_string( c_field_display_name_Next ) ),
          make_pair( c_str_field_mismatch_field2, get_module_string( c_field_display_name_Mask ) ) ) ) );
   }
   // [(finish for_auto_code)] 600295

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Auto_Code::impl::validate_set_fields(
 set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( !is_null( v_Mask )
    && ( fields_set.count( c_field_id_Mask ) || fields_set.count( c_field_name_Mask ) )
    && !g_Mask_domain.is_valid( v_Mask, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Mask,
       get_module_string( c_field_display_name_Mask ) + " " + error_message ) );

   if( !is_null( v_Next )
    && ( fields_set.count( c_field_id_Next ) || fields_set.count( c_field_name_Next ) )
    && !g_Next_domain.is_valid( v_Next, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Next,
       get_module_string( c_field_display_name_Next ) + " " + error_message ) );
}

void Meta_Auto_Code::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Auto_Code::impl::completed_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start completed_fetch>]
   // [<finish completed_fetch>]
}

void Meta_Auto_Code::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Auto_Code::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_Auto_Code::impl::to_store( bool is_create, bool is_internal )
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

void Meta_Auto_Code::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Auto_Code::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Auto_Code::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Auto_Code::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Auto_Code::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Auto_Code::impl::set_default_values( )
{
   clear( );
}

bool Meta_Auto_Code::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_Auto_Code::impl::get_required_transients( ) const
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

Meta_Auto_Code::Meta_Auto_Code( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Auto_Code::~Meta_Auto_Code( )
{
   cleanup( );
   delete p_impl;
}

bool Meta_Auto_Code::Exhausted( ) const
{
   return p_impl->impl_Exhausted( );
}

void Meta_Auto_Code::Exhausted( bool Exhausted )
{
   p_impl->impl_Exhausted( Exhausted );
}

const string& Meta_Auto_Code::Mask( ) const
{
   return p_impl->impl_Mask( );
}

void Meta_Auto_Code::Mask( const string& Mask )
{
   p_impl->impl_Mask( Mask );
}

const string& Meta_Auto_Code::Next( ) const
{
   return p_impl->impl_Next( );
}

void Meta_Auto_Code::Next( const string& Next )
{
   p_impl->impl_Next( Next );
}

void Meta_Auto_Code::Increment( string& Next_Value )
{
   p_impl->impl_Increment( Next_Value );
}

string Meta_Auto_Code::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Auto_Code::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_Auto_Code::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_Auto_Code::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_Auto_Code::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_Auto_Code::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_Auto_Code::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_Auto_Code::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_Auto_Code::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_Auto_Code::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Auto_Code::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_Auto_Code::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Auto_Code::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Auto_Code::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Auto_Code::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Auto_Code::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Auto_Code::clear( )
{
   p_impl->clear( );
}

void Meta_Auto_Code::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Auto_Code::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Auto_Code::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Auto_Code::completed_fetch( bool skip_set_original )
{
   p_impl->completed_fetch( skip_set_original );
}

void Meta_Auto_Code::at_create( )
{
   p_impl->at_create( );
}

void Meta_Auto_Code::post_init( )
{
   p_impl->post_init( );
}

void Meta_Auto_Code::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Auto_Code::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Auto_Code::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Auto_Code::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Auto_Code::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Auto_Code::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Auto_Code::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Auto_Code::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Auto_Code::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( 0 );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Exhausted )
   {
      p_id = c_field_id_Exhausted;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Mask )
   {
      p_id = c_field_id_Mask;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Next )
   {
      p_id = c_field_id_Next;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_id;
}

const char* Meta_Auto_Code::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( 0 );

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Exhausted )
   {
      p_name = c_field_name_Exhausted;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Mask )
   {
      p_name = c_field_name_Mask;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Next )
   {
      p_name = c_field_name_Next;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_name;
}

string& Meta_Auto_Code::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_Auto_Code::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_Auto_Code::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_Auto_Code::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_Auto_Code::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_Auto_Code::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Auto_Code::get_field_uom_symbol( const string& id_or_name ) const
{
   string uom_symbol;

   string name;
   pair< string, string > next;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_uom_symbol" );
   if( ( id_or_name == c_field_id_Exhausted ) || ( id_or_name == c_field_name_Exhausted ) )
   {
      name = string( c_field_display_name_Exhausted );
      get_module_string( c_field_display_name_Exhausted, &next );
   }
   if( ( id_or_name == c_field_id_Mask ) || ( id_or_name == c_field_name_Mask ) )
   {
      name = string( c_field_display_name_Mask );
      get_module_string( c_field_display_name_Mask, &next );
   }
   if( ( id_or_name == c_field_id_Next ) || ( id_or_name == c_field_name_Next ) )
   {
      name = string( c_field_display_name_Next );
      get_module_string( c_field_display_name_Next, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_Auto_Code::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Exhausted ) || ( id_or_name == c_field_name_Exhausted ) )
      display_name = get_module_string( c_field_display_name_Exhausted );
   if( ( id_or_name == c_field_id_Mask ) || ( id_or_name == c_field_name_Mask ) )
      display_name = get_module_string( c_field_display_name_Mask );
   if( ( id_or_name == c_field_id_Next ) || ( id_or_name == c_field_name_Next ) )
      display_name = get_module_string( c_field_display_name_Next );

   return display_name;
}

void Meta_Auto_Code::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Auto_Code::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Auto_Code::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Auto_Code::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

size_t Meta_Auto_Code::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Auto_Code::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Auto_Code::get_num_foreign_key_children( bool is_internal ) const
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

class_base* Meta_Auto_Code::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   ( void )child_num;
   ( void )next_child_field;
   ( void )op;

   return p_class_base;
}

void Meta_Auto_Code::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_Auto_Code::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_Auto_Code::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_Auto_Code::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_Auto_Code::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_Auto_Code::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_Auto_Code::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_Auto_Code::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "auto_code";

   return get_module_string( key );
}

string Meta_Auto_Code::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_Auto_Code::get_create_instance_info( ) const
{
   return "";
}

string Meta_Auto_Code::get_update_instance_info( ) const
{
   return "";
}

string Meta_Auto_Code::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_Auto_Code::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );
   else if( procedure_id == "125410" ) // i.e. Increment
      retval = "";

   return retval;
}

bool Meta_Auto_Code::get_is_alias( ) const
{
   return false;
}

void Meta_Auto_Code::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Auto_Code::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Auto_Code::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Auto_Code::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Exhausted" );
   names.push_back( "C_Mask" );
   names.push_back( "C_Next" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Auto_Code::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( to_string( Exhausted( ) ) );
   values.push_back( sql_quote( to_string( Mask( ) ) ) );
   values.push_back( sql_quote( to_string( Next( ) ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Auto_Code::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Auto_Code::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Auto_Code::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Auto_Code::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_Auto_Code::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Auto_Code::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Auto_Code::static_lock_class_id( )
{
   return "125100";
}

const char* Meta_Auto_Code::static_check_class_name( )
{
   return "Auto_Code";
}

const char* Meta_Auto_Code::static_persistence_extra( )
{
   return "";
}

bool Meta_Auto_Code::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Auto_Code::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.125100" );
}

void Meta_Auto_Code::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "125103", "Exhausted", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "125101", "Mask", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "125102", "Next", "string", false, "", "" ) );
}

void Meta_Auto_Code::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;
}

int Meta_Auto_Code::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Auto_Code::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_Auto_Code::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Auto_Code::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "125103";
      break;

      case 2:
      p_id = "125101";
      break;

      case 3:
      p_id = "125102";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Auto_Code" );

   return p_id;
}

const char* Meta_Auto_Code::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Exhausted";
      break;

      case 2:
      p_id = "Mask";
      break;

      case 3:
      p_id = "Next";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Auto_Code" );

   return p_id;
}

int Meta_Auto_Code::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( ( field == c_field_id_Exhausted ) || ( field == c_field_name_Exhausted ) )
      rc += 1;
   else if( ( field == c_field_id_Mask ) || ( field == c_field_name_Mask ) )
      rc += 2;
   else if( ( field == c_field_id_Next ) || ( field == c_field_name_Next ) )
      rc += 3;

   return rc - 1;
}

procedure_info_container& Meta_Auto_Code::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "125410", procedure_info( "Increment" ) ) );
   }

   return procedures;
}

string Meta_Auto_Code::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(75),"
    "C_Ver_ SMALLINT UNSIGNED NOT NULL,"
    "C_Rev_ BIGINT UNSIGNED NOT NULL,"
    "C_Sec_ BIGINT UNSIGNED NOT NULL,"
    "C_Typ_ VARCHAR(16) NOT NULL,"
    "C_Exhausted INTEGER NOT NULL,"
    "C_Mask VARCHAR(200) NOT NULL,"
    "C_Next VARCHAR(200) NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Auto_Code::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Auto_Code::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   ( void )pairs;
}

void Meta_Auto_Code::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   ( void )pairs;
}

void Meta_Auto_Code::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   ( void )unique_indexes;
}

void Meta_Auto_Code::static_get_sql_indexes( vector< string >& indexes )
{
   ( void )indexes;
}

void Meta_Auto_Code::static_get_sql_unique_indexes( vector< string >& indexes )
{
   ( void )indexes;
}

void Meta_Auto_Code::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Auto_Code::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Auto_Code::static_insert_external_alias( const string& module_and_class_id, Meta_Auto_Code* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Auto_Code::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Auto_Code::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Auto_Code::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
