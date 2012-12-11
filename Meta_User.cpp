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

#define CIYAM_BASE_LIB
#define MODULE_META_IMPL

// [<start macros>]
// [<finish macros>]

#include "Meta_User.h"

#include "Meta_Workgroup.h"

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

#include "Meta_User.cmh"

const int32_t c_version = 1;

const char* const c_okay = "okay";

const char* const c_field_id_Active = "100102";
const char* const c_field_id_Description = "100104";
const char* const c_field_id_Email = "100105";
const char* const c_field_id_Password = "100103";
const char* const c_field_id_Permissions = "100106";
const char* const c_field_id_User_Id = "100101";
const char* const c_field_id_Workgroup = "300100";

const char* const c_field_name_Active = "Active";
const char* const c_field_name_Description = "Description";
const char* const c_field_name_Email = "Email";
const char* const c_field_name_Password = "Password";
const char* const c_field_name_Permissions = "Permissions";
const char* const c_field_name_User_Id = "User_Id";
const char* const c_field_name_Workgroup = "Workgroup";

const char* const c_field_display_name_Active = "field_user_active";
const char* const c_field_display_name_Description = "field_user_description";
const char* const c_field_display_name_Email = "field_user_email";
const char* const c_field_display_name_Password = "field_user_password";
const char* const c_field_display_name_Permissions = "field_user_permissions";
const char* const c_field_display_name_User_Id = "field_user_user_id";
const char* const c_field_display_name_Workgroup = "field_user_workgroup";

const int c_num_fields = 7;

const char* const c_all_sorted_field_ids[ ] =
{
   "100101",
   "100102",
   "100103",
   "100104",
   "100105",
   "100106",
   "300100"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Active",
   "Description",
   "Email",
   "Password",
   "Permissions",
   "User_Id",
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

domain_string_max_size< 100 > g_Description_domain;
domain_string_max_size< 100 > g_Password_domain;
domain_string_max_size< 30 > g_User_Id_domain;

string g_order_field_name;

set< string > g_derivations;

set< string > g_file_field_names;

typedef map< string, Meta_User* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_User* > external_aliases_lookup_container;
typedef external_aliases_lookup_container::const_iterator external_aliases_lookup_const_iterator;

external_aliases_container g_external_aliases;
external_aliases_lookup_container g_external_aliases_lookup;

bool gv_default_Active = bool( 1 );
string gv_default_Description = string( );
string gv_default_Email = string( );
string gv_default_Password = string( );
string gv_default_Permissions = string( );
string gv_default_User_Id = string( );
string gv_default_Workgroup = string( );

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_User > User_registration( get_class_registry( ), "100100" );

class Meta_User_command_functor;

class Meta_User_command_handler : public command_handler
{
   friend class Meta_User_command_functor;

   public:
   Meta_User_command_handler( ) : p_Meta_User( 0 ) { }

   void set_Meta_User( Meta_User* p_new_Meta_User ) { p_Meta_User = p_new_Meta_User; }

   void handle_unknown_command( const string& command )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_User* p_Meta_User;

   protected:
   string retval;
};

class Meta_User_command_functor : public command_functor
{
   public:
   Meta_User_command_functor( Meta_User_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_User_command_handler& cmd_handler;
};

command_functor* Meta_User_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_User_command_functor( dynamic_cast< Meta_User_command_handler& >( handler ) );
}

void Meta_User_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_User_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_parm_Meta_User_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_User->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_User->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_User_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_User->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_User->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_User_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_User_get_field_name ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );
      else if( field_name == c_field_id_Active || field_name == c_field_name_Active )
         string_getter< bool >( cmd_handler.p_Meta_User->Active( ), cmd_handler.retval );
      else if( field_name == c_field_id_Description || field_name == c_field_name_Description )
         string_getter< string >( cmd_handler.p_Meta_User->Description( ), cmd_handler.retval );
      else if( field_name == c_field_id_Email || field_name == c_field_name_Email )
         string_getter< string >( cmd_handler.p_Meta_User->Email( ), cmd_handler.retval );
      else if( field_name == c_field_id_Password || field_name == c_field_name_Password )
         string_getter< string >( cmd_handler.p_Meta_User->Password( ), cmd_handler.retval );
      else if( field_name == c_field_id_Permissions || field_name == c_field_name_Permissions )
         string_getter< string >( cmd_handler.p_Meta_User->Permissions( ), cmd_handler.retval );
      else if( field_name == c_field_id_User_Id || field_name == c_field_name_User_Id )
         string_getter< string >( cmd_handler.p_Meta_User->User_Id( ), cmd_handler.retval );
      else if( field_name == c_field_id_Workgroup || field_name == c_field_name_Workgroup )
         string_getter< Meta_Workgroup >( cmd_handler.p_Meta_User->Workgroup( ), cmd_handler.retval );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_User_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_User_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_parm_Meta_User_set_field_value ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );
      else if( field_name == c_field_id_Active || field_name == c_field_name_Active )
         func_string_setter< Meta_User, bool >(
          *cmd_handler.p_Meta_User, &Meta_User::Active, field_value );
      else if( field_name == c_field_id_Description || field_name == c_field_name_Description )
         func_string_setter< Meta_User, string >(
          *cmd_handler.p_Meta_User, &Meta_User::Description, field_value );
      else if( field_name == c_field_id_Email || field_name == c_field_name_Email )
         func_string_setter< Meta_User, string >(
          *cmd_handler.p_Meta_User, &Meta_User::Email, field_value );
      else if( field_name == c_field_id_Password || field_name == c_field_name_Password )
         func_string_setter< Meta_User, string >(
          *cmd_handler.p_Meta_User, &Meta_User::Password, field_value );
      else if( field_name == c_field_id_Permissions || field_name == c_field_name_Permissions )
         func_string_setter< Meta_User, string >(
          *cmd_handler.p_Meta_User, &Meta_User::Permissions, field_value );
      else if( field_name == c_field_id_User_Id || field_name == c_field_name_User_Id )
         func_string_setter< Meta_User, string >(
          *cmd_handler.p_Meta_User, &Meta_User::User_Id, field_value );
      else if( field_name == c_field_id_Workgroup || field_name == c_field_name_Workgroup )
         func_string_setter< Meta_User, Meta_Workgroup >(
          *cmd_handler.p_Meta_User, &Meta_User::Workgroup, field_value );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_User_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_User_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_parm_Meta_User_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( field_name == c_field_id_Workgroup || field_name == c_field_name_Workgroup )
         cmd_handler.retval = cmd_handler.p_Meta_User->Workgroup( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
}

