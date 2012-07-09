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

#include "Meta_Relationship.h"

#include "Meta_List_Field.h"
#include "Meta_Specification.h"
#include "Meta_Specification_Content_Page.h"
#include "Meta_Specification_Copy_Child_Links.h"
#include "Meta_Class.h"
#include "Meta_Model.h"
#include "Meta_Relationship.h"

#include "ciyam_base.h"
#include "class_domains.h"
#include "module_strings.h"
#include "class_utilities.h"
#include "command_handler.h"
#include "module_interface.h"

// [(start meta_relationship_field)]
#include "Meta_Field.h"
#include "Meta_Workgroup.h"
// [(finish meta_relationship_field)]

// [<start includes>]
// [<finish includes>]

using namespace std;

// [<start namespaces>]
// [<finish namespaces>]

template< > inline string to_string( const Meta_Class& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Model& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Relationship& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Class& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Model& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Relationship& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline int system( const string& cmd ) { return exec_system( cmd ); }

namespace
{

#include "Meta_Relationship.cmh"

const int32_t c_version = 1;

const char* const c_okay = "okay";

const char* const c_field_id_Cascade_Op = "113103";
const char* const c_field_id_Child_Class = "301310";
const char* const c_field_id_Child_Class_File_Field_Name = "113109";
const char* const c_field_id_Child_Class_Name = "113108";
const char* const c_field_id_Child_Name = "113105";
const char* const c_field_id_Extra = "113104";
const char* const c_field_id_Field_Id = "113106";
const char* const c_field_id_Field_Key = "113107";
const char* const c_field_id_Internal = "113110";
const char* const c_field_id_Mandatory = "113102";
const char* const c_field_id_Model = "301300";
const char* const c_field_id_Name = "113101";
const char* const c_field_id_Parent_Class = "301320";
const char* const c_field_id_Source_Relationship = "301330";
const char* const c_field_id_Transient = "113111";

const char* const c_field_name_Cascade_Op = "Cascade_Op";
const char* const c_field_name_Child_Class = "Child_Class";
const char* const c_field_name_Child_Class_File_Field_Name = "Child_Class_File_Field_Name";
const char* const c_field_name_Child_Class_Name = "Child_Class_Name";
const char* const c_field_name_Child_Name = "Child_Name";
const char* const c_field_name_Extra = "Extra";
const char* const c_field_name_Field_Id = "Field_Id";
const char* const c_field_name_Field_Key = "Field_Key";
const char* const c_field_name_Internal = "Internal";
const char* const c_field_name_Mandatory = "Mandatory";
const char* const c_field_name_Model = "Model";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Parent_Class = "Parent_Class";
const char* const c_field_name_Source_Relationship = "Source_Relationship";
const char* const c_field_name_Transient = "Transient";

const char* const c_field_display_name_Cascade_Op = "field_relationship_cascade_op";
const char* const c_field_display_name_Child_Class = "field_relationship_child_class";
const char* const c_field_display_name_Child_Class_File_Field_Name = "field_relationship_child_class_file_field_name";
const char* const c_field_display_name_Child_Class_Name = "field_relationship_child_class_name";
const char* const c_field_display_name_Child_Name = "field_relationship_child_name";
const char* const c_field_display_name_Extra = "field_relationship_extra";
const char* const c_field_display_name_Field_Id = "field_relationship_field_id";
const char* const c_field_display_name_Field_Key = "field_relationship_field_key";
const char* const c_field_display_name_Internal = "field_relationship_internal";
const char* const c_field_display_name_Mandatory = "field_relationship_mandatory";
const char* const c_field_display_name_Model = "field_relationship_model";
const char* const c_field_display_name_Name = "field_relationship_name";
const char* const c_field_display_name_Parent_Class = "field_relationship_parent_class";
const char* const c_field_display_name_Source_Relationship = "field_relationship_source_relationship";
const char* const c_field_display_name_Transient = "field_relationship_transient";

const int c_num_fields = 15;

const char* const c_all_sorted_field_ids[ ] =
{
   "113101",
   "113102",
   "113103",
   "113104",
   "113105",
   "113106",
   "113107",
   "113108",
   "113109",
   "113110",
   "113111",
   "301300",
   "301310",
   "301320",
   "301330"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Cascade_Op",
   "Child_Class",
   "Child_Class_File_Field_Name",
   "Child_Class_Name",
   "Child_Name",
   "Extra",
   "Field_Id",
   "Field_Key",
   "Internal",
   "Mandatory",
   "Model",
   "Name",
   "Parent_Class",
   "Source_Relationship",
   "Transient"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool has_field( const string& field )
{
   return binary_search( c_all_sorted_field_ids, c_all_sorted_field_ids + c_num_fields, field.c_str( ), compare )
    || binary_search( c_all_sorted_field_names, c_all_sorted_field_names + c_num_fields, field.c_str( ), compare );
}

const int c_num_transient_fields = 3;

const char* const c_transient_sorted_field_ids[ ] =
{
   "113105",
   "113108",
   "113109"
};

const char* const c_transient_sorted_field_names[ ] =
{
   "Child_Class_File_Field_Name",
   "Child_Class_Name",
   "Child_Name"
};

inline bool transient_compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool is_transient_field( const string& field )
{
   return binary_search( c_transient_sorted_field_ids,
    c_transient_sorted_field_ids + c_num_transient_fields, field.c_str( ), transient_compare )
    || binary_search( c_transient_sorted_field_names,
    c_transient_sorted_field_names + c_num_transient_fields, field.c_str( ), transient_compare );
}

const uint64_t c_modifier_Is_Internal = UINT64_C( 0x100 );
const uint64_t c_modifier_Is_Transient = UINT64_C( 0x200 );

aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Child_Class_File_Field_Name_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Child_Class_Name_domain;
domain_string_max_size< 100 > g_Child_Name_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Field_Id_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Name_domain;

set< string > g_derivations;

typedef map< string, Meta_Relationship* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Relationship* > external_aliases_lookup_container;
typedef external_aliases_lookup_container::const_iterator external_aliases_lookup_const_iterator;

external_aliases_container g_external_aliases;
external_aliases_lookup_container g_external_aliases_lookup;

int gv_default_Cascade_Op = int( 0 );
string gv_default_Child_Class = string( );
string gv_default_Child_Class_File_Field_Name = string( );
string gv_default_Child_Class_Name = string( );
string gv_default_Child_Name = string( );
int gv_default_Extra = int( 0 );
string gv_default_Field_Id = string( );
string gv_default_Field_Key = string( );
bool gv_default_Internal = bool( 0 );
bool gv_default_Mandatory = bool( 1 );
string gv_default_Model = string( );
string gv_default_Name = string( );
string gv_default_Parent_Class = string( );
string gv_default_Source_Relationship = string( );
bool gv_default_Transient = bool( 0 );

set< int > g_cascade_op_enum;
set< int > g_relationship_extra_enum;

const int c_enum_cascade_op_restrict( 0 );
const int c_enum_cascade_op_unlink( 1 );
const int c_enum_cascade_op_destroy( 2 );
const int c_enum_cascade_op_not_applicable( -1 );

string get_enum_string_cascade_op( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for cascade_op" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_cascade_op_restrict";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_cascade_op_unlink";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_cascade_op_destroy";
   else if( to_string( val ) == to_string( "-1" ) )
      string_name = "enum_cascade_op_not_applicable";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for cascade_op" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_relationship_extra_none( 0 );
const int c_enum_relationship_extra_user_group( 2 );
const int c_enum_relationship_extra_user_other( 3 );
const int c_enum_relationship_extra_owning_user( 1 );
const int c_enum_relationship_extra_create_user_key( 4 );
const int c_enum_relationship_extra_modify_user_key( 5 );

string get_enum_string_relationship_extra( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for relationship_extra" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_relationship_extra_none";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_relationship_extra_user_group";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_relationship_extra_user_other";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_relationship_extra_owning_user";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_relationship_extra_create_user_key";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_relationship_extra_modify_user_key";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for relationship_extra" );

   return get_module_string( lower( string_name ) );
}

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Relationship > Relationship_registration( get_class_registry( ), "113100" );

class Meta_Relationship_command_functor;

class Meta_Relationship_command_handler : public command_handler
{
   friend class Meta_Relationship_command_functor;

   public:
   Meta_Relationship_command_handler( ) : p_Meta_Relationship( 0 ) { }

   void set_Meta_Relationship( Meta_Relationship* p_new_Meta_Relationship ) { p_Meta_Relationship = p_new_Meta_Relationship; }

   void handle_unknown_command( const string& command )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Relationship* p_Meta_Relationship;

   protected:
   string retval;
};

class Meta_Relationship_command_functor : public command_functor
{
   public:
   Meta_Relationship_command_functor( Meta_Relationship_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Relationship_command_handler& cmd_handler;
};

command_functor* Meta_Relationship_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Relationship_command_functor( dynamic_cast< Meta_Relationship_command_handler& >( handler ) );
}

void Meta_Relationship_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Relationship_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_parm_Meta_Relationship_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Relationship->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Relationship->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Relationship_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Relationship->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Relationship->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Relationship_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Relationship_get_field_name ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );
      else if( field_name == c_field_id_Cascade_Op || field_name == c_field_name_Cascade_Op )
         string_getter< int >( cmd_handler.p_Meta_Relationship->Cascade_Op( ), cmd_handler.retval );
      else if( field_name == c_field_id_Child_Class || field_name == c_field_name_Child_Class )
         string_getter< Meta_Class >( cmd_handler.p_Meta_Relationship->Child_Class( ), cmd_handler.retval );
      else if( field_name == c_field_id_Child_Class_File_Field_Name || field_name == c_field_name_Child_Class_File_Field_Name )
         string_getter< string >( cmd_handler.p_Meta_Relationship->Child_Class_File_Field_Name( ), cmd_handler.retval );
      else if( field_name == c_field_id_Child_Class_Name || field_name == c_field_name_Child_Class_Name )
         string_getter< string >( cmd_handler.p_Meta_Relationship->Child_Class_Name( ), cmd_handler.retval );
      else if( field_name == c_field_id_Child_Name || field_name == c_field_name_Child_Name )
         string_getter< string >( cmd_handler.p_Meta_Relationship->Child_Name( ), cmd_handler.retval );
      else if( field_name == c_field_id_Extra || field_name == c_field_name_Extra )
         string_getter< int >( cmd_handler.p_Meta_Relationship->Extra( ), cmd_handler.retval );
      else if( field_name == c_field_id_Field_Id || field_name == c_field_name_Field_Id )
         string_getter< string >( cmd_handler.p_Meta_Relationship->Field_Id( ), cmd_handler.retval );
      else if( field_name == c_field_id_Field_Key || field_name == c_field_name_Field_Key )
         string_getter< string >( cmd_handler.p_Meta_Relationship->Field_Key( ), cmd_handler.retval );
      else if( field_name == c_field_id_Internal || field_name == c_field_name_Internal )
         string_getter< bool >( cmd_handler.p_Meta_Relationship->Internal( ), cmd_handler.retval );
      else if( field_name == c_field_id_Mandatory || field_name == c_field_name_Mandatory )
         string_getter< bool >( cmd_handler.p_Meta_Relationship->Mandatory( ), cmd_handler.retval );
      else if( field_name == c_field_id_Model || field_name == c_field_name_Model )
         string_getter< Meta_Model >( cmd_handler.p_Meta_Relationship->Model( ), cmd_handler.retval );
      else if( field_name == c_field_id_Name || field_name == c_field_name_Name )
         string_getter< string >( cmd_handler.p_Meta_Relationship->Name( ), cmd_handler.retval );
      else if( field_name == c_field_id_Parent_Class || field_name == c_field_name_Parent_Class )
         string_getter< Meta_Class >( cmd_handler.p_Meta_Relationship->Parent_Class( ), cmd_handler.retval );
      else if( field_name == c_field_id_Source_Relationship || field_name == c_field_name_Source_Relationship )
         string_getter< Meta_Relationship >( cmd_handler.p_Meta_Relationship->Source_Relationship( ), cmd_handler.retval );
      else if( field_name == c_field_id_Transient || field_name == c_field_name_Transient )
         string_getter< bool >( cmd_handler.p_Meta_Relationship->Transient( ), cmd_handler.retval );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Relationship_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Relationship_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_parm_Meta_Relationship_set_field_value ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );
      else if( field_name == c_field_id_Cascade_Op || field_name == c_field_name_Cascade_Op )
         func_string_setter< Meta_Relationship, int >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Cascade_Op, field_value );
      else if( field_name == c_field_id_Child_Class || field_name == c_field_name_Child_Class )
         func_string_setter< Meta_Relationship, Meta_Class >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Child_Class, field_value );
      else if( field_name == c_field_id_Child_Class_File_Field_Name || field_name == c_field_name_Child_Class_File_Field_Name )
         func_string_setter< Meta_Relationship, string >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Child_Class_File_Field_Name, field_value );
      else if( field_name == c_field_id_Child_Class_Name || field_name == c_field_name_Child_Class_Name )
         func_string_setter< Meta_Relationship, string >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Child_Class_Name, field_value );
      else if( field_name == c_field_id_Child_Name || field_name == c_field_name_Child_Name )
         func_string_setter< Meta_Relationship, string >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Child_Name, field_value );
      else if( field_name == c_field_id_Extra || field_name == c_field_name_Extra )
         func_string_setter< Meta_Relationship, int >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Extra, field_value );
      else if( field_name == c_field_id_Field_Id || field_name == c_field_name_Field_Id )
         func_string_setter< Meta_Relationship, string >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Field_Id, field_value );
      else if( field_name == c_field_id_Field_Key || field_name == c_field_name_Field_Key )
         func_string_setter< Meta_Relationship, string >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Field_Key, field_value );
      else if( field_name == c_field_id_Internal || field_name == c_field_name_Internal )
         func_string_setter< Meta_Relationship, bool >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Internal, field_value );
      else if( field_name == c_field_id_Mandatory || field_name == c_field_name_Mandatory )
         func_string_setter< Meta_Relationship, bool >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Mandatory, field_value );
      else if( field_name == c_field_id_Model || field_name == c_field_name_Model )
         func_string_setter< Meta_Relationship, Meta_Model >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Model, field_value );
      else if( field_name == c_field_id_Name || field_name == c_field_name_Name )
         func_string_setter< Meta_Relationship, string >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Name, field_value );
      else if( field_name == c_field_id_Parent_Class || field_name == c_field_name_Parent_Class )
         func_string_setter< Meta_Relationship, Meta_Class >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Parent_Class, field_value );
      else if( field_name == c_field_id_Source_Relationship || field_name == c_field_name_Source_Relationship )
         func_string_setter< Meta_Relationship, Meta_Relationship >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Source_Relationship, field_value );
      else if( field_name == c_field_id_Transient || field_name == c_field_name_Transient )
         func_string_setter< Meta_Relationship, bool >(
          *cmd_handler.p_Meta_Relationship, &Meta_Relationship::Transient, field_value );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Relationship_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Relationship_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_parm_Meta_Relationship_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( field_name == c_field_id_Child_Class || field_name == c_field_name_Child_Class )
         cmd_handler.retval = cmd_handler.p_Meta_Relationship->Child_Class( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Model || field_name == c_field_name_Model )
         cmd_handler.retval = cmd_handler.p_Meta_Relationship->Model( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Parent_Class || field_name == c_field_name_Parent_Class )
         cmd_handler.retval = cmd_handler.p_Meta_Relationship->Parent_Class( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Source_Relationship || field_name == c_field_name_Source_Relationship )
         cmd_handler.retval = cmd_handler.p_Meta_Relationship->Source_Relationship( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
}

