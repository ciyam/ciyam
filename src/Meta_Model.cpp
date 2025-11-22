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

#include "Meta_Model.h"

#include "Meta_Class.h"
#include "Meta_List.h"
#include "Meta_Module.h"
#include "Meta_Package_Option.h"
#include "Meta_Package.h"
#include "Meta_Relationship.h"
#include "Meta_Specification.h"
#include "Meta_View.h"
#include "Meta_Permission.h"
#include "Meta_Workgroup.h"

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

// [(start modifier_field_null)] 610014
#include "Meta_Package.h"
// [(finish modifier_field_null)] 610014

// [<start includes>]
//nyi
#include <sys/stat.h>

#include "Meta_Enum.h"
#include "Meta_Type.h"
#include "Meta_User.h"
#include "Meta_Field.h"
#include "Meta_Index.h"
#include "Meta_Modifier.h"
#include "Meta_Enum_Item.h"
#include "Meta_List_Type.h"
#include "Meta_View_Type.h"
#include "Meta_Procedure.h"
#include "Meta_List_Field.h"
#include "Meta_View_Field.h"
#include "Meta_Package_Type.h"
#include "Meta_Initial_Record.h"
#include "Meta_List_Field_Type.h"
#include "Meta_View_Field_Type.h"
#include "Meta_Modifier_Affect.h"
#include "Meta_Specification_Type.h"
#include "Meta_Initial_Record_Value.h"
// [<finish includes>]

using namespace std;

// [<start namespaces>]
// [<finish namespaces>]

template< > inline string to_string( const Meta_Permission& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Workgroup& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Permission& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Workgroup& c, const string& s )
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

#include "Meta_Model.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Actions = "105109";
const char* const c_field_id_Add_Packages = "105112";
const char* const c_field_id_Allow_Anonymous_Access = "105116";
const char* const c_field_id_Commands_File = "105111";
const char* const c_field_id_Created = "105113";
const char* const c_field_id_Home_Title = "105117";
const char* const c_field_id_Id = "105104";
const char* const c_field_id_Name = "105101";
const char* const c_field_id_Next_Class_Id = "105105";
const char* const c_field_id_Next_List_Id = "105107";
const char* const c_field_id_Next_Specification_Id = "105106";
const char* const c_field_id_Next_View_Id = "105108";
const char* const c_field_id_Permission = "301600";
const char* const c_field_id_Source_File = "105110";
const char* const c_field_id_Status = "105114";
const char* const c_field_id_Type = "105118";
const char* const c_field_id_Use_Package_Demo_Data = "105115";
const char* const c_field_id_Version = "105102";
const char* const c_field_id_Workgroup = "300500";
const char* const c_field_id_Year_Created = "105103";

const char* const c_field_name_Actions = "Actions";
const char* const c_field_name_Add_Packages = "Add_Packages";
const char* const c_field_name_Allow_Anonymous_Access = "Allow_Anonymous_Access";
const char* const c_field_name_Commands_File = "Commands_File";
const char* const c_field_name_Created = "Created";
const char* const c_field_name_Home_Title = "Home_Title";
const char* const c_field_name_Id = "Id";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Next_Class_Id = "Next_Class_Id";
const char* const c_field_name_Next_List_Id = "Next_List_Id";
const char* const c_field_name_Next_Specification_Id = "Next_Specification_Id";
const char* const c_field_name_Next_View_Id = "Next_View_Id";
const char* const c_field_name_Permission = "Permission";
const char* const c_field_name_Source_File = "Source_File";
const char* const c_field_name_Status = "Status";
const char* const c_field_name_Type = "Type";
const char* const c_field_name_Use_Package_Demo_Data = "Use_Package_Demo_Data";
const char* const c_field_name_Version = "Version";
const char* const c_field_name_Workgroup = "Workgroup";
const char* const c_field_name_Year_Created = "Year_Created";

const char* const c_field_display_name_Actions = "field_model_actions";
const char* const c_field_display_name_Add_Packages = "field_model_add_packages";
const char* const c_field_display_name_Allow_Anonymous_Access = "field_model_allow_anonymous_access";
const char* const c_field_display_name_Commands_File = "field_model_commands_file";
const char* const c_field_display_name_Created = "field_model_created";
const char* const c_field_display_name_Home_Title = "field_model_home_title";
const char* const c_field_display_name_Id = "field_model_id";
const char* const c_field_display_name_Name = "field_model_name";
const char* const c_field_display_name_Next_Class_Id = "field_model_next_class_id";
const char* const c_field_display_name_Next_List_Id = "field_model_next_list_id";
const char* const c_field_display_name_Next_Specification_Id = "field_model_next_specification_id";
const char* const c_field_display_name_Next_View_Id = "field_model_next_view_id";
const char* const c_field_display_name_Permission = "field_model_permission";
const char* const c_field_display_name_Source_File = "field_model_source_file";
const char* const c_field_display_name_Status = "field_model_status";
const char* const c_field_display_name_Type = "field_model_type";
const char* const c_field_display_name_Use_Package_Demo_Data = "field_model_use_package_demo_data";
const char* const c_field_display_name_Version = "field_model_version";
const char* const c_field_display_name_Workgroup = "field_model_workgroup";
const char* const c_field_display_name_Year_Created = "field_model_year_created";

const int c_num_fields = 20;

const char* const c_all_sorted_field_ids[ ] =
{
   "105101",
   "105102",
   "105103",
   "105104",
   "105105",
   "105106",
   "105107",
   "105108",
   "105109",
   "105110",
   "105111",
   "105112",
   "105113",
   "105114",
   "105115",
   "105116",
   "105117",
   "105118",
   "300500",
   "301600"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Actions",
   "Add_Packages",
   "Allow_Anonymous_Access",
   "Commands_File",
   "Created",
   "Home_Title",
   "Id",
   "Name",
   "Next_Class_Id",
   "Next_List_Id",
   "Next_Specification_Id",
   "Next_View_Id",
   "Permission",
   "Source_File",
   "Status",
   "Type",
   "Use_Package_Demo_Data",
   "Version",
   "Workgroup",
   "Year_Created"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool has_field( const string& field )
{
   return binary_search( c_all_sorted_field_ids, c_all_sorted_field_ids + c_num_fields, field.c_str( ), compare )
    || binary_search( c_all_sorted_field_names, c_all_sorted_field_names + c_num_fields, field.c_str( ), compare );
}

const int c_num_encrypted_fields = 0;

bool is_encrypted_field( const string& ) { static bool false_value( false ); return false_value; }

const int c_num_transient_fields = 3;

const char* const c_transient_sorted_field_ids[ ] =
{
   "105109",
   "105112",
   "105114"
};

const char* const c_transient_sorted_field_names[ ] =
{
   "Actions",
   "Add_Packages",
   "Status"
};

inline bool is_transient_field( const string& field )
{
   return binary_search( c_transient_sorted_field_ids,
    c_transient_sorted_field_ids + c_num_transient_fields, field.c_str( ), compare )
    || binary_search( c_transient_sorted_field_names,
    c_transient_sorted_field_names + c_num_transient_fields, field.c_str( ), compare );
}

const char* const c_procedure_id_Create_Module = "105420";
const char* const c_procedure_id_Generate = "105410";
const char* const c_procedure_id_Generate_File_Links = "105435";
const char* const c_procedure_id_Get_Acyclic_Class_List = "105440";
const char* const c_procedure_id_Remove_All_Packages = "105450";
const char* const c_procedure_id_Remove_Module = "105430";

const uint64_t c_modifier_Is_Non_Traditional = UINT64_C( 0x100 );
const uint64_t c_modifier_Is_Not_Busy = UINT64_C( 0x200 );
const uint64_t c_modifier_Standard_Package_Installed = UINT64_C( 0x400 );

domain_string_max_size< 100 > g_Home_Title_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Id_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Name_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Next_Class_Id_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Next_List_Id_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Next_Specification_Id_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Next_View_Id_domain;
domain_string_max_size< 5 > g_Version_domain;
domain_int_range< 2005, 2055 > g_Year_Created_domain;

string g_group_field_name( "Workgroup" );
string g_level_field_name;
string g_order_field_name;
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_Model* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Model* > external_aliases_lookup_container;
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
int g_default_Add_Packages = int( 2 );
bool g_default_Allow_Anonymous_Access = bool( 1 );
string g_default_Commands_File = string( );
bool g_default_Created = bool( 0 );
string g_default_Home_Title = string( );
string g_default_Id = string( );
string g_default_Name = string( );
string g_default_Next_Class_Id = string( );
string g_default_Next_List_Id = string( );
string g_default_Next_Specification_Id = string( );
string g_default_Next_View_Id = string( );
string g_default_Permission = string( );
string g_default_Source_File = string( );
string g_default_Status = string( );
bool g_default_Type = bool( 1 );
bool g_default_Use_Package_Demo_Data = bool( 0 );
string g_default_Version = string( "0.1" );
string g_default_Workgroup = string( );
int g_default_Year_Created = int( );

set< int > g_add_packages_enum;
set< bool > g_model_type_enum;

const int c_enum_add_packages_none( 0 );
const int c_enum_add_packages_Standard( 1 );
const int c_enum_add_packages_Standard_User( 2 );
const int c_enum_add_packages_Standard_User_Group( 3 );

string get_enum_string_add_packages( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for add_packages" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_add_packages_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_add_packages_Standard";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_add_packages_Standard_User";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_add_packages_Standard_User_Group";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for add_packages" );

   return get_module_string( lower( string_name ) );
}

const bool c_enum_model_type_Blockchain( 0 );
const bool c_enum_model_type_Traditional( 1 );

string get_enum_string_model_type( bool val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for model_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_model_type_Blockchain";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_model_type_Traditional";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for model_type" );

   return get_module_string( lower( string_name ) );
}

// [<start anonymous>]
//nyi

const char* const c_parent_extra_null = "@null";

const char* const c_admin_password = "@admin_password";
const char* const c_admin_user_hash = "@admin_user_hash";

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

   string::size_type pos = 0;
   string::size_type lpos = 0;

   if( id.find_first_of( ".+" ) != string::npos )
   {
      retval.erase( );

      while( ( pos = id.find_first_of( ".+", lpos ) ) != string::npos )
      {
         char c( id[ pos ] );
         string next_id( id.substr( lpos, pos - lpos ) );

         if( ids.count( next_id ) )
            retval += ids[ next_id ];
         else
            retval += next_id;

         retval += c;
         lpos = pos + 1;
      }

      if( ids.count( id.substr( lpos ) ) )
         retval += ids[ id.substr( lpos ) ];
      else
         retval += id.substr( lpos );
   }

   return retval;
}
// [<finish anonymous>]

}

registration< Meta_Model > Model_registration( get_class_registry( ), "105100" );

class Meta_Model_command_functor;

class Meta_Model_command_handler : public command_handler
{
   friend class Meta_Model_command_functor;

   public:
   Meta_Model_command_handler( ) : p_Meta_Model( 0 ) { }

