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

#include "Meta_Package.h"

#include "Meta_Package_Option.h"
#include "Meta_Workgroup.h"
#include "Meta_Model.h"
#include "Meta_Package_Type.h"

#include "ciyam_base.h"
#include "class_domains.h"
#include "module_strings.h"
#include "class_utilities.h"
#include "command_handler.h"
#include "module_interface.h"

// [<start includes>]
//idk
#include "module_management.h"
#include "Meta_Enum.h"
#include "Meta_Type.h"
#include "Meta_Class.h"
#include "Meta_Field.h"
#include "Meta_Index.h"
#include "Meta_Modifier.h"
#include "Meta_Procedure.h"
#include "Meta_Workgroup.h"
#include "Meta_Relationship.h"
#include "Meta_Specification.h"
#include "Meta_Initial_Record.h"
#ifndef _WIN32
#  include <sys/stat.h>
#endif
// [<finish includes>]

using namespace std;

// [<start namespaces>]
// [<finish namespaces>]

template< > inline string to_string( const Meta_Model& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Package_Type& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Model& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Package_Type& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline int system( const string& cmd ) { return exec_system( cmd ); }

namespace
{

#include "Meta_Package.cmh"

const int32_t c_version = 1;

const char* const c_okay = "okay";

const char* const c_field_id_Actions = "136102";
const char* const c_field_id_Install_Details = "136104";
const char* const c_field_id_Installed = "136103";
const char* const c_field_id_Key = "136105";
const char* const c_field_id_Model = "302800";
const char* const c_field_id_Name = "136101";
const char* const c_field_id_Package_Type = "302810";
const char* const c_field_id_Plural = "136107";
const char* const c_field_id_Type_Name = "136108";
const char* const c_field_id_Usage_Count = "136106";

const char* const c_field_name_Actions = "Actions";
const char* const c_field_name_Install_Details = "Install_Details";
const char* const c_field_name_Installed = "Installed";
const char* const c_field_name_Key = "Key";
const char* const c_field_name_Model = "Model";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Package_Type = "Package_Type";
const char* const c_field_name_Plural = "Plural";
const char* const c_field_name_Type_Name = "Type_Name";
const char* const c_field_name_Usage_Count = "Usage_Count";

const char* const c_field_display_name_Actions = "field_package_actions";
const char* const c_field_display_name_Install_Details = "field_package_install_details";
const char* const c_field_display_name_Installed = "field_package_installed";
const char* const c_field_display_name_Key = "field_package_key";
const char* const c_field_display_name_Model = "field_package_model";
const char* const c_field_display_name_Name = "field_package_name";
const char* const c_field_display_name_Package_Type = "field_package_package_type";
const char* const c_field_display_name_Plural = "field_package_plural";
const char* const c_field_display_name_Type_Name = "field_package_type_name";
const char* const c_field_display_name_Usage_Count = "field_package_usage_count";

const int c_num_fields = 10;

const char* const c_all_sorted_field_ids[ ] =
{
   "136101",
   "136102",
   "136103",
   "136104",
   "136105",
   "136106",
   "136107",
   "136108",
   "302800",
   "302810"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Actions",
   "Install_Details",
   "Installed",
   "Key",
   "Model",
   "Name",
   "Package_Type",
   "Plural",
   "Type_Name",
   "Usage_Count"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool has_field( const string& field )
{
   return binary_search( c_all_sorted_field_ids, c_all_sorted_field_ids + c_num_fields, field.c_str( ), compare )
    || binary_search( c_all_sorted_field_names, c_all_sorted_field_names + c_num_fields, field.c_str( ), compare );
}

const int c_num_transient_fields = 2;

const char* const c_transient_sorted_field_ids[ ] =
{
   "136104",
   "136108"
};

const char* const c_transient_sorted_field_names[ ] =
{
   "Install_Details",
   "Type_Name"
};

inline bool transient_compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool is_transient_field( const string& field )
{
   return binary_search( c_transient_sorted_field_ids,
    c_transient_sorted_field_ids + c_num_transient_fields, field.c_str( ), transient_compare )
    || binary_search( c_transient_sorted_field_names,
    c_transient_sorted_field_names + c_num_transient_fields, field.c_str( ), transient_compare );
}

const char* const c_procedure_id_Cancel_Remove = "136450";
const char* const c_procedure_id_Check_Install = "136430";
const char* const c_procedure_id_Complete_Remove = "136440";
const char* const c_procedure_id_Install = "136410";
const char* const c_procedure_id_Remove = "136420";

const uint64_t c_modifier_Is_In_Use = UINT64_C( 0x100 );
const uint64_t c_modifier_Is_Not_Installed = UINT64_C( 0x200 );

aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Name_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Plural_domain;

set< string > g_derivations;

typedef map< string, Meta_Package* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Package* > external_aliases_lookup_container;
typedef external_aliases_lookup_container::const_iterator external_aliases_lookup_const_iterator;

external_aliases_container g_external_aliases;
external_aliases_lookup_container g_external_aliases_lookup;

string gv_default_Actions = string( "136410" );
string gv_default_Install_Details = string( );
bool gv_default_Installed = bool( 0 );
string gv_default_Key = string( );
string gv_default_Name = string( );
string gv_default_Plural = string( );
string gv_default_Type_Name = string( );
int gv_default_Usage_Count = int( 0 );

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Package > Package_registration( get_class_registry( ), "136100" );

class Meta_Package_command_functor;

class Meta_Package_command_handler : public command_handler
{
   friend class Meta_Package_command_functor;

   public:
   Meta_Package_command_handler( ) : p_Meta_Package( 0 ) { }

   void set_Meta_Package( Meta_Package* p_new_Meta_Package ) { p_Meta_Package = p_new_Meta_Package; }

   void handle_unknown_command( const string& command )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Package* p_Meta_Package;

   protected:
   string retval;
};

class Meta_Package_command_functor : public command_functor
{
   public:
   Meta_Package_command_functor( Meta_Package_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Package_command_handler& cmd_handler;
};

command_functor* Meta_Package_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Package_command_functor( dynamic_cast< Meta_Package_command_handler& >( handler ) );
}

void Meta_Package_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Package_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_parm_Meta_Package_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Package->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Package->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Package_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Package->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Package->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Package_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Package_get_field_name ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );
      else if( field_name == c_field_id_Actions || field_name == c_field_name_Actions )
         string_getter< string >( cmd_handler.p_Meta_Package->Actions( ), cmd_handler.retval );
      else if( field_name == c_field_id_Install_Details || field_name == c_field_name_Install_Details )
         string_getter< string >( cmd_handler.p_Meta_Package->Install_Details( ), cmd_handler.retval );
      else if( field_name == c_field_id_Installed || field_name == c_field_name_Installed )
         string_getter< bool >( cmd_handler.p_Meta_Package->Installed( ), cmd_handler.retval );
      else if( field_name == c_field_id_Key || field_name == c_field_name_Key )
         string_getter< string >( cmd_handler.p_Meta_Package->Key( ), cmd_handler.retval );
      else if( field_name == c_field_id_Model || field_name == c_field_name_Model )
         string_getter< Meta_Model >( cmd_handler.p_Meta_Package->Model( ), cmd_handler.retval );
      else if( field_name == c_field_id_Name || field_name == c_field_name_Name )
         string_getter< string >( cmd_handler.p_Meta_Package->Name( ), cmd_handler.retval );
      else if( field_name == c_field_id_Package_Type || field_name == c_field_name_Package_Type )
         string_getter< Meta_Package_Type >( cmd_handler.p_Meta_Package->Package_Type( ), cmd_handler.retval );
      else if( field_name == c_field_id_Plural || field_name == c_field_name_Plural )
         string_getter< string >( cmd_handler.p_Meta_Package->Plural( ), cmd_handler.retval );
      else if( field_name == c_field_id_Type_Name || field_name == c_field_name_Type_Name )
         string_getter< string >( cmd_handler.p_Meta_Package->Type_Name( ), cmd_handler.retval );
      else if( field_name == c_field_id_Usage_Count || field_name == c_field_name_Usage_Count )
         string_getter< int >( cmd_handler.p_Meta_Package->Usage_Count( ), cmd_handler.retval );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Package_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Package_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_parm_Meta_Package_set_field_value ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );
      else if( field_name == c_field_id_Actions || field_name == c_field_name_Actions )
         func_string_setter< Meta_Package, string >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Actions, field_value );
      else if( field_name == c_field_id_Install_Details || field_name == c_field_name_Install_Details )
         func_string_setter< Meta_Package, string >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Install_Details, field_value );
      else if( field_name == c_field_id_Installed || field_name == c_field_name_Installed )
         func_string_setter< Meta_Package, bool >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Installed, field_value );
      else if( field_name == c_field_id_Key || field_name == c_field_name_Key )
         func_string_setter< Meta_Package, string >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Key, field_value );
      else if( field_name == c_field_id_Model || field_name == c_field_name_Model )
         func_string_setter< Meta_Package, Meta_Model >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Model, field_value );
      else if( field_name == c_field_id_Name || field_name == c_field_name_Name )
         func_string_setter< Meta_Package, string >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Name, field_value );
      else if( field_name == c_field_id_Package_Type || field_name == c_field_name_Package_Type )
         func_string_setter< Meta_Package, Meta_Package_Type >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Package_Type, field_value );
      else if( field_name == c_field_id_Plural || field_name == c_field_name_Plural )
         func_string_setter< Meta_Package, string >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Plural, field_value );
      else if( field_name == c_field_id_Type_Name || field_name == c_field_name_Type_Name )
         func_string_setter< Meta_Package, string >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Type_Name, field_value );
      else if( field_name == c_field_id_Usage_Count || field_name == c_field_name_Usage_Count )
         func_string_setter< Meta_Package, int >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Usage_Count, field_value );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Package_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Package_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_parm_Meta_Package_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( field_name == c_field_id_Model || field_name == c_field_name_Model )
         cmd_handler.retval = cmd_handler.p_Meta_Package->Model( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Package_Type || field_name == c_field_name_Package_Type )
         cmd_handler.retval = cmd_handler.p_Meta_Package->Package_Type( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
   else if( command == c_cmd_Meta_Package_Cancel_Remove )
   {
      cmd_handler.p_Meta_Package->Cancel_Remove( );

      cmd_handler.retval.erase( );
   }
   else if( command == c_cmd_Meta_Package_Check_Install )
   {
      cmd_handler.p_Meta_Package->Check_Install( );

      cmd_handler.retval.erase( );
   }
   else if( command == c_cmd_Meta_Package_Complete_Remove )
   {
      cmd_handler.p_Meta_Package->Complete_Remove( );

      cmd_handler.retval.erase( );
   }
   else if( command == c_cmd_Meta_Package_Install )
   {
      cmd_handler.p_Meta_Package->Install( );

      cmd_handler.retval.erase( );
   }
   else if( command == c_cmd_Meta_Package_Remove )
   {
      cmd_handler.p_Meta_Package->Remove( );

      cmd_handler.retval.erase( );
   }
}

