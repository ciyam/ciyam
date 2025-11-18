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

#include "Meta_Package.h"

#include "Meta_Package_Option.h"
#include "Meta_Workgroup.h"
#include "Meta_Model.h"
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
//nyi
#include <sys/stat.h>
#include "module_management.h"

#include "Meta_Enum.h"
#include "Meta_List.h"
#include "Meta_Type.h"
#include "Meta_View.h"
#include "Meta_Class.h"
#include "Meta_Field.h"
#include "Meta_Index.h"
#include "Meta_Modifier.h"
#include "Meta_Procedure.h"
#include "Meta_Workgroup.h"
#include "Meta_Relationship.h"
#include "Meta_Specification.h"
#include "Meta_Initial_Record.h"
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

template< typename T > inline void sanity_check( const T& t ) { }

inline void sanity_check( const string& s )
{
   if( s.length( ) > c_max_string_length_limit )
      throw runtime_error( "unexpected max string length limit exceeded with: " + s );
}

#include "Meta_Package.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Actions = "136102";
const char* const c_field_id_Install_Details = "136104";
const char* const c_field_id_Installed = "136103";
const char* const c_field_id_Installed_Order = "136109";
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
const char* const c_field_name_Installed_Order = "Installed_Order";
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
const char* const c_field_display_name_Installed_Order = "field_package_installed_order";
const char* const c_field_display_name_Key = "field_package_key";
const char* const c_field_display_name_Model = "field_package_model";
const char* const c_field_display_name_Name = "field_package_name";
const char* const c_field_display_name_Package_Type = "field_package_package_type";
const char* const c_field_display_name_Plural = "field_package_plural";
const char* const c_field_display_name_Type_Name = "field_package_type_name";
const char* const c_field_display_name_Usage_Count = "field_package_usage_count";

const int c_num_fields = 11;

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
   "136109",
   "302800",
   "302810"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Actions",
   "Install_Details",
   "Installed",
   "Installed_Order",
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

const int c_num_encrypted_fields = 0;

bool is_encrypted_field( const string& ) { static bool false_value( false ); return false_value; }

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

