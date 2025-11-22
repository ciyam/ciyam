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

#include "Meta_Class.h"

#include "Meta_Relationship.h"
#include "Meta_List_Field.h"
#include "Meta_Specification.h"
#include "Meta_Specification_Field_Action.h"
#include "Meta_View_Field.h"
#include "Meta_Index.h"
#include "Meta_Initial_Record.h"
#include "Meta_Modifier_Affect.h"
#include "Meta_Modifier.h"
#include "Meta_Package_Option.h"
#include "Meta_Procedure.h"
#include "Meta_Permission.h"
#include "Meta_List.h"
#include "Meta_View.h"
#include "Meta_Model.h"
#include "Meta_Field.h"
#include "Meta_Class.h"

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

#include "Meta_Type.h"
#include "Meta_Enum.h"
#include "Meta_Enum_Item.h"
#include "Meta_Procedure_Arg.h"
#include "Meta_Specification_Type.h"
// [<finish includes>]

using namespace std;

// [<start namespaces>]
// [<finish namespaces>]

template< > inline string to_string( const Meta_Permission& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_List& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_View& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Model& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Field& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Class& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Permission& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_List& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_View& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Model& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Field& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Class& c, const string& s )
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

#include "Meta_Class.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Access_Permission = "300633";
const char* const c_field_id_Access_Restriction = "106124";
const char* const c_field_id_Change_Permission = "300634";
const char* const c_field_id_Change_Restriction = "106125";
const char* const c_field_id_Commands_File = "106110";
const char* const c_field_id_Create_List = "106123";
const char* const c_field_id_Create_Permission = "300635";
const char* const c_field_id_Create_Restriction = "106126";
const char* const c_field_id_Create_View = "106122";
const char* const c_field_id_Created_List = "300632";
const char* const c_field_id_Created_View = "300631";
const char* const c_field_id_Delay_Initial_Records = "106121";
const char* const c_field_id_Destroy_Permission = "300636";
const char* const c_field_id_Destroy_Restriction = "106127";
const char* const c_field_id_Extra = "106103";
const char* const c_field_id_Header_File = "106109";
const char* const c_field_id_Id = "106104";
const char* const c_field_id_Model = "300600";
const char* const c_field_id_Name = "106101";
const char* const c_field_id_Next_Field_Id = "106105";
const char* const c_field_id_Next_Procedure_Id = "106106";
const char* const c_field_id_Persistence_Extra = "106129";
const char* const c_field_id_Persistence_Type = "106128";
const char* const c_field_id_Plural = "106102";
const char* const c_field_id_Quick_Link_Field = "300630";
const char* const c_field_id_Source_Class = "300620";
const char* const c_field_id_Source_File = "106108";
const char* const c_field_id_Source_Model = "300610";
const char* const c_field_id_Static_Instance_Key = "106120";
const char* const c_field_id_Type = "106107";

const char* const c_field_name_Access_Permission = "Access_Permission";
const char* const c_field_name_Access_Restriction = "Access_Restriction";
const char* const c_field_name_Change_Permission = "Change_Permission";
const char* const c_field_name_Change_Restriction = "Change_Restriction";
const char* const c_field_name_Commands_File = "Commands_File";
const char* const c_field_name_Create_List = "Create_List";
const char* const c_field_name_Create_Permission = "Create_Permission";
const char* const c_field_name_Create_Restriction = "Create_Restriction";
const char* const c_field_name_Create_View = "Create_View";
const char* const c_field_name_Created_List = "Created_List";
const char* const c_field_name_Created_View = "Created_View";
const char* const c_field_name_Delay_Initial_Records = "Delay_Initial_Records";
const char* const c_field_name_Destroy_Permission = "Destroy_Permission";
const char* const c_field_name_Destroy_Restriction = "Destroy_Restriction";
const char* const c_field_name_Extra = "Extra";
const char* const c_field_name_Header_File = "Header_File";
const char* const c_field_name_Id = "Id";
const char* const c_field_name_Model = "Model";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Next_Field_Id = "Next_Field_Id";
const char* const c_field_name_Next_Procedure_Id = "Next_Procedure_Id";
const char* const c_field_name_Persistence_Extra = "Persistence_Extra";
const char* const c_field_name_Persistence_Type = "Persistence_Type";
const char* const c_field_name_Plural = "Plural";
const char* const c_field_name_Quick_Link_Field = "Quick_Link_Field";
const char* const c_field_name_Source_Class = "Source_Class";
const char* const c_field_name_Source_File = "Source_File";
const char* const c_field_name_Source_Model = "Source_Model";
const char* const c_field_name_Static_Instance_Key = "Static_Instance_Key";
const char* const c_field_name_Type = "Type";

const char* const c_field_display_name_Access_Permission = "field_class_access_permission";
const char* const c_field_display_name_Access_Restriction = "field_class_access_restriction";
const char* const c_field_display_name_Change_Permission = "field_class_change_permission";
const char* const c_field_display_name_Change_Restriction = "field_class_change_restriction";
const char* const c_field_display_name_Commands_File = "field_class_commands_file";
const char* const c_field_display_name_Create_List = "field_class_create_list";
const char* const c_field_display_name_Create_Permission = "field_class_create_permission";
const char* const c_field_display_name_Create_Restriction = "field_class_create_restriction";
const char* const c_field_display_name_Create_View = "field_class_create_view";
const char* const c_field_display_name_Created_List = "field_class_created_list";
const char* const c_field_display_name_Created_View = "field_class_created_view";
const char* const c_field_display_name_Delay_Initial_Records = "field_class_delay_initial_records";
const char* const c_field_display_name_Destroy_Permission = "field_class_destroy_permission";
const char* const c_field_display_name_Destroy_Restriction = "field_class_destroy_restriction";
const char* const c_field_display_name_Extra = "field_class_extra";
const char* const c_field_display_name_Header_File = "field_class_header_file";
const char* const c_field_display_name_Id = "field_class_id";
const char* const c_field_display_name_Model = "field_class_model";
const char* const c_field_display_name_Name = "field_class_name";
const char* const c_field_display_name_Next_Field_Id = "field_class_next_field_id";
const char* const c_field_display_name_Next_Procedure_Id = "field_class_next_procedure_id";
const char* const c_field_display_name_Persistence_Extra = "field_class_persistence_extra";
const char* const c_field_display_name_Persistence_Type = "field_class_persistence_type";
const char* const c_field_display_name_Plural = "field_class_plural";
const char* const c_field_display_name_Quick_Link_Field = "field_class_quick_link_field";
const char* const c_field_display_name_Source_Class = "field_class_source_class";
const char* const c_field_display_name_Source_File = "field_class_source_file";
const char* const c_field_display_name_Source_Model = "field_class_source_model";
const char* const c_field_display_name_Static_Instance_Key = "field_class_static_instance_key";
const char* const c_field_display_name_Type = "field_class_type";

const int c_num_fields = 30;

const char* const c_all_sorted_field_ids[ ] =
{
   "106101",
   "106102",
   "106103",
   "106104",
   "106105",
   "106106",
   "106107",
   "106108",
   "106109",
   "106110",
   "106120",
   "106121",
   "106122",
   "106123",
   "106124",
   "106125",
   "106126",
   "106127",
   "106128",
   "106129",
   "300600",
   "300610",
   "300620",
   "300630",
   "300631",
   "300632",
   "300633",
   "300634",
   "300635",
   "300636"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Access_Permission",
   "Access_Restriction",
   "Change_Permission",
   "Change_Restriction",
   "Commands_File",
   "Create_List",
   "Create_Permission",
   "Create_Restriction",
   "Create_View",
   "Created_List",
   "Created_View",
   "Delay_Initial_Records",
   "Destroy_Permission",
   "Destroy_Restriction",
   "Extra",
   "Header_File",
   "Id",
   "Model",
   "Name",
   "Next_Field_Id",
   "Next_Procedure_Id",
   "Persistence_Extra",
   "Persistence_Type",
   "Plural",
   "Quick_Link_Field",
   "Source_Class",
   "Source_File",
   "Source_Model",
   "Static_Instance_Key",
   "Type"
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
   "106122",
   "106123"
};

const char* const c_transient_sorted_field_names[ ] =
{
   "Create_List",
   "Create_View"
};

inline bool is_transient_field( const string& field )
{
   return binary_search( c_transient_sorted_field_ids,
    c_transient_sorted_field_ids + c_num_transient_fields, field.c_str( ), compare )
    || binary_search( c_transient_sorted_field_names,
    c_transient_sorted_field_names + c_num_transient_fields, field.c_str( ), compare );
}

const char* const c_procedure_id_Generate = "106410";

const uint64_t c_modifier_Is_Alias = UINT64_C( 0x100 );
const uint64_t c_modifier_Is_Not_Alias = UINT64_C( 0x200 );
const uint64_t c_modifier_Is_SQL_Persistence = UINT64_C( 0x400 );

aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Id_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Name_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Next_Field_Id_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Next_Procedure_Id_domain;
domain_string_max_size< 100 > g_Persistence_Extra_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Plural_domain;
aggregate_domain< string,
 domain_string_label_format,
 domain_string_max_size< 30 > > g_Static_Instance_Key_domain;

string g_group_field_name;
string g_level_field_name;
string g_order_field_name;
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_Class* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Class* > external_aliases_lookup_container;
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

string g_default_Access_Permission = string( );
int g_default_Access_Restriction = int( 0 );
string g_default_Change_Permission = string( );
int g_default_Change_Restriction = int( 0 );
string g_default_Commands_File = string( );
bool g_default_Create_List = bool( 0 );
string g_default_Create_Permission = string( );
int g_default_Create_Restriction = int( 0 );
bool g_default_Create_View = bool( 0 );
string g_default_Created_List = string( );
string g_default_Created_View = string( );
bool g_default_Delay_Initial_Records = bool( 0 );
string g_default_Destroy_Permission = string( );
int g_default_Destroy_Restriction = int( 0 );
int g_default_Extra = int( 0 );
string g_default_Header_File = string( );
string g_default_Id = string( );
string g_default_Model = string( );
string g_default_Name = string( );
string g_default_Next_Field_Id = string( );
string g_default_Next_Procedure_Id = string( );
string g_default_Persistence_Extra = string( );
int g_default_Persistence_Type = int( 0 );
string g_default_Plural = string( );
string g_default_Quick_Link_Field = string( );
string g_default_Source_Class = string( );
string g_default_Source_File = string( );
string g_default_Source_Model = string( );
string g_default_Static_Instance_Key = string( );
int g_default_Type = int( 0 );

set< int > g_view_access_restrict_enum;
set< int > g_view_change_restrict_enum;
set< int > g_list_restrict_enum;
set< int > g_class_extra_enum;
set< int > g_persistence_type_enum;
set< int > g_class_type_enum;

const int c_enum_view_access_restrict_none( 0 );
const int c_enum_view_access_restrict_owner_only( 1 );
const int c_enum_view_access_restrict_admin_only( 2 );
const int c_enum_view_access_restrict_admin_owner( 3 );
const int c_enum_view_access_restrict_denied_always( 4 );