struct Meta_Relationship::impl : public Meta_Relationship_command_handler
{
   impl( Meta_Relationship& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Relationship( &o );

      add_commands( 0, Meta_Relationship_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Relationship_command_definitions ) );
   }

   Meta_Relationship& get_obj( ) const
   {
      return *cp_obj;
   }

   int impl_Cascade_Op( ) const { return lazy_fetch( p_obj ), v_Cascade_Op; }
   void impl_Cascade_Op( int Cascade_Op ) { v_Cascade_Op = Cascade_Op; }

   const string& impl_Child_Class_File_Field_Name( ) const { return lazy_fetch( p_obj ), v_Child_Class_File_Field_Name; }
   void impl_Child_Class_File_Field_Name( const string& Child_Class_File_Field_Name ) { v_Child_Class_File_Field_Name = Child_Class_File_Field_Name; }

   const string& impl_Child_Class_Name( ) const { return lazy_fetch( p_obj ), v_Child_Class_Name; }
   void impl_Child_Class_Name( const string& Child_Class_Name ) { v_Child_Class_Name = Child_Class_Name; }

   const string& impl_Child_Name( ) const { return lazy_fetch( p_obj ), v_Child_Name; }
   void impl_Child_Name( const string& Child_Name ) { v_Child_Name = Child_Name; }

   int impl_Extra( ) const { return lazy_fetch( p_obj ), v_Extra; }
   void impl_Extra( int Extra ) { v_Extra = Extra; }

   const string& impl_Field_Id( ) const { return lazy_fetch( p_obj ), v_Field_Id; }
   void impl_Field_Id( const string& Field_Id ) { v_Field_Id = Field_Id; }

   const string& impl_Field_Key( ) const { return lazy_fetch( p_obj ), v_Field_Key; }
   void impl_Field_Key( const string& Field_Key ) { v_Field_Key = Field_Key; }

   bool impl_Internal( ) const { return lazy_fetch( p_obj ), v_Internal; }
   void impl_Internal( bool Internal ) { v_Internal = Internal; }

   bool impl_Mandatory( ) const { return lazy_fetch( p_obj ), v_Mandatory; }
   void impl_Mandatory( bool Mandatory ) { v_Mandatory = Mandatory; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { v_Name = Name; }

   bool impl_Transient( ) const { return lazy_fetch( p_obj ), v_Transient; }
   void impl_Transient( bool Transient ) { v_Transient = Transient; }

   Meta_Class& impl_Child_Class( )
   {
      if( !cp_Child_Class )
      {
         cp_Child_Class.init( );

         p_obj->setup_graph_parent( *cp_Child_Class, c_field_id_Child_Class, v_Child_Class );
      }
      return *cp_Child_Class;
   }

   const Meta_Class& impl_Child_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Child_Class )
      {
         cp_Child_Class.init( );

         p_obj->setup_graph_parent( *cp_Child_Class, c_field_id_Child_Class, v_Child_Class );
      }
      return *cp_Child_Class;
   }

   void impl_Child_Class( const string& key )
   {
      class_base_accessor cba( impl_Child_Class( ) );
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

   Meta_Class& impl_Parent_Class( )
   {
      if( !cp_Parent_Class )
      {
         cp_Parent_Class.init( );

         p_obj->setup_graph_parent( *cp_Parent_Class, c_field_id_Parent_Class, v_Parent_Class );
      }
      return *cp_Parent_Class;
   }

   const Meta_Class& impl_Parent_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Parent_Class )
      {
         cp_Parent_Class.init( );

         p_obj->setup_graph_parent( *cp_Parent_Class, c_field_id_Parent_Class, v_Parent_Class );
      }
      return *cp_Parent_Class;
   }

   void impl_Parent_Class( const string& key )
   {
      class_base_accessor cba( impl_Parent_Class( ) );
      cba.set_key( key );
   }

   Meta_Relationship& impl_Source_Relationship( )
   {
      if( !cp_Source_Relationship )
      {
         cp_Source_Relationship.init( );

         p_obj->setup_graph_parent( *cp_Source_Relationship, c_field_id_Source_Relationship, v_Source_Relationship );
      }
      return *cp_Source_Relationship;
   }

   const Meta_Relationship& impl_Source_Relationship( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Relationship )
      {
         cp_Source_Relationship.init( );

         p_obj->setup_graph_parent( *cp_Source_Relationship, c_field_id_Source_Relationship, v_Source_Relationship );
      }
      return *cp_Source_Relationship;
   }

   void impl_Source_Relationship( const string& key )
   {
      class_base_accessor cba( impl_Source_Relationship( ) );
      cba.set_key( key );
   }

   Meta_List_Field& impl_child_List_Field_Child( )
   {
      if( !cp_child_List_Field_Child )
      {
         cp_child_List_Field_Child.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Child, "302193" );
      }
      return *cp_child_List_Field_Child;
   }

   const Meta_List_Field& impl_child_List_Field_Child( ) const
   {
      if( !cp_child_List_Field_Child )
      {
         cp_child_List_Field_Child.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Child, "302193" );
      }
      return *cp_child_List_Field_Child;
   }

   Meta_Specification& impl_child_Specification_Child( )
   {
      if( !cp_child_Specification_Child )
      {
         cp_child_Specification_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Child, "301475" );
      }
      return *cp_child_Specification_Child;
   }

   const Meta_Specification& impl_child_Specification_Child( ) const
   {
      if( !cp_child_Specification_Child )
      {
         cp_child_Specification_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Child, "301475" );
      }
      return *cp_child_Specification_Child;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Child_Self( )
   {
      if( !cp_child_Specification_Content_Page_Child_Self )
      {
         cp_child_Specification_Content_Page_Child_Self.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Child_Self, "302620" );
      }
      return *cp_child_Specification_Content_Page_Child_Self;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Child_Self( ) const
   {
      if( !cp_child_Specification_Content_Page_Child_Self )
      {
         cp_child_Specification_Content_Page_Child_Self.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Child_Self, "302620" );
      }
      return *cp_child_Specification_Content_Page_Child_Self;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Link_Page_Link_Child( )
   {
      if( !cp_child_Specification_Content_Page_Link_Page_Link_Child )
      {
         cp_child_Specification_Content_Page_Link_Page_Link_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Link_Page_Link_Child, "302632" );
      }
      return *cp_child_Specification_Content_Page_Link_Page_Link_Child;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Link_Page_Link_Child( ) const
   {
      if( !cp_child_Specification_Content_Page_Link_Page_Link_Child )
      {
         cp_child_Specification_Content_Page_Link_Page_Link_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Link_Page_Link_Child, "302632" );
      }
      return *cp_child_Specification_Content_Page_Link_Page_Link_Child;
   }

   Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Page_Page_Link_Child( )
   {
      if( !cp_child_Specification_Content_Page_Page_Page_Link_Child )
      {
         cp_child_Specification_Content_Page_Page_Page_Link_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Page_Page_Link_Child, "302627" );
      }
      return *cp_child_Specification_Content_Page_Page_Page_Link_Child;
   }

   const Meta_Specification_Content_Page& impl_child_Specification_Content_Page_Page_Page_Link_Child( ) const
   {
      if( !cp_child_Specification_Content_Page_Page_Page_Link_Child )
      {
         cp_child_Specification_Content_Page_Page_Page_Link_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Content_Page_Page_Page_Link_Child, "302627" );
      }
      return *cp_child_Specification_Content_Page_Page_Page_Link_Child;
   }

   Meta_Specification_Copy_Child_Links& impl_child_Specification_Copy_Child_Links_Source_Child( )
   {
      if( !cp_child_Specification_Copy_Child_Links_Source_Child )
      {
         cp_child_Specification_Copy_Child_Links_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Copy_Child_Links_Source_Child, "302651" );
      }
      return *cp_child_Specification_Copy_Child_Links_Source_Child;
   }

   const Meta_Specification_Copy_Child_Links& impl_child_Specification_Copy_Child_Links_Source_Child( ) const
   {
      if( !cp_child_Specification_Copy_Child_Links_Source_Child )
      {
         cp_child_Specification_Copy_Child_Links_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Copy_Child_Links_Source_Child, "302651" );
      }
      return *cp_child_Specification_Copy_Child_Links_Source_Child;
   }

   Meta_Relationship& impl_child_Relationship_Source( )
   {
      if( !cp_child_Relationship_Source )
      {
         cp_child_Relationship_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Relationship_Source, "301330" );
      }
      return *cp_child_Relationship_Source;
   }

   const Meta_Relationship& impl_child_Relationship_Source( ) const
   {
      if( !cp_child_Relationship_Source )
      {
         cp_child_Relationship_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Relationship_Source, "301330" );
      }
      return *cp_child_Relationship_Source;
   }

   string get_field_value( int field ) const;
   void set_field_value( int field, const string& value );

   uint64_t get_state( ) const;

   const string& execute( const string& cmd_and_args );

   void clear_foreign_key( const string& field );

   void set_foreign_key_value( const string& field, const string& value );

   const string& get_foreign_key_value( const string& field );

   void get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const;

   void add_extra_paging_info( vector< pair< string, string > >& paging_info ) const;

   void clear( );

   bool value_will_be_provided( const string& field_name );

   void validate( unsigned state, bool is_internal, validation_error_container* p_validation_errors );
   void validate_set_fields( set< string >& fields_set, validation_error_container* p_validation_errors );

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

   Meta_Relationship* p_obj;
   class_pointer< Meta_Relationship > cp_obj;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   int v_Cascade_Op;
   string v_Child_Class_File_Field_Name;
   string v_Child_Class_Name;
   string v_Child_Name;
   int v_Extra;
   string v_Field_Id;
   string v_Field_Key;
   bool v_Internal;
   bool v_Mandatory;
   string v_Name;
   bool v_Transient;

   string v_Child_Class;
   mutable class_pointer< Meta_Class > cp_Child_Class;

   string v_Model;
   mutable class_pointer< Meta_Model > cp_Model;

   string v_Parent_Class;
   mutable class_pointer< Meta_Class > cp_Parent_Class;

   string v_Source_Relationship;
   mutable class_pointer< Meta_Relationship > cp_Source_Relationship;

   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Child;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Child;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Child_Self;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Link_Page_Link_Child;
   mutable class_pointer< Meta_Specification_Content_Page > cp_child_Specification_Content_Page_Page_Page_Link_Child;
   mutable class_pointer< Meta_Specification_Copy_Child_Links > cp_child_Specification_Copy_Child_Links_Source_Child;
   mutable class_pointer< Meta_Relationship > cp_child_Relationship_Source;
};