struct Meta_Package::impl : public Meta_Package_command_handler
{
   impl( Meta_Package& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Package( &o );

      add_commands( 0, Meta_Package_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Package_command_definitions ) );
   }

   Meta_Package& get_obj( ) const
   {
      return *cp_obj;
   }

   const string& impl_Actions( ) const { return lazy_fetch( p_obj ), v_Actions; }
   void impl_Actions( const string& Actions ) { v_Actions = Actions; }

   const string& impl_Install_Details( ) const { return lazy_fetch( p_obj ), v_Install_Details; }
   void impl_Install_Details( const string& Install_Details ) { v_Install_Details = Install_Details; }

   bool impl_Installed( ) const { return lazy_fetch( p_obj ), v_Installed; }
   void impl_Installed( bool Installed ) { v_Installed = Installed; }

   const string& impl_Key( ) const { return lazy_fetch( p_obj ), v_Key; }
   void impl_Key( const string& Key ) { v_Key = Key; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { v_Name = Name; }

   const string& impl_Plural( ) const { return lazy_fetch( p_obj ), v_Plural; }
   void impl_Plural( const string& Plural ) { v_Plural = Plural; }

   const string& impl_Type_Name( ) const { return lazy_fetch( p_obj ), v_Type_Name; }
   void impl_Type_Name( const string& Type_Name ) { v_Type_Name = Type_Name; }

   int impl_Usage_Count( ) const { return lazy_fetch( p_obj ), v_Usage_Count; }
   void impl_Usage_Count( int Usage_Count ) { v_Usage_Count = Usage_Count; }

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

   Meta_Package_Option& impl_child_Package_Option_Other( )
   {
      if( !cp_child_Package_Option_Other )
      {
         cp_child_Package_Option_Other.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option_Other, "302830" );
      }
      return *cp_child_Package_Option_Other;
   }

   const Meta_Package_Option& impl_child_Package_Option_Other( ) const
   {
      if( !cp_child_Package_Option_Other )
      {
         cp_child_Package_Option_Other.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option_Other, "302830" );
      }
      return *cp_child_Package_Option_Other;
   }

   Meta_Workgroup& impl_child_Workgroup_Standard( )
   {
      if( !cp_child_Workgroup_Standard )
      {
         cp_child_Workgroup_Standard.init( );

         p_obj->setup_graph_parent( *cp_child_Workgroup_Standard, "302835" );
      }
      return *cp_child_Workgroup_Standard;
   }

   const Meta_Workgroup& impl_child_Workgroup_Standard( ) const
   {
      if( !cp_child_Workgroup_Standard )
      {
         cp_child_Workgroup_Standard.init( );

         p_obj->setup_graph_parent( *cp_child_Workgroup_Standard, "302835" );
      }
      return *cp_child_Workgroup_Standard;
   }

   Meta_Package_Option& impl_child_Package_Option( )
   {
      if( !cp_child_Package_Option )
      {
         cp_child_Package_Option.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option, "302820" );
      }
      return *cp_child_Package_Option;
   }

   const Meta_Package_Option& impl_child_Package_Option( ) const
   {
      if( !cp_child_Package_Option )
      {
         cp_child_Package_Option.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option, "302820" );
      }
      return *cp_child_Package_Option;
   }

   void impl_Cancel_Remove( );

   void impl_Check_Install( );

   void impl_Complete_Remove( );

   void impl_Install( );

   void impl_Remove( );

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

   Meta_Package* p_obj;
   class_pointer< Meta_Package > cp_obj;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   string v_Actions;
   string v_Install_Details;
   bool v_Installed;
   string v_Key;
   string v_Name;
   string v_Plural;
   string v_Type_Name;
   int v_Usage_Count;

   string v_Model;
   mutable class_pointer< Meta_Model > cp_Model;

   string v_Package_Type;
   mutable class_pointer< Meta_Package_Type > cp_Package_Type;

   mutable class_pointer< Meta_Package_Option > cp_child_Package_Option_Other;
   mutable class_pointer< Meta_Workgroup > cp_child_Workgroup_Standard;
   mutable class_pointer< Meta_Package_Option > cp_child_Package_Option;
};

void Meta_Package::impl::impl_Cancel_Remove( )
{
   // [<start Cancel_Remove_impl>]
//idk
   if( !storage_locked_for_admin( ) )
   {
      get_obj( ).op_update( );
      get_obj( ).Actions( "136420" );
      get_obj( ).op_apply( );
   }
   // [<finish Cancel_Remove_impl>]
}