string get_enum_string_view_access_restrict( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_access_restrict" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_access_restrict_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_view_access_restrict_owner_only";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_access_restrict_admin_only";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_view_access_restrict_admin_owner";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_view_access_restrict_denied_always";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_access_restrict" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_view_change_restrict_none( 0 );
const int c_enum_view_change_restrict_owner_only( 1 );
const int c_enum_view_change_restrict_admin_only( 2 );
const int c_enum_view_change_restrict_admin_owner( 3 );
const int c_enum_view_change_restrict_denied_always( 4 );

string get_enum_string_view_change_restrict( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_change_restrict" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_change_restrict_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_view_change_restrict_owner_only";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_change_restrict_admin_only";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_view_change_restrict_admin_owner";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_view_change_restrict_denied_always";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_change_restrict" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_list_restrict_none( 0 );
const int c_enum_list_restrict_owner_only( 1 );
const int c_enum_list_restrict_admin_only( 2 );
const int c_enum_list_restrict_admin_owner( 3 );
const int c_enum_list_restrict_denied_always( 4 );

string get_enum_string_list_restrict( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_restrict" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_restrict_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_restrict_owner_only";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_list_restrict_admin_only";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_list_restrict_admin_owner";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_list_restrict_denied_always";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_restrict" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_class_extra_none( 0 );
const int c_enum_class_extra_ordered( 1 );

string get_enum_string_class_extra( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for class_extra" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_class_extra_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_class_extra_ordered";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for class_extra" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_persistence_type_SQL_DB( 0 );
const int c_enum_persistence_type_ODS_Local_DB( 1 );
const int c_enum_persistence_type_ODS_Global_DB( 2 );
const int c_enum_persistence_type_System_Variables( 3 );

string get_enum_string_persistence_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for persistence_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_persistence_type_SQL_DB";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_persistence_type_ODS_Local_DB";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_persistence_type_ODS_Global_DB";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_persistence_type_System_Variables";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for persistence_type" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_class_type_normal( 0 );
const int c_enum_class_type_user( 1 );
const int c_enum_class_type_system( 4 );
const int c_enum_class_type_user_group( 2 );
const int c_enum_class_type_user_other( 3 );

string get_enum_string_class_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for class_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_class_type_normal";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_class_type_user";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_class_type_system";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_class_type_user_group";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_class_type_user_other";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for class_type" );

   return get_module_string( lower( string_name ) );
}

// [<start anonymous>]
//nyi
string get_mapped_id( const string& model, const string& id )
{
   static map< string, string > ids;

   string retval( id );

   string file_name( model + ".ids.txt" );
   if( ids.empty( ) && exists_file( file_name ) )
   {
      ifstream inpf( file_name.c_str( ) );
      if( !inpf )
         throw runtime_error( "unable to open '" + file_name + "' for input" );

      string next;
      while( getline( inpf, next ) )
      {
         if( !next.empty( ) )
         {
            string::size_type pos = next.find( '=' );
            if( pos == string::npos )
               throw runtime_error( "unexpected format in id map line '" + next + "'" );

            ids.insert( make_pair( next.substr( 0, pos ), next.substr( pos + 1 ) ) );
         }
      }

      if( !inpf.eof( ) )
         throw runtime_error( "unexpected error occurred whilst reading '" + file_name + "' for input" );
   }

   if( ids.count( id ) )
      retval = ids[ id ];

   return retval;
}

string expand_arg( const string& arg )
{
   string::size_type pos = arg.find_first_of( "\" |<>&" );
   if( pos == string::npos )
      return arg;

   string s( "\"" );
   for( size_t i = 0; i < arg.size( ); i++ )
   {
      if( arg[ i ] == '"' )
         s += '"';
      s += arg[ i ];
   }
   s += "\"";

   return s;
}

void get_customised_types_for_class( Meta_Class& obj, set< string >& customised_types )
{
   if( obj.child_Field( ).iterate_forwards( ) )
   {
      do
      {
         bool is_custom = false;
         string field_type( meta_field_type_name( obj.child_Field( ).Primitive( ),
          obj.child_Field( ).Mandatory( ), obj.child_Field( ).Parent_Class( ).Name( ), obj.Model( ).Name( ), &is_custom ) );

         if( is_custom )
            customised_types.insert( field_type );
      } while( obj.child_Field( ).iterate_next( ) );
   }
}
// [<finish anonymous>]

}

registration< Meta_Class > Class_registration( get_class_registry( ), "106100" );

class Meta_Class_command_functor;

class Meta_Class_command_handler : public command_handler
{
   friend class Meta_Class_command_functor;

   public:
   Meta_Class_command_handler( ) : p_Meta_Class( 0 ) { }

   void set_Meta_Class( Meta_Class* p_new_Meta_Class ) { p_Meta_Class = p_new_Meta_Class; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Class* p_Meta_Class;

   protected:
   string retval;
};

class Meta_Class_command_functor : public command_functor
{
   public:
   Meta_Class_command_functor( Meta_Class_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Class_command_handler& cmd_handler;
};

command_functor* Meta_Class_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Class_command_functor( dynamic_cast< Meta_Class_command_handler& >( handler ) );
}

void Meta_Class_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Class_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_Class_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Class->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Class->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Class_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Class->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Class->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Class_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Class_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_Class->Access_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Restriction ) || ( field_name == c_field_name_Access_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Class->Access_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Permission ) || ( field_name == c_field_name_Change_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_Class->Change_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Restriction ) || ( field_name == c_field_name_Change_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Class->Change_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Commands_File ) || ( field_name == c_field_name_Commands_File ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Class->Commands_File( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Create_List ) || ( field_name == c_field_name_Create_List ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Class->Create_List( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Create_Permission ) || ( field_name == c_field_name_Create_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_Class->Create_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Create_Restriction ) || ( field_name == c_field_name_Create_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Class->Create_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Create_View ) || ( field_name == c_field_name_Create_View ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Class->Create_View( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Created_List ) || ( field_name == c_field_name_Created_List ) ) )
      {
         handled = true;

         string_getter< Meta_List >( cmd_handler.p_Meta_Class->Created_List( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Created_View ) || ( field_name == c_field_name_Created_View ) ) )
      {
         handled = true;

         string_getter< Meta_View >( cmd_handler.p_Meta_Class->Created_View( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Delay_Initial_Records ) || ( field_name == c_field_name_Delay_Initial_Records ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Class->Delay_Initial_Records( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Destroy_Permission ) || ( field_name == c_field_name_Destroy_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_Class->Destroy_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Destroy_Restriction ) || ( field_name == c_field_name_Destroy_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Class->Destroy_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Extra ) || ( field_name == c_field_name_Extra ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Class->Extra( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Header_File ) || ( field_name == c_field_name_Header_File ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Class->Header_File( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Id ) || ( field_name == c_field_name_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Class->Id( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) ) )
      {
         handled = true;

         string_getter< Meta_Model >( cmd_handler.p_Meta_Class->Model( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Class->Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Next_Field_Id ) || ( field_name == c_field_name_Next_Field_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Class->Next_Field_Id( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Next_Procedure_Id ) || ( field_name == c_field_name_Next_Procedure_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Class->Next_Procedure_Id( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Persistence_Extra ) || ( field_name == c_field_name_Persistence_Extra ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Class->Persistence_Extra( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Persistence_Type ) || ( field_name == c_field_name_Persistence_Type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Class->Persistence_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Plural ) || ( field_name == c_field_name_Plural ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Class->Plural( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Quick_Link_Field ) || ( field_name == c_field_name_Quick_Link_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Class->Quick_Link_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Class ) || ( field_name == c_field_name_Source_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Class->Source_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_File ) || ( field_name == c_field_name_Source_File ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Class->Source_File( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Model ) || ( field_name == c_field_name_Source_Model ) ) )
      {
         handled = true;

         string_getter< Meta_Model >( cmd_handler.p_Meta_Class->Source_Model( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Static_Instance_Key ) || ( field_name == c_field_name_Static_Instance_Key ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Class->Static_Instance_Key( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Class->Type( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Class_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Class_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_Class_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, Meta_Permission >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Access_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Restriction ) || ( field_name == c_field_name_Access_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, int >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Access_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Permission ) || ( field_name == c_field_name_Change_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, Meta_Permission >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Change_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Restriction ) || ( field_name == c_field_name_Change_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, int >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Change_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Commands_File ) || ( field_name == c_field_name_Commands_File ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, string >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Commands_File, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Create_List ) || ( field_name == c_field_name_Create_List ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, bool >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Create_List, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Create_Permission ) || ( field_name == c_field_name_Create_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, Meta_Permission >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Create_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Create_Restriction ) || ( field_name == c_field_name_Create_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, int >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Create_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Create_View ) || ( field_name == c_field_name_Create_View ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, bool >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Create_View, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Created_List ) || ( field_name == c_field_name_Created_List ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, Meta_List >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Created_List, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Created_View ) || ( field_name == c_field_name_Created_View ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, Meta_View >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Created_View, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Delay_Initial_Records ) || ( field_name == c_field_name_Delay_Initial_Records ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, bool >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Delay_Initial_Records, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Destroy_Permission ) || ( field_name == c_field_name_Destroy_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, Meta_Permission >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Destroy_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Destroy_Restriction ) || ( field_name == c_field_name_Destroy_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, int >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Destroy_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Extra ) || ( field_name == c_field_name_Extra ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, int >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Extra, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Header_File ) || ( field_name == c_field_name_Header_File ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, string >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Header_File, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Id ) || ( field_name == c_field_name_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, string >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Id, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, Meta_Model >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Model, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, string >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Next_Field_Id ) || ( field_name == c_field_name_Next_Field_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, string >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Next_Field_Id, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Next_Procedure_Id ) || ( field_name == c_field_name_Next_Procedure_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, string >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Next_Procedure_Id, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Persistence_Extra ) || ( field_name == c_field_name_Persistence_Extra ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, string >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Persistence_Extra, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Persistence_Type ) || ( field_name == c_field_name_Persistence_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, int >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Persistence_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Plural ) || ( field_name == c_field_name_Plural ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, string >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Plural, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Quick_Link_Field ) || ( field_name == c_field_name_Quick_Link_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, Meta_Field >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Quick_Link_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Class ) || ( field_name == c_field_name_Source_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, Meta_Class >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Source_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_File ) || ( field_name == c_field_name_Source_File ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, string >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Source_File, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Model ) || ( field_name == c_field_name_Source_Model ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, Meta_Model >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Source_Model, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Static_Instance_Key ) || ( field_name == c_field_name_Static_Instance_Key ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, string >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Static_Instance_Key, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Class, int >(
          *cmd_handler.p_Meta_Class, &Meta_Class::Type, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Class_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Class_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_Class_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_Class->Access_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Change_Permission ) || ( field_name == c_field_name_Change_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_Class->Change_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Create_Permission ) || ( field_name == c_field_name_Create_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_Class->Create_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Created_List ) || ( field_name == c_field_name_Created_List ) )
         cmd_handler.retval = cmd_handler.p_Meta_Class->Created_List( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Created_View ) || ( field_name == c_field_name_Created_View ) )
         cmd_handler.retval = cmd_handler.p_Meta_Class->Created_View( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Destroy_Permission ) || ( field_name == c_field_name_Destroy_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_Class->Destroy_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) )
         cmd_handler.retval = cmd_handler.p_Meta_Class->Model( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Quick_Link_Field ) || ( field_name == c_field_name_Quick_Link_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_Class->Quick_Link_Field( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Class ) || ( field_name == c_field_name_Source_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Class->Source_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Model ) || ( field_name == c_field_name_Source_Model ) )
         cmd_handler.retval = cmd_handler.p_Meta_Class->Source_Model( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
   else if( command == c_cmd_Meta_Class_Generate )
   {
      cmd_handler.p_Meta_Class->Generate( );

      cmd_handler.retval.erase( );

   }
}

struct Meta_Class::impl : public Meta_Class_command_handler
{
   impl( Meta_Class& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Class( &o );

      add_commands( 0, Meta_Class_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Class_command_definitions ) );
   }

   Meta_Class& get_obj( ) const
   {
      return *cp_obj;
   }

   int impl_Access_Restriction( ) const { return lazy_fetch( p_obj ), v_Access_Restriction; }
   void impl_Access_Restriction( int Access_Restriction ) { v_Access_Restriction = Access_Restriction; }

   int impl_Change_Restriction( ) const { return lazy_fetch( p_obj ), v_Change_Restriction; }
   void impl_Change_Restriction( int Change_Restriction ) { v_Change_Restriction = Change_Restriction; }

   const string& impl_Commands_File( ) const { return lazy_fetch( p_obj ), v_Commands_File; }
   void impl_Commands_File( const string& Commands_File ) { sanity_check( Commands_File ); v_Commands_File = Commands_File; }

   bool impl_Create_List( ) const { return lazy_fetch( p_obj ), v_Create_List; }
   void impl_Create_List( bool Create_List ) { v_Create_List = Create_List; }

   int impl_Create_Restriction( ) const { return lazy_fetch( p_obj ), v_Create_Restriction; }
   void impl_Create_Restriction( int Create_Restriction ) { v_Create_Restriction = Create_Restriction; }

   bool impl_Create_View( ) const { return lazy_fetch( p_obj ), v_Create_View; }
   void impl_Create_View( bool Create_View ) { v_Create_View = Create_View; }

   bool impl_Delay_Initial_Records( ) const { return lazy_fetch( p_obj ), v_Delay_Initial_Records; }
   void impl_Delay_Initial_Records( bool Delay_Initial_Records ) { v_Delay_Initial_Records = Delay_Initial_Records; }

   int impl_Destroy_Restriction( ) const { return lazy_fetch( p_obj ), v_Destroy_Restriction; }
   void impl_Destroy_Restriction( int Destroy_Restriction ) { v_Destroy_Restriction = Destroy_Restriction; }

   int impl_Extra( ) const { return lazy_fetch( p_obj ), v_Extra; }
   void impl_Extra( int Extra ) { v_Extra = Extra; }

   const string& impl_Header_File( ) const { return lazy_fetch( p_obj ), v_Header_File; }
   void impl_Header_File( const string& Header_File ) { sanity_check( Header_File ); v_Header_File = Header_File; }

   const string& impl_Id( ) const { return lazy_fetch( p_obj ), v_Id; }
   void impl_Id( const string& Id ) { sanity_check( Id ); v_Id = Id; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { sanity_check( Name ); v_Name = Name; }

   const string& impl_Next_Field_Id( ) const { return lazy_fetch( p_obj ), v_Next_Field_Id; }
   void impl_Next_Field_Id( const string& Next_Field_Id ) { sanity_check( Next_Field_Id ); v_Next_Field_Id = Next_Field_Id; }

   const string& impl_Next_Procedure_Id( ) const { return lazy_fetch( p_obj ), v_Next_Procedure_Id; }
   void impl_Next_Procedure_Id( const string& Next_Procedure_Id ) { sanity_check( Next_Procedure_Id ); v_Next_Procedure_Id = Next_Procedure_Id; }

   const string& impl_Persistence_Extra( ) const { return lazy_fetch( p_obj ), v_Persistence_Extra; }
   void impl_Persistence_Extra( const string& Persistence_Extra ) { sanity_check( Persistence_Extra ); v_Persistence_Extra = Persistence_Extra; }

   int impl_Persistence_Type( ) const { return lazy_fetch( p_obj ), v_Persistence_Type; }
   void impl_Persistence_Type( int Persistence_Type ) { v_Persistence_Type = Persistence_Type; }

   const string& impl_Plural( ) const { return lazy_fetch( p_obj ), v_Plural; }
   void impl_Plural( const string& Plural ) { sanity_check( Plural ); v_Plural = Plural; }

   const string& impl_Source_File( ) const { return lazy_fetch( p_obj ), v_Source_File; }
   void impl_Source_File( const string& Source_File ) { sanity_check( Source_File ); v_Source_File = Source_File; }

   const string& impl_Static_Instance_Key( ) const { return lazy_fetch( p_obj ), v_Static_Instance_Key; }
   void impl_Static_Instance_Key( const string& Static_Instance_Key ) { sanity_check( Static_Instance_Key ); v_Static_Instance_Key = Static_Instance_Key; }

   int impl_Type( ) const { return lazy_fetch( p_obj ), v_Type; }
   void impl_Type( int Type ) { v_Type = Type; }

   Meta_Permission& impl_Access_Permission( )
   {
      if( !cp_Access_Permission )
      {
         cp_Access_Permission.init( );

         p_obj->setup_graph_parent( *cp_Access_Permission, c_field_id_Access_Permission, v_Access_Permission );
      }
      return *cp_Access_Permission;
   }

   const Meta_Permission& impl_Access_Permission( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Access_Permission )
      {
         cp_Access_Permission.init( );

         p_obj->setup_graph_parent( *cp_Access_Permission, c_field_id_Access_Permission, v_Access_Permission );
      }
      return *cp_Access_Permission;
   }

   void impl_Access_Permission( const string& key )
   {
      class_base_accessor cba( impl_Access_Permission( ) );
      cba.set_key( key );
   }

   Meta_Permission& impl_Change_Permission( )
   {
      if( !cp_Change_Permission )
      {
         cp_Change_Permission.init( );

         p_obj->setup_graph_parent( *cp_Change_Permission, c_field_id_Change_Permission, v_Change_Permission );
      }
      return *cp_Change_Permission;
   }

   const Meta_Permission& impl_Change_Permission( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Change_Permission )
      {
         cp_Change_Permission.init( );

         p_obj->setup_graph_parent( *cp_Change_Permission, c_field_id_Change_Permission, v_Change_Permission );
      }
      return *cp_Change_Permission;
   }

   void impl_Change_Permission( const string& key )
   {
      class_base_accessor cba( impl_Change_Permission( ) );
      cba.set_key( key );
   }

   Meta_Permission& impl_Create_Permission( )
   {
      if( !cp_Create_Permission )
      {
         cp_Create_Permission.init( );

         p_obj->setup_graph_parent( *cp_Create_Permission, c_field_id_Create_Permission, v_Create_Permission );
      }
      return *cp_Create_Permission;
   }

   const Meta_Permission& impl_Create_Permission( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Create_Permission )
      {
         cp_Create_Permission.init( );

         p_obj->setup_graph_parent( *cp_Create_Permission, c_field_id_Create_Permission, v_Create_Permission );
      }
      return *cp_Create_Permission;
   }

   void impl_Create_Permission( const string& key )
   {
      class_base_accessor cba( impl_Create_Permission( ) );
      cba.set_key( key );
   }

   Meta_List& impl_Created_List( )
   {
      if( !cp_Created_List )
      {
         cp_Created_List.init( );

         p_obj->setup_graph_parent( *cp_Created_List, c_field_id_Created_List, v_Created_List );
      }
      return *cp_Created_List;
   }

   const Meta_List& impl_Created_List( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Created_List )
      {
         cp_Created_List.init( );

         p_obj->setup_graph_parent( *cp_Created_List, c_field_id_Created_List, v_Created_List );
      }
      return *cp_Created_List;
   }

   void impl_Created_List( const string& key )
   {
      class_base_accessor cba( impl_Created_List( ) );
      cba.set_key( key );
   }

   Meta_View& impl_Created_View( )
   {
      if( !cp_Created_View )
      {
         cp_Created_View.init( );

         p_obj->setup_graph_parent( *cp_Created_View, c_field_id_Created_View, v_Created_View );
      }
      return *cp_Created_View;
   }

   const Meta_View& impl_Created_View( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Created_View )
      {
         cp_Created_View.init( );

         p_obj->setup_graph_parent( *cp_Created_View, c_field_id_Created_View, v_Created_View );
      }
      return *cp_Created_View;
   }

   void impl_Created_View( const string& key )
   {
      class_base_accessor cba( impl_Created_View( ) );
      cba.set_key( key );
   }

   Meta_Permission& impl_Destroy_Permission( )
   {
      if( !cp_Destroy_Permission )
      {
         cp_Destroy_Permission.init( );

         p_obj->setup_graph_parent( *cp_Destroy_Permission, c_field_id_Destroy_Permission, v_Destroy_Permission );
      }
      return *cp_Destroy_Permission;
   }

   const Meta_Permission& impl_Destroy_Permission( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Destroy_Permission )
      {
         cp_Destroy_Permission.init( );

         p_obj->setup_graph_parent( *cp_Destroy_Permission, c_field_id_Destroy_Permission, v_Destroy_Permission );
      }
      return *cp_Destroy_Permission;
   }

   void impl_Destroy_Permission( const string& key )
   {
      class_base_accessor cba( impl_Destroy_Permission( ) );
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

   Meta_Field& impl_Quick_Link_Field( )
   {
      if( !cp_Quick_Link_Field )
      {
         cp_Quick_Link_Field.init( );

         p_obj->setup_graph_parent( *cp_Quick_Link_Field, c_field_id_Quick_Link_Field, v_Quick_Link_Field );
      }
      return *cp_Quick_Link_Field;
   }

   const Meta_Field& impl_Quick_Link_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Quick_Link_Field )
      {
         cp_Quick_Link_Field.init( );

         p_obj->setup_graph_parent( *cp_Quick_Link_Field, c_field_id_Quick_Link_Field, v_Quick_Link_Field );
      }
      return *cp_Quick_Link_Field;
   }

   void impl_Quick_Link_Field( const string& key )
   {
      class_base_accessor cba( impl_Quick_Link_Field( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Source_Class( )
   {
      if( !cp_Source_Class )
      {
         cp_Source_Class.init( );

         p_obj->setup_graph_parent( *cp_Source_Class, c_field_id_Source_Class, v_Source_Class );
      }
      return *cp_Source_Class;
   }

   const Meta_Class& impl_Source_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Class )
      {
         cp_Source_Class.init( );

         p_obj->setup_graph_parent( *cp_Source_Class, c_field_id_Source_Class, v_Source_Class );
      }
      return *cp_Source_Class;
   }

   void impl_Source_Class( const string& key )
   {
      class_base_accessor cba( impl_Source_Class( ) );
      cba.set_key( key );
   }

   Meta_Model& impl_Source_Model( )
   {
      if( !cp_Source_Model )
      {
         cp_Source_Model.init( );

         p_obj->setup_graph_parent( *cp_Source_Model, c_field_id_Source_Model, v_Source_Model );
      }
      return *cp_Source_Model;
   }

   const Meta_Model& impl_Source_Model( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Model )
      {
         cp_Source_Model.init( );

         p_obj->setup_graph_parent( *cp_Source_Model, c_field_id_Source_Model, v_Source_Model );
      }
      return *cp_Source_Model;
   }

   void impl_Source_Model( const string& key )
   {
      class_base_accessor cba( impl_Source_Model( ) );
      cba.set_key( key );
   }

   Meta_Relationship& impl_child_Relationship_Child( )
   {
      if( !cp_child_Relationship_Child )
      {
         cp_child_Relationship_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Relationship_Child, "301310" );
      }
      return *cp_child_Relationship_Child;
   }

   const Meta_Relationship& impl_child_Relationship_Child( ) const
   {
      if( !cp_child_Relationship_Child )
      {
         cp_child_Relationship_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Relationship_Child, "301310" );
      }
      return *cp_child_Relationship_Child;
   }

   Meta_List_Field& impl_child_List_Field_Child_Rel_Child( )
   {
      if( !cp_child_List_Field_Child_Rel_Child )
      {
         cp_child_List_Field_Child_Rel_Child.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Child_Rel_Child, "302194" );
      }
      return *cp_child_List_Field_Child_Rel_Child;
   }

   const Meta_List_Field& impl_child_List_Field_Child_Rel_Child( ) const
   {
      if( !cp_child_List_Field_Child_Rel_Child )
      {
         cp_child_List_Field_Child_Rel_Child.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Child_Rel_Child, "302194" );
      }
      return *cp_child_List_Field_Child_Rel_Child;
   }

   Meta_Specification& impl_child_Specification_Child_Rel_Child( )
   {
      if( !cp_child_Specification_Child_Rel_Child )
      {
         cp_child_Specification_Child_Rel_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Child_Rel_Child, "301491" );
      }
      return *cp_child_Specification_Child_Rel_Child;
   }

   const Meta_Specification& impl_child_Specification_Child_Rel_Child( ) const
   {
      if( !cp_child_Specification_Child_Rel_Child )
      {
         cp_child_Specification_Child_Rel_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Child_Rel_Child, "301491" );
      }
      return *cp_child_Specification_Child_Rel_Child;
   }

   Meta_List_Field& impl_child_List_Field_Child_Rel_Source_Parent( )
   {
      if( !cp_child_List_Field_Child_Rel_Source_Parent )
      {
         cp_child_List_Field_Child_Rel_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Child_Rel_Source_Parent, "302197" );
      }
      return *cp_child_List_Field_Child_Rel_Source_Parent;
   }

   const Meta_List_Field& impl_child_List_Field_Child_Rel_Source_Parent( ) const
   {
      if( !cp_child_List_Field_Child_Rel_Source_Parent )
      {
         cp_child_List_Field_Child_Rel_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Child_Rel_Source_Parent, "302197" );
      }
      return *cp_child_List_Field_Child_Rel_Source_Parent;
   }

   Meta_Specification& impl_child_Specification_Field( )
   {
      if( !cp_child_Specification_Field )
      {
         cp_child_Specification_Field.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Field, "301492" );
      }
      return *cp_child_Specification_Field;
   }

   const Meta_Specification& impl_child_Specification_Field( ) const
   {
      if( !cp_child_Specification_Field )
      {
         cp_child_Specification_Field.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Field, "301492" );
      }
      return *cp_child_Specification_Field;
   }

   Meta_Specification_Field_Action& impl_child_Specification_Field_Action_New_Record( )
   {
      if( !cp_child_Specification_Field_Action_New_Record )
      {
         cp_child_Specification_Field_Action_New_Record.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Field_Action_New_Record, "302300" );
      }
      return *cp_child_Specification_Field_Action_New_Record;
   }

   const Meta_Specification_Field_Action& impl_child_Specification_Field_Action_New_Record( ) const
   {
      if( !cp_child_Specification_Field_Action_New_Record )
      {
         cp_child_Specification_Field_Action_New_Record.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Field_Action_New_Record, "302300" );
      }
      return *cp_child_Specification_Field_Action_New_Record;
   }

   Meta_Specification& impl_child_Specification_Other( )
   {
      if( !cp_child_Specification_Other )
      {
         cp_child_Specification_Other.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other, "301421" );
      }
      return *cp_child_Specification_Other;
   }

   const Meta_Specification& impl_child_Specification_Other( ) const
   {
      if( !cp_child_Specification_Other )
      {
         cp_child_Specification_Other.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Other, "301421" );
      }
      return *cp_child_Specification_Other;
   }

   Meta_Field& impl_child_Field_Parent( )
   {
      if( !cp_child_Field_Parent )
      {
         cp_child_Field_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_Field_Parent, "300730" );
      }
      return *cp_child_Field_Parent;
   }

   const Meta_Field& impl_child_Field_Parent( ) const
   {
      if( !cp_child_Field_Parent )
      {
         cp_child_Field_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_Field_Parent, "300730" );
      }
      return *cp_child_Field_Parent;
   }

   Meta_List_Field& impl_child_List_Field_Parent( )
   {
      if( !cp_child_List_Field_Parent )
      {
         cp_child_List_Field_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Parent, "302115" );
      }
      return *cp_child_List_Field_Parent;
   }

   const Meta_List_Field& impl_child_List_Field_Parent( ) const
   {
      if( !cp_child_List_Field_Parent )
      {
         cp_child_List_Field_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Parent, "302115" );
      }
      return *cp_child_List_Field_Parent;
   }

   Meta_List& impl_child_List_Parent( )
   {
      if( !cp_child_List_Parent )
      {
         cp_child_List_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Parent, "301997" );
      }
      return *cp_child_List_Parent;
   }

   const Meta_List& impl_child_List_Parent( ) const
   {
      if( !cp_child_List_Parent )
      {
         cp_child_List_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Parent, "301997" );
      }
      return *cp_child_List_Parent;
   }

   Meta_Relationship& impl_child_Relationship_Parent( )
   {
      if( !cp_child_Relationship_Parent )
      {
         cp_child_Relationship_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_Relationship_Parent, "301320" );
      }
      return *cp_child_Relationship_Parent;
   }

   const Meta_Relationship& impl_child_Relationship_Parent( ) const
   {
      if( !cp_child_Relationship_Parent )
      {
         cp_child_Relationship_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_Relationship_Parent, "301320" );
      }
      return *cp_child_Relationship_Parent;
   }

   Meta_List_Field& impl_child_List_Field_Source_Child( )
   {
      if( !cp_child_List_Field_Source_Child )
      {
         cp_child_List_Field_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Source_Child, "302175" );
      }
      return *cp_child_List_Field_Source_Child;
   }

   const Meta_List_Field& impl_child_List_Field_Source_Child( ) const
   {
      if( !cp_child_List_Field_Source_Child )
      {
         cp_child_List_Field_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Source_Child, "302175" );
      }
      return *cp_child_List_Field_Source_Child;
   }

   Meta_Specification& impl_child_Specification_Source_Child( )
   {
      if( !cp_child_Specification_Source_Child )
      {
         cp_child_Specification_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Source_Child, "301489" );
      }
      return *cp_child_Specification_Source_Child;
   }

   const Meta_Specification& impl_child_Specification_Source_Child( ) const
   {
      if( !cp_child_Specification_Source_Child )
      {
         cp_child_Specification_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Source_Child, "301489" );
      }
      return *cp_child_Specification_Source_Child;
   }

   Meta_Specification& impl_child_Specification_Source( )
   {
      if( !cp_child_Specification_Source )
      {
         cp_child_Specification_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Source, "301494" );
      }
      return *cp_child_Specification_Source;
   }

   const Meta_Specification& impl_child_Specification_Source( ) const
   {
      if( !cp_child_Specification_Source )
      {
         cp_child_Specification_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Source, "301494" );
      }
      return *cp_child_Specification_Source;
   }

   Meta_Specification& impl_child_Specification_Source_Field( )
   {
      if( !cp_child_Specification_Source_Field )
      {
         cp_child_Specification_Source_Field.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Source_Field, "301482" );
      }
      return *cp_child_Specification_Source_Field;
   }

   const Meta_Specification& impl_child_Specification_Source_Field( ) const
   {
      if( !cp_child_Specification_Source_Field )
      {
         cp_child_Specification_Source_Field.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Source_Field, "301482" );
      }
      return *cp_child_Specification_Source_Field;
   }

   Meta_List_Field& impl_child_List_Field_Source_Parent( )
   {
      if( !cp_child_List_Field_Source_Parent )
      {
         cp_child_List_Field_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Source_Parent, "302170" );
      }
      return *cp_child_List_Field_Source_Parent;
   }

   const Meta_List_Field& impl_child_List_Field_Source_Parent( ) const
   {
      if( !cp_child_List_Field_Source_Parent )
      {
         cp_child_List_Field_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Source_Parent, "302170" );
      }
      return *cp_child_List_Field_Source_Parent;
   }

   Meta_Specification& impl_child_Specification_Source_Parent( )
   {
      if( !cp_child_Specification_Source_Parent )
      {
         cp_child_Specification_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Source_Parent, "301490" );
      }
      return *cp_child_Specification_Source_Parent;
   }

   const Meta_Specification& impl_child_Specification_Source_Parent( ) const
   {
      if( !cp_child_Specification_Source_Parent )
      {
         cp_child_Specification_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Source_Parent, "301490" );
      }
      return *cp_child_Specification_Source_Parent;
   }

   Meta_View_Field& impl_child_View_Field_Source_Parent( )
   {
      if( !cp_child_View_Field_Source_Parent )
      {
         cp_child_View_Field_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_View_Field_Source_Parent, "301970" );
      }
      return *cp_child_View_Field_Source_Parent;
   }

   const Meta_View_Field& impl_child_View_Field_Source_Parent( ) const
   {
      if( !cp_child_View_Field_Source_Parent )
      {
         cp_child_View_Field_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_View_Field_Source_Parent, "301970" );
      }
      return *cp_child_View_Field_Source_Parent;
   }

   Meta_Class& impl_child_Class_Source( )
   {
      if( !cp_child_Class_Source )
      {
         cp_child_Class_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Class_Source, "300620" );
      }
      return *cp_child_Class_Source;
   }

   const Meta_Class& impl_child_Class_Source( ) const
   {
      if( !cp_child_Class_Source )
      {
         cp_child_Class_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Class_Source, "300620" );
      }
      return *cp_child_Class_Source;
   }

   Meta_Specification& impl_child_Specification_Test_Field( )
   {
      if( !cp_child_Specification_Test_Field )
      {
         cp_child_Specification_Test_Field.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Test_Field, "301493" );
      }
      return *cp_child_Specification_Test_Field;
   }

   const Meta_Specification& impl_child_Specification_Test_Field( ) const
   {
      if( !cp_child_Specification_Test_Field )
      {
         cp_child_Specification_Test_Field.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Test_Field, "301493" );
      }
      return *cp_child_Specification_Test_Field;
   }

   Meta_Specification& impl_child_Specification_Test_Parent( )
   {
      if( !cp_child_Specification_Test_Parent )
      {
         cp_child_Specification_Test_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Test_Parent, "301487" );
      }
      return *cp_child_Specification_Test_Parent;
   }

   const Meta_Specification& impl_child_Specification_Test_Parent( ) const
   {
      if( !cp_child_Specification_Test_Parent )
      {
         cp_child_Specification_Test_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Test_Parent, "301487" );
      }
      return *cp_child_Specification_Test_Parent;
   }

   Meta_Field& impl_child_Field( )
   {
      if( !cp_child_Field )
      {
         cp_child_Field.init( );

         p_obj->setup_graph_parent( *cp_child_Field, "300700" );
      }
      return *cp_child_Field;
   }

   const Meta_Field& impl_child_Field( ) const
   {
      if( !cp_child_Field )
      {
         cp_child_Field.init( );

         p_obj->setup_graph_parent( *cp_child_Field, "300700" );
      }
      return *cp_child_Field;
   }

   Meta_Index& impl_child_Index( )
   {
      if( !cp_child_Index )
      {
         cp_child_Index.init( );

         p_obj->setup_graph_parent( *cp_child_Index, "300800" );
      }
      return *cp_child_Index;
   }

   const Meta_Index& impl_child_Index( ) const
   {
      if( !cp_child_Index )
      {
         cp_child_Index.init( );

         p_obj->setup_graph_parent( *cp_child_Index, "300800" );
      }
      return *cp_child_Index;
   }

   Meta_Initial_Record& impl_child_Initial_Record( )
   {
      if( !cp_child_Initial_Record )
      {
         cp_child_Initial_Record.init( );

         p_obj->setup_graph_parent( *cp_child_Initial_Record, "302410" );
      }
      return *cp_child_Initial_Record;
   }

   const Meta_Initial_Record& impl_child_Initial_Record( ) const
   {
      if( !cp_child_Initial_Record )
      {
         cp_child_Initial_Record.init( );

         p_obj->setup_graph_parent( *cp_child_Initial_Record, "302410" );
      }
      return *cp_child_Initial_Record;
   }

   Meta_List_Field& impl_child_List_Field( )
   {
      if( !cp_child_List_Field )
      {
         cp_child_List_Field.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field, "302110" );
      }
      return *cp_child_List_Field;
   }

   const Meta_List_Field& impl_child_List_Field( ) const
   {
      if( !cp_child_List_Field )
      {
         cp_child_List_Field.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field, "302110" );
      }
      return *cp_child_List_Field;
   }

   Meta_List& impl_child_List( )
   {
      if( !cp_child_List )
      {
         cp_child_List.init( );

         p_obj->setup_graph_parent( *cp_child_List, "301992" );
      }
      return *cp_child_List;
   }

   const Meta_List& impl_child_List( ) const
   {
      if( !cp_child_List )
      {
         cp_child_List.init( );

         p_obj->setup_graph_parent( *cp_child_List, "301992" );
      }
      return *cp_child_List;
   }

   Meta_Modifier_Affect& impl_child_Modifier_Affect( )
   {
      if( !cp_child_Modifier_Affect )
      {
         cp_child_Modifier_Affect.init( );

         p_obj->setup_graph_parent( *cp_child_Modifier_Affect, "301010" );
      }
      return *cp_child_Modifier_Affect;
   }

   const Meta_Modifier_Affect& impl_child_Modifier_Affect( ) const
   {
      if( !cp_child_Modifier_Affect )
      {
         cp_child_Modifier_Affect.init( );

         p_obj->setup_graph_parent( *cp_child_Modifier_Affect, "301010" );
      }
      return *cp_child_Modifier_Affect;
   }

   Meta_Modifier& impl_child_Modifier( )
   {
      if( !cp_child_Modifier )
      {
         cp_child_Modifier.init( );

         p_obj->setup_graph_parent( *cp_child_Modifier, "300900" );
      }
      return *cp_child_Modifier;
   }

   const Meta_Modifier& impl_child_Modifier( ) const
   {
      if( !cp_child_Modifier )
      {
         cp_child_Modifier.init( );

         p_obj->setup_graph_parent( *cp_child_Modifier, "300900" );
      }
      return *cp_child_Modifier;
   }

   Meta_Package_Option& impl_child_Package_Option( )
   {
      if( !cp_child_Package_Option )
      {
         cp_child_Package_Option.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option, "302830a" );
      }
      return *cp_child_Package_Option;
   }

   const Meta_Package_Option& impl_child_Package_Option( ) const
   {
      if( !cp_child_Package_Option )
      {
         cp_child_Package_Option.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option, "302830a" );
      }
      return *cp_child_Package_Option;
   }

   Meta_Procedure& impl_child_Procedure( )
   {
      if( !cp_child_Procedure )
      {
         cp_child_Procedure.init( );

         p_obj->setup_graph_parent( *cp_child_Procedure, "301100" );
      }
      return *cp_child_Procedure;
   }

   const Meta_Procedure& impl_child_Procedure( ) const
   {
      if( !cp_child_Procedure )
      {
         cp_child_Procedure.init( );

         p_obj->setup_graph_parent( *cp_child_Procedure, "301100" );
      }
      return *cp_child_Procedure;
   }

   Meta_Specification& impl_child_Specification( )
   {
      if( !cp_child_Specification )
      {
         cp_child_Specification.init( );

         p_obj->setup_graph_parent( *cp_child_Specification, "301420" );
      }
      return *cp_child_Specification;
   }

   const Meta_Specification& impl_child_Specification( ) const
   {
      if( !cp_child_Specification )
      {
         cp_child_Specification.init( );

         p_obj->setup_graph_parent( *cp_child_Specification, "301420" );
      }
      return *cp_child_Specification;
   }

   Meta_View_Field& impl_child_View_Field( )
   {
      if( !cp_child_View_Field )
      {
         cp_child_View_Field.init( );

         p_obj->setup_graph_parent( *cp_child_View_Field, "301905" );
      }
      return *cp_child_View_Field;
   }

   const Meta_View_Field& impl_child_View_Field( ) const
   {
      if( !cp_child_View_Field )
      {
         cp_child_View_Field.init( );

         p_obj->setup_graph_parent( *cp_child_View_Field, "301905" );
      }
      return *cp_child_View_Field;
   }

   Meta_View& impl_child_View( )
   {
      if( !cp_child_View )
      {
         cp_child_View.init( );

         p_obj->setup_graph_parent( *cp_child_View, "301820" );
      }
      return *cp_child_View;
   }

   const Meta_View& impl_child_View( ) const
   {
      if( !cp_child_View )
      {
         cp_child_View.init( );

         p_obj->setup_graph_parent( *cp_child_View, "301820" );
      }
      return *cp_child_View;
   }

   void impl_Generate( );

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

   Meta_Class* p_obj;
   class_pointer< Meta_Class > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   int v_Access_Restriction;
   int v_Change_Restriction;
   string v_Commands_File;
   bool v_Create_List;
   int v_Create_Restriction;
   bool v_Create_View;
   bool v_Delay_Initial_Records;
   int v_Destroy_Restriction;
   int v_Extra;
   string v_Header_File;
   string v_Id;
   string v_Name;
   string v_Next_Field_Id;
   string v_Next_Procedure_Id;
   string v_Persistence_Extra;
   int v_Persistence_Type;
   string v_Plural;
   string v_Source_File;
   string v_Static_Instance_Key;
   int v_Type;

   string v_Access_Permission;
   mutable class_pointer< Meta_Permission > cp_Access_Permission;

   string v_Change_Permission;
   mutable class_pointer< Meta_Permission > cp_Change_Permission;

   string v_Create_Permission;
   mutable class_pointer< Meta_Permission > cp_Create_Permission;

   string v_Created_List;
   mutable class_pointer< Meta_List > cp_Created_List;

   string v_Created_View;
   mutable class_pointer< Meta_View > cp_Created_View;

   string v_Destroy_Permission;
   mutable class_pointer< Meta_Permission > cp_Destroy_Permission;

   string v_Model;
   mutable class_pointer< Meta_Model > cp_Model;

   string v_Quick_Link_Field;
   mutable class_pointer< Meta_Field > cp_Quick_Link_Field;

   string v_Source_Class;
   mutable class_pointer< Meta_Class > cp_Source_Class;

   string v_Source_Model;
   mutable class_pointer< Meta_Model > cp_Source_Model;

   mutable class_pointer< Meta_Relationship > cp_child_Relationship_Child;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Child_Rel_Child;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Child_Rel_Child;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Child_Rel_Source_Parent;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Field;
   mutable class_pointer< Meta_Specification_Field_Action > cp_child_Specification_Field_Action_New_Record;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Other;
   mutable class_pointer< Meta_Field > cp_child_Field_Parent;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Parent;
   mutable class_pointer< Meta_List > cp_child_List_Parent;
   mutable class_pointer< Meta_Relationship > cp_child_Relationship_Parent;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Source_Child;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Source_Child;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Source;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Source_Field;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Source_Parent;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Source_Parent;
   mutable class_pointer< Meta_View_Field > cp_child_View_Field_Source_Parent;
   mutable class_pointer< Meta_Class > cp_child_Class_Source;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Test_Field;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Test_Parent;
   mutable class_pointer< Meta_Field > cp_child_Field;
   mutable class_pointer< Meta_Index > cp_child_Index;
   mutable class_pointer< Meta_Initial_Record > cp_child_Initial_Record;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field;
   mutable class_pointer< Meta_List > cp_child_List;
   mutable class_pointer< Meta_Modifier_Affect > cp_child_Modifier_Affect;
   mutable class_pointer< Meta_Modifier > cp_child_Modifier;
   mutable class_pointer< Meta_Package_Option > cp_child_Package_Option;
   mutable class_pointer< Meta_Procedure > cp_child_Procedure;
   mutable class_pointer< Meta_Specification > cp_child_Specification;
   mutable class_pointer< Meta_View_Field > cp_child_View_Field;
   mutable class_pointer< Meta_View > cp_child_View;
};