   void set_Meta_Model( Meta_Model* p_new_Meta_Model ) { p_Meta_Model = p_new_Meta_Model; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Model* p_Meta_Model;

   protected:
   string retval;
};

class Meta_Model_command_functor : public command_functor
{
   public:
   Meta_Model_command_functor( Meta_Model_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Model_command_handler& cmd_handler;
};

command_functor* Meta_Model_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Model_command_functor( dynamic_cast< Meta_Model_command_handler& >( handler ) );
}

void Meta_Model_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Model_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_Model_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Model->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Model->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Model_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Model->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Model->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Model_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Model_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Actions ) || ( field_name == c_field_name_Actions ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Model->Actions( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Add_Packages ) || ( field_name == c_field_name_Add_Packages ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Model->Add_Packages( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Anonymous_Access ) || ( field_name == c_field_name_Allow_Anonymous_Access ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Model->Allow_Anonymous_Access( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Commands_File ) || ( field_name == c_field_name_Commands_File ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Model->Commands_File( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Created ) || ( field_name == c_field_name_Created ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Model->Created( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Home_Title ) || ( field_name == c_field_name_Home_Title ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Model->Home_Title( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Id ) || ( field_name == c_field_name_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Model->Id( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Model->Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Next_Class_Id ) || ( field_name == c_field_name_Next_Class_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Model->Next_Class_Id( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Next_List_Id ) || ( field_name == c_field_name_Next_List_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Model->Next_List_Id( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Next_Specification_Id ) || ( field_name == c_field_name_Next_Specification_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Model->Next_Specification_Id( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Next_View_Id ) || ( field_name == c_field_name_Next_View_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Model->Next_View_Id( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Permission ) || ( field_name == c_field_name_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_Model->Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_File ) || ( field_name == c_field_name_Source_File ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Model->Source_File( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Status ) || ( field_name == c_field_name_Status ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Model->Status( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Model->Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Package_Demo_Data ) || ( field_name == c_field_name_Use_Package_Demo_Data ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Model->Use_Package_Demo_Data( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Version ) || ( field_name == c_field_name_Version ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Model->Version( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Workgroup ) || ( field_name == c_field_name_Workgroup ) ) )
      {
         handled = true;

         string_getter< Meta_Workgroup >( cmd_handler.p_Meta_Model->Workgroup( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Year_Created ) || ( field_name == c_field_name_Year_Created ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Model->Year_Created( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Model_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Model_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_Model_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Actions ) || ( field_name == c_field_name_Actions ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, string >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Actions, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Add_Packages ) || ( field_name == c_field_name_Add_Packages ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, int >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Add_Packages, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Anonymous_Access ) || ( field_name == c_field_name_Allow_Anonymous_Access ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, bool >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Allow_Anonymous_Access, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Commands_File ) || ( field_name == c_field_name_Commands_File ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, string >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Commands_File, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Created ) || ( field_name == c_field_name_Created ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, bool >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Created, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Home_Title ) || ( field_name == c_field_name_Home_Title ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, string >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Home_Title, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Id ) || ( field_name == c_field_name_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, string >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Id, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, string >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Next_Class_Id ) || ( field_name == c_field_name_Next_Class_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, string >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Next_Class_Id, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Next_List_Id ) || ( field_name == c_field_name_Next_List_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, string >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Next_List_Id, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Next_Specification_Id ) || ( field_name == c_field_name_Next_Specification_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, string >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Next_Specification_Id, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Next_View_Id ) || ( field_name == c_field_name_Next_View_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, string >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Next_View_Id, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Permission ) || ( field_name == c_field_name_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, Meta_Permission >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_File ) || ( field_name == c_field_name_Source_File ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, string >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Source_File, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Status ) || ( field_name == c_field_name_Status ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, string >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Status, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, bool >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Package_Demo_Data ) || ( field_name == c_field_name_Use_Package_Demo_Data ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, bool >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Use_Package_Demo_Data, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Version ) || ( field_name == c_field_name_Version ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, string >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Version, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Workgroup ) || ( field_name == c_field_name_Workgroup ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, Meta_Workgroup >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Workgroup, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Year_Created ) || ( field_name == c_field_name_Year_Created ) ) )
      {
         handled = true;

         func_string_setter< Meta_Model, int >(
          *cmd_handler.p_Meta_Model, &Meta_Model::Year_Created, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Model_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Model_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_Model_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_Permission ) || ( field_name == c_field_name_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_Model->Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Workgroup ) || ( field_name == c_field_name_Workgroup ) )
         cmd_handler.retval = cmd_handler.p_Meta_Model->Workgroup( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
   else if( command == c_cmd_Meta_Model_Create_Module )
   {
      cmd_handler.p_Meta_Model->Create_Module( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Model_Generate )
   {
      cmd_handler.p_Meta_Model->Generate( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Model_Generate_File_Links )
   {
      cmd_handler.p_Meta_Model->Generate_File_Links( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Model_Get_Acyclic_Class_List )
   {
      bool Check_Initial_Data( get_parm_val_from_string< bool >( parameters, c_cmd_Meta_Model_Get_Acyclic_Class_List_Check_Initial_Data ) );
      string Acyclic_Class_Keys;
      string Acyclic_Class_Names;

      cmd_handler.p_Meta_Model->Get_Acyclic_Class_List( Acyclic_Class_Keys, Acyclic_Class_Names, Check_Initial_Data );

      cmd_handler.retval.erase( );

      append_value( cmd_handler.retval, Acyclic_Class_Keys );

      append_value( cmd_handler.retval, Acyclic_Class_Names );
   }
   else if( command == c_cmd_Meta_Model_Remove_All_Packages )
   {
      cmd_handler.p_Meta_Model->Remove_All_Packages( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Model_Remove_Module )
   {
      cmd_handler.p_Meta_Model->Remove_Module( );

      cmd_handler.retval.erase( );

   }
}

struct Meta_Model::impl : public Meta_Model_command_handler
{
   impl( Meta_Model& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Model( &o );

      add_commands( 0, Meta_Model_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Model_command_definitions ) );
   }

   Meta_Model& get_obj( ) const
   {
      return *cp_obj;
   }

   const string& impl_Actions( ) const { return lazy_fetch( p_obj ), v_Actions; }
   void impl_Actions( const string& Actions ) { sanity_check( Actions ); v_Actions = Actions; }

   int impl_Add_Packages( ) const { return lazy_fetch( p_obj ), v_Add_Packages; }
   void impl_Add_Packages( int Add_Packages ) { v_Add_Packages = Add_Packages; }

   bool impl_Allow_Anonymous_Access( ) const { return lazy_fetch( p_obj ), v_Allow_Anonymous_Access; }
   void impl_Allow_Anonymous_Access( bool Allow_Anonymous_Access ) { v_Allow_Anonymous_Access = Allow_Anonymous_Access; }

   const string& impl_Commands_File( ) const { return lazy_fetch( p_obj ), v_Commands_File; }
   void impl_Commands_File( const string& Commands_File ) { sanity_check( Commands_File ); v_Commands_File = Commands_File; }

   bool impl_Created( ) const { return lazy_fetch( p_obj ), v_Created; }
   void impl_Created( bool Created ) { v_Created = Created; }

   const string& impl_Home_Title( ) const { return lazy_fetch( p_obj ), v_Home_Title; }
   void impl_Home_Title( const string& Home_Title ) { sanity_check( Home_Title ); v_Home_Title = Home_Title; }

   const string& impl_Id( ) const { return lazy_fetch( p_obj ), v_Id; }
   void impl_Id( const string& Id ) { sanity_check( Id ); v_Id = Id; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { sanity_check( Name ); v_Name = Name; }

   const string& impl_Next_Class_Id( ) const { return lazy_fetch( p_obj ), v_Next_Class_Id; }
   void impl_Next_Class_Id( const string& Next_Class_Id ) { sanity_check( Next_Class_Id ); v_Next_Class_Id = Next_Class_Id; }

   const string& impl_Next_List_Id( ) const { return lazy_fetch( p_obj ), v_Next_List_Id; }
   void impl_Next_List_Id( const string& Next_List_Id ) { sanity_check( Next_List_Id ); v_Next_List_Id = Next_List_Id; }

   const string& impl_Next_Specification_Id( ) const { return lazy_fetch( p_obj ), v_Next_Specification_Id; }
   void impl_Next_Specification_Id( const string& Next_Specification_Id ) { sanity_check( Next_Specification_Id ); v_Next_Specification_Id = Next_Specification_Id; }

   const string& impl_Next_View_Id( ) const { return lazy_fetch( p_obj ), v_Next_View_Id; }
   void impl_Next_View_Id( const string& Next_View_Id ) { sanity_check( Next_View_Id ); v_Next_View_Id = Next_View_Id; }

   const string& impl_Source_File( ) const { return lazy_fetch( p_obj ), v_Source_File; }
   void impl_Source_File( const string& Source_File ) { sanity_check( Source_File ); v_Source_File = Source_File; }

   const string& impl_Status( ) const { return lazy_fetch( p_obj ), v_Status; }
   void impl_Status( const string& Status ) { sanity_check( Status ); v_Status = Status; }

   bool impl_Type( ) const { return lazy_fetch( p_obj ), v_Type; }
   void impl_Type( bool Type ) { v_Type = Type; }

   bool impl_Use_Package_Demo_Data( ) const { return lazy_fetch( p_obj ), v_Use_Package_Demo_Data; }
   void impl_Use_Package_Demo_Data( bool Use_Package_Demo_Data ) { v_Use_Package_Demo_Data = Use_Package_Demo_Data; }

   const string& impl_Version( ) const { return lazy_fetch( p_obj ), v_Version; }
   void impl_Version( const string& Version ) { sanity_check( Version ); v_Version = Version; }

   int impl_Year_Created( ) const { return lazy_fetch( p_obj ), v_Year_Created; }
   void impl_Year_Created( int Year_Created ) { v_Year_Created = Year_Created; }

   Meta_Permission& impl_Permission( )
   {
      if( !cp_Permission )
      {
         cp_Permission.init( );

         p_obj->setup_graph_parent( *cp_Permission, c_field_id_Permission, v_Permission );
      }
      return *cp_Permission;
   }

   const Meta_Permission& impl_Permission( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Permission )
      {
         cp_Permission.init( );

         p_obj->setup_graph_parent( *cp_Permission, c_field_id_Permission, v_Permission );
      }
      return *cp_Permission;
   }

   void impl_Permission( const string& key )
   {
      class_base_accessor cba( impl_Permission( ) );
      cba.set_key( key );
   }

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

   Meta_Class& impl_child_Class_Source( )
   {
      if( !cp_child_Class_Source )
      {
         cp_child_Class_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Class_Source, "300610" );
      }
      return *cp_child_Class_Source;
   }

   const Meta_Class& impl_child_Class_Source( ) const
   {
      if( !cp_child_Class_Source )
      {
         cp_child_Class_Source.init( );

         p_obj->setup_graph_parent( *cp_child_Class_Source, "300610" );
      }
      return *cp_child_Class_Source;
   }

   Meta_Class& impl_child_Class( )
   {
      if( !cp_child_Class )
      {
         cp_child_Class.init( );

         p_obj->setup_graph_parent( *cp_child_Class, "300600" );
      }
      return *cp_child_Class;
   }

   const Meta_Class& impl_child_Class( ) const
   {
      if( !cp_child_Class )
      {
         cp_child_Class.init( );

         p_obj->setup_graph_parent( *cp_child_Class, "300600" );
      }
      return *cp_child_Class;
   }

   Meta_List& impl_child_List( )
   {
      if( !cp_child_List )
      {
         cp_child_List.init( );

         p_obj->setup_graph_parent( *cp_child_List, "301990" );
      }
      return *cp_child_List;
   }

   const Meta_List& impl_child_List( ) const
   {
      if( !cp_child_List )
      {
         cp_child_List.init( );

         p_obj->setup_graph_parent( *cp_child_List, "301990" );
      }
      return *cp_child_List;
   }

   Meta_Module& impl_child_Module( )
   {
      if( !cp_child_Module )
      {
         cp_child_Module.init( );

         p_obj->setup_graph_parent( *cp_child_Module, "302200" );
      }
      return *cp_child_Module;
   }

   const Meta_Module& impl_child_Module( ) const
   {
      if( !cp_child_Module )
      {
         cp_child_Module.init( );

         p_obj->setup_graph_parent( *cp_child_Module, "302200" );
      }
      return *cp_child_Module;
   }

   Meta_Package_Option& impl_child_Package_Option( )
   {
      if( !cp_child_Package_Option )
      {
         cp_child_Package_Option.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option, "302825" );
      }
      return *cp_child_Package_Option;
   }

   const Meta_Package_Option& impl_child_Package_Option( ) const
   {
      if( !cp_child_Package_Option )
      {
         cp_child_Package_Option.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option, "302825" );
      }
      return *cp_child_Package_Option;
   }

   Meta_Package& impl_child_Package( )
   {
      if( !cp_child_Package )
      {
         cp_child_Package.init( );

         p_obj->setup_graph_parent( *cp_child_Package, "302800" );
      }
      return *cp_child_Package;
   }

   const Meta_Package& impl_child_Package( ) const
   {
      if( !cp_child_Package )
      {
         cp_child_Package.init( );

         p_obj->setup_graph_parent( *cp_child_Package, "302800" );
      }
      return *cp_child_Package;
   }

   Meta_Relationship& impl_child_Relationship( )
   {
      if( !cp_child_Relationship )
      {
         cp_child_Relationship.init( );

         p_obj->setup_graph_parent( *cp_child_Relationship, "301300" );
      }
      return *cp_child_Relationship;
   }

   const Meta_Relationship& impl_child_Relationship( ) const
   {
      if( !cp_child_Relationship )
      {
         cp_child_Relationship.init( );

         p_obj->setup_graph_parent( *cp_child_Relationship, "301300" );
      }
      return *cp_child_Relationship;
   }

   Meta_Specification& impl_child_Specification( )
   {
      if( !cp_child_Specification )
      {
         cp_child_Specification.init( );

         p_obj->setup_graph_parent( *cp_child_Specification, "301400" );
      }
      return *cp_child_Specification;
   }

   const Meta_Specification& impl_child_Specification( ) const
   {
      if( !cp_child_Specification )
      {
         cp_child_Specification.init( );

         p_obj->setup_graph_parent( *cp_child_Specification, "301400" );
      }
      return *cp_child_Specification;
   }

   Meta_View& impl_child_View( )
   {
      if( !cp_child_View )
      {
         cp_child_View.init( );

         p_obj->setup_graph_parent( *cp_child_View, "301800" );
      }
      return *cp_child_View;
   }

   const Meta_View& impl_child_View( ) const
   {
      if( !cp_child_View )
      {
         cp_child_View.init( );

         p_obj->setup_graph_parent( *cp_child_View, "301800" );
      }
      return *cp_child_View;
   }

   void impl_Create_Module( );

   void impl_Generate( );

   void impl_Generate_File_Links( );

   void impl_Get_Acyclic_Class_List( string& Acyclic_Class_Keys, string& Acyclic_Class_Names, bool Check_Initial_Data );

   void impl_Remove_All_Packages( );

   void impl_Remove_Module( );

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

   Meta_Model* p_obj;
   class_pointer< Meta_Model > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   string v_Actions;
   int v_Add_Packages;
   bool v_Allow_Anonymous_Access;
   string v_Commands_File;
   bool v_Created;
   string v_Home_Title;
   string v_Id;
   string v_Name;
   string v_Next_Class_Id;
   string v_Next_List_Id;
   string v_Next_Specification_Id;
   string v_Next_View_Id;
   string v_Source_File;
   string v_Status;
   bool v_Type;
   bool v_Use_Package_Demo_Data;
   string v_Version;
   int v_Year_Created;

   string v_Permission;
   mutable class_pointer< Meta_Permission > cp_Permission;

   string v_Workgroup;
   mutable class_pointer< Meta_Workgroup > cp_Workgroup;

   mutable class_pointer< Meta_Class > cp_child_Class_Source;
   mutable class_pointer< Meta_Class > cp_child_Class;
   mutable class_pointer< Meta_List > cp_child_List;
   mutable class_pointer< Meta_Module > cp_child_Module;
   mutable class_pointer< Meta_Package_Option > cp_child_Package_Option;
   mutable class_pointer< Meta_Package > cp_child_Package;
   mutable class_pointer< Meta_Relationship > cp_child_Relationship;
   mutable class_pointer< Meta_Specification > cp_child_Specification;
   mutable class_pointer< Meta_View > cp_child_View;
};

void Meta_Model::impl::impl_Create_Module( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Create_Module_impl>]
//nyi
   // NOTE: If a recovery is in progress then do nothing.
   if( storage_locked_for_admin( ) )
      return;

   try
   {
      get_obj( ).op_update( );

      exec_system( "./create_module -rdbms " + get_obj( ).Name( ) + " >/dev/null" );

      bool found = false;

      string modules_list( "modules.lst" );

      if( exists_file( modules_list ) )
      {
         ifstream inpf( modules_list.c_str( ) );

         if( !inpf )
            throw runtime_error( "unexpected error opening '" + modules_list + "' for input" );

         string next;

         while( getline( inpf, next ) )
         {
            if( next == get_obj( ).Name( ) )
            {
               found = true;
               break;
            }
         }
      }

      if( !found )
         throw runtime_error( "unable to create module (try using the 'create_module' script from a console)" );

      get_obj( ).Created( true );
      get_obj( ).op_apply( );
   }
   catch( ... )
   {
      get_obj( ).op_cancel( );
      throw;
   }
   // [<finish Create_Module_impl>]
}

void Meta_Model::impl::impl_Generate( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Generate_impl>]
//nyi
   // NOTE: If a recovery is in progress then do nothing.
   if( storage_locked_for_admin( ) )
      return;

   string model_key( "Meta_Model_" + get_obj( ).get_key( ) );

   set_system_variable( model_key, "Generating model artifacts..." ); // FUTURE: Should be a module string...

   try
   {
      output_progress_message( "Generating " + get_obj( ).Name( ) + " metadata..." );

      string vars_file_name( get_obj( ).Name( ) + ".vars.xrep" );
      string make_file_name( get_obj( ).Name( ) + ".make.vars.xrep" );
      string svars_file_name( get_obj( ).Name( ) + ".spec.vars.xrep" );
      string string_file_name( get_obj( ).Name( ) + ".txt.new" );
      string classes_file_name( get_obj( ).Name( ) + ".classes.lst" );

      set< string > old_class_names;

      if( exists_file( classes_file_name ) )
         read_file_lines( classes_file_name, old_class_names );

      ofstream outv( vars_file_name.c_str( ) );
      if( !outv )
         throw runtime_error( "unexpected error opening '" + vars_file_name + "' for output" );

      ofstream outm( make_file_name.c_str( ) );
      if( !outm )
         throw runtime_error( "unexpected error opening '" + make_file_name + "' for output" );

      ofstream outf( svars_file_name.c_str( ) );
      if( !outf )
         throw runtime_error( "unexpected error opening '" + svars_file_name + "' for output" );

      ofstream outs( string_file_name.c_str( ) );
      if( !outs )
         throw runtime_error( "unexpected error opening '" + string_file_name + "' for output" );

      ofstream outc( classes_file_name.c_str( ) );
      if( !outc )
         throw runtime_error( "unexpected error opening '" + classes_file_name + "' for output" );

      string model_perm;

      if( get_obj( ).Allow_Anonymous_Access( ) )
         model_perm = "@anon";

      if( !is_null( get_obj( ).Permission( ) ) )
      {
         if( !model_perm.empty( ) )
            model_perm += ',';

         model_perm += get_obj( ).Permission( ).Id( );
      }

      outf << "\x60{\x60$model_id\x60=\x60'" << get_obj( ).Id( ) << "\x60'\x60}\n";
      outf << "\x60{\x60$model_name\x60=\x60'" << get_obj( ).Name( ) << "\x60'\x60}\n";
      outf << "\x60{\x60$model_perm\x60=\x60'" << model_perm << "\x60'\x60}\n";

      string model_name( get_obj( ).Name( ) );

      string title( get_obj( ).Name( ) + "_" );

      if( atof( get_obj( ).Version( ).c_str( ) ) < 1.0 )
         title += "Beta";
      else
         title += get_obj( ).Version( );

      outf << "\x60{\x60$model_title\x60=\x60'" << title << "\x60'\x60}\n";
      outf << "\x60{\x60$model_version\x60=\x60'" << get_obj( ).Version( ) << "\x60'\x60}\n";
      outf << "\x60{\x60$year_created\x60=\x60'" << get_obj( ).Year_Created( ) << "\x60'\x60}\n";
      outf << "\x60{\x60}\n";

      outv << "\x60{\x60$model_name\x60=\x60'" << get_obj( ).Name( ) << "\x60'\x60}\n";
      outv << "\x60{\x60$model_title\x60=\x60'" << title << "\x60'\x60}\n";

      outs << "title \"" << search_replace( title, "_", " " ) << "\"\n";
      outs << "home_title \"" << get_obj( ).Home_Title( ) << "\"\n";
      outs << "version \"" << get_obj( ).Version( ) << "\"\n";

      outs << "field_key \"Key\"\n";
      outs << "field_row \"#\"\n";

      set< string > enum_names;

      string all_enums;
      vector< pair< string, string > > all_enum_info;

      string enum_key_info( to_string( Meta_Enum::static_get_field_id( Meta_Enum::e_field_id_Name ) ) + ' ' );

      if( get_obj( ).Workgroup( ).child_Enum( ).iterate_forwards( enum_key_info ) )
      {
         do
         {
            if( get_obj( ).Workgroup( ).child_Enum( ).child_Field( ).iterate_forwards( ) )
            {
               do
               {
                  // NOTE: Only include enums that are actually used by non-aliased class fields from this model.
                  if( is_null( get_obj( ).Workgroup( ).child_Enum( ).child_Field( ).Class( ).Source_Class( ) )
                   && get_obj( ).Workgroup( ).child_Enum( ).child_Field( ).Class( ).Model( ).Id( ) == get_obj( ).Id( ) )
                  {
                     string name( get_obj( ).Workgroup( ).child_Enum( ).Name( ) );

                     if( enum_names.count( name ) )
                        continue;

                     enum_names.insert( name );

                     if( !all_enums.empty( ) )
                        all_enums += ' ';

                     all_enums += get_mapped_id( model_name, get_obj( ).Workgroup( ).child_Enum( ).Id( ) ) + ',' + name;

                     string enum_prefix( "enum_" + lower( name ) );

                     outs << enum_prefix << " \"" << search_replace( name, "_", " " ) << "\"\n";

                     string enum_item_key_info( to_string(
                      Meta_Enum_Item::static_get_field_id( Meta_Enum_Item::e_field_id_Order ) ) + ' ' );

                     if( get_obj( ).Workgroup( ).child_Enum( ).child_Enum_Item( ).iterate_forwards( enum_item_key_info ) )
                     {
                        string next_enum_info;

                        do
                        {
                           string label( get_obj( ).Workgroup( ).child_Enum( ).child_Enum_Item( ).Label( ) );
                           outs << enum_prefix << "_" << lower( label ) << " \"" << search_replace( label, "_", " " ) << "\"\n";

                           if( !next_enum_info.empty( ) )
                              next_enum_info += ' ';

                           next_enum_info += get_obj( ).Workgroup( ).child_Enum( ).child_Enum_Item( ).Value( )
                            + '=' + get_obj( ).Workgroup( ).child_Enum( ).child_Enum_Item( ).Label( );

                           if( !get_obj( ).Workgroup( ).child_Enum( ).child_Enum_Item( ).Filter( ).empty( ) )
                              next_enum_info += '='
                               + get_obj( ).Workgroup( ).child_Enum( ).child_Enum_Item( ).Filter( );

                        } while( get_obj( ).Workgroup( ).child_Enum( ).child_Enum_Item( ).iterate_next( ) );

                        all_enum_info.push_back( make_pair( name, next_enum_info ) );
                     }

                     get_obj( ).Workgroup( ).child_Enum( ).child_Field( ).iterate_stop( );

                     break;
                  }
               } while( get_obj( ).Workgroup( ).child_Enum( ).child_Field( ).iterate_next( ) );
            }
         } while( get_obj( ).Workgroup( ).child_Enum( ).iterate_next( ) );

         if( !all_enums.empty( ) )
         {
            outf << "\x60{\x60$all_enums\x60=\x60'" << all_enums << "\x60'\x60}\n";
            outf << "\x60{\x60}\n";

            for( size_t i = 0; i < all_enum_info.size( ); i++ )
            {
               outf << "\x60{\x60$enum_" << all_enum_info[ i ].first
                << "\x60=\x60'" << all_enum_info[ i ].second << "\x60'\x60}\n";
               outf << "\x60{\x60}\n";
            }
         }
      }

      string module_cpps;
      string module_cmss;

      string user_cid;
      string user_name;
      string user_group_cid;
      string user_group_name;
      string user_other_cid;
      string user_other_name;

      string system_cid;
      string system_name;

      set< string > external_modules;

      size_t class_num = 0;

      map< size_t, string > class_ids;
      map< string, vector< string > > all_class_strings;

      string class_key_info( to_string( Meta_Class::static_get_field_id( Meta_Class::e_field_id_Name ) ) + ' ' );

      if( get_obj( ).child_Class( ).iterate_forwards( class_key_info ) )
      {
         string all_classes;

         do
         {
            vector< string > class_strings;
            class_ids.insert( make_pair( class_num++, get_obj( ).child_Class( ).Id( ) ) );

            outc << get_obj( ).child_Class( ).Name( ) << '\n';

            if( old_class_names.count( get_obj( ).child_Class( ).Name( ) ) )
               old_class_names.erase( get_obj( ).child_Class( ).Name( ) );

            if( !is_null( get_obj( ).child_Class( ).Source_Model( ) ) )
               external_modules.insert( get_obj( ).child_Class( ).Source_Model( ).Name( ) );

            module_cpps += "\\\n " + get_obj( ).Name( ) + "_" + get_obj( ).child_Class( ).Name( ) + ".cpp";
            module_cmss += "\\\n " + get_obj( ).Name( ) + "_" + get_obj( ).child_Class( ).Name( ) + ".cms";

            class_strings.push_back( "class_" + lower( get_obj( ).child_Class( ).Name( ) )
             + " \"" + search_replace( get_obj( ).child_Class( ).Name( ), "_", " " ) + "\"" );

            class_strings.push_back( "plural_" + lower( get_obj( ).child_Class( ).Name( ) )
             + " \"" + search_replace( get_obj( ).child_Class( ).Plural( ), "_", " " ) + "\"" );

            if( !all_classes.empty( ) )
               all_classes += ' ';

            all_classes += get_obj( ).child_Class( ).Name( );

            if( get_obj( ).child_Class( ).Type( ) == 1 ) // i.e. user
            {
               if( !user_cid.empty( ) )
                  throw runtime_error( "duplicate \"user\" type class found in Model::Generate" );

               user_cid = get_obj( ).child_Class( ).Id( );
               user_name = get_obj( ).child_Class( ).Name( );
            }
            else if( get_obj( ).child_Class( ).Type( ) == 2 ) // i.e. user_group
            {
               if( !user_group_cid.empty( ) )
                  throw runtime_error( "duplicate \"user_group\" type class found in Model::Generate" );

               user_group_cid = get_obj( ).child_Class( ).Id( );
               user_group_name = get_obj( ).child_Class( ).Name( );
            }
            else if( get_obj( ).child_Class( ).Type( ) == 3 ) // i.e. user_other
            {
               if( !user_other_cid.empty( ) )
                  throw runtime_error( "duplicate \"user_other\" type class found in Model::Generate" );

               user_other_cid = get_obj( ).child_Class( ).Id( );
               user_other_name = get_obj( ).child_Class( ).Name( );
            }
            else if( get_obj( ).child_Class( ).Type( ) == 4 ) // i.e. system
            {
               if( !system_cid.empty( ) )
                  throw runtime_error( "duplicate \"system\" type class found in Model::Generate" );

               system_cid = get_obj( ).child_Class( ).Id( );
               system_name = get_obj( ).child_Class( ).Name( );
            }

            string field_key_info( to_string( Meta_Field::static_get_field_id( Meta_Field::e_field_id_Name ) ) + ' ' );

            if( is_null( get_obj( ).child_Class( ).Source_Class( ) )
             && get_obj( ).child_Class( ).child_Field( ).iterate_forwards( field_key_info ) )
            {
               do
               {
                  class_strings.push_back(
                   "field_" + lower( get_obj( ).child_Class( ).Name( ) )
                   + "_" + lower( get_obj( ).child_Class( ).child_Field( ).Name( ) )
                   + " \"" + search_replace( get_obj( ).child_Class( ).child_Field( ).Name( ), "_", " " ) + "\"" );

                  if( !get_obj( ).child_Class( ).child_Field( ).UOM_Name( ).empty( ) )
                     class_strings.push_back(
                      "field_" + lower( get_obj( ).child_Class( ).Name( ) )
                      + "_" + lower( get_obj( ).child_Class( ).child_Field( ).Name( ) )
                      + "_(" + lower( get_obj( ).child_Class( ).child_Field( ).UOM_Name( ) )
                      + ") \"" + escaped_string( get_obj( ).child_Class( ).child_Field( ).UOM_Symbol( ), "\"" ) + "\"" );

               } while( get_obj( ).child_Class( ).child_Field( ).iterate_next( ) );
            }

            string procedure_key_info( to_string( Meta_Procedure::static_get_field_id( Meta_Procedure::e_field_id_Name ) ) + ' ' );

            if( is_null( get_obj( ).child_Class( ).Source_Class( ) )
             && get_obj( ).child_Class( ).child_Procedure( ).iterate_forwards( procedure_key_info ) )
            {
               do
               {
                  class_strings.push_back(
                   "procedure_" + lower( get_obj( ).child_Class( ).Name( ) )
                   + "_" + lower( get_obj( ).child_Class( ).child_Procedure( ).Name( ) )
                   + " \"" + search_replace( get_obj( ).child_Class( ).child_Procedure( ).Name( ), "_", " " ) + "\"" );

                  class_strings.push_back(
                   "procedure_" + get_obj( ).child_Class( ).child_Procedure( ).Id( )
                   + " \"" + search_replace( get_obj( ).child_Class( ).child_Procedure( ).Name( ), "_", " " ) + "\"" );
               } while( get_obj( ).child_Class( ).child_Procedure( ).iterate_next( ) );
            }

            all_class_strings[ get_obj( ).child_Class( ).Id( ) ] = class_strings;

         } while( get_obj( ).child_Class( ).iterate_next( ) );

         outf << "\x60{\x60$all_classes\x60=\x60'" << all_classes << "\x60'\x60}\n";
      }

      if( !external_modules.empty( ) )
      {
         outf << "\x60{\x60$all_externals\x60=\x60'";

         for( set< string >::iterator i = external_modules.begin( ); i != external_modules.end( ); ++i )
         {
            if( i != external_modules.begin( ) )
               outf << ' ';
            outf << *i;
         }

         outf << "\x60'\x60}\n";
      }

      outm << "\x60{\x60$" << get_obj( ).Name( ) << "_dylk\x60=\x60'ciyam_base";

      for( set< string >::iterator i = external_modules.begin( ); i != external_modules.end( ); ++i )
         outm << " " << *i;

      outm << "\x60'\x60}\n";

      outm << "\x60{\x60$" << get_obj( ).Name( ) << "_cpps\x60=\x60'"
       << get_obj( ).Name( ) << ".cpp" << module_cpps << "\x60'\x60}\n";

      outm << "\x60{\x60$" << get_obj( ).Name( ) << "_cmss\x60=\x60'"
       << get_obj( ).Name( ) << ".cms" << module_cmss << "\x60'\x60}\n";

      if( get_obj( ).child_Specification( ).iterate_forwards( ) )
      {
         do
         {
            // NOTE: As there could be one or more derived Specification classes a review
            // is being used to ensure that a dynamic instance will be used when required.
            get_obj( ).child_Specification( ).begin_review( get_obj( ).child_Specification( ).get_key( ) );

            if( !get_obj( ).child_Specification( ).Vars( ).empty( )
             && get_obj( ).child_Specification( ).Specification_Type( ).Is_Required_For_UI_Gen( ) )
            {
               string specification_name( get_obj( ).child_Specification( ).Class( ).Name( ) );
               specification_name += "_" + get_obj( ).child_Specification( ).Specification_Type( ).Specification_Object( );

               if( get_obj( ).child_Specification( ).Specification_Type( ).Specification_Object( ) == "sys_info" )
                  specification_name = "sys_info";

               if( get_obj( ).child_Specification( ).Specification_Type( ).Specification_Object( ) == "user_info" )
                  specification_name = "user_info";

               if( get_obj( ).child_Specification( ).Specification_Type( ).Specification_Object( ) == "qlink_info" )
                  specification_name = "qlink_info";

               if( get_obj( ).child_Specification( ).Specification_Type( ).Specification_Object( ) == "home_message_stats" )
                  specification_name = "home_message";

               if( get_obj( ).child_Specification( ).Specification_Type( ).Specification_Object( ) == "home_message_top_n" )
                  specification_name = "home_message";

               string::size_type pos = 0;
               string specification_details;

               string strings( get_obj( ).child_Specification( ).All_Strings( ) );

               if( !strings.empty( ) )
               {
                  while( true )
                  {
                     pos = strings.find( '\n' );
                     string next( strings.substr( 0, pos ) );

                     string::size_type npos = next.find( '=' );
                     if( npos == string::npos )
                        throw runtime_error( "unexpected format for specification string '" + next + "'" );

                     string name( next.substr( 0, npos ) );
                     string value( next.substr( npos + 1 ) );

                     string next_string(
                      get_obj( ).child_Specification( ).Id( ) + "_" + name + " \"" + value + "\"" );

                     all_class_strings[ get_obj( ).child_Specification( ).Class( ).Id( ) ].push_back( next_string );

                     if( pos == string::npos )
                        break;

                     strings.erase( 0, pos + 1 );
                  }
               }

               string vars( get_obj( ).child_Specification( ).All_Vars( ) );

               outf << "\x60{\x60}\n";

               outf << "\x60{\x60$specification_" << specification_name
                + "_id\x60=\x60'" << get_obj( ).child_Specification( ).Id( ) << "\x60'\x60}\n";

               while( true )
               {
                  pos = vars.find( '\n' );
                  string next( vars.substr( 0, pos ) );

                  string::size_type npos = next.find( '=' );
                  if( npos == string::npos )
                     throw runtime_error( "unexpected format for specification var '" + next + "'" );

                  outf << "\x60{\x60$specification_" + specification_name
                   << "_" << next.substr( 0, npos ) << "\x60=\x60'"
                   << get_mapped_id( model_name, next.substr( npos + 1 ) ) << "\x60'\x60}\n";

                  if( pos == string::npos )
                     break;

                  vars.erase( 0, pos + 1 );
               }
            }

            get_obj( ).child_Specification( ).finish_review( );

         } while( get_obj( ).child_Specification( ).iterate_next( ) );
      }

      // NOTE: To ensure consistency views are processed in name order but as this is transient
      // first determine the key order from a map.
      map< string, string > view_records;

      if( get_obj( ).child_View( ).iterate_forwards( true, 0, e_sql_optimisation_unordered ) )
      {
         do
         {
            view_records.insert( make_pair( get_obj( ).child_View( ).Name( ), get_obj( ).child_View( ).get_key( ) ) );
         } while( get_obj( ).child_View( ).iterate_next( ) );
      }

      if( !view_records.empty( ) )
      {
         for( map< string, string >::iterator vi = view_records.begin( ); vi != view_records.end( ); ++vi )
         {
            get_obj( ).child_View( ).perform_fetch( vi->second );

            string specification_name( get_obj( ).child_View( ).Class( ).Name( ) + '_' );

            string type;
            string pdf_vars_file;

            if( get_obj( ).child_View( ).Type( ).get_key( ) == "normal"
             || get_obj( ).child_View( ).Type( ).get_key( ) == "user_info" )
            {
               if( get_obj( ).child_View( ).Access_Restriction( ) == 0 )
                  type = "standard";
               else if( get_obj( ).child_View( ).Access_Restriction( ) == 1 ) // i.e. owner_only
                  type = "owner";
               else if( get_obj( ).child_View( ).Access_Restriction( ) == 2 ) // i.e. admin_only
                  type = "admin";
               else if( get_obj( ).child_View( ).Access_Restriction( ) == 3 ) // i.e. admin_owner_only
                  type = "admin_owner";
               else if( get_obj( ).child_View( ).Access_Restriction( ) == 4 ) // i.e. denied_always
               {
                  if( is_null( get_obj( ).child_View( ).Access_Permission( ) ) )
                     type = "none";
                  else
                     type = "standard";
               }
               else
                  throw runtime_error( "unsupported view access restriction #"
                   + to_string( get_obj( ).child_View( ).Access_Restriction( ) ) + " in Model::Generate" );

               specification_name += "view";
            }
            else if( get_obj( ).child_View( ).Type( ).get_key( ) == "print" )
            {
               pdf_vars_file = get_obj( ).Name( ) + "_" + get_obj( ).child_View( ).Class( ).Name( ) + ".pdf.vars.xrep";

               if( exists_file( pdf_vars_file ) )
                  remove_file( pdf_vars_file );

               if( get_obj( ).child_View( ).PDF_View_Type( ) != 0 )
                  get_obj( ).child_View( ).Generate_PDF_View( );

               if( get_obj( ).child_View( ).Access_Restriction( ) == 0 )
                  type = "print";
               else if( get_obj( ).child_View( ).Access_Restriction( ) == 1 ) // i.e. owner_only
                  type = "owner_print";
               else if( get_obj( ).child_View( ).Access_Restriction( ) == 2 ) // i.e. admin_only
                  type = "admin_print";
               else if( get_obj( ).child_View( ).Access_Restriction( ) == 3 ) // i.e. admin_owner_only
                  type = "admin_owner_print";
               else if( get_obj( ).child_View( ).Access_Restriction( ) == 4 ) // i.e. denied_always
               {
                  if( is_null( get_obj( ).child_View( ).Access_Permission( ) ) )
                     type = "none";
                  else
                     type = "print";
               }
               else
                  throw runtime_error( "unsupported view access restriction #"
                   + to_string( get_obj( ).child_View( ).Access_Restriction( ) ) + " in Model::Generate" );

               specification_name += "print";
            }
            else
               throw runtime_error( "unexpected view type '" + get_obj( ).child_View( ).Type( ).get_key( ) + "' in Model::Generate" );

            outf << "\x60{\x60}\n";

            outf << "\x60{\x60$specification_" << specification_name
             + "_id\x60=\x60'" << get_obj( ).child_View( ).Id( ) << "\x60'\x60}\n";

            outf << "\x60{\x60$specification_" << specification_name
             + "_cid\x60=\x60'" << get_mapped_id( model_name, get_obj( ).child_View( ).Class( ).Id( ) ) << "\x60'\x60}\n";

            string title( search_replace( get_obj( ).child_View( ).Title( ), "_", " " ) );
            string::size_type pos = 0;

            while( true )
            {
               pos = title.find_first_of( "?*" );

               if( pos == string::npos )
                  break;

               if( title.substr( pos, 1 ) == "?" )
                  title.insert( pos + 1, search_replace( get_obj( ).child_View( ).Class( ).Name( ), "_", " " ) );
               else if( title.substr( pos, 1 ) == "*" )
                  title.insert( pos + 1, search_replace( get_obj( ).child_View( ).Class( ).Plural( ), "_", " " ) );

               title.erase( pos, 1 );
            }

            string id( get_obj( ).child_View( ).Class( ).Id( ) );

            all_class_strings[ id ].push_back( get_obj( ).child_View( ).Id( ) + "_name \"" + title + "\"" );

            string extras;

            switch( get_obj( ).child_View( ).Change_Restriction( ) )
            {
               case 0:
               break;

               case 1:
               extras = "owner_edit";
               break;
            
               case 2:
               extras = "admin_edit";
               break;

               case 3:
               extras = "admin_owner_edit";
               break;

               case 4:
               extras = "no_edit";
               break;

               default:
               throw runtime_error( "unexpected Change_Restriction #"
                + to_string( get_obj( ).child_View( ).Change_Restriction( ) ) + " in Model::Generate" );
            }

            if( !is_null( get_obj( ).child_View( ).Access_Permission( ) ) )
            {
               if( type == "standard" )
                  type += "=" + get_obj( ).child_View( ).Access_Permission( ).Id( );
               else
                  type += "=!" + get_obj( ).child_View( ).Access_Permission( ).Id( );
            }

            if( !is_null( get_obj( ).child_View( ).Change_Permission( ) ) )
            {
               if( extras.empty( ) )
                  extras = "no_edit";
               extras += "=!" + get_obj( ).child_View( ).Change_Permission( ).Id( );
            }

            if( get_obj( ).child_View( ).Allow_Copy_Action( ) )
            {
               if( !extras.empty( ) )
                  extras += ",";
               extras += "can_copy";
            }

            if( get_obj( ).child_View( ).Auto_Back_After_Save( ) )
            {
               if( !extras.empty( ) )
                  extras += ",";
               extras += "auto_back";
            }

            if( !get_obj( ).child_View( ).Allow_Printable_Version( ) )
            {
               if( !extras.empty( ) )
                  extras += ",";
               extras += "no_print";
            }

            if( get_obj( ).child_View( ).Type( ).get_key( ) == "user_info" )
            {
               if( !extras.empty( ) )
                  extras += ",";
               extras += "user_info";
            }

            if( get_obj( ).child_View( ).Print_Without_Highlight( ) )
            {
               if( !extras.empty( ) )
                  extras += ",";
               extras += "print_no_highlight";
            }

            if( get_obj( ).child_View( ).Use_First_Row_As_Header( ) )
            {
               if( !extras.empty( ) )
                  extras += ",";
               extras += "first_row_as_header";
            }

            if( get_obj( ).child_View( ).Ignore_Parent_Record( ) )
            {
               if( !extras.empty( ) )
                  extras += ",";
               extras += "ignore_parent_record";
            }

            if( get_obj( ).child_View( ).Ignore_Unactionable_Records( ) )
            {
               if( !extras.empty( ) )
                  extras += ",";
               extras += "ignore_unactionable";
            }

            if( get_obj( ).child_View( ).File_Links_Always_As_Single( ) )
            {
               if( !extras.empty( ) )
                  extras += ",";
               extras += "file_links_always_as_single";
            }

            if( !is_null( get_obj( ).child_View( ).Class( ).Static_Instance_Key( ) ) )
            {
               if( !extras.empty( ) )
                  extras += ",";
               extras += "static_instance_key=" + get_obj( ).child_View( ).Class( ).Static_Instance_Key( );
            }

            string modifiers;

            string modifier_key_info( to_string(
             Meta_Modifier::static_get_field_id( Meta_Modifier::e_field_id_Name ) ) + ' ' );

            if( get_obj( ).child_View( ).Class( ).child_Modifier( ).iterate_forwards( modifier_key_info ) )
            {
               uint64_t flag_value( UINT64_C( 0x100 ) );

               do
               {
                  ostringstream osstr;

                  osstr << hex << flag_value;

                  flag_value <<= 1;

                  if( get_obj( ).child_View( ).Class( ).child_Modifier( ).child_Modifier_Affect( ).iterate_forwards( ) )
                  {
                     do
                     {
                        if( get_obj( ).child_View( ).Class( ).child_Modifier( ).child_Modifier_Affect( ).Scope( ) == 1
                         && ( get_obj( ).child_View( ).Class( ).child_Modifier( ).child_Modifier_Affect( ).Extra( ) == 0
                         || get_obj( ).child_View( ).Class( ).child_Modifier( ).child_Modifier_Affect( ).Extra( ) == 2 ) )
                        {
                           if( !modifiers.empty( ) )
                              modifiers += ',';

                           modifiers += osstr.str( ) + ':';

                           int type = get_obj( ).child_View( ).Class( ).child_Modifier( ).child_Modifier_Affect( ).Type( );

                           switch( type )
                           {
                              case 0:
                              modifiers += "protect";
                              break;

                              case 1:
                              modifiers += "relegate";
                              break;

                              case 2:
                              modifiers += "lowlight";
                              break;

                              case 3:
                              modifiers += "lowlight1";
                              break;

                              case 4:
                              modifiers += "highlight";
                              break;

                              case 5:
                              modifiers += "highlight1";
                              break;

                              case 6:
                              modifiers += "extralight";
                              break;

                              case 7:
                              modifiers += "extralight1";
                              break;

                              default:
                              throw runtime_error( "unknown modifier type #" + to_string( type ) + " in Model::Generate" );
                           }
                        }
                     } while( get_obj( ).child_View( ).Class( ).child_Modifier( ).child_Modifier_Affect( ).iterate_next( ) );
                  }
               } while( get_obj( ).child_View( ).Class( ).child_Modifier( ).iterate_next( ) );
            }

            string view_binfo;

            if( !is_null( get_obj( ).child_View( ).Class( ).Source_Model( ) ) )
            {
               view_binfo = get_obj( ).child_View( ).Class( ).Source_Model( ).Name( );
               view_binfo += ":" + get_obj( ).child_View( ).Class( ).Name( );
            }

            outf << "\x60{\x60$specification_" << specification_name + "_name\x60=\x60'" << title << "\x60'\x60}\n";
            outf << "\x60{\x60$specification_" << specification_name + "_type\x60=\x60'" << type << "\x60'\x60}\n";
            outf << "\x60{\x60$specification_" << specification_name + "_extra\x60=\x60'" << extras << "\x60'\x60}\n";

            if( !pdf_vars_file.empty( ) && exists_file( pdf_vars_file ) )
            {
               string pdf_spec_file( get_obj( ).Name( ) + "_" + get_obj( ).child_View( ).Class( ).Name( ) + ".pdf.sio" );
               outf << "\x60{\x60$specification_" << specification_name + "_pdf_spec\x60=\x60'" << pdf_spec_file << "\x60'\x60}\n";
            }

            outf << "\x60{\x60$specification_" << specification_name + "_mods\x60=\x60'" << modifiers << "\x60'\x60}\n";
            outf << "\x60{\x60$specification_" << specification_name + "_binfo\x60=\x60'" << view_binfo << "\x60'\x60}\n";

            string view_field_key_info( to_string( Meta_View_Field::static_get_field_id( Meta_View_Field::e_field_id_Order ) ) + ' ' );

            if( get_obj( ).child_View( ).child_View_Field( ).iterate_forwards( view_field_key_info ) )
            {
               vector< string > field_ids;
               vector< string > field_names;
               vector< string > field_types;
               vector< string > field_extras;
               vector< string > field_modifiers;
               vector< string > field_mandatory;

               map< int, pair< string, string > > parent_class_info;
               map< int, pair< string, string > > parent_field_info;
               map< int, string > parent_dfield_info;
               map< int, string > parent_field_extra;

               int field_num = 0;

               do
               {
                  ++field_num;

                  string access_restriction;

                  switch( get_obj( ).child_View( ).child_View_Field( ).Access_Restriction( ) )
                  {
                     case 0:
                     break;

                     case 1:
                     access_restriction = "owner_only";
                     break;

                     case 2:
                     access_restriction = "admin_only";
                     break;

                     case 3:
                     access_restriction = "admin_owner_only";
                     break;

                     case 4: // i.e. denied_always
                     access_restriction = "hidden";
                     break;

                     default:
                     throw runtime_error( "unexpected access restriction #"
                      + to_string( get_obj( ).child_View( ).child_View_Field( ).Access_Restriction( ) ) + " in Model::Generate" );
                  }

                  string access_perm = get_obj( ).child_View( ).child_View_Field( ).Access_Permission( ).Id( );

                  if( !access_perm.empty( ) )
                  {
                     if( access_restriction.empty( ) )
                        access_restriction = "hidden";

                     access_restriction += "=!" + access_perm;
                  }

                  if( get_obj( ).child_View( ).child_View_Field( ).Type( ).get_key( ) == "field" )
                  {
                     string field_name( "field_" );

                     field_name += lower( get_obj( ).child_View( ).Class( ).Name( ) );

                     string extras;

                     if( get_obj( ).child_View( ).child_View_Field( ).Access_Scope( ) != 0 )
                     {
                        if( get_obj( ).child_View( ).child_View_Field( ).Access_Scope( ) == 1 ) // i.e. create_only
                           extras = "new_only";
                        else if( get_obj( ).child_View( ).child_View_Field( ).Access_Scope( ) == 2 ) // i.e. post_create
                           extras = "non_new";
                        else if( get_obj( ).child_View( ).child_View_Field( ).Access_Scope( ) == 3 ) // i.e. editing_only
                           extras = "non_view";
                        else if( get_obj( ).child_View( ).child_View_Field( ).Access_Scope( ) == 4 ) // i.e. viewing_only
                           extras = "view_only";
                        else if( get_obj( ).child_View( ).child_View_Field( ).Access_Scope( ) == 5 ) // i.e. updating_only
                           extras = "edit_only";
                        else if( get_obj( ).child_View( ).child_View_Field( ).Access_Scope( ) == 6 ) // i.e. viewing_non_null
                           extras = "view_non_null";
                        else if( get_obj( ).child_View( ).child_View_Field( ).Access_Scope( ) == 7 ) // i.e. always_if_non_null
                           extras = "always_non_null";
                        else
                           throw runtime_error( "unexpected access scope #"
                            + to_string( get_obj( ).child_View( ).child_View_Field( ).Access_Scope( ) ) + " in Model::Generate" );
                     }

                     if( !access_restriction.empty( ) )
                     {
                        if( !extras.empty( ) )
                           extras += '+';

                        extras += access_restriction;
                     }

                     if( get_obj( ).child_View( ).child_View_Field( ).Change_Scope( ) != 0 )
                     {
                        if( !extras.empty( ) )
                           extras += '+';

                        if( get_obj( ).child_View( ).child_View_Field( ).Change_Scope( ) == 1 ) // i.e. create_only
                           extras += "new_edit";
                        else if( get_obj( ).child_View( ).child_View_Field( ).Change_Scope( ) == 2 ) // i.e. update_only
                           extras += "edit_edit";
                        else if( get_obj( ).child_View( ).child_View_Field( ).Change_Scope( ) == 3 ) // i.e. always_editable
                           extras += "always_editable";
                        else
                           throw runtime_error( "unexpected change scope #"
                            + to_string( get_obj( ).child_View( ).child_View_Field( ).Change_Scope( ) ) + " in Model::Generate" );
                     }

                     bool no_change_restriction = false;

                     switch( get_obj( ).child_View( ).child_View_Field( ).Change_Restriction( ) )
                     {
                        case 0:
                        no_change_restriction = true;
                        break;

                        case 1:
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "owner_edit";
                        break;

                        case 2:
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "admin_edit";
                        break;

                        case 3:
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "admin_owner_edit";
                        break;

                        case 4: // i.e. denied_always
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "no_edit";
                        break;

                        default:
                        throw runtime_error( "unexpected change restriction #"
                         + to_string( get_obj( ).child_View( ).child_View_Field( ).Change_Restriction( ) ) + " in Model::Generate" );
                     }

                     string change_perm = get_obj( ).child_View( ).child_View_Field( ).Change_Permission( ).Id( );

                     if( !change_perm.empty( ) )
                     {
                        if( extras.empty( ) )
                           extras = "no_edit";
                        else if( no_change_restriction )
                           extras += "+no_edit";

                        extras += "=!" + change_perm;
                     }

                     bool is_mandatory = false;
                     bool has_date_precision = false;

                     Meta_Field* p_field = 0;
                     Meta_Field* p_pfield = 0;
                     Meta_Field* p_xfield = 0;

                     if( is_null( get_obj( ).child_View( ).child_View_Field( ).Source_Parent( ) ) )
                     {
                        p_field = &get_obj( ).child_View( ).child_View_Field( ).Source_Field( );

                        if( p_field->Mandatory( ) )
                           is_mandatory = true;

                        switch( get_obj( ).child_View( ).child_View_Field( ).Mandatory_Option( ) )
                        {
                           case 0:
                           break;

                           case 1:
                           is_mandatory = false;
                           break;

                           case 2:
                           is_mandatory = true;
                           break;

                           default:
                           throw runtime_error( "unexpected mandatory option #"
                            + to_string( get_obj( ).child_View( ).child_View_Field( ).Mandatory_Option( ) ) + " in Model::Generate" );
                        }

                        switch( get_obj( ).child_View( ).child_View_Field( ).Date_Precision_Option( ) )
                        {
                           case 0:
                           break;

                           case 1:
                           has_date_precision = true;
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "date_precision=months";
                           break;

                           case 2:
                           has_date_precision = true;
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "date_precision=decades";
                           break;

                           default:
                           throw runtime_error( "unexpected date precision option #"
                            + to_string( get_obj( ).child_View( ).child_View_Field( ).Date_Precision_Option( ) ) + " in Model::Generate" );
                        }

                        field_ids.push_back( p_field->Id( ) );

                        field_types.push_back( meta_field_type_name(
                         get_obj( ).child_View( ).child_View_Field( ).Source_Field( ).Primitive( ),
                         get_obj( ).child_View( ).child_View_Field( ).Source_Field( ).Mandatory( ), "", "" ) );

                        field_names.push_back( field_name + "_" + lower( p_field->Name( ) ) );

                        switch( get_obj( ).child_View( ).child_View_Field( ).Enum_Starts_At( ) )
                        {
                           case 0:
                           break;

                           case 1:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=key1";
                           break;

                           case 2:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=key2";
                           break;

                           case 3:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=key3";
                           break;

                           case 4:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=key4";
                           break;

                           case 5:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=key5";
                           break;

                           case 6:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=key6";
                           break;

                           case 7:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=key7";
                           break;

                           case 8:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=key8";
                           break;

                           case 9:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=key9";
                           break;

                           case 10:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=key0";
                           break;

                           case 11:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=fkey1";
                           break;

                           case 12:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=fkey2";
                           break;

                           case 13:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=fkey3";
                           break;

                           case 14:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=fkey4";
                           break;

                           case 15:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=fkey5";
                           break;

                           case 16:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=fkey6";
                           break;

                           case 17:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=fkey7";
                           break;

                           case 18:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=fkey8";
                           break;

                           case 19:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=fkey9";
                           break;

                           case 20:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "estart=fkey0";
                           break;

                           default:
                           throw runtime_error( "unexpected enum starts at #"
                            + to_string( get_obj( ).child_View( ).child_View_Field( ).Enum_Starts_At( ) ) + " in Model::Generate" );
                        }

                        switch( get_obj( ).child_View( ).child_View_Field( ).Enum_Finishes_At( ) )
                        {
                           case 0:
                           break;

                           case 1:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=key1";
                           break;

                           case 2:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=key2";
                           break;

                           case 3:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=key3";
                           break;

                           case 4:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=key4";
                           break;

                           case 5:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=key5";
                           break;

                           case 6:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=key6";
                           break;

                           case 7:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=key7";
                           break;

                           case 8:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=key8";
                           break;

                           case 9:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=key9";
                           break;

                           case 10:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=key0";
                           break;

                           case 11:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=fkey1";
                           break;

                           case 12:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=fkey2";
                           break;

                           case 13:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=fkey3";
                           break;

                           case 14:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=fkey4";
                           break;

                           case 15:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=fkey5";
                           break;

                           case 16:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=fkey6";
                           break;

                           case 17:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=fkey7";
                           break;

                           case 18:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=fkey8";
                           break;

                           case 19:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=fkey9";
                           break;

                           case 20:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "efinish=fkey0";
                           break;

                           default:
                           throw runtime_error( "unexpected enum finishes at #"
                            + to_string( get_obj( ).child_View( ).child_View_Field( ).Enum_Finishes_At( ) ) + " in Model::Generate" );
                        }

                        switch( get_obj( ).child_View( ).child_View_Field( ).Trigger_Option( ) )
                        {
                           case 0:
                           break;

                           case 1:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "key1";
                           break;

                           case 2:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "key2";
                           break;

                           case 3:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "key3";
                           break;

                           case 4:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "key4";
                           break;

                           case 5:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "key5";
                           break;

                           case 6:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "key6";
                           break;

                           case 7:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "key7";
                           break;

                           case 8:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "key8";
                           break;

                           case 9:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "key9";
                           break;

                           case 10:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "key0";
                           break;

                           default:
                           throw runtime_error( "unexpected trigger option #"
                            + to_string( get_obj( ).child_View( ).child_View_Field( ).Trigger_Option( ) ) + " in Model::Generate" );
                        }

                        switch( get_obj( ).child_View( ).child_View_Field( ).Trigger_Behaviour( ) )
                        {
                           case 0:
                           break;

                           case 1:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "force";
                           break;

                           case 2:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "forced";
                           break;
                        
                           case 3:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "force+forced";
                           break;

                           default:
                           throw runtime_error( "unexpected trigger behaviour #"
                            + to_string( get_obj( ).child_View( ).child_View_Field( ).Trigger_Behaviour( ) ) + " in Model::Generate" );
                        }

                        switch( get_obj( ).child_View( ).child_View_Field( ).Trigger_For_State( ) )
                        {
                           case 0:
                           break;

                           case 1:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "trigger_protected";
                           break;

                           case 2:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "trigger_relegated";
                           break;

                           default:
                           throw runtime_error( "unexpected trigger for state #"
                            + to_string( get_obj( ).child_View( ).child_View_Field( ).Trigger_For_State( ) ) + " in Model::Generate" );
                        }

                        if( p_field->Id( ) == p_field->Class( ).Quick_Link_Field( ).Id( ) )
                        {
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "quick_link";
                        }
                     }
                     else
                     {
                        p_field = &get_obj( ).child_View( ).child_View_Field( ).Source_Child( );
                        p_pfield = &get_obj( ).child_View( ).child_View_Field( ).Source_Parent( );

                        if( p_pfield->Parent_Class( ).Model( ).get_key( ) != get_obj( ).get_key( ) )
                           throw runtime_error( "Cannot use unaliased external parent class '"
                            + p_pfield->Parent_Class( ).Name( ) + "' in view '" + get_obj( ).child_View( ).Name( )
                            + "' for field '" + get_obj( ).child_View( ).child_View_Field( ).Name( ) + "'." );

                        field_ids.push_back( p_pfield->Id( ) );

                        field_types.push_back( meta_field_type_name(
                         get_obj( ).child_View( ).child_View_Field( ).Source_Child( ).Primitive( ),
                         get_obj( ).child_View( ).child_View_Field( ).Source_Child( ).Mandatory( ), "", "" ) );

                        field_names.push_back( field_name + "_"
                         + lower( get_obj( ).child_View( ).child_View_Field( ).Source_Parent( ).Name( ) ) );

                        if( get_obj( ).child_View( ).child_View_Field( ).Source_Parent( ).Mandatory( ) )
                           is_mandatory = true;

                        bool is_manual_link = false;

                        if( p_field->Extra( ) == 13 ) // i.e. "manual_link"
                           is_manual_link = true;

                        switch( get_obj( ).child_View( ).child_View_Field( ).Mandatory_Option( ) )
                        {
                           case 0:
                           break;

                           case 1:
                           is_mandatory = false;
                           break;

                           case 2:
                           is_mandatory = true;
                           break;

                           default:
                           throw runtime_error( "unexpected mandatory option #"
                            + to_string( get_obj( ).child_View( ).child_View_Field( ).Mandatory_Option( ) ) + " in Model::Generate" );
                        }

                        switch( get_obj( ).child_View( ).child_View_Field( ).Date_Precision_Option( ) )
                        {
                           case 0:
                           break;

                           case 1:
                           has_date_precision = true;
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "date_precision=months";
                           break;

                           case 2:
                           has_date_precision = true;
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "date_precision=decades";
                           break;

                           default:
                           throw runtime_error( "unexpected date precision option #"
                            + to_string( get_obj( ).child_View( ).child_View_Field( ).Date_Precision_Option( ) ) + " in Model::Generate" );
                        }

                        if( get_obj( ).child_View( ).child_View_Field( ).Source_Parent( ).Extra( ) < 0 )
                        {
                           if( !extras.empty( ) )
                              extras += '+';
                           switch( get_obj( ).child_View( ).child_View_Field( ).Source_Parent( ).Extra( ) )
                           {
                              case -5:
                              extras += "modify_user_key";
                              break;

                              case -4:
                              extras += "create_user_key";
                              break;

                              case -3:
                              extras += "user_other";
                              break;

                              case -2:
                              extras += "user_group";
                              break;

                              case -1:
                              extras += "owning_user";
                              break;

                              default:
                              throw runtime_error( "unexpected negative extra #"
                               + to_string( get_obj( ).child_View( ).child_View_Field( ).Source_Parent( ).Extra( ) ) + " in Model::Generate" );
                           }
                        }

                        bool invert_perm = false;

                        switch( get_obj( ).child_View( ).child_View_Field( ).Link_Restriction( ) )
                        {
                           case 0:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "link";
                           break;

                           case 1:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "owner_link";
                           invert_perm = true;
                           break;

                           case 2:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "admin_link";
                           invert_perm = true;
                           break;

                           case 3:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "admin_owner_link";
                           invert_perm = true;
                           break;

                           case 4: // i.e. denied_always
                           if( !is_null( get_obj( ).child_View( ).child_View_Field( ).Link_Permission( ) ) )
                           {
                              if( !extras.empty( ) )
                                 extras += '+';
                              extras += "link";
                           }
                           break;
                        }

                        if( !is_null( get_obj( ).child_View( ).child_View_Field( ).Link_Permission( ) ) )
                        {
                           extras += "=";
                           if( invert_perm )
                              extras += "!";
                           extras += get_obj( ).child_View( ).child_View_Field( ).Link_Permission( ).Id( );
                        }

                        switch( get_obj( ).child_View( ).child_View_Field( ).New_Source( ) )
                        {
                           case 0:
                           break;

                           case 1:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "new_value=@user";
                           break;

                           case 2:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "new_value=@group";
                           break;

                           case 3:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "new_value=@other";
                           break;

                           case 4:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "new_value=" + get_obj( ).child_View( ).child_View_Field( ).New_Value( );
                           break;

                           default:
                           throw runtime_error( "unexpected new source #"
                            + to_string( get_obj( ).child_View( ).child_View_Field( ).New_Source( ) ) + " in Model::Generate" );
                        }

                        switch( get_obj( ).child_View( ).child_View_Field( ).FK_Trigger_Option( ) )
                        {
                           case 0:
                           break;

                           case 1:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "fkey1";
                           break;

                           case 2:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "fkey2";
                           break;

                           case 3:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "fkey3";
                           break;

                           case 4:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "fkey4";
                           break;

                           case 5:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "fkey5";
                           break;

                           case 6:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "fkey6";
                           break;

                           case 7:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "fkey7";
                           break;

                           case 8:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "fkey8";
                           break;

                           case 9:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "fkey9";
                           break;

                           case 10:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "fkey0";
                           break;

                           default:
                           throw runtime_error( "unexpected fk trigger option #"
                            + to_string( get_obj( ).child_View( ).child_View_Field( ).FK_Trigger_Option( ) ) + " in Model::Generate" );
                        }

                        switch( get_obj( ).child_View( ).child_View_Field( ).FK_Trigger_Behaviour( ) )
                        {
                           case 0:
                           break;

                           case 1:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "force";
                           break;

                           case 2:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "forced";
                           break;
                        
                           case 3:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "force+forced";
                           break;

                           default:
                           throw runtime_error( "unexpected fk trigger behaviour #"
                            + to_string( get_obj( ).child_View( ).child_View_Field( ).FK_Trigger_Behaviour( ) ) + " in Model::Generate" );
                        }

                        switch( get_obj( ).child_View( ).child_View_Field( ).Trigger_For_State( ) )
                        {
                           case 0:
                           break;

                           case 1:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "trigger_protected";
                           break;

                           case 2:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "trigger_relegated";
                           break;

                           default:
                           throw runtime_error( "unexpected trigger for state #"
                            + to_string( get_obj( ).child_View( ).child_View_Field( ).Trigger_For_State( ) ) + " in Model::Generate" );
                        }

                        parent_class_info.insert( make_pair( field_num,
                         make_pair( get_obj( ).child_View( ).child_View_Field( ).Source_Child( ).Class( ).Id( ),
                         get_obj( ).child_View( ).child_View_Field( ).Source_Child( ).Class( ).Name( ) ) ) );

                        string id( p_field->Id( ) );

                        if( !get_obj( ).child_View( ).child_View_Field( ).Source_Edit_Child( ).Id( ).empty( ) )
                        {
                           p_xfield = &get_obj( ).child_View( ).child_View_Field( ).Source_Edit_Child( );

                           id += "+" + get_obj( ).child_View( ).child_View_Field( ).Source_Edit_Child( ).Id( );

                           parent_dfield_info.insert( make_pair( field_num,
                            get_obj( ).child_View( ).child_View_Field( ).Source_Child( ).Class( ).Name( )
                            + "_" + get_obj( ).child_View( ).child_View_Field( ).Source_Edit_Child( ).Name( ) ) );
                        }

                        parent_field_info.insert( make_pair( field_num, make_pair( id, p_field->Name( ) ) ) );

                        string field_extra;
                        bool is_restricted = false;

                        if( !is_null( get_obj( ).child_View( ).child_View_Field( ).Restriction_Spec( ) ) )
                        {
                           is_restricted = true;
                           field_extra = get_obj( ).child_View( ).child_View_Field( ).Restriction_Spec( ).Restrict_Values( );
                        }

                        if( p_field->Class( ).child_Field( ).iterate_forwards( ) )
                        {
                           do
                           {
                              if( p_field->Class( ).child_Field( ).Extra( ) == 5 ) // i.e. "order"
                              {
                                 if( !field_extra.empty( ) )
                                    field_extra += "+";
                                 field_extra += p_field->Class( ).child_Field( ).Id( );
                              }
                              else if( p_field->Class( ).child_Field( ).Extra( ) == 17 ) // i.e. "permission"
                              {
                                 // NOTE: If the field belongs to an aliased class then need to instead use
                                 // the Source Field in order to locate the filter.
                                 Meta_Field* p_sfield( &p_field->Class( ).child_Field( ) );

                                 if( !is_null( p_field->Class( ).child_Field( ).Source_Field( ) ) )
                                    p_sfield = &p_field->Class( ).child_Field( ).Source_Field( );

                                 if( p_sfield->child_Specification( ).iterate_forwards( ) )
                                 {
                                    do
                                    {
                                       if( p_sfield->child_Specification( ).Specification_Type( ) == "filter_perm_restricted" )
                                       {
                                          if( !field_extra.empty( ) )
                                             field_extra += "+";
                                          field_extra += "@permission:" + p_sfield->child_Specification( ).Id( );

                                          p_sfield->child_Specification( ).iterate_stop( );
                                          break;
                                       }
                                    } while( p_sfield->child_Specification( ).iterate_next( ) );
                                 }
                              }

                           } while( p_field->Class( ).child_Field( ).iterate_next( ) );
                        }

                        if( p_field->Encrypted( ) )
                        {
                           if( !field_extra.empty( ) )
                              field_extra += '+';
                           field_extra += "@decrypt";
                        }

                        if( get_obj( ).child_View( ).child_View_Field( ).Sort_Manually( ) )
                        {
                           if( !field_extra.empty( ) )
                              field_extra += '+';
                           field_extra += !is_manual_link ? "@sort" : "@sortlinks";
                        }

                        parent_field_extra.insert( make_pair( field_num, field_extra ) );
                     }

                     switch( get_obj( ).child_View( ).child_View_Field( ).Child_List_Extra_Option( ) )
                     {
                        case 0:
                        break;

                        case 1:
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "vextra1";
                        break;

                        case 2:
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "vextra2";
                        break;
                     }

                     switch( get_obj( ).child_View( ).child_View_Field( ).Font_Size( ) )
                     {
                        case 0: // i.e. normal
                        break;

                        case 1:
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "large";
                        break;

                        case 2:
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "larger";
                        break;

                        case 5:
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "small";
                        break;

                        case 6:
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "smaller";
                        break;

                        default:
                        throw runtime_error( "unexpected Font_Size value #"
                         + to_string( get_obj( ).child_View( ).child_View_Field( ).Font_Size( ) )
                         + " in Model::Generate" );
                     }

                     switch( get_obj( ).child_View( ).child_View_Field( ).Alignment( ) )
                     {
                        case 0: // i.e. default
                        break;

                        case 1:
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "left";
                        break;

                        case 2:
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "right";
                        break;

                        case 3:
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "center";
                        break;

                        case 4:
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "justify";
                        break;

                        default:
                        throw runtime_error( "unexpected Alignment value #"
                         + to_string( get_obj( ).child_View( ).child_View_Field( ).Alignment( ) )
                         + " in Model::Generate" );
                     }

                     if( get_obj( ).child_View( ).child_View_Field( ).Source_Field( ).Extra( ) == 3 ) // i.e. image
                     {
                        switch( get_obj( ).child_View( ).child_View_Field( ).Orientation( ) )
                        {
                           case 0:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "landscape";
                           break;

                           case 1:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "portrait";
                           break;

                           case 2:
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "neither";
                           break;

                           default:
                           throw runtime_error( "unexpected Orientation value #"
                            + to_string( get_obj( ).child_View( ).child_View_Field( ).Orientation( ) )
                            + " in Model::Generate" );
                        }
                     }

                     if( get_obj( ).child_View( ).child_View_Field( ).Use_Full_Width( ) )
                     {
                        if( !extras.empty( ) )
                           extras += '+';

                        extras += "full_width";
                     }

                     if( get_obj( ).child_View( ).child_View_Field( ).Use_Full_Height( ) )
                     {
                        if( !extras.empty( ) )
                           extras += '+';

                        extras += "full_height";
                     }

                     if( get_obj( ).child_View( ).child_View_Field( ).Ignore_Manual_Links( ) )
                     {
                        if( !extras.empty( ) )
                           extras += '+';

                        extras += "ignore_links";
                     }

                     switch( get_obj( ).child_View( ).child_View_Field( ).Label_Source_Child( ) )
                     {
                        case 0: // i.e. default
                        break;

                        case 1:
                        {
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "non_prefixed";
                           break;
                        }

                        case 2:
                        {
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "child_always";
                           break;
                        }

                        default:
                        throw runtime_error( "unexpected Label_Source_Child value #"
                         + to_string( get_obj( ).child_View( ).child_View_Field( ).Label_Source_Child( ) )
                         + " in Model::Generate" );
                     }

                     if( !get_obj( ).child_View( ).child_View_Field( ).Allow_Anonymous_Access( ) )
                     {
                        if( !extras.empty( ) )
                           extras += '+';

                        extras += "no_anon";
                     }

                     if( get_obj( ).child_View( ).child_View_Field( ).Show_Hide_Start_Point( ) )
                     {
                        if( !extras.empty( ) )
                           extras += '+';

                        extras += "show";
                     }

                     // NOTE: If a "source edit child" was present then will get its limits
                     // otherwise get these limits from the standard field or "source child".
                     if( !p_xfield )
                        p_xfield = p_field;

                     string other_extras( meta_field_extras( p_xfield->UOM( ), p_xfield->UOM_Name( ),
                      p_xfield->Extra( ), p_xfield->Encrypted( ), p_xfield->Transient( ),
                      p_xfield->Type( ).Max_Size( ), p_xfield->Enum( ).Id( ), p_xfield->Enum_Filter( ).Id( ),
                      p_xfield->Type( ).Primitive( ), p_xfield->Type( ).Min_Value( ), p_xfield->Type( ).Max_Value( ),
                      p_xfield->Type( ).Numeric_Digits( ), p_xfield->Type( ).Numeric_Decimals( ),
                      p_xfield->Type( ).String_Domain( ), has_date_precision ? 0 : p_xfield->Type( ).Date_Precision( ),
                      p_xfield->Type( ).Time_Precision( ), p_xfield->Type( ).Show_Plus_Sign( ), p_xfield->Type( ).Zero_Padding( ),
                      p_xfield->Type( ).Int_Type( ), p_xfield->Type( ).Numeric_Type( ), ( p_pfield != 0 ) ) );

                     if( !other_extras.empty( ) )
                     {
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += other_extras;
                     }

                     Meta_Field check_spec_field;

                     check_spec_field.perform_fetch( p_field->get_key( ) );

                     // FUTURE: Perhaps should try to avoid unnecessary iteration of field child specifications.
                     if( check_spec_field.child_Specification( ).iterate_forwards( ) )
                     {
                        do
                        {
                           if( check_spec_field.child_Specification( ).Specification_Type( ) == "field_from_module_string_placeholder" )
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "default_placeholder";

                              check_spec_field.child_Specification( ).iterate_stop( );

                              break;
                           }
                        } while( check_spec_field.child_Specification( ).iterate_next( ) );
                     }

                     field_extras.push_back( extras );

                     Meta_Field* p_mfield( p_pfield ? p_pfield : p_field );

                     string modifiers;
                     string modifier_key_info( to_string( Meta_Modifier::static_get_field_id( Meta_Modifier::e_field_id_Name ) ) + ' ' );

                     if( p_mfield->Class( ).child_Modifier( ).iterate_forwards( modifier_key_info ) )
                     {
                        uint64_t flag_value( UINT64_C( 0x100 ) );

                        do
                        {
                           ostringstream osstr;

                           osstr << hex << flag_value;

                           flag_value <<= 1;

                           if( p_mfield->Class( ).child_Modifier( ).child_Modifier_Affect( ).iterate_forwards( ) )
                           {
                              do
                              {
                                 if( p_mfield->Class( ).child_Modifier( ).child_Modifier_Affect( ).Scope( ) == 0
                                  && p_mfield->Id( ) == p_mfield->Class( ).child_Modifier( ).child_Modifier_Affect( ).Field( ).Id( )
                                  && ( p_mfield->Class( ).child_Modifier( ).child_Modifier_Affect( ).Extra( ) == 0
                                  || p_mfield->Class( ).child_Modifier( ).child_Modifier_Affect( ).Extra( ) == 2 ) )
                                 {
                                    if( !modifiers.empty( ) )
                                       modifiers += ',';

                                    modifiers += osstr.str( ) + ':';

                                    int type = p_mfield->Class( ).child_Modifier( ).child_Modifier_Affect( ).Type( );

                                    switch( type )
                                    {
                                       case 0:
                                       modifiers += "protect";
                                       break;

                                       case 1:
                                       modifiers += "relegate";
                                       break;

                                       case 2:
                                       modifiers += "lowlight";
                                       break;

                                       case 3:
                                       modifiers += "lowlight1";
                                       break;

                                       case 4:
                                       modifiers += "highlight";
                                       break;

                                       case 5:
                                       modifiers += "highlight1";
                                       break;

                                       case 6:
                                       modifiers += "extralight";
                                       break;

                                       case 7:
                                       modifiers += "extralight1";
                                       break;

                                       default:
                                       throw runtime_error( "unknown modifier type #" + to_string( type ) + " in Model::Generate" );
                                    }
                                 }
                              } while( p_mfield->Class( ).child_Modifier( ).child_Modifier_Affect( ).iterate_next( ) );
                           }
                        } while( p_mfield->Class( ).child_Modifier( ).iterate_next( ) );
                     }

                     field_modifiers.push_back( modifiers );
                     field_mandatory.push_back( is_mandatory ? "true" : "false" );
                  }
                  else if( get_obj( ).child_View( ).child_View_Field( ).Type( ).get_key( ) == "key" )
                  {
                     field_ids.push_back( "@key" );
                     field_names.push_back( "field_key" );
                     field_types.push_back( "" );
                     field_extras.push_back( "" );
                     field_modifiers.push_back( "" );
                     field_mandatory.push_back( "true" );
                  }
                  else if( get_obj( ).child_View( ).child_View_Field( ).Type( ).get_key( ) == "tab" )
                  {
                     string tab_extras( access_restriction );

                     if( !get_obj( ).child_View( ).child_View_Field( ).Allow_Anonymous_Access( ) )
                     {
                        if( !tab_extras.empty( ) )
                           tab_extras += '+';
                        tab_extras += "no_anon";
                     }

                     field_ids.push_back( "@tab" );
                     field_names.push_back( lower( get_obj( ).child_View( ).child_View_Field( ).Name( ) ) );
                     field_types.push_back( "" );
                     field_extras.push_back( tab_extras );
                     field_modifiers.push_back( "" );
                     field_mandatory.push_back( "false" );

                     all_class_strings[ get_obj( ).child_View( ).Class( ).Id( ) ].push_back(
                      get_obj( ).child_View( ).Id( ) + "_" + lower( get_obj( ).child_View( ).child_View_Field( ).Name( ) )
                      + " \"" + search_replace( get_obj( ).child_View( ).child_View_Field( ).Tab_Name( ), "_", " " ) + "\"" );
                  }
               } while( get_obj( ).child_View( ).child_View_Field( ).iterate_next( ) );

               string fnums;

               for( size_t i = 0; i < field_ids.size( ); i++ )
               {
                  string fnum( to_string( i ) );

                  if( fnum.length( ) < 3 )
                     fnum = '0' + fnum;

                  if( fnum.length( ) < 3 )
                     fnum = '0' + fnum;

                  if( i > 0 )
                     fnums += ' ';

                  fnums += fnum;
               }

               outf << "\x60{\x60$specification_" << specification_name + "_fnums" << "\x60=\x60'" << fnums << "\x60'\x60}\n";

               for( size_t i = 0; i < field_ids.size( ); i++ )
               {
                  string fnum( to_string( i ) );

                  if( fnum.length( ) < 3 )
                     fnum = '0' + fnum;

                  if( fnum.length( ) < 3 )
                     fnum = '0' + fnum;

                  outf << "\x60{\x60$specification_" << specification_name
                   + "_field" << fnum << "\x60=\x60'" << get_mapped_id( model_name, field_ids[ i ] ) << "\x60'\x60}\n";

                  outf << "\x60{\x60$specification_" << specification_name
                   + "_ftype" << fnum << "\x60=\x60'" << field_types[ i ] << "\x60'\x60}\n";

                  outf << "\x60{\x60$specification_" << specification_name
                   + "_extra" << fnum << "\x60=\x60'" << field_extras[ i ] << "\x60'\x60}\n";

                  outf << "\x60{\x60$specification_" << specification_name
                   + "_fmods" << fnum << "\x60=\x60'" << field_modifiers[ i ] << "\x60'\x60}\n";

                  outf << "\x60{\x60$specification_" << specification_name
                   + "_fname" << fnum << "\x60=\x60'" << field_names[ i ] << "\x60'\x60}\n";

                  outf << "\x60{\x60$specification_" << specification_name
                   + "_fmand" << fnum << "\x60=\x60'" << field_mandatory[ i ] << "\x60'\x60}\n";

                  if( parent_class_info.count( i + 1 ) )
                  {
                     outf << "\x60{\x60$specification_" << specification_name + "_prcls" << fnum << "\x60=\x60'"
                      << get_mapped_id( model_name, parent_class_info[ i + 1 ].first ) << "\x60'\x60}\n";

                     outf << "\x60{\x60$specification_" << specification_name
                      + "_prcnm" << fnum << "\x60=\x60'" << parent_class_info[ i + 1 ].second << "\x60'\x60}\n";
                  }

                  if( parent_field_info.count( i + 1 ) )
                  {
                     outf << "\x60{\x60$specification_" << specification_name + "_prfld" << fnum << "\x60=\x60'"
                      << get_mapped_id( model_name, parent_field_info[ i + 1 ].first ) << "\x60'\x60}\n";

                     outf << "\x60{\x60$specification_" << specification_name
                      + "_prfnm" << fnum << "\x60=\x60'" << parent_field_info[ i + 1 ].second << "\x60'\x60}\n";
                  }

                  if( parent_dfield_info.count( i + 1 ) )
                     outf << "\x60{\x60$specification_" << specification_name
                      + "_pdfld" << fnum << "\x60=\x60'" << parent_dfield_info[ i + 1 ] << "\x60'\x60}\n";

                  if( parent_field_extra.count( i + 1 ) )
                     outf << "\x60{\x60$specification_" << specification_name
                      + "_prxtr" << fnum << "\x60=\x60'" << parent_field_extra[ i + 1 ] << "\x60'\x60}\n";
               }
            }
         }
      }

      // NOTE: As the list names are transient determine the key order from a map (the lists need to be
      // processed in such a manner so that variations can be found immediately after the original list).
      map< string, string > list_records;

      if( get_obj( ).child_List( ).iterate_forwards( true, 0, e_sql_optimisation_unordered ) )
      {
         do
         {
            list_records.insert( make_pair( get_obj( ).child_List( ).Name( ), get_obj( ).child_List( ).get_key( ) ) );
         } while( get_obj( ).child_List( ).iterate_next( ) );
      }

      if( !list_records.empty( ) )
      {
         int variation = 0;

         map< string, int > child_lists;

         for( map< string, string >::iterator li = list_records.begin( ); li != list_records.end( ); ++li )
         {
            get_obj( ).child_List( ).perform_fetch( li->second );

            string specification_name( get_obj( ).child_List( ).Class( ).Name( ) + '_' );

            string list_type( "standard" );
            string list_extra;

            string list_ext;
            string pdf_vars_file;

            if( get_obj( ).child_List( ).Style( ) != 0 )
            {
               if( get_obj( ).child_List( ).Style( ) == 1 )
                  list_extra = "search";
               else
                  throw runtime_error( "unexpected Style #"
                   + to_string( get_obj( ).child_List( ).Style( ) ) + " in Model::Generate" );
            }

            if( get_obj( ).child_List( ).Allow_Quick_Link( ) )
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';
               list_extra += "quick_link";
            }

            if( get_obj( ).child_List( ).Search_Option_Limit( ) )
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';
               list_extra += "search_opt_limit=" + to_string( get_obj( ).child_List( ).Search_Option_Limit( ) );
            }

            if( get_obj( ).child_List( ).Allow_Text_Search( ) )
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';
               list_extra += "text_search";
            }

            if( get_obj( ).child_List( ).Delete_Direction( ) == 1 )
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';
               list_extra += "reverse_del";
            }

            switch( get_obj( ).child_List( ).Text_Match_Highlight( ) )
            {
               case 0:
               break;

               case 1:
               if( !list_extra.empty( ) )
                  list_extra += ',';
               list_extra += "text_highlight";
               break;

               case 2:
               if( !list_extra.empty( ) )
                  list_extra += ',';
               list_extra += "text_highlight1";
               break;

               default:
               throw runtime_error( "unexpected Text_Match_Highlight #"
                + to_string( get_obj( ).child_List( ).Text_Match_Highlight( ) ) + " in Model::Generate" );
            }

            if( get_obj( ).child_List( ).Print_Restriction( ) != 0 ) // i.e. none
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';

               if( get_obj( ).child_List( ).Print_Restriction( ) == 1 ) // i.e. admin_only
                  list_extra += "admin_print";
               else if( get_obj( ).child_List( ).Print_Restriction( ) == 2 ) // i.e. denied_always
                  list_extra += "no_print";
               else
                  throw runtime_error( "unexpected Print_Restriction #"
                   + to_string( get_obj( ).child_List( ).Print_Restriction( ) ) + " in Model::Generate" );
            }

            if( !get_obj( ).child_List( ).Is_Variation( ) )
               variation = 0;
            else
               ++variation;

            if( get_obj( ).child_List( ).Type( ).get_key( ) == "group" )
               list_ext = ".glist";
            else if( get_obj( ).child_List( ).Type( ).get_key( ) == "non_group" )
               list_ext = ".nglist";
            else if( get_obj( ).child_List( ).Type( ).get_key( ) == "normal" )
               list_ext = ".list";
            else if( get_obj( ).child_List( ).Type( ).get_key( ) == "user" )
               list_ext = ".ulist";
            else if( get_obj( ).child_List( ).Type( ).get_key( ) == "non_user" )
               list_ext = ".nulist";

            if( variation && !list_ext.empty( ) )
               list_ext += to_string( variation );

            if( !list_ext.empty( ) )
            {
               pdf_vars_file = get_obj( ).Name( )
                + "_" + get_obj( ).child_List( ).Class( ).Name( ) + list_ext + ".pdf.vars.xrep";

               if( exists_file( pdf_vars_file ) )
                  remove_file( pdf_vars_file );

               if( get_obj( ).child_List( ).PDF_List_Type( ) != 0 )
                  get_obj( ).child_List( ).Generate_PDF_List( variation );
            }

            if( !get_obj( ).child_List( ).Display_Totals( ) )
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';
               list_extra += "no_totals";
            }

            if( !get_obj( ).child_List( ).Display_Sub_Totals( ) )
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';
               list_extra += "no_sub_totals";
            }

            if( get_obj( ).child_List( ).Print_Restriction( ) != 2 ) // i.e. denied_always
            {
               if( get_obj( ).child_List( ).Print_Without_Highlight( ) )
               {
                  if( !list_extra.empty( ) )
                     list_extra += ',';
                  list_extra += "print_no_highlight";
               }

               if( !get_obj( ).child_List( ).Multiline_Truncate_For_Print( ) )
               {
                  if( !list_extra.empty( ) )
                     list_extra += ',';
                  list_extra += "print_no_trunc";
               }
            }

            if( get_obj( ).child_List( ).Allow_Anonymous_Access( ) )
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';
               list_extra += "allow_anonymous";
            }

            if( get_obj( ).child_List( ).Ignore_Uneditable_Parent( ) )
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';
               list_extra += "ignore_parent_state";
            }

            if( get_obj( ).child_List( ).Ignore_State_For_Display( ) )
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';
               list_extra += "ignore_display_state";
            }

            if( get_obj( ).child_List( ).Ignore_Unactionable_Records( ) )
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';
               list_extra += "ignore_unactionable";
            }

            if( get_obj( ).child_List( ).File_Links_Always_As_Single( ) )
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';
               list_extra += "file_links_always_as_single";
            }

            if( get_obj( ).child_List( ).Create_Only_If_Default_Other( ) )
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';
               list_extra += "new_if_default_other";
            }

            if( get_obj( ).child_List( ).Display_Only_If_Default_Other( ) )
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';
               list_extra += "show_if_default_other";
            }

            if( get_obj( ).child_List( ).Destroy_Only_If_Default_Other( ) )
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';
               list_extra += "erase_if_default_other";
            }

            bool is_admin_list = false;
            bool is_owner_list = false;
            bool is_no_access_list = false;
            bool is_admin_owner_list = false;

            if( get_obj( ).child_List( ).Access_Restriction( ) != 0 )
            {
               if( get_obj( ).child_List( ).Access_Restriction( ) == 1 )
                  is_owner_list = true;
               else if( get_obj( ).child_List( ).Access_Restriction( ) == 2 )
                  is_admin_list = true;
               else if( get_obj( ).child_List( ).Access_Restriction( ) == 3 )
                  is_admin_owner_list = true;
               else if( get_obj( ).child_List( ).Access_Restriction( ) == 4 )
                  is_no_access_list = true;
               else
                  throw runtime_error( "unexpected Access_Restriction #"
                   + to_string( get_obj( ).child_List( ).Access_Restriction( ) ) + " in Model::Generate" );
            }

            if( !is_null( get_obj( ).child_List( ).Access_Parent_Modifier( ) ) )
            {
               Meta_Class* p_parent_Class = &get_obj( ).child_List( ).Parent_Class( );

               string modifier_key_info( to_string( Meta_Modifier::static_get_field_id( Meta_Modifier::e_field_id_Name ) ) + ' ' );

               if( p_parent_Class->child_Modifier( ).iterate_forwards( modifier_key_info ) )
               {
                  uint64_t flag_value( UINT64_C( 0x100 ) );

                  do
                  {
                     ostringstream osstr;

                     osstr << hex << flag_value;

                     flag_value <<= 1;

                     if( get_obj( ).child_List( ).Access_Parent_Modifier( ) == p_parent_Class->child_Modifier( ) )
                     {
                        if( !list_extra.empty( ) )
                           list_extra += ',';
                        list_extra += "pstate=" + osstr.str( );
                     }
                  } while( p_parent_Class->child_Modifier( ).iterate_next( ) );
               }
            }

            if( get_obj( ).child_List( ).Create_Restriction( ) != 0 )
            {
               if( get_obj( ).child_List( ).Create_Restriction( ) == 1 )
               {
                  if( !list_extra.empty( ) )
                     list_extra += ',';
                  list_extra += "owner_new";
               }
               else if( get_obj( ).child_List( ).Create_Restriction( ) == 2 )
               {
                  if( !list_extra.empty( ) )
                     list_extra += ',';
                  list_extra += "admin_new";
               }
               else if( get_obj( ).child_List( ).Create_Restriction( ) == 3 )
               {
                  if( !list_extra.empty( ) )
                     list_extra += ',';
                  list_extra += "admin_owner_new";
               }
               else if( get_obj( ).child_List( ).Create_Restriction( ) == 4 )
               {
                  if( !list_extra.empty( ) )
                     list_extra += ',';
                  list_extra += "no_new";
               }
               else
                  throw runtime_error( "unexpected Create_Restriction #"
                   + to_string( get_obj( ).child_List( ).Create_Restriction( ) ) + " in Model::Generate" );

               if( !is_null( get_obj( ).child_List( ).Create_Permission( ) ) )
                  list_extra += "=!" + get_obj( ).child_List( ).Create_Permission( ).Id( );
            }
            else if( !is_null( get_obj( ).child_List( ).Create_Permission( ) ) )
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';

               list_extra += "no_new=!" + get_obj( ).child_List( ).Create_Permission( ).Id( );
            }

            if( !is_null( get_obj( ).child_List( ).Create_Parent_Modifier( ) ) )
            {
               Meta_Class* p_parent_Class = &get_obj( ).child_List( ).Parent_Class( );

               string modifier_key_info( to_string( Meta_Modifier::static_get_field_id( Meta_Modifier::e_field_id_Name ) ) + ' ' );

               if( p_parent_Class->child_Modifier( ).iterate_forwards( modifier_key_info ) )
               {
                  uint64_t flag_value( UINT64_C( 0x100 ) );

                  do
                  {
                     ostringstream osstr;

                     osstr << hex << flag_value;

                     flag_value <<= 1;

                     if( get_obj( ).child_List( ).Create_Parent_Modifier( ) == p_parent_Class->child_Modifier( ) )
                     {
                        if( !list_extra.empty( ) )
                           list_extra += ',';

                        list_extra += "cpstate=" + osstr.str( );
                     }
                  } while( p_parent_Class->child_Modifier( ).iterate_next( ) );
               }
            }

            if( get_obj( ).child_List( ).Destroy_Restriction( ) != 0 )
            {
               if( get_obj( ).child_List( ).Destroy_Restriction( ) == 1 )
               {
                  if( !list_extra.empty( ) )
                     list_extra += ',';

                  list_extra += "owner_erase";
               }
               else if( get_obj( ).child_List( ).Destroy_Restriction( ) == 2 )
               {
                  if( !list_extra.empty( ) )
                     list_extra += ',';

                  list_extra += "admin_erase";
               }
               else if( get_obj( ).child_List( ).Destroy_Restriction( ) == 3 )
               {
                  if( !list_extra.empty( ) )
                     list_extra += ',';

                  list_extra += "admin_owner_erase";
               }
               else if( get_obj( ).child_List( ).Destroy_Restriction( ) == 4 )
               {
                  if( !list_extra.empty( ) )
                     list_extra += ',';

                  list_extra += "no_erase";
               }
               else
                  throw runtime_error( "unexpected Destroy_Restriction #"
                   + to_string( get_obj( ).child_List( ).Destroy_Restriction( ) ) + " in Model::Generate" );

               if( !is_null( get_obj( ).child_List( ).Destroy_Permission( ) ) )
                  list_extra += "=!" + get_obj( ).child_List( ).Destroy_Permission( ).Id( );
            }
            else if( !is_null( get_obj( ).child_List( ).Destroy_Permission( ) ) )
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';

               list_extra += "no_erase=!" + get_obj( ).child_List( ).Destroy_Permission( ).Id( );
            }

            if( !is_null( get_obj( ).child_List( ).Destroy_Parent_Modifier( ) ) )
            {
               Meta_Class* p_parent_Class = &get_obj( ).child_List( ).Parent_Class( );

               string modifier_key_info( to_string( Meta_Modifier::static_get_field_id( Meta_Modifier::e_field_id_Name ) ) + ' ' );

               if( p_parent_Class->child_Modifier( ).iterate_forwards( modifier_key_info ) )
               {
                  uint64_t flag_value( UINT64_C( 0x100 ) );

                  do
                  {
                     ostringstream osstr;

                     osstr << hex << flag_value;

                     flag_value <<= 1;

                     if( get_obj( ).child_List( ).Destroy_Parent_Modifier( ) == p_parent_Class->child_Modifier( ) )
                     {
                        if( !list_extra.empty( ) )
                           list_extra += ',';

                        list_extra += "dpstate=" + osstr.str( );
                     }
                  } while( p_parent_Class->child_Modifier( ).iterate_next( ) );
               }
            }

            if( get_obj( ).child_List( ).Limit_Scroll_And_New( ) )
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';

               list_extra += "fixed";
            }

            if( get_obj( ).child_List( ).Display_Row_Limit( ) != 0 )
            {
               if( !list_extra.empty( ) )
                  list_extra += ',';

               if( get_obj( ).child_List( ).Display_Row_Limit( ) == 999 ) // i.e. unlimited
               {
                  if( !get_obj( ).child_List( ).Sort_Rows_In_UI( ) )
                     list_extra += "no_limit";
                  else
                     list_extra += "sort_no_limit";
               }
               else
                  list_extra += "limit=" + to_string( get_obj( ).child_List( ).Display_Row_Limit( ) );
            }

            string pclass_id, pclass_name, pfield_id, pfield_name;

            vector< string > rfield_ids;

            string type_key( get_obj( ).child_List( ).Type( ).get_key( ) );

            if( type_key == "normal" || type_key == "additional" )
            {
               specification_name += "list";

               if( type_key == "additional" )
                  specification_name += "_add";

               if( is_admin_list )
                  list_type = "admin";
               else if( is_owner_list )
                  throw runtime_error( "'owner' access is only applicable to 'view_child' lists" );
               else if( is_no_access_list )
                  list_type = "no_access";
               else if( is_admin_owner_list )
                  throw runtime_error( "'admin_owner' access is only applicable to 'view_child' lists" );
            }
            else if( type_key == "home" )
            {
               list_type = "home";

               if( is_no_access_list )
               {
                  if( get_obj( ).child_List( ).Allow_Anonymous_Access( ) )
                     list_type = "home_anon";
                  else
                     throw runtime_error( "'home' list is not compatible with this access restriction (unless allowing anonymous access)" );
               }

               specification_name += "home_list";

               if( !list_extra.empty( ) )
                  list_extra += ',';

               list_extra += "fixed,no_new,no_erase";
            }
            else if( type_key == "admin" )
            {
               list_type = "admin";
               specification_name += "admin_list";
            }
            else if( type_key == "user" || type_key == "non_user" )
            {
               if( type_key == "user" )
               {
                  list_type = "user";
                  specification_name += "user_list";
               }
               else
               {
                  list_type = "nonuser";
                  specification_name += "nonuser_list";
               }

               // NOTE: Special allows a list to be generated but inaccessible if is "user" and "admin_only".
               // KLUDGE: As 'denied_always' access is now used for this purpose this isn't needed but as it
               // has been used in one or more packages these must be changed before changing this code.
               if( is_admin_list || is_no_access_list )
                  list_type = "no_access";
               else if( is_admin_owner_list )
                  throw runtime_error( "'admin_owner' access is only applicable to 'view_child' lists" );

               if( get_obj( ).child_List( ).Class( ).child_Relationship_Child( ).iterate_forwards( ) )
               {
                  do
                  {
                     if( get_obj( ).child_List( ).Class( ).child_Relationship_Child( ).Extra( ) == 1 )
                     {
                        if( !pclass_id.empty( ) )
                           throw runtime_error( "Multiple 'owner' Relationships found for "
                            + get_obj( ).child_List( ).Class( ).Name( ) );

                        pclass_id = get_obj( ).child_List( ).Class( ).child_Relationship_Child( ).Parent_Class( ).Id( );
                        pclass_name = get_obj( ).child_List( ).Class( ).child_Relationship_Child( ).Parent_Class( ).Name( );
                        pfield_id = get_obj( ).child_List( ).Class( ).child_Relationship_Child( ).Field_Id( );
                        pfield_name = get_obj( ).child_List( ).Class( ).child_Relationship_Child( ).Name( );
                     }
                  } while( get_obj( ).child_List( ).Class( ).child_Relationship_Child( ).iterate_next( ) );
               }

               if( pclass_id.empty( ) )
                  throw runtime_error( "No 'owner' Relationship found for "
                   + get_obj( ).child_List( ).Class( ).Name( ) + " when processing '" + get_obj( ).child_List( ).Name( ) + "'" );
            }
            else if( type_key == "group" || type_key == "non_group" )
            {
               if( type_key == "group" )
               {
                  list_type = "group";
                  specification_name += "group_list";
               }
               else
               {
                  list_type = "nongroup";
                  specification_name += "list";
               }

               if( is_no_access_list )
                  list_type = "no_access";
               else if( is_admin_list )
                  throw runtime_error( "'admin' access is not applicable to 'group' lists" );
               else if( is_owner_list )
                  throw runtime_error( "'owner' access is only applicable to 'view_child' lists" );
               else if( is_admin_owner_list )
                  throw runtime_error( "'admin_owner' access is only applicable to 'view_child' lists" );

               if( get_obj( ).child_List( ).Class( ).child_Relationship_Child( ).iterate_forwards( ) )
               {
                  do
                  {
                     if( get_obj( ).child_List( ).Class( ).child_Relationship_Child( ).Extra( ) == 2 )
                     {
                        if( !pclass_id.empty( ) )
                           throw runtime_error( "Multiple 'user_group' Relationships found for "
                            + get_obj( ).child_List( ).Class( ).Name( ) );

                        pclass_id = get_obj( ).child_List( ).Class( ).child_Relationship_Child( ).Parent_Class( ).Id( );
                        pclass_name = get_obj( ).child_List( ).Class( ).child_Relationship_Child( ).Parent_Class( ).Name( );
                        pfield_id = get_obj( ).child_List( ).Class( ).child_Relationship_Child( ).Field_Id( );
                        pfield_name = get_obj( ).child_List( ).Class( ).child_Relationship_Child( ).Name( );
                     }
                  } while( get_obj( ).child_List( ).Class( ).child_Relationship_Child( ).iterate_next( ) );
               }

               if( pclass_id.empty( ) )
                  throw runtime_error( "No 'user_group' Relationship found for "
                   + get_obj( ).child_List( ).Class( ).Name( ) + " when processing '" + get_obj( ).child_List( ).Name( ) + "'" );
            }
            else if( type_key == "view_child" )
            {
               list_type = "child";
               specification_name += "child_list";

               if( is_owner_list )
                  list_type = "child_owner";
               else if( is_admin_list )
                  list_type = "child_admin";
               else if( is_admin_owner_list )
                  list_type = "child_admin_owner";

               if( is_no_access_list )
                  throw runtime_error( "'view_child' list is not compatible with this access restriction" );

               pclass_id = get_obj( ).child_List( ).Parent_Field( ).Parent_Class( ).Id( );
               pclass_name = get_obj( ).child_List( ).Parent_Field( ).Parent_Class( ).Name( );
               pfield_id = get_obj( ).child_List( ).Parent_Field( ).Id( );
               pfield_name = get_obj( ).child_List( ).Parent_Field( ).Name( );

               int num = 0;
               if( !child_lists.count( get_obj( ).child_List( ).Class( ).Id( ) ) )
                  child_lists[ get_obj( ).child_List( ).Class( ).Id( ) ] = num;
               else
                  num = ++child_lists[ get_obj( ).child_List( ).Class( ).Id( ) ];

               specification_name += to_string( num );
            }

            if( variation )
               specification_name += "_var" + to_string( variation );

            if( !is_null( get_obj( ).child_List( ).Access_Permission( ) ) )
            {
               string s;

               if( ( list_type == "admin" ) || ( is_admin_list && ( list_type == "view_child" ) ) )
                  s = "!";

               list_type += "=" + s + get_obj( ).child_List( ).Access_Permission( ).Id( );
            }

            outf << "\x60{\x60}\n";

            outf << "\x60{\x60$specification_" << specification_name
             + "_id\x60=\x60'" << get_obj( ).child_List( ).Id( ) << "\x60'\x60}\n";

            outf << "\x60{\x60$specification_" << specification_name
             + "_cid\x60=\x60'" << get_mapped_id( model_name, get_obj( ).child_List( ).Class( ).Id( ) ) << "\x60'\x60}\n";

            string title( search_replace( get_obj( ).child_List( ).Title( ), "_", " " ) );

            string::size_type pos = 0;

            while( true )
            {
               pos = title.find_first_of( "?*^%#$" );

               if( pos == string::npos )
                  break;

               if( title.substr( pos, 1 ) == "?" )
                  title.insert( pos + 1, search_replace( get_obj( ).child_List( ).Class( ).Name( ), "_", " " ) );
               else if( title.substr( pos, 1 ) == "*" )
                  title.insert( pos + 1, search_replace( get_obj( ).child_List( ).Class( ).Plural( ), "_", " " ) );
               else if( title.substr( pos, 1 ) == "^" )
                  title.insert( pos + 1, search_replace( pfield_name, "_", " " ) );
               else if( title.substr( pos, 1 ) == "%" )
                  title.insert( pos + 1, search_replace( pclass_name, "_", " " ) );
               else if( title.substr( pos, 1 ) == "#" )
                  title.insert( pos + 1, search_replace( user_group_name, "_", " " ) );
               else if( title.substr( pos, 1 ) == "$" )
                  title.insert( pos + 1, search_replace( user_other_name, "_", " " ) );

               title.erase( pos, 1 );
            }

            string id( get_obj( ).child_List( ).Class( ).Id( ) );

            all_class_strings[ id ].push_back( get_obj( ).child_List( ).Id( ) + "_name \"" + title + "\"" );

            outf << "\x60{\x60$specification_" << specification_name + "_name\x60=\x60'" << title << "\x60'\x60}\n";
            outf << "\x60{\x60$specification_" << specification_name + "_type\x60=\x60'" << list_type << "\x60'\x60}\n";

            vector< string > field_keys;
            vector< string > column_ids;
            vector< string > column_pids;
            vector< string > column_names;
            vector< string > column_types;
            vector< string > column_extras;
            vector< string > column_modifiers;

            string new_select_class_id;
            string new_select_field_id;
            string new_select_dfield_id;
            string new_select_dfenum_id;
            string new_select_extras;

            vector< string > restrict_field_ids;
            vector< string > restrict_field_names;
            vector< string > restrict_field_types;
            vector< string > restrict_field_operations;
            vector< pair< bool, string > > restrict_field_other;

            vector< string > parent_column_ids;
            vector< string > parent_column_names;
            vector< string > parent_column_extras;
            vector< string > parent_column_dextras;
            vector< string > parent_column_class_ids;
            vector< string > parent_column_operations;
            vector< string > parent_column_class_names;
            vector< string > parent_column_child_field_ids;
            vector< int > parent_column_other_info;

            map< string, string > parent_column_name_overrides;

            map< string, pair< bool, bool > > column_index_info;

            map< string, pair< string, string > > column_parent_fields;
            map< string, pair< string, string > > column_parent_classes;

            string actions, filters;

            string list_field_key_info( to_string(
             Meta_List_Field::static_get_field_id( Meta_List_Field::e_field_id_Order ) ) + ' ' );

            if( get_obj( ).child_List( ).child_List_Field( ).iterate_forwards( list_field_key_info ) )
            {
               // NOTE: If the Class is "ordered" (and the list is neither set to order the rows in the UI or to
               // ignore implicit ordering) then automatically add the Class's "order" field as a "hidden" field.
               if( !get_obj( ).child_List( ).Sort_Rows_In_UI( )
                && !get_obj( ).child_List( ).Ignore_Implicit_Ordering( )
                && get_obj( ).child_List( ).Class( ).Extra( ) == 1 ) // i.e. "ordered"
               {
                  if( get_obj( ).child_List( ).Class( ).child_Field( ).iterate_forwards( ) )
                  {
                     do
                     {
                        if( get_obj( ).child_List( ).Class( ).child_Field( ).Extra( ) == 5 ) // i.e. "order"
                        {
                           field_keys.push_back( "" );

                           column_ids.push_back( get_obj( ).child_List( ).Class( ).child_Field( ).Id( ) );
                           column_names.push_back( get_obj( ).child_List( ).Class( ).Name( )
                            + '_' + get_obj( ).child_List( ).Class( ).child_Field( ).Name( ) );

                           column_pids.push_back( "" );

                           column_types.push_back( meta_field_type_name(
                            get_obj( ).child_List( ).Class( ).child_Field( ).Primitive( ),
                            get_obj( ).child_List( ).Class( ).child_Field( ).Mandatory( ), "", "" ) );

                           column_extras.push_back( "hidden+order" );

                           column_modifiers.push_back( "" );

                           column_index_info.insert(
                            make_pair( get_obj( ).child_List( ).Class( ).child_Field( ).Id( ), make_pair( false, false ) ) );

                           get_obj( ).child_List( ).Class( ).child_Field( ).iterate_stop( );
                           break;
                        }
                     } while( get_obj( ).child_List( ).Class( ).child_Field( ).iterate_next( ) );
                  }
               }

               do
               {
                  string extras;

                  if( get_obj( ).child_List( ).child_List_Field( ).Access_Restriction( ) != 0 )
                  {
                     if( get_obj( ).child_List( ).child_List_Field( ).Access_Restriction( ) == 1 )
                        extras = "owner_only";
                     else if( get_obj( ).child_List( ).child_List_Field( ).Access_Restriction( ) == 2 )
                        extras = "admin_only";
                     else if( get_obj( ).child_List( ).child_List_Field( ).Access_Restriction( ) == 3 )
                        extras = "admin_owner_only";
                     else if( get_obj( ).child_List( ).child_List_Field( ).Access_Restriction( ) == 4 )
                        extras = "hidden";
                     else
                        throw runtime_error( "unexpected Access_Restriction value #"
                         + to_string( get_obj( ).child_List( ).child_List_Field( ).Access_Restriction( ) )
                         + " in Model::Generate" );

                     if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Access_Permission( ) ) )
                        extras += "=!" + get_obj( ).child_List( ).child_List_Field( ).Access_Permission( ).Id( );
                  }
                  else if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Access_Permission( ) ) )
                     extras += "hidden=!" + get_obj( ).child_List( ).child_List_Field( ).Access_Permission( ).Id( );

                  if( !get_obj( ).child_List( ).child_List_Field( ).Allow_Anonymous_Access( ) )
                  {
                     if( !extras.empty( ) )
                        extras += '+';
                     extras += "no_anon";
                  }

                  switch( get_obj( ).child_List( ).child_List_Field( ).Link_Empty_Restriction( ) )
                  {
                     case 0: // i.e. none
                     break;

                     case 1: // i.e. owner_only
                     {
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "link_none_owner_only";
                        break;
                     }

                     case 2: // i.e. admin_only
                     {
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "link_none_admin_only";
                        break;
                     }

                     case 3: // i.e. admin_owner
                     {
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "link_none_admin_owner";
                        break;
                     }

                     case 4: // i.e. denied_always
                     {
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "link_none_denied_always";
                        break;
                     }

                     default:
                     throw runtime_error( "unexpected Link_Empty_Restriction value #"
                      + to_string( get_obj( ).child_List( ).child_List_Field( ).Link_Empty_Restriction( ) )
                      + " in Model::Generate" );
                  }

                  if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Access_Parent_Modifier( ) ) )
                  {
                     Meta_Class* p_parent_Class = &get_obj( ).child_List( ).child_List_Field( ).Parent_Class( );

                     string modifier_key_info( to_string(
                      Meta_Modifier::static_get_field_id( Meta_Modifier::e_field_id_Name ) ) + ' ' );

                     if( p_parent_Class->child_Modifier( ).iterate_forwards( modifier_key_info ) )
                     {
                        uint64_t flag_value( UINT64_C( 0x100 ) );

                        do
                        {
                           ostringstream osstr;

                           osstr << hex << flag_value;

                           flag_value <<= 1;

                           if( get_obj( ).child_List( ).child_List_Field( ).Access_Parent_Modifier( ) == p_parent_Class->child_Modifier( ) )
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "pstate=" + osstr.str( );
                           }
                        } while( p_parent_Class->child_Modifier( ).iterate_next( ) );
                     }
                  }

                  if( get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "field" )
                  {
                     if( extras != "hidden" )
                     {
                        switch( get_obj( ).child_List( ).child_List_Field( ).Link_Restriction( ) )
                        {
                           case 0: // i.e. none
                           {
                              if( get_obj( ).child_List( ).child_List_Field( ).Link_Type( ) == 0 )
                              {
                                 if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ) ) )
                                 {
                                    if( !extras.empty( ) )
                                       extras += '+';

                                    extras += "link";
                                 }
                              }
                              else if( get_obj( ).child_List( ).child_List_Field( ).Link_Type( ) == 1 )
                              {
                                 if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ) ) )
                                 {
                                    if( !extras.empty( ) )
                                       extras += '+';

                                    extras += "non_fk_link";
                                 }
                              }
                              else
                                 throw runtime_error( "unexpected Link_Type value #"
                                  + to_string( get_obj( ).child_List( ).child_List_Field( ).Link_Type( ) ) + " in Model::Generate" );

                              if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Link_Permission( ) ) )
                              {
                                 if( !extras.empty( ) )
                                    extras += '+';

                                 extras += "non_link";
                              }

                              break;
                           }

                           case 1: // i.e. owner_only
                           {
                              if( get_obj( ).child_List( ).child_List_Field( ).Link_Type( ) == 0 )
                                 ; // i.e. do nothing here
                              else if( get_obj( ).child_List( ).child_List_Field( ).Link_Type( ) == 1 )
                              {
                                 if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ) ) )
                                 {
                                    if( !extras.empty( ) )
                                       extras += '+';

                                    extras += "non_fk_link";
                                 }
                              }
                              else
                                 throw runtime_error( "unexpected Link_Type value #"
                                  + to_string( get_obj( ).child_List( ).child_List_Field( ).Link_Type( ) ) + " in Model::Generate" );

                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "owner_link";

                              break;
                           }

                           case 2: // i.e. admin_only
                           {
                              if( get_obj( ).child_List( ).child_List_Field( ).Link_Type( ) == 0 )
                                 ; // i.e. do nothing here
                              else if( get_obj( ).child_List( ).child_List_Field( ).Link_Type( ) == 1 )
                              {
                                 if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ) ) )
                                 {
                                    if( !extras.empty( ) )
                                       extras += '+';

                                    extras += "non_fk_link";
                                 }
                              }
                              else
                                 throw runtime_error( "unexpected Link_Type value #"
                                  + to_string( get_obj( ).child_List( ).child_List_Field( ).Link_Type( ) ) + " in Model::Generate" );

                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "admin_link";

                              break;
                           }

                           case 3: // i.e. admin_owner_only
                           {
                              if( get_obj( ).child_List( ).child_List_Field( ).Link_Type( ) == 0 )
                                 ; // i.e. do nothing here
                              else if( get_obj( ).child_List( ).child_List_Field( ).Link_Type( ) == 1 )
                              {
                                 if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ) ) )
                                 {
                                    if( !extras.empty( ) )
                                       extras += '+';

                                    extras += "non_fk_link";
                                 }
                              }
                              else
                                 throw runtime_error( "unexpected Link_Type value #"
                                  + to_string( get_obj( ).child_List( ).child_List_Field( ).Link_Type( ) ) + " in Model::Generate" );

                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "admin_owner_link";

                              break;
                           }

                           case 4: // i.e. denied_always
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "non_link";

                              break;
                           }

                           default:
                           throw runtime_error( "unexpected Link_Restriction value #"
                            + to_string( get_obj( ).child_List( ).child_List_Field( ).Link_Restriction( ) )
                            + " in Model::Generate" );
                        }

                        if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Link_Permission( ) ) )
                           extras += "=!" + get_obj( ).child_List( ).child_List_Field( ).Link_Permission( ).Id( );

                        switch( get_obj( ).child_List( ).child_List_Field( ).Font_Size( ) )
                        {
                           case 0: // i.e. normal
                           break;

                           case 1:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "large";

                              break;
                           }

                           case 2:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "larger";

                              break;
                           }

                           case 5:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "small";

                              break;
                           }

                           case 6:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "smaller";

                              break;
                           }

                           default:
                           throw runtime_error( "unexpected Font_Size value #"
                            + to_string( get_obj( ).child_List( ).child_List_Field( ).Font_Size( ) )
                            + " in Model::Generate" );
                        }

                        switch( get_obj( ).child_List( ).child_List_Field( ).Alignment( ) )
                        {
                           case 0: // i.e. default
                           break;

                           case 1:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "left";

                              break;
                           }

                           case 2:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "right";

                              break;
                           }

                           case 3:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "center";

                              break;
                           }

                           case 4:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "justify";

                              break;
                           }

                           default:
                           throw runtime_error( "unexpected Alignment value #"
                            + to_string( get_obj( ).child_List( ).child_List_Field( ).Alignment( ) )
                            + " in Model::Generate" );
                        }

                        if( get_obj( ).child_List( ).child_List_Field( ).Source_Field( ).Extra( ) == 3 ) // i.e. image
                        {
                           switch( get_obj( ).child_List( ).child_List_Field( ).Orientation( ) )
                           {
                              case 0:
                              {
                                 if( !extras.empty( ) )
                                    extras += '+';

                                 extras += "landscape";

                                 break;
                              }

                              case 1:
                              {
                                 if( !extras.empty( ) )
                                    extras += '+';

                                 extras += "portrait";

                                 break;
                              }

                              case 2:
                              {
                                 if( !extras.empty( ) )
                                    extras += '+';

                                 extras += "neither";

                                 break;
                              }

                              default:
                              throw runtime_error( "unexpected Orientation value #"
                               + to_string( get_obj( ).child_List( ).child_List_Field( ).Orientation( ) )
                               + " in Model::Generate" );
                           }
                        }

                        switch( get_obj( ).child_List( ).child_List_Field( ).Print_Type( ) )
                        {
                           case 0:
                           break;

                           case 1:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "non_print";

                              break;
                           }

                           case 2:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "print_only";

                              break;
                           }

                           case 3:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "print_total";

                              break;
                           }

                           case 4:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "print_summary";

                              break;
                           }

                           case 5:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "print_only_total";

                              break;
                           }

                           case 6:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "print_only_summary";

                              break;
                           }

                           default:
                           throw runtime_error( "unexpected Print_Type value #"
                            + to_string( get_obj( ).child_List( ).child_List_Field( ).Print_Type( ) )
                            + " in Model::Generate" );
                        }

                        switch( get_obj( ).child_List( ).child_List_Field( ).Notes_Truncation( ) )
                        {
                           case 0:
                           break;

                           default:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "trunc=" + to_string( get_obj( ).child_List( ).child_List_Field( ).Notes_Truncation( ) );
                           }
                        }

                        switch( get_obj( ).child_List( ).child_List_Field( ).Label_Source( ) )
                        {
                           case 0:
                           break;

                           case 1:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "use_list_title";

                              break;
                           }

                           case 2:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "omit_label";

                              break;
                           }

                           default:
                           throw runtime_error( "unexpected Label_Source value #"
                            + to_string( get_obj( ).child_List( ).child_List_Field( ).Label_Source( ) )
                            + " in Model::Generate" );
                        }

                        switch( get_obj( ).child_List( ).child_List_Field( ).Label_Source_Child( ) )
                        {
                           case 0: // i.e. default
                           break;

                           case 1:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "non_prefixed";

                              break;
                           }

                           case 2:
                           {
                              if( !extras.empty( ) )
                                 extras += '+';

                              extras += "child_always";

                              break;
                           }

                           default:
                           throw runtime_error( "unexpected Label_Source_Child value #"
                            + to_string( get_obj( ).child_List( ).child_List_Field( ).Label_Source_Child( ) )
                            + " in Model::Generate" );
                        }
                     }

                     if( get_obj( ).child_List( ).child_List_Field( ).Use_In_Text_Search_Title( ) )
                     {
                        if( !extras.empty( ) )
                           extras += '+';
                        extras += "text_search_title";
                     }

                     field_keys.push_back( get_obj( ).child_List( ).child_List_Field( ).get_key( ) );

                     bool is_fk_field = false;
                     Meta_Field* p_list_field = 0;
                     Meta_Field* p_modifier_field = 0;

                     if( is_null( get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ) ) )
                     {
                        p_list_field = p_modifier_field = &get_obj( ).child_List( ).child_List_Field( ).Source_Field( );

                        if( p_list_field->Id( ) == p_list_field->Class( ).Quick_Link_Field( ).Id( ) )
                        {
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += "quick_link";
                        }

                        column_ids.push_back( get_obj( ).child_List( ).child_List_Field( ).Source_Field( ).Id( ) );
                        column_names.push_back( get_obj( ).child_List( ).Class( ).Name( )
                         + '_' + get_obj( ).child_List( ).child_List_Field( ).Source_Field( ).Name( ) );

                        column_pids.push_back( "" );

                        Meta_Field* p_type_field = &get_obj( ).child_List( ).child_List_Field( ).Source_Field( );

                        if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Use_Type_Field( ) ) )
                           p_type_field = &get_obj( ).child_List( ).child_List_Field( ).Use_Type_Field( );

                        column_types.push_back( meta_field_type_name(
                         p_type_field->Primitive( ), p_type_field->Mandatory( ), "", "" ) );

                        string other_extras( meta_field_extras(
                         p_type_field->UOM( ),
                         p_type_field->UOM_Name( ),
                         p_type_field->Extra( ),
                         p_type_field->Encrypted( ),
                         p_type_field->Transient( ),
                         p_type_field->Type( ).Max_Size( ),
                         p_type_field->Enum( ).Id( ),
                         p_type_field->Enum_Filter( ).Id( ),
                         p_type_field->Type( ).Primitive( ),
                         p_type_field->Type( ).Min_Value( ),
                         p_type_field->Type( ).Max_Value( ),
                         p_type_field->Type( ).Numeric_Digits( ),
                         p_type_field->Type( ).Numeric_Decimals( ),
                         p_type_field->Type( ).String_Domain( ),
                         p_type_field->Type( ).Date_Precision( ),
                         p_type_field->Type( ).Time_Precision( ),
                         p_type_field->Type( ).Show_Plus_Sign( ),
                         p_type_field->Type( ).Zero_Padding( ),
                         p_type_field->Type( ).Int_Type( ),
                         p_type_field->Type( ).Numeric_Type( ) ) );

                        if( !other_extras.empty( ) )
                        {
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += other_extras;
                        }

                        column_extras.push_back( extras );

                        // NOTE: Index determination will be made after all columns have been processed
                        // so that unique indexes that span multiple columns can be used as well as for
                        // handling a "restrict_field" to exist in the index before any sortable column
                        // even if it appears later in the field list. The dummy insert is performed so
                        // that only non-fk columns will be considered for sorting.
                        if( !get_obj( ).child_List( ).child_List_Field( ).Source_Field( ).Transient( ) )
                           column_index_info.insert(
                            make_pair( get_obj( ).child_List( ).child_List_Field( ).Source_Field( ).Id( ),
                            make_pair( false, false ) ) );
                     }
                     else
                     {
                        p_list_field = &get_obj( ).child_List( ).child_List_Field( ).Source_Child( );
                        p_modifier_field = &get_obj( ).child_List( ).child_List_Field( ).Source_Parent( );

                        is_fk_field = true;
                        column_ids.push_back( get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ).Id( ) );

                        string other_extras;

                        if( is_null( get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ) ) )
                        {
                           column_names.push_back( get_obj( ).child_List( ).Class( ).Name( )
                            + '_' + get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ).Name( ) );

                           column_pids.push_back( "" );

                           Meta_Field* p_type_field = &get_obj( ).child_List( ).child_List_Field( ).Source_Child( );

                           if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Use_Type_Field( ) ) )
                              p_type_field = &get_obj( ).child_List( ).child_List_Field( ).Use_Type_Field( );

                           column_types.push_back( meta_field_type_name(
                            p_type_field->Primitive( ), p_type_field->Mandatory( ), "", "" ) );

                           other_extras = meta_field_extras(
                            p_type_field->UOM( ),
                            p_type_field->UOM_Name( ),
                            p_type_field->Extra( ),
                            p_type_field->Encrypted( ),
                            p_type_field->Transient( ),
                            p_type_field->Type( ).Max_Size( ),
                            p_type_field->Enum( ).Id( ),
                            p_type_field->Enum_Filter( ).Id( ),
                            p_type_field->Type( ).Primitive( ),
                            p_type_field->Type( ).Min_Value( ),
                            p_type_field->Type( ).Max_Value( ),
                            p_type_field->Type( ).Numeric_Digits( ),
                            p_type_field->Type( ).Numeric_Decimals( ),
                            p_type_field->Type( ).String_Domain( ),
                            p_type_field->Type( ).Date_Precision( ),
                            p_type_field->Type( ).Time_Precision( ),
                            p_type_field->Type( ).Show_Plus_Sign( ),
                            p_type_field->Type( ).Zero_Padding( ),
                            p_type_field->Type( ).Int_Type( ),
                            p_type_field->Type( ).Numeric_Type( ), true );

                           if( !get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ).Transient( ) )
                              column_index_info.insert(
                               make_pair( get_obj( ).child_List( ).child_List_Field( ).get_key( ), make_pair( false, false ) ) );

                           column_parent_fields.insert(
                            make_pair( get_obj( ).child_List( ).child_List_Field( ).get_key( ),
                            make_pair( get_obj( ).child_List( ).child_List_Field( ).Source_Child( ).Id( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Child( ).Name( ) ) ) );

                           column_parent_classes.insert(
                            make_pair( get_obj( ).child_List( ).child_List_Field( ).get_key( ),
                            make_pair( get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ).Parent_Class( ).Id( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ).Parent_Class( ).Name( ) ) ) );
                        }
                        else
                        {
                           column_names.push_back(
                            get_obj( ).child_List( ).child_List_Field( ).Source_Child( ).Class( ).Name( )
                            + '_' + get_obj( ).child_List( ).child_List_Field( ).Source_Child( ).Name( ) );

                           column_pids.push_back( get_obj( ).child_List( ).child_List_Field( ).Source_Child( ).Id( ) );

                           column_types.push_back( meta_field_type_name(
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Primitive( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Mandatory( ), "", "" ) );

                           other_extras = meta_field_extras(
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).UOM( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).UOM_Name( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Extra( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Encrypted( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Transient( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Type( ).Max_Size( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Enum( ).Id( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Enum_Filter( ).Id( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Type( ).Primitive( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Type( ).Min_Value( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Type( ).Max_Value( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Type( ).Numeric_Digits( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Type( ).Numeric_Decimals( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Type( ).String_Domain( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Type( ).Date_Precision( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Type( ).Time_Precision( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Type( ).Show_Plus_Sign( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Type( ).Zero_Padding( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Type( ).Int_Type( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Type( ).Numeric_Type( ), true );

                           if( !get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ).Transient( ) )
                              column_index_info.insert(
                               make_pair( get_obj( ).child_List( ).child_List_Field( ).get_key( ), make_pair( false, false ) ) );

                           column_parent_fields.insert(
                            make_pair( get_obj( ).child_List( ).child_List_Field( ).get_key( ),
                            make_pair( get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Id( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Name( ) ) ) );

                           column_parent_classes.insert(
                            make_pair( get_obj( ).child_List( ).child_List_Field( ).get_key( ),
                            make_pair( get_obj( ).child_List( ).child_List_Field( ).Source_Child( ).Parent_Class( ).Id( ),
                            get_obj( ).child_List( ).child_List_Field( ).Source_Child( ).Parent_Class( ).Name( ) ) ) );
                        }

                        if( !other_extras.empty( ) )
                        {
                           if( !extras.empty( ) )
                              extras += '+';
                           extras += other_extras;
                        }

                        column_extras.push_back( extras );
                     }

                     string modifiers;

                     string modifier_key_info( to_string(
                      Meta_Modifier::static_get_field_id( Meta_Modifier::e_field_id_Name ) ) + ' ' );

                     if( p_modifier_field->Class( ).child_Modifier( ).iterate_forwards( modifier_key_info ) )
                     {
                        uint64_t flag_value( UINT64_C( 0x100 ) );

                        do
                        {
                           ostringstream osstr;

                           osstr << hex << flag_value;

                           flag_value <<= 1;

                           if( p_modifier_field->Class( ).child_Modifier( ).child_Modifier_Affect( ).iterate_forwards( ) )
                           {
                              do
                              {
                                 if( p_modifier_field->Class( ).child_Modifier( ).child_Modifier_Affect( ).Scope( ) == 0
                                  && p_modifier_field->Id( ) == p_modifier_field->Class( ).child_Modifier( ).child_Modifier_Affect( ).Field( ).Id( )
                                  && ( p_modifier_field->Class( ).child_Modifier( ).child_Modifier_Affect( ).Extra( ) == 0
                                  || p_modifier_field->Class( ).child_Modifier( ).child_Modifier_Affect( ).Extra( ) == 1 ) )
                                 {
                                    if( !modifiers.empty( ) )
                                       modifiers += ',';

                                    modifiers += osstr.str( ) + ':';

                                    int type = p_modifier_field->Class( ).child_Modifier( ).child_Modifier_Affect( ).Type( );
                                    switch( type )
                                    {
                                       case 0:
                                       modifiers += "protect";
                                       break;

                                       case 1:
                                       modifiers += "relegate";
                                       break;

                                       case 2:
                                       modifiers += "lowlight";
                                       break;

                                       case 3:
                                       modifiers += "lowlight1";
                                       break;

                                       case 4:
                                       modifiers += "highlight";
                                       break;

                                       case 5:
                                       modifiers += "highlight1";
                                       break;

                                       case 6:
                                       modifiers += "extralight";
                                       break;

                                       case 7:
                                       modifiers += "extralight1";
                                       break;

                                       default:
                                       throw runtime_error( "unknown modifier type #" + to_string( type ) + " in Model::Generate" );
                                    }
                                 }
                              } while( p_modifier_field->Class( ).child_Modifier( ).child_Modifier_Affect( ).iterate_next( ) );
                           }
                        } while( p_modifier_field->Class( ).child_Modifier( ).iterate_next( ) );
                     }

                     column_modifiers.push_back( modifiers );
                  }
                  else if( get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "row_number" )
                  {
                     string id( "@row" );

                     field_keys.push_back( get_obj( ).child_List( ).child_List_Field( ).get_key( ) );

                     column_ids.push_back( id );
                     column_pids.push_back( "" );
                     column_names.push_back( "row" );
                     column_types.push_back( "string" );
                     column_extras.push_back( "" );
                     column_modifiers.push_back( "" );
                     column_index_info.insert( make_pair( id, make_pair( false, false ) ) );
                  }
                  else if( ( ( get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "link_select"
                   || get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "restrict_select"
                   || get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "restrict_select_child_rel" )
                   && is_null( get_obj( ).child_List( ).child_List_Field( ).Source_Field( ) ) )
                   || ( get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "restrict_switch"
                   && !get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ).get_key( ).empty( ) ) )
                  {
                     Meta_Field* p_field;

                     bool is_mandatory = false;

                     string dextras( extras );

                     switch( get_obj( ).child_List( ).child_List_Field( ).Font_Size( ) )
                     {
                        case 0: // i.e. normal
                        break;

                        case 1:
                        {
                           if( !dextras.empty( ) )
                              dextras += '+';

                           dextras += "large";

                           break;
                        }

                        case 2:
                        {
                           if( !dextras.empty( ) )
                              dextras += '+';

                           dextras += "larger";

                           break;
                        }

                        case 5:
                        {
                           if( !dextras.empty( ) )
                              dextras += '+';

                           dextras += "small";

                           break;
                        }

                        case 6:
                        {
                           if( !dextras.empty( ) )
                              dextras += '+';

                           dextras += "smaller";

                           break;
                        }

                        default:
                        throw runtime_error( "unexpected Font_Size value #"
                         + to_string( get_obj( ).child_List( ).child_List_Field( ).Font_Size( ) )
                         + " in Model::Generate" );
                     }

                     if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Child_Rel_Source_Field( ) ) )
                     {
                        p_field = &get_obj( ).child_List( ).child_List_Field( ).Child_Rel_Source_Field( );
                        is_mandatory = get_obj( ).child_List( ).child_List_Field( ).Child_Rel_Source_Field( ).Mandatory( );
                        parent_column_ids.push_back( get_obj( ).child_List( ).child_List_Field( ).Child_Relationship( ).Field_Id( )
                         + "." + get_obj( ).child_List( ).child_List_Field( ).Child_Rel_Source_Field( ).Id( ) );
                        parent_column_names.push_back( get_obj( ).child_List( ).child_List_Field( ).Child_Rel_Source_Field( ).Name( ) );
                        parent_column_class_ids.push_back( get_obj( ).child_List( ).child_List_Field( ).Child_Rel_Source_Field( ).Parent_Class( ).Id( ) );
                        parent_column_class_names.push_back( get_obj( ).child_List( ).child_List_Field( ).Child_Rel_Source_Field( ).Parent_Class( ).Name( ) );
                        parent_column_child_field_ids.push_back( "" );

                        parent_column_name_overrides.insert( make_pair( parent_column_ids.back( ), "plural_"
                         + lower( get_obj( ).child_List( ).child_List_Field( ).Child_Relationship( ).Child_Class( ).Name( ) ) ) );
                     }
                     else if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Child_Rel_Source_Parent( ) ) )
                     {
                        p_field = &get_obj( ).child_List( ).child_List_Field( ).Child_Rel_Source_Child( );
                        is_mandatory = get_obj( ).child_List( ).child_List_Field( ).Child_Rel_Source_Parent( ).Mandatory( );
                        parent_column_ids.push_back( get_obj( ).child_List( ).child_List_Field( ).Child_Relationship( ).Field_Id( )
                         + "." + get_obj( ).child_List( ).child_List_Field( ).Child_Rel_Source_Parent( ).Id( ) );
                        parent_column_names.push_back( get_obj( ).child_List( ).child_List_Field( ).Child_Rel_Source_Parent( ).Name( ) );
                        parent_column_class_ids.push_back( get_obj( ).child_List( ).child_List_Field( ).Child_Rel_Source_Parent( ).Parent_Class( ).Id( ) );
                        parent_column_class_names.push_back( get_obj( ).child_List( ).child_List_Field( ).Child_Rel_Source_Parent( ).Parent_Class( ).Name( ) );
                        parent_column_child_field_ids.push_back( get_obj( ).child_List( ).child_List_Field( ).Child_Rel_Source_Child( ).Id( ) );

                        parent_column_name_overrides.insert( make_pair( parent_column_ids.back( ), "plural_"
                         + lower( get_obj( ).child_List( ).child_List_Field( ).Child_Rel_Source_Parent( ).Parent_Class( ).Name( ) ) ) );

                        switch( get_obj( ).child_List( ).child_List_Field( ).Label_Class( ) )
                        {
                           case 0: // i.e. child
                           break;

                           case 1:
                           {
                              if( !dextras.empty( ) )
                                 dextras += '+';

                              dextras += "label_class=plural_"
                               + lower( get_obj( ).child_List( ).child_List_Field( ).Child_Rel_Source_Parent( ).Class( ).Name( ) );

                              break;
                           }

                           default:
                           throw runtime_error( "unexpected Label_Class value #"
                            + to_string( get_obj( ).child_List( ).child_List_Field( ).Label_Class( ) )
                            + " in Model::Generate" );
                        }
                     }
                     else if( is_null( get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ) ) )
                     {
                        p_field = &get_obj( ).child_List( ).child_List_Field( ).Source_Child( );
                        is_mandatory = get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ).Mandatory( );
                        parent_column_ids.push_back( get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ).Id( ) );
                        parent_column_names.push_back( get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ).Name( ) );
                        parent_column_class_ids.push_back( get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ).Parent_Class( ).Id( ) );
                        parent_column_class_names.push_back( get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ).Parent_Class( ).Name( ) );
                        parent_column_child_field_ids.push_back( get_obj( ).child_List( ).child_List_Field( ).Source_Child( ).Id( ) );
                     }
                     else
                     {
                        p_field = &get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( );
                        is_mandatory = get_obj( ).child_List( ).child_List_Field( ).Source_Child( ).Mandatory( );
                        parent_column_ids.push_back( get_obj( ).child_List( ).child_List_Field( ).Source_Child( ).Id( ) );
                        parent_column_names.push_back( get_obj( ).child_List( ).child_List_Field( ).Source_Child( ).Name( ) );
                        parent_column_class_ids.push_back( get_obj( ).child_List( ).child_List_Field( ).Source_Child( ).Parent_Class( ).Id( ) );
                        parent_column_class_names.push_back( get_obj( ).child_List( ).child_List_Field( ).Source_Child( ).Parent_Class( ).Name( ) );
                        parent_column_child_field_ids.push_back( get_obj( ).child_List( ).child_List_Field( ).Source_Grandchild( ).Id( ) );
                     }

                     string pextras( get_obj( ).child_List( ).child_List_Field( ).Restriction_Spec( ).Restrict_Values( ) );

                     bool is_restricted( !pextras.empty( ) );

                     bool parent_mandatory = is_mandatory;

                     if( p_field->Encrypted( ) )
                     {
                        if( !pextras.empty( ) )
                           pextras += '+';

                        pextras += "@decrypt";
                     }

                     if( get_obj( ).child_List( ).child_List_Field( ).Sort_Manually( ) )
                     {
                        if( !pextras.empty( ) )
                           pextras += '+';

                        pextras += "@sort";
                     }

                     set< string > pextra_items;
                     split_string( pextras, pextra_items, '+' );

                     if( pextra_items.count( "@sort" ) && p_field->Extra( ) == 13 ) // i.e. "manual_link"
                        pextras = search_replace( pextras, "@sort", "@sortlinks" );

                     if( is_restricted && !get_obj( ).child_List( ).child_List_Field( ).Restriction_Value( ).empty( ) )
                        pextras += "#" + get_obj( ).child_List( ).child_List_Field( ).Restriction_Value( );

                     // NOTE: It is being assumed here that the only reason for using a "static instance key" is for a "folder".
                     if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ) )
                      && !is_null( get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ).Parent_Class( ).Static_Instance_Key( ) ) )
                        pextras = "[folder]" + pextras;

                     if( p_field->Class( ).child_Field( ).iterate_forwards( ) )
                     {
                        do
                        {
                           if( p_field->Class( ).child_Field( ).Extra( ) == 5 ) // i.e. "order"
                           {
                              if( !pextras.empty( ) )
                                 pextras += "+";

                              pextras += p_field->Class( ).child_Field( ).Id( );
                           }
                           else if( p_field->Class( ).child_Field( ).Extra( ) == 13 ) // i.e. "manual_link"
                           {
                              if( !pextras.empty( ) )
                                 pextras += "+";

                              pextras += "@manuallink";
                           }
                           else if( p_field->Class( ).child_Field( ).Extra( ) == 17 ) // i.e. "permission"
                           {
                              // NOTE: If the field belongs to an aliased class then need to instead use
                              // the Source Field in order to locate the filter.
                              Meta_Field* p_sfield( &p_field->Class( ).child_Field( ) );

                              if( !is_null( p_field->Class( ).child_Field( ).Source_Field( ) ) )
                                 p_sfield = &p_field->Class( ).child_Field( ).Source_Field( );

                              if( p_sfield->child_Specification( ).iterate_forwards( ) )
                              {
                                 do
                                 {
                                    if( p_sfield->child_Specification( ).Specification_Type( ) == "filter_perm_restricted" )
                                    {
                                       if( !pextras.empty( ) )
                                          pextras += "+";

                                       pextras += "@permission:" + p_sfield->child_Specification( ).Id( );

                                       p_sfield->child_Specification( ).iterate_stop( );

                                       break;
                                    }
                                 } while( p_sfield->child_Specification( ).iterate_next( ) );
                              }
                           }

                        } while( p_field->Class( ).child_Field( ).iterate_next( ) );

                        string select_key_exclusions( get_obj( ).child_List( ).child_List_Field( ).Select_Key_Exclusions( ) );

                        // NOTE: If select key exclusions value starts with "@null" then the parent will be treated as being
                        // mandatory (with this then being removed from the key exclusions value).
                        if( select_key_exclusions.find( c_parent_extra_null ) == 0 )
                        {
                           parent_mandatory = true;

                           select_key_exclusions.erase( 0, strlen( c_parent_extra_null ) );

                           if( !select_key_exclusions.empty( ) && ( select_key_exclusions[ 0 ] == ' ' ) )
                              select_key_exclusions.erase( 0, 1 );
                        }

                        if( !select_key_exclusions.empty( ) )
                        {
                           string str( "-" );

                           for( size_t i = 0; i < select_key_exclusions.size( ); i++ )
                           {
                              if( select_key_exclusions[ i ] == ' ' )
                                 str += "-";
                              else if( ( select_key_exclusions[ i ] >= '0' && select_key_exclusions[ i ] <= '9' )
                               || ( select_key_exclusions[ i ] >= 'A' && select_key_exclusions[ i ] <= 'Z' )
                               || ( select_key_exclusions[ i ] >= 'a' && select_key_exclusions[ i ] <= 'z' ) )
                                 str += select_key_exclusions[ i ];
                              else
                                 throw runtime_error( "invalid character '" + to_string( select_key_exclusions[ i ] )
                                  + "' found in select key exclusions (key values should be space separated)" );
                           }

                           pextras += str;
                        }
                     }

                     parent_column_extras.push_back( pextras );
                     parent_column_dextras.push_back( dextras );

                     parent_column_other_info.push_back( parent_mandatory );

                     string operations;

                     if( get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "link_select" )
                     {
                        operations = "link";

                        if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Link_Permission( ) ) )
                           operations += ":" + get_obj( ).child_List( ).child_List_Field( ).Link_Permission( ).Id( );
                     }
                     else if( get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "restrict_select" )
                     {
                        operations = "select";

                        string select_opt;

                        switch( get_obj( ).child_List( ).child_List_Field( ).Trigger_Option( ) )
                        {
                           case 0:
                           break;

                           case 1:
                           select_opt = "skey1";
                           break;

                           case 2:
                           select_opt = "skey2";
                           break;

                           case 3:
                           select_opt = "skey3";
                           break;

                           case 4:
                           select_opt = "skey4";
                           break;

                           case 5:
                           select_opt = "skey5";
                           break;

                           case 6:
                           select_opt = "skey6";
                           break;

                           case 7:
                           select_opt = "skey7";
                           break;

                           case 8:
                           select_opt = "skey8";
                           break;

                           case 9:
                           select_opt = "skey9";
                           break;

                           case 10:
                           select_opt = "skey0";
                           break;

                           default:
                           throw runtime_error( "unknown trigger option #"
                            + to_string( get_obj( ).child_List( ).child_List_Field( ).Trigger_Option( ) ) + " in Model::Generate" );
                        }

                        if( get_obj( ).child_List( ).child_List_Field( ).Search_Option_Limit( ) )
                           select_opt += "#" + to_string( get_obj( ).child_List( ).child_List_Field( ).Search_Option_Limit( ) );

                        if( !select_opt.empty( ) )
                           operations += ":" + select_opt + "!";
                        else if( !get_obj( ).child_List( ).child_List_Field( ).Restriction_Value( ).empty( ) )
                           operations += ":";

                        operations += get_obj( ).child_List( ).child_List_Field( ).Restriction_Value( );

                        string include_key_additions( get_obj( ).child_List( ).child_List_Field( ).Include_Key_Additions( ) );
                        if( !include_key_additions.empty( ) )
                        {
                           string str( "+" );

                           for( size_t i = 0; i < include_key_additions.size( ); i++ )
                           {
                              if( include_key_additions[ i ] == ' ' )
                                 str += "+";
                              else if( ( include_key_additions[ i ] >= '0' && include_key_additions[ i ] <= '9' )
                               || ( include_key_additions[ i ] >= 'A' && include_key_additions[ i ] <= 'Z' )
                               || ( include_key_additions[ i ] >= 'a' && include_key_additions[ i ] <= 'z' ) )
                                 str += include_key_additions[ i ];
                              else
                                 throw runtime_error( "invalid character '" + to_string( include_key_additions[ i ] )
                                  + "' found in select key additions (key values should be space separated)" );
                           }

                           operations += str;
                        }
                     }
                     else if( get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "restrict_select_child_rel" )
                     {
                        operations = "select_child";

                        if( get_obj( ).child_List( ).child_List_Field( ).Search_Option_Limit( ) )
                           operations += ":#" + to_string( get_obj( ).child_List( ).child_List_Field( ).Search_Option_Limit( ) );
                     }
                     else if( get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "restrict_switch" )
                     {
                        switch( get_obj( ).child_List( ).child_List_Field( ).Switch_Type( ) )
                        {
                           case 0:
                           operations = "checked";
                           break;

                           case 1:
                           operations = "unchecked";
                           break;

                           case 2:
                           operations = "rchecked";
                           break;

                           case 3:
                           operations = "runchecked";
                           break;

                           default:
                           throw runtime_error( "unknown switch type #"
                            + to_string( get_obj( ).child_List( ).child_List_Field( ).Switch_Type( ) ) + " in Model::Generate" );
                        }

                        if( !get_obj( ).child_List( ).child_List_Field( ).Restriction_Value( ).empty( ) )
                           operations += ":" + get_obj( ).child_List( ).child_List_Field( ).Restriction_Value( );
                     }

                     parent_column_operations.push_back( operations );
                  }
                  else if( get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "link_select"
                   || get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "restrict_field"
                   || get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "restrict_search"
                   || get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "restrict_select"
                   || get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "restrict_switch" )
                  {
                     Meta_Field* p_field( &get_obj( ).child_List( ).child_List_Field( ).Source_Field( ) );

                     if( get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "restrict_field" )
                        p_field = &get_obj( ).child_List( ).child_List_Field( ).Restriction_Field( );

                     restrict_field_ids.push_back( p_field->Id( ) );
                     restrict_field_names.push_back( p_field->Name( ) );
                     restrict_field_types.push_back( meta_field_type_name(
                      p_field->Primitive( ), p_field->Mandatory( ), "", "" ) );

                     string field_extras( meta_field_extras( p_field->UOM( ), p_field->UOM_Name( ),
                      p_field->Extra( ), p_field->Encrypted( ), p_field->Transient( ),
                      p_field->Type( ).Max_Size( ), p_field->Enum( ).Id( ), p_field->Enum_Filter( ).Id( ),
                      p_field->Type( ).Primitive( ), p_field->Type( ).Min_Value( ), p_field->Type( ).Max_Value( ),
                      p_field->Type( ).Numeric_Digits( ), p_field->Type( ).Numeric_Decimals( ),
                      p_field->Type( ).String_Domain( ), p_field->Type( ).Date_Precision( ),
                      p_field->Type( ).Time_Precision( ), p_field->Type( ).Show_Plus_Sign( ),
                      p_field->Type( ).Zero_Padding( ), p_field->Type( ).Int_Type( ), p_field->Type( ).Numeric_Type( ) ) );

                     if( !extras.empty( ) )
                     {
                        if( field_extras.empty( ) )
                           field_extras = extras;
                        else
                           field_extras = extras + "+" + field_extras;
                     }

                     if( get_obj( ).child_List( ).child_List_Field( ).Exact_Match_Only( ) )
                     {
                        if( !field_extras.empty( ) )
                           field_extras += "+";

                        field_extras += "exact_match";
                     }

                     switch( get_obj( ).child_List( ).child_List_Field( ).Font_Size( ) )
                     {
                        case 0: // i.e. normal
                        break;

                        case 1:
                        {
                           if( !field_extras.empty( ) )
                              field_extras += '+';

                           field_extras += "large";

                           break;
                        }

                        case 2:
                        {
                           if( !field_extras.empty( ) )
                              field_extras += '+';

                           field_extras += "larger";

                           break;
                        }

                        case 5:
                        {
                           if( !field_extras.empty( ) )
                              field_extras += '+';

                           field_extras += "small";

                           break;
                        }

                        case 6:
                        {
                           if( !field_extras.empty( ) )
                              field_extras += '+';

                           field_extras += "smaller";

                           break;
                        }

                        default:
                        throw runtime_error( "unexpected Font_Size value #"
                         + to_string( get_obj( ).child_List( ).child_List_Field( ).Font_Size( ) )
                         + " in Model::Generate" );
                     }

                     string vext;

                     switch( get_obj( ).child_List( ).child_List_Field( ).View_Parent_Extra( ) )
                     {
                        case 0: // i.e. none
                        break;

                        case 1:
                        vext = "vext=1";
                        break;

                        case 2:
                        vext = "vext=2";
                        break;

                        case 3:
                        vext = "vext=3";
                        break;

                        case 4:
                        vext = "vext=4";
                        break;

                        case 5:
                        vext = "vext=5";
                        break;

                        case 6:
                        vext = "vext=6";
                        break;

                        case 7:
                        vext = "vext=7";
                        break;

                        case 8:
                        vext = "vext=8";
                        break;

                        case 9:
                        vext = "vext=9";
                        break;
                        
                        case 10:
                        vext = "vext=0";
                        break;

                     }

                     if( !vext.empty( ) )
                     {
                        if( !field_extras.empty( ) )
                           field_extras += "+";

                        field_extras += vext;
                     }

                     restrict_field_other.push_back( make_pair( p_field->Mandatory( ), field_extras ) );

                     string operations;

                     if( get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "link_select" )
                     {
                        operations = "link";

                        if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Link_Permission( ) ) )
                           operations += ":" + get_obj( ).child_List( ).child_List_Field( ).Link_Permission( ).Id( );
                     }
                     else if( get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "restrict_field" )
                     {
                        operations = "restricted";

                        if( !p_field->Transient( ) )
                           rfield_ids.push_back( p_field->Id( ) );
                     }
                     else if( get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "restrict_search" )
                        operations = "search";
                     else if( get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "restrict_select" )
                        operations = "select";
                     else if( get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "restrict_switch" )
                     {
                        switch( get_obj( ).child_List( ).child_List_Field( ).Switch_Type( ) )
                        {
                           case 0:
                           operations = "checked";
                           break;

                           case 1:
                           operations = "unchecked";
                           break;

                           case 2:
                           operations = "rchecked";
                           break;

                           case 3:
                           operations = "runchecked";
                           break;

                           default:
                           throw runtime_error( "unknown switch type #"
                            + to_string( get_obj( ).child_List( ).child_List_Field( ).Switch_Type( ) ) + " in Model::Generate" );
                        }
                     }

                     if( !get_obj( ).child_List( ).child_List_Field( ).Restriction_Value( ).empty( ) )
                        operations += ":" + get_obj( ).child_List( ).child_List_Field( ).Restriction_Value( );

                     restrict_field_operations.push_back( operations );
                  }
                  else if( get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "new_select" )
                  {
                     if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Source_Field( ) ) )
                     {
                        Meta_Field* p_field( &get_obj( ).child_List( ).child_List_Field( ).Source_Field( ) );

                        if( is_null( get_obj( ).child_List( ).child_List_Field( ).Source_Field( ).Enum( ) ) )
                        {
                           new_select_class_id = get_obj( ).child_List( ).Class( ).Id( );
                           new_select_field_id = get_obj( ).child_List( ).child_List_Field( ).Source_Field( ).Id( );

                           new_select_dfield_id = c_key_field;
                           new_select_extras = get_obj( ).child_List( ).child_List_Field( ).Restriction_Spec( ).Restrict_Values( );
                        }
                        else
                        {
                           new_select_dfield_id = get_obj( ).child_List( ).child_List_Field( ).Source_Field( ).Id( );
                           new_select_dfenum_id = get_obj( ).child_List( ).child_List_Field( ).Source_Field( ).Enum( ).Id( );
                        }
                     }
                     else
                     {
                        Meta_Field* p_field = &get_obj( ).child_List( ).child_List_Field( ).Source_Child( );

                        new_select_class_id = p_field->Class( ).Id( );
                        new_select_field_id = p_field->Id( );
                        new_select_dfield_id = get_obj( ).child_List( ).child_List_Field( ).Source_Parent( ).Id( );

                        if( get_obj( ).child_List( ).child_List_Field( ).Exclude_In_Use_FK( ) )
                           new_select_extras += "~";

                        string rvalues( get_obj( ).child_List( ).child_List_Field( ).Restriction_Spec( ).Restrict_Values( ) );

                        if( !rvalues.empty( ) )
                           new_select_extras += rvalues;

                        if( p_field->Class( ).child_Field( ).iterate_forwards( ) )
                        {
                           do
                           {
                              if( p_field->Class( ).child_Field( ).Extra( ) == 5 ) // i.e. "order"
                              {
                                 if( !new_select_extras.empty( ) )
                                    new_select_extras += "+";
                                 new_select_extras += p_field->Class( ).child_Field( ).Id( );
                              }
                              else if( p_field->Class( ).child_Field( ).Extra( ) == 17 ) // i.e. "permission"
                              {
                                 // NOTE: If the field belongs to an aliased class then need to instead use
                                 // the Source Field in order to locate the filter.
                                 Meta_Field* p_sfield( &p_field->Class( ).child_Field( ) );

                                 if( !is_null( p_field->Class( ).child_Field( ).Source_Field( ) ) )
                                    p_sfield = &p_field->Class( ).child_Field( ).Source_Field( );

                                 if( p_sfield->child_Specification( ).iterate_forwards( ) )
                                 {
                                    do
                                    {
                                       if( p_sfield->child_Specification( ).Specification_Type( ) == "filter_perm_restricted" )
                                       {
                                          if( !new_select_extras.empty( ) )
                                             new_select_extras += "+";
                                          new_select_extras += "@permission:" + p_sfield->child_Specification( ).Id( );

                                          p_sfield->child_Specification( ).iterate_stop( );
                                          break;
                                       }
                                    } while( p_sfield->child_Specification( ).iterate_next( ) );
                                 }
                              }

                           } while( p_field->Class( ).child_Field( ).iterate_next( ) );
                        }
                     }
                  }
                  else if( get_obj( ).child_List( ).child_List_Field( ).Type( ).get_key( ) == "user_action_event" )
                  {
                     if( !actions.empty( ) )
                        actions += ',';

                     if( !is_null( get_obj( ).child_List( ).child_List_Field( ).Access_Parent_Modifier( ) ) )
                     {
                        Meta_Class* p_parent_Class = &get_obj( ).child_List( ).child_List_Field( ).Parent_Class( );
                        string modifier_key_info( to_string( Meta_Modifier::static_get_field_id( Meta_Modifier::e_field_id_Name ) ) + ' ' );

                        if( p_parent_Class->child_Modifier( ).iterate_forwards( modifier_key_info ) )
                        {
                           uint64_t flag_value( UINT64_C( 0x100 ) );
                           do
                           {
                              ostringstream osstr;
                              osstr << hex << flag_value;
                              flag_value <<= 1;

                              if( get_obj( ).child_List( ).child_List_Field( ).Access_Parent_Modifier( ) == p_parent_Class->child_Modifier( ) )
                                 actions += "[" + osstr.str( ) + "]";
                           } while( p_parent_Class->child_Modifier( ).iterate_next( ) );
                        }
                     }

                     switch( get_obj( ).child_List( ).child_List_Field( ).Access_Restriction( ) )
                     {
                        case 0:
                        break;

                        case 1: // i.e. owner_only
                        actions += '#';
                        break;

                        case 2: // i.e. admin_only
                        actions += '@';
                        break;

                        case 3: // i.e. admin_owner_only
                        actions += '%';
                        break;

                        default:
                        throw runtime_error( "unexpected Access_Restriction #"
                         + to_string( get_obj( ).child_List( ).child_List_Field( ).Access_Restriction( ) ) + " in Model::Generate" );
                     }

                     if( get_obj( ).child_List( ).child_List_Field( ).Hide_If_Is_Changing( ) )
                        actions += ':';

                     if( get_obj( ).child_List( ).child_List_Field( ).Retain_Selected_Rows( ) )
                        actions += '+';

                     if( get_obj( ).child_List( ).child_List_Field( ).Non_Instance_Procedure( ) )
                        actions += '-';

                     if( get_obj( ).child_List( ).child_List_Field( ).Omit_Versions( ) )
                        actions += '!';

                     if( get_obj( ).child_List( ).child_List_Field( ).Reverse_Order( ) )
                        actions += '^';

                     actions += get_obj( ).child_List( ).child_List_Field( ).Procedure( ).Id( );

                     if( !get_obj( ).child_List( ).child_List_Field( ).Procedure_Args( ).empty( ) )
                        actions += "+" + search_replace( get_obj( ).child_List( ).child_List_Field( ).Procedure_Args( ), " ", "+" );

                     if( !get_obj( ).child_List( ).child_List_Field( ).Access_Permission( ).get_key( ).empty( ) )
                        actions += "&" + get_obj( ).child_List( ).child_List_Field( ).Access_Permission( ).Id( );
                  }
               } while( get_obj( ).child_List( ).child_List_Field( ).iterate_next( ) );
            }

            outf << "\x60{\x60$specification_" << specification_name + "_acts\x60=\x60'" << actions << "\x60'\x60}\n";

            if( !pdf_vars_file.empty( ) && exists_file( pdf_vars_file ) )
            {
               string pdf_spec_file( get_obj( ).Name( ) + "_" + get_obj( ).child_List( ).Class( ).Name( ) + list_ext + ".pdf.sio" );
               outf << "\x60{\x60$specification_" << specification_name + "_pdf_spec\x60=\x60'" << pdf_spec_file << "\x60'\x60}\n";
            }

            string modifiers;

            string modifier_key_info( to_string(
             Meta_Modifier::static_get_field_id( Meta_Modifier::e_field_id_Name ) ) + ' ' );

            if( get_obj( ).child_List( ).Class( ).child_Modifier( ).iterate_forwards( modifier_key_info ) )
            {
               uint64_t flag_value( UINT64_C( 0x100 ) );

               do
               {
                  ostringstream osstr;

                  osstr << hex << flag_value;

                  flag_value <<= 1;

                  if( get_obj( ).child_List( ).Class( ).child_Modifier( ).child_Modifier_Affect( ).iterate_forwards( ) )
                  {
                     do
                     {
                        if( get_obj( ).child_List( ).Class( ).child_Modifier( ).child_Modifier_Affect( ).Scope( ) == 1
                         && ( get_obj( ).child_List( ).Class( ).child_Modifier( ).child_Modifier_Affect( ).Extra( ) == 0
                         || get_obj( ).child_List( ).Class( ).child_Modifier( ).child_Modifier_Affect( ).Extra( ) == 1 ) )
                        {
                           if( !modifiers.empty( ) )
                              modifiers += ',';

                           modifiers += osstr.str( ) + ':';

                           int type = get_obj( ).child_List( ).Class( ).child_Modifier( ).child_Modifier_Affect( ).Type( );

                           switch( type )
                           {
                              case 0:
                              modifiers += "protect";
                              break;

                              case 1:
                              modifiers += "relegate";
                              break;

                              case 2:
                              modifiers += "lowlight";
                              break;

                              case 3:
                              modifiers += "lowlight1";
                              break;

                              case 4:
                              modifiers += "highlight";
                              break;

                              case 5:
                              modifiers += "highlight1";
                              break;

                              case 6:
                              modifiers += "extralight";
                              break;

                              case 7:
                              modifiers += "extralight1";
                              break;

                              default:
                              throw runtime_error( "unknown modifier type #" + to_string( type ) + " in Model::Generate" );
                           }
                        }
                     } while( get_obj( ).child_List( ).Class( ).child_Modifier( ).child_Modifier_Affect( ).iterate_next( ) );
                  }
               } while( get_obj( ).child_List( ).Class( ).child_Modifier( ).iterate_next( ) );
            }

            // NOTE: When looking for general filters if the class is aliased then need to
            // instead use the Source Class.
            Meta_Class* p_sclass( &get_obj( ).child_List( ).Class( ) );
            if( !is_null( p_sclass->Source_Class( ) ) )
               p_sclass = &p_sclass->Source_Class( );

            // FUTURE: It would be more efficient if this query were restricted to just the "filter" records.
            if( p_sclass->child_Specification( ).iterate_forwards( ) )
            {
               do
               {
                  string specification_type( p_sclass->child_Specification( ).Specification_Type( ) );

                  if( specification_type == "filter_field_value"
                   || specification_type == "filter_fk_in_uid_set"
                   || specification_type == "filter_perm_restricted"
                   || specification_type == "filter_field_value_perm"
                   || ( !get_obj( ).child_List( ).Ignore_Username_Filter( )
                   && ( specification_type == "filter_field_uid" || specification_type == "filter_non_uid" ) ) )
                  {
                     if( !filters.empty( ) )
                        filters += ",";

                     filters += p_sclass->child_Specification( ).Id( );
                  }
               } while( p_sclass->child_Specification( ).iterate_next( ) );
            }

            outf << "\x60{\x60$specification_" << specification_name + "_mods\x60=\x60'" << modifiers << "\x60'\x60}\n";
            outf << "\x60{\x60$specification_" << specification_name + "_extra\x60=\x60'" << list_extra << "\x60'\x60}\n";

            string list_order( "forward" );

            if( get_obj( ).child_List( ).Direction( ) == 1 )
               list_order = "reverse";

            outf << "\x60{\x60$specification_" << specification_name + "_order\x60=\x60'" << list_order << "\x60'\x60}\n";

            string list_binfo;

            if( !is_null( get_obj( ).child_List( ).Class( ).Source_Model( ) ) )
            {
               list_binfo = get_obj( ).child_List( ).Class( ).Source_Model( ).Name( );
               list_binfo += ":" + get_obj( ).child_List( ).Class( ).Name( );
            }

            outf << "\x60{\x60$specification_" << specification_name + "_binfo\x60=\x60'" << list_binfo << "\x60'\x60}\n";

            if( !filters.empty( ) )
               outf << "\x60{\x60$specification_" << specification_name + "_filters\x60=\x60'" << filters << "\x60'\x60}\n";

            if( !pclass_id.empty( ) )
            {
               outf << "\x60{\x60$specification_" << specification_name
                + "_pclass\x60=\x60'" << get_mapped_id( model_name, pclass_id ) << "\x60'\x60}\n";
               outf << "\x60{\x60$specification_" << specification_name + "_pclsnm\x60=\x60'" << pclass_name << "\x60'\x60}\n";
               outf << "\x60{\x60$specification_" << specification_name
                + "_pfield\x60=\x60'" << get_mapped_id( model_name, pfield_id ) << "\x60'\x60}\n";
               outf << "\x60{\x60$specification_" << specification_name + "_pfldnm\x60=\x60'" << pfield_name << "\x60'\x60}\n";
            }

            if( !new_select_class_id.empty( ) )
            {
               outf << "\x60{\x60$specification_" << specification_name
                + "_nclass\x60=\x60'" << get_mapped_id( model_name, new_select_class_id ) << "\x60'\x60}\n";
               outf << "\x60{\x60$specification_" << specification_name
                + "_nfield\x60=\x60'" << get_mapped_id( model_name, new_select_field_id ) << "\x60'\x60}\n";
               outf << "\x60{\x60$specification_" << specification_name
                + "_nextra\x60=\x60'" << new_select_extras << "\x60'\x60}\n";
               outf << "\x60{\x60$specification_" << specification_name
                + "_dfield\x60=\x60'" << get_mapped_id( model_name, new_select_dfield_id ) << "\x60'\x60}\n";
            }

            if( !new_select_dfenum_id.empty( ) )
            {
               outf << "\x60{\x60$specification_" << specification_name
                + "_dfenum\x60=\x60'" << get_mapped_id( model_name, new_select_dfenum_id ) << "\x60'\x60}\n";
               outf << "\x60{\x60$specification_" << specification_name
                + "_dfield\x60=\x60'" << get_mapped_id( model_name, new_select_dfield_id ) << "\x60'\x60}\n";
            }

            for( size_t i = 0; i < parent_column_ids.size( ); i++ )
            {
               string pnum( to_string( i ) );

               outf << "\x60{\x60$specification_" << specification_name
                + "_parent" << pnum << "\x60=\x60'" << parent_column_names[ i ] << "\x60'\x60}\n";

               string mapped_field_id( get_mapped_id( model_name, parent_column_ids[ i ] ) );

               if( parent_column_name_overrides.count( mapped_field_id ) )
                  outf << "\x60{\x60$specification_" << specification_name
                   + "_prntnm" << pnum << "\x60=\x60'" << parent_column_name_overrides[ mapped_field_id ] << "\x60'\x60}\n";

               if( mapped_field_id.find( "." ) != string::npos )
                  mapped_field_id = "_" + mapped_field_id;

               outf << "\x60{\x60$specification_" << specification_name
                + "_jfield" << pnum << "\x60=\x60'" << mapped_field_id << "\x60'\x60}\n";

               outf << "\x60{\x60$specification_" << specification_name
                + "_dextra" << pnum << "\x60=\x60'" << parent_column_dextras[ i ] << "\x60'\x60}\n";

               outf << "\x60{\x60$specification_" << specification_name
                + "_pclass" << pnum << "\x60=\x60'" << get_mapped_id( model_name, parent_column_class_ids[ i ] ) << "\x60'\x60}\n";

               outf << "\x60{\x60$specification_" << specification_name
                + "_pclsnm" << pnum << "\x60=\x60'" << parent_column_class_names[ i ] << "\x60'\x60}\n";

               outf << "\x60{\x60$specification_" << specification_name
                + "_pfield" << pnum << "\x60=\x60'" << get_mapped_id( model_name, parent_column_child_field_ids[ i ] ) << "\x60'\x60}\n";

               outf << "\x60{\x60$specification_" << specification_name
                + "_pextra" << pnum << "\x60=\x60'" << parent_column_extras[ i ] << "\x60'\x60}\n";

               outf << "\x60{\x60$specification_" << specification_name + "_mandatory" << pnum << "\x60=\x60'"
                << ( parent_column_other_info[ i ] ? "true" : "false" ) << "\x60'\x60}\n";

               outf << "\x60{\x60$specification_" << specification_name
                + "_operations" << pnum << "\x60=\x60'" << parent_column_operations[ i ] << "\x60'\x60}\n";
            }

            for( size_t i = 0; i < restrict_field_ids.size( ); i++ )
            {
               string rnum( to_string( i ) );

               outf << "\x60{\x60$specification_" << specification_name
                + "_rfldnm" << rnum << "\x60=\x60'" << restrict_field_names[ i ] << "\x60'\x60}\n";

               outf << "\x60{\x60$specification_" << specification_name
                + "_rfield" << rnum << "\x60=\x60'" << get_mapped_id( model_name, restrict_field_ids[ i ] ) << "\x60'\x60}\n";

               outf << "\x60{\x60$specification_" << specification_name
                + "_rftype" << rnum << "\x60=\x60'" << restrict_field_types[ i ] << "\x60'\x60}\n";

               outf << "\x60{\x60$specification_" << specification_name + "_rfmand" << rnum
                << "\x60=\x60'" << ( restrict_field_other[ i ].first ? "true" : "false" ) << "\x60'\x60}\n";

               outf << "\x60{\x60$specification_" << specification_name
                + "_rfxtra" << rnum << "\x60=\x60'" << restrict_field_other[ i ].second << "\x60'\x60}\n";

               outf << "\x60{\x60$specification_" << specification_name
                + "_rfldops" << rnum << "\x60=\x60'" << restrict_field_operations[ i ] << "\x60'\x60}\n";
            }

            map< string, int > column_index_fcount;

            if( get_obj( ).child_List( ).Class( ).child_Index( ).iterate_forwards( ) )
            {
               map< string, bool > indexes;

               do
               {
                  string next_index( get_obj( ).child_List( ).Class( ).child_Index( ).Field_1( ).Id( ) );

                  if( !get_obj( ).child_List( ).Class( ).child_Index( ).Field_2( ).Id( ).empty( ) )
                     next_index += "," + get_obj( ).child_List( ).Class( ).child_Index( ).Field_2( ).Id( );

                  if( !get_obj( ).child_List( ).Class( ).child_Index( ).Field_3( ).Id( ).empty( ) )
                     next_index += "," + get_obj( ).child_List( ).Class( ).child_Index( ).Field_3( ).Id( );

                  if( !get_obj( ).child_List( ).Class( ).child_Index( ).Field_4( ).Id( ).empty( ) )
                     next_index += "," + get_obj( ).child_List( ).Class( ).child_Index( ).Field_4( ).Id( );

                  if( !get_obj( ).child_List( ).Class( ).child_Index( ).Field_5( ).Id( ).empty( ) )
                     next_index += "," + get_obj( ).child_List( ).Class( ).child_Index( ).Field_5( ).Id( );

                  indexes.insert( make_pair( next_index, get_obj( ).child_List( ).Class( ).child_Index( ).Unique( ) ) );

               } while( get_obj( ).child_List( ).Class( ).child_Index( ).iterate_next( ) );

               // NOTE: If a transient relationship was used for a "view_child" then there is no
               // point searching for an Index starting with that field (as there can't be one).
               string parent_field_id;

               if( !get_obj( ).child_List( ).Parent_Field( ).Transient( ) )
                  parent_field_id = pfield_id;

               for( size_t i = 0; i < column_ids.size( ); i++ )
               {
                  // NOTE: Only non-fk columns are considered for sorting so if a dummy
                  // record wasn't inserted during field processing then it will not be
                  // considered here.
                  if( !column_index_info.count( column_ids[ i ] ) )
                     continue;

                  string index_info( parent_field_id );

                  for( size_t j = 0; j < rfield_ids.size( ); j++ )
                  {
                     if( !index_info.empty( ) )
                        index_info += ",";
                     index_info += rfield_ids[ j ];
                  }

                  bool is_unique = false;
                  bool is_indexed = false;

                  for( size_t j = i; j < column_ids.size( ); j++ )
                  {
                     if( !index_info.empty( ) )
                        index_info += ",";
                     index_info += column_ids[ j ];

                     if( indexes.count( index_info ) )
                     {
                        column_index_info[ column_ids[ i ] ].first = true;
                        column_index_info[ column_ids[ i ] ].second = indexes[ index_info ];

                        column_index_fcount[ column_ids[ i ] ] = j - i + 1;

                        if( indexes[ index_info ] )
                           break;
                     }
                  }
               }
            }

            for( size_t i = 0; i < column_ids.size( ); i++ )
            {
               string cnum( to_string( i ) );

               if( cnum.length( ) < 2 )
                  cnum = '0' + cnum;

               string column_id( column_ids[ i ] );

               if( !column_pids[ i ].empty( ) )
                  column_id += "." + column_pids[ i ];

               outf << "\x60{\x60$specification_" << specification_name
                + "_column" << cnum << "\x60=\x60'" << get_mapped_id( model_name, column_id ) << "\x60'\x60}\n";

               outf << "\x60{\x60$specification_" << specification_name
                + "_clname" << cnum << "\x60=\x60'" << column_names[ i ] << "\x60'\x60}\n";

               outf << "\x60{\x60$specification_" << specification_name
                + "_cltype" << cnum << "\x60=\x60'" << column_types[ i ] << "\x60'\x60}\n";

               outf << "\x60{\x60$specification_" << specification_name
                + "_clxtra" << cnum << "\x60=\x60'" << column_extras[ i ] << "\x60'\x60}\n";

               outf << "\x60{\x60$specification_" << specification_name
                + "_clmods" << cnum << "\x60=\x60'" << column_modifiers[ i ] << "\x60'\x60}\n";

               outf << "\x60{\x60$specification_" << specification_name + "_cluniq" << cnum << "\x60=\x60'"
                << ( column_index_info[ column_ids[ i ] ].second ? "true" : "false" ) << "\x60'\x60}\n";

               string fcount;

               if( column_index_fcount.count( column_ids[ i ] ) )
                  fcount = ":" + to_string( column_index_fcount[ column_ids[ i ] ] );

               outf << "\x60{\x60$specification_" << specification_name + "_cindex" << cnum << "\x60=\x60'"
                << ( column_index_info[ column_ids[ i ] ].first ? "true" : "false" ) << fcount << "\x60'\x60}\n";

               if( column_parent_classes.count( field_keys[ i ] ) )
               {
                  outf << "\x60{\x60$specification_" << specification_name + "_pclass" << cnum << "\x60=\x60'"
                   << get_mapped_id( model_name, column_parent_classes[ field_keys[ i ] ].first ) << "\x60'\x60}\n";

                  outf << "\x60{\x60$specification_" << specification_name
                   + "_pclsnm" << cnum << "\x60=\x60'" << column_parent_classes[ field_keys[ i ] ].second << "\x60'\x60}\n";
               }

               if( column_parent_fields.count( field_keys[ i ] ) )
               {
                  outf << "\x60{\x60$specification_" << specification_name
                   + "_pfldnm" << cnum << "\x60=\x60'" << column_parent_fields[ field_keys[ i ] ].second << "\x60'\x60}\n";

                  outf << "\x60{\x60$specification_" << specification_name + "_pfield" << cnum << "\x60=\x60'"
                   << get_mapped_id( model_name, column_parent_fields[ field_keys[ i ] ].first ) << "\x60'\x60}\n";
               }
            }
         }
      }

      for( size_t i = 0; i < class_num; i++ )
      {
         vector< string >& next( all_class_strings[ class_ids[ i ] ] );

         for( size_t j = 0; j < next.size( ); j++ )
            outs << next[ j ] << '\n';
      }

      outv.flush( );
      if( !outv.good( ) )
         throw runtime_error( "vars output stream is bad" );

      outm.flush( );
      if( !outm.good( ) )
         throw runtime_error( "make output stream is bad" );

      outf.flush( );
      if( !outf.good( ) )
         throw runtime_error( "svars output stream is bad" );

      outs.flush( );
      if( !outs.good( ) )
         throw runtime_error( "strings output stream is bad" );

      outc.flush( );
      if( !outc.good( ) )
         throw runtime_error( "classes output stream is bad" );

      if( get_obj( ).child_Class( ).iterate_forwards( ) )
      {
         do
         {
            get_obj( ).child_Class( ).Generate( );
         } while( get_obj( ).child_Class( ).iterate_next( ) );
      }

      string all_class_keys, all_class_names;
      get_obj( ).Get_Acyclic_Class_List( all_class_keys, all_class_names, false );

      string aname( get_obj( ).Name( ) + ".acyclic.lst" );

      ofstream outa( aname.c_str( ) );
      if( !outa )
         throw runtime_error( "unexpected error opening '" + aname + "' for output" );

      vector< string > class_name_list;
      split_string( all_class_names, class_name_list );

      for( size_t i = 0; i < class_name_list.size( ); i++ )
         outa << class_name_list[ i ] << '\n';

      outa.flush( );
      if( !outa.good( ) )
         throw runtime_error( "file output stream '" + aname + "' is bad" );

      if( !old_class_names.empty( ) )
      {
         string cleanup_filename( get_obj( ).Name( ) + ".cleanup.sh" );

         ofstream outf( cleanup_filename.c_str( ), ios::out | ios::app );
         if( outf )
         {
            for( set< string >::iterator i = old_class_names.begin( ); i != old_class_names.end( ); ++i )
               outf << "if [ -f " << get_obj( ).Name( ) << '_' << *i
                << ".cpp ]; then\n ./remove_class " << get_obj( ).Name( ) << ' ' << *i << "\nfi\n";
         }

         outf.close( );

         string upgrade_ddl( get_obj( ).Name( ) + ".upgrade.sql" );
         outf.open( upgrade_ddl.c_str( ), ios::out | ios::app );

         if( !outf )
            throw runtime_error( "unable to open '" + upgrade_ddl + "' for output" );

         for( set< string >::iterator i = old_class_names.begin( ); i != old_class_names.end( ); ++i )
            outf << "DROP TABLE IF EXISTS T_" << get_obj( ).Name( ) << '_' << *i << ";\n";

         outf.flush( );
         if( !outf.good( ) )
            throw runtime_error( "file output stream '" + upgrade_ddl + "' is bad" );
      }

      string init_class_keys, init_class_names;
      get_obj( ).Get_Acyclic_Class_List( init_class_keys, init_class_names, true );

      vector< string > class_key_list;
      split_string( init_class_keys, class_key_list );

      string lname( get_obj( ).Name( ) + ".init.lst" );

      ofstream outl( lname.c_str( ) );
      if( !outl )
         throw runtime_error( "unexpected error opening '" + lname + "' for output" );

      for( size_t i = 0; i < class_key_list.size( ); i++ )
      {
         get_obj( ).child_Class( ).perform_fetch( class_key_list[ i ] );

         string key_info( to_string(
          Meta_Initial_Record::static_get_field_id( Meta_Initial_Record::e_field_id_Order ) ) + ' ' );

         if( get_obj( ).child_Class( ).child_Initial_Record( ).iterate_forwards( key_info ) )
         {
            outl << get_obj( ).child_Class( ).Name( ) << '\n';

            string columns( "@key," );

            bool first_row = true;
            vector< string > values;

            map< string, bool > mandatory_fk_fields;
            map< string, bool > any_non_empty_values;

            do
            {
               Meta_Initial_Record& initial_Record( get_obj( ).child_Class( ).child_Initial_Record( ) );

               string next_row( initial_Record.Key( ) + "," );
               if( initial_Record.child_Initial_Record_Value( ).iterate_forwards( ) )
               {
                  bool is_first_column = true;

                  do
                  {
                     if( is_first_column )
                        is_first_column = false;
                     else
                     {
                        next_row += ",";
                        if( first_row )
                           columns += ",";
                     }

                     string field_name( initial_Record.child_Initial_Record_Value( ).Field( ).Name( ) );

                     if( first_row )
                     {
                        columns += field_name;

                        mandatory_fk_fields[ field_name ] = false;

                        if( initial_Record.child_Initial_Record_Value( ).Field( ).Mandatory( )
                         && !is_null( initial_Record.child_Initial_Record_Value( ).Field( ).Parent_Class( ) ) )
                           mandatory_fk_fields[ field_name ] = true;
                     }

                     string next_value( initial_Record.child_Initial_Record_Value( ).Value( ) );

                     // NOTE: Default the "admin" login credentials to be the same as those for Meta.
                     if( ( next_value == c_admin_password ) || ( next_value == c_admin_user_hash ) )
                     {
                        Meta_User user;
                        user.perform_fetch( "admin" );

                        if( next_value == c_admin_password )
                           next_value = user.Password( );
                        else
                           next_value = user.User_Hash( );
                     }

                     if( !next_value.empty( ) )
                        any_non_empty_values[ field_name ] = true;
                     else if( first_row )
                        any_non_empty_values[ field_name ] = false;

                     if( next_value.find_first_of( ",\"\r\n" ) == string::npos )
                        next_row += next_value;
                     else
                        next_row += "\"" + escaped_string( next_value, "\"", '"' ) + "\"";
                  } while( initial_Record.child_Initial_Record_Value( ).iterate_next( ) );
               }

               first_row = false;
               values.push_back( next_row );

            } while( get_obj( ).child_Class( ).child_Initial_Record( ).iterate_next( ) );

            string iname( get_obj( ).Name( )
             + "_" + get_obj( ).child_Class( ).Name( ) + ".csv.new" );

            ofstream outi( iname.c_str( ) );
            if( !outi )
               throw runtime_error( "unexpected error opening file '" + iname + "' for output" );

            vector< string > column_names;
            split( columns, column_names );

            // NOTE: A mandatory FK field could be provided with a value from a "default_fk_global"
            // specification (i.e. via "at_create") in which case existing initial records that are
            // not updated would cause issues by having empty values for these. Assuming that every
            // initial record for such a FK has an empty value then the column name can be replaced
            // by the special "@ignore" column name.
            // FUTURE: Maybe it would be better to work out a way to instead provide default values
            // to initial record values for such cases (presumably by finding such specifications).
            for( size_t i = 0; i < column_names.size( ); i++ )
            {
               string next_column_name( column_names[ i ] );

               if( i > 0
                && mandatory_fk_fields[ next_column_name ]
                && !any_non_empty_values[ next_column_name ] )
                  next_column_name = "@ignore";

               if( i > 0 )
                  outi << ',';

               outi << next_column_name;
            }

            outi << '\n';

            for( size_t i = 0; i < values.size( ); i++ )
               outi << values[ i ] << '\n';

            outi.flush( );
            if( !outi.good( ) )
               throw runtime_error( "file output stream '" + iname + "' is bad" );
         }
      }

      outl.flush( );
      if( !outl.good( ) )
         throw runtime_error( "file output stream '" + lname + "' is bad" );

      // NOTE: If the "module" has not been created yet then do so now.
      if( !get_obj( ).Created( ) )
         get_obj( ).Create_Module( );

      set_system_variable( model_key, "" );
   }
   catch( ... )
   {
      set_system_variable( model_key, "" );
      throw;
   }
   // [<finish Generate_impl>]
}

