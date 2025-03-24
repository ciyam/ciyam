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

#include "Meta_Package_Type.h"

#include "Meta_Application_Script.h"
#include "Meta_Package.h"

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

#include "Meta_Package_Type.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Actions = "135105";
const char* const c_field_id_Dependencies = "135107";
const char* const c_field_id_File = "135103";
const char* const c_field_id_Installed = "135106";
const char* const c_field_id_Multi = "135102";
const char* const c_field_id_Name = "135101";
const char* const c_field_id_Plural = "135108";
const char* const c_field_id_Single = "135109";
const char* const c_field_id_Version = "135104";

const char* const c_field_name_Actions = "Actions";
const char* const c_field_name_Dependencies = "Dependencies";
const char* const c_field_name_File = "File";
const char* const c_field_name_Installed = "Installed";
const char* const c_field_name_Multi = "Multi";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Plural = "Plural";
const char* const c_field_name_Single = "Single";
const char* const c_field_name_Version = "Version";

const char* const c_field_display_name_Actions = "field_package_type_actions";
const char* const c_field_display_name_Dependencies = "field_package_type_dependencies";
const char* const c_field_display_name_File = "field_package_type_file";
const char* const c_field_display_name_Installed = "field_package_type_installed";
const char* const c_field_display_name_Multi = "field_package_type_multi";
const char* const c_field_display_name_Name = "field_package_type_name";
const char* const c_field_display_name_Plural = "field_package_type_plural";
const char* const c_field_display_name_Single = "field_package_type_single";
const char* const c_field_display_name_Version = "field_package_type_version";

const int c_num_fields = 9;