void Meta_Class::impl::impl_Generate( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Generate_impl>]
//nyi
   // NOTE: If a recovery is in progress then do nothing.
   if( storage_locked_for_admin( ) )
      return;

   string model_name( get_obj( ).Model( ).Name( ) );
   string xrep_file_name( model_name + "_" + get_obj( ).Name( ) + ".vars.xrep" );
   string extra_lst_file_name( get_obj( ).Model( ).Name( ) + "_" + get_obj( ).Name( ) + ".extra.lst" );

   set< string > old_extras;

   if( exists_file( extra_lst_file_name ) )
      read_file_lines( extra_lst_file_name, old_extras );

   if( exists_file( extra_lst_file_name ) )
      remove_file( extra_lst_file_name );

   ofstream outf( xrep_file_name.c_str( ) );
   if( !outf )
      throw runtime_error( "unexpected error opening '" + xrep_file_name + "' for output" );

   outf << "\x60{\x60$class_id\x60=\x60'" << get_mapped_id( model_name, get_obj( ).Id( ) ) << "\x60'\x60}\n";
   outf << "\x60{\x60$class_name\x60=\x60'" << get_obj( ).Name( ) << "\x60'\x60}\n";
   outf << "\x60{\x60$plural_name\x60=\x60'" << get_obj( ).Plural( ) << "\x60'\x60}\n";
   outf << "\x60{\x60$class_type\x60=\x60'" << get_obj( ).Type( ) << "\x60'\x60}\n";
   outf << "\x60{\x60$class_version\x60=\x60'1\x60'\x60}\n";

   if( get_obj( ).Persistence_Type( ) != c_enum_persistence_type_SQL_DB )
   {
      outf << "\x60{\x60$persistence_type\x60=\x60'" << get_obj( ).Persistence_Type( ) << "\x60'\x60}\n";
      outf << "\x60{\x60$persistence_extra\x60=\x60'" << get_obj( ).Persistence_Extra( ) << "\x60'\x60}\n";
   }

   outf << "\x60{\x60$module_id\x60=\x60'" << get_obj( ).Model( ).Id( ) << "\x60'\x60}\n";
   outf << "\x60{\x60$module_name\x60=\x60'" << get_obj( ).Model( ).Name( ) << "\x60'\x60}\n";
   outf << "\x60{\x60$year_created\x60=\x60'" << get_obj( ).Model( ).Year_Created( ) << "\x60'\x60}\n";
   outf << "\x60{\x60}\n";

   outf << "\x60{\x60$is_alias\x60=\x60'"
    << ( get_obj( ).Source_Class( ).get_key( ).empty( ) ? "false" : "true" ) << "\x60'\x60}\n";

   string instance_create;
   switch( get_obj( ).Create_Restriction( ) )
   {
      case 0: // i.e. none
      break;

      case 1:
      instance_create = c_owner_only;
      break;

      case 2:
      instance_create = c_admin_only;
      break;

      case 3:
      instance_create = c_admin_owner;
      break;

      case 4:
      instance_create = c_denied_always;
      break;
   }

   if( !is_null( get_obj( ).Create_Permission( ) ) )
   {
      if( instance_create.empty( ) )
         instance_create = c_denied_always;
      instance_create += "=!" + get_obj( ).Create_Permission( ).Id( );
   }
   else if( instance_create.empty( ) )
      instance_create = c_anyone;

   outf << "\x60{\x60$instance_create\x60=\x60'" << instance_create << "\x60'\x60}\n";

   string instance_update;

   switch( get_obj( ).Change_Restriction( ) )
   {
      case 0: // i.e. none
      break;

      case 1:
      instance_update = c_owner_only;
      break;

      case 2:
      instance_update = c_admin_only;
      break;

      case 3:
      instance_update = c_admin_owner;
      break;

      case 4:
      instance_update = c_denied_always;
      break;
   }

   if( !is_null( get_obj( ).Change_Permission( ) ) )
   {
      if( instance_update.empty( ) )
         instance_update = c_denied_always;
      instance_update += "=!" + get_obj( ).Change_Permission( ).Id( );
   }
   else if( instance_update.empty( ) )
      instance_update = c_anyone;

   outf << "\x60{\x60$instance_update\x60=\x60'" << instance_update << "\x60'\x60}\n";

   string instance_destroy;

   switch( get_obj( ).Destroy_Restriction( ) )
   {
      case 0: // i.e. none
      break;

      case 1:
      instance_destroy = c_owner_only;
      break;

      case 2:
      instance_destroy = c_admin_only;
      break;

      case 3:
      instance_destroy = c_admin_owner;
      break;

      case 4:
      instance_destroy = c_denied_always;
      break;
   }

   if( !is_null( get_obj( ).Destroy_Permission( ) ) )
   {
      if( instance_destroy.empty( ) )
         instance_destroy = c_denied_always;
      instance_destroy += "=!" + get_obj( ).Destroy_Permission( ).Id( );
   }
   else if( instance_destroy.empty( ) )
      instance_destroy = c_anyone;

   outf << "\x60{\x60$instance_destroy\x60=\x60'" << instance_destroy << "\x60'\x60}\n";

   vector< pair< string, string > > default_values;

   string group_field, level_field, order_field, owner_field, user_perms_field, permission_field;

   vector< string > basic_fields;
   vector< string > parent_fields;
   vector< string > complex_fields;
   vector< string > encrypted_fields;
   vector< string > file_field_names;
   vector< string > transient_fields;
   vector< string > sql_numeric_fields;
   vector< string > text_search_fields;

   vector< pair< string, string > > commandable_fields;

   vector< string > mandatory_normal_fields;
   vector< string > mandatory_parent_fields;

   vector< string > domain_fields;
   map< string, string > domain_types;
   map< string, string > domain_masks;

   vector< string > auto_round_fields;

   set< string > parent_types;

   set< string > customised_types;

   set< string > enum_names;
   string all_enums, enum_fields;
   vector< pair< string, string > > enum_field_values;

   string sql_columns;

   if( get_obj( ).Source_Class( ).get_key( ).empty( ) )
   {
      sql_columns += "C_Key_ VARCHAR(" + to_string( c_max_key_length ) + "),\n";
      sql_columns += "C_Ver_ SMALLINT UNSIGNED NOT NULL,\n";
      sql_columns += "C_Rev_ BIGINT UNSIGNED NOT NULL,\n";
      sql_columns += "C_Sec_ BIGINT UNSIGNED NOT NULL,\n";
      sql_columns += "C_Typ_ VARCHAR(16) NOT NULL";
   }
   else
   {
      outf << "\x60{\x60}\n";
      outf << "\x60{\x60$base_fields\x60=\x60'\\\n";

      string key_info( to_string( Meta_Field::static_get_field_id( Meta_Field::e_field_id_Name ) ) + ' ' );
      if( get_obj( ).child_Field( ).iterate_forwards( key_info ) )
      {
         bool is_first = true;
         do
         {
            if( is_first )
               is_first = false;
            else
               outf << " \\\n";

            outf << get_obj( ).child_Field( ).Name( )
             << "," << get_obj( ).child_Field( ).Id( ) << "," << get_obj( ).Source_Class( ).Name( );
         } while( get_obj( ).child_Field( ).iterate_next( ) );
      }

      outf << "\x60'\x60}\n";
   }

   // FUTURE: Need to be able to handle derived class fields here.
   if( get_obj( ).Source_Class( ).get_key( ).empty( ) )
   {
      outf << "\x60{\x60$all_fields\x60=\x60'\\\n";

      string key_info( to_string( Meta_Field::static_get_field_id( Meta_Field::e_field_id_Name ) ) + ' ' );
      if( get_obj( ).child_Field( ).iterate_forwards( key_info ) )
      {
         bool is_first = true;
         do
         {
            bool is_custom = false;

            string field_type( meta_field_type_name( get_obj( ).child_Field( ).Primitive( ),
             get_obj( ).child_Field( ).Mandatory( ), get_obj( ).child_Field( ).Parent_Class( ).Name( ), get_obj( ).Model( ).Name( ), &is_custom ) );

            if( is_custom )
               customised_types.insert( field_type );

            // NOTE: As it is likely that one or more specifications will involve fields that belong to a
            // parent class any customised types that are used by that parent class are added also.
            if( !get_obj( ).child_Field( ).Parent_Class( ).Name( ).empty( ) )
               get_customised_types_for_class( get_obj( ).child_Field( ).Parent_Class( ), customised_types );

            int text_type;
            get_obj( ).child_Field( ).Get_Text_Type( text_type );

            if( !sql_columns.empty( ) && !get_obj( ).child_Field( ).Transient( ) )
               sql_columns += ",\nC_" + get_obj( ).child_Field( ).Name( ) + ' '
                + meta_sql_type( field_type, get_obj( ).child_Field( ).Mandatory( ), ( sql_char_type )text_type );

            if( is_first )
               is_first = false;
            else
               outf << " \\\n";

            string field_scope( c_both );

            if( get_obj( ).child_Field( ).Change_Scope( ) != 0 )
            {
               if( get_obj( ).child_Field( ).Change_Scope( ) == 1 ) // i.e. create_only
                  field_scope = string( c_create );
               else if( get_obj( ).child_Field( ).Change_Scope( ) == 2 ) // i.e. post_create
                  field_scope = string( c_update );
               else if( get_obj( ).child_Field( ).Change_Scope( ) == 3 ) // i.e. editing_only
                  field_scope = string( c_both );
               else if( get_obj( ).child_Field( ).Change_Scope( ) == 4 ) // i.e. viewing_only
                  field_scope = string( c_both );
               else if( get_obj( ).child_Field( ).Change_Scope( ) == 5 ) // i.e. updating_only
                  field_scope = string( c_update );
               else
                  throw runtime_error( "unexpected change scope #"
                   + to_string( get_obj( ).child_Field( ).Change_Scope( ) ) + " in Class::Generate" );
            }

            string field_update;

            switch( get_obj( ).child_Field( ).Change_Restriction( ) )
            {
               case 0: // i.e. none
               break;

               case 1:
               field_update = c_owner_only;
               break;

               case 2:
               field_update = c_admin_only;
               break;

               case 3:
               field_update = c_admin_owner;
               break;

               case 4:
               field_update = c_denied_always;
               break;
            }

            if( !is_null( get_obj( ).child_Field( ).Change_Permission( ) ) )
            {
               if( field_update.empty( ) )
                  field_update = c_denied_always;
               field_update += "=!" + get_obj( ).child_Field( ).Change_Permission( ).Id( );
            }
            else if( field_update.empty( ) )
               field_update = c_anyone;

            bool is_group_fk = false;
            bool is_owner_fk = false;

            if( get_obj( ).child_Field( ).Extra( ) == -2 ) // i.e. "user_group"
            {
               is_group_fk = true;
               group_field = get_obj( ).child_Field( ).Name( );
            }

            if( get_obj( ).child_Field( ).Extra( ) == 18 ) // i.e. "security_level"
               level_field = get_obj( ).child_Field( ).Name( );

            if( get_obj( ).child_Field( ).Extra( ) == -1 ) // i.e. "owning_user"
            {
               is_owner_fk = true;
               owner_field = get_obj( ).child_Field( ).Name( );
            }

            if( get_obj( ).child_Field( ).Extra( ) == 12 ) // i.e. "user_perms"
               user_perms_field = get_obj( ).child_Field( ).Name( );

            if( get_obj( ).child_Field( ).Extra( ) == 17 ) // i.e. "permission"
               permission_field = get_obj( ).child_Field( ).Name( );

            bool is_encrypted = false;

            if( get_obj( ).child_Field( ).Extra( ) != 1 // i.e. "file"
             && get_obj( ).child_Field( ).Extra( ) != 3 // i.e. "image"
             && get_obj( ).child_Field( ).Extra( ) != 21 ) // i.e. "file_link"
               is_encrypted = get_obj( ).child_Field( ).Encrypted( );

            bool is_transient = get_obj( ).child_Field( ).Transient( );

            outf << get_obj( ).child_Field( ).Name( ) << ',' << field_type << ','
             << get_mapped_id( model_name, get_obj( ).child_Field( ).Id( ) ) << ',' << field_scope
             << ',' << field_update << ',' << is_owner_fk << ',' << is_encrypted << ',' << is_transient;

            if( !get_obj( ).child_Field( ).Default( ).empty( ) )
            {
               if( get_obj( ).child_Field( ).Primitive( ) >= 4 )
                  default_values.push_back( make_pair(
                   get_obj( ).child_Field( ).Name( ), get_obj( ).child_Field( ).Default( ) ) );
               else
                  default_values.push_back( make_pair(
                   get_obj( ).child_Field( ).Name( ), "\"" + get_obj( ).child_Field( ).Default( ) + "\"" ) );
            }

            if( get_obj( ).child_Field( ).Primitive( ) == 1 || get_obj( ).child_Field( ).Primitive( ) == 4 )
               commandable_fields.push_back( make_pair( get_obj( ).child_Field( ).Name( ), field_type ) );

            bool is_numeric_literal = false;
            if( get_obj( ).child_Field( ).Primitive( ) >= 4 && get_obj( ).child_Field( ).Primitive( ) <= 6 )
            {
               is_numeric_literal = true;
               sql_numeric_fields.push_back( get_obj( ).child_Field( ).Name( ) );
            }

            if( !get_obj( ).child_Field( ).Enum( ).get_key( ).empty( )
             && !enum_names.count( get_obj( ).child_Field( ).Enum( ).Name( ) ) )
            {
               enum_names.insert( get_obj( ).child_Field( ).Enum( ).Name( ) );

               if( !all_enums.empty( ) )
                  all_enums += ' ';
               all_enums += get_obj( ).child_Field( ).Enum( ).Name( ) + ',' + field_type;

               if( is_numeric_literal )
                  all_enums += ",numlit";
               else
                  all_enums += ",strlit";

               string enum_name( get_obj( ).child_Field( ).Enum( ).Name( ) );

               if( !enum_fields.empty( ) )
                  enum_fields += ' ';
               enum_fields += get_obj( ).child_Field( ).Name( ) + ',' + enum_name;

               string enum_values;
               string enum_key_info( to_string( Meta_Enum_Item::static_get_field_id( Meta_Enum_Item::e_field_id_Order ) ) + ' ' );
               if( get_obj( ).child_Field( ).Enum( ).child_Enum_Item( ).iterate_forwards( enum_key_info ) )
               {
                  do
                  {
                     if( !enum_values.empty( ) )
                        enum_values += ' ';
                     enum_values += get_obj( ).child_Field( ).Enum( ).child_Enum_Item( ).Value( )
                      + '=' + get_obj( ).child_Field( ).Enum( ).child_Enum_Item( ).Label( );
                  } while( get_obj( ).child_Field( ).Enum( ).child_Enum_Item( ).iterate_next( ) );
               }

               enum_field_values.push_back( make_pair( enum_name, enum_values ) );
            }

            if( get_obj( ).child_Field( ).Extra( ) == 5 ) // i.e. "order"
               order_field = get_obj( ).child_Field( ).Name( );

            if( get_obj( ).child_Field( ).Extra( ) == 1 // i.e. "file"
             || get_obj( ).child_Field( ).Extra( ) == 3 // i.e. "image"
             || get_obj( ).child_Field( ).Extra( ) == 21 ) // i.e. "file_link"
               file_field_names.push_back( get_obj( ).child_Field( ).Name( ) );

            if( get_obj( ).child_Field( ).Encrypted( ) )
               encrypted_fields.push_back( get_obj( ).child_Field( ).Id( ) + "," + get_obj( ).child_Field( ).Name( ) );

            // NOTE: The "int" and "bool" types are the only basic primitives.
            if( get_obj( ).child_Field( ).Primitive( ) == 5 || get_obj( ).child_Field( ).Primitive( ) == 6 )
               basic_fields.push_back( get_obj( ).child_Field( ).Name( ) );
            else if( !get_obj( ).child_Field( ).Parent_Class( ).get_key( ).empty( ) )
            {
               parent_fields.push_back( get_obj( ).child_Field( ).Name( ) );
               if( get_obj( ).child_Field( ).Mandatory( ) )
                  mandatory_parent_fields.push_back( get_obj( ).child_Field( ).Name( ) );

               parent_types.insert( get_obj( ).Model( ).Name( ) + '_' + get_obj( ).child_Field( ).Parent_Class( ).Name( ) );
            }
            else
            {
               complex_fields.push_back( get_obj( ).child_Field( ).Name( ) );
               if( get_obj( ).child_Field( ).Mandatory( ) && get_obj( ).child_Field( ).Primitive( ) != 4 )
                  mandatory_normal_fields.push_back( get_obj( ).child_Field( ).Name( ) );
            }

            string domain_mask;

            string domain_type( meta_field_domain_type( get_obj( ).child_Field( ).Enum( ).Id( ),
             get_obj( ).child_Field( ).Primitive( ), get_obj( ).child_Field( ).Type( ).Max_Size( ),
             get_obj( ).child_Field( ).Type( ).Min_Value( ), get_obj( ).child_Field( ).Type( ).Max_Value( ),
             get_obj( ).child_Field( ).Type( ).Numeric_Digits( ), get_obj( ).child_Field( ).Type( ).Numeric_Decimals( ),
             get_obj( ).child_Field( ).Type( ).String_Domain( ), get_obj( ).child_Field( ).Type( ).Date_Precision( ),
             get_obj( ).child_Field( ).Type( ).Time_Precision( ), get_obj( ).child_Field( ).Type( ).Show_Plus_Sign( ),
             get_obj( ).child_Field( ).Type( ).Zero_Padding( ), get_obj( ).child_Field( ).Type( ).Fraction_Limit( ), domain_mask ) );

            if( !domain_type.empty( ) )
            {
               domain_fields.push_back( get_obj( ).child_Field( ).Name( ) );
               domain_types.insert( make_pair( get_obj( ).child_Field( ).Name( ), domain_type ) );

               if( !domain_mask.empty( ) )
                  domain_masks.insert( make_pair( get_obj( ).child_Field( ).Name( ), domain_mask ) );
            }

            if( get_obj( ).child_Field( ).Type( ).Auto_Round( ) )
            {
               string method;
               switch( get_obj( ).child_Field( ).Type( ).Rounding_Method( ) )
               {
                  case 1:
                  method = "numeric::e_round_method_up";
                  break;

                  case 2:
                  method = "numeric::e_round_method_down";
                  break;

                  case 3:
                  method = "numeric::e_round_method_normal";
                  break;

                  case 4:
                  method = "numeric::e_round_method_bankers";
                  break;

                  default:
                  throw runtime_error( "unknown round method #" + to_string( get_obj( ).child_Field( ).Type( ).Rounding_Method( ) ) );
               }

               auto_round_fields.push_back( get_obj( ).child_Field( ).Name( ) + ","
                + to_string( get_obj( ).child_Field( ).Type( ).Numeric_Decimals( ) ) + "," + method );
            }

            if( get_obj( ).child_Field( ).Transient( ) )
               transient_fields.push_back( get_obj( ).child_Field( ).Id( ) + "," + get_obj( ).child_Field( ).Name( ) );

            if( get_obj( ).child_Field( ).Use_In_Text_Search( ) )
               text_search_fields.push_back( get_obj( ).child_Field( ).Name( ) );
         } while( get_obj( ).child_Field( ).iterate_next( ) );

         if( !sql_columns.empty( ) )
            sql_columns += ",\nPRIMARY KEY(C_Key_)";
      }

      outf << "\x60'\x60}\n";
   }

   for( size_t i = 0; i < default_values.size( ); i++ )
      outf << "\x60{\x60$field_default_" << default_values[ i ].first << "\x60=\x60'" << default_values[ i ].second << "\x60'\x60}\n";

   if( !group_field.empty( ) )
      outf << "\x60{\x60$group_field\x60=\x60'" << group_field << "\x60'\x60}\n";

   if( !level_field.empty( ) )
      outf << "\x60{\x60$level_field\x60=\x60'" << level_field << "\x60'\x60}\n";

   if( !order_field.empty( ) )
      outf << "\x60{\x60$order_field\x60=\x60'" << order_field << "\x60'\x60}\n";

   if( !owner_field.empty( ) )
      outf << "\x60{\x60$owner_field\x60=\x60'" << owner_field << "\x60'\x60}\n";

   if( !permission_field.empty( ) )
      outf << "\x60{\x60$permission_field\x60=\x60'" << permission_field << "\x60'\x60}\n";

   if( !user_perms_field.empty( ) )
      outf << "\x60{\x60$permissions_field\x60=\x60'" << user_perms_field << "\x60'\x60}\n";

   if( !basic_fields.empty( ) )
   {
      outf << "\x60{\x60$basic_fields\x60=\x60'";
      for( size_t i = 0; i < basic_fields.size( ); i++ )
      {
         if( i > 0 )
            outf << ' ';
         outf << basic_fields[ i ];
      }
      outf << "\x60'\x60}\n";
   }

   if( !parent_fields.empty( ) )
   {
      outf << "\x60{\x60$parent_fields\x60=\x60'";
      for( size_t i = 0; i < parent_fields.size( ); i++ )
      {
         if( i > 0 )
            outf << ' ';
         outf << parent_fields[ i ];
      }
      outf << "\x60'\x60}\n";
   }

   if( !complex_fields.empty( ) )
   {
      outf << "\x60{\x60$complex_fields\x60=\x60'";
      for( size_t i = 0; i < complex_fields.size( ); i++ )
      {
         if( i > 0 )
            outf << ' ';
         outf << complex_fields[ i ];
      }
      outf << "\x60'\x60}\n";
   }

   if( !file_field_names.empty( ) )
   {
      outf << "\x60{\x60$file_field_names\x60=\x60'";
      for( size_t i = 0; i < file_field_names.size( ); i++ )
      {
         if( i > 0 )
            outf << ' ';
         outf << file_field_names[ i ];
      }
      outf << "\x60'\x60}\n";
   }

   if( !encrypted_fields.empty( ) )
   {
      outf << "\x60{\x60$encrypted_fields\x60=\x60'";
      for( size_t i = 0; i < encrypted_fields.size( ); i++ )
      {
         if( i > 0 )
            outf << ' ';
         outf << encrypted_fields[ i ];
      }
      outf << "\x60'\x60}\n";
   }

   if( !transient_fields.empty( ) )
   {
      outf << "\x60{\x60$transient_fields\x60=\x60'";
      for( size_t i = 0; i < transient_fields.size( ); i++ )
      {
         if( i > 0 )
            outf << ' ';
         outf << transient_fields[ i ];
      }
      outf << "\x60'\x60}\n";
   }

   if( !text_search_fields.empty( ) )
   {
      outf << "\x60{\x60$text_search_fields\x60=\x60'";
      for( size_t i = 0; i < text_search_fields.size( ); i++ )
      {
         if( i > 0 )
            outf << ' ';
         outf << text_search_fields[ i ];
      }
      outf << "\x60'\x60}\n";
   }

   if( !sql_numeric_fields.empty( ) )
   {
      outf << "\x60{\x60$sql_numeric_fields\x60=\x60'";
      for( size_t i = 0; i < sql_numeric_fields.size( ); i++ )
      {
         if( i > 0 )
            outf << ' ';
         outf << sql_numeric_fields[ i ];
      }
      outf << "\x60'\x60}\n";
   }

   outf << "\x60{\x60}\n";

   if( !domain_fields.empty( ) )
   {
      outf << "\x60{\x60$domain_fields\x60=\x60'";
      for( size_t i = 0; i < domain_fields.size( ); i++ )
      {
         if( i > 0 )
            outf << ' ';
         outf << domain_fields[ i ];
      }
      outf << "\x60'\x60}\n";

      for( size_t i = 0; i < domain_fields.size( ); i++ )
      {
         outf << "\x60{\x60$domain_type_" << domain_fields[ i ] << "\x60=\x60'" << domain_types[ domain_fields[ i ] ] << "\x60'\x60}\n";
         if( domain_masks.count( domain_fields[ i ] ) )
            outf << "\x60{\x60$domain_mask_" << domain_fields[ i ] << "\x60=\x60'" << domain_masks[ domain_fields[ i ] ] << "\x60'\x60}\n";
      }

      outf << "\x60{\x60}\n";
   }

   if( !auto_round_fields.empty( ) )
   {
      outf << "\x60{\x60$auto_round_fields\x60=\x60'";
      for( size_t i = 0; i < auto_round_fields.size( ); i++ )
      {
         if( i > 0 )
            outf << ' ';
         outf << auto_round_fields[ i ];
      }
      outf << "\x60'\x60}\n";
   }

   if( !commandable_fields.empty( ) )
   {
      outf << "\x60{\x60$commandable_fields\x60=\x60'";
      for( size_t i = 0; i < commandable_fields.size( ); i++ )
      {
         if( i > 0 )
            outf << ' ';
         outf << commandable_fields[ i ].first << ',' << commandable_fields[ i ].second;
      }
      outf << "\x60'\x60}\n";

      outf << "\x60{\x60}\n";
   }

   if( !mandatory_normal_fields.empty( ) || !mandatory_parent_fields.empty( ) )
   {
      if( !mandatory_normal_fields.empty( ) )
      {
         outf << "\x60{\x60$mandatory_normal_fields\x60=\x60'";
         for( size_t i = 0; i < mandatory_normal_fields.size( ); i++ )
         {
            if( i > 0 )
               outf << ' ';
            outf << mandatory_normal_fields[ i ];
         }
         outf << "\x60'\x60}\n";
      }

      if( !mandatory_parent_fields.empty( ) )
      {
         outf << "\x60{\x60$mandatory_parent_fields\x60=\x60'";
         for( size_t i = 0; i < mandatory_parent_fields.size( ); i++ )
         {
            if( i > 0 )
               outf << ' ';
            outf << mandatory_parent_fields[ i ];
         }
         outf << "\x60'\x60}\n";
      }

      outf << "\x60{\x60}\n";
   }

   bool had_child_info_parents_or_custom = false;

   if( get_obj( ).child_Relationship_Parent( ).iterate_forwards( ) )
   {
      set< string > child_types;
      set< string > trans_types;
      map< string, string > child_info;

      do
      {
         if( get_obj( ).child_Relationship_Parent( ).Internal( ) )
            continue;

         if( get_obj( ).child_Relationship_Parent( ).Transient( ) )
         {
            trans_types.insert( get_obj( ).Model( ).Name( ) + '_'
             + get_obj( ).child_Relationship_Parent( ).Child_Class( ).Name( ) );
         }

         string sort_name(
          get_obj( ).child_Relationship_Parent( ).Child_Class( ).Name( )
          + ' ' + get_obj( ).child_Relationship_Parent( ).Name( ) );

         // NOTE: As it is likely that one or more specifications will involve fields that belong to a
         // child class any customised types that are used by that child class are added also.
         get_customised_types_for_class( get_obj( ).child_Relationship_Parent( ).Child_Class( ), customised_types );

         string cascade_op;
         switch( get_obj( ).child_Relationship_Parent( ).Cascade_Op( ) )
         {
            case -1:
            cascade_op = "none";
            break;

            case 0:
            cascade_op = "restrict";
            break;

            case 1:
            cascade_op = "unlink";
            break;

            case 2:
            cascade_op = "destroy";
            break;

            default:
            throw runtime_error( "unknown cascade op #" + to_string( get_obj( ).child_Relationship_Parent( ).Cascade_Op( ) ) );
         }

         child_info.insert( make_pair( sort_name, get_obj( ).child_Relationship_Parent( ).Child_Name( ) + ','
          + get_obj( ).Model( ).Name( ) + '_' + get_obj( ).child_Relationship_Parent( ).Child_Class( ).Name( ) + ','
          + get_obj( ).child_Relationship_Parent( ).Name( ) + ',' + get_mapped_id( model_name, get_obj( ).child_Relationship_Parent( ).Field_Id( ) ) + ',' + cascade_op ) );

         child_types.insert( get_obj( ).Model( ).Name( ) + '_' + get_obj( ).child_Relationship_Parent( ).Child_Class( ).Name( ) );
      } while( get_obj( ).child_Relationship_Parent( ).iterate_next( ) );

      if( !child_info.empty( ) )
      {
         had_child_info_parents_or_custom = true;

         bool is_first = true;
         outf << "\x60{\x60$child_info\x60=\x60'\\\n";

         for( map< string, string >::const_iterator i = child_info.begin( ), end = child_info.end( ); i != end; ++i )
         {
            if( is_first )
               is_first = false;
            else
               outf << " \\\n";

            outf << "child_" << i->second;
         }

         outf << "\x60'\x60}\n";
         outf << "\x60{\x60}\n";

         is_first = true;
         outf << "\x60{\x60$child_types\x60=\x60'";

         for( set< string >::const_iterator i = child_types.begin( ), end = child_types.end( ); i != end; ++i )
         {
            if( is_first )
               is_first = false;
            else
               outf << ' ';

            outf << *i;
         }

         outf << "\x60'\x60}\n";
      }

      if( !trans_types.empty( ) )
      {
         bool is_first = true;
         outf << "\x60{\x60$trans_types\x60=\x60'";

         for( set< string >::const_iterator i = trans_types.begin( ), end = trans_types.end( ); i != end; ++i )
         {
            if( is_first )
               is_first = false;
            else
               outf << ' ';

            outf << *i;
         }

         outf << "\x60'\x60}\n";
      }
   }

   if( !get_obj( ).Static_Instance_Key( ).empty( ) )
   {
      outf << "\x60{\x60}\n";
      outf << "\x60{\x60$fixed_key\x60=\x60'" << get_obj( ).Static_Instance_Key( ) << "\x60'\x60}\n";
   }

   // FUTURE: Need to be able to handle being a derived class here.
   if( !get_obj( ).Source_Class( ).get_key( ).empty( ) )
   {
      outf << "\x60{\x60}\n";

      if( get_obj( ).Source_Model( ).get_key( ).empty( ) )
         outf << "\x60{\x60$parent_class\x60=\x60'"
          << get_obj( ).Model( ).Name( ) << "_" << get_obj( ).Source_Class( ).Name( );
      else
         outf << "\x60{\x60$parent_class\x60=\x60'"
          << get_obj( ).Source_Model( ).Name( ) << "_" << get_obj( ).Source_Class( ).Name( );
      outf << "\x60'\x60}\n";
   }

   if( !parent_types.empty( ) )
   {
      had_child_info_parents_or_custom = true;
      outf << "\x60{\x60$parent_types\x60=\x60'";

      bool is_first = true;
      for( set< string >::const_iterator i = parent_types.begin( ), end = parent_types.end( ); i != end; ++i )
      {
         if( is_first )
            is_first = false;
         else
            outf << ' ';

         outf << *i;
      }
      outf << "\x60'\x60}\n";
   }

   if( !customised_types.empty( ) )
   {
      had_child_info_parents_or_custom = true;

      outf << "\x60{\x60$customised_types\x60=\x60'";
      for( set< string >::const_iterator i = customised_types.begin( ), end = customised_types.end( ); i != end; ++i )
      {
         if( i != customised_types.begin( ) )
            outf << ' ';
         outf << *i;
      }
      outf << "\x60'\x60}\n";
   }

   if( had_child_info_parents_or_custom )
      outf << "\x60{\x60}\n";

   if( !all_enums.empty( ) )
   {
      outf << "\x60{\x60$all_enums\x60=\x60'" << all_enums << "\x60'\x60}\n";
      outf << "\x60{\x60$enum_fields\x60=\x60'" << enum_fields << "\x60'\x60}\n";
      outf << "\x60{\x60}\n";

      for( size_t i = 0; i < enum_field_values.size( ); i++ )
         outf << "\x60{\x60$enum_" << enum_field_values[ i ].first << "\x60=\x60'" << enum_field_values[ i ].second << "\x60'\x60}\n";

      outf << "\x60{\x60}\n";
   }

   outf << "\x60{\x60$sql_columns\x60=\x60'\\\n";
   outf << sql_columns << "\x60'\x60}\n";
   outf << "\x60{\x60}\n";

   // FUTURE: Need to be able to handle derived class columns and indexes here.
   if( get_obj( ).Source_Class( ).get_key( ).empty( ) )
   {
      string all_indexes, all_sql_indexes, all_unique_indexes, all_sql_unique_indexes;

      string index_key_info( to_string( Meta_Index::static_get_field_id( Meta_Index::e_field_id_Order ) ) + ' ' );

      if( get_obj( ).child_Index( ).iterate_forwards( index_key_info ) )
      {
         do
         {
            if( !all_indexes.empty( ) )
               all_indexes += ' ';

            vector< string > index_field_types;

            if( get_obj( ).child_Index( ).Field_1( ).Transient( ) )
               throw runtime_error( "Cannot use Transient field '" + get_obj( ).child_Index( ).Field_1( ).Name( ) + "' in an Index." );

            string next_index( get_obj( ).child_Index( ).Field_1( ).Name( ) );
            index_field_types.push_back( meta_field_type_name(
             get_obj( ).child_Index( ).Field_1( ).Primitive( ), get_obj( ).child_Index( ).Field_1( ).Mandatory( ), "", "" ) );

            string next_sql_index( "C_" + get_obj( ).child_Index( ).Field_1( ).Name( ) );

            if( !get_obj( ).child_Index( ).Field_2( ).get_key( ).empty( ) )
            {
               if( get_obj( ).child_Index( ).Field_2( ).Transient( ) )
                  throw runtime_error( "Cannot use Transient field '" + get_obj( ).child_Index( ).Field_2( ).Name( ) + "' in an Index." );

               next_index += ',' + get_obj( ).child_Index( ).Field_2( ).Name( );

               index_field_types.push_back( meta_field_type_name(
                get_obj( ).child_Index( ).Field_2( ).Primitive( ), get_obj( ).child_Index( ).Field_2( ).Mandatory( ), "", "" ) );

               next_sql_index += ",C_" + get_obj( ).child_Index( ).Field_2( ).Name( );
            }

            if( !get_obj( ).child_Index( ).Field_3( ).get_key( ).empty( ) )
            {
               if( get_obj( ).child_Index( ).Field_3( ).Transient( ) )
                  throw runtime_error( "Cannot use Transient field '" + get_obj( ).child_Index( ).Field_3( ).Name( ) + "' in an Index." );

               next_index += ',' + get_obj( ).child_Index( ).Field_3( ).Name( );

               index_field_types.push_back( meta_field_type_name(
                get_obj( ).child_Index( ).Field_3( ).Primitive( ), get_obj( ).child_Index( ).Field_3( ).Mandatory( ), "", "" ) );

               next_sql_index += ",C_" + get_obj( ).child_Index( ).Field_3( ).Name( );
            }

            if( !get_obj( ).child_Index( ).Field_4( ).get_key( ).empty( ) )
            {
               if( get_obj( ).child_Index( ).Field_4( ).Transient( ) )
                  throw runtime_error( "Cannot use Transient field '" + get_obj( ).child_Index( ).Field_4( ).Name( ) + "' in an Index." );

               next_index += ',' + get_obj( ).child_Index( ).Field_4( ).Name( );

               index_field_types.push_back( meta_field_type_name(
                get_obj( ).child_Index( ).Field_4( ).Primitive( ), get_obj( ).child_Index( ).Field_4( ).Mandatory( ), "", "" ) );

               next_sql_index += ",C_" + get_obj( ).child_Index( ).Field_4( ).Name( );
            }

            if( !get_obj( ).child_Index( ).Field_5( ).get_key( ).empty( ) )
            {
               if( get_obj( ).child_Index( ).Field_5( ).Transient( ) )
                  throw runtime_error( "Cannot use Transient field '" + get_obj( ).child_Index( ).Field_5( ).Name( ) + "' in an Index." );

               next_index += ',' + get_obj( ).child_Index( ).Field_5( ).Name( );

               index_field_types.push_back( meta_field_type_name(
                get_obj( ).child_Index( ).Field_5( ).Primitive( ), get_obj( ).child_Index( ).Field_5( ).Mandatory( ), "", "" ) );

               next_sql_index += ",C_" + get_obj( ).child_Index( ).Field_5( ).Name( );
            }

            if( get_obj( ).child_Index( ).Unique( ) )
            {
               if( !all_unique_indexes.empty( ) )
                  all_unique_indexes += ' ';
               all_unique_indexes += next_index;

               if( !all_sql_unique_indexes.empty( ) )
                  all_sql_unique_indexes += ' ';
               all_sql_unique_indexes += next_sql_index;
            }
            else
            {
               next_index += ",@pk";
               next_sql_index += ",C_Key_";
               index_field_types.push_back( "string" );
            }

            all_indexes += next_index + ';';

            if( !all_sql_indexes.empty( ) )
               all_sql_indexes += " \\\n";
            all_sql_indexes += next_sql_index;

            for( size_t i = 0; i < index_field_types.size( ); i++ )
            {
               if( i > 0 )
                  all_indexes += ',';
               all_indexes += index_field_types[ i ];
            }

         } while( get_obj( ).child_Index( ).iterate_next( ) );
      }

      outf << "\x60{\x60$all_indexes\x60=\x60'" << all_indexes << "\x60'\x60}\n";
      outf << "\x60{\x60$all_unique_indexes\x60=\x60'" << all_unique_indexes << "\x60'\x60}\n";
      outf << "\x60{\x60$all_sql_indexes\x60=\x60'\\\n";
      outf << all_sql_indexes << "\x60'\x60}\n";
      outf << "\x60{\x60$all_sql_unique_indexes\x60=\x60'" << all_sql_unique_indexes << "\x60'\x60}\n";
      outf << "\x60{\x60}\n";
   }

   string modifier_key_info( to_string( Meta_Modifier::static_get_field_id( Meta_Modifier::e_field_id_Name ) ) + ' ' );

   if( get_obj( ).child_Modifier( ).iterate_forwards( modifier_key_info ) )
   {
      string all_modifiers;
      uint64_t flag_value( 0x100u );

      do
      {
         ostringstream osstr;

         osstr << hex << flag_value;

         if( !all_modifiers.empty( ) )
            all_modifiers += ' ';
         all_modifiers += get_obj( ).child_Modifier( ).Name( ) + ',' + osstr.str( );

         flag_value <<= 1;

      } while( get_obj( ).child_Modifier( ).iterate_next( ) );

      outf << "\x60{\x60$all_modifiers\x60=\x60'" << all_modifiers << "\x60'\x60}\n";
      outf << "\x60{\x60}\n";
   }

   // FUTURE: Need to be able to handle derived class procedure here.
   if( get_obj( ).Source_Class( ).get_key( ).empty( ) )
   {
      string procedure_key_info( to_string( Meta_Procedure::static_get_field_id( Meta_Procedure::e_field_id_Name ) ) + ' ' );

      if( get_obj( ).child_Procedure( ).iterate_forwards( procedure_key_info ) )
      {
         string all_procedures, all_procedure_info;

         vector< string > all_procedure_names;

         map< string, string > all_procedure_args;
         map< string, string > all_procedure_inputs;
         map< string, string > all_procedure_outputs;

         map< string, string > all_procedure_non_inputs;
         map< string, string > all_procedure_simple_inputs;
         map< string, string > all_procedure_complex_inputs;

         do
         {
            if( !all_procedures.empty( ) )
               all_procedures += ' ';
            all_procedures += get_obj( ).child_Procedure( ).Name( );

            all_procedure_names.push_back( get_obj( ).child_Procedure( ).Name( ) );

            if( !all_procedure_info.empty( ) )
               all_procedure_info += ' ';
            all_procedure_info += get_obj( ).child_Procedure( ).Id( ) + ',' + get_obj( ).child_Procedure( ).Name( );

            string procedure_access;

            switch( get_obj( ).child_Procedure( ).Access_Restriction( ) )
            {
               case 0: // i.e. none
               break;

               case 1:
               procedure_access = c_owner_only;
               break;

               case 2:
               procedure_access = c_admin_only;
               break;

               case 3:
               procedure_access = c_admin_owner;
               break;

               case 4:
               procedure_access = c_denied_always;
               break;
            }

            if( !is_null( get_obj( ).child_Procedure( ).Access_Permission( ) ) )
            {
               if( procedure_access.empty( ) )
                  procedure_access = c_denied_always;
               procedure_access += "=!" + get_obj( ).child_Procedure( ).Access_Permission( ).Id( );
            }
            else if( procedure_access.empty( ) )
               procedure_access = c_anyone;

            all_procedure_info += ',' + procedure_access;

            string next_args, next_inputs, next_outputs, next_output_names, next_simple_inputs, next_complex_inputs;

            bool has_args = false;

            if( get_obj( ).child_Procedure( ).child_Procedure_Arg( ).iterate_forwards( ) )
            {
               do
               {
                  has_args = true;

                  int arg_primitive = get_obj( ).child_Procedure( ).child_Procedure_Arg( ).Primitive( );

                  string arg_type( meta_procedure_arg_type( arg_primitive ) );
                  string arg_name( get_obj( ).child_Procedure( ).child_Procedure_Arg( ).Name( ) );

                  if( !next_args.empty( ) )
                     next_args += ' ';
                  next_args += arg_name + ',' + arg_type;

                  if( get_obj( ).child_Procedure( ).child_Procedure_Arg( ).Type( ) == 0 )
                  {
                     if( !next_inputs.empty( ) )
                        next_inputs += ' ';
                     next_inputs += arg_name + ',' + arg_type;

                     if( arg_primitive >= 4 && arg_primitive <= 6 )
                     {
                        if( !next_simple_inputs.empty( ) )
                           next_simple_inputs += ' ';
                        next_simple_inputs += arg_name;
                     }
                     else
                     {
                        if( !next_complex_inputs.empty( ) )
                           next_complex_inputs += ' ';
                        next_complex_inputs += arg_name;
                     }
                  }
                  else
                  {
                     if( !next_outputs.empty( ) )
                        next_outputs += ' ';
                     next_outputs += arg_name + ',' + arg_type;

                     if( !next_output_names.empty( ) )
                        next_output_names += ' ';
                     next_output_names += arg_name;
                  }
               } while( get_obj( ).child_Procedure( ).child_Procedure_Arg( ).iterate_next( ) );
            }

            all_procedure_args.insert( make_pair( get_obj( ).child_Procedure( ).Name( ), next_args ) );
            all_procedure_inputs.insert( make_pair( get_obj( ).child_Procedure( ).Name( ), next_inputs ) );
            all_procedure_outputs.insert( make_pair( get_obj( ).child_Procedure( ).Name( ), next_outputs ) );

            if( has_args )
            {
               all_procedure_simple_inputs.insert( make_pair( get_obj( ).child_Procedure( ).Name( ), next_simple_inputs ) );
               all_procedure_complex_inputs.insert( make_pair( get_obj( ).child_Procedure( ).Name( ), next_complex_inputs ) );
               all_procedure_non_inputs.insert( make_pair( get_obj( ).child_Procedure( ).Name( ), next_output_names ) );
            }
         } while( get_obj( ).child_Procedure( ).iterate_next( ) );

         outf << "\x60{\x60$all_procedures\x60=\x60'" << all_procedures << "\x60'\x60}\n";
         outf << "\x60{\x60$all_procedure_info\x60=\x60'" << all_procedure_info << "\x60'\x60}\n";
         outf << "\x60{\x60}\n";

         for( size_t i = 0; i < all_procedure_names.size( ); i++ )
         {
            outf << "\x60{\x60$" << all_procedure_names[ i ] << "_all_args\x60=\x60'"
             << all_procedure_args[ all_procedure_names[ i ] ] << "\x60'\x60}\n";

            outf << "\x60{\x60$" << all_procedure_names[ i ] << "_all_inputs\x60=\x60'"
             << all_procedure_inputs[ all_procedure_names[ i ] ] << "\x60'\x60}\n";

            outf << "\x60{\x60$" << all_procedure_names[ i ] << "_all_outputs\x60=\x60'"
             << all_procedure_outputs[ all_procedure_names[ i ] ] << "\x60'\x60}\n";

            if( all_procedure_simple_inputs.count( all_procedure_names[ i ] ) )
               outf << "\x60{\x60$" << all_procedure_names[ i ] << "_all_simple_inputs\x60=\x60'"
                << all_procedure_simple_inputs[ all_procedure_names[ i ] ] << "\x60'\x60}\n";

            if( all_procedure_complex_inputs.count( all_procedure_names[ i ] ) )
               outf << "\x60{\x60$" << all_procedure_names[ i ] << "_all_complex_inputs\x60=\x60'"
                << all_procedure_complex_inputs[ all_procedure_names[ i ] ] << "\x60'\x60}\n";

            if( all_procedure_non_inputs.count( all_procedure_names[ i ] ) )
               outf << "\x60{\x60$" << all_procedure_names[ i ] << "_all_non_inputs\x60=\x60'"
                << all_procedure_non_inputs[ all_procedure_names[ i ] ] << "\x60'\x60}\n";

            outf << "\x60{\x60}\n";
         }
      }
   }

   // FUTURE: It would be more efficient to iterate only the non-child specifications here (assuming an appropriate index exists).
   string specification_key_info( to_string( Meta_Specification::static_get_field_id( Meta_Specification::e_field_id_Order ) ) + ' ' );

   if( get_obj( ).child_Specification( ).iterate_forwards( specification_key_info ) )
   {
      vector< string > all_specification_details;
      string all_specifications, all_specification_types;

      do
      {
         // NOTE: As there are Specification derivations a review is being
         // used to ensure that a dynamic instance will be used if required.
         get_obj( ).child_Specification( ).begin_review( get_obj( ).child_Specification( ).get_key( ) );

         if( !get_obj( ).child_Specification( ).Specification_Type( ).Is_Child_Only( ) && !get_obj( ).child_Specification( ).Vars( ).empty( ) )
         {
            string specification_name( get_obj( ).child_Specification( ).Name( ) );

            string::size_type spos = specification_name.find( ' ' );

            if( spos != string::npos )
               specification_name.erase( spos );

            string specification_type( get_obj( ).child_Specification( ).Specification_Type( ).get_key( ) );
            string specification_object( get_obj( ).child_Specification( ).Specification_Type( ).Specification_Object( ) );

            string gen_xrep, gen_extra;

            bool is_gen_script_object = false;
            bool is_gen_script_object_only = false;

            if( ( specification_object == "gen_script" )
             || ( specification_object == "gen_ext_script" )
             || ( specification_object == "field_from_script" )
             || ( specification_object == "gen_script_vars_list" ) )
            {
               gen_xrep = "./xrep ";

               if( specification_object == "gen_ext_script" )
                  gen_xrep += "@" + specification_type + ".xrep";
               else
                  gen_xrep += "@" + specification_type + ".cin.xrep";

               is_gen_script_object = true;

               if( specification_object != "field_from_script" )
                  is_gen_script_object_only = true;

               if( specification_object == "gen_script_vars_list" )
                  gen_extra = ".vars.lst";
            }

            if( !is_gen_script_object_only )
            {
               if( !all_specifications.empty( ) )
                  all_specifications += " \\\n";

               all_specifications += specification_name + ',' + specification_object;

               if( !all_specification_types.empty( ) )
                  all_specification_types += " \\\n";

               all_specification_types += specification_object;
            }

            string::size_type pos = 0;
            string specification_details;

            string vars( get_obj( ).child_Specification( ).All_Vars( ) );

            if( !is_gen_script_object_only )
               specification_details += "\x60{\x60$specification_" + specification_name
                + "_id\x60=\x60'" + get_mapped_id( model_name, get_obj( ).child_Specification( ).Id( ) ) + "\x60'\x60}\n";

            string gen_script;

            while( true )
            {
               pos = vars.find( '\n' );

               string next( vars.substr( 0, pos ) );

               string::size_type npos = next.find( '=' );

               if( npos == string::npos )
                  throw runtime_error( "unexpected format for specification vars '" + get_obj( ).child_Specification( ).Vars( ) + "'" );

               string next_value( next.substr( npos + 1 ) );

               if( next_value == "{nyi}" )
                  next_value.erase( );

               if( is_gen_script_object )
               {
                  if( next.substr( 0, npos ) == "script" )
                     gen_script = next_value;
                  else if( next.substr( 0, npos ) != "sections" )
                     gen_xrep += " " + expand_arg( next );
               }

               if( !is_gen_script_object_only )
                  specification_details += "\x60{\x60$specification_" + specification_name
                   + "_" + next.substr( 0, npos ) + "\x60=\x60'" + next_value + "\x60'\x60}\n";

               if( pos == string::npos )
                  break;

               vars.erase( 0, pos + 1 );
            }

            if( specification_object == "gen_script_vars_list" )
               gen_script = get_obj( ).Model( ).Name( ) + '.' + gen_script;

            if( is_gen_script_object )
            {
               gen_xrep += " >" + gen_script + gen_extra;

               if( specification_object == "gen_ext_script" )
                  gen_xrep += "\nchmod a+x " + gen_script + gen_extra;

               exec_system( gen_xrep );

               ofstream outl( extra_lst_file_name.c_str( ), ios::out | ios::app );

               if( !outl )
                  throw runtime_error( "unexpected error opening '" + extra_lst_file_name + "' for output" );

               outl << ( gen_script + gen_extra ) << '\n';

               if( old_extras.count( gen_script + gen_extra ) )
                  old_extras.erase( gen_script + gen_extra );

               if( specification_type == "gen_bulk" )
               {
                  string csv_name( search_replace( gen_script, ".cin", ".csv" ) );

                  if( csv_name != gen_script )
                  {
                     outl << csv_name << '\n';

                     if( old_extras.count( csv_name ) )
                        old_extras.erase( csv_name );
                  }

                  string log_name( search_replace( gen_script, ".cin", ".log" ) );

                  if( log_name != gen_script )
                  {
                     outl << log_name << '\n';

                     if( old_extras.count( log_name ) )
                        old_extras.erase( log_name );
                  }
               }

               outl.flush( );

               if( !outl.good( ) )
                  throw runtime_error( "output stream extra lst is bad" );
            }

            if( !is_gen_script_object_only )
               all_specification_details.push_back( specification_details );
         }

         get_obj( ).child_Specification( ).finish_review( );

      } while( get_obj( ).child_Specification( ).iterate_next( ) );

      outf << "\x60{\x60$all_specifications\x60=\x60'\\\n" << all_specifications << "\x60'\x60}\n";
      outf << "\x60{\x60}\n";
      outf << "\x60{\x60$all_specification_types\x60=\x60'\\\n" << all_specification_types << "\x60'\x60}\n";
      outf << "\x60{\x60}\n";

      for( size_t i = 0; i < all_specification_details.size( ); i++ )
      {
         outf << all_specification_details[ i ];
         outf << "\x60{\x60}\n";
      }
   }

   if( !old_extras.empty( ) )
   {
      string cleanup_filename( get_obj( ).Model( ).Name( ) + ".cleanup.sh" );

      ofstream outc( cleanup_filename.c_str( ), ios::out | ios::app );

      if( outc )
      {
         for( set< string >::iterator i = old_extras.begin( ); i != old_extras.end( ); ++i )
            outc << "if [ -f \"" << *i << "\" ]; then\n rm \"" << *i << "\"\nfi\n";
      }
   }

   outf.flush( );

   if( !outf.good( ) )
      throw runtime_error( "generate output stream is bad" );
   // [<finish Generate_impl>]
}