void Meta_Model::impl::impl_Generate_File_Links( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Generate_File_Links_impl>]
//nyi
   // NOTE: If a recovery is in progress then do nothing.
   if( storage_locked_for_admin( ) )
      return;

   // NOTE: If the call here was direct (rather than being issued via an
   // Application parent) then generate new autoscript/manuscript files.
   if( !get_obj( ).get_graph_parent( ) )
      generate_new_script_sio_files( );

   string model_key( "Meta_Model_" + get_obj( ).get_key( ) );

   set_system_variable( model_key, "Generating file links..." ); // FUTURE: Should be a module string...

   try
   {
      if( get_obj( ).Source_File( ).empty( )
       || get_obj( ).Commands_File( ).empty( ) )
      {
         get_obj( ).op_update( );
         get_obj( ).op_apply( );
      }

      if( get_obj( ).child_Class( ).iterate_forwards( ) )
      {
         do
         {
            if( get_obj( ).child_Class( ).Header_File( ).empty( )
             || get_obj( ).child_Class( ).Source_File( ).empty( )
             || get_obj( ).child_Class( ).Commands_File( ).empty( ) )
            {
               get_obj( ).child_Class( ).op_update( );
               get_obj( ).child_Class( ).op_apply( );
            }
         } while( get_obj( ).child_Class( ).iterate_next( ) );
      }

      set_system_variable( model_key, "" );
   }
   catch( ... )
   {
      set_system_variable( model_key, "" );
      throw;
   }
   // [<finish Generate_File_Links_impl>]
}