inline bool is_transient_field( const string& field )
{
   return binary_search( c_transient_sorted_field_ids,
    c_transient_sorted_field_ids + c_num_transient_fields, field.c_str( ), compare )
    || binary_search( c_transient_sorted_field_names,
    c_transient_sorted_field_names + c_num_transient_fields, field.c_str( ), compare );
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

string g_group_field_name;
string g_level_field_name;
string g_order_field_name;
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_Package* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Package* > external_aliases_lookup_container;
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

string g_default_Actions = string( "136410" );
string g_default_Install_Details = string( );
bool g_default_Installed = bool( 0 );
string g_default_Installed_Order = string( );
string g_default_Key = string( );
string g_default_Model = string( );
string g_default_Name = string( );
string g_default_Package_Type = string( );
string g_default_Plural = string( );
string g_default_Type_Name = string( );
int g_default_Usage_Count = int( 0 );

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

   void handle_unknown_command( const string& command, const string& cmd_and_args )
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
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_Package_key_fixed ) );

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
      string field_name( get_parm_val( parameters, c_cmd_Meta_Package_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Actions ) || ( field_name == c_field_name_Actions ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package->Actions( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Install_Details ) || ( field_name == c_field_name_Install_Details ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package->Install_Details( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Installed ) || ( field_name == c_field_name_Installed ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package->Installed( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Installed_Order ) || ( field_name == c_field_name_Installed_Order ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package->Installed_Order( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Key ) || ( field_name == c_field_name_Key ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package->Key( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) ) )
      {
         handled = true;

         string_getter< Meta_Model >( cmd_handler.p_Meta_Package->Model( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package->Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Package_Type ) || ( field_name == c_field_name_Package_Type ) ) )
      {
         handled = true;

         string_getter< Meta_Package_Type >( cmd_handler.p_Meta_Package->Package_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Plural ) || ( field_name == c_field_name_Plural ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package->Plural( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Type_Name ) || ( field_name == c_field_name_Type_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package->Type_Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Usage_Count ) || ( field_name == c_field_name_Usage_Count ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Package->Usage_Count( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Package_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Package_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_Package_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Actions ) || ( field_name == c_field_name_Actions ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package, string >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Actions, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Install_Details ) || ( field_name == c_field_name_Install_Details ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package, string >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Install_Details, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Installed ) || ( field_name == c_field_name_Installed ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package, bool >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Installed, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Installed_Order ) || ( field_name == c_field_name_Installed_Order ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package, string >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Installed_Order, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Key ) || ( field_name == c_field_name_Key ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package, string >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Key, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package, Meta_Model >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Model, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package, string >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Package_Type ) || ( field_name == c_field_name_Package_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package, Meta_Package_Type >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Package_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Plural ) || ( field_name == c_field_name_Plural ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package, string >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Plural, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Type_Name ) || ( field_name == c_field_name_Type_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package, string >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Type_Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Usage_Count ) || ( field_name == c_field_name_Usage_Count ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package, int >(
          *cmd_handler.p_Meta_Package, &Meta_Package::Usage_Count, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Package_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Package_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_Package_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) )
         cmd_handler.retval = cmd_handler.p_Meta_Package->Model( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Package_Type ) || ( field_name == c_field_name_Package_Type ) )
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
   void impl_Actions( const string& Actions ) { sanity_check( Actions ); v_Actions = Actions; }

   const string& impl_Install_Details( ) const { return lazy_fetch( p_obj ), v_Install_Details; }
   void impl_Install_Details( const string& Install_Details ) { sanity_check( Install_Details ); v_Install_Details = Install_Details; }

   bool impl_Installed( ) const { return lazy_fetch( p_obj ), v_Installed; }
   void impl_Installed( bool Installed ) { v_Installed = Installed; }

   const string& impl_Installed_Order( ) const { return lazy_fetch( p_obj ), v_Installed_Order; }
   void impl_Installed_Order( const string& Installed_Order ) { sanity_check( Installed_Order ); v_Installed_Order = Installed_Order; }

   const string& impl_Key( ) const { return lazy_fetch( p_obj ), v_Key; }
   void impl_Key( const string& Key ) { sanity_check( Key ); v_Key = Key; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { sanity_check( Name ); v_Name = Name; }

   const string& impl_Plural( ) const { return lazy_fetch( p_obj ), v_Plural; }
   void impl_Plural( const string& Plural ) { sanity_check( Plural ); v_Plural = Plural; }

   const string& impl_Type_Name( ) const { return lazy_fetch( p_obj ), v_Type_Name; }
   void impl_Type_Name( const string& Type_Name ) { sanity_check( Type_Name ); v_Type_Name = Type_Name; }

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

   Meta_Package* p_obj;
   class_pointer< Meta_Package > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   string v_Actions;
   string v_Install_Details;
   bool v_Installed;
   string v_Installed_Order;
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
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Cancel_Remove_impl>]
//nyi
   if( !storage_locked_for_admin( ) )
   {
      get_obj( ).op_update( );
      get_obj( ).Actions( "136420" );
      get_obj( ).op_apply( );
   }

   string model_key( "Meta_Model_" + get_obj( ).Model( ).get_key( ) );

   if( get_system_variable( "@" + model_key ).empty( ) )
      set_system_variable( model_key, "" );
   // [<finish Cancel_Remove_impl>]
}

void Meta_Package::impl::impl_Check_Install( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Check_Install_impl>]
//nyi
   string map_filename( get_obj( ).get_attached_file_path( get_obj( ).get_key( ) + ".map" ) );
   string new_filename( get_obj( ).get_attached_file_path( get_obj( ).get_key( ) + ".new" ) );

   bool was_okay = false;

   get_obj( ).op_update( );

   if( storage_locked_for_admin( ) )
      was_okay = exists_file( map_filename );
   else
   {
      string temp_name( get_session_variable( get_special_var_name( e_special_var_package ) ) );

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

            string demo_keys_ext( c_demo_keys_suffix );

            string demo_keys_src( get_obj( ).Package_Type( ).Single( ) + demo_keys_ext );
            string demo_keys_dest( get_obj( ).Model( ).Name( ) + demo_keys_ext );

            if( exists_file( demo_keys_src ) )
               append_file( demo_keys_src, demo_keys_dest );
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
      get_obj( ).Installed_Order( get_dtm( ) );

      if( get_obj( ).Name( ) == "Standard" && is_null( get_obj( ).Model( ).Workgroup( ).Standard_Package( ) ) )
      {
         get_obj( ).Model( ).Workgroup( ).op_update( );
         get_obj( ).Model( ).Workgroup( ).Standard_Package( get_obj( ).get_key( ) );
         get_obj( ).Model( ).Workgroup( ).op_apply( );
      }
   }

   get_obj( ).op_apply( );

   string model_key( "Meta_Model_" + get_obj( ).Model( ).get_key( ) );

   if( get_system_variable( "@" + model_key ).empty( ) )
      set_system_variable( model_key, "" );
   // [<finish Check_Install_impl>]
}

void Meta_Package::impl::impl_Complete_Remove( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Complete_Remove_impl>]
//nyi
   get_obj( ).op_update( );

   get_obj( ).Actions( "136410" );
   get_obj( ).Installed( false );
   get_obj( ).Installed_Order( "" );

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

   if( get_system_variable( "@" + model_key ).empty( ) )
      set_system_variable( model_key, "" );
   // [<finish Complete_Remove_impl>]
}

void Meta_Package::impl::impl_Install( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Install_impl>]
//nyi
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

      string install_log( get_obj( ).get_key( ) + ".install.log" );

      ofstream outl( install_log.c_str( ) );

      if( !outl )
         throw runtime_error( "unable to open '" + install_log + "' for output" );

      // FUTURE: This message should be handled as a server string message.
      outl << "Linked to existing Standard package...";

      string model_key( "Meta_Model_" + get_obj( ).Model( ).get_key( ) );

      if( get_system_variable( "@" + model_key ).empty( ) )
         set_system_variable( model_key, "" );
   }
   else if( !storage_locked_for_admin( ) )
   {
      vector< string > dependencies;
      map< string, string > installed_types;

      // NOTE: Because "Remove_All_Packages" relies upon "Installed_Order" to determine
      // the package installation ordering (and its value comes from "get_dtm") a delay
      // of 1/10 of a second is here to ensure that this ordering will be unambiguous.
      msleep( 100 );

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
               {
                  string type_name( get_obj( ).Model( ).child_Package( ).Package_Type( ).Name( ) );

                  installed_types.insert( make_pair( type_name, get_obj( ).Model( ).child_Package( ).get_key( ) ) );
               }
            }
         } while( get_obj( ).Model( ).child_Package( ).iterate_next( ) );
      }

      string deps( get_obj( ).Package_Type( ).Dependencies( ) );

      deps = search_replace( deps, "\n", "," );

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
               // FUTURE: This message should be handled as a server string message.
               throw runtime_error( "This package requires '" + next + "' to first be installed." );

            dependency_keys.push_back( installed_types[ next ] );
         }
      }

      if( get_obj( ).child_Package_Option( ).iterate_forwards( ) )
      {
         do
         {
            // FUTURE: The following exceptions should be handled as a server string messages.

            if( get_obj( ).child_Package_Option( ).Is_Class( )
             && get_obj( ).child_Package_Option( ).Is_Mandatory_Class( )
             && get_obj( ).child_Package_Option( ).Value( ).empty( ) )
               throw runtime_error( "This package requires option '"
                + get_obj( ).child_Package_Option( ).Name( ) + "' to be provided." );
            else if( get_obj( ).child_Package_Option( ).Is_Class( )
             && get_obj( ).child_Package_Option( ).Is_Mandatory_View( )
             && is_null( get_obj( ).child_Package_Option( ).View( ) ) )
               throw runtime_error( "This package option '"
                + get_obj( ).child_Package_Option( ).Name( ) + "' requires a View." );
            else if( get_obj( ).child_Package_Option( ).Is_Class( )
             && get_obj( ).child_Package_Option( ).Is_Mandatory_List( )
             && is_null( get_obj( ).child_Package_Option( ).List( ) ) )
               throw runtime_error( "This package option '"
                + get_obj( ).child_Package_Option( ).Name( ) + "' requires a List." );
            else if( get_obj( ).child_Package_Option( ).Is_Class( )
             && get_obj( ).child_Package_Option( ).Is_Mandatory_Field( )
             && is_null( get_obj( ).child_Package_Option( ).Field( ) ) )
               throw runtime_error( "The package option '"
                + get_obj( ).child_Package_Option( ).Name( ) + "' requires a Field." );
            else if( get_obj( ).child_Package_Option( ).Is_Class( )
             && get_obj( ).child_Package_Option( ).Is_Mandatory_Other_Field( )
             && is_null( get_obj( ).child_Package_Option( ).Other_Field( ) ) )
               throw runtime_error( "The package option '"
                + get_obj( ).child_Package_Option( ).Name( ) + "' requires Other Field." );
            else if( get_obj( ).child_Package_Option( ).Is_Class( )
             && get_obj( ).child_Package_Option( ).Is_Mandatory_Other_Field_2( )
             && is_null( get_obj( ).child_Package_Option( ).Other_Field_2( ) ) )
               throw runtime_error( "The package option '"
                + get_obj( ).child_Package_Option( ).Name( ) + "' requires Other Field 2." );
            else if( get_obj( ).child_Package_Option( ).Is_Class( )
             && get_obj( ).child_Package_Option( ).Is_Mandatory_Source_Field( )
             && is_null( get_obj( ).child_Package_Option( ).Source_Field( ) ) )
               throw runtime_error( "The package option '"
                + get_obj( ).child_Package_Option( ).Name( ) + "' requires Source Field." );
            else if( get_obj( ).child_Package_Option( ).Is_Class( )
             && get_obj( ).child_Package_Option( ).Is_Mandatory_Other_Source_Field( )
             && is_null( get_obj( ).child_Package_Option( ).Other_Source_Field( ) ) )
               throw runtime_error( "The package option '"
                + get_obj( ).child_Package_Option( ).Name( ) + "' requires Other Source Field." );
            else if( get_obj( ).child_Package_Option( ).Is_Class( )
             && get_obj( ).child_Package_Option( ).Is_Mandatory_Modifier( )
             && is_null( get_obj( ).child_Package_Option( ).Modifier( ) ) )
               throw runtime_error( "The package option '"
                + get_obj( ).child_Package_Option( ).Name( ) + "' requires a Modifier." );
            else if( get_obj( ).child_Package_Option( ).Is_Class( )
             && get_obj( ).child_Package_Option( ).Is_Mandatory_Procedure( )
             && is_null( get_obj( ).child_Package_Option( ).Procedure( ) ) )
               throw runtime_error( "The package option '"
                + get_obj( ).child_Package_Option( ).Name( ) + "' requires a Procedure." );
            else if( get_obj( ).child_Package_Option( ).Is_Other_Package( )
             && get_obj( ).child_Package_Option( ).Other_Package_Required( )
             && is_null( get_obj( ).child_Package_Option( ).Other_Package( ) ) )
               throw runtime_error( "This package requires option '"
                + get_obj( ).child_Package_Option( ).Name( ) + "' to be provided." );
            else if( get_obj( ).child_Package_Option( ).Is_Other_Package( )
             && !is_null( get_obj( ).child_Package_Option( ).Other_Package( ) ) )
            {
               string other_type_name( get_obj( ).child_Package_Option( ).Other_Package( ).Package_Type( ).Name( ) );

               if( !installed_types.count( other_type_name ) )
                  throw runtime_error( "This package requires '"
                   + get_obj( ).child_Package_Option( ).Other_Package( ).Name( ) + "' to first be installed." );

               dependency_keys.push_back( get_obj( ).child_Package_Option( ).Other_Package( ).get_key( ) );
            }

         } while( get_obj( ).child_Package_Option( ).iterate_next( ) );
      }

      string temp_name( "~" + get_uuid( ) );
      string list_filename( temp_name + ".lst" );

      string script_filename( temp_name );
      string commands_filename( temp_name + ".cin" );

      string type_name( get_obj( ).Package_Type( ).Name( ) );
      string keys_filename( type_name + ".keys.lst" );

      string standard_client_args( "-quiet -no_prompt" );

      string rpc_password( get_rpc_password( ) );

      if( !rpc_password.empty( ) )
         standard_client_args += " -rpc_unlock=" + rpc_password;

      bool async = true;

      if( get_obj( ).get_variable( get_special_var_name( e_special_var_async ) ) == "0"
       || get_obj( ).get_variable( get_special_var_name( e_special_var_async ) ) == "false"
       || get_session_variable( get_special_var_name( e_special_var_allow_async ) ) == "0"
       || get_session_variable( get_special_var_name( e_special_var_allow_async ) ) == "false" )
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

         if( get_obj( ).Name( ) != get_obj( ).Package_Type( ).Single( ) )
            outf << get_obj( ).Package_Type( ).Single( ) << "=" << get_obj( ).Name( ) << '\n';

         if( get_obj( ).Plural( ) != get_obj( ).Package_Type( ).Plural( ) )
            outf << get_obj( ).Package_Type( ).Plural( ) << "=" << get_obj( ).Plural( ) << '\n';

         map< string, string > extras;
         map< string, string > options;

         if( get_obj( ).child_Package_Option( ).iterate_forwards( ) )
         {
            do
            {
               if( !get_obj( ).child_Package_Option( ).Is_Class( )
                && get_obj( ).child_Package_Option( ).Primitive( ) == e_primitive_bool )
                  options.insert( make_pair(
                   get_obj( ).child_Package_Option( ).Id( ), get_obj( ).child_Package_Option( ).Use_Option( ) ? "1" : "" ) );
               else
               {
                  options.insert( make_pair(
                   get_obj( ).child_Package_Option( ).Id( ), get_obj( ).child_Package_Option( ).Value( ) ) );

                  if( get_obj( ).child_Package_Option( ).Is_Class( )
                   && !is_null( get_obj( ).child_Package_Option( ).Class( ) ) )
                  {
                     string name( get_obj( ).child_Package_Option( ).Id( ) );

                     string::size_type pos = name.rfind( "_class" );

                     if( pos == string::npos )
                        throw runtime_error( "unexpected class option id name '" + name + "'" );

                     name.erase( pos + 1 );

                     extras.insert( make_pair(
                      name + "clskey", get_obj( ).child_Package_Option( ).Class( ).get_key( ) ) );

                     extras.insert( make_pair(
                      name + "plural", get_obj( ).child_Package_Option( ).Class( ).Plural( ) ) );

                     extras.insert( make_pair(
                      name + "vewkey", get_obj( ).child_Package_Option( ).View( ).get_key( ) ) );

                     extras.insert( make_pair(
                      name + "lstkey", get_obj( ).child_Package_Option( ).List( ).get_key( ) ) );

                     extras.insert( make_pair(
                      name + "fldnam", get_obj( ).child_Package_Option( ).Field( ).Name( ) ) );

                     extras.insert( make_pair(
                      name + "fldkey", get_obj( ).child_Package_Option( ).Field( ).get_key( ) ) );

                     extras.insert( make_pair(
                      name + "oflnam", get_obj( ).child_Package_Option( ).Other_Field( ).Name( ) ) );

                     extras.insert( make_pair(
                      name + "oflkey", get_obj( ).child_Package_Option( ).Other_Field( ).get_key( ) ) );

                     extras.insert( make_pair(
                      name + "of2nam", get_obj( ).child_Package_Option( ).Other_Field_2( ).Name( ) ) );

                     extras.insert( make_pair(
                      name + "of2key", get_obj( ).child_Package_Option( ).Other_Field_2( ).get_key( ) ) );

                     extras.insert( make_pair(
                      name + "srcnam", get_obj( ).child_Package_Option( ).Source_Field( ).Name( ) ) );

                     extras.insert( make_pair(
                      name + "srckey", get_obj( ).child_Package_Option( ).Source_Field( ).get_key( ) ) );

                     extras.insert( make_pair(
                      name + "srcpcl", get_obj( ).child_Package_Option( ).Source_Field( ).Parent_Class( ).get_key( ) ) );

                     extras.insert( make_pair(
                      name + "oscnam", get_obj( ).child_Package_Option( ).Other_Source_Field( ).Name( ) ) );

                     extras.insert( make_pair(
                      name + "osckey", get_obj( ).child_Package_Option( ).Other_Source_Field( ).get_key( ) ) );

                     extras.insert( make_pair(
                      name + "oscpcl", get_obj( ).child_Package_Option( ).Other_Source_Field( ).Parent_Class( ).get_key( ) ) );

                     extras.insert( make_pair(
                      name + "modkey", get_obj( ).child_Package_Option( ).Modifier( ).get_key( ) ) );

                     extras.insert( make_pair(
                      name + "prokey", get_obj( ).child_Package_Option( ).Procedure( ).get_key( ) ) );
                  }
               }
            } while( get_obj( ).child_Package_Option( ).iterate_next( ) );
         }

         string opt_prefix( "opt_" + lower( type_name ) + "_" );

         // NOTE: The "extras" need to preceed the normal keys so that they can be used in
         // conditional expressions.
         for( map< string, string >::iterator i = extras.begin( ); i != extras.end( ); ++i )
            outf << i->first << '=' << i->second << '\n';

         string next;

         set< string > options_processed;

         while( getline( inpf, next ) )
         {
            remove_trailing_cr_from_text_file_line( next );

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

                     next += options[ opt_name ];
                  }
               }
            }

            outf << next << '\n';
         }

         string install_log( get_obj( ).get_key( ) + ".install.log" );

         ofstream outl( install_log.c_str( ) );

         if( !outl )
            throw runtime_error( "unable to open '" + install_log + "' for output" );

         // FUTURE: This message should be handled as a server string message.
         outl << "Starting Install...\n";

         ofstream outs( script_filename.c_str( ) );

         if( !outs )
            throw runtime_error( "unable to open '" + script_filename + "' for output" );

         outs << "#!/bin/bash\n\n";

         outs << "./ciyam_client " << standard_client_args << " -no_stderr < ";
         outs << commands_filename << " >>" << install_log << "\n";
         outs << "echo Finished Install..." << ">>" << install_log << "\n"; // FUTURE: Should be a module string...

         outs << "rm " << list_filename << " " << commands_filename << "\n";

         ofstream outc( commands_filename.c_str( ) );

         if( !outc )
            throw runtime_error( "unable to open '" + commands_filename + "' for output" );

         outc << ".storage_init " << storage_name( ) << "\n";
         outc << ".session_variable @package " << temp_name << "\n";

         string attached_file_path_var( get_special_var_name( e_special_var_attached_file_path ) );

         if( !get_session_variable( attached_file_path_var ).empty( ) )
            outc << ".session_variable " << attached_file_path_var << " \"" << get_session_variable( attached_file_path_var ) << "\"\n";

         outc << "perform_package_import " << get_uid( ) << " @now " << get_obj( ).get_module_name( )
          << " " << type_name << ".package.sio -new_only -s=@Meta_Class.skips.lst -r=@" << list_filename << "\n";

         outc << ".pe " << get_uid( ) << " @now " << get_obj( ).get_module_id( ) << " "
          << get_obj( ).get_class_id( ) << " " << get_obj( ).get_key( ) << " 136430\n";

         if( async )
            outc << ".session_lock -release -at_term " << session_id( ) << "\n"; // see NOTE below...

         outc << ".quit\n";
      }

      get_obj( ).op_update( );
      get_obj( ).Actions( "" );
      get_obj( ).op_apply( );

      string model_key( "Meta_Model_" + get_obj( ).Model( ).get_key( ) );

      // FUTURE: This message should be handled as a server string message.
      set_system_variable( model_key, "Installing package '" + get_obj( ).Name( ) + "'..." );

      // NOTE: If the thread that has spawned the child process is terminated (due
      // to client deciding to finish its session) then this can potentially cause
      // big troubles due to resource inheritance so the session is captured prior
      // to the async request and will be released at the end of the script.
      if( async )
         capture_session( session_id( ) );

      chmod( script_filename.c_str( ), 0770 );

      exec_system( "./run_temp " + script_filename, async );
   }
   // [<finish Install_impl>]
}