string Meta_Class::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Access_Permission( ) );
      break;

      case 1:
      retval = to_string( impl_Access_Restriction( ) );
      break;

      case 2:
      retval = to_string( impl_Change_Permission( ) );
      break;

      case 3:
      retval = to_string( impl_Change_Restriction( ) );
      break;

      case 4:
      retval = to_string( impl_Commands_File( ) );
      break;

      case 5:
      retval = to_string( impl_Create_List( ) );
      break;

      case 6:
      retval = to_string( impl_Create_Permission( ) );
      break;

      case 7:
      retval = to_string( impl_Create_Restriction( ) );
      break;

      case 8:
      retval = to_string( impl_Create_View( ) );
      break;

      case 9:
      retval = to_string( impl_Created_List( ) );
      break;

      case 10:
      retval = to_string( impl_Created_View( ) );
      break;

      case 11:
      retval = to_string( impl_Delay_Initial_Records( ) );
      break;

      case 12:
      retval = to_string( impl_Destroy_Permission( ) );
      break;

      case 13:
      retval = to_string( impl_Destroy_Restriction( ) );
      break;

      case 14:
      retval = to_string( impl_Extra( ) );
      break;

      case 15:
      retval = to_string( impl_Header_File( ) );
      break;

      case 16:
      retval = to_string( impl_Id( ) );
      break;

      case 17:
      retval = to_string( impl_Model( ) );
      break;

      case 18:
      retval = to_string( impl_Name( ) );
      break;

      case 19:
      retval = to_string( impl_Next_Field_Id( ) );
      break;

      case 20:
      retval = to_string( impl_Next_Procedure_Id( ) );
      break;

      case 21:
      retval = to_string( impl_Persistence_Extra( ) );
      break;

      case 22:
      retval = to_string( impl_Persistence_Type( ) );
      break;

      case 23:
      retval = to_string( impl_Plural( ) );
      break;

      case 24:
      retval = to_string( impl_Quick_Link_Field( ) );
      break;

      case 25:
      retval = to_string( impl_Source_Class( ) );
      break;

      case 26:
      retval = to_string( impl_Source_File( ) );
      break;

      case 27:
      retval = to_string( impl_Source_Model( ) );
      break;

      case 28:
      retval = to_string( impl_Static_Instance_Key( ) );
      break;

      case 29:
      retval = to_string( impl_Type( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_Class::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Class::impl, Meta_Permission >( *this, &Meta_Class::impl::impl_Access_Permission, value );
      break;

      case 1:
      func_string_setter< Meta_Class::impl, int >( *this, &Meta_Class::impl::impl_Access_Restriction, value );
      break;

      case 2:
      func_string_setter< Meta_Class::impl, Meta_Permission >( *this, &Meta_Class::impl::impl_Change_Permission, value );
      break;

      case 3:
      func_string_setter< Meta_Class::impl, int >( *this, &Meta_Class::impl::impl_Change_Restriction, value );
      break;

      case 4:
      func_string_setter< Meta_Class::impl, string >( *this, &Meta_Class::impl::impl_Commands_File, value );
      break;

      case 5:
      func_string_setter< Meta_Class::impl, bool >( *this, &Meta_Class::impl::impl_Create_List, value );
      break;

      case 6:
      func_string_setter< Meta_Class::impl, Meta_Permission >( *this, &Meta_Class::impl::impl_Create_Permission, value );
      break;

      case 7:
      func_string_setter< Meta_Class::impl, int >( *this, &Meta_Class::impl::impl_Create_Restriction, value );
      break;

      case 8:
      func_string_setter< Meta_Class::impl, bool >( *this, &Meta_Class::impl::impl_Create_View, value );
      break;

      case 9:
      func_string_setter< Meta_Class::impl, Meta_List >( *this, &Meta_Class::impl::impl_Created_List, value );
      break;

      case 10:
      func_string_setter< Meta_Class::impl, Meta_View >( *this, &Meta_Class::impl::impl_Created_View, value );
      break;

      case 11:
      func_string_setter< Meta_Class::impl, bool >( *this, &Meta_Class::impl::impl_Delay_Initial_Records, value );
      break;

      case 12:
      func_string_setter< Meta_Class::impl, Meta_Permission >( *this, &Meta_Class::impl::impl_Destroy_Permission, value );
      break;

      case 13:
      func_string_setter< Meta_Class::impl, int >( *this, &Meta_Class::impl::impl_Destroy_Restriction, value );
      break;

      case 14:
      func_string_setter< Meta_Class::impl, int >( *this, &Meta_Class::impl::impl_Extra, value );
      break;

      case 15:
      func_string_setter< Meta_Class::impl, string >( *this, &Meta_Class::impl::impl_Header_File, value );
      break;

      case 16:
      func_string_setter< Meta_Class::impl, string >( *this, &Meta_Class::impl::impl_Id, value );
      break;

      case 17:
      func_string_setter< Meta_Class::impl, Meta_Model >( *this, &Meta_Class::impl::impl_Model, value );
      break;

      case 18:
      func_string_setter< Meta_Class::impl, string >( *this, &Meta_Class::impl::impl_Name, value );
      break;

      case 19:
      func_string_setter< Meta_Class::impl, string >( *this, &Meta_Class::impl::impl_Next_Field_Id, value );
      break;

      case 20:
      func_string_setter< Meta_Class::impl, string >( *this, &Meta_Class::impl::impl_Next_Procedure_Id, value );
      break;

      case 21:
      func_string_setter< Meta_Class::impl, string >( *this, &Meta_Class::impl::impl_Persistence_Extra, value );
      break;

      case 22:
      func_string_setter< Meta_Class::impl, int >( *this, &Meta_Class::impl::impl_Persistence_Type, value );
      break;

      case 23:
      func_string_setter< Meta_Class::impl, string >( *this, &Meta_Class::impl::impl_Plural, value );
      break;

      case 24:
      func_string_setter< Meta_Class::impl, Meta_Field >( *this, &Meta_Class::impl::impl_Quick_Link_Field, value );
      break;

      case 25:
      func_string_setter< Meta_Class::impl, Meta_Class >( *this, &Meta_Class::impl::impl_Source_Class, value );
      break;

      case 26:
      func_string_setter< Meta_Class::impl, string >( *this, &Meta_Class::impl::impl_Source_File, value );
      break;

      case 27:
      func_string_setter< Meta_Class::impl, Meta_Model >( *this, &Meta_Class::impl::impl_Source_Model, value );
      break;

      case 28:
      func_string_setter< Meta_Class::impl, string >( *this, &Meta_Class::impl::impl_Static_Instance_Key, value );
      break;

      case 29:
      func_string_setter< Meta_Class::impl, int >( *this, &Meta_Class::impl::impl_Type, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_Class::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Access_Permission( g_default_Access_Permission );
      break;

      case 1:
      impl_Access_Restriction( g_default_Access_Restriction );
      break;

      case 2:
      impl_Change_Permission( g_default_Change_Permission );
      break;

      case 3:
      impl_Change_Restriction( g_default_Change_Restriction );
      break;

      case 4:
      impl_Commands_File( g_default_Commands_File );
      break;

      case 5:
      impl_Create_List( g_default_Create_List );
      break;

      case 6:
      impl_Create_Permission( g_default_Create_Permission );
      break;

      case 7:
      impl_Create_Restriction( g_default_Create_Restriction );
      break;

      case 8:
      impl_Create_View( g_default_Create_View );
      break;

      case 9:
      impl_Created_List( g_default_Created_List );
      break;

      case 10:
      impl_Created_View( g_default_Created_View );
      break;

      case 11:
      impl_Delay_Initial_Records( g_default_Delay_Initial_Records );
      break;

      case 12:
      impl_Destroy_Permission( g_default_Destroy_Permission );
      break;

      case 13:
      impl_Destroy_Restriction( g_default_Destroy_Restriction );
      break;

      case 14:
      impl_Extra( g_default_Extra );
      break;

      case 15:
      impl_Header_File( g_default_Header_File );
      break;

      case 16:
      impl_Id( g_default_Id );
      break;

      case 17:
      impl_Model( g_default_Model );
      break;

      case 18:
      impl_Name( g_default_Name );
      break;

      case 19:
      impl_Next_Field_Id( g_default_Next_Field_Id );
      break;

      case 20:
      impl_Next_Procedure_Id( g_default_Next_Procedure_Id );
      break;

      case 21:
      impl_Persistence_Extra( g_default_Persistence_Extra );
      break;

      case 22:
      impl_Persistence_Type( g_default_Persistence_Type );
      break;

      case 23:
      impl_Plural( g_default_Plural );
      break;

      case 24:
      impl_Quick_Link_Field( g_default_Quick_Link_Field );
      break;

      case 25:
      impl_Source_Class( g_default_Source_Class );
      break;

      case 26:
      impl_Source_File( g_default_Source_File );
      break;

      case 27:
      impl_Source_Model( g_default_Source_Model );
      break;

      case 28:
      impl_Static_Instance_Key( g_default_Static_Instance_Key );
      break;

      case 29:
      impl_Type( g_default_Type );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_Class::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Access_Permission == g_default_Access_Permission );
      break;

      case 1:
      retval = ( v_Access_Restriction == g_default_Access_Restriction );
      break;

      case 2:
      retval = ( v_Change_Permission == g_default_Change_Permission );
      break;

      case 3:
      retval = ( v_Change_Restriction == g_default_Change_Restriction );
      break;

      case 4:
      retval = ( v_Commands_File == g_default_Commands_File );
      break;

      case 5:
      retval = ( v_Create_List == g_default_Create_List );
      break;

      case 6:
      retval = ( v_Create_Permission == g_default_Create_Permission );
      break;

      case 7:
      retval = ( v_Create_Restriction == g_default_Create_Restriction );
      break;

      case 8:
      retval = ( v_Create_View == g_default_Create_View );
      break;

      case 9:
      retval = ( v_Created_List == g_default_Created_List );
      break;

      case 10:
      retval = ( v_Created_View == g_default_Created_View );
      break;

      case 11:
      retval = ( v_Delay_Initial_Records == g_default_Delay_Initial_Records );
      break;

      case 12:
      retval = ( v_Destroy_Permission == g_default_Destroy_Permission );
      break;

      case 13:
      retval = ( v_Destroy_Restriction == g_default_Destroy_Restriction );
      break;

      case 14:
      retval = ( v_Extra == g_default_Extra );
      break;

      case 15:
      retval = ( v_Header_File == g_default_Header_File );
      break;

      case 16:
      retval = ( v_Id == g_default_Id );
      break;

      case 17:
      retval = ( v_Model == g_default_Model );
      break;

      case 18:
      retval = ( v_Name == g_default_Name );
      break;

      case 19:
      retval = ( v_Next_Field_Id == g_default_Next_Field_Id );
      break;

      case 20:
      retval = ( v_Next_Procedure_Id == g_default_Next_Procedure_Id );
      break;

      case 21:
      retval = ( v_Persistence_Extra == g_default_Persistence_Extra );
      break;

      case 22:
      retval = ( v_Persistence_Type == g_default_Persistence_Type );
      break;

      case 23:
      retval = ( v_Plural == g_default_Plural );
      break;

      case 24:
      retval = ( v_Quick_Link_Field == g_default_Quick_Link_Field );
      break;

      case 25:
      retval = ( v_Source_Class == g_default_Source_Class );
      break;

      case 26:
      retval = ( v_Source_File == g_default_Source_File );
      break;

      case 27:
      retval = ( v_Source_Model == g_default_Source_Model );
      break;

      case 28:
      retval = ( v_Static_Instance_Key == g_default_Static_Instance_Key );
      break;

      case 29:
      retval = ( v_Type == g_default_Type );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_Class::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start modifier_field_null)] 600032
   if( !is_null( get_obj( ).Source_Model( ) ) )
      state |= c_modifier_Is_Alias;
   // [(finish modifier_field_null)] 600032

   // [(start modifier_field_null)] 600033
   if( is_null( get_obj( ).Source_Model( ) ) )
      state |= c_modifier_Is_Not_Alias;
   // [(finish modifier_field_null)] 600033

   // [(start modifier_field_value)] 600033a
   if( get_obj( ).Persistence_Type( ) == 0 )
      state |= c_modifier_Is_SQL_Persistence;
   // [(finish modifier_field_value)] 600033a

   // [(start protect_not_equal)] 600034
   if( check_not_equal( get_obj( ).Source_Model( ), "" ) )
      state |= ( c_state_uneditable );
   // [(finish protect_not_equal)] 600034

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

