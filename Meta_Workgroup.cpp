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

#include "Meta_Workgroup.h"

#include "Meta_Application.h"
#include "Meta_Enum.h"
#include "Meta_Model.h"
#include "Meta_Permission.h"
#include "Meta_Type.h"
#include "Meta_User.h"
#include "Meta_Package.h"

#include "ciyam_base.h"
#include "class_domains.h"
#include "module_strings.h"
#include "class_utilities.h"
#include "command_handler.h"
#include "module_interface.h"

// [(start for_auto_code_user)]
#include "Meta_Auto_Code.h"
// [(finish for_auto_code_user)]

// [<start includes>]
// [<finish includes>]

using namespace std;

// [<start namespaces>]
// [<finish namespaces>]

template< > inline string to_string( const Meta_Package& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Package& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline int system( const string& cmd ) { return exec_system( cmd ); }

namespace
{

#include "Meta_Workgroup.cmh"

const int32_t c_version = 1;

const char* const c_okay = "okay";

const char* const c_field_id_Id = "101102";
const char* const c_field_id_Name = "101101";
const char* const c_field_id_Next_Enum_Id = "101104";
const char* const c_field_id_Next_Model_Id = "101103";
const char* const c_field_id_Next_Permission_Id = "101106";
const char* const c_field_id_Next_Type_Id = "101105";
const char* const c_field_id_Standard_Package = "302835";

const char* const c_field_name_Id = "Id";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Next_Enum_Id = "Next_Enum_Id";
const char* const c_field_name_Next_Model_Id = "Next_Model_Id";
const char* const c_field_name_Next_Permission_Id = "Next_Permission_Id";
const char* const c_field_name_Next_Type_Id = "Next_Type_Id";
const char* const c_field_name_Standard_Package = "Standard_Package";

const char* const c_field_display_name_Id = "field_workgroup_id";
const char* const c_field_display_name_Name = "field_workgroup_name";
const char* const c_field_display_name_Next_Enum_Id = "field_workgroup_next_enum_id";
const char* const c_field_display_name_Next_Model_Id = "field_workgroup_next_model_id";
const char* const c_field_display_name_Next_Permission_Id = "field_workgroup_next_permission_id";
const char* const c_field_display_name_Next_Type_Id = "field_workgroup_next_type_id";
const char* const c_field_display_name_Standard_Package = "field_workgroup_standard_package";

const int c_num_fields = 7;

const char* const c_all_sorted_field_ids[ ] =
{
   "101101",
   "101102",
   "101103",
   "101104",
   "101105",
   "101106",
   "302835"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Id",
   "Name",
   "Next_Enum_Id",
   "Next_Model_Id",
   "Next_Permission_Id",
   "Next_Type_Id",
   "Standard_Package"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool has_field( const string& field )
{
   return binary_search( c_all_sorted_field_ids, c_all_sorted_field_ids + c_num_fields, field.c_str( ), compare )
    || binary_search( c_all_sorted_field_names, c_all_sorted_field_names + c_num_fields, field.c_str( ), compare );
}

const int c_num_transient_fields = 0;

bool is_transient_field( const string& ) { static bool false_value( false ); return false_value; }

aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Id_domain;
domain_string_max_size< 100 > g_Name_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Next_Enum_Id_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Next_Model_Id_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Next_Permission_Id_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Next_Type_Id_domain;

set< string > g_derivations;

typedef map< string, Meta_Workgroup* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Workgroup* > external_aliases_lookup_container;
typedef external_aliases_lookup_container::const_iterator external_aliases_lookup_const_iterator;

external_aliases_container g_external_aliases;
external_aliases_lookup_container g_external_aliases_lookup;

string gv_default_Id = string( );
string gv_default_Name = string( );
string gv_default_Next_Enum_Id = string( "E100" );
string gv_default_Next_Model_Id = string( "M100" );
string gv_default_Next_Permission_Id = string( "P100" );
string gv_default_Next_Type_Id = string( "T100" );

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Workgroup > Workgroup_registration( get_class_registry( ), "101100" );

class Meta_Workgroup_command_functor;

class Meta_Workgroup_command_handler : public command_handler
{
   friend class Meta_Workgroup_command_functor;

   public:
   Meta_Workgroup_command_handler( ) : p_Meta_Workgroup( 0 ) { }

   void set_Meta_Workgroup( Meta_Workgroup* p_new_Meta_Workgroup ) { p_Meta_Workgroup = p_new_Meta_Workgroup; }

   void handle_unknown_command( const string& command )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Workgroup* p_Meta_Workgroup;

   protected:
   string retval;
};

class Meta_Workgroup_command_functor : public command_functor
{
   public:
   Meta_Workgroup_command_functor( Meta_Workgroup_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Workgroup_command_handler& cmd_handler;
};

command_functor* Meta_Workgroup_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Workgroup_command_functor( dynamic_cast< Meta_Workgroup_command_handler& >( handler ) );
}

void Meta_Workgroup_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Workgroup_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_parm_Meta_Workgroup_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Workgroup->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Workgroup->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Workgroup_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Workgroup->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Workgroup->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Workgroup_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Workgroup_get_field_name ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );
      else if( field_name == c_field_id_Id || field_name == c_field_name_Id )
         string_getter< string >( cmd_handler.p_Meta_Workgroup->Id( ), cmd_handler.retval );
      else if( field_name == c_field_id_Name || field_name == c_field_name_Name )
         string_getter< string >( cmd_handler.p_Meta_Workgroup->Name( ), cmd_handler.retval );
      else if( field_name == c_field_id_Next_Enum_Id || field_name == c_field_name_Next_Enum_Id )
         string_getter< string >( cmd_handler.p_Meta_Workgroup->Next_Enum_Id( ), cmd_handler.retval );
      else if( field_name == c_field_id_Next_Model_Id || field_name == c_field_name_Next_Model_Id )
         string_getter< string >( cmd_handler.p_Meta_Workgroup->Next_Model_Id( ), cmd_handler.retval );
      else if( field_name == c_field_id_Next_Permission_Id || field_name == c_field_name_Next_Permission_Id )
         string_getter< string >( cmd_handler.p_Meta_Workgroup->Next_Permission_Id( ), cmd_handler.retval );
      else if( field_name == c_field_id_Next_Type_Id || field_name == c_field_name_Next_Type_Id )
         string_getter< string >( cmd_handler.p_Meta_Workgroup->Next_Type_Id( ), cmd_handler.retval );
      else if( field_name == c_field_id_Standard_Package || field_name == c_field_name_Standard_Package )
         string_getter< Meta_Package >( cmd_handler.p_Meta_Workgroup->Standard_Package( ), cmd_handler.retval );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Workgroup_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Workgroup_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_parm_Meta_Workgroup_set_field_value ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );
      else if( field_name == c_field_id_Id || field_name == c_field_name_Id )
         func_string_setter< Meta_Workgroup, string >(
          *cmd_handler.p_Meta_Workgroup, &Meta_Workgroup::Id, field_value );
      else if( field_name == c_field_id_Name || field_name == c_field_name_Name )
         func_string_setter< Meta_Workgroup, string >(
          *cmd_handler.p_Meta_Workgroup, &Meta_Workgroup::Name, field_value );
      else if( field_name == c_field_id_Next_Enum_Id || field_name == c_field_name_Next_Enum_Id )
         func_string_setter< Meta_Workgroup, string >(
          *cmd_handler.p_Meta_Workgroup, &Meta_Workgroup::Next_Enum_Id, field_value );
      else if( field_name == c_field_id_Next_Model_Id || field_name == c_field_name_Next_Model_Id )
         func_string_setter< Meta_Workgroup, string >(
          *cmd_handler.p_Meta_Workgroup, &Meta_Workgroup::Next_Model_Id, field_value );
      else if( field_name == c_field_id_Next_Permission_Id || field_name == c_field_name_Next_Permission_Id )
         func_string_setter< Meta_Workgroup, string >(
          *cmd_handler.p_Meta_Workgroup, &Meta_Workgroup::Next_Permission_Id, field_value );
      else if( field_name == c_field_id_Next_Type_Id || field_name == c_field_name_Next_Type_Id )
         func_string_setter< Meta_Workgroup, string >(
          *cmd_handler.p_Meta_Workgroup, &Meta_Workgroup::Next_Type_Id, field_value );
      else if( field_name == c_field_id_Standard_Package || field_name == c_field_name_Standard_Package )
         func_string_setter< Meta_Workgroup, Meta_Package >(
          *cmd_handler.p_Meta_Workgroup, &Meta_Workgroup::Standard_Package, field_value );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Workgroup_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Workgroup_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_parm_Meta_Workgroup_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( field_name == c_field_id_Standard_Package || field_name == c_field_name_Standard_Package )
         cmd_handler.retval = cmd_handler.p_Meta_Workgroup->Standard_Package( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
}

struct Meta_Workgroup::impl : public Meta_Workgroup_command_handler
{
   impl( Meta_Workgroup& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Workgroup( &o );

      add_commands( 0, Meta_Workgroup_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Workgroup_command_definitions ) );
   }

   Meta_Workgroup& get_obj( ) const
   {
      return *cp_obj;
   }

   const string& impl_Id( ) const { return lazy_fetch( p_obj ), v_Id; }
   void impl_Id( const string& Id ) { v_Id = Id; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { v_Name = Name; }

   const string& impl_Next_Enum_Id( ) const { return lazy_fetch( p_obj ), v_Next_Enum_Id; }
   void impl_Next_Enum_Id( const string& Next_Enum_Id ) { v_Next_Enum_Id = Next_Enum_Id; }

   const string& impl_Next_Model_Id( ) const { return lazy_fetch( p_obj ), v_Next_Model_Id; }
   void impl_Next_Model_Id( const string& Next_Model_Id ) { v_Next_Model_Id = Next_Model_Id; }

   const string& impl_Next_Permission_Id( ) const { return lazy_fetch( p_obj ), v_Next_Permission_Id; }
   void impl_Next_Permission_Id( const string& Next_Permission_Id ) { v_Next_Permission_Id = Next_Permission_Id; }

   const string& impl_Next_Type_Id( ) const { return lazy_fetch( p_obj ), v_Next_Type_Id; }
   void impl_Next_Type_Id( const string& Next_Type_Id ) { v_Next_Type_Id = Next_Type_Id; }

   Meta_Package& impl_Standard_Package( )
   {
      if( !cp_Standard_Package )
      {
         cp_Standard_Package.init( );

         p_obj->setup_graph_parent( *cp_Standard_Package, c_field_id_Standard_Package, v_Standard_Package );
      }
      return *cp_Standard_Package;
   }

   const Meta_Package& impl_Standard_Package( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Standard_Package )
      {
         cp_Standard_Package.init( );

         p_obj->setup_graph_parent( *cp_Standard_Package, c_field_id_Standard_Package, v_Standard_Package );
      }
      return *cp_Standard_Package;
   }

   void impl_Standard_Package( const string& key )
   {
      class_base_accessor cba( impl_Standard_Package( ) );
      cba.set_key( key );
   }

   Meta_Application& impl_child_Application( )
   {
      if( !cp_child_Application )
      {
         cp_child_Application.init( );

         p_obj->setup_graph_parent( *cp_child_Application, "302220" );
      }
      return *cp_child_Application;
   }

   const Meta_Application& impl_child_Application( ) const
   {
      if( !cp_child_Application )
      {
         cp_child_Application.init( );

         p_obj->setup_graph_parent( *cp_child_Application, "302220" );
      }
      return *cp_child_Application;
   }

   Meta_Enum& impl_child_Enum( )
   {
      if( !cp_child_Enum )
      {
         cp_child_Enum.init( );

         p_obj->setup_graph_parent( *cp_child_Enum, "300300" );
      }
      return *cp_child_Enum;
   }

   const Meta_Enum& impl_child_Enum( ) const
   {
      if( !cp_child_Enum )
      {
         cp_child_Enum.init( );

         p_obj->setup_graph_parent( *cp_child_Enum, "300300" );
      }
      return *cp_child_Enum;
   }

   Meta_Model& impl_child_Model( )
   {
      if( !cp_child_Model )
      {
         cp_child_Model.init( );

         p_obj->setup_graph_parent( *cp_child_Model, "300500" );
      }
      return *cp_child_Model;
   }

   const Meta_Model& impl_child_Model( ) const
   {
      if( !cp_child_Model )
      {
         cp_child_Model.init( );

         p_obj->setup_graph_parent( *cp_child_Model, "300500" );
      }
      return *cp_child_Model;
   }

   Meta_Permission& impl_child_Permission( )
   {
      if( !cp_child_Permission )
      {
         cp_child_Permission.init( );

         p_obj->setup_graph_parent( *cp_child_Permission, "301500" );
      }
      return *cp_child_Permission;
   }

   const Meta_Permission& impl_child_Permission( ) const
   {
      if( !cp_child_Permission )
      {
         cp_child_Permission.init( );

         p_obj->setup_graph_parent( *cp_child_Permission, "301500" );
      }
      return *cp_child_Permission;
   }

   Meta_Type& impl_child_Type( )
   {
      if( !cp_child_Type )
      {
         cp_child_Type.init( );

         p_obj->setup_graph_parent( *cp_child_Type, "300200" );
      }
      return *cp_child_Type;
   }

   const Meta_Type& impl_child_Type( ) const
   {
      if( !cp_child_Type )
      {
         cp_child_Type.init( );

         p_obj->setup_graph_parent( *cp_child_Type, "300200" );
      }
      return *cp_child_Type;
   }

   Meta_User& impl_child_User( )
   {
      if( !cp_child_User )
      {
         cp_child_User.init( );

         p_obj->setup_graph_parent( *cp_child_User, "300100" );
      }
      return *cp_child_User;
   }

   const Meta_User& impl_child_User( ) const
   {
      if( !cp_child_User )
      {
         cp_child_User.init( );

         p_obj->setup_graph_parent( *cp_child_User, "300100" );
      }
      return *cp_child_User;
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

   bool is_filtered( ) const;

   Meta_Workgroup* p_obj;
   class_pointer< Meta_Workgroup > cp_obj;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   string v_Id;
   string v_Name;
   string v_Next_Enum_Id;
   string v_Next_Model_Id;
   string v_Next_Permission_Id;
   string v_Next_Type_Id;

   string v_Standard_Package;
   mutable class_pointer< Meta_Package > cp_Standard_Package;

   mutable class_pointer< Meta_Application > cp_child_Application;
   mutable class_pointer< Meta_Enum > cp_child_Enum;
   mutable class_pointer< Meta_Model > cp_child_Model;
   mutable class_pointer< Meta_Permission > cp_child_Permission;
   mutable class_pointer< Meta_Type > cp_child_Type;
   mutable class_pointer< Meta_User > cp_child_User;
};

string Meta_Workgroup::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Id( ) );
      break;

      case 1:
      retval = to_string( impl_Name( ) );
      break;

      case 2:
      retval = to_string( impl_Next_Enum_Id( ) );
      break;

      case 3:
      retval = to_string( impl_Next_Model_Id( ) );
      break;

      case 4:
      retval = to_string( impl_Next_Permission_Id( ) );
      break;

      case 5:
      retval = to_string( impl_Next_Type_Id( ) );
      break;

      case 6:
      retval = to_string( impl_Standard_Package( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }

   return retval;
}