void Meta_Model::impl::impl_Get_Acyclic_Class_List( string& Acyclic_Class_Keys, string& Acyclic_Class_Names, bool Check_Initial_Data )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Get_Acyclic_Class_List_impl>]
//nyi
   set< string > class_names;

   size_t num_classes = 0;
   bool is_first_pass = true;

   string leaf_class_keys;
   string leaf_class_names;

   while( true )
   {
      size_t last_count( class_names.size( ) );

      // NOTE: Even within a pass always process those classes that do not have Delay_Initial_Records
      // set before those that do (in the future an integer value rather than a boolean might need to
      // used to better control this behaviour).
      string class_key_info(
       to_string( Meta_Class::static_get_field_id( Meta_Class::e_field_id_Delay_Initial_Records ) )
       + ',' + to_string( Meta_Class::static_get_field_id( Meta_Class::e_field_id_Name ) ) + ' ' );

      if( get_obj( ).child_Class( ).iterate_forwards( class_key_info ) )
      {
         do
         {
            if( is_first_pass )
               ++num_classes;

            // NOTE: When records are created by rules code the normal dependency order (according to foreign
            // key relationships) may not be what is required so Delay_Initial_Records can be used to prevent
            // a class from being considered in the first pass.
            if( !class_names.count( get_obj( ).child_Class( ).Name( ) )
             && ( !is_first_pass || !Check_Initial_Data || !get_obj( ).child_Class( ).Delay_Initial_Records( ) ) )
            {
               if( get_obj( ).child_Class( ).child_Field( ).iterate_forwards( ) )
               {
                  bool okay = true;
                  bool is_leaf = ( !Check_Initial_Data || !get_obj( ).child_Class( ).Delay_Initial_Records( ) );
                  do
                  {
                     if( !is_null( get_obj( ).child_Class( ).child_Field( ).Parent_Class( ) ) )
                        is_leaf = false;

                     // NOTE: If an aliased class contains a FK field that belongs to a different module
                     // (due to that FK class not itself having been aliased) then ignore the field.
                     if( !is_null( get_obj( ).child_Class( ).child_Field( ).Parent_Class( ) )
                      && get_obj( ).child_Class( ).Model( ).get_key( )
                      != get_obj( ).child_Class( ).child_Field( ).Parent_Class( ).Model( ).get_key( ) )
                        continue;

                     if( !is_null( get_obj( ).child_Class( ).child_Field( ).Parent_Class( ) )
                      && !( get_obj( ).child_Class( ) == get_obj( ).child_Class( ).child_Field( ).Parent_Class( ) )
                      && !class_names.count( get_obj( ).child_Class( ).child_Field( ).Parent_Class( ).Name( ) ) )
                     {
                        // NOTE: If the FK Field is not mandatory then it will not be considered relevant
                        // unless is checking Initial Record Values and this turns up a non-empty string.
                        if( !get_obj( ).child_Class( ).child_Field( ).Mandatory( ) )
                        {
                           bool found_value = false;
                           if( Check_Initial_Data && !get_obj( ).child_Class( ).Delay_Initial_Records( )
                            && get_obj( ).child_Class( ).child_Field( ).child_Initial_Record_Value( ).iterate_forwards( ) )
                           {
                              do
                              {
                                 if( !get_obj( ).child_Class( ).child_Field( ).child_Initial_Record_Value( ).Value( ).empty( ) )
                                 {
                                    get_obj( ).child_Class( ).child_Field( ).child_Initial_Record_Value( ).iterate_stop( );
                                    found_value = true;
                                    break;
                                 }
                              } while( get_obj( ).child_Class( ).child_Field( ).child_Initial_Record_Value( ).iterate_next( ) );
                           }

                           if( !found_value )
                              continue;
                        }

                        get_obj( ).child_Class( ).child_Field( ).iterate_stop( );
                        okay = false;
                        break;
                     }

                  } while( get_obj( ).child_Class( ).child_Field( ).iterate_next( ) );

                  if( okay )
                  {
                     class_names.insert( get_obj( ).child_Class( ).Name( ) );
                     if( is_leaf )
                     {
                        if( !leaf_class_keys.empty( ) )
                        {
                           leaf_class_keys += ",";
                           leaf_class_names += ",";
                        }
                        leaf_class_keys += get_obj( ).child_Class( ).get_key( );
                        leaf_class_names += get_obj( ).child_Class( ).Name( );
                     }
                     else
                     {
                        if( !Acyclic_Class_Keys.empty( ) )
                        {
                           Acyclic_Class_Keys += ",";
                           Acyclic_Class_Names += ",";
                        }
                        Acyclic_Class_Keys += get_obj( ).child_Class( ).get_key( );
                        Acyclic_Class_Names += get_obj( ).child_Class( ).Name( );
                     }
                  }
               }
            }
         } while( get_obj( ).child_Class( ).iterate_next( ) );

         is_first_pass = false;
      }

      if( last_count == class_names.size( ) )
         break;
   }

   if( !leaf_class_keys.empty( ) )
   {
      if( Acyclic_Class_Keys.empty( ) )
      {
         Acyclic_Class_Keys = leaf_class_keys;
         Acyclic_Class_Names = leaf_class_names;
      }
      else
      {
         Acyclic_Class_Keys = leaf_class_keys + "," + Acyclic_Class_Keys;
         Acyclic_Class_Names = leaf_class_names + "," + Acyclic_Class_Names;
      }
   }

   if( num_classes != class_names.size( ) )
      throw runtime_error( "model contains invalid interdependent relationships" );
   // [<finish Get_Acyclic_Class_List_impl>]
}

