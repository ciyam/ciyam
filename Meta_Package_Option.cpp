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

#include "Meta_Package_Option.h"

#include "Meta_Model.h"
#include "Meta_Package.h"

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

template< > inline string to_string( const Meta_Model& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Package& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Model& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Package& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline int system( const string& cmd ) { return exec_system( cmd ); }

namespace
{

#include "Meta_Package_Option.cmh"

const int32_t c_version = 1;

const char* const c_okay = "okay";

const char* const c_field_id_Installed = "137103";
const char* const c_field_id_Is_Other_Package = "137104";
const char* const c_field_id_Model = "302825";
const char* const c_field_id_Name = "137101";
const char* const c_field_id_Other_Package = "302830";
const char* const c_field_id_Other_Package_Required = "137105";
const char* const c_field_id_Other_Package_Type = "137106";
const char* const c_field_id_Package = "302820";
const char* const c_field_id_Use_Option = "137102";
const char* const c_field_id_Value = "137107";

const char* const c_field_name_Installed = "Installed";
const char* const c_field_name_Is_Other_Package = "Is_Other_Package";
const char* const c_field_name_Model = "Model";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Other_Package = "Other_Package";
const char* const c_field_name_Other_Package_Required = "Other_Package_Required";
const char* const c_field_name_Other_Package_Type = "Other_Package_Type";
const char* const c_field_name_Package = "Package";
const char* const c_field_name_Use_Option = "Use_Option";
const char* const c_field_name_Value = "Value";

const char* const c_field_display_name_Installed = "field_package_option_installed";
const char* const c_field_display_name_Is_Other_Package = "field_package_option_is_other_package";
const char* const c_field_display_name_Model = "field_package_option_model";
const char* const c_field_display_name_Name = "field_package_option_name";
const char* const c_field_display_name_Other_Package = "field_package_option_other_package";
const char* const c_field_display_name_Other_Package_Required = "field_package_option_other_package_required";
const char* const c_field_display_name_Other_Package_Type = "field_package_option_other_package_type";
const char* const c_field_display_name_Package = "field_package_option_package";
const char* const c_field_display_name_Use_Option = "field_package_option_use_option";
const char* const c_field_display_name_Value = "field_package_option_value";

const int c_num_fields = 10;

const char* const c_all_sorted_field_ids[ ] =
{
   "137101",
   "137102",
   "137103",
   "137104",
   "137105",
   "137106",
   "137107",
   "302820",
   "302825",
   "302830"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Installed",
   "Is_Other_Package",
   "Model",
   "Name",
   "Other_Package",
   "Other_Package_Required",
   "Other_Package_Type",
   "Package",
   "Use_Option",
   "Value"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool has_field( const string& field )
{
   return binary_search( c_all_sorted_field_ids, c_all_sorted_field_ids + c_num_fields, field.c_str( ), compare )
    || binary_search( c_all_sorted_field_names, c_all_sorted_field_names + c_num_fields, field.c_str( ), compare );
}

const int c_num_transient_fields = 1;

const char* const c_transient_sorted_field_ids[ ] =
{
   "137107"
};

const char* const c_transient_sorted_field_names[ ] =
{
   "Value"
};

inline bool transient_compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool is_transient_field( const string& field )
{
   return binary_search( c_transient_sorted_field_ids,
    c_transient_sorted_field_ids + c_num_transient_fields, field.c_str( ), transient_compare )
    || binary_search( c_transient_sorted_field_names,
    c_transient_sorted_field_names + c_num_transient_fields, field.c_str( ), transient_compare );
}

const uint64_t c_modifier_Is_Not_Other_Package_Option = UINT64_C( 0x100 );
const uint64_t c_modifier_Is_Other_Package_Option = UINT64_C( 0x200 );

domain_string_max_size< 100 > g_Name_domain;

set< string > g_derivations;

typedef map< string, Meta_Package_Option* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Package_Option* > external_aliases_lookup_container;
typedef external_aliases_lookup_container::const_iterator external_aliases_lookup_const_iterator;

external_aliases_container g_external_aliases;
external_aliases_lookup_container g_external_aliases_lookup;

bool gv_default_Installed = bool( 0 );
bool gv_default_Is_Other_Package = bool( 0 );
string gv_default_Name = string( );
bool gv_default_Other_Package_Required = bool( 0 );
string gv_default_Other_Package_Type = string( );
bool gv_default_Use_Option = bool( 0 );
string gv_default_Value = string( );

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Package_Option > Package_Option_registration( get_class_registry( ), "137100" );

class Meta_Package_Option_command_functor;

class Meta_Package_Option_command_handler : public command_handler
{
   friend class Meta_Package_Option_command_functor;

   public:
   Meta_Package_Option_command_handler( ) : p_Meta_Package_Option( 0 ) { }

   void set_Meta_Package_Option( Meta_Package_Option* p_new_Meta_Package_Option ) { p_Meta_Package_Option = p_new_Meta_Package_Option; }

   void handle_unknown_command( const string& command )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Package_Option* p_Meta_Package_Option;

   protected:
   string retval;
};

class Meta_Package_Option_command_functor : public command_functor
{
   public:
   Meta_Package_Option_command_functor( Meta_Package_Option_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Package_Option_command_handler& cmd_handler;
};

command_functor* Meta_Package_Option_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Package_Option_command_functor( dynamic_cast< Meta_Package_Option_command_handler& >( handler ) );
}

void Meta_Package_Option_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Package_Option_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_parm_Meta_Package_Option_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Package_Option_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Package_Option->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Package_Option->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Package_Option_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Package_Option_get_field_name ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );
      else if( field_name == c_field_id_Installed || field_name == c_field_name_Installed )
         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Installed( ), cmd_handler.retval );
      else if( field_name == c_field_id_Is_Other_Package || field_name == c_field_name_Is_Other_Package )
         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Is_Other_Package( ), cmd_handler.retval );
      else if( field_name == c_field_id_Model || field_name == c_field_name_Model )
         string_getter< Meta_Model >( cmd_handler.p_Meta_Package_Option->Model( ), cmd_handler.retval );
      else if( field_name == c_field_id_Name || field_name == c_field_name_Name )
         string_getter< string >( cmd_handler.p_Meta_Package_Option->Name( ), cmd_handler.retval );
      else if( field_name == c_field_id_Other_Package || field_name == c_field_name_Other_Package )
         string_getter< Meta_Package >( cmd_handler.p_Meta_Package_Option->Other_Package( ), cmd_handler.retval );
      else if( field_name == c_field_id_Other_Package_Required || field_name == c_field_name_Other_Package_Required )
         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Other_Package_Required( ), cmd_handler.retval );
      else if( field_name == c_field_id_Other_Package_Type || field_name == c_field_name_Other_Package_Type )
         string_getter< string >( cmd_handler.p_Meta_Package_Option->Other_Package_Type( ), cmd_handler.retval );
      else if( field_name == c_field_id_Package || field_name == c_field_name_Package )
         string_getter< Meta_Package >( cmd_handler.p_Meta_Package_Option->Package( ), cmd_handler.retval );
      else if( field_name == c_field_id_Use_Option || field_name == c_field_name_Use_Option )
         string_getter< bool >( cmd_handler.p_Meta_Package_Option->Use_Option( ), cmd_handler.retval );
      else if( field_name == c_field_id_Value || field_name == c_field_name_Value )
         string_getter< string >( cmd_handler.p_Meta_Package_Option->Value( ), cmd_handler.retval );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Package_Option_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Package_Option_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_parm_Meta_Package_Option_set_field_value ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );
      else if( field_name == c_field_id_Installed || field_name == c_field_name_Installed )
         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Installed, field_value );
      else if( field_name == c_field_id_Is_Other_Package || field_name == c_field_name_Is_Other_Package )
         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Is_Other_Package, field_value );
      else if( field_name == c_field_id_Model || field_name == c_field_name_Model )
         func_string_setter< Meta_Package_Option, Meta_Model >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Model, field_value );
      else if( field_name == c_field_id_Name || field_name == c_field_name_Name )
         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Name, field_value );
      else if( field_name == c_field_id_Other_Package || field_name == c_field_name_Other_Package )
         func_string_setter< Meta_Package_Option, Meta_Package >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Other_Package, field_value );
      else if( field_name == c_field_id_Other_Package_Required || field_name == c_field_name_Other_Package_Required )
         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Other_Package_Required, field_value );
      else if( field_name == c_field_id_Other_Package_Type || field_name == c_field_name_Other_Package_Type )
         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Other_Package_Type, field_value );
      else if( field_name == c_field_id_Package || field_name == c_field_name_Package )
         func_string_setter< Meta_Package_Option, Meta_Package >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Package, field_value );
      else if( field_name == c_field_id_Use_Option || field_name == c_field_name_Use_Option )
         func_string_setter< Meta_Package_Option, bool >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Use_Option, field_value );
      else if( field_name == c_field_id_Value || field_name == c_field_name_Value )
         func_string_setter< Meta_Package_Option, string >(
          *cmd_handler.p_Meta_Package_Option, &Meta_Package_Option::Value, field_value );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Package_Option_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Package_Option_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_parm_Meta_Package_Option_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( field_name == c_field_id_Model || field_name == c_field_name_Model )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->Model( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Other_Package || field_name == c_field_name_Other_Package )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->Other_Package( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Package || field_name == c_field_name_Package )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Option->Package( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
}