string Meta_Class::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   if( state & c_modifier_Is_Alias )
      state_names += "|" + string( "Is_Alias" );
   if( state & c_modifier_Is_Not_Alias )
      state_names += "|" + string( "Is_Not_Alias" );
   if( state & c_modifier_Is_SQL_Persistence )
      state_names += "|" + string( "Is_SQL_Persistence" );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_Class::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Class::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      impl_Access_Permission( "" );
   else if( ( field == c_field_id_Change_Permission ) || ( field == c_field_name_Change_Permission ) )
      impl_Change_Permission( "" );
   else if( ( field == c_field_id_Create_Permission ) || ( field == c_field_name_Create_Permission ) )
      impl_Create_Permission( "" );
   else if( ( field == c_field_id_Created_List ) || ( field == c_field_name_Created_List ) )
      impl_Created_List( "" );
   else if( ( field == c_field_id_Created_View ) || ( field == c_field_name_Created_View ) )
      impl_Created_View( "" );
   else if( ( field == c_field_id_Destroy_Permission ) || ( field == c_field_name_Destroy_Permission ) )
      impl_Destroy_Permission( "" );
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      impl_Model( "" );
   else if( ( field == c_field_id_Quick_Link_Field ) || ( field == c_field_name_Quick_Link_Field ) )
      impl_Quick_Link_Field( "" );
   else if( ( field == c_field_id_Source_Class ) || ( field == c_field_name_Source_Class ) )
      impl_Source_Class( "" );
   else if( ( field == c_field_id_Source_Model ) || ( field == c_field_name_Source_Model ) )
      impl_Source_Model( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Class::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      v_Access_Permission = value;
   else if( ( field == c_field_id_Change_Permission ) || ( field == c_field_name_Change_Permission ) )
      v_Change_Permission = value;
   else if( ( field == c_field_id_Create_Permission ) || ( field == c_field_name_Create_Permission ) )
      v_Create_Permission = value;
   else if( ( field == c_field_id_Created_List ) || ( field == c_field_name_Created_List ) )
      v_Created_List = value;
   else if( ( field == c_field_id_Created_View ) || ( field == c_field_name_Created_View ) )
      v_Created_View = value;
   else if( ( field == c_field_id_Destroy_Permission ) || ( field == c_field_name_Destroy_Permission ) )
      v_Destroy_Permission = value;
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      v_Model = value;
   else if( ( field == c_field_id_Quick_Link_Field ) || ( field == c_field_name_Quick_Link_Field ) )
      v_Quick_Link_Field = value;
   else if( ( field == c_field_id_Source_Class ) || ( field == c_field_name_Source_Class ) )
      v_Source_Class = value;
   else if( ( field == c_field_id_Source_Model ) || ( field == c_field_name_Source_Model ) )
      v_Source_Model = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Class::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      return v_Access_Permission;
   else if( ( field == c_field_id_Change_Permission ) || ( field == c_field_name_Change_Permission ) )
      return v_Change_Permission;
   else if( ( field == c_field_id_Create_Permission ) || ( field == c_field_name_Create_Permission ) )
      return v_Create_Permission;
   else if( ( field == c_field_id_Created_List ) || ( field == c_field_name_Created_List ) )
      return v_Created_List;
   else if( ( field == c_field_id_Created_View ) || ( field == c_field_name_Created_View ) )
      return v_Created_View;
   else if( ( field == c_field_id_Destroy_Permission ) || ( field == c_field_name_Destroy_Permission ) )
      return v_Destroy_Permission;
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      return v_Model;
   else if( ( field == c_field_id_Quick_Link_Field ) || ( field == c_field_name_Quick_Link_Field ) )
      return v_Quick_Link_Field;
   else if( ( field == c_field_id_Source_Class ) || ( field == c_field_name_Source_Class ) )
      return v_Source_Class;
   else if( ( field == c_field_id_Source_Model ) || ( field == c_field_name_Source_Model ) )
      return v_Source_Model;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Class::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Access_Permission, v_Access_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Change_Permission, v_Change_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Create_Permission, v_Create_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Created_List, v_Created_List ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Created_View, v_Created_View ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Destroy_Permission, v_Destroy_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Model, v_Model ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Quick_Link_Field, v_Quick_Link_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Class, v_Source_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Model, v_Source_Model ) );
}

void Meta_Class::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_Class::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Class::impl::clear( )
{
   v_Access_Restriction = g_default_Access_Restriction;
   v_Change_Restriction = g_default_Change_Restriction;
   v_Commands_File = g_default_Commands_File;
   v_Create_List = g_default_Create_List;
   v_Create_Restriction = g_default_Create_Restriction;
   v_Create_View = g_default_Create_View;
   v_Delay_Initial_Records = g_default_Delay_Initial_Records;
   v_Destroy_Restriction = g_default_Destroy_Restriction;
   v_Extra = g_default_Extra;
   v_Header_File = g_default_Header_File;
   v_Id = g_default_Id;
   v_Name = g_default_Name;
   v_Next_Field_Id = g_default_Next_Field_Id;
   v_Next_Procedure_Id = g_default_Next_Procedure_Id;
   v_Persistence_Extra = g_default_Persistence_Extra;
   v_Persistence_Type = g_default_Persistence_Type;
   v_Plural = g_default_Plural;
   v_Source_File = g_default_Source_File;
   v_Static_Instance_Key = g_default_Static_Instance_Key;
   v_Type = g_default_Type;

   v_Access_Permission = string( );
   if( cp_Access_Permission )
      p_obj->setup_foreign_key( *cp_Access_Permission, v_Access_Permission );

   v_Change_Permission = string( );
   if( cp_Change_Permission )
      p_obj->setup_foreign_key( *cp_Change_Permission, v_Change_Permission );

   v_Create_Permission = string( );
   if( cp_Create_Permission )
      p_obj->setup_foreign_key( *cp_Create_Permission, v_Create_Permission );

   v_Created_List = string( );
   if( cp_Created_List )
      p_obj->setup_foreign_key( *cp_Created_List, v_Created_List );

   v_Created_View = string( );
   if( cp_Created_View )
      p_obj->setup_foreign_key( *cp_Created_View, v_Created_View );

   v_Destroy_Permission = string( );
   if( cp_Destroy_Permission )
      p_obj->setup_foreign_key( *cp_Destroy_Permission, v_Destroy_Permission );

   v_Model = string( );
   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   v_Quick_Link_Field = string( );
   if( cp_Quick_Link_Field )
      p_obj->setup_foreign_key( *cp_Quick_Link_Field, v_Quick_Link_Field );

   v_Source_Class = string( );
   if( cp_Source_Class )
      p_obj->setup_foreign_key( *cp_Source_Class, v_Source_Class );

   v_Source_Model = string( );
   if( cp_Source_Model )
      p_obj->setup_foreign_key( *cp_Source_Model, v_Source_Model );
}

bool Meta_Class::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [(start parent_auto_int_inc)] 600037
   if( field_name == "Id" )
      return true;
   // [(finish parent_auto_int_inc)] 600037

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Class::impl::validate(
 uint64_t state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( is_null( v_Id ) && !value_will_be_provided( c_field_name_Id ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Id,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Id ) ) ) ) );

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

   if( !is_null( v_Id )
    && ( v_Id != g_default_Id
    || !value_will_be_provided( c_field_name_Id ) )
    && !g_Id_domain.is_valid( v_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Id,
       get_module_string( c_field_display_name_Id ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( v_Name != g_default_Name
    || !value_will_be_provided( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   if( !is_null( v_Next_Field_Id )
    && ( v_Next_Field_Id != g_default_Next_Field_Id
    || !value_will_be_provided( c_field_name_Next_Field_Id ) )
    && !g_Next_Field_Id_domain.is_valid( v_Next_Field_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Next_Field_Id,
       get_module_string( c_field_display_name_Next_Field_Id ) + " " + error_message ) );

   if( !is_null( v_Next_Procedure_Id )
    && ( v_Next_Procedure_Id != g_default_Next_Procedure_Id
    || !value_will_be_provided( c_field_name_Next_Procedure_Id ) )
    && !g_Next_Procedure_Id_domain.is_valid( v_Next_Procedure_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Next_Procedure_Id,
       get_module_string( c_field_display_name_Next_Procedure_Id ) + " " + error_message ) );

   if( !is_null( v_Persistence_Extra )
    && ( v_Persistence_Extra != g_default_Persistence_Extra
    || !value_will_be_provided( c_field_name_Persistence_Extra ) )
    && !g_Persistence_Extra_domain.is_valid( v_Persistence_Extra, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Persistence_Extra,
       get_module_string( c_field_display_name_Persistence_Extra ) + " " + error_message ) );

   if( !is_null( v_Plural )
    && ( v_Plural != g_default_Plural
    || !value_will_be_provided( c_field_name_Plural ) )
    && !g_Plural_domain.is_valid( v_Plural, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Plural,
       get_module_string( c_field_display_name_Plural ) + " " + error_message ) );

   if( !is_null( v_Static_Instance_Key )
    && ( v_Static_Instance_Key != g_default_Static_Instance_Key
    || !value_will_be_provided( c_field_name_Static_Instance_Key ) )
    && !g_Static_Instance_Key_domain.is_valid( v_Static_Instance_Key, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Static_Instance_Key,
       get_module_string( c_field_display_name_Static_Instance_Key ) + " " + error_message ) );

   if( !g_view_access_restrict_enum.count( v_Access_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Access_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Access_Restriction ) ) ) ) );

   if( !g_view_change_restrict_enum.count( v_Change_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Change_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Change_Restriction ) ) ) ) );

   if( !g_list_restrict_enum.count( v_Create_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Create_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Create_Restriction ) ) ) ) );

   if( !g_list_restrict_enum.count( v_Destroy_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Destroy_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Destroy_Restriction ) ) ) ) );

   if( !g_class_extra_enum.count( v_Extra ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Extra,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Extra ) ) ) ) );

   if( !g_persistence_type_enum.count( v_Persistence_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Persistence_Type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Persistence_Type ) ) ) ) );

   if( !g_class_type_enum.count( v_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Type ) ) ) ) );

   // [(start check_null_match)] 600030
   if( is_null( get_obj( ).Source_Class( ) ) && !is_null( get_obj( ).Source_Model( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Source_Class, get_string_message( GS( c_str_field_must_be_empty_match ), make_pair(
       c_str_field_must_be_empty_match_field2, get_module_string( c_field_display_name_Source_Model ) ),
       make_pair( c_str_field_must_be_empty_match_field1, get_module_string( c_field_display_name_Source_Class ) ) ) ) );
   // [(finish check_null_match)] 600030

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Class::impl::validate_set_fields(
 set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( !is_null( v_Id )
    && ( fields_set.count( c_field_id_Id ) || fields_set.count( c_field_name_Id ) )
    && !g_Id_domain.is_valid( v_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Id,
       get_module_string( c_field_display_name_Id ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( fields_set.count( c_field_id_Name ) || fields_set.count( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   if( !is_null( v_Next_Field_Id )
    && ( fields_set.count( c_field_id_Next_Field_Id ) || fields_set.count( c_field_name_Next_Field_Id ) )
    && !g_Next_Field_Id_domain.is_valid( v_Next_Field_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Next_Field_Id,
       get_module_string( c_field_display_name_Next_Field_Id ) + " " + error_message ) );

   if( !is_null( v_Next_Procedure_Id )
    && ( fields_set.count( c_field_id_Next_Procedure_Id ) || fields_set.count( c_field_name_Next_Procedure_Id ) )
    && !g_Next_Procedure_Id_domain.is_valid( v_Next_Procedure_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Next_Procedure_Id,
       get_module_string( c_field_display_name_Next_Procedure_Id ) + " " + error_message ) );

   if( !is_null( v_Persistence_Extra )
    && ( fields_set.count( c_field_id_Persistence_Extra ) || fields_set.count( c_field_name_Persistence_Extra ) )
    && !g_Persistence_Extra_domain.is_valid( v_Persistence_Extra, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Persistence_Extra,
       get_module_string( c_field_display_name_Persistence_Extra ) + " " + error_message ) );

   if( !is_null( v_Plural )
    && ( fields_set.count( c_field_id_Plural ) || fields_set.count( c_field_name_Plural ) )
    && !g_Plural_domain.is_valid( v_Plural, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Plural,
       get_module_string( c_field_display_name_Plural ) + " " + error_message ) );

   if( !is_null( v_Static_Instance_Key )
    && ( fields_set.count( c_field_id_Static_Instance_Key ) || fields_set.count( c_field_name_Static_Instance_Key ) )
    && !g_Static_Instance_Key_domain.is_valid( v_Static_Instance_Key, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Static_Instance_Key,
       get_module_string( c_field_display_name_Static_Instance_Key ) + " " + error_message ) );
}

void Meta_Class::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Access_Permission )
      p_obj->setup_foreign_key( *cp_Access_Permission, v_Access_Permission );

   if( cp_Change_Permission )
      p_obj->setup_foreign_key( *cp_Change_Permission, v_Change_Permission );

   if( cp_Create_Permission )
      p_obj->setup_foreign_key( *cp_Create_Permission, v_Create_Permission );

   if( cp_Created_List )
      p_obj->setup_foreign_key( *cp_Created_List, v_Created_List );

   if( cp_Created_View )
      p_obj->setup_foreign_key( *cp_Created_View, v_Created_View );

   if( cp_Destroy_Permission )
      p_obj->setup_foreign_key( *cp_Destroy_Permission, v_Destroy_Permission );

   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   if( cp_Quick_Link_Field )
      p_obj->setup_foreign_key( *cp_Quick_Link_Field, v_Quick_Link_Field );

   if( cp_Source_Class )
      p_obj->setup_foreign_key( *cp_Source_Class, v_Source_Class );

   if( cp_Source_Model )
      p_obj->setup_foreign_key( *cp_Source_Model, v_Source_Model );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_fetch>]
//nyi
   string id( get_obj( ).Id( ) );

   string model_id( get_obj( ).Model( ).Id( ) );

   get_obj( ).set_variable( "@Id", search_replace( id, model_id, "" ) );
   // [<finish after_fetch>]
}

void Meta_Class::impl::completed_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start completed_fetch>]
   // [<finish completed_fetch>]
}

void Meta_Class::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Class::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_Class::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_from_other_field)] 600025
   if( is_create && get_obj( ).get_clone_key( ).empty( ) && !is_null( get_obj( ).Source_Class( ) ) )
      get_obj( ).Name( get_obj( ).Source_Class( ).Name( ) );
   // [(finish field_from_other_field)] 600025

   // [(start field_from_other_field)] 600026
   if( is_create && get_obj( ).get_clone_key( ).empty( ) && !is_null( get_obj( ).Source_Class( ) ) )
      get_obj( ).Plural( get_obj( ).Source_Class( ).Plural( ) );
   // [(finish field_from_other_field)] 600026

   // [(start default_to_global)] 600028a
   if( is_create
    && get_obj( ).get_key( ).empty( )
    && get_obj( ).get_clone_key( ).empty( )
    && get_obj( ).Create_View( ) == g_default_Create_View )
      get_obj( ).Create_View( 1 );
   // [(finish default_to_global)] 600028a

   // [(start default_to_global)] 600028b
   if( is_create
    && get_obj( ).get_key( ).empty( )
    && get_obj( ).get_clone_key( ).empty( )
    && get_obj( ).Create_List( ) == g_default_Create_List )
      get_obj( ).Create_List( 1 );
   // [(finish default_to_global)] 600028b

   // [(start field_from_other_field)] 610026
   if( is_create && get_obj( ).get_clone_key( ).empty( ) && !is_null( get_obj( ).Source_Class( ) ) )
      get_obj( ).Id( get_obj( ).Source_Class( ).Id( ) );
   // [(finish field_from_other_field)] 610026

   // [(start field_from_other_field)] 620026
   if( is_create && get_obj( ).get_clone_key( ).empty( ) && !is_null( get_obj( ).Source_Class( ) ) )
      get_obj( ).Type( get_obj( ).Source_Class( ).Type( ) );
   // [(finish field_from_other_field)] 620026

   // [(start field_from_other_field)] 630026
   if( is_create && get_obj( ).get_clone_key( ).empty( ) && !is_null( get_obj( ).Source_Class( ) ) )
      get_obj( ).Extra( get_obj( ).Source_Class( ).Extra( ) );
   // [(finish field_from_other_field)] 630026

   // [(start field_from_other_field)] 640026
   if( is_create && get_obj( ).get_clone_key( ).empty( ) && !is_null( get_obj( ).Source_Class( ) ) )
      get_obj( ).Static_Instance_Key( get_obj( ).Source_Class( ).Static_Instance_Key( ) );
   // [(finish field_from_other_field)] 640026

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Class::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start child_field_change_cascade)] 600029
   if( !is_create )
   {
      string original_value( to_string(
       get_obj( ).get_original_field_value( get_obj( ).get_field_num( "Name" ) ) ) );

      if( to_string( get_obj( ).Name( ) ) != original_value
       && get_obj( ).child_Relationship_Parent( ).iterate_forwards( ) )
      {
         do
         {
            get_obj( ).child_Relationship_Parent( ).op_update( );

            string str( to_string( get_obj( ).child_Relationship_Parent( ).Name( ) ) );
            string new_str( search_replace( str, original_value, to_string( get_obj( ).Name( ) ) ) );

            if( new_str != str )
            {
               // FUTURE: To handle non-string types would need to use a "from_string" call here.
               get_obj( ).child_Relationship_Parent( ).Name( new_str );
               get_obj( ).child_Relationship_Parent( ).op_apply( );
            }
            else
               get_obj( ).child_Relationship_Parent( ).op_cancel( );

         } while( get_obj( ).child_Relationship_Parent( ).iterate_next( ) );
      }
   }
   // [(finish child_field_change_cascade)] 600029

   // [(start parent_auto_int_inc)] 600037
   if( is_create && is_null( get_obj( ).Id( ) ) )
   {
      if( is_null( get_obj( ).Model( ) ) )
         throw runtime_error( "unexpected empty Model" );

      get_obj( ).Model( ).op_update( get_obj( ).Model( ), FIELD_NAME( Meta, Model, Next_Class_Id ) );

      get_obj( ).Id( get_obj( ).Model( ).Next_Class_Id( ) );

      get_obj( ).Model( ).Next_Class_Id( auto_int_increment( get_obj( ).Model( ).Next_Class_Id( ) ) );
      get_obj( ).Model( ).op_apply( );
   }
   // [(finish parent_auto_int_inc)] 600037

   // [(start default_to_field)] 600038
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Next_Field_Id( ) == g_default_Next_Field_Id ) )
      get_obj( ).Next_Field_Id( get_obj( ).Id( ) + "F100" );
   // [(finish default_to_field)] 600038

   // [(start default_to_field)] 610038
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Next_Procedure_Id( ) == g_default_Next_Procedure_Id ) )
      get_obj( ).Next_Procedure_Id( get_obj( ).Id( ) + "P100" );
   // [(finish default_to_field)] 610038

   // [(start file_link)] 620038
   // NOTE: Empty code block for scope purposes.
   if( !get_obj( ).get_is_being_cascaded( ) )
   {
      string sep( "-" );
      string ext( ".cpp" );
      string source( get_obj( ).Model( ).Name( ) + "_" + get_obj( ).Name( ) + ext );

      bool has_source_file( exists_file( source ) );
      string name( get_obj( ).get_key( ) + sep + c_field_id_Source_File + ext + ".txt" );

      string dest( get_obj( ).get_attached_file_path( name ) );

      if( exists_file( dest, false ) )
         remove_file( dest );
      else if( has_source_file )
         create_directories_for_file_name( dest );

      if( !has_source_file )
         get_obj( ).Source_File( "" );
      else
      {
         link_file( source, dest );
         get_obj( ).Source_File( name );
      }
   }
   // [(finish file_link)] 620038

   // [(start file_link)] 630038
   // NOTE: Empty code block for scope purposes.
   if( !get_obj( ).get_is_being_cascaded( ) )
   {
      string sep( "-" );
      string ext( ".h" );
      string source( get_obj( ).Model( ).Name( ) + "_" + get_obj( ).Name( ) + ext );

      bool has_source_file( exists_file( source ) );
      string name( get_obj( ).get_key( ) + sep + c_field_id_Header_File + ext + ".txt" );

      string dest( get_obj( ).get_attached_file_path( name ) );

      if( exists_file( dest, false ) )
         remove_file( dest );
      else if( has_source_file )
         create_directories_for_file_name( dest );

      if( !has_source_file )
         get_obj( ).Header_File( "" );
      else
      {
         link_file( source, dest );
         get_obj( ).Header_File( name );
      }
   }
   // [(finish file_link)] 630038

   // [(start file_link)] 640038
   // NOTE: Empty code block for scope purposes.
   if( !get_obj( ).get_is_being_cascaded( ) )
   {
      string sep( "-" );
      string ext( ".cms" );
      string source( get_obj( ).Model( ).Name( ) + "_" + get_obj( ).Name( ) + ext );

      bool has_source_file( exists_file( source ) );
      string name( get_obj( ).get_key( ) + sep + c_field_id_Commands_File + ext + ".txt" );

      string dest( get_obj( ).get_attached_file_path( name ) );

      if( exists_file( dest, false ) )
         remove_file( dest );
      else if( has_source_file )
         create_directories_for_file_name( dest );

      if( !has_source_file )
         get_obj( ).Commands_File( "" );
      else
      {
         link_file( source, dest );
         get_obj( ).Commands_File( name );
      }
   }
   // [(finish file_link)] 640038

   // [<start for_store>]
//nyi
   if( is_create && get_obj( ).Create_View( ) )
   {
      string view_key( incremented_key_val( get_obj( ).get_key( ), 1 ) );

      get_obj( ).Created_View( ).op_create( view_key );
      get_obj( ).Created_View( ).Class( get_obj( ).get_key( ) );
      get_obj( ).Created_View( ).Model( get_obj( ).Model( ) );
      get_obj( ).Created_View( ).op_apply( );

      get_obj( ).Created_View( view_key );
   }

   if( is_create && get_obj( ).Create_List( ) )
   {
      string list_key( incremented_key_val( get_obj( ).get_key( ), 2 ) );

      get_obj( ).Created_List( ).op_create( list_key );
      get_obj( ).Created_List( ).Class( get_obj( ).get_key( ) );
      get_obj( ).Created_List( ).Model( get_obj( ).Model( ) );
      get_obj( ).Created_List( ).op_apply( );

      get_obj( ).Created_List( list_key );
   }
   // [<finish for_store>]
}