void Meta_Package::impl::impl_Check_Install( )
{
   // [<start Check_Install_impl>]
//idk
   string map_filename( get_obj( ).get_attached_file_path( get_obj( ).get_key( ) + ".map" ) );
   string new_filename( get_obj( ).get_attached_file_path( get_obj( ).get_key( ) + ".new" ) );

   bool was_okay = false;
   get_obj( ).op_update( );

   if( storage_locked_for_admin( ) )
      was_okay = exists_file( map_filename );
   else
   {
      string temp_name( get_session_variable( "@package" ) );

      string temp_map_filename( temp_name + ".lst.map" );
      string temp_new_filename( temp_name + ".lst.new" );

      was_okay = exists_file( temp_map_filename );

      if( was_okay )
      {
         if( lower( get_obj( ).Install_Details( ) ).find( "error" ) == string::npos )
         {
            create_directories_for_file_name( map_filename );

            copy_file( temp_map_filename, map_filename );

            if( exists_file( temp_new_filename ) )
               copy_file( temp_new_filename, new_filename );
         }
         else
            was_okay = false;

         remove_file( temp_map_filename );
      }

      remove_file( temp_new_filename );
   }

   if( !was_okay )
      get_obj( ).Actions( "136410" );
   else
   {
      get_obj( ).Actions( "136420" );
      get_obj( ).Installed( true );

      if( get_obj( ).Name( ) == "Standard" && is_null( get_obj( ).Model( ).Workgroup( ).Standard_Package( ) ) )
      {
         get_obj( ).Model( ).Workgroup( ).op_update( );
         get_obj( ).Model( ).Workgroup( ).Standard_Package( get_obj( ).get_key( ) );
         get_obj( ).Model( ).Workgroup( ).op_apply( );
      }
   }

   get_obj( ).op_apply( );

   string model_key( "Meta_Model_" + get_obj( ).Model( ).get_key( ) );
   set_system_variable( model_key, "" );
   // [<finish Check_Install_impl>]
}

void Meta_Package::impl::impl_Complete_Remove( )
{
   // [<start Complete_Remove_impl>]
//idk
   get_obj( ).op_update( );

   get_obj( ).Actions( "136410" );
   get_obj( ).Installed( false );

   string std_package_key;
   if( !is_null( get_obj( ).Model( ).Workgroup( ).Standard_Package( ) ) )
      std_package_key = get_obj( ).Model( ).Workgroup( ).Standard_Package( ).get_key( );

   if( get_obj( ).get_key( ) == std_package_key )
   {
      get_obj( ).Model( ).Workgroup( ).op_update( );
      get_obj( ).Model( ).Workgroup( ).Standard_Package( "" );
      get_obj( ).Model( ).Workgroup( ).op_apply( );
   }

   get_obj( ).op_apply( );

   if( !storage_locked_for_admin( ) )
   {
      string new_filename( get_obj( ).get_attached_file_path( get_obj( ).get_key( ) + ".new" ) );
      remove_file( new_filename );
   }

   string model_key( "Meta_Model_" + get_obj( ).Model( ).get_key( ) );
   set_system_variable( model_key, "" );
   // [<finish Complete_Remove_impl>]
}

void Meta_Package::impl::impl_Install( )
{
   // [<start Install_impl>]
//idk
   string std_package_key;
   if( !is_null( get_obj( ).Model( ).Workgroup( ).Standard_Package( ) ) )
      std_package_key = get_obj( ).Model( ).Workgroup( ).Standard_Package( ).get_key( );

   if( !std_package_key.empty( ) && get_obj( ).Name( ) == "Standard" )
   {
      get_obj( ).op_update( );

      get_obj( ).Actions( "136420" );
      get_obj( ).Installed( true );

      get_obj( ).Model( ).Workgroup( ).Standard_Package( ).op_update( );

      int count = get_obj( ).Model( ).Workgroup( ).Standard_Package( ).Usage_Count( );
      get_obj( ).Model( ).Workgroup( ).Standard_Package( ).Usage_Count( ++count );

      get_obj( ).Model( ).Workgroup( ).Standard_Package( ).op_apply( );

      get_obj( ).op_apply( );
   }
   else if( !storage_locked_for_admin( ) )
   {
      vector< string > dependencies;
      map< string, string > installed_types;

      if( get_obj( ).Model( ).child_Package( ).iterate_forwards( ) )
      {
         do
         {
            if( get_obj( ).Model( ).child_Package( ).Installed( ) )
            {
               if( !std_package_key.empty( )
                && get_obj( ).Model( ).child_Package( ).Package_Type( ).Name( ) == "Standard" )
                  installed_types.insert( make_pair( "Standard", std_package_key ) );
               else
                  installed_types.insert( make_pair(
                   get_obj( ).Model( ).child_Package( ).Package_Type( ).Name( ), get_obj( ).Model( ).child_Package( ).get_key( ) ) );
            }
         } while( get_obj( ).Model( ).child_Package( ).iterate_next( ) );
      }

      string deps( get_obj( ).Package_Type( ).Dependencies( ) );
      deps = search_replace( deps, "\r\n", "," );

      vector< string > dependency_keys;

      if( !deps.empty( ) )
      {
         split_string( deps, dependencies );
         for( size_t i = 0; i < dependencies.size( ); i++ )
         {
            string next( dependencies[ i ] );

            string::size_type pos = next.find( ' ' );
            if( pos != string::npos )
               next.erase( pos );

            if( !installed_types.count( next ) )
               throw runtime_error( "This package requires '" + next + "' to first be installed." ); // FUTURE: Should be a module string...

            dependency_keys.push_back( installed_types[ next ] );
         }
      }

      if( get_obj( ).child_Package_Option( ).iterate_forwards( ) )
      {
         do
         {
            if( get_obj( ).child_Package_Option( ).Is_Other_Package( )
             && get_obj( ).child_Package_Option( ).Other_Package_Required( )
             && is_null( get_obj( ).child_Package_Option( ).Other_Package( ) ) )
               throw runtime_error( "This package requires option '"
                + get_obj( ).child_Package_Option( ).Name( ) + "' to be provided." ); // FUTURE: Should be a module string...
            else if( get_obj( ).child_Package_Option( ).Is_Other_Package( )
             && !is_null( get_obj( ).child_Package_Option( ).Other_Package( ) ) )
            {
               if( !installed_types.count( get_obj( ).child_Package_Option( ).Other_Package( ).Package_Type( ).Name( ) ) )
                  throw runtime_error( "This package requires '"
                   + get_obj( ).child_Package_Option( ).Other_Package( ).Name( ) + "' to first be installed." ); // FUTURE: Should be a module string...

               dependency_keys.push_back( get_obj( ).child_Package_Option( ).Other_Package( ).get_key( ) );
            }

         } while( get_obj( ).child_Package_Option( ).iterate_next( ) );
      }

      string temp_name( "~" + get_uuid( ) );
      string list_filename( temp_name + ".lst" );
#ifndef _WIN32
      string script_filename( temp_name );
#else
      string script_filename( temp_name + ".bat" );
#endif
      string commands_filename( temp_name + ".cin" );

      string type_name( get_obj( ).Package_Type( ).Name( ) );
      string keys_filename( type_name + ".keys.lst" );

      bool async = true;
      if( get_obj( ).get_variable( "@async" ) == "0" || get_obj( ).get_variable( "@async" ) == "false" )
         async = false;

      // NOTE: Empty code block for scope purposes.
      {
         ifstream inpf( keys_filename.c_str( ) );
         if( !inpf )
            throw runtime_error( "unable to open '" + keys_filename + "' for input" );

         ofstream outf( list_filename.c_str( ) );

         if( !outf )
            throw runtime_error( "unable to open '" + list_filename + "' for output" );

         if( dependency_keys.empty( ) )
         {
            outf << "key_group=" << get_obj( ).Model( ).Workgroup( ).get_key( ) << "\n";
            outf << "key_model=" << get_obj( ).Model( ).get_key( ) << "\n";
         }
         else for( size_t i = 0; i < dependency_keys.size( ); i++ )
         {
            vector< string > lines;
            string map_filename( get_obj( ).get_attached_file_path( dependency_keys[ i ] + ".map" ) );

            read_file_lines( map_filename, lines );
            for( size_t j = 0; j < lines.size( ); j++ )
               outf << lines[ j ] << '\n';
         }

         if( get_obj( ).Plural( ) != get_obj( ).Package_Type( ).Plural( ) )
            outf << get_obj( ).Package_Type( ).Plural( ) << "=" << get_obj( ).Plural( ) << '\n';

         if( get_obj( ).Name( ) != get_obj( ).Package_Type( ).Name( ) )
            outf << get_obj( ).Package_Type( ).Name( ) << "=" << get_obj( ).Name( ) << '\n';

         map< string, bool > options;
         if( get_obj( ).child_Package_Option( ).iterate_forwards( ) )
         {
            do
            {
               options.insert( make_pair(
                get_obj( ).child_Package_Option( ).Name( ), get_obj( ).child_Package_Option( ).Use_Option( ) ) );
            } while( get_obj( ).child_Package_Option( ).iterate_next( ) );
         }

         string opt_prefix( "opt_" + lower( get_obj( ).Package_Type( ).Name( ) ) + "_" );

         string next;
         set< string > options_processed;
         while( getline( inpf, next ) )
         {
            // NOTE: In case the input file had been treated as binary during an FTP remove trailing CR.
            if( next.size( ) && next[ next.size( ) - 1 ] == '\r' )
               next.erase( next.size( ) - 1 );

            if( next.find( opt_prefix ) == 0 )
            {
               string::size_type pos = next.find( '=' );
               if( pos != string::npos )
               {
                  string opt_name( next.substr( 0, pos ) );
                  if( options.count( opt_name ) && !options_processed.count( opt_name ) )
                  {
                     next = opt_name + '=';
                     options_processed.insert( opt_name );

                     if( options[ opt_name ] )
                        next += "1";
                  }
               }
            }

            outf << next << '\n';
         }

         string install_log( get_obj( ).get_key( ) + ".install.log" );

         ofstream outl( install_log.c_str( ) );
         if( !outl )
            throw runtime_error( "unable to open '" + install_log + "' for output" );

         outl << "Starting Install...\n"; // FUTURE: Should be a module string...

         ofstream outs( script_filename.c_str( ) );
         if( !outs )
            throw runtime_error( "unable to open '" + script_filename + "' for output" );
#ifdef _WIN32
         outs << "@echo off\n";
         outs << "ciyam_client -quiet -no_prompt < ";
#else
         outs << "./ciyam_client -quiet -no_prompt < ";
#endif
         outs << commands_filename << " >>" << install_log << "\n";
         outs << "echo Finished Install..." << ">>" << install_log << "\n"; // FUTURE: Should be a module string...

#ifdef _WIN32
         outs << "del " << list_filename << " " << commands_filename << "\n";
#else
         outs << "rm " << list_filename << " " << commands_filename << "\n";
#endif
         ofstream outc( commands_filename.c_str( ) );
         if( !outc )
            throw runtime_error( "unable to open '" + commands_filename + "' for output" );

         outc << ".storage_init " << storage_name( ) << "\n";
         outc << ".session_variable @package " << temp_name << "\n";

         outc << "perform_package_import " << get_uid( ) << " @now " << get_obj( ).module_name( )
          << " " << type_name << ".package.sio -new_only -s=@Meta_Class.skips.lst -r=@" << list_filename << "\n";

         outc << ".perform_execute " << get_uid( ) << " @now " << get_obj( ).module_id( ) << " "
          << get_obj( ).class_id( ) << " " << get_obj( ).get_key( ) << " 136430\n";

         if( async )
            outc << ".session_lock -release -at_term " << session_id( ) << "\n"; // see NOTE below...

         outc << ".quit\n";
      }

      get_obj( ).op_update( );
      get_obj( ).Actions( "" );
      get_obj( ).op_apply( );

      string model_key( "Meta_Model_" + get_obj( ).Model( ).get_key( ) );
      set_system_variable( model_key, "Installing package '" + get_obj( ).Name( ) + "'..." ); // FUTURE: Should be a module string...

      // NOTE: If the thread that has spawned the child process is terminated (due
      // to client deciding to finish its session) then this can potentially cause
      // big troubles due to resource inheritance so the session is captured prior
      // to the async request and will be released at the end of the script.
      if( async )
         capture_session( session_id( ) );

#ifdef _WIN32
      exec_system( "run_temp " + script_filename, async );
#else
      chmod( script_filename.c_str( ), 0777 );
      exec_system( "./run_temp " + script_filename, async );
#endif
   }
   // [<finish Install_impl>]
}