struct Meta_Package_Option::impl : public Meta_Package_Option_command_handler
{
   impl( Meta_Package_Option& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Package_Option( &o );

      add_commands( 0, Meta_Package_Option_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Package_Option_command_definitions ) );
   }

   Meta_Package_Option& get_obj( ) const
   {
      return *cp_obj;
   }

   bool impl_Installed( ) const { return lazy_fetch( p_obj ), v_Installed; }
   void impl_Installed( bool Installed ) { v_Installed = Installed; }

   bool impl_Is_Other_Package( ) const { return lazy_fetch( p_obj ), v_Is_Other_Package; }
   void impl_Is_Other_Package( bool Is_Other_Package ) { v_Is_Other_Package = Is_Other_Package; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { v_Name = Name; }

   bool impl_Other_Package_Required( ) const { return lazy_fetch( p_obj ), v_Other_Package_Required; }
   void impl_Other_Package_Required( bool Other_Package_Required ) { v_Other_Package_Required = Other_Package_Required; }

   const string& impl_Other_Package_Type( ) const { return lazy_fetch( p_obj ), v_Other_Package_Type; }
   void impl_Other_Package_Type( const string& Other_Package_Type ) { v_Other_Package_Type = Other_Package_Type; }

   bool impl_Use_Option( ) const { return lazy_fetch( p_obj ), v_Use_Option; }
   void impl_Use_Option( bool Use_Option ) { v_Use_Option = Use_Option; }

   const string& impl_Value( ) const { return lazy_fetch( p_obj ), v_Value; }
   void impl_Value( const string& Value ) { v_Value = Value; }

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

   Meta_Package& impl_Other_Package( )
   {
      if( !cp_Other_Package )
      {
         cp_Other_Package.init( );

         p_obj->setup_graph_parent( *cp_Other_Package, c_field_id_Other_Package, v_Other_Package );
      }
      return *cp_Other_Package;
   }

   const Meta_Package& impl_Other_Package( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Other_Package )
      {
         cp_Other_Package.init( );

         p_obj->setup_graph_parent( *cp_Other_Package, c_field_id_Other_Package, v_Other_Package );
      }
      return *cp_Other_Package;
   }

   void impl_Other_Package( const string& key )
   {
      class_base_accessor cba( impl_Other_Package( ) );
      cba.set_key( key );
   }

   Meta_Package& impl_Package( )
   {
      if( !cp_Package )
      {
         cp_Package.init( );

         p_obj->setup_graph_parent( *cp_Package, c_field_id_Package, v_Package );
      }
      return *cp_Package;
   }

   const Meta_Package& impl_Package( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Package )
      {
         cp_Package.init( );

         p_obj->setup_graph_parent( *cp_Package, c_field_id_Package, v_Package );
      }
      return *cp_Package;
   }

   void impl_Package( const string& key )
   {
      class_base_accessor cba( impl_Package( ) );
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

   Meta_Package_Option* p_obj;
   class_pointer< Meta_Package_Option > cp_obj;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   bool v_Installed;
   bool v_Is_Other_Package;
   string v_Name;
   bool v_Other_Package_Required;
   string v_Other_Package_Type;
   bool v_Use_Option;
   string v_Value;

   string v_Model;
   mutable class_pointer< Meta_Model > cp_Model;

   string v_Other_Package;
   mutable class_pointer< Meta_Package > cp_Other_Package;

   string v_Package;
   mutable class_pointer< Meta_Package > cp_Package;
};