void Meta_Class::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start update_children)] 600027
   if( !is_create
    && !get_obj( ).get_is_for_peer( )
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Specification( ).iterate_forwards( ) )
   {
      do
      {
         if( !is_update_or_destroy_locked_by_own_session( get_obj( ).child_Specification( ) ) )
         {
            get_obj( ).child_Specification( ).op_update( );
            get_obj( ).child_Specification( ).op_apply( );
         }
      } while( get_obj( ).child_Specification( ).iterate_next( ) );
   }
   // [(finish update_children)] 600027

   // [(start clone_children_from_fk)] 600035
   if( is_create && !is_null( get_obj( ).Source_Class( ) ) )
   {
      if( get_obj( ).Source_Class( ).child_Field( ).iterate_forwards( ) )
      {
         int child_num = 0;
         do
         {
            string key_info( construct_key_from_int( get_obj( ).get_key( ), ++child_num ) );
            key_info += ' ';
            key_info += get_obj( ).Source_Class( ).child_Field( ).get_key( );

            get_obj( ).child_Field( ).op_create( key_info );
            get_obj( ).child_Field( ).Class( get_obj( ).get_key( ) );
            get_obj( ).child_Field( ).Source_Field( get_obj( ).Source_Class( ).child_Field( ).get_key( ) );
            get_obj( ).child_Field( ).op_apply( );

         } while( get_obj( ).Source_Class( ).child_Field( ).iterate_next( ) );
      }
   }
   // [(finish clone_children_from_fk)] 600035

   // [(start child_field_cascade)] 600036
   if( !is_create )
   {
      if( get_obj( ).has_field_changed( c_field_id_Name )
       || get_obj( ).has_field_changed( c_field_id_Plural )
       || get_obj( ).has_field_changed( c_field_id_Type )
       || get_obj( ).has_field_changed( c_field_id_Extra )
       || get_obj( ).has_field_changed( c_field_id_Static_Instance_Key ) )
      {
         if( get_obj( ).child_Class_Source( ).iterate_forwards( "", "@key", true, 0, e_sql_optimisation_unordered ) )
         {
            do
            {
               get_obj( ).child_Class_Source( ).op_update( );
               get_obj( ).child_Class_Source( ).Name( get_obj( ).Name( ) );
               get_obj( ).child_Class_Source( ).Plural( get_obj( ).Plural( ) );
               get_obj( ).child_Class_Source( ).Type( get_obj( ).Type( ) );
               get_obj( ).child_Class_Source( ).Extra( get_obj( ).Extra( ) );
               get_obj( ).child_Class_Source( ).Static_Instance_Key( get_obj( ).Static_Instance_Key( ) );
               get_obj( ).child_Class_Source( ).op_apply( );

            } while( get_obj( ).child_Class_Source( ).iterate_next( ) );
         }
      }
   }
   // [(finish child_field_cascade)] 600036

   // [(start update_children)] 610027
   if( !is_create
    && !get_obj( ).get_is_for_peer( )
    && get_obj( ).has_field_changed( c_field_id_Name )
    && get_obj( ).child_Relationship_Child( ).iterate_forwards( ) )
   {
      do
      {
         if( !is_update_or_destroy_locked_by_own_session( get_obj( ).child_Relationship_Child( ) ) )
         {
            get_obj( ).child_Relationship_Child( ).op_update( );
            get_obj( ).child_Relationship_Child( ).op_apply( );
         }
      } while( get_obj( ).child_Relationship_Child( ).iterate_next( ) );
   }
   // [(finish update_children)] 610027

   // [(start clone_children_from_fk)] 610035
   if( is_create && !is_null( get_obj( ).Source_Class( ) ) )
   {
      if( get_obj( ).Source_Class( ).child_Index( ).iterate_forwards( ) )
      {
         int child_num = 0;
         do
         {
            string key_info( construct_key_from_int( get_obj( ).get_key( ), ++child_num ) );
            key_info += ' ';
            key_info += get_obj( ).Source_Class( ).child_Index( ).get_key( );

            get_obj( ).child_Index( ).op_create( key_info );
            get_obj( ).child_Index( ).Class( get_obj( ).get_key( ) );
            get_obj( ).child_Index( ).Source_Index( get_obj( ).Source_Class( ).child_Index( ).get_key( ) );
            get_obj( ).child_Index( ).op_apply( );

         } while( get_obj( ).Source_Class( ).child_Index( ).iterate_next( ) );
      }
   }
   // [(finish clone_children_from_fk)] 610035

   // [(start clone_children_from_fk)] 620035
   if( is_create && !is_null( get_obj( ).Source_Class( ) ) )
   {
      if( get_obj( ).Source_Class( ).child_Modifier( ).iterate_forwards( ) )
      {
         int child_num = 0;
         do
         {
            string key_info( construct_key_from_int( get_obj( ).get_key( ), ++child_num ) );
            key_info += ' ';
            key_info += get_obj( ).Source_Class( ).child_Modifier( ).get_key( );

            get_obj( ).child_Modifier( ).op_create( key_info );
            get_obj( ).child_Modifier( ).Class( get_obj( ).get_key( ) );
            get_obj( ).child_Modifier( ).Source_Modifier( get_obj( ).Source_Class( ).child_Modifier( ).get_key( ) );
            get_obj( ).child_Modifier( ).op_apply( );

         } while( get_obj( ).Source_Class( ).child_Modifier( ).iterate_next( ) );
      }
   }
   // [(finish clone_children_from_fk)] 620035

   // [(start clone_children_from_fk)] 630035
   if( is_create && !is_null( get_obj( ).Source_Class( ) ) )
   {
      if( get_obj( ).Source_Class( ).child_Procedure( ).iterate_forwards( ) )
      {
         int child_num = 0;
         do
         {
            string key_info( construct_key_from_int( get_obj( ).get_key( ), ++child_num ) );
            key_info += ' ';
            key_info += get_obj( ).Source_Class( ).child_Procedure( ).get_key( );

            get_obj( ).child_Procedure( ).op_create( key_info );
            get_obj( ).child_Procedure( ).Class( get_obj( ).get_key( ) );
            get_obj( ).child_Procedure( ).Source_Procedure( get_obj( ).Source_Class( ).child_Procedure( ).get_key( ) );
            get_obj( ).child_Procedure( ).op_apply( );

         } while( get_obj( ).Source_Class( ).child_Procedure( ).iterate_next( ) );
      }
   }
   // [(finish clone_children_from_fk)] 630035

   // [<start after_store>]
//nyi
   if( is_create && !is_null( get_obj( ).Source_Class( ) ) )
   {
      map< string, string > all_classes;

      if( get_obj( ).Model( ).child_Class( ).iterate_forwards( ) )
      {
         do
         {
            all_classes.insert(
             make_pair( get_obj( ).Model( ).child_Class( ).Id( ), get_obj( ).Model( ).child_Class( ).get_key( ) ) );
         } while( get_obj( ).Model( ).child_Class( ).iterate_next( ) );
      }

      if( get_obj( ).Source_Class( ).child_Relationship_Child( ).iterate_forwards( ) )
      {
         int child_num = 0;
         do
         {
            if( all_classes.count( get_obj( ).Source_Class( ).child_Relationship_Child( ).Parent_Class( ).Id( ) ) )
            {
               string key_info( construct_key_from_int( get_obj( ).get_key( ) + "_C", ++child_num ) );
               key_info += ' ';
               key_info += get_obj( ).Source_Class( ).child_Relationship_Child( ).get_key( );

               get_obj( ).Model( ).child_Relationship( ).op_create( key_info );

               get_obj( ).Model( ).child_Relationship( ).Model( get_obj( ).Model( ) );

               get_obj( ).Model( ).child_Relationship( ).Child_Class( get_obj( ).get_key( ) );

               get_obj( ).Model( ).child_Relationship( ).Parent_Class(
                all_classes[ get_obj( ).Source_Class( ).child_Relationship_Child( ).Parent_Class( ).Id( ) ] );

               get_obj( ).Model( ).child_Relationship( ).Source_Relationship( get_obj( ).Source_Class( ).child_Relationship_Child( ) );

               get_obj( ).Model( ).child_Relationship( ).op_apply( );

               key_info = FIELD_ID( Meta, Field, Id );
               key_info += "#1 " + get_obj( ).Model( ).child_Relationship( ).Field_Id( );

               // NOTE: When initially cloned the Field in the Relationship's Child Class has its Parent Class linked to the Class from the source model
               // so locate the field (via the Id which is stored in the Relationship) and change the Parent Class to link to the correct model here.
               if( get_obj( ).Model( ).child_Relationship( ).Child_Class( ).child_Field( ).iterate_forwards( key_info, true, 1, e_sql_optimisation_unordered ) )
               {
                  get_obj( ).Model( ).child_Relationship( ).Child_Class( ).child_Field( ).op_update( );
                  get_obj( ).Model( ).child_Relationship( ).Child_Class( ).child_Field( ).Parent_Class( get_obj( ).Model( ).child_Relationship( ).Parent_Class( ) );
                  get_obj( ).Model( ).child_Relationship( ).Child_Class( ).child_Field( ).op_apply( );

                  get_obj( ).Model( ).child_Relationship( ).Child_Class( ).child_Field( ).iterate_stop( );
               }
            }
         } while( get_obj( ).Source_Class( ).child_Relationship_Child( ).iterate_next( ) );
      }

      if( get_obj( ).Source_Class( ).child_Relationship_Parent( ).iterate_forwards( ) )
      {
         int child_num = 0;
         do
         {
            if( all_classes.count( get_obj( ).Source_Class( ).child_Relationship_Parent( ).Child_Class( ).Id( ) ) )
            {
               string key_info( construct_key_from_int( get_obj( ).get_key( ) + "_P", ++child_num ) );
               key_info += ' ';
               key_info += get_obj( ).Source_Class( ).child_Relationship_Parent( ).get_key( );

               get_obj( ).Model( ).child_Relationship( ).op_create( key_info );

               get_obj( ).Model( ).child_Relationship( ).Model( get_obj( ).Model( ) );

               get_obj( ).Model( ).child_Relationship( ).Parent_Class( get_obj( ).get_key( ) );

               // NOTE: Need to skip any self-relationships here otherwise would be creating duplicates.
               if( get_obj( ).Source_Class( ).child_Relationship_Parent( ).Child_Class( )
                == get_obj( ).Source_Class( ).child_Relationship_Parent( ).Parent_Class( ) )
                  get_obj( ).Model( ).child_Relationship( ).op_cancel( );
               else
               {
                  get_obj( ).Model( ).child_Relationship( ).Child_Class( get_obj( ).get_key( ) );

                  get_obj( ).Model( ).child_Relationship( ).Child_Class(
                   all_classes[ get_obj( ).Source_Class( ).child_Relationship_Parent( ).Child_Class( ).Id( ) ] );

                  get_obj( ).Model( ).child_Relationship( ).Source_Relationship( get_obj( ).Source_Class( ).child_Relationship_Parent( ) );

                  get_obj( ).Model( ).child_Relationship( ).op_apply( );

                  key_info = FIELD_ID( Meta, Field, Id );
                  key_info += "#1 " + get_obj( ).Model( ).child_Relationship( ).Field_Id( );

                  // NOTE: When initially cloned the Field in the Relationship's Child Class has its Parent Class linked to the Class from the source model
                  // so locate the field (via the Id which is stored in the Relationship) and change the Parent Class to link to the correct model here.
                  if( get_obj( ).Model( ).child_Relationship( ).Child_Class( ).child_Field( ).iterate_forwards( key_info, true, 1, e_sql_optimisation_unordered ) )
                  {
                     get_obj( ).Model( ).child_Relationship( ).Child_Class( ).child_Field( ).op_update( );
                     get_obj( ).Model( ).child_Relationship( ).Child_Class( ).child_Field( ).Parent_Class( get_obj( ).Model( ).child_Relationship( ).Parent_Class( ) );
                     get_obj( ).Model( ).child_Relationship( ).Child_Class( ).child_Field( ).op_apply( );

                     get_obj( ).Model( ).child_Relationship( ).Child_Class( ).iterate_stop( );
                  }
               }
            }
         } while( get_obj( ).Source_Class( ).child_Relationship_Parent( ).iterate_next( ) );
      }
   }
   // [<finish after_store>]
}

bool Meta_Class::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Class::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start file_link)] 620038
   if( !get_obj( ).Source_File( ).empty( ) )
      remove_file( get_obj( ).get_attached_file_path( get_obj( ).Source_File( ) ) );
   // [(finish file_link)] 620038

   // [(start file_link)] 630038
   if( !get_obj( ).Header_File( ).empty( ) )
      remove_file( get_obj( ).get_attached_file_path( get_obj( ).Header_File( ) ) );
   // [(finish file_link)] 630038

   // [(start file_link)] 640038
   if( !get_obj( ).Commands_File( ).empty( ) )
      remove_file( get_obj( ).get_attached_file_path( get_obj( ).Commands_File( ) ) );
   // [(finish file_link)] 640038

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Class::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Class::impl::set_default_values( )
{
   clear( );
}

bool Meta_Class::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_Class::impl::get_required_transients( ) const
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

