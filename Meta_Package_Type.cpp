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

#include "Meta_Package_Type.h"

#include "Meta_Package.h"

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

inline int system( const string& cmd ) { return exec_system( cmd ); }

namespace
{

#include "Meta_Package_Type.cmh"

const int32_t c_version = 1;

const char* const c_okay = "okay";

const char* const c_field_id_Actions = "135105";
const char* const c_field_id_Dependencies = "135107";
const char* const c_field_id_File = "135103";
const char* const c_field_id_Installed = "135106";
const char* const c_field_id_Multi = "135102";
const char* const c_field_id_Name = "135101";
const char* const c_field_id_Plural = "135108";
const char* const c_field_id_Version = "135104";

const char* const c_field_name_Actions = "Actions";
const char* const c_field_name_Dependencies = "Dependencies";
const char* const c_field_name_File = "File";
const char* const c_field_name_Installed = "Installed";
const char* const c_field_name_Multi = "Multi";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Plural = "Plural";
const char* const c_field_name_Version = "Version";

const char* const c_field_display_name_Actions = "field_package_type_actions";
const char* const c_field_display_name_Dependencies = "field_package_type_dependencies";
const char* const c_field_display_name_File = "field_package_type_file";
const char* const c_field_display_name_Installed = "field_package_type_installed";
const char* const c_field_display_name_Multi = "field_package_type_multi";
const char* const c_field_display_name_Name = "field_package_type_name";
const char* const c_field_display_name_Plural = "field_package_type_plural";
const char* const c_field_display_name_Version = "field_package_type_version";

const int c_num_fields = 8;

const char* const c_all_sorted_field_ids[ ] =
{
   "135101",
   "135102",
   "135103",
   "135104",
   "135105",
   "135106",
   "135107",
   "135108"
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
   "Version"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool has_field( const string& field )
{
   return binary_search( c_all_sorted_field_ids, c_all_sorted_field_ids + c_num_fields, field.c_str( ), compare )
    || binary_search( c_all_sorted_field_names, c_all_sorted_field_names + c_num_fields, field.c_str( ), compare );
}

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

set< string > g_derivations;

typedef map< string, Meta_Package_Type* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Package_Type* > external_aliases_lookup_container;
typedef external_aliases_lookup_container::const_iterator external_aliases_lookup_const_iterator;

external_aliases_container g_external_aliases;
external_aliases_lookup_container g_external_aliases_lookup;

string gv_default_Actions = string( );
string gv_default_Dependencies = string( );
string gv_default_File = string( );
bool gv_default_Installed = bool( 0 );
bool gv_default_Multi = bool( 0 );
string gv_default_Name = string( "New_Package_Type" );
string gv_default_Plural = string( "New_Package_Type" );
int gv_default_Version = int( 0 );

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

   void handle_unknown_command( const string& command )
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
      bool want_fixed( has_parm_val( parameters, c_cmd_parm_Meta_Package_Type_key_fixed ) );

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
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Package_Type_get_field_name ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );
      else if( field_name == c_field_id_Actions || field_name == c_field_name_Actions )
         string_getter< string >( cmd_handler.p_Meta_Package_Type->Actions( ), cmd_handler.retval );
      else if( field_name == c_field_id_Dependencies || field_name == c_field_name_Dependencies )
         string_getter< string >( cmd_handler.p_Meta_Package_Type->Dependencies( ), cmd_handler.retval );
      else if( field_name == c_field_id_File || field_name == c_field_name_File )
         string_getter< string >( cmd_handler.p_Meta_Package_Type->File( ), cmd_handler.retval );
      else if( field_name == c_field_id_Installed || field_name == c_field_name_Installed )
         string_getter< bool >( cmd_handler.p_Meta_Package_Type->Installed( ), cmd_handler.retval );
      else if( field_name == c_field_id_Multi || field_name == c_field_name_Multi )
         string_getter< bool >( cmd_handler.p_Meta_Package_Type->Multi( ), cmd_handler.retval );
      else if( field_name == c_field_id_Name || field_name == c_field_name_Name )
         string_getter< string >( cmd_handler.p_Meta_Package_Type->Name( ), cmd_handler.retval );
      else if( field_name == c_field_id_Plural || field_name == c_field_name_Plural )
         string_getter< string >( cmd_handler.p_Meta_Package_Type->Plural( ), cmd_handler.retval );
      else if( field_name == c_field_id_Version || field_name == c_field_name_Version )
         string_getter< int >( cmd_handler.p_Meta_Package_Type->Version( ), cmd_handler.retval );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Package_Type_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Package_Type_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_parm_Meta_Package_Type_set_field_value ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );
      else if( field_name == c_field_id_Actions || field_name == c_field_name_Actions )
         func_string_setter< Meta_Package_Type, string >(
          *cmd_handler.p_Meta_Package_Type, &Meta_Package_Type::Actions, field_value );
      else if( field_name == c_field_id_Dependencies || field_name == c_field_name_Dependencies )
         func_string_setter< Meta_Package_Type, string >(
          *cmd_handler.p_Meta_Package_Type, &Meta_Package_Type::Dependencies, field_value );
      else if( field_name == c_field_id_File || field_name == c_field_name_File )
         func_string_setter< Meta_Package_Type, string >(
          *cmd_handler.p_Meta_Package_Type, &Meta_Package_Type::File, field_value );
      else if( field_name == c_field_id_Installed || field_name == c_field_name_Installed )
         func_string_setter< Meta_Package_Type, bool >(
          *cmd_handler.p_Meta_Package_Type, &Meta_Package_Type::Installed, field_value );
      else if( field_name == c_field_id_Multi || field_name == c_field_name_Multi )
         func_string_setter< Meta_Package_Type, bool >(
          *cmd_handler.p_Meta_Package_Type, &Meta_Package_Type::Multi, field_value );
      else if( field_name == c_field_id_Name || field_name == c_field_name_Name )
         func_string_setter< Meta_Package_Type, string >(
          *cmd_handler.p_Meta_Package_Type, &Meta_Package_Type::Name, field_value );
      else if( field_name == c_field_id_Plural || field_name == c_field_name_Plural )
         func_string_setter< Meta_Package_Type, string >(
          *cmd_handler.p_Meta_Package_Type, &Meta_Package_Type::Plural, field_value );
      else if( field_name == c_field_id_Version || field_name == c_field_name_Version )
         func_string_setter< Meta_Package_Type, int >(
          *cmd_handler.p_Meta_Package_Type, &Meta_Package_Type::Version, field_value );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Package_Type_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Package_Type_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_parm_Meta_Package_Type_cmd_cmd_and_args ) );

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
   void impl_Actions( const string& Actions ) { v_Actions = Actions; }

   const string& impl_Dependencies( ) const { return lazy_fetch( p_obj ), v_Dependencies; }
   void impl_Dependencies( const string& Dependencies ) { v_Dependencies = Dependencies; }

   const string& impl_File( ) const { return lazy_fetch( p_obj ), v_File; }
   void impl_File( const string& File ) { v_File = File; }

   bool impl_Installed( ) const { return lazy_fetch( p_obj ), v_Installed; }
   void impl_Installed( bool Installed ) { v_Installed = Installed; }

   bool impl_Multi( ) const { return lazy_fetch( p_obj ), v_Multi; }
   void impl_Multi( bool Multi ) { v_Multi = Multi; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { v_Name = Name; }

   const string& impl_Plural( ) const { return lazy_fetch( p_obj ), v_Plural; }
   void impl_Plural( const string& Plural ) { v_Plural = Plural; }

   int impl_Version( ) const { return lazy_fetch( p_obj ), v_Version; }
   void impl_Version( int Version ) { v_Version = Version; }

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

   Meta_Package_Type* p_obj;
   class_pointer< Meta_Package_Type > cp_obj;

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
   int v_Version;

   mutable class_pointer< Meta_Package > cp_child_Package;
};