void Meta_Package::impl::impl_Remove( )
{
   // [<start Remove_impl>]
//idk
   string std_package_key;
   if( !is_null( get_obj( ).Model( ).Workgroup( ).Standard_Package( ) ) )
      std_package_key = get_obj( ).Model( ).Workgroup( ).Standard_Package( ).get_key( );

   if( !std_package_key.empty( ) && get_obj( ).get_key( ) != std_package_key && get_obj( ).Name( ) == "Standard" )
   {
      get_obj( ).op_update( );

      get_obj( ).Model( ).Workgroup( ).Standard_Package( ).op_update( );

      int count = get_obj( ).Model( ).Workgroup( ).Standard_Package( ).Usage_Count( );
      get_obj( ).Model( ).Workgroup( ).Standard_Package( ).Usage_Count( --count );

      get_obj( ).Model( ).Workgroup( ).Standard_Package( ).op_apply( );

      get_obj( ).Actions( "136410" );
      get_obj( ).Installed( false );

      get_obj( ).op_apply( );
   }
   else if( !storage_locked_for_admin( ) )
   {
      if( get_obj( ).Usage_Count( ) )
         throw runtime_error( "This package cannot be removed as it is being referenced by other models." );

      class_pointer< Meta_Package > cp_other( e_create_instance );
      if( cp_other->iterate_forwards( ) )
      {
         do
         {
            if( cp_other->Installed( ) && cp_other->get_key( ) != get_obj( ).get_key( ) )
            {
               string deps( cp_other->Package_Type( ).Dependencies( ) );
               deps = search_replace( deps, "\r\n", "," );

               if( !deps.empty( ) )
               {
                  vector< string > dependencies;
                  split_string( deps, dependencies );

                  for( size_t i = 0; i < dependencies.size( ); i++ )
                  {
                     string next( dependencies[ i ] );

                     string::size_type pos = next.find( ' ' );
                     if( pos != string::npos )
                        next.erase( pos );

                     if( next == get_obj( ).Package_Type( ).Name( ) )
                        throw runtime_error( "Need to remove '"
                         + cp_other->Name( ) + "' before this package can be removed." );
                  }
               }

               if( cp_other->child_Package_Option( ).iterate_forwards( ) )
               {
                  do
                  {
                     if( get_obj( ).get_key( ) == cp_other->child_Package_Option( ).Other_Package( ).get_key( ) )
                        throw runtime_error( "Need to remove '" + cp_other->Name( ) + "' before this package can be removed." );
                  } while( cp_other->child_Package_Option( ).iterate_next( ) );
               }
            }
         } while( cp_other->iterate_next( ) );
      }

      get_obj( ).op_update( );

      string new_filename( get_obj( ).get_attached_file_path( get_obj( ).get_key( ) + ".new" ) );
      if( exists_file( new_filename ) )
      {
         vector< string > lines;
         read_file_lines( new_filename, lines );

         map< string, vector< string > > class_keys;

         for( size_t i = 0; i < lines.size( ); i++ )
         {
            string next( lines[ i ] );

            string::size_type pos = next.find( ':' );
            if( pos == string::npos )
               throw runtime_error( "unexpected new key line '" + next + "'" );

            class_keys[ next.substr( 0, pos ) ].push_back( next.substr( pos + 1 ) );
         }

#ifndef _WIN32
         string script_filename( get_obj( ).get_key( ) );
#else
         string script_filename( get_obj( ).get_key( ) + ".bat" );
#endif
         string commands_filename( get_obj( ).get_key( ) + ".cin" );
         // NOTE: Empty code block for scope purposes.
         {
            ofstream outf( commands_filename.c_str( ) );
            if( !outf )
               throw runtime_error( "unable to open file '" + commands_filename + "' for output" );

            outf << ".storage_init " << storage_name( ) << '\n';
            outf << "@ifndef %ERROR%\n";
            outf << ".storage_trans_start\n";
            outf << "@endif\n";

            // NOTE: Packages could contain updates of external artifacts (such as specifications) which
            // need to be "undone" as updates (rather than occurring automatically via cascades) so here
            // the package will be processed for just this purpose (the keys for such updates are marked
            // specifically for this purpose).
            string map_filename( get_obj( ).get_attached_file_path( get_obj( ).get_key( ) + ".map" ) );
            if( exists_file( map_filename ) )
               outf << ".perform_package_import " << get_uid( ) << " @now " << get_obj( ).module_name( )
                << " " << get_obj( ).Package_Type( ).Name( ) << ".package.sio -for_remove -r=@" << map_filename << "\n";

            vector< string > ordered;
            string acyclic_filename( string( get_obj( ).module_name( ) ) + ".acyclic.lst" );
            read_file_lines( acyclic_filename, ordered );

            // NOTE: Forcing "class" to be processed first is done as a performance optimisation
            // (as it will automatically cascade numerous other records).
            ordered.push_back( "Class" );

            // NOTE: In order to make sure deletes are correctedly ordered they need to be
            // processed in the opposite of the acyclic class list (as its ordering is for
            // record creation).
            reverse( ordered.begin( ), ordered.end( ) );

            int total = 0;
            for( size_t i = 0; i < ordered.size( ); i++ )
            {
               string next_cid = get_class_id_for_id_or_name( get_obj( ).module_id( ), ordered[ i ] );
               for( int j = 0; j < class_keys[ next_cid ].size( ); j++ )
               {
                  outf << "@ifndef %ERROR%\n";
                  outf << ".perform_destroy " << get_uid( ) << " @now "
                   << get_obj( ).module_id( ) << ' ' << next_cid << " -q " << class_keys[ next_cid ][ j ] << '\n';
                  outf << "@ifdef %ERROR%\n";
                  outf << "#(failed to delete " << ordered[ i ] << " record " << class_keys[ next_cid ][ j ] << ")\n";
                  outf << "@endif\n";
                  if( ++total % 50 == 0 )
                     outf << "#Processed " << total << " records...\n";
                  outf << "@endif\n";
               }
            }

            outf << "@ifndef %ERROR%\n";
            outf << ".storage_trans_commit\n";
            outf << "@else\n";
            outf << ".storage_trans_rollback\n";
            outf << "@endif\n";
            outf << "@ifndef %ERROR%\n";
            outf << ".perform_execute " << get_uid( ) << " @now " << get_obj( ).module_id( ) << " "
             << get_obj( ).class_id( ) << " " << get_obj( ).get_key( ) << " 136440\n";
            outf << "@else\n";
            outf << ".perform_execute " << get_uid( ) << " @now " << get_obj( ).module_id( ) << " "
             << get_obj( ).class_id( ) << " " << get_obj( ).get_key( ) << " 136450\n";
            outf << "@endif\n";
#ifdef _WIN32
            outf << ".session_lock -release " << session_id( ) << "\n"; // see NOTE below...
#endif
            outf << ".quit\n";

            string install_log( get_obj( ).get_key( ) + ".install.log" );

            ofstream outl( install_log.c_str( ), ios::out | ios::app );
            if( !outl )
               throw runtime_error( "unable to open '" + install_log + "' for output" );

            outl << "\nStarting Remove...\n";

            ofstream outs( script_filename.c_str( ) );
            if( !outs )
               throw runtime_error( "unable to open '" + script_filename + "' for output" );
#ifdef _WIN32
            outs << "@echo off\n";
            outs << "ciyam_client -quiet -no_prompt < ";
#else
            outs << "./ciyam_client -quiet -no_prompt < ";
#endif
            outs << commands_filename << " >>" << install_log << "\n";
#ifdef _WIN32
            outs << "del " << commands_filename << "\n";
#else
            outs << "rm " << commands_filename << "\n";
#endif
            outs << "echo Finished Remove...>>" << install_log << "\n";
            get_obj( ).Actions( "" );
            get_obj( ).op_apply( );
         }

         string model_key( "Meta_Model_" + get_obj( ).Model( ).get_key( ) );
         set_system_variable( model_key, "Removing package '" + get_obj( ).Name( ) + "'..." ); // FUTURE: Should be a module string...

#ifdef _WIN32
         // NOTE: Due to file locking inheritance in Win32 prevent a dead socket from
         // killing this session until the asychronous operations have been completed.
         capture_session( session_id( ) );
         exec_system( "run_temp " + script_filename, true );
#else
         chmod( script_filename.c_str( ), 0777 );
         exec_system( "./run_temp " + script_filename, true );
#endif
      }
      else
      {
         get_obj( ).Actions( "136410" );
         get_obj( ).Installed( false );
         get_obj( ).op_apply( );
      }
   }
   // [<finish Remove_impl>]
}