void Meta_Workgroup::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Workgroup::impl, string >( *this, &Meta_Workgroup::impl::impl_Id, value );
      break;

      case 1:
      func_string_setter< Meta_Workgroup::impl, string >( *this, &Meta_Workgroup::impl::impl_Name, value );
      break;

      case 2:
      func_string_setter< Meta_Workgroup::impl, string >( *this, &Meta_Workgroup::impl::impl_Next_Enum_Id, value );
      break;

      case 3:
      func_string_setter< Meta_Workgroup::impl, string >( *this, &Meta_Workgroup::impl::impl_Next_Model_Id, value );
      break;

      case 4:
      func_string_setter< Meta_Workgroup::impl, string >( *this, &Meta_Workgroup::impl::impl_Next_Permission_Id, value );
      break;

      case 5:
      func_string_setter< Meta_Workgroup::impl, string >( *this, &Meta_Workgroup::impl::impl_Next_Type_Id, value );
      break;

      case 6:
      func_string_setter< Meta_Workgroup::impl, Meta_Package >( *this, &Meta_Workgroup::impl::impl_Standard_Package, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }
}

uint64_t Meta_Workgroup::impl::get_state( ) const
{
   uint64_t state = 0;

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

const string& Meta_Workgroup::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Workgroup::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Standard_Package || field == c_field_name_Standard_Package )
      impl_Standard_Package( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Workgroup::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( field == c_field_id_Standard_Package || field == c_field_name_Standard_Package )
      v_Standard_Package = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Workgroup::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Standard_Package || field == c_field_name_Standard_Package )
      return v_Standard_Package;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Workgroup::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Standard_Package, v_Standard_Package ) );
}