Meta_Class::Meta_Class( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Class::~Meta_Class( )
{
   cleanup( );
   delete p_impl;
}

int Meta_Class::Access_Restriction( ) const
{
   return p_impl->impl_Access_Restriction( );
}

void Meta_Class::Access_Restriction( int Access_Restriction )
{
   p_impl->impl_Access_Restriction( Access_Restriction );
}

int Meta_Class::Change_Restriction( ) const
{
   return p_impl->impl_Change_Restriction( );
}

void Meta_Class::Change_Restriction( int Change_Restriction )
{
   p_impl->impl_Change_Restriction( Change_Restriction );
}

const string& Meta_Class::Commands_File( ) const
{
   return p_impl->impl_Commands_File( );
}

void Meta_Class::Commands_File( const string& Commands_File )
{
   p_impl->impl_Commands_File( Commands_File );
}

bool Meta_Class::Create_List( ) const
{
   return p_impl->impl_Create_List( );
}

void Meta_Class::Create_List( bool Create_List )
{
   p_impl->impl_Create_List( Create_List );
}

int Meta_Class::Create_Restriction( ) const
{
   return p_impl->impl_Create_Restriction( );
}

void Meta_Class::Create_Restriction( int Create_Restriction )
{
   p_impl->impl_Create_Restriction( Create_Restriction );
}

bool Meta_Class::Create_View( ) const
{
   return p_impl->impl_Create_View( );
}

void Meta_Class::Create_View( bool Create_View )
{
   p_impl->impl_Create_View( Create_View );
}

bool Meta_Class::Delay_Initial_Records( ) const
{
   return p_impl->impl_Delay_Initial_Records( );
}

void Meta_Class::Delay_Initial_Records( bool Delay_Initial_Records )
{
   p_impl->impl_Delay_Initial_Records( Delay_Initial_Records );
}

int Meta_Class::Destroy_Restriction( ) const
{
   return p_impl->impl_Destroy_Restriction( );
}

void Meta_Class::Destroy_Restriction( int Destroy_Restriction )
{
   p_impl->impl_Destroy_Restriction( Destroy_Restriction );
}

int Meta_Class::Extra( ) const
{
   return p_impl->impl_Extra( );
}

void Meta_Class::Extra( int Extra )
{
   p_impl->impl_Extra( Extra );
}

const string& Meta_Class::Header_File( ) const
{
   return p_impl->impl_Header_File( );
}

void Meta_Class::Header_File( const string& Header_File )
{
   p_impl->impl_Header_File( Header_File );
}

const string& Meta_Class::Id( ) const
{
   return p_impl->impl_Id( );
}

void Meta_Class::Id( const string& Id )
{
   p_impl->impl_Id( Id );
}

const string& Meta_Class::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_Class::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

const string& Meta_Class::Next_Field_Id( ) const
{
   return p_impl->impl_Next_Field_Id( );
}

void Meta_Class::Next_Field_Id( const string& Next_Field_Id )
{
   p_impl->impl_Next_Field_Id( Next_Field_Id );
}

const string& Meta_Class::Next_Procedure_Id( ) const
{
   return p_impl->impl_Next_Procedure_Id( );
}

void Meta_Class::Next_Procedure_Id( const string& Next_Procedure_Id )
{
   p_impl->impl_Next_Procedure_Id( Next_Procedure_Id );
}

const string& Meta_Class::Persistence_Extra( ) const
{
   return p_impl->impl_Persistence_Extra( );
}

void Meta_Class::Persistence_Extra( const string& Persistence_Extra )
{
   p_impl->impl_Persistence_Extra( Persistence_Extra );
}

int Meta_Class::Persistence_Type( ) const
{
   return p_impl->impl_Persistence_Type( );
}

void Meta_Class::Persistence_Type( int Persistence_Type )
{
   p_impl->impl_Persistence_Type( Persistence_Type );
}

const string& Meta_Class::Plural( ) const
{
   return p_impl->impl_Plural( );
}

void Meta_Class::Plural( const string& Plural )
{
   p_impl->impl_Plural( Plural );
}

const string& Meta_Class::Source_File( ) const
{
   return p_impl->impl_Source_File( );
}

void Meta_Class::Source_File( const string& Source_File )
{
   p_impl->impl_Source_File( Source_File );
}

const string& Meta_Class::Static_Instance_Key( ) const
{
   return p_impl->impl_Static_Instance_Key( );
}

void Meta_Class::Static_Instance_Key( const string& Static_Instance_Key )
{
   p_impl->impl_Static_Instance_Key( Static_Instance_Key );
}

int Meta_Class::Type( ) const
{
   return p_impl->impl_Type( );
}

void Meta_Class::Type( int Type )
{
   p_impl->impl_Type( Type );
}

Meta_Permission& Meta_Class::Access_Permission( )
{
   return p_impl->impl_Access_Permission( );
}

const Meta_Permission& Meta_Class::Access_Permission( ) const
{
   return p_impl->impl_Access_Permission( );
}

void Meta_Class::Access_Permission( const string& key )
{
   p_impl->impl_Access_Permission( key );
}

Meta_Permission& Meta_Class::Change_Permission( )
{
   return p_impl->impl_Change_Permission( );
}

const Meta_Permission& Meta_Class::Change_Permission( ) const
{
   return p_impl->impl_Change_Permission( );
}

void Meta_Class::Change_Permission( const string& key )
{
   p_impl->impl_Change_Permission( key );
}

Meta_Permission& Meta_Class::Create_Permission( )
{
   return p_impl->impl_Create_Permission( );
}

const Meta_Permission& Meta_Class::Create_Permission( ) const
{
   return p_impl->impl_Create_Permission( );
}

void Meta_Class::Create_Permission( const string& key )
{
   p_impl->impl_Create_Permission( key );
}

Meta_List& Meta_Class::Created_List( )
{
   return p_impl->impl_Created_List( );
}

const Meta_List& Meta_Class::Created_List( ) const
{
   return p_impl->impl_Created_List( );
}

void Meta_Class::Created_List( const string& key )
{
   p_impl->impl_Created_List( key );
}

Meta_View& Meta_Class::Created_View( )
{
   return p_impl->impl_Created_View( );
}

const Meta_View& Meta_Class::Created_View( ) const
{
   return p_impl->impl_Created_View( );
}

void Meta_Class::Created_View( const string& key )
{
   p_impl->impl_Created_View( key );
}

Meta_Permission& Meta_Class::Destroy_Permission( )
{
   return p_impl->impl_Destroy_Permission( );
}

const Meta_Permission& Meta_Class::Destroy_Permission( ) const
{
   return p_impl->impl_Destroy_Permission( );
}

void Meta_Class::Destroy_Permission( const string& key )
{
   p_impl->impl_Destroy_Permission( key );
}

Meta_Model& Meta_Class::Model( )
{
   return p_impl->impl_Model( );
}

const Meta_Model& Meta_Class::Model( ) const
{
   return p_impl->impl_Model( );
}

void Meta_Class::Model( const string& key )
{
   p_impl->impl_Model( key );
}

Meta_Field& Meta_Class::Quick_Link_Field( )
{
   return p_impl->impl_Quick_Link_Field( );
}

const Meta_Field& Meta_Class::Quick_Link_Field( ) const
{
   return p_impl->impl_Quick_Link_Field( );
}

void Meta_Class::Quick_Link_Field( const string& key )
{
   p_impl->impl_Quick_Link_Field( key );
}

Meta_Class& Meta_Class::Source_Class( )
{
   return p_impl->impl_Source_Class( );
}

const Meta_Class& Meta_Class::Source_Class( ) const
{
   return p_impl->impl_Source_Class( );
}

void Meta_Class::Source_Class( const string& key )
{
   p_impl->impl_Source_Class( key );
}

Meta_Model& Meta_Class::Source_Model( )
{
   return p_impl->impl_Source_Model( );
}

const Meta_Model& Meta_Class::Source_Model( ) const
{
   return p_impl->impl_Source_Model( );
}

void Meta_Class::Source_Model( const string& key )
{
   p_impl->impl_Source_Model( key );
}

Meta_Relationship& Meta_Class::child_Relationship_Child( )
{
   return p_impl->impl_child_Relationship_Child( );
}

const Meta_Relationship& Meta_Class::child_Relationship_Child( ) const
{
   return p_impl->impl_child_Relationship_Child( );
}

Meta_List_Field& Meta_Class::child_List_Field_Child_Rel_Child( )
{
   return p_impl->impl_child_List_Field_Child_Rel_Child( );
}

const Meta_List_Field& Meta_Class::child_List_Field_Child_Rel_Child( ) const
{
   return p_impl->impl_child_List_Field_Child_Rel_Child( );
}

Meta_Specification& Meta_Class::child_Specification_Child_Rel_Child( )
{
   return p_impl->impl_child_Specification_Child_Rel_Child( );
}

const Meta_Specification& Meta_Class::child_Specification_Child_Rel_Child( ) const
{
   return p_impl->impl_child_Specification_Child_Rel_Child( );
}

Meta_List_Field& Meta_Class::child_List_Field_Child_Rel_Source_Parent( )
{
   return p_impl->impl_child_List_Field_Child_Rel_Source_Parent( );
}

const Meta_List_Field& Meta_Class::child_List_Field_Child_Rel_Source_Parent( ) const
{
   return p_impl->impl_child_List_Field_Child_Rel_Source_Parent( );
}

Meta_Specification& Meta_Class::child_Specification_Field( )
{
   return p_impl->impl_child_Specification_Field( );
}

const Meta_Specification& Meta_Class::child_Specification_Field( ) const
{
   return p_impl->impl_child_Specification_Field( );
}

Meta_Specification_Field_Action& Meta_Class::child_Specification_Field_Action_New_Record( )
{
   return p_impl->impl_child_Specification_Field_Action_New_Record( );
}

const Meta_Specification_Field_Action& Meta_Class::child_Specification_Field_Action_New_Record( ) const
{
   return p_impl->impl_child_Specification_Field_Action_New_Record( );
}

Meta_Specification& Meta_Class::child_Specification_Other( )
{
   return p_impl->impl_child_Specification_Other( );
}

const Meta_Specification& Meta_Class::child_Specification_Other( ) const
{
   return p_impl->impl_child_Specification_Other( );
}

Meta_Field& Meta_Class::child_Field_Parent( )
{
   return p_impl->impl_child_Field_Parent( );
}

const Meta_Field& Meta_Class::child_Field_Parent( ) const
{
   return p_impl->impl_child_Field_Parent( );
}

Meta_List_Field& Meta_Class::child_List_Field_Parent( )
{
   return p_impl->impl_child_List_Field_Parent( );
}

const Meta_List_Field& Meta_Class::child_List_Field_Parent( ) const
{
   return p_impl->impl_child_List_Field_Parent( );
}

Meta_List& Meta_Class::child_List_Parent( )
{
   return p_impl->impl_child_List_Parent( );
}

const Meta_List& Meta_Class::child_List_Parent( ) const
{
   return p_impl->impl_child_List_Parent( );
}

Meta_Relationship& Meta_Class::child_Relationship_Parent( )
{
   return p_impl->impl_child_Relationship_Parent( );
}

const Meta_Relationship& Meta_Class::child_Relationship_Parent( ) const
{
   return p_impl->impl_child_Relationship_Parent( );
}

Meta_List_Field& Meta_Class::child_List_Field_Source_Child( )
{
   return p_impl->impl_child_List_Field_Source_Child( );
}

const Meta_List_Field& Meta_Class::child_List_Field_Source_Child( ) const
{
   return p_impl->impl_child_List_Field_Source_Child( );
}

Meta_Specification& Meta_Class::child_Specification_Source_Child( )
{
   return p_impl->impl_child_Specification_Source_Child( );
}

const Meta_Specification& Meta_Class::child_Specification_Source_Child( ) const
{
   return p_impl->impl_child_Specification_Source_Child( );
}

Meta_Specification& Meta_Class::child_Specification_Source( )
{
   return p_impl->impl_child_Specification_Source( );
}

const Meta_Specification& Meta_Class::child_Specification_Source( ) const
{
   return p_impl->impl_child_Specification_Source( );
}

Meta_Specification& Meta_Class::child_Specification_Source_Field( )
{
   return p_impl->impl_child_Specification_Source_Field( );
}

const Meta_Specification& Meta_Class::child_Specification_Source_Field( ) const
{
   return p_impl->impl_child_Specification_Source_Field( );
}

Meta_List_Field& Meta_Class::child_List_Field_Source_Parent( )
{
   return p_impl->impl_child_List_Field_Source_Parent( );
}

const Meta_List_Field& Meta_Class::child_List_Field_Source_Parent( ) const
{
   return p_impl->impl_child_List_Field_Source_Parent( );
}

Meta_Specification& Meta_Class::child_Specification_Source_Parent( )
{
   return p_impl->impl_child_Specification_Source_Parent( );
}

const Meta_Specification& Meta_Class::child_Specification_Source_Parent( ) const
{
   return p_impl->impl_child_Specification_Source_Parent( );
}

Meta_View_Field& Meta_Class::child_View_Field_Source_Parent( )
{
   return p_impl->impl_child_View_Field_Source_Parent( );
}

const Meta_View_Field& Meta_Class::child_View_Field_Source_Parent( ) const
{
   return p_impl->impl_child_View_Field_Source_Parent( );
}

Meta_Class& Meta_Class::child_Class_Source( )
{
   return p_impl->impl_child_Class_Source( );
}

const Meta_Class& Meta_Class::child_Class_Source( ) const
{
   return p_impl->impl_child_Class_Source( );
}

Meta_Specification& Meta_Class::child_Specification_Test_Field( )
{
   return p_impl->impl_child_Specification_Test_Field( );
}

const Meta_Specification& Meta_Class::child_Specification_Test_Field( ) const
{
   return p_impl->impl_child_Specification_Test_Field( );
}

Meta_Specification& Meta_Class::child_Specification_Test_Parent( )
{
   return p_impl->impl_child_Specification_Test_Parent( );
}

const Meta_Specification& Meta_Class::child_Specification_Test_Parent( ) const
{
   return p_impl->impl_child_Specification_Test_Parent( );
}

Meta_Field& Meta_Class::child_Field( )
{
   return p_impl->impl_child_Field( );
}

const Meta_Field& Meta_Class::child_Field( ) const
{
   return p_impl->impl_child_Field( );
}

Meta_Index& Meta_Class::child_Index( )
{
   return p_impl->impl_child_Index( );
}

const Meta_Index& Meta_Class::child_Index( ) const
{
   return p_impl->impl_child_Index( );
}

Meta_Initial_Record& Meta_Class::child_Initial_Record( )
{
   return p_impl->impl_child_Initial_Record( );
}

const Meta_Initial_Record& Meta_Class::child_Initial_Record( ) const
{
   return p_impl->impl_child_Initial_Record( );
}

Meta_List_Field& Meta_Class::child_List_Field( )
{
   return p_impl->impl_child_List_Field( );
}

const Meta_List_Field& Meta_Class::child_List_Field( ) const
{
   return p_impl->impl_child_List_Field( );
}

Meta_List& Meta_Class::child_List( )
{
   return p_impl->impl_child_List( );
}

const Meta_List& Meta_Class::child_List( ) const
{
   return p_impl->impl_child_List( );
}

Meta_Modifier_Affect& Meta_Class::child_Modifier_Affect( )
{
   return p_impl->impl_child_Modifier_Affect( );
}

const Meta_Modifier_Affect& Meta_Class::child_Modifier_Affect( ) const
{
   return p_impl->impl_child_Modifier_Affect( );
}

Meta_Modifier& Meta_Class::child_Modifier( )
{
   return p_impl->impl_child_Modifier( );
}

const Meta_Modifier& Meta_Class::child_Modifier( ) const
{
   return p_impl->impl_child_Modifier( );
}

Meta_Package_Option& Meta_Class::child_Package_Option( )
{
   return p_impl->impl_child_Package_Option( );
}

const Meta_Package_Option& Meta_Class::child_Package_Option( ) const
{
   return p_impl->impl_child_Package_Option( );
}

Meta_Procedure& Meta_Class::child_Procedure( )
{
   return p_impl->impl_child_Procedure( );
}

const Meta_Procedure& Meta_Class::child_Procedure( ) const
{
   return p_impl->impl_child_Procedure( );
}

Meta_Specification& Meta_Class::child_Specification( )
{
   return p_impl->impl_child_Specification( );
}

const Meta_Specification& Meta_Class::child_Specification( ) const
{
   return p_impl->impl_child_Specification( );
}

Meta_View_Field& Meta_Class::child_View_Field( )
{
   return p_impl->impl_child_View_Field( );
}

const Meta_View_Field& Meta_Class::child_View_Field( ) const
{
   return p_impl->impl_child_View_Field( );
}

Meta_View& Meta_Class::child_View( )
{
   return p_impl->impl_child_View( );
}

const Meta_View& Meta_Class::child_View( ) const
{
   return p_impl->impl_child_View( );
}

void Meta_Class::Generate( )
{
   p_impl->impl_Generate( );
}

string Meta_Class::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Class::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_Class::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_Class::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_Class::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_Class::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_Class::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_Class::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_Class::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_Class::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Class::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_Class::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Class::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Class::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Class::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Class::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Class::clear( )
{
   p_impl->clear( );
}

void Meta_Class::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Class::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Class::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Class::completed_fetch( bool skip_set_original )
{
   p_impl->completed_fetch( skip_set_original );
}

void Meta_Class::at_create( )
{
   p_impl->at_create( );
}

void Meta_Class::post_init( )
{
   p_impl->post_init( );
}

void Meta_Class::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Class::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Class::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Class::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Class::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Class::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Class::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Class::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Class::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( 0 );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Access_Permission )
   {
      p_id = c_field_id_Access_Permission;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Access_Restriction )
   {
      p_id = c_field_id_Access_Restriction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Change_Permission )
   {
      p_id = c_field_id_Change_Permission;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Change_Restriction )
   {
      p_id = c_field_id_Change_Restriction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Commands_File )
   {
      p_id = c_field_id_Commands_File;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Create_List )
   {
      p_id = c_field_id_Create_List;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Create_Permission )
   {
      p_id = c_field_id_Create_Permission;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Create_Restriction )
   {
      p_id = c_field_id_Create_Restriction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Create_View )
   {
      p_id = c_field_id_Create_View;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Created_List )
   {
      p_id = c_field_id_Created_List;

      if( p_type_name )
         *p_type_name = "Meta_List";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Created_View )
   {
      p_id = c_field_id_Created_View;

      if( p_type_name )
         *p_type_name = "Meta_View";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Delay_Initial_Records )
   {
      p_id = c_field_id_Delay_Initial_Records;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Destroy_Permission )
   {
      p_id = c_field_id_Destroy_Permission;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Destroy_Restriction )
   {
      p_id = c_field_id_Destroy_Restriction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Extra )
   {
      p_id = c_field_id_Extra;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Header_File )
   {
      p_id = c_field_id_Header_File;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Id )
   {
      p_id = c_field_id_Id;

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
   else if( name == c_field_name_Next_Field_Id )
   {
      p_id = c_field_id_Next_Field_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Next_Procedure_Id )
   {
      p_id = c_field_id_Next_Procedure_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Persistence_Extra )
   {
      p_id = c_field_id_Persistence_Extra;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Persistence_Type )
   {
      p_id = c_field_id_Persistence_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Plural )
   {
      p_id = c_field_id_Plural;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Quick_Link_Field )
   {
      p_id = c_field_id_Quick_Link_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_Class )
   {
      p_id = c_field_id_Source_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_File )
   {
      p_id = c_field_id_Source_File;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_Model )
   {
      p_id = c_field_id_Source_Model;

      if( p_type_name )
         *p_type_name = "Meta_Model";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Static_Instance_Key )
   {
      p_id = c_field_id_Static_Instance_Key;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Type )
   {
      p_id = c_field_id_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_id;
}

const char* Meta_Class::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( 0 );

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Access_Permission )
   {
      p_name = c_field_name_Access_Permission;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Access_Restriction )
   {
      p_name = c_field_name_Access_Restriction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Change_Permission )
   {
      p_name = c_field_name_Change_Permission;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Change_Restriction )
   {
      p_name = c_field_name_Change_Restriction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Commands_File )
   {
      p_name = c_field_name_Commands_File;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Create_List )
   {
      p_name = c_field_name_Create_List;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Create_Permission )
   {
      p_name = c_field_name_Create_Permission;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Create_Restriction )
   {
      p_name = c_field_name_Create_Restriction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Create_View )
   {
      p_name = c_field_name_Create_View;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Created_List )
   {
      p_name = c_field_name_Created_List;

      if( p_type_name )
         *p_type_name = "Meta_List";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Created_View )
   {
      p_name = c_field_name_Created_View;

      if( p_type_name )
         *p_type_name = "Meta_View";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Delay_Initial_Records )
   {
      p_name = c_field_name_Delay_Initial_Records;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Destroy_Permission )
   {
      p_name = c_field_name_Destroy_Permission;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Destroy_Restriction )
   {
      p_name = c_field_name_Destroy_Restriction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Extra )
   {
      p_name = c_field_name_Extra;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Header_File )
   {
      p_name = c_field_name_Header_File;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Id )
   {
      p_name = c_field_name_Id;

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
   else if( id == c_field_id_Next_Field_Id )
   {
      p_name = c_field_name_Next_Field_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Next_Procedure_Id )
   {
      p_name = c_field_name_Next_Procedure_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Persistence_Extra )
   {
      p_name = c_field_name_Persistence_Extra;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Persistence_Type )
   {
      p_name = c_field_name_Persistence_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Plural )
   {
      p_name = c_field_name_Plural;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Quick_Link_Field )
   {
      p_name = c_field_name_Quick_Link_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_Class )
   {
      p_name = c_field_name_Source_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_File )
   {
      p_name = c_field_name_Source_File;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_Model )
   {
      p_name = c_field_name_Source_Model;

      if( p_type_name )
         *p_type_name = "Meta_Model";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Static_Instance_Key )
   {
      p_name = c_field_name_Static_Instance_Key;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Type )
   {
      p_name = c_field_name_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_name;
}

string& Meta_Class::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_Class::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_Class::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_Class::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_Class::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_Class::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Class::get_field_uom_symbol( const string& id_or_name ) const
{
   string uom_symbol;

   string name;
   pair< string, string > next;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_uom_symbol" );
   if( ( id_or_name == c_field_id_Access_Permission ) || ( id_or_name == c_field_name_Access_Permission ) )
   {
      name = string( c_field_display_name_Access_Permission );
      get_module_string( c_field_display_name_Access_Permission, &next );
   }
   if( ( id_or_name == c_field_id_Access_Restriction ) || ( id_or_name == c_field_name_Access_Restriction ) )
   {
      name = string( c_field_display_name_Access_Restriction );
      get_module_string( c_field_display_name_Access_Restriction, &next );
   }
   if( ( id_or_name == c_field_id_Change_Permission ) || ( id_or_name == c_field_name_Change_Permission ) )
   {
      name = string( c_field_display_name_Change_Permission );
      get_module_string( c_field_display_name_Change_Permission, &next );
   }
   if( ( id_or_name == c_field_id_Change_Restriction ) || ( id_or_name == c_field_name_Change_Restriction ) )
   {
      name = string( c_field_display_name_Change_Restriction );
      get_module_string( c_field_display_name_Change_Restriction, &next );
   }
   if( ( id_or_name == c_field_id_Commands_File ) || ( id_or_name == c_field_name_Commands_File ) )
   {
      name = string( c_field_display_name_Commands_File );
      get_module_string( c_field_display_name_Commands_File, &next );
   }
   if( ( id_or_name == c_field_id_Create_List ) || ( id_or_name == c_field_name_Create_List ) )
   {
      name = string( c_field_display_name_Create_List );
      get_module_string( c_field_display_name_Create_List, &next );
   }
   if( ( id_or_name == c_field_id_Create_Permission ) || ( id_or_name == c_field_name_Create_Permission ) )
   {
      name = string( c_field_display_name_Create_Permission );
      get_module_string( c_field_display_name_Create_Permission, &next );
   }
   if( ( id_or_name == c_field_id_Create_Restriction ) || ( id_or_name == c_field_name_Create_Restriction ) )
   {
      name = string( c_field_display_name_Create_Restriction );
      get_module_string( c_field_display_name_Create_Restriction, &next );
   }
   if( ( id_or_name == c_field_id_Create_View ) || ( id_or_name == c_field_name_Create_View ) )
   {
      name = string( c_field_display_name_Create_View );
      get_module_string( c_field_display_name_Create_View, &next );
   }
   if( ( id_or_name == c_field_id_Created_List ) || ( id_or_name == c_field_name_Created_List ) )
   {
      name = string( c_field_display_name_Created_List );
      get_module_string( c_field_display_name_Created_List, &next );
   }
   if( ( id_or_name == c_field_id_Created_View ) || ( id_or_name == c_field_name_Created_View ) )
   {
      name = string( c_field_display_name_Created_View );
      get_module_string( c_field_display_name_Created_View, &next );
   }
   if( ( id_or_name == c_field_id_Delay_Initial_Records ) || ( id_or_name == c_field_name_Delay_Initial_Records ) )
   {
      name = string( c_field_display_name_Delay_Initial_Records );
      get_module_string( c_field_display_name_Delay_Initial_Records, &next );
   }
   if( ( id_or_name == c_field_id_Destroy_Permission ) || ( id_or_name == c_field_name_Destroy_Permission ) )
   {
      name = string( c_field_display_name_Destroy_Permission );
      get_module_string( c_field_display_name_Destroy_Permission, &next );
   }
   if( ( id_or_name == c_field_id_Destroy_Restriction ) || ( id_or_name == c_field_name_Destroy_Restriction ) )
   {
      name = string( c_field_display_name_Destroy_Restriction );
      get_module_string( c_field_display_name_Destroy_Restriction, &next );
   }
   if( ( id_or_name == c_field_id_Extra ) || ( id_or_name == c_field_name_Extra ) )
   {
      name = string( c_field_display_name_Extra );
      get_module_string( c_field_display_name_Extra, &next );
   }
   if( ( id_or_name == c_field_id_Header_File ) || ( id_or_name == c_field_name_Header_File ) )
   {
      name = string( c_field_display_name_Header_File );
      get_module_string( c_field_display_name_Header_File, &next );
   }
   if( ( id_or_name == c_field_id_Id ) || ( id_or_name == c_field_name_Id ) )
   {
      name = string( c_field_display_name_Id );
      get_module_string( c_field_display_name_Id, &next );
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
   if( ( id_or_name == c_field_id_Next_Field_Id ) || ( id_or_name == c_field_name_Next_Field_Id ) )
   {
      name = string( c_field_display_name_Next_Field_Id );
      get_module_string( c_field_display_name_Next_Field_Id, &next );
   }
   if( ( id_or_name == c_field_id_Next_Procedure_Id ) || ( id_or_name == c_field_name_Next_Procedure_Id ) )
   {
      name = string( c_field_display_name_Next_Procedure_Id );
      get_module_string( c_field_display_name_Next_Procedure_Id, &next );
   }
   if( ( id_or_name == c_field_id_Persistence_Extra ) || ( id_or_name == c_field_name_Persistence_Extra ) )
   {
      name = string( c_field_display_name_Persistence_Extra );
      get_module_string( c_field_display_name_Persistence_Extra, &next );
   }
   if( ( id_or_name == c_field_id_Persistence_Type ) || ( id_or_name == c_field_name_Persistence_Type ) )
   {
      name = string( c_field_display_name_Persistence_Type );
      get_module_string( c_field_display_name_Persistence_Type, &next );
   }
   if( ( id_or_name == c_field_id_Plural ) || ( id_or_name == c_field_name_Plural ) )
   {
      name = string( c_field_display_name_Plural );
      get_module_string( c_field_display_name_Plural, &next );
   }
   if( ( id_or_name == c_field_id_Quick_Link_Field ) || ( id_or_name == c_field_name_Quick_Link_Field ) )
   {
      name = string( c_field_display_name_Quick_Link_Field );
      get_module_string( c_field_display_name_Quick_Link_Field, &next );
   }
   if( ( id_or_name == c_field_id_Source_Class ) || ( id_or_name == c_field_name_Source_Class ) )
   {
      name = string( c_field_display_name_Source_Class );
      get_module_string( c_field_display_name_Source_Class, &next );
   }
   if( ( id_or_name == c_field_id_Source_File ) || ( id_or_name == c_field_name_Source_File ) )
   {
      name = string( c_field_display_name_Source_File );
      get_module_string( c_field_display_name_Source_File, &next );
   }
   if( ( id_or_name == c_field_id_Source_Model ) || ( id_or_name == c_field_name_Source_Model ) )
   {
      name = string( c_field_display_name_Source_Model );
      get_module_string( c_field_display_name_Source_Model, &next );
   }
   if( ( id_or_name == c_field_id_Static_Instance_Key ) || ( id_or_name == c_field_name_Static_Instance_Key ) )
   {
      name = string( c_field_display_name_Static_Instance_Key );
      get_module_string( c_field_display_name_Static_Instance_Key, &next );
   }
   if( ( id_or_name == c_field_id_Type ) || ( id_or_name == c_field_name_Type ) )
   {
      name = string( c_field_display_name_Type );
      get_module_string( c_field_display_name_Type, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_Class::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Access_Permission ) || ( id_or_name == c_field_name_Access_Permission ) )
      display_name = get_module_string( c_field_display_name_Access_Permission );
   if( ( id_or_name == c_field_id_Access_Restriction ) || ( id_or_name == c_field_name_Access_Restriction ) )
      display_name = get_module_string( c_field_display_name_Access_Restriction );
   if( ( id_or_name == c_field_id_Change_Permission ) || ( id_or_name == c_field_name_Change_Permission ) )
      display_name = get_module_string( c_field_display_name_Change_Permission );
   if( ( id_or_name == c_field_id_Change_Restriction ) || ( id_or_name == c_field_name_Change_Restriction ) )
      display_name = get_module_string( c_field_display_name_Change_Restriction );
   if( ( id_or_name == c_field_id_Commands_File ) || ( id_or_name == c_field_name_Commands_File ) )
      display_name = get_module_string( c_field_display_name_Commands_File );
   if( ( id_or_name == c_field_id_Create_List ) || ( id_or_name == c_field_name_Create_List ) )
      display_name = get_module_string( c_field_display_name_Create_List );
   if( ( id_or_name == c_field_id_Create_Permission ) || ( id_or_name == c_field_name_Create_Permission ) )
      display_name = get_module_string( c_field_display_name_Create_Permission );
   if( ( id_or_name == c_field_id_Create_Restriction ) || ( id_or_name == c_field_name_Create_Restriction ) )
      display_name = get_module_string( c_field_display_name_Create_Restriction );
   if( ( id_or_name == c_field_id_Create_View ) || ( id_or_name == c_field_name_Create_View ) )
      display_name = get_module_string( c_field_display_name_Create_View );
   if( ( id_or_name == c_field_id_Created_List ) || ( id_or_name == c_field_name_Created_List ) )
      display_name = get_module_string( c_field_display_name_Created_List );
   if( ( id_or_name == c_field_id_Created_View ) || ( id_or_name == c_field_name_Created_View ) )
      display_name = get_module_string( c_field_display_name_Created_View );
   if( ( id_or_name == c_field_id_Delay_Initial_Records ) || ( id_or_name == c_field_name_Delay_Initial_Records ) )
      display_name = get_module_string( c_field_display_name_Delay_Initial_Records );
   if( ( id_or_name == c_field_id_Destroy_Permission ) || ( id_or_name == c_field_name_Destroy_Permission ) )
      display_name = get_module_string( c_field_display_name_Destroy_Permission );
   if( ( id_or_name == c_field_id_Destroy_Restriction ) || ( id_or_name == c_field_name_Destroy_Restriction ) )
      display_name = get_module_string( c_field_display_name_Destroy_Restriction );
   if( ( id_or_name == c_field_id_Extra ) || ( id_or_name == c_field_name_Extra ) )
      display_name = get_module_string( c_field_display_name_Extra );
   if( ( id_or_name == c_field_id_Header_File ) || ( id_or_name == c_field_name_Header_File ) )
      display_name = get_module_string( c_field_display_name_Header_File );
   if( ( id_or_name == c_field_id_Id ) || ( id_or_name == c_field_name_Id ) )
      display_name = get_module_string( c_field_display_name_Id );
   if( ( id_or_name == c_field_id_Model ) || ( id_or_name == c_field_name_Model ) )
      display_name = get_module_string( c_field_display_name_Model );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
      display_name = get_module_string( c_field_display_name_Name );
   if( ( id_or_name == c_field_id_Next_Field_Id ) || ( id_or_name == c_field_name_Next_Field_Id ) )
      display_name = get_module_string( c_field_display_name_Next_Field_Id );
   if( ( id_or_name == c_field_id_Next_Procedure_Id ) || ( id_or_name == c_field_name_Next_Procedure_Id ) )
      display_name = get_module_string( c_field_display_name_Next_Procedure_Id );
   if( ( id_or_name == c_field_id_Persistence_Extra ) || ( id_or_name == c_field_name_Persistence_Extra ) )
      display_name = get_module_string( c_field_display_name_Persistence_Extra );
   if( ( id_or_name == c_field_id_Persistence_Type ) || ( id_or_name == c_field_name_Persistence_Type ) )
      display_name = get_module_string( c_field_display_name_Persistence_Type );
   if( ( id_or_name == c_field_id_Plural ) || ( id_or_name == c_field_name_Plural ) )
      display_name = get_module_string( c_field_display_name_Plural );
   if( ( id_or_name == c_field_id_Quick_Link_Field ) || ( id_or_name == c_field_name_Quick_Link_Field ) )
      display_name = get_module_string( c_field_display_name_Quick_Link_Field );
   if( ( id_or_name == c_field_id_Source_Class ) || ( id_or_name == c_field_name_Source_Class ) )
      display_name = get_module_string( c_field_display_name_Source_Class );
   if( ( id_or_name == c_field_id_Source_File ) || ( id_or_name == c_field_name_Source_File ) )
      display_name = get_module_string( c_field_display_name_Source_File );
   if( ( id_or_name == c_field_id_Source_Model ) || ( id_or_name == c_field_name_Source_Model ) )
      display_name = get_module_string( c_field_display_name_Source_Model );
   if( ( id_or_name == c_field_id_Static_Instance_Key ) || ( id_or_name == c_field_name_Static_Instance_Key ) )
      display_name = get_module_string( c_field_display_name_Static_Instance_Key );
   if( ( id_or_name == c_field_id_Type ) || ( id_or_name == c_field_name_Type ) )
      display_name = get_module_string( c_field_display_name_Type );

   return display_name;
}

void Meta_Class::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Class::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Class::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Class::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Class::setup_foreign_key( Meta_Permission& o, const string& value )
{
   static_cast< Meta_Permission& >( o ).set_key( value );
}

void Meta_Class::setup_foreign_key( Meta_List& o, const string& value )
{
   static_cast< Meta_List& >( o ).set_key( value );
}

void Meta_Class::setup_foreign_key( Meta_View& o, const string& value )
{
   static_cast< Meta_View& >( o ).set_key( value );
}

void Meta_Class::setup_foreign_key( Meta_Model& o, const string& value )
{
   static_cast< Meta_Model& >( o ).set_key( value );
}

void Meta_Class::setup_foreign_key( Meta_Field& o, const string& value )
{
   static_cast< Meta_Field& >( o ).set_key( value );
}

void Meta_Class::setup_foreign_key( Meta_Class& o, const string& value )
{
   static_cast< Meta_Class& >( o ).set_key( value );
}

void Meta_Class::setup_graph_parent( Meta_Relationship& o, const string& foreign_key_field )
{
   static_cast< Meta_Relationship& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Class::setup_graph_parent( Meta_List_Field& o, const string& foreign_key_field )
{
   static_cast< Meta_List_Field& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Class::setup_graph_parent( Meta_Specification& o, const string& foreign_key_field )
{
   static_cast< Meta_Specification& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Class::setup_graph_parent( Meta_Specification_Field_Action& o, const string& foreign_key_field )
{
   static_cast< Meta_Specification_Field_Action& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Class::setup_graph_parent( Meta_Field& o, const string& foreign_key_field )
{
   static_cast< Meta_Field& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Class::setup_graph_parent( Meta_List& o, const string& foreign_key_field )
{
   static_cast< Meta_List& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Class::setup_graph_parent( Meta_View_Field& o, const string& foreign_key_field )
{
   static_cast< Meta_View_Field& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Class::setup_graph_parent( Meta_Class& o, const string& foreign_key_field )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Class::setup_graph_parent( Meta_Index& o, const string& foreign_key_field )
{
   static_cast< Meta_Index& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Class::setup_graph_parent( Meta_Initial_Record& o, const string& foreign_key_field )
{
   static_cast< Meta_Initial_Record& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Class::setup_graph_parent( Meta_Modifier_Affect& o, const string& foreign_key_field )
{
   static_cast< Meta_Modifier_Affect& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Class::setup_graph_parent( Meta_Modifier& o, const string& foreign_key_field )
{
   static_cast< Meta_Modifier& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Class::setup_graph_parent( Meta_Package_Option& o, const string& foreign_key_field )
{
   static_cast< Meta_Package_Option& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Class::setup_graph_parent( Meta_Procedure& o, const string& foreign_key_field )
{
   static_cast< Meta_Procedure& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Class::setup_graph_parent( Meta_View& o, const string& foreign_key_field )
{
   static_cast< Meta_View& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Class::setup_graph_parent(
 Meta_Permission& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Permission& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Permission& >( o ).set_key( init_value );
}

void Meta_Class::setup_graph_parent(
 Meta_List& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_List& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_List& >( o ).set_key( init_value );
}

void Meta_Class::setup_graph_parent(
 Meta_View& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_View& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_View& >( o ).set_key( init_value );
}

void Meta_Class::setup_graph_parent(
 Meta_Model& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Model& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Model& >( o ).set_key( init_value );
}

void Meta_Class::setup_graph_parent(
 Meta_Field& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Field& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Field& >( o ).set_key( init_value );
}

void Meta_Class::setup_graph_parent(
 Meta_Class& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Class& >( o ).set_key( init_value );
}

size_t Meta_Class::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Class::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Class::get_num_foreign_key_children( bool is_internal ) const
{
   size_t rc = 33;

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

class_base* Meta_Class::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   if( child_num >= 33 )
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
            next_child_field = "301310";
            p_class_base = &child_Relationship_Child( );
         }
         break;

         case 1:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302194";
            p_class_base = &child_List_Field_Child_Rel_Child( );
         }
         break;

         case 2:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301491";
            p_class_base = &child_Specification_Child_Rel_Child( );
         }
         break;

         case 3:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302197";
            p_class_base = &child_List_Field_Child_Rel_Source_Parent( );
         }
         break;

         case 4:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301492";
            p_class_base = &child_Specification_Field( );
         }
         break;

         case 5:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302300";
            p_class_base = &child_Specification_Field_Action_New_Record( );
         }
         break;

         case 6:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301421";
            p_class_base = &child_Specification_Other( );
         }
         break;

         case 7:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "300730";
            p_class_base = &child_Field_Parent( );
         }
         break;

         case 8:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302115";
            p_class_base = &child_List_Field_Parent( );
         }
         break;

         case 9:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301997";
            p_class_base = &child_List_Parent( );
         }
         break;

         case 10:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301320";
            p_class_base = &child_Relationship_Parent( );
         }
         break;

         case 11:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302175";
            p_class_base = &child_List_Field_Source_Child( );
         }
         break;

         case 12:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301489";
            p_class_base = &child_Specification_Source_Child( );
         }
         break;

         case 13:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301494";
            p_class_base = &child_Specification_Source( );
         }
         break;

         case 14:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301482";
            p_class_base = &child_Specification_Source_Field( );
         }
         break;

         case 15:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302170";
            p_class_base = &child_List_Field_Source_Parent( );
         }
         break;

         case 16:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301490";
            p_class_base = &child_Specification_Source_Parent( );
         }
         break;

         case 17:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301970";
            p_class_base = &child_View_Field_Source_Parent( );
         }
         break;

         case 18:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "300620";
            p_class_base = &child_Class_Source( );
         }
         break;

         case 19:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301493";
            p_class_base = &child_Specification_Test_Field( );
         }
         break;

         case 20:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301487";
            p_class_base = &child_Specification_Test_Parent( );
         }
         break;

         case 21:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "300700";
            p_class_base = &child_Field( );
         }
         break;

         case 22:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "300800";
            p_class_base = &child_Index( );
         }
         break;

         case 23:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302410";
            p_class_base = &child_Initial_Record( );
         }
         break;

         case 24:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302110";
            p_class_base = &child_List_Field( );
         }
         break;

         case 25:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301992";
            p_class_base = &child_List( );
         }
         break;

         case 26:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301010";
            p_class_base = &child_Modifier_Affect( );
         }
         break;

         case 27:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "300900";
            p_class_base = &child_Modifier( );
         }
         break;

         case 28:
         if( op == e_cascade_op_unlink )
         {
            next_child_field = "302830a";
            p_class_base = &child_Package_Option( );
         }
         break;

         case 29:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301100";
            p_class_base = &child_Procedure( );
         }
         break;

         case 30:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301420";
            p_class_base = &child_Specification( );
         }
         break;

         case 31:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301905";
            p_class_base = &child_View_Field( );
         }
         break;

         case 32:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301820";
            p_class_base = &child_View( );
         }
         break;
      }
   }

   return p_class_base;
}

void Meta_Class::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_Class::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_Class::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_Class::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_Class::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_Class::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_Class::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_Class::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "class";

   return get_module_string( key );
}

string Meta_Class::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_Class::get_create_instance_info( ) const
{
   return "";
}

string Meta_Class::get_update_instance_info( ) const
{
   return "";
}

string Meta_Class::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_Class::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );
   else if( procedure_id == "106410" ) // i.e. Generate
      retval = "";

   return retval;
}

bool Meta_Class::get_is_alias( ) const
{
   return false;
}