string Meta_Package::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Actions( ) );
      break;

      case 1:
      retval = to_string( impl_Install_Details( ) );
      break;

      case 2:
      retval = to_string( impl_Installed( ) );
      break;

      case 3:
      retval = to_string( impl_Key( ) );
      break;

      case 4:
      retval = to_string( impl_Model( ) );
      break;

      case 5:
      retval = to_string( impl_Name( ) );
      break;

      case 6:
      retval = to_string( impl_Package_Type( ) );
      break;

      case 7:
      retval = to_string( impl_Plural( ) );
      break;

      case 8:
      retval = to_string( impl_Type_Name( ) );
      break;

      case 9:
      retval = to_string( impl_Usage_Count( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }

   return retval;
}

void Meta_Package::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Package::impl, string >( *this, &Meta_Package::impl::impl_Actions, value );
      break;

      case 1:
      func_string_setter< Meta_Package::impl, string >( *this, &Meta_Package::impl::impl_Install_Details, value );
      break;

      case 2:
      func_string_setter< Meta_Package::impl, bool >( *this, &Meta_Package::impl::impl_Installed, value );
      break;

      case 3:
      func_string_setter< Meta_Package::impl, string >( *this, &Meta_Package::impl::impl_Key, value );
      break;

      case 4:
      func_string_setter< Meta_Package::impl, Meta_Model >( *this, &Meta_Package::impl::impl_Model, value );
      break;

      case 5:
      func_string_setter< Meta_Package::impl, string >( *this, &Meta_Package::impl::impl_Name, value );
      break;

      case 6:
      func_string_setter< Meta_Package::impl, Meta_Package_Type >( *this, &Meta_Package::impl::impl_Package_Type, value );
      break;

      case 7:
      func_string_setter< Meta_Package::impl, string >( *this, &Meta_Package::impl::impl_Plural, value );
      break;

      case 8:
      func_string_setter< Meta_Package::impl, string >( *this, &Meta_Package::impl::impl_Type_Name, value );
      break;

      case 9:
      func_string_setter< Meta_Package::impl, int >( *this, &Meta_Package::impl::impl_Usage_Count, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }
}

uint64_t Meta_Package::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start protect_equal)]
   if( check_equal( get_obj( ).Installed( ), true ) )
      state |= ( c_state_uneditable | c_state_undeletable );
   // [(finish protect_equal)]

   // [(start protect_equal)]
   if( check_equal( get_obj( ).Actions( ), "" ) )
      state |= ( c_state_undeletable | c_state_is_changing );
   // [(finish protect_equal)]

   // [(start modifier_field_value)]
   if( get_obj( ).Installed( ) == false )
      state |= c_modifier_Is_Not_Installed;
   // [(finish modifier_field_value)]

   // [<start get_state>]
//idk
   if( get_obj( ).Usage_Count( ) > 0 )
      state |= c_modifier_Is_In_Use;
   // [<finish get_state>]

   return state;
}

const string& Meta_Package::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Package::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Model || field == c_field_name_Model )
      impl_Model( "" );
   else if( field == c_field_id_Package_Type || field == c_field_name_Package_Type )
      impl_Package_Type( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Package::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( field == c_field_id_Model || field == c_field_name_Model )
      v_Model = value;
   else if( field == c_field_id_Package_Type || field == c_field_name_Package_Type )
      v_Package_Type = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Package::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Model || field == c_field_name_Model )
      return v_Model;
   else if( field == c_field_id_Package_Type || field == c_field_name_Package_Type )
      return v_Package_Type;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Package::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Model, v_Model ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Package_Type, v_Package_Type ) );
}

void Meta_Package::impl::clear( )
{
   v_Actions = gv_default_Actions;
   v_Install_Details = gv_default_Install_Details;
   v_Installed = gv_default_Installed;
   v_Key = gv_default_Key;
   v_Name = gv_default_Name;
   v_Plural = gv_default_Plural;
   v_Type_Name = gv_default_Type_Name;
   v_Usage_Count = gv_default_Usage_Count;

   v_Model = string( );
   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   v_Package_Type = string( );
   if( cp_Package_Type )
      p_obj->setup_foreign_key( *cp_Package_Type, v_Package_Type );
}