string Meta_Package_Option::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Installed( ) );
      break;

      case 1:
      retval = to_string( impl_Is_Other_Package( ) );
      break;

      case 2:
      retval = to_string( impl_Model( ) );
      break;

      case 3:
      retval = to_string( impl_Name( ) );
      break;

      case 4:
      retval = to_string( impl_Other_Package( ) );
      break;

      case 5:
      retval = to_string( impl_Other_Package_Required( ) );
      break;

      case 6:
      retval = to_string( impl_Other_Package_Type( ) );
      break;

      case 7:
      retval = to_string( impl_Package( ) );
      break;

      case 8:
      retval = to_string( impl_Use_Option( ) );
      break;

      case 9:
      retval = to_string( impl_Value( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }

   return retval;
}

void Meta_Package_Option::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Installed, value );
      break;

      case 1:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Is_Other_Package, value );
      break;

      case 2:
      func_string_setter< Meta_Package_Option::impl, Meta_Model >( *this, &Meta_Package_Option::impl::impl_Model, value );
      break;

      case 3:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Name, value );
      break;

      case 4:
      func_string_setter< Meta_Package_Option::impl, Meta_Package >( *this, &Meta_Package_Option::impl::impl_Other_Package, value );
      break;

      case 5:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Other_Package_Required, value );
      break;

      case 6:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Other_Package_Type, value );
      break;

      case 7:
      func_string_setter< Meta_Package_Option::impl, Meta_Package >( *this, &Meta_Package_Option::impl::impl_Package, value );
      break;

      case 8:
      func_string_setter< Meta_Package_Option::impl, bool >( *this, &Meta_Package_Option::impl::impl_Use_Option, value );
      break;

      case 9:
      func_string_setter< Meta_Package_Option::impl, string >( *this, &Meta_Package_Option::impl::impl_Value, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }
}