void Meta_Class::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Class::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Class::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == "_301310" ) || ( sub_context == "child_Relationship_Child" ) )
      p_class_base = &child_Relationship_Child( );
   else if( ( sub_context == "_302194" ) || ( sub_context == "child_List_Field_Child_Rel_Child" ) )
      p_class_base = &child_List_Field_Child_Rel_Child( );
   else if( ( sub_context == "_301491" ) || ( sub_context == "child_Specification_Child_Rel_Child" ) )
      p_class_base = &child_Specification_Child_Rel_Child( );
   else if( ( sub_context == "_302197" ) || ( sub_context == "child_List_Field_Child_Rel_Source_Parent" ) )
      p_class_base = &child_List_Field_Child_Rel_Source_Parent( );
   else if( ( sub_context == "_301492" ) || ( sub_context == "child_Specification_Field" ) )
      p_class_base = &child_Specification_Field( );
   else if( ( sub_context == "_302300" ) || ( sub_context == "child_Specification_Field_Action_New_Record" ) )
      p_class_base = &child_Specification_Field_Action_New_Record( );
   else if( ( sub_context == "_301421" ) || ( sub_context == "child_Specification_Other" ) )
      p_class_base = &child_Specification_Other( );
   else if( ( sub_context == "_300730" ) || ( sub_context == "child_Field_Parent" ) )
      p_class_base = &child_Field_Parent( );
   else if( ( sub_context == "_302115" ) || ( sub_context == "child_List_Field_Parent" ) )
      p_class_base = &child_List_Field_Parent( );
   else if( ( sub_context == "_301997" ) || ( sub_context == "child_List_Parent" ) )
      p_class_base = &child_List_Parent( );
   else if( ( sub_context == "_301320" ) || ( sub_context == "child_Relationship_Parent" ) )
      p_class_base = &child_Relationship_Parent( );
   else if( ( sub_context == "_302175" ) || ( sub_context == "child_List_Field_Source_Child" ) )
      p_class_base = &child_List_Field_Source_Child( );
   else if( ( sub_context == "_301489" ) || ( sub_context == "child_Specification_Source_Child" ) )
      p_class_base = &child_Specification_Source_Child( );
   else if( ( sub_context == "_301494" ) || ( sub_context == "child_Specification_Source" ) )
      p_class_base = &child_Specification_Source( );
   else if( ( sub_context == "_301482" ) || ( sub_context == "child_Specification_Source_Field" ) )
      p_class_base = &child_Specification_Source_Field( );
   else if( ( sub_context == "_302170" ) || ( sub_context == "child_List_Field_Source_Parent" ) )
      p_class_base = &child_List_Field_Source_Parent( );
   else if( ( sub_context == "_301490" ) || ( sub_context == "child_Specification_Source_Parent" ) )
      p_class_base = &child_Specification_Source_Parent( );
   else if( ( sub_context == "_301970" ) || ( sub_context == "child_View_Field_Source_Parent" ) )
      p_class_base = &child_View_Field_Source_Parent( );
   else if( ( sub_context == "_300620" ) || ( sub_context == "child_Class_Source" ) )
      p_class_base = &child_Class_Source( );
   else if( ( sub_context == "_301493" ) || ( sub_context == "child_Specification_Test_Field" ) )
      p_class_base = &child_Specification_Test_Field( );
   else if( ( sub_context == "_301487" ) || ( sub_context == "child_Specification_Test_Parent" ) )
      p_class_base = &child_Specification_Test_Parent( );
   else if( ( sub_context == "_300700" ) || ( sub_context == "child_Field" ) )
      p_class_base = &child_Field( );
   else if( ( sub_context == "_300800" ) || ( sub_context == "child_Index" ) )
      p_class_base = &child_Index( );
   else if( ( sub_context == "_302410" ) || ( sub_context == "child_Initial_Record" ) )
      p_class_base = &child_Initial_Record( );
   else if( ( sub_context == "_302110" ) || ( sub_context == "child_List_Field" ) )
      p_class_base = &child_List_Field( );
   else if( ( sub_context == "_301992" ) || ( sub_context == "child_List" ) )
      p_class_base = &child_List( );
   else if( ( sub_context == "_301010" ) || ( sub_context == "child_Modifier_Affect" ) )
      p_class_base = &child_Modifier_Affect( );
   else if( ( sub_context == "_300900" ) || ( sub_context == "child_Modifier" ) )
      p_class_base = &child_Modifier( );
   else if( ( sub_context == "_302830a" ) || ( sub_context == "child_Package_Option" ) )
      p_class_base = &child_Package_Option( );
   else if( ( sub_context == "_301100" ) || ( sub_context == "child_Procedure" ) )
      p_class_base = &child_Procedure( );
   else if( ( sub_context == "_301420" ) || ( sub_context == "child_Specification" ) )
      p_class_base = &child_Specification( );
   else if( ( sub_context == "_301905" ) || ( sub_context == "child_View_Field" ) )
      p_class_base = &child_View_Field( );
   else if( ( sub_context == "_301820" ) || ( sub_context == "child_View" ) )
      p_class_base = &child_View( );
   else if( ( sub_context == c_field_id_Access_Permission ) || ( sub_context == c_field_name_Access_Permission ) )
      p_class_base = &Access_Permission( );
   else if( ( sub_context == c_field_id_Change_Permission ) || ( sub_context == c_field_name_Change_Permission ) )
      p_class_base = &Change_Permission( );
   else if( ( sub_context == c_field_id_Create_Permission ) || ( sub_context == c_field_name_Create_Permission ) )
      p_class_base = &Create_Permission( );
   else if( ( sub_context == c_field_id_Created_List ) || ( sub_context == c_field_name_Created_List ) )
      p_class_base = &Created_List( );
   else if( ( sub_context == c_field_id_Created_View ) || ( sub_context == c_field_name_Created_View ) )
      p_class_base = &Created_View( );
   else if( ( sub_context == c_field_id_Destroy_Permission ) || ( sub_context == c_field_name_Destroy_Permission ) )
      p_class_base = &Destroy_Permission( );
   else if( ( sub_context == c_field_id_Model ) || ( sub_context == c_field_name_Model ) )
      p_class_base = &Model( );
   else if( ( sub_context == c_field_id_Quick_Link_Field ) || ( sub_context == c_field_name_Quick_Link_Field ) )
      p_class_base = &Quick_Link_Field( );
   else if( ( sub_context == c_field_id_Source_Class ) || ( sub_context == c_field_name_Source_Class ) )
      p_class_base = &Source_Class( );
   else if( ( sub_context == c_field_id_Source_Model ) || ( sub_context == c_field_name_Source_Model ) )
      p_class_base = &Source_Model( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Class::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Access_Permission" );
   names.push_back( "C_Access_Restriction" );
   names.push_back( "C_Change_Permission" );
   names.push_back( "C_Change_Restriction" );
   names.push_back( "C_Commands_File" );
   names.push_back( "C_Create_Permission" );
   names.push_back( "C_Create_Restriction" );
   names.push_back( "C_Created_List" );
   names.push_back( "C_Created_View" );
   names.push_back( "C_Delay_Initial_Records" );
   names.push_back( "C_Destroy_Permission" );
   names.push_back( "C_Destroy_Restriction" );
   names.push_back( "C_Extra" );
   names.push_back( "C_Header_File" );
   names.push_back( "C_Id" );
   names.push_back( "C_Model" );
   names.push_back( "C_Name" );
   names.push_back( "C_Next_Field_Id" );
   names.push_back( "C_Next_Procedure_Id" );
   names.push_back( "C_Persistence_Extra" );
   names.push_back( "C_Persistence_Type" );
   names.push_back( "C_Plural" );
   names.push_back( "C_Quick_Link_Field" );
   names.push_back( "C_Source_Class" );
   names.push_back( "C_Source_File" );
   names.push_back( "C_Source_Model" );
   names.push_back( "C_Static_Instance_Key" );
   names.push_back( "C_Type" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Class::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Access_Permission( ) ) ) );
   values.push_back( to_string( Access_Restriction( ) ) );
   values.push_back( sql_quote( to_string( Change_Permission( ) ) ) );
   values.push_back( to_string( Change_Restriction( ) ) );
   values.push_back( sql_quote( to_string( Commands_File( ) ) ) );
   values.push_back( sql_quote( to_string( Create_Permission( ) ) ) );
   values.push_back( to_string( Create_Restriction( ) ) );
   values.push_back( sql_quote( to_string( Created_List( ) ) ) );
   values.push_back( sql_quote( to_string( Created_View( ) ) ) );
   values.push_back( to_string( Delay_Initial_Records( ) ) );
   values.push_back( sql_quote( to_string( Destroy_Permission( ) ) ) );
   values.push_back( to_string( Destroy_Restriction( ) ) );
   values.push_back( to_string( Extra( ) ) );
   values.push_back( sql_quote( to_string( Header_File( ) ) ) );
   values.push_back( sql_quote( to_string( Id( ) ) ) );
   values.push_back( sql_quote( to_string( Model( ) ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( sql_quote( to_string( Next_Field_Id( ) ) ) );
   values.push_back( sql_quote( to_string( Next_Procedure_Id( ) ) ) );
   values.push_back( sql_quote( to_string( Persistence_Extra( ) ) ) );
   values.push_back( to_string( Persistence_Type( ) ) );
   values.push_back( sql_quote( to_string( Plural( ) ) ) );
   values.push_back( sql_quote( to_string( Quick_Link_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Source_File( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Model( ) ) ) );
   values.push_back( sql_quote( to_string( Static_Instance_Key( ) ) ) );
   values.push_back( to_string( Type( ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Class::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [(start field_from_other_field)] 600025
   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Source_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Class ) ) )
         names.insert( "Source_Class" );
   }
   // [(finish field_from_other_field)] 600025

   // [(start field_from_other_field)] 600026
   if( needs_field_value( "Plural", dependents ) )
   {
      dependents.insert( "Source_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Class ) ) )
         names.insert( "Source_Class" );
   }
   // [(finish field_from_other_field)] 600026

   // [(start field_from_other_field)] 610026
   if( needs_field_value( "Id", dependents ) )
   {
      dependents.insert( "Source_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Class ) ) )
         names.insert( "Source_Class" );
   }
   // [(finish field_from_other_field)] 610026

   // [(start field_from_other_field)] 620026
   if( needs_field_value( "Type", dependents ) )
   {
      dependents.insert( "Source_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Class ) ) )
         names.insert( "Source_Class" );
   }
   // [(finish field_from_other_field)] 620026

   // [(start field_from_other_field)] 630026
   if( needs_field_value( "Extra", dependents ) )
   {
      dependents.insert( "Source_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Class ) ) )
         names.insert( "Source_Class" );
   }
   // [(finish field_from_other_field)] 630026

   // [(start field_from_other_field)] 640026
   if( needs_field_value( "Static_Instance_Key", dependents ) )
   {
      dependents.insert( "Source_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Class ) ) )
         names.insert( "Source_Class" );
   }
   // [(finish field_from_other_field)] 640026

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Class::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start modifier_field_null)] 600032
   dependents.insert( "Source_Model" ); // (for Is_Alias modifier)

   if( ( use_transients && is_field_transient( e_field_id_Source_Model ) )
    || ( !use_transients && !is_field_transient( e_field_id_Source_Model ) ) )
      names.insert( "Source_Model" );
   // [(finish modifier_field_null)] 600032

   // [(start modifier_field_null)] 600033
   dependents.insert( "Source_Model" ); // (for Is_Not_Alias modifier)

   if( ( use_transients && is_field_transient( e_field_id_Source_Model ) )
    || ( !use_transients && !is_field_transient( e_field_id_Source_Model ) ) )
      names.insert( "Source_Model" );
   // [(finish modifier_field_null)] 600033

   // [(start modifier_field_value)] 600033a
   dependents.insert( "Persistence_Type" ); // (for Is_SQL_Persistence modifier)

   if( ( use_transients && is_field_transient( e_field_id_Persistence_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Persistence_Type ) ) )
      names.insert( "Persistence_Type" );
   // [(finish modifier_field_value)] 600033a

   // [(start protect_not_equal)] 600034
   dependents.insert( "Source_Model" );

   if( ( use_transients && is_field_transient( e_field_id_Source_Model ) )
    || ( !use_transients && !is_field_transient( e_field_id_Source_Model ) ) )
      names.insert( "Source_Model" );
   // [(finish protect_not_equal)] 600034

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Class::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Class::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_Class::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Class::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Class::static_lock_class_id( )
{
   return "106100";
}

const char* Meta_Class::static_check_class_name( )
{
   return "Class";
}

const char* Meta_Class::static_persistence_extra( )
{
   return "";
}

bool Meta_Class::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Class::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.106100" );
}

void Meta_Class::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "300633", "Access_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "106124", "Access_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "300634", "Change_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "106125", "Change_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "106110", "Commands_File", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "106123", "Create_List", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "300635", "Create_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "106126", "Create_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "106122", "Create_View", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "300632", "Created_List", "Meta_List", false, "", "" ) );
   all_field_info.push_back( field_info( "300631", "Created_View", "Meta_View", false, "", "" ) );
   all_field_info.push_back( field_info( "106121", "Delay_Initial_Records", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "300636", "Destroy_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "106127", "Destroy_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "106103", "Extra", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "106109", "Header_File", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "106104", "Id", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "300600", "Model", "Meta_Model", true, "", "" ) );
   all_field_info.push_back( field_info( "106101", "Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "106105", "Next_Field_Id", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "106106", "Next_Procedure_Id", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "106129", "Persistence_Extra", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "106128", "Persistence_Type", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "106102", "Plural", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "300630", "Quick_Link_Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "300620", "Source_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "106108", "Source_File", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "300610", "Source_Model", "Meta_Model", false, "", "" ) );
   all_field_info.push_back( field_info( "106120", "Static_Instance_Key", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "106107", "Type", "int", false, "", "" ) );
}

void Meta_Class::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Access_Permission, make_pair( "Meta.106100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Change_Permission, make_pair( "Meta.106100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Create_Permission, make_pair( "Meta.106100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Created_List, make_pair( "Meta.106100", "Meta_List" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Created_View, make_pair( "Meta.106100", "Meta_View" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Destroy_Permission, make_pair( "Meta.106100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Model, make_pair( "Meta.106100", "Meta_Model" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Quick_Link_Field, make_pair( "Meta.106100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Class, make_pair( "Meta.106100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Model, make_pair( "Meta.106100", "Meta_Model" ) ) );
}

int Meta_Class::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Class::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_Class::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Class::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "300633";
      break;

      case 2:
      p_id = "106124";
      break;

      case 3:
      p_id = "300634";
      break;

      case 4:
      p_id = "106125";
      break;

      case 5:
      p_id = "106110";
      break;

      case 6:
      p_id = "106123";
      break;

      case 7:
      p_id = "300635";
      break;

      case 8:
      p_id = "106126";
      break;

      case 9:
      p_id = "106122";
      break;

      case 10:
      p_id = "300632";
      break;

      case 11:
      p_id = "300631";
      break;

      case 12:
      p_id = "106121";
      break;

      case 13:
      p_id = "300636";
      break;

      case 14:
      p_id = "106127";
      break;

      case 15:
      p_id = "106103";
      break;

      case 16:
      p_id = "106109";
      break;

      case 17:
      p_id = "106104";
      break;

      case 18:
      p_id = "300600";
      break;

      case 19:
      p_id = "106101";
      break;

      case 20:
      p_id = "106105";
      break;

      case 21:
      p_id = "106106";
      break;

      case 22:
      p_id = "106129";
      break;

      case 23:
      p_id = "106128";
      break;

      case 24:
      p_id = "106102";
      break;

      case 25:
      p_id = "300630";
      break;

      case 26:
      p_id = "300620";
      break;

      case 27:
      p_id = "106108";
      break;

      case 28:
      p_id = "300610";
      break;

      case 29:
      p_id = "106120";
      break;

      case 30:
      p_id = "106107";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Class" );

   return p_id;
}

const char* Meta_Class::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Access_Permission";
      break;

      case 2:
      p_id = "Access_Restriction";
      break;

      case 3:
      p_id = "Change_Permission";
      break;

      case 4:
      p_id = "Change_Restriction";
      break;

      case 5:
      p_id = "Commands_File";
      break;

      case 6:
      p_id = "Create_List";
      break;

      case 7:
      p_id = "Create_Permission";
      break;

      case 8:
      p_id = "Create_Restriction";
      break;

      case 9:
      p_id = "Create_View";
      break;

      case 10:
      p_id = "Created_List";
      break;

      case 11:
      p_id = "Created_View";
      break;

      case 12:
      p_id = "Delay_Initial_Records";
      break;

      case 13:
      p_id = "Destroy_Permission";
      break;

      case 14:
      p_id = "Destroy_Restriction";
      break;

      case 15:
      p_id = "Extra";
      break;

      case 16:
      p_id = "Header_File";
      break;

      case 17:
      p_id = "Id";
      break;

      case 18:
      p_id = "Model";
      break;

      case 19:
      p_id = "Name";
      break;

      case 20:
      p_id = "Next_Field_Id";
      break;

      case 21:
      p_id = "Next_Procedure_Id";
      break;

      case 22:
      p_id = "Persistence_Extra";
      break;

      case 23:
      p_id = "Persistence_Type";
      break;

      case 24:
      p_id = "Plural";
      break;

      case 25:
      p_id = "Quick_Link_Field";
      break;

      case 26:
      p_id = "Source_Class";
      break;

      case 27:
      p_id = "Source_File";
      break;

      case 28:
      p_id = "Source_Model";
      break;

      case 29:
      p_id = "Static_Instance_Key";
      break;

      case 30:
      p_id = "Type";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Class" );

   return p_id;
}

int Meta_Class::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      rc += 1;
   else if( ( field == c_field_id_Access_Restriction ) || ( field == c_field_name_Access_Restriction ) )
      rc += 2;
   else if( ( field == c_field_id_Change_Permission ) || ( field == c_field_name_Change_Permission ) )
      rc += 3;
   else if( ( field == c_field_id_Change_Restriction ) || ( field == c_field_name_Change_Restriction ) )
      rc += 4;
   else if( ( field == c_field_id_Commands_File ) || ( field == c_field_name_Commands_File ) )
      rc += 5;
   else if( ( field == c_field_id_Create_List ) || ( field == c_field_name_Create_List ) )
      rc += 6;
   else if( ( field == c_field_id_Create_Permission ) || ( field == c_field_name_Create_Permission ) )
      rc += 7;
   else if( ( field == c_field_id_Create_Restriction ) || ( field == c_field_name_Create_Restriction ) )
      rc += 8;
   else if( ( field == c_field_id_Create_View ) || ( field == c_field_name_Create_View ) )
      rc += 9;
   else if( ( field == c_field_id_Created_List ) || ( field == c_field_name_Created_List ) )
      rc += 10;
   else if( ( field == c_field_id_Created_View ) || ( field == c_field_name_Created_View ) )
      rc += 11;
   else if( ( field == c_field_id_Delay_Initial_Records ) || ( field == c_field_name_Delay_Initial_Records ) )
      rc += 12;
   else if( ( field == c_field_id_Destroy_Permission ) || ( field == c_field_name_Destroy_Permission ) )
      rc += 13;
   else if( ( field == c_field_id_Destroy_Restriction ) || ( field == c_field_name_Destroy_Restriction ) )
      rc += 14;
   else if( ( field == c_field_id_Extra ) || ( field == c_field_name_Extra ) )
      rc += 15;
   else if( ( field == c_field_id_Header_File ) || ( field == c_field_name_Header_File ) )
      rc += 16;
   else if( ( field == c_field_id_Id ) || ( field == c_field_name_Id ) )
      rc += 17;
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      rc += 18;
   else if( ( field == c_field_id_Name ) || ( field == c_field_name_Name ) )
      rc += 19;
   else if( ( field == c_field_id_Next_Field_Id ) || ( field == c_field_name_Next_Field_Id ) )
      rc += 20;
   else if( ( field == c_field_id_Next_Procedure_Id ) || ( field == c_field_name_Next_Procedure_Id ) )
      rc += 21;
   else if( ( field == c_field_id_Persistence_Extra ) || ( field == c_field_name_Persistence_Extra ) )
      rc += 22;
   else if( ( field == c_field_id_Persistence_Type ) || ( field == c_field_name_Persistence_Type ) )
      rc += 23;
   else if( ( field == c_field_id_Plural ) || ( field == c_field_name_Plural ) )
      rc += 24;
   else if( ( field == c_field_id_Quick_Link_Field ) || ( field == c_field_name_Quick_Link_Field ) )
      rc += 25;
   else if( ( field == c_field_id_Source_Class ) || ( field == c_field_name_Source_Class ) )
      rc += 26;
   else if( ( field == c_field_id_Source_File ) || ( field == c_field_name_Source_File ) )
      rc += 27;
   else if( ( field == c_field_id_Source_Model ) || ( field == c_field_name_Source_Model ) )
      rc += 28;
   else if( ( field == c_field_id_Static_Instance_Key ) || ( field == c_field_name_Static_Instance_Key ) )
      rc += 29;
   else if( ( field == c_field_id_Type ) || ( field == c_field_name_Type ) )
      rc += 30;

   return rc - 1;
}

procedure_info_container& Meta_Class::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "106410", procedure_info( "Generate" ) ) );
   }

   return procedures;
}

string Meta_Class::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(75),"
    "C_Ver_ SMALLINT UNSIGNED NOT NULL,"
    "C_Rev_ BIGINT UNSIGNED NOT NULL,"
    "C_Sec_ BIGINT UNSIGNED NOT NULL,"
    "C_Typ_ VARCHAR(16) NOT NULL,"
    "C_Access_Permission VARCHAR(75) NOT NULL,"
    "C_Access_Restriction INTEGER NOT NULL,"
    "C_Change_Permission VARCHAR(75) NOT NULL,"
    "C_Change_Restriction INTEGER NOT NULL,"
    "C_Commands_File VARCHAR(200) NOT NULL,"
    "C_Create_Permission VARCHAR(75) NOT NULL,"
    "C_Create_Restriction INTEGER NOT NULL,"
    "C_Created_List VARCHAR(75) NOT NULL,"
    "C_Created_View VARCHAR(75) NOT NULL,"
    "C_Delay_Initial_Records INTEGER NOT NULL,"
    "C_Destroy_Permission VARCHAR(75) NOT NULL,"
    "C_Destroy_Restriction INTEGER NOT NULL,"
    "C_Extra INTEGER NOT NULL,"
    "C_Header_File VARCHAR(200) NOT NULL,"
    "C_Id VARCHAR(200) NOT NULL,"
    "C_Model VARCHAR(75) NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Next_Field_Id VARCHAR(200) NOT NULL,"
    "C_Next_Procedure_Id VARCHAR(200) NOT NULL,"
    "C_Persistence_Extra VARCHAR(200) NOT NULL,"
    "C_Persistence_Type INTEGER NOT NULL,"
    "C_Plural VARCHAR(200) NOT NULL,"
    "C_Quick_Link_Field VARCHAR(75) NOT NULL,"
    "C_Source_Class VARCHAR(75) NOT NULL,"
    "C_Source_File VARCHAR(256) NOT NULL,"
    "C_Source_Model VARCHAR(256) NOT NULL,"
    "C_Static_Instance_Key VARCHAR(256) NOT NULL,"
    "C_Type INTEGER NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Class::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Class::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "enum_view_access_restrict_0", get_enum_string_view_access_restrict( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_access_restrict_1", get_enum_string_view_access_restrict( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_access_restrict_2", get_enum_string_view_access_restrict( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_access_restrict_3", get_enum_string_view_access_restrict( 3 ) ) );
   pairs.push_back( make_pair( "enum_view_access_restrict_4", get_enum_string_view_access_restrict( 4 ) ) );

   pairs.push_back( make_pair( "enum_view_change_restrict_0", get_enum_string_view_change_restrict( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_change_restrict_1", get_enum_string_view_change_restrict( 1 ) ) );
   pairs.push_back( make_pair( "enum_view_change_restrict_2", get_enum_string_view_change_restrict( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_change_restrict_3", get_enum_string_view_change_restrict( 3 ) ) );
   pairs.push_back( make_pair( "enum_view_change_restrict_4", get_enum_string_view_change_restrict( 4 ) ) );

   pairs.push_back( make_pair( "enum_list_restrict_0", get_enum_string_list_restrict( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_restrict_1", get_enum_string_list_restrict( 1 ) ) );
   pairs.push_back( make_pair( "enum_list_restrict_2", get_enum_string_list_restrict( 2 ) ) );
   pairs.push_back( make_pair( "enum_list_restrict_3", get_enum_string_list_restrict( 3 ) ) );
   pairs.push_back( make_pair( "enum_list_restrict_4", get_enum_string_list_restrict( 4 ) ) );

   pairs.push_back( make_pair( "enum_class_extra_0", get_enum_string_class_extra( 0 ) ) );
   pairs.push_back( make_pair( "enum_class_extra_1", get_enum_string_class_extra( 1 ) ) );

   pairs.push_back( make_pair( "enum_persistence_type_0", get_enum_string_persistence_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_persistence_type_1", get_enum_string_persistence_type( 1 ) ) );
   pairs.push_back( make_pair( "enum_persistence_type_2", get_enum_string_persistence_type( 2 ) ) );
   pairs.push_back( make_pair( "enum_persistence_type_3", get_enum_string_persistence_type( 3 ) ) );

   pairs.push_back( make_pair( "enum_class_type_0", get_enum_string_class_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_class_type_1", get_enum_string_class_type( 1 ) ) );
   pairs.push_back( make_pair( "enum_class_type_4", get_enum_string_class_type( 4 ) ) );
   pairs.push_back( make_pair( "enum_class_type_2", get_enum_string_class_type( 2 ) ) );
   pairs.push_back( make_pair( "enum_class_type_3", get_enum_string_class_type( 3 ) ) );
}

void Meta_Class::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "Model,Delay_Initial_Records,Name", "string,bool,string" ) );
   pairs.push_back( make_pair( "Model,Id", "string,string" ) );
   pairs.push_back( make_pair( "Model,Name", "string,string" ) );
   pairs.push_back( make_pair( "Source_Class,Id", "string,string" ) );
   pairs.push_back( make_pair( "Source_Class,Name", "string,string" ) );
}

void Meta_Class::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "Model,Delay_Initial_Records,Name" );
   unique_indexes.push_back( "Model,Id" );
   unique_indexes.push_back( "Model,Name" );
   unique_indexes.push_back( "Source_Class,Id" );
   unique_indexes.push_back( "Source_Class,Name" );
}

void Meta_Class::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Model,C_Delay_Initial_Records,C_Name" );
   indexes.push_back( "C_Model,C_Id" );
   indexes.push_back( "C_Model,C_Name" );
   indexes.push_back( "C_Source_Class,C_Id" );
   indexes.push_back( "C_Source_Class,C_Name" );
}

void Meta_Class::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Model,C_Delay_Initial_Records,C_Name" );
   indexes.push_back( "C_Model,C_Id" );
   indexes.push_back( "C_Model,C_Name" );
   indexes.push_back( "C_Source_Class,C_Id" );
   indexes.push_back( "C_Source_Class,C_Name" );
}

void Meta_Class::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Class::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Class::static_insert_external_alias( const string& module_and_class_id, Meta_Class* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Class::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Class::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   g_view_access_restrict_enum.insert( 0 );
   g_view_access_restrict_enum.insert( 1 );
   g_view_access_restrict_enum.insert( 2 );
   g_view_access_restrict_enum.insert( 3 );
   g_view_access_restrict_enum.insert( 4 );

   g_view_change_restrict_enum.insert( 0 );
   g_view_change_restrict_enum.insert( 1 );
   g_view_change_restrict_enum.insert( 2 );
   g_view_change_restrict_enum.insert( 3 );
   g_view_change_restrict_enum.insert( 4 );

   g_list_restrict_enum.insert( 0 );
   g_list_restrict_enum.insert( 1 );
   g_list_restrict_enum.insert( 2 );
   g_list_restrict_enum.insert( 3 );
   g_list_restrict_enum.insert( 4 );

   g_class_extra_enum.insert( 0 );
   g_class_extra_enum.insert( 1 );

   g_persistence_type_enum.insert( 0 );
   g_persistence_type_enum.insert( 1 );
   g_persistence_type_enum.insert( 2 );
   g_persistence_type_enum.insert( 3 );

   g_class_type_enum.insert( 0 );
   g_class_type_enum.insert( 1 );
   g_class_type_enum.insert( 4 );
   g_class_type_enum.insert( 2 );
   g_class_type_enum.insert( 3 );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Class::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