string Meta_Relationship::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Cascade_Op( ) );
      break;

      case 1:
      retval = to_string( impl_Child_Class( ) );
      break;

      case 2:
      retval = to_string( impl_Child_Class_File_Field_Name( ) );
      break;

      case 3:
      retval = to_string( impl_Child_Class_Name( ) );
      break;

      case 4:
      retval = to_string( impl_Child_Name( ) );
      break;

      case 5:
      retval = to_string( impl_Extra( ) );
      break;

      case 6:
      retval = to_string( impl_Field_Id( ) );
      break;

      case 7:
      retval = to_string( impl_Field_Key( ) );
      break;

      case 8:
      retval = to_string( impl_Internal( ) );
      break;

      case 9:
      retval = to_string( impl_Mandatory( ) );
      break;

      case 10:
      retval = to_string( impl_Model( ) );
      break;

      case 11:
      retval = to_string( impl_Name( ) );
      break;

      case 12:
      retval = to_string( impl_Parent_Class( ) );
      break;

      case 13:
      retval = to_string( impl_Source_Relationship( ) );
      break;

      case 14:
      retval = to_string( impl_Transient( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }

   return retval;
}

void Meta_Relationship::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Relationship::impl, int >( *this, &Meta_Relationship::impl::impl_Cascade_Op, value );
      break;

      case 1:
      func_string_setter< Meta_Relationship::impl, Meta_Class >( *this, &Meta_Relationship::impl::impl_Child_Class, value );
      break;

      case 2:
      func_string_setter< Meta_Relationship::impl, string >( *this, &Meta_Relationship::impl::impl_Child_Class_File_Field_Name, value );
      break;

      case 3:
      func_string_setter< Meta_Relationship::impl, string >( *this, &Meta_Relationship::impl::impl_Child_Class_Name, value );
      break;

      case 4:
      func_string_setter< Meta_Relationship::impl, string >( *this, &Meta_Relationship::impl::impl_Child_Name, value );
      break;

      case 5:
      func_string_setter< Meta_Relationship::impl, int >( *this, &Meta_Relationship::impl::impl_Extra, value );
      break;

      case 6:
      func_string_setter< Meta_Relationship::impl, string >( *this, &Meta_Relationship::impl::impl_Field_Id, value );
      break;

      case 7:
      func_string_setter< Meta_Relationship::impl, string >( *this, &Meta_Relationship::impl::impl_Field_Key, value );
      break;

      case 8:
      func_string_setter< Meta_Relationship::impl, bool >( *this, &Meta_Relationship::impl::impl_Internal, value );
      break;

      case 9:
      func_string_setter< Meta_Relationship::impl, bool >( *this, &Meta_Relationship::impl::impl_Mandatory, value );
      break;

      case 10:
      func_string_setter< Meta_Relationship::impl, Meta_Model >( *this, &Meta_Relationship::impl::impl_Model, value );
      break;

      case 11:
      func_string_setter< Meta_Relationship::impl, string >( *this, &Meta_Relationship::impl::impl_Name, value );
      break;

      case 12:
      func_string_setter< Meta_Relationship::impl, Meta_Class >( *this, &Meta_Relationship::impl::impl_Parent_Class, value );
      break;

      case 13:
      func_string_setter< Meta_Relationship::impl, Meta_Relationship >( *this, &Meta_Relationship::impl::impl_Source_Relationship, value );
      break;

      case 14:
      func_string_setter< Meta_Relationship::impl, bool >( *this, &Meta_Relationship::impl::impl_Transient, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }
}