bool Meta_Package::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Package::impl::validate( unsigned state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   if( is_null( v_Key ) && !value_will_be_provided( c_field_name_Key ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Key,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Key ) ) ) ) );

   if( is_null( v_Name ) && !value_will_be_provided( c_field_name_Name ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Name,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Name ) ) ) ) );

   if( is_null( v_Plural ) && !value_will_be_provided( c_field_name_Plural ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Plural,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Plural ) ) ) ) );

   if( v_Model.empty( ) && !value_will_be_provided( c_field_name_Model ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Model,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Model ) ) ) ) );

   if( v_Package_Type.empty( ) && !value_will_be_provided( c_field_name_Package_Type ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Package_Type,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Package_Type ) ) ) ) );

   string error_message;
   if( !is_null( v_Name )
    && ( v_Name != gv_default_Name
    || !value_will_be_provided( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   if( !is_null( v_Plural )
    && ( v_Plural != gv_default_Plural
    || !value_will_be_provided( c_field_name_Plural ) )
    && !g_Plural_domain.is_valid( v_Plural, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Plural,
       get_module_string( c_field_display_name_Plural ) + " " + error_message ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Package::impl::after_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   if( cp_Package_Type )
      p_obj->setup_foreign_key( *cp_Package_Type, v_Package_Type );

   // [(start field_from_other_field)]
   get_obj( ).Type_Name( get_obj( ).Package_Type( ).Name( ) );
   // [(finish field_from_other_field)]

   // [(start transient_field_from_file)]
   if( !get_obj( ).get_key( ).empty( )
    && ( get_obj( ).needs_field_value( "Install_Details" )
    || required_transients.count( "Install_Details" ) ) )
      get_obj( ).Install_Details( load_file( get_obj( ).Key( ) + ".install.log", true ) );
   // [(finish transient_field_from_file)]

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Package::impl::finalise_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Package::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Package::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_from_changed_fk)]
   if( get_obj( ).get_key( ).empty( ) && get_obj( ).Package_Type( ).has_changed( ) )
      get_obj( ).Name( get_obj( ).Package_Type( ).Name( ) );
   // [(finish field_from_changed_fk)]

   // [(start field_from_changed_fk)]
   if( get_obj( ).get_key( ).empty( ) && get_obj( ).Package_Type( ).has_changed( ) )
      get_obj( ).Plural( get_obj( ).Package_Type( ).Plural( ) );
   // [(finish field_from_changed_fk)]

   // [(start default_from_key)]
   if( !get_obj( ).get_clone_key( ).empty( ) || ( is_create && is_null( get_obj( ).Key( ) ) ) )
      get_obj( ).Key( get_obj( ).get_key( ) );
   // [(finish default_from_key)]

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Package::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [<start for_store>]
//idk
   if( is_create && !get_obj( ).Package_Type( ).Multi( ) )
   {
      if( get_obj( ).Model( ).child_Package( ).iterate_forwards( ) )
      {
         do
         {
            if( get_obj( ).Package_Type( ).get_key( )
             == get_obj( ).Model( ).child_Package( ).Package_Type( ).get_key( ) )
            {
               get_obj( ).Model( ).child_Package( ).iterate_stop( );
               throw runtime_error( "This package type is singular and has already been added." ); // FUTURE: Should be a module string...
            }
         } while( get_obj( ).Model( ).child_Package( ).iterate_next( ) );
      }
   }
   // [<finish for_store>]
}

void Meta_Package::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [<start after_store>]
//idk
   if( is_create )
   {
      string key_list( get_obj( ).Package_Type( ).Name( ) + ".keys.lst" );

      ifstream inpf( key_list.c_str( ) );
      if( !inpf )
         throw runtime_error( "unable to open '" + key_list + "' for input" );

      set< string > options;

      string ext_prefix( "ext_" + lower( get_obj( ).Package_Type( ).Name( ) ) + "_" );
      string opt_prefix( "opt_" + lower( get_obj( ).Package_Type( ).Name( ) ) + "_" );

      map< string, string > package_types;
      class_pointer< Meta_Package_Type > cp_type( e_create_instance );
      if( cp_type->iterate_forwards( ) )
      {
         do
         {
            package_types.insert( make_pair( cp_type->Name( ), cp_type->get_key( ) ) );
         } while( cp_type->iterate_next( ) );
      }

      string next;
      int child_num = 0;

      while( getline( inpf, next ) )
      {
         // NOTE: In case the input file had been treated as binary during an FTP remove trailing CR.
         if( next.size( ) && next[ next.size( ) - 1 ] == '\r' )
            next.erase( next.size( ) - 1 );

         if( next.find( ext_prefix ) == 0 )
         {
            string::size_type pos = next.find( '=' );
            if( pos != string::npos )
            {
               string opt_name = next.substr( 0, pos );
               if( !options.count( opt_name ) )
               {
                  get_obj( ).child_Package_Option( ).op_create( construct_key_from_int( get_obj( ).get_key( ), ++child_num ) );

                  get_obj( ).child_Package_Option( ).Package( get_obj( ).get_key( ) );

                  get_obj( ).child_Package_Option( ).Name( opt_name );
                  get_obj( ).child_Package_Option( ).Is_Other_Package( true );
                  get_obj( ).child_Package_Option( ).Other_Package_Type( package_types[ next.substr( pos + 1 ) ] );

                  get_obj( ).child_Package_Option( ).Other_Package_Required( true );

                  get_obj( ).child_Package_Option( ).op_apply( );

                  options.insert( opt_name );
               }
            }
         }
         else if( next.find( opt_prefix ) == 0 )
         {
            string::size_type pos = next.find( '=' );
            if( pos != string::npos )
            {
               string opt_name = next.substr( 0, pos );
               if( !options.count( opt_name ) )
               {
                  get_obj( ).child_Package_Option( ).op_create( construct_key_from_int( get_obj( ).get_key( ), ++child_num ) );

                  get_obj( ).child_Package_Option( ).Package( get_obj( ).get_key( ) );

                  get_obj( ).child_Package_Option( ).Name( opt_name );

                  string value = next.substr( pos + 1 );
                  if( value.empty( ) || value == "1" )
                     get_obj( ).child_Package_Option( ).Use_Option( value == "1" );
                  else
                  {
                     get_obj( ).child_Package_Option( ).Is_Other_Package( true );
                     get_obj( ).child_Package_Option( ).Other_Package_Type( package_types[ value ] );
                  }

                  get_obj( ).child_Package_Option( ).op_apply( );

                  options.insert( opt_name );
               }
            }   
         }
      }
   }
   else if( get_obj( ).has_field_changed( c_field_id_Installed ) )
   {
      if( get_obj( ).child_Package_Option( ).iterate_forwards( ) )
      {
         do
         {
            get_obj( ).child_Package_Option( ).op_update( );
            get_obj( ).child_Package_Option( ).Installed( get_obj( ).Installed( ) );
            get_obj( ).child_Package_Option( ).op_apply( );
         } while( get_obj( ).child_Package_Option( ).iterate_next( ) );
      }
   }
   // [<finish after_store>]
}

bool Meta_Package::impl::can_destroy( bool is_internal )
{
   bool retval = is_internal || !( get_state( ) & c_state_undeletable );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Package::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Package::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start after_destroy>]
//idk
   string install_log( get_obj( ).get_key( ) + ".install.log" );
   string map_filename( get_obj( ).get_attached_file_path( get_obj( ).get_key( ) + ".map" ) );
   string new_filename( get_obj( ).get_attached_file_path( get_obj( ).get_key( ) + ".new" ) );
   string map_new_filename( get_obj( ).get_attached_file_path( get_obj( ).get_key( ) + ".map.new" ) );

   if( exists_file( install_log ) )
      remove_file( install_log );

   if( exists_file( map_filename ) )
      remove_file( map_filename );

   if( exists_file( new_filename ) )
      remove_file( new_filename );

   if( exists_file( map_new_filename ) )
      remove_file( map_new_filename );
   // [<finish after_destroy>]
}