void Meta_Workgroup::impl::clear( )
{
   v_Id = gv_default_Id;
   v_Name = gv_default_Name;
   v_Next_Enum_Id = gv_default_Next_Enum_Id;
   v_Next_Model_Id = gv_default_Next_Model_Id;
   v_Next_Permission_Id = gv_default_Next_Permission_Id;
   v_Next_Type_Id = gv_default_Next_Type_Id;

   v_Standard_Package = string( );
   if( cp_Standard_Package )
      p_obj->setup_foreign_key( *cp_Standard_Package, v_Standard_Package );
}

bool Meta_Workgroup::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [(start for_auto_code_user)]
   if( field_name == "Id" )
      return true;
   // [(finish for_auto_code_user)]

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Workgroup::impl::validate( unsigned state, bool is_internal, validation_error_container* p_validation_errors )
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

   if( is_null( v_Next_Enum_Id ) && !value_will_be_provided( c_field_name_Next_Enum_Id ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Next_Enum_Id,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Next_Enum_Id ) ) ) ) );

   if( is_null( v_Next_Model_Id ) && !value_will_be_provided( c_field_name_Next_Model_Id ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Next_Model_Id,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Next_Model_Id ) ) ) ) );

   if( is_null( v_Next_Permission_Id ) && !value_will_be_provided( c_field_name_Next_Permission_Id ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Next_Permission_Id,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Next_Permission_Id ) ) ) ) );

   if( is_null( v_Next_Type_Id ) && !value_will_be_provided( c_field_name_Next_Type_Id ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Next_Type_Id,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Next_Type_Id ) ) ) ) );

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

   if( !is_null( v_Next_Enum_Id )
    && ( v_Next_Enum_Id != gv_default_Next_Enum_Id
    || !value_will_be_provided( c_field_name_Next_Enum_Id ) )
    && !g_Next_Enum_Id_domain.is_valid( v_Next_Enum_Id, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Next_Enum_Id,
       get_module_string( c_field_display_name_Next_Enum_Id ) + " " + error_message ) );

   if( !is_null( v_Next_Model_Id )
    && ( v_Next_Model_Id != gv_default_Next_Model_Id
    || !value_will_be_provided( c_field_name_Next_Model_Id ) )
    && !g_Next_Model_Id_domain.is_valid( v_Next_Model_Id, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Next_Model_Id,
       get_module_string( c_field_display_name_Next_Model_Id ) + " " + error_message ) );

   if( !is_null( v_Next_Permission_Id )
    && ( v_Next_Permission_Id != gv_default_Next_Permission_Id
    || !value_will_be_provided( c_field_name_Next_Permission_Id ) )
    && !g_Next_Permission_Id_domain.is_valid( v_Next_Permission_Id, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Next_Permission_Id,
       get_module_string( c_field_display_name_Next_Permission_Id ) + " " + error_message ) );

   if( !is_null( v_Next_Type_Id )
    && ( v_Next_Type_Id != gv_default_Next_Type_Id
    || !value_will_be_provided( c_field_name_Next_Type_Id ) )
    && !g_Next_Type_Id_domain.is_valid( v_Next_Type_Id, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Next_Type_Id,
       get_module_string( c_field_display_name_Next_Type_Id ) + " " + error_message ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Workgroup::impl::after_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   if( cp_Standard_Package )
      p_obj->setup_foreign_key( *cp_Standard_Package, v_Standard_Package );

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Workgroup::impl::finalise_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Workgroup::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Workgroup::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Workgroup::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [(start for_auto_code_user)]
   if( is_create && is_null( get_obj( ).Id( ) ) )
   {
      class_pointer< Meta_Auto_Code > cp_auto_code( e_create_instance );

      string key( get_obj( ).lock_class_id( ) );
      key += "X" + to_string( c_field_id_Id );

      cp_auto_code->set_instance( key );

      string next_value;
      cp_auto_code->Increment( next_value );
      if( next_value.empty( ) ) // FUTURE: The error message itself should be a module string.
         throw runtime_error( "Unable to automatically generate value for Id" );

      get_obj( ).Id( next_value );
   }
   // [(finish for_auto_code_user)]

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Workgroup::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [(start clone_default_group_children)]
   if( is_create && get_obj( ).get_clone_key( ).empty( ) )
   {
      class_pointer< Meta_Type > cp_child( e_create_instance );
      class_pointer< Meta_Workgroup > cp_source( e_create_instance );

      if( cp_source->child_Type( ).iterate_forwards( ) )
      {
         do
         {
            cp_child->op_create( get_obj( ).get_key( )
             + "_" + cp_source->child_Type( ).get_key( )
             + " " + cp_source->child_Type( ).get_key( ) );

            cp_child->Workgroup( get_obj( ).get_key( ) );

            cp_child->op_apply( );
         } while( cp_source->child_Type( ).iterate_next( ) );
      }
   }
   // [(finish clone_default_group_children)]

   // [(start clone_default_group_children)]
   if( is_create && get_obj( ).get_clone_key( ).empty( ) )
   {
      class_pointer< Meta_Enum > cp_child( e_create_instance );
      class_pointer< Meta_Workgroup > cp_source( e_create_instance );

      if( cp_source->child_Enum( ).iterate_forwards( ) )
      {
         do
         {
            cp_child->op_create( get_obj( ).get_key( )
             + "_" + cp_source->child_Enum( ).get_key( )
             + " " + cp_source->child_Enum( ).get_key( ) );

            cp_child->Workgroup( get_obj( ).get_key( ) );

            cp_child->op_apply( );
         } while( cp_source->child_Enum( ).iterate_next( ) );
      }
   }
   // [(finish clone_default_group_children)]

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Workgroup::impl::can_destroy( bool is_internal )
{
   bool retval = is_internal || !( get_state( ) & c_state_undeletable );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Workgroup::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Workgroup::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Workgroup::impl::set_default_values( )
{
   clear( );
}

bool Meta_Workgroup::impl::is_filtered( ) const
{
   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

#undef MODULE_TRACE
#define MODULE_TRACE( x ) trace( x )

Meta_Workgroup::Meta_Workgroup( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Workgroup::~Meta_Workgroup( )
{
   cleanup( );
   delete p_impl;
}

const string& Meta_Workgroup::Id( ) const
{
   return p_impl->impl_Id( );
}

void Meta_Workgroup::Id( const string& Id )
{
   p_impl->impl_Id( Id );
}

const string& Meta_Workgroup::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_Workgroup::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

const string& Meta_Workgroup::Next_Enum_Id( ) const
{
   return p_impl->impl_Next_Enum_Id( );
}

void Meta_Workgroup::Next_Enum_Id( const string& Next_Enum_Id )
{
   p_impl->impl_Next_Enum_Id( Next_Enum_Id );
}

const string& Meta_Workgroup::Next_Model_Id( ) const
{
   return p_impl->impl_Next_Model_Id( );
}

void Meta_Workgroup::Next_Model_Id( const string& Next_Model_Id )
{
   p_impl->impl_Next_Model_Id( Next_Model_Id );
}

const string& Meta_Workgroup::Next_Permission_Id( ) const
{
   return p_impl->impl_Next_Permission_Id( );
}

void Meta_Workgroup::Next_Permission_Id( const string& Next_Permission_Id )
{
   p_impl->impl_Next_Permission_Id( Next_Permission_Id );
}

const string& Meta_Workgroup::Next_Type_Id( ) const
{
   return p_impl->impl_Next_Type_Id( );
}

void Meta_Workgroup::Next_Type_Id( const string& Next_Type_Id )
{
   p_impl->impl_Next_Type_Id( Next_Type_Id );
}

Meta_Package& Meta_Workgroup::Standard_Package( )
{
   return p_impl->impl_Standard_Package( );
}

const Meta_Package& Meta_Workgroup::Standard_Package( ) const
{
   return p_impl->impl_Standard_Package( );
}

void Meta_Workgroup::Standard_Package( const string& key )
{
   p_impl->impl_Standard_Package( key );
}

Meta_Application& Meta_Workgroup::child_Application( )
{
   return p_impl->impl_child_Application( );
}

const Meta_Application& Meta_Workgroup::child_Application( ) const
{
   return p_impl->impl_child_Application( );
}

Meta_Enum& Meta_Workgroup::child_Enum( )
{
   return p_impl->impl_child_Enum( );
}

const Meta_Enum& Meta_Workgroup::child_Enum( ) const
{
   return p_impl->impl_child_Enum( );
}

Meta_Model& Meta_Workgroup::child_Model( )
{
   return p_impl->impl_child_Model( );
}

const Meta_Model& Meta_Workgroup::child_Model( ) const
{
   return p_impl->impl_child_Model( );
}

Meta_Permission& Meta_Workgroup::child_Permission( )
{
   return p_impl->impl_child_Permission( );
}

const Meta_Permission& Meta_Workgroup::child_Permission( ) const
{
   return p_impl->impl_child_Permission( );
}

Meta_Type& Meta_Workgroup::child_Type( )
{
   return p_impl->impl_child_Type( );
}

const Meta_Type& Meta_Workgroup::child_Type( ) const
{
   return p_impl->impl_child_Type( );
}

Meta_User& Meta_Workgroup::child_User( )
{
   return p_impl->impl_child_User( );
}

const Meta_User& Meta_Workgroup::child_User( ) const
{
   return p_impl->impl_child_User( );
}

string Meta_Workgroup::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Workgroup::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

bool Meta_Workgroup::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Workgroup::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Workgroup::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Workgroup::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Workgroup::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Workgroup::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Workgroup::clear( )
{
   p_impl->clear( );
}

void Meta_Workgroup::validate( unsigned state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Workgroup::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Workgroup::finalise_fetch( )
{
   p_impl->finalise_fetch( );
}

void Meta_Workgroup::at_create( )
{
   p_impl->at_create( );
}

void Meta_Workgroup::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Workgroup::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Workgroup::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Workgroup::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Workgroup::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Workgroup::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Workgroup::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Workgroup::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Workgroup::get_field_id(
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
   else if( name == c_field_name_Name )
   {
      p_id = c_field_id_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Next_Enum_Id )
   {
      p_id = c_field_id_Next_Enum_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Next_Model_Id )
   {
      p_id = c_field_id_Next_Model_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Next_Permission_Id )
   {
      p_id = c_field_id_Next_Permission_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Next_Type_Id )
   {
      p_id = c_field_id_Next_Type_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Standard_Package )
   {
      p_id = c_field_id_Standard_Package;

      if( p_type_name )
         *p_type_name = "Meta_Package";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_id;
}

const char* Meta_Workgroup::get_field_name(
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
   else if( id == c_field_id_Name )
   {
      p_name = c_field_name_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Next_Enum_Id )
   {
      p_name = c_field_name_Next_Enum_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Next_Model_Id )
   {
      p_name = c_field_name_Next_Model_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Next_Permission_Id )
   {
      p_name = c_field_name_Next_Permission_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Next_Type_Id )
   {
      p_name = c_field_name_Next_Type_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Standard_Package )
   {
      p_name = c_field_name_Standard_Package;

      if( p_type_name )
         *p_type_name = "Meta_Package";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_name;
}

string Meta_Workgroup::get_field_display_name( const string& id ) const
{
   string display_name;

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_display_name" );
   else if( id == c_field_id_Id )
      display_name = get_module_string( c_field_display_name_Id );
   else if( id == c_field_id_Name )
      display_name = get_module_string( c_field_display_name_Name );
   else if( id == c_field_id_Next_Enum_Id )
      display_name = get_module_string( c_field_display_name_Next_Enum_Id );
   else if( id == c_field_id_Next_Model_Id )
      display_name = get_module_string( c_field_display_name_Next_Model_Id );
   else if( id == c_field_id_Next_Permission_Id )
      display_name = get_module_string( c_field_display_name_Next_Permission_Id );
   else if( id == c_field_id_Next_Type_Id )
      display_name = get_module_string( c_field_display_name_Next_Type_Id );
   else if( id == c_field_id_Standard_Package )
      display_name = get_module_string( c_field_display_name_Standard_Package );

   return display_name;
}

void Meta_Workgroup::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Workgroup::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Workgroup::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Workgroup::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Workgroup::setup_foreign_key( Meta_Package& o, const string& value )
{
   static_cast< Meta_Package& >( o ).set_key( value );
}

void Meta_Workgroup::setup_graph_parent( Meta_Application& o, const string& foreign_key_field )
{
   static_cast< Meta_Application& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Workgroup::setup_graph_parent( Meta_Enum& o, const string& foreign_key_field )
{
   static_cast< Meta_Enum& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Workgroup::setup_graph_parent( Meta_Model& o, const string& foreign_key_field )
{
   static_cast< Meta_Model& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Workgroup::setup_graph_parent( Meta_Permission& o, const string& foreign_key_field )
{
   static_cast< Meta_Permission& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Workgroup::setup_graph_parent( Meta_Type& o, const string& foreign_key_field )
{
   static_cast< Meta_Type& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Workgroup::setup_graph_parent( Meta_User& o, const string& foreign_key_field )
{
   static_cast< Meta_User& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Workgroup::setup_graph_parent(
 Meta_Package& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Package& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Package& >( o ).set_key( init_value );
}

size_t Meta_Workgroup::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Workgroup::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Workgroup::get_num_foreign_key_children( bool is_internal ) const
{
   size_t rc = 6;

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

class_base* Meta_Workgroup::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   if( child_num >= 6 )
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
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302220";
            p_class_base = &child_Application( );
         }
         break;

         case 1:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "300300";
            p_class_base = &child_Enum( );
         }
         break;

         case 2:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "300500";
            p_class_base = &child_Model( );
         }
         break;

         case 3:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301500";
            p_class_base = &child_Permission( );
         }
         break;

         case 4:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "300200";
            p_class_base = &child_Type( );
         }
         break;

         case 5:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "300100";
            p_class_base = &child_User( );
         }
         break;
      }
   }

   return p_class_base;
}

const char* Meta_Workgroup::class_id( ) const
{
   return static_class_id( );
}

const char* Meta_Workgroup::class_name( ) const
{
   return static_class_name( );
}

const char* Meta_Workgroup::plural_name( ) const
{
   return static_plural_name( );
}

const char* Meta_Workgroup::module_id( ) const
{
   return static_module_id( );
}

const char* Meta_Workgroup::module_name( ) const
{
   return static_module_name( );
}

string Meta_Workgroup::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );
   key += "workgroup";

   return get_module_string( key );
}

bool Meta_Workgroup::get_is_alias( ) const
{
   return false;
}

void Meta_Workgroup::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Workgroup::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Workgroup::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( sub_context == "_302220" || sub_context == "child_Application" )
      p_class_base = &child_Application( );
   else if( sub_context == "_300300" || sub_context == "child_Enum" )
      p_class_base = &child_Enum( );
   else if( sub_context == "_300500" || sub_context == "child_Model" )
      p_class_base = &child_Model( );
   else if( sub_context == "_301500" || sub_context == "child_Permission" )
      p_class_base = &child_Permission( );
   else if( sub_context == "_300200" || sub_context == "child_Type" )
      p_class_base = &child_Type( );
   else if( sub_context == "_300100" || sub_context == "child_User" )
      p_class_base = &child_User( );
   else if( sub_context == c_field_id_Standard_Package || sub_context == c_field_name_Standard_Package )
      p_class_base = &Standard_Package( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Workgroup::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Id" );
   names.push_back( "C_Name" );
   names.push_back( "C_Next_Enum_Id" );
   names.push_back( "C_Next_Model_Id" );
   names.push_back( "C_Next_Permission_Id" );
   names.push_back( "C_Next_Type_Id" );
   names.push_back( "C_Standard_Package" );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Workgroup::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Id( ) ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( sql_quote( to_string( Next_Enum_Id( ) ) ) );
   values.push_back( sql_quote( to_string( Next_Model_Id( ) ) ) );
   values.push_back( sql_quote( to_string( Next_Permission_Id( ) ) ) );
   values.push_back( sql_quote( to_string( Next_Type_Id( ) ) ) );
   values.push_back( sql_quote( to_string( Standard_Package( ) ) ) );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Workgroup::get_required_field_names(
 set< string >& names, bool required_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, required_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Workgroup::get_always_required_field_names(
 set< string >& names, bool required_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )required_transients;

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Workgroup::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Workgroup::do_generate_sql( generate_sql_type type, vector< string >& sql_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts );
}

const char* Meta_Workgroup::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Workgroup::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Workgroup::static_lock_class_id( )
{
   return "101100";
}

const char* Meta_Workgroup::static_check_class_name( )
{
   return "Workgroup";
}

bool Meta_Workgroup::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Workgroup::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.101100" );
}

void Meta_Workgroup::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "101102", "Id", "string", false ) );
   all_field_info.push_back( field_info( "101101", "Name", "string", false ) );
   all_field_info.push_back( field_info( "101104", "Next_Enum_Id", "string", false ) );
   all_field_info.push_back( field_info( "101103", "Next_Model_Id", "string", false ) );
   all_field_info.push_back( field_info( "101106", "Next_Permission_Id", "string", false ) );
   all_field_info.push_back( field_info( "101105", "Next_Type_Id", "string", false ) );
   all_field_info.push_back( field_info( "302835", "Standard_Package", "Meta_Package", false ) );
}

void Meta_Workgroup::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Standard_Package, make_pair( "Meta.101100", "Meta_Package" ) ) );
}