void Meta_Package::impl::impl_Remove( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Remove_impl>]
//nyi
   string std_package_key;

   if( !is_null( get_obj( ).Model( ).Workgroup( ).Standard_Package( ) ) )
      std_package_key = get_obj( ).Model( ).Workgroup( ).Standard_Package( ).get_key( );

   string model_key( "Meta_Model_" + get_obj( ).Model( ).get_key( ) );

   bool do_exec = true;

   if( get_obj( ).get_variable( get_special_var_name( e_special_var_do_exec ) ) == "0"
    || get_obj( ).get_variable( get_special_var_name( e_special_var_do_exec ) ) == "false" )
      do_exec = false;

   bool is_last = false;

   if( get_obj( ).get_variable( get_special_var_name( e_special_var_is_last ) ) == "1"
    || get_obj( ).get_variable( get_special_var_name( e_special_var_is_last ) ) == "true" )
      is_last = true;

   if( !std_package_key.empty( )
    && ( get_obj( ).get_key( ) != std_package_key ) && ( get_obj( ).Name( ) == "Standard" ) )
   {
      get_obj( ).op_update( );

      get_obj( ).Model( ).Workgroup( ).Standard_Package( ).op_update( );

      int count = get_obj( ).Model( ).Workgroup( ).Standard_Package( ).Usage_Count( );

      get_obj( ).Model( ).Workgroup( ).Standard_Package( ).Usage_Count( --count );

      get_obj( ).Model( ).Workgroup( ).Standard_Package( ).op_apply( );

      get_obj( ).Actions( "136410" );
      get_obj( ).Installed( false );

      string install_log( get_obj( ).get_key( ) + ".install.log" );

      string install_details( load_file( install_log, true ) );

      ofstream outl( install_log.c_str( ) );

      if( !outl )
         throw runtime_error( "unable to open '" + install_log + "' for output" );

      if( !install_details.empty( ) )
         outl << install_details << "\n\n";

      // FUTURE: This message should be handled as a server string message.
      outl << "Unlinked from existing Standard package...";

      get_obj( ).op_apply( );

      if( is_last )
         set_system_variable( "@" + model_key, "" );
   }
   else if( !storage_locked_for_admin( ) )
   {
      bool skip = false;

      if( get_obj( ).Usage_Count( ) )
      {
         if( do_exec )
            skip = true;
         else
            // FUTURE: This message should be handled as a server string message.
            throw runtime_error( "This package cannot be removed as it is being referenced by other models." );
      }

      class_pointer< Meta_Package > cp_other( e_create_instance );

      if( do_exec && cp_other->iterate_forwards( ) )
      {
         do
         {
            if( cp_other->Installed( ) && ( cp_other->get_key( ) != get_obj( ).get_key( ) ) )
            {
               string deps( cp_other->Package_Type( ).Dependencies( ) );

               deps = search_replace( deps, "\n", "," );

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
                     {
                        cp_other->iterate_stop( );

                        set_system_variable( model_key, "" );
                        set_system_variable( "@" + model_key, "" );

                        // FUTURE: This message should be handled as a server string message.
                        throw runtime_error( "Need to remove '"
                         + cp_other->Name( ) + "' before this package can be removed." );
                     }
                  }
               }

               if( cp_other->child_Package_Option( ).iterate_forwards( ) )
               {
                  do
                  {
                     if( get_obj( ).get_key( ) == cp_other->child_Package_Option( ).Other_Package( ).get_key( ) )
                     {
                        cp_other->child_Package_Option( ).iterate_stop( );

                        cp_other->iterate_stop( );

                        set_system_variable( model_key, "" );
                        set_system_variable( "@" + model_key, "" );

                        // FUTURE: This message should be handled as a server string message.
                        throw runtime_error( "Need to remove '"
                         + cp_other->Name( ) + "' before this package can be removed." );
                     }
                  } while( cp_other->child_Package_Option( ).iterate_next( ) );
               }
            }
         } while( cp_other->iterate_next( ) );
      }

      if( !skip )
         get_obj( ).op_update( );

      string standard_client_args( "-quiet -no_prompt" );

      string rpc_password( get_rpc_password( ) );

      if( !rpc_password.empty( ) )
         standard_client_args += " -rpc_unlock=" + rpc_password;

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

         string script_filename( get_obj( ).get_key( ) );

         string commands_filename( get_obj( ).get_key( ) + ".cin" );

         // NOTE: A "progress pipe" file supports viewing progress
         // when this function has been called from a console with
         // a bash script (such as "reconstruct_meta") as standard
         // output is captured and standard error would not appear
         // when running as a daemon.
         string progress_pipe( get_raw_session_variable(
          get_special_var_name( e_special_var_progress_pipe ) ) );

         // NOTE: Empty code block for scope purposes.
         {
            ofstream outf( commands_filename.c_str( ) );

            if( !outf )
               throw runtime_error( "unable to open file '" + commands_filename + "' for output" );

            if( skip && is_last )
               outf << ".system_variable @" << model_key << " \"\"\n";

            if( !skip )
            {
               outf << ".storage_init " << storage_name( ) << '\n';

               outf << "@ifndef $ERROR\n";
               outf << ".storage_transaction_start\n";
               outf << "@endif\n";

               // FUTURE: This message should be handled as a server string message.
               if( !progress_pipe.empty( ) )
                  outf << "~echo \"\rRemoving " << get_obj( ).Name( ) << " package...\" >> " << progress_pipe << "\n";

               string attached_file_path_var( get_special_var_name( e_special_var_attached_file_path ) );

               if( !get_session_variable( attached_file_path_var ).empty( ) )
                  outf << ".session_variable " << attached_file_path_var << " \"" << get_session_variable( attached_file_path_var ) << "\"\n";

               // NOTE: Packages could contain updates of external artifacts (such as specifications) which
               // need to be "undone" as updates (rather than occurring automatically via cascades) so here
               // the package will be processed for just this purpose (the keys for such updates are marked
               // specifically for this purpose).
               string map_filename( get_obj( ).get_attached_file_path( get_obj( ).get_key( ) + ".map" ) );

               if( exists_file( map_filename ) )
                  outf << ".perform_package_import " << get_uid( ) << " @now "
                   << get_obj( ).get_module_name( ) << " " << get_obj( ).Package_Type( ).Name( )
                   << ".package.sio -for_remove -r=@" << escaped_string( map_filename ) << "\n";

               vector< string > ordered;

               string acyclic_filename( get_obj( ).get_module_name( ) + ".acyclic.lst" );

               read_file_lines( acyclic_filename, ordered );

               // NOTE: Forcing the order to reach "Class" as quickly as possible is done as a performance
               // optimisation (as it will automatically cascade numerous other records). Although for the
               // package's own Views and Lists the View Fields and List Fields are automatically cascaded
               // this will not be the case for View Fields and List Fields added to Views and Lists which
               // belong to other packages (so they are included explicitly before Specifications as these
               // can be tied to View Fields and List Fields as "restrictions").
               ordered.push_back( "Class" );
               ordered.push_back( "Specification" );
               ordered.push_back( "View_Field" );
               ordered.push_back( "View" );
               ordered.push_back( "List_Field" );
               ordered.push_back( "List" );

               // NOTE: In order to make sure deletes are correctedly ordered they need to be
               // processed in the opposite of the acyclic class list (as its ordering is for
               // record creation).
               reverse( ordered.begin( ), ordered.end( ) );

               // NOTE: For UI "progress" outputs the total number of
               // records that have been processed every ten seconds.
               outf << ";\n";
               outf << "TOTAL_RECORDS=0\n";
               outf << ".utc_to_unix_time @now\n";
               outf << "SHOW_PROGRESS_TIME=@$OUTPUT+10\n";
               outf << ";\n";
               outf << "session_variable @force_internal 1\n";
               outf << ";\n";

               for( size_t i = 0; i < ordered.size( ); i++ )
               {
                  string next_cid = get_class_id_for_id_or_name( get_obj( ).get_module_name( ), ordered[ i ] );

                  for( int j = 0; j < class_keys[ next_cid ].size( ); j++ )
                  {
                     outf << "@ifndef $ERROR\n";

                     outf << ".pd " << get_uid( ) << " @now "
                      << get_obj( ).get_module_id( ) << ' ' << next_cid << " -p -q " << class_keys[ next_cid ][ j ] << '\n';

                     outf << "@ifdef $ERROR\n";
                     outf << "#(failed to delete " << ordered[ i ] << " record " << class_keys[ next_cid ][ j ] << ")\n";
                     outf << "@endif\n";

                     outf << ";\n";
                     outf << "TOTAL_RECORDS=@$TOTAL_RECORDS+1\n";
                     outf << ";\n";
                     outf << ".utc_to_unix_time @now\n";
                     outf << "SHOW_PROGRESS=@$OUTPUT>=$SHOW_PROGRESS_TIME\n";

                     outf << "@ifeq $SHOW_PROGRESS 1\n";

                     // FUTURE: These messages should be handled as a server string messages.
                     if( progress_pipe.empty( ) )
                        outf << "#Processed $TOTAL_RECORDS records...\n";
                     else
                        outf << "~echo \"Processed $TOTAL_RECORDS records...\" >> " << progress_pipe << "\n";

                     outf << ".utc_to_unix_time @now\n";
                     outf << "SHOW_PROGRESS_TIME=@$OUTPUT+10\n";
                     outf << "@endif\n";

                     outf << ";\n";
                     outf << "@endif\n";
                     outf << ";\n";
                  }

                  class_keys[ next_cid ].clear( );
               }

               outf << "@ifndef $ERROR\n";
               outf << ".storage_transaction_commit\n";
               outf << "@else\n";
               outf << ".storage_transaction_rollback\n";
               outf << "@endif\n";
               outf << ";\n";

               if( is_last )
                  outf << ".system_variable @" << model_key << " \"\"\n";

               outf << "@ifndef $ERROR\n";
               outf << ".pe " << get_uid( ) << " @now " << get_obj( ).get_module_id( ) << " "
                << get_obj( ).get_class_id( ) << " " << get_obj( ).get_key( ) << " 136440\n"; // i.e. Complete_Remove
               outf << "@else\n";
               outf << ".pe " << get_uid( ) << " @now " << get_obj( ).get_module_id( ) << " "
                << get_obj( ).get_class_id( ) << " " << get_obj( ).get_key( ) << " 136450\n"; // i.e. Cancel_Remove
               outf << "@endif\n";

               if( do_exec || is_last )
                  outf << ".session_lock -release " << session_id( ) << "\n"; // see NOTE below...
            }

            outf << ".quit\n";

            string install_log( get_obj( ).get_key( ) + ".install.log" );

            if( !skip )
            {
               ofstream outl( install_log.c_str( ), ios::out | ios::app );

               if( !outl )
                  throw runtime_error( "unable to open '" + install_log + "' for output" );

               // FUTURE: This message should be handled as a server string message.
               outl << "\nStarting Remove...\n";
            }

            ofstream outs( script_filename.c_str( ) );

            if( !outs )
               throw runtime_error( "unable to open '" + script_filename + "' for output" );

            outs << "#!/bin/bash\n\n";

            outs << "./ciyam_client " << standard_client_args << " < ";
            outs << commands_filename << " >>" << install_log << "\n";
            outs << "rm " << commands_filename << "\n";

            if( !skip )
            {
               // FUTURE: This message should be handled as a server string message.
               outs << "echo Finished Remove...>>" << install_log << "\n";

               get_obj( ).Actions( "" );
               get_obj( ).op_apply( );
            }
         }

         // FUTURE: This message should be handled as a server string message.
         set_system_variable( model_key, "Removing package '" + get_obj( ).Name( ) + "'..." );

         if( do_exec )
         {
            // NOTE: If the thread that has spawned the child process is terminated (due
            // to client deciding to finish its session) then this can potentially cause
            // big troubles due to resource inheritance so the session is captured prior
            // to the async request and will be released at the end of the script.
            capture_session( session_id( ) );

            chmod( script_filename.c_str( ), 0770 );
            exec_system( "./run_temp " + script_filename, true );
         }
         else
            chmod( script_filename.c_str( ), 0770 );
      }
      else
      {
         if( !skip )
         {
            get_obj( ).Actions( "136410" );
            get_obj( ).Installed( false );
            get_obj( ).op_apply( );
         }
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
      retval = to_string( impl_Installed_Order( ) );
      break;

      case 4:
      retval = to_string( impl_Key( ) );
      break;

      case 5:
      retval = to_string( impl_Model( ) );
      break;

      case 6:
      retval = to_string( impl_Name( ) );
      break;

      case 7:
      retval = to_string( impl_Package_Type( ) );
      break;

      case 8:
      retval = to_string( impl_Plural( ) );
      break;

      case 9:
      retval = to_string( impl_Type_Name( ) );
      break;

      case 10:
      retval = to_string( impl_Usage_Count( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
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
      func_string_setter< Meta_Package::impl, string >( *this, &Meta_Package::impl::impl_Installed_Order, value );
      break;

      case 4:
      func_string_setter< Meta_Package::impl, string >( *this, &Meta_Package::impl::impl_Key, value );
      break;

      case 5:
      func_string_setter< Meta_Package::impl, Meta_Model >( *this, &Meta_Package::impl::impl_Model, value );
      break;

      case 6:
      func_string_setter< Meta_Package::impl, string >( *this, &Meta_Package::impl::impl_Name, value );
      break;

      case 7:
      func_string_setter< Meta_Package::impl, Meta_Package_Type >( *this, &Meta_Package::impl::impl_Package_Type, value );
      break;

      case 8:
      func_string_setter< Meta_Package::impl, string >( *this, &Meta_Package::impl::impl_Plural, value );
      break;

      case 9:
      func_string_setter< Meta_Package::impl, string >( *this, &Meta_Package::impl::impl_Type_Name, value );
      break;

      case 10:
      func_string_setter< Meta_Package::impl, int >( *this, &Meta_Package::impl::impl_Usage_Count, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_Package::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Actions( g_default_Actions );
      break;

      case 1:
      impl_Install_Details( g_default_Install_Details );
      break;

      case 2:
      impl_Installed( g_default_Installed );
      break;

      case 3:
      impl_Installed_Order( g_default_Installed_Order );
      break;

      case 4:
      impl_Key( g_default_Key );
      break;

      case 5:
      impl_Model( g_default_Model );
      break;

      case 6:
      impl_Name( g_default_Name );
      break;

      case 7:
      impl_Package_Type( g_default_Package_Type );
      break;

      case 8:
      impl_Plural( g_default_Plural );
      break;

      case 9:
      impl_Type_Name( g_default_Type_Name );
      break;

      case 10:
      impl_Usage_Count( g_default_Usage_Count );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_Package::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Actions == g_default_Actions );
      break;

      case 1:
      retval = ( v_Install_Details == g_default_Install_Details );
      break;

      case 2:
      retval = ( v_Installed == g_default_Installed );
      break;

      case 3:
      retval = ( v_Installed_Order == g_default_Installed_Order );
      break;

      case 4:
      retval = ( v_Key == g_default_Key );
      break;

      case 5:
      retval = ( v_Model == g_default_Model );
      break;

      case 6:
      retval = ( v_Name == g_default_Name );
      break;

      case 7:
      retval = ( v_Package_Type == g_default_Package_Type );
      break;

      case 8:
      retval = ( v_Plural == g_default_Plural );
      break;

      case 9:
      retval = ( v_Type_Name == g_default_Type_Name );
      break;

      case 10:
      retval = ( v_Usage_Count == g_default_Usage_Count );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_Package::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start protect_equal)] 600852
   if( check_equal( get_obj( ).Installed( ), true ) )
      state |= ( c_state_uneditable | c_state_undeletable );
   // [(finish protect_equal)] 600852

   // [(start protect_equal)] 600854
   if( check_equal( get_obj( ).Actions( ), "" ) )
      state |= ( c_state_undeletable | c_state_is_changing );
   // [(finish protect_equal)] 600854

   // [(start modifier_field_value)] 600857
   if( get_obj( ).Installed( ) == false )
      state |= c_modifier_Is_Not_Installed;
   // [(finish modifier_field_value)] 600857

   // [<start get_state>]
//nyi
   if( get_obj( ).Usage_Count( ) > 0 )
      state |= c_modifier_Is_In_Use;

   string model_key( "Meta_Model_" + get_obj( ).Model( ).get_key( ) );

   if( !get_system_variable( model_key ).empty( ) )
      state |= ( c_state_is_changing | c_state_uneditable | c_state_unactionable );
   // [<finish get_state>]

   return state;
}