uint64_t Meta_Package_Option::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start protect_equal)]
   if( check_equal( get_obj( ).Installed( ), true ) )
      state |= ( c_state_uneditable | c_state_undeletable );
   // [(finish protect_equal)]

   // [(start modifier_field_value)]
   if( get_obj( ).Is_Other_Package( ) == true )
      state |= c_modifier_Is_Other_Package_Option;
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   if( get_obj( ).Is_Other_Package( ) == false )
      state |= c_modifier_Is_Not_Other_Package_Option;
   // [(finish modifier_field_value)]

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

const string& Meta_Package_Option::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Package_Option::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Model || field == c_field_name_Model )
      impl_Model( "" );
   else if( field == c_field_id_Other_Package || field == c_field_name_Other_Package )
      impl_Other_Package( "" );
   else if( field == c_field_id_Package || field == c_field_name_Package )
      impl_Package( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Package_Option::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( field == c_field_id_Model || field == c_field_name_Model )
      v_Model = value;
   else if( field == c_field_id_Other_Package || field == c_field_name_Other_Package )
      v_Other_Package = value;
   else if( field == c_field_id_Package || field == c_field_name_Package )
      v_Package = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Package_Option::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Model || field == c_field_name_Model )
      return v_Model;
   else if( field == c_field_id_Other_Package || field == c_field_name_Other_Package )
      return v_Other_Package;
   else if( field == c_field_id_Package || field == c_field_name_Package )
      return v_Package;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Package_Option::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Model, v_Model ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Other_Package, v_Other_Package ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Package, v_Package ) );
}

void Meta_Package_Option::impl::clear( )
{
   v_Installed = gv_default_Installed;
   v_Is_Other_Package = gv_default_Is_Other_Package;
   v_Name = gv_default_Name;
   v_Other_Package_Required = gv_default_Other_Package_Required;
   v_Other_Package_Type = gv_default_Other_Package_Type;
   v_Use_Option = gv_default_Use_Option;
   v_Value = gv_default_Value;

   v_Model = string( );
   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   v_Other_Package = string( );
   if( cp_Other_Package )
      p_obj->setup_foreign_key( *cp_Other_Package, v_Other_Package );

   v_Package = string( );
   if( cp_Package )
      p_obj->setup_foreign_key( *cp_Package, v_Package );
}