const char* const c_all_sorted_field_ids[ ] =
{
   "135101",
   "135102",
   "135103",
   "135104",
   "135105",
   "135106",
   "135107",
   "135108",
   "135109"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Actions",
   "Dependencies",
   "File",
   "Installed",
   "Multi",
   "Name",
   "Plural",
   "Single",
   "Version"
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

const char* const c_procedure_id_Install = "135410";

const uint64_t c_modifier_Is_Installed = UINT64_C( 0x100 );

aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Name_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Plural_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Single_domain;

string g_group_field_name;
string g_level_field_name;
string g_order_field_name;
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_Package_Type* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Package_Type* > external_aliases_lookup_container;
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

string g_default_Actions = string( );
string g_default_Dependencies = string( );
string g_default_File = string( );
bool g_default_Installed = bool( 0 );
bool g_default_Multi = bool( 0 );
string g_default_Name = string( "New_Package_Type" );
string g_default_Plural = string( "New_Package_Type" );
string g_default_Single = string( "New_Package_Type" );
int g_default_Version = int( 0 );

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Package_Type > Package_Type_registration( get_class_registry( ), "135100" );

class Meta_Package_Type_command_functor;

class Meta_Package_Type_command_handler : public command_handler
{
   friend class Meta_Package_Type_command_functor;

   public:
   Meta_Package_Type_command_handler( ) : p_Meta_Package_Type( 0 ) { }

   void set_Meta_Package_Type( Meta_Package_Type* p_new_Meta_Package_Type ) { p_Meta_Package_Type = p_new_Meta_Package_Type; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Package_Type* p_Meta_Package_Type;

   protected:
   string retval;
};

class Meta_Package_Type_command_functor : public command_functor
{
   public:
   Meta_Package_Type_command_functor( Meta_Package_Type_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Package_Type_command_handler& cmd_handler;
};

command_functor* Meta_Package_Type_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Package_Type_command_functor( dynamic_cast< Meta_Package_Type_command_handler& >( handler ) );
}

void Meta_Package_Type_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Package_Type_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_Package_Type_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Package_Type->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Package_Type->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Package_Type_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Package_Type->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Package_Type->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Package_Type_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Package_Type_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Actions ) || ( field_name == c_field_name_Actions ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package_Type->Actions( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Dependencies ) || ( field_name == c_field_name_Dependencies ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package_Type->Dependencies( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_File ) || ( field_name == c_field_name_File ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package_Type->File( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Installed ) || ( field_name == c_field_name_Installed ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Type->Installed( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Multi ) || ( field_name == c_field_name_Multi ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Package_Type->Multi( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package_Type->Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Plural ) || ( field_name == c_field_name_Plural ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package_Type->Plural( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Single ) || ( field_name == c_field_name_Single ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Package_Type->Single( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Version ) || ( field_name == c_field_name_Version ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Package_Type->Version( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Package_Type_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Package_Type_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_Package_Type_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Actions ) || ( field_name == c_field_name_Actions ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Type, string >(
          *cmd_handler.p_Meta_Package_Type, &Meta_Package_Type::Actions, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Dependencies ) || ( field_name == c_field_name_Dependencies ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Type, string >(
          *cmd_handler.p_Meta_Package_Type, &Meta_Package_Type::Dependencies, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_File ) || ( field_name == c_field_name_File ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Type, string >(
          *cmd_handler.p_Meta_Package_Type, &Meta_Package_Type::File, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Installed ) || ( field_name == c_field_name_Installed ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Type, bool >(
          *cmd_handler.p_Meta_Package_Type, &Meta_Package_Type::Installed, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Multi ) || ( field_name == c_field_name_Multi ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Type, bool >(
          *cmd_handler.p_Meta_Package_Type, &Meta_Package_Type::Multi, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Type, string >(
          *cmd_handler.p_Meta_Package_Type, &Meta_Package_Type::Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Plural ) || ( field_name == c_field_name_Plural ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Type, string >(
          *cmd_handler.p_Meta_Package_Type, &Meta_Package_Type::Plural, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Single ) || ( field_name == c_field_name_Single ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Type, string >(
          *cmd_handler.p_Meta_Package_Type, &Meta_Package_Type::Single, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Version ) || ( field_name == c_field_name_Version ) ) )
      {
         handled = true;

         func_string_setter< Meta_Package_Type, int >(
          *cmd_handler.p_Meta_Package_Type, &Meta_Package_Type::Version, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Package_Type_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Package_Type_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_Package_Type_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
   else if( command == c_cmd_Meta_Package_Type_Install )
   {
      cmd_handler.p_Meta_Package_Type->Install( );

      cmd_handler.retval.erase( );

   }
}

struct Meta_Package_Type::impl : public Meta_Package_Type_command_handler
{
   impl( Meta_Package_Type& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Package_Type( &o );

      add_commands( 0, Meta_Package_Type_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Package_Type_command_definitions ) );
   }

   Meta_Package_Type& get_obj( ) const
   {
      return *cp_obj;
   }

   const string& impl_Actions( ) const { return lazy_fetch( p_obj ), v_Actions; }
   void impl_Actions( const string& Actions ) { sanity_check( Actions ); v_Actions = Actions; }

   const string& impl_Dependencies( ) const { return lazy_fetch( p_obj ), v_Dependencies; }
   void impl_Dependencies( const string& Dependencies ) { sanity_check( Dependencies ); v_Dependencies = Dependencies; }

   const string& impl_File( ) const { return lazy_fetch( p_obj ), v_File; }
   void impl_File( const string& File ) { sanity_check( File ); v_File = File; }

   bool impl_Installed( ) const { return lazy_fetch( p_obj ), v_Installed; }
   void impl_Installed( bool Installed ) { v_Installed = Installed; }

   bool impl_Multi( ) const { return lazy_fetch( p_obj ), v_Multi; }
   void impl_Multi( bool Multi ) { v_Multi = Multi; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { sanity_check( Name ); v_Name = Name; }

   const string& impl_Plural( ) const { return lazy_fetch( p_obj ), v_Plural; }
   void impl_Plural( const string& Plural ) { sanity_check( Plural ); v_Plural = Plural; }

   const string& impl_Single( ) const { return lazy_fetch( p_obj ), v_Single; }
   void impl_Single( const string& Single ) { sanity_check( Single ); v_Single = Single; }

   int impl_Version( ) const { return lazy_fetch( p_obj ), v_Version; }
   void impl_Version( int Version ) { v_Version = Version; }

   Meta_Application_Script& impl_child_Application_Script( )
   {
      if( !cp_child_Application_Script )
      {
         cp_child_Application_Script.init( );

         p_obj->setup_graph_parent( *cp_child_Application_Script, "302840" );
      }
      return *cp_child_Application_Script;
   }

   const Meta_Application_Script& impl_child_Application_Script( ) const
   {
      if( !cp_child_Application_Script )
      {
         cp_child_Application_Script.init( );

         p_obj->setup_graph_parent( *cp_child_Application_Script, "302840" );
      }
      return *cp_child_Application_Script;
   }

   Meta_Package& impl_child_Package( )
   {
      if( !cp_child_Package )
      {
         cp_child_Package.init( );

         p_obj->setup_graph_parent( *cp_child_Package, "302810" );
      }
      return *cp_child_Package;
   }

   const Meta_Package& impl_child_Package( ) const
   {
      if( !cp_child_Package )
      {
         cp_child_Package.init( );

         p_obj->setup_graph_parent( *cp_child_Package, "302810" );
      }
      return *cp_child_Package;
   }

   void impl_Install( );

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

   Meta_Package_Type* p_obj;
   class_pointer< Meta_Package_Type > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   string v_Actions;
   string v_Dependencies;
   string v_File;
   bool v_Installed;
   bool v_Multi;
   string v_Name;
   string v_Plural;
   string v_Single;
   int v_Version;

   mutable class_pointer< Meta_Application_Script > cp_child_Application_Script;
   mutable class_pointer< Meta_Package > cp_child_Package;
};

void Meta_Package_Type::impl::impl_Install( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Install_impl>]
//nyi
   string package_file;
   string info_filename( "package.info" );

   string path( get_session_variable( get_special_var_name( e_special_var_package_type_path ) ) );

   if( storage_locked_for_admin( ) )
      info_filename = get_obj( ).get_attached_file_path( get_obj( ).get_key( ) + ".info" );
   else
   {
      if( !path.empty( ) )
         package_file = path + "/" + get_obj( ).File( );
      else
         package_file = get_obj( ).get_attached_file_path( get_obj( ).File( ) );

      string cmd( "./unbundle -o -q " );

      cmd += package_file;

      cmd += " >/dev/null";

      exec_system( cmd, false );

      if( !exists_file( info_filename ) )
         throw runtime_error( "Missing package information file (corrupt or incorrect bundle '" + get_obj( ).File( ) + "'?)." );
   }

   vector< string > lines;
   read_file_lines( info_filename, lines );

   if( lines.empty( ) )
      throw runtime_error( "Package information file is empty." );

   string details( lines[ 0 ] );
   string::size_type pos = details.find( ' ' );
   if( pos == string::npos )
      throw runtime_error( "unexpected package details: " + lines[ 0 ] );

   string name( details.substr( 0, pos ) );
   details.erase( 0, pos + 1 );

   pos = details.find( ' ' );
   if( pos == string::npos )
      throw runtime_error( "unexpected package details: " + lines[ 0 ] );

   string plural( details.substr( 0, pos ) );
   details.erase( 0, pos + 1 );

   pos = details.find( ' ' );
   if( pos == string::npos )
      throw runtime_error( "unexpected package details: " + lines[ 0 ] );

   int version( atoi( details.substr( 0, pos ).c_str( ) ) );
   details.erase( 0, pos + 1 );

   bool is_multi = false;

   if( lower( details ) == "multi" || lower( details ) == "multiple" )
      is_multi = true;
   else if( lower( details ) != "single" )
      throw runtime_error( "unexpected package details: " + lines[ 0 ] );

   string key( get_obj( ).get_key( ) );

   map< string, int > package_versions;

   if( !storage_locked_for_admin( ) && get_obj( ).iterate_forwards( ) )
   {
      do
      {
         if( get_obj( ).Installed( ) )
            package_versions.insert( make_pair( get_obj( ).Name( ), get_obj( ).Version( ) ) );
      } while( get_obj( ).iterate_next( ) );
   }

   get_obj( ).op_update( key );

   string single( name );

   pos = name.find( ':' );

   if( pos != string::npos )
   {
      single = name.substr( pos + 1 );
      name.erase( pos );
   }

   get_obj( ).Name( name );
   get_obj( ).Single( single );
   get_obj( ).Plural( plural );
   get_obj( ).Multi( is_multi );
   get_obj( ).Version( version );

   string dependencies;
   vector< string > application_scripts;

   for( size_t i = 1; i < lines.size( ); i++ )
   {
      string next( lines[ i ] );

      if( next.find( c_application_script_prefix ) == 0 )
      {
         application_scripts.push_back( next.substr( strlen( c_application_script_prefix ) ) );
         continue;
      }

      if( !dependencies.empty( ) )
         dependencies += '\n';
      dependencies += next;

      if( !storage_locked_for_admin( ) )
      {
         string::size_type pos = next.find( ' ' );
         if( pos == string::npos )
            throw runtime_error( "dependency " + next + " is missing version" );

         string name = next.substr( 0, pos );
         int version = atoi( next.substr( pos + 1 ).c_str( ) );

         if( !package_versions.count( name ) )
            throw runtime_error( "This package type requires '" + name + "' to have first been installed." );

         if( package_versions[ name ] < version )
            throw runtime_error( "This package type requires '" + name + "' to be at least version " + to_string( version ) + "." );
      }
   }

   get_obj( ).Dependencies( dependencies );
   get_obj( ).Installed( true );
   get_obj( ).File( "" );

   get_obj( ).op_apply( );

   if( !storage_locked_for_admin( ) )
   {
      for( size_t i = 0; i < application_scripts.size( ); i++ )
      {
         string next( application_scripts[ i ] );

         string::size_type pos = next.find( '=' );

         if( pos == string::npos )
            throw runtime_error( "invalid application script info: " + next );

         string key( next.substr( 0, pos ) );

         string::size_type spos = next.find( ',', pos + 1 );

         if( spos == string::npos )
            throw runtime_error( "invalid application script info: " + next );

         string name( next.substr( spos + 1 ) );
         string script_name( next.substr( pos + 1, spos - pos - 1 ) );

         get_obj( ).child_Application_Script( ).Name( name );
         get_obj( ).child_Application_Script( ).Script_Name( script_name );
         get_obj( ).child_Application_Script( ).Package_Type( get_obj( ).get_key( ) );

         string fields_and_values( get_obj( ).child_Application_Script( ).get_fields_and_values( ) );

         set_session_variable( get_special_var_name( e_special_var_key ), key );
         set_session_variable( get_special_var_name( e_special_var_name ), name );
         set_session_variable( get_special_var_name( e_special_var_fields_and_values ), fields_and_values );

         run_script( "install_application_script", false, true, true );
      }

      string extra( get_session_variable( get_special_var_name( e_special_var_package_install_extra ) ) );

      if( !extra.empty( ) )
         extra = " " + extra;

      exec_system( "./install_package " + name + extra + " >/dev/null", false );

      if( exists_file( name + ".specs.sio" ) )
      {
         set_session_variable( get_special_var_name( e_special_var_name ), name );
         run_script( "install_package", false, true );
      }

      copy_file( info_filename, name + "." + info_filename );
      create_directories_for_file_name( get_obj( ).get_attached_file_path( get_obj( ).get_key( ) + ".info" ) );

      copy_file( info_filename, get_obj( ).get_attached_file_path( get_obj( ).get_key( ) + ".info" ) );

      if( path.empty( ) )
         remove_file( package_file );

      remove_file( info_filename );
   }
   // [<finish Install_impl>]
}

string Meta_Package_Type::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Actions( ) );
      break;

      case 1:
      retval = to_string( impl_Dependencies( ) );
      break;

      case 2:
      retval = to_string( impl_File( ) );
      break;

      case 3:
      retval = to_string( impl_Installed( ) );
      break;

      case 4:
      retval = to_string( impl_Multi( ) );
      break;

      case 5:
      retval = to_string( impl_Name( ) );
      break;

      case 6:
      retval = to_string( impl_Plural( ) );
      break;

      case 7:
      retval = to_string( impl_Single( ) );
      break;

      case 8:
      retval = to_string( impl_Version( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_Package_Type::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Package_Type::impl, string >( *this, &Meta_Package_Type::impl::impl_Actions, value );
      break;

      case 1:
      func_string_setter< Meta_Package_Type::impl, string >( *this, &Meta_Package_Type::impl::impl_Dependencies, value );
      break;

      case 2:
      func_string_setter< Meta_Package_Type::impl, string >( *this, &Meta_Package_Type::impl::impl_File, value );
      break;

      case 3:
      func_string_setter< Meta_Package_Type::impl, bool >( *this, &Meta_Package_Type::impl::impl_Installed, value );
      break;

      case 4:
      func_string_setter< Meta_Package_Type::impl, bool >( *this, &Meta_Package_Type::impl::impl_Multi, value );
      break;

      case 5:
      func_string_setter< Meta_Package_Type::impl, string >( *this, &Meta_Package_Type::impl::impl_Name, value );
      break;

      case 6:
      func_string_setter< Meta_Package_Type::impl, string >( *this, &Meta_Package_Type::impl::impl_Plural, value );
      break;

      case 7:
      func_string_setter< Meta_Package_Type::impl, string >( *this, &Meta_Package_Type::impl::impl_Single, value );
      break;

      case 8:
      func_string_setter< Meta_Package_Type::impl, int >( *this, &Meta_Package_Type::impl::impl_Version, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_Package_Type::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Actions( g_default_Actions );
      break;

      case 1:
      impl_Dependencies( g_default_Dependencies );
      break;

      case 2:
      impl_File( g_default_File );
      break;

      case 3:
      impl_Installed( g_default_Installed );
      break;

      case 4:
      impl_Multi( g_default_Multi );
      break;

      case 5:
      impl_Name( g_default_Name );
      break;

      case 6:
      impl_Plural( g_default_Plural );
      break;

      case 7:
      impl_Single( g_default_Single );
      break;

      case 8:
      impl_Version( g_default_Version );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_Package_Type::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Actions == g_default_Actions );
      break;

      case 1:
      retval = ( v_Dependencies == g_default_Dependencies );
      break;

      case 2:
      retval = ( v_File == g_default_File );
      break;

      case 3:
      retval = ( v_Installed == g_default_Installed );
      break;

      case 4:
      retval = ( v_Multi == g_default_Multi );
      break;

      case 5:
      retval = ( v_Name == g_default_Name );
      break;

      case 6:
      retval = ( v_Plural == g_default_Plural );
      break;

      case 7:
      retval = ( v_Single == g_default_Single );
      break;

      case 8:
      retval = ( v_Version == g_default_Version );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_Package_Type::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start protect_equal)] 600802
   if( check_equal( get_obj( ).Installed( ), true ) )
      state |= c_state_uneditable;
   // [(finish protect_equal)] 600802

   // [(start modifier_field_value)] 600804
   if( get_obj( ).Installed( ) == true )
      state |= c_modifier_Is_Installed;
   // [(finish modifier_field_value)] 600804

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

string Meta_Package_Type::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   if( state & c_modifier_Is_Installed )
      state_names += "|" + string( "Is_Installed" );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_Package_Type::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Package_Type::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Package_Type::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Package_Type::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Package_Type::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   ( void )foreign_key_values;
}

void Meta_Package_Type::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_Package_Type::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Package_Type::impl::clear( )
{
   v_Actions = g_default_Actions;
   v_Dependencies = g_default_Dependencies;
   v_File = g_default_File;
   v_Installed = g_default_Installed;
   v_Multi = g_default_Multi;
   v_Name = g_default_Name;
   v_Plural = g_default_Plural;
   v_Single = g_default_Single;
   v_Version = g_default_Version;
}

bool Meta_Package_Type::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Package_Type::impl::validate(
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

   if( is_null( v_Plural ) && !value_will_be_provided( c_field_name_Plural ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Plural,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Plural ) ) ) ) );

   if( is_null( v_Single ) && !value_will_be_provided( c_field_name_Single ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Single,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Single ) ) ) ) );

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

   if( !is_null( v_Single )
    && ( v_Single != g_default_Single
    || !value_will_be_provided( c_field_name_Single ) )
    && !g_Single_domain.is_valid( v_Single, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Single,
       get_module_string( c_field_display_name_Single ) + " " + error_message ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Package_Type::impl::validate_set_fields(
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

   if( !is_null( v_Single )
    && ( fields_set.count( c_field_id_Single ) || fields_set.count( c_field_name_Single ) )
    && !g_Single_domain.is_valid( v_Single, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Single,
       get_module_string( c_field_display_name_Single ) + " " + error_message ) );
}

void Meta_Package_Type::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Package_Type::impl::finalise_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Package_Type::impl::at_create( )
{
   // [(start file_attachment)] 600803
   // NOTE: This is being done to ensure that clones won't end up owning another instance's file.
   get_obj( ).File( "" );
   // [(finish file_attachment)] 600803

   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Package_Type::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_Package_Type::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_empty_action)] 600800
   if( !get_obj( ).get_key( ).empty( ) )
   {
      if( is_null( get_obj( ).File( ) ) )
         get_obj( ).Actions( "" );
      else
         get_obj( ).Actions( "135410" );
   }
   // [(finish field_empty_action)] 600800

   // [(start field_clear)] 600801
   if( get_obj( ).Installed( ) )
      get_obj( ).Actions( string( ) );
   // [(finish field_clear)] 600801

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Package_Type::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Package_Type::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_store>]
//nyi
   if( is_create
    && !is_internal && exists_file( get_obj( ).Name( ) + ".package.bun.gz" ) )
   {
      temporary_session_variable tmp_session_package_type_path( get_special_var_name( e_special_var_package_type_path ), "." );

      class_pointer< Meta_Package_Type > cp_other( e_create_instance );
      cp_other->perform_fetch( get_obj( ).get_key( ) );

      cp_other->File( get_obj( ).Name( ) + ".package.bun.gz" );

      cp_other->Install( );
   }
   // [<finish after_store>]
}

bool Meta_Package_Type::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Package_Type::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start file_attachment)] 600803
   if( !storage_locked_for_admin( ) && !get_obj( ).File( ).empty( ) )
      remove_file( get_obj( ).get_attached_file_path( get_obj( ).File( ) ) );
   // [(finish file_attachment)] 600803

   // [<start for_destroy>]
//nyi
   if( !storage_locked_for_admin( ) )
   {
      class_pointer< Meta_Application_Script > cp_application_scripts( e_create_instance );

      vector< string > application_scripts_to_remove;

      if( cp_application_scripts->iterate_forwards( ) )
      {
         do
         {
            string other_package_types( cp_application_scripts->Other_Package_Types( ) );

            if( !other_package_types.empty( ) )
            {
               set< string > all_other_package_types;

               split( other_package_types, all_other_package_types );

               if( all_other_package_types.count( get_obj( ).get_key( ) ) )
                  application_scripts_to_remove.push_back( cp_application_scripts->get_key( ) );
            }
         } while( cp_application_scripts->iterate_next( ) );
      }

      if( !application_scripts_to_remove.empty( ) )
      {
         for( size_t i = 0; i < application_scripts_to_remove.size( ); i++ )
         {
            cp_application_scripts->op_destroy( application_scripts_to_remove[ i ] );
            cp_application_scripts->op_apply( );
         }
      }
   }
   // [<finish for_destroy>]
}

void Meta_Package_Type::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Package_Type::impl::set_default_values( )
{
   clear( );
}

bool Meta_Package_Type::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_Package_Type::impl::get_required_transients( ) const
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

Meta_Package_Type::Meta_Package_Type( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Package_Type::~Meta_Package_Type( )
{
   cleanup( );
   delete p_impl;
}

const string& Meta_Package_Type::Actions( ) const
{
   return p_impl->impl_Actions( );
}

void Meta_Package_Type::Actions( const string& Actions )
{
   p_impl->impl_Actions( Actions );
}

const string& Meta_Package_Type::Dependencies( ) const
{
   return p_impl->impl_Dependencies( );
}

void Meta_Package_Type::Dependencies( const string& Dependencies )
{
   p_impl->impl_Dependencies( Dependencies );
}

const string& Meta_Package_Type::File( ) const
{
   return p_impl->impl_File( );
}

void Meta_Package_Type::File( const string& File )
{
   p_impl->impl_File( File );
}

bool Meta_Package_Type::Installed( ) const
{
   return p_impl->impl_Installed( );
}

void Meta_Package_Type::Installed( bool Installed )
{
   p_impl->impl_Installed( Installed );
}

bool Meta_Package_Type::Multi( ) const
{
   return p_impl->impl_Multi( );
}

void Meta_Package_Type::Multi( bool Multi )
{
   p_impl->impl_Multi( Multi );
}

const string& Meta_Package_Type::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_Package_Type::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

const string& Meta_Package_Type::Plural( ) const
{
   return p_impl->impl_Plural( );
}

void Meta_Package_Type::Plural( const string& Plural )
{
   p_impl->impl_Plural( Plural );
}

const string& Meta_Package_Type::Single( ) const
{
   return p_impl->impl_Single( );
}

void Meta_Package_Type::Single( const string& Single )
{
   p_impl->impl_Single( Single );
}

int Meta_Package_Type::Version( ) const
{
   return p_impl->impl_Version( );
}

void Meta_Package_Type::Version( int Version )
{
   p_impl->impl_Version( Version );
}

Meta_Application_Script& Meta_Package_Type::child_Application_Script( )
{
   return p_impl->impl_child_Application_Script( );
}

const Meta_Application_Script& Meta_Package_Type::child_Application_Script( ) const
{
   return p_impl->impl_child_Application_Script( );
}

Meta_Package& Meta_Package_Type::child_Package( )
{
   return p_impl->impl_child_Package( );
}

const Meta_Package& Meta_Package_Type::child_Package( ) const
{
   return p_impl->impl_child_Package( );
}

void Meta_Package_Type::Install( )
{
   p_impl->impl_Install( );
}

string Meta_Package_Type::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Package_Type::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_Package_Type::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_Package_Type::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_Package_Type::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_Package_Type::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_Package_Type::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_Package_Type::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_Package_Type::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_Package_Type::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Package_Type::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_Package_Type::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Package_Type::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Package_Type::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Package_Type::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Package_Type::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Package_Type::clear( )
{
   p_impl->clear( );
}

void Meta_Package_Type::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Package_Type::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Package_Type::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Package_Type::finalise_fetch( bool skip_set_original )
{
   p_impl->finalise_fetch( skip_set_original );
}

void Meta_Package_Type::at_create( )
{
   p_impl->at_create( );
}

void Meta_Package_Type::post_init( )
{
   p_impl->post_init( );
}

void Meta_Package_Type::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Package_Type::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Package_Type::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Package_Type::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Package_Type::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Package_Type::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Package_Type::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Package_Type::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Package_Type::get_field_id(
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
   else if( name == c_field_name_Dependencies )
   {
      p_id = c_field_id_Dependencies;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_File )
   {
      p_id = c_field_id_File;

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
   else if( name == c_field_name_Multi )
   {
      p_id = c_field_id_Multi;

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
   else if( name == c_field_name_Plural )
   {
      p_id = c_field_id_Plural;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Single )
   {
      p_id = c_field_id_Single;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Version )
   {
      p_id = c_field_id_Version;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_id;
}

const char* Meta_Package_Type::get_field_name(
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
   else if( id == c_field_id_Dependencies )
   {
      p_name = c_field_name_Dependencies;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_File )
   {
      p_name = c_field_name_File;

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
   else if( id == c_field_id_Multi )
   {
      p_name = c_field_name_Multi;

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
   else if( id == c_field_id_Plural )
   {
      p_name = c_field_name_Plural;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Single )
   {
      p_name = c_field_name_Single;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Version )
   {
      p_name = c_field_name_Version;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_name;
}

string& Meta_Package_Type::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_Package_Type::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_Package_Type::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_Package_Type::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_Package_Type::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_Package_Type::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Package_Type::get_field_uom_symbol( const string& id_or_name ) const
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
   if( ( id_or_name == c_field_id_Dependencies ) || ( id_or_name == c_field_name_Dependencies ) )
   {
      name = string( c_field_display_name_Dependencies );
      get_module_string( c_field_display_name_Dependencies, &next );
   }
   if( ( id_or_name == c_field_id_File ) || ( id_or_name == c_field_name_File ) )
   {
      name = string( c_field_display_name_File );
      get_module_string( c_field_display_name_File, &next );
   }
   if( ( id_or_name == c_field_id_Installed ) || ( id_or_name == c_field_name_Installed ) )
   {
      name = string( c_field_display_name_Installed );
      get_module_string( c_field_display_name_Installed, &next );
   }
   if( ( id_or_name == c_field_id_Multi ) || ( id_or_name == c_field_name_Multi ) )
   {
      name = string( c_field_display_name_Multi );
      get_module_string( c_field_display_name_Multi, &next );
   }
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
   {
      name = string( c_field_display_name_Name );
      get_module_string( c_field_display_name_Name, &next );
   }
   if( ( id_or_name == c_field_id_Plural ) || ( id_or_name == c_field_name_Plural ) )
   {
      name = string( c_field_display_name_Plural );
      get_module_string( c_field_display_name_Plural, &next );
   }
   if( ( id_or_name == c_field_id_Single ) || ( id_or_name == c_field_name_Single ) )
   {
      name = string( c_field_display_name_Single );
      get_module_string( c_field_display_name_Single, &next );
   }
   if( ( id_or_name == c_field_id_Version ) || ( id_or_name == c_field_name_Version ) )
   {
      name = string( c_field_display_name_Version );
      get_module_string( c_field_display_name_Version, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_Package_Type::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Actions ) || ( id_or_name == c_field_name_Actions ) )
      display_name = get_module_string( c_field_display_name_Actions );
   if( ( id_or_name == c_field_id_Dependencies ) || ( id_or_name == c_field_name_Dependencies ) )
      display_name = get_module_string( c_field_display_name_Dependencies );
   if( ( id_or_name == c_field_id_File ) || ( id_or_name == c_field_name_File ) )
      display_name = get_module_string( c_field_display_name_File );
   if( ( id_or_name == c_field_id_Installed ) || ( id_or_name == c_field_name_Installed ) )
      display_name = get_module_string( c_field_display_name_Installed );
   if( ( id_or_name == c_field_id_Multi ) || ( id_or_name == c_field_name_Multi ) )
      display_name = get_module_string( c_field_display_name_Multi );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
      display_name = get_module_string( c_field_display_name_Name );
   if( ( id_or_name == c_field_id_Plural ) || ( id_or_name == c_field_name_Plural ) )
      display_name = get_module_string( c_field_display_name_Plural );
   if( ( id_or_name == c_field_id_Single ) || ( id_or_name == c_field_name_Single ) )
      display_name = get_module_string( c_field_display_name_Single );
   if( ( id_or_name == c_field_id_Version ) || ( id_or_name == c_field_name_Version ) )
      display_name = get_module_string( c_field_display_name_Version );

   return display_name;
}

void Meta_Package_Type::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Package_Type::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Package_Type::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Package_Type::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Package_Type::setup_graph_parent( Meta_Application_Script& o, const string& foreign_key_field )
{
   static_cast< Meta_Application_Script& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Package_Type::setup_graph_parent( Meta_Package& o, const string& foreign_key_field )
{
   static_cast< Meta_Package& >( o ).set_graph_parent( this, foreign_key_field );
}

size_t Meta_Package_Type::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Package_Type::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Package_Type::get_num_foreign_key_children( bool is_internal ) const
{
   size_t rc = 2;

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

class_base* Meta_Package_Type::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   if( child_num >= 2 )
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
            next_child_field = "302840";
            p_class_base = &child_Application_Script( );
         }
         break;

         case 1:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302810";
            p_class_base = &child_Package( );
         }
         break;
      }
   }

   return p_class_base;
}

void Meta_Package_Type::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_Package_Type::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_Package_Type::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_Package_Type::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_Package_Type::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_Package_Type::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_Package_Type::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_Package_Type::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "package_type";

   return get_module_string( key );
}

string Meta_Package_Type::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_Package_Type::get_create_instance_info( ) const
{
   return "";
}

string Meta_Package_Type::get_update_instance_info( ) const
{
   return "";
}

string Meta_Package_Type::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_Package_Type::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );
   else if( procedure_id == "135410" ) // i.e. Install
      retval = "";

   return retval;
}

bool Meta_Package_Type::get_is_alias( ) const
{
   return false;
}

void Meta_Package_Type::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Package_Type::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Package_Type::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == "_302840" ) || ( sub_context == "child_Application_Script" ) )
      p_class_base = &child_Application_Script( );
   else if( ( sub_context == "_302810" ) || ( sub_context == "child_Package" ) )
      p_class_base = &child_Package( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Package_Type::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Actions" );
   names.push_back( "C_Dependencies" );
   names.push_back( "C_File" );
   names.push_back( "C_Installed" );
   names.push_back( "C_Multi" );
   names.push_back( "C_Name" );
   names.push_back( "C_Plural" );
   names.push_back( "C_Single" );
   names.push_back( "C_Version" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Package_Type::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Actions( ) ) ) );
   values.push_back( sql_quote( to_string( Dependencies( ) ) ) );
   values.push_back( sql_quote( to_string( File( ) ) ) );
   values.push_back( to_string( Installed( ) ) );
   values.push_back( to_string( Multi( ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( sql_quote( to_string( Plural( ) ) ) );
   values.push_back( sql_quote( to_string( Single( ) ) ) );
   values.push_back( to_string( Version( ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Package_Type::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Package_Type::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start protect_equal)] 600802
   dependents.insert( "Installed" );

   if( ( use_transients && is_field_transient( e_field_id_Installed ) )
    || ( !use_transients && !is_field_transient( e_field_id_Installed ) ) )
      names.insert( "Installed" );
   // [(finish protect_equal)] 600802

   // [(start modifier_field_value)] 600804
   dependents.insert( "Installed" ); // (for Is_Installed modifier)

   if( ( use_transients && is_field_transient( e_field_id_Installed ) )
    || ( !use_transients && !is_field_transient( e_field_id_Installed ) ) )
      names.insert( "Installed" );
   // [(finish modifier_field_value)] 600804

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Package_Type::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Package_Type::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_Package_Type::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Package_Type::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Package_Type::static_lock_class_id( )
{
   return "135100";
}

const char* Meta_Package_Type::static_check_class_name( )
{
   return "Package_Type";
}

const char* Meta_Package_Type::static_persistence_extra( )
{
   return "";
}

bool Meta_Package_Type::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Package_Type::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.135100" );
}

void Meta_Package_Type::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "135105", "Actions", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "135107", "Dependencies", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "135103", "File", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "135106", "Installed", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "135102", "Multi", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "135101", "Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "135108", "Plural", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "135109", "Single", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "135104", "Version", "int", false, "", "" ) );
}

void Meta_Package_Type::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;
}

int Meta_Package_Type::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Package_Type::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_Package_Type::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Package_Type::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "135105";
      break;

      case 2:
      p_id = "135107";
      break;

      case 3:
      p_id = "135103";
      break;

      case 4:
      p_id = "135106";
      break;

      case 5:
      p_id = "135102";
      break;

      case 6:
      p_id = "135101";
      break;

      case 7:
      p_id = "135108";
      break;

      case 8:
      p_id = "135109";
      break;

      case 9:
      p_id = "135104";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Package_Type" );

   return p_id;
}

const char* Meta_Package_Type::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Actions";
      break;

      case 2:
      p_id = "Dependencies";
      break;

      case 3:
      p_id = "File";
      break;

      case 4:
      p_id = "Installed";
      break;

      case 5:
      p_id = "Multi";
      break;

      case 6:
      p_id = "Name";
      break;

      case 7:
      p_id = "Plural";
      break;

      case 8:
      p_id = "Single";
      break;

      case 9:
      p_id = "Version";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Package_Type" );

   return p_id;
}