struct Meta_User::impl : public Meta_User_command_handler
{
   impl( Meta_User& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_User( &o );

      add_commands( 0, Meta_User_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_User_command_definitions ) );
   }

   Meta_User& get_obj( ) const
   {
      return *cp_obj;
   }

   bool impl_Active( ) const { return lazy_fetch( p_obj ), v_Active; }
   void impl_Active( bool Active ) { v_Active = Active; }

   const string& impl_Description( ) const { return lazy_fetch( p_obj ), v_Description; }
   void impl_Description( const string& Description ) { v_Description = Description; }

   const string& impl_Email( ) const { return lazy_fetch( p_obj ), v_Email; }
   void impl_Email( const string& Email ) { v_Email = Email; }

   const string& impl_Password( ) const { return lazy_fetch( p_obj ), v_Password; }
   void impl_Password( const string& Password ) { v_Password = Password; }

   const string& impl_Permissions( ) const { return lazy_fetch( p_obj ), v_Permissions; }
   void impl_Permissions( const string& Permissions ) { v_Permissions = Permissions; }

   const string& impl_User_Id( ) const { return lazy_fetch( p_obj ), v_User_Id; }
   void impl_User_Id( const string& User_Id ) { v_User_Id = User_Id; }

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

   Meta_User* p_obj;
   class_pointer< Meta_User > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   bool v_Active;
   string v_Description;
   string v_Email;
   string v_Password;
   string v_Permissions;
   string v_User_Id;

   string v_Workgroup;
   mutable class_pointer< Meta_Workgroup > cp_Workgroup;
};

string Meta_User::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Active( ) );
      break;

      case 1:
      retval = to_string( impl_Description( ) );
      break;

      case 2:
      retval = to_string( impl_Email( ) );
      break;

      case 3:
      retval = to_string( impl_Password( ) );
      break;

      case 4:
      retval = to_string( impl_Permissions( ) );
      break;

      case 5:
      retval = to_string( impl_User_Id( ) );
      break;

      case 6:
      retval = to_string( impl_Workgroup( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }

   return retval;
}