bool Meta_Package_Option::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Package_Option::impl::validate( unsigned state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   if( is_null( v_Name ) && !value_will_be_provided( c_field_name_Name ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Name,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Name ) ) ) ) );

   if( v_Model.empty( ) && !value_will_be_provided( c_field_name_Model ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Model,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Model ) ) ) ) );

   if( v_Package.empty( ) && !value_will_be_provided( c_field_name_Package ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Package,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Package ) ) ) ) );

   string error_message;
   if( !is_null( v_Name )
    && ( v_Name != gv_default_Name
    || !value_will_be_provided( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Package_Option::impl::after_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   if( cp_Other_Package )
      p_obj->setup_foreign_key( *cp_Other_Package, v_Other_Package );

   if( cp_Package )
      p_obj->setup_foreign_key( *cp_Package, v_Package );

   // [<start after_fetch>]
//idk
   if( !get_obj( ).Is_Other_Package( ) )
   {
      if( get_obj( ).Use_Option( ) )
         get_obj( ).Value( "yes" );
      else
         get_obj( ).Value( "---" );
   }
   else
      get_obj( ).Value( get_obj( ).Other_Package( ).Name( ) );
   // [<finish after_fetch>]
}

void Meta_Package_Option::impl::finalise_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Package_Option::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Package_Option::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start default_to_field)]
   if( is_create && is_null( get_obj( ).Model( ) ) )
      get_obj( ).Model( get_obj( ).Package( ).Model( ) );
   // [(finish default_to_field)]

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Package_Option::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [<start for_store>]
//idk
   if( get_obj( ).Is_Other_Package( ) )
      get_obj( ).Use_Option( !is_null( get_obj( ).Other_Package( ) ) );
   // [<finish for_store>]
}