int Meta_Package_Type::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Actions || field == c_field_name_Actions )
      rc += 1;
   else if( field == c_field_id_Dependencies || field == c_field_name_Dependencies )
      rc += 2;
   else if( field == c_field_id_File || field == c_field_name_File )
      rc += 3;
   else if( field == c_field_id_Installed || field == c_field_name_Installed )
      rc += 4;
   else if( field == c_field_id_Multi || field == c_field_name_Multi )
      rc += 5;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 6;
   else if( field == c_field_id_Plural || field == c_field_name_Plural )
      rc += 7;
   else if( field == c_field_id_Single || field == c_field_name_Single )
      rc += 8;
   else if( field == c_field_id_Version || field == c_field_name_Version )
      rc += 9;

   return rc - 1;
}

procedure_info_container& Meta_Package_Type::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "135410", procedure_info( "Install" ) ) );
   }

   return procedures;
}

string Meta_Package_Type::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(75),"
    "C_Ver_ SMALLINT UNSIGNED NOT NULL,"
    "C_Rev_ BIGINT UNSIGNED NOT NULL,"
    "C_Sec_ BIGINT UNSIGNED NOT NULL,"
    "C_Typ_ VARCHAR(16) NOT NULL,"
    "C_Actions VARCHAR(200) NOT NULL,"
    "C_Dependencies TEXT NOT NULL,"
    "C_File VARCHAR(200) NOT NULL,"
    "C_Installed INTEGER NOT NULL,"
    "C_Multi INTEGER NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Plural VARCHAR(200) NOT NULL,"
    "C_Single VARCHAR(200) NOT NULL,"
    "C_Version INTEGER NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Package_Type::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Package_Type::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   ( void )pairs;
}

void Meta_Package_Type::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "Installed,Name", "bool,string" ) );
   pairs.push_back( make_pair( "Name", "string" ) );
}

void Meta_Package_Type::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "Installed,Name" );
   unique_indexes.push_back( "Name" );
}

void Meta_Package_Type::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Installed,C_Name" );
   indexes.push_back( "C_Name" );
}

void Meta_Package_Type::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Installed,C_Name" );
   indexes.push_back( "C_Name" );
}

void Meta_Package_Type::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Package_Type::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Package_Type::static_insert_external_alias( const string& module_and_class_id, Meta_Package_Type* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Package_Type::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Package_Type::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Package_Type::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