uint64_t Meta_Relationship::impl::get_state( ) const
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

   // [(start modifier_field_value)]
   if( get_obj( ).Transient( ) == true )
      state |= c_modifier_Is_Transient;
   // [(finish modifier_field_value)]

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

const string& Meta_Relationship::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Relationship::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Child_Class || field == c_field_name_Child_Class )
      impl_Child_Class( "" );
   else if( field == c_field_id_Model || field == c_field_name_Model )
      impl_Model( "" );
   else if( field == c_field_id_Parent_Class || field == c_field_name_Parent_Class )
      impl_Parent_Class( "" );
   else if( field == c_field_id_Source_Relationship || field == c_field_name_Source_Relationship )
      impl_Source_Relationship( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Relationship::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( field == c_field_id_Child_Class || field == c_field_name_Child_Class )
      v_Child_Class = value;
   else if( field == c_field_id_Model || field == c_field_name_Model )
      v_Model = value;
   else if( field == c_field_id_Parent_Class || field == c_field_name_Parent_Class )
      v_Parent_Class = value;
   else if( field == c_field_id_Source_Relationship || field == c_field_name_Source_Relationship )
      v_Source_Relationship = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Relationship::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Child_Class || field == c_field_name_Child_Class )
      return v_Child_Class;
   else if( field == c_field_id_Model || field == c_field_name_Model )
      return v_Model;
   else if( field == c_field_id_Parent_Class || field == c_field_name_Parent_Class )
      return v_Parent_Class;
   else if( field == c_field_id_Source_Relationship || field == c_field_name_Source_Relationship )
      return v_Source_Relationship;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Relationship::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Child_Class, v_Child_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Model, v_Model ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Parent_Class, v_Parent_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Relationship, v_Source_Relationship ) );
}

void Meta_Relationship::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Relationship::impl::clear( )
{
   v_Cascade_Op = gv_default_Cascade_Op;
   v_Child_Class_File_Field_Name = gv_default_Child_Class_File_Field_Name;
   v_Child_Class_Name = gv_default_Child_Class_Name;
   v_Child_Name = gv_default_Child_Name;
   v_Extra = gv_default_Extra;
   v_Field_Id = gv_default_Field_Id;
   v_Field_Key = gv_default_Field_Key;
   v_Internal = gv_default_Internal;
   v_Mandatory = gv_default_Mandatory;
   v_Name = gv_default_Name;
   v_Transient = gv_default_Transient;

   v_Child_Class = string( );
   if( cp_Child_Class )
      p_obj->setup_foreign_key( *cp_Child_Class, v_Child_Class );

   v_Model = string( );
   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   v_Parent_Class = string( );
   if( cp_Parent_Class )
      p_obj->setup_foreign_key( *cp_Parent_Class, v_Parent_Class );

   v_Source_Relationship = string( );
   if( cp_Source_Relationship )
      p_obj->setup_foreign_key( *cp_Source_Relationship, v_Source_Relationship );
}