void Meta_Package_Option::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Package_Option::impl::can_destroy( bool is_internal )
{
   bool retval = is_internal || !( get_state( ) & c_state_undeletable );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Package_Option::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Package_Option::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Package_Option::impl::set_default_values( )
{
   clear( );
}

bool Meta_Package_Option::impl::is_filtered( const set< string >& filters ) const
{
   ( void )filters;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

#undef MODULE_TRACE
#define MODULE_TRACE( x ) trace( x )

Meta_Package_Option::Meta_Package_Option( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Package_Option::~Meta_Package_Option( )
{
   cleanup( );
   delete p_impl;
}

bool Meta_Package_Option::Installed( ) const
{
   return p_impl->impl_Installed( );
}

void Meta_Package_Option::Installed( bool Installed )
{
   p_impl->impl_Installed( Installed );
}

bool Meta_Package_Option::Is_Other_Package( ) const
{
   return p_impl->impl_Is_Other_Package( );
}

void Meta_Package_Option::Is_Other_Package( bool Is_Other_Package )
{
   p_impl->impl_Is_Other_Package( Is_Other_Package );
}

const string& Meta_Package_Option::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_Package_Option::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

bool Meta_Package_Option::Other_Package_Required( ) const
{
   return p_impl->impl_Other_Package_Required( );
}

void Meta_Package_Option::Other_Package_Required( bool Other_Package_Required )
{
   p_impl->impl_Other_Package_Required( Other_Package_Required );
}

const string& Meta_Package_Option::Other_Package_Type( ) const
{
   return p_impl->impl_Other_Package_Type( );
}

void Meta_Package_Option::Other_Package_Type( const string& Other_Package_Type )
{
   p_impl->impl_Other_Package_Type( Other_Package_Type );
}

bool Meta_Package_Option::Use_Option( ) const
{
   return p_impl->impl_Use_Option( );
}

void Meta_Package_Option::Use_Option( bool Use_Option )
{
   p_impl->impl_Use_Option( Use_Option );
}

const string& Meta_Package_Option::Value( ) const
{
   return p_impl->impl_Value( );
}

void Meta_Package_Option::Value( const string& Value )
{
   p_impl->impl_Value( Value );
}

Meta_Model& Meta_Package_Option::Model( )
{
   return p_impl->impl_Model( );
}

const Meta_Model& Meta_Package_Option::Model( ) const
{
   return p_impl->impl_Model( );
}

void Meta_Package_Option::Model( const string& key )
{
   p_impl->impl_Model( key );
}

Meta_Package& Meta_Package_Option::Other_Package( )
{
   return p_impl->impl_Other_Package( );
}

const Meta_Package& Meta_Package_Option::Other_Package( ) const
{
   return p_impl->impl_Other_Package( );
}

void Meta_Package_Option::Other_Package( const string& key )
{
   p_impl->impl_Other_Package( key );
}

Meta_Package& Meta_Package_Option::Package( )
{
   return p_impl->impl_Package( );
}

const Meta_Package& Meta_Package_Option::Package( ) const
{
   return p_impl->impl_Package( );
}

void Meta_Package_Option::Package( const string& key )
{
   p_impl->impl_Package( key );
}

string Meta_Package_Option::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Package_Option::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

bool Meta_Package_Option::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Package_Option::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Package_Option::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Package_Option::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Package_Option::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Package_Option::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Package_Option::clear( )
{
   p_impl->clear( );
}

void Meta_Package_Option::validate( unsigned state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Package_Option::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Package_Option::finalise_fetch( )
{
   p_impl->finalise_fetch( );
}

void Meta_Package_Option::at_create( )
{
   p_impl->at_create( );
}

void Meta_Package_Option::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Package_Option::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Package_Option::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Package_Option::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Package_Option::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Package_Option::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Package_Option::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Package_Option::is_filtered( const set< string >& filters ) const
{
   return p_impl->is_filtered( filters );
}

const char* Meta_Package_Option::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( 0 );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Installed )
   {
      p_id = c_field_id_Installed;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Other_Package )
   {
      p_id = c_field_id_Is_Other_Package;

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
   else if( name == c_field_name_Other_Package )
   {
      p_id = c_field_id_Other_Package;

      if( p_type_name )
         *p_type_name = "Meta_Package";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Package_Required )
   {
      p_id = c_field_id_Other_Package_Required;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Other_Package_Type )
   {
      p_id = c_field_id_Other_Package_Type;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Package )
   {
      p_id = c_field_id_Package;

      if( p_type_name )
         *p_type_name = "Meta_Package";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Use_Option )
   {
      p_id = c_field_id_Use_Option;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Value )
   {
      p_id = c_field_id_Value;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_id;
}

const char* Meta_Package_Option::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( 0 );

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Installed )
   {
      p_name = c_field_name_Installed;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Other_Package )
   {
      p_name = c_field_name_Is_Other_Package;

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
   else if( id == c_field_id_Other_Package )
   {
      p_name = c_field_name_Other_Package;

      if( p_type_name )
         *p_type_name = "Meta_Package";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Package_Required )
   {
      p_name = c_field_name_Other_Package_Required;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Other_Package_Type )
   {
      p_name = c_field_name_Other_Package_Type;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Package )
   {
      p_name = c_field_name_Package;

      if( p_type_name )
         *p_type_name = "Meta_Package";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Use_Option )
   {
      p_name = c_field_name_Use_Option;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Value )
   {
      p_name = c_field_name_Value;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_name;
}

string Meta_Package_Option::get_field_display_name( const string& id ) const
{
   string display_name;

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_display_name" );
   else if( id == c_field_id_Installed )
      display_name = get_module_string( c_field_display_name_Installed );
   else if( id == c_field_id_Is_Other_Package )
      display_name = get_module_string( c_field_display_name_Is_Other_Package );
   else if( id == c_field_id_Model )
      display_name = get_module_string( c_field_display_name_Model );
   else if( id == c_field_id_Name )
      display_name = get_module_string( c_field_display_name_Name );
   else if( id == c_field_id_Other_Package )
      display_name = get_module_string( c_field_display_name_Other_Package );
   else if( id == c_field_id_Other_Package_Required )
      display_name = get_module_string( c_field_display_name_Other_Package_Required );
   else if( id == c_field_id_Other_Package_Type )
      display_name = get_module_string( c_field_display_name_Other_Package_Type );
   else if( id == c_field_id_Package )
      display_name = get_module_string( c_field_display_name_Package );
   else if( id == c_field_id_Use_Option )
      display_name = get_module_string( c_field_display_name_Use_Option );
   else if( id == c_field_id_Value )
      display_name = get_module_string( c_field_display_name_Value );

   return display_name;
}

void Meta_Package_Option::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Package_Option::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Package_Option::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Package_Option::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Package_Option::setup_foreign_key( Meta_Model& o, const string& value )
{
   static_cast< Meta_Model& >( o ).set_key( value );
}

void Meta_Package_Option::setup_foreign_key( Meta_Package& o, const string& value )
{
   static_cast< Meta_Package& >( o ).set_key( value );
}

void Meta_Package_Option::setup_graph_parent(
 Meta_Model& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Model& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Model& >( o ).set_key( init_value );
}

void Meta_Package_Option::setup_graph_parent(
 Meta_Package& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Package& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Package& >( o ).set_key( init_value );
}

size_t Meta_Package_Option::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Package_Option::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Package_Option::get_num_foreign_key_children( bool is_internal ) const
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

class_base* Meta_Package_Option::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   ( void )child_num;
   ( void )next_child_field;
   ( void )op;

   return p_class_base;
}

const char* Meta_Package_Option::class_id( ) const
{
   return static_class_id( );
}

const char* Meta_Package_Option::class_name( ) const
{
   return static_class_name( );
}

const char* Meta_Package_Option::plural_name( ) const
{
   return static_plural_name( );
}

const char* Meta_Package_Option::module_id( ) const
{
   return static_module_id( );
}

const char* Meta_Package_Option::module_name( ) const
{
   return static_module_name( );
}

string Meta_Package_Option::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );
   key += "package_option";

   return get_module_string( key );
}

bool Meta_Package_Option::get_is_alias( ) const
{
   return false;
}

void Meta_Package_Option::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Package_Option::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Package_Option::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( sub_context == c_field_id_Model || sub_context == c_field_name_Model )
      p_class_base = &Model( );
   else if( sub_context == c_field_id_Other_Package || sub_context == c_field_name_Other_Package )
      p_class_base = &Other_Package( );
   else if( sub_context == c_field_id_Package || sub_context == c_field_name_Package )
      p_class_base = &Package( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Package_Option::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Installed" );
   names.push_back( "C_Is_Other_Package" );
   names.push_back( "C_Model" );
   names.push_back( "C_Name" );
   names.push_back( "C_Other_Package" );
   names.push_back( "C_Other_Package_Required" );
   names.push_back( "C_Other_Package_Type" );
   names.push_back( "C_Package" );
   names.push_back( "C_Use_Option" );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Package_Option::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( to_string( Installed( ) ) );
   values.push_back( to_string( Is_Other_Package( ) ) );
   values.push_back( sql_quote( to_string( Model( ) ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Package( ) ) ) );
   values.push_back( to_string( Other_Package_Required( ) ) );
   values.push_back( sql_quote( to_string( Other_Package_Type( ) ) ) );
   values.push_back( sql_quote( to_string( Package( ) ) ) );
   values.push_back( to_string( Use_Option( ) ) );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Package_Option::get_required_field_names(
 set< string >& names, bool required_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, required_transients, dependents );

   // [<start get_required_field_names>]
//idk
   if( needs_field_value( "Value", dependents ) )
   {
      dependents.insert( "Use_Option" );

      if( ( required_transients && is_field_transient( e_field_id_Use_Option ) )
       || ( !required_transients && !is_field_transient( e_field_id_Use_Option ) ) )
         names.insert( "Use_Option" );
   }

   if( needs_field_value( "Value", dependents ) )
   {
      dependents.insert( "Other_Package" );

      if( ( required_transients && is_field_transient( e_field_id_Other_Package ) )
       || ( !required_transients && !is_field_transient( e_field_id_Other_Package ) ) )
         names.insert( "Other_Package" );
   }
   // [<finish get_required_field_names>]
}

void Meta_Package_Option::get_always_required_field_names(
 set< string >& names, bool required_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )required_transients;

   // [(start protect_equal)]
   dependents.insert( "Installed" );

   if( ( required_transients && is_field_transient( e_field_id_Installed ) )
    || ( !required_transients && !is_field_transient( e_field_id_Installed ) ) )
      names.insert( "Installed" );
   // [(finish protect_equal)]

   // [(start modifier_field_value)]
   dependents.insert( "Is_Other_Package" ); // (for Is_Other_Package_Option modifier)

   if( ( required_transients && is_field_transient( e_field_id_Is_Other_Package ) )
    || ( !required_transients && !is_field_transient( e_field_id_Is_Other_Package ) ) )
      names.insert( "Is_Other_Package" );
   // [(finish modifier_field_value)]

   // [(start modifier_field_value)]
   dependents.insert( "Is_Other_Package" ); // (for Is_Not_Other_Package_Option modifier)

   if( ( required_transients && is_field_transient( e_field_id_Is_Other_Package ) )
    || ( !required_transients && !is_field_transient( e_field_id_Is_Other_Package ) ) )
      names.insert( "Is_Other_Package" );
   // [(finish modifier_field_value)]

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Package_Option::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Package_Option::do_generate_sql( generate_sql_type type, vector< string >& sql_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts );
}