int Meta_Workgroup::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Workgroup::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Workgroup::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "101102";
      break;

      case 2:
      p_id = "101101";
      break;

      case 3:
      p_id = "101104";
      break;

      case 4:
      p_id = "101103";
      break;

      case 5:
      p_id = "101106";
      break;

      case 6:
      p_id = "101105";
      break;

      case 7:
      p_id = "302835";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Workgroup" );

   return p_id;
}

const char* Meta_Workgroup::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Id";
      break;

      case 2:
      p_id = "Name";
      break;

      case 3:
      p_id = "Next_Enum_Id";
      break;

      case 4:
      p_id = "Next_Model_Id";
      break;

      case 5:
      p_id = "Next_Permission_Id";
      break;

      case 6:
      p_id = "Next_Type_Id";
      break;

      case 7:
      p_id = "Standard_Package";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Workgroup" );

   return p_id;
}

int Meta_Workgroup::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Id || field == c_field_name_Id )
      rc += 1;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 2;
   else if( field == c_field_id_Next_Enum_Id || field == c_field_name_Next_Enum_Id )
      rc += 3;
   else if( field == c_field_id_Next_Model_Id || field == c_field_name_Next_Model_Id )
      rc += 4;
   else if( field == c_field_id_Next_Permission_Id || field == c_field_name_Next_Permission_Id )
      rc += 5;
   else if( field == c_field_id_Next_Type_Id || field == c_field_name_Next_Type_Id )
      rc += 6;
   else if( field == c_field_id_Standard_Package || field == c_field_name_Standard_Package )
      rc += 7;

   return rc - 1;
}

procedure_info_container& Meta_Workgroup::static_get_procedure_info( )
{
   static procedure_info_container procedures;

   return procedures;
}

string Meta_Workgroup::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(64),"
    "C_Ver_ INTEGER NOT NULL,"
    "C_Rev_ INTEGER NOT NULL,"
    "C_Typ_ VARCHAR(24) NOT NULL,"
    "C_Id VARCHAR(200) NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Next_Enum_Id VARCHAR(200) NOT NULL,"
    "C_Next_Model_Id VARCHAR(200) NOT NULL,"
    "C_Next_Permission_Id VARCHAR(200) NOT NULL,"
    "C_Next_Type_Id VARCHAR(200) NOT NULL,"
    "C_Standard_Package VARCHAR(64) NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Workgroup::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Workgroup::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   ( void )pairs;
}

void Meta_Workgroup::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Id" );
   indexes.push_back( "C_Name" );
}

void Meta_Workgroup::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Id" );
   indexes.push_back( "C_Name" );
}

void Meta_Workgroup::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Workgroup::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Workgroup::static_insert_external_alias( const string& module_and_class_id, Meta_Workgroup* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Workgroup::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Workgroup::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Workgroup::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}