void Meta_Model::impl::impl_Remove_All_Packages( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Remove_All_Packages_impl>]
//nyi
   if( !storage_locked_for_admin( ) && get_obj( ).child_Package( ).iterate_forwards( ) )
   {
      map< string, string > packages;

      do
      {
         if( get_obj( ).child_Package( ).Installed( ) )
         {
            packages.insert( make_pair(
             get_obj( ).child_Package( ).Installed_Order( ), get_obj( ).child_Package( ).get_key( ) ) );
         }
      } while( get_obj( ).child_Package( ).iterate_next( ) );

      string script_filename( get_obj( ).get_key( ) );

      string model_key( "Meta_Model_" + get_obj( ).get_key( ) );

      bool async = true;

      if( get_obj( ).get_variable( get_special_var_name( e_special_var_async ) ) == "0"
       || get_obj( ).get_variable( get_special_var_name( e_special_var_async ) ) == "false"
       || get_session_variable( get_special_var_name( e_special_var_allow_async ) ) == "0"
       || get_session_variable( get_special_var_name( e_special_var_allow_async ) ) == "false" )
         async = false;

      if( !packages.empty( ) )
      {
         ofstream outf( script_filename.c_str( ) );

         if( async )
            set_system_variable( "@" + model_key, "1" );

         for( map< string, string >::iterator i = packages.end( ); ; --i )
         {
            if( i != packages.end( ) )
            {
               get_obj( ).child_Package( ).perform_fetch( i->second );
               get_obj( ).child_Package( ).set_variable( get_special_var_name( e_special_var_do_exec ), "0" );

               if( async && ( i == packages.begin( ) ) )
                  get_obj( ).child_Package( ).set_variable( get_special_var_name( e_special_var_is_last ), "1" );

               get_obj( ).child_Package( ).Remove( );

               outf << "./" << get_obj( ).child_Package( ).get_key( ) << '\n';
               outf << "rm " << get_obj( ).child_Package( ).get_key( ) << '\n';
            }

            if( i == packages.begin( ) )
               break;
         }
      }

      if( !packages.empty( ) )
      {
         set_system_variable( model_key, "Removing packages..." ); // FUTURE: Should be a module string...

         // NOTE: If the thread that has spawned the child process is terminated (due
         // to client deciding to finish its session) then this can potentially cause
         // big troubles due to resource inheritance so the session is captured prior
         // to the async request and will be released at the end of the script.
         if( async )
            capture_session( session_id( ) );

         chmod( script_filename.c_str( ), 0770 );
         exec_system( "./run_temp " + script_filename, async );

         if( !async )
            set_system_variable( model_key, "" );
      }
   }
   // [<finish Remove_All_Packages_impl>]
}