const char* Meta_Package_Option::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Package_Option::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Package_Option::static_lock_class_id( )
{
   return "137100";
}

const char* Meta_Package_Option::static_check_class_name( )
{
   return "Package_Option";
}

bool Meta_Package_Option::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Package_Option::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.137100" );
}

void Meta_Package_Option::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "137103", "Installed", "bool", false ) );
   all_field_info.push_back( field_info( "137104", "Is_Other_Package", "bool", false ) );
   all_field_info.push_back( field_info( "302825", "Model", "Meta_Model", true ) );
   all_field_info.push_back( field_info( "137101", "Name", "string", false ) );
   all_field_info.push_back( field_info( "302830", "Other_Package", "Meta_Package", false ) );
   all_field_info.push_back( field_info( "137105", "Other_Package_Required", "bool", false ) );
   all_field_info.push_back( field_info( "137106", "Other_Package_Type", "string", false ) );
   all_field_info.push_back( field_info( "302820", "Package", "Meta_Package", true ) );
   all_field_info.push_back( field_info( "137102", "Use_Option", "bool", false ) );
   all_field_info.push_back( field_info( "137107", "Value", "string", false ) );
}

void Meta_Package_Option::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Model, make_pair( "Meta.137100", "Meta_Model" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Other_Package, make_pair( "Meta.137100", "Meta_Package" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Package, make_pair( "Meta.137100", "Meta_Package" ) ) );
}