void Meta_User::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_User::impl, bool >( *this, &Meta_User::impl::impl_Active, value );
      break;

      case 1:
      func_string_setter< Meta_User::impl, string >( *this, &Meta_User::impl::impl_Description, value );
      break;

      case 2:
      func_string_setter< Meta_User::impl, string >( *this, &Meta_User::impl::impl_Email, value );
      break;

      case 3:
      func_string_setter< Meta_User::impl, string >( *this, &Meta_User::impl::impl_Password, value );
      break;

      case 4:
      func_string_setter< Meta_User::impl, string >( *this, &Meta_User::impl::impl_Permissions, value );
      break;

      case 5:
      func_string_setter< Meta_User::impl, string >( *this, &Meta_User::impl::impl_User_Id, value );
      break;

      case 6:
      func_string_setter< Meta_User::impl, Meta_Workgroup >( *this, &Meta_User::impl::impl_Workgroup, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }
}

uint64_t Meta_User::impl::get_state( ) const
{
   uint64_t state = 0;

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

const string& Meta_User::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_User::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Workgroup || field == c_field_name_Workgroup )
      impl_Workgroup( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_User::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( field == c_field_id_Workgroup || field == c_field_name_Workgroup )
      v_Workgroup = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_User::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Workgroup || field == c_field_name_Workgroup )
      return v_Workgroup;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_User::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Workgroup, v_Workgroup ) );
}

void Meta_User::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_User::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_User::impl::clear( )
{
   v_Active = gv_default_Active;
   v_Description = gv_default_Description;
   v_Email = gv_default_Email;
   v_Password = gv_default_Password;
   v_Permissions = gv_default_Permissions;
   v_User_Id = gv_default_User_Id;

   v_Workgroup = string( );
   if( cp_Workgroup )
      p_obj->setup_foreign_key( *cp_Workgroup, v_Workgroup );
}