bool Meta_Relationship::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Relationship::impl::validate( unsigned state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   if( v_Child_Class.empty( ) && !value_will_be_provided( c_field_name_Child_Class ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Child_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Child_Class ) ) ) ) );

   if( v_Model.empty( ) && !value_will_be_provided( c_field_name_Model ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Model,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Model ) ) ) ) );

   if( v_Parent_Class.empty( ) && !value_will_be_provided( c_field_name_Parent_Class ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Parent_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Parent_Class ) ) ) ) );

   string error_message;
   if( !is_null( v_Child_Class_File_Field_Name )
    && ( v_Child_Class_File_Field_Name != gv_default_Child_Class_File_Field_Name
    || !value_will_be_provided( c_field_name_Child_Class_File_Field_Name ) )
    && !g_Child_Class_File_Field_Name_domain.is_valid( v_Child_Class_File_Field_Name, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Child_Class_File_Field_Name,
       get_module_string( c_field_display_name_Child_Class_File_Field_Name ) + " " + error_message ) );

   if( !is_null( v_Child_Class_Name )
    && ( v_Child_Class_Name != gv_default_Child_Class_Name
    || !value_will_be_provided( c_field_name_Child_Class_Name ) )
    && !g_Child_Class_Name_domain.is_valid( v_Child_Class_Name, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Child_Class_Name,
       get_module_string( c_field_display_name_Child_Class_Name ) + " " + error_message ) );

   if( !is_null( v_Child_Name )
    && ( v_Child_Name != gv_default_Child_Name
    || !value_will_be_provided( c_field_name_Child_Name ) )
    && !g_Child_Name_domain.is_valid( v_Child_Name, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Child_Name,
       get_module_string( c_field_display_name_Child_Name ) + " " + error_message ) );

   if( !is_null( v_Field_Id )
    && ( v_Field_Id != gv_default_Field_Id
    || !value_will_be_provided( c_field_name_Field_Id ) )
    && !g_Field_Id_domain.is_valid( v_Field_Id, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Field_Id,
       get_module_string( c_field_display_name_Field_Id ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( v_Name != gv_default_Name
    || !value_will_be_provided( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   if( !g_cascade_op_enum.count( v_Cascade_Op ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Cascade_Op,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Cascade_Op ) ) ) ) );

   if( !g_relationship_extra_enum.count( v_Extra ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Extra,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Extra ) ) ) ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Relationship::impl::validate_set_fields( set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;

   if( !is_null( v_Child_Class_File_Field_Name )
    && ( fields_set.count( c_field_id_Child_Class_File_Field_Name ) || fields_set.count( c_field_name_Child_Class_File_Field_Name ) )
    && !g_Child_Class_File_Field_Name_domain.is_valid( v_Child_Class_File_Field_Name, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Child_Class_File_Field_Name,
       get_module_string( c_field_display_name_Child_Class_File_Field_Name ) + " " + error_message ) );

   if( !is_null( v_Child_Class_Name )
    && ( fields_set.count( c_field_id_Child_Class_Name ) || fields_set.count( c_field_name_Child_Class_Name ) )
    && !g_Child_Class_Name_domain.is_valid( v_Child_Class_Name, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Child_Class_Name,
       get_module_string( c_field_display_name_Child_Class_Name ) + " " + error_message ) );

   if( !is_null( v_Child_Name )
    && ( fields_set.count( c_field_id_Child_Name ) || fields_set.count( c_field_name_Child_Name ) )
    && !g_Child_Name_domain.is_valid( v_Child_Name, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Child_Name,
       get_module_string( c_field_display_name_Child_Name ) + " " + error_message ) );

   if( !is_null( v_Field_Id )
    && ( fields_set.count( c_field_id_Field_Id ) || fields_set.count( c_field_name_Field_Id ) )
    && !g_Field_Id_domain.is_valid( v_Field_Id, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Field_Id,
       get_module_string( c_field_display_name_Field_Id ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( fields_set.count( c_field_id_Name ) || fields_set.count( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );
}

void Meta_Relationship::impl::after_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   if( cp_Child_Class )
      p_obj->setup_foreign_key( *cp_Child_Class, v_Child_Class );

   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   if( cp_Parent_Class )
      p_obj->setup_foreign_key( *cp_Parent_Class, v_Parent_Class );

   if( cp_Source_Relationship )
      p_obj->setup_foreign_key( *cp_Source_Relationship, v_Source_Relationship );

   // [(start meta_relationship_child_name)]
   if( get_obj( ).needs_field_value( "Child_Name" )
    || required_transients.count( "Child_Name" ) )
   {
      string str( get_obj( ).Name( ) );
      meta_relationship_child_name( str, get_obj( ).Child_Class( ).Name( ), get_obj( ).Parent_Class( ).Name( ), "_" );

      get_obj( ).Child_Name( str );
   }
   // [(start meta_relationship_child_name)]

   // [(start field_from_other_field)]
   get_obj( ).Child_Class_Name( get_obj( ).Child_Class( ).Name( ) );
   // [(finish field_from_other_field)]

   // [<start after_fetch>]
//nyi
   if( get_obj( ).needs_field_value( "Child_Class_File_Field_Name" )
    || required_transients.count( "Child_Class_File_Field_Name" ) )
   {
      if( !is_null( get_obj( ).Child_Class( ) )
       && get_obj( ).Child_Class( ).child_Field( ).iterate_forwards( ) )
      {
         do
         {
            if( get_obj( ).Child_Class( ).child_Field( ).Extra( ) == 1 ) // i.e. "file"
            {
               get_obj( ).Child_Class_File_Field_Name( get_obj( ).Child_Class( ).child_Field( ).Name( ) );
               get_obj( ).Child_Class( ).child_Field( ).iterate_stop( );
               break;
            }
         } while( get_obj( ).Child_Class( ).child_Field( ).iterate_next( ) );
      }
   }
   // [<finish after_fetch>]
}

void Meta_Relationship::impl::finalise_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Relationship::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Relationship::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_from_changed_fk)]
   if( get_obj( ).get_key( ).empty( ) && get_obj( ).Parent_Class( ).has_changed( ) )
      get_obj( ).Name( get_obj( ).Parent_Class( ).Name( ) );
   // [(finish field_from_changed_fk)]

   // [(start default_to_global)]
   if( is_create && get_obj( ).get_key( ).empty( ) && get_obj( ).Parent_Class( ).Type( ) == 1 )
      get_obj( ).Extra( 1 );
   // [(finish default_to_global)]

   // [(start default_to_global)]
   if( is_create && get_obj( ).get_key( ).empty( ) && get_obj( ).Parent_Class( ).Type( ) == 2 )
      get_obj( ).Extra( 2 );
   // [(finish default_to_global)]

   // [(start default_to_global)]
   if( is_create && get_obj( ).get_key( ).empty( ) && get_obj( ).Parent_Class( ).Type( ) == 3 )
      get_obj( ).Extra( 3 );
   // [(finish default_to_global)]

   // [(start field_empty_action)]
   if( !get_obj( ).get_key( ).empty( ) )
   {
      if( !is_null( get_obj( ).Source_Relationship( ) ) )
         get_obj( ).Internal( true );
   }
   // [(finish field_empty_action)]

   // [(start modifier_set_field)]
   if( state & c_modifier_Is_Transient )
      get_obj( ).Cascade_Op( -1 );
   // [(finish modifier_set_field)]

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Relationship::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [(start meta_relationship_field)]
   if( is_null( get_obj( ).Source_Relationship( ) ) )
   {
      Meta_Field rel_field;

      if( is_create )
         rel_field.op_create( get_obj( ).get_key( ) + "_C" );
      else
         rel_field.op_update( get_obj( ).get_key( ) + "_C" );

      rel_field.Class( get_obj( ).Child_Class( ) );
      rel_field.Id( get_obj( ).Field_Id( ) );
      rel_field.Name( get_obj( ).Name( ) );
      rel_field.Mandatory( get_obj( ).Mandatory( ) );
      rel_field.Transient( get_obj( ).Transient( ) );
      rel_field.Parent_Class( get_obj( ).Parent_Class( ) );

      rel_field.Type( get_obj( ).Model( ).Workgroup( ).get_key( ) + "_foreign_key" );
      rel_field.Extra( get_obj( ).Extra( ) * -1 );
      rel_field.Internal( true );

      rel_field.op_apply( );

      get_obj( ).Field_Id( rel_field.Id( ) );
      get_obj( ).Field_Key( rel_field.get_key( ) );
   }
   // [(finish meta_relationship_field)]

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Relationship::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [(start update_children)]
   if( !is_create
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Child( ).iterate_forwards( ) )
   {
      do
      {
         get_obj( ).child_Specification_Child( ).op_update( );
         get_obj( ).child_Specification_Child( ).op_apply( );
      } while( get_obj( ).child_Specification_Child( ).iterate_next( ) );
   }
   // [(finish update_children)]

   // [(start update_children)]
   if( !is_create
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification_Copy_Child_Links_Source_Child( ).iterate_forwards( ) )
   {
      do
      {
         get_obj( ).child_Specification_Copy_Child_Links_Source_Child( ).op_update( );
         get_obj( ).child_Specification_Copy_Child_Links_Source_Child( ).op_apply( );
      } while( get_obj( ).child_Specification_Copy_Child_Links_Source_Child( ).iterate_next( ) );
   }
   // [(finish update_children)]

   // [<start after_store>]
//nyi
   if( !is_create && get_obj( ).child_Relationship_Source( ).iterate_forwards( ) )
   {
      do
      {
         get_obj( ).child_Relationship_Source( ).op_update( );
         string parent( get_obj( ).child_Relationship_Source( ).Model( ) );

         string child_class( get_obj( ).child_Relationship_Source( ).Child_Class( ) );
         string parent_class( get_obj( ).child_Relationship_Source( ).Parent_Class( ) );

         get_obj( ).child_Relationship_Source( ).copy_all_field_values( get_obj( ) );
         get_obj( ).child_Relationship_Source( ).Model( parent );
         get_obj( ).child_Relationship_Source( ).Source_Relationship( get_obj( ).get_key( ) );

         get_obj( ).child_Relationship_Source( ).Child_Class( child_class );
         get_obj( ).child_Relationship_Source( ).Parent_Class( parent_class );

         get_obj( ).child_Relationship_Source( ).op_apply( );
      } while( get_obj( ).child_Relationship_Source( ).iterate_next( ) );
   }

   if( is_create && is_null( get_obj( ).Source_Relationship( ) )
    && get_obj( ).Child_Class( ).child_Class_Source( ).iterate_forwards( ) )
   {
      int child_num = 0;
      do
      {
         map< string, string > all_classes;

         if( get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Class( ).iterate_forwards( ) )
         {
            do
            {
               all_classes.insert( make_pair(
                get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Class( ).Id( ),
                get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Class( ).get_key( ) ) );
            } while( get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Class( ).iterate_next( ) );
         }

         // NOTE: Only create a relationship for another model if it contains both child and parent aliases.
         if( all_classes.count( get_obj( ).Child_Class( ).Id( ) )
          && all_classes.count( get_obj( ).Parent_Class( ).Id( ) ) )
         {
            string key_info( construct_key_from_int( get_obj( ).get_key( ), ++child_num ) );
            key_info += ' ';
            key_info += get_obj( ).get_key( );

            get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Relationship( ).op_create( key_info );
            get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Relationship( ).Model( get_obj( ).Child_Class( ).child_Class_Source( ).Model( ) );
            get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Relationship( ).Source_Relationship( get_obj( ).get_key( ) );

            get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Relationship( ).Child_Class( all_classes[ get_obj( ).Child_Class( ).Id( ) ] );
            get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Relationship( ).Parent_Class( all_classes[ get_obj( ).Parent_Class( ).Id( ) ] );

            key_info = FIELD_ID( Meta, Field, Id );
            key_info += "#1 " + get_obj( ).Field_Id( );

            Meta_Class child_class;
            child_class.perform_fetch( all_classes[ get_obj( ).Child_Class( ).Id( ) ] );

            // NOTE: When initially cloned the Field in the Relationship's Child Class has its Parent Class linked
            // to the Class from the source model so locate the field (via the Id which is stored in the Relationship)
            // and change the Parent Class to link to the correct model here.
            if( child_class.child_Field( ).iterate_forwards( key_info, true, 1, e_sql_optimisation_unordered ) )
            {
               child_class.child_Field( ).op_update( );
               child_class.child_Field( ).Parent_Class( all_classes[ get_obj( ).Parent_Class( ).Id( ) ] );
               child_class.child_Field( ).op_apply( );

               get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Relationship( ).Field_Key( child_class.child_Field( ).get_key( ) );
            }

            get_obj( ).Child_Class( ).child_Class_Source( ).Model( ).child_Relationship( ).op_apply( );
         }
      } while( get_obj( ).Child_Class( ).child_Class_Source( ).iterate_next( ) );
   }
   // [<finish after_store>]
}

bool Meta_Relationship::impl::can_destroy( bool is_internal )
{
   bool retval = is_internal || !( get_state( ) & c_state_undeletable );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Relationship::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   // [(start meta_relationship_field)]
   if( !get_obj( ).get_is_being_cascaded( ) && is_null( get_obj( ).Source_Relationship( ) ) )
   {
      Meta_Field rel_field;

      rel_field.op_destroy( get_obj( ).get_key( ) + "_C" );
      rel_field.op_apply( );
   }
   // [(finish meta_relationship_field)]

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Relationship::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Relationship::impl::set_default_values( )
{
   clear( );
}

bool Meta_Relationship::impl::is_filtered( ) const
{
   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

#undef MODULE_TRACE
#define MODULE_TRACE( x ) trace( x )

Meta_Relationship::Meta_Relationship( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Relationship::~Meta_Relationship( )
{
   cleanup( );
   delete p_impl;
}

int Meta_Relationship::Cascade_Op( ) const
{
   return p_impl->impl_Cascade_Op( );
}

void Meta_Relationship::Cascade_Op( int Cascade_Op )
{
   p_impl->impl_Cascade_Op( Cascade_Op );
}

const string& Meta_Relationship::Child_Class_File_Field_Name( ) const
{
   return p_impl->impl_Child_Class_File_Field_Name( );
}

void Meta_Relationship::Child_Class_File_Field_Name( const string& Child_Class_File_Field_Name )
{
   p_impl->impl_Child_Class_File_Field_Name( Child_Class_File_Field_Name );
}

const string& Meta_Relationship::Child_Class_Name( ) const
{
   return p_impl->impl_Child_Class_Name( );
}

void Meta_Relationship::Child_Class_Name( const string& Child_Class_Name )
{
   p_impl->impl_Child_Class_Name( Child_Class_Name );
}

const string& Meta_Relationship::Child_Name( ) const
{
   return p_impl->impl_Child_Name( );
}

void Meta_Relationship::Child_Name( const string& Child_Name )
{
   p_impl->impl_Child_Name( Child_Name );
}

int Meta_Relationship::Extra( ) const
{
   return p_impl->impl_Extra( );
}

void Meta_Relationship::Extra( int Extra )
{
   p_impl->impl_Extra( Extra );
}

const string& Meta_Relationship::Field_Id( ) const
{
   return p_impl->impl_Field_Id( );
}

void Meta_Relationship::Field_Id( const string& Field_Id )
{
   p_impl->impl_Field_Id( Field_Id );
}

const string& Meta_Relationship::Field_Key( ) const
{
   return p_impl->impl_Field_Key( );
}

void Meta_Relationship::Field_Key( const string& Field_Key )
{
   p_impl->impl_Field_Key( Field_Key );
}

bool Meta_Relationship::Internal( ) const
{
   return p_impl->impl_Internal( );
}

void Meta_Relationship::Internal( bool Internal )
{
   p_impl->impl_Internal( Internal );
}

bool Meta_Relationship::Mandatory( ) const
{
   return p_impl->impl_Mandatory( );
}

void Meta_Relationship::Mandatory( bool Mandatory )
{
   p_impl->impl_Mandatory( Mandatory );
}

const string& Meta_Relationship::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_Relationship::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

bool Meta_Relationship::Transient( ) const
{
   return p_impl->impl_Transient( );
}

void Meta_Relationship::Transient( bool Transient )
{
   p_impl->impl_Transient( Transient );
}

Meta_Class& Meta_Relationship::Child_Class( )
{
   return p_impl->impl_Child_Class( );
}

const Meta_Class& Meta_Relationship::Child_Class( ) const
{
   return p_impl->impl_Child_Class( );
}

void Meta_Relationship::Child_Class( const string& key )
{
   p_impl->impl_Child_Class( key );
}

Meta_Model& Meta_Relationship::Model( )
{
   return p_impl->impl_Model( );
}

const Meta_Model& Meta_Relationship::Model( ) const
{
   return p_impl->impl_Model( );
}

void Meta_Relationship::Model( const string& key )
{
   p_impl->impl_Model( key );
}

Meta_Class& Meta_Relationship::Parent_Class( )
{
   return p_impl->impl_Parent_Class( );
}

const Meta_Class& Meta_Relationship::Parent_Class( ) const
{
   return p_impl->impl_Parent_Class( );
}

void Meta_Relationship::Parent_Class( const string& key )
{
   p_impl->impl_Parent_Class( key );
}

Meta_Relationship& Meta_Relationship::Source_Relationship( )
{
   return p_impl->impl_Source_Relationship( );
}

const Meta_Relationship& Meta_Relationship::Source_Relationship( ) const
{
   return p_impl->impl_Source_Relationship( );
}

void Meta_Relationship::Source_Relationship( const string& key )
{
   p_impl->impl_Source_Relationship( key );
}

Meta_List_Field& Meta_Relationship::child_List_Field_Child( )
{
   return p_impl->impl_child_List_Field_Child( );
}

const Meta_List_Field& Meta_Relationship::child_List_Field_Child( ) const
{
   return p_impl->impl_child_List_Field_Child( );
}

Meta_Specification& Meta_Relationship::child_Specification_Child( )
{
   return p_impl->impl_child_Specification_Child( );
}

const Meta_Specification& Meta_Relationship::child_Specification_Child( ) const
{
   return p_impl->impl_child_Specification_Child( );
}

Meta_Specification_Content_Page& Meta_Relationship::child_Specification_Content_Page_Child_Self( )
{
   return p_impl->impl_child_Specification_Content_Page_Child_Self( );
}

const Meta_Specification_Content_Page& Meta_Relationship::child_Specification_Content_Page_Child_Self( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Child_Self( );
}

Meta_Specification_Content_Page& Meta_Relationship::child_Specification_Content_Page_Link_Page_Link_Child( )
{
   return p_impl->impl_child_Specification_Content_Page_Link_Page_Link_Child( );
}

const Meta_Specification_Content_Page& Meta_Relationship::child_Specification_Content_Page_Link_Page_Link_Child( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Link_Page_Link_Child( );
}

Meta_Specification_Content_Page& Meta_Relationship::child_Specification_Content_Page_Page_Page_Link_Child( )
{
   return p_impl->impl_child_Specification_Content_Page_Page_Page_Link_Child( );
}

const Meta_Specification_Content_Page& Meta_Relationship::child_Specification_Content_Page_Page_Page_Link_Child( ) const
{
   return p_impl->impl_child_Specification_Content_Page_Page_Page_Link_Child( );
}

Meta_Specification_Copy_Child_Links& Meta_Relationship::child_Specification_Copy_Child_Links_Source_Child( )
{
   return p_impl->impl_child_Specification_Copy_Child_Links_Source_Child( );
}

const Meta_Specification_Copy_Child_Links& Meta_Relationship::child_Specification_Copy_Child_Links_Source_Child( ) const
{
   return p_impl->impl_child_Specification_Copy_Child_Links_Source_Child( );
}

Meta_Relationship& Meta_Relationship::child_Relationship_Source( )
{
   return p_impl->impl_child_Relationship_Source( );
}

const Meta_Relationship& Meta_Relationship::child_Relationship_Source( ) const
{
   return p_impl->impl_child_Relationship_Source( );
}

string Meta_Relationship::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Relationship::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

bool Meta_Relationship::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Relationship::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Relationship::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Relationship::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Relationship::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Relationship::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Relationship::clear( )
{
   p_impl->clear( );
}

void Meta_Relationship::validate( unsigned state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Relationship::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Relationship::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Relationship::finalise_fetch( )
{
   p_impl->finalise_fetch( );
}

void Meta_Relationship::at_create( )
{
   p_impl->at_create( );
}

void Meta_Relationship::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Relationship::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Relationship::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Relationship::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Relationship::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Relationship::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Relationship::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Relationship::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Relationship::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( 0 );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Cascade_Op )
   {
      p_id = c_field_id_Cascade_Op;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Child_Class )
   {
      p_id = c_field_id_Child_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Child_Class_File_Field_Name )
   {
      p_id = c_field_id_Child_Class_File_Field_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Child_Class_Name )
   {
      p_id = c_field_id_Child_Class_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Child_Name )
   {
      p_id = c_field_id_Child_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Extra )
   {
      p_id = c_field_id_Extra;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Field_Id )
   {
      p_id = c_field_id_Field_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Field_Key )
   {
      p_id = c_field_id_Field_Key;

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
   else if( name == c_field_name_Mandatory )
   {
      p_id = c_field_id_Mandatory;

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
   else if( name == c_field_name_Parent_Class )
   {
      p_id = c_field_id_Parent_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_Relationship )
   {
      p_id = c_field_id_Source_Relationship;

      if( p_type_name )
         *p_type_name = "Meta_Relationship";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Transient )
   {
      p_id = c_field_id_Transient;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_id;
}

const char* Meta_Relationship::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( 0 );

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Cascade_Op )
   {
      p_name = c_field_name_Cascade_Op;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Child_Class )
   {
      p_name = c_field_name_Child_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Child_Class_File_Field_Name )
   {
      p_name = c_field_name_Child_Class_File_Field_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Child_Class_Name )
   {
      p_name = c_field_name_Child_Class_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Child_Name )
   {
      p_name = c_field_name_Child_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Extra )
   {
      p_name = c_field_name_Extra;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Field_Id )
   {
      p_name = c_field_name_Field_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Field_Key )
   {
      p_name = c_field_name_Field_Key;

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
   else if( id == c_field_id_Mandatory )
   {
      p_name = c_field_name_Mandatory;

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
   else if( id == c_field_id_Parent_Class )
   {
      p_name = c_field_name_Parent_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_Relationship )
   {
      p_name = c_field_name_Source_Relationship;

      if( p_type_name )
         *p_type_name = "Meta_Relationship";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Transient )
   {
      p_name = c_field_name_Transient;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_name;
}

string Meta_Relationship::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   else if( id_or_name == c_field_id_Cascade_Op || id_or_name == c_field_name_Cascade_Op )
      display_name = get_module_string( c_field_display_name_Cascade_Op );
   else if( id_or_name == c_field_id_Child_Class || id_or_name == c_field_name_Child_Class )
      display_name = get_module_string( c_field_display_name_Child_Class );
   else if( id_or_name == c_field_id_Child_Class_File_Field_Name || id_or_name == c_field_name_Child_Class_File_Field_Name )
      display_name = get_module_string( c_field_display_name_Child_Class_File_Field_Name );
   else if( id_or_name == c_field_id_Child_Class_Name || id_or_name == c_field_name_Child_Class_Name )
      display_name = get_module_string( c_field_display_name_Child_Class_Name );
   else if( id_or_name == c_field_id_Child_Name || id_or_name == c_field_name_Child_Name )
      display_name = get_module_string( c_field_display_name_Child_Name );
   else if( id_or_name == c_field_id_Extra || id_or_name == c_field_name_Extra )
      display_name = get_module_string( c_field_display_name_Extra );
   else if( id_or_name == c_field_id_Field_Id || id_or_name == c_field_name_Field_Id )
      display_name = get_module_string( c_field_display_name_Field_Id );
   else if( id_or_name == c_field_id_Field_Key || id_or_name == c_field_name_Field_Key )
      display_name = get_module_string( c_field_display_name_Field_Key );
   else if( id_or_name == c_field_id_Internal || id_or_name == c_field_name_Internal )
      display_name = get_module_string( c_field_display_name_Internal );
   else if( id_or_name == c_field_id_Mandatory || id_or_name == c_field_name_Mandatory )
      display_name = get_module_string( c_field_display_name_Mandatory );
   else if( id_or_name == c_field_id_Model || id_or_name == c_field_name_Model )
      display_name = get_module_string( c_field_display_name_Model );
   else if( id_or_name == c_field_id_Name || id_or_name == c_field_name_Name )
      display_name = get_module_string( c_field_display_name_Name );
   else if( id_or_name == c_field_id_Parent_Class || id_or_name == c_field_name_Parent_Class )
      display_name = get_module_string( c_field_display_name_Parent_Class );
   else if( id_or_name == c_field_id_Source_Relationship || id_or_name == c_field_name_Source_Relationship )
      display_name = get_module_string( c_field_display_name_Source_Relationship );
   else if( id_or_name == c_field_id_Transient || id_or_name == c_field_name_Transient )
      display_name = get_module_string( c_field_display_name_Transient );

   return display_name;
}

void Meta_Relationship::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Relationship::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Relationship::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Relationship::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Relationship::setup_foreign_key( Meta_Class& o, const string& value )
{
   static_cast< Meta_Class& >( o ).set_key( value );
}

void Meta_Relationship::setup_foreign_key( Meta_Model& o, const string& value )
{
   static_cast< Meta_Model& >( o ).set_key( value );
}

void Meta_Relationship::setup_foreign_key( Meta_Relationship& o, const string& value )
{
   static_cast< Meta_Relationship& >( o ).set_key( value );
}

void Meta_Relationship::setup_graph_parent( Meta_List_Field& o, const string& foreign_key_field )
{
   static_cast< Meta_List_Field& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Relationship::setup_graph_parent( Meta_Specification& o, const string& foreign_key_field )
{
   static_cast< Meta_Specification& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Relationship::setup_graph_parent( Meta_Specification_Content_Page& o, const string& foreign_key_field )
{
   static_cast< Meta_Specification_Content_Page& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Relationship::setup_graph_parent( Meta_Specification_Copy_Child_Links& o, const string& foreign_key_field )
{
   static_cast< Meta_Specification_Copy_Child_Links& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Relationship::setup_graph_parent( Meta_Relationship& o, const string& foreign_key_field )
{
   static_cast< Meta_Relationship& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Relationship::setup_graph_parent(
 Meta_Class& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Class& >( o ).set_key( init_value );
}

void Meta_Relationship::setup_graph_parent(
 Meta_Model& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Model& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Model& >( o ).set_key( init_value );
}

void Meta_Relationship::setup_graph_parent(
 Meta_Relationship& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Relationship& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Relationship& >( o ).set_key( init_value );
}

size_t Meta_Relationship::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Relationship::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Relationship::get_num_foreign_key_children( bool is_internal ) const
{
   size_t rc = 7;

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

class_base* Meta_Relationship::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   if( child_num >= 7 )
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
            next_child_field = "302193";
            p_class_base = &child_List_Field_Child( );
         }
         break;

         case 1:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301475";
            p_class_base = &child_Specification_Child( );
         }
         break;

         case 2:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302620";
            p_class_base = &child_Specification_Content_Page_Child_Self( );
         }
         break;

         case 3:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302632";
            p_class_base = &child_Specification_Content_Page_Link_Page_Link_Child( );
         }
         break;

         case 4:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302627";
            p_class_base = &child_Specification_Content_Page_Page_Page_Link_Child( );
         }
         break;

         case 5:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302651";
            p_class_base = &child_Specification_Copy_Child_Links_Source_Child( );
         }
         break;

         case 6:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301330";
            p_class_base = &child_Relationship_Source( );
         }
         break;
      }
   }

   return p_class_base;
}

void Meta_Relationship::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

const char* Meta_Relationship::class_id( ) const
{
   return static_class_id( );
}

const char* Meta_Relationship::class_name( ) const
{
   return static_class_name( );
}

const char* Meta_Relationship::plural_name( ) const
{
   return static_plural_name( );
}

const char* Meta_Relationship::module_id( ) const
{
   return static_module_id( );
}

const char* Meta_Relationship::module_name( ) const
{
   return static_module_name( );
}

string Meta_Relationship::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );
   key += "relationship";

   return get_module_string( key );
}