int Meta_Package_Option::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Package_Option::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Package_Option::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "137103";
      break;

      case 2:
      p_id = "137104";
      break;

      case 3:
      p_id = "302825";
      break;

      case 4:
      p_id = "137101";
      break;

      case 5:
      p_id = "302830";
      break;

      case 6:
      p_id = "137105";
      break;

      case 7:
      p_id = "137106";
      break;

      case 8:
      p_id = "302820";
      break;

      case 9:
      p_id = "137102";
      break;

      case 10:
      p_id = "137107";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Package_Option" );

   return p_id;
}

const char* Meta_Package_Option::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Installed";
      break;

      case 2:
      p_id = "Is_Other_Package";
      break;

      case 3:
      p_id = "Model";
      break;

      case 4:
      p_id = "Name";
      break;

      case 5:
      p_id = "Other_Package";
      break;

      case 6:
      p_id = "Other_Package_Required";
      break;

      case 7:
      p_id = "Other_Package_Type";
      break;

      case 8:
      p_id = "Package";
      break;

      case 9:
      p_id = "Use_Option";
      break;

      case 10:
      p_id = "Value";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Package_Option" );

   return p_id;
}

int Meta_Package_Option::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Installed || field == c_field_name_Installed )
      rc += 1;
   else if( field == c_field_id_Is_Other_Package || field == c_field_name_Is_Other_Package )
      rc += 2;
   else if( field == c_field_id_Model || field == c_field_name_Model )
      rc += 3;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 4;
   else if( field == c_field_id_Other_Package || field == c_field_name_Other_Package )
      rc += 5;
   else if( field == c_field_id_Other_Package_Required || field == c_field_name_Other_Package_Required )
      rc += 6;
   else if( field == c_field_id_Other_Package_Type || field == c_field_name_Other_Package_Type )
      rc += 7;
   else if( field == c_field_id_Package || field == c_field_name_Package )
      rc += 8;
   else if( field == c_field_id_Use_Option || field == c_field_name_Use_Option )
      rc += 9;
   else if( field == c_field_id_Value || field == c_field_name_Value )
      rc += 10;

   return rc - 1;
}

procedure_info_container& Meta_Package_Option::static_get_procedure_info( )
{
   static procedure_info_container procedures;

   return procedures;
}

string Meta_Package_Option::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(64),"
    "C_Ver_ INTEGER NOT NULL,"
    "C_Rev_ INTEGER NOT NULL,"
    "C_Typ_ VARCHAR(24) NOT NULL,"
    "C_Installed INTEGER NOT NULL,"
    "C_Is_Other_Package INTEGER NOT NULL,"
    "C_Model VARCHAR(64) NOT NULL,"
    "C_Name VARCHAR(128) NOT NULL,"
    "C_Other_Package VARCHAR(64) NOT NULL,"
    "C_Other_Package_Required INTEGER NOT NULL,"
    "C_Other_Package_Type VARCHAR(128) NOT NULL,"
    "C_Package VARCHAR(64) NOT NULL,"
    "C_Use_Option INTEGER NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Package_Option::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Package_Option::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   ( void )pairs;
}

void Meta_Package_Option::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Package,C_Name" );
}

void Meta_Package_Option::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Package,C_Name" );
}

void Meta_Package_Option::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Package_Option::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Package_Option::static_insert_external_alias( const string& module_and_class_id, Meta_Package_Option* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Package_Option::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Package_Option::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Package_Option::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}