void Meta_Package::impl::set_default_values( )
{
   clear( );
}

bool Meta_Package::impl::is_filtered( ) const
{
   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

#undef MODULE_TRACE
#define MODULE_TRACE( x ) trace( x )

Meta_Package::Meta_Package( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Package::~Meta_Package( )
{
   cleanup( );
   delete p_impl;
}

const string& Meta_Package::Actions( ) const
{
   return p_impl->impl_Actions( );
}

void Meta_Package::Actions( const string& Actions )
{
   p_impl->impl_Actions( Actions );
}

const string& Meta_Package::Install_Details( ) const
{
   return p_impl->impl_Install_Details( );
}

void Meta_Package::Install_Details( const string& Install_Details )
{
   p_impl->impl_Install_Details( Install_Details );
}

bool Meta_Package::Installed( ) const
{
   return p_impl->impl_Installed( );
}

void Meta_Package::Installed( bool Installed )
{
   p_impl->impl_Installed( Installed );
}

const string& Meta_Package::Key( ) const
{
   return p_impl->impl_Key( );
}

void Meta_Package::Key( const string& Key )
{
   p_impl->impl_Key( Key );
}

const string& Meta_Package::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_Package::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

const string& Meta_Package::Plural( ) const
{
   return p_impl->impl_Plural( );
}

void Meta_Package::Plural( const string& Plural )
{
   p_impl->impl_Plural( Plural );
}

const string& Meta_Package::Type_Name( ) const
{
   return p_impl->impl_Type_Name( );
}

void Meta_Package::Type_Name( const string& Type_Name )
{
   p_impl->impl_Type_Name( Type_Name );
}

int Meta_Package::Usage_Count( ) const
{
   return p_impl->impl_Usage_Count( );
}

void Meta_Package::Usage_Count( int Usage_Count )
{
   p_impl->impl_Usage_Count( Usage_Count );
}

Meta_Model& Meta_Package::Model( )
{
   return p_impl->impl_Model( );
}

const Meta_Model& Meta_Package::Model( ) const
{
   return p_impl->impl_Model( );
}

void Meta_Package::Model( const string& key )
{
   p_impl->impl_Model( key );
}

Meta_Package_Type& Meta_Package::Package_Type( )
{
   return p_impl->impl_Package_Type( );
}

const Meta_Package_Type& Meta_Package::Package_Type( ) const
{
   return p_impl->impl_Package_Type( );
}

void Meta_Package::Package_Type( const string& key )
{
   p_impl->impl_Package_Type( key );
}

Meta_Package_Option& Meta_Package::child_Package_Option_Other( )
{
   return p_impl->impl_child_Package_Option_Other( );
}

const Meta_Package_Option& Meta_Package::child_Package_Option_Other( ) const
{
   return p_impl->impl_child_Package_Option_Other( );
}

Meta_Workgroup& Meta_Package::child_Workgroup_Standard( )
{
   return p_impl->impl_child_Workgroup_Standard( );
}

const Meta_Workgroup& Meta_Package::child_Workgroup_Standard( ) const
{
   return p_impl->impl_child_Workgroup_Standard( );
}

Meta_Package_Option& Meta_Package::child_Package_Option( )
{
   return p_impl->impl_child_Package_Option( );
}

const Meta_Package_Option& Meta_Package::child_Package_Option( ) const
{
   return p_impl->impl_child_Package_Option( );
}

void Meta_Package::Cancel_Remove(  )
{
   p_impl->impl_Cancel_Remove(  );
}

void Meta_Package::Check_Install(  )
{
   p_impl->impl_Check_Install(  );
}

void Meta_Package::Complete_Remove(  )
{
   p_impl->impl_Complete_Remove(  );
}

void Meta_Package::Install(  )
{
   p_impl->impl_Install(  );
}

void Meta_Package::Remove(  )
{
   p_impl->impl_Remove(  );
}

string Meta_Package::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Package::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

bool Meta_Package::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Package::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Package::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Package::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Package::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Package::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Package::clear( )
{
   p_impl->clear( );
}

void Meta_Package::validate( unsigned state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Package::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Package::finalise_fetch( )
{
   p_impl->finalise_fetch( );
}

void Meta_Package::at_create( )
{
   p_impl->at_create( );
}

void Meta_Package::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Package::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Package::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Package::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Package::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Package::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Package::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Package::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Package::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( 0 );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Actions )
   {
      p_id = c_field_id_Actions;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Install_Details )
   {
      p_id = c_field_id_Install_Details;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Installed )
   {
      p_id = c_field_id_Installed;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Key )
   {
      p_id = c_field_id_Key;

      if( p_type_name )
         *p_type_name = "string";

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
   else if( name == c_field_name_Name )
   {
      p_id = c_field_id_Name;

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
   else if( name == c_field_name_Plural )
   {
      p_id = c_field_id_Plural;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Type_Name )
   {
      p_id = c_field_id_Type_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Usage_Count )
   {
      p_id = c_field_id_Usage_Count;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_id;
}

const char* Meta_Package::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( 0 );

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Actions )
   {
      p_name = c_field_name_Actions;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Install_Details )
   {
      p_name = c_field_name_Install_Details;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Installed )
   {
      p_name = c_field_name_Installed;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Key )
   {
      p_name = c_field_name_Key;

      if( p_type_name )
         *p_type_name = "string";

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
   else if( id == c_field_id_Name )
   {
      p_name = c_field_name_Name;

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
   else if( id == c_field_id_Plural )
   {
      p_name = c_field_name_Plural;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Type_Name )
   {
      p_name = c_field_name_Type_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Usage_Count )
   {
      p_name = c_field_name_Usage_Count;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_name;
}

string Meta_Package::get_field_display_name( const string& id ) const
{
   string display_name;

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_display_name" );
   else if( id == c_field_id_Actions )
      display_name = get_module_string( c_field_display_name_Actions );
   else if( id == c_field_id_Install_Details )
      display_name = get_module_string( c_field_display_name_Install_Details );
   else if( id == c_field_id_Installed )
      display_name = get_module_string( c_field_display_name_Installed );
   else if( id == c_field_id_Key )
      display_name = get_module_string( c_field_display_name_Key );
   else if( id == c_field_id_Model )
      display_name = get_module_string( c_field_display_name_Model );
   else if( id == c_field_id_Name )
      display_name = get_module_string( c_field_display_name_Name );
   else if( id == c_field_id_Package_Type )
      display_name = get_module_string( c_field_display_name_Package_Type );
   else if( id == c_field_id_Plural )
      display_name = get_module_string( c_field_display_name_Plural );
   else if( id == c_field_id_Type_Name )
      display_name = get_module_string( c_field_display_name_Type_Name );
   else if( id == c_field_id_Usage_Count )
      display_name = get_module_string( c_field_display_name_Usage_Count );

   return display_name;
}

void Meta_Package::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Package::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Package::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Package::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Package::setup_foreign_key( Meta_Model& o, const string& value )
{
   static_cast< Meta_Model& >( o ).set_key( value );
}

void Meta_Package::setup_foreign_key( Meta_Package_Type& o, const string& value )
{
   static_cast< Meta_Package_Type& >( o ).set_key( value );
}

void Meta_Package::setup_graph_parent( Meta_Package_Option& o, const string& foreign_key_field )
{
   static_cast< Meta_Package_Option& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Package::setup_graph_parent( Meta_Workgroup& o, const string& foreign_key_field )
{
   static_cast< Meta_Workgroup& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Package::setup_graph_parent(
 Meta_Model& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Model& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Model& >( o ).set_key( init_value );
}

void Meta_Package::setup_graph_parent(
 Meta_Package_Type& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Package_Type& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Package_Type& >( o ).set_key( init_value );
}

size_t Meta_Package::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Package::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Package::get_num_foreign_key_children( bool is_internal ) const
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

class_base* Meta_Package::get_next_foreign_key_child(
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
         if( op == e_cascade_op_unlink )
         {
            next_child_field = "302830";
            p_class_base = &child_Package_Option_Other( );
         }
         break;

         case 1:
         if( op == e_cascade_op_unlink )
         {
            next_child_field = "302835";
            p_class_base = &child_Workgroup_Standard( );
         }
         break;

         case 2:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302820";
            p_class_base = &child_Package_Option( );
         }
         break;
      }
   }

   return p_class_base;
}

const char* Meta_Package::class_id( ) const
{
   return static_class_id( );
}

const char* Meta_Package::class_name( ) const
{
   return static_class_name( );
}

const char* Meta_Package::plural_name( ) const
{
   return static_plural_name( );
}

const char* Meta_Package::module_id( ) const
{
   return static_module_id( );
}

const char* Meta_Package::module_name( ) const
{
   return static_module_name( );
}

string Meta_Package::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );
   key += "package";

   return get_module_string( key );
}

bool Meta_Package::get_is_alias( ) const
{
   return false;
}

void Meta_Package::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Package::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Package::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( sub_context == "_302830" || sub_context == "child_Package_Option_Other" )
      p_class_base = &child_Package_Option_Other( );
   else if( sub_context == "_302835" || sub_context == "child_Workgroup_Standard" )
      p_class_base = &child_Workgroup_Standard( );
   else if( sub_context == "_302820" || sub_context == "child_Package_Option" )
      p_class_base = &child_Package_Option( );
   else if( sub_context == c_field_id_Model || sub_context == c_field_name_Model )
      p_class_base = &Model( );
   else if( sub_context == c_field_id_Package_Type || sub_context == c_field_name_Package_Type )
      p_class_base = &Package_Type( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Package::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Actions" );
   names.push_back( "C_Installed" );
   names.push_back( "C_Key" );
   names.push_back( "C_Model" );
   names.push_back( "C_Name" );
   names.push_back( "C_Package_Type" );
   names.push_back( "C_Plural" );
   names.push_back( "C_Usage_Count" );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Package::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Actions( ) ) ) );
   values.push_back( to_string( Installed( ) ) );
   values.push_back( sql_quote( to_string( Key( ) ) ) );
   values.push_back( sql_quote( to_string( Model( ) ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( sql_quote( to_string( Package_Type( ) ) ) );
   values.push_back( sql_quote( to_string( Plural( ) ) ) );
   values.push_back( to_string( Usage_Count( ) ) );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Package::get_required_field_names(
 set< string >& names, bool required_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, required_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Package::get_always_required_field_names(
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

   // [(start protect_equal)]
   dependents.insert( "Actions" );

   if( ( required_transients && is_field_transient( e_field_id_Actions ) )
    || ( !required_transients && !is_field_transient( e_field_id_Actions ) ) )
      names.insert( "Actions" );
   // [(finish protect_equal)]

   // [(start modifier_field_value)]
   dependents.insert( "Installed" ); // (for Is_Not_Installed modifier)

   if( ( required_transients && is_field_transient( e_field_id_Installed ) )
    || ( !required_transients && !is_field_transient( e_field_id_Installed ) ) )
      names.insert( "Installed" );
   // [(finish modifier_field_value)]

   // [<start get_always_required_field_names>]
//idk
   dependents.insert( "Usage_Count" ); // (for Is_In_Use modifier)

   if( ( required_transients && is_field_transient( e_field_id_Usage_Count ) )
    || ( !required_transients && !is_field_transient( e_field_id_Usage_Count ) ) )
      names.insert( "Usage_Count" );
   // [<finish get_always_required_field_names>]
}

void Meta_Package::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Package::do_generate_sql( generate_sql_type type, vector< string >& sql_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts );
}

const char* Meta_Package::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Package::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Package::static_lock_class_id( )
{
   return "136100";
}

const char* Meta_Package::static_check_class_name( )
{
   return "Package";
}

bool Meta_Package::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Package::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.136100" );
}

void Meta_Package::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "136102", "Actions", "string", false ) );
   all_field_info.push_back( field_info( "136104", "Install_Details", "string", false ) );
   all_field_info.push_back( field_info( "136103", "Installed", "bool", false ) );
   all_field_info.push_back( field_info( "136105", "Key", "string", false ) );
   all_field_info.push_back( field_info( "302800", "Model", "Meta_Model", true ) );
   all_field_info.push_back( field_info( "136101", "Name", "string", false ) );
   all_field_info.push_back( field_info( "302810", "Package_Type", "Meta_Package_Type", true ) );
   all_field_info.push_back( field_info( "136107", "Plural", "string", false ) );
   all_field_info.push_back( field_info( "136108", "Type_Name", "string", false ) );
   all_field_info.push_back( field_info( "136106", "Usage_Count", "int", false ) );
}