void Meta_Package_Type::impl::impl_Install( )
{
   // [<start Install_impl>]
//nyi
   string package_file;
   string info_filename( "package.info" );

   if( storage_locked_for_admin( ) )
      info_filename = get_obj( ).get_attached_file_path( get_obj( ).get_key( ) + ".info" );
   else
   {
      package_file = get_obj( ).get_attached_file_path( get_obj( ).File( ) );

#ifdef _WIN32
      string cmd( "unbundle -o " );
#else
      string cmd( "./unbundle -o " );
#endif
      cmd += package_file;

      // NOTE: If is project's own environment then don't extract specification files
      // that are expected to already be under source control (as otherwise these can
      // cause issues with the source control software due to CR/LF's).
      if( storage_name( ) == "ciyam" )
         cmd += " -x for_auto_code*.spec.xrep";

      exec_system( cmd, false );

      if( !exists_file( info_filename ) )
         throw runtime_error( "Missing package information file (corrupt or incorrect bundle?)." );
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

   get_obj( ).Name( name );
   get_obj( ).Plural( plural );
   get_obj( ).Multi( is_multi );
   get_obj( ).Version( version );

   string dependencies;
   for( size_t i = 1; i < lines.size( ); i++ )
   {
      string next( lines[ i ] );

      if( !dependencies.empty( ) )
         dependencies += "\r\n";
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
#ifndef _WIN32
      exec_system( "./install_package " + name, false );
#else
      exec_system( "install_package.bat " + name, false );
#endif

      if( exists_file( name + ".specs.sio" ) )
      {
         set_session_variable( "@name", name );
         run_script( "install_package" );
      }

      copy_file( info_filename, name + "." + info_filename );
      copy_file( info_filename, get_obj( ).get_attached_file_path( get_obj( ).get_key( ) + ".info" ) );

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
      retval = to_string( impl_Version( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
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
      func_string_setter< Meta_Package_Type::impl, int >( *this, &Meta_Package_Type::impl::impl_Version, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }
}

uint64_t Meta_Package_Type::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start protect_equal)]
   if( check_equal( get_obj( ).Installed( ), true ) )
      state |= ( c_state_uneditable | c_state_undeletable );
   // [(finish protect_equal)]

   // [(start modifier_field_value)]
   if( get_obj( ).Installed( ) == true )
      state |= c_modifier_Is_Installed;
   // [(finish modifier_field_value)]

   // [<start get_state>]
   // [<finish get_state>]

   return state;
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

void Meta_Package_Type::impl::clear( )
{
   v_Actions = gv_default_Actions;
   v_Dependencies = gv_default_Dependencies;
   v_File = gv_default_File;
   v_Installed = gv_default_Installed;
   v_Multi = gv_default_Multi;
   v_Name = gv_default_Name;
   v_Plural = gv_default_Plural;
   v_Version = gv_default_Version;
}

bool Meta_Package_Type::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Package_Type::impl::validate( unsigned state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   if( is_null( v_Name ) && !value_will_be_provided( c_field_name_Name ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Name,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Name ) ) ) ) );

   if( is_null( v_Plural ) && !value_will_be_provided( c_field_name_Plural ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Plural,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Plural ) ) ) ) );

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