string Meta_Package::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   if( state & c_modifier_Is_In_Use )
      state_names += "|" + string( "Is_In_Use" );
   if( state & c_modifier_Is_Not_Installed )
      state_names += "|" + string( "Is_Not_Installed" );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
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
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      impl_Model( "" );
   else if( ( field == c_field_id_Package_Type ) || ( field == c_field_name_Package_Type ) )
      impl_Package_Type( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Package::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      v_Model = value;
   else if( ( field == c_field_id_Package_Type ) || ( field == c_field_name_Package_Type ) )
      v_Package_Type = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Package::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      return v_Model;
   else if( ( field == c_field_id_Package_Type ) || ( field == c_field_name_Package_Type ) )
      return v_Package_Type;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Package::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Model, v_Model ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Package_Type, v_Package_Type ) );
}

void Meta_Package::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_Package::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Package::impl::clear( )
{
   v_Actions = g_default_Actions;
   v_Install_Details = g_default_Install_Details;
   v_Installed = g_default_Installed;
   v_Installed_Order = g_default_Installed_Order;
   v_Key = g_default_Key;
   v_Name = g_default_Name;
   v_Plural = g_default_Plural;
   v_Type_Name = g_default_Type_Name;
   v_Usage_Count = g_default_Usage_Count;

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

void Meta_Package::impl::validate(
 uint64_t state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( is_null( v_Key ) && !value_will_be_provided( c_field_name_Key ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Key,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Key ) ) ) ) );

   if( is_null( v_Name ) && !value_will_be_provided( c_field_name_Name ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Name ) ) ) ) );

   if( is_null( v_Plural ) && !value_will_be_provided( c_field_name_Plural ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Plural,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Plural ) ) ) ) );

   if( v_Model.empty( ) && !value_will_be_provided( c_field_name_Model ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Model,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Model ) ) ) ) );

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

   if( !is_null( v_Plural )
    && ( v_Plural != g_default_Plural
    || !value_will_be_provided( c_field_name_Plural ) )
    && !g_Plural_domain.is_valid( v_Plural, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Plural,
       get_module_string( c_field_display_name_Plural ) + " " + error_message ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Package::impl::validate_set_fields(
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

   if( !is_null( v_Plural )
    && ( fields_set.count( c_field_id_Plural ) || fields_set.count( c_field_name_Plural ) )
    && !g_Plural_domain.is_valid( v_Plural, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Plural,
       get_module_string( c_field_display_name_Plural ) + " " + error_message ) );
}

void Meta_Package::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   if( cp_Package_Type )
      p_obj->setup_foreign_key( *cp_Package_Type, v_Package_Type );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start transient_field_alias)] 600289
   if( get_obj( ).needs_field_value( "Type_Name" )
    || required_transients.count( "Type_Name" ) )
      get_obj( ).Type_Name( get_obj( ).Package_Type( ).Name( ) );
   // [(finish transient_field_alias)] 600289

   // [(start transient_field_from_file)] 600855
   if( !get_obj( ).get_key( ).empty( )
    && ( get_obj( ).needs_field_value( "Install_Details" )
    || required_transients.count( "Install_Details" ) ) )
      get_obj( ).Install_Details( load_file( get_obj( ).Key( ) + ".install.log", true ) );
   // [(finish transient_field_from_file)] 600855

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Package::impl::finalise_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Package::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Package::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_Package::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_from_changed_fk)] 600850
   if( get_obj( ).get_key( ).empty( ) && get_obj( ).Package_Type( ).has_changed( ) )
      get_obj( ).Name( get_obj( ).Package_Type( ).Single( ) );
   // [(finish field_from_changed_fk)] 600850

   // [(start field_from_changed_fk)] 600851
   if( get_obj( ).get_key( ).empty( ) && get_obj( ).Package_Type( ).has_changed( ) )
      get_obj( ).Plural( get_obj( ).Package_Type( ).Plural( ) );
   // [(finish field_from_changed_fk)] 600851

   // [(start default_from_key)] 600856
   if( !get_obj( ).get_clone_key( ).empty( ) || ( is_create && ( get_obj( ).Key( ) == g_default_Key ) ) )
      get_obj( ).Key( get_obj( ).get_key( ) );
   // [(finish default_from_key)] 600856

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Package::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_store>]
//nyi
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

               // FUTURE: This message should be handled as a server string message.
               throw runtime_error( "This package type is singular and has already been added." );
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

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_store>]
//nyi
   if( is_create )
   {
      string key_list( get_obj( ).Package_Type( ).Name( ) + ".keys.lst" );

      ifstream inpf( key_list.c_str( ) );

      if( !inpf )
         throw runtime_error( "unable to open '" + key_list + "' for input" );

      set< string > options;

      string name( lower( get_obj( ).Package_Type( ).Name( ) ) );

      string ext_prefix( "ext_" + name + "_" );
      string opt_prefix( "opt_" + name + "_" );

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
         remove_trailing_cr_from_text_file_line( next );

         if( next.find( ext_prefix ) == 0 )
         {
            string::size_type pos = next.find( '=' );

            if( pos != string::npos )
            {
               string opt_id = next.substr( 0, pos );

               if( !options.count( opt_id ) )
               {
                  get_obj( ).child_Package_Option( ).op_create( construct_key_from_int( get_obj( ).get_key( ), ++child_num ) );

                  get_obj( ).child_Package_Option( ).Package( get_obj( ).get_key( ) );

                  get_obj( ).child_Package_Option( ).Id( opt_id );
                  get_obj( ).child_Package_Option( ).Name( opt_id.substr( ext_prefix.length( ) ) );

                  get_obj( ).child_Package_Option( ).Is_Other_Package( true );
                  get_obj( ).child_Package_Option( ).Other_Package_Type( package_types[ next.substr( pos + 1 ) ] );

                  get_obj( ).child_Package_Option( ).Other_Package_Required( true );

                  get_obj( ).child_Package_Option( ).op_apply( );

                  options.insert( opt_id );
               }
            }
         }
         else if( next.find( opt_prefix ) == 0 )
         {
            string::size_type pos = next.find( '=' );

            if( pos != string::npos )
            {
               string opt_id = next.substr( 0, pos );
               string type_and_id = opt_id.substr( opt_prefix.length( ) );

               if( !type_and_id.empty( ) )
               {
                  string::size_type tpos = type_and_id.find( '_' );

                  if( ( tpos == 0 ) || ( tpos == string::npos ) )
                     throw runtime_error( "unexpected invalid option type_and_id: " + type_and_id );

                  string type( type_and_id.substr( 0, tpos ) );

                  if( !options.count( opt_id ) )
                  {
                     get_obj( ).child_Package_Option( ).op_create( construct_key_from_int( get_obj( ).get_key( ), ++child_num ) );

                     get_obj( ).child_Package_Option( ).Package( get_obj( ).get_key( ) );

                     get_obj( ).child_Package_Option( ).Id( opt_id );
                     get_obj( ).child_Package_Option( ).Name( opt_id.substr( opt_prefix.length( ) + type.length( ) + 1 ) );

                     string value = next.substr( pos + 1 );

                     if( type == "class" )
                     {
                        get_obj( ).child_Package_Option( ).Is_Class( true );

                        vector< string > class_options;
                        split_string( value, class_options, '+' );

                        for( size_t i = 0; i < class_options.size( ); i++ )
                        {
                           bool valid = false;

                           if( class_options[ i ] == "mandatory" )
                           {
                              valid = true;

                              get_obj( ).child_Package_Option( ).Is_Mandatory_Class( true );
                           }
                           else if( class_options[ i ].find( "notes=" ) == 0 )
                           {
                              valid = true;

                              get_obj( ).child_Package_Option( ).Notes( class_options[ i ].substr( 6 ) );
                           }
                           else if( class_options[ i ].find( "view" ) == 0 )
                           {
                              string view_info( class_options[ i ] );

                              string::size_type pos = view_info.find( '=' );

                              if( pos == string::npos )
                              {
                                 if( view_info == "view" )
                                 {
                                    valid = true;

                                    get_obj( ).child_Package_Option( ).Has_View( true );
                                 }
                              }
                              else
                              {
                                 string info( view_info.substr( 0, pos ) );
                                 string extra = view_info.substr( pos + 1 );

                                 if( ( info == "view" ) && ( extra == "mandatory" ) )
                                 {
                                    valid = true;

                                    get_obj( ).child_Package_Option( ).Has_View( true );
                                    get_obj( ).child_Package_Option( ).Is_Mandatory_View( true );
                                 }
                              }
                           }
                           else if( class_options[ i ].find( "list" ) == 0 )
                           {
                              string list_info( class_options[ i ] );

                              string::size_type pos = list_info.find( '=' );

                              if( pos == string::npos )
                              {
                                 if( list_info == "list" )
                                 {
                                    valid = true;
                                    get_obj( ).child_Package_Option( ).Has_List( true );
                                 }
                              }
                              else
                              {
                                 string info( list_info.substr( 0, pos ) );
                                 string extra = list_info.substr( pos + 1 );

                                 if( ( info == "list" ) && ( extra == "mandatory" ) )
                                 {
                                    valid = true;

                                    get_obj( ).child_Package_Option( ).Has_List( true );
                                    get_obj( ).child_Package_Option( ).Is_Mandatory_List( true );
                                 }
                              }
                           }
                           else if( class_options[ i ].find( "field" ) == 0 )
                           {
                              string field_info( class_options[ i ] );

                              string::size_type pos = field_info.find( '=' );

                              if( pos == string::npos )
                              {
                                 if( field_info == "field" )
                                 {
                                    valid = true;

                                    get_obj( ).child_Package_Option( ).Has_Field( true );
                                 }
                              }
                              else
                              {
                                 string info( field_info.substr( 0, pos ) );
                                 string extra = field_info.substr( pos + 1 );

                                 if( ( info == "field" ) && ( extra == "mandatory" ) )
                                 {
                                    valid = true;

                                    get_obj( ).child_Package_Option( ).Has_Field( true );
                                    get_obj( ).child_Package_Option( ).Is_Mandatory_Field( true );
                                 }
                              }
                           }
                           else if( class_options[ i ].find( "ofield" ) == 0 )
                           {
                              string field_info( class_options[ i ] );

                              string::size_type pos = field_info.find( '=' );

                              if( pos == string::npos )
                              {
                                 if( field_info == "ofield" )
                                 {
                                    valid = true;

                                    get_obj( ).child_Package_Option( ).Has_Other_Field( true );
                                 }
                              }
                              else
                              {
                                 string info( field_info.substr( 0, pos ) );
                                 string extra = field_info.substr( pos + 1 );

                                 if( ( info == "ofield" ) && ( extra == "mandatory" ) )
                                 {
                                    valid = true;

                                    get_obj( ).child_Package_Option( ).Has_Other_Field( true );
                                    get_obj( ).child_Package_Option( ).Is_Mandatory_Other_Field( true );
                                 }
                              }
                           }
                           else if( class_options[ i ].find( "o2field" ) == 0 )
                           {
                              string field_info( class_options[ i ] );

                              string::size_type pos = field_info.find( '=' );

                              if( pos == string::npos )
                              {
                                 if( field_info == "o2field" )
                                 {
                                    valid = true;

                                    get_obj( ).child_Package_Option( ).Has_Other_Field_2( true );
                                 }
                              }
                              else
                              {
                                 string info( field_info.substr( 0, pos ) );
                                 string extra = field_info.substr( pos + 1 );

                                 if( ( info == "o2field" ) && ( extra == "mandatory" ) )
                                 {
                                    valid = true;

                                    get_obj( ).child_Package_Option( ).Has_Other_Field_2( true );
                                    get_obj( ).child_Package_Option( ).Is_Mandatory_Other_Field_2( true );
                                 }
                              }
                           }
                           else if( class_options[ i ].find( "sfield" ) == 0 )
                           {
                              string field_info( class_options[ i ] );

                              string::size_type pos = field_info.find( '=' );

                              if( pos == string::npos )
                              {
                                 if( field_info == "sfield" )
                                 {
                                    valid = true;

                                    get_obj( ).child_Package_Option( ).Has_Source_Field( true );
                                 }
                              }
                              else
                              {
                                 string info( field_info.substr( 0, pos ) );
                                 string extra = field_info.substr( pos + 1 );

                                 if( ( info == "sfield" ) && ( extra == "mandatory" ) )
                                 {
                                    valid = true;

                                    get_obj( ).child_Package_Option( ).Has_Source_Field( true );
                                    get_obj( ).child_Package_Option( ).Is_Mandatory_Source_Field( true );
                                 }
                              }
                           }
                           else if( class_options[ i ].find( "osfield" ) == 0 )
                           {
                              string field_info( class_options[ i ] );

                              string::size_type pos = field_info.find( '=' );

                              if( pos == string::npos )
                              {
                                 if( field_info == "osfield" )
                                 {
                                    valid = true;

                                    get_obj( ).child_Package_Option( ).Has_Other_Source_Field( true );
                                 }
                              }
                              else
                              {
                                 string info( field_info.substr( 0, pos ) );
                                 string extra = field_info.substr( pos + 1 );

                                 if( ( info == "osfield" ) && ( extra == "mandatory" ) )
                                 {
                                    valid = true;

                                    get_obj( ).child_Package_Option( ).Has_Other_Source_Field( true );
                                    get_obj( ).child_Package_Option( ).Is_Mandatory_Other_Source_Field( true );
                                 }
                              }
                           }
                           else if( class_options[ i ].find( "modifier" ) == 0 )
                           {
                              string modifier_info( class_options[ i ] );

                              string::size_type pos = modifier_info.find( '=' );

                              if( pos == string::npos )
                              {
                                 if( modifier_info == "modifier" )
                                 {
                                    valid = true;
                                    get_obj( ).child_Package_Option( ).Has_Modifier( true );
                                 }
                              }
                              else
                              {
                                 string info( modifier_info.substr( 0, pos ) );
                                 string extra = modifier_info.substr( pos + 1 );

                                 if( ( info == "modifier" ) && ( extra == "mandatory" ) )
                                 {
                                    valid = true;

                                    get_obj( ).child_Package_Option( ).Has_Modifier( true );
                                    get_obj( ).child_Package_Option( ).Is_Mandatory_Modifier( true );
                                 }
                              }
                           }
                           else if( class_options[ i ].find( "procedure" ) == 0 )
                           {
                              string procedure_info( class_options[ i ] );

                              string::size_type pos = procedure_info.find( '=' );

                              if( pos == string::npos )
                              {
                                 if( procedure_info == "procedure" )
                                 {
                                    valid = true;
                                    get_obj( ).child_Package_Option( ).Has_Procedure( true );
                                 }
                              }
                              else
                              {
                                 string info( procedure_info.substr( 0, pos ) );
                                 string extra = procedure_info.substr( pos + 1 );

                                 if( ( info == "procedure" ) && ( extra == "mandatory" ) )
                                 {
                                    valid = true;

                                    get_obj( ).child_Package_Option( ).Has_Procedure( true );
                                    get_obj( ).child_Package_Option( ).Is_Mandatory_Procedure( true );
                                 }
                              }
                           }

                           if( !valid )
                              throw runtime_error( "unexpected class option value '"
                               + class_options[ i ] + "' for " + get_obj( ).child_Package_Option( ).Name( ) );
                        }
                     }
                     else if( type == "package" )
                     {
                        get_obj( ).child_Package_Option( ).Is_Other_Package( true );
                        get_obj( ).child_Package_Option( ).Other_Package_Type( package_types[ value ] );
                     }
                     else
                     {
                        get_obj( ).child_Package_Option( ).Primitive( meta_field_type_primitive( type ) );

                        switch( get_obj( ).child_Package_Option( ).Primitive( ) )
                        {
                           case e_primitive_string:
                           get_obj( ).child_Package_Option( ).String( value );
                           break;

                           case e_primitive_datetime:
                           get_obj( ).child_Package_Option( ).Datetime( value );
                           break;

                           case e_primitive_date:
                           get_obj( ).child_Package_Option( ).Date( value );
                           break;

                           case e_primitive_time:
                           get_obj( ).child_Package_Option( ).Time( value );
                           break;

                           case e_primitive_numeric:
                           get_obj( ).child_Package_Option( ).Numeric( value );
                           break;

                           case e_primitive_int:
                           get_obj( ).child_Package_Option( ).Integer( atoi( value.c_str( ) ) );
                           break;

                           case e_primitive_bool:
                           if( get_obj( ).child_Package_Option( ).Name( ) == "@use_demo_data" )
                           {
                              if( get_obj( ).Model( ).Use_Package_Demo_Data( ) )
                                 value = "1";
                              else
                                 value.erase( );
                           }
                           else if( ( get_obj( ).child_Package_Option( ).Name( ) == "@has_blockchain" )
                            || ( get_obj( ).child_Package_Option( ).Name( ) == "@use_peerchains" ) )
                           {
                              if( !get_obj( ).Model( ).Type( ) )
                                 value = c_true_value;
                              else
                                 value.erase( );
                           }
                           get_obj( ).child_Package_Option( ).Use_Option( ( value == c_true ) || ( value == c_true_value ) );
                           break;

                           default:
                           throw runtime_error( "unexpected primitive value #"
                            + to_string( get_obj( ).child_Package_Option( ).Primitive( ) ) + "in Meta_Package::after_store" );
                        }
                     }

                     get_obj( ).child_Package_Option( ).op_apply( );

                     options.insert( opt_id );
                  }
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
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Package::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Package::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
//nyi
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
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_Package::impl::get_required_transients( ) const
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

const string& Meta_Package::Installed_Order( ) const
{
   return p_impl->impl_Installed_Order( );
}

void Meta_Package::Installed_Order( const string& Installed_Order )
{
   p_impl->impl_Installed_Order( Installed_Order );
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

void Meta_Package::Cancel_Remove( )
{
   p_impl->impl_Cancel_Remove( );
}

void Meta_Package::Check_Install( )
{
   p_impl->impl_Check_Install( );
}

void Meta_Package::Complete_Remove( )
{
   p_impl->impl_Complete_Remove( );
}

void Meta_Package::Install( )
{
   p_impl->impl_Install( );
}

void Meta_Package::Remove( )
{
   p_impl->impl_Remove( );
}

string Meta_Package::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Package::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_Package::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_Package::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_Package::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_Package::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_Package::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_Package::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_Package::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_Package::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Package::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
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

void Meta_Package::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Package::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Package::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Package::finalise_fetch( bool skip_set_original )
{
   p_impl->finalise_fetch( skip_set_original );
}

void Meta_Package::at_create( )
{
   p_impl->at_create( );
}

void Meta_Package::post_init( )
{
   p_impl->post_init( );
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
   else if( name == c_field_name_Installed_Order )
   {
      p_id = c_field_id_Installed_Order;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
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
   else if( id == c_field_id_Installed_Order )
   {
      p_name = c_field_name_Installed_Order;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
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

string& Meta_Package::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_Package::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_Package::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_Package::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_Package::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_Package::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Package::get_field_uom_symbol( const string& id_or_name ) const
{
   string uom_symbol;

   string name;
   pair< string, string > next;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_uom_symbol" );
   if( ( id_or_name == c_field_id_Actions ) || ( id_or_name == c_field_name_Actions ) )
   {
      name = string( c_field_display_name_Actions );
      get_module_string( c_field_display_name_Actions, &next );
   }
   if( ( id_or_name == c_field_id_Install_Details ) || ( id_or_name == c_field_name_Install_Details ) )
   {
      name = string( c_field_display_name_Install_Details );
      get_module_string( c_field_display_name_Install_Details, &next );
   }
   if( ( id_or_name == c_field_id_Installed ) || ( id_or_name == c_field_name_Installed ) )
   {
      name = string( c_field_display_name_Installed );
      get_module_string( c_field_display_name_Installed, &next );
   }
   if( ( id_or_name == c_field_id_Installed_Order ) || ( id_or_name == c_field_name_Installed_Order ) )
   {
      name = string( c_field_display_name_Installed_Order );
      get_module_string( c_field_display_name_Installed_Order, &next );
   }
   if( ( id_or_name == c_field_id_Key ) || ( id_or_name == c_field_name_Key ) )
   {
      name = string( c_field_display_name_Key );
      get_module_string( c_field_display_name_Key, &next );
   }
   if( ( id_or_name == c_field_id_Model ) || ( id_or_name == c_field_name_Model ) )
   {
      name = string( c_field_display_name_Model );
      get_module_string( c_field_display_name_Model, &next );
   }
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
   {
      name = string( c_field_display_name_Name );
      get_module_string( c_field_display_name_Name, &next );
   }
   if( ( id_or_name == c_field_id_Package_Type ) || ( id_or_name == c_field_name_Package_Type ) )
   {
      name = string( c_field_display_name_Package_Type );
      get_module_string( c_field_display_name_Package_Type, &next );
   }
   if( ( id_or_name == c_field_id_Plural ) || ( id_or_name == c_field_name_Plural ) )
   {
      name = string( c_field_display_name_Plural );
      get_module_string( c_field_display_name_Plural, &next );
   }
   if( ( id_or_name == c_field_id_Type_Name ) || ( id_or_name == c_field_name_Type_Name ) )
   {
      name = string( c_field_display_name_Type_Name );
      get_module_string( c_field_display_name_Type_Name, &next );
   }
   if( ( id_or_name == c_field_id_Usage_Count ) || ( id_or_name == c_field_name_Usage_Count ) )
   {
      name = string( c_field_display_name_Usage_Count );
      get_module_string( c_field_display_name_Usage_Count, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_Package::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Actions ) || ( id_or_name == c_field_name_Actions ) )
      display_name = get_module_string( c_field_display_name_Actions );
   if( ( id_or_name == c_field_id_Install_Details ) || ( id_or_name == c_field_name_Install_Details ) )
      display_name = get_module_string( c_field_display_name_Install_Details );
   if( ( id_or_name == c_field_id_Installed ) || ( id_or_name == c_field_name_Installed ) )
      display_name = get_module_string( c_field_display_name_Installed );
   if( ( id_or_name == c_field_id_Installed_Order ) || ( id_or_name == c_field_name_Installed_Order ) )
      display_name = get_module_string( c_field_display_name_Installed_Order );
   if( ( id_or_name == c_field_id_Key ) || ( id_or_name == c_field_name_Key ) )
      display_name = get_module_string( c_field_display_name_Key );
   if( ( id_or_name == c_field_id_Model ) || ( id_or_name == c_field_name_Model ) )
      display_name = get_module_string( c_field_display_name_Model );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
      display_name = get_module_string( c_field_display_name_Name );
   if( ( id_or_name == c_field_id_Package_Type ) || ( id_or_name == c_field_name_Package_Type ) )
      display_name = get_module_string( c_field_display_name_Package_Type );
   if( ( id_or_name == c_field_id_Plural ) || ( id_or_name == c_field_name_Plural ) )
      display_name = get_module_string( c_field_display_name_Plural );
   if( ( id_or_name == c_field_id_Type_Name ) || ( id_or_name == c_field_name_Type_Name ) )
      display_name = get_module_string( c_field_display_name_Type_Name );
   if( ( id_or_name == c_field_id_Usage_Count ) || ( id_or_name == c_field_name_Usage_Count ) )
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

      if( ( ealci == g_external_aliases_lookup.end( ) ) || ( ealci->first > child_num ) )
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

void Meta_Package::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_Package::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_Package::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_Package::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_Package::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_Package::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_Package::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_Package::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "package";

   return get_module_string( key );
}

string Meta_Package::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_Package::get_create_instance_info( ) const
{
   return "";
}

string Meta_Package::get_update_instance_info( ) const
{
   return "";
}

string Meta_Package::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_Package::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );
   else if( procedure_id == "136450" ) // i.e. Cancel_Remove
      retval = "";
   else if( procedure_id == "136430" ) // i.e. Check_Install
      retval = "";
   else if( procedure_id == "136440" ) // i.e. Complete_Remove
      retval = "";
   else if( procedure_id == "136410" ) // i.e. Install
      retval = "";
   else if( procedure_id == "136420" ) // i.e. Remove
      retval = "";

   return retval;
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
   else if( ( sub_context == "_302830" ) || ( sub_context == "child_Package_Option_Other" ) )
      p_class_base = &child_Package_Option_Other( );
   else if( ( sub_context == "_302835" ) || ( sub_context == "child_Workgroup_Standard" ) )
      p_class_base = &child_Workgroup_Standard( );
   else if( ( sub_context == "_302820" ) || ( sub_context == "child_Package_Option" ) )
      p_class_base = &child_Package_Option( );
   else if( ( sub_context == c_field_id_Model ) || ( sub_context == c_field_name_Model ) )
      p_class_base = &Model( );
   else if( ( sub_context == c_field_id_Package_Type ) || ( sub_context == c_field_name_Package_Type ) )
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
   names.push_back( "C_Installed_Order" );
   names.push_back( "C_Key" );
   names.push_back( "C_Model" );
   names.push_back( "C_Name" );
   names.push_back( "C_Package_Type" );
   names.push_back( "C_Plural" );
   names.push_back( "C_Usage_Count" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Package::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Actions( ) ) ) );
   values.push_back( to_string( Installed( ) ) );
   values.push_back( sql_quote( to_string( Installed_Order( ) ) ) );
   values.push_back( sql_quote( to_string( Key( ) ) ) );
   values.push_back( sql_quote( to_string( Model( ) ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( sql_quote( to_string( Package_Type( ) ) ) );
   values.push_back( sql_quote( to_string( Plural( ) ) ) );
   values.push_back( to_string( Usage_Count( ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Package::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [(start transient_field_alias)] 600289
   if( needs_field_value( "Type_Name", dependents ) )
   {
      dependents.insert( "Package_Type" );

      if( ( use_transients && is_field_transient( e_field_id_Package_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Package_Type ) ) )
         names.insert( "Package_Type" );
   }
   // [(finish transient_field_alias)] 600289

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Package::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start protect_equal)] 600852
   dependents.insert( "Installed" );

   if( ( use_transients && is_field_transient( e_field_id_Installed ) )
    || ( !use_transients && !is_field_transient( e_field_id_Installed ) ) )
      names.insert( "Installed" );
   // [(finish protect_equal)] 600852

   // [(start protect_equal)] 600854
   dependents.insert( "Actions" );

   if( ( use_transients && is_field_transient( e_field_id_Actions ) )
    || ( !use_transients && !is_field_transient( e_field_id_Actions ) ) )
      names.insert( "Actions" );
   // [(finish protect_equal)] 600854

   // [(start modifier_field_value)] 600857
   dependents.insert( "Installed" ); // (for Is_Not_Installed modifier)

   if( ( use_transients && is_field_transient( e_field_id_Installed ) )
    || ( !use_transients && !is_field_transient( e_field_id_Installed ) ) )
      names.insert( "Installed" );
   // [(finish modifier_field_value)] 600857

   // [<start get_always_required_field_names>]
//nyi
   dependents.insert( "Usage_Count" ); // (for Is_In_Use modifier)

   if( ( use_transients && is_field_transient( e_field_id_Usage_Count ) )
    || ( !use_transients && !is_field_transient( e_field_id_Usage_Count ) ) )
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

void Meta_Package::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
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

const char* Meta_Package::static_persistence_extra( )
{
   return "";
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
   all_field_info.push_back( field_info( "136102", "Actions", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "136104", "Install_Details", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "136103", "Installed", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "136109", "Installed_Order", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "136105", "Key", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "302800", "Model", "Meta_Model", true, "", "" ) );
   all_field_info.push_back( field_info( "136101", "Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "302810", "Package_Type", "Meta_Package_Type", true, "", "" ) );
   all_field_info.push_back( field_info( "136107", "Plural", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "136108", "Type_Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "136106", "Usage_Count", "int", false, "", "" ) );
}

void Meta_Package::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Model, make_pair( "Meta.136100", "Meta_Model" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Package_Type, make_pair( "Meta.136100", "Meta_Package_Type" ) ) );
}

int Meta_Package::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Package::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
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
      p_id = "136109";
      break;

      case 5:
      p_id = "136105";
      break;

      case 6:
      p_id = "302800";
      break;

      case 7:
      p_id = "136101";
      break;

      case 8:
      p_id = "302810";
      break;

      case 9:
      p_id = "136107";
      break;

      case 10:
      p_id = "136108";
      break;

      case 11:
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
      p_id = "Installed_Order";
      break;

      case 5:
      p_id = "Key";
      break;

      case 6:
      p_id = "Model";
      break;

      case 7:
      p_id = "Name";
      break;

      case 8:
      p_id = "Package_Type";
      break;

      case 9:
      p_id = "Plural";
      break;

      case 10:
      p_id = "Type_Name";
      break;

      case 11:
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
   else if( field == c_field_id_Installed_Order || field == c_field_name_Installed_Order )
      rc += 4;
   else if( field == c_field_id_Key || field == c_field_name_Key )
      rc += 5;
   else if( field == c_field_id_Model || field == c_field_name_Model )
      rc += 6;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 7;
   else if( field == c_field_id_Package_Type || field == c_field_name_Package_Type )
      rc += 8;
   else if( field == c_field_id_Plural || field == c_field_name_Plural )
      rc += 9;
   else if( field == c_field_id_Type_Name || field == c_field_name_Type_Name )
      rc += 10;
   else if( field == c_field_id_Usage_Count || field == c_field_name_Usage_Count )
      rc += 11;

   return rc - 1;
}

procedure_info_container& Meta_Package::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "136450", procedure_info( "Cancel_Remove" ) ) );
      procedures.insert( make_pair( "136430", procedure_info( "Check_Install" ) ) );
      procedures.insert( make_pair( "136440", procedure_info( "Complete_Remove" ) ) );
      procedures.insert( make_pair( "136410", procedure_info( "Install" ) ) );
      procedures.insert( make_pair( "136420", procedure_info( "Remove" ) ) );
   }

   return procedures;
}

string Meta_Package::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(75),"
    "C_Ver_ SMALLINT UNSIGNED NOT NULL,"
    "C_Rev_ BIGINT UNSIGNED NOT NULL,"
    "C_Sec_ BIGINT UNSIGNED NOT NULL,"
    "C_Typ_ VARCHAR(16) NOT NULL,"
    "C_Actions VARCHAR(200) NOT NULL,"
    "C_Installed INTEGER NOT NULL,"
    "C_Installed_Order VARCHAR(200) NOT NULL,"
    "C_Key VARCHAR(200) NOT NULL,"
    "C_Model VARCHAR(75) NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Package_Type VARCHAR(75) NOT NULL,"
    "C_Plural VARCHAR(200) NOT NULL,"
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

void Meta_Package::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "Model,Name", "string,string" ) );
   pairs.push_back( make_pair( "Model,Package_Type,Name", "string,string,string" ) );
}

void Meta_Package::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "Model,Name" );
   unique_indexes.push_back( "Model,Package_Type,Name" );
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

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

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