void Meta_Model::impl::impl_Remove_Module( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Remove_Module_impl>]
//nyi
   // NOTE: If a recovery is in progress then do nothing.
   if( storage_locked_for_admin( ) )
      return;

   bool already_updating( get_obj( ).get_is_updating( ) );

   try
   {
      string name;

      if( already_updating )
         name = get_obj( ).get_original_field_value( c_field_name_Name );
      else
      {
         get_obj( ).op_update( );
         name = get_obj( ).Name( );
      }

      exec_system( "./remove_module -rdbms " + name + " >/dev/null" );

      bool found = false;

      string modules_list( "modules.lst" );

      if( exists_file( modules_list ) )
      {
         ifstream inpf( modules_list.c_str( ) );

         if( !inpf )
            throw runtime_error( "unexpected error opening '" + modules_list + "' for input" );

         string next;

         while( getline( inpf, next ) )
         {
            if( next == name )
            {
               found = true;
               break;
            }
         }
      }

      if( found )
         throw runtime_error( "unable to remove module (try using the 'remove_module' script from a console)" );

      get_obj( ).Created( false );

      if( !already_updating )
         get_obj( ).op_apply( );

      // NOTE: Get rid of any child Class file links.
      if( get_obj( ).child_Class( ).iterate_forwards( ) )
      {
         do
         {
            get_obj( ).child_Class( ).op_update( );
            get_obj( ).child_Class( ).op_apply( );
         } while( get_obj( ).child_Class( ).iterate_next( ) );
      }

      generate_new_script_sio_files( );

      if( exists_file( "autoscript.sio.new" ) )
         exec_system( "./update autoscript.sio autoscript.sio.new >/dev/null" );

      if( exists_file( "manuscript.sio.new" ) )
         exec_system( "./update manuscript.sio manuscript.sio.new >/dev/null" );
   }
   catch( ... )
   {
      if( !already_updating )
         get_obj( ).op_cancel( );
      throw;
   }
   // [<finish Remove_Module_impl>]
}