bool Meta_Relationship::get_is_alias( ) const
{
   return false;
}

void Meta_Relationship::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Relationship::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Relationship::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( sub_context == "_302193" || sub_context == "child_List_Field_Child" )
      p_class_base = &child_List_Field_Child( );
   else if( sub_context == "_301475" || sub_context == "child_Specification_Child" )
      p_class_base = &child_Specification_Child( );
   else if( sub_context == "_302620" || sub_context == "child_Specification_Content_Page_Child_Self" )
      p_class_base = &child_Specification_Content_Page_Child_Self( );
   else if( sub_context == "_302632" || sub_context == "child_Specification_Content_Page_Link_Page_Link_Child" )
      p_class_base = &child_Specification_Content_Page_Link_Page_Link_Child( );
   else if( sub_context == "_302627" || sub_context == "child_Specification_Content_Page_Page_Page_Link_Child" )
      p_class_base = &child_Specification_Content_Page_Page_Page_Link_Child( );
   else if( sub_context == "_302651" || sub_context == "child_Specification_Copy_Child_Links_Source_Child" )
      p_class_base = &child_Specification_Copy_Child_Links_Source_Child( );
   else if( sub_context == "_301330" || sub_context == "child_Relationship_Source" )
      p_class_base = &child_Relationship_Source( );
   else if( sub_context == c_field_id_Child_Class || sub_context == c_field_name_Child_Class )
      p_class_base = &Child_Class( );
   else if( sub_context == c_field_id_Model || sub_context == c_field_name_Model )
      p_class_base = &Model( );
   else if( sub_context == c_field_id_Parent_Class || sub_context == c_field_name_Parent_Class )
      p_class_base = &Parent_Class( );
   else if( sub_context == c_field_id_Source_Relationship || sub_context == c_field_name_Source_Relationship )
      p_class_base = &Source_Relationship( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Relationship::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Cascade_Op" );
   names.push_back( "C_Child_Class" );
   names.push_back( "C_Extra" );
   names.push_back( "C_Field_Id" );
   names.push_back( "C_Field_Key" );
   names.push_back( "C_Internal" );
   names.push_back( "C_Mandatory" );
   names.push_back( "C_Model" );
   names.push_back( "C_Name" );
   names.push_back( "C_Parent_Class" );
   names.push_back( "C_Source_Relationship" );
   names.push_back( "C_Transient" );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Relationship::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( to_string( Cascade_Op( ) ) );
   values.push_back( sql_quote( to_string( Child_Class( ) ) ) );
   values.push_back( to_string( Extra( ) ) );
   values.push_back( sql_quote( to_string( Field_Id( ) ) ) );
   values.push_back( sql_quote( to_string( Field_Key( ) ) ) );
   values.push_back( to_string( Internal( ) ) );
   values.push_back( to_string( Mandatory( ) ) );
   values.push_back( sql_quote( to_string( Model( ) ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( sql_quote( to_string( Parent_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Relationship( ) ) ) );
   values.push_back( to_string( Transient( ) ) );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Relationship::get_required_field_names(
 set< string >& names, bool required_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, required_transients, dependents );

   // [(start meta_relationship_child_name)]
   if( needs_field_value( "Child_Name", dependents ) )
   {
      dependents.insert( "Name" );

      if( ( required_transients && is_field_transient( e_field_id_Name ) )
       || ( !required_transients && !is_field_transient( e_field_id_Name ) ) )
         names.insert( "Name" );
   }

   if( needs_field_value( "Child_Name", dependents ) )
   {
      dependents.insert( "Child_Class" );

      if( ( required_transients && is_field_transient( e_field_id_Child_Class ) )
       || ( !required_transients && !is_field_transient( e_field_id_Child_Class ) ) )
         names.insert( "Child_Class" );
   }

   if( needs_field_value( "Child_Name", dependents ) )
   {
      dependents.insert( "Parent_Class" );

      if( ( required_transients && is_field_transient( e_field_id_Parent_Class ) )
       || ( !required_transients && !is_field_transient( e_field_id_Parent_Class ) ) )
         names.insert( "Parent_Class" );
   }
   // [(finish meta_relationship_child_name)]

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Relationship::get_always_required_field_names(
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

   // [(start modifier_field_value)]
   dependents.insert( "Transient" ); // (for Is_Transient modifier)

   if( ( required_transients && is_field_transient( e_field_id_Transient ) )
    || ( !required_transients && !is_field_transient( e_field_id_Transient ) ) )
      names.insert( "Transient" );
   // [(finish modifier_field_value)]

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Relationship::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Relationship::do_generate_sql( generate_sql_type type, vector< string >& sql_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts );
}

const char* Meta_Relationship::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Relationship::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Relationship::static_lock_class_id( )
{
   return "113100";
}

const char* Meta_Relationship::static_check_class_name( )
{
   return "Relationship";
}

bool Meta_Relationship::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Relationship::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.113100" );
}

void Meta_Relationship::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "113103", "Cascade_Op", "int", false ) );
   all_field_info.push_back( field_info( "301310", "Child_Class", "Meta_Class", true ) );
   all_field_info.push_back( field_info( "113109", "Child_Class_File_Field_Name", "string", false ) );
   all_field_info.push_back( field_info( "113108", "Child_Class_Name", "string", false ) );
   all_field_info.push_back( field_info( "113105", "Child_Name", "string", false ) );
   all_field_info.push_back( field_info( "113104", "Extra", "int", false ) );
   all_field_info.push_back( field_info( "113106", "Field_Id", "string", false ) );
   all_field_info.push_back( field_info( "113107", "Field_Key", "string", false ) );
   all_field_info.push_back( field_info( "113110", "Internal", "bool", false ) );
   all_field_info.push_back( field_info( "113102", "Mandatory", "bool", false ) );
   all_field_info.push_back( field_info( "301300", "Model", "Meta_Model", true ) );
   all_field_info.push_back( field_info( "113101", "Name", "string", false ) );
   all_field_info.push_back( field_info( "301320", "Parent_Class", "Meta_Class", true ) );
   all_field_info.push_back( field_info( "301330", "Source_Relationship", "Meta_Relationship", false ) );
   all_field_info.push_back( field_info( "113111", "Transient", "bool", false ) );
}