bool Meta_User::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_User::impl::validate( unsigned state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   if( is_null( v_Description ) && !value_will_be_provided( c_field_name_Description ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Description,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Description ) ) ) ) );

   if( is_null( v_Password ) && !value_will_be_provided( c_field_name_Password ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Password,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Password ) ) ) ) );

   if( is_null( v_User_Id ) && !value_will_be_provided( c_field_name_User_Id ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_User_Id,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_User_Id ) ) ) ) );

   string error_message;
   if( !is_null( v_Description )
    && ( v_Description != gv_default_Description
    || !value_will_be_provided( c_field_name_Description ) )
    && !g_Description_domain.is_valid( v_Description, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Description,
       get_module_string( c_field_display_name_Description ) + " " + error_message ) );

   if( !is_null( v_Password )
    && ( v_Password != gv_default_Password
    || !value_will_be_provided( c_field_name_Password ) )
    && !g_Password_domain.is_valid( v_Password, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Password,
       get_module_string( c_field_display_name_Password ) + " " + error_message ) );

   if( !is_null( v_User_Id )
    && ( v_User_Id != gv_default_User_Id
    || !value_will_be_provided( c_field_name_User_Id ) )
    && !g_User_Id_domain.is_valid( v_User_Id, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_User_Id,
       get_module_string( c_field_display_name_User_Id ) + " " + error_message ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_User::impl::validate_set_fields( set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;

   if( !is_null( v_Description )
    && ( fields_set.count( c_field_id_Description ) || fields_set.count( c_field_name_Description ) )
    && !g_Description_domain.is_valid( v_Description, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Description,
       get_module_string( c_field_display_name_Description ) + " " + error_message ) );

   if( !is_null( v_Password )
    && ( fields_set.count( c_field_id_Password ) || fields_set.count( c_field_name_Password ) )
    && !g_Password_domain.is_valid( v_Password, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Password,
       get_module_string( c_field_display_name_Password ) + " " + error_message ) );

   if( !is_null( v_User_Id )
    && ( fields_set.count( c_field_id_User_Id ) || fields_set.count( c_field_name_User_Id ) )
    && !g_User_Id_domain.is_valid( v_User_Id, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_User_Id,
       get_module_string( c_field_display_name_User_Id ) + " " + error_message ) );
}

void Meta_User::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Workgroup )
      p_obj->setup_foreign_key( *cp_Workgroup, v_Workgroup );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_User::impl::finalise_fetch( )
{
   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_User::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_User::impl::post_init( )
{
   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_User::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_clear)]
   get_obj( ).Permissions( string( ) );
   // [(finish field_clear)]

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_User::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_User::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_User::impl::can_destroy( bool is_internal )
{
   bool retval = is_internal || !( get_state( ) & c_state_undeletable );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_User::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_User::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_User::impl::set_default_values( )
{
   clear( );
}

bool Meta_User::impl::is_filtered( ) const
{
   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_User::impl::get_required_transients( ) const
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

Meta_User::Meta_User( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_User::~Meta_User( )
{
   cleanup( );
   delete p_impl;
}

bool Meta_User::Active( ) const
{
   return p_impl->impl_Active( );
}

void Meta_User::Active( bool Active )
{
   p_impl->impl_Active( Active );
}

const string& Meta_User::Description( ) const
{
   return p_impl->impl_Description( );
}

void Meta_User::Description( const string& Description )
{
   p_impl->impl_Description( Description );
}

const string& Meta_User::Email( ) const
{
   return p_impl->impl_Email( );
}

void Meta_User::Email( const string& Email )
{
   p_impl->impl_Email( Email );
}

const string& Meta_User::Password( ) const
{
   return p_impl->impl_Password( );
}

void Meta_User::Password( const string& Password )
{
   p_impl->impl_Password( Password );
}

const string& Meta_User::Permissions( ) const
{
   return p_impl->impl_Permissions( );
}

void Meta_User::Permissions( const string& Permissions )
{
   p_impl->impl_Permissions( Permissions );
}

const string& Meta_User::User_Id( ) const
{
   return p_impl->impl_User_Id( );
}

void Meta_User::User_Id( const string& User_Id )
{
   p_impl->impl_User_Id( User_Id );
}

Meta_Workgroup& Meta_User::Workgroup( )
{
   return p_impl->impl_Workgroup( );
}

const Meta_Workgroup& Meta_User::Workgroup( ) const
{
   return p_impl->impl_Workgroup( );
}

void Meta_User::Workgroup( const string& key )
{
   p_impl->impl_Workgroup( key );
}

string Meta_User::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_User::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

bool Meta_User::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_User::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_User::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_User::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_User::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_User::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_User::clear( )
{
   p_impl->clear( );
}

void Meta_User::validate( unsigned state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_User::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_User::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_User::finalise_fetch( )
{
   p_impl->finalise_fetch( );
}

void Meta_User::at_create( )
{
   p_impl->at_create( );
}

void Meta_User::post_init( )
{
   p_impl->post_init( );
}

void Meta_User::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_User::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_User::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_User::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_User::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_User::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_User::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_User::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_User::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( 0 );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Active )
   {
      p_id = c_field_id_Active;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Description )
   {
      p_id = c_field_id_Description;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Email )
   {
      p_id = c_field_id_Email;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Password )
   {
      p_id = c_field_id_Password;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Permissions )
   {
      p_id = c_field_id_Permissions;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_User_Id )
   {
      p_id = c_field_id_User_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
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

const char* Meta_User::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( 0 );

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Active )
   {
      p_name = c_field_name_Active;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Description )
   {
      p_name = c_field_name_Description;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Email )
   {
      p_name = c_field_name_Email;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Password )
   {
      p_name = c_field_name_Password;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Permissions )
   {
      p_name = c_field_name_Permissions;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_User_Id )
   {
      p_name = c_field_name_User_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
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

string& Meta_User::get_order_field_name( ) const
{
   return g_order_field_name;
}

bool Meta_User::is_file_field_name( const string& name ) const
{
   return g_file_field_names.count( name );
}

void Meta_User::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_User::get_field_uom_symbol( const string& id_or_name ) const
{
   string uom_symbol;

   string name;
   pair< string, string > next;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_uom_symbol" );
   else if( id_or_name == c_field_id_Active || id_or_name == c_field_name_Active )
   {
      name = string( c_field_display_name_Active );
      get_module_string( c_field_display_name_Active, &next );
   }
   else if( id_or_name == c_field_id_Description || id_or_name == c_field_name_Description )
   {
      name = string( c_field_display_name_Description );
      get_module_string( c_field_display_name_Description, &next );
   }
   else if( id_or_name == c_field_id_Email || id_or_name == c_field_name_Email )
   {
      name = string( c_field_display_name_Email );
      get_module_string( c_field_display_name_Email, &next );
   }
   else if( id_or_name == c_field_id_Password || id_or_name == c_field_name_Password )
   {
      name = string( c_field_display_name_Password );
      get_module_string( c_field_display_name_Password, &next );
   }
   else if( id_or_name == c_field_id_Permissions || id_or_name == c_field_name_Permissions )
   {
      name = string( c_field_display_name_Permissions );
      get_module_string( c_field_display_name_Permissions, &next );
   }
   else if( id_or_name == c_field_id_User_Id || id_or_name == c_field_name_User_Id )
   {
      name = string( c_field_display_name_User_Id );
      get_module_string( c_field_display_name_User_Id, &next );
   }
   else if( id_or_name == c_field_id_Workgroup || id_or_name == c_field_name_Workgroup )
   {
      name = string( c_field_display_name_Workgroup );
      get_module_string( c_field_display_name_Workgroup, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_User::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   else if( id_or_name == c_field_id_Active || id_or_name == c_field_name_Active )
      display_name = get_module_string( c_field_display_name_Active );
   else if( id_or_name == c_field_id_Description || id_or_name == c_field_name_Description )
      display_name = get_module_string( c_field_display_name_Description );
   else if( id_or_name == c_field_id_Email || id_or_name == c_field_name_Email )
      display_name = get_module_string( c_field_display_name_Email );
   else if( id_or_name == c_field_id_Password || id_or_name == c_field_name_Password )
      display_name = get_module_string( c_field_display_name_Password );
   else if( id_or_name == c_field_id_Permissions || id_or_name == c_field_name_Permissions )
      display_name = get_module_string( c_field_display_name_Permissions );
   else if( id_or_name == c_field_id_User_Id || id_or_name == c_field_name_User_Id )
      display_name = get_module_string( c_field_display_name_User_Id );
   else if( id_or_name == c_field_id_Workgroup || id_or_name == c_field_name_Workgroup )
      display_name = get_module_string( c_field_display_name_Workgroup );

   return display_name;
}

void Meta_User::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_User::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_User::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_User::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_User::setup_foreign_key( Meta_Workgroup& o, const string& value )
{
   static_cast< Meta_Workgroup& >( o ).set_key( value );
}

void Meta_User::setup_graph_parent(
 Meta_Workgroup& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Workgroup& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Workgroup& >( o ).set_key( init_value );
}

size_t Meta_User::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_User::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_User::get_num_foreign_key_children( bool is_internal ) const
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

class_base* Meta_User::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   ( void )child_num;
   ( void )next_child_field;
   ( void )op;

   return p_class_base;
}

void Meta_User::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_User::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_User::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_User::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_User::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_User::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_User::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_User::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );
   key += "user";

   return get_module_string( key );
}

bool Meta_User::get_is_alias( ) const
{
   return false;
}

void Meta_User::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_User::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_User::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( sub_context == c_field_id_Workgroup || sub_context == c_field_name_Workgroup )
      p_class_base = &Workgroup( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_User::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Active" );
   names.push_back( "C_Description" );
   names.push_back( "C_Email" );
   names.push_back( "C_Password" );
   names.push_back( "C_Permissions" );
   names.push_back( "C_User_Id" );
   names.push_back( "C_Workgroup" );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_User::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( to_string( Active( ) ) );
   values.push_back( sql_quote( to_string( Description( ) ) ) );
   values.push_back( sql_quote( to_string( Email( ) ) ) );
   values.push_back( sql_quote( to_string( Password( ) ) ) );
   values.push_back( sql_quote( to_string( Permissions( ) ) ) );
   values.push_back( sql_quote( to_string( User_Id( ) ) ) );
   values.push_back( sql_quote( to_string( Workgroup( ) ) ) );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_User::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_User::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_User::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_User::do_generate_sql(
 generate_sql_type type, vector< string >& sql_stmts, set< string >& tx_key_info ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info );
}

const char* Meta_User::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_User::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_User::static_lock_class_id( )
{
   return "100100";
}

const char* Meta_User::static_check_class_name( )
{
   return "User";
}

bool Meta_User::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_User::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.100100" );
}

void Meta_User::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "100102", "Active", "bool", false ) );
   all_field_info.push_back( field_info( "100104", "Description", "string", false ) );
   all_field_info.push_back( field_info( "100105", "Email", "string", false ) );
   all_field_info.push_back( field_info( "100103", "Password", "string", false ) );
   all_field_info.push_back( field_info( "100106", "Permissions", "string", false ) );
   all_field_info.push_back( field_info( "100101", "User_Id", "string", false ) );
   all_field_info.push_back( field_info( "300100", "Workgroup", "Meta_Workgroup", false ) );
}