string Meta_Model::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Actions( ) );
      break;

      case 1:
      retval = to_string( impl_Add_Packages( ) );
      break;

      case 2:
      retval = to_string( impl_Allow_Anonymous_Access( ) );
      break;

      case 3:
      retval = to_string( impl_Commands_File( ) );
      break;

      case 4:
      retval = to_string( impl_Created( ) );
      break;

      case 5:
      retval = to_string( impl_Home_Title( ) );
      break;

      case 6:
      retval = to_string( impl_Id( ) );
      break;

      case 7:
      retval = to_string( impl_Name( ) );
      break;

      case 8:
      retval = to_string( impl_Next_Class_Id( ) );
      break;

      case 9:
      retval = to_string( impl_Next_List_Id( ) );
      break;

      case 10:
      retval = to_string( impl_Next_Specification_Id( ) );
      break;

      case 11:
      retval = to_string( impl_Next_View_Id( ) );
      break;

      case 12:
      retval = to_string( impl_Permission( ) );
      break;

      case 13:
      retval = to_string( impl_Source_File( ) );
      break;

      case 14:
      retval = to_string( impl_Status( ) );
      break;

      case 15:
      retval = to_string( impl_Type( ) );
      break;

      case 16:
      retval = to_string( impl_Use_Package_Demo_Data( ) );
      break;

      case 17:
      retval = to_string( impl_Version( ) );
      break;

      case 18:
      retval = to_string( impl_Workgroup( ) );
      break;

      case 19:
      retval = to_string( impl_Year_Created( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_Model::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Model::impl, string >( *this, &Meta_Model::impl::impl_Actions, value );
      break;

      case 1:
      func_string_setter< Meta_Model::impl, int >( *this, &Meta_Model::impl::impl_Add_Packages, value );
      break;

      case 2:
      func_string_setter< Meta_Model::impl, bool >( *this, &Meta_Model::impl::impl_Allow_Anonymous_Access, value );
      break;

      case 3:
      func_string_setter< Meta_Model::impl, string >( *this, &Meta_Model::impl::impl_Commands_File, value );
      break;

      case 4:
      func_string_setter< Meta_Model::impl, bool >( *this, &Meta_Model::impl::impl_Created, value );
      break;

      case 5:
      func_string_setter< Meta_Model::impl, string >( *this, &Meta_Model::impl::impl_Home_Title, value );
      break;

      case 6:
      func_string_setter< Meta_Model::impl, string >( *this, &Meta_Model::impl::impl_Id, value );
      break;

      case 7:
      func_string_setter< Meta_Model::impl, string >( *this, &Meta_Model::impl::impl_Name, value );
      break;

      case 8:
      func_string_setter< Meta_Model::impl, string >( *this, &Meta_Model::impl::impl_Next_Class_Id, value );
      break;

      case 9:
      func_string_setter< Meta_Model::impl, string >( *this, &Meta_Model::impl::impl_Next_List_Id, value );
      break;

      case 10:
      func_string_setter< Meta_Model::impl, string >( *this, &Meta_Model::impl::impl_Next_Specification_Id, value );
      break;

      case 11:
      func_string_setter< Meta_Model::impl, string >( *this, &Meta_Model::impl::impl_Next_View_Id, value );
      break;

      case 12:
      func_string_setter< Meta_Model::impl, Meta_Permission >( *this, &Meta_Model::impl::impl_Permission, value );
      break;

      case 13:
      func_string_setter< Meta_Model::impl, string >( *this, &Meta_Model::impl::impl_Source_File, value );
      break;

      case 14:
      func_string_setter< Meta_Model::impl, string >( *this, &Meta_Model::impl::impl_Status, value );
      break;

      case 15:
      func_string_setter< Meta_Model::impl, bool >( *this, &Meta_Model::impl::impl_Type, value );
      break;

      case 16:
      func_string_setter< Meta_Model::impl, bool >( *this, &Meta_Model::impl::impl_Use_Package_Demo_Data, value );
      break;

      case 17:
      func_string_setter< Meta_Model::impl, string >( *this, &Meta_Model::impl::impl_Version, value );
      break;

      case 18:
      func_string_setter< Meta_Model::impl, Meta_Workgroup >( *this, &Meta_Model::impl::impl_Workgroup, value );
      break;

      case 19:
      func_string_setter< Meta_Model::impl, int >( *this, &Meta_Model::impl::impl_Year_Created, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_Model::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Actions( g_default_Actions );
      break;

      case 1:
      impl_Add_Packages( g_default_Add_Packages );
      break;

      case 2:
      impl_Allow_Anonymous_Access( g_default_Allow_Anonymous_Access );
      break;

      case 3:
      impl_Commands_File( g_default_Commands_File );
      break;

      case 4:
      impl_Created( g_default_Created );
      break;

      case 5:
      impl_Home_Title( g_default_Home_Title );
      break;

      case 6:
      impl_Id( g_default_Id );
      break;

      case 7:
      impl_Name( g_default_Name );
      break;

      case 8:
      impl_Next_Class_Id( g_default_Next_Class_Id );
      break;

      case 9:
      impl_Next_List_Id( g_default_Next_List_Id );
      break;

      case 10:
      impl_Next_Specification_Id( g_default_Next_Specification_Id );
      break;

      case 11:
      impl_Next_View_Id( g_default_Next_View_Id );
      break;

      case 12:
      impl_Permission( g_default_Permission );
      break;

      case 13:
      impl_Source_File( g_default_Source_File );
      break;

      case 14:
      impl_Status( g_default_Status );
      break;

      case 15:
      impl_Type( g_default_Type );
      break;

      case 16:
      impl_Use_Package_Demo_Data( g_default_Use_Package_Demo_Data );
      break;

      case 17:
      impl_Version( g_default_Version );
      break;

      case 18:
      impl_Workgroup( g_default_Workgroup );
      break;

      case 19:
      impl_Year_Created( g_default_Year_Created );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_Model::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Actions == g_default_Actions );
      break;

      case 1:
      retval = ( v_Add_Packages == g_default_Add_Packages );
      break;

      case 2:
      retval = ( v_Allow_Anonymous_Access == g_default_Allow_Anonymous_Access );
      break;

      case 3:
      retval = ( v_Commands_File == g_default_Commands_File );
      break;

      case 4:
      retval = ( v_Created == g_default_Created );
      break;

      case 5:
      retval = ( v_Home_Title == g_default_Home_Title );
      break;

      case 6:
      retval = ( v_Id == g_default_Id );
      break;

      case 7:
      retval = ( v_Name == g_default_Name );
      break;

      case 8:
      retval = ( v_Next_Class_Id == g_default_Next_Class_Id );
      break;

      case 9:
      retval = ( v_Next_List_Id == g_default_Next_List_Id );
      break;

      case 10:
      retval = ( v_Next_Specification_Id == g_default_Next_Specification_Id );
      break;

      case 11:
      retval = ( v_Next_View_Id == g_default_Next_View_Id );
      break;

      case 12:
      retval = ( v_Permission == g_default_Permission );
      break;

      case 13:
      retval = ( v_Source_File == g_default_Source_File );
      break;

      case 14:
      retval = ( v_Status == g_default_Status );
      break;

      case 15:
      retval = ( v_Type == g_default_Type );
      break;

      case 16:
      retval = ( v_Use_Package_Demo_Data == g_default_Use_Package_Demo_Data );
      break;

      case 17:
      retval = ( v_Version == g_default_Version );
      break;

      case 18:
      retval = ( v_Workgroup == g_default_Workgroup );
      break;

      case 19:
      retval = ( v_Year_Created == g_default_Year_Created );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_Model::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start modifier_field_value)] 600014
   if( get_obj( ).Type( ) == 0 )
      state |= c_modifier_Is_Non_Traditional;
   // [(finish modifier_field_value)] 600014

   // [(start modifier_field_null)] 610014
   if( !is_null( get_obj( ).Workgroup( ).Standard_Package( ) ) )
      state |= c_modifier_Standard_Package_Installed;
   // [(finish modifier_field_null)] 610014

   // [<start get_state>]
//nyi
   if( get_obj( ).Status( ).empty( ) )
      state |= c_modifier_Is_Not_Busy;
   else
      state |= ( c_state_is_changing | c_state_uneditable | c_state_undeletable | c_state_ignore_uneditable );
   // [<finish get_state>]

   return state;
}

string Meta_Model::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   if( state & c_modifier_Is_Non_Traditional )
      state_names += "|" + string( "Is_Non_Traditional" );
   if( state & c_modifier_Is_Not_Busy )
      state_names += "|" + string( "Is_Not_Busy" );
   if( state & c_modifier_Standard_Package_Installed )
      state_names += "|" + string( "Standard_Package_Installed" );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_Model::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Model::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Permission ) || ( field == c_field_name_Permission ) )
      impl_Permission( "" );
   else if( ( field == c_field_id_Workgroup ) || ( field == c_field_name_Workgroup ) )
      impl_Workgroup( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Model::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_Permission ) || ( field == c_field_name_Permission ) )
      v_Permission = value;
   else if( ( field == c_field_id_Workgroup ) || ( field == c_field_name_Workgroup ) )
      v_Workgroup = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Model::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Permission ) || ( field == c_field_name_Permission ) )
      return v_Permission;
   else if( ( field == c_field_id_Workgroup ) || ( field == c_field_name_Workgroup ) )
      return v_Workgroup;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Model::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Permission, v_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Workgroup, v_Workgroup ) );
}

void Meta_Model::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_Model::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Model::impl::clear( )
{
   v_Actions = g_default_Actions;
   v_Add_Packages = g_default_Add_Packages;
   v_Allow_Anonymous_Access = g_default_Allow_Anonymous_Access;
   v_Commands_File = g_default_Commands_File;
   v_Created = g_default_Created;
   v_Home_Title = g_default_Home_Title;
   v_Id = g_default_Id;
   v_Name = g_default_Name;
   v_Next_Class_Id = g_default_Next_Class_Id;
   v_Next_List_Id = g_default_Next_List_Id;
   v_Next_Specification_Id = g_default_Next_Specification_Id;
   v_Next_View_Id = g_default_Next_View_Id;
   v_Source_File = g_default_Source_File;
   v_Status = g_default_Status;
   v_Type = g_default_Type;
   v_Use_Package_Demo_Data = g_default_Use_Package_Demo_Data;
   v_Version = g_default_Version;
   v_Year_Created = g_default_Year_Created;

   v_Permission = string( );
   if( cp_Permission )
      p_obj->setup_foreign_key( *cp_Permission, v_Permission );

   v_Workgroup = string( );
   if( cp_Workgroup )
      p_obj->setup_foreign_key( *cp_Workgroup, v_Workgroup );
}

bool Meta_Model::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [(start parent_auto_int_inc)] 600010
   if( field_name == "Id" )
      return true;
   // [(finish parent_auto_int_inc)] 600010

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Model::impl::validate(
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

   if( is_null( v_Version ) && !value_will_be_provided( c_field_name_Version ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Version,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Version ) ) ) ) );

   if( v_Workgroup.empty( ) && !value_will_be_provided( c_field_name_Workgroup ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Workgroup,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Workgroup ) ) ) ) );

   if( !is_null( v_Home_Title )
    && ( v_Home_Title != g_default_Home_Title
    || !value_will_be_provided( c_field_name_Home_Title ) )
    && !g_Home_Title_domain.is_valid( v_Home_Title, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Home_Title,
       get_module_string( c_field_display_name_Home_Title ) + " " + error_message ) );

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

   if( !is_null( v_Next_Class_Id )
    && ( v_Next_Class_Id != g_default_Next_Class_Id
    || !value_will_be_provided( c_field_name_Next_Class_Id ) )
    && !g_Next_Class_Id_domain.is_valid( v_Next_Class_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Next_Class_Id,
       get_module_string( c_field_display_name_Next_Class_Id ) + " " + error_message ) );

   if( !is_null( v_Next_List_Id )
    && ( v_Next_List_Id != g_default_Next_List_Id
    || !value_will_be_provided( c_field_name_Next_List_Id ) )
    && !g_Next_List_Id_domain.is_valid( v_Next_List_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Next_List_Id,
       get_module_string( c_field_display_name_Next_List_Id ) + " " + error_message ) );

   if( !is_null( v_Next_Specification_Id )
    && ( v_Next_Specification_Id != g_default_Next_Specification_Id
    || !value_will_be_provided( c_field_name_Next_Specification_Id ) )
    && !g_Next_Specification_Id_domain.is_valid( v_Next_Specification_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Next_Specification_Id,
       get_module_string( c_field_display_name_Next_Specification_Id ) + " " + error_message ) );

   if( !is_null( v_Next_View_Id )
    && ( v_Next_View_Id != g_default_Next_View_Id
    || !value_will_be_provided( c_field_name_Next_View_Id ) )
    && !g_Next_View_Id_domain.is_valid( v_Next_View_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Next_View_Id,
       get_module_string( c_field_display_name_Next_View_Id ) + " " + error_message ) );

   if( !is_null( v_Version )
    && ( v_Version != g_default_Version
    || !value_will_be_provided( c_field_name_Version ) )
    && !g_Version_domain.is_valid( v_Version, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Version,
       get_module_string( c_field_display_name_Version ) + " " + error_message ) );

   if( !is_null( v_Year_Created )
    && ( v_Year_Created != g_default_Year_Created
    || !value_will_be_provided( c_field_name_Year_Created ) )
    && !g_Year_Created_domain.is_valid( v_Year_Created, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Year_Created,
       get_module_string( c_field_display_name_Year_Created ) + " " + error_message ) );

   if( !g_add_packages_enum.count( v_Add_Packages ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Add_Packages,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Add_Packages ) ) ) ) );

   if( !g_model_type_enum.count( v_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Type ) ) ) ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Model::impl::validate_set_fields(
 set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( !is_null( v_Home_Title )
    && ( fields_set.count( c_field_id_Home_Title ) || fields_set.count( c_field_name_Home_Title ) )
    && !g_Home_Title_domain.is_valid( v_Home_Title, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Home_Title,
       get_module_string( c_field_display_name_Home_Title ) + " " + error_message ) );

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

   if( !is_null( v_Next_Class_Id )
    && ( fields_set.count( c_field_id_Next_Class_Id ) || fields_set.count( c_field_name_Next_Class_Id ) )
    && !g_Next_Class_Id_domain.is_valid( v_Next_Class_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Next_Class_Id,
       get_module_string( c_field_display_name_Next_Class_Id ) + " " + error_message ) );

   if( !is_null( v_Next_List_Id )
    && ( fields_set.count( c_field_id_Next_List_Id ) || fields_set.count( c_field_name_Next_List_Id ) )
    && !g_Next_List_Id_domain.is_valid( v_Next_List_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Next_List_Id,
       get_module_string( c_field_display_name_Next_List_Id ) + " " + error_message ) );

   if( !is_null( v_Next_Specification_Id )
    && ( fields_set.count( c_field_id_Next_Specification_Id ) || fields_set.count( c_field_name_Next_Specification_Id ) )
    && !g_Next_Specification_Id_domain.is_valid( v_Next_Specification_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Next_Specification_Id,
       get_module_string( c_field_display_name_Next_Specification_Id ) + " " + error_message ) );

   if( !is_null( v_Next_View_Id )
    && ( fields_set.count( c_field_id_Next_View_Id ) || fields_set.count( c_field_name_Next_View_Id ) )
    && !g_Next_View_Id_domain.is_valid( v_Next_View_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Next_View_Id,
       get_module_string( c_field_display_name_Next_View_Id ) + " " + error_message ) );

   if( !is_null( v_Version )
    && ( fields_set.count( c_field_id_Version ) || fields_set.count( c_field_name_Version ) )
    && !g_Version_domain.is_valid( v_Version, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Version,
       get_module_string( c_field_display_name_Version ) + " " + error_message ) );

   if( !is_null( v_Year_Created )
    && ( fields_set.count( c_field_id_Year_Created ) || fields_set.count( c_field_name_Year_Created ) )
    && !g_Year_Created_domain.is_valid( v_Year_Created, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Year_Created,
       get_module_string( c_field_display_name_Year_Created ) + " " + error_message ) );
}

void Meta_Model::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Permission )
      p_obj->setup_foreign_key( *cp_Permission, v_Permission );

   if( cp_Workgroup )
      p_obj->setup_foreign_key( *cp_Workgroup, v_Workgroup );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_fetch>]
//nyi
   string script_filename( get_obj( ).get_key( ) );

   string model_key( "Meta_Model_" + get_obj( ).get_key( ) );

   if( !get_obj( ).get_graph_parent( )
    && get_system_variable( model_key ).empty( ) && !exists_file( script_filename ) )
   {
      if( get_system_variable( "@" + storage_name( ) + "_protect" ).empty( )
       && !get_obj( ).child_Package( ).get_is_iterating( ) && get_obj( ).child_Package( ).iterate_forwards( ) )
      {
         do
         {
            if( get_obj( ).child_Package( ).Installed( ) && get_obj( ).child_Package( ).Usage_Count( ) == 0 )
            {
               get_obj( ).Actions( get_obj( ).Actions( ) + "?105450" ); // i.e. Remove_All_Packages
               get_obj( ).child_Package( ).iterate_stop( );
               break;
            }
         } while( get_obj( ).child_Package( ).iterate_next( ) );
      }
   }
   else
      get_obj( ).Status( get_system_variable( model_key ) );
   // [<finish after_fetch>]
}

void Meta_Model::impl::completed_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start completed_fetch>]
   // [<finish completed_fetch>]
}

void Meta_Model::impl::at_create( )
{
   // [<start at_create>]
   //nyi
   if( !get_system_variable(
    get_special_var_name( e_special_var_blockchain_backup_identity ) ).empty( ) )
      get_obj( ).Type( 0 );
   // [<finish at_create>]
}

void Meta_Model::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_Model::impl::to_store( bool is_create, bool is_internal )
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

void Meta_Model::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start parent_auto_int_inc)] 600010
   if( is_create && is_null( get_obj( ).Id( ) ) )
   {
      if( is_null( get_obj( ).Workgroup( ) ) )
         throw runtime_error( "unexpected empty Workgroup" );

      get_obj( ).Workgroup( ).op_update( get_obj( ).Workgroup( ), FIELD_NAME( Meta, Workgroup, Next_Model_Id ) );

      get_obj( ).Id( get_obj( ).Workgroup( ).Next_Model_Id( ) );

      get_obj( ).Workgroup( ).Next_Model_Id( auto_int_increment( get_obj( ).Workgroup( ).Next_Model_Id( ) ) );
      get_obj( ).Workgroup( ).op_apply( );
   }
   // [(finish parent_auto_int_inc)] 600010

   // [(start file_link)] 600011
   // NOTE: Empty code block for scope purposes.
   if( !get_obj( ).get_is_being_cascaded( ) )
   {
      string sep( "-" );
      string ext( ".cpp" );
      string source( get_obj( ).Name( ) + ext );

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
   // [(finish file_link)] 600011

   // [(start file_link)] 600012
   // NOTE: Empty code block for scope purposes.
   if( !get_obj( ).get_is_being_cascaded( ) )
   {
      string sep( "-" );
      string ext( ".cms" );
      string source( get_obj( ).Name( ) + ext );

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
   // [(finish file_link)] 600012

   // [(start default_to_field)] 610010
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Next_Class_Id( ) == g_default_Next_Class_Id ) )
      get_obj( ).Next_Class_Id( get_obj( ).Id( ) + "C100" );
   // [(finish default_to_field)] 610010

   // [(start default_to_field)] 620010
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Next_Specification_Id( ) == g_default_Next_Specification_Id ) )
      get_obj( ).Next_Specification_Id( get_obj( ).Id( ) + "S1000" );
   // [(finish default_to_field)] 620010

   // [(start default_to_field)] 630010
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Next_List_Id( ) == g_default_Next_List_Id ) )
      get_obj( ).Next_List_Id( get_obj( ).Id( ) + "L100" );
   // [(finish default_to_field)] 630010

   // [(start default_to_field)] 640010
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Next_View_Id( ) == g_default_Next_View_Id ) )
      get_obj( ).Next_View_Id( get_obj( ).Id( ) + "V100" );
   // [(finish default_to_field)] 640010

   // [<start for_store>]
//nyi
   if( !is_null( get_obj( ).Workgroup( ).Standard_Package( ) ) )
      get_obj( ).Add_Packages( 0 );

   if( is_create && get_obj( ).Add_Packages( ) && !storage_locked_for_admin( ) )
   {
      string model_key( "Meta_Model_" + get_obj( ).get_key( ) );
      set_system_variable( model_key, "Installing initial packages..." ); // FUTURE: Should be a module string...
   }

   if( !is_create && !storage_locked_for_admin( )
    && get_obj( ).Created( ) && get_obj( ).has_field_changed( c_field_id_Name ) )
   {
      string original_name( get_obj( ).get_original_field_value( c_field_id_Name ) );

      if( file_exists( original_name + c_demo_keys_suffix ) )
         copy_file( original_name + c_demo_keys_suffix, get_obj( ).Name( ) + c_demo_keys_suffix );

      get_obj( ).Remove_Module( );
   }
   // [<finish for_store>]
}

void Meta_Model::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_store>]
//nyi
   if( is_create && get_obj( ).Add_Packages( ) )
   {
      vector< pair< string, string > > single_types;

      class_pointer< Meta_Package_Type > cp_type( e_create_instance );

      if( cp_type->iterate_forwards( ) )
      {
         do
         {
            if( cp_type->Name( ) == "Standard" )
               single_types.push_back( make_pair( cp_type->get_key( ), cp_type->Name( ) + " " + cp_type->Plural( ) ) );
            else if( cp_type->Name( ) == "User" && get_obj( ).Add_Packages( ) >= c_enum_add_packages_Standard_User )
               single_types.push_back( make_pair( cp_type->get_key( ), cp_type->Name( ) + " " + cp_type->Plural( ) ) );
            else if( cp_type->Name( ) == "Group" && get_obj( ).Add_Packages( ) >= c_enum_add_packages_Standard_User_Group )
               single_types.push_back( make_pair( cp_type->get_key( ), cp_type->Name( ) + " " + cp_type->Plural( ) ) );
         } while( cp_type->iterate_next( ) );
      }

      int child_num = 0;
      for( size_t i = 0; i < single_types.size( ); i++ )
      {
         get_obj( ).child_Package( ).op_create( construct_key_from_int( get_obj( ).get_key( ), ++child_num ) );

         string name_and_plural( single_types[ i ].second );
         string::size_type pos = name_and_plural.find( ' ' );

         string name( name_and_plural.substr( 0, pos ) );
         string plural( pos == string::npos ? name_and_plural : name_and_plural.substr( pos + 1 ) );

         get_obj( ).child_Package( ).Model( get_obj( ).get_key( ) );
         get_obj( ).child_Package( ).Package_Type( single_types[ i ].first );

         get_obj( ).child_Package( ).Name( name );
         get_obj( ).child_Package( ).Plural( plural );

         get_obj( ).child_Package( ).op_apply( );

         if( name == "Standard" )
            get_obj( ).child_Package( ).Install( );
      }
   }
   // [<finish after_store>]
}

bool Meta_Model::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
//nyi
   if( !storage_locked_for_admin( ) && get_obj( ).child_Package( ).iterate_forwards( ) )
   {
      do
      {
         if( get_obj( ).child_Package( ).Installed( ) )
         {
            retval = false;
            get_obj( ).child_Package( ).iterate_stop( );

            break;
         }
      } while( get_obj( ).child_Package( ).iterate_next( ) );
   }
   // [<finish can_destroy>]

   return retval;
}

void Meta_Model::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start file_link)] 600011
   if( !get_obj( ).Source_File( ).empty( ) )
      remove_file( get_obj( ).get_attached_file_path( get_obj( ).Source_File( ) ) );
   // [(finish file_link)] 600011

   // [(start file_link)] 600012
   if( !get_obj( ).Commands_File( ).empty( ) )
      remove_file( get_obj( ).get_attached_file_path( get_obj( ).Commands_File( ) ) );
   // [(finish file_link)] 600012

   // [<start for_destroy>]
//nyi
   if( exists_file( get_obj( ).Name( ) + ".classes.lst" ) )
   {
      exec_system( "./remove_module -rdbms " + get_obj( ).Name( ) + " >/dev/null" );

      generate_new_script_sio_files( );

      if( exists_file( "autoscript.sio.new" ) )
         exec_system( "./update autoscript.sio autoscript.sio.new >/dev/null" );

      if( exists_file( "manuscript.sio.new" ) )
         exec_system( "./update manuscript.sio manuscript.sio.new >/dev/null" );
   }
   // [<finish for_destroy>]
}

void Meta_Model::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Model::impl::set_default_values( )
{
   clear( );
}

bool Meta_Model::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_Model::impl::get_required_transients( ) const
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