void Meta_Relationship::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Child_Class, make_pair( "Meta.113100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Model, make_pair( "Meta.113100", "Meta_Model" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Parent_Class, make_pair( "Meta.113100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Relationship, make_pair( "Meta.113100", "Meta_Relationship" ) ) );
}

int Meta_Relationship::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Relationship::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Relationship::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "113103";
      break;

      case 2:
      p_id = "301310";
      break;

      case 3:
      p_id = "113109";
      break;

      case 4:
      p_id = "113108";
      break;

      case 5:
      p_id = "113105";
      break;

      case 6:
      p_id = "113104";
      break;

      case 7:
      p_id = "113106";
      break;

      case 8:
      p_id = "113107";
      break;

      case 9:
      p_id = "113110";
      break;

      case 10:
      p_id = "113102";
      break;

      case 11:
      p_id = "301300";
      break;

      case 12:
      p_id = "113101";
      break;

      case 13:
      p_id = "301320";
      break;

      case 14:
      p_id = "301330";
      break;

      case 15:
      p_id = "113111";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Relationship" );

   return p_id;
}

const char* Meta_Relationship::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Cascade_Op";
      break;

      case 2:
      p_id = "Child_Class";
      break;

      case 3:
      p_id = "Child_Class_File_Field_Name";
      break;

      case 4:
      p_id = "Child_Class_Name";
      break;

      case 5:
      p_id = "Child_Name";
      break;

      case 6:
      p_id = "Extra";
      break;

      case 7:
      p_id = "Field_Id";
      break;

      case 8:
      p_id = "Field_Key";
      break;

      case 9:
      p_id = "Internal";
      break;

      case 10:
      p_id = "Mandatory";
      break;

      case 11:
      p_id = "Model";
      break;

      case 12:
      p_id = "Name";
      break;

      case 13:
      p_id = "Parent_Class";
      break;

      case 14:
      p_id = "Source_Relationship";
      break;

      case 15:
      p_id = "Transient";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Relationship" );

   return p_id;
}