void Meta_User::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Workgroup, make_pair( "Meta.100100", "Meta_Workgroup" ) ) );
}

int Meta_User::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_User::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_User::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "100102";
      break;

      case 2:
      p_id = "100104";
      break;

      case 3:
      p_id = "100105";
      break;

      case 4:
      p_id = "100103";
      break;

      case 5:
      p_id = "100106";
      break;

      case 6:
      p_id = "100101";
      break;

      case 7:
      p_id = "300100";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class User" );

   return p_id;
}

const char* Meta_User::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Active";
      break;

      case 2:
      p_id = "Description";
      break;

      case 3:
      p_id = "Email";
      break;

      case 4:
      p_id = "Password";
      break;

      case 5:
      p_id = "Permissions";
      break;

      case 6:
      p_id = "User_Id";
      break;

      case 7:
      p_id = "Workgroup";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class User" );

   return p_id;
}

int Meta_User::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Active || field == c_field_name_Active )
      rc += 1;
   else if( field == c_field_id_Description || field == c_field_name_Description )
      rc += 2;
   else if( field == c_field_id_Email || field == c_field_name_Email )
      rc += 3;
   else if( field == c_field_id_Password || field == c_field_name_Password )
      rc += 4;
   else if( field == c_field_id_Permissions || field == c_field_name_Permissions )
      rc += 5;
   else if( field == c_field_id_User_Id || field == c_field_name_User_Id )
      rc += 6;
   else if( field == c_field_id_Workgroup || field == c_field_name_Workgroup )
      rc += 7;

   return rc - 1;
}