Meta_Model::Meta_Model( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Model::~Meta_Model( )
{
   cleanup( );
   delete p_impl;
}

const string& Meta_Model::Actions( ) const
{
   return p_impl->impl_Actions( );
}

void Meta_Model::Actions( const string& Actions )
{
   p_impl->impl_Actions( Actions );
}

int Meta_Model::Add_Packages( ) const
{
   return p_impl->impl_Add_Packages( );
}

void Meta_Model::Add_Packages( int Add_Packages )
{
   p_impl->impl_Add_Packages( Add_Packages );
}

bool Meta_Model::Allow_Anonymous_Access( ) const
{
   return p_impl->impl_Allow_Anonymous_Access( );
}

void Meta_Model::Allow_Anonymous_Access( bool Allow_Anonymous_Access )
{
   p_impl->impl_Allow_Anonymous_Access( Allow_Anonymous_Access );
}

const string& Meta_Model::Commands_File( ) const
{
   return p_impl->impl_Commands_File( );
}

void Meta_Model::Commands_File( const string& Commands_File )
{
   p_impl->impl_Commands_File( Commands_File );
}

bool Meta_Model::Created( ) const
{
   return p_impl->impl_Created( );
}

void Meta_Model::Created( bool Created )
{
   p_impl->impl_Created( Created );
}

const string& Meta_Model::Home_Title( ) const
{
   return p_impl->impl_Home_Title( );
}

void Meta_Model::Home_Title( const string& Home_Title )
{
   p_impl->impl_Home_Title( Home_Title );
}

const string& Meta_Model::Id( ) const
{
   return p_impl->impl_Id( );
}

void Meta_Model::Id( const string& Id )
{
   p_impl->impl_Id( Id );
}

const string& Meta_Model::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_Model::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

const string& Meta_Model::Next_Class_Id( ) const
{
   return p_impl->impl_Next_Class_Id( );
}

void Meta_Model::Next_Class_Id( const string& Next_Class_Id )
{
   p_impl->impl_Next_Class_Id( Next_Class_Id );
}

const string& Meta_Model::Next_List_Id( ) const
{
   return p_impl->impl_Next_List_Id( );
}

void Meta_Model::Next_List_Id( const string& Next_List_Id )
{
   p_impl->impl_Next_List_Id( Next_List_Id );
}

const string& Meta_Model::Next_Specification_Id( ) const
{
   return p_impl->impl_Next_Specification_Id( );
}

void Meta_Model::Next_Specification_Id( const string& Next_Specification_Id )
{
   p_impl->impl_Next_Specification_Id( Next_Specification_Id );
}

const string& Meta_Model::Next_View_Id( ) const
{
   return p_impl->impl_Next_View_Id( );
}

void Meta_Model::Next_View_Id( const string& Next_View_Id )
{
   p_impl->impl_Next_View_Id( Next_View_Id );
}

const string& Meta_Model::Source_File( ) const
{
   return p_impl->impl_Source_File( );
}

void Meta_Model::Source_File( const string& Source_File )
{
   p_impl->impl_Source_File( Source_File );
}

const string& Meta_Model::Status( ) const
{
   return p_impl->impl_Status( );
}

void Meta_Model::Status( const string& Status )
{
   p_impl->impl_Status( Status );
}

bool Meta_Model::Type( ) const
{
   return p_impl->impl_Type( );
}

void Meta_Model::Type( bool Type )
{
   p_impl->impl_Type( Type );
}

bool Meta_Model::Use_Package_Demo_Data( ) const
{
   return p_impl->impl_Use_Package_Demo_Data( );
}

void Meta_Model::Use_Package_Demo_Data( bool Use_Package_Demo_Data )
{
   p_impl->impl_Use_Package_Demo_Data( Use_Package_Demo_Data );
}

const string& Meta_Model::Version( ) const
{
   return p_impl->impl_Version( );
}

void Meta_Model::Version( const string& Version )
{
   p_impl->impl_Version( Version );
}

int Meta_Model::Year_Created( ) const
{
   return p_impl->impl_Year_Created( );
}

void Meta_Model::Year_Created( int Year_Created )
{
   p_impl->impl_Year_Created( Year_Created );
}

Meta_Permission& Meta_Model::Permission( )
{
   return p_impl->impl_Permission( );
}

const Meta_Permission& Meta_Model::Permission( ) const
{
   return p_impl->impl_Permission( );
}

void Meta_Model::Permission( const string& key )
{
   p_impl->impl_Permission( key );
}

Meta_Workgroup& Meta_Model::Workgroup( )
{
   return p_impl->impl_Workgroup( );
}

const Meta_Workgroup& Meta_Model::Workgroup( ) const
{
   return p_impl->impl_Workgroup( );
}

void Meta_Model::Workgroup( const string& key )
{
   p_impl->impl_Workgroup( key );
}

Meta_Class& Meta_Model::child_Class_Source( )
{
   return p_impl->impl_child_Class_Source( );
}

const Meta_Class& Meta_Model::child_Class_Source( ) const
{
   return p_impl->impl_child_Class_Source( );
}

Meta_Class& Meta_Model::child_Class( )
{
   return p_impl->impl_child_Class( );
}

const Meta_Class& Meta_Model::child_Class( ) const
{
   return p_impl->impl_child_Class( );
}

Meta_List& Meta_Model::child_List( )
{
   return p_impl->impl_child_List( );
}

const Meta_List& Meta_Model::child_List( ) const
{
   return p_impl->impl_child_List( );
}

Meta_Module& Meta_Model::child_Module( )
{
   return p_impl->impl_child_Module( );
}

const Meta_Module& Meta_Model::child_Module( ) const
{
   return p_impl->impl_child_Module( );
}

Meta_Package_Option& Meta_Model::child_Package_Option( )
{
   return p_impl->impl_child_Package_Option( );
}

const Meta_Package_Option& Meta_Model::child_Package_Option( ) const
{
   return p_impl->impl_child_Package_Option( );
}

Meta_Package& Meta_Model::child_Package( )
{
   return p_impl->impl_child_Package( );
}

const Meta_Package& Meta_Model::child_Package( ) const
{
   return p_impl->impl_child_Package( );
}

Meta_Relationship& Meta_Model::child_Relationship( )
{
   return p_impl->impl_child_Relationship( );
}

const Meta_Relationship& Meta_Model::child_Relationship( ) const
{
   return p_impl->impl_child_Relationship( );
}

Meta_Specification& Meta_Model::child_Specification( )
{
   return p_impl->impl_child_Specification( );
}

const Meta_Specification& Meta_Model::child_Specification( ) const
{
   return p_impl->impl_child_Specification( );
}

Meta_View& Meta_Model::child_View( )
{
   return p_impl->impl_child_View( );
}

const Meta_View& Meta_Model::child_View( ) const
{
   return p_impl->impl_child_View( );
}

void Meta_Model::Create_Module( )
{
   p_impl->impl_Create_Module( );
}

void Meta_Model::Generate( )
{
   p_impl->impl_Generate( );
}

void Meta_Model::Generate_File_Links( )
{
   p_impl->impl_Generate_File_Links( );
}

void Meta_Model::Get_Acyclic_Class_List( string& Acyclic_Class_Keys, string& Acyclic_Class_Names, bool Check_Initial_Data )
{
   p_impl->impl_Get_Acyclic_Class_List( Acyclic_Class_Keys, Acyclic_Class_Names, Check_Initial_Data );
}

void Meta_Model::Remove_All_Packages( )
{
   p_impl->impl_Remove_All_Packages( );
}

void Meta_Model::Remove_Module( )
{
   p_impl->impl_Remove_Module( );
}

string Meta_Model::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Model::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_Model::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_Model::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_Model::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_Model::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_Model::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_Model::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_Model::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_Model::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Model::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_Model::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Model::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Model::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Model::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Model::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Model::clear( )
{
   p_impl->clear( );
}

void Meta_Model::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Model::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Model::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Model::completed_fetch( bool skip_set_original )
{
   p_impl->completed_fetch( skip_set_original );
}

void Meta_Model::at_create( )
{
   p_impl->at_create( );
}

void Meta_Model::post_init( )
{
   p_impl->post_init( );
}

void Meta_Model::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Model::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Model::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Model::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Model::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Model::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Model::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Model::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Model::get_field_id(
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
   else if( name == c_field_name_Add_Packages )
   {
      p_id = c_field_id_Add_Packages;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Allow_Anonymous_Access )
   {
      p_id = c_field_id_Allow_Anonymous_Access;

      if( p_type_name )
         *p_type_name = "bool";

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
   else if( name == c_field_name_Created )
   {
      p_id = c_field_id_Created;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Home_Title )
   {
      p_id = c_field_id_Home_Title;

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
   else if( name == c_field_name_Name )
   {
      p_id = c_field_id_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Next_Class_Id )
   {
      p_id = c_field_id_Next_Class_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Next_List_Id )
   {
      p_id = c_field_id_Next_List_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Next_Specification_Id )
   {
      p_id = c_field_id_Next_Specification_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Next_View_Id )
   {
      p_id = c_field_id_Next_View_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Permission )
   {
      p_id = c_field_id_Permission;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

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
   else if( name == c_field_name_Status )
   {
      p_id = c_field_id_Status;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Type )
   {
      p_id = c_field_id_Type;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Package_Demo_Data )
   {
      p_id = c_field_id_Use_Package_Demo_Data;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Version )
   {
      p_id = c_field_id_Version;

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
   else if( name == c_field_name_Year_Created )
   {
      p_id = c_field_id_Year_Created;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_id;
}

const char* Meta_Model::get_field_name(
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
   else if( id == c_field_id_Add_Packages )
   {
      p_name = c_field_name_Add_Packages;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Allow_Anonymous_Access )
   {
      p_name = c_field_name_Allow_Anonymous_Access;

      if( p_type_name )
         *p_type_name = "bool";

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
   else if( id == c_field_id_Created )
   {
      p_name = c_field_name_Created;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Home_Title )
   {
      p_name = c_field_name_Home_Title;

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
   else if( id == c_field_id_Name )
   {
      p_name = c_field_name_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Next_Class_Id )
   {
      p_name = c_field_name_Next_Class_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Next_List_Id )
   {
      p_name = c_field_name_Next_List_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Next_Specification_Id )
   {
      p_name = c_field_name_Next_Specification_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Next_View_Id )
   {
      p_name = c_field_name_Next_View_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Permission )
   {
      p_name = c_field_name_Permission;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

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
   else if( id == c_field_id_Status )
   {
      p_name = c_field_name_Status;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Type )
   {
      p_name = c_field_name_Type;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Package_Demo_Data )
   {
      p_name = c_field_name_Use_Package_Demo_Data;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Version )
   {
      p_name = c_field_name_Version;

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
   else if( id == c_field_id_Year_Created )
   {
      p_name = c_field_name_Year_Created;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_name;
}

string& Meta_Model::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_Model::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_Model::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_Model::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_Model::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_Model::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Model::get_field_uom_symbol( const string& id_or_name ) const
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
   if( ( id_or_name == c_field_id_Add_Packages ) || ( id_or_name == c_field_name_Add_Packages ) )
   {
      name = string( c_field_display_name_Add_Packages );
      get_module_string( c_field_display_name_Add_Packages, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Anonymous_Access ) || ( id_or_name == c_field_name_Allow_Anonymous_Access ) )
   {
      name = string( c_field_display_name_Allow_Anonymous_Access );
      get_module_string( c_field_display_name_Allow_Anonymous_Access, &next );
   }
   if( ( id_or_name == c_field_id_Commands_File ) || ( id_or_name == c_field_name_Commands_File ) )
   {
      name = string( c_field_display_name_Commands_File );
      get_module_string( c_field_display_name_Commands_File, &next );
   }
   if( ( id_or_name == c_field_id_Created ) || ( id_or_name == c_field_name_Created ) )
   {
      name = string( c_field_display_name_Created );
      get_module_string( c_field_display_name_Created, &next );
   }
   if( ( id_or_name == c_field_id_Home_Title ) || ( id_or_name == c_field_name_Home_Title ) )
   {
      name = string( c_field_display_name_Home_Title );
      get_module_string( c_field_display_name_Home_Title, &next );
   }
   if( ( id_or_name == c_field_id_Id ) || ( id_or_name == c_field_name_Id ) )
   {
      name = string( c_field_display_name_Id );
      get_module_string( c_field_display_name_Id, &next );
   }
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
   {
      name = string( c_field_display_name_Name );
      get_module_string( c_field_display_name_Name, &next );
   }
   if( ( id_or_name == c_field_id_Next_Class_Id ) || ( id_or_name == c_field_name_Next_Class_Id ) )
   {
      name = string( c_field_display_name_Next_Class_Id );
      get_module_string( c_field_display_name_Next_Class_Id, &next );
   }
   if( ( id_or_name == c_field_id_Next_List_Id ) || ( id_or_name == c_field_name_Next_List_Id ) )
   {
      name = string( c_field_display_name_Next_List_Id );
      get_module_string( c_field_display_name_Next_List_Id, &next );
   }
   if( ( id_or_name == c_field_id_Next_Specification_Id ) || ( id_or_name == c_field_name_Next_Specification_Id ) )
   {
      name = string( c_field_display_name_Next_Specification_Id );
      get_module_string( c_field_display_name_Next_Specification_Id, &next );
   }
   if( ( id_or_name == c_field_id_Next_View_Id ) || ( id_or_name == c_field_name_Next_View_Id ) )
   {
      name = string( c_field_display_name_Next_View_Id );
      get_module_string( c_field_display_name_Next_View_Id, &next );
   }
   if( ( id_or_name == c_field_id_Permission ) || ( id_or_name == c_field_name_Permission ) )
   {
      name = string( c_field_display_name_Permission );
      get_module_string( c_field_display_name_Permission, &next );
   }
   if( ( id_or_name == c_field_id_Source_File ) || ( id_or_name == c_field_name_Source_File ) )
   {
      name = string( c_field_display_name_Source_File );
      get_module_string( c_field_display_name_Source_File, &next );
   }
   if( ( id_or_name == c_field_id_Status ) || ( id_or_name == c_field_name_Status ) )
   {
      name = string( c_field_display_name_Status );
      get_module_string( c_field_display_name_Status, &next );
   }
   if( ( id_or_name == c_field_id_Type ) || ( id_or_name == c_field_name_Type ) )
   {
      name = string( c_field_display_name_Type );
      get_module_string( c_field_display_name_Type, &next );
   }
   if( ( id_or_name == c_field_id_Use_Package_Demo_Data ) || ( id_or_name == c_field_name_Use_Package_Demo_Data ) )
   {
      name = string( c_field_display_name_Use_Package_Demo_Data );
      get_module_string( c_field_display_name_Use_Package_Demo_Data, &next );
   }
   if( ( id_or_name == c_field_id_Version ) || ( id_or_name == c_field_name_Version ) )
   {
      name = string( c_field_display_name_Version );
      get_module_string( c_field_display_name_Version, &next );
   }
   if( ( id_or_name == c_field_id_Workgroup ) || ( id_or_name == c_field_name_Workgroup ) )
   {
      name = string( c_field_display_name_Workgroup );
      get_module_string( c_field_display_name_Workgroup, &next );
   }
   if( ( id_or_name == c_field_id_Year_Created ) || ( id_or_name == c_field_name_Year_Created ) )
   {
      name = string( c_field_display_name_Year_Created );
      get_module_string( c_field_display_name_Year_Created, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_Model::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Actions ) || ( id_or_name == c_field_name_Actions ) )
      display_name = get_module_string( c_field_display_name_Actions );
   if( ( id_or_name == c_field_id_Add_Packages ) || ( id_or_name == c_field_name_Add_Packages ) )
      display_name = get_module_string( c_field_display_name_Add_Packages );
   if( ( id_or_name == c_field_id_Allow_Anonymous_Access ) || ( id_or_name == c_field_name_Allow_Anonymous_Access ) )
      display_name = get_module_string( c_field_display_name_Allow_Anonymous_Access );
   if( ( id_or_name == c_field_id_Commands_File ) || ( id_or_name == c_field_name_Commands_File ) )
      display_name = get_module_string( c_field_display_name_Commands_File );
   if( ( id_or_name == c_field_id_Created ) || ( id_or_name == c_field_name_Created ) )
      display_name = get_module_string( c_field_display_name_Created );
   if( ( id_or_name == c_field_id_Home_Title ) || ( id_or_name == c_field_name_Home_Title ) )
      display_name = get_module_string( c_field_display_name_Home_Title );
   if( ( id_or_name == c_field_id_Id ) || ( id_or_name == c_field_name_Id ) )
      display_name = get_module_string( c_field_display_name_Id );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
      display_name = get_module_string( c_field_display_name_Name );
   if( ( id_or_name == c_field_id_Next_Class_Id ) || ( id_or_name == c_field_name_Next_Class_Id ) )
      display_name = get_module_string( c_field_display_name_Next_Class_Id );
   if( ( id_or_name == c_field_id_Next_List_Id ) || ( id_or_name == c_field_name_Next_List_Id ) )
      display_name = get_module_string( c_field_display_name_Next_List_Id );
   if( ( id_or_name == c_field_id_Next_Specification_Id ) || ( id_or_name == c_field_name_Next_Specification_Id ) )
      display_name = get_module_string( c_field_display_name_Next_Specification_Id );
   if( ( id_or_name == c_field_id_Next_View_Id ) || ( id_or_name == c_field_name_Next_View_Id ) )
      display_name = get_module_string( c_field_display_name_Next_View_Id );
   if( ( id_or_name == c_field_id_Permission ) || ( id_or_name == c_field_name_Permission ) )
      display_name = get_module_string( c_field_display_name_Permission );
   if( ( id_or_name == c_field_id_Source_File ) || ( id_or_name == c_field_name_Source_File ) )
      display_name = get_module_string( c_field_display_name_Source_File );
   if( ( id_or_name == c_field_id_Status ) || ( id_or_name == c_field_name_Status ) )
      display_name = get_module_string( c_field_display_name_Status );
   if( ( id_or_name == c_field_id_Type ) || ( id_or_name == c_field_name_Type ) )
      display_name = get_module_string( c_field_display_name_Type );
   if( ( id_or_name == c_field_id_Use_Package_Demo_Data ) || ( id_or_name == c_field_name_Use_Package_Demo_Data ) )
      display_name = get_module_string( c_field_display_name_Use_Package_Demo_Data );
   if( ( id_or_name == c_field_id_Version ) || ( id_or_name == c_field_name_Version ) )
      display_name = get_module_string( c_field_display_name_Version );
   if( ( id_or_name == c_field_id_Workgroup ) || ( id_or_name == c_field_name_Workgroup ) )
      display_name = get_module_string( c_field_display_name_Workgroup );
   if( ( id_or_name == c_field_id_Year_Created ) || ( id_or_name == c_field_name_Year_Created ) )
      display_name = get_module_string( c_field_display_name_Year_Created );

   return display_name;
}

void Meta_Model::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Model::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Model::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Model::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Model::setup_foreign_key( Meta_Permission& o, const string& value )
{
   static_cast< Meta_Permission& >( o ).set_key( value );
}

void Meta_Model::setup_foreign_key( Meta_Workgroup& o, const string& value )
{
   static_cast< Meta_Workgroup& >( o ).set_key( value );
}

void Meta_Model::setup_graph_parent( Meta_Class& o, const string& foreign_key_field )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Model::setup_graph_parent( Meta_List& o, const string& foreign_key_field )
{
   static_cast< Meta_List& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Model::setup_graph_parent( Meta_Module& o, const string& foreign_key_field )
{
   static_cast< Meta_Module& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Model::setup_graph_parent( Meta_Package_Option& o, const string& foreign_key_field )
{
   static_cast< Meta_Package_Option& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Model::setup_graph_parent( Meta_Package& o, const string& foreign_key_field )
{
   static_cast< Meta_Package& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Model::setup_graph_parent( Meta_Relationship& o, const string& foreign_key_field )
{
   static_cast< Meta_Relationship& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Model::setup_graph_parent( Meta_Specification& o, const string& foreign_key_field )
{
   static_cast< Meta_Specification& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Model::setup_graph_parent( Meta_View& o, const string& foreign_key_field )
{
   static_cast< Meta_View& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Model::setup_graph_parent(
 Meta_Permission& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Permission& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Permission& >( o ).set_key( init_value );
}

void Meta_Model::setup_graph_parent(
 Meta_Workgroup& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Workgroup& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Workgroup& >( o ).set_key( init_value );
}

size_t Meta_Model::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Model::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Model::get_num_foreign_key_children( bool is_internal ) const
{
   size_t rc = 9;

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

class_base* Meta_Model::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   if( child_num >= 9 )
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
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "300610";
            p_class_base = &child_Class_Source( );
         }
         break;

         case 1:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "300600";
            p_class_base = &child_Class( );
         }
         break;

         case 2:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301990";
            p_class_base = &child_List( );
         }
         break;

         case 3:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302200";
            p_class_base = &child_Module( );
         }
         break;

         case 4:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302825";
            p_class_base = &child_Package_Option( );
         }
         break;

         case 5:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302800";
            p_class_base = &child_Package( );
         }
         break;

         case 6:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301300";
            p_class_base = &child_Relationship( );
         }
         break;

         case 7:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301400";
            p_class_base = &child_Specification( );
         }
         break;

         case 8:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301800";
            p_class_base = &child_View( );
         }
         break;
      }
   }

   return p_class_base;
}

void Meta_Model::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_Model::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_Model::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_Model::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_Model::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_Model::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_Model::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_Model::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "model";

   return get_module_string( key );
}

string Meta_Model::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_Model::get_create_instance_info( ) const
{
   return "";
}

string Meta_Model::get_update_instance_info( ) const
{
   return "";
}

string Meta_Model::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_Model::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );
   else if( procedure_id == "105420" ) // i.e. Create_Module
      retval = "";
   else if( procedure_id == "105410" ) // i.e. Generate
      retval = "";
   else if( procedure_id == "105435" ) // i.e. Generate_File_Links
      retval = "";
   else if( procedure_id == "105440" ) // i.e. Get_Acyclic_Class_List
      retval = "";
   else if( procedure_id == "105450" ) // i.e. Remove_All_Packages
      retval = "";
   else if( procedure_id == "105430" ) // i.e. Remove_Module
      retval = "";

   return retval;
}

bool Meta_Model::get_is_alias( ) const
{
   return false;
}

void Meta_Model::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Model::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Model::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == "_300610" ) || ( sub_context == "child_Class_Source" ) )
      p_class_base = &child_Class_Source( );
   else if( ( sub_context == "_300600" ) || ( sub_context == "child_Class" ) )
      p_class_base = &child_Class( );
   else if( ( sub_context == "_301990" ) || ( sub_context == "child_List" ) )
      p_class_base = &child_List( );
   else if( ( sub_context == "_302200" ) || ( sub_context == "child_Module" ) )
      p_class_base = &child_Module( );
   else if( ( sub_context == "_302825" ) || ( sub_context == "child_Package_Option" ) )
      p_class_base = &child_Package_Option( );
   else if( ( sub_context == "_302800" ) || ( sub_context == "child_Package" ) )
      p_class_base = &child_Package( );
   else if( ( sub_context == "_301300" ) || ( sub_context == "child_Relationship" ) )
      p_class_base = &child_Relationship( );
   else if( ( sub_context == "_301400" ) || ( sub_context == "child_Specification" ) )
      p_class_base = &child_Specification( );
   else if( ( sub_context == "_301800" ) || ( sub_context == "child_View" ) )
      p_class_base = &child_View( );
   else if( ( sub_context == c_field_id_Permission ) || ( sub_context == c_field_name_Permission ) )
      p_class_base = &Permission( );
   else if( ( sub_context == c_field_id_Workgroup ) || ( sub_context == c_field_name_Workgroup ) )
      p_class_base = &Workgroup( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Model::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Allow_Anonymous_Access" );
   names.push_back( "C_Commands_File" );
   names.push_back( "C_Created" );
   names.push_back( "C_Home_Title" );
   names.push_back( "C_Id" );
   names.push_back( "C_Name" );
   names.push_back( "C_Next_Class_Id" );
   names.push_back( "C_Next_List_Id" );
   names.push_back( "C_Next_Specification_Id" );
   names.push_back( "C_Next_View_Id" );
   names.push_back( "C_Permission" );
   names.push_back( "C_Source_File" );
   names.push_back( "C_Type" );
   names.push_back( "C_Use_Package_Demo_Data" );
   names.push_back( "C_Version" );
   names.push_back( "C_Workgroup" );
   names.push_back( "C_Year_Created" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Model::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( to_string( Allow_Anonymous_Access( ) ) );
   values.push_back( sql_quote( to_string( Commands_File( ) ) ) );
   values.push_back( to_string( Created( ) ) );
   values.push_back( sql_quote( to_string( Home_Title( ) ) ) );
   values.push_back( sql_quote( to_string( Id( ) ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( sql_quote( to_string( Next_Class_Id( ) ) ) );
   values.push_back( sql_quote( to_string( Next_List_Id( ) ) ) );
   values.push_back( sql_quote( to_string( Next_Specification_Id( ) ) ) );
   values.push_back( sql_quote( to_string( Next_View_Id( ) ) ) );
   values.push_back( sql_quote( to_string( Permission( ) ) ) );
   values.push_back( sql_quote( to_string( Source_File( ) ) ) );
   values.push_back( to_string( Type( ) ) );
   values.push_back( to_string( Use_Package_Demo_Data( ) ) );
   values.push_back( sql_quote( to_string( Version( ) ) ) );
   values.push_back( sql_quote( to_string( Workgroup( ) ) ) );
   values.push_back( to_string( Year_Created( ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Model::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Model::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start modifier_field_value)] 600014
   dependents.insert( "Type" ); // (for Is_Non_Traditional modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600014

   // [(start modifier_field_null)] 610014
   dependents.insert( "Workgroup" ); // (for Standard_Package_Installed modifier)

   if( ( use_transients && is_field_transient( e_field_id_Workgroup ) )
    || ( !use_transients && !is_field_transient( e_field_id_Workgroup ) ) )
      names.insert( "Workgroup" );
   // [(finish modifier_field_null)] 610014

   // [<start get_always_required_field_names>]
//nyi
   dependents.insert( "Created" );

   if( ( use_transients && is_field_transient( e_field_id_Created ) )
    || ( !use_transients && !is_field_transient( e_field_id_Created ) ) )
      names.insert( "Created" );

   dependents.insert( "Status" );

   if( ( use_transients && is_field_transient( e_field_id_Status ) )
    || ( !use_transients && !is_field_transient( e_field_id_Status ) ) )
      names.insert( "Status" );
   // [<finish get_always_required_field_names>]
}

void Meta_Model::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Model::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_Model::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Model::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Model::static_lock_class_id( )
{
   return "105100";
}

const char* Meta_Model::static_check_class_name( )
{
   return "Model";
}

const char* Meta_Model::static_persistence_extra( )
{
   return "";
}

bool Meta_Model::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Model::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.105100" );
}

void Meta_Model::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "105109", "Actions", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "105112", "Add_Packages", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "105116", "Allow_Anonymous_Access", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "105111", "Commands_File", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "105113", "Created", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "105117", "Home_Title", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "105104", "Id", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "105101", "Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "105105", "Next_Class_Id", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "105107", "Next_List_Id", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "105106", "Next_Specification_Id", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "105108", "Next_View_Id", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "301600", "Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "105110", "Source_File", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "105114", "Status", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "105118", "Type", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "105115", "Use_Package_Demo_Data", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "105102", "Version", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "300500", "Workgroup", "Meta_Workgroup", true, "", "" ) );
   all_field_info.push_back( field_info( "105103", "Year_Created", "int", false, "", "" ) );
}

void Meta_Model::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Permission, make_pair( "Meta.105100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Workgroup, make_pair( "Meta.105100", "Meta_Workgroup" ) ) );
}

int Meta_Model::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Model::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_Model::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Model::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "105109";
      break;

      case 2:
      p_id = "105112";
      break;

      case 3:
      p_id = "105116";
      break;

      case 4:
      p_id = "105111";
      break;

      case 5:
      p_id = "105113";
      break;

      case 6:
      p_id = "105117";
      break;

      case 7:
      p_id = "105104";
      break;

      case 8:
      p_id = "105101";
      break;

      case 9:
      p_id = "105105";
      break;

      case 10:
      p_id = "105107";
      break;

      case 11:
      p_id = "105106";
      break;

      case 12:
      p_id = "105108";
      break;

      case 13:
      p_id = "301600";
      break;

      case 14:
      p_id = "105110";
      break;

      case 15:
      p_id = "105114";
      break;

      case 16:
      p_id = "105118";
      break;

      case 17:
      p_id = "105115";
      break;

      case 18:
      p_id = "105102";
      break;

      case 19:
      p_id = "300500";
      break;

      case 20:
      p_id = "105103";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Model" );

   return p_id;
}

const char* Meta_Model::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Actions";
      break;

      case 2:
      p_id = "Add_Packages";
      break;

      case 3:
      p_id = "Allow_Anonymous_Access";
      break;

      case 4:
      p_id = "Commands_File";
      break;

      case 5:
      p_id = "Created";
      break;

      case 6:
      p_id = "Home_Title";
      break;

      case 7:
      p_id = "Id";
      break;

      case 8:
      p_id = "Name";
      break;

      case 9:
      p_id = "Next_Class_Id";
      break;

      case 10:
      p_id = "Next_List_Id";
      break;

      case 11:
      p_id = "Next_Specification_Id";
      break;

      case 12:
      p_id = "Next_View_Id";
      break;

      case 13:
      p_id = "Permission";
      break;

      case 14:
      p_id = "Source_File";
      break;

      case 15:
      p_id = "Status";
      break;

      case 16:
      p_id = "Type";
      break;

      case 17:
      p_id = "Use_Package_Demo_Data";
      break;

      case 18:
      p_id = "Version";
      break;

      case 19:
      p_id = "Workgroup";
      break;

      case 20:
      p_id = "Year_Created";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Model" );

   return p_id;
}

int Meta_Model::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( ( field == c_field_id_Actions ) || ( field == c_field_name_Actions ) )
      rc += 1;
   else if( ( field == c_field_id_Add_Packages ) || ( field == c_field_name_Add_Packages ) )
      rc += 2;
   else if( ( field == c_field_id_Allow_Anonymous_Access ) || ( field == c_field_name_Allow_Anonymous_Access ) )
      rc += 3;
   else if( ( field == c_field_id_Commands_File ) || ( field == c_field_name_Commands_File ) )
      rc += 4;
   else if( ( field == c_field_id_Created ) || ( field == c_field_name_Created ) )
      rc += 5;
   else if( ( field == c_field_id_Home_Title ) || ( field == c_field_name_Home_Title ) )
      rc += 6;
   else if( ( field == c_field_id_Id ) || ( field == c_field_name_Id ) )
      rc += 7;
   else if( ( field == c_field_id_Name ) || ( field == c_field_name_Name ) )
      rc += 8;
   else if( ( field == c_field_id_Next_Class_Id ) || ( field == c_field_name_Next_Class_Id ) )
      rc += 9;
   else if( ( field == c_field_id_Next_List_Id ) || ( field == c_field_name_Next_List_Id ) )
      rc += 10;
   else if( ( field == c_field_id_Next_Specification_Id ) || ( field == c_field_name_Next_Specification_Id ) )
      rc += 11;
   else if( ( field == c_field_id_Next_View_Id ) || ( field == c_field_name_Next_View_Id ) )
      rc += 12;
   else if( ( field == c_field_id_Permission ) || ( field == c_field_name_Permission ) )
      rc += 13;
   else if( ( field == c_field_id_Source_File ) || ( field == c_field_name_Source_File ) )
      rc += 14;
   else if( ( field == c_field_id_Status ) || ( field == c_field_name_Status ) )
      rc += 15;
   else if( ( field == c_field_id_Type ) || ( field == c_field_name_Type ) )
      rc += 16;
   else if( ( field == c_field_id_Use_Package_Demo_Data ) || ( field == c_field_name_Use_Package_Demo_Data ) )
      rc += 17;
   else if( ( field == c_field_id_Version ) || ( field == c_field_name_Version ) )
      rc += 18;
   else if( ( field == c_field_id_Workgroup ) || ( field == c_field_name_Workgroup ) )
      rc += 19;
   else if( ( field == c_field_id_Year_Created ) || ( field == c_field_name_Year_Created ) )
      rc += 20;

   return rc - 1;
}

procedure_info_container& Meta_Model::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "105420", procedure_info( "Create_Module" ) ) );
      procedures.insert( make_pair( "105410", procedure_info( "Generate" ) ) );
      procedures.insert( make_pair( "105435", procedure_info( "Generate_File_Links" ) ) );
      procedures.insert( make_pair( "105440", procedure_info( "Get_Acyclic_Class_List" ) ) );
      procedures.insert( make_pair( "105450", procedure_info( "Remove_All_Packages" ) ) );
      procedures.insert( make_pair( "105430", procedure_info( "Remove_Module" ) ) );
   }

   return procedures;
}

string Meta_Model::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(75),"
    "C_Ver_ SMALLINT UNSIGNED NOT NULL,"
    "C_Rev_ BIGINT UNSIGNED NOT NULL,"
    "C_Sec_ BIGINT UNSIGNED NOT NULL,"
    "C_Typ_ VARCHAR(16) NOT NULL,"
    "C_Allow_Anonymous_Access INTEGER NOT NULL,"
    "C_Commands_File VARCHAR(200) NOT NULL,"
    "C_Created INTEGER NOT NULL,"
    "C_Home_Title VARCHAR(200) NOT NULL,"
    "C_Id VARCHAR(200) NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Next_Class_Id VARCHAR(200) NOT NULL,"
    "C_Next_List_Id VARCHAR(200) NOT NULL,"
    "C_Next_Specification_Id VARCHAR(200) NOT NULL,"
    "C_Next_View_Id VARCHAR(200) NOT NULL,"
    "C_Permission VARCHAR(75) NOT NULL,"
    "C_Source_File VARCHAR(200) NOT NULL,"
    "C_Type INTEGER NOT NULL,"
    "C_Use_Package_Demo_Data INTEGER NOT NULL,"
    "C_Version VARCHAR(200) NOT NULL,"
    "C_Workgroup VARCHAR(75) NOT NULL,"
    "C_Year_Created INTEGER NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Model::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Model::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "enum_add_packages_0", get_enum_string_add_packages( 0 ) ) );
   pairs.push_back( make_pair( "enum_add_packages_1", get_enum_string_add_packages( 1 ) ) );
   pairs.push_back( make_pair( "enum_add_packages_2", get_enum_string_add_packages( 2 ) ) );
   pairs.push_back( make_pair( "enum_add_packages_3", get_enum_string_add_packages( 3 ) ) );

   pairs.push_back( make_pair( "enum_model_type_0", get_enum_string_model_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_model_type_1", get_enum_string_model_type( 1 ) ) );
}

void Meta_Model::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "Workgroup,Id", "string,string" ) );
   pairs.push_back( make_pair( "Workgroup,Name", "string,string" ) );
}

void Meta_Model::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "Workgroup,Id" );
   unique_indexes.push_back( "Workgroup,Name" );
}

void Meta_Model::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Workgroup,C_Id" );
   indexes.push_back( "C_Workgroup,C_Name" );
}

void Meta_Model::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Workgroup,C_Id" );
   indexes.push_back( "C_Workgroup,C_Name" );
}

void Meta_Model::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Model::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Model::static_insert_external_alias( const string& module_and_class_id, Meta_Model* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Model::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Model::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   g_add_packages_enum.insert( 0 );
   g_add_packages_enum.insert( 1 );
   g_add_packages_enum.insert( 2 );
   g_add_packages_enum.insert( 3 );

   g_model_type_enum.insert( 0 );
   g_model_type_enum.insert( 1 );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Model::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