int Meta_Relationship::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Cascade_Op || field == c_field_name_Cascade_Op )
      rc += 1;
   else if( field == c_field_id_Child_Class || field == c_field_name_Child_Class )
      rc += 2;
   else if( field == c_field_id_Child_Class_File_Field_Name || field == c_field_name_Child_Class_File_Field_Name )
      rc += 3;
   else if( field == c_field_id_Child_Class_Name || field == c_field_name_Child_Class_Name )
      rc += 4;
   else if( field == c_field_id_Child_Name || field == c_field_name_Child_Name )
      rc += 5;
   else if( field == c_field_id_Extra || field == c_field_name_Extra )
      rc += 6;
   else if( field == c_field_id_Field_Id || field == c_field_name_Field_Id )
      rc += 7;
   else if( field == c_field_id_Field_Key || field == c_field_name_Field_Key )
      rc += 8;
   else if( field == c_field_id_Internal || field == c_field_name_Internal )
      rc += 9;
   else if( field == c_field_id_Mandatory || field == c_field_name_Mandatory )
      rc += 10;
   else if( field == c_field_id_Model || field == c_field_name_Model )
      rc += 11;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 12;
   else if( field == c_field_id_Parent_Class || field == c_field_name_Parent_Class )
      rc += 13;
   else if( field == c_field_id_Source_Relationship || field == c_field_name_Source_Relationship )
      rc += 14;
   else if( field == c_field_id_Transient || field == c_field_name_Transient )
      rc += 15;

   return rc - 1;
}

procedure_info_container& Meta_Relationship::static_get_procedure_info( )
{
   static procedure_info_container procedures;

   return procedures;
}

string Meta_Relationship::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(64),"
    "C_Ver_ INTEGER NOT NULL,"
    "C_Rev_ INTEGER NOT NULL,"
    "C_Typ_ VARCHAR(24) NOT NULL,"
    "C_Cascade_Op INTEGER NOT NULL,"
    "C_Child_Class VARCHAR(64) NOT NULL,"
    "C_Extra INTEGER NOT NULL,"
    "C_Field_Id VARCHAR(200) NOT NULL,"
    "C_Field_Key VARCHAR(200) NOT NULL,"
    "C_Internal INTEGER NOT NULL,"
    "C_Mandatory INTEGER NOT NULL,"
    "C_Model VARCHAR(64) NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Parent_Class VARCHAR(64) NOT NULL,"
    "C_Source_Relationship VARCHAR(64) NOT NULL,"
    "C_Transient INTEGER NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Relationship::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Relationship::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "enum_cascade_op_0", get_enum_string_cascade_op( 0 ) ) );
   pairs.push_back( make_pair( "enum_cascade_op_1", get_enum_string_cascade_op( 1 ) ) );
   pairs.push_back( make_pair( "enum_cascade_op_2", get_enum_string_cascade_op( 2 ) ) );
   pairs.push_back( make_pair( "enum_cascade_op_-1", get_enum_string_cascade_op( -1 ) ) );

   pairs.push_back( make_pair( "enum_relationship_extra_0", get_enum_string_relationship_extra( 0 ) ) );
   pairs.push_back( make_pair( "enum_relationship_extra_2", get_enum_string_relationship_extra( 2 ) ) );
   pairs.push_back( make_pair( "enum_relationship_extra_3", get_enum_string_relationship_extra( 3 ) ) );
   pairs.push_back( make_pair( "enum_relationship_extra_1", get_enum_string_relationship_extra( 1 ) ) );
   pairs.push_back( make_pair( "enum_relationship_extra_4", get_enum_string_relationship_extra( 4 ) ) );
   pairs.push_back( make_pair( "enum_relationship_extra_5", get_enum_string_relationship_extra( 5 ) ) );
}

void Meta_Relationship::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Model,C_Child_Class,C_Name" );
}

void Meta_Relationship::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Model,C_Child_Class,C_Name" );
}

void Meta_Relationship::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Relationship::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Relationship::static_insert_external_alias( const string& module_and_class_id, Meta_Relationship* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Relationship::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Relationship::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_cascade_op_enum.insert( 0 );
   g_cascade_op_enum.insert( 1 );
   g_cascade_op_enum.insert( 2 );
   g_cascade_op_enum.insert( -1 );
   g_relationship_extra_enum.insert( 0 );
   g_relationship_extra_enum.insert( 2 );
   g_relationship_extra_enum.insert( 3 );
   g_relationship_extra_enum.insert( 1 );
   g_relationship_extra_enum.insert( 4 );
   g_relationship_extra_enum.insert( 5 );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Relationship::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}