void Meta_Package_Type::impl::after_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Package_Type::impl::finalise_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Package_Type::impl::at_create( )
{
   // [(start file_attachment)]
   // NOTE: This is being done to ensure that clones won't end up owning another instance's file.
   get_obj( ).File( "" );
   // [(finish file_attachment)]

   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Package_Type::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_empty_action)]
   if( !get_obj( ).get_key( ).empty( ) )
   {
      if( is_null( get_obj( ).File( ) ) )
         get_obj( ).Actions( "" );
      else
         get_obj( ).Actions( "135410" );
   }
   // [(finish field_empty_action)]

   // [(start field_clear)]
   if( get_obj( ).Installed( ) )
      get_obj( ).Actions( "" );
   // [(finish field_clear)]

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Package_Type::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Package_Type::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Package_Type::impl::can_destroy( bool is_internal )
{
   bool retval = is_internal || !( get_state( ) & c_state_undeletable );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Package_Type::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   // [(start file_attachment)]
   if( !storage_locked_for_admin( ) && !get_obj( ).File( ).empty( ) )
      remove_file( get_obj( ).get_attached_file_path( get_obj( ).File( ) ) );
   // [(finish file_attachment)]

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Package_Type::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Package_Type::impl::set_default_values( )
{
   clear( );
}

bool Meta_Package_Type::impl::is_filtered( ) const
{
   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
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

int Meta_Package_Type::Version( ) const
{
   return p_impl->impl_Version( );
}

void Meta_Package_Type::Version( int Version )
{
   p_impl->impl_Version( Version );
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

bool Meta_Package_Type::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
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

void Meta_Package_Type::validate( unsigned state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Package_Type::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Package_Type::finalise_fetch( )
{
   p_impl->finalise_fetch( );
}

void Meta_Package_Type::at_create( )
{
   p_impl->at_create( );
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

string Meta_Package_Type::get_field_display_name( const string& id ) const
{
   string display_name;

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_display_name" );
   else if( id == c_field_id_Actions )
      display_name = get_module_string( c_field_display_name_Actions );
   else if( id == c_field_id_Dependencies )
      display_name = get_module_string( c_field_display_name_Dependencies );
   else if( id == c_field_id_File )
      display_name = get_module_string( c_field_display_name_File );
   else if( id == c_field_id_Installed )
      display_name = get_module_string( c_field_display_name_Installed );
   else if( id == c_field_id_Multi )
      display_name = get_module_string( c_field_display_name_Multi );
   else if( id == c_field_id_Name )
      display_name = get_module_string( c_field_display_name_Name );
   else if( id == c_field_id_Plural )
      display_name = get_module_string( c_field_display_name_Plural );
   else if( id == c_field_id_Version )
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

class_base* Meta_Package_Type::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   if( child_num >= 1 )
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
            next_child_field = "302810";
            p_class_base = &child_Package( );
         }
         break;
      }
   }

   return p_class_base;
}

const char* Meta_Package_Type::class_id( ) const
{
   return static_class_id( );
}

const char* Meta_Package_Type::class_name( ) const
{
   return static_class_name( );
}

const char* Meta_Package_Type::plural_name( ) const
{
   return static_plural_name( );
}

const char* Meta_Package_Type::module_id( ) const
{
   return static_module_id( );
}

const char* Meta_Package_Type::module_name( ) const
{
   return static_module_name( );
}

string Meta_Package_Type::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );
   key += "package_type";

   return get_module_string( key );
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
   else if( sub_context == "_302810" || sub_context == "child_Package" )
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
   names.push_back( "C_Version" );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
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
   values.push_back( to_string( Version( ) ) );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Package_Type::get_required_field_names(
 set< string >& names, bool required_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, required_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Package_Type::get_always_required_field_names(
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
   dependents.insert( "Installed" ); // (for Is_Installed modifier)

   if( ( required_transients && is_field_transient( e_field_id_Installed ) )
    || ( !required_transients && !is_field_transient( e_field_id_Installed ) ) )
      names.insert( "Installed" );
   // [(finish modifier_field_value)]

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

void Meta_Package_Type::do_generate_sql( generate_sql_type type, vector< string >& sql_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts );
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
   all_field_info.push_back( field_info( "135105", "Actions", "string", false ) );
   all_field_info.push_back( field_info( "135107", "Dependencies", "string", false ) );
   all_field_info.push_back( field_info( "135103", "File", "string", false ) );
   all_field_info.push_back( field_info( "135106", "Installed", "bool", false ) );
   all_field_info.push_back( field_info( "135102", "Multi", "bool", false ) );
   all_field_info.push_back( field_info( "135101", "Name", "string", false ) );
   all_field_info.push_back( field_info( "135108", "Plural", "string", false ) );
   all_field_info.push_back( field_info( "135104", "Version", "int", false ) );
}

void Meta_Package_Type::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;
}

int Meta_Package_Type::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;

   return c_num_fields;
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
   else if( field == c_field_id_Version || field == c_field_name_Version )
      rc += 8;

   return rc - 1;
}

procedure_info_container& Meta_Package_Type::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "135410", "Install" ) );
   }

   return procedures;
}

string Meta_Package_Type::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(64),"
    "C_Ver_ INTEGER NOT NULL,"
    "C_Rev_ INTEGER NOT NULL,"
    "C_Typ_ VARCHAR(24) NOT NULL,"
    "C_Actions VARCHAR(200) NOT NULL,"
    "C_Dependencies TEXT NOT NULL,"
    "C_File VARCHAR(200) NOT NULL,"
    "C_Installed INTEGER NOT NULL,"
    "C_Multi INTEGER NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Plural VARCHAR(200) NOT NULL,"
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