procedure_info_container& Meta_User::static_get_procedure_info( )
{
   static procedure_info_container procedures;

   return procedures;
}

string Meta_User::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(64),"
    "C_Ver_ INTEGER NOT NULL,"
    "C_Rev_ INTEGER NOT NULL,"
    "C_Typ_ VARCHAR(24) NOT NULL,"
    "C_Active INTEGER NOT NULL,"
    "C_Description VARCHAR(200) NOT NULL,"
    "C_Email VARCHAR(200) NOT NULL,"
    "C_Password VARCHAR(200) NOT NULL,"
    "C_Permissions VARCHAR(200) NOT NULL,"
    "C_User_Id VARCHAR(200) NOT NULL,"
    "C_Workgroup VARCHAR(64) NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_User::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_User::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   ( void )pairs;
}

void Meta_User::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Active,C_Description,C_Key_" );
   indexes.push_back( "C_Active,C_User_Id" );
   indexes.push_back( "C_Description,C_Key_" );
   indexes.push_back( "C_User_Id" );
   indexes.push_back( "C_Workgroup,C_Active,C_Description,C_Key_" );
   indexes.push_back( "C_Workgroup,C_Active,C_User_Id" );
   indexes.push_back( "C_Workgroup,C_Description,C_Key_" );
   indexes.push_back( "C_Workgroup,C_User_Id" );
}

void Meta_User::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Active,C_User_Id" );
   indexes.push_back( "C_User_Id" );
   indexes.push_back( "C_Workgroup,C_Active,C_User_Id" );
   indexes.push_back( "C_Workgroup,C_User_Id" );
}

void Meta_User::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_User::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_User::static_insert_external_alias( const string& module_and_class_id, Meta_User* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_User::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_User::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_User::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}