void Meta_Package::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Model, make_pair( "Meta.136100", "Meta_Model" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Package_Type, make_pair( "Meta.136100", "Meta_Package_Type" ) ) );
}

int Meta_Package::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Package::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Package::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "136102";
      break;

      case 2:
      p_id = "136104";
      break;

      case 3:
      p_id = "136103";
      break;

      case 4:
      p_id = "136105";
      break;

      case 5:
      p_id = "302800";
      break;

      case 6:
      p_id = "136101";
      break;

      case 7:
      p_id = "302810";
      break;

      case 8:
      p_id = "136107";
      break;

      case 9:
      p_id = "136108";
      break;

      case 10:
      p_id = "136106";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Package" );

   return p_id;
}

const char* Meta_Package::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Actions";
      break;

      case 2:
      p_id = "Install_Details";
      break;

      case 3:
      p_id = "Installed";
      break;

      case 4:
      p_id = "Key";
      break;

      case 5:
      p_id = "Model";
      break;

      case 6:
      p_id = "Name";
      break;

      case 7:
      p_id = "Package_Type";
      break;

      case 8:
      p_id = "Plural";
      break;

      case 9:
      p_id = "Type_Name";
      break;

      case 10:
      p_id = "Usage_Count";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Package" );

   return p_id;
}

int Meta_Package::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Actions || field == c_field_name_Actions )
      rc += 1;
   else if( field == c_field_id_Install_Details || field == c_field_name_Install_Details )
      rc += 2;
   else if( field == c_field_id_Installed || field == c_field_name_Installed )
      rc += 3;
   else if( field == c_field_id_Key || field == c_field_name_Key )
      rc += 4;
   else if( field == c_field_id_Model || field == c_field_name_Model )
      rc += 5;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 6;
   else if( field == c_field_id_Package_Type || field == c_field_name_Package_Type )
      rc += 7;
   else if( field == c_field_id_Plural || field == c_field_name_Plural )
      rc += 8;
   else if( field == c_field_id_Type_Name || field == c_field_name_Type_Name )
      rc += 9;
   else if( field == c_field_id_Usage_Count || field == c_field_name_Usage_Count )
      rc += 10;

   return rc - 1;
}

procedure_info_container& Meta_Package::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "136450", "Cancel_Remove" ) );
      procedures.insert( make_pair( "136430", "Check_Install" ) );
      procedures.insert( make_pair( "136440", "Complete_Remove" ) );
      procedures.insert( make_pair( "136410", "Install" ) );
      procedures.insert( make_pair( "136420", "Remove" ) );
   }

   return procedures;
}

string Meta_Package::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(64),"
    "C_Ver_ INTEGER NOT NULL,"
    "C_Rev_ INTEGER NOT NULL,"
    "C_Typ_ VARCHAR(24) NOT NULL,"
    "C_Actions VARCHAR(128) NOT NULL,"
    "C_Installed INTEGER NOT NULL,"
    "C_Key VARCHAR(128) NOT NULL,"
    "C_Model VARCHAR(64) NOT NULL,"
    "C_Name VARCHAR(128) NOT NULL,"
    "C_Package_Type VARCHAR(64) NOT NULL,"
    "C_Plural VARCHAR(128) NOT NULL,"
    "C_Usage_Count INTEGER NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Package::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Package::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   ( void )pairs;
}

void Meta_Package::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Model,C_Name" );
   indexes.push_back( "C_Model,C_Package_Type,C_Name" );
}

void Meta_Package::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Model,C_Name" );
   indexes.push_back( "C_Model,C_Package_Type,C_Name" );
}

void Meta_Package::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Package::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Package::static_insert_external_alias( const string& module_and_class_id, Meta_Package* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Package::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Package::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Package::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}

